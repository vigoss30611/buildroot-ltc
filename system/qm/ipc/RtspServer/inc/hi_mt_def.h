/*
2008-11-10 10:00 v0.1
*/

//#include "hi_type.h"
//#include "hi_mbuf_def.h"
//#include "hi_vs_media_comm.h"
//#include "hi_buffer_abstract_define.h"
//#include "hi_msession_http.h"
#ifndef  __HI_MT_DEF_H__
#define  __HI_MT_DEF_H__
#ifdef __cplusplus
#if __cplusplus

extern "C"
{
#endif
#endif /* __cplusplus */


#define PROTONAME_MAX_LEN   32
/*VSCHN号的表示方式*/
/*xxy xx是摄像头号, 从1开始编号, y是码流号, 从1开始编号*/
#define VS_MAX_CAM_CNT 1 /*支持的最大摄像头数*/
#define VS_STREAM_PER_CAM 6 /*每个摄像头支持的最大码流数*/
#define VS_MAX_STREAM_CNT (VS_MAX_CAM_CNT * VS_STREAM_PER_CAM )


/*媒体流打包协议类型*/
typedef enum hiPACK_TYPE_E
{
    PACK_TYPE_RAW = 0, /*海思媒体包头类型MEDIA_HDR_S，头长8个字节，用于TCP传输*/
    PACK_TYPE_RTP,     /*普通RTP打包方式，头是12个字节*/
    PACK_TYPE_RTP_STAP,/* STAP-A打包方式，加了3个字节的净荷头，头是15个字节*/
    PACK_TYPE_RTP_FUA, /*FU-A打包方式,一个slice分为若干rtp包发送，数据前加了2type头*/
    PACK_TYPE_RTSP_ITLV, /*rtsp interleaved 打包方式,在rtp头上前加了4个净荷头*/
    PACK_TYPE_HISI_ITLV, /*hisi interleaved 打包方式,在rtp头上前加了8个净荷头*/
    PACK_TYPE_LANGTAO,
    PACK_TYPE_OWSP,
    PACK_TYPE_RTSP_O_HTTP,
    PACK_TYPE_BUTT     
} PACK_TYPE_E;

/*传输模式,目前仅支持MTRANS_MODE_UDP 和 MTRANS_MODE_TCP_ITLV方式:
 MTRANS_MODE_UDP: 普通udp传输模式:媒体数据和流控数据在两个端口上发送
 MTRANS_MODE_TCP_ITLV:interleaved方式的tcp传输模式:会话,媒体数据和流控数据在同一个链接上发送
*/
typedef enum hiMTRANS_MODE_E
{
    MTRANS_MODE_UDP = 0,
    MTRANS_MODE_UDP_ITLV,
    MTRANS_MODE_TCP,
    MTRANS_MODE_TCP_ITLV,
    MTRANS_MODE_BROADCAST,
    MTRANS_MODE_MULTICAST,
    MTRANS_MODE_BUTT
}MTRANS_MODE_E;

typedef enum hiLIVE_MODE_E
{
	LIVE_MODE_1CHN1USER = 0,/**单通道单用户模式*/
	LIVE_MODE_1CHNnUSER,/**单通道多用户模式*/
	LIVE_MODE_BUTT
}LIVE_MODE_E;

/*为了处理hi3510与hi3511兼容而增加的一个回调*/
typedef struct hiHTTP_LIVE_PACKPROTO_S
{
    PACK_TYPE_E enPackType;
    char aszPackProtoName[PROTONAME_MAX_LEN];  /*打包协议的名字*/
}HTTP_LIVE_PACKPROTO_S;

typedef int (*HI_HTTP_GetPackProtoInfo_PTR)(HTTP_LIVE_PACKPROTO_S *pstruProto);

typedef struct hiMT_CONFIG_RTSP_STREAMSVR_S
{
    int bEnable;
    int listen_port;
    int max_connections;
    /*udp的端口范围*/
    int udp_send_port_min;
    int udp_send_port_max;    
}MT_CONFIG_RTSP_STREAMSVR_S;

typedef struct hiMT_CONFIG_RTSPoHTTP_SVR_S
{
    int bEnable;
    //int listen_port;
    int max_connections;
}MT_CONFIG_RTSPoHTTP_SVR_S;

typedef struct hiMBUF_CHENNEL_INFO_S
{
    int  chnid; /* 011 */
    unsigned int max_connect_num;     //该通道支持的最大连接数
    unsigned int buf_size;            //该通道每块buf的大小(4bytes对齐)
    /*T41030 2010-02-11 Add Config of Buffer leftspace*/
    unsigned int buf_discard_threshold;
}MBUF_CHENNEL_INFO_S;

typedef struct hiMT_CONFIG_S
{
    MT_CONFIG_RTSP_STREAMSVR_S rtspsvr;
    MT_CONFIG_RTSPoHTTP_SVR_S rtspOhttpsvr;
    
    /*打包长度, 目前仅用于rtsp/rtp的udp发送中*/
    int              packet_len;
    
    LIVE_MODE_E         enLiveMode;
    int              maxUserNum;
    int 				maxFrameLen;    
    int              chn_cnt;
    MBUF_CHENNEL_INFO_S mbuf[VS_MAX_STREAM_CNT];

}MT_CONFIG_S;

typedef struct tagMT_MulticastInfo_s
{
    unsigned int channel;
    unsigned int streamType;
    char multicastAddr[32];
    unsigned short port[2];
}MT_MulticastInfo_s;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

