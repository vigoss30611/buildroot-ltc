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
	unsigned int chroma_key=0, alpha_non_key_area=0, alpha_key_area=0;
	char str_dev_node[16] = {0};
	int fd;
	int ret;

	if (argc <3 || argc > 7)
		goto usage;

	overlay = strtoul(argv[1], NULL, 10);
	if (!overlay) {
		dlog_err("overlay0 does not support chroma key\n");
		return -EINVAL;
	}

	enable = strtoul(argv[2], NULL, 10);

	if (argc >= 4)
		chroma_key = strtoul(argv[3], NULL, 16);

	if (argc >= 5)
		alpha_non_key_area = strtoul(argv[4], NULL, 16);

	if (argc >= 6)
		alpha_key_area = strtoul(argv[5], NULL, 16);

	if (argc >= 7)
		debug = strtoul(argv[6], NULL, 10);

	dlog_info("argc=%d, overlay=%d, enable=%d, chroma_key=0x%X, alpha_non_key_area=0x%X, alpha_key_area=0x%X, debug=%d\n",
				argc, overlay, enable, chroma_key, alpha_non_key_area, alpha_key_area, debug);

	sprintf(str_dev_node, "/dev/fb%d", overlay);
	dlog_dbg("open %s\n", str_dev_node);
	fd = open(str_dev_node, O_RDWR|O_SYNC);	
	if (fd < 0) {
		dlog_err("Failed to open %s\n", str_dev_node);
		return fd;
	}

	if (argc >= 4) {
		dlog_dbg("FBIO_SET_CHROMA_KEY_VALUE\n");
		ret = ioctl(fd, FBIO_SET_CHROMA_KEY_VALUE, chroma_key);
		if (ret) {
			dlog_err("ioctl FBIO_SET_CHROMA_KEY_VALUE failed: %s\n", strerror(errno));
			return ret;
		}
	}

	if (argc >= 5) {
		dlog_dbg("FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA\n");
		ret = ioctl(fd, FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA, alpha_non_key_area);
		if (ret) {
			dlog_err("ioctl FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA failed: %s\n", strerror(errno));
			return ret;
		}
	}

	if (argc >= 6) {
		dlog_dbg("FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA\n");
		ret = ioctl(fd, FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA, alpha_key_area);
		if (ret) {
			dlog_err("ioctl FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA failed: %s\n", strerror(errno));
			return ret;
		}
	}

	dlog_dbg("FBIO_SET_CHROMA_KEY_ENABLE\n");
	ret = ioctl(fd, FBIO_SET_CHROMA_KEY_ENABLE, enable);
	if (ret) {
		dlog_err("ioctl FBIO_SET_CHROMA_KEY_ENABLE failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("end\n");

	close(fd);

	return 0;

usage:
	printf("Usage:\n"
			"\tck2fb <overlay> <enable> [chroma_key] [alpha_non_key_area] [alpha_key_area] [debug]\n"
			"\t@overlay: which overlay to use\n"
			"\t@enable: enable/disable chroma key function\n"
			"\t@chroma_key: chroma key value in RGB888\n"
			"\t@alpha_non_key_area: alpha value for none key area, in RGB888\n"
			"\t@alpha_key_area: alpha value for key area, in RGB888\n"
			"\t@debug: 1: print debug log. 0 or none: disable debug log\n\n");
	return -EINVAL;
}

