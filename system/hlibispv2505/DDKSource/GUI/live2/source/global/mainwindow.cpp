#include "mainwindow.hpp"

#include "about.hpp"
#include "css.hpp"
#include "names.hpp"
#include "algorithms.hpp"
#include "tablogview.hpp"
#include "livefeedview.hpp"
#include "objectregistry.hpp"
#include "hwinfo.hpp"
#include "histogram.hpp"
#include "capturegallery.hpp"
#include "capturegalleryitem.hpp"
#include "vectorscope.hpp"
#include "lineview.hpp"
#include "proxyhandler.hpp"
#include "test.hpp"
#include "htmlviewer.hpp"

#include "modulebase.hpp"
#include "out.hpp"
#include "exposure.hpp"
#include "focus.hpp"
#include "blc.hpp"
#include "noise.hpp"
#include "r2y.hpp"
#include "y2r.hpp"
#include "mgm.hpp"
#include "dgm.hpp"
#include "ens.hpp"
#include "wbc.hpp"
#include "lsh.hpp"
#include "dpf.hpp"
#include "tnm.hpp"
#include "vib.hpp"
#include "lbc.hpp"
#include "gma.hpp"
#include "mie.hpp"
#include "lca.hpp"

#include "ispc/ParameterFileParser.h"

#include <qgraphicsview.h>
#include <qgraphicsscene.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtoolbar.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <QCloseEvent>
#include <qspinbox.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <qtablewidget.h>
#include <qtextedit.h>

//
// Public Functions
//

VisionLive::MainWindow::MainWindow(bool testMode, QString testFile, IMG_BOOL8 testStart, QMainWindow *parent): 
	_testMode(testMode), _testFile(testFile), _testStart(testStart), QMainWindow(parent)
{
	Ui::MainWindow::setupUi(this);

	retranslate();

	closeVisionLive = false;

	_pObjReg = nullptr;
	_pStatusBar = nullptr;
	_pLogView = nullptr;
	_pModuleView = nullptr;
	_pModuleOUT = nullptr;
	_pModuleExposure = nullptr;
	_pModuleFocus = nullptr;
	_pModuleBLC = nullptr;
	_pModuleNoise = nullptr;
	_pModuleR2Y = nullptr;
	_pModuleY2R = nullptr;
	_pModuleMGM = nullptr;
	_pModuleDGM = nullptr;
	_pModuleENS = nullptr;
	_pModuleWBC = nullptr;
	_pModuleLSH = nullptr;
	_pModuleDPF = nullptr;
	_pModuleTNM = nullptr;
	_pModuleVIB = nullptr;
	_pModuleLBC = nullptr;
	_pModuleGMA = nullptr;
	_pModuleMIE = nullptr;
	_pModuleLCA = nullptr;
	_pLiveFeedView = nullptr;
	_pWaitingConnection = nullptr;
	_pWaitingDiscommection = nullptr;
	_pWaitingRecording = nullptr;
	_pProxyHandler = nullptr;
	_pProxyHandler = nullptr;
	_pTest = nullptr;
	_pHWInfo = nullptr;
	_pHistogram = nullptr;
	_pLiveFeedViewControl = nullptr;
	_pConnectAction = nullptr;
	_pDisconnectAction = nullptr;
	_pHWVersionOption = nullptr;
	_pResumeAction = nullptr;
	_pPauseAction = nullptr;
	_pRecordAction = nullptr;
	_pFramesNum = nullptr;
    _pPDPAction = nullptr;
	_pENCAction = nullptr;
	_pDISPAction = nullptr;
	_pDEAction = nullptr;
	_pHDRAction = nullptr;
	_pRAW2DAction = nullptr;
	_pCaptureGalleryAction = nullptr;
	_pHWInfoAction = nullptr;
	_pHistogramAction = nullptr;
	_pCaptureGallery = nullptr;
	_pVectorscopeAction = nullptr;
	_pLineViewAction = nullptr;
	_pLineViewOption = nullptr;
	_pCurrentShowAction = nullptr;
	_pVectorScope = nullptr;
	_pLineView = nullptr;
	_pDPFAction = nullptr;
	_pISPDoc = nullptr;

	_DPFDisplayed = false;

    _connected = false;

#if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 4
	_hwVersion = HW_2_X;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN == 4
	_hwVersion = HW_2_4;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN == 6
	_hwVersion = HW_2_6;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN == 7
	_hwVersion = HW_2_7;
#elif FELIX_VERSION_MAJ == 3
	_hwVersion = HW_3_X;
#else
	_hwVersion = HW_UNKNOWN;
#endif

	initObjectRegistry();

	initMainManu();

	initStatusBar();

	initLogView();

	initLiveFeedView();

	initLiveFeedViewControl();

	initProxyHandler();

	initModuleView();

	initCaptureGallery();

	initHWInfo();

	initHistogram();

	initVectorScope();

	initLineView();

	if(_testMode)
	{
		initTest();
	}

	setSplitters(1, 0, 1, 0);

	if(_pLogView)
	{
		QObject::connect(this, SIGNAL(logError(const QString &, const QString &)), _pLogView, SLOT(logError(const QString &, const QString &)));
		QObject::connect(this, SIGNAL(logWarning(const QString &, const QString &)), _pLogView, SLOT(logWarning(const QString &, const QString &)));
		QObject::connect(this, SIGNAL(logMessage(const QString &, const QString &)), _pLogView, SLOT(logMessage(const QString &, const QString &)));
		QObject::connect(this, SIGNAL(logAction(const QString &)), _pLogView, SLOT(logAction(const QString &)));

		QObject::connect(actionExport_Log, SIGNAL(triggered()), _pLogView, SLOT(exportLog()));
		QObject::connect(actionExport_Action_Log, SIGNAL(triggered()), _pLogView, SLOT(exportActionLog()));
	}

	if(_pObjReg && _pLogView)
	{
		QObject::connect(_pObjReg, SIGNAL(logError(const QString &, const QString &)), _pLogView, SLOT(logError(const QString &, const QString &)));
		QObject::connect(_pObjReg, SIGNAL(logWarning(const QString &, const QString &)), _pLogView, SLOT(logWarning(const QString &, const QString &)));
		QObject::connect(_pObjReg, SIGNAL(logMessage(const QString &, const QString &)), _pLogView, SLOT(logMessage(const QString &, const QString &)));
		QObject::connect(_pObjReg, SIGNAL(logAction(const QString &)), _pLogView, SLOT(logAction(const QString &)));
	}

	if(_pLiveFeedView && _pLogView)
	{
		QObject::connect(_pLiveFeedView, SIGNAL(logError(const QString &, const QString &)), _pLogView, SLOT(logError(const QString &, const QString &)));
		QObject::connect(_pLiveFeedView, SIGNAL(logWarning(const QString &, const QString &)), _pLogView, SLOT(logWarning(const QString &, const QString &)));
		QObject::connect(_pLiveFeedView, SIGNAL(logMessage(const QString &, const QString &)), _pLogView, SLOT(logMessage(const QString &, const QString &)));
		QObject::connect(_pLiveFeedView, SIGNAL(logAction(const QString &)), _pLogView, SLOT(logAction(const QString &)));
	}

	if(_pProxyHandler && _pLogView)
	{
		QObject::connect(_pProxyHandler, SIGNAL(logError(const QString &, const QString &)), _pLogView, SLOT(logError(const QString &, const QString &)));
		QObject::connect(_pProxyHandler, SIGNAL(logWarning(const QString &, const QString &)), _pLogView, SLOT(logWarning(const QString &, const QString &)));
		QObject::connect(_pProxyHandler, SIGNAL(logMessage(const QString &, const QString &)), _pLogView, SLOT(logMessage(const QString &, const QString &)));
		QObject::connect(_pProxyHandler, SIGNAL(logAction(const QString &)), _pLogView, SLOT(logAction(const QString &)));
	}

	if(_pCaptureGallery && _pLogView)
	{
		QObject::connect(_pCaptureGallery, SIGNAL(logError(const QString &, const QString &)), _pLogView, SLOT(logError(const QString &, const QString &)));
		QObject::connect(_pCaptureGallery, SIGNAL(logWarning(const QString &, const QString &)), _pLogView, SLOT(logWarning(const QString &, const QString &)));
		QObject::connect(_pCaptureGallery, SIGNAL(logMessage(const QString &, const QString &)), _pLogView, SLOT(logMessage(const QString &, const QString &)));
		QObject::connect(_pCaptureGallery, SIGNAL(logAction(const QString &)), _pLogView, SLOT(logAction(const QString &)));
	}

	if(_pProxyHandler && _pLiveFeedView)
	{
		QObject::connect(_pProxyHandler, SIGNAL(imageReceived(QImage *, int, int, int, int, int, bool)), 
			_pLiveFeedView, SLOT(imageReceived(QImage *, int, int, int, int, int, bool)));
		QObject::connect(_pLiveFeedView, SIGNAL(CS_setMarker(QPointF)), _pProxyHandler, SLOT(setPixelMarkerPosition(QPointF)));
		QObject::connect(_pLiveFeedView, SIGNAL(CS_setSelection(QRect, bool)), _pProxyHandler, SLOT(setSelection(QRect, bool)));
		QObject::connect(_pProxyHandler, SIGNAL(pixelInfoReceived(QPoint, std::vector<int>)), _pLiveFeedView, SLOT(pixelInfoReceived(QPoint, std::vector<int>)));
		QObject::connect(_pProxyHandler, SIGNAL(areaStatisticsReceived(std::vector<double>)), _pLiveFeedView, SLOT(areaStatisticsReceived(std::vector<double>)));
		QObject::connect(_pProxyHandler, SIGNAL(fpsInfoReceived(double)), _pLiveFeedView, SLOT(fpsInfoReceived(double)));
	}

	if(_pProxyHandler && _pHistogram)
	{
		QObject::connect(_pProxyHandler, SIGNAL(histogramReceived(QVector<QVector<QPointF> >)), _pHistogram, SLOT(histogramReceived(QVector<QVector<QPointF> >)));
	}

	if(_pProxyHandler && _pHWInfo)
	{
		QObject::connect(_pProxyHandler, SIGNAL(HWInfoReceived(CI_HWINFO)), _pHWInfo, SLOT(HWInfoReceived(CI_HWINFO)));
	}

	if(_pProxyHandler && _pVectorScope)
	{
		QObject::connect(_pProxyHandler, SIGNAL(vectorscopeReceived(QMap<int, std::pair<int, QRgb> >)), _pVectorScope, SLOT(vectorscopeReceived(QMap<int, std::pair<int, QRgb> >)));
	}

	if(_pProxyHandler && _pLineView)
	{
		QObject::connect(_pProxyHandler, SIGNAL(lineViewReceived(QPoint, QMap<QString, std::vector<std::vector<int> > >, unsigned int)), 
			_pLineView, SLOT(lineViewReceived(QPoint, QMap<QString, std::vector<std::vector<int> > >, unsigned int)));
	}

	_pHWVersionOption->setCurrentIndex((int)_hwVersion);

	triggerAction(actionCustom);
}

