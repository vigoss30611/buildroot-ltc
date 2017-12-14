/******************************************************************************

  Copyright (C), 2007-, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_vod.c
  Version       : Initial Draft
  Author        : W54723
  Created       : 2008/02/15
  Description   : 
  History       :
  1.Date        : 
    Author      : 
    Modification: 
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hi_buffer_abstract_define.h"
#include "hi_buffer_abstract_ext.h"
#include "hi_vod.h"
#include "hi_vod_session.h"
#include "hi_vod_error.h"
//#include "hi_vs_enc.h"
//#include "hi_mp_enc.h"

//btlib use callback
//#include "hi_authmng.h"
//#include "hi_actiontype.h"

//btlib
//#include "hi_log_app.h"
//#include "hi_debug_def.h"

#include "ext_defs.h"
#include "hi_mt.h"

//#include "hi_vs_idm.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern MT_CONFIG_S *g_pMTConfig;            
VOD_SVR_S g_stVodSvr; /*vod server struct*/
#define VOD_VIDEO_BITMAP 0x01                 /*媒体类型为视频类型时为0000 0001 */
#define VOD_AUDIO_BITMAP 0x02                 /*媒体类型为音频类型时为0000 0011 */
#define VOD_DATA_BITMAP  0x04                 /*媒体类型为data类型时为0000 0100 */
//BT
/*
#define IS_VOD_VIDEO_TYPE(u8MediaType) ((0!= ((u8MediaType)&(VOD_VIDEO_BITMAP)) )? HI_TRUE:HI_FALSE)
#define IS_VOD_AUDIO_TYPE(u8MediaType) ((0!= ((u8MediaType)&(VOD_AUDIO_BITMAP)) )? HI_TRUE:HI_FALSE)
#define IS_VOD_DATA_TYPE(u8MediaType)  ((0!= ((u8MediaType)&(VOD_DATA_BITMAP)) )? HI_TRUE:HI_FALSE)
*/
#define IS_VOD_VIDEO_TYPE(u8MediaType) ((u8MediaType)&(VOD_VIDEO_BITMAP))
#define IS_VOD_AUDIO_TYPE(u8MediaType) ((u8MediaType)&(VOD_AUDIO_BITMAP))
#define IS_VOD_DATA_TYPE(u8MediaType)  ((u8MediaType)&(VOD_DATA_BITMAP)) 

extern HI_S32 BufAbstract_PlaybackByTime_GetMediaInfo(char *csReqInfo, MT_MEDIAINFO_S *pstMtMediaoInfo, char *csRespFileName, HI_U32*start_offset);
HI_VOID DEBUGPrtVODDes(VOD_DESCRIBE_MSG_S *stDesReqInfo,VOD_ReDESCRIBE_MSG_S *stDesRespInfo)
{
    //DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_ERROR," HI_VOD_DESCRIBE \n");
   #if 1
   DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_ERROR," HI_VOD_DESCRIBE \n"
                   "request:\n"
                    "au8MediaFile = %s\n"
                    "au8UserName= %s\n"
                    "au8PassWord= %s\n"
                    "au8ClientVersion= %s\n"
                    "response:\n"
                    "s32ErrCode= %X\n"
                    "u32MediaDurTime= %d\n"
                    "enAudioType= %d\n"
                    "u32AudioSampleRate= %d\n"
                    "u32AudioChannelNum= %d\n"
                    "enVideoType= %d\n"
                    "u32VideoPicWidth= %d\n"
                    "u32VideoPicHeight= %d\n"
                    
                    "u32Bitrate= %d\n"
                    "u32Framerate= %d\n"
                    "u32VideoSampleRate= %d\n"
                    "au8VideoDataInfo= %s\n"
                    "u32VideoDataLen= %d\n"
                    "au8BoardVersion= %s\n\n",
                    
                    stDesReqInfo->au8MediaFile,
                    stDesReqInfo->au8UserName,
                    stDesReqInfo->au8PassWord,
                    stDesReqInfo->au8ClientVersion,
                    stDesRespInfo->s32ErrCode,
                    stDesRespInfo->u32MediaDurTime,
                    stDesRespInfo->enAudioType,
                    stDesRespInfo->u32AudioSampleRate,
                    stDesRespInfo->u32AudioChannelNum,
                    stDesRespInfo->enVideoType,
                    stDesRespInfo->u32VideoPicWidth,
                    stDesRespInfo->u32VideoPicHeight,
                    
                    stDesRespInfo->u32Bitrate,
                    stDesRespInfo->u32Framerate,
                    stDesRespInfo->u32VideoSampleRate,
                    stDesRespInfo->au8VideoDataInfo,
                    stDesRespInfo->u32DataInfoLen,
                    stDesRespInfo->au8BoardVersion
                   );
       #endif             
}


HI_VOID DEBUGPrtVODSetup(VOD_SETUP_MSG_S *stVodSetupReqInfo, VOD_ReSETUP_MSG_S *stVodSetupRespInfo)
{
    HI_S32 i = 0;
   DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE," HI_VOD_SETUP \n"\
                   "request:\n"\
                    "enSessType = %d\n"\
                    "aszSessID= %s\n"\
                    "au8ClientIp= %s\n"\
                    "au8MediaFile= %s\n"\
                    "u32Ssrc= %d\n"\
                    "u8SetupMeidaType= %d\n"\
                    "au8UserName= %s\n"\
                    "au8PassWord= %s\n"\
                    "au8ClientVersion = %s\n"\
                    
                    "response:\n"\
                    "s32ErrCode = %X\n"\
                    "u8SetupMeidaType= %d\n"\
                    "enPackType= %d\n"\
                    "enMediaTransMode= %d\n"\
                    "au8BoardVersion = %s\n\n",
                    
                    stVodSetupReqInfo->enSessType,
                    stVodSetupReqInfo->aszSessID,
                    stVodSetupReqInfo->au8ClientIp,
                    stVodSetupReqInfo->au8MediaFile,
                    stVodSetupReqInfo->u32Ssrc,
                    stVodSetupReqInfo->u8SetupMeidaType,
                    stVodSetupReqInfo->au8UserName,
                    stVodSetupReqInfo->au8PassWord,
                    stVodSetupReqInfo->au8ClientVersion,
                    
                    stVodSetupRespInfo->s32ErrCode,
                    stVodSetupRespInfo->u8SetupMeidaType,
                    stVodSetupRespInfo->enPackType,
                    stVodSetupRespInfo->stTransInfo.enMediaTransMode,
                    stVodSetupRespInfo->au8BoardVersion
                   );
    for(i = 0;i <VOD_MAX_TRANSTYPE_NUM; i++)
    {
        if(HI_TRUE == stVodSetupReqInfo->astCliSupptTransList[i].bValidFlag)
        {
            if(MTRANS_MODE_TCP_ITLV == stVodSetupReqInfo->astCliSupptTransList[i].enMediaTransMode)
            {
                DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE," client support tcp itlv\n"\
                    "astCliSupptTransList[i].bValidFlag = %d\n"\
                    "astCliSupptTransList[i].enMediaTransMode = %d\n"\
                    "astCliSupptTransList[i].enPackType = %d\n"\
                    "astCliSupptTransList[i].stTcpItlvTransInfo.s32SessSock = %d\n"\
                    "astCliSupptTransList[i].stTcpItlvTransInfo.u32ItlvCliMediaChnId = %d\n"\
                    "astCliSupptTransList[i].stTcpItlvTransInfo.u32ItlvCliAdaptChnId = %d\n\n"\

                    "response:\n"\
                    "stTcpItlvTransInfo.u32SvrMediaChnId = %d\n"\
                    "stTcpItlvTransInfo.u32SvrAdaptChnId= %d\n"\
                    "stTcpItlvTransInfo.u32TransHandle = %X\n"\
                    "stTcpItlvTransInfo.pProcMediaTrans = %X\n\n",

                    stVodSetupReqInfo->astCliSupptTransList[i].bValidFlag,
                    stVodSetupReqInfo->astCliSupptTransList[i].enMediaTransMode,
                    stVodSetupReqInfo->astCliSupptTransList[i].enPackType,
                    stVodSetupReqInfo->astCliSupptTransList[i].stTcpItlvTransInfo.s32SessSock,
                    stVodSetupReqInfo->astCliSupptTransList[i].stTcpItlvTransInfo.u32ItlvCliMediaChnId,
                    stVodSetupReqInfo->astCliSupptTransList[i].stTcpItlvTransInfo.u32ItlvCliAdaptChnId,

                    stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32SvrMediaChnId,
                    stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32SvrAdaptChnId,
                    stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32TransHandle,
                    stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.pProcMediaTrans);
            }
        }
        else if(MTRANS_MODE_UDP == stVodSetupReqInfo->astCliSupptTransList[i].enMediaTransMode)
        {
            DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE," client support udp\n"\
                    "astCliSupptTransList[i].bValidFlag = %d\n"\
                    "astCliSupptTransList[i].enMediaTransMode = %d\n"\
                    "astCliSupptTransList[i].enPackType = %d\n"\
                    "astCliSupptTransList[i].stUdpTransInfo.u32CliMediaRecvPort = %d\n"\
                    "astCliSupptTransList[i].stUdpTransInfo.u32CliAdaptRecvPort = %d\n\n"\

                    "response:\n"\
                    "stUdpTransInfo.u32CliMediaRecvPort = %d\n"\
                    "stUdpTransInfo.u32CliAdaptRecvPort= %d\n"\
                    "stUdpTransInfo.u32SvrMediaSendPort = %d\n"\
                    "stUdpTransInfo.u32SvrAdaptSendPort= %d\n"\
                    "stUdpTransInfo.u32TransHandle = %X\n"\
                    "stUdpTransInfo.pProcMediaTrans = %X\n\n",

                    stVodSetupReqInfo->astCliSupptTransList[i].bValidFlag,
                    stVodSetupReqInfo->astCliSupptTransList[i].enMediaTransMode,
                    stVodSetupReqInfo->astCliSupptTransList[i].enPackType,
                    stVodSetupReqInfo->astCliSupptTransList[i].stUdpTransInfo.u32CliMediaRecvPort,
                    stVodSetupReqInfo->astCliSupptTransList[i].stUdpTransInfo.u32CliAdaptRecvPort,

                    stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliMediaRecvPort,
                    stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort,
                    stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrMediaSendPort,
                    stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort,
                    stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32TransHandle,
                    stVodSetupRespInfo->stTransInfo.stUdpTransInfo.pProcMediaTrans);
        }
    
    }
}

HI_VOID DEBUGPrtVODPlay(VOD_PLAY_MSG_S *stVodPlayReqInfo, VOD_RePLAY_MSG_S *stVodPlayRespInfo)
{

   DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE," HI_VOD_PLAY \n"
                   "request:\n"
                    "enSessType = %d\n"
                    "aszSessID= %s\n"
                    "u32StartTime= %d\n"
                    "u32Duration= %d\n"
                    "u8PlayMediaType= %d\n\n"
                    "response:\n"
                    "s32ErrCode = %X\n\n",
                    stVodPlayReqInfo->enSessType,
                    stVodPlayReqInfo->aszSessID,
                    stVodPlayReqInfo->u32StartTime,
                    stVodPlayReqInfo->u32Duration,
                    stVodPlayReqInfo->u8PlayMediaType,
                    stVodPlayRespInfo->s32ErrCode
                   );

}

