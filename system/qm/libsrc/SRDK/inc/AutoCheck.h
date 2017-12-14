#ifndef _AUTO_CHECK_H_
#define _AUTO_CHECK_H_

#include "QMAPINetSdk.h"
#include "DMSErrno.h"
#include "wireless.h"

typedef struct tagAUTOCHECKCMDNODE
{
    int rSock;
    char szBuff[512];
    struct tagAUTOCHECKCMDNODE *pNext;
}CAutoCheckCmdNode;
CAutoCheckCmdNode* AutoCheckCmdNode_AddTail(char *pData, int len, int rSock);

#define     CMD_TEST_UDISK_OK            0x00000000
#define     CMD_TEST_UDISK_NO_UDISK     0x00000001
#define     CMD_TEST_UDISK_WRITE_ERROR  0x00000002
typedef struct tagRESULT_TEST_UDISK
{
    int errorCode;
    unsigned long udiskSize;
}RESULT_TEST_UDISK;
typedef struct tagRESULT_TEST_ALARM_IN
{
    int alarmInChnNum;
    int alarmInChnState;//按位，1:报警，0:
}RESULT_TEST_ALARM_IN;

typedef struct tagTEST_DEVICE_INFO
{
	DWORD dwSize;
	char  DevType[24];				//设备类型
	DWORD dwServerIp;				//ip地址
	DWORD dwDataPort;				//数据端口
	DWORD dwWebPort;				//web端口
	DWORD dwChannelNum;				//通道数
	DWORD dwCPUType;				//cpu类型
	BYTE  bServerSerial[48];		//设备序列号
	BYTE  byMACAddr[6];				//设备MAC地址
	DWORD dwAlarmInCount;			//报警输入个数
	DWORD dwAlarmOutCount;			//报警输出个数
	DWORD dwLanguageSupport;		//设备支持的语言
	char  cDeviceVersion[32];		//设备版本号
	DWORD dwVideoType;				//0:PAL,1:NTSC
}TEST_DEVICE_INFO;
typedef struct tagTEST_NET_PACK_HEAD
{
	DWORD	nFlag;
	DWORD	nCommand;
	DWORD	nChannel;
	DWORD	nErrorCode;
	DWORD	nBufSize;
}TEST_NET_PACK_HEAD;
typedef struct tagTEST_DEFAULT
{
	DWORD	dwSize;
	DWORD	dwLanguage;
	DWORD	dwVideoType;
}TEST_DEFAULT;

void DMS_Log_System_Start(QMAPI_TIME *pTime);
void DMS_Log_System_Run(QMAPI_TIME *pTime);
void DMS_Log_System_Reboot(QMAPI_TIME *pTime);
void DMS_Log_System_Get_Log(int iSocket);

#endif


