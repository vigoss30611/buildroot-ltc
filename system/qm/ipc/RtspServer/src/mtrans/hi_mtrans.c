/*****************************************************************************
  Copyright (C), 2007-, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_mtrans.c
  Version       : Initial Draft
  Author        : w54723
  Created       : 2008/02/13
  Description   : kernel process code of media transition model

  修改历史   :
  1.日    期   : 2008年10月31日
    作    者   : w54723
    修改内容   : 增加udp传输功能
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/select.h>
#include<sys/prctl.h>
#include "hi_mtrans.h"
#include "hi_mtrans_internal.h"
#include "hi_mtrans_task_list.h"
#include<sys/time.h>
#include "hi_buffer_abstract_define.h"
#include "hi_buffer_abstract_ext.h"
#include "hi_rtp.h"
#include "hi_vs_media_comm.h"
#include "hi_mtrans_error.h"
//btlib
//#include "hi_log_app.h"
//#include "hi_debug_def.h"
#include "ext_defs.h"
#include "hi_log_def.h"
//#include "hi_socket.h"
#include "hi_mt_socket.h"
#include "mjpeg_mtran.h"

void *multicast_send_thread(void *arg);
HI_CHAR * find_startcode( HI_CHAR * pDataAddr,unsigned int * DataLen);


MTRANS_ATTR_S g_stMtransAttr;

HI_VOID DEBUGPrtMtransCreat(MTRANS_SEND_CREATE_MSG_S *pstSendInfo,MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo)
{
   DEBUG_MTRANS_PRINT("MTRANS",LOG_LEVEL_ERROR," HI_MTRANS_CREAT \n"
                   "request:\n"
                    "au8CliIP = %s\n"
                    "u32MediaHandle= %p\n"
                    "u8MeidaType= %d\n"
                    "u32Ssrc= %d\n"
                    "enVideoFormat= %d\n"
                    "enAudioFormat= %d\n"
                    "enPackType= %d\n"
                    "enMediaTransMode = %d\n"
                    "--------tcp intlv---------\n"
                    "stTcpItlvTransInfo.s32SessSock= %d\n"
                    "stTcpItlvTransInfo.u32ItlvCliMDataId= %d\n"
                    "--------udp ---------\n"
                    "stUdpTransInfo.u32CliMDataPort = %d\n"
                    
                    "response:\n"
                    "s32ErrCode= %X\n"
                    "u32TransTaskHandle= %d\n"
                    "--------tcp intlv---------\n"
                    "stTcpItlvTransInfo.u32SvrMediaDataId= %d\n"
                    "stTcpItlvTransInfo.pProcMediaTrans= %p\n"
                    "--------udp ---------\n"
                    "stUdpTransInfo.u32SvrMediaSendPort= %d\n"
                    "stUdpTransInfo.pProcMediaTrans= %p\n\n",
                    
                    pstSendInfo->au8CliIP,
                    pstSendInfo->u32MediaHandle,
                    pstSendInfo->u8MeidaType,
                    pstSendInfo->u32Ssrc,
                    pstSendInfo->enVideoFormat,
                    pstSendInfo->enAudioFormat,
                    pstSendInfo->enPackType,
                    pstSendInfo->enMediaTransMode,
                    
                    pstSendInfo->stTcpItlvTransInfo.s32SessSock,
                    pstSendInfo->stTcpItlvTransInfo.u32ItlvCliMDataId,
                    
                    pstSendInfo->stUdpTransInfo.u32CliMDataPort,
                    
                    pstReSendInfo->s32ErrCode,
                    pstReSendInfo->u32TransTaskHandle,
                    
                    pstReSendInfo->stTcpItlvTransInfo.u32SvrMediaDataId,
                    pstReSendInfo->stTcpItlvTransInfo.pProcMediaTrans,

                    pstReSendInfo->stUdpTransInfo.u32SvrMediaSendPort,
                    pstReSendInfo->stUdpTransInfo.pProcMediaTrans
                   );         
}


HI_VOID DEBUGPrtMtransStart(MTRANS_START_MSG_S *pstStartSendInfo, MTRANS_ReSTART_MSG_S *pstReStartInfo)
{

   DEBUG_MTRANS_PRINT("MTRANS",LOG_LEVEL_NOTICE," HI_MTRANS_START \n"\
                   "request:\n"\
                    "u32TransTaskHandle = %X\n"\
                    "u8MeidaType= %d\n"\
                    
                    "response:\n"\
                    "s32ErrCode = %X\n",
                    
                    pstStartSendInfo->u32TransTaskHandle,
                    pstStartSendInfo->u8MeidaType,
                    pstReStartInfo->s32ErrCode
                   );

}

HI_VOID DEBUGPrtMtransStop(MTRANS_STOP_MSG_S *pstStopSendInfo,MTRANS_ReSTOP_MSG_S *pstReStopInfo)
{

   DEBUG_MTRANS_PRINT("MTRANS",LOG_LEVEL_NOTICE," HI_MTRANS_STOP \n"
                   "request:\n"
                    "u32TransTaskHandle= %X\n"
                    "u8MeidaType= %d\n"
                    "response:\n"
                    "s32ErrCode = %X\n",
                    pstStopSendInfo->u32TransTaskHandle,
                    pstStopSendInfo->u8MeidaType,
                    pstReStopInfo->s32ErrCode
                   );

}

HI_VOID DEBUGPrtMtransDestroy(MTRANS_DESTROY_MSG_S *pstDestroyInfo,MTRANS_ReDESTROY_MSG_S *pstReDestroyInfo)
{

   DEBUG_MTRANS_PRINT("MTRANS",LOG_LEVEL_NOTICE," HI_MTRANS_DESTROY \n"
                   "request:\n"
                    "u32TransTaskHandle = %X\n"
                    "response:\n"
                    "s32ErrCode = %X\n\n",
                    pstDestroyInfo->u32TransTaskHandle,
                    pstReDestroyInfo->s32ErrCode
                   );

}

HI_S32 DEBUGBeforeTransSave (HI_CHAR *pBuff, HI_U32 len,MBUF_PT_E mediaType, int type)
{
    /*!!!!!!!!!!!!!!!!!test: save file(struct)!!!!!!!!!!!*/

    char filename[255] = {0};
    FILE *pfile  = NULL;   
    HI_S32 WriteLen=0;

    if(NULL == pBuff)
    {
        return HI_FAILURE;
    }

    if(type == 1)
    {
        if(AUDIO_FRAME == mediaType)
        {
            sprintf(filename, "before_trans_audio.h264");
        }
        else if(VIDEO_KEY_FRAME == mediaType || VIDEO_NORMAL_FRAME ==mediaType)
        {
            sprintf(filename, "web/before_trans_video.h264");
        }
    }   
    else if (type == 2)
    {
        if(AUDIO_FRAME == mediaType)
        {
            sprintf(filename, "after_trans_audio.h264");
        }
        else if(VIDEO_KEY_FRAME == mediaType || VIDEO_NORMAL_FRAME ==mediaType)
        {
            sprintf(filename, "web/after_trans_video.h264");
        }
    }
    pfile = fopen(filename, "a+");
    if(NULL == pfile)
    {
        printf("StreamBeforeTrans : open file error\n");
        return HI_FAILURE;
    }
           
    WriteLen=fwrite(pBuff,len, 1, pfile);
    if ( 1 != WriteLen )
    {
        printf("StreamBeforeTrans : write file error: toLen:%u, aucLen:%d\n",len, WriteLen);
        fclose(pfile); pfile = NULL;
        

        //return HI_FAILURE;
    }
    
    fclose(pfile); pfile = NULL;
    return HI_SUCCESS;

}

int port_isfree (int port)  
{  
        struct sockaddr_in sin;  
        int                sock = -1;  
        int                ret = 0;  
        int                opt = 0;  
      
  
        memset (&sin, 0, sizeof (sin));  
        sin.sin_family = PF_INET;  
        sin.sin_port = htons (port);  
  
        sock = socket (PF_INET, SOCK_DGRAM, 0);  
        if (sock == -1)  
                return -1;  
   		ret = setsockopt (sock, SOL_SOCKET, SO_REUSEADDR,  
                          &opt, sizeof (opt));  //关闭端口复用再测试
        ret = bind (sock, (struct sockaddr *)&sin, sizeof (sin));  
        close (sock);  
  
        return (ret == 0) ? 1 : 0;  
}  

/*to do : 获取发送端口号需要互斥操作*/
HI_VOID MTANS_GetMediaSendPort(HI_U32 *pu32SendPort)
{
    HI_U32 u32MaxPort = 0;
    HI_U32 u32MinPort = 0;
    static HI_U32 u32CurrPort = 0;

    u32MinPort = g_stMtransAttr.u16MinSendPort;  
    u32MaxPort = g_stMtransAttr.u16MaxSendPort;
    (HI_VOID)pthread_mutex_lock(&(g_stMtransAttr.MutexGetPort));
	int i;
	for (i = 0;i < 20;i++) {
    /*在用户指定的一定范围内连续，环回分配*/
	    if(u32CurrPort < u32MaxPort && u32CurrPort >= u32MinPort)
	    {
	        u32CurrPort += 2;
	    }
	    else
	    {
	        u32CurrPort = u32MinPort;
	    }
		
		if ( port_isfree (u32CurrPort)  &&  port_isfree(u32CurrPort+1) )
			break;
		else
			printf("%s,%d port_isfree =0  port=%d or %d is used\n",__FILE__,__LINE__, u32CurrPort,u32CurrPort+1);
	}
	
    *pu32SendPort = u32CurrPort;
	(HI_VOID)pthread_mutex_unlock(&(g_stMtransAttr.MutexGetPort));
    printf("min %d ---max %d ,cur = %d v1\n",u32MinPort,u32MaxPort,u32CurrPort);
}

/*convert mediabuff's type to rtp payload type*/
HI_S32 MTRANSGetRtpPayloadType(MTRANS_TASK_S *pTaskHandle,MBUF_PT_E enMbuffPayloadType,RTP_PT_E* pPayloadType)
{
    MT_AUDIO_FORMAT_E enAudioFormat = MT_AUDIO_FORMAT_BUTT;   /*audio format*/
    MT_VIDEO_FORMAT_E enVideoFormat = MT_VIDEO_FORMAT_BUTT;   /*video format*/

    if( VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
    { 
        /* get currnet video format*/
        enVideoFormat = pTaskHandle->enVideoFormat;
        //printf(" ~~~~~~~~~~~~~~~~rtp packet: video format %d~~~~~~~~~\n",enVideoFormat);
        if (MT_VIDEO_FORMAT_H264 == enVideoFormat)
        {
            *pPayloadType = RTP_PT_H264;
        }
        else if (MT_VIDEO_FORMAT_MJPEG == enVideoFormat)
        {
            *pPayloadType = RTP_PT_JPEG;
        }
		else if (MT_VIDEO_FORMAT_H265 == enVideoFormat)
		{
			*pPayloadType = RTP_PT_H265;
		}	
        else
        {
        	
        	
            printf("the video formate is :%d \n",enVideoFormat);
            return HI_ERR_MTRANS_UNSUPPORT_PACKET_MEDIA_TYPE;
        }
    }
    else if (AUDIO_FRAME == enMbuffPayloadType)
    {
        /* get currnet audio format*/
        enAudioFormat = pTaskHandle->enAudioFormat;

        if(MT_AUDIO_FORMAT_G711A == enAudioFormat)
        {
            *pPayloadType = RTP_PT_ALAW;
        }
        else if (MT_AUDIO_FORMAT_G711Mu == enAudioFormat)
        {
            *pPayloadType = RTP_PT_ULAW;
        }
        else if (MT_AUDIO_FORMAT_G726 == enAudioFormat)
        {
            *pPayloadType = RTP_PT_G726;
        }
        else if (MT_AUDIO_FORMAT_AMR == enAudioFormat)
        {
            *pPayloadType = RTP_PT_ARM;
        }
        else if (MT_AUDIO_FORMAT_ADPCM == enAudioFormat)
        {
            *pPayloadType = RTP_PT_ADPCM;
        }
		else if (MT_AUDIO_FORMAT_AAC == enAudioFormat )
		{
			*pPayloadType = RTP_PT_AAC;
		}
        else
        {
            printf("the audio formate is :%d \n",enAudioFormat);
            return HI_ERR_MTRANS_UNSUPPORT_PACKET_MEDIA_TYPE;
        }
        
    }
    else if (MD_FRAME == enMbuffPayloadType)
    {
        *pPayloadType = RTP_PT_DATA;
    }
    else
    {
        return HI_ERR_MTRANS_UNSUPPORT_PACKET_MEDIA_TYPE;
    }

    return HI_SUCCESS;
}

/*根据打包类型，数据首地址和长度，计算打包开始首地址和包长度*/
HI_VOID MTRANSCalcPacketInfo(PACK_TYPE_E enStreamPackType,HI_U8*pu8DataAddr,HI_U32 u32DataLen,
                                       HI_U8 **ppu8PackAddr, HI_U32 *pu32PackLen)
{
    HI_U8* pu8PackAddr = NULL;
    HI_U32 u32PackLen = 0;
    if(PACK_TYPE_RTSP_ITLV == enStreamPackType)
    {
        u32PackLen = sizeof(RTSP_ITLEAVED_HDR_S) + u32DataLen;
        pu8PackAddr = pu8DataAddr - sizeof(RTSP_ITLEAVED_HDR_S);
        
    }
    else if(PACK_TYPE_HISI_ITLV == enStreamPackType)
    {
        u32PackLen = sizeof(HISI_ITLEAVED_HDR_S) + u32DataLen;
        pu8PackAddr = pu8DataAddr - sizeof(HISI_ITLEAVED_HDR_S);
    }
    else if (PACK_TYPE_RTP == enStreamPackType)
    {
        u32PackLen = sizeof(RTP_HDR_S) + u32DataLen;
        pu8PackAddr = pu8DataAddr - sizeof(RTP_HDR_S);
    }
    else if (PACK_TYPE_RTP_STAP == enStreamPackType)
    {
        u32PackLen = sizeof(RTP_HDR_S) + 3 + u32DataLen;
        /*stap 打包格式:rtp头+3字节+数据*/
        pu8PackAddr = pu8DataAddr - sizeof(RTP_HDR_S)-3;
    }

    *ppu8PackAddr = pu8PackAddr;
    *pu32PackLen = u32PackLen;
    
    //printf("pack type=%d data addr %X len %d, pack addr %X len %d\n",
    //enStreamPackType,pu8DataAddr,u32DataLen,*pu8PackAddr,*pu32PackLen);
        
}

HI_U32 MTRANS_GetPackPayloadLen()
{
    return g_stMtransAttr.u32PackPayloadLen;
}

/*发送接口，适用于tcp和udp方式
tcp时: s32WritSock为tcp socket，pPeerSockAddr为NULL
tcp时: s32WritSock为udp socket，pPeerSockAddr为对端接收sockaddr*/
HI_S32 HI_MTRANS_Send(HI_SOCKET s32WritSock,HI_CHAR* pcBuff,
                                      HI_U32 u32DataLen,struct sockaddr_in* pPeerSockAddr)
{
    HI_S32 s32Ret = 0;
    HI_U32 u32RemSize =0;
    HI_S32 s32Size    = 0;
    HI_CHAR*  pBufferPos = NULL;
    fd_set write_fds;
    struct timeval TimeoutVal;                      /* Timeout value */
    memset(&TimeoutVal,0,sizeof(struct timeval));
    HI_S32    s32Errno = 0;
    
    u32RemSize = u32DataLen;
    pBufferPos = pcBuff;

    while(u32RemSize > 0)
    {
        FD_ZERO(&write_fds);
        FD_SET(s32WritSock, &write_fds);
        TimeoutVal.tv_sec = 10;
        TimeoutVal.tv_usec = 0;
        /*judge if it can send */
        s32Ret = select(s32WritSock + 1, NULL, &write_fds, NULL, &TimeoutVal);
        if (s32Ret > 0)
        {
            if(FD_ISSET(s32WritSock, &write_fds))
            {
                if(pPeerSockAddr == NULL)
                {
                    s32Size = send(s32WritSock, pBufferPos, u32RemSize, 0);
                }
                else
                {
                	s32Size = sendto(s32WritSock, pBufferPos, u32RemSize, 0,
                                (struct sockaddr*)pPeerSockAddr,sizeof(struct sockaddr));
                }
                if (s32Size < 0)
                {
                    /*if it is not eagain error, means can not send*/
                    if (errno != EINTR && errno != EAGAIN)
                    {
                        s32Errno = errno;
                        HI_LOG_SYS("MTRANS",LOG_LEVEL_ERR,"send media data error 1: %s\n",strerror(s32Errno));
                        return HI_ERR_MTRANS_SEND;
                    }
                    /*it is eagain error, means can try again*/
                    continue;
                }
                u32RemSize -= s32Size;
                pBufferPos += s32Size;
            }
            else
            {
                s32Errno = errno;
                HI_LOG_SYS("MTRANS",LOG_LEVEL_ERR,"send media data error: write fd not in fd set\n");
                return HI_ERR_MTRANS_SEND;
            }
        }
        /*select found over time or error happend*/
        else if( 0 == s32Ret )
        {
            s32Errno = errno;
            HI_LOG_SYS("MTRANS",LOG_LEVEL_ERR,"send media data error: send over time 10s. s32WritSock is %d\n", s32WritSock);
            return HI_ERR_MTRANS_SEND;
        }
        else if( 0 > s32Ret  )
        {
            s32Errno = errno;
            HI_LOG_SYS("MTRANS",LOG_LEVEL_ERR,"send media data error 2: %s",strerror(s32Errno));
            return HI_ERR_MTRANS_SEND;
        }
    }

    return HI_SUCCESS;
}

HI_S32 MTRANS_TaskStartTcpItlv(MTRANS_TASK_S *pTaskHandle, HI_U8 enMediaType)
{
    HI_S32 i = 0;
    MTRANS_TASK_STATE_E enMtransState = TRANS_TASK_STATE_BUTT;

    for(i=0; i< MTRANS_STREAM_MAX; i++)
    {
        /*user creat video sending task, */
        if( IS_MTRANS_REQ_TYPE(enMediaType,i))
        {
            MTRANS_GetTaskState(pTaskHandle,i,&enMtransState);
            if(TRANS_TASK_STATE_READY == enMtransState)
            {
                /*set video trans state as playing*/
                MTRANS_SetTaskState(pTaskHandle,i,TRANS_TASK_STATE_PLAYING);
            }
            else if (TRANS_TASK_STATE_PLAYING == enMtransState)
            {
                /*video already in playing stat, do nothing */
            }
            else
            {
                DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"start video error: video is in init state.\n");
                return HI_ERR_MTRANS_NOT_ALLOWED_START_TASK;
            }
        }
    }

    return HI_SUCCESS;
}


HI_S32 MTRANS_TaskStartUdp(MTRANS_TASK_S *pTaskHandle, HI_U8 enMediaType)
{
	HI_S32 s32TempSock = 0;
    HI_S32 i = 0;
    MTRANS_UDPTRANS_INFO_S *pstUdpTransInfo = NULL;
    //MTRANS_TASK_STATE_E enOrigMediaState[MTRANS_STREAM_MAX] = {0}; 
    MTRANS_TASK_STATE_E enMediaState = TRANS_TASK_STATE_BUTT;

    /*在更新媒体状态前先保存原状态，以便出错时回滚状态*/
    /*to do :验证如此cpy是否正确*/
    //memcpy(enOrigMediaState,pTaskHandle->enTaskState,sizeof(MTRANS_TASK_STATE_E)*MTRANS_STREAM_MAX);
    
    pstUdpTransInfo = &(pTaskHandle->struUdpTransInfo);

    /*对请求发送的媒体,
     1)置其状态为playing 2)创建视频发送socket 3)根据peer 的ip，port，获取其接受sockaddr*/
    for(i=0; i< MTRANS_STREAM_MAX; i++)
    {
        if( IS_MTRANS_REQ_TYPE(enMediaType,i))
        {
            MTRANS_GetTaskState(pTaskHandle,i,&enMediaState);
            if(TRANS_TASK_STATE_READY == enMediaState)
            {
                if(pstUdpTransInfo->as32SendSock[i] < 0)
                {
                    /*创建视频发送socket*/
    				s32TempSock = MT_SOCKET_Udp_Open(pstUdpTransInfo->svrMediaSendPort[i]);
    				if(-1 == s32TempSock )
    				{
    					HI_LOG_SYS(MTRANS_SEND,LOG_LEVEL_ERR,"error:get socket from port%d.\n",
                       				pstUdpTransInfo->svrMediaSendPort[i]);
    					return HI_ERR_MTRANS_SOCKET_ERR;
    				}
    				pstUdpTransInfo->as32SendSock[i] = s32TempSock;

                    if(pstUdpTransInfo->cliAdaptDataPort[i] > 0)
                    {
        				s32TempSock = MT_SOCKET_Udp_Open(pstUdpTransInfo->svrMediaSendPort[i]+1);
        				if(-1 == s32TempSock )
        				{
        					HI_LOG_SYS(MTRANS_SEND,LOG_LEVEL_ERR,"error:get socket from port%d.\n",
                           				pstUdpTransInfo->svrMediaSendPort[i]+1);
        					return HI_ERR_MTRANS_SOCKET_ERR;
        				}
                        pstUdpTransInfo->as32RecvSock[i] = s32TempSock;
                    }
                }
                
				printf("MTRANS udp start:%d sendport %d,send sock %d,cli ip %s ---- rtcport:%d\n",i,pstUdpTransInfo->svrMediaSendPort[i],
									   pstUdpTransInfo->as32SendSock[i],pstUdpTransInfo->au8CliIP[i],pstUdpTransInfo->svrMediaSendPort[i]+1); 

                /*根据peer 的ip，port，获取其接受sockaddr*/
                /*to do:是否会出现获取错误*/
                printf("MTRANS udp start:cli ip %s ---- cli port:%d\n",pstUdpTransInfo->au8CliIP[i],pstUdpTransInfo->cliMediaPort[i]);
                MT_SOCKET_GetPeerSockAddr(pstUdpTransInfo->au8CliIP[i],pstUdpTransInfo->cliMediaPort[i],
                                      &(pstUdpTransInfo->struCliAddr[i]));
                                     
                /*set media trans state as ready*/
                MTRANS_SetTaskState(pTaskHandle,i,TRANS_TASK_STATE_PLAYING);
            }
            else if (TRANS_TASK_STATE_PLAYING == enMediaState)
            {
                /*media already in playing stat, do nothing */
            }
            else
            {
                DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,
                                   "start media error: media %d is in init state.\n",i);
                return HI_ERR_MTRANS_NOT_ALLOWED_START_TASK;
            }
        }
    }

#if 0
    /*如果未启动udp发送线程，则创建发送线程*/
    if(-1 == pstUdpTransInfo->s32SendThdId)
    {
        s32Ret = HI_PthreadMNG_Create("MTRANSUdpSendThread",&(pstUdpTransInfo->s32SendThdId),
                                       NULL, MTRANS_UdpSendProc, pTaskHandle);
        if(HI_SUCCESS != s32Ret)
        {
            pstUdpTransInfo->s32SendThdId = -1;
            /*各媒体类型的传输任务状态回滚*/
            memcpy(pTaskHandle->enTaskState,enOrigMediaState,sizeof(enOrigMediaState));
            return HI_ERR_MTRANS_CREATE_THREAD
        }
    }
