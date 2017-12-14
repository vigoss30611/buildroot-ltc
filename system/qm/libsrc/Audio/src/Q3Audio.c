
#include <sys/prctl.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <qsdk/codecs.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>

#include "Q3Audio.h"

#include "MediaBuffer.h"
#include "MediaBufferErrno.h"

#include "audio.h"

/*
ptNumPerFrm:每次采集音频数据字节长度
wFormatTag:QMAPI_PAYLOAD_TYPE_E枚举
*/

typedef struct{
	void *adec_codec;
	audio_fmt_t play_fmt;
	int handle;
	pthread_mutex_t codec_mutex;
    char *dec_buf;
    int dec_buf_size;
	int res[3];
}Q3_Audio_Decoder;

int g_audioPause = 0;
int g_audioPaused = 1;
int g_audioRun = 0;
pthread_t g_audiothread = 0;
CBAudioProc         m_pfQ3AudioCallBk = NULL;

int QMAPI_Audio_Decoder_Init(Q3_Audio_Info *Info)
{
    return infotm_audio_out_init();
}

int QMAPI_Audio_Decoder_UnInit(int handle)
{
    infotm_audio_out_deinit(handle);
}

int QMAPI_Audio_Decoder_Data(int handle, char *data, int dataSize)
{
    infotm_audio_out_send(handle, data, dataSize);
}    


#if 0
int QMAPI_Audio_Decoder_Init(Q3_Audio_Info *Info)
{
	int ret = 0;
	Q3_Audio_Decoder *Audio_Decoder;
	struct codec_info codec_info;

	printf("Q3_Audio_Decoder_Init\n");
	Audio_Decoder = malloc(sizeof(Q3_Audio_Decoder));
	if (!Audio_Decoder) {
		printf("Q3_Audio_Decoder_Init malloc error\n");
		return 0;
    }

    
    memset(Audio_Decoder, 0, sizeof(Q3_Audio_Decoder));
    Audio_Decoder->handle      = -1;
    pthread_mutex_init(&Audio_Decoder->codec_mutex, NULL);
    
    Audio_Decoder->dec_buf_size = DECODE_BUFFER_SIZE;
    Audio_Decoder->dec_buf = malloc(Audio_Decoder->dec_buf_size);
    if (!Audio_Decoder->dec_buf) {
        printf("Q3_Audio_Decoder_Init malloc dec_buf error\n");
        free(Audio_Decoder);
        return 0;
    }

    if (Info->wFormatTag == QMAPI_PT_G726)
        codec_info.in.codec_type = AUDIO_CODEC_G726;
	else if (Info->wFormatTag == QMAPI_PT_ADPCMA)
		codec_info.in.codec_type = AUDIO_CODEC_ADPCM;
	else if (Info->wFormatTag == QMAPI_PT_MP3)
		codec_info.in.codec_type = AUDIO_CODEC_MP3;
	else if (Info->wFormatTag == QMAPI_PT_G711A)
		codec_info.in.codec_type = AUDIO_CODEC_G711A;
	else if (Info->wFormatTag == QMAPI_PT_G711U)
		codec_info.in.codec_type = AUDIO_CODEC_G711U;
	else {
		printf("audio recv not support encode type %x \n",  Info->wFormatTag);
		return 0;
	}

	codec_info.in.channel     = Info->channel;
	codec_info.in.bitwidth    = Info->bitwidth;
	codec_info.in.sample_rate = Info->sample_rate;

	codec_info.in.bit_rate = codec_info.in.channel * codec_info.in.sample_rate \
												* GET_BITWIDTH(codec_info.in.bitwidth);

	codec_info.out.codec_type = AUDIO_CODEC_PCM;
	codec_info.out.channel = 2;
	codec_info.out.sample_rate = 16000;
	codec_info.out.bitwidth = 32;
	codec_info.out.bit_rate = codec_info.out.channel * codec_info.out.sample_rate \
											* GET_BITWIDTH(codec_info.out.bitwidth);
    pthread_mutex_lock(&Audio_Decoder->codec_mutex);
	Audio_Decoder->adec_codec = codec_open(&codec_info);
	if (!Audio_Decoder->adec_codec) {
		printf("audio recv open codec decoder failed %x \n",  Info->wFormatTag);
        pthread_mutex_unlock(&Audio_Decoder->codec_mutex);
		free(Audio_Decoder);
		return 0;
	}
    pthread_mutex_unlock(&Audio_Decoder->codec_mutex);
	
	Audio_Decoder->play_fmt.channels = 2;
	Audio_Decoder->play_fmt.bitwidth = 32;
	Audio_Decoder->play_fmt.sample_size = 1024;
	Audio_Decoder->play_fmt.samplingrate = 16000;

	Audio_Decoder->handle = audio_get_channel("default", &Audio_Decoder->play_fmt, CHANNEL_BACKGROUND);
	if (Audio_Decoder->handle < 0) {
		printf("audio recv get play channel failed %d \n", Audio_Decoder->handle);
		codec_close(Audio_Decoder->adec_codec);
		free(Audio_Decoder);
		return 0;
	}

	ret = audio_set_volume(Audio_Decoder->handle, Info->volume);
	if(ret < 0){
		printf("audio set volume failed ret:%d\n", ret);
	}

    return (int)Audio_Decoder;
}


