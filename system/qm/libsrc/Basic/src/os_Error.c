#include <stdio.h>
#include "QMAPIErrno.h"
#include "os_Error.h"

DWORD m_dwPrintMode = PRINT_TO_TERMINAL;
DWORD m_dwPrintLevel = PRINT_LEVEL_ALL;

int m_nErrorID = QMAPI_SUCCESS;

 void DMS_SetPrintfMode(DWORD dwMode, DWORD nLevel)
{
	m_dwPrintMode = dwMode;
	m_dwPrintLevel = nLevel;
}

 DWORD  DMS_GetPrintfMode()
{
	return m_dwPrintMode;
}

/*函数将参数中的信息输出到指定的地方*/
 void DMS_Printf(DWORD nLevel, char *_FILE, const char *FUNCTION, int LINE, const char *format, ...)
{
	if( (nLevel & m_dwPrintLevel) == 0 ) 
	{
		return;
	}
	if(m_dwPrintMode |PRINT_TO_TERMINAL)
	{
		printf("%s:%d [%s]  ", _FILE, LINE, FUNCTION);
		printf(format);
	}
	if(m_dwPrintMode |PRINT_TO_FILE)
	{
//		http_log("%s:%d [%s]  %s",_FILE, LINE, FUNCTION,format);
	}
}

 void DMS_SetLastError(int nErrorID)
{
	m_nErrorID = nErrorID;
}

 int  DMS_GetLastError()
{
	return m_nErrorID;
}

 char *DMS_GetStrError(int nErrorID)
{
	return "not support .";
}

 void DMS_ErrPrintfEx(int nErrorID, char *FILE, const  char *FUNCTION, int LINE)

{
	char buf[128];\
	sprintf(buf, "ErrorID(0x%X) %s\n",  nErrorID, strerror(nErrorID)); 
	DMS_Printf(PRINT_LEVEL_ERROR, FILE, FUNCTION, LINE, buf);
}
