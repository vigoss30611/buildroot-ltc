#ifndef ISPC2_TCP
#define ISPC2_TCP

#include "paramsocket/demoguicommands.hpp"
#include "ispc/ParameterList.h"
#include "ispc/Camera.h"
#include <ispc/ControlLSH.h>
#include <pthread.h>

// Data Generator
#ifdef EXT_DATAGEN
#include <sensors/dgsensor.h> // External data generator
#endif
#include <sensors/iifdatagen.h> // Internal data generator

// CSIM
#ifdef FELIX_FAKE
#include <ci_kernel/ci_debug.h> // access to the simulator IP and Port
#include <ispctest/driver_init.h>
#endif

#include "ci/ci_api.h"

#define N_TRIES 5

class CommandHandler;
class ImageHandler;

enum HW_VERSION {
	/** @brief unkown HW */
	HW_UNKNOWN = 0,
	HW_2_X,
	HW_2_4,
	HW_2_6,
	/** @brief Not supported yet */
	HW_3_X,
};

enum HandlerState
{
	DISCONNECTED,
	CONNECTED,
	RUNNING,
	PAUSED,
	WAITING_FOR_CMD,
	STOPPED
};

enum ImageType
{
	DE,
	RGB,
	YUV,
	HDR,
	RAW2D
};

class VL_ControlLSH : public ISPC::ControlLSH
{
public:
    IMG_RESULT load(const ISPC::ParameterList &parameters) { return IMG_SUCCESS; }
};

class CameraHandler
{
public:
	CameraHandler(
		char *appName,
		char *sensor, 
		int sensorMode, 
		int nBuffers,
		char *ip, 
		int port,
		char *DGFrameFile,
		unsigned int gasket,
		unsigned int aBlanking[],
        bool aSensorFlip[]);
	~CameraHandler();
	void start();
	bool isRunning();
	void stop();
	void pause();
	void pauseCmdH();
	void pauseImgH();
	bool isPaused();
	void resume();
	void resumeCmdH();
	void resumeImgH();
	ISPC::Camera *takeCamera(int line);
	void releaseCamera(int line);
	MC_STATS_DPF &takeDPFStats(int line);
	void releaseDPFStats(int line);
	ISPC::Statistics &takeENSStats(int line);
	void releaseENSStats(int line);
	int startCamera();
	int stopCamera();
	int takeShot(ISPC::Shot &frame);
	int releaseShot(ISPC::Shot &frame);
	int startCapture();
	int stopCapture();
	int allocateBufferPool(unsigned int nBuffers, std::list<IMG_UINT32> &bufferIds);
	int deallocateBufferPool(std::list<IMG_UINT32> &bufferIds);
	int updateCameraParameters(ISPC::ParameterList &list);
	int setGMA(CI_MODULE_GMA_LUT &glut);
	HW_VERSION getHWVersion();
    void setPDP(bool enable);
	int shutDown();

private:
	enum DGType
	{
		NO_DG,
		INT_DG,
		EXT_DG
	};

	char *_appName;
	char *_ip;
	int _port;

	CommandHandler *_pCommandHandler;
	int initCommandHandler(
		char *appName,
		char *ip, 
		int port);

	ImageHandler *_pImageHandler;
	int initImageHandler(
		char *appName,
		char *ip, 
		int port);

	int initCameraHandler();
	int NL_registerControlModules();
	void run();

	pthread_mutex_t _hwVerLock;
	pthread_mutexattr_t _hwVerLockAttr;
	HW_VERSION _hwVersion;
	HW_VERSION NL_getHWVersion(ISPC::Camera *camera);

	pthread_mutex_t _runLock;
	pthread_mutexattr_t _runLockAttr;
	bool _run;
	bool _pause;

	double _fps;
	IMG_UINT32 _frameCount;

	pthread_mutex_t _cameraLock;
	pthread_mutexattr_t _cameraLockAttr;
	ISPC::Camera *_pCamera;
	std::list<IMG_UINT32> _bufferIds;
	MC_STATS_DPF _dpfStats;
	ISPC::Statistics _ensStats;

	CI_MODULE_GMA_LUT _GMA_initial;

