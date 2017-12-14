#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>

#include <qsdk/sys.h>
#include <qsdk/videobox.h>
#include <system_msg.h>
#include "q3_wireless.h"
#include "q3_gpio.h"
//#include "tlmp4drv.h"
#include "tlmisc.h"
#include <qm-hal/libQMHAL.h>

#include "ipc.h"
#include "motion_controller.h"
#include "encode_controller.h"

#define LOGTAG  "IRCUT_CTRL"

#define IRCUT_SWITCH_TIME    2
enum {
    IRCUT_MODE_AUTO = 0,
    IRCUT_MODE_NIGHT = 1,
    IRCUT_MODE_DAY = 2,
};

enum {
    IRCUT_STATE_DAY = 0,
    IRCUT_STATE_NIGHT = 1,
    IRCUT_STATE_DARK = 2,
    IRCUT_STATE_LOWLUX = 3,
};

#define CRCM_DAY_SWITCH_TIMES    2
#define CRCM_NIGHT_SWITCH_TIMES  2

#define CRCM_DARK_SWITCH_TIMES   6
#define CRCM_DARKRECOVERY_SWITCH_TIMES  8

#define CRCM_LOWLUX_SWITCH_TIMES   6
#define CRCM_LOWLUXRECOVERY_SWITCH_TIMES  8

#define CRCM_BITRATE_OVER_SWITCH_TIMES   6
#define CRCM_BITRATE_RECOVERY_SWITCH_TIMES  12

typedef struct {
    int mode;
    int state;

    int crcmDayCount;
    int crcmNightCount;
    int crcmDarkCount;
    int crcmDarkRecoveryCount;

    int crcmLowluxCount;
    int crcmLowluxRecoveryCount;

    int crcmBitrateOverCount;
    int crcmBitrateRecoveryCount;
} IRCUT_CONTROLLER_T;

static IRCUT_CONTROLLER_T sController = {
    .mode = IRCUT_MODE_AUTO,
    .state = IRCUT_STATE_DAY
};

static pthread_mutex_t g_IRCutLock = PTHREAD_MUTEX_INITIALIZER;
//static int g_IRCutMode = 0;
//static int g_IRCutState = 0;     //0----白天,1----黑夜
//static int g_LightSensorLowCnt = 0, g_LightSensorHightCnt = 0;

extern int qmapi_alarm_run;
extern int qmapi_alarm_count;

static int ircut_controller_set_mode(IRCUT_CONTROLLER_T *controller, int mode);

void IRcut_init()
{
    pthread_mutex_lock(&g_IRCutLock);
    QMHAL_IRCUT_SetState(QMHAL_IRCUT_STATE_DAY);
    QMHAL_RLight_SetState(QMHAL_LIGHT_STATE_OFF);
    pthread_mutex_unlock(&g_IRCutLock);
    //g_IRCutState = 0;
}

void IRcut_reset()
{
    pthread_mutex_lock(&g_IRCutLock);
    QMHAL_IRCUT_SetState(QMHAL_IRCUT_STATE_DAY);
    QMHAL_RLight_SetState(QMHAL_LIGHT_STATE_OFF);
    pthread_mutex_unlock(&g_IRCutLock);
    //g_IRCutState = 0;
}

void IRcut_close()
{
    pthread_mutex_lock(&g_IRCutLock);
    QMHAL_IRCUT_SetState(QMHAL_IRCUT_STATE_DAY);
    QMHAL_RLight_SetState(QMHAL_LIGHT_STATE_OFF);
    pthread_mutex_unlock(&g_IRCutLock);
    //g_IRCutState = 0;
}

void IRcut_open()
{
    pthread_mutex_lock(&g_IRCutLock);
    QMHAL_IRCUT_SetState(QMHAL_IRCUT_STATE_NIGHT);
    QMHAL_RLight_SetState(QMHAL_LIGHT_STATE_ON);
    pthread_mutex_unlock(&g_IRCutLock);
    //g_IRCutState = 1;
}

