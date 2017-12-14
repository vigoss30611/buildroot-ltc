/******************************************************************************

  Copyright (C), 2007-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_mession_RTSP.c
  Version       : Initial Draft
  Author        : w54723
  Created       : 2008/1/30
  Description   : RTSP session protocal process for live and vod
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
#include <sys/prctl.h>
#include <string.h>
#include <netinet/tcp.h>
#include <pthread.h>

#ifdef HI_PTHREAD_MNG
#include "hi_pthread_wait.h"
#endif

//btlib
//#include "hi_debug_def.h"
//#include "hi_log_app.h"
#include "ext_defs.h"
#include "hi_msession_rtsp.h"
#include "hi_rtsp_session.h"
#include "hi_msession_rtsp_error.h"
#include "hi_msession_lisnsvr.h"
#include "hi_rtsp_parse.h"
#include "hi_vod_error.h"
#include "hi_vod.h"
//#include "hi_version.h"
#include "hi_mtrans_error.h"


#define RTSP_LIVESVR "RTSP_LIVESVR"

/*RTSP server struct for live or vod*/
RTSP_LIVE_SVR_S g_struRTSPSvr;

#define  RTSP_TRANS_TIMEVAL_SEC    10
#define  RTSP_TRANS_TIMEVAL_USEC   0

#define RTSP_TEAR_DOWN_END_FALG  2
//#define SPS_PPS_VALUE "adfsadfasdfafsadf"

//HI_U8 SPS[9] = {0x67,0x42,0xE0,0x1E,0xDA,0x02,0x80,0xF4,0x40};
//HI_U8 PPS[5] = {0x68,0xCE,0x30,0xA4,0x80};
HI_U8 SPS[8] = {0x67,0x42,0xE0,0x14,0xDA,0x05,0x07,0xC4};
HI_U8 PPS[5] = {0x68,0xCE,0x30,0xA4,0x80};
#if 0
HI_U8 SPS_PPS_VALUE[80];
HI_VOID SPS_PPS_CAL()
{
    HI_U8 achSPS[40];
    HI_U8 achPPS[40];
    memset(achSPS,0,40);
	memset(achPPS,0,40);
	memset(SPS_PPS_VALUE,0,80);
	MP4ToBase64( SPS, 8, achSPS );

	MP4ToBase64( PPS, 5,  achPPS );
	sprintf( SPS_PPS_VALUE, "%s,%s", achSPS, achPPS );
    printf("the sps_pps is :%s ",SPS_PPS_VALUE);
    return;
}

#endif

static char *dateHeader() {
	static char buf[200];

	time_t tt = time(NULL);
	strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n",
			gmtime(&tt));

	return buf;
}

HI_S32 HI_RTSP_Send(HI_SOCKET s32WritSock,HI_CHAR* pszBuff, HI_U32 u32DataLen)
{
    HI_S32 s32Ret = 0;
    HI_U32 u32RemSize =0;
    HI_S32 s32Size    = 0;
    HI_CHAR*  ps8BufferPos = NULL;
    fd_set write_fds;
    struct timeval TimeoutVal;  /* Timeout value */
    HI_S32 s32Errno = 0;
    
    memset(&TimeoutVal,0,sizeof(struct timeval));

    u32RemSize = u32DataLen;
    ps8BufferPos = pszBuff;
    while(u32RemSize > 0)
    {
        FD_ZERO(&write_fds);
        FD_SET(s32WritSock, &write_fds);
        TimeoutVal.tv_sec = RTSP_TRANS_TIMEVAL_SEC;
        TimeoutVal.tv_usec = RTSP_TRANS_TIMEVAL_USEC;
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
                        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP response Send error:%X\n",strerror(s32Errno));
                        return HI_ERR_RTSP_SEND_FAILED;
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
                HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP response Send error:fd not in fd_set\n");
                return HI_ERR_RTSP_SEND_FAILED;
            }
        }
        /*select found over time or error happend*/
        else if( s32Ret == 0 )
        {
            s32Errno = errno;
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP response Send error: select overtime %d.%ds\n",
                        RTSP_TRANS_TIMEVAL_SEC,RTSP_TRANS_TIMEVAL_USEC);
            return HI_ERR_RTSP_SEND_FAILED;
        }
        else if( s32Ret < 0 )
        {
            s32Errno = errno;
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP response Send error:%X\n",strerror(s32Errno));
            return HI_ERR_RTSP_SEND_FAILED;
        }
        
    }

    return HI_SUCCESS;
}

HI_S32 HI_RTSP_Send_ext(RTSP_LIVE_SESSION *pSess)
{
    HI_S32 ret = 0;
    
    ret = HI_RTSP_Send(pSess->s32SessSock, pSess->au8SendBuff, strlen(pSess->au8SendBuff)) ;
    if (HI_SUCCESS != ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"to send RTSP response %d byte to cli %s thought sock %d failed\n",
                   strlen(pSess->au8SendBuff),pSess->au8CliIP,pSess->s32SessSock);
        
       ret = HI_FAILURE;
    }
    else
    {
        HI_LOG_DEBUG(RTSP_LIVESVR,LOG_LEVEL_NOTICE, "Send Reply .Chn:%s S --> C(%s) >>>>>>\n"
                       "%s\n"
                       "<<<<<<\n",
        pSess->auFileName, pSess->au8CliIP,pSess->au8SendBuff);
        ret = HI_SUCCESS;
    }
    
    HI_RTSP_ClearSendBuff(pSess);

    /*updat read to send flag as false*/
    pSess->readyToSend = HI_FALSE;

    return ret;
}

#if 0
HI_S32 HI_RTSP_Read(HI_S32 sock, HI_CHAR *pBuff, HI_U32 len, HI_S32 opt)
{
    HI_S32 ret=0;
    HI_U32 offset = 0;
    HI_S32 recvBytes = 0;
    HI_S32 s32Errno = 0;
    HI_S32 fd_max = 0;
    fd_set rfds;
    struct timeval tv;

    while(offset < len)
    {
        fd_max = sock;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        tv.tv_sec = RTSP_TRANS_TIMEVAL_SEC;
        tv.tv_usec = RTSP_TRANS_TIMEVAL_USEC;
        
        ret = select(fd_max + 1, &rfds, NULL, NULL, &tv);
        if (ret > 0)
        {
            if(FD_ISSET(sock, &rfds))
            {
                recvBytes = recv(sock, pBuff+offset, len-offset, 0);
                /*network error by zrchang  */
                if(recvBytes <= 0)
                {
                    s32Errno = errno;
                    //printf("net enviorment is so bad!\n");
                    if(recvBytes == 0)
                    {
                        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, 
                                     "recv data from sock %d error: peer shut down\r\n",sock);
                    }
                    else
                    {
                        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, 
                                     "recv data from sock %d error = %s\r\n",sock,strerror(s32Errno));
                    }
                    return HI_ERR_RTSP_READ_FAILED;        
                }

                offset += recvBytes;
            }
            else
            {
                s32Errno = errno;
                HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP response Send error:fd not in fd_set\n");
                return HI_ERR_RTSP_READ_FAILED;
            }
        }
        /*select found over time or error happend*/
        else if( ret == 0 )
        {
            s32Errno = errno;
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"Recv error: select overtime %d.%ds\n",
                        RTSP_TRANS_TIMEVAL_SEC,RTSP_TRANS_TIMEVAL_USEC);
            return HI_ERR_RTSP_READ_FAILED;
        }
        else if( ret < 0 )
        {
            s32Errno = errno;
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"Recv error:%X\n",strerror(s32Errno));
            return HI_ERR_RTSP_READ_FAILED;
        }
    }     

  return HI_SUCCESS;    
}
#endif

/* 
 func:   parse trans type in RTSP play request
input:
        pSess : RTSP session ptr;
        pTransList: trans struct list,may be several trans type,
              each trans info struct contained corresponding parsed info
        pu16ListLen: the len of pTransList
output:
        pu16ListLen: the actural trans type number*/

HI_S32 RTSP_TransInfo_Handle(RTSP_LIVE_SESSION *pSess,VOD_CLI_SUPPORT_TRANS_INFO_S* pTransList,
                             HI_U32 *pu32ListLen, HI_U32 trackid)
{
    HI_BOOL bLastTransTypeFlag = HI_FALSE;
    HI_U32 i = 0;
    HI_CHAR *p = NULL;
    HI_CHAR trash[255] = {0};
    HI_CHAR line[255] = {0};
    HI_CHAR line1[255] = {0};
    HI_CHAR line2[255] = {0};
    HI_CHAR *pLine1 = NULL;
    HI_CHAR *pLine2 = NULL;
    int Port1 = 0;
    int Port2 = 0;

    i = trackid;
    if(NULL ==  pSess || NULL == pTransList || NULL == pu32ListLen || trackid >4)
    {
        return HI_ERR_RTSP_ILLEGALE_PARAM;
    }
    
    if ((p=strstr(pSess->au8RecvBuff, RTSP_HDR_TRANSPORT)) == NULL) 
    {
		/*must has "Transport:", otherwise not accetpable*/
		return HI_ERR_RTSP_ILLEGALE_REQUEST;
	}
    
    /*    
    Transport: RTP/AVP/TCP;unicast;interleaved=0-1;source=10.71.147.222;ssrc=00003654 
    trash-- Transport:
    line: RTP/AVP/TCP;unicast;interleaved=0-1;source=10.71.147.222;ssrc=00003654 
    */
	if (sscanf(p, "%10s %255s", trash, line) != 2) 
    {
		HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"Transport formed unstandard\n");
		return HI_ERR_RTSP_ILLEGALE_REQUEST;
	}
	else
	{
        pLine1 = line;
	}

	/*assert how mang trans type contained by Transport*/
	do
	{
	    memset(line1,0,255);
        memset(line2,0,255);
        //memset(Port1,0,32);
        //memset(Port2,0,32);
        p = NULL;
	    /*if the string has no "," it means has reached the last trans type info */
	    /*line: RTP/AVP/TCP;unicast;interleaved=0-1,RTP/AVP;unicast;clientport=5000-50002
          line1: RTP/AVP/TCP;unicast;interleaved=0-1
          line2: RTP/AVP;unicast;clientport=5000-50002*/
        if (sscanf(pLine1, "%255s,%255s", line1, line2) != 2) 
        {
		    bLastTransTypeFlag = HI_TRUE;
	    }

	    pLine1 = line1;
	    pLine2 = line2;

        /*parse the trans type info*/
        /*client want to request stream packet in 'rtsp interleaved'
          and trans over tcp*/
        if ( strstr(line1, "RTP/AVP/TCP") != NULL && strstr(line1, "interleaved") != NULL)
        {
            pTransList[i].bValidFlag = HI_TRUE;
            pTransList[i].enMediaTransMode = MTRANS_MODE_TCP_ITLV;
            pTransList[i].enPackType = PACK_TYPE_RTSP_ITLV;
            pTransList[i].stTcpItlvTransInfo.s32SessSock = pSess->s32SessSock;
            /*get client's tcp receive channel: port1-- media data packet channel(rtp), 
                                                port2-- net adapt packet channel(rtcp)*/
            p = strstr(line1, "interleaved");
    		if(NULL == p )
    		{
                return HI_ERR_RTSP_ILLEGALE_REQUEST;
    		}
    		else
    		{
                //if(2 != sscanf(p,"%*[^=]=%[^-]-%[^,]",Port1,Port2))
                if(sscanf(p,"interleaved=%d-%d",&Port1,&Port2) < 1)
                {
                    //printf("--------interleaved port pase error ----------\n");
                    return HI_ERR_RTSP_ILLEGALE_REQUEST;
                }
                else
                {
                    pTransList[i].stTcpItlvTransInfo.u32ItlvCliMediaChnId = (HI_U32)Port1;
                    pTransList[i].stTcpItlvTransInfo.u32ItlvCliAdaptChnId = (HI_U32)Port2;
                }
    		}
        }
        else if (strstr(line1, "RTP/AVP") != NULL && strstr(line1, "interleaved") == NULL)
        {
            if(strstr(line1, "multicast")!= NULL)
                pTransList[i].enMediaTransMode = MTRANS_MODE_MULTICAST;
            else
                pTransList[i].enMediaTransMode = MTRANS_MODE_UDP;

            pTransList[i].bValidFlag = HI_TRUE;
            /*注:目前rtsp的udp点播按照rtp fu-a方式打包*/
            pTransList[i].enPackType = PACK_TYPE_RTP_FUA;

            /*get client's udp receive port: port1--recv media data packet(rtp), 
                 port2--recv net adapt packet(rtcp) */
            p = strstr(line1, "client_port");
            if(NULL == p )
            {
            	p = strstr(line1,"port");
				if (NULL == p)
                	return HI_ERR_RTSP_ILLEGALE_REQUEST;
				
				if(sscanf(p,"port=%d-%d",&Port1,&Port2) < 1) {
						return HI_ERR_RTSP_ILLEGALE_REQUEST ;
				}

				pTransList[i].stUdpTransInfo.u32CliMediaRecvPort = Port1;
                pTransList[i].stUdpTransInfo.u32CliAdaptRecvPort = Port2;
            }
            else
            {
                
                //if(2 != sscanf(p,"%*[^=]=%[^-]-%[^,]",Port1,Port2))
                if(sscanf(p,"client_port=%d-%d",&Port1,&Port2) < 1)
                {
                    printf("--------udp port pase error ----------\n");
                    return HI_ERR_RTSP_ILLEGALE_REQUEST;
                }
                else
                {
                    pTransList[i].stUdpTransInfo.u32CliMediaRecvPort = Port1;
                    pTransList[i].stUdpTransInfo.u32CliAdaptRecvPort = Port2;
                }
            }
        }
        /*not support tans type or not understand characters*/
        else
        {
            return HI_ERR_RTSP_ILLEGALE_REQUEST;
        }
        i++;
        pLine1 = pLine2; /*search trans info in line2  at next loop*/
        
    }  while( bLastTransTypeFlag != HI_TRUE);

    /*output param: the actural trans type number*/
     *pu32ListLen = i;
    
    return HI_SUCCESS;  
}

HI_S32 RTSP_Make_RespHdr(RTSP_LIVE_SESSION *pSess, int err)
{
    HI_RTSP_ClearSendBuff(pSess);
    
    char *pTmp = pSess->au8SendBuff;

    pTmp += sprintf(pTmp, "%s %d %s\r\n", RTSP_VER_STR, err, RTSP_Get_Status_Str( err ));
#if 0		//雄迈NVR的Cesq是从0开始的
    if(pSess->last_recv_seq != 0)
    {
    	pTmp += sprintf(pTmp, "Cseq: %d\r\n",pSess->last_recv_seq);
    }
#endif
	pTmp += sprintf(pTmp, "CSeq: %d\r\n",pSess->last_recv_seq);

	time_t timep;
	time (&timep);

	//Date: Mon, 12 Nov 2012 03:47:17 GMT
	//pTmp += sprintf(pTmp, "Date: %s GMT\r\n", ctime(&timep));
	pTmp += sprintf(pTmp, "Server: %s\r\n",pSess->au8SvrVersion);

    return (strlen(pSess->au8SendBuff)) ;
}


/*TODO*/
HI_VOID RTSP_Send_Reply(RTSP_LIVE_SESSION* pSess, int err, char *addon , int simple)
{
    char* pTmp = NULL;
    pTmp = pSess->au8SendBuff;

    if (simple == 1)
    {
        memset(pTmp,0,sizeof(RTSP_RESP_BUFFER));
        pTmp += RTSP_Make_RespHdr(pSess, err);
    }
    
    if ( addon )
    {
        pTmp += sprintf(pTmp, "%s", addon );
    }

	if(RTSP_STATUS_UNAUTHORIZED == err)
	{
		time_t timep;
		time (&timep);

		if (RTSP_AUTHENTICATE_BASIC==pSess->s32AuthenticateType)	{
			
			pTmp += sprintf(pTmp, "WWW-Authenticate: Basic realm=\".\"\r\n");
			pTmp += sprintf(pTmp, "Date: %s GMT\r\n",ctime(&timep));
		}
		else if (RTSP_AUTHENTICATE_DIGEST==pSess->s32AuthenticateType) {			
			pTmp += sprintf(pTmp, "Date: %s GMT\r\n", ctime(&timep));
			pTmp += sprintf(pTmp, "WWW-Authenticate: Digest realm=\"LIVE555 Streaming Media\", nonce=\"70b16a51eb135f69c972231f4dbeff5e\"\r\n");
		}
	}
    if (simple)
        strcat( pSess->au8SendBuff, RTSP_LRLF );

    pSess->readyToSend = HI_TRUE;
    return;
}

HI_VOID RTSP_SendErrResp(HI_S32 s32LinkFd,HI_S32 RTSP_err_code)
{
    HI_S32 s32Ret = 0;
    HI_CHAR  OutBuff[512] = {0};
    HI_CHAR *pTemp = NULL;
    
    /*如果前面任何一个步骤出现错误，则组织并返回返回错误信息并关闭socket*/
    pTemp = OutBuff;
    pTemp += sprintf(pTemp, "%s %d %s\r\n", RTSP_VER_STR, RTSP_err_code,
                    RTSP_Get_Status_Str( RTSP_err_code ));
    pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");
    pTemp += sprintf(pTemp,"\r\n");


    /*发送回应信息*/
    s32Ret = HI_RTSP_Send(s32LinkFd,OutBuff,strlen(OutBuff));	/* bad request */
    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"to send RTSP pause response %d byte thought sock %d failed\n",
                   strlen(OutBuff),s32LinkFd);
    }
    else
    {
        
        HI_LOG_DEBUG(RTSP_LIVESVR,LOG_LEVEL_NOTICE, "Send Reply .S --> C >>>>>>\n"
                       "%s\n"
                       "<<<<<<\n",
                       OutBuff);
    }

    /*关闭该链接*/
    close(s32LinkFd);
    
}
HI_S32 RTSP_Handle_Options(RTSP_LIVE_SESSION *pSess )
{
    int i = 0;
    HI_S32 ret = 0;
    HI_S32 Cseq = 0;
	HI_CHAR *pTemp = NULL;
	HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH] = {0};
	
    /*获取CSeq信息*/
    ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    pSess->last_recv_seq= Cseq;
#if 0
	/*获取sessionid*/
	ret = RTSP_Get_SessId(pSess->au8RecvBuff,aszSessID);
	if(HI_SUCCESS != ret)
	{
		printf("<RTSP>RTSP_Get_SessId failed:%x",ret);
		strncpy(aszSessID,pSess->aszSessID,RTSP_MAX_SESSID_LEN);
		RTSP_Send_Reply(pSess, 400, NULL, 1);
		return HI_FAILURE;
	}
