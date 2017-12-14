/*****************************************************************************
 * info_log.h
 *
 * log utility.
 * Copyright (c) 2014 Infotm. All rights reserved.
 * Created on: 2014-11-20
 * Author: fengwu
 * *****************************************************************************/

#ifndef  _INFO_LOG_H
#define _INFO_LOG_H


#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>


typedef enum
{
    LOG_MODE_MIN = 0,
    LOG_MODE_NONE, //no extra info
    LOG_MODE_TAG,
    LOG_MODE_FILE_LINE, //only include file and line
    LOG_MODE_FUNC_LINE,
    LOG_MODE_ALL, //include all info
    LOG_MODE_MAX
} ENUM_LOG_MODE;

typedef enum
{
    LOG_CLS_MIN = 0,
    LOG_CLS_COMMON = 0,
    LOG_CLS_SHM = 1,
    LOG_CLS_PIPELINE = 2,
    LOG_CLS_IPC  = 3,
    LOG_CLS_TEST = 4 ,
    LOG_CLS_SAVE = 5 ,
    LOG_CLS_VIDEO = 6 ,
    LOG_CLS_BUF  = 7,
    LOG_CLS_V4L2  = 8,
    LOG_CLS_WIS  = 9,
    LOG_CLS_SYS = 10,
    LOG_CLS_GUI = 11,
    LOG_CLS_AUDIO =12,
    LOG_CLS_MAX ,
} LOG_CLS;

/*
 *      LOG_LVL_VERBOSE: debug purpose
 *      LOG_LVL_I : normal
 *      LOG_LVL_ERROR : clean
 */
typedef enum
{
    LOG_LVL_MIN = 0,
    LOG_LVL_V,
    LOG_LVL_I,
    LOG_LVL_E,
    LOG_LVL_MAX
}LOG_LEVEL;


extern ENUM_LOG_MODE LogMod;
extern LOG_LEVEL *LogLevel;
extern char* LogClsNm[LOG_CLS_MAX];


#ifdef __cplusplus 
extern "C" {
#endif

int Log_init();
int Log_get_mode();
void Log_set_mode(int mode);
int Log_get_level(int class1);
void Log_set_level(int class1, int level);

char* getFileFromPath(char* inputDir);
void LogFunc(LOG_LEVEL level, char* tag, const char* filename, const char* funcName, int line, char *logbuf  );
void LOG_PRT_HEX(unsigned char*dataIn, int lenIn, int maxprtIn) ;
void Log_debug_level();

#ifdef __cplusplus 
}
#endif


// for class common
#define LOGV(...) LOG_WCLASS(LOG_CLS_COMMON, LOG_LVL_V, __VA_ARGS__)
#define LOGI(...) LOG_WCLASS(LOG_CLS_COMMON, LOG_LVL_I,    __VA_ARGS__)
#define LOGE(...) LOG_WCLASS(LOG_CLS_COMMON, LOG_LVL_E,   __VA_ARGS__)

// for class SHM
#define LOGV_SHM(...) LOG_WCLASS(LOG_CLS_SHM, LOG_LVL_V, __VA_ARGS__)
#define LOGI_SHM(...) LOG_WCLASS(LOG_CLS_SHM, LOG_LVL_I,    __VA_ARGS__)
#define LOGE_SHM(...) LOG_WCLASS(LOG_CLS_SHM, LOG_LVL_E,   __VA_ARGS__)

// for class pipeline
#define LOGV_PIPE(...) LOG_WCLASS(LOG_CLS_PIPELINE, LOG_LVL_V, __VA_ARGS__)
#define LOGI_PIPE(...) LOG_WCLASS(LOG_CLS_PIPELINE, LOG_LVL_I,    __VA_ARGS__)
#define LOGE_PIPE(...) LOG_WCLASS(LOG_CLS_PIPELINE, LOG_LVL_E,   __VA_ARGS__)