    bool _PDPState;

    //bool configValid;

	char *_sensor;
	int _sensorMode;
	int _nBuffers;
    int _sensorFlip;

	DGType _dataGen;
	const char *_DGFrameFile;
	unsigned int _gasket;
	unsigned int _aBlanking[2];

	int NL_StartCamera();
	int NL_StopCamera();
	int NL_TakeShot(ISPC::Shot &frame);
	int NL_ReleaseShot(ISPC::Shot &frame);
	int disableSensor();
	int restoreInitialGMA();
	void lock(pthread_mutex_t *mutex, int line);
	void unlock(pthread_mutex_t *mutex, int line);
};

class CommandHandler
{
public:
	CommandHandler(
		char *appName,
		char *ip, 
		int port,
		CameraHandler *pCameraHandler);
	~CommandHandler();
	int connect();
    void start();
	bool isRunning();
	void pause();
	bool isPaused();
	void resume();
	void stop();

private:
    pthread_t _thread;

	char *_appName;

	char *_ip;
	int _port;
	ParamSocketClient *_pCmdSocket;

	CameraHandler *_pCameraHandler;

	pthread_mutex_t _runLock;
	pthread_mutexattr_t _runLockAttr;
	bool _run;
	bool _pause;

	static void *staticEntryPoint(void *c);
    void run();

	int HWINFO_send(ParamSocket &socket);
	int SENSOR_send(ParamSocket &socket);
	int receiveParameterList(ParamSocket &socket);
	int AE_send(ParamSocket &socket);
	int AE_enable(ParamSocket &socket);
	bool _focusing;
	int AF_send(ParamSocket &socket);
	int AF_enable(ParamSocket &socket);
	int AF_triggerSearch(ParamSocket &socket);
	int AWB_send(ParamSocket &socket);
	int AWB_changeMode(ParamSocket &socket);
    std::map<std::string, int> _LSH_Grids;
    IMG_UINT16 _currentLSHGridTileSize;
    IMG_UINT32 _currentLSHGridSize;
	int LSH_addGrid(ParamSocket &socket);
    int LSH_removeGrid(ParamSocket &socket);
    int LSH_applyGrid(ParamSocket &socket);
    int LSH_enable(ParamSocket &socket);
    int LSH_send(ParamSocket &socket);
	int DNS_enable(ParamSocket &socket);
	int DPF_send(ParamSocket &socket);
	int DPF_map_apply(ParamSocket &socket);
	int TNM_send(ParamSocket &socket);
	int TNM_enable(ParamSocket &socket);
	int ENS_send(ParamSocket &socket);
	int LBC_send(ParamSocket &socket);
	int LBC_enable(ParamSocket &socket);
	int GMA_send(ParamSocket &socket);
	int GMA_set(ParamSocket &socket);
    int PDP_enable(ParamSocket &socket);
	int FB_send(ParamSocket &socket);
};

class ImageHandler
{
public:
	ImageHandler(
		char *appName,
		char *ip, 
		int port,
		CameraHandler *pCameraHandler);
	~ImageHandler();
	int connect();
    void start();
	bool isRunning();
	void pause();
	bool isPaused();
	void resume();
	void stop();
	void setAWBDebugMode(bool option);
	bool getAWBDebugMode();

private:
    pthread_t _thread;

	char *_appName;

	char *_ip;
	int _port;
	ParamSocketClient *_pImgSocket;

	CameraHandler *_pCameraHandler;

	pthread_mutex_t _runLock;
	pthread_mutexattr_t _runLockAttr;
	bool _run;
	bool _pause;

	pthread_mutex_t _awbDebugModeLock;
	pthread_mutexattr_t _awbDebugModeLockAttr;
	bool _awbDebugMode;

	IMG_UINT8 *dpfWriteMap;
	IMG_UINT32 dpfWriteMap_nDefects;

	static void *staticEntryPoint(void *c);
    void run();

	int capture(ParamSocket &socket);
	int record(ParamSocket &socket);
	int sendParameterList(ParamSocket &socket);
	int DPF_map_send(ParamSocket &socket);
};

#endif //ISPC_TCP