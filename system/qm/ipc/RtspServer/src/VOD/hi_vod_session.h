/******************************************************************************

  Copyright (C), 2007-, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_vod_session.h
  Version       : Initial Draft
  Author        : W54723
  Created       : 2008/02/15
  Description   : 
  History       :
  1.Date        : 
    Author      : 
    Modification: 
******************************************************************************/
#ifndef __HI_VOD_SESSION_H__
#define __HI_VOD_SESSION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "hi_type.h"
#include "hi_vod.h"
#include "hi_list.h"

/*========================VOD内部数据结构=================*/
#define HI_VOD_MODEL "VOD"
#define HI_VOD_MAX_SUPPORT_CHN 4
#define VOD_DEFAULT_SESS_NUM 32
#define VOD_MAX_SESS_NUM 32
#define VOD_MAX_SESSID_LEN 64       /*vod session length*/

#define HI_VOD_CALLOC calloc
#define HI_VOD_FREE   free

typedef char  VOD_SESSION_ID[VOD_MAX_SESSID_LEN];

typedef enum hiVOD_SVR_STATE
{
    VODSVR_STATE_STOP = 0,         /*准备阶段， 还没运行*/        
    VODSVR_STATE_Running = 1,      /*准备阶段， 还没运行*/
    VODSVR_STATE_BUTT              /*准备阶段， 还没运行*/
    
}VOD_SVR_STATE_E;

/*VOD抽象session的状态机*/
typedef enum hiVOD_SESSION_STATE_E
{
    VOD_SESS_STATE_INIT        = 0,
    VOD_SESS_STATE_READY       = 1,
    VOD_SESS_STATE_PLAYED      = 2,
    VOD_SESS_STATE_BUTT  
}VOD_SESSION_STATE_E;

typedef enum hiVOD_SESSION_THREAD_STATE
{
    SESSION_THREAD_STATE_INIT    = 0, /*已被创建*/
    SESSION_THREAD_STATE_RUNNING = 1, /*创建并运行*/
    SESSION_THREAD_STATE_STOP = 2, /*已停止并销毁*/
    SESSION_THREAD_STATE_BUTT
    
}VOD_SESSION_THREAD_STATE; 

typedef struct hiVOD_SESSION_S
{           
    /*用于维护链表*/
    List_Head_S vod_sess_list;

    /*会话协议类型. '会话协议类型+session id'作为session的key值*/
    VOD_SESS_PROTOCAL_TYPE_E   enSessType;

    /*session id, 由链接创建时候按会话协议随机产生*/
    VOD_SESSION_ID        aszSessID;

    /*该用户是否已通过鉴权的标志,0-还未通过鉴权 1-已通过鉴权
      当该session已在setup状态时，若用户还未通过鉴权则认为该
      用户为非法用户*/
    HI_BOOL       bPassedVarifyFlag;

    /*该Session所对应的文件名,点播时表示文件名,直播时表示监控通道号*/
    //HI_CHAR       au8MediaFile[VOD_MAX_STRING_LENGTH];
    HI_CHAR       au8MediaFile[128];

    /*媒体资源中的视频格式*/
    MT_VIDEO_FORMAT_E  enVideoFormat;

    /*媒体资源中的音频频格式*/
    MT_AUDIO_FORMAT_E  enAudioFormat;           

    /*该会话已请求建立的媒体类型:a v data*/
    HI_U8      enMediaType;

    /*该session的各媒体状态 0是视频会话状态，1是音频会话状态, 2是Data类数据会话状态 */
    volatile VOD_SESSION_STATE_E enSessState[VOD_STREAM_MAX];

    /*该session对应的媒体分发任务的handle*/
    HI_U32     u32MbuffHandle; 

    /*该session对应的媒体传输任务的handle*/
    HI_U32     u32MTransHandle;

    /*该session对应的自适应调节任务的handle*/
    HI_U32     u32MAdaptHandle;
    
}VOD_SESSION_S;

