#include "proxyhandler.hpp"
#include "imagehandler.hpp"
#include "commandhandler.hpp"

//
// Public Functions
//

VisionLive::ProxyHandler::ProxyHandler(QObject *parent): QObject(parent)
{
	_pImageHandler = NULL;
	_pCommandHandler = NULL;

	_imgPort = 0;
	_cmdPort = 0;

	_imageHandlerState = VisionLive::DISCONNECTED;
	_commandHandlerState = VisionLive::DISCONNECTED;

	initImageHandler();
	initCommandHandler();
}

VisionLive::ProxyHandler::~ProxyHandler()
{
	if(_pImageHandler)
	{
		_pImageHandler->deleteLater();
	}
	if(_pCommandHandler)
	{
		_pCommandHandler->deleteLater();
	}
}

//
// Private Functions
//

void VisionLive::ProxyHandler::initImageHandler()
{
	_pImageHandler = new ImageHandler();

	QObject::connect(_pImageHandler, SIGNAL(logError(const QString &, const QString &)), this, SIGNAL(logError(const QString &, const QString &)));
	QObject::connect(_pImageHandler, SIGNAL(logWarning(const QString &, const QString &)), this, SIGNAL(logWarning(const QString &, const QString &)));
	QObject::connect(_pImageHandler, SIGNAL(logMessage(const QString &, const QString &)), this, SIGNAL(logMessage(const QString &, const QString &)));
	QObject::connect(_pImageHandler, SIGNAL(logAction(const QString &)), this, SIGNAL(logAction(const QString &)));

	QObject::connect(_pImageHandler, SIGNAL(riseError(const QString &)), this, SIGNAL(riseError(const QString &)));
	QObject::connect(_pImageHandler, SIGNAL(notifyStatus(const QString &)), this, SIGNAL(notifyStatus(const QString &)));

	QObject::connect(_pImageHandler, SIGNAL(connected()), this, SLOT(IH_connected()));
	QObject::connect(_pImageHandler, SIGNAL(disconnected()), this, SLOT(IH_disconnected()));
	QObject::connect(_pImageHandler, SIGNAL(connectionCanceled()), this, SLOT(IH_connectionCanceled()));

	QObject::connect(_pImageHandler, SIGNAL(imageReceived(QImage *, int, int, int, int, int, bool)), this, SIGNAL(imageReceived(QImage *, int, int, int, int, int, bool)));
	QObject::connect(_pImageHandler, SIGNAL(recordingProgress(int, int)), this, SIGNAL(recordingProgress(int, int)));
	QObject::connect(_pImageHandler, SIGNAL(movieReceived(RecData *)), this, SIGNAL(movieReceived(RecData *)));
	QObject::connect(_pImageHandler, SIGNAL(histogramReceived(QVector<QVector<QPointF> >)), this, SIGNAL(histogramReceived(QVector<QVector<QPointF> >)));
	QObject::connect(_pImageHandler, SIGNAL(pixelInfoReceived(QPoint, std::vector<int>)), this, SIGNAL(pixelInfoReceived(QPoint, std::vector<int>)));
	QObject::connect(_pImageHandler, SIGNAL(areaStatisticsReceived(std::vector<double>)), this, SIGNAL(areaStatisticsReceived(std::vector<double>)));
	QObject::connect(_pImageHandler, SIGNAL(vectorscopeReceived(QMap<int, std::pair<int, QRgb> >)), this, SIGNAL(vectorscopeReceived(QMap<int, std::pair<int, QRgb> >)));
	QObject::connect(_pImageHandler, SIGNAL(lineViewReceived(QPoint, QMap<QString, std::vector<std::vector<int> > >, unsigned int)), 
		this, SIGNAL(lineViewReceived(QPoint, QMap<QString, std::vector<std::vector<int> > >, unsigned int)));
	QObject::connect(_pImageHandler, SIGNAL(fpsInfoReceived(double)), this, SIGNAL(fpsInfoReceived(double)));
}

