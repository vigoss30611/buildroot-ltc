/******************************************************************************

  Copyright (C), 2007-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_mession_rtsp_o_http.c
  Version       : Initial Draft
  Author        : w54723
  Created       : 2008/1/30
  Description   : rtsp_o_http session protocal process for live and vod
  History       :
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>


#ifdef HI_PTHREAD_MNG
#include "hi_pthread_wait.h"
#endif

//#include "hi_debug_def.h"
//nclude "hi_log_app.h"
#include "ext_defs.h"

#include "hi_msession_rtsp_o_http.h"
#include "hi_rtsp_o_http_session.h"
#include "hi_msession_rtsp_o_http_error.h"
//#include "hi_msession_rtsp_o_http_lisnsvr.h"
#include "hi_rtsp_o_http_parse.h"
#include "hi_vod_error.h"
#include "hi_vod.h"
//#include "hi_version.h"
#include "hi_mtrans_error.h"
#include "base64.h"
#include <sys/types.h>          
#include <sys/socket.h>
#include "hi_msession_lisnsvr.h"

#include "cgiinterface.h"
#include "hi_log_def.h"

#define RTSP_O_HTTP_LIVESVR "RTSP_O_HTTP_LIVESVR"

/*rtsp_o_http server struct for live or vod*/
RTSP_O_HTTP_LIVE_SVR_S g_strurtsp_o_httpSvr;

#define  RTSP_O_HTTP_TRANS_TIMEVAL_SEC    10
#define  RTSP_O_HTTP_TRANS_TIMEVAL_USEC   0

extern RTSP_O_HTTP_LIVE_SESSION* HTTP_LIVE_FindRelateSess(HI_CHAR* SessCookie);

HI_S32 HI_RTSP_O_HTTP_Send(HI_SOCKET s32WritSock,HI_CHAR* pszBuff, HI_U32 u32DataLen)
{
    HI_S32 s32Ret = 0;
    HI_U32 u32RemSize =0;
    HI_S32 s32Size    = 0;
    HI_CHAR*  ps8BufferPos = NULL;
    fd_set write_fds;
    struct timeval TimeoutVal;  /* Timeout value */
    HI_S32 s32Errno = 0;

	//printf("the send msg is %s \n",pszBuff);
    memset(&TimeoutVal,0,sizeof(struct timeval));

    u32RemSize = u32DataLen;
    ps8BufferPos = pszBuff;
    while(u32RemSize > 0)
    {
        FD_ZERO(&write_fds);
        FD_SET(s32WritSock, &write_fds);
        TimeoutVal.tv_sec = RTSP_O_HTTP_TRANS_TIMEVAL_SEC;
        TimeoutVal.tv_usec = RTSP_O_HTTP_TRANS_TIMEVAL_USEC;
        /*judge if it can send */
        s32Ret = select(s32WritSock + 1, NULL, &write_fds, NULL, &TimeoutVal);
        if (s32Ret > 0)
        {
            if( FD_ISSET(s32WritSock, &write_fds))
            {
                s32Size = send(s32WritSock, ps8BufferPos, u32RemSize, 0);
                if (s32Size < 0)
                {
                    /*if it is not eagain error, means can not send*/
                    if (errno != EINTR && errno != EAGAIN)
                    {
                        s32Errno = errno;
                        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"rtsp_o_http response Send error:%X\n",strerror(s32Errno));
                        return HI_ERR_RTSP_O_HTTP_SEND_FAILED;
                    }

                    /*it is eagain error, means can try again*/
                    continue;
                }

                u32RemSize -= s32Size;
                ps8BufferPos += s32Size;
            }
            else
            {
                s32Errno = errno;
                HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"rtsp_o_http response Send error:fd not in fd_set\n");
                return HI_ERR_RTSP_O_HTTP_SEND_FAILED;
            }
        }
        /*select found over time or error happend*/
        else if( s32Ret == 0 )
        {
            s32Errno = errno;
            HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"rtsp_o_http response Send error: select overtime %d.%ds\n",
                        RTSP_O_HTTP_TRANS_TIMEVAL_SEC,RTSP_O_HTTP_TRANS_TIMEVAL_USEC);
            return HI_ERR_RTSP_O_HTTP_SEND_FAILED;
        }
        else if( s32Ret < 0 )
        {
            s32Errno = errno;
            HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"rtsp_o_http response Send error:%X\n",strerror(s32Errno));
            return HI_ERR_RTSP_O_HTTP_SEND_FAILED;
        }
        
    }

    return HI_SUCCESS;
}

HI_S32 RTSP_O_HTTP_Make_RespHdr(RTSP_O_HTTP_LIVE_SESSION *pSess, int err)
{
    HI_RTSP_O_HTTP_ClearSendBuff(pSess);
    
    char *pTmp = pSess->au8SendBuff;

    pTmp += sprintf(pTmp, "%s %d %s\r\n", RTSP_O_HTTP_VER_STR, err, RTSP_O_HTTP_Get_Status_Str( err ));
    pTmp += sprintf(pTmp, "Host: %s\r\n",pSess->au8HostIP);
    pTmp += sprintf(pTmp,"Connection: Keep-Alive\r\n");
   // pTmp += sprintf(pTmp,"CSeq: %d\r\n", pSess->last_recv_seq);
   // pTmp += sprintf(pTmp,"Server: Hisilicon %s\r\n",pSess->au8SvrVersion);

    return (strlen(pSess->au8SendBuff)) ;
}


/*TODO*/
HI_VOID RTSP_O_HTTP_Send_Reply(RTSP_O_HTTP_LIVE_SESSION* pSess, int err, char *addon , int simple)
{
    char* pTmp = NULL;
    pTmp = pSess->au8SendBuff;

    if (simple == 1)
    {
        memset(pTmp,0,sizeof(RTSP_O_HTTP_RESP_BUFFER));
        pTmp += RTSP_O_HTTP_Make_RespHdr(pSess, err);
    }
    
    if ( addon )
    {
        pTmp += sprintf(pTmp, "%s", addon );
    }

    if (simple)
        strcat( pSess->au8SendBuff, RTSP_O_HTTP_LRLF );

    pSess->readyToSend = HI_TRUE;
    return;
}

HI_VOID RTSP_O_HTTP_SendErrResp(HI_S32 s32LinkFd,HI_S32 rtsp_o_http_err_code)
{
    HI_S32 s32Ret = 0;
    HI_CHAR  OutBuff[1024] = {0};
    HI_CHAR *pTemp = NULL;
    
    /*如果前面任何一个步骤出现错误，则组织并返回返回错误信息并关闭socket*/
    pTemp = OutBuff;
    pTemp += sprintf(pTemp, "%s %d %s\r\n", RTSP_O_HTTP_VER_STR, rtsp_o_http_err_code,
                    RTSP_O_HTTP_Get_Status_Str( rtsp_o_http_err_code ));
    pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");
    pTemp += sprintf(pTemp,"\r\n");


    /*发送回应信息*/
    s32Ret = HI_RTSP_O_HTTP_Send(s32LinkFd,OutBuff,strlen(OutBuff));	/* bad request */
    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERR,"to send rtsp_o_http pause response %d byte thought sock %d failed\n",
                   strlen(OutBuff),s32LinkFd);
    }
    else
    {
        
        HI_LOG_DEBUG(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE, "Send Reply .S --> C >>>>>>\n"
                       "%s\n"
                       "<<<<<<\n",
                       OutBuff);
    }

    /*关闭该链接*/
    close(s32LinkFd);
    
}

HI_S32 RTSP_O_HTTP_CMD_MethodGet(RTSP_O_HTTP_LIVE_SESSION* pSess,RTSP_O_HTTP_REQ_METHOD_E* pMethod)
{
	RTSP_O_HTTP_REQ_METHOD_E method;
	HI_CHAR au8Method[64];
	HI_CHAR  au8Url[128];
	HI_CHAR  au8Ver[64];
	HI_S32 pcnt;
	//printf("recv msg == %s\n",pSess->au8RecvBuff);
	pcnt = sscanf( pSess->au8RecvBuff, "%s %255s %31s", au8Method,au8Url, au8Ver);
    if ( pcnt != 3 )
    {
        return HI_RTSP_O_HTTP_PARSE_INVALID;
    }
	if(strcmp(au8Method,"DESCRIBE") == 0)
	{
		method = RTSP_O_HTTP_DISCRIBLE_METHOD;
	}
	else if(strcmp(au8Method,"SETUP") == 0)
	{
		method = RTSP_O_HTTP_SETUP_METHOD;
	}
	else if(strcmp(au8Method,"PLAY") == 0)
	{
		method = RTSP_O_HTTP_PLAY_METHOD;
	}
	else if(strcmp(au8Method,"PAUSE") == 0)
	{
		method = RTSP_O_HTTP_PAUSE_METHOD;
	}
	else if(strcmp(au8Method,"TEARDOWN") == 0)
	{
		method = RTSP_O_HTTP_TEARDOWN_METHOD;
	}
	else if(strcmp(au8Method,"SET_PARAMETER") == 0)
	{
		method = RTSP_O_HTTP_SET_PARAM_METHOD;
	}
    else if(strcmp(au8Method,"OPTIONS") == 0)
	{
		method = RTSP_O_HTTP_OPTIONS_METHOD;
	}
	else
	{
		printf("the method %s not support \n",au8Method);
		return HI_FAILURE;
	}
	printf("the method is :%s %d \n",au8Method,method);
	*pMethod = method;
	return HI_SUCCESS;
}

#define TRACK_ID_VIDEO 0
#define TRACK_ID_AUDIO 1

/*
DESCRIBE rtsp://192.168.1.102:80/2.mov RTSP/1.0
CSeq: 1
Accept: application/sdp
Bandwidth: 384000
Accept-Language: hr-HR
User-Agent: QuickTime/7.4.5 (qtver=7.4.5;os=Windows NT 5.1Service Pack 3)


RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 1

Last-Modified: Mon, 07 Mar 2005 08:02:10 GMT

Cache-Control: must-revalidate

Content-length: 357

Date: Tue, 20 Jan 2009 12:23:29 GMT

Expires: Tue, 20 Jan 2009 12:23:29 GMT

Content-Type: application/sdp

x-Accept-Retransmit: our-retransmit

x-Accept-Dynamic-Rate: 1

Content-Base: rtsp://192.168.1.102:80/2.mov/



v=0

o=StreamingServer 3441443066 1110182530000 IN IP4 192.168.1.102

s=\2.mov

u=http:///

e=admin@

c=IN IP4 0.0.0.0

b=AS:94

t=0 0

a=control:*

a=range:npt=0-  70.00000

m=video 0 RTP/AVP 96

b=AS:79

a=rtpmap:96 X-SV3V-ES/90000

a=control:trackID=3

m=audio 0 RTP/AVP 97

b=AS:14

a=rtpmap:97 X-QDM/22050/2

a=control:trackID=4

a=x-bufferdelay:4.97
*/