#endif
    return HI_SUCCESS;
}

HI_S32 MTRANS_TaskStartMulticast(MTRANS_TASK_S *pTaskHandle, HI_U8 enMediaType)
{
	HI_S32 s32TempSock = 0;
    HI_S32 i = 0;
    MTRANS_UDPTRANS_INFO_S *pstUdpTransInfo = NULL;
    //MTRANS_TASK_STATE_E enOrigMediaState[MTRANS_STREAM_MAX] = {0}; 
    MTRANS_TASK_STATE_E enMediaState = TRANS_TASK_STATE_BUTT;

    /*在更新媒体状态前先保存原状态，以便出错时回滚状态*/
    /*to do :验证如此cpy是否正确*/
    
    pstUdpTransInfo = &(pTaskHandle->struUdpTransInfo);

    /*对请求发送的媒体,
     1)置其状态为playing 2)创建视频发送socket 3)根据peer 的ip，port，获取其接受sockaddr*/
    for(i=0; i< MTRANS_STREAM_MAX; i++)
    {
        if( IS_MTRANS_REQ_TYPE(enMediaType,i))
        {
            MTRANS_GetTaskState(pTaskHandle,i,&enMediaState);
            if(TRANS_TASK_STATE_READY == enMediaState)
            {
                /*创建视频发送socket*/
                s32TempSock = MT_SOCKET_Multicast_Open(pstUdpTransInfo->au8CliIP[i], pstUdpTransInfo->svrMediaSendPort[i]);
                if(-1 == s32TempSock )
                {
                    HI_LOG_SYS(MTRANS_SEND,LOG_LEVEL_ERR,"error:get socket from port%d.\n",
                        pstUdpTransInfo->svrMediaSendPort[i]);
                    return HI_ERR_MTRANS_SOCKET_ERR;
                }
                pstUdpTransInfo->as32SendSock[i] = s32TempSock;

                s32TempSock = MT_SOCKET_Multicast_Open(pstUdpTransInfo->au8CliIP[i], pstUdpTransInfo->svrMediaSendPort[i]+1);
                if(-1 == s32TempSock )
                {
                    HI_LOG_SYS(MTRANS_SEND,LOG_LEVEL_ERR,"error:get socket from port%d.\n",
                        pstUdpTransInfo->svrMediaSendPort[i]+1);
                    return HI_ERR_MTRANS_SOCKET_ERR;
                }
                pstUdpTransInfo->as32RecvSock[i] = s32TempSock;
                printf("MTRANS udp start:%d sendport %d,send sock %d,cli ip %s ---- rtcport:%d\n",i,pstUdpTransInfo->svrMediaSendPort[i],
                    pstUdpTransInfo->as32SendSock[i],pstUdpTransInfo->au8CliIP[i],pstUdpTransInfo->svrMediaSendPort[i]+1); 

                printf("MTRANS udp start:cli ip %s ---- cli port:%d\n",pstUdpTransInfo->au8CliIP[i],pstUdpTransInfo->cliMediaPort[i]);
                MT_SOCKET_GetPeerSockAddr(pstUdpTransInfo->au8CliIP[i],
                    pstUdpTransInfo->cliMediaPort[i],&(pstUdpTransInfo->struCliAddr[i]));
                                     
                /*set media trans state as ready*/
                MTRANS_SetTaskState(pTaskHandle,i,TRANS_TASK_STATE_PLAYING);
            }
            else if (TRANS_TASK_STATE_PLAYING == enMediaState)
            {
                /*media already in playing stat, do nothing */
            }
            else
            {
                DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,
                                   "start media error: media %d is in init state.\n",i);
                return HI_ERR_MTRANS_NOT_ALLOWED_START_TASK;
            }
        }
    }

#if 0
    /*如果未启动udp发送线程，则创建发送线程*/
    if(-1 == pstUdpTransInfo->s32SendThdId)
    {
        s32Ret = HI_PthreadMNG_Create("MTRANSUdpSendThread",&(pstUdpTransInfo->s32SendThdId),
                                       NULL, MTRANS_UdpSendProc, pTaskHandle);
        if(HI_SUCCESS != s32Ret)
        {
            pstUdpTransInfo->s32SendThdId = -1;
            /*各媒体类型的传输任务状态回滚*/
            memcpy(pTaskHandle->enTaskState,enOrigMediaState,sizeof(enOrigMediaState));
            return HI_ERR_MTRANS_CREATE_THREAD
        }
    }
#endif
    return HI_SUCCESS;
}

#define MAX_IFRAME_LEN 64*1024
#define MAX_LANGTAO_HEAD_LEN 128

HI_S32 data_flag = 0;

#if 0
/*init a appointed send task*/
HI_S32 HI_MTRANS_TaskInit(MTRANS_TASK_S *pTaskHandle,const MTRANS_SEND_CREATE_MSG_S *pstSendInfo)
{
    HI_S32 i = 0;
	
    /*record trans mode:tcp, udp or broadcast*/
    pTaskHandle->enTransMode = pstSendInfo->enMediaTransMode;

    /*record media data handle (mbuff handle */
    pTaskHandle->u32MbuffHandle = pstSendInfo->u32MediaHandle;

    /*record client ip address*/
    memcpy(pTaskHandle->au8CliIP,pstSendInfo->au8CliIP,sizeof(pstSendInfo->au8CliIP));

    pTaskHandle->pu8PackBuff = NULL;
    pTaskHandle->u32PackDataLen = 0;
    /*init all media's(video/audio/data) trans state as init */
    for(i=0; i<MTRANS_STREAM_MAX; i++)
    {
        pTaskHandle->enTaskState[i] = TRANS_TASK_STATE_INIT;
		pTaskHandle->au32Ssrc[i] = 0;
    }

    /*if it is tcp trans type, record corresponding info*/
    if(MTRANS_MODE_TCP_ITLV == pTaskHandle->enTransMode)
    {
        /*init data sequence number as 0*/
        memset(&(pTaskHandle->struTcpItlvTransInfo),0,sizeof(pTaskHandle->struTcpItlvTransInfo));
    }
    /*if it is udp trans type, record corresponding info*/
    else if(MTRANS_MODE_UDP == pTaskHandle->enTransMode )
    {
        memset(&(pTaskHandle->struUdpTransInfo),0,sizeof(pTaskHandle->struUdpTransInfo));
        
        /*初始时，udp发送线程号为-1,表示线程未创建*/
        pTaskHandle->struUdpTransInfo.s32SendThdId = -1;
		for(i = 0;i<MTRANS_STREAM_MAX; i++)
		{
			pTaskHandle->struUdpTransInfo.as32SendSock[i] = -1;
		}
    }
    /*if it is broadcast trans type, record corresponding info*/
    else if (MTRANS_MODE_BROADCAST == pTaskHandle->enTransMode)
    {
        /*to do :增加broadcast传输模式后,补充该部分*/
    }
     
    pTaskHandle->u32FrameIndex = 0;
    pTaskHandle->u32FramePacketIndex = 0;
    pTaskHandle->u32VideoPacketIndex = 0;
    return HI_SUCCESS;
}

#endif

HI_S32 HI_MTRANS_TaskInit(MTRANS_TASK_S *pTaskHandle,const MTRANS_SEND_CREATE_MSG_S *pstSendInfo)
{
    HI_S32 i = 0;
    HI_U8 u8TempMediaType = 0;

    u8TempMediaType = pstSendInfo->u8MeidaType;
    if(MTRANS_MODE_MULTICAST == pstSendInfo->enMediaTransMode)
    {
        pthread_mutex_lock(&pTaskHandle->struUdpTransInfo.multicastLock);
        if(pTaskHandle->struUdpTransInfo.InitFlag == 0)
        {
            pTaskHandle->u32MbuffHandle = 0;
        }
        
        if(pTaskHandle->struUdpTransInfo.InitFlag & u8TempMediaType)
        {
            pthread_mutex_unlock(&pTaskHandle->struUdpTransInfo.multicastLock);
            return HI_SUCCESS;
        }
        else
        {
            pTaskHandle->struUdpTransInfo.InitFlag |= u8TempMediaType;
            pthread_mutex_unlock(&pTaskHandle->struUdpTransInfo.multicastLock);
        }
    }
    pTaskHandle->u32FrameIndex = 0;
    pTaskHandle->u32FramePacketIndex = 0;
    /*record trans mode:tcp, udp or broadcast*/
    pTaskHandle->enTransMode = pstSendInfo->enMediaTransMode;

    /*record media data handle (mbuff handle */
    //pTaskHandle->u32MbuffHandle = pstSendInfo->u32MediaHandle;
    pTaskHandle->videoFrame = pstSendInfo->resv1;
    pTaskHandle->SPS_LEN = pstSendInfo->SPS_LEN;
    pTaskHandle->PPS_LEN = pstSendInfo->PPS_LEN;
    pTaskHandle->SEL_LEN = pstSendInfo->SEL_LEN;
	pTaskHandle->audioSampleRate = pstSendInfo->AudioSampleRate;
    /*record client ip address*/
    if(MTRANS_MODE_MULTICAST == pTaskHandle->enTransMode)
        memcpy(pTaskHandle->au8CliIP, "239.1.2.3",sizeof(pstSendInfo->au8CliIP));
    else
        memcpy(pTaskHandle->au8CliIP, pstSendInfo->au8CliIP,sizeof(pstSendInfo->au8CliIP));

    /*init all media's(video/audio/data) trans state as init */
    for(i=0; i<MTRANS_STREAM_MAX; i++)
    {
        //pTaskHandle->enTaskState[i] = TRANS_TASK_STATE_INIT;
    }

    pTaskHandle->u32PackDataLen = 0;
    /*user creat audio sending task, set audio trans state as ready*/
    if( IS_MTRANS_AUDIO_TYPE(u8TempMediaType))
    {
        /*set audio format*/
        pTaskHandle->enAudioFormat = pstSendInfo->enAudioFormat;
    }

    if(IS_MTRANS_VIDEO_TYPE(u8TempMediaType))
    {
        /*set video format*/
        pTaskHandle->enVideoFormat = pstSendInfo->enVideoFormat;
    }
    
    /*if it is tcp trans type, record corresponding info*/
    if(MTRANS_MODE_TCP_ITLV == pTaskHandle->enTransMode)
    {
        /*init data sequence number as 0*/
        pTaskHandle->struTcpItlvTransInfo.u32VSendSeqNum = 0;
        pTaskHandle->struTcpItlvTransInfo.u32ASendSeqNum = 0;
        /*set data sending socket same as session socket*/
        pTaskHandle->struTcpItlvTransInfo.s32TcpTransSock = 
                                              pstSendInfo->stTcpItlvTransInfo.s32SessSock;

        u8TempMediaType = pstSendInfo->u8MeidaType;
		printf("mtrans task init the media:%d\n",u8TempMediaType);
        /*user creat video sending task, */
        if( IS_MTRANS_VIDEO_TYPE(u8TempMediaType))
        {
            DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_NOTICE,
                            "mtrans init : user want to trans video, media type = %d\n ",u8TempMediaType);
            /*set video trans state as ready*/
            pTaskHandle->enTaskState[MTRANS_STREAM_VIDEO] = TRANS_TASK_STATE_READY;
            /*if user want to creat video, the video interleaved chn id is user appointed*/
            pTaskHandle->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]
                                                          = pstSendInfo->stTcpItlvTransInfo.u32ItlvCliMDataId;
                                                          
            pTaskHandle->struTcpItlvTransInfo.au32SvrITLMediaDataChnId[MTRANS_STREAM_VIDEO]
                                                          = MTRANS_TCPITLV_SVR_MEDIADATA_CHNID;
            /*set media pack type*/
            pTaskHandle->aenPackType[MTRANS_STREAM_VIDEO] = pstSendInfo->enPackType;
			
            /*set media pack ssrc*/
            pTaskHandle->au32Ssrc[MTRANS_STREAM_VIDEO] = pstSendInfo->u32Ssrc;
        }

        /*user creat audio sending task, set audio trans state as ready*/
        if( IS_MTRANS_AUDIO_TYPE(u8TempMediaType))
        {
            DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_NOTICE,
                            "mtrans init : user want to trans audio, media type = %d\n ",u8TempMediaType);
            pTaskHandle->enTaskState[MTRANS_STREAM_AUDIO] = TRANS_TASK_STATE_READY;
            pTaskHandle->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_AUDIO]
                                                          = pstSendInfo->stTcpItlvTransInfo.u32ItlvCliMDataId;

            pTaskHandle->struTcpItlvTransInfo.au32SvrITLMediaDataChnId[MTRANS_STREAM_AUDIO]
                                                          = MTRANS_TCPITLV_SVR_MEDIADATA_CHNID;
            /*set media pack type*/
            pTaskHandle->aenPackType[MTRANS_STREAM_AUDIO] = pstSendInfo->enPackType;
		//	printf("DEBUG %s %d audio ssrc=%08x\n",__FILE__,__LINE__,pstSendInfo->u32Ssrc);
			/*set media pack ssrc*/
            pTaskHandle->au32Ssrc[MTRANS_STREAM_AUDIO] = pstSendInfo->u32Ssrc;
        }

        /*user creat data sending task, set data trans state as ready*/
        if( IS_MTRANS_DATA_TYPE(u8TempMediaType))
        {
            DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_NOTICE,
                            "mtrans init : user want to trans data, media type = %d\n ",u8TempMediaType);
            pTaskHandle->enTaskState[MTRANS_STREAM_DATA] = TRANS_TASK_STATE_READY;
            pTaskHandle->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_DATA]
                                                          = pstSendInfo->stTcpItlvTransInfo.u32ItlvCliMDataId;
                                                          
            pTaskHandle->struTcpItlvTransInfo.au32SvrITLMediaDataChnId[MTRANS_STREAM_DATA]
                                                          = MTRANS_TCPITLV_SVR_MEDIADATA_CHNID;
            /*set media pack type*/
            pTaskHandle->aenPackType[MTRANS_STREAM_DATA] = pstSendInfo->enPackType;

            /*set media pack ssrc*/
            pTaskHandle->au32Ssrc[MTRANS_STREAM_DATA] = pstSendInfo->u32Ssrc;
        }
    }
    /*if it is udp trans type, record corresponding info*/
    else if(MTRANS_MODE_UDP == pTaskHandle->enTransMode 
        || MTRANS_MODE_MULTICAST == pTaskHandle->enTransMode)
    {   
        u8TempMediaType = pstSendInfo->u8MeidaType;
        printf("mtrans task init udp the media:%d\n",u8TempMediaType);
        /*to do :增加udp传输模式后,补充该部分*/
        for (i = 0; i< MTRANS_STREAM_MAX; i++)
        {
            
            if( IS_MTRANS_REQ_TYPE(u8TempMediaType,i))
            {
                pTaskHandle->struUdpTransInfo.as32SendSock[i] = -1;
                pTaskHandle->struUdpTransInfo.as32RecvSock[i] = -1;
                DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_NOTICE,
                                "mtrans set attr : user want to trans %d, media type = %d\n ",i,u8TempMediaType);
                /*set video trans state as ready*/
                pTaskHandle->enTaskState[i] = TRANS_TASK_STATE_READY;
                /*if user want to creat video, the video recv port is user appointed*/
                pTaskHandle->struUdpTransInfo.cliMediaPort[i]
                      = pstSendInfo->stUdpTransInfo.u32CliMDataPort;

                pTaskHandle->struUdpTransInfo.cliAdaptDataPort[i]
                      = pstSendInfo->stUdpTransInfo.u32CliAdaptDataPort;

                if(MTRANS_MODE_UDP == pTaskHandle->enTransMode)
                {
                    MTANS_GetMediaSendPort(&(pTaskHandle->struUdpTransInfo.svrMediaSendPort[i]));
                    strncpy(pTaskHandle->struUdpTransInfo.au8CliIP[i], pstSendInfo->au8CliIP,sizeof(HI_IP_ADDR));
                }
                else
                {
                    strcpy(pTaskHandle->struUdpTransInfo.au8CliIP[i], "239.1.2.3");
                    pTaskHandle->struUdpTransInfo.svrMediaSendPort[i] = pstSendInfo->stUdpTransInfo.u32CliMDataPort;  
                }
                
                /*set media pack type*/
                pTaskHandle->aenPackType[i] = pstSendInfo->enPackType;
                pTaskHandle->struUdpTransInfo.au32SeqNum[i] = 0;
				//if (1==i) printf("DEBUG %s %d audio ssrc=%08x\n",__FILE__,__LINE__,pstSendInfo->u32Ssrc);
                /*set media pack ssrc*/
                pTaskHandle->au32Ssrc[i] = pstSendInfo->u32Ssrc;

            }
        }
    }
    /*if it is broadcast trans type, record corresponding info*/
    else if (MTRANS_MODE_BROADCAST == pTaskHandle->enTransMode)
    {
        /*to do :增加broadcast传输模式后,补充该部分*/
    }
    pTaskHandle->liveMode = pstSendInfo->enLiveMode;
    return HI_SUCCESS;
}

HI_VOID HI_MTRANS_TaskSetAttr(MTRANS_TASK_S *pTaskHandle,const MTRANS_SEND_CREATE_MSG_S *pstSendInfo)
{
    HI_S32 i = 0;
    HI_U8 u8TempMediaType = 0;

    u8TempMediaType = pstSendInfo->u8MeidaType;
    /*user creat audio sending task, set audio trans state as ready*/
    if( IS_MTRANS_AUDIO_TYPE(u8TempMediaType))
    {
        /*set audio format*/
        pTaskHandle->enAudioFormat = pstSendInfo->enAudioFormat;
    }

    if(IS_MTRANS_VIDEO_TYPE(u8TempMediaType))
    {
        /*set video format*/
        pTaskHandle->enVideoFormat = pstSendInfo->enVideoFormat;
    }
    
    /*if it is tcp trans type, record corresponding info*/
    if(MTRANS_MODE_TCP_ITLV == pTaskHandle->enTransMode)
    {
        /*set data sending socket same as session socket*/
        pTaskHandle->struTcpItlvTransInfo.s32TcpTransSock = 
                                              pstSendInfo->stTcpItlvTransInfo.s32SessSock;

        for (i = 0; i< MTRANS_STREAM_MAX; i++)
        {
            /*user creat video sending task, */
            if( IS_MTRANS_REQ_TYPE(u8TempMediaType,i))
            {
                DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_NOTICE,
                                "mtrans init : user want to trans %d, media type = %d\n ",i,u8TempMediaType);
                /*set video trans state as ready*/
                pTaskHandle->enTaskState[i] = TRANS_TASK_STATE_READY;
                /*if user want to creat video, the video interleaved chn id is user appointed*/
                pTaskHandle->struTcpItlvTransInfo.au32CliITLMediaDataChnId[i]
                                                              = pstSendInfo->stTcpItlvTransInfo.u32ItlvCliMDataId;
                                                              
                pTaskHandle->struTcpItlvTransInfo.au32SvrITLMediaDataChnId[i]
                                                              = MTRANS_TCPITLV_SVR_MEDIADATA_CHNID;
                /*set media pack type*/
                pTaskHandle->aenPackType[i] = pstSendInfo->enPackType;

				 /*set media pack ssrc*/
                pTaskHandle->au32Ssrc[i] = pstSendInfo->u32Ssrc;

            }
        }
    }
    /*if it is udp trans type, record corresponding info*/
    else if(MTRANS_MODE_UDP == pTaskHandle->enTransMode )
    {   
        for (i = 0; i< MTRANS_STREAM_MAX; i++)
        {
            if( IS_MTRANS_REQ_TYPE(u8TempMediaType,i))
            {
                DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_NOTICE,
                                "mtrans set attr : user want to trans %d, media type = %d\n ",i,u8TempMediaType);
                /*set video trans state as ready*/
                pTaskHandle->enTaskState[i] = TRANS_TASK_STATE_READY;
                /*if user want to creat video, the video recv port is user appointed*/
                pTaskHandle->struUdpTransInfo.cliMediaPort[i]
                                                              = pstSendInfo->stUdpTransInfo.u32CliMDataPort;
                                                              
                MTANS_GetMediaSendPort(&(pTaskHandle->struUdpTransInfo.svrMediaSendPort[i]));
                
                /*set media pack type*/
                pTaskHandle->aenPackType[i] = pstSendInfo->enPackType;

				/*set media pack ssrc*/
                pTaskHandle->au32Ssrc[i] = pstSendInfo->u32Ssrc;

                strncpy(pTaskHandle->struUdpTransInfo.au8CliIP[i], pstSendInfo->au8CliIP,sizeof(HI_IP_ADDR));
            }
        }
                
    }
    /*if it is broadcast trans type, record corresponding info*/
    else if (MTRANS_MODE_BROADCAST == pTaskHandle->enTransMode)
    {
        /*to do :增加broadcast传输模式后,补充该部分*/
    }
    
}

