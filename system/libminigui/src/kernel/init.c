/* 
** $Id: init.c 13736 2011-03-04 06:57:33Z wanzheng $
**
** init.c: The Initialization/Termination routines for MiniGUI-Threads.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/11/05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>

#include "common.h"
#include "minigui.h"

#ifndef __NOUNIX__
#include <unistd.h>
#include <signal.h>
#include <sys/termios.h>
#endif

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "ial.h"
#include "internals.h"
#include "ctrlclass.h"
#include "cursor.h"
#include "event.h"
#include "misc.h"
#include "menu.h"
#include "timer.h"
#include "accelkey.h"
#include "clipboard.h"
#include "license.h"
#include "sys/stat.h"

#include "spv_ui_status.h"
#include "lcd_backlight.h"
#include "power.h"
#include "sd_stuff.h"
#include "led.h"
#include "camera_spv.h"
#include "gps.h"

pthread_t __info_server;
pthread_t __info_monkey;

#define MONKEY_TEST

int __mg_quiting_stage;

#ifdef _MGRM_THREADS
int __mg_enter_terminategui;

HWND GetMainWindow();

int DEBUG_ENABLED = 1;

int ROTATE_SCREEN = 0;

int GPS_ENABLE = 0;

//distance of  sliding, larger value means shorter distance
static const int sli_dist = 10;

/***********backlight_controller by jeason*****************/
int bl_status = 0;
int bl_brightness = 255;

#if 0
void setBacklight(int blank)			//0:turn on backlight; 1:turn off backlight
{
	printf("[BACKLIGHT]Try to control backlight blank=%d\n", blank);
	//lcd_blank(blank);		//don`t have result, >>maybe cause some error<<
	bl_status = blank;
	printf("[BACKLIGHT]finish control backlight value\n");
	return;
}

int setBacklightBrightness(int value)
{
	int ret = set_backlight(value);
	if(ret < 0) {
		printf("[BACKLIGHT]Set backlight %d failed! ret:%d\n", value, ret);
		return ret;
	}
	return 0;
}

int getBacklightBrightness()
{
	int ret = get_backlight();
	if(ret < 0) {
		printf("[BACKLIGHT]Get backlight failed! ret:%d\n", ret);
	}
	return ret;
}
/************************** end ***************************/
#endif
/******************************* extern data *********************************/
extern void* DesktopMain (void* data);

/************************* Entry of the thread of parsor *********************/
static void ParseEvent (PLWEVENT lwe)
{
    PMOUSEEVENT me;
    PKEYEVENT ke;
    MSG Msg;
    static int mouse_x = 0, mouse_y = 0;

//added content about scrolling&sliding by Zhen
    static unsigned int scrolltime;
	static unsigned int preTime;
    static int scrollX;
    static int scrollY;
    static int slidingX;
    static int slidingY;
	
    int deltX;
    int deltY;
    int direction = 0;
    int velocity = 0;
//end of content about scrolling%sliding

    ke = &(lwe->data.ke);
    me = &(lwe->data.me);
    Msg.hwnd = HWND_DESKTOP;
    Msg.wParam = 0;
    Msg.lParam = 0;

    //printf("ParseEvent lwe->type:%d\n", lwe->type);

    if (lwe->type == LWETYPE_TIMEOUT) {
        Msg.message = MSG_TIMEOUT;
        Msg.wParam = (WPARAM)lwe->count;
        Msg.lParam = 0;
        QueueDeskMessage (&Msg);
    } else if (lwe->type == LWETYPE_KEY) {
        Msg.wParam = ke->scancode;
        Msg.lParam = ke->status;
        if(ke->event == KE_KEYDOWN){
            Msg.message = MSG_KEYDOWN;
			printf("\n[KEY]Send keydown-event code=%d\n\n", Msg.wParam);
        }
        else if (ke->event == KE_KEYUP) {
            Msg.message = MSG_KEYUP;
			printf("\n[KEY]Send keyup-event code=%d, lParam=%d\n\n", Msg.wParam, Msg.lParam);
        }
        else if (ke->event == KE_KEYLONGPRESS) {
            Msg.message = MSG_KEYLONGPRESS;
        }
        else if (ke->event == KE_KEYALWAYSPRESS) {
            Msg.message = MSG_KEYALWAYSPRESS;
        }
        QueueDeskMessage (&Msg);
    } else if(lwe->type == LWETYPE_MOUSE) {
        Msg.wParam = me->status;
        Msg.lParam = MAKELONG (me->x, me->y);

        switch (me->event) {
        case ME_MOVED:
            Msg.message = MSG_MOUSEMOVE;
            break;
        case ME_LEFTDOWN:
            Msg.message = MSG_LBUTTONDOWN;
            scrolltime = __mg_timer_counter;
			preTime = __mg_timer_counter;
            scrollX = me->x;
            scrollY = me->y;
            slidingX = me->x;
            slidingY = me->y;
            break;
        case ME_LEFTUP:
            Msg.message = MSG_LBUTTONUP;
            break;
        case ME_LEFTDBLCLICK:
            Msg.message = MSG_LBUTTONDBLCLK;
            break;
        case ME_RIGHTDOWN:
            Msg.message = MSG_RBUTTONDOWN;
            break;
        case ME_RIGHTUP:
            Msg.message = MSG_RBUTTONUP;
            break;
        case ME_RIGHTDBLCLICK:
            Msg.message = MSG_RBUTTONDBLCLK;
            break;
        }
        QueueDeskMessage (&Msg);
#if 0
        if (me->event != ME_MOVED && (mouse_x != me->x || mouse_y != me->y)) {
            int old = Msg.message;

            Msg.message = MSG_MOUSEMOVE;
            QueueDeskMessage (&Msg);
            Msg.message = old;

            mouse_x = me->x; mouse_y = me->y;
        }
#endif
// added content about scrolling&sliding by Zhen
        if(Msg.message == MSG_MOUSEMOVE)
        {
            if(__mg_timer_counter >(scrolltime+10))
            {                 
                
                deltX = me->x - scrollX;
                deltY = me->y - scrollY;
                scrolltime = __mg_timer_counter;  
                scrollX = me->x;
                scrollY = me->y;
                
                if(abs(deltX) > abs(deltY)) 
                {    
                    velocity = abs(deltX);   
                    if(deltX>0)
                        direction = INFOTMIC_MOUSE_MOVE_RIGHT;  
                    else
                        direction = INFOTMIC_MOUSE_MOVE_LEFT;             
                }
                else   
                {
                    velocity = abs(deltY);
                    if(deltY>0)
                        direction = INFOTMIC_MOUSE_MOVE_DOWN; 
                    else
                        direction = INFOTMIC_MOUSE_MOVE_UP; 
                }    

				switch(direction){
					case INFOTMIC_MOUSE_MOVE_RIGHT:
						break;
					case INFOTMIC_MOUSE_MOVE_LEFT:
						break;
					case INFOTMIC_MOUSE_MOVE_DOWN:
						break;
					case INFOTMIC_MOUSE_MOVE_UP:
						break;						
				}
				
                Msg.message = INFOTMIC_MSG_MOUSE_SCROLLING;
                Msg.wParam  = MAKELONG (direction, velocity/sli_dist);
                Msg.lParam  = MAKELONG (me->x, me->y);  
                QueueDeskMessage (&Msg);
            }
        }
        
        if(Msg.message == MSG_LBUTTONUP)
        {
            deltX = me->x - slidingX;
            deltY = me->y - slidingY; 
            if(abs(deltX)>20 || abs(deltY)>20)
            {
                if(abs(deltX) > abs(deltY))  
                {    
                    velocity = abs(deltX);   
                    if(deltX>0)
                        direction = INFOTMIC_MOUSE_MOVE_RIGHT; 
                    else
                        direction = INFOTMIC_MOUSE_MOVE_LEFT;                      
                }
                else  
                {
                    velocity = abs(deltY);
                    if(deltY>0)
                        direction = INFOTMIC_MOUSE_MOVE_DOWN; 
                    else
                        direction = INFOTMIC_MOUSE_MOVE_UP;
                }      
				
				switch(direction){
					case INFOTMIC_MOUSE_MOVE_RIGHT:
						break;
					case INFOTMIC_MOUSE_MOVE_LEFT:
						break;
					case INFOTMIC_MOUSE_MOVE_DOWN:
						break;
					case INFOTMIC_MOUSE_MOVE_UP:
						break;						
				}
				
                Msg.message = INFOTMIC_MSG_MOUSE_SLIDING;
                Msg.wParam  = MAKELONG (direction, velocity/sli_dist);
                Msg.lParam  = MAKELONG (me->x, me->y);

//				struct timeval timeval;
//				gettimeofday(&timeval, NULL);
//				int64_t startTime = timeval.tv_sec * 1000 + timeval.tv_usec / 1000;
//				int64_t newTime = timeval.tv_sec * 1000 + timeval.tv_usec / 1000;
//				int64_t preTime = startTime;
//				while(((newTime - startTime) <= sliding_duration)){					
//					if(newTime - preTime >= time_rec_max){
						QueueDeskMessage (&Msg);
//						preTime = newTime;
//					}
//					gettimeofday(&timeval, NULL);
//					newTime = timeval.tv_sec * 1000 + timeval.tv_usec / 1000;
//				}
									
            }
        }
//end of content about scrolling&sliding by Zhen    
        //printf("ParseEvent->QueueDeskMessage message:%d\n", Msg.message);
     //   QueueDeskMessage (&Msg);
    }
}


