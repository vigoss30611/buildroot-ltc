#define _GNU_SOURCE

#include <stdlib.h>
#include <sys/vfs.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <mntent.h>
#include <sys/stat.h>
//#include <sys/mount.h>
#define SIG_START
/* player for push button */
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <errno.h>
#include <pthread.h>

#include <sys/prctl.h>
#include "QMAPIType.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPI.h"
#include "librecord.h"
#include "QMAPIErrno.h"
#include "FileRander_ASF.h"
#include "logger.h"

#include "hd_mng.h"
#include "system_msg.h"
#include "RecType.h"
//#include "SnapMng.h"

#include <qsdk/videobox.h>
#include "ipc.h"
#include "looper.h"
#include "playback.h"

#define LOGTAG  "QM_PLAYBACK"

typedef struct {
	unsigned char buser;
	unsigned char thread_run; /*1 代表运行*/
	unsigned char thread_over;
	unsigned char res;
	int Handle;
	int FindHandle;

	time_t FileStartTime;
	UINT64 VideoStarttime;
	UINT64 AudioStarttime;
	
	DWORD dwUser;
	CBPlayProc Callback;
}PlayDataCallBack_t;

PlayDataCallBack_t PlayDataCallBack[MAX_PLAY_CALL_BACK];

pthread_mutex_t 	play_data_call_back=PTHREAD_MUTEX_INITIALIZER;	//保证同一时间,只能一个线程在添加索引
static int g_playBackStop = 0;

BOOL IsRecordingFile(const char *sFileName, int nChannel, int nStorType);

BOOL PlayBackSettoTime(int Handle, DWORD dwSecond)
{
	PLAYFILE_INFO *pHead = (PLAYFILE_INFO *)Handle;

	if (pHead->Mode)
	{
		return TRUE;
	}
	
	//return SetFilePosToTime(pHead->hFileHandle, dwSecond);
}

int PlayBackByName(int Handle, char *csFileName)
{
    PLAYFILE_INFO *pHead = NULL;
    BOOL bHasIndex = FALSE;
    FilePlay_INFO streamInfo;
    struct stat FileInfo;
    struct tm tim;
    int nFileYear = 0;
    int nFileMonth = 0;
    int nFileDay = 0;
    int nFileHour = 0;
    int nFileMin = 0;
    int nFileSecond = 0;
	char buf[16]={0};
	if(!csFileName) return 0;
    printf("%s %d-file name:%s!\n", __FUNCTION__,__LINE__,csFileName);

    stat(csFileName, &FileInfo);
    //文件开始时间
	if(strncmp(csFileName, "/mnt/mmc/", 9)==0 || strncmp(csFileName, "/mnt/usb/", 9)==0){
		sscanf(csFileName, "/mnt/%3s/%04d%02d%02d/0/%02d%02d%02d.asf",
			buf, &nFileYear, &nFileMonth, &nFileDay, &nFileHour, &nFileMin, &nFileSecond);
	}else{
	    sscanf(csFileName, "/mnt/%4s/%04d%02d%02d/0/%02d%02d%02d.asf",
    	    buf, &nFileYear, &nFileMonth, &nFileDay, &nFileHour, &nFileMin, &nFileSecond);
   	}
    //文件结束时间
    localtime_r(&FileInfo.st_mtime, &tim);
    
    pHead = (PLAYFILE_INFO*)malloc(sizeof(PLAYFILE_INFO));
    if(NULL == pHead)
    {
        REC_ERR("\n");
        //dms_sysnetapi_PlayBackStop(Handle,(int)pHead);
        return 0;
    }
    memset(pHead, 0, sizeof(*pHead));
    pHead->pStreamBuffer = (JB_STREAM_BUFFER*)malloc(sizeof(JB_STREAM_BUFFER));
    strcpy(pHead->sFileName,csFileName);
    pHead->BeginTime.dwYear = nFileYear;
    pHead->BeginTime.dwMonth = nFileMonth;
    pHead->BeginTime.dwDay = nFileDay;
    pHead->BeginTime.dwHour = nFileHour;
    pHead->BeginTime.dwMinute = nFileMin;
    pHead->BeginTime.dwSecond = nFileSecond;
    
    pHead->EndTime.dwYear   = tim.tm_year + 1900;
    pHead->EndTime.dwMonth  = tim.tm_mon+1;
    pHead->EndTime.dwDay    = tim.tm_mday;
    pHead->EndTime.dwHour   = tim.tm_hour;
    pHead->EndTime.dwMinute = tim.tm_min;
    pHead->EndTime.dwSecond = tim.tm_sec;
    
    pHead->CurTime = pHead->BeginTime;

    pHead->StartTime.dwYear   = pHead->BeginTime.dwYear;
    pHead->StartTime.dwMonth  = pHead->BeginTime.dwMonth;
    pHead->StartTime.dwDay    = pHead->BeginTime.dwDay;
    pHead->StartTime.dwHour   = pHead->BeginTime.dwHour;
    pHead->StartTime.dwMinute = pHead->BeginTime.dwMinute;
    pHead->StartTime.dwSecond = pHead->BeginTime.dwSecond;

    pHead->StopTime.dwYear   = pHead->EndTime.dwYear;
    pHead->StopTime.dwMonth  = pHead->EndTime.dwMonth;
    pHead->StopTime.dwDay    = pHead->EndTime.dwDay;
    pHead->StopTime.dwHour   = pHead->EndTime.dwHour;
    pHead->StopTime.dwMinute = pHead->EndTime.dwMinute;
    pHead->StopTime.dwSecond = pHead->EndTime.dwSecond;
    pHead->Mode = 0;

    pHead->hFileHandle = playback_init(csFileName);
    if (NULL == pHead->hFileHandle) {
        REC_ERR("\n");
        PlayBackStop(Handle,(int)pHead);
        return 0;
    }
    
    playback_info_t playbackInfo;
    if (playback_get_info((int)(pHead->hFileHandle), &playbackInfo)  < 0) {
        ipclog_error("playback_get_info failed\n");
        PlayBackStop(Handle,(int)pHead);
        return 0;
    }

    pHead->VideoFormat = playbackInfo.video_codec_id;
    pHead->width       = playbackInfo.video_width;
    pHead->height      = playbackInfo.video_height;
    pHead->framerate   = playbackInfo.video_fps;

    pHead->bHaveAudio  = (playbackInfo.audio_index==-1) ? 0 : 1;
    pHead->formatTag   = playbackInfo.audio_codec_id;

    pHead->firstStamp  = playbackInfo.time_pos;
    pHead->dwTotalTime = playbackInfo.time_len;
    pHead->filesize    = FileInfo.st_size;

    pHead->filesize    = FileInfo.st_size;
    return (int)pHead;
}

