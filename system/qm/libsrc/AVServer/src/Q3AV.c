
#include <sys/prctl.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "MediaBuffer.h"
#include "MediaBufferErrno.h"

#include "Q3AV.h"
#include "basic.h"
#include "video.h"
#include "ipc.h"

#define LOGTAG  "VIDEO"

/*!< CFG_FEATURE_MARKER */
#define CFG_MARKER_FONT_COLOR           0x00FFFFFF  /*!< 32bit 0/R/G/B */
#define CFG_MARKER_BACK_COLOR           0x80CCCCCC  /*!< 32bit A/R/G/B */

//#ifdef DMS_SYSNET_PT_SUPPORT
CBVideoProc         m_pfQ3MainVideoCallBk = NULL;
CBVideoProc         m_pfQ3SecondVideoCallBk = NULL;
CBVideoProc         m_pfQ3MobileVideoCallBk = NULL;
//#endif

int Q3wHeight[QMAPI_MAX_STREAM_TYPE_NUM];
int Q3wWidth[QMAPI_MAX_STREAM_TYPE_NUM];
int u32BlockSize[QMAPI_MAX_STREAM_TYPE_NUM];


#define JPEG_CHANNEL_NAME "jpeg-out"
#define VIDEO_STREAM_MAX  3
typedef struct {
    pthread_mutex_t mutex;
    int channel; //qm channel means which camera 
    int stream;  //qm stream means which stream seperate from one camera
    int enable;
    int waitIFrame;

    unsigned long long s64Time;
    unsigned int s32Time;

    int media_type;
    char *rtp_info;
    void *priv;
} video_stream_t;

static pthread_mutex_t sMediaMutex = PTHREAD_MUTEX_INITIALIZER;
static video_stream_t sVideoStream[VIDEO_STREAM_MAX];


static int Q3_AV_SetVideoFrameRate(int nStreamType, int nFrameRate);
static int Q3_AV_SetVideoBitRate(int nStreamType, int nBitRate, int rcMode, 
        int pFrame, int imagQuality);
static void Q3_Video_Stream_Handler(void const *client, media_vframe_t const *vfr);

int Get_Q3_Vi_Height(DWORD dwTvMode, DWORD dwVFormat)
{
    int u32PicHeight = QMAPI_NTSC_CIF_FORMAT_HEIGHT;
    ipclog_info("%s dwVFormat=%ld\n", __FUNCTION__, dwVFormat);

    switch(dwVFormat)
    {//分辨率   (0为CIF,1为D1,2为HALF-D1,3为QCIF)
    case QMAPI_VIDEO_FORMAT_640x360:
        u32PicHeight = 360;
        break;
    case QMAPI_VIDEO_FORMAT_CIF:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicHeight = QMAPI_NTSC_CIF_FORMAT_HEIGHT;
        }
        else 
        {
            u32PicHeight = QMAPI_PAL_CIF_FORMAT_HEIGHT;
        }
        break;
    case QMAPI_VIDEO_FORMAT_D1:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicHeight = QMAPI_NTSC_D1_FORMAT_HEIGHT;
        }
        else 
        {
            u32PicHeight = QMAPI_PAL_D1_FORMAT_HEIGHT;
        }
        break;
    case QMAPI_VIDEO_FORMAT_HD1:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicHeight = QMAPI_NTSC_HD1_FORMAT_HEIGHT;
        }
        else 
        {
            u32PicHeight = QMAPI_PAL_HD1_FORMAT_HEIGHT;
        }
        break;
    case QMAPI_VIDEO_FORMAT_QCIF:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicHeight = QMAPI_NTSC_QCIF_FORMAT_HEIGHT;
        }
        else 
        {
            u32PicHeight = QMAPI_PAL_QCIF_FORMAT_HEIGHT;
        }
        break;
    case QMAPI_VIDEO_FORMAT_VGA:
        u32PicHeight = QMAPI_VGA_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_SVGA:
        u32PicHeight = QMAPI_SVGA_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_QVGA:
        u32PicHeight = QMAPI_QVGA_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_UXGA:
        u32PicHeight = QMAPI_UXGA_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_130H:
        u32PicHeight = QMAPI_HH130H_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_720P:
        u32PicHeight = QMAPI_HH720P_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_960P:
        u32PicHeight = QMAPI_960P_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_300H:
        u32PicHeight = QMAPI_HH300H_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_QQVGA:
        u32PicHeight = QMAPI_QQVGA_FORMAT_HEIGHT;
        break;
    case QMAPI_VIDEO_FORMAT_1080P:
        u32PicHeight = QMAPI_HH1080P_FORMAT_HEIGHT;
        break;
    default:
        ipclog_error("NOT Support VFormat %ld \n",dwVFormat);
        break;
    }

    return u32PicHeight;
}

