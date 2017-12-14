#ifndef __DEV_H__
#define __DEV_H__

struct display_dev {
	char *name;
	void *priv;
	struct list_head link;

	int (*config)(char *args, int num);
	int (*enable)(int en);
};

int dev_register(struct display_dev *udev);
void dev_config(char *args, int num);

#endif

