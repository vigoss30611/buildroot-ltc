#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/prctl.h>

#include "spv_app.h"
#include "spv_stream.h"
#include "spv_config_keys.h"
#include "spv_wifi.h"
#include "spv_input.h"
#include "spv_event.h"
#include "spv_debug.h"
#include "spv_utils.h"
#include "spv_server.h"
#include "spv_config.h"
#include "spv_common.h"
#include "spv_message.h"
#include "spv_properties.h"
#include "config_manager.h"
#include "spv_language_res.h"
#include "spv_file_manager.h"
#include "spv_file_receiver.h"
#include "spv_indication.h"

#include "gps.h"
#include "camera_spv.h"

#include "file_dir.h"

#define FLAG_POWERED_BY_GSENSOR	8424
#define FLAG_POWERED_BY_OTHER	8425

static camera_spv* g_camera;
static wifi_p g_wifi;
static HMSG g_launcher_handler;
PSleepStatus g_status = NULL;
static BOOL g_recompute_locations = FALSE;
static BOOL g_sync_gps_time;
int g_fucking_volume = 0;
int CAMERA_REAR_CONNECTED = 1;
int REVERSE_IMAGE = 0;


static SPVSTATE *g_spv_camera_state = NULL;
const char* SPV_WAV[] = {
    RESOURCE_PATH"res/booting.wav",
    RESOURCE_PATH"res/error.wav",
    RESOURCE_PATH"res/key.wav",
    RESOURCE_PATH"res/photo.wav",
    RESOURCE_PATH"res/photo3.wav",
    RESOURCE_PATH"res/warning.wav",
    RESOURCE_PATH"res/warning.wav",
};

static SpvClient g_spv_client[TYPE_FROM_COUNT] = {{0, NULL}, {0, NULL}};

int IsMainWindowActive();

int GetLauncherHandler()
{
	return g_launcher_handler;
}

PSleepStatus GetSleepStatus()
{
    return g_status;
}

int SpvIsInVideoMode()
{
    if(g_spv_camera_state != NULL) {
        return g_spv_camera_state->mode == MODE_VIDEO;
    }
    return 0;
}

BOOL IsKeyBelongsCurrentMode(const char *key)	//the cunrrent mode just might be MODE_VIDEO
{
	if(key == NULL)
		return FALSE;


    switch(g_spv_camera_state->mode) {
        case MODE_VIDEO:
            if(!strncasecmp("video.", key, 6))
                return TRUE;
            break;
        case MODE_PHOTO:
            if(!strncasecmp("photo.", key, 6))
                return TRUE;
            break;
        case MODE_SETUP:
            if(!strncasecmp("setup.", key, 6))
                return TRUE;
            break;
    }    

	if(strstr(key, "irled") || strstr(key, "imagerotation"))
		return TRUE;

	return FALSE;
}

/************************* functions for devices actions ********************/
static void SpvSetCollision(camera_spv *camera) {
	if(camera == NULL)
		return;
	INFO("SpvSetCollision\n");
	g_spv_camera_state->isLocked = TRUE;
	g_recompute_locations = TRUE;

	config_camera config;
	GetCameraConfig(&config);
	strcpy(config.other_collide, "On");
	camera->set_config(config);
}

void SetKeyTone(char *newvalue)
{
	char value[VALUE_SIZE] = {0};
    if(newvalue != NULL) {
        strcpy(value, newvalue);
    } else {
	    GetConfigValue(GETKEY(ID_SETUP_KEY_TONE), value);
    }
	g_spv_camera_state->isKeyTone = !strcasecmp(value, GETVALUE(STRING_ON));
}

int SpvKeyToneEnabled()
{
    return g_spv_camera_state->isKeyTone;
}

void SetAutoSleepStatus(char *newvalue)
{
	char value[VALUE_SIZE] = {0};
    if(newvalue != NULL) {
        strcpy(value, newvalue);
    } else {
	    GetConfigValue(GETKEY(ID_SETUP_AUTO_SLEEP), value);
    }
	if(atoi(value))
		g_status->autoSleep = atoi(value);
	else
		g_status->autoSleep = -1;
}

void SetDelayedShutdwonStatus(char *newvalue)
{
	char value[VALUE_SIZE] = {0};
    if(newvalue != NULL) {
        strcpy(value, newvalue);
    } else {
	    GetConfigValue(GETKEY(ID_SETUP_DELAYED_SHUTDOWN), value);
    }
	if(atoi(value))
		g_status->delayedShutdown = atoi(value);
	else
		g_status->delayedShutdown = -1;
}

void SetAutoPowerOffStatus(char *newvalue)
{
	char value[VALUE_SIZE] = {0};
    if(newvalue != NULL) {
        strcpy(value, newvalue);
    } else {
	    GetConfigValue(GETKEY(ID_SETUP_AUTO_POWER_OFF), value);
    }
	if(atoi(value))
		g_status->autoPowerOff = atoi(value) * 60;
	else
		g_status->autoPowerOff = -1;
}

/************************* functions for sys-time ***************************/
static int SetTimeByData(gps_utc_time timedata)
{
	static time_t rawtime;
	static struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_year = timedata.year - 1900;
	timeinfo->tm_mon = timedata.month - 1;
	timeinfo->tm_mday = timedata.day;
	timeinfo->tm_hour = timedata.hour;
	timeinfo->tm_min = timedata.minute;
	timeinfo->tm_sec = timedata.second;

	rawtime = mktime(timeinfo);

	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tv.tv_sec = rawtime + 8 * 60 * 60;
	tv.tv_usec = 0;
	int result = SetTimeToSystem(tv, tz);

	return result;
}

static void InitSystemTime()
{
	static time_t rawtime;
	static struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	if((1900+timeinfo->tm_year < 2016) || (1900+timeinfo->tm_year > 2025)) {
        INFO("system time is reset, we need to change to 2016/01/01\n");
		struct timeval currentTime;
		gettimeofday(&currentTime, NULL);
		gps_utc_time time = {2016, 1, 1, 0, 0, 0};
		SetTimeByData(time);
	}
}

/************************* functions for sdcard *****************************/
BOOL IsSdcardFull()
{
	if(IsSdcardMounted()) {
		if(g_spv_camera_state->availableSpace < SDCARD_FREE_SPACE_NOT_ENOUGH_VALUE * 1024) {
			INFO("sdcard is full!, availableSpace: %ldKB\n", g_spv_camera_state->availableSpace);
			return TRUE;
		}
	}
	return FALSE;
}

int SpvUpdateSDcardSpace(int space)
{
    if(g_spv_camera_state == NULL)
        return -1;

    g_spv_camera_state->availableSpace = space;
    return 0;
}

int IsSdcardMounted()
{
    if(g_spv_camera_state == NULL)
        return 0;

	return g_spv_camera_state->sdStatus == SD_STATUS_MOUNTED;
}

int SpvIsOnLine()
{
    if(g_spv_camera_state == NULL)
        return 0;

	return g_spv_camera_state->online;
}

int SpvGetChargingStatus()
{
    if(g_spv_camera_state == NULL)
        return 0;

    return g_spv_camera_state->charging;
}

