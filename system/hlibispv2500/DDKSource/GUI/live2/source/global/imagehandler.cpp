#include "imagehandler.hpp"
#include "paramsocket/paramsocket.hpp"
#include "img_errors.h"
#include "felixcommon/pixel_format.h"
#include "sim_image.h"
#include "savefile.h"
#include "imageUtils.h"
#include "ispc/Shot.h"

#ifndef WIN32
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <qrgb.h>
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#define N_TRIES 5
#define FPS_MAX_FRAMES 10

IMG_RESULT VisionLive::Save::open(SaveType _type, ePxlFormat fmt, MOSAICType mos, int width, int height, const std::string &_filename)
{
	int ret = IMG_ERROR_ALREADY_INITIALISED;
    if (!file)
    {
        //printf("create SaveFile\n");
        type = _type;
        filename = _filename;
        file = (SaveFile*)IMG_CALLOC(1, sizeof(SaveFile));
        if(!file)
        {
            //printf("failed to allocate SaveFile object\n");
            return IMG_ERROR_MALLOC_FAILED;
        }
        SaveFile_init(file);

        switch(type)
        {
        case Bayer:
            ret = openBayer(fmt, mos, width, height);
            break;

        /*case Bayer_TIFF:
            ret = openTiff(glb->raw2DExtractionType,
                context.getSensor()->eBayerFormat,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;*/

        case YUV:
        case Bytes:
            ret = openBytes();
            break;

        case RGB:
            ret = openRGB(fmt, width, height);
            break;

        /*case RGB_EXT:
            ret = openRGB(glb->hdrExtractionType,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;

        case RGB_INS:
            ret = openRGB(glb->hdrInsertionType,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;*/

        default:
            ret = IMG_ERROR_NOT_INITIALISED;
        }

        if (IMG_SUCCESS != ret)
        {
            SaveFile_destroy(file);
            delete file;
            file = 0;
        }
    }
    return ret;
}

//
// Public Functions
//

VisionLive::ImageHandler::ImageHandler(QThread *parent): QThread(parent)
{
	_continueRun = false;

	_pSocket = NULL;
	_pDPFOutMap = NULL;
	_nDefects = 0;

	_FPS_framesReceived = 0;
	_FPS = 0;

	_port = 2346;
	_clientName = QString();
}

VisionLive::ImageHandler::~ImageHandler()
{
	if(_pSocket)
	{
		delete _pSocket;
	}

	if(_pDPFOutMap)
	{
		free(_pDPFOutMap);
	}
}

//
// Protected Functions
//

void VisionLive::ImageHandler::run()
{
    _continueRun = true;
	if(startConnection() != IMG_SUCCESS)
	{
		return;
	}
    _continueRun = false;
    emit connected();

    while (!_continueRun) { yieldCurrentThread(); }
	emit logMessage(tr("Running..."), tr("ImageHandler::run()"));

	_FPS_Timer.start();

	while(_continueRun)
	{
		if(!_cmds.isEmpty())
		{
			GUIParamCMD cmd = popCommand();

			switch(cmd)
			{
			case GUICMD_QUIT:
				emit logMessage("Executing GUICMD_QUIT", tr("ImageHandler::run()"));
				if(setQuit(_pSocket) != IMG_SUCCESS)
				{
					emit logError("setQuit() failed!", tr("ImageHandler::run()"));
				}
				_continueRun = false;
				break;
			case GUICMD_SET_LF_ENABLE:
				emit logMessage("Executing GUICMD_SET_LF_ENABLE", tr("ImageHandler::run()"));
				if(setImageSending(_pSocket) != IMG_SUCCESS)
				{
					emit logError("setImageSending() failed!", tr("ImageHandler::run()"));
					_continueRun = false;
				}
				break;
			case GUICMD_SET_IMAGE_RECORD:
				emit logMessage("Executing GUICMD_SET_IMAGE_RECORD", tr("ImageHandler::run()"));
				if(record(_pSocket) != IMG_SUCCESS)
				{
					emit logError("record() failed!", tr("ImageHandler::run()"));
					_continueRun = false;
				}
				break;
			default:
				emit logError("Unrecognized command!", tr("ImageHandler::run()"));
				_continueRun = false;
			}
		}

		if(_continueRun)
		{
			//emit logMessage("Executing GUICMD_GET_IMAGE", tr("ImageHandler::run()"));
			if(getImage(_pSocket) != IMG_SUCCESS) 
			{
				emit logError("getImage() failed!", tr("ImageHandler::run()"));
				_continueRun = false;
			}

			// Calculate FPS
			_FPS_framesReceived++;
			if(_FPS_framesReceived == FPS_MAX_FRAMES)
			{
				_FPS = (double)_FPS_framesReceived/(double)((double)_FPS_Timer.elapsed()/(double)1000);
				_FPS_framesReceived = 0;
				_FPS_Timer.restart();
				emit fpsInfoReceived(_FPS);
			}
		}

		yieldCurrentThread();
	}

	emit logMessage(tr("Stopped"), tr("ImageHandler::run()"));
	emit disconnected();
}

//
// Private Functions
//

void VisionLive::ImageHandler::pushCommand(GUIParamCMD cmd)
{
	_lock.lock();
	_cmds.push_back(cmd);
	_lock.unlock();
}

GUIParamCMD VisionLive::ImageHandler::popCommand()
{
	GUIParamCMD retCmd;

	_lock.lock();
	retCmd = _cmds[0];
	_cmds.pop_front();
	_lock.unlock();

	return retCmd;
}

int VisionLive::ImageHandler::startConnection()
{
	int ret = 0;

	_pSocket = new ParamSocketServer(_port);

    while(_continueRun)
    {
		if((ret = _pSocket->wait_for_client("VisionLive2.0", 1)) == IMG_SUCCESS)
		{
			// Connected
			emit logMessage(tr("Connected"), tr("ImageHandler::startConnection()"));
			_clientName = QString::fromStdString(_pSocket->clientName());
			break;
		}
		else if(ret == -3)
		{
			// Timeout
			continue;
		}
		else
		{
			// Error
			emit logError(tr("Failed to connect!"), tr("ImageHandler::startConnection()"));
			break;
		}
    }

	if(!_continueRun)
	{
		emit logMessage(tr("Connecting terminated"), tr("ImageHandler::startConnection()"));
		emit connectionCanceled();
		return IMG_ERROR_FATAL;
	}
	else
	{
		emit logMessage(tr("Connecting stopped"), tr("ImageHandler::startConnection()"));
	}

	return ret;
}

