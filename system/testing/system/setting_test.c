#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include "sysset.h"

int setting_test(int argc, char** argv)
{
	char owner[256], key[256], string[256], string_r[256];
	int d, d_r;
	double lf, lf_r;

	if (0 == strcmp(argv[1], "string")) {
		strcpy(owner, argv[2]);
		strcpy(key, argv[3]);
		strcpy(string, argv[4]);
		sys_info("Set %s %s %s\n", owner, key, string);
		setting_set_string(owner, key, string);
		sleep(1);
		setting_get_string(owner, key, string_r, 256);
		sys_info("Get %s %s %s\n", owner, key, string_r);
	} else if (0 == strcmp(argv[1], "int")) {
		strcpy(owner, argv[2]);
		strcpy(key, argv[3]);
		d = atoi(argv[4]);
		sys_info("Set %s %s %d\n", owner, key, d);
		setting_set_int(owner, key, d);
		sleep(1);
		d_r = setting_get_int(owner, key);
		sys_info("Get %s %s %d\n", owner, key, d_r);
	} else if (0 == strcmp(argv[1], "double")) {
		strcpy(owner, argv[2]);
		strcpy(key, argv[3]);
		lf = atof(argv[4]);
		sys_info("Set %s %s %lf\n", owner, key, lf);
		setting_set_double(owner, key, lf);
		sleep(1);
		lf_r = setting_get_double(owner, key);
		sys_info("Get %s %s %lf\n", owner, key, lf_r);
	} else if (0 == strcmp(argv[1], "rmkey")) {
		strcpy(owner, argv[2]);
		strcpy(key, argv[3]);
		sys_info("Rm %s %s\n", owner, key);
		setting_remove_key(owner, key);
	} else if (0 == strcmp(argv[1], "rmall")) {
		strcpy(owner, argv[2]);
		sys_info("Rm %s\n", owner);
		setting_remove_all(owner);
	} else {
		sys_err("Invalid argument\n");
		sys_err("Suggested test case:\n");
		sys_err("Case 1: sysset setting string qiwo ssid whatever\n");
		sys_err("Case 2: sysset setting int qiwo sid 55\n");
		sys_err("Case 3: sysset setting double qiwo ssidd 32.53136\n");
		sys_err("Case 4: sysset setting rmkey qiwo ssid\n");
		sys_err("Case 5: sysset setting rmall qiwo\n");
	}

	return 0;
}
