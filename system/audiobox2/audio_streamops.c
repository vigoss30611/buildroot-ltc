#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <log.h>
#include <qsdk/event.h>
#include <fr/libfr.h>
#include <audiobox2.h>
#include <list.h>
#include <dirent.h>
#include <audio_rpc.h>
#include <audiobox_listen.h>
#include <audio_chn.h>
#include <alsa/asoundlib.h>

int audio_start_service(void)
{
	DIR *dir;
	FILE *fp;
	struct dirent *ptr;
	char path[50];
	char buf[50];
	int len;

	dir = opendir("/proc");
	if (!dir)
		return -1;
	
	while ((ptr = readdir(dir))) {
		if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, ".."))
			continue;

		if (DT_DIR != ptr->d_type)
			continue;

		sprintf(path, "/proc/%s/cmdline", ptr->d_name);
		fp = fopen(path, "r");
		if (!fp)
			continue;

		len = fread(buf, 1, 49, fp);
		if (len < 0) {
			fclose(fp);
			continue;
		}

		if (strstr(buf, "audiobox")) {
			AUD_DBG("AudioBox have start\n");
			goto done;
		}

		fclose(fp);
	}
	AUD_DBG("Restart audiobox\n");
	system("audiobox &");
done:
	closedir(dir);
	return 0;
}