// for class IPC
#define LOGV_IPC(...) LOG_WCLASS(LOG_CLS_IPC, LOG_LVL_V, __VA_ARGS__)
#define LOGI_IPC(...) LOG_WCLASS(LOG_CLS_IPC, LOG_LVL_I,    __VA_ARGS__)
#define LOGE_IPC(...) LOG_WCLASS(LOG_CLS_IPC, LOG_LVL_E,   __VA_ARGS__)

// for class TEST
#define LOGV_TEST(...) LOG_WCLASS(LOG_CLS_TEST, LOG_LVL_V, __VA_ARGS__)
#define LOGI_TEST(...) LOG_WCLASS(LOG_CLS_TEST, LOG_LVL_I,	 __VA_ARGS__)
#define LOGE_TEST(...) LOG_WCLASS(LOG_CLS_TEST, LOG_LVL_E,	 __VA_ARGS__)

// for class SAVE
#define LOGV_SAV(...) LOG_WCLASS(LOG_CLS_SAVE, LOG_LVL_V, __VA_ARGS__)
#define LOGI_SAV(...) LOG_WCLASS(LOG_CLS_SAVE, LOG_LVL_I,	 __VA_ARGS__)
#define LOGE_SAV(...) LOG_WCLASS(LOG_CLS_SAVE, LOG_LVL_E,	 __VA_ARGS__)

// for class Video
#define LOGV_VID(...) LOG_WCLASS(LOG_CLS_VIDEO, LOG_LVL_V, __VA_ARGS__)
#define LOGI_VID(...) LOG_WCLASS(LOG_CLS_VIDEO, LOG_LVL_I,	 __VA_ARGS__)
#define LOGE_VID(...) LOG_WCLASS(LOG_CLS_VIDEO, LOG_LVL_E,	 __VA_ARGS__)


// for class BUFFER
#define LOGV_BUF(...) LOG_WCLASS(LOG_CLS_BUF, LOG_LVL_V, __VA_ARGS__)
#define LOGI_BUF(...) LOG_WCLASS(LOG_CLS_BUF, LOG_LVL_I,    __VA_ARGS__)
#define LOGE_BUF(...) LOG_WCLASS(LOG_CLS_BUF, LOG_LVL_E,   __VA_ARGS__)

// for class V4L2
#define LOGV_V4L2(...) LOG_WCLASS(LOG_CLS_V4L2, LOG_LVL_V, __VA_ARGS__)
#define LOGI_V4L2(...) LOG_WCLASS(LOG_CLS_V4L2, LOG_LVL_I,    __VA_ARGS__)
#define LOGE_V4L2(...) LOG_WCLASS(LOG_CLS_V4L2, LOG_LVL_E,   __VA_ARGS__)

// for class WIS
#define LOGV_WIS(...) LOG_WCLASS(LOG_CLS_WIS, LOG_LVL_V, __VA_ARGS__)
#define LOGI_WIS(...) LOG_WCLASS(LOG_CLS_WIS, LOG_LVL_I,    __VA_ARGS__)
#define LOGE_WIS(...) LOG_WCLASS(LOG_CLS_WIS, LOG_LVL_E,   __VA_ARGS__)

// for class SYS
#define LOGV_SYS(...) LOG_WCLASS(LOG_CLS_SYS, LOG_LVL_V, __VA_ARGS__)
#define LOGI_SYS(...) LOG_WCLASS(LOG_CLS_SYS, LOG_LVL_I, __VA_ARGS__)
#define LOGE_SYS(...) LOG_WCLASS(LOG_CLS_SYS, LOG_LVL_E, __VA_ARGS__)

// for class GUI
#define LOGV_GUI(...) LOG_WCLASS(LOG_CLS_GUI, LOG_LVL_V, __VA_ARGS__)
#define LOGI_GUI(...) LOG_WCLASS(LOG_CLS_GUI, LOG_LVL_I, __VA_ARGS__)
#define LOGE_GUI(...) LOG_WCLASS(LOG_CLS_GUI, LOG_LVL_E, __VA_ARGS__)