int PlayBackStop(int Handle, int PlayHandle)
{
    PLAYFILE_INFO *pPlayFileInfo = (PLAYFILE_INFO*)PlayHandle;
    printf("########### StopPlayBack(%d,%d)....\n", Handle, PlayHandle);
    if (NULL==PlayHandle)
        return QMAPI_SUCCESS;

    if (NULL != pPlayFileInfo->hFileHandle) {
        playback_deinit((int)(pPlayFileInfo->hFileHandle));
        pPlayFileInfo->hFileHandle = NULL;
    }
    
    if (NULL != pPlayFileInfo->pStreamBuffer) {
        free(pPlayFileInfo->pStreamBuffer);
        pPlayFileInfo->pStreamBuffer = NULL;
    }
    
    free(pPlayFileInfo);
    pPlayFileInfo = NULL;

	return QMAPI_SUCCESS;
}

int SetPlayBackMode(int handle, int PlayHandle, int mode)
{
	PLAYFILE_INFO *pHead = (PLAYFILE_INFO*)PlayHandle;
	if (!pHead)
	{
		return QMAPI_FAILURE;
	}

	if (pHead->Mode != mode)
	{
		pHead->Mode = mode;
	}
	return QMAPI_SUCCESS;
}

int PlayBackGetInfo(int Handle, int PlayHandle, QMAPI_RECFILE_INFO *pRecFileInfo)
{
	PLAYFILE_INFO *pHead = (PLAYFILE_INFO*)PlayHandle;

    if(NULL == pHead || NULL == pRecFileInfo)
    {
        return QMAPI_FAILURE;
    }

    pRecFileInfo->u16Width            = pHead->width;
    pRecFileInfo->u16Height           = pHead->height;
    //pRecFileInfo->u16FrameRate = 25;
    pRecFileInfo->u16VideoTag         = pHead->VideoFormat;
    pRecFileInfo->u16HaveAudio        = pHead->bHaveAudio;
    pRecFileInfo->u16AudioTag         = pHead->formatTag;
    pRecFileInfo->u16AudioBitWidth    = 16;
    pRecFileInfo->u16AudioSampleRate  = 8000;
    pRecFileInfo->u32TotalTime        = pHead->dwTotalTime;
    pRecFileInfo->u16FrameRate        = pHead->framerate;
    pRecFileInfo->u32TotalSize        = pHead->filesize;
    return QMAPI_SUCCESS;
}

