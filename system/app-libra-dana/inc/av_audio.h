
#ifndef _AUDIO_H_
#define _AUDIO_H_

#define ALG_AUD_CODEC_PCM      0
#define ALG_AUD_CODEC_G711U    1
#define ALG_AUD_CODEC_G711A    2
#define ALG_AUD_CODEC_ADPCM    3
#define ALG_AUD_CODEC_G726     4
#define ALG_AUD_CODEC_MP3      5
#define ALG_AUD_CODEC_SPEEX    6
#define ALG_AUD_CODEC_AACLC    7

#define C_AUDIO_LEN  (128)
#define PLAYINPUTSIZE 256

#include <drv_audio.h>
#include "osa_thr.h"
#define	AUDIO_BUF_SIZE	1024


#define DEFAULT_CODEC_PLAY		"default"
#define DEFAULT_CODEC_RECORD	"default_mic"

#define DEFAULT_CODEC_SAMPLERATE	48000
#define DEFAULT_CODEC_BITWIDTH		16
#define DEFAULT_CODEC_CHANNLES		2

typedef enum _AUDIO_CHANNEL 
{
    AUDIO_CHAN_MONO =1,
	AUDIO_CHAN_STEREO =2
}ADUIO_CHANNEL;

typedef enum _AUDIO_FORMAT 
{
    AUDIO_FMT_16 =16,
	AUDIO_FMT_24 =24
}ADUIO_FORMAT;

typedef enum _AUDIO_RATE 
{
    AUDIO_RATE_8K =8000,
	AUDIO_RATE_48K =48000
}ADUIO_RATE;



typedef struct {
  void *physAddr;
  void *virtAddr;
  unsigned long long  timestamp;
  unsigned int encFrameSize;
} AUDIO_BufInfo;



typedef struct {
  OSA_ThrHndl 		audioTsk;
  DRV_AudioHndl 	audioHndl; 
  int 				streamId;
  void 			    *encodedBuffer;
  int				encodeBufSize;
  void 	            *inputBuffer;
  int				inputBufSize;
  int               bResample;
  void 	            *ResampBuffer;
  int				ResampBufSize;
} AUDIO_Ctrl;

int AUDIO_audioCreate();
int AUDIO_audioDelete();

int audio_stream_procdata(int streamId, AUDIO_BufInfo *pBufInfo);

int AUDIO_streamDeal(int streamId,void * buf,int buflen);
int AUDIO_streamPlayClose();

#endif  /*  _AUDIO_H_ */

