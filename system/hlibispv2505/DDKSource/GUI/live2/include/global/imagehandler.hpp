#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include <qthread.h>
#include "paramsocket/demoguicommands.hpp"
#include "ci/ci_api_structs.h"
#include "cv.h"
#include "algorithms.hpp"
#include "imagedata.hpp"
#include "ispc/Save.h"
#include "ispc/ParameterList.h"

#include <qelapsedtimer.h>
#include <qmutex.h>

#include <savefile.h>

class ParamSocketServer;

namespace VisionLive
{

struct ImageConfig
{
	ImageType type;
	bool setCorrectGamma;
	bool generateHistogram;
	bool generatePixelInfo;
	bool generateVectorscope;
	bool markDPFPoints;
	bool sendImage;
	QPointF marker;
	bool generateAreaStatistics;
	QRect selectionArea;
	bool generateLineView;

	ImageConfig()
	{
		type = RGB;
		setCorrectGamma = false;
		generateHistogram = false;
		generatePixelInfo = true;
		generateVectorscope = false;
		markDPFPoints = false;
		sendImage = true;
		marker = QPointF(0, 0);
		generateAreaStatistics = false;
		selectionArea = QRect(0, 0, 0, 0);
		generateLineView = false;
	}
};

struct RecConfig
{
	ImageType type;
	QString name;
	int nFrames;

	RecConfig()
	{
		type = RGB;
		name = QString();
		nFrames = 0;
	}
};

struct HistData
{
	//std::vector<cv::Mat> histograms;
	QVector<QVector<QPointF> > histograms;
	int nChannels;
	int nElem;

	HistData()
	{
		nChannels = 0;
		nElem = 0;
	}
};

struct RecData
{
	QString name;
	int nFrames;
	size_t frameSize;
	std::vector<IMG_UINT32> frameTimeStamp;
	std::vector<ImageData> frameData;
	std::vector<IMG_UINT32> nDefects;
	std::vector<IMG_UINT8 *> DPFMaps;
	std::vector<HistData> HistDatas;
	ImageType type;
	double xScaler;
	double yScaler;
	ISPC::ParameterList configuration;
	QMap<int, std::pair<int, QRgb> > vectorScopeData;

	RecData()
	{
		name = QString();
		nFrames = 0;
		frameSize = 0;
		frameTimeStamp.clear();
		frameData.clear();
		type = RGB;
		xScaler = 1.0f;
		yScaler = 1.0f;
	}
};

class Save: public ISPC::Save
{
public:
	IMG_RESULT open(SaveType type, ePxlFormat fmt, MOSAICType mos, int width, int height, const std::string &filename);
	IMG_RESULT saveYUV_m(const ISPC::Buffer &buff) { return saveYUV(buff); }
	IMG_RESULT saveRGB_m(const ISPC::Buffer &buff) { return saveRGB(buff); }
	IMG_RESULT saveBayer_m(const ISPC::Buffer &buff) { return saveBayer(buff); }
	IMG_RESULT saveBayerTiff_m(const ISPC::Buffer &buff) { return saveTiff(buff); }
};

class ImageHandler: public QThread
{
	Q_OBJECT

public:
	ImageHandler(QThread *parent = 0); // Class constructor
	~ImageHandler(); // Class destructor
    void startRunning() { _continueRun = true; } // Sets _continueRun to true

protected:
	void run(); // Thread loop

private:
	bool _continueRun; // Indicates if thread loop should continue

	QElapsedTimer _FPS_Timer; // Timer used to calculate fps
	int _FPS_framesReceived; // Frame counter used to calculate fps
	double _FPS; // Calculated fps

	QMutex _lock; // Mutex used to syncronise use of _cmds
	QList<GUIParamCMD> _cmds; // Queue of commands to run
	void pushCommand(GUIParamCMD cmd); // Thread safe funk to add commands to queue
	GUIParamCMD popCommand(); // Thread safe funk to remove commands from queue

	ParamSocketServer *_pSocket; // Connection socket
	IMG_UINT16 _port; // Connection port
	QString _clientName; // Name of connected app
	int startConnection(); // Starts connection

	int setQuit(ParamSocket *pSocket); // Called to end connection on both sides

