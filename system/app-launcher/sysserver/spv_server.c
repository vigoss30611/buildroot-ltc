#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/prctl.h>

#include "spv_indication.h"
#include "spv_common.h"

#include "spv_message.h"
#include "spv_server.h"
#include "spv_input.h"
#include "spv_debug.h"
#include "spv_utils.h"

#include <qsdk/items.h>
#include "gps.h"

#include "qsdk/event.h"
#include "qsdk/cep.h"

#define  EVENT_USB_HOST_STATUS "usb_host_status"

pthread_t __info_server;
static int loopServerRun = 1;
HMSG serverHandler;

enum {
    ACTION_CLICK,
    ACTION_DOUBLE_CLICK,
    ACTION_LONG_PRESS,
};
#if 0
#define FIFO_SERVER 	"/tmp/myfifo"
#define ACC_NODE        "/sys/devices/platform/power_domain/caracc"

#define BATTERY_EVENT
//#define SDCARD_EVENT
#define AMBIENT_EVENT	1
#define COLLISION_EVENT
#define REAR_CAMERA_EVENT
#define REVERSE_IMAGE_EVENT
//#define GPS_EVENT

#define EMPTY_POWER		38
#define LOW_POWER		40
#define LOW_VOLTAGE		3600
#define EMP_VOLTAGE		3450

#define CHECK_INTERVAL 200*1000

static int ACC_ENABLE = 0;

