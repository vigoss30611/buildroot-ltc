#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
//#include <asm-generic/errno-base.h>
#include "display_fb.h"

static int debug = 0;

#define dlog_dbg(format, arg...)	\
({if(debug) printf("[buf2fb] dbg: " format, ## arg);0;})

#define dlog_info(format, arg...)	\
({printf("[buf2fb] info: " format, ## arg);0;})

#define dlog_err(format, arg...)	\
({printf("[buf2fb] error: " format, ## arg);0;})



int main(int argc, char *argv[])
{
	int ret;
	int fd;
	int overlay;
	int path_enable;
	char str_dev_node[16] = {0};

	if (argc != 3 && argc != 4)
		goto usage;

	overlay = strtoul(argv[1], NULL, 10);

	if (argc >= 4)
		debug = strtoul(argv[3], NULL, 10);

	dlog_info("overlay=%d, peripheral=%s\n", overlay, argv[2]);

	sprintf(str_dev_node, "/dev/fb%d", overlay);
	dlog_dbg("open %s\n", str_dev_node);
	fd = open(str_dev_node, O_RDWR|O_SYNC);	
	if (fd < 0) {
		dlog_err("Failed to open %s\n", str_dev_node);
		return fd;
	}

	dlog_dbg("FBIO_GET_BLANK_STATE\n");
	ret = ioctl(fd, FBIO_GET_BLANK_STATE, &path_enable);
	if (ret) {
		dlog_err("ioctl FBIO_GET_BLANK_STATE failed: %s\n", strerror(errno));
		return ret;
	}
	dlog_dbg("path enable is %d\n", path_enable);

	if (path_enable) {
		dlog_dbg("FBIOBLANK: POWERDOWN\n");
		ret = ioctl(fd, FBIOBLANK, FB_BLANK_POWERDOWN);
		if (ret) {
			dlog_err("ioctl FBIOBLANK failed: %s\n", strerror(errno));
			return ret;
		}
	}

	dlog_dbg("FBIO_SET_PERIPHERAL_DEVICE\n");
	ret = ioctl(fd, FBIO_SET_PERIPHERAL_DEVICE, argv[2]);
	if (ret) {
		dlog_err("ioctl FBIO_SET_PERIPHERAL_DEVICE failed: %s\n", strerror(errno));
		return ret;
	}

	if (path_enable) {
		dlog_dbg("FBIOBLANK: UNBLANK\n");
		ret = ioctl(fd, FBIOBLANK, FB_BLANK_UNBLANK);
		if (ret) {
			dlog_err("ioctl FBIOBLANK failed: %s\n", strerror(errno));
			return ret;
		}
	}

	dlog_dbg("end\n");

	close(fd);

	return 0;


usage:
	printf("Usage:\n"
			"\tselect_peripheral <overlay> <peripheral> [debug]\n"
			"\t@overlay: specify overlay to select the desired path\n"
			"\t@peripheral: name string of the peripheral device\n"
			"\t@debug: 1: print debug log. 0 or none: disable debug log\n\n");

	return -EINVAL;
}

