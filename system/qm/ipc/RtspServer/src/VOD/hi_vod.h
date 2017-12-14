/****************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_vod.h
* Description: 
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-01-31   w54723     NULL         Create this file.
*****************************************************************************/

#ifndef  __HI_VOD_H__
#define  __HI_VOD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "hi_mt_def.h"
#include "hi_mbuf_def.h"
#include "hi_mt_socket.h"
#include "hi_mtrans.h"
#include "hi_vs_media_comm.h"
//#include "hi_vs_enc.h"

/*vod模块各消息的msg type定义*/
#define MSG_VOD_DESCRIBE  0x00050000
#define MSG_VOD_SETUP     0x00050001
#define MSG_VOD_PLAY      0x00050002
#define MSG_VOD_PAUSE     0x00050003
#define MSG_VOD_TEARDOWN  0x00050004

#define VOD_MAX_STRING_LENGTH  32          //各字串最大长度
//#define VOD_MAX_VIDEO_DATAINFO_LEN  64      //sps,pps base64运算结果最大长度
//panhn   兼容TI项目
#define VOD_MAX_VIDEO_DATAINFO_LEN  256      //sps,pps base64运算结果最大长度
#define VOD_MAX_VERSION_LEN  64          //sps,pps base64运算结果最大长度

#define VOD_MAX_TRANSTYPE_NUM  4           //传输方式集最大个数

#define VOD_STREAM_VIDEO 0
#define VOD_STREAM_AUDIO 1
#define VOD_STREAM_DATA  2
#define VOD_STREAM_MAX   3

/*-------------客户端支持的各种传输模式的属性----------*/
typedef struct hiVOD_CLI_SUPPORT_UDP_S
{
    /*client端的数据收发端口*/
    /*1)UDP方式时为客户端等待接收媒体数据的端口号
      -1时表示无效*/
    HI_U32 u32CliMediaRecvPort;  

    /*client端的流控包接收端口*/
    /*1)UDP方式时为客户端等待接收流控包的端口号
      -1时表示无效*/
    HI_U32 u32CliAdaptRecvPort;

}VOD_CLI_SUPPORT_UDP_S;

/*暂不支持tcp传输模式，故暂不定义*/
typedef struct hiVOD_CLI_SUPPORT_TCP_S
{

    
}VOD_CLI_SUPPORT_TCP_S;

/*暂不支持udp interleaved传输模式，故暂不定义*/
typedef struct hiVOD_CLI_SUPPORT_UDP_ITLV_S
{

}VOD_CLI_SUPPORT_UDP_ITLV_S;

typedef struct hiVOD_CLI_SUPPORT_TCP_ITLV_S
{
    /*interleaved方式时数据传输socket(即会话socket)*/
    HI_SOCKET s32SessSock;

    /*interleaved方式时表示cli->svr的媒体数据的虚拟通道号
      -1时表示无效*/
    HI_U32 u32ItlvCliMediaChnId;  

    /*interleaved方式时表示cli->svr的流控包的虚拟通道号
      -1时表示无效*/
    HI_U32 u32ItlvCliAdaptChnId;

}VOD_CLI_SUPPORT_TCP_ITLV_S;

/*暂不支持广播传输模式，故暂不定义*/
typedef struct hiVOD_CLI_SUPPORT_BRODCAST_S
{
    

}VOD_CLI_SUPPORT_BRODCAST_S;


/*-------------设备端选中的cli端的传输模式及svr端针对该传输模式的属性----------*/
typedef struct hiVOD_SVR_SUPPORT_UDP_S
{

     /*svr端确认的client端的数据收发端口*/
    /*UDP方式时为客户端等待接收媒体数据的端口号
      -1时表示无效*/
     HI_U32 u32CliMediaRecvPort;   

    /*svr端确认的client端的流控包接收端口*/
    /*UDP方式时为客户端等待接收流控包的端口号
      -1时表示无效*/
    HI_U32 u32CliAdaptRecvPort;

     /*svr端媒体数据数据发送端口*/
    /*UDP方式时为svr端发送媒体数据的端口号
      -1时表示无效*/
    HI_U32 u32SvrMediaSendPort;  

    /*svr端流控包发送端口*/
    /*UDP方式时为svr端发送流控包的端口号
      -1时表示无效*/
    HI_U32 u32SvrAdaptSendPort;

    /*返回传输码流的函数指针，tcp潜入式流传输时在会话线呈中发送媒体码流*/
    PROC_MEIDA_TRANS pProcMediaTrans;

    /*返回相应的媒体传输任务句柄，tcp潜入式流传输时在会话线呈中发送媒体码流时
      指定相应的传输任务*/
    HI_U32 u32TransHandle;

}VOD_SVR_SUPPORT_UDP_S;

