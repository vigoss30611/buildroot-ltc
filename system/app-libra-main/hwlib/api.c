/*
 * =====================================================================================
 *
 *       Filename:  api.c
 *
 *    Description:  qsdk to spv api
 *
 *        Version:  1.0
 *        Created:  05/04/16 15:02:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <errno.h>
#include "camera_spv.h"
#include "config_camera.h"
#include "media_play.h"
#include "demuxer_interface.h"
#include "common_inft_cfg.h"
#include "gps.h"
#include <sys/reboot.h>
#include "spv_debug.h"
#include <qsdk/demux.h>
#include <qsdk/audiobox.h>

#define GSENSOR_ENABLE "/sys/class/misc/%s/attribute/enable"
#define LCD_BACKLIGHT_MAX 255
#define BATTERY_FULL			1
#define BATTERY_DISCHARGING		2
#define BATTERY_CHARGING		3

#define SD_FORMAT_OK	0
#define SD_FORMAT_ERROR	-1

#define SD_MOUNTED	1
#define SD_UNMOUNTED	0
char const *const BACKLIGHT_PATH = 
		"/sys/class/backlight/lcd-backlight/brightness";
char const *const BACKLIGHT_MAX = 
		"/sys/class/backlight/lcd-backlight/max_brightness";
char const *const FB0_PATH = "/dev/fb0";

int get_audio_handle(void)
{
	int handle;
	char pcm_name[32] = "default";
	audio_fmt_t fmt = {
		.channels = 2,
		.bitwidth = 32,
		.samplingrate = 48000,
		.sample_size = 512,
	};

	handle = audio_get_channel(pcm_name, &fmt, CHANNEL_BACKGROUND);
	if (handle < 0) {
		ERROR("get audio channel err:%d\n", handle);
		return -1;
	}

	return handle;
}

//audiobox_set_volume(struct event_scatter * event)
int set_volume(int volume)
{
	INFO("[LIBRA_MAIN] ENTER ******%s: volume %d**********\n", __func__, volume);
	return 0;
	int handle, ret;

	handle = get_audio_handle();
	if (handle < 0) {
		ERROR("[libramain] %s get audio handle failed\n", __func__);
		return -1;
	}

	ret = audio_set_volume(handle, volume);
	if (ret < 0) {
		ERROR("[libramain] set volume failed\n");
	}

	audio_put_channel(handle);
	return ret;
}

static int safe_read(int fd, void *buf, size_t count)
{
	int result = 0, res;

	while (count > 0) {
		if ((res = read(fd, buf, count)) == 0) 
			break;
		if (res < 0)
			return result > 0 ? result : res;
		count -= res;
		result += res;
		buf = (char *)buf + res;
	}

	return result;
}

static int get_buffer_size(audio_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;
	
	bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	return (fmt->channels * (bitwidth >> 3) * fmt->sample_size);
}

int play_audio(char *filename)
{
	INFO("[LIBRA_MAIN] ENTER ******%s**********\n", __func__);
	int handle;
	int len, fd, bufsize, ret;
	char pcm_name[32] = "default";
	char audiobuf[8192];
	audio_fmt_t real;

	handle = get_audio_handle();
	if (handle < 0) {
		ERROR("get audio handle failed\n");
		return -1;
	}

	ret = audio_get_format(pcm_name, &real);
	if (ret < 0) {
		ERROR("get format err:%d\n", ret);
		return -1;
	}
	bufsize = get_buffer_size(&real);
	INFO("[libramain] bufsize : %d\n", bufsize);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		ERROR("open filename : %s failed\n", filename);
		return -1;
	}

	do {
		len = safe_read(fd, audiobuf, bufsize);
		if (len < 0)
			break;

		ret = audio_write_frame(handle, audiobuf, len);
		if (ret < 0)
			break;
	} while (len == bufsize);

	close(fd);
	audio_put_channel(handle);

	return 0;
}

int stream_open(const char *file_name, DemuxerContext *demuxer_context)
{
	struct demux_t *demux;
	AVStream *av_stream;
	int ret = 0, i;

	demux = malloc(sizeof(struct demux_t));
	if (demux == NULL) {
		ERROR("demux allocate failed\n");
		return -1;
	}
	memset(demux, 0, sizeof(struct demux_t));

	av_register_all();
	ret = avformat_open_input(&demux->av_format_dtx, file_name, NULL, NULL);
	if (ret < 0) {
		ERROR("demux open file:%d failed\n", file_name);
		free(demux);
		return ret;
	}

	ret = avformat_find_stream_info(demux->av_format_dtx, NULL);
	if (ret < 0) {
		ERROR("demux find stream %s failed\n", file_name);
		free(demux);
		return ret;
	}

	demux->video_index = av_find_best_stream(demux->av_format_dtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	demux->audio_index = av_find_best_stream(demux->av_format_dtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

	if (demux->video_index >= 0) {
		av_stream = demux->av_format_dtx->streams[demux->video_index];
		if (av_stream->codec->codec_id == AV_CODEC_ID_H264) {
			demux->bsfc = av_bitstream_filter_init("h264_mp4toannexb");
			if (demux->bsfc == NULL) {
				ERROR("could not open h264_mp4toannexb\n");
				return -2;
			}
		} else if (av_stream->codec->codec_id == AV_CODEC_ID_H265) {
			demux->bsfc = av_bitstream_filter_init("hevc_mp4toannexb");
			if (demux->bsfc == NULL) {
				ERROR("could not open hevc_mp4toannexb\n");
				return -2;
			}
		} else {
			ERROR("could not suopport codec id:%8x now\n", av_stream->codec->codec_id);
			return -1;
		}
	}
	
	*demuxer_context = (DemuxerContext)demux;

	return 0;
}

int stream_get_info(DemuxerContext demuxer_context, StreamInfo *info)
{
	struct demux_t *demux = (struct demux_t *)demuxer_context;

	if (info == NULL) {
		ERROR("stream info is NULL\n");
		return -1;
	}

	if (demux->av_format_dtx == NULL) {
		ERROR("stream av format is NULL\n");
		return -1;
	}

	memset(info, 0, sizeof(*info));
	info->nb_streams = demux->av_format_dtx->nb_streams;
	info->file_size = avio_size(demux->av_format_dtx->pb);
	info->duration = demux->av_format_dtx->duration;
	INFO("video duration :%"PRId64" \n", info->duration);

	/* other info*/
	/* TODO */

	return 0;
}