void VisionLive::ProxyHandler::initCommandHandler()
{
	_pCommandHandler = new CommandHandler();

	QObject::connect(_pCommandHandler, SIGNAL(logError(const QString &, const QString &)), this, SIGNAL(logError(const QString &, const QString &)));
	QObject::connect(_pCommandHandler, SIGNAL(logWarning(const QString &, const QString &)), this, SIGNAL(logWarning(const QString &, const QString &)));
	QObject::connect(_pCommandHandler, SIGNAL(logMessage(const QString &, const QString &)), this, SIGNAL(logMessage(const QString &, const QString &)));
	QObject::connect(_pCommandHandler, SIGNAL(logAction(const QString &)), this, SIGNAL(logAction(const QString &)));

	QObject::connect(_pCommandHandler, SIGNAL(riseError(const QString &)), this, SIGNAL(riseError(const QString &)));
	QObject::connect(_pCommandHandler, SIGNAL(notifyStatus(const QString &)), this, SIGNAL(notifyStatus(const QString &)));

	QObject::connect(_pCommandHandler, SIGNAL(connected()), this, SLOT(CH_connected()));
	QObject::connect(_pCommandHandler, SIGNAL(disconnected()), this, SLOT(CH_disconnected()));
	QObject::connect(_pCommandHandler, SIGNAL(connectionCanceled()), this, SLOT(CH_connectionCanceled()));

	QObject::connect(_pCommandHandler, SIGNAL(HWInfoReceived(CI_HWINFO)), this, SIGNAL(HWInfoReceived(CI_HWINFO)));
    QObject::connect(_pCommandHandler, SIGNAL(FB_received(const ISPC::ParameterList)), this, SIGNAL(FB_received(const ISPC::ParameterList)));
	QObject::connect(_pCommandHandler, SIGNAL(SENSOR_received(QMap<QString, QString>)), this, SIGNAL(SENSOR_received(QMap<QString, QString>)));
	QObject::connect(_pCommandHandler, SIGNAL(AE_received(QMap<QString, QString>)), this, SIGNAL(AE_received(QMap<QString, QString>)));
	QObject::connect(_pCommandHandler, SIGNAL(AF_received(QMap<QString, QString>)), this, SIGNAL(AF_received(QMap<QString, QString>)));
	QObject::connect(_pCommandHandler, SIGNAL(ENS_received(IMG_UINT32)), this, SIGNAL(ENS_received(IMG_UINT32)));
	QObject::connect(_pCommandHandler, SIGNAL(AWB_received(QMap<QString, QString>)), this, SIGNAL(AWB_received(QMap<QString, QString>)));
	QObject::connect(_pCommandHandler, SIGNAL(DPF_received(QMap<QString, QString>)), this, SIGNAL(DPF_received(QMap<QString, QString>)));
	QObject::connect(_pCommandHandler, SIGNAL(TNM_received(QMap<QString, QString>, QVector<QPointF>)), this, SIGNAL(TNM_received(QMap<QString, QString>, QVector<QPointF>)));
	QObject::connect(_pCommandHandler, SIGNAL(LBC_received(QMap<QString, QString>)), this, SIGNAL(LBC_received(QMap<QString, QString>)));
	QObject::connect(_pCommandHandler, SIGNAL(GMA_received(CI_MODULE_GMA_LUT)), this, SIGNAL(GMA_received(CI_MODULE_GMA_LUT)));
    QObject::connect(_pCommandHandler, SIGNAL(LSH_added(QString)), this, SIGNAL(LSH_added(QString)));
    QObject::connect(_pCommandHandler, SIGNAL(LSH_removed(QString)), this, SIGNAL(LSH_removed(QString)));
    QObject::connect(_pCommandHandler, SIGNAL(LSH_applyed(QString)), this, SIGNAL(LSH_applyed(QString)));
    QObject::connect(_pCommandHandler, SIGNAL(LSH_received(QString)), this, SIGNAL(LSH_received(QString)));
}

//
// Public Slots
//

