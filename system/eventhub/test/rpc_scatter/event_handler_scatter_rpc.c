#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "event.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>
#include <wait.h>
#include "message_queue.h"

#define EVENT_NAME "KEY4"

struct key_value
{
	int data;
	int data2;
	int data3;
};

struct key_value data[3];

void handler(char* name, void *arg)
{
	printf(" %s...callback function call.........\n", __FUNCTION__);

    if((NULL != name) && (NULL != arg) && (0 == strcmp(name, EVENT_NAME))) {
	    int *value1 = (int *)arg;

	    memcpy(&data, (struct key_value *)arg, sizeof(struct key_value) * 3);

	    printf("%s---data:%d,data2:%d,data3:%d---\n",__FUNCTION__, data[0].data,data[0].data2,data[0].data3);

	    *value1 = 30;
	    *(value1 + 1) = 40;
	    *(value1 + 2) = 80;

    } else {
        printf("%s.....callback function failed...\n", __FUNCTION__);
    }
}

/*
* the main function
*/
int main(int argc, char *argv[])
{
	int status = 0;
	status = event_register_handler(EVENT_NAME, EVENT_RPC, handler);

	if(0 != status) {
		printf("%s--event register handler failed.......\n", __FUNCTION__);
	}

    // loop
    while(1){
		usleep(200000);
		//break;
//		if (getchar() == 'q')
//			break;
	};

    return 0;
}