HI_S32 HI_MTRANS_TaskStart(MTRANS_TASK_S *pTaskHandle, HI_U8 enMediaType)
{
    HI_S32 s32Ret = HI_SUCCESS;
    
    if(NULL == pTaskHandle)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }
    
    /*check the appointed task exit or not*/
    if(HI_TRUE != MTRANS_TaskCheck(pTaskHandle))
    {
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"start task with illegale task handel %p.\n",
                   pTaskHandle);
        return HI_ERR_MTRANS_ILLEGALE_TASK;
    }
    
    if((pTaskHandle->aenPackType[MTRANS_STREAM_VIDEO] == PACK_TYPE_LANGTAO)
        ||(pTaskHandle->aenPackType[MTRANS_STREAM_VIDEO] == PACK_TYPE_OWSP))
    {
        printf("<mtrans> the packlangtao malloc pack buffer \n");
        pTaskHandle->pu8PackBuff = malloc(MAX_IFRAME_LEN+MAX_LANGTAO_HEAD_LEN);
    }
    else
    {
        printf("<mtrans> the fua malloc pack buffer \n");
    	pTaskHandle->pu8PackBuff = malloc(MTRANS_GetPackPayloadLen()+128);
    	//printf("mtrans malloc for pack = %X len %d\n ",pTaskHandle->pu8PackBuff,MTRANS_GetPackPayloadLen()+128)	;
	    if(NULL == pTaskHandle->pu8PackBuff)
	    {
	        HI_LOG_SYS(MTRANS_SEND,LOG_LEVEL_ERR,"error:calloc pack buff for task handel %p.\n",
	                   pTaskHandle);
	        return HI_ERR_MTRANS_CALLOC_FAILED;
	    }
    }
    
    /*if it is tcp trans type */
    if(MTRANS_MODE_TCP_ITLV == pTaskHandle->enTransMode)
    {
        /*因为tcp潜入式发送的线程和会话线呈为一个线程，所以只需要
        配置相应媒体的状态为playing即可*/
        s32Ret = MTRANS_TaskStartTcpItlv(pTaskHandle,enMediaType);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    /*if it is udp trans type, record corresponding info*/
    else if(MTRANS_MODE_UDP == pTaskHandle->enTransMode )
    {   
        s32Ret = MTRANS_TaskStartUdp(pTaskHandle,enMediaType);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    else if(MTRANS_MODE_MULTICAST == pTaskHandle->enTransMode )
    {   
        s32Ret = MTRANS_TaskStartMulticast(pTaskHandle,enMediaType);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    /*if it is broadcast trans type, record corresponding info*/
    else if (MTRANS_MODE_BROADCAST == pTaskHandle->enTransMode)
    {
        /*to do :增加broadcast传输模式后,补充该部分*/
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"start trans task error: not support broadcast trans now.\n");
        return HI_ERR_MTRANS_UNRECOGNIZED_TRANS_MODE;
    }
    /*if it is tcp trans type, record corresponding info*/
    else if (MTRANS_MODE_TCP == pTaskHandle->enTransMode)
    {
        /*to do :增加tcp传输模式后,补充该部分*/
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"start trans task error: not support tcp trans now.\n");
        return HI_ERR_MTRANS_UNRECOGNIZED_TRANS_MODE;
    }
    /*if it is udp interleaved trans type, record corresponding info*/
    else if (MTRANS_MODE_UDP_ITLV == pTaskHandle->enTransMode)
    {
        /*to do :增加udp interleaved传输模式后,补充该部分*/
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"start trans task error: not support udp interleaved trans now.\n");
        return HI_ERR_MTRANS_UNRECOGNIZED_TRANS_MODE;
    }
    else
    {
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"start trans task error: unrecognized trans now.\n");
        return HI_ERR_MTRANS_UNRECOGNIZED_TRANS_MODE;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MTRANS_TaskStop(MTRANS_TASK_S *pTaskHandle, HI_U8 enMediaType)
{
    HI_S32 i = 0;
    MTRANS_TASK_STATE_E enMtransState = TRANS_TASK_STATE_BUTT;
    
    if(NULL == pTaskHandle)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }
    
    /*check the appointed task exit or not*/
    if(HI_TRUE != MTRANS_TaskCheck(pTaskHandle))
    {
   
        HI_LOG_SYS(MTRANS_SEND,LOG_LEVEL_ERR,"stop task with illegale task handel %p.\n",
                   pTaskHandle);
        return HI_ERR_MTRANS_ILLEGALE_TASK;
    }

    /*if it is tcp trans type*/
    if(MTRANS_MODE_TCP_ITLV == pTaskHandle->enTransMode)
    {
        for(i = 0; i< MTRANS_STREAM_MAX; i++)
        {
            /*user creat video sending task, */
            if( IS_MTRANS_REQ_TYPE(enMediaType,i))
            {
                MTRANS_GetTaskState(pTaskHandle,i,&enMtransState);
                if(TRANS_TASK_STATE_PLAYING == enMtransState)
                {
                    /*set media trans state as ready*/
                    MTRANS_SetTaskState(pTaskHandle,i,TRANS_TASK_STATE_READY);
                }
                else
                {
                    DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"stop media %d error: %d is not in playing state.\n",
                                        enMediaType,i);
                    return HI_ERR_MTRANS_NOT_ALLOWED_STOP_TASK;
                }
            }
        }
        
    }
    /*if it is udp trans type, record corresponding info*/
    else if(MTRANS_MODE_UDP == pTaskHandle->enTransMode )
    {   
        for(i = 0; i< MTRANS_STREAM_MAX; i++)
        {
            /*user creat video sending task, */
            if( IS_MTRANS_REQ_TYPE(enMediaType,i))
            {
                MTRANS_GetTaskState(pTaskHandle,i,&enMtransState);
                if(TRANS_TASK_STATE_PLAYING == enMtransState)
                {
                    /*set media trans state as ready*/
                    MTRANS_SetTaskState(pTaskHandle,i,TRANS_TASK_STATE_READY);
                }
                else
                {
                    DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"stop media %d error: %d is not in playing state.\n",
                                        enMediaType,i);
                    return HI_ERR_MTRANS_NOT_ALLOWED_STOP_TASK;
                }
            }
        }
    }
    /*if it is broadcast trans type, record corresponding info*/
    else if (MTRANS_MODE_BROADCAST == pTaskHandle->enTransMode)
    {
        /*to do :增加broadcast传输模式后,补充该部分*/
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"top trans task error: not support broadcast trans now.\n");
        return HI_ERR_MTRANS_UNRECOGNIZED_TRANS_MODE;
    }
    else
    {
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERR,"top trans task error: not unrecognized trans type %d.\n",
                  pTaskHandle->enTransMode);
        return HI_ERR_MTRANS_UNRECOGNIZED_TRANS_MODE;
    }
    return HI_SUCCESS;
}

/*get server's transition response info to client */
HI_S32 HI_MTRANS_GetSvrTransInfo(MTRANS_TASK_S *pTaskHandle,HI_U8 u8MediaType,
                                MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo)
{
    HI_S32 i = 0;
    MTRANS_MODE_E enTempTransMode = MTRANS_MODE_BUTT;
    
    if(HI_NULL == pTaskHandle ||HI_NULL ==  pstReSendInfo)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }

    /*check the appointed task exit or not*/
    if(HI_TRUE != MTRANS_TaskCheck(pTaskHandle))
    {
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_ERROR,"get svr tans info with illegale task handel %p.\n",
                   pTaskHandle);
        return HI_ERR_MTRANS_ILLEGALE_TASK;
    }

    pstReSendInfo->s32ErrCode = HI_SUCCESS;

    pstReSendInfo->u32TransTaskHandle =(HI_U32) pTaskHandle;

    enTempTransMode = pTaskHandle->enTransMode;
    /*set reSendInfo: trans mode */
    pstReSendInfo->enMediaTransMode = enTempTransMode;

    /*if it is tcp interleaved trans type, return svr corresponding info*/
    if(MTRANS_MODE_TCP_ITLV == enTempTransMode)
    {
        for(i = 0; i<MTRANS_STREAM_MAX; i++)
        {
            /*user creat video sending task, */
            if( IS_MTRANS_REQ_TYPE(u8MediaType,i))
            {
                if(TRANS_TASK_STATE_INIT != pTaskHandle->enTaskState[i])
                {
                    /*if user want to creat video, the video interleaved chn id is user appointed*/
                    pstReSendInfo->stTcpItlvTransInfo.u32SvrMediaDataId = 
                                        pTaskHandle->struTcpItlvTransInfo.au32SvrITLMediaDataChnId[i];
                }
            }
        }
        
        pstReSendInfo->stTcpItlvTransInfo.pProcMediaTrans = OnProcItlvTrans;
        //printf("mtrans func : OnProcItlvTrans = %X \n",OnProcItlvTrans);
    }
    /*if it is udp trans type, record corresponding info*/
    else if(MTRANS_MODE_UDP == enTempTransMode )
    {   
        for(i = 0; i<MTRANS_STREAM_MAX; i++)
        {
            /*user creat video sending task, */
            if( IS_MTRANS_REQ_TYPE(u8MediaType,i))
            {
                if(TRANS_TASK_STATE_INIT != pTaskHandle->enTaskState[i])
                {
                    /*if user want to creat video, the video interleaved chn id is user appointed*/
                    pstReSendInfo->stUdpTransInfo.u32SvrMediaSendPort = 
                                        pTaskHandle->struUdpTransInfo.svrMediaSendPort[i];
                }
            }
        }
        
        pstReSendInfo->stUdpTransInfo.pProcMediaTrans = OnProcItlvTrans;
    }
    /*if it is broadcast trans type, record corresponding info*/
    else if (MTRANS_MODE_BROADCAST == enTempTransMode)
    {
        /*to do :增加broadcast传输模式后,补充该部分*/
    }
    else if(MTRANS_MODE_MULTICAST == enTempTransMode )
    {   
        for(i = 0; i<MTRANS_STREAM_MAX; i++)
        {
            /*user creat video sending task, */
            if( IS_MTRANS_REQ_TYPE(u8MediaType,i))
            {
                if(TRANS_TASK_STATE_INIT != pTaskHandle->enTaskState[i])
                {
                    /*if user want to creat video, the video interleaved chn id is user appointed*/
                    pstReSendInfo->stUdpTransInfo.u32SvrMediaSendPort = 
                        pTaskHandle->struUdpTransInfo.svrMediaSendPort[i];
                    pstReSendInfo->stMulticastTransInfo.svrMediaPort = 
                        pTaskHandle->struUdpTransInfo.svrMediaSendPort[i];
                    pstReSendInfo->stMulticastTransInfo.cliMediaPort = 
                        pTaskHandle->struUdpTransInfo.cliMediaPort[i];

                    if(pTaskHandle->u32MbuffHandle)
                    {
                        pstReSendInfo->stMulticastTransInfo.u32MediaHandle = pTaskHandle->u32MbuffHandle;
                    }
                    else
                    {
                        pstReSendInfo->stMulticastTransInfo.u32MediaHandle = 0;
                    }
                }
            }
        }
        
        pstReSendInfo->stUdpTransInfo.pProcMediaTrans = (PROC_MEIDA_TRANS)multicast_send_thread;
    }

    return HI_SUCCESS;
}

HI_S32 MTRANS_UpdateSeqNum(MTRANS_TASK_S *pTaskHandle,MBUF_PT_E enMbuffPayloadType,
                                        MTRANS_MODE_E enTransMode,HI_U32 u32SeqNum)
{
    if(MTRANS_MODE_TCP_ITLV == enTransMode)
    {
        if(VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
        {
        	pTaskHandle->struTcpItlvTransInfo.u32VSendSeqNum = u32SeqNum;
        }
        else if(AUDIO_FRAME == enMbuffPayloadType)
        {
            pTaskHandle->struTcpItlvTransInfo.u32ASendSeqNum = u32SeqNum;
        }
    }
    else //该分支一定为udp发送模式
    {
        if(VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
        {
            pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_VIDEO] = u32SeqNum;
        }
        else if(AUDIO_FRAME == enMbuffPayloadType)
        {
            pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_AUDIO]= u32SeqNum;
        }
        else if(MD_FRAME == enMbuffPayloadType)
        {
            pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_DATA]= u32SeqNum;
        }
        else
        {
            HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"udp mtrans task %p for cli %s withdraw"\
                        "for invalid data type %d from mbuff \n ",
                        pTaskHandle,pTaskHandle->au8CliIP,enMbuffPayloadType);
            return HI_ERR_MTRANS_SEND;
        }
    }
    return HI_SUCCESS;
}

HI_S32 MTRANS_GetPacketAndSendParam(MTRANS_TASK_S *pTaskHandle,MBUF_PT_E enMbuffPayloadType,
            HI_SOCKET* pWriteSock,struct sockaddr_in *pPeerSockAddr,HI_U32 *pu32LastSn,PACK_TYPE_E *penPackType,
            RTP_PT_E *penRtpPayloadType,HI_U32 *pu32Ssrc,HI_U32 *pu32FrameIndex
            ,HI_U32 *pu32FramePacketIndex,HI_U32 *pu32VideoPacketIndex)
{
    HI_S32 s32Ret = 0;
  //  HI_SOCKET s32WriteSock = -1;
  //  HI_BOOL   bDiscardFlag = HI_FALSE;
   // HI_U32   u32LastSn = 0;
    MTRANS_MODE_E      enTransMode = MTRANS_MODE_BUTT;
   // PACK_TYPE_E enTempPackType = PACK_TYPE_BUTT;
    enTransMode = pTaskHandle->enTransMode;
    *pu32FrameIndex = pTaskHandle->u32FrameIndex;
    *pu32FramePacketIndex = pTaskHandle->u32FramePacketIndex;
    *pu32VideoPacketIndex =pTaskHandle->u32VideoPacketIndex;
    /*1 get packet type*/
    /*if stream waiting packet is video and vidieo is in playing state,
      stream can be packeted*/
     if( VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
     {  
        /*packet the stream according to different packet type */
        *penPackType = pTaskHandle->aenPackType[MTRANS_STREAM_VIDEO];
		*pu32Ssrc = pTaskHandle->au32Ssrc[MTRANS_STREAM_VIDEO];
     }

     /*if stream waiting packet is Audio and Audio is in playing state
       it stream can be packeted*/
     else if( AUDIO_FRAME == enMbuffPayloadType)
     {  
        /*packet the stream according to different packet type */
        *penPackType = pTaskHandle->aenPackType[MTRANS_STREAM_AUDIO];
		*pu32Ssrc = pTaskHandle->au32Ssrc[MTRANS_STREAM_AUDIO];
	//	printf("DEBUG %s %d audio ssrc=%08x \n",__FILE__,__LINE__,pTaskHandle->au32Ssrc[MTRANS_STREAM_AUDIO]);
     }

     /*if stream waiting packet is data and data is in playing state, 
       stream can be packeted*/
     else if( MD_FRAME == enMbuffPayloadType)
     {  
        /*packet the stream according to different packet type */
        *penPackType = pTaskHandle->aenPackType[MTRANS_STREAM_DATA];
		*pu32Ssrc = pTaskHandle->au32Ssrc[MTRANS_STREAM_DATA];
     }
     else
     {
        return HI_ERR_MTRANS_UNSUPPORT_PACKET_TYPE;
     }
    if(MTRANS_MODE_TCP_ITLV == enTransMode)
    {
        *pWriteSock = pTaskHandle->struTcpItlvTransInfo.s32TcpTransSock;
        pPeerSockAddr = NULL;
        if(VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
        {
        	*pu32LastSn = pTaskHandle->struTcpItlvTransInfo.u32VSendSeqNum;
        }
        else if(AUDIO_FRAME == enMbuffPayloadType)
        {
            *pu32LastSn = pTaskHandle->struTcpItlvTransInfo.u32ASendSeqNum;
        }
    }
    else //该分支一定为udp发送模式
    {
        if(VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
        {
            *pWriteSock = pTaskHandle->struUdpTransInfo.as32SendSock[MTRANS_STREAM_VIDEO];
            memcpy(pPeerSockAddr,&(pTaskHandle->struUdpTransInfo.struCliAddr[MTRANS_STREAM_VIDEO]),
                    sizeof(struct sockaddr_in));
            *pu32LastSn = pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_VIDEO];
        }
        else if(AUDIO_FRAME == enMbuffPayloadType)
        {
            *pWriteSock = pTaskHandle->struUdpTransInfo.as32SendSock[MTRANS_STREAM_AUDIO];
            memcpy(pPeerSockAddr,&(pTaskHandle->struUdpTransInfo.struCliAddr[MTRANS_STREAM_AUDIO]),
                sizeof(struct sockaddr_in));
            *pu32LastSn = pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_AUDIO];
        }
        else if(MD_FRAME == enMbuffPayloadType)
        {
            *pWriteSock = pTaskHandle->struUdpTransInfo.as32SendSock[MTRANS_STREAM_DATA];
            memcpy(pPeerSockAddr, &(pTaskHandle->struUdpTransInfo.struCliAddr[MTRANS_STREAM_DATA]),
                sizeof(struct sockaddr_in));
            *pu32LastSn = pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_DATA];
        }
        else
        {
            HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"udp mtrans task %p for cli %s withdraw"\
                        "for invalid data type %d from mbuff \n ",
                        pTaskHandle,pTaskHandle->au8CliIP,enMbuffPayloadType);
            return HI_ERR_MTRANS_SEND;
        }
    }
     /*get rtp payloadtype according to stream's type (mbuff modle naming the stream type)*/
    s32Ret = MTRANSGetRtpPayloadType(pTaskHandle,enMbuffPayloadType,penRtpPayloadType);
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }
    
    return HI_SUCCESS;
}



HI_S32 MTRANS_FindFrame(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,HI_U32 u32DataLen,HI_U32 *u32OneFrame,HI_U32 *u32FrameLen)
{
	HI_U8 u8NALType;
//	printf("pData :%x %x %x %x %x %x\n",*(pDataAddr),*(pDataAddr+1),*(pDataAddr+2),*(pDataAddr+3),*(pDataAddr+4), *(pDataAddr+5));
	u8NALType = *(pDataAddr + 4) & 0x1F; 	//pMessage + 4 是NAL单元的字节头
	if(*(pDataAddr) == 0 && *(pDataAddr+1) == 0 && *(pDataAddr+2) == 0 && *(pDataAddr+3) == 1)
	{
		if(0x7 == u8NALType)
		{
			*u32OneFrame = 1;
			*u32FrameLen = pTask->SPS_LEN+4;
		}
		else if(0x8 == u8NALType) //
		{
			*u32OneFrame = 1;
			*u32FrameLen = pTask->PPS_LEN+4;
		}
		else if(0x6 == u8NALType) //SEI
		{
			//*u32OneFrame = 1;
			//*u32FrameLen = pTask->SEL_LEN+4;
			int i;
			for(i=4;i<(u32DataLen-4);i++)
			{
				if(*(pDataAddr+i) == 0 && *(pDataAddr+i+1) == 0 && *(pDataAddr+i+2) == 0 && *(pDataAddr+i+3) == 1)
				{
					*u32OneFrame = 1;
					*u32FrameLen = i;
					break;
				}
			}
		}		
		else
		{
			*u32OneFrame = 1;
			*u32FrameLen = u32DataLen;	
		}
	}
	else
	{
		*u32OneFrame = 0;
		*u32FrameLen = 0;
		return HI_FAILURE;
	}
	return HI_SUCCESS;
}