/*获取视频分辨率宽像素*/
int Get_Q3_Vi_Width(DWORD dwTvMode, DWORD dwVFormat)
{
    int u32PicWidth =  QMAPI_VIDEO_FORMAT_CIF;
    switch(dwVFormat)
    {//分辨率   (0为CIF,1为D1,2为HALF-D1,3为QCIF)
    case QMAPI_VIDEO_FORMAT_640x360:
        u32PicWidth = 640;
        break;
    case QMAPI_VIDEO_FORMAT_CIF:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicWidth = QMAPI_NTSC_CIF_FORMAT_WIDTH;
        }
        else 
        {
            u32PicWidth = QMAPI_PAL_CIF_FORMAT_WIDTH;
        }
        break;
    case QMAPI_VIDEO_FORMAT_D1:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicWidth = QMAPI_NTSC_D1_FORMAT_WIDTH;
        }
        else 
        {
            u32PicWidth = QMAPI_PAL_D1_FORMAT_WIDTH;
        }
        break;
    case QMAPI_VIDEO_FORMAT_HD1:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicWidth = QMAPI_NTSC_HD1_FORMAT_WIDTH;
        }
        else 
        {
            u32PicWidth = QMAPI_PAL_HD1_FORMAT_WIDTH;
        }
        break;
    case QMAPI_VIDEO_FORMAT_QCIF:
        if(dwTvMode == videoStandardNTSC)
        {
            u32PicWidth = QMAPI_NTSC_QCIF_FORMAT_WIDTH;
        }
        else 
        {
            u32PicWidth = QMAPI_PAL_QCIF_FORMAT_WIDTH;
        }
        break;
    case QMAPI_VIDEO_FORMAT_VGA:
        u32PicWidth = QMAPI_VGA_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_SVGA:
        u32PicWidth = QMAPI_SVGA_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_QVGA:
        u32PicWidth = QMAPI_QVGA_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_UXGA:
        u32PicWidth = QMAPI_UXGA_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_130H:
        u32PicWidth = QMAPI_HH130H_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_720P:
        u32PicWidth = QMAPI_HH720P_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_960P:
        u32PicWidth = QMAPI_960P_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_300H:
        u32PicWidth = QMAPI_HH300H_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_QQVGA:
        u32PicWidth = QMAPI_QQVGA_FORMAT_WIDTH;
        break;
    case QMAPI_VIDEO_FORMAT_1080P:
        u32PicWidth = QMAPI_HH1080P_FORMAT_WIDTH;
        break;
        
    default:
        ipclog_error("NOT Support VFormat %ld\n",dwVFormat);
        break;
    }

    return u32PicWidth;
}

/*获取视频分辨率宽像素*/
int Get_Q3_Vi_Format(int w , int h, DWORD *dwVFormat)
{
    *dwVFormat =  QMAPI_VIDEO_FORMAT_CIF;
    if ((w == 2624) && (h == 1520)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_300H;

    } else if ((w == 2176) && (h == 1088)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_300H;
    }
    else if ((w == 2048) && (h == 1536)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_300H;

    } else if ((w == 1280) && (h == 1024)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_130H;

    } else if ((w == 1536) && (h == 1536)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_1080P;

    } else if ((w == 1920) && (h == 1080)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_1080P;

    } else if ((w == 1600) && (h == 1200)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_UXGA;

    } else if ((w == 1280) && (h == 960)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_960P;

    } else if ((w == 1280) && (h == 720)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_720P;

    } else if ((w == 704) && (h == 576)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_D1;

    } else if ((w == 640) && (h == 640)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_VGA;

    } else if ((w == 640) && (h == 480)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_VGA;

    } else if ((w == 640) && (h == 360)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_640x360;

    } else if ((w == 360) && (h == 360)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_640x360;

    } else if ((w == 352) && (h == 288)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_CIF;

    }  else if ((w == 176) && (h == 144)) {
        *dwVFormat = QMAPI_VIDEO_FORMAT_QCIF;

    }  else {
        ipclog_warn("%s cannot match dwVFormat for %d*%d\n", __FUNCTION__, w, h);
        return -1;
    }

    ipclog_info("%s dwVFormat=%ld\n", __FUNCTION__, *dwVFormat);

    return 0;
}

static int Q3_Video_Stream_Init(void)
{
    int i = 0;
    ipclog_info("%s start\n", __FUNCTION__);
    pthread_mutex_lock(&sMediaMutex);
    for (i = 0; i < VIDEO_STREAM_MAX; i++) {
        sVideoStream[i].rtp_info  = malloc(sizeof(SPS_PPS_VALUE));  
        if (!sVideoStream[i].rtp_info) {
            ipclog_error("%s Out of memory for stream:%d psp\n", __FUNCTION__, i);
            pthread_mutex_unlock(&sMediaMutex);
            return -1;
        }
        memset(sVideoStream[i].rtp_info, 0, sizeof(SPS_PPS_VALUE));
        
        sVideoStream[i].waitIFrame = 1;
        sVideoStream[i].channel = 0;
        sVideoStream[i].stream = i;
        sVideoStream[i].enable = 0;
        sVideoStream[i].priv = NULL;
    }
    
    pthread_mutex_unlock(&sMediaMutex);
    ipclog_info("%s done\n", __FUNCTION__);
    return 0;    
}

static int Q3_Video_Stream_Deinit(void)
{
    int i = 0;
    ipclog_info("%s start\n", __FUNCTION__);
    
    pthread_mutex_lock(&sMediaMutex);
    for(i = 0; i < VIDEO_STREAM_MAX; i++) {
        if (sVideoStream[i].rtp_info) {
            free(sVideoStream[i].rtp_info);
            sVideoStream[i].rtp_info = NULL;
        }
    }

    pthread_mutex_unlock(&sMediaMutex);
    ipclog_info("%s done\n", __FUNCTION__); 
    return 0;
}