#endif	
    HI_VOD_GetVersion(pSess->au8SvrVersion, RTSP_MAX_VERSION_LEN);
    i = RTSP_Make_RespHdr(pSess, 200);    
  	sprintf(pSess->au8SendBuff+ i , "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY,SET_PARAMETER, GET_PARAMETER\r\n\r\n");   
    RTSP_Send_Reply(pSess, 200, NULL, 0);    
    return HI_SUCCESS;
}

/*

DESCRIBE rtsp://10.85.180.240/sample_h264_100kbit.mp4 RTSP/1.0

CSeq: 1

Accept: application/sdp

Bandwidth: 384000

Accept-Language: hr-HR

User-Agent: QuickTime/7.0.3 (qtver=7.0.3;os=Windows NT 5.1Service Pack 2)

///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 1

Last-Modified: Mon, 07 Mar 2005 08:02:14 GMT

Cache-Control: must-revalidate

Content-length: 682

Date: Wed, 29 Oct 2008 06:16:43 GMT

Expires: Wed, 29 Oct 2008 06:16:43 GMT

Content-Type: application/sdp

x-Accept-Retransmit: our-retransmit

x-Accept-Dynamic-Rate: 1

Content-Base: rtsp://10.85.180.240/sample_h264_100kbit.mp4/



v=0

o=StreamingServer 3434249851 1110182534000 IN IP4 10.85.180.240

s=\sample_h264_100kbit.mp4

u=http:///

e=admin@

c=IN IP4 0.0.0.0

b=AS:2097172

t=0 0

a=control:*

a=isma-compliance:2,2.0,2

a=range:npt=0-  70.00000

m=video 0 RTP/AVP 96

b=AS:2097151

a=rtpmap:96 H264/90000

a=control:trackID=3

a=cliprect:0,0,242,192

a=framesize:96 192-242

a=fmtp:96 packetization-mode=1;profile-level-id=4D400C;sprop-parameter-sets=J01ADKkYYELxCA==,KM4JiA==

a=mpeg4-esid:201

m=audio 0 RTP/AVP 97

b=AS:20

a=rtpmap:97 mpeg4-generic/8000/2

a=control:trackID=4

a=fmtp:97 profile-level-id=15;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3;config=1590

a=mpeg4-esid:101

*/

void get_bandwidth(int *all, int *v)
{
    
    FILE* fp = NULL;
    fp = fopen("bandwidth", "r");
    if (fp)
    {
	    fscanf(fp , "%d %d", all, v);
        printf("get Bandwidth, all:%d, v:%d\n", *all, *v);
    	fclose(fp);
    }
    else
	{
		perror("open file error\n");
	}
    
}

HI_U32 RTSP_SDP_AAC_CONFIG_Computer(HI_U32 SamepleRate,HI_U32 AU_CHN,HI_U32 profile,HI_CHAR * strConfig,HI_S32 strlen)
{
	HI_U32 sampling_frequency_index = 0;
	switch (SamepleRate )
		{
			case 96000:
				sampling_frequency_index = 0x0;
				break;
			case 88200:
				sampling_frequency_index = 0x1;
				break;
			case 64000:
				sampling_frequency_index = 0x2;
				break;
			case 48000:
				sampling_frequency_index = 0x3;
				break;
			case 44100:
				sampling_frequency_index = 0x4;
				break;
			case 32000:
				sampling_frequency_index = 0x5;
				break;
			case 24000:
				sampling_frequency_index = 0x6;
				break;
			case 22050:
				sampling_frequency_index = 0x7;
				break;
			case 16000:
				sampling_frequency_index = 0x8;
				break;
			case 12000:
				sampling_frequency_index = 0x9;
				break;
			case 11025:
				sampling_frequency_index = 0xa;
				break;
			case 8000:
				sampling_frequency_index = 0xb;
				break;			
			default:
				printf("SamepleRate err =%u\n",SamepleRate);
		}
	HI_U8 audioConfig[2] = {0};
	HI_U8 audioObjectType = profile +1;
	audioConfig[0] = (audioObjectType<<3) | (sampling_frequency_index>>1); 
	audioConfig[1] = (sampling_frequency_index<<7) | (AU_CHN<<3);
	snprintf(strConfig,strlen,"%02x%02x",audioConfig[0], audioConfig[1]);
	printf ("RTSP_SDP_AAC_CONFIG_Computer %s %d \n",strConfig,strlen);
	return 0;
	
}

HI_S32 RTSP_Handle_Discrible(RTSP_LIVE_SESSION *pSess )
{
    HI_S32   ret = 0;
    RTSP_URL url;
    HI_S32 Cseq = 0;
    char*   pTemp = NULL;
    char    sdpMsg[1024*2] = {0};
    
    memset(url,0,256);
    HI_VOD_GetVersion(pSess->au8SvrVersion, RTSP_MAX_VERSION_LEN);
    
    if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
    {
	    RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
	    return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }

    url_info urlinfo;
    memset(&urlinfo, 0, sizeof(urlinfo));
    ret = RTSP_Parse_Url(url, &urlinfo);
    if (ret == HI_FAILURE)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }
    /*获取客户端请求媒体的描述方法 application/sdp*/
    if (strstr(pSess->au8RecvBuff, RTSP_HDR_ACCEPT) != NULL) 
    {
        if (strstr(pSess->au8RecvBuff, "application/sdp") == NULL) 
        {
            HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"Only accept require.\n");
            RTSP_Send_Reply(pSess, 551, NULL, 1);	/* Option not supported */
            return HI_ERR_RTSP_ILLEGALE_REQUEST;
        }
    }

    if(urlinfo.playback == 0)
    {
        if(urlinfo.filename[0] == '\0')
            pSess->s32VsChn = 11;
        else
            pSess->s32VsChn = atoi(urlinfo.filename);
        pSess->bIsPlayback = HI_FALSE;
    }
    else
    {
        strcpy(pSess->auFileName,urlinfo.filename);
        pSess->bIsPlayback = HI_TRUE;
        if(strstr(urlinfo.filename, "date="))
        {
            pSess->s32PlayBackType = 1;
        }
    }

    /*get User-Agent: VLC media player (LIVE555 Streaming Media v2006.03.16)*/
    HI_U32 u32UserAgentLen = RTSP_MAX_VERSION_LEN;
    ret = RTSP_GetUserAgent(pSess->au8RecvBuff, pSess->au8UserAgent,&u32UserAgentLen);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "error user-agent: %d\n",ret);
    }
    
    /*获取CSeq信息*/
    ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if (ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }
    else
    {
        pSess->last_recv_seq= Cseq;
    }

	/*获取设置的密码校验方式*/
	ret=RTSP_GetUserAuthenticateType();
	printf("%s,%d RTSP Authorization type=%d\n",__FILE__,__LINE__,ret);
	if (ret<0 || ret>2)
		ret = 0;
	pSess->s32AuthenticateType=ret;
	printf("%s,%d RTSP Authorization type=%d\n",__FILE__,__LINE__,ret);
    
    /*获取用户名及密码*/
    HI_CHAR au8UserName[256] = {0};            //用户名
    HI_CHAR au8PassWord[256] = {0};            //密码
    HI_U32 u32UserLen = 256;
    HI_U32 u32PassLen = 256;
	printf("#### %s %d, server:%s,filename:%s\n", __FUNCTION__, __LINE__, urlinfo.server, urlinfo.filename);
    ret = RTSP_GetUserName(pSess->au8RecvBuff, &urlinfo, au8UserName, &u32UserLen, au8PassWord, &u32PassLen);
    if(HI_RTSP_PARSE_NO_AUTHORIZATION == ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_NOTICE, "Un Authorization: %s\n", url);
		if (RTSP_AUTHENTICATE_NONE!=pSess->s32AuthenticateType) {
			RTSP_Send_Reply(pSess, RTSP_STATUS_UNAUTHORIZED, NULL, 1);
			return HI_SUCCESS;
		}
    }
	else if (HI_RTSP_PARSE_INVALID_AUTHORIZATION ==ret)
	{
		if (RTSP_AUTHENTICATE_NONE!=pSess->s32AuthenticateType) {
			RTSP_Send_Reply(pSess, RTSP_STATUS_UNAUTHORIZED, NULL, 1);
			return HI_SUCCESS;
		}
		
	}
	else if(HI_SUCCESS != ret)
	{
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "invalid Authorization: %s\n", url);
        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }
    
    
    /*向vod请求媒体信息*/
    VOD_DESCRIBE_MSG_S  stDesReqInfo; 
    VOD_ReDESCRIBE_MSG_S stDesRespInfo;
    memset(&stDesReqInfo,0,sizeof(VOD_DESCRIBE_MSG_S));
    memset(&stDesRespInfo,0,sizeof(VOD_ReDESCRIBE_MSG_S));
    strncpy(stDesReqInfo.au8MediaFile,urlinfo.filename,128);
    strncpy(stDesReqInfo.au8UserName,au8UserName,u32UserLen);
    strncpy(stDesReqInfo.au8PassWord,au8PassWord,u32UserLen);
    strncpy(stDesReqInfo.au8ClientVersion,pSess->au8UserAgent,u32UserAgentLen);
    
    if(urlinfo.playback==0)
        stDesReqInfo.bLive = HI_TRUE;
    else
        stDesReqInfo.bLive = HI_FALSE;

    if(pSess->s32PlayBackType == 1)
    {
        stDesReqInfo.s32PlayBackType = 1;
    }
	
    ret = HI_VOD_DESCRIBE(&stDesReqInfo, &stDesRespInfo);

    DEBUGPrtVODDes(&stDesReqInfo, &stDesRespInfo);
    if(ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "vod describe failed %X(client %s)\n",
                   ret,pSess->au8CliIP);
        RTSP_Send_Reply(pSess, RTSP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        return ret;
    }
    /*RTSP会话结构保存server的版本信息*/
    else
    {
       // strncpy(pSess->au8SvrVersion,stDesRespInfo.au8BoardVersion,RTSP_MAX_VERSION_LEN-1);
       // pSess->au8SvrVersion[RTSP_MAX_VERSION_LEN-1] = '\0';
    }

    if(pSess->s32PlayBackType == 1)
    {
        strcpy(pSess->RealFileName, stDesRespInfo.RealFileName);
        pSess->start_offset = stDesRespInfo.start_offset;
    }
    
    pTemp = pSess->au8SendBuff + RTSP_Make_RespHdr(pSess, 200);    
    pTemp += sprintf(pTemp,"Content-Type: application/sdp\r\n");   
    //TODO 把一些固定值改为动态值   
#if 0
	//华拓智能
	char SerialNo[64] = {0};
	ret = HI_BufAbstract_GetNetPt_13_Info(SerialNo);
	if(ret == 0)
	{
		pTemp += sprintf(pTemp,"AdditionalHead: %s\r\n", SerialNo);
	}
#endif
#if 0
  //上海无线
    sprintf( achSDPMsg, "v=0\r\no=- 1 1 IN IP4 %s\r\n", IPAddress);
	strcat(  achSDPMsg, "s=Live Media\r\n" );
	sprintf(pstrIPTemp,"c=IN IP4 %s\r\n",IPAddress);
	strcat(  achSDPMsg, pstrIPTemp);
	strcat(  achSDPMsg, "t=0 21\r\n" );
	strcat(  achSDPMsg, "m=video 0 RTP/AVP 96\r\n");
	strcat(  achSDPMsg, "a=control:video\r\n" );
	strcat(  achSDPMsg, "a=rtpmap:96 H264/90000\r\n" );
	strcat(  achSDPMsg, "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=" );
	strcat(  achSDPMsg, achRlt );
	strcat(  achSDPMsg, "\r\n\r\n");
#endif
    HI_S32 bandSum=0;
    HI_S32 bandVideo=0;

    //get_bandwidth(&bandSum, &bandVideo);
    
    HI_U32 bandAudio=0;

    bandVideo = stDesRespInfo.u32Bitrate/1000;
    bandAudio= stDesRespInfo.u32AudioSampleRate/1000;
    bandSum = (bandVideo + bandAudio);
    printf("<RTSP> DISCRIBLE the band all:%d video :%d audio:%d \n",bandSum,bandVideo,bandAudio);
    
    char *pTemp2 = sdpMsg;    
    pTemp2 += sprintf(pTemp2,"v=0\r\n");

    //pTemp2 += sprintf(pTemp2,"o=StreamingServer 3331435948 1116907222000 IN IP4 %s\r\n", pSess->au8HostIP); 
    //pTemp2 += sprintf(pTemp2,"o=StreamingServer 3433055887 1360307302917838 IN IP4 %s\r\n", pSess->au8HostIP); 
    
    //pTemp2 += sprintf(pTemp2, "o=- 72199192344 1 IN IP4 192.168.1.123\r\n");   //topsee
    //pTemp2 += sprintf(pTemp2, "o=- 1418744474352605 1418744474352605 1 IN IP4 %s\r\n", pSess->au8HostIP);   //topsee
    pTemp2 += sprintf(pTemp2, "o=- 1418744474352605 1418744474352605 IN IP4 %s\r\n", pSess->au8HostIP);

#if 0
    pTemp2 += sprintf(pTemp2,"s=\\%s\r\n",au8FileInfo);    
#else
    //pTemp2 += sprintf(pTemp2,"s=rtsp session\r\ne=NONE\r\n");   
    pTemp2 += sprintf(pTemp2,"s=Media Presentation\r\ne=NONE\r\n");   
	pTemp2 += sprintf(pTemp2,"b=AS:5100\r\n"); 

    //pTemp2 += sprintf(pTemp2, "s=RTSP/RTP stream from Network Video Server\r\n");   //topsee
    pTemp2 += sprintf(pTemp2, "i=mpeg4\r\n");   //topsee
    pTemp2 += sprintf(pTemp2, "t=0 0\r\n"); 
    pTemp2 += sprintf(pTemp2, "a=tool:LIVE555 Streaming Media v2009.09.28\r\n");   //topsee
    pTemp2 += sprintf(pTemp2, "a=type:broadcast\r\n");   //topsee
   	pTemp2 += sprintf(pTemp2, "a=control:*\r\n");	//topsee
    pTemp2 += sprintf(pTemp2, "a=range:npt=0-\r\n");	//topsee
    pTemp2 += sprintf(pTemp2, "a=x-qt-text-nam:RTSP/RTP stream from Network Video Server\r\n");	//topsee
    pTemp2 += sprintf(pTemp2, "a=x-qt-text-inf:mpeg4\r\n");	//topsee
#endif

    if(urlinfo.multicast)
        pTemp2 += sprintf(pTemp2,"c=IN IP4 239.1.2.3/127\r\n");
    else
        pTemp2 += sprintf(pTemp2,"c=IN IP4 0.0.0.0\r\n");   //使用 IPv4 

    //pTemp2 += sprintf(pTemp2,"b=AS:%d\r\n",bandSum); 
 //   pTemp2 += sprintf(pTemp2,"t=0 0\r\n");    
    //pTemp2 += sprintf(pTemp2,"a=control:*\r\n"); 
    pTemp2 += 	sprintf(pTemp2,"a=range:npt=now- \r\n");
	//pTemp2 +=	sprintf(pTemp2,"c=IN IP4 0.0.0.0\r\n");


    /*视频媒体描述*/    
    /*H264 TrackID=0 RTP_PT 96*/
    if(MT_VIDEO_FORMAT_MJPEG == stDesRespInfo.enVideoType)
    {
        pTemp2 += sprintf(pTemp2,"m=video 0 RTP/AVP 26\r\n");
    }
    else//这里有点问题，不是MJPEG的码流都按H264处理，以后按需求修改。
    {
        pTemp2 += sprintf(pTemp2,"m=video 0 RTP/AVP 96\r\n");
    }

	
	//pTemp2 +=sprintf(pTemp2,"a=framerate:30.000000\r\n");
    /*
    if(bPlayback)
    {
        float sec = (float)stDesRespInfo.u32TotalTime/1000;
        if(pSess->s32PlayBackType == 1)
        {
            sec -= pSess->start_offset;
        }
        pTemp2 += sprintf(pTemp2,"a=range:npt=0-%f\r\n", sec);
    }
    */
    
    bandVideo = 5000;
    //该字串中AS位数据流大小
    pTemp2 += sprintf(pTemp2,"b=AS:%d\r\n",bandVideo); 
    pTemp2 += sprintf(pTemp2,"a=control:trackID=%d\r\n",RTSP_TRACKID_VIDEO);
    if(MT_VIDEO_FORMAT_MJPEG == stDesRespInfo.enVideoType)
    {
        pTemp2 += sprintf(pTemp2,"a=rtpmap:26 JPEG/90000\r\n");
    }
    else
	if (MT_VIDEO_FORMAT_H265== stDesRespInfo.enVideoType)
	{
		pTemp2 += sprintf(pTemp2,"a=rtpmap:96 H265/90000\r\n");
		pTemp2 += sprintf(pTemp2,"a=x-dimensions: %d, %d\r\n",stDesRespInfo.u32VideoPicWidth,stDesRespInfo.u32VideoPicHeight);
	}
	else
    {
        pTemp2 += sprintf(pTemp2,"a=rtpmap:96 H264/90000\r\n"); 
    }
	
#if 1
    if(MT_VIDEO_FORMAT_H264 == stDesRespInfo.enVideoType)
    {
        pTemp2 += sprintf(pTemp2,"a=fmtp:96 profile-level-id=%s; packetization-mode=1; sprop-parameter-sets=%s\r\n", 
            stDesRespInfo.profile_level_id,stDesRespInfo.au8VideoDataInfo);//profile-level-id=42e032; 
    }
#else
    pTemp2 += sprintf(pTemp2,"a=fmtp:96 packetization-mode=1; sprop-parameter-sets=%s\r\n"
       , stDesRespInfo.au8VideoDataInfo);
