
/* hid_gadget_test */

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hid.h>
#define BUF_LEN 512
#define HID_FUNC HID_JOYSTICK
#define MIN(a,b) (((a)<(b))?(a):(b))
int main(int argc, const char *argv[])
{
	const char *name = "/dev/hidg0";
	int fd = 0;
	unsigned int count = 1;
	struct hid_device* dev ;
	struct hid_event event_send; 
	struct hid_event event_recv; 
	struct timeval timeout = {10, 0};
	int ret;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s send/recv/send_recv type\n",
			argv[0]);
		return 1;
	}
	
	dev = hid_open(name);
	if (dev == NULL) {
		printf("%s,%d: open %s failed\n", __FUNCTION__, __LINE__, name);
		return -1;
	}
	event_send.type = 0;
	if (argc > 2){
		event_send.type = MIN(strtoul(argv[2], 0 , 10), 3); 
	}
	event_send.func = HID_FUNC;
	if (!strcmp(argv[1], "send")) {
		if(	hid_send_event(dev, &event_send) < 0) {
			printf("%s,%d: send event%d failed\n", __FUNCTION__, __LINE__, event_send.type);
		}
	}
	else if (!strcmp(argv[1], "recv")) {
		ret = hid_recv_event(dev, &event_recv, &timeout);
		if (ret < 0) {
			printf("%s,%d: recv event failed\n", __FUNCTION__, __LINE__);
		}
		if (ret > 0) {
			printf("%s,%d: recv event %d\n", __FUNCTION__, __LINE__, event_recv.type);
		}
	}
	else if (!strcmp(argv[1], "send_recv"))	{
		if (hid_send_event(dev, &event_send) < 0) {
			printf("%s,%d: send event%d failed\n", __FUNCTION__, __LINE__, event_send.type);
		}
		ret = hid_recv_event(dev, &event_recv, &timeout);
		if (ret < 0) {
			printf("%s,%d: recv event failed\n", __FUNCTION__, __LINE__);
		}
		if (ret > 0) {
			printf("%s,%d: recv event %d\n", __FUNCTION__, __LINE__, event_recv.type);
		}
	}

	hid_close(dev);
	return 0;
}

