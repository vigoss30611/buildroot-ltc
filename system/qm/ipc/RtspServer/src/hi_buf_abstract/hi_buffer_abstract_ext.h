/***********************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_buffer_abstract_ext.h
* Description: The MBUF module.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2010-07-30   m56132     NULL         Create this file.
***********************************************************************************/

#ifndef __HI_BUFFER_ABSTRACT_EXT_H__
#define __HI_BUFFER_ABSTRACT_EXT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "hi_buffer_abstract_define.h"
#include "hi_mbuf_def.h"
#include "hi_mt_def.h"


HI_S32 HI_BufAbstract_Init(int enLiveMode,HI_VOID* pBufInfo);

HI_S32 HI_BufAbstract_Deinit(int enLiveMode);

HI_S32 HI_BufAbstract_Alloc(int enLiveMode,HI_S32 ChanID, HI_VOID **pphandle);

HI_S32 HI_BufAbstract_Free(int enLiveMode,HI_VOID **pphandle);

HI_VOID HI_BufAbstract_Register(int enLiveMode,HI_VOID *phandle, MBUF_VIDEO_STATUS_E video_status, HI_BOOL audio_enable, HI_BOOL data_enable);

HI_VOID HI_BufAbstract_GetRegister(int enLiveMode,HI_VOID *phandle, MBUF_VIDEO_STATUS_E* pvideo_status, HI_BOOL* paudio_enable, HI_BOOL* pdata_enable);

HI_S32 HI_BufAbstract_Read(int enLiveMode,HI_VOID *phandle, HI_ADDR*paddr, HI_U32 *plen, 
            HI_U32 *ppts, MBUF_PT_E *ppayload_type,HI_U8 *pslice_type, 
            HI_U32 *pslicenum, HI_U32 *pframe_len);

HI_VOID HI_BufAbstract_Write(int enMode,HI_S32 ChanID, MBUF_PT_E payload_type, 
                   const HI_ADDR addr, HI_U32 len, HI_U64 pts, 
                   HI_U8 *pslice_type, HI_U32 slicenum, HI_U32 frame_len);

HI_VOID HI_BufAbstract_Set(int enLiveMode,HI_VOID *phandle);

HI_S32 HI_BufAbstract_GetCurPts(int enLiveMode,HI_VOID *phandle,HI_U64* pPts);

HI_S32 HI_BufAbstract_GetChn(int enLiveMode,HI_VOID *phandle,HI_S32* ps32Chn);

HI_VOID HI_BufAbstract_RequestIFrame(HI_S32 ChanID);

HI_S32 HI_BufAbstract_GetMediaInfo(HI_S32 ChanID, MT_MEDIAINFO_S *pMediaInfo);

HI_U8 HI_BufAbstract_GetAudioEnable(HI_S32 ChanID);

HI_S32 HI_BufAbstract_GetNetPt_13_Info(char *pSerialIndex);

HI_S32 BufAbstract_Playback_GetMediaInfo(char *csFileName, MT_MEDIAINFO_S *pstMtMediaoInfo);

HI_S32 BufAbstract_Playback_Alloc(int enLiveMode, char *csFileName, HI_VOID **pphandle);

HI_S32 BufAbstract_Playback_Free(int enLiveMode,HI_VOID **pphandle);

HI_S32 BufAbstract_Playback_Read(int enLiveMode,
    HI_VOID *phandle, HI_ADDR*paddr, HI_U32 *plen, 
    HI_U32 *ppts, MBUF_PT_E *ppayload_type);

HI_S32 BufAbstract_Playback_GetAudioEnable(char *csFileName);

int BufAbstract_Register_StunServer(char *pServerAddr, unsigned short port);
int BufAbstract_Deregister_StunServer();
int BufAbstract_Get_NatAddr(unsigned short MediaPort, unsigned short AdaptPort, char *pNatAddr, unsigned short *pMapedPortPair, int *pSocketPair);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif




