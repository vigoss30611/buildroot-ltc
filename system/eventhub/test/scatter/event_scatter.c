#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "event.h"
#define EVENT_NAME "KEY3"

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
	struct key_value value1, value2, value3;
	struct event_scatter data[3];

	value1.data = 10;
	value1.data2 = 20;
	value1.data3 = 30;

	value2.data = 40;
	value2.data2 = 50;
	value2.data3 = 60;

	value3.data = 70;
	value3.data2 = 80;
	value3.data3 = 90;

	data[0].buf = (char*)&value1;
	data[0].len = sizeof(struct key_value);

	data[1].buf = (char*)&value2;
	data[1].len = sizeof(struct key_value);

	data[2].buf = (char*)&value3;
	data[2].len = sizeof(struct key_value);

	sleep(2);


	status = event_send_scatter(EVENT_NAME, data, 3);
	if(0 != status) {
		printf("%s.....event send scatter failed......\n", __FUNCTION__);
	} else {
		printf("%s....%d...%d....%d....status:%d.\n", __FUNCTION__, value2.data, value2.data2, value2.data3,status);
	}

    // loop
    while(1){
	};

    return 0;
}
