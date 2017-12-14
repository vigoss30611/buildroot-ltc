#include <codecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <core.h>
#ifdef DSP_AAC_SUPPORT
#include <dsp/lib-tl421.h>
#include <dsp/tl421-user.h>
#endif

static int codec_check_fmt(codec_info_t *fmt)
{
	codec_fmt_t *in = &fmt->in;
	codec_fmt_t *out = &fmt->out;
	int value = 0;

	if (!fmt)
		return -1;
	
	if (in->codec_type != AUDIO_CODEC_PCM && out->codec_type != AUDIO_CODEC_PCM)
		return -1;

	if (in->codec_type == AUDIO_CODEC_PCM && out->codec_type == AUDIO_CODEC_PCM)
		value = CODEC_RESAMPLE;
	else if (in->codec_type == AUDIO_CODEC_PCM)
		value = CODEC_ENCODE;
	else
		value = CODEC_DECODE;
	
	if (in->effect != out->effect ||
		in->channel != out->channel ||
		in->sample_rate != out->sample_rate ||
		in->bitwidth != out->bitwidth ||
		in->bit_rate != out->bit_rate)
		value |= CODEC_RESAMPLE;

	return value;
}

void *codec_open(codec_info_t *fmt)
{
	codec_t dev;
	int ret;

	if (!fmt)
		return NULL;

	CODEC_INFO(
			 ("Audio Codec:\n"
			  "In:\n"
			  "codec_type: %d\n"
			  "effect: %d\n"
			  "channel: %d\n"
			  "sample_rate: %d\n"
			  "bitwidth: %d\n"
			  "bit_rate: %d\n"
			  "Out:\n"
			  "codec_type: %d\n"
			  "effect: %d\n"
			  "channel: %d\n"
			  "sample_rate: %d\n"
			  "bitwidth: %d\n"
			  "bit_rate: %d\n"
			  ), fmt->in.codec_type, fmt->in.effect, fmt->in.channel,
			  fmt->in.sample_rate, fmt->in.bitwidth, fmt->in.bit_rate,
			  fmt->out.codec_type, fmt->out.effect, fmt->out.channel,
			  fmt->out.sample_rate, fmt->out.bitwidth, fmt->out.bit_rate);

	dev = (codec_t)malloc(sizeof(*dev));
	if (!dev) {
		CODEC_ERR("Codec malloc err\n");
		return NULL;
	}
	memcpy(&dev->fmt, fmt, sizeof(codec_info_t));

	dev->codec_mode = codec_check_fmt(fmt);
	if (dev->codec_mode < 0) {
		CODEC_ERR("Codec check fmt err\n");
		return NULL;
	}

#ifdef DSP_AAC_SUPPORT
	CODEC_INFO("dev->codec_mode %d, fmt->out.codec_type %d\n", dev->codec_mode, fmt->out.codec_type);
	if ((dev->codec_mode & CODEC_ENCODE) && 
			(fmt->out.codec_type == AUDIO_CODEC_AAC)) {
		ret = ceva_tl421_open(NULL);
		if (ret < 0) {
			CODEC_ERR("Open DSP failed, please decode audio with ffmpeg\n");
		}

		ret = ceva_tl421_set_format(NULL, fmt);
		if (ret < 0) {
			CODEC_ERR("DSP can not support the format\n");
		} else {
			dev->codec_dsp = 1;
		}
	}
#endif

	av_register_all();

	ret = ffmpeg_init(dev);
	if (ret < 0) {
		CODEC_ERR("FFmpeg init err\n");
		return NULL;
	}

	dev->dframe = (uint8_t *)malloc(CODEC_DF_BUF);
	if (!dev->dframe) {
		CODEC_ERR("Codec malloc dframe err\n");
		return NULL;
	}
	dev->dframe_len = CODEC_DF_BUF;
	dev->dfrestore = 0;
	dev->framelen = 0;

	return dev;
}

