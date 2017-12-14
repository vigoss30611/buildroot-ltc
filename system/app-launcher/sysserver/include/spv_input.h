#ifndef __SPV_INPUT_H__
#define __SPV_INPUT_H__

#include "spv_common.h"
#include "spv_message.h"

#define IAL_KEYEVENT	0x01

typedef struct _KEYEVENT {
	int event;
	int scancode;
	DWORD status;
}KEYEVENT;
typedef KEYEVENT* PKEYEVENT;
#define KE_KEYMASK              0x000F
#define KE_KEYDOWN              0x0001
#define KE_KEYUP                0x0002
#define KE_KEYLONGPRESS         0x0004

typedef union _LWEVENTDATA {
	KEYEVENT ke;
}LWEVENTDATA;

typedef struct _LWEVENT {
	int type;
	int count;
	LWEVENTDATA data;
}LWEVENT;
typedef LWEVENT* PLWEVENT;

//Low level event type
#define LWETYPE_TIMEOUT		0
#define LWETYPE_KEY			1

typedef unsigned int __u32;
typedef int __s32;
typedef unsigned short __u16;

struct input_event {
	struct timeval time;
	__u16 type;
	__u16 code;
	__s32 value;
};

#ifndef NR_KEYS
#define NR_KEYS			128	//brief The number of keys defined by Linux operating system.
#endif

#define SCANCODE_USER				(NR_KEYS + 1)
/*##### self-defined scancode by Jeason @ infotm#####*/
#define SCANCODE_INFOTM_MENU		(SCANCODE_USER + 1)
#define SCANCODE_INFOTM_MODE		(SCANCODE_USER + 2)
#define SCANCODE_INFOTM_OK			(SCANCODE_USER + 3)
#define SCANCODE_INFOTM_DOWN		(SCANCODE_USER + 4)
#define SCANCODE_INFOTM_UP			(SCANCODE_USER + 5)
#define SCANCODE_INFOTM_POWER		(SCANCODE_USER + 6)
#define SCANCODE_INFOTM_SHUTDOWN	(SCANCODE_USER + 7)
#define SCANCODE_INFOTM_RESET		(SCANCODE_USER + 8)
#define SCANCODE_INFOTM_SWITCH		(SCANCODE_USER + 9)
#define SCANCODE_INFOTM_LOCK		(SCANCODE_USER + 10)
#define SCANCODE_INFOTM_PHOTO		(SCANCODE_USER + 11)
#define SCANCODE_INFOTM_WIFI		(SCANCODE_USER + 12)
/*##### self-defined scancode end #####*/

//#define INPUT_DEBUG
#ifdef INPUT_DEBUG
#define dbg(format,...) printf("[SPV_INPUT](%d):"format, __LINE__, ##__VA_ARGS__)
#else
#define dbg(format,...)
#endif

int KeyReportKeyUp(int keycode);
int InitDummyInput();
void TermDummyInput();
int InitKeyServer(HMSG h);
void TerminateKeyServer();
#endif
