/****************************************************************************
*              Copyright 2007 - 2011, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_mtrans_task_list.c
* Description: process on media transition trask list.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-02-13   W54723     NULL         creat this file.
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "hi_type.h"
#include "hi_mtrans.h"
#include "hi_mtrans_task_list.h"
#include "hi_mtrans_error.h"
//#include "hi_log_app.h"
//#include "hi_debug_def.h"
#include "ext_defs.h"
#include "hi_defs.h"
//#include "Hi_log_def.h"

static MTRANS_TASK_S   *g_astrSendTask = NULL ;           /*transition send tasks list pointer*/
static List_Head_S     g_strTaskFree ; /*free session list head*/
static List_Head_S     g_strTaskBusy ;/*busy session list head*/
static pthread_mutex_t g_TaskListLock;                      /*session list lock*/


HI_VOID DEBUGMTRANS_GetTaskListNum(HI_U32 *pu32FreeNum,HI_U32 *pu32BusyNum)
{
    List_Head_S* pos = NULL;
    HI_U32 u32FreeNum = 0;
    HI_U32 u32BusyNum = 0;
    
    (HI_VOID)pthread_mutex_lock(&g_TaskListLock);
    /*check the appointed task exit or not*/
    HI_List_For_Each(pos, &g_strTaskBusy)
    {
        u32BusyNum++;
    }

    HI_List_For_Each(pos, &g_strTaskFree)
    {
        u32FreeNum++;
    }
    (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);
    *pu32FreeNum = u32FreeNum;
    *pu32BusyNum = u32BusyNum;
    return ;
}
/* MediaType : [MTRANS_STREAM_VIDEO | MTRANS_STREAM_AUDIO | MTRANS_STREAM_DATA]*/

INLINE HI_VOID MTRANS_SetTaskState(MTRANS_TASK_S *pTaskHandle, HI_S32 MediaType,MTRANS_TASK_STATE_E enState)
{
    pTaskHandle->enTaskState[MediaType] = enState;
    return ;
}

INLINE HI_VOID MTRANS_GetTaskState(const MTRANS_TASK_S *pTaskHandle, HI_S32 MediaType,MTRANS_TASK_STATE_E* penState)
{

    *penState = pTaskHandle->enTaskState[MediaType];
    return ;
}

HI_BOOL MTRANS_TaskCheck(const MTRANS_TASK_S *pTaskHandle)
{
    List_Head_S* pos = NULL;
    MTRANS_TASK_S *pTempTaskHandle = NULL;

    (HI_VOID)pthread_mutex_lock(&g_TaskListLock);
    /*check the appointed task exit or not*/
    HI_List_For_Each(pos, &g_strTaskBusy)
    {
        /*get the node*/
        pTempTaskHandle = HI_LIST_ENTRY(pos,MTRANS_TASK_S,mtrans_sendtask_list);
        
        if(pTempTaskHandle == pTaskHandle)
        {
            (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);
            return HI_TRUE;
        }
    }

    (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);
    return HI_FALSE;
}


