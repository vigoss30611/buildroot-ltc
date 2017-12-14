#ifndef _IPC_SHM_H_
#define _IPC_SHM_H_

#include "info_log.h"
#include "shm_util.h"
#include "wifi_operation.h"
#include "av_video_common.h"
#include "Appro_avi_shm.h"
#include "wifi.h"


typedef struct {
    IPC_VIDEO_ActivePipeline activePipeline;
    WifiOperationT wifiOperation;
    ISHM_VIDEO_PARAMETER videoParam;
    T_ishm_file_save fileSave;
    T_ishm_sdcard sdcard;
    int sensorFps;
    int targetFps;
} T_IPC_Shm;


#ifdef __cplusplus 
extern "C" {
#endif

int ipc_shm_init();
void ipc_shm_open();
void  ipc_shm_free();

/*-------------------------------------------------------------------------------------
            GET/SET operations
-------------------------------------------------------------------------------------*/

// parameter pipeline
IPC_VIDEO_ActivePipeline* ipc_shm_getActivePipelines(); 
IPC_VIDEO_PipelineInfo* ipc_shm_getActivePipelineById(unsigned int pipelineId);

// wifi
WifiOperationT* ipc_shm_getWifiOpera();

// parameter dv mode
T_DV_MOD ipcDvModGet() ;
void ipcDvModSet(T_DV_MOD mod) ;  
T_DV_MOD ipcDvModNext() ;

// parameter picture burst type
T_BURST_TYPE ipcBurstTypeGet() ;
void ipcBurstTypeSet(T_BURST_TYPE mod) ;

// parameter gui flag
int ipcIsGuiFlagGet();
void ipcIsGuiFlagSet(int flag);

// parameter file save
void ishm_setFileSaveParam(int steamId_in, int duration_in, int frameRate_in);
void ishm_setFileSaveStopFlag(int flag);
int  ishm_getIsAviSaveRunning();
void ishm_setIsAviSaveRunning(int flag);

// parameter sd card
unsigned int ishm_getSDCardInserted();
void ishm_setSDCardInserted(unsigned int inserted);
void ishm_getSDCardCapacity(unsigned int *totalKB, unsigned int *freeKB);
void ishm_setSDCardCapacity(unsigned int totalKB, unsigned int freeKB);

int ishm_getTargetFPS();
void ishm_setTargetFPS(int targetFps);
int ishm_getSensorFPS();
void ishm_setSensorFPS(int sensorFps);


#ifdef __cplusplus 
}
#endif

#endif