int VisionLive::ImageHandler::setQuit(ParamSocket *pSocket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_QUIT);
	if(pSocket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "ImageHandler::setQuit()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::ImageHandler::getImage(ParamSocket *pSocket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;
	size_t nRead;
    size_t nToRead;
	size_t nread = 0;

    IMG_UINT16 width = 0;
	IMG_UINT16 stride = 0;
	IMG_UINT16 height = 0;
    ePxlFormat fmt = PXL_NONE;
    eMOSAIC mosaic = MOSAIC_NONE;
	double xScaler = 1.0f;
	double yScaler = 1.0f;

	bool sendIMG = true;

	_imgLock.lock();
	ImageConfig currConf;
	currConf = _imgConf;
	_imgLock.unlock();

	// Request image from FPGA
	cmd[0] = htonl(GUICMD_GET_IMAGE);
	cmd[1] = htonl(currConf.type);
	if(pSocket->write(&cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		emit logError("Failed to write to socket!", tr("ImageHandler::getImage()"));
		return IMG_ERROR_FATAL;
	}

	// Receive imageInfo from FPGA
	if(pSocket->read(&cmd, 10*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		emit logError("Failed to read from socket!", tr("ImageHandler::getImage()"));
		return IMG_ERROR_FATAL;
	}

	if(ntohl(cmd[0]) != GUICMD_SET_IMAGE)
	{
		emit logError("Unexpected command received!", tr("ImageHandler::getImage()"));
		return IMG_ERROR_FATAL;
	}

    ImageType reqImgType = (ImageType)ntohl(cmd[1]);
	width = ntohl(cmd[2]);
	height = ntohl(cmd[3]);
	stride = ntohl(cmd[4]);
	fmt = (ePxlFormat)ntohl(cmd[5]);
	mosaic = (MOSAICType)ntohl(cmd[6]);
	sendIMG = ntohl(cmd[7]);
	xScaler = (double)ntohl(cmd[8])/(double)1000.0f;
	yScaler = (double)ntohl(cmd[9])/(double)1000.0f;

	/*emit logMessage(tr("Image info ") + QString::number(width) + "x" + QString::number(height) + 
		" (str " + QString::number(stride) + ") fmt " + QString::number((int)fmt) + "-" + QString::number((int)mosaic), tr("ImageHandler::getImage()"));*/

	if(width == 0 || height == 0)
	{
		emit logWarning("No image data generated!", tr("ImageHandler::getImage()"));
		return IMG_SUCCESS;
	}

	if(sendIMG)
	{
		PIXELTYPE sYUVType;
		bool bYUV = false;

		bYUV = PixelTransformYUV(&sYUVType, fmt) == IMG_SUCCESS;

		// sendImage() sends a stride*height image, so we compute the image size and divide by stride to have the height
		if((ePxlFormat)fmt == YVU_420_PL12_10 || (ePxlFormat)fmt == YUV_420_PL12_10 || (ePxlFormat)fmt == YVU_422_PL12_10 || (ePxlFormat)fmt == YUV_422_PL12_10)
		{
			unsigned bop_line = width/sYUVType.ui8PackedElements;
			if(width%sYUVType.ui8PackedElements)
			{
				bop_line++;
			}
			bop_line = bop_line*sYUVType.ui8PackedStride;

			nToRead = bop_line*height + 2*bop_line/sYUVType.ui8HSubsampling*height/sYUVType.ui8VSubsampling;
			stride = bop_line;
		}
		else
		{
			if(bYUV)
			{
				nToRead = width*height + 2*width/sYUVType.ui8HSubsampling*height/sYUVType.ui8VSubsampling;
				stride = width; // sent YUV has removed stride
			}
			else
			{
				nToRead = stride*height;
			}
		}

		IMG_UINT8 *data = (IMG_UINT8 *)malloc(nToRead);

		size_t toRead = nToRead;
		size_t w = 0;
		size_t maxChunk = PARAMSOCK_MAX;

		emit notifyStatus(QString(tr("Downloading image... ")));

		while(w < toRead)
		{
			if(pSocket->read(((IMG_UINT8*)data)+w, (toRead-w > maxChunk)? maxChunk : toRead-w, nRead, -1) != IMG_SUCCESS)
			{
				emit logError("Failed to write to socket!", tr("ImageHandler::getImage()"));
				return IMG_ERROR_FATAL;
			}
			//if(w%10000 == 0) printf("image read %ld/%ld\n", w, toRead);
			w += nRead;
		}

		emit notifyStatus(QString(tr("Processing image... ")));

		_ImageData = generateImageData(data, width, height, stride, fmt, mosaic);

		free(data);
	}

	if(_ImageData.valid)
	{
		if(sendIMG)
		{
			emit notifyStatus(QString(tr("Converting image... ")));
			QImage *pImage;

			if(currConf.setCorrectGamma)
			{
				pImage = _ImageData.gammaCorrect().convertToQImage();
			}
			else
			{
				pImage = _ImageData.convertToQImage();
			}

			if(currConf.generateHistogram)
			{
				emit notifyStatus(QString(tr("Generating histograms... ")));
				generateHistogram(_ImageData.bitdepth);
			}

			if(currConf.generatePixelInfo)
			{
				emit notifyStatus(QString(tr("Generating pixelInfo... ")));
				generatePixelInfo(currConf.marker);
			}

			if(currConf.generateAreaStatistics)
			{
				emit notifyStatus(QString(tr("Generating areaStatistics... ")));
				generateAreaStatistics(_ImageData);
			}

			if(currConf.generateVectorscope)
			{
				emit notifyStatus(QString(tr("Generating vectorscope... ")));
				QMap<int, std::pair<int, QRgb> > vectorScopeData = VisionLive::Algorithms::generateVectorScope(pImage, false);
				emit vectorscopeReceived(vectorScopeData);
			}

			if(currConf.generateLineView)
			{
				emit notifyStatus(QString(tr("Generating lineView... ")));
				QMap<QString, std::vector<std::vector<int> > > lineViewData = VisionLive::Algorithms::generateLineView(pImage, currConf.marker.toPoint(), 3);
				emit lineViewReceived(currConf.marker.toPoint(), lineViewData, 3);
			}

			if(currConf.markDPFPoints)
			{
				emit notifyStatus(QString(tr("Marking DPF points... ")));
				DPF_getMap(_pSocket);
				markDPFPoints(pImage, xScaler, yScaler);
			}

			emit notifyStatus(QString(tr("Done ")));
			emit imageReceived(pImage, (int)reqImgType, (int)fmt, (int)mosaic, (int)width, (int)height);
		}
	}

	return IMG_SUCCESS;
}

int VisionLive::ImageHandler::record(ParamSocket *socket)
{
	_recLock.lock();
	QString captureName = _recConf.name;
	int nFrameToCapture = _recConf.nFrames;
	ImageType imgType = _recConf.type;
	_recLock.unlock();

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;
	size_t nRead;
	size_t nToRead;

	cmd[0] = htonl(GUICMD_SET_IMAGE_RECORD);
	cmd[1] = htonl(nFrameToCapture);
	cmd[2] = htonl(imgType);
	if(socket->write(&cmd, 3*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
		emit logError("Failed to write to socket!", "ImageHandler::record()");
		return IMG_ERROR_FATAL;
    }

	IMG_UINT16 width = 0, stride = 0, height = 0;
    ePxlFormat fmt = PXL_NONE;
    eMOSAIC mosaic = MOSAIC_NONE;

	bool bBayer = false;
    bool bYUV = false;
    size_t nread = 0;
    bool toRet = true;

	double xScaler = 0;
	double yScaler = 0;

	double cos = 0;

    PIXELTYPE sYUVType;

	// Receive imageInfo from FPGA
	if(socket->read(&cmd, 7*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		emit logError("Failed to write to socket!", "ImageHandler::record()");
		return IMG_ERROR_FATAL;
	}

	if(ntohl(cmd[0]) != GUICMD_SET_IMAGE)
	{
		emit logError("Unexpected command received!", "ImageHandler::record()");
		return IMG_ERROR_FATAL;
	}

	ImageType recImgType = (ImageType)ntohl(cmd[1]);
	width = ntohl(cmd[2]);
	height = ntohl(cmd[3]);
	stride = ntohl(cmd[4]);
	fmt = (ePxlFormat)ntohl(cmd[5]);
	mosaic = (MOSAICType)ntohl(cmd[6]);


	// Receive number of parameters to read
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "CommandHandler::record()");
		return IMG_ERROR_FATAL;
    }
	int nParams = ntohl(cmd[0]); 

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Receive all parameters
	QMap<QString, QString> params;
	for(int i = 0; i < nParams; i++)
	{
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::record()");
			return IMG_ERROR_FATAL;
		}
		int paramLength = ntohl(cmd[0]);
		if(socket->read(buff, paramLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "CommandHandler::record()");
			return IMG_ERROR_FATAL;
		}
		buff[nRead] = '\0';
		std::string param = std::string(buff);
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		QStringList words = ((QString::fromLatin1(param.c_str())).remove(QRegExp("[\n\t\r]"))).split(" ", QString::SkipEmptyParts);
		if(words.size() >= 2)
		{
			QString p;
			for(int i = 1; i < words.size(); i++)
			{
				p = p + words[i] + " ";
			}
			params.insert(words[0], p);
		}
		else
		{
			emit logError(tr("AWB parameter lacks name or value!"), "CommandHandler::record()");
			return IMG_ERROR_FATAL;
		}
	}
	xScaler = params.find("xScaler").value().toDouble();
	yScaler = params.find("yScaler").value().toDouble();


	emit logMessage(tr("Image info ") + QString::number(width) + "x" + QString::number(height) + 
		" (str " + QString::number(stride) + ") fmt " + QString::number((int)fmt) + "-" + QString::number((int)mosaic), tr("ImageHandler::record()"));

	bYUV = PixelTransformYUV(&sYUVType, fmt) == IMG_SUCCESS;

	unsigned bop_line = width/sYUVType.ui8PackedElements;
	if(width%sYUVType.ui8PackedElements)
	{
		bop_line++;
	}
	bop_line = bop_line*sYUVType.ui8PackedStride;

	if((ePxlFormat)fmt == YVU_420_PL12_10 || (ePxlFormat)fmt == YUV_420_PL12_10 || (ePxlFormat)fmt == YVU_422_PL12_10 || (ePxlFormat)fmt == YUV_422_PL12_10)
	{
		nToRead = bop_line*height + 2*bop_line/sYUVType.ui8HSubsampling*height/sYUVType.ui8VSubsampling;
		stride = bop_line;
	}
	else
	{
		if(bYUV)
		{
			nToRead = width*height + 2*width/sYUVType.ui8HSubsampling*height/sYUVType.ui8VSubsampling;
			stride = width; // sent YUV has removed stride
		}
		else
		{
			nToRead = stride*height;
		}
	}

	RecData *newRecData = new RecData();
	newRecData->name = captureName;
	newRecData->nFrames = nFrameToCapture;
	newRecData->frameSize = nToRead;
	newRecData->type = recImgType;
	newRecData->frameTimeStamp.clear();
	newRecData->frameData.clear();

	IMG_UINT8 *data = 0;
	data = (IMG_UINT8 *)malloc(nToRead);

	size_t toRead = nToRead;
	size_t w = 0;
	size_t maxChunk = PARAMSOCK_MAX;

	for(int i = 0; i < nFrameToCapture; i++)
	{
		// Receive frame timestamp
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError("Failed to read from socket!", "ImageHandler::record()");
			return IMG_ERROR_FATAL;
		}
		newRecData->frameTimeStamp.push_back(ntohl(cmd[0]));

		toRead = nToRead;
		w = 0;
		//memset(data, 0, sizeof(IMG_UINT8)*nToRead);

		while(w < toRead)
		{
			if(socket->read(((IMG_UINT8*)data)+w, (toRead-w > maxChunk)? maxChunk : toRead-w, nRead, -1) != IMG_SUCCESS)
			{
				emit logError("Failed to write to socket!", "ImageHandler::record()");
				return IMG_ERROR_FATAL;
			}
			//if(w%1000000 == 0)printf("Image read %ld/%ld\n", w, toRead);
			w += nRead;
		}

		newRecData->frameData.push_back(generateImageData(data, width, height, stride, fmt, mosaic));

		// Save original data
		newRecData->frameData[i].origData.data = (IMG_UINT8 *)malloc(nToRead);
		memcpy((IMG_UINT8 *)newRecData->frameData[i].origData.data, data, nToRead);
		newRecData->frameData[i].origData.height = height;
		newRecData->frameData[i].origData.width = width;
		newRecData->frameData[i].origData.pxlFormat = fmt;
		newRecData->frameData[i].origData.stride = stride;
		newRecData->frameData[i].origData.isTiled = false;
		newRecData->frameData[i].origData.offset = 0;
		newRecData->frameData[i].origData.offsetCbCr = bop_line*height;
		newRecData->frameData[i].origData.strideCbCr = 2*(bop_line/sYUVType.ui8HSubsampling);

		// Get DPF map
		newRecData->xScaler = xScaler;
		newRecData->yScaler = yScaler;

		// Receive number of defects
		if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError("Failed to write to socket!", "ImageHandler::record()");
			return IMG_ERROR_FATAL;
		}
		newRecData->nDefects.push_back(ntohl(cmd[0]));

		// Receive wright map
		newRecData->DPFMaps.push_back(NULL);
		newRecData->DPFMaps[newRecData->DPFMaps.size()-1] = (IMG_UINT8 *)malloc(ntohl(cmd[0])*sizeof(IMG_UINT64));

		toRead = ntohl(cmd[0])*sizeof(IMG_UINT64);
		w = 0;
		maxChunk = PARAMSOCK_MAX;

		// Receive DPF output map
		while(w < toRead)
		{
			if(socket->read(((IMG_UINT8*)newRecData->DPFMaps[newRecData->DPFMaps.size()-1])+w, (toRead-w > maxChunk)? maxChunk : toRead-w, nRead, -1) != IMG_SUCCESS)
			{
				emit logError(tr("Failed to write to socket!"), "ImageHandler::record()");
				return IMG_ERROR_FATAL;
			}
			//if(w%PARAMSOCK_MAX == 0)printf("dpfMap read %ld/%ld\n", w, toRead);
			w += nRead;
		}

		// Generate Histogram
		newRecData->HistDatas.push_back(generateHistogram(newRecData->frameData[i]));

		emit recordingProgress(i+1, nFrameToCapture);
	}

	// Get ParameterList
	receiveConfiguration(_pSocket);
	newRecData->configuration = _receivedConfiguration;

	// Generate Vectorscope
	QImage *image = newRecData->frameData[0].convertToQImage();
	newRecData->vectorScopeData = VisionLive::Algorithms::generateVectorScope(image, false);
	delete image;

	free(data);

	_recLock.lock();
	_recData.push_back(newRecData);
	_recLock.unlock();

	emit movieReceived(newRecData);

	return IMG_SUCCESS;
}

