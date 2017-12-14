/****************************************************************************
*              Copyright 2006 - , Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_rtsp_o_http_session.h
* Description: The rtsp server.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-01-30   W54723     NULL         creat this file.
*****************************************************************************/
#ifndef  __HI_RTSP_O_HTTP_SESSION_H__
#define  __HI_RTSP_O_HTTP_SESSION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_list.h"
#include "hi_mtrans.h"

#define RTSP_O_HTTP_SESS_MODEL  "RTSP_O_HTTP_SESS"

#define RTSP_O_HTTP_MAX_SESS_NUM   16  /*最大session数，根据编码路数及图像大小的不同，
                                  实际支持的最大session数往往小于该值 */
#define RTSP_O_HTTP_MAX_SESSID_LEN 16  /*sessionid号的最大长度*/
#define RTSP_O_HTTP_MAX_VERSION_LEN 128 /*版本号的最大长度*/

#define RTSP_TRACKID_VIDEO 0        /*Setup 消息中的track 信息*/
#define RTSP_TRACKID_AUDIO 1

typedef char  RTSP_O_HTTP_URI[256];         /*URI数据类型 rtsp://xxx:333/222 */
typedef char  RTSP_O_HTTP_SESSION_ID[RTSP_O_HTTP_MAX_SESSID_LEN];

/*rtsp_o_http的socket收发buffer大小*/
#define RTSP_O_HTTP_MAX_PROTOCOL_BUFFER 1024
/*rtsp_o_http请求buffer*/
typedef char RTSP_O_HTTP_REQ_BUFFER[RTSP_O_HTTP_MAX_PROTOCOL_BUFFER];
/*RTSP回复buffer*/
typedef RTSP_O_HTTP_REQ_BUFFER RTSP_O_HTTP_RESP_BUFFER;

/*rtsp_o_http会话的状态机*/
typedef enum hiRTSP_O_HTTP_SESSION_STATE
{
    RTSP_O_HTTP_SESSION_STATE_INIT    = 0, /*已被创建*/
    RTSP_O_HTTP_SESSION_STATE_READY   = 1, 
    RTSP_O_HTTP_SESSION_STATE_PLAY = 2, /*创建并运行*/
    RTSP_O_HTTP_SESSION_STATE_STOP = 3,    /*已停止并销毁*/
    RTSP_O_HTTP_SESSION_STATE_BUTT
    
}RTSP_O_HTTP_SESSION_STATE;

