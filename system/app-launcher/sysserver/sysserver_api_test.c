#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <qsdk/sys.h>
#include <sys/reboot.h>

#include "sysserver_api.h"

int main(int argc, char *argv[])
{
    int ret;
    disk_info_t info;
    
    if (!strncmp(argv[1], "get_backlight")) {
	printf("get_backlight %d \n", get_backlight());
    } else if (!strncmp(argv[1], "set_backlight")) {
	printf("set_backlight %d \n", atoi(argv[2]));
	set_backlight(atoi(argv[2]));
    } else if (!strncmp(argv[1], "set_lcd_blank")) {
	printf("set_lcd_blank %d \n", atoi(argv[2]));
	set_lcd_blank(atoi(argv[2]));
    } else if (!strncmp(argv[1], "check_battery_capacity")) {
	printf("check_battery_capacity %d v. \n", check_battery_capacity());
    } else if (!strncmp(argv[1], "is_charging")) {
	printf("is_charging %d \n", is_charging());
    } else if (!strncmp(argv[1], "is_adapter")) {
	printf("is_adapter %d \n", is_adapter());
    } else if (!strncmp(argv[1], "sd_format")) {
	printf("sd_format %s \n", argv[2]);
	sd_format(argv[2]);
    } else if (!strncmp(argv[1], "sd_status")) {
	info.name = "/dev/mmcblk0"; 
	ret =  sd_status(&info);
	if(ret) 
	    printf("get disk %s status failed.\n", info.name);
	else
	    printf("get disk %s status ok. inserted :%d. mounted:%d.\n", 
		    info.name, info.inserted, info.mounted);
    } else if (!strncmp(argv[1], "check_wakeup_state")) {
	check_wakeup_state();
    } else if (!strncmp(argv[1], "enable_gsensor")) {
	printf("enable_gsensor %d \n", atoi(argv[2]));
	enable_gsensor(atoi(argv[2]));
	reboot(RB_POWER_OFF);
    } else if (!strncmp(argv[1], "set_volume")) {
	printf("set_volume %d \n", atoi(argv[2]));
	set_volume(atoi(argv[2]));
    } else if (!strncmp(argv[1], "get_volume")) {
	printf("get_volume %d \n", get_volume());
    } else if (!strncmp(argv[1], "play_audio")) {
	printf("play_audio %s \n", argv[2]);
	play_audio(argv[2]);
    } else if (!strncmp(argv[1], "set_led")) {
	if (argc == 4) {
	    printf("set led %s %d\n", argv[2], atoi(argv[3]));
	    set_led(argv[2], atoi(argv[3]), 0, 0);
	} else {
	    printf("set led %s, time_on %d ms, time_off %d ms \n", argv[2], atoi(argv[3]), atoi(argv[4]));
	    set_led(argv[2], atoi(argv[3]), atoi(argv[4]), 1);
	}
    }
    exit(0);
}