HI_S32 RTSP_O_HTTP_Handle_Describle(RTSP_O_HTTP_LIVE_SESSION* pSess)
{
    HI_CHAR url[256];
    HI_S32 ret = 0;
    HI_S32 Cseq = 0;
    HI_CHAR* pTemp = NULL;
    if (!sscanf(pSess->au8RecvBuff, "%*s %254s ", url)) 
    {
    	printf("DEBUG %s,%d 400 err\n",__FILE__,__LINE__);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
        return HI_SUCCESS;
    }

    if(pSess == NULL)
    {
        printf("the pSess is null \n");
        return HI_FAILURE;
    }
    
    /*get User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
    HI_U32 u32UserAgentLen = RTSP_O_HTTP_MAX_VERSION_LEN;
    ret = RTSP_O_HTTP_GetUserAgent(pSess->au8RecvBuff, pSess->au8UserAgent,&u32UserAgentLen);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "error user-agent: %d\n",ret);
    }

    /*获取CSeq信息*/
    ret = RTSP_O_HTTP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
		printf("DEBUG %s,%d 400 err\n",__FILE__,__LINE__);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_SUCCESS;
    }
    else
    {
        pSess->last_recv_seq= Cseq;
    }
    
    /*获取用户名及密码*/
    HI_CHAR au8UserName[256] = {0};            //用户名
    HI_CHAR au8PassWord[256] = {0};            //密码
    HI_U32 u32UserLen = 256;
    HI_U32 u32PassLen = 256;


    /*
    ret = RTSP_O_HTTP_GetUserName(pSess->au8RecvBuff,au8UserName,&u32UserLen, au8PassWord,&u32PassLen);
    if (ret != HI_SUCCESS)
    {
    HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid Authorization: %s\n", url);
    RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
    return HI_SUCCESS;
    }
    */

    ret = RTSP_O_HTTP_GetFileName(url, pSess->au8FileName);
    if(ret != HI_SUCCESS)
    {
        printf("%s, RTSP_O_HTTP_GetFileName error!\n",__FUNCTION__);
        sprintf(pSess->au8FileName,"%d",0);
        return HI_FAILURE;
    }
    /*向vod请求媒体信息*/
    VOD_DESCRIBE_MSG_S  stDesReqInfo={0}; 
    VOD_ReDESCRIBE_MSG_S stDesRespInfo={0};
    memset(&stDesReqInfo,0,sizeof(VOD_DESCRIBE_MSG_S));
    memset(&stDesRespInfo,0,sizeof(VOD_ReDESCRIBE_MSG_S));
    strcpy(stDesReqInfo.au8MediaFile,pSess->au8FileName);
    //strncpy(stDesReqInfo.au8MediaFile,au8FileInfo,sizeof(au8FileInfo));
    //strncpy(stDesReqInfo.au8UserName,au8UserName,u32UserLen);
    //strncpy(stDesReqInfo.au8PassWord,au8PassWord,u32UserLen);
    strncpy(stDesReqInfo.au8ClientVersion,pSess->au8UserAgent,u32UserAgentLen);

	
	if (NULL == strstr(url,"playback/")) 
    	stDesReqInfo.bLive = HI_TRUE;
	else
		stDesReqInfo.bLive = HI_FALSE;
   



    ret = HI_VOD_DESCRIBE(&stDesReqInfo, &stDesRespInfo);

    DEBUGPrtVODDes(&stDesReqInfo, &stDesRespInfo);

    if(ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod describe failed %X(client %s)\n",
               ret,pSess->au8CliIP);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        return ret;
    }
    /*rtsp_o_http会话结构保存server的版本信息*/
    else
    {
        strncpy(pSess->au8SvrVersion,stDesRespInfo.au8BoardVersion,RTSP_O_HTTP_MAX_VERSION_LEN-1);
        pSess->au8SvrVersion[RTSP_O_HTTP_MAX_VERSION_LEN-1] = '\0';
    }


    HI_CHAR content[1024];
    HI_S32 s32UrlLen = strlen(url);
    HI_CHAR* pTempContent = content;

    HI_CHAR* pSource = strrchr(url,'/');

    HI_S32 bandSum=0;
    HI_S32 bandVideo=0;
    HI_U32 bandAudio=0;

    bandVideo = stDesRespInfo.u32Bitrate/1000;
    bandAudio= stDesRespInfo.u32AudioSampleRate/1000;
    bandSum = (bandVideo + bandAudio);


    HI_S32 namelen = 0;
    struct sockaddr_in addr;
    namelen = sizeof(struct sockaddr);
    getsockname(pSess->s32GetConnSock, (struct sockaddr *)&addr, &namelen);
    strcpy(pSess->au8HostIP, inet_ntoa( addr.sin_addr ));

    pTempContent += sprintf(pTempContent,"v=0\r\n");
    pTempContent += sprintf(pTempContent,"o=StreamingServer 3441443066 1110182530000 IN IP4 %s\r\n",pSess->au8HostIP);
    //pTempContent += sprintf(pTempContent,"s=\\iphone.mov\r\n");
    pTempContent += sprintf(pTempContent,"s=\r\ne=NONE\r\n");

    pTempContent += sprintf(pTempContent,"c=IN IP4 0.0.0.0\r\n");
    pTempContent += sprintf(pTempContent,"t=0 0\r\n");

    /*视频媒体描述*/    
    if(MT_VIDEO_FORMAT_MJPEG == stDesRespInfo.enVideoType)
    {
        pTempContent += sprintf(pTempContent,"m=video 0 RTP/AVP 26\r\n");
    }
    else//这里有点问题，不是MJPEG的码流都按H264处理，以后按需求修改。
    {
        pTempContent += sprintf(pTempContent,"m=video 0 RTP/AVP 96\r\n");
    }

    pTempContent += sprintf(pTempContent,"b=AS:%d\r\n",bandVideo); 
    pTempContent += sprintf(pTempContent,"a=control:trackID=%d\r\n",RTSP_TRACKID_VIDEO);
    if(MT_VIDEO_FORMAT_MJPEG == stDesRespInfo.enVideoType)
    {
        pTempContent += sprintf(pTempContent,"a=rtpmap:26 JPEG/90000\r\n");
    }
    else
    {
        pTempContent += sprintf(pTempContent,"a=rtpmap:96 H264/90000\r\n");
    }

    if(MT_VIDEO_FORMAT_H264 == stDesRespInfo.enVideoType)
    {
        pTempContent += sprintf(pTempContent,"a=fmtp:96 profile-level-id=%s; packetization-mode=1; sprop-parameter-sets=%s\r\n", 
            stDesRespInfo.profile_level_id,stDesRespInfo.au8VideoDataInfo);

        pTempContent += sprintf(pTempContent,"a=framesize:96 %d-%d\r\n",
            stDesRespInfo.u32VideoPicWidth,stDesRespInfo.u32VideoPicHeight);
    }

    /*音频媒体描述*/
    HI_S32 s32Chn = 0;
    s32Chn = atoi(pSess->au8FileName);
    ret = HI_BufAbstract_GetAudioEnable(s32Chn);
    if(ret)    
    {
        pTempContent += sprintf(pTempContent,"m=audio 0 RTP/AVP 8\r\n");	 
        pTempContent += sprintf(pTempContent,"a=control:trackID=%d\r\n",RTSP_TRACKID_AUDIO);
        pTempContent += sprintf(pTempContent,"a=rtpmap:8 PCMA/8000\r\n");
    }

    pTemp = pSess->au8SendBuff;
    pTemp += sprintf(pTemp,"RTSP/1.0 200 OK\r\n");
    pTemp += sprintf(pTemp,"CSeq: %d\r\n",Cseq);
    //pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");
    pTemp += sprintf(pTemp,"Content-Type: application/sdp\r\n");
    pTemp += sprintf(pTemp,"Content-length: %d\r\n",strlen(content));
    //pTemp += sprintf(pTemp,"Content-Base: %s\r\n",url);
    pTemp += sprintf(pTemp,"\r\n");

    strcat(pTemp,content);
    pSess->readyToSend = HI_TRUE;

    return HI_SUCCESS;
}
/*
SETUP rtsp://192.168.1.102:80/2.mov/trackID=3 RTSP/1.0
CSeq: 2
Transport: RTP/AVP/TCP;unicast
x-dynamic-rate: 1
x-transport-options: late-tolerance=2.384000
User-Agent: QuickTime/7.4.5 (qtver=7.4.5;os=Windows NT 5.1Service Pack 3)
Accept-Language: hr-HR

RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 2

Last-Modified: Mon, 07 Mar 2005 08:02:10 GMT

Cache-Control: must-revalidate

Session: 133771051426407

Date: Tue, 20 Jan 2009 12:23:30 GMT

Expires: Tue, 20 Jan 2009 12:23:30 GMT

Transport: RTP/AVP/TCP;unicast;interleaved=0-1;ssrc=000026E9

x-Transport-Options: late-tolerance=2.384000

x-Dynamic-Rate: 1;rtt=109
*/
HI_S32 RTSP_O_HTTP_Handle_Setup(RTSP_O_HTTP_LIVE_SESSION* pSess)
{
    HI_CHAR url[256];
    HI_S32 ret = 0;
    HI_S32 Cseq = 0;
    HI_CHAR* pTemp = NULL;
    HI_S32 s32TrackId = TRACK_ID_VIDEO;
    if (!sscanf(pSess->au8RecvBuff, "%*s %254s ", url)) 
    {
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
        return HI_SUCCESS;
    }
    pTemp = strstr(url,"trackID");
    if(pTemp != NULL)
    {
        sscanf(pTemp,"trackID=%d",&s32TrackId);
        printf("RTSP_O_HTTP_Handle_Setup tackId;%d\n",s32TrackId);
    }
    /*get User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
    HI_U32 u32UserAgentLen = RTSP_O_HTTP_MAX_VERSION_LEN;
    ret = RTSP_O_HTTP_GetUserAgent(pSess->au8RecvBuff, pSess->au8UserAgent,&u32UserAgentLen);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "error user-agent: %d\n",ret);
    }

    /*获取CSeq信息*/
    ret = RTSP_O_HTTP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_SUCCESS;
    }
    else
    {
        pSess->last_recv_seq= Cseq;
    }

    #if 0
    HI_S32 namelen = 0;
    struct sockaddr_in addr;

    namelen = sizeof(struct sockaddr_in);
    getpeername(pSess->s32GetConnSock, (struct sockaddr *)&addr, &namelen);


    pSess->s32CliHttpPort = (unsigned short)ntohs(addr.sin_port);
    strcpy(pSess->au8CliIP, inet_ntoa( addr.sin_addr ));
    #endif
    

#if 0
    /*获取用户名及密码*/
    HI_CHAR au8UserName[256] = {0};            //用户名
    HI_CHAR au8PassWord[256] = {0};            //密码
    HI_U32 u32UserLen = 256;
    HI_U32 u32PassLen = 256;


    ret = RTSP_O_HTTP_GetUserName(pSess->au8RecvBuff,au8UserName,&u32UserLen, au8PassWord,&u32PassLen);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid Authorization: %s\n", url);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_SUCCESS;
    }