int codec_close(void *codec)
{
	codec_t dev = (codec_t)codec;

	if (!dev)
		return -1;

	if (dev->dframe)
		free(dev->dframe);

#ifdef DSP_AAC_SUPPORT
	if (dev->codec_dsp)
		ceva_tl421_close(NULL);
#endif

	ffmpeg_exit(dev);
	free(dev);

	return 0;
}

int codec_encode_frame(void *codec, codec_addr_t *desc)
{
	codec_t dev = (codec_t)codec;
	int need = 0, length = 0;

	if (!dev || !desc)
		return -1;
	
	if (dev->codec_mode & CODEC_RESAMPLE) {
		if (dev->codec_mode & CODEC_ENCODE) {
			length = ffmpeg_resample(dev, NULL, 0, desc->in, desc->len_in);
			if (length < 0)
				return -1;
			need = 1;
		} else 
			return ffmpeg_resample(dev, desc->out, desc->len_out, desc->in, desc->len_in);
	}

#ifdef DSP_AAC_SUPPORT
	if (dev->codec_dsp)
		return ceva_tl421_encode(NULL, desc->out, (need ? dev->ref : desc->in),
				(need ? length : desc->len_in));
#endif
	return ffmpeg_encode(dev, desc->out, desc->len_out, (need ? dev->ref : desc->in),
						(need ? length : desc->len_in));
}

int codec_encode(void *codec, codec_addr_t *desc)
{
	codec_t dev = (codec_t)codec;
	codec_addr_t addr;
	int needlen, frame_len;
	int least, length = 0;
	int encode_len = 0;

	if (dev->dfrestore > 0) {
		needlen = dev->dfrestore + desc->len_in;
		if (dev->dframe_len < needlen) {
			dev->dframe = (uint8_t *)realloc(dev->dframe, needlen);
			if (!dev->dframe) {
				CODEC_ERR("Codec realloc encode buf err\n");
				return -1;
			}
			dev->dframe_len = needlen;
		}
		memcpy(dev->dframe + dev->dfrestore, desc->in, desc->len_in);
		dev->dfrestore += desc->len_in;
	}

	addr.in = (dev->dfrestore > 0) ? dev->dframe : desc->in;
	addr.len_in = (dev->dfrestore > 0) ? dev->dfrestore : desc->len_in;
	addr.out = desc->out;
	addr.len_out = desc->len_out;
	
	frame_len = ffmpeg_get_framelen(dev, addr.in, addr.len_in);
	if (frame_len <= 0 && dev->framelen <= 0) {
		CODEC_ERR("Codec encode get frame len err\n");
		return -1;
	}
	if (frame_len > 0)
		dev->framelen = frame_len;

	least = addr.len_in;
	while (least >= dev->framelen) {
		addr.len_in = dev->framelen;
		addr.out += length;
		addr.len_out -= length;
		length = codec_encode_frame(dev, &addr);
		if (length < 0) {
			CODEC_ERR("Codec encode a frame err, length:%d\n", length);
			return length;
		}

		encode_len += length;
		least -= dev->framelen;
		addr.in += dev->framelen;
	}

	if (least > 0) {
		if (dev->dframe_len < least) {
			dev->dframe = (uint8_t *)realloc(dev->dframe, least);
			if (!dev->dframe) {
				CODEC_ERR("Codec realloc encode buf err\n");
				return -1;
			}
			dev->dframe_len = least;
		}
		memcpy(dev->dframe, addr.in, least);
	}

	dev->dfrestore = least;

	return encode_len;
}

int codec_decode_frame(codec_t codec, codec_addr_t *desc)
{
	codec_t dev = codec;
	int length = 0, need = 0;

	if (!codec || !desc)
		return -1;
	
	if (dev->codec_mode & CODEC_RESAMPLE)
		need = 1;
	
	if (dev->codec_mode & CODEC_DECODE) 
		length = ffmpeg_decode((codec_t)codec, (need ? NULL : desc->out),
			(need ? 0 : desc->len_out), desc->in, desc->len_in);

	if (dev->codec_mode & CODEC_RESAMPLE)
		length = ffmpeg_resample(dev, desc->out, desc->len_out,
				(dev->codec_mode & CODEC_DECODE) ? dev->ref : desc->in,
				(dev->codec_mode & CODEC_DECODE) ? length : desc->len_in);

	return length;
}

