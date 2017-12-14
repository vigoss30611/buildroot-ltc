#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <log.h>
#include <qsdk/event.h>
#include <fr/libfr.h>
#include <audiobox.h>
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
	char channel[32];
	int ret;
	
	if (!rpcname)
		return;

	ret = audio_get_chnname(handle, channel);	
	if (ret < 0)
		return;

	sprintf(rpcname, "%s-%s", AUDIOBOX_RPC_BASE, channel);
	AUD_DBG("RPC NAME %s\n", rpcname);
}

static void audio_get_rpcname_by_channel(char *rpcname, const char *channel)
{
	if (!rpcname || !channel)
		return;
	
	sprintf(rpcname, "%s-%s", AUDIOBOX_RPC_BASE, channel);
	AUD_DBG("RPC NAME %s\n", rpcname);
}

int audio_get_channel(const char *channel, audio_fmt_t *fmt, int flag)
{
	int cmd = AB_GET_CHANNEL;
	char rpcname[64];
	char chnname[64];
	int result = 0;
	int ret = 0;
	int pid;
	int priority = (flag & PRIORITY_MASK) >> PRIORITY_BIT;

	if (!channel)
		return -1;

	if (priority < CHANNEL_BACKGROUND || priority > CHANNEL_FOREGROUND)
		return -1;

	memset(chnname, 0, 64);
	sprintf(chnname, "%s", channel);
	audio_get_rpcname_by_channel(rpcname, chnname);
	pid = getpid();
	AB_EVENT_RESV_PID(sc, &cmd, chnname, &flag, &result, fmt, &pid);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;
	
	ret = audio_init_logchn(chnname, fmt, *(int *)AB_GET_RESULT(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
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
	AUD_ERR("audio put channel ok\n");
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


	if (!buf)
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
		//AUD_DBG("audio_write_frame addr 0x%x, len %d\n", frbuf.phys_addr, len);
		memcpy(frbuf.virt_addr, buf + writed, len);
		fr_put_buf(&frbuf);
		size -= len;
		writed += len;
	}

	return writed;
}

int audio_get_format(const char *channel, audio_fmt_t *fmt)
{
	int cmd = AB_GET_FORMAT;
	char rpcname[64];
	char chnname[64];
	int result = 0;
	int ret;

	if (!channel || !fmt)
		return -1;

	memset(chnname, 0, 64);
	sprintf(chnname, "%s", channel);
	audio_get_rpcname_by_channel(rpcname, chnname);
	AB_EVENT(sc, &cmd, chnname, fmt, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_set_format(const char *channel, audio_fmt_t *fmt)
{
	int cmd = AB_SET_FORMAT;
	char rpcname[64];
	char chnname[64];
	int result = 0;
	int ret;

	if (!channel || !fmt)
		return -1;

	memset(chnname, 0, 64);
	sprintf(chnname, "%s", channel);
	audio_get_rpcname_by_channel(rpcname, chnname);
	AB_EVENT(sc, &cmd, chnname, fmt, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	ret = audio_reset_logchn_by_name(chnname, fmt);
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_get_mute(int handle)
{
	int cmd = AB_GET_MUTE;
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
	int cmd = AB_SET_MUTE;
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
	int cmd = AB_GET_VOLUME;
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
	int cmd = AB_SET_VOLUME;
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

int audio_get_master_volume(const char *channel, int dir)
{
	int cmd = AB_GET_MASTER_VOLUME;
	char rpcname[64];
	char chnname[64];
	int volume;
	int result = 0;
	int ret;

	if (!channel)
		return -1;

	memset(chnname, 0, 64);
	sprintf(chnname, "%s", channel);
	audio_get_rpcname_by_channel(rpcname, chnname);
	AB_EVENT_RESV(sc, &cmd, chnname, &volume, &result, &dir);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return ((result < 0) ? result : volume);
}

int audio_set_master_volume(const char *channel, int volume, int dir)
{
	int cmd = AB_SET_MASTER_VOLUME;
	char rpcname[64];
	char chnname[64];
	int result = 0;
	int ret;

	if (!channel)
		return -1;

	memset(chnname, 0, 64);
	sprintf(chnname, "%s", channel);
	audio_get_rpcname_by_channel(rpcname, chnname);
	AB_EVENT_RESV(sc, &cmd, chnname, &volume, &result, &dir);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}

int audio_enable_aec(int handle, int enable) {

	int cmd = AB_ENABLE_AEC;
	char rpcname[64];
	int result = 0;
	int ret;

	if (handle < 0)
		return -1;

	AUD_DBG("xym-debug: %s: enable = %d\n", __func__, enable);
	audio_get_rpcname_by_handle(rpcname, handle);
	AB_EVENT(sc, &cmd, &handle, &enable, &result);
	ret = audiobox_rpc_call_scatter(rpcname, sc,
			AB_EVENT_SIZE(sc));
	if (ret < 0)
		return -1;

	return *(int *)AB_GET_RESULT(sc);
}