static int UpdateCapacityStatus()
{
	char capacity[128] = {0};
	int videoCount = 0, picCount = 0;
	g_spv_camera_state->availableSpace = SpvGetAvailableSpace();
	GetMediaFileCount(&videoCount, &picCount);
#ifdef GUI_NETWORK_ENABLE
    int timeRemain;
    int countRemain;
    char value[VALUE_SIZE] = {0};
     GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), value);
     timeRemain = GetVideoTimeRemain(g_spv_camera_state->availableSpace, value);
	 GetConfigValue(GETKEY(ID_PHOTO_RESOLUTION), value);
	 countRemain = GetPictureRemain(g_spv_camera_state->availableSpace, value);
	 sprintf(capacity, "%ld/%d/%d/%d/%d", g_spv_camera_state->availableSpace, videoCount , picCount,timeRemain,countRemain);
     SetConfigValue(g_status_keys[STATUS_CAPACITY], capacity, TYPE_FROM_DEVICE);
#endif
	return 0;
}

static int UpdateWorkingStatus()
{
#ifdef GUI_NETWORK_ENABLE
    SetConfigValue(g_status_keys[STATUS_WORKING], 
            (g_spv_camera_state->isRecording || g_spv_camera_state->isPicturing) ? 
            GETVALUE(STRING_ON) : GETVALUE(STRING_OFF), TYPE_FROM_DEVICE);
#endif
    return 0;
}

static int UpdateBatteryStatus()
{//(0-7)(low, medium, high, full, low-charging, medium-charging, high-charging, full-charing)
#ifdef GUI_NETWORK_ENABLE
    int l;
    char level[16] = {0};
    if(g_spv_camera_state->charging)
    {
        l = g_spv_camera_state->battery / 26 + 4;
    } else {
        l = g_spv_camera_state->battery / 26;
    }
    sprintf(level, "%d", l);
    SetConfigValue(g_status_keys[STATUS_BATTERY], level, TYPE_FROM_DEVICE);
#endif
    return 0;
}

/************************* functions for files ******************************/
static void UpdatePhotoCount(unsigned int photoCount)
{
	g_spv_camera_state->photoCount = photoCount;
}

/************************* functions for camera *****************************/
static int CameraInfoCallback(int flag)
{
	SpvPostMessage(g_launcher_handler, MSG_CAMERA_CALLBACK, flag, 0);
	return 0;
}

static BOOL CameraConfigIsSame(config_camera config1, config_camera config2)
{
	return memcmp(&config1, &config2, sizeof(config_camera)) == 0;
}

static void UpdateRearCameraStatus(int devInserted)
{
#ifdef GUI_CAMERA_REAR_ENABLE
    g_spv_camera_state->rearCamInserted = devInserted;
    CAMERA_REAR_CONNECTED = g_spv_camera_state->rearCamInserted; 
#ifdef GUI_NETWORK_ENABLE
    SetConfigValue(g_status_keys[STATUS_REAR], 
            (g_spv_camera_state->rearCamInserted) ? 
            GETVALUE(STRING_ON) : GETVALUE(STRING_OFF), TYPE_FROM_DEVICE);
#endif
#endif
}

BOOL SetCameraConfig(BOOL directly)
{
    if(g_spv_camera_state->calibration == 1)
        return 1;

	static config_camera sCurrentConfig;
	config_camera config;
	memset(&config, 0, sizeof(config_camera));
	GetCameraConfig(&config);
	if(CameraConfigIsSame(sCurrentConfig, config)) {
		return FALSE;
	}
	memcpy(&sCurrentConfig, &config, sizeof(config_camera));
	if(directly)
		g_camera->stop_preview();
	g_camera->set_config(config);

	if(!directly)
		g_camera->start_preview();

	return TRUE;
}

static int StartVideoRecord()
{
    if(g_spv_camera_state->calibration == 1)
        return 0;
	SetCameraConfig(FALSE);
	INFO("------StartVideoRecord------\n");
	if(!g_camera->start_video()) {
        SpvIndication(IND_VIDEO_START, 0, 0);
		g_status->idle = 0;
		g_spv_camera_state->isRecording = TRUE;
		g_spv_camera_state->videoCount += 1;
		gettimeofday(&g_spv_camera_state->beginTime, 0);
		g_spv_camera_state->timeLapse = 0;
        //SpvSetLed(LED_WORK, 500, 500, 1);
		//SetTimer(hWnd, TIMER_CLOCK, 10);
	} else {
		return ERROR_CAMERA_BUSY;
	}
	UpdateWorkingStatus();
	return 0;
}

static int StopVideoRecord() 
{
    if(g_spv_camera_state->calibration == 1)
        return 0;
	if(!g_spv_camera_state->isRecording)
		return 0;
	INFO("------StopVideoRecord------\n");
	int ret = g_camera->stop_video();
	UpdateCapacityStatus();
	g_status->idle = 1;
	clock_gettime(CLOCK_MONOTONIC_RAW, &g_status->idleTime);
	g_spv_camera_state->isRecording = FALSE;
	g_spv_camera_state->isLocked = FALSE;
	//KillTimer(hWnd, TIMER_CLOCK);
    //SpvSetLed(LED_WORK, 100, 50, 1);
    //usleep(450000);
    //SpvSetLed(LED_WORK, 1, 0, 0);
    SpvIndication(IND_VIDEO_END, 0, 0);

	UpdateWorkingStatus();
	return 0;
}

static int TakePicture()
{
    
    if(g_spv_camera_state->calibration == 1)
        return 0;
    INFO("TakePicture\n");
    char value[VALUE_SIZE] = {0};
    if(!g_camera->start_photo()) {
        g_camera->stop_photo();
        UpdateCapacityStatus();
        GetConfigValue(GETKEY(ID_PHOTO_MODE_SEQUENCE), value);
        BOOL sequence = !strcasecmp(value, GETVALUE(STRING_ON));
        //ShowPicturingEffect(hWnd, sequence);
        //UpdatePhotoCount(hWnd, g_spv_camera_state.photoCount + (sequence ? 3 : 1));
        SpvIndication(sequence ? IND_PHOTO_BURST : IND_PHOTO, 0, 0);
        //SpvPlayAudio(SPV_WAV[sequence ? WAV_PHOTO3 : WAV_PHOTO]);
    } else {
        ERROR("taking photo failed\n");
        //ShowToast(hWnd, TOAST_CAMERA_BUSY, 160);
        return ERROR_CAMERA_BUSY;
    }

    return 0;
}

int IsLoopingRecord()
{
    if(IsCardvEnable()) {
        return 1;
    }
    char value[128] = {0};
    GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
    if(strlen(value) > 0 && strcasecmp(value, GETVALUE(STRING_OFF))) {
        return 1;
    }
    return 0;
}

int SpvIsRecording()
{
    if(g_spv_camera_state == NULL)
        return 0;

    return g_spv_camera_state->isRecording;
}

