/***********************************************************************************
*              Copyright 2006 - , Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_mtrans.h
* Description:
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-02-13   w54723     NULL         Create this file.
***********************************************************************************/

#ifndef _HI_MTRANS_H_
#define _HI_MTRANS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "hi_type.h"
#include "hi_mt_def.h"
//#include "hi_adpt_socket.h"
#include "hi_mt_socket.h"
#include "hi_vs_media_comm.h"

typedef int (*PROC_MEIDA_TRANS)(HI_U32,HI_U32);        /*interleaved时发送媒体流的处理函数的指针*/


/*-------------客户端支持的各种传输模式的属性----------*/
typedef struct hiMTRANS_CLI_SUPPORT_UDP_S
{
    /*client端的数据收发端口*/
    /*1)UDP方式时为客户端等待接收媒体数据的端口号
      -1时表示无效*/
    HI_U32 u32CliMDataPort;
    HI_U32 u32CliAdaptDataPort;

}MTRANS_CLI_SUPPORT_UDP_S;

/*暂不支持tcp传输模式，故暂不定义*/
typedef struct hiMTRANS_CLI_SUPPORT_TCP_S
{

    
}MTRANS_CLI_SUPPORT_TCP_S;

/*暂不支持udp interleaved传输模式，故暂不定义*/
typedef struct hiMTRANS_CLI_SUPPORT_UDP_ITLV_S
{

}MTRANS_CLI_SUPPORT_UDP_ITLV_S;

typedef struct hiMTRANS_CLI_SUPPORT_TCP_ITLV_S
{
    /*interleaved方式时数据传输socket(即会话socket)*/
    //btlib
    int s32SessSock;

    /*interleaved方式时表示cli->svr的媒体数据的虚拟通道号
      -1时表示无效*/
    HI_U32 u32ItlvCliMDataId;  

}MTRANS_CLI_SUPPORT_TCP_ITLV_S;

/*暂不支持广播传输模式，故暂不定义*/
typedef struct hiMTRANS_CLI_SUPPORT_BRODCAST_S
{
    

}MTRANS_CLI_SUPPORT_BRODCAST_S;


/*-------------设备端选中的cli端的传输模式及svr端针对该传输模式的属性----------*/
typedef struct hiMTRANS_SVR_SUPPORT_UDP_S
{

     /*svr端媒体数据数据发送端口*/
    /*UDP方式时为svr端发送媒体数据的端口号
      -1时表示无效*/
    HI_U32 u32SvrMediaSendPort;

    /*返回传输码流的函数指针，目前udp传输时在会话线呈中发送媒体码流*/
    PROC_MEIDA_TRANS pProcMediaTrans;

}MTRANS_SVR_SUPPORT_UDP_S;

/*暂不支持tcp传输模式，故暂不定义*/
typedef struct hiMTRANS_SVR_SUPPORT_TCP_S
{

    
}MTRANS_SVR_SUPPORT_TCP_S;

/*暂不支持udp interleaved传输模式，故暂不定义*/
typedef struct hiMTRANS_SVR_SUPPORT_UDP_ITLV_S
{

}MTRANS_SVR_SUPPORT_UDP_ITLV_S;

typedef struct hiMTRANS_SVR_SUPPORT_TCP_ITLV_S
{
     /*svr端媒体数据数据发送端口*/
    /*interleaved方式时表示svr->cli的媒体数据的虚拟通道号
      -1时表示无效*/
    HI_U32 u32SvrMediaDataId;  

    /*返回传输码流的函数指针，tcp潜入式流传输时在会话线呈中发送媒体码流*/
    PROC_MEIDA_TRANS pProcMediaTrans;
    
}MTRANS_SVR_SUPPORT_TCP_ITLV_S;

/*暂不支持广播传输模式，故暂不定义*/
typedef struct hiMTRANS_SVR_SUPPORT_BRODCAST_S
{
    

}MTRANS_SVR_SUPPORT_BRODCAST_S;

/* 返回多播传输模式*/
typedef struct hiMTRANS_SVR_SUPPORT_MULTICAST_S
{
    /* 如果多播已经建立，则返回服务端和客户端的端口号*/
    HI_U16    cliMediaPort;
    HI_U16    svrMediaPort;
    HI_U32 u32MediaHandle;    /*媒体资源句柄*/
}MTRANS_SVR_SUPPORT_MULTICAST_S;

