/******************************************************************************

  Copyright (C), 2007-, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_vod_session.c
  Version       : Initial Draft
  Author        : W54723
  Created       : 2008/02/15
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
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "hi_vod_session.h"
#include "hi_vod_error.h"
//btlib
//#include "hi_debug_def.h"
//#include "hi_log_app.h"
#include "ext_defs.h"
#include "hi_mbuf.h"

HI_CHAR au8CliInfo[2048];
//VOD_SESSION_S   *g_astrVodSess = NULL;      /*vod sessions list pointer*/
//static List_Head_S     g_strVodSesFree ;                       /*free session list head*/
static List_Head_S     g_strVodSesBusy ;                       /*busy session list head*/
static pthread_mutex_t g_VodSesListLock;                      /*session list lock*/

HI_S32 HI_VOD_SessionGetInfo(HI_CHAR **ppInfoStr,HI_U32* pu32InfoLen)
{
   VOD_SESSION_S *pSess = NULL;
   List_Head_S *pos = NULL;
   HI_CHAR *pTempPtr = au8CliInfo;
   HI_S32 s32CliNum = 0;
   //HI_S32 s32FreeSessNum = 0;

   (HI_VOID)pthread_mutex_lock(&g_VodSesListLock);

    HI_List_For_Each(pos, &g_strVodSesBusy)
    {
        pSess = HI_LIST_ENTRY(pos, VOD_SESSION_S, vod_sess_list);

        /*get the node from busy list*/
        pTempPtr +=sprintf(pTempPtr,"Num:%d sessId:%s ",++s32CliNum,pSess->aszSessID);

        
        pTempPtr +=sprintf(pTempPtr,"Video state:%d  Audio state:%d   MDAlarm state:%d",pSess->enSessState[VOD_STREAM_VIDEO],
                pSess->enSessState[VOD_STREAM_AUDIO],pSess->enSessState[VOD_STREAM_DATA]);
        
        pTempPtr +=sprintf(pTempPtr,"loseFramNum:%llu",((MBUF_HEADER_S *)(pSess->u32MbuffHandle))->discard_count);

        pTempPtr +=sprintf(pTempPtr,"\r\n\r\n");
    }
   (HI_VOID) pthread_mutex_unlock(&g_VodSesListLock);

    /*statistic for free session number*/
    (HI_VOID)pthread_mutex_lock(&g_VodSesListLock);

    #if 0
    HI_List_For_Each(pos, &g_strVodSesFree)
    {
        pSess = HI_LIST_ENTRY(pos, VOD_SESSION_S, vod_sess_list);

        ++s32FreeSessNum;
    }
    #endif
    
    (HI_VOID)pthread_mutex_unlock(&g_VodSesListLock);

    //pTempPtr +=sprintf(pTempPtr,"free session num: %d.\n",s32FreeSessNum);


    *ppInfoStr = au8CliInfo;
    *pu32InfoLen = strlen(au8CliInfo);

    return HI_SUCCESS;
}


/*get flag of user passed varify or not */
HI_S32 HI_VOD_SessionGetVarifyFlag(VOD_SESSION_S *pSession,HI_BOOL *bVarifyFlag)
{
    if(NULL == pSession ||NULL ==  bVarifyFlag)
    {
        return HI_FAILURE;
    }

    *bVarifyFlag = pSession->bPassedVarifyFlag;

    return HI_SUCCESS;
}

/*get media video format of pointed vod session */
HI_VOID HI_VOD_SessionGetVideoFormat(VOD_SESSION_S *pSession,MT_VIDEO_FORMAT_E* penVideoFormat)
{
    *penVideoFormat = pSession->enVideoFormat;
    return;
}

/*set media video format of pointed vod session */
HI_VOID HI_VOD_SessionSetVideoFormat(VOD_SESSION_S *pSession,MT_VIDEO_FORMAT_E enVideoFormat)
{
    pSession->enVideoFormat = enVideoFormat;
    return;
}


/*get media audio format of pointed vod session */
HI_VOID HI_VOD_SessionGetAudioFormat(VOD_SESSION_S *pSession,MT_AUDIO_FORMAT_E* penAudioFormat)
{
    *penAudioFormat = pSession->enAudioFormat;
    return;
}

/*set media video format of pointed vod session */
HI_VOID HI_VOD_SessionSetAudioFormat(VOD_SESSION_S *pSession,MT_AUDIO_FORMAT_E enAudioFormat)
{
    pSession->enAudioFormat = enAudioFormat;
    return;
}


/*get media type's state of pointed vod session */
HI_VOID HI_VOD_SessionGetState(VOD_SESSION_S *pSession,HI_U8 u8Type, VOD_SESSION_STATE_E* penState)
{
    *penState = pSession->enSessState[u8Type];
    return;
}

/*set media type's state of pointed vod session */
HI_VOID HI_VOD_SessionSetState(VOD_SESSION_S *pSession,HI_U8 u8Type, VOD_SESSION_STATE_E enState)
{
    pSession->enSessState[u8Type] = enState;
    return;
}