#endif

    HI_U8 u8ReqMediaType;
    VOD_SETUP_MSG_S  stSetupReqInfo; 
    VOD_ReSETUP_MSG_S stSetupRespInfo;
    memset(&stSetupReqInfo,0,sizeof(VOD_SETUP_MSG_S));
    memset(&stSetupRespInfo,0,sizeof(VOD_ReSETUP_MSG_S));
    stSetupReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP;

    if(s32TrackId == TRACK_ID_VIDEO)
    {
        u8ReqMediaType = 0x01;
    }
    else 
    {
        u8ReqMediaType = 0x02;
    }
    pSess->sessMediaType = pSess->sessMediaType | u8ReqMediaType;
    printf("the setup media type is :%d\n",pSess->sessMediaType);
    /*to do:如何cpy字符串*/
    strncpy(stSetupReqInfo.aszSessID,pSess->aszSessID,VOD_MAX_STRING_LENGTH);
    stSetupReqInfo.u8SetupMeidaType = u8ReqMediaType;
    strncpy(stSetupReqInfo.au8ClientIp,pSess->au8CliIP,sizeof(stSetupReqInfo.au8ClientIp));
    strcpy(stSetupReqInfo.au8MediaFile,pSess->au8FileName);
    //strncpy(stSetupReqInfo.au8UserName,au8UserName,VOD_MAX_STRING_LENGTH);
    //strncpy(stSetupReqInfo.au8PassWord,au8PassWord,VOD_MAX_STRING_LENGTH);
    strncpy(stSetupReqInfo.au8ClientVersion,pSess->au8UserAgent,VOD_MAX_STRING_LENGTH);

    /*解析client端的传输方式*/
    HI_U32 u32TransListLen = VOD_MAX_TRANSTYPE_NUM;
    stSetupReqInfo.astCliSupptTransList[0].enMediaTransMode = MTRANS_MODE_TCP_ITLV;
    stSetupReqInfo.astCliSupptTransList[0].enPackType = PACK_TYPE_RTSP_ITLV;
    stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.s32SessSock = pSess->s32GetConnSock;
    //stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliMediaChnId = 0;
    //stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliAdaptChnId = 1;
    stSetupReqInfo.astCliSupptTransList[0].bValidFlag = HI_TRUE;

    pTemp = strstr(pSess->au8RecvBuff, "interleaved=");
    if(!pTemp)
    {
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
        return -1;
    }

    ret = sscanf(pTemp, "interleaved=%u-%u", 
    &stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliMediaChnId,
    &stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliAdaptChnId);
    if(ret != 2)
    {
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
        return -2;
    }

    HI_U32 SSRC;
    SSRC = (HI_U32)random();
    stSetupReqInfo.u32Ssrc = SSRC;
#if 0
    ret =  RTSP_O_HTTP_TransInfo_Handle(pSess,stSetupReqInfo.astCliSupptTransList,&u32TransListLen);
    if(ret != HI_SUCCESS)
    {
    HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X(client %s)\n",
    ret,pSess->au8CliIP);
    RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_SERVICE_UNAVAILIABLE, NULL, 1);
    return ret;
    }
#endif

    //if(s32TrackId == TRACK_ID_AUDIO )
    if( NULL != strstr(pSess->au8RecvBuff,"Session"))
    {
        stSetupReqInfo.resv1 = 1;
    }

    ret = HI_VOD_SetUp(&stSetupReqInfo,&stSetupRespInfo);
    DEBUGPrtVODSetup(&stSetupReqInfo,&stSetupRespInfo);
    if(ret != HI_SUCCESS)
    {
        if(HI_ERR_VOD_ILLEGAL_USER == ret )
        {

            //HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X:illegal user %s(%s %s)\n",
            //       ret,pSess->au8CliIP,au8UserName,au8PassWord);
            RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_UNAUTHORIZED, NULL, 1);
        }
        else if(HI_ERR_VOD_OVER_CONN_NUM == ret )
        {
            HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X:over max usernumber for cli %s\n",
               ret,pSess->au8CliIP);
            RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_OVER_SUPPORTED_CONNECTION, NULL, 1);
        }
        else
        {
            HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X for %s\n",
               ret,pSess->au8CliIP);
            RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        }
        return ret;
    }
    printf("the vod setup call after \n");

    pSess->bIsIntlvTrans = HI_TRUE;
    pSess->CBonTransItlvMeidaDatas = stSetupRespInfo.stTransInfo.stTcpItlvTransInfo.pProcMediaTrans;
    pSess->u32TransHandle = stSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32TransHandle;

    /*组织回复消息*/
    pTemp = pSess->au8SendBuff;

    pTemp += sprintf(pTemp,"RTSP/1.0 200 OK\r\n");
    pTemp += sprintf(pTemp,"CSeq: %d\r\n",Cseq);
    pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");
    pTemp += sprintf(pTemp,"Session:: %s\r\n",pSess->aszSessID);
    if(s32TrackId == TRACK_ID_VIDEO)
    {
        pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;interleaved=%u-%u;ssrc=%x\r\n",
            stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliMediaChnId,
            stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliAdaptChnId,
            SSRC);
    }
    else
    {
        pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;interleaved=%u-%u;ssrc=%x\r\n",
            stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliMediaChnId,
            stSetupReqInfo.astCliSupptTransList[0].stTcpItlvTransInfo.u32ItlvCliAdaptChnId,
            SSRC);
    }

    pTemp += sprintf(pTemp,"\r\n");

    pSess->readyToSend = HI_TRUE;

    return HI_SUCCESS;
	
}
/*
PLAY rtsp://192.168.1.102:80/2.mov RTSP/1.0
CSeq: 4
Range: npt=0.000000-70.000000
x-prebuffer: maxtime=2.000000
x-transport-options: late-tolerance=10
Session: 133771051426407
User-Agent: QuickTime/7.4.5 (qtver=7.4.5;os=Windows NT 5.1Service Pack 3)

RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 4

Session: 133771051426407

Range: npt=0.00000-70.00000

RTP-Info: url=rtsp://192.168.1.102:80/2.mov/trackID=3;seq=23282;rtptime=16827,url=rtsp://192.168.1.102:80/2.mov/trackID=4;seq=492;rtptime=2995
*/
HI_S32 RTSP_O_HTTP_Handle_Play(RTSP_O_HTTP_LIVE_SESSION* pSess)
{
	HI_S32 ret;
	HI_S32 Cseq;
	HI_CHAR url[256];
	HI_CHAR range[64];
	HI_CHAR* pTemp = NULL;
	HI_CHAR* pTemp2 = NULL;

	if(pSess == NULL)
	{
		printf("the session is null in play \n");
		return HI_FAILURE;
	}
	if (!sscanf(pSess->au8RecvBuff, "%*s %254s ", url)) 
    {
		RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
		return HI_SUCCESS;
	}
	
	 /*获取CSeq信息*/
    ret = RTSP_O_HTTP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_SUCCESS;
    }
    else
    {
        pSess->last_recv_seq= Cseq;
    }
	pTemp2= strstr(pSess->au8RecvBuff,"Range");
	sscanf(pTemp2,"Range: %s\r\n",range);
	//printf("the sceq is:%d range is:%s ver:%s sessId:%s\n",Cseq,range,pSess->au8SvrVersion,pSess->aszSessID);
	
	VOD_PLAY_MSG_S  stPlayReqInfo; 
    VOD_RePLAY_MSG_S stPlayRespInfo;
    memset(&stPlayReqInfo,0,sizeof(VOD_PLAY_MSG_S));
    memset(&stPlayRespInfo,0,sizeof(VOD_RePLAY_MSG_S));
    stPlayReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP ;
    strncpy(stPlayReqInfo.aszSessID,pSess->aszSessID,sizeof(pSess->aszSessID));
    stPlayReqInfo.u8PlayMediaType = pSess->sessMediaType;
    stPlayReqInfo.u32Duration = 0;
    stPlayReqInfo.u32StartTime = 0;
    ret = HI_VOD_Play(&stPlayReqInfo, &stPlayRespInfo);
    DEBUGPrtVODPlay(&stPlayReqInfo,&stPlayRespInfo);
    if(ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod play failed %X(client %s)\n",
                   ret,pSess->au8CliIP);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        return ret;
    }
	printf("vod play call after");
	HI_U64 timeStamp;
	if(stPlayRespInfo.audio_pts > 0)
	{
		timeStamp = stPlayRespInfo.audio_pts;
	}
	else
	{
		timeStamp = stPlayRespInfo.video_pts;
	}
	
	pTemp = pSess->au8SendBuff;
	pTemp += sprintf(pTemp,"RTSP/1.0 200 OK\r\n");
	pTemp += sprintf(pTemp,"CSeq: %d\r\n",Cseq);
	pTemp += sprintf(pTemp,"Session: %s\r\n",pSess->aszSessID);
	pTemp += sprintf(pTemp,"Range: npt=now-\r\n");

	if((pSess->sessMediaType == 1)||(pSess->sessMediaType == 3))
	{
		//pTemp += sprintf(pTemp,"RTP-Info: url=%s/trackID=3;seq=0;rtptime=%u",url,(HI_U32)((stPlayRespInfo.video_pts/1000)*90));
		pTemp += sprintf(pTemp,"RTP-Info: url=%s/trackID=0;seq=0;rtptime=0",url);
	}
	printf("the play session media type is:%d\n",pSess->sessMediaType);
	if((pSess->sessMediaType == 2)||(pSess->sessMediaType == 3))
	{
		//pTemp += sprintf(pTemp,",url=%s/trackID=4;seq=0;rtptime=%u",url,(HI_U32)((stPlayRespInfo.audio_pts/1000)*8));
		pTemp += sprintf(pTemp,",url=%s/trackID=1;seq=0;rtptime=%u",url);
	}

	pTemp += sprintf(pTemp,"\r\n\r\n");
	
	//printf("the play resp :%s \n",pSess->au8SendBuff);
	pSess->readyToSend = HI_TRUE;
    /*rtsp_o_http has complete apply resouce to vod server, and enter into 'play'state*/
    /*to do:是否应该在这里设置*/
    pSess->enSessState = RTSP_O_HTTP_SESSION_STATE_PLAY;
	
    return HI_SUCCESS;
}
HI_S32 RTSP_O_HTTP_Handle_Teardown(RTSP_O_HTTP_LIVE_SESSION* pSess)
{
	HI_S32 s32Ret = 0;
    VOD_TEAWDOWN_MSG_S stVodCloseMsg;
    VOD_ReTEAWDOWN_MSG_S stVodReCloseMsg;
    memset(&stVodCloseMsg,0,sizeof(VOD_TEAWDOWN_MSG_S));
    memset(&stVodReCloseMsg,0,sizeof(VOD_ReTEAWDOWN_MSG_S));

    if(NULL == pSess)
    {
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_PARAM;
    }

    /*consist vod teardown msg*/
    stVodCloseMsg.enSessType = SESS_PROTOCAL_TYPE_RTSP ;
    strncpy(stVodCloseMsg.aszSessID,pSess->aszSessID,strlen(pSess->aszSessID));
    DEBUGPrtVODTeawdown(&stVodCloseMsg,&stVodReCloseMsg);

    s32Ret = HI_VOD_Teardown(&stVodCloseMsg,&stVodReCloseMsg);
    if(HI_SUCCESS != s32Ret)
    {
        /*vod has not creat corresponding vod session or 
         the session has colsed*/
        if(HI_ERR_VOD_INVALID_SESSION == s32Ret)
        {
            return HI_SUCCESS;
        }
        return s32Ret;
    }

    DEBUGPrtVODTeawdown(&stVodCloseMsg,&stVodReCloseMsg);

    return HI_SUCCESS;
}

