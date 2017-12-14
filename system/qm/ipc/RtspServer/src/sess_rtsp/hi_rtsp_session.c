/****************************************************************************
*              Copyright 2007 - 2011, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_RTSP_session.c
* Description: The rtsp server.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-01-30   W54723     NULL         Create this file.
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "hi_type.h"
//btlib
//#include "hi_log_app.h"
//#include "hi_debug_def.h"
#include "ext_defs.h"
#include "miscfunc.h"
//#include "hi_socket.h"
//#include "hi_adpt_interface.h"
#include "hi_rtsp_session.h"
#include "hi_msession_rtsp_error.h"
#include<sys/socket.h>
#include "hi_log_def.h"

//RTSP_LIVE_SESSION   g_astrRTSPSess[RTSP_MAX_SESS_NUM];      /*rtsp sessions */
//static List_Head_S     g_strSessionFree;                       /*free session list head*/
static List_Head_S     g_strSessionBusy;                       /*busy session list head*/
static pthread_mutex_t g_SessionListLock;                      /*session list lock*/

HI_VOID RTSP_InitSessID(HI_CHAR* pszSessId)
{
    mtrans_random_id(pszSessId, 15);
}

#if 0
HI_S32 HI_RTSP_SessionGetInfo(HI_CHAR **ppInfoStr,HI_S32* ps32InfoLen)
{
   VOD_SESSION_S *pSess = NULL;
   List_Head_S *pos = NULL;
   HI_CHAR *pTempPtr = au8CliInfo;
   HI_S32 s32CliNum = 0;
   HI_S32 s32FreeSessNum = 0;

   pthread_mutex_lock(&g_SessionListLock);

    HI_List_For_Each(pos, &g_strSessionBusy)
    {
        pSess = HI_LIST_ENTRY(pos, VOD_SESSION_S, rtsp_sess_list);

        /*get the node from busy list*/
        pTempPtr +=sprintf(pTempPtr,"Num:%d  cliIp:%s   ",++s32CliNum,pSess->au8CliIP);

        if(RTP_TRANSPORT_TCP == pSess->enStreamTransType)
        {
            pTempPtr +=sprintf(pTempPtr,"   tranType:tcp ");
        }
        else if(RTP_TRANSPORT_UDP == pSess->enStreamTransType)
        {
            pTempPtr +=sprintf(pTempPtr,"   tranType:udp ");
        }

        
        pTempPtr +=sprintf(pTempPtr,"requVideo:%d   requAudio:%d    requMDAlarm:%d",pSess->bRequestStreamFlag[VOD_RTP_STREAM_VIDEO],
                pSess->bRequestStreamFlag[VOD_RTP_STREAM_AUDIO],pSess->bRequestStreamFlag[VOD_RTP_STREAM_DATA]);
        
        pTempPtr +=sprintf(pTempPtr,"   sendThdId:%d  loseFramNum:%llu",pSess->s32SessThdId,pSess->pMBuffHandle->discard_count);

        pTempPtr +=sprintf(pTempPtr,"\r\n\r\n");
    }
    pthread_mutex_unlock(&g_SessionListLock);

    /*statistic for free session number*/
    pthread_mutex_lock(&g_SessionListLock);

    HI_List_For_Each(pos, &g_strSessionFree)
    {
        pSess = HI_LIST_ENTRY(pos, VOD_SESSION_S, rtsp_sess_list);

        ++s32FreeSessNum;
    }
    pthread_mutex_unlock(&g_SessionListLock);

    pTempPtr +=sprintf(pTempPtr,"free session num: %d.\n",s32FreeSessNum);


    *ppInfoStr = au8CliInfo;
    *ps32InfoLen = strlen(au8CliInfo);

    return HI_SUCCESS;
}

#endif
/*get currect valid connect number*/
HI_S32 HI_RTSP_SessionGetConnNum()
{
    HI_S32 s32ValidConnNum = 0;
    List_Head_S *pos = NULL;
    
    (HI_VOID)pthread_mutex_lock(&g_SessionListLock);

    HI_List_For_Each(pos, &g_strSessionBusy)
    {
        ++s32ValidConnNum;
    }    
    (HI_VOID)pthread_mutex_unlock(&g_SessionListLock);

    return s32ValidConnNum;
}

/*init session*/
HI_S32 HI_RTSP_SessionListInit(HI_S32 s32TotalConnNum)
{
#if 0
    int i = 0;
    
    /*real conn num over max conn num */
    if(RTSP_MAX_SESS_NUM < s32TotalConnNum )
    {
        HI_LOG_SYS(RTSP_SESS_MODEL,LOG_LEVEL_FATAL,"RTSP session list init:"    \
                "max supported connect num %d illegale.\n",s32TotalConnNum);
        return HI_ERR_RTSP_OVER_CONN_NUM;
    }

    if(s32TotalConnNum <= 0)
    {
        HI_LOG_SYS(RTSP_SESS_MODEL,LOG_LEVEL_FATAL,"max supported connect num %d illegale.\n",
                   s32TotalConnNum);
        return HI_ERR_RTSP_ILLEGALE_CONN_NUM;           
    }

    memset(g_astrRTSPSess,0,sizeof(RTSP_LIVE_SESSION)*RTSP_MAX_SESS_NUM);
#endif

    /*init session busy and free list*/
    //HI_LIST_HEAD_INIT_PTR(&g_strSessionFree);
    HI_LIST_HEAD_INIT_PTR(&g_strSessionBusy);
    (HI_VOID)pthread_mutex_init(&g_SessionListLock,  NULL);

    /*add sessions( with supported conn num) into free list*/
    /*
    for(i = 0; i<s32TotalConnNum; i++)
    {
        HI_List_Add_Tail(&g_astrRTSPSess[i].RTSP_sess_list, &g_strSessionFree);
    }
    */
    
    return HI_SUCCESS;
}

/*remove a session: del it from busy list and add to free list*/
HI_VOID HI_RTSP_SessionListRemove(RTSP_LIVE_SESSION *pSession)
{
    /*free*/
    List_Head_S *pSessList;
    List_Head_S *pos;

    //to do: pos pSessList哪一个是从busylist被摘除的节点
    pSessList = &(pSession->RTSP_sess_list);

    (HI_VOID)pthread_mutex_lock(&g_SessionListLock);

    HI_List_For_Each(pos, &g_strSessionBusy)
    {
        /*get the node from busy list*/
        if(pos == pSessList)
        {
            /*del the node to busy list，and add to free list*/
            HI_List_Del(pos);
            //HI_List_Add(pos, &g_strSessionFree);
            break;
        }
    }
    (HI_VOID)pthread_mutex_unlock(&g_SessionListLock);

    free(pSession);
}