HI_S32 MTRANS_FindFrame_TCP(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,HI_U32 u32DataLen,HI_U32 *u32OneFrame,HI_U32 *u32FrameLen)
{
	HI_U8 u8NALType;
	u8NALType = *(pDataAddr + 4) & 0x1F; 	//pMessage + 4 是NAL单元的字节头
	//printf("pData :%x %x %x %x %x %x SEL_LEN:%d  u32DataLen: %d  u8NALType: %x\n",*(pDataAddr),*(pDataAddr+1),*(pDataAddr+2),*(pDataAddr+3),*(pDataAddr+4), *(pDataAddr+5), pTask->SEL_LEN, u32DataLen, u8NALType);
	if(*(pDataAddr) == 0 && *(pDataAddr+1) == 0 && *(pDataAddr+2) == 0 && *(pDataAddr+3) == 1)
	{
		if(0x7 == u8NALType)
		{
			*u32OneFrame = 1;
			*u32FrameLen = pTask->SPS_LEN+4;
			
		}
		else if(0x8 == u8NALType)
		{
			*u32OneFrame = 1;
			*u32FrameLen = pTask->PPS_LEN+4;
		}
		/* 兼容TI项目,动态获取SEI 长度  */
		else if(0x6 == u8NALType) //SEI
		{
			//*u32FrameLen = pTask->SEL_LEN+4;
			int i;
			for(i=4;i<(u32DataLen-4);i++)
			{
				if(*(pDataAddr+i) == 0 && *(pDataAddr+i+1) == 0 && *(pDataAddr+i+2) == 0 && *(pDataAddr+i+3) == 1)
				{
					*u32OneFrame = 1;
					*u32FrameLen = i;
					break;
				}
			}
		}	
		/*
		else if(0x6 == u8NALType) //SEI
		{
			*u32OneFrame = 1;
			*u32FrameLen = pTask->SEL_LEN+4;
		}
		*/

		else if(0x1c == u8NALType)
		{
		    printf("%s-%d:u8NALType = 0x%02x!\n", __FUNCTION__,__LINE__,u8NALType);
			*u32OneFrame = 1;
			*u32FrameLen = pTask->SPS_LEN+4;
		}
		else
		{
			*u32OneFrame = 1;
			*u32FrameLen = u32DataLen;
		}
	}
	else
	{
		*u32OneFrame = 0;
		*u32FrameLen = 0;
		return HI_FAILURE;
	}
	return HI_SUCCESS;
}
#define JPEG_DPT_NUM 2
HI_S32 MTRANS_SendJpegDataInRtpFUA_TCP(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,
	 	    HI_U32 u32DataLen,HI_U32 u32TimeStamp, RTP_PT_E PackageType,HI_U32 *pu32SeqNum,
	 	    HI_U32 u32Ssrc,HI_SOCKET WriteSock,struct sockaddr_in *pPeerSockAddr )
{
    unsigned char jpeg_dqt[JPEG_DPT_NUM][64] = {{0}};
    unsigned char jpeg_dqt_max = 0;
    unsigned char *pJpegData = pDataAddr;
	unsigned int JpegDataLen = u32DataLen;
    unsigned short xLen = 0;
    int jpeg_dri  = 0;
    unsigned short jpeg_width = 0,jpeg_height = 0;
    int bytes_left = JpegDataLen;
    HI_U32 u32PackPayloadLen = MTRANS_GetPackPayloadLen();
    HI_CHAR *pPackBuf = pTask->pu8PackBuff; /*打包buff地址*/
    HI_CHAR *ptr = 0;
    HI_U8  u8Marker = 0;
    HI_U32 u32SeqNum = *pu32SeqNum;
    HI_S32 s32Ret = 0;
    HI_U32 u32RTPSendLen = 0;
    int data_len = 0;
    unsigned int jpeg_q = 255;
    struct jpeghdr jpghdr;
    struct jpeghdr_rst rsthdr;
    struct jpeghdr_qtable qtblhdr;

    JpegDataLen -= 2;//去掉结尾的两个字节
    while(pJpegData[0] == 0xFF)
    {
        if(0xD8 == pJpegData[1])//图像开始(start of image)
        {
            pJpegData   += 2;
            JpegDataLen -= 2;
        }
        else if(0xE0 == pJpegData[1])//APP0 (Application)
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xDB == pJpegData[1])//DQT(Define Quantization Table)量化表定义段
        {
            unsigned char dri_index = 0;
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            
            jpeg_dqt_max = xLen/64;
            pJpegData   += 4;
            JpegDataLen -= 4;
            for(dri_index = 0; dri_index < jpeg_dqt_max; dri_index++)
            {
                unsigned char nn = pJpegData[0];
                if(nn < JPEG_DPT_NUM)
                {
                    memcpy(jpeg_dqt[nn], pJpegData+1, 64);
                }
                pJpegData   += (1+64);
                JpegDataLen -= (1+64);
            }
        }
        else if(0xDD == pJpegData[1])//DRI(Define Restart Interval)
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);

            jpeg_dri = (int)pJpegData[4];
            jpeg_dri = (jpeg_dri << 8) | (pJpegData[5]);

            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xC0 == pJpegData[1])//SOF0(帧开始start of frame)
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            //获取分辨率-高
            jpeg_height = (unsigned short)pJpegData[5];
            jpeg_height = (jpeg_height << 8) | (pJpegData[6]);
            //获取分辨率-宽
            jpeg_width = (unsigned short)pJpegData[7];
            jpeg_width = (jpeg_width << 8) | (pJpegData[8]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xC4 == pJpegData[1])//DHT(Define Huffman Table)Huffman表定义段
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xDA == pJpegData[1])//SOS(start of Scan)扫描参数段-后面紧跟头就是真正编码数据的长度
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
            break;
        }
        else//重新查找0xFF标记-未实现
        {
        }
    }

    memset(&jpghdr, 0, sizeof(jpghdr));
    jpghdr.tspec = 0;
    jpghdr.off = 0;
    jpghdr.type = 1 | ((jpeg_dri != 0) ? RTP_JPEG_RESTART : 0);
    jpghdr.q = jpeg_q;
    jpghdr.width = jpeg_width / 8;
    jpghdr.height = jpeg_height / 8;

    if (jpeg_dri != 0) 
    {
        rsthdr.dri = htons(jpeg_dri);
        rsthdr.f = 1;       
        rsthdr.l = 1;
        rsthdr.count = htons(0x3fff);
    }

    if (jpeg_q >= 128)
    {
        qtblhdr.mbz = 0;
        qtblhdr.precision = 0;
        qtblhdr.length = htons(128);
    }
    bytes_left = JpegDataLen;
    u8Marker = 0;
    while (bytes_left > 0)
    {
        u32RTPSendLen = sizeof( RTSP_ITLEAVED_HDR_S );
        ptr = pPackBuf + sizeof( RTSP_ITLEAVED_HDR_S );
        memcpy(ptr, &jpghdr, sizeof(jpghdr));
        ptr[1] = jpghdr.off >> 16;
        ptr[2] = jpghdr.off >> 8;
        ptr[3] = jpghdr.off;

        ptr += sizeof(jpghdr);

        u32RTPSendLen += sizeof(jpghdr);

        if (jpeg_dri != 0)
        {
            memcpy(ptr, &rsthdr, sizeof(rsthdr));
            ptr += sizeof(rsthdr);
            u32RTPSendLen += sizeof(rsthdr);
        }

        if (jpeg_q >= 128 && jpghdr.off == 0)
        {
            unsigned char inde = 0;
            memcpy(ptr, &qtblhdr, sizeof(qtblhdr));
            ptr += sizeof(qtblhdr);
            u32RTPSendLen += sizeof(qtblhdr);
            for(inde = 0; inde < JPEG_DPT_NUM; inde++)
            {
                memcpy(ptr, jpeg_dqt[inde], 64);
                ptr += 64;
                u32RTPSendLen += 64;
            }
        }

        data_len = u32PackPayloadLen-(int)(ptr-pPackBuf);
        if (data_len >= bytes_left)
        {
            data_len = bytes_left;
            u8Marker = 1;
        }
        u32RTPSendLen += data_len;

        //memcpy(ptr, pJpegData + jpghdr.off, data_len);

        /*打rtsp+rtp头*/
        HI_RTSP_ITLV_Packet(pPackBuf, u32RTPSendLen, u32TimeStamp, u8Marker, PackageType, u32Ssrc, u32SeqNum++,
        	pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]);
        s32Ret = HI_MTRANS_Send(WriteSock,pPackBuf, u32RTPSendLen-data_len,pPeerSockAddr);
        if(HI_SUCCESS != s32Ret)
		{
			printf("[%s-%d].RTP video send error\n",__FUNCTION__,__LINE__); 
			return HI_FAILURE;
		}
        s32Ret = HI_MTRANS_Send(WriteSock,(char *)(pJpegData+jpghdr.off), data_len,pPeerSockAddr);
        if(HI_SUCCESS != s32Ret)
		{
			printf("[%s-%d].RTP video send error\n",__FUNCTION__,__LINE__); 
			return HI_FAILURE;
		}

        jpghdr.off += data_len;
        bytes_left -= data_len;
    }

    *pu32SeqNum = u32SeqNum;
    return HI_SUCCESS;
    return HI_SUCCESS;
}
HI_S32 MTRANS_SendJpegDataInRtpFUA(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,
	 	    HI_U32 u32DataLen,HI_U32 u32TimeStamp, RTP_PT_E PackageType,HI_U32 *pu32SeqNum,
	 	    HI_U32 u32Ssrc,HI_SOCKET WriteSock,struct sockaddr_in *pPeerSockAddr )
{
    unsigned char jpeg_dqt[JPEG_DPT_NUM][64] = {{0}};
    unsigned char jpeg_dqt_max = 0;
    unsigned char *pJpegData = pDataAddr;
	unsigned int JpegDataLen = u32DataLen;
    unsigned short xLen = 0;
    int jpeg_dri  = 0;
    unsigned short jpeg_width = 0,jpeg_height = 0;
    int bytes_left = JpegDataLen;
    HI_U32 u32PackPayloadLen = MTRANS_GetPackPayloadLen();
    HI_CHAR *pPackBuf = pTask->pu8PackBuff; /*打包buff地址*/
    HI_CHAR *ptr = 0;
    HI_U8  u8Marker = 0;
    HI_U32 u32SeqNum = *pu32SeqNum;
    HI_S32 s32Ret = 0;
    int data_len = 0;
    unsigned int jpeg_q = 255;
    struct jpeghdr jpghdr;
    struct jpeghdr_rst rsthdr;
    struct jpeghdr_qtable qtblhdr;

    JpegDataLen -= 2;//去掉结尾的两个字节
    while(pJpegData[0] == 0xFF)
    {
        if(0xD8 == pJpegData[1])//图像开始(start of image)
        {
            pJpegData   += 2;
            JpegDataLen -= 2;
        }
        else if(0xE0 == pJpegData[1])//APP0 (Application)
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xDB == pJpegData[1])//DQT(Define Quantization Table)量化表定义段
        {
            unsigned char dri_index = 0;
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            
            jpeg_dqt_max = xLen/64;
            pJpegData   += 4;
            JpegDataLen -= 4;
            for(dri_index = 0; dri_index < jpeg_dqt_max; dri_index++)
            {
                unsigned char nn = pJpegData[0];
                if(nn < JPEG_DPT_NUM)
                {
                    memcpy(jpeg_dqt[nn], pJpegData+1, 64);
                }
                pJpegData   += (1+64);
                JpegDataLen -= (1+64);
            }
        }
        else if(0xDD == pJpegData[1])//DRI(Define Restart Interval)
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);

            jpeg_dri = (int)pJpegData[4];
            jpeg_dri = (jpeg_dri << 8) | (pJpegData[5]);

            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xC0 == pJpegData[1])//SOF0(帧开始start of frame)
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            //获取分辨率-高
            jpeg_height = (unsigned short)pJpegData[5];
            jpeg_height = (jpeg_height << 8) | (pJpegData[6]);
            //获取分辨率-宽
            jpeg_width = (unsigned short)pJpegData[7];
            jpeg_width = (jpeg_width << 8) | (pJpegData[8]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xC4 == pJpegData[1])//DHT(Define Huffman Table)Huffman表定义段
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
        }
        else if(0xDA == pJpegData[1])//SOS(start of Scan)扫描参数段-后面紧跟头就是真正编码数据的长度
        {
            xLen = (unsigned short)pJpegData[2];
            xLen = (xLen << 8) | (pJpegData[3]);
            pJpegData   += (xLen+2);
            JpegDataLen -= (xLen+2);
            break;
        }
        else//重新查找0xFF标记-未实现
        {
        }
    }

    memset(&jpghdr, 0, sizeof(jpghdr));
    jpghdr.tspec = 0;
    jpghdr.off = 0;
    jpghdr.type = 1 | ((jpeg_dri != 0) ? RTP_JPEG_RESTART : 0);
    jpghdr.q = jpeg_q;
    jpghdr.width = jpeg_width / 8;
    jpghdr.height = jpeg_height / 8;

    if (jpeg_dri != 0) 
    {
        rsthdr.dri = htons(jpeg_dri);
        rsthdr.f = 1;       
        rsthdr.l = 1;
        rsthdr.count = htons(0x3fff);
    }

    if (jpeg_q >= 128)
    {
        qtblhdr.mbz = 0;
        qtblhdr.precision = 0;
        qtblhdr.length = htons(128);
    }
    bytes_left = JpegDataLen;
    u8Marker = 0;
    while (bytes_left > 0)
    {
        ptr = pPackBuf + sizeof( RTP_HDR_S );
        memcpy(ptr, &jpghdr, sizeof(jpghdr));
        ptr[1] = jpghdr.off >> 16;
        ptr[2] = jpghdr.off >> 8;
        ptr[3] = jpghdr.off;

        ptr += sizeof(jpghdr);

        if (jpeg_dri != 0)
        {
            memcpy(ptr, &rsthdr, sizeof(rsthdr));
            ptr += sizeof(rsthdr);
        }

        if (jpeg_q >= 128 && jpghdr.off == 0)
        {
            unsigned char inde = 0;
            memcpy(ptr, &qtblhdr, sizeof(qtblhdr));
            ptr += sizeof(qtblhdr);
            for(inde = 0; inde < JPEG_DPT_NUM; inde++)
            {
                memcpy(ptr, jpeg_dqt[inde], 64);
                ptr += 64;
            }
        }

        data_len = u32PackPayloadLen-(int)(ptr-pPackBuf);
        if (data_len >= bytes_left)
        {
            data_len = bytes_left;
            u8Marker = 1;
        }

        memcpy(ptr, pJpegData + jpghdr.off, data_len);

        /*打rtp头*/
        HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, PackageType, u32Ssrc,u32SeqNum++);
        s32Ret = HI_MTRANS_Send(WriteSock,pPackBuf, (ptr-pPackBuf)+data_len,pPeerSockAddr);
        if(HI_SUCCESS != s32Ret)
		{
			printf("[%s-%d].RTP video send error\n",__FUNCTION__,__LINE__); 
			return HI_FAILURE;
		}

        jpghdr.off += data_len;
        bytes_left -= data_len;
    }

    *pu32SeqNum = u32SeqNum;
    return HI_SUCCESS;
}
typedef struct 
{
    //byte 0
	unsigned char TYPE:5;
    unsigned char NRI:2;
	unsigned char F:1;    
} NALU_HEADER; /**//* 1 BYTES */



HI_S32 MTRANS_SendH265DataInRtpFUA_TCP(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,
				HI_U32 u32DataLen,HI_U32 u32TimeStamp, RTP_PT_E PackageType,HI_U32 *pu32SeqNum,
				HI_U32 u32Ssrc,HI_SOCKET WriteSock,struct sockaddr_in *pPeerSockAddr )

{
		//HI_CHAR* pPackDataAddr = NULL ;
		HI_CHAR *pPackBuf = NULL;
		HI_U32 u32PackPayloadLen = 0;
		HI_U32 u32SeqNum = 0;
		HI_U8  u8Marker = 0;
		HI_S32 s32Ret = 0;
		HI_U32 u32RTPSendLen = 0;
		HI_U32 datalen=u32DataLen;
		HI_U32 RTP_HEVC_HEADERS_SIZE=3;
		u32PackPayloadLen = MTRANS_GetPackPayloadLen();
		pPackBuf = pTask->pu8PackBuff; /*打包buff地址*/
		//pPackDataAddr = pDataAddr ; /*slice数据首地址*/
		u32SeqNum = *pu32SeqNum;
		
		HI_S32 rtp_payload_size=u32PackPayloadLen-sizeof(RTP_HDR_S)-RTP_HEVC_HEADERS_SIZE;
		rtp_payload_size -= 4;  //tcp header
	
		HI_CHAR * r, * r1 , * start;
		int nalsize;
		
		r = pDataAddr;
		do { 
			r1 = find_startcode ( r, &datalen);
	
			if (r1!=NULL) {
				nalsize = r1 -r;
				
			}
			else {
				nalsize = datalen;
			}
			start = r + 4 ;  // 去掉 0 0 0 1
			nalsize = nalsize -4;
			r = r1;
	
	
			if (nalsize< rtp_payload_size) {
				
				u32RTPSendLen = nalsize + sizeof (RTP_HDR_S);
				u32RTPSendLen += 4;
				
				HI_RTSP_ITLV_Packet(pPackBuf, u32RTPSendLen, u32TimeStamp, u8Marker, 96, u32Ssrc, u32SeqNum++,
					pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]);
				//HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, 96, u32Ssrc,u32SeqNum++);
				memcpy(pPackBuf+sizeof(RTP_HDR_S)+4,start,nalsize);

				s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen,pPeerSockAddr);
				if(HI_SUCCESS != s32Ret)
				{
					printf("%s,%d RTP video send error\n",__FILE__,__LINE__); 
					return s32Ret;
				}
				
				
			}
			else {
				HI_CHAR nal_type = (start[0] >> 1) & 0x3F; 
				HI_CHAR * h265Longdata = NULL;
				h265Longdata = pPackBuf + sizeof( RTP_HDR_S )+4;
				h265Longdata[0]=49<<1;
				h265Longdata[1]=1;
				h265Longdata[2] = nal_type; 
				h265Longdata[2] |= 1<<7;
				//此处要注意，当是分组的第一报数据时，应该覆盖掉前两个字节的数据，h264要覆盖前一个字节的数据，即是第一包要去除hevc帧数据的paylaodhdr 
				start += 2;
				nalsize -=2;
	
				while(nalsize > rtp_payload_size) {
					memcpy(h265Longdata+RTP_HEVC_HEADERS_SIZE,start,rtp_payload_size);
					//HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, 96, u32Ssrc,u32SeqNum++);
					HI_RTSP_ITLV_Packet(pPackBuf, u32PackPayloadLen, u32TimeStamp, u8Marker, 96, u32Ssrc, u32SeqNum++,
						pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]);
					s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32PackPayloadLen,pPeerSockAddr);
					if(HI_SUCCESS != s32Ret) {
						printf("%s,%d RTP video send error\n",__FILE__,__LINE__); 
						return s32Ret;
					}
					start += rtp_payload_size;
					nalsize -= rtp_payload_size;
					
					h265Longdata[2] &=~(1<<7);
					
				}
				
				h265Longdata[2] |= 1<<6 ;
				u8Marker=1;
				memcpy(h265Longdata+RTP_HEVC_HEADERS_SIZE, start, nalsize);  
				//HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, 96, u32Ssrc,u32SeqNum++);
				HI_RTSP_ITLV_Packet(pPackBuf, nalsize+sizeof(RTP_HDR_S)+RTP_HEVC_HEADERS_SIZE+4, u32TimeStamp, u8Marker, 96, u32Ssrc, u32SeqNum++,
					pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]);
				s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,nalsize+sizeof(RTP_HDR_S)+RTP_HEVC_HEADERS_SIZE+4,pPeerSockAddr);
				if(HI_SUCCESS != s32Ret) {
						printf("%s,%d RTP video send error\n",__FILE__,__LINE__); 
						return s32Ret;
				}
			}
				
			
		} while(r!=NULL);
		*pu32SeqNum = u32SeqNum;
			
		
		return HI_SUCCESS;

	
}



#define TCP_NODELAY 0x0001
//peter add for rtp over tcp,2011.11.4


HI_S32 MTRANS_SendDataInRtpFUA_TCP(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,
	 	    HI_U32 u32DataLen,HI_U32 u32TimeStamp, RTP_PT_E PackageType,HI_U32 *pu32SeqNum,
	 	    HI_U32 u32Ssrc,HI_SOCKET WriteSock,struct sockaddr_in *pPeerSockAddr )
{
	HI_S32 s32Ret = 0;
	HI_U32 u32PackageLen =0;
	HI_U32 u32RTPSendLen = 0;
	HI_U8  u8Marker = 0;
	HI_CHAR* pPackDataAddr = NULL ;
	HI_S32 s32Sendlen = 0;
	HI_CHAR *pPackBuf = NULL;
	//const HI_U32	u32HisiHeadLen = 4;
	//const HI_U32       u32FUHeadLen = 2;
	HI_U32 u32SeqNum = 0;
	HI_U32 u32PackPayloadLen = 0;
//	int tmp = 1;

    //setsockopt(WriteSock, IPPROTO_TCP, TCP_NODELAY, (int)&tmp, sizeof(int));

	u32PackPayloadLen = MTRANS_GetPackPayloadLen();
	pPackBuf = pTask->pu8PackBuff; /*打包buff地址*/
	u32SeqNum = *pu32SeqNum;
	pPackDataAddr = pDataAddr ;   /*slice数据首地址*/
#if 1	
	if (RTP_PT_AAC == PackageType)  //AAC audio
	{
		//printf("datalen=%d \n",u32DataLen);
		
		u8Marker = 1;
		s32Sendlen =  u32DataLen -7;  //跳过 ADTS 头，7个字节
		u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S ) ;

		/*cp数据*/

		pPackDataAddr +=7;  //跳过 ADTS 头，7个字节
		
		//memcpy(pPackBuf+4+ sizeof( RTP_HDR_S ),pPackDataAddr,s32Sendlen);

		/*打rtsp + rtp + FU-A 头*/
		//memcpy(pPackBuf+4, pPackBuf, s32Sendlen+sizeof(RTP_HDR_S)+2);
		u32RTPSendLen += 4;  //
		u32RTPSendLen += 4; //aachead
		//
		//printf(" ITLV u32RTPSendLen =%d ,s32Sendlen=%d PackageType=%d,u32SeqNum=%d\n",u32RTPSendLen,s32Sendlen,PackageType,u32SeqNum);
		HI_RTSP_ITLV_Packet(pPackBuf, u32RTPSendLen, u32TimeStamp, 1, PackageType, u32Ssrc, u32SeqNum++,
			pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_AUDIO]);
		//HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, PackageType, u32Ssrc,u32SeqNum++);
		s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen-s32Sendlen -4,pPeerSockAddr);   
		//printf(" ret= %d, u32DataLen:%d, u32PackPayloadLen = %d, pPackDataAddr:0x%x,  u32PackageLen:%d, s32Sendlen:%d, u32RTPSendLen:%d\n",s32Ret, u32DataLen, u32PackPayloadLen, pPackDataAddr, u32PackageLen, s32Sendlen, u32RTPSendLen);
		if(HI_SUCCESS != s32Ret)
		{
			printf("1.RTP video send error\n"); 
			return s32Ret;
		}
		char acchead[4]={0};
		acchead[0]= 0x0;
		acchead[1]= 0x10;
		acchead[2]= ((u32DataLen - 7)  & 0x1fe0) >> 5;  
		acchead[3]= ((u32DataLen - 7) & 0x1f) << 3;   
		s32Ret = HI_MTRANS_Send(WriteSock, acchead,4,pPeerSockAddr);
		s32Ret = HI_MTRANS_Send(WriteSock, pPackDataAddr,s32Sendlen,pPeerSockAddr);
		//printf("s32ret=%d s32sendlen=%d\n",s32Ret,s32Sendlen);
		if(HI_SUCCESS != s32Ret)
		{
			printf("2.RTP video send error\n"); 
			return s32Ret;
		}

		

		
		*pu32SeqNum = u32SeqNum; /*在内部发送了多个包,将序号返回*/

		return 0;		
	}  
	
#endif

	if((PackageType == 8) || (PackageType == 4) ) //audio 
	{
		//printf("----> u32DataLen:%d packagelen=%d\n", u32DataLen,u32PackageLen);
		//u32PackPayloadLen = 80;
		//又换回320了
		u32PackPayloadLen = 960;
		/*数据分为多个包发送*/ 
		//while(u32PackageLen < u32DataLen)
		{
			if(u32PackageLen  >= u32DataLen - u32PackPayloadLen - 4)
			{
				u8Marker = 1;
				s32Sendlen = u32PackPayloadLen;//u32DataLen - u32PackageLen;
			}
			else
			{
				s32Sendlen =  u32PackPayloadLen;
				u8Marker = 0;
			}
            //u32Ssrc += 1000;  //why +1000?
			u8Marker = 1;
			s32Sendlen =  u32PackPayloadLen;
			u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S );

			/*cp数据*/
			u32PackageLen += 4;
			pPackDataAddr += 4;
			//memcpy(pPackBuf+4+ sizeof( RTP_HDR_S ),pPackDataAddr,s32Sendlen);

			/*打rtsp + rtp + FU-A 头*/
			//memcpy(pPackBuf+4, pPackBuf, s32Sendlen+sizeof(RTP_HDR_S)+2);
			u32RTPSendLen += 4;

			//
			
			HI_RTSP_ITLV_Packet(pPackBuf, u32RTPSendLen, u32TimeStamp, 1, PackageType, u32Ssrc, u32SeqNum++,
				pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_AUDIO]);
			//HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, PackageType, u32Ssrc,u32SeqNum++);
			s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen-s32Sendlen,pPeerSockAddr);
			//printf(" u32DataLen:%d, u32PackPayloadLen = %d, pPackDataAddr:0x%x,  u32PackageLen:%d, s32Sendlen:%d, u32RTPSendLen:%d\n", u32DataLen, u32PackPayloadLen, pPackDataAddr, u32PackageLen, s32Sendlen, u32RTPSendLen);
			if(HI_SUCCESS != s32Ret)
			{
				printf("1.RTP video send error\n"); 
				return s32Ret;
			}
            s32Ret = HI_MTRANS_Send(WriteSock, pPackDataAddr,s32Sendlen,pPeerSockAddr);
			if(HI_SUCCESS != s32Ret)
			{
				printf("2.RTP video send error\n"); 
				return s32Ret;
			}

			pPackDataAddr +=  s32Sendlen; /*更新指向数据的指针*/
			u32PackageLen += s32Sendlen ;  /*更新已发送数据长度*/

		}

		*pu32SeqNum = u32SeqNum; /*在内部发送了多个包,将序号返回*/

		return 0;
	}

	HI_U32 u32FrameLen = 0;
	HI_U32 u32OneFrame = 0; 
	HI_S32 s32FindFramelen = 0;
    NALU_HEADER *pNaluHeader = NULL;
    HI_U8 u8NALType;
    int iframe = 0;
    
    //u32PackPayloadLen = 1460;
    if(26 == PackageType)//发送MJPEG
    {
        return MTRANS_SendJpegDataInRtpFUA_TCP(pTask, pDataAddr,
	 	    u32DataLen,u32TimeStamp, PackageType,pu32SeqNum,
	 	    u32Ssrc,WriteSock,pPeerSockAddr);
    }
	if (RTP_PT_H265== PackageType)
	{
		
		return MTRANS_SendH265DataInRtpFUA_TCP(pTask, pDataAddr,
	 	    u32DataLen,u32TimeStamp, PackageType,pu32SeqNum,
	 	    u32Ssrc,WriteSock,pPeerSockAddr);
	}

    //以下代码是处理发送h264数据的。
	while(s32FindFramelen < u32DataLen)
	{
	    u32OneFrame = 0;
        MTRANS_FindFrame_TCP(pTask, pDataAddr+s32FindFramelen,u32DataLen-s32FindFramelen,&u32OneFrame,&u32FrameLen);
        if(u32OneFrame != 1)
        {
            printf("%s(%d) this package is not have OneFrame\n", __FUNCTION__, __LINE__);
            return HI_FAILURE;
        }

        if(u32FrameLen<=5)
		{
			printf("%s(%d) MTRANS_FindFrame error!u32FrameLen=%d\n", __FUNCTION__, __LINE__, u32FrameLen);
			return HI_FAILURE;
		}
        
        pPackDataAddr = pDataAddr+s32FindFramelen;
        u8NALType = *(pPackDataAddr + 4) & 0x1F;
        if(u8NALType == 7 || u8NALType == 8 || u8NALType == 6)
            iframe = 1;
        else
            iframe = 0;
        
        if(u32FrameLen-5 < u32PackPayloadLen-17)//单nalu单包
        {
            u8Marker = 1;
/*			if(u32FrameLen == (u32DataLen - s32FindFramelen))
				u8Marker = 1;
			else
				u8Marker = 0;*/
			//s32Sendlen = u32FrameLen; //topsee
            s32Sendlen = u32FrameLen-5;
            
            u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S )+1;
            /*打rtsp + rtp */
			u32RTPSendLen += 4;
			HI_RTSP_ITLV_Packet(pPackBuf, u32RTPSendLen, u32TimeStamp, u8Marker, PackageType, u32Ssrc, u32SeqNum++,
				pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]);
            pNaluHeader = (NALU_HEADER*)(pPackBuf+16);
            pNaluHeader->F = (*(pPackDataAddr+4)) & 0x80;
            pNaluHeader->NRI = ((*(pPackDataAddr+4)) & 0x60) >> 5;
            pNaluHeader->TYPE = (*(pPackDataAddr+4)) & 0x1f;
			/*发送*/
			s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen-s32Sendlen,pPeerSockAddr);
			if(HI_SUCCESS != s32Ret)
			{
				printf("3.RTP video send error\n"); 
				return s32Ret;
			}
            s32Ret = HI_MTRANS_Send(WriteSock, pPackDataAddr+5,s32Sendlen,pPeerSockAddr);
			if(HI_SUCCESS != s32Ret)
			{
				printf("4.RTP video send error\n"); 
				return s32Ret;
			}
        }
        else//单个nalu多包
        {
        	
            int realpacketcount = (u32FrameLen-5) / (u32PackPayloadLen-18);
            int lastpacketlen = (u32FrameLen-5) % (u32PackPayloadLen-18);
            int j = 0;
            HI_U8 *pNalu = (HI_U8 *)pPackDataAddr;
            pPackDataAddr += 5;
            for(j = 0; j <= realpacketcount; j++)
            {
                u8Marker = 0;
                if(0 == j)//第一包
                {
                    s32Sendlen = (u32PackPayloadLen-18);
                    s32Ret = HI_RTP_FU_PackageHeader(pPackBuf+4+ sizeof( RTP_HDR_S ) ,pNalu,1,0);
                }
                else if(j == realpacketcount)//最后一包
                {
                    u8Marker = 1;
                    s32Sendlen = lastpacketlen;
                    s32Ret = HI_RTP_FU_PackageHeader(pPackBuf+4+ sizeof( RTP_HDR_S ) ,pNalu,0,1);
                }
                else//中间包
                {
                    s32Sendlen = (u32PackPayloadLen-18);
                    s32Ret = HI_RTP_FU_PackageHeader(pPackBuf+4+ sizeof( RTP_HDR_S ) ,pNalu,0,0);
                }
                u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S ) + 2 + 4;
                HI_RTSP_ITLV_Packet(pPackBuf, u32RTPSendLen, u32TimeStamp, u8Marker, PackageType, u32Ssrc, u32SeqNum++,
					pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]);
                s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen-s32Sendlen,pPeerSockAddr);
				if(HI_SUCCESS != s32Ret)
				{
					printf("6.RTP video send error\n"); 
					return s32Ret;
				}
                s32Ret = HI_MTRANS_Send(WriteSock, pPackDataAddr,s32Sendlen,pPeerSockAddr);
				if(HI_SUCCESS != s32Ret)
				{
					printf("7.RTP video send error\n"); 
					return s32Ret;
				}

				pPackDataAddr +=  s32Sendlen; /*更新指向数据的指针*/
            }
        }
		s32FindFramelen += u32FrameLen;
	}
	*pu32SeqNum = u32SeqNum; /*在内部发送了多个包,将序号返回*/
    if(iframe)
        return MTRANS_SEND_IFRAME;
    
	return HI_SUCCESS;
}

