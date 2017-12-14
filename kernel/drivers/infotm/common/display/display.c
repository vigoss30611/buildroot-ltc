/*
 * display.c - display subsystem driver
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <dss_common.h>
#include <abstraction.h>
#include <implementation.h>
#include <ids_access.h>
#include <controller.h>

#include <linux/workqueue.h>
#include <mach/pad.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/wait.h>

#include <linux/device.h>
#include <imapx_dev_mmu.h>

#define DSS_LAYER	"[display]"
#define DSS_SUB1	""
#define DSS_SUB2	""

#define IM_BUFFER_FLAG_DEVADDR          (1<<2)
#define DMMU_DEV_OSD1       (13)
// TODO: To support temp hdmi function, code become shit
#define CONFIG_INFOTM_MEDIA_DMMU_SUPPORT_private
struct hdmi_buffer_t{
	int		fd;
	int		fence;
};

struct hdmi_vic_t {
	int count;
	int vic[40];
};

struct hdmi_vic_t vic_list = {
	.count = 1,
	.vic[0] = 4
};

typedef struct{
    int    uid;        // universal buffer id, negative is invalid.
    void *      vir_addr;
    int   phy_addr;   // valid only when flag has bit of IM_BUFFER_FLAG_PHYADDR.
    int   dev_addr;   // valid only when flag has bit of IM_BUFFER_FLAG_DEVADDR.
    int   size;
    int   flag;
}IM_Buffer;

#define HDMI_IOCTL_CONFIG_SETUP			0x10001
#define HDMI_IOCTL_LAYER_SWAP_BUFFER	0x10002
#define HDMI_IOCTL_QUERY_EVENT			0x10003
#define HDMI_IOCTL_VSYNC					0x10004


#define HDMI_IOCTL_GET_DEVINFO		0x18001
#define HDMI_IOCTL_GET_VIC			0x18002
#define HDMI_IOCTL_SET_VIC	 		0x18003
#define HDMI_IOCTL_TOGGLE			0x18004
#define HDMI_IOCTL_QUERY_STATE		0x10005


#ifdef CONFIG_INFOTM_MEDIA_DMMU_SUPPORT_private
static int fb_dma_buf_fd = -1;
static imapx_dmmu_handle_t *fb_dmmu_handle = NULL;
static bool UseDevmmu = false;
#endif
// Temp
extern int terminal_configure(char * terminal_name, int vic, int enable);
extern int hdmi_configure(int vic);
extern int swap_hdmi_buffer(u32 phy_addr);
extern int ids_access_sysmgr_init(int idsx);
extern int api_HotPlugDetected();
extern void msleep(int ms);
//extern int print_irqnr;
static int hdmi_vic = 0;
static int buf_uid_old = 0;
static int mmu_enable = 0;
int dss_log_level = DSS_TRACE;//DSS_ERR;
extern int ids_irq[2];
static dev_t dev_index;
static struct cdev * hdmi_cdev = NULL;
static DECLARE_WAIT_QUEUE_HEAD(wait_queue);
static int hot_plug = 0;
static int hot_plug_old = 0;
static struct class * hdmidev_class;
static int vic = 0;
static unsigned int dma_addr = 0;
static int query_waiting = 0;
static int query_event = 0;
static unsigned int register_store[3][22] = {{0x00, 0x04, 0x08, 0x0C, 0x10, 0x18, 0x30, 0x54, 0x58, 0x5C,
	0x1000, 0x1004, 0x1008, 0x1080, 0x1084, 0x1088, 0x108C, 0x1090, 0x1094, 0x1098, 0x109C, 0x10A0},{0}};
static struct work_struct hdmi_work;
static int hdmien = 0;

extern int hdmi_update_state(int);
static void hdmi_work_func(struct work_struct *work)
{
	hdmi_update_state(hdmien);
}

static int hdmi_enable(int en)
{
    printk("hdmi_enable(hdmien=%d,en=%d)\n",hdmien,en);
	if(hdmien == !!en)
		return 0;

	hdmien = !!en;
	schedule_work(&hdmi_work);
	return terminal_configure("hdmi_sink", hdmien? vic: 0, !!hdmien);
}

int hdmi_hpd_event(int hpd)
{
	dss_trace("~\n");
	
    printk("hdmi_hpd_event(hpd=%d)\n",hpd);
	if (!hpd)
		hdmi_enable(0);

	dma_addr = 0;
	vic = 0;
	hot_plug = hpd;
    printk("hdmi_hpd_event(hot_plug=%d)\n",hot_plug);
	if (query_waiting)
		wake_up_interruptible(&wait_queue);
	else
		query_event = 1;
	return 0;
}

static int hdmi_open(struct inode *inode, struct file *filp)
{
	dss_dbg("...\n");
	return 0;
}

static int hdmi_release(struct inode *inode, struct file *filp)
{
	dss_dbg("...\n");
	return 0;
}

static long hdmi_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int tmp, tmp_vic;
    int dev_addr;
    int s;
    IM_Buffer hwc_buffer;
	switch(cmd) {
	case HDMI_IOCTL_TOGGLE:
		if(arg) {
			if (vic && dma_addr) {	// validate vic
				hdmi_enable(1);
			} else
				dss_dbg("Trying enable HDMI with wrong config: %d, %u\n", vic, dma_addr);
		} else {
			hdmi_enable(0);
		}
		break;
	case HDMI_IOCTL_GET_VIC:
		copy_to_user((void __user *)arg, &vic_list, sizeof(struct hdmi_vic_t));
		break;
	case HDMI_IOCTL_SET_VIC:
		if(arg < 1 || arg > 39) {
			dss_dbg("Wrong VIC number: %lu\n", arg);
			return -EINVAL;
		}

		dss_dbg("VIC set to: %lu\n", arg);
		vic = arg;
		hdmi_vic = vic;
		break;
	case HDMI_IOCTL_QUERY_STATE:
		put_user(hot_plug_old, (int __user *)arg);
		break;
	case HDMI_IOCTL_QUERY_EVENT:
		dss_dbg("cmd: HDMI_IOCTL_QUERY_EVENT\n");
		if (query_event) {
			dss_dbg("Immediate query event hot plug = %d\n", hot_plug);
			hot_plug_old = hot_plug;
			query_event = 0;
		}
		else {
			dss_dbg("Waiting event...\n");
			query_waiting = 1;
			wait_event_interruptible(wait_queue, hot_plug != hot_plug_old);//just condition true(return 0) to wake up,use poll&select to replace
			query_waiting = 0;
			hot_plug_old = hot_plug;
			dss_dbg("Wake up\n");
		}
		put_user(hot_plug, (int __user *)arg);
		break;
	case HDMI_IOCTL_CONFIG_SETUP:
		get_user(tmp_vic, (uint32_t __user *)arg);
		/* emu set vic */
		if(tmp_vic < 1 || tmp_vic > 39) {
			dss_dbg("Wrong VIC number: %d\n", tmp_vic);
			return -EINVAL;
		}

		dss_dbg("VIC set to: %d\n", tmp_vic);
		vic = tmp_vic;
        hdmi_vic = tmp_vic;
		/* emu enable */
		if (vic && dma_addr)	// validate vic
			hdmi_enable(1);
		else
			dss_dbg("Trying enable HDMI with wrong config: %d, %d\n", vic, dma_addr);

		break;

	case HDMI_IOCTL_LAYER_SWAP_BUFFER:
        s = copy_from_user((void*)&hwc_buffer, (void*)arg, sizeof(IM_Buffer));
		//get_user(tmp, (u32 __user *)arg);
		//dss_dbg("cmd: HDMI_IOCTL_LAYER_SWAP_BUFFER. uid: 0x%X\n", hwc_buffer.uid);