static void* InfotmServerLoop(void)
{
	int timeRotator = 0;
	char buf[16];
	//for UI status control
	PSleepStatus ui_status;
	int ledStatus = 0;
	time_t time_idle;
	//for ACC Event
	int accHandle = 0;
	int accConnect = 0, old_accConnect = -1;		//is the ACC connect?
#ifdef COLLISION_EVENT
	int gHandle = 0;
	int oldGsenesor = -1;
	static int eventHandled = 0;
	struct input_event eventValue;
	char *gEvent = getenv("GSENSOR_DEVICE");
	if(gEvent != NULL)
		gHandle = open(gEvent, O_RDONLY|O_NOCTTY|O_NONBLOCK);
	if(gHandle <= 0)
		ERROR("Failed to open g-sensor handle(%s)\n", gEvent);
#endif

#ifdef REAR_CAMERA_EVENT
	int rearCameraSta = 0, old_rearCameraSta = -1;	//is the rear camera connect?
#endif
#ifdef REVERSE_IMAGE_EVENT
	int riHandle = 0;
	int reverseImage = 0, old_reverseImage = -1;	//need to reverse the image or not.
#endif
#ifdef BATTERY_EVENT
	time_t timep_discharge;
	int tmp_voltage = 0;
	int isCharge = 0, old_isCharge = -1;			//the charge status
	int batteryCapa = 0, old_batteryCapa = -1;		//the capacity of the battery
	int isLowPower = 0;								//battery is low power
	int needShutDown = 0;							//the device need to be shuted, for example:the turn to discharging
	int cap_sleep = 10;								//use for setting the sleep time of checking battery capacity
#endif
#ifdef SDCARD_EVENT
	int sd_status = 0, old_sd_status = -1;			//get the status of Sdcard
#endif

	time(&time_idle);

	while(loopServerRun)
	{
#ifdef COLLISION_EVENT
		if(gHandle > 0 && !(timeRotator % 5)) {
			eventHandled = 0;
			lseek(gHandle, 0, SEEK_SET);
			if(read(gHandle, &eventValue, sizeof(eventValue)) > 0
					&& eventValue.code == 220
					&& oldGsenesor != eventValue.value
					&& !eventHandled) {
				INFO("Report g-sensor event,message ID=%d.\n", MSG_COLLISION);
				SpvPostMessage(serverHandler, MSG_COLLISION, 1, 0);
				oldGsenesor = eventValue.value;
				eventHandled = 1;
			}
		}
#endif

#ifdef REAR_CAMERA_EVENT
		if(!(timeRotator % 5)) {
			rearCameraSta = !access("/dev/video0", F_OK);
			if(rearCameraSta != old_rearCameraSta) {
				INFO("Rear camera status changes,msgId=%d,status=%d.\n", MSG_REAR_CAMERA, rearCameraSta);
				SpvPostMessage(serverHandler, MSG_REAR_CAMERA, rearCameraSta, 0);
				old_rearCameraSta = rearCameraSta;
			}
		}
#endif

#ifdef REVERSE_IMAGE_EVENT
		if(!(timeRotator % 5)) {
			riHandle = open("/sys/devices/virtual/switch/p2g/state", O_RDONLY);
			reverseImage = 0;
			if(riHandle > 0) {
				memset(buf, 0, sizeof(buf));
				if(read(riHandle, buf, 16) > 0)
					reverseImage = atoi(buf) == 1 ? 1 : 0;
				close(riHandle);
			}
			if(reverseImage != old_reverseImage) {
				INFO("Reverse image status changes,msgId=%d,status=%d.\n", MSG_REVERSE_IMAGE, reverseImage);
				SpvPostMessage(serverHandler, MSG_REVERSE_IMAGE, reverseImage, 0);
				old_reverseImage = reverseImage;
			}
		}
#endif

		if(ACC_ENABLE && !(timeRotator % 5)) {
			accHandle = open(ACC_NODE, O_RDONLY);
			accConnect = 0;
			if(accHandle > 0) {
				if(read(accHandle, buf, 16) > 0)
					accConnect = atoi(buf) == 1 ? 1 : 0;
				close(accHandle);
			}
			if(accConnect != old_accConnect) {
				INFO("Acc connect status changes,msgId=%d,status=%d.\n",MSG_ACC_CONNECT, accConnect);
				SpvPostMessage(serverHandler, MSG_ACC_CONNECT, accConnect, 0);
				old_accConnect = accConnect;
			}
		}

#ifdef BATTERY_EVENT
		if(!(timeRotator % 5)) {
			isCharge = is_charging();
			if(isCharge < 0) {
				ERROR("Get battery charge status failed\n");
			} else {
				if(isCharge != old_isCharge) {
					//INFO("Charge status changes,msgId=%d,status=%d.\n", MSG_CHARGE_STATUS, isCharge);
					//SpvPostMessage(serverHandler, MSG_CHARGE_STATUS, isCharge, 0);
					if(isCharge == BATTERY_DISCHARGING && old_isCharge > 0)
						needShutDown = 1;
					else
						needShutDown = 0;

					old_isCharge = isCharge;
				}
				if(isCharge != BATTERY_DISCHARGING)
					time(&timep_discharge);
			}
		}
		if(!(timeRotator % cap_sleep)) {
			tmp_voltage = check_battery_capacity();
			if(tmp_voltage < 0) {
				ERROR("Get the battery voltage failed\n");
			} else {
				if(tmp_voltage < EMP_VOLTAGE && isCharge == BATTERY_DISCHARGING) {
					INFO("Device is EMP_VOLTAGE,vol=%d\n", tmp_voltage);
					KeyReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);
				}
				if((isCharge == BATTERY_DISCHARGING && old_batteryCapa > batteryCapa)
						||(isCharge == BATTERY_CHARGING && old_batteryCapa < batteryCapa)
						||(isCharge == BATTERY_FULL && old_batteryCapa != batteryCapa))
				{
					//INFO("Battery capacity changes,msgID=%d,capacity=%d,vol=%d.\n", MSG_BATTERY_STATUS, batteryCapa, tmp_voltage);
					//SpvPostMessage(serverHandler, MSG_BATTERY_STATUS, (int)batteryCapa, 0);
					old_batteryCapa = batteryCapa;
				}
			}
		}
#endif

#ifdef SDCARD_EVENT
		if(!(timeRotator % 2)) {
			SD_Status status;
			Inft_sdcard_getStatus(&status);
			sd_status = (int)status.sdStatus;
			if(sd_status != old_sd_status) {
				INFO("Sdcard status changes,msgId=%d,staus=%d.\n",MSG_SDCARD_MOUNT, sd_status);
				SpvPostMessage(serverHandler, MSG_SDCARD_MOUNT, sd_status, 0);
				old_sd_status = sd_status;
			}
		}
#endif
		//get ui_status
		if(!(timeRotator % 2)) {
			ui_status = SpvSendMessage(serverHandler, MSG_GET_UI_STATUS, 0, 0);
			if(ui_status <= 0) {
				INFO("UIStatus update failed!\n");
				if(isCharge == BATTERY_DISCHARGING) {
					INFO("UIStatue error, report shutdown(discharging)\n");
					KeyReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);
				}
			} else {
				if(ui_status->idle) {
					//the device is idle
					if(ledStatus != 1) {
						indicator_led_switch(1);
						ledStatus = 1;
					}
				} else {
					set_indicator_led_flicker(1);
					ledStatus = 0;
					time(&time_idle);
				}
			}
		}


		usleep(CHECK_INTERVAL);
		if(timeRotator++ == 50)
			timeRotator = 0;
	}//while
}

