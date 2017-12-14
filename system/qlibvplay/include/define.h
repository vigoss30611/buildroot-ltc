#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <qsdk/vplay.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <mutex>
using namespace std;
enum  ThreadState {
    THREAD_IDLE  = 0,
    THREAD_STARTED ,
    THREAD_STOP_PENDING,
    THREAD_STOPPED
};
typedef enum  {
    VIDEO_TYPE_MJPEG ,
    VIDEO_TYPE_H264 ,
    VIDEO_TYPE_HEVC ,
}video_type_t  ;

#define MEDIA_MUXER_HUGE_TIMESDTAMP  0xEFFFFFFFFFFFFFFFull
#define MEDIA_END_DUMMY_FRAME   0xFFFF

#ifndef RECORDER_CACHE_SECOND
#define RECORDER_CACHE_SECOND  3
#endif

#ifndef PLAYER_CACHE_SECOND
#define PLAYER_CACHE_SECOND  1
#endif

#define MAKE_STRING2(x)  #x
#define MAKE_STRING(x)  MAKE_STRING2(x)

#pragma message("############ RECORDER CACHE SECOND->" MAKE_STRING(RECORDER_CACHE_SECOND))
#pragma message("############ PLAYER CACHE SECOND->" MAKE_STRING(PLAYER_CACHE_SECOND))
/*
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#ifdef __cplusplus
}
#endif
*/

#define LOGGER_HANDLER     stderr

#define LOGGER_PRI_ERR         5
#define LOGGER_PRI_WARN     4
#define LOGGER_PRI_INFO     3
#define LOGGER_PRI_DBG         2
#define LOGGER_PRI_TRC        1

#ifndef  LOGGER_LEVEL_SET
    #error "set default error"
    #define LOGGER_LEVEL_SET    ERR
#endif

    #if LOGGER_LEVEL_SET == LOGGER_PRI_ERR
        #define LOGGER_PRI_SET  LOGGER_PRI_ERR
    #elif  LOGGER_LEVEL_SET  == LOGGER_PRI_WARN
        #define LOGGER_PRI_SET  LOGGER_PRI_WARN
    #elif  LOGGER_LEVEL_SET  == LOGGER_PRI_DBG
        #define LOGGER_PRI_SET  LOGGER_PRI_DBG
    #elif  LOGGER_LEVEL_SET  == LOGGER_INFO
        #define LOGGER_PRI_SET  LOGGER_PRI_INFO
    #elif  LOGGER_LEVEL_SET  == LOGGER_PRI_TRC
        #define LOGGER_PRI_SET  LOGGER_PRI_TRC
    #else
        #define LOGGER_PRI_SET  LOGGER_PRI_INFO
    #endif



#pragma message("############ LOG LEVEL->" MAKE_STRING(LOGGER_PRI_SET))


#define LOGGER_ERR(args ...)  if(LOGGER_PRI_SET <= LOGGER_PRI_ERR){ \
    fprintf(LOGGER_HANDLER,"ERROR->%s:%3d:",__FILE__,__LINE__);\
    fprintf(LOGGER_HANDLER ,args ); }

#define LOGGER_WARN(args ...)  if(LOGGER_PRI_SET <= LOGGER_PRI_WARN){ \
    fprintf(LOGGER_HANDLER,"WARN ->%s:%3d:",__FILE__,__LINE__);\
    fprintf(LOGGER_HANDLER ,args ); }

#define LOGGER_INFO(args ...)  if(LOGGER_PRI_SET <= LOGGER_PRI_INFO){ \
    fprintf(LOGGER_HANDLER,"INFO ->%s:%3d:",__FILE__,__LINE__);\
    fprintf(LOGGER_HANDLER ,args ); }

#define LOGGER_DBG(args ...)  if(LOGGER_PRI_SET <= LOGGER_PRI_DBG){ \
    fprintf(LOGGER_HANDLER,"DEBUG->%s:%3d:",__FILE__,__LINE__);\
    fprintf(LOGGER_HANDLER ,args ); }

#define LOGGER_TRC(args ...)  if(LOGGER_PRI_SET <= LOGGER_PRI_TRC){ \
    fprintf(LOGGER_HANDLER,"TRACE->%s:%3d:",__FILE__,__LINE__);\
    fprintf(LOGGER_HANDLER ,args ); }


#define  FREE_MEM_P(p)   if(1){\
                if(p != NULL){\
                    free(p);\
                    p = NULL;\
                }\
            }

#define  DELETE_OBJ_P(p)   if(1){\
                if(p != NULL){\
                    delete p;\
                    p = NULL;\
                }\
            }

int save_tmp_file(int index, void *buf, int size, char *prefix); //0 not write ,just skip ;>0 write size ;<0 :error
uint64_t  get_timestamp(void);
int vplay_report_event (vplay_event_enum_t event , char *dat)  ;
typedef void (*ErrorCallback)(void *arg, void *);
uint32_t calCrc32(  char *buf, uint32_t size);
void vplay_dump_buffer(char *buf, int size ,const char *name);
#endif