#ifdef CONFIG_INFOTM_MEDIA_DMMU_SUPPORT_private
#if 1
    if((!hot_plug) &&(hwc_buffer.uid == -1) && (fb_dmmu_handle)){
        if(buf_uid_old){
            imapx_dmmu_detach(fb_dmmu_handle,buf_uid_old);
            buf_uid_old = 0;
        }
        ids_write(1,0x6000 ,0, 8,0);// disable OVCMMU
        imapx_dmmu_disable(fb_dmmu_handle);
        //printk("imapx_dmmu_deinit\n");
        imapx_dmmu_deinit(fb_dmmu_handle);
        fb_dmmu_handle = NULL;
        mmu_enable = 0;
        return 0;
    }
#endif
#endif

if(hot_plug){
#ifdef CONFIG_INFOTM_MEDIA_DMMU_SUPPORT_private
        if(hwc_buffer.flag & IM_BUFFER_FLAG_DEVADDR){

            if(!mmu_enable){
                UseDevmmu = true;
                fb_dmmu_handle = imapx_dmmu_init(DMMU_DEV_OSD1);
                imapx_dmmu_enable(fb_dmmu_handle);
                ids_write(1,0x6000 ,0, 8,0xff); //enable OVCMMU
                mmu_enable = 1;
            }
            if(fb_dmmu_handle ){
                dev_addr = imapx_dmmu_attach(fb_dmmu_handle,hwc_buffer.uid);
                dev_addr = hwc_buffer.dev_addr;
            }
            if(fb_dmmu_handle && buf_uid_old){
                imapx_dmmu_detach(fb_dmmu_handle,buf_uid_old);
            }
            buf_uid_old = hwc_buffer.uid;
        }
#endif


#ifdef CONFIG_INFOTM_MEDIA_DMMU_SUPPORT_private
        if(hwc_buffer.flag & IM_BUFFER_FLAG_DEVADDR){
            swap_hdmi_buffer(dev_addr);	// Wait IDS1 LCDINT
		    //dss_dbg("swap return(dev_addr =0x%x)\n",dev_addr);
        }else{
            swap_hdmi_buffer(hwc_buffer.phy_addr);	// Wait IDS1 LCDINT
        }
#else
		swap_hdmi_buffer(hwc_buffer.phy_addr);	// Wait IDS1 LCDINT
#endif
		//dss_dbg("swap return\n");

		if (!dma_addr && vic)	// only when first time to call swap
			hdmi_enable(1);
        if(hwc_buffer.flag & IM_BUFFER_FLAG_DEVADDR){
            dma_addr = hwc_buffer.dev_addr;
        }else{
            dma_addr = hwc_buffer.phy_addr;
        }
}
        break;
	case HDMI_IOCTL_VSYNC:
		//dss_dbg("cmd: HDMI_IOCTL_VSYNC\n");
		wait_vsync_timeout(0, IRQ_LCDINT, TIME_SWAP_INT);	// IDS0 LCDINT
		//dss_dbg("vsync return\n");
		break;
	default:
		dss_err("Invalid cmd %d\n", cmd);
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations hdmi_ops = {
	.owner = THIS_MODULE,
	.open = hdmi_open,
	.release = hdmi_release,
	.unlocked_ioctl = hdmi_ioctl,
};

