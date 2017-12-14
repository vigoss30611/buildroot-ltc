#include "recorder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/input.h>
#include <stdarg.h>


#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <fr/libfr.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>

#include <qsdk/vplay.h>
#include <qsdk/cep.h>

#include "ipc.h"
#include "looper.h"

#include "hd_mng.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPI.h"
#include "QMAPIRecord.h" 

#include "Ftp.h"
#include "DiskFullMng.h"

#include "recorder.h"
#include "motion_controller.h"

#define LOGTAG  "QM_RECORD"

typedef enum {
    RECORD_STOP,
    RECORD_START,
} RECORD_STATE_E;

typedef struct {
    looper_t schedule_lp;
    pthread_mutex_t mutex;
    int inited;
    int started;
    RECORD_STATE_E record_state;
    
    int ioctrl_handle;
    int hd_state;
    int time_state;
    int motion_state;
    RECORD_ATTR_T attr;
    record_callback callback;
} recorder_manager_t;

static recorder_manager_t sRecorderManager = {
    .inited = 0,
    .started = 0,
    .record_state = RECORD_STOP,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .ioctrl_handle = -1,
    .callback = NULL,
};

int recorder_manager_update_motion_state(int motion);
int recorder_manager_update_time_schedule(int comming);

extern int recorder_file_manager_add_file(RECORD_FILE_T *filename);
extern int snap_write(char *FileName, char *SnapData, int SnapLen);
extern int snap_read(char *FileName, char *SnapData, int SnapLen);


//TODO: should modify code or move out of this file
/*
   事件回调函数
   1. 记录日志
   2. 开启录像
   3. 抓图
   */
