#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <codecs.h>
#include <core.h>

static int ffmpeg_get_frame_number(ffmpeg_fmt_t *fmt, int length)
{
	int nb_channels;

	if (!fmt)
		return -1;
	
	nb_channels = av_get_channel_layout_nb_channels(fmt->channels);
	return length / (nb_channels * av_get_bytes_per_sample(fmt->av_fmt));
}

static int ffmpeg_get_buffer_size(ffmpeg_fmt_t *fmt, int frame_size)
{
	int nb_channels = av_get_channel_layout_nb_channels(fmt->channels);
	int sample_fmt = fmt->av_fmt;

	if (!fmt)
		return -1;

	return av_samples_get_buffer_size(NULL, nb_channels, frame_size, sample_fmt, 1);
}

int ffmpeg_get_framelen(codec_t dev, char *buf, int len)
{
	int codec_type;

	if (!dev) 	// adts header 7 or 9 bytes
		return -1;
	
	if (dev->codec_mode & CODEC_DECODE)
		codec_type = dev->fmt.in.codec_type;
	else
		codec_type = dev->fmt.out.codec_type;

	switch (codec_type) {
		case AUDIO_CODEC_PCM:
		case AUDIO_CODEC_G711U:
		case AUDIO_CODEC_G711A:
		case AUDIO_CODEC_G726:
			return len;
		case AUDIO_CODEC_AAC:
			if (dev->codec_mode & CODEC_DECODE) {
				/* TODO */
				return ((buf[3] & 0x3) << 11) | (buf[4] << 3) | (buf[5] >> 5);
			} else
				return ffmpeg_get_buffer_size(&dev->ffmpeg->in, 1024);
		default:
			CODEC_ERR("ffmpeg get frame not support this mode: %d\n", codec_type);
	}
	return 0;	
}

static int ffmpeg_resample_init(codec_t dev)
{
	ffmpeg_info_t ffmpeg = dev->ffmpeg;
	int ret;

	if (!dev || !ffmpeg)
		return -1;

	dev->swr_ctx = swr_alloc();
	if (!dev->swr_ctx) {
		CODEC_ERR("Could not allocate resampler context\n");
		return -1;
	}

	av_opt_set_int(dev->swr_ctx, "in_channel_layout",    ffmpeg->in.channels, 0);
	av_opt_set_int(dev->swr_ctx, "in_sample_rate",       ffmpeg->in.sample_rate, 0);
	av_opt_set_sample_fmt(dev->swr_ctx, "in_sample_fmt", ffmpeg->in.av_fmt, 0);
	
	av_opt_set_int(dev->swr_ctx, "out_channel_layout",    ffmpeg->out.channels, 0);
	av_opt_set_int(dev->swr_ctx, "out_sample_rate",       ffmpeg->out.sample_rate, 0);
	av_opt_set_sample_fmt(dev->swr_ctx, "out_sample_fmt", ffmpeg->out.av_fmt, 0);

	if ((ret = swr_init(dev->swr_ctx)) < 0) {
		CODEC_ERR("Failed to initialize the resampling context\n");
		swr_free(&dev->swr_ctx);
		return -1;
	}
	return 0;
}

static ssize_t s24le2s32le(uint8_t *dst, uint8_t *src, int len)
{
	int *dst_addr = (int *)dst;
	int *src_addr = (int *)src;
	int i;

	for(i = 0; i< (len / 4); i++)
		*dst_addr++ = *src_addr++ << 8;

	return (len / 4) * 4;
}

static ssize_t s32le2s24le(uint8_t *dst, uint8_t *src, int len)
{
	int *dst_addr = (int *)dst;
	int *src_addr = (int *)src;
	int i;

	for(i=0; i< (len / 4); i++)
		*dst_addr++ = *src_addr++ >> 8;

	return (len / 4) * 4;
}

