#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include <pthread.h>
#include "sysset.h"

static int wdt_feeding_flag = 0;

static void print_usage(const char *program_name)
{
    printf("\n");
    printf("Suggested Test Case:\n");
	printf("Case 1: sysset %s start -d 30000 -t 5000 -f 3000\n", program_name);
    printf("\tset 5s timeout, feed dog per 3s, test terminate 30s later, expect no reboot\n");
    printf("Case 2: sysset %s start -t 5000\n", program_name);
    printf("\tset 5s timeout, don't feed dog, expect rebooting 5s later\n");
    printf("Case 3: sysset %s stop\n", program_name);
    printf("\tstop watchdog timer, expect no reboot\n");
}

void thread_feed_dog(void* arg)
{
	int feedtime = (int)arg;

	while (0 != wdt_feeding_flag) {
		usleep(feedtime * 1000);
		system_feed_watchdog();
		sys_info("Feeding dog every %d ms!!\n", feedtime);
	}

	sys_err("Stop feeding dog!!\n");
	pthread_exit(0);
}

int wdt_test(int argc, char** argv)
{
	pthread_t	pid_feeddog = 0;
	int ret = 0, val = 0;
	int duration = 0, timeout = 0, feedtime = 0;

//	signal(SIGINT, signal_handler);

	if (argc > 1) {
		if (0 == strcmp(argv[1], "start")) {
			sys_info("watchdog start\n");
		} else if (0 == strcmp(argv[1], "stop")) {
			system_disable_watchdog();
			sys_info("watchdog stopped\n");
			return 0;
		} else {
			sys_err("Invalid command\n");
			print_usage(argv[0]);
			return -1;
		}
	}

	char const wdt_short_options[] = "d:t:f:";
	struct option wdt_long_options[] = {
		{"duration", 0, NULL, 'd'},
		{"timeout", 0, NULL, 't'},
		{"feedtime", 0, NULL, 'f'},
		{0, 0, 0, 0}
	};

	while ((val = getopt_long(argc-1, argv+1, wdt_short_options,
					wdt_long_options, NULL)) != -1) {
		switch (val) {
			case 'd':
				duration = strtol(optarg, NULL, 0);
				break;
			case 't':
				timeout = strtol(optarg, NULL, 0);
				break;
			case 'f':
				feedtime = strtol(optarg, NULL, 0);
				break;
			default:
				sys_err("Invalid argument\n");
				return -1;
		}
	}

	if (timeout > 0) {
		system_enable_watchdog();
		system_set_watchdog(timeout);

		if ((feedtime > 0) && (feedtime < timeout)) {
			wdt_feeding_flag = 1;
			ret = pthread_create(&pid_feeddog, NULL, (void *)thread_feed_dog, (void *)feedtime);
			if (ret != 0) {
				sys_err("Create feeddog pthread failed\n");
				wdt_feeding_flag = 0;
				pid_feeddog = 0;
				return -1;
			}
		} else {
			if (feedtime >= timeout) {
				sys_err("Invalid feedtime %d\n", feedtime);
			}
			sys_err("System should reboot %ds later!\n", timeout/1000);
		}

		duration /= 1000;
		while(duration > 0) {
			duration --;
			sleep(1);
		}

		if (0 != pid_feeddog) {
			wdt_feeding_flag = 0;
			pthread_join(pid_feeddog, NULL);
			system_disable_watchdog();
		}
	}

	return 0;
}