VisionLive::ImageData VisionLive::ImageHandler::generateImageData(IMG_UINT8 *rawData, int width, int height, int stride, pxlFormat fmt, MOSAICType mosaic)
{
	ImageData ret;

	bool bBayer = false;
    bool bYUV = false;

	PIXELTYPE sYUVType;

	bYUV = PixelTransformYUV(&sYUVType, fmt) == IMG_SUCCESS;

	sSimImageOut imageOut;
	SimImageOut_init(&imageOut);

	imageOut.info.ui32Width = width;
    imageOut.info.ui32Height = height;
    imageOut.info.stride = stride;
    imageOut.info.ui8BitDepth = 0;

    switch(fmt)
    {
	case RGB_888_24:
        imageOut.info.eColourModel = SimImage_RGB24;
		imageOut.info.ui8BitDepth = 8;
        imageOut.info.stride = 3*width;
        break;
	case BGR_888_24:
        imageOut.info.eColourModel = SimImage_RGB24;
		imageOut.info.ui8BitDepth = 8;
        imageOut.info.stride = 3*width;
        break;
    case RGB_888_32:
        imageOut.info.eColourModel = SimImage_RGB32;
        imageOut.info.ui8BitDepth = 8;
        imageOut.info.stride = 4*width;
        break;
    case BGR_888_32:
        imageOut.info.eColourModel = SimImage_RGB32;
        imageOut.info.ui8BitDepth = 8;
        imageOut.info.stride = 4*width;
        break;
	case RGB_101010_32:
        imageOut.info.eColourModel = SimImage_RGB32;
        imageOut.info.ui8BitDepth = 10;
        imageOut.info.stride = 4*width;
        break;
	case BGR_101010_32:
        imageOut.info.eColourModel = SimImage_RGB32;
        imageOut.info.ui8BitDepth = 10;
        imageOut.info.stride = 4*width;
        break;
    case BAYER_RGGB_8:
        imageOut.info.eColourModel = SimImage_RGGB;
        imageOut.info.ui8BitDepth = 8;
        imageOut.info.stride = 8*width; // 4*2 bytes per pixel (always saved as u16)
        bBayer = true;
        break;
    case BAYER_RGGB_10:
        imageOut.info.eColourModel = SimImage_RGGB;
        imageOut.info.ui8BitDepth = 10;
        imageOut.info.stride = 8*width; // 4*2 bytes per pixel (always saved as u16)
        bBayer = true;
        break;
    case BAYER_RGGB_12:
        imageOut.info.eColourModel = SimImage_RGGB;
        imageOut.info.ui8BitDepth = 12;
        imageOut.info.stride = 8*width; // 4*2 bytes per pixel (always saved as u16)
        bBayer = true;
        break;
    case YVU_420_PL12_8:
	case YUV_420_PL12_8:
    case YVU_422_PL12_8:
	case YUV_422_PL12_8:
        imageOut.info.ui8BitDepth = 8;
        imageOut.info.stride = width; // 1 byte per pixel
        bYUV = true;
        break;
	case YVU_420_PL12_10:
	case YUV_420_PL12_10:
    case YVU_422_PL12_10:
	case YUV_422_PL12_10:
        imageOut.info.ui8BitDepth = 10;
        imageOut.info.stride = width; // 1 byte per pixel
        bYUV = true;
        break;
    default:
		emit logError("Received an unsupported format!", "ImageHandler::createCVBuffer()");
		return ret;
    }

	ret.bitdepth = imageOut.info.ui8BitDepth;

    switch(mosaic)
    {
    case MOSAIC_GRBG:
        imageOut.info.eColourModel = SimImage_GRBG;
        break;
    case MOSAIC_GBRG:
        imageOut.info.eColourModel = SimImage_GBRG;
        break;
    case MOSAIC_BGGR:
        imageOut.info.eColourModel = SimImage_BGGR;
        break;
    }

    if(imageOut.info.ui8BitDepth > 0)
    {
        // generate an openCV buffer
		ret.mosaic = mosaic;
		ret.pxlFormat = fmt;

        if(bBayer)
        {
            IMG_SIZE outsize;
            IMG_UINT16 outSizes[2] = { width, height };
            void *converted = 0;

            SimImageOut_create(&imageOut);

            convertToPlanarBayer(outSizes, rawData, stride, imageOut.info.ui8BitDepth, &converted, &outsize);

            if(converted)
            {
                if(SimImageOut_addFrame(&imageOut, converted, outsize) != IMG_SUCCESS)
                {
					emit logError("SimImageOut_addFrame() failed!", "ImageHandler::createCVBuffer()");
                }

                IMG_FREE(converted);
            }

            ret.data = FLXBAYER2MatMosaic( *((CImageFlx*)imageOut.saveToFLX) );
        }
        else if(bYUV)
        {
			if(imageOut.info.ui8BitDepth == 8)
			{
				unsigned char *yuv444 = 0;
				if(fmt == YVU_420_PL12_8 || fmt == YVU_422_PL12_8) 
					yuv444 = BufferTransformYUVTo444((IMG_UINT8*)rawData, IMG_FALSE, fmt, width, height, width, height);
				else if(fmt == YUV_420_PL12_8 || fmt == YUV_422_PL12_8)
					yuv444 = BufferTransformYUVTo444((IMG_UINT8*)rawData, IMG_TRUE, fmt, width, height, width, height);
				unsigned char *y444 = yuv444;
				unsigned char *u444 = yuv444 + width*height;
				unsigned char *v444 = yuv444 + 2*width*height;

				std::vector<cv::Mat> buff;

				ret.data = cv::Mat(height, width, CV_8UC3);

				buff.push_back(cv::Mat(height, width, CV_8UC1, y444));
				buff.push_back(cv::Mat(height, width, CV_8UC1, u444));
				buff.push_back(cv::Mat(height, width, CV_8UC1, v444));

				cv::merge(buff, ret.data);

				free(yuv444);
			}
			else if(imageOut.info.ui8BitDepth == 10)
			{
				// Change format to RGB_888_24
				ret.pxlFormat = RGB_888_24;

				cv::Mat yuv444;
				yuv444 = VisionLive::Algorithms::convert_YUV_420_422_10bit_to_YUV444Mat(rawData, width, height, false, fmt);
				ret.data = VisionLive::Algorithms::convert_YUV444Mat_to_RGB888Mat(yuv444, width, height, false, BT709_coeff, BT709_Inputoff); 
			}
        }
        else // RGB
        {
            imageOut.info.isBGR = true;

            SimImageOut_create(&imageOut);
            IMG_UINT8 *converted = 0; // converted buffer with stride removed
            int size = imageOut.info.stride;

            //SimImageOut_open("output_rgb2.flx", &imageOut);

            // remove the stride
            {
                IMG_UINT8* pSave = rawData;
                int i;
                converted = (IMG_UINT8*)malloc(size*height);

                for(i = 0 ; i < height ; i++)
                {
                    memcpy(&(converted[i*size]), pSave, size);
                    pSave+=stride;
                }
            }

            if(SimImageOut_addFrame(&imageOut, converted, size*height) != IMG_SUCCESS)
            {
				emit logMessage("Failed to add rgb frame!", "ImageHandler::createCVBuffer()");
            }

			if(imageOut.info.ui8BitDepth == 8)
			{
				ret.data = FLXRGB2Mat( *((CImageFlx*)imageOut.saveToFLX) );
			}
			else if(imageOut.info.ui8BitDepth == 10)
			{
				ret.data = FLXRGB2Mat_10bit( *((CImageFlx*)imageOut.saveToFLX) );
			}

            free(converted);
        }
    }

	SimImageOut_clean(&imageOut);

	return ret;
}

