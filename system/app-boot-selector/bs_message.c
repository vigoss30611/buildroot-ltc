#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include <sys/prctl.h>

#include "bs_message.h"
#include "bs_debug.h"

static HMSG g_hmsg_list[HMSG_MAX_NUM] = {0};

/*static SpvMsgQueue g_msg_queue = {0, NULL, NULL};

static pthread_mutex_t g_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_queue_notempty = PTHREAD_COND_INITIALIZER;*/

static void AddMsgHandler(HMSG handler)
{
    int i = 0;
    for(i = 0; i < HMSG_MAX_NUM; i++) {
        if(g_hmsg_list[i] == 0) {
            g_hmsg_list[i] = handler;
            return;
        }
    }

    if(i == HMSG_MAX_NUM) {
        ERROR("Msg handler list is FULL, unregister unused hanlder first, handler: %ld\n", handler);
    }
}

static void RemoveMsgHandler(HMSG hanlder)
{
    int i = 0;
    for(i = 0; i < HMSG_MAX_NUM; i++) {
        if(g_hmsg_list[i] == hanlder) {
            g_hmsg_list[i] = 0;
            return;
        }
    }

    if(i == HMSG_MAX_NUM)
        ERROR("Msg not found in list, no need to remove, hanlder: %ld\n", hanlder);
}

static int IsValidHandler(HMSG hanlder)
{
    if(hanlder == 0)
        return 0;

    int i = 0;
    for(i = 0; i < HMSG_MAX_NUM; i++) {
        if(g_hmsg_list[i] = hanlder)
            return 1;
    }

    return 0;
}

static int SpvFreeMsgQueue(SpvHandler *pHandler)
{
    pthread_cond_signal(&pHandler->queueCond);
    pthread_mutex_lock(&pHandler->queueMutex);
    PSpvMsgNode node = pHandler->msgQueue.head;
    while(node != NULL) {
        PSpvMsgNode next = node->next;
        free(node);
        node = next;
    }
    memset(&pHandler->msgQueue, 0, sizeof(SpvMsgQueue));
    pthread_mutex_unlock(&pHandler->queueMutex);
}

HMSG SpvRegisterHandler(MSGPROC msgProc)
{
    SpvHandler *pHandler = (SpvHandler *)malloc(sizeof(SpvHandler));
    if(pHandler == NULL)
        return HMSG_INVALID;
    memset(pHandler, 0, sizeof(SpvHandler));

    pHandler->valid = 1;
    memset(&pHandler->msgQueue, 0, sizeof(SpvMsgQueue));
    pHandler->MsgProc = msgProc;
    pthread_mutex_init(&pHandler->queueMutex, NULL);
    pthread_cond_init(&pHandler->queueCond, NULL);

    AddMsgHandler((HMSG)pHandler);

    return (HMSG)pHandler;
}

int SpvUnregisterHandler(HMSG handler)
{
    if(IsValidHandler(handler)) {
        SpvHandler *pHandler = (SpvHandler *)handler;
        RemoveMsgHandler(handler);
        SpvFreeMsgQueue(pHandler);
        free(pHandler);
    }

    return 0;
}

static int AddMessage(HMSG handler, PSpvMessage message)
{
    if(handler == 0 || message == NULL)
        return -1;

    SpvHandler *pHandler = (SpvHandler *)handler;

    PSpvMsgNode pNode = (PSpvMsgNode)malloc(sizeof(SpvMsgNode));
    if(pNode == NULL) {
        ERROR("malloc for pNode failed\n");
        return -1;
    }
    //INFO("pHandler->msgQueue.head: %ld, pNode: %ld, count: %d\n", pHandler->msgQueue.head, pNode, pHandler->msgQueue.msgCount);
    memset(pNode, 0, sizeof(SpvMsgNode));
    pNode->msg = message;
    pNode->next = NULL;

    pthread_mutex_lock(&pHandler->queueMutex);
    if(pHandler->msgQueue.msgCount <= 0) {
        pHandler->msgQueue.head = pNode;
        pHandler->msgQueue.tail = pNode;
    } else {
        pHandler->msgQueue.tail->next = pNode;
        pHandler->msgQueue.tail = pNode;
    }

    pHandler->msgQueue.msgCount++;

    pthread_cond_signal(&pHandler->queueCond);
    pthread_mutex_unlock(&pHandler->queueMutex);

    return 0;
}