HI_VOID DEBUGPrtVODPause(VOD_PAUSE_MSG_S*stVodPauseReqInfo, VOD_RePAUSE_MSG_S *stVodPauseRespInfo)
{

   DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE," HI_VOD_Pause \n"
                   "request:\n"
                    "enSessType = %d\n"
                    "aszSessID= %s\n"
                    "u32PauseTime= %d\n"
                    "u8PauseMediasType= %d\n"
                    "response:\n"
                    "s32ErrCode = %X\n\n",
                    stVodPauseReqInfo->enSessType,
                    stVodPauseReqInfo->aszSessID,
                    stVodPauseReqInfo->u32PauseTime,
                    stVodPauseReqInfo->u8PauseMediasType,
                    stVodPauseRespInfo->s32ErrCode
                   );

}

HI_VOID DEBUGPrtVODTeawdown(VOD_TEAWDOWN_MSG_S *stVodCloseReqInfo, VOD_ReTEAWDOWN_MSG_S *stVodCloseRespInfo)
{

   DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE," HI_VOD_TEAWDOWN \n"
                   "request:\n"
                    "enSessType = %d\n"
                    "aszSessID= %s\n"
                    "u32Reason= %s\n"
                    "response:\n"
                    "s32ErrCode = %X\n\n",
                    stVodCloseReqInfo->enSessType,
                    stVodCloseReqInfo->aszSessID,
                    stVodCloseReqInfo->u32Reason,
                    stVodCloseRespInfo->s32ErrCode
                   );

}

#ifdef OLDDIR
HI_S32 VODGetMaxChnNum(HI_S32 *ps32MaxChnNum)
{
    HI_S32 s32Ret = 0;
    MP_VIDEO_MODE_S struVideoMode;
    memset(&struVideoMode,0,sizeof(MP_VIDEO_MODE_S));

    *ps32MaxChnNum = 0;
        
    s32Ret = HI_MPMNG_GetVideoMode(&struVideoMode);
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    if(  (MODE_4CH_4CIF == struVideoMode.enVideoMode) || (MODE_4CH_CIF ==  struVideoMode.enVideoMode)
       ||(MODE_4CH_D1 == struVideoMode.enVideoMode) || (MODE_4CH_QCIF ==  struVideoMode.enVideoMode))
    {   
        *ps32MaxChnNum = 4;
    }
    else if (MODE_2CH_HD1 == struVideoMode.enVideoMode)
    {
        *ps32MaxChnNum = 2;
    }
    else
    {
        *ps32MaxChnNum = 1;
    }

    return HI_SUCCESS;
}
#endif

/*file name here may be a record file name or channel number
  judge it is a file or channel*/
HI_BOOL VODIsChnNum(HI_CHAR* pu8MediaFile,HI_S32 *ps32Chn)
{
    *ps32Chn = atoi(pu8MediaFile);
    printf("[VOD]asd vod of vschn %d \n",*ps32Chn); 
    if(*ps32Chn>0)
        return HI_TRUE;
    else
       return HI_FALSE; 
}

/*check the user and check he has right to vod or not
 if ok, return HI_TRUE
 if not ok, return HI_FALSE*/
#define VIEW_DEFAULT_USER_NAME "ipcam"
#define VIEW_DEFAULT_USER_PASSWORD "w54723"
HI_S32 VODVarifyUser( HI_CHAR* pszUserName, HI_CHAR* pszPassWord)
{
//    HI_S32 s32Ret = 0;
    if(NULL == pszUserName || NULL == pszPassWord)
    {
        return HI_FALSE;
    }
    
    if((0 == strcmp(pszUserName,VIEW_DEFAULT_USER_NAME)) && 
   	  (0 ==strcmp(pszPassWord,VIEW_DEFAULT_USER_PASSWORD))
   	 )
    {
        return HI_TRUE;
    }
    //这里调用DMS接口获取用户名开始比较
    	
    return HI_TRUE;

}

/*in all client supported transition type, chose one and return its number*/
HI_S32 VODChoseTransParam(VOD_CLI_SUPPORT_TRANS_INFO_S* pstCliSupptTransList,HI_U32 *pChosedNum)
{
    HI_U32 i = 0;

    /*here we always chose first effective transition */
    for(i=0;i<VOD_MAX_TRANSTYPE_NUM;i++)
    {
        /*to do:在此我们认为排在前面的为优先级高的，且目前仅支持tcp interleaved和udp方式*/
        if( HI_TRUE == pstCliSupptTransList[i].bValidFlag
           && (MTRANS_MODE_TCP_ITLV == pstCliSupptTransList[i].enMediaTransMode
               || MTRANS_MODE_UDP == pstCliSupptTransList[i].enMediaTransMode
               || MTRANS_MODE_MULTICAST == pstCliSupptTransList[i].enMediaTransMode
              )
           )
        {
            *pChosedNum = i;
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}
HI_S32 HI_VOD_GetVersion(HI_CHAR *pszSvrVersion, HI_U32 u32VersionLen)
{
    if(NULL == pszSvrVersion || 0 == u32VersionLen)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    strncpy(pszSvrVersion, "HiIpcam/V100R003 VodServer/1.0.0",u32VersionLen-1);
    pszSvrVersion[u32VersionLen-1] = '\0';

    return HI_SUCCESS;
}

HI_S32 HI_VOD_Init(VOD_CONFIG_S* pstVodConfig)
{
    HI_S32 s32Ret         = 0;
    HI_S32 s32ValidChnNum = 0;  /*valid vs channel number, get from 'ENC' model*/
    HI_S32 u32TotalUserNum = 0;
    HI_S32 i = 0;
    MBUF_CHENNEL_INFO_S *pstruMBUFInfo = NULL;
    MTRANS_INIT_CONFIG_S struMTransInfo;
	LIVE_1CHNnUSER_BUFINFO_S stru1ChnNUser;
    LIVE_1CHN1USER_BUFINFO_S stru1Chn1User;
    
    memset(&struMTransInfo,0,sizeof(MTRANS_INIT_CONFIG_S));
    
    if(NULL == pstVodConfig)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }
    
    /*if vod has already return ok*/
    if(VODSVR_STATE_Running == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_NOTICE,"VOD Init Failed: already inited.\n");
        return HI_SUCCESS;
    }
    
    /* clear vod server struct*/
    memset(&g_stVodSvr,0,sizeof(g_stVodSvr));
    g_stVodSvr.enSvrState = VODSVR_STATE_STOP;

    /*1 init vod svr*/
    /* 1.1 init supported channel number currently*/
    //BT NewDIR
    /*
    s32Ret = VODGetMaxChnNum(&s32ValidChnNum);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,
                    "VOD Init Failed (get channel number need to support failed=%d).\n",s32Ret);
        return s32Ret;
    }
    else
    {
        g_stVodSvr.s32MaxChnNum = s32ValidChnNum;
    }*/
    //END BT Newdir
    s32ValidChnNum = pstVodConfig->s32SupportChnNum;
    if(0 > s32ValidChnNum)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,
                    "VOD Init Failed (chn num error=%d).\n",s32ValidChnNum);
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    g_stVodSvr.s32MaxChnNum = s32ValidChnNum;

    /*计算vod支持的总用户数*/
    for(i= 0; i<s32ValidChnNum; i++)
    {
        u32TotalUserNum += pstVodConfig->stMbufConfig[i].max_connect_num;
    }
    printf("s32ValidChnNum:%d,u32TotalUserNum=%d!\n",s32ValidChnNum,u32TotalUserNum);
	/*3 init session list*/
	/*vod的最大支持用户数应等于mubff支持的各通道用户数之和*/
	s32Ret = HI_VOD_SessionListInit(u32TotalUserNum);
	if(HI_SUCCESS != s32Ret)
	{
		HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,
		        "VOD Init Failed (Session Init Error=%d).\n",s32Ret);
		return s32Ret;
	}

	/*2 init mbuff model*/
	if(pstVodConfig->enLiveMode == LIVE_MODE_1CHNnUSER)
	{
		pstruMBUFInfo = (MBUF_CHENNEL_INFO_S*)malloc(sizeof(MBUF_CHENNEL_INFO_S)*s32ValidChnNum);
		if(NULL == pstruMBUFInfo)
		{
			HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,
			"VOD Init Failed (malloc for muff info Error).\n");
			return HI_ERR_VOD_CALLOC_FAILED;
		}
		else
		{
			for(i= 0; i<s32ValidChnNum; i++)
			{
				pstruMBUFInfo[i].chnid = pstVodConfig->stMbufConfig[i].chnid;
				pstruMBUFInfo[i].max_connect_num = pstVodConfig->stMbufConfig[i].max_connect_num;
				pstruMBUFInfo[i].buf_size = pstVodConfig->stMbufConfig[i].buf_size;
				printf("num:%d  i:%d Mbuf Init cfg chnid %d , max connect num %d, bufsize %d\n ",i,s32ValidChnNum,pstruMBUFInfo[i].chnid,
						pstruMBUFInfo[i].max_connect_num,pstruMBUFInfo[i].buf_size);	
			}
		}
		stru1ChnNUser.pMbufCfg = pstruMBUFInfo;
		stru1ChnNUser.chnnum = s32ValidChnNum;
		s32Ret = HI_BufAbstract_Init(pstVodConfig->enLiveMode,(HI_VOID*)&stru1ChnNUser);
	}
	else if(pstVodConfig->enLiveMode == LIVE_MODE_1CHN1USER)
	{
		stru1Chn1User.maxUserNum = pstVodConfig->maxUserNum;
		stru1Chn1User.maxFrameLen = pstVodConfig->maxFrameLen;
		s32Ret = HI_BufAbstract_Init(pstVodConfig->enLiveMode,(HI_VOID*)&stru1Chn1User);
	}
	else
	{
		s32Ret = HI_FAILURE;
	}
     
    //s32Ret = MBUF_Init(s32ValidChnNum, pstruMBUFInfo);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD Init Failed (MBUF Init Error=%d).\n",s32Ret);
		free(pstruMBUFInfo);
		pstruMBUFInfo = NULL;
        return s32Ret;
    }

    /*max trans task number that MTRANS Model supported should equals to 
      max user number of VOD Model supported*/
    struMTransInfo.u32MaxTransTaskNum = u32TotalUserNum;
    struMTransInfo.u16MaxSendPort = pstVodConfig->u16UdpSendPortMax;
    struMTransInfo.u16MinSendPort = pstVodConfig->u16UdpSendPortMin;
    struMTransInfo.u32PackPayloadLen = pstVodConfig->u32PackPayloadLen;
    s32Ret = HI_MTRANS_Init(&struMTransInfo);
    if(HI_SUCCESS != s32Ret)
    {
		HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
		        "VOD Init Failed (MBUF Init Error=%d).\n",s32Ret);
		free(pstruMBUFInfo);
		pstruMBUFInfo = NULL;		
		return s32Ret;
    }
    
    /*5 init adapt model*/
    /*to do: 实现自适应模块后补充该部分*/

    //BTT:如果初始化失败需要释放前面的资?
	free(pstruMBUFInfo);
	pstruMBUFInfo = NULL;
	g_stVodSvr.enLiveMode = pstVodConfig->enLiveMode;
	g_stVodSvr.enSvrState = VODSVR_STATE_Running;
	DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_CRIT,"VOD Init success\n");

	return HI_SUCCESS;
}
/*vod deinit: 注意 vod deinit 前必需先将所有媒体会话服务停止完成,如rtsp媒体会话，http媒体会话
 1 deinit mtrans 
 2 deinit adapt model
 3 deinit mbuff
 4 free vod session list
 note: mtrans must deinited before mbuff.
 */