void VisionLive::ImageHandler::generateHistogram(int bitdepth)
{
	int nChannels = 4;
	nChannels = (_ImageData.mosaic != MOSAIC_NONE)? 4 : 3;

	std::vector<cv::Mat> channelPlains;
	cv::split(_ImageData.data, channelPlains);

    std::vector<cv::Mat> histograms;

	int nElem = 1<<bitdepth;
	int histSize = 2<<bitdepth;
		
	float range[] = { -1*nElem, nElem-1};
    const float *histRange = {range};

	bool uniform = true; 
	bool accumulate = true;

	int intervalWidth = 5;

    for(int c = 0; c < nChannels; c++)
    {
		histograms.push_back(cv::Mat());
        cv::calcHist(&channelPlains[c], 1, 0, cv::Mat(), histograms[c], 1, &histSize, &histRange, uniform, accumulate);
    }

	emit histogramReceived(histograms, nChannels, nElem);
}

VisionLive::HistData VisionLive::ImageHandler::generateHistogram(const ImageData &imgData)
{
	HistData data;

	data.nChannels = 4;
	data.nChannels = (imgData.mosaic != MOSAIC_NONE)? 4 : 3;

	std::vector<cv::Mat> channelPlains;
	cv::split(imgData.data, channelPlains);

	data.nElem = 1<<imgData.bitdepth;
	int histSize = 2<<imgData.bitdepth;
		
	float range[] = { -1*data.nElem, data.nElem-1};
    const float *histRange = {range};

	bool uniform = true; 
	bool accumulate = true;

	int intervalWidth = 5;

    for(int c = 0; c < data.nChannels; c++)
    {
		data.histograms.push_back(cv::Mat());
        cv::calcHist(&channelPlains[c], 1, 0, cv::Mat(), data.histograms[c], 1, &histSize, &histRange, uniform, accumulate);
    }

	return data;
}