/*----------------------------------------------------*/
typedef struct hiMTRANS_SEND_CREATE_MSG_S{
    HI_IP_ADDR   au8CliIP;    /*字串表示客户端的ip地址*/

    HI_U32 u32MediaHandle;    /*媒体资源句柄*/

    HI_U32 enLiveMode;//传输模式
    /*请求建立的媒体类型*/
    /*0x01表示仅请求视频流,Ox02表示仅请求音频流 0x04表示仅建立data流*/
    HI_U8   u8MeidaType;

    /*媒体资源,对于直播为通道号，对于点播为媒体路径*/
    HI_CHAR au8MediaFile[32];

    HI_U8   resv1;/**暂时用来存放视频帧率*/
    HI_U8  SPS_LEN;
    HI_U8  PPS_LEN;
    HI_U8  SEL_LEN;

    /*对以上请求的媒体采用什么样的打包方式*/
    PACK_TYPE_E enPackType;

    MT_VIDEO_FORMAT_E  enVideoFormat;           /*媒体资源中的视频格式*/
    MT_AUDIO_FORMAT_E  enAudioFormat;           /*媒体资源中的音频频格式*/

	/*请求的媒体类型对应的同源标识
	注:若MeidaType为多个媒体类型，表示他们的ssrc相同*/
	HI_U32 u32Ssrc;							    
	
    /*媒体传输方式:tcp udp or broadcast*/
    MTRANS_MODE_E enMediaTransMode;
    
    /*udp传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP时有效*/
    MTRANS_CLI_SUPPORT_UDP_S stUdpTransInfo;

    /*tcp传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP时有效*/
    MTRANS_CLI_SUPPORT_TCP_S stTcpTransInfo;

    /*udp interleaved传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP_ITLV时有效*/
    MTRANS_CLI_SUPPORT_UDP_ITLV_S stUdpItlvTransInfo;

    /*tcp interleaved传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP_ITLV时有效*/
    MTRANS_CLI_SUPPORT_TCP_ITLV_S stTcpItlvTransInfo;

    /*广播传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_BROADCAST时有效*/
    MTRANS_CLI_SUPPORT_BRODCAST_S stBroadcastTransInfo;

	HI_U32  AudioSampleRate;
    
}MTRANS_SEND_CREATE_MSG_S;

typedef struct hiMTRANS_ReSEND_CREATE_MSG_S{
    HI_S32 s32ErrCode; /*0 成功，>0 对应错误码*/

    HI_U32 u32TransTaskHandle;    /*返回的传输任务句柄，0表示创建失败*/

    /*媒体传输方式:tcp udp or broadcast*/
    MTRANS_MODE_E enMediaTransMode;

    /*udp传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP时有效*/
    MTRANS_SVR_SUPPORT_UDP_S stUdpTransInfo;

    /*tcp传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP时有效*/
    MTRANS_SVR_SUPPORT_TCP_S stTcpTransInfo;

    /*udp interleaved传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP_ITLV时有效*/
    MTRANS_SVR_SUPPORT_UDP_ITLV_S stUdpItlvTransInfo;

    /*tcp interleaved传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP_ITLV时有效*/
    MTRANS_SVR_SUPPORT_TCP_ITLV_S stTcpItlvTransInfo;

    /*广播传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_BROADCAST时有效*/
    MTRANS_SVR_SUPPORT_BRODCAST_S stBroadcastTransInfo;

    /*多播传输模式时，返回已建立多播传输参数信息
      当enMediaTransMode = MTRANS_MODE_MULTICAST时有效*/
    MTRANS_SVR_SUPPORT_MULTICAST_S stMulticastTransInfo;
}MTRANS_ReSEND_CREATE_MSG_S;

typedef struct hiMTRANS_START_MSG_S{
    HI_U32 u32TransTaskHandle;    /*发送或接收任务的句柄*/
    /*请求开始传输的媒体类型*/
    /*0x01表示仅传输视频流,Ox02表示仅传输音频流 0x04表示仅传输data流*/
    HI_U8   u8MeidaType;

    HI_U8   rsv1;
    HI_U16  rsv2;
}MTRANS_START_MSG_S;

typedef struct hiMTRANS_ReSTART_MSG_S{
    HI_S32 s32ErrCode; /*0 成功，>0 对应错误码*/
}MTRANS_ReSTART_MSG_S;

typedef struct hiMTRANS_RESUME_MSG_S{
    HI_U32 u32TransTaskHandle;    /*发送或接收任务的句柄*/
    /*请求开始传输的媒体类型*/
    /*0x01表示仅传输视频流,Ox02表示仅传输音频流 0x04表示仅传输data流*/
    HI_U8   u8MeidaType;

    HI_U8   rsv1;
    HI_U16  rsv2;
}MTRANS_RESUME_MSG_S;

typedef struct hiMTRANS_RESUME_RespMSG_S{
    HI_S32 s32ErrCode; /*0 成功，>0 对应错误码*/
}MTRANS_RESUME_RespMSG_S;


typedef struct hiMTRANS_STOP_MSG_S{
    HI_U32 u32TransTaskHandle;    /*发送或接收任务的句柄*/
    /*请求停止传输的媒体类型*/
    /*0x01表示仅停止传输视频流,Ox02表示仅停止传输音频流 0x04表示仅停止传输data流*/
    HI_U8   u8MeidaType;
    HI_U8   rsv1;
    HI_U16  rsv2;
}MTRANS_STOP_MSG_S;

typedef struct hiMTRANS_ReSTOP_MSG_S{
    HI_S32 s32ErrCode; /*0 成功，>0 对应错误码*/
}MTRANS_ReSTOP_MSG_S;


typedef struct hiMTRANS_DESTROY_MSG_S{
    HI_U32 u32TransTaskHandle;    /*发送任务的句柄*/
}MTRANS_DESTROY_MSG_S;