static int DoShutterAction()
{
    if(g_spv_camera_state->mode > MODE_PHOTO) {
        INFO("current mode is : %d, do nothing\n", g_spv_camera_state->mode);
        return 0;
    }
	if(!IsSdcardMounted() && !g_spv_camera_state->isPicturing && !g_spv_camera_state->isRecording) {
		return -1;
	}

    if(IsSdcardFull() && !IsLoopingRecord() && !g_spv_camera_state->isRecording) {
        return -2;
    }
	
	int ret = 0;
    if(g_spv_camera_state->mode == MODE_VIDEO) {
        if(g_spv_camera_state->isRecording){
            ret = StopVideoRecord();
        } else {
            g_spv_camera_state->availableSpace = SpvGetAvailableSpace();
            ret = StartVideoRecord();
        }
    } else if(g_spv_camera_state->mode == MODE_PHOTO) {
        g_spv_camera_state->availableSpace = SpvGetAvailableSpace();
        //if(g_spv_camera_state->isPicturing)
        //    return 0;
        //SetCameraConfig(FALSE);
        ret = TakePicture();
    }

	if(ret) {
		ERROR("DoShutterAction failed! ret: %d\n", ret);
		return ret;
	}
	return 0;
}

/************************* functions for shutdown ***************************/
static void ShutdownProcess(int delay)
{
	static BOOL shutdown;
	if(shutdown)
		return;
	SpvPostMessage(g_launcher_handler, MSG_SHUTDOWN, delay, delay > 0 ? SHUTDOWN_ACC : 0);
	shutdown = TRUE;
}

void DelayedShutdownThread(int delay)
{
	usleep(delay * 1000);
	pthread_testcancel();
	ShutdownProcess(delay);
}

void SpvDelayedShutdown(int delay)
{
	INFO("SpvDelayedShutdown, delay: %d\n", delay);
	static pthread_t t = 0;
	if(delay >= 0) {
		t = SpvCreateServerThread((void *)DelayedShutdownThread, (void *)delay);
	} else {
		if (t > 0)
			pthread_cancel(t);
		t = 0;
	}
}

/************************* functions for wifi & app *************************/
int SpvGetWifiStatus()
{
	return g_spv_camera_state->wifiStatus;
}

void GetWifiApInfo(char *ssid, char *pswd)
{
	GetConfigValue(GETKEY(ID_WIFI_AP_SSID), ssid);
	GetConfigValue(GETKEY(ID_WIFI_AP_PSWD), pswd);
	if(!strcmp(ssid, DEFAULT)) {
		memset(ssid, 0, strlen(DEFAULT));
	}
	if(strlen(pswd) < 8) {
		memset(pswd, 0, 8);
	}
}

static void SpvSetWifiStatus(PWifiOpt pWifi)
{
#ifdef GUI_WIFI_ENABLE
	int ret = -1;
	int old_wifiStatus;
	if(pWifi == NULL)
		return;
	char oldinfo[INFO_WIFI_COUNT][INFO_LENGTH] = {0};
	char *ssid = pWifi->wifiInfo[INFO_SSID];
	if(strlen(ssid) == 0)
		ssid = NULL;
	char *pswd = pWifi->wifiInfo[INFO_PSWD];
	if(strlen(pswd) == 0)
		pswd = NULL;
	char *keymgmt = pWifi->wifiInfo[INFO_KEYMGMT];
	INFO("set wifi status, action:%d, ssid:%s, pswd:%s, keymgmt:%s\n", pWifi->operation,
			pWifi->wifiInfo[INFO_SSID], pWifi->wifiInfo[INFO_PSWD], pWifi->wifiInfo[INFO_KEYMGMT]);
	switch(pWifi->operation) {
		case DISABLE_WIFI:
			if(g_spv_camera_state->wifiStatus == WIFI_OFF)
				break;
			old_wifiStatus = g_spv_camera_state->wifiStatus;
			g_spv_camera_state->wifiStatus = WIFI_CLOSING;
			if(old_wifiStatus == WIFI_AP_ON) {
				ret = g_wifi->stop_softap();
			} else {
				ret = g_wifi->stop_sta();
			}
            SpvIndication(IND_WIFI_OFF, 0, 0);
            //SpvSetLed(LED_WIFI, 0, 0, 0);
			break;
		case ENABLE_AP:
			if(pswd != NULL && strlen(pswd) < 8) {
				pswd = NULL;
			}
			GetWifiApInfo(oldinfo[INFO_SSID], oldinfo[INFO_PSWD]);
			if(g_spv_camera_state->wifiStatus == WIFI_AP_ON) {
				INFO("current wifi is in ap mode\n");
				if((ssid == NULL || pswd == NULL) && (!strcmp(oldinfo[INFO_SSID], DEFAULT) && strlen(oldinfo[INFO_PSWD]) < 8)) {
					break; 	//default ssid & password
				}
				if(((ssid == NULL && strlen(oldinfo[INFO_SSID]) == 0) || !strcmp(ssid, oldinfo[INFO_SSID])) &&
						((ssid == NULL && strlen(oldinfo[INFO_SSID]) == 0) || !strcmp(pswd, oldinfo[INFO_PSWD]))) {
					break;	//same info
				}
			}
            g_spv_camera_state->wifiStatus = WIFI_OPEN_AP;
            DispatchMsgToClient(MSG_WIFI_STATUS, WIFI_OPEN_AP, 0, TYPE_FROM_DEVICE);
            SpvIndication(IND_WIFI_ON, 0, 0);
            //SpvSetLed(LED_WIFI, 500, 500, 1);
			ret = g_wifi->start_softap(ssid, pswd);
			break;
		case ENABLE_STA:
			if(g_spv_camera_state->wifiStatus == WIFI_STA_ON)
				break;
			g_spv_camera_state->wifiStatus = WIFI_OPEN_STA;
            DispatchMsgToClient(MSG_WIFI_STATUS, WIFI_OPEN_STA, 0, TYPE_FROM_DEVICE);
			if(ssid == NULL) {
				ERROR("invalid ssid!\n");
				break;
			}
			if(keymgmt == NULL)
				keymgmt = "WPA-PSK";
			if(pswd != NULL) {
				if(strlen(pswd) < 8 && !strcasecmp(keymgmt, "WPA_PSK")) {
					ERROR("invalid password:%s\n", pswd);
					break;
				}
			}
            SpvIndication(IND_WIFI_ON, 0, 0);
            //SpvSetLed(LED_WIFI, 500, 500, 1);
			ret = g_wifi->start_sta(ssid, keymgmt, pswd);
			break;
		default:
			break;
	}
	free(pWifi);
	pWifi = NULL;
#endif
}

static int SpvSetWifiStatusByConfig(char *newvalue)
{
#ifdef GUI_WIFI_ENABLE
    char value[128] = {0};
	//wifi status
    if(newvalue != NULL) {
        strcpy(value, newvalue);
    } else {
	    GetConfigValue(GETKEY(ID_SETUP_WIFI_MODE), value);
    }
	PWifiOpt pWifi = (PWifiOpt)malloc(sizeof(WifiOpt));
	if(pWifi != NULL) {
		memset(pWifi, 0, sizeof(WifiOpt));
		if(!strcasecmp(GETVALUE(STRING_DIRECT), value)) {
			pWifi->operation = ENABLE_AP;
		    GetWifiApInfo(pWifi->wifiInfo[INFO_SSID], pWifi->wifiInfo[INFO_PSWD]);
		} else if (!strcasecmp(GETVALUE(STRING_ROUTER), value)) {
			pWifi->operation = ENABLE_STA;
		} else {
            pWifi->operation = DISABLE_WIFI;
        }
		SpvSetWifiStatus(pWifi);
	} else {
		ERROR("malloc for pWifi failed!\n");
	}
#endif
	
}

