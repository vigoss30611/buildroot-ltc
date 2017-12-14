#ifndef __AUDIO_HAL_H__
#define __AUDIO_HAL_H__

struct audio_threshold {
	int sample_rate;
	int bitwidth;
	int channels;
	int value;
};
typedef struct audio_threshold threshold_t;

extern int audio_hal_open(channel_t handle);
extern int audio_hal_close(channel_t handle);
extern int audio_hal_read(channel_t handle,  void *buf, int len);
extern int audio_hal_write(channel_t handle,  void *buf, int len);
extern int audio_hal_set_format(channel_t handle, audio_fmt_t *fmt);
extern int audio_hal_get_format(channel_t handle, audio_ext_fmt_t *dev_fmt);
extern uint64_t audio_frame_times(channel_t handle, int length);

#endif