/*create session: get a unused node from the free session list*/
/*to do : 如何返回一个session 的指针*/
HI_S32 HI_RTSP_SessionCreat(RTSP_LIVE_SESSION **ppSession )
{
    List_Head_S *plist = NULL;
    RTSP_LIVE_SESSION *pSess = NULL;
    
    /*lock the creat process*/
   (HI_VOID)pthread_mutex_lock(&g_SessionListLock);
    
    #if 0
    if(HI_List_Empty(&g_strSessionFree))
    {
        HI_LOG_SYS(RTSP_SESS_MODEL,LOG_LEVEL_ERR,
                    "can't creat session:over preset RTSP session num\n");
        (HI_VOID)pthread_mutex_unlock(&g_SessionListLock);            
        return HI_ERR_RTSP_OVER_CONN_NUM;            
    }
    else
    {
        /*get a session from free list,and add to busy list*/
        plist = g_strSessionFree.next;
        HI_List_Del(plist);
        HI_List_Add(plist, &g_strSessionBusy);
        *ppSession = HI_LIST_ENTRY(plist, RTSP_LIVE_SESSION, RTSP_sess_list);
        
    }
    #endif

    pSess = (RTSP_LIVE_SESSION *)calloc(1, sizeof(RTSP_LIVE_SESSION));
    if(!pSess)
    {
        printf("%s  %d, calloc failed!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
        return HI_ERR_RTSP_MALLOC;
    }
	memset(pSess, 0, sizeof(RTSP_LIVE_SESSION));
    *ppSession = pSess;
    HI_List_Add(&pSess->RTSP_sess_list, &g_strSessionBusy);
    
    (HI_VOID)pthread_mutex_unlock(&g_SessionListLock);

    return HI_SUCCESS;
}

/*init a appointed session*/
HI_VOID HI_RTSP_SessionInit(RTSP_LIVE_SESSION *pSession, HI_SOCKET s32SockFd)
{
    
    /*init session socket which responsbile for recv/send RTSP request msg,
      also, if stream is trans in interleaved way, stream will send through
      this socket too.*/
    pSession->s32SessSock = s32SockFd;

    /*init session ID*/
    RTSP_InitSessID(pSession->aszSessID);

    /*get peer host info */
    MT_SOCKET_GetPeerIPPort(s32SockFd, pSession->au8CliIP, &(pSession->s32CliRTSPPort));

    /*get local host info*/
    MT_SOCKET_GetHostIP(s32SockFd, pSession->au8HostIP);

    /*init sendbuff flag*/
    pSession->readyToSend = HI_FALSE;

    /*init RTSP session process thread state as running*/
    pSession->enSessThdState = RTSPSESS_THREAD_STATE_INIT;

    /*init session stat*/
    pSession->enSessState = RTSP_SESSION_STATE_INIT;

    /*default, believe fisrt msg has not recved before create RTSP session */
    pSession->bIsFirstMsgHasInRecvBuff = HI_FALSE;
    
    /*clear other field*/
    memset(pSession->au8UserAgent,0,RTSP_MAX_VERSION_LEN);
    memset(pSession->au8SvrVersion,0,RTSP_MAX_VERSION_LEN);
    memset(pSession->au8RecvBuff,0,RTSP_MAX_PROTOCOL_BUFFER);
    memset(pSession->au8SendBuff,0,RTSP_MAX_PROTOCOL_BUFFER);
    memset(pSession->au8CliUrl[0],0,256);
    memset(pSession->au8CliUrl[1],0,256);    
    memset(pSession->auFileName,0,128);
    
    pSession->last_recv_req = 0;
    pSession->last_recv_seq = 0;
    pSession->last_send_seq = 0;
    pSession->last_send_req = 0;
    pSession->heartTime = 0;
    
    pSession->bRequestStreamFlag[0] = HI_FALSE;
    pSession->bRequestStreamFlag[1] = HI_FALSE;
    pSession->remoteRTPPort[0] = 0;
    pSession->remoteRTPPort[1] = 0;
    pSession->remoteRTCPPort[0] = 0;
    pSession->remoteRTCPPort[1] = 0;
    pSession->enMediaTransMode = MTRANS_MODE_BUTT;
    pSession->bIsPlayback = HI_FALSE;
    pSession->s32PlayBackType = 0;
    pSession->start_offset = 0;
    pSession->s32WaitDoPause = 0;
    pSession->last_send_frametype = 0;
    memset(pSession->RealFileName, 0, 64);

    pSession->bIsTurnRelay = HI_FALSE;
    memset(pSession->au8DestIP, 0, sizeof(HI_IP_ADDR));
    
    return;
}

/*free the resource the session possess: close socket,
  and remove the session from busy list*/
HI_VOID HI_RTSP_SessionClose(RTSP_LIVE_SESSION *pSession)
{

    /*close socket for session */
    /*to do :socket是否可重复关闭*/
	if(pSession->s32SessSock != -1)
    {
        printf("<RTSP>Close the session socket :%d \n",pSession->s32SessSock);
        shutdown(pSession->s32SessSock,SHUT_RDWR );
        close(pSession->s32SessSock);
        pSession->s32SessSock = -1;
    }
	
    /*set session stat as ini*/
    pSession->enSessState = RTSP_SESSION_STATE_STOP;

    /*remove the session from busy list*/
    HI_RTSP_SessionListRemove(pSession);

}

HI_VOID HI_RTSP_SessionFreeCheck()
{
    //HI_LOG_DEBUG(RTSP_SESS_MODEL,LOG_LEVEL_NOTICE,"waiting for session list free.\n");
    /*check if every session exit success!!!*/
    while(!list_empty(&g_strSessionBusy))
    {
        (HI_VOID)usleep(10);
        
    }
    //HI_LOG_DEBUG(RTSP_SESS_MODEL,LOG_LEVEL_NOTICE,"session list free sucess.\n");
}

/*all busy session destroy: */
HI_VOID HI_RTSP_SessionAllDestroy()
{
    RTSP_LIVE_SESSION* pTempSession = NULL;
    List_Head_S *pos = NULL;

     /*lock the destroy process*/
    (HI_VOID)pthread_mutex_lock(&g_SessionListLock);
    
    HI_List_For_Each(pos, &g_strSessionBusy)
    {
        /*get the session*/
        pTempSession = HI_LIST_ENTRY(pos, RTSP_LIVE_SESSION, RTSP_sess_list);
        /*set the withdraw flag of corresponding 'RTSPSessionProcess' thread of the session*/
        pTempSession->enSessThdState = RTSPSESS_THREAD_STATE_STOP;
    }

    (HI_VOID)pthread_mutex_unlock(&g_SessionListLock);

    HI_RTSP_SessionFreeCheck();

}

/*copy the first msg,  which is recived by RTSP lisnsvr,
  to its corresponding session's recvbuff */
HI_S32 HI_RTSP_SessionCopyMsg(RTSP_LIVE_SESSION *pSession,HI_CHAR* pMsgBuffAddr,
                              HI_U32 u32MsgLen)
{
    if(NULL == pSession || NULL == pMsgBuffAddr)
    {
        return HI_ERR_RTSP_ILLEGALE_PARAM;
    }

    /*session recvbuff is smaller than msglen*/
    if(RTSP_MAX_PROTOCOL_BUFFER < u32MsgLen)
    {
        HI_LOG_SYS(RTSP_SESS_MODEL,LOG_LEVEL_ERR,"RTSP msg too long %d byte: over %d byte.\nmsg: %s.\n",u32MsgLen,RTSP_MAX_PROTOCOL_BUFFER,pMsgBuffAddr);
        return HI_ERR_RTSP_BUFF_SMALL;
    }
	memset(pSession->au8RecvBuff,0,RTSP_MAX_PROTOCOL_BUFFER);
	
    /*to do : 结尾是否需要加'\0'*/
    strncpy(pSession->au8RecvBuff,pMsgBuffAddr,u32MsgLen);
    
    return HI_SUCCESS;
}

HI_VOID HI_RTSP_ClearSendBuff(RTSP_LIVE_SESSION *pSession)
{
    memset(pSession->au8SendBuff, 0, RTSP_MAX_PROTOCOL_BUFFER);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