HI_S32 RTSP_O_HTTP_Handle_Options(RTSP_O_HTTP_LIVE_SESSION* pSess)
{
    char *p;
	HI_S32 s32Ret = 0;
    HI_S32 Cseq = 0;
    char outbuf[512] = {0};

    s32Ret = RTSP_O_HTTP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if(s32Ret != HI_SUCCESS)
    {
        printf("%s RTSP_O_HTTP_Get_CSeq failed!\n",__FUNCTION__);
        return s32Ret;
        
    }
    pSess->last_recv_seq= Cseq;

    p = pSess->au8SendBuff;
    
    p += sprintf(p,"RTSP/1.0 200 OK\r\n");
    p += sprintf(p,"CSeq: %d\r\n",Cseq);
    p += sprintf(p, "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, SET_PARAMETER\r\n");  
    p += sprintf(p,"\r\n");
    pSess->readyToSend = HI_TRUE;

    return HI_SUCCESS;
}


/* pc  ->>> svr
GET http://ip:port/getstream/0?action=pause&media=audio_video http/1.1\r\n
user-Agent:hisilicon-Client\r\n
Content-Type: application/octet-stream\r\n
Content-Length:89\r\n
Authorization:Basic adsfasfsa\r\n
\r\n
Session: 36753562
CSeq: 2

svr ->>> pc
RTSP_O_HTTP/1.1 200 OK\r\n
Cache-Control: no-cache\r\n
Server: Hisilicon Ipcam\r\n
Content-Type: application/octet-stream\r\n
Content-Length: 70\r\n
Connection:Keep-Alive\r\n
\r\n
Session: 36753562
CSeq: 2
*/
//HI_VOID RTSP_O_HTTP_Handle_Pause(HI_S32 s32LinkFd, HI_CHAR* pMsgBuffAddr,HI_U32 u32MsgLen)
HI_S32 RTSP_O_HTTP_Handle_Pause(RTSP_O_HTTP_LIVE_SESSION* pSess)
{
    HI_S32 s32Ret = HI_SUCCESS;
    RTSP_O_HTTP_URL url = {0};
    int rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_OK;
    HI_CHAR   OutBuff[512] = {0};
    HI_CHAR   content[512] = {0};
    HI_CHAR *pTemp = NULL;
    HI_CHAR *pTemp2 = NULL;
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH] = {0};
    HI_CHAR aszSvrVersion[RTSP_O_HTTP_MAX_VERSION_LEN] = {0}; 
    HI_U8 u8PauseMediasType = 0;
    HI_S32 Cseq = 0; 
    RTSP_O_HTTP_REQ_METHOD_E method = RTSP_O_HTTP_REQ_METHOD_BUTT;

    HI_LOG_DEBUG(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C --> S >>>>>>\n"
                   "%s\n"
                   "<<<<<<\n" ,
                   pSess->au8RecvBuff);
                   
   #if 0
    /*获取CSeq信息*/
    s32Ret = RTSP_O_HTTP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if( HI_SUCCESS != s32Ret)
    {
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(pSess->s32SessSock,rtsp_o_http_err_code);
        return HI_FAILURE;
    }
    
    /*获取sessionid*/
    s32Ret = RTSP_O_HTTP_Get_SessId(pSess->au8RecvBuff,aszSessID);
    if(HI_SUCCESS != s32Ret)
    {
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(pSess->s32SessSock,rtsp_o_http_err_code);
        return HI_FAILURE;
    }
	#endif
    /*如果前面解析都无错，则向vod请求暂停*/
    VOD_PAUSE_MSG_S stVodPauseMsg;
    VOD_RePAUSE_MSG_S stVodRePauseMsg;
    memset(&stVodPauseMsg,0,sizeof(VOD_PAUSE_MSG_S));
    memset(&stVodRePauseMsg,0,sizeof(VOD_RePAUSE_MSG_S));

    /*consist vod pause msg*/
    stVodPauseMsg.enSessType = SESS_PROTOCAL_TYPE_RTSP;
    strncpy(stVodPauseMsg.aszSessID,pSess->aszSessID,VOD_MAX_STRING_LENGTH);
    stVodPauseMsg.u32PauseTime = 0;     /*this attribution is invalid for live*/
   // stVodPauseMsg.u8PauseMediasType = u8PauseMediasType;
   stVodPauseMsg.u8PauseMediasType = pSess->sessMediaType;

    s32Ret = HI_VOD_Pause(&stVodPauseMsg,&stVodRePauseMsg);
    DEBUGPrtVODPause(&stVodPauseMsg,&stVodRePauseMsg);
    if(HI_SUCCESS != s32Ret)
    {
    	printf("HI_VOD_Pause failed:%x\n",s32Ret);
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(pSess->s32GetConnSock,rtsp_o_http_err_code);
        return HI_FAILURE;
    }

    s32Ret = HI_VOD_GetVersion(aszSvrVersion,RTSP_O_HTTP_MAX_VERSION_LEN);
    if(s32Ret != HI_SUCCESS)
    {
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(pSess->s32GetConnSock,rtsp_o_http_err_code);
        return HI_FAILURE;
    }

    /*consist pause response*/
    pTemp = pSess->au8SendBuff;
    pTemp += sprintf(pTemp, "RTSP/1.0 200 OK\r\n");
    pTemp += sprintf(pTemp,"Server: %s\r\n",aszSvrVersion);
    pTemp += sprintf(pTemp,"Session: %s\r\n",aszSessID);
	pTemp += sprintf(pTemp,"Cseq: %d\r\n",Cseq);

	pTemp += sprintf(pTemp,"\r\n");
 
    pSess->readyToSend = HI_TRUE;
    /*关闭该链接*/
    //close(s32LinkFd);
    return HI_SUCCESS;
}

