

#include <stdlib.h>
#include <string.h>


#include "QMAPIRecord.h"
#include "librecord.h"
#include "recorder_manager.h"

#include "ipc.h"
#define LOGTAG  "QM_RECORD"

static int g_playback_stop = 0;
static pthread_mutex_t playback_mutex = PTHREAD_MUTEX_INITIALIZER;

int QMAPI_Record_Init(void *lpDeafultParam, record_callback callback)
{
    RECORD_ATTR_T attr;
    attr.mode = MEDIA_RECMODE_FULLTIME;
    attr.duration = CFG_RECORD_SLICE_DURATION;

    attr.audioEnable = 1;
    attr.videoStream = MEDIA_REC_STREAM_HIGH;
    attr.videoEnable = 1;

    attr.avFormat = CFG_RECORD_AV_FORMAT;
    attr.audioFormat = CFG_RECORD_AUDIO_FORMAT; 

    return recorder_manager_init(&attr, callback);
}

int QMAPI_Record_UnInit(int Handle)
{
    recorder_manager_deinit();
	QMAPI_Record_PlayBackDelAll();
	return 0;
}

int QMAPI_Record_Start(int Handle, int nTrigerType, int nChannel)
{
    return recorder_manager_start();
}


int QMAPI_Record_Stop(int Handle, int nTrigerType, int nChannel)
{
    recorder_manager_stop();
}

int QMAPI_Record_IOCtrl(int Handle, int nCmd, int nChannel, void* Param, int nSize)
{
    if (nCmd != QMAPI_SYSCFG_GET_RECORDCFG
            && nCmd != QMAPI_SYSCFG_SET_RECORDCFG
            && nCmd != QMAPI_SYSCFG_GET_DEF_RECORDCFG
            && nCmd != QMAPI_SYSCFG_GET_RECORDMODECFG
            && nCmd != QMAPI_SYSCFG_GET_RECORD_ATTR
            && nCmd != QMAPI_SYSCFG_SET_RECORD_ATTR)
    {
        ipclog_error("RECORD: CMD not support!\n");
		return -1;
	}

    switch (nCmd) {
        case QMAPI_SYSCFG_GET_RECORD_ATTR:
            {
                if (nSize != sizeof(RECORD_ATTR_T)) {
                    ipclog_error("%s Invalid param nSize:%d for nCmd:%x\n", __FUNCTION__, nSize, nCmd);
                    return -1;
                }
                RECORD_ATTR_T *attr = (RECORD_ATTR_T *)Param;
                return recorder_manager_get_attr(attr);
            }
            break;

        case QMAPI_SYSCFG_SET_RECORD_ATTR:
            {
                if (nSize != sizeof(RECORD_ATTR_T)) {
                    ipclog_error("%s Invalid param nSize:%d for nCmd:%x\n", __FUNCTION__, nSize, nCmd);
                    return -1;
                }
                RECORD_ATTR_T *attr = (RECORD_ATTR_T *)Param;
                return recorder_manager_set_attr(attr);
            }
            break;

        default:
            return QMapi_sys_ioctrl(Handle, nCmd, nChannel, Param, nSize);
        break;
    }
}