/*remove a task: del it from busy list and add to free list*/
HI_VOID HI_MTRANS_TaskRemoveFromList(MTRANS_TASK_S *pTask)
{
    /*free*/
    List_Head_S *pTaskList;
    List_Head_S *pos;

    //to do: pos pSessList哪一个是从busylist被摘除的节点
    pTaskList = &(pTask->mtrans_sendtask_list);

    (HI_VOID)pthread_mutex_lock(&g_TaskListLock);

    HI_List_For_Each(pos, &g_strTaskBusy)
    {
        /*get the node from busy list*/
        if(pos == pTaskList)
        {
            if(pTask->enTransMode == MTRANS_MODE_MULTICAST)
            {
                pthread_mutex_lock(&pTask->struUdpTransInfo.multicastLock);
                if(--pTask->struUdpTransInfo.clientNum == 0)
                {
                    while(pTask->struUdpTransInfo.threadstatus > 0)
                    {
                        pthread_cond_wait(&pTask->struUdpTransInfo.multicastCond, &pTask->struUdpTransInfo.multicastLock);
                    }
                    
                    if(pTask->u32MbuffHandle)
                    {
                        HI_BufAbstract_Free((int)pTask->liveMode, (HI_VOID**)&pTask->u32MbuffHandle);
                        pTask->u32MbuffHandle = 0;
                    }
                    
                    int i;
                    pTask->enTaskState[MTRANS_STREAM_VIDEO] = TRANS_TASK_STATE_INIT;
                    pTask->enTaskState[MTRANS_STREAM_AUDIO] = TRANS_TASK_STATE_INIT;
                    pTask->enTaskState[MTRANS_STREAM_DATA] = TRANS_TASK_STATE_INIT;
                    for(i = 0; i<MTRANS_STREAM_MAX; i++)
                    {
                        if(-1 != pTask->struUdpTransInfo.as32SendSock[i])
                        {
                            close(pTask->struUdpTransInfo.as32SendSock[i]);
                            pTask->struUdpTransInfo.as32SendSock[i] = -1;
                        }
                        if(-1 != pTask->struUdpTransInfo.as32RecvSock[i])
                        {
                            close(pTask->struUdpTransInfo.as32RecvSock[i]);
                            pTask->struUdpTransInfo.as32RecvSock[i] = -1;
                        }
                    }
                    pTask->struUdpTransInfo.chno = 0;
                    pTask->struUdpTransInfo.streamType = 0;
                    pTask->struUdpTransInfo.InitFlag = 0;
                    if(!MTRANS_MODE_BROADCAST == pTask->enTransMode 
                        && NULL != pTask->pu8PackBuff)
                    {
                        //printf("mtrans free for pack malloc  = %X\n ",pTaskHandle->pu8PackBuff)	;
                        free(pTask->pu8PackBuff);
                        pTask->pu8PackBuff = NULL;
                    }
                    
                    pthread_cond_destroy(&pTask->struUdpTransInfo.multicastCond);
                    pthread_mutex_destroy(&pTask->struUdpTransInfo.multicastLock);
                    HI_List_Del(pos);
                    HI_List_Add(pos, &g_strTaskFree);
                    break;
                }
                pthread_mutex_unlock(&pTask->struUdpTransInfo.multicastLock);
            }
            else
            {
                /*del the node from busy list，and add to free list*/
                HI_List_Del(pos);
                HI_List_Add(pos, &g_strTaskFree);
                break;
            }
        }
    }
    (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);
}
#if 0
HI_VOID* MTRANS_UdpSendProc(HI_VOID * pAvg)
{
    HI_S32 s32Ret = 0;
    MTRANS_TASK_S *pTaskHandle = NULL;
    MTRANS_TASK_STATE_E enVedioState = TRANS_TASK_STATE_BUTT;
    MTRANS_TASK_STATE_E enAudioState = TRANS_TASK_STATE_BUTT;
    MTRANS_TASK_STATE_E enDataState = TRANS_TASK_STATE_BUTT;
    MBUF_HEADER_S *pMBuffHandle  = NULL;
    HI_U8*    pBuffAddr     = NULL;          /*pointer to addr of stream ready to send */
    HI_U32     u32DataLen      = 0;          /*len of stream ready to send*/
    HI_U64     u64Pts          = 0;          /*stream pts*/
    MBUF_PT_E  enMbuffPayloadType = MBUF_PT_BUTT;    /*payload type*/
    HI_SOCKET s32WriteSock = -1;
    struct sockaddr* pPeerSockAddr = NULL;
    HI_U32 u32LastSn = 0;

    if(NULL == pAvg)
    {
        return NULL;
    }
    
    pTaskHandle = (MTRANS_TASK_S*)pAvg;

    HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"udp mtrans task %p for cli %s started ...\n ",
                pTaskHandle,pTaskHandle->au8CliIP);

    /*get all media's state of the trans task */
    MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_VIDEO,&enVedioState);
    MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_AUDIO,&enAudioState);
    MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_DATA,&enDataState); 
    pMBuffHandle =(MBUF_HEADER_S*)(pTaskHandle->u32MbuffHandle);

   /* 1 if one of meida types has in playing state, get meida from mbuff*/
    while( (TRANS_TASK_STATE_PLAYING == enVedioState )
       ||( TRANS_TASK_STATE_PLAYING == enAudioState)
       ||( TRANS_TASK_STATE_PLAYING == enDataState)
      )
    {
        /*request data from mediabuff */
        s32Ret = MBUF_Read(pMBuffHandle, (HI_ADDR*)(&pBuffAddr), &u32DataLen, &u64Pts, &enMbuffPayloadType);
        /*there is no data ready */
        if(HI_SUCCESS != s32Ret)
        {
           //printf("MBUF_Read read no data \n");
           
           (HI_VOID)usleep(20000);
           continue;
        }
        
        /*to do:  do some statistic*/
        DEBUGBeforeTransSave(pBuffAddr,u32DataLen,enMbuffPayloadType,1);
        
        if(VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
        {
            s32WriteSock = pTaskHandle->struUdpTransInfo.as32SendSock[MTRANS_STREAM_VIDEO];
            pPeerSockAddr = pTaskHandle->struUdpTransInfo.struCliAddr[MTRANS_STREAM_VIDEO];
            u32LastSn = (pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_VIDEO])++;
        }
        else if(AUDIO_FRAME == enMbuffPayloadType)
        {
            s32WriteSock = pTaskHandle->struUdpTransInfo.as32SendSock[MTRANS_STREAM_AUDIO];
            pPeerSockAddr = pTaskHandle->struUdpTransInfo.struCliAddr[MTRANS_STREAM_AUDIO];
            u32LastSn = (pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_AUDIO])++;
        }
        else if(MD_FRAME == enMbuffPayloadType)
        {
            s32WriteSock = pTaskHandle->struUdpTransInfo.as32SendSock[MTRANS_STREAM_DATA];
            pPeerSockAddr = pTaskHandle->struUdpTransInfo.struCliAddr[MTRANS_STREAM_DATA];
            u32LastSn = (pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_DATA])++;
        }
        else
        {
            HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"udp mtrans task %p for cli %s withdraw"\
                        "for invalid data type %d from mbuff \n ",
                        pTaskHandle,pTaskHandle->au8CliIP,enMbuffPayloadType);
            goto err;            
        }
        
        /*2 packet the stream*/
        s32Ret = MTRANS_PacketStream(pTaskHandle,&pBuffAddr,&u32DataLen,u64Pts,u32LastSn,enMbuffPayloadType);
        if(s32Ret != HI_SUCCESS)
        {
            HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"udp mtrans task %p for cli %s withdraw for packet error %X\n ",
                pTaskHandle,pTaskHandle->au8CliIP,s32Ret);
            goto err;
        }
        
        DEBUGBeforeTransSave(pBuffAddr,u32DataLen,enMbuffPayloadType,2);
        
        /*3 sending the media*/
        s32Ret = HI_MTRANS_Send(s32WriteSock,pBuffAddr,u32DataLen,pPeerSockAddr);
        if(s32Ret != HI_SUCCESS)
        {
             HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"udp mtrans task %p for cli %s withdraw for udp send error %X\n ",
                pTaskHandle,pTaskHandle->au8CliIP,s32Ret);
            goto err;
        }
         /*send success, then notice MBUF to free the buff*/
        else
        {
            MBUF_Set(pMBuffHandle);
        }
    }

    err:
    /*if pcaket this slice stream error, skip it and send next slice */
    MBUF_Set(pMBuffHandle);
    /*to do:notify vod 发送出错*/
    return NULL;
}
#endif