int QMAPI_Audio_Decoder_UnInit(int handle)
{
	Q3_Audio_Decoder *Audio_Decoder = (Q3_Audio_Decoder *)handle;

	if (Audio_Decoder)
	{
	    pthread_mutex_lock(&Audio_Decoder->codec_mutex);
	    if(Audio_Decoder->adec_codec)
	    {
	    	codec_close(Audio_Decoder->adec_codec);
	    	Audio_Decoder->adec_codec = NULL;
	    }
	    pthread_mutex_unlock(&Audio_Decoder->codec_mutex);
		if (Audio_Decoder->handle >= 0)
		{
			audio_put_channel(Audio_Decoder->handle);
			Audio_Decoder->handle = -1;
		}
		
        if (Audio_Decoder->dec_buf) {
            free(Audio_Decoder->dec_buf);
            Audio_Decoder->dec_buf = NULL;
        }

        pthread_mutex_destroy(&Audio_Decoder->codec_mutex);
        
        free(Audio_Decoder);
	}

    return 0;
}

int QMAPI_Audio_Decoder_Data(int handle, char *data, int dataSize)
{
	int ret = 0, length, decsize = 0;
	struct codec_addr codec_addr;
	Q3_Audio_Decoder *Audio_Decoder = (Q3_Audio_Decoder *)handle;

	if (!Audio_Decoder || Audio_Decoder->adec_codec==NULL || Audio_Decoder->handle<0)
	{
		printf("Q3_Audio_Decoder_Data handle error\n");
		return FALSE;
	}
    pthread_mutex_lock(&Audio_Decoder->codec_mutex);

	codec_addr.in = data;
	codec_addr.len_in = dataSize;
	codec_addr.out = Audio_Decoder->dec_buf;
	codec_addr.len_out = Audio_Decoder->dec_buf_size;
    
	decsize = codec_decode(Audio_Decoder->adec_codec, &codec_addr);
	if (decsize < 0) {
		printf("audio recv codec decoder failed %d len_in:%d len_out:%d\n", decsize, codec_addr.len_in, codec_addr.len_out);
		do {
			ret = codec_flush(Audio_Decoder->adec_codec, &codec_addr, &length);
			if (ret == CODEC_FLUSH_ERR)
				break;

			/* TODO you need least data or not ?*/
		} while (ret == CODEC_FLUSH_AGAIN);
        pthread_mutex_unlock(&Audio_Decoder->codec_mutex);
		return FALSE;
	}
    pthread_mutex_unlock(&Audio_Decoder->codec_mutex);

	ret = audio_write_frame(Audio_Decoder->handle, Audio_Decoder->dec_buf, decsize);
	if (ret < 0){
		printf("audio recv codec play failed %d\n",  ret);
		audio_put_channel(Audio_Decoder->handle);
		Audio_Decoder->handle = audio_get_channel("default", &Audio_Decoder->play_fmt, CHANNEL_BACKGROUND);
        if (Audio_Decoder->handle < 0) {
            printf("reinit audio_get_channel() failed, ret=%d\n", Audio_Decoder->handle);
			pthread_mutex_unlock(&Audio_Decoder->codec_mutex);
			return FALSE;
        }
	}

    return TRUE;
}

#endif

static int Q3_Audio_Write(int nChannelNo, const VS_STREAM_TYPE_E enStreamType, char *pData, QMAPI_NET_FRAME_HEADER *pFrame)
{
    struct tm sttm = {0};
	time_t timetick = 0;
	VS_DATETIME_S stDateTime;
	
	if (NULL == pFrame || enStreamType >= VS_STREAM_TYPE_BUTT || nChannelNo >= QMAPI_MAX_CHANNUM || nChannelNo < 0)
	{
		printf("pFrame %p , enStreamType  %d nChannelNo  %d \n" , pFrame , enStreamType , nChannelNo );
		err();
		return 1;
	}

	if(pFrame->dwVideoSize)
	{
		err();
		return 2;
	}
	
	timetick = pFrame->dwTimeTick;
	localtime_r(&timetick, &sttm);
	
	memset(&stDateTime , 0 , sizeof(stDateTime));
	stDateTime.s32Year = 			sttm.tm_year;
	stDateTime.s32Month = 		sttm.tm_mon;
	stDateTime.s32Day = 			sttm.tm_mday;
	stDateTime.s32Hour = 		sttm.tm_hour;
	stDateTime.s32Minute = 		sttm.tm_min;
	stDateTime.s32Second = 	sttm.tm_sec;
	stDateTime.enWeek = 			sttm.tm_wday;
	VS_BUF_WriteFrame(nChannelNo, enStreamType, VS_FRAME_A, (unsigned char *)pData, pFrame->wAudioSize, pFrame->dwTimeTick, &stDateTime);

	return 0;
}