static void NetworkServerInitThread()
{
#ifdef GUI_NETWORK_ENABLE
#ifdef GUI_WIFI_ENABLE
	//wait until wifi is enable
	while(!g_wifi->is_softap_enabled()) {
		sleep(1);
	}
#endif
#ifdef GUI_EVENT_HTTP_ENABLE
    SpvCreateServerThread((void *)init_event_server, NULL);
#endif
#endif
}

int HandleConfigChangedByHttp(int keyId, int valueId)
{
    char value[VALUE_SIZE] = {0};
    GetConfigValue(GETKEY(keyId), value);
    if(!strcasecmp(value, GETVALUE(valueId))) {
        return 0;
    }
    int recording = g_spv_camera_state->isRecording;
    if(recording) {
        StopVideoRecord();
    } 
    SetConfigValue(GETKEY(keyId), GETVALUE(valueId), TYPE_FROM_P2P_DANA);
    SetCameraConfig(FALSE);
    if(recording) {
        if(keyId == ID_VIDEO_LOOP_RECORDING || keyId == ID_VIDEO_RESOLUTION)
        usleep(1000 * 1000);
        //StartVideoRecord();
        DoShutterAction();
    }

    return 0;
}

static int SetModeByConfig(char *mode, FROM_TYPE from)
{
    int i = 0;
    int modeId = -1;
    for(i = 0; i < MODE_COUNT; i++) {
        if(!strcasecmp(g_mode_name[i], mode)) {
            modeId = i;
            break;
        }
    }

    if(modeId == -1) {
        ERROR("mode is not valid, mode: %s\n", mode);
        return -1;
    }

    if(modeId == g_spv_camera_state->mode) {
        INFO("Same mode, no need to change\n");
        return 0;
    }

    if(g_spv_camera_state->isRecording || g_spv_camera_state->isPicturing) {
        INFO("current is recording or picturing, mode should not be changed\n");
        return -1;
    }

    g_spv_camera_state->mode = modeId;
    INFO("mode change, set camera config, modeId: %d\n", modeId);
    SetConfigValue(GETKEY(ID_CURRENT_MODE), g_mode_name[modeId], from);
    switch(g_spv_camera_state->mode) {
        case MODE_VIDEO:
            SetCameraConfig(FALSE);
            SpvIndication(IND_VIDEO_MODE, 0, 0);
            break;
        case MODE_PHOTO:
            SetCameraConfig(FALSE);
            SpvIndication(IND_PHOTO_MODE, 0, 0);
            break;
        default:
            break;
    }

    return 0;
}

static int AdaptConfigChange(int keyId, char *value, FROM_TYPE from)
{
    INFO("keyId: %d, value: %s\n", keyId, value);
    int ret = 0;
    switch(keyId) {
        case ID_CURRENT_MODE:
            ret = SetModeByConfig(value, from);
            break;
        case ID_SETUP_WIFI_MODE:
            ret = SpvSetWifiStatusByConfig(value);
            break;
        case ID_SETUP_LED_FREQ:
            break;
        case ID_SETUP_LED:
            break;
        case ID_SETUP_CARDV_MODE:
        case ID_SETUP_PARKING_GUARD:
            ret = SpvSetParkingGuard(value);
            break;
        case ID_SETUP_VOLUME:
            ret = SpvSetVolume(atoi(value));
            break;
        case ID_SETUP_KEY_TONE:
            SetKeyTone(value);
            break;
        case ID_SETUP_DISPLAY_BRIGHTNESS:
            ret = SpvSetBacklight(value);
            break;
        case ID_SETUP_AUTO_SLEEP:
            SetAutoSleepStatus(value);
            break;
        case ID_SETUP_DELAYED_SHUTDOWN:
            SetDelayedShutdwonStatus(value);
            break;
        case ID_SETUP_AUTO_POWER_OFF:
            SetAutoPowerOffStatus(value);
            break;
    }

    return ret;
}

int HandleConfigChangedByClient(char *keys[], char *values[], int argc, FROM_TYPE from)
{
	int i = 0;
    int ret = 0;
	char existValue[VALUE_SIZE] = {0};
	BOOL reConfigCamera = FALSE;

	for(i = 0; i < argc; i ++) {
        INFO("%d, key: %s, value: %s\n", i, keys[i], values[i]);
		if(keys[i] == NULL || strlen(keys[i]) <= 0 || values[i] == NULL || strlen(values[i]) <= 0)
            return ERROR_INVALID_ARGS;

        int ret = 0;
        int keyId = GetKeyId(keys[i]);
        if(keyId < 0) {
            ERROR("key not found in list, key: %s, value: %s\n", keys[i], values[i]);
            return -1;
        }

		GetConfigValue(keys[i], existValue);
		if(!strcasecmp(existValue, values[i])) {
			INFO("same value, key: %s, value:%s\n", keys[i], values[i]);
			continue;
		}

        ret = AdaptConfigChange(keyId, values[i], from);
        if(keyId != ID_CURRENT_MODE) {
            SetConfigValue(keys[i], values[i], from);
        }

		if(IsKeyBelongsCurrentMode(keys[i])) {		//the cunrrent mode just might be MODE_VIDEO
			reConfigCamera = TRUE;
		}
	}

	if(reConfigCamera) {
		SetCameraConfig(FALSE);
	}
	return 0;
}

/**
 * Called by socket thread, key / value MUST be correct, verified in the socket thread!!
 **/
