/*
 ** $Id: dummy.c 8944 2015-06-04 jeason $
 **
 ** dummy.c: Common Input Engine for Custom IAL Engine.
 ** 
 ** Copyright (C) 2015 infotmic Software
 **
 ** Created by Jeason.Zhao, 2015/06/04
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "common.h"
#include "misc.h"
#include "tslib.h"

#define _DUMMY_IAL
#ifdef _DUMMY_IAL

#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>
#include "minigui.h"
#include "ial.h"
#include "dummy.h"

//#define TOUCHSCREEN	"/dev/input/event4"
//#define KEY_INPUT "/dev/input/event1"
//#define KEY_INPUT_POWER "/dev/input/event2"

typedef unsigned int __u32;
typedef int __s32;
typedef unsigned short __u16;
#define COMM_MOUSEINPUT    0x01
#define COMM_KBINPUT       0x02
#define DEBUG 1

/* -----------------------------Touchscreen------------------------------- */
/* for storing data reading from /dev/touchScreen/0raw */
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;

static int mousex = 0;
static int mousey = 0;
static TS_EVENT ts_event;
static struct tsdev *ts;
#define TS_MAX_W 800
#define TS_MAX_H 480

/* -----------------------------Keyboard---------------------------------- */
static unsigned char state [NR_KEYS + 20];
static short KEYCODE = -1, KEYSTATUS = -1;
static int btn_fd = -1;
static int btn_fd1 =-1;
static cur_key = 0;
static int EVENT_HANDLE = 0;
static int keys_rollover = 0x00;

struct input_event{
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
};

/************************  Low Level Input Operations **********************/
/* 
 * Keyboard operations -- Event
 */
static int keycode_map(struct input_event inputVal)
{
    switch(inputVal.code)
    {
        case 139:
            state[SCANCODE_INFOTM_MENU] = inputVal.value ^ keys_rollover;	//for sportdv
            //state[SCANCODE_INFOTM_MENU] = inputVal.value;
            KEYCODE = SCANCODE_INFOTM_MENU;
            return 0;
            //case 102:
        case 108:
            //state[SCANCODE_INFOTM_DOWN] = inputVal.value ^ 0x01;
            state[SCANCODE_INFOTM_DOWN] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_DOWN;
            return 0;
        case 102:
            //state[SCANCODE_INFOTM_DOWN] = inputVal.value ^ 0x01;
            state[SCANCODE_INFOTM_DOWN] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_DOWN;
            return 0;
		case 114:	//for sportdv
			state[SCANCODE_INFOTM_OK] = inputVal.value ^ keys_rollover;
			KEYCODE = SCANCODE_INFOTM_OK;
			return 0;
        case 352:
            //state[SCANCODE_INFOTM_OK] = inputVal.value ^ 0x01;
            state[SCANCODE_INFOTM_OK] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_OK;
            return 0;
		case 115:	//for sportdv
			state[SCANCODE_INFOTM_UP] = inputVal.value ^ keys_rollover;
			KEYCODE = SCANCODE_INFOTM_UP;
			return 0;
        case 103:
            //state[SCANCODE_INFOTM_UP] = inputVal.value ^ 0x01;
            state[SCANCODE_INFOTM_UP] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_UP;
            return 0;
        case 116:
            state[SCANCODE_INFOTM_POWER] = inputVal.value ;
            KEYCODE = SCANCODE_INFOTM_POWER;
            return 0;
        case 0x175:
            state[SCANCODE_INFOTM_MODE] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_MODE;
            return 0;
        case 152:
            state[SCANCODE_INFOTM_LOCK] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_LOCK;
            return 0;
        case 212:
            state[SCANCODE_INFOTM_PHOTO] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_PHOTO;
            return 0;
        default:
            return 1;
    }
    return 1;
}