HI_S32 HI_VOD_DeInit()
{
    HI_S32 s32Ret = 0;
//    HI_S32 i = 0;
    /*if vod has deinit return ok*/
    if(VODSVR_STATE_STOP == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_NOTICE,"VOD deInit Failed: already deinited.\n");
        return HI_SUCCESS;
    }

    /*1 deinit mtrans*/
    s32Ret = HI_MTRANS_DeInit();
    if(HI_SUCCESS != s32Ret)
    {
        DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE,"VOD deInit Failed: HI_MTRANS_DeInit fail = %X\n",s32Ret);
    }

    /*2 deinit adapt model*/
    /*to do:自适应模块实现后补充该部分*/
    
    /*3 deinit mbuff*/
    //MBUF_DEInit();
	HI_BufAbstract_Deinit(g_stVodSvr.enLiveMode);
    /*4 free vod session list*/
    HI_VOD_SessionAllDestroy();
	
    #if 0 //comment 1107 N95
    /*added by w59413*/
    for (i = 0; i < MAX_CHN_NUM; i++)
    {
        if (HI_NULL != g_pau8SliceBuf[i])
        {
            free(g_pau8SliceBuf[i]);
            g_pau8SliceBuf[i] = HI_NULL;
        }
    }
    #endif
    g_stVodSvr.enSvrState = VODSVR_STATE_STOP;
    DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_CRIT,"VOD deInit success\n");
    return HI_SUCCESS;
}

HI_S32 HI_VOD_DESCRIBE(VOD_DESCRIBE_MSG_S *stDesReqInfo,VOD_ReDESCRIBE_MSG_S *stDesRespInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Chn = 0;
  //  HI_BOOL bIsChnFlag = HI_TRUE;
    MT_MEDIAINFO_S  stMediaInfo;
  //  MT_VIDEO_FORMAT_E  enVideoFormat = MT_VIDEO_FORMAT_BUTT;
  //  MT_AUDIO_FORMAT_E  enAudioFormat = MT_AUDIO_FORMAT_BUTT;

    memset(&stMediaInfo,0,sizeof(MT_MEDIAINFO_S));
    
    /*check parameters*/
    if(NULL == stDesReqInfo|| NULL == stDesRespInfo)
    {
    	
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*first set respons is error*/
    stDesRespInfo->s32ErrCode = HI_ERR_VOD_DESCRIBE_FAILED;

    /*user request real time stream of channel*/
    if(stDesReqInfo->bLive)
    {
        /*file name here may be a record file name or channel number
                judge it is a file or channel*/
        VODIsChnNum(stDesReqInfo->au8MediaFile,&s32Chn);
        s32Ret = HI_BufAbstract_GetMediaInfo(s32Chn,&stMediaInfo);

        /*vod real time stream, so set media duration time is 0*/
        stDesRespInfo->u32MediaDurTime = 0;
    }
    else
    {
        if(stDesReqInfo->s32PlayBackType == 0)
        {
            s32Ret = BufAbstract_Playback_GetMediaInfo(stDesReqInfo->au8MediaFile,&stMediaInfo);
        }
        else
        {
            s32Ret = BufAbstract_PlaybackByTime_GetMediaInfo(stDesReqInfo->au8MediaFile,
                &stMediaInfo, stDesRespInfo->RealFileName, &stDesRespInfo->start_offset);
        }
    }

    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD describe Failed : get chn %d info err %X\n",
                  s32Chn,s32Ret);  
		
        return s32Ret;
    }
    else
    {
        stDesRespInfo->enVideoType = stMediaInfo.enVideoType;
        stDesRespInfo->u32VideoPicWidth = stMediaInfo.u32VideoPicWidth;
        stDesRespInfo->u32VideoPicHeight = stMediaInfo.u32VideoPicHeight;
        stDesRespInfo->u32Bitrate = stMediaInfo.u32Bitrate;
        stDesRespInfo->u32Framerate = stMediaInfo.u32Framerate;
        stDesRespInfo->u32VideoSampleRate = stMediaInfo.u32VideoSampleRate;
        strncpy(stDesRespInfo->au8VideoDataInfo,stMediaInfo.auVideoDataInfo,
                stMediaInfo.u32VideoDataLen);
                
        stDesRespInfo->u32DataInfoLen = stMediaInfo.u32VideoDataLen;
        
        stDesRespInfo->enAudioType = stMediaInfo.enAudioType;
        stDesRespInfo->u32AudioSampleRate = stMediaInfo.u32AudioSampleRate;
        stDesRespInfo->u32AudioChannelNum = stMediaInfo.u32AudioChannelNum;
        memcpy(stDesRespInfo->profile_level_id,stMediaInfo.profile_level_id,sizeof(stDesRespInfo->profile_level_id));

        stDesRespInfo->u32TotalTime = stMediaInfo.u32TotalTime;
    }

    /*get server version*/
    s32Ret = HI_VOD_GetVersion(stDesRespInfo->au8BoardVersion, VOD_MAX_VERSION_LEN);
    if(HI_SUCCESS != s32Ret)
    {
    	
        return s32Ret;
    }

    /*if all step are ok, set response error code = 0*/
    stDesRespInfo->s32ErrCode = HI_SUCCESS;
    
    return HI_SUCCESS;
}

HI_S32 HI_VOD_SetUp(VOD_SETUP_MSG_S *stVodSetupReqInfo, VOD_ReSETUP_MSG_S *stVodSetupRespInfo)
{
    HI_S32 s32Ret = 0;
    VOD_SESSION_S *pSession = NULL;
    //HI_BOOL bIsChnFlag = HI_TRUE;
    HI_S32 s32Chn = 0;
    //MBUF_HEADER_S *pMbufHandle = NULL;
    HI_VOID *pMbufHandle = NULL;
    HI_U8 u8TempMediaType = 0;
    HI_U32 u32ChosedTransSeqNum = 0;      /*chosed sequence number of client transition list */
    MTRANS_SEND_CREATE_MSG_S    stMtransCreatInfo;     
    MTRANS_ReSEND_CREATE_MSG_S  stReMtransCreatInfo;
	HI_U32* pMtransHandle = NULL;
    MT_VIDEO_FORMAT_E enVideoFormat = MT_VIDEO_FORMAT_BUTT;
    MT_AUDIO_FORMAT_E enAudioFormat = MT_AUDIO_FORMAT_BUTT;
    HI_U32 u32CliUdpMediaRecvPort = 0;
    HI_U32 u32CliUdpAdaptRecvPort = 0;
    MT_MEDIAINFO_S  stMediaInfo;
	HI_U32 u32AudioSampleRate = 0;
    memset(&stMediaInfo,0,sizeof(MT_MEDIAINFO_S));
    memset(&stMtransCreatInfo,0,sizeof(MTRANS_SEND_CREATE_MSG_S));
    memset(&stReMtransCreatInfo,0,sizeof(MTRANS_ReSEND_CREATE_MSG_S));

    /*check parameters*/
    if(NULL == stVodSetupReqInfo|| NULL == stVodSetupRespInfo)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*first set respons is error*/
    stVodSetupRespInfo->s32ErrCode = HI_ERR_VOD_SETUP_FAILED;

    /*if vod has deinit return failed*/
    if(VODSVR_STATE_STOP == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD setup Failed: vod svr already deinited .\n");
        return HI_ERR_VOD_ALREADY_DEINIT;
    }

    /*1 varify username and password*/
    s32Ret = VODVarifyUser(stVodSetupReqInfo->au8UserName,stVodSetupReqInfo->au8PassWord);
    if(HI_TRUE != s32Ret )
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD setup Failed : unAuthorization  for user/pwd (%s/%s)error.\n", 
            stVodSetupReqInfo->au8UserName,stVodSetupReqInfo->au8PassWord);

        return HI_ERR_VOD_ILLEGAL_USER;
    }

    //mhua 
    HI_BOOL sessFlag = HI_FALSE;
    if(stVodSetupReqInfo->resv1 > 0)
    {
        printf("set up have aszSessID len is:%d!\n",strlen(stVodSetupReqInfo->aszSessID));
        s32Ret = HI_VOD_SessionFind(stVodSetupReqInfo->enSessType, stVodSetupReqInfo->aszSessID, &pSession);
        if(s32Ret == HI_SUCCESS)
        {
            sessFlag = HI_TRUE;
            pMtransHandle = (HI_U32*)pSession->u32MTransHandle;
        }
    }

    if(sessFlag != HI_TRUE)
    {
        /*2 creat a new vod session*/
        s32Ret = HI_VOD_SessionCreat(&pSession);
        if(HI_SUCCESS != s32Ret )
        {
            printf("the vod session create after failure!\n");
            return s32Ret;
        }
        else
        {
            /*init the session*/
            HI_VOD_SessionInit(pSession,stVodSetupReqInfo,HI_TRUE);
            printf("the vod session init after \n");
        }
    }
    else
    {
        printf("set up have before\n");
        pSession->enMediaType = pSession->enMediaType | stVodSetupReqInfo->u8SetupMeidaType;
    }

    /*file name here may be a record file name or channel number
      judge it is a file or channel*/
    //bIsChnFlag =VODIsChnNum(pSession->au8MediaFile,&s32Chn);

    if(stVodSetupReqInfo->bIsPlayBack == 0)
    {
        s32Chn = atoi(pSession->au8MediaFile);
    }
    
    /*user request real time stream of channel*/
    
    if(sessFlag != HI_TRUE)
    {
        /*get media foramt*/
        u8TempMediaType = stVodSetupReqInfo->u8SetupMeidaType;

        if(stVodSetupReqInfo->bIsPlayBack == 0)
            s32Ret = HI_BufAbstract_GetMediaInfo(s32Chn,&stMediaInfo);
        else
            s32Ret = BufAbstract_Playback_GetMediaInfo(stVodSetupReqInfo->au8MediaFile,&stMediaInfo);
        
        if(HI_SUCCESS != s32Ret)
        {
            //mhua add 
            s32Ret = HI_VOD_SessionDestroy(pSession);
            if(HI_SUCCESS != s32Ret)
            {
                HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                "HI_VOD_SessionDestroy cli (vod session destroy fail %X \n",s32Ret);
                //return HI_ERR_VOD_TEARDOWN_FAILED;
            }
            return s32Ret;
        }
        else
        {
            //if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
            //{
            enVideoFormat = stMediaInfo.enVideoType;
            enAudioFormat = stMediaInfo.enAudioType;
			u32AudioSampleRate = stMediaInfo.u32AudioSampleRate;
            HI_VOD_SessionSetVideoFormat(pSession,enVideoFormat);
            //}
            //if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
            //{
            HI_VOD_SessionSetAudioFormat(pSession,enAudioFormat);
            //}
        }
    }
    else
    {
        printf("vod setup handle have\n");
        pMbufHandle = (HI_VOID*)pSession->u32MbuffHandle;
#if 1 //ZQ 2009-06-10
        /*get media foramt*/
        u8TempMediaType = stVodSetupReqInfo->u8SetupMeidaType;

        if(stVodSetupReqInfo->bIsPlayBack == 0)
            s32Ret = HI_BufAbstract_GetMediaInfo(s32Chn,&stMediaInfo);
        else
            s32Ret = BufAbstract_Playback_GetMediaInfo(stVodSetupReqInfo->au8MediaFile,&stMediaInfo);
        
        if(HI_SUCCESS != s32Ret)
        {
            //mhua add 
            s32Ret = HI_VOD_SessionDestroy(pSession);
            if(HI_SUCCESS != s32Ret)
            {
                HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                "HI_VOD_SessionDestroy cli (vod session destroy fail %X \n",s32Ret);
                //return HI_ERR_VOD_TEARDOWN_FAILED;
            }
            return s32Ret;
        }
        else
        {
            //if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
            //{
            enVideoFormat = stMediaInfo.enVideoType;
            enAudioFormat = stMediaInfo.enAudioType;
			u32AudioSampleRate = stMediaInfo.u32AudioSampleRate;
            HI_VOD_SessionSetVideoFormat(pSession,enVideoFormat);
            //}
            //if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
            //{
            HI_VOD_SessionSetAudioFormat(pSession,enAudioFormat);
            //}
        }
