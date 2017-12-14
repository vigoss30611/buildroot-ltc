/********************************************************************************
 @file ISPC2_tcp.cpp

 @brief Implementation of the ISPC2_tcp class.

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K.

******************************************************************************/

// Control algorithms
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB.h>
#include <ispc/ControlAWB_PID.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlDNS.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlAF.h>

// Modules
#include "ispc/ModuleOUT.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ModuleTNM.h"
#include "ispc/ModuleHIS.h"
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ModuleDNS.h"
#include "ispc/ModuleSHA.h"
#include "ispc/ModuleDPF.h"
#include "ispc/ModuleR2Y.h"
#include "ispc/ModuleMIE.h"
#include "ispc/ModuleVIB.h"
#include "ispc/ModuleESC.h"
#include "ispc/ModuleDSC.h"
#include "ispc/ModuleMGM.h"
#include "ispc/ModuleDGM.h"
#include "ispc/ModuleENS.h"
#include "ispc/ModuleY2R.h"

// Main
#include "ISPC2_tcp.hpp"

#include <sensorapi/sensorapi.h>
#include "ispc/ParameterFileParser.h"
#include "felixcommon/lshgrid.h"
#include <ispc/Save.h>
#include <time.h>

#include "paramsocket/paramsocket.hpp"
#ifdef WIN32
#include <WinSock2.h>
#define YIELD YieldProcessor();
#else
extern "C" {
#include <errno.h>
#include <arpa/inet.h>
#ifdef ANDROID
// - include "sched.h"?
#define YIELD sched_yield();
#else
#define YIELD pthread_yield();
#endif
}
#endif

#define LOG_TAG "LISTENER"
#include <felixcommon/userlog.h>

#include "felix_hw_info.h"

#include "ci/ci_api_structs.h"

/*
 * FeedSender
 */

FeedSender::FeedSender(Listener *parent): _parent(parent)
{
	_continueRun = false;
	_pause = false;
	_running = false;

	_sendIMG = true;
	_format = (int)RGB;

	_fps = 0.00f;
	_frameCount = 0;
}

FeedSender::~FeedSender()
{
	pthread_join(_thread, NULL);
}

void FeedSender::start()
{
    pthread_create(&_thread, NULL, FeedSender::staticEntryPoint, this);
}

void *FeedSender::staticEntryPoint(void *c)
{
    ((FeedSender *)c)->run();
    return NULL;
}

void FeedSender::run()
{
	_continueRun = true;

	_running = true;

	time_t t;
	t = time(NULL);
	int nFrames = 0;

	IMG_UINT32 cmd;
    size_t nRead;

	while(_continueRun)
	{	
		if(_pause) 
		{
			if(_running)
			{
				_running = false;
			}
			continue;
		}
		else
		{
			if(!_running)
			{
				_running = true;
			}
		}

		LOG_DEBUG("Waiting for command...\n");
		if(_parent->_feedSocket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) == IMG_SUCCESS)
		{
			LOG_DEBUG("Command received!\n");
			switch(ntohl(cmd))
			{
			case GUICMD_QUIT:
				LOG_DEBUG("CMD_QUIT\n");
				_continueRun = false;
				break;
			case GUICMD_GET_PARAMETER_LIST:
				//LOG_DEBUG("GUICMD_GET_PARAMETER_LIST\n");
				if(_parent->sendParameterList(*_parent->_feedSocket) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: sendParameterList() failed!\n");
					_continueRun = false;
				}
				break;
			case GUICMD_GET_IMAGE:
				LOG_DEBUG("GUICMD_GET_IMAGE\n");
				if(sendImage(*_parent->_feedSocket, false, _sendIMG) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: sendImage() failed!\n");
					_continueRun = false;
				}
				break;
			case GUICMD_SET_IMAGE_RECORD:
				LOG_DEBUG("GUICMD_SET_IMAGE_RECORD\n");
				if(record(*_parent->_feedSocket) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: record() failed!\n");
					_continueRun = false;
				}
				break;
			case GUICMD_SET_LF_ENABLE:
				LOG_DEBUG("GUICMD_SET_LF_ENABLE\n");
				if(LF_enable(*_parent->_feedSocket) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: LF_enable() failed!\n");
					_continueRun = false;
				}
				break;
			case GUICMD_SET_LF_FORMAT:
				LOG_DEBUG("GUICMD_SET_LF_FORMAT\n");
				if(LF_setFormat(*_parent->_feedSocket) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: LF_setFormat() failed!\n");
					_continueRun = false;
				}
				break;
			case GUICMD_GET_LF:
				LOG_DEBUG("GUICMD_GET_FEED\n");
				if(LF_send(*_parent->_feedSocket) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: LF_send() failed!\n");
					_continueRun = false;
				}
				break;
			case GUICMD_GET_DPF_MAP:
				//LOG_DEBUG("GUICMD_GET_DPF_MAP\n");
				if(_parent->DPF_map_send(*_parent->_feedSocket) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: DPF_map_send() failed!\n");
					_continueRun = false;
				}
				break;
			default: 
				LOG_ERROR("Error: Unrecognized command!\n");
				_continueRun = false;
				break;
			}
		}
		else
		{
			LOG_ERROR("Error: failed to read from socket!\n");
			_continueRun = false;
			break;
		}
	}

	LOG_DEBUG("Live feed stoped!\n");
	_parent->stop();
	_running = false;
}

void FeedSender::pause()
{
	_pause = true;
}

bool FeedSender::isRunning() const 
{
	return _running;
}

bool FeedSender::isPaused() const
{
	return _pause;
}

void FeedSender::stop()
{
	_continueRun = false;
}

void FeedSender::resume()
{
	_pause = false;
}