//FILE * debugfile=NULL;

 HI_CHAR * find_startcode( HI_CHAR * pDataAddr,unsigned int * DataLen)
{
    int i;
	HI_CHAR * Pos = NULL;
	for(i=4;i<( *DataLen-4);i++)
	{
		if(*(pDataAddr+i) == 0 && *(pDataAddr+i+1) == 0 && *(pDataAddr+i+2) == 0 && *(pDataAddr+i+3) == 1)
		{
			Pos = pDataAddr +i;
			*DataLen = *DataLen - i;
			break;
			
		}
	}
	return Pos;
	
}


HI_S32 MTRANS_SendH265DataInRtpFUA(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,
				HI_U32 u32DataLen,HI_U32 u32TimeStamp, RTP_PT_E PackageType,HI_U32 *pu32SeqNum,
				HI_U32 u32Ssrc,HI_SOCKET WriteSock,struct sockaddr_in *pPeerSockAddr )
{
	//HI_CHAR* pPackDataAddr = NULL ;
	HI_CHAR *pPackBuf = NULL;
	HI_U32 u32PackPayloadLen = 0;
	HI_U32 u32SeqNum = 0;
	HI_U8  u8Marker = 0;
//	HI_S32 s32Ret = 0;
	HI_U32 u32RTPSendLen = 0;
	HI_U32 datalen=u32DataLen;
	HI_U32 RTP_HEVC_HEADERS_SIZE=3;
	u32PackPayloadLen = MTRANS_GetPackPayloadLen();
	pPackBuf = pTask->pu8PackBuff; /*打包buff地址*/
	//pPackDataAddr = pDataAddr ; /*slice数据首地址*/
	u32SeqNum = *pu32SeqNum;
	
	HI_S32 rtp_payload_size=u32PackPayloadLen-sizeof(RTP_HDR_S)-RTP_HEVC_HEADERS_SIZE;
	

	HI_CHAR * r, * r1 , * start;
	int nalsize;
	
	r = pDataAddr;
	do { 
		r1 = find_startcode ( r, &datalen);

		if (r1!=NULL) {
			nalsize = r1 -r;
			
		}
		else {
			nalsize = datalen;
		}
		start = r + 4 ;  // 去掉 0 0 0 1
		nalsize = nalsize -4;
		r = r1;


		if (nalsize< rtp_payload_size) {
			//u8Marker = 1;
			u32RTPSendLen = nalsize + sizeof (RTP_HDR_S);
			HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, 96, u32Ssrc,u32SeqNum++);
			memcpy(pPackBuf+sizeof(RTP_HDR_S),start,nalsize);
			HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen,pPeerSockAddr);
		}
		else {
			HI_CHAR nal_type = (start[0] >> 1) & 0x3F; 
			HI_CHAR * h265Longdata = NULL;
			h265Longdata = pPackBuf + sizeof( RTP_HDR_S );
			h265Longdata[0]=49<<1;
			h265Longdata[1]=1;
			h265Longdata[2] = nal_type; 
			h265Longdata[2] |= 1<<7;
			//此处要注意，当是分组的第一报数据时，应该覆盖掉前两个字节的数据，h264要覆盖前一个字节的数据，即是第一包要去除hevc帧数据的paylaodhdr 
			start += 2;
			nalsize -=2;

			while(nalsize > rtp_payload_size) {
				memcpy(h265Longdata+RTP_HEVC_HEADERS_SIZE,start,rtp_payload_size);
				HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, 96, u32Ssrc,u32SeqNum++);
				HI_MTRANS_Send(WriteSock, pPackBuf,u32PackPayloadLen,pPeerSockAddr);

				start += rtp_payload_size;
				nalsize -= rtp_payload_size;
				
				h265Longdata[2] &=~(1<<7);
				
			}
			
			h265Longdata[2] |= 1<<6 ;
			u8Marker=1;
			memcpy(h265Longdata+RTP_HEVC_HEADERS_SIZE, start, nalsize);  
			HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, 96, u32Ssrc,u32SeqNum++);
			HI_MTRANS_Send(WriteSock, pPackBuf,nalsize+sizeof(RTP_HDR_S)+RTP_HEVC_HEADERS_SIZE,pPeerSockAddr);
		}
			
		
	} while(r!=NULL);
	*pu32SeqNum = u32SeqNum;
		
	
	return HI_SUCCESS;
}

//FILE *audiofp = NULL;
//int s32Index = 0;
HI_S32 MTRANS_SendDataInRtpFUA(MTRANS_TASK_S* pTask, HI_CHAR* pDataAddr,
	 	    HI_U32 u32DataLen,HI_U32 u32TimeStamp, RTP_PT_E PackageType,HI_U32 *pu32SeqNum,
	 	    HI_U32 u32Ssrc,HI_SOCKET WriteSock,struct sockaddr_in *pPeerSockAddr )
{
	HI_S32 s32Ret = 0;
	HI_U32 u32PackageLen =0;
	HI_U32 u32RTPSendLen = 0;
	HI_U8  u8Marker = 0;
	HI_CHAR* pPackDataAddr = NULL ;
	HI_S32 s32Sendlen = 0;
	HI_CHAR *pPackBuf = NULL;
//	const HI_U32	u32HisiHeadLen = 4;
//	const HI_U32       u32FUHeadLen = 2;
	HI_U32 u32SeqNum = 0;
	//printf(" u32DataLen is %d\n", u32DataLen);
	HI_U32 u32PackPayloadLen = 0;

	u32PackPayloadLen = MTRANS_GetPackPayloadLen();
	pPackBuf = pTask->pu8PackBuff; /*打包buff地址*/
	u32SeqNum = *pu32SeqNum;
	pPackDataAddr = pDataAddr ;   /*slice数据首地址*/
//	printf("%s(%d) u32DataLen:%d, u32TimeStamp:%d, PackageType:%d pData[%X-%X-%X-%X]\n", __FUNCTION__, __LINE__, 
//		u32DataLen, u32TimeStamp, PackageType, pDataAddr[0], pDataAddr[1], pDataAddr[2], pDataAddr[3]);

#if 1	
		if (RTP_PT_AAC == PackageType)	//AAC audio
		{
			//printf("datalen=%d udp\n",u32DataLen);
			
			u8Marker = 1;
			s32Sendlen =  u32DataLen -7;  //跳过 ADTS 头，7个字节
			u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S ) ;


			pPackDataAddr +=7;	//跳过 ADTS 头，7个字节
			


			u32RTPSendLen += 4; //aachead
			//
			char acchead[4]={0};
			acchead[0]= 0x0;
			acchead[1]= 0x10;
			acchead[2]= ((u32DataLen - 7)  & 0x1fe0) >> 5;	
			acchead[3]= ((u32DataLen - 7) & 0x1f) << 3;
			memcpy(pPackBuf + sizeof( RTP_HDR_S),acchead,4);
			memcpy(pPackBuf+ sizeof( RTP_HDR_S )+4,pPackDataAddr,s32Sendlen);
			
			HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, PackageType, u32Ssrc,u32SeqNum++);
			//HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, PackageType, u32Ssrc,u32SeqNum++);
			s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen,pPeerSockAddr);   
			//printf(" ret= %d, u32DataLen:%d, u32PackPayloadLen = %d, pPackDataAddr:0x%x,	u32PackageLen:%d, s32Sendlen:%d, u32RTPSendLen:%d\n",s32Ret, u32DataLen, u32PackPayloadLen, pPackDataAddr, u32PackageLen, s32Sendlen, u32RTPSendLen);
			if(HI_SUCCESS != s32Ret)
			{
				printf("1.RTP audio send error\n"); 
				return s32Ret;
			}
			
			
			*pu32SeqNum = u32SeqNum; /*在内部发送了多个包,将序号返回*/
	
			return 0;		
		}  
#endif

	if((PackageType == 8) || (PackageType == 4)) //audio 
	{
		//printf("----> u32DataLen:%d\n", u32DataLen);
		u32PackPayloadLen = 960;
		/*数据分为多个包发送*/ 
//		while(u32PackageLen < u32DataLen)
		{
			if(u32PackageLen  >= u32DataLen - u32PackPayloadLen - 4)
			{
				u8Marker = 1;
				s32Sendlen = u32PackPayloadLen;//u32DataLen - u32PackageLen;//
			}
			else
			{
				s32Sendlen =  u32PackPayloadLen;
				u8Marker = 0;
			}
           // u32Ssrc += 1000; why ?
			u8Marker = 1;
			s32Sendlen =  u32PackPayloadLen;
			u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S );

			/*cp数据*/
			//u32PackageLen += 4;
			//pPackDataAddr += 4;
#if 0
            if(NULL == audiofp && s32Index < 200)
            {
                audiofp = fopen("/tmp/audio", "w+");
                printf("#### %s %d, Close fp\n", __FUNCTION__, __LINE__);
            }
    		if(NULL != audiofp && s32Index < 200)
    		{
    			fwrite(pPackDataAddr, 1, s32Sendlen, audiofp);
                s32Index++;
                printf("#### %s %d, s32Sendlen=%d\n", __FUNCTION__, __LINE__, s32Sendlen);
    		}
       
    		if(s32Index == 200)
    		{
    			if(NULL != audiofp)
                {
    				fclose(audiofp);
    				audiofp = NULL;
    				printf("#### %s %d, Close fp\n", __FUNCTION__, __LINE__);
    			}
    		}
#endif

			memcpy(pPackBuf+ sizeof( RTP_HDR_S ),pPackDataAddr,s32Sendlen);
			//printf("DEBUG %s %d ssrc=%08x, s32Sendlen=%d, s32Index=%d\n",__FILE__,__LINE__,u32Ssrc, s32Sendlen, s32Index);
			/*打rtp头*/
			HI_RTP_Packet( pPackBuf, u32TimeStamp, u8Marker, PackageType, u32Ssrc,u32SeqNum++);
			//printf("DEBUG %s %d ssrc=%08x  htonl=%08x\n",__FILE__,__LINE__,(unsigned int)(pPackBuf+12-4),htonl((unsigned int)(pPackBuf+12-4)));
			s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen,pPeerSockAddr);
			//printf(" u32DataLen:%d, u32PackPayloadLen = %d, pPackDataAddr:0x%x,  u32PackageLen:%d, s32Sendlen:%d, u32RTPSendLen:%d\n", u32DataLen, u32PackPayloadLen, pPackDataAddr, u32PackageLen, s32Sendlen, u32RTPSendLen);
			if(HI_SUCCESS != s32Ret)
			{
				HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"RTP video send error\n"); 
				return s32Ret;
			}

			pPackDataAddr +=  s32Sendlen; /*更新指向数据的指针*/
			u32PackageLen += s32Sendlen ;  /*更新已发送数据长度*/
		}

		*pu32SeqNum = u32SeqNum; /*在内部发送了多个包,将序号返回*/

		return 0;
	}


	HI_U32 u32FrameLen = 0;
	HI_U32 u32OneFrame = 0; 
	HI_S32 s32FindFramelen = 0;
    NALU_HEADER *pNaluHeader = NULL;
    HI_U8 u8NALType;
    int iframe = 0;
    //u32PackPayloadLen = 1448;

    if(26 == PackageType)
    {
        return MTRANS_SendJpegDataInRtpFUA(pTask, pDataAddr,
	 	    u32DataLen,u32TimeStamp, PackageType,pu32SeqNum,
	 	    u32Ssrc,WriteSock,pPeerSockAddr);
    }
	if (RTP_PT_H265== PackageType)
	{
		//printf("%s %d into h265 FUA \n",__FILE__,__LINE__);
		return MTRANS_SendH265DataInRtpFUA(pTask, pDataAddr,
	 	    u32DataLen,u32TimeStamp, PackageType,pu32SeqNum,
	 	    u32Ssrc,WriteSock,pPeerSockAddr);
	}
	//把一个I帧拆分 找到PPS、SPS发送出去
	while(s32FindFramelen < u32DataLen)
	{
		MTRANS_FindFrame(pTask, pDataAddr+s32FindFramelen,u32DataLen-s32FindFramelen,&u32OneFrame,&u32FrameLen);
		if(u32OneFrame != 1)
		{
			printf("%s(%d) this package is not have OneFrame=%d\n", __FUNCTION__, __LINE__, u32OneFrame);
			return HI_FAILURE;
		}

		if(u32FrameLen<=5)
		{
			printf("%s(%d) MTRANS_FindFrame error!u32FrameLen=%d\n", __FUNCTION__, __LINE__, u32FrameLen);
			return HI_FAILURE;
		}
		
		pPackDataAddr = pDataAddr+s32FindFramelen;
        u8NALType = *(pPackDataAddr + 4) & 0x1F;
        if(u8NALType == 7 || u8NALType == 8 || u8NALType == 6)
            iframe = 1;
        else
            iframe = 0;

        if(u32FrameLen-5 < u32PackPayloadLen-13)//单nalu单包
        {
            u8Marker = 1;
/*			if(u32FrameLen == (u32DataLen - s32FindFramelen))
				u8Marker = 1;
			else
				u8Marker = 0;
*/
            s32Sendlen = u32FrameLen-5;
            u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S )+1;

            memcpy(pPackBuf+ sizeof( RTP_HDR_S )+1,pPackDataAddr+5,s32Sendlen);
            /*打rtsp + rtp */
            pNaluHeader = (NALU_HEADER*)(pPackBuf+12);
            pNaluHeader->F = (*(pPackDataAddr+4)) & 0x80;
            pNaluHeader->NRI = ((*(pPackDataAddr+4)) & 0x60) >> 5;
            pNaluHeader->TYPE = (*(pPackDataAddr+4)) & 0x1f;
            HI_RTP_Packet(pPackBuf,u32TimeStamp, u8Marker, PackageType, u32Ssrc, u32SeqNum++);
            /*发送*/
            
            s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen,pPeerSockAddr);
            if(HI_SUCCESS != s32Ret)
            {
                printf("3.RTP video send error\n"); 
                return s32Ret;
            }
        }
        else//单nalu多包
        {
            int realpacketcount = (u32FrameLen-5) / (u32PackPayloadLen-14);
            int lastpacketlen = (u32FrameLen-5) % (u32PackPayloadLen-14);
            int j = 0;
            HI_U8 *pNalu = (HI_U8 *)pPackDataAddr;
            pPackDataAddr += 5;
            for(j = 0; j <= realpacketcount; j++)
            {
                u8Marker = 0;
                if(0 == j)//第一包
                {
                    s32Sendlen = (u32PackPayloadLen-14);

                    s32Ret = HI_RTP_FU_PackageHeader(pPackBuf+ sizeof( RTP_HDR_S ) ,pNalu,1,0);
                }
                else if(j == realpacketcount)//最后一包
                {
                    u8Marker = 1;
                    s32Sendlen = lastpacketlen;
                    s32Ret = HI_RTP_FU_PackageHeader(pPackBuf+ sizeof( RTP_HDR_S ) ,pNalu,0,1);
                }
                else//中间包
                {
                    s32Sendlen = (u32PackPayloadLen-14);
                    s32Ret = HI_RTP_FU_PackageHeader(pPackBuf+ sizeof( RTP_HDR_S ) ,pNalu,0,0);
                }
                if(HI_SUCCESS != s32Ret)
				{
					HI_LOG_SYS("MTRANS",LOG_LEVEL_NOTICE,"RTP FU Head pack error %X\n",s32Ret); 
					return s32Ret;
				}
                /*cp数据*/
				memcpy(pPackBuf+ sizeof( RTP_HDR_S )+2,pPackDataAddr,s32Sendlen);
                
                u32RTPSendLen = s32Sendlen + sizeof( RTP_HDR_S ) + 2;
                HI_RTP_Packet( pPackBuf, u32TimeStamp,u8Marker, PackageType, u32Ssrc,u32SeqNum++);
                s32Ret = HI_MTRANS_Send(WriteSock, pPackBuf,u32RTPSendLen,pPeerSockAddr);
				if(HI_SUCCESS != s32Ret)
				{
					printf("6.RTP video send error\n"); 
					return s32Ret;
				}

				pPackDataAddr +=  s32Sendlen; /*更新指向数据的指针*/
            }
        }
		s32FindFramelen += u32FrameLen;
	}
	*pu32SeqNum = u32SeqNum; /*在内部发送了多个包,将序号返回*/
    if(iframe)
        return MTRANS_SEND_IFRAME;
    
	return HI_SUCCESS;
}

HI_S32 MTRANS_SendDataInComm(MTRANS_TASK_S* pTask,PACK_TYPE_E enPackType,
            HI_CHAR* pDataAddr,HI_U32 u32DataLen,HI_U32 u32Pts, RTP_PT_E RtpPackageType,
            HI_U32 *pu32SeqNum,HI_U32 u32Ssrc,HI_SOCKET WriteSock,struct sockaddr_in *pPeerSockAddr )
{
    HI_S32 s32Ret = 0;
    //HI_U32 u32SeqNum = 0;
    HI_CHAR *pPackAddr = NULL;
    HI_U32 u32PackLen = 0;
    /*get packet addr and len, gaving data addr and data len*/
    //MTRANSCalcPacketInfo(enPackType,pDataAddr,u32DataLen,&pu8PackAddr,&u32PackLen);

     /*2 process packet according different packet type*/
    if((PACK_TYPE_RTSP_ITLV == enPackType)||(PACK_TYPE_RTSP_O_HTTP == enPackType))
    {
        u32PackLen = sizeof(RTSP_ITLEAVED_HDR_S) + u32DataLen;		
				//u32DataLen+sizeof(RTSP_ITLEAVED_HDR_S):相当于数据长度加上了RTP+RTSP的数据头的长度
        pPackAddr = pDataAddr - sizeof(RTSP_ITLEAVED_HDR_S);
				//pDataAddr-sizeof(RTSP_ITLEAVED_HDR_S):相当于在视频数据之前加了一个rtp+rtsp数据头
        /*pack stream as rtsp interleaved*/
        HI_RTSP_ITLV_Packet(pPackAddr,u32PackLen,u32Pts,0,RtpPackageType,u32Ssrc,(HI_U16)(*pu32SeqNum),
        	pTask->struTcpItlvTransInfo.au32CliITLMediaDataChnId[MTRANS_STREAM_VIDEO]);
    }
    else if(PACK_TYPE_HISI_ITLV == enPackType)
    {
        u32PackLen = sizeof(HISI_ITLEAVED_HDR_S) + u32DataLen;
        pPackAddr = pDataAddr - sizeof(HISI_ITLEAVED_HDR_S);
        /*pack stream as rtsp interleaved*/
        HI_HISI_ITLV_Packet(pPackAddr,u32PackLen,u32Pts,0,RtpPackageType,u32Ssrc,(HI_U16)(*pu32SeqNum));
    }
    else if(PACK_TYPE_RTP == enPackType)
    {
        u32PackLen = sizeof(RTP_HDR_S) + u32DataLen;
        pPackAddr = pDataAddr - sizeof(RTP_HDR_S);
        /*pack stream as rtsp interleaved*/
        HI_RTP_Packet(pPackAddr,u32Pts,0,RtpPackageType,u32Ssrc,(HI_U16)(*pu32SeqNum));
    }
    else if(PACK_TYPE_RTP_STAP == enPackType)
    {
        u32PackLen = sizeof(RTP_HDR_S) + 3 + u32DataLen;
        /*stap 打包格式:rtp头+3字节+数据*/
        pPackAddr = pDataAddr - sizeof(RTP_HDR_S)-3;
        /*pack stream as rtsp interleaved*/
        HI_RTP_STAP_Packet(pPackAddr,u32PackLen,u32Pts,0,RtpPackageType,u32Ssrc,(HI_U16)(*pu32SeqNum));
    }
    else if(PACK_TYPE_LANGTAO == enPackType)
    {
        u32PackLen = sizeof(RTP_HDR_S) + 3 + u32DataLen;
        /*stap 打包格式:rtp头+3字节+数据*/
        pPackAddr = pDataAddr - sizeof(RTP_HDR_S)-3;
        /*pack stream as rtsp interleaved*/
        HI_RTP_STAP_Packet(pPackAddr,u32PackLen,u32Pts,0,RtpPackageType,u32Ssrc,(HI_U16)(*pu32SeqNum));
    }
    else
    {
        DEBUG_MTRANS_PRINT("MTRANS",LOG_LEVEL_ERR,"pack stream: unregonized packet type %d\n ",enPackType);
        return HI_ERR_MTRANS_UNSUPPORT_PACKET_TYPE;
    }
    /*printf("pack type=%d data addr %X len %d, pack addr %X len %d\n",
    enPackType,pDataAddr,u32DataLen,pPackAddr,u32PackLen);
    */
    /*测试用:保存打包后数据*/
     //DEBUGBeforeTransSave(pu8PackAddr,u32PackLen,enMbuffPayloadType,2);
   
    /*3 sending the media*/
    s32Ret = HI_MTRANS_Send(WriteSock,pPackAddr,u32PackLen,pPeerSockAddr);
    if(s32Ret != HI_SUCCESS)
    {
        return s32Ret;
    }
    /*发送序号更新*/
    else
    {
        (*pu32SeqNum)++;
    }
    return HI_SUCCESS;
}

