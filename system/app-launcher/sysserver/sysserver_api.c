#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <qsdk/sys.h>
#include <qsdk/audiobox.h>

#include "sysserver_api.h"

int get_backlight(void)
{
    return system_get_backlight(); // value or -1
}

int set_backlight(int brightness)
{
    return system_set_backlight(brightness); // value or -1
}

void set_lcd_blank(int blank)
{
    system_enable_backlight(!blank);
}

int check_battery_capacity(void)
{
    battery_info_t bati;
    system_get_battry_info(&bati);
    return bati.voltage_now;
}

int is_charging(void)
{
    int ret = -1;
    battery_info_t bati;
    system_get_battry_info(&bati);

    if (strcasestr(bati.status, "full")) {
	ret = BATTERY_FULL;
    } else if (strcasestr(bati.status, "discharging")){
	ret = BATTERY_DISCHARGING;
    } else if (strcasestr(bati.status, "charging")){
	ret = BATTERY_CHARGING;
    }

    return ret;
}

int is_adapter(void)
{
    ac_info_t aci;
    system_get_ac_info(&aci);
    return aci.online;
}

int sd_format(char *mount_point)
{
    printf("sd_format %s \n", mount_point);
    return system_format_storage(mount_point); // 0 success, other fail
}

int sd_status(disk_info_t *info)
{

    return system_get_disk_status(info);
}

int check_wakeup_state(void)
{
    enum boot_reason reason;
    reason = system_get_boot_reason();
    switch(reason) {
	case BOOT_REASON_ACIN:
	    printf("power up by ac in\n");
	    break;
	case BOOT_REASON_EXTINT:
	    printf("power up by ac extern irq\n");
	    break;
	case BOOT_REASON_POWERKEY:
	    printf("power up by press power key\n");
	    break;
	case BOOT_REASON_UNKNOWN:
	default:
	    printf("power up reason unknown\n");
    }
    return (int)reason;
}

void enable_gsensor(int sw)
{
    enum externel_device_type dev_type = EXT_DEVICE_GSENSOR;
    char *dev_name = "mir3da";
    int ret = system_enable_externel_powerup(dev_type, dev_name, 1);

    if(ret == 0)
	printf("externel device boot cpu enable ok!\n");
    else
	printf("externel device boot cpu enable failed!\n");
}

int get_audio_handle(void)
{
    int handle;
    char pcm_name[32] = "default";
    audio_fmt_t fmt = {
	.channels = 2,
	.bitwidth = 32,
	.samplingrate = 16000,
	.sample_size = 1024,
    };

    handle = audio_get_channel(pcm_name, &fmt, CHANNEL_BACKGROUND);
    if (handle < 0) {
	printf("get audio channel err:%d\n", handle);
	return -1;
    }

    return handle;
}

int set_volume(int volume)
{
    int handle, ret;

    handle = get_audio_handle();
    if (handle < 0) {
	printf("%s get audio handle failed\n", __func__);
	return -1;
    }

    ret = audio_set_volume(handle, volume);
    if (ret < 0) {
	printf("set volume failed\n");
    }

    audio_put_channel(handle);
    return ret;

}

int get_volume(void)
{
    int handle, volume;

    handle = get_audio_handle();
    if (handle < 0) {
	printf("%s get audio handle failed\n", __func__);
	return -1;
    }

    volume = audio_get_volume(handle);
    if (volume < 0) {
	printf("get volume: %d\n", volume);
    }

    audio_put_channel(handle);

    return volume;
}

static int get_buffer_size(audio_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;
	
	bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	return (fmt->channels * (bitwidth >> 3) * fmt->sample_size);
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

int play_audio(char *filename)
{
    int handle;
    int len, fd, bufsize, ret;
    char pcm_name[32] = "default";
    char audiobuf[8192];
    audio_fmt_t real;

    handle = get_audio_handle();
    if (handle < 0) {
	printf("get audio handle failed\n");
	return -1;
    }

    ret = audio_get_format(pcm_name, &real);
    if (ret < 0) {
	printf("get format err:%d\n", ret);
	return -1;
    }
    bufsize = get_buffer_size(&real);
    printf("bufsize : %d\n", bufsize);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
	printf("open filename : %s failed\n", filename);
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

void set_led(const char *led, int time_on, int time_off, int blink_on)
{
    system_set_led(led, time_on, time_off, blink_on);
}