/*暂不支持tcp传输模式，故暂不定义*/
typedef struct hiVOD_SVR_SUPPORT_TCP_S
{

    
}VOD_SVR_SUPPORT_TCP_S;

/*暂不支持udp interleaved传输模式，故暂不定义*/
typedef struct hiVOD_SVR_SUPPORT_UDP_ITLV_S
{

}VOD_SVR_SUPPORT_UDP_ITLV_S;

typedef struct hiVOD_SVR_SUPPORT_TCP_ITLV_S
{
     /*svr端媒体数据数据发送端口*/
    /*interleaved方式时表示svr->cli的媒体数据的虚拟通道号
      -1时表示无效*/
    HI_U32 u32SvrMediaChnId;  

    /*svr端流控包发送端口*/
    /*interleaved方式时表示svr->cli的流控包的虚拟通道号
      -1时表示无效*/
    HI_U32 u32SvrAdaptChnId;

    /*返回传输码流的函数指针，tcp潜入式流传输时在会话线呈中发送媒体码流*/
    PROC_MEIDA_TRANS pProcMediaTrans;

    /*返回相应的媒体传输任务句柄，tcp潜入式流传输时在会话线呈中发送媒体码流时
      指定相应的传输任务*/
    HI_U32 u32TransHandle;
    
}VOD_SVR_SUPPORT_TCP_ITLV_S;

/*暂不支持广播传输模式，故暂不定义*/
typedef struct hiVOD_SVR_SUPPORT_BRODCAST_S
{
    

}VOD_SVR_SUPPORT_BRODCAST_S;


/*媒体会话协议类型*/
typedef enum hiVOD_SESS_PROTOCAL_TYPE_E
{
    SESS_PROTOCAL_TYPE_RTSP = 0,               /*rtsp会话协议*/
    SESS_PROTOCAL_TYPE_HTTP = 1,               /*http会话协议*/
    SESS_PROTOCAL_TYPE_SIP  = 2,               /*sip会话协议*/
    SESS_PROTOCAL_TYPE_OWSP = 3,
    SESS_PROTOCAL_TYPE_USER_DEFIN1 = 101,       /*用户自定义会话协议1*/
    SESS_PROTOCAL_TYPE_USER_DEFIN2               /*用户自定义会话协议2*/
}VOD_SESS_PROTOCAL_TYPE_E;
/*----------------------------------------------------*/
/*---------------------------------------------------------*/

typedef struct hiVOD_CLI_SUPPORT_TRANS_INFO_S{

    /*该传输参数结构是否有效 0-无效，1-有效*/
    HI_BOOL bValidFlag;

    /*对以上请求的媒体采用什么样的打包方式*/
    PACK_TYPE_E enPackType;
    
    /*对以上媒体采用何种传输模式:tcp udp or broadcast*/
    MTRANS_MODE_E enMediaTransMode;

    /*udp传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP时有效*/
    VOD_CLI_SUPPORT_UDP_S stUdpTransInfo;

    /*tcp传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP时有效*/
    VOD_CLI_SUPPORT_TCP_S stTcpTransInfo;

    /*udp interleaved传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP_ITLV时有效*/
    VOD_CLI_SUPPORT_UDP_ITLV_S stUdpItlvTransInfo;

    /*tcp interleaved传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP_ITLV时有效*/
    VOD_CLI_SUPPORT_TCP_ITLV_S stTcpItlvTransInfo;

    /*广播传输模式时，传输参数信息
      当enMediaTransMode = MTRANS_MODE_BROADCAST时有效*/
    VOD_CLI_SUPPORT_BRODCAST_S stBroadcastTransInfo;
 
}VOD_CLI_SUPPORT_TRANS_INFO_S;

