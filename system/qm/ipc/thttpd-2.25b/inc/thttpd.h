#ifndef __HI_THTTPD_H__
#define __HI_THTTPD_H__


#include "timers.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define HTTPD_CONFIG_PATH "/etc/thttpd.conf"

typedef int (*PTR_FUNC_DistribLink)(int s32LinkFd, char* pMsgBuffAddr,
                           unsigned int u32MsgLen);

void* thttpd_start_main(void* pvParam);
void thttpd_stop_main();

int thttpd_getport(void);

int thttpd_setStopFlag(void);

int thttpd_clearStopFlag(void);

int HI_THTTPD_RegisterDistribLink(PTR_FUNC_DistribLink pfunVodDistribLink,
    								PTR_FUNC_DistribLink pfunRTSPoHTTPDistribLink,
                                    PTR_FUNC_DistribLink pfunTalkDistribLink,
                                    PTR_FUNC_DistribLink pfunPlaybackDistribLink);

void occasional( ClientData client_data, struct timeval* nowP );

#define DEBUG_CGI_PRINT(x, y, ...) 

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_THTTPD_H__ */


