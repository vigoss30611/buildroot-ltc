
#ifndef _HI_MSESS_RTSP_O_HTTP_H_
#define _HI_MSESS_RTSP_O_HTTP_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "hi_type.h"
#include "cgiinterface.h"

#define RTSP_O_HTTP_LISNSVR_FIRST_MSGBUFF_LEN 1024

typedef enum hiRTSP_O_HTTP_LIVESVR_STATE
{
    RTSP_O_HTTP_LIVESVR_STATE_STOP = 0,         /*准备阶段， 还没运行*/        
    RTSP_O_HTTP_LIVESVR_STATE_Running = 1,      /*准备阶段， 还没运行*/
    RTSP_O_HTTP_LIVESVR_STATE_BUTT              /*准备阶段， 还没运行*/
    
}RTSP_O_HTTP_LIVESVR_STATE;

typedef struct hiRTSP_O_HTTP_LIVE_SVR_S
{
    HI_U32* pu32RTSP_O_HTTPLisnSvrHandle;
    HI_U32* pu32RTSP_O_HTTPSessListHandle;
    RTSP_O_HTTP_LIVESVR_STATE enSvrState;      /*http live server state*/
    HI_U16  u16UserNum;                 /*current http user number,should less than RTSP_O_HTTP_MAX_SESS_NUM*/
    
    
}RTSP_O_HTTP_LIVE_SVR_S;

/*初始化http会话服务*/
HI_S32 HI_RTSP_O_HTTP_LiveSvr_Init(HI_S32 s32MaxConnNum);

/*去初始化http会话服务*/
HI_S32 HI_RTSP_O_HTTP_LiveSvr_DeInit();


/*创建会话任务，若第一个会话消息已接收则使pMsgBuffAddr指向消息首地址,
 否则置pMsgBuffAddr为null*/
HI_S32 HI_RTSP_O_HTTP_CreatHttpLiveSession(httpd_conn* hc);

/*接收一个由监听模块(webserver)分发的链接
 1 解析是否是http 直播连接
 2 解析请求的直播方法
 3 按不同的方式分别处理各请求方法*/
HI_S32 HI_RTSP_O_HTTP_DistribLink(httpd_conn* hc);


#ifndef DEBUG_RTSP_O_HTTP_LIVE
#define DEBUG_RTSP_O_HTTP_LIVE 8
#if (DEBUG_RTSP_LIVE == 0) || !defined(DEBUG_ON)
    #define DEBUG_RTSP_O_HTTP_LIVE_PRINT(pszModeName, u32Level, args ...)
#else
#define DEBUG_RTSP_O_HTTP_LIVE_PRINT(pszModeName, u32Level, args ...)   \
    do  \
    {   \
        if (DEBUG_RTSP_O_HTTP_LIVE >= u32Level)    \
        {   \
            printf(args);   \
        }   \
    }   \
    while(0)
#endif
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