int FeedSender::refresh(bool sendIMG)
{
	IMG_UINT32 cmd;
	size_t nWritten;

	if(sendIMG)
	{
		cmd = htonl(GUICMD_SET_LF_REFRESH);
		if(_parent->_feedSocket->write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}

	if(sendImage(*(_parent->_feedSocket), true, _sendIMG) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: sendImage() failed!\n");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

void FeedSender::toggleSending()
{
	_sendIMG = !_sendIMG;
}

void FeedSender::setFormat(int format)
{
	_format = format;
}

int FeedSender::getFormat() const
{
	return _format;
}

double FeedSender::getFps() const
{
	return _fps;
}
	
IMG_UINT32 FeedSender::getFrameCount() const
{
	return _frameCount;
}

//
// Image
//

int FeedSender::sendImage(ParamSocket &socket, bool feed, bool sendIMG)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;
	size_t nRead;

	ImageType imgTypeRequested;

	// Check which type of image to send
	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: failed to read from socket!\n");
		return IMG_ERROR_FATAL;
	}
	imgTypeRequested = (ImageType)ntohl(cmd[0]);

	// Take shot
	ISPC::Shot frame;
	if(_parent->takeShot(frame) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: takeShot() failed!\n");
		return IMG_ERROR_FATAL;
	}

	// Send imageInfo to inform of upcomming data size and image type
	const IMG_UINT8 *data;
	unsigned width;
	unsigned height;
	unsigned stride;
	unsigned fmt;
	unsigned mos;
	unsigned offset;

	switch(imgTypeRequested)
	{
	case DE:
		width = frame.BAYER.width;
		height = frame.BAYER.height;
		stride = frame.BAYER.stride;
		fmt = (IMG_UINT16)frame.BAYER.pxlFormat;
		mos = (IMG_UINT16)_parent->_camera->getSensor()->eBayerFormat;
		data = frame.BAYER.data;
		break;
	case RGB:
		width = frame.RGB.width;
		height = frame.RGB.height;
		stride = frame.RGB.stride;
		fmt = (IMG_UINT16)frame.RGB.pxlFormat;
		mos = MOSAIC_NONE;
		data = frame.RGB.data;
		break;
	case YUV:
		width = frame.YUV.width;
		height = frame.YUV.height;
		stride = frame.YUV.stride;
		fmt = (IMG_UINT16)frame.YUV.pxlFormat;
		mos = MOSAIC_NONE;
		data = frame.YUV.data;
		break;
	default:
		LOG_ERROR("Error: Unrecognized image type!\n");
		return IMG_ERROR_FATAL;
		break;
	}
	offset = frame.YUV.offsetCbCr;
	
	LOG_DEBUG("Image info %dx%d (str %d) fmt %d-%d\n", width, height, stride, fmt, mos);

	double xScaler = 0;
	double yScaler = 0;
	if(imgTypeRequested == RGB)
	{
		ISPC::ModuleDSC *dsc = _parent->_camera->getModule<ISPC::ModuleDSC>();
		xScaler = dsc->aPitch[0];
		yScaler = dsc->aPitch[1];
	}
	else if(imgTypeRequested == YUV)
	{
		ISPC::ModuleESC *esc = _parent->_camera->getModule<ISPC::ModuleESC>();
		xScaler = esc->aPitch[0];
		yScaler = esc->aPitch[1];
	}

	cmd[0] = htonl(GUICMD_SET_IMAGE);
	cmd[1] = htonl(imgTypeRequested);
	cmd[2] = htonl(width);
	cmd[3] = htonl(height);
	cmd[4] = htonl(stride);
	cmd[5] = htonl(fmt);
	cmd[6] = htonl(mos);
	cmd[7] = htonl((int)_sendIMG);
	cmd[8] = htonl(xScaler*1000.0f);
	cmd[9] = htonl(yScaler*1000.0f);

	if(socket.write(cmd, 10*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
	}

	if(sendIMG)
	{
		// If no data don't send anything
		if(width == 0 || height == 0) 
		{
			LOG_WARNING("No image data generated!\n");
			// Release shot always at the end
			if(_parent->releaseShot(frame) != IMG_SUCCESS)
			{
				LOG_ERROR("Error: releaseShot() failed!\n");
				return IMG_ERROR_FATAL;
			}

			return IMG_SUCCESS;
		}

		PIXELTYPE sYUVType;
		bool bYUV = PixelTransformYUV(&sYUVType, (ePxlFormat)fmt) == IMG_SUCCESS;

		if((ePxlFormat)fmt == YVU_420_PL12_10 || (ePxlFormat)fmt == YUV_420_PL12_10 || (ePxlFormat)fmt == YVU_422_PL12_10 || (ePxlFormat)fmt == YUV_422_PL12_10)
		{
			unsigned bop_line = width/sYUVType.ui8PackedElements;
			if(width%sYUVType.ui8PackedElements)
			{
				bop_line++;
			}
			bop_line = bop_line*sYUVType.ui8PackedStride;
			
			printf("Writting [byted]: %d\n",  bop_line*height + 2*(bop_line/sYUVType.ui8HSubsampling)*height/sYUVType.ui8VSubsampling);
			long int w = 0;
			for(unsigned line = 0; line < height; line++)
			{
				// Write Y plane
				if(socket.write((IMG_UINT8*)data + line*stride, bop_line, nWritten, -1) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: failed to write to socket!\n");
					return IMG_ERROR_FATAL;
				}
				w += nWritten;
				//if(w%10000 == 0) printf("image sent %ld/%ld\n", w, bop_line*height + 2*(bop_line/sYUVType.ui8HSubsampling)*height/sYUVType.ui8VSubsampling);
			}
			
			data += offset;


			// Write CbCr plane
			for(unsigned line = 0; line < height/sYUVType.ui8VSubsampling; line++)
			{
				if(socket.write((IMG_UINT8*)data + line*stride, 2*(bop_line/sYUVType.ui8HSubsampling), nWritten, -1) != IMG_SUCCESS )
				{
					LOG_ERROR("Error: failed to write to socket!\n");
					return IMG_ERROR_FATAL;
				}
				w += nWritten;
				//if(w%10000 == 0) printf("image sent %ld/%ld\n", w, bop_line*height + 2*(bop_line/sYUVType.ui8HSubsampling)*height/sYUVType.ui8VSubsampling);
			}
		}
		else
		{
	
			nWritten = 0;
	
			// Send frame data 
			if(bYUV && stride != width)
			{
				// the stride is different than the size so we are going to send the image without it
				IMG_UINT8 *toSend = (IMG_UINT8*)data;

				LOG_DEBUG("Writing YUV data %d bytes\n", width*height + 2*(width/sYUVType.ui8HSubsampling)*(height/sYUVType.ui8VSubsampling));
				for(unsigned line = 0; line < height; line++)
				{
					if(socket.write(toSend + line*stride, width, nWritten, -1) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: failed to write to socket!\n");
						return IMG_ERROR_FATAL;
					}
				}

				toSend += offset;

				for(unsigned line = 0; line < height/sYUVType.ui8VSubsampling; line++)
				{
					if ( socket.write(toSend + line*stride, 2*(width/sYUVType.ui8HSubsampling), nWritten, -1) != IMG_SUCCESS )
					{
						LOG_ERROR("Error: failed to write to socket!\n");
						return IMG_ERROR_FATAL;
					}
				}
			}
			else
			{
				int sumHeight = height;
				if(bYUV)
				{
					sumHeight += height/sYUVType.ui8VSubsampling;
				}

				size_t toSend = stride*sumHeight*sizeof(IMG_UINT8);
				size_t w = 0;
				size_t maxChunk = PARAMSOCK_MAX;

				LOG_DEBUG("Writing image data %d bytes\n", toSend);
				while(w < toSend)
				{
					if(socket.write(((IMG_UINT8*)data)+w, (toSend-w > maxChunk)? maxChunk : toSend-w, nWritten, -1) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: failed to write to socket!\n");
						return IMG_ERROR_FATAL;
					}
					//if(w%(PARAMSOCK_MAX*2) == 0)LOG_DEBUG("image write %ld/%ld\n", w, toSend);
					w += nWritten;
				}
			}
		}
	}

	// Release shot always at the end
	if(_parent->releaseShot(frame) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: releaseShot() failed!\n");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int FeedSender::record(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	if(socket.read(cmd, 2*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	const int nFrames = ntohl(cmd[0]);
	ImageType imgType = (ImageType)ntohl(cmd[1]);

	LOG_DEBUG("Requested number of frames - %d\n", nFrames);

	// Stop camera
	_parent->stopCamera();

	std::list<IMG_UINT32> temp_bufferIds;

	if(_parent->_camera->allocateBufferPool(nFrames, temp_bufferIds) != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: allocateBufferPool() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	if(_parent->_camera->startCapture() != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: failed to start the capture!\n");
	   return IMG_ERROR_FATAL;
	}

	LOG_DEBUG("Recoring... started\n");

	ISPC::Shot *frames = new ISPC::Shot[nFrames];

	for(int i = 0; i < nFrames; i++)
	{
		LOG_DEBUG("Recoring... taking %d frame...\n", i);

		// Take shot
		if(_parent->takeShot(frames[i]) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: takeShot() failed!\n");
			return IMG_ERROR_FATAL;
		}
	}

	const IMG_UINT8 *data;
	unsigned width;
	unsigned height;
	unsigned stride;
	unsigned fmt;
	unsigned mos;
	unsigned offset;

	switch(imgType)
	{
	case DE:
		width = frames[0].BAYER.width;
		height = frames[0].BAYER.height;
		stride = frames[0].BAYER.stride;
		fmt = (IMG_UINT16)frames[0].BAYER.pxlFormat;
		mos = (IMG_UINT16)_parent->_camera->getSensor()->eBayerFormat;
		data = frames[0].BAYER.data;
		break;
	case RGB:
		width = frames[0].RGB.width;
		height = frames[0].RGB.height;
		stride = frames[0].RGB.stride;
		fmt = (IMG_UINT16)frames[0].RGB.pxlFormat;
		mos = MOSAIC_NONE;
		data = frames[0].RGB.data;
		break;
	case YUV:
		width = frames[0].YUV.width;
		height = frames[0].YUV.height;
		stride = frames[0].YUV.stride;
		fmt = (IMG_UINT16)frames[0].YUV.pxlFormat;
		mos = MOSAIC_NONE;
		data = frames[0].YUV.data;
		break;
	default:
		LOG_ERROR("Error: Unrecognized image type!\n");
		return IMG_ERROR_FATAL;
		break;
	}
	offset = frames[0].YUV.offsetCbCr;

	LOG_DEBUG("Image info %dx%d (str %d) fmt %d-%d\n", width, height, stride, fmt, mos);

	double xScaler = 0;
	double yScaler = 0;
	if(imgType == RGB)
	{
		ISPC::ModuleDSC *dsc = _parent->_camera->getModule<ISPC::ModuleDSC>();
		xScaler = dsc->aPitch[0];
		yScaler = dsc->aPitch[1];
	}
	else if(imgType == YUV)
	{
		ISPC::ModuleESC *esc = _parent->_camera->getModule<ISPC::ModuleESC>();
		xScaler = esc->aPitch[0];
		yScaler = esc->aPitch[1];
	}

	cmd[0] = htonl(GUICMD_SET_IMAGE);
	cmd[1] = htonl(imgType);
	cmd[2] = htonl(width);
	cmd[3] = htonl(height);
	cmd[4] = htonl(stride);
	cmd[5] = htonl(fmt);
	cmd[6] = htonl(mos);

	if(socket.write(cmd, 7*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
	}

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "xScaler %f", xScaler); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "yScaler %f", yScaler); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	
	PIXELTYPE sYUVType;
	bool bYUV = PixelTransformYUV(&sYUVType, (ePxlFormat)fmt) == IMG_SUCCESS;

	for(int i = 0; i < nFrames; i++)
	{
		switch(imgType)
		{
		case DE:
			data = frames[i].BAYER.data;
			break;
		case RGB:
			data = frames[i].RGB.data;
			break;
		case YUV:
			data = frames[i].YUV.data;
			break;
		default:
			LOG_ERROR("Error: Unrecognized image type!\n");
			return IMG_ERROR_FATAL;
			break;
		}

		cmd[0] = htonl(frames[i].metadata.timestamps.ui32StartFrameOut);
		if(socket.write(cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}

		nWritten = 0;
	
		if((ePxlFormat)fmt == YVU_420_PL12_10 || (ePxlFormat)fmt == YUV_420_PL12_10 || (ePxlFormat)fmt == YVU_422_PL12_10 || (ePxlFormat)fmt == YUV_422_PL12_10)
		{
			unsigned bop_line = width/sYUVType.ui8PackedElements;
			if(width%sYUVType.ui8PackedElements)
			{
				bop_line++;
			}
			bop_line = bop_line*sYUVType.ui8PackedStride;
			
			printf("Writting [byted]: %d\n",  bop_line*height + 2*(bop_line/sYUVType.ui8HSubsampling)*height/sYUVType.ui8VSubsampling);
			long int w = 0;
			for(unsigned line = 0; line < height; line++)
			{
				// Write Y plane
				if(socket.write((IMG_UINT8*)data + line*stride, bop_line, nWritten, -1) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: failed to write to socket!\n");
					return IMG_ERROR_FATAL;
				}
				w += nWritten;
				//if(w%10000 == 0) printf("image sent %ld/%ld\n", w, bop_line*height + 2*(bop_line/sYUVType.ui8HSubsampling)*height/sYUVType.ui8VSubsampling);
			}
			
			data += offset;

			// Write CbCr plane
			for(unsigned line = 0; line < height/sYUVType.ui8VSubsampling; line++)
			{
				if(socket.write((IMG_UINT8*)data + line*stride, 2*(bop_line/sYUVType.ui8HSubsampling), nWritten, -1) != IMG_SUCCESS )
				{
					LOG_ERROR("Error: failed to write to socket!\n");
					return IMG_ERROR_FATAL;
				}
				w += nWritten;
				//if(w%10000 == 0) printf("image sent %ld/%ld\n", w, bop_line*height + 2*(bop_line/sYUVType.ui8HSubsampling)*height/sYUVType.ui8VSubsampling);
			}
		}
		else
		{
			// Send frame data 
			if(bYUV && stride != width)
			{
				// the stride is different than the size so we are going to send the image without it
				IMG_UINT8 *toSend = (IMG_UINT8*)data;

				LOG_DEBUG("Writing YUV data %d bytes\n", width*height + 2*(width/sYUVType.ui8HSubsampling)*(height/sYUVType.ui8VSubsampling));
				for(unsigned line = 0; line < height; line++)
				{
					if(socket.write(toSend + line*stride, width, nWritten, -1) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: failed to write to socket!\n");
						return IMG_ERROR_FATAL;
					}
				}

				toSend += offset;

				for(unsigned line = 0; line < height/sYUVType.ui8VSubsampling; line++)
				{
					if ( socket.write(toSend + line*stride, 2*(width/sYUVType.ui8HSubsampling), nWritten, -1) != IMG_SUCCESS )
					{
						LOG_ERROR("Error: failed to write to socket!\n");
						return IMG_ERROR_FATAL;
					}
				}
			}
			else
			{
				int sumHeight = height;
				if(bYUV)
				{
					sumHeight += height/sYUVType.ui8VSubsampling;
				}

				size_t toSend = stride*sumHeight*sizeof(IMG_UINT8);
				size_t w = 0;
				size_t maxChunk = PARAMSOCK_MAX;

				LOG_DEBUG("Writing image data %d bytes\n", toSend);
				while(w < toSend)
				{
					if(socket.write(((IMG_UINT8*)data)+w, (toSend-w > maxChunk)? maxChunk : toSend-w, nWritten, -1) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: failed to write to socket!\n");
						return IMG_ERROR_FATAL;
					}
					//if(w%(PARAMSOCK_MAX*2) == 0)LOG_DEBUG("image write %ld/%ld\n", w, toSend);
					w += nWritten;
				}
			}
		}

		// Send nDefects of DPF map
		cmd[0] = htonl(frames[i].DPF.size/frames[i].DPF.elementSize);
		if(socket.write(cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}

		// Send DPF map
		size_t toSend = (frames[i].DPF.size/frames[i].DPF.elementSize)*sizeof(IMG_UINT64);
		size_t w = 0;
		size_t maxChunk = PARAMSOCK_MAX;

		//LOG_DEBUG("Writing dpfMap data %d bytes\n", toSend);
		while(w < toSend)
		{
			if(socket.write(((IMG_UINT8*)frames[i].DPF.data)+w, (toSend-w > maxChunk)? maxChunk : toSend-w, nWritten, -1) != IMG_SUCCESS)
			{
				LOG_ERROR("Error: failed to write to socket!\n");
				return IMG_ERROR_FATAL;
			}
			//if(w%(PARAMSOCK_MAX) == 0)LOG_DEBUG("dpfMap write %ld/%ld\n", w, toSend);
			w += nWritten;
		}
	}

	for(int i = 0; i < nFrames; i++)
	{
		LOG_DEBUG("Recoring... releasing %d frame...\n", i);

		// Release shot always at the end
		if(_parent->releaseShot(frames[i]) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: releaseShot() failed!\n");
			return IMG_ERROR_FATAL;
		}
	}

	LOG_DEBUG("Recoring... finished\n");

	if(_parent->_camera->stopCapture())
	{
		LOG_ERROR("Error: failed to stop capturing!\n");
		return IMG_ERROR_FATAL;
	}

	LOG_DEBUG("BufferIds size %d\n", temp_bufferIds.size());
	std::list<IMG_UINT32>::iterator it;
    for(it = temp_bufferIds.begin(); it != temp_bufferIds.end(); it++)
    {
		LOG_DEBUG("Deregister buffer %d\n", *it);
		if(_parent->_camera->deregisterBuffer(*it) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: deregisterBuffer() failed on %d buffer!\n", *it);
			return EXIT_FAILURE;
		}
	}
    temp_bufferIds.clear();

	if(_parent->_camera->getPipeline()->deleteShots() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: deleteShots() failed!\n");
		return IMG_ERROR_FATAL;
	}

	// Start camera
	_parent->startCamera();

	return IMG_SUCCESS;
}

//
// LiveFeed
//

int FeedSender::LF_send(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	IMG_UINT32 state = _parent->_liveFeed->isRunning()? 1 : 0;
	IMG_UINT32 format = _parent->_liveFeed->getFormat();
	IMG_UINT32 fps = static_cast<IMG_UINT32>(_parent->_liveFeed->getFps()*1000);
	IMG_UINT32 frameCount = _parent->_liveFeed->getFrameCount();
	IMG_UINT32 missedFrames = _parent->_missedFrames;

	cmd[0] = htonl(state);
	cmd[1] = htonl(format);
	cmd[2] = htonl(fps);
	cmd[3] = htonl(frameCount);
	cmd[4] = htonl(missedFrames);

	if(socket.write(&cmd, 5*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int FeedSender::LF_enable(ParamSocket &socket)
{
	_sendIMG = !_sendIMG;

	return IMG_SUCCESS;
}

int FeedSender::LF_setFormat(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int format = ntohl(cmd[0]);

	_parent->pauseLiveFeed();
	
	_parent->_liveFeed->setFormat(format);

	cmd[0] = htonl(format);
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	_parent->resumeLiveFeed();

	return IMG_SUCCESS;
}


/*
 * Listener
 */

void Listener::disableSensor()
{
	if(_camera)
	{
		_camera->getSensor()->disable();
	}
}

int Listener::cleanUp()
{
	stopLiveFeed();
	if(stopCamera() != IMG_SUCCESS) return IMG_ERROR_FATAL;
	disableSensor();
	if(CI_DriverSetGammaLUT(_camera->getConnection(), &_GMA_initial) != IMG_SUCCESS) 
	{
		LOG_ERROR("Error: restoring initial Gamma LUT failed!\n");
		return IMG_ERROR_FATAL;
	}
	else
	{
		LOG_INFO("Initial Gamma LUT restored!\n");
	}

	return IMG_SUCCESS;
}

void Listener::stop()
{
	_continueRun = false;
}

Listener::Listener(
		const char *appName,
		const char *sensor, 
		const int sensorMode, 
		const int nBuffers,
		const char *ip, 
		const int port,
		const char *DGFrameFile, 
		unsigned int gasket, 
		unsigned int aBlanking[],
        bool aSensorFlip[]): 
		_appName(appName),
		_sensor(sensor), 
		_sensorMode(sensorMode), 
		_sensorFlip(
            (aSensorFlip[0]?SENSOR_FLIP_HORIZONTAL:SENSOR_FLIP_NONE) |
            (aSensorFlip[1]?SENSOR_FLIP_VERTICAL:SENSOR_FLIP_NONE)
        ),
		_nBuffers(nBuffers), 
		_ip(ip), 
		_port(port)
{
	_liveFeed = NULL;
	_camera = NULL;

	_continueRun = false;
	_focusing = false;
	_missedFrames = 0;

	_sendIMG = true;

	dpfStats.ui32DroppedMapModifications = 0;
	dpfStats.ui32FixedPixels = 0;
	dpfStats.ui32MapModifications = 0;
	dpfStats.ui32NOutCorrection = 0;

	dpfWriteMap = 0;
	dpfWriteMap_nDefects = 0;

	ensStats.data = 0;
	ensStats.elementSize = 0;
	ensStats.size = 0;
	ensStats.stride = 0;

	_dataGen = NO_DG;
	_DGFrameFile = DGFrameFile;
	_gasket = gasket;
	_aBlanking[0] = aBlanking[0];
	_aBlanking[1] = aBlanking[1];
    
    LOG_DEBUG("_sensorFlip=0x%x aSensorFlip=%d,%d 0x%x 0x%x\n",
        _sensorFlip, (int)aSensorFlip[0], (int)aSensorFlip[1],
        aSensorFlip[0]?SENSOR_FLIP_HORIZONTAL:SENSOR_FLIP_NONE,
        aSensorFlip[1]?SENSOR_FLIP_VERTICAL:SENSOR_FLIP_NONE
    );
}

Listener::~Listener()
{
	if(_liveFeed) delete _liveFeed;
	if(_camera) delete _camera;
	if(dpfWriteMap) free(dpfWriteMap);
}

int Listener::updateCameraParameters(ISPC::ParameterList &list)
{
	// Pause live feed
	pauseLiveFeed();

	if(stopCamera() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: stopCamera() failed!\n");
		return IMG_ERROR_FATAL;
	}

	// Store current CCM and AWB
	ISPC::ModuleCCM *ccm = _camera->getModule<ISPC::ModuleCCM>();
    ISPC::ModuleWBC *wbc = _camera->getModule<ISPC::ModuleWBC>();
	ISPC::ControlAWB_PID *awb = _camera->getControlModule<ISPC::ControlAWB_PID>();

    double   aMatrix[9];
	for(int i = 0; i < 9; i++)
		aMatrix[i] = ccm->aMatrix[i];
    double   aOffset[3];
	for(int i = 0; i < 3; i++)
		aOffset[i] = ccm->aOffset[i];
	double aWBGain[4];
	for(int i = 0; i < 4; i++)
		aWBGain[i] = wbc->aWBGain[i];
    double aWBClip[4];
	for(int i = 0; i < 4; i++)
		aWBClip[i] = wbc->aWBClip[i];


	ISPC::ParameterList currentParameterList;
	_camera->saveParameters(currentParameterList, ISPC::ModuleBase::SAVE_VAL);

	ISPC::ParameterList newParameterList;
	_camera->saveParameters(newParameterList, ISPC::ModuleBase::SAVE_VAL);
	
	newParameterList += list;

	if(_camera->loadParameters(newParameterList) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: loadParameters() failed!\n");
		LOG_ERROR("Loading previous parameter list!\n");
		if(_camera->loadParameters(currentParameterList) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: loadParameters() failed!\n");
			return IMG_ERROR_FATAL;
		}
	}

	// If AWB was ON restore previous state
	if(awb->getCorrectionMode() != ISPC::ControlAWB::WB_NONE)
	{
		for(int i = 0; i < 9; i++)
			ccm->aMatrix[i] = aMatrix[i];

		for(int i = 0; i < 3; i++)
			ccm->aOffset[i] = aOffset[i];

		for(int i = 0; i < 4; i++)
			wbc->aWBGain[i] = aWBGain[i];

		for(int i = 0; i < 4; i++)
			wbc->aWBClip[i] = aWBClip[i];

        ccm->requestUpdate();
        wbc->requestUpdate();
	}

	if (_camera->setupModules() != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: setupModules() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	if(_camera->program() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: program() failed!\n");
		return IMG_ERROR_FATAL;
	}

	if(startCamera() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: startCamera() failed!\n");
		return IMG_ERROR_FATAL;
	}

	ISPC::ControlAE *ae = _camera->getControlModule<ISPC::ControlAE>();
	ISPC::ControlAF *af = _camera->getControlModule<ISPC::ControlAF>();

	if(!ae || !af) return IMG_ERROR_FATAL;

	// SENSOR
	if(!ae->isEnabled()) // Don't apply those params when AE is enabled
	{
		if(newParameterList.exists("SENSOR_SET_EXPOSURE"))
		    _camera->getSensor()->setExposure(newParameterList.getParameter("SENSOR_SET_EXPOSURE")->get<int>());
		if(newParameterList.exists("SENSOR_SET_GAIN"))
		    _camera->getSensor()->setGain(newParameterList.getParameter("SENSOR_SET_GAIN")->get<float>());
	}

	// AF
	if(!af->isEnabled()) // Don't apply those params when AF is enabled or
	{
		if(newParameterList.exists("AF_DISTANCE"))
			_camera->getSensor()->setFocusDistance(newParameterList.getParameter("AF_DISTANCE")->get<int>());
	}

	// Resume live feed if paused
	resumeLiveFeed();

	return IMG_SUCCESS;
}

void Listener::pauseLiveFeed()
{
	if(!_liveFeed) return;

	if(_liveFeed->isRunning())
	{
		_liveFeed->pause();
		while(_liveFeed->isRunning()) {YIELD}
		LOG_DEBUG("Live feed paused...\n");
	}
}

void Listener::resumeLiveFeed()
{
	if(!_liveFeed) return;

	if(!_liveFeed->isRunning() && _liveFeed->isPaused())
	{
		_liveFeed->resume();
		while(!_liveFeed->isRunning()) {YIELD}
		LOG_DEBUG("Live feed working...\n");
	}
}

void Listener::stopLiveFeed()
{
	if(!_liveFeed) return;

	if(_liveFeed->isRunning())
	{
		_liveFeed->stop();
		while(_liveFeed->isRunning()) {YIELD}
	}
}

int Listener::initCamera()
{
	ISPC::Sensor *sensor = NULL;
	if(strcmp(_sensor, IIFDG_SENSOR_INFO_NAME) == 0)
	{
		_dataGen = INT_DG;
            
        if(!ISPC::DGCamera::supportIntDG())
        {
            std::cerr << "ERROR trying to use internal data generator but it is not supported by HW!\n" << std::endl;
			return IMG_ERROR_FATAL;
        }
            
		_camera = new ISPC::DGCamera(0, _DGFrameFile, _gasket, true);
                        
		if(_camera->state != ISPC::CAM_ERROR)
		{
			sensor = _camera->getSensor();	
		}
		    
        // Apply additional parameters
		if(sensor)
		{
			IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), _nBuffers);
			IIFDG_ExtendedSetBlanking(sensor->getHandle(), _aBlanking[0], _aBlanking[1]);
		}

		// Sensor should be 0 for populateCameraFromHWVersion
		sensor = NULL;
	}
#ifdef EXT_DATAGEN
	else if(strcmp(_sensor, EXTDG_SENSOR_INFO_NAME) == 0)
	{
		_dataGen = EXT_DG;
            
        if(!ISPC::DGCamera::supportExtDG())
        {
            LOG_ERROR("Error: trying to use external data generator but it is not supported by HW!\n");
            return IMG_ERROR_FATAL;
        }
            
        _camera = new ISPC::DGCamera(0, _DGFrameFile, _gasket, false);

		if(_camera->state != ISPC::CAM_ERROR)
		{
			sensor = _camera->getSensor();
		}
            
        // Apply additional parameters
		if(sensor)
		{
			DGCam_ExtendedSetBlanking(sensor->getHandle(), _aBlanking[0], _aBlanking[1]);
		}
          
		// Sensor should be 0 for populateCameraFromHWVersion
        sensor = NULL;
	}
#endif
	else
	{
		_dataGen = NO_DG;

#ifdef INFOTM_ISP
        _camera = new ISPC::Camera(0, ISPC::Sensor::GetSensorId(_sensor), _sensorMode, _sensorFlip, 0);
#else
        _camera = new ISPC::Camera(0, ISPC::Sensor::GetSensorId(_sensor), _sensorMode, _sensorFlip);
#endif

		if(_camera->state != ISPC::CAM_ERROR)
		{
			sensor = _camera->getSensor();
		}
	}

	if(!_camera || _camera->state == ISPC::CAM_ERROR)
    {
        LOG_ERROR("Error: failed to create camera correctly!\n");
        delete _camera;
        _camera = NULL;
        return IMG_ERROR_FATAL;
    }
    
    // Store initial Gamma LUT
    _GMA_initial = _camera->getConnection()->sGammaLUT;

	if(ISPC::CameraFactory::populateCameraFromHWVersion(*_camera, sensor) != IMG_SUCCESS)
	{
		delete _camera;
		_camera = NULL;
		LOG_ERROR("Error: hw not found!\n");
		return IMG_ERROR_FATAL;
	}

	if(registerControlModules() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: registerControlModules() failed!\n");
		return IMG_ERROR_FATAL;
	}

	if(_camera->loadParameters(_paramsList) != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: loadParameters() failed!\n");
	   return IMG_ERROR_FATAL;
	}
	if(_camera->loadControlParameters(_paramsList) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: loadControlParameters() failed!\n");
		return IMG_ERROR_FATAL;
	}

	// Set defaults for live feed
	ISPC::ModuleOUT *out = _camera->getModule<ISPC::ModuleOUT>();
	out->encoderType = YVU_420_PL12_8;
	out->displayType = RGB_888_32;
	out->dataExtractionType = PXL_NONE;
	out->hdrExtractionType = PXL_NONE;
	out->raw2DExtractionType = PXL_NONE;

	//ISPC::ParameterList tmp;
	//_camera->saveParameters(tmp, ISPC::ModuleBase::SAVE_VAL);
	//tmp.save("before.txt", true);

	if (_camera->setupModules() != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: setupModules() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	if (_camera->program() != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: program() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	if(startCamera() != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: startCamerafailed() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int Listener::startCamera()
{

	if(_camera->allocateBufferPool(_nBuffers, bufferIds) != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: allocateBufferPool() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	if(_camera->startCapture() != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: failed to start the capture!\n");
	   return IMG_ERROR_FATAL;
	}

	for(int i = 0; i < _nBuffers - 1; i++)
	{
		if(_camera->enqueueShot() != IMG_SUCCESS)
		{
		   LOG_ERROR("Error: failed to enque shot\n");
		   return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}

int Listener::stopCamera()
{
	ISPC::Shot tmpFrame;

	for(int i = 0; i < _nBuffers - 1; i++)
	{
		if(_camera->acquireShot(tmpFrame) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: acquireShot() failed!\n");
			return IMG_ERROR_FATAL;
		}

		if(_camera->releaseShot(tmpFrame) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: acquireShot() failed!\n");
			return IMG_ERROR_FATAL;
		}
	}

	if(_camera->stopCapture())
	{
		LOG_ERROR("Error: failed to stop capturing!\n");
		return IMG_ERROR_FATAL;
	}

	LOG_DEBUG("BufferIds size %d\n", bufferIds.size());
	std::list<IMG_UINT32>::iterator it;
    for(it = bufferIds.begin(); it != bufferIds.end(); it++)
    {
		LOG_DEBUG("Deregister buffer %d\n", *it);
		if(_camera->deregisterBuffer(*it) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: deregisterBuffer() failed on %d buffer!\n", *it);
			return EXIT_FAILURE;
		}
	}
    bufferIds.clear();

	if(_camera->getPipeline()->deleteShots() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: deleteShots() failed!\n");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int Listener::takeShot(ISPC::Shot &frame)
{
	if(_camera->state != ISPC::CAM_CAPTURING)
	{
		LOG_ERROR("Error: camera is not in state CAM_CAPTURING!\n");
		return IMG_ERROR_FATAL;
	}

	if(_camera->enqueueShot() != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: enqueueShot() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	if(_camera->acquireShot(frame) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: acquireShot() failed!\n");
		return IMG_ERROR_FATAL;
	}

	// LF
	_missedFrames = frame.iMissedFrames;

	// DPF
	dpfStats = frame.metadata.defectiveStats;
	
	free(dpfWriteMap); // Clean old dpfWriteMaps
	dpfWriteMap_nDefects = 0;
	dpfWriteMap = (IMG_UINT8 *)malloc(frame.DPF.size); // Allocate memory for new dpfWriteMap
	memcpy(dpfWriteMap, frame.DPF.data, frame.DPF.size); // Copy received map to new dpfWriteMap
	dpfWriteMap_nDefects = frame.DPF.size/frame.DPF.elementSize; // Set number of defects

	// ENS
	ensStats = frame.ENS;

	return IMG_SUCCESS;
}

int Listener::releaseShot(ISPC::Shot &frame)
{
	if(_camera->releaseShot(frame) != IMG_SUCCESS)
	{
	   LOG_ERROR("Error: releaseShot() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int Listener::registerControlModules()
{
	ISPC::ControlAE *ae = new ISPC::ControlAE();
	if(_camera->registerControlModule(ae) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: registerControlModule() AE failed!\n");
		return IMG_ERROR_FATAL;
	}
	ae->enableControl(false);
	
	ISPC::ControlAWB_PID *awb = new ISPC::ControlAWB_PID();
	if(_camera->registerControlModule(awb) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: registerControlModule() AWB failed!\n");
		return IMG_ERROR_FATAL;
	}
    awb->setCorrectionMode(ISPC::ControlAWB::WB_NONE);
	awb->enableControl(false);
	//awb->setResetStates(false);

	ISPC::ControlAF *af = new ISPC::ControlAF();
	if(_camera->registerControlModule(af) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: registerControlModule() AF failed!\n");
		return IMG_ERROR_FATAL;
	}
	af->enableControl(false);

	ISPC::ControlTNM *tnm = new ISPC::ControlTNM();
	if(_camera->registerControlModule(tnm) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: registerControlModule() TNM failed!\n");
		return IMG_ERROR_FATAL;
	}
	tnm->enableControl(false);

	ISPC::ControlLBC *lbc = new ISPC::ControlLBC();
	if(_camera->registerControlModule(lbc) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: registerControlModule() LBC failed!\n");
		return IMG_ERROR_FATAL;
	}
	lbc->enableControl(false);

	ISPC::ControlDNS *dns = new ISPC::ControlDNS();
	if(_camera->registerControlModule(dns) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: registerControlModule() DNS failed!\n");
		return IMG_ERROR_FATAL;
	}
	dns->enableControl(false);

	return IMG_SUCCESS;
}

int Listener::startConnection()
{
	LOG_DEBUG("Connecting to server...\n");

	// Connect cmdSocket
	_cmdSocket = ParamSocketClient::connect_to_server(_ip, _port, _appName);
	if(_cmdSocket == NULL)
	{
		LOG_ERROR("Error while connecting to server!\n");
		return IMG_ERROR_FATAL;
	}
	else
	{
		LOG_DEBUG("Successfully connected CMD to server!\n");
	}

	// Connect cmdSocket
	_feedSocket = ParamSocketClient::connect_to_server(_ip, _port + 1, _appName);
	if(_feedSocket == NULL)
	{
		LOG_ERROR("Error while connecting to server!\n");
		return IMG_ERROR_FATAL;
	}
	else
	{
		LOG_DEBUG("Successfully connected FEED to server!\n");
	}

	return IMG_SUCCESS;
}

void Listener::run()
{
	// Start live feed
	_liveFeed = new FeedSender(this);
	_liveFeed->start();

	_continueRun = true;

	IMG_UINT32 cmd;
    size_t nRead;

	while(_continueRun)
	{
		if(_cmdSocket)
		{
			//LOG_DEBUG("Waiting for command...\n");
			if(_cmdSocket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) == IMG_SUCCESS)
			{
				//LOG_DEBUG("Command received!\n");
				switch(ntohl(cmd))
				{
				case GUICMD_QUIT:
					//LOG_DEBUG("CMD_QUIT\n");
					_continueRun = false;
					break;
				case GUICMD_GET_HWINFO:
					//LOG_DEBUG("GUICMD_GET_HWINFO\n");
					if(HWINFO_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: HWINFO_send() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_PARAMETER_LIST:
					//LOG_DEBUG("GUICMD_SET_PARAMETER_LIST\n");
					if(receiveParameterList(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: receiveParameterList() failed!\n");
						_continueRun = false;
					}
					if(updateCameraParameters(_paramsList) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: updateCameraParameters() failed!\n");
						_continueRun = false;
					}
					break;
				//case GUICMD_GET_PARAMETER_LIST:
				//	//LOG_DEBUG("GUICMD_GET_PARAMETER_LIST\n");
				//	if(sendParameterList(*_cmdSocket) != IMG_SUCCESS)
				//	{
				//		LOG_ERROR("Error: sendParameterList() failed!\n");
				//		_continueRun = false;
				//	}
				//	break;
				//case GUICMD_GET_IMAGE:
				//	//LOG_DEBUG("GUICMD_GET_IMAGE\n");
				//	if(sendImage(*_cmdSocket, false, _sendIMG) != IMG_SUCCESS)
				//	{
				//		LOG_ERROR("Error: sendImage() failed!\n");
				//		_continueRun = false;
				//	}
				//	break;
				//case GUICMD_SET_IMAGE_RECORD:
				//	//LOG_DEBUG("GUICMD_SET_IMAGE_RECORD\n");
				//	if(record(*_cmdSocket) != IMG_SUCCESS)
				//	{
				//		LOG_ERROR("Error: record() failed!\n");
				//		_continueRun = false;
				//	}
				//	break;
				case GUICMD_GET_SENSOR:
					//LOG_DEBUG("GUICMD_GET_SENSOR\n");
					if(SENSOR_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: SENSOR_send() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_AE:
					//LOG_DEBUG("GUICMD_GET_AE\n");
					if(AE_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: AE_send() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_AE_ENABLE:
					//LOG_DEBUG("GUICMD_SET_AE_ENABLE\n");
					if(AE_enable(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: AE_enable() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_AWB:
					//LOG_DEBUG("GUICMD_GET_AWB\n");
					if(AWB_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: sendAWB() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_AWB_MODE:
					//LOG_DEBUG("GUICMD_SET_AWB_MODE\n");
					if(AWB_changeMode(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: AWB_changeMode() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_AF:
					//LOG_DEBUG("GUICMD_GET_AF\n");
					if(AF_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: sendAF() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_AF_ENABLE:
					//LOG_DEBUG("GUICMD_SET_AF_ENABLE\n");
					if(AF_enable(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: AF_enable() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_AF_TRIG:
					//LOG_DEBUG("GUICMD_SET_AF_TRIG\n");
					if(AF_triggerSearch(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: AF_triggerSearch() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_TNM:
					//LOG_DEBUG("GUICMD_GET_TNM\n");
					if(TNM_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: TNM_send() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_TNM_ENABLE:
					//LOG_DEBUG("GUICMD_SET_TNM_ENABLE\n");
					if(TNM_enable(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: TNM_enable() failed!\n");
						_continueRun = false;
					}
					break;
				//case GUICMD_SET_LF_ENABLE:
				//	//LOG_DEBUG("GUICMD_SET_LF_ENABLE\n");
				//	if(LF_enable(*_cmdSocket) != IMG_SUCCESS)
				//	{
				//		LOG_ERROR("Error: LF_enable() failed!\n");
				//		_continueRun = false;
				//	}
				//	break;
				//case GUICMD_SET_LF_FORMAT:
				//	//LOG_DEBUG("GUICMD_SET_LF_FORMAT\n");
				//	if(LF_setFormat(*_cmdSocket) != IMG_SUCCESS)
				//	{
				//		LOG_ERROR("Error: LF_setFormat() failed!\n");
				//		_continueRun = false;
				//	}
				//	break;
				//case GUICMD_GET_LF:
				//	//LOG_DEBUG("GUICMD_GET_FEED\n");
				//	if(LF_send(*_cmdSocket) != IMG_SUCCESS)
				//	{
				//		LOG_ERROR("Error: LF_send() failed!\n");
				//		_continueRun = false;
				//	}
				//	break;
				case GUICMD_GET_DPF:
					//LOG_DEBUG("GUICMD_GET_DPF\n");
					if(DPF_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: DPF_send() failed!\n");
						_continueRun = false;
					}
					break;
				//case GUICMD_GET_DPF_MAP:
				//	//LOG_DEBUG("GUICMD_GET_DPF_MAP\n");
				//	if(DPF_map_send(*_cmdSocket) != IMG_SUCCESS)
				//	{
				//		LOG_ERROR("Error: DPF_map_send() failed!\n");
				//		_continueRun = false;
				//	}
				//	break;
				case GUICMD_SET_DPF_MAP:
					//LOG_DEBUG("GUICMD_SET_DPF_MAP\n");
					if(DPF_map_apply(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: DPF_map_apply() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_LSH_GRID:
					//LOG_DEBUG("GUICMD_SET_LSH_GRID\n");
					if(LSH_updateGrid(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: LSH_updateGrid() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_LBC:
					//LOG_DEBUG("GUICMD_GET_LBC\n");
					if(LBC_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: LBC_send() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_LBC_ENABLE:
					//LOG_DEBUG("GUICMD_SET_LBC_ENABLE\n");
					if(LBC_enable(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: LBC_enable() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_DNS_ENABLE:
					//LOG_DEBUG("GUICMD_SET_DNS_ENABLE\n");
					if(DNS_enable(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: DNS_enable() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_ENS:
					//LOG_DEBUG("GUICMD_GET_ENS\n");
					if(ENS_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: ENS_enable() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_GMA:
					//LOG_DEBUG("GUICMD_GET_GMA\n");
					if(GMA_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: GMA_send() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_SET_GMA:
					//LOG_DEBUG("GUICMD_SET_GMA\n");
					if(GMA_set(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: GMA_set() failed!\n");
						_continueRun = false;
					}
					break;
				case GUICMD_GET_FB:
					//LOG_DEBUG("GUICMD_GET_FB\n");
					if(FB_send(*_cmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: FB_send() failed!\n");
						_continueRun = false;
					}
					break;
				default: 
					LOG_ERROR("Error: Unrecognized command!\n");
					_continueRun = false;
					break;
				}
			}
			else
			{
				LOG_ERROR("Error: failed to read from socket!\n");
				_continueRun = false;
				break;
			}
		}
		else 
		{
			_continueRun = false;
			break;
		}
	}

	if(cleanUp() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: cleanUp() failed!\n");
	}

	LOG_DEBUG("Listening stopped!\n");
}

//
// HWINFO
//

int Listener::HWINFO_send(ParamSocket &socket)
{
	std::vector<std::string> HWINFO_params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.ui8GroupId); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.ui8AllocationId); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.ui16ConfigId); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.rev_ui8Designer); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.rev_ui8Major); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.rev_ui8Minor); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.rev_ui8Maint); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.rev_uid); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui8PixPerClock); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui8Parallelism); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui8BitDepth); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui8NContexts); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui8NImagers); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui8NIIFDataGenerator); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui16MemLatency); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.config_ui32DPFInternalSize); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.scaler_ui8EncHLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.scaler_ui8EncVLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.scaler_ui8EncVChromaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.scaler_ui8DispHLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.scaler_ui8DispVLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	for(int i = 0; i < CI_N_IMAGERS; i++)
	{
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.gasket_aImagerPortWidth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.gasket_aType[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.gasket_aMaxWidth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.gasket_aBitDepth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	for(int i = 0; i < CI_N_CONTEXT; i++)
	{
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.context_aMaxWidthMult[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.context_aMaxHeight[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.context_aMaxWidthSingle[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.context_aBitDepth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", _camera->getConnection()->sHWInfo.context_aPendingQueue[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.ui32MaxLineStore); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.eFunctionalities); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.uiSizeOfPipeline); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.uiChangelist); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.uiTiledScheme); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.uiMMUEnabled); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.ui32MMUPageSize); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", _camera->getConnection()->sHWInfo.ui32RefClockMhz); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));


	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(HWINFO_params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < HWINFO_params.size(); i++)
	{
		int paramLength = HWINFO_params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, HWINFO_params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	return IMG_SUCCESS;
}

//
// ParameterList
//

int Listener::receiveParameterList(ParamSocket &socket)
{
	ISPC::ParameterList newParamsList;

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	bool continueRunning = true;

	while(continueRunning)
	{
		if(socket.read(cmd, 2*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Failed to read the socket!\n");
			return IMG_ERROR_FATAL;
		}

		switch(ntohl(cmd[0]))
		{
		case GUICMD_SET_PARAMETER_NAME:
			{
				std::string paramName;
				std::vector<std::string> paramValues;

				bool readingValues = true;

				if(socket.read(buff, ntohl(cmd[1]), nRead, N_TRIES) != IMG_SUCCESS)
				{
					LOG_ERROR("Error: failed to read from socket!\n");
					return IMG_ERROR_FATAL;
				}
				buff[nRead] = '\0';
				paramName = std::string(buff);
				memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

				while(readingValues)
				{
					if(socket.read(cmd, 2*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
					{
						LOG_ERROR("Error: failed to read from socket!\n");
						return IMG_ERROR_FATAL;
					}

					switch(ntohl(cmd[0]))
					{
					case GUICMD_SET_PARAMETER_VALUE:
						if(socket.read(buff, ntohl(cmd[1]), nRead, N_TRIES) != IMG_SUCCESS)
						{
							LOG_ERROR("Error: failed to read from socket!\n");
							return IMG_ERROR_FATAL;
						}
						buff[nRead] = '\0';
						paramValues.push_back(std::string(buff));
						memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
						break;
					case GUICMD_SET_PARAMETER_END:
						readingValues = false;
						break;
					default:
						LOG_ERROR("Unrecognized command received!\n");
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
			LOG_DEBUG("GUICMD_SET_PARAMETER_LIST_END\n");
			continueRunning = false;
			_paramsList = newParamsList;
			break;
		default:
			LOG_ERROR("Unrecognized command received!\n");
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}

int Listener::sendParameterList(ParamSocket &socket)
{
	ISPC::ParameterList listToSend;
	_camera->saveParameters(listToSend, ISPC::ModuleBase::SAVE_VAL);
	_paramsList += listToSend;

	// Add Sensog params to parameter list
	char intBuff[10];
    const ISPC::Sensor *sensor = _camera->getSensor();
	sprintf(intBuff, "%d", (int)(sensor->getExposure()));
	ISPC::Parameter exposure("SENSOR_CURRENT_EXPOSURE", std::string(intBuff));
	listToSend.addParameter(exposure);
	sprintf(intBuff, "%f", (sensor->getGain()));
	ISPC::Parameter gain("SENSOR_CURRENT_GAIN",  std::string(intBuff));
	listToSend.addParameter(gain);
	sprintf(intBuff, "%d", (int)(sensor->getFocusDistance()));
	ISPC::Parameter focus("SENSOR_CURRENT_FOCUS", std::string(intBuff));
	listToSend.addParameter(focus);

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	std::string param;

	cmd[0] = htonl(GUICMD_SET_PARAMETER_LIST);
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	std::map<std::string, ISPC::Parameter>::const_iterator it;
	for(it = listToSend.begin(); it != listToSend.end(); it++)
	{
		param = it->first;

		// Send command informing of upcomming parameter name
		cmd[0] = htonl(GUICMD_SET_PARAMETER_NAME);
		cmd[1] = htonl(param.length());
		if(socket.write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}

		// Send parameter name
		strcpy(buff, param.c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

		for(unsigned int i = 0; i < it->second.size(); i++)
		{
			param = it->second.getString(i);

			// Send command informing of upcomming parameter value
			cmd[0] = htonl(GUICMD_SET_PARAMETER_VALUE);
			cmd[1] = htonl(param.length());
			if(socket.write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
			{
				LOG_ERROR("Error: failed to write to socket!\n");
				return IMG_ERROR_FATAL;
			}

			// Send parameter value
			strcpy(buff, param.c_str());
			if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
			{
				LOG_ERROR("Error: failed to write to socket!\n");
				return IMG_ERROR_FATAL;
			}
			memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		}

		// Send command informing of upcomming parameter value
		cmd[0] = htonl(GUICMD_SET_PARAMETER_END);
		cmd[1] = htonl(0);
		if(socket.write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}

	// Send ending commend
	cmd[0] = htonl(GUICMD_SET_PARAMETER_LIST_END);
	cmd[1] = htonl(0);
	if(socket.write(cmd, 2*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

//
// SENSOR
//

int Listener::SENSOR_send(ParamSocket &socket)
{
    const ISPC::Sensor *sensor = _camera->getSensor();

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "Exposure %ld", sensor->getExposure()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MinExposure %ld", sensor->getMinExposure()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MaxExposure %ld", sensor->getMaxExposure()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Gain %f", sensor->getGain()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MinGain %f", sensor->getMinGain()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MaxGain %f", sensor->getMaxGain()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus %d", sensor->getFocusDistance()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MinFocus %d", sensor->getMinFocus()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MaxFocus %d", sensor->getMaxFocus()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FocusSupported %d", sensor->getFocusSupported()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Width %d", sensor->uiWidth); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Height %d", sensor->uiHeight); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	return IMG_SUCCESS;
}

//
// AE
//

int Listener::AE_send(ParamSocket &socket)
{
	ISPC::ControlAE *ae = _camera->getControlModule<ISPC::ControlAE>();

	if(!ae) return IMG_ERROR_FATAL;

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "MeasuredBrightness %f", ae->getCurrentBrightness()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	return IMG_SUCCESS;
}

int Listener::AE_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::ControlLBC *lbc = _camera->getControlModule<ISPC::ControlLBC>();
	ISPC::ControlAE *ae = _camera->getControlModule<ISPC::ControlAE>();
	ISPC::ControlTNM *tnm = _camera->getControlModule<ISPC::ControlTNM>();

	if(!ae || !tnm || !lbc) return IMG_ERROR_FATAL;

	//ae->enableControl(!ae->isEnabled());
	ae->enableControl((state > 0));
	tnm->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());
	lbc->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());

	return IMG_SUCCESS;
}

//
// AWB
//

int Listener::AWB_send(ParamSocket &socket)
{
	ISPC::ControlAWB_PID *awb = _camera->getControlModule<ISPC::ControlAWB_PID>();
	ISPC::ModuleWBC *wbc = _camera->getModule<ISPC::ModuleWBC>();
	ISPC::ModuleCCM *ccm = _camera->getModule<ISPC::ModuleCCM>();

	if(!awb || !wbc || !ccm) return IMG_ERROR_FATAL;

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "MeasuredTemperature %f", awb->getMeasuredTemperature()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Gains %f %f %f %f", wbc->aWBGain[0], wbc->aWBGain[1], wbc->aWBGain[2], wbc->aWBGain[3]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Clips %f %f %f %f", wbc->aWBClip[0], wbc->aWBClip[1], wbc->aWBClip[2], wbc->aWBClip[3]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Matrix %f %f %f %f %f %f %f %f %f", 
		ccm->aMatrix[0], ccm->aMatrix[1], ccm->aMatrix[2], ccm->aMatrix[3], ccm->aMatrix[4], ccm->aMatrix[5], ccm->aMatrix[6], ccm->aMatrix[7], ccm->aMatrix[8]); 
	params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Offsets %f %f %f", ccm->aOffset[0], ccm->aOffset[1], ccm->aOffset[2]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));


	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	return IMG_SUCCESS;
}

int Listener::AWB_changeMode(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}

    ISPC::ControlAWB::Correction_Types mode = (ISPC::ControlAWB::Correction_Types)ntohl(cmd[0]);

	ISPC::ControlAWB_PID *awb = _camera->getControlModule<ISPC::ControlAWB_PID>();

	if(!awb) return IMG_ERROR_FATAL;

    awb->enableControl(mode != ISPC::ControlAWB::WB_NONE);
	awb->setCorrectionMode(mode);

	return IMG_SUCCESS;
}

//
// AF
//

int Listener::AF_send(ParamSocket &socket)
{
	ISPC::ControlAF *af = _camera->getControlModule<ISPC::ControlAF>();

	if(!af) return IMG_ERROR_FATAL;

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "FocusState %d", af->getState()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FocusScanState %d", af->getScanState()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	if(_focusing)
	{
        if (af->getState() == ISPC::ControlAF::AF_IDLE)
		{
			_camera->getSensor()->setFocusDistance(af->getBestFocusDistance());
			_focusing = false;
		}
	}

	return IMG_SUCCESS;
}

int Listener::AF_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::ControlAF *af = _camera->getControlModule<ISPC::ControlAF>();

	if(!af) return IMG_ERROR_FATAL;

	if(!_focusing)
	{
		//af->enableControl(!af->isEnabled());
		af->enableControl((state > 0));
	}
		
	return IMG_SUCCESS;
}

int Listener::AF_triggerSearch(ParamSocket &socket)
{
	ISPC::ControlAF *af = _camera->getControlModule<ISPC::ControlAF>();

	if(!af) return IMG_ERROR_FATAL;

	if(!_focusing)
	{
        af->setCommand(ISPC::ControlAF::AF_TRIGGER);
		_focusing = true;
	}
		
	return IMG_SUCCESS;
}

//
// TNM
//

int Listener::TNM_send(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(ISPC::ModuleTNM::TNM_CURVE.n);
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	ISPC::ModuleTNM *tnm = _camera->getPipeline()->getModule<ISPC::ModuleTNM>();

	if(!tnm) return IMG_ERROR_FATAL;

	for(unsigned int i = 0; i < ISPC::ModuleTNM::TNM_CURVE.n; i++)
	{
		cmd[0] = htonl(static_cast<u_long>(tnm->aCurve[i]*1000));
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		//printf("%f  ", tnm->aCurve[i]);
	}

	//ISPC::ControlTNM *ctnm = _camera->getControlModule<ISPC::ControlTNM>();
	//ISPC::ControlAE *cae = _camera->getControlModule<ISPC::ControlAE>();
	//printf("\n ------------------------- AE CONFIGURED %s  TNM CONFIGURED %s ---------------------\n", 
	//	(cae->getConfigured())? "YES" : "NO", (ctnm->getAllowHISConfig())? "YES" : "NO");

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "LumaMin %f", tnm->aInY[0]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LumaMax %f", tnm->aInY[1]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Bypass %d", (int)tnm->bBypass); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "ColourConfidance %f", tnm->fColourConfidence); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "ColourSaturation %f", tnm->fColourSaturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FlatFactor %f", tnm->fFlatFactor); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FlatMin %f", tnm->fFlatMin); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WeightLine %f", tnm->fWeightLine); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WeightLocal %f", tnm->fWeightLocal); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	return IMG_SUCCESS;
}

int Listener::TNM_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::ControlLBC *lbc = _camera->getControlModule<ISPC::ControlLBC>();
	ISPC::ControlTNM *tnm = _camera->getControlModule<ISPC::ControlTNM>();
	ISPC::ControlAE *ae = _camera->getControlModule<ISPC::ControlAE>();

	if(!tnm || !ae || !lbc) return IMG_ERROR_FATAL;

	//tnm->enableControl(!tnm->isEnabled());
	tnm->enableControl((state > 0));
	tnm->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());
	lbc->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());

	return IMG_SUCCESS;
}

//
// DPF
//

int Listener::DPF_send(ParamSocket &socket)
{
	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "DroppedMapModifications %u", dpfStats.ui32DroppedMapModifications); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FixedPixels %ld", dpfStats.ui32FixedPixels); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MapModifications %u", dpfStats.ui32MapModifications); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "NOutCorrection %u", dpfStats.ui32NOutCorrection); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Parallelism %d", _camera->getConnection()->sHWInfo.config_ui8Parallelism); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DPFInternalSize %u", _camera->getConnection()->sHWInfo.config_ui32DPFInternalSize); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	return IMG_SUCCESS;
}

int Listener::DPF_map_send(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of defects (indicates size of wright map)
	cmd[0] = htonl(dpfWriteMap_nDefects);
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send wright map
	size_t toSend = dpfWriteMap_nDefects*sizeof(IMG_UINT64);
	size_t w = 0;
	size_t maxChunk = PARAMSOCK_MAX;

	//LOG_DEBUG("Writing dpfMap data %d bytes\n", toSend);
	while(w < toSend)
	{
		if(socket.write(((IMG_UINT8*)dpfWriteMap)+w, (toSend-w > maxChunk)? maxChunk : toSend-w, nWritten, -1) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		//if(w%(PARAMSOCK_MAX) == 0)LOG_DEBUG("dpfMap write %ld/%ld\n", w, toSend);
		w += nWritten;
	}

	return IMG_SUCCESS;
}

int Listener::DPF_map_apply(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nToRead;

	// Receive number of defects (indicates wright map size)
	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_DEBUG("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int nDefects = ntohl(cmd[0]);

	// Receive wright map
	nToRead = nDefects*sizeof(IMG_UINT32);
	IMG_UINT8 *map = 0;
	map = (IMG_UINT8 *)malloc(nToRead);

	size_t toRead = nToRead;
	size_t w = 0;
	size_t maxChunk = PARAMSOCK_MAX;

	// Receive DPF write map
	while(w < toRead)
	{
		if(socket.read(((IMG_UINT8*)map)+w, (toRead-w > maxChunk)? maxChunk : toRead-w, nRead, -1) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		//if(w%PARAMSOCK_MAX == 0)LOG_DEBUG("dpfMap read %ld/%ld\n", w, toRead);
		w += nRead;
	}

	ISPC::ModuleDPF *dpf = _camera->getPipeline()->getModule<ISPC::ModuleDPF>();
	bool previousEnableReadMap = dpf->bRead;

	if(!dpf) return IMG_ERROR_FATAL;

	// Temporary disable usage of DPF read map (to apply new map)
	dpf->bRead = false;
	dpf->setup();

	free(dpf->pDefectMap);
	// Allocate memory for new _receivedDPFWriteMap
	dpf->pDefectMap = (IMG_UINT16 *)malloc(nToRead);
	// Copy received map to new _receivedDPFWriteMap
	memcpy(dpf->pDefectMap, map, nToRead);
	// Free map
	free(map);

	dpf->ui32NbDefects = nDefects;

	// Reenable usage of DPF map (if was enabled before)
	dpf->bRead = previousEnableReadMap;
	dpf->setup();

	return IMG_SUCCESS;
}

//
// LSH
//

int Listener::LSH_updateGrid(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	ISPC::ModuleLSH *lsh = _camera->getPipeline()->getModule<ISPC::ModuleLSH>();
	bool previousEnableGrid = lsh->bEnableMatrix;

	if(!lsh) return IMG_ERROR_FATAL;

	if(socket.read(&cmd, 3*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Temporary disable usage of LSH grid (to apply new grid)
	lsh->bEnableMatrix = false;
	lsh->setup();

	lsh->sGrid.ui16TileSize = (IMG_UINT16)ntohl(cmd[0]);
	lsh->sGrid.ui16Width = (IMG_UINT16)ntohl(cmd[1]);
	lsh->sGrid.ui16Height = (IMG_UINT16)ntohl(cmd[2]);

	// Allocate new memory size for LSH matrix
	for(int channel = 0; channel < 4; channel++)
	{
		free(lsh->sGrid.apMatrix[channel]);
		lsh->sGrid.apMatrix[channel] = (float *)malloc(lsh->sGrid.ui16Width*lsh->sGrid.ui16Height*sizeof(float));
		memset(lsh->sGrid.apMatrix[channel], 0, lsh->sGrid.ui16Width*lsh->sGrid.ui16Height*sizeof(float));
	}

	for(int channel = 0; channel < 4; channel++)
	{
		//lsh->sGrid.apMatrix[channel] = (float *)malloc(lsh->sGrid.ui16Width*lsh->sGrid.ui16Height*sizeof(float));
		if(socket.read(lsh->sGrid.apMatrix[channel], lsh->sGrid.ui16Width*lsh->sGrid.ui16Height*sizeof(float), nRead, -1) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}

	// Reenable usage of LSH grid (if was enabled before)
	lsh->bEnableMatrix = previousEnableGrid;
	lsh->setup();

	return IMG_SUCCESS;
}

//
// LBC
//

int Listener::LBC_send(ParamSocket &socket)
{
	ISPC::ControlLBC *lbc = _camera->getControlModule<ISPC::ControlLBC>();

	if(!lbc) return IMG_ERROR_FATAL;

	/*IMG_UINT32 lightLevel = static_cast<IMG_UINT32>(lbc->getCurrentCorrection().lightLevel*1000);
	IMG_UINT32 brightness = static_cast<IMG_UINT32>(lbc->getCurrentCorrection().brightness*1000);
	IMG_UINT32 contrast = static_cast<IMG_UINT32>(lbc->getCurrentCorrection().contrast*1000);
	IMG_UINT32 saturation = static_cast<IMG_UINT32>(lbc->getCurrentCorrection().saturation*1000);
	IMG_UINT32 sharpness = static_cast<IMG_UINT32>(lbc->getCurrentCorrection().sharpness*1000);
	IMG_UINT32 configurations = lbc->getConfigurationsNumber();
	IMG_UINT32 meteredLightLevel = static_cast<IMG_UINT32>(lbc->getMeteredLightLevel()*1000);*/

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "LightLevel %f", lbc->getCurrentCorrection().lightLevel); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Brightness %f", lbc->getCurrentCorrection().brightness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Contrast %f", lbc->getCurrentCorrection().contrast); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Saturation %f", lbc->getCurrentCorrection().saturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Sharpness %f", lbc->getCurrentCorrection().sharpness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MeasuredLightLevel %f", lbc->getMeteredLightLevel()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}



	/*IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(lightLevel);
	cmd[1] = htonl(brightness);
	cmd[2] = htonl(contrast);
	cmd[3] = htonl(saturation);
	cmd[4] = htonl(sharpness);
	cmd[5] = htonl(configurations);
	cmd[6] = htonl(meteredLightLevel);
	if(socket.write(&cmd, 7*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }*/

	return IMG_SUCCESS;
}

int Listener::LBC_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::ControlLBC *lbc = _camera->getControlModule<ISPC::ControlLBC>();
	ISPC::ControlTNM *tnm = _camera->getControlModule<ISPC::ControlTNM>();
	ISPC::ControlAE *ae = _camera->getControlModule<ISPC::ControlAE>();

	if(!tnm || !ae || !lbc) return IMG_ERROR_FATAL;

	//lbc->enableControl(!lbc->isEnabled());
	lbc->enableControl((state > 0));
	lbc->setAllowHISConfig(!ae->isEnabled() && !tnm->isEnabled() && lbc->isEnabled());
	tnm->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());

	return IMG_SUCCESS;
}

//
// DNS
//

int Listener::DNS_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::ControlDNS *dns = _camera->getControlModule<ISPC::ControlDNS>();

	if(!dns) return IMG_ERROR_FATAL;

	//dns->enableControl(!dns->isEnabled());
	dns->enableControl((state > 0));

	return IMG_SUCCESS;
}

//
// ENS
//

int Listener::ENS_send(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(ensStats.size);
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

//
// GMA
//

int Listener::GMA_send(ParamSocket &socket)
{
	CI_CONNECTION *conn  = _camera->getConnection();
	if(CI_DriverGetGammaLUT(conn) != IMG_SUCCESS)
	{
		return IMG_ERROR_FATAL;
	}

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(conn->sGammaLUT.aRedPoints[i]);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(conn->sGammaLUT.aGreenPoints[i]);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(conn->sGammaLUT.aBluePoints[i]);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}

int Listener::GMA_set(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	CI_MODULE_GMA_LUT data;

	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to read to socket!\n");
			return IMG_ERROR_FATAL;
		}
		data.aRedPoints[i] = static_cast<IMG_UINT16>(ntohl(cmd[0]));
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to read to socket!\n");
			return IMG_ERROR_FATAL;
		}
		data.aGreenPoints[i] = static_cast<IMG_UINT16>(ntohl(cmd[0]));
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to read to socket!\n");
			return IMG_ERROR_FATAL;
		}
		data.aBluePoints[i] = static_cast<IMG_UINT16>(ntohl(cmd[0]));
	}

	// Pause live feed
	pauseLiveFeed();

	// Stop camera
	if(stopCamera() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: stopCamera() failed!\n");
		return IMG_ERROR_FATAL;
	}

	// Replace Gamma LUT
	CI_CONNECTION *conn  = _camera->getConnection();
	if(CI_DriverSetGammaLUT(conn, &data) != IMG_SUCCESS)
	{
		LOG_ERROR("Error: CI_DriverSetGammaLUT failed!\n");
		return IMG_ERROR_FATAL;
	}

	// Stare camera
	if(startCamera() != IMG_SUCCESS)
	{
		LOG_ERROR("Error: startCamera() failed!\n");
		return IMG_ERROR_FATAL;
	}

	// Resume live feed if paused
	resumeLiveFeed();

	return IMG_SUCCESS;
}

//
// TEST
//

int Listener::FB_send(ParamSocket &socket)
{
	// ControlModules
	ISPC::ControlAE *cae = _camera->getControlModule<ISPC::ControlAE>();
	ISPC::ControlAF *caf = _camera->getControlModule<ISPC::ControlAF>();
	ISPC::ControlAWB_PID *cawb = _camera->getControlModule<ISPC::ControlAWB_PID>();
	ISPC::ControlDNS *cdns = _camera->getControlModule<ISPC::ControlDNS>();
	ISPC::ControlLBC *clbc = _camera->getControlModule<ISPC::ControlLBC>();
	ISPC::ControlTNM *ctnm = _camera->getControlModule<ISPC::ControlTNM>();

	// Modules
	ISPC::ModuleOUT *out = _camera->getModule<ISPC::ModuleOUT>();
	ISPC::ModuleESC *esc = _camera->getModule<ISPC::ModuleESC>();
	ISPC::ModuleDSC *dsc = _camera->getModule<ISPC::ModuleDSC>();
	ISPC::ModuleBLC *blc = _camera->getModule<ISPC::ModuleBLC>();
	ISPC::ModuleLSH *lsh = _camera->getModule<ISPC::ModuleLSH>();
	ISPC::ModuleCCM *ccm = _camera->getModule<ISPC::ModuleCCM>();
	ISPC::ModuleWBC *wbc = _camera->getModule<ISPC::ModuleWBC>();
	ISPC::ModuleDNS *dns = _camera->getModule<ISPC::ModuleDNS>();
	ISPC::ModuleSHA *sha = _camera->getModule<ISPC::ModuleSHA>();
	ISPC::ModuleDPF *dpf = _camera->getModule<ISPC::ModuleDPF>();
	ISPC::ModuleR2Y *r2y = _camera->getModule<ISPC::ModuleR2Y>();
	ISPC::ModuleMIE *mie = _camera->getModule<ISPC::ModuleMIE>();
	ISPC::ModuleTNM *tnm = _camera->getModule<ISPC::ModuleTNM>();
	ISPC::ModuleVIB *vib = _camera->getModule<ISPC::ModuleVIB>();
	ISPC::ModuleMGM *mgm = _camera->getModule<ISPC::ModuleMGM>();
	ISPC::ModuleDGM *dgm = _camera->getModule<ISPC::ModuleDGM>();
	ISPC::ModuleENS *ens = _camera->getModule<ISPC::ModuleENS>();
	ISPC::ModuleY2R *y2r = _camera->getModule<ISPC::ModuleY2R>();

	ISPC::Sensor *sensor = _camera->getSensor();

	if(!cae || !caf || !cawb || !cdns || !clbc || !ctnm ||
		!out || !esc || !dsc || !blc || !lsh || !ccm || !wbc || 
		!dns || !sha || !dpf || !r2y || !mie || !tnm || !vib ||
		!mgm || !dgm || !ens || !y2r || /*!tc ||*/ !sensor) 
	{
		return IMG_ERROR_FATAL;
	}

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Calculate number of types in pxlFormat enum
	int encoderTypeCount = 0;
	int displayTypeCount = 0;
	for(int i = PXL_NONE; i < PXL_N; i++)
	{
		std::string formatName = FormatString((pxlFormat)i);
		if(formatName.find("NV") != std::string::npos)
			encoderTypeCount++;
		if(formatName.find("BI") != std::string::npos)
			displayTypeCount++;
	}

	// Minimum image sizes
	int qcifWidth = 144;
	int qcifHeight = 176;
	
	// OutputTab
	sprintf(buff, "OUT_encoderFormat %d", (out->encoderType==0)?0:out->encoderType-0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // EncoderFormat_lb
	sprintf(buff, "OUT_encoderFormat_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_encoderFormat_max %d", encoderTypeCount); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_encoderWidth %d", esc->aRect[2]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // EncoderWidth_sb
	sprintf(buff, "OUT_encoderWidth_min %d", qcifWidth); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_encoderWidth_max %d", sensor->uiWidth); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_encoderHeight %d", esc->aRect[3]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // EncoderHeight_sb
	sprintf(buff, "OUT_encoderHeight_min %d", qcifHeight); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_encoderHeight_max %d", sensor->uiHeight); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_displayFormat %d", (out->displayType==0)?0:out->displayType-encoderTypeCount-((out->displayType<11)?0:1)); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // DisplayFormat_lb
	sprintf(buff, "OUT_displayFormat_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_displayFormat_max %d", 6); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_displayWidth %d", dsc->aRect[2]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // DisplayWidth_sb
	sprintf(buff, "OUT_displayWidth_min %d", qcifWidth); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_displayWidth_max %d", sensor->uiWidth); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_displayHeight %d", dsc->aRect[3]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // DisplayHeight_sb
	sprintf(buff, "OUT_displayHeight_min %d", qcifHeight); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_displayHeight_max %d", sensor->uiHeight); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_dataExtractionPoint %d", out->dataExtractionPoint); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // DEPoint_lb
	sprintf(buff, "OUT_dataExtractionPoint_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_dataExtractionPoint_max %d", 1); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_dataExtractionFormat %d", (out->dataExtractionType==0)?0:out->dataExtractionType-(encoderTypeCount+displayTypeCount)); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // DEFormat_lb
	sprintf(buff, "OUT_dataExtractionFormat_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "OUT_dataExtractionFormat_max %d", 3); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// ExposureTab
	sprintf(buff, "Exposure_Auto %d", cae->isEnabled()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Auto_cb
	sprintf(buff, "Exposure_Flicker %d", cae->getFlickerRejection()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Flicker_cb
	sprintf(buff, "Exposure_FlickerHz %d", (cae->getFlickerRejectionFrequency()==50)?0:1); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // FlickerHz_lb
	sprintf(buff, "Exposure_FlickerHz_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_FlickerHz_max %d", 1); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_TargetBrightness %f", cae->getTargetBrightness()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // TargetBrightness_sb
	sprintf(buff, "Exposure_TargetBrightness_min %f", cae->AE_TARGET_BRIGHTNESS.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_TargetBrightness_max %f", cae->AE_TARGET_BRIGHTNESS.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_BracketSize %f", cae->getTargetBracket()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // BracketSize_sb
	sprintf(buff, "Exposure_BracketSize_min %f", cae->AE_BRACKET_SIZE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_BracketSize_max %f", cae->AE_BRACKET_SIZE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_SensorExposure %lu", (_camera->getSensor()->getExposure())); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SensorExposure_sb
	sprintf(buff, "Exposure_SensorExposure_min %lu", sensor->getMinExposure()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_SensorExposure_max %lu", sensor->getMaxExposure()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_SensorGain %f", _camera->getSensor()->getGain()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SensorGain_sb
	sprintf(buff, "Exposure_SensorGain_min %f", sensor->getMinGain()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_SensorGain_max %f", sensor->getMaxGain()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_UpdateSpeed %f", cae->getUpdateSpeed()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // UpdateSpeed_sb
	sprintf(buff, "Exposure_UpdateSpeed_min %f", cae->AE_UPDATE_SPEED.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Exposure_UpdateSpeed_max %f", cae->AE_UPDATE_SPEED.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// FocusTab
	sprintf(buff, "Focus_Auto %d", caf->isEnabled()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Auto_cb
	sprintf(buff, "Focus_State %d", caf->getState()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // State
	sprintf(buff, "Focus_State_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus_State_max %d", 3); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus_ScanState %d", caf->getScanState()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ScanState
	sprintf(buff, "Focus_ScanState_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus_ScanState_max %d", 6); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus_Distance %d", _camera->getSensor()->getFocusDistance()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Distance_cb
	sprintf(buff, "Focus_Distance_min %d", sensor->getMinFocus()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus_Distance_max %d", sensor->getMaxFocus()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus_CentreWeight %f", caf->getCenterWeight()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // CenterWaight_cb
	sprintf(buff, "Focus_CentreWeight_min %f", caf->AF_CENTER_WEIGTH.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Focus_CentreWeight_max %f", caf->AF_CENTER_WEIGTH.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// BLCTab
	sprintf(buff, "BLC_SensorBlack0 %d", blc->aSensorBlack[0]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SensorBlack0_sb
	sprintf(buff, "BLC_SensorBlack0_min %d", blc->BLC_SENSOR_BLACK.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SensorBlack0_max %d", blc->BLC_SENSOR_BLACK.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SensorBlack1 %d", blc->aSensorBlack[1]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SensorBlack1_sb
	sprintf(buff, "BLC_SensorBlack1_min %d", blc->BLC_SENSOR_BLACK.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SensorBlack1_max %d", blc->BLC_SENSOR_BLACK.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SensorBlack2 %d", blc->aSensorBlack[2]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SensorBlack2_sb
	sprintf(buff, "BLC_SensorBlack2_min %d", blc->BLC_SENSOR_BLACK.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SensorBlack2_max %d", blc->BLC_SENSOR_BLACK.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SensorBlack3 %d", blc->aSensorBlack[3]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SensorBlack3_sb
	sprintf(buff, "BLC_SensorBlack3_min %d", blc->BLC_SENSOR_BLACK.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SensorBlack3_max %d", blc->BLC_SENSOR_BLACK.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SystemBlack %d", blc->ui32SystemBlack); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SystemBlack_sb
	sprintf(buff, "BLC_SystemBlack_min %d", blc->BLC_SYS_BLACK.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "BLC_SystemBlack_max %d", blc->BLC_SYS_BLACK.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// LSHTab
	sprintf(buff, "LSH_GridApplied %d", lsh->hasGrid()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Apply_btn
	sprintf(buff, "LSH_Enable %d", lsh->bEnableMatrix? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Enable_cb

	// WBCTab
	sprintf(buff, "WBC_Mode %d", cawb->getCorrectionMode()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Mode_lb
    sprintf(buff, "WBC_Mode_min %d", ISPC::ControlAWB::WB_NONE); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    sprintf(buff, "WBC_Mode_max %d", ISPC::ControlAWB::WB_COMBINED); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	ISPC::TemperatureCorrection tc;
	ISPC::ParameterList currentParameterList;
	_camera->saveParameters(currentParameterList, ISPC::ModuleBase::SAVE_VAL);
	tc.loadParameters(currentParameterList);
	sprintf(buff, "WBC_CorrectionsCount %d", tc.size()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Configurations count
	sprintf(buff, "WBC_CorrectionsCount_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	for(int c = 0; c < tc.size(); c++)
	{
		ISPC::ColorCorrection cc = tc.getCorrection(c);
		sprintf(buff, "WBC_Correction%d_Temperature %f", c, cc.temperature); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Temperature
		for(int i = 0; i < 4; i++)
		{
			sprintf(buff, "WBC_Correction%d_Gain%d %f", c, i, cc.gains[0][i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // GainX_sb
		}
		sprintf(buff, "WBC_Correction%d_Gain_min %f", c, wbc->WBC_GAIN.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "WBC_Correction%d_Gain_max %f", c, wbc->WBC_GAIN.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				sprintf(buff, "WBC_Correction%d_Matrix%d %f", c, i*3+j, cc.coefficients[i][j]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MatrixX_sb
			}

		}
		sprintf(buff, "WBC_Correction%d_Matrix_min %f", c, ccm->CCM_MATRIX.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "WBC_Correction%d_Matrix_max %f", c, ccm->CCM_MATRIX.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		for(int i = 0; i < 3; i++)
		{
			sprintf(buff, "WBC_Correction%d_Offset%d %f", c, i, cc.offsets[0][i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // OffsetX_sb
		}
		sprintf(buff, "WBC_Correction%d_Offset_min %f", c, ccm->CCM_OFFSETS.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "WBC_Correction%d_Offset_max %f", c, ccm->CCM_OFFSETS.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}
	for(int i = 0; i < 4; i++)
	{
		sprintf(buff, "WBC_Gain%d %f", i, wbc->aWBGain[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // GainX_sb
		sprintf(buff, "WBC_Clip%d %f", i, wbc->aWBClip[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ClipX_sb
	}
	sprintf(buff, "WBC_Gain_min %f", wbc->WBC_GAIN.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WBC_Gain_max %f", wbc->WBC_GAIN.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WBC_Clip_min %f", wbc->WBC_CLIP.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WBC_Clip_max %f", wbc->WBC_CLIP.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	for(int i = 0; i < 9; i++)
	{
		sprintf(buff, "WBC_Matrix%d %f", i, ccm->aMatrix[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MatrixX_sb
	}
	sprintf(buff, "WBC_Matrix_min %f", ccm->CCM_MATRIX.min); params.push_back(std::string(buff));
	sprintf(buff, "WBC_Matrix_max %f", ccm->CCM_MATRIX.max); params.push_back(std::string(buff));
	for(int i = 0; i < 3; i++)
	{
		sprintf(buff, "WBC_Offset%d %f", i, ccm->aOffset[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // OffsetX_sb
	}
	sprintf(buff, "WBC_Offset_min %f", ccm->CCM_OFFSETS.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WBC_Offset_max %f", ccm->CCM_OFFSETS.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	

	// NoiseTab
	sprintf(buff, "Noise_Auto %d", cdns->isEnabled()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Auto_cb
	sprintf(buff, "Noise_PrimaryDenoiser_Strength %f", dns->fStrength); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // PrimaryDenoiser_Strength_sb
	sprintf(buff, "Noise_PrimaryDenoiser_Strength_min %f", dns->DNS_STRENGTH.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_PrimaryDenoiser_Strength_max %f", dns->DNS_STRENGTH.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Radius %f", sha->fRadius); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Sharpening_Radius_cb
	sprintf(buff, "Noise_Sharpening_Radius_min %f", sha->SHA_RADIUS.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Radius_max %f", sha->SHA_RADIUS.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Strength %f", sha->fStrength); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Sharpening_Strength_cb
	sprintf(buff, "Noise_Sharpening_Strength_min %f", sha->SHA_STRENGTH.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Strength_max %f", sha->SHA_STRENGTH.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Treshold %f", sha->fThreshold); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Sharpening_Treshold_cb
	sprintf(buff, "Noise_Sharpening_Treshold_min %f", sha->SHA_THRESH.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Treshold_max %f", sha->SHA_THRESH.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Detail %f", sha->fDetail); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Sharpening_Detail_cb
	sprintf(buff, "Noise_Sharpening_Detail_min %f", sha->SHA_DETAIL.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_Sharpening_Detail_max %f", sha->SHA_DETAIL.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_NoiseSuppresion_EdgeAvoidance %f", sha->fDenoiseTau); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // NoiseSuppresion_EdgeAvoidance_cb
	sprintf(buff, "Noise_NoiseSuppresion_EdgeAvoidance_min %f", sha->SHADN_TAU.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_NoiseSuppresion_EdgeAvoidance_max %f", sha->SHADN_TAU.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_NoiseSuppresion_Strength %f", sha->fDenoiseSigma); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // NoiseSuppresion_Strength_cb
	sprintf(buff, "Noise_NoiseSuppresion_Strength_min %f", sha->SHADN_SIGMA.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Noise_NoiseSuppresion_Strength_max %f", sha->SHADN_SIGMA.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// DPFTab
	sprintf(buff, "DPF_HWDetection %d", dpf->bDetect? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // HWDetection_cb
	sprintf(buff, "DPF_Treshold %d", dpf->ui32Threshold); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Treshold_sb
	sprintf(buff, "DPF_Treshold_min %d", dpf->DPF_THRESHOLD.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DPF_Treshold_max %d", dpf->DPF_THRESHOLD.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DPF_Weight %f", dpf->fWeight); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Weight_sb
	sprintf(buff, "DPF_Weight_min %f", dpf->DPF_WEIGHT.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DPF_Weight_max %f", dpf->DPF_WEIGHT.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DPF_InputMap %d", dpf->bRead? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // InputMap_cb
	sprintf(buff, "DPF_OutputMap %d", dpf->bWrite? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // OutputMap_cb

	// R2YTab
	sprintf(buff, "R2Y_Brightness %f", r2y->fBrightness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Brightness_sb
	sprintf(buff, "R2Y_Brightness_min %f", r2y->R2Y_BRIGHTNESS.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_Brightness_max %f", r2y->R2Y_BRIGHTNESS.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_Contras %f", r2y->fContrast); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Contras_sb
	sprintf(buff, "R2Y_Contras_min %f", r2y->R2Y_CONTRAST.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_Contras_max %f", r2y->R2Y_CONTRAST.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_Saturation %f", r2y->fSaturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Saturation_sb
	sprintf(buff, "R2Y_Saturation_min %f", r2y->R2Y_SATURATION.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_Saturation_max %f", r2y->R2Y_SATURATION.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_Hue %f", r2y->fHue); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Hue_sb
	sprintf(buff, "R2Y_Hue_min %f", r2y->R2Y_HUE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_Hue_max %f", r2y->R2Y_HUE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_ConversionMatrix %d", r2y->eMatrix); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ConversionMatrix_lb
	sprintf(buff, "R2Y_ConversionMatrix_min %d", r2y->BT601); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_ConversionMatrix_max %d", r2y->JFIF); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_MultY %f", r2y->aRangeMult[0]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MultY_sb
	sprintf(buff, "R2Y_MultY_min %f", r2y->R2Y_RANGEMULT.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_MultY_max %f", r2y->R2Y_RANGEMULT.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_MultCb %f", r2y->aRangeMult[1]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MultCb_sb
	sprintf(buff, "R2Y_MultCb_min %f", r2y->R2Y_RANGEMULT.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_MultCb_max %f", r2y->R2Y_RANGEMULT.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_MultCr %f", r2y->aRangeMult[2]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MultCr_sb
	sprintf(buff, "R2Y_MultCr_min %f", r2y->R2Y_RANGEMULT.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "R2Y_MultCr_max %f", r2y->R2Y_RANGEMULT.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	
	// MIETab
	for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
	{	
		sprintf(buff, "MIE_EnableMC%d %d", i, mie->bEnabled[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // EnableMC_cb
		sprintf(buff, "MIE_Minimum%d %f", i, mie->aYMin[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Minimum_sb
		sprintf(buff, "MIE_Minimum%d_min %f", i, mie->MIE_YMIN_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Minimum%d_max %f", i, mie->MIE_YMIN_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Maximum%d %f", i, mie->aYMax[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Maximum_sb
		sprintf(buff, "MIE_Maximum%d_min %f", i, mie->MIE_YMAX_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Maximum%d_max %f", i, mie->MIE_YMAX_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		for(int j = 0; j < 4; j++)
		{
			sprintf(buff, "MIE_Gain%d%d %f", i, j, mie->aYGain[i][j]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // GainX_sb
			sprintf(buff, "MIE_Gain%d%d_min %f", i, j, mie->MIE_YGAINS_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
			sprintf(buff, "MIE_Gain%d%d_max %f", i, j, mie->MIE_YGAINS_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		}
		sprintf(buff, "MIE_Brightness%d %f", i, mie->aOutBrightness[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Brightness_sb
		sprintf(buff, "MIE_Brightness%d_min %f", i, mie->MIE_OUT_BRIGHTNESS_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Brightness%d_max %f", i, mie->MIE_OUT_BRIGHTNESS_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Contrast%d %f", i, mie->aOutContrast[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Contrast_sb
		sprintf(buff, "MIE_Contrast%d_min %f", i, mie->MIE_OUT_CONTRAST_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Contrast%d_max %f", i, mie->MIE_OUT_CONTRAST_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Saturation%d %f", i, mie->aOutStaturation[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Saturation_sb
		sprintf(buff, "MIE_Saturation%d_min %f", i, mie->MIE_OUT_SATURATION_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Saturation%d_max %f", i, mie->MIE_OUT_SATURATION_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Hue%d %f", i, mie->aOutHue[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Hue_sb
		sprintf(buff, "MIE_Hue%d_min %f", i, mie->MIE_OUT_HUE_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Hue%d_max %f", i, mie->MIE_OUT_HUE_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_CbCenter%d %f", i, mie->aCbCenter[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // CbCenter_sb
		sprintf(buff, "MIE_CbCenter%d_min %f", i, -0.25f); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_CbCenter%d_max %f", i, 0.25f); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_CrCenter%d %f", i, mie->aCrCenter[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // CrCenter_sb
		sprintf(buff, "MIE_CrCenter%d_min %f", i, -0.25f); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_CrCenter%d_max %f", i, 0.25f); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Aspect%d %f", i, mie->aCAspect[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Aspect_sb
		sprintf(buff, "MIE_Aspect%d_min %f", i, mie->MIE_CASPECT_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Aspect%d_max %f", i, mie->MIE_CASPECT_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Rotation%d %f", i, mie->aCRotation[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Rotation_sb
		sprintf(buff, "MIE_Rotation%d_min %f", i, mie->MIE_CROTATION_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MIE_Rotation%d_max %f", i, mie->MIE_CROTATION_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		for(int j = 0; j < 4; j++)
		{
			sprintf(buff, "MIE_Extent%d%d %f", i, j, mie->aCExtent[i][j]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ExtentX_sb
			sprintf(buff, "MIE_Extent%d%d_min %f", i, j, mie->MIE_CEXTENT_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
			sprintf(buff, "MIE_Extent%d%d_max %f", i, j, 1.0f); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		}
	}

	// TNMTab
	sprintf(buff, "TNM_Auto %d", ctnm->isEnabled()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Auto_cb
	sprintf(buff, "TNM_Adaptive %d", ctnm->getAdaptiveTNM()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Adaptive_cb
	sprintf(buff, "TNM_LocalTNM %d", ctnm->getLocalTNM()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // LocalTNM_cb
	sprintf(buff, "TNM_MinHist %f", ctnm->getHistMin()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MinHist_sb
	sprintf(buff, "TNM_MinHist_min %f", ctnm->TNMC_HISTMIN.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_MinHist_max %f", ctnm->TNMC_HISTMIN.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_MaxHist %f", ctnm->getHistMax()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MaxHist_sb
	sprintf(buff, "TNM_MaxHist_min %f", ctnm->TNMC_HISTMAX.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_MaxHist_max %f", ctnm->TNMC_HISTMAX.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Tempering %f", ctnm->getTempering()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Tempering_sb
	sprintf(buff, "TNM_Tempering_min %f", ctnm->TNMC_TEMPERING.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Tempering_max %f", ctnm->TNMC_TEMPERING.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Smoothing %f", ctnm->getSmoothing()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Smoothing_sb
	sprintf(buff, "TNM_Smoothing_min %f", ctnm->TNMC_SMOOTHING.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Smoothing_max %f", ctnm->TNMC_SMOOTHING.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LocalStrength %f", ctnm->getLocalStrength()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // LocalStrength_sb
	sprintf(buff, "TNM_LocalStrength_min %f", ctnm->TNMC_LOCAL_STRENGTH.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LocalStrength_max %f", ctnm->TNMC_LOCAL_STRENGTH.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_UpdateSpeed %f", ctnm->getUpdateSpeed()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // UpdateSpeed_sb
	sprintf(buff, "TNM_UpdateSpeed_min %f", ctnm->TNMC_UPDATESPEED.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_UpdateSpeed_max %f", ctnm->TNMC_UPDATESPEED.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LumaRangeMin %f", tnm->aInY[0]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // LumaRangeMin_sb
	sprintf(buff, "TNM_LumaRangeMin_min %f", tnm->TNM_IN_Y.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LumaRangeMin_max %f", tnm->TNM_IN_Y.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LumaRangeMax %f", tnm->aInY[1]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // LumaRangeMax_sb
	sprintf(buff, "TNM_LumaRangeMax_min %f", tnm->TNM_IN_Y.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LumaRangeMax_max %f", tnm->TNM_IN_Y.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_ColourConfidence %f", tnm->fColourConfidence); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ColourConfidence_sb
	sprintf(buff, "TNM_ColourConfidence_min %f", tnm->TNM_COLOUR_CONFIDENCE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_ColourConfidence_max %f", tnm->TNM_COLOUR_CONFIDENCE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Saturation %f", tnm->fColourSaturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Saturation_sb
	sprintf(buff, "TNM_Saturation_min %f", tnm->TNM_COLOUR_SATURATION.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Saturation_max %f", tnm->TNM_COLOUR_SATURATION.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LineUpdateWeight %f", tnm->fWeightLine); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // LineUpdateWeight_sb
	sprintf(buff, "TNM_LineUpdateWeight_min %f", tnm->TNM_WEIGHT_LINE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LineUpdateWeight_max %f", tnm->TNM_WEIGHT_LINE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LocalMappingWeight %f", tnm->fWeightLocal); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // LocalMappingWeight_sb
	sprintf(buff, "TNM_LocalMappingWeight_min %f", tnm->TNM_WEIGHT_LOCAL.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_LocalMappingWeight_max %f", tnm->TNM_WEIGHT_LOCAL.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Flattering %f", tnm->fFlatFactor); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Flattering_sb
	sprintf(buff, "TNM_Flattering_min %f", tnm->TNM_FLAT_FACTOR.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Flattering_max %f", tnm->TNM_FLAT_FACTOR.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_FlatteringMin %f", tnm->fFlatMin); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // FlatteringMin_sb
	sprintf(buff, "TNM_FlatteringMin_min %f", tnm->TNM_FLAT_MIN.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_FlatteringMin_max %f", tnm->TNM_FLAT_MIN.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Bypass %d", tnm->bBypass? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Bypass
	for(int i = 0; i < TNM_CURVE_NPOINTS; i++)
	{
		sprintf(buff, "TNM_Curve%d %f", i, tnm->aCurve[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SaturationCurve
	}
	sprintf(buff, "TNM_Curve_min %f", 0.0f); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "TNM_Curve_max %f", 1.0f); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// VIBTab
	sprintf(buff, "VIB_VibrancyOn %d", vib->bVibrancyOn? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // VibrancyOn_cb
	for(int i = 0; i < 32; i++)
	{
		sprintf(buff, "VIB_SaturationCurve%d %f", i, vib->aSaturationCurve[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SaturationCurve
	}
	sprintf(buff, "VIB_SaturationCurve_min %f", vib->VIB_SATURATION_CURVE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "VIB_SaturationCurve_max %f", vib->VIB_SATURATION_CURVE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// LBCTab
	sprintf(buff, "LBC_Auto %d", clbc->isEnabled()? 1 : 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Auto_cb
	sprintf(buff, "LBC_UpdateSpeed %f", clbc->getUpdateSpeed()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // UpdateSpeed_sb
	sprintf(buff, "LBC_UpdateSpeed_min %f", clbc->LBC_UPDATE_SPEED.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_UpdateSpeed_max %f", clbc->LBC_UPDATE_SPEED.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_ConfigurationsNumber %d", clbc->getConfigurationsNumber()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ConfigurationsNumber
	sprintf(buff, "LBC_ConfigurationsNumber_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	for(int c = 0; c < clbc->getConfigurationsNumber(); c++)
	{
		ISPC::LightCorrection lc = clbc->getCorrection(c);
		sprintf(buff, "LBC_Correction%d_LightLevel %f", c, lc.lightLevel); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Light Level
		sprintf(buff, "LBC_Correction%d_Brightness %f", c, lc.brightness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Brightness
		sprintf(buff, "LBC_Correction%d_Contrast %f", c, lc.contrast); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Contrast
		sprintf(buff, "LBC_Correction%d_Saturation %f", c, lc.saturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Saturation
		sprintf(buff, "LBC_Correction%d_Sharpness %f", c, lc.sharpness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Sharpness
	}
	sprintf(buff, "LBC_LightLevel_min %f", clbc->LBC_LIGHT_LEVEL_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_LightLevel_max %f", clbc->LBC_LIGHT_LEVEL_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Brightness_min %f", clbc->LBC_BRIGHTNESS_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Brightness_max %f", clbc->LBC_BRIGHTNESS_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Contrast_min %f", clbc->LBC_CONTRAST_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Contrast_max %f", clbc->LBC_CONTRAST_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Saturation_min %f", clbc->LBC_SATURATION_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Saturation_max %f", clbc->LBC_SATURATION_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Sharpness_min %f", clbc->LBC_SHARPNESS_S.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LBC_Sharpness_max %f", clbc->LBC_SHARPNESS_S.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// MGMTab
	for(int i = 0; i < 6; i++)
	{
		sprintf(buff, "MGM_Coeff%d %f", i, mgm->aCoeff[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Coeffs
		sprintf(buff, "MGM_Coeff%d_min %f", i, mgm->MGM_COEFF.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MGM_Coeff%d_max %f", i, mgm->MGM_COEFF.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}
	for(int i = 0; i < 3; i++)
	{
		sprintf(buff, "MGM_Slope%d %f", i, mgm->aSlope[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Slopes
		sprintf(buff, "MGM_Slope%d_min %f", i, mgm->MGM_SLOPE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "MGM_Slope%d_max %f", i, mgm->MGM_SLOPE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}
	sprintf(buff, "MGM_ClipMin %f", mgm->fClipMin); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ClipMin
	sprintf(buff, "MGM_ClipMin_min %f", mgm->MGM_CLIP_MIN.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MGM_ClipMin_max %f", mgm->MGM_CLIP_MIN.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MGM_SrcNorm %f", mgm->fSrcNorm); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SrcNorm
	sprintf(buff, "MGM_SrcNorm_min %f", mgm->MGM_SRC_NORM.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MGM_SrcNorm_max %f", mgm->MGM_SRC_NORM.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MGM_ClipMax %f", mgm->fClipMax); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ClipMax
	sprintf(buff, "MGM_ClipMax_min %f", mgm->MGM_CLIP_MAX.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MGM_ClipMax_max %f", mgm->MGM_CLIP_MAX.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// DGMTab
	for(int i = 0; i < 6; i++)
	{
		sprintf(buff, "DGM_Coeff%d %f", i, dgm->aCoeff[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Coeffs
		sprintf(buff, "DGM_Coeff%d_min %f", i, dgm->DGM_COEFF.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "DGM_Coeff%d_max %f", i, dgm->DGM_COEFF.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}
	for(int i = 0; i < 3; i++)
	{
		sprintf(buff, "DGM_Slope%d %f", i, dgm->aSlope[i]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Slopes
		sprintf(buff, "DGM_Slope%d_min %f", i, dgm->DGM_SLOPE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "DGM_Slope%d_max %f", i, dgm->DGM_SLOPE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}
	sprintf(buff, "DGM_ClipMin %f", dgm->fClipMin); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ClipMin
	sprintf(buff, "DGM_ClipMin_min %f", dgm->DGM_CLIP_MIN.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DGM_ClipMin_max %f", dgm->DGM_CLIP_MIN.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DGM_SrcNorm %f", dgm->fSrcNorm); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // SrcNorm
	sprintf(buff, "DGM_SrcNorm_min %f", dgm->DGM_SRC_NORM.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DGM_SrcNorm_max %f", dgm->DGM_SRC_NORM.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DGM_ClipMax %f", dgm->fClipMax); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ClipMax
	sprintf(buff, "DGM_ClipMax_min %f", dgm->DGM_CLIP_MAX.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DGM_ClipMax_max %f", dgm->DGM_CLIP_MAX.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// ENSTab
	sprintf(buff, "ENS_Enable %d", ens->bEnable?1:0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ENS_Enable_cb
	sprintf(buff, "ENS_NLines %d", ens->ui32NLines); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ENS_NLines_sb
	sprintf(buff, "ENS_NLines_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "ENS_NLines_max %d", 5); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "ENS_SubSampling %d", ens->ui32SubsamplingFactor); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ENS_SubSampling_sb
	sprintf(buff, "ENS_SubSampling_min %d", 0); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "ENS_SubSampling_max %d", 5); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	// Y2RTab
	sprintf(buff, "Y2R_Brightness %f", y2r->fBrightness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Brightness_sb
	sprintf(buff, "Y2R_Brightness_min %f", y2r->Y2R_BRIGHTNESS.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_Brightness_max %f", y2r->Y2R_BRIGHTNESS.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_Contras %f", y2r->fContrast); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Contras_sb
	sprintf(buff, "Y2R_Contras_min %f", y2r->Y2R_CONTRAST.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_Contras_max %f", y2r->Y2R_CONTRAST.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_Saturation %f", y2r->fSaturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Saturation_sb
	sprintf(buff, "Y2R_Saturation_min %f", y2r->Y2R_SATURATION.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_Saturation_max %f", y2r->Y2R_SATURATION.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_Hue %f", y2r->fHue); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // Hue_sb
	sprintf(buff, "Y2R_Hue_min %f", y2r->Y2R_HUE.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_Hue_max %f", y2r->Y2R_HUE.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_ConversionMatrix %d", y2r->eMatrix); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // ConversionMatrix_lb
	sprintf(buff, "Y2R_ConversionMatrix_min %d", y2r->BT601); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_ConversionMatrix_max %d", y2r->JFIF); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_MultY %f", y2r->aRangeMult[0]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MultY_sb
	sprintf(buff, "Y2R_MultY_min %f", y2r->Y2R_RANGEMULT.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_MultY_max %f", y2r->Y2R_RANGEMULT.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_MultCb %f", y2r->aRangeMult[1]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MultCb_sb
	sprintf(buff, "Y2R_MultCb_min %f", y2r->Y2R_RANGEMULT.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_MultCb_max %f", y2r->Y2R_RANGEMULT.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_MultCr %f", y2r->aRangeMult[2]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char)); // MultCr_sb
	sprintf(buff, "Y2R_MultCr_min %f", y2r->Y2R_RANGEMULT.min); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Y2R_MultCr_max %f", y2r->Y2R_RANGEMULT.max); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	// Send number of parameters that will be send
	cmd[0] = htonl(params.size());
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	return IMG_SUCCESS;
}
