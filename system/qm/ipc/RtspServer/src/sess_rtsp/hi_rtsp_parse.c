/****************************************************************************
*              Copyright 2006 - , Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_RTSP_parse.c
* Description: The rtsp server.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2006-05-20   t41030  NULL         Create this file.
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_type.h"

    
#include "hi_defs.h"

#include "list.h"
//#include "hi_adpt_interface.h"

#include "hi_mt_socket.h"

#include "hi_rtsp_parse.h"
#include "hi_rtp.h"
#include "hi_vs_media_comm.h"
#include "hi_msession_rtsp_error.h"
//#include "hi_log_app.h"
//#include "hi_debug_def.h"
#include "ext_defs.h"
#ifdef HI_PTHREAD_MNG
//#include "hi_pthread_wait.h"
#else
#include <pthread.h>
#endif
#include "hi_log_def.h"

#include "QMAPI.h"
//#include "md5.h"
//#include "sys_fun_interface.h"


#define MEDIA_TYPE_VIDEO 0x01           /*媒体类型为视频类型时为0000 0001 */
#define MEDIA_TYPE_AUDIO 0x02           /*媒体类型为音频类型时为0000 0010 */
#define MEDIA_TYPE_DATA  0x04           /*媒体类型为其他数据类型时为0000 0100 */

RTSP_TKN_S RTSP_Status [] =
{
   {"Continue", 100},
   {"OK", 200},
   {"Created", 201},
   {"Accepted", 202},
   {"Non-Authoritative Information", 203},
   {"No Content", 204},
   {"Reset Content", 205},
   {"Partial Content", 206},
   {"Multiple Choices", 300},
   {"Moved Permanently", 301},
   {"Moved Temporarily", 302},
   {"Bad Request", 400},
   {"Unauthorized", 401},
   {"Payment Required", 402},
   {"Forbidden", 403},
   {"Not Found", 404},
   {"Method Not Allowed", 405},
   {"Not Acceptable", 406},
   {"Proxy Authentication Required", 407},
   {"Request Time-out", 408},
   {"Conflict", 409},
   {"Gone", 410},
   {"Length Required", 411},
   {"Precondition Failed", 412},
   {"Request Entity Too Large", 413},
   {"Request-URI Too Large", 414},
   {"Unsupported Media Type", 415},
   {"Over Supported connection ", 416},
   {"Bad Extension", 420},
   {"Invalid Parameter", 450},
   {"Parameter Not Understood", 451},
   {"Conference Not Found", 452},
   {"Not Enough Bandwidth", 453},
   {"Session Not Found", 454},
   {"Method Not Valid In This State", 455},
   {"Header Field Not Valid for Resource", 456},
   {"Invalid Range", 457},
   {"Parameter Is Read-Only", 458},
   {"Internal Server Error", 500},
   {"Not Implemented", 501},
   {"Bad Gateway", 502},
   {"Service Unavailable", 503},
   {"Gateway Time-out", 504},
   {"RTSP Version Not Supported", 505},
   {"Extended Error:", 911},
   {0, HI_RTSP_PARSE_INVALID_OPCODE}
};


HI_CHAR  *RTSP_Invalid_Method_STR = "Invalid Method";






