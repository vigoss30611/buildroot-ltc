/****************************************************************************
*              Copyright 2007 - 2011, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_rtsp_o_http_session.c
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

//#include "hi_log_app.h"
//#include "hi_debug_def.h"
//#include "miscfunc.h"

//#include "hi_socket.h"
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ext_defs.h"
#include "miscfunc.h"
//#include "hi_socket.h"
#include "hi_mt_socket.h"
#include "hi_log_def.h"
//#include "hi_adpt_interface.h"
#include "hi_rtsp_o_http_session.h"
#include "hi_msession_rtsp_o_http_error.h"


static RTSP_O_HTTP_LIVE_SESSION   g_astrRtspoHttpSess[RTSP_O_HTTP_MAX_SESS_NUM];      /*rtsp sessions */
static List_Head_S     g_strRtspSessionFree;                       /*free session list head*/
static List_Head_S     g_strRtspSessionBusy;                       /*busy session list head*/
static pthread_mutex_t g_RtspSessionListLock;                      /*session list lock*/

HI_VOID RTSP_O_HTTP_InitSessID(HI_CHAR* pszSessId)
{
    mtrans_random_id(pszSessId, 8);
}
HI_S32 HI_SOCKET_GetPeerIPPort(HI_SOCKET s, char* ip, unsigned short *port)
{
    HI_S32 namelen = 0;
    struct sockaddr_in addr;
    
    namelen = sizeof(struct sockaddr_in);
    getpeername(s, (struct sockaddr *)&addr, &namelen);

    
    *port = (unsigned short)ntohs(addr.sin_port);
    strcpy(ip, inet_ntoa( addr.sin_addr ));
    
    return HI_SUCCESS;
}

HI_S32 HI_SOCKET_GetHostIP(HI_SOCKET s, char* ip)
{
    HI_S32 namelen = 0;
    struct sockaddr_in addr;
    namelen = sizeof(struct sockaddr);
    getsockname(s, (struct sockaddr *)&addr, &namelen);
    strcpy(ip, inet_ntoa( addr.sin_addr ));

    return HI_SUCCESS;
}

#if 0
HI_S32 HI_RTSP_O_HTTP_SessionGetInfo(HI_CHAR **ppInfoStr,HI_S32* ps32InfoLen)
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

//bt-qt 根据sessiontag, 找相关的session
RTSP_O_HTTP_LIVE_SESSION* HTTP_LIVE_FindRelateSess(HI_CHAR* SessCookie)
{
    RTSP_O_HTTP_LIVE_SESSION* pRet= NULL;
	RTSP_O_HTTP_LIVE_SESSION* pFinded = NULL;
    
   // List_Head_S *pSessList;
    List_Head_S *pos;
    
    if (SessCookie == NULL )
    {
        return NULL;
    }

    (HI_VOID)pthread_mutex_lock(&g_RtspSessionListLock);

    HI_List_For_Each(pos, &g_strRtspSessionBusy)
    {
        pFinded = HI_LIST_ENTRY(pos, RTSP_O_HTTP_LIVE_SESSION, rtsp_o_http_sess_list);
        
        if (pFinded)
        {
            if (strcmp(pFinded->SessCookie, SessCookie)  == 0)
            {
                printf("<httplive>find relate , sessiontag: %s., pFinded:%p\n", 
                    SessCookie, pFinded);
                pRet = pFinded;
                break;
            }
        }
        
    }
    (HI_VOID)pthread_mutex_unlock(&g_RtspSessionListLock);    
    
    return pRet;
}


/*get currect valid connect number*/
HI_S32 HI_RTSP_O_HTTP_SessionGetConnNum()
{
    HI_S32 s32ValidConnNum = 0;
    List_Head_S *pos = NULL;
    
    (HI_VOID)pthread_mutex_lock(&g_RtspSessionListLock);

    HI_List_For_Each(pos, &g_strRtspSessionBusy)
    {
        ++s32ValidConnNum;
    }    
    (HI_VOID)pthread_mutex_unlock(&g_RtspSessionListLock);

    return s32ValidConnNum;
}