//0---auto  1---night  2----day
int DMS_DEV_GPIO_SetIRCutState(int nDay)
{
	//g_IRCutMode = nDay;
    ircut_controller_set_mode(&sController, nDay);
    return 0;
}

int DMS_DEV_GPIO_GetResetState(DWORD *lpdwState)
{
	QMHAL_GPIO_LEVEL_TYPE_E nLevel = 0;
	
    *lpdwState = 0;
    nLevel = QMHAL_Reset_GetState();
    if(nLevel == QMHAL_GPIO_LEVEL_LOW)
        *lpdwState = 1;
	return (int)*lpdwState;
}

int DMS_DEV_GPIO_GetLightSensorState()
{
    int nRes = 0;

    nRes = QMHAL_LDR_GetState();
    return nRes;
}

static int camera_set_day(void)
{
    motion_controller_blind(2000);
    IRcut_close();
    camera_set_scenario("isp", 0);
    camera_monochrome_enable("isp", 0);
    encode_controller_set_night(MODE_DAY);
    
    return 0;
}

static int camera_set_night(void)
{
    motion_controller_blind(2000);
    camera_monochrome_enable("isp", 1);
    camera_set_scenario("isp", 1);
    IRcut_open();
    encode_controller_set_night(MODE_NIGHT);

    return 0;
}

static int camera_set_dark(void)
{
    motion_controller_blind(2000);
    encode_controller_set_night(MODE_DARK);
    
    return 0;
}


static int camera_set_lowlux(void)
{
    motion_controller_blind(2000);
    encode_controller_set_night(MODE_LOWLUX);
    
    return 0;
}

static int camera_set_dark_recovery(void)
{
    //motion_controller_blind(2000);
    //encode_controller_set_dark(MODE_DAY);
    
    return 0;
}

static int ircut_controller_set_day(IRCUT_CONTROLLER_T *controller)
{
    int last_state = controller->state;
    camera_set_day();
    controller->state = IRCUT_STATE_DAY;
    ipclog_info("%s, mode:%d state:%d-->%d(day)\n",__FUNCTION__, 
            controller->mode, last_state, controller->state);

    return 0;
}

static int ircut_controller_set_night(IRCUT_CONTROLLER_T *controller)
{
    int last_state = controller->state;
    camera_set_night();
    controller->state = IRCUT_STATE_NIGHT;
    ipclog_info("%s, mode:%d state:%d-->%d(night)\n",__FUNCTION__, 
            controller->mode, last_state, controller->state);
    return 0;
}

static int ircut_controller_set_dark(IRCUT_CONTROLLER_T *controller)
{
    int last_state = controller->state;
    camera_set_dark();
    controller->state = IRCUT_STATE_DARK;
    ipclog_info("%s, mode:%d state:%d-->%d(dark)\n",__FUNCTION__, 
            controller->mode, last_state, controller->state);
    return 0;
}

static int ircut_controller_set_lowlux(IRCUT_CONTROLLER_T *controller)
{
    int last_state = controller->state;
    camera_set_lowlux();
    controller->state = IRCUT_STATE_LOWLUX;
    ipclog_info("%s, mode:%d state:%d-->%d(lowlux)\n",__FUNCTION__, 
            controller->mode, last_state, controller->state);
    return 0;
}



static int ircut_controller_set_dark_recovery(IRCUT_CONTROLLER_T *controller)
{
    int last_state = controller->state;
    camera_set_dark_recovery();
    //controller->state = IRCUT_STATE_DAY;
    ipclog_info("%s, mode:%d state:%d-->%d(dark recovery)\n",__FUNCTION__, 
            controller->mode, last_state, controller->state);
 
    return 0;
}

