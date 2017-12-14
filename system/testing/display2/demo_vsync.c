#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fr/libfr.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <fr/libfr.h>
#include "display_fb.h"

static int debug = 0;

#define dlog_dbg(format, arg...)	\
({if(debug) printf("[demo_vsync] dbg: " format, ## arg);0;})

#define dlog_info(format, arg...)	\
({printf("[demo_vsync] info: " format, ## arg);0;})

#define dlog_err(format, arg...)	\
({printf("[demo_vsync] error: " format, ## arg);0;})

static char strbuf[64] = {0};
static char * buffer;
static unsigned int phys_addr;


void clean_up(int sig)
{
	dlog_dbg("clean up resource\n");
	fr_free_single(buffer, phys_addr);
	exit(0);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int speed = 1;
	char * line;
	unsigned int win_addr;
	unsigned char buf=0;
	int width=0, height=0, rgb_align;
	int i, j;
	int ret;
	int bytes_per_pixel = 4;
	struct sigaction sgh;
	int fd, fdrn;
	int peripheral_w, peripheral_h;
	struct display_fb_win_size win_size;
	struct fb_var_screeninfo var;
	int xoffset, yoffset;
	int direction, steps, steer;
	unsigned long randnum;

	if (argc < 2 || argc > 4) {
		goto usage;
	}

	if (argc >= 3)
		speed = strtoul(argv[2], NULL, 10);
	if (speed < 1) {
		dlog_err("the min speed is 1\n");
		return -EINVAL;
	}

	if (argc >= 4)
		debug = strtoul(argv[3], NULL, 10);

	fp = fopen(argv[1], "rb");
	if (!fp) {
		dlog_err("can NOT open file %s\n", argv[1]);
		return -ENOENT;
	}

	/* parse width */
	fseek(fp, 18, SEEK_SET);
	for (i = 0; i < 4; i++) {
		fread(&buf, 1, 1, fp);
		width+=buf<<(i*8);
	}
	rgb_align = width*3+3;
	rgb_align /= 4;
	rgb_align *= 4;

	/* parse height */
	fseek(fp, 22, SEEK_SET);
	for (i = 0; i < 4; i++) {
		fread(&buf, 1, 1, fp);
		height+=buf<<(i*8);
	}

	line = malloc(rgb_align);
	if (!line) {
		dlog_err("No memory for line\n");
		return -ENOMEM;
	}

	dlog_dbg("filling logo buffer...\n");

	buffer = fr_alloc_single("bmp2fb", width*height*bytes_per_pixel, &phys_addr);
	if (!buffer) {
		dlog_err("fr_alloc_single failed\n");
		return -ENOMEM;
	}
	for (i = 0; i < height; i++) {
		fseek(fp, 54+rgb_align*(height-1-i), SEEK_SET);
		fread(line, 1, rgb_align, fp);
		for (j = 0; j < width; j++) {
			buffer[(width*4*i)+j*4+3] = 0;				// a
			buffer[(width*4*i)+j*4+2] = line[j*3+2];	// r
			buffer[(width*4*i)+j*4+1] = line[j*3+1];	// g
			buffer[(width*4*i)+j*4+0] = line[j*3+0];	// b
		}
	}

	fclose(fp);
	free(line);
	
	dlog_dbg("open /dev/fb0\n");
	fd = open("/dev/fb0", O_RDWR|O_SYNC);
	if (fd < 0) {
		dlog_err("Failed to open /dev/fb0\n");
		return fd;
	}

	dlog_dbg("FBIOBLANK: POWERDOWN\n");
	ret = ioctl(fd, FBIOBLANK, FB_BLANK_POWERDOWN);
	if (ret) {
		dlog_err("ioctl FBIOBLANK failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("FBIO_GET_PERIPHERAL_SIZE\n");
	ret = ioctl(fd, FBIO_GET_PERIPHERAL_SIZE, &win_size);
	if (ret) {
		dlog_err("ioctl FBIO_GET_PERIPHERAL_SIZE failed: %s\n", strerror(errno));
		return ret;
	}
	peripheral_w = win_size.width;
	peripheral_h = win_size.height;

	dlog_info("bmp width %d, height %d, rgb_align %d, peripheral_w %d, peripheral_h %d, speed %d\n",
				width, height, rgb_align, peripheral_w, peripheral_h, speed);

	if (width <= peripheral_w || height <= peripheral_h) {
		dlog_err("bmp resolution must be larger than device's\n");
		return -EINVAL;
	}

	dlog_dbg("FBIO_SET_OSD_SIZE\n");
	ret = ioctl(fd, FBIO_SET_OSD_SIZE, &win_size);
	if (ret) {
		dlog_err("ioctl FBIO_SET_OSD_SIZE failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("FBIOGET_VSCREENINFO\n");
	ret = ioctl(fd, FBIOGET_VSCREENINFO, &var);
	if (ret) {
		dlog_err("ioctl FBIOGET_VSCREENINFO failed: %s\n", strerror(errno));
		return ret;
	}

	var.xres = peripheral_w;
	var.yres = peripheral_h;
	var.xres_virtual = width;
	var.yres_virtual = peripheral_h;
	var.xoffset = 0;
	var.yoffset = 0;
	var.width = peripheral_w;
	var.height = peripheral_h;
	var.reserved[0] = 0;
	var.reserved[1] = 0;
	var.grayscale = V4L2_PIX_FMT_RGB32;
	var.activate = FB_ACTIVATE_FORCE;		// necessary

	dlog_dbg("FBIOPUT_VSCREENINFO\n");
	ret = ioctl(fd, FBIOPUT_VSCREENINFO, &var);
	if (ret) {
		dlog_err("ioctl FBIOPUT_VSCREENINFO failed: %s\n", strerror(errno));
		return ret;
	}

	xoffset = (width - peripheral_w) / 2;
	yoffset = (height - peripheral_h) / 2;
	win_addr = phys_addr + (yoffset * width + xoffset) * 4;

	dlog_dbg("FBIO_SET_BUFFER_ADDR\n");
	ret = ioctl(fd, FBIO_SET_BUFFER_ADDR, win_addr);
	if (ret) {
		dlog_err("ioctl FBIO_SET_BUFFER_ADDR failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("FBIOBLANK: UNBLANK\n");
	ret = ioctl(fd, FBIOBLANK, FB_BLANK_UNBLANK);
	if (ret) {
		dlog_err("ioctl FBIOBLANK failed: %s\n", strerror(errno));
		return ret;
	}

	dlog_dbg("open /proc/sys/kernel/random/uuid\n");
	fdrn = open("/proc/sys/kernel/random/uuid", O_RDONLY);
	if (fdrn < 0) {
		dlog_err("Failed to open /proc/sys/kernel/random/uuid\n");
		return fd;
	}

	ret = read(fdrn, strbuf, 64);
	if (ret < 0) {
		dlog_err("read uuid failed %d\n", ret);
		return ret;
	}

	strbuf[7] = '\0';
	randnum = strtoul(strbuf, NULL, 16);
	direction = randnum % 8;
	dlog_dbg("uuid=%s, direction=%d\n", strbuf, direction);

	close(fdrn);

	sgh.sa_handler = clean_up;
	sigemptyset(&sgh.sa_mask);
	sgh.sa_flags = 0;
	sigaction(SIGINT, &sgh, NULL);

	steps = 1;
	i = 0;

	for (;;) {
		//dlog_dbg("FBIOBLANK: FBIO_WAITFORVSYNC\n");
		ret = ioctl(fd, FBIO_WAITFORVSYNC, 1000);
		if (ret) {
			dlog_err("ioctl FBIO_WAITFORVSYNC failed: %s\n", strerror(errno));
			return ret;
		}

		if (--steps <= 0) {
			fdrn = open("/proc/sys/kernel/random/uuid", O_RDONLY);
			if (fdrn < 0) {
				dlog_err("Failed to open /proc/sys/kernel/random/uuid\n");
				return fd;
			}
			ret = read(fdrn, strbuf, 64);
			if (ret < 0) {
				dlog_err("read uuid failed %d\n", ret);
				return ret;
			}
			close(fdrn);
			strbuf[7] = '\0';
			//dlog_dbg("uuid1=%s\n", strbuf);
			randnum = strtoul(strbuf, NULL, 16);
			steer = (randnum % 3)-1;	// 0,1,2 -> -1,0,1
			//dlog_dbg("randnum1=%ld, steer=%d\n", randnum, steer);
			direction += steer;
			if (direction > 7)
				direction = 0;
			else if (direction < 0)
				direction = 7;
			strbuf[5] = '\0';
			//dlog_dbg("uuid2=%s\n", strbuf);
			randnum = strtoul(strbuf, NULL, 16);
			steps = randnum % ((width/speed)/8);
			//dlog_dbg("randnum2=%ld, width/speed/8=%d, steps=%d\n", randnum, (width/speed)/8, steps);
			if (steps == 0)
				steps = 1;

			if (speed > 10) {
				if (steps < 10)
					steps = 10;
			}
			else {
				if (steps < speed)
					steps = speed;
			}

			dlog_dbg("direction=%d, steps=%d\n", direction, steps);
		}

		if (direction == 0 || direction == 1 || direction == 7)
			yoffset -= speed;
		else if (direction == 3 || direction == 4 || direction == 5)
			yoffset += speed;

		if (direction == 1 || direction == 2 || direction == 3)
			xoffset += speed;
		else if (direction == 5 || direction == 6 || direction == 7)
			xoffset -= speed;

		if (xoffset < 0) {
			dlog_dbg("left edge\n");
			xoffset = 0;
			steps = 1;
			if (direction == 7)
				direction = 0;
			else if (direction == 5)
				direction = 4;
			else if (direction == 6) {
				if (randnum % 2)
					direction = 0;
				else
					direction = 4;
			}
		}
		if (yoffset < 0) {
			dlog_dbg("top edge\n");
			yoffset = 0;
			steps = 1;
			if (direction == 1)
				direction = 2;
			else if (direction == 7)
				direction = 6;
			else if (direction == 0) {
				if (randnum % 2)
					direction = 2;
				else
					direction = 6;
			}
		}
		if (xoffset + peripheral_w > width) {
			dlog_dbg("right edge\n");
			xoffset = width - peripheral_w;
			steps = 1;
			if (direction == 3)
				direction = 4;
			else if (direction == 1)
				direction = 0;
			else if (direction == 2) {
				if (randnum % 2)
					direction = 4;
				else
					direction = 0;
			}
		}
		if (yoffset + peripheral_h > height) {
			dlog_dbg("bottom edge\n");
			yoffset = height - peripheral_h;
			steps = 1;
			if (direction == 5)
				direction = 6;
			else if (direction == 3)
				direction = 2;
			else if (direction == 4) {
				if (randnum % 2)
					direction = 6;
				else
					direction = 2;
			}
		}

		win_addr = phys_addr + (yoffset * width + xoffset) * 4;
		ret = ioctl(fd, FBIO_SET_BUFFER_ADDR, win_addr);
		if (ret) {
			dlog_err("ioctl FBIO_SET_BUFFER_ADDR failed: %s\n", strerror(errno));
			return ret;
		}
	}

	close(fdrn);
	fr_free_single(buffer, phys_addr);

	return 0;

usage:
	printf("Usage:\n\tdemo_vsync <bmp_file> [speed] [debug]\n"
			"\t@speed: window moving speed, multiplier\n"
			"\t@debug: 1: print debug log. 0 or none: disable debug log\n"
			"\tNotice: <bmp_file> resolution must be larger than device's\n\n");
	return 0;
}
