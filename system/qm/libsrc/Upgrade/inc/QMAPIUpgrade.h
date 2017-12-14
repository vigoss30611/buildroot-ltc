#ifndef __QMAPI_UPGRADE_H__
#define __QMAPI_UPGRADE_H__

#include "QMAPI.h"
#include "QMAPINetSdk.h"
#include "QMAPIErrno.h"


typedef int (*CBUpgradeExit)(void);

/***************************************************************************************/
// 升级模块初始化，(若不初始化,则代表不支持软件升级)
//  pFunc [in] 注册升级时模块退出函数，释放内存，
//    需要高度注意模块退出的依耐性
int QMAPI_Upgrade_Init(CBUpgradeExit pFunc);

/***************************************************************************************/
// 升级方式一，
//   先判断校验字段
//   把升级包数据写到临时文件内，
//   调用平台提供的升级接口
int QMAPI_UpgradeFlashReq(int userId, QMAPI_NET_UPGRADE_REQ *lpUpgradeReq);
int QMAPI_UpgradeFlashData(int userId, QMAPI_NET_UPGRADE_DATA *lpUpgradeData);

// 升级方式二，
//   直接传入文件名，创建线程直接调用平台提供的升级接口
int QMAPI_StartUpgrade(char *pFilePath);

/*
* 设置升级回调接口
*/
int QMAPI_PrepareUpgrade(CBUpgradeProc pFunc);

/*
* 获取升级模块当前状态
*/
int QMAPI_UpgradeFlashGetState(QMAPI_NET_UPGRADE_RESP * pPara);

#endif