/*init session*/
HI_S32 HI_RTSP_O_HTTP_SessionListInit(HI_S32 s32TotalConnNum)
{

    int i = 0;
    
    /*real conn num over max conn num */
    if(RTSP_O_HTTP_MAX_SESS_NUM < s32TotalConnNum )
    {
        HI_LOG_SYS(RTSP_O_HTTP_SESS_MODEL,LOG_LEVEL_FATAL,"rtsp_o_http session list init:"    \
                "max supported connect num %d illegale.\n",s32TotalConnNum);
        return HI_ERR_RTSP_O_HTTP_OVER_CONN_NUM;
    }

    if(s32TotalConnNum <= 0)
    {
        HI_LOG_SYS(RTSP_O_HTTP_SESS_MODEL,LOG_LEVEL_FATAL,"max supported connect num %d illegale.\n",
                   s32TotalConnNum);
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_CONN_NUM;           
    }

    memset(g_astrRtspoHttpSess,0,sizeof(RTSP_O_HTTP_LIVE_SESSION)*RTSP_O_HTTP_MAX_SESS_NUM);

    /*init session busy and free list*/
    HI_LIST_HEAD_INIT_PTR(&g_strRtspSessionFree);
    HI_LIST_HEAD_INIT_PTR(&g_strRtspSessionBusy);
    (HI_VOID)pthread_mutex_init(&g_RtspSessionListLock,  NULL);

    /*add sessions( with supported conn num) into free list*/
    for(i = 0; i<s32TotalConnNum; i++)
    {
        HI_List_Add_Tail(&g_astrRtspoHttpSess[i].rtsp_o_http_sess_list, &g_strRtspSessionFree);
    }
    
    return HI_SUCCESS;
}

/*remove a session: del it from busy list and add to free list*/
HI_VOID HI_RTSP_O_HTTP_SessionListRemove(RTSP_O_HTTP_LIVE_SESSION *pSession)
{
    /*free*/
    List_Head_S *pSessList;
    List_Head_S *pos;

    //to do: pos pSessList哪一个是从busylist被摘除的节点
    pSessList = &(pSession->rtsp_o_http_sess_list);

    (HI_VOID)pthread_mutex_lock(&g_RtspSessionListLock);

    HI_List_For_Each(pos, &g_strRtspSessionBusy)
    {
        /*get the node from busy list*/
        if(pos == pSessList)
        {
            /*del the node to busy list，and add to free list*/
            HI_List_Del(pos);
            HI_List_Add(pos, &g_strRtspSessionFree);
            break;
        }
    }
    (HI_VOID)pthread_mutex_unlock(&g_RtspSessionListLock);
}

/*create session: get a unused node from the free session list*/
/*to do : 如何返回一个session 的指针*/
HI_S32 HI_RTSP_O_HTTP_SessionCreat(RTSP_O_HTTP_LIVE_SESSION **ppSession )
{
    List_Head_S *plist = NULL;
    
    /*lock the creat process*/
   (HI_VOID)pthread_mutex_lock(&g_RtspSessionListLock);
    
    if(HI_List_Empty(&g_strRtspSessionFree))
    {
        HI_LOG_SYS(RTSP_O_HTTP_SESS_MODEL,LOG_LEVEL_ERR,
                    "can't creat session:over preset rtsp_o_http session num\n");
        (HI_VOID)pthread_mutex_unlock(&g_RtspSessionListLock);            
        return HI_ERR_RTSP_O_HTTP_OVER_CONN_NUM;            
    }
    else
    {
        /*get a session from free list,and add to busy list*/
        plist = g_strRtspSessionFree.next;
        HI_List_Del(plist);
        HI_List_Add(plist, &g_strRtspSessionBusy);
        *ppSession = HI_LIST_ENTRY(plist, RTSP_O_HTTP_LIVE_SESSION, rtsp_o_http_sess_list);
        
    }
    (HI_VOID)pthread_mutex_unlock(&g_RtspSessionListLock);

    return HI_SUCCESS;
}

/*init a appointed session*/
HI_VOID HI_RTSP_O_HTTP_SessionInit(RTSP_O_HTTP_LIVE_SESSION *pSession, HI_SOCKET s32SockFd)
{

    /*init session socket which responsbile for recv/send rtsp_o_http request msg,
    also, if stream is trans in interleaved way, stream will send through
    this socket too.*/
    pSession->s32GetConnSock = s32SockFd;

    /*init session ID*/
    RTSP_O_HTTP_InitSessID(pSession->aszSessID);

    /*get peer host info */
    //HI_SOCKET_GetPeerIPPort(s32SockFd, pSession->au8CliIP, &(pSession->s32CliHttpPort));

    /*get local host info*/
    HI_SOCKET_GetHostIP(s32SockFd, pSession->au8HostIP);

    /*init sendbuff flag*/
    pSession->readyToSend = HI_FALSE;

    /*init rtsp_o_http session process thread state as running*/
    pSession->enSessThdState = RTSP_O_HTTPSESS_THREAD_STATE_INIT;

    /*init session stat*/
    pSession->enSessState = RTSP_O_HTTP_SESSION_STATE_INIT;

    pSession->bIsCmdMsgInRecvBuff = HI_FALSE;

    pSession->s32PostConnSock = -1;

    /*clear other field*/
    memset(pSession->au8UserAgent,0,RTSP_O_HTTP_MAX_VERSION_LEN);
    memset(pSession->au8SvrVersion,0,RTSP_O_HTTP_MAX_VERSION_LEN);
    memset(pSession->au8RecvBuff,0,RTSP_O_HTTP_MAX_PROTOCOL_BUFFER);
    memset(pSession->au8SendBuff,0,RTSP_O_HTTP_MAX_PROTOCOL_BUFFER);
    memset(pSession->au8CmdRecvBuf,0,RTSP_O_HTTP_MAX_PROTOCOL_BUFFER);
    memset(pSession->SessCookie,0,RTSP_O_HTTP_COOKIE_LEN);
    memset(pSession->au8FileName,0,8);

    pSession->last_recv_req = 0;
    pSession->last_recv_seq = 0;
    pSession->last_send_seq = 0;
    pSession->last_send_req = 0;
    pSession->sessMediaType = 0;
    pSession->pGetConnHandle = 0;
    pSession->pPostConnHandle = 0;

    return;
}