static int RemoveMessage(HMSG handler, PSpvMessage message)
{
	//INFO("the entry of RemoveMessage\n");
    int ret = -1;
    SpvHandler *pHandler = (SpvHandler *)handler;
    if(handler == 0 || message == NULL || pHandler->msgQueue.msgCount <= 0)
        return ret;
    
    PSpvMsgNode pPrevNode = NULL;
    PSpvMsgNode pNode = pHandler->msgQueue.head;
    while(pNode != NULL) {
        if(pNode->msg == message) {
            //INFO("Message found to remove\n");
            if(message == pHandler->msgQueue.head->msg) {//head
                pHandler->msgQueue.head = pNode->next;
            }
            if(message == pHandler->msgQueue.tail->msg) {//tail
                pHandler->msgQueue.tail = pPrevNode;
            }
            if(pPrevNode != NULL) {
                pPrevNode->next = pNode->next;
            }
            pHandler->msgQueue.msgCount --;

            free(pNode);
            pNode == NULL;
            ret = 0;
            break;
        }
        pPrevNode = pNode;
        pNode = pNode->next;
    }

    free(message);

	//INFO("leave RemoveMessage\n");
    return ret;    
}

PSpvMessage PeekMessage(HMSG handler)
{
    int ret = 0;
    PSpvMessage msg = NULL;
    SpvHandler *pHandler = (SpvHandler *)handler;
    pthread_mutex_lock(&pHandler->queueMutex);

    if(pHandler->msgQueue.msgCount <= 0) {
        //INFO("msg queue is empty, wait---\n");
        pthread_cond_wait(&pHandler->queueCond, &pHandler->queueMutex);
    }
    if(pHandler->msgQueue.head != NULL) {
        msg = (pHandler->msgQueue.head)->msg;
    }

    pthread_mutex_unlock(&pHandler->queueMutex);

    return msg;
}


/**
 * Launcher message loop, this func will block the thread
 **/
int SpvMessageLoop(HMSG handler)
{
    prctl(PR_SET_NAME, __func__);
    if(handler == 0) {
        ERROR("handler == NULL, not valid handler\n");
        return -1;
    }

    while(1) {
        PSpvMessage msg = PeekMessage(handler);
        if(msg == NULL)
            continue;
		//INFO("PeekMessage, handler: %ld, msgId: %d\n", handler, msg->msgId);
        int ret = ((SpvHandler *)handler)->MsgProc(handler, msg->msgId, msg->wParam, msg->lParam);
        int messageId = msg->msgId;
        RemoveMessage(handler, msg);

        if(messageId == MSG_USER_DESTROY) {
            INFO("MSG_USER_DESTROY received, exit message loop\n");
            break;
        }
    }

    SpvUnregisterHandler(handler);

    return 0;
}


int SpvSendMessage(HMSG handler, int msgId, int wParam, long lParam)
{
    //INFO("HMSG: %ld, msgId: %d, wParam: %d, lParam: %ld\n", handler, msgId, wParam, lParam);
    if(IsValidHandler(handler)) {
        return ((SpvHandler *)handler)->MsgProc(handler, msgId, wParam, lParam);
    }

    return -1;
}

int SpvPostMessage(HMSG handler, int msgId, int wParam, long lParam)
{
    //INFO("HMSG: %ld, msgId: %d, wParam: %d, lParam: %ld\n", handler, msgId, wParam, lParam);
    if(!IsValidHandler(handler)) {
        ERROR("invalid handler, HMSG: %ld, msgId: %d, wParam: %d, lParam: %ld\n", handler, msgId, wParam, lParam);
        return -1;
    }

    PSpvMessage msg = (PSpvMessage)malloc(sizeof(SpvMessage));
    if(msg == NULL) {
        ERROR("malloc message failed\n");
        return -1;
    }

    msg->msgId = msgId;
    msg->wParam = wParam;
    msg->lParam = lParam;

    AddMessage(handler, msg);
}


