
#ifndef __HD_MNG_H__
#define __HD_MNG_H__


#include "QMAPINetSdk.h"

typedef enum _HD_EVENT_TYPE_E
{
	HD_EVENT_FORMAT,
	HD_EVENT_MOUNT,
	HD_EVENT_UNMOUNT,
	HD_EVENT_ERROR,		//检测到硬盘错误
	HD_EVENT_UNFORMAT,		//检测到硬盘未格式化
	HD_EVENT_BUTT
} HD_EVENT_TYPE_E;


typedef struct {
    HD_EVENT_TYPE_E event;
    int diskNo;
    void *priv;
} DISK_INFO_T; 
  
#define EVENT_HD_NOTIFY     "event_hd_notify"
  
typedef void(*HDmngCallback)(HD_EVENT_TYPE_E event, int DiskNo, int *pStatus);
typedef int(*HDmng_GetSdWriteProtectFun)();

void HdUnloadAllDisks();
int HdGetFormatStatus(QMAPI_NET_DISK_FORMAT_STATUS *pStatus);

/*
    获取存储设备信息，有存储设备返回0，没有存储设备的时候返回-1
*/
int HdGetDiskInfo(QMAPI_NET_HDCFG *pHdInfo);

int HdSetDiskInfo(int nDiskNo, int nStat);
int HdFormat(QMAPI_NET_DISK_FORMAT *pFormatInfo);
int QMAPI_Hdmng_Init(HDmngCallback fHdmng, HDmng_GetSdWriteProtectFun fWriteProtect);
int QMAPI_Hdmng_DeInit(void);

#endif /*__HD_MNG_H__*/

