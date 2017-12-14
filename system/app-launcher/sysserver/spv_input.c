#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "spv_debug.h"
#include "spv_input.h"
//#include "spv_server.h"
#include "spv_message.h"

pthread_t __info_keyevent;
static int loopKeyRun = 1;
HMSG keyHandler;

static unsigned char keystate[NR_KEYS + 20];
static unsigned char oldkeystate[NR_KEYS + 20];
static short KEYCODE = -1, KEYSTATUS = -1;
static int btn_fd = -1;
static int btn_fd1 =-1;
static int EVENT_HANDLE = 0;
static int keys_rollover = 0x00;

static int KeyCodeMap(struct input_event inputVal)
{
	switch(inputVal.code)
	{
		case 139:
			keystate[SCANCODE_INFOTM_MENU] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_MENU;
            return 0;
		//case 102:
        case 108:
            keystate[SCANCODE_INFOTM_DOWN] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_DOWN;
            return 0;
		case 114:	//for sportdv
			keystate[SCANCODE_INFOTM_OK] = inputVal.value ^ keys_rollover;
			KEYCODE = SCANCODE_INFOTM_OK;
			return 0;
        case 352:
            keystate[SCANCODE_INFOTM_OK] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_OK;
            return 0;
		case 115:	//for sportdv
			keystate[SCANCODE_INFOTM_UP] = inputVal.value ^ keys_rollover;
			KEYCODE = SCANCODE_INFOTM_UP;
			return 0;
        case 103:
            keystate[SCANCODE_INFOTM_UP] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_UP;
            return 0;
        case 116:
            keystate[SCANCODE_INFOTM_POWER] = inputVal.value ;
            KEYCODE = SCANCODE_INFOTM_POWER;
            return 0;
        case 0x175:
            keystate[SCANCODE_INFOTM_MODE] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_MODE;
            return 0;
        case 152:
            keystate[SCANCODE_INFOTM_LOCK] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_LOCK;
            return 0;
        case 212:
            keystate[SCANCODE_INFOTM_PHOTO] = inputVal.value ^ keys_rollover;
            KEYCODE = SCANCODE_INFOTM_PHOTO;
            return 0;
        default:
            return 1;

	}
	return 1;
}

static int InputUpdate()
{
	dbg("enter tht fuction InputUdate\n");
    char buf[48];
    struct input_event inputVal;
    struct input_event inputVal1;
    int ret = 0;
    int ret1 = 0;
    if(EVENT_HANDLE == 1 && read(btn_fd, &inputVal, sizeof(inputVal)) > 0) {
        if(inputVal.code != 0) {
            dbg("type :%d\ncode :%d\nvalue :%d\n", inputVal.type, inputVal.code, inputVal.value);
            if(!KeyCodeMap(inputVal))
                ret = 1;
        }
    } else {
        //dbg("Read btn_fd failed\n");
    }
    if(EVENT_HANDLE == 2 && read(btn_fd1, &inputVal1, sizeof(inputVal1)) > 0) {
        if(inputVal1.code != 0) {
            dbg("type :%d\ncode :%d\nvalue :%d\n", inputVal1.type, inputVal1.code, inputVal1.value);
            if(!KeyCodeMap(inputVal1))
                ret = 1;
        }
    } else {
        //dbg("[DUMMY]Read btn_fd1 failed\n");
    }
	dbg("leave tht fuction InputUdate\n");
    if(ret || ret1) {
        return NR_KEYS + 20;
    } else {
        return 0;
    }
}

static int InputWaitEvent (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except, struct timeval *timeout)
{
    int retvalue = 0;
    fd_set rfds;
    int e;
    EVENT_HANDLE = 0;
    if(!in) {
        in = &rfds;
        FD_ZERO(in);
    }

    if((which & IAL_KEYEVENT) && btn_fd > 0){
        FD_SET(btn_fd, in);
        if(btn_fd > maxfd) maxfd = btn_fd;
    }

    if((which & IAL_KEYEVENT) && btn_fd1 > 0){
        FD_SET(btn_fd1, in);
        if(btn_fd1 > maxfd) maxfd = btn_fd1;
    }

    e = select(maxfd + 1, in, out, except, timeout);

    if(e > 0) {
        //keys
        if(btn_fd > 0 && FD_ISSET(btn_fd, in)) {
            FD_CLR(btn_fd, in);
            retvalue |= IAL_KEYEVENT;
            EVENT_HANDLE = 1;
        } else if(btn_fd1 > 0 && FD_ISSET(btn_fd1, in)) {
            FD_CLR(btn_fd1, in);
            retvalue |= IAL_KEYEVENT;
            EVENT_HANDLE = 2;
        }
    } else if(e < 0) {
        return -1;
    }

    dbg("The value of retvalue = 0x%x\n", retvalue);
    return retvalue;
}

