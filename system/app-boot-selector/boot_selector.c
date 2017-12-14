#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/reboot.h>
#include <sys/prctl.h>

#include "bs_message.h"
#include "bs_debug.h"

#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <qsdk/cep.h>

pthread_t __info_server;
static int loopServerRun = 1;

#if 0
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
#endif

static int SpvKeyMap(int keycode)
{
    int scancode = -1;
    switch(keycode) {
        case 116:
            scancode = SCANCODE_INFOTM_OK;
            break;
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

static int CheckLongPressEvent()
{
    pthread_mutex_lock(&longpress_lock);
    switch(longpress_key)
    {
        case SCANCODE_INFOTM_OK:
            if(longpress_count == 60 && (~sign_key & REPORT_SHUT)) {
                INFO("[LONGPRESS_EVENT]report key event boot up\n");
                sign_key |= REPORT_SHUT;
                //here we exit the blocked process, goto booting up
                exit(0);
                //SpvReportKeyEvent(MSG_USER_KEYUP, SCANCODE_INFOTM_SHUTDOWN, 0);
                longpress_key = 0;
            }
            break;
        default :
            break;
    }
    longpress_count++;
    pthread_mutex_unlock(&longpress_lock);

    return 0;
}
#if 0
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
#endif

static void* BootMonitorLoop()
{
    static unsigned int loopCount = 0;
    sign_key = 0;
    prctl(PR_SET_NAME, __func__);
    while(loopServerRun) {
        CheckLongPressEvent();
        loopCount++;
        usleep(50000);		//100ms
    }
}

static int SpvKeyHandler(int down, int keycode)
{
    INFO("key event, code: %d, down: %d\n", keycode, down);
    int msg = down ? 1 : 0;
    int scancode = SpvKeyMap(keycode);
    if(scancode == -1) {
        return -1;
    }

    if(down) {
        pthread_mutex_lock(&longpress_lock);
        longpress_key = scancode;
        longpress_count = 0;
        sign_key = 0;
        pthread_mutex_unlock(&longpress_lock);
        //SpvReportKeyEvent(msg, scancode, 0);
    } else {
        pthread_mutex_lock(&longpress_lock);
        int discard = 0;
        switch(scancode)
        {
            case SCANCODE_INFOTM_OK:
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
        if(!discard) {
            //SpvReportKeyEvent(msg, scancode, 0);
        }
    }
    return 0;
}

static int BsSetBatteryLed(int capacity)
{
    static int last_status = -1;
    int status = 0;
    if(capacity == 100) {
        status = 1;
    }

    if(last_status != status) {
        last_status = status;
        if(status) {
            system_set_led("led_battery", 0, 0, 0);
            system_set_led("led_battery_g", 1, 0, 0);
        } else {
            system_set_led("led_battery", 1, 0, 0);
            system_set_led("led_battery_g", 0, 0, 0);
        }
        system_set_led("led_battery_b", 0, 0, 0);
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
    } else if(!strcasecmp(name, EVENT_BATTERY_ST)) {
	    struct event_battery_st *battery_st_cp = args;
	    INFO("=====battery status event status: %s\n", battery_st_cp->battery_st);
    } else if(!strcasecmp(name, EVENT_BATTERY_CAP)) {
        struct event_battery_cap *battery_cap_cp = args;
	    INFO("=====battery capacity event status %d\n", battery_cap_cp->battery_cap_cur);
        BsSetBatteryLed(battery_cap_cp->battery_cap_cur);
    } else if(!strcasecmp(name, EVENT_CHARGING)) {
	    struct event_charge *charge_cp = args;
	    INFO("=====charge status event status %d\n", charge_cp->charge_st_cur);
        if(charge_cp->charge_st_cur == 0) {//update discharge time in status
            INFO("charger has been removed, poweroff\n");
	        reboot(RB_POWER_OFF);
        }
    }
}

unsigned long int CreateServerThread(void (*func(void *arg)), void *args)
{
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    INFO("pthread_create\n");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr, func, args);
    pthread_attr_destroy (&attr);
    return t;
}


int main(int argc, char *argv[])
{
    loopServerRun = 1;


    enum boot_reason reason = system_get_boot_reason();
    switch(reason) {
        case BOOT_REASON_ACIN:
            INFO("power up by ac in\n");
            break;
        case BOOT_REASON_EXTINT:
            INFO("power up by ac extern irq\n");
            break;
        case BOOT_REASON_POWERKEY:
            INFO("power up by press power key\n");
            break;
        case BOOT_REASON_UNKNOWN:
        default:
            INFO("power up reason unknown\n");
            break;
    }

    if(reason != BOOT_REASON_ACIN) {
        exit(0);
    }

    ac_info_t aci;
    system_get_ac_info(&aci);
    if(!aci.online) {
        INFO("power up by ac in, but now adapter removed, we need to poweroff");
	    reboot(RB_POWER_OFF);
        //exit(0);
    }
    system_set_led("led_work", 0, 0, 0);
    system_set_led("led_work1", 0, 0, 0);

    battery_info_t bati;
    system_get_battry_info(&bati);
    BsSetBatteryLed(bati.capacity);

	event_register_handler(EVENT_KEY, 0, SysEventHandler);
	event_register_handler(EVENT_ACCELEROMETER, 0, SysEventHandler);
	event_register_handler(EVENT_BATTERY_ST, 0, SysEventHandler);
	event_register_handler(EVENT_BATTERY_CAP, 0, SysEventHandler);
	event_register_handler(EVENT_CHARGING, 0, SysEventHandler);

    BootMonitorLoop();
	return 0;
}

