/****************************************************************************
*              Copyright 2006 - , Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_rtsp_o_http_parse.c
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
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "hi_type.h"
#include "hi_log_def.h"
#include "list.h"
//#include "hi_adpt_interface.h"
#include "hi_mt_socket.h"
#include "hi_rtsp_o_http_parse.h"
#include "hi_rtp.h"
#include "hi_vs_media_comm.h"
#include "hi_msession_rtsp_o_http_error.h"
//#include "hi_log_app.h"
//#include "hi_debug_def.h"
//#include "hi_pthread_wait.h"




#define MEDIA_TYPE_VIDEO 0x01           /*媒体类型为视频类型时为0000 0001 */
#define MEDIA_TYPE_AUDIO 0x02           /*媒体类型为音频类型时为0000 0010 */
#define MEDIA_TYPE_DATA  0x04           /*媒体类型为其他数据类型时为0000 0100 */

RTSP_O_HTTP_TKN_S RTSP_O_HTTP_Status [] =
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
   {"HTTP Version Not Supported", 505},
   {"Extended Error:", 911},
   {0, HI_RTSP_O_HTTP_PARSE_INVALID_OPCODE}
};

HI_CHAR  *RTSP_O_HTTP_Invalid_Method_STR = "Invalid Method";

/* message header keywords */

HI_CHAR *RTSP_O_HTTP_Get_Status_Str( int code )
{
   RTSP_O_HTTP_TKN_S  *pTkn;
   
   for ( pTkn = RTSP_O_HTTP_Status; pTkn->opcode != HI_RTSP_O_HTTP_PARSE_INVALID_OPCODE; pTkn++ )
   {
      if ( pTkn->opcode == code )
      {
         return( pTkn->token );
      }
   }

   //WRITE_LOG_ERROR("Invalid status code.%d .\n", code);
   return( RTSP_O_HTTP_Invalid_Method_STR );
}

HI_VOID RTSP_O_HTTP_GetAudioFormatStr(MT_AUDIO_FORMAT_E enAudioFormat ,HI_CHAR * pAudioFormatStr,HI_S32 *s32Value)
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

HI_VOID RTSP_O_HTTP_GetVideoFormatStr(MT_VIDEO_FORMAT_E enVideoFormat ,HI_CHAR * pVideoFormatStr,HI_S32 *s32Value)
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
        *s32Value = RTP_PT_MPEGVIDEO;
    }
    

}

HI_S32 RTSP_O_HTTP_Get_SessId(HI_CHAR *pMsgStr, HI_CHAR *pszSessId)
{
    HI_CHAR * pTemp = NULL;
    /*获取Sessid信息*/
	if ((pTemp = strstr(pMsgStr, RTSP_O_HTTP_HDR_SESSION)) == NULL) 
    {
        return HI_RTSP_O_HTTP_PARSE_NO_SESSION;
    }
    else
    {
		if (sscanf(pTemp, "%*s %s", pszSessId) != 1) 
        {
            return HI_RTSP_O_HTTP_PARSE_INVALID_SESSION;
        }
    }

    return HI_SUCCESS;
}

HI_S32 RTSP_O_HTTP_Get_CSeq(HI_CHAR *pStr,HI_S32 *pCseq)
{

    HI_CHAR * pTemp = NULL;
    /*获取CSeq信息*/
    if ((pTemp = strstr(pStr, RTSP_O_HTTP_HDR_CSEQ)) == NULL) 
    {
        return HI_RTSP_O_HTTP_PARSE_INVALID_SESSIONCOOKIE;
    } 
    else 
    {
        if (sscanf(pTemp, "%*s %d", pCseq) != 1) 
        {
            return HI_RTSP_O_HTTP_PARSE_INVALID_SESSIONCOOKIE;
        }
    }
    return HI_SUCCESS;
}


HI_S32 RTSP_O_HTTP_Get_sessioncookie(HI_CHAR *pStr,HI_CHAR *pSessCookie)
{
    
    HI_CHAR * pTemp = NULL;
    /*获取CSeq信息*/
	if ((pTemp = strstr(pStr, RTSP_O_HTTP_HDR_sessioncookie)) == NULL) 
    {
        return HI_RTSP_O_HTTP_PARSE_INVALID_SESSIONCOOKIE;
	} 
    else 
    {
		if (sscanf(pTemp, "%*s %s", pSessCookie) != 1) 
        {
            return HI_RTSP_O_HTTP_PARSE_INVALID_SESSIONCOOKIE;
		}
	}
	printf("the session cookie is :%s \n",pSessCookie);
    return HI_SUCCESS;
    
}

