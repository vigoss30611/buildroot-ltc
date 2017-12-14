/**
 \file hi_buf_define.h
 \brief buffer
 \author Shenzhen Hisilicon Co., Ltd.
 \date 2010 - 2050
 \version 草稿
 \author 
 */

#ifndef __HI_BUF_DEFINE_H__
#define __HI_BUF_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "MediaBuffer.h"

/*----------------------------------------------*
* 宏定义
*----------------------------------------------*/

/** @defgroup Buffer_API_macro Buffer_API模块
 *  @ingroup H2
 *  @brief 详细描述模块(Buffer_API)宏定义
 *  @{  */

/**最大输入块大小*/

/**Buffer中没读到码流错误码，外面可继续循环读*/
#define BUF_NO_DATA 1

/** @} */  /*! <!-- Macro Definition end */

/*----------------------------------------------*
* 类型/结构定义
*----------------------------------------------*/

/** @defgroup Buffer_API_types Buffer_API模块
 *  @ingroup H2
 *  @brief 详细描述模块(Buffer_API)类型定义
 *  @{  */

//#define MAX_CHANNEL_NUM 4
//#define VS_STREAM_TYPE_BUTT 5

 
/**Buf码流信息*/
typedef struct hiBUF_STREAM_INFO_S
{
    HI_U8 *pu8BufferStart;/**<缓冲区首地址*/
    HI_U32 pu8BufferStartPhy;/**<缓冲区首地址物理地址*/
    HI_U32 u32BufferSize;/**<缓冲区大小*/
    BUF_BLOCK_INFO_S *pstBlockInfo;/**<缓冲区各个块(64K一个块)的相关信息*/
    HI_U32 u32WritePos;/**<写指针*/
    HI_U32 u32WriteCycle;/**<写的圈数*/
} BUF_STREAM_INFO_S;

/**I帧管理*/
typedef struct hiIFRAME_MNG_S
{
    HI_U32 u32ExpectNumber;/**<the expectant number of I-frame*/
    HI_U32 u32ValidNumber;/**<the valid I-frame number*/
    HI_U32 u32OldestIFrameIndex;/**<the oldest I-frame index(the index of I-frame position array)*/
    HI_U32 *pu32Position;/**<I-frame position*/
    HI_U32 *pu32Cycle;/**<圈数*/
} BUF_IFRAME_MNG_S;

/** @} */  /*! <!-- types Definition end */


HI_S32 VS_BUF_GetReadPtrPos(HI_S32 s32ReaderId,
                           BUF_POSITION_POLICY_E enPolicy,
                           HI_S32 s32N,
                           BUF_READPTR_INFO_S *pstReadptr);
						   
HI_S32 VS_BUF_SetReadPtrPos(HI_S32 s32ReaderId,
                               HI_U32 u32Len);
							   
HI_S32 VS_BUF_CanRead(HI_S32 s32ReaderId, HI_U32 u32Len);

HI_S32 VS_BUF_CheckReaderPacketNo(HI_S32 s32ReaderId,
                     HI_BOOL bVideo , HI_U32 u32PacketNo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /*__HI_BUF_DEFINE_H__*/