int Q3_Codec_Init(QMAPI_NET_CHANNEL_PIC_INFO *pstChannelInfo, int Resolution, int ManBlockSize, int SubBlockSize)
{
    int i = 0;
    
    Q3_Video_Stream_Init();
    
    infotm_video_init();

    //update dwCompressionType here, for multi product support, write back to config later
    ven_info_t *ven_info = infotm_video_info_get(0);
    if (ven_info->media_type == VIDEO_MEDIA_H264) {
        pstChannelInfo->stRecordPara.dwCompressionType  = QMAPI_PT_H264;    
        pstChannelInfo->stNetPara.dwCompressionType     = QMAPI_PT_H264;    
        pstChannelInfo->stPhonePara.dwCompressionType   = QMAPI_PT_H264;   
        pstChannelInfo->stEventRecordPara.dwCompressionType = QMAPI_PT_H264;   
    } else {
        pstChannelInfo->stRecordPara.dwCompressionType  = QMAPI_PT_H265;    
        pstChannelInfo->stNetPara.dwCompressionType     = QMAPI_PT_H265;    
        pstChannelInfo->stPhonePara.dwCompressionType   = QMAPI_PT_H265;   
        pstChannelInfo->stEventRecordPara.dwCompressionType = QMAPI_PT_H265;   
    }

    int num = infotm_video_info_num_get();
    for (i = 0; i < num; i++)
    {
        QMAPI_NET_COMPRESSION_INFO  *strCompressionPara = NULL;
        //In fact, should not limit BitRate and fps here, should limit when setting
        if (0 == i) {
            strCompressionPara = &pstChannelInfo->stRecordPara;
            u32BlockSize[QMAPI_MAIN_STREAM] = ManBlockSize;
            //limit BitRate 16k~8*1024K 
            if (strCompressionPara->dwBitRate<16*1024) {
                ipclog_info("%s main channel invalid bitRate:%lu\n",__FUNCTION__, strCompressionPara->dwBitRate);
                strCompressionPara->dwBitRate = 16*1024;
            } else if(strCompressionPara->dwBitRate > 8*1024*1024) {
                ipclog_info("%s main channel invalid bitRate:%lu\n",__FUNCTION__,strCompressionPara->dwBitRate);
				strCompressionPara->dwBitRate = 8*1024*1024;
			}
        } else { //second channel
            strCompressionPara = &pstChannelInfo->stNetPara;
            u32BlockSize[QMAPI_SECOND_STREAM] = SubBlockSize;
            //limit BitRate 16k~1024K 
            if (strCompressionPara->dwBitRate<16*1024) {
                ipclog_info("%s main channel invalid bitRate:%lu\n",__FUNCTION__, strCompressionPara->dwBitRate);
                strCompressionPara->dwBitRate = 16*1024;
            } else if(strCompressionPara->dwBitRate > 1*1024*1024) {
                ipclog_info("%s main channel invalid bitRate:%lu\n",__FUNCTION__, strCompressionPara->dwBitRate);
                strCompressionPara->dwBitRate = 1*1024*1024;
            }
        }

        ven_info_t *ven_info = infotm_video_info_get(i);
        sVideoStream[i].priv = (void *)ven_info;
        sVideoStream[i].media_type = ven_info->media_type;
        if (ven_info->media_type == VIDEO_MEDIA_H264) {
            strCompressionPara->dwCompressionType  = QMAPI_PT_H264;    
        } else {
            strCompressionPara->dwCompressionType  = QMAPI_PT_H265;    
        }

        //limit fps 0~30
        if (strCompressionPara->dwFrameRate < 0) {
            ipclog_info("%s invalid frame rate:%lu\n",__FUNCTION__, strCompressionPara->dwFrameRate);
            strCompressionPara->dwFrameRate = 15;
        } else if(strCompressionPara->dwFrameRate>30) {
            ipclog_info("%s invalid frame rate:%lu\n",__FUNCTION__, strCompressionPara->dwFrameRate);
            strCompressionPara->dwFrameRate = 25;
        }
    }

    return 0;
}


int Q3_Codec_Deinit(void)
{
    infotm_video_deinit();
    Q3_Video_Stream_Deinit();

    return 0;
}    

int Q3_Video_Start(int stream)
{
    if (stream >= infotm_video_info_num_get()) {
        ipclog_error("%s invalid stream:%d \n", __FUNCTION__, stream);
        return -1;
    }

    infotm_video_preview_start(stream, (void *)&sVideoStream[stream], 
            Q3_Video_Stream_Handler);
    sVideoStream[stream].enable = 1;
    return 0;
}

int Q3_Video_Stop(int stream)
{
    if (stream >= infotm_video_info_num_get()) {
        ipclog_error("%s invalid stream:%d \n", __FUNCTION__, stream);
        return -1;
    }

    sVideoStream[stream].enable = 0;
    infotm_video_preview_stop(stream, (void *)&sVideoStream[stream]);
    return 0;
}