VisionLive::MainWindow::~MainWindow()
{
}

//
// Protected Functions
//

void VisionLive::MainWindow::closeEvent(QCloseEvent *event)
{
	if(_pProxyHandler && !closeVisionLive)
	{
		if(_pProxyHandler->isRunning())
		{
			closeVisionLive = true;
			event->ignore();
			disconnect();
		}

		// Delete all modules
		for(unsigned int i = 0; i < _modules.size(); i++)
		{
			if(_modules[i])
			{
				//_modules[i]->deleteLater();
				delete _modules[i];
			}
		}

		if(_pCaptureGallery)
		{
			delete _pCaptureGallery;
		}

		if(_pISPDoc)
		{
			delete _pISPDoc;
		}

		if(_pTest)
		{
			delete _pTest;
		}
	}
	else
	{
		event->accept();
	}
}

//
// Private Functions
//

void VisionLive::MainWindow::initMainManu()
{
	// If documentation not installed
#ifndef VISION_LIVE_DOC
	actionISP_Documentation->setEnabled(false);
#endif

	actionLoad_Configuration->setObjectName("actionLoad_Configuration");
	actionSave_Configuration->setObjectName("actionSave_Configuration");
	actionExport_Log->setObjectName("actionExport_Log");
	actionExport_Action_Log->setObjectName("actionExport_Action_Log");

	actionConnect->setObjectName("actionConnect");
	actionDisconnect->setObjectName("actionDisconnect");
	actionApply_Configuration->setObjectName("actionApply_Configuration");

	actionDefault->setObjectName("actionDefault");
	actionCustom->setObjectName("actionCustom");

	actionGallery->setObjectName("actionGallery");
	actionHW_Info->setObjectName("actionHW_Info");
	actionHistogram->setObjectName("actionHistogram");
	actionVectorscope->setObjectName("actionVectorscope");
	actionLineView->setObjectName("actionLineView");
	actionDefective_Pixels->setObjectName("actionDefective_Pixels");

    actionTest->setObjectName("actionTest");

	actionAbout_VisionLive->setObjectName("actionAbout_VisionLive");
	actionAbout_Qt->setObjectName("actionAbout_Qt");
	actionISP_Documentation->setObjectName("actionISP_Documentation");

	actionConnect->setEnabled(true);
	actionDisconnect->setEnabled(false);
	actionApply_Configuration->setEnabled(false);

    actionDefault->setEnabled(false);
	actionDefault->setCheckable(true);
	actionCustom->setCheckable(true);
	actionDefault->setChecked(true);
	actionCustom->setChecked(false);

	actionGallery->setEnabled(false);
	actionHW_Info->setEnabled(false);
	actionHistogram->setEnabled(false);
	actionVectorscope->setEnabled(false);
	actionLineView->setEnabled(false);
	actionDefective_Pixels->setEnabled(false);

	QObject::connect(actionLoad_Configuration, SIGNAL(triggered()), this, SLOT(loadConfiguration()));
	QObject::connect(actionSave_Configuration, SIGNAL(triggered()), this, SLOT(saveConfiguration()));

	QObject::connect(actionConnect, SIGNAL(triggered()), this, SLOT(connect()));
	QObject::connect(actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnect()));
	QObject::connect(actionApply_Configuration, SIGNAL(triggered()), this, SLOT(applyConfiguration()));

	QObject::connect(actionDefault, SIGNAL(triggered()), this, SLOT(changeStyleSheet()));
	QObject::connect(actionCustom, SIGNAL(triggered()), this, SLOT(changeStyleSheet()));

	QObject::connect(actionGallery, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(actionHW_Info, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(actionHistogram, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(actionVectorscope, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(actionLineView, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(actionDefective_Pixels, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));

    QObject::connect(actionTest, SIGNAL(triggered()), this, SLOT(openTestTool()));

	QObject::connect(actionAbout_VisionLive, SIGNAL(triggered()), this, SLOT(aboutVisionLive()));
	QObject::connect(actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	QObject::connect(actionISP_Documentation, SIGNAL(triggered()), this, SLOT(showISPDoc()));

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(menuWidget());

		QString path;

		path =  menuWidget()->objectName() + "/" + menuFile->objectName();
		_pObjReg->registerObject(actionLoad_Configuration->objectName(), actionLoad_Configuration, path);
		_pObjReg->registerObject(actionSave_Configuration->objectName(), actionSave_Configuration, path);
		_pObjReg->registerObject(actionExport_Log->objectName(), actionExport_Log, path);
		_pObjReg->registerObject(actionExport_Action_Log->objectName(), actionExport_Action_Log, path);

		path =  menuWidget()->objectName() + "/" + menuConnection->objectName();
		_pObjReg->registerObject(actionConnect->objectName(), actionConnect, path);
		_pObjReg->registerObject(actionDisconnect->objectName(), actionDisconnect, path);
		_pObjReg->registerObject(actionApply_Configuration->objectName(), actionApply_Configuration, path);

		path =  menuWidget()->objectName() + "/" + menuView->objectName()+ "/" + menuShow->objectName();
		_pObjReg->registerObject(actionGallery->objectName(), actionGallery, path);
		_pObjReg->registerObject(actionHW_Info->objectName(), actionHW_Info, path);
		_pObjReg->registerObject(actionHistogram->objectName(), actionHistogram, path);
		_pObjReg->registerObject(actionVectorscope->objectName(), actionVectorscope, path);
		_pObjReg->registerObject(actionLineView->objectName(), actionLineView, path);
		_pObjReg->registerObject(actionDefective_Pixels->objectName(), actionDefective_Pixels, path);

        path = menuWidget()->objectName() + "/" + menuTools->objectName();
        _pObjReg->registerObject(actionTest->objectName(), actionTest, path);

		path =  menuWidget()->objectName() + "/" + menuView->objectName() + "/" + menuSet_Style_Sheet->objectName();
		_pObjReg->registerObject(actionDefault->objectName(), actionDefault, path);
		_pObjReg->registerObject(actionCustom->objectName(), actionCustom, path);

		path =  menuWidget()->objectName() + "/" + menuHelp->objectName();
		_pObjReg->registerObject(actionAbout_VisionLive->objectName(), actionAbout_VisionLive, path);
		_pObjReg->registerObject(actionAbout_Qt->objectName(), actionAbout_Qt, path);
		_pObjReg->registerObject(actionISP_Documentation->objectName(), actionISP_Documentation, path);
	}
	else
	{
		emit logError("Couldn,t register MainMenu!", "MainWindow::initMainManu()");
	}
}

void VisionLive::MainWindow::initStatusBar()
{
	_pStatusBar = statusBar();

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(_pStatusBar);
	}
	else
	{
		emit logError("Couldn,t register StatusBar!", "MainWindow::initStatusBar()");
	}
}

void VisionLive::MainWindow::initLogView()
{
	_pLogView = new TabLogView();
	logViewLayout->addWidget(_pLogView);

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(_pLogView);
	}
	else
	{
		emit logError("Couldn,t register LogView!", "MainWindow::initLogView()");
	}
}

void VisionLive::MainWindow::initModuleView()
{
	_pModuleView = new QTabWidget();
	moduleViewLayout->addWidget(_pModuleView);
	QObject::connect(_pModuleView, SIGNAL(currentChanged(int)), this, SLOT(changeModuleViewWidget(int)));

	_pModuleOUT = new ModuleOUT();
	addModule(_pModuleOUT);

	_pModuleExposure = new ModuleExposure();
	addModule(_pModuleExposure);

	_pModuleFocus = new ModuleFocus();
	addModule(_pModuleFocus);

	_pModuleBLC = new ModuleBLC();
	addModule(_pModuleBLC);

	_pModuleWBC = new ModuleWBC();
	addModule(_pModuleWBC);

	_pModuleLSH = new ModuleLSH();
	addModule(_pModuleLSH);

	_pModuleLCA = new ModuleLCA();
	addModule(_pModuleLCA);

	_pModuleNoise = new ModuleNoise();
	addModule(_pModuleNoise);

	_pModuleDPF = new ModuleDPF();
	addModule(_pModuleDPF);

	_pModuleTNM = new ModuleTNM();
	addModule(_pModuleTNM);

	_pModuleMIE = new ModuleMIE();
	addModule(_pModuleMIE);

	_pModuleVIB = new ModuleVIB();
	addModule(_pModuleVIB);

	_pModuleR2Y = new ModuleR2Y();
	addModule(_pModuleR2Y);

	_pModuleY2R = new ModuleY2R();
	addModule(_pModuleY2R);

	_pModuleMGM = new ModuleMGM();
	addModule(_pModuleMGM);

	_pModuleDGM = new ModuleDGM();
	addModule(_pModuleDGM);

	_pModuleENS = new ModuleENS();
	addModule(_pModuleENS);

	_pModuleLBC = new ModuleLBC();
	addModule(_pModuleLBC);

	_pModuleGMA = new ModuleGMA();
	addModule(_pModuleGMA);

	QObject::connect(_pModuleBLC, SIGNAL(moduleBLCChanged(int, int, int, int, int)), _pModuleWBC, SLOT(moduleBLCChanged(int, int, int, int, int)));
	QObject::connect(_pModuleBLC, SIGNAL(moduleBLCChanged(int, int, int, int, int)), _pModuleLSH, SLOT(moduleBLCChanged(int, int, int, int, int)));
    QObject::connect(_pModuleWBC, SIGNAL(moduleAWBModeChanged(int)), _pModuleLSH, SLOT(moduleAWBModeChanged(int)));

	for(unsigned int i = 0; i < _modules.size(); i++)
	{
		_modules[i]->load(_config);
		_modules[i]->removeObjectValueChangedMark();
	}
}

void VisionLive::MainWindow::addModule(ModuleBase *module)
{
	if(_pObjReg)
	{
		module->setObjectRegistry(_pObjReg);
	}

	QAction *pModuleAccessAction = new QAction(module->objectName(), menuModule_Access);
	pModuleAccessAction->setObjectName(module->objectName());
	pModuleAccessAction->setCheckable(true);
	if(_modules.empty())
	{
		pModuleAccessAction->setChecked(true);
	}
	QObject::connect(pModuleAccessAction, SIGNAL(triggered()), this, SLOT(changeModuleViewWidget()));
	menuModule_Access->addAction(pModuleAccessAction);
	
	_modules.push_back(module);

	_pModuleView->addTab(module, module->objectName());

	module->setHWVersion(_hwVersion);

	QObject::connect(module, SIGNAL(logError(const QString &, const QString &)), _pLogView, SLOT(logError(const QString &, const QString &)));
	QObject::connect(module, SIGNAL(logWarning(const QString &, const QString &)), _pLogView, SLOT(logWarning(const QString &, const QString &)));
	QObject::connect(module, SIGNAL(logMessage(const QString &, const QString &)), _pLogView, SLOT(logMessage(const QString &, const QString &)));
	QObject::connect(module, SIGNAL(logAction(const QString &)), _pLogView, SLOT(logAction(const QString &)));
    QObject::connect(module, SIGNAL(riseError(const QString &)), this, SLOT(riseError(const QString &)));

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(module, "Modules/", module->objectName());
		QString path =  menuWidget()->objectName() + "/" + menuView->objectName() + "/" + menuModule_Access->objectName();
		_pObjReg->registerObject("action" + pModuleAccessAction->objectName(), pModuleAccessAction, path);
	}
	else
	{
		emit logError("Couldn,t register Modules!", "MainWindow::addModule()");
	}
}

