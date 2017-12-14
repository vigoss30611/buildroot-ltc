/******************************************************************************

  Copyright (C), 2007-, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : 
  Version       : Initial Draft
  Author        : 
  Created       : 
  Description   : 
  History       :
  1.Date        : 
    Author      : 
    Modification: 
******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//#define MT_SYSLOG

#include <stdio.h>
#include <string.h>
#include "hi_type.h"
#include "hi_vod.h"
#include "hi_mt.h"
#include "hi_msession_rtsp.h"
#include "hi_rtsp_session.h"
#include "hi_buffer_abstract_ext.h"
#ifdef MT_SYSLOG
#include <syslog.h>
#endif

#include "cgiinterface.h"
#include "hi_msession_rtsp_o_http.h"

#define mt_version "v0.4build20090319-1732"

#ifdef MT_SYSLOG
#define himt_log(priority, args...) \
    do \
    {\
        syslog(priority, args);   \
        printf(args);                \
        printf("\n");                 \
    }while(0)
#else
#define himt_log(priority, args...) printf(args)
#endif

#define himt_dbg printf
#define himt_info printf


MT_CONFIG_S  g_MTConfig;
    
MT_CONFIG_S *g_pMTConfig = &g_MTConfig;

#define get_mtconfig_p() g_pMTConfig


#define MT_STATE_INIT 1
#define MT_STATE_RTSP_RUNNING 2
#define MT_STATE_RTSPoHTTP_RUNNING 4
//#define MT_STATE_HTTP_RUNNING 8

//static HI_S32 rtspSessioncount = 0;
static pthread_mutex_t rtspSessioncountLock;

int HI_MT_StopSvr_11();

volatile HI_S32 g_MTState = 0;

static HI_S32 MT_RTSPoHTTP_DistribLink(httpd_conn* hc);

int lib_rtsp_init(void * cfg)
{

	if (!cfg) return -1;
	MT_CONFIG_S * pMTConfig = (MT_CONFIG_S *)cfg;
    HI_S32 ret = -1;
    HI_S32 i=0;
    HI_S32 j=0;
	printf("libmt version: %s\n", mt_version);

	g_pMTConfig = pMTConfig;
#ifdef MT_SYSLOG    
	openlog("mt", LOG_NDELAY|LOG_PID, LOG_DAEMON);
#endif
	
	if(pMTConfig == NULL)
	{
		himt_dbg("<MT>init error. invalid args\n");
		return -1;
	}
	if( MT_STATE_INIT == (MT_STATE_INIT&g_MTState))
	{
		himt_log(LOG_ERR, "HI_MT_Init have init,init is repeate .\n");
		return -1;
	}

	if((pMTConfig->chn_cnt > VS_MAX_STREAM_CNT)||(pMTConfig->chn_cnt <= 0))
	{
		himt_log(LOG_ERR, "HI_MT_Init failed the param invalid the chn cnt is :%d .\n",pMTConfig->chn_cnt);
		return -1;
	}
	
	for(i=0;i<pMTConfig->chn_cnt;i++)
	{
		for(j = 0;j<pMTConfig->chn_cnt;j++ )
		{
			if(i != j)
			{
				if(pMTConfig->mbuf[i].chnid == pMTConfig->mbuf[j].chnid )
				{	 
					himt_log(LOG_ERR, "HI_MT_Init failed the mbuf %d 's chnid is the same as %d cnt:%d.\n",i,j,pMTConfig->chn_cnt);
					return -1;	
				}
			}
		}
	}
	
	printf("%s: g_pMTConfig->rtspsvr.listen_port=%d!\n",__FUNCTION__,pMTConfig->rtspsvr.listen_port);
    VOD_CONFIG_S vod_config;

    vod_config.u16UdpSendPortMin = pMTConfig->rtspsvr.udp_send_port_min;
    vod_config.u16UdpSendPortMax = pMTConfig->rtspsvr.udp_send_port_max;
    vod_config.u32PackPayloadLen = pMTConfig->packet_len;

    vod_config.enLiveMode = pMTConfig->enLiveMode;
    vod_config.maxFrameLen= pMTConfig->maxFrameLen;
    vod_config.maxUserNum = pMTConfig->maxUserNum;
  
    vod_config.s32SupportChnNum = pMTConfig->chn_cnt;
    memcpy(vod_config.stMbufConfig, pMTConfig->mbuf, 
                sizeof(MBUF_CHENNEL_INFO_S)*VS_MAX_STREAM_CNT);
  
    
    ret = HI_VOD_Init(&vod_config);
    if(ret == HI_SUCCESS)
    {
        himt_log(LOG_NOTICE, "Start HI_VOD_Init.vod Successed.\n");
    }
    else
    {
        himt_log(LOG_ERR, "Start MediaTrans.vod Failed:%#x.\n", ret);
        return ret;
    }
    g_MTState = g_MTState|MT_STATE_INIT;


//----------	-------------------------------------------------------

	if(g_pMTConfig->rtspsvr.bEnable)
    {
        if( MT_STATE_RTSP_RUNNING == (MT_STATE_RTSP_RUNNING&g_MTState))
	    {
	        himt_log(LOG_ERR, "the MT have start the rtsp server.repeate\n");
	        return 0;
	    }
        printf("%s: g_pMTConfig->rtspsvr.listen_port=%d!\n",__FUNCTION__,g_pMTConfig->rtspsvr.listen_port);
        
        
        g_MTState = g_MTState|MT_STATE_RTSP_RUNNING; 
    }
	(HI_VOID)pthread_mutex_init(&rtspSessioncountLock,  NULL);

	int s32Ret = HI_RTSP_SessionListInit(g_pMTConfig->rtspsvr.max_connections);
	if(HI_SUCCESS != s32Ret)
	{
		himt_log(LOG_ERR,
		        "RTSP LiveSvr Init Failed (Session Init Error=%d).\n",s32Ret);
	}
	return s32Ret;
}

void * rtsp_Session_Thread(void * cfg)
{
//	if( MT_STATE_INIT != (MT_STATE_INIT&g_MTState))
 //   {
 //       himt_log(LOG_ERR, "the MT haven't init,please first init the mt.\n");
 //       return NULL;
 //   }
//	int ret;
	NW_ClientInfo * clientinfo = (NW_ClientInfo*)cfg;
	if(clientinfo == NULL)
	{
		himt_dbg("<MT>rtspsessthread. invalid args\n");
		return NULL;
	}
	printf("start HI_RTSP_LiveSvr_Init_11\n");

	HI_RTSP_LiveSvr_Init_11(g_pMTConfig->rtspsvr.max_connections, 
                        g_pMTConfig->rtspsvr.listen_port,clientinfo); //u32MaxConnNum<0 有默认值

	clientinfo->OnLogoff(clientinfo->id);
    printf("end Session Thread!\n");                    
	return NULL;
}

int lib_rtsp_uninit()
{
/*	if( MT_STATE_INIT != (MT_STATE_INIT&g_MTState))
    {
        himt_log(LOG_ERR, "the MT have deinit.repeate\n");
        return -1;
    } 
    himt_log(LOG_NOTICE, "<MT>MediaTrans deinit.\n");

    HI_MT_StopSvr();*/
    
    g_MTState = 0;
    HI_VOD_DeInit();
    #ifdef MT_SYSLOG    
    closelog();
    #endif    
	return 0;
}

int lib_rtsp_stopsvr()
{
	printf("%s,%d stop rtsp svr\n",__FUNCTION__,__LINE__);
	HI_MT_StopSvr_11();
	g_MTState = 0;
	HI_VOD_DeInit();
#ifdef MT_SYSLOG	
    closelog();
#endif	 
	return 0;
	
}


int lib_rtspOhttpsvr_init(int MaxConn)
{
	printf("%s,%d start\n",__FUNCTION__,__LINE__);
	return HI_RTSP_O_HTTP_LiveSvr_Init(MaxConn);
}

int lib_rtspOhttpsvr_start(httpd_conn * hc)
{
	printf("%s,%d start\n",__FUNCTION__,__LINE__);
	return MT_RTSPoHTTP_DistribLink(hc);
}

int lib_rtspOhttpsvr_DeInit()
{
	printf("%s,%d start\n",__FUNCTION__,__LINE__);
	return HI_RTSP_O_HTTP_LiveSvr_DeInit();
}

int HI_MT_Init(MT_CONFIG_S *pMTConfig)
{
    HI_S32 ret = -1;
    HI_S32 i=0;
    HI_S32 j=0;
    
    printf("libmt version: %s\n", mt_version);
    
#ifdef MT_SYSLOG    
    openlog("mt", LOG_NDELAY|LOG_PID, LOG_DAEMON);
#endif
    
    if(pMTConfig == NULL)
    {
        himt_dbg("<MT>init error. invalid args\n");
        return -1;
    }
    if( MT_STATE_INIT == (MT_STATE_INIT&g_MTState))
    {
        himt_log(LOG_ERR, "HI_MT_Init have init,init is repeate .\n");
        return -1;
    }

    if((pMTConfig->chn_cnt > VS_MAX_STREAM_CNT)||(pMTConfig->chn_cnt <= 0))
    {
        himt_log(LOG_ERR, "HI_MT_Init failed the param invalid the chn cnt is :%d .\n",pMTConfig->chn_cnt);
        return -1;
    }
    
	for(i=0;i<pMTConfig->chn_cnt;i++)
    {
		for(j = 0;j<pMTConfig->chn_cnt;j++ )
		{
			if(i != j)
            {
                if(pMTConfig->mbuf[i].chnid == pMTConfig->mbuf[j].chnid )
                {    
					himt_log(LOG_ERR, "HI_MT_Init failed the mbuf %d 's chnid is the same as %d cnt:%d.\n",i,j,pMTConfig->chn_cnt);
					return -1;	
				}
			}
		}
    }
  
    memcpy(g_pMTConfig, pMTConfig, sizeof(MT_CONFIG_S));
    printf("%s: g_pMTConfig->rtspsvr.listen_port=%d!\n",__FUNCTION__,g_pMTConfig->rtspsvr.listen_port);
    VOD_CONFIG_S vod_config;

    vod_config.u16UdpSendPortMin = g_pMTConfig->rtspsvr.udp_send_port_min;
    vod_config.u16UdpSendPortMax = g_pMTConfig->rtspsvr.udp_send_port_max;
    vod_config.u32PackPayloadLen = g_pMTConfig->packet_len;

    vod_config.enLiveMode = g_pMTConfig->enLiveMode;
    vod_config.maxFrameLen= g_pMTConfig->maxFrameLen;
    vod_config.maxUserNum = g_pMTConfig->maxUserNum;
  
    vod_config.s32SupportChnNum = g_pMTConfig->chn_cnt;
    memcpy(vod_config.stMbufConfig, g_pMTConfig->mbuf, 
                sizeof(MBUF_CHENNEL_INFO_S)*VS_MAX_STREAM_CNT);
  
    
    ret = HI_VOD_Init(&vod_config);
    if(ret == HI_SUCCESS)
    {
        himt_log(LOG_NOTICE, "Start HI_VOD_Init.vod Successed.\n");
    }
    else
    {
        himt_log(LOG_ERR, "Start MediaTrans.vod Failed:%#x.\n", ret);
        return ret;
    }
    g_MTState = g_MTState|MT_STATE_INIT;
    
    return HI_SUCCESS;
    
}

