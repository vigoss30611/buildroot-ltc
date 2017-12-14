#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>
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
	int overlay;
	int enable;
	char str_dev_node[16] = {0};
	int fd;
	int ret;

	if (argc <3 || argc > 4)
		goto usage;

	overlay = strtoul(argv[1], NULL, 10);
	enable = strtoul(argv[2], NULL, 10);

	if (argc >= 4)
		debug = strtoul(argv[3], NULL, 10);

	dlog_info("overlay=%d, enable=%d, debug=%d\n", overlay, enable, debug);

	sprintf(str_dev_node, "/dev/fb%d", overlay);
	dlog_dbg("open %s\n", str_dev_node);
	fd = open(str_dev_node, O_RDWR|O_SYNC);	
	if (fd < 0) {
		dlog_err("Failed to open %s\n", str_dev_node);
		return fd;
	}

	dlog_dbg("FBIO_SET_BUFFER_ENABLE\n");
	ret = ioctl(fd, FBIO_SET_BUFFER_ENABLE, enable);
	if (ret) {
		dlog_err("ioctl FBIO_SET_BUFFER_ENABLE failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("end\n");

	close(fd);

	return 0;

usage:
	printf("Usage:\n"
			"\tfben <overlay> <enable> [debug]\n"
			"\t@overlay: which overlay to control\n"
			"\t@enable: enable/disable overlay\n"
			"\t@debug: 1: print debug log. 0 or none: disable debug log\n\n");
	return -EINVAL;
}