/***********add for long press by jeason*****************/
#define LONGPRESS_DEBUG
#ifdef LONGPRESS_DEBUG
#define DEBUG_KEY(fmt, args...) if(DEBUG_ENABLED) printf(fmt, ##args)
#else
#define DEBUG_KEY(fmt, ...)
#endif

#define REPORT_MODE		1
#define REPORT_SHUT		2
#define REPORT_RESET	4
#define REPORT_ALWAYS	8
#define REPORT_SWITCH	16
#define REPORT_LOCK	    32

static void infotmReportKeyUp(int keycode)
{
	LWEVENT lwe;

	lwe.type = LWETYPE_KEY;
	lwe.data.ke.event = KE_KEYUP;
	lwe.data.ke.scancode = keycode;
	lwe.data.ke.status = 1;
	if(keycode == SCANCODE_INFOTM_POWER) {
		DEBUG_KEY("power key change LCD backlight\n");
		//setBacklight(!bl_status);
	}
	ParseEvent (&lwe);
}

pthread_t __info_longpress;
int sign_key = 0;
int longpress_num = 0;
int longpress_key = 0;

static void* infotmLongPress(void* data)
{
	sign_key = 0;
	while(__mg_quiting_stage > _MG_QUITING_STAGE_EVENT) {
		
		sem_wait((sem_t*)data);
		switch(longpress_key)
		{
			case SCANCODE_INFOTM_DOWN:
			case SCANCODE_INFOTM_UP:
				if(longpress_num > 4 && (longpress_num % 2)) {
					//report a key_up event every 200ms after 400ms
					DEBUG_KEY("[LONGPRESS_EVENT]report key event keycode=%d\n", longpress_key);
					sign_key |= REPORT_ALWAYS;
					infotmReportKeyUp(longpress_key);
				}
				break;
			case SCANCODE_INFOTM_MENU:
				if(longpress_num == 8 && (~sign_key & REPORT_MODE)) {
					DEBUG_KEY("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_MODE\n");
					sign_key |= REPORT_MODE; 
					infotmReportKeyUp(SCANCODE_INFOTM_MODE);
					longpress_key = 0;
				}
				break;
			case SCANCODE_INFOTM_OK:
                if(longpress_num == 8 && (~sign_key & REPORT_SWITCH)) {
					DEBUG_KEY("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_SWITCH\n");
					sign_key |= REPORT_SWITCH; 
					infotmReportKeyUp(SCANCODE_INFOTM_SWITCH);
					longpress_key = 0;
				}
				break;
			case SCANCODE_INFOTM_POWER:
				if(longpress_num == 25 && (~sign_key & REPORT_SHUT)) {
					DEBUG_KEY("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_SHUTDOWN\n");
					sign_key |= REPORT_SHUT;
					infotmReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);
					longpress_key = 0;
				}
				break;
			case SCANCODE_INFOTM_PHOTO:
                if(longpress_num == 8 && (~sign_key & REPORT_LOCK)) {
					DEBUG_KEY("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_LOCK\n");
					sign_key |= REPORT_LOCK; 
					infotmReportKeyUp(SCANCODE_INFOTM_LOCK);
					longpress_key = 0;
				}
				/* define reset key-event
				if(longpress_num == 50 && (~sign_key & REPORT_RESET)) {
					DEBUG_KEY("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_RESET\n");
					sign_key |= REPORT_RESET; 
					infotmReportKeyUp(SCANCODE_INFOTM_RESET);
					longpress_key = 0;
				}*/
				break;
			default :
				break;

		}
		sem_post ((sem_t*)data);
		longpress_num ++;

		usleep(100000);		//100ms
	}
}
/************************ end ***************************/

extern struct timeval __mg_event_timeout;

static void* EventLoop (void* data)
{
    LWEVENT lwe;
    int event;

    lwe.data.me.x = 0; lwe.data.me.y = 0;

    sem_post ((sem_t*)data);

    while (__mg_quiting_stage > _MG_QUITING_STAGE_EVENT) {
        event = IAL_WaitEvent (IAL_MOUSEEVENT | IAL_KEYEVENT, 0,
                        NULL, NULL, NULL, (void*)&__mg_event_timeout);
        if (event < 0)
            continue;

        lwe.status = 0L;

        if (event & IAL_MOUSEEVENT && kernel_GetLWEvent (IAL_MOUSEEVENT, &lwe))
            ParseEvent (&lwe);

        lwe.status = 0L;
        if (event & IAL_KEYEVENT && kernel_GetLWEvent (IAL_KEYEVENT, &lwe))
            ParseEvent (&lwe);

        if (event == 0 && kernel_GetLWEvent (0, &lwe))
            ParseEvent (&lwe);
    }

    /* printf("Quit from EventLoop()\n"); */
    return NULL;
}