void VisionLive::MainWindow::initLiveFeedView()
{
	_pLiveFeedView = new LiveFeedView();
	showViewLayout->addWidget(_pLiveFeedView);
}

void VisionLive::MainWindow::initCaptureGallery()
{
	_pCaptureGallery = new CaptureGallery();
	if(_pObjReg)
	{
		_pCaptureGallery->setObjectRegistry(_pObjReg);
	}
	showViewLayout->addWidget(_pCaptureGallery);
	_pCaptureGallery->hide();
}

void VisionLive::MainWindow::showCaptureGallery(bool option)
{
	if(option) _pCaptureGallery->show();
	else _pCaptureGallery->hide();

	actionGallery->setChecked(option);
	_pCaptureGalleryAction->setChecked(option);
}

void VisionLive::MainWindow::initLiveFeedViewControl()
{
	_pLiveFeedViewControl = new QToolBar(this);
	_pLiveFeedViewControl->setObjectName("LiveFeedViewControl");

	_pLiveFeedViewControl->setMovable(true);
	_pLiveFeedViewControl->setFloatable(true);

	_pConnectAction = new QAction(QIcon(":/files/connect_icon.png"), tr("Connect"), _pLiveFeedViewControl);
	_pConnectAction->setObjectName("ConnectAction");
	_pConnectAction->setEnabled(true);
	_pDisconnectAction = new QAction(QIcon(":/files/disconnect_icon.png"), tr("Disconnect"), _pLiveFeedViewControl);
	_pDisconnectAction->setObjectName("DisconnectAction");
	_pDisconnectAction->setEnabled(false);

	_pHWVersionOption = new QComboBox();
	_pHWVersionOption->setObjectName("HWVersionOption_lb");
	_pHWVersionOption->addItem("Default");
	_pHWVersionOption->addItem("2.X");
	_pHWVersionOption->addItem("2.4");
	_pHWVersionOption->addItem("2.6");
	_pHWVersionOption->addItem("2.7");
	_pHWVersionOption->addItem("3.X");
	_pHWVersionOption->setToolTip(tr("HW version"));
	assert(_pHWVersionOption->count() != MAX_HW_VERSION);

	_pResumeAction = new QAction(QIcon(":/files/play_icon.png"), tr("Resume"), _pLiveFeedViewControl);
	_pResumeAction->setObjectName("ResumeAction");
	_pResumeAction->setEnabled(false);
	_pPauseAction = new QAction(QIcon(":/files/pause_icon.png"), tr("Pause"), _pLiveFeedViewControl);
	_pPauseAction->setObjectName("PauseAction");
	_pRecordAction = new QAction(QIcon(":/files/record_icon.png"), tr("Record"), _pLiveFeedViewControl);
	_pRecordAction->setObjectName("RecordAction");
	_pRecordAction->setEnabled(false);

	_pFramesNum = new QSpinBox();
	_pFramesNum->setObjectName("FrameNumber_sb");
	_pFramesNum->setAlignment(Qt::AlignHCenter);
	_pFramesNum->setRange(1, 100);
	_pFramesNum->setToolTip(tr("Frames to record"));

	_pLineViewOption = new QComboBox();
	_pLineViewOption->setObjectName("LineViewOption_lb");
	_pLineViewOption->addItem("Horizontal");
	_pLineViewOption->addItem("Vertical");
	_pLineViewOption->setToolTip(tr("LineView option"));

    _pPDPAction = new QAction("PDP", _pLiveFeedViewControl);
    _pPDPAction->setObjectName("PDPAction");
    _pPDPAction->setEnabled(false);
    _pPDPAction->setCheckable(true);

	_pENCAction = new QAction("ENC", _pLiveFeedViewControl);
	_pENCAction->setObjectName("ENCAction");
	_pENCAction->setEnabled(false);
	_pDISPAction = new QAction("DISP", _pLiveFeedViewControl);
	_pDISPAction->setObjectName("DISPAction");
	_pDISPAction->setEnabled(false);
	_pDEAction = new QAction("DE", _pLiveFeedViewControl);
	_pDEAction->setObjectName("DEAction");
	_pDEAction->setEnabled(false);
	_pHDRAction = new QAction("HDR", _pLiveFeedViewControl);
	_pHDRAction->setObjectName("HDRAction");
	_pHDRAction->setEnabled(false);
	_pRAW2DAction = new QAction("RAW2D", _pLiveFeedViewControl);
	_pRAW2DAction->setObjectName("RAW2DAction");
	_pRAW2DAction->setEnabled(false);

	_pCaptureGalleryAction = new QAction(QIcon(":/files/gallery_icon.png"), tr("Capture Gallery"), _pLiveFeedViewControl);
	_pCaptureGalleryAction->setObjectName("CaptureGalleryAction");
	_pCaptureGalleryAction->setCheckable(true);
	_pCaptureGalleryAction->setEnabled(false);
	_pHWInfoAction = new QAction(QIcon(":/files/hwinfo_icon.png"), tr("HW Info"), _pLiveFeedViewControl);
	_pHWInfoAction->setObjectName("HWInfoAction");
	_pHWInfoAction->setCheckable(true);
	_pHWInfoAction->setEnabled(false);
	_pHistogramAction = new QAction(QIcon(":/files/histogram_icon.png"), tr("Histogram"), _pLiveFeedViewControl);
	_pHistogramAction->setObjectName("HistogramAction");
	_pHistogramAction->setCheckable(true);
	_pHistogramAction->setEnabled(false);
	_pVectorscopeAction = new QAction(QIcon(":/files/histogram_icon.png"), tr("Vectorscope"), _pLiveFeedViewControl);
	_pVectorscopeAction->setObjectName("VectorscopeAction");
	_pVectorscopeAction->setCheckable(true);
	_pVectorscopeAction->setEnabled(false);
	_pLineViewAction = new QAction(QIcon(":/files/histogram_icon.png"), tr("Line View"), _pLiveFeedViewControl);
	_pLineViewAction->setObjectName("LineViewAction");
	_pLineViewAction->setCheckable(true);
	_pLineViewAction->setEnabled(false);
	_pDPFAction = new QAction(QIcon(":/files/DPF_icon.png"), tr("Defective Pixels"), _pLiveFeedViewControl);
	_pDPFAction->setObjectName("DefectivePixelsAction");
	_pDPFAction->setCheckable(true);
	_pDPFAction->setEnabled(false);

	_pLiveFeedViewControl->addAction(_pConnectAction);
	_pLiveFeedViewControl->addAction(_pDisconnectAction);
	_pLiveFeedViewControl->addWidget(_pHWVersionOption);
	_pLiveFeedViewControl->addSeparator();
	_pLiveFeedViewControl->addAction(_pResumeAction);
	_pLiveFeedViewControl->addAction(_pPauseAction);
	_pLiveFeedViewControl->addAction(_pRecordAction);
	_pLiveFeedViewControl->addWidget(_pFramesNum);
	_pLiveFeedViewControl->addSeparator();
    _pLiveFeedViewControl->addAction(_pPDPAction);
	_pLiveFeedViewControl->addAction(_pENCAction);
	_pLiveFeedViewControl->addAction(_pDISPAction);
	_pLiveFeedViewControl->addAction(_pDEAction);
	_pLiveFeedViewControl->addAction(_pHDRAction);
	_pLiveFeedViewControl->addAction(_pRAW2DAction);
	_pLiveFeedViewControl->addSeparator();
	_pLiveFeedViewControl->addAction(_pCaptureGalleryAction);
	_pLiveFeedViewControl->addAction(_pHWInfoAction);
	_pLiveFeedViewControl->addAction(_pHistogramAction);
	_pLiveFeedViewControl->addAction(_pVectorscopeAction);
	_pLiveFeedViewControl->addAction(_pLineViewAction);
	_pLiveFeedViewControl->addWidget(_pLineViewOption);
	_pLiveFeedViewControl->addAction(_pDPFAction);

	addToolBar(Qt::TopToolBarArea, _pLiveFeedViewControl);

	QObject::connect(_pConnectAction, SIGNAL(triggered()), actionConnect, SIGNAL(triggered()));
	QObject::connect(_pDisconnectAction, SIGNAL(triggered()), actionDisconnect, SIGNAL(triggered()));
	QObject::connect(_pHWVersionOption, SIGNAL(currentIndexChanged(int)), this, SLOT(HWVersionChanged(int)));
    QObject::connect(_pPDPAction, SIGNAL(triggered()), this, SLOT(enablePDP()));
	QObject::connect(_pENCAction, SIGNAL(triggered()), this, SLOT(changeImageType()));
	QObject::connect(_pDISPAction, SIGNAL(triggered()), this, SLOT(changeImageType()));
	QObject::connect(_pDEAction, SIGNAL(triggered()), this, SLOT(changeImageType()));
	QObject::connect(_pHDRAction, SIGNAL(triggered()), this, SLOT(changeImageType()));
	QObject::connect(_pRAW2DAction, SIGNAL(triggered()), this, SLOT(changeImageType()));
	QObject::connect(_pResumeAction, SIGNAL(triggered()), this, SLOT(enableImagesSending()));
	QObject::connect(_pPauseAction, SIGNAL(triggered()), this, SLOT(disableImagesSending()));
	QObject::connect(_pRecordAction, SIGNAL(triggered()), this, SLOT(startRecording()));
	QObject::connect(_pCaptureGalleryAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pHWInfoAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pHistogramAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pVectorscopeAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pLineViewAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));
	QObject::connect(_pLineViewOption, SIGNAL(currentIndexChanged(int)), this, SLOT(lineViewOptionChanged()));
	QObject::connect(_pDPFAction, SIGNAL(triggered()), this, SLOT(changeShowViewWidget()));

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(_pLiveFeedViewControl);
	}
	else
	{
		emit logError("Couldn,t register ImageviewControl!", "MainWindow::initLiveFeedViewControl()");
	}
}