int Q3_AV_SetVideoResolution(int stream, int w, int h)
{
    if (stream >= infotm_video_info_num_get()) {
        ipclog_error("%s invalid stream:%d \n", __FUNCTION__, stream);
        return -1;
    }

    int ret = infotm_video_set_resolution(stream, w, h);

    if (ret < 0) {
        ipclog_warn("%s stream:%d %d*%d ret:%d \n", __FUNCTION__, stream, w, h, ret);
    } else {
        sVideoStream[stream].waitIFrame = 1;
    }

    return ret;
}

// 设置通道分辨率、码率、帧率 等
// need fixme yi.zhang
int Q3_Video_StreamPIC_Set(QMAPI_NET_CHANNEL_PIC_INFO *pInfo)
{
    //NOTE: just second stream can change resolution
    Q3_AV_SetVideoResolution(QMAPI_SECOND_STREAM, pInfo->stNetPara.wWidth, pInfo->stNetPara.wHeight);

    Q3_AV_SetVideoFrameRate(QMAPI_MAIN_STREAM, pInfo->stRecordPara.dwFrameRate);
    Q3_AV_SetVideoBitRate(QMAPI_MAIN_STREAM, pInfo->stRecordPara.dwBitRate, 
            pInfo->stRecordPara.dwRateType, pInfo->stRecordPara.dwMaxKeyInterval, 
            pInfo->stRecordPara.dwImageQuality);

    Q3_AV_SetVideoFrameRate(QMAPI_SECOND_STREAM, pInfo->stNetPara.dwFrameRate);
	Q3_AV_SetVideoBitRate(QMAPI_SECOND_STREAM, pInfo->stNetPara.dwBitRate, 
            pInfo->stNetPara.dwRateType, pInfo->stNetPara.dwMaxKeyInterval,
            pInfo->stNetPara.dwImageQuality);

        //NOTE: media_type cannot change by user now
    video_stream_t *vs = &sVideoStream[QMAPI_MAIN_STREAM];
    pInfo->stRecordPara.dwCompressionType = 
        (vs->media_type == VIDEO_MEDIA_H264)?QMAPI_PT_H264:QMAPI_PT_H265;
    pInfo->stNetPara.dwCompressionType    = 
        (vs->media_type == VIDEO_MEDIA_H264)?QMAPI_PT_H264:QMAPI_PT_H265;
    pInfo->stPhonePara.dwCompressionType  = 
        (vs->media_type == VIDEO_MEDIA_H264)?QMAPI_PT_H264:QMAPI_PT_H265;


    return 0;
}

void Q3_Video_ViewMode_Set(int viewmode)
{
    int ret = infotm_video_set_viewmode(QMAPI_MAIN_STREAM, viewmode);

    ipclog_info("%s viewmode:%d return %d\n", __FUNCTION__, viewmode, ret);
    if (ret < 0) {
        ipclog_warn("%s view mode:%d ret:%d \n", __FUNCTION__, viewmode, ret);
    } else {
        sVideoStream[QMAPI_MAIN_STREAM].waitIFrame = 1;
    }

}


int Q3_Encode_Intra_FrameEx(int nChannel,int nStream)
{
    ipclog_info("%s stream:%d \n", __FUNCTION__, nStream);
    char *pChannelName = infotm_video_get_name(nStream);
    if(!pChannelName)
    {
        ipclog_error("%s, Unsupport stream type:%d\n",__FUNCTION__, nStream);
        return -1;
    }
    //TODO: move down to video.c
    video_trigger_key_frame(pChannelName);

    return 0;
}

static int Q3_Stream_Write_VideoFrame(int nChannelNo, const VS_STREAM_TYPE_E enStreamType, char *Date, QMAPI_NET_FRAME_HEADER *pFrame)
{
	struct tm sttm={0};
	time_t timetick=0;
	VS_DATETIME_S stDateTime;
    VS_FRAME_TYPE_E enFrameType = VS_FRAME_I;
    
	if (NULL == pFrame || enStreamType >= VS_STREAM_TYPE_BUTT || nChannelNo >= QMAPI_MAX_CHANNUM || nChannelNo < 0)
	{
		ipclog_warn("pFrame %p , enStreamType  %d nChannelNo  %d \n", pFrame, enStreamType, nChannelNo);
		VS_BUF_ReleaseWriteMemPos(nChannelNo, enStreamType, 0, 0, 0, NULL);
		return 1;
	}
	
	if (pFrame->wAudioSize)
	{
		err();
		return 2;
	}

	if (pFrame->byFrameType == 0)
	{
		enFrameType = VS_FRAME_I;
	}
	else 
	{
		
		enFrameType = VS_FRAME_P;
	}
	
	timetick = pFrame->dwTimeTick;
	localtime_r(&timetick, &sttm);
	
	stDateTime.s32Year  = 	sttm.tm_year;
	stDateTime.s32Month = 	sttm.tm_mon;
	stDateTime.s32Day   = 	sttm.tm_mday;
	stDateTime.s32Hour  = 	sttm.tm_hour;
	stDateTime.s32Minute = 	sttm.tm_min;
	stDateTime.s32Second = 	sttm.tm_sec;
	stDateTime.enWeek   = 	sttm.tm_wday;
	VS_BUF_ReleaseWriteMemPos(nChannelNo, enStreamType, enFrameType, pFrame->dwVideoSize, pFrame->dwTimeTick, &stDateTime);

	return 0;
}