void VisionLive::ImageHandler::generatePixelInfo(QPointF pos)
{
	if(pos.toPoint().y() < 0 || pos.toPoint().x() < 0 || pos.toPoint().x() >= _ImageData.data.cols || pos.toPoint().y() >= _ImageData.data.rows)
	{
		emit logMessage("Point out of rage!", tr("ImageHandler::generatePixelInfo()"));
		return;
	}

	std::vector<int> colorValues;

	if(!_ImageData.data.empty())
	{
		unsigned int nChannels = 3;
		unsigned int maxVal = 0;

		if(_ImageData.mosaic != MOSAIC_NONE)
		{
			nChannels = 4;
			maxVal = 1<<10; // normalised to 10b when loaded
		}
		else
		{
			PIXELTYPE sType;
			if(PixelTransformYUV(&sType, _ImageData.pxlFormat) != IMG_SUCCESS)
			{
				PixelTransformRGB(&sType, _ImageData.pxlFormat);
			}
			if(sType.ui8BitDepth > 0)
			{
				maxVal = 1<<sType.ui8BitDepth;
			}
		}

		std::vector<cv::Mat> plains;
		cv::split(_ImageData.data, plains);

		if(plains.size() < nChannels)
		{
			emit riseError("Wrong number of image plains!");
		}

		for(unsigned int i = 0; i < plains.size(); i++)
		{
			int v = -1;
			switch(plains[i].depth())
			{
			case CV_32F:
				v = floor(plains[i].at<float>(pos.toPoint().y(), pos.toPoint().x()));
				colorValues.push_back(v);
				break;
			case CV_8U:
				v = plains[i].at<unsigned char>(pos.toPoint().y(), pos.toPoint().x());
				colorValues.push_back(v);
				break;
			}
		}
	}

	emit pixelInfoReceived(QPoint(pos.toPoint().x(), pos.toPoint().y()), colorValues);
}