#ifdef GPS_EVENT
static void GPSUpdateLoop()
{
	gps_data *gdata = NULL;

	while(loopServerRun) {
		sleep(1);
		gdata = (gps_data *)malloc(sizeof(gps_data));
		if(gd != NULL) {
			memset(gdata, 0 sizeof(gps_data));
			if(!gps_request(gdata)) {
				INFO("GPS status changes,msgId=%d,status=%ld.\n", MSG_GPS, gdata);
				SpvPostMessage(serverHandler, MSG_GPS, 0, gdata);
			}
			free(gdata);
		}
		gdata = NULL;
	}
}
#endif
#endif

int UpdateIdleTime()
{
    PSleepStatus pStatus = GetSleepStatus();
    clock_gettime(CLOCK_MONOTONIC_RAW, &pStatus->idleTime);
    return 0;
}

static SpvReportKeyEvent(int msg, int scancode, long lParam)
{
    if(msg == MSG_USER_KEYDOWN) {
        UpdateIdleTime();
        SpvIndication(IND_KEY, 0, 0);
        //SpvPlayAudio(SPV_WAV[WAV_KEY]);
    }
#ifdef GUI_GUI_ENABLE
        DispatchMsgToClient(msg, scancode, lParam, TYPE_FROM_DEVICE);
#else
        SpvPostMessage(serverHandler, msg, scancode, lParam);
#endif
}

static int SpvKeyMap(int keycode)
{
    int scancode = -1;
    switch(keycode) {
#if defined(PRODUCT_LESHI_C23)
        case 116:
            scancode = SCANCODE_INFOTM_OK;
            break;
        case 352:
            scancode = SCANCODE_INFOTM_WIFI;
            break;
        case 158:
            scancode = SCANCODE_INFOTM_RESET;
            break;
#else
        case 108:
            scancode = SCANCODE_INFOTM_DOWN;
            break;
        case 103:
            scancode = SCANCODE_INFOTM_UP;
            break;
        case 352:
            scancode = SCANCODE_INFOTM_OK;
            break;
        case 116:
            scancode = SCANCODE_INFOTM_MODE;
            break;
#endif
        default:
            break;
    }
    return scancode;
}

#define REPORT_MENU		0x0001
#define REPORT_SHUT		(0x0001 << 1)
#define REPORT_RESET	(0x0001 << 2)
#define REPORT_SWITCH	(0x0001 << 4)
#define REPORT_ALWAYS	(0x0001 << 5)

int sign_key = 0;
int longpress_key = 0;
//sem_t sem_longpress;
pthread_mutex_t longpress_lock = PTHREAD_MUTEX_INITIALIZER;
int longpress_count = 0;

int keyup_count = 0;

/**
 * Hold wifi key and double press ok key, 
 * to enter otg device mode, for calibration or pcb_test
 **/
int double_press_flag = 0;
int discard_wifi_key = 0;

typedef struct _KeyEvent {
    int down;
    int scancode;
    struct timespec ts;
} SpvKeyEvent;
static SpvKeyEvent lastUpEvent;

