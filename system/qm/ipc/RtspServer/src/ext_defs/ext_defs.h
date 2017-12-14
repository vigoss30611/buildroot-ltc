#ifndef __HI_EXT_DEFS_H__
#define __HI_EXT_DEFS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#if !defined(LOG_LEVEL_EMERG)
#define LOG_LEVEL_FATAL     0    /* just for compatibility with previous version */

#define LOG_LEVEL_EMERG     0       /* system is unusable */
#define LOG_LEVEL_ALERT     1       /* action must be taken immediately */
#define LOG_LEVEL_CRIT      2       /* critical conditions */
#define LOG_LEVEL_ERR       3       /* error conditions */
#define LOG_LEVEL_ERROR     3       /* error conditions                             */
#define LOG_LEVEL_WARNING   4       /* warning conditions */
#define LOG_LEVEL_NOTICE    5       /* normal but significant condition */
#define LOG_LEVEL_INFO      6       /* informational */
#define LOG_LEVEL_DEBUG     7       /* debug-level messages */

#define LOG_LEVEL_TRACE     8       /*for trace only, if want to use this */
#endif

#ifdef MT_SYSLOG
#define HI_LOG_SYS(pszMode,u32LogLevel, args ...) syslog(u32LogLevel, args)
    #ifdef DEBUG_ON
        #define HI_LOG_DEBUG(pszMode,u32LogLevel, args ...) syslog(u32LogLevel, args)
    #else
        #define HI_LOG_DEBUG(pszMode,u32LogLevel, args ...)
    #endif    
#else
#define HI_LOG_SYS(pszMode,u32LogLevel, args ...) printf(args)
    #ifdef DEBUG_ON
        #define HI_LOG_DEBUG(pszMode,u32LogLevel, args ...) printf(args)
    #else 
        #define HI_LOG_DEBUG(pszMode,u32LogLevel, args ...)
    #endif            
#endif

#if HI_OS_TYPE == HI_OS_LINUX
     #define HI_Close close
#elif HI_OS_TYPE == HI_OS_WIN32
      #define HI_Close _close
#else
    #error YOU MUST DEFINE HI OS TYPE HI_OS_TYPE=HI_OS_WIN32 OR HI_OS_LINUX !    
#endif



//typedef void*              HI_ADDR;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
