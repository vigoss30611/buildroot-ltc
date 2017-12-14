#ifndef __Q3AV_H__
#define __Q3AV_H__

#include "QMAPICommon.h"
#include "QMAPIType.h"
#include "QMAPINetSdk.h"
#include "QMAPIDataType.h"
#include "QMAPIErrno.h"
#include "QMAPI.h"

#include "h264nalu.h"

#include <pthread.h>

#include <qsdk/videobox.h>


/* CODEC ID */
typedef enum 
{
    MEDIA_CODEC_UNKNOWN			= 0x00,
    MEDIA_CODEC_VIDEO_MPEG4		= 0x4C,
    MEDIA_CODEC_VIDEO_H263		= 0x4D,
    MEDIA_CODEC_VIDEO_H264		= 0x4E,
    MEDIA_CODEC_VIDEO_MJPEG		= 0x4F,
    MEDIA_CODEC_VIDEO_H265		= 0x50,

    MEDIA_CODEC_AUDIO_G711U     = 0x89,
    MEDIA_CODEC_AUDIO_G711A     = 0x8A,
    MEDIA_CODEC_AUDIO_ADPCM     = 0x8B,
    MEDIA_CODEC_AUDIO_PCM		= 0x8C,
    MEDIA_CODEC_AUDIO_SPEEX		= 0x8D,
    MEDIA_CODEC_AUDIO_MP3		= 0x8E,
    MEDIA_CODEC_AUDIO_G726      = 0x8F,
}ENUM_CODECID;


int Q3_Codec_Init(QMAPI_NET_CHANNEL_PIC_INFO *pstChannelInfo, int Resolution, int ManBlockSize, int SubBlockSize);
int Q3_Codec_Deinit(void);

int Q3_Video_Start(int stream);
int Q3_Video_Stop(int stream);

int Q3_Get_SPSPPSValue(int nChannel, int nStreamType, SPS_PPS_VALUE  *pstValue);

/*
	
*/
int Q3_Video_StreamPIC_Set(QMAPI_NET_CHANNEL_PIC_INFO *pstChannelInfo);

void Q3_Video_ViewMode_Set(int viewmode);

int Q3_Encode_Intra_FrameEx(int nChannel,int nStream);

int Get_Q3_SC1135_DefaultColor(QMAPI_NET_CHANNEL_COLOR  *pColor);
int Get_Q3_SC2135_DefaultColor(QMAPI_NET_CHANNEL_COLOR  *pColor);

int Get_Q3_ColorSupport(QMAPI_NET_COLOR_SUPPORT  *lpColorSupport);


int Get_Q3_Vi_Height(DWORD dwTvMode, DWORD dwVFormat);
int Get_Q3_Vi_Width(DWORD dwTvMode, DWORD dwVFormat);

BOOL IsSizeOk(int nMainLen,int nSubLen);
BOOL IsFormatComp(int nMainFormat,int nSubFormat);

//int SetVadcVideoFrameRate(int nFrameRate);


int Q3_SetVideoStreamCallBack(int nStreamType, CBVideoProc  pfun);


int Q3_Video_SnapShot(int nChannel, char **lpBuf, int *lpBuflen);


#endif