/**
*TCP传输模式处理
*
**/
HI_S32 OnProcItlvTrans(HI_U32 pTask,HI_U32 privData)
{   
	HI_S32 s32Ret = 0;
	MTRANS_TASK_S *pTaskHandle = NULL;
	MTRANS_TASK_STATE_E enVedioState = TRANS_TASK_STATE_BUTT;
	MTRANS_TASK_STATE_E enAudioState = TRANS_TASK_STATE_BUTT;
	MTRANS_TASK_STATE_E enDataState = TRANS_TASK_STATE_BUTT;
	//MBUF_HEADER_S *pMBuffHandle  = NULL;
	HI_VOID *pMBuffHandle  = NULL;
	HI_CHAR*    pBuffAddr     = NULL;          /*pointer to addr of stream ready to send */

//	HI_CHAR* pBuffAddrForL = NULL;	/*pointer to addr of stream ready to send for langtao*/

	HI_U32     u32DataLen      = 0;          /*len of stream ready to send*/
	HI_U32     u32Pts          = 0;          /*stream pts*/
	HI_U32     u32CalcedPts    = 0;          /*换算过的pts,FU-A打包时使用*/
	MBUF_PT_E  enMbuffPayloadType = MBUF_PT_BUTT;    /*payload type*/
	HI_SOCKET s32WriteSock = -1;
	HI_BOOL   bDiscardFlag = HI_FALSE;
	HI_U32   u32LastSn = 0;
	struct sockaddr_in* pPeerSockAddr = NULL;
	MTRANS_MODE_E      enTransMode = MTRANS_MODE_BUTT;
	struct sockaddr_in stPeerSockAddr;
	PACK_TYPE_E enPackType = PACK_TYPE_BUTT;
	RTP_PT_E enRtpPayloadType = RTP_PT_INVALID;
	HI_U32 u32Ssrc = 0;
//	HI_S32 s32Chn = 0;

//	struct timeval strPreTime;
//	struct timeval strProTime;

	HI_U8  slice_type = 0;
	HI_U32  slicenum = 0;
	HI_U32  frame_len = 0;

	HI_U32  u32VideoFrameIndex = 0;
	HI_U32  u32VideoFramePacketIndex = 0;
	HI_U32  u32VideoPacketIndex = 0;
	HI_S32 	audioPTSXX = 8;

	if(0 == pTask)
	{
		return HI_ERR_MTRANS_ILLEGAL_PARAM;
	}
	
	pTaskHandle = (MTRANS_TASK_S*)pTask;
	enTransMode = pTaskHandle->enTransMode;
	/* mtrans mode must be tcp interleaved or udp 
	otherwise, media data is not send by session thread*/
	if(MTRANS_MODE_TCP_ITLV != enTransMode
		&& MTRANS_MODE_UDP != enTransMode)
	{
		return HI_SUCCESS;
	}
	/*get all media's state of the trans task */
	MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_VIDEO,&enVedioState);
	MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_AUDIO,&enAudioState);
	MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_DATA,&enDataState); 
	pMBuffHandle = (MBUF_HEADER_S*)(pTaskHandle->u32MbuffHandle);
	/* 1 if one of meida types has in playing state, get meida from mbuff*/
	if( (TRANS_TASK_STATE_PLAYING != enVedioState )
		&&( TRANS_TASK_STATE_PLAYING != enAudioState)
		&&( TRANS_TASK_STATE_PLAYING != enDataState)
	)
	/*trans task is not ready to send media*/  
	{
		/*video,audio and data are all not playing,so sleep 1 s,
		in case of http session loop busy*/
		(HI_VOID)usleep(15000);
		return HI_SUCCESS;
	}
	else 
	{
        if(pTaskHandle->enTransMode == MTRANS_MODE_UDP && pTaskHandle->struUdpTransInfo.au32SeqNum[0] == 0)
            usleep(3000);
		/*request data from mediabuff */
		s32Ret = HI_BufAbstract_Read(pTaskHandle->liveMode, pMBuffHandle, (HI_ADDR*)(&pBuffAddr), &u32DataLen, &u32Pts, &enMbuffPayloadType,&slice_type,&slicenum,&frame_len);
        if(HI_SUCCESS != s32Ret)
        {
            (HI_VOID)usleep(2000);
            return HI_ERR_MTRANS_NO_DATA;
        }
        /*judge the readed data is whether in playing state, if is not, just free this slice and not sending*/
        else
        {
            switch (enMbuffPayloadType)
            {
            case VIDEO_KEY_FRAME:
            case VIDEO_NORMAL_FRAME:
                (TRANS_TASK_STATE_PLAYING != enVedioState)?
                    (bDiscardFlag=HI_TRUE):(bDiscardFlag=HI_FALSE);
                break;
            case AUDIO_FRAME:
                (TRANS_TASK_STATE_PLAYING != enAudioState)?
                    (bDiscardFlag=HI_TRUE):(bDiscardFlag=HI_FALSE);       
                break;
            case MD_FRAME:      
                (TRANS_TASK_STATE_PLAYING != enDataState)?
                    (bDiscardFlag=HI_TRUE):(bDiscardFlag=HI_FALSE);
                break;
            default:
                bDiscardFlag=HI_TRUE;
                break;
            }
            /*if this media type is not in playing state, discard the slice*/
            if(HI_TRUE == bDiscardFlag)
            {
                HI_BufAbstract_Set(pTaskHandle->liveMode,pMBuffHandle);
                return HI_SUCCESS;
            }
		}
	}

	/*to do:  do some statistic*/
	//DEBUGBeforeTransSave(pBuffAddr,u32DataLen,enMbuffPayloadType,1);
	/*分别计算音视频的时间戳*/
	if( VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
	{
		u32CalcedPts = (HI_U32)(u32Pts * 90) ;
	}
	else if( AUDIO_FRAME == enMbuffPayloadType)
	{  
#if 1	
		
		audioPTSXX = pTaskHandle ->audioSampleRate /1000;
		//u32CalcedPts = (HI_U32)(u32Pts * 8);
		u32CalcedPts = (HI_U32)(u32Pts * audioPTSXX);
		//printf("audioPTSXX =%d \n",audioPTSXX);
#else
		//给傅工改的 48000 hz 的AAC声音
		u32CalcedPts = (HI_U32)(u32Pts * 48); 
		
#endif		
	}
  //  printf("DEBUG %s %d GetPacketAndSendParam \n",__FILE__,__LINE__);
	s32Ret = MTRANS_GetPacketAndSendParam(pTaskHandle,enMbuffPayloadType,&s32WriteSock,
						&stPeerSockAddr,&u32LastSn,&enPackType,&enRtpPayloadType,&u32Ssrc,
						&u32VideoFrameIndex,&u32VideoFramePacketIndex,&u32VideoPacketIndex);
	if(HI_SUCCESS != s32Ret)
	{
		HI_BufAbstract_Set(pTaskHandle->liveMode,pMBuffHandle);
		printf("DEBUG %s,%d OnProcItlvTrans step4.2 return %d\n",__FILE__,__LINE__,s32Ret);
		return s32Ret;
	} 
    
    if(MTRANS_MODE_TCP_ITLV == enTransMode)
    {
    	pPeerSockAddr = NULL;
    }
    else if(MTRANS_MODE_UDP == enTransMode)
    {
        pPeerSockAddr = &stPeerSockAddr;
    }
    /*rtp fu-a 打包方式:需要切包发送,故和其它打包方式的发送处理分开*/
    if(PACK_TYPE_RTP_FUA == enPackType)
    {
        if((enMbuffPayloadType == VIDEO_KEY_FRAME)||(enMbuffPayloadType == VIDEO_NORMAL_FRAME)
            ||(enMbuffPayloadType == AUDIO_FRAME))
        {
        	//if (enMbuffPayloadType == AUDIO_FRAME)
        	//	printf("DEBUG %s %d MTRANS_SendDataInRtpFUA u32Ssrc=%08x \n",__FILE__,__LINE__,u32Ssrc,enMbuffPayloadType);
        	s32Ret = MTRANS_SendDataInRtpFUA(pTaskHandle,pBuffAddr,u32DataLen,u32CalcedPts,
                enRtpPayloadType,&u32LastSn,u32Ssrc,s32WriteSock, pPeerSockAddr);
        }
    }
	else if(PACK_TYPE_RTSP_ITLV == enPackType)
    {
        pPeerSockAddr = NULL;
		//if (enMbuffPayloadType == AUDIO_FRAME)
		//	printf("audio----- u32DataLen=%d ,enRtpPayloadType=%d\n",u32DataLen,enRtpPayloadType);
		//printf("DEBUG %s %d MTRANS_SendDataInRtpFUA tcp datalen=%d,ssrc=%x\n",__FILE__,__LINE__,u32DataLen,u32Ssrc);
		
		s32Ret = MTRANS_SendDataInRtpFUA_TCP(pTaskHandle,pBuffAddr,u32DataLen,u32CalcedPts,
						enRtpPayloadType,&u32LastSn,u32Ssrc,s32WriteSock, pPeerSockAddr);
	}
	else
    {
		printf("%s, Unsupport enPackType:%d\n",__FUNCTION__, enPackType);
		return -1;
    }
    
    if(HI_SUCCESS != s32Ret && MTRANS_SEND_IFRAME != s32Ret)
    {
       printf("%s(%d) get base stamp:MBUF_Read read no data \n", __FUNCTION__, __LINE__);
       //HI_BufAbstract_Set(pTaskHandle->liveMode,pMBuffHandle);
       return s32Ret;
    }
    else
    {
        HI_BufAbstract_Set(pTaskHandle->liveMode,pMBuffHandle);
        MTRANS_UpdateSeqNum(pTaskHandle,enMbuffPayloadType,
                            enTransMode,u32LastSn);
    }
    return s32Ret;
}

void *multicast_send_thread(void *arg)
{   
    HI_S32 s32Ret = 0;
    MTRANS_TASK_S *pTaskHandle = (MTRANS_TASK_S *)arg;
    HI_VOID *pMBuffHandle  = NULL;
    HI_CHAR*    pBuffAddr     = NULL;          /*pointer to addr of stream ready to send */

    HI_U32     u32DataLen      = 0;          /*len of stream ready to send*/
    HI_U32     u32Pts          = 0;          /*stream pts*/
    HI_U32     u32CalcedPts    = 0;          /*换算过的pts,FU-A打包时使用*/
    MBUF_PT_E  enMbuffPayloadType = MBUF_PT_BUTT;    /*payload type*/
    HI_SOCKET s32WriteSock = -1;
    HI_BOOL   bDiscardFlag = HI_FALSE;
    HI_U32   u32LastSn = 0;
    struct sockaddr_in* pPeerSockAddr = NULL;
    MTRANS_MODE_E      enTransMode = MTRANS_MODE_BUTT;
    struct sockaddr_in stPeerSockAddr;
    PACK_TYPE_E enPackType = PACK_TYPE_BUTT;
    RTP_PT_E enRtpPayloadType = RTP_PT_INVALID;
    HI_U32 u32Ssrc = 0;

    HI_U8  slice_type = 0;
    HI_U32  slicenum = 0;
    HI_U32  frame_len = 0;

    HI_U32  u32VideoFrameIndex = 0;
    HI_U32  u32VideoFramePacketIndex = 0;
    HI_U32  u32VideoPacketIndex = 0;

    enTransMode = pTaskHandle->enTransMode;
    /* mtrans mode must be tcp interleaved or udp 
    otherwise, media data is not send by session thread*/

    if(!pTaskHandle || pTaskHandle->enTransMode != MTRANS_MODE_MULTICAST)
    {
        return (void *)-1;
    }

    char cName[128] = {0};
    sprintf(cName,"RtpMcast%d_%d", pTaskHandle->struUdpTransInfo.chno, pTaskHandle->struUdpTransInfo.streamType);
    prctl(PR_SET_NAME, (unsigned long)cName, 0,0,0);
    
    printf("%s  %d, start multicast task.Port:%u\n",__FUNCTION__, __LINE__,pTaskHandle->struUdpTransInfo.cliMediaPort[0]);
    #if 0
    pthread_mutex_lock(&pTaskHandle->struUdpTransInfo.multicastLock);
    if(pTaskHandle->struUdpTransInfo.threadstatus)
    {
        pthread_mutex_unlock(&pTaskHandle->struUdpTransInfo.multicastLock);
        printf("%s  %d, Multicast task already started,so thread exit!\n",__FUNCTION__, __LINE__);
        return (void *)-1;
    }
    pTaskHandle->struUdpTransInfo.threadstatus = 1;
    pthread_mutex_unlock(&pTaskHandle->struUdpTransInfo.multicastLock);
    #endif

    pMBuffHandle = (HI_VOID *)(pTaskHandle->u32MbuffHandle);

    /* 1 if one of meida types has in playing state, get meida from mbuff*/
    if( (TRANS_TASK_STATE_PLAYING != pTaskHandle->enTaskState[MTRANS_STREAM_VIDEO])
        &&(TRANS_TASK_STATE_PLAYING != pTaskHandle->enTaskState[MTRANS_STREAM_AUDIO])
        &&(TRANS_TASK_STATE_PLAYING != pTaskHandle->enTaskState[MTRANS_STREAM_DATA])
    )
    /*trans task is not ready to send media*/  
    {
        printf("%s  %d, Multicast task is not ready to work!\n",__FUNCTION__, __LINE__);
        goto thr_end;
    }

    while(pTaskHandle->struUdpTransInfo.clientNum>0)
    {
        /*request data from mediabuff */
        s32Ret = HI_BufAbstract_Read(pTaskHandle->liveMode,pMBuffHandle, (HI_ADDR*)(&pBuffAddr), &u32DataLen, &u32Pts, &enMbuffPayloadType,&slice_type,&slicenum,&frame_len);
        if(HI_SUCCESS != s32Ret)
        {
            printf("%s  %d, HI_BufAbstract_Read failed.\n",__FUNCTION__, __LINE__);
            goto thr_end;
        }
        /*judge the readed data is whether in playing state, if is not, just free this slice and not sending*/
        else
        {
            switch (enMbuffPayloadType)
            {
                case VIDEO_KEY_FRAME:
                case VIDEO_NORMAL_FRAME:
                    (TRANS_TASK_STATE_PLAYING != pTaskHandle->enTaskState[MTRANS_STREAM_VIDEO])?
                        (bDiscardFlag=HI_TRUE):(bDiscardFlag=HI_FALSE);
                    break;
                case AUDIO_FRAME:
                    (TRANS_TASK_STATE_PLAYING != pTaskHandle->enTaskState[MTRANS_STREAM_AUDIO])?
                        (bDiscardFlag=HI_TRUE):(bDiscardFlag=HI_FALSE);  					
                    break;
                case MD_FRAME:      
                    (TRANS_TASK_STATE_PLAYING != pTaskHandle->enTaskState[MTRANS_STREAM_DATA])?
                        (bDiscardFlag=HI_TRUE):(bDiscardFlag=HI_FALSE);
                    break;
                default:
                    bDiscardFlag=HI_TRUE;
                    break;
            }
            /*if this media type is not in playing state, discard the slice*/
            if(HI_TRUE == bDiscardFlag )
            {
            	//printf("DEBUG %s,%d  enMbuffPayloadType=%d go end\n",__FILE__,__LINE__,enMbuffPayloadType);

				if ((pTaskHandle->enTaskState[MTRANS_STREAM_VIDEO] != TRANS_TASK_STATE_PLAYING ) &&
					(pTaskHandle->enTaskState[MTRANS_STREAM_AUDIO] != TRANS_TASK_STATE_PLAYING ))
                	goto thr_end;
              	else
              		continue;
            }
        }

        /*to do:  do some statistic*/
        /*分别计算音视频的时间戳*/
        if( VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
        {
            u32CalcedPts = (HI_U32)(u32Pts * 90) ;
        }
        else if( AUDIO_FRAME == enMbuffPayloadType)
        {  
            u32CalcedPts = (HI_U32)(u32Pts * 8);
        }
		//printf("DEBUG %s %d GetPacketAndSendParam \n",__FILE__,__LINE__);
        s32Ret = MTRANS_GetPacketAndSendParam(pTaskHandle,enMbuffPayloadType,&s32WriteSock,
            &stPeerSockAddr,&u32LastSn,&enPackType,&enRtpPayloadType,&u32Ssrc,
            &u32VideoFrameIndex,&u32VideoFramePacketIndex,&u32VideoPacketIndex);
        if(HI_SUCCESS != s32Ret)
        {
        	printf("DEBUG %s,%d go end\n",__FILE__,__LINE__);
            goto thr_end;
        }
        pPeerSockAddr = &stPeerSockAddr;
        
        /*rtp fu-a 打包方式:需要切包发送,故和其它打包方式的发送处理分开*/
        if(PACK_TYPE_RTP_FUA != enPackType)
        {
        	printf("DEBUG %s,%d fua go end\n",__FILE__,__LINE__);
            goto thr_end;
        }
        
        if((enMbuffPayloadType == VIDEO_KEY_FRAME)||(enMbuffPayloadType == VIDEO_NORMAL_FRAME)
            ||(enMbuffPayloadType == AUDIO_FRAME))
        {
//        	printf("DEBUG %s %d MTRANS_SendDataInRtpFUA\n",__FILE__,__LINE__);
            s32Ret = MTRANS_SendDataInRtpFUA(pTaskHandle,pBuffAddr,u32DataLen,u32CalcedPts,
                enRtpPayloadType,&u32LastSn,u32Ssrc,s32WriteSock, pPeerSockAddr);
        }
        
        if(HI_SUCCESS != s32Ret && MTRANS_SEND_IFRAME != s32Ret)
        {
            printf("%s(%d) get base stamp:MBUF_Read read no data \n", __FUNCTION__, __LINE__);
            goto thr_end;
        }
        else
        {
            MTRANS_UpdateSeqNum(pTaskHandle,enMbuffPayloadType,enTransMode,u32LastSn);
        }
    }

thr_end:
    pthread_mutex_lock(&pTaskHandle->struUdpTransInfo.multicastLock);
    pTaskHandle->struUdpTransInfo.threadstatus = 0;
    pthread_cond_signal(&pTaskHandle->struUdpTransInfo.multicastCond);
    pthread_mutex_unlock(&pTaskHandle->struUdpTransInfo.multicastLock);
    printf("%s  %d, Multicast thread exit.\n",__FUNCTION__, __LINE__);
    return (void *)0;
}

unsigned long GetTick()
{
    struct timeval stTVal1;
    gettimeofday(&stTVal1, NULL);
    return (stTVal1.tv_sec%1000000)*1000+stTVal1.tv_usec/1000;
}

HI_S32 MTRANS_playback_send(HI_U32 pTask,HI_U32 privData)
{   
	HI_S32 s32Ret = 0;
	MTRANS_TASK_S *pTaskHandle = NULL;
	MTRANS_TASK_STATE_E enVedioState = TRANS_TASK_STATE_BUTT;
	MTRANS_TASK_STATE_E enAudioState = TRANS_TASK_STATE_BUTT;
	MTRANS_TASK_STATE_E enDataState = TRANS_TASK_STATE_BUTT;
	HI_VOID *pMBuffHandle  = NULL;
	HI_CHAR*    pBuffAddr     = NULL;          /*pointer to addr of stream ready to send */

	HI_U32     u32DataLen      = 0;          /*len of stream ready to send*/
	HI_U32     u32Pts          = 0;          /*stream pts*/
	HI_U32     u32CalcedPts    = 0;          /*换算过的pts,FU-A打包时使用*/
	MBUF_PT_E  enMbuffPayloadType = MBUF_PT_BUTT;    /*payload type*/
	HI_SOCKET s32WriteSock = -1;
	HI_U32   u32LastSn = 0;
	struct sockaddr_in* pPeerSockAddr = NULL;
	MTRANS_MODE_E      enTransMode = MTRANS_MODE_BUTT;
	struct sockaddr_in stPeerSockAddr;
	PACK_TYPE_E enPackType = PACK_TYPE_BUTT;
	RTP_PT_E enRtpPayloadType = RTP_PT_INVALID;
	HI_U32 u32Ssrc = 0;

	HI_U32  u32VideoFrameIndex = 0;
	HI_U32  u32VideoFramePacketIndex = 0;
	HI_U32  u32VideoPacketIndex = 0;

    struct MTRANS_PlayBackInfo *pPbInfo;

	if(0 == pTask || privData == 0)
	{
		return HI_ERR_MTRANS_ILLEGAL_PARAM;
	}

    pPbInfo = (struct MTRANS_PlayBackInfo *)privData;

	pTaskHandle = (MTRANS_TASK_S*)pTask;
	enTransMode = pTaskHandle->enTransMode;

	/*get all media's state of the trans task */
	MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_VIDEO,&enVedioState);
	MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_AUDIO,&enAudioState);
	MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_DATA,&enDataState); 
	pMBuffHandle = (MBUF_HEADER_S*)(pTaskHandle->u32MbuffHandle);

	/* 1 if one of meida types has in playing state, get meida from mbuff*/
	if( (TRANS_TASK_STATE_PLAYING != enVedioState )
		&&( TRANS_TASK_STATE_PLAYING != enAudioState)
		&&( TRANS_TASK_STATE_PLAYING != enDataState)
	)
	/*trans task is not ready to send media*/  
	{
		/*video,audio and data are all not playing,so sleep 1 s,
		in case of http session loop busy*/
		(HI_VOID)usleep(1500000);
		return HI_SUCCESS;
	}

    if(pPbInfo->cmd == RTSP_SEEK_FLAG)
    {
        pPbInfo->videoframe = 0;
        pPbInfo->pData == NULL;
    }

    int read_new = 0;
    if(pPbInfo->pData == NULL)
    {
    	/*request data from mediabuff */
    	s32Ret = BufAbstract_Playback_Read(pTaskHandle->liveMode,pMBuffHandle, (HI_ADDR*)(&pBuffAddr), &u32DataLen, &u32Pts, &enMbuffPayloadType);
        if(HI_FAILURE == s32Ret)
        {
            usleep(20000);
            return HI_ERR_MTRANS_NO_DATA;
        }
        else if(s32Ret == 1)
        {
            usleep(20000);
            //return HI_SUCCESS;
            return HI_ERR_MTRANS_NO_DATA;
        }
        read_new = 1;
    }
    else
    {
        pBuffAddr = pPbInfo->pData;
        u32DataLen = pPbInfo->datalen;
        u32Pts = pPbInfo->timestamp;
        enMbuffPayloadType = VIDEO_NORMAL_FRAME;

        read_new = 0;

        //pPbInfo->pData = NULL;
        //pPbInfo->datalen = 0;
        //pPbInfo->timestamp = 0;
    }

    if(pPbInfo->cmd == RTSP_SEEK_FLAG)
    {
        pPbInfo->VideoTick = u32Pts;
        pPbInfo->tick = GetTick();
        pPbInfo->cmd = 0;
    }