/*create session: get a unused node from the free session list*/
HI_S32 HI_MTRANS_TaskCreat(MTRANS_TASK_S **ppTask )
{
    List_Head_S *plist = NULL;
    
    /*lock the creat process*/
    (HI_VOID)pthread_mutex_lock(&g_TaskListLock);
    
    if(HI_List_Empty(&g_strTaskFree))
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,
                   "can't creat new task:over preset max num\n");
        (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);            
        return HI_ERR_MTRANS_OVER_TASK_NUM;            
    }
    else
    {
        /*get a new task from free list,and add to busy list*/
        plist = g_strTaskFree.next;
        HI_List_Del(plist);
        HI_List_Add(plist, &g_strTaskBusy);
        *ppTask = HI_LIST_ENTRY(plist, MTRANS_TASK_S, mtrans_sendtask_list);
    }
    (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);

    return HI_SUCCESS;
}


/*free the resource the session possess: close socket,
  and remove the session from busy list*/
HI_S32 HI_MTRANS_TaskDestroy(MTRANS_TASK_S *pTaskHandle)
{
    HI_S32 i = 0;
    if(NULL == pTaskHandle)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }

    /*check the appointed task exit or not*/
    if(HI_TRUE != MTRANS_TaskCheck(pTaskHandle))
    {
        HI_LOG_SYS(MTRANS_SEND,LOG_LEVEL_ERR,"destroy task with illegale task handel %p.\n",
            pTaskHandle);
        return HI_ERR_MTRANS_ILLEGALE_TASK;
    }
    
    /*if it is tcp trans type, record corresponding info*/
    if(MTRANS_MODE_TCP_ITLV == pTaskHandle->enTransMode)
    {
        /*set all meida state as init*/
        pTaskHandle->enTaskState[MTRANS_STREAM_VIDEO] = TRANS_TASK_STATE_INIT;
        pTaskHandle->enTaskState[MTRANS_STREAM_AUDIO] = TRANS_TASK_STATE_INIT;
        pTaskHandle->enTaskState[MTRANS_STREAM_DATA] = TRANS_TASK_STATE_INIT;
    }
    /*if it is udp trans type, record corresponding info*/
    else if(MTRANS_MODE_UDP == pTaskHandle->enTransMode )
    {   
        /*set all meida state as init*/
        pTaskHandle->enTaskState[MTRANS_STREAM_VIDEO] = TRANS_TASK_STATE_INIT;
        pTaskHandle->enTaskState[MTRANS_STREAM_AUDIO] = TRANS_TASK_STATE_INIT;
        pTaskHandle->enTaskState[MTRANS_STREAM_DATA] = TRANS_TASK_STATE_INIT;
        for(i = 0; i<MTRANS_STREAM_MAX; i++)
        {
            if(-1 != pTaskHandle->struUdpTransInfo.as32SendSock[i])
            {
                close(pTaskHandle->struUdpTransInfo.as32SendSock[i]);
                pTaskHandle->struUdpTransInfo.as32SendSock[i] = -1;
            }
            if(-1 != pTaskHandle->struUdpTransInfo.as32RecvSock[i])
            {
                close(pTaskHandle->struUdpTransInfo.as32RecvSock[i]);
                pTaskHandle->struUdpTransInfo.as32RecvSock[i] = -1;
            }
        }
    }
    /*if it is broadcast trans type, record corresponding info*/
    else if (MTRANS_MODE_BROADCAST == pTaskHandle->enTransMode)
    {
        /*to do :增加broadcast传输模式后,补充该部分*/
    }

    /*clear task info */
    if(!MTRANS_MODE_BROADCAST == pTaskHandle->enTransMode && NULL != pTaskHandle->pu8PackBuff)
    {
        //printf("mtrans free for pack malloc  = %X\n ",pTaskHandle->pu8PackBuff)	;
        free(pTaskHandle->pu8PackBuff);
        pTaskHandle->pu8PackBuff = NULL;
    }
    /*remove the node from busy list*/
    HI_MTRANS_TaskRemoveFromList(pTaskHandle);

    return HI_SUCCESS;
}