void VisionLive::ProxyHandler::connect(IMG_UINT16 cmdPort, IMG_UINT16 imgPort)
{
	_cmdPort = cmdPort;
	_imgPort = imgPort;

	if(_commandHandlerState == VisionLive::DISCONNECTED)
	{
		_commandHandlerState = VisionLive::CONNECTING;
		_pCommandHandler->connect(cmdPort);
	}

	if(_imageHandlerState == VisionLive::DISCONNECTED)
	{
		_imageHandlerState = VisionLive::CONNECTING;
		_pImageHandler->connect(imgPort);
	}

	if(_commandHandlerState == VisionLive::CONNECTED && 
	    _imageHandlerState == VisionLive::CONNECTED)
	{
        _pCommandHandler->startRunning();
        _pImageHandler->startRunning();

		emit connected();
	}
}

void VisionLive::ProxyHandler::IH_connected()
{
	_imageHandlerState = VisionLive::CONNECTED;

	connect(_cmdPort, _imgPort);
}

void VisionLive::ProxyHandler::CH_connected()
{
	_commandHandlerState = VisionLive::CONNECTED;

	connect(_cmdPort, _imgPort);
}

void VisionLive::ProxyHandler::disconnect()
{
    if (!_pCommandHandler || !_pImageHandler) return;

    _pCommandHandler->disconnect();
    _pImageHandler->disconnect();
}

void VisionLive::ProxyHandler::IH_disconnected()
{
	_imageHandlerState = VisionLive::DISCONNECTED;

	if(_imageHandlerState == VisionLive::DISCONNECTED && _commandHandlerState == VisionLive::DISCONNECTED)
	{
		emit disconnected();
	}
}

void VisionLive::ProxyHandler::CH_disconnected()
{
	_commandHandlerState = VisionLive::DISCONNECTED;

	if(_imageHandlerState == VisionLive::DISCONNECTED && _commandHandlerState == VisionLive::DISCONNECTED)
	{
		emit disconnected();
	}
}

void VisionLive::ProxyHandler::cancelConnection()
{
	if(_pCommandHandler && _commandHandlerState == VisionLive::CONNECTING)
	{
		_pCommandHandler->cancelConnection();
	}
	else if(_pImageHandler && _imageHandlerState == VisionLive::CONNECTING)
	{
		_pImageHandler->cancelConnection();
	}
}

void VisionLive::ProxyHandler::IH_connectionCanceled()
{
	_imageHandlerState = VisionLive::DISCONNECTED;

	if(_imageHandlerState == VisionLive::DISCONNECTED && _commandHandlerState == VisionLive::DISCONNECTED)
	{
		emit connectionCanceled();
	}
}

void VisionLive::ProxyHandler::CH_connectionCanceled()
{
	_commandHandlerState = VisionLive::DISCONNECTED;

	if(_imageHandlerState == VisionLive::DISCONNECTED && _commandHandlerState == VisionLive::DISCONNECTED)
	{
		emit connectionCanceled();
	}
}


void VisionLive::ProxyHandler::record(int nFrames, const QString &name)
{
	if(_pImageHandler)
	{
		_pImageHandler->record(nFrames, name);
	}
}

void VisionLive::ProxyHandler::removeRecord(int index)
{
	if(_pImageHandler)
	{
		_pImageHandler->removeRecord(index);
	}
}

void VisionLive::ProxyHandler::setCorrectGamma(bool opt)
{
	if(_pImageHandler)
	{
		_pImageHandler->setCorrectGamma(opt);
	}
}

void VisionLive::ProxyHandler::setGenerateHistograms(bool opt)
{
	if(_pImageHandler)
	{
		_pImageHandler->setGenerateHistograms(opt);
	}
}

void VisionLive::ProxyHandler::setGenerateVectorscope(bool opt)
{
	if(_pImageHandler)
	{
		_pImageHandler->setGenerateVectorscope(opt);
	}
}

void VisionLive::ProxyHandler::setGenerateLineView(bool opt)
{
	if(_pImageHandler)
	{
		_pImageHandler->setGenerateLineView(opt);
	}
}

void VisionLive::ProxyHandler::setMarkDPFPoints(bool opt)
{
	if(_pImageHandler)
	{
		_pImageHandler->setMarkDPFPoints(opt);
	}
}