//[2010-12-28 9:57:12]  媒体传输初始化，包含HTTP，RTSP，OWSP，RTSP_O_HTTP
int HI_MT_StartSvr()
{
    if( MT_STATE_INIT != (MT_STATE_INIT&g_MTState))
    {
        himt_log(LOG_ERR, "the MT haven't init,please first init the mt.\n");
        return -1;
    }
    
    HI_S32 ret;

    if(g_pMTConfig->rtspsvr.bEnable)
    {
        if( MT_STATE_RTSP_RUNNING == (MT_STATE_RTSP_RUNNING&g_MTState))
	    {
	        himt_log(LOG_ERR, "the MT have start the rtsp server.repeate\n");
	        return -1;
	    }
        printf("%s: g_pMTConfig->rtspsvr.listen_port=%d!\n",__FUNCTION__,g_pMTConfig->rtspsvr.listen_port);
        ret = HI_RTSP_LiveSvr_Init(g_pMTConfig->rtspsvr.max_connections, 
                        g_pMTConfig->rtspsvr.listen_port); //u32MaxConnNum<0 有默认值
        if(ret == HI_SUCCESS)
        {
            himt_log(LOG_ERR, "Start MediaTrans.rtsp_streamsvr Successed.\n");
        }
        else
        {
            himt_log(LOG_NOTICE, "Start MediaTrans.rtsp_streamsvr Failed: %#x.\n", ret);
            return ret;
        }        
        g_MTState = g_MTState|MT_STATE_RTSP_RUNNING; 
    }
    
    if(g_pMTConfig->rtspOhttpsvr.bEnable)
    {
        if( MT_STATE_RTSPoHTTP_RUNNING == (MT_STATE_RTSPoHTTP_RUNNING&g_MTState))
	    {
	        himt_log(LOG_ERR, "the MT have start the rtspOhttp server.repeate\n");
	        return -1;
	    }  
        #if 1
        ret = HI_RTSP_O_HTTP_LiveSvr_Init(g_pMTConfig->rtspOhttpsvr.max_connections);
        if(ret == HI_SUCCESS)
        {
            himt_log(LOG_ERR, "RtspOhttp init Successeful!.\n");
        }
        else
        {
            himt_log(LOG_NOTICE, "RtspOhttp init Failed: %#x.\n", ret);
            return ret;
        }      
        #endif
        
        HI_S_ICGI_Proc struProc;
    	memset(&struProc, 0, sizeof(HI_S_ICGI_Proc));
    	strcpy(struProc.pfunType,"rtsp");
    	struProc.pfunProc = MT_RTSPoHTTP_DistribLink;
#if 0
    	HI_ICGI_Register_Proc(&struProc);
#endif
        g_MTState = g_MTState|MT_STATE_RTSPoHTTP_RUNNING; 
    }
    
    return ret;

}

