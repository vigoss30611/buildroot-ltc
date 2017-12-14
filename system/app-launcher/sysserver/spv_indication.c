
/**
 * Launcher indicator, contains led speaker and buzzer
 **/
#include <sys/prctl.h>

#include "spv_debug.h"
#include "spv_indication.h"
#include "spv_utils.h"

static void FlickWorkLedForPhoto(int burst)
{
    prctl(PR_SET_NAME, __func__);
    if(IsSdcardFull()) {
        //SpvSetLed(LED_WORK, 500, 500, 1);
        return;
    }
    SpvSetLed(LED_WORK, 1, 0, 0);
    SpvSetLed(LED_WORK1, 1, 0, 0);
    usleep(250000);
    SpvSetLed(LED_WORK, 0, 0, 0);
    SpvSetLed(LED_WORK1, 0, 0, 0);
    usleep(250000);
    SpvSetLed(LED_WORK, 1, 0, 0);
    SpvSetLed(LED_WORK1, 1, 0, 0);
    usleep(250000);
    if(!SpvIsInVideoMode()) {
        SpvSetLed(LED_WORK, 0, 0, 0);
        SpvSetLed(LED_WORK1, 0, 0, 0);
    } else if(SpvIsRecording()) {
        SpvSetLed(LED_WORK, 500, 500, 1);
        SpvSetLed(LED_WORK1, 500, 500, 1);
    } else {
        SpvSetLed(LED_WORK, 1, 0, 0);
        SpvSetLed(LED_WORK1, 1, 0, 0);
    }
}

static void ShutdownIndication(int reason)
{
    BuzzerUnit *unit = (BuzzerUnit *)malloc(sizeof(BuzzerUnit));
    memset(unit, 0, sizeof(BuzzerUnit));
    unit->freq = 2730;
    unit->duration = 200;
    unit->interval = 100;
    unit->times = 2;

    int led_repeat = 0;
    switch(reason) {
        case SHUTDOWN_POWER_LOW:
            unit->times = 2;
            led_repeat = 3;
            break;
        case SHUTDOWN_AUTO:
            unit->interval = 0;
            unit->times = 1;
            led_repeat = 3;
            break;
        case SHUTDOWN_KEY:
            unit->times = 2;
            break;
    }
    SpvPlayAudio(unit);

    SpvSetLed(LED_BATTERY, 0, 0, 0);
    SpvSetLed(LED_BATTERY_G, 0, 0, 0);
    SpvSetLed(LED_BATTERY_B, 0, 0, 0);
    while(led_repeat) {
        SpvSetLed(LED_BATTERY, 1, 0, 0);
        usleep(250000);
        SpvSetLed(LED_BATTERY, 0, 0, 0);
        usleep(250000);

        led_repeat--;
    }

/*  SpvSetLed(LED_WORK, 0, 0, 0);
    SpvSetLed(LED_WORK1, 0, 0, 0);
    SpvSetLed(LED_WIFI, 0, 0, 0);*/
}

void SpvSetBatteryLed(int capacity, int charging)
{
    static int ind_bat_last = -1;
    int ind_bat = -1;
    //INFO("capacity: %d, charging: %d\n", capacity, charging);
    if(charging) {
        if(capacity == 100) {
            ind_bat = IND_CHARGE_FULL;
        } else {
            ind_bat = IND_CHARGE;
        }
    } else {
        if(capacity > 75) {
            ind_bat = IND_POWER_FULL;
        } else if(capacity > 50) {
            ind_bat = IND_POWER_HIGH;
        } else if(capacity > 20) {
            ind_bat = IND_POWER_MIDDLE;
        } else if(capacity > 10) {
            ind_bat = IND_POWER_LOW;
        } else {
            ind_bat = IND_POWER_EMPTY;
        }
    }
    if(ind_bat != ind_bat_last) {
        ind_bat_last = ind_bat;
        SpvIndication(ind_bat, 0, 0);
    }
}