#if 0
/*
C --> S
    GET http://ip:port/0/play=video&audio http/1.1\r\n
    User-Agent:hisilicon-Client\r\n
    Content-Type: application/octet-stream\r\n
    Content-Length:89\r\n
    Connection:Keep-Alive\r\n
    Cache-Control:no-cache\r\n
    Authorization:Basic adsfasfsa\r\n
    \r\n
    CSeq: 1
    Transport: RTP/AVP;unicast;client_port=4588-4589,RTP/AVP/TCP;unicast\r\n
S --> C
    RTSP_O_HTTP/1.1 200 OK\r\n
    Server: Hisilicon Ipcam\r\n
    Content-Type: application/octet-stream\r\n
    Content-Length: 70\r\n
    Connection:Keep-Alive\r\n
    \r\n
    Session: 36753562
    CSeq: 2
    m=video 96 H264/90000
    m=audio 97 G726/8000/1
    Transport: RTP/AVP;unicast;client_port=4588-4589;server_port=6256-6257
1) 根据用户的URI, 向VOD服务请求获取媒体信息，HI_VOD_DESCRIBE()方法
2) 解析用户的传输参数信息，向VOD服务请求按相应传输模式建立媒体会话
3) 按用户url中指定的媒体类型，向VOD服务请求播放相应的媒体数据
4) 按协议规定,组织回应信息,
3) 发送回复
*/
HI_S32 RTSP_O_HTTP_Handle_Play_bak(RTSP_O_HTTP_LIVE_SESSION *pSess )
{
    
    HI_S32   ret = 0;
    RTSP_O_HTTP_URL url;
    RTSP_O_HTTP_REQ_METHOD_E enHttpReqMethod = RTSP_O_HTTP_REQ_METHOD_BUTT;
    HI_U8  u8ReqMediaType = 0;
    HI_CHAR au8FileInfo[125] = {0};
    HI_S32 Cseq = 0;
    HI_U32 filenamelen = 125;
    char*   pTemp = NULL;
    char    sdpMsg[512] = {0};

    HI_S32 s32VideoRTPPt = 0;
    HI_S32 s32AudioRTPPt = 0;
    HI_S32 s32AudioSampRate = 8000;
    HI_S32 s32AudioChannNum = 1;
    HI_CHAR  aszAudioFormatStr[32] = "G726";
    HI_CHAR  aszVideoFormatStr[32] = "H264";
    
	if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
    {
		RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
		return HI_SUCCESS;
	}

    /*解析url,获取请求的方法，请求的媒体类型，请求的媒体文件名*/
    ret = RTSP_O_HTTP_Parse_Url(url, au8FileInfo,&filenamelen,&enHttpReqMethod,&u8ReqMediaType);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_SUCCESS;
    }

    //printf("parse url %X: fllename =%s filelen = %d method = %d media = %d\n",
    //       ret,au8FileInfo,filenamelen,enHttpReqMethod,u8ReqMediaType);

    /*get User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
    HI_U32 u32UserAgentLen = RTSP_O_HTTP_MAX_VERSION_LEN;
    ret = RTSP_O_HTTP_GetUserAgent(pSess->au8RecvBuff, pSess->au8UserAgent,&u32UserAgentLen);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "error user-agent: %d\n",ret);
    }
    
    /*获取CSeq信息*/
    ret = RTSP_O_HTTP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_SUCCESS;
    }
    else
    {
        pSess->last_recv_seq= Cseq;
    }
    
    /*获取用户名及密码*/
    HI_CHAR au8UserName[256] = {0};            //用户名
    HI_CHAR au8PassWord[256] = {0};            //密码
    HI_U32 u32UserLen = 256;
    HI_U32 u32PassLen = 256;
    ret = RTSP_O_HTTP_GetUserName(pSess->au8RecvBuff,au8UserName,&u32UserLen, au8PassWord,&u32PassLen);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "invalid Authorization: %s\n", url);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_SUCCESS;
    }
    
    /*向vod请求媒体信息*/
    VOD_DESCRIBE_MSG_S  stDesReqInfo; 
    VOD_ReDESCRIBE_MSG_S stDesRespInfo;
    memset(&stDesReqInfo,0,sizeof(VOD_DESCRIBE_MSG_S));
    memset(&stDesRespInfo,0,sizeof(VOD_ReDESCRIBE_MSG_S));
    strncpy(stDesReqInfo.au8MediaFile,au8FileInfo,sizeof(au8FileInfo));
    strncpy(stDesReqInfo.au8UserName,au8UserName,u32UserLen);
    strncpy(stDesReqInfo.au8PassWord,au8PassWord,u32UserLen);
    strncpy(stDesReqInfo.au8ClientVersion,pSess->au8UserAgent,u32UserAgentLen);
    

    ret = HI_VOD_DESCRIBE(&stDesReqInfo, &stDesRespInfo);

    DEBUGPrtVODDes(&stDesReqInfo, &stDesRespInfo);
    
    if(ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod describe failed %X(client %s)\n",
                   ret,pSess->au8CliIP);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        return ret;
    }
    /*rtsp_o_http会话结构保存server的版本信息*/
    else
    {
        strncpy(pSess->au8SvrVersion,stDesRespInfo.au8BoardVersion,RTSP_O_HTTP_MAX_VERSION_LEN-1);
        pSess->au8SvrVersion[RTSP_O_HTTP_MAX_VERSION_LEN-1] = '\0';
    }

    VOD_SETUP_MSG_S  stSetupReqInfo; 
    VOD_ReSETUP_MSG_S stSetupRespInfo;
    memset(&stSetupReqInfo,0,sizeof(VOD_SETUP_MSG_S));
    memset(&stSetupRespInfo,0,sizeof(VOD_ReSETUP_MSG_S));
    stSetupReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP;
    /*to do:如何cpy字符串*/
    strncpy(stSetupReqInfo.aszSessID,pSess->aszSessID,VOD_MAX_STRING_LENGTH);
    stSetupReqInfo.u8SetupMeidaType = u8ReqMediaType;
    strncpy(stSetupReqInfo.au8ClientIp,pSess->au8CliIP,sizeof(stSetupReqInfo.au8ClientIp));
    strncpy(stDesReqInfo.au8MediaFile,au8FileInfo,VOD_MAX_STRING_LENGTH);
    strncpy(stSetupReqInfo.au8UserName,au8UserName,VOD_MAX_STRING_LENGTH);
    strncpy(stSetupReqInfo.au8PassWord,au8PassWord,VOD_MAX_STRING_LENGTH);
    strncpy(stSetupReqInfo.au8ClientVersion,pSess->au8UserAgent,VOD_MAX_STRING_LENGTH);

    /*解析client端的传输方式*/
    HI_U32 u32TransListLen = VOD_MAX_TRANSTYPE_NUM;
    ret =  RTSP_O_HTTP_TransInfo_Handle(pSess,stSetupReqInfo.astCliSupptTransList,&u32TransListLen);
    if(ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X(client %s)\n",
                   ret,pSess->au8CliIP);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_SERVICE_UNAVAILIABLE, NULL, 1);
        return ret;
    }
    //DEBUGPrtVODDes(&stDesReqInfo, &stDesRespInfo);
    ret = HI_VOD_SetUp(&stSetupReqInfo,&stSetupRespInfo);
    DEBUGPrtVODSetup(&stSetupReqInfo,&stSetupRespInfo);
    if(ret != HI_SUCCESS)
    {
        if(HI_ERR_VOD_ILLEGAL_USER == ret )
        {
        	
            HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X:illegal user %s(%s %s)\n",
                   ret,pSess->au8CliIP,au8UserName,au8PassWord);
            RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_UNAUTHORIZED, NULL, 1);
        }
    	else if(HI_ERR_VOD_OVER_CONN_NUM == ret )
    	{
    	    HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X:over max usernumber for cli %s\n",
                       ret,pSess->au8CliIP);
            RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_OVER_SUPPORTED_CONNECTION, NULL, 1);
    	}
    	else
    	{
    		HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X for %s\n",
                       ret,pSess->au8CliIP);
            RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
    	}
        return ret;
    }
    //DEBUGPrtVODDes(&stDesReqInfo, &stDesRespInfo);
    VOD_PLAY_MSG_S  stPlayReqInfo; 
    VOD_RePLAY_MSG_S stPlayRespInfo;
    memset(&stPlayReqInfo,0,sizeof(VOD_PLAY_MSG_S));
    memset(&stPlayRespInfo,0,sizeof(VOD_RePLAY_MSG_S));
    stPlayReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP ;
    strncpy(stPlayReqInfo.aszSessID,pSess->aszSessID,sizeof(pSess->aszSessID));
    stPlayReqInfo.u8PlayMediaType = u8ReqMediaType;
    stPlayReqInfo.u32Duration = 0;
    stPlayReqInfo.u32StartTime = 0;
    ret = HI_VOD_Play(&stPlayReqInfo, &stPlayRespInfo);
    DEBUGPrtVODPlay(&stPlayReqInfo,&stPlayRespInfo);
    if(ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_ERR, "vod play failed %X(client %s)\n",
                   ret,pSess->au8CliIP);
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        return ret;
    }

    /*组织回复消息*/
    pTemp = pSess->au8SendBuff+ RTSP_O_HTTP_Make_RespHdr(pSess, 200);
    pTemp += sprintf(pTemp,"Server: %s\r\n",stDesRespInfo.au8BoardVersion);
    pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");
    pTemp += sprintf(pTemp,"Accept-Ranges: Bytes\r\n");
    pTemp += sprintf(pTemp,"Content-Type: application/octet-stream\r\n");
    pTemp += sprintf(pTemp,"Connection: close\r\n");
    pTemp += sprintf(pTemp,"\r\n");

    /*组织rtsp_o_http回应信息的包体*/
    char *pTemp2 = sdpMsg;
    pTemp2 += sprintf(pTemp2,"Session: %s\r\n",pSess->aszSessID);
    pTemp2 += sprintf(pTemp2,"Cseq: %d\r\n",(pSess->last_recv_seq)++);

    /*视频媒体描述*/
    /* m=video 96 H264/90000*/
    RTSP_O_HTTP_GetVideoFormatStr(stDesRespInfo.enVideoType, aszVideoFormatStr, &s32VideoRTPPt);
    pTemp2 += sprintf(pTemp2,"m=video %d %s/90000/%d/%d\r\n",s32VideoRTPPt,aszVideoFormatStr,
                      stDesRespInfo.u32VideoPicWidth,stDesRespInfo.u32VideoPicHeight);
    
    /*音频媒体描述*/
    /*m=audio 97 G726/8000/1*/
    /* to do :vod 的采用率和单双通道定义,HI_S32? 枚举?
              由describe的回应转换为协议的采用率和通道字段*/
    RTSP_O_HTTP_GetAudioFormatStr(stDesRespInfo.enAudioType, aszAudioFormatStr, &s32AudioRTPPt);
    pTemp2 += sprintf(pTemp2,"m=audio %d %s/%d/%d\r\n",
                      s32AudioRTPPt,aszAudioFormatStr,s32AudioSampRate,s32AudioChannNum);

    /*Transport: RTP/AVP;unicast;client_port=4588-4589;server_port=6256-6257*/
    if(MTRANS_MODE_TCP_ITLV == stSetupRespInfo.stTransInfo.enMediaTransMode)
    {
        /*tcp潜入式流传输时在会话线呈中发送媒体码流,故传给rtsp_o_http会话相应的媒体传输任务的句柄和函数指针*/
        pSess->bIsIntlvTrans = HI_TRUE;
        pSess->CBonTransItlvMeidaDatas = stSetupRespInfo.stTransInfo.stTcpItlvTransInfo.pProcMediaTrans;
        pSess->u32TransHandle = stSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32TransHandle;

        /*构造回应信息的传输字段*/
        pTemp2 += sprintf(pTemp2,"Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n",
                               stSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrMediaChnId,
                               stSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrAdaptChnId);
    }
    else if(MTRANS_MODE_UDP == stSetupRespInfo.stTransInfo.enMediaTransMode)
    {
        pTemp2 += sprintf(pTemp2,"Transport: RTP/AVP;unicast;client_port=%d-%d;"
                               "server_port=%d-%d\r\n",
                               stSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliMediaRecvPort,
                               stSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort,
                               stSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort,
                               stSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort);
                               
    }
    else
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_NOTICE, "vod chose trans type that client can't support.\n");
        RTSP_O_HTTP_Send_Reply(pSess, RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        return HI_FAILURE;
    }

    pTemp2 += sprintf(pTemp2,"\r\n");     
    
    strcat(pTemp, sdpMsg);

    RTSP_O_HTTP_Send_Reply(pSess, 200, NULL, 0);

    /*rtsp_o_http has complete apply resouce to vod server, and enter into 'play'state*/
    /*to do:是否应该在这里设置*/
    pSess->enSessState = RTSP_O_HTTP_SESSION_STATE_PLAY;
    return HI_SUCCESS;
}
#endif


/*POST http://ip:port/0/replay=audio&video http/1.1\r\n
user-Agent:hisilicon-Client\r\n
Content-Type: application/octet-stream\r\n
Content-Length:89\r\n
Authorization:Basic adsfasfsa\r\n
\r\n
Session: 36753562
CSeq: 3

6
RTSP_O_HTTP/1.1 200 OK\r\n
Cache-Control: no-cache\r\n
Server: Hisilicon Ipcam\r\n
Content-Type: application/octet-stream\r\n
Content-Length: 70\r\n
Connection:Keep-Alive\r\n
\r\n
Session: 36753562
CSeq: 3
*/
HI_VOID RTSP_O_HTTP_Handle_RePlay(HI_S32 s32LinkFd, HI_CHAR* pMsgBuffAddr,HI_U32 u32MsgLen)
{
    HI_S32 s32Ret = 0;
    RTSP_O_HTTP_URL url = {0};
    int rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_OK;
    HI_CHAR   OutBuff[512]= {0};
    HI_CHAR   content[512]= {0};
    HI_CHAR *pTemp = NULL;
    HI_CHAR *pTemp2 = NULL;
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH]= {0}; 
    HI_CHAR aszSvrVersion[RTSP_O_HTTP_MAX_VERSION_LEN] = {0}; 
    HI_U8 u8PlayMediasType = 0;
    HI_S32 Cseq = 0; 
    RTSP_O_HTTP_REQ_METHOD_E method = RTSP_O_HTTP_REQ_METHOD_BUTT;

    HI_LOG_DEBUG(RTSP_O_HTTP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C --> S >>>>>>\n"
                   "%s\n"
                   "<<<<<<\n" ,
                   pMsgBuffAddr);
                   
    if (!sscanf(pMsgBuffAddr, " %*s %254s ", url)) 
    {
        s32Ret = HI_ERR_RTSP_O_HTTP_ILLEGALE_PARAM;
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(s32LinkFd,rtsp_o_http_err_code);
        return;
	}

	/*获取url中指定的暂停媒体类型及pause方法*/
	s32Ret= RTSP_O_HTTP_ParseParam(url,&method, &u8PlayMediasType);
	if(   (s32Ret != HI_SUCCESS) 
	   || (s32Ret == HI_SUCCESS && RTSP_O_HTTP_REPLAY_METHOD != method))
	{
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(s32LinkFd,rtsp_o_http_err_code);
        return;
	}

    /*获取CSeq信息*/
    s32Ret = RTSP_O_HTTP_Get_CSeq(pMsgBuffAddr,&Cseq);
    if( HI_SUCCESS != s32Ret)
    {
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(s32LinkFd,rtsp_o_http_err_code);
        return;
    }
    
    /*获取sessionid*/
    s32Ret = RTSP_O_HTTP_Get_SessId(pMsgBuffAddr,aszSessID);
    if(HI_SUCCESS != s32Ret)
    {
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(s32LinkFd,rtsp_o_http_err_code);
        return;
    }

    /*如果前面解析都无错，则向vod请求replay*/
    VOD_PLAY_MSG_S stVodPlayMsg;
    VOD_RePLAY_MSG_S stVodRePlayMsg;
    memset(&stVodPlayMsg,0,sizeof(VOD_PLAY_MSG_S));
    memset(&stVodRePlayMsg,0,sizeof(VOD_RePLAY_MSG_S));

    /*consist vod play msg*/
    stVodPlayMsg.enSessType = SESS_PROTOCAL_TYPE_RTSP ;
    strncpy(stVodPlayMsg.aszSessID,aszSessID,VOD_MAX_STRING_LENGTH);
    stVodPlayMsg.u32StartTime = 0;     /*this attribution is invalid for live*/
    stVodPlayMsg.u32Duration =0;       /*this attribution is invalid for live*/
    stVodPlayMsg.u8PlayMediaType= u8PlayMediasType;

    s32Ret = HI_VOD_Play(&stVodPlayMsg,&stVodRePlayMsg);
    DEBUGPrtVODPlay(&stVodPlayMsg,&stVodRePlayMsg);
    if(HI_SUCCESS != s32Ret)
    {
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(s32LinkFd,rtsp_o_http_err_code);
        return;
    }

    s32Ret = HI_VOD_GetVersion(aszSvrVersion,RTSP_O_HTTP_MAX_VERSION_LEN);
    if(s32Ret != HI_SUCCESS)
    {
        rtsp_o_http_err_code = RTSP_O_HTTP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
        RTSP_O_HTTP_SendErrResp(s32LinkFd,rtsp_o_http_err_code);
        return;
    }

    /*consist replay response*/
    pTemp = OutBuff;
    pTemp += sprintf(pTemp, "%s %d %s\r\n", RTSP_O_HTTP_VER_STR, 200, "OK");
    pTemp += sprintf(pTemp,"Server: %s\r\n",aszSvrVersion);
    pTemp += sprintf(pTemp,"Connection: Keep-Alive\r\n");
    pTemp += sprintf(pTemp,"Content-Type: application/octet-stream\r\n");
    pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");

    /* to do:回应中是否需要带cseq session id, rtsp_o_httpsess中的Cseq如相应增加*/
    pTemp2 = content;
    pTemp2 += sprintf(pTemp2,"Session: %s\r\n",aszSessID);
    pTemp2 += sprintf(pTemp2,"Cseq: %d\r\n",Cseq);
    pTemp2 += sprintf(pTemp2,"\r\n");
    
    pTemp += sprintf(pTemp,"Content-Length: %d\r\n\r\n", strlen(content));  

    strcat(pTemp, content);
     

    /*发送回应信息*/
    s32Ret = HI_RTSP_O_HTTP_Send(s32LinkFd,OutBuff,strlen(OutBuff));	/* bad request */
    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERR,"to send rtsp_o_http replay response %d byte thought sock %d failed\n",
                   strlen(OutBuff),s32LinkFd);
    }
    else
    {
        
        HI_LOG_DEBUG(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE, "Send Reply .S --> C >>>>>>\n"
                       "%s\n"
                       "<<<<<<\n",
                       OutBuff);
    }

    /*关闭该链接*/
    close(s32LinkFd);

}