//BT todo . 移到mbuf中

int HI_MT_WriteBuf(
       HI_S32 ChanID, MBUF_PT_E payload_type, 
       const HI_ADDR addr, HI_U32 len, HI_U64 pts, 
       HI_U8 *pslice_type, HI_U32 slicenum, HI_U32 frame_len)
{
    if( MT_STATE_INIT != (MT_STATE_INIT&g_MTState))
    {
       // himt_log(LOG_ERR, "the MT haven't init,please first init the mt.\n");
        return -1;
    }
    HI_BufAbstract_Write(g_MTConfig.enLiveMode,ChanID, payload_type, addr, len, pts, pslice_type,slicenum, frame_len);
	return 0;
        
}

int HI_MT_StopSvr_11()
{
    HI_S32 ret;
	printf("HI_MT_StopSvr_11");
    if(g_pMTConfig->rtspsvr.bEnable)
    {
        if( MT_STATE_RTSP_RUNNING != (MT_STATE_RTSP_RUNNING&g_MTState))
	    {
	        himt_log(LOG_ERR, "the MT have stop the rtsp server.repeate\n");
	        return -1;
	    }  
		
        g_MTState = g_MTState^MT_STATE_RTSP_RUNNING;
		printf("%s,%d HI_RTSP_LiveSvr_DeInit_11 start \n",__FILE__,__LINE__);
        ret = HI_RTSP_LiveSvr_DeInit_11(); //u32MaxConnNum<0 有默认值
        if(ret == HI_SUCCESS)
        {
            himt_log(LOG_ERR, "<MT>Stop MediaTrans.rtsp_streamsvr Successed.\n");
        }
        else
        {
            himt_log(LOG_NOTICE, "<MT>Stop MediaTrans.rtsp_streamsvr Failed: %#x.\n", ret);
        }        
    }
	return 0;
}