static int ircut_controller_set_mode(IRCUT_CONTROLLER_T *controller, int mode)
{
    int last_mode = controller->mode;
    if (controller->mode != mode) {
        controller->mode = mode;
        ipclog_info("%s, mode:%d-->%d\n",__FUNCTION__, last_mode, controller->mode);
    }

    return 0;
}

static int crcm_state_check(IRCUT_CONTROLLER_T *controller)
{   
    int last_state = controller->state;
    int state = controller->state;

    int ret = DMS_DEV_GPIO_GetLightSensorState();
    if (ret == QMHAL_LDR_LEVEL_HIGH) {
        controller->crcmNightCount++;
        controller->crcmDayCount = 0;
    } else if( ret == QMHAL_LDR_LEVEL_LOW) {
        controller->crcmNightCount = 0;
        controller->crcmDayCount++;
    }

    if (controller->crcmNightCount > CRCM_DAY_SWITCH_TIMES) {
        state = IRCUT_STATE_NIGHT;        
    } else if(controller->crcmDayCount > CRCM_NIGHT_SWITCH_TIMES) {
        state = IRCUT_STATE_DAY;     
    }

#if 0
    ipclog_debug("%s nightCount:%d dayCount:%d state:%d-->%d\n", 
            __FUNCTION__, 
            controller->crcmNightCount, controller->crcmDayCount,
            last_state, state);
#endif

#if 1
    //check state only when it is not ngith mode 
    if (state != IRCUT_STATE_NIGHT) {
        int out_day_mode = 0;
        int bitrate_state = 0;
        camera_get_day_mode("isp", &out_day_mode);
        bitrate_state = encode_controller_check_bitrate(0);

        //LOWLUX check
        if ((out_day_mode == 0)) {
            controller->crcmLowluxCount++;
            controller->crcmLowluxRecoveryCount = 0;
        } else if ((last_state == IRCUT_STATE_LOWLUX) && (out_day_mode == 1) ) {
            controller->crcmLowluxCount = 0;
            controller->crcmLowluxRecoveryCount++;
        } else {
            controller->crcmLowluxCount = 0;
            controller->crcmLowluxRecoveryCount = 0;
        }

        if ((controller->crcmLowluxCount > CRCM_LOWLUX_SWITCH_TIMES) && (last_state == IRCUT_STATE_DAY)) {
            state = IRCUT_STATE_LOWLUX;        
        } else if (controller->crcmLowluxRecoveryCount > CRCM_LOWLUXRECOVERY_SWITCH_TIMES) {
            state = IRCUT_STATE_DAY;     
        } else if (last_state == IRCUT_STATE_LOWLUX) {
            //NOTE: keep in lowlux state
            state = IRCUT_STATE_LOWLUX;
        }
#if 0
        ipclog_debug("%s day_mode:%d lowLuxsCount:%d lowLuxsRecovery:%d state:%d-->%d\n", 
                __FUNCTION__, out_day_mode, controller->crcmLowluxCount, 
                controller->crcmLowluxRecoveryCount,
                last_state, state);
#endif

#if 0
        //DARK check, will overwrite lowlux 
        if ((out_day_mode == 0) && (bitrate_state >= ENCODE_BITRATE_HIGH)) {
            controller->crcmDarkCount++;
            controller->crcmDarkRecoveryCount = 0;
        } else if ((last_state == IRCUT_STATE_DARK) 
                && (out_day_mode == 1) && (bitrate_state <= ENCODE_BITRATE_NORMAL)) {
            //NOTE:dark recovery count only when state is DARK
            controller->crcmDarkCount = 0;
            controller->crcmDarkRecoveryCount++;
        } else {
            controller->crcmDarkCount = 0;
            controller->crcmDarkRecoveryCount = 0;
        }

        if (controller->crcmDarkCount > CRCM_DARK_SWITCH_TIMES) {
            state = IRCUT_STATE_DARK;        
        } else if (controller->crcmDarkRecoveryCount > CRCM_DARKRECOVERY_SWITCH_TIMES) {
            state = IRCUT_STATE_DAY;     
        } else if (last_state == IRCUT_STATE_DARK) {
            //NOTE: keep in dark state
            state = IRCUT_STATE_DARK;
        }
#if 0
        ipclog_debug("%s day_mode:%d bitrate_state:%d darkCount:%d darkRecovery:%d state:%d-->%d\n", 
                __FUNCTION__, out_day_mode, bitrate_state, 
              controller->crcmDarkCount, controller->crcmDarkRecoveryCount,
              last_state, state);
#endif
#endif
    }
#endif

#if 1
    int bitrate_state = 0;
    bitrate_state = encode_controller_check_bitrate(0);
    int last_bitrate_control = encode_controller_check_control();
    int current_bitrate_control = last_bitrate_control;

    if ((last_bitrate_control != ENCODE_CONTROLLER_CONTROL) 
            && (bitrate_state >= ENCODE_BITRATE_HIGH)) {
        controller->crcmBitrateOverCount++;
        controller->crcmBitrateRecoveryCount = 0;
    } else if ((last_bitrate_control != ENCODE_CONTROLLER_NONE
                && (bitrate_state <= ENCODE_BITRATE_NORMAL))) {
        controller->crcmBitrateOverCount = 0;
        controller->crcmBitrateRecoveryCount++;
    } else {
        controller->crcmBitrateOverCount = 0;
        controller->crcmBitrateRecoveryCount = 0;
    }

    if (controller->crcmBitrateOverCount > CRCM_BITRATE_OVER_SWITCH_TIMES) {
        current_bitrate_control = ENCODE_CONTROLLER_CONTROL;        
    } else if (controller->crcmBitrateRecoveryCount > CRCM_BITRATE_RECOVERY_SWITCH_TIMES) {
        if (last_bitrate_control == ENCODE_CONTROLLER_RECOVERY) {
            current_bitrate_control = ENCODE_CONTROLLER_NONE;     
        } else {
            current_bitrate_control = ENCODE_CONTROLLER_RECOVERY;     
        }
    }

    //ipclog_debug("%s bitrate_state:%d BitrateOverCount:%d BitrateRecovery:%d control:%d-->%d\n", 
    //        __FUNCTION__, bitrate_state, 
    //        controller->crcmBitrateOverCount, controller->crcmBitrateRecoveryCount,
    //        last_bitrate_control, current_bitrate_control);

    if (last_bitrate_control != current_bitrate_control) {
        controller->crcmBitrateOverCount = 0;
        controller->crcmBitrateRecoveryCount = 0;
        encode_controller_set_control(current_bitrate_control);
    }
#endif

    return state;
}

