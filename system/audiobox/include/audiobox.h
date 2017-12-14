#ifndef __AUDIOBOX_H__
#define __AUDIOBOX_H__

#include <qsdk/event.h>

#ifdef __cplusplus
extern "C" {
#endif

#define min(x, y)   ((x)>(y)?(y):(x))

#define TIMEOUT_BIT (3)
#define TIMEOUT_MASK (0x1<<TIMEOUT_BIT)
#define AUD_TIMEOUT (0x1<<TIMEOUT_BIT)
#define PLAYBACK_TIMEOUT (30 * 60) //30min

#define PRIORITY_BIT (0)
#define PRIORITY_MASK (0x3<<PRIORITY_BIT)

#define AB_RPC_DEBUG_INFO 		"audiobox_debug_info"

#define PARSE_OBJ_AUDIO_DEV		"audio_dev"
#define PARSE_OBJ_AUDIO_CHAN	"audio_chan"
#define MAX_AUDIO_DEV_NUM		4
#define MAX_AUDIO_CHAN_NUM		32

enum audio_direct {
	DEVICE_OUT_ALL,
	DEVICE_IN_ALL
};

enum channel_priority {
	CHANNEL_BACKGROUND,
	CHANNEL_FOREGROUND,
};

enum audiobox_direct {
	AUDIO_PLAYBACK,
	AUDIO_CAPTURE
};

typedef struct audio_format {
	int channels;
	int bitwidth;
	int samplingrate;
	int sample_size;
}audio_fmt_t;

/*audio format info, read from snd_pcm hardware*/
typedef struct audio_ext_format {
	unsigned int channels;
	unsigned int bitwidth;
	unsigned int samplingrate;
	unsigned int sample_size;
	unsigned int access;
	unsigned int pcm_max_frm;
}audio_ext_fmt_t;

typedef struct audio_devinfo{
	int active;
	char nodename[32];
	audio_ext_fmt_t fmt;
	int master_volume;
	int direct;
	int stop_server;
	int ena_aec;
	int mute;
	int chan_cnt;
	int xruncnt;
}audio_devinfo_t;

typedef struct audio_chaninfo{
	int active;
	char devname[32];
	int priority;
	int direct;
	int volume;
	int chan_id;
	int	chan_pid;
	int req_stop;
}audio_chaninfo_t;

typedef struct _audio_dbg_info {
	audio_devinfo_t	audio_devinfo[MAX_AUDIO_DEV_NUM];
	audio_chaninfo_t	audio_chaninfo[MAX_AUDIO_CHAN_NUM];
}audio_dbg_info_t;

extern int audio_start_service(void);
extern int audio_stop_service(void);

extern int audio_get_channel(const char *channel, audio_fmt_t *fmt, int priority);
extern int audio_put_channel(int handle);
extern int audio_test_frame(int handle, struct fr_buf_info *buf);
extern int audio_get_frame(int handle, struct fr_buf_info *buf);
extern int audio_put_frame(int handle, struct fr_buf_info *buf);
extern int audio_read_frame(int handle , char *buf, int size);
extern int audio_write_frame(int handle, char *buf, int size);
extern int audio_get_format(const char *channel, audio_fmt_t *fmt);
extern int audio_set_format(const char *channel, audio_fmt_t *fmt);
extern int audio_get_mute(int handle);
extern int audio_set_mute(int handle, int mute);
extern int audio_get_volume(int handle);
extern int audio_set_volume(int handle, int volume);
extern int audio_get_master_volume(const char *channel, int dir);
extern int audio_set_master_volume(const char *channel, int volume, int dir);
extern int audio_enable_aec(int handle, int enable);
#ifdef __cplusplus
}
#endif

#endif