typedef struct
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
}MD5_CTX;

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
{ \
    a += F(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
#define GG(a,b,c,d,x,s,ac) \
{ \
    a += G(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
#define HH(a,b,c,d,x,s,ac) \
{ \
    a += H(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
#define II(a,b,c,d,x,s,ac) \
{ \
    a += I(b,c,d) + x + ac; \
    a = ROTATE_LEFT(a,s); \
    a += b; \
}
static void MD5Init(MD5_CTX *context);
static void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
static void MD5Final(MD5_CTX *context,unsigned char digest[16]);
static void MD5Transform(unsigned int state[4],unsigned char block[64]);
static void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
static void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);


static unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static void MD5Init(MD5_CTX *context)
{
    context->count[0] = 0;
    context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
}
static void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)
{
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if(context->count[0] < (inputlen << 3))
        context->count[1]++;
    context->count[1] += inputlen >> 29;

    if(inputlen >= partlen)
    {
        memcpy(&context->buffer[index],input,partlen);
        MD5Transform(context->state,context->buffer);
        for(i = partlen;i+64 <= inputlen;i+=64)
            MD5Transform(context->state,&input[i]);
        index = 0;
    }
    else
    {
        i = 0;
    }
    memcpy(&context->buffer[index],&input[i],inputlen-i);
}
static void MD5Final(MD5_CTX *context,unsigned char digest[16])
{
    unsigned int index = 0,padlen = 0;
    unsigned char bits[8];
    index = (context->count[0] >> 3) & 0x3F;
    padlen = (index < 56)?(56-index):(120-index);
    MD5Encode(bits,context->count,8);
    MD5Update(context,PADDING,padlen);
    MD5Update(context,bits,8);
    MD5Encode(digest,context->state,16);
}
static void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
        output[j] = input[i] & 0xFF;
        output[j+1] = (input[i] >> 8) & 0xFF;
        output[j+2] = (input[i] >> 16) & 0xFF;
        output[j+3] = (input[i] >> 24) & 0xFF;
        i++;
        j+=4;
    }
}
static void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
        output[i] = (input[j]) |
            (input[j+1] << 8) |
            (input[j+2] << 16) |
            (input[j+3] << 24);
        i++;
        j+=4;
    }
}
static void MD5Transform(unsigned int state[4],unsigned char block[64])
{
    unsigned int a = state[0];
    unsigned int b = state[1];
    unsigned int c = state[2];
    unsigned int d = state[3];
    unsigned int x[64];
    MD5Decode(x,block,64);
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478);
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);
    FF(c, d, a, b, x[ 2], 17, 0x242070db);
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf);
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a);
    FF(c, d, a, b, x[ 6], 17, 0xa8304613);
    FF(b, c, d, a, x[ 7], 22, 0xfd469501);
    FF(a, b, c, d, x[ 8], 7, 0x698098d8);
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);
    FF(c, d, a, b, x[10], 17, 0xffff5bb1);
    FF(b, c, d, a, x[11], 22, 0x895cd7be);
    FF(a, b, c, d, x[12], 7, 0x6b901122);
    FF(d, a, b, c, x[13], 12, 0xfd987193);
    FF(c, d, a, b, x[14], 17, 0xa679438e);
    FF(b, c, d, a, x[15], 22, 0x49b40821);


    GG(a, b, c, d, x[ 1], 5, 0xf61e2562);
    GG(d, a, b, c, x[ 6], 9, 0xc040b340);
    GG(c, d, a, b, x[11], 14, 0x265e5a51);
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d);
    GG(d, a, b, c, x[10], 9,  0x2441453);
    GG(c, d, a, b, x[15], 14, 0xd8a1e681);
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6);
    GG(d, a, b, c, x[14], 9, 0xc33707d6);
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87);
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed);
    GG(a, b, c, d, x[13], 5, 0xa9e3e905);
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8);
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9);
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);


    HH(a, b, c, d, x[ 5], 4, 0xfffa3942);
    HH(d, a, b, c, x[ 8], 11, 0x8771f681);
    HH(c, d, a, b, x[11], 16, 0x6d9d6122);
    HH(b, c, d, a, x[14], 23, 0xfde5380c);
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44);
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60);
    HH(b, c, d, a, x[10], 23, 0xbebfbc70);
    HH(a, b, c, d, x[13], 4, 0x289b7ec6);
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085);
    HH(b, c, d, a, x[ 6], 23,  0x4881d05);
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039);
    HH(d, a, b, c, x[12], 11, 0xe6db99e5);
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);


    II(a, b, c, d, x[ 0], 6, 0xf4292244);
    II(d, a, b, c, x[ 7], 10, 0x432aff97);
    II(c, d, a, b, x[14], 15, 0xab9423a7);
    II(b, c, d, a, x[ 5], 21, 0xfc93a039);
    II(a, b, c, d, x[12], 6, 0x655b59c3);
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
    II(c, d, a, b, x[10], 15, 0xffeff47d);
    II(b, c, d, a, x[ 1], 21, 0x85845dd1);
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f);
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
    II(c, d, a, b, x[ 6], 15, 0xa3014314);
    II(b, c, d, a, x[13], 21, 0x4e0811a1);
    II(a, b, c, d, x[ 4], 6, 0xf7537e82);
    II(d, a, b, c, x[11], 10, 0xbd3af235);
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
    II(b, c, d, a, x[ 9], 21, 0xeb86d391);
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}
/* MD5Code
  const BYTE *src: 源串
  int sLen: 源串长度
  unsigned char *buf: 目标md5串(16字节)
*/
void MD5Code(const unsigned char *src, int sLen, unsigned char *buf)
{
	MD5_CTX c;
	MD5Init(&c);
	MD5Update(&c, (unsigned char *)src, sLen);
	MD5Final(&c, buf);
}

char *MD5Str(const unsigned char *src, int sLen, char *s)
{
	unsigned char buf[16];
	int i;
	MD5Code(src, sLen, buf);
	for(i=0;i<16;i++){
        sprintf(s+i*2, "%02x",buf[i]);
    }
	return s;
}




/* message header keywords */

HI_CHAR *RTSP_Get_Status_Str( int code )
{
   RTSP_TKN_S  *pTkn;
   
   for ( pTkn = RTSP_Status; pTkn->opcode != HI_RTSP_PARSE_INVALID_OPCODE; pTkn++ )
   {
      if ( pTkn->opcode == code )
      {
         return( pTkn->token );
      }
   }

   //WRITE_LOG_ERROR("Invalid status code.%d .\n", code);
   return( RTSP_Invalid_Method_STR );
}

HI_VOID RTSP_GetAudioFormatStr(MT_AUDIO_FORMAT_E enAudioFormat ,HI_CHAR * pAudioFormatStr,HI_S32 *s32Value)
{
    if(MT_AUDIO_FORMAT_G711A == enAudioFormat)
    {
         strcpy(pAudioFormatStr,"PCMA");
         *s32Value = RTP_PT_ALAW;
    }
    else if(MT_AUDIO_FORMAT_G711Mu == enAudioFormat)
    {
        strcpy(pAudioFormatStr,"PCMU");
        *s32Value = RTP_PT_ULAW;
    }
    else if(MT_AUDIO_FORMAT_G726 == enAudioFormat)
    {
        strcpy(pAudioFormatStr,"G726");
        *s32Value = RTP_PT_G726;
    }
    else if(MT_AUDIO_FORMAT_AMR == enAudioFormat)
    {
        strcpy(pAudioFormatStr,"AMR");
        *s32Value = RTP_PT_ARM;
    }
}

HI_VOID RTSP_GetVideoFormatStr(MT_VIDEO_FORMAT_E enVideoFormat ,HI_CHAR * pVideoFormatStr,HI_S32 *s32Value)
{
    if(MT_VIDEO_FORMAT_H264 == enVideoFormat)
    {
         strcpy(pVideoFormatStr,"H264");
         *s32Value = RTP_PT_H264;
    }
    else if(MT_VIDEO_FORMAT_H261 == enVideoFormat)
    {
        strcpy(pVideoFormatStr,"H261");
        *s32Value = RTP_PT_H261;
    }
    else if(MT_VIDEO_FORMAT_H263 == enVideoFormat)
    {
        strcpy(pVideoFormatStr,"H263");
        *s32Value = RTP_PT_H263;
    }
    else if(MT_VIDEO_FORMAT_MJPEG == enVideoFormat)
    {
        strcpy(pVideoFormatStr,"MJPEG");
        *s32Value = RTP_PT_MJPEG;
    }
}

