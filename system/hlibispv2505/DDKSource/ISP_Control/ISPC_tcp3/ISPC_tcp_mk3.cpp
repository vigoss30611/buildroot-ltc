// Control algorithms
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB.h>
#include <ispc/ControlAWB_PID.h>
#include <ispc/ControlAWB_Planckian.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlDNS.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlAF.h>
//#include <ispc/ControlLSH.h>

// Modules
#include "ispc/ModuleOUT.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ModuleTNM.h"
#include "ispc/ModuleHIS.h"
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ModuleWBS.h"
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
#include "ispc/ModuleLCA.h"

// Main
#include "ISPC_tcp_mk3.hpp"

#include <sensorapi/sensorapi.h>
#include "ispc/ParameterFileParser.h"
#include "felixcommon/lshgrid.h"
#include <ispc/Save.h>
#include <ispctest/info_helper.h>
#include <time.h>

#include "paramsocket/paramsocket.hpp"

#ifdef WIN32
	#include <WinSock2.h>
	#define YIELD YieldProcessor();
#else
	extern "C"
	{
	#include <errno.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#ifdef ANDROID
	// - include "sched.h"?
	#define YIELD sched_yield();
	#else
	#define YIELD pthread_yield();
	#endif
	}
#endif

#define LOG_TAG "ISPC_tcp_mk3"

#include <felixcommon/userlog.h>

#include "felix_hw_info.h"

#include "ci/ci_api_structs.h"

/*
 * Global
 */

void pauseThread(unsigned int ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	usleep(ms*1000);
#endif
}

pthread_mutex_t camHStateLock;
HandlerState camHState;
void setCamHState(HandlerState state)
{
	pthread_mutex_lock(&camHStateLock);
	camHState = state;
	pthread_mutex_unlock(&camHStateLock);
}
HandlerState getCamHState()
{
	HandlerState state;
	pthread_mutex_lock(&camHStateLock);
	state = camHState;
	pthread_mutex_unlock(&camHStateLock);
	return state;
}

pthread_mutex_t cmdHStateLock;
HandlerState cmdHState;
void setCmdHState(HandlerState state)
{
	pthread_mutex_lock(&cmdHStateLock);
	cmdHState = state;
	pthread_mutex_unlock(&cmdHStateLock);
}
HandlerState getCmdHState()
{
	HandlerState state;
	pthread_mutex_lock(&cmdHStateLock);
	state = cmdHState;
	pthread_mutex_unlock(&cmdHStateLock);
	return state;
}

pthread_mutex_t imgHStateLock;
HandlerState imgHState;
void setImgHState(HandlerState state)
{
	pthread_mutex_lock(&imgHStateLock);
	imgHState = state;
	pthread_mutex_unlock(&imgHStateLock);
}
HandlerState getImgHState()
{
	HandlerState state;
	pthread_mutex_lock(&imgHStateLock);
	state = imgHState;
	pthread_mutex_unlock(&imgHStateLock);
	return state;
}

pthread_mutex_t imgHCapturingLock;
bool imgHCapturing = false;
void setImgHCapturing(bool capturing)
{
	pthread_mutex_lock(&imgHCapturingLock);
	imgHCapturing = capturing;
	pthread_mutex_unlock(&imgHCapturingLock);
}
bool getImgHCapturing()
{
	bool capturing;
	pthread_mutex_lock(&imgHCapturingLock);
	capturing = imgHCapturing;
	pthread_mutex_unlock(&imgHCapturingLock);
	return capturing;
}

// Uncomment to log Camera object locking/unlocking
//#define LOG_LOCKING

/*
 * CameraHandler
 */

//
// Public Functions
//

CameraHandler::CameraHandler(
	char *appName,
	char *sensor,
	int sensorMode,
	int nBuffers,
	char *ip,
	int port,
	char *DGFrameFile,
	unsigned int gasket,
	unsigned int aBlanking[],
    bool aSensorFlip[])
{
	_pCommandHandler = NULL;
	_pImageHandler = NULL;
	_pCamera = NULL;

	pthread_mutexattr_init(&_hwVerLockAttr);
	pthread_mutexattr_settype(&_hwVerLockAttr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&_hwVerLock, &_hwVerLockAttr);

#if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 4
	_hwVersion = HW_2_X;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 6
	_hwVersion = HW_2_4;
#elif FELIX_VERSION_MAJ == 2
	_hwVersion = HW_2_6;
#elif FELIX_VERSION_MAJ == 3
	_hwVersion = HW_3_X;
#else
	_hwVersion = HW_UNKNOWN;
#endif

	_appName = appName;
	_ip = ip;
	_port = port;

	_sensor = sensor;
	_sensorMode = sensorMode;
	_sensorFlip = (aSensorFlip[0]?SENSOR_FLIP_HORIZONTAL:SENSOR_FLIP_NONE) |
				  (aSensorFlip[1]?SENSOR_FLIP_VERTICAL:SENSOR_FLIP_NONE);
	_nBuffers = nBuffers;

    //configValid = false;

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

	pthread_mutexattr_init(&_runLockAttr);
	pthread_mutexattr_settype(&_runLockAttr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&_runLock, &_runLockAttr);

	pthread_mutexattr_init(&_cameraLockAttr);
	pthread_mutexattr_settype(&_cameraLockAttr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&_cameraLock, &_cameraLockAttr);

	_run = false;
	_pause = false;

	_fps = 0.00f;
	_frameCount = 0;

	_dpfStats.ui32DroppedMapModifications = 0;
	_dpfStats.ui32FixedPixels = 0;
	_dpfStats.ui32MapModifications = 0;
	_dpfStats.ui32NOutCorrection = 0;

	_ensStats.data = 0;
	_ensStats.elementSize = 0;
	_ensStats.size = 0;
	_ensStats.stride = 0;

    _PDPState = false;

	setCamHState(STOPPED);
	setCmdHState(DISCONNECTED);
	setImgHState(DISCONNECTED);
}

CameraHandler::~CameraHandler()
{
	if(_pCommandHandler)
	{
		delete _pCommandHandler;
	}

	if(_pImageHandler)
	{
		delete _pImageHandler;
	}

	if(_pCamera)
	{
		delete _pCamera;
	}
}

void CameraHandler::start()
{
	initCommandHandler(_appName, _ip, _port);

	if(getCmdHState() != CONNECTED)
	{
		LOG_ERROR("initCommandHandler() failed\n");
		return;
	}

	//pauseThread(2000);

	initImageHandler(_appName, _ip, _port + 1);

	if(getImgHState() != CONNECTED)
	{
		LOG_ERROR("initImageHandler() failed\n");
		return;
	}

	//pauseThread(2000);

	pthread_mutex_lock(&_runLock);
	_run = (initCameraHandler() == IMG_SUCCESS);
	pthread_mutex_unlock(&_runLock);

	run();
}

bool CameraHandler::isRunning()
{
	bool isRunning;
	pthread_mutex_lock(&_runLock);
	isRunning = _run;
	pthread_mutex_unlock(&_runLock);
	return isRunning;
}

void CameraHandler::stop()
{
	pthread_mutex_lock(&_runLock);
	_run = false;
	pthread_mutex_unlock(&_runLock);
}

void CameraHandler::pause()
{
	if(getCamHState() == RUNNING)
	{
		pthread_mutex_lock(&_runLock);
		_pause = true;
		pthread_mutex_unlock(&_runLock);
		while(getCamHState() != PAUSED) {pauseThread(5);}
	}
}

void CameraHandler::pauseCmdH()
{
	if(_pCommandHandler)
	{
		_pCommandHandler->pause();
	}
}

void CameraHandler::pauseImgH()
{
	if(_pImageHandler)
	{
		_pImageHandler->pause();
	}
}

bool CameraHandler::isPaused()
{
	bool isPaused;
	pthread_mutex_lock(&_runLock);
	isPaused = _pause;
	pthread_mutex_unlock(&_runLock);
	return isPaused;
}

void CameraHandler::resume()
{
	if(getCamHState() == PAUSED)
	{
		pthread_mutex_lock(&_runLock);
		_pause = false;
		pthread_mutex_unlock(&_runLock);
		while(getCamHState() != RUNNING) {pauseThread(5);}
	}
}

void CameraHandler::resumeCmdH()
{
	if(_pCommandHandler)
	{
		_pCommandHandler->resume();
	}
}

void CameraHandler::resumeImgH()
{
	if(_pImageHandler)
	{
		_pImageHandler->resume();
	}
}

ISPC::Camera *CameraHandler::takeCamera(int line)
{
	lock(&_cameraLock, line);
	return _pCamera;
}

void CameraHandler::releaseCamera(int line)
{
	unlock(&_cameraLock, __LINE__);
}

MC_STATS_DPF &CameraHandler::takeDPFStats(int line)
{
	lock(&_cameraLock, __LINE__);
	return _dpfStats;
}

void CameraHandler::releaseDPFStats(int line)
{
	unlock(&_cameraLock, __LINE__);
}

ISPC::Statistics &CameraHandler::takeENSStats(int line)
{
	lock(&_cameraLock, __LINE__);
	return _ensStats;
}

void CameraHandler::releaseENSStats(int line)
{
	unlock(&_cameraLock, __LINE__);
}