#endif
    if(MT_VIDEO_FORMAT_H264 == stDesRespInfo.enVideoType && urlinfo.playback==0)
    {
        pTemp2 += sprintf(pTemp2,"a=framesize:96 %d-%d\r\n",stDesRespInfo.u32VideoPicWidth,stDesRespInfo.u32VideoPicHeight);
        pTemp2 += sprintf(pTemp2,"a=x-dimensions: %d, %d\r\n",stDesRespInfo.u32VideoPicWidth,stDesRespInfo.u32VideoPicHeight);//topsee
		pTemp2 += sprintf(pTemp2,"a=x-framerate: %d\r\n",stDesRespInfo.u32Framerate>30 ? 30 : stDesRespInfo.u32Framerate);//topsee
    }


    /*音频媒体描述*/    
    /*G711*/   
    /*TODO 音频*/
    if(urlinfo.playback==0)
    {
        ret = HI_BufAbstract_GetAudioEnable(pSess->s32VsChn);
    }
    else
    {
        if(pSess->s32PlayBackType==0)
            ret = BufAbstract_Playback_GetAudioEnable(pSess->auFileName);
        else
            ret = BufAbstract_Playback_GetAudioEnable(pSess->RealFileName);
        ret = 0;
    }
    
	if (stDesRespInfo.enAudioType == MT_AUDIO_FORMAT_AAC)
	{
		if (ret)
		{
			 pTemp2 += sprintf(pTemp2,"m=audio 0 RTP/AVP 104\r\n");	
			 pTemp2 += sprintf(pTemp2,"b=AS:50\r\n");	
        	 pTemp2 += sprintf(pTemp2,"a=control:trackID=%d\r\n",RTSP_TRACKID_AUDIO);
#if 1		
			//IPC AAC 参数
       		 pTemp2 += sprintf(pTemp2,"a=rtpmap:104 mpeg4-generic/%u/%u\r\n",stDesRespInfo.u32AudioSampleRate,stDesRespInfo.u32AudioChannelNum);
			 char strconfig[8] = {0};
			 RTSP_SDP_AAC_CONFIG_Computer(stDesRespInfo.u32AudioSampleRate,stDesRespInfo.u32AudioChannelNum,1,(HI_CHAR*)&strconfig,sizeof(strconfig));
			 pTemp2 += sprintf(pTemp2,"a=fmtp:104 streamtype=5; profile-level-id=15; mode=AAC-hbr; config=%s; SizeLength=13;IndexLength=3; IndexDeltaLength=3; Profile=1;\r\n",strconfig);
#else
			// 为傅工修改的一个版本  采样率  44100  单声道 1
			pTemp2 += sprintf(pTemp2,"a=rtpmap:104 mpeg4-generic/48000/1\r\n");
			pTemp2 += sprintf(pTemp2,"a=fmtp:104 streamtype=5; profile-level-id=15; mode=AAC-hbr; config=1188; SizeLength=13;IndexLength=3; IndexDeltaLength=3; Profile=1;\r\n");			
			//pTemp2 += sprintf(pTemp2,"a=fmtp:104 streamtype=5; profile-level-id=15; mode=AAC-hbr; object=1;cpresent=0;config=1210\r\n");			
			
			
		
#endif
			
			 
		}
	}
	else
		{
			  if(ret)    
			  {
			        pTemp2 += sprintf(pTemp2,"m=audio 0 RTP/AVP 8\r\n");	
					pTemp2 += sprintf(pTemp2,"b=AS:50\r\n");	
			        pTemp2 += sprintf(pTemp2,"a=control:trackID=%d\r\n",RTSP_TRACKID_AUDIO);
			        pTemp2 += sprintf(pTemp2,"a=rtpmap:8 PCMA/8000\r\n");
			  }
		}
	//494D4B48010100000400010000000000000000000000000000000000000000000000000000000000	video
	//494D4B48010100000400010010710110401F000000FA000000000000000000000000000000000000	video+audio
	//海康文件头
	if (MT_VIDEO_FORMAT_H265== stDesRespInfo.enVideoType)
		pTemp2 += sprintf(pTemp2,"a=Media_header:MEDIAINFO=494D4B48010100000400050000000000000000000000000000000000000000000000000000000000;\r\n");
	else
		pTemp2 += sprintf(pTemp2,"a=Media_header:MEDIAINFO=494D4B48010100000400010000000000000000000000000000000000000000000000000000000000;\r\n");
	pTemp2 += sprintf(pTemp2,"a=appversion:1.0\r\n");

    //pTemp += sprintf(pTemp,"Cache-Control: must-revalidate\r\n");
    pTemp += sprintf(pTemp,"Content-Length: %d\r\n\r\n", strlen(sdpMsg));         
 //   pTemp += sprintf(pTemp,"Content-Base: %s\r\n\r\n", url);

    printf("#### %s %d, pTemp2:%s\n", __FUNCTION__, __LINE__, pTemp2);
    strcat(pTemp, sdpMsg);
    RTSP_Send_Reply(pSess, 200, NULL, 0);

    return HI_SUCCESS;
}

/*
SETUP rtsp://10.85.180.240/sample_h264_100kbit.mp4/trackID=4 RTSP/1.0

CSeq: 3

Transport: RTP/AVP;unicast;client_port=6972-6973

x-retransmit: our-retransmit

x-dynamic-rate: 1

x-transport-options: late-tolerance=0.400000

Session: 115302692050290

User-Agent: QuickTime/7.0.3 (qtver=7.0.3;os=Windows NT 5.1Service Pack 2)

Accept-Language: hr-HR

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 3

Session: 115302692050290

Last-Modified: Mon, 07 Mar 2005 08:02:14 GMT

Cache-Control: must-revalidate

Date: Wed, 29 Oct 2008 06:16:43 GMT

Expires: Wed, 29 Oct 2008 06:16:43 GMT

Transport: RTP/AVP;unicast;source=10.85.180.240;client_port=6972-6973;server_port=6970-6971;ssrc=0000711E

x-Transport-Options: late-tolerance=0.400000

x-Retransmit: our-retransmit

x-Dynamic-Rate: 1
*/


HI_S32 RTSP_Handle_Setup(RTSP_LIVE_SESSION *pSess )
{
    char* pTemp;
    char* p = NULL;
    char trash[255];
    char line[255];
    char url[255];
    int rtpport=0, rtcpport=0;

    int  trackid;
    HI_S32 ret;
    int iCseq;
    HI_U32 SSRC =0 ;
    //int IsTurnRelay = 0;
    char destaddr[64]={0};
    
    memset(url,0,255);
    //mhua change
   /*获取CSeq信息*/
    ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&iCseq);
    if (iCseq > 0 )
    {
        pSess->last_recv_seq= iCseq;
        printf("Cseq: %d \n", iCseq);
    }
    else
    {
        printf("invalid cseq no.\n");
    }
    HI_VOD_GetVersion(pSess->au8SvrVersion, RTSP_MAX_VERSION_LEN);
    /*获取客户端指定的rtp传输端口信息*/
   
	if ((p = strstr(pSess->au8RecvBuff, "client_port")) == NULL && 
        strstr(pSess->au8RecvBuff, "RTP/AVP/TCP") == NULL && strstr(pSess->au8RecvBuff,"multicast") == NULL )    
    {
		RTSP_Send_Reply(pSess, 406, "Require: Transport settings of rtp/udp;client_port=nnnn. ", 1);	/* Not Acceptable */
		return HI_ERR_RTSP_ILLEGALE_REQUEST;
	}

    /*获取Transport Token*/
	if ((p = strstr(pSess->au8RecvBuff, RTSP_HDR_TRANSPORT)) == NULL) 
    {
		RTSP_Send_Reply(pSess, 406, "Require: Transport settings of rtp/udp;port=nnnn. ", 1);	/* Not Acceptable */
		return HI_ERR_RTSP_ILLEGALE_REQUEST;
	}

    /*    
    Transport: RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;server_port=6970-6971;ssrc=00003654 
    trash-- Transport:
    line: RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;server_port=6970-6971;ssrc=00003654 
    */

	if (sscanf(p, "%10s%255s", trash, line) != 2) 
    {
		//WRITE_LOG_ERROR(MODULE_RTSP_STR"SETUP request malformed\n");
		RTSP_Send_Reply(pSess, 400, 0, 1);	/* Bad Request */
		printf("DEBUG %s %d send 400\n",__FILE__,__LINE__);
		return HI_ERR_RTSP_ILLEGALE_REQUEST;
	}
	

    /*获取客户端的rtp传输端口*/
    if( (p = strstr(line, "client_port") )!= NULL)
    {
        ret = sscanf(p, "client_port=%d-%d", &rtpport, &rtcpport);
        if(p = strstr(pSess->au8RecvBuff, "destination="))
        {
            //IsTurnRelay = 1;
            sscanf(p, "destination=%[^; \r\n]", destaddr);
            //pSess->bIsTurnRelay = HI_TRUE;
            strcpy(pSess->au8DestIP, destaddr);
        }
    }

    /* Get the URL */
    if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
    {
        RTSP_Send_Reply(pSess, 400, 0, 1);	/* bad request */
		printf("DEBUG %s %d send 400\n",__FILE__,__LINE__);
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }

    /* Validate the URL */


    //UpdateUrl(pSess->au8RecvBuff);

    url_info urlinfo;
    memset(&urlinfo, 0, sizeof(urlinfo));
    if (RTSP_Parse_Url(url, &urlinfo) != HI_SUCCESS) 
    {   
        RTSP_Send_Reply(pSess, 400, 0, 1);	/* bad request */
		printf("DEBUG %s %d send 400\n",__FILE__,__LINE__);
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }	
	
    /*获取trackID*/
    p = strstr(url, "trackID");
    if (p == NULL)
    {
       // WRITE_LOG_ERROR(MODULE_RTSP_STR"No track info.\n");
        /*ToDo 发什么回复*/
	    printf("DEBUG %s,%d not trackID url=%s\n",__FILE__,__LINE__,url);
        RTSP_Send_Reply(pSess, 406, "Require: Transport settings of rtp/udp;port=nnnn. ", 1);	/* Not Acceptable */
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }

    sscanf(p, "%8s%d", trash, &trackid);
    /* 0/trackid=6 */
    #if 0
    p = strstr(object, "/");
    if (p == NULL)
    {
        chn = 11;
        //strncpy(au8Chn,"11",2);
        //RTSP_Send_Reply(pSess, 400, NULL, 1);
        //return HI_FAILURE;
    }
    else
    {
    	int t1, t2;

        /** 在该段增加解析思科协议的rtsp请求 **/
        if(NULL != strstr(object, "av"))
        {
            sscanf(object, "av%d_%d.*/%*s", &t1, &t2);
            chn = (t1+1)*10+t2+1;
            printf("======== chn:%d\n", chn);
        }
        else
        {
            sscanf(object, "%d.*/%*s", &chn);
        }


        if (chn <=0 || chn > 323)
        {
            chn = 11;
        }
        //sscanf(object, "%s/%s", au8Chn,trash);
    }
    #endif
    
    if(urlinfo.playback == 0)
    {
        printf("rtsp setup: chn %d traID %d pSess:%d 0:%d 1:%d\n ",pSess->s32VsChn,trackid,
    		pSess,pSess->bRequestStreamFlag[0],pSess->bRequestStreamFlag[1]);
    }
    else
    {
        printf("%s,file name:%s,trackid:%d\n",__FUNCTION__, pSess->auFileName, trackid);
    }
    
    if(trackid == RTSP_TRACKID_VIDEO)
    {
         /*如果video已经setup过，不允许重复setup*/
        if(pSess->bRequestStreamFlag[0] == HI_TRUE)
        {
            HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "setup video request repeat (client %s)\n",pSess->au8CliIP);
            RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
            return HI_ERR_RTSP_ILLEGALE_REQUEST;
        }   
        pSess->remoteRTPPort[0] = rtpport;
        pSess->remoteRTCPPort[0] = rtcpport ;
        pSess->bRequestStreamFlag[0]= HI_TRUE;
        printf("setup the url is :%s \n",url);
        strncpy(pSess->au8CliUrl[0],url,strlen(url));
		printf("#### %s %d\n", __FUNCTION__, __LINE__);
    }
    else
    {
        /*如果audio已经setup过，不允许重复setup*/
        if(pSess->bRequestStreamFlag[1] == HI_TRUE)
	    {
	        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "setup audio request repeat (client %s)\n",pSess->au8CliIP);
	        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
			printf("DEBUG %s %d send 400\n",__FILE__,__LINE__);
            return HI_ERR_RTSP_ILLEGALE_REQUEST;
	    } 
        pSess->remoteRTPPort[1] = rtpport;
        pSess->remoteRTCPPort[1] = rtcpport ;
        pSess->bRequestStreamFlag[1]= HI_TRUE;
        strncpy(pSess->au8CliUrl[1],url,strlen(url));
    }
    
    //create the sessionID
  	//mtrans_random_id( pSess->sessID, 15 );

    /*获取用户名及密码*/
    HI_CHAR au8UserName[256] = {0};            //用户名
    HI_CHAR au8PassWord[256] = {0};            //密码
    HI_U32 u32UserLen = 256;
    HI_U32 u32PassLen = 256;
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    if( RTSP_GetUserName(pSess->au8RecvBuff, &urlinfo, au8UserName, &u32UserLen, au8PassWord, &u32PassLen) < 0)
    {
        
		if (RTSP_AUTHENTICATE_NONE!=pSess->s32AuthenticateType) {
	        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
			printf("DEBUG %s %d send 400\n",__FILE__,__LINE__);
	        return HI_ERR_RTSP_ILLEGALE_REQUEST;
		}
    }
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    VOD_SETUP_MSG_S struVodSetupReqInfo;
    VOD_ReSETUP_MSG_S struVodSetupRespInfo;
    memset(&struVodSetupReqInfo, 0, sizeof(struVodSetupReqInfo));
    strcpy(struVodSetupReqInfo.aszSessID ,pSess->aszSessID);
    if(pSess->au8DestIP[0] != 0)
        strcpy(struVodSetupReqInfo.au8ClientIp,pSess->au8DestIP);
    else
        strcpy(struVodSetupReqInfo.au8ClientIp,pSess->au8CliIP);
    
    strncpy(struVodSetupReqInfo.au8UserName, au8UserName, VOD_MAX_STRING_LENGTH);
    strncpy(struVodSetupReqInfo.au8PassWord, au8PassWord, VOD_MAX_STRING_LENGTH);


	printf("#### %s %d, pSess->bIsPlayback=%d\n", __FUNCTION__, __LINE__, pSess->bIsPlayback);
    if(pSess->bIsPlayback == 0)
    {
        sprintf(struVodSetupReqInfo.au8MediaFile,"%d",pSess->s32VsChn);
		struVodSetupReqInfo.bIsPlayBack = 0;
    }
    else
    {
        struVodSetupReqInfo.bIsPlayBack = 1;
        if(pSess->s32PlayBackType == 0)
        {
            strcpy(struVodSetupReqInfo.au8MediaFile, pSess->auFileName);
            struVodSetupReqInfo.s32PlayBackType = 0;
        }
        else
        {
            strcpy(struVodSetupReqInfo.au8MediaFile, pSess->RealFileName);
            struVodSetupReqInfo.s32PlayBackType = 1;
        }
    }
    
    printf("sprintf au8MediaFile = %s\n",struVodSetupReqInfo.au8MediaFile);
    
    struVodSetupReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP;
    
    if (trackid == RTSP_TRACKID_VIDEO )
    {
        struVodSetupReqInfo.u8SetupMeidaType = 0x01;
    }
    else if (trackid == RTSP_TRACKID_AUDIO)
    {
        struVodSetupReqInfo.u8SetupMeidaType = 0x02;
    }
    
    //strcpy(struVodSetupReqInfo.au8ClientVersion,pSess->au8UserAgent);
    strncpy(struVodSetupReqInfo.au8ClientVersion, pSess->au8UserAgent, VOD_MAX_VERSION_LEN);

    /*解析client端的传输方式*/
    HI_U32 u32TransListLen = VOD_MAX_TRANSTYPE_NUM;
    ret =  RTSP_TransInfo_Handle(pSess,struVodSetupReqInfo.astCliSupptTransList,&u32TransListLen, trackid);
    if(ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X(client %s)\n",
                   ret,pSess->au8CliIP);
        RTSP_Send_Reply(pSess, 503, NULL, 1);
        return ret;
    }
    SSRC = (HI_U32)random();
    printf("the ssrc is :%d %08x\n",SSRC,SSRC);
    struVodSetupReqInfo.u32Ssrc = SSRC;
    
    //if((pSess->bRequestStreamFlag[0] == HI_TRUE)&&(pSess->bRequestStreamFlag[1] == HI_TRUE))
    //如果setup请求中带Session字段，则认为是第二次setup请求
    if( NULL != strstr(pSess->au8RecvBuff,"Session"))
    {
        struVodSetupReqInfo.resv1 = 1;
        printf("rtsp handle setup two time \n");
        //ret = HI_VOD_SetUp(&struVodSetupReqInfo, &struVodSetupRespInfo);    
    }
    else 
    {
        struVodSetupReqInfo.resv1 = 0;
    }

    /**注册视频回调函数**/
    ret = HI_VOD_SetUp(&struVodSetupReqInfo, &struVodSetupRespInfo); 
    DEBUGPrtVODSetup(&struVodSetupReqInfo,&struVodSetupRespInfo);
    if(ret != HI_SUCCESS)
    {
        if(HI_ERR_VOD_ILLEGAL_USER == ret )
        {
            HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X:illegal user %s(%s %s)\n",
                   ret,pSess->au8CliIP,au8UserName,au8PassWord);
            RTSP_Send_Reply(pSess, RTSP_STATUS_UNAUTHORIZED, NULL, 1);
        }
    	else if(HI_ERR_VOD_OVER_CONN_NUM == ret )
    	{
    	    HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X:over max usernumber for cli %s\n",
                       ret,pSess->au8CliIP);
            RTSP_Send_Reply(pSess, RTSP_STATUS_OVER_SUPPORTED_CONNECTION, NULL, 1);
    	}
    	else
    	{
    		HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "vod setup failed %X for %s\n",
                       ret,pSess->au8CliIP);
            RTSP_Send_Reply(pSess, RTSP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
    	}
        return ret;
    }

    char NatAddr[32]={0};
    unsigned short portpair[2]={0};
    if(pSess->au8DestIP[0] != 0 && MTRANS_MODE_UDP == struVodSetupRespInfo.stTransInfo.enMediaTransMode)
    {
        if(rtcpport > 0)
        {
            ret = MTRANS_Get_NatAddr(struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32TransHandle,
                (unsigned char)trackid,
                (unsigned short)struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort,
                (unsigned short)struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort+1,
                NatAddr, portpair);
        }
        else
        {
            ret = MTRANS_Get_NatAddr(struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32TransHandle,
                (unsigned char)trackid,
                (unsigned short)struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort,
                0, NatAddr, portpair);
        }

        if(ret)
        {
            printf("fun:%s,map addr failed!\n",__FUNCTION__);
            RTSP_Send_Reply(pSess, RTSP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
            return ret;
        }
    }
        
    pTemp = pSess->au8SendBuff;
    pTemp += RTSP_Make_RespHdr(pSess, 200);
    
    pTemp += sprintf(pTemp,"Session: %s\r\n", pSess->aszSessID);
    //pTemp += sprintf(pTemp, "Cache-Control: must-revalidate\r\n");

    pSess->enMediaTransMode = struVodSetupRespInfo.stTransInfo.enMediaTransMode;

    if(MTRANS_MODE_UDP == struVodSetupRespInfo.stTransInfo.enMediaTransMode
        || MTRANS_MODE_MULTICAST == struVodSetupRespInfo.stTransInfo.enMediaTransMode)
    {
        pSess->bIsTransData = HI_TRUE;
        pSess->u32TransHandle = struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32TransHandle;
        pSess->CBonTransMeidaDatas = struVodSetupRespInfo.stTransInfo.stUdpTransInfo.pProcMediaTrans;
    }
    else if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_TCP_ITLV)
    {
        pSess->bIsTransData = HI_TRUE;
        pSess->u32TransHandle = struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32TransHandle;
        pSess->CBonTransMeidaDatas = struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.pProcMediaTrans;
    }

    char strport[128] = {0};
    unsigned short server_rtpport = 0;
    unsigned short server_rtcpport = 0;
        
    if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_UDP)
    {
        if(pSess->au8DestIP[0] != 0)
        {
            server_rtpport = portpair[0];
            server_rtcpport = portpair[1];
        }
        else
        {
            server_rtpport = struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort;
            server_rtcpport = struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort;
        }
        
        if(rtcpport > 0)
        {
            sprintf(strport, "client_port=%d-%d;server_port=%d-%d", 
                rtpport, rtcpport, server_rtpport, server_rtcpport);
        }
        else
        {
            sprintf(strport, "client_port=%d;server_port=%d", rtpport,server_rtpport);
        }
        
        if(pSess->au8DestIP[0] != 0)
        {
            pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;mode=play;source=%s;%s;destination=%s;ssrc=%08x\r\n\r\n",
                               NatAddr,strport,destaddr,SSRC);
        }
        else
        {
            pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;mode=play;source=%s;%s;ssrc=%08x\r\n\r\n",
                               pSess->au8HostIP,strport,SSRC);
        }
    }
    else if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_TCP_ITLV)
    {
        if(trackid == RTSP_TRACKID_VIDEO)
        {
/*      
            pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;mode=play;source=%s;interleaved=%d-%d;ssrc=%08x\r\n\r\n",
                pSess->au8HostIP,
                struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrMediaChnId, 
                struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrAdaptChnId,SSRC);
*/
			pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d;ssrc=%08x;mode=\"play\"\r\n",
 				struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrMediaChnId, 
				struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrAdaptChnId,SSRC);

			pTemp += sprintf(pTemp, "Date:  Wed, Dec 17 2014 16:44:21 GMT\r\n\r\n");

        }
        else if(trackid == RTSP_TRACKID_AUDIO)
        {
            int port1 = 2, port2 = 3;		//peter add for test,2011.11.4
		    port1 = struVodSetupReqInfo.astCliSupptTransList[RTSP_TRACKID_AUDIO].stTcpItlvTransInfo.u32ItlvCliMediaChnId ;
		   	port2 = struVodSetupReqInfo.astCliSupptTransList[RTSP_TRACKID_AUDIO].stTcpItlvTransInfo.u32ItlvCliAdaptChnId ;
		   pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;mode=play;source=%s;interleaved=%d-%d;ssrc=%08x\r\n\r\n",
                pSess->au8HostIP, port1, port2,SSRC);
			
			
        }
        else
        {
            printf("Client want to setup.but trackid %d is wrong. \n", trackid);
            RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
			printf("DEBUG %s %d send 400\n",__FILE__,__LINE__);
            return HI_FAILURE;
        }
    }
    else if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_MULTICAST)
    {
        pTemp += sprintf(pTemp,"Transport: RTP/AVP;multicast;mode=play;client_port=%d-%d;destination=239.1.2.3;server_port=%d-%d\r\n\r\n",
                //pSess->au8HostIP,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliMediaRecvPort,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort, 
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort);
    }
    
    
    #if 0
    if (trackid == RTSP_TRACKID_VIDEO )
    {
        printf("Client want to setup video,Track: %d. \n", trackid);
        if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_UDP)
        {
            if(pSess->au8DestIP[0] != 0)
            {
                pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;mode=play;source=%s;%s;destination=%s;ssrc=%08x\r\n\r\n",
                                   NatAddr,strport,destaddr,SSRC);
            }
            else
            {
                pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;mode=play;source=%s;%s;ssrc=%08x\r\n\r\n",
                                   pSess->au8HostIP,strport,SSRC);
            }
        }
        else if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_TCP_ITLV)
        {
            pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;mode=play;source=%s;interleaved=%d-%d;ssrc=%08x\r\n\r\n",
                pSess->au8HostIP,
                struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrMediaChnId, 
                struVodSetupRespInfo.stTransInfo.stTcpItlvTransInfo.u32SvrAdaptChnId,SSRC);
        }
        else if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_MULTICAST)
        {
            pTemp += sprintf(pTemp,"Transport: RTP/AVP;multicast;mode=play;client_port=%d-%d;destination=239.1.2.3;server_port=%d-%d\r\n\r\n",
                //pSess->au8HostIP,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliMediaRecvPort,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort, 
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort);
        }
    }
    else if (trackid == RTSP_TRACKID_AUDIO)
    {
        printf("Client want to setup audio,Track: %d. \n", trackid);
        if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_UDP)
        {
            if(pSess->au8DestIP[0] != 0)
            {
                pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;mode=play;source=%s;%s;destination=%s;ssrc=%08x\r\n\r\n",
                                   NatAddr,strport,destaddr,SSRC);
            }
            else
            {
                pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;mode=play;source=%s;%s;ssrc=%08x\r\n\r\n",
                                   pSess->au8HostIP,strport,SSRC);
            }
        }
        else if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_TCP_ITLV)
        {
            int port1 = 2, port2 = 3;		//peter add for test,2011.11.4
            pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;mode=play;source=%s;interleaved=%d-%d;ssrc=%08x\r\n\r\n",
                pSess->au8HostIP, port1, port2,SSRC);
        }
        else if(struVodSetupRespInfo.stTransInfo.enMediaTransMode == MTRANS_MODE_MULTICAST)
        {
            pTemp += sprintf(pTemp,"Transport: RTP/AVP;multicast;mode=play;client_port=%d-%d;destination=239.1.2.3;server_port=%d-%d\r\n\r\n",
                //pSess->au8HostIP,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliMediaRecvPort,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort,
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrMediaSendPort, 
                struVodSetupRespInfo.stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort);
        }

    }
    else
    {
        printf("Client want to setup.but trackid %d is wrong. \n", trackid);
        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_FAILURE;
    }
    #endif

    RTSP_Send_Reply(pSess, 200, NULL, 0);
    
    pSess->enSessState = RTSP_SESSION_STATE_READY;
    return HI_SUCCESS;
}


