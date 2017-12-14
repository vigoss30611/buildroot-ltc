/****************************************************************************
*              Copyright 2006 - , Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_RTSP_session.h
* Description: The rtsp server.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-01-30   W54723     NULL         creat this file.
*****************************************************************************/
#ifndef  __HI_RTSP_SESSION_H__
#define  __HI_RTSP_SESSION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_list.h"
#include "hi_mtrans.h"

#define GT_PRINTF(fmt...) fprintf(stderr,##fmt)

#if 1
#define DEBUG_MSGRTSP(fmt...) do{ \
		GT_PRINTF("[FILE]:%s [Line]:%d:",__FILE__, __LINE__);\
		GT_PRINTF(fmt); \
}while(0)
#else
#define DEBUG_MSG(fmt...) {}
#endif



#define RTSP_SESS_MODEL  "RTSP_SESS"

#define RTSP_MAX_SESS_NUM   32  /*最大session数，根据编码路数及图像大小的不同，
                                  实际支持的最大session数往往小于该值 */
#define RTSP_MAX_SESSID_LEN 16  /*sessionid号的最大长度*/
#define RTSP_MAX_VERSION_LEN 128 /*版本号的最大长度*/

#define RTSP_TRACKID_VIDEO 0        /*Setup 消息中的track 信息*/
#define RTSP_TRACKID_AUDIO 1

typedef char  RTSP_URI[256];         /*URI数据类型 rtsp://xxx:333/222 */
typedef char  RTSP_SESSION_ID[RTSP_MAX_SESSID_LEN];

/*RTSP的socket收发buffer大小*/
#define RTSP_MAX_PROTOCOL_BUFFER 1024*2
/*RTSP请求buffer*/
typedef char RTSP_REQ_BUFFER[RTSP_MAX_PROTOCOL_BUFFER];
/*RTSP回复buffer*/
typedef RTSP_REQ_BUFFER RTSP_RESP_BUFFER;

/*RTSP会话的状态机*/
typedef enum hiRTSP_SESSION_STATE
{
    RTSP_SESSION_STATE_INIT    = 0, /*已被创建*/
    RTSP_SESSION_STATE_READY   = 1, 
    RTSP_SESSION_STATE_PLAY = 2, /*创建并运行*/
    RTSP_SESSION_STATE_STOP = 3,    /*已停止并销毁*/
    RTSP_SESSION_STATE_PAUSE   = 4, 
    RTSP_SESSION_STATE_BUTT
    
}RTSP_SESSION_STATE;

/*RTSP会话线呈状态机*/
typedef enum hiRTSP_LIVESESS_THREAD_STATE
{
    RTSPSESS_THREAD_STATE_INIT    = 0, /*已被创建*/
    RTSPSESS_THREAD_STATE_RUNNING = 1, /*创建并运行*/
    RTSPSESS_THREAD_STATE_STOP = 2, /*已停止并销毁*/
    RTSPSESS_THREAD_STATE_BUTT
}RTSPSESS_THREAD_STATE; 

