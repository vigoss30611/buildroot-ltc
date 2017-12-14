#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>

#include "spv_common.h"
#include "spv_debug.h"

#include "spv_adas.h"

static int g_AdasDetectionState;
static pthread_t g_DetectionThread;

int GetAdasDetectionState()
{
    return g_AdasDetectionState;
}

int ConfigAdasDetection(ADAS_LDWS_CONFIG *ldConfig, ADAS_FCWS_CONFIG *fcConfig)
{
    INFO("ConfigLDDetection begin\n");
    ADAS_GUI_MSG_INFO info;
	int msgId = 0, ret = 0;
    info.adas_gui_msg_from = GUI_PORT;
    info.adas_gui_msg_to = ADAS_PORT;
	msgId = msgget(ADAS_GUI_BINDER_KEY, 0);
	if(msgId < 0) {
		msgId = msgget(ADAS_GUI_BINDER_KEY, IPC_EXCL | IPC_CREAT | 0660);
		
		if(msgId < 0) {
            ERROR("init ADAS_GUI_BINDER_KEY failed\n");
			return -1;
		}

	}

    if(ldConfig != NULL) {
        memcpy(&(info.adas_ldws_config), ldConfig, sizeof(ADAS_LDWS_CONFIG));
        info.adas_gui_command = ADAS_GUI_CMD_LDWS_CONFIG;
        ret = msgsnd(msgId, &info, ADAS_GUI_MSG_SIZE, 0);
        if(ret == -1) {
            ERROR("config ldws error, %s\n", strerror(errno));
        }
    }
    
    if(fcConfig != NULL) {
        memcpy(&(info.adas_fcws_config), fcConfig, sizeof(ADAS_FCWS_CONFIG));
        info.adas_gui_command = ADAS_GUI_CMD_FCWS_CONFIG;
        ret = msgsnd(msgId, &info, ADAS_GUI_MSG_SIZE, 0);
        if(ret == -1) {
            ERROR("config fcws error, %s\n", strerror(errno));
        }
    }

    INFO("ConfigLDDetection end\n");
    return ret;
}

static void AdasDetectionThread()
{
    static ADAS_PROC_RESULT sResult;
    ADAS_GUI_MSG_INFO info;
	int msgId = -1, ret = 0;

	msgId = msgget(ADAS_GUI_BINDER_KEY, 0);
	if(msgId < 0) {
		msgId = msgget(ADAS_GUI_BINDER_KEY, IPC_EXCL | IPC_CREAT | 0666);
		
		if(msgId < 0) {
            ERROR("init ADAS_GUI_BINDER_KEY failed, %s\n", strerror(errno));
            g_AdasDetectionState = ADAS_DETECTION_DEINIT;
			return;
		}

	}

    while(1) {
        pthread_testcancel();
        if(g_AdasDetectionState == ADAS_DETECTION_DEINIT)
            break;

        memset(&info, 0, sizeof(ADAS_GUI_MSG_INFO));
        info.adas_gui_msg_to = GUI_PORT;
        ret = msgrcv(msgId, (void *)&info, ADAS_GUI_MSG_SIZE, info.adas_gui_msg_to, 0);
        if(g_AdasDetectionState != ADAS_DETECTION_RUNNING) {//here clean the msgqueue
            //usleep(100000);
            continue;
        }

        if(ret == -1){
            ERROR("adas_gui_msgqueue receive error:%s\n", strerror(errno));
            if(errno == EIDRM) {
                //do exit actions
                break;
            } else {
                continue;
            }
        }

        HWND mainWnd = GetMainWindow();
        if(!IsMainWindow(mainWnd)) {
            INFO("main window does not exist, retry\n");
            continue;
        }

        switch(info.adas_gui_command) {
#if 0
            case ADAS_GUI_CMD_LDWS_RESULT:
                INFO("ADAS_GUI_CMD_LDWS_RESULT, [(%d, %d)--(%d, %d)], [(%d, %d)--(%d, %d)], mark: %d, state:%d\n", info.adas_ldws_result.left_lane_x1, info.adas_ldws_result.left_lane_y1, info.adas_ldws_result.left_lane_x2, info.adas_ldws_result.left_lane_y2, info.adas_ldws_result.right_lane_x1, info.adas_ldws_result.right_lane_y1, info.adas_ldws_result.right_lane_x2, info.adas_ldws_result.right_lane_y2, info.adas_ldws_result.ldws_lanemark, info.adas_ldws_result.veh_ldws_state);
                SendMessage(mainWnd, MSG_LDWS, 0, (long)&(info.adas_ldws_result));
                break;
            case ADAS_GUI_CMD_FCWS_RESULT:
                INFO("ADAS_GUI_CMD_FCWS_RESULT, num: %d, target_info: 0x%x\n", info.adas_fcws_result.target_num, (unsigned int)(info.adas_fcws_result.target_info));
                SendMessage(mainWnd, MSG_FCWS, 0, (long)&(info.adas_fcws_result));
                break;
#else
            case ADAS_GUI_CMD_ADAS_RESULT:
                if(!memcmp(&sResult, &(info.adas_proc_result), sizeof(ADAS_PROC_RESULT)))
                    break;
                memcpy(&sResult, &(info.adas_proc_result), sizeof(ADAS_PROC_RESULT));
                SendMessage(mainWnd, MSG_ADAS, 0, (long)&(info.adas_proc_result));
                break;
#endif
        }
    }

}