#endif
    }
    /*5 chose one transition type in all of client supported transition set*/
    s32Ret = VODChoseTransParam(stVodSetupReqInfo->astCliSupptTransList,&u32ChosedTransSeqNum);
    if(HI_SUCCESS != s32Ret)
    {
        s32Ret = HI_VOD_SessionDestroy(pSession);
        if(HI_SUCCESS != s32Ret)
	    {
	        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
	                    "HI_VOD_SessionDestroy cli (vod session destroy fail %X \n",s32Ret);
	        //return HI_ERR_VOD_TEARDOWN_FAILED;
	    }
        
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                "there is no effective one in client supported transition list.\n");
        return HI_ERR_VOD_SETUP_FAILED;
    }
    else
    {
        DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE,
                "chose client supported transition info %d of list.\n",u32ChosedTransSeqNum);
    }
    
    /*6 creat transition task*/
    /*6.1 consist HI_MTRANS_CreatSendTask input parameters*/
    /*client ip*//*to do:超长怎么办?*/
    strncpy (stMtransCreatInfo.au8CliIP,stVodSetupReqInfo->au8ClientIp,strlen(stVodSetupReqInfo->au8ClientIp));

    /*mbuff handle*/
    stMtransCreatInfo.u32MediaHandle = (HI_U32)(pMbufHandle);

    /*media type*/
    stMtransCreatInfo.u8MeidaType = stVodSetupReqInfo->u8SetupMeidaType;

    /*media format :video and(or) audio format*/
    if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
    {
        stMtransCreatInfo.enVideoFormat = enVideoFormat;
    }
    if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
    {
        stMtransCreatInfo.enAudioFormat = enAudioFormat;
    }
	stMtransCreatInfo.AudioSampleRate = u32AudioSampleRate;
    /*packet type: equal to packet type of chosed transition node */
    stMtransCreatInfo.enPackType = stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].enPackType;

    /*media trans mode: equal to trans mode of chosed transition node */
    stMtransCreatInfo.enMediaTransMode = 
            stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].enMediaTransMode;
    stMtransCreatInfo.u32Ssrc = stVodSetupReqInfo->u32Ssrc;

    /*set transition parma according to different trans mode*/
    if(MTRANS_MODE_TCP_ITLV == stMtransCreatInfo.enMediaTransMode)
    {
        /*set mtrans task's  trans param: session socket */
        stMtransCreatInfo.stTcpItlvTransInfo.s32SessSock = 
                stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stTcpItlvTransInfo.s32SessSock;

         /*set mtrans task's  trans param: chn id of cli->svr's media data packet */
         stMtransCreatInfo.stTcpItlvTransInfo.u32ItlvCliMDataId = 
                stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stTcpItlvTransInfo.u32ItlvCliMediaChnId;
    }
    else if(MTRANS_MODE_UDP == stMtransCreatInfo.enMediaTransMode
        || MTRANS_MODE_MULTICAST == stMtransCreatInfo.enMediaTransMode)
    {
        u32CliUdpMediaRecvPort = stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stUdpTransInfo.u32CliMediaRecvPort;
        u32CliUdpAdaptRecvPort = stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stUdpTransInfo.u32CliAdaptRecvPort;

         /*set mtrans task's  trans param: client udp port which are ready to receive media data*/
         stMtransCreatInfo.stUdpTransInfo.u32CliMDataPort = u32CliUdpMediaRecvPort;
         stMtransCreatInfo.stUdpTransInfo.u32CliAdaptDataPort = u32CliUdpAdaptRecvPort;
    }
    else if(MTRANS_MODE_TCP == stMtransCreatInfo.enMediaTransMode)
    {
        /*to do:当实现tco传输时补充该部分*/
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support tcp mode now\n");
        return HI_ERR_VOD_UNRECOGNIZED_TRANS_MODE;
        
    }
    else if(MTRANS_MODE_UDP_ITLV == stMtransCreatInfo.enMediaTransMode)
    {
        /*to do:当实现udp interleaved传输时补充该部分*/
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support udp interleaved mode now\n");
        return HI_ERR_VOD_UNRECOGNIZED_TRANS_MODE;
    }
    else 
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"not unrecognized trans mode %d\n",
                   stMtransCreatInfo.enMediaTransMode);
        return HI_ERR_VOD_UNRECOGNIZED_TRANS_MODE;
    }
    stMtransCreatInfo.enLiveMode = g_stVodSvr.enLiveMode;

    stMtransCreatInfo.resv1 = stMediaInfo.u32Framerate;
    stMtransCreatInfo.SPS_LEN = stMediaInfo.SPS_LEN;
    stMtransCreatInfo.PPS_LEN = stMediaInfo.PPS_LEN;
    stMtransCreatInfo.SEL_LEN = stMediaInfo.SEL_LEN;

    strcpy(stMtransCreatInfo.au8MediaFile, stVodSetupReqInfo->au8MediaFile);
	
	//mhua change
    /*6.2 creat mtrans task*/
    
    s32Ret = HI_MTRANS_CreatSendTask(pMtransHandle,&stMtransCreatInfo,&stReMtransCreatInfo);
    
    DEBUGPrtMtransCreat(&stMtransCreatInfo,&stReMtransCreatInfo);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                "VOD setup Failed for cli %s (creat trans task Error=%X).\n",stVodSetupReqInfo->au8ClientIp,s32Ret);
        return s32Ret;
    }
    else
    {
        /*record trans handle in vod session*/
        HI_VOD_SessionSetMtransHdl(pSession, stReMtransCreatInfo.u32TransTaskHandle);
    }

    if(MTRANS_MODE_MULTICAST == stMtransCreatInfo.enMediaTransMode 
        && stReMtransCreatInfo.stMulticastTransInfo.u32MediaHandle)
    {
        printf("%s  %d, #########,%d\n",__FUNCTION__, __LINE__,stReMtransCreatInfo.stMulticastTransInfo.u32MediaHandle);
        HI_VOD_SessionSetMbufHdl(pSession, stReMtransCreatInfo.stMulticastTransInfo.u32MediaHandle);
    }
    else if((MTRANS_MODE_MULTICAST != stMtransCreatInfo.enMediaTransMode 
        && sessFlag != HI_TRUE) 
        || (MTRANS_MODE_MULTICAST == stMtransCreatInfo.enMediaTransMode 
        && stReMtransCreatInfo.stMulticastTransInfo.u32MediaHandle == 0))
    {
        /*3 alloc a mbuff for the session*/
        if(stVodSetupReqInfo->bIsPlayBack == 0)
            s32Ret = HI_BufAbstract_Alloc(g_stVodSvr.enLiveMode, s32Chn,&pMbufHandle);
        else
            s32Ret = BufAbstract_Playback_Alloc(g_stVodSvr.enLiveMode, pSession->au8MediaFile,&pMbufHandle);
        
        if(HI_SUCCESS != s32Ret)
        {
            //mhua add 
            s32Ret = HI_VOD_SessionDestroy(pSession);
            if(HI_SUCCESS != s32Ret)
            {
                HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                "HI_VOD_SessionDestroy cli (vod session destroy fail %X \n",s32Ret);
                //return HI_ERR_VOD_TEARDOWN_FAILED;
            }
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
            "VOD setup Failed for cli %s (MBUF alloc Error=%d).\n",stVodSetupReqInfo->au8ClientIp,s32Ret);
            return HI_ERR_VOD_SETUP_FAILED;
        }
        else
        {
            /*record mbuff handle in vod session*/
            HI_MTRANS_SetMbufHandle(pSession->u32MTransHandle, (HI_U32)(pMbufHandle));
            HI_VOD_SessionSetMbufHdl(pSession,(HI_U32)(pMbufHandle));
            DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE,"VOD setup for cli %s (MBUF alloc ok:handle = %X).\n",
                stVodSetupReqInfo->au8ClientIp,pMbufHandle);
        }
    }

    /*7 creat adapter task*/
    /*to do:自适应模块实现后补充该部分*/

    /*8 update vod meida state as ready*/
    u8TempMediaType = stVodSetupReqInfo->u8SetupMeidaType;
    if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_VIDEO, VOD_SESS_STATE_READY);
    }
    if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_AUDIO, VOD_SESS_STATE_READY);
    }
    if(IS_VOD_DATA_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_DATA, VOD_SESS_STATE_READY);
    }

    /*9 consist HI_VOD_SetUp output param*/
    /*consist output param: media type*/
    stVodSetupRespInfo->u8SetupMeidaType = stMtransCreatInfo.u8MeidaType;
    /*consist output param: packet type*/
    stVodSetupRespInfo->enPackType = stMtransCreatInfo.enPackType;
    /*consist output param: trans mode*/
    stVodSetupRespInfo->stTransInfo.enMediaTransMode = stReMtransCreatInfo.enMediaTransMode;
    /*set vod output param :transition parma according to different trans mode*/
    if(MTRANS_MODE_TCP_ITLV == stReMtransCreatInfo.enMediaTransMode)
    {
        /*set trans param:chn id of svr->cli's media  */
        stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32SvrMediaChnId = 
                                stReMtransCreatInfo.stTcpItlvTransInfo.u32SvrMediaDataId;
         /*set trans param: function handle which process sending interleaved media data  */
         stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.pProcMediaTrans  = 
                                stReMtransCreatInfo.stTcpItlvTransInfo.pProcMediaTrans;

         /*set trans param:chn id of svr->cli's adapt data  */
         /*to do:设置自适应调控数据的chn id,应由自适应模块提供，暂时设置为数据id+1
           自适应模块实现后去掉该句话*/
         stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32SvrAdaptChnId = 
                                stReMtransCreatInfo.stTcpItlvTransInfo.u32SvrMediaDataId+1;
         /*notify mtrans handle*/
         stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32TransHandle =
                                stReMtransCreatInfo.u32TransTaskHandle;
    }
    else if(MTRANS_MODE_UDP == stMtransCreatInfo.enMediaTransMode)
    {
        /*设置client的数据和流控包的接收端口: 等同于用户请求中带的端口号 */
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliMediaRecvPort = u32CliUdpMediaRecvPort;
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort = u32CliUdpAdaptRecvPort;

        /*设置server的数据和流控包的发送端口 */
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrMediaSendPort= 
                                stReMtransCreatInfo.stUdpTransInfo.u32SvrMediaSendPort;
        /*set trans param:chn id of svr->cli's adapt data  */
         /*to do:设置自适应调控数据的recv port,应由自适应模块提供，暂时设置为数据id+1
           自适应模块实现后去掉该句话*/                        
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort= 
                                stReMtransCreatInfo.stUdpTransInfo.u32SvrMediaSendPort + 1;                        

         /*set trans param: function handle which process sending media data  */
         stVodSetupRespInfo->stTransInfo.stUdpTransInfo.pProcMediaTrans  = 
                                stReMtransCreatInfo.stUdpTransInfo.pProcMediaTrans;

         /*notify mtrans handle*/
         stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32TransHandle =
                                stReMtransCreatInfo.u32TransTaskHandle;
    }
    else if(MTRANS_MODE_MULTICAST == stMtransCreatInfo.enMediaTransMode)
    {
        /*设置client的数据和流控包的接收端口: 等同于用户请求中带的端口号 */
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliMediaRecvPort = 
            stReMtransCreatInfo.stMulticastTransInfo.cliMediaPort;
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort = 
            stReMtransCreatInfo.stMulticastTransInfo.cliMediaPort+1;

        /*设置server的数据和流控包的发送端口 */
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrMediaSendPort= 
                                stReMtransCreatInfo.stMulticastTransInfo.svrMediaPort;
        /*set trans param:chn id of svr->cli's adapt data  */
         /*to do:设置自适应调控数据的recv port,应由自适应模块提供，暂时设置为数据id+1
           自适应模块实现后去掉该句话*/                        
        stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort= 
                                stReMtransCreatInfo.stMulticastTransInfo.svrMediaPort + 1;                        

         stVodSetupRespInfo->stTransInfo.stUdpTransInfo.pProcMediaTrans  = 
                stReMtransCreatInfo.stUdpTransInfo.pProcMediaTrans;

         /*notify mtrans handle*/
         stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32TransHandle =
                                stReMtransCreatInfo.u32TransTaskHandle;
    }
    else if(MTRANS_MODE_TCP == stMtransCreatInfo.enMediaTransMode)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support tcp interleaved mode now\n");
        /*to do:当实现tco传输时补充该部分*/
    }
    else if(MTRANS_MODE_UDP_ITLV == stMtransCreatInfo.enMediaTransMode)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support udp interleaved mode now\n");
        /*to do:当实现udp interleaved传输时补充该部分*/
    }

    /*get server version*/
    s32Ret = HI_VOD_GetVersion(stVodSetupRespInfo->au8BoardVersion, VOD_MAX_VERSION_LEN);
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    stVodSetupRespInfo->s32ErrCode = HI_SUCCESS;
    return HI_SUCCESS;
}

