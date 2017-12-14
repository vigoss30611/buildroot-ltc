/**
 * \file pcm/pcm_softvol.c
 * \ingroup PCM_Plugins
 * \brief PCM Soft Volume Plugin Interface
 * \author Takashi Iwai <tiwai@suse.de>
 * \date 2004
 */
/*
 *  PCM - Soft Volume Plugin
 *  Copyright (c) 2004 by Takashi Iwai <tiwai@suse.de>
 */

#include <byteswap.h>
#include <math.h>
#include <audio_softvol.h>
#include <log.h>
#include <pthread.h>

#define VOL_SCALE_SHIFT		16
#define VOL_SCALE_MASK          ((1 << VOL_SCALE_SHIFT) - 1)

#define MAX_DB_UPPER_LIMIT      50

static const unsigned int preset_dB_value[PRESET_RESOLUTION] = {
	0x00b8, 0x00bd, 0x00c1, 0x00c5, 0x00ca, 0x00cf, 0x00d4, 0x00d9,
	0x00de, 0x00e3, 0x00e8, 0x00ed, 0x00f3, 0x00f9, 0x00fe, 0x0104,
	0x010a, 0x0111, 0x0117, 0x011e, 0x0124, 0x012b, 0x0132, 0x0139,
	0x0140, 0x0148, 0x0150, 0x0157, 0x015f, 0x0168, 0x0170, 0x0179,
	0x0181, 0x018a, 0x0194, 0x019d, 0x01a7, 0x01b0, 0x01bb, 0x01c5,
	0x01cf, 0x01da, 0x01e5, 0x01f1, 0x01fc, 0x0208, 0x0214, 0x0221,
	0x022d, 0x023a, 0x0248, 0x0255, 0x0263, 0x0271, 0x0280, 0x028f,
	0x029e, 0x02ae, 0x02be, 0x02ce, 0x02df, 0x02f0, 0x0301, 0x0313,
	0x0326, 0x0339, 0x034c, 0x035f, 0x0374, 0x0388, 0x039d, 0x03b3,
	0x03c9, 0x03df, 0x03f7, 0x040e, 0x0426, 0x043f, 0x0458, 0x0472,
	0x048d, 0x04a8, 0x04c4, 0x04e0, 0x04fd, 0x051b, 0x053a, 0x0559,
	0x0579, 0x0599, 0x05bb, 0x05dd, 0x0600, 0x0624, 0x0648, 0x066e,
	0x0694, 0x06bb, 0x06e3, 0x070c, 0x0737, 0x0762, 0x078e, 0x07bb,
	0x07e9, 0x0818, 0x0848, 0x087a, 0x08ac, 0x08e0, 0x0915, 0x094b,
	0x0982, 0x09bb, 0x09f5, 0x0a30, 0x0a6d, 0x0aab, 0x0aeb, 0x0b2c,
	0x0b6f, 0x0bb3, 0x0bf9, 0x0c40, 0x0c89, 0x0cd4, 0x0d21, 0x0d6f,
	0x0dbf, 0x0e11, 0x0e65, 0x0ebb, 0x0f12, 0x0f6c, 0x0fc8, 0x1026,
	0x1087, 0x10e9, 0x114e, 0x11b5, 0x121f, 0x128b, 0x12fa, 0x136b,
	0x13df, 0x1455, 0x14ce, 0x154a, 0x15c9, 0x164b, 0x16d0, 0x1758,
	0x17e4, 0x1872, 0x1904, 0x1999, 0x1a32, 0x1ace, 0x1b6e, 0x1c11,
	0x1cb9, 0x1d64, 0x1e13, 0x1ec7, 0x1f7e, 0x203a, 0x20fa, 0x21bf,
	0x2288, 0x2356, 0x2429, 0x2500, 0x25dd, 0x26bf, 0x27a6, 0x2892,
	0x2984, 0x2a7c, 0x2b79, 0x2c7c, 0x2d85, 0x2e95, 0x2fab, 0x30c7,
	0x31ea, 0x3313, 0x3444, 0x357c, 0x36bb, 0x3801, 0x394f, 0x3aa5,
	0x3c02, 0x3d68, 0x3ed6, 0x404d, 0x41cd, 0x4355, 0x44e6, 0x4681,
	0x4826, 0x49d4, 0x4b8c, 0x4d4f, 0x4f1c, 0x50f3, 0x52d6, 0x54c4,
	0x56be, 0x58c3, 0x5ad4, 0x5cf2, 0x5f1c, 0x6153, 0x6398, 0x65e9,
	0x6849, 0x6ab7, 0x6d33, 0x6fbf, 0x7259, 0x7503, 0x77bd, 0x7a87,
	0x7d61, 0x804d, 0x834a, 0x8659, 0x897a, 0x8cae, 0x8ff5, 0x934f,
	0x96bd, 0x9a40, 0x9dd8, 0xa185, 0xa548, 0xa922, 0xad13, 0xb11b,
	0xb53b, 0xb973, 0xbdc5, 0xc231, 0xc6b7, 0xcb58, 0xd014, 0xd4ed,
	0xd9e3, 0xdef6, 0xe428, 0xe978, 0xeee8, 0xf479, 0xfa2b, 0xffff,
};