int audio_stop_service(void)
{
	int cmd = AB_STOP_SERVICE;
	int result = 0;
	int ret;

	AB_EVENT(sc, &cmd, NULL, NULL, &result);
	ret = audiobox_rpc_call_scatter(AUDIOBOX_RPC_BASE, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

static void audio_get_rpcname_by_handle(char *rpcname, int handle)
{
	char devname[32];
	int ret;
	
	if (!rpcname)
		return;

	ret = audio_get_devname(handle, devname);
	if (ret < 0)
		return;

	sprintf(rpcname, "%s-%s", AUDIOBOX_RPC_BASE, devname);
	AUD_DBG("RPC NAME %s\n", rpcname);
}

static void audio_get_rpcname_by_dev(char *rpcname, const char *dev)
{
	if (!rpcname || !dev)
		return;
	
	sprintf(rpcname, "%s-%s", AUDIOBOX_RPC_BASE, dev);
	AUD_DBG("RPC NAME %s\n", rpcname);
}

int audio_get_format(const char *dev, audio_fmt_t *dev_attr)
{
	int cmd = AB_GET_DEV_ATTR;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int ret;

	if (!dev || !dev_attr)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, dev_attr, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_set_format(const char *dev, audio_fmt_t *dev_attr)
{
	int cmd = AB_SET_DEV_ATTR;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int ret;

	if (!dev || !dev_attr)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, dev_attr, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;


	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_master_volume(const char *dev, ...)
{
	int cmd = AB_GET_DEV_VOLUME;
	char rpcname[64];
	char devname[64];
	int volume;
	int result = 0;
	int ret;

	if (!dev)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, &volume, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return ((result < 0) ? result : volume);
}

int audio_set_master_volume(const char *dev, int volume, ...)
{
	int cmd = AB_SET_DEV_VOLUME;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int ret;

	if (!dev)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, &volume, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_dev_module(const char *dev)
{
	int cmd = AB_GET_DEV_MODULE;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int module;
	int ret;

	if (!dev)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, &module, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return ((result < 0) ? result : module);
}

int audio_set_dev_module(const char *dev, int module)
{
	int cmd = AB_SET_DEV_MODULE;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int ret;

	if (!dev)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, &module, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_enable_dev(const char *dev)
{
	int cmd = AB_ENABLE_DEV;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int ret;

	if (!dev)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, NULL, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_disable_dev(const char *dev)
{
	int cmd = AB_DISABLE_DEV;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int ret;

	if (!dev)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT(sc, &cmd, devname, NULL, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_channel_codec(const char *dev, audio_fmt_t *chn_fmt, int flag)
{
	int cmd = AB_GET_CHANNEL;
	char rpcname[64];
	char devname[64];
	int result = 0;
	int ret = 0;
	int priority = (flag & PRIORITY_MASK) >> PRIORITY_BIT;

	if (!dev)
		return -1;

	if (priority < CHANNEL_BACKGROUND || priority > CHANNEL_FOREGROUND)
		return -1;

	memset(devname, 0, 64);
	sprintf(devname, "%s", dev);
	audio_get_rpcname_by_dev(rpcname, devname);
	AB_EVENT_RESV(sc, &cmd, devname, &flag, &result, chn_fmt);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;
	
	ret = audio_init_logchn(devname, chn_fmt, *(int *)AB_GET_RESULT(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_channel(const char *dev, audio_fmt_t *chn_fmt, int flag)
{
	#if 0
	audio_chn_fmt_t chn_fmt;
		
	chn_fmt.codec_type = AUDIO_CODEC_PCM;
	chn_fmt.channels= fmt->channels;
	chn_fmt.bitwidth= fmt->bitwidth;
	chn_fmt.samplingrate= fmt->samplingrate;
	chn_fmt.sample_size= fmt->sample_size;
	
	return audio_get_channel_codec(dev, &chn_fmt, flag);
	#else
	if(chn_fmt)
		chn_fmt->codec_type = AUDIO_CODEC_PCM;
	return audio_get_channel_codec(dev, chn_fmt, flag);
	#endif
}

int audio_put_channel(int handle)
{
	int cmd = AB_PUT_CHANNEL;
	char rpcname[64];
	int result = 0;
	int ret;

	if (handle < 0)
		return -1;
	
	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, NULL, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;
	
	ret = audio_release_logchn(handle);
	if (ret < 0)
		return -1;
	AUD_ERR("audio put channel:%d ok\n", handle);
	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_chn_fmt(int handle, audio_chn_fmt_t *chn_fmt)
{
	int cmd = AB_GET_CHN_FMT;
	char rpcname[64];
	int result = 0;
	int ret;

	if (handle < 0 || !chn_fmt)
		return -1;

	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, chn_fmt, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_set_chn_fmt(int handle, audio_chn_fmt_t *chn_fmt)
{
	int cmd = AB_SET_CHN_FMT;
	char rpcname[64];
	int result = 0;
	int ret;

	if (handle < 0 || !chn_fmt)
		return -1;

	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, chn_fmt, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	ret = audio_reset_logchn_by_id(handle, chn_fmt);
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_mute(int handle)
{
	int cmd = AB_GET_CHN_MUTE;
	char rpcname[64];
	int mute = 0;
	int result = 0;
	int ret;

	if (handle < 0)
		return -1;
	
	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, &mute, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return ((result < 0) ? result : mute);
}

int audio_set_mute(int handle, int mute)
{
	int cmd = AB_SET_CHN_MUTE;
	char rpcname[64];
	int result = 0;
	int ret;

	if (handle < 0)
		return -1;
	
	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, &mute, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_volume(int handle)
{
	int cmd = AB_GET_CHN_VOLUME;
	char rpcname[64];
	int volume = 0;
	int result = 0;
	int ret;

	if (handle < 0)
		return -1;
	
	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, &volume, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return ((result < 0) ? result : volume);
}

int audio_set_volume(int handle, int volume)
{
	int cmd = AB_SET_CHN_VOLUME;
	char rpcname[64];
	int result = 0;
	int ret;

	if (handle < 0)
		return -1;
	
	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, &volume, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_test_frame(int handle, struct fr_buf_info *buf)
{
	int ret;
	char frname[32];

	if (!buf)
		return -1;

	ret = audio_get_frname(handle, frname); 
	if (ret < 0)
		return -1;

	return fr_test_new_by_name(frname, buf);
}

int audio_get_frame(int handle, struct fr_buf_info *buf)
{
	int ret;
	char frname[32];

	if (!buf)
		return -1;
	
	ret = audio_get_frname(handle, frname);	
	if (ret < 0)
		return -1;
reget:
	/* Maybe capture server have not push data to frbuf */
	ret = fr_get_ref_by_name(frname, buf);
	if (ret == FR_ENOBUFFER)
		goto reget;

	return ret;
}

int audio_put_frame(int handle, struct fr_buf_info *buf)
{
	if (!buf)
		return -1;
	
	return fr_put_ref(buf);
}

int audio_read_frame(int handle, char *buf, int size)
{
	struct fr_buf_info frbuf;
	char frname[32];
	int ret;
	int len;

	int cmd = AB_READ_CHN_FRAME;
	char rpcname[64];
	int result = 0;

	if (size <= 0 || !buf)
		return -1;
	
	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, NULL, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	ret = audio_get_fr(handle, &frbuf);
	if (ret < 0)
		return -1;

	ret = audio_get_frname(handle, frname);
	if (ret < 0)
		return -1;

retest:
	ret = fr_get_ref_by_name(frname, &frbuf);
	if (ret < 0)
		goto retest;
	
	if (frbuf.size > size)
		AUD_ERR("Actual frame size(%d) greater than bufsize(%d)\n", frbuf.size, size);
	
	len = min(frbuf.size, size);	
	memcpy(buf, frbuf.virt_addr, len);	
	fr_put_ref(&frbuf);
	
	ret = audio_put_fr(handle, &frbuf);
	if (ret < 0)
		return -1;
	//AUD_DBG("audio_read_frame 0x%p, len %d\n", frbuf.virt_addr, len);
	return len;
}

int audio_write_frame(int handle, char *buf, int size)
{
	struct fr_buf_info frbuf;
	char frname[32];
	int ret;
	int len;
	int frsize;
	int writed = 0;

	if (!buf)
		return -1;
	
	ret = audio_get_frname(handle, frname);
	if (ret < 0)
		return -1;
	
	while (size > 0) {
		fr_INITBUF(&frbuf, NULL);
		ret = fr_get_buf_by_name(frname, &frbuf);
		if (ret < 0)
			return -1;

		frsize = audio_get_frsize(handle);
		//if (frsize < size)
		//	AUD_ERR("Actual frame size less than bufsize: %d==%d\n", frsize, size);

		len = min(frsize, size);
		frbuf.size = len;
		AUD_DBG("audio_write_frame addr 0x%x, len %d\n", frbuf.phys_addr, len);
		memcpy(frbuf.virt_addr, buf + writed, len);
		fr_put_buf(&frbuf);
		size -= len;
		writed += len;
	}

	return writed;
}

int audio_get_chn_size(audio_chn_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;

	bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	return (fmt->channels * (bitwidth >> 3) * fmt->sample_size);
}

int audio_get_codec_ratio(audio_chn_fmt_t *fmt)
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

int audio_enable_aec(int handle, int enable)
{
	AUD_ERR("audio_enable_aec absoleted, please use audio_set_dev_module()\n");
	return -1;
}