int REC_EventCallBack(int nChannel, int nAlarmType, int nAction, void* pParam)
{
    int ret = 0;
    int snap_len = 0;
    char filename[128];
    //TAVRecord *precd = (TAVRecord *)&m_avrecord[nChannel * MAX_STOR_TYPE + 0];
    QMAPI_NET_SNAP_DATA snap;
    QMAPI_SYSTEMTIME *systime = (QMAPI_SYSTEMTIME *)pParam;
    recorder_manager_t *rcm =  &sRecorderManager;

    ipclog_info("%s %d %d %d %p\n", __FUNCTION__, nChannel, nAlarmType, nAction, pParam);

    if (nAlarmType == QMAPI_ALARM_TYPE_VMOTION)
    {
        printf("REC_EventCallBack motion start ch:%d. \n", nChannel);
#if 0
        ret = REC_StartRecord(0, TRIGER_MOTION, nChannel, 60*1000);
        if (ret) {
			printf("func:%s(%d), REC_StartRecord error!!\n",__FUNCTION__, __LINE__);
			return -1;
        }
#endif	
        do {
            int diskNo = -1;
            char mountPath[128];
            QMAPI_NET_CHANNEL_MOTION_DETECT stMotionDetectCfg;
            if (diskNo < 0) {
                QMAPI_NET_HDCFG hd;
                if (QMapi_sys_ioctrl(rcm->ioctrl_handle, QMAPI_SYSCFG_GET_HDCFG, 0, &hd, sizeof(QMAPI_NET_HDCFG))==QMAPI_SUCCESS && hd.dwHDCount) {
                    int i, nFreeSize=0, nMax=-1;
                    for ( i=0; i<hd.dwHDCount; i++) {
                        if (!hd.stHDInfo[i].dwHdStatus && nFreeSize<hd.stHDInfo[i].dwFreeSpace) {
                            nFreeSize = hd.stHDInfo[i].dwFreeSpace;//保存最大空间
                            nMax = i;
                        }
                    }

                    if (nMax != -1) {
                        diskNo = nMax;
                        memcpy(mountPath, hd.stHDInfo[nMax].byPath, 32);
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }

            if (QMapi_sys_ioctrl(rcm->ioctrl_handle, QMAPI_SYSCFG_GET_MOTIONCFG, nChannel, &stMotionDetectCfg, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT))==QMAPI_SUCCESS 
                    && stMotionDetectCfg.stHandle.wActionFlag & QMAPI_ALARM_EXCEPTION_TOSNAP) {
                snap.nChannel = nChannel;
                snap.nLevel = 1;
                snap.dwFileType = 0;
                snap.dwBufSize = 1024*1024;
                snap.pDataBuffer = malloc(snap.dwBufSize);
                //snap.pDataBuffer = NULL;
                if (snap.pDataBuffer) {
                    if (QMapi_sys_ioctrl(rcm->ioctrl_handle, QMAPI_NET_STA_SNAPSHOT, nChannel, &snap, sizeof(QMAPI_NET_SNAP_DATA)) == QMAPI_SUCCESS) {
                        sprintf(filename, "%s/%04d%02d%02d/picture/%02d%02d%02d%02d%02d.JPEG", mountPath,systime->wYear,systime->wMonth,systime->wDay,
                                nChannel,nAlarmType,systime->wHour,systime->wMinute,systime->wSecond);
                        snap_len = snap_write(filename, snap.pDataBuffer, snap.dwBufSize);
                        if (snap_len != snap.dwBufSize) {
                            printf("[MOTION_DETECT]: write jpeg to %s failed: date length %lu, write %d\n", filename, snap.dwBufSize, snap_len);
                        } else {
                            // upload file to FTP
                            QMAPI_NET_FTP_PARAM finfo;
                            if (QMapi_sys_ioctrl(rcm->ioctrl_handle, QMAPI_SYSCFG_GET_FTPCFG, 0, &finfo, sizeof(QMAPI_NET_FTP_PARAM)) == QMAPI_SUCCESS 
                                    && finfo.bEnableFTP) {
                                int retry=0;
                                int ret=-1;
                                char ftp_filename[128];
                                char ftp_topdir[64];
                                char ftp_subdir[64];
                                int use_topdir=0;
                                int use_subdir=0;

                                switch (finfo.wTopDirMode) 
                                {
                                    case 0:
                                        if (0 == get_ip_addr("eth0", ftp_topdir)) {
                                            use_topdir = 1;
                                        }
                                        break;

                                    case 1:
                                        {
                                            QMAPI_NET_DEVICE_INFO dinfo;
                                            if (QMapi_sys_ioctrl(rcm->ioctrl_handle, QMAPI_SYSCFG_GET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO)) 
                                                    == QMAPI_SUCCESS) {
                                                use_topdir=1;
                                                sprintf(ftp_topdir, "%s", dinfo.csDeviceName);
                                            }
                                        }
                                        break;
                                    default:
                                        //none:
                                        break;
                                }

                                switch (finfo.wSubDirMode)
                                {
                                    case 0:
                                        { // channel id
                                            use_subdir=1;
                                            sprintf(ftp_subdir, "channel%d", nChannel);
                                        }
                                        break;
                                    case 1:
                                        { // channel name
                                            //FIXME: how to get channel name
                                            use_subdir=1;
                                            sprintf(ftp_subdir, "channel%d", nChannel);
                                        }
                                        break;
                                    default:
                                        break;
                                }

                                if (use_topdir && use_subdir) {
                                    sprintf(ftp_filename, "%s/%s/%02d%04d%02d%02d%02d%02d%02d.JPEG",ftp_topdir,ftp_subdir,nAlarmType,
                                            systime->wYear,systime->wMonth,systime->wDay,systime->wHour,systime->wMinute,systime->wSecond);
                                } else if (use_topdir) {
                                    sprintf(ftp_filename, "%s/%02d%04d%02d%02d%02d%02d%02d.JPEG",ftp_topdir,nAlarmType,
                                            systime->wYear,systime->wMonth,systime->wDay,systime->wHour,systime->wMinute,systime->wSecond);
                                } else {
                                    sprintf(ftp_filename, "%02d%02d%04d%02d%02d%02d%02d%02d.JPEG",nChannel,nAlarmType,
                                            systime->wYear,systime->wMonth,systime->wDay,systime->wHour,systime->wMinute,systime->wSecond);
                                }

                                printf("[MOTION_DETECT]: upload %s to ftp: %s\n", filename, ftp_filename);
                                while (ret && retry++<5) {
                                    ret = ftpPutFile(finfo.csFTPIpAddress, finfo.dwFTPPort, finfo.csUserName, finfo.csPassword, filename, ftp_filename, TRUE);
                                }

                                if(ret==0)
                                {
                                    remove(filename);
                                }
                            }
                        }
                    }

                    free(snap.pDataBuffer);
                }
            }
		
#if 0
            REC_PTHREAD_MUTEX_LOCK(&precd->log_mutex);
			
            if (!precd->logHead) {
				sprintf(filename, "%s/%04d%02d%02d/index.dat",precd->sMountPath, systime->wYear, systime->wMonth, systime->wDay);
				Local_CheckAndMkdir(filename);
				precd->logHead = log_init(filename, 1);
			}
			
            if (precd->logHead) {
				struct tm st;
				log_content_t cont;
				
				cont.log_ch = nChannel;
				cont.log_type = TRIGER_MOTION;
				cont.log_opt = 1<<0;
				if (snap_len) cont.log_opt |= 1<<1;
				st.tm_year = systime->wYear - 1900;
				st.tm_mon  = systime->wMonth - 1;
				st.tm_mday = systime->wDay;
				st.tm_hour = systime->wHour;
				st.tm_min  = systime->wMinute;
				st.tm_sec  = systime->wSecond;
				cont.log_time = mktime(&st);
				
                if (precd->hRecord.nStat) {
					memcpy(cont.log_text, m_avrecord[nChannel * MAX_STOR_TYPE + 0].sFileName, LOG_TEXT_LEN);
				}

				log_write(precd->logHead, &cont, 1);
				log_exit(precd->logHead);
				precd->logHead = NULL;
			}

			REC_PTHREAD_MUTEX_UNLOCK(&precd->log_mutex);
#endif
		} while(0);
	} else if (nAlarmType == QMAPI_ALARM_TYPE_VMOTION_RESUME) {
		printf("REC_EventCallBack motion stop ch:%d. \n", nChannel);
		//REC_StopRecord(TRIGER_MOTION, nChannel);
	}
	
	return 0;
}