/***********add for infotm server by jeason*****************/
#define SERVER_DEBUG
#ifdef SERVER_DEBUG
#define DEBUG_INFO(fmt, args...) if(DEBUG_ENABLED) printf(fmt, ##args)
#else
#define DEBUG_INFO(fmt, ...)
#endif

//#define G_SENSOR_EVENT	"/dev/input/event3"
#define FIFO_SERVER 	"/tmp/myfifo"
#define ACC_NODE        "/sys/devices/platform/power_domain/caracc"

#define BATTERY_EVENT	1
#define SDCARD_EVENT	0
#define AMBIENT_EVENT	0
#define COLLISION_EVENT	0
#define REAR_CAMERA_EVENT	0
#define REVERSE_IMAGE_EVENT	0
#define GPS_EVENT	0

#define EMPTY_POWER		38
#define LOW_POWER		40
#define HIGH_VOLTAGE	3800
#define MID_VOLTAGE		3700
#define LOW_VOLTAGE		3600
#define EMP_VOLTAGE		3450

static int ACC_ENABLE = 0;

typedef unsigned int __u32;
typedef int __s32;
typedef unsigned short __u16;

struct input_event{
	struct timeval time;
	__u16 type;
	__u16 code;
	__s32 value;
};

static void* infotmServerLoop(void)
{
	int time_rotator = 0;
	PUIStatus ui_status;
	time_t timep_sleep; 		//user for sleep timer
	time_t timep_dile; 			//user for idle timer
	time_t timep_discharge; 	//user for dischanging timer
	MSG Msg;
	FILE *fp = NULL;
	char config_items[128];
	char buffer[16];
	int capacity = 0;			//battery capacity
	int old_capacity = -1;		//old battery capacity
	int cap_sleep = 10;			//use for set the sleep time of checking battery capacity
	int tmp_voltage = 0;
	int video_voltage = 100;	//100mv

	int isCharge = 0;			//battery charge status
	int old_isChagre = -1;		//old battery charge status
	int isMounted = 0;			//sdcard mounted status
	int isNode = 0;				//sdcard dev node status
	int sd_status = 0;			//sdcard status
	int old_sd_status = -1;		//old sdcard status	
	int ambient_bright = 0;		//get the ambient value
	int old_ambient_bright = -1;//old ambient value
	int retvalue = 0;
	int old_collision_ret = -1;

	int rearcamera_status = 0;			//rearcamera status
	int old_rearcamera_status = -1;			//old rearcamera status

	int reverse_iamge_status = 0;			//reverse image status
	int old_reverse_iamge_status = -1;		//old reverse image status

	int acc_connect = 0;			//reverse image status
	int old_acc_connect = -1;		//old reverse image status

	int led_status = 0;

	struct input_event coll_val;

	int isLowPower = 0;			//battery is low power
	int needshutdown = 0;		//if the battery turn to discharging, change it to 1

	struct timeval idle_time = {0,0};
	struct timeval sensor_time;	//user for g-sensor detect timer
	int sensor_handle = 0;			//user for get g-sensor evemt
	fd_set rfds;
	
	Msg.wParam = 0;
	Msg.lParam = 0;

	time(&timep_sleep);
	time(&timep_dile);
	time(&timep_discharge);

	//for myfifo
	int fifo_fd = 0;
#if SDCARD_EVENT
	char fifo_buf[32];
	unlink(FIFO_SERVER);
	if((mkfifo(FIFO_SERVER, 0666) < 0) && (errno != EEXIST)) {	//O_CREAT|O_EXCL
        printf("INFOTMSERVER]ERR: failed create fifoserver\n");
    } else {
    	fifo_fd = open(FIFO_SERVER, O_RDONLY|O_NONBLOCK, 0);
		if(fifo_fd <= 0) {
			printf("INFOTMSERVER]ERR: failed open fifoserver\n");
		}
    }
	//end
#endif

    char *gsensor_event = getenv("GSENSOR_DEVICE");
    if(gsensor_event != NULL)
	    sensor_handle = open(gsensor_event, O_RDONLY|O_NOCTTY|O_NONBLOCK);
	if(sensor_handle <= 0)
		printf("[INFOTMSERVER]ERR:failed to open %s\n", gsensor_event);
	
	while(__mg_quiting_stage > _MG_QUITING_STAGE_EVENT) {
		//get the main window hwnd
		Msg.hwnd = GetMainWindow();
		if(Msg.hwnd == 0) 
			continue;
#if 0		
#if COLLISION_EVENT
		if(sensor_handle > 0) {
            static int eventHandled = 0;
            if(!(time_rotator % 5))//read a vailable event per 1 second
                eventHandled = 0;
			lseek(sensor_handle, 0, SEEK_SET);
			if(read(sensor_handle, &coll_val, sizeof(coll_val)) > 0
					&& coll_val.code == 220
					&& old_collision_ret != coll_val.value 
                    && !eventHandled) {
				Msg.message = MSG_USER_COLLISION;
				Msg.wParam = 1;
				DEBUG_INFO("[INFOTMSERVER]MSG:sensor event!\n");
				SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
				old_collision_ret = coll_val.value;
                eventHandled = 1;
			}
		}
#endif

#if REAR_CAMERA_EVENT 
        if(!(time_rotator % 5)) {
            rearcamera_status = !access("/dev/video0", F_OK);
            if(rearcamera_status != old_rearcamera_status){
                Msg.message = MSG_USER_REAR_CAMERA;
                Msg.wParam = rearcamera_status;
                DEBUG_INFO("[INFOTMSERVER]MSG:rear camera status changes, status=%d.\n", rearcamera_status);
                SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
                old_rearcamera_status = rearcamera_status;
            }
        }
#endif

#if REVERSE_IMAGE_EVENT 
        if(!(time_rotator % 5)) {
            int ri_fd = open("/sys/devices/virtual/switch/p2g/state", O_RDONLY);
            char buf[16] = {0};
            reverse_iamge_status = 0;
            if(ri_fd > 0) {
                int size = read(ri_fd, buf, 16);
                if(size > 0) {
                    reverse_iamge_status = atoi(buf) == 1 ? 1 : 0;
                }
                close(ri_fd);
            }
            if(reverse_iamge_status != old_reverse_iamge_status){
                Msg.message = MSG_USER_REVERSE_IMAGE;
                Msg.wParam = reverse_iamge_status;
                DEBUG_INFO("[INFOTMSERVER]MSG:reverse image status changes, status=%d.\n", reverse_iamge_status);
                SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
                old_reverse_iamge_status = reverse_iamge_status;
            }
        }
#endif

        if(ACC_ENABLE && !(time_rotator % 5)) {
            int an_fd = open(ACC_NODE, O_RDONLY);
            char buf[16] = {0};
            reverse_iamge_status = 0;
            if(an_fd > 0) {
                int size = read(an_fd, buf, 16);
                if(size > 0) {
                    acc_connect = atoi(buf) == 1 ? 1 : 0;
                }
                close(an_fd);
            }
            if(acc_connect != old_acc_connect){
                Msg.message = MSG_USER_ACC_CONNECT;
                Msg.wParam = acc_connect;
                DEBUG_INFO("[INFOTMSERVER]MSG:acc connect status changes, status=%d.\n", acc_connect);
                SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
                old_acc_connect = acc_connect;
            }
        }

#if BATTERY_EVENT	
		//get battery charge status
		if(!(time_rotator % 5)) {
			isCharge = is_charging();
			if(isCharge < 0) {
				printf("[INFOTMSERVER]ERR:get battery charge status failed\n");
			} else {
				if(isCharge != old_isChagre) {
					Msg.message = MSG_USER_CHARGE_STATUS;
					Msg.wParam = (unsigned int)isCharge;
					DEBUG_INFO("[INFOTMSERVER]MSG:charge status!-->%d\n", isCharge);
					SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
					if(old_isChagre == BATTERY_DISCHARGING) {
						time(&timep_sleep);
						if((isCharge == BATTERY_CHARGING || isCharge == BATTERY_FULL) && bl_status)
							setBacklight(0);
					}
					if(isCharge == BATTERY_DISCHARGING && old_isChagre > 0) {
						needshutdown = 1;
					} else {
						needshutdown = 0;
					}
					//set the backlight
					if(isCharge == BATTERY_DISCHARGING) {
						set_backlight(LCD_BACKLIGHT_MAX / 6);
					}
					if(old_isChagre == BATTERY_DISCHARGING) {
						fp = fopen("/config/libramain.config", "r");
						if(NULL == fp) {
							set_backlight(LCD_BACKLIGHT_MAX / 3);
							printf("[INFOTMSERVER]ERR:failed to open libramain.config\n");
						} else {
							while (!feof(fp)) {
								if(NULL == fgets(config_items, 128, fp)) {
									set_backlight(LCD_BACKLIGHT_MAX / 3);
									break;
								}
								if(strstr(config_items, "setup.display.brightness=High")) {
									set_backlight(LCD_BACKLIGHT_MAX);
									break;
								} else if(strstr(config_items, "setup.display.brightness=Medium")) {
									set_backlight(LCD_BACKLIGHT_MAX / 3);
									break;
								} else if(strstr(config_items, "setup.display.brightness=Low")) {
									set_backlight(LCD_BACKLIGHT_MAX / 6);
									break;
								}
							}
							close(fp);
						}
					}
					old_isChagre = isCharge;
				}	
				if(isCharge != BATTERY_DISCHARGING) {
					time(&timep_discharge);
				}
			}
		}

		//get baterry capacity
		if(!(time_rotator % cap_sleep)) {
			tmp_voltage = check_battery_capacity();
			//printf("#################show the voltage=%d######\n",tmp_voltage);
			if(tmp_voltage < 0) {
				printf("[INFOTMSERVER]ERR:get battery voltage failed\n");
			} else {
				//get the sence
				Msg.message = MSG_USER_GET_UI_STATUS;
				Msg.wParam = 0;
				ui_status = SendMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
				if(ui_status == 0) {
					printf("[INFOTMSERVER]ERR: UIStatus update failed!\n");
				} else {
					if(ui_status->highLoad) {
						//the device is high load
						tmp_voltage += video_voltage;
					}

					if(tmp_voltage >= HIGH_VOLTAGE || isCharge == BATTERY_FULL) {
						capacity = 100;
						cap_sleep = 10;
					} else if(tmp_voltage > MID_VOLTAGE && tmp_voltage < HIGH_VOLTAGE) {
						capacity = 80;
						cap_sleep = 10;
					} else if(tmp_voltage > LOW_VOLTAGE && tmp_voltage <= MID_VOLTAGE) {
						capacity = 50;
						cap_sleep = 5;
					} else if(tmp_voltage < LOW_VOLTAGE) {
						if(isCharge == BATTERY_DISCHARGING) {
							if(bl_status)
								setBacklight(0);    //turn on backlight when device is low_power
							isLowPower = 1;
						}
						if(tmp_voltage < EMP_VOLTAGE && isCharge == BATTERY_DISCHARGING) {
							DEBUG_INFO("[INFOTMSERVER]MSG:Device is EMP_VOLTAGE,vol=%d\n", tmp_voltage);
							infotmReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);
						}
						capacity = 20;
						cap_sleep = 2;
					}

					if(isCharge == BATTERY_DISCHARGING && old_capacity <= 0) {
						old_capacity = 110;
					}
					if( (isCharge == BATTERY_DISCHARGING && old_capacity > capacity) || 
							(isCharge == BATTERY_CHARGING && old_capacity < capacity)
							||(isCharge == BATTERY_FULL && old_capacity != capacity))
					{
						Msg.message = MSG_USER_BATTERY_STATUS;
						Msg.wParam = (unsigned int)capacity;
						DEBUG_INFO("[INFOTMSERVER]MSG:capacity=%d,vol=%d\n", capacity, tmp_voltage);
						SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
						old_capacity = capacity;
					}
				}
			}

#if 0
			capacity = check_battery_capacity();
			if(capacity < 0) {
				printf("[INFOTMSERVER]ERR:get battery capacity failed\n");
			} else {
				if(capacity != old_capacity) {
					Msg.message = MSG_BATTERY_STATUS;
					Msg.wParam = (unsigned int)capacity;				
					if(capacity < EMPTY_POWER && isCharge == 2) {
						infotmReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);	//report shutdown-event when the battery is EMPTY_POWER
					}
					if(capacity <= LOW_POWER && old_capacity > capacity){
						if(bl_status)
							setBacklight(0);	//turn on backlight when device is low_power
						isLowPower = 1;
					}
					DEBUG_INFO("[INFOTMSERVER]MSG:capacity changed!-->%d<\n", capacity);
					SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
					old_capacity = capacity;
				}
			}
#endif
		}
	