int QMAPI_Record_FindFile(int Handle, LPQMAPI_NET_FILECOND pFindCond)
{

    QMAPI_FILES_INFO *pFilesInfo = NULL;
    int maxNum = 0;
    int FileCounts = 0;
    QMAPI_NET_FILECOND FindFileReq;
    QMAPI_NET_FINDDATA  *lpFileData = NULL;

    if(NULL == pFindCond)
    {
        printf("QMAPI_Record_FindFile-pFindCond is NULL!\n");
        return 0;
    }

    pFilesInfo = (QMAPI_FILES_INFO*)malloc(sizeof(QMAPI_FILES_INFO));
    if(NULL == pFilesInfo)
    {
        printf("QMAPI_Record_FindFile-pFilesInfo malloc failure!\n");
        return 0;
    }
    lpFileData = (QMAPI_NET_FINDDATA *)pFilesInfo->szBuf;
    memset(pFilesInfo, 0, sizeof(QMAPI_FILES_INFO));
    maxNum = QMAPI_FILES_SIZE/sizeof(QMAPI_NET_FINDDATA);

    FindFileReq.dwSize               = sizeof(FindFileReq);
    FindFileReq.dwFileType           = pFindCond->dwFileType;
    FindFileReq.dwChannel            = pFindCond->dwChannel;
    FindFileReq.dwFileType           = 0xFF;
    FindFileReq.dwIsLocked           = 0xFF;
    FindFileReq.stStartTime.dwYear   = pFindCond->stStartTime.dwYear;
    FindFileReq.stStartTime.dwMonth  = pFindCond->stStartTime.dwMonth;
    FindFileReq.stStartTime.dwDay    = pFindCond->stStartTime.dwDay;
    FindFileReq.stStartTime.dwHour   = pFindCond->stStartTime.dwHour;
    FindFileReq.stStartTime.dwMinute = pFindCond->stStartTime.dwMinute;
    FindFileReq.stStartTime.dwSecond = pFindCond->stStartTime.dwSecond;
    FindFileReq.stStopTime.dwYear    = pFindCond->stStopTime.dwYear;
    FindFileReq.stStopTime.dwMonth   = pFindCond->stStopTime.dwMonth;
    FindFileReq.stStopTime.dwDay     = pFindCond->stStopTime.dwDay;
    FindFileReq.stStopTime.dwHour    = pFindCond->stStopTime.dwHour;
    FindFileReq.stStopTime.dwMinute  = pFindCond->stStopTime.dwMinute;
    FindFileReq.stStopTime.dwSecond  = pFindCond->stStopTime.dwSecond;
    

    FileCounts = REC_FindFile(lpFileData, maxNum, FindFileReq.dwFileType,
                                FindFileReq.dwChannel, FindFileReq.stStartTime, FindFileReq.stStopTime);
    if(FileCounts <= 0)
    {
        printf("QMAPI_Record_FindFile-TL_search_file_list_add failure-FileCounts=%d!\n",FileCounts);
        free(pFilesInfo);
        return 0;
    }
    pFilesInfo->TotalNum = FileCounts;
    pFilesInfo->CurNum = pFilesInfo->StartIndex;
    pFilesInfo->EndIndex = pFilesInfo->TotalNum-1;
	memcpy(&pFilesInfo->StartTime, &FindFileReq.stStartTime, sizeof(QMAPI_TIME));
    printf("FileCounts:%d,StartIndex:%d,EndIndex:%d,maxNum:%d!\n", 
        pFilesInfo->TotalNum,pFilesInfo->StartIndex,pFilesInfo->EndIndex,maxNum);
    return (int)pFilesInfo;
}


/*********************************************************************************
* 参数说明：
    逐个获取查找到的文件数量。
* 输入参数：
    Handle：模块句柄
    FindHandle: 文件查找句柄，QMAPI_Record_FindFile()的返回值 
* 输出参数：无
* 返回值  ：
   -1表示失败，其他值表示当前的获取状态等信息
**********************************************************************************/
int QMAPI_Record_FindFileNumber(int Handle, int FindHandle)
{
	 QMAPI_FILES_INFO  *lpFileData = (QMAPI_FILES_INFO*)FindHandle;

	  if (!lpFileData)
	  {
			return QMAPI_FAILURE;
	  }
	  return (lpFileData->TotalNum);
}


/*********************************************************************************
* 参数说明：
    逐个获取查找到的文件信息。
* 输入参数：
    Handle：模块句柄
    FindHandle: 文件查找句柄，NET_DVR_FindFile_V30()的返回值 
    lpFindData:保存文件信息的指针
* 输出参数：无
* 返回值  ：
   -1表示失败，其他值表示当前的获取状态等信息
**********************************************************************************/
int QMAPI_Record_FindNextFile(int Handle, int FindHandle, LPQMAPI_NET_FINDDATA lpFindData)
{
    return REC_FindNextFile(Handle, FindHandle, lpFindData);
}

int QMAPI_Record_FindClose(int Handle, int FindHandle)
{
    QMAPI_FILES_INFO *pFilesInfo = (QMAPI_FILES_INFO*)FindHandle;

    if(NULL == pFilesInfo)
    {
        return QMAPI_SEARCH_FILE_SUCCESS;
    }

    free(pFilesInfo);
    return QMAPI_SEARCH_FILE_SUCCESS;
}
/*typedef struct tagPLAYFILE_INFO
{
    HANDLE hFileHandle;
    NET_DVR_TIME BeginTime;//文件起始时间
    NET_DVR_TIME EndTime;//文件结束时间
    NET_DVR_TIME CurTime;
    QMAPI_TIME StartTime;//搜索开始时间
    QMAPI_TIME StopTime;//搜索结束时间
    int index;
    unsigned int firstReads;//0:未开始读，1:已经开始
    unsigned long lastTimeTick;//上一个I帧的时间戳
    unsigned long firstStamp;//第一个I帧的时间戳
    char reserve[20];
    int width;
    int height;
    
    char sFileName[128];
    TL_STREAM_BUFFER *pStreamBuffer;
}PLAYFILE_INFO;*/