void VisionLive::MainWindow::initHWInfo()
{
	_pHWInfo = new HWInfo();
	showViewLayout->addWidget(_pHWInfo);
	_pHWInfo->hide();
}

void VisionLive::MainWindow::showHWInfo(bool option)
{
	if(option) _pHWInfo->show();
	else _pHWInfo->hide();

	actionHW_Info->setChecked(option);
	_pHWInfoAction->setChecked(option);
}

void VisionLive::MainWindow::initHistogram()
{
	_pHistogram = new Histogram();
	showViewLayout->addWidget(_pHistogram);
	_pHistogram->hide();
}

void VisionLive::MainWindow::showHistogram(bool option)
{
	if(option) _pHistogram->show();
	else _pHistogram->hide();

	actionHistogram->setChecked(option);
	_pHistogramAction->setChecked(option);

	if(_pProxyHandler)
	{
		_pProxyHandler->setGenerateHistograms(option);
	}
}

void VisionLive::MainWindow::initVectorScope()
{
	_pVectorScope = new VectorScope();
	showViewLayout->addWidget(_pVectorScope);
	_pVectorScope->hide();
}

void VisionLive::MainWindow::showVectorscope(bool option)
{
	if(option) _pVectorScope->show();
	else _pVectorScope->hide();

	actionVectorscope->setChecked(option);
	_pVectorscopeAction->setChecked(option);

	if(_pProxyHandler)
	{
		_pProxyHandler->setGenerateVectorscope(option);
	}
}