typedef struct hiRTSP_LIVE_SESSION 
{           
    /*用于维护链表*/
    List_Head_S RTSP_sess_list;

    /*该Session所对应的监控通道号*/
    HI_S32         s32VsChn;
    HI_CHAR        auFileName[128];

    /*线呈id,该线呈负责rtsp交互及数据发送*/
    HI_S32          s32SessThdId;

    /*上述线呈的状态*/
    RTSPSESS_THREAD_STATE enSessThdState;
    
    /*该session所对应的session id, 由链接创建时候随机产生*/
    RTSP_SESSION_ID aszSessID;

    /*客户端的ip地址*/
    HI_IP_ADDR     au8CliIP;
    
    /*客户端的RTSP链接端口*/
    HI_PORT        s32CliRTSPPort;

    /*本地ip*/
    HI_IP_ADDR      au8HostIP; 

    /*该session对应的svr端会话socket，在链接创建时候被赋值*/
    HI_SOCKET       s32SessSock;

    /*该RTSP session的状态*/
    RTSP_SESSION_STATE enSessState;

    /*是否请求视频， 音频的标志*/
    HI_BOOL    bRequestStreamFlag[2]; //0:视频，1:音频

    /*该session中， 用户输入的url， rtsp://xxxxxx:553/1 */
    RTSP_URI         au8CliUrl[2];//0:视频，1:音频请求的setup的url

    HI_CHAR         au8UserAgent[RTSP_MAX_VERSION_LEN];

    HI_CHAR         au8SvrVersion[RTSP_MAX_VERSION_LEN];

    /*是否会话创建时，第一个消息已经接收，若已接收，则会话线呈先处理
      该消息再接收新消息，否则直接进入消息接收处理
      note:如果是webserver负责监听，则它会接收RTSP点播会话的第一个消息*/
    HI_BOOL         bIsFirstMsgHasInRecvBuff;
    /*rtsp接收命令的buffer*/
    RTSP_REQ_BUFFER  au8RecvBuff;
    
    /*rtsp发送命令的buffer*/
    RTSP_RESP_BUFFER au8SendBuff;

    /*发送命令buffer是否可发送标志*/
    HI_BOOL readyToSend;

    /*媒体数据传输是否在协商线程中发送，在协商传输方式
      时赋值 0-不是，1-是*/
    volatile HI_BOOL bIsTransData;

    /*传输任务handle*/
    HI_U32 u32TransHandle;

    /*媒体数据传输处理函数，interleaved方式和udp方式时有效*/
    PROC_MEIDA_TRANS    CBonTransMeidaDatas;
    
    /*最后接收的Req*/
    HI_S32 last_recv_req;

    /*最后发送的Req*/
    HI_S32 last_send_req;

    /*最后发送的Cseq*/
    HI_U32 last_send_seq;
    
    /*最后接收命令的Cseq*/
    HI_U32 last_recv_seq;

    HI_PORT remoteRTPPort[2]; //0:记录视频的端口,1:记录音频的端口
    
    HI_PORT remoteRTCPPort[2];
    HI_S32 heartTime;//心跳计数，当heartTime>=3时，如果还没有收到回复，则认为链接断开

    MTRANS_MODE_E enMediaTransMode; //rtp传输方式

    HI_BOOL bIsTurnRelay;   //是否通过turn服务器请求连接
    HI_IP_ADDR au8DestIP;   //实际的客户端地址

    /* 回放相关参数 */
    HI_BOOL bIsPlayback;        // 是否是回放
    HI_S32 s32WaitDoPause;
    HI_S32 last_send_frametype; // 1:I帧
    HI_S32 s32PlayBackType;     // 0:by name  1:by time
    HI_U32 start_offset;        //开始时间偏移,by time 时有效
    HI_CHAR RealFileName[64];   //对应的文件名,by time 时有效

	/*密码验证方式*/
	HI_S32 s32AuthenticateType;  //0 不验证  1 basic  2 digest
}RTSP_LIVE_SESSION ;


/*get currect valid connect number*/
HI_S32 HI_RTSP_SessionGetConnNum();

/*init session*/
HI_S32 HI_RTSP_SessionListInit(HI_S32 s32TotalConnNum);

/*create session: get a unused node from the free session list*/
/*to do : 如何返回一个session 的指针*/
HI_S32 HI_RTSP_SessionCreat(RTSP_LIVE_SESSION **ppSession );


/*init a appointed session*/
HI_VOID HI_RTSP_SessionInit(RTSP_LIVE_SESSION *pSession, HI_SOCKET s32SockFd);

/*free the resource the session possess: close socket,
  and remove the session from busy list*/
HI_VOID HI_RTSP_SessionClose(RTSP_LIVE_SESSION *pSession);

/*all busy session destroy: */
HI_VOID HI_RTSP_SessionAllDestroy();

/*copy the first msg,  which is recived by RTSP lisnsvr,
  to its corresponding session's recvbuff */
HI_S32 HI_RTSP_SessionCopyMsg(RTSP_LIVE_SESSION *pSession,HI_CHAR* pMsgBuffAddr,HI_U32 u32MsgLen);

HI_VOID HI_RTSP_ClearSendBuff(RTSP_LIVE_SESSION *pSession);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__HI_RTSP_SESSION_H__*/