#endif
		
#if SDCARD_EVENT
		if(!(time_rotator % 5) && fifo_fd > 0) {
			memset(fifo_buf, 0, 32);
			if(read(fifo_fd, fifo_buf, 32) > 0) {
				sd_status = atoi(fifo_buf);
				if(sd_status != old_sd_status){
					Msg.message = MSG_USER_SDCARD_MOUNT;
					Msg.wParam = sd_status;
					DEBUG_INFO("[INFOTMSERVER]MSG:sdcard status changes, status=%d.\n", sd_status);
					SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
					old_sd_status = sd_status;
				}
			}
		}
#endif

#if AMBIENT_EVENT
		//get ambient bright
		if(!(time_rotator % 5)) {
			ambient_bright = get_ambientBright();
			if(ambient_bright != old_ambient_bright) {
				Msg.message = MSG_USER_IR_LED;
				Msg.wParam = ambient_bright;
				DEBUG_INFO("[INFOTMSERVER]MSG:ambientBright status-->%d\n", ambient_bright);
				SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
				old_ambient_bright = ambient_bright;
			}
		}
#endif

		//get ui_status
		if(!(time_rotator % 2)) {
			Msg.message = MSG_USER_GET_UI_STATUS;
			Msg.wParam = 0;
			ui_status = SendMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
			if(ui_status == 0) {
				printf("[INFOTMSERVER]ERR: UIStatus update failed!\n");
				if(isCharge == BATTERY_DISCHARGING) {
					DEBUG_INFO("[INFOTMSERVER]MSG: Discharging 1 report SHUTDOWN\n");
					infotmReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);	//report the shutdown-event when device is discharging
				}
			} else {
				if(ui_status->iWantLightLcd){
					if(bl_status)
						setBacklight(0);
					ui_status->iWantLightLcd = 0;
				}
				
				if(ui_status->idleTime.tv_sec != idle_time.tv_sec || ui_status->idleTime.tv_usec != idle_time.tv_usec) {
					idle_time.tv_sec = ui_status->idleTime.tv_sec;
					idle_time.tv_usec = ui_status->idleTime.tv_usec;
					time(&timep_dile);
					time(&timep_sleep);
				} else {
					if(ui_status->displaySleep > 0 && (time(NULL) - timep_sleep) >= ui_status->displaySleep){
						if(!bl_status) {
							DEBUG_INFO("[INFOTMSERVER]MSG:turn off the backlight\n");
							setBacklight(1);				//turn off the backlight when the device has been idle "displaySleep" seconds
						}
					}
				}
				
				if(ui_status->idle) {
					//the device is idle
					if(led_status != 1){
						indicator_led_switch(1);
						led_status = 1;
					}
					if(ui_status->autoPowerOff > 0 && (time(NULL) - timep_dile)>= ui_status->autoPowerOff) {
						if(!((time(NULL) - timep_dile) % 10)) {
							DEBUG_INFO("[INFOTMSERVER]MSG:report the shutdown-event\n");
							infotmReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);//report the shutdown-event,the device been idle "autopoweroff" sec
						}
					}
				} else {
					set_indicator_led_flicker(1);
					led_status = 0;
					time(&timep_dile);
				}

				if(needshutdown) {
					if(ui_status->delayedShutdown > 0){
						if((time(NULL) - timep_discharge) >= ui_status->delayedShutdown && !((time(NULL) - timep_discharge) % 10)){
							DEBUG_INFO("[INFOTMSERVER]MSG:report shutdown when discharging\n");
							infotmReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);//report shutdown,the device been discharging "delayedShutdown" sec
						}
					} else {
						if(needshutdown > 1) {
							DEBUG_INFO("[INFOTMSERVER]MSG:report shutdown when discharging\n");
							infotmReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);
						} else {
							needshutdown++;
						}
					}
				}			
			}
		}
		