#if 1
void Q3_Audio_pause(void)
{
    g_audioPause = 1;
    while(1)
    {
        if(g_audioPaused)
            return;
        usleep(20000);
    }
}

void Q3_Audio_resume(void)
{
    g_audioPause = 0;
}

void Q3_Audio_Thread(void *param)
{
    char *pFrameBuf = NULL;
    int s32ret, length;//, frame_cnt = 0;
    //static int s32Index = 0;
    QMAPI_NET_FRAME_HEADER Framehdr = {0};
    int count = 0;
    unsigned int u32Time = 0;
    unsigned long long u64Time = 0;

    prctl(PR_SET_NAME, (unsigned long)"AEnc", 0, 0, 0);
	
    int handle;
    void *handleCodec;
    char pcm_name[32] = "default_mic";

    struct fr_buf_info frbuf = FR_BUFINITIALIZER;
    audio_fmt_t real;
	audio_fmt_t fmt;

	fmt.channels = 2;
	fmt.bitwidth = 32;
	fmt.samplingrate = 8000;
	fmt.sample_size = 960;

    int codec_len = 960;
    struct codec_info stCodecInfo;
    struct codec_addr stCodecAddr;
    memset(&stCodecInfo, 0, sizeof(struct codec_info));

    //pFrameBuf = (unsigned char *)malloc(TL_MAX_AENC_LEN);
    pFrameBuf = (char *)malloc(1920);
    if (pFrameBuf == NULL){
        printf("file(%s), line(%d), alloc for audio buffer fail !\n", __FILE__, __LINE__);
        return;
    }

	handle = audio_get_channel(pcm_name, &fmt, CHANNEL_BACKGROUND);
	if (handle < 0) {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
		printf("Get channel err: %d\n", handle);
		return;
	}

    //audio_enable_aec(handle, 1);

	s32ret = audio_get_format(pcm_name, &real);
	if (s32ret < 0) {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
		printf("Get format err: %d\n", s32ret);
		return;
	}
	
	stCodecInfo.in.codec_type = AUDIO_CODEC_PCM;
    stCodecInfo.in.effect = 0;
    stCodecInfo.in.channel = fmt.channels;
    stCodecInfo.in.sample_rate = fmt.samplingrate;
    stCodecInfo.in.bitwidth = fmt.bitwidth;
    stCodecInfo.in.bit_rate = stCodecInfo.in.channel * stCodecInfo.in.sample_rate * GET_BITWIDTH(stCodecInfo.in.bitwidth);
    stCodecInfo.out.codec_type = AUDIO_CODEC_G711A;
    stCodecInfo.out.effect = 0;
    stCodecInfo.out.channel = 1;
    stCodecInfo.out.sample_rate = 8000;
    stCodecInfo.out.bitwidth = 16;
    stCodecInfo.out.bit_rate = stCodecInfo.out.channel * stCodecInfo.out.sample_rate * GET_BITWIDTH(stCodecInfo.out.bitwidth);

    handleCodec = codec_open(&stCodecInfo);
    if (NULL == handleCodec) {
        printf("codec open for type %d failed\n", stCodecInfo.out.codec_type);
        return;
    }

    while(g_audioRun)
    {
        if (g_audioPause)
		{
            g_audioPaused = 1;
            usleep(100000);
            continue;
        }

        if (g_audioPaused)
            g_audioPaused = 0;

        s32ret = audio_get_frame(handle, &frbuf);
		if(s32ret < 0 || (frbuf.size <= 0) || (frbuf.virt_addr == NULL))
		{

		}
		else
		{
			stCodecAddr.in = frbuf.virt_addr;
	        stCodecAddr.len_in = frbuf.size;
	        stCodecAddr.out = pFrameBuf;
	        stCodecAddr.len_out = 1024;//codec_len;

	        int i=0;
	        int *ptr = NULL;
	        ptr = (int *)stCodecAddr.in;
#if 0
	        while(i<(stCodecAddr.len_in/4)){
	            *ptr = *ptr << 1;
	            i++;ptr++;
	        }
#endif
	        ptr = NULL;

			Framehdr.byFrameType = QMAPI_FRAME_A;
	        Framehdr.dwTimeTick = (DWORD)frbuf.timestamp;
	        Framehdr.wAudioSize = codec_len;//stCodecAddr.len_out;
			Framehdr.byAudioCode = QMAPI_PT_G711A;
	        s32ret = codec_encode(handleCodec, &stCodecAddr);
	        //printf("codec_encode %d===>%d\n", frbuf.size, s32ret);

	        if(0 == (count++ %10))
	        {
	            struct timeval tv;
	            gettimeofday(&tv, NULL);

	            u64Time = tv.tv_sec;
	            u64Time *= 1000;
	            u64Time += tv.tv_usec/1000;
	            u32Time = Framehdr.dwTimeTick;
	        }
	        u64Time += Framehdr.dwTimeTick - u32Time;
	        u32Time = Framehdr.dwTimeTick;
			//printf("audio %lu. %llu. sizeof:%u. \n",Framehdr.dwTimeTick,frbuf.timestamp,Framehdr.wAudioSize);
	        Q3_Audio_Write(0, (VS_STREAM_TYPE_E)QMAPI_MAIN_STREAM, (char*)pFrameBuf, &Framehdr);
	       
	        if(m_pfQ3AudioCallBk)
	            m_pfQ3AudioCallBk((int *)pFrameBuf, s32ret, u64Time & 0xffffffff, (u64Time>> 32) & 0xffffffff);
	        audio_put_frame(handle, &frbuf);
		}
		//usleep(10000);
    }
    do{
        s32ret = codec_flush(handleCodec, &stCodecAddr, &length);
        if (s32ret== CODEC_FLUSH_ERR)
            break;

        /* TODO you need least data or not ?*/
    } while (s32ret == CODEC_FLUSH_AGAIN);
	codec_close(handleCodec);
	handleCodec = NULL;
	free(pFrameBuf);
	if (handle >= 0) {
		usleep(150000);
        //audio_enable_aec(handle, 0);
		audio_put_channel(handle);
	}
}

