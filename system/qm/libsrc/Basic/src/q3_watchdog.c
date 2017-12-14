#include <stdio.h>
#include <unistd.h>
#include <qsdk/sys.h>
#include <pthread.h>

//#define WATCHDOG_TIMEOUT_SECOND   3000
#define WATCHDOG_TIMEOUT_SECOND   5000

static int g_nStopFeed = 0;

void feed_dog_thread(void)
{
	while(!g_nStopFeed)
	{
		system_feed_watchdog();
		sleep(1);
	}
	printf("%s  %d, Exit dog thread!!!!!!!!!!!\n",__FUNCTION__, __LINE__);
}

void watchdog_stop_feed()
{
	g_nStopFeed = 1;
}

int watchdog_set_timeout(int ms)
{
    int ret = 0;
	ret = system_set_watchdog(ms);
	if(ret)
	{
		printf("%s  %d, set watch dog timeout failed!!\n",__FUNCTION__, __LINE__);
		return -1;
	}

    return 0;
}

int watchdog_start()
{
	int ret = 0;
	pthread_t tid;

	g_nStopFeed = 0;
	ret = system_enable_watchdog();
	if(ret)
	{
		printf("%s  %d, enable watch dog failed!!\n",__FUNCTION__, __LINE__);
		return -1;
	}

	ret = system_set_watchdog(WATCHDOG_TIMEOUT_SECOND);
	if(ret)
	{
		printf("%s  %d, set watch dog failed!!\n",__FUNCTION__, __LINE__);
		return -1;
	}

	pthread_create(&tid, NULL, (void *)feed_dog_thread, NULL);

	return 0;
}

int watchdog_stop()
{
    int ret;
	watchdog_set_timeout(10000);
    watchdog_stop_feed();
    sleep(2);

    ret = system_disable_watchdog();
    if(ret < 0) {
        printf("disable watchdog failed.\n");
        return -1;
    }
    else {
        printf("disable watchdog success.\n");
    }

    return 0;
}

