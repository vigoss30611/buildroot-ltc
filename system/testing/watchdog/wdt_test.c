#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/watchdog.h>

#if 0
#define WDT_DEBUG(arg...) printf(arg)
#else
#define WDT_DEBUG(format, ...)
#endif

#define WDT_DEV_NAME     		"/dev/watchdog"

static int  sig_break_flag = 0;
static  int wdt_feeding_flag = 0; //
const char *wdt_short_options = "d:t:f:";
//no_argument required_argument optional_argument
const struct option wdt_long_options[] = {
	{"duration", required_argument, NULL, 'd'},
	{"timeout",  required_argument, NULL, 't'},
	{"feedtime", required_argument, NULL, 'f'},
    {NULL, 0, NULL, 0 }
};

static void signal_handler(int sig)
{
    if(SIGINT == sig) {
        sig_break_flag = 1;
    }
}

int system_enable_watchdog(void)
{
	int fd = -1, args = WDIOS_ENABLECARD;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_SETOPTIONS, &args);
	close(fd);

	return 0;
}

int system_disable_watchdog(void)
{
	int fd = -1, args = WDIOS_DISABLECARD;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_SETOPTIONS, &args);
	close(fd);

	return 0;
}

int system_feed_watchdog(void)
{
	int fd = -1, args = 0;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_KEEPALIVE, &args);
	close(fd);

	return 0;
}

int system_set_watchdog(int interval)
{
	int fd = -1, args = interval;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_SETTIMEOUT, &args);
	close(fd);

	return 0;
}

void thread_feed_dog(void* arg)
{
    int feedtime = (int)arg;
        
    while(0 != wdt_feeding_flag){
        usleep(feedtime*1000);
        system_feed_watchdog();
        WDT_DEBUG("Feeding dog every %d ms!!\n", feedtime);
    }
    WDT_DEBUG("Stop feeding dog!!\n");
    pthread_exit(0);
} 

static void print_usage_1(const char *program_name)
{	
	printf("%s 1.0.0 : for watchdog timer test\n", program_name);
    printf("Usage: %s [OPTION]\n", program_name);
    printf("start				Enable wdt, can terminate with (ctrl+c)\n");
    printf("\t-d, --duration=		Specify test duration(ms)\n");
    printf("\t-t, --timeout=		Specify wdt timeout time(ms)\n");
    printf("\t-f, --feedtime=		Specify feeding dog time interval(ms)\n");
    printf("stop				Disable wdt\n");
}

static void print_usage_2(const char *program_name)
{
    printf("\n");
    printf("Suggested Test Case:\n");
	printf("Case 1: %s start -d 30000 -t 5000 -f 3000\n", program_name);
    printf("\tset 5s timeout, feed dog per 3s, test terminate 30s later, expect no reboot\n");
    printf("Case 2: %s start -t 5000\n", program_name);
    printf("\tset 5s timeout, don't feed dog, expect rebooting 5s later\n");
    printf("Case 3: %s stop\n", program_name);
    printf("\tstop watchdog timer, expect no reboot\n");
}

int main(int argc, char** argv)
{
    pthread_t   pid_feeddog = 0;
    int         ret = 0, val = 0;
    int         duration = 0;
    int         timeout = 0;
    int         feedtime = 0;

    signal(SIGINT, signal_handler);

    if(argc > 1) {
    	if(0 == strcmp(argv[1], "start")) {
            ;
    	} else if(0 == strcmp(argv[1], "stop")) {
    		system_disable_watchdog();
            WDT_DEBUG("Watchdog stopped\n");
            return 0;
		} else {
			printf("Invalid command, please check: \n");
    		print_usage_2(argv[0]);
            return -1;
    	}
    } else {
    	print_usage_1(argv[0]);
        print_usage_2(argv[0]);
        return -1;
    }

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
				printf("Invalid argument\n");
				return -1;
		}
	}

    if(timeout > 0){
        system_enable_watchdog();
        system_set_watchdog(timeout);
        
        if((feedtime > 0) && (feedtime < timeout)){
            wdt_feeding_flag = 1;
            ret = pthread_create(&pid_feeddog,NULL,(void *)thread_feed_dog, (void*)feedtime);
            if(ret != 0){
                printf("Create feeddog pthread failed!\n");
                wdt_feeding_flag = 0;
                pid_feeddog = 0;
                return -1;
            }
        }else{
            if(feedtime >= timeout){
                printf("Invalid feedtime %d!\n", feedtime);
            }
            printf("System should reboot %ds later!\n", timeout/1000);
        }
        duration /= 1000;
        while((0 == sig_break_flag) && (duration > 0)){
            duration --;
            sleep(1);
        }
        
        if(0 != pid_feeddog){
            wdt_feeding_flag = 0;
            pthread_join(pid_feeddog, NULL);
            system_disable_watchdog();
        }
        return 0;
    }else{
        printf("Invalid argument: timeout == %d\n", timeout);
        return -1;
    }
}
