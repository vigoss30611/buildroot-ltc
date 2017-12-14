/*
*********************************************************************************************************
*
*                       Copyright (c) 2012 Hangzhou QianMing Technology Co.,Ltd
*
*********************************************************************************************************
*/
#ifndef __PLAY_SDK_H__
#define __PLAY_SDK_H__

#ifdef __cplusplus
extern "C" {
#endif

#define PB_OK		0
#define PB_ERR		(-1)

#define PLAY_CONTROL_START		(1 << 0) /* 播放开始 */
#define PLAY_CONTROL_PAUSE		(1 << 1) /* 播放暂停 */
#define PLAY_CONTROL_STOP		(1 << 2) /* 播放停止 */
#define PLAY_CONTROL_SLOW		(1 << 3) /* 慢速播放 */
#define PLAY_CONTROL_FAST		(1 << 4) /* 快速播放 */
#define PLAY_CONTROL_STEP		(1 << 5) /* 单帧播放 */
#define PLAY_CONTROL_SOUND		(1 << 6) /* 声音播放 */
#define PLAY_CONTROL_REVERSE	(1 << 7) /* 关键帧倒播 */
#define PLAY_CONTROL_ONEFRAME	(1 << 8) /* 仅播放一帧 */
#define PLAY_CONTROL_KEYFRAME	(1 << 9) /* 仅播关键帧 */

#define DISP_TYPE_YUV420		(1 << 0) /* YUV420图像 */
#define DISP_TYPE_BMP24			(1 << 1) /* BMP24位位图 */
#define DISP_TYPE_JPEG			(1 << 2) /* JPEG图像 */

typedef struct {
	int nEncProfile; /* 编码算法 */
	LONG lPicWidth; /* 视频宽度 */
	LONG lPicHeight; /* 视频高度 */
} video_format_t;

typedef struct {
	int nEncProfile; /* 编码算法 */
	WORD uChannels; /* 声道数 */
	WORD uDataBits; /* 编码数据位数 */
	DWORD dwDataBitrate; /* 编码数据码率 */
	DWORD dwSampleFreq; /* 采样频率 */
} audio_format_t;

typedef struct {
	int nDispType; /* 图像类型(DISP_TYPE_YUV420等) */
	WORD uPicWidth; /* 图像宽度 */
	WORD uPicHeight; /* 图像高度 */
	DWORD dwTimestamp; /* 图像时间戳 */
	union {
		struct {
			BYTE *pY; /* Y分量 */
			BYTE *pU; /* U分量 */
			BYTE *pV; /* V分量 */		
			WORD uYStride; /* Y分量跨度 */
			WORD uUVStride; /* UV分量跨度 */
		} yuv;
		struct {
			BYTE *pBMP; /* RGB分量 */
			DWORD uBMPSize; /* RGB分量跨度 */
		} bmp;
		struct {
			BYTE *pJPEG; /* JPEG数据缓冲 */
			DWORD uJPEGSize; /* JPEG数据大小 */
		} jpeg;
	};
} disp_info_t;

typedef void (CALLBACK *pGetDataFunc)(void *pPlay, UINT *pLastTimestamp, void *pUser); /* 获取数据回调函数 */
typedef void (CALLBACK *pCaptureDataFunc)(int nType, void *pData, int nLen, void *pUser); /* 捕获数据回调函数 */

typedef void (CALLBACK *pDrawFunc)(void *pPlay, HDC hDC, void *pUser); /* 用户绘制回调函数 */
typedef void (CALLBACK *pDisplayFunc)(void *pPlay, disp_info_t *pInfo, void *pUser); /* 解码图像回调函数 */
typedef void (CALLBACK *pPlayEndFunc)(void *pPlay, void *pUser); /* 文件播放结束回调函数 */

int __stdcall play_init(void);
void __stdcall play_exit(void);
void* __stdcall play_open_file(HWND hWnd, const char *sFileName);
void* __stdcall play_open_stream(HWND hWnd, int bLivePlay, pGetDataFunc pFunc, void *pUser);
void* __stdcall play_create(HWND hWnd, video_format_t *pvFormat, audio_format_t *paFormat, int bLivePlay);
int __stdcall play_destroy(void *pPlay);
int __stdcall capture_audio_stream(pCaptureDataFunc pFunc, audio_format_t *paFormat, void *pUser);
int __stdcall play_get_error(void *pPlay);
int __stdcall play_config_audio(void *pPlay, audio_format_t *paFormat);
int __stdcall play_control(void *pPlay, int nControl, int nParam);
int __stdcall play_refresh(void *pPlay);
int __stdcall play_input_data(void *pPlay, DWORD dwType, DWORD dwTimestamp, const BYTE *pData, DWORD dwLen);
int __stdcall play_clear_data(void *pPlay);
int __stdcall play_draw_func(void *pPlay, pDrawFunc pFunc, void *pUser);
int __stdcall play_display_func(void *pPlay, int nType, ULONG uJpegQuality, pDisplayFunc pFunc, void *pUser);
int __stdcall play_capture_bmp(void *pPlay, const char *sFileName);
int __stdcall play_capture_bmp_buffer(void *pPlay, void *pBmpBuffer, DWORD dwBufferSize, DWORD *pSizeReturned/*[OUT]*/);
int __stdcall play_capture_jpeg(void *pPlay, ULONG uJpegQuality, const char *sFileName);
int __stdcall play_capture_jpeg_buffer(void *pPlay, ULONG uJpegQuality, void *pJpegBuffer, DWORD dwBufferSize, DWORD *pSizeReturned/*[OUT]*/);
int __stdcall play_pic_enhance(void *pPlay, DWORD dwVaule);
int __stdcall play_set_rect(void *pPlay, HWND hWnd, RECT *pRect);
int __stdcall play_get_pic_size(void *pPlay, LONG *pWidth/*[OUT]*/, LONG *pHeight/*[OUT]*/);
int __stdcall play_get_file_info(void *pPlay, LONG *pTotalFrames/*[OUT]*/, LONG *pTotalTime/*[OUT]*/);
int __stdcall play_get_position(void *pPlay, DWORD *pCurrentFrame/*[OUT]*/, DWORD *pCurrentTime/*[OUT]*/);
int __stdcall play_set_position(void *pPlay, DWORD *pCurrentFrame,/*Choose one(OR)*/DWORD *pCurrentTime);
int __stdcall play_end_msg(void *pPlay, HWND hWnd, UINT uMsg, int nUser);
int __stdcall play_end_callback(void *pPlay, pPlayEndFunc pFunc, void *pUser);
int __stdcall play_get_volume(void *pPlay, LONG *pVolume/*[OUT]*/);
int __stdcall play_set_volume(void *pPlay, LONG lVolume);
int __stdcall play_get_balance(void *pPlay, LONG *pBalance/*[OUT]*/);
int __stdcall play_set_balance(void *pPlay, LONG lBalance);
int __stdcall play_get_vision(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __PLAY_SDK_H__ */