void recManagerCallback(int type, const char *filename)
{
    RECORD_NOTIFY_T notify;
    RECORD_FILE_T file;
    memset(&notify, 0, sizeof(notify));
    memset(&file, 0, sizeof(file));
    ipclog_info("%s type:%d %p\n", __FUNCTION__, type, filename);

    recorder_manager_t *rcm = &sRecorderManager;
    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return;
    }
    pthread_mutex_unlock(&rcm->mutex);

    switch(type) {
        case VPLAY_EVENT_NEWFILE_START:
            file.trigger = 0x01;
            file.name    = filename;

            notify.type = RECORD_NOTIFY_START;
            notify.arg = &file;

            QM_Event_Send(EVENT_RECORD, &notify, sizeof(notify));
            break;

        case VPLAY_EVENT_NEWFILE_FINISH:
            file.trigger = 0x01;
            file.name    = filename;

            notify.type = RECORD_NOTIFY_FINISH;
            notify.arg = &file;

            QM_Event_Send(EVENT_RECORD, &notify, sizeof(notify));
            
            if (rcm->callback) {
                rcm->callback(&file);
            } else {
                recorder_file_manager_add_file(&file); 
            }
            break;

        case VPLAY_EVENT_VIDEO_ERROR:
        case VPLAY_EVENT_AUDIO_ERROR:
        case VPLAY_EVENT_DISKIO_ERROR:
            recorder_manager_update_hd_state(0);
            
            notify.type = RECORD_NOTIFY_ERROR;
            notify.arg = NULL;
            QM_Event_Send(EVENT_RECORD, &notify, sizeof(notify));
            break;

        default:
            break;
    }


    return;
}

#if 0
//for test only
typedef struct {
    int hd_state;
    int motion_state;
    int time_comming;
} TRIGGER_CONDITION_T;

#define TRIGGER_ARRAY_MAX 9
#define ATTR_ARRAY_MAX    5

static TRIGGER_CONDITION_T triggerArrays[TRIGGER_ARRAY_MAX] = {
    {1, 1, 1},
    {0, 1, 0},
    {1, 0, 0},

    {0, 0, 1},
    {1, 0, 0},
    {0, 0, 1},

    {1, 1, 0},
    {0, 1, 1},
    {1, 0, 0},
    {0, 1, 1},
};

static RECORD_ATTR_T attrArrays[ATTR_ARRAY_MAX] = {
    {0, 1 , 1 , 0, 0},
    {1, 1 , 0 , 0, 0},
    {0, 1 , 1 , 0, 0},
    {1, 1 , 0 , 0, 0},
    {0, 1 , 1 , 0, 0},
};

#endif