/****************** key events report *******************/
#define REPORT_MODE		1
#define REPORT_SHUT		2
#define REPORT_RESET	4
#define REPORT_ALWAYS	8
#define REPORT_SWITCH	16
#define REPORT_LOCK		32

pthread_t __info_longpress;
static unsigned char olddownkey = 0;
int sign_key = 0;
int longpress_num = 0;
int longpress_key = 0;

static void ParseEvent(PLWEVENT lwe)
{
	dbg("enter tht fuction ParseEvent\n");
	SpvMessage msg;
	PKEYEVENT ke;

	ke = &(lwe->data.ke);
	msg.handler = keyHandler;
	msg.wParam = 0;
	msg.lParam = 0;
	if(lwe->type == LWETYPE_KEY) {
		msg.wParam = ke->scancode;
		msg.lParam = ke->status;
		if(ke->event == KE_KEYDOWN) {
			msg.msgId = MSG_USER_KEYDOWN;
		} else if(ke->event == KE_KEYUP) {
			msg.msgId = MSG_USER_KEYUP;
		} else if(ke->event == KE_KEYLONGPRESS) {
			msg.msgId == MSG_USER_KEYLONGPRESS;
		}
		dbg("Post the key-event,handler=%d,msgId=%d,wP=%d,lP=%d;\n",msg.handler, msg.msgId, msg.wParam, msg.lParam);
		SpvPostMessage(msg.handler, msg.msgId, msg.wParam, msg.lParam);
	}
	dbg("leave tht fuction ParseEvent\n");
}

int KeyReportKeyUp(int keycode)
{
	LWEVENT lwe;
	lwe.type = LWETYPE_KEY;
	lwe.data.ke.event = KE_KEYUP;
	lwe.data.ke.scancode = keycode;
	lwe.data.ke.status = 1;
	ParseEvent(&lwe);
	return 0;
}