/* (32bit x 16bit) >> 16 */
typedef union {
	int i;
	short s[2];
} val_t;
static inline int MULTI_DIV_32x16(int a, unsigned short b)
{
	val_t v, x, y;
	v.i = a;
	y.i = 0;

	x.i = (unsigned short)v.s[0];
	x.i *= b;
	y.s[0] = x.s[1];
	y.i += (int)v.s[1] * b;
	return y.i;
}

static inline int MULTI_DIV_int(int a, unsigned int b, int swap)
{
	unsigned int gain = (b >> VOL_SCALE_SHIFT);
	int fraction;
	a = swap ? (int)bswap_32(a) : a;
	fraction = MULTI_DIV_32x16(a, b & VOL_SCALE_MASK);
	if (gain) {
		long long amp = (long long)a * gain + fraction;
		if (amp > (int)0x7fffffff)
			amp = (int)0x7fffffff;
		else if (amp < (int)0x80000000)
			amp = (int)0x80000000;
		return swap ? (int)bswap_32((int)amp) : (int)amp;
	}
	return swap ? (int)bswap_32(fraction) : fraction;
}

/* always little endian */
static inline int MULTI_DIV_24(int a, unsigned int b)
{
	unsigned int gain = b >> VOL_SCALE_SHIFT;
	int fraction;
	fraction = MULTI_DIV_32x16(a, b & VOL_SCALE_MASK);
	if (gain) {
		long long amp = (long long)a * gain + fraction;
		if (amp > (int)0x7fffff)
			amp = (int)0x7fffff;
		else if (amp < (int)0x800000)
			amp = (int)0x800000;
		return (int)amp;
	}
	return fraction;
}

static inline short MULTI_DIV_short(short a, unsigned int b, int swap)
{
	unsigned int gain = b >> VOL_SCALE_SHIFT;
	int fraction;
	a = swap ? (short)bswap_16(a) : a;
	fraction = (int)(a * (b & VOL_SCALE_MASK)) >> VOL_SCALE_SHIFT;
	if (gain) {
		int amp = a * gain + fraction;
		if (abs(amp) > 0x7fff)
			amp = (a<0) ? (short)0x8000 : (short)0x7fff;
		return swap ? (short)bswap_16((short)amp) : (short)amp;
	}
	return swap ? (short)bswap_16((short)fraction) : (short)fraction;
}


// copy from alsa_lib
static inline void *snd_pcm_channel_area_addr(const snd_pcm_channel_area_t *area, snd_pcm_uframes_t offset)
{
	unsigned int bitofs = area->first + area->step * offset;
	
	return (char *) area->addr + bitofs / 8;
}

static inline unsigned int snd_pcm_channel_area_step(const snd_pcm_channel_area_t *area)
{
	return area->step / 8;
}

/*
 * apply volumue attenuation
 *
 * TODO: use SIMD operations
 */