int parseRangeParam(char *paramStr,
			double *rangeStart, double *rangeEnd,
			char *absStartTime, char *absEndTime,
			int *startTimeIsNow) 
{
    *startTimeIsNow = 0; // by default
    *absStartTime = '\0';
    *absEndTime = '\0';
    double start, end;
    int numCharsMatched1 = 0, numCharsMatched2 = 0, numCharsMatched3 = 0, numCharsMatched4 = 0;
	printf("DEBUG %s,%d parseRangeParam start\n",__FILE__,__LINE__);
    if (sscanf(paramStr, "npt = %lf - %lf", &start, &end) == 2)
    {
        *rangeStart = start;
        *rangeEnd = end;
    }
    else if (sscanf(paramStr, "npt = %n%lf -", &numCharsMatched1, &start) == 1)
    {
        if (paramStr[numCharsMatched1] == '-')
        {
            // special case for "npt = -<endtime>", which matches here:
            *rangeStart = 0.0;
            *startTimeIsNow = 1;
            *rangeEnd = -start;
        }
        else
        {
            *rangeStart = start;
            *rangeEnd = 0.0;
        }
    }
    else if (sscanf(paramStr, "npt = now - %lf", &end) == 1)
    {
        *rangeStart = 0.0;
        *startTimeIsNow = 1;
        *rangeEnd = end;
    }
    else if (sscanf(paramStr, "npt = now -%n", &numCharsMatched2) == 0 && numCharsMatched2 > 0)
    {
        *rangeStart = 0.0;
        *startTimeIsNow = 1;
        *rangeEnd = 0.0;
    }
    else if (sscanf(paramStr, "clock = %n", &numCharsMatched3) == 0 && numCharsMatched3 > 0)
    {
        *rangeStart = *rangeEnd = 0.0;

        char *utcTimes = &paramStr[numCharsMatched3];

        int sscanfResult = sscanf(utcTimes, "%[^-]-%s", absStartTime, absEndTime);
        if(sscanfResult == 1) 
        {
            *absEndTime='\0';
        } 
        else if(sscanfResult != 2)
        {
            return 0;
        }
    } 
    else if (sscanf(paramStr, "smtpe = %n", &numCharsMatched4) == 0 && numCharsMatched4 > 0)
    {
        // We accept "smtpe=" parameters, but currently do not interpret them.
    } 
    else
    {
        return 0; // The header is malformed
    }
	printf("DEBUG %s,%d parseRangeParam end\n",__FILE__,__LINE__);
    return 1;
}

int parseRangeHeader(char *buf,
			 double *rangeStart, double *rangeEnd,
			 char *absStartTime, char *absEndTime,
			 int *startTimeIsNow) {
    // First, find "Range:"
    int i;
    char *p = buf;
    p = strstr(buf, "Range: ");
    if(p == NULL)
        return 0;

    p += 7;
    while(*p!='\0') 
    {
        if(*p == ' ')
            p++;
        else
            break;
    }
    printf("DEBUG %s,%d parseRangeHeader\n",__FILE__,__LINE__);
    return parseRangeParam(p, rangeStart, rangeEnd, absStartTime, absEndTime, startTimeIsNow);
}



/*
PLAY rtsp://10.85.180.240/sample_h264_100kbit.mp4 RTSP/1.0

CSeq: 7

Range: npt=0.000000-70.000000

x-prebuffer: maxtime=2.000000

x-transport-options: late-tolerance=10

Session: 115302692050290

User-Agent: QuickTime/7.0.3 (qtver=7.0.3;os=Windows NT 5.1Service Pack 2)

//////////////////////////////////////////////////////////////

RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 7

Session: 115302692050290

Range: npt=0.00000-70.00000

RTP-Info: url=rtsp://10.85.180.240/sample_h264_100kbit.mp4/trackID=3;seq=23717;rtptime=11394,url=rtsp://10.85.180.240/sample_h264_100kbit.mp4/trackID=4;seq=28541;rtptime=1464
*/
HI_S32 RTSP_Handle_Play(RTSP_LIVE_SESSION *pSess )
{
    //int i = 0;
   	//HI_BOOL bHasTran = HI_TRUE;
    HI_S32 iRet ;
    HI_CHAR * pTemp;
    int iCseq,ret;
    RTSP_URL url;

    double rangeStart = 0.0, rangeEnd = 0.0;
    char absStart[32] = {0}; char absEnd[32] = {0};
    int startTimeIsNow = 0;
    int sawRangeHeader = 0;
    //mhua change
   	/*获取CSeq信息*/
    ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&iCseq);
    if (iCseq > 0 )
    {
        pSess->last_recv_seq= iCseq;
        printf("Cseq: %d \n", iCseq);
    }
    else
    {
        printf("invalid cseq no.\n");
    }
#if 0
	//TODO change the chn's check
	if (!RTSP_CHECK_CHN( pSess->s32VsChn))
	{
	    printf("chnid : %d error.reset to chn 0", pSess->s32VsChn);
	    pSess->s32VsChn= 11;
	    //return HI_FAILURE;
	} 
#endif
    HI_VOD_GetVersion(pSess->au8SvrVersion, RTSP_MAX_VERSION_LEN);

    if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
    {
        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }

    //UpdateUrl(pSess->au8RecvBuff);
    url_info urlinfo;
    memset(&urlinfo, 0, sizeof(urlinfo));
    ret = RTSP_Parse_Url(url, &urlinfo);
    if (ret == HI_FAILURE)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "invalid url: %s\n", url);
       	//RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
       	//return HI_SUCCESS;
    }
    sawRangeHeader = parseRangeHeader(pSess->au8RecvBuff,&rangeStart, &rangeEnd, absStart, absEnd, &startTimeIsNow);
    if((pSess->bRequestStreamFlag[0] != HI_TRUE)&&(pSess->bRequestStreamFlag[1] != HI_TRUE))
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "nothing to play (client %s)\n",pSess->au8CliIP);
        RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }
    if(pSess->enSessState == RTSP_SESSION_STATE_PAUSE)
    {
        VOD_RESUME_MSG_S stVodResumeReq;
        VOD_RESUME_RespMSG_S stVodResumeResp;
        memset(&stVodResumeReq, 0, sizeof(stVodResumeReq));
        memset(&stVodResumeResp, 0, sizeof(stVodResumeResp));
        strcpy(stVodResumeReq.aszSessID,pSess->aszSessID);

        if(pSess->bRequestStreamFlag[0] == HI_TRUE)
        {
            stVodResumeReq.u8MediasType += 0x01;
        }
        if(pSess->bRequestStreamFlag[1] == HI_TRUE)
        {
            stVodResumeReq.u8MediasType += 0x02;
        }
        iRet = HI_VOD_Resume(&stVodResumeReq, &stVodResumeResp);
        if(iRet != HI_SUCCESS)
        {
            printf("HI_VOD_Resume failed :%x \n",iRet);
            RTSP_Send_Reply(pSess, 500, NULL, 1);
            return iRet;
        }
    }
    else if(pSess->enSessState != RTSP_SESSION_STATE_PLAY)
    {
        VOD_PLAY_MSG_S stVodPlayReqInfo;
        VOD_RePLAY_MSG_S stVodPlayRespInfo;
        memset(&stVodPlayReqInfo, 0, sizeof(stVodPlayReqInfo));
        memset(&stVodPlayRespInfo, 0, sizeof(stVodPlayRespInfo));
        strcpy(stVodPlayReqInfo.aszSessID,pSess->aszSessID);
        stVodPlayReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP;
        if(pSess->bRequestStreamFlag[0] == HI_TRUE)
        {
            stVodPlayReqInfo.u8PlayMediaType += 0x01;
        }
        if(pSess->bRequestStreamFlag[1] == HI_TRUE)
        {
            stVodPlayReqInfo.u8PlayMediaType += 0x02;
        }
        stVodPlayReqInfo.u32Duration = 0;
        stVodPlayReqInfo.u32StartTime = 0;
        
        iRet = HI_VOD_Play(&stVodPlayReqInfo, &stVodPlayRespInfo);
        DEBUGPrtVODPlay(&stVodPlayReqInfo,&stVodPlayRespInfo);
        if(iRet != HI_SUCCESS)
        {
            printf("HI_VOD_Play failed :%x \n",iRet);
            RTSP_Send_Reply(pSess, 500, NULL, 1);
            return iRet;
        }
    }
    if(pSess->bIsTurnRelay 
        && pSess->enMediaTransMode == MTRANS_MODE_UDP
        && pSess->au8DestIP[0] != 0)
    {
        ret = MTRANS_Check_Redirect(pSess->u32TransHandle, pSess->bRequestStreamFlag[0], pSess->bRequestStreamFlag[1]);
        if(ret<0)
        {
            printf("fun:%s,MTRANS_Check_Redirect error!\n",__FUNCTION__);
        }
    }
    pTemp = pSess->au8SendBuff;
    pTemp += RTSP_Make_RespHdr(pSess, 200);
    //pTemp += sprintf(pTemp,"Date: 23 Jan 1997 15:35:06 GMT\r\n");
    pTemp += sprintf(pTemp,"Session: %s\r\n", pSess->aszSessID);
    
    //TODO 替换为真正的值
    HI_U32 timeVideo = 0,timeAudio = 0;
    VOD_GET_BASEPTS_MSG_S stVodGetPtsReqInfo;
    VOD_ReGET_BASEPTS_MSG_S stVodGetPtsRespInfo;
    stVodGetPtsReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP;
    strncpy(stVodGetPtsReqInfo.aszSessID,pSess->aszSessID,RTSP_MAX_SESSID_LEN);
//peter modify 2011.11.1
#if 0
    ret = HI_VOD_GetBaseTimeStamp(&stVodGetPtsReqInfo, &stVodGetPtsRespInfo);
    if(HI_SUCCESS != ret)
    {
        memset(pSess->au8SendBuff,0,sizeof(pSess->au8SendBuff));
        RTSP_Send_Reply(pSess, RTSP_STATUS_INTERNAL_SERVER_ERROR, NULL, 1);
        return ret;
    }
   	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");fflush(stdout);
    timeVideo = stVodGetPtsRespInfo.u64BaseTimeStamp;
    //              "RTP-Info:url=rtsp://%s/chn=%d/video;seq=0;rtptime=%lu"
#endif
    //video info
#if 0
	pTemp += 
	sprintf(pTemp,"RTP-Info:url=rtsp://%s/%s/trackID=%d;seq=0;rtptime=%lu "
	                 ,pSess->au8HostIP,au8FileInfo,RTSP_TRACKID_VIDEO,timeVideo);
#endif

    unsigned long videoseq = 0;
    unsigned long audioseq = 0;
    int seek = 0;
    if(rangeStart > 0.0 || pSess->start_offset>0)
    {
        pTemp += sprintf(pTemp,"Range: npt=%lf-\r\n", rangeStart);
        
        /*
        if(pSess->bIsPlayback == HI_TRUE 
            && (pSess->enSessState == RTSP_SESSION_STATE_PLAY
            || pSess->enSessState == RTSP_SESSION_STATE_PAUSE))
            */
        if(pSess->bIsPlayback == HI_TRUE )
        {
            SeekResp_S stSeekResp;
            memset(&stSeekResp, 0, sizeof(stSeekResp));
            printf("%s, seek to time:%lfS\n",__FUNCTION__,rangeStart);
            rangeStart += pSess->start_offset;
            ret = MTRANS_playback_seek(pSess->u32TransHandle, rangeStart, &stSeekResp);
            if(ret != HI_SUCCESS)
            {
                printf("%s, MTRANS_playback_seek failed!\n",__FUNCTION__);
                return ret;
            }

            timeVideo = stSeekResp.vtime*90;
            timeAudio = stSeekResp.atime*8;
            videoseq = stSeekResp.vseq;
            audioseq = stSeekResp.aseq;

            seek = 1;
        }
    }
    if(pSess->bRequestStreamFlag[0] == HI_TRUE)
    {
		//注意，这里是最后字段，不能加 \r\n ，因为下面加了两个 \r\n strcat( pSess->au8SendBuff, RTSP_LRLF ) ,有些做得烂的如果多加一个\r\n都会断开。
		pTemp += 
		    sprintf(pTemp,"RTP-Info:url=%s;seq=%lu;rtptime=%lu\r\n",pSess->au8CliUrl[0],videoseq, timeVideo);

		//pTemp += sprintf(pTemp, "RTP-Info:url=trackID=0;seq=4463\r\n");
		pTemp += sprintf(pTemp, "User-Agent:NKPlayer-1.00.00.081112");
    }
    if(pSess->bRequestStreamFlag[1] == HI_TRUE)
    {
		pTemp += 
		    sprintf(pTemp,"RTP-Info:url=%s;seq=%lu;rtptime=%lu",pSess->au8CliUrl[1],audioseq, timeAudio);
    }
    //pTemp += sprintf(pTemp,"RTP-Info:url=%s", pSess->au8CliUrl[0]);