#if 0
HI_S32 HI_VOD_SetUp_Add(VOD_SETUP_MSG_S *stVodSetupReqInfo, VOD_ReSETUP_MSG_S *stVodSetupRespInfo)
{
    HI_S32 s32Ret = 0;
    VOD_SESSION_S *pSession = NULL;
    HI_BOOL bIsChnFlag = HI_TRUE;
    HI_S32 s32Chn = 0;
    HI_U32 u32MTransHandle = 0;
    HI_U32 u32MBufHandle = 0;
    HI_U8 u8TempMediaType = 0;
    HI_U32 u32ChosedTransSeqNum = 0;      /*chosed sequence number of client transition list */
    MTRANS_SEND_CREATE_MSG_S    stMtransCreatInfo;     
    MTRANS_ReSEND_CREATE_MSG_S  stReMtransCreatInfo;
    HI_U32 u32CliUdpMediaRecvPort = 0;
    HI_U32 u32CliUdpAdaptRecvPort = 0;
    HI_BOOL bTempVarifyFlag = HI_FALSE;
    
    memset(&stMtransCreatInfo,0,sizeof(MTRANS_SEND_CREATE_MSG_S));
    memset(&stReMtransCreatInfo,0,sizeof(MTRANS_ReSEND_CREATE_MSG_S));
    
    /*check parameters*/
    if(NULL == stVodSetupReqInfo|| NULL == stVodSetupRespInfo)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*first set respons is error*/
    stVodSetupRespInfo->s32ErrCode = HI_ERR_VOD_SETUP_FAILED;

    /*if vod has deinit return failed*/
    if(VODSVR_STATE_STOP == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD setup Failed: vod svr already deinited .\n");
        return HI_ERR_VOD_ALREADY_DEINIT;
    }

    /*2 get session handle according to protocal type+session id*/
    s32Ret = HI_VOD_SessionFind(stVodSetupReqInfo->enSessType,stVodSetupReqInfo->aszSessID,
                                  &pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD setup_add Failed for cli (can't find session :protocal %d, session id %s \n",
                    stVodSetupReqInfo->enSessType,stVodSetupReqInfo->aszSessID);
        return HI_ERR_VOD_PLAY_FAILED;
    }

    /*1 varify username and password*/
    s32Ret = HI_VOD_SessionGetVarifyFlag(pSession,&bTempVarifyFlag);
    printf("ret = %X bTempVarifyFlag=%d\n",s32Ret,bTempVarifyFlag);
    if(HI_SUCCESS != s32Ret || HI_TRUE != bTempVarifyFlag)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD setup_add Failed for cli (user has no right to setup add :protocal %d, session id %s \n",
                    stVodSetupReqInfo->enSessType,stVodSetupReqInfo->aszSessID);
        return HI_ERR_VOD_ILLEGAL_USER;
    }

    s32Ret = HI_VOD_SessionGetMbufHdl(pSession, &u32MBufHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD setup_add Failed for cli (get mbuf handle from session failed %X \n",s32Ret);
        return HI_ERR_VOD_PLAY_FAILED;
    }

    s32Ret = HI_VOD_SessionGetMtransHdl(pSession, &u32MTransHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD setup_add Failed for cli (get mtrans handle from session failed %X \n",s32Ret);
        return HI_ERR_VOD_PLAY_FAILED;
    }

    /*file name here may be a record file name or channel number
      judge it is a file or channel*/
    bIsChnFlag =VODIsChnNum(pSession->au8MediaFile,&s32Chn);

    /*user request real time stream of channel*/
    if(HI_TRUE == bIsChnFlag )
    {
        /*get media foramt*/
        u8TempMediaType = stVodSetupReqInfo->u8SetupMeidaType;

        /*5 chose one transition type in all of client supported transition set*/
        s32Ret = VODChoseTransParam(stVodSetupReqInfo->astCliSupptTransList,&u32ChosedTransSeqNum);
        if(HI_SUCCESS != s32Ret)
        {
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "there is no effective one in client supported transition list.\n");
            return HI_ERR_VOD_SETUP_FAILED;
        }
        else
        {
            DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_NOTICE,
                    "chose client supported transition info %d of list.\n",u32ChosedTransSeqNum);
          
        }
        
        /*6 creat transition task*/
        /*6.1 consist HI_MTRANS_CreatSendTask input parameters*/
        /*client ip*//*to do:超长怎么办?*/
        strncpy (stMtransCreatInfo.au8CliIP,stVodSetupReqInfo->au8ClientIp,strlen(stVodSetupReqInfo->au8ClientIp));

        /*mbuff handle*/
        stMtransCreatInfo.u32MediaHandle = u32MBufHandle;

        /*media type*/
        stMtransCreatInfo.u8MeidaType = stVodSetupReqInfo->u8SetupMeidaType;
		stMtransCreatInfo.u32Ssrc = stVodSetupReqInfo->u32Ssrc;
		
        /*media format :video and(or) audio format*/
        //if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
        {
            stMtransCreatInfo.enVideoFormat = pSession->enVideoFormat;
        }
        //if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
        {
            stMtransCreatInfo.enAudioFormat = pSession->enAudioFormat;
        }

        /*packet type: equal to packet type of chosed transition node */
        stMtransCreatInfo.enPackType = stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].enPackType;

        /*media trans mode: equal to trans mode of chosed transition node */
        stMtransCreatInfo.enMediaTransMode = 
                stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].enMediaTransMode;
        /*set transition parma according to different trans mode*/
        if(MTRANS_MODE_TCP_ITLV == stMtransCreatInfo.enMediaTransMode)
        {
            /*set mtrans task's  trans param: session socket */
            stMtransCreatInfo.stTcpItlvTransInfo.s32SessSock = 
                    stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stTcpItlvTransInfo.s32SessSock;

             /*set mtrans task's  trans param: chn id of cli->svr's media data packet */
             stMtransCreatInfo.stTcpItlvTransInfo.u32ItlvCliMDataId = 
                    stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stTcpItlvTransInfo.u32ItlvCliMediaChnId;
        }
        else if(MTRANS_MODE_UDP == stMtransCreatInfo.enMediaTransMode)
        {
            u32CliUdpMediaRecvPort = stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stUdpTransInfo.u32CliMediaRecvPort;
            u32CliUdpAdaptRecvPort = stVodSetupReqInfo->astCliSupptTransList[u32ChosedTransSeqNum].stUdpTransInfo.u32CliAdaptRecvPort;

             /*set mtrans task's  trans param: client udp port which are ready to receive media data*/
             stMtransCreatInfo.stUdpTransInfo.u32CliMDataPort = u32CliUdpMediaRecvPort;
        }
        else if(MTRANS_MODE_TCP == stMtransCreatInfo.enMediaTransMode)
        {
            /*to do:当实现tco传输时补充该部分*/
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support tcp mode now\n");
            return HI_ERR_VOD_UNRECOGNIZED_TRANS_MODE;
            
        }
        else if(MTRANS_MODE_UDP_ITLV == stMtransCreatInfo.enMediaTransMode)
        {
            /*to do:当实现udp interleaved传输时补充该部分*/
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support udp interleaved mode now\n");
            return HI_ERR_VOD_UNRECOGNIZED_TRANS_MODE;
        }
        else 
        {
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"not unrecognized trans mode %d\n",
                       stMtransCreatInfo.enMediaTransMode);
            return HI_ERR_VOD_UNRECOGNIZED_TRANS_MODE;
        }

        /*6.2 creat mtrans task*/
        s32Ret = HI_MTRANS_Add2CreatedTask(u32MTransHandle,&stMtransCreatInfo,&stReMtransCreatInfo);
        DEBUGPrtMtransCreat(&stMtransCreatInfo,&stReMtransCreatInfo);
        if(HI_SUCCESS != s32Ret)
        {
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD setup Failed for cli %s (creat trans task Error=%X).\n",stVodSetupReqInfo->au8ClientIp,s32Ret);
            return s32Ret;
        }

        /*7 creat adapter task*/
        /*to do:自适应模块实现后补充该部分*/

        /*8 update vod meida state as ready*/
        u8TempMediaType = stVodSetupReqInfo->u8SetupMeidaType;
        if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
        {
            HI_VOD_SessionSetState(pSession, VOD_STREAM_VIDEO, VOD_SESS_STATE_READY);
        }
        if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
        {
            HI_VOD_SessionSetState(pSession, VOD_STREAM_AUDIO, VOD_SESS_STATE_READY);
        }
        if(IS_VOD_DATA_TYPE(u8TempMediaType))
        {
            HI_VOD_SessionSetState(pSession, VOD_STREAM_DATA, VOD_SESS_STATE_READY);
        }

        /*9 consist HI_VOD_SetUp output param*/
        /*consist output param: media type*/
        stVodSetupRespInfo->u8SetupMeidaType = stMtransCreatInfo.u8MeidaType;
        /*consist output param: packet type*/
        stVodSetupRespInfo->enPackType = stMtransCreatInfo.enPackType;
        /*consist output param: trans mode*/
        stVodSetupRespInfo->stTransInfo.enMediaTransMode = stReMtransCreatInfo.enMediaTransMode;
        /*set vod output param :transition parma according to different trans mode*/
        if(MTRANS_MODE_TCP_ITLV == stReMtransCreatInfo.enMediaTransMode)
        {
            /*set trans param:chn id of svr->cli's media  */
            stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32SvrMediaChnId = 
                                    stReMtransCreatInfo.stTcpItlvTransInfo.u32SvrMediaDataId;
             /*set trans param: function handle which process sending interleaved media data  */
             stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.pProcMediaTrans  = 
                                    stReMtransCreatInfo.stTcpItlvTransInfo.pProcMediaTrans;

             /*set trans param:chn id of svr->cli's adapt data  */
             /*to do:设置自适应调控数据的chn id,应由自适应模块提供，暂时设置为数据id+1
               自适应模块实现后去掉该句话*/
             stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32SvrAdaptChnId = 
                                    stReMtransCreatInfo.stTcpItlvTransInfo.u32SvrMediaDataId+1;
             /*notify mtrans handle*/
             stVodSetupRespInfo->stTransInfo.stTcpItlvTransInfo.u32TransHandle =
                                    stReMtransCreatInfo.u32TransTaskHandle;
                                                  
        }
        else if(MTRANS_MODE_UDP == stMtransCreatInfo.enMediaTransMode)
        {
            /*设置client的数据和流控包的接收端口: 等同于用户请求中带的端口号 */
            stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliMediaRecvPort = u32CliUdpMediaRecvPort;
            stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32CliAdaptRecvPort = u32CliUdpAdaptRecvPort;

            /*设置server的数据和流控包的发送端口 */
            stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrMediaSendPort= 
                                    stReMtransCreatInfo.stUdpTransInfo.u32SvrMediaSendPort;
            /*set trans param:chn id of svr->cli's adapt data  */
             /*to do:设置自适应调控数据的recv port,应由自适应模块提供，暂时设置为数据id+1
               自适应模块实现后去掉该句话*/                        
            stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32SvrAdaptSendPort= 
                                    stReMtransCreatInfo.stUdpTransInfo.u32SvrMediaSendPort + 1;                        

             /*set trans param: function handle which process sending media data  */
             stVodSetupRespInfo->stTransInfo.stUdpTransInfo.pProcMediaTrans  = 
                                    stReMtransCreatInfo.stUdpTransInfo.pProcMediaTrans;

             /*notify mtrans handle*/
             stVodSetupRespInfo->stTransInfo.stUdpTransInfo.u32TransHandle =
                                    stReMtransCreatInfo.u32TransTaskHandle;
        }
        else if(MTRANS_MODE_TCP == stMtransCreatInfo.enMediaTransMode)
        {

            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support tcp interleaved mode now\n");
            /*to do:当实现tco传输时补充该部分*/
        }
        else if(MTRANS_MODE_UDP_ITLV == stMtransCreatInfo.enMediaTransMode)
        {
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERROR,"we not support udp interleaved mode now\n");
            /*to do:当实现udp interleaved传输时补充该部分*/
        }

        /*get server version*/
        s32Ret = HI_VOD_GetVersion(stVodSetupRespInfo->au8BoardVersion, VOD_MAX_VERSION_LEN);
        if(HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }

        
    }
    /*user vod a record file */
    else
    {
        /*点播分支:获取点播文件的解析器句柄并创建传输任务*/
    }

    stVodSetupRespInfo->s32ErrCode = HI_SUCCESS;
    return HI_SUCCESS;
}
#endif

