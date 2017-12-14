
#ifndef __HI_COMM_LOG_H__
#define __HI_COMM_LOG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



#define LOG_LEVEL_EMERG     0       /* system is unusable */
#define LOG_LEVEL_ALERT     1       /* action must be taken immediately */
#define LOG_LEVEL_CRIT      2       /* critical conditions */
#define LOG_LEVEL_ERR       3       /* error conditions */
#define LOG_LEVEL_WARNING   4       /* warning conditions */
#define LOG_LEVEL_NOTICE    5       /* normal but significant condition */
#define LOG_LEVEL_INFO      6       /* informational */
#define LOG_LEVEL_DEBUG     7       /* debug-level messages */

#define LOG_LEVEL_TRACE     8       /*for trace only, if want to use this 
                                      function , please modify to 7 */

#define MOD_MAL         "mal"
#define MOD_ALM         "alm"
#define MOD_FILENMG     "filemng"
#define MOD_REC         "rec"

/*HI_DEBUG 在后续会实现*/
#define HI_Debug(Module, Level, args ...) printf(args)

#define HI_Info(Module, Level, args ...) printf(args)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_COMML_LOG_H__ */
