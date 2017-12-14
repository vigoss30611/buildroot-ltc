


#ifndef __HI_BUFFER_ABSTRACT_DEFINE__
#define __HI_BUFFER_ABSTRACT_DEFINE__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "hi_mbuf.h"



/**单通道多用户模式下，对buffer的配置信息*/
typedef struct hiLIVE_1CHNnUSER_BUFINFO
{
	HI_S32 chnnum;   /**通道数*/
	MBUF_CHENNEL_INFO_S *pMbufCfg;/**所有通道能点播的用户数据，以及每个用户的buffer信息*/
}LIVE_1CHNnUSER_BUFINFO_S;

/**单通道单用户模式下，对发送buffer的配置信息*/
typedef struct hiLIVE_1CHN1USER_BUFINFO
{
	HI_S32 maxUserNum;  /**支持的用户最大数*/
	HI_S32 maxFrameLen;/**最大的帧长度*/
}LIVE_1CHN1USER_BUFINFO_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif



