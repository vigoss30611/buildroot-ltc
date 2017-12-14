#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include "ui_mainwindow.h"
#include "ci/ci_api_structs.h"
#include "cv.h"
#include "img_types.h"

#include "ispc/ParameterList.h"

class QTextEdit;
class QMessageBox;
class QSpinBox;
class QComboBox;

class QwtPlot;
class QwtPlotHistogram;
class QwtIntervalSample;
class QwtColumnSymbol;

namespace VisionLive
{

class TabLogView;
class LiveFeedView;
class ObjectRegistry;
class ProxyHandler;
class HWInfo;
class Histogram;
struct RecData;
class CaptureGallery;
class VectorScope;
class LineView;
class Test;

class ModuleBase;
class ModuleOUT;
class ModuleExposure;
class ModuleFocus;
class ModuleBLC;
class ModuleNoise;
class ModuleR2Y;
class ModuleY2R;
class ModuleMGM;
class ModuleDGM;
class ModuleENS;
class ModuleWBC;
class ModuleLSH;
class ModuleDPF;
class ModuleTNM;
class ModuleVIB;
class ModuleLBC;
class ModuleGMA;
class ModuleMIE;
class ModuleLCA;

class MainWindow: public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

public:
    MainWindow(bool testMode, QString testFile, IMG_BOOL8 testStart, QMainWindow *parent = 0); // Class constructor
    ~MainWindow(); // Class destructor

protected:
	void closeEvent(QCloseEvent *event); // Called on closing (after connection LiveFeedView closeEvent() is called instead)

private:
	bool closeVisionLive; // Indicates ready to close app

	ObjectRegistry *_pObjReg; // Object holding pointers to all created objects
	void initObjectRegistry(); // Initiates ObjectRegistry

	QAction *_pCurrentShowAction; // Currently selected action
	void initMainManu(); // Initiates MenuBar

	QStatusBar *_pStatusBar; // StatusBar widget
	void initStatusBar(); // Initiates StatusBar

	TabLogView *_pLogView; // LogView widget
	void initLogView();  // Initiates LogView widget

	ISPC::ParameterList _config; // Current modules configuration 
	QTabWidget *_pModuleView; // ModulesView widget
	std::vector<ModuleBase *> _modules; // Holds pointers to all module widgets
	ModuleOUT *_pModuleOUT; // ModuleOut widget
	ModuleExposure *_pModuleExposure; // ModuleExposure widget
	ModuleFocus *_pModuleFocus; // ModuleFocus widget
	ModuleBLC *_pModuleBLC; // ModuleBLC widget
	ModuleNoise *_pModuleNoise; // ModuleNoise widget
	ModuleR2Y *_pModuleR2Y; // ModuleR2Y widget
	ModuleY2R *_pModuleY2R; // ModuleY2R widget
	ModuleMGM *_pModuleMGM; // ModuleMGM widget
	ModuleDGM *_pModuleDGM; // ModuleDGM widget
	ModuleENS *_pModuleENS; // ModuleENS widget
	ModuleWBC *_pModuleWBC; // ModuleWBC widget
	ModuleLSH *_pModuleLSH; // ModuleWBC widget
	ModuleDPF *_pModuleDPF; // ModuleDPF widget
	ModuleTNM *_pModuleTNM; // ModuleTNM widget
	ModuleVIB *_pModuleVIB; // ModuleVIB widget
	ModuleLBC *_pModuleLBC; // ModuleLBC widget
	ModuleGMA *_pModuleGMA; // ModuleGMA widget
	ModuleMIE *_pModuleMIE; // ModuleMIE widget
	ModuleLCA *_pModuleLCA; // ModuleLCA widget
	void initModuleView(); // Initiates ModulesView widget
	void addModule(ModuleBase *module); // Adds module widgets to ModuleView

	LiveFeedView *_pLiveFeedView; // LiveFeedView widget
	void initLiveFeedView(); // Initiates LiveFeedView widget

	CaptureGallery *_pCaptureGallery; // CaptureGallery widget
	void initCaptureGallery(); // Initiates CaptureGallery widget
	void showCaptureGallery(bool option); // Shows/Hides CaptureGallery widget

