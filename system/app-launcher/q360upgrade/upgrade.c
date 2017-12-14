#include <unistd.h>
#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include <qsdk/upgrade.h>

#define MMC_IUS_PATH "/mnt/mmc/update.ius"

static int fbfd = 0;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static void *fbp = NULL;
static int bits_per_pixel = 0;
static int screensize = 0;

int fb_open(void)
{
    fbfd = open("/dev/fb1", O_RDWR);
    if (!fbfd) {
	printf("Error: cannot open framebuffer device. \n");
	return(-1);
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
	printf("Error: reading fixed information. \n");
	return(-1);
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
	printf("Error: reading variable information. \n");
	return(-1);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    bits_per_pixel = vinfo.bits_per_pixel;
    screensize = vinfo.xres * vinfo.yres * bits_per_pixel / 8;
    printf("screensize=%d byte\n",screensize);

    fbp = mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp == MAP_FAILED)
    {
	printf("Error: failed to map framebuffer device to memory.\n");
	return(-1);
    }

    //reset the fb pointer to the 0 offset for double buffer
    vinfo.yoffset = 0;
    ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo);

    return(0);
}

void fb_close(void)
{
    if (fbp != NULL)
	munmap(fbp, screensize);
    if (fbfd)
	close(fbfd);
}

static void upgrade_state_callback_q360(void *arg, int image_num, int state, int state_arg)
{
    struct upgrade_t *upgrade = (struct upgrade_t *)arg;
    int cost_time;

    switch (state) {
	case UPGRADE_START:
	    system_set_led("led_work", 1000, 800, 1);
	    gettimeofday(&upgrade->start_time, NULL);
	    printf("q360 upgrade start \n");
	    break;
	case UPGRADE_COMPLETE:
	    gettimeofday(&upgrade->end_time, NULL);
	    cost_time = (upgrade->end_time.tv_sec - upgrade->start_time.tv_sec);
	    system_set_led("led_work", 1, 0, 0);
	    if (system_update_check_flag() == UPGRADE_OTA)
				system_update_clear_flag(UPGRADE_FLAGS_OTA);
	    printf("\n q360 upgrade sucess time is %ds \n", cost_time);
	    /* TODO: lcd flushing does not work.
	    if (item_exist("lcd.model")) {
		if (fb_open() == -1)
		    break;

		while(!access("/dev/mmcblk0", F_OK)) {
		    if (fbp != NULL)
			memset((char *)fbp, 0xff, screensize);
		    sleep(1);
		    if (fbp != NULL)
			memset((char *)fbp, 0, screensize);
		}

		fb_close();
	    }
	    */
	    break;
	case UPGRADE_WRITE_STEP:
	    printf("\r q360 %s image -- %d %% complete", image_name[image_num], state_arg);
	    fflush(stdout);
	    break;
	case UPGRADE_VERIFY_STEP:
	    if (state_arg)
		printf("\n q360 %s verify failed %x \n", image_name[image_num], state_arg);
	    else
		printf("\n q360 %s verify success\n", image_name[image_num]);
	    break;
	case UPGRADE_FAILED:
	    system_set_led("led_work", 300, 200, 1);
	    printf("q360 upgrade %s failed %d \n", image_name[image_num], state_arg);
	    break;
    }

    return;
}

int main(int argc, char** argv)
{
    int flag;
    char *path = NULL;
    char cmd[256];

    if (argc > 1) {
	path = argv[1];
    } else {
	flag = system_update_check_flag();
	if (flag == UPGRADE_IUS) {
	    path = NULL;
	} else if (flag == UPGRADE_OTA) {
	    if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0)) {
		sprintf(cmd, "mount -t vfat /dev/mmcblk1p1 /mnt/mmc");
	    } else {
		sprintf(cmd, "mount -t vfat /dev/mmcblk0p1 /mnt/mmc");
	    }
	    system(cmd);
	    if(access(MMC_IUS_PATH, F_OK) == 0)
		path = MMC_IUS_PATH;
	} else {
	    printf("nothing upgrade image \n");
	    return SUCCESS;
	}
    }

    system_update_upgrade(path, upgrade_state_callback_q360, NULL);

    if (flag == UPGRADE_OTA)
	printf("\n Remove mmc card to enter the new system.\n");
    else
	printf("\n Remove ius card to enter the new system.\n");

    if ((access("/dev/mmcblk0", F_OK) == 0) && (access("/dev/mmcblk1", F_OK) == 0))
	while(!access("/dev/mmcblk1", F_OK));
    else
	while(!access("/dev/mmcblk0", F_OK));

    reboot(LINUX_REBOOT_CMD_RESTART);

    return SUCCESS;
}

