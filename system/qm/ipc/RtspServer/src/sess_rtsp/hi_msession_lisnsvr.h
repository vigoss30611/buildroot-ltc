
#ifndef _HI_MSESS_LISNSVR_H_
#define _HI_MSESS_LISNSVR_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "hi_type.h"
//#include "hi_log.h"

typedef enum hiLISNSVR_TYPE_E
{
    LISNSVR_TYPE_HTTP,
    LISNSVR_TYPE_RTSP,
    LISNSVR_TYPE_RTSPoHTTP,
    LISNSVR_TYPE_OWSP,
    LISNSVR_TYPE_BUTT
}LISNSVR_TYPE_E;

#define HI_ERR_MSESS_LISNSVR_SOCKET_ERR          HI_APP_DEF_ERR(HI_APPID_LISNSVR, LOG_LEVEL_ERROR, 0x01)
#define HI_ERR_MSESS_LISNSVR_LISNSVR_INIT        HI_APP_DEF_ERR(HI_APPID_LISNSVR, LOG_LEVEL_ERROR, 0x02)
#define HI_ERR_MSESS_LISNSVR_SELECT              HI_APP_DEF_ERR(HI_APPID_LISNSVR, LOG_LEVEL_ERROR, 0x03)
#define HI_ERR_MSESS_LISNSVR_ILLEGALE_PARAM      HI_APP_DEF_ERR(HI_APPID_LISNSVR, LOG_LEVEL_ERROR, 0x04)
#define HI_ERR_MSESS_LISNSVR_MALLOC              HI_APP_DEF_ERR(HI_APPID_LISNSVR, LOG_LEVEL_ERROR, 0x05)
#define HI_ERR_MSESS_LISNSVR_OVER_CONN_NUM       HI_APP_DEF_ERR(HI_APPID_LISNSVR, LOG_LEVEL_ERROR, 0x06)

/*Æô¶¯¼àÌý·þÎñ*/
HI_S32 HI_MSESS_LISNSVR_StartSvr(HI_S32 s32LisnPort,HI_S32 s32MaxConnNum, 
                                             HI_U32 **ppu32LisnSvrHandle,LISNSVR_TYPE_E enSvrType);

/*stop the http lisen svr*/
HI_S32 HI_MSESS_LISNSVR_StopSvr(HI_U32 *pu32LisnSvrHandle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

