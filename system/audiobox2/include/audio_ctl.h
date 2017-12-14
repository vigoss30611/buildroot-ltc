#ifndef __AUDIO_CTL_H__
#define __AUDIO_CTL_H__

extern int audio_route_cget(audio_dev_t dev, char *route, char *value);
extern int audio_route_cset(audio_dev_t dev, char *route, char *value);
extern int audio_open_ctl(audio_dev_t dev);
extern void audio_close_ctl(audio_dev_t dev);
#endif