int StartAdasDetection()
{
    ADAS_GUI_MSG_INFO info;
	int msgId = 0, ret = 0;

	msgId = msgget(ADAS_GUI_BINDER_KEY, 0);
	if(msgId < 0) {
		msgId = msgget(ADAS_GUI_BINDER_KEY, IPC_EXCL | IPC_CREAT | 0660);
		
		if(msgId < 0) {
            ERROR("init ADAS_GUI_BINDER_KEY failed\n");
            g_AdasDetectionState = ADAS_DETECTION_DEINIT;
			return;
		}

    }
    INFO("StartAdasDetection\n");

    info.adas_gui_msg_from = GUI_PORT;
    info.adas_gui_msg_to = ADAS_PORT;

    info.adas_gui_command = ADAS_GUI_CMD_LDWS_START;
    ret = msgsnd(msgId, &info, ADAS_GUI_MSG_SIZE, IPC_NOWAIT);
    if(ret == -1) {
        ERROR("start ldws error, %s\n", strerror(errno));
        //return ret;
    }

    info.adas_gui_command = ADAS_GUI_CMD_FCWS_START;
    ret = msgsnd(msgId, &info, ADAS_GUI_MSG_SIZE, IPC_NOWAIT);
    if(ret == -1) {
        ERROR("start ldws error, %s\n", strerror(errno));
        //return ret;
    }

    INFO("StartAdasDetection end\n");
    g_AdasDetectionState = ADAS_DETECTION_RUNNING;
    return 0;
}

int StopAdasDetection()
{
    ADAS_GUI_MSG_INFO info;
	int msgId = 0, ret = -1;

	msgId = msgget(ADAS_GUI_BINDER_KEY, 0);
	if(msgId < 0) {
		msgId = msgget(ADAS_GUI_BINDER_KEY, IPC_EXCL | IPC_CREAT | 0660);
		
		if(msgId < 0) {
            ERROR("init ADAS_GUI_BINDER_KEY failed\n");
            g_AdasDetectionState = ADAS_DETECTION_DEINIT;
			return -1;
		}

    }

    info.adas_gui_msg_from = GUI_PORT;
    info.adas_gui_msg_to = ADAS_PORT;

    info.adas_gui_command = ADAS_GUI_CMD_LDWS_STOP;
    ret = msgsnd(msgId, &info, ADAS_GUI_MSG_SIZE, 0);
    if(ret == -1) {
        ERROR("stop ldws error, %s\n", strerror(errno));
        return ret;
    }

    info.adas_gui_command = ADAS_GUI_CMD_FCWS_STOP;
    ret = msgsnd(msgId, &info, ADAS_GUI_MSG_SIZE, 0);
    if(ret == -1) {
        ERROR("stop ldws error, %s\n", strerror(errno));
        return ret;
    }

    g_AdasDetectionState = ADAS_DETECTION_STOPPED;
    return 0;
}

int InitAdasDetection()
{
    g_AdasDetectionState = ADAS_DETECTION_INIT;
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    INFO("pthread_create, init LCWS FDWS detect\n");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&g_DetectionThread, &attr, (void *)AdasDetectionThread, NULL);
    pthread_attr_destroy (&attr);
    return 0;
}

int DeinitAdasDetection()
{
    g_AdasDetectionState = ADAS_DETECTION_DEINIT;
    pthread_cancel(g_DetectionThread);
    return 0;
}