#define CONVERT_AREA(TYPE, swap) do {	\
	unsigned int ch, fr; \
	TYPE *src, *dst; \
	for (ch = 0; ch < channels; ch++) { \
		src_area = &src_areas[ch]; \
		dst_area = &dst_areas[ch]; \
		src = snd_pcm_channel_area_addr(src_area, src_offset); \
		dst = snd_pcm_channel_area_addr(dst_area, dst_offset); \
		src_step = snd_pcm_channel_area_step(src_area) / sizeof(TYPE); \
		dst_step = snd_pcm_channel_area_step(dst_area) / sizeof(TYPE); \
		GET_VOL_SCALE; \
		fr = frames; \
		if (! vol_scale) { \
			while (fr--) { \
				*dst = 0; \
				dst += dst_step; \
			} \
		} else if (vol_scale == 0xffff) { \
			while (fr--) { \
				*dst = *src; \
				src += src_step; \
				dst += dst_step; \
			} \
		} else { \
			while (fr--) { \
				*dst = (TYPE) MULTI_DIV_##TYPE(*src, vol_scale, swap); \
				src += src_step; \
				dst += dst_step; \
			} \
		} \
	} \
} while (0)

#define CONVERT_AREA_S24_3LE() do {					\
	unsigned int ch, fr;						\
	unsigned char *src, *dst;					\
	int tmp;							\
	for (ch = 0; ch < channels; ch++) {				\
		src_area = &src_areas[ch];				\
		dst_area = &dst_areas[ch];				\
		src = snd_pcm_channel_area_addr(src_area, src_offset);	\
		dst = snd_pcm_channel_area_addr(dst_area, dst_offset);	\
		src_step = snd_pcm_channel_area_step(src_area);		\
		dst_step = snd_pcm_channel_area_step(dst_area);		\
		GET_VOL_SCALE;						\
		fr = frames;						\
		if (! vol_scale) {					\
			while (fr--) {					\
				dst[0] = dst[1] = dst[2] = 0;		\
				dst += dst_step;			\
			}						\
		} else if (vol_scale == 0xffff) {			\
			while (fr--) {					\
				dst[0] = src[0];			\
				dst[1] = src[1];			\
				dst[2] = src[2];			\
				src += dst_step;			\
				dst += src_step;			\
			}						\
		} else {						\
			while (fr--) {					\
				tmp = src[0] |				\
				      (src[1] << 8) |			\
				      (((signed char *) src)[2] << 16);	\
				tmp = MULTI_DIV_24(tmp, vol_scale);	\
				dst[0] = tmp;				\
				dst[1] = tmp >> 8;			\
				dst[2] = tmp >> 16;			\
				src += dst_step;			\
				dst += src_step;			\
			}						\
		}							\
	}								\
} while (0)
		
#define GET_VOL_SCALE \
	switch (ch) { \
	case 0: \
	case 2: \
		vol_scale = (channels == ch + 1) ? vol_c : vol[0]; \
		break; \
	case 4: \
	case 5: \
		vol_scale = vol_c; \
		break; \
	default: \
		vol_scale = vol[ch & 1]; \
		break; \
	}


/* 2-channel stereo control */
static void softvol_convert_stereo_vol(softvol_dev_t *svol,
				       const snd_pcm_channel_area_t *dst_areas,
				       snd_pcm_uframes_t dst_offset,
				       const snd_pcm_channel_area_t *src_areas,
				       snd_pcm_uframes_t src_offset,
				       unsigned int channels,
				       snd_pcm_uframes_t frames)
{
	const snd_pcm_channel_area_t *dst_area, *src_area;
	unsigned int src_step, dst_step;
	unsigned int vol_scale, vol[2], vol_c;

	if (svol->cur_vol[0] == 0 && svol->cur_vol[1] == 0) {
		snd_pcm_areas_silence(dst_areas, dst_offset, channels, frames,
				      svol->sformat);
		return;
	} else if (svol->zero_dB_val && svol->cur_vol[0] == svol->zero_dB_val &&
		   svol->cur_vol[1] == svol->zero_dB_val) {
		snd_pcm_areas_copy(dst_areas, dst_offset, src_areas, src_offset,
				   channels, frames, svol->sformat);
		return;
	}

	if (svol->max_val == 1) {
		vol[0] = svol->cur_vol[0] ? 0xffff : 0;
		vol[1] = svol->cur_vol[1] ? 0xffff : 0;
		vol_c = vol[0] | vol[1];
	} else {
		vol[0] = svol->dB_value[svol->cur_vol[0]];
		vol[1] = svol->dB_value[svol->cur_vol[1]];
		vol_c = svol->dB_value[(svol->cur_vol[0] + svol->cur_vol[1]) / 2];
	}
	switch (svol->sformat) {
	case SND_PCM_FORMAT_S16_LE:
	case SND_PCM_FORMAT_S16_BE:
		/* 16bit samples */
		CONVERT_AREA(short, 
			     !snd_pcm_format_cpu_endian(svol->sformat));
		break;
	case SND_PCM_FORMAT_S32_LE:
	case SND_PCM_FORMAT_S32_BE:
		/* 32bit samples */
		CONVERT_AREA(int,
			     !snd_pcm_format_cpu_endian(svol->sformat));
		break;
	case SND_PCM_FORMAT_S24_3LE:
		CONVERT_AREA_S24_3LE();
		break;
	default:
		break;
	}
}