HI_S32 RTSP_O_HTTP_Get_Method( HI_CHAR *pStr,HI_S32 *pmethodcode)
{

    HI_CHAR           method [32]= {0};
    HI_CHAR           object [256]= {0};
    HI_CHAR           ver [32]= {0};
    HI_S32            pcnt = 0;          /* parameter count */
//    HI_CHAR *         pos = NULL;
    RTSP_O_HTTP_REQ_METHOD_E methodcode = RTSP_O_HTTP_REQ_METHOD_BUTT;
    /*
    * Check for a valid method token and convert
    * it to a switch code.
    * Returns -2 if there are not 4 valid tokens in the request line.
    * Returns -1 if the Method token is invalid.
    * Otherwise, the method code is returned.
    */
    *method = *object = '\0';
	
    pcnt = sscanf( pStr, "%s %255s %31s\r\n", method,object, ver );
    if ( pcnt != 3 )
    {
        return HI_RTSP_O_HTTP_PARSE_INVALID;
    }
	printf("the method is :%s ver:%s\n",method,ver);
	if(0 == strcasecmp(method,"GET"))
	{	
		if(0 == strcasecmp(ver,"HTTP/1.1"))
		{
			methodcode = RTSP_O_HTTP_FIRST_GET_METHOD;
		}
		else if(0 == strcasecmp(ver,"HTTP/1.0"))
		{
			methodcode = RTSP_O_HTTP_GET_METHOD;
		}
		else
		{
			printf("not support protocl \n");
		}
	}
	else if( 0 == strcasecmp(method,"POST"))
	{
		methodcode = RTSP_O_HTTP_POST_METHOD;
	}
	else
	{
		printf("not support mehod in get \n");
	}
    *pmethodcode = methodcode;
    return HI_SUCCESS;
    
}


/*porting from rtsp_ref::valid_resp_msg  */
HI_S32 RTSP_O_HTTP_Valid_RespMsg( HI_CHAR *pStr, HI_U32 *pSeq_num, HI_U32 *pStatus)

{
   HI_CHAR        ver [32] = {0};
   HI_U32         stat = 0;;
   HI_S32         pcnt = 0;          /* parameter count */

   HI_CHAR           uncare[256] = {0};

   /* assuming "stat" may not be zero (probably faulty) */
   stat = 0;
   pcnt = sscanf( pStr, " %31s %d  %255s ", ver, &stat,  uncare );

   if ( strncasecmp ( ver, "HTTP/", 5 ) )
   {
      return( HI_RTSP_O_HTTP_PARSE_ISNOT_RESP);  /* not a response message */
   }

   /* confirm that at least the version, status code and sequence are there. */
   if ( pcnt < 3 || stat == 0 )
   {
      return( HI_RTSP_O_HTTP_PARSE_ISNOT_RESP );  /* not a response message */
   }

   *pStatus = stat;
   return( HI_RTSP_O_HTTP_PARSE_IS_RESP);
}

/*Authorization: W54723 mypassword\r\n*/
HI_S32 RTSP_O_HTTP_GetUserName(HI_CHAR *pHttpRecvBuff, HI_CHAR *pUsername,HI_U32* pu32Userlen,
                        HI_CHAR * pPassword,HI_U32* pu32Pswlen)
{
//    HI_CHAR *pTemp = NULL;
    
    if(NULL == pHttpRecvBuff || NULL == pUsername || NULL == pu32Userlen
       || NULL == pPassword|| NULL ==  pu32Pswlen)
    {
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_PARAM;
    }
    
    memset(pUsername,0,sizeof(HI_CHAR)*(*pu32Userlen));
    memset(pPassword,0,sizeof(HI_CHAR)*(*pu32Pswlen));

/*
    pTemp = strstr(pHttpRecvBuff,RTSP_O_HTTP_HDR_AUTHORIZATION);
    if(NULL != pTemp )
    {

        if(2 != sscanf(pTemp,"%*s %s %s",pUsername,pPassword))
        {
            return HI_RTSP_O_HTTP_PARSE_INVALID_AUTHORIZATION;
        }

        *pu32Userlen = strlen(pUsername);
        *pu32Pswlen = strlen(pPassword);
        
    }
    else
    {
        return HI_RTSP_O_HTTP_PARSE_NO_AUTHORIZATION;
    }
*/
	strcpy(pUsername,"ipcam");
	strcpy(pPassword,"w54723");
	*pu32Userlen = strlen("ipcam");
	*pu32Pswlen = strlen("w54723");
    return HI_SUCCESS;
}