// for class AUDIO
#define LOGV_AUDIO(...) LOG_WCLASS(LOG_CLS_AUDIO, LOG_LVL_V, __VA_ARGS__)
#define LOGI_AUDIO(...) LOG_WCLASS(LOG_CLS_AUDIO, LOG_LVL_I, __VA_ARGS__)
#define LOGE_AUDIO(...) LOG_WCLASS(LOG_CLS_AUDIO, LOG_LVL_E, __VA_ARGS__)

// low level macro, don't use.
#define LOG_WCLASS(logcls, loglevel,  ...) {\
    if(Log_get_level(logcls)<= loglevel) \
    {\
        char logbuf[2000]; \
        sprintf(logbuf,  __VA_ARGS__);  \
        LogFunc(loglevel, LogClsNm[logcls], __FILE__, __FUNCTION__, __LINE__, logbuf);\
    }\
}

// inc_cnt: count to increase
// log_sec: print interval
#define DEBUG_LOG_CNT(inc_cnt, log_sec, ...)  \
{\
    static int cnt_debug_total=0; \
    static int cnt_debug_last=0; \
    static struct timeval tvStart;\
    static struct timeval tvLast;\
    struct timeval tvnow;\
    char logbuf[2000]; \
    logbuf[0]='\0';\
    sprintf(logbuf,  __VA_ARGS__);  \
    if(cnt_debug_total==0) {\
        gettimeofday(&tvStart, NULL);\
        gettimeofday(&tvLast, NULL);\
    }\
    cnt_debug_total += (inc_cnt);\
    gettimeofday(&tvnow, NULL);\
    unsigned int timeDiffLast = tvnow.tv_sec - tvLast.tv_sec;\
    unsigned int timeDiffTotal = tvnow.tv_sec - tvStart.tv_sec;\
    unsigned int cntDiffLast = cnt_debug_total - cnt_debug_last;\
    if(cnt_debug_total < cnt_debug_last || cnt_debug_total<0 || cnt_debug_last<0)\
    {\
        cnt_debug_total=0;\
        cnt_debug_last=0;\
        tvStart.tv_sec=0;\
        tvLast.tv_sec=0;\
    }\
    else if(timeDiffLast > log_sec )\
    {\
        int cnt_av_total = cnt_debug_total/(tvnow.tv_sec - tvStart.tv_sec);\
        int cnt_av_last = cntDiffLast/(tvnow.tv_sec - tvLast.tv_sec);\
        (void)printf("%s. average:%d. total_cnt:%d. Total_average:%d. ", \
                logbuf, cnt_av_last, cnt_debug_total, cnt_av_total ); \
        tvLast.tv_sec=tvnow.tv_sec;\
        cnt_debug_last=cnt_debug_total;\
    }\
}

#define TIMED_LOG(seconds, ...) \
{\
    char logbuf[2000]; \
    sprintf(logbuf,  __VA_ARGS__);  \
    static struct timeval tvLast={0,0};\
    struct timeval tvnow;\
    gettimeofday(&tvnow, NULL);\
    if(tvLast.tv_sec==0) {\
        gettimeofday(&tvLast, NULL);\
    }\
    if(tvnow.tv_sec-tvLast.tv_sec > seconds)\
    {\
        (void)printf(LOG_LVL_I, "KiQOS", "%s:%d - %s", \
                getFileFromPath((char*)__FILE__), \
                __LINE__,logbuf);\
        gettimeofday(&tvLast, NULL);\
    }\
}

#define COUNTED_LOG(count, ...) \
{\
    char logbuf[2000]; \
    sprintf(logbuf,  __VA_ARGS__);  \
    static int cnt = 0;\
    cnt++;\
    if((cnt % count) == 0 )\
    {\
        (void)printf(LOG_LVL_I, "KiQOS", "%s:%d(cnt:%d) - %s", \
                getFileFromPath((char*)__FILE__), \
                __LINE__,cnt, logbuf);\
    }\
}