int stream_close(DemuxerContext demuxer_context)
{
	struct demux_t *demux = (struct demux_t *)demuxer_context;

	if (demux->bsfc) {
		av_bitstream_filter_close(demux->bsfc);
	}
	avformat_close_input(&demux->av_format_dtx);
	free(demux);

	return 0;
}

//lcd_set_backlight(int brightness)
int set_backlight(int brightness)
{
	INFO("SPI_GUI api %s %d\n", __func__, brightness);
	int ret, fd;
	int max_brightness;
	int bytes = 0;
	char buffer[20];

	fd = open(BACKLIGHT_MAX, O_RDONLY);
	if (fd < 0) {
		ERROR("cannot open max_brightness\n");
		return -1;
	}

	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0) {
		ERROR("cannot read max_brightness\n");
		return -1;
	}
	max_brightness = LCD_BACKLIGHT_MAX < atoi(buffer) ? LCD_BACKLIGHT_MAX : atoi(buffer);
	INFO("can set backlight max_brightness : %d\n", max_brightness);
	close(fd);

	if (brightness < 0 || brightness > max_brightness) {
		ERROR("set brightness:%d > max_brightness:%d\n", brightness, max_brightness);
		brightness = max_brightness;
	}

	fd = open(BACKLIGHT_PATH, O_RDWR);
	if ( fd < 0) {
		ERROR("cannot open BACKLIGHT_PATH.\n");
		return -errno;
	}

	bytes = sprintf(buffer, "%d\n", brightness);
	ret = write(fd, buffer, bytes);
	if (ret < 0) {
		ERROR("write BACKLIGHT_PATH failed.\n");
		return -errno;
	}

	close(fd);

	return 0;
}

//system_set_led(const char *led, int time_on, int time_off, int blink_on)
int fill_light_switch(int sw)
{
	INFO("[LIBRA_MAIN] %s status : %d\n", __func__, sw);

	static char cmd[] = "echo 0 > /sys/class/leds/led0/brightness";
	cmd[5] = sw ? '1' : '0';
	system(cmd);

	return 0;
}

//gps_set_nmea(char *buf, int len)
int Inft_gps_setData(GPS_Data *data)
{
	return 0;
}

void infotm_shutdown(void)
{
	reboot(RB_POWER_OFF);
}

int check_battery_capacity(void)
{

	int ret, fd;
	char buffer[20];

	fd = open("/sys/class/power_supply/battery/voltage_now", O_RDONLY);
	if (fd < 0) {
		ERROR("cannot open battery capacity\n");
		return -errno;
	}

	ret = read(fd, buffer, 20);
	if (ret < 0) {
		ERROR("read battery capacity failed\n");
		close(fd);
		return -errno;
	}

//	INFO("[LIBRA_MAIN] %s, capacity %d\n", __func__, atoi(buffer));
	close(fd);

	return 3800;

	return atoi(buffer);
}