static void *time_schedule_process(void *p)
{
    int req;
    recorder_manager_t *rcm = (recorder_manager_t *)p;
    int last_time_comming = 0;
    int time_comming = 0;
    int count = 51;
    
    int trigger_index = 0;
    int attr_index = 0;

    ipclog_debug("%s enter \n", __FUNCTION__);

    /*!< loop of timer processing */
    lp_lock(&rcm->schedule_lp);
    lp_set_state(&rcm->schedule_lp, STATE_IDLE);
    while (lp_get_req(&rcm->schedule_lp) != REQ_QUIT) {
        /*!< wait for start/quit requst when in idle state */
        if (lp_get_state(&rcm->schedule_lp) == STATE_IDLE) {
            req = lp_wait_request_l(&rcm->schedule_lp);
            if (req == REQ_START) {
                /*!< do business */
                ipclog_debug("timer start to do things\n");
            } else if (req == REQ_QUIT) {
                goto Quit;
            }
        } else {
            /*!< check stop request */
            if (lp_check_request_l(&rcm->schedule_lp) == REQ_STOP) {
                continue;
            }
        }

        if (count++ > 50) {
            time_comming = !time_comming;
            count = 0;
        } 

        if (last_time_comming != time_comming) {
#if 0
            //for test only
            RECORD_ATTR_T *attr = &attrArrays[attr_index];
            ipclog_info("%s mode:%d stream:%d\n", __FUNCTION__, attr->mode, attr->videoStream);
            
            recorder_manager_set_mode(attr->mode);
            recorder_manager_set_stream(attr->videoStream);

            TRIGGER_CONDITION_T *trigger = &triggerArrays[trigger_index];
            ipclog_info("%s hd_state:%d motion_state:%d time_comming:%d \n", __FUNCTION__, 
                    trigger->hd_state, trigger->motion_state, trigger->time_comming);
            recorder_manager_update_hd_state(trigger->hd_state);
            recorder_manager_update_motion_state(trigger->motion_state);
            recorder_manager_update_time_schedule(trigger->time_comming);

            if (trigger_index++ >= TRIGGER_ARRAY_MAX) {
                trigger_index = 0;
            }

            if (attr_index++ >= ATTR_ARRAY_MAX) {
                attr_index = 0;
            }
#endif      

            recorder_manager_update_time_schedule(1);
#if 0
            if (rcm->attr.mode == MEDIA_RECMODE_FULLTIME) {
                recorder_manager_update_time_schedule(time_comming);
            }
#endif
            last_time_comming = time_comming;
        }

        /*!< timed check per-proc */
        lp_unlock(&rcm->schedule_lp);
        usleep(500*1000);
        lp_lock(&rcm->schedule_lp);

        /*!< for requestion checking */
        lp_unlock(&rcm->schedule_lp);
        usleep(0);
        lp_lock(&rcm->schedule_lp);
    }

Quit:
    lp_unlock(&rcm->schedule_lp);
    ipclog_debug("%s exit\n", __FUNCTION__);
    return NULL;
}


static int recManagerTimeScheduleStart(void)
{
    recorder_manager_t *rcm =  &sRecorderManager;
    /*!< create record looper */
    if (lp_init(&rcm->schedule_lp, "time_schedule_process", time_schedule_process, 
                (void *)rcm) != 0) {
        ipclog_ooo("create schedule looper failed\n");
        return -1;
    }

    lp_lock(&rcm->schedule_lp);
    lp_start(&rcm->schedule_lp, true);
    lp_unlock(&rcm->schedule_lp);
    
    return 0;
}

static int recManagerTimeScheduleStop(void)
{
    recorder_manager_t *rcm =  &sRecorderManager;
    lp_deinit(&rcm->schedule_lp);
    return 0;
}

//TODO: may be deadlock here
void motion_trigger_handler(const char *event, void *arg, int size)
{
    int motion = *((int *)arg);
    recorder_manager_update_motion_state(motion);
}

static int recManagerMotionHandleStart(void)
{
    int ret = QM_Event_RegisterHandler(EVENT_MOTION_DETECT, 0, motion_trigger_handler);
    return 0;
}

static int recManagerMotionHandleStop(void) 
{
    QM_Event_UnregisterHandler(EVENT_MOTION_DETECT, motion_trigger_handler);
    return 0;
}


int hd_handler(const char *event, void *arg, int size)
{
    DISK_INFO_T *info = (DISK_INFO_T*)arg;
    ipclog_info("%s event:%d \n", __FUNCTION__, info->event);
    switch (info->event) {
        case HD_EVENT_MOUNT:
            recorder_manager_update_hd_state(1);
            break;
        case HD_EVENT_UNMOUNT:
            recorder_manager_update_hd_state(0);
            break;
        case HD_EVENT_ERROR:
            recorder_manager_update_hd_state(0);
            break;
        default:
            break;
    }    

    return 0;
}