int codec_decode(void *codec, codec_addr_t *desc)
{
	codec_t dev = (codec_t)codec;
	codec_addr_t addr;
	int needlen, frame_len;
	int least, length = 0;
	int decode_len = 0;

	if (dev->dfrestore > 0) {
		needlen = dev->dfrestore + desc->len_in;
		if (dev->dframe_len < needlen) {
			dev->dframe = (uint8_t *)realloc(dev->dframe, needlen);
			if (!dev->dframe) {
				CODEC_ERR("Codec realloc decode buf err\n");
				return -1;
			}
			dev->dframe_len = needlen;
		}
		memcpy(dev->dframe + dev->dfrestore, desc->in, desc->len_in);
		dev->dfrestore += desc->len_in;
	}

	addr.in = (dev->dfrestore > 0) ? dev->dframe : desc->in;
	addr.len_in = (dev->dfrestore > 0) ? dev->dfrestore : desc->len_in;
	addr.out = desc->out;
	addr.len_out = desc->len_out;

	length = codec_decode_frame(dev, &addr);
	if (length <= 0) {
		CODEC_ERR("Codec decode a frame err, length:%d\n", length);
		return length;
	}

	return length;
	
	if (!dev->framelen) {
		frame_len = ffmpeg_get_framelen(dev, desc->in, desc->len_in);
		if (frame_len <= 0 && dev->framelen <= 0) {
			CODEC_ERR("Codec decode data don`t have framelen\n");
			return -1;
		}
		if (frame_len > 0)
			dev->framelen = frame_len;
	}

	least = addr.len_in;
	while (least >= dev->framelen) {
		addr.len_in = dev->framelen;
		addr.out += length;
		addr.len_out -= length;
		length = codec_decode_frame(dev, &addr);
		if (length < 0) {
			CODEC_ERR("Codec decode a frame err, length:%d\n", length);
			return length;
		}

		decode_len += length;
		least -= dev->framelen;
		addr.in += dev->framelen;
	}

	if (least > 0) {
		if (dev->dframe_len < least) {
			dev->dframe = (uint8_t *)realloc(dev->dframe, least);
			if (!dev->dframe) {
				CODEC_ERR("Codec realloc decode buf err\n");
				return -1;
			}
			dev->dframe_len = least;
		}
		memcpy(dev->dframe, addr.in, least);
	}

	dev->dfrestore = least;

	return decode_len;
}

int codec_resample(void *codec, char *dst, int dstlen, char *src, int srclen)
{
	codec_t dev = (codec_t)codec;

	if (!dev)
		return -1;

	return ffmpeg_resample(dev, dst, dstlen, src, srclen);
}

codec_flush_t codec_flush(void *codec, codec_addr_t *desc, int *len)
{
	codec_t dev = (codec_t)codec;
	codec_addr_t addr;

	if (!dev || !desc || !len)
		return -1;

	if (dev->dfrestore > 0) {
		addr.in = dev->dframe;
		addr.len_in = dev->dfrestore;
		addr.out = desc->out;
		addr.len_out = desc->len_out;
	}

	if (dev->codec_mode & CODEC_ENCODE) {
		if (dev->dfrestore > 0) {

			*len = codec_encode_frame(dev, &addr);
			dev->dfrestore = 0;

			return CODEC_FLUSH_AGAIN;
		}
		return ffmpeg_flush_encode(dev, desc->out, desc->len_out, len);
	}
	
	if (dev->codec_mode & CODEC_DECODE) {
		if (dev->dfrestore > 0) {
			*len = codec_decode_frame(dev, &addr);
			dev->dfrestore = 0;

			return CODEC_FLUSH_FINISH;
		}
	}
	
	*len = 0;
	return CODEC_FLUSH_FINISH;	
}
