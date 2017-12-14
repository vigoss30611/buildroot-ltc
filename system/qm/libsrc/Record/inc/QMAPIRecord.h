
#ifndef __QMAPI_RECORD_H__
#define __QMAPI_RECORD_H__


#include "QMAPIType.h"
#include "QMAPINetSdk.h"
#include "QMAPIErrno.h"
#include "QMAPI.h"

#define EVENT_RECORD "record_notify"

typedef enum {
    RECORD_NOTIFY_START,
    RECORD_NOTIFY_FINISH,
    RECORD_NOTIFY_ERROR,
    RECORD_NOTIFY_HD_FULL,
} RECORD_NOTIFY_TYPE_E;

typedef struct {
    RECORD_NOTIFY_TYPE_E type;
    void *arg;
} RECORD_NOTIFY_T;

typedef struct {
    int trigger;
    char *name;
} RECORD_FILE_T;

typedef int (*record_callback)(RECORD_FILE_T *file);

typedef enum {
    MEDIA_RECMODE_FULLTIME=0, 
    MEDIA_RECMODE_MOTDECT
} MEDIA_REC_MODE_E;

typedef enum {
    MEDIA_REC_STREAM_HIGH = 0,
    MEDIA_REC_STREAM_MID 
} MEDIA_REC_STREAM_E;

typedef enum {
    MEDIA_REC_AV_FORMAT_MKV = 0,
    MEDIA_REC_AV_FORMAT_MP4 
} MEDIA_REC_AV_FORMAT_E;

typedef enum {
    MEDIA_REC_AUDIO_FORMAT_G711A = 0,
    MEDIA_REC_AUDIO_FORMAT_AAC 
} MEDIA_REC_AUDIO_FORMAT_E;

typedef struct {
    int mode;
    int duration;

    int videoEnable;
    int videoStream;

    int audioEnable;
    int audioStream;

    MEDIA_REC_AV_FORMAT_E avFormat;
    MEDIA_REC_AUDIO_FORMAT_E audioFormat;
} RECORD_ATTR_T;

//Triger类型
#define TRIGER_UNKNOW	0	//未知
#define TRIGER_SCHED	 0x000001 //定时录像
#define TRIGER_MOTION  0x000002  //移到侦测录像
#define TRIGER_ALARM   0x000004  //报警录像
#define TRIGER_CMD     0x000008  //命令录像
#define TRIGER_MANU    0x000010  //手工录像
#define TRIGER_UMOUNT  0x000020  //UMOUNT SD/UDISK

/************************************* 命令 ***************************************************************/


//参数说明：
//dwDataType：1系统头数据;2 流数据, 3结束
//pBuffer：存放数据的缓冲区指针 (传入NULL表示读取数据结束)
//dwBufSize: 缓冲区大小 
//stFrameHeader:帧头信息
//dwUser: 用户数据
typedef void (*fNetPlayDataCallBack)(DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize, QMAPI_NET_FRAME_HEADER stFrameHeader, ULONGLONG TimeTick, DWORD dwUser);

/**************************************************************************************************
	模块初始化
		QMAPI_Record_Init 模块初始化, 成功返回操作句柄，失败返回0
			lpDeafultParam [in]:

		QMAPI_Record_UnInit 模块反初始化, 成功反馈QMAPI_RECORD_SUCCESS
			Handle   [in]: 初始化成功反馈的句柄
*/
int QMAPI_Record_Init(void *lpDeafultParam, record_callback callback);
int QMAPI_Record_UnInit(int Handle);

/**************************************************************************************************
	模块录像功能启停
		QMAPI_Record_Start 模块录像启动，成功反馈QMAPI_RECORD_SUCCESS
			Handle       [in]: 初始化成功反馈的句柄
			nTrigerType  [in]: 
			nChannel     [in]: 通道号
			
		QMAPI_Record_Stop 模块录像停止，成功反馈QMAPI_RECORD_SUCCESS
			Handle       [in]: 初始化成功反馈的句柄
			nTrigerType  [in]:
			nChannel     [in]: 通道号

//Triger类型
#define TRIGER_UNKNOW	0	//未知
#define TRIGER_SCHED	 0x000001 //定时录像
#define TRIGER_MOTION  0x000002  //移到侦测录像
#define TRIGER_ALARM   0x000004  //报警录像
#define TRIGER_CMD     0x000008  //命令录像
#define TRIGER_MANU    0x000010  //手工录像
#define TRIGER_UMOUNT  0x000020  //UMOUNT SD/UDISK
*/
int QMAPI_Record_Start(int Handle, int nTrigerType, int nChannel);
int QMAPI_Record_Stop(int Handle, int nTrigerType, int nChannel);

