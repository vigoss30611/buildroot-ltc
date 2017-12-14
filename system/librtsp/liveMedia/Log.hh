#ifndef _LOG_HH
#define _LOG_HH

//#define DEBUG

#ifndef TAG
#define TAG "live555"
#endif

#define LOGE(fmt, arg...)   printf("ERROR: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#ifdef SHOW_WARNING
#define LOGW(fmt, arg...)   printf("WARNING: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#else
#define LOGW while(0);
#endif
#ifdef DEBUG
#define LOGD(fmt, arg...)   printf("DEBUG: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#define LOGI(fmt, arg...)   printf("INFOR: [%s], %s, %s, %d: "fmt, TAG, __FILE__, __FUNCTION__, __LINE__, ##arg);
#else
#define LOGD while(0);
#define LOGI while(0);
#endif

#endif