void VisionLive::MainWindow::initLineView()
{
	_pLineView = new LineView();
	showViewLayout->addWidget(_pLineView);
	_pLineView->hide();
}

void VisionLive::MainWindow::showLineView(bool option)
{
	if(option)
	{
		_pLineView->setLineOption(_pLineViewOption->currentIndex());
		_pLineView->show();
		if(_pLiveFeedView)
		{
			_pLiveFeedView->setLiveViewOption(_pLineViewOption->currentIndex()+1);
		}
	}
	else 
	{
		_pLineView->hide();
		if(_pLiveFeedView)
		{
			_pLiveFeedView->setLiveViewOption(0);
		}
	}

	actionLineView->setChecked(option);
	_pLineViewAction->setChecked(option);

	if(_pProxyHandler)
	{
		_pProxyHandler->setGenerateLineView(option);
	}
}

void VisionLive::MainWindow::showDPF(bool option)
{
	_DPFDisplayed = option;

	actionDefective_Pixels->setChecked(option);
	_pDPFAction->setChecked(option);

	if(_pProxyHandler)
	{
		_pProxyHandler->setMarkDPFPoints(option);
	}
}

void VisionLive::MainWindow::initProxyHandler()
{
	_pProxyHandler = new ProxyHandler();

	QObject::connect(_pProxyHandler, SIGNAL(connected()), this, SLOT(connected()));
	QObject::connect(_pProxyHandler, SIGNAL(disconnected()), this, SLOT(disconnected()));
	QObject::connect(_pProxyHandler, SIGNAL(connectionCanceled()), this, SLOT(connectionCanceled()));

	QObject::connect(_pProxyHandler, SIGNAL(riseError(const QString &)), this, SLOT(riseError(const QString &)));

	QObject::connect(_pProxyHandler, SIGNAL(notifyStatus(const QString &)), this, SLOT(notifyStatus(const QString &)));

	QObject::connect(_pProxyHandler, SIGNAL(recordingProgress(int, int)), this, SLOT(recordingProgress(int, int)));
	QObject::connect(_pProxyHandler, SIGNAL(movieReceived(RecData *)), this, SLOT(movieReceived(RecData *)));

	QObject::connect(_pProxyHandler, SIGNAL(pixelInfoReceived(QPoint, std::vector<int>)), this, SLOT(pixelInfoReceived(QPoint, std::vector<int>)));

	QObject::connect(_pProxyHandler, SIGNAL(HWInfoReceived(CI_HWINFO)), this, SLOT(HWInfoReceived(CI_HWINFO)));

    QObject::connect(_pProxyHandler, SIGNAL(FB_received(const ISPC::ParameterList)), this, SLOT(FB_received(const ISPC::ParameterList)));
}

void VisionLive::MainWindow::initTest()
{
    if (_pTest)
    {
        _pTest->deleteLater();
    }

    if (_testMode)
    {
        _pTest = new Test(_pObjReg, _testFile, _testStart);
    }
    else
    {
        _pTest = new Test(_pObjReg);
    }

    if (_pProxyHandler)
    {
        _pTest->setProxyHandler(_pProxyHandler);
    }

    if (actionDefault->isChecked())
        _pTest->setStyleSheet(CSS_DEFAULT_STYLESHEET);
    else if (actionCustom->isChecked())
        _pTest->setStyleSheet(CSS_CUSTOM_STYLESHEET);

    QObject::connect(_pTest, SIGNAL(testClosed()), this, SLOT(testToolClosed()));
    QObject::connect(_pTest, SIGNAL(triggerAction(QAction *)), this, SLOT(triggerAction(QAction *)));
    QObject::connect(_pTest, SIGNAL(applyConfiguration()), this, SLOT(applyConfiguration()));
    QObject::connect(_pTest, SIGNAL(loadConfiguration(const QString &)), this, SLOT(loadConfiguration(const QString &)));
    QObject::connect(_pTest, SIGNAL(saveConfiguration(const QString &)), this, SLOT(saveConfiguration(const QString &)));
    QObject::connect(_pTest, SIGNAL(record(const QString &, unsigned int)), this, SLOT(startRecording(const QString &, unsigned int)));
    QObject::connect(_pTest, SIGNAL(connect(unsigned int)), this, SLOT(connect(unsigned int)));
    QObject::connect(_pTest, SIGNAL(disconnect()), this, SLOT(disconnect()));
    QObject::connect(this, SIGNAL(ISPC_connected()), _pTest, SLOT(connected()));
    QObject::connect(this, SIGNAL(ISPC_disconnected()), _pTest, SLOT(disconnected()));

    _connected ? _pTest->connected() : _pTest->disconnected();

    if (_pObjReg)
    {
        QObject::connect(_pObjReg, SIGNAL(objectRegistered(const QString &, const QString &)), _pTest, SLOT(addObject(const QString &, const QString &)));
        QObject::connect(_pObjReg, SIGNAL(objectDeregistered(const QString &)), _pTest, SLOT(removeObject(const QString &)));
    }

    _pTest->show();
}