static int keyboard_update (void)
{
    char buf[48];
    struct input_event inputVal;
    struct input_event inputVal1;
    int ret = 0;
    int ret1 = 0;
    if(EVENT_HANDLE == 1 && read(btn_fd, &inputVal, sizeof(inputVal)) > 0) {
        if(inputVal.code != 0) {
            if(DEBUG) printf ("type :%d\ncode :%d\nvalue :%d\n", inputVal.type, inputVal.code, inputVal.value);
            if(!keycode_map(inputVal))
                ret = 1;
        }
    } else {
        //if(DEBUG) printf ("[DUMMY]Read btn_fd failed\n");
    }
    if(EVENT_HANDLE == 2 && read(btn_fd1, &inputVal1, sizeof(inputVal1)) > 0) {
        if(inputVal1.code != 0) {
            if(DEBUG) printf ("type :%d\ncode :%d\nvalue :%d\n", inputVal1.type, inputVal1.code, inputVal1.value);
            if(!keycode_map(inputVal1))
                ret = 1;
        }
    } else {
        //if(DEBUG) printf ("[DUMMY]Read btn_fd1 failed\n");
    }
    if(ret || ret1) {
        return NR_KEYS + 20;
    } else {
        return 0;
    }
}

static const char* keyboard_getstate (void)
{
    return (const char*)state;
}

/*
 * Mouse operations -- Event
*/
static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    if (mousex < 0) mousex = 0;
    if (mousey < 0) mousey = 0;
    //if (mousex > 239) mousex = 239; //fengwu del
    //if (mousey > 319) mousey = 319;
    if (mousex > TS_MAX_W) mousex = TS_MAX_W; //fengwu del
    if (mousey > TS_MAX_H) mousey = TS_MAX_H;
#ifdef _DEBUG
    // printf ("mousex = %d, mousey = %d/n", mousex, mousey);
#endif
    *x = mousex;
    *y = mousey;
    if(DEBUG) printf("ts mouse_getxy switchxy2. x:%d, y:%d\n", *x, *y);
}

static int dummy_mouse_getbutton(void)
{
    //#define IAL_MOUSE_LEFTBUTTON    1
    //#define IAL_MOUSE_RIGHTBUTTON   2
    //#define IAL_MOUSE_MIDDLEBUTTON  4
    if(DEBUG) printf("------- ts dummy_mouse_getbutton 2: press(4->2):%d, x-y:(%d-%d)\n", 
            ts_event.pressure, ts_event.x, ts_event.y);
    return ts_event.pressure;
}

#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except, struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except, struct timeval *timeout)
#endif
{
#if 0
    struct ts_sample sample;
    int retvalue = 0;
    int ret = 0;
    int fd = 0;
    fd_set rfds;
    int e;
    EVENT_HANDLE = 0;
    if(!in) {
        in = &rfds;
        FD_ZERO(in);
    }

    if((which & IAL_KEYEVENT) && btn_fd > 0){
        FD_SET(btn_fd, in);
#ifdef _LITE_VERSION
        if(btn_fd > maxfd) maxfd = btn_fd;
#endif
    }

    if((which & IAL_KEYEVENT) && btn_fd1 > 0){
        FD_SET(btn_fd1, in);
#ifdef _LITE_VERSION
        if(btn_fd1 > maxfd) maxfd = btn_fd1;
#endif
    }
    if(ts) {
        fd = ts_fd(ts);
    } else {
        fd = 0;
    }
    if ((which & IAL_MOUSEEVENT) && fd > 0) {
        FD_SET (fd, in);
#ifdef _LITE_VERSION
        if (fd > maxfd) maxfd = fd;
#endif
    }

#ifdef _LITE_VERSION
    e = select(maxfd + 1, in, out, except, timeout);
#else
    e = select(FD_SETSIZE, in, out, except, timeout) ;
#endif
    if(e > 0) {
        //keys
        if(btn_fd > 0 && FD_ISSET(btn_fd, in)) {
            //unsigned char key;
            FD_CLR(btn_fd, in);
            //read(btn_fd, &key, sizeof(key));
            //btn_state = key;
            retvalue |= IAL_KEYEVENT;
            EVENT_HANDLE = 1;
        } else if(btn_fd1 > 0 && FD_ISSET(btn_fd1, in)) {
            FD_CLR(btn_fd1, in);
            retvalue |= IAL_KEYEVENT;
            EVENT_HANDLE = 2;
        } else if (fd > 0 && FD_ISSET (fd, in) && ts != NULL) {
            FD_CLR (fd, in);
            ts_event.x=0;
            ts_event.y=0;
            ret = ts_read(ts, &sample, 1);
            if (ret < 0) {
                perror("ts_read()");
                return -1;
            }
            //ts_event.x = sample.x;
            //ts_event.y = sample.y;
            ts_event.pressure = (sample.pressure > 0 ? 1:0);
            // fengwu exchange x/y and set button to left ( instead of middle)                  
            ts_event.x = sample.y;
            ts_event.y = TS_MAX_H-sample.x;
            if((ts_event.x >= 0 && ts_event.x <= TS_MAX_W) && (ts_event.y >= 0 && ts_event.y <= TS_MAX_H)) {
                mousex = ts_event.x;
                mousey = ts_event.y;
                // printf("ts_event.x is %d, ts_event.y is %d------------------------------------->/n",ts_event.x ,ts_event.y);
            }
            //#ifdef _DEBUG
            //    if (ts_event.pressure > 0) {
            //  printf ("mouse down: ts_event.x = %d, ts_event.y = %d,ts_event.pressure = %d/n",ts_event.x,ts_event.y,ts_event.pressure);
            //   }
            //#endif
            retvalue |= IAL_MOUSEEVENT;
            {
                //fengwu check
                static int oldx=0, oldy=0, oldpressure=0;
                if(oldx == ts_event.x && oldy == ts_event.y && oldpressure == ts_event.pressure)
                {
                    //printf("duplicate event in wait evt. return -1\n");
                    return -1;
                }
                oldx = ts_event.x ; 
                oldy = ts_event.y ; 
                oldpressure = ts_event.pressure;
                if(DEBUG) printf("wait evt. x:%d(old:%d), y:%d(old:%d), pressure:%d(old:%d)\n", 
                        ts_event.x, oldx, ts_event.y, oldy, ts_event.pressure,oldpressure );
            }
            //return (ret);
        }
    } else if(e < 0) {
        return -1;
    }

    if(DEBUG) printf("[DUMMY]the value of retvalue = 0x%x\n", retvalue);
    return retvalue;
#else
    return 0;
#endif
}