/*init send task list*/
HI_S32 HI_MTRANS_TaskListInit(HI_S32 s32TotalTaskNum)
{

    int i = 0;

    /*if parameters is <=0, or over max supported task number,
      set default task num */
    if(s32TotalTaskNum <= 0 || s32TotalTaskNum > MTRANS_MAX_TASK_NUM)
    {
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_INFO,"appointed send task num %d illegale, set to default value %d.\n",
                   s32TotalTaskNum,MTRANS_DEFAULT_TASK_NUM);
        s32TotalTaskNum = MTRANS_DEFAULT_TASK_NUM;           
    }

    
    /*calloc for mtrans task list*/
    g_astrSendTask = (MTRANS_TASK_S *)MTRANS_CALLOC((HI_U32)s32TotalTaskNum,sizeof(MTRANS_TASK_S));
    if(NULL == g_astrSendTask)
    {
        DEBUG_MTRANS_PRINT(MTRANS_SEND ,LOG_LEVEL_ERR,
                    "mtrans task list init error :calloc failed for %d tasks num\n",s32TotalTaskNum);
        return HI_ERR_MTRANS_CALLOC_FAILED;
    }
    else
    {
        memset(g_astrSendTask,0,sizeof(MTRANS_TASK_S)*((HI_U32)s32TotalTaskNum));
    }

    /*init send task  busy and free list*/
    HI_LIST_HEAD_INIT_PTR(&g_strTaskFree);
    HI_LIST_HEAD_INIT_PTR(&g_strTaskBusy);
    (HI_VOID)pthread_mutex_init(&g_TaskListLock,  NULL);

    /*add send task( with supported send task num) into free list*/
    for(i = 0; i<s32TotalTaskNum; i++)
    {
        HI_List_Add_Tail(&g_astrSendTask[i].mtrans_sendtask_list, &g_strTaskFree);
    }
    
    return HI_SUCCESS;
}

