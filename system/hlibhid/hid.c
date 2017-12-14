/*
	You can add as many HID functions as you want, only limited by
	the amount of interrupt endpoints your gadget driver supports.

Send and receive HID reports

	HID reports can be sent/received using read/write on the
	/dev/hidgX character devices. See below for an example program
	to do this.

	hid_gadget_test is a small interactive program to test the HID
	gadget driver. To use, point it at a hidg device and set the
	device type (keyboard / mouse / joystick) - E.G.:

		# hid_gadget_test /dev/hidg0 keyboard

	You are now in the prompt of hid_gadget_test. You can type any
	combination of options and values. Available options and
	values are listed at program start. In keyboard mode you can
	send up to six values.

	For example type: g i s t r --left-shift

	Hit return and the corresponding report will be sent by the
	HID gadget.

	Another interesting example is the caps lock test. Type
	--caps-lock and hit return. A report is then sent by the
	gadget and you should receive the host answer, corresponding
	to the caps lock LED status.

		--caps-lock
		recv report:2

	With this command:

		# hid_gadget_test /dev/hidg1 mouse

	You can test the mouse emulation. Values are two signed numbers.


Sample code
*/

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

unsigned char* keyboard_event_tbl[] = {
	"--left-alt a d d ",
	"--left-alt r m d "
};

unsigned char* joystick_event_tbl[] = {
	"0 0 0  --b1",
	"0 0 0  --b2",
	"0 0 0  --b3",
	"0 0 0  --b4"
};

struct options {
	const char    *opt;
	unsigned char val;
};

static struct options kmod[] = {
	{.opt = "--left-ctrl",		.val = 0x01},
	{.opt = "--right-ctrl",		.val = 0x10},
	{.opt = "--left-shift",		.val = 0x02},
	{.opt = "--right-shift",	.val = 0x20},
	{.opt = "--left-alt",		.val = 0x04},
	{.opt = "--right-alt",		.val = 0x40},
	{.opt = "--left-meta",		.val = 0x08},
	{.opt = "--right-meta",		.val = 0x80},
	{.opt = NULL}
};

static struct options kval[] = {
	{.opt = "--return",	.val = 0x28},
	{.opt = "--esc",	.val = 0x29},
	{.opt = "--bckspc",	.val = 0x2a},
	{.opt = "--tab",	.val = 0x2b},
	{.opt = "--spacebar",	.val = 0x2c},
	{.opt = "--caps-lock",	.val = 0x39},
	{.opt = "--f1",		.val = 0x3a},
	{.opt = "--f2",		.val = 0x3b},
	{.opt = "--f3",		.val = 0x3c},
	{.opt = "--f4",		.val = 0x3d},
	{.opt = "--f5",		.val = 0x3e},
	{.opt = "--f6",		.val = 0x3f},
	{.opt = "--f7",		.val = 0x40},
	{.opt = "--f8",		.val = 0x41},
	{.opt = "--f9",		.val = 0x42},
	{.opt = "--f10",	.val = 0x43},
	{.opt = "--f11",	.val = 0x44},
	{.opt = "--f12",	.val = 0x45},
	{.opt = "--insert",	.val = 0x49},
	{.opt = "--home",	.val = 0x4a},
	{.opt = "--pageup",	.val = 0x4b},
	{.opt = "--del",	.val = 0x4c},
	{.opt = "--end",	.val = 0x4d},
	{.opt = "--pagedown",	.val = 0x4e},
	{.opt = "--right",	.val = 0x4f},
	{.opt = "--left",	.val = 0x50},
	{.opt = "--down",	.val = 0x51},
	{.opt = "--kp-enter",	.val = 0x58},
	{.opt = "--up",		.val = 0x52},
	{.opt = "--num-lock",	.val = 0x53},
	{.opt = NULL}
};

int keyboard_fill_report(char report[8], char buf[BUF_LEN], int *hold)
{
	char *tok = strtok(buf, " ");
	int key = 0;
	int i = 0;

	for (; tok != NULL; tok = strtok(NULL, " ")) {

		if (strcmp(tok, "--quit") == 0)
			return -1;

		if (strcmp(tok, "--hold") == 0) {
			*hold = 1;
			continue;
		}

		if (key < 6) {
			for (i = 0; kval[i].opt != NULL; i++)
				if (strcmp(tok, kval[i].opt) == 0) {
					report[2 + key++] = kval[i].val;
					break;
				}
			if (kval[i].opt != NULL)
				continue;
		}

		if (key < 6)
			if (islower(tok[0])) {
				report[2 + key++] = (tok[0] - ('a' - 0x04));
				continue;
			}

		for (i = 0; kmod[i].opt != NULL; i++)
			if (strcmp(tok, kmod[i].opt) == 0) {
				report[0] = report[0] | kmod[i].val;
				break;
			}
		if (kmod[i].opt != NULL)
			continue;

		if (key < 6)
			fprintf(stderr, "unknown option: %s\n", tok);
	}
	return 8;
}