typedef struct hiVOD_SVR_SUPPORT_TRANS_INFO_S{
     
    /*svr端选中的媒体传输模式*/
    MTRANS_MODE_E enMediaTransMode;

    /*udp传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP时有效*/
    VOD_SVR_SUPPORT_UDP_S stUdpTransInfo;

    /*tcp传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP时有效*/
    VOD_SVR_SUPPORT_TCP_S stTcpTransInfo;

    /*udp interleaved传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_UDP_ITLV时有效*/
    VOD_SVR_SUPPORT_UDP_ITLV_S stUdpItlvTransInfo;

    /*tcp interleaved传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_TCP_ITLV时有效*/
    VOD_SVR_SUPPORT_TCP_ITLV_S stTcpItlvTransInfo;

    /*广播传输模式时，svr端传输参数信息
      当enMediaTransMode = MTRANS_MODE_BROADCAST时有效*/
    VOD_SVR_SUPPORT_BRODCAST_S stBroadcastTransInfo;

}VOD_SVR_SUPPORT_TRANS_INFO_S;

 typedef struct hiVOD_DESCRIBE_MSG_S{
    /*媒体资源,对于直播为通道号，对于点播为媒体路径*/
    //HI_CHAR au8MediaFile[VOD_MAX_STRING_LENGTH];           
    HI_BOOL bLive;                  //是否实时码流
    HI_S32 s32PlayBackType;    //bLive=0时有效
    HI_CHAR au8MediaFile[128]; 
    HI_CHAR au8UserName[VOD_MAX_STRING_LENGTH];            //用户名
    HI_CHAR au8PassWord[VOD_MAX_STRING_LENGTH];            //密码
    HI_CHAR au8ClientVersion[VOD_MAX_VERSION_LEN];      //客户端版本信息  
}VOD_DESCRIBE_MSG_S;

typedef struct hiVOD_ReDESCRIBE_MSG_S{
    HI_S32 s32ErrCode; //0 成功，>0 对应错误码
    HI_U32  u32MediaDurTime;          //媒体时长，单位秒。对于直播通道时长为0
    MT_AUDIO_FORMAT_E enAudioType;    //音频格式
    HI_U32  u32AudioSampleRate;       //音频采样率，单位赫兹
    HI_U32  u32AudioChannelNum;       // 1单声道 2双声道
    MT_VIDEO_FORMAT_E enVideoType;    //视频格式
    HI_U32  u32VideoPicWidth;              //视频宽
    HI_U32  u32VideoPicHeight;             //视频高
    HI_U32  u32VideoSampleRate;       //视频采用率
    HI_U32  u32Framerate;
    HI_U32  u32Bitrate;
    unsigned long u32TotalTime;            //回放文件长度
    HI_CHAR au8VideoDataInfo[VOD_MAX_VIDEO_DATAINFO_LEN]; /*视频数据信息,sps,pps做base64结果字符串*/
    int SPS_LEN;
    int PPS_LEN;
    int SEL_LEN;
    HI_U32  u32DataInfoLen;
    HI_CHAR au8BoardVersion[VOD_MAX_VERSION_LEN];      //设备版本信息 
    unsigned char profile_level_id[8];
    char RealFileName[64];//回放by time 方式时有效
    HI_U32 start_offset;
}VOD_ReDESCRIBE_MSG_S;

typedef struct hiVOD_SETUP_MSG_S{
    VOD_SESS_PROTOCAL_TYPE_E enSessType;  //会话协议类型
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH];  //会话id，不同会话协议维护各自会话id
    HI_IP_ADDR au8ClientIp; //客户端地址
    /*媒体资源,对于直播为通道号，对于点播为媒体路径*/
    //HI_CHAR au8MediaFile[VOD_MAX_STRING_LENGTH];
    HI_CHAR au8MediaFile[128];
    /*请求建立的媒体类型*/
    /*0x01表示仅请求视频流,Ox02表示仅请求音频流 0x04表示仅建立data流*/
    HI_U8   u8SetupMeidaType;
    HI_U8   resv1;
    HI_U16  resv2;
    /*请求的媒体类型对应的同源标识
	注:若MeidaType为多个媒体类型，表示他们的ssrc相同*/
	HI_U32 u32Ssrc;	
    VOD_CLI_SUPPORT_TRANS_INFO_S astCliSupptTransList[VOD_MAX_TRANSTYPE_NUM];   //客户端支持的传输方式
    HI_CHAR au8UserName[VOD_MAX_STRING_LENGTH];            //用户名
    HI_CHAR au8PassWord[VOD_MAX_STRING_LENGTH];            //密码
    HI_CHAR au8ClientVersion[VOD_MAX_VERSION_LEN];      //客户端版本信息
    HI_BOOL bIsPlayBack;
    HI_S32 s32PlayBackType;
}VOD_SETUP_MSG_S;

