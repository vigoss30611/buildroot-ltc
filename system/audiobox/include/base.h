#ifndef __BASE_H__
#define __BASE_H__
#include <list.h>
#include <stdio.h>
#include <alsa/asoundlib.h>
#include <fr/libfr.h>
#include <qsdk/vcp7g.h>
#include <audiobox.h>

struct audio_dev {
	char devname[32];
	int flag;
	int timeout;
	int priority;
	int direct;
	int volume;
	void *handle;	// for soft volume
	int id;
	int	dev_pid;
	int req_stop;
	pthread_mutex_t mutex;
	/* fr circle buf */
	struct fr_info fr;
	struct fr_buf_info ref;	//when playback, save ref
	struct list_head node;
};
typedef struct audio_dev * audio_t;

#define CHN_DATA		32
#define VOLUME_DEFAULT	100
struct audio_save {
	char *devname;
	int volume_p;
	int volume_c;
};
typedef struct audio_save save_t;

struct audio_aec {
	char name[64];
	char initiated;
	struct vcp_object vcpobj;
	pthread_t serv_id;
	pthread_mutex_t mutex;
	pthread_rwlock_t rwlock;
	pthread_cond_t cond;
	char server_running;
	struct fr_info fr;
};
typedef struct audio_aec aec_t;

#define CHN_SAVE(_devname, _volume_p, _volume_c) \
	    {  .devname = (char *)_devname, .volume_p = _volume_p, .volume_c = _volume_c }

struct channel_node {
	char nodename[32];
	int priority;		//record highest priority in the list
	snd_pcm_t *handle;
	snd_ctl_t *ctl;
	audio_fmt_t fmt;
	pthread_mutex_t mutex;
	int volume_p;
	int volume_c;
	/* server for playback or capture */
	int direct;
	pthread_t serv_id;
	int stop_server;
	/* phy chn list */
	struct list_head node;
	struct list_head head;
	/* for capture fr buf */
	struct fr_info fr;
	/* for aec */
	aec_t aec;
#define NORMAL_MODE 0
#define ENTERING_AEC_MODE 1
#define AEC_MODE 2
#define ENTERING_NORMAL_MODE 3
	int state;
	int xruncnt;
	/* for aec */
};
typedef struct channel_node * channel_t;
#endif