int Q3_Get_SPSPPSValue(int nChannel, int nStreamType, SPS_PPS_VALUE  *pstValue)
{
    //lock
    if (!sVideoStream[nStreamType].rtp_info) {
        ipclog_error("%s not inited already\n", __FUNCTION__);
        return -1;
    }
    memcpy(pstValue, sVideoStream[nStreamType].rtp_info, sizeof(SPS_PPS_VALUE));
	//unlock
    ipclog_info("%s, nChannel:%d. %d....%d:%d.\n",__FUNCTION__, nChannel, nStreamType, 
            pstValue->SPS_LEN, pstValue->SPS_PPS_BASE64_LEN);
    return 0;
}

static int Q3_Video_Stream_H264_Header_Parser(video_stream_t *vs, char *head, int s32HeadLen)
{
    int i = 0;
    SPS_PPS_VALUE *lpValue = (SPS_PPS_VALUE *)vs->rtp_info;
    if(s32HeadLen < 5 || head[0] != 0 || head[1] != 0 || head[2] != 0 || head[3] != 1)
    {
        ipclog_error("%s Invalid video header!!!!!!!!!!!!\n", __FUNCTION__);
        return -1;
    }

    lpValue->SPS_PPS_BASE64_LEN = 0;

    if(lpValue->SPS_PPS_BASE64_LEN == 0)
    {
        char strPPS[128] = {0};
        char strSPS[128] = {0};
        char *p = NULL;
        i=0;
        lpValue->SPS_LEN = 0;
        lpValue->PPS_LEN = 0;
        //char strVPS[128] = {0};
        int j;

        while(i<(s32HeadLen-4))
        {
            if(head[i] == 0 && head[i+1] == 0 && head[i+2] == 0 && head[i+3] == 1)
            {
                i += 4;
                p = &head[i];
                    
                if((head[i]&0x1f) == 0x7)
                {
                    for(j=0;j<(s32HeadLen-i);j++)
                    {
                        if(j < (s32HeadLen-i-4) && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 
                                && p[j+3] == 0x1)
                        {
                            break;
                        }

                        strSPS[j] = p[j];
                        lpValue->SPS_LEN++;
                    }
                    i += lpValue->SPS_LEN;
                }
                else if((head[i]&0x1f) == 0x8)
                {
                    for(j=0;j<(s32HeadLen-i);j++)
                    {
                        if(j < (s32HeadLen-i-4) && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 
                                && p[j+3] == 1)
                        {
                            break;
                        }

                        strPPS[j] = p[j];
                        lpValue->PPS_LEN++;
                    }
                    i += lpValue->PPS_LEN;
                }
            }
            else
            {
                i++;
            }

            if(lpValue->SPS_LEN>0 && lpValue->PPS_LEN>0)
            {
                break;
            }
        }

        for(i = 0; i< s32HeadLen; i++)
        {
            printf("0x%02X ", head[i]);
            if ((i & 0x1F) == 0x1F) {
                printf("\n");
            }
        }
        printf("\n");

        if(lpValue->SPS_PPS_BASE64_LEN == 0 && lpValue->SPS_LEN && lpValue->PPS_LEN)
        {
            int SPS_BASE64_Len = 0, PPS_BASE64_Len = 0;
            char* pTmp = lpValue->SPS_PPS_BASE64;
            memset(lpValue->SPS_PPS_BASE64, 0, sizeof(lpValue->SPS_PPS_BASE64));

            memcpy(lpValue->SPS_VALUE, strSPS, lpValue->SPS_LEN);
            memcpy(lpValue->PPS_VALUE, strPPS, lpValue->PPS_LEN);
            SPS_BASE64_Len = base64encode_ex((unsigned char*)lpValue->SPS_VALUE, lpValue->SPS_LEN, 
                    (unsigned char*)lpValue->SPS_PPS_BASE64, lpValue->SPS_PPS_BASE64_LEN);
            lpValue->SPS_PPS_BASE64[SPS_BASE64_Len] = ',';
            pTmp = lpValue->SPS_PPS_BASE64 + SPS_BASE64_Len +1;
            PPS_BASE64_Len = base64encode_ex((unsigned char*)lpValue->PPS_VALUE, lpValue->PPS_LEN, 
                    (unsigned char*)pTmp, sizeof(lpValue->SPS_PPS_BASE64)-SPS_BASE64_Len-1);
            lpValue->SPS_PPS_BASE64_LEN = SPS_BASE64_Len+1+PPS_BASE64_Len;

            lpValue->SEL_LEN = 10;

            ipclog_info("%s, stream:%d. %d-%d-%d\n", __FUNCTION__, 
                    vs->stream, SPS_BASE64_Len, PPS_BASE64_Len, lpValue->SPS_PPS_BASE64_LEN);
        }
    }

    return 0;

}