int PlayBackGetReadPtrPos(int Handle, int PlayHandle, QMAPI_NET_DATA_PACKET *pstReadptr,LPQMAPI_TIME lpFrameTime)
{
    PLAYFILE_INFO *pHead = (PLAYFILE_INFO*)PlayHandle;
    struct demux_frame_t demuxframe;

    if (NULL == pHead || NULL == pstReadptr || NULL == lpFrameTime) {
        printf("%s Invalide pHead or pstReadptr or lpFrameTime", __FUNCTION__);
        return QMAPI_FAILURE;
    }
    
    DWORD nLastTick=0;
    if (pHead->firstReads) {
        nLastTick = pHead->pStreamBuffer->dwTimeTick;
        if (!pHead->Mode) {
            if (playback_get_frame((int)(pHead->hFileHandle), &demuxframe) < 0) {
                //printf("######## readonepacket failed, type:%lu....\n", pHead->pStreamBuffer->FrameType);
                //读取文件失败时，虚拟一帧I帧数据出来，结束此次下载
#if 0
                pHead->pStreamBuffer->dwSize = 1024;
                pHead->pStreamBuffer->dwTimeTick = pHead->dwTotalTime;//0xFFFF;
                pHead->pStreamBuffer->FrameType = MY_FRAME_TYPE_I;
                return QMAPI_FAILURE;
#else
                return QMAPI_FAILURE;
#endif
            }
        } else if (playback_get_iframe((int)(pHead->hFileHandle), &demuxframe) < 0) {
            return QMAPI_FAILURE;
        }
        //else
        //{
        //	printf("pts:%lu\n", pHead->pStreamBuffer->dwTimeTick);
        //}
    } else {//第一读帧,读到I帧才退出
        long            SeekPlayStamp = 0;
        long            SeekPlayStamp1 = 0;
        long            SeekPlayStamp2 = 0;
#if 0
        printf("file start time:%04lu-%02lu-%02lu %02lu:%02lu:%02lu\n",pHead->BeginTime.dwYear,pHead->BeginTime.dwMonth,
                pHead->BeginTime.dwDay,pHead->BeginTime.dwHour,pHead->BeginTime.dwMinute,pHead->BeginTime.dwSecond);
        printf("search start time:%04lu-%02lu-%02lu %02lu:%02lu:%02lu\n",pHead->StartTime.dwYear,pHead->StartTime.dwMonth,
                pHead->StartTime.dwDay,pHead->StartTime.dwHour,pHead->StartTime.dwMinute,pHead->StartTime.dwSecond);
#endif
        SeekPlayStamp1 = (long)(pHead->BeginTime.dwHour*3600+pHead->BeginTime.dwMinute*60+pHead->BeginTime.dwSecond);
        SeekPlayStamp2 = (long)(pHead->StartTime.dwHour*3600+pHead->StartTime.dwMinute*60+pHead->StartTime.dwSecond);
        SeekPlayStamp = SeekPlayStamp2-SeekPlayStamp1;
        //printf("SeekPlayStamp=%ld,line:%d!\n",SeekPlayStamp,__LINE__);
        if (SeekPlayStamp < 0) {
            SeekPlayStamp = 0;
        }
        SeekPlayStamp *= 1000;
        //printf("SeekPlayStamp=%ld,line:%d!\n",SeekPlayStamp,__LINE__);
        SeekPlayStamp += pHead->firstStamp;
        //printf("SeekPlayStamp=%ld,line:%d!\n",SeekPlayStamp,__LINE__);
        //暂时不调用这个定位。。有问题
        //SetFilePosToTime(pHead->hFileHandle, SeekPlayStamp);

        while (playback_get_frame((int)(pHead->hFileHandle), &demuxframe) == 0) {
            if (demuxframe.flags == VIDEO_FRAME_I) {
                if(SeekPlayStamp - demuxframe.timestamp < 1000 
                        || demuxframe.timestamp >= SeekPlayStamp) {
                    demuxframe.timestamp += 1;
                    pHead->lastTimeTick = demuxframe.timestamp;
                    nLastTick = pHead->lastTimeTick;
                    break;
                }
            }
        }

        if (0 == pHead->lastTimeTick) {
            printf("PlayBackGetReadPtrPos--first read frame failure!\n");
            return QMAPI_FAILURE;
        }

        pHead->firstReads = 1;
        //构造lpFrameTime
        lpFrameTime->dwYear = pHead->StartTime.dwYear;
        lpFrameTime->dwMonth = pHead->StartTime.dwMonth;
        lpFrameTime->dwDay = pHead->StartTime.dwDay;
        lpFrameTime->dwHour = pHead->StartTime.dwHour;
        lpFrameTime->dwMinute = pHead->StartTime.dwMinute;
        lpFrameTime->dwSecond = pHead->StartTime.dwSecond;
    }

    void *data = NULL;
    int size;
    int frameType;

    if (demuxframe.flags == VIDEO_FRAME_I) {
#if 1
        playback_handle_t *handle = (playback_handle_t *)(pHead->hFileHandle);
        data = pHead->pStreamBuffer->pBuffer;
        size = demuxframe.data_size + handle->vhdrlen;
        if (size >= MAX_STREAM_BUFF_SIZE) {
            ipclog_warn("%s size:%d \n", __FUNCTION__, size);
            return QMAPI_FAILURE;
        }
        
        memcpy(data, handle->vheader, handle->vhdrlen);
        memcpy(data + handle->vhdrlen, demuxframe.data, demuxframe.data_size);
#else
        data = demuxframe.data;
        size = demuxframe.data_size;
#endif
    } else {
        data = demuxframe.data;
        size = demuxframe.data_size;
    }

    if (size >= MAX_STREAM_BUFF_SIZE) {
        ipclog_warn("%s size:%d \n", __FUNCTION__, size);
        return QMAPI_FAILURE;
    }

    pHead->pStreamBuffer->dwSize = size;
    pHead->pStreamBuffer->dwTimeTick = demuxframe.timestamp;
    switch (demuxframe.flags) {
        case AUDIO_FRAME_RAW:
            frameType = MY_FRAME_TYPE_A;
            break;

        case VIDEO_FRAME_I:
            frameType = MY_FRAME_TYPE_I;
            break;

        case VIDEO_FRAME_P:
            frameType = MY_FRAME_TYPE_P;
            break;

        default:
            frameType = MY_FRAME_TYPE_A;
            break;
    }
    pHead->pStreamBuffer->FrameType = frameType;
    pHead->pStreamBuffer->dwTimeTick = demuxframe.timestamp;
    
    pstReadptr->pData = data;
    pstReadptr->dwPacketSize = pHead->pStreamBuffer->dwSize;
    pstReadptr->stFrameHeader.dwSize = sizeof(pstReadptr->stFrameHeader);
    pstReadptr->stFrameHeader.byAudioCode= QMAPI_PT_G711A;//g_jb_globel_param.Jb_ChannelPic_Param[0].stRecordPara.wFormatTag;//
    pstReadptr->stFrameHeader.byVideoCode = pHead->VideoFormat;
    pstReadptr->stFrameHeader.dwFrameIndex = pHead->index++;
    if ( pHead->pStreamBuffer->FrameType == MY_FRAME_TYPE_A) {
        pstReadptr->stFrameHeader.dwVideoSize = 0;
        pstReadptr->stFrameHeader.wAudioSize = pHead->pStreamBuffer->dwSize;
        pstReadptr->stFrameHeader.byFrameType = 4;//音频帧
        pstReadptr->stFrameHeader.dwTimeTick = pHead->pStreamBuffer->dwTimeTick;
    } else if ( pHead->pStreamBuffer->FrameType == MY_FRAME_TYPE_I) {
    	 ++pHead->IFrameIndex;
        
        unsigned long timeStamp = 0;
        unsigned long second = pHead->BeginTime.dwSecond;
        unsigned long minute = pHead->BeginTime.dwMinute;
        unsigned long hour = pHead->BeginTime.dwHour;
        int m = 0, n = 0;

        pHead->lastTimeTick = nLastTick;//更新LastTimeTick
        timeStamp = pHead->pStreamBuffer->dwTimeTick - pHead->firstStamp;
        timeStamp = timeStamp/1000;//取秒

        second += timeStamp;
        m = second/60;
        n = second%60;
        lpFrameTime->dwSecond = n;

        minute += m;
        m = minute/60;
        n = minute%60;
        lpFrameTime->dwMinute = n;

        hour += m;
        m = hour/24;
        n = hour%24;
        lpFrameTime->dwHour = n;

        pstReadptr->stFrameHeader.wAudioSize = 0;
        pstReadptr->stFrameHeader.dwVideoSize = pHead->pStreamBuffer->dwSize;
        pstReadptr->stFrameHeader.byFrameType = 0;
        pstReadptr->stFrameHeader.wVideoWidth = pHead->width;
        pstReadptr->stFrameHeader.wVideoHeight= pHead->height;
        pstReadptr->stFrameHeader.dwTimeTick = pHead->pStreamBuffer->dwTimeTick;
    } else if( pHead->pStreamBuffer->FrameType == MY_FRAME_TYPE_P) {
        pstReadptr->stFrameHeader.wAudioSize = 0;
        pstReadptr->stFrameHeader.dwVideoSize = pHead->pStreamBuffer->dwSize;
        pstReadptr->stFrameHeader.byFrameType = 1;
        pstReadptr->stFrameHeader.wVideoWidth = pHead->width;
        pstReadptr->stFrameHeader.wVideoHeight= pHead->height;
        pstReadptr->stFrameHeader.dwTimeTick = pHead->pStreamBuffer->dwTimeTick;
    }

    return QMAPI_SUCCESS;

}