HI_S32 RTSP_Get_SessId(HI_CHAR *pMsgStr, HI_CHAR *pszSessId)
{
    HI_CHAR * pTemp = NULL;
    /*获取Sessid信息*/
	if ((pTemp = strstr(pMsgStr, RTSP_HDR_SESSION)) == NULL) 
    {
        return HI_RTSP_PARSE_NO_SESSION;
    }
    else
    {
		if (sscanf(pTemp, "%*s %s", pszSessId) != 1) 
        {
            return HI_RTSP_PARSE_INVALID_SESSION;
        }
    }

    return HI_SUCCESS;
}

HI_S32 RTSP_Get_CSeq(HI_CHAR *pStr,HI_S32 *pCseq)
{
    HI_CHAR * pTemp = NULL;
    /*获取CSeq信息*/
	if ((pTemp = strstr(pStr, RTSP_HDR_CSEQ)) == NULL) 
    {
        return HI_RTSP_PARSE_INVALID_CSEQ;
	} 
    else 
    {
    	if (sscanf(pTemp, "CSeq:%d", pCseq) != 1) 
		//if (sscanf(pTemp, "%*s %d", pCseq) != 1) 
        {
            return HI_RTSP_PARSE_INVALID_CSEQ;
		}
	}
	
    return HI_SUCCESS;
}

HI_S32 RTSP_Get_Method( HI_CHAR *pStr,HI_S32 *pmethodcode)
{
    HI_CHAR           method [32]= {0};
    HI_CHAR           object [256]= {0};
   // HI_CHAR           ver [32]= {0};
  //  HI_S32            pcnt = 0;          /* parameter count */
  //  HI_CHAR *         pos = NULL;
  //  RTSP_REQ_METHOD_E methodcode = RTSP_REQ_METHOD_BUTT;
    /*
    * Check for a valid method token and convert
    * it to a switch code.
    * Returns -2 if there are not 4 valid tokens in the request line.
    * Returns -1 if the Method token is invalid.
    * Otherwise, the method code is returned.
    */
    *method = *object = '\0';

    HI_CHAR		achMethod[ 15 ];
	if( 1 != sscanf( pStr, "%15[^ ]", achMethod ))
	{
		printf(
            "Parse URL Failed!Read Method Fail.  %s\n",pStr );
		return HI_RTSP_PARSE_INVALID;
	}
    //printf("RTSP_Get_Method-Method: %s !\n", achMethod);
    
	if( 0 == strcmp( achMethod, "DESCRIBE" )) 
	{
        *pmethodcode = RTSP_DISCRIBLE_METHOD;
	}
	else if( 0 == strcmp( achMethod, "SETUP" ))
	{
        *pmethodcode = RTSP_SETUP_METHOD;
	}
	else if( 0 == strcmp( achMethod, "PLAY" ))
	{
        *pmethodcode = RTSP_PLAY_METHOD;
	}
	else if( 0 == strcmp( achMethod, "TEARDOWN" ))
	{
        *pmethodcode = RTSP_TEARDOWN_METHOD;
	}
	else if( 0 == strcmp( achMethod, "OPTIONS" ))
	{
        *pmethodcode = RTSP_OPTIONS_METHOD;
	}
	else if(0 == strcmp(achMethod,"PAUSE"))
	{
        *pmethodcode = RTSP_PAUSE_METHOD;
	}
	else if(0 == strcmp(achMethod,"SET_PARAMETER"))
	{
		*pmethodcode = RTSP_SET_PARAM_METHOD;
	}
    else if(0 == strcmp(achMethod,"CMD:INSERTIFRAME"))
    {
        *pmethodcode = RTSP_REQUEST_I_FRAME;
    }
	else if(0 == strcmp(achMethod,"GET_PARAMETER"))
	{
		*pmethodcode = RTSP_GET_PARAM_METHOD;
	}
	else if(0 == strcmp(achMethod,"HEARTBEAT"))
	{
		*pmethodcode = RTSP_HEARTBEAT_METHOD;
	}
	else
	{
        return HI_RTSP_PARSE_INVALID_OPCODE;
	}

    return HI_SUCCESS;
}


/*porting from rtsp_ref::valid_resp_msg  */
HI_S32 RTSP_Valid_RespMsg( HI_CHAR *pStr, HI_U32 *pSeq_num, HI_U32 *pStatus)
{
   HI_CHAR        ver [32] = {0};
   HI_U32         stat = 0;;
   HI_S32         pcnt = 0;          /* parameter count */

   HI_CHAR           uncare[256] = {0};

   /* assuming "stat" may not be zero (probably faulty) */
   stat = 0;
   pcnt = sscanf( pStr, " %31s %d  %255s ", ver, &stat,  uncare );

   if ( strncasecmp ( ver, "RTSP/", 5 ) )
   {
      return( HI_RTSP_PARSE_ISNOT_RESP);  /* not a response message */
   }

   /* confirm that at least the version, status code and sequence are there. */
   if ( pcnt < 3 || stat == 0 )
   {
      return( HI_RTSP_PARSE_ISNOT_RESP );  /* not a response message */
   }

   *pStatus = stat;
   return( HI_RTSP_PARSE_IS_RESP);
}



HI_S32 RTSP_GetUserAuthenticateType()
{
	QMAPI_NET_RTSP_CFG rtspcfg={0};
	int s32Ret;
	s32Ret=QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_RTSPCFG,0,&rtspcfg,sizeof(rtspcfg));
	if (HI_SUCCESS!=s32Ret)
		return HI_FAILURE;
	return rtspcfg.nAuthenticate;
}


