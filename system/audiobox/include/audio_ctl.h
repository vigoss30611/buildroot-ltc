#ifndef __AUDIO_CTL_H__
#define __AUDIO_CTL_H__

extern int audio_route_cget(channel_t dev, char *route, char *value);
extern int audio_route_cset(channel_t dev, char *route, char *value);
extern int audio_open_ctl(channel_t dev);
extern void audio_close_ctl(channel_t dev);
#endif