HI_S32 HI_VOD_Play(VOD_PLAY_MSG_S *stVodPlayReqInfo, VOD_RePLAY_MSG_S *stVodPlayRespInfo)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32Chn = 0;	
    VOD_SESSION_S *pSession = NULL;
    HI_U8 u8TempMediaType = 0;
    MBUF_VIDEO_STATUS_E video_enable = VIDEO_DISABLE;
    HI_BOOL audio_enable = HI_FALSE;
    HI_BOOL data_enable  = HI_FALSE;
    HI_BOOL bTempVarifyFlag = HI_FALSE;
    VOD_SESSION_STATE_E enVideoVodState = VOD_SESS_STATE_BUTT;
    VOD_SESSION_STATE_E enAudioVodState = VOD_SESS_STATE_BUTT;
    VOD_SESSION_STATE_E enDataVodState = VOD_SESS_STATE_BUTT;
    MTRANS_START_MSG_S   stMtransStartInfo;
    MTRANS_ReSTART_MSG_S stReMtransStartInfo;
    HI_U32 u32MTransHandle = 0;  
    HI_U32 u32MBuffHandle = 0;  
    memset(&stMtransStartInfo,0,sizeof(MTRANS_START_MSG_S));
    memset(&stReMtransStartInfo,0,sizeof(MTRANS_ReSTART_MSG_S));
    
    printf("vod play proc start\n");
    /*check parameters*/
    if(NULL == stVodPlayReqInfo|| NULL == stVodPlayRespInfo)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*if vod has deinit return failed*/
    if(VODSVR_STATE_STOP == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD play Failed: vod svr already deinited .\n");
        return HI_ERR_VOD_ALREADY_DEINIT;
    }
    /*1 first set respons is error*/
    stVodPlayRespInfo->s32ErrCode = HI_ERR_VOD_PLAY_FAILED;

    /*2 get session handle according to protocal type+session id*/
    s32Ret = HI_VOD_SessionFind(stVodPlayReqInfo->enSessType,stVodPlayReqInfo->aszSessID,
                                  &pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD play Failed for cli (can't find session :protocal %d, session id %s \n",
                    stVodPlayReqInfo->enSessType,stVodPlayReqInfo->aszSessID);
        return HI_ERR_VOD_PLAY_FAILED;
    }

    /*3 judge if the session has passed user varify or not*/
    s32Ret = HI_VOD_SessionGetVarifyFlag(pSession,&bTempVarifyFlag);
    if(HI_SUCCESS != s32Ret || HI_TRUE != bTempVarifyFlag)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD play Failed for cli (user has no right to play :protocal %d, session id %s \n",
                    stVodPlayReqInfo->enSessType,stVodPlayReqInfo->aszSessID);
        return HI_ERR_VOD_ILLEGAL_USER;
    }
    
    /*4 check media type(a|v|data) requested to play is in ready state or not. if not, not allowed to play  */
    HI_VOD_SessionGetState(pSession, VOD_STREAM_VIDEO, &enVideoVodState);
    HI_VOD_SessionGetState(pSession, VOD_STREAM_AUDIO, &enAudioVodState);
    HI_VOD_SessionGetState(pSession, VOD_STREAM_DATA, &enDataVodState);
    
    u8TempMediaType = stVodPlayReqInfo->u8PlayMediaType;

    if(   ( IS_VOD_VIDEO_TYPE(u8TempMediaType) && VOD_SESS_STATE_PLAYED == enVideoVodState)
       || ( IS_VOD_AUDIO_TYPE(u8TempMediaType) && VOD_SESS_STATE_PLAYED == enAudioVodState)
       || ( IS_VOD_DATA_TYPE(u8TempMediaType) && VOD_SESS_STATE_PLAYED == enDataVodState)
      )
    {
        
        stVodPlayRespInfo->s32ErrCode = HI_SUCCESS;
        return HI_SUCCESS;
    }
    
    if(   ( IS_VOD_VIDEO_TYPE(u8TempMediaType) && VOD_SESS_STATE_READY != enVideoVodState)
       || ( IS_VOD_AUDIO_TYPE(u8TempMediaType) && VOD_SESS_STATE_READY != enAudioVodState)
       || ( IS_VOD_DATA_TYPE(u8TempMediaType) && VOD_SESS_STATE_READY != enDataVodState)
      )
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD play Failed for cli (request media %d is not in ready state \n",
                    u8TempMediaType);
        return HI_ERR_VOD_PLAY_METHOD_MEDIA_NOTINPLAYSTAE;
    }

    /*5 start trans task*/
    /*5.1 consist HI_MTRANS_StartSendTask input param */
    stMtransStartInfo.u8MeidaType = u8TempMediaType;
    s32Ret = HI_VOD_SessionGetMtransHdl(pSession, &u32MTransHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD play Failed for cli (get mtrans handle from session failed %X \n",s32Ret);
        return HI_ERR_VOD_PLAY_FAILED;
    }
    else
    {
        /*set mtans handle*/
        stMtransStartInfo.u32TransTaskHandle = u32MTransHandle;
    }
    printf("start send task befor\n");
    /*5.2 start trans task*/
    s32Ret = HI_MTRANS_StartSendTask(&stMtransStartInfo,&stReMtransStartInfo);
    printf("start send task after\n");
    DEBUGPrtMtransStart(&stMtransStartInfo,&stReMtransStartInfo);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD play Failed for cli (can't start mtrans task %X \n",s32Ret);
        return HI_ERR_VOD_PLAY_METHOD_START_MTRANS_FAILED;
    }

     /*6 start mbuff:get mbuff handle and start mbuff*/
    s32Ret = HI_VOD_SessionGetMbufHdl(pSession, &u32MBuffHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD play Failed for cli (get mbuff handle from session failed %X \n",s32Ret);
        return HI_ERR_VOD_PLAY_FAILED;
    }
    else
    {
        /*get original register state*/
        //MBUF_GetRegister((MBUF_HEADER_S *)u32MBuffHandle,&video_enable, &audio_enable, &data_enable);
        HI_BufAbstract_GetRegister(g_stVodSvr.enLiveMode, (HI_VOID*)u32MBuffHandle,&video_enable, &audio_enable, &data_enable);
        /*start mbuff:register mbuff with a, v or data */
        if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
        {
            video_enable = VIDEO_REGISTER;
        }
        if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
        {
            audio_enable = HI_TRUE;
        }
        if(IS_VOD_DATA_TYPE(u8TempMediaType))
        {
            data_enable = HI_TRUE;
        }
        printf("+++++++++++play handle:%d: MBUF_Register video_enable= %d,audio_enable=%d, data_enable = %d\n",
               u32MBuffHandle,video_enable, audio_enable, data_enable);
        //MBUF_Register((MBUF_HEADER_S *)u32MBuffHandle,video_enable, audio_enable, data_enable);
        HI_BufAbstract_Register(g_stVodSvr.enLiveMode,(HI_VOID *)u32MBuffHandle,video_enable, audio_enable, data_enable);
        /*request I frame*/
        s32Chn = atoi(pSession->au8MediaFile);
        HI_BufAbstract_RequestIFrame(s32Chn);
    }
    
    /*6 start adapt task*/
    /*to do:自适应模块实现后补充该部分*/

    /*7 update vod meida state as play*/
    if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_VIDEO, VOD_SESS_STATE_PLAYED);
    }
    if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_AUDIO, VOD_SESS_STATE_PLAYED);
    }
    if(IS_VOD_DATA_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_DATA, VOD_SESS_STATE_PLAYED);
    }
	#if 0
	//mhua add for get the time stamp
	HI_ADDR   addr;
	HI_U32  len;
	HI_U64  pts;
	MBUF_PT_E   payload_type;
	HI_S32 ret = HI_FAILURE;

    HI_U8  slice_type = 0;
    HI_U32  slicenum = 0;
    HI_U32  frame_len = 0;
    
	while( HI_SUCCESS != ret )
	{
		ret = MBUF_Read((MBUF_HEADER_S *) u32MBuffHandle, &addr, &len,   &pts,  &payload_type,&slice_type,&slicenum,&frame_len);
	}
	
	if((payload_type== VIDEO_KEY_FRAME) ||(payload_type == VIDEO_NORMAL_FRAME) )
	{
		stVodPlayRespInfo->video_pts = pts;
		stVodPlayRespInfo->audio_pts = pts;
		printf("get the video frame \n");
	}
	else if(payload_type== AUDIO_FRAME)
	{
		stVodPlayRespInfo->audio_pts = pts;
		stVodPlayRespInfo->video_pts = pts;
		printf("get the audio frame \n");
	}
	#endif
    HI_U64  pts;
    HI_BufAbstract_GetCurPts(g_stVodSvr.enLiveMode, (HI_VOID*) u32MBuffHandle, &pts);
    stVodPlayRespInfo->video_pts = pts;
    stVodPlayRespInfo->audio_pts = pts;
    
    /*if can get this step, HI_VOD_Play SUCCESS*/
    stVodPlayRespInfo->s32ErrCode = HI_SUCCESS;
    return HI_SUCCESS;
}

