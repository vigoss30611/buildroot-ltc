#ifndef __AUDIO_SOFTVOL_H__
#define __AUDIO_SOFTVOL_H__

#include <alsa/asoundlib.h>

#define PRESET_RESOLUTION	256
#define PRESET_MIN_DB		-51.0
#define ZERO_DB                  0.0

typedef struct {
	int format;
	unsigned int channels;
	unsigned int volume;
	double min_dB;
	double max_dB;
	int resolution;
} softvol_cfg_t;

typedef struct {
	/* This field need to be the first */
	int handle;
	snd_pcm_format_t sformat;
	unsigned int cchannels;
	unsigned int cur_vol[2];
	unsigned int max_val;     /* max index */
	unsigned int zero_dB_val; /* index at 0 dB */
	double min_dB;
	double max_dB;
	unsigned int *dB_value;
	pthread_mutex_t mutex;
} softvol_dev_t;

extern void *softvol_open(int chn, softvol_cfg_t *cfg);
extern void softvol_close(void *handle);
extern int softvol_convert(void *handle, char *dst, int dst_len, char *src, int src_len);
extern int softvol_set_volume(void *handle, int volume);

#endif