static int access_init(int idsx, struct resource *res)
{
	if (!res->start || res->end < res->start) {
		dss_err("Invalid resource.\n");
		return -1;
	}
	return ids_access_Initialize(idsx, res->start, (res->end - res->start + 1));
}

static int display_probe(struct platform_device *pdev)
{
	int idsx;
	struct resource *res;
	int ret;
	
	dss_trace("~\n");

	INIT_WORK(&hdmi_work, hdmi_work_func);
	ret = alloc_chrdev_region(&dev_index, 0, 1, "hdmi");
	if (ret) {
		dss_err("alloc chrdev region failed\n");
		return -1;
	}
	dss_info("Major %d, Minor %d\n", MAJOR(dev_index), MINOR(dev_index));

	dss_dbg("cdev\n");
	hdmi_cdev = cdev_alloc();
	cdev_init(hdmi_cdev, &hdmi_ops);
	hdmi_cdev->owner = hdmi_ops.owner;
	hdmi_cdev->ops = &hdmi_ops;
	ret = cdev_add(hdmi_cdev, dev_index, 1);
	if (ret)
		dss_err("cdev add failed %d\n", ret);

	dss_dbg("class\n");
	hdmidev_class = class_create(THIS_MODULE, "hdmidev");
	if(IS_ERR(hdmidev_class)){
		dss_err("class create failed\n");
		return -1;
	}
	device_create(hdmidev_class, NULL, dev_index, NULL, "hdmi");

	dss_dbg("Init\n");
	for (idsx = 0; idsx < 2; idsx++) {
		res = platform_get_resource(pdev, IORESOURCE_IRQ, idsx);
		printk("ids%d irq = %d\n", idsx, res->start);
		ids_irq_set(idsx, res->start);
		res = platform_get_resource(pdev, IORESOURCE_MEM, idsx);
		access_init(idsx, res);
	}
	
	/* Initialization */
	implementation_init();
	abstraction_init();
	//strategy_init();


	// temp
	//lighten_lcd();

	return 0;
}