HI_S32 RTSP_O_HTTP_Handle_Stop(RTSP_O_HTTP_LIVE_SESSION *pSess )
{
    HI_S32 s32Ret = 0;
    VOD_TEAWDOWN_MSG_S stVodCloseMsg;
    VOD_ReTEAWDOWN_MSG_S stVodReCloseMsg;
    memset(&stVodCloseMsg,0,sizeof(VOD_TEAWDOWN_MSG_S));
    memset(&stVodReCloseMsg,0,sizeof(VOD_ReTEAWDOWN_MSG_S));

    if(NULL == pSess)
    {
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_PARAM;
    }

    /*consist vod teardown msg*/
    stVodCloseMsg.enSessType = SESS_PROTOCAL_TYPE_RTSP ;
    strncpy(stVodCloseMsg.aszSessID,pSess->aszSessID,strlen(pSess->aszSessID));
    DEBUGPrtVODTeawdown(&stVodCloseMsg,&stVodReCloseMsg);

    s32Ret = HI_VOD_Teardown(&stVodCloseMsg,&stVodReCloseMsg);
    if(HI_SUCCESS != s32Ret)
    {
        /*vod has not creat corresponding vod session or 
         the session has colsed*/
        if(HI_ERR_VOD_INVALID_SESSION == s32Ret)
        {
            return HI_SUCCESS;
        }
        return s32Ret;
    }

    DEBUGPrtVODTeawdown(&stVodCloseMsg,&stVodReCloseMsg);

    return HI_SUCCESS;
}

HI_S32 RTSP_O_HTTP_CloseSession(RTSP_O_HTTP_LIVE_SESSION * pSess)
{
    httpd_conn* pHc;
    if(pSess->pGetConnHandle)
    {
        pHc = (httpd_conn*)pSess->pGetConnHandle;
        pHc->delay_close = 0;
        pSess->pGetConnHandle = 0;
    }

    if(pSess->pPostConnHandle)
    {
        pHc = (httpd_conn*)pSess->pPostConnHandle;
        pHc->delay_close = 0;
        pSess->pPostConnHandle = 0;
    }

    HI_RTSP_O_HTTP_SessionClose(pSess);

    return HI_SUCCESS;
}

/*configrate read set's timeval*/
HI_VOID RTSP_O_HTTPConfigTimeval(RTSP_O_HTTP_LIVE_SESSION* pSession,struct timeval* pTimeoutVal)
{
    RTSP_O_HTTP_SESSION_STATE enSessStat = RTSP_O_HTTP_SESSION_STATE_BUTT;  /*rtsp_o_http session state*/
    
    enSessStat = pSession->enSessState;

    /*1. if rtsp_o_http session procedure has not complete, 
        1)will at least one message come
        2)haven't send media data yet 
    then should wait for enough time ,so timeval:10sec*/
    if(RTSP_O_HTTP_SESSION_STATE_PLAY != enSessStat)
    {
        //printf("%s:just rtsp_o_http msg.\n",pSession->aszSessID);
        pTimeoutVal->tv_sec = 10;
        pTimeoutVal->tv_usec = 0;
    }
    /*2. if rtsp_o_http procedure has completed, and media data is not interleaved trans type
        1)maybe there will be message come or not,
        2)will not send data in rtsp_o_http session thread;
      then should wait for a long enough time, oterwise, the rtsp_o_http session thread is busywaiting,
      so timeval 10sec*/
    else if(RTSP_O_HTTP_SESSION_STATE_PLAY == enSessStat && HI_FALSE == pSession->bIsIntlvTrans)
    {
        //printf("%s:ready to send,but not trans data in rtsp_o_http thread.\n",pSession->aszSessID);
        pTimeoutVal->tv_sec = 10;
        pTimeoutVal->tv_usec = 0;
    }
    
     /*3. if rtsp_o_http procedure has completed, and media data is interleaved trans type
        1)maybe there will be message come or not,
        2)will send data in rtsp_o_http session thread, and data should be send in time;
       then we should no wait if there is no msg comeing , oterwise, the data will not be 
      blcoked send untile spare recv select time,
      so timeval 0 msec*/
    else if(RTSP_O_HTTP_SESSION_STATE_PLAY == enSessStat && HI_TRUE == pSession->bIsIntlvTrans)
    {
        //printf("%s:send data.\n",pSession->aszSessID);
        pTimeoutVal->tv_sec = 0;
        pTimeoutVal->tv_usec = 0;      
    }
    
}



HI_S32 RTSP_O_HTTP_CMD_Proc(RTSP_O_HTTP_LIVE_SESSION* pSess)
{
    // 1.base64decode
    //2.decode the cmd
    HI_S32 s32SendRespRet = 0 ;

    HI_S32 s32Ret = 0,s32MehodProcRet = 0;
    HI_CHAR au8DeContent[1024]={0};
	HI_CHAR au8tmp[1024] = {0};
    RTSP_O_HTTP_REQ_METHOD_E enMethod =RTSP_O_HTTP_REQ_METHOD_BUTT;
	HI_S32 s32declen = 0;

    //printf(" in cmd proc the content is :%s \n",pSess->au8RecvBuff);
    //HI_U32 au8DeContentLen = 0;
    
    if (strlen(pSess->au8RecvBuff) == 0) {
		/*发现有空的字段送达，在此忽略*/
		printf("strlen(au8RecvBuff) = 0\n");	
		return HI_SUCCESS;
    }
	//有些BASE64字符串中带有换行符,必须去掉，否则会得出不完整的命令行（line breaks in base64 encoding）
	if (NULL != strstr(pSess->au8RecvBuff,"\r\n")) {
		int i,xpos=0;
		for (i=0;i<strlen(pSess->au8RecvBuff);i++) {
			if ('\r'== *(pSess->au8RecvBuff+i) || '\n'== *(pSess->au8RecvBuff+i))
				continue;
			memcpy(&au8tmp[xpos],pSess->au8RecvBuff+i,1);
			xpos ++;
		}		
		s32declen = base64decode(au8tmp, strlen(au8tmp), au8DeContent, 1024);	
		
	}else
    	s32declen = base64decode(pSess->au8RecvBuff, strlen(pSess->au8RecvBuff), au8DeContent, 1024);
	
	if (s32declen == 0 ) {
		/*发现有空的字段送达，在此忽略*/
		printf("s32declen = 0\n");	
		return HI_SUCCESS;	
	}
    memset(pSess->au8RecvBuff,0,RTSP_O_HTTP_MAX_PROTOCOL_BUFFER);
    strncpy(pSess->au8RecvBuff,au8DeContent,strlen(au8DeContent));
    printf("the decode recv msg is :%s \n",au8DeContent);

    //3.handle every cmd :discrible,setup,play,pause ,teardown
    s32Ret = RTSP_O_HTTP_CMD_MethodGet(pSess, &enMethod);
    if(s32Ret == HI_SUCCESS)
    {
        switch( enMethod)
        {
            case RTSP_O_HTTP_DISCRIBLE_METHOD:
                printf("the discribe method \n");
                s32MehodProcRet = RTSP_O_HTTP_Handle_Describle(pSess);
                break;
            case RTSP_O_HTTP_SETUP_METHOD:
                printf("the setup method \n");
                s32MehodProcRet = RTSP_O_HTTP_Handle_Setup(pSess);
                break;
            case RTSP_O_HTTP_PLAY_METHOD:
                printf("the play method \n");
                s32MehodProcRet= RTSP_O_HTTP_Handle_Play(pSess);
                break;
            case RTSP_O_HTTP_PAUSE_METHOD:
                printf("the pause method \n");
                s32MehodProcRet = RTSP_O_HTTP_Handle_Pause(pSess);
                break;
            case RTSP_O_HTTP_TEARDOWN_METHOD:
                printf("the teardown method \n");
                s32MehodProcRet = RTSP_O_HTTP_Handle_Teardown(pSess);
                break;
            case RTSP_O_HTTP_SET_PARAM_METHOD:
                RTSP_O_HTTP_Send_Reply(pSess, 200, NULL, 1);
                break;
            case RTSP_O_HTTP_OPTIONS_METHOD:
                printf("the options method \n");
                s32MehodProcRet = RTSP_O_HTTP_Handle_Options(pSess);
                break;
            default:
                s32Ret = HI_FAILURE;
                break;
        }
    }
    if(s32Ret != HI_SUCCESS)
    {
        //send the not support msg	
        printf("%s, unsupport method %d ret=%d\n",__FUNCTION__, enMethod,s32Ret);
        RTSP_O_HTTP_Send_Reply(pSess, 501, NULL, 1);
    }
    //4.send the response msg

    if (pSess->readyToSend == HI_TRUE)
    {

        s32SendRespRet = HI_RTSP_O_HTTP_Send(pSess->s32GetConnSock, pSess->au8SendBuff, strlen(pSess->au8SendBuff)) ;
        if (HI_SUCCESS != s32SendRespRet)
        {
            printf("%s, HI_RTSP_O_HTTP_Send failed :%x \n",__FUNCTION__, s32SendRespRet);
            HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERR,"to send rtsp_o_http response %d byte to cli %s thought sock %d failed\n",
                strlen(pSess->au8SendBuff),pSess->au8CliIP,pSess->s32GetConnSock);
            s32SendRespRet = HI_ERR_RTSP_O_HTTP_SEND_FAILED;
        }
        else
        {

            HI_LOG_DEBUG(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE, "Send Reply .Chn:%d S --> C(%s) >>>>>>\n"
                "%s\n"
                "<<<<<<\n",
                pSess->s32VsChn, pSess->au8CliIP,pSess->au8SendBuff);    
        }

        HI_RTSP_O_HTTP_ClearSendBuff(pSess);

        /*updat read to send flag as false*/
        pSess->readyToSend = HI_FALSE;
    }
    
    if(enMethod == RTSP_O_HTTP_TEARDOWN_METHOD)
    {
        return HI_FAILURE;
    }
    if(s32MehodProcRet != HI_SUCCESS)
    {
        return s32MehodProcRet;
    }
    else if(s32SendRespRet != HI_SUCCESS)
    {
        return s32SendRespRet;
    }
    return HI_SUCCESS;
}