#endif
		usleep(200*1000); //200ms
		time_rotator++;
		if(time_rotator == 50)
			time_rotator = 0;
	}

	if(fifo_fd >= 0)
		close(fifo_fd);
	if(sensor_handle > 0)
		close(sensor_handle);
	return NULL;
}

#if 0
static void GPSUpdateLoop()
{
	MSG Msg;
    Msg.wParam = 0;
    Msg.lParam = 0;
    gps_data *gd = NULL;
	while(__mg_quiting_stage > _MG_QUITING_STAGE_EVENT) {
		//get the main window hwnd
        sleep(1);
		Msg.hwnd = GetMainWindow();
		if(Msg.hwnd == 0) 
			continue;

        gd = (gps_data *)malloc(sizeof(gps_data));
        if(gd != NULL) {
            memset(gd, 0, sizeof(gps_data));
            if(!gps_request(gd)) {
                Msg.message = MSG_USER_GPS;
                Msg.lParam = gd;
                DEBUG_INFO("[INFOTMSERVER]MSG:gps status changes, lParam=%ld.\n", gd);
                SendNotifyMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
            } else {
                free(gd);
            }
        }
        gd = NULL;
    }

}
#endif

#define WAKEUP_STATUS	"/sys/class/wakeup/state"
#if 0
static int initInfotmServer()
{
    int err;
	int retvalue;
	int hd;
	char buffer[16];
	//check wakeup status
	if((hd = open(WAKEUP_STATUS, O_RDONLY)) < 0) {
		printf("[INFOTMSERVER]ERR: failed to open wake-up status,hd=%d\n", hd);
	} else {
		if((retvalue = read(hd, buffer, 16)) < 0){
			printf("[INFOTMSERVER]ERR:read wake-up status failed,retvalue=%d\n", retvalue);
		} else {
			if(atoi(buffer) == 1 && is_charging() == BATTERY_DISCHARGING) {
				setBacklight(1);
				DEBUG_INFO("[INFOTMSERVER]MSG:turn off backlight, according to poweron by collision\n");
			}
		}
		close(hd);
	}

    if(!access(ACC_NODE, F_OK))
        ACC_ENABLE = 1;

    printf("init.c, pthread_create, initInfotmServer\n");
	err = pthread_create(&__info_server, NULL, infotmServerLoop, NULL);
	if(err != 0) {
		printf("[INFOTMSERVER]ERR:create infotmServerLoop thread failed!\n");
		return -1;
	}

#if GPS_EVENT
    if(GPS_ENABLE) {
        pthread_attr_t attr;
        pthread_t t;
        pthread_attr_init (&attr);
        printf("init.c, pthread_create, GPSUpdateLoop\n");
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&t, &attr, (void *)GPSUpdateLoop, NULL);
        pthread_attr_destroy (&attr);
    }
#endif

	return 0;
}
#endif

/***********add for sdcard status check by jeason****************/
#define MMC_DEVICE	"/dev/mmcblk0p1"
#define DATA_PATH	"/mnt/sd0"
#define SDCARD_CONFIG	"/mnt/sd0/sdcard.cfg"
static void guiSystemCmd(char *cmd)
{
	printf("[SDCARD_CHECK]MSG:show the system command <%s>\n", cmd);
	pid_t status;
	status = system(cmd);
	if (-1 == status) {
		printf("system error!");
	} else {
		if (WIFEXITED(status)) {
			if (0 != WEXITSTATUS(status)) {
				printf("shell script fail,exit code: %d\n", WEXITSTATUS(status));
			}
		}
	}
}

static int guiCheckMount()
{
	FILE *fp = NULL;
	char mount_contents[256];
	
	fp = fopen("/proc/self/mountinfo", "r");
	if(NULL == fp) {
		printf("[SDCARD_CHECK]ERR: failed to open /proc/self/mountinfo\n");
		return -1;
	}
	while(!feof(fp)) {
		if(NULL == fgets(mount_contents, 256, fp)) {
			printf("[SDCARD_CHECK]ERR: failed to read /proc/self/mountinfo\n");
			fclose(fp);
			return -1;
		}
		if(strstr(mount_contents, DATA_PATH)) {
			fclose(fp);
			return 1; //find /mnt/sd0 in mount-table
		}
	}
	fclose(fp);
	return 0;
}

int gui_sdcard_status = 0;
static int initSdcardCheck()
{
	int ret, fd;
	char cmd[128];

	if(access(SDCARD_CONFIG, F_OK) == 0) {
		printf("[SDCARD_CHECK]MSG:Congratulations access the %s success!!!\n", SDCARD_CONFIG);
		gui_sdcard_status = 1;
		remove(SDCARD_CONFIG);
		return 0;
	}

	if(guiCheckMount() > 0) {
		sprintf(cmd, "umount %s", DATA_PATH);
		guiSystemCmd(cmd);
		sprintf(cmd, "/sbin/fsck.fat -a  %s", MMC_DEVICE);
		printf("[SDCARD_CHECK]MSG:##Start to fsck %s ...\n", MMC_DEVICE);
		guiSystemCmd(cmd);
		printf("[SDCARD_CHECK]MSG:##Finish fscking %s\n", MMC_DEVICE);
			if(access(DATA_PATH, F_OK) !=0 ) {
				sprintf(cmd, "mkdir %s", DATA_PATH);
				guiSystemCmd(cmd);
			}
		sprintf(cmd, "mount -o flush %s %s; rm -f /mnt/sd0/FSCK*.REC", MMC_DEVICE, DATA_PATH);
		guiSystemCmd(cmd);
	}

	gui_sdcard_status = guiCheckMount() > 0 ? 1 : 0;

	return 0;
}