/*****************************************************************************************************
	模块命令控制
	Handle     [in ]: QMAPI_Record_Init 成功反馈的句柄
	nCmd       [in ]: 命令类型
	nChannel   [in ]:
	Param      [in ][out]: 输入、输出缓存；
	nSize      [in ]: 输入缓存大小

	//图像录像（QMAPI_NET_CHANNEL_RECORD结构）
#define QMAPI_SYSCFG_GET_RECORDCFG				(QMAPI_RECORD_MODULE+0xF300)	// 获取图象录像参数 
#define QMAPI_SYSCFG_SET_RECORDCFG				(QMAPI_RECORD_MODULE+0xF301)	// 设置图象录像参数 
#define QMAPI_SYSCFG_GET_DEF_RECORDCFG			(QMAPI_RECORD_MODULE+0xF302)

	//图像手动录像（QMAPI_NET_CHANNEL_RECORD结构)
#define QMAPI_SYSCFG_GET_RECORDMODECFG			(QMAPI_RECORD_MODULE+0xF320)	// 获取图象手动录像参数 
#define QMAPI_SYSCFG_SET_RECORDMODECFG			(QMAPI_RECORD_MODULE+0xF321)	// 设置图象手动录像参数 

*/
int QMAPI_Record_IOCtrl(int Handle, int nCmd, int nChannel, void* Param, int nSize);


/*********************************************************************************
文件查询系列函数；
	QMAPI_Record_FindFile 开始查找文件，成功返回查询句柄，失败返回0；
		Channel   [in]:  通道号
		pFindCond [in]:  查询条件

	QMAPI_Record_FindFileNumber 获取查询文件数量，成功返回实际查询数量，失败返回QMAPI_FAILURE；
		Channel   [in]:  通道号
		FindHandle[in]:  QMAPI_Record_FindFile 返回的查询句柄

	QMAPI_Record_FindNextFile 查询下个文件信息.成功返回QMAPI_NET_FILE_SUCCESS，
		Channel    [in]:  通道号
		FindHandle [in]:  QMAPI_Record_FindFile 返回的查询句柄
		lpFindData [out]: 实际文件的信息；(长度、起始时间、类型等)

	QMAPI_Record_FindClose 关闭查询
		Channel   [in]:  通道号
		FindHandle[in]:  QMAPI_Record_FindFile 返回的查询句柄
		
**********************************************************************************/
int QMAPI_Record_FindFile(int Channel, LPQMAPI_NET_FILECOND pFindCond);
int QMAPI_Record_FindFileNumber(int Channel, int FindHandle);
int QMAPI_Record_FindNextFile(int Channel, int FindHandle, LPQMAPI_NET_FINDDATA lpFindData);
int QMAPI_Record_FindClose(int Channel, int FindHandle);



/*********************************************************************
回放系列1，数据获取方式
	QMAPI_Record_PlayBackByName
		Channel   [in]:
		csFileName[in]:

	QMAPI_Record_PlayBackGetReadPtrPos
		Channel    [in ]:
		PlayHandle [in ]:
		pstReadptr [out]:
		lpFrameTime[out]:

	QMAPI_Record_PlayBackGetInfo
		Channel     [in ]:
		PlayHandle  [in ]:
		pRecFileInfo[out]:

	QMAPI_Record_PlayBackControl
		Channel       [in ]:
		PlayHandle    [in ]:
		dwControlCode [in ]:
		lpInBuffer    [in ]:
		dwInLen       [in ]:
		lpOutBuffer   [out]:
		lpOutLen      [out]:

	QMAPI_Record_PlayBackStop
		Channel       [in ]:
		PlayHandle    [in ]:
********************************************************************/
int QMAPI_Record_PlayBackByName(int Channel,char *csFileName);
int QMAPI_Record_PlayBackGetReadPtrPos(int Channel, int PlayHandle, QMAPI_NET_DATA_PACKET *pstReadptr, LPQMAPI_TIME lpFrameTime);
int QMAPI_Record_PlayBackGetInfo(int Channel,int PlayHandle,QMAPI_RECFILE_INFO *pRecFileInfo);
int QMAPI_Record_PlayBackControl(int Channel, int PlayHandle,DWORD dwControlCode,char *lpInBuffer,DWORD dwInLen,char *lpOutBuffer,DWORD *lpOutLen);
int QMAPI_Record_PlayBackStop(int Channel, int PlayHandle);



/*********************************************************************
回放系列2，数据推送方式
	QMAPI_Record_PlayBackAddCallBack 添加数据推送回调函数，成功返回QMAPI_SUCCESS
		Channel       [in ]:  通道号
		FindHandle    [in ]:  查询反馈的句柄
		Callback      [in ]:  数据回调函数
		dwUser        [in ]:  用户指针

	QMAPI_Record_PlayBackDelCallBack 删除数据推送回调函数，成功返回QMAPI_SUCCESS
		Channel       [in ]:  通道号
		FindHandle    [in ]:  查询反馈的句柄
		Callback      [in ]:  数据回调函数
		dwUser        [in ]:  用户指针

	QMAPI_Record_PlayBackDelAll 删除所有数据推送回调函数，成功返回QMAPI_SUCCESS
		
		
********************************************************************/
int QMAPI_Record_PlayBackAddCallBack(int Channel, int FindHandle, CBPlayProc Callback, DWORD dwUser);
int QMAPI_Record_PlayBackDelCallBack(int Channel, int FindHandle, CBPlayProc Callback, DWORD dwUser);
int QMAPI_Record_PlayBackDelAll(void);


#endif //
