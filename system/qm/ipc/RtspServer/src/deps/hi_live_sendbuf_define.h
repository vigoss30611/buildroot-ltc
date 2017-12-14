
/***********************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_mbuf.c
* Description: The MBUF module.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2010-07-28    m56132 NULL         Create this file.
*
***********************************************************************************/

#ifndef __HI_LIVE_SENDBUF_DEFINE_H__
#define __HI_LIVE_SENDBUF_DEFINE_H__



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_mbuf_def.h"

typedef struct hiLIVE_SENDBUF_HEADER_S
{
    HI_BOOL bufferState;/**HI_TRUE:已被用，HI_FALSE:未被占用*/
    HI_S32 s32Chn;
	MBUF_VIDEO_STATUS_E enVideoState;
    HI_BOOL audioStat;
    HI_BOOL dataStat;

	HI_VOID  	*pStream;
	HI_VOID   		*pReadPos;
	HI_BOOL              		bFirstRead;
	HI_VOID				*pSendBufAddr;	
	HI_VOID              *pAudioBufAddr;//标记在上次获取视频流的时候音频数据指针
	HI_S32                s32AudioLen; //标记在上次获取视频流的时候音频数据长度
	HI_U32                u32TimeTick; //标记在上次获取视频流的时候音频时间戳
}LIVE_SENDBUF_HEADER_S;
 




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



#endif


