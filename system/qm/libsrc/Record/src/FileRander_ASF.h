// FileRander_ASF.h: interface for the CFileRander_ASF class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _FILE_RANDER_ASF_H
#define _FILE_RANDER_ASF_H

//#include "JB_common.h"
#include "QMAPIType.h"
#include "QMAPINetSdk.h"
#include "JBFileFormat.h"

/*#include "jb_xtream_producer_config.h"
#include "jb_xtream_producer_keytypes.h"
*/
#define TRACE printf
#define TRACE0(fmt, args...) do {} while(0)

#define MAX_STREAM_BUFF_SIZE 512*1024
#define VIDEO_XVID  0x58564944
#define VIDEO_H264  0x34363248
#define VIDEO_MJPEG 0x47504A4D
#define VIDEO_H265	0x35363248

#define	ENCODE_FORMAT_MGPE4				0x0
//#define	ENCODE_FORMAT_H264				0x2
#define	ENCODE_FORMAT_H264				0x4
#define  ENCODE_FORMAT_MJPEG				0x5
#define  ENCODE_FORMAT_H265				265

typedef struct tagJB_STREAM_BUFFER
{
	DWORD dwSize;
	DWORD dwPosition;
	DWORD dwTimeTick;
	DWORD FrameType;
	BYTE  pBuffer[MAX_STREAM_BUFF_SIZE];
}JB_STREAM_BUFFER,*PJB_STREAM_BUFFER;
typedef struct tagFilePlay_INFO
{
	DWORD   dwSize;
	DWORD   dwStream1Height;			//视频高(1)
	DWORD   dwStream1Width;			//视频宽
	DWORD   dwStream1CodecID;			//视频编码类型号（MPEG4为0，JPEG2000为1,H264为2）
	DWORD   dwAudioChannels;			//音频通道数
	DWORD   dwAudioBits;				//音频比特率
	DWORD   dwAudioSamples;			//音频采样率
	DWORD   dwWaveFormatTag;			//音频编码类型号
	char	csChannelName[MAX_CHANNEL_NAME_LEN];			//通道名称
	DWORD	dwFileSize;
	DWORD	dwTotalTime;			/*文件的总时间长度*/
	DWORD	dwPlayStamp;			/*时间戳*/
	DWORD  dwFrameRate;
} FilePlay_INFO, *PFilePlay_INFO;

typedef struct tagPLAYFILE_INFO
{
    HANDLE hFileHandle;	//FILE_RANDER_ASF
    QMAPI_TIME BeginTime;//文件起始时间
    QMAPI_TIME EndTime;//文件结束时间
    QMAPI_TIME CurTime;
    QMAPI_TIME StartTime;//搜索开始时间
    QMAPI_TIME StopTime;//搜索结束时间
    int index;
    unsigned int firstReads;//0:未开始读，1:已经开始
    unsigned long lastTimeTick;//上一个I帧的时间戳
    unsigned long firstStamp;//第一个I帧的时间戳
    unsigned long dwTotalTime;//文件总时间,播放时能获取到
    unsigned long filesize;
    char reserve[12];
    int width;
    int height;
    int Mode; //0:正常播放，1:I帧回放
    int IFrameIndex;
    int VideoFormat;
    //int AudioFormat;
    char sFileName[128];
    int framerate;
    WORD bHaveAudio;
	WORD formatTag;
    JB_STREAM_BUFFER *pStreamBuffer;
}PLAYFILE_INFO;

#define MY_FRAME_TYPE_A        0x0d
#define MY_FRAME_TYPE_I        0x0e
#define MY_FRAME_TYPE_P        0x0b

#define WAVE_FORMAT_CG729		0x7101
#define WAVE_FORMAT_MP3			0x7102
#define WAVE_FORMAT_TM722		0x0065
#define WAVE_FORMAT_CT729		0x8102
#define WAVE_FORMAT_JBG711		0x9101
#define WAVE_FORMAT_JBADPCM		0x9102

#endif //_FILE_RANDER_ASF_H