/*rtsp_o_http会话线呈状态机*/
typedef enum hiRTSP_O_HTTP_LIVESESS_THREAD_STATE
{
    RTSP_O_HTTPSESS_THREAD_STATE_INIT    = 0, /*已被创建*/
    RTSP_O_HTTPSESS_THREAD_STATE_RUNNING = 1, /*创建并运行*/
    RTSP_O_HTTPSESS_THREAD_STATE_STOP = 2, /*已停止并销毁*/
    RTSP_O_HTTPSESS_THREAD_STATE_BUTT
}RTSP_O_HTTPSESS_THREAD_STATE; 
#define RTSP_O_HTTP_COOKIE_LEN 64
typedef struct hiRTSP_O_HTTP_LIVE_SESSION 
{           
    /*用于维护链表*/
    List_Head_S rtsp_o_http_sess_list;

    /*该Session所对应的监控通道号*/
    HI_S32         s32VsChn;

    /*线呈id,该线呈负责rtsp交互及数据发送*/
    pthread_t          s32SessThdId;

    /*上述线呈的状态*/
    RTSP_O_HTTPSESS_THREAD_STATE enSessThdState;
    
    /*该session所对应的session id, 由链接创建时候随机产生*/
    RTSP_O_HTTP_SESSION_ID aszSessID;

    //mhua add
    HI_CHAR SessCookie[RTSP_O_HTTP_COOKIE_LEN];
    HI_SOCKET s32PostConnSock;//用来接收rtsp请求的fd
    HI_BOOL bIsCmdMsgInRecvBuff;//记录当前recvBuf中有由s32CmdSock接收到的数据
    HI_CHAR au8CmdRecvBuf[RTSP_O_HTTP_MAX_PROTOCOL_BUFFER];
    /*客户端的ip地址*/
    HI_IP_ADDR     au8CliIP;
    
    /*客户端的rtsp_o_http链接端口*/
    HI_PORT        s32CliHttpPort;

    /*本地ip*/
    HI_IP_ADDR      au8HostIP; 

    /*该session对应的svr端会话socket，在链接创建时候被赋值*/
    HI_SOCKET       s32GetConnSock;

    /*该rtsp_o_http session的状态*/
    RTSP_O_HTTP_SESSION_STATE enSessState;

    /*是否请求视频， 音频的标志*/
    //HI_BOOL    bRequestStreamFlag[VOD_RTP_STREAM_MAX];

    /*该session中， 用户输入的url， rtsp://xxxxxx:553/1 */
    RTSP_O_HTTP_URI         au8CliUrl;

    HI_CHAR         au8UserAgent[RTSP_O_HTTP_MAX_VERSION_LEN];

    HI_CHAR         au8SvrVersion[RTSP_O_HTTP_MAX_VERSION_LEN];

    HI_CHAR         au8FileName[8];

    HI_BOOL         bIsFirstMsgHasInRecvBuff;

    /*rtsp接收命令的buffer*/
    RTSP_O_HTTP_REQ_BUFFER  au8RecvBuff;
    HI_U8                   sessMediaType;
    /*rtsp发送命令的buffer*/
    RTSP_O_HTTP_RESP_BUFFER au8SendBuff;

    /*发送命令buffer是否可发送标志*/
    HI_BOOL readyToSend;

    /*媒体数据传输是否是interleaved方式的，在协商传输方式
       时赋值 0-不是，1-是*/
    HI_BOOL bIsIntlvTrans;

    /*传输任务handle,只有当媒体数据是interleaved方式时有效*/
    /*note:当媒体数据是interleaved方式时，媒体数据在会话线呈中处理*/
    HI_U32 u32TransHandle;

    /*媒体数据传输处理函数，interleaved方式时有效*/
    PROC_MEIDA_TRANS    CBonTransItlvMeidaDatas;
    
    /*最后接收的Req*/
    HI_S32 last_recv_req;

    /*最后发送的Req*/
    HI_S32 last_send_req;

    /*最后发送的Cseq*/
    HI_U32 last_send_seq;
    
    /*最后接收命令的Cseq*/
    HI_U32 last_recv_seq;

    HI_U32 pGetConnHandle;
    HI_U32 pPostConnHandle;

}RTSP_O_HTTP_LIVE_SESSION ;


/*get currect valid connect number*/
HI_S32 HI_RTSP_O_HTTP_SessionGetConnNum();

/*init session*/
HI_S32 HI_RTSP_O_HTTP_SessionListInit(HI_S32 s32TotalConnNum);

/*create session: get a unused node from the free session list*/
/*to do : 如何返回一个session 的指针*/
HI_S32 HI_RTSP_O_HTTP_SessionCreat(RTSP_O_HTTP_LIVE_SESSION **ppSession );


/*init a appointed session*/
HI_VOID HI_RTSP_O_HTTP_SessionInit(RTSP_O_HTTP_LIVE_SESSION *pSession, HI_SOCKET s32SockFd);

/*free the resource the session possess: close socket,
  and remove the session from busy list*/
HI_VOID HI_RTSP_O_HTTP_SessionClose(RTSP_O_HTTP_LIVE_SESSION *pSession);

/*all busy session destroy: */
HI_VOID HI_RTSP_O_HTTP_SessionAllDestroy();

/*copy the first msg,  which is recived by rtsp_o_http lisnsvr,
  to its corresponding session's recvbuff */
HI_S32 HI_RTSP_O_HTTP_SessionCopyMsg(RTSP_O_HTTP_LIVE_SESSION *pSession,HI_CHAR* pMsgBuffAddr,HI_U32 u32MsgLen);

HI_VOID HI_RTSP_O_HTTP_ClearSendBuff(RTSP_O_HTTP_LIVE_SESSION *pSession);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__HI_RTSP_O_HTTP_SESSION_H__*/

