/**
*
**/
#ifndef __QMAPI_ALARMSERVER_H__
#define __QMAPI_ALARMSERVER_H__

#include "QMAPINetSdk.h"
#include "QMAPIErrno.h"

/*********************************************************************
    Function:   QMAPI_AlarmServer_Init
    Description: 初始化报警服务模块
    Calls:
    Called By:
    parameter:
            [in] lpDeafultParam QMAPI_NET_ALARM_SERVER
    Return: 模块句柄
********************************************************************/
int QMAPI_AlarmServer_Init(void *lpDeafultParam);

/*********************************************************************
    Function:   QMAPI_AlarmServer_UnInit
    Description: 反初始化报警模块
    Calls:
    Called By:
    parameter:
            [in] Handle 模块句柄
    Return: QMAPI_SUCCESS/QMAPI_FAILURE
********************************************************************/
int QMAPI_AlarmServer_UnInit(int Handle);

/*********************************************************************
    Function:   QMAPI_AlarmServer_Start
    Description: 启用报警模块服务
    Calls:
    Called By:
    parameter:
            [in] Handle 模块句柄
    Return: QMAPI_SUCCESS/QMAPI_FAILURE
********************************************************************/
int QMAPI_AlarmServer_Start(int Handle);

/*********************************************************************
    Function:   QMAPI_AlarmServer_Stop
    Description: 停用报警模块服务
    Calls:
    Called By:
    parameter:
            [in] Handle 模块句柄
    Return: QMAPI_SUCCESS/QMAPI_FAILURE
********************************************************************/
int QMAPI_AlarmServer_Stop(int Handle);

/*********************************************************************
    Function:   QMAPI_AlarmServer_IOCtrl
    Description: 配置报警服务器参数
    Calls:
    Called By:
    parameter:
            [in] Handle 控制句柄
            [in] nCmd 控制命令
                    有效值为: QMAPI_SYSCFG_GET_MOTIONCFG/QMAPI_SYSCFG_SET_MOTIONCFG/QMAPI_SYSCFG_GET_VLOSTCFG/QMAPI_SYSCFG_SET_VLOSTCFG
                                QMAPI_SYSCFG_GET_HIDEALARMCFG/QMAPI_SYSCFG_SET_HIDEALARMCFG/QMAPI_SYSCFG_GET_ALARMINCFG/QMAPI_SYSCFG_SET_ALARMINCFG
            [in/out] lpInParam QMAPI_NET_CHANNEL_PIC_INFO
            [in/out] nInSize 配置结构体大小
    Return: QMAPI_SUCCESS/QMAPI_FAILURE/QMAPI_ERR_UNKNOW_COMMNAD
********************************************************************/
int QMAPI_AlarmServer_IOCtrl(int Handle, int nCmd, int nChannel, void* Param, int nSize);






















#endif



