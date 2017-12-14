#ifndef __SMARTRC_LOG_H__
#define __SMARTRC_LOG_H__

/*#ifdef __cplusplus
extern "C" {
#endif
*/
//#define  SMARTRC_DEBUG

#ifdef SMARTRC_DEBUG
#define LOGD(arg...)        printf(arg) 
#else
#define LOGD(arg...)
#endif

#define LOGE(arg...)     printf(arg)
#define LOGI(arg...)     printf(arg)

/*
#ifdef __cplusplus
}
#endif
*/
#endif
