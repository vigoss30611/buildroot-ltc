#ifndef __CODECS_CORE_H__
#define __CODECS_CORE_H__

#define CODEC_DEBUG

#ifdef CODEC_DEBUG
#define CODEC_DBG(arg...)	fprintf(stderr, arg)
#else
#define CODEC_DBG(arg...)
#endif
#define CODEC_ERR(arg...)  fprintf(stderr, arg)
#define CODEC_INFO(arg...)  fprintf(stderr, arg)

#define CODEC_RESAMPLE		(0x1 << 0)
#define CODEC_ENCODE		(0x1 << 1)
#define CODEC_DECODE		(0x1 << 2)

#define ENCODE_MODE			1
#define DECODE_MODE			2

struct ffmpeg_format {
	enum AVCodecID av_codecid;
	enum AVSampleFormat av_fmt;
	int channels;
	int bit_rate;
	int sample_rate;
	int convert;		// for 24bit to 32bit sample_fmt
};
typedef struct ffmpeg_format ffmpeg_fmt_t;

struct ffmpeg_info {
	ffmpeg_fmt_t in;
	ffmpeg_fmt_t out;
};
typedef struct ffmpeg_info *ffmpeg_info_t;

struct codec_dev {
	codec_info_t fmt;
	ffmpeg_info_t ffmpeg;		// for ffmpeg format
	int codec_mode;
	int codec_dsp;			// for dsp codec
	/* for libcodecs internal use */
#define CODEC_REF_BUF	1024
	uint8_t *ref;
	int ref_nsamples;
	/* for decode or encodec */
	AVCodec *codec;
	AVCodecContext *codec_ctx;
	AVFrame *frame;
	/* for resample */
	struct SwrContext *swr_ctx;
	/* for 24bit data input */
	uint8_t *cvt;
#define CODEC_CVT_BUF	4096
	int cvtlen;
	/* for decode frame align */
	uint8_t *dframe;
#define CODEC_DF_BUF	4096
	int dframe_len;		// malloc dframe len
	int dfrestore;		// restore not alignment decode frame
	int framelen;		// parse decode frame len
};
typedef struct codec_dev * codec_t;

extern int ffmpeg_resample(codec_t dev, char *dst, int dstlen, char *src, int srclen);
extern int ffmpeg_encode(codec_t dev, char *dst, int dstlen, char *src, int srclen);
extern codec_flush_t ffmpeg_flush_encode(codec_t dev, char *dst, int dstlen, int *len);
extern int ffmpeg_decode(codec_t dev, char *dst, int dstlen, char *src, int srclen);
extern int ffmpeg_get_framelen(codec_t dev, char *buf, int len);
extern int ffmpeg_init(codec_t dev);
extern void ffmpeg_exit(codec_t dev);

#endif