void *PlayBackSendData(void *arg)
{
    unsigned char sleepcount = 0;
    int ret, playhandle = 0;
    QMAPI_TIME FrameTime;
    QMAPI_FILES_INFO *file;
    ULONGLONG DataTime; /* 绝对时间戳 毫秒*/
    QMAPI_NET_DATA_PACKET stReadptr;
    PlayDataCallBack_t *play = (PlayDataCallBack_t *)arg;
    prctl(PR_SET_NAME, (unsigned long)__FUNCTION__);
    pthread_detach(pthread_self()); /* 线程结束时自动清除 */

    while (play->thread_run) {
        if (!playhandle) {
            QMAPI_NET_FINDDATA FindData;
            ret = REC_FindNextFile(play->Handle, play->FindHandle, &FindData);
            if (ret ==  QMAPI_SEARCH_FILE_SUCCESS) {
                playhandle = PlayBackByName(play->Handle, FindData.csFileName);
                if (playhandle) {
                    struct tm st;
                    time_t FindTime, FileTime;
                    file = (QMAPI_FILES_INFO *)play->FindHandle;
                    st.tm_year = file->StartTime.dwYear - 1900;
                    st.tm_mon  = file->StartTime.dwMonth - 1;
                    st.tm_mday = file->StartTime.dwDay;
                    st.tm_hour = file->StartTime.dwHour;
                    st.tm_min  = file->StartTime.dwMinute;
                    st.tm_sec  = file->StartTime.dwSecond;
                    FileTime = mktime(&st);
                    st.tm_year = FindData.stStartTime.dwYear - 1900;
                    st.tm_mon  = FindData.stStartTime.dwMonth - 1;
                    st.tm_mday = FindData.stStartTime.dwDay;
                    st.tm_hour = FindData.stStartTime.dwHour;
                    st.tm_min  = FindData.stStartTime.dwMinute;
                    st.tm_sec  = FindData.stStartTime.dwSecond;
                    FindTime = mktime(&st);
                    if (FileTime > FindTime) {
                        BOOL bRet = FALSE;
                        printf("PlayBackSendData. File:%ld. Find:%ld. \n", FileTime, FindTime);
                        bRet = PlayBackSettoTime(playhandle, (DWORD)(FileTime-FindTime));
                        if (bRet == FALSE) {
                            PlayBackStop(play->Handle, playhandle);
                            playhandle = 0;
                            continue;
                        }
                        play->FileStartTime = FileTime;
                    } else {
                        play->FileStartTime = FindTime;
                    }
                    play->VideoStarttime = 0;
                    play->AudioStarttime = 0;
                    continue;
                }
            }

            /* 数据读取结束 */
            if (play->Callback) {
                play->Callback(play->Handle, 2, NULL, 0, stReadptr.stFrameHeader, play->dwUser);
            }
            printf("PlayBackSendData. ret:%d. play->FindHandle:%d. \n",ret,play->FindHandle);
            break;

        } else {/* 发送数据 */
            if (PlayBackGetReadPtrPos(play->Handle, playhandle, &stReadptr, &FrameTime) == QMAPI_SUCCESS) {
                if (play->Callback) {
                    DataTime = play->FileStartTime;
                    DataTime *= 1000;
                    if (stReadptr.stFrameHeader.dwVideoSize) {
                        if (!play->VideoStarttime) {
                            play->VideoStarttime = stReadptr.stFrameHeader.dwTimeTick;
                        }
                        DataTime += stReadptr.stFrameHeader.dwTimeTick - play->VideoStarttime;
                        stReadptr.stFrameHeader.dwTimeTick = DataTime;
                    } else if (stReadptr.stFrameHeader.wAudioSize) {
                        if (!play->AudioStarttime) {
                            play->AudioStarttime = stReadptr.stFrameHeader.dwTimeTick;
                        }
                        DataTime += stReadptr.stFrameHeader.dwTimeTick - play->AudioStarttime;
                        stReadptr.stFrameHeader.dwTimeTick = DataTime;
                    } else {
                        continue;
                    }
                    play->Callback(play->Handle, 2, stReadptr.pData, stReadptr.dwPacketSize, stReadptr.stFrameHeader, play->dwUser);
                }
            } else {/* 单个文件数据读取结束 */
                PlayBackStop(play->Handle, playhandle);
                playhandle = 0;
            }
        }

        if (sleepcount++ > 64) {
            sleepcount = 0;
            usleep(12);
        }
    }

    if (playhandle) {
        PlayBackStop(play->Handle, playhandle);
        playhandle = 0;
    }
    play->thread_over = 1;

    return NULL;
}

int PlayBackAddDataCallBack(int Handle, int FindHandle, CBPlayProc Callback, DWORD dwUser)
{
    if (FindHandle && Callback)
    {
        int i;
        REC_PTHREAD_MUTEX_LOCK(&play_data_call_back);
        if(g_playBackStop)
            return QMAPI_FAILURE;
        for (i=0; i<MAX_PLAY_CALL_BACK; i++)
        {
            if (!PlayDataCallBack[i].buser)
            {
                PlayDataCallBack[i].buser      = 1;
                PlayDataCallBack[i].thread_run = 1;
                PlayDataCallBack[i].Handle     = Handle;
                PlayDataCallBack[i].FindHandle = FindHandle;
                PlayDataCallBack[i].dwUser     = dwUser;
                PlayDataCallBack[i].Callback   = Callback;
                break;
            }
        }
        REC_PTHREAD_MUTEX_UNLOCK(&play_data_call_back);
        if (i < MAX_PLAY_CALL_BACK)
        {
            pthread_t th;
            if (pthread_create(&th, NULL, PlayBackSendData, (void *)(&PlayDataCallBack[i])) == 0)
            {
                return QMAPI_SUCCESS;
            }
        }
    }
    return QMAPI_FAILURE;
}