void VisionLive::MainWindow::initObjectRegistry()
{
	_pObjReg = new ObjectRegistry();
}

void VisionLive::MainWindow::setSplitters(int l, int r, int u, int d)
{
	QList<int> list;
	list << u << d;
	splitter->setSizes(list);
	list.clear();
	list << l << r;
	splitter_2->setSizes(list);
}

void VisionLive::MainWindow::retranslate()
{
	Ui::MainWindow::retranslateUi(this);
}

//
// Public Slots
//

void VisionLive::MainWindow::loadConfiguration(const QString &filename)
{
	QString fileToUse = filename;
	if(fileToUse.isEmpty())
    {
		fileToUse = QFileDialog::getOpenFileName(this, tr("Load configuration"), ".", tr("Parameter file (*.txt)"));
    }

	emit logAction("CALL loadConfiguration() " + fileToUse);

	if(fileToUse.isEmpty()) 
	{
		return;
	}

	_config = ISPC::ParameterFileParser::parseFile(std::string(fileToUse.toLatin1()));

	// Update modules
	for(unsigned int i = 0; i < _modules.size(); i++)
    {
		_modules[i]->load(_config);
    }
}

void VisionLive::MainWindow::saveConfiguration(const QString &filename)
{
	QString fileToUse = filename;

    if(fileToUse.isEmpty())
    {
        fileToUse = QFileDialog::getSaveFileName(this, tr("Save configuration"), ".", tr("Parameter file (*.txt)"));
    }

	emit logAction("CALL saveConfiguration() " + fileToUse);

	if(fileToUse.isEmpty())
    {
        return;
    }

	// Save modules congigurations
    saveModulesParams();

	if(ISPC::ParameterFileParser::saveGrouped(_config, std::string(fileToUse.toLatin1())) != IMG_SUCCESS)
    {
		emit riseError(tr("Failed to save configuration into ") + fileToUse);
    }
    else
    {
		emit notifyStatus(tr("Parameters saved"));
    }
}

void VisionLive::MainWindow::connect(unsigned int port)
{
	if(_pProxyHandler && !_pProxyHandler->isRunning())
	{
		unsigned int portToUse = port;
		if(port == 0)
		{
			bool ok;
			portToUse = QInputDialog::getInt(this, tr("New Connection"), tr("Port:"), 2346, 1025, 65535, 1, &ok);
			if(!ok)
			{
				return;
			}
		}

		if(!_pWaitingConnection)
		{
			_pWaitingConnection = new QMessageBox(QMessageBox::Information, tr("Connection"), tr("Waiting for a client to connect on port %n...", "", portToUse), QMessageBox::Cancel, this);
			QObject::connect(_pWaitingConnection, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(cancelConnection(QAbstractButton *)));
			_pWaitingConnection->show();
		}

		_pProxyHandler->connect(portToUse, portToUse+1);

		emit logAction(QString("CALL connect() ") + QString::number(portToUse));
	}
}

void VisionLive::MainWindow::connected()
{
	if(_pWaitingConnection)
	{
		_pWaitingConnection->close();
		_pWaitingConnection = nullptr;
	}

	actionConnect->setEnabled(false);
	actionDisconnect->setEnabled(true);

	_pConnectAction->setEnabled(false);
	_pDisconnectAction->setEnabled(true);
	actionApply_Configuration->setEnabled(true);

    _pRecordAction->setEnabled(true);

    _pPDPAction->setEnabled(true);

	_pENCAction->setEnabled(true);
	_pDISPAction->setEnabled(false);
	_pDEAction->setEnabled(true);
	_pHDRAction->setEnabled(true);
	_pRAW2DAction->setEnabled(true);

	actionGallery->setEnabled(true);
	actionHW_Info->setEnabled(true);
	actionHistogram->setEnabled(true);
	actionVectorscope->setEnabled(true);
	actionLineView->setEnabled(true);
	actionDefective_Pixels->setEnabled(true);

	_pCaptureGalleryAction->setEnabled(true);
	_pHWInfoAction->setEnabled(true);
	_pHistogramAction->setEnabled(true);
	_pVectorscopeAction->setEnabled(true);
	_pLineViewAction->setEnabled(true);
	_pDPFAction->setEnabled(true);

    _connected = true;

	if(_pCaptureGallery)
	{
		_pCaptureGallery->setProxyHandler(_pProxyHandler);
	}

	for(unsigned int i = 0; i < _modules.size(); i++)
	{
		if(_modules[i])
		{
			_modules[i]->setProxyHandler(_pProxyHandler);

			// Set again all "instant" controls (needed after reconnectin)
			_modules[i]->resetConnectionDependantControls();
		}
	}

	if(_pTest)
	{
		_pTest->setProxyHandler(_pProxyHandler);
	}

	emit ISPC_connected();
}

void VisionLive::MainWindow::disconnect()
{
	if(_pProxyHandler && _pProxyHandler->isRunning())
	{
		if(!_pWaitingDiscommection)
		{
			_pWaitingDiscommection = new QMessageBox(QMessageBox::Information, tr("Connection"), tr("Disconnecting..."), QMessageBox::Ok, this);
			_pWaitingDiscommection->show();
		}

		_pProxyHandler->disconnect();

		emit logAction(QString("CALL disconnect()"));
	}
}

void VisionLive::MainWindow::disconnected()
{
	if(_pWaitingDiscommection)
	{
		_pWaitingDiscommection->close();
		_pWaitingDiscommection = nullptr;
	}

	actionConnect->setEnabled(true);
	actionDisconnect->setEnabled(false);
	actionApply_Configuration->setEnabled(false);

	_pConnectAction->setEnabled(true);
	_pDisconnectAction->setEnabled(false);

	_pPauseAction->setEnabled(true);
	_pResumeAction->setEnabled(false);
	_pRecordAction->setEnabled(false);

    _pPDPAction->setEnabled(false);

	_pENCAction->setEnabled(false);
	_pDISPAction->setEnabled(false);
	_pDEAction->setEnabled(false);
	_pHDRAction->setEnabled(false);
	_pRAW2DAction->setEnabled(false);

	actionGallery->setEnabled(false);
	actionHW_Info->setEnabled(false);
	actionHistogram->setEnabled(false);
	actionVectorscope->setEnabled(false);
	actionLineView->setEnabled(false);
	actionDefective_Pixels->setEnabled(false);

	_pCaptureGalleryAction->setEnabled(false);
	_pHWInfoAction->setEnabled(false);
	_pHistogramAction->setEnabled(false);
	_pVectorscopeAction->setEnabled(false);
	_pLineViewAction->setEnabled(false);
	_pDPFAction->setEnabled(false);

    _connected = false;

	emit ISPC_disconnected();

	QMessageBox::information(this, tr("Message"), tr("Connection closed"));

	if(closeVisionLive)
	{
		close();
	}
}

void VisionLive::MainWindow::cancelConnection(QAbstractButton *botton)
{
	Q_UNUSED(botton);

	if(_pWaitingConnection)
	{
		_pWaitingConnection->close();
		_pWaitingConnection = nullptr;
	}

	if(!_pWaitingDiscommection)
	{
		_pWaitingDiscommection = new QMessageBox(QMessageBox::Information, tr("Connection"), tr("Terminating connecting..."), QMessageBox::Ok, this);
		_pWaitingDiscommection->show();
	}

	_pProxyHandler->cancelConnection();

	emit logAction(QString("CALL cancelConnection()"));
}