/***********add for infotm monkey test by jeason*****************/
#define MONKEY_PATH "/root/monkey-test"
static void* infotmMonkeyTest(void)
{
	printf("[MONKEYTEST]MSG:entry  infotmMonkeyTest !!!!\n");

	int ret = 0;
	int i = 0;
	int key1 = 0;
	int key2 = 0;
	int key3 = 0;
	int test_time = 0;
	int old_test_time = -1;
	int test_handle = 0;
	int key_long = 0;
	int sleep_time = 0;
	int used_time = 0;
	char buf[64];
	char old_buf[64];
	char buf_1[64];
	char buf_2[64];
	char buf_3[64];
	time_t timep_monkey;
	
	LWEVENT lwe;	
	lwe.type = LWETYPE_KEY;

	fd_set rfds;
	int retvalue = 0;
	
	test_handle = open(MONKEY_PATH, O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	if(test_handle < 0) {
		printf("[MONKEYTEST]ERR:failed to open %s\n", MONKEY_PATH);
		return;
	}
	close(test_handle);
	memset(old_buf, 1, 1);
	while(__mg_quiting_stage > _MG_QUITING_STAGE_EVENT) {
		test_handle = open(MONKEY_PATH, O_RDWR);
		if(test_handle < 0) {
			printf("[MONKEYTEST]ERR:failed to open %s\n", MONKEY_PATH);
			break;
		}
		memset(buf, 0, 64);
		ret = read(test_handle, buf, 64);
		if(ret < 0) {
			printf("[MONKEYTEST]ERR:failed to read %s\n", MONKEY_PATH);
			close(test_handle);
			break;
		}
		
		close(test_handle);
		
		if(strcmp(old_buf, buf)) {
			printf("[MONKEYTEST]MSG:buf changes+++++++++++++++++++++++++++\n");
			if(strlen(buf)==0){
				printf("[MONKEYTEST]MSG:no argument==========\n");
				strcpy(old_buf, buf);
				usleep(5*1000*1000);
				continue;
			}
			sscanf(buf,"%[^,],%[^,],%[^,]",buf_1,buf_2,buf_3);
			key1 = atoi(buf_1);
			key2 = atoi(buf_2);
			key3 = atoi(buf_3);
			strcpy(old_buf, buf);
			if(key1 <= 0 || key2 <= 0 || key3 <= 0 || key2 > key3) {
				printf("[MONKEYTEST]argument set failed! times=%dmin, min delay=%d*100ms, max delay=%d*100ms\n", key1, key2, key3);
				printf("[MONKEYTEST]argument must be like: echo 10,200,600 > monkey-test\n");
				usleep(5*1000*1000);
				test_time = 0;
				used_time = 0;
				continue;
			}
			
			test_time = key1;
			used_time = 0;
			printf("[MONKEYTEST]get the times=%dmin, min delay=%d*100ms, max delay=%d*100ms\n",key1, key2, key3);
		}
		used_time ++;
		if((used_time) < (test_time * 600)){
			srand((unsigned)time(NULL));
			switch(rand()%20)
			{
				case 0:
				case 1:
				case 2:
				case 19:
					lwe.data.ke.scancode = SCANCODE_INFOTM_MENU;
					lwe.data.ke.event = KE_KEYUP;
					ParseEvent (&lwe);
					break;
				case 3:
				case 4:
				case 5:
					lwe.data.ke.scancode = SCANCODE_INFOTM_MODE;
					lwe.data.ke.event = KE_KEYUP;
					ParseEvent (&lwe);
					break;
				case 6:
				case 7:
				case 15:
				case 16:
				case 17:
				case 18:
					lwe.data.ke.scancode = SCANCODE_INFOTM_OK;
					lwe.data.ke.event = KE_KEYUP;
					ParseEvent (&lwe);
					break;
				case 8:
					lwe.data.ke.scancode = SCANCODE_INFOTM_OK;
					lwe.data.ke.event = KE_KEYDOWN;
					ParseEvent (&lwe);
					srand((unsigned)time(NULL));
					key_long = rand()%5;
					for(i=0; i < key_long; i++)
					{
						usleep(100*1000);
						used_time ++;
						lwe.data.ke.event = KE_KEYUP;
						ParseEvent (&lwe);
					}
					break;
				case 9:
				case 10:
					lwe.data.ke.scancode = SCANCODE_INFOTM_DOWN;
					lwe.data.ke.event = KE_KEYUP;
					ParseEvent (&lwe);
					break;
				case 11:					
					lwe.data.ke.scancode = SCANCODE_INFOTM_DOWN;
					lwe.data.ke.event = KE_KEYDOWN;
					ParseEvent (&lwe);
					srand((unsigned)time(NULL));
					key_long = rand()%5;
					for(i=0; i < key_long; i++)
					{
						usleep(100*1000);
						used_time ++;
						lwe.data.ke.event = KE_KEYUP;
						ParseEvent (&lwe);
					}
					break;
				case 12:
				case 13:
					lwe.data.ke.scancode = SCANCODE_INFOTM_UP;
					lwe.data.ke.event = KE_KEYUP;
					ParseEvent (&lwe);
					break;
				case 14:					
					lwe.data.ke.scancode = SCANCODE_INFOTM_UP;
					lwe.data.ke.event = KE_KEYDOWN;
					ParseEvent (&lwe);
					srand((unsigned)time(NULL));
					key_long = rand()%5;
					for(i=0; i < key_long; i++)
					{
						usleep(100*1000);
						used_time ++;
						lwe.data.ke.event = KE_KEYUP;
						ParseEvent (&lwe);
					}
					break;
				default:
					break;
			}			
			srand((unsigned)time(NULL));
			sleep_time = rand()%(key3-key2);
			usleep((sleep_time*100*1000) + (key2*100*1000));
			used_time += key2;
			used_time += (sleep_time * 2);
		} else {
			usleep(5*1000*1000);
		}
	}
}

static int initMonkeyTest()
{
	int err;
    printf("init.c, pthread_create, infotmMonkeyTest\n");
	err = pthread_create(&__info_monkey, NULL, infotmMonkeyTest, NULL);
	if(err != 0) {
		printf("[MONKEYTEST]ERR:create infotmMonkeyTest thread failed!\n");
		return -1;
	}
	return 0;
}
/************************ end ***************************/

/************************** Thread Information  ******************************/

pthread_key_t __mg_threadinfo_key;

static inline BOOL createThreadInfoKey (void)
{
    if (pthread_key_create (&__mg_threadinfo_key, NULL))
        return FALSE;
    return TRUE;
}

static inline void deleteThreadInfoKey (void)
{
    pthread_key_delete (__mg_threadinfo_key);
}

MSGQUEUE* mg_InitMsgQueueThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    if (!(pMsgQueue = malloc(sizeof(MSGQUEUE))) ) {
        return NULL;
    }

    if (!mg_InitMsgQueue(pMsgQueue, 0)) {
        free (pMsgQueue);
        return NULL;
    }

    pthread_setspecific (__mg_threadinfo_key, pMsgQueue);
    return pMsgQueue;
}

