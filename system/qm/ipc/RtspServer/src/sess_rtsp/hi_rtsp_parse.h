/****************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_RTSP_parse.h
* Description: RTSP common info and  error code list
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-01-31   w54723     NULL         Create this file.
*****************************************************************************/

#ifndef  __HI_RTSP_PARSE_H__
#define  __HI_RTSP_PARSE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_vs_media_comm.h"

#define RTSP_VER_STR  "RTSP/1.0"

#define RTSP_LR   "\r"
#define RTSP_LF   "\n"
#define RTSP_LRLF "\r\n"

typedef char RTSP_URL[256];

#define RTSP_METHOD_PLAY       "play"
#define RTSP_METHOD_PAUSE      "pause"
#define RTSP_METHOD_REPLAY     "replay"
#define RTSP_METHOD_STOP       "stop"

/* message header keywords */
#define RTSP_HDR_LIVE          "livestream"     /*RTSP url中指定为点播请求的标志字段*/
#define RTSP_HDR_CONTENTLENGTH "Content-Length"
#define RTSP_HDR_ACCEPT "Accept"
#define RTSP_HDR_ALLOW "Allow"
#define RTSP_HDR_CONTENTTYPE "Content-Type"
#define RTSP_HDR_DATE "Date"
#define RTSP_HDR_CSEQ "CSeq"
#define RTSP_HDR_SESSION "Session"
#define RTSP_HDR_TRANSPORT "Transport"
#define RTSP_HDR_RANGE "Range"	
#define RTSP_HDR_USER_AGENT "User-Agent"
#define RTSP_HDR_AUTHORIZATION "Authorization"




//TODO 
typedef enum hiRTSP_REQ_METHOD
{
    /* method codes */
    RTSP_OPTIONS_METHOD     = 0 ,
    RTSP_DISCRIBLE_METHOD     = 1 ,
    RTSP_SETUP_METHOD   = 2,
    RTSP_PLAY_METHOD   = 3 ,
    RTSP_PAUSE_METHOD  = 4 ,
    RTSP_TEARDOWN_METHOD  = 5 ,
    RTSP_GET_PARAM_METHOD = 6 ,
    RTSP_SET_PARAM_METHOD = 7,
    RTSP_REQUEST_I_FRAME = 8,
    RTSP_HEARTBEAT_METHOD = 9,
    RTSP_REQ_METHOD_BUTT 
}RTSP_REQ_METHOD_E;

/*
 * method response codes.  These are 100 greater than their
 * associated method values.  This allows for simplified
 * creation of event codes that get used in event_handler()
 */
#define RTSP_MAKE_RESP_CMD(req) (req + 100) 

#define RTSP_STATUS_CONTINUE             100
#define RTSP_STATUS_OK                   200
#define RTSP_STATUS_ACCEPTED             202
#define RTSP_STATUS_BAD_REQUEST          400
#define RTSP_STATUS_UNAUTHORIZED         401
#define RTSP_STATUS_METHOD_NOT_ALLOWED   405
#define RTSP_STATUS_OVER_SUPPORTED_CONNECTION   416
#define RTSP_STATUS_SERVICE_UNAVAILIABLE 503
#define RTSP_STATUS_INTERNAL_SERVER_ERROR 500
typedef struct hiRTSP_Tokens
{
   char  *token;
   int   opcode;
} RTSP_TKN_S;
#define VOD_MAX_CHN 169

typedef struct hiurl_info{
    char server[32];
    char filename[128];
    unsigned short port;
    unsigned char multicast;
    unsigned char playback;
    unsigned char tunnel;
    //实时视频有效
    unsigned char channel;
    unsigned char streamType;
}url_info;

#define RTSP_AUTHENTICATE_NONE 0
#define RTSP_AUTHENTICATE_BASIC 1
#define RTSP_AUTHENTICATE_DIGEST 2


/*判断是否是resp消息， 用于RSTP_Valid_RespMsg*/
#define HI_RTSP_PARSE_OK 0

#define HI_RTSP_PARSE_IS_RESP  -4

#define HI_RTSP_PARSE_INVALID_OPCODE -1

#define HI_RTSP_PARSE_INVALID - 2

#define HI_RTSP_PARSE_ISNOT_RESP -3

#define HI_RTSP_PARSE_NO_USERAGENT -5

#define HI_RTSP_PARSE_INVALID_USERAGENT -6

#define HI_RTSP_PARSE_NO_AUTHORIZATION -7

#define HI_RTSP_PARSE_INVALID_AUTHORIZATION -8

#define HI_RTSP_PARSE_INVALID_CSEQ -9

#define HI_RTSP_PARSE_NO_SESSION -10

#define HI_RTSP_PARSE_INVALID_SESSION -11

#define HI_RTSP_PARSE_INVALID_URL_FOR_LIVE -12

char *RTSP_Get_Status_Str( int code );

char *RTSP_Get_Method_Str( int code );

#define HI_RTSP_BAD_STATUS_BEGIN 202


#define RTSP_DEFAULT_SVR_PORT 554

#define RTSP_CHECK_CHN(_chn) (( (_chn) >= 0) && ((_chn) < VOD_MAX_CHN))

char *RTSP_Get_Status_Str( int code );


HI_VOID RTSP_GetAudioFormatStr(MT_AUDIO_FORMAT_E enAudioFormat ,HI_CHAR * pAudioFormatStr,HI_S32 *s32Value);


HI_VOID RTSP_GetVideoFormatStr(MT_VIDEO_FORMAT_E enVideoFormat ,HI_CHAR * pVideoFormatStr,HI_S32 *s32Value);

HI_S32 RTSP_Get_SessId(HI_CHAR *pMsgStr, HI_CHAR *pszSessId);


HI_S32 RTSP_Get_CSeq(HI_CHAR *pStr,HI_S32 *pCseq);

HI_S32 RTSP_Get_Method( HI_CHAR *pStr,HI_S32 *pmethodcode);

/*porting from rtsp_ref::valid_resp_msg  */
HI_S32 RTSP_Valid_RespMsg( HI_CHAR *pStr, HI_U32 *pSeq_num, HI_U32 *pStatus);

/*Authorization:W54723 mypassword\r\n*/
HI_S32 RTSP_GetUserName(HI_CHAR *pRTSPRecvBuff, url_info *pUrlInfo, HI_CHAR *pUsername,HI_U32* pu32Userlen,
                        HI_CHAR * pPassword,HI_U32* pu32Pswlen);

HI_S32 RTSP_GetUserAuthenticateType();



/*先定位到User-Agent, 再定位到第一个空格, 就是: 后面, 然后已知搜索到行尾*/
    /*User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
HI_S32 RTSP_GetUserAgent(HI_CHAR *pRTSPRecvBuff, HI_CHAR *pUserAgentBuff,HI_U32* pu32Bufflen);

/*return chn number*/
HI_S32 RTSP_Get_Chn(HI_CHAR* pChn);

HI_VOID RTSP_GetPeerAddr(struct sockaddr_in* psaddr,HI_CHAR* pszCliIp,HI_U16 u16CliPort);

//HI_S32 RTSP_ParseParam(HI_CHAR *param, RTSP_REQ_METHOD_E *method, HI_U8 *meidatype);

/*porting from oms project: parse_url */
HI_S32 RTSP_Parse_Url(const char *url, url_info *urlinfo);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#endif