static void camera_mode_controller(IRCUT_CONTROLLER_T *controller)
{
    int new_state = controller->state;
    if (controller->mode == IRCUT_MODE_AUTO) {
        int  crcm_state = crcm_state_check(controller);
        new_state = crcm_state;
    } else {
        if ((controller->mode == IRCUT_MODE_DAY) && (controller->state != IRCUT_STATE_DAY)) {
            new_state = IRCUT_STATE_DAY;
        } else if ((controller->mode == IRCUT_MODE_NIGHT) 
                && (controller->state != IRCUT_STATE_NIGHT)) {    
            new_state = IRCUT_STATE_NIGHT;
        }
    }

    if (new_state != controller->state) {
        //TODO: any good way
        if (controller->state == IRCUT_STATE_DARK) {
            ircut_controller_set_dark_recovery(controller);
        }

        switch(new_state) {
            case IRCUT_STATE_DAY:
                ircut_controller_set_day(controller);
                break;

            case IRCUT_STATE_LOWLUX:
                ircut_controller_set_lowlux(controller);
                break;

            case IRCUT_STATE_DARK:
                ircut_controller_set_dark(controller);
                break;
            case IRCUT_STATE_NIGHT:
                ircut_controller_set_night(controller);
                break;

            default:
                break;
        }
    }

    return;
}