BOOL InitDummyInput (INPUT* input, const char* mdev, const char* mtype)
{
#if 0
	char *key_rollover_msg= NULL;
	char *key_device = NULL;
	char *key_power_device = NULL;
    /* input hardware should be initialized before this function is called */
    char *ts_device = NULL;
    if ((ts_device = getenv("TSLIB_TSDEVICE")) != NULL) {
        // open touch screen event device in blocking mode
        ts = ts_open(ts_device, 0);
        printf("InitDummyInput open done.%s\n", ts_device);
    }
    if(DEBUG) printf("ts: %x\n", ts);

    if (!ts) {
        perror("ts_open()");
    } else {
        if(DEBUG) printf ("TSLIB_TSDEVICE is open!!!!!!!!!!!\n");
        if (ts_config(ts)) {
            perror("ts_config()");
        }   
    }

    //init the key input	
    memset(state, 0, NR_KEYS + 20);

	if ((key_rollover_msg = getenv("KEYS_ROLLOVER")) != NULL) {
		if(!strcmp("yes", key_rollover_msg))
			keys_rollover = 0x01;
	}
	printf("[DUMMY]msg:show the keys rollover=0x%x\n", keys_rollover);

    //btn_fd = open(KEY_INPUT, O_RDONLY|O_NOCTTY);
	if((key_device = getenv("KEY_EVENT")) != NULL)
		btn_fd = open(key_device, O_RDONLY|O_NOCTTY);
    if(btn_fd < 0) {
        if(DEBUG) printf("[DUMMY]error:key's output event open failed!\n");
        return FALSE;
    }

    //btn_fd1 = open(KEY_INPUT_POWER, O_RDONLY|O_NOCTTY);
	if((key_power_device = getenv("PWRKEY_DEVICE")) != NULL)
		btn_fd1 = open(key_power_device, O_RDONLY|O_NOCTTY);
    if(btn_fd1 < 0) {
        if(DEBUG) printf("[DUMMY]error:power-key's output event open failed!\n");
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = dummy_mouse_getbutton;
    input->set_mouse_range = NULL;
    mousex = 0;
    mousey = 0;
    ts_event.x = ts_event.y = ts_event.pressure = 0;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;
    input->wait_event = wait_event;
#endif
    return TRUE;
}

void TermDummyInput (void)
{
#if 0
    if(btn_fd >= 0)
        close(btn_fd);

    if(btn_fd1 >= 0)
        close(btn_fd1);

    if(ts)
        ts_close(ts);
#endif
}

#endif /* _DUMMY_IAL */
