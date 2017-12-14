#ifndef __AUDIOBOX_H__
#define __AUDIOBOX_H__
#include <qsdk/codecs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLE_SIZE(samplingrate) ((((samplingrate) * 64 / 1000)))
#define min(x, y)   ((x)>(y)?(y):(x))

#define TIMEOUT_BIT (3)
#define TIMEOUT_MASK (0x1<<TIMEOUT_BIT)
#define AUD_TIMEOUT (0x1<<TIMEOUT_BIT)
#define PLAYBACK_TIMEOUT (30 * 60) //30min

#define PRIORITY_BIT (0)
#define PRIORITY_MASK (0x3<<PRIORITY_BIT)

enum channel_priority {
	CHANNEL_BACKGROUND,
	CHANNEL_FOREGROUND,
};

enum audiobox_direct {
	AUDIO_PLAYBACK,
	AUDIO_CAPTURE
};

enum audio_module {
	MODULE_AEC_BIT = 0,
	MODULE_NS_BIT,
	MODULE_AGC_BIT,
};

#define BIT_TO_FLAG(b) (1 << (b))
#define MODULE_DEFINE(f) ((MODULE_##f) (BIT_TO_FLAG(MODULE_##f##_BIT)))

#define MODULE_AEC     BIT_TO_FLAG(MODULE_AEC_BIT)
#define MODULE_NS      BIT_TO_FLAG(MODULE_NS_BIT)
#define MODULE_AGC     BIT_TO_FLAG(MODULE_AGC_BIT)

struct audio_dev_attr {
	int channels;
	int bitwidth;
	int samplingrate;
	int sample_size;
};

struct audio_chn_fmt {
	int channels;
	int bitwidth;
	int samplingrate;
	int sample_size;

	int codec_type;
};
//typedef struct audio_dev_attr audio_dev_attr_t;
typedef struct audio_chn_fmt audio_dev_attr_t;
typedef struct audio_chn_fmt audio_fmt_t;
typedef struct audio_chn_fmt audio_chn_fmt_t;

extern int audio_start_service(void);
extern int audio_stop_service(void);

extern int audio_get_format(const char *dev, audio_fmt_t *dev_attr);
extern int audio_set_format(const char *dev, audio_fmt_t *dev_attr);
/*ignore parameter: int dir*/
//int audio_get_master_volume(const char *channel, int dir)
extern int audio_get_master_volume(const char *dev, ...);
/*ignore parameter: int dir*/
//int audio_set_master_volume(const char *channel, int volume, int dir)
extern int audio_set_master_volume(const char *dev, int volume, ...);
extern int audio_get_dev_module(const char *dev);
extern int audio_set_dev_module(const char *dev, int module);
extern int audio_enable_dev(const char *dev);
extern int audio_disable_dev(const char *dev);
extern int audio_get_channel(const char *dev, audio_fmt_t *chn_fmt, int flag);
extern int audio_get_channel_codec(const char *dev, audio_fmt_t *chn_fmt, int flag);
extern int audio_put_channel(int handle);
extern int audio_get_chn_fmt(int handle, audio_fmt_t *chn_fmt);
extern int audio_set_chn_fmt(int handle, audio_fmt_t *chn_fmt);
extern int audio_get_mute(int handle);
extern int audio_set_mute(int handle, int mute);
extern int audio_get_volume(int handle);
extern int audio_set_volume(int handle, int volume);
extern int audio_read_frame(int handle, char *buf, int size);
extern int audio_write_frame(int handle, char *buf, int size);
extern int audio_get_chn_size(audio_fmt_t *chn_fmt);
extern int audio_get_codec_ratio(audio_fmt_t *chn_fmt);
#ifdef __cplusplus
}
#endif

#endif
