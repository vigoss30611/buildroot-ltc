/****************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_mtrans_task_list.h
* Description: process on media transition trask list.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-02-13   W54723     NULL         creat this file.
*****************************************************************************/
#ifndef  __HI_MTRANS_TASK_LIST_H__
#define  __HI_MTRANS_TASK_LIST_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_list.h"
#include "hi_mtrans.h"
#include "hi_mtrans_internal.h"

#define MTRANS_DEFAULT_TASK_NUM 32  /*default trans task number*/
#define MTRANS_MAX_TASK_NUM 32  /*max trans task number*/

#define MTRANS_VIDEO_BITMAP 0x01                 /*媒体类型为视频类型时为0000 0001 */
#define MTRANS_AUDIO_BITMAP 0x02                 /*媒体类型为音频类型时为0000 0011 */
#define MTRANS_DATA_BITMAP  0x04                 /*媒体类型为data类型时为0000 0100 */
#define IS_MTRANS_VIDEO_TYPE(MediaType) ((MediaType)&(MTRANS_VIDEO_BITMAP))
#define IS_MTRANS_AUDIO_TYPE(MediaType) ((MediaType)&(MTRANS_AUDIO_BITMAP))
#define IS_MTRANS_DATA_TYPE(MediaType)  ((MediaType)&(MTRANS_DATA_BITMAP))
/*媒体类型MediaType是否包含seq对应的媒体项
seq:0 - 视频, 1-音频, 2- data */
#define IS_MTRANS_REQ_TYPE(MediaType,seq)   ((MediaType)&(0x01<<(seq)) )

HI_VOID DEBUGMTRANS_GetTaskListNum(HI_U32 *pu32FreeNum,HI_U32 *pu32BusyNum);

HI_BOOL MTRANS_TaskCheck(const MTRANS_TASK_S *pTaskHandle);

/*init a appointed send task*/
HI_S32 HI_MTRANS_TaskInit(MTRANS_TASK_S *pTaskHandle,const MTRANS_SEND_CREATE_MSG_S *pstSendInfo);

HI_VOID HI_MTRANS_TaskSetAttr(MTRANS_TASK_S *pTaskHandle,const MTRANS_SEND_CREATE_MSG_S *pstSendInfo);

/*create session: get a unused node from the free session list*/
HI_S32 HI_MTRANS_TaskCreat(MTRANS_TASK_S **ppTask );

HI_S32 HI_MTRANS_TaskStart(MTRANS_TASK_S *pTaskHandle, HI_U8 enMediaType);

HI_S32 HI_MTRANS_TaskStop(MTRANS_TASK_S *pTaskHandle, HI_U8 enMediaType);

/*free the resource the session possess: close socket,
  and remove the session from busy list*/
HI_S32 HI_MTRANS_TaskDestroy(MTRANS_TASK_S *pTaskHandle);

/*init send task list*/
HI_S32 HI_MTRANS_TaskListInit(HI_S32 s32TotalTaskNum);

/*all busy trans task destroy: */
HI_VOID HI_MTRANS_TaskAllDestroy();

HI_S32  HI_MTRANS_GetSvrTransInfo(MTRANS_TASK_S *pTaskHandle,HI_U8 u8MediaType,
                                MTRANS_ReSEND_CREATE_MSG_S *pstReSendInfo);

HI_VOID MTRANS_SetTaskState(MTRANS_TASK_S *pTaskHandle, HI_S32 MediaType,MTRANS_TASK_STATE_E enState);

HI_VOID MTRANS_GetTaskState(const MTRANS_TASK_S *pTaskHandle, HI_S32 MediaType,MTRANS_TASK_STATE_E* penState);
HI_S32 HI_MTRANS_TaskCreat_Multicast(MTRANS_TASK_S **ppTask, int channel, int streamType);
HI_S32 HI_MTRANS_TaskCreat_Multicast_ext(MTRANS_TASK_S **ppTask, int channel, int streamType, unsigned short vPort);
HI_S32 HI_MTRANS_GetMulticastTaskHandle(MTRANS_TASK_S **ppTask, int channel, int streamType, unsigned short vPort);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__HI_MTRANS_TASK_LIST_H__*/