void VisionLive::ImageHandler::generateAreaStatistics(const ImageData &imgData)
{
	_imgLock.lock();
	QRect area = _imgConf.selectionArea;
	_imgLock.unlock();

	if(area.height()*area.width() <= 0 ||
		area.x()+area.width() > imgData.data.cols ||
		area.y()+area.height() > imgData.data.rows)
	{
		return;
	}

	std::vector<double> areaStats;

	int nChannels = (imgData.mosaic != MOSAIC_NONE)? 4 : 3;

	std::vector<cv::Mat> channelPlains;
	cv::split(imgData.data, channelPlains);

	unsigned long int nPixels = area.height()*area.width();

	double rAvg = 0;
	double gAvg = 0;
	double bAvg = 0;

	switch(channelPlains[0].depth())
	{
	case CV_32F:
		for(int i = 0; i < area.height(); i++)
		{
			for(int j = 0; j < area.width(); j++)
			{
				rAvg += channelPlains[0].at<float>(area.y()+i, area.x()+j);
				gAvg += channelPlains[1].at<float>(area.y()+i, area.x()+j);
				bAvg += channelPlains[2].at<float>(area.y()+i, area.x()+j);
			}
		}
		break;
	case CV_8U:
		for(int i = 0; i < area.height(); i++)
		{
			for(int j = 0; j < area.width(); j++)
			{
				rAvg += channelPlains[0].at<unsigned char>(area.y()+i, area.x()+j);
				gAvg += channelPlains[1].at<unsigned char>(area.y()+i, area.x()+j);
				bAvg += channelPlains[2].at<unsigned char>(area.y()+i, area.x()+j);
			}
		}
		break;
	}

	rAvg /= (double)nPixels;
	gAvg /= (double)nPixels;
	bAvg /= (double)nPixels;

	double rDevAvg = 0;
	double gDevAvg = 0;
	double bDevAvg = 0;

	switch(channelPlains[0].depth())
	{
	case CV_32F:
		for(int i = 0; i < area.height(); i++)
		{
			for(int j = 0; j < area.width(); j++)
			{
				rDevAvg += (channelPlains[0].at<float>(area.y()+i, area.x()+j) - rAvg)*(channelPlains[0].at<float>(area.y()+i, area.x()+j) - rAvg);
				gDevAvg += (channelPlains[1].at<float>(area.y()+i, area.x()+j) - gAvg)*(channelPlains[1].at<float>(area.y()+i, area.x()+j) - gAvg);
				bDevAvg += (channelPlains[2].at<float>(area.y()+i, area.x()+j) - bAvg)*(channelPlains[2].at<float>(area.y()+i, area.x()+j) - bAvg);
			}
		}
		break;
	case CV_8U:
		for(int i = 0; i < area.height(); i++)
		{
			for(int j = 0; j < area.width(); j++)
			{
				rDevAvg += (channelPlains[0].at<unsigned char>(area.y()+i, area.x()+j) - rAvg)*(channelPlains[0].at<unsigned char>(area.y()+i, area.x()+j) - rAvg);
				gDevAvg += (channelPlains[1].at<unsigned char>(area.y()+i, area.x()+j) - gAvg)*(channelPlains[1].at<unsigned char>(area.y()+i, area.x()+j) - gAvg);
				bDevAvg += (channelPlains[2].at<unsigned char>(area.y()+i, area.x()+j) - bAvg)*(channelPlains[2].at<unsigned char>(area.y()+i, area.x()+j) - bAvg);
			}
		}
		break;
	}
#ifdef USE_MATH_NEON
	rDevAvg = (double)sqrtf_neon((float)rDevAvg/(float)nPixels);
	gDevAvg = (double)sqrtf_neon((float)gDevAvg/(float)nPixels);
	bDevAvg = (double)sqrtf_neon((float)bDevAvg/(float)nPixels);

	double rSNR = 20*(double)log10f_neon((float)(rAvg/rDevAvg));
	double gSNR = 20*(double)log10f_neon((float)(gAvg/gDevAvg));
	double bSNR = 20*(double)log10f_neon((float)(bAvg/bDevAvg));
#else
	rDevAvg = sqrt(rDevAvg/(double)nPixels);
	gDevAvg = sqrt(gDevAvg/(double)nPixels);
	bDevAvg = sqrt(bDevAvg/(double)nPixels);

	double rSNR = 20*log10(rAvg/rDevAvg);
	double gSNR = 20*log10(gAvg/gDevAvg);
	double bSNR = 20*log10(bAvg/bDevAvg);
#endif

	areaStats.push_back(rAvg);
	areaStats.push_back(gAvg);
	areaStats.push_back(bAvg);
	areaStats.push_back(rSNR);
	areaStats.push_back(gSNR);
	areaStats.push_back(bSNR);

	emit areaStatisticsReceived(areaStats);
}

int VisionLive::ImageHandler::setImageSending(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(GUICMD_SET_LF_ENABLE);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "ImageHandler::setImageSending()");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int VisionLive::ImageHandler::DPF_getMap(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;
	size_t nRead;
	size_t nToRead;
	size_t toRead;
	size_t w;
	size_t maxChunk = PARAMSOCK_MAX;

	// Send command
	cmd[0] = htonl(GUICMD_GET_DPF_MAP);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        emit logError(tr("Failed to write to socket!"), "ImageHandler::DPF_getMap()");
		return IMG_ERROR_FATAL;
    }

	// Receive number of defects (indicates wright map size)
	if(socket->read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		emit logError(tr("Failed to read from socket!"), "ImageHandler::DPF_getMap()");
		return IMG_ERROR_FATAL;
	}
	int nDefects = ntohl(cmd[0]);

	// Receive wright map
	nToRead = nDefects*sizeof(IMG_UINT64);
	IMG_UINT8 *map = NULL;
	map = (IMG_UINT8 *)malloc(nToRead);

	toRead = nToRead;
	w = 0;

	// Receive DPF write map
	while(w < toRead)
	{
		if(socket->read(((IMG_UINT8*)map)+w, (toRead-w > maxChunk)? maxChunk : toRead-w, nRead, -1) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to read from socket!"), "ImageHandler::DPF_getMap()");
			return IMG_ERROR_FATAL;
		}
		//if(w%PARAMSOCK_MAX == 0)printf("dpfMap read %ld/%ld\n", w, toRead);
		w += nRead;
	}

	// Clean old _receivedDPFWriteMap
	free(_pDPFOutMap);
	// Allocate memory for new _receivedDPFWriteMap
	_pDPFOutMap = (IMG_UINT8 *)malloc(nToRead);
	// Copy received map to new _receivedDPFWriteMap
	memcpy(_pDPFOutMap, map, nToRead);
	// Free map
	free(map);
	// Save number of defects
	_nDefects = nDefects;

	return IMG_SUCCESS;
}

void VisionLive::ImageHandler::markDPFPoints(QImage *image, double xScaler, double yScaler)
{
	IMG_UINT16 xCoord = 0;
	IMG_UINT16 yCoord = 0;

	// Load DPF map
	for(unsigned int i = 0; i < _nDefects*8; i+=8)
	{
		xCoord = (_pDPFOutMap[i+1]<<8)|_pDPFOutMap[i+0];
		yCoord = (_pDPFOutMap[i+3]<<8)|_pDPFOutMap[i+2];
		/*IMG_UINT16 value = (_pDPFOutMap[i+5]<<8)|_pDPFOutMap[i+4];
		IMG_UINT16 conf = ((_pDPFOutMap[i+7]<<8)|_pDPFOutMap[i+6])&0x0FFF;
		IMG_UINT16 s = (_pDPFOutMap[i+7])>>7;*/

		xCoord = xCoord/xScaler;
		yCoord = yCoord/yScaler;

		image->setPixel(xCoord, yCoord, qRgb(255, 0, 0));
	}
}