void VisionLive::MainWindow::connectionCanceled()
{
	if(_pWaitingDiscommection)
	{
		_pWaitingDiscommection->close();
		_pWaitingDiscommection = nullptr;
	}

	QMessageBox::information(this, tr("Message"), tr("Connectingn terminated"));
}

void VisionLive::MainWindow::applyConfiguration()
{
	if(_pProxyHandler && _pProxyHandler->isRunning())
	{
		for(unsigned int i = 0; i < _modules.size(); i++)
		{
			_modules[i]->save(_config);
		}
		_pProxyHandler->applyConfiguration(_config);
	}

	for(unsigned int i = 0; i < _modules.size(); i++)
	{
		_modules[i]->removeObjectValueChangedMark();
	}

	// Due to css issues disable changing styleSheet option after applying parameters
	menuSet_Style_Sheet->setEnabled(false);

	emit logAction("CALL applyConfiguration()");
}

void VisionLive::MainWindow::changeStyleSheet()
{
	QString styleSheetName = ((QAction *)sender())->text();

	if(styleSheetName == tr(NAME_MAINWINDOW_MENUBAR_STYLESHEETMENU_DEFAULTACTION))
	{
		setStyleSheet(CSS_DEFAULT_STYLESHEET);
		for(unsigned int i = 0; i < _modules.size(); i++)
		{
			_modules[i]->setCSS(VisionLive::DEFAULT_CSS);
		}
		if(_pCaptureGallery) _pCaptureGallery->setCSS(VisionLive::DEFAULT_CSS);
		if(_pTest) _pTest->setStyleSheet(CSS_DEFAULT_STYLESHEET);
	}
	else
	{
		actionDefault->setChecked(false);
	}

	if(styleSheetName == tr(NAME_MAINWINDOW_MENUBAR_STYLESHEETMENU_CUSTOMACTION))
	{
		setStyleSheet(CSS_CUSTOM_STYLESHEET);
		for(unsigned int i = 0; i < _modules.size(); i++)
		{
			_modules[i]->setCSS(VisionLive::CUSTOM_CSS);
		}
		if(_pCaptureGallery) _pCaptureGallery->setCSS(VisionLive::CUSTOM_CSS);
		if(_pTest) _pTest->setStyleSheet(CSS_CUSTOM_STYLESHEET);
	}
	else
	{
		actionCustom->setChecked(false);
	}
}

void VisionLive::MainWindow::riseError(const QString &error)
{
	QMessageBox::critical(this, tr("Error"), error);
}

void VisionLive::MainWindow::notifyStatus(const QString &status)
{
	if(_pStatusBar)
	{
		_pStatusBar->showMessage(status);
	}
}

void VisionLive::MainWindow::changeModuleViewWidget(int index)
{
	VisionLive::Type objectType = VisionLive::Algorithms::getObjectType(sender());

	if(objectType == VisionLive::QACTION)
	{
		QString moduleName = ((QAction *)sender())->text();

		for(unsigned int i = 0; i < _modules.size(); i++)
		{
			if(_pModuleView->tabText(i) == moduleName)
			{
				menuModule_Access->actions()[_pModuleView->currentIndex()]->setChecked(false);
				_pModuleView->setCurrentIndex(i);
				menuModule_Access->actions()[_pModuleView->currentIndex()]->setChecked(true);
				break;
			}
		}

		setSplitters(this->width()/2, this->width()/2, 1, 0);
	}
	else
	{
		QList<QAction *> actions = menuModule_Access->actions();
		for(int i = 0; i < actions.size(); i++)
		{
			if(actions[i]->text() == _pModuleView->tabText(_pModuleView->currentIndex()))
			{
				menuModule_Access->actions()[i]->setChecked(true);
			}
			else
			{
				menuModule_Access->actions()[i]->setChecked(false);
			}
		}
	}
}

void VisionLive::MainWindow::changeShowViewWidget()
{
	QAction *showAction = ((QAction *)sender());
	QString showActionName = ((QAction *)sender())->text();

	// Show CaptureGallery
	if(showAction == actionGallery || showAction == _pCaptureGalleryAction)
	{
		showCaptureGallery(_pCaptureGallery->isHidden());
	}
	else
	{
		showCaptureGallery(false);
	}

	// Show HW Info
	if(showAction == actionHW_Info || showAction == _pHWInfoAction)
	{
		showHWInfo(_pHWInfo->isHidden());
	}
	else
	{
		showHWInfo(false);
	}

	// Show Histogram
	if(showAction == actionHistogram || showAction == _pHistogramAction)
	{
		showHistogram(_pHistogram->isHidden());
	}
	else
	{
		showHistogram(false);
	}

	// Show Vectorscope
	if(showAction == actionVectorscope || showAction == _pVectorscopeAction)
	{
		showVectorscope(_pVectorScope->isHidden());
	}
	else
	{
		showVectorscope(false);
	}

	// Show LineView
	if(showAction == actionLineView || showAction == _pLineViewAction)
	{
		showLineView(_pLineView->isHidden());
	}
	else
	{
		showLineView(false);
	}

	// Show DPF
	if(showAction == actionDefective_Pixels || showAction == _pDPFAction)
	{
		showDPF(!_DPFDisplayed);
	}
	else
	{
		showDPF(false);
	}
}

void VisionLive::MainWindow::lineViewOptionChanged()
{
	if(_pLineView)
	{
		_pLineView->setLineOption(_pLineViewOption->currentIndex());

		if(_pLiveFeedView && !_pLineView->isHidden())
		{
			_pLiveFeedView->setLiveViewOption(_pLineViewOption->currentIndex()+1);
		}
	}
}

void VisionLive::MainWindow::triggerAction(QAction *action)
{
	emit action->trigger();
}

void VisionLive::MainWindow::enableImagesSending()
{
	emit logAction(QString("CALL enableImagesSending()"));

	if(_pProxyHandler)
	{
		_pProxyHandler->setImageSending();
	}

	_pResumeAction->setEnabled(false);
	_pPauseAction->setEnabled(true);
}

void VisionLive::MainWindow::disableImagesSending()
{
	emit logAction(QString("CALL disableImagesSending()"));

	if(_pProxyHandler)
	{
		_pProxyHandler->setImageSending();
	}

	_pResumeAction->setEnabled(true);
	_pPauseAction->setEnabled(false);
}

void VisionLive::MainWindow::startRecording(const QString &name, unsigned int nFrames)
{
	QString recordName = name;

	if(recordName.isEmpty())
	{
		bool ok;
		recordName = QInputDialog::getText(this, tr("Recording"), tr("Name:"), QLineEdit::Normal, QString(), &ok);
		if(!ok || recordName.isEmpty())
		{
			return;
		}
	}

	// Prevent recording under already used name
	if(_pCaptureGallery)
	{
		QStringList captures = _pCaptureGallery->getItemsList();
		for(int i = 0; i < captures.size(); i++)
		{
			if(captures[i] == recordName)
			{
				emit riseError(QStringLiteral("Recording failed - name already in use!"));
				return;
			}
		}
	}

	if(nFrames > 0)
	{
		emit logAction(QString("CALL record() ") + QString::number(nFrames) + " " + name);
	}
	else
	{
		emit logAction(QString("CALL record() ") + QString::number(_pFramesNum->value()) + " " + name);
	}

	if(_pProxyHandler)
	{
		if(nFrames > 0)
		{
			_pProxyHandler->record(nFrames, recordName);
		}
		else
		{
			_pProxyHandler->record(_pFramesNum->value(), recordName);
		}

		_pRecordAction->setEnabled(false);
		_pFramesNum->setEnabled(false);

		if(!_pWaitingRecording)
		{
			_pWaitingRecording = new QMessageBox(QMessageBox::Information, tr("Recording"), tr("Recording, please wait..."), QMessageBox::Cancel, this);
			if(_pWaitingRecording->exec() == QMessageBox::Cancel)
			{
				_pRecordAction->setEnabled(true);
				_pFramesNum->setEnabled(true);
			}
		}
	}
}