#if 1
    if( VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
    {
        if(pPbInfo->videoframe>0 && pPbInfo->videoframe%50==0)
        {
            unsigned long now = GetTick();
            unsigned long tick = now - pPbInfo->tick;
            unsigned long sendtick = u32Pts - pPbInfo->VideoTick;
            //printf("%s, aaaaaaaaa,now:%lu,tick:%lu,sendtick:%lu,vt:%lu,pts:%lu\n",__FUNCTION__,now,tick,sendtick,pPbInfo->VideoTick,u32Pts);
            if(tick<sendtick)
            {
                //printf("%s bbbbbbbbbbbbbb\n",__FUNCTION__);
                //if(read_new)
                {
                    pPbInfo->datalen = u32DataLen;
                    pPbInfo->timestamp = u32Pts;
                    pPbInfo->pData = (void*)pBuffAddr;
                }
                usleep(5000);
                return HI_SUCCESS;
            }

            pPbInfo->tick = now;
            pPbInfo->VideoTick = u32Pts;
            pPbInfo->pData = NULL;
        }

        if(read_new)
            pPbInfo->videoframe++;
    }
#endif

	/*分别计算音视频的时间戳*/
	if( VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
	{
		u32CalcedPts = (HI_U32)(u32Pts * 90) ;
	}
	else if( AUDIO_FRAME == enMbuffPayloadType)
	{  
		u32CalcedPts = (HI_U32)(u32Pts * 8);
	}

    //printf("DEBUG %s %d GetPacketAndSendParam \n",__FILE__,__LINE__); 
	s32Ret = MTRANS_GetPacketAndSendParam(pTaskHandle,enMbuffPayloadType,&s32WriteSock,
						&stPeerSockAddr,&u32LastSn,&enPackType,&enRtpPayloadType,&u32Ssrc,
						&u32VideoFrameIndex,&u32VideoFramePacketIndex,&u32VideoPacketIndex);
	if(HI_SUCCESS != s32Ret)
	{
		HI_BufAbstract_Set(pTaskHandle->liveMode,pMBuffHandle);
		return s32Ret;
	} 
    
    if(MTRANS_MODE_TCP_ITLV == enTransMode)
    {
    	pPeerSockAddr = NULL;
    }
    else if(MTRANS_MODE_UDP == enTransMode)
    {
        pPeerSockAddr = &stPeerSockAddr;
    }
    /*rtp fu-a 打包方式:需要切包发送,故和其它打包方式的发送处理分开*/
    if(PACK_TYPE_RTP_FUA == enPackType)
    {
        if((enMbuffPayloadType == VIDEO_KEY_FRAME)||(enMbuffPayloadType == VIDEO_NORMAL_FRAME)
            ||(enMbuffPayloadType == AUDIO_FRAME))
        {
        //	printf("DEBUG %s %d MTRANS_SendDataInRtpFUA\n",__FILE__,__LINE__);
        	s32Ret = MTRANS_SendDataInRtpFUA(pTaskHandle,pBuffAddr,u32DataLen,u32CalcedPts,
                enRtpPayloadType,&u32LastSn,u32Ssrc,s32WriteSock, pPeerSockAddr);
        }
    }
	else if(PACK_TYPE_RTSP_ITLV == enPackType)
    {
        pPeerSockAddr = NULL;
	//	printf("DEBUG %s %d MTRANS_SendDataInRtpFUA tcp\n",__FILE__,__LINE__);
		s32Ret = MTRANS_SendDataInRtpFUA_TCP(pTaskHandle,pBuffAddr,u32DataLen,u32CalcedPts,
						enRtpPayloadType,&u32LastSn,u32Ssrc,s32WriteSock, pPeerSockAddr);
	}
	else
    {
		printf("%s, Unsupport enPackType:%d\n",__FUNCTION__, enPackType);
		return -1;
    }
    
    if(HI_SUCCESS != s32Ret && MTRANS_SEND_IFRAME != s32Ret)
    {
       printf("%s(%d) get base stamp:MBUF_Read read no data \n", __FUNCTION__, __LINE__);
       return s32Ret;
    }
    else
    {
        MTRANS_UpdateSeqNum(pTaskHandle,enMbuffPayloadType,
                            enTransMode,u32LastSn);
    }
    usleep(20000);
    return s32Ret;
}
extern HI_S32 BufAbstract_Playback_Seek(HI_VOID *phandle, double seektime, HI_U32 *ppts);
HI_S32 MTRANS_playback_seek(HI_U32 pTask,double seektime, SeekResp_S *pSeekResp)
{
    HI_S32 s32Ret = 0;
    HI_U32 u32pts;
    MTRANS_TASK_S* pTaskHandle;
    pTaskHandle = (MTRANS_TASK_S*)pTask;

    s32Ret = BufAbstract_Playback_Seek((void *)pTaskHandle->u32MbuffHandle, seektime, &u32pts);
    if(s32Ret != HI_SUCCESS)
    {
        printf("%s, BufAbstract_Playback_Seek failed!\n",__FUNCTION__);
        return s32Ret;
    }

    pSeekResp->vtime = u32pts;
    pSeekResp->atime = u32pts;
    
    if(pTaskHandle->enTransMode == MTRANS_MODE_TCP_ITLV)
    {
        pSeekResp->vseq = pTaskHandle->struTcpItlvTransInfo.u32VSendSeqNum;
        pSeekResp->aseq = pTaskHandle->struTcpItlvTransInfo.u32ASendSeqNum;
    }
    else
    {
        pSeekResp->vseq = pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_VIDEO];
        pSeekResp->aseq = pTaskHandle->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_AUDIO];
    }
    return HI_SUCCESS;
}



/*func :HI_MTRANS_Init
  Describe: init media trans model 
  input: s32ChnNum      : channel total number to support its media 
         pstruInitCfg   : pointer to init struct list
         s32TransTaskNum: max trans number to support 
*/
HI_S32 HI_MTRANS_Init(MTRANS_INIT_CONFIG_S* pstruInitCfg)
{
    HI_S32 s32Ret = 0;

    printf("Hi_mtrans Module Build:%s %s\n", __DATE__, __TIME__);
    /*check parameters*/
    if(pstruInitCfg == NULL)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }
	
    /*检查输入正确性*/
    if(pstruInitCfg->u32MaxTransTaskNum <= 0)
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,"MTRANS Init failed: illegal task num %d.\n",
                   pstruInitCfg->u32MaxTransTaskNum);
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }

    if(pstruInitCfg->u16MaxSendPort<= 0 || pstruInitCfg->u16MinSendPort<= 0
       || pstruInitCfg->u16MaxSendPort <= pstruInitCfg->u16MinSendPort)
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,"MTRANS Init failed: illegal udp prot %d-%d.\n",
                   pstruInitCfg->u16MinSendPort,pstruInitCfg->u16MaxSendPort);
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }

    if(pstruInitCfg->u32PackPayloadLen <= 0)
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,"MTRANS Init failed: illegal pack len %d.\n",
                   pstruInitCfg->u32PackPayloadLen);
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }

	HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_INFO,"MTRANS Init:conn num=%d minport-maxport=%d-%d,pack len=%d.\n",
               pstruInitCfg->u32MaxTransTaskNum,pstruInitCfg->u16MinSendPort,
               pstruInitCfg->u16MaxSendPort,pstruInitCfg->u32PackPayloadLen);
    /*if mtrans has already init return ok*/
    if(MTRANSSVR_STATE_Running == g_stMtransAttr.enSvrState)
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,"MTRANS Init failed: already inited.\n");
        return HI_SUCCESS;
    }

    /*clear mtrans interanl attribuiton struct */
    memset(&g_stMtransAttr,0,sizeof(MTRANS_ATTR_S));
    g_stMtransAttr.enSvrState = MTRANSSVR_STATE_STOP;

    g_stMtransAttr.u32MaxTransTaskNum = pstruInitCfg->u32MaxTransTaskNum;
    g_stMtransAttr.u16MinSendPort     = pstruInitCfg->u16MinSendPort;
    g_stMtransAttr.u16MaxSendPort     = pstruInitCfg->u16MaxSendPort;
    g_stMtransAttr.u32PackPayloadLen  = pstruInitCfg->u32PackPayloadLen;
	(HI_VOID)pthread_mutex_init(&g_stMtransAttr.MutexGetPort,  NULL);
    /*init trans task list: here use default max trans task number
      by set parameter 's32TotalTaskNum' <0*/
    s32Ret = HI_MTRANS_TaskListInit(pstruInitCfg->u32MaxTransTaskNum);
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    g_stMtransAttr.enSvrState =  MTRANSSVR_STATE_Running;
    DEBUG_MTRANS_PRINT(MTRANS_SEND, LOG_LEVEL_ERR,"HI_MTRANS_Init init ok.\n");

    return HI_SUCCESS;
}

/*deinit mtrans model: free all running send task and other global variables*/
HI_S32 HI_MTRANS_DeInit()
{
    /*if mtrans has already deinit return failed*/
    if(MTRANSSVR_STATE_STOP == g_stMtransAttr.enSvrState)
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,"MTRANS Init failed: already deinited.\n");
        return HI_SUCCESS;
    }

    /*1 free send task list */
    HI_MTRANS_TaskAllDestroy();

    /*init other global struct,such as init config struct*/
    memset(&g_stMtransAttr,0,sizeof(g_stMtransAttr));

    g_stMtransAttr.enSvrState =  MTRANSSVR_STATE_STOP;
    DEBUG_MTRANS_PRINT(MTRANS_SEND, LOG_LEVEL_ERR,"HI_MTRANS_deInit ok.\n");
    return HI_SUCCESS;
}

HI_S32 HI_MTRANS_SetMbufHandle(HI_U32 mtransHandle, HI_U32 pMbufHandle)
{
    if(!mtransHandle || !pMbufHandle)
        return -1;
    MTRANS_TASK_S *pTask = NULL;
    pTask = (MTRANS_TASK_S*) mtransHandle;
    pTask->u32MbuffHandle = (HI_U32)pMbufHandle;

    return 0;
    
}

/*creat a send task*/
//HI_S32 HI_MTRANS_CreatSendTask(MTRANS_SEND_CREATE_MSG_S *pstSendInfo,MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo)
HI_S32 HI_MTRANS_CreatSendTask(HI_U32* mtransHandle,MTRANS_SEND_CREATE_MSG_S *pstSendInfo,MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo)
{
    HI_S32 s32Ret = 0;
    MTRANS_TASK_S *pTask = NULL;

    /*check parameters*/
    if(NULL == pstSendInfo|| NULL == pstReSendInfo)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }
	
    /*now we just support tcp interleaved and udp trans mode.
      to do :增加了其他传输模式后去掉此项判断*/
    if(!(MTRANS_MODE_TCP_ITLV == pstSendInfo->enMediaTransMode
         || MTRANS_MODE_UDP == pstSendInfo->enMediaTransMode
         || MTRANS_MODE_MULTICAST == pstSendInfo->enMediaTransMode))
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR,"not support trans mode %d.\n",pstSendInfo->enMediaTransMode);
        return HI_ERR_MTRANS_NOT_SUPPORT_TRANSMODE;
    }

    if(mtransHandle == NULL)
    {
        /*creat a new trans task*/
        
        if(MTRANS_MODE_MULTICAST == pstSendInfo->enMediaTransMode)
        {
            int medianum = atoi(pstSendInfo->au8MediaFile);
            int chno = medianum/10-1;
            int streamtype = medianum%10-1;
            
            s32Ret = HI_MTRANS_TaskCreat_Multicast(&pTask, chno, streamtype);
        }
        else
        {
            s32Ret = HI_MTRANS_TaskCreat(&pTask);
        }
        
        
        if(HI_SUCCESS != s32Ret)
        {
            /*set response info with error code*/
            pstReSendInfo->s32ErrCode = s32Ret;
            return s32Ret;
        }
    }
    else
    {
        printf("have the mtrans task");
        pTask = (MTRANS_TASK_S*) mtransHandle;
        /*init trans task*/
    }   
    
    s32Ret = HI_MTRANS_TaskInit(pTask,pstSendInfo);
    if(HI_SUCCESS != s32Ret)
    {
        /*set response info with error code*/
        pstReSendInfo->s32ErrCode = s32Ret;
        return s32Ret;
    }
	
    
    /*consist reSendIfo*/
    s32Ret = HI_MTRANS_GetSvrTransInfo(pTask,pstSendInfo->u8MeidaType,pstReSendInfo);
    
    if(s32Ret != HI_SUCCESS)
    {
        /*set response info with error code*/
        pstReSendInfo->s32ErrCode = s32Ret;
        return s32Ret;
    }    
#if 0
	/*trans handle != NULL, it means user want to send other meida in a exist trans task
	example: user has created a task which just to send video media
	now, user can add audio media to the task
	but, here not allowed to add a meida which is already in ready or playing state*/
	else 
	{
		pTask = pstSendInfo->u32TransTaskHandle;

		/*modify exit task with the appointed task handle*/
		s32Ret = HI_MTRANS_TaskModify(pTask,pstReSendInfo);
		if(HI_SUCCESS != s32Ret)
		{
			HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR, "creat trans task failed: add meida to exist task failed %X.\n",
			s32Ret);
			return s32Ret;
		}
		else
		{
			/*consist reSendIfo*/
			HI_MTRANS_GetSvrTransInfo(pTask,pstSendInfo->u8MeidaType,pstReSendInfo);
		}
	}
#endif
    return HI_SUCCESS;
}

/*****************************************************************************
 函 数 名  : HI_MTRANS_AddSubTask
 功能描述  : add media to a created send task,
             for example, you have creat a trans task'A'to 
             send video by HI_MTRANS_CreatSendTask();
             now, you can add audio send subtask to task'A'
             by HI_MTRANS_AddSubtask();
 输入参数  : u32TransTaskHandle      : trans task handle
             pstSendInfo             : 
 输出参数  : pstReSendInfo     : 
 返 回 值  : HI_S32         : 0 -- success; !0 : failure
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2008年10月31日
    作    者   : w54723
    修改内容   : 新生成函数

*****************************************************************************/
/*
*/
HI_S32 HI_MTRANS_Add2CreatedTask(HI_U32 u32TransTaskHandle,
                                        MTRANS_SEND_CREATE_MSG_S *pstSendInfo,MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
//    HI_U8 u8ReqMeidaType = 0;
    MTRANS_TASK_S *pTask = NULL;
    /*check parameters*/
    if( NULL == pstSendInfo ||NULL == pstReSendInfo )
    {
        s32Ret = HI_ERR_MTRANS_ILLEGAL_PARAM;
        goto err;
    }
    
    /*not allow send task handle is null*/
    if( 0 == u32TransTaskHandle)
    {
        /*set response info with error code*/
        s32Ret = HI_ERR_MTRANS_ILLEGAL_PARAM;
        goto err;
    }

    pTask = (MTRANS_TASK_S *)u32TransTaskHandle;

    /*判断必须与之前请求的媒体类型的传输方式一致*/
    if(pTask->enTransMode != pstSendInfo->enMediaTransMode)
    {
        s32Ret = HI_ERR_MTRANS_NOT_SUPPORT_TRANSMODE;
        goto err;
    }

    /*判断必须与之前的媒体句柄一致*/
    if(pTask->u32MbuffHandle != pstSendInfo->u32MediaHandle)
    {
        s32Ret = HI_ERR_MTRANS_NOT_SUPPORT_TRANSMODE;
        goto err;
    }

    /*判断addsbuTask接口调用之前必须先调用creattask
     即判断:
     a 至少已申请一种媒体类型
     b 申请的媒体类型的状态不能是init*/
     for(i=0; i<MTRANS_STREAM_MAX; i++)
     {
        if(TRANS_TASK_STATE_INIT !=pTask->enTaskState[i])
        {
            break;
        }

        if(MTRANS_STREAM_MAX-1 == i)
        {
            s32Ret = HI_ERR_MTRANS_NOT_ALLOWED_CREAT_TASK;
            goto err;
        }
        continue;
     }
        
     /*2)若已经有媒体处在playing状态，则不支持改变该媒体属性或添加另一个媒体类型
     （即Video playing的同时，不允许setup请求改变video的传输参数，也不允许setup audio）*/
    // u8ReqMeidaType = pstSendInfo->u8MeidaType;
     for(i=0; i<MTRANS_STREAM_MAX; i++)
     {
        if(TRANS_TASK_STATE_PLAYING == pTask->enTaskState[i])
        {
            HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR, 
            "can't add or change media's param when there is a playing meidia.\n");
            s32Ret = HI_ERR_MTRANS_NOT_ALLOWED_CREAT_TASK;
            goto err;
        }
     }

    /*destroy the task*/
    //printf("mtrans creat hand = %X\n",pTask);
    HI_MTRANS_TaskSetAttr(pTask,pstSendInfo);

    /*consist reSendIfo*/
    s32Ret = HI_MTRANS_GetSvrTransInfo(pTask,pstSendInfo->u8MeidaType,pstReSendInfo);
    if(s32Ret != HI_SUCCESS)
    {
        goto err;
    }

    err:
    pstReSendInfo->s32ErrCode = s32Ret;
    return s32Ret;
     
}
HI_S32 HI_MTRANS_DestroySendTask(MTRANS_DESTROY_MSG_S *pstDestroyInfo,MTRANS_ReDESTROY_MSG_S *pstReDestroyInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MTRANS_TASK_S *pTask = NULL;
    /*check parameters*/
    if( NULL == pstDestroyInfo ||NULL == pstReDestroyInfo )
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }
    
    /*not allow send task handle is null*/
    if( 0 == pstDestroyInfo->u32TransTaskHandle)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }

    pTask = (MTRANS_TASK_S *)(pstDestroyInfo->u32TransTaskHandle);
    /*destroy the task*/
    s32Ret = HI_MTRANS_TaskDestroy(pTask);
    if(HI_SUCCESS != s32Ret)
    {
        DEBUG_MTRANS_PRINT(MTRANS_SEND,LOG_LEVEL_CRIT,"mtrans destroy task error %p\n",s32Ret);
        pstReDestroyInfo->s32ErrCode = s32Ret;
        return s32Ret;
    }
    
    pstReDestroyInfo->s32ErrCode = HI_SUCCESS;

    return s32Ret;
}

HI_S32 MTRANS_GetTaskStatusMulticast(HI_U32 u32TransTaskHandle)
{
    if(!u32TransTaskHandle)
        return HI_FAILURE;

    MTRANS_TASK_S *pTask = (MTRANS_TASK_S *)u32TransTaskHandle;
    return pTask->struUdpTransInfo.threadstatus;
}