int CameraHandler::startCamera()
{
	lock(&_cameraLock, __LINE__);

	if(NL_StartCamera() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_StartCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::stopCamera()
{
	lock(&_cameraLock, __LINE__);

	if(NL_StopCamera() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_StopCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::takeShot(ISPC::Shot &frame)
{
	lock(&_cameraLock, __LINE__);

	if(NL_TakeShot(frame) != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_TakeShot() failed\n");
		return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::releaseShot(ISPC::Shot &frame)
{
	lock(&_cameraLock, __LINE__);

	if(NL_ReleaseShot(frame) != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_ReleaseShot() failed\n");
		return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::startCapture()
{
	lock(&_cameraLock, __LINE__);

	if(!_pCamera)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCamera->startCapture() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
	   LOG_ERROR("failed to start the capture\n");
	   return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::stopCapture()
{
	lock(&_cameraLock, __LINE__);

	if(!_pCamera)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCamera->stopCapture() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
	   LOG_ERROR("stopCapture() failed\n");
	   return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::allocateBufferPool(unsigned int nBuffers, std::list<IMG_UINT32> &bufferIds)
{
	lock(&_cameraLock, __LINE__);

	if(!_pCamera)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCamera->allocateBufferPool(nBuffers, bufferIds) != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
	   LOG_ERROR("allocateBufferPool() failed\n");
	   return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::deallocateBufferPool(std::list<IMG_UINT32> &bufferIds)
{
	lock(&_cameraLock, __LINE__);

	if(!_pCamera)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	LOG_DEBUG("BufferIds size %d\n", bufferIds.size());
	std::list<IMG_UINT32>::iterator it;
    for(it = bufferIds.begin(); it != bufferIds.end(); it++)
    {
		LOG_DEBUG("Deregister buffer %d\n", *it);
		if(_pCamera->deregisterBuffer(*it) != IMG_SUCCESS)
		{
			unlock(&_cameraLock, __LINE__);
			LOG_ERROR("deregisterBuffer() failed on %d buffer\n", *it);
			return IMG_ERROR_FATAL;
		}
	}
    bufferIds.clear();

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::updateCameraParameters(ISPC::ParameterList &list)
{
	pause();
	if (getImgHState() == RUNNING) pauseImgH();

	lock(&_cameraLock, __LINE__);

	if(NL_StopCamera() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_StopCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	// Store current CCM and AWB
	ISPC::ModuleCCM *ccm = _pCamera->getModule<ISPC::ModuleCCM>();
    ISPC::ModuleWBC *wbc = _pCamera->getModule<ISPC::ModuleWBC>();

	ISPC::ControlAWB *awb;
	if (getHWVersion() < HW_2_6)
	{
		awb = _pCamera->getControlModule<ISPC::ControlAWB_PID>();
	}
	else
	{
		awb = _pCamera->getControlModule<ISPC::ControlAWB_Planckian>();
	}

	if (!ccm || !wbc || !awb)
	{
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}

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

	//ISPC::ParameterList currentParameterList;
	//_pCamera->saveParameters(currentParameterList, ISPC::ModuleBase::SAVE_VAL);

	ISPC::ParameterList newParameterList;
	_pCamera->saveParameters(newParameterList, ISPC::ModuleBase::SAVE_VAL);

	newParameterList += list;

    if (_pCamera->loadParameters(newParameterList) != IMG_SUCCESS)
    {
        //LOG_ERROR("Error: loadParameters() failed!\n");
        unlock(&_cameraLock, __LINE__);
        LOG_ERROR("Error: loadParameters() failed!\n");
        return IMG_ERROR_FATAL;
        /*if (configValid)
        {
            LOG_ERROR("Loading previous parameter list!\n");
            if (_pCamera->loadParameters(currentParameterList) != IMG_SUCCESS)
            {
                unlock(&_cameraLock, __LINE__);
                LOG_ERROR("Error: loadParameters() failed!\n");
                return IMG_ERROR_FATAL;
            }
        }
        else
        {
            LOG_ERROR("Loading default parameter list!\n");
            if (_pCamera->loadParameters(ISPC::ParameterList()) != IMG_SUCCESS)
            {
                unlock(&_cameraLock, __LINE__);
                LOG_ERROR("Error: loadParameters() failed!\n");
                return IMG_ERROR_FATAL;
            }
        }*/
	}

    //configValid = true;

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

	if (_pCamera->setupModules() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
	   LOG_ERROR("Error: setupModules() failed!\n");
	   return IMG_ERROR_FATAL;
	}

	if(_pCamera->program() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Error: program() failed!\n");
		return IMG_ERROR_FATAL;
	}

	if(NL_StartCamera() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_StartCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	ISPC::ControlAE *ae = _pCamera->getControlModule<ISPC::ControlAE>();
	ISPC::ControlAF *af = _pCamera->getControlModule<ISPC::ControlAF>();

	if (!ae || !af)
	{
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}

	// SENSOR
	if(!ae->isEnabled()) // Don't apply those params when AE is enabled
	{
		if (newParameterList.exists("SENSOR_SET_EXPOSURE"))
		{
			_pCamera->getSensor()->setExposure(newParameterList.getParameter("SENSOR_SET_EXPOSURE")->get<int>());
		}
		if (newParameterList.exists("SENSOR_SET_GAIN"))
		{
			_pCamera->getSensor()->setGain(newParameterList.getParameter("SENSOR_SET_GAIN")->get<float>());
		}
	}

	// AF
	if (!af->isEnabled()) // Don't apply those params when AF is enabled or
	{
		if (newParameterList.exists("AF_DISTANCE"))
		{
			_pCamera->getSensor()->setFocusDistance(newParameterList.getParameter("AF_DISTANCE")->get<int>());
		}
	}

	// AWS
	if (_hwVersion >= HW_2_6)
	{
		ISPC::ModuleAWS *aws = _pCamera->getModule<ISPC::ModuleAWS>();
		if (_pImageHandler && aws)
		{
			_pImageHandler->setAWBDebugMode(aws->debugMode);
		}
	}

	unlock(&_cameraLock, __LINE__);

	if (getImgHState() == PAUSED) resumeImgH();
	resume();

	return IMG_SUCCESS;
}

int CameraHandler::setGMA(CI_MODULE_GMA_LUT &glut)
{
	pause();
	if (getImgHState() == RUNNING) pauseImgH();

	lock(&_cameraLock, __LINE__);

	if(NL_StopCamera() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_StopCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	CI_CONNECTION *conn  = _pCamera->getConnection();
	if(CI_DriverSetGammaLUT(conn, &glut) != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("CI_DriverSetGammaLUT() failed\n");
		return IMG_ERROR_FATAL;
	}

	if(NL_StartCamera() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("NL_StartCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	unlock(&_cameraLock, __LINE__);

	if (getImgHState() == PAUSED) resumeImgH();
	resume();

	return IMG_SUCCESS;
}

HW_VERSION CameraHandler::getHWVersion()
{
	HW_VERSION hwV;

	pthread_mutex_lock(&_hwVerLock);
	hwV = _hwVersion;
	pthread_mutex_unlock(&_hwVerLock);

	return hwV;
}

void CameraHandler::setPDP(bool enable)
{
    _PDPState = enable;
}

int CameraHandler::shutDown()
{
	LOG_INFO("Shutting down...\n");

	lock(&_cameraLock, __LINE__);
	if(_pCamera)
	{
		std::cout << "*****GASCET INFO*****" << std::endl;
		PrintGasketInfo(*_pCamera, std::cout);
		std::cout << "******RTM INFO*******" << std::endl;
		PrintRTMInfo(*_pCamera, std::cout);
		std::cout << "*********************" << std::endl;
	}
	unlock(&_cameraLock, __LINE__);

	if(_pCommandHandler)
	{
		_pCommandHandler->stop();
		while(getCmdHState() != STOPPED) {pauseThread(5);}
	}

	if(_pImageHandler)
	{
		_pImageHandler->stop();
		while(getImgHState() != STOPPED) {pauseThread(5);}
	}

	if(stopCamera() != IMG_SUCCESS)
	{
		LOG_ERROR("stopCamera() failed\n");
	}

	if(disableSensor() != IMG_SUCCESS)
	{
		LOG_ERROR("disableSensor() failed\n");
	}

	if(restoreInitialGMA() != IMG_SUCCESS)
	{
		LOG_ERROR("restoreInitialGMA() failed\n");
	}

#ifdef FELIX_FAKE
    finaliseFakeDriver(); // FAKE driver rmmod
#endif

	return IMG_SUCCESS;
}

//
// Private Functions
//

int CameraHandler::initCommandHandler(
		char *appName,
		char *ip,
		int port)
{
	_pCommandHandler = new CommandHandler(appName, ip, port, this);

	if(_pCommandHandler->connect() != IMG_SUCCESS)
	{
		return IMG_ERROR_FATAL;
	}

	_pCommandHandler->start();

	return IMG_SUCCESS;
}

int CameraHandler::initImageHandler(
		char *appName,
		char *ip,
		int port)
{
	_pImageHandler = new ImageHandler(appName, ip, port, this);

	if(_pImageHandler->connect() != IMG_SUCCESS)
	{
		return IMG_ERROR_FATAL;
	}

	_pImageHandler->start();

	return IMG_SUCCESS;
}

int CameraHandler::initCameraHandler()
{
	lock(&_cameraLock, __LINE__);

	ISPC::Sensor *sensor = NULL;
	if(strcmp(_sensor, IIFDG_SENSOR_INFO_NAME) == 0)
	{
		_dataGen = INT_DG;

        if(!ISPC::DGCamera::supportIntDG())
        {
            std::cerr << "ERROR trying to use internal data generator but it is not supported by HW\n" << std::endl;
			unlock(&_cameraLock, __LINE__);
			return IMG_ERROR_FATAL;
        }

		_pCamera = new ISPC::DGCamera(0, _DGFrameFile, _gasket, true);

		if(_pCamera->state != ISPC::Camera::CAM_ERROR)
		{
			sensor = _pCamera->getSensor();
		}

        // Apply additional parameters
		if(sensor)
		{
			//IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), _nBuffers + 1);
            if (IMG_SUCCESS != IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), _nBuffers + 1))
            {
                LOG_ERROR("failed to set the number of DG buffers\n");
                return EXIT_FAILURE;
            }
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
            LOG_ERROR("trying to use external data generator but it is not supported by HW\n");
			unlock(&_cameraLock, __LINE__);
            return IMG_ERROR_FATAL;
        }

        _pCamera = new ISPC::DGCamera(0, _DGFrameFile, _gasket, false);

		if(_pCamera->state != ISPC::Camera::CAM_ERROR)
		{
			sensor = _pCamera->getSensor();
		}

        // Apply additional parameters
		if(sensor)
		{
            if (IMG_SUCCESS != DGCam_ExtendedSetNbBuffers(sensor->getHandle(), _nBuffers + 1))
            {
                LOG_ERROR("failed to set the number of DG buffers\n");
                return EXIT_FAILURE;
            }

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
		_pCamera = new ISPC::Camera(0, ISPC::Sensor::GetSensorId(_sensor), _sensorMode, _sensorFlip, 0);
#else
		_pCamera = new ISPC::Camera(0, ISPC::Sensor::GetSensorId(_sensor), _sensorMode, _sensorFlip);
#endif
		if(_pCamera->state != ISPC::Camera::CAM_ERROR)
		{
			sensor = _pCamera->getSensor();
		}
	}

	if(!_pCamera || _pCamera->state == ISPC::Camera::CAM_ERROR)
    {
        LOG_ERROR("Failed to create camera correctly\n");
		unlock(&_cameraLock, __LINE__);
        delete _pCamera;
        _pCamera = NULL;
        return IMG_ERROR_FATAL;
    }

	// Get HW vwrsion
	pthread_mutex_lock(&_hwVerLock);
	_hwVersion = NL_getHWVersion(_pCamera);
	pthread_mutex_unlock(&_hwVerLock);

    // Store initial Gamma LUT
    _GMA_initial = _pCamera->getConnection()->sGammaLUT;

	// Register setup modules
	if(ISPC::CameraFactory::populateCameraFromHWVersion(*_pCamera, sensor) != IMG_SUCCESS)
	{
		delete _pCamera;
		_pCamera = NULL;
		LOG_ERROR("HW not found\n");
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}

	// Registering control modules
	if (NL_registerControlModules() != IMG_SUCCESS)
	{
		LOG_ERROR("registerControlModules() failed\n");
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}

	// Load default parameters to registered setup modules
	if(_pCamera->loadParameters(ISPC::ParameterList()) != IMG_SUCCESS)
	{
	   LOG_ERROR("loadParameters() failed\n");
	   unlock(&_cameraLock, __LINE__);
	   return IMG_ERROR_FATAL;
	}

	// Load default parameters to registered control modules
	if(_pCamera->loadControlParameters(ISPC::ParameterList()) != IMG_SUCCESS)
	{
		LOG_ERROR("loadControlParameters() failed\n");
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}

	// Set defaults for live feed
	ISPC::ModuleOUT *out = _pCamera->getModule<ISPC::ModuleOUT>();
	out->encoderType = YVU_420_PL12_8;
	out->displayType = PXL_NONE;
	out->dataExtractionType = PXL_NONE;
	out->hdrExtractionType = PXL_NONE;
	out->raw2DExtractionType = PXL_NONE;

	if (_pCamera->setupModules() != IMG_SUCCESS)
	{
	   LOG_ERROR("setupModules() failed\n");
	   unlock(&_cameraLock, __LINE__);
	   return IMG_ERROR_FATAL;
	}

	if (_pCamera->program() != IMG_SUCCESS)
	{
	   LOG_ERROR("program() failed\n");
	   unlock(&_cameraLock, __LINE__);
	   return IMG_ERROR_FATAL;
	}

	// + 1 extra buffer for global capturedFrame
	if(_pCamera->allocateBufferPool(_nBuffers + 1, _bufferIds) != IMG_SUCCESS)
	{
	   LOG_ERROR("allocateBufferPool() failed\n");
	   unlock(&_cameraLock, __LINE__);
	   return IMG_ERROR_FATAL;
	}

	if(_pCamera->startCapture() != IMG_SUCCESS)
	{
	   LOG_ERROR("failed to start the capture\n");
	   unlock(&_cameraLock, __LINE__);
	   return IMG_ERROR_FATAL;
	}

	for(int i = 0; i < _nBuffers - 1; i++)
	{
		if(_pCamera->enqueueShot() != IMG_SUCCESS)
		{
		   LOG_ERROR("failed to enque shot\n");
		   unlock(&_cameraLock, __LINE__);
		   return IMG_ERROR_FATAL;
		}
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::NL_registerControlModules()
{
	ISPC::ControlAE *ae = new ISPC::ControlAE();
	if (_pCamera->registerControlModule(ae) != IMG_SUCCESS)
	{
		LOG_ERROR("registerControlModule() AE failed\n");
		return IMG_ERROR_FATAL;
	}
	ae->enableControl(false);

	if (_hwVersion < HW_2_6)
	{
		ISPC::ControlAWB_PID *awb = new ISPC::ControlAWB_PID();
		if (_pCamera->registerControlModule(awb) != IMG_SUCCESS)
		{
			LOG_ERROR("registerControlModule() AWB_PID failed\n");
			return IMG_ERROR_FATAL;
		}
		awb->setCorrectionMode(ISPC::ControlAWB::WB_NONE);
		awb->enableControl(false);
	}
	else
	{
		ISPC::ControlAWB_Planckian *awb = new ISPC::ControlAWB_Planckian();
		if (_pCamera->registerControlModule(awb) != IMG_SUCCESS)
		{
			LOG_ERROR("registerControlModule() AWB_Planckian failed\n");
			return IMG_ERROR_FATAL;
		}
		awb->setCorrectionMode(ISPC::ControlAWB::WB_NONE);
		awb->enableControl(false);
	}

	ISPC::ControlAF *af = new ISPC::ControlAF();
	if (_pCamera->registerControlModule(af) != IMG_SUCCESS)
	{
		LOG_ERROR("registerControlModule() AF failed\n");
		return IMG_ERROR_FATAL;
	}
	af->enableControl(false);

	ISPC::ControlTNM *tnm = new ISPC::ControlTNM();
	if (_pCamera->registerControlModule(tnm) != IMG_SUCCESS)
	{
		LOG_ERROR("registerControlModule() TNM failed\n");
		return IMG_ERROR_FATAL;
	}
	tnm->enableControl(false);

	ISPC::ControlLBC *lbc = new ISPC::ControlLBC();
	if (_pCamera->registerControlModule(lbc) != IMG_SUCCESS)
	{
		LOG_ERROR("registerControlModule() LBC failed\n");
		return IMG_ERROR_FATAL;
	}
	lbc->enableControl(false);

	ISPC::ControlDNS *dns = new ISPC::ControlDNS();
	if (_pCamera->registerControlModule(dns) != IMG_SUCCESS)
	{
		LOG_ERROR("registerControlModule() DNS failed\n");
		return IMG_ERROR_FATAL;
	}
	dns->enableControl(false);

    //ISPC::ControlLSH *lsh = new ISPC::ControlLSH();
    VL_ControlLSH *lsh = new VL_ControlLSH();
    if (_pCamera->registerControlModule(lsh) != IMG_SUCCESS)
    {
        LOG_ERROR("registerControlModule() LSH failed\n");
        return IMG_ERROR_FATAL;
    }
    lsh->registerCtrlAWB(_pCamera->getControlModule<ISPC::ControlAWB_Planckian>());
    lsh->enableControl(false);

	return IMG_SUCCESS;
}

void CameraHandler::run()
{
#ifndef FELIX_FAKE
    time_t t;
    t = time(NULL);
    int nFrames = 0;

    ISPC::Shot frame;
#endif // FELIX_FAKE

    while (isRunning())
    {
        if (getCmdHState() == STOPPED && getImgHState() == STOPPED)
        {
            LOG_DEBUG("Stopping CameraHandler due to CommandHandler and ImageHandler has stopped\n");
            break;
        }
        if (isPaused())
        {
            if (getCamHState() != PAUSED)
            {
                setCamHState(PAUSED);
                LOG_DEBUG("CameraHandler paused\n");
            }

            pauseThread(5);

            continue;
        }
        else
        {
            if (getCamHState() != RUNNING)
            {
                setCamHState(RUNNING);
                LOG_DEBUG("CameraHandler running\n");
            }
        }

#ifndef FELIX_FAKE
        if(_PDPState)
        {
            lock(&_cameraLock, __LINE__);

            if(NL_TakeShot(frame) != IMG_SUCCESS)
            {
                unlock(&_cameraLock, __LINE__);
                LOG_ERROR("NL_TakeShot() failed\n");
                break;
            }

            _dpfStats = frame.metadata.defectiveStats;

            if(NL_ReleaseShot(frame) != IMG_SUCCESS)
            {
                unlock(&_cameraLock, __LINE__);
                LOG_ERROR("NL_ReleaseShot() failed\n");
                break;
            }

            unlock(&_cameraLock, __LINE__);

            _frameCount++;
            nFrames++;
            if(time(NULL) - t > 10)
            {
                _fps = (double)nFrames/(double)(time(NULL) - t);
                nFrames = 0;
                LOG_DEBUG("FPS - %f\n", _fps);
                t = time(NULL);
            }
        }
#endif // FELIX_FAKE

		pauseThread(5);
	}

	if(shutDown() != IMG_SUCCESS)
	{
		LOG_ERROR("shutDown() failed\n");
	}

	LOG_DEBUG("CameraHandler stopped\n");
}

HW_VERSION CameraHandler::NL_getHWVersion(ISPC::Camera *camera)
{
	if (!camera)
	{
		return _hwVersion;
	}

	IMG_UINT8 maj = camera->getConnection()->sHWInfo.rev_ui8Major;
	IMG_UINT8 min = camera->getConnection()->sHWInfo.rev_ui8Minor;

	if (maj == 2 && min < 4)
		return HW_2_X;
	else if (maj == 2 && min < 6)
		return HW_2_4;
	else if (maj == 2)
		return HW_2_6;
	else if (maj == 3)
		return HW_3_X;
	else
		return HW_UNKNOWN;
}

int CameraHandler::NL_StartCamera()
{
	if(!_pCamera)
	{
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	// + 1 extra buffer for global capturedFrame
	if(_pCamera->allocateBufferPool(_nBuffers + 1, _bufferIds) != IMG_SUCCESS)
	{
	   LOG_ERROR("allocateBufferPool() failed\n");
	   return IMG_ERROR_FATAL;
	}

	if(_pCamera->startCapture() != IMG_SUCCESS)
	{
	   LOG_ERROR("startCapture() failed\n");
	   return IMG_ERROR_FATAL;
	}

	for(int i = 0; i < _nBuffers - 1; i++)
	{
		if(_pCamera->enqueueShot() != IMG_SUCCESS)
		{
		   LOG_ERROR("enqueueShot() failed\n");
		   return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}

int CameraHandler::NL_StopCamera()
{
	if(!_pCamera)
	{
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	ISPC::Shot tmpFrame;

	for(int i = 0; i < _nBuffers - 1; i++)
	{
		if(_pCamera->acquireShot(tmpFrame) != IMG_SUCCESS)
		{
			LOG_ERROR("acquireShot() failed\n");
			return IMG_ERROR_FATAL;
		}

		if(_pCamera->releaseShot(tmpFrame) != IMG_SUCCESS)
		{
			LOG_ERROR("acquireShot() failed\n");
			return IMG_ERROR_FATAL;
		}
	}

	if(_pCamera->stopCapture())
	{
		LOG_ERROR("failed to stop capturing\n");
		return IMG_ERROR_FATAL;
	}

	LOG_DEBUG("BufferIds size %d\n", _bufferIds.size());
	std::list<IMG_UINT32>::iterator it;
    for(it = _bufferIds.begin(); it != _bufferIds.end(); it++)
    {
		LOG_DEBUG("Deregister buffer %d\n", *it);
		if(_pCamera->deregisterBuffer(*it) != IMG_SUCCESS)
		{
			LOG_ERROR("deregisterBuffer() failed on %d buffer\n", *it);
			return IMG_ERROR_FATAL;
		}
	}
    _bufferIds.clear();

	if(_pCamera->getPipeline()->deleteShots() != IMG_SUCCESS)
	{
		LOG_ERROR("deleteShots() failed\n");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int CameraHandler::NL_TakeShot(ISPC::Shot &frame)
{
	if(!_pCamera)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCamera->state != ISPC::Camera::CAM_CAPTURING)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Camera is not in state CAM_CAPTURING\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCamera->enqueueShot() != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
	   LOG_ERROR("enqueueShot() failed\n");
	   return IMG_ERROR_FATAL;
	}

	if(_pCamera->acquireShot(frame) != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("acquireShot() failed\n");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int CameraHandler::NL_ReleaseShot(ISPC::Shot &frame)
{
	if(!_pCamera)
	{
		unlock(&_cameraLock, __LINE__);
		LOG_ERROR("Camera not initialized\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCamera->releaseShot(frame) != IMG_SUCCESS)
	{
		unlock(&_cameraLock, __LINE__);
	   LOG_ERROR("releaseShot() failed\n");
	   return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

int CameraHandler::disableSensor()
{
	lock(&_cameraLock, __LINE__);

	if(!_pCamera)
	{
		LOG_ERROR("Camera not initialized\n");
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}

	_pCamera->getSensor()->disable();

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

int CameraHandler::restoreInitialGMA()
{
	lock(&_cameraLock, __LINE__);

	if(!_pCamera)
	{
		LOG_ERROR("Camera not initialized\n");
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}

	if(CI_DriverSetGammaLUT(_pCamera->getConnection(), &_GMA_initial) != IMG_SUCCESS)
	{
		LOG_ERROR("Restoring initial Gamma LUT failed\n");
		unlock(&_cameraLock, __LINE__);
		return IMG_ERROR_FATAL;
	}
	else
	{
		LOG_INFO("Initial Gamma LUT restored\n");
	}

	unlock(&_cameraLock, __LINE__);

	return IMG_SUCCESS;
}

void CameraHandler::lock(pthread_mutex_t *mutex, int line)
{
#ifdef LOG_LOCKING
	LOG_DEBUG("LOCKING... in line %d\n", line);
#endif
	pthread_mutex_lock(mutex);
#ifdef LOG_LOCKING
	LOG_DEBUG("LOCKED in line %d\n", line);
#endif
}

void CameraHandler::unlock(pthread_mutex_t *mutex, int line)
{
#ifdef LOG_LOCKING
	LOG_DEBUG("UNLOCKING... in line %d\n", line);
#endif
	pthread_mutex_unlock(mutex);
#ifdef LOG_LOCKING
	LOG_DEBUG("UNLOCKED in line %d\n", line);
#endif
}

/*
 * CommandHandler
 */

//
// Public Functions
//

CommandHandler::CommandHandler(
	char *appName,
	char *ip,
	int port,
	CameraHandler *pCameraHandler):
	_appName(appName),
	_ip(ip),
	_port(port),
	_pCameraHandler(pCameraHandler)
{
	_pCmdSocket = NULL;

	pthread_mutexattr_init(&_runLockAttr);
	pthread_mutexattr_settype(&_runLockAttr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&_runLock, &_runLockAttr);

	_run = false;
	_pause = false;
	_focusing = false;

    _currentLSHGridTileSize = 0;
    _currentLSHGridSize = 0;
}

CommandHandler::~CommandHandler()
{
	if(_pCmdSocket)
	{
		delete _pCmdSocket;
	}
}

int CommandHandler::connect()
{
	LOG_DEBUG("Connecting...\n");

	_pCmdSocket = ParamSocketClient::connect_to_server(_ip, _port, _appName);
	if(_pCmdSocket == NULL)
	{
		LOG_ERROR("Connection failed\n");
		return IMG_ERROR_FATAL;
	}

	setCmdHState(CONNECTED);
	LOG_DEBUG("Connection established\n");

	return IMG_SUCCESS;
}

void CommandHandler::start()
{
	pthread_mutex_lock(&_runLock);
	_run = true;
	pthread_mutex_unlock(&_runLock);
	pthread_create(&_thread, NULL, CommandHandler::staticEntryPoint, this);
}

bool CommandHandler::isRunning()
{
	bool isRunning;
	pthread_mutex_lock(&_runLock);
	isRunning = _run;
	pthread_mutex_unlock(&_runLock);
	return isRunning;
}

void CommandHandler::pause()
{
	if(getCmdHState() == RUNNING)
	{
		pthread_mutex_lock(&_runLock);
		_pause = true;
		pthread_mutex_unlock(&_runLock);
		while(getCmdHState() != PAUSED) {pauseThread(5);}
	}
}

bool CommandHandler::isPaused()
{
	bool isPaused;
	pthread_mutex_lock(&_runLock);
	isPaused = _pause;
	pthread_mutex_unlock(&_runLock);
	return isPaused;
}

void CommandHandler::resume()
{
	if(getCmdHState() == PAUSED)
	{
		pthread_mutex_lock(&_runLock);
		_pause = false;
		pthread_mutex_unlock(&_runLock);
		while(getCmdHState() != RUNNING) {pauseThread(5);}
	}
}

void CommandHandler::stop()
{
	pthread_mutex_lock(&_runLock);
	_run = false;
	pthread_mutex_unlock(&_runLock);
}

//
// Private Functions
//

void *CommandHandler::staticEntryPoint(void *c)
{
    ((CommandHandler *)c)->run();
    return NULL;
}

void CommandHandler::run()
{
	IMG_UINT32 cmd;
    size_t nRead;

	while(getCamHState() != RUNNING && isRunning()) {pauseThread(5);}
	setCmdHState(RUNNING);

	while(isRunning())
	{
		if(isPaused())
		{
			if(getCmdHState() != PAUSED)
			{
				setCmdHState(PAUSED);
				LOG_DEBUG("CommandHandler paused\n");
			}

			pauseThread(2);

			continue;
		}
		else
		{
			if(getCmdHState() != RUNNING)
			{
				setCmdHState(RUNNING);
				LOG_DEBUG("CommandHandler running\n");
			}
		}

		if(_pCmdSocket)
		{
			//LOG_DEBUG("Waiting for command...\n");
			setCmdHState(WAITING_FOR_CMD);
			if(_pCmdSocket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) == IMG_SUCCESS)
			{
				setCmdHState(RUNNING);
				switch(ntohl(cmd))
				{
				case GUICMD_QUIT:
					//LOG_DEBUG("CMD_QUIT\n");
					stop();
					break;
				case GUICMD_GET_HWINFO:
					//LOG_DEBUG("GUICMD_GET_HWINFO\n");
					if(HWINFO_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("HWINFO_send() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_SENSOR:
					//LOG_DEBUG("GUICMD_GET_SENSOR\n");
					if(SENSOR_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("SENSOR_send() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_PARAMETER_LIST:
					//LOG_DEBUG("GUICMD_SET_PARAMETER_LIST\n");
					if(receiveParameterList(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("receiveParameterList() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_AE:
					//LOG_DEBUG("GUICMD_GET_AE\n");
					if(AE_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("AE_send() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_AE_ENABLE:
					//LOG_DEBUG("GUICMD_SET_AE_ENABLE\n");
					if(AE_enable(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("AE_enable() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_AF:
					//LOG_DEBUG("GUICMD_GET_AF\n");
					if(AF_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("sendAF() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_AF_ENABLE:
					//LOG_DEBUG("GUICMD_SET_AF_ENABLE\n");
					if(AF_enable(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("AF_enable() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_AF_TRIG:
					//LOG_DEBUG("GUICMD_SET_AF_TRIG\n");
					if(AF_triggerSearch(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("AF_triggerSearch() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_AWB:
					//LOG_DEBUG("GUICMD_GET_AWB\n");
					if(AWB_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("sendAWB() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_AWB_MODE:
					//LOG_DEBUG("GUICMD_SET_AWB_MODE\n");
					if(AWB_changeMode(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("AWB_changeMode() failed\n");
						stop();
					}
					break;
				case GUICMD_ADD_LSH_GRID:
					//LOG_DEBUG("GUICMD_ADD_LSH_GRID\n");
					if(LSH_addGrid(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("LSH_addGrid() failed\n");
						stop();
					}
					break;
                case GUICMD_REMOVE_LSH_GRID:
                    //LOG_DEBUG("GUICMD_REMOVE_LSH_GRID\n");
                    if (LSH_removeGrid(*_pCmdSocket) != IMG_SUCCESS)
                    {
                        LOG_ERROR("LSH_removeGrid() failed\n");
                        stop();
                    }
                    break;
                case GUICMD_SET_LSH_GRID:
                    //LOG_DEBUG("GUICMD_SET_LSH_GRID\n");
                    if (LSH_applyGrid(*_pCmdSocket) != IMG_SUCCESS)
                    {
                        LOG_ERROR("LSH_applyGrid() failed\n");
                        stop();
                    }
                    break;
                case GUICMD_SET_LSH_ENABLE:
                    //LOG_DEBUG("GUICMD_SET_LSH_ENABLE\n");
                    if (LSH_enable(*_pCmdSocket) != IMG_SUCCESS)
                    {
                        LOG_ERROR("LSH_enable() failed\n");
                        stop();
                    }
                    break;
                case GUICMD_GET_LSH:
                    //LOG_DEBUG("GUICMD_GET_LSH\n");
                    if (LSH_send(*_pCmdSocket) != IMG_SUCCESS)
                    {
                        LOG_ERROR("LSH_send() failed\n");
                        stop();
                    }
                    break;
				case GUICMD_SET_DNS_ENABLE:
					//LOG_DEBUG("GUICMD_SET_DNS_ENABLE\n");
					if(DNS_enable(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("DNS_enable() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_DPF:
					//LOG_DEBUG("GUICMD_GET_DPF\n");
					if(DPF_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("DPF_send() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_DPF_MAP:
					//LOG_DEBUG("GUICMD_SET_DPF_MAP\n");
					if(DPF_map_apply(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("DPF_map_apply() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_TNM:
					//LOG_DEBUG("GUICMD_GET_TNM\n");
					if(TNM_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("TNM_send() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_TNM_ENABLE:
					//LOG_DEBUG("GUICMD_SET_TNM_ENABLE\n");
					if(TNM_enable(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("TNM_enable() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_ENS:
					//LOG_DEBUG("GUICMD_GET_ENS\n");
					if(ENS_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("ENS_enable() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_LBC:
					//LOG_DEBUG("GUICMD_GET_LBC\n");
					if(LBC_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("LBC_send() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_LBC_ENABLE:
					//LOG_DEBUG("GUICMD_SET_LBC_ENABLE\n");
					if(LBC_enable(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("LBC_enable() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_GMA:
					//LOG_DEBUG("GUICMD_GET_GMA\n");
					if(GMA_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("GMA_send() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_GMA:
					//LOG_DEBUG("GUICMD_SET_GMA\n");
					if(GMA_set(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("GMA_set() failed\n");
						stop();
					}
					break;
                case GUICMD_SET_PDP_ENABLE:
                    //LOG_DEBUG("GUICMD_SET_PDP_ENABLE\n");
                    if (PDP_enable(*_pCmdSocket) != IMG_SUCCESS)
                    {
                        LOG_ERROR("PDP_enable() failed\n");
                        stop();
                    }
                    break;
				case GUICMD_GET_FB:
					//LOG_DEBUG("GUICMD_GET_FB\n");
					if(FB_send(*_pCmdSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("FB_send() failed\n");
						stop();
					}
					break;
				default:
					LOG_ERROR("Unrecognized command received\n");
					stop();
					break;
				}
			}
			else
			{
				LOG_ERROR("Failed to read from socket\n");
				stop();
			}
		}
		else
		{
			LOG_ERROR("Socket error\n");
			stop();
		}

		pauseThread(5);
	}

	setCmdHState(STOPPED);
	LOG_DEBUG("CommandHandler stopped\n");
}

int CommandHandler::HWINFO_send(ParamSocket &socket)
{
	std::vector<std::string> HWINFO_params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	ISPC::Camera *pCamera = _pCameraHandler->takeCamera(__LINE__);

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.ui8GroupId); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.ui8AllocationId); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.ui16ConfigId); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.rev_ui8Designer); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.rev_ui8Major); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.rev_ui8Minor); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.rev_ui8Maint); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.rev_uid); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui8PixPerClock); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui8Parallelism); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui8BitDepth); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui8NContexts); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui8NImagers); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui8NIIFDataGenerator); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui16MemLatency); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.config_ui32DPFInternalSize); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.scaler_ui8EncHLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.scaler_ui8EncVLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.scaler_ui8EncVChromaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.scaler_ui8DispHLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.scaler_ui8DispVLumaTaps); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	for(int i = 0; i < CI_N_IMAGERS; i++)
	{
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.gasket_aImagerPortWidth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.gasket_aType[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.gasket_aMaxWidth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.gasket_aBitDepth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	for(int i = 0; i < CI_N_CONTEXT; i++)
	{
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.context_aMaxWidthMult[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.context_aMaxHeight[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.context_aMaxWidthSingle[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.context_aBitDepth[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
		sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.context_aPendingQueue[i]); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.ui32MaxLineStore); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.eFunctionalities); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.uiSizeOfPipeline); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.uiChangelist); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.uiTiledScheme); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

    sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.mmu_ui8Enabled); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.mmu_ui32PageSize); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.mmu_ui8RevMajor); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.mmu_ui8RevMinor); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.mmu_ui8RevMaint); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	sprintf(buff, "%d", pCamera->getConnection()->sHWInfo.ui32RefClockMhz); HWINFO_params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::SENSOR_send(ParamSocket &socket)
{
	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	ISPC::Sensor *sensor = _pCameraHandler->takeCamera(__LINE__)->getSensor();

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

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::receiveParameterList(ParamSocket &socket)
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
			printf("Waiting for IMGH to finish capture...\n");
			_pCameraHandler->updateCameraParameters(newParamsList);
			break;
		default:
			LOG_ERROR("Unrecognized command received!\n");
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}

int CommandHandler::AE_send(ParamSocket &socket)
{
	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	ISPC::ControlAE *ae = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlAE>();

	if(!ae)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	sprintf(buff, "MeasuredBrightness %f", ae->getCurrentBrightness()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DetectedFlickerFrequency %f", ae->getFlickerRejectionFrequency()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::AE_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	ISPC::ControlLBC *lbc = camera->getControlModule<ISPC::ControlLBC>();
	ISPC::ControlAE *ae = camera->getControlModule<ISPC::ControlAE>();
	ISPC::ControlTNM *tnm = camera->getControlModule<ISPC::ControlTNM>();

	if(!ae || !tnm || !lbc)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	ae->enableControl(state > 0);
	tnm->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());
	lbc->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::AF_send(ParamSocket &socket)
{
	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	ISPC::ControlAF *af = camera->getControlModule<ISPC::ControlAF>();

	if(!af)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	sprintf(buff, "FocusState %d", af->getState()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FocusScanState %d", af->getScanState()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	if(_focusing)
	{
        if (af->getState() == ISPC::ControlAF::AF_IDLE)
		{
			camera->getSensor()->setFocusDistance(af->getBestFocusDistance());
			_focusing = false;
		}
	}

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::AF_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::ControlAF *af = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlAF>();

	if(!af)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	if(!_focusing)
	{
		af->enableControl(state > 0);
	}

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::AF_triggerSearch(ParamSocket &socket)
{
	ISPC::ControlAF *af = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlAF>();

	if(!af)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	if(!_focusing)
	{
        af->setCommand(ISPC::ControlAF::AF_TRIGGER);
		_focusing = true;
	}

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::AWB_send(ParamSocket &socket)
{
	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	ISPC::ControlAWB *awb;
	if (_pCameraHandler->getHWVersion() < HW_2_6)
	{
		awb = camera->getControlModule<ISPC::ControlAWB_PID>();
	}
	else
	{
		awb = camera->getControlModule<ISPC::ControlAWB_Planckian>();
	}
	ISPC::ModuleWBC *wbc = camera->getModule<ISPC::ModuleWBC>();
	ISPC::ModuleCCM *ccm = camera->getModule<ISPC::ModuleCCM>();

	if(!awb || !wbc || !ccm)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	sprintf(buff, "MeasuredTemperature %f", awb->getMeasuredTemperature()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Gains %f %f %f %f", wbc->aWBGain[0], wbc->aWBGain[1], wbc->aWBGain[2], wbc->aWBGain[3]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Clips %f %f %f %f", wbc->aWBClip[0], wbc->aWBClip[1], wbc->aWBClip[2], wbc->aWBClip[3]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Matrix %f %f %f %f %f %f %f %f %f",
		ccm->aMatrix[0], ccm->aMatrix[1], ccm->aMatrix[2], ccm->aMatrix[3], ccm->aMatrix[4], ccm->aMatrix[5], ccm->aMatrix[6], ccm->aMatrix[7], ccm->aMatrix[8]);
	params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Offsets %f %f %f", ccm->aOffset[0], ccm->aOffset[1], ccm->aOffset[2]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::AWB_changeMode(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}

    ISPC::ControlAWB::Correction_Types mode = (ISPC::ControlAWB::Correction_Types)ntohl(cmd[0]);

	ISPC::ControlAWB *awb;
	if (_pCameraHandler->getHWVersion() < HW_2_6)
	{
		awb = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlAWB_PID>();
	}
	else
	{
		awb = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlAWB_Planckian>();
	}

	if(!awb)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

    awb->enableControl(mode != ISPC::ControlAWB::WB_NONE);
	awb->setCorrectionMode(mode);

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::LSH_addGrid(ParamSocket &socket)
{
    _pCameraHandler->pauseImgH();
    _pCameraHandler->pause();

    IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;
    size_t nWritten;
    LSH_GRID sGrid = LSH_GRID();
    IMG_RESULT ret;

    IMG_UINT16 ui16TileSize = 0;
    IMG_UINT16 ui16Width = 0;
    IMG_UINT16 ui16Height = 0;
    IMG_UINT32 ui32ChannelSize = 0;
    IMG_UINT32 matrixId = 0;
    IMG_UINT32 prevMatrixId = 0;
    IMG_UINT8 bitsPerDiff = 0;
    IMG_UINT16 temp = 0;
    double wbScale = 0.0f;

    ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);
    ISPC::ModuleLSH *lsh = camera->getModule<ISPC::ModuleLSH>();
    ISPC::ControlLSH *clsh = camera->getControlModule<ISPC::ControlLSH>();

    if(!lsh || !clsh)
    {
        LOG_ERROR("no LSH module found!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    ret = socket.read(&cmd, 6 * sizeof(IMG_UINT32), nRead, N_TRIES);
    if(IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to read from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    ui16TileSize = (IMG_UINT16)ntohl(cmd[0]);
    ui16Width = (IMG_UINT16)ntohl(cmd[1]);
    ui16Height = (IMG_UINT16)ntohl(cmd[2]);
    ui32ChannelSize = ui16Width*ui16Height*sizeof(LSH_FLOAT);
    temp = (IMG_UINT16)ntohl(cmd[3]);
    bitsPerDiff = (IMG_UINT8)ntohl(cmd[4]);
    wbScale = ((double)ntohl(cmd[5]))/1000.0f;
    // because GUI sends floats
    IMG_ASSERT(sizeof(LSH_FLOAT) == sizeof(float));

    // Rceive filename
    char buff[PARAMSOCK_MAX];
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    if (socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to read from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    int filenameLength = ntohl(cmd[0]);
    if (socket.read(buff, filenameLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to read from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    buff[nRead] = '\0';
    std::string filename = std::string(buff);
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    LOG_INFO("LSH received - %s\n", filename.c_str());

    /* asks for size in pixel covers not in tiles while stored in tiles
     * should override sGrid with new size and tile size but should be back
     * to the same values */
    ret = LSH_AllocateMatrix(&sGrid, ui16Width,
        ui16Height, ui16TileSize);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to allocate matrix\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    if (sGrid.ui16Width != ui16Width
        || sGrid.ui16Height != ui16Height
        || sGrid.ui16TileSize != sGrid.ui16TileSize)
    {
        LOG_ERROR("failed to setup matrix\n");
        _pCameraHandler->releaseCamera(__LINE__);
        LSH_Free(&sGrid);
        return IMG_ERROR_FATAL;
    }

    for(int channel = 0; channel < 4; channel++)
    {
        ret = socket.read(sGrid.apMatrix[channel], ui32ChannelSize, nRead, -1);
        if (ret)
        {
            LOG_ERROR("failed to write to socket!\n");
            _pCameraHandler->releaseCamera(__LINE__);
            LSH_Free(&sGrid);
            return IMG_ERROR_FATAL;
        }
    }

    IMG_UINT32 currentGridId = clsh->getChosenMatrixId();
    if (_LSH_Grids.empty())
    {
        _currentLSHGridTileSize = ui16TileSize;
        _currentLSHGridSize = ui32ChannelSize;
    }
    else
    {
        // Check if matrix size and tileSize are the same with first in the map
        if (_currentLSHGridTileSize != ui16TileSize || _currentLSHGridSize != ui32ChannelSize)
        {
            LOG_ERROR("trying to add matrix of different size or tile size then those already applyed\n");
            ret = IMG_ERROR_FATAL;
        }
    }

    if (IMG_SUCCESS == ret)
    {
        if (_LSH_Grids.find(filename) != _LSH_Grids.end())
        {
            LOG_INFO("matrix already exists, replacing...\n");
            IMG_UINT32 id = _LSH_Grids.find(filename)->second;
            if (currentGridId == id)
            {
                _pCameraHandler->releaseCamera(__LINE__);

                ret = _pCameraHandler->stopCamera();
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to stop the camera\n");
                    _pCameraHandler->releaseCamera(__LINE__);
                    return IMG_ERROR_FATAL;
                }

                _pCameraHandler->takeCamera(__LINE__);

                ret = lsh->configureMatrix(0);

                ret = clsh->removeMatrix(_LSH_Grids.find(filename)->second);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to remove a matrix!\n");
                    _pCameraHandler->releaseCamera(__LINE__);
                    return IMG_ERROR_FATAL;
                }

                _LSH_Grids.erase(_LSH_Grids.find(filename));

                ret = clsh->addMatrix(temp, sGrid, matrixId, wbScale, bitsPerDiff);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to add a matrix!\n");
                    //_pCameraHandler->releaseCamera(__LINE__);
                    LSH_Free(&sGrid);
                    //return IMG_ERROR_FATAL;
                }

                // If this matrix was applyed then apply this one also
                ret = lsh->configureMatrix(matrixId);

                _pCameraHandler->releaseCamera(__LINE__);

                ret = _pCameraHandler->startCamera();
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to restart the camera!\n");
                    _pCameraHandler->releaseCamera(__LINE__);
                    return IMG_ERROR_FATAL;
                }

                _pCameraHandler->takeCamera(__LINE__);
            }
            else
            {
                ret = clsh->removeMatrix(id);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to remove a matrix!\n");
                    _pCameraHandler->releaseCamera(__LINE__);
                    LSH_Free(&sGrid);
                    return IMG_ERROR_FATAL;
                }
                _LSH_Grids.erase(_LSH_Grids.find(filename));

                ret = clsh->addMatrix(temp, sGrid, matrixId, wbScale, bitsPerDiff);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to add a matrix!\n");
                    //_pCameraHandler->releaseCamera(__LINE__);
                    LSH_Free(&sGrid);
                    //return IMG_ERROR_FATAL;
                }
            }
        }
        else
        {
            ret = clsh->addMatrix(temp, sGrid, matrixId, wbScale, bitsPerDiff);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to add a matrix!\n");
                //_pCameraHandler->releaseCamera(__LINE__);
                LSH_Free(&sGrid);
                //return IMG_ERROR_FATAL;
            }
        }
    }

    _pCameraHandler->releaseCamera(__LINE__);

    // Send acknowledgement
    cmd[0] = ret;
    if (socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to write from socket!\n");
        return IMG_ERROR_FATAL;
    }

    if (ret == IMG_SUCCESS)
        _LSH_Grids.insert(std::make_pair(filename, matrixId));

    // Print current state of matrixes map
    LOG_INFO("\ncurrent LSH state:\n");
    std::map<std::string, int>::const_iterator it;
    for (it = _LSH_Grids.begin(); it != _LSH_Grids.end(); it++)
        printf("%s %d\n", it->first.c_str(), it->second);
    printf("\n");

    _pCameraHandler->resume();
    _pCameraHandler->resumeImgH();

    return IMG_SUCCESS;
}

int CommandHandler::LSH_removeGrid(ParamSocket &socket)
{
    _pCameraHandler->pauseImgH();
    _pCameraHandler->pause();

    IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;
    size_t nWritten;
    IMG_RESULT ret;

    ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);
    ISPC::ModuleLSH *lsh = camera->getModule<ISPC::ModuleLSH>();
    ISPC::ControlLSH *clsh = camera->getControlModule<ISPC::ControlLSH>();
    if (!lsh || !clsh)
    {
        LOG_ERROR("no LSH module found!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    // Rceive filename
    char buff[PARAMSOCK_MAX];
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    if (socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to read from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    int filenameLength = ntohl(cmd[0]);
    if (socket.read(buff, filenameLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to read from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    buff[nRead] = '\0';
    std::string filename = std::string(buff);
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

    if (_LSH_Grids.find(filename) != _LSH_Grids.end())
    {
        // Check if applyed if not remove
        IMG_UINT32 currentMatrixID = clsh->getChosenMatrixId();
        if (currentMatrixID != _LSH_Grids.find(filename)->second)
        {
            ret = clsh->removeMatrix(_LSH_Grids.find(filename)->second);
            if (IMG_SUCCESS != ret)
            {
                LOG_WARNING("failed to remove matrix %d\n", _LSH_Grids.find(filename)->second);
            }
            else
            {
                _LSH_Grids.erase(_LSH_Grids.find(filename));
            }
        }
        else
        {
            _pCameraHandler->releaseCamera(__LINE__);

            ret = _pCameraHandler->stopCamera();
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to stop the camera\n");
                return IMG_ERROR_FATAL;
            }

            _pCameraHandler->takeCamera(__LINE__);

            ret = lsh->configureMatrix(0);

            ret = clsh->removeMatrix(_LSH_Grids.find(filename)->second);
            if (IMG_SUCCESS != ret)
            {
                LOG_WARNING("failed to remove matrix %d\n", _LSH_Grids.find(filename)->second);
            }
            else
            {
                _LSH_Grids.erase(_LSH_Grids.find(filename));
            }

            _pCameraHandler->releaseCamera(__LINE__);

            ret = _pCameraHandler->startCamera();
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to restart the camera!\n");
                return IMG_ERROR_FATAL;
            }

            _pCameraHandler->takeCamera(__LINE__);
        }
    }
    else
    {
        LOG_WARNING("matrix %s not found\n", filename.c_str());
        ret = IMG_ERROR_FATAL;
    }

    // Send acknowledgement
    cmd[0] = ret;
    if (socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to write from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    _pCameraHandler->releaseCamera(__LINE__);

    // Print current state of matrixes map
    LOG_INFO("\ncurrent LSH state:\n");
    std::map<std::string, int>::const_iterator it;
    for (it = _LSH_Grids.begin(); it != _LSH_Grids.end(); it++)
        printf("%s %d\n", it->first.c_str(), it->second);
    printf("\n");

    _pCameraHandler->resume();
    _pCameraHandler->resumeImgH();

    return IMG_SUCCESS;
}

int CommandHandler::LSH_applyGrid(ParamSocket &socket)
{
    _pCameraHandler->pauseImgH();
    _pCameraHandler->pause();

    IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;
    size_t nWritten;
    IMG_RESULT ret;

    ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);
    ISPC::ModuleLSH *lsh = camera->getModule<ISPC::ModuleLSH>();
    ISPC::ControlLSH *clsh = camera->getControlModule<ISPC::ControlLSH>();
    if (!lsh || !clsh)
    {
        LOG_ERROR("no LSH module found!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    // Rceive filename
    char buff[PARAMSOCK_MAX];
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    if (socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to read from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    int filenameLength = ntohl(cmd[0]);
    if (socket.read(buff, filenameLength*sizeof(char), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to read from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    buff[nRead] = '\0';
    std::string filename = std::string(buff);
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

    if (_LSH_Grids.find(filename) != _LSH_Grids.end())
    {
        IMG_UINT32 currentMatrixID = clsh->getChosenMatrixId();
        // This option should be valid if we check if Sizes and TileSizes of
        /*if (currentMatrixID == 0)
        {
            LOG_INFO("configuring matrix id %d\n", _LSH_Grids.find(filename)->second);
            ret = lsh->configureMatrix(_LSH_Grids.find(filename)->second);
            if (IMG_SUCCESS != ret)
                LOG_WARNING("failed to configure matrix %d\n", _LSH_Grids.find(filename)->second);
        }
        else if (currentMatrixID == _LSH_Grids.find(filename)->second)*/
        if (currentMatrixID == _LSH_Grids.find(filename)->second)
        {
            LOG_INFO("requested matrix is already applyed\n");
        }
        else
        {
            _pCameraHandler->releaseCamera(__LINE__);

            ret = _pCameraHandler->stopCamera();
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to stop the camera\n");
                _pCameraHandler->releaseCamera(__LINE__);
                return IMG_ERROR_FATAL;
            }

            _pCameraHandler->takeCamera(__LINE__);

            ret = lsh->configureMatrix(0);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to configure the matrix even with camera stopped\n");
                _pCameraHandler->releaseCamera(__LINE__);
                return IMG_ERROR_FATAL;
            }

            ret = lsh->configureMatrix(_LSH_Grids.find(filename)->second);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to configure the matrix even with camera stopped\n");
                _pCameraHandler->releaseCamera(__LINE__);
                return IMG_ERROR_FATAL;
            }

            _pCameraHandler->releaseCamera(__LINE__);

            ret = _pCameraHandler->startCamera();
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to restart the camera!\n");
                _pCameraHandler->releaseCamera(__LINE__);
                return IMG_ERROR_FATAL;
            }

            _pCameraHandler->takeCamera(__LINE__);
        }
    }
    else
    {
        LOG_WARNING("matrix %s not found\n", filename.c_str());
        ret = IMG_ERROR_FATAL;
    }

    // Send acknowledgement
    cmd[0] = ret;
    if (socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to write from socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    _pCameraHandler->releaseCamera(__LINE__);

    // Print current state of matrixes map
    LOG_INFO("\ncurrent LSH state:\n");
    std::map<std::string, int>::const_iterator it;
    for (it = _LSH_Grids.begin(); it != _LSH_Grids.end(); it++)
        printf("%s %d\n", it->first.c_str(), it->second);
    printf("\n");

    _pCameraHandler->resume();
    _pCameraHandler->resumeImgH();

    return IMG_SUCCESS;
}

int CommandHandler::LSH_enable(ParamSocket &socket)
{
    IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

    if (socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Failed to read the socket!\n");
        return IMG_ERROR_FATAL;
    }
    int state = ntohl(cmd[0]);

    ISPC::ControlLSH *lsh = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlLSH>();

    if (!lsh)
    {
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    lsh->enableControl(state > 0);

    _pCameraHandler->releaseCamera(__LINE__);

    return IMG_SUCCESS;
}

int CommandHandler::LSH_send(ParamSocket &socket)
{
    IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

    ISPC::ControlLSH *clsh = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlLSH>();

    if (!clsh)
    {
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }

    IMG_UINT32 currId = clsh->getChosenMatrixId();

    std::string filename;
    std::map<std::string, int>::const_iterator it;
    for (it = _LSH_Grids.begin(); it != _LSH_Grids.end(); it++)
    {
        if (it->second == currId)
        {
            filename = it->first;
            break;
        }
    }

    // Send filename
    char buff[PARAMSOCK_MAX];
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    int filenameLength = filename.length();
    cmd[0] = htonl(filenameLength);
    if (socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Failed to write to the socket!\n");
        _pCameraHandler->releaseCamera(__LINE__);
        return IMG_ERROR_FATAL;
    }
    if (filename.size() > 0)
    {
        strcpy(buff, filename.c_str());
        if (socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
        {
            LOG_ERROR("Failed to write to the socket!\n");
            _pCameraHandler->releaseCamera(__LINE__);
            return IMG_ERROR_FATAL;
        }
        memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    }

    _pCameraHandler->releaseCamera(__LINE__);

    return IMG_SUCCESS;
}

int CommandHandler::DNS_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::ControlDNS *dns = _pCameraHandler->takeCamera(__LINE__)->getControlModule<ISPC::ControlDNS>();

	if(!dns)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	dns->enableControl(state > 0);

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::DPF_send(ParamSocket &socket)
{
	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	MC_STATS_DPF &dpfStats = _pCameraHandler->takeDPFStats(__LINE__);

	sprintf(buff, "DroppedMapModifications %u", dpfStats.ui32DroppedMapModifications); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FixedPixels %u", dpfStats.ui32FixedPixels); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MapModifications %u", dpfStats.ui32MapModifications); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "NOutCorrection %u", dpfStats.ui32NOutCorrection); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	_pCameraHandler->releaseDPFStats(__LINE__);

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	sprintf(buff, "Parallelism %d", camera->getConnection()->sHWInfo.config_ui8Parallelism); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "DPFInternalSize %u", camera->getConnection()->sHWInfo.config_ui32DPFInternalSize); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::DPF_map_apply(ParamSocket &socket)
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

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	ISPC::ModuleDPF *dpf = camera->getPipeline()->getModule<ISPC::ModuleDPF>();
	bool previousEnableReadMap = dpf->bRead;

	if(!dpf)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

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

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::TNM_send(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(ISPC::ModuleTNM::TNM_CURVE.n);
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	ISPC::ModuleTNM *tnm = _pCameraHandler->takeCamera(__LINE__)->getPipeline()->getModule<ISPC::ModuleTNM>();

	if(!tnm)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	for(unsigned int i = 0; i < ISPC::ModuleTNM::TNM_CURVE.n; i++)
	{
		cmd[0] = htonl(static_cast<u_long>(tnm->aCurve[i]*1000));
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			_pCameraHandler->releaseCamera(__LINE__);
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
		//printf("%f  ", tnm->aCurve[i]);
	}

	//ISPC::ControlTNM *ctnm = _camera->getControlModule<ISPC::ControlTNM>();
	//ISPC::ControlAE *cae = _camera->getControlModule<ISPC::ControlAE>();
	//printf("\n ------------------------- AE CONFIGURED %s  TNM CONFIGURED %s ---------------------\n",
	//	(cae->getConfigured())? "YES" : "NO", (ctnm->getAllowHISConfig())? "YES" : "NO");

	sprintf(buff, "LumaInMin %f", tnm->aInY[0]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "LumaInMax %f", tnm->aInY[1]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    sprintf(buff, "LumaOutMin %f", tnm->aOutY[0]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    sprintf(buff, "LumaOutMax %f", tnm->aOutY[1]); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Bypass %d", (int)tnm->bBypass); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "ColourConfidance %f", tnm->fColourConfidence); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "ColourSaturation %f", tnm->fColourSaturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FlatFactor %f", tnm->fFlatFactor); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "FlatMin %f", tnm->fFlatMin); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WeightLine %f", tnm->fWeightLine); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "WeightLocal %f", tnm->fWeightLocal); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::TNM_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	ISPC::ControlLBC *lbc = camera->getControlModule<ISPC::ControlLBC>();
	ISPC::ControlTNM *tnm = camera->getControlModule<ISPC::ControlTNM>();
	ISPC::ControlAE *ae = camera->getControlModule<ISPC::ControlAE>();

	if(!tnm || !ae || !lbc)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	tnm->enableControl(state > 0);
	tnm->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());
	lbc->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::ENS_send(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	cmd[0] = htonl(_pCameraHandler->takeENSStats(__LINE__).size); _pCameraHandler->releaseENSStats(__LINE__);
	if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
		return IMG_ERROR_FATAL;
    }

	return IMG_SUCCESS;
}

int CommandHandler::LBC_send(ParamSocket &socket)
{
	std::vector<std::string> params;
	char buff[PARAMSOCK_MAX];
	memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	ISPC::ControlLBC *lbc = camera->getControlModule<ISPC::ControlLBC>();

	if(!lbc)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	sprintf(buff, "LightLevel %f", lbc->getCurrentCorrection().lightLevel); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Brightness %f", lbc->getCurrentCorrection().brightness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Contrast %f", lbc->getCurrentCorrection().contrast); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Saturation %f", lbc->getCurrentCorrection().saturation); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "Sharpness %f", lbc->getCurrentCorrection().sharpness); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	sprintf(buff, "MeasuredLightLevel %f", lbc->getMeteredLightLevel()); params.push_back(std::string(buff)); memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

	_pCameraHandler->releaseCamera(__LINE__);

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

int CommandHandler::LBC_enable(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket!\n");
		return IMG_ERROR_FATAL;
	}
	int state = ntohl(cmd[0]);

	ISPC::Camera *camera = _pCameraHandler->takeCamera(__LINE__);

	ISPC::ControlLBC *lbc = camera->getControlModule<ISPC::ControlLBC>();
	ISPC::ControlTNM *tnm = camera->getControlModule<ISPC::ControlTNM>();
	ISPC::ControlAE *ae = camera->getControlModule<ISPC::ControlAE>();

	if(!tnm || !ae || !lbc)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	lbc->enableControl(state > 0);
	lbc->setAllowHISConfig(!ae->isEnabled() && !tnm->isEnabled() && lbc->isEnabled());
	tnm->setAllowHISConfig(!lbc->isEnabled() && !ae->isEnabled() && tnm->isEnabled());

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::GMA_send(ParamSocket &socket)
{
	ISPC::Camera* camera = _pCameraHandler->takeCamera(__LINE__);

	CI_CONNECTION *conn  = camera->getConnection();
	if(CI_DriverGetGammaLUT(conn) != IMG_SUCCESS)
	{
		_pCameraHandler->releaseCamera(__LINE__);
		return IMG_ERROR_FATAL;
	}

	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(conn->sGammaLUT.aRedPoints[i]);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			_pCameraHandler->releaseCamera(__LINE__);
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(conn->sGammaLUT.aGreenPoints[i]);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			_pCameraHandler->releaseCamera(__LINE__);
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		cmd[0] = htonl(conn->sGammaLUT.aBluePoints[i]);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			_pCameraHandler->releaseCamera(__LINE__);
			LOG_ERROR("Error: failed to write to socket!\n");
			return IMG_ERROR_FATAL;
		}
	}

	_pCameraHandler->releaseCamera(__LINE__);

	return IMG_SUCCESS;
}

int CommandHandler::GMA_set(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

	CI_MODULE_GMA_LUT glut;

	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to read to socket!\n");
			return IMG_ERROR_FATAL;
		}
		glut.aRedPoints[i] = static_cast<IMG_UINT16>(ntohl(cmd[0]));
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to read to socket!\n");
			return IMG_ERROR_FATAL;
		}
		glut.aGreenPoints[i] = static_cast<IMG_UINT16>(ntohl(cmd[0]));
	}
	for(int i = 0; i < GMA_N_POINTS; i++)
	{
		if(socket.read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: failed to read to socket!\n");
			return IMG_ERROR_FATAL;
		}
		glut.aBluePoints[i] = static_cast<IMG_UINT16>(ntohl(cmd[0]));
	}

	_pCameraHandler->setGMA(glut);

	return IMG_SUCCESS;
}

int CommandHandler::PDP_enable(ParamSocket &socket)
{
    IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead;

    if (socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Failed to read the socket!\n");
        return IMG_ERROR_FATAL;
    }
    int state = ntohl(cmd[0]);

    _pCameraHandler->setPDP(state?true:false);

    return IMG_SUCCESS;
}

int CommandHandler::FB_send(ParamSocket &socket)
{
    ISPC::ParameterList listToSend;
    _pCameraHandler->takeCamera(__LINE__)->saveParameters(listToSend, ISPC::ModuleBase::SAVE_VAL); _pCameraHandler->releaseCamera(__LINE__);
    //_paramsList += listToSend;

    // Add Sensog params to parameter list
    char intBuff[10];
    const ISPC::Sensor *sensor = _pCameraHandler->takeCamera(__LINE__)->getSensor();
    sprintf(intBuff, "%d", (int)(sensor->getExposure()));
    ISPC::Parameter exposure("SENSOR_CURRENT_EXPOSURE", std::string(intBuff));
    listToSend.addParameter(exposure);
    sprintf(intBuff, "%f", (sensor->getGain()));
    ISPC::Parameter gain("SENSOR_CURRENT_GAIN", std::string(intBuff));
    listToSend.addParameter(gain);
    sprintf(intBuff, "%d", (int)(sensor->getFocusDistance()));
    ISPC::Parameter focus("SENSOR_CURRENT_FOCUS", std::string(intBuff));
    listToSend.addParameter(focus);
    _pCameraHandler->releaseCamera(__LINE__);

    IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;

    char buff[PARAMSOCK_MAX];
    memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
    std::string param;

    cmd[0] = htonl(GUICMD_SET_PARAMETER_LIST);
    if (socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
        return IMG_ERROR_FATAL;
    }

    std::map<std::string, ISPC::Parameter>::const_iterator it;
    for (it = listToSend.begin(); it != listToSend.end(); it++)
    {
        param = it->first;

        // Send command informing of upcomming parameter name
        cmd[0] = htonl(GUICMD_SET_PARAMETER_NAME);
        cmd[1] = htonl(param.length());
        if (socket.write(cmd, 2 * sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
        {
            LOG_ERROR("Error: failed to write to socket!\n");
            return IMG_ERROR_FATAL;
        }

        // Send parameter name
        strcpy(buff, param.c_str());
        if (socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
        {
            LOG_ERROR("Error: failed to write to socket!\n");
            return IMG_ERROR_FATAL;
        }
        memset(buff, 0, PARAMSOCK_MAX * sizeof(char));

        for (unsigned int i = 0; i < it->second.size(); i++)
        {
            param = it->second.getString(i);

            // Send command informing of upcomming parameter value
            cmd[0] = htonl(GUICMD_SET_PARAMETER_VALUE);
            cmd[1] = htonl(param.length());
            if (socket.write(cmd, 2 * sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
            {
                LOG_ERROR("Error: failed to write to socket!\n");
                return IMG_ERROR_FATAL;
            }

            // Send parameter value
            strcpy(buff, param.c_str());
            if (socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
            {
                LOG_ERROR("Error: failed to write to socket!\n");
                return IMG_ERROR_FATAL;
            }
            memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
        }

        // Send command informing of upcomming parameter value
        cmd[0] = htonl(GUICMD_SET_PARAMETER_END);
        cmd[1] = htonl(0);
        if (socket.write(cmd, 2 * sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
        {
            LOG_ERROR("Error: failed to write to socket!\n");
            return IMG_ERROR_FATAL;
        }
    }

    // Send ending commend
    cmd[0] = htonl(GUICMD_SET_PARAMETER_LIST_END);
    cmd[1] = htonl(0);
    if (socket.write(cmd, 2 * sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
    {
        LOG_ERROR("Error: failed to write to socket!\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

/*
 * ImageHandler
 */

//
// Public Functions
//

ImageHandler::ImageHandler(
	char *appName,
	char *ip,
	int port,
	CameraHandler *pCameraHandler):
	_appName(appName),
	_ip(ip),
	_port(port),
	_pCameraHandler(pCameraHandler)
{
	_pImgSocket = NULL;

	pthread_mutexattr_init(&_runLockAttr);
	pthread_mutexattr_settype(&_runLockAttr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&_runLock, &_runLockAttr);

	_run = false;
	_pause = false;

	pthread_mutexattr_init(&_awbDebugModeLockAttr);
	pthread_mutexattr_settype(&_awbDebugModeLockAttr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&_runLock, &_awbDebugModeLockAttr);

	_awbDebugMode = false;

	dpfWriteMap = 0;
	dpfWriteMap_nDefects = 0;
}

ImageHandler::~ImageHandler()
{
	if(_pImgSocket)
	{
		delete _pImgSocket;
	}

	if(dpfWriteMap)
	{
		free(dpfWriteMap);
	}
}

int ImageHandler::connect()
{
	LOG_DEBUG("Connecting...\n");

	_pImgSocket = ParamSocketClient::connect_to_server(_ip, _port, _appName);
	if(_pImgSocket == NULL)
	{
		LOG_ERROR("Connection failed\n");
		return IMG_ERROR_FATAL;
	}

	setImgHState(CONNECTED);
	LOG_DEBUG("Connection established\n");

	return IMG_SUCCESS;
}

void ImageHandler::start()
{
	pthread_mutex_lock(&_runLock);
	_run = true;
	pthread_mutex_unlock(&_runLock);
	pthread_create(&_thread, NULL, ImageHandler::staticEntryPoint, this);
}

bool ImageHandler::isRunning()
{
	bool isRunning;
	pthread_mutex_lock(&_runLock);
	isRunning = _run;
	pthread_mutex_unlock(&_runLock);
	return isRunning;
}

void ImageHandler::pause()
{
	if(getImgHState() == RUNNING)
	{
		pthread_mutex_lock(&_runLock);
		_pause = true;
		pthread_mutex_unlock(&_runLock);
		while(getImgHState() != PAUSED) {pauseThread(5);}
	}
}

bool ImageHandler::isPaused()
{
	bool isPaused;
	pthread_mutex_lock(&_runLock);
	isPaused = _pause;
	pthread_mutex_unlock(&_runLock);
	return isPaused;
}

void ImageHandler::resume()
{
	if(getImgHState() == PAUSED)
	{
		pthread_mutex_lock(&_runLock);
		_pause = false;
		pthread_mutex_unlock(&_runLock);
		while(getImgHState() != RUNNING) {pauseThread(5);}
	}
}

void ImageHandler::stop()
{
	pthread_mutex_lock(&_runLock);
	_run = false;
	pthread_mutex_unlock(&_runLock);
}

void ImageHandler::setAWBDebugMode(bool option)
{
	pthread_mutex_lock(&_awbDebugModeLock);
	_awbDebugMode = option;
	pthread_mutex_unlock(&_awbDebugModeLock);
}

bool ImageHandler::getAWBDebugMode()
{
	bool ret;

	pthread_mutex_lock(&_awbDebugModeLock);
	ret = _awbDebugMode;
	pthread_mutex_unlock(&_awbDebugModeLock);

	return ret;
}

//
// Private Functions
//

void *ImageHandler::staticEntryPoint(void *c)
{
    ((ImageHandler *)c)->run();
    return NULL;
}

void ImageHandler::run()
{
	IMG_UINT32 cmd;
    size_t nRead;

	while(getCamHState() != RUNNING && isRunning()) {pauseThread(5);}
	setImgHState(RUNNING);

	while(isRunning())
	{
		if(isPaused())
		{
			if(getImgHState() != PAUSED)
			{
				setImgHState(PAUSED);
				LOG_DEBUG("ImageHandler paused\n");
			}

			pauseThread(2);

			continue;
		}
		else
		{
			if(getImgHState() != RUNNING)
			{
				setImgHState(RUNNING);
				LOG_DEBUG("ImageHandler running\n");
			}
		}

		if(_pImgSocket)
		{
			LOG_DEBUG("Waiting for command...\n");
			setImgHState(WAITING_FOR_CMD);
			if(_pImgSocket->read(&cmd, sizeof(IMG_UINT32), nRead, N_TRIES) == IMG_SUCCESS)
			{
				setImgHState(RUNNING);
				switch(ntohl(cmd))
				{
				case GUICMD_QUIT:
					LOG_DEBUG("CMD_QUIT\n");
					stop();
					break;
				case GUICMD_GET_IMAGE:
					LOG_DEBUG("GUICMD_GET_IMAGE\n");
					if(capture(*_pImgSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("sendImage() failed\n");
						stop();
					}
					break;
				case GUICMD_SET_IMAGE_RECORD:
					LOG_DEBUG("GUICMD_SET_IMAGE_RECORD\n");
					_pCameraHandler->pause();
					_pCameraHandler->pauseCmdH();
					if(record(*_pImgSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("record() failed\n");
						_pCameraHandler->resume();
						_pCameraHandler->resumeCmdH();
						stop();
					}
					_pCameraHandler->resume();
					_pCameraHandler->resumeCmdH();
					break;
				case GUICMD_GET_PARAMETER_LIST:
					LOG_DEBUG("GUICMD_GET_PARAMETER_LIST\n");
					if(sendParameterList(*_pImgSocket) != IMG_SUCCESS)
					{
						LOG_ERROR("sendParameterList() failed\n");
						stop();
					}
					break;
				case GUICMD_GET_DPF_MAP:
				LOG_DEBUG("GUICMD_GET_DPF_MAP\n");
				if(DPF_map_send(*_pImgSocket) != IMG_SUCCESS)
				{
					LOG_ERROR("DPF_map_send() failed\n");
					stop();
				}
				break;
				default:
					LOG_ERROR("Unrecognized command received\n");
					stop();
					break;
				}
			}
			else
			{
				LOG_ERROR("Failed to read from socket\n");
				stop();
			}
		}
		else
		{
			LOG_ERROR("Socket error\n");
			stop();
		}

		pauseThread(5);
	}

	setImgHState(STOPPED);
	LOG_DEBUG("ImageHandler stopped\n");
}

int ImageHandler::capture(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nWritten;
	size_t nRead;

	if(socket.read(cmd, sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read from socket\n");
		return IMG_ERROR_FATAL;
	}
	ImageType imgTypeRequested = (ImageType)ntohl(cmd[0]);

	setImgHCapturing(true);
	std::vector<ISPC::Shot> capturedFrames;
	capturedFrames.push_back(ISPC::Shot());
	LOG_DEBUG("ImageHandler capturing...\n");
	_pCameraHandler->takeShot(capturedFrames[0]);

	free(dpfWriteMap); // Clean old dpfWriteMaps
	dpfWriteMap_nDefects = 0;
	dpfWriteMap = (IMG_UINT8 *)malloc(capturedFrames[0].DPF.size); // Allocate memory for new dpfWriteMap
	memcpy(dpfWriteMap, capturedFrames[0].DPF.data, capturedFrames[0].DPF.size); // Copy received map to new dpfWriteMap
	dpfWriteMap_nDefects = capturedFrames[0].DPF.size/capturedFrames[0].DPF.elementSize; // Set number of defects

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
		width = capturedFrames[0].BAYER.width;
		height = capturedFrames[0].BAYER.height;
		stride = capturedFrames[0].BAYER.stride;
		offset = capturedFrames[0].BAYER.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].BAYER.pxlFormat;
		mos = (IMG_UINT16)_pCameraHandler->takeCamera(__LINE__)->getSensor()->eBayerFormat; _pCameraHandler->releaseCamera(__LINE__);
		data = capturedFrames[0].BAYER.data;
		break;
	case RGB:
		width = capturedFrames[0].DISPLAY.width;
		height =  capturedFrames[0].DISPLAY.height;
		stride =  capturedFrames[0].DISPLAY.stride;
		offset = capturedFrames[0].DISPLAY.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].DISPLAY.pxlFormat;
		mos = MOSAIC_NONE;
		data =  capturedFrames[0].DISPLAY.data;
		break;
	case YUV:
		width = capturedFrames[0].YUV.width;
		height = capturedFrames[0].YUV.height;
		stride = capturedFrames[0].YUV.stride;
		offset = capturedFrames[0].YUV.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].YUV.pxlFormat;
		mos = MOSAIC_NONE;
		data = capturedFrames[0].YUV.data;
		break;
	case HDR:
		width = capturedFrames[0].HDREXT.width;
		height = capturedFrames[0].HDREXT.height;
		stride = capturedFrames[0].HDREXT.stride;
		offset = capturedFrames[0].HDREXT.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].HDREXT.pxlFormat;
		mos = MOSAIC_NONE;
		data = capturedFrames[0].HDREXT.data;
		break;
	case RAW2D:
		width = capturedFrames[0].RAW2DEXT.width;
		height = capturedFrames[0].RAW2DEXT.height;
		stride = capturedFrames[0].RAW2DEXT.stride;
		offset = capturedFrames[0].RAW2DEXT.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].RAW2DEXT.pxlFormat;
		mos = (IMG_UINT16)_pCameraHandler->takeCamera(__LINE__)->getSensor()->eBayerFormat; _pCameraHandler->releaseCamera(__LINE__);
		data = capturedFrames[0].RAW2DEXT.data;
		break;
	default:
		LOG_ERROR("Unrecognized image type\n");
		return IMG_ERROR_FATAL;
		break;
	}

	//LOG_DEBUG("Image info %dx%d (str %d) fmt %d-%d\n", width, height, stride, fmt, mos);

	double xScaler = 0;
	double yScaler = 0;
	if(imgTypeRequested == RGB)
	{
		ISPC::ModuleDSC *dsc = _pCameraHandler->takeCamera(__LINE__)->getModule<ISPC::ModuleDSC>(); _pCameraHandler->releaseCamera(__LINE__);
		xScaler = dsc->aPitch[0];
		yScaler = dsc->aPitch[1];
	}
	else if(imgTypeRequested == YUV)
	{
		ISPC::ModuleESC *esc = _pCameraHandler->takeCamera(__LINE__)->getModule<ISPC::ModuleESC>(); _pCameraHandler->releaseCamera(__LINE__);
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
	cmd[7] = htonl((int)true);
	cmd[8] = htonl((int)xScaler*1000);
	cmd[9] = htonl((int)yScaler*1000);
	cmd[10] = htonl((int)getAWBDebugMode());

	if(socket.write(cmd, 11*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to write to socket\n");
		return IMG_ERROR_FATAL;
	}

	// If no data don't send anything
	if(width == 0 || height == 0)
	{
		LOG_WARNING("No image data generated!\n");
		// Release shot always at the end
		if(_pCameraHandler->releaseShot(capturedFrames[0]) != IMG_SUCCESS)
		{
			LOG_ERROR("Error: releaseShot() failed!\n");
			return IMG_ERROR_FATAL;
		}
		capturedFrames.clear();
		setImgHCapturing(false);

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
				LOG_ERROR("Failed to write to socket\n");
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
				LOG_ERROR("Failed to write to socket\n");
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
        if (fmt == PXL_ISP_444IL3YCrCb8 || fmt == PXL_ISP_444IL3YCbCr8 || fmt == PXL_ISP_444IL3YCrCb10 || fmt == PXL_ISP_444IL3YCbCr10)
        {
            size_t toSend = stride*height*sizeof(IMG_UINT8);
            size_t w = 0;
            size_t maxChunk = PARAMSOCK_MAX;

            LOG_DEBUG("Writing image data %d bytes\n", toSend);
            while (w < toSend)
            {
                if (socket.write(((IMG_UINT8*)data) + w, (toSend - w > maxChunk) ? maxChunk : toSend - w, nWritten, -1) != IMG_SUCCESS)
                {
                    LOG_ERROR("Error: failed to write to socket!\n");
                    return IMG_ERROR_FATAL;
                }
                w += nWritten;
                //if(w%(PARAMSOCK_MAX*2) == 0)LOG_DEBUG("image write %ld/%ld\n", w, toSend);
            }
        }
        else if (bYUV && stride != width)
		{
			// the stride is different than the size so we are going to send the image without it
			IMG_UINT8 *toSend = (IMG_UINT8*)data;

            unsigned int size = width*height + 2 * (width / sYUVType.ui8HSubsampling)*(height / sYUVType.ui8VSubsampling);

            LOG_DEBUG("Writing YUV data %d bytes, offset %d\n", width*height + 2 * (width / sYUVType.ui8HSubsampling)*(height / sYUVType.ui8VSubsampling), offset);
            unsigned line = 0;
			for(unsigned line = 0; line < height; line++)
			{
				if(socket.write(toSend + line*stride, width, nWritten, -1) != IMG_SUCCESS)
				{
					LOG_ERROR("Failed to write to socket\n");
					return IMG_ERROR_FATAL;
				}
			}

			toSend += offset;

			for(unsigned line = 0; line < height/sYUVType.ui8VSubsampling; line++)
			{
				if ( socket.write(toSend + line*stride, 2*(width/sYUVType.ui8HSubsampling), nWritten, -1) != IMG_SUCCESS )
				{
					LOG_ERROR("Failed to write to socket\n");
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

			//LOG_DEBUG("Writing image data %d bytes\n", toSend);
			while(w < toSend)
			{
				if(socket.write(((IMG_UINT8*)data)+w, (toSend-w > maxChunk)? maxChunk : toSend-w, nWritten, -1) != IMG_SUCCESS)
				{
					LOG_ERROR("Failed to write to socket\n");
					return IMG_ERROR_FATAL;
				}
				w += nWritten;
                //if(w%(PARAMSOCK_MAX*2) == 0)LOG_DEBUG("fmt %d stride %d - image write %ld/%ld\n", int(fmt), stride, w, toSend);
			}
		}
	}

	// Release shot always at the end
	_pCameraHandler->releaseShot(capturedFrames[0]);
	capturedFrames.clear();
	setImgHCapturing(false);
	LOG_DEBUG("ImageHandler capturing finished\n");

	return IMG_SUCCESS;
}

int ImageHandler::record(ParamSocket &socket)
{
	IMG_UINT32 cmd[MAX_CMD_SIZE];
    size_t nRead, nWritten;

	if(socket.read(cmd, 2*sizeof(IMG_UINT32), nRead, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to read the socket\n");
		return IMG_ERROR_FATAL;
	}
	unsigned int nFrames = ntohl(cmd[0]);
	ImageType imgType = (ImageType)ntohl(cmd[1]);

	std::vector<ISPC::Shot> capturedFrames;
	std::list<IMG_UINT32> captureBufferIds;

	//_pCameraHandler->pauseCmdH();
	//_pCameraHandler->pause();

	LOG_DEBUG("Requested number of frames - %d\n", nFrames);

	if(_pCameraHandler->stopCamera() != IMG_SUCCESS)
	{
		LOG_ERROR("stopCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCameraHandler->allocateBufferPool(nFrames, captureBufferIds) != IMG_SUCCESS)
	{
		LOG_ERROR("allocateBufferPool() failed\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCameraHandler->startCapture() != IMG_SUCCESS)
	{
		LOG_ERROR("startCapture() failed\n");
		return IMG_ERROR_FATAL;
	}

	for(unsigned int i = 0; i < nFrames; i++)
	{
		LOG_DEBUG("Capturing frame %d...\n", i);
		capturedFrames.push_back(ISPC::Shot());
		if(_pCameraHandler->takeShot(capturedFrames[i]) != IMG_SUCCESS)
		{
			LOG_ERROR("takeShot() failed\n");
		   return IMG_ERROR_FATAL;
		}

		free(dpfWriteMap); // Clean old dpfWriteMaps
		dpfWriteMap_nDefects = 0;
		dpfWriteMap = (IMG_UINT8 *)malloc(capturedFrames[i].DPF.size); // Allocate memory for new dpfWriteMap
		memcpy(dpfWriteMap, capturedFrames[i].DPF.data, capturedFrames[i].DPF.size); // Copy received map to new dpfWriteMap
		dpfWriteMap_nDefects = capturedFrames[i].DPF.size/capturedFrames[i].DPF.elementSize; // Set number of defects
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
		width = capturedFrames[0].BAYER.width;
		height = capturedFrames[0].BAYER.height;
		stride = capturedFrames[0].BAYER.stride;
		fmt = (IMG_UINT16)capturedFrames[0].BAYER.pxlFormat;
		mos = (IMG_UINT16)_pCameraHandler->takeCamera(__LINE__)->getSensor()->eBayerFormat; _pCameraHandler->releaseCamera(__LINE__);
		data = capturedFrames[0].BAYER.data;
		break;
	case RGB:
		width = capturedFrames[0].DISPLAY.width;
		height = capturedFrames[0].DISPLAY.height;
		stride = capturedFrames[0].DISPLAY.stride;
		offset = capturedFrames[0].DISPLAY.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].DISPLAY.pxlFormat;
		mos = MOSAIC_NONE;
		data = capturedFrames[0].DISPLAY.data;
		break;
	case YUV:
		width = capturedFrames[0].YUV.width;
		height = capturedFrames[0].YUV.height;
		stride = capturedFrames[0].YUV.stride;
		offset = capturedFrames[0].YUV.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].YUV.pxlFormat;
		mos = MOSAIC_NONE;
		data = capturedFrames[0].YUV.data;
		break;
	case HDR:
		width = capturedFrames[0].HDREXT.width;
		height = capturedFrames[0].HDREXT.height;
		stride = capturedFrames[0].HDREXT.stride;
		offset = capturedFrames[0].HDREXT.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].HDREXT.pxlFormat;
		mos = MOSAIC_NONE;
		data = capturedFrames[0].HDREXT.data;
		break;
	case RAW2D:
		width = capturedFrames[0].RAW2DEXT.width;
		height = capturedFrames[0].RAW2DEXT.height;
		stride = capturedFrames[0].RAW2DEXT.stride;
		offset = capturedFrames[0].RAW2DEXT.offsetCbCr;
		fmt = (IMG_UINT16)capturedFrames[0].RAW2DEXT.pxlFormat;
		mos = (IMG_UINT16)_pCameraHandler->takeCamera(__LINE__)->getSensor()->eBayerFormat; _pCameraHandler->releaseCamera(__LINE__);
		data = capturedFrames[0].RAW2DEXT.data;
		break;
	default:
		LOG_ERROR("Unrecognized image type\n");
		return IMG_ERROR_FATAL;
		break;
	}

	LOG_DEBUG("Image info %dx%d (str %d) fmt %d-%d\n", width, height, stride, fmt, mos);

	double xScaler = 0;
	double yScaler = 0;
	if(imgType == RGB)
	{
		ISPC::ModuleDSC *dsc = _pCameraHandler->takeCamera(__LINE__)->getModule<ISPC::ModuleDSC>(); _pCameraHandler->releaseCamera(__LINE__);
		xScaler = dsc->aPitch[0];
		yScaler = dsc->aPitch[1];
	}
	else if(imgType == YUV)
	{
		ISPC::ModuleESC *esc = _pCameraHandler->takeCamera(__LINE__)->getModule<ISPC::ModuleESC>(); _pCameraHandler->releaseCamera(__LINE__);
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
	cmd[7] = htonl((int)getAWBDebugMode());

	if(socket.write(cmd, 8*sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
	{
		LOG_ERROR("Failed to write to socket\n");
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
        LOG_ERROR("Failed to write to socket\n");
		return IMG_ERROR_FATAL;
    }

	// Send all parameters
	for(unsigned int i = 0; i < params.size(); i++)
	{
		int paramLength = params[i].length();
		cmd[0] = htonl(paramLength);
		if(socket.write(&cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Failed to write to socket\n");
			return IMG_ERROR_FATAL;
		}
		strcpy(buff, params[i].c_str());
		if(socket.write(buff, strlen(buff), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Failed to write to socket\n");
			return IMG_ERROR_FATAL;
		}
		memset(buff, 0, PARAMSOCK_MAX * sizeof(char));
	}


	PIXELTYPE sYUVType;
	bool bYUV = PixelTransformYUV(&sYUVType, (ePxlFormat)fmt) == IMG_SUCCESS;

	for(unsigned int i = 0; i < nFrames; i++)
	{
		switch(imgType)
		{
		case DE:
			data = capturedFrames[i].BAYER.data;
			break;
		case RGB:
			data = capturedFrames[i].DISPLAY.data;
			break;
		case YUV:
			data = capturedFrames[i].YUV.data;
			break;
		case HDR:
			data = capturedFrames[i].HDREXT.data;
			break;
		case RAW2D:
			data = capturedFrames[i].RAW2DEXT.data;
			break;
		default:
			LOG_ERROR("Unrecognized image type\n");
			return IMG_ERROR_FATAL;
			break;
		}

		cmd[0] = htonl(capturedFrames[i].metadata.timestamps.ui32StartFrameOut);
		if(socket.write(cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Failed to write to socket\n");
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
					LOG_ERROR("Failed to write to socket\n");
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
					LOG_ERROR("Failed to write to socket\n");
					return IMG_ERROR_FATAL;
				}
				w += nWritten;
				//if(w%10000 == 0) printf("image sent %ld/%ld\n", w, bop_line*height + 2*(bop_line/sYUVType.ui8HSubsampling)*height/sYUVType.ui8VSubsampling);
			}
		}
		else
		{
			// Send frame data
            if (fmt == PXL_ISP_444IL3YCrCb8 || fmt == PXL_ISP_444IL3YCbCr8 || fmt == PXL_ISP_444IL3YCrCb10 || fmt == PXL_ISP_444IL3YCbCr10)
            {
                size_t toSend = stride*height*sizeof(IMG_UINT8);
                size_t w = 0;
                size_t maxChunk = PARAMSOCK_MAX;

                LOG_DEBUG("Writing image data %d bytes\n", toSend);
                while (w < toSend)
                {
                    if (socket.write(((IMG_UINT8*)data) + w, (toSend - w > maxChunk) ? maxChunk : toSend - w, nWritten, -1) != IMG_SUCCESS)
                    {
                        LOG_ERROR("Error: failed to write to socket!\n");
                        return IMG_ERROR_FATAL;
                    }
                    w += nWritten;
                    //if(w%(PARAMSOCK_MAX*2) == 0)LOG_DEBUG("image write %ld/%ld\n", w, toSend);
                }
            }
            else if (bYUV && stride != width)
			{
				// the stride is different than the size so we are going to send the image without it
				IMG_UINT8 *toSend = (IMG_UINT8*)data;

				LOG_DEBUG("Writing YUV data %d bytes\n", width*height + 2*(width/sYUVType.ui8HSubsampling)*(height/sYUVType.ui8VSubsampling));
				for(unsigned line = 0; line < height; line++)
				{
					if(socket.write(toSend + line*stride, width, nWritten, -1) != IMG_SUCCESS)
					{
						LOG_ERROR("Failed to write to socket\n");
						return IMG_ERROR_FATAL;
					}
				}

				toSend += offset;

				for(unsigned line = 0; line < height/sYUVType.ui8VSubsampling; line++)
				{
					if ( socket.write(toSend + line*stride, 2*(width/sYUVType.ui8HSubsampling), nWritten, -1) != IMG_SUCCESS )
					{
						LOG_ERROR("Failed to write to socket\n");
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
						LOG_ERROR("Failed to write to socket\n");
						return IMG_ERROR_FATAL;
					}
					//if(w%(PARAMSOCK_MAX*2) == 0)LOG_DEBUG("image write %ld/%ld\n", w, toSend);
					w += nWritten;
				}
			}
		}

		// Send nDefects of DPF map
		cmd[0] = htonl(capturedFrames[i].DPF.size/capturedFrames[i].DPF.elementSize);
		if(socket.write(cmd, sizeof(IMG_UINT32), nWritten, N_TRIES) != IMG_SUCCESS)
		{
			LOG_ERROR("Failed to write to socket\n");
			return IMG_ERROR_FATAL;
		}

		// Send DPF map
		size_t toSend = (capturedFrames[i].DPF.size/capturedFrames[i].DPF.elementSize)*sizeof(IMG_UINT64);
		size_t w = 0;
		size_t maxChunk = PARAMSOCK_MAX;

		LOG_DEBUG("Writing dpfMap data %d bytes\n", toSend);
		while(w < toSend)
		{
			if(socket.write(((IMG_UINT8*)capturedFrames[i].DPF.data)+w, (toSend-w > maxChunk)? maxChunk : toSend-w, nWritten, -1) != IMG_SUCCESS)
			{
				LOG_ERROR("Failed to write to socket\n");
				return IMG_ERROR_FATAL;
			}
			//if(w%(PARAMSOCK_MAX) == 0)LOG_DEBUG("dpfMap write %ld/%ld\n", w, toSend);
			w += nWritten;
		}
	}

	for(unsigned int i = 0; i < capturedFrames.size(); i++)
	{
		if(_pCameraHandler->releaseShot(capturedFrames[i]) != IMG_SUCCESS)
		{
			LOG_ERROR("releaseShot() failed\n");
			return IMG_ERROR_FATAL;
		}
	}
	capturedFrames.clear();

	if(_pCameraHandler->stopCapture())
	{
		LOG_ERROR("stopCapture() failed\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCameraHandler->deallocateBufferPool(captureBufferIds))
	{
		LOG_ERROR("Failed to stop capturing\n");
		return IMG_ERROR_FATAL;
	}

	if(_pCameraHandler->takeCamera(__LINE__)->getPipeline()->deleteShots() != IMG_SUCCESS)
	{
		LOG_ERROR("deleteShots() failed\n");
		return IMG_ERROR_FATAL;
	}
	_pCameraHandler->releaseCamera(__LINE__);

	if(_pCameraHandler->startCamera() != IMG_SUCCESS)
	{
		LOG_ERROR("startCamera() failed\n");
		return IMG_ERROR_FATAL;
	}

	LOG_DEBUG("Recoring finished\n");

	//_pCameraHandler->resume();
	//_pCameraHandler->resumeCmdH();

	return IMG_SUCCESS;
}

int ImageHandler::sendParameterList(ParamSocket &socket)
{
	ISPC::ParameterList listToSend;
	_pCameraHandler->takeCamera(__LINE__)->saveParameters(listToSend, ISPC::ModuleBase::SAVE_VAL); _pCameraHandler->releaseCamera(__LINE__);
	//_paramsList += listToSend;

	// Add Sensog params to parameter list
	char intBuff[10];
	const ISPC::Sensor *sensor = _pCameraHandler->takeCamera(__LINE__)->getSensor();
	sprintf(intBuff, "%d", (int)(sensor->getExposure()));
	ISPC::Parameter exposure("SENSOR_CURRENT_EXPOSURE", std::string(intBuff));
	listToSend.addParameter(exposure);
	sprintf(intBuff, "%f", (sensor->getGain()));
	ISPC::Parameter gain("SENSOR_CURRENT_GAIN",  std::string(intBuff));
	listToSend.addParameter(gain);
	sprintf(intBuff, "%d", (int)(sensor->getFocusDistance()));
	ISPC::Parameter focus("SENSOR_CURRENT_FOCUS", std::string(intBuff));
	listToSend.addParameter(focus);
	_pCameraHandler->releaseCamera(__LINE__);

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

int ImageHandler::DPF_map_send(ParamSocket &socket)
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

