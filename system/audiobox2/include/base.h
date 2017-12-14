#ifndef __BASE_H__
#define __BASE_H__
#include <list.h>
#include <stdio.h>
#include <alsa/asoundlib.h>
#include <fr/libfr.h>
#include <qsdk/vcp7g.h>
#include <audiobox2.h>

enum audio_direct {
	DEVICE_OUT_ALL,
	DEVICE_IN_ALL
};

#define CHN_DATA		32
#define VOLUME_DEFAULT	100
struct audio_save {
	char *devname;
	int volume_p;
	int volume_c;
};
typedef struct audio_save save_t;

struct audio_preproc {
	char name[64];
	char initiated;
	struct vcp_object vcpobj;
	pthread_t serv_id;
	pthread_mutex_t mutex;
	pthread_rwlock_t rwlock;
	pthread_cond_t cond;
	char server_running;
	struct fr_info fr;
	int module;
	int stop_server;
};
typedef struct audio_preproc preproc_t;

#define CHN_SAVE(_devname, _volume_p, _volume_c) \
	    {  .devname = (char *)_devname, .volume_p = _volume_p, .volume_c = _volume_c }

struct audio_dev {
	char devname[32];
	snd_pcm_t *handle;
	snd_ctl_t *ctl;
	audio_dev_attr_t attr;
	pthread_mutex_t mutex;
	/* server for playback or capture */
	int direct;
	int volume_p;
	int volume_c;
	int priority;		//record highest priority in the list
	int enable;		    //dev is enable?
	pthread_t serv_id;
	int stop_server;
	/* phy chn list */
	struct list_head node;
	struct list_head head;
	/* for capture fr buf */
	struct fr_info fr;
	struct fr_buf_info ref;	//when playback, save ref
	/* for aec */
	preproc_t preproc;
#define NORMAL_MODE 0
#define ENTERING_PREPROC_MODE 1
#define PREPROC_MODE 2
#define ENTERING_NORMAL_MODE 3

#define MODULE_DISABLED  0
#define MODULE_ENABLING  1
#define MODULE_ENABLED   2
#define MODULE_DISABLING 3
	int state;
	/* for aec */
};
typedef struct audio_dev * audio_dev_t;

struct audio_chn {
	char chnname[32];
    audio_chn_fmt_t fmt;
	int flag;
	int timeout;
	int priority;
	int direct;
	int volume;
	int in_used;
	int active;
	void *softvol_handle;	// for soft volume
	void *codec_handle;
	int id;
	int	dev_pid;
	int req_stop;
	pthread_mutex_t mutex;
	/* fr circle buf */
	struct fr_info fr;
	struct fr_buf_info ref;	//when playback, save ref
	struct list_head node;
	audio_dev_t dev;
};
typedef struct audio_chn * audio_chn_t;
#endif