#if 0
	//audio info
	pTemp += 
	sprintf(pTemp,",url=rtsp://%s/%d/trackID=%d;seq=0;rtptime=%lu"
	                ,pSess->au8HostIP,pSess->s32VsChn,RTSP_TRACKID_AUDIO,timeAudio);
#endif
	
    strcat( pSess->au8SendBuff, RTSP_LRLF );
    strcat( pSess->au8SendBuff, RTSP_LRLF );
    RTSP_Send_Reply(pSess, 200, NULL, 0);
    if(pSess->enSessState != RTSP_SESSION_STATE_PLAY 
        && MTRANS_MODE_MULTICAST == pSess->enMediaTransMode)
    {
        ret = HI_RTSP_Send_ext(pSess);
        if(ret != HI_SUCCESS)
            return HI_FAILURE;
        ret = MTRANS_StartMulticastSendTask((HI_U32)pSess->u32TransHandle);
        if(ret != HI_SUCCESS)
            return ret;

    }
    pSess->enSessState = RTSP_SESSION_STATE_PLAY;
    if(seek)
        return RTSP_SEEK_FLAG;
    
    return HI_SUCCESS;
}

/*
PAUSE rtsp://10.85.180.240/sample_h264_100kbit.mp4 RTSP/1.0

CSeq: 8

Session: 115302692050290

User-Agent: QuickTime/7.0.3 (qtver=7.0.3;os=Windows NT 5.1Service Pack 2)

////////////////////////////////////////////////////////////////////////////////

RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 8

Session: 115302692050290

*/
HI_S32 RTSP_Handle_Pause(RTSP_LIVE_SESSION *pSess)
{
    HI_S32 s32Ret = HI_SUCCESS;
    RTSP_URL url = {0};
    int RTSP_err_code = RTSP_STATUS_OK;
    HI_CHAR *pTemp = NULL;
  	//HI_CHAR *pTemp2 = NULL;
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH] = {0};
    //HI_CHAR aszSvrVersion[RTSP_MAX_VERSION_LEN] = {0}; 
    HI_S32 Cseq = 0; 

    HI_LOG_DEBUG(RTSP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C --> S >>>>>>\n"
                   "%s\n"
                   "<<<<<<\n" ,
                   pSess->au8RecvBuff);
   
    HI_VOD_GetVersion(pSess->au8SvrVersion, RTSP_MAX_VERSION_LEN);               
    if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
    {
        /*发送错误回复消息，并关闭socket*/
        RTSP_Send_Reply(pSess, 400, NULL, 1);
        return HI_ERR_RTSP_ILLEGALE_PARAM;
	}
#if 0
	/*获取url中指定的暂停媒体类型及pause方法*/
	s32Ret= RTSP_ParseParam(url,&method, &u8PauseMediasType);
	if(   (s32Ret != HI_SUCCESS) 
	   || (s32Ret == HI_SUCCESS && RTSP_PAUSE_METHOD != method))
	{
	    RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
	    /*发送错误回复消息，并关闭socket*/
	    RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
	    return;
	}
#endif
   
    /*获取CSeq信息*/
    s32Ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if( HI_SUCCESS != s32Ret)
    {
        RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_Send_Reply(pSess, 400, NULL, 1);
        return s32Ret;
    }
    
    /*获取sessionid*/
    s32Ret = RTSP_Get_SessId(pSess->au8RecvBuff,aszSessID);
    if(HI_SUCCESS != s32Ret)
    {
        RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_Send_Reply(pSess, 400, NULL, 1);
        return s32Ret;
    }

    if(pSess->last_send_frametype == 0)
    {
        VOD_PAUSE_MSG_S stVodPauseMsg;
        VOD_RePAUSE_MSG_S stVodRePauseMsg;
        memset(&stVodPauseMsg,0,sizeof(VOD_PAUSE_MSG_S));
        memset(&stVodRePauseMsg,0,sizeof(VOD_RePAUSE_MSG_S));

        /*consist vod pause msg*/
        stVodPauseMsg.enSessType = SESS_PROTOCAL_TYPE_RTSP;
        strncpy(stVodPauseMsg.aszSessID,aszSessID,VOD_MAX_STRING_LENGTH);
        stVodPauseMsg.u32PauseTime = 0;     /*this attribution is invalid for live*/
        if(pSess->bRequestStreamFlag[0] == HI_TRUE)
        {
            stVodPauseMsg.u8PauseMediasType = 0x01;
        }
        if(pSess->bRequestStreamFlag[1] == HI_TRUE)
        {
            stVodPauseMsg.u8PauseMediasType += 0x02;
        }
    	if(stVodPauseMsg.u8PauseMediasType == 0)
        {
            HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "nothing to pause (client %s)\n",pSess->au8CliIP);
    	    RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
            return HI_ERR_RTSP_ILLEGALE_REQUEST;
        }    
        s32Ret = HI_VOD_Pause(&stVodPauseMsg,&stVodRePauseMsg);
        DEBUGPrtVODPause(&stVodPauseMsg,&stVodRePauseMsg);
        if(HI_SUCCESS != s32Ret)
        {
            RTSP_err_code = RTSP_STATUS_INTERNAL_SERVER_ERROR;
            /*发送错误回复消息，并关闭socket*/
            RTSP_Send_Reply(pSess, 500, NULL, 1);
            return s32Ret;
        }
        
        pTemp = pSess->au8SendBuff;
        pTemp += RTSP_Make_RespHdr(pSess, 200);
        //pTemp += sprintf(pTemp,"Date: 23 Jan 1997 15:35:06 GMT\r\n");
        pTemp += sprintf(pTemp,"Session: %s\r\n\r\n", pSess->aszSessID);
       	//pTemp += sprintf(pTemp,"Server: %s\r\n", aszSvrVersion);
        RTSP_Send_Reply(pSess, 200, NULL, 0);
    }
    else
        pSess->s32WaitDoPause = 1;
    
    pSess->enSessState = RTSP_SESSION_STATE_PAUSE;
    
	return HI_SUCCESS; 
}

HI_S32 RTSP_Do_Pause(RTSP_LIVE_SESSION *pSess)
{
    char *pTemp;
    HI_S32 s32Ret = HI_SUCCESS;
    VOD_PAUSE_MSG_S stVodPauseMsg;
    VOD_RePAUSE_MSG_S stVodRePauseMsg;
    memset(&stVodPauseMsg,0,sizeof(VOD_PAUSE_MSG_S));
    memset(&stVodRePauseMsg,0,sizeof(VOD_RePAUSE_MSG_S));

    /*consist vod pause msg*/
    stVodPauseMsg.enSessType = SESS_PROTOCAL_TYPE_RTSP;
    strncpy(stVodPauseMsg.aszSessID,pSess->aszSessID,VOD_MAX_STRING_LENGTH);
    stVodPauseMsg.u32PauseTime = 0;     /*this attribution is invalid for live*/
    if(pSess->bRequestStreamFlag[0] == HI_TRUE)
    {
        stVodPauseMsg.u8PauseMediasType = 0x01;
    }
    if(pSess->bRequestStreamFlag[1] == HI_TRUE)
    {
        stVodPauseMsg.u8PauseMediasType += 0x02;
    }
	if(stVodPauseMsg.u8PauseMediasType == 0)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR, "nothing to pause (client %s)\n",pSess->au8CliIP);
	    RTSP_Send_Reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
        return HI_ERR_RTSP_ILLEGALE_REQUEST;
    }    
    s32Ret = HI_VOD_Pause(&stVodPauseMsg,&stVodRePauseMsg);
    if(HI_SUCCESS != s32Ret)
    {
        RTSP_Send_Reply(pSess, 500, NULL, 1);
        return s32Ret;
    }

    pTemp = pSess->au8SendBuff;
    pTemp += RTSP_Make_RespHdr(pSess, 200);
    //pTemp += sprintf(pTemp,"Date: 23 Jan 1997 15:35:06 GMT\r\n");
    pTemp += sprintf(pTemp,"Session: %s\r\n\r\n", pSess->aszSessID);
   	//pTemp += sprintf(pTemp,"Server: %s\r\n", aszSvrVersion);
    RTSP_Send_Reply(pSess, 200, NULL, 0);

    s32Ret = HI_RTSP_Send(pSess->s32SessSock, pSess->au8SendBuff, strlen(pSess->au8SendBuff)) ;
    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"to send RTSP pause response %d byte to cli %s thought sock %d failed\n",
                   strlen(pSess->au8SendBuff),pSess->au8CliIP,pSess->s32SessSock);
       s32Ret = HI_ERR_RTSP_SEND_FAILED;
    }
    else
    {
        HI_LOG_DEBUG(RTSP_LIVESVR,LOG_LEVEL_NOTICE, "Send pause Reply .Chn:%s S --> C(%s) >>>>>>\n"
                       "%s\n"
                       "<<<<<<\n",
            pSess->auFileName, pSess->au8CliIP,pSess->au8SendBuff);    
    }

    HI_RTSP_ClearSendBuff(pSess);
    pSess->readyToSend = HI_FALSE;

    return s32Ret;
}

HI_S32 RTSP_HeartBeat_Pause(RTSP_LIVE_SESSION *pSess)
{
    HI_S32 ret;
    int iCseq;
	char *pTemp;

	/*获取CSeq信息*/
	 ret = RTSP_Get_CSeq(pSess->au8RecvBuff, &iCseq);
	 if (iCseq > 0 )
	 {
		 pSess->last_recv_seq= iCseq;
		 //printf("Cseq: %d \n", iCseq);
	 }
	 else
	 {
		 printf("invalid cseq no.\n");
	 }

	pTemp = pSess->au8SendBuff;
	pTemp += RTSP_Make_RespHdr(pSess, 200);
	//pTemp += sprintf(pTemp, "CSeq: %d\r\n", iCseq);;

	time_t timep;
	time (&timep);
	//Date: Mon, 12 Nov 2012 03:47:17 GMT
	//		Wed Jan 28 10:14:50 2015
	pTemp += sprintf(pTemp, "Date: %sGMT\r\n\r\n", ctime(&timep));

	//printf("#### %s %d, pSess->au8SendBuff:%s\n", __FUNCTION__, __LINE__, pSess->au8SendBuff);
	RTSP_Send_Reply(pSess, 200, NULL, 0);

	return HI_SUCCESS;
}



#if 0
HI_VOID RTSP_Handle_RePlay(HI_S32 s32LinkFd, HI_CHAR* pMsgBuffAddr,HI_U32 u32MsgLen)
{
    HI_S32 s32Ret = 0;
    RTSP_URL url = {0};
    int RTSP_err_code = RTSP_STATUS_OK;
    HI_CHAR   OutBuff[512]= {0};
    HI_CHAR   content[512]= {0};
    HI_CHAR *pTemp = NULL;
    HI_CHAR *pTemp2 = NULL;
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH]= {0}; 
    HI_CHAR aszSvrVersion[RTSP_MAX_VERSION_LEN] = {0}; 
    HI_U8 u8PlayMediasType = 0;
    HI_S32 Cseq = 0; 
    RTSP_REQ_METHOD_E method = RTSP_REQ_METHOD_BUTT;

    HI_LOG_DEBUG(RTSP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C --> S >>>>>>\n"
                   "%s\n"
                   "<<<<<<\n" ,
                   pMsgBuffAddr);
                   
    if (!sscanf(pMsgBuffAddr, " %*s %254s ", url)) 
    {
        s32Ret = HI_ERR_RTSP_ILLEGALE_PARAM;
        RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
        return;
	}

	/*获取url中指定的暂停媒体类型及pause方法*/
	s32Ret= RTSP_ParseParam(url,&method, &u8PlayMediasType);
	if(   (s32Ret != HI_SUCCESS) 
	   || (s32Ret == HI_SUCCESS && RTSP_REPLAY_METHOD != method))
	{
        RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
        return;
	}

    /*获取CSeq信息*/
    s32Ret = RTSP_Get_CSeq(pMsgBuffAddr,&Cseq);
    if( HI_SUCCESS != s32Ret)
    {
        RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
        return;
    }
    
    /*获取sessionid*/
    s32Ret = RTSP_Get_SessId(pMsgBuffAddr,aszSessID);
    if(HI_SUCCESS != s32Ret)
    {
        RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
        return;
    }

    /*如果前面解析都无错，则向vod请求replay*/
    VOD_PLAY_MSG_S stVodPlayMsg;
    VOD_RePLAY_MSG_S stVodRePlayMsg;
    memset(&stVodPlayMsg,0,sizeof(VOD_PLAY_MSG_S));
    memset(&stVodRePlayMsg,0,sizeof(VOD_RePLAY_MSG_S));

    /*consist vod play msg*/
    stVodPlayMsg.enSessType = SESS_PROTOCAL_TYPE_RTSP;
    strncpy(stVodPlayMsg.aszSessID,aszSessID,VOD_MAX_STRING_LENGTH);
    stVodPlayMsg.u32StartTime = 0;     /*this attribution is invalid for live*/
    stVodPlayMsg.u32Duration =0;       /*this attribution is invalid for live*/
    stVodPlayMsg.u8PlayMediaType= u8PlayMediasType;

    s32Ret = HI_VOD_Play(&stVodPlayMsg,&stVodRePlayMsg);
    DEBUGPrtVODPlay(&stVodPlayMsg,&stVodRePlayMsg);
    if(HI_SUCCESS != s32Ret)
    {
        RTSP_err_code = RTSP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
        RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
        return;
    }

    s32Ret = HI_VOD_GetVersion(aszSvrVersion,RTSP_MAX_VERSION_LEN);
    if(s32Ret != HI_SUCCESS)
    {
        RTSP_err_code = RTSP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
        RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
        return;
    }

    /*consist replay response*/
    pTemp = OutBuff;
    pTemp += sprintf(pTemp, "%s %d %s\r\n", RTSP_VER_STR, 200, "OK");
    pTemp += sprintf(pTemp,"Server: %s\r\n",aszSvrVersion);
    pTemp += sprintf(pTemp,"Connection: Keep-Alive\r\n");
    pTemp += sprintf(pTemp,"Content-Type: application/octet-stream\r\n");
    pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");

    /* to do:回应中是否需要带cseq session id, RTSPsess中的Cseq如相应增加*/
    pTemp2 = content;
    pTemp2 += sprintf(pTemp2,"Session: %s\r\n",aszSessID);
    pTemp2 += sprintf(pTemp2,"Cseq: %d\r\n",Cseq);
    pTemp2 += sprintf(pTemp2,"\r\n");
    
    pTemp += sprintf(pTemp,"Content-Length: %d\r\n\r\n", strlen(content));  

    strcat(pTemp, content);
     

    /*发送回应信息*/
    s32Ret = HI_RTSP_Send(s32LinkFd,OutBuff,strlen(OutBuff));	/* bad request */
    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"to send RTSP replay response %d byte thought sock %d failed\n",
                   strlen(OutBuff),s32LinkFd);
    }
    else
    {
        
        HI_LOG_DEBUG(RTSP_LIVESVR,LOG_LEVEL_NOTICE, "Send Reply .S --> C >>>>>>\n"
                       "%s\n"
                       "<<<<<<\n",
                       OutBuff);
    }

    /*关闭该链接*/
    close(s32LinkFd);

}
#endif



/*
TEARDOWN rtsp://119.122.106.167/sample_100kbit.mp4/ RTSP/1.0

Session: 91534343022737

User-Agent: RealMedia Player/mc.30.26.01 (s60; epoc_av30_armv5)

CSeq: 7



RTSP/1.0 200 OK

Server: DSS/5.5.5 (Build/489.16; Platform/Win32; Release/Darwin; state/beta; )

Cseq: 7

Session: 91534343022737

Connection: Close
*/
HI_S32 RTSP_Handle_Teardown(RTSP_LIVE_SESSION *pSess )
{
    HI_S32 s32Ret = HI_SUCCESS;
    RTSP_URL url = {0};
    int RTSP_err_code = RTSP_STATUS_OK;
  	//HI_CHAR   OutBuff[512] = {0};
  	//HI_CHAR   content[512] = {0};
    HI_CHAR *pTemp = NULL;
   	//HI_CHAR *pTemp2 = NULL;
    HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH] = {0};
    HI_CHAR aszSvrVersion[RTSP_MAX_VERSION_LEN] = {0}; 
  	//HI_U8 u8PauseMediasType = 0;
    HI_S32 Cseq = 0; 
   	//RTSP_REQ_METHOD_E method = RTSP_REQ_METHOD_BUTT;

    HI_LOG_DEBUG(RTSP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C --> S >>>>>>\n"
                   "%s\n"
                   "<<<<<<\n" ,
                   pSess->au8RecvBuff);

	printf("Recive C --> S >>>>>>\n"
                   "%s\n"
                   "<<<<<<\n" ,
                   pSess->au8RecvBuff);

#if 1
	if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
	{
	    /*发送错误回复消息，并关闭socket*/
	    //RTSP_Send_Reply(pSess, 400, NULL, 1);
	    //return HI_FAILURE;
	}
#endif

#if 0
	/*获取url中指定的暂停媒体类型及pause方法*/
	s32Ret= RTSP_ParseParam(url,&method, &u8PauseMediasType);
	if(   (s32Ret != HI_SUCCESS) 
	   || (s32Ret == HI_SUCCESS && RTSP_PAUSE_METHOD != method))
	{
	    RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
	    /*发送错误回复消息，并关闭socket*/
	    RTSP_SendErrResp(s32LinkFd,RTSP_err_code);
	    return;
	}
#endif
   
    /*获取CSeq信息*/
    s32Ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
    if(s32Ret == HI_SUCCESS)
    {
        pSess->last_recv_seq = Cseq;
    }
#if 0
	if( HI_SUCCESS != s32Ret)
	{
	    RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
	    /*发送错误回复消息，并关闭socket*/
	    RTSP_Send_Reply(pSess, 400, NULL, 1);
	    return HI_FAILURE;
	}
