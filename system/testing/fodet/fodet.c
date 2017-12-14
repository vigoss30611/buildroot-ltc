#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <qsdk/event.h>
#include <qsdk/videobox.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "fb.h"

#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

typedef struct {
	void *fbp;
	int fd_osd1;
	int screen_Size;
	int xres;
	int yres;
}FODET;

static FODET gfodet;
static struct itimerval oldtv;

static void display_color_key_l(int x, int y, int w, int h)
{
    int size, i, j, offset_x, offset_y;

	size = gfodet.screen_Size;
	int *ptr = (int *)gfodet.fbp;

	printf("draw H line\n");
	for(i=0; i<w; i++){
		offset_x = y*gfodet.xres + i + x;
		if(offset_x > size/4)
			continue;
		*(ptr+offset_x) = _RGB32BIT(0, 255, 0, 255);

		offset_x = (y+h)*gfodet.xres + i + x;
		if(offset_x >size/4)
			continue;
		*(ptr+offset_x) = _RGB32BIT(0, 255, 0, 255);
	}

	printf("draw V line\n");
	for(j=0; j<h; j++){
		offset_y = (y+j)*gfodet.xres + x;
		if(offset_y>size/4)
			continue;
		*(ptr+offset_y) = _RGB32BIT(0, 255, 0, 255);

		offset_y = (y+j)*gfodet.xres + x + w;
		if(offset_y>size/4)
			continue;
		*(ptr+offset_y) = _RGB32BIT(0, 255, 0, 255);
	}
}

//setting color bit may depend on specific screen,so you may need to fix the function on different displays
static void display_color_key(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    uint32_t i, j;
    int *ptr = (int *)gfodet.fbp;
    uint32_t color = 0xff00;
    for(i = 0; i < w; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + j)*gfodet.xres + x + i) > gfodet.xres * gfodet.yres -1)
                break;
            *(ptr + (y + j)*gfodet.xres + x + i) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }

    for(i = 0; i < w; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + j + h)*gfodet.xres + x + i) > gfodet.xres * gfodet.yres -1)
                break;
            *(ptr + (y + j + h)*gfodet.xres + x + i) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }

    for(i = 0; i < h; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + i)*gfodet.xres + x + j) > gfodet.xres * gfodet.yres -1)
                break;
            *(ptr + (y + i)*gfodet.xres + x + j) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }

    for(i = 0; i < h; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + i)*gfodet.xres + x + w + j) > gfodet.xres * gfodet.yres -1)
                break;
            *(ptr + (y + i)*gfodet.xres + x + w + j) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }
}

static void event_fodet_handle(char *event, void *arg)
{
    int i = 0;
    struct itimerval itv;
    struct event_fodet e_fd;

    if (!strncmp(event, EVENT_FODET, strlen(EVENT_FODET))) {
        memcpy(&e_fd, (struct event_fodet *)arg, sizeof(struct event_fodet));
        memset((int *)gfodet.fbp, 0x01, gfodet.screen_Size);
        itv.it_interval.tv_sec = 10;
        itv.it_interval.tv_usec = 0;
        itv.it_value.tv_sec = 10;
        itv.it_value.tv_usec = 0;
        setitimer(ITIMER_REAL, &itv, &oldtv);

        for(i=0; i<e_fd.num; i++){
            printf("num:%d, x:%d, y:%d, w:%d, h:%d\n", e_fd.num, e_fd.x[i], e_fd.y[i], e_fd.w[i], e_fd.h[i]);
            display_color_key(e_fd.x[i], e_fd.y[i], e_fd.w[i], e_fd.h[i]);
        }
    }
}

static void init_fb()
{
    struct fb_var_screeninfo vinfo;

	gfodet.fd_osd1 = open("/dev/fb1", O_RDWR);
	if(gfodet.fd_osd1<0){
		printf("open /dev/fb1 failed\n");
		exit(-1);
	}

    ioctl(gfodet.fd_osd1, FBIOGET_VSCREENINFO, &vinfo);

	gfodet.xres = vinfo.xres;
	gfodet.yres = vinfo.yres;
	gfodet.screen_Size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	gfodet.fbp = mmap(0, gfodet.screen_Size, PROT_READ|PROT_WRITE, MAP_SHARED, gfodet.fd_osd1, 0);
    if (gfodet.fbp == MAP_FAILED) {
		printf("Error: failed to map framebuffer device to memory.\n");
		close(gfodet.fd_osd1);
		return;
    }

    vinfo.yoffset = 0;
    ioctl(gfodet.fd_osd1, FBIOPAN_DISPLAY, &vinfo);
}

static void free_fb()
{
	munmap(gfodet.fbp, gfodet.screen_Size);
	close(gfodet.fd_osd1);
}

//setting color bit may depend on specific screen,so you may need to fix the function on different displays
void signal_handler()
{
	memset((int *)gfodet.fbp, 0x01, gfodet.screen_Size);
}

int main(void)
{
	init_fb();
	signal(SIGALRM, signal_handler);

	event_register_handler(EVENT_FODET, 0, event_fodet_handle);
    while(1){
        usleep(20000);
    }
	free_fb();

	return 0;
}
