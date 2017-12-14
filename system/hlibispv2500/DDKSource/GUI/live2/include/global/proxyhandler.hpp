#ifndef PROXYHANDLER_H
#define PROXYHANDLER_H

#include <qobject.h>
#include "ci/ci_api_structs.h"
#include "cv.h"
#include "algorithms.hpp"
#include "ispc/ParameterList.h"

namespace VisionLive
{

class ImageHandler;
class CommandHandler;
struct RecData;

class ProxyHandler: public QObject
{
	Q_OBJECT

public:
	ProxyHandler(QObject *parent = 0); // Class constructor
	~ProxyHandler(); // Class destructor

	bool isRunning() const { return (_imageHandlerState == VisionLive::CONNECTED || _commandHandlerState == VisionLive::CONNECTED); } // Indicates if ImageHandler or CommandHandler are running 
	
private:
	void initProxyHandler(); // Initializes ProxyHandler

	HandlerState _imageHandlerState; // Holds ImageHandler state
	int _imgPort; // Holds ImageHandler port
	ImageHandler *_pImageHandler; // ImageHandler object
	void initImageHandler(); // Initializes ImageHandler

	HandlerState _commandHandlerState; // Holds CommandHandler state 
	int _cmdPort; // Holds CommandHandler port
	CommandHandler *_pCommandHandler; // CommandHandler object
	void initCommandHandler(); // Initializes CommandHandler
	
signals:
	void logError(const QString &error, const QString &src); // Emited to log Error
	void logWarning(const QString &warning, const QString &src); // Emited to log Warning
	void logMessage(const QString &message, const QString &src); // Emited to log Info
	void logAction(const QString &action); // Emited to log Action

	void riseError(const QString &error); // Emited to popup error window
	void notifyStatus(const QString &status); // Emited to notify status (displayed in statusBar)

	void connected(); // Emited to inticated connection
	void disconnected(); // Emited to indicate disconnection
	void connectionCanceled(); // Emited to indicate connecting terminated

	// Proxy signals for ImageHandler signals
	void imageReceived(QImage *pImage, int type, int format, int mosaic, int width, int height);
	void recordingProgress(int currFrame, int maxFrame);
	void movieReceived(RecData *recData);
	void histogramReceived(std::vector<cv::Mat> histData, int nChannels, int nElem);
	void pixelInfoReceived(QPoint pixel, std::vector<int> pixelInfo);
	void areaStatisticsReceived(std::vector<double> areaStats);
	void vectorscopeReceived(QMap<int, std::pair<int, QRgb> > points);
	void lineViewReceived(QPoint point, QMap<QString, std::vector<std::vector<int> > > lineViewData, unsigned int step);
	void fpsInfoReceived(double fps);

	// Proxy signals for CommandHandler signals
	void HWInfoReceived(CI_HWINFO hwInfo);
	void FB_received(QMap<QString, QString> params);
	void SENSOR_received(QMap<QString, QString> params);
	void AE_received(QMap<QString, QString> params);
	void AF_received(QMap<QString, QString> params);
	void ENS_received(IMG_UINT32 size);
	void AWB_received(QMap<QString, QString> params);
	void DPF_received(QMap<QString, QString> params);
	void TNM_received(QMap<QString, QString> params, QVector<QPointF> curve);
	void LBC_received(QMap<QString, QString> params);
	void GMA_received(CI_MODULE_GMA_LUT data);

public slots:
	void connect(IMG_UINT16 cmdPort, IMG_UINT16 imgPort); // Starts connection of ImageHandler and CommandHandler
	void IH_connected(); // Called when ImaheHandler is connected
	void CH_connected(); // Called when CommandHandler is Connected
	void disconnect(); // Disconnects ImagaHandler and CommandHandler
	void IH_disconnected(); // Called when ImageHandler is disconnected
	void CH_disconnected(); // Called whan CommansHandler is disconnected
	void cancelConnection(); // Cancels Connection of ImageHandler or CommandHandler
	void IH_connectionCanceled(); // Called when ImageHandler connection is canceled
	void CH_connectionCanceled(); // Called when CommandHandler connection is canceled

	// Proxy slots to ImageHandler slots
	void record(int nFrames, const QString &name);
	void removeRecord(int index);
	void setCorrectGamma(bool opt);
	void setGenerateHistograms(bool opt);
	void setGenerateVectorscope(bool opt);
	void setGenerateLineView(bool opt);
	void setMarkDPFPoints(bool opt);
	void setImageType(ImageType type);
	void saveRecAsJPEG(int recDataIndex, const QString &filename);
	void saveRecAsFLX(int recDataIndex, const QString &filename);
	void setPixelMarkerPosition(QPointF point);
	void setSelection(QRect area, bool genStats);
	void setImageSending();

	// Proxy slots to CommandHandler slots
	void applyConfiguration(const ISPC::ParameterList &config);
	void FB_get();
	void AE_set(bool enable);
	void AF_set(bool enable);
	void AF_search();
	void DNS_set(int enable);
	void AWB_set(int mode);
	void LSH_set(std::string filename);
	void DPF_set(IMG_UINT8 *map, IMG_UINT32 nDefs);
	void TNM_set(bool enable);
	void LBC_set(bool enable);
	void GMA_set(CI_MODULE_GMA_LUT data);
	void GMA_get();

};

} // namespace VisionLive

#endif // PROXYHANDLER_H