int SpvIndication(IND_TYPE type, int wparam, int lparam)
{
#if defined(PRODUCT_LESHI_C23)
    BuzzerUnit *unit = (BuzzerUnit *)malloc(sizeof(BuzzerUnit));
    memset(unit, 0, sizeof(BuzzerUnit));
    switch(type) {
        case IND_VIDEO_MODE:
            if(!wparam) {
                unit->freq = 2730;
                unit->duration = 200;
                unit->interval = 0;
                unit->times = 1;
            }
            SpvSetLed(LED_WORK, 1, 0, 0);
            SpvSetLed(LED_WORK1, 1, 0, 0);
            break;
        case IND_VIDEO_START:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 0;
            unit->times = 1;
            SpvSetLed(LED_WORK, 500, 500, 1);
            SpvSetLed(LED_WORK1, 500, 500, 1);
            break;
        case IND_VIDEO_END:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 100;
            unit->times = 2;
            SpvSetLed(LED_WORK, 1, 0, 0);
            SpvSetLed(LED_WORK1, 1, 0, 0);
            break;
        case IND_PHOTO_MODE:
            if(!wparam) {
                unit->freq = 2730;
                unit->duration = 200;
                unit->interval = 0;
                unit->times = 1;
            }
            SpvSetLed(LED_WORK, 0, 0, 0);
            SpvSetLed(LED_WORK1, 0, 0, 0);
            break;
        case IND_PHOTO:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 0;
            unit->times = 1;
            SpvCreateServerThread((void *)FlickWorkLedForPhoto, 0);
            //SpvSetLed(LED_WORK, 500, 500, 1);
            break;
        case IND_PHOTO_BURST:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 100;
            unit->times = 3;
            SpvCreateServerThread((void *)FlickWorkLedForPhoto, 3);
            //SpvSetLed(LED_WORK, 500, 500, 1);
            break;
        case IND_SDCARD_FULL:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 500;
            unit->times = 3;
            SpvSetLed(LED_WORK, 250, 250, 1);
            SpvSetLed(LED_WORK1, 250, 250, 1);
            break;
        case IND_BOOTING_UP:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 100;
            unit->times = 2;
            break;
        case IND_SHUTDOWN:
            /*unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 100;
            unit->times = 2;*/
            free(unit);
            ShutdownIndication(wparam);
            //SpvCreateServerThread((void *)ShutdownIndication, wparam);
            return;
        /*case IND_AUTO_SHUTDOWN:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 0;
            unit->times = 1;
            break;*/
        case IND_POWER_FULL:
        case IND_POWER_HIGH:
        case IND_POWER_MIDDLE:
			SpvSetLed(LED_BATTERY, 0, 0, 0);
			SpvSetLed(LED_BATTERY_B, 0, 0, 0);
			SpvSetLed(LED_BATTERY_G, 1, 0, 0);
            break;
        /*case IND_POWER_HIGH:
        case IND_POWER_MIDDLE:
			SpvSetLed(LED_BATTERY, 1, 0, 0);
			SpvSetLed(LED_BATTERY_B, 0, 0, 0);
			SpvSetLed(LED_BATTERY_G, 1, 0, 0);
            break;
        case IND_POWER_LOW:
			SpvSetLed(LED_BATTERY, 0, 0, 0);
			SpvSetLed(LED_BATTERY_B, 1, 0, 0);
			SpvSetLed(LED_BATTERY_G, 0, 0, 0);
            break;*/
        case IND_POWER_LOW:
        case IND_POWER_EMPTY:
            SpvSetLed(LED_BATTERY, 1, 1, 0);
			SpvSetLed(LED_BATTERY_B, 0, 0, 0);
			SpvSetLed(LED_BATTERY_G, 0, 0, 0);
            break;
        /*case IND_POWER_EMPTY:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 0;
            unit->times = 1;
			SpvSetLed(LED_BATTERY, 350, 350, 1);
			SpvSetLed(LED_BATTERY_B, 0, 0, 0);
			SpvSetLed(LED_BATTERY_G, 0, 0, 0);
            break;
		case IND_DISCHARGE:
			SpvSetLed(LED_BATTERY, 0, 0, 0);
			SpvSetLed(LED_BATTERY_B, 0, 0, 0);
			SpvSetLed(LED_BATTERY_G, 0, 0, 0);
			break;*/
		case IND_CHARGE:
			SpvSetLed(LED_BATTERY, 1, 0, 0);
			SpvSetLed(LED_BATTERY_B, 0, 0, 0);
			SpvSetLed(LED_BATTERY_G, 0, 0, 0);
			break;
		case IND_CHARGE_FULL:
			SpvSetLed(LED_BATTERY, 0, 0, 0);
			SpvSetLed(LED_BATTERY_B, 0, 0, 0);
			SpvSetLed(LED_BATTERY_G, 1, 0, 0);
			break;
        case IND_WIFI_ON:
            SpvSetLed(LED_WIFI, 500, 500, 1);
            break;
        case IND_WIFI_OFF:
            SpvSetLed(LED_WIFI, 0, 0, 0);
            break;
        case IND_WIFI_CONNECT:
            SpvSetLed(LED_WIFI, 1, 0, 0);
            break;
        case IND_WIFI_DISCONNECT:
            SpvSetLed(LED_WIFI, 500, 500, 1);
            break;
        case IND_WIFI_SWITCH:
            break;
        case IND_WIFI_AIRKISS:
            break;
        case IND_CAMERA_ERROR:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 0;
            unit->times = 1;
            //SpvSetLed(LED_WORK, 500, 500, 1);
            break;
        case IND_CAMERA_RESET:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 0;
            unit->times = 1;
            //SpvSetLed(LED_WORK, 500, 500, 1);
            break;
		case IND_DEVICE_MODE:
            unit->freq = 2730;
            unit->duration = 200;
            unit->interval = 100;
            unit->times = 9;
            SpvSetLed(LED_WORK, 500, 500, 1);
            SpvSetLed(LED_WORK1, 500, 500, 1);
            SpvSetLed(LED_WIFI, 500, 500, 1);
            SpvSetLed(LED_BATTERY, 200, 900, 1);
            usleep(300000);
            SpvSetLed(LED_BATTERY_G, 200, 900, 1);
            usleep(300000);
            SpvSetLed(LED_BATTERY_B, 200, 900, 1);
			break;
        case IND_FCWS_WARN:
            break;
        case IND_LDWS_WARN:
            break;
        default:
            break;
    }
    if(unit->duration) {
        SpvPlayAudio((int)unit);
    } else {
        free(unit);
    }
#else
    switch(type) {
        case IND_KEY:
            if(SpvKeyToneEnabled())
                SpvPlayAudio(WAV_KEY);
            break;
        case IND_VIDEO_START:
            SpvSetLed(LED_WIFI, 500, 500, 1);
            break;
        case IND_VIDEO_END:
            SpvSetLed(LED_WIFI, 1, 0, 0);
            break;
        case IND_PHOTO:
            SpvPlayAudio(WAV_PHOTO);
            break;
        case IND_PHOTO_BURST:
            SpvPlayAudio(WAV_PHOTO3);
            break;
        case IND_WIFI_ON:
            SpvSetLed(LED_WIFI, 500, 500, 1);
            break;
        case IND_WIFI_OFF:
            SpvSetLed(LED_WIFI, 0, 0, 0);
            break;
        case IND_WIFI_CONNECT:
            SpvSetLed(LED_WIFI, 1, 0, 0);
            break;
        case IND_WIFI_DISCONNECT:
            SpvSetLed(LED_WIFI, 500, 500, 1);
            break;
		case IND_BOOTING_UP:
        case IND_SHUTDOWN:
            SpvPlayAudio(WAV_BOOTING);
            break;
        default:
            break;
    }
#endif
}
