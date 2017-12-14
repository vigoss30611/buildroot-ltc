#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include "sysset.h"

int ac_test(int argc, char** argv)
{
	ac_info_t aci;

	system_get_ac_info(&aci);

	sys_err("Now system ac status:%d\n", aci.online);

	if (aci.online == 0) {
		sys_err("Now ac outline\n");
	} else {
		sys_err("Now ac inline\n");
	}

	return 0;
}

int batt_test(int argc, char** argv)
{
	battery_info_t batt;

	system_get_battry_info(&batt);

	sys_err("Battery status %s, voltage current %d\n", batt.status, batt.voltage_now);

	return 0;
}

int soc_test(int argc, char** argv)
{
	soc_info_t s_info;
	int ret = 0;

	if (0 == strcmp(argv[1], "type")) {
		ret = system_get_soc_type(&s_info);
		sys_err("The soc type: %d\n", ret);
	} else if (0 == strcmp(argv[1], "mac")) {
		ret = system_get_soc_mac(&s_info);
		sys_err("The soc mac: %d\n", ret);
	} else {
		ret = system_get_soc_type(&s_info);
		sys_err("The soc type: %d\n", ret);
		ret = system_get_soc_mac(&s_info);
		sys_err("The soc mac: %d\n", ret);
	}

	return 0;
}

int boot_test(int argc, char** argv)
{
	enum boot_reason reason;

	reason = system_get_boot_reason();

	switch(reason) {
		case BOOT_REASON_ACIN:
			sys_info("power up by ac in\n");
			break;
		case BOOT_REASON_EXTINT:
			sys_info("power up by ac extern irq\n");
			break;
		case BOOT_REASON_POWERKEY:
			sys_info("power up by press power key\n");
			break;
		case BOOT_REASON_UNKNOWN:
		default:
			sys_info("power up reason unknown\n");
			break;
	}

	return 0;
}