/*all busy trans task destroy: */
HI_VOID HI_MTRANS_TaskAllDestroy()
{
    MTRANS_TASK_S* pTempTask = NULL;
    List_Head_S *pos = NULL;
	List_Head_S *n = NULL;

     /*lock the destroy process*/
   //BTT: pthread_mutex_lock(&g_TaskListLock);
    
    HI_List_For_Each_Safe(pos,n,&g_strTaskBusy)
    {
        /*get the send task*/
        pTempTask = HI_LIST_ENTRY(pos, MTRANS_TASK_S, mtrans_sendtask_list);

        pthread_mutex_lock(&g_TaskListLock);
        if(pTempTask->enTransMode == MTRANS_MODE_MULTICAST
            && pTempTask->struUdpTransInfo.clientNum>1)
        {
            pthread_mutex_lock(&pTempTask->struUdpTransInfo.multicastLock);
            pTempTask->struUdpTransInfo.clientNum = 1;
            pthread_mutex_unlock(&pTempTask->struUdpTransInfo.multicastLock);
        }
        pthread_mutex_unlock(&g_TaskListLock);

        /*destroy the task*/
        (HI_VOID)HI_MTRANS_TaskDestroy(pTempTask);
    }

    /*free mtrans tasks list*/
    MTRANS_FREE(g_astrSendTask);
    g_astrSendTask = NULL;
  //  pthread_mutex_unlock(&g_TaskListLock);

}

HI_S32 HI_MTRANS_TaskCreat_Multicast(MTRANS_TASK_S **ppTask, int channel, int streamType)
{
    List_Head_S *plist = NULL;
    MTRANS_TASK_S *pTempTaskHandle = NULL;

    (HI_VOID)pthread_mutex_lock(&g_TaskListLock);
    /*check the appointed task exit or not*/
    HI_List_For_Each(plist, &g_strTaskBusy)
    {
        /*get the node*/
        pTempTaskHandle = HI_LIST_ENTRY(plist,MTRANS_TASK_S,mtrans_sendtask_list);
        
        if(pTempTaskHandle->enTransMode == MTRANS_MODE_MULTICAST
            &&pTempTaskHandle->struUdpTransInfo.chno == channel
            && pTempTaskHandle->struUdpTransInfo.streamType == streamType)
        {
            *ppTask = pTempTaskHandle;
            pthread_mutex_lock(&pTempTaskHandle->struUdpTransInfo.multicastLock);
            pTempTaskHandle->struUdpTransInfo.clientNum++;
            pthread_mutex_unlock(&pTempTaskHandle->struUdpTransInfo.multicastLock);
            (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);
            
            return HI_SUCCESS;
        }
    }

    if(HI_List_Empty(&g_strTaskFree))
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,
                   "can't creat new task:over preset max num\n");
        (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);            
        return HI_ERR_MTRANS_OVER_TASK_NUM;            
    }
    else
    {
        /*get a new task from free list,and add to busy list*/
        plist = g_strTaskFree.next;
        HI_List_Del(plist);
        HI_List_Add(plist, &g_strTaskBusy);
        pTempTaskHandle = HI_LIST_ENTRY(plist, MTRANS_TASK_S, mtrans_sendtask_list);
        pthread_cond_init(&pTempTaskHandle->struUdpTransInfo.multicastCond, NULL);
        pthread_mutex_init(&pTempTaskHandle->struUdpTransInfo.multicastLock,  NULL);
        pTempTaskHandle->struUdpTransInfo.chno = channel;
        pTempTaskHandle->struUdpTransInfo.streamType = streamType;
        pTempTaskHandle->struUdpTransInfo.clientNum = 1;
        pTempTaskHandle->struUdpTransInfo.InitFlag = 0;
        pTempTaskHandle->struUdpTransInfo.threadstatus = 0;
        *ppTask = pTempTaskHandle;
    }
    (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);

    return HI_SUCCESS;
}

