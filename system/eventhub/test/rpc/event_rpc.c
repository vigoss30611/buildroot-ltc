#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "event.h"

#define EVENT_NAME "KEY2"

struct key_value
{
	int data;
	int data2;
	int data3;
};

/*
* the main function
*/
int main(int argc, char *argv[])
{
	int status = 0;
	struct key_value value;

	value.data = 10;
	value.data2 = 20;
	value.data3 = 100;

	sleep(2);


	status = event_rpc_call(EVENT_NAME, (char*)(&value), sizeof(struct key_value));
	if(0 != status) {
		printf("%s.....event rpc call failed......\n", __FUNCTION__);
	} else {
		printf("%s....%d...%d....%d....status:%d.\n", __FUNCTION__, value.data, value.data2, value.data3,status);
	}

    // loop
    while(1){
    	usleep(200000);
	};

    return 0;
}
