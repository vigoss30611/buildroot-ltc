/****************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_rtsp_o_http_parse.h
* Description: rtsp_o_http common info and  error code list
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-01-31   w54723     NULL         Create this file.
*****************************************************************************/

#ifndef  __HI_RTSP_O_HTTP_PARSE_H__
#define  __HI_RTSP_O_HTTP_PARSE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_vs_media_comm.h"

#define RTSP_O_HTTP_VER_STR  "HTTP/1.1"

#define RTSP_O_HTTP_LR   "\r"
#define RTSP_O_HTTP_LF   "\n"
#define RTSP_O_HTTP_LRLF "\r\n"

typedef char RTSP_O_HTTP_URL[256];

#define RTSP_O_HTTP_METHOD_PLAY       "play"
#define RTSP_O_HTTP_METHOD_PAUSE      "pause"
#define RTSP_O_HTTP_METHOD_REPLAY     "replay"
#define RTSP_O_HTTP_METHOD_STOP       "stop"

/* message header keywords */
#define RTSP_O_HTTP_HDR_LIVE          "livestream"     /*rtsp_o_http url中指定为点播请求的标志字段*/
#define RTSP_O_HTTP_HDR_CONTENTLENGTH "Content-Length"
#define RTSP_O_HTTP_HDR_ACCEPT "Accept"
#define RTSP_O_HTTP_HDR_ALLOW "Allow"
#define RTSP_O_HTTP_HDR_CONTENTTYPE "Content-Type"
#define RTSP_O_HTTP_HDR_DATE "Date"
#define RTSP_O_HTTP_HDR_CSEQ "CSeq"
#define RTSP_O_HTTP_HDR_SESSION "Session"
#define RTSP_O_HTTP_HDR_TRANSPORT "Transport"
#define RTSP_O_HTTP_HDR_RANGE "Range"	
#define RTSP_O_HTTP_HDR_USER_AGENT "User-Agent"
#define RTSP_O_HTTP_HDR_AUTHORIZATION "Authorization"
#define RTSP_O_HTTP_HDR_sessioncookie "x-sessioncookie"
typedef enum hiRTSP_O_HTTP_REQ_METHOD
{
    /* method codes */
    RTSP_O_HTTP_PLAY_METHOD     = 0 ,
    RTSP_O_HTTP_PAUSE_METHOD     = 1 ,
    RTSP_O_HTTP_REPLAY_METHOD   = 2,
    RTSP_O_HTTP_PING_METHOD   = 3 ,
    RTSP_O_HTTP_TEARDOWN_METHOD  = 4 ,
    RTSP_O_HTTP_GET_PARAM_METHOD = 5 ,
    RTSP_O_HTTP_SET_PARAM_METHOD = 6,
    RTSP_O_HTTP_FIRST_GET_METHOD = 7,
    RTSP_O_HTTP_GET_METHOD = 8,
    
    RTSP_O_HTTP_POST_METHOD = 9,
    RTSP_O_HTTP_DISCRIBLE_METHOD = 10,
    RTSP_O_HTTP_SETUP_METHOD = 11,
    RTSP_O_HTTP_OPTIONS_METHOD = 12,
    
    RTSP_O_HTTP_REQ_METHOD_BUTT 
}RTSP_O_HTTP_REQ_METHOD_E;

/*
 * method response codes.  These are 100 greater than their
 * associated method values.  This allows for simplified
 * creation of event codes that get used in event_handler()
 */
#define RTSP_O_HTTP_MAKE_RESP_CMD(req) (req + 100) 

#define RTSP_O_HTTP_STATUS_CONTINUE             100
#define RTSP_O_HTTP_STATUS_OK                   200
#define RTSP_O_HTTP_STATUS_ACCEPTED             202
#define RTSP_O_HTTP_STATUS_BAD_REQUEST          400
#define RTSP_O_HTTP_STATUS_UNAUTHORIZED         401
#define RTSP_O_HTTP_STATUS_METHOD_NOT_ALLOWED   405
#define RTSP_O_HTTP_STATUS_OVER_SUPPORTED_CONNECTION   416
#define RTSP_O_HTTP_STATUS_SERVICE_UNAVAILIABLE 503
#define RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR 500
typedef struct hiRTSP_O_HTTP_Tokens
{
   char  *token;
   int   opcode;
} RTSP_O_HTTP_TKN_S;