/*先定位到User-Agent, 再定位到第一个空格, 就是: 后面, 然后已知搜索到行尾*/
    /*User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
/*to do :review this func*/
HI_S32 RTSP_O_HTTP_GetUserAgent(HI_CHAR *pHttpRecvBuff, HI_CHAR *pUserAgentBuff,HI_U32* pu32Bufflen)
{

    HI_CHAR *pParseTmp = NULL;
   
    if ( NULL == ( pParseTmp = strstr(pHttpRecvBuff, "User-Agent"))  )
    {
    	return HI_RTSP_O_HTTP_PARSE_NO_USERAGENT;
    }
	else
	{
		if (sscanf(pParseTmp, "%*s %[^\r]", pUserAgentBuff) != 1) 
        {
            return HI_RTSP_O_HTTP_PARSE_INVALID_USERAGENT;
		}
        *pu32Bufflen = strlen(pUserAgentBuff) ;
	}
    return HI_SUCCESS;
}

/*return chn number*/
HI_S32 RTSP_O_HTTP_Get_Chn(HI_CHAR* pChn)
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

HI_VOID RTSP_O_HTTP_GetPeerAddr(struct sockaddr_in* psaddr,HI_CHAR* pszCliIp,HI_U16 u16CliPort)
{
    //psaddr = (struct sockaddr_in*)psaddr;
    psaddr->sin_family = AF_INET;
    psaddr->sin_addr.s_addr = inet_addr(pszCliIp);
    psaddr->sin_port = htons(u16CliPort);
    memset(psaddr->sin_zero, '\0', 8);

}

HI_S32 RTSP_O_HTTP_ParseParam(HI_CHAR *param, RTSP_O_HTTP_REQ_METHOD_E *method, HI_U8 *meidatype)
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
                return HI_RTSP_O_HTTP_PARSE_INVALID_OPCODE;
            }
            
            if(0 == strcmp(methodstr,RTSP_O_HTTP_METHOD_PLAY))
            {
                *method =  RTSP_O_HTTP_PLAY_METHOD;
            }
            else if(0 == strcmp(methodstr,RTSP_O_HTTP_METHOD_PAUSE))
            {
                *method =  RTSP_O_HTTP_PAUSE_METHOD;
            }
            else if(0 == strcmp(methodstr,RTSP_O_HTTP_METHOD_REPLAY))
            {
                *method =  RTSP_O_HTTP_REPLAY_METHOD;
            }
            else if(0 == strcmp(methodstr,RTSP_O_HTTP_METHOD_STOP))
            {
                *method =  RTSP_O_HTTP_TEARDOWN_METHOD;
            }
            else
            {
                return HI_RTSP_O_HTTP_PARSE_INVALID_OPCODE;
            }
                
        }
		/*has no 'action=', set play action default*/
		else
		{
			*method=RTSP_O_HTTP_PLAY_METHOD;
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

HI_S32 RTSP_O_HTTP_GetFileName(const char *url, char *file_name)
{
    HI_CHAR *pTemp = NULL;
    if (strncmp(url, "rtsp://", 7) == 0) 
    {
        pTemp = strrchr(url,'/');
        if(pTemp != NULL)
        {
            pTemp += 1;
            strcpy(file_name,pTemp);
            return HI_SUCCESS;
        }
    }
    return HI_FAILURE;
}

/*porting from oms project: parse_url */
HI_S32 RTSP_O_HTTP_Parse_Url(const char *url, char *server, int *port, char *file_name)
{
	/* expects format '[rtsp://server[:port/]]filename' */

	int ret = HI_FAILURE;
	/* copy url */
	char *full = malloc(strlen(url) + 1);
    *port = 80;
	strcpy(full, url);
	if (strncmp(full, "rtsp://", 7) == 0) 
    {
		char *token;
		int has_port = 0;

		char *aSubStr = malloc(strlen(url) + 1);
		strcpy(aSubStr, &full[7]);
		if (strchr(aSubStr, '/')) 
        {
			int len = 0;
			unsigned short i = 0;
			char *end_ptr;
			end_ptr = strchr(aSubStr, '/');
			len = end_ptr - aSubStr;
			for (; (i < strlen(url)); i++)
			{
				aSubStr[i] = 0;
			}
			strncpy(aSubStr, &full[7], len);
		}
		if (strchr(aSubStr, ':'))
		{
			has_port = 1;
		}
		free(aSubStr);
        /*TODO ?*/
		token = strtok(&full[7], " :/\t\n");
		if (token) 
        {
			strcpy(server, token);
			if (has_port) 
            {
				char *port_str = strtok(&full[strlen(server) + 7 + 1], " /\t\n");
				if (port_str)
					*port = atol(port_str);
			} 
			/* don't require a file name */
			ret = HI_SUCCESS;
			token = strtok(NULL, " ");
			if (token)
				strcpy(file_name, token);
			else
				file_name[0] = '\0';
		}
	} 
    else 
    {
		/* try just to extract a file name */
		char *token = strtok(full, " \t\n");
		if (token) 
        {
			strcpy(file_name, token);
			server[0] = '\0';
			ret = HI_SUCCESS;
		}
	}
	free(full);
    
	return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

