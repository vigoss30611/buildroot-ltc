/****************************************************************************
*              Copyright 2007 - 2011, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_mtrans_internal.h
* Description: mtrans internal struct
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2008-02-18   w54723     NULL         Create this file.
*****************************************************************************/

#ifndef __HI_MTRANS_INTERNAL_H__
#define __HI_MTRANS_INTERNAL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_list.h"
//#include "hi_adpt_socket.h"
/************************************************************************************/
#define MTRANS_SEND  "MTRANS_SEND" 

#define MTRANS_CALLOC calloc
#define MTRANS_FREE free 

/*发送数据流类型*/
#define MTRANS_STREAM_VIDEO 0
#define MTRANS_STREAM_AUDIO 1
#define MTRANS_STREAM_DATA  2
#define MTRANS_STREAM_MAX   3

/*interleaved方式时,server端的媒体数据和流控数据的虚拟通道号*/
#define MTRANS_TCPITLV_SVR_MEDIADATA_CHNID 0
#define MTRANS_TCPITLV_SVR_ADAPTDATA_CHNID 1

#define MTRANS_PACKAGE_SIZE 512 /*FU-A打包的最大数据长度*/

/*tcp数据流的统计信息*/
typedef struct hiMTRANS_TCP_TRANS_STAT_S
{

}MTRANS_TCP_TRANS_STAT_S;

/*udp数据流的统计信息*/
typedef struct hiMTRANS_UDP_TRANS_STAT_S
{

}MTRANS_UDP_TRANS_STAT_S;


/*数据流通过普通tcp方式的传输参数*/
typedef struct hiMTRANS_TCPTRANS_INFO_S
{

                                
}MTRANS_TCPTRANS_INFO_S;

/*数据流通过udp传输参数*/
typedef struct hiMTRANS_UDPTRANS_INFO_S
{
    /*[0] 是视频端口， 1是音频端口*/
    HI_S32     s32SendThdId;    /*udp时发送线呈id,该线呈负责发送媒体流*/
    HI_IP_ADDR au8CliIP[MTRANS_STREAM_MAX];         /*客户端的接收ip地址*/
    HI_U32    cliMediaPort[MTRANS_STREAM_MAX];      /*客户端的媒体数据接收端口*/
    struct sockaddr_in struCliAddr[MTRANS_STREAM_MAX]; /*客户端的接收地址*/ 
    HI_U32    cliAdaptDataPort[MTRANS_STREAM_MAX];     /*客户端网络状况统计数据的接收端口*/
    //btlib
    int  as32SendSock[MTRANS_STREAM_MAX];    /*svr端数据发送socket*/
    int  as32RecvSock[MTRANS_STREAM_MAX];    /*svr端数据发送socket*/
    HI_U32    svrMediaSendPort[MTRANS_STREAM_MAX];  /*svr端发送端口*/ 
    HI_U32    svrAdaptDataSendPort[MTRANS_STREAM_MAX];  /*svr端发送端口*/ 
    HI_U32     au32SeqNum[MTRANS_STREAM_MAX];      /*svr端数据发送序号*/
    MTRANS_UDP_TRANS_STAT_S struUDPTransStat;          /*udp流统计信息*/
    /* 多播需要的信息 */
    HI_U8 InitFlag; /* 0x1视频已经初始化,0x2音频已经初始化,0x4 数据已经初始化*/
    HI_U8 ttl;
    HI_U8 chno;
    HI_U8 streamType;//0主码流,1子码流
    HI_U8 clientNum;
    HI_U8 threadstatus;//0数据处理线程没有在运行,1线程正在运行
    pthread_mutex_t multicastLock;  //保护多播信息
    pthread_cond_t multicastCond;
}MTRANS_UDPTRANS_INFO_S;

/*数据流通过tcp interleaved方式的传输参数*/
typedef struct hiMTRANS_TCPITLV_TRANS_INFO_S
{
    HI_U32                u32ASendSeqNum;       /*数据发送序号*/
    HI_U32                u32VSendSeqNum;
    //btlib
    int             s32TcpTransSock;     /*svr端tcp传输sock,嵌入式tcp流时等同于会话 sock*/
    
    HI_U32                 au32CliITLMediaDataChnId[MTRANS_STREAM_MAX];/*interleaved方式:cli->svr指示媒体流的chnId*/
    HI_U32                 au32SvrITLMediaDataChnId[MTRANS_STREAM_MAX];   /*interleaved方式:svr->cli指示媒体流的chnId*/
    MTRANS_TCP_TRANS_STAT_S  strTCPTransStat; /*tcp流统计信息*/
    
}MTRANS_TCPITLV_TRANS_INFO_S;