int PlayBackDelDataCallBack(int Handle, int FindHandle, CBPlayProc Callback, DWORD dwUser)
{
    int i;
    REC_PTHREAD_MUTEX_LOCK(&play_data_call_back);
    if(g_playBackStop)
        return QMAPI_SUCCESS;
    for (i=0; i<MAX_PLAY_CALL_BACK; i++)
    {
        if (PlayDataCallBack[i].buser 
                && PlayDataCallBack[i].Handle == Handle
                && PlayDataCallBack[i].FindHandle == FindHandle
                && PlayDataCallBack[i].Callback == Callback)
        {
            PlayDataCallBack[i].thread_run = 0;
            while (!PlayDataCallBack[i].thread_over) /* 等待线程结束*/
                usleep(20);
            memset(&PlayDataCallBack[i], 0, sizeof(PlayDataCallBack_t));
            break;
        }
    }
    REC_PTHREAD_MUTEX_UNLOCK(&play_data_call_back);

    return (i<MAX_PLAY_CALL_BACK ? QMAPI_SUCCESS : QMAPI_FAILURE);
}

int PlayBackDelAll(void)
{
    int i;
    REC_PTHREAD_MUTEX_LOCK(&play_data_call_back);
    g_playBackStop = 1;
    for (i=0; i<MAX_PLAY_CALL_BACK; i++)
    {
        if (PlayDataCallBack[i].buser)
        {
            PlayDataCallBack[i].thread_run = 0;
            while (!PlayDataCallBack[i].thread_over) /* 等待线程结束*/
                usleep(100);
            memset(&PlayDataCallBack[i], 0, sizeof(PlayDataCallBack_t));
            break;
        }
    }
    REC_PTHREAD_MUTEX_UNLOCK(&play_data_call_back);

    return QMAPI_SUCCESS;
}

//-----------------------------------------------
int REC_FindFile(QMAPI_NET_FINDDATA *pFileDataArray, 
        int maxNum,
        unsigned int nFileType,
        unsigned int nChannel,
        QMAPI_TIME struStartTime,
        QMAPI_TIME struStopTime)
{
    int nFileYear   = 0;
    int nFileMonth  = 0;
    int nFileDay        = 0;
    int nStartDate  = 0;
    int nStartTime  = 0;
    int nStopDate   = 0;
    int nStopTime   = 0;
    int nFileDate   = 0;
    int nFileNum        = 0;
    int nTempFileNum = 0;
    int i = 0;
    char csDirPath[256];
    DIR* dir = NULL;
    BOOL bSearchbyTime = FALSE;

    QMAPI_NET_FINDDATA *pFileData = (QMAPI_NET_FINDDATA *)pFileDataArray;


    if( struStartTime.dwYear    == 0 && struStartTime.dwMonth   == 0 && struStartTime.dwDay         == 0 && 
            struStartTime.dwHour == 0 && struStartTime.dwMinute     == 0 && struStartTime.dwSecond  == 0 &&
            struStopTime.dwYear     == 0 && struStopTime.dwMonth    == 0 && struStopTime.dwDay      == 0 && 
            struStopTime.dwHour     == 0 && struStopTime.dwMinute   == 0 && struStopTime.dwSecond   == 0 )
    {
        bSearchbyTime = FALSE;
    }
    else
    {
        nStartDate = struStartTime.dwYear*360 + struStartTime.dwMonth*30 + struStartTime.dwDay;
        nStopDate = struStopTime.dwYear*360 + struStopTime.dwMonth*30 + struStopTime.dwDay;
#if 1
        printf("### StartTime:%lu-%lu-%lu %lu:%lu:%lu\n",
                struStartTime.dwYear, struStartTime.dwMonth, struStartTime.dwDay, 
                struStartTime.dwHour, struStartTime.dwMinute, struStartTime.dwSecond);
        printf("### StopTime :%lu-%lu-%lu %lu:%lu:%lu\n",
                struStopTime.dwYear, struStopTime.dwMonth, struStopTime.dwDay, 
                struStopTime.dwHour, struStopTime.dwMinute, struStopTime.dwSecond);
#endif      
        bSearchbyTime = TRUE;       
    }
    QMAPI_NET_HDCFG hd;
    int nRet=QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_HDCFG, 0, &hd, sizeof(QMAPI_NET_HDCFG));
    if(!(nRet==0 && hd.dwHDCount>=1 && hd.stHDInfo[0].dwHdStatus==0)){//No mount or ...
        printf("###REC_FindFile nRet:%d. count:%lu. staus:%lu. byPath:%s.\n",nRet, hd.dwHDCount,hd.stHDInfo[0].dwHdStatus,hd.stHDInfo[i].byPath);
        return nFileNum;
    }

    for(i = 0; i < hd.dwHDCount; i++)
    {
        if(!(hd.stHDInfo[i].dwHdStatus==0 && hd.stHDInfo[i].byPath[0]))
            continue;

        nFileYear = struStartTime.dwYear;
        nFileMonth = struStartTime.dwMonth;
        nFileDay = struStartTime.dwDay;
        while((nFileYear<struStopTime.dwYear)
                || (nFileYear==struStopTime.dwYear && nFileMonth<struStopTime.dwMonth)
                || (nFileYear==struStopTime.dwYear && nFileMonth == struStopTime.dwMonth && nFileDay<=struStopTime.dwDay))
        {
            sprintf(csDirPath, "%s/%04d%02d%02d", hd.stHDInfo[i].byPath, nFileYear, nFileMonth, nFileDay);
            dir = opendir(csDirPath);
            if(dir)
            {
                closedir(dir);
                if( bSearchbyTime )
                {
                    nFileDate = nFileYear*360 + nFileMonth*30 + nFileDay;
                    if( nStartDate > nFileDate || nStopDate < nFileDate )
                        continue;
                    /* 跨天查询 */
                    if (nFileDate != nStartDate)
                        nStartTime = 0;
                    else
                        nStartTime = struStartTime.dwHour *3600+ struStartTime.dwMinute*60 + struStartTime.dwSecond;
                    if (nFileDate != nStopDate)
                        nStopTime =  23*3600 + 59*60 + 59;
                    else
                        nStopTime =  struStopTime.dwHour*3600 + struStopTime.dwMinute*60 + struStopTime.dwSecond;

                }

#if 0
                nTempFileNum = JB_search_file_list_from_date_dir_add(csDirPath, pFileData, maxNum-nFileNum, 
                        bSearchbyTime, nStartTime, nStopTime, 
                        nFileYear, nFileMonth, nFileDay, nChannel, nFileType);
#endif

                nTempFileNum = TL_search_file_list_from_date_dir_add(csDirPath, pFileData, maxNum-nFileNum, 
                        bSearchbyTime, nStartTime, nStopTime, 
                        nFileYear, nFileMonth, nFileDay, nChannel, nFileType);

                nFileNum += nTempFileNum;
                pFileData += nTempFileNum;

            }
            else
                printf("FUNC:%s(%d), opendir(%s) failed!!\n",__FUNCTION__, __LINE__, csDirPath);

            //下一天
            time_t tval;
            struct tm tm_val;
            memset(&tm_val, 0, sizeof(tm_val));
            tm_val.tm_year = nFileYear-1900;
            tm_val.tm_mon = nFileMonth -1;
            tm_val.tm_mday = nFileDay;
            tval = mktime(&tm_val);

            tval += 3600*24;
            localtime_r(&tval, &tm_val);
            nFileYear = tm_val.tm_year + 1900;
            nFileMonth = tm_val.tm_mon + 1;
            nFileDay = tm_val.tm_mday;
        }

    }