int is_charging(void)
{
	int ret, fd;
	char buffer[20];

	fd = open("/sys/class/power_supply/battery/status", O_RDONLY);
	if (fd < 0) {
		ERROR("cannot open battery status\n");
		return -errno;
	}

	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0) {
		ERROR("read battery status failed\n");
		close(fd);
		return -errno;
	}

	close(fd);

//	INFO("[LIBRA_MAIN] battery status : %s\n", buffer);
	if (strcasestr(buffer, "full")) {
		ret = BATTERY_FULL;
	} else if (strcasestr(buffer, "discharging")) {
		ret = BATTERY_DISCHARGING;
	} else if (strcasestr(buffer, "charging")) {
		ret = BATTERY_CHARGING;
	} else {
		ERROR("unknow battery status\n");
		ret = -1;
	}

	return ret;
}

int check_wakeup_state(void)
{
	INFO("[LIBRA_MAIN] %s\n", __func__);

	int ret, fd;
	char buffer[20];

	fd = open("/sys/class/wakeup/state", O_RDONLY);
	if (fd < 0) {
		ERROR("cannot open wakeup state\n");
		return -errno;
	}

	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0) {
		ERROR("read wakeup state failed\n");
		close(fd);
		return -errno;
	}

	close(fd);

	system("echo 0 > /sys/class/wakeup/state");

	return atoi(buffer);
}

int enable_gsensor(int status)
{
	INFO("[LIBRA_MAIN] %s status : %d\n", __func__, status);

	int ret, fd;
	char buffer[16] = {0};
	char cmd[128] = {0};
	char sensor_node[128] = {0};

	char *sensor_model = getenv("GSENSOR_MODEL");
	INFO("[LIBRA_MAIN] sensor_model : %s\n", sensor_model);
	if (sensor_model == NULL) {
		ERROR("gsensor model not found\n");
		return -1;
	}

	sprintf(sensor_node, GSENSOR_ENABLE, sensor_model);
	sprintf(cmd, "echo %d > %s", status, sensor_node);
	system(cmd);

	fd = open(sensor_node, O_RDONLY);
	if (fd < 0) {
		ERROR("cannot open gsensor enable node\n");
		return -errno;
	}

	ret = read(fd, buffer, 16);
	if (ret < 0) {
		ERROR("read gsensor enable state failed\n");
		close(fd);
		return -errno;
	}

	close(fd);

	INFO("[LIBRA_MAIN] read gsensor state : %d\n", atoi(buffer));
	if (atoi(buffer) == status)
		return 0;
	else
		return -1;
}



int get_volume(void)
{
	INFO("[LIBRA_MAIN] ENTER ******%s**********\n", __func__);
	int handle, volume;

	handle = get_audio_handle();
	if (handle < 0) {
		ERROR("[libramain] %s get audio handle failed\n", __func__);
		return -1;
	}

	volume = audio_get_volume(handle);
	if (volume < 0) {
		ERROR("[libramain] get volume: %d\n", volume);
	}

	audio_put_channel(handle);

	return volume;
}


void lcd_blank(int blank)
{
	int ret, fd;

	fd = open(FB0_PATH, O_RDWR);
	if (fd < 0) {
		ERROR("cannot open fb0\n");
		return;
	}

	ret = ioctl(fd, FBIOBLANK, blank?FB_BLANK_POWERDOWN:FB_BLANK_UNBLANK);
	if (ret < 0) {
		ERROR("ioctl(): blank error.\n");
		close(fd);
		return;
	}
	close(fd);

	return;
}

//gps_set_nmea(char *buf, int len)
int gps_request(gps_data *gd)
{
	return 0;
}

//system_set_led(const char *led, int time_on, int time_off, int blink_on)
int indicator_led_switch(int sw)
{
	INFO("[LIBRA_MAIN] %s status : %d\n", __func__, sw);

	static char cmd[] = "echo 0 > /sys/class/leds/led0/brightness";
	cmd[5] = sw ? '1' : '0';
	system(cmd);

	return 0;
}

//lcd_get_brightness(void)
int get_backlight(void)
{
	int ret, fd;
	char buffer[20];

	fd = open(BACKLIGHT_PATH, O_RDWR);
	if (fd < 0) {
		ERROR("cannot open BACKLIGHT_PATH.\n");
		return -errno;
	}

	ret = read(fd, buffer, 5);
	if (ret < 0) {
		ERROR("read BACKLIGHT_PATH failed.\n");
		return -errno;
	}

	close(fd);

	return atoi(buffer);
}

//system_set_led(const char *led, int time_on, int time_off, int blink_on)
void set_indicator_led_flicker(int times)
{
	INFO("[LIBRA_MAIN] %s times = %d\n", __func__, times);
	int i;
	for (i = 0; i < times; i++) {
		indicator_led_switch(1);
		usleep(200 * 1000);
		indicator_led_switch(0);
		usleep(200 * 1000);
	}
	
	return;
}