int ffmpeg_resample(codec_t dev, char *dst, int dstlen, char *src, int srclen)
{
	ffmpeg_info_t ffmpeg = dev->ffmpeg;
	int src_nb_samples, dst_nb_samples, dst_nb_samples_need;
	int ret;
	int length = 0;
	uint8_t *src_addr = (uint8_t *)src;
	uint8_t *dst_addr = ((dst == NULL) ? dev->ref : (uint8_t *)dst);
	int dst_len = ((dst == NULL) ? dev->ref_nsamples : dstlen);
	int bufsize;

	if (!dev || !src_addr)
		return -1;

	if (!dev->swr_ctx) {
		CODEC_ERR("Need ffmpeg resample init first\n");
		return -1;
	}
	
	if (ffmpeg->in.convert) {
		if (dev->cvtlen < srclen) {
			dev->cvt = (uint8_t *)realloc(dev->cvt, srclen);
			if (!dev->cvt) {
				CODEC_ERR("Codec realloc cvt buf err\n");
				return -1;
			}
			dev->cvtlen = srclen;
		}

		s24le2s32le(dev->cvt, (uint8_t *)src, srclen);
		src_addr = dev->cvt;
	}
	src_nb_samples = ffmpeg_get_frame_number(&ffmpeg->in, srclen);
	dst_nb_samples_need = av_rescale_rnd(swr_get_delay(dev->swr_ctx, ffmpeg->in.sample_rate)
				+ src_nb_samples, ffmpeg->out.sample_rate, ffmpeg->in.sample_rate, AV_ROUND_UP);
	dst_nb_samples = ffmpeg_get_frame_number(&ffmpeg->out, dst_len);
	if (dst_nb_samples_need > dst_nb_samples) {
		if (dev->codec_mode & CODEC_ENCODE) {
			bufsize = ffmpeg_get_buffer_size(&ffmpeg->out, dst_nb_samples_need);
			dst_addr = (uint8_t *)realloc(dst_addr, bufsize);
			if (!dst_addr) {
				CODEC_ERR("Could not allocate destination samples\n");
				return -1;
			}
			dst_len = bufsize;

			dev->ref = dst_addr;
			dev->ref_nsamples = dst_len;
		} else {
			CODEC_ERR("FFmpeg resample dst mem less than needed: %d:%d\n",
					dst_nb_samples_need, dst_nb_samples);
			return -1;
		}
	}

	ret = swr_convert(dev->swr_ctx, (uint8_t **)(&dst_addr), dst_nb_samples_need,
						(const uint8_t **)(&src_addr), src_nb_samples);
	if (ret < 0) {
		CODEC_ERR("Error while converting\n");
		return -1;
	}
	length = ret ? ffmpeg_get_buffer_size(&ffmpeg->out, ret) : 0;

	if (ffmpeg->out.convert)
		s32le2s24le(dst_addr, dst_addr, length);
	return length;
}

static void ffmpeg_resample_exit(codec_t dev)
{
	if (!dev)
		return;

	if (dev->swr_ctx)
		swr_free(&dev->swr_ctx);
}

static int ffmpeg_encode_init(codec_t dev)
{
	ffmpeg_info_t ffmpeg = dev->ffmpeg;
	AVCodecContext *codec_ctx;
	AVFrame *frame;

	if (!dev)
		return -1;
	
	dev->codec = avcodec_find_encoder(ffmpeg->out.av_codecid);
	if (!dev->codec) {
		CODEC_ERR("FFmpeg do not support encoder: %d\n", ffmpeg->out.av_codecid);
		return -1;
	}

	codec_ctx = avcodec_alloc_context3(dev->codec);
	if (!codec_ctx) {
		CODEC_ERR("FFmpeg alloc context err\n");
		return -1;
	}
	codec_ctx->bit_rate = ffmpeg->out.bit_rate;
	codec_ctx->sample_fmt = ffmpeg->out.av_fmt;
	codec_ctx->sample_rate = ffmpeg->out.sample_rate;
	codec_ctx->channel_layout = ffmpeg->out.channels;
	codec_ctx->channels = av_get_channel_layout_nb_channels(codec_ctx->channel_layout);
	dev->codec_ctx = codec_ctx;
	
	if (avcodec_open2(dev->codec_ctx, dev->codec, NULL) < 0) {
		CODEC_ERR("Could not open codec\n");
		goto err0;
	}

	frame = av_frame_alloc();
	if (!frame) {
		CODEC_ERR("Could not allocate audio frame\n");
		goto err1;
	}

	//frame->nb_samples = codec_ctx->frame_size;
	frame->format = codec_ctx->sample_fmt;
	frame->channel_layout = codec_ctx->channel_layout;
	dev->frame = frame;
	return 0;

err1:
	avcodec_close(dev->codec_ctx);
err0:
	av_free(dev->codec_ctx);
	return -1;
}

int ffmpeg_encode(codec_t dev, char *dst, int dstlen, char *src, int srclen)
{
	ffmpeg_info_t ffmpeg = dev->ffmpeg;
	AVCodecContext *codec_ctx = dev->codec_ctx;
	AVPacket pkt;
	int got_output;
	int length = 0;
	int ret;
	int need_frame_size, needlen;
	uint8_t *src_addr = (uint8_t *)src;
	
	if (!dev || !dst || !src)
		return -1;
	
	if (!codec_ctx) {
		CODEC_ERR("Need ffmpeg encode first\n");
		return -1;
	}

	dev->frame->nb_samples = ffmpeg_get_frame_number(&ffmpeg->out, srclen);
	/* nsample need 32 alignment */
	need_frame_size = 4 * av_rescale_rnd(dev->frame->nb_samples, 1, 4, AV_ROUND_UP);
	if (dev->frame->nb_samples != need_frame_size) {
		needlen = ffmpeg_get_buffer_size(&ffmpeg->out, need_frame_size);
		if (dev->cvtlen < needlen) {
			dev->cvt = (uint8_t *)realloc(dev->cvt, needlen);
			if (!dev->cvt) {
				CODEC_ERR("Codec realloc cvt buf err\n");
				return -1;
			}
			dev->cvtlen = needlen;
		}
		memcpy(dev->cvt, src, srclen);
		src_addr = dev->cvt;
		srclen = needlen;
	}

	if ((ret = avcodec_fill_audio_frame(dev->frame, codec_ctx->channels, 
						codec_ctx->sample_fmt, src_addr, srclen, 4)) < 0) {
		CODEC_ERR("Could not setup audio frame: %d\n", ret);
		return -1;
	}

	av_init_packet(&pkt);                                            
	pkt.data = NULL;                                                 
	pkt.size = 0;                                                    

	ret = avcodec_encode_audio2(codec_ctx, &pkt, dev->frame, &got_output);
	if (ret < 0) {                                                   
		CODEC_ERR("Error encoding audio frame\n");                   
		return -1;                                                   
	}

	if (got_output) { 
		if ((length + pkt.size) > dstlen) {
			CODEC_ERR("ffmpeg_encode dst mem less than needed: %d:%d\n",
					length + pkt.size, dstlen);
			return -1;
		}

		memcpy(dst, pkt.data, pkt.size);                             
		length += pkt.size;                                          
		av_packet_unref(&pkt);                                       
	}

	return length;
}

codec_flush_t ffmpeg_flush_encode(codec_t dev, char *dst, int dstlen, int *len)
{
	AVCodecContext *codec_ctx = dev->codec_ctx;
	AVPacket pkt;
	int got_output;
	int length = 0;
	int ret;
	
	if (!dev || !dst)
		return CODEC_FLUSH_ERR;
	
	if (!codec_ctx) {
		CODEC_ERR("Need ffmpeg encode first\n");
		return CODEC_FLUSH_ERR;
	}

	av_init_packet(&pkt);                                            
	pkt.data = NULL;                                                 
	pkt.size = 0;                                                    

	ret = avcodec_encode_audio2(codec_ctx, &pkt, NULL, &got_output);
	if (ret < 0) {                                                   
		CODEC_ERR("Error encoding audio frame\n");                   
		return CODEC_FLUSH_ERR;                                                   
	}                                                                

	if (got_output) { 
		if ((length + pkt.size) > dstlen) {
			CODEC_ERR("ffmpeg_encode dst mem less than needed: %d:%d\n",
					length + pkt.size, dstlen);
			return CODEC_FLUSH_ERR;
		}

		memcpy(dst, pkt.data, pkt.size);                             
		length += pkt.size;                                          
		av_packet_unref(&pkt);                                       
	}
	
	if (len)
		*len = length;

	return (got_output ? CODEC_FLUSH_AGAIN : CODEC_FLUSH_FINISH);
}