HI_S32 HI_MTRANS_TaskCreat_Multicast_ext(MTRANS_TASK_S **ppTask, int channel, int streamType, unsigned short vPort)
{
    List_Head_S *plist = NULL;
    MTRANS_TASK_S *pTempTaskHandle = NULL;

    (HI_VOID)pthread_mutex_lock(&g_TaskListLock);
    /*check the appointed task exit or not*/
    HI_List_For_Each(plist, &g_strTaskBusy)
    {
        /*get the node*/
        pTempTaskHandle = HI_LIST_ENTRY(plist,MTRANS_TASK_S,mtrans_sendtask_list);
        
        if(pTempTaskHandle->enTransMode == MTRANS_MODE_MULTICAST
            && pTempTaskHandle->struUdpTransInfo.chno == channel
            && pTempTaskHandle->struUdpTransInfo.streamType == streamType
            && pTempTaskHandle->struUdpTransInfo.cliMediaPort[0] == vPort)
        {
            *ppTask = pTempTaskHandle;
            pthread_mutex_lock(&pTempTaskHandle->struUdpTransInfo.multicastLock);
            pTempTaskHandle->struUdpTransInfo.clientNum++;
            pthread_mutex_unlock(&pTempTaskHandle->struUdpTransInfo.multicastLock);
            (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);
            
            return HI_SUCCESS;
        }
    }

    
    if(HI_List_Empty(&g_strTaskFree))
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,
                   "can't creat new task:over preset max num\n");
        (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);            
        return HI_ERR_MTRANS_OVER_TASK_NUM;            
    }
    else
    {
        /*get a new task from free list,and add to busy list*/
        plist = g_strTaskFree.next;
        HI_List_Del(plist);
        HI_List_Add(plist, &g_strTaskBusy);
        pTempTaskHandle = HI_LIST_ENTRY(plist, MTRANS_TASK_S, mtrans_sendtask_list);
        pthread_cond_init(&pTempTaskHandle->struUdpTransInfo.multicastCond, NULL);
        pthread_mutex_init(&pTempTaskHandle->struUdpTransInfo.multicastLock,  NULL);
        pTempTaskHandle->struUdpTransInfo.chno = channel;
        pTempTaskHandle->struUdpTransInfo.streamType = streamType;
        pTempTaskHandle->struUdpTransInfo.clientNum = 1;
        pTempTaskHandle->struUdpTransInfo.InitFlag = 0;
        pTempTaskHandle->struUdpTransInfo.threadstatus = 0;
        pTempTaskHandle->enTransMode = MTRANS_MODE_MULTICAST;
        *ppTask = pTempTaskHandle;
    }
    (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);

    return HI_SUCCESS;
}

HI_S32 HI_MTRANS_GetMulticastTaskHandle(MTRANS_TASK_S **ppTask, int channel, int streamType, unsigned short vPort)
{
    List_Head_S *plist = NULL;
    MTRANS_TASK_S *pTempTaskHandle = NULL;

    (HI_VOID)pthread_mutex_lock(&g_TaskListLock);
    /*check the appointed task exit or not*/
    HI_List_For_Each(plist, &g_strTaskBusy)
    {
        /*get the node*/
        pTempTaskHandle = HI_LIST_ENTRY(plist,MTRANS_TASK_S,mtrans_sendtask_list);
        
        if(pTempTaskHandle->enTransMode == MTRANS_MODE_MULTICAST
            && pTempTaskHandle->struUdpTransInfo.chno == channel
            && pTempTaskHandle->struUdpTransInfo.streamType == streamType
            && pTempTaskHandle->struUdpTransInfo.cliMediaPort[0] == vPort)
        {
            *ppTask = pTempTaskHandle;
            (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);
            
            return HI_SUCCESS;
        }
    }

    (HI_VOID)pthread_mutex_unlock(&g_TaskListLock);

    return HI_FAILURE;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

