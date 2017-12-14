#ifndef _rtsp_server_protocol_h_
#define _rtsp_server_protocol_h_


#include "NW_Common.h"


//#include "hi_list.h"

#define VS_MAX_CAM_CNT 16 /*支持的最大摄像头数*/
#define VS_STREAM_PER_CAM 5 /*每个摄像头支持的最大码流数*/
#define VS_MAX_STREAM_CNT (VS_MAX_CAM_CNT * VS_STREAM_PER_CAM )


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


typedef enum hiLIVE_MODE_E
{
	LIVE_MODE_1CHN1USER = 0,/**单通道单用户模式*/
	LIVE_MODE_1CHNnUSER,/**单通道多用户模式*/
	LIVE_MODE_BUTT
}LIVE_MODE_E;

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


int rtsp_Server_Cfg_Init(NW_Server_Cfg *para);

extern int rtsp_ServiceDeinit();

#endif