HI_S32 HI_VOD_Pause(VOD_PAUSE_MSG_S*stVodPauseReqInfo, VOD_RePAUSE_MSG_S *stVodPauseRespInfo)
{
    
    HI_S32 s32Ret = 0;
    VOD_SESSION_S *pSession = NULL;
    HI_U8 u8TempMediaType = 0;
    MBUF_VIDEO_STATUS_E video_enable = VIDEO_DISABLE;
    HI_BOOL audio_enable = HI_FALSE;
    HI_BOOL data_enable  = HI_FALSE;
    HI_BOOL bTempVarifyFlag = HI_FALSE;
    VOD_SESSION_STATE_E enVideoVodState = VOD_SESS_STATE_BUTT;
    VOD_SESSION_STATE_E enAudioVodState = VOD_SESS_STATE_BUTT;
    VOD_SESSION_STATE_E enDataVodState = VOD_SESS_STATE_BUTT;
    MTRANS_STOP_MSG_S stMtransStopSendInfo;
    MTRANS_ReSTOP_MSG_S stReMtransStopInfo;
    HI_U32 u32MTransHandle = 0;  
    HI_U32 u32MBuffHandle = 0;  
    memset(&stMtransStopSendInfo,0,sizeof(MTRANS_STOP_MSG_S));
    memset(&stReMtransStopInfo,0,sizeof(MTRANS_ReSTOP_MSG_S));

    /*check parameters*/
    if(NULL == stVodPauseReqInfo|| NULL == stVodPauseRespInfo)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    if(VODSVR_STATE_STOP == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD pause Failed: vod svr already deinited .\n");
        return HI_ERR_VOD_ALREADY_DEINIT;
    }

    u8TempMediaType = stVodPauseReqInfo->u8PauseMediasType;
    /*if user request unrecognized stream, return error*/
    if( 0 == ((VOD_VIDEO_BITMAP|VOD_AUDIO_BITMAP|VOD_DATA_BITMAP)& u8TempMediaType))
    {
        DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_CRIT,"HI_VOD_Pause : error pause media type %d\n",u8TempMediaType);
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*1 first set respons is error*/
    stVodPauseRespInfo->s32ErrCode = HI_ERR_VOD_PAUSE_FAILED;

    /*2 get session handle according to protocal type+session id*/
    s32Ret = HI_VOD_SessionFind(stVodPauseReqInfo->enSessType,stVodPauseReqInfo->aszSessID,
                                  &pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD pause Failed for cli (can't find session :protocal %d, session id %s \n",
                    stVodPauseReqInfo->enSessType,stVodPauseReqInfo->aszSessID);
        return s32Ret;
    }

    /*3 judge if the session has passed user varify or not*/
    s32Ret = HI_VOD_SessionGetVarifyFlag(pSession,&bTempVarifyFlag);
    if(HI_SUCCESS != s32Ret || HI_TRUE != bTempVarifyFlag)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD pause Failed for cli (user has no right to pause :protocal %d, session id %s \n",
                    stVodPauseReqInfo->enSessType,stVodPauseReqInfo->aszSessID);
        return HI_ERR_VOD_ILLEGAL_USER;
    }
    
    /*4 check media type(a|v|data) requested to pause is in playing state or not. if not, not allowed to pause*/
    HI_VOD_SessionGetState(pSession, VOD_STREAM_VIDEO, &enVideoVodState);
    HI_VOD_SessionGetState(pSession, VOD_STREAM_AUDIO, &enAudioVodState);
    HI_VOD_SessionGetState(pSession, VOD_STREAM_DATA, &enDataVodState);
    
    if(   ( IS_VOD_VIDEO_TYPE(u8TempMediaType) && VOD_SESS_STATE_PLAYED!= enVideoVodState)
       || ( IS_VOD_AUDIO_TYPE(u8TempMediaType) && VOD_SESS_STATE_PLAYED != enAudioVodState)
       || ( IS_VOD_DATA_TYPE(u8TempMediaType) && VOD_SESS_STATE_PLAYED != enDataVodState)
      )
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD pause Failed for cli (request media %d is not in play state \n",
                    u8TempMediaType);
        return HI_ERR_VOD_PAUSE_FAILED;
    }

    /*5 stop mbuff:disenable mbuff with a, v or data */
    /*get mbuff handle and stop mbuff*/
    s32Ret = HI_VOD_SessionGetMbufHdl(pSession, &u32MBuffHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD pause Failed for cli (get mbuff handle from session failed %X \n",s32Ret);
        return HI_ERR_VOD_PAUSE_FAILED;
    }
    else
    {  
        /*get original register state*/
     //   MBUF_GetRegister((MBUF_HEADER_S *)u32MBuffHandle,&video_enable, &audio_enable, &data_enable);
        HI_BufAbstract_GetRegister(g_stVodSvr.enLiveMode,(HI_VOID *)u32MBuffHandle,&video_enable, &audio_enable, &data_enable);
        /*stop mbuff:disenable mbuff with a, v or data */
        if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
        {
            video_enable = VIDEO_DISABLE;
        }
        if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
        {
            audio_enable = HI_FALSE;
        }
        if(IS_VOD_DATA_TYPE(u8TempMediaType))
        {
            data_enable = HI_FALSE;
        }
      //  MBUF_Register((MBUF_HEADER_S *)u32MBuffHandle,video_enable, audio_enable, data_enable);
    	HI_BufAbstract_Register(g_stVodSvr.enLiveMode,(HI_VOID *)u32MBuffHandle,video_enable, audio_enable, data_enable);    
    }

    /*important: 此处sleep一段时间，即稍后停止发送任务，使已写入mbuff的数据发送完。防止立即
      停止发送导致已写入mbuff中的数据不能读出阻塞mbuff*/
    //(HI_VOID)usleep(40000);

    /*5 stop trans task: 此处不用停止mtrans也可达到pause的目的,由于mbuff已停止写入相应的媒体流*/
    /*5.1 consist HI_MTRANS_StopSendTask input param */
    stMtransStopSendInfo.u8MeidaType = u8TempMediaType;
    s32Ret = HI_VOD_SessionGetMtransHdl(pSession, &u32MTransHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD pause Failed for cli (get mtrans handle from session failed %X \n",s32Ret);
        return HI_ERR_VOD_PAUSE_FAILED;
    }
    else
    {
        /*set mtans handle*/
        stMtransStopSendInfo.u32TransTaskHandle = u32MTransHandle;
    }
    
    /*5.2 stop trans task*/
    s32Ret = HI_MTRANS_StopSendTask(&stMtransStopSendInfo,&stReMtransStopInfo);
    DEBUGPrtMtransStop(&stMtransStopSendInfo,&stReMtransStopInfo);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD pause Failed for cli (can't pause mtrans task %X \n",s32Ret);
        return s32Ret;
    }

    
    /*6 stop adapt task*/
    /*to do:自适应模块实现后补充该部分*/

    /*7 update vod meida state as ready*/
    if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_VIDEO, VOD_SESS_STATE_READY);
    }
    if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_AUDIO, VOD_SESS_STATE_READY);
    }
    if(IS_VOD_DATA_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_DATA, VOD_SESS_STATE_READY);
    }

    /*if can get this step, HI_VOD_Pause SUCCESS*/
    stVodPauseRespInfo->s32ErrCode = HI_SUCCESS;
    return HI_SUCCESS;
}


