
#ifndef _LIB_RECORD_H_
#define _LIB_RECORD_H_
#include "hd_mng.h"
#include <stdio.h>
#include "QMAPI.h"

#define MAX_RECORD_TIME (10*60*1000) /*10分钟 单位毫秒*/
#define MAX_PLAY_CALL_BACK 8 /* 最大支持8 个回调*/

int REC_FindFile(QMAPI_NET_FINDDATA *pFileDataArray, 
			                int maxNum,
			                unsigned int nFileType,
			                unsigned int nChannel,
			                QMAPI_TIME struStartTime,
			                QMAPI_TIME struStopTime);
int REC_FindNextFile(int Handle, int FindHandle, LPQMAPI_NET_FINDDATA lpFindData);
int PlayBackByName(int Handle,char *csFileName);
int PlayBackStop(int Handle, int PlayHandle);
int PlayBackGetInfo(int Handle, int PlayHandle, QMAPI_RECFILE_INFO *pRecFileInfo);
int PlayBackGetReadPtrPos(int Handle, int PlayHandle, QMAPI_NET_DATA_PACKET *pstReadptr,LPQMAPI_TIME lpFrameTime);
int PlayBackAddDataCallBack(int Handle, int FindHandle, CBPlayProc Callback, DWORD dwUser);
int PlayBackDelDataCallBack(int Handle, int FindHandle, CBPlayProc Callback, DWORD dwUser);
int PlayBackControl(int Handle, int PlayHandle, DWORD dwControlCode, const char *lpInBuffer, DWORD dwInLen, char *lpOutBuffer, DWORD *lpOutLen);

/* FtpModeInit
  录像模块初始化
  返回值:0: 成功, -1:失败
 */
int REC_Init();

void REC_UnInit();

/* 
  Start_Record
  启动录像功能
  nTrigerType: TRIGER_XXXX
  nDuration: 录像时间,单位为毫秒，nDuration==0，则手动停止或计划时间时后停止
  返回值: 0:执行成功, -1:执行失败
  
 */
int REC_StartRecord(int nStartType, int nTrigerType, int nChannel, DWORD nDuration);

/* Stop_Record
  关闭录像
  nTrigerType: TRIGER_XXXX
  返回值: 0:执行成功, -1:执行失败
 */
int REC_StopRecord(int nTrigerType, int nChannel);

/* 	StopAllRecord
  	关闭所有正在录制的文件
 */
void REC_StopAllRecord(int type);

/*
	
*/
int REC_GetDiskRecordStatus(int nDiskNo);

#endif

