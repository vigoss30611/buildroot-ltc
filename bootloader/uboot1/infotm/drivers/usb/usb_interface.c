#include <linux/types.h>
#include <common.h>
#include <vstorage.h>
#include <asm/byteorder.h>
#include <part.h>
#include <usb.h>

static int usb_stor_curr_dev = -1, usb_initialized = 0;
static block_dev_desc_t *udisk_dev = NULL;


int udisk_vs_read(uint8_t *buf, loff_t start, int length, int extra)
{
	int cnt, blk, n;

	if (!udisk_dev) {
	    printf("no udisk device found\n");
	    return -1;
	}

	blk = start / 512;
	cnt = length / 512;

//    printf("\nUSB read: device %d block # %d, count %d"
//				" ...", usb_stor_curr_dev, blk, cnt);
    n = udisk_dev->block_read(usb_stor_curr_dev, blk, cnt,
                 buf);
	if (n != cnt) {
		printf("udisk read is uncorrect\n");
		return (cnt * 512);
	}

//	printf("%d blocks read: %s\n", n,
//				(n == cnt) ? "OK" : "ERROR");
	return (cnt * 512);
}

int udisk_vs_write(uint8_t *buf, loff_t start, int length, int extra)
{
	int n, blk, cnt;

	if (!udisk_dev) {
	    printf("no udisk device found\n");
	    return -1;
	}
	
	blk = start / 512;
	cnt = length / 512;

//    printf("\nUSB write: device %d block # %d, count %d"
//				" ...", usb_stor_curr_dev, blk, cnt);
    n = udisk_dev->block_write(usb_stor_curr_dev, blk, cnt,
                 buf);
//	printf("%d blocks write: %s\n", n,
//				(n == cnt) ? "OK" : "ERROR");
	return (cnt * 512);
}


int udisk_reset(int index)
{
	int ret;

	if(usb_initialized)
	  goto __get_dev__;

	/* init the usb system */
	usb_stop();
	ret = usb_init();
	if (ret < 0) {
		printf("usb init failed.\n");
		return ret;
	}

	ret = usb_stor_scan(1);
	if (ret < 0) {
		printf("usb init storage failed.\n");
		return ret;
	}
	usb_initialized = 1;

__get_dev__:
    udisk_dev = usb_stor_get_dev(index);
	if(!udisk_dev) {
		printf("get udisk device(%d) failed.\n", index);
		return -2;
	}

	usb_stor_curr_dev = index;
	return 0;
}

int udisk0_vs_reset(void) { return udisk_reset(0); }
int udisk1_vs_reset(void) { return udisk_reset(1); }
int udisk0_vs_align(void) 
{
	udisk_reset(0);
	return udisk_dev!=NULL?	(udisk_dev->blksz? udisk_dev->blksz:-1) :-1;
}
int udisk1_vs_align(void) { 
	udisk_reset(1);
	return udisk_dev!=NULL? (udisk_dev->blksz? udisk_dev->blksz:-1) :-1;
}
loff_t udisk0_vs_capacity(void)
{
	    udisk_reset(0);
		    return udisk_dev!=NULL? (udisk_dev->lba? udisk_dev->lba:-1) :-1;
}   
loff_t udisk1_vs_capacity(void) { 
	    udisk_reset(1);
		    return udisk_dev!=NULL? (udisk_dev->lba? udisk_dev->lba:-1) :-1;
}