static void ffmpeg_encode_exit(codec_t dev)
{
	if (!dev)
		return;

	if (dev->frame)
		av_frame_free(&dev->frame);

	if (dev->codec_ctx) {
		avcodec_close(dev->codec_ctx);
		av_free(dev->codec_ctx);
	}
}

static int ffmpeg_decode_init(codec_t dev)
{
	AVCodecContext *codec_ctx = NULL;
	ffmpeg_info_t ffmpeg = dev->ffmpeg;

	if (!dev)
		return -1;
	
	dev->codec = avcodec_find_decoder(ffmpeg->in.av_codecid);
	if (!dev->codec) {
		CODEC_ERR("FFmpeg do not support encoder: %d\n", ffmpeg->in.av_codecid);
		return -1;
	}

	codec_ctx = avcodec_alloc_context3(dev->codec);
	if (!codec_ctx) {
		CODEC_ERR("FFmpeg alloc context err\n");
		return -1;
	}
	codec_ctx->bit_rate = ffmpeg->in.bit_rate;
	codec_ctx->sample_fmt = ffmpeg->in.av_fmt;
	codec_ctx->sample_rate = ffmpeg->in.sample_rate;
	codec_ctx->channel_layout = ffmpeg->in.channels;
	codec_ctx->channels = av_get_channel_layout_nb_channels(codec_ctx->channel_layout);
	if (ffmpeg->in.av_codecid == AV_CODEC_ID_ADPCM_G726)
		codec_ctx->bits_per_coded_sample = (codec_ctx->bit_rate + codec_ctx->sample_rate/2) / codec_ctx->sample_rate;
	dev->codec_ctx = codec_ctx;
	
	if (avcodec_open2(dev->codec_ctx, dev->codec, NULL) < 0) {
		CODEC_ERR("Could not open codec\n");
		goto err0;
	}

	dev->frame = av_frame_alloc();                         
	if (!dev->frame) {                                     
		CODEC_ERR("Could not allocate audio frame\n");
		goto err1;                                    
	}                                                 

	return 0;
err1:
	avcodec_close(dev->codec_ctx);	
err0:
	av_free(dev->codec_ctx);
	return -1;
}

#define max(a, b) ((a > b)?a:b)

