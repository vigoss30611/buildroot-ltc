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
	int period=2000, step_period, fade_dir, alpha;
	unsigned int chroma_key=0, alpha_non_key_area;
	char str_dev_node[16] = {0};
	int fd;
	int ret;

	if (argc <3 || argc > 6)
		goto usage;

	overlay = strtoul(argv[1], NULL, 10);
	if (!overlay) {
		dlog_err("overlay0 does not support chroma key\n");
		return -EINVAL;
	}

	enable = strtoul(argv[2], NULL, 10);

	if (argc >= 4) {
		period = strtoul(argv[3], NULL, 10);
		if (period <= 0) {
			dlog_err("Invalid period %d\n", period);
			return -EINVAL;
		}
	}

	if (argc >= 5)
		chroma_key = strtoul(argv[4], NULL, 16);

	if (argc >= 6)
		debug = strtoul(argv[5], NULL, 10);

	dlog_info("argc=%d, overlay=%d, enable=%d, period=%d, chroma_key=0x%X, debug=%d\n",
				argc, overlay, enable, period, chroma_key, debug);

	sprintf(str_dev_node, "/dev/fb%d", overlay);
	dlog_dbg("open %s\n", str_dev_node);
	fd = open(str_dev_node, O_RDWR|O_SYNC);	
	if (fd < 0) {
		dlog_err("Failed to open %s\n", str_dev_node);
		return fd;
	}

	if (!enable) {
		dlog_dbg("FBIO_SET_CHROMA_KEY_ENABLE\n");
		ret = ioctl(fd, FBIO_SET_CHROMA_KEY_ENABLE, 0);
		if (ret) {
			dlog_err("ioctl FBIO_SET_CHROMA_KEY_ENABLE failed: %s\n", strerror(errno));
			return ret;
		}
		return 0;
	}

	if (argc >= 5) {
		dlog_dbg("FBIO_SET_CHROMA_KEY_VALUE\n");
		ret = ioctl(fd, FBIO_SET_CHROMA_KEY_VALUE, chroma_key);
		if (ret) {
			dlog_err("ioctl FBIO_SET_CHROMA_KEY_VALUE failed: %s\n", strerror(errno));
			return ret;
		}
	}

	dlog_dbg("FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA\n");
	ret = ioctl(fd, FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA, 0);
	if (ret) {
		dlog_err("ioctl FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("FBIO_SET_CHROMA_KEY_ENABLE\n");
	ret = ioctl(fd, FBIO_SET_CHROMA_KEY_ENABLE, 1);
	if (ret) {
		dlog_err("ioctl FBIO_SET_CHROMA_KEY_ENABLE failed: %s\n", strerror(errno));
		return ret;
	}

	step_period = period/2/16;
	alpha = 0x00;
	fade_dir = 1;

	dlog_info("start fade movie...\n");

	for (;;) {

		alpha_non_key_area = (alpha << 20) | (alpha << 12) | (alpha << 4);

		dlog_dbg("0x%X%c\n", alpha, fade_dir?'+':'-');
		ret = ioctl(fd, FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA, alpha_non_key_area);
		if (ret) {
			dlog_err("ioctl FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA failed: %s\n", strerror(errno));
			return ret;
		}

		usleep(step_period*1000);

		if (fade_dir) {
			alpha++;
			if (alpha >= 16) {
				fade_dir = 0;
				alpha = 15;
			}
		}
		else {
			alpha--;
			if (alpha < 0) {
				fade_dir = 1;
				alpha = 0;
			}
		}
	}

	dlog_dbg("end\n");

	close(fd);

	return 0;

usage:
	printf("Usage:\n"
			"\tdemo_ck <overlay> <enable> [period] [chroma_key] [debug]\n"
			"\t@overlay: which overlay to use\n"
			"\t@enable: enable/disable chroma key function\n"
			"\t@period: fade period in ms\n"
			"\t@chroma_key: chroma key value in RGB888\n"
			"\t@debug: 1: print debug log. 0 or none: disable debug log\n\n");
	return -EINVAL;
}

