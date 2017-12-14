#include <stdio.h>
#include <unistd.h>
#include "sysset.h"

void usage(char *cmd)
{
	printf(
			("Usage: %s [OPTION]... [FILE]...\n"
			 "bl				Test system backlight\n"
			 "\tset					set system brightness\n"
			 "\tget					get system brightness\n"
			 "led				Test system led\n"
			 "\tlist				display led\n"
			 "\t-n, --timeon		time on for led\n"
			 "\t-f, --timeoff		time off for led\n"
			 "\t-m, --mode			blink on for led\n"
			 "wdt				Test system watchdog\n"
			 "\tstart				Start watchdog test\n"
			 "\t-d, --duration=		Specify test duration(ms)\n"
			 "\t-t, --timeout=  	Specify wdt timeout time(ms)\n"
			 "\t-f, --feedtime= 	Specify feeding dog time interval(ms)\n"
			 "\tstop				Stop watchdog test\n"
			 "ac				Get system AC info\n"
			 "batt				Get battery status and voltage\n"
			 "soc				Get soc type and mac info\n"
			 "\ttype				Get soc type info\n"
			 "\tmac					Get soc mac info\n"
			 "boot				Get system boot reason\n"
			 "storage			Storage Test\n"
			 "\tstatus				Get storage status\n"
			 "\tinfo				Get storage size info\n"
			 "\tformat				storage format\n"
			 "process			System process test\n"
			 "\tcheck				Get the process pid if exist\n"
			 "\tkill				Kill the process\n"
			 "setting			Setting test\n"
			 "\tstring\n"
			 "\tint\n"
			 "\tdouble\n"
			 "\trmkey\n"
			 "\trmall\n"
			 ), cmd);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		usage(argv[0]);
		return -1;
	}

	if (!strcmp(argv[1], "bl"))
		bl_test(--argc, ++argv);
	else if (!strcmp(argv[1], "led"))
		led_test(--argc, ++argv);
	else if (!strcmp(argv[1], "gpio"))
		gpio_test(--argc, ++argv);
	else if (!strcmp(argv[1], "wdt"))
		wdt_test(--argc, ++argv);
	else if (!strcmp(argv[1], "ac"))
		ac_test(--argc, ++argv);
	else if (!strcmp(argv[1], "batt"))
		batt_test(--argc, ++argv);
	else if (!strcmp(argv[1], "soc"))
		soc_test(--argc, ++argv);
	else if (!strcmp(argv[1], "boot"))
		boot_test(--argc, ++argv);
	else if (!strcmp(argv[1], "storage"))
		storage_test(--argc, ++argv);
	else if (!strcmp(argv[1], "process"))
		process_test(--argc, ++argv);
	else if (!strcmp(argv[1], "setting"))
		setting_test(--argc, ++argv);
	else
		usage(argv[0]);
}
