#ifndef __AUDIO_HAL_H__
#define __AUDIO_HAL_H__

struct audio_threshold {
	int sample_rate;
	int bitwidth;
	int channels;
	int value;
};
typedef struct audio_threshold threshold_t;

extern int audio_hal_open(audio_dev_t dev);
extern int audio_hal_close(audio_dev_t dev);
extern int audio_hal_read(audio_dev_t dev,  void *buf, int len);
extern int audio_hal_write(audio_dev_t dev,  void *buf, int len);
extern int audio_hal_set_format(audio_dev_t dev, audio_dev_attr_t *fmt);
extern uint64_t audio_frame_times(audio_dev_t dev, int length);

#endif