HI_S32 MTRANS_StartMulticastSendTask(HI_U32 u32TransTaskHandle)
{
    int ret;
    pthread_t  tid;
    pthread_attr_t attr;
    
    if(!u32TransTaskHandle)
        return HI_FAILURE;

    MTRANS_TASK_S *pTask = (MTRANS_TASK_S *)u32TransTaskHandle;
    if(pTask->enTransMode != MTRANS_MODE_MULTICAST)
    {
        return HI_FAILURE;
    }
    
    pthread_mutex_lock(&pTask->struUdpTransInfo.multicastLock);
    if(pTask->struUdpTransInfo.threadstatus)
    {
        pthread_mutex_unlock(&pTask->struUdpTransInfo.multicastLock);
        printf("%s  %d, Multicast task had been started.\n",__FUNCTION__, __LINE__);
        return HI_SUCCESS;
    }
    pTask->struUdpTransInfo.threadstatus = 1;
    pthread_mutex_unlock(&pTask->struUdpTransInfo.multicastLock);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&tid, &attr, multicast_send_thread, (void *)pTask);
    pthread_attr_destroy(&attr);
    if(ret)
    {
        pthread_mutex_lock(&pTask->struUdpTransInfo.multicastLock);
        pTask->struUdpTransInfo.threadstatus = 0;
        pthread_mutex_unlock(&pTask->struUdpTransInfo.multicastLock);
        printf("%s  %d, create thread fail!ret=%d,%s(errno:%d)\n",
                __FUNCTION__, __LINE__, ret, strerror(errno), errno);
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

int MTRANS_StartMulticastStreaming(MT_MulticastInfo_s *pInfo)
{
    int ret;
    int need_init = 0;
    HI_S32 s32Chn = 0;
    MTRANS_TASK_S *pTask = NULL;
    MT_MEDIAINFO_S  stMediaInfo;
    HI_U8 enMediaType = 0;
    HI_VOID *pMbufHandle = NULL;
    
    memset(&stMediaInfo,0,sizeof(MT_MEDIAINFO_S));
    if(!pInfo)
        return HI_FAILURE;
    
    ret = HI_MTRANS_TaskCreat_Multicast_ext(&pTask, pInfo->channel, pInfo->streamType, pInfo->port[0]);
    if(ret != HI_SUCCESS)
    {
        printf("%s  %d, Create send task failed!\n",__FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    s32Chn = ((pInfo->channel+1)*10) + pInfo->streamType+1;

    pthread_mutex_lock(&pTask->struUdpTransInfo.multicastLock);
    if(pTask->struUdpTransInfo.InitFlag == 0)
    {
        if(pInfo->port[0] > 0)
            pTask->struUdpTransInfo.InitFlag |= 0x1;

        if(pInfo->port[1] > 0)
            pTask->struUdpTransInfo.InitFlag |= 0x2;

        need_init = 1;
    }
    pthread_mutex_unlock(&pTask->struUdpTransInfo.multicastLock);

    if(need_init)
    {
        ret = HI_BufAbstract_GetMediaInfo(s32Chn, &stMediaInfo);
        if(HI_SUCCESS != ret)
        {
            printf("%s  %d, HI_BufAbstract_GetMediaInfo error!\n",__FUNCTION__, __LINE__);
            HI_MTRANS_TaskDestroy(pTask);
            return HI_FAILURE;
        }

        pTask->u32FrameIndex = 0;
        pTask->u32FramePacketIndex = 0;
        //pTask->enTransMode = pstSendInfo->enMediaTransMode;
        pTask->videoFrame = stMediaInfo.u32Framerate;
        pTask->SPS_LEN = stMediaInfo.SPS_LEN;
        pTask->PPS_LEN = stMediaInfo.PPS_LEN;
        pTask->SEL_LEN = stMediaInfo.SEL_LEN;
		pTask->audioSampleRate = stMediaInfo.u32AudioSampleRate;
        strcpy(pTask->au8CliIP, pInfo->multicastAddr);
        pTask->u32PackDataLen = 0;
        pTask->enAudioFormat = stMediaInfo.enAudioType;
        pTask->enVideoFormat = stMediaInfo.enVideoType;
        pTask->liveMode = LIVE_MODE_1CHN1USER;
        pTask->u32MbuffHandle = 0;
        pTask->pu8PackBuff = (HI_CHAR *)malloc(MTRANS_GetPackPayloadLen()+128);
        if(pTask->pu8PackBuff == NULL)
        {
            printf("%s  %d, malloc memory failed!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
            HI_MTRANS_TaskDestroy(pTask);
            return HI_FAILURE;
        }
        
        if(stMediaInfo.u16EncodeVideo)
        {
            pTask->aenPackType[MTRANS_STREAM_VIDEO] = PACK_TYPE_RTP_FUA;
            pTask->au32Ssrc[MTRANS_STREAM_VIDEO] = (HI_U32)random();
            pTask->enTaskState[MTRANS_STREAM_VIDEO] = TRANS_TASK_STATE_READY;
            pTask->struUdpTransInfo.cliMediaPort[MTRANS_STREAM_VIDEO] = pInfo->port[0];
            pTask->struUdpTransInfo.svrMediaSendPort[MTRANS_STREAM_VIDEO] = pInfo->port[0];
            pTask->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_VIDEO] = 0;
            
            strcpy(pTask->struUdpTransInfo.au8CliIP[MTRANS_STREAM_VIDEO], pInfo->multicastAddr);
            enMediaType |= 0x1;
        }
        else
        {
            pTask->enTaskState[MTRANS_STREAM_VIDEO] = TRANS_TASK_STATE_BUTT;
        }

        if(stMediaInfo.u16EncodeAudio)
        {
            pTask->aenPackType[MTRANS_STREAM_AUDIO] = PACK_TYPE_RTP_FUA;
            pTask->au32Ssrc[MTRANS_STREAM_AUDIO] = (HI_U32)random();
            pTask->enTaskState[MTRANS_STREAM_AUDIO] = TRANS_TASK_STATE_READY;
            pTask->struUdpTransInfo.cliMediaPort[MTRANS_STREAM_AUDIO] = pInfo->port[1];
            pTask->struUdpTransInfo.svrMediaSendPort[MTRANS_STREAM_AUDIO] = pInfo->port[1];
            pTask->struUdpTransInfo.au32SeqNum[MTRANS_STREAM_AUDIO] = 0;
            pTask->struUdpTransInfo.as32SendSock[MTRANS_STREAM_AUDIO] = -1;
            pTask->struUdpTransInfo.as32RecvSock[MTRANS_STREAM_AUDIO] = -1;
            strcpy(pTask->struUdpTransInfo.au8CliIP[MTRANS_STREAM_AUDIO], pInfo->multicastAddr);
            enMediaType |= 0x2;
        }
        else
        {
            pTask->enTaskState[MTRANS_STREAM_AUDIO] = TRANS_TASK_STATE_BUTT;
        }

        pTask->enTaskState[MTRANS_STREAM_DATA] = TRANS_TASK_STATE_BUTT;
        pTask->struUdpTransInfo.as32SendSock[MTRANS_STREAM_DATA] = -1;
        pTask->struUdpTransInfo.as32RecvSock[MTRANS_STREAM_DATA] = -1;

        ret = HI_BufAbstract_Alloc(LIVE_MODE_1CHN1USER, s32Chn,&pMbufHandle);
        if(HI_SUCCESS != ret)
        {
            printf("%s  %d, HI_BufAbstract_Alloc failed!\n",__FUNCTION__, __LINE__);
            HI_MTRANS_TaskDestroy(pTask);
            return HI_FAILURE;
        }

        pTask->u32MbuffHandle = (HI_U32)pMbufHandle;

        ret = MTRANS_TaskStartMulticast(pTask,enMediaType);
        if(HI_SUCCESS != ret)
        {
            HI_MTRANS_TaskDestroy(pTask);
            printf("%s  %d, MTRANS_TaskStartMulticast error!\n",__FUNCTION__, __LINE__);
            return HI_FAILURE;
        }

        HI_BufAbstract_RequestIFrame(s32Chn);
	//	printf("DEBUG %s %d MTRANS_StartMulticastSendTask\n"__FILE__,__LINE__);
        ret = MTRANS_StartMulticastSendTask((HI_U32)pTask);
        return ret;

        #if 0
        pthread_t  tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&tid, &attr, multicast_send_thread, (void *)pTask);
	
	pthread_attr_destroy(&attr);

        if(ret)
	{
            printf("%s  %d, create thread fail!ret=%d,%s(errno:%d)\n",
                        __FUNCTION__, __LINE__, ret, strerror(errno), errno);

            HI_MTRANS_TaskDestroy(pTask);
            return HI_FAILURE;
	}
        #endif
            
    }

   return  HI_SUCCESS;
}

int MTRANS_StopMulticastStreaming(MT_MulticastInfo_s *pInfo)
{
    int ret;
    MTRANS_TASK_S *pTask = NULL;

    if(!pInfo)
        return HI_FAILURE;

    ret = HI_MTRANS_GetMulticastTaskHandle(&pTask, pInfo->channel, pInfo->streamType, pInfo->port[0]);
    if(HI_SUCCESS != ret)
    {
        return HI_SUCCESS;
    }

    HI_MTRANS_TaskDestroy(pTask);

    return HI_SUCCESS;
}

HI_S32 HI_MTRANS_StartSendTask(MTRANS_START_MSG_S *pstStartSendInfo, MTRANS_ReSTART_MSG_S *pstReStartInfo)
{
    HI_S32 s32Ret = HI_FAILURE;
    MTRANS_TASK_S *pTask = NULL;
    HI_U8   u8MeidaType = 0;
    
    /*check parameters*/
    if( NULL == pstStartSendInfo || NULL == pstReStartInfo)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }
    pTask = (MTRANS_TASK_S *)(pstStartSendInfo->u32TransTaskHandle);
    u8MeidaType = pstStartSendInfo->u8MeidaType;

    if(pTask->enTransMode == MTRANS_MODE_MULTICAST 
        && (pTask->enTaskState[MTRANS_STREAM_VIDEO] == TRANS_TASK_STATE_PLAYING
        || pTask->enTaskState[MTRANS_STREAM_AUDIO] == TRANS_TASK_STATE_PLAYING
        || pTask->enTaskState[MTRANS_STREAM_DATA] == TRANS_TASK_STATE_PLAYING))
    {
        pstReStartInfo->s32ErrCode = HI_SUCCESS;
        return HI_SUCCESS;
    }
    
    s32Ret = HI_MTRANS_TaskStart(pTask,u8MeidaType);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR, "start trans task failed %X.\n",
                     s32Ret);
        pstReStartInfo->s32ErrCode = s32Ret;
    }
    else
    {
      	pstReStartInfo->s32ErrCode = s32Ret;
    }

    return s32Ret;
}

HI_S32 HI_MTRANS_ResumeSendTask(MTRANS_RESUME_MSG_S *pstResumeInfo, MTRANS_RESUME_RespMSG_S *pstResumeResp)
{
    MTRANS_TASK_S *pTask = NULL;
    HI_U8   u8MeidaType = 0;
    
    /*check parameters*/
    if( NULL == pstResumeInfo || NULL == pstResumeResp)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }
    pTask = (MTRANS_TASK_S *)(pstResumeInfo->u32TransTaskHandle);
    u8MeidaType = pstResumeInfo->u8MeidaType;

    if(IS_MTRANS_REQ_TYPE(u8MeidaType,MTRANS_STREAM_VIDEO))
    {
        MTRANS_SetTaskState(pTask,MTRANS_STREAM_VIDEO,TRANS_TASK_STATE_PLAYING);
    }

    if(IS_MTRANS_REQ_TYPE(u8MeidaType,MTRANS_STREAM_AUDIO))
    {
        MTRANS_SetTaskState(pTask,MTRANS_STREAM_AUDIO,TRANS_TASK_STATE_PLAYING);
    }

    if(IS_MTRANS_REQ_TYPE(u8MeidaType,MTRANS_STREAM_DATA))
    {
        MTRANS_SetTaskState(pTask,MTRANS_STREAM_DATA,TRANS_TASK_STATE_PLAYING);
    }

    pstResumeResp->s32ErrCode = HI_SUCCESS;

    return HI_SUCCESS;
}


HI_S32 HI_MTRANS_StopSendTask(MTRANS_STOP_MSG_S *pstStopSendInfo,MTRANS_ReSTOP_MSG_S *pstReStopInfo)
{
    HI_S32 s32Ret = 0;
    MTRANS_TASK_S *pTask = NULL;
    HI_U8   u8MeidaType = 0;
    
    /*check parameters*/
    if( NULL == pstStopSendInfo || NULL == pstReStopInfo)
    {
        return HI_ERR_MTRANS_ILLEGAL_PARAM;
    }

    pTask = (MTRANS_TASK_S *)(pstStopSendInfo->u32TransTaskHandle);
    u8MeidaType = pstStopSendInfo->u8MeidaType;

    s32Ret = HI_MTRANS_TaskStop(pTask,u8MeidaType);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR, "stop trans task failed %X.\n",
                     s32Ret);
        pstReStopInfo->s32ErrCode = s32Ret;
    }
    else
    {
      pstReStopInfo->s32ErrCode = s32Ret;
    }

    return s32Ret;

}

/*获取首个发送数据的时间戳，并发送首个数据包*/
HI_U32 HI_MTRANS_GetBaseTimeStamp(HI_U32 u32TransTaskHandle,HI_U64 *u64BasePts)
{
    HI_S32 s32Ret = 0;
    MTRANS_TASK_S *pTaskHandle = NULL;
//    HI_BOOL bLoopFlag = HI_FALSE;
    MTRANS_TASK_STATE_E enVedioState = TRANS_TASK_STATE_BUTT;
    MTRANS_TASK_STATE_E enAudioState = TRANS_TASK_STATE_BUTT;
    MTRANS_TASK_STATE_E enDataState = TRANS_TASK_STATE_BUTT;
    //MBUF_HEADER_S *pMBuffHandle  = NULL;
    HI_VOID *pMBuffHandle  = NULL;
    HI_U8*    pBuffAddr     = NULL;          /*pointer to addr of stream ready to send */
    HI_U32     u32DataLen      = 0;          /*len of stream ready to send*/
    HI_U32     u32Pts          = 0;          /*stream pts*/
    HI_U64     u64CalcedPts    = 0;          /*换算过的pts,FU-A打包时使用*/
    MBUF_PT_E  enMbuffPayloadType = MBUF_PT_BUTT;    /*payload type*/
    HI_SOCKET s32WriteSock = -1;
    HI_U32   u32LastSn = 0;
    struct sockaddr_in* pPeerSockAddr = NULL;	
    struct sockaddr_in stPeerSockAddr;
    PACK_TYPE_E enPackType = PACK_TYPE_BUTT;
    RTP_PT_E enRtpPayloadType = RTP_PT_INVALID;
	HI_U32 u32Ssrc = 0;
    MTRANS_MODE_E      enTransMode = MTRANS_MODE_BUTT;
    HI_U8  slice_type = 0;
    HI_U32  slicenum = 0;
    HI_U32  frame_len = 0;

    HI_U32  u32FrameIndex = 0;
    HI_U32  u32FramePacketIndex = 0;
    HI_U32  u32VideoPacketIndex = 0;
    
    pTaskHandle = (MTRANS_TASK_S *)(u32TransTaskHandle);

    MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_VIDEO,&enVedioState);
    MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_AUDIO,&enAudioState);
    MTRANS_GetTaskState(pTaskHandle,MTRANS_STREAM_DATA,&enDataState); 
	 enTransMode = pTaskHandle->enTransMode;
    /* mtrans mode must be tcp interleaved or udp 
      otherwise, media data is not send by session thread*/
    if(MTRANS_MODE_TCP_ITLV != enTransMode
       && MTRANS_MODE_UDP != enTransMode)
    {
        return HI_SUCCESS;
    }


   /* 该接口必须在启动mtrans后调用，即至少一种媒体处在发送状态?
   否则返回失败*/
    if( (TRANS_TASK_STATE_PLAYING != enVedioState )
       &&( TRANS_TASK_STATE_PLAYING != enAudioState)
       &&( TRANS_TASK_STATE_PLAYING != enDataState))
	{
		HI_LOG_SYS(MTRANS_SEND, LOG_LEVEL_ERR, "not allowed to get basepts before start mtrans task.\n");
		return HI_ERR_MTRANS_UNSUPPORT_GETPTS;
	} 

    pMBuffHandle =(HI_VOID*)(pTaskHandle->u32MbuffHandle);
    
     /*获取码流数据直到I帧，并利用其PTS计算其basestamp*/
    do
    {
        /*request data from mediabuff */
        //s32Ret = MBUF_Read(pMBuffHandle, (HI_ADDR*)(&pBuffAddr), &u32DataLen, &u32Pts, &enMbuffPayloadType,&slice_type,&slicenum,&frame_len);
        s32Ret = HI_BufAbstract_Read((int)pTaskHandle->liveMode,pMBuffHandle, (HI_ADDR*)(&pBuffAddr), &u32DataLen, &u32Pts, &enMbuffPayloadType,&slice_type,&slicenum,&frame_len);
        /*there is no data ready */
        if(HI_SUCCESS != s32Ret)
        {
           printf("%s(%d) get base stamp:MBUF_Read read no data \n", __FUNCTION__, __LINE__);
           
           (HI_VOID)usleep(10000);
           continue;
        }
        
        /*如果不是I帧则丢弃*/
        if(VIDEO_KEY_FRAME != enMbuffPayloadType)
        {
            printf("not key video\n");
            HI_BufAbstract_Set((int)pTaskHandle->liveMode,pMBuffHandle);
            continue;
        }
        /*如果为I帧或其首个slice,利用其PTS计算其basestamp，并发送该数据*/
        else
        {
			/*分别计算音视频的时间戳*/
			if( VIDEO_KEY_FRAME == enMbuffPayloadType || VIDEO_NORMAL_FRAME == enMbuffPayloadType)
			{  
				printf("video come,orig pts =%u\n",u32Pts);
				u64CalcedPts = u32Pts * 90 / 1000 ;
				printf("video come,pro pts =%llu\n",u64CalcedPts);
			}
			else if( AUDIO_FRAME == enMbuffPayloadType)
			{  
				printf("audio come,orig pts =%u\n",u32Pts);
				u64CalcedPts = u32Pts * 8 / 1000;
				printf("audio come,pro pts =%llu\n",u64CalcedPts);
			}
		//	printf("DEBUG %s %d GetPacketAndSendParam \n",__FILE__,__LINE__); 
            s32Ret = MTRANS_GetPacketAndSendParam(pTaskHandle,enMbuffPayloadType,&s32WriteSock,
            					&stPeerSockAddr,&u32LastSn,&enPackType,&enRtpPayloadType,&u32Ssrc,
            					&u32FrameIndex,&u32FramePacketIndex,&u32VideoPacketIndex);
			if(HI_SUCCESS != s32Ret)
			{
				printf("get base stamp:MTRANS_GetPacketAndSendParam error %X \n",s32Ret);
				//MBUF_Set(pMBuffHandle);
				HI_BufAbstract_Set((int)pTaskHandle->liveMode,pMBuffHandle);
				return s32Ret;
			} 
			if(MTRANS_MODE_TCP_ITLV == enTransMode)
			{
				pPeerSockAddr = NULL;
			}
			else if(MTRANS_MODE_UDP == enTransMode)
			{
				pPeerSockAddr = &stPeerSockAddr;
			}
	
            if(PACK_TYPE_RTP_FUA == enPackType)
            {
            //	printf("DEBUG %s %d MTRANS_SendDataInRtpFUA\n",__FILE__,__LINE__);
                s32Ret = MTRANS_SendDataInRtpFUA(pTaskHandle,(char *)pBuffAddr,u32DataLen,u64CalcedPts,
                        enRtpPayloadType,&u32LastSn,u32Ssrc,s32WriteSock,pPeerSockAddr);
            }
            /*其它方式的打包方式:认为不需要切包发送
               目前，仅有hisi interleave打包方式*/
            else
            {
                s32Ret = MTRANS_SendDataInComm(pTaskHandle,enPackType,(char *)pBuffAddr,u32DataLen,u32Pts,
                        enRtpPayloadType,&u32LastSn,u32Ssrc,s32WriteSock,pPeerSockAddr);
            }

            if(HI_SUCCESS != s32Ret && MTRANS_SEND_IFRAME != s32Ret)
            {
				//printf("get base stamp:MBUF_Read read no data \n");
				//MBUF_Set(pMBuffHandle);
				HI_BufAbstract_Set((int)pTaskHandle->liveMode,pMBuffHandle);
				return s32Ret;
            }
            /*第一个slice发送成功，该循环结束*/
            else
            {
                //MBUF_Set(pMBuffHandle);
                HI_BufAbstract_Set((int)pTaskHandle->liveMode,pMBuffHandle);
                MTRANS_UpdateSeqNum(pTaskHandle,enMbuffPayloadType,
                                    pTaskHandle->enTransMode,u32LastSn);
		   		printf("get base time ok, now withdraw \n");			
                break;
            }
        }
    }while(1);

    //*u64BasePts = u32Pts;
    *u64BasePts = u64CalcedPts;
    return HI_SUCCESS;
    
}

int MTRANS_Register_StunServer(char *pServerAddr, unsigned short port)
{
    return BufAbstract_Register_StunServer(pServerAddr, port);
}

int MTRANS_Deregister_StunServer()
{
    return BufAbstract_Deregister_StunServer();
}

int MTRANS_Get_NatAddr(unsigned int TaskHandle,unsigned char stream, unsigned short MediaPort, unsigned short AdaptPort, char *pNatAddr, unsigned short *pMapedPortPair)
{
    int ret = 0;
    int socketpair[2] = {0};
    MTRANS_TASK_S *pTask = (MTRANS_TASK_S *)TaskHandle;
    ret = BufAbstract_Get_NatAddr(MediaPort, AdaptPort, pNatAddr, pMapedPortPair, socketpair);
    if(ret)
        return -1;

    pTask->struUdpTransInfo.as32SendSock[stream] = socketpair[0];
    pTask->struUdpTransInfo.as32RecvSock[stream] = socketpair[1];

    return 0;
}

int MTRANS_Check_Redirect(unsigned int TaskHandle, int IsRequestVideo, int IsRequestAudio)
{
    int ret = 0;
    struct timeval tv;
    fd_set fdSet; 
    MTRANS_TASK_S *pTask = (MTRANS_TASK_S *)TaskHandle;
    if(!TaskHandle)
        return -1;
    if(pTask->enTransMode != MTRANS_MODE_UDP)
        return -1;

    int count = 0;
    int fdmax;
    char buf[256];
    int recvlen = 0;
    struct sockaddr_in peer_addr;
    int videofd = pTask->struUdpTransInfo.as32SendSock[0];
    int audiofd = pTask->struUdpTransInfo.as32SendSock[1];
    int addr_len = sizeof(struct sockaddr_in);
    int v_recv = 0;
    int a_recv = 0;
    
    while(count++<8)
    {
        fdmax = 0;
        FD_ZERO(&fdSet);
        if(IsRequestVideo)
        {
            fdmax = videofd;
            FD_SET(videofd,&fdSet);
        }
        if(IsRequestAudio)
        {
            FD_SET(pTask->struUdpTransInfo.as32SendSock[1],&fdSet);
            fdmax = fdmax > audiofd ? fdmax : audiofd;
        }

        tv.tv_sec = 0;
        tv.tv_usec = 20000;
        ret = select(fdmax+1, &fdSet, NULL, NULL, &tv);
        if(ret == 0)
        {
            continue;
        }
        else if(ret<0)
        {
            printf("fun:%s  line:%d, select error,err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
            return -1;
        }

        if(IsRequestVideo && FD_ISSET(videofd, &fdSet))
        {
            recvlen = recvfrom(videofd, buf, 256, 0, (struct sockaddr*)&peer_addr, (socklen_t*)&addr_len);
            if(recvlen<=0)
            {
                printf("fun:%s  line:%d, recvfrom failed,err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
                return -1;
            }

            if(peer_addr.sin_addr.s_addr != pTask->struUdpTransInfo.struCliAddr[0].sin_addr.s_addr)
            {
                pTask->struUdpTransInfo.struCliAddr[0].sin_addr.s_addr = peer_addr.sin_addr.s_addr;
                printf("fun:%s,redirect video to\n",__FUNCTION__);
            }
            
            if(ntohs(peer_addr.sin_port) != pTask->struUdpTransInfo.cliMediaPort[0]
                && peer_addr.sin_addr.s_addr == pTask->struUdpTransInfo.struCliAddr[0].sin_addr.s_addr)
            {
                pTask->struUdpTransInfo.cliMediaPort[0] = ntohs(peer_addr.sin_port);
                pTask->struUdpTransInfo.struCliAddr[0].sin_port = peer_addr.sin_port; 
                printf("fun:%s,redirect video to port:%d\n",__FUNCTION__, (int)pTask->struUdpTransInfo.cliMediaPort[0]);
            }

            v_recv = 1;
        }

        if(IsRequestAudio && FD_ISSET(audiofd, &fdSet))
        {
            recvlen = recvfrom(audiofd, buf, 256, 0, (struct sockaddr*)&peer_addr, (socklen_t*)&addr_len);
            if(recvlen<=0)
            {
                printf("fun:%s  line:%d, recvfrom failed,err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
                return -1;
            }

            if(peer_addr.sin_addr.s_addr != pTask->struUdpTransInfo.struCliAddr[1].sin_addr.s_addr)
            {
                pTask->struUdpTransInfo.struCliAddr[1].sin_addr.s_addr = peer_addr.sin_addr.s_addr;
                printf("fun:%s,redirect audio to \n",__FUNCTION__);
            }

            if(ntohs(peer_addr.sin_port) != pTask->struUdpTransInfo.cliMediaPort[1])
            {
                pTask->struUdpTransInfo.cliMediaPort[1] = ntohs(peer_addr.sin_port);
                pTask->struUdpTransInfo.struCliAddr[1].sin_port = peer_addr.sin_port; 
                printf("fun:%s,redirect audio to port:%d\n",__FUNCTION__, (int)pTask->struUdpTransInfo.cliMediaPort[1]);
            }

            a_recv = 1;
        }

        if(IsRequestVideo && IsRequestAudio)
        {
            if(v_recv && a_recv)
                break;

            continue;
        }
        else if(v_recv || a_recv)
            break;
        
    }

    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
