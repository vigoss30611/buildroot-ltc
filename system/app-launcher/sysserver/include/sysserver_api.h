#ifndef __SYSSERVER_API_H__
#define __SYSSERVER_API_H__

#define BATTERY_FULL 		1
#define BATTERY_DISCHARGING 	2
#define BATTERY_CHARGING 	3

int get_backlight(void);
int set_backlight(int brightness);
void set_lcd_blank(int blank);
int check_battery_capacity(void);
int is_charging(void);
int is_adapter(void);
int sd_format(char *mount_point);
int sd_status(disk_info_t *info);
int check_wakeup_state(void);
void enable_gsensor(int sw);
int set_volume(int volume);
int get_volume(void);
int play_audio(char *filename);
void set_led(const char *led, int time_on, int time_off, int blink_on);

#endif