void VisionLive::ProxyHandler::setImageType(ImageType type)
{
	if(_pImageHandler)
	{
		_pImageHandler->setImageType(type);
	}
}

void VisionLive::ProxyHandler::saveRecAsJPEG(int recDataIndex, const QString &filename)
{
	if(_pImageHandler)
	{
		_pImageHandler->saveRecAsJPEG(recDataIndex, filename);
	}
}

void VisionLive::ProxyHandler::saveRecAsFLX(int recDataIndex, const QString &filename)
{
	if(_pImageHandler)
	{
		_pImageHandler->saveRecAsFLX(recDataIndex, filename);
	}
}

void VisionLive::ProxyHandler::setPixelMarkerPosition(QPointF point)
{
	if(_pImageHandler)
	{
		_pImageHandler->setPixelMarkerPosition(point);
	}
}

void VisionLive::ProxyHandler::setSelection(QRect area, bool genStats)
{
	if(_pImageHandler)
	{
		_pImageHandler->setSelection(area, genStats);
	}
}

void VisionLive::ProxyHandler::setImageSending()
{
	if(_pImageHandler)
	{
		_pImageHandler->setImageSending();
	}
}

void VisionLive::ProxyHandler::saveRec_TF(const QString &recName, const QString &fileName, bool saveAsFLX)
{
	if(_pImageHandler)
	{
		_pImageHandler->saveRec_TF(recName, fileName, saveAsFLX);
	}
}


void VisionLive::ProxyHandler::applyConfiguration(const ISPC::ParameterList &config)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->applyConfiguration(config);
	}
}

void VisionLive::ProxyHandler::FB_get()
{
	if(_pCommandHandler)
	{
		_pCommandHandler->FB_get();
	}
}

void VisionLive::ProxyHandler::AE_set(bool enable)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->AE_set(enable);
	}
}

void VisionLive::ProxyHandler::AF_set(bool enable)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->AF_set(enable);
	}
}

void VisionLive::ProxyHandler::AF_search()
{
	if(_pCommandHandler)
	{
		_pCommandHandler->AF_search();
	}
}

void VisionLive::ProxyHandler::DNS_set(int enable)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->DNS_set(enable);
	}
}

void VisionLive::ProxyHandler::AWB_set(int mode)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->AWB_set(mode);
	}
}

void VisionLive::ProxyHandler::LSH_add(std::string filename, int temp, unsigned int bitsPerDiff, double wbScale)
{
	if(_pCommandHandler)
	{
        _pCommandHandler->LSH_add(filename, temp, bitsPerDiff, wbScale);
	}
}

void VisionLive::ProxyHandler::LSH_remove(std::string filename)
{
    if (_pCommandHandler)
    {
        _pCommandHandler->LSH_remove(filename);
    }
}

void VisionLive::ProxyHandler::LSH_apply(std::string filename)
{
    if (_pCommandHandler)
    {
        _pCommandHandler->LSH_apply(filename);
    }
}

void VisionLive::ProxyHandler::LSH_set(bool enable)
{
    if (_pCommandHandler)
    {
        _pCommandHandler->LSH_set(enable);
    }
}

void VisionLive::ProxyHandler::DPF_set(IMG_UINT8 *map, IMG_UINT32 nDefs)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->DPF_set(map, nDefs);
	}
}

void VisionLive::ProxyHandler::TNM_set(bool enable)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->TNM_set(enable);
	}
}

void VisionLive::ProxyHandler::LBC_set(bool enable)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->LBC_set(enable);
	}
}

void VisionLive::ProxyHandler::GMA_set(CI_MODULE_GMA_LUT data)
{
	if(_pCommandHandler)
	{
		_pCommandHandler->GMA_set(data);
	}
}

void VisionLive::ProxyHandler::GMA_get()
{
	if(_pCommandHandler)
	{
		_pCommandHandler->GMA_get();
	}
}

void VisionLive::ProxyHandler::PDP_set(bool enable)
{
    if (_pCommandHandler)
    {
        _pCommandHandler->PDP_set(enable);
    }
}