static int CheckLongPressEvent()
{
    pthread_mutex_lock(&longpress_lock);
    switch(longpress_key)
    {
        /*case SCANCODE_INFOTM_UP:
          if(longpress_count > 4 && (longpress_count % 2)) {
        //report a key_up event every 200ms after 400ms
        dbg("[LONGPRESS_EVENT]report key event keycode=%d\n", longpress_key);
        sign_key |= REPORT_ALWAYS;
        KeyReportKeyUp(longpress_key);
        }
        break;*/
        case SCANCODE_INFOTM_UP:
            if(longpress_count == 30 && (~sign_key & REPORT_MENU)) {
                INFO("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_MENU\n");
                sign_key |= REPORT_MENU; 
                SpvReportKeyEvent(MSG_USER_KEYUP, SCANCODE_INFOTM_MENU, 0);
                longpress_key = 0;
            }
            break;
            /*case SCANCODE_INFOTM_OK:
              if(longpress_count == 8 && (~sign_key & REPORT_SWITCH)) {
              INFO("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_SWITCH\n");
              sign_key |= REPORT_SWITCH; 
              KeyReportKeyUp(SCANCODE_INFOTM_SWITCH);
              longpress_key = 0;
              }
              break;*/
#if defined(PRODUCT_LESHI_C23)
        case SCANCODE_INFOTM_OK:
#endif
        case SCANCODE_INFOTM_MODE:
            if(longpress_count == 60 && (~sign_key & REPORT_SHUT)) {
                INFO("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_SHUTDOWN\n");
                sign_key |= REPORT_SHUT;
                SpvReportKeyEvent(MSG_USER_KEYUP, SCANCODE_INFOTM_SHUTDOWN, 0);
                longpress_key = 0;
            }
            break;
            /*case SCANCODE_INFOTM_PHOTO:
              if(longpress_count == 8 && (~sign_key & REPORT_LOCK)) {
              INFO("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_LOCK\n");
              sign_key |= REPORT_LOCK; 
              KeyReportKeyUp(SCANCODE_INFOTM_LOCK);
              longpress_key = 0;
              }*/
            /* define reset key-event
               if(longpress_count == 50 && (~sign_key & REPORT_RESET)) {
               INFO("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_RESET\n");
               sign_key |= REPORT_RESET; 
               KeyReportKeyUp(SCANCODE_INFOTM_RESET);
               longpress_key = 0;
               }*/
            break;
        default :
            break;
    }
    longpress_count++;
#ifdef GUI_DOUBLE_CLICK_ENABLE
    if(lastUpEvent.scancode > 0) {
        if(keyup_count > 8) {
            SpvReportKeyEvent(MSG_USER_KEYUP, lastUpEvent.scancode, 0);
            INFO("no double click found, scancode: %d\n", lastUpEvent.scancode);
            memset(&lastUpEvent, 0, sizeof(SpvKeyEvent));
            keyup_count = 0;
			double_press_flag = 0;
        } else {
            keyup_count++;
        }
    } else {
        keyup_count = 0;
    }
#endif
    pthread_mutex_unlock(&longpress_lock);

    return 0;
}

/**
 * time lapse in ms
 **/
int GetTimeDiff(struct timespec begin, struct timespec end)
{
    if(begin.tv_sec <= 0 || end.tv_sec <= 0) {
        return -1;
    }

    int diff = ((end.tv_sec - begin.tv_sec) * 1000 + (end.tv_nsec - begin.tv_nsec) / 1000000);
    return diff;
}

static int g_adapter_removed = 0;
static int CheckSleepEvent()
{
    static int sleep = 0, delayedshutdown = 0, autoshutdown = 0;
    PSleepStatus pStatus = GetSleepStatus();
    struct timespec cs = {0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &cs);
    int idleTimeDiff = GetTimeDiff(pStatus->idleTime, cs) / 1000;//seconds
    int shutTimeDiff = GetTimeDiff(pStatus->dischargeTime, cs) / 1000;//seconds
    if(pStatus->idle) {
        if(pStatus->autoSleep > 0) {
            if(!sleep) {
                if(idleTimeDiff >= pStatus->autoSleep) {
                    sleep = 1;
                    SpvLcdBlank(0);
                    INFO("time to sleep\n");
                }
            } else {
                if(idleTimeDiff != -1 && idleTimeDiff < pStatus->autoSleep) {
                    sleep = 0;
                    SpvLcdBlank(1);
                    INFO("time to wakeup\n");
                }
            }
        }

        if(pStatus->autoPowerOff > 0 && !autoshutdown && SpvGetWifiStatus() == WIFI_OFF && !SpvIsOnLine()) {//!SpvGetChargingStatus()
            if(idleTimeDiff > pStatus->autoPowerOff) {
                autoshutdown = 1;
                SpvPostMessage(serverHandler, MSG_SHUTDOWN, 0, SHUTDOWN_AUTO);
            }
        }
    } else if(sleep) {
        sleep = 0;
        SpvLcdBlank(1);
    }

    if(g_adapter_removed && IsCardvEnable() && !delayedshutdown && pStatus->delayedShutdown > 0) {
        if(shutTimeDiff >= pStatus->delayedShutdown) {
            delayedshutdown = 1;
            SpvPostMessage(serverHandler, MSG_SHUTDOWN, 0, SHUTDOWN_DELAY);
        }
    }

    return 0;
}

