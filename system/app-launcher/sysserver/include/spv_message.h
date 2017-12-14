#ifndef __SPV_MESSAGE_H__
#define __SPV_MESSAGE_H__

#include "spv_common.h"

//#define MSG_DESTORY 		0x00
//
//#define MSG_KEYDOWN			0x0010
//#define MSG_KEYUP			0x0012
//#define MSG_KEYLONGPRESS	0x0016
//
//#define MSG_USER			0x0800
//
///*##### self-defined MSGs#####*/
//#define MSG_GET_UI_STATUS	(MSG_USER + 3)
//#define MSG_SHUTDOWN		(MSG_USER + 4)
//#define MSG_IR_LED			(MSG_USER + 5)
//#define MSG_COLLISION		(MSG_USER + 6)
//#define MSG_CHARGE_STATUS	(MSG_USER + 10)
//#define MSG_BATTERY_STATUS	(MSG_USER + 11)
//#define MSG_SDCARD_MOUNT	(MSG_USER + 20)
//#define MSG_HDMI_HOTPLUG	(MSG_USER + 30)
//#define MSG_REAR_CAMERA		(MSG_USER + 31)
//#define MSG_REVERSE_IMAGE	(MSG_USER + 32)
//#define MSG_GPS				(MSG_USER + 33)
//#define MSG_ACC_CONNECT		(MSG_USER + 34)
//
//#define MSG_PAIRING_SUCCEED		(MSG_USER + 101)
//#define MSG_SHUTTER_PRESSED		(MSG_USER + 104)
//#define MSG_MEDIA_UPDATED	(MSG_USER + 108)
//#define MSG_CAMERA_CALLBACK		(MSG_USER + 109)
/*##### self-defined MSGs end #####*/

#define HMSG_INVALID 0
#define HMSG_MAX_NUM 128

typedef int (*MSGPROC)(HMSG msgHandler, int msgId, int wParam, long lParam);

typedef struct _SpvMessage {
    int msgId;
    int wParam;
    int lParam;
} SpvMessage;
typedef SpvMessage* PSpvMessage;

typedef struct _SpvMsgNode {
    PSpvMessage msg;
    struct _SpvMsgNode *next;
} SpvMsgNode;
typedef SpvMsgNode* PSpvMsgNode;

typedef struct _SpvMsgQueue {
    int msgCount;
    PSpvMsgNode head;
    PSpvMsgNode tail;
} SpvMsgQueue;

typedef struct _SpvHandler {
    int valid;
    SpvMsgQueue msgQueue;
    MSGPROC MsgProc;

    pthread_mutex_t queueMutex;
    pthread_cond_t queueCond;
} SpvHandler;


HMSG SpvRegisterHandler(MSGPROC msgProc);
int SpvUnregisterHandler(HMSG handler);

PSpvMessage PeekMessage(HMSG handler);
int SpvMessageLoop(HMSG handler);

int SpvPostMessage(HMSG handler, int msgId, int wParam, long lParam);

int SpvSendMessage(HMSG handler, int msgId, int wParam, long lParam);

#endif
