#define LOG_TAG "ISPC_tcp_mk3"

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <dyncmd/commandline.h>
#include "ISPC_tcp_mk3.hpp"
#include <felixcommon/userlog.h>
#include <ci/ci_version.h>
#include <sensorapi/sensorapi.h>

CameraHandler* pCameraHandler = NULL;

void ctrlcHandler(sig_atomic_t sig)
{
	LOG_INFO("CTRL+C sequence caought\n");

	if(pCameraHandler)
	{
		LOG_INFO("Stopping...\n");
		pCameraHandler->stop();
	}
	else
	{
		LOG_ERROR("CameraHandler not initialized\n");
	}
}

int main(int argc, char *argv[])
{
	signal(SIGINT, ctrlcHandler);

	bool fakeDriverInitialized = false;

	char *sensor = NULL;
	unsigned int sensorMode = 0;
	unsigned int nBuffers = 2;
	char *ip = NULL;
	unsigned int port = 0;

	// For data generator only
	char *DGFrameFile = NULL;
	unsigned int gasket = 0;
	unsigned int aBlanking[2] = {FELIX_MIN_H_BLANKING, FELIX_MIN_V_BLANKING};
	bool DGSet = false;

	// CSIM
	char *simIP = 0; // simulator IP address when using TCP/IP interface - NULL means localhost
	unsigned int simPort = 2345;

	char appName[50];
	memset(appName, '\0', strlen(appName)); 

	int ret = RET_FOUND;
	int missing = 0;
    bool bPrintHelp = false;
	IMG_UINT32 unregistered;
    IMG_BOOL8 flip[2]; // horizontal and vertical flipping
    bool sensorFlip[2];

    LOG_INFO("CHANGELIST %s - DATE %s\n", CI_CHANGELIST_STR, CI_DATE_STR);

    ret=DYNCMD_AddCommandLine(argc, argv, "-source");
	if(IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to parse command line\n");
        missing++;
    }

    ret = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND, "usage information", NULL);
    if (RET_FOUND == ret)
    {
        bPrintHelp = true;
    }

    ret = DYNCMD_RegisterParameter("-sensor", DYNCMDTYPE_STRING, "sensor to use", &(sensor));
	if(RET_FOUND != ret && !bPrintHelp)
    {
        sensor = NULL;
        LOG_ERROR("-sensor option not found!\n");
        missing++;
    }

	if(sensor && strcmp(sensor, IIFDG_SENSOR_INFO_NAME) == 0)
	{
		DGSet = true;
	}
#ifdef EXT_DATAGEN
	if(sensor && strcmp(sensor, EXTDG_SENSOR_INFO_NAME) == 0)
	{
		DGSet = true;
	}