static void* InfotmMonitorThread()
{
    static unsigned int loopCount = 0;
    sign_key = 0;
    prctl(PR_SET_NAME, __func__);
    while(loopServerRun) {
        if(loopCount % 10 == 0)
            CheckSleepEvent();
        if(loopCount % 1200 == 0 && SpvIsRecording()) {
            SpvCheckFreeSpace();
        }
        CheckLongPressEvent();
        loopCount++;
        usleep(50000);		//100ms
    }
}

static int SpvKeyHandler(int down, int keycode)
{
    INFO("key event, code: %d, down: %d\n", keycode, down);
    int msg = down ? MSG_USER_KEYDOWN : MSG_USER_KEYUP;
    int scancode = SpvKeyMap(keycode);
    if(scancode == -1) {
        return -1;
    }

    if(down) {
#ifdef GUI_DOUBLE_CLICK_ENABLE
        if(lastUpEvent.scancode > 0 && scancode != lastUpEvent.scancode && keyup_count < 4) {
            INFO("diff key down, reporting keyup event, lastscancode: %d\n", lastUpEvent.scancode);
            SpvReportKeyEvent(MSG_USER_KEYUP, lastUpEvent.scancode, 0);
            memset(&lastUpEvent, 0, sizeof(lastUpEvent));
            keyup_count = 0;
        }
#endif
		if(scancode == SCANCODE_INFOTM_WIFI) {
			double_press_flag = 1;
		}
        pthread_mutex_lock(&longpress_lock);
        longpress_key = scancode;
        longpress_count = 0;
        sign_key = 0;
        pthread_mutex_unlock(&longpress_lock);
        SpvReportKeyEvent(msg, scancode, 0);
    } else {
		if(SCANCODE_INFOTM_WIFI == scancode) {
			double_press_flag = 0;
		}
        pthread_mutex_lock(&longpress_lock);
        int discard = 0;
        switch(scancode)
        {
            case SCANCODE_INFOTM_UP:
                if(sign_key & REPORT_MENU) {
                    INFO("discard down event\n");
                    discard = 1;
                }
                break;
#if defined(PRODUCT_LESHI_C23)
            case SCANCODE_INFOTM_OK:
#endif
            case SCANCODE_INFOTM_MODE:
                if(sign_key & REPORT_SHUT) {
                    INFO("discard mode event\n");
                    discard = 1;
                }
                break;
            default:
                break;
        }
        longpress_key = 0;
        longpress_count = 0;
        sign_key = 0;
        pthread_mutex_unlock(&longpress_lock);
#ifndef GUI_DOUBLE_CLICK_ENABLE
        if(!discard) {
            SpvReportKeyEvent(msg, scancode, 0);
        }
#else
        if(!discard) {
            struct timespec tmpts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &tmpts);
            if(lastUpEvent.scancode > 0 && scancode == lastUpEvent.scancode && GetTimeDiff(lastUpEvent.ts, tmpts) < 500) {
                INFO("double click detected---------, scancode: %d\n", scancode);
                switch(scancode) {
#if defined(PRODUCT_LESHI_C23)
                    case SCANCODE_INFOTM_OK:
						if(!double_press_flag) {
							SpvReportKeyEvent(msg, SCANCODE_INFOTM_MODE, 0);
						} else {
							//wifi key hold on, double click of ok key, enter device mode
							INFO("---------change to device mode\n");
							double_press_flag = 0;
							discard_wifi_key = 1;
                            SpvEnterCalibrationMode();
						}
						break;
                    case SCANCODE_INFOTM_WIFI:
						double_press_flag = 0;
                        SpvReportKeyEvent(msg, SCANCODE_INFOTM_WIFI, 1);
                        break;
#endif
                    default:
                        SpvReportKeyEvent(msg, scancode, 0);
                        break;
                }
                memset(&lastUpEvent, 0, sizeof(SpvKeyEvent));
			} else if(scancode == SCANCODE_INFOTM_WIFI && discard_wifi_key) {
				discard_wifi_key = 0;
				INFO("wifi up discard for device mode\n");
            } else {
                lastUpEvent.scancode = scancode;
                lastUpEvent.down = 0;
                clock_gettime(CLOCK_MONOTONIC_RAW, &lastUpEvent.ts);
            }
        } else {
            memset(&lastUpEvent, 0, sizeof(SpvKeyEvent));
        }
        keyup_count = 0;
#endif
    }
    return 0;
}

