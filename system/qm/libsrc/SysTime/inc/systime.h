
#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

int QMAPI_SysTime_Init(int nTimeZone);
int QMAPI_SysTime_UnInit();
/*
	获取系统时间
	设置系统时间
	获取时区
	设置时区
	启动NTP功能
	停止NTP功能
*/
int QMapi_SysTime_ioctrl(int Handle, int nCmd, int nChannel, void* Param, int nSize);

#endif