static int Q3_Video_Stream_H265_Header_Parser(video_stream_t *vs, char *head, int s32HeadLen)
{
    int i = 0;
    SPS_PPS_VALUE *lpValue = (SPS_PPS_VALUE *)vs->rtp_info;
    if(s32HeadLen < 5 || head[0] != 0 || head[1] != 0 || head[2] != 0 || head[3] != 1) {
        ipclog_error("%s , Invalid video header!!!!!!!!!!!!\n",__FUNCTION__);
        return -1;
    }

    lpValue->SPS_PPS_BASE64_LEN = 0;
    if(lpValue->SPS_PPS_BASE64_LEN == 0) {
        char strPPS[128] = {0};
        char strSPS[128] = {0};
        char *p = NULL;
        i=0;
        lpValue->SPS_LEN = 0;
        lpValue->PPS_LEN = 0;
        int VPS_LEN = 0;

        int j;

        while(i<(s32HeadLen-4))
        {
            if(head[i] == 0 && head[i+1] == 0 && head[i+2] == 0 && head[i+3] == 1)
            {
                i += 4;
                p = &head[i];

                if(((head[i]>>1) & 0x3F) == 32)     //VPS
                {
                    for(j=0;j<(s32HeadLen-i);j++)
                    {
                        if(j < (s32HeadLen-i-4) && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 
                                && p[j+3] == 0x1)
                            break;

                        //strVPS[j] = p[j];
                        VPS_LEN++;
                    }
                    i += VPS_LEN;
                }
                else if(((head[i]>>1) & 0x3F) == 33)     //SPS
                {
                    for(j=0;j<(s32HeadLen-i);j++)
                    {
                        if(j < (s32HeadLen-i-4) && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 
                                && p[j+3] == 0x1)
                            break;

                        strSPS[j] = p[j];
                        lpValue->SPS_LEN++;
                    }
                    i += lpValue->SPS_LEN;
                }
                else if(((head[i]>>1) & 0x3F) == 34)     //PPS
                {
                    for(j=0;j<(s32HeadLen-i);j++)
                    {
                        if(j < (s32HeadLen-i-4) && p[j] == 0 && p[j+1] == 0 && p[j+2] == 0 
                                && p[j+3] == 0x1)
                            break;

                        strPPS[j] = p[j];
                        lpValue->PPS_LEN++;
                    }
                    i += lpValue->PPS_LEN;
                }
            }
            else
            {
                i++;
            }

            if(lpValue->SPS_LEN>0 && lpValue->PPS_LEN>0)
            {
                break;
            }
        }

        for(i = 0; i< s32HeadLen; i++)
        {
            printf("0x%02X ", head[i]);
            if ((i & 0x1F) == 0x1F) {
                printf("\n");
            }
        }
        printf("\n");

        if(lpValue->SPS_PPS_BASE64_LEN == 0 && lpValue->SPS_LEN && lpValue->PPS_LEN)
        {
            int SPS_BASE64_Len = 0, PPS_BASE64_Len = 0;
            char* pTmp = lpValue->SPS_PPS_BASE64;
            memset(lpValue->SPS_PPS_BASE64, 0, sizeof(lpValue->SPS_PPS_BASE64));

            memcpy(lpValue->SPS_VALUE, strSPS, lpValue->SPS_LEN);
            memcpy(lpValue->PPS_VALUE, strPPS, lpValue->PPS_LEN);
            SPS_BASE64_Len = base64encode_ex((unsigned char*)lpValue->SPS_VALUE, lpValue->SPS_LEN, 
                    (unsigned char*)lpValue->SPS_PPS_BASE64, lpValue->SPS_PPS_BASE64_LEN);
            lpValue->SPS_PPS_BASE64[SPS_BASE64_Len] = ',';
            pTmp = lpValue->SPS_PPS_BASE64 + SPS_BASE64_Len +1;
            PPS_BASE64_Len = base64encode_ex((unsigned char*)lpValue->PPS_VALUE, lpValue->PPS_LEN, 
                    (unsigned char*)pTmp, sizeof(lpValue->SPS_PPS_BASE64)-SPS_BASE64_Len-1);
            lpValue->SPS_PPS_BASE64_LEN = SPS_BASE64_Len+1+PPS_BASE64_Len;

            lpValue->SEL_LEN = 10;

            ipclog_info("%s , stream:%d. %d-%d-%d\n", __FUNCTION__, vs->stream, 
                    SPS_BASE64_Len, PPS_BASE64_Len, lpValue->SPS_PPS_BASE64_LEN);
        }
    }

    return 0;
}

int Q3_Video_Stream_Header_Parse(video_stream_t *vs, char *header, int len)
{ 
    if (VIDEO_MEDIA_H264 == vs->media_type) {
        Q3_Video_Stream_H264_Header_Parser(vs, header, len);
    } else {
        Q3_Video_Stream_H265_Header_Parser(vs, header, len);
    }

    return 0;
}