int recorder_manager_init(RECORD_ATTR_T *attr, void* callback)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);

    pthread_mutex_lock(&rcm->mutex);
    if (rcm->inited) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    rcm->callback = (record_callback)callback;

    memcpy(&rcm->attr, attr, sizeof(RECORD_ATTR_T));
    infotm_record_init(&rcm->attr, recManagerCallback);

    rcm->started = 0;
    rcm->record_state = 0;
    rcm->hd_state  = 0;
    rcm->motion_state = 0;
    rcm->time_state = 0;

    rcm->inited = 1;
 
    pthread_mutex_unlock(&rcm->mutex);
    
    QM_Event_RegisterHandler(EVENT_HD_NOTIFY, 0, hd_handler);
   
    rcm->ioctrl_handle = QMapi_sys_open(QMAPI_NETPT_REC);
    if (rcm->ioctrl_handle < 0) {
        ipclog_error("Open platform error!\n");
        return 0;
    }

    QMapi_sys_ioctrl(rcm->ioctrl_handle, QMAPI_NET_REG_ALARMCALLBACK, 0, REC_EventCallBack, 0);

    return 0;
}

int recorder_manager_deinit(void)
{
    recorder_manager_t *rcm = &sRecorderManager;

    QM_Event_UnregisterHandler(EVENT_HD_NOTIFY, hd_handler);

    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    infotm_record_deinit();

    rcm->started = 0;
    rcm->record_state = 0;
    rcm->hd_state  = 0;
    rcm->motion_state = 0;
    rcm->time_state = 0;

    rcm->inited = 0;

    pthread_mutex_unlock(&rcm->mutex); 
    
    if (rcm->ioctrl_handle >= 0) {
        QMapi_sys_close(rcm->ioctrl_handle);
        rcm->ioctrl_handle = -1;
    }

    return 0;
}

int recorder_manager_start(void)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);

    pthread_mutex_lock(&rcm->mutex);

    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (rcm->started) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    rcm->started = 1;
    pthread_mutex_unlock(&rcm->mutex);

    //if (rcm->attr.mode == MEDIA_RECMODE_MOTDECT) {
    recManagerMotionHandleStart();
    //} else {
    recManagerTimeScheduleStart();
    //}

    disk_manager_start(rcm->ioctrl_handle);   

    return 0;
}

int recorder_manager_stop(void)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);

    pthread_mutex_lock(&rcm->mutex);

    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (!rcm->started) {
        ipclog_warn("%s not started\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (rcm->record_state == RECORD_START) {
        infotm_record_stop();
        rcm->record_state = RECORD_STOP;
    }

    rcm->started = 0;
    pthread_mutex_unlock(&rcm->mutex);

    //if (rcm->attr.mode == MEDIA_RECMODE_MOTDECT) {
    recManagerMotionHandleStop();
    //} else {
    recManagerTimeScheduleStop();
    //}

    disk_manager_stop(rcm->ioctrl_handle);   

    return 0;
}


static int recManagerJustice_l(void)
{   
    int state  = RECORD_STOP;
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s old record_state:%d \n", __FUNCTION__, rcm->record_state);
    if (rcm->attr.mode == MEDIA_RECMODE_MOTDECT) {
        if (rcm->motion_state) {
            state = RECORD_START;
        } else {
            state = RECORD_STOP;
        }
    } else {
        if (rcm->time_state) {
            state = RECORD_START;
        } else {
            state = RECORD_STOP;
        }
    }
    
    /* overwrite state when hw_state is not ready*/
    if (rcm->hd_state == 0) {
        ipclog_info("%s overwrite state cause hw_state is 0\n", __FUNCTION__);
        state = RECORD_STOP;
    }

    if (rcm->record_state != state) {
        if (state == RECORD_START) {
            infotm_record_start();
            rcm->record_state = RECORD_START;
        } else {
            infotm_record_stop();
            rcm->record_state = RECORD_STOP;
        }
    }

    ipclog_info("%s new record_state:%d \n", __FUNCTION__, rcm->record_state);

    return 0;
}

int recorder_manager_update_hd_state(int ready)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);

    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }
    
    if (!rcm->started) {
        ipclog_warn("%s not started\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    rcm->hd_state = ready;
    recManagerJustice_l();
    
    pthread_mutex_unlock(&rcm->mutex);
    return 0;
}

int recorder_manager_update_motion_state(int motion)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);
    
    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (!rcm->started) {
        ipclog_warn("%s not started\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (rcm->attr.mode != MEDIA_RECMODE_MOTDECT) {
        ipclog_warn("record mode is not motdect now\n");
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    } 

    rcm->motion_state = motion;
    recManagerJustice_l();
    
    pthread_mutex_unlock(&rcm->mutex);
    return 0;
}