/*Authorization: W54723 mypassword\r\n*/
HI_S32 RTSP_GetUserName(HI_CHAR *pRTSPRecvBuff, url_info *pUrlInfo, HI_CHAR *pUsername,HI_U32* pu32Userlen,
                        HI_CHAR * pPassword,HI_U32* pu32Pswlen)
{
    HI_CHAR *pTemp = NULL;
	char tmp[128];
	char tmp1[128];
	char buf[128];
	char buf1[128];
	char buf2[128];
	char buf3[128];
	char namebuf[128];
	char realm[128];
	char nonce[128];
	char uri[128];
	char responsebuf[128];
	QMAPI_NET_USER_INFO stUserInfo;
	int s32Ret, i;
    if(NULL == pRTSPRecvBuff || NULL == pUsername || NULL == pu32Userlen
       || NULL == pPassword|| NULL ==  pu32Pswlen)
    {
        return HI_ERR_RTSP_ILLEGALE_PARAM;
    }
    
    memset(pUsername,0,sizeof(HI_CHAR)*(*pu32Userlen));
    memset(pPassword,0,sizeof(HI_CHAR)*(*pu32Pswlen));

//RTSP 摘要认证登录方法
#if 1
    if(NULL != (pTemp = strstr(pRTSPRecvBuff, "DESCRIBE")))
    {
    	printf("#### %s %d, pRTSPRecvBuff:%s\n", __FUNCTION__, __LINE__, pRTSPRecvBuff);
		if(NULL == (pTemp = strstr(pTemp, RTSP_HDR_AUTHORIZATION)))
			return HI_RTSP_PARSE_NO_AUTHORIZATION;
		printf("#### %s %d, pTemp:%s\n", __FUNCTION__, __LINE__, pTemp);
/*
例如：OPTIONS rtsp://192.168.123.158:554/11 RTSP/1.0
字段依次包含：public_method,uri地址，协议版本
DESCRIBE rtsp://192.168.90.32/11 RTSP/1.0
CSeq: 6
Authorization: Digest username="admin", realm="LIVE555 Streaming Media", nonce="70b16a51eb135f69c972231f4dbeff5e", uri="rtsp://192.168.90.32/11", response="a81b32c66eae0f1945ea82fb9b04ceed"
User-Agent: LibVLC/2.2.0 (LIVE555 Streaming Media v2014.07.25)
Accept: application/sdp
*/
/*
(1)当password为MD5编码,则
   response = md5(password:nonce:md5(public_method:url));
*/
#if 0
    	MD5Str("DESCRIBE:rtsp://192.168.90.32/11", strlen("DESCRIBE:rtsp://192.168.90.32/11"), buf1);
		printf("#### %s %d, md5buf:%s\n", __FUNCTION__, __LINE__, buf1);
		sprintf(buf2, "admin:70b16a51eb135f69c972231f4dbeff5e:%s", buf1);
		MD5Str(buf2, strlen(buf2), buf1);
#endif
/*
(2)当password为ANSI字符串,则
    response= md5(md5(username:realm:password):nonce:md5(public_method:url));
*/
		if((NULL != strstr(pRTSPRecvBuff, "Authorization: Basic")) ||(NULL != strstr(pRTSPRecvBuff, "Authorization:Basic")))	//Basic Authentication  基本认证,暂时忽略  直接通过
		{
			pTemp= strstr(pRTSPRecvBuff, "Basic");
			if (sscanf(pTemp,"Basic %s",responsebuf)==1) {
				memset(tmp,0,sizeof(tmp));
				base64decode(responsebuf,strlen(responsebuf),tmp,sizeof(tmp));
				sscanf(tmp,"%[^:]:%s",pUsername,pPassword);
			//	printf("DEBUG %s %d username=%s pwd=%s tmp=%s\n",__FILE__,__LINE__,pUsername,pPassword,tmp);
				*pu32Userlen = strlen(pUsername);
				*pu32Pswlen = strlen(pPassword);
				for(i = 0; i < 10; i++) {
					//s32Ret = QMapi_sys_ioctrl(1, QMAPI_NET_GET_USERCFG_V2, i, &stUserInfo, sizeof(stUserInfo));
					s32Ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_USERCFG, i, &stUserInfo, sizeof(stUserInfo));
					if(HI_SUCCESS != s32Ret){
						printf("%s(%d): QMAPI_SYSCFG_GET_PICCFG failure!\n", __FUNCTION__, __LINE__);
						return HI_FAILURE;
					}
					printf("DEBUG %s %d csusername=%s,pwd=%s, usersend %s,%s\n",__FILE__,__LINE__,stUserInfo.csUserName,stUserInfo.csPassword,pUsername,pPassword);
					if ((strncmp(pUsername,stUserInfo.csUserName,sizeof(pUsername))==0) && (strncmp(pPassword,stUserInfo.csPassword,sizeof(pPassword))==0))
						return HI_SUCCESS;
				}
				
				if(10 == i)
					return HI_RTSP_PARSE_INVALID_AUTHORIZATION;
			}

			strncpy(pUsername, "ipcam", 5);
			strncpy(pPassword, "w54723", 6);
			*pu32Userlen = strlen(pUsername);
			*pu32Pswlen = strlen(pPassword);
			return HI_SUCCESS;
		}
		printf("%s,%d Digest check...... \n",__FILE__,__LINE__);
		
		pTemp = strstr(pTemp, "username=\"");
		if (NULL == pTemp) {
			printf("%s,%d find username failed \n",__FILE__,__LINE__);
			return HI_FAILURE;
		}
		sscanf(pTemp, "username=\"%s\"", namebuf);
		pTemp = strstr(pTemp, "realm=\"");
		if (NULL == pTemp) {
			printf("%s,%d find realm failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "realm=\"%s\"", realm);
		pTemp = strstr(pTemp, "nonce=\"");
		if (NULL == pTemp) {
			printf("%s,%d find nonce failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "nonce=\"%s\"", nonce);
		pTemp = strstr(pTemp, "uri=\"");
		if (NULL == pTemp) {
			printf("%s,%d find uri failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "uri=\"%s\"", uri);
		pTemp = strstr(pTemp, "response=\"");
		if (NULL == pTemp) {
			printf("%s,%d find response failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "response=\"%s\"", responsebuf);

		bzero(tmp, sizeof(tmp));
		strncpy(tmp, uri, (strlen(uri)-2));
		bzero(tmp1, sizeof(tmp1));
		sprintf(tmp1, "DESCRIBE:%s", tmp);
		MD5Str(tmp1, strlen(tmp1), buf);
		//sys_fun_md5str(tmp1,strlen(tmp1),buf);
		
		for(i = 0; i < 10; i++)
		{
			//s32Ret = QMapi_sys_ioctrl(1, QMAPI_NET_GET_USERCFG_V2, i, &stUserInfo, sizeof(stUserInfo));
			s32Ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_USERCFG, i, &stUserInfo, sizeof(stUserInfo));
			if(HI_SUCCESS != s32Ret)
			{
				printf("%s(%d): QMAPI_SYSCFG_GET_PICCFG failure!\n", __FUNCTION__, __LINE__);
				return HI_FAILURE;
			}

			sprintf(tmp, "%s:LIVE555 Streaming Media:%s", stUserInfo.csUserName, stUserInfo.csPassword);
			//sys_fun_md5str(tmp, strlen(tmp), buf2);
			MD5Str(tmp, strlen(tmp), buf2);
			sprintf(buf3, "%s:70b16a51eb135f69c972231f4dbeff5e:%s", buf2, buf);
			//sys_fun_md5str(buf3, strlen(buf3), buf1);
			MD5Str(buf3, strlen(buf3), buf1);

			if(0 == strncmp(stUserInfo.csUserName, namebuf, strlen(stUserInfo.csUserName)))
			{
				if(0 == strncmp(buf1, responsebuf, strlen(buf1)))
					break;
			}
		}

		if(10 == i)
			return HI_RTSP_PARSE_INVALID_AUTHORIZATION;

		strcpy(pUsername, stUserInfo.csUserName);
		strcpy(pPassword, stUserInfo.csPassword);
        *pu32Userlen = strlen(pUsername);
        *pu32Pswlen = strlen(pUsername);
		printf("#### %s %d, pUsername:%s, pPassword:%s\n", __FUNCTION__, __LINE__, pUsername, pPassword);
    }
	else if(NULL != (pTemp = strstr(pRTSPRecvBuff, "SETUP")))
	{
		if(NULL == (pTemp = strstr(pTemp, RTSP_HDR_AUTHORIZATION)))
			return HI_RTSP_PARSE_NO_AUTHORIZATION;

		if((NULL != strstr(pRTSPRecvBuff, "Authorization: Basic")) || (NULL != strstr(pRTSPRecvBuff, "Authorization:Basic")))	//Basic Authentication  基本认证,暂时忽略  直接通过
		{
			strncpy(pUsername, "ipcam", 5);
			strncpy(pPassword, "w54723", 6);
			*pu32Userlen = strlen(pUsername);
			*pu32Pswlen = strlen(pPassword);

			return HI_SUCCESS;
		}

		pTemp = strstr(pTemp, "username=\"");
		if (NULL == pTemp) {
			printf("%s,%d find username failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "username=\"%s\"", namebuf);
		pTemp = strstr(pTemp, "realm=\"");
		if (NULL == pTemp) {
			printf("%s,%d find realm failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "realm=\"%s\"", realm);
		pTemp = strstr(pTemp, "nonce=\"");
		if (NULL == pTemp) {
			printf("%s,%d find nonce failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "nonce=\"%s\"", nonce);
		pTemp = strstr(pTemp, "uri=\"");
		if (NULL == pTemp) {
			printf("%s,%d find uri failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "uri=\"%s\"", uri);
		pTemp = strstr(pTemp, "response=\"");
		if (NULL == pTemp) {
			printf("%s,%d find response failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "response=\"%s\"", responsebuf);

		bzero(tmp, sizeof(tmp));
		strncpy(tmp, uri, (strlen(uri)-2));
		bzero(tmp1, sizeof(tmp1));
		sprintf(tmp1, "SETUP:%s", tmp);
		//sys_fun_md5str(tmp1, strlen(tmp1), buf);
		MD5Str(tmp1, strlen(tmp1), buf);

		for(i = 0; i < 10; i++)
		{
			//s32Ret = QMapi_sys_ioctrl(1, QMAPI_NET_GET_USERCFG_V2, i, &stUserInfo, sizeof(stUserInfo));
			s32Ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_USERCFG, i, &stUserInfo, sizeof(stUserInfo));
			if(HI_SUCCESS != s32Ret)
			{
				printf("%s(%d): QMAPI_SYSCFG_GET_PICCFG failure!\n", __FUNCTION__, __LINE__);
				return HI_FAILURE;
			}

			sprintf(tmp, "%s:LIVE555 Streaming Media:%s", stUserInfo.csUserName, stUserInfo.csPassword);
			//sys_fun_md5str(tmp, strlen(tmp), buf2);
			MD5Str(tmp, strlen(tmp), buf2);
			sprintf(buf3, "%s:70b16a51eb135f69c972231f4dbeff5e:%s", buf2, buf);
			//sys_fun_md5str(buf3, strlen(buf3), buf1);
			MD5Str(buf3, strlen(buf3), buf1);

			if(0 == strncmp(stUserInfo.csUserName, namebuf, strlen(stUserInfo.csUserName)))
			{
				if(0 == strncmp(buf1, responsebuf, strlen(buf1)))
					break;
			}
		}

		if(10 == i)
			return HI_RTSP_PARSE_INVALID_AUTHORIZATION;

		strcpy(pUsername, stUserInfo.csUserName);
		strcpy(pPassword, stUserInfo.csPassword);
        *pu32Userlen = strlen(pUsername);
        *pu32Pswlen = strlen(pUsername);
	}
	else if(NULL != (pTemp = strstr(pRTSPRecvBuff, "PLAY")))
	{
		if(NULL == (pTemp = strstr(pTemp, RTSP_HDR_AUTHORIZATION)))
			return HI_RTSP_PARSE_NO_AUTHORIZATION;

		if((NULL != strstr(pRTSPRecvBuff, "Authorization: Basic"))||(NULL != strstr(pRTSPRecvBuff, "Authorization:Basic")) )	//Basic Authentication  基本认证,暂时忽略  直接通过
		{
			strncpy(pUsername, "ipcam", 5);
			strncpy(pPassword, "w54723", 6);
			*pu32Userlen = strlen(pUsername);
			*pu32Pswlen = strlen(pPassword);

			return HI_SUCCESS;
		}

		pTemp = strstr(pTemp, "username=\"");
		if (NULL == pTemp) {
			printf("%s,%d find username failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}	
		sscanf(pTemp, "username=\"%s\"", namebuf);
		pTemp = strstr(pTemp, "realm=\"");
		if (NULL == pTemp) {
			printf("%s,%d find realm failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "realm=\"%s\"", realm);
		pTemp = strstr(pTemp, "nonce=\"");
		if (NULL == pTemp) {
			printf("%s,%d find nonce failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "nonce=\"%s\"", nonce);
		pTemp = strstr(pTemp, "uri=\"");
		if (NULL == pTemp) {
			printf("%s,%d find uri failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "uri=\"%s\"", uri);
		pTemp = strstr(pTemp, "response=\"");
		if (NULL == pTemp) {
			printf("%s,%d find response failed \n",__FILE__,__LINE__);
			return HI_FAILURE;	
		}
		sscanf(pTemp, "response=\"%s\"", responsebuf);

		bzero(tmp, sizeof(tmp));
		strncpy(tmp, uri, (strlen(uri)-2));
		bzero(tmp1, sizeof(tmp1));
		sprintf(tmp1, "PLAY:%s", tmp);
		//sys_fun_md5str(tmp1, strlen(tmp1), buf1);
		MD5Str(tmp1, strlen(tmp1), buf1);

		for(i = 0; i < 10; i++)
		{
			//s32Ret = QMapi_sys_ioctrl(1, QMAPI_NET_GET_USERCFG_V2, i, &stUserInfo, sizeof(stUserInfo));
			s32Ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_USERCFG, i, &stUserInfo, sizeof(stUserInfo));
			if(HI_SUCCESS != s32Ret)
			{
				printf("%s(%d): QMAPI_SYSCFG_GET_PICCFG failure!\n", __FUNCTION__, __LINE__);
				return HI_FAILURE;
			}

			sprintf(tmp, "%s:LIVE555 Streaming Media:%s", stUserInfo.csUserName, stUserInfo.csPassword);
			//sys_fun_md5str(tmp, strlen(tmp), buf2);
			MD5Str(tmp, strlen(tmp), buf2);
			sprintf(buf3, "%s:70b16a51eb135f69c972231f4dbeff5e:%s", buf2, buf);
			//sys_fun_md5str(buf3, strlen(buf3), buf1);
			MD5Str(buf3, strlen(buf3), buf1);

			if(0 == strncmp(stUserInfo.csUserName, namebuf, strlen(stUserInfo.csUserName)))
			{
				if(0 == strncmp(buf1, responsebuf, strlen(buf1)))
					break;
			}
		}

		if(10 == i)
			return HI_RTSP_PARSE_INVALID_AUTHORIZATION;

		strcpy(pUsername, stUserInfo.csUserName);
		strcpy(pPassword, stUserInfo.csPassword);
        *pu32Userlen = strlen(pUsername);
        *pu32Pswlen = strlen(pUsername);
	}
    else
    {
    	printf("#### %s %d, pRTSPRecvBuff:%s\n", __FUNCTION__, __LINE__, pRTSPRecvBuff);
        return HI_RTSP_PARSE_NO_AUTHORIZATION;
    }
#else
    strncpy(pUsername,"ipcam",5);
    strncpy(pPassword,"w54723",6);
    *pu32Userlen = strlen(pUsername);
    *pu32Pswlen = strlen(pPassword);
#endif
    return HI_SUCCESS;
}

/*先定位到User-Agent, 再定位到第一个空格, 就是: 后面, 然后已知搜索到行尾*/
    /*User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
	/*User-Agent:NKPlayer-1.00.00.0*/
	
/*to do :review this func*/
HI_S32 RTSP_GetUserAgent(HI_CHAR *pRTSPRecvBuff, HI_CHAR *pUserAgentBuff,HI_U32* pu32Bufflen)
{
    HI_CHAR *pParseTmp = NULL;
   
    if ( NULL == ( pParseTmp = strstr(pRTSPRecvBuff, "User-Agent"))  )
    {
    	return HI_RTSP_PARSE_NO_USERAGENT;
    }
	else
	{
		if (sscanf(pParseTmp, "%*s %[^\r]", pUserAgentBuff) != 1)
        {
        	if (sscanf(pParseTmp , "%*[^:]:%[^\r]",pUserAgentBuff)!=1) 
        		{
        			printf("pParseTmp =%s sscanf failed\n");
            		return HI_RTSP_PARSE_INVALID_USERAGENT;
        		}
		}
        *pu32Bufflen = strlen(pUserAgentBuff) ;
	}
    return HI_SUCCESS;
}
#if 0
/*return chn number*/
HI_S32 RTSP_Get_Chn(HI_CHAR* pChn)
{
    int chn = 0;
    if (pChn == NULL)
    {
        return 0;
    }
    else
    {
        chn = pChn[0] - '0';
        //TODO macro
        if (chn <0 || chn > 4)
        {
            chn = 0;
        }
        return chn;
    }
}
#endif
HI_VOID RTSP_GetPeerAddr(struct sockaddr_in* psaddr,HI_CHAR* pszCliIp,HI_U16 u16CliPort)
{
    //psaddr = (struct sockaddr_in*)psaddr;
    psaddr->sin_family = AF_INET;
    psaddr->sin_addr.s_addr = inet_addr(pszCliIp);
    psaddr->sin_port = htons(u16CliPort);
    memset(psaddr->sin_zero, '\0', 8);

}
#if 0
HI_S32 RTSP_ParseParam(HI_CHAR *param, RTSP_REQ_METHOD_E *method, HI_U8 *meidatype)
{
        /*to do:应该把param指向的内容存下了*/
        HI_CHAR *P1 = NULL;
        HI_CHAR methodstr[32]={0};
        
        P1 = strstr(param, "action=");
        /*if has "action=", url must has method such as play or pause,
         otherwise, invalid url*/
        if( NULL != P1 )
        {
            /*url like this:  'action=replay&media=...', method = replay */
            if(1 != sscanf(P1,"%*[^=]=%32[^&]", methodstr))
            {
                return HI_RTSP_PARSE_INVALID_OPCODE;
            }
            
            if(0 == strcmp(methodstr,RTSP_METHOD_PLAY))
            {
                *method =  RTSP_PLAY_METHOD;
            }
            else if(0 == strcmp(methodstr,RTSP_METHOD_PAUSE))
            {
                *method =  RTSP_PAUSE_METHOD;
            }
            else if(0 == strcmp(methodstr,RTSP_METHOD_REPLAY))
            {
                *method =  RTSP_REPLAY_METHOD;
            }
            else if(0 == strcmp(methodstr,RTSP_METHOD_STOP))
            {
                *method =  RTSP_TEARDOWN_METHOD;
            }
            else
            {
                return HI_RTSP_PARSE_INVALID_OPCODE;
            }
                
        }
		/*has no 'action=', set play action default*/
		else
		{
			*method=RTSP_PLAY_METHOD;
		}

        /*clear media type */
        *meidatype = 0;
        
        P1 = strstr(param, "media=");
        /* if has no "action=", url not has method, url like this: .../live?media=vido_audio*/
        if( NULL != P1 )
        {
            HI_CHAR *ptr3=strstr(param,"video");
            HI_CHAR *ptr4=strstr(param,"audio");
            HI_CHAR *ptr5=strstr(param,"data");

            /*we get meidatype :video&audio&data */
            if(NULL != ptr3)
            {
                *meidatype = *meidatype|MEDIA_TYPE_VIDEO;
            }

            if(NULL != ptr4)
            {
                *meidatype = *meidatype|MEDIA_TYPE_AUDIO;
            }
            if(NULL != ptr5)
            {
                *meidatype = *meidatype|MEDIA_TYPE_DATA;
            }

        }
        else
        {
              /*url like this: .../livestream?  or .../livestream?somestr
              we set meidatype=video_audio_data */
            *meidatype = MEDIA_TYPE_VIDEO|MEDIA_TYPE_AUDIO|MEDIA_TYPE_DATA;
        }
        return HI_SUCCESS;
}
#endif

HI_S32 handleHKvsRtsp(const char *p,  url_info *urlinfo)
{
	
	printf("#### %s:%d, input url %s\n", __FUNCTION__, __LINE__, p);
	
		if(strstr(p, "ch1"))
		{
			if(strstr(p, "sub"))
				strcpy(urlinfo->filename, "12");
			if(strstr(p, "main"))
				strcpy(urlinfo->filename, "11");
			return 1;
		} 
	
		return 0;
	
}


/*porting from oms project: parse_url */
HI_S32 RTSP_Parse_Url(const char *url, url_info *urlinfo)
{
    /* expects format '[rtsp://server[:port/]]filename' */
    //   rtsp://192.168.1.151:554/mcast/11

    char *p;
    char *q=NULL;
    int len = 0;
    int ret = HI_FAILURE;
    /* copy url */
    char *full = malloc(strlen(url) + 1);
    urlinfo->port = RTSP_DEFAULT_SVR_PORT;
    urlinfo->multicast = 0;
    urlinfo->playback = HI_FALSE;
    strcpy(full, url);
    if (strncmp(full, "rtsp://", 7) == 0) 
    {
        char *aSubStr = malloc(strlen(url) + 1);
        memset(aSubStr, 0, strlen(url) + 1);
        strcpy(aSubStr, &full[7]);
        
        p=strstr(aSubStr, "mcast");
        if(p)
        {
            urlinfo->multicast = 1;
        }
        
        q=strchr(aSubStr, ':');
        p=strchr(aSubStr, '/');
        if(q)
        {
            len = q-aSubStr;
            strncpy(urlinfo->server, aSubStr, len);

            if(*(q+1)!='\0')
                urlinfo->port = atoi(q+1);
        }
        else if(p)
        {
            len = p-aSubStr;
            strncpy(urlinfo->server, aSubStr, len);
        }
        else
        {
            strcpy(urlinfo->server, aSubStr);
        }
        
        if(p)
        {
            if(strncmp(p+1, "playback/", strlen("playback/"))==0)
            {
            	printf("playback == true\n");
                urlinfo->playback = HI_TRUE;
                strcpy(urlinfo->filename, p+strlen("/playback"));
                //strtok(file_name, " \r\t\n");
                int i;
                for(i=0;*(urlinfo->filename+i)!='\0';i++)
                {
                    if(*(urlinfo->filename+i)==' ' 
                        || *(urlinfo->filename+i)=='\n' 
                        || *(urlinfo->filename+i)=='\t' 
                        || *(urlinfo->filename+i)=='\r')
                    {
                        *(urlinfo->filename+i)='\0';
                        break;
                    }
                }
                free(aSubStr);
                free(full);
                return HI_SUCCESS;
            }
            else if(strncmp(p, "/av", 2) == 0)//思科平台
            {
                int chn,stream;
                if(sscanf(p, "/av%d_%d", &chn, &stream) == 2)
                {
                    int id;
                    if(p=strstr(aSubStr, "trackID="))
                    {
                        id = atoi(p+strlen("trackID="));
                        sprintf(urlinfo->filename, "%d%d/trackID=%d", chn+1, stream+1,id);
                    }
                    else
                    {
                        sprintf(urlinfo->filename, "%d%d", chn+1, stream+1);
                    }
                    free(aSubStr);
                    free(full);
                    return HI_SUCCESS;
                }
            }
            else if(strstr(p, "hashtoken="))//天翼家+
            {
                int chn = 0,stream = 1;
                if(p=strstr(aSubStr, "channelno="))
                {
                    chn = atoi(p+strlen("channelno="));
                }

                if(p=strstr(aSubStr, "streamtype="))
                {
                    stream = atoi(p+strlen("streamtype="));
                }

                sprintf(urlinfo->filename, "%d%d", chn+1, stream);
                free(aSubStr);
                free(full);
                return HI_SUCCESS;
            }
            else if((q = strstr(aSubStr, "cast/")) && (*(q+5)!= '\0'))
            {
                strcpy(urlinfo->filename, q+5);
            }

			else if(strstr(p, "mpeg4"))
            {
            	printf("#### %s %d\n", __FUNCTION__, __LINE__);
            	if(strstr(p, "mpeg4cif"))
					strcpy(urlinfo->filename, "12");
				else
                	strcpy(urlinfo->filename, "11");
                free(aSubStr);
                free(full);
                return HI_SUCCESS;
            }
			else if(handleHKvsRtsp(p, urlinfo)){
                free(aSubStr);
                free(full);
                return HI_SUCCESS;
			}
			else if (strstr(p,"ROH/channel/")) {
				int chn = 0;
				if(p=strstr(aSubStr, "ROH/channel/"))
                {
                    chn = atoi(p+strlen("ROH/channel/"));
                }
				snprintf(urlinfo->filename,sizeof(urlinfo->filename),"%d",chn);
				free(aSubStr);
                free(full);
                return HI_SUCCESS;
			}

			else if(strstr(p, "H264MainStream"))
			{
				strcpy(urlinfo->filename, "11");
			}
			else if(strstr(p, "H264SubStream"))
			{
				strcpy(urlinfo->filename, "12");
			}

            else
            {
                strcpy(urlinfo->filename, p+1);
            }
        }
        else
        {
            strcpy(urlinfo->filename, "11");
        }
        
        ret = HI_SUCCESS;
        free(aSubStr);

    } 
    else 
    {
        /* try just to extract a file name */
        char *token = strtok(full, " \t\n");
        if (token) 
        {
            strcpy(urlinfo->filename, token);
            urlinfo->server[0] = '\0';
            ret = HI_SUCCESS;
        }
    }
    free(full);

    return ret;
}

#if 0
/*porting from oms project: parse_url 
url: DESCRIBE rtsp://10.85.180.240/sample_h264_100kbit.mp4 RTSP/1.0 */

HI_S32 RTSP_Parse_Url(HI_CHAR *url, HI_CHAR *file_name, HI_U32 *filenamelen, RTSP_REQ_METHOD_E *method, HI_U8 *meidatype)
{
	/* expects format '[rtsp://server[:port/]]filename' */
	HI_CHAR *full = NULL;
	int ret = HI_FAILURE;
	HI_CHAR* u8TempPtr = NULL;
	HI_CHAR *ptr1= NULL;
    HI_CHAR *ptr2=NULL;

    if(NULL == url || NULL == file_name || NULL ==filenamelen 
       || NULL ==method || NULL == meidatype)
    {
        return HI_ERR_RTSP_ILLEGALE_PARAM;
    }   

    memset(file_name,0,sizeof(HI_CHAR)*(*filenamelen));
    
	/* copy url */
	full = (HI_CHAR*)malloc(strlen(url) + 1);
	if(NULL == full)
	{
        return HI_ERR_RTSP_MALLOC;
	}
	else
    {	
	    strcpy(full, url);
    }

    u8TempPtr = strstr(full,RTSP_HDR_LIVE);
    /*the RTSP request is not for live or vod */
    if(NULL == u8TempPtr)
    {
        free(full);
        full = NULL;
        return HI_RTSP_PARSE_INVALID_URL_FOR_LIVE;
    }

    /*parse the content, to get filename ,method and media type*/
    /*the RTSP request is for live, then get filename
     .../livestream/filename?...*/
	else
	{
     	ptr1 = strstr(u8TempPtr,"/");
     	ptr2 = strstr(u8TempPtr,"?");
     
	     /*url like this : RTSP://ip:port/livestream*/
	     if(NULL ==  ptr1 && NULL == ptr2)
	     {
	        /*we defaultly set filename = 0,method = play, meidatype=video&audio&data */
	        strncpy(file_name,"11",1);
	        *method = RTSP_PLAY_METHOD;
	        *meidatype = MEDIA_TYPE_VIDEO|MEDIA_TYPE_AUDIO|MEDIA_TYPE_DATA;
	        
	     }
	     /*url like this : RTSP://ip:port/livestream/filename*/
	     else if(NULL != ptr1 && ptr2 == NULL)
	     {
	        /*we get filename  and defaultly method = play, meidatype=video&audio&data */
	        /*to do :如果是?.../live/ ,filename 就为空了 */
	        strncpy(file_name,ptr1+1,strlen(ptr1+1));
	        *method = RTSP_PLAY_METHOD;
	        *meidatype = MEDIA_TYPE_VIDEO|MEDIA_TYPE_AUDIO|MEDIA_TYPE_DATA;
	     }
	     /*url like this : RTSP://ip:port/livestream?action=play&media=viedo_audio_data*/
	     else if(NULL == ptr1 && ptr2 != NULL)
	     {
	        /*we defaultly set filename = 0, and get method,mediatype*/
	        strncpy(file_name,"0",1);

	        ret = RTSP_ParseParam(ptr2+1, method,meidatype);
	        if(HI_SUCCESS != ret)
	        {
	            free(full);
	            full = NULL;
	            return HI_FAILURE;
	        }
	        
	     }
	     /*url like this : RTSP://ip:port/livestream/filename?action=play&media=viedo_audio_data*/
	     else if(NULL != ptr1 && ptr2 != NULL)
	     {
	        /*we get filename  and  method, meidatype */
	        strncpy(file_name,ptr1+1,((ptr2-ptr1)-1) );
            
	        ret = RTSP_ParseParam(ptr2+1, method,meidatype);
	        if(HI_SUCCESS != ret)
	        {
	            free(full);
	            full = NULL;
	            return HI_FAILURE;
	        }
	     }
	}

	*filenamelen = strlen(file_name);
     
	free(full);
	full = NULL;
    
	return ret;

}

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