#if 1 //def FIND_FILE_DEBUG  
    printf("REC_FindFile return nFileNum:%d\n",nFileNum);
    printf("leave REC_FindFile>>>>>>>>>>>>>>>>>>>>>>>>>\n");     
#endif
    return nFileNum;
}


int REC_FindNextFile(int Handle, int FindHandle, LPQMAPI_NET_FINDDATA lpFindData)
{
    QMAPI_FILES_INFO *pFilesInfo = (QMAPI_FILES_INFO*)FindHandle;
    QMAPI_NET_FINDDATA *lpFileData = NULL;
    QMAPI_NET_FINDDATA *pInfo = NULL;

    if (!pFilesInfo)
    {
        return QMAPI_FAILURE;
    }
    if (pFilesInfo->CurNum > pFilesInfo->EndIndex)
    {
        printf("## dms_sysnetapi_FindNextFile-pFilesInfo->CurNum:%d,pFilesInfo->EndIndex:%d!\n",
                pFilesInfo->CurNum, pFilesInfo->EndIndex);
        return QMAPI_SEARCH_NOMOREFILE;
    }
    lpFileData = (QMAPI_NET_FINDDATA*)pFilesInfo->szBuf;

    pInfo = &lpFileData[pFilesInfo->CurNum];
    memcpy(lpFindData->csFileName,pInfo->csFileName,sizeof(lpFindData->csFileName));
    lpFindData->byLocked = 0;
    lpFindData->dwFileSize = pInfo->dwFileSize;
    lpFindData->dwFileType = pInfo->dwFileType;
    lpFindData->stStartTime.dwYear = pInfo->stStartTime.dwYear;
    lpFindData->stStartTime.dwMonth = pInfo->stStartTime.dwMonth;
    lpFindData->stStartTime.dwDay= pInfo->stStartTime.dwDay;
    lpFindData->stStartTime.dwHour = pInfo->stStartTime.dwHour;
    lpFindData->stStartTime.dwMinute= pInfo->stStartTime.dwMinute;
    lpFindData->stStartTime.dwSecond= pInfo->stStartTime.dwSecond;

    lpFindData->stStopTime.dwYear = pInfo->stStopTime.dwYear;
    lpFindData->stStopTime.dwMonth = pInfo->stStopTime.dwMonth;
    lpFindData->stStopTime.dwDay= pInfo->stStopTime.dwDay;
    lpFindData->stStopTime.dwHour = pInfo->stStopTime.dwHour;
    lpFindData->stStopTime.dwMinute= pInfo->stStopTime.dwMinute;
    lpFindData->stStopTime.dwSecond= pInfo->stStopTime.dwSecond;

    pFilesInfo->CurNum++;

    return QMAPI_SEARCH_FILE_SUCCESS;
}