/*数据流通过广播方式的传输参数*/
typedef struct hiMTRANS_BRODCAST_TRANS_S
{
   
    
}MTRANS_BRODCAST_TRANS_S;


/*传输任务状态*/
typedef enum hiMTRANS_TASK_STATE_E
{
    TRANS_TASK_STATE_INIT = 0,          /*传输任务已初始化*/
    TRANS_TASK_STATE_READY,              /*传输任务建立*/
    TRANS_TASK_STATE_PLAYING,           /*传输任务正在运行*/
    TRANS_TASK_STATE_BUTT

}MTRANS_TASK_STATE_E;

typedef struct hiMTRANS_TASK_S
{
    /*用于维护链表*/
    List_Head_S        mtrans_sendtask_list;   /*媒体传输任务链表*/
    HI_IP_ADDR         au8CliIP;               /*字串表示客户端的ip地址*/
    HI_U32             u32MbuffHandle;         /*媒体资源句柄*/
    MT_VIDEO_FORMAT_E  enVideoFormat;           /*媒体资源中的视频格式*/
    MT_AUDIO_FORMAT_E  enAudioFormat;           /*媒体资源中的音频频格式*/
    MTRANS_MODE_E      enTransMode;                 /*媒体传输模式*/
    MTRANS_TCPTRANS_INFO_S struTcpTransInfo;  /*udp方式时的传输参数，当enTransMode=TRANS_TYPE_UDP有效*/
    MTRANS_UDPTRANS_INFO_S struUdpTransInfo;  /*tcp方式时的传输参数，当enTransMode=TRANS_TYPE_TDP有效*/
    MTRANS_TCPITLV_TRANS_INFO_S struTcpItlvTransInfo; /*tcp interleaved方式时的传输参数，当enTransMode=TRANS_MODE_TCP_ITLV有效*/
    MTRANS_BRODCAST_TRANS_S struBrodTransInfo; /*广播方式时的传输参数，当enTransMode=TRANS_TYPE_BROADCAST有效*/
    volatile MTRANS_TASK_STATE_E enTaskState[MTRANS_STREAM_MAX];        /*传输任务状态，0是视频传输状态，1是音频传输状态, 2是Data类数据传输状态 */
    PACK_TYPE_E         aenPackType[MTRANS_STREAM_MAX];      /*媒体数据的打包格式*/                                                            
	HI_U32        au32Ssrc[MTRANS_STREAM_MAX];      /*媒体数据的打包ssrc 标示*/  
	HI_U32        u32VideoPacketIndex;      //已发送的视频包总个数
	HI_U32        u32FrameIndex;           //已发送的视频帧总帧数
	HI_U32        u32FramePacketIndex;     //当前帧已发送包的个数
    HI_U32        u32PackDataLen;         //当前pu8PackBuff中已有的数据长度
	HI_CHAR*             pu8PackBuff;           /*FU-A打包的buff*/

    HI_U32     videoFrame;//帧率
    
    HI_U32        liveMode;//传输模式
    int SPS_LEN;
    int PPS_LEN; 
    int SEL_LEN;
	HI_U32		audioSampleRate; //采样率
}MTRANS_TASK_S;

typedef enum hiMTRANS_SVR_STATE
{
    MTRANSSVR_STATE_STOP = 0,         /*准备阶段， 还没运行*/        
    MTRANSSVR_STATE_Running = 1,      /*准备阶段， 还没运行*/
    MTRANSSVR_STATE_BUTT              /*准备阶段， 还没运行*/
    
}MTRANS_SVR_STATE_E;

typedef struct hiMTRANS_ATTR_S
{
    HI_U32  u32MaxTransTaskNum;          /*最大传输任务数*/
    HI_PORT u16MinSendPort;           /*udp时设备端最小发送端口*/
    HI_PORT u16MaxSendPort;           /*udp时设备端最大发送端口*/
    HI_U32 u32PackPayloadLen;           /*媒体流数据包payload长度*/
    HI_U32* pTransTaskListHead;         /*媒体传输任务链表的首地址*/
    MTRANS_SVR_STATE_E    enSvrState;   /*传输server state*/
    pthread_mutex_t  MutexGetPort; 
}MTRANS_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