void mg_FreeMsgQueueThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    pMsgQueue = pthread_getspecific (__mg_threadinfo_key);
#ifdef __VXWORKS__
    if (pMsgQueue != (void *)0 && pMsgQueue != (void *)-1) {
#else
    if (pMsgQueue) {
#endif
        mg_DestroyMsgQueue (pMsgQueue);
        free (pMsgQueue);
#ifdef __VXWORKS__
        pthread_setspecific (__mg_threadinfo_key, (void*)-1);
#else
        pthread_setspecific (__mg_threadinfo_key, NULL);
#endif
    }
}

/*
The following function is moved to src/include/internals.h as an inline 
function.
MSGQUEUE* GetMsgQueueThisThread (void)
{
    return (MSGQUEUE*) pthread_getspecific (__mg_threadinfo_key);
}
*/

/************************** System Initialization ****************************/

int __mg_timer_init (void);

static BOOL SystemThreads(void)
{
    sem_t wait;

    sem_init (&wait, 0, 0);

    // this is the thread for desktop window.
    // this thread should have a normal priority same as
    // other main window threads. 
    // if there is no main window can receive the low level events,
    // this desktop window is the only one can receive the events.
    // to close a MiniGUI application, we should close the desktop 
    // window.
#ifdef __NOUNIX__
    {
        pthread_attr_t new_attr;
        pthread_attr_init (&new_attr);
        pthread_attr_setstacksize (&new_attr, 16 * 1024);
        printf("init.c, pthread_create, DesktopMain\n");
        pthread_create (&__mg_desktop, &new_attr, DesktopMain, &wait);
        pthread_attr_destroy (&new_attr);
    }
#else
        printf("init.c, pthread_create, DesktopMain2\n");
    pthread_create (&__mg_desktop, NULL, DesktopMain, &wait);
#endif

    sem_wait (&wait);

    __mg_timer_init ();

    // this thread collect low level event from outside,
    // if there is no event, this thread will suspend to wait a event.
    // the events maybe mouse event, keyboard event, or timeout event.
    //
    // this thread also parse low level event and translate it to message,
    // then post the message to the approriate message queue.
    // this thread should also have a higher priority.
        printf("init.c, pthread_create, EventLoop\n");
    pthread_create (&__mg_parsor, NULL, EventLoop, &wait);
    sem_wait (&wait);

    sem_destroy (&wait);
    return TRUE;
}

BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text)
{
    return SendMessage (HWND_DESKTOP, MSG_REINITSESSION, init_sys_text, 0) == 0;
}

#ifndef __NOUNIX__
static struct termios savedtermio;

void* GUIAPI GetOriginalTermIO (void)
{
    return &savedtermio;
}

static void sig_handler (int v)
{
    if (v == SIGSEGV) {
        kill (getpid(), SIGABRT); /* cause core dump */
    }else if (__mg_quiting_stage > 0) {
        ExitGUISafely(-1);
    }else{
        exit(1); /* force to quit */
    }
}

static BOOL InstallSEGVHandler (void)
{
    struct sigaction siga;
    
    siga.sa_handler = sig_handler;
    siga.sa_flags = 0;
    
    memset (&siga.sa_mask, 0, sizeof (sigset_t));
    sigaction (SIGSEGV, &siga, NULL);
    sigaction (SIGTERM, &siga, NULL);
    sigaction (SIGINT, &siga, NULL);
    sigaction (SIGPIPE, &siga, NULL);

    return TRUE;
}
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

int GUIAPI InitGUI (int args, const char *agr[])
{
    int step = 0;
	//init backlight_status by jeason
	bl_status = 0;

    char *rotate = getenv("SCREEN_ORIENTATION");
    if(rotate && !strcasecmp(rotate, "portrait")) {
        ROTATE_SCREEN = 1;
    } else {
        ROTATE_SCREEN = 0;
    }

    char *gps_enable = getenv("GPS_ENABLE");
    if(gps_enable && !strcasecmp(gps_enable, "yes")) {
        GPS_ENABLE = 1;
    } else {
        GPS_ENABLE = 0;
    }

    __mg_quiting_stage = _MG_QUITING_STAGE_RUNNING;
    __mg_enter_terminategui = 0;

#ifdef HAVE_SETLOCALE
//    setlocale (LC_ALL, "C");
#endif

#if defined (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC)
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, _sysvipc_mutex_sem_init\n", step);
    if (_sysvipc_mutex_sem_init ())
        return step;
#endif

#ifndef __NOUNIX__
    // Save original termio
    tcgetattr (0, &savedtermio);
#endif

    /*initialize default window process*/
    __mg_def_proc[0] = PreDefMainWinProc;
    __mg_def_proc[1] = PreDefDialogProc;
    __mg_def_proc[2] = PreDefControlProc;

    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitFixStr\n", step);
    if (!mg_InitFixStr ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Fixed String module failure!\n");
        return step;
    }
    
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitMisc\n", step);
    /* Init miscelleous*/
    if (!mg_InitMisc ()) {
        fprintf (stderr, "KERNEL>InitGUI: Initialization of misc things failure!\n");
        return step;
    }

    step++;
    fprintf(stderr, "InitGUI step %d, mg_InitGAL\n", step);
    switch (mg_InitGAL ()) {
    case ERR_CONFIG_FILE:
        fprintf (stderr, 
            "KERNEL>InitGUI: Reading configuration failure!\n");
        return step;

    case ERR_NO_ENGINE:
        fprintf (stderr, 
            "KERNEL>InitGUI: No graphics engine defined!\n");
        return step;

    case ERR_NO_MATCH:
        fprintf (stderr, 
            "KERNEL>InitGUI: Can not get graphics engine information!\n");
        return step;

    case ERR_GFX_ENGINE:
        fprintf (stderr, 
            "KERNEL>InitGUI: Can not initialize graphics engine!\n");
        return step;
    }

#ifndef __NOUNIX__
    InstallSEGVHandler ();
#endif

    /*
     * Load system resource here.
     */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitSystemRes\n", step);
    if (!mg_InitSystemRes ()) {
        fprintf (stderr, "KERNEL>InitGUI: Can not initialize system resource!\n");
        goto failure1;
    }

    /* Init GDI. */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitGDI\n", step);
    if(!mg_InitGDI()) {
        fprintf (stderr, "KERNEL>InitGUI: Initialization of GDI failure!\n");
        goto failure1;
    }

    /* Init Master Screen DC here */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitScreenDC\n", step);
    if (!mg_InitScreenDC (__gal_screen)) {
        fprintf (stderr, "KERNEL>InitGUI: Can not initialize screen DC!\n");
        goto failure1;
    }

    g_rcScr.left = 0;
    g_rcScr.top = 0;
    g_rcScr.right = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXX) + 1;
    g_rcScr.bottom = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXY) + 1;

    license_create();
    
    fprintf (stderr, "KERNEL>InitGUI: skip splash_draw_framework\n");
    //splash_draw_framework(); 

    /* Init mouse cursor. */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitCursor\n", step);
    if( !mg_InitCursor() ) {
        fprintf (stderr, "KERNEL>InitGUI: Count not init mouse cursor!fengwu continue\n");
        //goto failure1;
    }
    splash_progress();

    /* Init low level event */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitLWEvent\n", step);
    if(!mg_InitLWEvent()) {
        fprintf(stderr, "KERNEL>InitGUI: Low level event initialization failure!\n");
        goto failure1;
    }
    splash_progress();

    /** Init LF Manager */
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitLFManager\n", step);
    step++;
    if (!mg_InitLFManager ()) {
        fprintf (stderr, "KERNEL>InitGUI: Initialization of LF Manager failure!\n");
        goto failure;
    }
    splash_progress();

