#ifndef __CODECS_H__
#define __CODECS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum AUDIO_CODEC_TYPE {
	AUDIO_CODEC_PCM,
	AUDIO_CODEC_G711U,
	AUDIO_CODEC_G711A,
	AUDIO_CODEC_ADPCM,
	AUDIO_CODEC_G726,
	AUDIO_CODEC_MP3,
	AUDIO_CODEC_SPEEX,
	AUDIO_CODEC_AAC,
	AUDIO_CODEC_MAX
} CODEC_TYPE;

typedef enum CODEC_FLUSH_RESULT {
	CODEC_FLUSH_ERR = -1,
	CODEC_FLUSH_AGAIN,
	CODEC_FLUSH_FINISH,
} codec_flush_t;

struct codec_format {
	int codec_type;
	int effect;
	int channel;
	int sample_rate;
	int bitwidth;
	int bit_rate;
};
typedef struct codec_format codec_fmt_t;

struct codec_info {
	codec_fmt_t in;
	codec_fmt_t out;
};
typedef struct codec_info codec_info_t;

struct codec_addr {
	void *in;
	int len_in;
	void *out;
	int len_out;
};
typedef struct codec_addr codec_addr_t;

#define GET_BITWIDTH(x) ((x > 16) ? 32 : x)

extern void *codec_open(codec_info_t *fmt);
extern int codec_close(void *codec);
extern int codec_encode(void *codec, codec_addr_t *desc);
extern int codec_decode(void *codec, codec_addr_t *desc);
extern int codec_resample(void *codec, char *dst, int dstlen, char *src, int srclen);
extern codec_flush_t codec_flush(void *codec, codec_addr_t *desc, int *len);

#ifdef __cplusplus
}
#endif
#endif