int send_message_to_gui(MSG_TYPE msg, char *keys[], char *values[], int argc)
{
    INFO("MainWindow, got message from socket,argc:%d\n", argc);
    if(msg == MSG_PIN_SUCCEED) {
		SpvSendMessage(g_launcher_handler, MSG_PAIRING_SUCCEED, 0, 0);
        return 0;
    }

    if(argc <= 0)
        return ERROR_INVALID_ARGS;

    if(msg == MSG_DO_ACTION) {
        if(!strcasecmp(keys[0], g_action_keys[ACTION_SHUTTER_PRESS])) {//shutter action
            if(!strcasecmp(values[0], "photo")) {
				  int camID = 0;
				  if(argc>=2)
				  {
					  if(!strcasecmp(keys[1], "camera"))
					  {
					     camID = atoi(values[1]);
					  }
					  INFO("camID=%d\n", camID);
				  }
                  int ret = TakePicture();
                /*int ret = g_camera->start_photo(camID);
                if(!ret) {
                    g_camera->stop_photo();
                    //UpdateCapacityStatus();
                    UpdatePhotoCount(g_spv_camera_state->photoCount + 1);
                }*/
                return ret;
            }

            if(g_spv_camera_state->mode > MODE_PHOTO)
            {// In playback or setup, discard the mode value
                return ERROR_INVALID_STATUS;
            }

            return SpvSendMessage(g_launcher_handler, MSG_SHUTTER_ACTION, 0, 0);
        } else if(!strcasecmp(keys[0], g_action_keys[ACTION_FILE_DELETE])) {
            UpdateCapacityStatus();
         } else if(!strcasecmp(keys[0], g_status_keys[STATUS_RECORDTIME])) {
		       struct timeval tv;
				gettimeofday(&tv, NULL);
				int time=0;
				if(g_spv_camera_state->isRecording)
				{
					if(tv.tv_sec >g_spv_camera_state->beginTime.tv_sec)
					{
						 time=tv.tv_sec -g_spv_camera_state->beginTime.tv_sec;
					}
				}
				INFO("recordtime=%u,isRecording=%d\n",time,g_spv_camera_state->isRecording);
				return time;
        }else if(!strcasecmp(keys[0], g_action_keys[ACTION_TIME_SET])) {
		       struct timeval tv;
				struct timezone tz;
				time_t rawtime = atoi(values[0]);
				gettimeofday(&tv, &tz);
				int recordTime=0;
              if(g_spv_camera_state->isRecording)
			  	{
					
					if(tv.tv_sec >g_spv_camera_state->beginTime.tv_sec)
					{
						 recordTime=tv.tv_sec -g_spv_camera_state->beginTime.tv_sec;
					}
					g_spv_camera_state->beginTime.tv_sec=rawtime + 8 * 60 * 60-recordTime;
			  	}
				tv.tv_sec = rawtime + 8 * 60 * 60;
				tv.tv_usec = 0;
				
				int result = SetTimeToSystem(tv, tz);
				INFO("recordTime=%d,rawtime=%u\n",recordTime,rawtime);
				return result;
        } else if(!strcasecmp(keys[0], g_action_keys[ACTION_WIFI_SET])) {
            INFO("Got Message: %s:%s\n", keys[0], values[0]);
            PWifiOpt pWifi = (PWifiOpt) malloc(sizeof(WifiOpt));
            if(pWifi == NULL) {
                ERROR("malloc for PWifiOpt failed\n");
                return -1;
            }
            memset(pWifi, 0, sizeof(WifiOpt));
            char *valueString = values[0];
            char opt[128] = {0};
            char *tmpBegin = NULL;
            char *tmpEnd = NULL;
            tmpBegin = strstr(valueString, ";;");
            if(tmpBegin == NULL) {
                if(!strncasecmp(valueString, "off", 3)) {
                    pWifi->operation = DISABLE_WIFI;
                    SpvSetWifiStatus(pWifi);
                    return 0;
                } else if(!strncasecmp(valueString, "ap", 2)) {
                    pWifi->operation = ENABLE_AP;
                    SpvSetWifiStatus(pWifi);
                    return 0;
                }
                ERROR("invalide values, %s\n", values[0]);
                return -1;
           }
            strncpy(opt, valueString, tmpBegin - valueString);
            if(!strncasecmp(opt, "ap", 2)) {
                pWifi->operation = ENABLE_AP;
            } else if(!strncasecmp(opt, "sta", 3)) {
                pWifi->operation = ENABLE_STA;
            } else if(!strncasecmp(opt, "off", 3)) {
                pWifi->operation = DISABLE_WIFI;
                SpvSetWifiStatus(pWifi);
                return 0;
            } else {
                ERROR("invalide values, %s\n", values[0]);
                return -1;
            }

            int i = 0;
            while (i < INFO_WIFI_COUNT) {
                tmpBegin += 2;
                tmpEnd = strstr(tmpBegin, ";;");
                if(tmpEnd == NULL) {//the end
                    INFO("i: %d, info: %s\n", i, pWifi->wifiInfo[i]);
                    strcpy(pWifi->wifiInfo[i], tmpBegin);
                    break;
                }
                strncpy(pWifi->wifiInfo[i], tmpBegin, tmpEnd - tmpBegin);
                INFO("i: %d, info: %s\n", i, pWifi->wifiInfo[i]);
                tmpBegin = tmpEnd;
                i++;
            }
            SpvSetWifiStatus(pWifi);
            return 0;
        }
        return 0;
    }

    int ret = HandleConfigChangedByClient(keys, values, argc, TYPE_FROM_P2P_DANA);
    return ret;

}

int SendMsgToServer(int message, int wParam, long lParam, int from)
{
    int ret = 0;
    switch(message) {
        case MSG_FORMAT_SDCARD:
            ret = SpvSendMessage(g_launcher_handler, MSG_FORMAT_SDCARD, 0, 0);
            break;
        case MSG_CONFIG_CHANGED:
        {
            char *keys[1], *values[1];
            keys[0] = (char *)wParam;
            values[0] = (char *)lParam;
            INFO("Got msg, MSG_CONFIG_CHANGED, key: %s, value: %s\n", keys[0], values[0]);
            ret = HandleConfigChangedByClient(keys, values, 1, from);
            break;
        }
        case MSG_SHUTTER_ACTION:
        {
            return DoShutterAction();
        }
        case MSG_TAKE_SNAPSHOT:
        {
            if(g_camera == NULL) {
                ERROR("MSG_TAKE_SNAPSHOT, g_camera == NULL\n");
                return -1;
            }
            //ret = g_camera->start_photo();
            ret = TakePicture();
            break;
        }
        case MSG_SHUTDOWN:
        {
            SpvPostMessage(g_launcher_handler, MSG_SHUTDOWN, wParam, lParam);
            break;
        }
        case MSG_RESTORE_SETTINGS:
        {
            ResetCamera();
            DispatchMsgToClient(MSG_RESTORE_SETTINGS, 0, 0, from);
            break;
        }
        default:
            break;
    }
    return ret;
}

int DispatchMsgToClient(int msg, int wParam, long lParam, int from)
{
    int i = 0;
    for(i = 0; i < TYPE_FROM_COUNT; i++) {
        if(((from != i)||(from==TYPE_FROM_P2P_DANA)) && g_spv_client[i].SystemMsgListener != NULL) {
            g_spv_client[i].SystemMsgListener(msg, wParam, lParam);
        }
    }
    return 0;
}

int RegisterSpvClient(int type, int (*SystemMsgListener)(int message, int wParam, long lParam))
{
    if(type < 0 || type >= TYPE_FROM_COUNT) {
        INFO("error type: %d\n", type);
    }

    g_spv_client[type].SystemMsgListener = SystemMsgListener;
    return 0;
}


SPVSTATE* GetSpvState()
{
    return g_spv_camera_state;
}

#ifndef GUI_GUI_ENABLE
int IsMainWindowActive()
{
    return 1;
}
#endif

/************************* functions for keys *******************************/
static void OnKeyDown(int keyCode, long lParam)
{
}

