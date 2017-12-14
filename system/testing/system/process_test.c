#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include "sysset.h"

const char* pid_name[5] = {
	"audiobox", "eventhub", "videobox",
	"wifi", "cepd"
};

int process_test(int argc, char** argv)
{
	int pid = 0;
	int index;

	if (0 == strcmp(argv[1], "check")) {
		index = atoi(argv[2]);
		pid = system_check_process(pid_name[index]);
		if (pid > 0) {
			sys_err("Process %s pid: %d\n", pid_name[index], pid);
		} else {
			sys_err("Process %s is not exist\n", pid_name[index]);
		}
	} else if (0 == strcmp(argv[1], "kill")) {
		index = atoi(argv[2]);
		sys_err("Sysset process kill %s\n", pid_name[index]);
		system_kill_process(pid_name[index]);
		sleep(1);
		pid = system_check_process(pid_name[index]);
		if (pid > 0) {
			sys_err("Process %s kill failed, pid: %d\n", pid_name[index], pid);
		} else {
			sys_err("Process %s killed\n", pid_name[index]);
		}
	} else {
		sys_err("Invalid argument\n");
		sys_err("Process list:\n");
		sys_err("0--------audiobox\n");
		sys_err("1--------eventhub\n");
		sys_err("2--------videobox\n");
		sys_err("3--------wifi\n");
		sys_err("4--------cepd\n");
		sys_err("Test Case 1: sysset process check 0\n");
		sys_err("Test case 2: sysset process kill 0\n");
	}

	return 0;
}
