#ifndef __SPV_MESSAGE_H__
#define __SPV_MESSAGE_H__

#define HMSG_INVALID 0
#define HMSG_MAX_NUM 128

#define NR_KEYS 128
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



typedef long 			HMSG;

typedef unsigned long	DWORD;
typedef int 			BOOL;

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