static void OnKeyUp(int keyCode, long lParam)
{
    INFO("Get the KEYUP message(code:%d, lparam:%d).\n");
    char value[VALUE_SIZE] = {0};
    switch(keyCode)
    {
		case SCANCODE_INFOTM_OK:
			DoShutterAction();
			break;
		case SCANCODE_INFOTM_LOCK:
			if(g_spv_camera_state->isRecording)
				SpvSetCollision(g_camera);
			break;
		case SCANCODE_INFOTM_PHOTO:
            if(IsSdcardMounted() && !IsSdcardFull())
                TakePicture();
			/*if(!g_camera->start_photo(2)) {
				g_camera->stop_photo();
				UpdatePhotoCount(g_spv_camera_state->photoCount + 1);
			}*/
			break;
        case SCANCODE_INFOTM_MODE:
            if(g_spv_camera_state->isRecording || g_spv_camera_state->isPicturing) {
                INFO("current is recording/picturing, should not change mode\n");
                return;
            }
            if(SpvIsInVideoMode()) {
                SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_CURRENT_MODE), g_mode_name[MODE_PHOTO], TYPE_FROM_DEVICE);
            } else {
                SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_CURRENT_MODE), g_mode_name[MODE_VIDEO], TYPE_FROM_DEVICE);
            }
            break;
		case SCANCODE_INFOTM_SHUTDOWN:
			SpvPostMessage(g_launcher_handler, MSG_SHUTDOWN, 0, SHUTDOWN_KEY);
			break;
        case SCANCODE_INFOTM_RESET:
            if(g_spv_camera_state->isRecording) {
                StopVideoRecord();
            }
            SendMsgToServer(MSG_RESTORE_SETTINGS, 0, 0, TYPE_FROM_DEVICE);
            SpvIndication(IND_CAMERA_RESET, 0, 0);
            break;
		case SCANCODE_INFOTM_WIFI:
			if(g_spv_camera_state->wifiStatus == WIFI_OFF) {
				SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_SETUP_WIFI_MODE), GETVALUE(STRING_DIRECT), TYPE_FROM_DEVICE);
			} else if(g_spv_camera_state->wifiStatus == WIFI_AP_ON || g_spv_camera_state->wifiStatus == WIFI_STA_ON) {
				SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_SETUP_WIFI_MODE), GETVALUE(STRING_OFF), TYPE_FROM_DEVICE);
			}
			break;
		default:
			break;
	}
}

