#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include "sysset.h"

void gpio_detect(void)
{
	sys_err("Start gpio request test\n");
	int i, index, ret, value = 0;
	for (i= 0; i < 130; i++) {
		index = i;
		ret = system_request_pin(index);
		if (ret < 0)
			continue;
		system_set_pin_value(index, 1);
		value = system_get_pin_value(index);
		sys_err("index %d, value %d\n", index, value);
		if (value == 1) {
			sys_err("\tindex %d, get value 1\n", index);
			system_set_pin_value(index, 0);
			value = system_get_pin_value(index);
			if (value == 0) {
				sys_err("\t\tindex %d, get value 0\n", index);
			}
		}
	}

	return;
}

int gpio_test(int argc, char** argv)
{
	int ret = 0, index = 0, value = 0;

//	gpio_detect();

	if (argc > 1) {
		if (0 == strcmp(argv[1], "request"))
		{
			index = atoi(argv[2]);
			ret = system_request_pin(index);
			if (ret < 0) {
				sys_err("GPIO %d request failed\n", index);
				return -1;
			}

			sys_info("GPIO %d request success\n", index);

			return 0;
		} else if (0 == strcmp(argv[1], "get"))
		{
			index = atoi(argv[2]);

			value = system_get_pin_value(index);

			sys_info("GPIO%d value %d\n", index, value);

			return 0;
		} else if (0 == strcmp(argv[1], "set"))
		{
			index = atoi(argv[2]);
			value = atoi(argv[3]);

			ret = system_set_pin_value(index, value);
			if (ret < 0) {
				sys_err("Set gpio %d value failed\n", index);
				return -1;
			}

			sys_info("GPIO%d set value success\n", index);

			return 0;
		} else {
			sys_err("please input sysset gpio request index\n");
			return -1;
		}
	}

}
