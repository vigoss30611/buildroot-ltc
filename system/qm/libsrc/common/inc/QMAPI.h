#ifndef __QMAPI_SYSNETAPI_H__
#define __QMAPI_SYSNETAPI_H__

#include "QMAPIType.h"
#include "QMAPINetSdk.h"
#include "QMAPIErrno.h"


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

//-------------回调函数信息------------------
/*********************************************************************************
* 函数介绍：数据处理回调
* 输入参数：channel: 通道号
			streamType:码流类型0,主码流，1,子码流
            data: 数据接收缓冲区
            size: 数据接收缓冲区大小
            flag: 0--主码流数据 1--子码流数据
            frame_header---帧头信息
* 输出参数：无
* 返回值  ：无
**********************************************************************************/
typedef int (*CBVideoProc)(int nChannel, int nStreamType, char* pData, unsigned int dwSize, QMAPI_NET_FRAME_HEADER stFrameHeader, DWORD dwUserData);

/*********************************************************************************
* 函数介绍：对讲音频数据处理回调
* 输入参数：pdata： 音频数据指针
            len： 音频数据大小
            param： 扩充指针暂不用
* 输出参数：无
* 返回值  ：
    0 --- 成功，<0 --- 失败
**********************************************************************************/
typedef int (*CBAudioProc)(int* pData, unsigned int dwSize, int nType, DWORD dwUserData);


/*********************************************************************************
* 函数介绍：报警信息处理回调
* 输入参数：nChannel
            nAlarmType：告警类型
            nAction：告警状态
* 输出参数：无
* 返回值  ：
    0 --- 成功，<0 --- 失败
**********************************************************************************/
typedef void (*CBAlarmProc)(int nChannel, int nAlarmType, int nAction, void* pParam);


/*********************************************************************************
* 函数介绍：回放处理回调
* 输入参数：PlayHandle：回放句柄
            dwDataType：1系统头数据;2 流数据
            pBuffer：存放数据的缓冲区指针 
			dwBufSize: 缓冲区大小 
			stFrameHeader:帧头信息
			dwUser: 用户数据
* 输出参数：无
* 返回值  ：
    0 --- 成功，<0 --- 失败
**********************************************************************************/
typedef void (*CBPlayProc)(int PlayHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize, QMAPI_NET_FRAME_HEADER stFrameHeader, DWORD dwUser);


/*********************************************************************************
* 函数介绍：透明串口数据处理回调
* 输入参数：
					port: 串口类型,1 232, 2:485
					data --- 透明串口数据指针
                                	size --- 透明串口数据大小
* 输出参数：无
* 返回值  ：
    0 --- 成功，<0 --- 失败
**********************************************************************************/
typedef int(*CBSerialDataProc)(int nPort, char *pData, unsigned int dwSize);

/*********************************************************************************
* 函数介绍：升级进度状态回调
* 输入参数：
					result:   升级结果,  -1-- 失败, 0---正在进行,1----升级成功
					progress: 升级进度,  0-100 升级进度百分比
* 输出参数：无
* 返回值  ：无
**********************************************************************************/
typedef void(*CBUpgradeProc)(int result, int progress);

/*********************************************************************************
* 函数介绍：透明串口数据处理回调
* 输入参数：
					port: 串口类型,1 232, 2:485
					data --- 透明串口数据指针
                                	size --- 透明串口数据大小
* 输出参数：无
* 返回值  ：
    0 --- 成功，<0 --- 失败
**********************************************************************************/
typedef int(*CBSystemReboot)(void );


typedef struct tagQMAPI_SYSNETAPI
{
	QMAPI_NETPT_TYPE_E			euNetPtType;
	
	int							bMainVideoStarted;
	CBVideoProc		pfMainVideoCallBk;
	int							dwMainVideoUser;

	int							bSecondVideoStarted;
	CBVideoProc		pfSecondVideoCallBk;
	int							dwSecondVideoUser;

	int							bMobileVideoStarted;
	CBVideoProc		pfMobileVideoCallBk;
	int							dwMobileVideoUser;
		
	CBAlarmProc			pfAlarmCallBk;

	CBAudioProc		pfAudioStreamCallBk;
	int							dwAudioUser;
    
	CBSerialDataProc		pfnSerialDataCallBk;
	struct tagQMAPI_SYSNETAPI	*pNext;
}QMAPI_SYSNETAPI, *PQMAPI_SYSNETAPI;




/*************************************************************
* 函数介绍：打开模块
* 输入参数：pt_flag--平台句柄
* 输出参数：无
* 返回值  ：平台模块句柄>0-成功，<0-错误代码
*************************************************************/
int QMapi_sys_open(int nFlag);

/*************************************************************
* 函数介绍：关闭模块
* 输入参数：
* 输出参数：无
* 返回值  ：>0-成功，<0-错误代码
*************************************************************/
int QMapi_sys_close(int Handle);

/*************************************************************
* 函数介绍：启动模块
* 输入参数：
* 输出参数：无
* 返回值  ：>0-成功，<0-错误代码
*************************************************************/
int QMapi_sys_start(int Handle, CBSystemReboot CallBack);

int QMapi_sys_stop(int Handle);

/************************************************************
*
*
*
*
*
**************************************************************/
int QMapi_sys_send_alarm(int nChannel, DWORD dwMsgType, QMAPI_SYSTEMTIME *SysTime);

/*************************************************************
* 函数介绍：网络接口模块配置
* 输入参数：handle:接口模块句柄
            cmd: 命令；
            channel: 通道号
            param：输入参数；
            size: param长度，特别对于GET命令时，输出参数应先判断缓冲区是否足够
* 输出参数：param：输出参数；
* 返回值  ：>=0-成功，<0-错误代码
*************************************************************/
int QMapi_sys_ioctrl(int Handle, int nCmd, int nChannel, void* Param, int nSize);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