#endif  
	s32Ret = HI_VOD_GetVersion(aszSvrVersion,RTSP_MAX_VERSION_LEN);
    if(s32Ret != HI_SUCCESS)
    {
        DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"HI_VOD_GetVersion error =%X\n",s32Ret);
        RTSP_err_code = RTSP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
       // RTSP_Send_Reply(pSess, 500, NULL, 1);
        printf("<RTSP>HI_VOD_GetVersion failed:%x",s32Ret);
        //return HI_FAILURE;
    }

    /*如果前面解析都无错，则向vod请求暂停*/
    VOD_TEAWDOWN_MSG_S stVodCloseReqInfo;
    VOD_ReTEAWDOWN_MSG_S stVodCloseRespInfo;
    memset(&stVodCloseReqInfo,0,sizeof(VOD_PAUSE_MSG_S));
    memset(&stVodCloseRespInfo,0,sizeof(VOD_RePAUSE_MSG_S));
   
    /*获取sessionid*/
    s32Ret = RTSP_Get_SessId(pSess->au8RecvBuff,aszSessID);
    printf("s32Ret=%d,aszSessID=%s,pSess->aszSessID=%s!\n",s32Ret,aszSessID,pSess->aszSessID);
    if(HI_SUCCESS != s32Ret)
    {
        printf("<RTSP>RTSP_Get_SessId failed:%x\n pSess->au8RecvBuff=%s\n",s32Ret,pSess->au8RecvBuff);
        RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
        /*发送错误回复消息，并关闭socket*/
        strncpy(aszSessID,pSess->aszSessID,RTSP_MAX_SESSID_LEN);
		//RTSP_Send_Reply(pSess, 400, NULL, 1);
        //return HI_FAILURE;
    }
    //strcpy(aszSessID,pSess->aszSessID);
    strcpy(stVodCloseReqInfo.aszSessID,aszSessID);
    stVodCloseReqInfo.enSessType = SESS_PROTOCAL_TYPE_RTSP;
    stVodCloseReqInfo.u32Reason = 0;
    stVodCloseReqInfo.enTransMode = pSess->enMediaTransMode;
    printf("<RTSP> Handle Teardown aszSessID:%s\n",stVodCloseReqInfo.aszSessID);
    s32Ret = HI_VOD_Teardown(&stVodCloseReqInfo,&stVodCloseRespInfo);
    DEBUGPrtVODTeawdown(&stVodCloseReqInfo,&stVodCloseRespInfo);
    if(HI_SUCCESS != s32Ret)
    {
        DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"HI_VOD_Teardown error =%X\n",s32Ret);
        RTSP_err_code = RTSP_STATUS_INTERNAL_SERVER_ERROR;
        /*发送错误回复消息，并关闭socket*/
        RTSP_Send_Reply(pSess, 500, NULL, 1);
        return HI_FAILURE;
    }

 	pSess->bIsTransData = HI_FALSE;
    pTemp = pSess->au8SendBuff;
    pTemp += RTSP_Make_RespHdr(pSess, 200);
    
     //pTemp += sprintf(pTemp,"Date: 23 Jan 1997 15:35:06 GMT\r\n");
    pTemp += sprintf(pTemp,"Session: %s\r\n\r\n", pSess->aszSessID);
    RTSP_Send_Reply(pSess, 200, NULL, 0);

    pSess->enSessState = RTSP_SESSION_STATE_STOP;
    return RTSP_TEAR_DOWN_END_FALG;
    //return HI_SUCCESS;
}
HI_S32 RTSP_Handle_RequestIFrame(RTSP_LIVE_SESSION *pSess )
{
    HI_S32 s32Ret = HI_SUCCESS;
	RTSP_URL url = {0};
	int RTSP_err_code = RTSP_STATUS_OK;
	HI_CHAR *pTemp = NULL;
	HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH] = {0};
	HI_CHAR aszSvrVersion[RTSP_MAX_VERSION_LEN] = {0}; 
	HI_S32 Cseq = 0; 
    
    printf("RTSP_Handle_RequestIFrame--Enter!\n");

    printf("Recive C --> S >>>>>>\n"
					 "%s\n"
					 "<<<<<<\n" ,
					 pSess->au8RecvBuff);

    
	pTemp = pSess->au8SendBuff;
	pTemp += RTSP_Make_RespHdr(pSess, 200);
	pTemp += sprintf(pTemp,"Session: %s\r\n\r\n", pSess->aszSessID);
	RTSP_Send_Reply(pSess, 200, NULL, 0);
    
    printf("RTSP_Handle_RequestIFrame--Exit!\n");
    return HI_SUCCESS;
}
HI_S32 RTSP_Handle_SetParam(RTSP_LIVE_SESSION *pSess )
{
	HI_S32 s32Ret = HI_SUCCESS;
	RTSP_URL url = {0};
	int RTSP_err_code = RTSP_STATUS_OK;
	HI_CHAR *pTemp = NULL;
	HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH] = {0};
	HI_CHAR aszSvrVersion[RTSP_MAX_VERSION_LEN] = {0}; 
	HI_S32 Cseq = 0; 

	HI_LOG_DEBUG(RTSP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C --> S >>>>>>\n"
					 "%s\n"
					 "<<<<<<\n" ,
					 pSess->au8RecvBuff);
	
 	if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
	{
		/*发送错误回复消息，并关闭socket*/
		//RTSP_Send_Reply(pSess, 400, NULL, 1);
		//return HI_FAILURE;
	}
 
	/*获取CSeq信息*/
	s32Ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
	if(s32Ret == HI_SUCCESS)
	{
		pSess->last_recv_seq = Cseq;
	}

	s32Ret = HI_VOD_GetVersion(aszSvrVersion,RTSP_MAX_VERSION_LEN);
	if(s32Ret != HI_SUCCESS)
	{
		DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"HI_VOD_GetVersion error =%X\n",s32Ret);
		RTSP_err_code = RTSP_STATUS_INTERNAL_SERVER_ERROR;
		/*发送错误回复消息，并关闭socket*/
		// RTSP_Send_Reply(pSess, 500, NULL, 1);
		printf("<RTSP>HI_VOD_GetVersion failed:%x",s32Ret);
		//return HI_FAILURE;
	}

	/*获取sessionid*/
	s32Ret = RTSP_Get_SessId(pSess->au8RecvBuff,aszSessID);
	if(HI_SUCCESS != s32Ret)
	{
		printf("<RTSP>RTSP_Get_SessId failed:%x",s32Ret);
		RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
		/*发送错误回复消息，并关闭socket*/
		strncpy(aszSessID,pSess->aszSessID,RTSP_MAX_SESSID_LEN);
		//RTSP_Send_Reply(pSess, 400, NULL, 1);
		//return HI_FAILURE;
	}

	// pSess->bIsTransData = HI_FALSE;
	pTemp = pSess->au8SendBuff;
	pTemp += RTSP_Make_RespHdr(pSess, 200);
	pTemp += sprintf(pTemp,"Session: %s\r\n\r\n", pSess->aszSessID);
	RTSP_Send_Reply(pSess, 200, NULL, 0);
	return HI_SUCCESS;
}

HI_S32 RTSP_Handle_GetParam(RTSP_LIVE_SESSION *pSess )
{
	HI_S32 s32Ret = HI_SUCCESS;
	RTSP_URL url = {0};
	int RTSP_err_code = RTSP_STATUS_OK;
	HI_CHAR *pTemp = NULL;
	HI_CHAR aszSessID[VOD_MAX_STRING_LENGTH] = {0};
	HI_CHAR aszSvrVersion[RTSP_MAX_VERSION_LEN] = {0}; 
	HI_S32 Cseq = 0; 

	HI_LOG_DEBUG(RTSP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C --> S >>>>>>\n"
					 "%s\n"
					 "<<<<<<\n" ,
					 pSess->au8RecvBuff);
	
 	if (!sscanf(pSess->au8RecvBuff, " %*s %254s ", url)) 
	{
		/*发送错误回复消息，并关闭socket*/
		//RTSP_Send_Reply(pSess, 400, NULL, 1);
		//return HI_FAILURE;
	}
 
	/*获取CSeq信息*/
	s32Ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
	if(s32Ret == HI_SUCCESS)
	{
		pSess->last_recv_seq = Cseq;
	}

	s32Ret = HI_VOD_GetVersion(aszSvrVersion,RTSP_MAX_VERSION_LEN);
	if(s32Ret != HI_SUCCESS)
	{
		DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"HI_VOD_GetVersion error =%X\n",s32Ret);
		RTSP_err_code = RTSP_STATUS_INTERNAL_SERVER_ERROR;
		/*发送错误回复消息，并关闭socket*/
		// RTSP_Send_Reply(pSess, 500, NULL, 1);
		printf("<RTSP>HI_VOD_GetVersion failed:%x",s32Ret);
		//return HI_FAILURE;
	}

	/*获取sessionid*/
	s32Ret = RTSP_Get_SessId(pSess->au8RecvBuff,aszSessID);
	if(HI_SUCCESS != s32Ret)
	{
		printf("<RTSP>RTSP_Get_SessId failed:%x",s32Ret);
		RTSP_err_code = RTSP_STATUS_BAD_REQUEST;
		/*发送错误回复消息，并关闭socket*/
		strncpy(aszSessID,pSess->aszSessID,RTSP_MAX_SESSID_LEN);
		//RTSP_Send_Reply(pSess, 400, NULL, 1);
		//return HI_FAILURE;
	}

	// pSess->bIsTransData = HI_FALSE;
	pTemp = pSess->au8SendBuff;
	pTemp += RTSP_Make_RespHdr(pSess, 200);
	pTemp += sprintf(pTemp,"Session: %s\r\n\r\n", pSess->aszSessID);
	RTSP_Send_Reply(pSess, 200, NULL, 0);
	return HI_SUCCESS;
}


/*
rfc2326bis06-Page42 SETUP can be used in all three states; INIT,and READY
InitState:
    Describe: --> init_state
    Setup   : --> *ready_state
    Teardown: --> init_state
    Options : --> init_state
    Play    : --> init_state
    Pause   : --> init_state

ReadyState:
    Play    : --> *play_state
    Setup   : --> ready
    Teardown: --> ??
    Options : --> 
    pause   : -->
    describe: --> 

*/

HI_S32 RTSP_Handle_Event(RTSP_LIVE_SESSION *pSess, int event, HI_U32 status, char *buf)
{
    HI_S32 iRet = 0;
    switch (event) 
    {
        case RTSP_OPTIONS_METHOD:
            iRet = RTSP_Handle_Options(pSess);
            break;
        case RTSP_DISCRIBLE_METHOD:
            iRet = RTSP_Handle_Discrible(pSess);
            break;
        case RTSP_SETUP_METHOD:
            iRet = RTSP_Handle_Setup(pSess);
            break;
        case RTSP_PLAY_METHOD:
            iRet = RTSP_Handle_Play(pSess);
            break;
        case RTSP_PAUSE_METHOD:
            iRet = RTSP_Handle_Pause(pSess);
            break;
        case RTSP_TEARDOWN_METHOD:
            iRet = RTSP_Handle_Teardown(pSess);
            break;
		case RTSP_SET_PARAM_METHOD:
            iRet = RTSP_Handle_SetParam(pSess);
            break;
        case RTSP_REQUEST_I_FRAME:
            iRet = RTSP_Handle_RequestIFrame(pSess);
            break;
        case RTSP_GET_PARAM_METHOD:
            iRet = RTSP_Handle_GetParam(pSess);
            break;
		case RTSP_HEARTBEAT_METHOD:
			iRet = RTSP_HeartBeat_Pause(pSess);
			break;
    	default:
    		RTSP_Send_Reply(pSess, 501, NULL, 1);
    		iRet = HI_ERR_RTSP_PARSE_METHOD_ERROR;
            break;
    }

//	printf("#### %s %d, event:%d\n", __FUNCTION__, __LINE__, event);
	return iRet;
}

/*****************************************************************************
 Prototype       : RTSPMethodProcess
 Description     : rtsp 协商处理函数
 Input           : pSess  **
 Output          : None
 Return Value    : 
 Global Variable   
    Read Only    : 
    Read & Write : 
  History         
  1.Date         : 2006/5/20
    Author       : T41030
    Modification : Created function
  2.Date         : 2007/5/13
    Author       : W54723
    Modification : Modify function
*****************************************************************************/
HI_S32 RTSPMethodProcess(RTSP_LIVE_SESSION *pSess)
{
    HI_S32   s32Ret = 0;
    HI_S32   opcode;
    HI_S32   rep;
    HI_U32   seq_num;
    HI_U32   status;
    HI_S32   Cseq = 0;

    if (pSess == NULL)
    {
        HI_LOG_DEBUG(RTSP_LIVESVR, LOG_LEVEL_CRIT, "RTSP process, but input session is null.\n");
        return HI_ERR_RTSP_ILLEGALE_PARAM;
    }
    
    HI_LOG_DEBUG(RTSP_LIVESVR, LOG_LEVEL_NOTICE, "Recive C(%s) --> S >>>>>>\n"
                   "%s\n"
                   "<<<<<<\n" ,
                   pSess->au8CliIP, pSess->au8RecvBuff);

    pSess->heartTime = 0;              
    /* check for request message. */
    rep = RTSP_Valid_RespMsg(pSess->au8RecvBuff, &seq_num, &status);

    if ( rep == HI_RTSP_PARSE_ISNOT_RESP)
    {  
        /* not a response message, check for method request. */
        s32Ret = RTSP_Get_Method( pSess->au8RecvBuff,&opcode);
        if(s32Ret != HI_SUCCESS)
        {
            //HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"3.RTSPMethodProcess Method requested was invalid.  Message discarded."
            //       "Method = %s\n", pSess->au8RecvBuff );

			//Itlv rtsp的TCP模式传输
			//HI_RTCP_recv_packet(pSess->au8RecvBuff);

			return HI_SUCCESS;
			//return HI_RTSP_Parse_Itlv_Packet(pSess->au8RecvBuff);
        }
        s32Ret = RTSP_Get_CSeq(pSess->au8RecvBuff,&Cseq);
        if (HI_SUCCESS == s32Ret )
        {
            pSess->last_recv_seq= Cseq;
            //HI_LOG_DEBUG(HI_RTSP_MODEL, LOG_LEVEL_DEBUG, "Cseq: %d \n", iCseq);
        }

        status = 0;
    }
    else
    {  
        /* response message was valid.  Derive opcode from request- */
        /* response pairing to determine what actions to take. */
#if 0
		if (seq_num != pSess->last_send_seq + 1)
		{
			HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_DEBUG,"Last requse send by me sn is: %d, but resp seq is", pSess->last_send_seq , seq_num  );
			return HI_ERR_RTSP_SEQ;
		}
#endif

#if 0
		opcode = RTSP_MAKE_RESP_CMD(pSess->last_send_req);
		if ( status > HI_RTSP_BAD_STATUS_BEGIN )
		{
			HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_DEBUG,"Response had status = %d.\n", status );
		}
#endif
        return HI_SUCCESS;
    }

    return RTSP_Handle_Event(pSess,  opcode, status, pSess->au8RecvBuff);
}

HI_S32 GetValueFromObject(const char *pObject, const char *pNode)
{
	int chn = 0;
	char *p1 = NULL;
	char szChn[8] = {0};
	int i = 0;

	if(NULL == pObject || NULL == pNode)
	{
		return HI_FAILURE;
	}

	p1 = strstr(pObject,pNode);
	if(NULL == p1)
	{
		return HI_FAILURE;
	}
	p1 += strlen(pNode);
	p1++;//去掉'='
	while(*p1 != '&' && *p1 != '\0')
	{
		szChn[i] = *p1;
		i++;
		p1++;
	}

	chn = atoi(szChn);
	return chn;
}

HI_S32 HI_RTSP_Process(RTSP_LIVE_SESSION * pSess)
{
    HI_S32 s32RTSPProcRet = HI_FAILURE;
    HI_S32 s32SendRespRet  = HI_FAILURE;

    //UpdateUrl(pSess->au8RecvBuff); 
    s32RTSPProcRet = RTSPMethodProcess(pSess); 
    if (  s32RTSPProcRet != HI_SUCCESS && s32RTSPProcRet != RTSP_SEEK_FLAG) 
    {
         HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE, "RTSPMethodProcess process error =%X",s32RTSPProcRet);
    }
	
    memset(pSess->au8RecvBuff,0,RTSP_MAX_PROTOCOL_BUFFER);
    /*2 send the reponse message*/        
    if (pSess->readyToSend == HI_TRUE)
    {
        s32SendRespRet = HI_RTSP_Send(pSess->s32SessSock, pSess->au8SendBuff, strlen(pSess->au8SendBuff)) ;
        if (HI_SUCCESS != s32SendRespRet)
        {
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"to send RTSP response %d byte to cli %s thought sock %d failed\n",
                       strlen(pSess->au8SendBuff),pSess->au8CliIP,pSess->s32SessSock);
           s32SendRespRet = HI_ERR_RTSP_SEND_FAILED;
        }
        else
        {
            HI_LOG_DEBUG(RTSP_LIVESVR,LOG_LEVEL_NOTICE, "Send Reply .Chn:%s S --> C(%s) >>>>>>\n"
                           "%s\n"
                           "<<<<<<\n",
                pSess->auFileName, pSess->au8CliIP,pSess->au8SendBuff);    
        }
        
        HI_RTSP_ClearSendBuff(pSess);

        /*updat read to send flag as false*/
        pSess->readyToSend = HI_FALSE;
    }
    else
    {
        s32SendRespRet = HI_SUCCESS;
    }
 //   printf("----------s32RTSPProcRet =%X,s32SendRespRet = %X----------\n",s32RTSPProcRet,s32SendRespRet);
    /*eithor  process RTSP queset worng or send its messge wrong, we return 
    error  code, and wrong of prcess RTSP queset is top-priority */
    /*if proces message is wrong, return its error code*/
    if(HI_SUCCESS != s32RTSPProcRet)
    {
        return s32RTSPProcRet;
    }
    /*if sendmessage is wrong, return its error code*/
    else if (HI_SUCCESS != s32SendRespRet)
    {
        return s32SendRespRet;
    }

    return HI_SUCCESS;  
}


