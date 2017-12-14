
#ifndef __Q3AUDIO_H__
#define __Q3AUDIO_H__

#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPIDataType.h"
#include "QMAPIErrno.h"
#include "QMAPI.h"

#define AUDIO_BUF_SIZE			1024

typedef struct{
	int wFormatTag; /*输入音频格式 QMAPI_PT_G726 等*/
	int channel;    /*通道数*/
	int bitwidth;   /*位宽*/
	int sample_rate; /**/
	int volume;    /*音量 默认 256*/
	int res[4];
}Q3_Audio_Info;

/*
	音频解码初始化,成功返回非0的handle，失败返回0
		Info [in]:
*/
int QMAPI_Audio_Decoder_Init(Q3_Audio_Info *Info);
int QMAPI_Audio_Decoder_UnInit(int handle);
/*
	音频数据解码，成功返回TRUE，失败返回FALSE，失败需要Q3_Audio_Decoder_UnInit
		handle   [in]:
		data     [in]:
		dataSize [in]:
*/
int QMAPI_Audio_Decoder_Data(int handle, char *data, int dataSize);

// 设置音频初始化参数
int QMAPI_Audio_Init(void *param);
int QMAPI_Audio_UnInit(int handle);

int QMAPI_Audio_Start(int handle);
int QMAPI_Audio_Stop(int handle);

/*
	设置音频数据回调函数
		pfun    [in]:
*/
int QMAPI_AudioStreamCallBackSet(CBAudioProc pfun);
#endif