void SysEventHandler(char *name, void*args)
{
	if((NULL == name) || (NULL == args)) {
		INFO("invalid parameter!\n");
		return;
	}

    INFO("Got event: %s\n", name);
    if(!strcasecmp(name, EVENT_KEY)) {
        struct event_key *key = args;
        SpvKeyHandler(key->down, key->key_code);
    } else if(!strcasecmp(name, EVENT_ACCELEROMETER)) {
	    struct event_gsensor *g_sensor_cp = args;
	    INFO("G-sensor event collision %d\n", g_sensor_cp->collision);
		SpvPostMessage(serverHandler, MSG_COLLISION, 0, 0);
    } else if(!strcasecmp(name, EVENT_MOUNT)) {
        //struct event_mount *em = args;
        INFO("mount event, args: %s\n", args);
        if(!strncasecmp(args, "/mnt/udisk", 10)) {
            INFO("udisk inserted\n");
            extern int usb_host_pass;
            usb_host_pass = 1;
            SpvSetLed(LED_WIFI, 100, 100, 1);
        } else {
            SpvPostMessage(serverHandler, MSG_SDCARD_MOUNT, SD_STATUS_MOUNTED, 0);
        }
    } else if(!strcasecmp(name, EVENT_UNMOUNT)) {
        INFO("umount event, args: %s\n", args);
        if(!strncasecmp(args, "/mnt/udisk", 10)) {
            SpvSetLed(LED_WIFI, 0, 0, 0);
        } else {
            SpvPostMessage(serverHandler, MSG_SDCARD_MOUNT, SD_STATUS_UMOUNT, 0);
        }
    } else if(!strcasecmp(name, EVENT_BATTERY_ST)) {
	    struct event_battery_st *battery_st_cp = args;
	    INFO("=====battery status event status: %s\n", battery_st_cp->battery_st);
        int chargeStatus = -1;
        if(!strncasecmp(battery_st_cp->battery_st, "Full", 4)) {
            chargeStatus = BATTERY_FULL;
			//SpvIndication(IND_CHARGE_FULL, 0, 0);
        } else if(!strncasecmp(battery_st_cp->battery_st, "Discharging", 11)) {
            chargeStatus = BATTERY_DISCHARGING;
            g_adapter_removed = 1;
			//SpvIndication(IND_DISCHARGE, 0, 0);
        } else if(!strncasecmp(battery_st_cp->battery_st, "Charging", 8)) {
            chargeStatus = BATTERY_CHARGING;
			g_adapter_removed = 0;
			//SpvIndication(IND_CHARGE, 0, 0);
        }
        if(chargeStatus != -1) {
            SpvPostMessage(serverHandler, MSG_CHARGE_STATUS, chargeStatus, 0);
        }
    } else if(!strcasecmp(name, EVENT_BATTERY_CAP)) {
        struct event_battery_cap *battery_cap_cp = args;
        //SpvSetBatteryLed(battery_cap_cp->battery_cap_cur, SpvGetChargingStatus());
        SpvPostMessage(serverHandler, MSG_BATTERY_STATUS, battery_cap_cp->battery_cap_cur, 0);
	    //INFO("=====battery capacity event status %d\n", battery_cap_cp->battery_cap_cur);
		//if(battery_cap_cp->battery_cap_cur <= BATTERY_DIV && g_adapter_removed)
    } else if(!strcasecmp(name, EVENT_CHARGING)) {
	    struct event_charge *charge_cp = args;
	    INFO("=====charge status event status %d\n", charge_cp->charge_st_cur);
        UpdateIdleTime();
        if(charge_cp->charge_st_cur == 0) {//update discharge time in status
            PSleepStatus pStatus = GetSleepStatus();
            clock_gettime(CLOCK_MONOTONIC_RAW, &pStatus->dischargeTime);
            g_adapter_removed = 1;
        } else {
            g_adapter_removed = 0;
        }
    } else if(!strcasecmp(name, EVENT_LIGHTSENCE_D)) {
    } else if(!strcasecmp(name, EVENT_LIGHTSENCE_B)) {
    } else if(!strcasecmp(name, EVENT_STORAGE_INSERT)) {
	    char *path = args;
	    INFO("EVENT_STORAGE_INSERT %s\n", path);
    } else if(!strcasecmp(name, EVENT_STORAGE_REMOVE)) {
	    char *path = args;
	    INFO("EVENT_STORAGE_REMOVE %s\n", path);
        //SpvPostMessage(serverHandler, MSG_SDCARD_MOUNT, SD_STATUS_UMOUNT, 0);
    } else if(!strcasecmp(name, EVENT_USB_HOST_STATUS)) {
        INFO("Got event: usb_host_status, status: %s\n", args);
        char *status = args;
        if(!strncasecmp(status, "connected", 9)) {
            SpvPostMessage(serverHandler, MSG_ONLINE, 1, 0);
        } else if(!strncasecmp(status, "disconnected", 12)) {
            SpvPostMessage(serverHandler, MSG_ONLINE, 0, 0);
        }
    }
}