#endif

    ret = DYNCMD_RegisterParameter("-sensorMode", DYNCMDTYPE_UINT, "[optional] sensor running mode", &(sensorMode));
    if (RET_FOUND != ret && !bPrintHelp)
    {
		LOG_INFO("Info: -sensorMode option not found. Using default - %d\n", sensorMode);
    }
    
    ret=DYNCMD_RegisterArray("-sensorFlip", DYNCMDTYPE_BOOL8, 
        "[optional] Set 1 to flip the sensor Horizontally and Vertically. If only 1 option is given it is assumed to be the horizontal one\n",
        flip, 2);
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_INCOMPLETE == ret)
        {
            LOG_WARNING("incomplete -sensorFlip, flipping is H=%d V=%d\n", (int)flip[0], (int)flip[1]);
        }
        else if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("Error while parsing parameter -sensorFlip\n");
            missing++;
        }
    }

    ret = DYNCMD_RegisterParameter("-setupIP", DYNCMDTYPE_STRING, "IP adress of server aplication", &(ip));
    if (RET_FOUND != ret && !bPrintHelp)
    {
		ip = NULL;
        LOG_ERROR("-setupIP option not found!\n");
        missing++;
    }

    ret = DYNCMD_RegisterParameter("-setupPort", DYNCMDTYPE_UINT, "port which server app is listening on", &(port));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        LOG_ERROR("-setupPort option not found!\n");
        missing++;
    }

    ret = DYNCMD_RegisterParameter("-nBuffers", DYNCMDTYPE_UINT, "[optional] number of pre-allocated buffers to run with", &(nBuffers));
	if(RET_FOUND != ret && !bPrintHelp)
    {
        LOG_INFO("Info: -nBuffers option not found. Using default - %d\n", nBuffers);
    }

    ret = DYNCMD_RegisterParameter("-DG_inputFLX", DYNCMDTYPE_STRING, "FLX file to use as an input of the data-generator (only if specified sensor is a data generator)", &(DGFrameFile));
    if (RET_FOUND != ret && !bPrintHelp)
    {
		if(DGSet)
		{
			LOG_ERROR("-DG_inputFLX option not found! For %s this option is mandatory.\n", sensor);
			missing++;
		}
    }
        
    ret = DYNCMD_RegisterParameter("-DG_gasket", DYNCMDTYPE_UINT, "Gasket to use for data-generator (only if specified sensor is a data generator)", &(gasket));
    if (RET_FOUND != ret && !bPrintHelp)
    {
		if(DGSet)
		{
			LOG_INFO("-DG_gasket option not found. Using default - %d\n", gasket);
		}
    }
        
    ret = DYNCMD_RegisterArray("-DG_blanking", DYNCMDTYPE_UINT, "Horizontal and vertical blanking in pixels (only if specified sensor is data generator)", &(aBlanking), 2);
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if(ret == RET_INCOMPLETE || ret == RET_NOT_FOUND)
        {
			LOG_INFO("-DG_blanking option incomplete or not found. Using blanking values - {%u, %u}\n", aBlanking[0], aBlanking[1]);
        }
        else if(ret != RET_NOT_FOUND)
        {
            LOG_ERROR("Error while parsing parameter -DG_blanking\n");
			missing++;
        }
    }

#ifdef FELIX_FAKE
    ret = DYNCMD_RegisterParameter("-simIP", DYNCMDTYPE_STRING, "Simulator IP address to use when using TCP/IP interface.", &(simIP));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        simIP = IMG_STRDUP("localhost");
        if(ret != RET_NOT_FOUND)
        {
            missing++;
            LOG_ERROR("-simIP expects a string but none was found\n");
        }
    }
	
	ret = DYNCMD_RegisterParameter("-simPort", DYNCMDTYPE_UINT, "Simulator port to use when running TCP/IP interface.", &(simPort));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if(ret != RET_NOT_FOUND)
        {
            missing++;
            LOG_ERROR("-simPort expects an int but none was found\n");
        }
    }
#endif // FELIX_FAKE
	
    /*
     * should be the last one
     */
    if(bPrintHelp || missing > 0)
    {
        DYNCMD_PrintUsage();
        
        Sensor_PrintAllModes(stdout);
        missing++;
    }

    DYNCMD_HasUnregisteredElements(&unregistered);

    DYNCMD_ReleaseParameters();

    if(missing > 0) goto exit_point;

#ifdef FELIX_FAKE
	sprintf(appName, "CSIM"); 
    // global variables from ci_debug.h
	pszTCPSimIp = simIP;
	ui32TCPPort = simPort;
    bEnablePdump = IMG_FALSE;
 
    initializeFakeDriver(2, 0); // FAKE driver insmod
	fakeDriverInitialized = true;
#else
	sprintf(appName, "%s (mode %d)", sensor, sensorMode); 
#endif // FELIX_FAKE

    sensorFlip[0] = flip[0]==IMG_TRUE;
    sensorFlip[1] = flip[1]==IMG_TRUE;
    printf("sensorFlip=%d,%d\n", (int)sensorFlip[0], (int)sensorFlip[1]);
    
	pCameraHandler = new CameraHandler(appName, sensor, sensorMode, nBuffers, ip, port, DGFrameFile, gasket, aBlanking, sensorFlip);
	if(pCameraHandler)
	{
		pCameraHandler->start();
	}

exit_point:
	if(pCameraHandler) delete(pCameraHandler);
    if(sensor) IMG_FREE(sensor);
    if(ip) IMG_FREE(ip);
    if(DGFrameFile) IMG_FREE(DGFrameFile);
    if(simIP) IMG_FREE(simIP);

#ifdef FELIX_FAKE
    if(fakeDriverInitialized) finaliseFakeDriver(); // FAKE driver rmmod
#endif

	return 0;
}
