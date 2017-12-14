#ifndef  _DEV_ERROR_H_
#define  _DEV_ERROR_H_
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "QMAPIType.h"
/************************************************************************/
/* 系统消息打印输出处理                                                 */
/************************************************************************/

#define PRINT_TO_NULL              0x0000  //不输出
#define PRINT_TO_TERMINAL          0x0001  //输出到终端
#define PRINT_TO_FILE              0x0002  //输出到文件,保存到系统程序运行日志文件
#define PRINT_TO_NET               0x0004  //输出到telnet登录用户

#define PRINT_LEVEL_DEBUG           0x0001
#define PRINT_LEVEL_TRACE           0x0002
#define PRINT_LEVEL_WARN            0x0004
#define PRINT_LEVEL_ERROR           0x0008

#define PRINT_LEVEL_ALL	          0x000F

 void DMS_SetPrintfMode(DWORD dwMode, DWORD nLevel);
 DWORD  DMS_GetPrintfMode();
/*函数将参数中的信息输出到指定的地方*/
 void DMS_Printf(DWORD nLevel, char *_FILE, const char *FUNCTION, int LINE, const char *format, ...);

 void DMS_SetLastError(int nErrorID);
 int  DMS_GetLastError();
 char *DMS_GetStrError(int nErrorID);

 void  DMS_ErrPrintfEx(int nErrorID, char *FILE, const char *FUNCTION, int LINE);

#if 1

#ifndef DMS_TRACE
#define DMS_TRACE(level, fmt, args...)\
	do{\
		char buf[128];\
		sprintf(buf, fmt,##args); \
		DMS_Printf(level, __FILE__, __FUNCTION__, __LINE__, buf); \
	}while(0)
#endif

#else

#ifndef DMS_TRACE
#define DMS_TRACE(level, fmt,args...)
#endif

#endif

#ifndef DMS_ErrPrintf
#define DMS_ErrPrintf(nErrorID) do{ DMS_ErrPrintfEx(nErrorID, __FILE__, __FUNCTION__, __LINE__);}while(0)
#endif


#ifndef DMS_ErrReturn
#define DMS_ErrReturn(nErrorID) do{ if(nErrorID != 0) DMS_ErrPrintfEx(nErrorID, __FILE__, __FUNCTION__, __LINE__); return nErrorID;}while(0)
#endif

#ifndef DMS_Assert
#define DMS_Assert(nErrorID) do{ if(nErrorID != 0) { DMS_ErrPrintfEx(nErrorID, __FILE__, __FUNCTION__, __LINE__); return nErrorID; } }while(0)
#endif

#ifndef DMS_Valid
#define DMS_Valid( value ) do{ if( value == 0) { DMS_ErrPrintfEx(errno, __FILE__, __FUNCTION__, __LINE__); } }while(0)
#endif

#ifndef DMS_err
#define DMS_err() do{ DMS_ErrPrintfEx(errno, __FILE__, __FUNCTION__, __LINE__);}while(0)
#endif

#ifndef DMS_CHECK_RES
#define DMS_CHECK_RES(nRes) do { if(nRes != 0) DMS_ErrPrintfEx(nRes, __FILE__, __FUNCTION__, __LINE__); } while (0);
#endif

#ifndef DMS_CHECK
#define DMS_CHECK(express,name)\
    do{\
	 int		nRet;\
	nRet = express;\
        if (QMAPI_SUCCESS != nRet)\
        {\
            DMS_TRACE(PRINT_LEVEL_ERROR, "%s failed(0x%x) at %s: LINE: %d !\n", name,nRet, __FUNCTION__, __LINE__);\
            return nRet;\
        }\
    }while(0)
#endif    

#endif //_DEV_ERROR_H_
