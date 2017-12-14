
#ifndef _LOG_MNG_H_
#define _LOG_MNG_H_

#include "QMAPIType.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"

/******************************************************************************************************
  头信息按照256字节对齐               日志索引信息             扩展块
|              QMAPI_LOG_HEADER           |    QMAPI_LOG_INDEX * index_max     | block_size  *  block_max |

*******************************************************************************************************/

typedef struct tagQMAPI_LOG_HEADER
{
	char			magic[8];			//log_dms
	char			version[8];			//1.0

	unsigned long	ver_date;			//20151026
	unsigned long	file_size;			//文件大小

	unsigned long	offset_index;		//索引起始偏移
	unsigned long	index_size;			//一个索引大小
	unsigned long	index_max;			//索引最大数
	unsigned long	curr_index;			//当前写入的索引序号   写到index末尾或者写入了扩展block才会写入

	unsigned long	offset_block;		//块起始偏移
	unsigned long 	block_size; 			//块大小
	unsigned long	block_max;			//块最大数
	unsigned long	curr_block;			//当前写入的块序号  这个会实时更新
	
	BYTE 		index_used_flag; 		//每次写到文件末尾会更新这个标记，1或者2
	BYTE 		recv[3];

	BYTE 		block_used[QMAPI_LOG_BLOCK_MAX]; 		//块是否被使用
}QMAPI_LOG_HEADER;

/*
typedef struct tagQMAPI_LOG_INDEX
{
	BYTE bUsed;//对应QMAPI_LOG_HEADER的currUsedFlag
	BYTE mainType;//日志主类型//QMAPI_LOG_MAIN_TYPE_E
	BYTE subType;//详细类型
	BYTE chnn;
	
	unsigned long time;//秒数，发生时间
	unsigned long ip;
	
	BYTE userID;
	BYTE blockID;//从1开始0表示不扩展
	BYTE recv[2];//3//预留
}QMAPI_LOG_INDEX;*/

typedef struct tagQMAPI_LOG_INFO
{
	QMAPI_LOG_MAIN_TYPE_E maintype;
	unsigned long subtype;
	unsigned long ip;
	BYTE chnn;
       union
        {
            BYTE byUserID;
            BYTE byAlarmEnd;
        };
	
	BYTE byUsedExInfo;
	BYTE recv;
	char pExInfo[QMAPI_LOG_BLOCK_SIZE];
}QMAPI_LOG_INFO;

int DmsLog_Init(const char * plogPath);
void DmsLog_Quit();

int Update_PowerOnTime();
int Repair_LogTime();

int Wirte_Log(const QMAPI_LOG_INFO* pLogInfo);

void Search_Log(QMAPI_LOG_MAIN_TYPE_E maintype, unsigned long subType, QMAPI_LOG_INDEX* pLogIndex, unsigned int* pNum, char (*pLogExtInfo)[QMAPI_LOG_BLOCK_SIZE], unsigned int extInfoBufLen);

int Get_LogExtInfo(const QMAPI_LOG_INDEX *logInfo, char* pBuf, unsigned int* pLen);

#endif