static struct options jmod[] = {
    {.opt = "--b1",     .val = 0x10},
    {.opt = "--b2",     .val = 0x20},
    {.opt = "--b3",     .val = 0x40},
    {.opt = "--b4",     .val = 0x80},
    {.opt = "--hat1",   .val = 0x00},
    {.opt = "--hat2",   .val = 0x01},
    {.opt = "--hat3",   .val = 0x02},
    {.opt = "--hat4",   .val = 0x03},
    {.opt = "--hatneutral", .val = 0x04},
    {.opt = NULL}
};

int joystick_fill_report(char report[8], char buf[BUF_LEN], int *hold)
{
    char *tok = strtok(buf, " ");
    int mvt = 0;
    int i = 0;

    *hold = 1;

    for (; tok != NULL; tok = strtok(NULL, " ")) {

        if (strcmp(tok, "--quit") == 0)
            return -1;

        for (i = 0; jmod[i].opt != NULL; i++)
            if (strcmp(tok, jmod[i].opt) == 0) {
                report[3] = (report[3] & 0xF0) | jmod[i].val;
                break;
            }
        if (jmod[i].opt != NULL)
            continue;
    }
    return 4;
}

struct hid_device* hid_open(char * name)
{
	int fd;
	const char *filename = NULL;
	struct hid_device* dev = NULL;
	filename = name;
	if ((fd = open(filename, O_RDWR, 0666)) == -1) {
		perror(filename);
		return NULL;
	}
	dev = malloc(sizeof(struct hid_device));
	if (dev==NULL )
	{
		close(fd);
		printf("%s,%d, dev==NULL\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	dev->fd = fd;
	return dev;
}

void hid_close(struct hid_device* dev)
{
	close(dev->fd);
}

void hid_event_trans_out(struct hid_event* evt, char * buf)
{
	if (evt->func == HID_KEYBOARD)
	{
		if (evt->type < sizeof(keyboard_event_tbl)/sizeof(keyboard_event_tbl[0]))	{
			strncpy(buf, joystick_event_tbl[evt->type], BUF_LEN);
			return;
		}
	} else if(evt->func == HID_JOYSTICK) {
		if (evt->type < sizeof(joystick_event_tbl)/sizeof(joystick_event_tbl[0]))	{
			strncpy(buf, joystick_event_tbl[evt->type], BUF_LEN);
			return;
		}
	}
	printf("%s,%d: invalid event func=%d, type=%d\n", __FUNCTION__, __LINE__, evt->func, evt->type);
	
}

void hid_event_trans_in(struct hid_event *evt, char * buf)
{
}

int hid_send_event(struct hid_device* dev, struct hid_event *evt)
{
	char buf[BUF_LEN];
	char report[8];
	int to_send = 0;
	int hold = 0;
	HID_ASSERT(dev!=NULL);
	HID_ASSERT(evt!=NULL);
	hid_event_trans_out(evt, buf);
	memset(report, 0x0, sizeof(report));
	if (evt->func == HID_KEYBOARD)	{
		to_send = keyboard_fill_report(report, buf, &hold);
	} else if (evt->func == HID_JOYSTICK) {
		to_send = joystick_fill_report(report, buf, &hold);	
	}


	if (to_send == -1)
		return -1;

	if (write(dev->fd, report, to_send) != to_send) {
		perror("write()");
		return -1;
	}
	return 1;
}

int hid_recv_event(struct hid_device *dev, struct hid_event *evt, struct timeval *time)
{
	char buf[BUF_LEN];
	fd_set rfds;
	int fd;
	int retval;
	int i;
	int cmd_len;
	
	HID_ASSERT(dev!=NULL);
	HID_ASSERT(evt!=NULL);
	fd = dev->fd;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	
	retval = select(fd + 1, &rfds, NULL, NULL, time);
	if (retval == -1 && errno == EINTR)
		return -1;
	if (retval < 0) {
		perror("select()");
		return -1;
	}
	if (retval == 0)
	{
		printf("recv time out, please op in host like caplock\n");
		return 0;
	}

	if (FD_ISSET(fd, &rfds)) {
		cmd_len = read(fd, buf, BUF_LEN - 1);
		printf("recv report:");
		for (i = 0; i < cmd_len; i++)
			printf(" %02x", buf[i]);
		printf("\n");
	}
	hid_event_trans_in(evt, buf);
	return 1;
}