	QMutex _imgLock; // Mutex used to syncronize use of _imgConf
	ImageConfig _imgConf; // Configuration for getImage() function
	ImageData _ImageData; // Image data updated in getImage() function
	int getImage(ParamSocket *pSocket); // Called to get next frame

	QMutex _recLock; // Synchronize usa of _recConf and _recData
	RecConfig _recConf; // Configuration for record() function
	std::vector<RecData *> _recData; // Recorded data updated in record() function
	int record(ParamSocket *socket); // Captures frames

	ImageData generateImageData(IMG_UINT8 *rawData, int width, int height, int stride, pxlFormat fmt, MOSAICType mosaic); // Called to create CVBuffer from rawData
	void generateHistogram(int bitdepth); // Generates histogram based on _pImageData
	HistData generateHistogram(const ImageData &imgData); // Generates histogram based on referenced ImgData
	void generatePixelInfo(QPointF pos); // Generates pixel info based on _pImageData and _marker
	void generateAreaStatistics(const ImageData &imgData); // Generates pixel info based on referenced ImgData and ImageConfig
	QImage *generateAWBDebugImage(ImageData &imgData); // Generates QImage with marked AWB statistics data

	bool _enableLiveFeed;

	IMG_UINT8 *_pDPFOutMap; // Holds received DPF output map
	IMG_UINT32 _nDefects; // Holds number of defects in _pDPFOutMap
	int DPF_getMap(ParamSocket *socket); // Receives DPF output map and stors it in _pDPFOutMap
	void markDPFPoints(QImage *image, double xScaler, double yScaler); // Marks DPF points from _pDPFOutMap on image

	ISPC::ParameterList _receivedConfiguration;
	int receiveConfiguration(ParamSocket *socket); // Gets current configuration from ISPC_tcp

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

	void imageReceived(QImage *pImage, int type, int format, int mosaic, int width, int height, bool awbDebugMode); // Emited to indicate next frame received
	void recordingProgress(int currFrame, int maxFrame); // Indicates recording progress to MainWindow
	void movieReceived(RecData *recData); // Indicates to MainWindow that recording has finished
	void histogramReceived(QVector<QVector<QPointF> > histData);
	void pixelInfoReceived(QPoint pixel, std::vector<int> pixelInfo); // Emited to indicate Pixel Info generated
	void areaStatisticsReceived(std::vector<double> areaStats); // Emited to indicate area statistics generated
	void vectorscopeReceived(QMap<int, std::pair<int, QRgb> > points); // Emited to indicate Vectorscope data generated
	void lineViewReceived(QPoint point, QMap<QString, std::vector<std::vector<int> > > lineViewData, unsigned int step); // Emited to indicate line view data generated
	void fpsInfoReceived(double fps); // Emited to indicate FPS calculated

public slots:
	void connect(IMG_UINT16 port); // Callec do start connection
	void disconnect(); // Called to disconnect
	void cancelConnection(); // Called to cancel connecting

	void record(int nFrames, const QString &name); // Requests recording number of frames
	void removeRecord(int index); // Request to remove _recData of specyfis index
	void setCorrectGamma(bool opt); // Called to set option of gamma correction in _imgConf
	void setGenerateHistograms(bool opt); // Called to set option of histogram generation in _imgConf
	void setGenerateVectorscope(bool opt); // Called to set option of vectorscope generation in _imgConf
	void setGenerateLineView(bool opt); // Called to set option of line view generation in _imgConf
	void setMarkDPFPoints(bool opt); //  Called to set option of marking DPF points on image
	void setImageType(ImageType type); // Called to set requested image type in _imgConf
	void saveRecAsJPEG(int recDataIndex, const QString &filename); // Saves origData of _recData of index recDataIndex as JPEG
	void saveRecAsFLX(int recDataIndex, const QString &filename); // Saves origData of _recData of index recDataIndex as FLX
	void saveRec_TF(const QString &recName, const QString &fileName, bool saveAsFLX); // Convenience function used by TF
	void setPixelMarkerPosition(QPointF point); // Called to set pixel info marker coordinates in _imgConf
	void setSelection(QRect area, bool genStats); // Called to set selectionArea rect and optionn to generate selection area statistics in _imgConf
	void setImageSending(); // Called pause/resume image sending
};

} // namespace VisionLive

#endif // IMAGEHANDLER_H