static void* InfotmLongPress(void* data)
{
	sign_key = 0;
	while(loopKeyRun) {	
		sem_wait((sem_t*)data);
		switch(longpress_key)
		{
			case SCANCODE_INFOTM_DOWN:
			case SCANCODE_INFOTM_UP:
				if(longpress_num > 4 && (longpress_num % 2)) {
					//report a key_up event every 200ms after 400ms
					dbg("[LONGPRESS_EVENT]report key event keycode=%d\n", longpress_key);
					sign_key |= REPORT_ALWAYS;
					KeyReportKeyUp(longpress_key);
				}
				break;
			case SCANCODE_INFOTM_MENU:
				if(longpress_num == 8 && (~sign_key & REPORT_MODE)) {
					dbg("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_MODE\n");
					sign_key |= REPORT_MODE; 
					KeyReportKeyUp(SCANCODE_INFOTM_MODE);
					longpress_key = 0;
				}
				break;
			case SCANCODE_INFOTM_OK:
                if(longpress_num == 8 && (~sign_key & REPORT_SWITCH)) {
					dbg("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_SWITCH\n");
					sign_key |= REPORT_SWITCH; 
					KeyReportKeyUp(SCANCODE_INFOTM_SWITCH);
					longpress_key = 0;
				}
				break;
			case SCANCODE_INFOTM_POWER:
				if(longpress_num == 25 && (~sign_key & REPORT_SHUT)) {
					dbg("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_SHUTDOWN\n");
					sign_key |= REPORT_SHUT;
					KeyReportKeyUp(SCANCODE_INFOTM_SHUTDOWN);
					longpress_key = 0;
				}
				break;
			case SCANCODE_INFOTM_PHOTO:
                if(longpress_num == 8 && (~sign_key & REPORT_LOCK)) {
					dbg("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_LOCK\n");
					sign_key |= REPORT_LOCK; 
					KeyReportKeyUp(SCANCODE_INFOTM_LOCK);
					longpress_key = 0;
				}
				/* define reset key-event
				if(longpress_num == 50 && (~sign_key & REPORT_RESET)) {
					dbg("[LONGPRESS_EVENT]report key event SCANCODE_INFOTM_RESET\n");
					sign_key |= REPORT_RESET; 
					KeyReportKeyUp(SCANCODE_INFOTM_RESET);
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

static int KeyGetEvent(int event, PLWEVENT lwe)
{
	dbg("enter tht fuction KeyGetEvent\n");
	PKEYEVENT ke = &(lwe->data.ke);
	int i;

	if(event & IAL_KEYEVENT) {
		int nr_keys  = InputUpdate();
		if(nr_keys == 0)
			goto Failed;
		lwe->type = LWETYPE_KEY;
		for(i = 1; i < nr_keys; i++) {
			if(!oldkeystate[i] && keystate[i]) {
				ke->event =  KE_KEYDOWN;
				ke->scancode = i;
				olddownkey = i;
				break;
			}
			if(oldkeystate[i] && !keystate[i]) {
				ke->event = KE_KEYUP;
				ke->scancode = i;
				break;
			}
		}
		if(i == nr_keys) {
			if(olddownkey == 0)
				goto Failed;
			ke->scancode = olddownkey;
			if(ke->event == 0)
				goto Failed;
		}
		memcpy(oldkeystate, keystate, nr_keys);
		dbg("leave tht fuction KeyGetEvent\n");
		return 1;
	}
Failed:
	dbg("leave tht fuction KeyGetEvent\n");
	return 0;
}

static void* KeyEventLoop(void* data)
{
	LWEVENT lwe;
	int event, err;
	int i = 0;
	sem_t longpress_wait;

	sem_post((sem_t*)data);

	sem_init(&longpress_wait, 0, 1);
	//create long press thread
	err = pthread_create(&__info_longpress, NULL, InfotmLongPress, &longpress_wait);

	while(loopKeyRun) {
		event = InputWaitEvent(IAL_KEYEVENT,0,NULL,NULL,NULL,NULL);
		if(event < 0)
			continue;

		if(event & IAL_KEYEVENT && KeyGetEvent(IAL_KEYEVENT, &lwe)) {
			lwe.data.ke.status = 0;
			if(lwe.data.ke.event == KE_KEYDOWN) {
				ParseEvent(&lwe);
				sem_wait(&longpress_wait);
				longpress_key = lwe.data.ke.scancode;
				sem_post(&longpress_wait);
				longpress_num = 0;
				sign_key = 0;
			} else if (lwe.data.ke.event == KE_KEYUP) {
				sem_wait(&longpress_key);
				longpress_key = 0;
				sem_post(&longpress_wait);
				usleep(50000);
				switch(lwe.data.ke.scancode)
				{
					case SCANCODE_INFOTM_DOWN:
					case SCANCODE_INFOTM_UP:
						if(~sign_key & REPORT_ALWAYS) {
							ParseEvent(&lwe);
						}
						break;
					case SCANCODE_INFOTM_MENU:
						if(~sign_key & REPORT_MODE) {
							ParseEvent(&lwe);
						}
						break;
					case SCANCODE_INFOTM_OK:
						if(sign_key & REPORT_SWITCH) {
							lwe.data.ke.status = 1;
						}
						ParseEvent(&lwe);
						break;
					case SCANCODE_INFOTM_POWER:
						if(~sign_key & REPORT_SHUT) {
							KeyReportKeyUp(SCANCODE_INFOTM_POWER);
						}
						break;
					case SCANCODE_INFOTM_PHOTO:
						if(~sign_key & REPORT_LOCK) {
							ParseEvent(&lwe);
						}
						break;
					default:
						ParseEvent(&lwe);
						break;
				}
				longpress_num = 0;
				sign_key = 0;
			}
		}
	}
	sem_destroy(&longpress_wait);
	return NULL;
}
/************************ end ***************************/


int InitDummyInput()
{
	char *key_rollover_msg= NULL;
	char *key_device = NULL;
	char *key_power_device = NULL;
    /* input hardware should be initialized before this function is called */
	//reset the key input	
    memset(keystate, 0, NR_KEYS + 20);
	memset(oldkeystate, 0 , NR_KEYS + 20);
	olddownkey = 0;

	if ((key_rollover_msg = getenv("KEYS_ROLLOVER")) != NULL) {
		if(!strcmp("yes", key_rollover_msg))
			keys_rollover = 0x01;
	}
	dbg("Show the keys rollover=0x%x\n", keys_rollover);

	if((key_device = getenv("KEY_EVENT")) != NULL)
		btn_fd = open(key_device, O_RDONLY|O_NOCTTY);
    if(btn_fd < 0) {
        dbg("Key's output event open failed!\n");
        return 0;
    }

	if((key_power_device = getenv("PWRKEY_DEVICE")) != NULL)
		btn_fd1 = open(key_power_device, O_RDONLY|O_NOCTTY);
    if(btn_fd1 < 0) {
        dbg("Power-key's output event open failed!\n");
        return 0;
    }

    return 1;
}

void TermDummyInput()
{
    if(btn_fd >= 0)
        close(btn_fd);

    if(btn_fd1 >= 0)
        close(btn_fd1);
}

int InitKeyServer(HMSG h)
{
	int err, ret;
	sem_t wait;

	if(!InitDummyInput()) {
		return -1;
	}
	
	keyHandler = h;
	loopKeyRun = 1;

	sem_init(&wait, 0, 0);
	err = pthread_create(&__info_keyevent, NULL, KeyEventLoop, &wait);
	sem_wait(&wait);

	sem_destroy(&wait);

	return 0;
}

void TerminateKeyServer()
{
	loopKeyRun = 0;
	TermDummyInput();
	if(pthread_cancel(__info_keyevent)!= 0) {
		ERROR("Cancel the infotm server thread failed!\n");
	}
	pthread_join(__info_keyevent, NULL);
}

