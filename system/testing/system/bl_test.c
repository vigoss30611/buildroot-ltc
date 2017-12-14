#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include "sysset.h"

int bl_test(int argc, char** argv)
{
	int ret = 0, brightness = 0;
	if (argc > 1) {
		if (0 == strcmp(argv[1], "set")) 
		{
			brightness = atoi(argv[2]);
			system_enable_backlight(1);
			ret = system_set_backlight(brightness);

			if (ret < 0) {
				sys_err("system set backlight fail.\n");
				return -1;
			}

			sys_info("set backlight %d success\n", brightness);

		} 
		else if (0 == strcmp(argv[1], "get"))
		{
			brightness = system_get_backlight();

			sys_info("get system backlinght, now brightness = %d\n", brightness);
		}
		else
		{
			brightness = system_get_backlight();

			sys_info("get system backlinght, now brightness = %d\n", brightness);
		}
	} else {
		brightness = system_get_backlight();

		sys_info("get system backlinght, now brightness = %d\n", brightness);
	}

	return 0;
}