int VisionLive::ImageHandler::receiveConfiguration(ParamSocket *socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;
	size_t nRead;

	ISPC::ParameterList newParamsList;

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Request parameterList from FPGA
	cmd[0] = htonl(GUICMD_GET_PARAMETER_LIST);
	if(socket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		emit logError(tr("Failed to write to socket!"), "ImageHandler::receiveConfiguration()");
		return IMG_ERROR_FATAL;
	}

	// Expect GUICMD_SET_PARAMETER_LIST from FPGA
	if(socket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		emit logError(tr("Failed to read from socket!"), "ImageHandler::receiveConfiguration()");
		return IMG_ERROR_FATAL;
	}

	if(ntohl(cmd[0]) != GUICMD_SET_PARAMETER_LIST)
	{
		emit logError(tr("Unexpected command received!"), "ImageHandler::receiveConfiguration()");
		return IMG_ERROR_FATAL;
	}

	emit logMessage(tr("GUICMD_SET_PARAMETER_LIST"), "ImageHandler::receiveConfiguration()");

	bool continueRunning = true;

	while(continueRunning)
	{
		if(socket->read(cmd, 2*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			emit logError(tr("Failed to write to socket!"), "ImageHandler::receiveConfiguration()");
			return IMG_ERROR_FATAL;
		}
		switch(ntohl(cmd[0]))
		{
		case GUICMD_SET_PARAMETER_NAME:
			{
				std::string paramName;
				std::vector<std::string> paramValues;

				bool readingValues = true;

				if(socket->read(buff, ntohl(cmd[1]), nRead, N_TRIES) != IMG_SUCCESS)
				{
					emit logError(tr("Failed to write to socket!"), "ImageHandler::receiveConfiguration()");
					return IMG_ERROR_FATAL;
				}
				buff[nRead] = '\0';
				paramName = std::string(buff);
				memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

				while(readingValues)
				{
					if(socket->read(cmd, 2*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
					{
						emit logError(tr("Failed to write to socket!"), "ImageHandler::receiveConfiguration()");
						return IMG_ERROR_FATAL;
					}

					switch(ntohl(cmd[0]))
					{
					case GUICMD_SET_PARAMETER_VALUE:
						{
							if(socket->read(buff, ntohl(cmd[1]), nRead, N_TRIES) != IMG_SUCCESS)
							{
								emit logError(tr("Failed to write to socket!"), "ImageHandler::receiveConfiguration()");
								return IMG_ERROR_FATAL;
							}
							buff[nRead] = '\0';
							paramValues.push_back(std::string(buff));
							memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
						}
						break;
					case GUICMD_SET_PARAMETER_END:
						readingValues = false;
						break;
					default:
						emit logError(tr("Urecognized command received!"), "ImageHandler::receiveConfiguration()");
						return IMG_ERROR_FATAL;
					}
				}
				
				/*printf("%s ", paramName.c_str());
				for(unsigned int i = 0; i < paramValues.size(); i++)
					printf("%s ", paramValues[i].c_str());
				printf("\n");*/

				ISPC::Parameter newParam(paramName, paramValues);
				newParamsList.addParameter(newParam);
			}
			break;
		case GUICMD_SET_PARAMETER_LIST_END:
			emit logMessage(tr("GUICMD_SET_PARAMETER_LIST_END"), "ImageHandler::receiveConfiguration()");
			continueRunning = false;
			_receivedConfiguration = newParamsList;
			break;
		default:
			emit logError(tr("Urecognized command received!"), "ImageHandler::receiveConfiguration()");
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}

//
// Public Slots
//

void VisionLive::ImageHandler::connect(IMG_UINT16 port)
{
	_port = port;


	if(!isRunning()) 
	{
		start();
	}
}

void VisionLive::ImageHandler::disconnect()
{
	if(isRunning())
	{
		emit logMessage(tr("Waiting for disconnection..."), tr("ImageHandler::disconnect()"));
		pushCommand(GUICMD_QUIT);
	}
}

void VisionLive::ImageHandler::cancelConnection()
{
	_continueRun = false;
}

void VisionLive::ImageHandler::record(int nFrames, const QString &name)
{
	_recLock.lock();
	_recConf.nFrames = nFrames;
	_recConf.name = name;
	_recLock.unlock();

	pushCommand(GUICMD_SET_IMAGE_RECORD);
}

void VisionLive::ImageHandler::removeRecord(int index)
{
	_recLock.lock();
	for(int i = 0; i < _recData[index]->nFrames; i++)
	{
		free((IMG_UINT8 *)_recData[index]->frameData[i].origData.data);
	}
	for(unsigned int i = 0; i < _recData[index]->DPFMaps.size(); i++)
	{
		free(_recData[index]->DPFMaps[i]);
	}
	delete _recData[index];
	_recData.erase(_recData.begin() + index);
	_recLock.unlock();
}

void VisionLive::ImageHandler::setCorrectGamma(bool opt)
{
	_imgLock.lock();
	_imgConf.setCorrectGamma = opt;
	_imgLock.unlock();
}

void VisionLive::ImageHandler::setGenerateHistograms(bool opt)
{
	_imgLock.lock();
	_imgConf.generateHistogram = opt;
	_imgLock.unlock();
}

void VisionLive::ImageHandler::setGenerateVectorscope(bool opt)
{
	_imgLock.lock();
	_imgConf.generateVectorscope = opt;
	_imgLock.unlock();
}

void VisionLive::ImageHandler::setGenerateLineView(bool opt)
{
	_imgLock.lock();
	_imgConf.generateLineView = opt;
	_imgLock.unlock();
}

void  VisionLive::ImageHandler::setMarkDPFPoints(bool opt)
{
	_imgLock.lock();
	_imgConf.markDPFPoints = opt;
	_imgLock.unlock();
}

void VisionLive::ImageHandler::setImageType(ImageType type)
{
	_imgLock.lock();
	_imgConf.type = type;
	_imgLock.unlock();

	_recLock.lock();
	_recConf.type = type;
	_recLock.unlock();
}

void VisionLive::ImageHandler::saveRecAsJPEG(int recDataIndex, const QString &filename)
{
	std::vector<QImage *> images;

	_recLock.lock();
	for(unsigned int i = 0; i < _recData[recDataIndex]->frameData.size(); i++)
	{
		images.push_back(_recData[recDataIndex]->frameData[i].convertToQImage());
	}
	_recLock.unlock();

	QString fileExtension = filename.right(4);

	for(unsigned int i = 0; i < images.size(); i++)
	{
		QString fileToUse = filename;
		if(images.size() > 1)
		{
			fileToUse.chop(4);
			fileToUse = fileToUse + QString::number(i) + fileExtension;
		}

		if(images[i])
		{
			emit logMessage(tr("Saving imge as JPEG..."), tr("ImageHandler::saveImageAsJPEG()"));
			if(images[i]->save(fileToUse, "JPG"))
			{
				emit logMessage(tr("Image saved!"), tr("ImageHandler::saveImageAsJPEG()"));
			}
			else
			{
				emit logError(tr("Failed to save image!"), tr("ImageHandler::saveImageAsJPEG()"));
				emit riseError(tr("Error while saving image!"));
				return;
			}
		}
		else
		{
			emit logError(tr("Failed to save image!"), tr("ImageHandler::saveImageAsJPEG()"));
			emit riseError(tr("Error while converting image!"));
			return;
		}

		delete images[i];
	}
}

void VisionLive::ImageHandler::saveRecAsFLX(int recDataIndex, const QString &filename)
{
	_recLock.lock();

	Save s;
	
	ISPC::Save::SaveType saveType;

	switch(_recData[recDataIndex]->type)
	{
	case YUV: saveType = ISPC::Save::YUV; break;
	case RGB: saveType = ISPC::Save::RGB; break;
	case DE: saveType = ISPC::Save::Bayer; break;
	}

	s.open(saveType, 
		_recData[recDataIndex]->frameData[0].pxlFormat, 
		_recData[recDataIndex]->frameData[0].mosaic,
		_recData[recDataIndex]->frameData[0].origData.width,
		_recData[recDataIndex]->frameData[0].origData.height, 
		std::string(filename.toLatin1()));

	for(int i = 0; i < _recData[recDataIndex]->nFrames; i++)
	{
		switch(_recData[recDataIndex]->type)
		{
		case YUV: s.saveYUV_m(_recData[recDataIndex]->frameData[i].origData); break;
		case RGB: s.saveRGB_m(_recData[recDataIndex]->frameData[i].origData); break;
		case DE: s.saveBayer_m(_recData[recDataIndex]->frameData[i].origData); break;
		}
	}

	s.close();

	_recLock.unlock();
}

void VisionLive::ImageHandler::setPixelMarkerPosition(QPointF point)
{
	_imgLock.lock();
	_imgConf.marker = point;
	_imgLock.unlock();
}

void VisionLive::ImageHandler::setSelection(QRect area, bool genStats)
{
	_imgLock.lock();
	_imgConf.selectionArea = area;
	_imgConf.generateAreaStatistics = genStats;
	_imgLock.unlock();
}

void VisionLive::ImageHandler::setImageSending()
{
	pushCommand(GUICMD_SET_LF_ENABLE);
}