int MainMsgProc(HMSG hMsg, int msgId, int wParam, long lParam)
{
    //INFO("hMsg: %ld, msgId: %d, wParam: %d, lParam: %ld\n", hMsg, msgId, wParam, lParam);
    switch(msgId) {
		case MSG_HDMI_HOTPLUG:
			INFO("MSG_HDMI_HOTPLUG, wParam:%d, lParam:%ld\n", wParam, lParam);
			return 0;
        case MSG_BATTERY_STATUS:
        case MSG_CHARGE_STATUS:
        {
            INFO("MSG_BATTERY_STATUS, MSG_CHARGE_STATUS, msg: %d, wParam:%d, lParam:%ld\n", msgId, wParam, lParam);
            if(msgId == MSG_BATTERY_STATUS) {
                g_spv_camera_state->battery = wParam;//SpvGetBattery();
            } else if(msgId == MSG_CHARGE_STATUS) {
                g_spv_camera_state->charging = (wParam == BATTERY_CHARGING);//SpvIsCharging();
            }
            //if(MSG_CHARGE_STATUS == msgId && SpvAdapterConnected()) {
                //if(IsTimerInstalled(hWnd, TIMER_AUTO_SHUTDOWN)) {
                //    KillTimer(hWnd, TIMER_AUTO_SHUTDOWN);
                //}
            //}
			if(g_spv_camera_state->battery < 5 && !SpvAdapterConnected()) {
				SendMsgToServer(MSG_SHUTDOWN, 0, SHUTDOWN_POWER_LOW, TYPE_FROM_DEVICE);
			}
            SpvSetBatteryLed(g_spv_camera_state->battery, g_spv_camera_state->charging);
            UpdateBatteryStatus();
            DispatchMsgToClient(msgId, wParam, lParam, TYPE_FROM_DEVICE);
            return 0;
        }
        case MSG_WIFI_STATUS:
        {
            g_spv_camera_state->wifiStatus = wParam;
            /*if(wParam == WIFI_AP_ON || wParam == WIFI_STA_ON) {
                SpvSetLed(LED_WIFI, 500, 500, 1);
            }*/
            DispatchMsgToClient(msgId, wParam, lParam, TYPE_FROM_DEVICE);
            break;
        }
        case MSG_WIFI_CONNECTION:
        {
            switch(wParam) {
                case WIFI_AP_CONNECTED:
                case WIFI_STA_CONNECTED:
                    SpvSetLed(LED_WIFI, 1, 0, 0);
                    break;
                case WIFI_AP_DISCONNECTED:
                    if(lParam > 0) {
                        break;
                    }
                case WIFI_STA_DISCONNECTED:
                    if(g_spv_camera_state->wifiStatus >= WIFI_OPEN_AP)
                        SpvSetLed(LED_WIFI, 500, 500, 1);
                    break;
            }
            DispatchMsgToClient(msgId, wParam, lParam, TYPE_FROM_DEVICE);
            break;
        }
		case MSG_SDCARD_MOUNT:
			INFO("MSG_SDCARD_MOUNT, wParam:%d, lParam:%ld\n", wParam, lParam);
	        clock_gettime(CLOCK_MONOTONIC_RAW, &g_status->idleTime);
			if(g_spv_camera_state->sdStatus == wParam)
				return 0;
            g_spv_camera_state->sdStatus = wParam;
            if(IsSdcardMounted() && !access("/mnt/sd0/i_need_calibration", F_OK)) {
                SpvEnterCalibrationMode();
            }
			UpdateCapacityStatus();
			if(wParam == SD_STATUS_NEED_FORMAT) {
				SpvFormatSdcard();
				g_spv_camera_state->sdStatus = wParam;
				UpdateCapacityStatus();
                SpvScanMediaFiles();
				return 0;
			}
			if(wParam == SD_STATUS_MOUNTED && g_spv_camera_state->availableSpace <= 0) {
				ERROR("Outdated message, sdcard status has been changed\n");
				return 0;
			}
			if(!IsSdcardMounted()) {
				//sdcard removed or need format or error
				//stop video & picture
				if(g_spv_camera_state->isRecording) {
                    StopVideoRecord();
				}
				if(g_spv_camera_state->isLocked)
					g_spv_camera_state->isLocked = FALSE;
			} else if(IsSdcardFull()) {
                DispatchMsgToClient(MSG_CAMERA_CALLBACK, CAMERA_ERROR_SD_FULL, 0, TYPE_FROM_DEVICE);
				//sdcard is full
			} else {
				//sdcard mounted
				//we start record when sdcard inserted in hidden style
				//DoShutterAction();
			}
            SpvScanMediaFiles();
            DispatchMsgToClient(msgId, wParam, lParam, TYPE_FROM_DEVICE);
			return 0;
        case MSG_FORMAT_SDCARD:
            if(g_spv_camera_state->sdStatus != SD_STATUS_UMOUNT) {
                int ret = SpvFormatSdcard();
                if(!ret) {
                    SpvScanMediaFiles();
                }
                return ret;
            } 
            return -1;
		case MSG_MEDIA_UPDATED:
			{
				int vc, pc;
				GetMediaFileCount(&vc, &pc);
				INFO("MSG_MEDIA_UPDATED, videocount: %d, photocount: %d\n", vc, pc);
				UpdatePhotoCount(pc);
#ifdef GUI_NETWORK_ENABLE
				UpdateCapacityStatus();
#endif
                DispatchMsgToClient(msgId, wParam, lParam, TYPE_FROM_DEVICE);
				return 0;
			}
		case MSG_SHUTTER_ACTION:
            {
			if(lParam == 1 && g_spv_camera_state->sdStatus != SD_STATUS_MOUNTED)
				return 0;
            if(!IsMainWindowActive()) {
                return -1;
            }
			int ret = DoShutterAction();
			if(g_spv_camera_state->isRecording != TRUE) {
				ERROR("MSG_SHUTTER_ACTION, start shutter pressed\n");
				return ret;
			}
			if(FLAG_POWERED_BY_GSENSOR != wParam)
				return ret;
			INFO("FLAG_POWERED_BY_GSENSOR detected\n");
			SpvSetCollision(g_camera);
			return ret;
            }
		case MSG_SHUTDOWN:
			ERROR("MSG_SHUTDOWN: shutdown, reason: %d\n", lParam);
			StopVideoRecord();
            SpvIndication(IND_SHUTDOWN, lParam, 0);
			SaveConfigToDisk();
			system("umount /mnt/config");
			sync();
			//SpvPlayAudio(SPV_WAV[WAV_BOOTING]);
			/*system("echo 0x01b5 >/sys/class/pmu-debug/reg; echo \"echo 0x01b5\"; cat /sys/class/pmu-debug/reg; \
					echo 0x8218 >/sys/class/pmu-debug/reg; echo \"echo 0x8218\"; cat /sys/class/pmu-debug/reg");*/
            if(!wParam)
                sleep(1);//delay 2s for saving files
			SpvShutdown();
			break;
		case MSG_GET_UI_STATUS:
			return (int)g_status;
		case MSG_COLLISION:
			INFO("MSG_COLLISION\n");
            if(!IsCardvEnable())
                return 0;

			char value[VALUE_SIZE] = {0};
			GetConfigValue(GETKEY(ID_VIDEO_GSENSOR), value);
			if(!strcasecmp(value, GETVALUE(STRING_OFF))) {
				return 0;
			}
            if(!IsMainWindowActive()) {
                return 0;
            }

			if(g_spv_camera_state->isRecording) {
				SpvSetCollision(g_camera);
				return 0;
			} else if(g_spv_camera_state->mode == MODE_VIDEO) {
			    DoShutterAction();
            }
			if(g_spv_camera_state->isRecording) {
				//SpvPlayAudio(SPV_WAV[WAV_KEY]);
				SpvSetCollision(g_camera);
				DispatchMsgToClient(MSG_COLLISION, wParam, lParam, TYPE_FROM_DEVICE);
			}
			return 0;
		case MSG_REAR_CAMERA:
#ifdef GUI_CAMERA_REAR_ENABLE
			INFO("MSG_REAR_CAMERA, device inserted:%d\n", wParam);
			UpdateRearCameraStatus(wParam);
			SetConfigValue(GETKEY(ID_VIDEO_FRONTBIG), GETVALUE(STRING_ON), TYPE_FROM_GUI);
			if(!SetCameraConfig(FALSE))
				g_camera->start_preview();
			if(g_spv_camera_state->isRecording && wParam) {
				g_camera->start_video();
			}
            DispatchMsgToClient(MSG_REAR_CAMERA, wParam, lParam, TYPE_FROM_DEVICE);
#endif
			break;
		case MSG_ACC_CONNECT:
			INFO("MSG_ACC_CONNECT, wParam: %d\n", wParam);
			SpvDelayedShutdown(wParam ? -1 : 8000);
            DispatchMsgToClient(MSG_ACC_CONNECT, wParam, lParam, TYPE_FROM_DEVICE);
			break;
		case MSG_GPS:
#ifdef GUI_GPS_ENABLE
			INFO("MSG_GPS:lParam:%ld, g_sync:%d\n", lParam, g_sync_gps_time);
			gps_data *gd = (gps_data *)lParam;
			if(lParam == 0)
				return 0;
			GPS_Data data;
			data.latitude = gd->latitude;
			data.longitude = gd->longitude;
			data.speed = gd->speed * 1.852f;
			if(data.latitude == 0.0f || data.longitude == 0.0f) {
				data.latitude = 0xFF;
				data.longitude = 0xFF;
				data.speed = 0xFF;
			}
			SpvUpdateGpsData(&data);
			memcpy(&(g_spv_camera_state->gpsData), &data, sizeof(GPS_Data));

			static BOOL time_updated = FALSE;
			if(!g_spv_camera_state->isRecording && g_sync_gps_time && !time_updated) {
				if(!SetTimeByData(gd->gut))
					time_updated = TRUE;
			}
			free(gd);
            DispatchMsgToClient(MSG_GPS, wParam, lParam, TYPE_FROM_DEVICE);
#endif
			break;
		case MSG_CAMERA_CALLBACK:
			switch(wParam) {
				case CAMERA_ERROR_INTERNAL:
				case CAMERA_ERROR_DISKIO:
			        INFO("MSG_CAMERA_CALLBACK, wParam: %d, isRecording: %d\n", wParam, g_spv_camera_state->isRecording);
					if(g_spv_camera_state->isRecording) {
						StopVideoRecord();
					}
                    DispatchMsgToClient(MSG_CAMERA_CALLBACK, wParam, lParam, TYPE_FROM_DEVICE);
                    break;
                case CAMERA_ERROR_SD_FULL: 
                    {
                    static int last_status = 0;
                    if(last_status != lParam) {
			            INFO("MSG_CAMERA_CALLBACK, wParam: %d, isRecording: %d\n", wParam, g_spv_camera_state->isRecording);
                        INFO("CAMERA_ERROR_SD_FULL, status: %d\n", lParam);
                        last_status = lParam;
                        SpvIndication(IND_SDCARD_FULL, last_status, 0);
                        if(last_status) {
                            if(g_spv_camera_state->isRecording)
                                StopVideoRecord();
                            UpdateIdleTime();
                            DispatchMsgToClient(MSG_CAMERA_CALLBACK, wParam, lParam, TYPE_FROM_DEVICE);
                        }
                    }
                    }
					break;
				default:
					break;
			}
            DispatchMsgToClient(MSG_CAMERA_CALLBACK, wParam, lParam, TYPE_FROM_DEVICE);
			break;
		case MSG_USER_KEYUP:
			OnKeyUp(wParam, lParam);
			break;
        case MSG_CALIBRATION:
            INFO("Got message, calibration\n");
            SpvSetLed(LED_WORK, 300, 300, 1);
            SpvSetLed(LED_WIFI, 300, 300, 1);
            if(g_spv_camera_state->calibration == 1)
                return 0;
            g_spv_camera_state->calibration = 1;
            extern int enable_calibration();
            SpvCreateServerThread((void *)enable_calibration, NULL);
            break;
#ifdef UVC_ENABLE
		case MSG_SWITCH_TO_UMS:
            INFO("Got message, switch to ums\n");
			uvc_server_stop();
			system("/etc/init.d/ums_1.sh");
			uvc_server("encavc0-stream");
			system("/etc/init.d/ums_2.sh");
			break;
		case MSG_SWITCH_TO_UVC:
            INFO("Got message, switch to uvc\n");
			uvc_server_stop();
			system("/etc/init.d/uvc_1.sh");
			uvc_server("encavc0-stream");
			system("/etc/init.d/uvc_2.sh");
			break;
#endif
        case MSG_ONLINE:
            UpdateIdleTime();
            g_spv_camera_state->online = wParam;
            break;
		default:
            break;
    }
    return 0;
}