int InitInfotmServer(HMSG h)
{
	serverHandler = h;
#if 0
	int err, ret, hd;
	char buf[16];

	loopServerRun = 1;
	if(!access(ACC_NODE, F_OK))
		ACC_ENABLE  = 1;

	err = pthread_create(&__info_server, NULL, InfotmServerLoop, NULL);
	if(err != 0) {
		ERROR("Create InfotmServerLoop thread failed!\n");
		return -1;
	}

#ifdef GPS_EVENT
	pthread_attr_t attr;
	pthread_t t;
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&t, &attr, (void *)GPSUpdateLoop, NULL);
	pthread_attr_destroy (&attr);
#endif
#else
    loopServerRun = 1;
    //sem_init(&sem_longpress, 0, 1);
    SpvCreateServerThread(InfotmMonitorThread, NULL);

	event_register_handler(EVENT_KEY, 0, SysEventHandler);
	event_register_handler(EVENT_ACCELEROMETER, 0, SysEventHandler);
	event_register_handler(EVENT_MOUNT, 0, SysEventHandler);
	event_register_handler(EVENT_UNMOUNT, 0, SysEventHandler);
	event_register_handler(EVENT_BATTERY_ST, 0, SysEventHandler);
	event_register_handler(EVENT_BATTERY_CAP, 0, SysEventHandler);
	event_register_handler(EVENT_CHARGING, 0, SysEventHandler);
	event_register_handler(EVENT_LIGHTSENCE_D, 0, SysEventHandler);
	event_register_handler(EVENT_LIGHTSENCE_B, 0, SysEventHandler);
	event_register_handler(EVENT_STORAGE_INSERT, 0, SysEventHandler);
	event_register_handler(EVENT_STORAGE_REMOVE, 0, SysEventHandler);
	event_register_handler(EVENT_USB_HOST_STATUS, 0, SysEventHandler);
#endif
	return 0;
}

void TerminateInfotmServer()
{
	loopServerRun = 0;
	if(pthread_cancel(__info_server) != 0) {
		ERROR("Cancel the infotm server thread failed!\n");
	}
	pthread_join(__info_server, NULL);
}