	// LiveFeedViewControl menu and actions
	QToolBar *_pLiveFeedViewControl;
	QAction *_pConnectAction;
	QAction *_pDisconnectAction;
	QAction *_pResumeAction;
	QAction *_pPauseAction;
	QAction *_pRecordAction;
	QSpinBox *_pFramesNum;
	QAction *_pYUVAction;
	QAction *_pRGBAction;
	QAction *_pDEAction;
	QAction *_pCaptureGalleryAction;
	QAction *_pHWInfoAction;
	QAction *_pHistogramAction;
	QAction *_pVectorscopeAction;
	QAction *_pLineViewAction;
	QComboBox *_pLineViewOption;
	QAction *_pDPFAction;
	void initLiveFeedViewControl(); // Initiates LiveFeedViewControl widget

	QMessageBox *_pWaitingConnection; // Waiting connection popup window
	QMessageBox *_pWaitingDiscommection; // Waiting disconnection popup window
	QMessageBox *_pWaitingRecording; // Waiting recording popup window

	HWInfo *_pHWInfo; // HWInfo widget
	void initHWInfo(); // Initializes HWInfo widget
	void showHWInfo(bool option); // Shows/Hides HWInfo widget

	Histogram *_pHistogram; // Histogram widget
	void initHistogram(); // Initializes Histogram widget
	void showHistogram(bool option); // Shows/Hides Histogram widget

	VectorScope *_pVectorScope; // VectorScope widget
	void initVectorScope(); // Initializes VectorScope widget
	void showVectorscope(bool option); // Shows/Hides VectorScope widget

	LineView *_pLineView; // LineView widget
	void initLineView(); // Initializes VectorScope widget
	void showLineView(bool option); // Shows/Hides VectorScope widget

	bool _DPFDisplayed; // Indicates if DPF points are displaye on LiveFeedView or not
	void showDPF(bool option); // Shows/Hides DPF points marked on LiveFeedView

	ProxyHandler *_pProxyHandler; // ProxyHandler object
	void initProxyHandler(); // Initializes ProxyHandler object

	Test *_pTest; // Test widget
	bool _testMode; // Indicates if VisionLive is running in test mode
	QString _testFile; // Holds path to test script file
	IMG_BOOL8 _testStart; // Indicates if test should start right afters starting
	void initTest(); // Initiates Test widget

	void setSplitters(int l, int r, int u, int d); // Sets ModuleView and LogView widgets minimized at sturtup

	void retranslate(); // Retranslates GUI

signals:
	void logError(const QString &error, const QString &src); // Requests to TabLogView error loging
	void logWarning(const QString &warning, const QString &src); // Requests to TabLogView error warning
	void logMessage(const QString &message, const QString &src); // Requests to TabLogView error message
	void logAction(const QString &action); // Requests to TabLogView error action

public slots:
	void loadConfiguration(const QString &filename = QString()); // Called to load configuration from file
	void saveConfiguration(const QString &filename = QString()); // Called to save configuration to file

	void connect(unsigned int port = 0); // Initiates connection to ISPC_tcp
	void connected(); // Called once connected to ISPC_tcp
	void disconnect(); // Initiates disconnection from ISPC_tcp
	void disconnected(); // Called once disconnected from ISPC_tcp
	void cancelConnection(QAbstractButton *botton); // Cancels waiting for connection
	void connectionCanceled(); // Called after terminating connecting
	void applyConfiguration(); // Called to apply configuration to ISPC_tcp

	void changeStyleSheet(); // Changes global stylesheet
	void riseError(const QString &error); // Popups error MessageBox

	void notifyStatus(const QString &status); // Sets statusBar text;

	void changeModuleViewWidget(int index = -1); // Changes currently displayed module tab in ModuleView

	void changeShowViewWidget(); // Changes currently displayed widget in ShowView

	void lineViewOptionChanged(); // Called when _pLineViewOption current index changed

	void triggerAction(QAction *action); // Triggers any given action. Used mainly to trigger actions from Test object thread

	void enableImagesSending(); // Enables sending image frames
	void disableImagesSending(); // Disables sending image frames
	void startRecording(const QString &name = QString(), unsigned int nFrames = 0); // Called to request ImageHandler (through ProxyHandler) to start recording frames
	void recordingProgress(int currFrame, int maxFrame); // Called from ImageHandler (through ProxyHandler) to indicate recording progress
	void movieReceived(RecData *recData); // Called when ImageHandler finished recording frames
	void changeImageType(); // Changes sending image type

	void pixelInfoReceived(QPoint pixel, std::vector<int> pixelInfo); // Called from ImageHandler (through ProxyHandler) to indicate pixel info received

	void aboutVisionLive(); // Displays information about VisionLive
	void aboutQt(); // Displays information about Qt version
};

} // namespace VisionLive

#endif // MAINWINDOW_H