typedef struct hiMTRANS_ReDESTROY_MSG_S{
    HI_S32 s32ErrCode; /*0 成功，>0 对应错误码*/
}MTRANS_ReDESTROY_MSG_S;

typedef struct hiMTRANS_INIT_CONFIG_S
{
    HI_U32 u32MaxTransTaskNum;          /*最大传输任务数*/
    HI_U32 u32PackPayloadLen;           /*媒体流数据包payload长度*/
    HI_PORT u16MinSendPort;           /*udp时设备端最小发送端口*/
    HI_PORT u16MaxSendPort;           /*udp时设备端最大发送端口*/

}MTRANS_INIT_CONFIG_S;

#define RTSP_SEEK_FLAG   9

struct MTRANS_PlayBackInfo{
    int cmd;
    int videoframe;
    unsigned long tick;
    unsigned long VideoTick;
    unsigned long datalen;
    unsigned long timestamp;
    void *pData;
    
};

typedef struct tgSeekResp{
    unsigned long vseq;
    unsigned long aseq;
    unsigned long vtime;
    unsigned long atime;
}SeekResp_S;

#define MTRANS_SEND_IFRAME 'i'


HI_S32 HI_MTRANS_Init(MTRANS_INIT_CONFIG_S* pstruInitCfg);

HI_S32 HI_MTRANS_DeInit();

HI_S32 HI_MTRANS_CreatSendTask(HI_U32* mtransHandle,MTRANS_SEND_CREATE_MSG_S *pstSendInfo,MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo);

HI_S32 HI_MTRANS_Add2CreatedTask(HI_U32 u32TransTaskHandle,
                                        MTRANS_SEND_CREATE_MSG_S *pstSendInfo,MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo);

HI_S32 HI_MTRANS_DestroySendTask(MTRANS_DESTROY_MSG_S *pstDestroyInfo,MTRANS_ReDESTROY_MSG_S *pstReDestroyInfo);

HI_S32 HI_MTRANS_StartSendTask(MTRANS_START_MSG_S *pstStartSendInfo, MTRANS_ReSTART_MSG_S *pstReStartInfo);

HI_S32 HI_MTRANS_StopSendTask(MTRANS_STOP_MSG_S *pstStopSendInfo,MTRANS_ReSTOP_MSG_S *pstReStopInfo);

HI_S32 OnProcItlvTrans(HI_U32 pTask,HI_U32 privData);

/*以下为调试打印接口*/
HI_VOID DEBUGPrtMtransCreat(MTRANS_SEND_CREATE_MSG_S *pstSendInfo,MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo);

HI_VOID DEBUGPrtMtransStart(MTRANS_START_MSG_S *pstStartSendInfo, MTRANS_ReSTART_MSG_S *pstReStartInfo);

HI_VOID DEBUGPrtMtransStop(MTRANS_STOP_MSG_S *pstStopSendInfo,MTRANS_ReSTOP_MSG_S *pstReStopInfo);


HI_VOID DEBUGPrtMtransDestroy(MTRANS_DESTROY_MSG_S *pstDestroyInfo,MTRANS_ReDESTROY_MSG_S *pstReDestroyInfo);

/*获取首个发送数据的时间戳，并发送首个数据包*/
HI_U32 HI_MTRANS_GetBaseTimeStamp(HI_U32 u32TransTaskHandle,HI_U64 *u64BasePts);

HI_S32 HI_MTRANS_SetMbufHandle(HI_U32 mtransHandle, HI_U32 pMbufHandle);

HI_S32 MTRANS_GetTaskStatusMulticast(HI_U32 u32TransTaskHandle);

HI_S32 MTRANS_StartMulticastSendTask(HI_U32 u32TransTaskHandle);

int MTRANS_StartMulticastStreaming(MT_MulticastInfo_s *pInfo);

int MTRANS_StopMulticastStreaming(MT_MulticastInfo_s *pInfo);

HI_S32 HI_MTRANS_ResumeSendTask(MTRANS_RESUME_MSG_S *pstResumeInfo, MTRANS_RESUME_RespMSG_S *pstResumeResp);

int MTRANS_Register_StunServer(char *pServerAddr, unsigned short port);

int MTRANS_Deregister_StunServer();
int MTRANS_Get_NatAddr(unsigned int TaskHandle,unsigned char stream, unsigned short MediaPort, unsigned short AdaptPort, char *pNatAddr, unsigned short *pMapedPortPair);
int MTRANS_Check_Redirect(unsigned int TaskHandle, int IsRequestVideo, int IsRequestAudio);


#ifndef DEBUG_MTRANS 

    #define DEBUG_MTRANS 8
    #if (DEBUG_MTRANS == 0) || !defined(DEBUG_ON)
        #define DEBUG_MTRANS_PRINT(pszModeName, u32Level, args ...)
    #else
    #define DEBUG_MTRANS_PRINT(pszModeName, u32Level, args ...)   \
        do  \
        {   \
            if (DEBUG_MTRANS >= u32Level)    \
            {   \
                printf(args);   \
            }   \
        }   \
        while(0)
    #endif
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

