#include <list.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <log.h>
#include <fr/libfr.h>
#include <audiobox2.h>
#include <audio_chn.h>

LIST_HEAD(logic_chnlist);

static int audio_get_direct(char *chnname)
{
	char *p;

	if (!chnname)
		return -1;
	
	AUD_DBG("audio_get_direct: %s\n", chnname);
	p = strstr(chnname, "mic");
	if (p)
		return DEVICE_CHN_IN;
	else
		return DEVICE_CHN_OUT;
	
}

static int audio_cal_frsize(audio_chn_fmt_t *fmt)
{
	audio_chn_fmt_t def = {
		.channels = 2,     
		.bitwidth = 32,
		.samplingrate = 16000,
		.sample_size = 1024, 
	};
	audio_chn_fmt_t *use = fmt ? fmt : &def;
	int bitwidth;

	bitwidth = ((use->bitwidth > 16) ? 32 : use->bitwidth);
	return (use->channels * (bitwidth >> 3) * use->sample_size);
}

int audio_init_logchn(char *devname, audio_chn_fmt_t *fmt, int handle)
{
	logic_t logchn;

	if (!devname || handle < 0)
		return -1;

	logchn = (logic_t)malloc(sizeof(*logchn));
	if (!logchn)
		return -1;

	logchn->handle = handle;
	logchn->direct = audio_get_direct(devname);
	logchn->frsize = audio_cal_frsize(fmt);
	sprintf(logchn->devname, "%s", devname);
	sprintf(logchn->chnname, "%s_c%d", devname, handle);

	fr_INITBUF(&logchn->fr, NULL);

	list_add_tail(&logchn->node, &logic_chnlist);
	return 0;
}

logic_t audio_get_logchn_by_handle(int handle)
{
	struct list_head *pos;
	logic_t logchn = NULL;

	list_for_each(pos, &logic_chnlist) {
		logchn = list_entry(pos, struct logic_chn, node);
		if (!logchn)
			continue;
		if (logchn->handle == handle)
			return logchn;
	}

	return NULL;
}

int audio_reset_logchn_by_id(int handle, audio_chn_fmt_t *fmt)
{
	struct list_head *pos;
	logic_t logchn = NULL;

	if (handle < 0)
		return -1;

	list_for_each(pos, &logic_chnlist) {
		logchn = list_entry(pos, struct logic_chn, node);
		if (!logchn)
			continue;
		if (logchn->handle == handle) {
			AUD_DBG("logchn->chnname: %s, handle:%d\n", logchn->chnname, handle);
			logchn->frsize = audio_cal_frsize(fmt);
			logchn->direct = audio_get_direct(logchn->chnname);
			fr_INITBUF(&logchn->fr, NULL);
		}
	}

	return 0;
}

int audio_release_logchn(int handle)
{
	logic_t logchn = NULL;
	
	logchn = audio_get_logchn_by_handle(handle);
	if (!logchn)
		return -1;
	
	list_del(&logchn->node);
	free(logchn);
	return 0;
}

int audio_get_devname(int handle, char *name)
{
	logic_t logchn = NULL;

	if (!name)
		return -1;

	logchn = audio_get_logchn_by_handle(handle);
	if (!logchn) {
		AUD_ERR("logchn no found: id=%d\n", handle);
		return -1;
	}

	sprintf(name, "%s", logchn->devname);
	return 0;
}

int audio_get_chnname(int handle, char *name)
{
	logic_t logchn = NULL;

	if (!name)
		return -1;

	logchn = audio_get_logchn_by_handle(handle);
	if (!logchn)
		return -1;

	sprintf(name, "%s", logchn->chnname);
	return 0;
}

int audio_get_frname(int handle, char *name)
{
	logic_t logchn = NULL;

	if (!name)
		return -1;

	logchn = audio_get_logchn_by_handle(handle);
	if (!logchn)
		return -1;

	sprintf(name, "%s", logchn->chnname);
	return 0;
}

int audio_get_frsize(int handle)
{
	logic_t logchn = NULL;

	logchn = audio_get_logchn_by_handle(handle);
	if (!logchn)
		return -1;

	return logchn->frsize;
}

int audio_get_fr(int handle, struct fr_buf_info *fr)
{
	logic_t logchn = NULL;

	if (!fr)
		return -1;

	logchn = audio_get_logchn_by_handle(handle);
	if (!logchn)
		return -1;

	memcpy(fr, &logchn->fr, sizeof(struct fr_buf_info));
	return 0;
}

int audio_put_fr(int handle, struct fr_buf_info *fr)
{
	logic_t logchn = NULL;

	if (!fr)
		return -1;

	logchn = audio_get_logchn_by_handle(handle);
	if (!logchn)
		return -1;

	memcpy(&logchn->fr, fr, sizeof(struct fr_buf_info));
	return 0;
}