#define SESSION_SOCK 1
#define CMD_SOCK 2
/*process client stream request: rtsp process and send stream*/
HI_VOID* RTSP_O_HTTPSessionProc(HI_VOID * arg)
{

    HI_S32      s32Ret      = 0;
    HI_S32      s32RTSP_O_HTTPStopRet = 0;
    HI_S32      s32Errno    = 0;                        /*linux system errno*/
    HI_SOCKET   s32TempSessSock = -1;
    HI_S32      s32RecvBytes = 0;
    HI_IP_ADDR  au8CliIP;
    HI_S32     s32SessThdId  = 0;
    RTSP_O_HTTP_LIVE_SESSION *pSession = NULL;
    memset(au8CliIP,0,sizeof(HI_IP_ADDR));

    fd_set read_fds;                           /* read socket fd set */    
    struct timeval TimeoutVal;                      /* Timeout value */
    struct timeval TempTimeoutVal;
    
    memset(&TimeoutVal,0,sizeof(struct timeval));
    memset(&TempTimeoutVal,0,sizeof(struct timeval));

    pSession = (RTSP_O_HTTP_LIVE_SESSION *)arg;
    s32TempSessSock = pSession->s32GetConnSock;
    s32SessThdId = pSession->s32SessThdId;
    memcpy(au8CliIP,pSession->au8CliIP,sizeof(HI_IP_ADDR));
    
	//HI_S32 on = 1;
	
	//setsockopt(pSession->s32SessSock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));

    HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE,"[Info]rtsp_o_http client %s connected (%d client alive) !!!\n",
               au8CliIP,HI_RTSP_O_HTTP_SessionGetConnNum());

    HI_LOG_DEBUG(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE,"[Info]rtsp_o_http live process thread %d start ok, communite with cli %s....\n",
                s32SessThdId, au8CliIP);

    s32TempSessSock = pSession->s32PostConnSock;

    if(HI_TRUE == pSession->bIsFirstMsgHasInRecvBuff)
    {
        s32Ret = RTSP_O_HTTP_CMD_Proc(pSession);
        if(s32Ret != HI_SUCCESS)
        {
            printf("%s(%d),Process First message failed!ret:%x\n",__FUNCTION__,__LINE__,s32Ret);
        }
    }
    
    /*process rtsp_o_http live session*/
    while(RTSP_O_HTTPSESS_THREAD_STATE_RUNNING == pSession->enSessThdState)
    {
        FD_ZERO(&read_fds);
        FD_SET(pSession->s32PostConnSock, &read_fds);

        /*important:config select timeval.different state should wait for differ time*/
        RTSP_O_HTTPConfigTimeval(pSession, &TimeoutVal);
        memcpy(&TempTimeoutVal,&TimeoutVal,sizeof(struct timeval));

        s32Ret = select(s32TempSessSock+1,&read_fds,NULL,NULL,&TimeoutVal);
        
        /*3.1 some wrong with network*/
        if ( s32Ret < 0 ) 
        {
            s32Errno = errno;
            /*notify VOD destroyVODTask*/
            s32RTSP_O_HTTPStopRet = RTSP_O_HTTP_Handle_Stop(pSession);
            if(HI_SUCCESS != s32RTSP_O_HTTPStopRet)
            {
                DEBUG_RTSP_O_HTTP_LIVE_PRINT(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_O_HTTP_Handle_Stop error = %X\n",s32RTSP_O_HTTPStopRet);
            }
            
            RTSP_O_HTTP_CloseSession(pSession);

            HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_FATAL,"rtsp_o_http client %s disconnected (%d client alive):[select: %s]\n",
                       au8CliIP,HI_RTSP_O_HTTP_SessionGetConnNum(),strerror(s32Errno));
            
            return HI_NULL;
        }
        #if 0
        /*3.2 over select time*/
        else if(0 == s32Ret)
        {
            /*overtime:rtsp_o_http session has not completed, and has wait for a long time,
               here believe some wrong with network,so close the link and withdraw the thread. 
            */
            if(RTSP_O_HTTP_SESSION_STATE_PLAY != pSession->enSessState)
            {
                /*notify VOD destroyVODTask*/
                s32RTSP_O_HTTPStopRet = RTSP_O_HTTP_Handle_Stop(pSession);
                if(HI_SUCCESS != s32RTSP_O_HTTPStopRet)
                {
                    DEBUG_RTSP_O_HTTP_LIVE_PRINT(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_O_HTTP_Handle_Stop error = %X\n",s32RTSP_O_HTTPStopRet);
                }
                HI_RTSP_O_HTTP_SessionClose(pSession);
                
                HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_FATAL," client %s disconnected (%d client alive):"\
                           "[not complete rtsp_o_http session,select: overtime %d.%ds)]\n",
                           au8CliIP,HI_RTSP_O_HTTP_SessionGetConnNum(), TempTimeoutVal.tv_sec,TempTimeoutVal.tv_usec);
                
                return HI_NULL;
                
            }
			
            /*overtime:rtsp_o_http session has completed, may be there is no message come for a long time, 
             so continue to wait. (wait timeval see function configSet)
            */
            else
            {
                //do nothing, just go to ProcTrans, if media data is interleaved transed. 
            }
        }
        #endif
        /*3.3 process*/
        else if(s32Ret > 0)
        {
            /*3.3.1 there is msg come, then recv the msg and process it*/
            if(FD_ISSET(pSession->s32PostConnSock,&read_fds))
            {
                memset(pSession->au8RecvBuff,0,RTSP_O_HTTP_MAX_PROTOCOL_BUFFER);

                // to do :recv 应封装并加超时
                s32RecvBytes = recv(s32TempSessSock, 
                    pSession->au8RecvBuff, RTSP_O_HTTP_MAX_PROTOCOL_BUFFER, 0);
                if (s32RecvBytes < 0 )
                {

                    printf("%s  %d, recv failed!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
                    break; 
                }
                /*peer close*/
                else if (s32RecvBytes == 0)
                {
					printf("%s  %d, peer close!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
                    break; 
                    
                }
                /*process the request msg and send its response*/
                else
                {
                	
                    s32Ret = RTSP_O_HTTP_CMD_Proc(pSession);
                    if(s32Ret != HI_SUCCESS)
                    {
                        printf("the cmd session is closed ret:%x\n",s32Ret);
                        break;
                    }
                }
            
             }
             
         }
        //}
         /*二 process media data trans. just when rtsp_o_http session is in playing state,
              and media data is interleaved transed */
        if(RTSP_O_HTTP_SESSION_STATE_PLAY == pSession->enSessState && HI_TRUE == pSession->bIsIntlvTrans)
        {
            /*prodess media trans task:send media data which in interleaved type*/
            s32Ret = pSession->CBonTransItlvMeidaDatas(pSession->u32TransHandle,0);
			
            /*something wrong in send data*/
            if(s32Ret != HI_SUCCESS 
                && HI_ERR_MTRANS_NO_DATA != s32Ret
                && MTRANS_SEND_IFRAME != s32Ret)
            {
                /*notify VOD destroyVODTask*/
                s32RTSP_O_HTTPStopRet = RTSP_O_HTTP_Handle_Stop(pSession);
                if(HI_SUCCESS != s32RTSP_O_HTTPStopRet)
                {
                    DEBUG_RTSP_O_HTTP_LIVE_PRINT(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_O_HTTP_Handle_Stop error = %X\n",s32RTSP_O_HTTPStopRet);
                }
				
                RTSP_O_HTTP_CloseSession(pSession);
                HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_FATAL,"client %s disconnected (%d client alive):[send media data error %X]\n",
                           au8CliIP, HI_RTSP_O_HTTP_SessionGetConnNum(),s32Ret);
             
                return HI_NULL;
            }

        }


    }   /*end of while()*/     

    /*notify VOD destroyVODTask*/
    s32RTSP_O_HTTPStopRet = RTSP_O_HTTP_Handle_Stop(pSession);
    if(HI_SUCCESS != s32RTSP_O_HTTPStopRet)
    {
        DEBUG_RTSP_O_HTTP_LIVE_PRINT(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_O_HTTP_Handle_Stop error = %X\n",s32RTSP_O_HTTPStopRet);
    }
    RTSP_O_HTTP_CloseSession(pSession);
    HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE,"client %s disconnected (%d client alive):[some operation happened,"\
               "such as root, set video size ... ]\n",au8CliIP,HI_RTSP_O_HTTP_SessionGetConnNum());

    HI_LOG_DEBUG(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE,"[Info]thd %d withdraw: stop process with client %s... \n",
                 s32SessThdId, au8CliIP);

    return HI_NULL;
}

/*rtsp_o_http live server init*/
HI_S32 HI_RTSP_O_HTTP_LiveSvr_Init(HI_S32 s32MaxConnNum)
{
    HI_S32 s32Ret         = 0;

    if(RTSP_O_HTTP_LIVESVR_STATE_Running == g_strurtsp_o_httpSvr.enSvrState)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERR,"RTSP_O_HTTP LiveSvr Init Failed .\n");
        return HI_ERR_RTSP_O_HTTP_SVR_ALREADY_RUNNING;
    }

    if( 0 >= s32MaxConnNum)
    {
        s32MaxConnNum = RTSP_O_HTTP_MAX_SESS_NUM;
    }

    /* clear rtsp_o_http server struct*/
    memset(&g_strurtsp_o_httpSvr,0,sizeof(RTSP_O_HTTP_LIVE_SVR_S));
    g_strurtsp_o_httpSvr.enSvrState = RTSP_O_HTTP_LIVESVR_STATE_STOP; 

    /*2 init rtsp_o_http session list*/
    s32Ret = HI_RTSP_O_HTTP_SessionListInit(s32MaxConnNum);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERR,
                    "RTSP_O_HTTP LiveSvr Init Failed (Session Init Error=%d).\n",s32Ret);
        return s32Ret;
    }

    /*5 update vod svr's state as running*/
    g_strurtsp_o_httpSvr.enSvrState = RTSP_O_HTTP_LIVESVR_STATE_Running;
    HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE,"rtsp_o_http Live svr Init ok.\n");
    
    return HI_SUCCESS;
}

