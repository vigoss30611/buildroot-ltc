#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include "sysset.h"

int led_test(int argc, char** argv)
{
	char led_name[32];
	int ret = 0, value = 0;
	int val = 0;
	int time_on = 0, time_off = 0, blink_on = 0;

	if (argc > 1) {
		if (0 == strcmp(argv[1], "list"))
		{
			sys_info("display leds : %s\n ",system_get_ledlist());

			return 0;
		} else {
			strcpy(led_name, argv[1]);
			sys_info("set led name %s\n", led_name);
		}
	}

	int option_index;
	char const led_short_options[] = "n:f:m:";
	struct option led_long_options[] = {
		{ "timeon", 0, NULL, 'n'},
		{ "timeoff", 0, NULL, 'f'},
		{ "mode", 0, NULL, 'm'},
		{ 0, 0, 0, 0},
	};
	while ((val = getopt_long(argc-1, argv+1, led_short_options,
				led_long_options, &option_index)) != -1) {
		switch(val) {
			case 'n':
				time_on = strtol(optarg, NULL, 0);
				break;
			case 'f':
				time_off = strtol(optarg, NULL, 0);
				break;
			case 'm':
				blink_on = strtol(optarg, NULL, 0);
				break;
			default:
				sys_err("Invalid argument\n");
				return -1;
		}
	}

	sys_info("led_name %s, time_on %d, time_off %d, blink_on %d\n", led_name, time_on, time_off, blink_on);
	ret = system_set_led(led_name, time_on, time_off, blink_on);
	if (ret < 0) {
		sys_err("set %s failed\n", led_name);
		return -1;
	}

	return 0;
}