int ffmpeg_decode(codec_t dev, char *dst, int dstlen, char *src, int srclen)
{
	ffmpeg_info_t ffmpeg = dev->ffmpeg;
	AVPacket pkt;
	AVCodecContext *codec_ctx = dev->codec_ctx;
	int len, got_frame, i, length = 0;
	int data_size, ch;
	int bufsize, need_bufsize, pcmbps;
	uint8_t *dst_addr = ((dst == NULL) ? dev->ref : (uint8_t *)dst);
	int dst_len = ((dst == NULL) ? dev->ref_nsamples : dstlen);
	
	if (!dev || !src)
		return -1;
	
	if (!dev->codec_ctx) {
		CODEC_ERR("Need ffmpeg decode first\n");
		return -1;
	}

	av_init_packet(&pkt);
	pkt.data = (uint8_t *)src;
	pkt.size = srclen;

	while (pkt.size > 0) {
		len = avcodec_decode_audio4(dev->codec_ctx, dev->frame, &got_frame, &pkt);
		if (len < 0) {
			CODEC_ERR("Error while decoding\n");
			return -1;
		}
		if (got_frame) {
			need_bufsize = length + ffmpeg_get_buffer_size(&ffmpeg->in, dev->frame->nb_samples);
			if (need_bufsize > dst_len) {
				if (dev->codec_mode & CODEC_RESAMPLE) {
					/* get decode pcm bufsize from src data type*/
					pcmbps = ffmpeg_get_buffer_size(&ffmpeg->in, 1) * ffmpeg->in.sample_rate * 8; // decode to pcm bps
					bufsize = av_rescale_rnd(pcmbps, srclen, ffmpeg->in.bit_rate, AV_ROUND_UP);
					/* maybe set bit_rate err or ?*/
					bufsize = max(need_bufsize, bufsize);
					dst_addr = realloc(dst_addr, bufsize);
					if (!dst_addr) {
						CODEC_ERR("Could not allocate destination samples\n");
						return -1;
					}
					dst_len = bufsize;

					dev->ref = dst_addr;
					dev->ref_nsamples = dst_len;
				} else {
					CODEC_ERR("FFmpeg decode dst mem less than needed: %d:%d\n",
							need_bufsize, dst_len);
					return -1;
				}
			}
			if (av_sample_fmt_is_planar(codec_ctx->sample_fmt)) {
				data_size = av_get_bytes_per_sample(codec_ctx->sample_fmt);
				if (data_size < 0) {
					CODEC_ERR("Failed to calculate data size\n");	
					return -1;
				}
				for (i = 0; i < dev->frame->nb_samples; i++) {
					for (ch = 0; ch < codec_ctx->channels; ch++) {
						memcpy(dst_addr + length, dev->frame->data[ch] + data_size * i, data_size);
						length += data_size;
					}
				}
			} else {
				memcpy(dst_addr + length, dev->frame->data[0], dev->frame->linesize[0]);
				length += dev->frame->linesize[0];
			}
		}
		pkt.size -= len;
		pkt.data += len;
		pkt.dts =
		pkt.pts = AV_NOPTS_VALUE;
	}

	return length;
}

static void ffmpeg_decode_exit(codec_t dev)
{
	if (!dev)
		return;
	
	if (dev->frame)
		av_frame_free(&dev->frame);

	if (dev->codec_ctx) {
		avcodec_close(dev->codec_ctx);	
		av_free(dev->codec_ctx);
	}
}

static int ffmpeg_get_pcm_codecid(int sample_fmt)
{
	switch (sample_fmt) {
		case 8:
			return AV_CODEC_ID_PCM_S8;
		case 16:
			return AV_CODEC_ID_PCM_S16LE;
		case 24:
		case 32:
			return AV_CODEC_ID_PCM_S32LE;
		default:
			CODEC_ERR("Codec do not support this codec id: %d\n", sample_fmt);
			return -1;
	}
}

static int ffmpeg_get_codecid(int codec_type, int sample_fmt)
{
	switch (codec_type) {
		case AUDIO_CODEC_PCM:
			return ffmpeg_get_pcm_codecid(sample_fmt);
		case AUDIO_CODEC_G711U:
			return AV_CODEC_ID_PCM_MULAW;
		case AUDIO_CODEC_G711A:
			return AV_CODEC_ID_PCM_ALAW;
		case AUDIO_CODEC_AAC:
			return AV_CODEC_ID_AAC;
		case AUDIO_CODEC_G726:
			return AV_CODEC_ID_ADPCM_G726;
        case AUDIO_CODEC_MP3:
            return AV_CODEC_ID_MP3;
		case AUDIO_CODEC_ADPCM:
		//case AUDIO_CODEC_G726:
		case AUDIO_CODEC_SPEEX:
		default:
			CODEC_ERR("Codec do not support this codec type: %d\n", codec_type);
			return -1;
	}
}

static int ffmpeg_get_samplefmt(int sample_fmt)
{
	switch (sample_fmt) {
		case 8:
			return AV_SAMPLE_FMT_U8;
		case 16:
			return AV_SAMPLE_FMT_S16;
		case 24:
		case 32:
			return AV_SAMPLE_FMT_S32;
		default:
			CODEC_ERR("Codec do not support this sample fmt: %d\n", sample_fmt);
			return -1;
	}
}