void Q3_Video_Frame_Post_l(video_stream_t *vs, media_vframe_t const *vfr)
{
    int s32Ret = 0;
    int realFramelen = 0;
    unsigned char *pbuff = NULL;
    QMAPI_NET_FRAME_HEADER stFrameHeader;
    DWORD s32Userdate = 0;

    //ipclog_debug("%s client_index:%d \n", __FUNCTION__, vs->client_index);
    if (!vs->enable) {
        ipclog_warn("%s stream:%d not enable already\n", __FUNCTION__, vs->stream);
        return;
    }

    if ((vs->waitIFrame == 1)) {
        if (!(vfr->flag & MEDIA_VFRAME_FLAG_KEY)) {
            ipclog_debug("video stream:%d wait for first I frame\n", vs->stream);
            return;
        } else {
            vs->waitIFrame = 0;
            Q3_Video_Stream_Header_Parse(vs, vfr->header, vfr->hdrlen);
        }
    }

    stFrameHeader.dwSize       = sizeof(QMAPI_NET_FRAME_HEADER);
    //stFrameHeader.dwFrameIndex = vfr->frcnt;
    stFrameHeader.dwVideoSize  = vfr->size;
    stFrameHeader.dwTimeTick   = (DWORD)vfr->ts;
    stFrameHeader.wAudioSize   = 0;
    stFrameHeader.byFrameType  = (vfr->flag & MEDIA_VFRAME_FLAG_KEY)?0:1;

    /*!< post frame to avbuffer */
    if (stFrameHeader.byFrameType == 0) {
        realFramelen = vfr->hdrlen + vfr->size;
    } else {
        realFramelen = vfr->size;
    }
    stFrameHeader.dwVideoSize = realFramelen;

    if (realFramelen >= u32BlockSize[vs->stream])
    {
        ipclog_error("@@@ %s. frame:%d. BlockSize:%d. type:%d. \n",
                __FUNCTION__, realFramelen, u32BlockSize[vs->stream], vs->stream);
        vs->waitIFrame = 1;
        return;
    }

    pbuff = VS_BUF_GetWriteMemPos(vs->channel, vs->stream, realFramelen);
    if (!pbuff) {
        ipclog_error("VS_BUF_GetWriteMemPos:0x%x\n",s32Ret);
        return;
    }

    if ((stFrameHeader.byFrameType==0) && vfr->hdrlen) {
        memcpy(pbuff, vfr->header, vfr->hdrlen);
        memcpy(pbuff + (vfr->hdrlen), vfr->buf, vfr->size);            
    } else {
        memcpy(pbuff, vfr->buf, vfr->size);
    }

    if (stFrameHeader.byFrameType == 0) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        vs->s64Time = tv.tv_sec;
        vs->s64Time *= 1000;
        vs->s64Time += tv.tv_usec/1000;
        vs->s32Time = stFrameHeader.dwTimeTick;
    }
    //duration between two frame, reset s32Time when i frame is comming? for what? 
    vs->s64Time += stFrameHeader.dwTimeTick - vs->s32Time;    
    vs->s32Time = stFrameHeader.dwTimeTick;
    
    stFrameHeader.byVideoCode = (vs->media_type == VIDEO_MEDIA_H264)?QMAPI_PT_H264:QMAPI_PT_H265;
    stFrameHeader.byAudioCode = 0;
    stFrameHeader.byReserved1 = 0;// 按4位对齐
    stFrameHeader.byReserved2 = 0; // 按4位对齐
    stFrameHeader.byReserved3 = 0; // 按4位对齐

    ven_info_t *info = (ven_info_t*)(vs->priv);
    stFrameHeader.wVideoWidth  = info->width;
    stFrameHeader.wVideoHeight = info->height;

    Q3_Stream_Write_VideoFrame(vs->channel, vs->stream, (char *)pbuff, &stFrameHeader);

    stFrameHeader.dwTimeTick = vs->s64Time & 0xffffffff;
    s32Userdate = (vs->s64Time >> 32) & 0xffffffff;
    if (m_pfQ3MainVideoCallBk) {
        m_pfQ3MainVideoCallBk(vs->channel, vs->stream, (char*)(pbuff), realFramelen, stFrameHeader, s32Userdate);
    }

    return;
}

static void Q3_Video_Stream_Handler(void const *client, media_vframe_t const *vfr)
{
    video_stream_t *vs = (video_stream_t *)client;
    //pthread_mutex_lock(vs->mutex);
    Q3_Video_Frame_Post_l(vs, vfr); 
    //pthread_mutex_unlock(vs->mutex);
}

int Q3_Video_SnapShot(int nChannel, char **lpBuf, int *lpBuflen)
{
    struct fr_buf_info stBuf;
    int ret = 0;
    video_get_snap(JPEG_CHANNEL_NAME, &stBuf);

    if (*lpBuflen<stBuf.size)
    {
        ipclog_error("%s error: buffer allocated not enough\n", __FUNCTION__);
        ret = QMAPI_ERR_DATA_TOO_BIG;
    }
    else
    {
        memcpy(*lpBuf, stBuf.virt_addr, stBuf.size);
        *lpBuflen = stBuf.size;
    }

    video_put_snap(JPEG_CHANNEL_NAME, &stBuf);
    return ret;
}

//长和宽各调用一次
BOOL IsSizeOk(int nMainLen,int nSubLen)
{
   int nSub = nMainLen - nSubLen;
   int nDuSub = nMainLen / 2 - nSubLen;
   int nQuSub = nMainLen /4 - nSubLen;

   if( nSub >= 0 && nSub <= 16 )
   {
      return TRUE;
   }

   if( nDuSub >= 0 && nDuSub <= 16 )
   {
      return TRUE;
   }

   if( nQuSub >= 0 && nQuSub <= 16 )
   {
      return TRUE;
   }     
   return FALSE;
}