#undef GET_VOL_SCALE
#define GET_VOL_SCALE

/* mono control */
static void softvol_convert_mono_vol(softvol_dev_t *svol,
				     const snd_pcm_channel_area_t *dst_areas,
				     snd_pcm_uframes_t dst_offset,
				     const snd_pcm_channel_area_t *src_areas,
				     snd_pcm_uframes_t src_offset,
				     unsigned int channels,
				     snd_pcm_uframes_t frames)
{
	const snd_pcm_channel_area_t *dst_area, *src_area;
	unsigned int src_step, dst_step;
	unsigned int vol_scale;

	if (svol->cur_vol[0] == 0) {
		snd_pcm_areas_silence(dst_areas, dst_offset, channels, frames,
				      svol->sformat);
		return;
	} else if (svol->zero_dB_val && svol->cur_vol[0] == svol->zero_dB_val) {
		snd_pcm_areas_copy(dst_areas, dst_offset, src_areas, src_offset,
				   channels, frames, svol->sformat);
		return;
	}

	if (svol->max_val == 1)
		vol_scale = svol->cur_vol[0] ? 0xffff : 0;
	else
		vol_scale = svol->dB_value[svol->cur_vol[0]];
	switch (svol->sformat) {
	case SND_PCM_FORMAT_S16_LE:
	case SND_PCM_FORMAT_S16_BE:
		/* 16bit samples */
		CONVERT_AREA(short, 
			     !snd_pcm_format_cpu_endian(svol->sformat));
		break;
	case SND_PCM_FORMAT_S32_LE:
	case SND_PCM_FORMAT_S32_BE:
		/* 32bit samples */
		CONVERT_AREA(int,
			     !snd_pcm_format_cpu_endian(svol->sformat));
		break;
	case SND_PCM_FORMAT_S24_3LE:
		CONVERT_AREA_S24_3LE();
		break;
	default:
		break;
	}
}

#define MAX_CHN	2

int softvol_convert(void *handle, char *dst, int dst_len, char *src, int src_len)
{
	softvol_dev_t *svol = (softvol_dev_t *)handle;
	snd_pcm_channel_area_t src_areas[MAX_CHN];
	snd_pcm_channel_area_t dst_areas[MAX_CHN];
	int i;

	if (!svol)
		return -1;
	
	pthread_mutex_lock(&svol->mutex);
	if (src_len > dst_len)
		src_len = dst_len;

	if (svol->cchannels == 1) {
		dst_areas[0].addr = dst;
		dst_areas[0].first = 0;
		dst_areas[0].step = svol->cchannels * snd_pcm_format_physical_width(svol->sformat);
	
		src_areas[0].addr = src;
		src_areas[0].first = 0;
		src_areas[0].step = svol->cchannels * snd_pcm_format_physical_width(svol->sformat);
		softvol_convert_mono_vol(svol, dst_areas, 0, src_areas, 0, svol->cchannels, src_len * 8 / src_areas[0].step);
	} else {
		for (i = 0; i < svol->cchannels; i++) {
			dst_areas[i].addr = dst;
			dst_areas[i].first = i * snd_pcm_format_physical_width(svol->sformat);
			dst_areas[i].step = svol->cchannels * snd_pcm_format_physical_width(svol->sformat);

			src_areas[i].addr = src;
			src_areas[i].first = i * snd_pcm_format_physical_width(svol->sformat);
			src_areas[i].step = svol->cchannels * snd_pcm_format_physical_width(svol->sformat);
		}
		//printf("srclen %d, dst_len %d, frames %d\n", src_len, dst_len, src_len * 8 / src_areas[0].step);
		softvol_convert_stereo_vol(svol, dst_areas, 0, src_areas, 0, svol->cchannels, src_len * 8 / src_areas[0].step);
	}

	pthread_mutex_unlock(&svol->mutex);
	return src_len;
}