HI_S32 HI_VOD_Resume(VOD_RESUME_MSG_S*stVodResumeReqInfo, VOD_RESUME_RespMSG_S *stVodResumeRespInfo)
{
    
    HI_S32 s32Ret = 0;
    VOD_SESSION_S *pSession = NULL;
    HI_U8 u8TempMediaType = 0;
    VOD_SESSION_STATE_E enVideoVodState = VOD_SESS_STATE_BUTT;
    VOD_SESSION_STATE_E enAudioVodState = VOD_SESS_STATE_BUTT;
    VOD_SESSION_STATE_E enDataVodState = VOD_SESS_STATE_BUTT;
    MTRANS_RESUME_MSG_S stMtransResumeInfo;
    MTRANS_RESUME_RespMSG_S stReMtransResumeResp;
    HI_U32 u32MTransHandle = 0;  
    memset(&stMtransResumeInfo,0,sizeof(stMtransResumeInfo));
    memset(&stReMtransResumeResp,0,sizeof(stReMtransResumeResp));

    /*check parameters*/
    if(NULL == stVodResumeReqInfo|| NULL == stVodResumeRespInfo)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    if(VODSVR_STATE_STOP == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD resume Failed: vod svr already deinited .\n");
        return HI_ERR_VOD_ALREADY_DEINIT;
    }

    u8TempMediaType = stVodResumeReqInfo->u8MediasType;
    /*if user request unrecognized stream, return error*/
    if( 0 == ((VOD_VIDEO_BITMAP|VOD_AUDIO_BITMAP|VOD_DATA_BITMAP)& u8TempMediaType))
    {
        DEBUG_VOD_PRINT(HI_VOD_MODEL,LOG_LEVEL_CRIT,"HI_VOD_Resume : error pause media type %d\n",u8TempMediaType);
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*1 first set respons is error*/
    stVodResumeRespInfo->s32ErrCode = HI_ERR_VOD_PAUSE_FAILED;

    /*2 get session handle according to protocal type+session id*/
    s32Ret = HI_VOD_SessionFind(stVodResumeReqInfo->enSessType,stVodResumeReqInfo->aszSessID,
                                  &pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD resume Failed for cli (can't find session :protocal %d, session id %s \n",
                    stVodResumeReqInfo->enSessType,stVodResumeReqInfo->aszSessID);
        return s32Ret;
    }

    /*4 check media type(a|v|data) requested to pause is in playing state or not. if not, not allowed to pause*/
    HI_VOD_SessionGetState(pSession, VOD_STREAM_VIDEO, &enVideoVodState);
    HI_VOD_SessionGetState(pSession, VOD_STREAM_AUDIO, &enAudioVodState);
    HI_VOD_SessionGetState(pSession, VOD_STREAM_DATA, &enDataVodState);
    
    if(   ( IS_VOD_VIDEO_TYPE(u8TempMediaType) && VOD_SESS_STATE_READY!= enVideoVodState)
       || ( IS_VOD_AUDIO_TYPE(u8TempMediaType) && VOD_SESS_STATE_READY != enAudioVodState)
       || ( IS_VOD_DATA_TYPE(u8TempMediaType) && VOD_SESS_STATE_READY != enDataVodState)
      )
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD resume Failed for cli (request media %d is not in ready state \n",
                    u8TempMediaType);
        return HI_ERR_VOD_PAUSE_FAILED;
    }


    stMtransResumeInfo.u8MeidaType = u8TempMediaType;
    s32Ret = HI_VOD_SessionGetMtransHdl(pSession, &u32MTransHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD resume Failed for cli (get mtrans handle from session failed %X \n",s32Ret);
        return HI_ERR_VOD_PAUSE_FAILED;
    }
    else
    {
        /*set mtans handle*/
        stMtransResumeInfo.u32TransTaskHandle = u32MTransHandle;
    }
    
    s32Ret = HI_MTRANS_ResumeSendTask(&stMtransResumeInfo,&stReMtransResumeResp);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD resume Failed for cli (can't resume mtrans task %X \n",s32Ret);
        return s32Ret;
    }

    
    /*6 stop adapt task*/
    /*to do:自适应模块实现后补充该部分*/

    /*7 update vod meida state as play*/
    if(IS_VOD_VIDEO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_VIDEO, VOD_SESS_STATE_PLAYED);
    }
    if(IS_VOD_AUDIO_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_AUDIO, VOD_SESS_STATE_PLAYED);
    }
    if(IS_VOD_DATA_TYPE(u8TempMediaType))
    {
        HI_VOD_SessionSetState(pSession, VOD_STREAM_DATA, VOD_SESS_STATE_PLAYED);
    }

    stVodResumeRespInfo->s32ErrCode = HI_SUCCESS;
    return HI_SUCCESS;
}


HI_S32 HI_VOD_Teardown(VOD_TEAWDOWN_MSG_S *stVodCloseReqInfo, VOD_ReTEAWDOWN_MSG_S *stVodCloseRespInfo)
{
    HI_S32 s32Ret = 0;
//    HI_S32 s32Chn = 0;
    VOD_SESSION_S *pSession = NULL;
    MTRANS_DESTROY_MSG_S stMtransDestroyInfo;
    MTRANS_ReDESTROY_MSG_S stReMtransDestroyInfo;
    HI_U32 u32MTransHandle = 0;  
    HI_U32 u32MBuffHandle = 0;  
    memset(&stMtransDestroyInfo,0,sizeof(MTRANS_DESTROY_MSG_S));
    memset(&stReMtransDestroyInfo,0,sizeof(MTRANS_ReDESTROY_MSG_S));

    /*check parameters*/
    if(NULL == stVodCloseReqInfo|| NULL == stVodCloseRespInfo)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*1 first set respons is error*/
    stVodCloseRespInfo->s32ErrCode = HI_ERR_VOD_TEARDOWN_FAILED;

    //printf(" before HI_VOD_SessionFind\n");
    /*2 get session handle according to protocal type+session id*/
    printf("HI_VOD_Teardown--enSessType=%d,aszSessID:%s!\n",stVodCloseReqInfo->enSessType,stVodCloseReqInfo->aszSessID);
    s32Ret = HI_VOD_SessionFind(stVodCloseReqInfo->enSessType,stVodCloseReqInfo->aszSessID,
                                  &pSession);
    /*the vod session has not creat,so no resource need to free,just return ok */
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD teardown Failed for cli (can't find session :protocal %d, session id %s \n",
                    stVodCloseReqInfo->enSessType,stVodCloseReqInfo->aszSessID);
        
        return HI_SUCCESS;
    }

    /*5 destroy trans task*/
    /*5.1 consist HI_MTRANS_DestroySendTask input param */
    s32Ret = HI_VOD_SessionGetMtransHdl(pSession, &u32MTransHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD teardown Failed for cli (get mtrans handle from vod session failed %X \n",s32Ret);
        //return HI_ERR_VOD_TEARDOWN_FAILED;
    }
    else
    {
        /*set mtans handle*/
        stMtransDestroyInfo.u32TransTaskHandle = u32MTransHandle;
        /*5.2 destroy trans task*/
        s32Ret = HI_MTRANS_DestroySendTask(&stMtransDestroyInfo,&stReMtransDestroyInfo);
        DEBUGPrtMtransDestroy(&stMtransDestroyInfo,&stReMtransDestroyInfo);
        if(HI_SUCCESS != s32Ret)
        {
            HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                        "VOD teardown Failed for cli (can't destroy mtrans task %X \n",s32Ret);
        }
        printf("<VOD> vod hand teardown destroySendTask ret:%x \n",s32Ret);
    }

    /*to do: 是否需要适当的sleep?
           mtrans task destroy 返回后，有可能还在read mbuff,因为只是置了task destroy 标志。
           interleaved tcp方式可保证立即释放mbuff不出错，因为会话线呈不会再进入发送数据的分支，故不会read buff*/

    /*6 free mbuff for this vod session */
    /*get mbuff handle*/
    s32Ret = HI_VOD_SessionGetMbufHdl(pSession, &u32MBuffHandle);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD teardown Failed for cli (get mbuff handle from session failed %X \n",s32Ret);
        //return HI_ERR_VOD_TEARDOWN_FAILED;
    }
    else
    {
        if(0 != u32MBuffHandle)
        {
            /*free mbuff and set vod mubff handle as 0*/
            //s32Ret = MBUF_Free(&u32MBuffHandle);
            if(stVodCloseReqInfo->enTransMode != MTRANS_MODE_MULTICAST)
            {
                s32Ret = HI_BufAbstract_Free(g_stVodSvr.enLiveMode,(HI_VOID**)&u32MBuffHandle);
                if(HI_SUCCESS != s32Ret)
                {
                    HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                                "VOD teardown Failed for cli (mbuff free fail %X \n",s32Ret);
                }
            }
            
            HI_VOD_SessionSetMbufHdl(pSession,0);
		    printf("<VOD> vod hand teardown free mbuf ret:%x \n",s32Ret);
        }
    }

    /*6 destroy adapt task*/
    /*to do:自适应模块实现后补充该部分*/

    /*7 destroy the vod session itself*/
    s32Ret = HI_VOD_SessionDestroy(pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD teardown Failed for cli (vod session destroy fail %X \n",s32Ret);
        //return HI_ERR_VOD_TEARDOWN_FAILED;
    }
	 printf("<VOD> vod hand teardown HI_VOD_SessionDestroy ret:%x \n",s32Ret);
    /*if can get this step, HI_VOD_Play SUCCESS*/
    stVodCloseRespInfo->s32ErrCode = HI_SUCCESS;
    return HI_SUCCESS;
}

/*获取首个发送数据的时间戳，并发送首个数据包
该接口会阻塞,直到成功后期基准时戳或发生错误*/
HI_U32 HI_VOD_GetBaseTimeStamp(VOD_GET_BASEPTS_MSG_S *stVodGetPtsReqInfo,
                                            VOD_ReGET_BASEPTS_MSG_S *stVodGetPtsRespInfo)
{
    HI_U32 s32Ret = 0;
//    HI_U32 u32MTransHandle = 0;  
    VOD_SESSION_S *pSession = NULL;
    HI_BOOL bTempVarifyFlag = HI_FALSE;
    HI_U64  u64BasePts = 0;
    
    /*check parameters*/
    if(NULL == stVodGetPtsReqInfo|| NULL == stVodGetPtsRespInfo)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }

    /*if vod has deinit return failed*/
    if(VODSVR_STATE_STOP == g_stVodSvr.enSvrState)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_ERR,"VOD play Failed: vod svr already deinited .\n");
        return HI_ERR_VOD_ALREADY_DEINIT;
    }
    /*1 first set respons is error*/
    stVodGetPtsRespInfo->s32ErrCode = HI_ERR_VOD_PLAY_FAILED;

    /*2 get session handle according to protocal type+session id*/
    s32Ret = HI_VOD_SessionFind(stVodGetPtsReqInfo->enSessType,stVodGetPtsReqInfo->aszSessID,
                                  &pSession);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD get base pts Failed for cli (can't find session :protocal %d, session id %s \n",
                    stVodGetPtsReqInfo->enSessType,stVodGetPtsReqInfo->aszSessID);
        return HI_ERR_VOD_PLAY_FAILED;
    }

    /*3 judge if the session has passed user varify or not*/
    s32Ret = HI_VOD_SessionGetVarifyFlag(pSession,&bTempVarifyFlag);
    if(HI_SUCCESS != s32Ret || HI_TRUE != bTempVarifyFlag)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD get base pts Failed for cli (user has no right to play :protocal %d, session id %s \n",
                    stVodGetPtsReqInfo->enSessType,stVodGetPtsReqInfo->aszSessID);
        return HI_ERR_VOD_ILLEGAL_USER;
    }
	printf("VOD call Mtrans get base time :VOD id %s hdl %p,trans hdl %X",stVodGetPtsReqInfo->aszSessID,pSession,pSession->u32MTransHandle)	;

    s32Ret = HI_MTRANS_GetBaseTimeStamp(pSession->u32MTransHandle, &u64BasePts);
    if(HI_SUCCESS != s32Ret)
    {
        HI_LOG_SYS(HI_VOD_MODEL,LOG_LEVEL_CRIT,
                    "VOD get base pts Failed for cli (get pts from mtrans err %X :protocal %d, session id %s \n",
                    s32Ret,stVodGetPtsReqInfo->enSessType,stVodGetPtsReqInfo->aszSessID);
        return s32Ret;
    }

    /*if can get this step, HI_VOD_GetPts SUCCESS*/
    stVodGetPtsRespInfo->s32ErrCode = HI_SUCCESS;
    stVodGetPtsRespInfo->u64BaseTimeStamp = u64BasePts;
   	printf("vod get base time finish\n");	
    return HI_SUCCESS;
}

HI_BOOL HI_VOD_IsRunning()
{
    return (VODSVR_STATE_Running == g_stVodSvr.enSvrState);
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