// this macro will print error percentage like packet lost rate
// parameters:
// succ_cnt - success cnt to increase
// err_cnt - error cnt to increase
// log_sec - time to calculate percentage
#define ERROR_PERCENTAGE_LOG(succ_cnt, err_cnt, log_sec, ...)  \
{\
    static int cnt_sucss_total=0; \
    static int cnt_error_total=0; \
    static struct timeval tvStart;\
    struct timeval tvnow;\
    char logbuf[2000]; \
    logbuf[0]='\0';\
    sprintf(logbuf,  __VA_ARGS__);  \
    gettimeofday(&tvnow, NULL);\
    if(cnt_sucss_total==0 && cnt_error_total) {\
        gettimeofday(&tvStart, NULL);\
    }\
    cnt_sucss_total += (succ_cnt);\
    cnt_error_total += (err_cnt);\
    unsigned int timeDiffTotal = tvnow.tv_sec - tvStart.tv_sec;\
    unsigned int cntDiffLast = cnt_debug_total - cnt_debug_last;\
    if(timeDiffTotal >= log_sec && cnt_sucss_total>0)\
    {\
        float error_percentage = 100*cnt_error_total/(cnt_error_total+cnt_sucss_total);\
        (void)printf(LOG_LVL_I, "KiQOS", \
                "%s:%d - %s. percentage:\%%f. ", \
                getFileFromPath((char*)__FILE__), \
                __LINE__, logbuf, error_percentage ); \
        cnt_sucss_total=0; \
        cnt_error_total=0; \
    }\
}


#define CHECK_ELAPED_TIME(sec, ret_ptr )\
{\
    static struct timeval tvLast={0,0};\
    struct timeval tvnow;\
    gettimeofday(&tvnow, NULL);\
    if(tvLast.tv_sec==0) {\
        gettimeofday(&tvLast, NULL);\
        *ret_ptr = true;\
    }\
    else if((tvnow.tv_sec - tvLast.tv_sec) >= (sec))\
    {\
        *ret_ptr = true;\
        tvLast.tv_sec = tvnow.tv_sec;\
    }\
    else\
    {\
        *ret_ptr = false;\
    }\
}


/*
 * macro Name: CALULATE_FPS
 * Input:
 *      content2prt_in: module name to print
 *      timePrt_in: how long to print statistic. unit is second
 *      cnt_in: count input
 * output:
 *      every timePrt_in seconds passed, will print statistics
 */
#define CALULATE_FPS(content2prt_in, timePrt_in, cnt_in) \
{\
    static struct timeval timeStart ={0,0}; \
    static struct timeval timeLast= {0,0};\
    struct timeval timeNow; \
    static long frameCntTotal = 0;\
    static long frameCntLast  = 0;\
    frameCntTotal += cnt_in;\
    frameCntLast += cnt_in;\
    if(timeStart.tv_sec == 0)\
    {\
        gettimeofday(&timeStart, NULL);\
        gettimeofday(&timeLast, NULL);\
    }\
    gettimeofday(&timeNow, NULL);\
    long secDiffStart = timeNow.tv_sec - timeStart.tv_sec;\
    if(secDiffStart <= 0) secDiffStart = 1;\
    long secDiffLast  = timeNow.tv_sec - timeLast.tv_sec;\
    if(secDiffLast  <= 0) secDiffLast  = 1;\
    int fpsAll  = frameCntTotal / secDiffStart;\
    int fpsLast = frameCntLast  / secDiffLast ;\
    if(secDiffLast > timePrt_in || timeNow.tv_sec<timeStart.tv_sec)\
    {\
        printf("%s Total speed: %d /s, count:%ld, time:%ld sec.  --- LastSpeed:%d /s, cnt:%ld, time:%ld sec. LastIn:%d\n", \
                 content2prt_in, fpsAll, frameCntTotal, secDiffStart,\
                 fpsLast, frameCntLast, secDiffLast, cnt_in);\
        gettimeofday(&timeLast, NULL);\
        frameCntLast = 0;\
        if(timeNow.tv_sec<timeStart.tv_sec){\
            gettimeofday(&timeStart, NULL);\
            gettimeofday(&timeLast, NULL);\
        }\
    }\
}

#define TIME_DIFF(start, end) (end.tv_sec-start.tv_sec)*1000 + (end.tv_usec-start.tv_usec)/1000




#endif
