#ifndef __AUDIO_CHN_H__
#define __AUDIO_CHN_H__

enum audio_chn_direct {
	DEVICE_CHN_OUT,
	DEVICE_CHN_IN
};

struct logic_chn {
	//for dev
	char devname[32];
	//for chn
	char chnname[32];
	int handle;
	int direct;
	int frsize;
	struct list_head node;
	struct fr_buf_info fr;
};
typedef struct logic_chn *logic_t;

extern int audio_init_logchn(char *devname, audio_chn_fmt_t *fmt, int handle);
extern int audio_release_logchn(int handle);
extern int audio_get_chnname(int handle, char *name);
extern int audio_get_devname(int handle, char *name);
extern int audio_get_frname(int handle, char *name);
extern int audio_get_fr(int handle, struct fr_buf_info *fr);
extern int audio_get_frsize(int handle);
extern int audio_put_fr(int handle, struct fr_buf_info *fr);
extern int audio_reset_logchn_by_id(int handle, audio_chn_fmt_t *fmt);
#endif