int HI_MT_StopSvr()
{
    HI_S32 ret;

    if(g_pMTConfig->rtspsvr.bEnable)
    {
        if( MT_STATE_RTSP_RUNNING != (MT_STATE_RTSP_RUNNING&g_MTState))
	    {
	        himt_log(LOG_ERR, "the MT have stop the rtsp server.repeate\n");
	        return -1;
	    }  

        g_MTState = g_MTState^MT_STATE_RTSP_RUNNING;
        ret = HI_RTSP_LiveSvr_DeInit(); //u32MaxConnNum<0 有默认值
        if(ret == HI_SUCCESS)
        {
            himt_log(LOG_ERR, "<MT>Stop MediaTrans.rtsp_streamsvr Successed.\n");
        }
        else
        {
            himt_log(LOG_NOTICE, "<MT>Stop MediaTrans.rtsp_streamsvr Failed: %#x.\n", ret);
        }        
    }
    
    if(g_pMTConfig->rtspOhttpsvr.bEnable)
    {
        if( MT_STATE_RTSPoHTTP_RUNNING != (MT_STATE_RTSPoHTTP_RUNNING&g_MTState))
	    {
	        himt_log(LOG_ERR, "the MT have stop the rtsp server.repeate\n");
	        return -1;
	    }

        g_MTState = g_MTState^MT_STATE_RTSPoHTTP_RUNNING;
        
        ret = HI_RTSP_O_HTTP_LiveSvr_DeInit(); 
        if(ret == HI_SUCCESS)
        {
            himt_log(LOG_ERR, "<MT>Deinit rtspOhttp Successed.\n");
        }
        else
        {
            himt_log(LOG_NOTICE, "<MT>Deinit rtspOhttp Failed: %#x.\n", ret);
        }        
    }
    return 0;

}

int HI_MT_Deinit()
{
    if( MT_STATE_INIT != (MT_STATE_INIT&g_MTState))
    {
        himt_log(LOG_ERR, "the MT have deinit.repeate\n");
        return -1;
    } 
    himt_log(LOG_NOTICE, "<MT>MediaTrans deinit.\n");

    HI_MT_StopSvr();
    g_MTState = 0;
    HI_VOD_DeInit();
    #ifdef MT_SYSLOG    
    closelog();
    #endif    
    return 0;
}

int HI_MT_IsRunning()
{
    return HI_VOD_IsRunning();
}

static int MT_RTSPoHTTP_DistribLink(httpd_conn* hc)
{
    int ret;
	printf("\n");
    if((g_MTState&MT_STATE_RTSPoHTTP_RUNNING) == 0)
        return -1;
  //  printf("%s,%d MT_RTSPoHTTP_DistribLink step1 \n"__FILE__,__LINE__);
    ret = HI_RTSP_O_HTTP_DistribLink(hc);
    if(ret != HI_SUCCESS)
    {
        return -1;
    }

    hc->delay_close = 2000;
    hc->closeflag = 1;
    hc->permanent_connect = 1;
 //    printf("%s,%d MT_RTSPoHTTP_DistribLink step2 \n"__FILE__,__LINE__);
    return 2000;        //ms
}

int MT_StartMulticastStreaming(MT_MulticastInfo_s *pInfo)
{
    if(!pInfo)
        return HI_FAILURE;
    
    if(!(g_MTState&MT_STATE_INIT))
    {
        printf("%s  %d, The rtsp server has not inited.\n",__FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    return MTRANS_StartMulticastStreaming(pInfo);
}

int MT_StopMulticastStreaming(MT_MulticastInfo_s *pInfo)
{
    if(!pInfo)
        return HI_FAILURE;
    
    if(!(g_MTState&MT_STATE_INIT))
    {
        printf("%s  %d, The rtsp server has not inited.\n",__FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    return MTRANS_StopMulticastStreaming(pInfo);
}

int MT_Register_StunServer(char *pServerAddr, unsigned short port)
{
    if(!(g_MTState&MT_STATE_INIT))
        return -1;

    return MTRANS_Register_StunServer(pServerAddr, port);
}

int MT_Deregister_StunServer(void)
{
    return MTRANS_Deregister_StunServer();
}

int MT_Rtsp_DistribLink(int sockfd, char *pMsg, int msgLen)
{
    if(!(g_MTState&MT_STATE_INIT))
        return -1;

    return HI_RTSP_DistribLink(sockfd, pMsg, msgLen, HI_TRUE);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