typedef struct hiVOD_ReSETUP_MSG_S{
    HI_S32 s32ErrCode; //0 成功，>0 对应错误码    
    /*已接受建立请求的的媒体类型*/
    /*0x01表示仅请求视频流,Ox02表示仅请求音频流 0x04表示仅建立data流*/
    HI_U8   u8SetupMeidaType;
    HI_U8   resv1;
    HI_U16  resv2;

    /*对以上请求的媒体采用什么样的打包方式*/
    PACK_TYPE_E enPackType;
    
    VOD_SVR_SUPPORT_TRANS_INFO_S stTransInfo;  //板端选定的客户端传输方式及svr端传输信息
    
    HI_CHAR au8BoardVersion[VOD_MAX_VERSION_LEN];      //设备版本信息 
}VOD_ReSETUP_MSG_S;

typedef struct hiVOD_PLAY_MSG_S{
    VOD_SESS_PROTOCAL_TYPE_E enSessType;  //会话协议类型
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH];  //会话id，不同会话协议维护各自会话id
    HI_U32  u32StartTime;  //播放起始时间(相对于媒体文件的起始偏移)，单位毫秒,直播时此字段无效
    HI_U32  u32Duration;      //播放时长，单位毫秒,直播时此字段无效
    /*请求播放媒体类型*/
    /*0x01表示仅请求视频流,Ox02表示仅请求音频流 0x04表示仅建立data流*/
    HI_U8   u8PlayMediaType;  
    HI_U8   resv1;
    HI_U16  resv2;
}VOD_PLAY_MSG_S;

typedef struct hiVOD_RePLAY_MSG_S{
    HI_U64 video_pts;
	HI_U64 audio_pts;
    HI_S32 s32ErrCode; //0 成功，>0 对应错误码
}VOD_RePLAY_MSG_S;

typedef struct hiVOD_PAUSE_MSG_S{
    VOD_SESS_PROTOCAL_TYPE_E enSessType;  //会话协议类型
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH];  //会话id，不同会话协议维护各自会话id
    HI_U32 u32PauseTime;  //播放暂停时间(相对于媒体文件的起始偏移)，单位毫秒,直播时此字段无效
    /*请求播放媒体类型*/
    /*0x01表示仅请求暂停视频流,Ox02表示仅请求暂停音频流 0x04表示仅暂停data流*/
    HI_U8 u8PauseMediasType;  //请求暂停媒体类型
    HI_U8   resv1;
    HI_U16  resv2;
    
}VOD_PAUSE_MSG_S;

typedef struct hiVOD_RePAUSE_MSG_S{
    HI_S32 s32ErrCode; //0 成功，>0 对应错误码
}VOD_RePAUSE_MSG_S;

typedef struct tagVOD_RESUME_MSG_S{
    VOD_SESS_PROTOCAL_TYPE_E enSessType;  //会话协议类型
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH];  //会话id，不同会话协议维护各自会话id
    HI_U8 u8MediasType;
    HI_U8   resv1;
    HI_U16  resv2;
}VOD_RESUME_MSG_S;

typedef struct hiVOD_RESUME_RespMSG_S{
    HI_S32 s32ErrCode; //0 成功，>0 对应错误码
}VOD_RESUME_RespMSG_S;


typedef struct hiVOD_TEAWDOWN_MSG_S{
    VOD_SESS_PROTOCAL_TYPE_E enSessType;  //会话协议类型
    MTRANS_MODE_E      enTransMode;                 /*媒体传输模式*/
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH];  //会话id，不同会话协议维护各自会话id        
    HI_U32 u32Reason;  //关闭连接的原因
}VOD_TEAWDOWN_MSG_S;

typedef struct hiVOD_ReTEAWDOWN_MSG_S{
    HI_S32 s32ErrCode; //0 成功，>0 对应错误码
}VOD_ReTEAWDOWN_MSG_S;

#define VOD_DEFAULT_MAX_CHNNUM 16

typedef struct hiVOD_GET_BASEPTS_MSG_S{
    VOD_SESS_PROTOCAL_TYPE_E enSessType;  //会话协议类型
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH];  //会话id，不同会话协议维护各自会话id        
}VOD_GET_BASEPTS_MSG_S;

typedef struct hiVOD_ReGET_BASEPTS_MSG_S{
    HI_S32 s32ErrCode; //0 成功，>0 对应错误码
    HI_U64 u64BaseTimeStamp; //基准时间戳
}VOD_ReGET_BASEPTS_MSG_S;

typedef struct hiVOD_CONFIG_S
{
    /*mtrans相关属性:udp的端口范围*/
    HI_U16              u16UdpSendPortMin;
    HI_U16              u16UdpSendPortMax;
    /*媒体流数据包payload长度*/
    HI_U32              u32PackPayloadLen;           

    /*mbuf相关属性:udp的端口范围*/
    //#define MT_MAX_CHN_CNT 16
    int enLiveMode;/**点播模式*/
    HI_S32              maxUserNum;//最大支持的用户数
    HI_S32              maxFrameLen;//最大帧长度；
    HI_S32              s32SupportChnNum;
    MBUF_CHENNEL_INFO_S stMbufConfig[VS_MAX_STREAM_CNT];
}VOD_CONFIG_S;