/*configrate read set's timeval*/
HI_VOID RTSPConfigTimeval(RTSP_LIVE_SESSION* pSession,struct timeval* pTimeoutVal)
{
    RTSP_SESSION_STATE enSessStat = RTSP_SESSION_STATE_BUTT;  /*RTSP session state*/
    
    enSessStat = pSession->enSessState;

    /*1. if RTSP session procedure has not complete, 
        1)will at least one message come
        2)haven't send media data yet 
    then should wait for enough time ,so timeval:10sec*/
    if(RTSP_SESSION_STATE_PLAY != enSessStat)
    {
        printf("%s:just RTSP msg.\n",pSession->aszSessID);
        pTimeoutVal->tv_sec = 10;
        pTimeoutVal->tv_usec = 0;
    }
    /*2. if RTSP procedure has completed, and media data is not interleaved trans type
        1)maybe there will be message come or not,
        2)will not send data in RTSP session thread;
      then should wait for a long enough time, oterwise, the RTSP session thread is busywaiting,
      so timeval 10sec*/
    else if(RTSP_SESSION_STATE_PLAY == enSessStat && HI_FALSE == pSession->bIsTransData)
    {
        printf("%s:ready to send,but not trans data in RTSP thread.\n",pSession->aszSessID);
        pTimeoutVal->tv_sec = 10;
        pTimeoutVal->tv_usec = 0;
    }
    else if(RTSP_SESSION_STATE_PLAY == enSessStat && MTRANS_MODE_MULTICAST == pSession->enMediaTransMode)
    {
        pTimeoutVal->tv_sec = 10;
        pTimeoutVal->tv_usec = 0;
    }
    
     /*3. if RTSP procedure has completed, and media data is interleaved trans type
        1)maybe there will be message come or not,
        2)will send data in RTSP session thread, and data should be send in time;
       then we should no wait if there is no msg comeing , oterwise, the data will not be 
      blcoked send untile spare recv select time,
      so timeval 0 msec*/
    else if(RTSP_SESSION_STATE_PLAY == enSessStat && HI_TRUE == pSession->bIsTransData)
    {
        //printf("%s:send data.\n",pSession->aszSessID);
        pTimeoutVal->tv_sec = 0;
        pTimeoutVal->tv_usec = 0;      
    }

    if(pSession->s32WaitDoPause && enSessStat == RTSP_SESSION_STATE_PAUSE)
    {
        pTimeoutVal->tv_sec = 0;
        pTimeoutVal->tv_usec = 0;    
    }
    
}
/*
GET_PARAMETER rtsp://example.com/fizzle/foo RTSP/1.0
           CSeq: 431
           Content-Type: text/parameters
           Session: 12345678
           Content-Length: 15

           packets_received
           jitter

*/
#define GET_PARAMETER "GET_PARAMETER"
#define PING_METHOD "PING"


HI_S32 RTSP_SEND_GetParameter(RTSP_LIVE_SESSION* pSess)
{
	HI_S32 s32Ret = 0;
	HI_RTSP_ClearSendBuff(pSess);

    if(pSess->heartTime >= 4)
    {
        printf("<RTSP> have not recv the msg response ,the conn is disconnected\n");
        return HI_FAILURE;
    }
    char *pTmp = pSess->au8SendBuff;

    pTmp += sprintf(pTmp, "%s %s %s\r\n", GET_PARAMETER, "*",RTSP_VER_STR );
    //if(pSess->last_recv_seq != 0)
    //{
    pSess->last_send_seq ++;
    pTmp += sprintf(pTmp, "Cseq: %d\r\n",pSess->last_send_seq);
    //}
    pTmp += sprintf(pTmp,"Session: %s\r\n\r\n",pSess->aszSessID);
	s32Ret = HI_RTSP_Send(pSess->s32SessSock,pSess->au8SendBuff,strlen(pSess->au8SendBuff));
	pSess->heartTime ++;
    
	printf("\n S --> C :%s\n",pSess->au8SendBuff);
	return s32Ret;

	
}
#if 0 
const char *DDAL_NET_IF_NAME = "eth0";

HI_S32 RTSP_TEST_ETH(int skfd)
{
	
	struct ifreq ifr;
	struct ethtool_value edata;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, DDAL_NET_IF_NAME);

	edata.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (char *)&edata;

	if (!(ioctl(skfd, SIOCETHTOOL, &ifr)<0)) 
	{
		if(edata.data)
		{
			return HI_SUCCESS;
		}
		else
		{
			printf("socket is disconnected\n");
			return HI_FAILURE;
		}
	} 
	else 
	{
		printf("socket is error\n");
		return HI_FAILURE;
	}
}

#endif

HI_S32 rtsp_keep_alive = 0;

HI_VOID setRtspKeepAlive()
{
    FILE* fp = NULL;
    fp = fopen("rtsp_keep_alive", "r");
    if (fp)
    {
	    fscanf(fp , "%d", &rtsp_keep_alive);
        printf("get flag:%d\n", rtsp_keep_alive);
    	fclose(fp);
    }
    else
	{
		perror("open file error\n");
	}
    return ;
}


#define MAX_KEEP_ALIVE 1000

/*process client stream request: rtsp process and send stream*/
static int tttmp = 0;
HI_VOID* RTSPSessionProc(HI_VOID * arg)
{
    HI_S32      s32Ret      = 0;
    HI_S32      s32RTSPStopRet = 0;
    HI_S32      s32Errno    = 0;                        /*linux system errno*/
    HI_SOCKET   s32TempSessSock = -1;
    HI_S32      s32RecvBytes = 0;
    HI_IP_ADDR  au8CliIP;
    HI_S32     s32SessThdId  = 0;
    RTSP_LIVE_SESSION *pSession = NULL;
    HI_U64     keepAlive = 0;/**/
    HI_S32 s32KeepAlive = 0;
    memset(au8CliIP,0,sizeof(HI_IP_ADDR));

    fd_set read_fds;                           /* read socket fd set */    
    struct timeval TimeoutVal;                      /* Timeout value */
    struct timeval TempTimeoutVal;
    memset(&TimeoutVal,0,sizeof(struct timeval));
    memset(&TempTimeoutVal,0,sizeof(struct timeval));

    char cName[128] = {0};
    sprintf(cName,"RtspSesProc");
    prctl(PR_SET_NAME, (unsigned long)cName, 0,0,0);

    struct MTRANS_PlayBackInfo stPbInfo;
    memset(&stPbInfo, 0, sizeof(stPbInfo));

    stPbInfo.tick = GetTick();

    if(arg == NULL)
    {
        printf("the RTSP session is null in RTSPSessionProc \n");
        return NULL;
    }
        
    pSession = (RTSP_LIVE_SESSION *)arg;
    
    s32TempSessSock = pSession->s32SessSock;
    s32SessThdId = pSession->s32SessThdId;
    memcpy(au8CliIP,pSession->au8CliIP,sizeof(HI_IP_ADDR));

    /*init read fds and set the session's session Sock into the set*/
    FD_ZERO(&read_fds);
    /*1 first: process the first msg recved by  RTSP listen server(here is webserver)*/
    if(HI_TRUE == pSession->bIsFirstMsgHasInRecvBuff)
    {
        printf("first msg come! SessID=%s!\n",pSession->aszSessID);
        s32Ret = HI_RTSP_Process(pSession);

        /*if the msg is teardown,the close the session*/
        if(s32Ret == RTSP_TEAR_DOWN_END_FALG)
        {
            /*destroy the session */    
            HI_RTSP_SessionClose(pSession);
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_INFO," client teardown %s disconnected the session \n",au8CliIP);
            return HI_NULL;
        }
        
        else if(HI_SUCCESS != s32Ret)
        {
            /*notify VOD destroyVODTask*/
            s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
            if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
            {
                DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
            }
            /*destroy the session */    
            HI_RTSP_SessionClose(pSession);
            
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"client %s disconnected (%d client alive):[RTSP process: %X]\n",
                      au8CliIP,HI_RTSP_SessionGetConnNum(),s32Ret);
            return HI_NULL;
        }
    }

    HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"[Info]client %s connected (%d client alive) !!!\n",
               au8CliIP,HI_RTSP_SessionGetConnNum());

    //HI_LOG_DEBUG(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"[Info]RTSP live process thread %d start ok, communite with cli %s....\n",
    //            s32SessThdId, au8CliIP);
	HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"[Info]RTSP live process thread %d start ok, communite with cli %s....\n",
				   s32SessThdId, au8CliIP);

    //multicast only
    HI_U8 TaskFlag = 0;
    int playing_count = 0;
    
    /*process RTSP live session*/
    while(RTSPSESS_THREAD_STATE_RUNNING == pSession->enSessThdState)
    {
        /*一 process RTSP session */
        /*clear the read set*/
        FD_ZERO(&read_fds);

        /*reset the read set */
        FD_SET(s32TempSessSock, &read_fds);

        /*important:config select timeval.different state should wait for differ time*/
        RTSPConfigTimeval(pSession, &TimeoutVal);
        memcpy(&TempTimeoutVal,&TimeoutVal,sizeof(struct timeval));

        /*3. jude if there is  message come 
        note: select will return inmediately once there is no msg coming*/
        //if(RTSP_SESSION_STATE_PLAY != pSession->enSessState)
        //{
        if(pSession->enSessState == RTSP_SESSION_STATE_PLAY
            && MTRANS_MODE_MULTICAST == pSession->enMediaTransMode
            && playing_count<3)
        {
            playing_count++;
            TimeoutVal.tv_sec = 10;//2;
        }
        
        s32Ret = select(s32TempSessSock+1,&read_fds,NULL,NULL,&TimeoutVal);

        /*3.1 some wrong with network*/
        if ( s32Ret < 0 ) 
        {
            s32Errno = errno;
            /*notify VOD destroyVODTask*/
            s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
            if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
            {
                DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
            }
            HI_RTSP_SessionClose(pSession);

            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_FATAL,"client %s disconnected (%d client alive):[select: %s]\n",
                au8CliIP,HI_RTSP_SessionGetConnNum(),strerror(s32Errno));
            printf("Exit  RTSPSessionProc  !\n");
            return HI_NULL;
        }
        /*3.2 over select time*/
        else if(0 == s32Ret)
        {
            /*overtime:RTSP session has not completed, and has wait for a long time,
            here believe some wrong with network,so close the link and withdraw the thread. 
            */
            if(RTSP_SESSION_STATE_PLAY != pSession->enSessState 
                && RTSP_SESSION_STATE_PAUSE != pSession->enSessState)
            {
                /*notify VOD destroyVODTask*/
                s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                if((HI_SUCCESS != s32RTSPStopRet)&&(s32Ret != RTSP_TEAR_DOWN_END_FALG))
                {
                    DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                }
                HI_RTSP_SessionClose(pSession);

                HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_FATAL," client %s disconnected (%d client alive):"\
                    "[not complete RTSP session,select: overtime %d.%ds)]\n",
                    au8CliIP,HI_RTSP_SessionGetConnNum(), TempTimeoutVal.tv_sec,TempTimeoutVal.tv_usec);
                printf("Exit  RTSPSessionProc  !\n");
                return HI_NULL;
            }
            /*overtime:RTSP session has completed, may be there is no message come for a long time, 
            so continue to wait. (wait timeval see function configSet)
            */
            else
            {
                //do nothing, just go to ProcTrans, if media data is interleaved transed. 
            }
        }
        /*3.3 process*/
        else if(s32Ret > 0)
        {
            /*3.3.1 there is msg come, then recv the msg and process it*/
            if(FD_ISSET(s32TempSessSock, &read_fds))
            {
                memset(pSession->au8RecvBuff,0,RTSP_MAX_PROTOCOL_BUFFER);
				//printf("Accapt socket =%d \n",s32TempSessSock);
                // to do :recv 应封装并加超时
                s32RecvBytes = recv(s32TempSessSock, pSession->au8RecvBuff, RTSP_MAX_PROTOCOL_BUFFER, 0);
                if (s32RecvBytes < 0 )
                {
                    s32Errno = errno;
                    /*notify VOD destroyVODTask*/
                    s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                    if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
                    {
                        DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                    }
                    HI_RTSP_SessionClose(pSession);

                    HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR, 
                        "client %s disconnected (%d client alive):[RTSP msg read from socket: %s]\n",
                        pSession->au8CliIP,HI_RTSP_SessionGetConnNum(),strerror(s32Errno));
                    printf("Exit  RTSPSessionProc  !\n");
                    return HI_NULL;
                }
                /*peer close*/
                else if (s32RecvBytes == 0)
                {
                    /*notify VOD destroyVODTask*/
                    s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                    if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
                    {
                        DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                    }
                    HI_RTSP_SessionClose(pSession);

                    HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR,"client %s disconnected (%d client alive):[peer closed].\n",
                        au8CliIP, HI_RTSP_SessionGetConnNum());
                    printf("Exit  RTSPSessionProc  !\n");
                    return HI_NULL;             
                }
                /*process the request msg and send its response*/
                else
                {
                	if (s32RecvBytes >800)
                		HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_INFO," recv %d msg>800 msg=%s\n",s32RecvBytes,pSession->au8RecvBuff);
                    s32Ret = HI_RTSP_Process(pSession);

                    if(s32Ret == RTSP_SEEK_FLAG)
                    {
                        stPbInfo.cmd = RTSP_SEEK_FLAG;
                    }
                    /*if the msg is teardown,the close the session*/
                    else if(s32Ret == RTSP_TEAR_DOWN_END_FALG)
                    {
                        /*destroy the session */    
                        HI_RTSP_SessionClose(pSession);
                        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_INFO," client teardown %s disconnected the session \n",au8CliIP);
                        printf("Exit  RTSPSessionProc  !\n");
                        return HI_NULL;
                    }
                    else if(HI_SUCCESS != s32Ret)
                    {
                        /*notify VOD destroyVODTask*/
                        s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                        if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
                        {
                            DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                        }
                        /*destroy the session */    
                        HI_RTSP_SessionClose(pSession);

                        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR," client %s disconnected (%d client alive):[RTSP process: %X]\n",
                            au8CliIP,HI_RTSP_SessionGetConnNum(),s32Ret);
                        printf("Exit  RTSPSessionProc  !\n");
                        return HI_NULL;
                    }
                }
            }
            /*select readset ok, but soket not in read set, something wrong,so withdraw*/
            else
            {
                /*notify VOD destroyVODTask*/
                s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
                {
                    DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                }
                /*destroy the session */    
                HI_RTSP_SessionClose(pSession);
                /*to do :notify VOD destroyVODTask*/
                HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR," client %s disconnected (%d client alive):[something wrong in network ]\n",
                    au8CliIP,HI_RTSP_SessionGetConnNum());
                printf("Exit  RTSPSessionProc  !\n");
                return HI_NULL;
            }
        }
        //}
        /*二 process media data trans. just when RTSP session is in playing state,
        and media data is interleaved transed */
        if(0 == s32KeepAlive)
        {
            int keepIdle = 8;/* 开始发送KEEP ALIVE数据包之前经历的时间 */
            int keepInterval = 10;/* KEEP ALIVE数据包之前间隔的时间 */
            int keepCount = 5;/* 重试的最大次数 */

            s32KeepAlive = 1;
            setsockopt(s32TempSessSock,SOL_SOCKET,SO_KEEPALIVE,(void*)&s32KeepAlive, sizeof(s32KeepAlive));
            setsockopt(s32TempSessSock,IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle));
            setsockopt(s32TempSessSock,IPPROTO_TCP,TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
            setsockopt(s32TempSessSock,IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
        }
        
        if((RTSP_SESSION_STATE_PLAY == pSession->enSessState && HI_TRUE == pSession->bIsTransData)
            || pSession->s32WaitDoPause)
        {
            //检查多播线程的状态
            if(MTRANS_MODE_MULTICAST == pSession->enMediaTransMode)
            {
                if(MTRANS_GetTaskStatusMulticast(pSession->u32TransHandle) == 0)
                {
                    TaskFlag++;
                }

                if(TaskFlag>=2)
                {
                    s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                    if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
                    {
                        DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                    }
                    HI_RTSP_SessionClose(pSession);
                    HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_FATAL,"client %s disconnected (%d client alive):[send media data error %X]\n",
                               au8CliIP, HI_RTSP_SessionGetConnNum(),s32Ret);
                    printf("Exit  RTSPSessionProc  !\n");
                    return HI_NULL;
                }
                
            }
            else
            {
                if(pSession->bIsPlayback == HI_FALSE)
                {
                    /*prodess media trans task:send media data which in interleaved type*/
                    s32Ret = pSession->CBonTransMeidaDatas(pSession->u32TransHandle,0);
					tttmp ++;
                }
                else
                {
                    s32Ret = MTRANS_playback_send(pSession->u32TransHandle, &stPbInfo);
                }

                /*something wrong in send data*/

                if(s32Ret != HI_SUCCESS && s32Ret != MTRANS_SEND_IFRAME)
                {
                    if(HI_ERR_MTRANS_NO_DATA != s32Ret 
                        || (pSession->bIsPlayback && HI_ERR_MTRANS_NO_DATA == s32Ret))
                    {
                        /*notify VOD destroyVODTask*/
                        s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                        if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
                        {
                            DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                        }
                        HI_RTSP_SessionClose(pSession);
                        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_FATAL,"client %s disconnected (%d client alive):[send media data error %X]\n",
                                   au8CliIP, HI_RTSP_SessionGetConnNum(),s32Ret);
                        printf("Exit  RTSPSessionProc  !\n");
                        return HI_NULL;
                    }
                }

                if(s32Ret == MTRANS_SEND_IFRAME)
                {
                    pSession->last_send_frametype = 1;
                }
                else
                {
                    pSession->last_send_frametype = 0;
                }

                if(pSession->s32WaitDoPause && s32Ret != MTRANS_SEND_IFRAME)
                {
                    pSession->s32WaitDoPause = 0;
                    s32Ret = RTSP_Do_Pause(pSession);
                    if(s32Ret != HI_SUCCESS)
                    {
                        s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
                        if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
                        {
                            DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
                        }
                        /*destroy the session */    
                        HI_RTSP_SessionClose(pSession);

                        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERROR," client %s disconnected (%d client alive):[RTSP process: %X]\n",
                            au8CliIP,HI_RTSP_SessionGetConnNum(),s32Ret);
                        printf("Exit  RTSPSessionProc  !\n");
                        return HI_NULL;
                    }
                }
                
            }
            
            if(rtsp_keep_alive == 1)
            {
                keepAlive++;
                if(keepAlive >= MAX_KEEP_ALIVE)
                {
                    s32Ret = RTSP_SEND_GetParameter(pSession);
                    if(HI_SUCCESS != s32Ret)
                    {
                        printf("RTSP_SEND_GetParameter send failed :%x the client:%s is disconnected\n",s32Ret,pSession->au8CliIP);
                        break;
                    }
#if 0
                    s32Ret = RTSP_TEST_ETH(pSession->s32SessSock);
                    if(HI_SUCCESS != s32Ret)
                    {
                        printf("con %sis disconnected\n",pSession->au8CliIP);
                        break;
                    }
#endif
                    keepAlive = 0;
                }
            }
        }
        else if(RTSP_SESSION_STATE_PLAY == pSession->enSessState )
        {
            //send the media date by udp
        }
    }   /*end of while()*/     

    /*notify VOD destroyVODTask*/
    s32RTSPStopRet = RTSP_Handle_Teardown(pSession);
    if((HI_SUCCESS != s32RTSPStopRet)&&(s32RTSPStopRet != RTSP_TEAR_DOWN_END_FALG))
    {
        DEBUG_RTSP_LIVE_PRINT(RTSP_LIVESVR,LOG_LEVEL_ERROR,"RTSP_Handle_Teardown error = %X\n",s32RTSPStopRet);
    }
    HI_RTSP_SessionClose(pSession);
    HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"client %s disconnected (%d client alive):[some operation happened,"\
               "such as root, set video size ... ]\n",au8CliIP,HI_RTSP_SessionGetConnNum());

    HI_LOG_DEBUG(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"[Info]thd %d withdraw: stop process with client %s... \n",
                 s32SessThdId, au8CliIP);

    printf("Exit  RTSPSessionProc  !\n");

    return HI_NULL;
}