/*get mbuff handle of pointed vod session */
HI_S32 HI_VOD_SessionGetMbufHdl(VOD_SESSION_S *pSession, HI_U32 *pMbufHandle )
{
    if(NULL == pSession || NULL == pMbufHandle)
    {
        return HI_FAILURE;
    }
    *pMbufHandle = pSession->u32MbuffHandle;

    return HI_SUCCESS;
}

/*set mbuff handle of pointed vod session */
HI_VOID HI_VOD_SessionSetMbufHdl(VOD_SESSION_S *pSession,HI_U32 pMbufHandle)
{
    pSession->u32MbuffHandle = pMbufHandle;
    return;
}

/*get mtrans task handle of pointed vod session */
HI_S32 HI_VOD_SessionGetMtransHdl(VOD_SESSION_S *pSession, HI_U32 *pMtransHandle )
{
    if(NULL == pSession || NULL == pMtransHandle)
    {
        return HI_FAILURE;
    }

    *pMtransHandle = pSession->u32MTransHandle;
    return HI_SUCCESS;
}

/*set mtrans task  handle of pointed vod session */
HI_VOID HI_VOD_SessionSetMtransHdl(VOD_SESSION_S *pSession,HI_U32 pMtransHandle)
{
    pSession->u32MTransHandle = pMtransHandle;
    return;
}

/*get currect valid connect number*/
HI_S32 HI_VOD_SessionGetConnNum()
{
    HI_S32 s32ValidConnNum = 0;
    List_Head_S *pos = NULL;
    
    (HI_VOID)pthread_mutex_lock(&g_VodSesListLock);

    HI_List_For_Each(pos, &g_strVodSesBusy)
    {
        ++s32ValidConnNum;
    }    
    (HI_VOID)pthread_mutex_unlock(&g_VodSesListLock);

    return s32ValidConnNum;
}

/*init session list*/
HI_S32 HI_VOD_SessionListInit(HI_S32 s32TotalConnNum)
{

    #if 0
    int i = 0;

    /*if parameters is <=0 or > max supported vod session number, set session num as default*/
    if(s32TotalConnNum <= 0 || s32TotalConnNum > VOD_MAX_SESS_NUM)
    {
        s32TotalConnNum = VOD_DEFAULT_SESS_NUM;
    }

    //printf("  VOD Session number = %d\n",s32TotalConnNum);
    /*calloc for vod session list*/
    g_astrVodSess =(VOD_SESSION_S*)HI_VOD_CALLOC((HI_U32)s32TotalConnNum,sizeof(VOD_SESSION_S));
    if(NULL == g_astrVodSess)
    {
        HI_LOG_SYS(HI_VOD_MODEL ,LOG_LEVEL_ERR,
                    "vod session list init error :calloc failed for %d session num\n",s32TotalConnNum);
        return HI_ERR_VOD_CALLOC_FAILED;
    }
    else
    {
        memset(g_astrVodSess,0,sizeof(VOD_SESSION_S)*((HI_U32)s32TotalConnNum));
    }
    
    /*init session busy and free list*/
    HI_LIST_HEAD_INIT_PTR(&g_strVodSesFree);
    #endif
    
    HI_LIST_HEAD_INIT_PTR(&g_strVodSesBusy);
    (HI_VOID)pthread_mutex_init(&g_VodSesListLock,  NULL);

    #if 0
    /*add sessions( with supported conn num) into free list*/
    for(i = 0; i<s32TotalConnNum; i++)
    {
        HI_List_Add_Tail(&g_astrVodSess[i].vod_sess_list, &g_strVodSesFree);
    }
    #endif
    
    return HI_SUCCESS;
}

/*remove a session: del it from busy list and add to free list*/
HI_VOID HI_VOD_SessionListRemove(VOD_SESSION_S *pSession)
{
    /*free*/
    List_Head_S *pSessList;
    List_Head_S *pos;

    pSessList = &(pSession->vod_sess_list);

    (HI_VOID)pthread_mutex_lock(&g_VodSesListLock);

    HI_List_For_Each(pos, &g_strVodSesBusy)
    {
        /*get the node from busy list*/
        if(pos == pSessList)
        {
            /*del the node to busy list，and add to free list*/
            HI_List_Del(pos);
            //HI_List_Add(pos, &g_strVodSesFree);
            break;
        }
    }
    (HI_VOID)pthread_mutex_unlock(&g_VodSesListLock);

    free(pSession);
    
}