//根据全局制式和显示格式计算出主次码流画面尺寸判断长宽是否兼容

BOOL IsFormatComp(int nMainFormat,int nSubFormat)
{
    int nRecWidth = Get_Q3_Vi_Width(QMAPI_PAL, nMainFormat);
    int nRecHeight = Get_Q3_Vi_Height(QMAPI_PAL, nMainFormat);

    int nNetWidth =  Get_Q3_Vi_Width(QMAPI_PAL, nSubFormat);
    int nNetHeight =  Get_Q3_Vi_Height(QMAPI_PAL, nSubFormat);

    if(IsSizeOk(nRecWidth,nNetWidth) && IsSizeOk(nRecHeight, nNetHeight))
    {
       return TRUE;
    }

    return FALSE;
}


int Q3_SetVideoStreamCallBack(int nStreamType, CBVideoProc  pfun)
{
    switch(nStreamType)
    {
        case QMAPI_MAIN_STREAM:
        {
            m_pfQ3MainVideoCallBk = pfun;         
        }
        break;
        case QMAPI_SECOND_STREAM:
        {
            m_pfQ3SecondVideoCallBk = pfun;
        }
        break;      
    }
    return 0;
}

static int Q3_AV_SetVideoFrameRate(int nStreamType, int nFrameRate)
{	
	int nRet = -1;
    if (nStreamType >= infotm_video_info_num_get()) {
        ipclog_error("%s invalid stream:%d \n", __FUNCTION__, nStreamType);
        return -1;
    }

    nRet = infotm_video_set_fps(nStreamType, nFrameRate);
    ipclog_debug("%s: nStreamType:%d, nFrameRate=%d nRet:%d !\n", __FUNCTION__, nStreamType, nFrameRate, nRet);
    return nRet;
}

static int Q3_AV_SetVideoBitRate(int nStreamType, int nBitRate, int rcMode, int pFrame, int imageQuality)
{	
	int nRet = -1;
    if (nStreamType >= infotm_video_info_num_get()) {
        ipclog_error("%s invalid stream:%d \n", __FUNCTION__, nStreamType);
        return -1;
    }

    if(nBitRate > 8*1024*1024)
        nBitRate = 8*1024*1024;
	else if(nBitRate<16*1024)
		nBitRate = 16*1024;

    nRet = infotm_video_set_bitrate(nStreamType, nBitRate, rcMode, pFrame, imageQuality);

    ipclog_debug("%s: nStreamType:%d, nBitRate=%d rcMode:%d pFrame:%d imageQuality:%d nRet:%d !\n", __FUNCTION__, 
            nStreamType, nBitRate, rcMode, pFrame, imageQuality, nRet);
	
    return nRet;
}


int Get_Q3_ColorSupport(QMAPI_NET_COLOR_SUPPORT  *lpColorSupport)
{
	memset(lpColorSupport, 0x00, sizeof(QMAPI_NET_COLOR_SUPPORT));
	lpColorSupport->dwSize = sizeof(QMAPI_NET_COLOR_SUPPORT);
	lpColorSupport->strBrightness.nMin = 1;		    
	lpColorSupport->strHue.nMin = 1;			
	lpColorSupport->strSaturation.nMin = 1;
	lpColorSupport->strContrast.nMin = 1;				
	lpColorSupport->strDefinition.nMin = 1;

	lpColorSupport->strBrightness.nMax = 255;		    
	lpColorSupport->strHue.nMax = 255;			
	lpColorSupport->strSaturation.nMax = 255;
	lpColorSupport->strContrast.nMax = 255;				
	lpColorSupport->strDefinition.nMax = 255;

	lpColorSupport->dwMask = QMAPI_COLOR_SET_BRIGHTNESS | QMAPI_COLOR_SET_HUE | QMAPI_COLOR_SET_SATURATION | 
        QMAPI_COLOR_SET_CONTRAST | QMAPI_COLOR_SET_DEFINITION | QMAPI_COLOR_SET_ROTATE;
	return 0;
}

int Get_Q3_SC1135_DefaultColor(QMAPI_NET_CHANNEL_COLOR  *pColor)
{
    if(!pColor || pColor->dwChannel != 0)
        return -1;

    pColor->dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
    camera_get_brightness("isp", &pColor->nBrightness);
    camera_get_contrast("isp", &pColor->nContrast);
    camera_get_hue("isp", &pColor->nHue);
    camera_get_saturation("isp", &pColor->nSaturation);
    camera_get_sharpness("isp", &pColor->nDefinition);

    return 0;
}

int Get_Q3_SC2135_DefaultColor(QMAPI_NET_CHANNEL_COLOR  *pColor)
{
    if(!pColor || pColor->dwChannel != 0)
        return -1;

    pColor->dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
    camera_get_brightness("isp", &pColor->nBrightness);
    camera_get_contrast("isp", &pColor->nContrast);
    camera_get_hue("isp", &pColor->nHue);
    camera_get_saturation("isp", &pColor->nSaturation);
    camera_get_sharpness("isp", &pColor->nDefinition);

    return 0;
}


