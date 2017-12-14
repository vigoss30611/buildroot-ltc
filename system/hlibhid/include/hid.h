#ifndef __INCLUDE_HID_H__
#define __INCLUDE_HID_H__
#define HID_ASSERT(expr)  if (!(expr)) printf("+++ HID ASSERT(%s:%d) :"#expr"...\n", __FILE__, __LINE__)

enum hid_func
{
	HID_KEYBOARD,
	HID_MOUSE,
	HID_JOYSTICK
};

enum hid_status
{
	HID_UNINITED,
	HID_OPEN,
	HID_CLOSE
};

struct hid_device {
	int fd;
	int status;
};


struct hid_event {
	unsigned int func;
	unsigned int type;
};


struct hid_device* hid_open(char * name);
void hid_close(struct hid_device* dev);
int hid_send_event(struct hid_device* dev, struct hid_event *evt);
int hid_recv_event(struct hid_device *dev, struct hid_event *evt, struct timeval *time);
#endif
