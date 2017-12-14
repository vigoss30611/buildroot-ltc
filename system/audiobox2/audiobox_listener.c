#include <fr/libfr.h>
#include <audiobox2.h>
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
//#include <dsp/lib-tl421.h>

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
//#define SAMPLE_SIZE(samplingrate) ((((samplingrate) >> 3)))

LIST_HEAD(audiobox_devlist);
pthread_mutex_t devlist_mutex;

/* for save some things to audio channel */
save_t chndata[CHN_DATA] = {
	CHN_SAVE("default", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE("default_mic", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE("btcodec", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE("btcodecmic", VOLUME_DEFAULT, VOLUME_DEFAULT),
	CHN_SAVE(NULL, -1, -1),
};

//extern int audiobox_cleanup_aec(auido_dev_t dev);
extern void *audio_preproc_server(void* arg);
extern int audio_hal_set_params(audio_dev_t dev, audio_dev_attr_t *attr);
int _audiobox_enable_dev(char * dev_name);
int audiobox_disable_preproc_module(audio_dev_t dev);
int audiobox_enable_preproc_module(audio_dev_t dev, int module);
void audiobox_delete_dev(char *devname);

int	audiobox_get_sample_size(int samplingrate)
{
	if(samplingrate == 11025)
		samplingrate = 16000;
	else if(samplingrate == 22050)
		samplingrate = 32000;
	else if(samplingrate == 44100)
		samplingrate = 48000;

	return SAMPLE_SIZE(samplingrate);
}

int audiobox_get_direct(char *dev_name)
{
	char *p;

	if (!dev_name)
		return -1;
	
	AUD_DBG("audiobox_get_direct: %s\n", dev_name);
	p = strstr(dev_name, "mic");
	if (p)
		return DEVICE_IN_ALL;
	else
		return DEVICE_OUT_ALL;
	
}

int audiobox_get_default_dev_attr(audio_dev_attr_t *attr, char *name)
{
	if (!attr || !name)
		return -1;
	
	attr->channels = 2;
	attr->bitwidth = 32;
	attr->samplingrate = 16000;
	attr->sample_size = audiobox_get_sample_size(attr->samplingrate);

	return 0;
}

// get dev frame size for a 1s/8(125ms) audio data
int audiobox_get_dev_frame_size(audio_dev_attr_t *attr)
{
	int bitwidth;

	if (!attr)
		return -1;

	bitwidth = ((attr->bitwidth > 16) ? 32 : attr->bitwidth);
	return (attr->channels * (bitwidth >> 3) * (audiobox_get_sample_size(attr->samplingrate)));
}

// get chn frame size for a 1s/8(125ms) audio data
int audiobox_get_chn_frame_size(audio_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;

	bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	return (fmt->channels * (bitwidth >> 3) * (audiobox_get_sample_size(fmt->samplingrate)));
}

int audiobox_get_codec_ratio(audio_fmt_t *fmt)
{
	int ratio = 1;

	switch (fmt->codec_type) {
		case AUDIO_CODEC_G711U:
			ratio = 2;    //ratio: 2:1
			break;

		case AUDIO_CODEC_G711A:
			ratio = 2;    //ratio: 2:1
			break;

		case AUDIO_CODEC_G726:
			ratio = 8;    //ratio: 8:1 or 16:3 or 4:1 or 16:5
			break;

		case AUDIO_CODEC_MP3:
			ratio = 1;    //ratio: 8:1 or 16:3 or 4:1 or 16:5
			break;

		case AUDIO_CODEC_AAC:
			ratio = 1;    //ratio: 18:1
			break;

		case AUDIO_CODEC_PCM:
		default:
			ratio = 1;
			break;
	}

	return ratio;
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

audio_chn_t audiobox_get_chn_by_id(int id)
{
	audio_chn_t chn = NULL;
	audio_dev_t dev = NULL;
	struct list_head *pos, *pos1;

	pthread_mutex_lock(&devlist_mutex);
	list_for_each(pos, &audiobox_devlist) {
		dev = list_entry(pos, struct audio_dev, node);
		if (!dev) 
			continue;

		pthread_mutex_lock(&dev->mutex);
		list_for_each(pos1, &dev->head) {
			chn = list_entry(pos1, struct audio_chn, node);
			if (!chn)
				continue;
			if (chn->id == id) {
				pthread_mutex_lock(&chn->mutex);
				return chn;
			}
		}
		pthread_mutex_unlock(&dev->mutex);
	}
	pthread_mutex_unlock(&devlist_mutex);
	
	return NULL;
}

void audiobox_put_chn(audio_chn_t chn)
{
	pthread_mutex_unlock(&chn->mutex);
	pthread_mutex_unlock(&chn->dev->mutex);
	pthread_mutex_unlock(&devlist_mutex);
}

audio_dev_t audiobox_get_dev_by_name(char *dev_name)
{
	audio_dev_t dev = NULL;
	struct list_head *pos;

	if(!dev_name)
		return NULL;
	
	pthread_mutex_lock(&devlist_mutex);
	list_for_each(pos, &audiobox_devlist) {
		dev = list_entry(pos, struct audio_dev, node);
		if (!dev) 
			continue;
		if (!strcmp(dev->devname, dev_name)) {
			pthread_mutex_lock(&dev->mutex);
			return dev;
		}
	}
	pthread_mutex_unlock(&devlist_mutex);
	
	return NULL;
}

int audiobox_put_dev(audio_dev_t dev) {

	if(!dev)
		return -1;
	
	pthread_mutex_unlock(&dev->mutex);
	pthread_mutex_unlock(&devlist_mutex);

	return 0;
}

int audiobox_add_new_dev(char *dev_name, audio_dev_attr_t *attr)
{
	struct list_head *pos;
	audio_dev_t dev;
	int ret;
	int i;

	if (!dev_name)
		return -1;

	/* already exist, do nothing */
	list_for_each(pos, &audiobox_devlist) {
		dev = list_entry(pos, struct audio_dev, node);
		if (!dev) 
			continue;

		if (!strcmp(dev->devname, dev_name)){
			if(list_empty(&dev->head)) {
				if (attr && (memcmp(&dev->attr, attr, sizeof(attr->channels) + sizeof(attr->bitwidth) + sizeof(attr->samplingrate)) != 0)) {
					AUD_ERR("dev is empty and para changed!\n");
					audiobox_delete_dev(dev->devname);
				}else{
					return 0;
				}
			}else{
				return 0;
			}
		}
	}

	dev = (audio_dev_t)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	memset(dev, 0, sizeof(*dev));
	dev->volume_p = VOLUME_DEFAULT;
	dev->volume_c = VOLUME_DEFAULT;
	for (i = 0; i < CHN_DATA; i++) {
		if (!chndata[i].devname) {
			AUD_ERR("do not support audio channel %s\n", dev_name);
			return -1;
		}

		if (!strcmp(chndata[i].devname, dev_name)) {
			AUD_ERR("get master volume: p:%d, c:%d\n", chndata[i].volume_p, chndata[i].volume_c);
			dev->volume_p = chndata[i].volume_p;
			dev->volume_c = chndata[i].volume_c;
			break;
		}
	}

	sprintf(dev->devname, "%s", dev_name);
	dev->serv_id = -1;
	dev->direct = audiobox_get_direct(dev->devname);
	if (dev->direct < 0)
		return -1;
	
	if (attr){
		attr->sample_size = audiobox_get_sample_size(attr->samplingrate);
		memcpy(&dev->attr, attr, sizeof(audio_dev_attr_t));
	}else
		if (audiobox_get_default_dev_attr(&dev->attr, dev_name) < 0) {
			AUD_ERR("Audio get default format err\n");
			goto err;
		}

	if (audio_hal_open(dev) < 0) {
		AUD_ERR("Audio open channel %s err\n", dev_name);
		goto err;
	}

	if (audio_open_ctl(dev) < 0) {
		AUD_ERR("Audio open channel %s err\n", dev_name);
		goto err1;
	}

	AUD_DBG("Audio FRname is %s\n", dev->devname);
	if (dev->direct == DEVICE_IN_ALL){
		ret = fr_alloc(&dev->fr, dev->devname, 
			audiobox_get_dev_frame_size(&dev->attr),
			FR_FLAG_RING(5));
	}else{
		ret = fr_alloc(&dev->fr, dev->devname, 
			audiobox_get_dev_frame_size(&dev->attr),
			FR_FLAG_RING(5) | FR_FLAG_NODROP);
	}
	if (ret < 0) {
		AUD_ERR("Audio fr alloc err: %d\n", ret);
		goto err2;
	}
	
	pthread_mutex_init(&dev->mutex, NULL);
	pthread_mutex_init(&dev->preproc.mutex, NULL);
	pthread_rwlock_init(&dev->preproc.rwlock, NULL);
	pthread_cond_init(&dev->preproc.cond, NULL);

	AUD_DBG("create chn server\n");
	INIT_LIST_HEAD(&dev->head);

	/* add dev to devlist */
	list_add_tail(&dev->node, &audiobox_devlist);
	return 0;

err2:
	audio_close_ctl(dev);
err1:
	audio_hal_close(dev);
err:
	free(dev);
	return -1;
}

void audiobox_delete_dev(char *devname)
{
	struct list_head *pos;
	audio_dev_t dev;

	if (!devname)
		return;	

	pthread_mutex_lock(&devlist_mutex);
	list_for_each(pos, &audiobox_devlist) {
		dev = list_entry(pos, struct audio_dev, node);
		if (!dev)
			continue;

		pthread_mutex_lock(&dev->mutex);
		if (!strncmp(dev->devname, devname, strlen(dev->devname))) {
			if (list_empty(&dev->head)) {
				list_del(&dev->node);

				// exit the thread
				if(dev->enable) {
					AUD_DBG("AudioBox release chnserv!\n");
					pthread_mutex_unlock(&(dev->mutex));
					audio_release_chnserv(dev);
					pthread_mutex_lock(&(dev->mutex));
				}

				// disable the module
				if(dev->preproc.module != 0) {
					AUD_DBG("AudioBox disable preproc!\n");
					audiobox_disable_preproc_module(dev);
				}

				// close the ctl
				if (dev->ctl) {
					AUD_DBG("AudioBox close ctl!\n");
					audio_close_ctl(dev);
					dev->ctl = NULL;
				}

				// close the hal
				if (dev->handle) {
					AUD_DBG("AudioBox close hal!\n");
					audio_hal_close(dev);
					dev->handle = NULL;
				}

				// free the dev fr
				if (dev->fr.fr) {
					AUD_DBG("AudioBox free fr!\n");
					fr_free(&dev->fr);
					dev->fr.fr = NULL;
				}

				// destory all the mutex & cond
				AUD_DBG("AudioBox destory mutex!\n");
				pthread_cond_destroy(&(dev->preproc.cond));
				pthread_rwlock_destroy(&(dev->preproc.rwlock));
				pthread_mutex_destroy(&(dev->preproc.mutex));
				pthread_mutex_unlock(&dev->mutex);
				pthread_mutex_destroy(&(dev->mutex));

				// free dev finally
				AUD_DBG("AudioBox free dev!\n");
				free(dev);

				AUD_ERR("AudioBox release dev success!\n");
				break;
			}
		}
		pthread_mutex_unlock(&dev->mutex);
	}
	pthread_mutex_unlock(&devlist_mutex);
}


int _audiobox_get_dev_attr(char *dev_name, audio_dev_attr_t *attr)
{
	audio_dev_t dev = NULL;
	
	if (!attr)
		return -1;

	dev = audiobox_get_dev_by_name(dev_name);
	if (!dev)
		return -1;

	memcpy(attr, &dev->attr, sizeof(audio_dev_attr_t));
	audiobox_put_dev(dev);
	return 0;
}

int audiobox_get_dev_attr(struct event_scatter *event)
{
	audio_dev_attr_t *attr;
	char *dev_name = NULL;

	dev_name = AB_GET_DEV(event);	
	attr = (audio_dev_attr_t *)AB_GET_PRIV(event);
	if (!attr || !dev_name)
		return -1;
	else
		return _audiobox_get_dev_attr(dev_name, attr);
}

int _audiobox_set_dev_attr(char *dev_name, audio_dev_attr_t *attr)
{
	audio_dev_t dev = NULL;
	struct fr_info tmp_fr;
	int err = -1;

	err = audiobox_add_new_dev(dev_name, attr);
	if(err < 0)
		return -1;

	dev = audiobox_get_dev_by_name(dev_name);

	if (!dev)
		return -1;
	
	// set dev attr for enabled dev is not allowed, must disable dev first
	if(dev->enable)
		goto put_dev;

	if (attr && (memcmp(&dev->attr, attr, sizeof(attr->channels) + sizeof(attr->bitwidth) + sizeof(attr->samplingrate)) != 0)) {
		err = audio_hal_set_params(dev, attr);
		if(err < 0) {
			goto put_dev;
		}

		//err = fr_alloc(&tmp_fr, dev->devname, audiobox_get_dev_frame_size(attr), FR_FLAG_RING(5) | FR_FLAG_NODROP);
		err = fr_alloc(&tmp_fr, dev->devname, audiobox_get_dev_frame_size(attr), FR_FLAG_RING(5));
		if(err < 0) {
			// roll back to original attr
			audio_hal_set_params(dev, &dev->attr);
			goto put_dev;
		}

		// free the old fr
		if(dev->fr.fr) {
			fr_free(&dev->fr);
			dev->fr.fr = NULL;
		}

		// finally register the fr & attr back to dev
		memcpy(&dev->fr, &tmp_fr, sizeof(struct fr_info));
		memcpy(&dev->attr, attr, sizeof(audio_dev_attr_t));
	}
	err = 0;

	AUD_DBG(
		("AudioBox dev name %s, set attr to:"
		"attr.bitwidth: %d\n"
		"attr.samplingrate: %d\n"
		"attr.channels: %d\n"),
	   	dev_name, dev->attr.bitwidth, dev->attr.samplingrate, dev->attr.channels);

put_dev:
	audiobox_put_dev(dev);

	_audiobox_enable_dev(dev_name);
	return err;
}

int audiobox_set_dev_attr(struct event_scatter *event)
{
	audio_dev_attr_t *attr;
	char *dev_name = NULL;

	dev_name = AB_GET_DEV(event);	
	attr = (audio_dev_attr_t *)AB_GET_PRIV(event);
	if (!attr || !dev_name)
		return -1;
	else
		return _audiobox_set_dev_attr(dev_name, attr);
}

int audiobox_get_dev_volume(struct event_scatter *event)
{
	audio_dev_t dev = NULL;
	char *dev_name = NULL;
	int *volume = 0;

	if (!event)
		return -1;
	
	volume = (int *)AB_GET_PRIV(event);
	if (!volume)
		return -1;

	dev_name = AB_GET_DEV(event);	
	dev = audiobox_get_dev_by_name(dev_name);
	if (!dev)
		return -1;

	if(dev->direct == DEVICE_IN_ALL)
		*volume = dev->volume_c;
	else
		*volume = dev->volume_p;

	audiobox_put_dev(dev);
	return 0;
}

int audiobox_set_dev_volume(struct event_scatter *event)
{
	audio_dev_t dev = NULL;
	char *dev_name = NULL;
	int volume = 0;

	if (!event)
		return -1;
	
	volume = *(int *)AB_GET_PRIV(event);

	if (volume > 100)
		volume = 100;
	if (volume < 0)
		volume = 0;

	dev_name = AB_GET_DEV(event);	
	dev = audiobox_get_dev_by_name(dev_name);
	if (!dev)
		return -1;

	if(dev->direct == DEVICE_IN_ALL)
		dev->volume_c = volume;
	else
		dev->volume_p = volume;

	audiobox_put_dev(dev);
	return 0;
}

int audiobox_get_dev_module(struct event_scatter *event)
{
	audio_dev_t dev = NULL;
	int *module;
	char *dev_name = NULL;

	if (!event)
		return -1;

	module = (int *)AB_GET_PRIV(event);

	dev_name = AB_GET_DEV(event);
	dev = audiobox_get_dev_by_name(dev_name);
	if (!dev)
		return -1;

	*module = dev->preproc.module;
	audiobox_put_dev(dev);
	return 0;
}

int audiobox_set_dev_module(struct event_scatter *event)
{
	audio_dev_t dev = NULL;
	int module;
	int err = -1;
	char *dev_name = NULL;

	if (!event)
		return -1;

	module = *(int *)AB_GET_PRIV(event);

	dev_name = AB_GET_DEV(event);
	dev = audiobox_get_dev_by_name(dev_name);
	if (!dev)
		return -1;

	AUD_DBG("%s: dev->preproc.module = %x, module = %x\n", __func__, dev->preproc.module, module);
	// playback have not AEC module
	if(dev->direct == DEVICE_OUT_ALL && (module & MODULE_AEC))
		goto put_dev;

	if( (module != dev->preproc.module) ) {
		audiobox_disable_preproc_module(dev);
		if(audiobox_enable_preproc_module(dev, module) < 0)
			goto put_dev;
	}
	err = 0;
	

put_dev:
	audiobox_put_dev(dev);
	return err;
}

int _audiobox_enable_dev(char * dev_name) {
	audio_dev_t dev = NULL;

	dev = audiobox_get_dev_by_name(dev_name);
	if (!dev)
		return -1;

	if(dev->enable)
		goto put_dev;

	if (audio_create_chnserv(dev) < 0) {
		AUD_ERR("Audio create chnserv err: %s\n", dev_name);
		goto err;
	}

	dev->enable = 1;
put_dev:
	audiobox_put_dev(dev);
	return 0;

err:
	audio_release_chnserv(dev);
	audiobox_put_dev(dev);
	return -1;
}

int audiobox_enable_dev(struct event_scatter *event) {
	char *dev_name = NULL;

	dev_name = AB_GET_DEV(event);	
	if (!dev_name)
		return -1;
	else
		return _audiobox_enable_dev(dev_name);
}

int _audiobox_disable_dev(char *dev_name) {
	audio_dev_t dev = NULL;
	audio_chn_t chn = NULL;
	struct list_head *pos, *next_pos;

	dev = audiobox_get_dev_by_name(dev_name);
	if (!dev)
		return -1;

	if(!dev->enable)
		goto put_dev;

	audio_release_chnserv(dev);

	list_for_each_safe(pos, next_pos, &dev->head) {
		chn = list_entry(pos, struct audio_chn, node);
		if (!chn)
			continue;

		pthread_mutex_lock(&chn->mutex);
		list_del(&chn->node);	

		AUD_DBG("AudioBox release Channel %s\n", chn->chnname);
		if (chn->softvol_handle) {
			softvol_close(chn->softvol_handle);
			chn->softvol_handle = NULL;
		}

		if (chn->codec_handle) {
			codec_close(chn->codec_handle);
			chn->codec_handle = NULL;
		}

		if (chn->fr.fr) {
			fr_free(&chn->fr);
			chn->fr.fr = NULL;
		}

		audiobox_put_id(chn->id);
		pthread_mutex_unlock(&chn->mutex);
		free(chn);
	}

	dev->enable = 0;

put_dev:
	audiobox_put_dev(dev);
	return 0;
}


int audiobox_disable_dev(struct event_scatter *event) {
	char *dev_name = NULL;

	dev_name = AB_GET_DEV(event);	
	if (!dev_name)
		return -1;
	else
		return _audiobox_disable_dev(dev_name);
}

int audiobox_get_channel(struct event_scatter *event)
{
	audio_chn_t chn = NULL;
	audio_fmt_t *fmt = NULL;
	softvol_cfg_t cfg;
	audio_dev_t dev;
	char *dev_name;
	int flag;
	int ret;
	struct list_head *pos;
	
	if (!event)
		return -1;	
	
	dev_name = AB_GET_DEV(event);
	flag = *(int *)AB_GET_PRIV(event);
	if (AB_GET_RESVLEN(event) == sizeof(audio_fmt_t))
		fmt = (audio_fmt_t *)AB_GET_RESV(event);

	audiobox_get_default_dev_attr(fmt, NULL);
		
	// must pass fmt in
	if (fmt && fmt->bitwidth && fmt->samplingrate && fmt->channels) {
		AUD_DBG(
			("AudioBox dev name %s, flag %d, fmt %p\n"
			"fmt.bitwidth: %d\n"
			"fmt.samplingrate: %d\n"
			"fmt.channels: %d\n"
			"fmt.codec_type: %d\n"),
			dev_name, flag, fmt, fmt->bitwidth, fmt->samplingrate, fmt->channels, fmt->codec_type);
	}
	else {
		return -1;
	}

	// get dev
	dev = audiobox_get_dev_by_name(dev_name);
	if(!dev) {
		//set dev attr with default value
		_audiobox_set_dev_attr(dev_name, NULL);
		_audiobox_enable_dev(dev_name);
		dev = audiobox_get_dev_by_name(dev_name);

		//fail again
		if(!dev) 
			return -1;
	}
	if(!dev->enable) {
		goto err;
	}


	// looking for the chn that have the same fmt
	list_for_each(pos, &dev->head) {
		chn = list_entry(pos, struct audio_chn, node);
		if (!chn)
			continue;
		// the same fmt chn found, just return it, only enabled for capture
		if((!memcmp(fmt, &chn->fmt, sizeof(fmt->bitwidth) + sizeof(fmt->samplingrate) + sizeof(fmt->channels) + sizeof(fmt->codec_type))) && (chn->direct == DEVICE_IN_ALL))
			goto put_dev;
	}

	// new chn
	chn = malloc(sizeof(*chn));
	if (!chn)
		goto err;

	memset(chn, 0, sizeof(*chn));
	chn->id = audiobox_get_id();
	if (chn->id < 0)
		goto err1;

	sprintf(chn->chnname, "%s_c%d", dev->devname, chn->id);
	chn->dev = dev;
	pthread_mutex_init(&chn->mutex, NULL);
	chn->flag = flag;
	chn->timeout = (flag & TIMEOUT_MASK)? PLAYBACK_TIMEOUT : 0;
	chn->priority = (flag & PRIORITY_MASK) >> PRIORITY_BIT;
	chn->direct = dev->direct;
	fr_INITBUF(&chn->ref, NULL);
	if (chn->priority > dev->priority)
		dev->priority = chn->priority;
		
	// fr for every chn
	ret = fr_alloc(&chn->fr, chn->chnname, audiobox_get_chn_frame_size(fmt) / audiobox_get_codec_ratio(fmt), FR_FLAG_RING(5) | FR_FLAG_NODROP);
	if (ret < 0) {
		AUD_ERR("Audio fr alloc err: %d\n", ret);
		goto err2;
	}

	// codecer for chn
	if(fmt->codec_type != AUDIO_CODEC_PCM || (fmt->channels != dev->attr.channels) || (fmt->bitwidth != dev->attr.bitwidth) || (fmt->samplingrate != dev->attr.samplingrate)) {
		codec_info_t codec_fmt;

		memset(&codec_fmt, 0, sizeof(codec_fmt));
		if(dev->direct == DEVICE_OUT_ALL) {
			codec_fmt.in.codec_type = fmt->codec_type;
			codec_fmt.in.effect = 0;
			codec_fmt.in.channel = fmt->channels;
			codec_fmt.in.sample_rate = fmt->samplingrate;
			codec_fmt.in.bitwidth = fmt->bitwidth;
			codec_fmt.in.bit_rate = fmt->channels * fmt->samplingrate * fmt->bitwidth;

			codec_fmt.out.codec_type = AUDIO_CODEC_PCM;
			codec_fmt.out.effect = 0;
			codec_fmt.out.channel = dev->attr.channels;
			codec_fmt.out.sample_rate = dev->attr.samplingrate;
			codec_fmt.out.bitwidth = dev->attr.bitwidth;
			codec_fmt.out.bit_rate = dev->attr.channels * dev->attr.samplingrate * dev->attr.bitwidth;
		}
		else {
			codec_fmt.in.codec_type = AUDIO_CODEC_PCM;
			codec_fmt.in.effect = 0;
			codec_fmt.in.channel = dev->attr.channels;
			codec_fmt.in.sample_rate = dev->attr.samplingrate;
			codec_fmt.in.bitwidth = dev->attr.bitwidth;
			codec_fmt.in.bit_rate = dev->attr.channels * dev->attr.samplingrate * dev->attr.bitwidth;

			codec_fmt.out.codec_type = fmt->codec_type;
			codec_fmt.out.effect = 0;
			codec_fmt.out.channel = fmt->channels;
			codec_fmt.out.sample_rate = fmt->samplingrate;
			codec_fmt.out.bitwidth = fmt->bitwidth;
			codec_fmt.out.bit_rate = fmt->channels * fmt->samplingrate * fmt->bitwidth;
		}

		chn->codec_handle = codec_open(&codec_fmt);
		if (!chn->codec_handle) {
			AUD_ERR("Open codecer err\n");
			goto err3;
		}
	}
	fmt->sample_size = audiobox_get_sample_size(fmt->samplingrate) / audiobox_get_codec_ratio(fmt);
	memcpy(&chn->fmt, fmt, sizeof(*fmt));

	// we may need softvol
	chn->volume = PRESET_RESOLUTION;
	cfg.format = chn->fmt.bitwidth;
	cfg.channels = chn->fmt.channels;
	cfg.min_dB = PRESET_MIN_DB;
	cfg.max_dB = ZERO_DB;
	cfg.resolution = PRESET_RESOLUTION;
	chn->softvol_handle = softvol_open(chn->id, &cfg);
	if (!chn->softvol_handle) {
		AUD_ERR("Audio open Softvol err\n");
		goto err4;
	}

	list_add_tail(&chn->node, &dev->head);

put_dev:
	audiobox_put_dev(dev);
	AUD_ERR("AudioBox get channel %s for %s\n", chn->chnname, dev->devname);
	return chn->id;

err4:
	if (chn->softvol_handle) {
		softvol_close(chn->softvol_handle);
		chn->softvol_handle = NULL;
	}

	if (chn->codec_handle) {
		codec_close(chn->codec_handle);
		chn->codec_handle = NULL;
	}

err3:
	if (chn->fr.fr) {
		fr_free(&chn->fr);
		chn->fr.fr = NULL;
	}

err2:
	audiobox_put_id(chn->id);

err1:
	free(chn);

err:
	audiobox_put_dev(dev);
	return -1;
}

int audiobox_put_channel(struct event_scatter *event)
{
	audio_chn_t chn;
	//audio_dev_t dev;
	int id;

	if (!event)
		return -1;
	
	id = *(int *)AB_GET_CHN(event);

retry:
	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	if(chn->in_used > 0) {
		audiobox_put_chn(chn);
		AUD_ERR("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~AudioBox release Channel %s, pending!!\n", chn->chnname);
		usleep(5 * 1000);
		goto retry;
	}

	list_del(&chn->node);	
	audiobox_put_chn(chn);

	AUD_DBG("AudioBox release Channel %s\n", chn->chnname);
	if (chn->softvol_handle) {
		softvol_close(chn->softvol_handle);
		chn->softvol_handle = NULL;
	}

	if (chn->codec_handle) {
		codec_close(chn->codec_handle);
		chn->codec_handle = NULL;
	}

	if (chn->fr.fr) {
		fr_free(&chn->fr);
		chn->fr.fr = NULL;
	}

	//dev = chn->dev;
	audiobox_put_id(chn->id);
	free(chn);

	//if(list_empty(&dev->head)) {
		//AUD_ERR("dev is empty now\n");
		//audiobox_delete_dev(dev->devname);
	//}

	AUD_DBG("AudioBox release Channel OK\n");
	return 0;
}

int audiobox_get_chn_fmt(struct event_scatter *event)
{
	audio_chn_t chn = NULL;
	audio_fmt_t *fmt;
	int id;

	//AUD_ERR("entering %s\n", __func__);
	if (!event)
		return -1;
	
	id = *(int *)AB_GET_CHN(event);
	fmt = (audio_fmt_t *)AB_GET_PRIV(event);
	//AUD_ERR("%s: id = %d, fmt = %p\n", __func__, id, fmt);
	if (!fmt)
		return -1;

	//AUD_ERR("%s: try to get chn by id\n", __func__);
	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	memcpy(fmt, &chn->fmt, sizeof(*fmt));

	//AUD_ERR("%s: try to put chn\n", __func__);
	audiobox_put_chn(chn);
	return 0;
}

int audiobox_set_chn_fmt(struct event_scatter *event)
{
	audio_dev_t dev = NULL;
	audio_chn_t chn = NULL;
	audio_fmt_t *fmt;
	codec_info_t codec_fmt;
	int id;

	if (!event)
		return -1;
	
	id = *(int *)AB_GET_CHN(event);
	fmt = (audio_fmt_t *)AB_GET_PRIV(event);
	if (!fmt)
		return -1;

	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	if(memcmp(fmt, &chn->fmt, sizeof(fmt->bitwidth) + sizeof(fmt->samplingrate) + sizeof(fmt->channels) + sizeof(fmt->codec_type))) {
		if (chn->codec_handle) {
			codec_close(chn->codec_handle);
			chn->codec_handle = NULL;
		}

		dev = chn->dev;
		if(fmt->codec_type != AUDIO_CODEC_PCM || (fmt->channels != dev->attr.channels) || (fmt->bitwidth != dev->attr.bitwidth) || (fmt->samplingrate != dev->attr.samplingrate)) {
			memset(&codec_fmt, 0, sizeof(codec_fmt));
			if(chn->direct == DEVICE_OUT_ALL) {
				codec_fmt.in.codec_type = fmt->codec_type;
				codec_fmt.in.effect = 0;
				codec_fmt.in.channel = fmt->channels;
				codec_fmt.in.sample_rate = fmt->samplingrate;
				codec_fmt.in.bitwidth = fmt->bitwidth;
				codec_fmt.in.bit_rate = fmt->channels * fmt->samplingrate * fmt->bitwidth;

				codec_fmt.out.codec_type = AUDIO_CODEC_PCM;
				codec_fmt.out.effect = 0;
				codec_fmt.out.channel = dev->attr.channels;
				codec_fmt.out.sample_rate = dev->attr.samplingrate;
				codec_fmt.out.bitwidth = dev->attr.bitwidth;
				codec_fmt.out.bit_rate = dev->attr.channels * dev->attr.samplingrate * dev->attr.bitwidth;
			}
			else {
				codec_fmt.in.codec_type = AUDIO_CODEC_PCM;
				codec_fmt.in.effect = 0;
				codec_fmt.in.channel = dev->attr.channels;
				codec_fmt.in.sample_rate = dev->attr.samplingrate;
				codec_fmt.in.bitwidth = dev->attr.bitwidth;
				codec_fmt.in.bit_rate = dev->attr.channels * dev->attr.samplingrate * dev->attr.bitwidth;

				codec_fmt.out.codec_type = fmt->codec_type;
				codec_fmt.out.effect = 0;
				codec_fmt.out.channel = fmt->channels;
				codec_fmt.out.sample_rate = fmt->samplingrate;
				codec_fmt.out.bitwidth = fmt->bitwidth;
				codec_fmt.out.bit_rate = fmt->channels * fmt->samplingrate * fmt->bitwidth;
			}

			chn->codec_handle = codec_open(&codec_fmt);
			if (!chn->codec_handle) {
				AUD_ERR("Open codecer err\n");
				goto err;
			}
		}
		
		memcpy(&chn->fmt, fmt, sizeof(*fmt));
	}

	audiobox_put_chn(chn);
	return 0;

	if (chn->codec_handle) {
		codec_close(chn->codec_handle);
		chn->codec_handle = NULL;
	}
err:
	audiobox_put_chn(chn);
	return -1;

}

int audiobox_get_chn_mute(struct event_scatter *event)
{
	audio_chn_t chn = NULL;
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

	err = audio_route_cget(chn->dev, "numid=3", value);
	if (err < 0)
		goto err_out;
	
	if (!strcmp(value, "off"))
		*mute = 0;
	else if (!strcmp(value, "on"))
		*mute = 1;

	audiobox_put_chn(chn);

	return 0;

err_out:
	audiobox_put_chn(chn);
	return -1;
}

int audiobox_set_chn_mute(struct event_scatter *event)
{
	audio_chn_t chn = NULL;
	int id;
	int mute;
	int err;

	id = *(int *)AB_GET_CHN(event);
	mute = *(int *)AB_GET_PRIV(event);

	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	err = audio_route_cset(chn->dev, "numid=3", mute ? "on" : "off");

	audiobox_put_chn(chn);
	return err;
}

int audiobox_get_chn_volume(struct event_scatter *event)
{
	audio_chn_t chn = NULL;
	int id;
	int *volume;

	id = *(int *)AB_GET_CHN(event);
	volume = (int *)AB_GET_PRIV(event);
	
	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;
	
	AUD_DBG("get volume %d\n", chn->volume);
	*volume = chn->volume;

	audiobox_put_chn(chn);
	return 0;
}

int audiobox_set_chn_volume(struct event_scatter *event)
{
	audio_chn_t chn = NULL;
	int id;
	int setvol;

	id = *(int *)AB_GET_CHN(event);
	setvol = *(int *)AB_GET_PRIV(event);

	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;
	
	if (setvol != chn->volume) {
		AUD_DBG("Set soft volume %d\n", setvol);
		chn->volume = setvol;
	}

	audiobox_put_chn(chn);
	return 0;
}



int audiobox_disable_preproc_module(audio_dev_t dev) 
{
	int err;

	if(dev->state == PREPROC_MODE || dev->state == ENTERING_PREPROC_MODE) {

		AUD_DBG("xym-debug: %s: prepare to changing from PREPROC to NORMAL mode\n", __func__);
		dev->state = ENTERING_NORMAL_MODE;

		pthread_rwlock_wrlock(&(dev->preproc.rwlock));
		if(!dev->preproc.initiated) { //module processing isn't initiated
			pthread_rwlock_unlock(&(dev->preproc.rwlock));
			goto exit;
		}

		AUD_DBG("xym-debug: %s: releasing vcp\n", __func__);
		if(vcp7g_is_init(&dev->preproc.vcpobj)) {
			vcp7g_deinit(&(dev->preproc.vcpobj)); //it worth to deinit again even it have been done before
			memset(&(dev->preproc.vcpobj), 0, sizeof(struct vcp_object));
		}

		if(dev->preproc.module & MODULE_AEC) {
			//normal status: L1AL=enable, R1AR=enable, DLAL=disable, L1AR=disable
			//err = audio_route_cset(dev, "name='Mic L1AL Enable Switch'", "on"); //not working
			err = audio_route_cset(dev, "numid=1", "on");
			//err |= audio_route_cset(dev, "name='Mic L2AL Enable Switch'", "on");//not working
			err |= audio_route_cset(dev, "numid=2", "on");

			//err |= audio_route_cset(dev, "name='Mic R1AR Enable Switch'", "on");//not working
			err |= audio_route_cset(dev, "numid=5", "on");
			//err |= audio_route_cset(dev, "name='Mic R2AR Enable Switch'", "on");//not working
			err |= audio_route_cset(dev, "numid=6", "on");

			//err |= audio_route_cset(dev, "name='Mic L1AR Enable Switch'", "off");//not working
			err |= audio_route_cset(dev, "numid=3", "off");
			//err |= audio_route_cset(dev, "name='Mic L2AR Enable Switch'", "off");//not working
			err |= audio_route_cset(dev, "numid=4", "off");

			//err |= audio_route_cset(dev, "name='Mic DLAL Enable Switch'", "off");//not working
			err |= audio_route_cset(dev, "numid=7", "off");
			if(err)
				AUD_ERR("back from AEC mode to NORMAL mode fail!\n"); //wrong again? ALSA or snd_card may broken down
		}

		dev->preproc.initiated = 0;
		dev->preproc.module = 0;
		pthread_rwlock_unlock(&(dev->preproc.rwlock));

exit:

		dev->state = NORMAL_MODE; //back to NORMAL MODE
		AUD_DBG("xym-debug: %s: change from PREPROC to NORMAL mode done, server_running = %d\n", __func__, chn->aec.server_running);
	}

	return 0;
}

int audiobox_enable_preproc_module(audio_dev_t dev, int module)
{
	struct vcp_object vcpobj;
	audio_dev_attr_t *attr;
//	int size;
	int err;
    int use_dsp = 1;
//	struct timeval now;
//	struct timespec timeout;

	//no module is set
	if(!module)
		return 0;

	AUD_DBG("%s: dev->state = %d\n", __func__, dev->state);
	if(dev->state == NORMAL_MODE || dev->state == ENTERING_NORMAL_MODE) {

		AUD_DBG("xym-debug: %s: prepare to changing from NORMAL to PREPROC mode\n", __func__);
		pthread_rwlock_wrlock(&(dev->preproc.rwlock));
		if(dev->preproc.initiated) {
			AUD_ERR("%s: preproc already initiated!\n", __func__);
			goto err_initiated_locked;
		}

		dev->state = ENTERING_PREPROC_MODE;
		attr = &(dev->attr);

		//TODO: modify this to support more preproc other than AEC
		//err = vcp7g_init(&vcpobj, attr->channels, attr->samplingrate, attr->bitwidth, dev->preproc.module, &use_dsp);
		err = vcp7g_init(&vcpobj, attr->channels, attr->samplingrate, attr->bitwidth, &use_dsp);
		if (err < 0) {
			AUD_ERR("%s: vcp7g_init failed, use_dsp = %d\n", __func__, use_dsp);
			goto err_vcp_init_locked;
		}
		AUD_DBG("%s: vcp7g_init success, use_dsp = %d\n", __func__, use_dsp);
		memcpy(&(dev->preproc.vcpobj), &vcpobj, sizeof(struct vcp_object));

		if(module & MODULE_AEC) {
			//aec status: L1AL=disable, R1AR=disable, DLAL=enable, L1AR=enable
			//err = audio_route_cset(dev, "name='Mic L1AL Enable Switch'", "on"); //not working
			err = audio_route_cset(dev, "numid=1", "off");
			//err |= audio_route_cset(dev, "name='Mic L2AL Enable Switch'", "on");//not working
			err |= audio_route_cset(dev, "numid=2", "off");

			//err |= audio_route_cset(dev, "name='Mic R1AR Enable Switch'", "on");//not working
			err |= audio_route_cset(dev, "numid=5", "off");
			//err |= audio_route_cset(dev, "name='Mic R2AR Enable Switch'", "on");//not working
			err |= audio_route_cset(dev, "numid=6", "off");

			//err |= audio_route_cset(dev, "name='Mic L1AR Enable Switch'", "off");//not working
			err |= audio_route_cset(dev, "numid=3", "on");
			//err |= audio_route_cset(dev, "name='Mic L2AR Enable Switch'", "off");//not working
			err |= audio_route_cset(dev, "numid=4", "on");

			//err |= audio_route_cset(dev, "name='Mic DLAL Enable Switch'", "off");//not working
			err |= audio_route_cset(dev, "numid=7", "on");
			if(err) {
				AUD_ERR("AudioBox AEC route cset err\n");
				goto err_route_cset_locked;
			}
		}

		dev->preproc.initiated = 1;
		dev->preproc.module = module;
		pthread_rwlock_unlock(&(dev->preproc.rwlock));

		dev->state = PREPROC_MODE;
		AUD_DBG("xym-debug: %s: change from NORMAL to PREPROC mode success\n", __func__);

	}
	

	return 0;

//err_pthread_create_locked:
	//pthread_mutex_unlock(&(dev->preproc.mutex));
	pthread_rwlock_wrlock(&(dev->preproc.rwlock));

	if(module &	MODULE_AEC) {
		//normal status: L1AL=enable, R1AR=enable, DLAL=disable, L1AR=disable
		//err = audio_route_cset(dev, "name='Mic L1AL Enable Switch'", "on"); //not working
		err = audio_route_cset(dev, "numid=1", "on");
		//err |= audio_route_cset(dev, "name='Mic L2AL Enable Switch'", "on");//not working
		err |= audio_route_cset(dev, "numid=2", "on");

		//err |= audio_route_cset(dev, "name='Mic R1AR Enable Switch'", "on");//not working
		err |= audio_route_cset(dev, "numid=5", "on");
		//err |= audio_route_cset(dev, "name='Mic R2AR Enable Switch'", "on");//not working
		err |= audio_route_cset(dev, "numid=6", "on");

		//err |= audio_route_cset(dev, "name='Mic L1AR Enable Switch'", "off");//not working
		err |= audio_route_cset(dev, "numid=3", "off");
		//err |= audio_route_cset(dev, "name='Mic L2AR Enable Switch'", "off");//not working
		err |= audio_route_cset(dev, "numid=4", "off");

		//err |= audio_route_cset(dev, "name='Mic DLAL Enable Switch'", "off");//not working
		err |= audio_route_cset(dev, "numid=7", "off");
		if(err)
			AUD_ERR("back from AEC mode to NORMAL mode fail!\n"); //wrong again? ALSA or snd_card may broken down
	}

err_route_cset_locked:
	if(vcp7g_is_init(&dev->preproc.vcpobj)) {
		vcp7g_deinit(&(dev->preproc.vcpobj)); //it worth to deinit again even it have been done before
		memset(&(dev->preproc.vcpobj), 0, sizeof(struct vcp_object));
	}

err_vcp_init_locked:
err_initiated_locked:
	pthread_rwlock_unlock(&(dev->preproc.rwlock));
	dev->state = NORMAL_MODE; //back to NORMAL MODE
	AUD_DBG("xym-debug: %s: back to NORMAL mode done, server_running = %d\n", __func__, dev->preproc.server_running);
	return -1;

}

int audiobox_stop_service(struct event_scatter *event)
{
	audio_chn_t chn = NULL;
	audio_dev_t dev = NULL;
	struct list_head *pos;

	pthread_mutex_lock(&devlist_mutex);
	list_for_each(pos, &audiobox_devlist) {
		dev = list_entry(pos, struct audio_dev, node);
		if (!dev) 
			continue;
		pthread_mutex_lock(&dev->mutex);
		list_for_each(pos, &dev->head) {
			chn = list_entry(pos, struct audio_chn, node);
			if (!chn)
				continue;
			
			AUD_DBG("AudioBox release Channel %s\n", chn->chnname);
			pthread_mutex_lock(&chn->mutex);
			list_del(&chn->node);
			pthread_mutex_unlock(&chn->mutex);

			if (chn->softvol_handle) {
				softvol_close(chn->softvol_handle);
				chn->softvol_handle = NULL;
			}

			if (chn->codec_handle) {
				codec_close(chn->codec_handle);
				chn->codec_handle = NULL;
			}

			if (chn->fr.fr) {
				fr_free(&chn->fr);
				chn->fr.fr = NULL;
			}

			audiobox_put_id(chn->id);
			free(chn);
		}
		pthread_mutex_unlock(&dev->mutex);
	}
	pthread_mutex_unlock(&devlist_mutex);
	audiobox_signal_handler(0);
	return 0;
}

#if 0
int audiobox_get_chn_info(struct event_scatter *event)
{
	audio_chn_t chn;
	audio_dev_t dev;
	audio_chn_t chn_info;
	audio_dev_t dev_info;
	int size = 0;
	int id;
	int err;

	if (!event)
		return -1;
	
	id = AB_GET_CHN(event);	
	chn_info = (audio_chn_t)AB_GET_PRIV(event);
	dev_info = (audio_dev_t)AB_GET_RESV(event);

	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;
	dev = chn->dev;

	memcpy(chn_info, chn, sizeof(*chn));
	memcpy(dev_info, dev, sizeof(*dev));

	audiobox_put_chn(chn);

	return 0;
}
#endif

int audiobox_read_chn_frame(struct event_scatter *event)
{
	audio_chn_t chn;
	int id;

	if (!event)
		return -1;
	
	id = *(int *)AB_GET_CHN(event);
	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	chn->active = 1;

	audiobox_put_chn(chn);

	return 0;
}

#if 0
int audiobox_write_chn_frame(struct event_scatter *event)
{
	audio_chn_t chn;
	audio_dev_t dev;
	int size = 0;
	int id;
	int err;

	if (!event)
		return -1;
	
	id = AB_GET_CHN(event);	
	chn = audiobox_get_chn_by_id(id);
	if (!chn)
		return -1;

	chn->active = 1;
	audiobox_put_chn(chn);

	return 0;
}
#endif

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
		case AB_GET_DEV_ATTR:
			*result = audiobox_get_dev_attr(audiobox);
			break;
		case AB_SET_DEV_ATTR:
			*result = audiobox_set_dev_attr(audiobox);
			break;
		case AB_GET_DEV_VOLUME:
			*result = audiobox_get_dev_volume(audiobox);
			break;
		case AB_SET_DEV_VOLUME:
			*result = audiobox_set_dev_volume(audiobox);
			break;
		case AB_GET_DEV_MODULE:
			*result = audiobox_get_dev_module(audiobox);
			break;
		case AB_SET_DEV_MODULE:
			*result = audiobox_set_dev_module(audiobox);
			break;
		case AB_ENABLE_DEV:
			*result = audiobox_enable_dev(audiobox);
			break;
		case AB_DISABLE_DEV:
		    *result = audiobox_disable_dev(audiobox);
			break;
		case AB_GET_CHANNEL:
			*result = audiobox_get_channel(audiobox);
			break;
		case AB_PUT_CHANNEL:
			*result = audiobox_put_channel(audiobox);
			break;
		case AB_GET_CHN_FMT:
			*result = audiobox_get_chn_fmt(audiobox);
			break;
		case AB_SET_CHN_FMT:
			*result = audiobox_set_chn_fmt(audiobox);
			break;
		case AB_GET_CHN_MUTE:
			*result = audiobox_get_chn_mute(audiobox);
			break;
		case AB_SET_CHN_MUTE:
			*result = audiobox_set_chn_mute(audiobox);
			break;
		case AB_GET_CHN_VOLUME:
			*result = audiobox_get_chn_volume(audiobox);
			break;
		case AB_SET_CHN_VOLUME:
			*result = audiobox_set_chn_volume(audiobox);
			break;
#if 0
		case AB_GET_CHN_INFO:
			*result = audiobox_get_chn_info(audiobox);
			break;
#endif
		case AB_READ_CHN_FRAME:
			*result = audiobox_read_chn_frame(audiobox);
			break;
#if 0
		case AB_WRITE_CHN_FRAME:
			*result = audiobox_write_chn_frame(audiobox);
			break;
#endif

		default:
			AUD_ERR("Do not support this cmd: %d\n", cmd);
			*result = -1;
			break;
	}
	AUD_DBG("Audiobox_Listener result %d\n", *result);
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