typedef struct hiVOD_SVR_S
{
    HI_S32              s32MaxChnNum;                                /*channel number support to vod*/
    HI_S32              s32MaxUserNum;                              /*max connecting user number  */
    HI_S32              s32CurrUserNum;                             /*current connected user number
                                                                      which should be less than Max User Num */
    VOD_SVR_STATE_E    enSvrState;                                  /*vod server state*/
    int        enLiveMode;
    VOD_SESSION_S       *pVODSessList;                                  /*vod session list*/

}VOD_SVR_S;

HI_S32 HI_VOD_SessionGetInfo(HI_CHAR **ppInfoStr,HI_U32* pu32InfoLen);

/*get flag of user passed varify or not */
HI_S32 HI_VOD_SessionGetVarifyFlag(VOD_SESSION_S *pSession,HI_BOOL *bVarifyFlag);

/*get media video format of pointed vod session */
HI_VOID HI_VOD_SessionGetVideoFormat(VOD_SESSION_S *pSession,MT_VIDEO_FORMAT_E* penVideoFormat);

/*set media video format of pointed vod session */
HI_VOID HI_VOD_SessionSetVideoFormat(VOD_SESSION_S *pSession,MT_VIDEO_FORMAT_E enVideoFormat);

/*get media audio format of pointed vod session */
HI_VOID HI_VOD_SessionGetAudioFormat(VOD_SESSION_S *pSession,MT_AUDIO_FORMAT_E* penAudioFormat);

/*set media video format of pointed vod session */
HI_VOID HI_VOD_SessionSetAudioFormat(VOD_SESSION_S *pSession,MT_AUDIO_FORMAT_E enAudioFormat);

/*get media type's state of pointed vod session */
HI_VOID HI_VOD_SessionGetState(VOD_SESSION_S *pSession,HI_U8 u8Type, VOD_SESSION_STATE_E* penState);


/*set media type's state of pointed vod session */
HI_VOID HI_VOD_SessionSetState(VOD_SESSION_S *pSession,HI_U8 u8Type, VOD_SESSION_STATE_E enState);


/*get mbuff handle of pointed vod session */
HI_S32 HI_VOD_SessionGetMbufHdl(VOD_SESSION_S *pSession, HI_U32 *pMbufHandle );
/*set mbuff handle of pointed vod session */
HI_VOID HI_VOD_SessionSetMbufHdl(VOD_SESSION_S *pSession,HI_U32 pMbufHandle);

/*get mtrans task handle of pointed vod session */
HI_S32 HI_VOD_SessionGetMtransHdl(VOD_SESSION_S *pSession, HI_U32 *pMtransHandle );

/*set mtrans task  handle of pointed vod session */
HI_VOID HI_VOD_SessionSetMtransHdl(VOD_SESSION_S *pSession,HI_U32 pMtransHandle);

/*get currect valid connect number*/
HI_S32 HI_VOD_SessionGetConnNum();

/*init session list*/
HI_S32 HI_VOD_SessionListInit(HI_S32 s32TotalConnNum);

/*create session: get a unused node from the free session list*/
HI_S32 HI_VOD_SessionCreat(VOD_SESSION_S **ppSession );

/*init a appointed session*/
HI_VOID HI_VOD_SessionInit(VOD_SESSION_S *pSession, VOD_SETUP_MSG_S *stSetupReqInfo,HI_BOOL bUserPassed);

/*free the resource the session:here just
 need to remove the session from busy list*/
HI_S32 HI_VOD_SessionDestroy(VOD_SESSION_S *pSession);

/*2 get session handle according to protocal type+session id
    return pointer to vod session if success, otherwise return NULL 
*/
HI_S32 HI_VOD_SessionFind(VOD_SESS_PROTOCAL_TYPE_E enSessType,HI_CHAR* pau8SessID,
                          VOD_SESSION_S **ppSession);
                          
/*all busy session destroy: */
HI_VOID HI_VOD_SessionAllDestroy();


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*End of #ifndef __HI_VOD_SESSION_H__*/