/*判断是否是resp消息， 用于RSTP_Valid_RespMsg*/
#define HI_RTSP_O_HTTP_PARSE_OK 0

#define HI_RTSP_O_HTTP_PARSE_IS_RESP  -4

#define HI_RTSP_O_HTTP_PARSE_INVALID_OPCODE -1

#define HI_RTSP_O_HTTP_PARSE_INVALID - 2

#define HI_RTSP_O_HTTP_PARSE_ISNOT_RESP -3

#define HI_RTSP_O_HTTP_PARSE_NO_USERAGENT -5

#define HI_RTSP_O_HTTP_PARSE_INVALID_USERAGENT -6

#define HI_RTSP_O_HTTP_PARSE_NO_AUTHORIZATION -7

#define HI_RTSP_O_HTTP_PARSE_INVALID_AUTHORIZATION -8

#define HI_RTSP_O_HTTP_PARSE_INVALID_CSEQ -9

#define HI_RTSP_O_HTTP_PARSE_NO_SESSION -10

#define HI_RTSP_O_HTTP_PARSE_INVALID_SESSION -11

#define HI_RTSP_O_HTTP_PARSE_INVALID_URL_FOR_LIVE -12
#define HI_RTSP_O_HTTP_PARSE_INVALID_SESSIONCOOKIE -13

char *RTSP_Get_Status_Str( int code );

char *RTSP_Get_Method_Str( int code );

#define HI_RTSP_O_HTTP_BAD_STATUS_BEGIN 202


#define RTSP_O_HTTP_DEFAULT_SVR_PORT 80

#define RTSP_O_HTTP_CHECK_CHN(_chn) (( (_chn) >= 0) && ((_chn) < VOD_MAX_CHN))

char *RTSP_O_HTTP_Get_Status_Str( int code );


HI_VOID RTSP_O_HTTP_GetAudioFormatStr(MT_AUDIO_FORMAT_E enAudioFormat ,HI_CHAR * pAudioFormatStr,HI_S32 *s32Value);


HI_VOID RTSP_O_HTTP_GetVideoFormatStr(MT_VIDEO_FORMAT_E enVideoFormat ,HI_CHAR * pVideoFormatStr,HI_S32 *s32Value);

HI_S32 RTSP_O_HTTP_Get_SessId(HI_CHAR *pMsgStr, HI_CHAR *pszSessId);


HI_S32 RTSP_O_HTTP_Get_CSeq(HI_CHAR *pStr,HI_S32 *pCseq);

HI_S32 RTSP_O_HTTP_Get_sessioncookie(HI_CHAR *pStr,HI_CHAR *pSessCookie);

HI_S32 RTSP_O_HTTP_Get_Method( HI_CHAR *pStr,HI_S32 *pmethodcode);

/*porting from rtsp_ref::valid_resp_msg  */
HI_S32 RTSP_O_HTTP_Valid_RespMsg( HI_CHAR *pStr, HI_U32 *pSeq_num, HI_U32 *pStatus);

/*Authorization:W54723 mypassword\r\n*/
HI_S32 RTSP_O_HTTP_GetUserName(HI_CHAR *pHttpRecvBuff, HI_CHAR *pUsername,HI_U32* pu32Userlen,
                        HI_CHAR * pPassword,HI_U32* pu32Pswlen);


/*先定位到User-Agent, 再定位到第一个空格, 就是: 后面, 然后已知搜索到行尾*/
    /*User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
HI_S32 RTSP_O_HTTP_GetUserAgent(HI_CHAR *pHttpRecvBuff, HI_CHAR *pUserAgentBuff,HI_U32* pu32Bufflen);

/*return chn number*/
HI_S32 RTSP_O_HTTP_Get_Chn(HI_CHAR* pChn);

HI_VOID RTSP_O_HTTP_GetPeerAddr(struct sockaddr_in* psaddr,HI_CHAR* pszCliIp,HI_U16 u16CliPort);

HI_S32 RTSP_O_HTTP_ParseParam(HI_CHAR *param, RTSP_O_HTTP_REQ_METHOD_E *method, HI_U8 *meidatype);

/*porting from oms project: parse_url */
HI_S32 RTSP_O_HTTP_Parse_Url(const char *url, char *server, int *port, char *file_name);
HI_S32 RTSP_O_HTTP_GetFileName(const char *url, char *file_name);

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#endif