HI_S32 HI_RTSP_LiveSvr_Init_11(HI_S32 s32MaxConnNum, HI_U16 u16Port,NW_ClientInfo * clientinfo)
{
	HI_S32 s32Ret         = 0;
	HI_U32 *pu32LisnSvrHandle = NULL;

	

	if( 0 >= s32MaxConnNum)
	{
		s32MaxConnNum = RTSP_MAX_SESS_NUM;
	}
	if(u16Port == 0)
	{
		return HI_ERR_RTSP_SVR_ILLEGALE_PARAM;
	}
	//mhua debug
	// SPS_PPS_CAL();

	/* clear RTSP server struct*/
	memset(&g_struRTSPSvr,0,sizeof(RTSP_LIVE_SVR_S));
	g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_STOP; 
	


		
	if(u16Port > 0)
	{
		/*3  init RTSP lisen svr*/
		/*webserver is treated as RTSP live listen server */
		
		s32Ret = HI_MSESS_LISNSVR_StartSvr_11(u16Port,RTSP_MAX_SESS_NUM,&pu32LisnSvrHandle,LISNSVR_TYPE_RTSP,clientinfo);
		if(HI_SUCCESS != s32Ret)
		{
			HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_CRIT,
			            "RTSP Listen svr Init error=%d,"\
			            "instread, user can connet through web server listen svr.\n",s32Ret);
		}
		else
		{
			g_struRTSPSvr.pu32RTSPLisnSvrHandle = pu32LisnSvrHandle;
		}
	}
	/*5 update vod svr's state as running*/
	g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_Running;
	setRtspKeepAlive();
	
	return HI_SUCCESS;
}


/*RTSP live server init*/
HI_S32 HI_RTSP_LiveSvr_Init(HI_S32 s32MaxConnNum, HI_U16 u16Port)
{
	HI_S32 s32Ret         = 0;
	HI_U32 *pu32LisnSvrHandle = NULL;

	if(RTSP_LIVESVR_STATE_Running == g_struRTSPSvr.enSvrState)
	{
		HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"RTSP LiveSvr Init Failed .\n");
		return HI_ERR_RTSP_SVR_ALREADY_RUNNING;
	}

	if( 0 >= s32MaxConnNum)
	{
		s32MaxConnNum = RTSP_MAX_SESS_NUM;
	}
	if(u16Port == 0)
	{
		return HI_ERR_RTSP_SVR_ILLEGALE_PARAM;
	}
	//mhua debug
	// SPS_PPS_CAL();

	/* clear RTSP server struct*/
	memset(&g_struRTSPSvr,0,sizeof(RTSP_LIVE_SVR_S));
	g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_STOP; 

	/*2 init RTSP session list*/
	s32Ret = HI_RTSP_SessionListInit(s32MaxConnNum);
	if(HI_SUCCESS != s32Ret)
	{
		HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,
		        "RTSP LiveSvr Init Failed (Session Init Error=%d).\n",s32Ret);
		return s32Ret;
	}

	if(u16Port > 0)
	{
		/*3  init RTSP lisen svr*/
		/*webserver is treated as RTSP live listen server */
		s32Ret = HI_MSESS_LISNSVR_StartSvr(u16Port,RTSP_MAX_SESS_NUM,&pu32LisnSvrHandle,LISNSVR_TYPE_RTSP);
		if(HI_SUCCESS != s32Ret)
		{
			HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_CRIT,
			            "RTSP Listen svr Init error=%d,"\
			            "instread, user can connet through web server listen svr.\n",s32Ret);
		}
		else
		{
			g_struRTSPSvr.pu32RTSPLisnSvrHandle = pu32LisnSvrHandle;
		}
	}
	/*5 update vod svr's state as running*/
	g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_Running;
	setRtspKeepAlive();
	HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"RTSP Live svr Init ok.\n");

	return HI_SUCCESS;
}


/*live deinit: waiting for every 'RTSP sesssion process' thread to withdraw*/
HI_S32 HI_RTSP_LiveSvr_DeInit_11()
{
    HI_S32 s32Ret = 0;
    /*
    if(RTSP_LIVESVR_STATE_STOP == g_struRTSPSvr.enSvrState)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"RTSP LiveSvr DeInit Failed:already deinited.\n");
        return HI_ERR_RTSP_SVR_ALREADY_STOPED;
    }*/
    
    /*1 stop the RTSPlisen svr*/
    
    g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_STOP;
	printf("HI_MSESS_LISNSVR_StopSvr_11 start\n");
    s32Ret = HI_MSESS_LISNSVR_StopSvr_11(g_struRTSPSvr.pu32RTSPLisnSvrHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_CRIT,
                    "RTSP Listen svr deInit error=%X\n",s32Ret);
    }
	printf("HI_RTSP_SessionAllDestroy strart");
    /*2 stop all 'RTSPSessionProcess' thread,close the link, free all session */
    HI_RTSP_SessionAllDestroy();
    
    /*4 set vod svr as stop, */

    // to do :??so getstream thread will not write data to mbuff*/
    //g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_STOP;
    HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"RTSP Live DeInit ok ...");

    return HI_SUCCESS;

}


/*live deinit: waiting for every 'RTSP sesssion process' thread to withdraw*/
HI_S32 HI_RTSP_LiveSvr_DeInit()
{
    HI_S32 s32Ret = 0;
    
    if(RTSP_LIVESVR_STATE_STOP == g_struRTSPSvr.enSvrState)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"RTSP LiveSvr DeInit Failed:already deinited.\n");
        return HI_ERR_RTSP_SVR_ALREADY_STOPED;
    }
    
    /*1 stop the RTSPlisen svr*/
    
    g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_STOP;
    s32Ret = HI_MSESS_LISNSVR_StopSvr(g_struRTSPSvr.pu32RTSPLisnSvrHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_CRIT,
                    "RTSP Listen svr deInit error=%X\n",s32Ret);
    }

    /*2 stop all 'RTSPSessionProcess' thread,close the link, free all session */
    HI_RTSP_SessionAllDestroy();
    
    /*4 set vod svr as stop, */

    // to do :??so getstream thread will not write data to mbuff*/
    //g_struRTSPSvr.enSvrState = RTSP_LIVESVR_STATE_STOP;
    HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"RTSP Live DeInit ok ...");

    return HI_SUCCESS;

}

/*创建会话任务，若第一个会话消息已接收则使pMsgBuffAddr指向消息首地址,
 否则置pMsgBuffAddr为null*/
HI_S32 HI_RTSP_CreatRTSPLiveSession(HI_S32 s32LinkFd, HI_CHAR* pMsgBuffAddr,
                                HI_U32 u32MsgLen, HI_BOOL bIsTurnRelay)
{
    HI_S32 s32Ret = 0;
    RTSP_LIVE_SESSION *pSession = NULL;

    /*1 get a free session first*/
    s32Ret = HI_RTSP_SessionCreat(&pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,
                    "creat session for a new connect error=%d.\n",s32Ret);

        return s32Ret;            
    }
    else
    {
        /*init the session according the connection's socket */
        HI_RTSP_SessionInit(pSession,s32LinkFd);

       // printf("************sess=%X,sock=%d.**************\n",pSession,s32LinkFd);
    }

    if(bIsTurnRelay == HI_TRUE)
    {
        pSession->bIsTurnRelay = HI_TRUE;
    }

    /*2 copy first message of RTSP session*/
    if(NULL == pMsgBuffAddr)
    {
        /*no msg received yet*/
        pSession->bIsFirstMsgHasInRecvBuff = HI_FALSE;
    }
    /*has received first msg*/
    else
    {
        /*copy first msg to session msgBuff*/
        s32Ret = HI_RTSP_SessionCopyMsg(pSession,pMsgBuffAddr,u32MsgLen);
        if(HI_SUCCESS != s32Ret)
        {
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,
                        "copy first msg to session buff error=%d.\n",s32Ret);
            HI_RTSP_SessionClose(pSession);            
            return s32Ret;            
        }
        else
        {
             pSession->bIsFirstMsgHasInRecvBuff = HI_TRUE;
        }
     }

    /*init RTSP session process thread state as running*/
    pSession->enSessThdState = RTSPSESS_THREAD_STATE_RUNNING;

    /*3 creat thread 'liveProcess' which process rtsp and stream send */
#ifdef HI_PTHREAD_MNG
    s32Ret = HI_PthreadMNG_Create("HI_RTSP_LIVESESS_PRODESS", &(pSession->s32SessThdId),
		NULL, RTSPSessionProc, pSession);
#else
    {
        pthread_attr_t attr;  
        pthread_attr_init(&attr);  
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
        s32Ret = pthread_create((pthread_t *)&(pSession->s32SessThdId),
            &attr, RTSPSessionProc, pSession);
    }
#endif
    if (s32Ret != HI_SUCCESS)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_NOTICE,"Start RTSP session process thread error. cann't create thread.\n");
        pSession->enSessThdState = RTSPSESS_THREAD_STATE_STOP;
        HI_RTSP_SessionClose(pSession);
        return  HI_ERR_RTSP_CREAT_SESSION_THREAD_FAILED;
    }
    /*set thread's state as 'init' */
    /*to do:什么是detach,在这里有什么用*/
    else
    {
        /*detach every sending thread*/
        //HI_PthreadDetach(pSession->s32SessThdId);
    }
    
    return HI_SUCCESS;
}      

/*创建会话任务，若第一个会话消息已接收则使pMsgBuffAddr指向消息首地址,
 否则置pMsgBuffAddr为null*/
HI_S32 HI_RTSP_CreatRTSPLiveSession_11(HI_S32 s32LinkFd, HI_CHAR* pMsgBuffAddr,
                                HI_U32 u32MsgLen, HI_BOOL bIsTurnRelay)
{
    HI_S32 s32Ret = 0;
    RTSP_LIVE_SESSION *pSession = NULL;

    /*1 get a free session first*/
    s32Ret = HI_RTSP_SessionCreat(&pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,
                    "creat session for a new connect error=%d.\n",s32Ret);

        return s32Ret;            
    }
    else
    {
        /*init the session according the connection's socket */
        HI_RTSP_SessionInit(pSession,s32LinkFd);

       // printf("************sess=%X,sock=%d.**************\n",pSession,s32LinkFd);
    }

    if(bIsTurnRelay == HI_TRUE)
    {
        pSession->bIsTurnRelay = HI_TRUE;
    }

    /*2 copy first message of RTSP session*/
    if(NULL == pMsgBuffAddr)
    {
        /*no msg received yet*/
        pSession->bIsFirstMsgHasInRecvBuff = HI_FALSE;
    }
    /*has received first msg*/
    else
    {
        /*copy first msg to session msgBuff*/
        s32Ret = HI_RTSP_SessionCopyMsg(pSession,pMsgBuffAddr,u32MsgLen);
        if(HI_SUCCESS != s32Ret)
        {
            HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,
                        "copy first msg to session buff error=%d.\n",s32Ret);
            HI_RTSP_SessionClose(pSession);            
            return s32Ret;            
        }
        else
        {
             pSession->bIsFirstMsgHasInRecvBuff = HI_TRUE;
        }
     }

    /*init RTSP session process thread state as running*/
    pSession->enSessThdState = RTSPSESS_THREAD_STATE_RUNNING;

	RTSPSessionProc(pSession);    

    return HI_SUCCESS;
}   


/*接收一个由监听模块(webserver)分发的链接
 1 解析是否是RTSP 直播连接
 2 解析请求的直播方法
 3 按不同的方式分别处理各请求方法*/
HI_S32 HI_RTSP_DistribLink(HI_S32 s32LinkFd, HI_CHAR* pMsgBuffAddr,
                           HI_U32 u32MsgLen, HI_BOOL bIsTurnRelay)
{
    HI_S32   s32Ret  = HI_SUCCESS;
    HI_S32   methodcode = 0;
    HI_S32   rep;
    HI_U32   seq_num;
    HI_U32   status;
    HI_CHAR   OutBuff[512] = {0};
    HI_CHAR  *pTemp = NULL;
    HI_S32   errcode = RTSP_STATUS_OK;

    /*
    if(NULL == pMsgBuffAddr)
    {
        return HI_ERR_RTSP_ILLEGALE_PARAM;
    }
    */

    if(RTSP_LIVESVR_STATE_Running != g_struRTSPSvr.enSvrState)
    {
        HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"distribute link to RTSP svr failed:"\
                   "RTSP svr not start yet.\n");
        return HI_ERR_RTSP_SVR_ALREADY_STOPED;
    }

    if(pMsgBuffAddr && (u32MsgLen > 0))
    {
        /* check for request message. */
        rep = RTSP_Valid_RespMsg(pMsgBuffAddr, &seq_num, &status);

        if ( rep == HI_RTSP_PARSE_ISNOT_RESP )
        {  
            /* not a response message, check for method request. */
            s32Ret = RTSP_Get_Method( pMsgBuffAddr,&methodcode);
            if(s32Ret != HI_SUCCESS)
            {
                HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"4.Method requested was invalid.  Message discarded."
                       "Method = %s\n", pMsgBuffAddr);
                errcode = RTSP_STATUS_BAD_REQUEST;
            }
        }
        else
        {  
            HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"RTSP server not accept a RTSP response now.Message discarded."
                       "Method = %s\n", pMsgBuffAddr );

            printf("RTSP server not accept a RTSP response now.Message discarded Method = %s\n", pMsgBuffAddr);
            errcode = RTSP_STATUS_BAD_REQUEST;
        }

        /*如果前面任何一个步骤出现错误，则组织并返回返回错误信息并关闭socket*/
        if( RTSP_STATUS_OK != errcode)
        {
            pTemp = OutBuff;
            pTemp += sprintf(pTemp, "%s %d %s\r\n", RTSP_VER_STR, errcode,
                            RTSP_Get_Status_Str( errcode ));
            pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");
            pTemp += sprintf(pTemp,"Server: Hisilicon Ipcam\r\n");
            pTemp += sprintf(pTemp,"\r\n");

            /*发送回应信息*/
            s32Ret = HI_RTSP_Send(s32LinkFd,OutBuff,strlen(OutBuff));	/* bad request */
            if (HI_SUCCESS != s32Ret)
            {
                HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"request message for live is not a desired RTSP format, and send its response %d byte"
                           "thought sock %d failed\n",strlen(OutBuff),s32LinkFd);
            }

            /*关闭该链接*/
            close(s32LinkFd);

            return HI_ERR_RTSP_ILLEGALE_REQUEST;
        }
    }
   
    s32Ret = HI_RTSP_CreatRTSPLiveSession(s32LinkFd,pMsgBuffAddr,u32MsgLen, bIsTurnRelay);
    
    return s32Ret;
}



/*接收一个由监听模块(webserver)分发的链接
 1 解析是否是RTSP 直播连接
 2 解析请求的直播方法
 3 按不同的方式分别处理各请求方法*/
HI_S32 HI_RTSP_DistribLink_11(HI_S32 s32LinkFd, HI_CHAR* pMsgBuffAddr,
                           HI_U32 u32MsgLen, HI_BOOL bIsTurnRelay)
{
    HI_S32   s32Ret  = HI_SUCCESS;
    HI_S32   methodcode = 0;
    HI_S32   rep;
    HI_U32   seq_num;
    HI_U32   status;
    HI_CHAR   OutBuff[512*2] = {0};
    HI_CHAR  *pTemp = NULL;
    HI_S32   errcode = RTSP_STATUS_OK;

    /*
    if(NULL == pMsgBuffAddr)
    {
        return HI_ERR_RTSP_ILLEGALE_PARAM;
    }
    */

    if(pMsgBuffAddr && (u32MsgLen > 0))
    {
        /* check for request message. */
        rep = RTSP_Valid_RespMsg(pMsgBuffAddr, &seq_num, &status);

        if ( rep == HI_RTSP_PARSE_ISNOT_RESP )
        {  
            /* not a response message, check for method request. */
            s32Ret = RTSP_Get_Method( pMsgBuffAddr,&methodcode);
            if(s32Ret != HI_SUCCESS)
            {
                HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"4.Method requested was invalid.  Message discarded."
                       "Method = %s\n", pMsgBuffAddr);
                errcode = RTSP_STATUS_BAD_REQUEST;
            }
        }
        else
        {  
            HI_LOG_SYS(RTSP_LIVESVR, LOG_LEVEL_ERR,"RTSP server not accept a RTSP response now.Message discarded."
                       "Method = %s\n", pMsgBuffAddr );

            printf("RTSP server not accept a RTSP response now.Message discarded Method = %s\n", pMsgBuffAddr);
            errcode = RTSP_STATUS_BAD_REQUEST;
        }

        /*如果前面任何一个步骤出现错误，则组织并返回返回错误信息并关闭socket*/
        if( RTSP_STATUS_OK != errcode)
        {
            pTemp = OutBuff;
            pTemp += sprintf(pTemp, "%s %d %s\r\n", RTSP_VER_STR, errcode,
                            RTSP_Get_Status_Str( errcode ));
            pTemp += sprintf(pTemp,"Cache-Control: no-cache\r\n");
            pTemp += sprintf(pTemp,"Server: Hisilicon Ipcam\r\n");
            pTemp += sprintf(pTemp,"\r\n");

            /*发送回应信息*/
            s32Ret = HI_RTSP_Send(s32LinkFd,OutBuff,strlen(OutBuff));	/* bad request */
            if (HI_SUCCESS != s32Ret)
            {
                HI_LOG_SYS(RTSP_LIVESVR,LOG_LEVEL_ERR,"request message for live is not a desired RTSP format, and send its response %d byte"
                           "thought sock %d failed\n",strlen(OutBuff),s32LinkFd);
            }

            /*关闭该链接*/
            close(s32LinkFd);

            return HI_ERR_RTSP_ILLEGALE_REQUEST;
        }
    }
   
    s32Ret = HI_RTSP_CreatRTSPLiveSession_11(s32LinkFd,pMsgBuffAddr,u32MsgLen, bIsTurnRelay);
    
    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