static void InitSPV()
{
	struct timeval t1, t2;
	gettimeofday(&t1, 0);
	char value[VALUE_SIZE] = {0};

	g_spv_camera_state->sdStatus = SpvGetSdcardStatus();
    if(IsSdcardMounted() && !access("/mnt/sd0/i_need_calibration", F_OK)) {
        SpvEnterCalibrationMode();
    }

	char *configPath = NULL;
	configPath = CONFIG_PATH"./spv_gui.config";
	if(access(CONFIG_PATH"./spv_gui.config", F_OK)) {
		int ret = copy_file(RESOURCE_PATH"./config/spv_gui.config.bak", CONFIG_PATH"./spv_gui.config", TRUE);
		INFO("config file not exist, copy it, ret: %d\n", ret);
		if(ret) { 
			ERROR("Can not get config file:%s, use the default!!!\n", CONFIG_PATH"./spv_gui.config");
			configPath = RESOURCE_PATH"./config/spv_gui.config";
		}
	}
	InitConfigManager(configPath);

#ifdef GUI_DEFAULT_MODE_PHOTO
	SetConfigValue(GETKEY(ID_CURRENT_MODE), g_mode_name[MODE_PHOTO], TYPE_FROM_DEVICE);
    g_spv_camera_state->mode = MODE_PHOTO;
    SpvIndication(IND_PHOTO_MODE, 1, 0);
#else
	SetConfigValue(GETKEY(ID_CURRENT_MODE), g_mode_name[MODE_VIDEO], TYPE_FROM_DEVICE);
    g_spv_camera_state->mode = MODE_VIDEO;
    SpvIndication(IND_VIDEO_MODE, 1, 0);
#endif

#ifdef GUI_CAMERA_REAR_ENABLE
	UpdateRearCameraStatus(rear_camera_inserted());
#ifndef GUI_CAMERA_PIP_ENABLE
	SetConfigValue(GETKEY(ID_VIDEO_PIP), "Off", TYPE_FROM_GUI);
#endif
#endif

#ifdef GUI_WIFI_ENABLE    
	SetConfigValue(GETKEY(ID_SETUP_WIFI_MODE), GETVALUE(STRING_OFF), TYPE_FROM_DEVICE);
    SpvSetWifiStatusByConfig(NULL);
    //SpvCreateServerThread((void *)NetworkServerInitThread, NULL);
#endif

    g_spv_camera_state->battery = SpvGetBattery();
    g_spv_camera_state->charging = SpvIsCharging();
    SpvSetBatteryLed(g_spv_camera_state->battery, g_spv_camera_state->charging);
    UpdateBatteryStatus();

	UpdateCapacityStatus();
	UpdateWorkingStatus();
	//ScanSdcardMediaFiles(IsSdcardMounted());
	SpvSetParkingGuard(NULL);
	GetConfigValue(GETKEY(ID_SETUP_VOLUME), value);
	SpvSetVolume(atoi(value));
	g_fucking_volume = atoi(value);
	SetKeyTone(NULL);
#if 0
	g_spv_camera_state->isDaytime = get_ambientBright();
	SetIrLed();
#endif
	
	memset(g_status, 0, sizeof(SleepStatus));
	g_status->idle = 1;
	clock_gettime(CLOCK_MONOTONIC_RAW, &g_status->idleTime);
    SetAutoSleepStatus(NULL);
	SetDelayedShutdwonStatus(NULL);
	SetAutoPowerOffStatus(NULL); 
}

void ResetCamera()
{
	DeinitConfigManager();
	copy_file(RESOURCE_PATH"./config/spv_gui.config.bak", CONFIG_PATH"./spv_gui.config", TRUE);
	InitSPV();
	SetCameraConfig(TRUE);
}
/***************************************************************************/
#ifdef GUI_GUI_ENABLE
int sysserver_proc()
#else
int main(int argc, char *argv[])
#endif
{
#ifdef UVC_ENABLE
	uvc_server("encavc0-stream");
#endif

#ifdef GUI_GUI_ENABLE
    prctl(PR_SET_NAME, __func__);
#endif
    int ret = 0;
    INFO("sysserver start\n");
	g_launcher_handler = SpvRegisterHandler(MainMsgProc);
    if(g_launcher_handler == 0) {
        ERROR("main handler register failed\n");
        return -1;
    }

    g_spv_camera_state = (SPVSTATE*)malloc(sizeof(SPVSTATE));
    memset(g_spv_camera_state, 0, sizeof(SPVSTATE));

	InitSystemTime();
#ifdef GUI_NETWORK_ENABLE
#ifdef GUI_WIFI_ENABLE
	g_wifi = wifi_create();
	INFO("g_wifi:0x%x\n", g_wifi);
#endif
	init_socket_server();
	SpvCreateServerThread((void *)init_stream_server, NULL);
#endif
    SpvCreateServerThread((void *)init_file_receiver_server, NULL);

#ifdef GUI_TIRED_ALARM
	SpvTiredAlarm(SPV_WAV[WAV_KEY]);
#endif

	g_status = (PSleepStatus)malloc(sizeof(SleepStatus));
	if(g_status == NULL) {
		ERROR("malloc g_status failed\n");
		exit(1);
	}
	memset(g_status, 0, sizeof(SleepStatus));
    //g_status->idle = 1;

	InitSPV();

	if(InitInfotmServer(g_launcher_handler)) {
		ERROR("Init infotmServer threads failure!\n");
		goto failure;
	}

    if(SpvInitFileManager()) {
        ERROR("init file manager failed\n");
        goto failure;
    }

#ifdef GUI_GPS_ENABLE
	char value[VALUE_SIZE] = {0};
	GetConfigValue(GETKEY(ID_TIME_AUTO_SYNC), value);
	if(!strcasecmp(value, GETVALUE(STRING_ON)))
		g_sync_gps_time = TRUE;
	else
		g_sync_gps_time = FALSE;
#endif

	config_camera config;
	GetCameraConfig(&config);
	g_camera = camera_create(config);
    if(g_camera == NULL) {
        ERROR("camera_create failed\n");
    }
	g_camera->init_camera(); //empty
	register_camera_callback(g_camera, CameraInfoCallback);
	//SetCameraConfig(TRUE);

    g_spv_camera_state->initialized = 1;

#ifdef GUI_P2P_DANA_ENABLE
    extern int p2p_dana_server();
    SpvCreateServerThread((void *)p2p_dana_server, NULL);
#endif

    if(IsCardvEnable()) {
	    SpvPostMessage(g_launcher_handler, MSG_SHUTTER_ACTION, SpvPowerByGsensor() ? FLAG_POWERED_BY_GSENSOR : FLAG_POWERED_BY_OTHER, 1);
    }

	SpvIndication(IND_BOOTING_UP, 0, 0);

    //message loop
    SpvMessageLoop(g_launcher_handler);

#ifdef GUI_WIFI_ENABLE
	wifi_destroy();
	g_wifi = NULL;
#endif
failure1:
	//TerminateKeyServer();
failure:
	TerminateInfotmServer();

	INFO("spv_launcher exit!!!\n");
	return 0;
}