static int camera_mode_controller_init(IRCUT_CONTROLLER_T *controller)
{
    IRcut_init();
    camera_monochrome_enable("isp", 0);
    camera_set_scenario("isp", 0);
    encode_controller_set_night(MODE_DAY);
 
    return 0;
}

void detectIOThread(void)
{
    DWORD reset = 0;
    int reset_second = 0;
    unsigned char  select_count = 0;
    //int cnt = 0,ircutopen = 0;

    prctl(PR_SET_NAME, (unsigned long)"detectIOThread", 0,0,0);
    pthread_detach(pthread_self());
    
    camera_mode_controller_init(&sController);

    while(qmapi_alarm_run)
    {
        if ((select_count % 10) == 0)
        {
	        DMS_DEV_GPIO_GetResetState(&reset);
	        if(reset)
	        {
	            ++reset_second;
	            printf("%s  %d, Reset button is pressed,reset_second:%d\n",__FUNCTION__, __LINE__,reset_second);
	            if(reset_second>10)
	            {
	            	// need fixme yi.zhang
	                //tl_set_default_param(fFlagDefault);
	                tl_set_default_param(180);
	                /* RM#3541: clean wifi config (forget the connected info) when reset device.    henry.li    2017/05/15 */
	                QMAPI_Wireless_Stop();
	                //TL_SysFun_Play_Audio(QMAPINOTIFY_TYPE_RESTORE_FACTORY_SETTING, 1);
	                SystemCall_msg("reboot", SYSTEM_MSG_BLOCK_RUN);
	                // need fixme yi.zhang
	                //restartSystem();
	                return;
	            }
	        }
	        else
	        {
	            if(reset_second>=3 && reset_second<=5)
	            {
	                printf("%s  %d, start sonic matching....\n",__FUNCTION__, __LINE__);
	                //q3_wireless_start_swave();
	            }
	            else if(reset_second>5 && reset_second<=10)
	            {
	                printf("%s  %d, wifi switch to AP mode....\n",__FUNCTION__, __LINE__);
	                //QMAPI_Wireless_SwitchMode(1);
	            }
	            reset_second = 0;
	        }

            camera_mode_controller(&sController);
        }
        select_count ++;
        usleep(100*1000);
    }
    qmapi_alarm_count --;

    return ;
}

int DMS_DEV_GPIO_SetWifiLed(LED_STATE_E eState)
{
    int nRes = 0;
    switch(eState)
    {
        //常亮、关闭搞反了
        case LED_STATE_OFF:
            nRes = QMHAL_SystemLed_SetState(QMHAL_LIGHT_STATE_OFF);
            break;
        case LED_STATE_ON:
            nRes = QMHAL_SystemLed_SetState(QMHAL_LIGHT_STATE_ON);
            break;
        case LED_STATE_SLOW_BLINK:
            nRes = QMHAL_SystemLed_SetBlink(1000, 1000);
            break;
        case LED_STATE_FAST_BLINK:
            nRes = QMHAL_SystemLed_SetBlink(200, 200);
            break;
        default:
            return -1;
    }
    if(nRes)
        printf("%s  %d, set wifi state led failed!!\n",__FUNCTION__, __LINE__);
    return 0;
}

int DMS_DEV_GPIO_SetSpeakerOnOff(int nOn)
{
    return QMHAL_SPEAKER_Enable(nOn);
}


int DMS_DEV_GPIO_Init_Ext(int IRcutMode)
{
    if(IRcutMode<0 || IRcutMode>2)
    {
        printf("%s  %d, Invalid IRcutMode:%d\n",__FUNCTION__, __LINE__,IRcutMode);
        return -1;
    }

    ircut_controller_set_mode(&sController, IRcutMode);
    //g_IRCutMode = IRcutMode;
    QMHAL_GPIO_Init();
    DMS_DEV_GPIO_SetWifiLed(LED_STATE_OFF);

    return 0;
}