/*create session: get a unused node from the free session list*/
HI_S32 HI_VOD_SessionCreat(VOD_SESSION_S **ppSession )
{
    //List_Head_S *plist = NULL;
    VOD_SESSION_S *pSess = NULL;
    
    /*lock the creat process*/
    (HI_VOID)pthread_mutex_lock(&g_VodSesListLock);
    
    #if 0
    if(HI_List_Empty(&g_strVodSesFree))
    {
        HI_LOG_SYS(HI_VOD_MODEL ,LOG_LEVEL_ERR,
                    "can't creat vod session:over preset session num\n");
        (HI_VOID)pthread_mutex_unlock(&g_VodSesListLock);            
        return HI_ERR_VOD_OVER_CONN_NUM;            
    }
    else
    {
        /*get a session from free list,and add to busy list*/
        plist = g_strVodSesFree.next;
        HI_List_Del(plist);
        HI_List_Add(plist, &g_strVodSesBusy);
        *ppSession = HI_LIST_ENTRY(plist, VOD_SESSION_S, vod_sess_list);
        
    }
    #endif

    pSess = (VOD_SESSION_S *)calloc(1, sizeof(VOD_SESSION_S));
    if(!pSess)
    {
        printf("%s  %d, calloc failed!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
        return HI_ERR_VOD_CALLOC_FAILED;
    }
    *ppSession = pSess;
    HI_List_Add(&pSess->vod_sess_list, &g_strVodSesBusy);
    (HI_VOID)pthread_mutex_unlock(&g_VodSesListLock);

    return HI_SUCCESS;
}

/*init a appointed session*/
HI_VOID HI_VOD_SessionInit(VOD_SESSION_S *pSession, VOD_SETUP_MSG_S *stSetupReqInfo,HI_BOOL bUserPassed)
{
    HI_S32 i = 0;

    /*record session protocal type*/
    pSession->enSessType = stSetupReqInfo->enSessType;

    /*record session id
     to do:若sessionid超过预定长度如何处理?*/
    strncpy(pSession->aszSessID,stSetupReqInfo->aszSessID, VOD_MAX_SESSID_LEN-1);
    pSession->aszSessID[VOD_MAX_SESSID_LEN-1] = '\0';

    /*record media file*/
    /*to do:若filename超过预定长度如何处理?*/
    strncpy(pSession->au8MediaFile,stSetupReqInfo->au8MediaFile,128-1);
    pSession->au8MediaFile[128-1] = '\0';
    
    /*set user varify flag */
    pSession->bPassedVarifyFlag = bUserPassed;

    /*record media type*/
    pSession->enMediaType = stSetupReqInfo->u8SetupMeidaType;

    /*init media's vod state as initial*/
    /*init request stream flag as false*/
    for(i = 0; i<VOD_STREAM_MAX; i++)
    {
        pSession->enSessState[i] = VOD_SESS_STATE_INIT;
    }

    /*init mbuff handle as null*/
    pSession->u32MbuffHandle = 0;

    /*init mtrans task handle as null */
    pSession->u32MTransHandle = 0;

    /*init adapt task handle as null*/
    pSession->u32MAdaptHandle = 0;
    
    return;
}



/*free the resource the session:here just
 need to remove the session from busy list*/
HI_S32 HI_VOD_SessionDestroy(VOD_SESSION_S *pSession)
{
    if(NULL == pSession)
    {
        return HI_ERR_VOD_ILLEGAL_PARAM;
    }
    /*remove the session from busy list*/
    HI_VOD_SessionListRemove(pSession);
    return HI_SUCCESS;
}

/*2 get session handle according to protocal type+session id
    return pointer to vod session if success, otherwise return NULL 
*/
HI_S32 HI_VOD_SessionFind(VOD_SESS_PROTOCAL_TYPE_E enSessType,HI_CHAR* pas8SessID,
                          VOD_SESSION_S **ppSession)
{
    HI_S32 s32Ret = 0;
    List_Head_S *pos= NULL;
    VOD_SESSION_S *pTempSession = NULL;

    s32Ret = HI_FAILURE;
    (HI_VOID)pthread_mutex_lock(&g_VodSesListLock);

    HI_List_For_Each(pos, &g_strVodSesBusy)
    {
        /*get the node*/
        pTempSession = HI_LIST_ENTRY(pos,VOD_SESSION_S,vod_sess_list);

        /*check session type and id*/
        if(enSessType == pTempSession->enSessType
           && 0 == strcmp(pas8SessID, pTempSession->aszSessID))
        {
            printf("HI_VOD_SessionFind--enSessType=%d,pas8SessID=%s,pTempSession->aszSessID=%s!\n",
                enSessType,pas8SessID,pTempSession->aszSessID);
            *ppSession = pTempSession;
            s32Ret = HI_SUCCESS;
            break;
        }
    }
    (HI_VOID)pthread_mutex_unlock(&g_VodSesListLock); 

    return s32Ret;
}

/*all busy session destroy: */
HI_VOID HI_VOD_SessionAllDestroy()
{
    //VOD_SESSION_S* pTempSession = NULL;
    List_Head_S *pos = NULL, *n = NULL;

    /*lock the destroy process*/
    (HI_VOID)pthread_mutex_lock(&g_VodSesListLock);
    
    /*get every busy session and destroy them*/
    HI_List_For_Each_Safe(pos, n, &g_strVodSesBusy)
    {
        /*del the node to busy list，and add to free list*/
        HI_List_Del(pos);
        //HI_List_Add(pos, &g_strVodSesFree);
    }

    //printf("HI_VOD_SessionAllDestroy ok\n");
    /*free vod session list*/
    //HI_VOD_FREE(g_astrVodSess);
    //g_astrVodSess = NULL;
    
    (HI_VOID)pthread_mutex_unlock(&g_VodSesListLock);

}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