int QMAPI_Record_PlayBackByName(int Handle,char *csFileName)
{   
	pthread_mutex_lock(&playback_mutex);
	if(g_playback_stop)
	{
		pthread_mutex_unlock(&playback_mutex);
		return QMAPI_FAILURE;
	}
	int ret = PlayBackByName(Handle, csFileName);
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}

int QMAPI_Record_PlayBackByTime(int Handle, int nChannel, LPQMAPI_TIME lpStartTime, LPQMAPI_TIME lpStopTime)
{
    /*char szName[128] = {0};
    struct stat s;
    
    if(g_dms_sysnet_stop != 0)
    {
        return 0;
    }
    sprintf(szName, "/mnt/usb4/real/%04ld%02ld%02ld/0/%02ld%02ld%02ld.asf",
        lpStartTime->dwYear,lpStartTime->dwMonth,lpStartTime->dwDay,
        lpStartTime->dwHour,lpStartTime->dwMinute,lpStartTime->dwSecond);
    printf("%s-%d: szName=%s!\n",__FUNCTION__,__LINE__,szName);
    if(stat(szName,&s))
    {
        //文件不存在,必须查找
        return 0;
    }

    return QMAPI_Record_PlayBackByName(Handle,szName);*/
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    return QMAPI_SUCCESS;
}

int QMAPI_Record_PlayBackGetFrame(int Handle, int PlayHandle, char *pBuff,int *pSize,QMAPI_NET_FRAME_HEADER *pHeader,LPQMAPI_TIME lpFrameTime)
{
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    return QMAPI_FAILURE;
}
int QMAPI_Record_PlayBackGetReadPtrPos(int Handle, int PlayHandle, QMAPI_NET_DATA_PACKET *pstReadptr,LPQMAPI_TIME lpFrameTime)
{
	pthread_mutex_lock(&playback_mutex);
	if(g_playback_stop)
	{
		pthread_mutex_unlock(&playback_mutex);
		return QMAPI_FAILURE;
	}
	int ret = PlayBackGetReadPtrPos(Handle, PlayHandle, pstReadptr, lpFrameTime);
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}
int QMAPI_Record_PlayBackGetInfo(int Handle, int PlayHandle, QMAPI_RECFILE_INFO *pRecFileInfo)
{
	pthread_mutex_lock(&playback_mutex);
	if(g_playback_stop)
	{
		pthread_mutex_unlock(&playback_mutex);
		return QMAPI_FAILURE;
	}
	int ret = PlayBackGetInfo(Handle, PlayHandle, pRecFileInfo);
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}
int QMAPI_Record_PlayBackControl(int Handle, int PlayHandle, DWORD dwControlCode, char    *lpInBuffer, DWORD    dwInLen,  char     *lpOutBuffer,  DWORD    *lpOutLen)
{
	pthread_mutex_lock(&playback_mutex);
	if(g_playback_stop)
	{
		pthread_mutex_unlock(&playback_mutex);
		return QMAPI_FAILURE;
	}
	int ret = PlayBackControl(Handle, PlayHandle, dwControlCode, lpInBuffer, dwInLen, lpOutBuffer, lpOutLen);
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}

int QMAPI_Record_PlayBackStop(int Handle, int PlayHandle)
{
	pthread_mutex_lock(&playback_mutex);
	if(g_playback_stop)
	{
		pthread_mutex_unlock(&playback_mutex);
		return QMAPI_SUCCESS;
	}
	int ret = PlayBackStop(Handle, PlayHandle);
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}

int QMAPI_Record_PlayBackAddCallBack(int Handle, int FindHandle, CBPlayProc Callback, DWORD dwUser)
{
	pthread_mutex_lock(&playback_mutex);
	if(g_playback_stop)
	{
		pthread_mutex_unlock(&playback_mutex);
		return QMAPI_FAILURE;
	}
	int ret = PlayBackAddDataCallBack(Handle, FindHandle, Callback, dwUser);
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}


int QMAPI_Record_PlayBackDelCallBack(int Handle, int FindHandle, CBPlayProc Callback, DWORD dwUser)
{
	pthread_mutex_lock(&playback_mutex);
	if(g_playback_stop)
	{
		pthread_mutex_unlock(&playback_mutex);
		return QMAPI_FAILURE;
	}
	int ret = PlayBackDelDataCallBack(Handle, FindHandle, Callback, dwUser);
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}

int QMAPI_Record_PlayBackDelAll(void)
{
	pthread_mutex_lock(&playback_mutex);
	g_playback_stop = 1;
	int ret = PlayBackDelAll();
	pthread_mutex_unlock(&playback_mutex);
	return ret;
}