int recorder_manager_update_time_schedule(int comming)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);

    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (!rcm->started) {
        ipclog_warn("%s not started \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (rcm->attr.mode != MEDIA_RECMODE_FULLTIME) {
        ipclog_warn("record mode is not fulltime now\n");
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    } 

    rcm->time_state = comming;
    recManagerJustice_l();

    pthread_mutex_unlock(&rcm->mutex);

    ipclog_info("%s done\n", __FUNCTION__);
    return 0;
}

int recManagerSetAttribute_l(RECORD_ATTR_T *attr)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);
    int record_state = rcm->record_state;
    //1. stop & deinit
    #if 0
    if (rcm->attr.mode == MEDIA_RECMODE_MOTDECT) {
        recManagerMotionHandleStop();
    } else {
        recManagerTimeScheduleStop();
    }
    #endif

    if (record_state == RECORD_START) {
        infotm_record_stop();
    }
    infotm_record_deinit();

    //2. update new attr
    memcpy(&rcm->attr, attr, sizeof(RECORD_ATTR_T));

    //3. reinit & restart
    infotm_record_init(attr, recManagerCallback);
    if (record_state == RECORD_START) {
        infotm_record_start();
    }

 #if 0
    if (rcm->attr.mode == MEDIA_RECMODE_MOTDECT) {
        recManagerMotionHandleStart();
    } else {
        recManagerTimeScheduleStart();
    }
#endif

    return 0;
}

int recorder_manager_set_attr(RECORD_ATTR_T *attr)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);
    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    recManagerSetAttribute_l(attr);
    
    pthread_mutex_unlock(&rcm->mutex);
    return 0;
}

int recorder_manager_get_attr(RECORD_ATTR_T *attr)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);
    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return -1;
    }

    memcpy(attr, &rcm->attr, sizeof(RECORD_ATTR_T));
    pthread_mutex_unlock(&rcm->mutex);
    return 0;
}


int recorder_manager_set_mode(int mode)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s mode:%d \n", __FUNCTION__, mode);
    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (!rcm->started) {
        ipclog_warn("%s not started\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (rcm->attr.mode == mode) {
        ipclog_warn("%s mode:%d already \n", __FUNCTION__, mode);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    } 

    //TODO: check mode
    //assert((mode == MEDIA_RECMODE_FULLTIME) || (mode == MEDIA_RECMODE_MOTDECT));

    RECORD_ATTR_T attr;
    memcpy(&attr, &rcm->attr, sizeof(RECORD_ATTR_T));
    attr.mode = mode;
    recManagerSetAttribute_l(&attr);

    pthread_mutex_unlock(&rcm->mutex);
    return 0;
}

int recorder_manager_update_time(void)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s \n", __FUNCTION__);

    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (!rcm->started) {
        ipclog_warn("%s not started\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    RECORD_ATTR_T attr;
    memcpy(&attr, &rcm->attr, sizeof(RECORD_ATTR_T));
    recManagerSetAttribute_l(&attr);

    pthread_mutex_unlock(&rcm->mutex);
    return 0;
}

int recorder_manager_set_stream(int stream)
{
    recorder_manager_t *rcm = &sRecorderManager;
    ipclog_info("%s stream:%d \n", __FUNCTION__, stream);

    pthread_mutex_lock(&rcm->mutex);
    if (!rcm->inited) {
        ipclog_warn("%s not inited \n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (!rcm->started) {
        ipclog_warn("%s not started\n", __FUNCTION__);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    }

    if (rcm->attr.videoStream == stream) {
        ipclog_warn("%s stream:%d already \n", __FUNCTION__, stream);
        pthread_mutex_unlock(&rcm->mutex);
        return 0;
    } 

    //TODO: check stream 
    //assert((stream == MEDIA_REC_STREAM_HIGH) || (stream == MEDIA_REC_STREAM_MID));

    RECORD_ATTR_T attr;
    memcpy(&attr, &rcm->attr, sizeof(RECORD_ATTR_T));
    attr.videoStream = stream;
    recManagerSetAttribute_l(&attr);
    
    pthread_mutex_unlock(&rcm->mutex);
    return 0;
}