// 设置音频初始化参数
int QMAPI_Audio_Init(void *param)
{
	g_audioRun = 0;
	return 0;
}

int QMAPI_Audio_UnInit(int handle)
{
	return 0;
}

int QMAPI_Audio_Start(int handle)
{
	g_audioRun = 1;
	if (pthread_create(&g_audiothread, NULL, (void *)Q3_Audio_Thread, (void *)QMAPI_MAIN_STREAM))
	{
		g_audiothread = 0;
	}

	return 0;
}

int QMAPI_Audio_Stop(int handle)
{
	g_audioRun = 0;
	if (g_audiothread)
	{
		pthread_join(g_audiothread, NULL);
		g_audiothread = 0;
	}
	return 0;
}

int QMAPI_AudioStreamCallBackSet(CBAudioProc pfun)
{
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    m_pfQ3AudioCallBk = pfun;
    return 0;
}

#else

static void Q3_Audio_Post_l(const void *client, media_aframe_t const *afr)
{    
    static int count = 0;
    char *pFrameBuf = NULL;
    QMAPI_NET_FRAME_HEADER Framehdr = {0};
    unsigned int u32Time = 0;
    unsigned long long u64Time = 0;

    pFrameBuf = afr->buf;
    Framehdr.byFrameType = QMAPI_FRAME_A;
    Framehdr.dwTimeTick = afr->ts;
    Framehdr.wAudioSize = afr->size;
    Framehdr.byAudioCode = QMAPI_PT_G711A;

    if(0 == (count++ %10))
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        u64Time = tv.tv_sec;
        u64Time *= 1000;
        u64Time += tv.tv_usec/1000;
        u32Time = Framehdr.dwTimeTick;
    }
    u64Time += Framehdr.dwTimeTick - u32Time;
    u32Time = Framehdr.dwTimeTick;

    //printf("#### %s %d, u64time:%lld, size:%d\n", __FUNCTION__, __LINE__, u64Time, afr->size);
    Q3_Audio_Write(0, (VS_STREAM_TYPE_E)QMAPI_MAIN_STREAM, (char*)pFrameBuf, &Framehdr);

    if(m_pfQ3AudioCallBk)
        m_pfQ3AudioCallBk((int *)pFrameBuf, afr->size, u64Time & 0xffffffff, (u64Time>> 32) & 0xffffffff);

    return 0;
}

int QMAPI_Audio_Init(void *param)
{ 
    infotm_audio_in_init(Q3_Audio_Post_l);
    return 0;
}

int QMAPI_Audio_UnInit(int handle)
{
    infotm_audio_in_deinit();
	return 0;
}

int QMAPI_Audio_Start(int handle)
{
    infotm_audio_in_start();
	return 0;
}

int QMAPI_Audio_Stop(int handle)
{
    infotm_audio_in_stop();
	return 0;
}

int QMAPI_AudioStreamCallBackSet(CBAudioProc pfun)
{
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    m_pfQ3AudioCallBk = pfun;
    return 0;
}

#endif