static int ffmpeg_get_channels(int channels)
{
	switch (channels) {
		case 1:
			return AV_CH_LAYOUT_MONO;
		case 2:
			return AV_CH_LAYOUT_STEREO;
		default:
			CODEC_ERR("Codec do not support this channels: %d\n", channels);
			return -1;
	}
}

static int ffmpeg_convert_format(codec_t dev)
{
	codec_info_t *fmt = &dev->fmt;
	ffmpeg_info_t ffmpeg;

	ffmpeg = (ffmpeg_info_t)malloc(sizeof(*ffmpeg));
	if (!ffmpeg)
		return -1;

	ffmpeg->in.av_codecid = ffmpeg_get_codecid(fmt->in.codec_type, fmt->in.bitwidth);
	ffmpeg->in.av_fmt = ffmpeg_get_samplefmt(fmt->in.bitwidth);
	ffmpeg->in.channels = ffmpeg_get_channels(fmt->in.channel);
	ffmpeg->in.bit_rate = fmt->in.bit_rate;
	ffmpeg->in.sample_rate = fmt->in.sample_rate;
	ffmpeg->in.convert = (fmt->in.bitwidth == 24);
	ffmpeg->out.av_codecid = ffmpeg_get_codecid(fmt->out.codec_type, fmt->out.bitwidth);
	ffmpeg->out.av_fmt = ffmpeg_get_samplefmt(fmt->out.bitwidth);
	ffmpeg->out.channels = ffmpeg_get_channels(fmt->out.channel);
	ffmpeg->out.bit_rate = fmt->out.bit_rate;
	ffmpeg->out.sample_rate = fmt->out.sample_rate;
	ffmpeg->out.convert = (fmt->out.bitwidth == 24);
	dev->ffmpeg = ffmpeg;

	return 0;
}

static int ffmpeg_malloc_refbuf(codec_t dev)
{
	ffmpeg_info_t ffmpeg = dev->ffmpeg;

	if (!ffmpeg)
		return -1;

	dev->ref_nsamples = ffmpeg_get_buffer_size(&ffmpeg->out, CODEC_REF_BUF);
	dev->ref = (uint8_t *)malloc(dev->ref_nsamples);
	if (!dev->ref) {
		CODEC_ERR("Could not allocate destination samples\n");
		return -1;
	}

	dev->cvt = (uint8_t *)malloc(CODEC_CVT_BUF);
	if (!dev->cvt) {
		CODEC_ERR("Could not allocate 24bit input cvt buf\n");
		return -1;
	}
	dev->cvtlen = CODEC_CVT_BUF;
	return 0;
}

int ffmpeg_init(codec_t dev)
{
	int ret = 0;
	
	if (!dev)
		return -1;

	if (ffmpeg_convert_format(dev) < 0)
		return -1;
	
	if (ffmpeg_malloc_refbuf(dev) < 0)
		return -1;
	
	if (dev->codec_mode & CODEC_RESAMPLE)
		ret += ffmpeg_resample_init(dev);

	if (dev->codec_mode & CODEC_ENCODE)
		ret += ffmpeg_encode_init(dev);

	if (dev->codec_mode & CODEC_DECODE)
		ret += ffmpeg_decode_init(dev);

	return ret;
}

void ffmpeg_exit(codec_t dev)
{
	if (!dev)
		return;
	
	if (dev->codec_mode & CODEC_RESAMPLE)
		ffmpeg_resample_exit(dev);

	if (dev->codec_mode & CODEC_ENCODE)
		ffmpeg_encode_exit(dev);

	if (dev->codec_mode & CODEC_DECODE)
		ffmpeg_decode_exit(dev);
	
	if (dev->ref)     
		free(dev->ref);
	
	if (dev->cvt)
		free(dev->cvt);

	if (dev->ffmpeg)
		free(dev->ffmpeg);
}