/*live deinit: waiting for every 'rtsp_o_http sesssion process' thread to withdraw*/
HI_S32 HI_RTSP_O_HTTP_LiveSvr_DeInit()
{
    if(RTSP_O_HTTP_LIVESVR_STATE_STOP == g_strurtsp_o_httpSvr.enSvrState)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERR,"RTSP_O_HTTP LiveSvr DeInit Failed:already deinited.\n");
        return HI_ERR_RTSP_O_HTTP_SVR_ALREADY_STOPED;
    }
    
    /*1 stop the rtsp_o_httplisen svr*/
    /*webserver is treated as rtsp_o_http live listen server,here not stop webserver */
    
    //HI_MSESS_LISNSVR_StopSvr(g_strurtsp_o_httpSvr.pu32RTSP_O_HTTPSessListHandle);
    g_strurtsp_o_httpSvr.enSvrState = RTSP_O_HTTP_LIVESVR_STATE_STOP;
    
    /*2 stop all 'rtsp_o_httpSessionProcess' thread,close the link, free all session */
    HI_RTSP_O_HTTP_SessionAllDestroy();

    /*4 set vod svr as stop, */

    // to do :??so getstream thread will not write data to mbuff*/
    
    HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE,"rtsp_o_http Live DeInit ok ...");

    return HI_SUCCESS;

}

HI_S32 Rtsp_o_Http_CheckMsgValid(char *pMsgBufAddr, char *pCookie)
{
    int ret = 0;
    char *p;
    if(!pMsgBufAddr || !pCookie)
        return -1;

    p = strstr(pMsgBufAddr, "x-sessioncookie:");
    if(p == NULL)
        return -2;

    ret = sscanf(p, "x-sessioncookie: %s", pCookie);
    if(ret != 1)
        return -3;

    p = strstr(pMsgBufAddr, "x-rtsp-tunnelled");
    if(p == NULL)
        return -4;

    return 0;
}


/*创建会话任务，若第一个会话消息已接收则使pMsgBuffAddr指向消息首地址,
 否则置pMsgBuffAddr为null*/
HI_S32 HI_RTSP_O_HTTP_CreatHttpLiveSession(httpd_conn* hc)
{
    HI_S32 s32Ret = 0;
    char *p;
    HI_CHAR OutBuff[1024] = {0};
    RTSP_O_HTTP_LIVE_SESSION *pSession = NULL;
    HI_CHAR SessCookie[RTSP_O_HTTP_COOKIE_LEN] = {0};

    printf("--------get,fd=%d-------\n%s\n------------------\n",hc->conn_fd,hc->tmp_buf);

    s32Ret = Rtsp_o_Http_CheckMsgValid(hc->tmp_buf, SessCookie);
    if(s32Ret != 0)
    {
        printf("%s  %d, Invalid rtsp over http request msg!s32Ret:%d\n",__FUNCTION__, __LINE__,s32Ret);

        return HI_ERR_RTSP_O_HTTP_ILLEGALE_REQUEST;
    }
    /*1 get a free session first*/
	//printf("%s,%d HI_RTSP_O_HTTP_CreatHttpLiveSession step1 \n"__FILE__,__LINE__);
    s32Ret = HI_RTSP_O_HTTP_SessionCreat(&pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_ERR,
                    "creat session for a new connect error=%d.\n",s32Ret);
		//printf("%s,%d HI_RTSP_O_HTTP_CreatHttpLiveSession step1.1 \n"__FILE__,__LINE__);
        return s32Ret;            
    }
    else
    {
    	//printf("%s,%d HI_RTSP_O_HTTP_CreatHttpLiveSession step1.2 \n"__FILE__,__LINE__);
        /*init the session according the connection's socket */
        HI_RTSP_O_HTTP_SessionInit(pSession, hc->conn_fd);

        pSession->pGetConnHandle = (HI_U32)hc;
        strcpy(pSession->SessCookie, SessCookie);
		strcpy(pSession->au8CliIP, inet_ntoa(hc->client_addr.sa_in.sin_addr));
        pSession->s32CliHttpPort = (unsigned short)ntohs(hc->client_addr.sa_in.sin_port);
    }

    p = OutBuff;
    p += sprintf(p,"HTTP/1.0 200 OK\r\n");
    p += sprintf(p,"Content-Type: application/x-rtsp-tunnelled\r\n");
    p += sprintf(p,"\r\n");
	//printf("%s,%d HI_RTSP_O_HTTP_CreatHttpLiveSession step2 \n"__FILE__,__LINE__);
    s32Ret = HI_RTSP_O_HTTP_Send(hc->conn_fd,OutBuff,strlen(OutBuff));	
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s  %d, Send response msg failed!err:\n",__FUNCTION__, __LINE__, s32Ret);
        HI_RTSP_O_HTTP_SessionListRemove(&pSession);
        return s32Ret;
    }
  //  printf("%s,%d HI_RTSP_O_HTTP_CreatHttpLiveSession step3 \n"__FILE__,__LINE__);
    return HI_SUCCESS;
}      


HI_S32 HI_RTSP_O_HTTP_PostMsgProc(httpd_conn* hc)
{
    HI_S32 s32Ret = 0;
    RTSP_O_HTTP_LIVE_SESSION* pSess = NULL; 
    HI_CHAR SessCookie[RTSP_O_HTTP_COOKIE_LEN] = {0};
    char query[MAX_CNT_CGI];

    printf("%s ----------post----------\n%s\n---------------------------\n",__FUNCTION__,hc->tmp_buf);
#if 0
    HI_CGI_strdecode(query, hc->tmp_buf);
#endif
    s32Ret = Rtsp_o_Http_CheckMsgValid(query, SessCookie);
    if(s32Ret != 0)
    {
        printf("%s  %d, Invalid rtsp over http request msg!s32Ret:%d\n",__FUNCTION__, __LINE__,s32Ret);

        return HI_ERR_RTSP_O_HTTP_ILLEGALE_REQUEST;
    }
	
    pSess = HTTP_LIVE_FindRelateSess(SessCookie);
    if(pSess == NULL)
    {
        printf("%s  %d, can't find the session which have a same cookie:%s\n",__FUNCTION__, __LINE__,SessCookie);
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_REQUEST;
    }
    
    pSess->s32PostConnSock = hc->conn_fd;
    pSess->pPostConnHandle = (HI_U32)hc;
    pSess->enSessThdState = RTSP_O_HTTPSESS_THREAD_STATE_RUNNING;

    char *p;
    p = strrchr(hc->tmp_buf, '\n');
    if(p && *(++p) != '\0')
    {
        strcpy(pSess->au8RecvBuff, p);
        pSess->bIsFirstMsgHasInRecvBuff = HI_TRUE;
    }

    s32Ret = pthread_create( &(pSess->s32SessThdId),NULL, RTSP_O_HTTPSessionProc, (void *)pSess);
    pthread_detach(pSess->s32SessThdId);
    if (s32Ret)
    {
        HI_LOG_SYS(RTSP_O_HTTP_LIVESVR,LOG_LEVEL_NOTICE,"Start rtsp_o_http session process thread error. cann't create thread.\n");
        pSess->enSessThdState = RTSP_O_HTTPSESS_THREAD_STATE_STOP;
        RTSP_O_HTTP_CloseSession(pSess);
        return  HI_ERR_RTSP_O_HTTP_CREAT_SESSION_THREAD_FAILED;
    }
	

	return HI_SUCCESS;
}

/*接收一个由监听模块(webserver)分发的链接
 1 解析是否是rtsp_o_http 直播连接
 2 解析请求的直播方法
 3 按不同的方式分别处理各请求方法*/

HI_S32 HI_RTSP_O_HTTP_DistribLink(httpd_conn* hc)
{
    HI_S32   s32Ret  = HI_SUCCESS;
    HI_S32   methodcode = 0;
    HI_S32   rep;
    HI_U32   seq_num;
    HI_U32   status;
    HI_CHAR   OutBuff[512] = {0};
    HI_CHAR  *pTemp = NULL;
    HI_S32   errcode = RTSP_O_HTTP_STATUS_OK;
	//printf("%s,%d HI_RTSP_O_HTTP_DistribLink step1 \n"__FILE__,__LINE__);
    if(NULL == hc)
    {
        printf("the http msg is null\n");
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_PARAM;
    }
    if(g_strurtsp_o_httpSvr.enSvrState == RTSP_O_HTTP_LIVESVR_STATE_STOP)
    {
        printf("%s, Rtsp_o_http service already stoped!\n",__FUNCTION__);
        return HI_ERR_RTSP_O_HTTP_SVR_ALREADY_STOPED;
    }
    
    printf("rtsp_o_http distribute link start\n");
	//printf("%s,%d HI_RTSP_O_HTTP_DistribLink step2 \n"__FILE__,__LINE__);
    /* check for request message. */
    rep = RTSP_O_HTTP_Valid_RespMsg(hc->tmp_buf, &seq_num, &status);

    if ( rep != HI_RTSP_O_HTTP_PARSE_ISNOT_RESP )
    {  
        printf("rtsp_o_http distribute link is resp\n");

        return HI_ERR_RTSP_O_HTTP_ILLEGALE_REQUEST;
    }
	//printf("%s,%d HI_RTSP_O_HTTP_DistribLink step3 \n"__FILE__,__LINE__);
    if(METHOD_GET == hc->method)
    {
    	//printf("%s,%d HI_RTSP_O_HTTP_DistribLink step3.1 \n"__FILE__,__LINE__);
        printf("rtsp_o_http distribute link start HI_RTSP_O_HTTP_CreatHttpLiveSession\n");
        s32Ret = HI_RTSP_O_HTTP_CreatHttpLiveSession(hc);
        if(s32Ret != HI_SUCCESS)
            printf("%s, HI_RTSP_O_HTTP_CreatHttpLiveSession error!\n",__FUNCTION__);
    }
    else if(METHOD_POST == hc->method)
    {
    	//printf("%s,%d HI_RTSP_O_HTTP_DistribLink step3.2 \n"__FILE__,__LINE__);
        printf("rtsp_o_http distribute link start HI_RTSP_O_HTTP_PostMsgProc\n");
        s32Ret = HI_RTSP_O_HTTP_PostMsgProc(hc);
    }
    else
    {
    	//printf("%s,%d HI_RTSP_O_HTTP_DistribLink step3.3 \n"__FILE__,__LINE__);
        printf("%s  %d, the method %d is not supported.\n",__FUNCTION__, __LINE__, hc->method);
        return HI_ERR_RTSP_O_HTTP_ILLEGALE_REQUEST;
    }

    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

