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
#include <linux/fb.h>

#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))
//#define MV_DEBUG

#ifdef MV_DEBUG
#define DG(fmt, x...) printf("[mvdetect]: "fmt, ##x)
#else
#define DG(fmt, x...)
#endif

typedef struct {
	void *fbp;
	int fd_osd1;
	int screen_Size;
	int xres;
	int yres;
} MVDET;

static MVDET gmvdet;
uint32_t g_timestamp;
uint32_t vx, vy;

void rect_draw(int *ptr, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    uint32_t i, j;
    for(i = 0; i < w; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + j)*gmvdet.xres + x + i) > gmvdet.xres * gmvdet.yres -1)
                break;
            *(ptr + (y + j)*gmvdet.xres + x + i) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }

    for(i = 0; i < w; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + j + h)*gmvdet.xres + x + i) > gmvdet.xres * gmvdet.yres -1)
                break;
            *(ptr + (y + j + h)*gmvdet.xres + x + i) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }

    for(i = 0; i < h; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + i)*gmvdet.xres + x + j) > gmvdet.xres * gmvdet.yres -1)
                break;
            *(ptr + (y + i)*gmvdet.xres + x + j) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }

    for(i = 0; i < h; i++) {
        for(j = 0; j < 3; j++) {
            if(((y + i)*gmvdet.xres + x + w + j) > gmvdet.xres * gmvdet.yres -1)
                break;
            *(ptr + (y + i)*gmvdet.xres + x + w + j) = _RGB32BIT(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        }
    }
}

static void event_mvdet_handle(char *event, void *arg)
{
	struct evd e_fd;
	struct roiInfo_t roi;
	int *ptr = (int *)gmvdet.fbp;
    uint32_t x, y, w, h;
    int i, j;
    float scale_x = gmvdet.xres * 1.0 / vx;
    float scale_y = gmvdet.yres * 1.0 / vy;

    if(!strcmp(event, EVENT_MVMOVE)) {
        memcpy(&e_fd, (struct evd *)arg, sizeof(struct evd));
        if(e_fd.blk_x && e_fd.blk_y) {
            //block grid info
            DG("blkinfo x: %d, y: %d\n", e_fd.mx, e_fd.my);
            w = gmvdet.xres / e_fd.blk_x;
            h = gmvdet.yres / e_fd.blk_y;
            x = e_fd.mx * w;
            y = e_fd.my * h;
            rect_draw(ptr, x, y, w, h, 0xff0000);
        } else {
            //move mb(16x16) info
            DG("mbinfo x: %d, y: %d\n", e_fd.mx, e_fd.my);
            x = e_fd.mx * 16;
            y = e_fd.my * 16;

            x = (uint32_t)x * scale_x;
            y = (uint32_t)y * scale_y;

            for(i = 0; i < 16; i++) {
                for(j = 0; j < 16; j++) {
                    if(((y + j)*gmvdet.xres + x + i) > gmvdet.xres * gmvdet.yres -1)
                        break;
                    *(ptr + (y + j)*gmvdet.xres+x + i) = _RGB32BIT(0x0, 0, 0, 0xff);
                }
            }
        }
        if(g_timestamp != e_fd.timestamp) {
            g_timestamp = e_fd.timestamp;
            memset((int *)gmvdet.fbp, 0x01, gmvdet.screen_Size);
        }
    } else if(!strcmp(event, EVENT_MVMOVE_ROI)) {
        memcpy(&roi, (struct roiInfo_t *)arg, sizeof(struct roiInfo_t));
        DG("roiinfo x: %d, y: %d, w: %d, h: %d, num: %d, roinum: %d\n",
                roi.mv_roi.x, roi.mv_roi.y, roi.mv_roi.w, roi.mv_roi.h, roi.mv_roi.mv_mb_num, roi.roi_num);
        rect_draw(ptr, (uint32_t)roi.mv_roi.x * scale_x, (uint32_t)roi.mv_roi.y * scale_y,
                (uint32_t)roi.mv_roi.w * scale_x, (uint32_t)roi.mv_roi.h * scale_y, 0xff00);
    }
}

static void init_fb()
{
    struct fb_var_screeninfo vinfo;

	gmvdet.fd_osd1 = open("/dev/fb1", O_RDWR);
	if(gmvdet.fd_osd1<0){
		printf("open /dev/fb1 failed\n");
		exit(-1);
	}

    ioctl(gmvdet.fd_osd1, FBIOGET_VSCREENINFO, &vinfo);
	gmvdet.xres = vinfo.xres;
	gmvdet.yres = vinfo.yres;
	gmvdet.screen_Size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	gmvdet.fbp = mmap(0, gmvdet.screen_Size, PROT_READ|PROT_WRITE, MAP_SHARED, gmvdet.fd_osd1, 0);
    if (gmvdet.fbp == MAP_FAILED) {
		printf("Error: failed to map framebuffer device to memory.\n");
		close(gmvdet.fd_osd1);
		return;
    }
    vinfo.yoffset = 0;
    ioctl(gmvdet.fd_osd1, FBIOPAN_DISPLAY, &vinfo);
}

void KillHandler()
{
    event_unregister_handler(EVENT_MVMOVE, event_mvdet_handle);
    event_unregister_handler(EVENT_MVMOVE_ROI, event_mvdet_handle);
    if(gmvdet.fbp)
        munmap(gmvdet.fbp, gmvdet.screen_Size);
    if(gmvdet.fd_osd1)
        close(gmvdet.fd_osd1);
    exit(0);
}


int main(int argc, char** argv)
{
    if(argc != 3) {
        printf("mvdetect inputvideo_width inputvideo_height\n");
        return -1;
    }

    vx = atoi(argv[1]);
    vy = atoi(argv[2]);
    if(vx < 0 || vy < 0) {
        printf("invalid width or height param\n");
        exit(-1);
    }
	init_fb();
    signal(SIGINT, KillHandler); // ctrl-c
	event_register_handler(EVENT_MVMOVE, 0, event_mvdet_handle);
	event_register_handler(EVENT_MVMOVE_ROI, 0, event_mvdet_handle);

    while(1){
        usleep(20000);
    }
	return 0;
}