void VisionLive::MainWindow::recordingProgress(int currFrame, int maxFrame)
{
	if(_pWaitingRecording)
	{
		_pWaitingRecording->setText(tr("Recording progress: ") + QString::number(currFrame) + "/" + QString::number(maxFrame));
	}
}

void VisionLive::MainWindow::movieReceived(RecData *recData)
{
	if(_pWaitingRecording)
	{
		_pWaitingRecording->close();
		_pWaitingRecording = nullptr;
	}

	if(!actionGallery->isChecked())
	{
		triggerAction(actionGallery);
	}

	_pRecordAction->setEnabled(true);
	_pFramesNum->setEnabled(true);

	if(_pCaptureGallery && recData)
	{
		_pCaptureGallery->addItem(new CaptureGalleryItem(recData));
	}
}

void VisionLive::MainWindow::changeImageType()
{
	if(_pProxyHandler)
	{
		if(sender()->objectName() == "ENCAction")
		{
			emit logAction(QString("CALL changeImageType() ENC"));

			_pProxyHandler->setImageType(YUV);

			_pENCAction->setEnabled(false);
			_pDISPAction->setEnabled(true);
			_pDEAction->setEnabled(true);
			_pHDRAction->setEnabled(true);
			_pRAW2DAction->setEnabled(true);
		}
		else if(sender()->objectName() == "DISPAction")
		{
			emit logAction(QString("CALL changeImageType() DISP"));

			_pProxyHandler->setImageType(RGB);

			_pENCAction->setEnabled(true);
			_pDISPAction->setEnabled(false);
			_pDEAction->setEnabled(true);
			_pHDRAction->setEnabled(true);
			_pRAW2DAction->setEnabled(true);
		}
		else if(sender()->objectName() == "DEAction")
		{
			emit logAction(QString("CALL changeImageType() DE"));

			_pProxyHandler->setImageType(DE);

			_pENCAction->setEnabled(true);
			_pDISPAction->setEnabled(true);
			_pDEAction->setEnabled(false);
			_pHDRAction->setEnabled(true);
			_pRAW2DAction->setEnabled(true);
		}
		else if(sender()->objectName() == "HDRAction")
		{
			emit logAction(QString("CALL changeImageType() HDR"));

			_pProxyHandler->setImageType(HDR);

			_pENCAction->setEnabled(true);
			_pDISPAction->setEnabled(true);
			_pDEAction->setEnabled(true);
			_pHDRAction->setEnabled(false);
			_pRAW2DAction->setEnabled(true);
		}
		else if(sender()->objectName() == "RAW2DAction")
		{
			emit logAction(QString("CALL changeImageType() RAW2D"));

			_pProxyHandler->setImageType(RAW2D);

			_pENCAction->setEnabled(true);
			_pDISPAction->setEnabled(true);
			_pDEAction->setEnabled(true);
			_pHDRAction->setEnabled(true);
			_pRAW2DAction->setEnabled(false);
		}
		else
		{
			emit logError(tr("Unrecognized image type selected!"), tr("MainWindow::changeImageType()"));
		}
	}

	if(_pHistogram)
	{
		if(sender()->objectName() == "ENCAction")
		{
			_pHistogram->setPlotColors(Qt::black, Qt::yellow, Qt::darkYellow, Qt::blue);
		}
		else
		{
			_pHistogram->setPlotColors(Qt::red, Qt::green, Qt::darkGreen, Qt::blue);
		}
	}

	if(_pLineView)
	{
		if(sender()->objectName() == "ENCAction")
		{
			_pLineView->setPlotColors(Qt::black, Qt::yellow, Qt::darkYellow, Qt::blue);
		}
		else
		{
			_pLineView->setPlotColors(Qt::red, Qt::green, Qt::darkGreen, Qt::blue);
		}
	}
}

void VisionLive::MainWindow::enablePDP()
{
    if (sender()->objectName() == "PDPAction" && _pProxyHandler)
    {
        emit logAction(QString("CALL enablePDP() ") + QString(_pPDPAction->isChecked() ? "enable" : "disable"));

        _pProxyHandler->PDP_set(_pPDPAction->isChecked());
    }
}

void VisionLive::MainWindow::pixelInfoReceived(QPoint pixel, std::vector<int> pixelInfo)
{
	if(_pModuleMIE)
	{
		_pModuleMIE->updatePixelInfo(pixel, pixelInfo);
	}
}

void VisionLive::MainWindow::HWInfoReceived(CI_HWINFO hwInfo)
{
	int major = hwInfo.rev_ui8Major;
	int minor = hwInfo.rev_ui8Minor;

	HW_VERSION hwVersion;

	if(major == 2 && minor < 4)
	{
        hwVersion = HW_2_X;
	}
	else if(major == 2 && minor == 4)
	{
		hwVersion = HW_2_4;
	}
	else if(major == 2 && minor == 6)
	{
		hwVersion = HW_2_6;
	}
	else if (major == 2 && minor == 7)
	{
		hwVersion = HW_2_7;
	}
	else if(major == 3)
	{
		hwVersion = HW_3_X;
	}
	else
	{
		hwVersion = HW_UNKNOWN;
	}

	_pHWVersionOption->setCurrentIndex((int)hwVersion);
	_pHWVersionOption->setEnabled(false);
	HWVersionChanged((int)hwVersion);
}

void VisionLive::MainWindow::HWVersionChanged(int hwv)
{
	for(unsigned int i = 0; i < _modules.size(); i++)
	{
		if(_modules[i])
		{
			_modules[i]->setHWVersion((HW_VERSION)hwv);
		}
	}
}

void VisionLive::MainWindow::aboutVisionLive()
{
	QMessageBox::about(this, TITLE_STR, VERSION_STR + "\n" + LICENCE_STR + "\n\n" + MESSAGE_STR);
}

void VisionLive::MainWindow::aboutQt()
{
	QMessageBox::aboutQt(this, tr("About Qt"));
}

void VisionLive::MainWindow::showISPDoc()
{
	if(_pISPDoc)
	{
		delete _pISPDoc;
	}

	_pISPDoc = new HTMLViewer();
	QObject::connect(_pISPDoc, SIGNAL(loadingFailed()), this, SLOT(ISPDocLoadingFailed()));
	_pISPDoc->show();

	QDir dir;
	dir.setPath(dir.absolutePath() + QStringLiteral("/html"));
	if(dir.exists())
	{
		_pISPDoc->loadFile(dir.path() + QStringLiteral("/index.html"));
	}
	else
	{
		delete _pISPDoc;
		_pISPDoc = nullptr;
		emit logError(QStringLiteral("Documentation path ") + dir.path() + QStringLiteral(" not found!"),  __FUNCTION__ + QStringLiteral(" ") + QString::number(__LINE__)); 
	}
}

void VisionLive::MainWindow::ISPDocLoadingFailed()
{
	emit logError(tr("Failed to load ISP Docummentation!"), __FUNCTION__ + QStringLiteral(" ") + QString::number(__LINE__));
}

void VisionLive::MainWindow::openTestTool()
{
    initTest();
}

void VisionLive::MainWindow::testToolClosed()
{
    _pTest = nullptr;
}

void VisionLive::MainWindow::saveModulesParams()
{
    for (auto &e:_modules) e->save(_config);
}

void VisionLive::MainWindow::FB_received(const ISPC::ParameterList list)
{
    if (_pTest)
    {
        saveModulesParams();
        _pTest->FB_received(_config, list);
    }
}