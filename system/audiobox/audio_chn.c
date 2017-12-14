#include <list.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <log.h>
#include <fr/libfr.h>
#include <audiobox.h>
#include <audio_chn.h>

LIST_HEAD(logic_chnlist);

static int audio_get_direct(char *devname)
{
	char *p;

	if (!devname)
		return -1;
	
	AUD_DBG("audio_get_direct: %s\n", devname);
	p = strstr(devname, "mic");
	if (p)
		return DEVICE_CHN_IN;
	else
		return DEVICE_CHN_OUT;
	
}

static int audio_cal_frsize(audio_fmt_t *fmt)
{
	audio_fmt_t def = {
		.channels = 2,     
		.bitwidth = 24,       
		.samplingrate = 48000,
		.sample_size = 1024, 
	};
	audio_fmt_t *use = fmt ? fmt : &def;
	int bitwidth;

	bitwidth = ((use->bitwidth > 16) ? 32 : use->bitwidth);
	return (use->channels * (bitwidth >> 3) * use->sample_size);
}

int audio_init_logchn(char *devname, audio_fmt_t *fmt, int handle)
{
	logic_t dev;

	if (!devname || handle < 0)
		return -1;

	dev = (logic_t)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->handle = handle;
	dev->direct = audio_get_direct(devname);
	dev->frsize = audio_cal_frsize(fmt);
	sprintf(dev->devname, "%s", devname);

	fr_INITBUF(&dev->fr, NULL);

	list_add_tail(&dev->node, &logic_chnlist);
	return 0;
}

logic_t audio_get_logchn_by_handle(int handle)
{
	struct list_head *pos;
	logic_t dev = NULL;

	list_for_each(pos, &logic_chnlist) {
		dev = list_entry(pos, struct logic_chn, node);
		if (!dev)
			continue;
		if (dev->handle == handle)
			return dev;
	}

	return NULL;
}

int audio_reset_logchn_by_name(char *devname, audio_fmt_t *fmt)
{
	struct list_head *pos;
	logic_t dev = NULL;

	if (!devname)
		return -1;

	list_for_each(pos, &logic_chnlist) {
		dev = list_entry(pos, struct logic_chn, node);
		if (!dev)
			continue;
		if (!strcmp(dev->devname, devname)) {
			AUD_DBG("dev->devname: %s,devname:%s\n", dev->devname, devname);
			dev->frsize = audio_cal_frsize(fmt);
			dev->direct = audio_get_direct(devname);
			fr_INITBUF(&dev->fr, NULL);
		}
	}

	return 0;
}

int audio_release_logchn(int handle)
{
	logic_t dev = NULL;
	
	dev = audio_get_logchn_by_handle(handle);
	if (!dev)
		return -1;
	
	list_del(&dev->node);
	free(dev);
	return 0;
}

int audio_get_chnname(int handle, char *name)
{
	logic_t dev = NULL;

	if (!name)
		return -1;

	dev = audio_get_logchn_by_handle(handle);
	if (!dev)
		return -1;

	sprintf(name, "%s", dev->devname);
	return 0;
}

int audio_get_frname(int handle, char *name)
{
	logic_t dev = NULL;

	if (!name)
		return -1;

	dev = audio_get_logchn_by_handle(handle);
	if (!dev)
		return -1;

	if (dev->direct == DEVICE_CHN_OUT)
		sprintf(name, "%s%d", dev->devname, handle);
	else
		sprintf(name, "%s", dev->devname);
	return 0;
}

int audio_get_frsize(int handle)
{
	logic_t dev = NULL;

	dev = audio_get_logchn_by_handle(handle);
	if (!dev)
		return -1;

	return dev->frsize;
}

int audio_get_fr(int handle, struct fr_buf_info *fr)
{
	logic_t dev = NULL;

	if (!fr)
		return -1;

	dev = audio_get_logchn_by_handle(handle);
	if (!dev)
		return -1;

	memcpy(fr, &dev->fr, sizeof(struct fr_buf_info));
	return 0;
}

int audio_put_fr(int handle, struct fr_buf_info *fr)
{
	logic_t dev = NULL;

	if (!fr)
		return -1;

	dev = audio_get_logchn_by_handle(handle);
	if (!dev)
		return -1;

	memcpy(&dev->fr, fr, sizeof(struct fr_buf_info));
	return 0;
}
