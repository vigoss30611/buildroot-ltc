#include <fr/libfr.h>
#include <audiobox.h>
#include <stdio.h>
#include <log.h>
#include <alsa/asoundlib.h>
#include <list.h>
#include <base.h>
#include <malloc.h>
#include <qsdk/event.h>
#include <audio_chn.h>
#include <audio_hal.h>
#include <audio_ctl.h>
#include <audio_rpc.h>
#include <audio_service.h>
#include <audiobox_listen.h>
#include <audio_softvol.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/shm.h>

//#include <dsp/lib-tl421.h>

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))

LIST_HEAD(audiobox_chnlist);

/* for save some things to audio channel */
save_t chndata[CHN_DATA] = {
	CHN_SAVE("default", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE("default_mic", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE("btcodec", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE("btcodecmic", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE(NULL, -1, -1),
};

extern int audiobox_cleanup_aec(channel_t chn);
extern void *audio_aec_server(void* arg);

int audiobox_get_direct(char *devname)
{
	char *p;

	if (!devname)
		return -1;
	
	AUD_DBG("audiobox_get_direct: %s\n", devname);
	p = strstr(devname, "mic");
	if (p)
		return DEVICE_IN_ALL;
	else
		return DEVICE_OUT_ALL;
	
}

int audiobox_get_default_format(audio_fmt_t *fmt, char *name)
{
	if (!fmt || !name)
		return -1;
	
	fmt->channels = 2;
	fmt->bitwidth = 24;
	fmt->samplingrate = 48000;
	fmt->sample_size = 1024;

	return 0;
}

int audiobox_get_buffer_size(audio_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;
	
	bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	return (fmt->channels * (bitwidth >> 3) * fmt->sample_size);
}

static uint8_t audiobox_id[AB_ID] = {0, 0, 0, 0};

int audiobox_get_id(void)
{
	int i, j;

	for (i = 0; i < AB_ID; i++)
		for (j = 0; j < 8; j++)
			if (!(audiobox_id[i] & (0x1 << j))) {
				audiobox_id[i] |= (0x1 << j);
				return (i * 8 + j);
			}
	
	AUD_ERR("Do not have enough id for audiobox\n");
	return -1;
}

void audiobox_put_id(int idnum)
{
	if (idnum >= AB_ID * 8) {
		AUD_ERR("Can not release audiobox id: %d\n", idnum);
		return;
	}

	audiobox_id[idnum / 8] &= ~(0x1 << (idnum % 8));
}

audio_t audiobox_get_handle_by_id(int id)
{
	channel_t chn = NULL;
	audio_t dev = NULL;
	struct list_head *pos, *pos1;

	list_for_each(pos, &audiobox_chnlist) {
		chn = list_entry(pos, struct channel_node, node);
		if (!chn) 
			continue;
		list_for_each(pos1, &chn->head) {
			dev = list_entry(pos1, struct audio_dev, node);
			if (!dev)
				continue;
			if (dev->id == id)
				return dev;
		}
	}
	
	return NULL;
}

channel_t audiobox_get_chn_by_id(int id)
{
	channel_t chn = NULL;
	audio_t dev = NULL;
	struct list_head *pos, *pos1;

	list_for_each(pos, &audiobox_chnlist) {
		chn = list_entry(pos, struct channel_node, node);
		if (!chn) 
			continue;
		list_for_each(pos1, &chn->head) {
			dev = list_entry(pos1, struct audio_dev, node);
			if (!dev)
				continue;
			if (dev->id == id)
				return chn;
		}
	}
	
	return NULL;
}

channel_t audiobox_get_chn_by_name(char *devname)
{
	channel_t chn = NULL;
	struct list_head *pos;

	list_for_each(pos, &audiobox_chnlist) {
		chn = list_entry(pos, struct channel_node, node);
		if (!chn) 
			continue;
		if (!strcmp(chn->nodename, devname))
			return chn;
	}
	
	return NULL;
}

channel_t audiobox_get_phychn(char *devname, audio_fmt_t *fmt)
{
	struct list_head *pos;
	channel_t node;
	int ret;
	int i;

	if (!devname)
		return NULL;

	list_for_each(pos, &audiobox_chnlist) {
		node = list_entry(pos, struct channel_node, node);
		if (!node) 
			continue;

		if (!strcmp(node->nodename, devname)) {
			if (fmt && memcmp(fmt, &node->fmt, 12)) {//without compare sample_size;sizeof(audio_fmt_t))) {
				AUD_DBG(
						("Origin:\n"
						 "fmt.bitwidth: %d\n"
						 "fmt.samplingrate: %d\n"
						 "fmt.channels: %d\n"),
						fmt->bitwidth, fmt->samplingrate, fmt->channels);
				AUD_DBG(
						("Setting:\n"
						 "fmt.bitwidth: %d\n"
						 "fmt.samplingrate: %d\n"
						 "fmt.channels: %d\n"),
						node->fmt.bitwidth, node->fmt.samplingrate, node->fmt.channels);

				return NULL;
			}

			return node;
		}
	}

	/* add node to chnlist */
	node = (channel_t)malloc(sizeof(*node));
	if (!node)
		return NULL;

	memset(node, 0, sizeof(*node));
	node->volume_p = VOLUME_DEFAULT;
	node->volume_c = VOLUME_DEFAULT;
	for (i = 0; i < CHN_DATA; i++) {
		if (!chndata[i].devname) {
			AUD_ERR("do not support audio channel %s\n", devname);
			return NULL;
		}

		if (!strcmp(chndata[i].devname, devname)) {
			AUD_ERR("get master volume: p:%d, c:%d\n", chndata[i].volume_p, chndata[i].volume_c);
			node->volume_p = chndata[i].volume_p;
			node->volume_c = chndata[i].volume_c;
			break;
		}
	}

	sprintf(node->nodename, "%s", devname);
	node->priority = CHANNEL_BACKGROUND;
	node->serv_id = -1;
	node->direct = audiobox_get_direct(node->nodename);
	if (node->direct < 0)
		return NULL;
	
	if (fmt)
		memcpy(&node->fmt, fmt, sizeof(audio_fmt_t));
	else
		if (audiobox_get_default_format(&node->fmt, devname) < 0) {
			AUD_ERR("Audio get default format err\n");
			goto err;
		}

	if (audio_hal_open(node) < 0) {
		AUD_ERR("Audio open channel %s err\n", devname);
		goto err;
	}

	if (audio_open_ctl(node) < 0) {
		AUD_ERR("Audio open channel %s err\n", devname);
		goto err1;
	}

	if (node->direct == DEVICE_IN_ALL) {
		AUD_DBG("Audio IN FRname is %s\n", node->nodename);
		ret = fr_alloc(&node->fr, node->nodename, 
				audiobox_get_buffer_size(&node->fmt),
				FR_FLAG_RING(5));
		if (ret < 0) {
			AUD_ERR("Audio fr alloc err: %d\n", ret);
			goto err2;
		}
	}
	
	pthread_mutex_init(&node->mutex, NULL);
	pthread_mutex_init(&node->aec.mutex, NULL);
	pthread_rwlock_init(&node->aec.rwlock, NULL);
	pthread_cond_init(&node->aec.cond, NULL);

	AUD_DBG("create chn server\n");
	INIT_LIST_HEAD(&node->head);
	if (audio_create_chnserv(node) < 0) {
		AUD_ERR("Audio create chnserv err: %s\n", devname);
		goto err3;
	}
	
	list_add_tail(&node->node, &audiobox_chnlist);
	return node;
err3:
	audio_release_chnserv(node);
err2:
	audio_close_ctl(node);
err1:
	audio_hal_close(node);
err:
	free(node);
	return NULL;
}

void audiobox_release_phychn(char *devname)
{
	struct list_head *pos;
	channel_t node;	

	if (!devname)
		return;	

	list_for_each(pos, &audiobox_chnlist) {
		node = list_entry(pos, struct channel_node, node);
		if (!node) 
			continue;

		pthread_mutex_lock(&node->mutex);
		if (!strncmp(node->nodename, devname, strlen(node->nodename))) {
			if (list_empty(&node->head)) {
				list_del(&node->node);

				audio_release_chnserv(node);
				audio_close_ctl(node);
				
				AUD_DBG("AudioBox shutoff phychn:%s\n", node->nodename);	

				if (node->handle)
					audio_hal_close(node);

				if (node->direct == DEVICE_IN_ALL)
					fr_free(&node->fr);

				audiobox_cleanup_aec(node);
				pthread_cond_destroy(&(node->aec.cond));
				pthread_rwlock_destroy(&(node->aec.rwlock));
				pthread_mutex_destroy(&(node->aec.mutex));
			}
		}
		pthread_mutex_unlock(&node->mutex);
	}
	AUD_DBG("AudioBox release phychn\n");	
}

int audiobox_get_channel(struct event_scatter *event)
{
	audio_fmt_t *fmt = NULL;
	softvol_cfg_t cfg;
	audio_t dev;
	char *devname;
	int flag;
	channel_t node;
	int ret;
	
	if (!event)
		return -1;	
	
	devname = AB_GET_CHN(event);
	flag = *(int *)AB_GET_PRIV(event);
	if (AB_GET_RESVLEN(event) == sizeof(audio_fmt_t))
		fmt = (audio_fmt_t *)AB_GET_RESV(event);
	
	AUD_DBG("AudioBox channel name %s, flag %d, fmt %p\n",
			devname, flag, fmt);
	if (fmt)
		AUD_DBG(
			("fmt.bitwidth: %d\n"
			"fmt.samplingrate: %d\n"
			"fmt.channels: %d\n"),
		   	fmt->bitwidth, fmt->samplingrate, fmt->channels);
	dev = malloc(sizeof(*dev));
	if (!dev)
		return -1;

	memset(dev, 0, sizeof(*dev));
	sprintf(dev->devname, "%s", devname);
	dev->flag = flag;
	dev->dev_pid = *(int *)AB_GET_PID(event);
	dev->timeout = (flag & TIMEOUT_MASK)? PLAYBACK_TIMEOUT : 0;
	dev->priority = (flag & PRIORITY_MASK) >> PRIORITY_BIT;
	dev->id = audiobox_get_id();

	pthread_mutex_init(&dev->mutex, NULL);
	fr_INITBUF(&dev->ref, NULL);
	if (dev->id < 0)
		return -1;

reget:
	node = audiobox_get_phychn(dev->devname, fmt);
	if (!node)
		goto err;
	
#if 0
	/* Do not permit 2 CHANNEL_FOREGROUND priotity */
	if (dev->priority == CHANNEL_FOREGROUND &&
			node->priority == CHANNEL_FOREGROUND) {
		AUD_ERR("%s: priority err\n", devname);
		goto err;
	}
#endif
	
	if (pthread_mutex_trylock(&node->mutex)) {
		AUD_DBG("Audio get channel can not lock, try again\n");
		/* give a chance for others to release the lock, ain't we ? */
		usleep(5 * 1000);
		goto reget;
	}
	if (dev->priority > node->priority)
		node->priority = dev->priority;
	
	if (node->direct == DEVICE_OUT_ALL) {
		sprintf(dev->devname, "%s%d", devname, dev->id);
		AUD_DBG("Audio OUT FRname is %s\n", dev->devname);
		ret = fr_alloc(&dev->fr, dev->devname, 
				audiobox_get_buffer_size(&node->fmt),
				FR_FLAG_RING(20) | FR_FLAG_NODROP);
		if (ret < 0) {
			AUD_ERR("Audio fr alloc err: %d\n", ret);
			goto err2;
		}
	}
	// for easy for release
	dev->direct = node->direct;
	/* if playback, we may need softvol */
	//if (dev->direct == DEVICE_OUT_ALL) {
		dev->volume = PRESET_RESOLUTION;
		cfg.format = node->fmt.bitwidth;
		cfg.channels = node->fmt.channels;
		cfg.min_dB = PRESET_MIN_DB;
		cfg.max_dB = ZERO_DB;
		cfg.resolution = PRESET_RESOLUTION;
		dev->handle = softvol_open(dev->id, &cfg);
		if (!dev->handle) {
			AUD_ERR("Audio open Softvol err\n");
			goto err1;
		}
	//}

	list_add_tail(&dev->node, &node->head);
	pthread_mutex_unlock(&node->mutex);
	AUD_ERR("AudioBox %s get channel id %d\n", devname, dev->id);
	return dev->id;

err2:
	softvol_close(dev->handle);
err1:
	pthread_mutex_unlock(&node->mutex);
	audiobox_release_phychn(node->nodename);
err:
	audiobox_put_id(dev->id);
	free(dev);
	return -1;
}

int audiobox_inner_put_channel(audio_t dev)
{
	if (!dev)
		return -1;

	pthread_mutex_lock(&dev->mutex);
	list_del(&dev->node);	

	audiobox_release_phychn(dev->devname);

	if (dev->direct == DEVICE_OUT_ALL)
		fr_free(&dev->fr);

	if ((dev->direct == DEVICE_OUT_ALL || dev->direct == DEVICE_IN_ALL) && dev->handle)
		softvol_close(dev->handle);
	audiobox_put_id(dev->id);

	pthread_mutex_unlock(&dev->mutex);
	free(dev);

	AUD_DBG("AudioBox release Channel OK\n");
	return 0;
}

int audiobox_inner_release_channel(audio_t dev)
{
	int id, handle, ret;

	if (!dev)
		return -1;

	handle = id = dev->id;

	ret = audiobox_inner_put_channel(dev);
	if (ret < 0)
		return -1;

	ret = audio_release_logchn(handle);
	if (ret < 0)
		return -1;

	AUD_DBG("AudioBox inner release Channel OK, id = %d\n", id);
	return 0;
}

int audiobox_put_channel(struct event_scatter *event)
{
	audio_t dev;
	int id;

	if (!event)
		return -1;
	
	id = *(int *)AB_GET_CHN(event);
	AUD_DBG("AudioBox release Channel id %d\n", id);
	dev = audiobox_get_handle_by_id(id);
	if (!dev)
		return -1;
	
	dev->req_stop = 1;

	AUD_DBG("AudioBox release Channel OK\n");
	return 0;
}

int audiobox_get_format(struct event_scatter *event)
{
	channel_t chn = NULL;
	audio_fmt_t *fmt;
	char *devname = NULL;

	if (!event)
		return -1;
	
	devname = AB_GET_CHN(event);	
	fmt = (audio_fmt_t *)AB_GET_PRIV(event);
	if (!fmt)
		return -1;

	chn = audiobox_get_chn_by_name(devname);
	if (!chn)
		return -1;

	memcpy(fmt, &chn->fmt, sizeof(audio_fmt_t));
	return 0;
}

int audiobox_set_format(struct event_scatter *event)
{
	channel_t chn = NULL;
	audio_fmt_t *fmt;
	char *devname = NULL;
	int err;
	struct list_head *pos;
	audio_t dev = NULL;


	if (!event)
		return -1;
	
	devname = AB_GET_CHN(event);	
	fmt = (audio_fmt_t *)AB_GET_PRIV(event);
	if (!fmt)
		return -1;

	chn = audiobox_get_chn_by_name(devname);
	if (!chn)
		return -1;

	if((fmt->channels != chn->fmt.channels) || (fmt->bitwidth != chn->fmt.bitwidth) || (fmt->sample_size != chn->fmt.sample_size) || (fmt->samplingrate != chn->fmt.samplingrate)) {
		if (chn->direct == DEVICE_OUT_ALL) {
			list_for_each(pos, &chn->head) {
				dev = list_entry(pos, struct audio_dev, node);
				pthread_mutex_lock(&dev->mutex);
				pthread_mutex_lock(&chn->mutex);

				//first one
				if(chn->head.next == pos) {
					AUD_DBG("Audio audio_hal_set_format\n");
					err = audio_hal_set_format(chn, fmt);
					if (err < 0) {
						AUD_ERR("audio_hal_set_format: %d\n", err);
						goto err_out;
					}
				}

				if(audiobox_get_buffer_size(fmt) != audiobox_get_buffer_size(&chn->fmt)) {
					AUD_ERR("Audio changing fr, OUT FRname is %s, fr size change from %d to %d\n", dev->devname, audiobox_get_buffer_size(&chn->fmt), audiobox_get_buffer_size(fmt));
					fr_free(&dev->fr);
					fr_INITBUF(&dev->ref, NULL);
					err = fr_alloc(&dev->fr, dev->devname,
							audiobox_get_buffer_size(fmt),
							FR_FLAG_RING(5) | FR_FLAG_NODROP);
					if (err < 0) {
						AUD_ERR("Audio fr alloc err: %d\n", err);
						goto err_out;
					}
				}

				//last one
				if(pos->next == &chn->head) {
					AUD_DBG("Audio set chn->fmt to fmt\n");
					memcpy(&chn->fmt, fmt, sizeof(audio_fmt_t));
				}

				pthread_mutex_unlock(&chn->mutex);
				pthread_mutex_unlock(&dev->mutex);

			}
		} else if (chn->direct == DEVICE_IN_ALL) {
			pthread_mutex_lock(&chn->mutex);
			err = audio_hal_set_format(chn, fmt);
			if (err < 0) {
				goto err_out;
			}
			if(audiobox_get_buffer_size(fmt) != audiobox_get_buffer_size(&chn->fmt)) {
				AUD_DBG("Audio changing fr, IN FRname is %s, fr size change from %d to %d\n", chn->nodename, audiobox_get_buffer_size(&chn->fmt), audiobox_get_buffer_size(fmt));
				pthread_rwlock_wrlock(&chn->aec.rwlock);
				fr_free(&chn->fr);
				list_for_each(pos, &chn->head) {
					dev = list_entry(pos, struct audio_dev, node);
					fr_INITBUF(&dev->ref, NULL);
				}
				err = fr_alloc(&chn->fr, chn->nodename,
						audiobox_get_buffer_size(fmt),
						FR_FLAG_RING(5));
				if (err < 0) {
					AUD_ERR("Audio fr alloc err: %d\n", err);
					goto err_out;
				}
				if(chn->state == AEC_MODE) {
					AUD_DBG("Audio changing fr, IN FRname is %s, fr size change from %d to %d\n", chn->aec.name, audiobox_get_buffer_size(&chn->fmt), audiobox_get_buffer_size(fmt));
					if(chn->aec.fr.fr) {
						fr_free(&chn->aec.fr);
						chn->aec.fr.fr = NULL;
					}
					err = fr_alloc(&chn->aec.fr, chn->aec.name,
							audiobox_get_buffer_size(fmt),
							FR_FLAG_RING(5));
					if (err < 0) {
						AUD_ERR("Audio fr alloc err: %d\n", err);
						goto err_out;
					}
					fr_set_timeout(&chn->aec.fr, 800); //timeout: 800ms
				}
				pthread_rwlock_unlock(&(chn->aec.rwlock));
			}
			memcpy(&chn->fmt, fmt, sizeof(audio_fmt_t));
			pthread_mutex_unlock(&chn->mutex);
		}
	}

	return 0;

err_out:
	if((chn->direct == DEVICE_IN_ALL) && (chn->state == AEC_MODE))
			pthread_rwlock_unlock(&(chn->aec.rwlock));
	audio_hal_set_format(chn, &chn->fmt);
	pthread_mutex_unlock(&chn->mutex);
	if(chn->direct == DEVICE_OUT_ALL && dev)
		pthread_mutex_unlock(&dev->mutex);
	return -1;

}

int audiobox_get_mute(struct event_scatter *event)
{
	channel_t chn = NULL;
	int id;
	int *mute;
	int err;
	char value[16];
	
	if (!event)
		return -1;

	id = *(int *)AB_GET_CHN(event);
	mute = (int *)AB_GET_PRIV(event);

	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	err = audio_route_cget(chn, "numid=3", value);
	if (err < 0)
		return -1;
	
	if (!strcmp(value, "off"))
		*mute = 0;
	else if (!strcmp(value, "on"))
		*mute = 1;

	return 0;
}

int audiobox_set_mute(struct event_scatter *event)
{
	channel_t chn = NULL;
	int id;
	int mute;

	id = *(int *)AB_GET_CHN(event);
	mute = *(int *)AB_GET_PRIV(event);

	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	return audio_route_cset(chn, "numid=3", mute ? "on" : "off");
}

int audiobox_get_volume(struct event_scatter *event)
{
	audio_t dev = NULL;
	int id;
	int *volume;

	id = *(int *)AB_GET_CHN(event);
	volume = (int *)AB_GET_PRIV(event);
	
	dev = audiobox_get_handle_by_id(id);
	if (!dev)
		return -1;
	
	AUD_DBG("get volume %d\n", dev->volume);
	*volume = dev->volume;
	return 0;
}

int audiobox_set_volume(struct event_scatter *event)
{
	audio_t dev = NULL;
	int id;
	int setvol;

	id = *(int *)AB_GET_CHN(event);
	setvol = *(int *)AB_GET_PRIV(event);

	dev = audiobox_get_handle_by_id(id);
	if (!dev)
		return -1;
	
	if (setvol != dev->volume) {
		AUD_DBG("Set soft volume %d\n", setvol);
		dev->volume = setvol;
	}
	return 0;
}

int audiobox_get_master_volume(struct event_scatter *event)
{
	int *volume;
	char *devname = NULL;
	int i;
	int *dir;

	if (!event)
		return -1;
	
	devname = AB_GET_CHN(event);	
	volume = (int *)AB_GET_PRIV(event);
	dir = (int *)AB_GET_RESV(event);

	for (i = 0; i < CHN_DATA; i++) {
		if (!chndata[i].devname) {
			AUD_ERR("Do not support audio channel %s\n", devname);
			return -1;
		}

		if (!strcmp(chndata[i].devname, devname)) {
			if (*dir == DEVICE_OUT_ALL) {
				AUD_DBG("get playback master volume: %d\n", chndata[i].volume_p);
				*volume = chndata[i].volume_p;
				break;
			} else if (*dir == DEVICE_IN_ALL) {
				AUD_DBG("get capture_master volume: %d\n", chndata[i].volume_c);
				*volume = chndata[i].volume_c;
				break;
			}
		}
	}
	return 0;
}

int audiobox_set_master_volume(struct event_scatter *event)
{
	channel_t chn = NULL;
	int volume;
	char *devname = NULL;
	int i;
	int dir;

	if (!event)
		return -1;
	
	devname = AB_GET_CHN(event);	
	volume = *(int *)AB_GET_PRIV(event);
	dir = *(int *)AB_GET_RESV(event);
	AUD_DBG("Set %s master volume %s: %d\n", dir?"capture":"playback", devname, volume);
	
	for (i = 0; i < CHN_DATA; i++) {
		if (!chndata[i].devname) {
			AUD_ERR("do not support audio channel %s\n", devname);
			return -1;
		}

		if (!strcmp(chndata[i].devname, devname)) {
			if (dir == DEVICE_OUT_ALL) {
				AUD_DBG("Set playback master volume: %d\n", volume);
				if (volume > 100)
					volume = 100;
				if (volume < 0)
					volume = 0;
				chndata[i].volume_p = volume;
				break;
			} else if (dir == DEVICE_IN_ALL) {
				AUD_DBG("Set capture master volume: %d\n", volume);
				if (volume > 100)
					volume = 100;
				if (volume < 0)
					volume = 0;
				chndata[i].volume_c = volume;
				break;
			}
		}
	}

	chn = audiobox_get_chn_by_name(devname);
	if (chn) {
		if (dir == DEVICE_OUT_ALL)
			chn->volume_p = volume;
		else if (dir == DEVICE_IN_ALL)
			chn->volume_c = volume;
	}
	return 0;
}

int audiobox_enable_aec(struct event_scatter *event)
{
	int id;
	channel_t chn = NULL;
	int enable_aec;

	struct vcp_object vcpobj;
	audio_fmt_t *fmt;
	int size;
	int err;
    int use_dsp = 1;
	struct timeval now;
	struct timespec timeout;

	if (!event)
		return -1;

	id = *(int *)AB_GET_CHN(event);
	enable_aec = ( (*(int *)AB_GET_PRIV(event)) ? 1 : 0);
	AUD_DBG("%s: enable_ace = %d\n", __func__, enable_aec);

	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	AUD_DBG("%s: chn->state = %d\n", __func__, chn->state);
	if(enable_aec && chn->state == NORMAL_MODE) {

		AUD_DBG("xym-debug: %s: prepare to changing from NORMAL to AEC mode\n", __func__);
		pthread_rwlock_wrlock(&(chn->aec.rwlock));
		if(chn->aec.initiated) {
			AUD_ERR("%s: aec already initiated!\n", __func__);
			goto err_initiated_locked;
		}

		chn->state = ENTERING_AEC_MODE;
		fmt = &(chn->fmt);
		size = audiobox_get_buffer_size(fmt);
		AUD_DBG("%s: alloc %d buffer\n", __func__, size);

		snprintf(chn->aec.name, sizeof(chn->aec.name), "%s_aec", chn->nodename);
		err = fr_alloc(&chn->aec.fr, chn->aec.name,
				size,
				FR_FLAG_RING(5));
		if (err < 0) {
			AUD_ERR("Audio fr alloc err: %d\n", err);
			goto err_fr_alloc_locked;
		}
		AUD_DBG("%s: fr_alloc success! frname = %s\n", __func__, chn->aec.fr.name);
		fr_set_timeout(&chn->aec.fr, 800); //timeout: 800ms

		err = vcp7g_init(&vcpobj, fmt->channels, fmt->samplingrate, fmt->bitwidth, &use_dsp);
		if (err < 0) {
			AUD_ERR("%s: vcp7g_init failed, use_dsp = %d\n", __func__, use_dsp);
			goto err_vcp_init_locked;
		}
		AUD_DBG("%s: vcp7g_init success, use_dsp = %d\n", __func__, use_dsp);
		memcpy(&(chn->aec.vcpobj), &vcpobj, sizeof(struct vcp_object));

        audiobox_aec_inner_start();
#if defined(AUDIOBOX_USE_AECV1)
		//aec status: L1AL=disable, R1AR=disable, DLAL=enable, L1AR=enable
		//err = audio_route_cset(chn, "name='Mic L1AL Enable Switch'", "on"); //not working
		err = audio_route_cset(chn, "numid=1", "off");
		//err |= audio_route_cset(chn, "name='Mic L2AL Enable Switch'", "on");//not working
		err |= audio_route_cset(chn, "numid=2", "off");

		//err |= audio_route_cset(chn, "name='Mic R1AR Enable Switch'", "on");//not working
		err |= audio_route_cset(chn, "numid=5", "off");
		//err |= audio_route_cset(chn, "name='Mic R2AR Enable Switch'", "on");//not working
		err |= audio_route_cset(chn, "numid=6", "off");

		//err |= audio_route_cset(chn, "name='Mic L1AR Enable Switch'", "off");//not working
		err |= audio_route_cset(chn, "numid=3", "on");
		//err |= audio_route_cset(chn, "name='Mic L2AR Enable Switch'", "off");//not working
		err |= audio_route_cset(chn, "numid=4", "on");

		//err |= audio_route_cset(chn, "name='Mic DLAL Enable Switch'", "off");//not working
		err |= audio_route_cset(chn, "numid=7", "on");
		if(err) {
			AUD_ERR("AudioBox AEC route cset err\n");
			goto err_route_cset_locked;
		}
#endif
		chn->aec.initiated = 1;
		pthread_rwlock_unlock(&(chn->aec.rwlock));

		//create thread
		pthread_mutex_lock(&(chn->aec.mutex));
		if(chn->aec.server_running) {
			AUD_ERR("%s: audio_aec_server already exist! will not create new one\n", __func__);
			goto thread_exist;
		}

		err = pthread_create(&chn->aec.serv_id, NULL, audio_aec_server, chn);
		if (err != 0 || chn->aec.serv_id <= 0) {
			AUD_ERR("Create AudioBox AEC server err\n");
			goto err_pthread_create_locked;
		}
		pthread_detach(chn->aec.serv_id);

thread_exist:
		//timeout = 1s
		gettimeofday(&now, NULL);
		timeout.tv_sec = now.tv_sec + 1;
		timeout.tv_nsec = (now.tv_usec * 1000);
		do {
			err = pthread_cond_timedwait(&chn->aec.cond, &chn->aec.mutex, &timeout);
		} while(!chn->aec.server_running && err == 0);  // if timeout or server running, break out

		if(!chn->aec.server_running) {
			AUD_ERR("waiting for server starting timeout!\n");
			goto err_pthread_create_locked;
		}

		pthread_mutex_unlock(&(chn->aec.mutex));

		chn->state = AEC_MODE;
		AUD_DBG("xym-debug: %s: change from NORMAL to AEC mode success\n", __func__);

	}
	else if(!enable_aec && chn->state == AEC_MODE) {

		AUD_DBG("xym-debug: %s: prepare to changing from AEC to NORMAL mode\n", __func__);
		chn->state = ENTERING_NORMAL_MODE;

		pthread_rwlock_wrlock(&(chn->aec.rwlock));
		if(!chn->aec.initiated) { //aec isn't initiated
			pthread_rwlock_unlock(&(chn->aec.rwlock));
			goto exit_thread;
		}


		AUD_DBG("xym-debug: %s: releasing fr\n", __func__);
		if(chn->aec.fr.fr) {
			fr_free(&chn->aec.fr);
			chn->aec.fr.fr = NULL;
		}

		AUD_DBG("xym-debug: %s: releasing vcp\n", __func__);
		if(vcp7g_is_init(&chn->aec.vcpobj)) {
			vcp7g_deinit(&(chn->aec.vcpobj)); //it worth to deinit again even it have been done before
			memset(&(chn->aec.vcpobj), 0, sizeof(struct vcp_object));
		}

        audiobox_aec_inner_stop();
#if defined(AUDIOBOX_USE_AECV1)
		//normal status: L1AL=enable, R1AR=enable, DLAL=disable, L1AR=disable
		//err = audio_route_cset(chn, "name='Mic L1AL Enable Switch'", "on"); //not working
		err = audio_route_cset(chn, "numid=1", "on");
		//err |= audio_route_cset(chn, "name='Mic L2AL Enable Switch'", "on");//not working
		err |= audio_route_cset(chn, "numid=2", "on");

		//err |= audio_route_cset(chn, "name='Mic R1AR Enable Switch'", "on");//not working
		err |= audio_route_cset(chn, "numid=5", "on");
		//err |= audio_route_cset(chn, "name='Mic R2AR Enable Switch'", "on");//not working
		err |= audio_route_cset(chn, "numid=6", "on");

		//err |= audio_route_cset(chn, "name='Mic L1AR Enable Switch'", "off");//not working
		err |= audio_route_cset(chn, "numid=3", "off");
		//err |= audio_route_cset(chn, "name='Mic L2AR Enable Switch'", "off");//not working
		err |= audio_route_cset(chn, "numid=4", "off");

		//err |= audio_route_cset(chn, "name='Mic DLAL Enable Switch'", "off");//not working
		err |= audio_route_cset(chn, "numid=7", "off");
		if(err)
			AUD_ERR("back from AEC mode to NORMAL mode fail!\n"); //wrong again? ALSA or snd_card may broken down
#endif
		chn->aec.initiated = 0;
		pthread_rwlock_unlock(&(chn->aec.rwlock));

exit_thread:
		pthread_mutex_lock(&(chn->aec.mutex));

		AUD_DBG("xym-debug: %s: checking server_running\n", __func__);
		if(chn->aec.server_running) {
			AUD_DBG("xym-debug: %s: server_running is running\n", __func__);
			//timeout = 1s
			gettimeofday(&now, NULL);
			timeout.tv_sec = now.tv_sec + 1;
			timeout.tv_nsec = (now.tv_usec * 1000);
			do {
				err = pthread_cond_timedwait(&chn->aec.cond, &chn->aec.mutex, &timeout);
			} while(chn->aec.server_running && err == 0);  // if timeout or server not running, break out

			if(chn->aec.server_running) {
				AUD_ERR("waiting for server stoping timeout!\n");
			}
		}
		pthread_mutex_unlock(&(chn->aec.mutex));

		chn->state = NORMAL_MODE; //back to NORMAL MODE
		AUD_DBG("xym-debug: %s: change from AEC to NORMAL mode done, server_running = %d\n", __func__, chn->aec.server_running);
	}

	return 0;

err_pthread_create_locked:
	pthread_mutex_unlock(&(chn->aec.mutex));
	pthread_rwlock_wrlock(&(chn->aec.rwlock));
#if defined(AUDIOBOX_USE_AECV1)
	//normal status: L1AL=enable, R1AR=enable, DLAL=disable, L1AR=disable
	//err = audio_route_cset(chn, "name='Mic L1AL Enable Switch'", "on"); //not working
	err = audio_route_cset(chn, "numid=1", "on");
	//err |= audio_route_cset(chn, "name='Mic L2AL Enable Switch'", "on");//not working
	err |= audio_route_cset(chn, "numid=2", "on");

	//err |= audio_route_cset(chn, "name='Mic R1AR Enable Switch'", "on");//not working
	err |= audio_route_cset(chn, "numid=5", "on");
	//err |= audio_route_cset(chn, "name='Mic R2AR Enable Switch'", "on");//not working
	err |= audio_route_cset(chn, "numid=6", "on");

	//err |= audio_route_cset(chn, "name='Mic L1AR Enable Switch'", "off");//not working
	err |= audio_route_cset(chn, "numid=3", "off");
	//err |= audio_route_cset(chn, "name='Mic L2AR Enable Switch'", "off");//not working
	err |= audio_route_cset(chn, "numid=4", "off");

	//err |= audio_route_cset(chn, "name='Mic DLAL Enable Switch'", "off");//not working
	err |= audio_route_cset(chn, "numid=7", "off");
	if(err)
		AUD_ERR("back from AEC mode to NORMAL mode fail!\n"); //wrong again? ALSA or snd_card may broken down

err_route_cset_locked:
#endif
    audiobox_aec_inner_stop();
	if(vcp7g_is_init(&chn->aec.vcpobj)) {
		vcp7g_deinit(&(chn->aec.vcpobj)); //it worth to deinit again even it have been done before
		memset(&(chn->aec.vcpobj), 0, sizeof(struct vcp_object));
	}

err_vcp_init_locked:
	if(chn->aec.fr.fr) {
		fr_free(&chn->aec.fr);
		chn->aec.fr.fr = NULL;
	}

err_fr_alloc_locked:
err_initiated_locked:
	pthread_rwlock_unlock(&(chn->aec.rwlock));
	chn->state = NORMAL_MODE; //back to NORMAL MODE
	AUD_DBG("xym-debug: %s: back to NORMAL mode done, server_running = %d\n", __func__, chn->aec.server_running);
	return -1;

}

int audiobox_stop_service(struct event_scatter *event)
{
	channel_t chn = NULL;
	audio_t dev = NULL;
	struct list_head *pos, *pos1;

	list_for_each(pos, &audiobox_chnlist) {
		chn = list_entry(pos, struct channel_node, node);
		if (!chn) 
			continue;
		list_for_each(pos1, &chn->head) {
			dev = list_entry(pos1, struct audio_dev, node);
			if (!dev)
				continue;
			
			pthread_mutex_lock(&dev->mutex);
			list_del(&dev->node);
			audiobox_release_phychn(dev->devname);
			if (dev->direct == DEVICE_OUT_ALL)
				fr_free(&dev->fr);
			audiobox_put_id(dev->id);
			free(dev);
			pthread_mutex_unlock(&dev->mutex);
		}
	}
	audiobox_signal_handler(0);
	return 0;
}

void audiobox_listener(char *name, void *arg)
{
	struct event_scatter audiobox[AB_COUNT];
	int cmd;
	int *result;

	if (!name || !arg)
		return;

	AUD_DBG("audiobox_listener: %s(%d)\n", name, *(int *)arg);
	if (strncmp(AUDIOBOX_RPC_BASE, name, strlen(AUDIOBOX_RPC_BASE)))
		return;
	
	memset(audiobox, 0, AB_COUNT * sizeof(struct event_scatter));
	if (audiobox_rpc_parse(audiobox, AB_COUNT, (char *)arg) < 0) {
		AUD_ERR("audiobox rpc parse err\n");
		return;
	}

	result = (int *)AB_GET_RESULT(audiobox);
	cmd = *(int *)AB_GET_CMD(audiobox);
	switch (cmd) {
		case AB_START_SERVICE:
			*result = 0;
			break;
		case AB_STOP_SERVICE:
			*result = audiobox_stop_service(audiobox);
			break;
		case AB_GET_CHANNEL:
			*result = audiobox_get_channel(audiobox);
			break;
		case AB_PUT_CHANNEL:
			*result = audiobox_put_channel(audiobox);
			break;
		case AB_GET_FORMAT:
			*result = audiobox_get_format(audiobox);
			break;
		case AB_SET_FORMAT:
			*result = audiobox_set_format(audiobox);
			break;
		case AB_GET_MUTE:
			*result = audiobox_get_mute(audiobox);
			break;
		case AB_SET_MUTE:
			*result = audiobox_set_mute(audiobox);
			break;
		case AB_GET_VOLUME:
			*result = audiobox_get_volume(audiobox);
			break;
		case AB_SET_VOLUME:
			*result = audiobox_set_volume(audiobox);
			break;
		case AB_GET_MASTER_VOLUME:
			*result = audiobox_get_master_volume(audiobox);
			break;
		case AB_SET_MASTER_VOLUME:
			*result = audiobox_set_master_volume(audiobox);
			break;
		case AB_ENABLE_AEC:
			*result = audiobox_enable_aec(audiobox);
			break;
		default:
			AUD_ERR("Do not support this cmd: %d\n", cmd);
			*result = -1;
			break;
	}
	AUD_DBG("Audiobox_Listener result %d\n", *result);
}

static int audiobox_put_channels_by_pid(int dev_pid)
{
	channel_t chn = NULL;
	audio_t dev = NULL;
	struct list_head *pos, *pos1;

	list_for_each(pos, &audiobox_chnlist) {
		chn = list_entry(pos, struct channel_node, node);
		if (!chn) 
			continue;
		list_for_each(pos1, &chn->head) {
			dev = list_entry(pos1, struct audio_dev, node);
			if (!dev)
				continue;
			if (dev->dev_pid == dev_pid){
				dev->req_stop = 1;
				AUD_ERR("Put LogicChan:%d(on phychn:%s) by EVENT_PROCESS_END(pid:%d)\n", dev->id, chn->nodename, dev->dev_pid);
			}
		}
	}

	return 0;
}

void audiobox_listener_on_process_end(char *name, void *arg)
{
	struct event_process* p_event;

	if (!name || !arg)
		return;

	AUD_DBG("audiobox receive: %s(pid:%d)\n", name, *(int *)arg);
	if (strncmp(EVENT_PROCESS_END, name, strlen(EVENT_PROCESS_END)))
		return;

	p_event = (struct event_process*)arg;

	audiobox_put_channels_by_pid(p_event->pid);
}

int audiobox_get_devinfo(audio_devinfo_t* devinfo)
{
	channel_t chn = NULL;
	audio_t dev = NULL;
	audio_ext_fmt_t  real_fmt;
	struct list_head *pos, *pos1;
	int		dev_cnt;
	int		chan_cnt;
	char	amute[16] = "";
	int		err;

	dev_cnt = 0;
	list_for_each(pos, &audiobox_chnlist) {
		chn = list_entry(pos, struct channel_node, node);
		if (!chn)
			continue;

		devinfo->active = 1;
		audio_hal_get_format(chn, &real_fmt);
		memcpy(devinfo->nodename, chn->nodename, min(sizeof(devinfo->nodename), sizeof(chn->nodename)));
		memcpy(&devinfo->fmt, &real_fmt, sizeof(devinfo->fmt));
		devinfo->direct = chn->direct;
		devinfo->master_volume = (chn->direct == DEVICE_IN_ALL)? chn->volume_c:chn->volume_p;
		devinfo->stop_server = chn->direct;
		devinfo->ena_aec = chn->aec.initiated;
		devinfo->xruncnt = 0;

		chan_cnt = 0;
		list_for_each(pos1, &chn->head) {
			dev = list_entry(pos1, struct audio_dev, node);
			if (!dev)
				continue;
			chan_cnt++;
		}
		devinfo->chan_cnt = chan_cnt;

		devinfo->mute = 0;
		err = audio_route_cget(chn, "numid=3", amute);
		if(err < 0){
			devinfo->mute = 0;
		}else{
			if (!strcmp(amute, "off"))
				devinfo->mute = 0;
			else if (!strcmp(amute, "on"))
				devinfo->mute = 1;
		}

		devinfo++;
		dev_cnt++;
		if(dev_cnt >= MAX_AUDIO_DEV_NUM)
			break;
	}

	return dev_cnt;
}

int audiobox_get_chaninfo(audio_chaninfo_t* chaninfo)
{
	channel_t chn = NULL;
	audio_t dev = NULL;
	struct list_head *pos, *pos1;
	int		chan_cnt = 0;

	list_for_each(pos, &audiobox_chnlist) {
		chn = list_entry(pos, struct channel_node, node);
		if (!chn)
			continue;

		list_for_each(pos1, &chn->head) {
			dev = list_entry(pos1, struct audio_dev, node);
			if (!dev)
				continue;

			chaninfo->active = 1;
			memcpy(chaninfo->devname, chn->nodename, min(sizeof(chaninfo->devname), sizeof(chn->nodename)));
			chaninfo->priority = dev->priority;
			chaninfo->direct = dev->direct;
			chaninfo->volume = dev->volume;;
			chaninfo->chan_id = dev->id;
			chaninfo->chan_pid = dev->dev_pid;
			chaninfo->req_stop = dev->req_stop;
			chaninfo++;
			chan_cnt++;
			if(chan_cnt >= MAX_AUDIO_CHAN_NUM)
				return chan_cnt;
		}
	}

	return chan_cnt;
}

int audiobox_get_debug_info(const char *pArg)
{
	ST_SHM_INFO stShmInfo;
	int s32ShmID;
	char *pMapMem = NULL;

	if (!pArg) {
		AUD_DBG("argument invalid!\n");
		return -1;
	}

	memcpy((char *)&stShmInfo, pArg, sizeof(stShmInfo));

	s32ShmID = shmget(stShmInfo.s32Key, 0, 0666);
	if ((int)s32ShmID == -1) {
		AUD_DBG("(%s:L%d) shmget is error (key=0x%x) errno:%s!!!\n", __func__, __LINE__,
			stShmInfo.s32Key, strerror(errno)) ;
		return -1;
	}

	pMapMem = (char*)shmat(s32ShmID, NULL, 0);
	if ((int)pMapMem == -1) {
		AUD_DBG("(%s:L%d) pbyMap is NULL (iShmId=%d) errno:%s!!!\n", __func__, __LINE__,
			s32ShmID, strerror(errno));
		return -1;
	}

	if(strncmp(stShmInfo.ps8SubType, PARSE_OBJ_AUDIO_DEV, strlen(PARSE_OBJ_AUDIO_DEV)) == 0)
		audiobox_get_devinfo((audio_devinfo_t*)pMapMem);
	else if(strncmp(stShmInfo.ps8SubType, PARSE_OBJ_AUDIO_CHAN, strlen(PARSE_OBJ_AUDIO_CHAN)) == 0)
		audiobox_get_chaninfo((audio_chaninfo_t*)pMapMem);

	if (shmdt(pMapMem) == -1) {
		AUD_DBG("(%s:L%d) shmdt is error (pbyMap=%p) errno:%s!!!\n", __func__, __LINE__,
			pMapMem, strerror(errno));
		return -1;
	}

	return 0;
}

void audiobox_listener_dbginfo(char *name, void *arg)
{
	if (!name || !arg)
		return;

	if (strncmp(AB_RPC_DEBUG_INFO, name, strlen(AB_RPC_DEBUG_INFO)))
		return;

	audiobox_get_debug_info(arg);
}

/* rpc transfer for same process but different thread */
static char eventrpc[AUDIO_RPC_MAX][32] = {
	AUDIOBOX_RPC_BASE,
	AUDIOBOX_RPC_DEFAULT,
	AUDIOBOX_RPC_DEFAULTMIC,
	AUDIOBOX_RPC_BTCODEC,
	AUDIOBOX_RPC_BTCODECMIC,
	AUDIOBOX_RPC_END,		// channel end flags
};

int audiobox_listener_init(void)
{
	int err;
	int i;

	/* set audiobox listener */
	for (i = 0; i < AUDIO_RPC_MAX; i++) {
		if (!strcmp(AUDIOBOX_RPC_END, eventrpc[i]))
			break;

		err = event_register_handler(eventrpc[i], EVENT_RPC, audiobox_listener);
		if (err < 0) {
			AUD_ERR("Register %s err\n", eventrpc[i]);
			return -1;
		}
	}
	err = event_register_handler(EVENT_PROCESS_END, EVENT_RPC, audiobox_listener_on_process_end);
	if (err < 0) {
		AUD_ERR("Audiobox Register EVENT_PROCESS_END err\n");
		return -1;
	}

	err = event_register_handler(AB_RPC_DEBUG_INFO, EVENT_RPC, audiobox_listener_dbginfo);
	if (err < 0) {
		AUD_ERR("Audiobox Register EVENT_PROCESS_END err\n");
		return -1;
	}

	return 0;
}

int audiobox_listener_deinit(void)
{
	int err;
	int i;

	for (i = 0; i < AUDIO_RPC_MAX; i++) {
		if (!strcmp(AUDIOBOX_RPC_END, eventrpc[i]))
			break;

		err = event_unregister_handler(eventrpc[i], audiobox_listener);
		if (err < 0) {
			AUD_ERR("Unregister %s err\n", eventrpc[i]);
			return -1;
		}
	}
	return 0;
}
