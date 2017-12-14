/*
2008-11-10 10:00 v0.1
*/

#include "hi_mt_def.h"
#ifndef  __HI_MEDIATRANS_H__
#define  __HI_MEDIATRANS_H__
#ifdef __cplusplus
#if __cplusplus

extern "C"
{
#endif
#endif /* __cplusplus */

__attribute__((visibility("default"))) int HI_MT_Init(MT_CONFIG_S *pMTConfig);

__attribute__((visibility("default"))) int HI_MT_StartSvr();

__attribute__((visibility("default"))) int HI_MT_StopSvr();

__attribute__((visibility("default"))) int HI_MT_Deinit();

__attribute__((visibility("default"))) int HI_MT_IsRunning() ;

/*如果其他监听应用收到点播连接，通过此接口分发给mt模块*/
//int HI_MT_RTSPoHTTP_DistribLink(int s32LinkFd, HI_CHAR* pMsgBuffAddr,
//                           HI_U32 u32MsgLen);

__attribute__((visibility("default"))) int MT_StartMulticastStreaming(MT_MulticastInfo_s *pInfo);

__attribute__((visibility("default"))) int MT_StopMulticastStreaming(MT_MulticastInfo_s *pInfo);

__attribute__((visibility("default"))) int MT_Register_StunServer(char *pServerAddr, unsigned short port);

__attribute__((visibility("default"))) int MT_Deregister_StunServer(void);

__attribute__((visibility("default"))) int MT_Rtsp_DistribLink(int sockfd, char *pMsg, int msgLen);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