#ifdef _MGHAVE_MENU
    /* Init menu */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitMenu\n", step);
    if (!mg_InitMenu ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Menu module failure!\n");
        goto failure;
    }
#endif

    /* Init control class */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitControlClass\n", step);
    if(!mg_InitControlClass()) {
        fprintf(stderr, "KERNEL>InitGUI: Init Control Class failure!\n");
        goto failure;
    }
    splash_progress();

    /* Init accelerator */
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitAccel\n", step);
    if(!mg_InitAccel()) {
        fprintf(stderr, "KERNEL>InitGUI: Init Accelerator failure!\n");
        goto failure;
    }
    splash_progress();

    step++;
    printf("InitGUI step %d, mg_InitDesktop\n", step);
    if (!mg_InitDesktop ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Desktop failure!\n");
        goto failure;
    }
    splash_progress();
   
    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, mg_InitFreeQMSGList\n", step);
    if (!mg_InitFreeQMSGList ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init free QMSG list failure!\n");
        goto failure;
    }
    splash_progress();

    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, createThreadInfoKey. no delay\n", step);
    if (!createThreadInfoKey ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init thread hash table failure!\n");
        goto failure;
    }

    //splash_delay();

    step++;
    if(DEBUG_ENABLED) printf("InitGUI step %d, SystemThreads\n", step);
    if (!SystemThreads()) {
        fprintf (stderr, "KERNEL>InitGUI: Init system threads failure!\n");
        goto failure;
    }

	// init sdcard status check by jeason
	//step++;
	//if(DEBUG_ENABLED) printf("InitGUI step %d,SD card status check\n", step);
	//if(initSdcardCheck()) {	
	//	fprintf (stderr, "KERNEL>InitGUI: Init sdcard status check threads failure!\n");
	//	goto failure;
	//}

	//init infotm server by jeason
	// this thread collect system events from outside and get the setting configs
    // the events maybe:
    // 1.battery charge status change.
    // 2.battery capacity change.
    // 3.sdcard mounted or umounted.
    // 4.system is idle
    // the setting configs include:
    // 1.delayedShutdown time
    // 2.autoPowerOff time
    // 3.displaySleep time
	//step++;
	//printf("InitGUI step %d infotmServer\n", step);
	//if(initInfotmServer()) {
	//	fprintf (stderr, "KERNEL>InitGUI: Init infotmServer threads failure!\n");
	//	goto failure;
	//}
	
//#ifdef MONKEY_TEST
//	step++;
//	if(DEBUG_ENABLED) printf("InitGUI step %d initMonkeyTest\n", step);
//	if(initMonkeyTest()) {
//		fprintf (stderr, "KERNEL>InitGUI: Init initMonkeyTest threads failure!\n");
//		goto failure;
//	}
//#endif
	
	//init infotm server end

    if(DEBUG_ENABLED) printf("InitGUI SetKeyboardLayout\n");
    SetKeyboardLayout ("default");

    if(DEBUG_ENABLED) printf("InitGUI SetCursor\n");
    SetCursor (GetSystemCursor (IDC_ARROW));
    
    fprintf(stderr, "InitGUI mg_TerminateMgEtc\n");
    SetCursorPos (g_rcScr.right >> 1, g_rcScr.bottom >> 1);

    mg_TerminateMgEtc ();
    return 0;

failure:
    mg_TerminateLWEvent ();
failure1:
    mg_TerminateGAL ();
    fprintf (stderr, "KERNEL>InitGUI: Init failure, please check "
            "your MiniGUI configuration or resource.\n");
    return step;
}

void GUIAPI TerminateGUI (int not_used)
{
    /* printf("Quit from MiniGUIMain()\n"); */

    __mg_enter_terminategui = 1;

    pthread_join (__mg_desktop, NULL);

    /* DesktopProc() will set __mg_quiting_stage to _MG_QUITING_STAGE_EVENT */
    pthread_join (__mg_parsor, NULL);

	/********** cancel infotm Server by jeason **********/
	printf("[INFOTMSERVER]Cancel the Infotm_IdleDetect thread\n");
	if(pthread_cancel(__info_server) != 0) {
		printf("[INFOTMSERVER]Cancel the Infotm_IdleDetect thread failed!\n");
	}
	pthread_join(__info_server, NULL);
	
	/********** cancel monkey test Server by jeason **********/
#ifdef MONKEY_TEST
	printf("[MONKEYTEST]Cancel the monkey test thread\n");
	if(pthread_cancel(__info_monkey) != 0) {
		printf("[MONKEYTEST]Cancel the monkey test thread failed!\n");
	}
	pthread_join(__info_monkey, NULL);
#endif
	/********** end **********/

    deleteThreadInfoKey ();
    license_destroy();

    __mg_quiting_stage = _MG_QUITING_STAGE_TIMER;
    mg_TerminateTimer ();

    mg_TerminateDesktop ();

    mg_DestroyFreeQMSGList ();
    mg_TerminateAccel ();
    mg_TerminateControlClass ();
#ifdef _MGHAVE_MENU
    mg_TerminateMenu ();
#endif
    mg_TerminateMisc ();
    mg_TerminateFixStr ();

#ifdef _MGHAVE_CURSOR
    mg_TerminateCursor ();
#endif
    mg_TerminateLWEvent ();

    mg_TerminateScreenDC ();
    mg_TerminateGDI ();
    mg_TerminateLFManager ();
    mg_TerminateGAL ();

    extern void mg_miFreeArcCache (void);
    mg_miFreeArcCache ();

    /* 
     * Restore original termio
     *tcsetattr (0, TCSAFLUSH, &savedtermio);
     */

#if defined (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC)
    _sysvipc_mutex_sem_term ();
#endif
}

#endif /* _MGRM_THREADS */

void GUIAPI ExitGUISafely (int exitcode)
{
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
#   define IDM_CLOSEALLWIN (MINID_RESERVED + 1) /* see src/kernel/desktop-*.c */
        SendNotifyMessage(HWND_DESKTOP, MSG_COMMAND, IDM_CLOSEALLWIN, 0);
        SendNotifyMessage(HWND_DESKTOP, MSG_ENDSESSION, 0, 0);

        if (__mg_quiting_stage > 0) {
            __mg_quiting_stage = _MG_QUITING_STAGE_START;
        }
    }
}