/*
	查询当前文件夹下的文件

		lpDir  /mnt/mmc1/20160906
*/
static int TL_search_file_list_from_channel_dir_add(const char *lpDir,
                                             QMAPI_NET_FINDDATA_LIST **ppFileList,
                                             int maxNum, 
                                             BOOL bSearchbyTime,
                                             int nStartTime,
                                             int nStopTime, 
                                             int nFileYear,
                                             int nFileMonth,
                                             int nFileDay,
                                             int nChannel,
                                             int nFileType)
{

    int nFileHour   = 0;
    int nFileMin        = 0;
    int nFileSecond     = 0;
    int nFileTime   = 0;    
    int i, nFileNum        = 0;
	int FileType;
    char csBuf[128];
    char name[256];
	log_head_t *head;
	log_content_t cont;
	log_search_t search = {0};
	int nSearchNum = 0;
    DIR* dir = NULL;
    struct stat FileInfo;
    struct dirent *pDirent = NULL;
    struct dirent s_dir;
    
    struct tm tim;
	QMAPI_NET_FINDDATA_LIST *pHeadItem = NULL;
    
    sprintf(csBuf, "%s/%d", lpDir,nChannel);
    dir = opendir(csBuf);
    if(dir == NULL)
    {
        printf("TL_search_file_list_from_channel_dir_add: opendir(%s) failed\n", csBuf);
        return 0;
    }
	sprintf(name, "%s/index.dat", lpDir);
	head = log_init(name, 0);
	if (head)
	{
		printf("log_init ok name:%s. \n",name);
	}
    //DATE dir
    while(!readdir_r(dir, &s_dir, &pDirent) && pDirent) 
    {
        if(nFileNum  > maxNum)
        {
            break;
        }
        
        if ((strcmp(s_dir.d_name,".")==0) || (strcmp(s_dir.d_name,"..")==0) || (s_dir.d_type == DT_DIR))
            continue;   
        
        sprintf(name, "%s/%s", csBuf, s_dir.d_name);
        stat(name, &FileInfo);
        
        sscanf(s_dir.d_name, "%02d%02d%02d.mp4", &nFileHour, &nFileMin, &nFileSecond);
        //printf("After sscanf:%02d%02d%02d.mp4\n",nFileHour, nFileMin, nFileSecond);

        if( bSearchbyTime )
        {
            nFileTime = nFileHour*3600 + nFileMin*60 + nFileSecond;

            if (nStartTime > nFileTime)/* 当传入的时间点为一个文件的时间区间内，也把当前文件反馈到检索列表内*/
            {
                int EndTime;

                localtime_r(&FileInfo.st_mtime, &tim);
                EndTime = tim.tm_hour * 3600 + tim.tm_min * 60 + tim.tm_sec;
                if (nStartTime >= EndTime)
                {
                    continue;
                }
            }
            
            if (nStopTime < nFileTime )
            {
                continue;
            }
            
            if (nSearchNum==0 && head)
            {
                struct tm st;
                search.log_type = 0xFF;
                st.tm_year = nFileYear - 1900;
                st.tm_mon  = nFileMonth - 1;
                st.tm_mday = nFileDay;
                st.tm_hour = nFileHour;
                st.tm_min  = nFileMin;
				st.tm_sec  = nFileSecond;
#if 0			
                search.log_begin = mktime(&st);
				search.log_end = FileInfo.st_mtime;
				nSearchNum = log_search(head, &search);
				printf("log_search log:%ld %ld. %d.\n",search.log_begin, search.log_end, nSearchNum);
#endif
            }
        }
#if 0
        if(IsRecordingFile(name, nChannel, STOR_LOCAL_HD))
        {
            printf("channel %d file %s  is recording and skipped \n",nChannel,name);
            continue;
        }
#endif

#if 0
        FileType = 0;
		if (nSearchNum > 0)
		{
			for (i=0; i<nSearchNum; i++)
			{
				if (log_read(head, &search, &cont, 1) == 1)
				{
					//printf("%s ch:%u. type:%u opt:%u. time:%ld text:%s.\n",__FUNCTION__,cont.log_ch,cont.log_type,cont.log_opt,cont.log_time,cont.log_text);
					if ((cont.log_opt & 1) == 1)
						FileType |= cont.log_type;
				}
			}
			nSearchNum = 0;
		}

		if (!(nFileType & FileType))
		{
			printf("TL_search_file_list_from_channel_dir_add name:%s. filetype:%d.%d. \n", name,FileType, nFileType);
			continue;
		}
#endif
		QMAPI_NET_FINDDATA_LIST *pFileData = (QMAPI_NET_FINDDATA_LIST*)malloc(sizeof(QMAPI_NET_FINDDATA_LIST));
		memset(pFileData, 0, sizeof(QMAPI_NET_FINDDATA_LIST));
		strcpy(pFileData->data.csFileName, name);
        pFileData->data.stStartTime.dwYear     = nFileYear;
        pFileData->data.stStartTime.dwMonth    = nFileMonth;
        pFileData->data.stStartTime.dwDay      = nFileDay;
        pFileData->data.stStartTime.dwHour     = nFileHour;
        pFileData->data.stStartTime.dwMinute   = nFileMin;
        pFileData->data.stStartTime.dwSecond   = nFileSecond;
        
        time_t timeStart, timeEnd;
		struct tm tmStart;
		memset(&tmStart, 0, sizeof(tmStart));
		tmStart.tm_year = nFileYear-1900;
		tmStart.tm_mon = nFileMonth-1;
		tmStart.tm_mday = nFileDay;
		tmStart.tm_hour = nFileHour;
		tmStart.tm_min = nFileMin;
		tmStart.tm_sec = nFileSecond;
		timeStart = mktime(&tmStart);


        //printf("%s stattime %4d:%2d:%2d-%2d:%2d:%2d\n", __FUNCTION__, 
        //        nFileYear, nFileMonth, nFileDay, 
        //        nFileHour, nFileMin, nFileSecond);

		if(/*abs(FileInfo.st_mtime-timeStart)>(2*gs_MaxRecordTime/1000)*/0)
		{
			//timeEnd = timeStart + gs_MaxRecordTime/1000;
            //localtime_r(&timeEnd, &tim);

            //printf("%s timestart:%ld st_mtime:%ld timeEnd:%ld\n", __FUNCTION__, FileInfo.st_mtime, timeStart, timeEnd);
        }
        else
        {
            localtime_r(&FileInfo.st_mtime, &tim);
            printf("%s timestart:%ld st_mtime:%ld\n", __FUNCTION__, timeStart, FileInfo.st_mtime);
        }

        pFileData->data.stStopTime.dwYear   = tim.tm_year + 1900;
        pFileData->data.stStopTime.dwMonth  = tim.tm_mon+1;
        pFileData->data.stStopTime.dwDay    = tim.tm_mday;
        pFileData->data.stStopTime.dwHour   = tim.tm_hour;
        pFileData->data.stStopTime.dwMinute = tim.tm_min;
        pFileData->data.stStopTime.dwSecond = tim.tm_sec;
      
        //printf("%s endtime %4d:%2d:%2d-%2d:%2d:%2d\n", __FUNCTION__, 
        //        tim.tm_year+1900, tim.tm_mon+1, tim.tm_mday, 
        //        tim.tm_hour, tim.tm_min, tim.tm_sec);

        pFileData->data.dwFileSize   = FileInfo.st_size;
        pFileData->data.nChannel     = nChannel;
        pFileData->data.dwFileType   = FileType;
        pFileData->data.dwSize = sizeof(QMAPI_NET_FINDDATA);

        if(pHeadItem == NULL)
		{
			pHeadItem = pFileData;
		}
		else
		{
			
			QMAPI_NET_FINDDATA_LIST *pItem = pHeadItem;
			while(pItem)
			{
				char *p1 = strrchr(pItem->data.csFileName, '/');
				char *p2 = strrchr(pFileData->data.csFileName, '/');
				int time1 = atoi(p1+1);
				int time2 = atoi(p2+1);
				
				if(time2 < time1)
				{
					if(pItem->pPre)
					{
						pFileData->pPre = pItem->pPre;
						pItem->pPre->pNext = pFileData;
						pFileData->pNext = pItem;
						pItem->pPre = pFileData;
					}
					else
					{
						pItem->pPre = pFileData;
						pFileData->pNext = pItem;
						pHeadItem = pFileData;
					}
					break;
				}
				else if(pItem->pNext == NULL)
				{
					pItem->pNext = pFileData;
					pFileData->pPre = pItem;
					break;
				}

				pItem = pItem->pNext;
			}
		}
        
        nFileNum ++;
        
    }
    log_exit(head);
    closedir(dir);

	*ppFileList = pHeadItem;
    
#ifdef FIND_FILE_DEBUG  
    printf("leave TL_search_file_list_from_channel_dir_add>>>>>>>>>>>>>>>>>>>>>>>>>\n");    
#endif
    
    return nFileNum;
}