static int display_suspend(struct device *dev)
{
//	int i, j;

	dss_trace("~\n");
#if 0
	for (i = 0; i < 2; i++)
		for (j = 0; j < 22; j++)
		 	register_store[i+1][j] = ids_readword(i, register_store[0][j]);
#endif
    printk("display_suspend1\n");
	terminal_configure("lcd_panel", LCD_VIC, 0);
    if(hot_plug){
    printk("display_suspend2\n");
	hdmi_enable(0);
    if(UseDevmmu){
        //printk("display_suspend\n");
        if(buf_uid_old){
            imapx_dmmu_detach(fb_dmmu_handle,buf_uid_old);
            buf_uid_old = 0;
        }
        //ids_access_sysmgr_init(0);
        ids_write(1,0x6000 ,0, 8,0);// disable OVCMMU
        imapx_dmmu_disable(fb_dmmu_handle);
        //printk("imapx_dmmu_deinit\n");
        imapx_dmmu_deinit(fb_dmmu_handle);
        fb_dmmu_handle = NULL;
        mmu_enable = 0;
    }
    }
    //ids_access_sysmgr_init(0);
    return 0;
}

static int display_resume(struct device *dev)
{
//	int i, j;
	
	dss_trace("~\n");
    printk("display_resume\n");

	ids_access_sysmgr_init(0);
	ids_access_sysmgr_init(1);
	implementation_init();
	terminal_configure("lcd_panel", LCD_VIC, 1);

    if(hot_plug){
    if(UseDevmmu){
        printk("display_resume(hot_plug=%d)\n",hot_plug);
        fb_dmmu_handle = imapx_dmmu_init(DMMU_DEV_OSD1);
        imapx_dmmu_enable(fb_dmmu_handle);
        ids_write(1,0x6000 ,0, 8,0xff); //enable OVCMMU
        buf_uid_old = 0;//if not 0,mmu detach may err
    }
    //RM#8253
    msleep(17);//Test shows if open device immediately after mmu enable. mmu will not work.so map color for a while (for one frame period.)
    vic = hdmi_vic;
    if (vic && dma_addr)	// validate vic
        hdmi_enable(1);
    }
#if 1
    //RM#8259
    //put after mmu_init to have a whole mmu resume procedure
    //workaround for hdmi unplug bug after suspend
    if(hot_plug){
       hot_plug =  api_HotPlugDetected();
       if(!hot_plug){
           hdmi_hpd_event(0);
       }
    }
#endif 
#if 0
	for (i = 0; i < 2; i++)
		for (j = 0; j < 22; j++)
		 	ids_writeword(i, register_store[0][j], register_store[i+1][j]);
#endif
	//print_irqnr= 1;
	return 0;
}

static const struct dev_pm_ops display_pm_ops = {
	.suspend		= display_suspend,
	.resume		= display_resume,
};

static struct platform_driver display_driver = {
	.probe		= display_probe,
	//.remove		= display_remove,
	.driver		= {
		.name	= "display",
		.owner	= THIS_MODULE,
		.pm = &display_pm_ops,
	},
};

int __init display_init(void)
{
	return platform_driver_register(&display_driver);
}
static void __exit display_exit(void)
{
	platform_driver_unregister(&display_driver);
}

module_init(display_init);
module_exit(display_exit);

MODULE_DESCRIPTION("InfoTM iMAP display subsystem driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