/*free the resource the session possess: close socket,
  and remove the session from busy list*/
HI_VOID HI_RTSP_O_HTTP_SessionClose(RTSP_O_HTTP_LIVE_SESSION *pSession)
{

    /*close socket for session */
    /* socket由HTTPD自己关掉*/
    
    pSession->s32GetConnSock = -1;
    pSession->s32PostConnSock = -1;
    /*set session stat as ini*/
    pSession->enSessState = RTSP_O_HTTP_SESSION_STATE_STOP;

    /*remove the session from busy list*/
    HI_RTSP_O_HTTP_SessionListRemove(pSession);

}

HI_VOID HI_RTSP_O_HTTP_SessionFreeCheck()
{
    //HI_LOG_DEBUG(RTSP_O_HTTP_SESS_MODEL,LOG_LEVEL_NOTICE,"waiting for session list free.\n");
    /*check if every session exit success!!!*/
    while(!list_empty(&g_strRtspSessionBusy))
    {
        (HI_VOID)usleep(10);
        
    }
    //HI_LOG_DEBUG(RTSP_O_HTTP_SESS_MODEL,LOG_LEVEL_NOTICE,"session list free sucess.\n");
}

/*all busy session destroy: */
HI_VOID HI_RTSP_O_HTTP_SessionAllDestroy()
{
    RTSP_O_HTTP_LIVE_SESSION* pTempSession = NULL;
    List_Head_S *pos = NULL;

     /*lock the destroy process*/
    (HI_VOID)pthread_mutex_lock(&g_RtspSessionListLock);
    
    HI_List_For_Each(pos, &g_strRtspSessionBusy)
    {
        /*get the session*/
        pTempSession = HI_LIST_ENTRY(pos, RTSP_O_HTTP_LIVE_SESSION, rtsp_o_http_sess_list);

        /*set the withdraw flag of corresponding 'rtsp_o_httpSessionProcess' thread of the session*/
        pTempSession->enSessThdState = RTSP_O_HTTPSESS_THREAD_STATE_STOP;
        
    }

    (HI_VOID)pthread_mutex_unlock(&g_RtspSessionListLock);

    HI_RTSP_O_HTTP_SessionFreeCheck();

}

/*copy the first msg,  which is recived by rtsp_o_http lisnsvr,
  to its corresponding session's recvbuff */
HI_S32 HI_RTSP_O_HTTP_SessionCopyMsg(RTSP_O_HTTP_LIVE_SESSION *pSession,HI_CHAR* pMsgBuffAddr,
                              HI_U32 u32MsgLen)
{
    if(NULL == pSession || NULL == pMsgBuffAddr)
    {
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_PARAM;
    }

    /*session recvbuff is smaller than msglen*/
    if(RTSP_O_HTTP_MAX_PROTOCOL_BUFFER < u32MsgLen)
    {
        HI_LOG_SYS(RTSP_O_HTTP_SESS_MODEL,LOG_LEVEL_ERR,"rtsp_o_http msg too long %d byte: over %d byte.\nmsg: %s.\n",u32MsgLen,RTSP_O_HTTP_MAX_PROTOCOL_BUFFER,pMsgBuffAddr);
        return HI_ERR_RTSP_O_HTTP_BUFF_SMALL;
    }

    /*to do : 结尾是否需要加'\0'*/
    strncpy(pSession->au8RecvBuff,pMsgBuffAddr,u32MsgLen);
    
    return HI_SUCCESS;
}

HI_VOID HI_RTSP_O_HTTP_ClearSendBuff(RTSP_O_HTTP_LIVE_SESSION *pSession)
{
    memset(pSession->au8SendBuff, 0, RTSP_O_HTTP_MAX_PROTOCOL_BUFFER);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