/*获取首个发送数据的时间戳，并发送首个数据包该接口会阻塞,直到成功后期基准时戳或发生错误*/
HI_U32 HI_VOD_GetBaseTimeStamp(VOD_GET_BASEPTS_MSG_S *stVodGetPtsReqInfo,
                                            VOD_ReGET_BASEPTS_MSG_S 
                                            *stVodGetPtsRespInfo);

HI_S32 HI_VOD_Init(VOD_CONFIG_S* pstVodConfig);

HI_S32 HI_VOD_DeInit();

HI_S32 HI_VOD_DESCRIBE(VOD_DESCRIBE_MSG_S *stDesReqInfo,VOD_ReDESCRIBE_MSG_S *stDesRespInfo);

HI_S32 HI_VOD_SetUp(VOD_SETUP_MSG_S *stVodSetupReqInfo, VOD_ReSETUP_MSG_S *stVodSetupRespInfo);

//HI_S32 HI_VOD_SetUp_Add(VOD_SETUP_MSG_S *stVodSetupReqInfo, VOD_ReSETUP_MSG_S *stVodSetupRespInfo);

HI_S32 HI_VOD_Play(VOD_PLAY_MSG_S *stVodPlayReqInfo, VOD_RePLAY_MSG_S *stVodPlayRespInfo);

HI_S32 HI_VOD_Pause(VOD_PAUSE_MSG_S*stVodPauseReqInfo, VOD_RePAUSE_MSG_S *stVodPauseRespInfo);

HI_S32 HI_VOD_Resume(VOD_RESUME_MSG_S*stVodResumeReqInfo, VOD_RESUME_RespMSG_S *stVodResumeRespInfo);


HI_S32 HI_VOD_Teardown(VOD_TEAWDOWN_MSG_S *stVodCloseReqInfo, VOD_ReTEAWDOWN_MSG_S *stVodCloseRespInfo);
#if 0
HI_S32 HI_VOD_Run(HI_S32 s32Chn, ENC_STREAM_S* pAvenc_Stream);

HI_S32 HI_VOD_DATA(HI_S32 s32Chn,HI_CHAR * pszAlarmInfo, HI_S32 s32Len);
#endif
HI_S32 HI_VOD_GetVersion(HI_CHAR *pszSvrVersion, HI_U32 u32VersionLen);
/*debug 调试打印接口*/
HI_VOID DEBUGPrtVODDes(VOD_DESCRIBE_MSG_S *stDesReqInfo,VOD_ReDESCRIBE_MSG_S *stDesRespInfo);

HI_VOID DEBUGPrtVODSetup(VOD_SETUP_MSG_S *stVodSetupReqInfo, VOD_ReSETUP_MSG_S *stVodSetupRespInfo);

HI_VOID DEBUGPrtVODPlay(VOD_PLAY_MSG_S * stVodPlayReqInfo, VOD_RePLAY_MSG_S * stVodPlayRespInfo);

HI_VOID DEBUGPrtVODPause(VOD_PAUSE_MSG_S*stVodPauseReqInfo, VOD_RePAUSE_MSG_S *stVodPauseRespInfo);

HI_VOID DEBUGPrtVODTeawdown(VOD_TEAWDOWN_MSG_S *stVodCloseReqInfo, VOD_ReTEAWDOWN_MSG_S *stVodCloseRespInfo);

HI_BOOL HI_VOD_IsRunning();



typedef void (*NW_LogoffFunc)(int id);

typedef struct NW_ClientInfotag
{
	int 				DmsHandle;
	int 				ClientSocket;
	struct sockaddr_in	ClientAddr;
	int					id;
	NW_LogoffFunc 		OnLogoff; //在客户端退出时调用这个函数
}NW_ClientInfo;


#ifndef DEBUG_VOD
#define DEBUG_VOD 8
#if (DEBUG_VOD == 0) || !defined(DEBUG_ON)
    #define DEBUG_VOD_PRINT(pszModeName, u32Level, args ...)
#else
#define DEBUG_VOD_PRINT(pszModeName, u32Level, args ...)   \
    do  \
    {   \
        if (DEBUG_VOD >= u32Level)    \
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
#endif /* __cplusplus */

#endif /*__HI_VOD_H__*/