int TL_search_file_list_from_date_dir_add(const char *lpDir,
    QMAPI_NET_FINDDATA *pFileDataArray,
    int maxNum, 
    BOOL bSearchbyTime,
    int nStartTime,
    int nStopTime, 
    int nFileYear,
    int nFileMonth,
    int nFileDay,
    int nChannel,
    int nFileType)
{
    QMAPI_NET_FINDDATA *pFileData = (QMAPI_NET_FINDDATA *)pFileDataArray;
    int  nFileNum = 0, nTempFileNum = 0;
    char csBuf[128];
    char name[256];
    DIR* dir = NULL;
    struct dirent *pDirent = NULL;
    struct dirent s_dir;
    int nFileChannel = 0;

    sprintf(csBuf, "%s", lpDir);
    dir = opendir(csBuf);
    if(dir == NULL)
    {
        printf("TL_search_file_list_from_date_dir_add: opendir(%s) failed\n",csBuf);
        return 0;
    }
    while(!readdir_r(dir, &s_dir, &pDirent) && pDirent) 
    {
        if(nFileNum  >= maxNum)
        {
            break;
        }
        if ((strcmp(s_dir.d_name,".")==0)||(strcmp(s_dir.d_name,"..")==0) || (s_dir.d_type != DT_DIR))
            continue;
        if(strstr(s_dir.d_name,"index") != NULL)
        {
            continue;
        }
        sprintf(name, "%s/%s", csBuf, s_dir.d_name);
 
        nFileChannel = atoi(s_dir.d_name);

        if( nChannel != 0xff && nChannel != nFileChannel && nChannel != -1)
            continue;
 
		QMAPI_NET_FINDDATA_LIST *pFileInfo = NULL;
        nTempFileNum = TL_search_file_list_from_channel_dir_add(csBuf, &pFileInfo, maxNum-nFileNum, 
            bSearchbyTime, nStartTime, nStopTime, nFileYear, nFileMonth, nFileDay, nFileChannel, nFileType);

		if(nTempFileNum>0 && pFileInfo)
		{
			QMAPI_NET_FINDDATA_LIST *pDel = NULL;
			QMAPI_NET_FINDDATA_LIST *pItem = pFileInfo;
			while((nFileNum < maxNum) && pItem)
			{
				memcpy(pFileData, &pItem->data, sizeof(QMAPI_NET_FINDDATA));
				pItem = pItem->pNext;
				nFileNum++;
				pFileData++;
			}

			pItem = pFileInfo;
			while(pItem)
			{
				pDel = pItem;
				pItem = pItem->pNext;
				free(pDel);
			}
		}
 
    }

    closedir(dir);

//#ifdef FIND_FILE_DEBUG  
    printf("### leave TL_search_file_list_from_date_dir_add %d,%d\n", nTempFileNum, nFileNum);   
//#endif

    return nFileNum;
}

int PlayBackControl(int Handle, int PlayHandle, DWORD dwControlCode, const char *lpInBuffer, DWORD dwInLen, char *lpOutBuffer, DWORD *lpOutLen)
{
    switch(dwControlCode)
    {
    case QMAPI_REC_PLAYSETPOS:
		{
			PLAYFILE_INFO *pPlayFileInfo = (PLAYFILE_INFO*)PlayHandle;
			
			if(NULL == pPlayFileInfo)
			{	printf("No PlayFileInfo...\n");
				return QMAPI_SUCCESS;
			}
			if(!lpInBuffer || dwInLen!=4)
			{
				printf("dwInLen %lu...\n", dwInLen);
				return QMAPI_FAILURE;
			}
			
			DWORD nSeek=*((int *)lpInBuffer);	//指定毫秒数
			BOOL blSetPos=FALSE;
			printf("### play set pos(%lu/%lu) %02f.\n", nSeek, pPlayFileInfo->dwTotalTime, nSeek*100.0/pPlayFileInfo->dwTotalTime);
			if(nSeek<=pPlayFileInfo->dwTotalTime){
				blSetPos=PlayBackSettoTime(PlayHandle,nSeek/1000);// 2*60*1000, 2 Min
			}
			printf("### end of set pos %s..\n", blSetPos?"OK":"Failed");
		}
        return QMAPI_SUCCESS;
    case QMAPI_REC_PLAYFRAME:
	{
		if(!lpInBuffer || dwInLen != sizeof(DWORD))
		{
			printf("dwInLen %lu...\n", dwInLen);
			return QMAPI_FAILURE;
		}

		DWORD mode = *((DWORD *)lpInBuffer);
		return SetPlayBackMode(Handle, PlayHandle, mode);
	}
    default:
        break;
    }
	return QMAPI_FAILURE;
}