int softvol_set_volume(void *handle, int volume)
{
	softvol_dev_t *svol = (softvol_dev_t *)handle;

	if (!svol)
		return -1;
	
	pthread_mutex_lock(&svol->mutex);
	if (volume < 1)
		volume = 1;
	if (volume > (svol->max_val + 1))
		volume = svol->max_val + 1;

	svol->cur_vol[0] = volume - 1;
	svol->cur_vol[1] = volume - 1;
	pthread_mutex_unlock(&svol->mutex);
	return 0;
}

static unsigned int format_to_alsa(int format)
{ 
	switch (format) { 
		case 32: 
			return SND_PCM_FORMAT_S32_LE; 
		case 16: 
			return SND_PCM_FORMAT_S16_LE;
		default: 
			AUD_ERR("Softvol only support 32 or 16bit\n");
			return -1; 
	} 
}

void *softvol_open(int chn, softvol_cfg_t *cfg)
{
	softvol_dev_t *svol;

	if (chn < 0 || !cfg)
		return NULL;

	svol = (softvol_dev_t *)malloc(sizeof(*svol));
	if (! svol)
		return NULL;
	
	pthread_mutex_init(&svol->mutex, NULL);
	svol->handle = chn;
	svol->sformat = format_to_alsa(cfg->format);
	svol->cchannels = cfg->channels;
	svol->cur_vol[0] = cfg->volume - 1;
	svol->cur_vol[1] = cfg->volume - 1;
	svol->max_val = cfg->resolution - 1;
	svol->min_dB = cfg->min_dB;
	svol->max_dB = cfg->max_dB;
	if (svol->max_val == 1 || svol->max_dB == ZERO_DB)
		svol->zero_dB_val = svol->max_val;
	else if (svol->max_dB < 0)
		svol->zero_dB_val = 0; /* there is no 0 dB setting */
	else
		svol->zero_dB_val = (svol->min_dB / (svol->min_dB - svol->max_dB)) * svol->max_val;
		
	/* set up dB table */
	if (svol->min_dB == PRESET_MIN_DB && svol->max_dB == ZERO_DB && cfg->resolution == PRESET_RESOLUTION)
		svol->dB_value = (unsigned int*)preset_dB_value;
	else {
#if 0
		svol->dB_value = calloc(resolution, sizeof(unsigned int));
		if (! svol->dB_value) {
			AUD_ERR("cannot allocate dB table");
			return NULL;
		}
		svol->min_dB = min_dB;
		svol->max_dB = max_dB;
		for (i = 0; i <= svol->max_val; i++) {
			double db = svol->min_dB + (i * (svol->max_dB - svol->min_dB)) / svol->max_val;
			double v = (pow(10.0, db / 20.0) * (double)(1 << VOL_SCALE_SHIFT));
			svol->dB_value[i] = (unsigned int)v;
		}
		if (svol->zero_dB_val)
			svol->dB_value[svol->zero_dB_val] = 65535;
#else
		AUD_ERR("Cannot handle the given dB range and resolution");
		return NULL;
#endif
	}
	return svol;
}

void softvol_close(void *handle)
{
	softvol_dev_t *svol = (softvol_dev_t *)handle;

	if (!svol)
		return;

	if (svol->dB_value && svol->dB_value != preset_dB_value)
		free(svol->dB_value);
	free(svol);
}
