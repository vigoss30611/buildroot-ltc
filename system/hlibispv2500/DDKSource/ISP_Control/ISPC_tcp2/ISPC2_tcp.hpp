/********************************************************************************
 @file ISPC2_tcp.h

 @brief Header of the ISPC2_tcp class.

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
#ifndef ISPC2_TCP
#define ISPC2_TCP

#include "paramsocket/demoguicommands.hpp"
#include "ispc/ParameterList.h"
#include "ispc/Camera.h"
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

class Listener;

class FeedSender
{
public:
	FeedSender(Listener *parent);
	~FeedSender();
    void start(); // Creates sending thread
	void stop(); // Kills sending thread
	void pause(); // Pause sending thread
	void resume(); // Resume sending thread
	void toggleSending(); // Toggles _sendIMG so images are send through socket or not
	void setFormat(int format); // Sets format of live feed frames
	int getFormat() const; // Returns currently set format
	IMG_UINT32 getFrameCount() const; // Returns number of frames that where captured until now
	bool isRunning() const; // True if live feed is acquiring frames (not nessesery sending them over the socket
	bool isPaused() const; // True if live feed has beed paused (thrad running but is not acquiringany frames)
	double getFps() const; // Returns current fps ratio

private:
	enum ImageType
	{
		DE,
		RGB,
		YUV
	};

	Listener *_parent;
    pthread_t _thread;
	bool _continueRun;
	bool _pause;
	bool _running;
	bool _sendIMG;
	int _format;
	double _fps;
	IMG_UINT32 _frameCount;

	static void *staticEntryPoint(void *c); // Starts run function
    void run(); // Thread loop
	int refresh(bool sendIMG); // Sennds new RGB frame to GUI

	// Image
	int sendImage(ParamSocket &socket, bool feed = false, bool sendIMG = true); // Sends one frame to master. Used by Listener and FeedSender
	int record(ParamSocket &socket);

	// LiveFeed
	int LF_send(ParamSocket &socket);
	int LF_enable(ParamSocket &socket);
	int LF_setFormat(ParamSocket &socket);
};

class Listener
{

	friend class FeedSender;

private:

	enum DGType
	{
		NO_DG,
		INT_DG,
		EXT_DG
	};

	const char *_appName;
	const char *_sensor;
	const int _sensorMode;
    const int _sensorFlip;
	const int _nBuffers;
	const char *_ip;
	const int _port;
	int _missedFrames;
	bool _continueRun;

	bool _sendIMG;

	MC_STATS_DPF dpfStats;

	IMG_UINT8 *dpfWriteMap;
	IMG_UINT32 dpfWriteMap_nDefects;

	ISPC::Statistics ensStats;

	// For data generator only
	const char *_DGFrameFile;
	unsigned int _gasket;
	unsigned int _aBlanking[2];
	DGType _dataGen;

	CI_MODULE_GMA_LUT _GMA_initial;

	ParamSocketClient *_cmdSocket;
	ParamSocketClient *_feedSocket;

	ISPC::ParameterList _paramsList; // Holds all parameters
	int updateCameraParameters(ISPC::ParameterList &list); // Updates parameters in camera

	FeedSender *_liveFeed; // Holds live feed thread object
	void pauseLiveFeed(); // Pauses live feed
	void resumeLiveFeed(); // Resumes live feed
	void stopLiveFeed(); // Ends live feed thread

	ISPC::Camera *_camera; // Holds camera object
	std::list<IMG_UINT32> bufferIds; // Holds ids of all allocated buffers
	int startCamera(); // Start capturing
	int stopCamera(); // Stop capturing
	int registerControlModules(); // Register all control modules in camera
	int takeShot(ISPC::Shot &shot); // Takes one frame. Used by Listener and FeedSender
	int releaseShot(ISPC::Shot &shot); // Releases taken frame. Used by Listener and FeedSender

	void disableSensor(); // Disables sensor (Function called when Ctrl+C is caught to shut down sensor properly)

	int HWINFO_send(ParamSocket &socket);

	// ParameterList
	int receiveParameterList(ParamSocket &socket); // Receives parameterList from master
	int sendParameterList(ParamSocket &socket); // Sends parameter list to maste

	// SENSOR
	int SENSOR_send(ParamSocket &socket);
	
	// AE
	int AE_send(ParamSocket &socket);
	int AE_enable(ParamSocket &socket);

	// AWB
	int AWB_send(ParamSocket &socket);
	int AWB_changeMode(ParamSocket &socket);

	// AF
	bool _focusing;
	int AF_send(ParamSocket &socket);
	int AF_enable(ParamSocket &socket);
	int AF_triggerSearch(ParamSocket &socket);

	// TNM
	int TNM_send(ParamSocket &socket);
	int TNM_enable(ParamSocket &socket);

	// DPF
	int DPF_send(ParamSocket &socket);
	int DPF_map_send(ParamSocket &socket);
	int DPF_map_apply(ParamSocket &socket);

	// LSH
	int LSH_updateGrid(ParamSocket &socket);

	// LBC
	int LBC_send(ParamSocket &socket);
	int LBC_enable(ParamSocket &socket);

	// DNS
	int DNS_enable(ParamSocket &socket);

	// ENS
	int ENS_send(ParamSocket &socket);

	// GMA
	int GMA_send(ParamSocket &socket);
	int GMA_set(ParamSocket &socket);

	// TEST
	int FB_send(ParamSocket &socket);

public:
	Listener(
		// App name
		const char *appName,
		// Sensor
		const char *sensor, 
		const int sensorMode, 
		// Multi-buffering
		const int nBuffers,
		// Connection
		const char *ip, 
		const int port,
		// Data generator
		const char *DGFrameFile,
		unsigned int gasket,
		unsigned int aBlanking[],
        bool aSensorFlip[]);

	~Listener();

	int initCamera(); // Creates and initialise Camera object
	int startConnection(); // Creates connection with master
	void run(); // Main Listener loop
	void stop(); // Called by Live Feed when it stops to end Listener thread
	int cleanUp();
};

#endif //ISPC_TCP