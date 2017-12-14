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
#include <linux/workqueue.h>
#include <linux/clk.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <mach/power-gate.h>

#include <mach/pad.h>

#include <dss_common.h>
#include <abstraction.h>
#include <implementation.h>
#include <ids_access.h>
#include <controller.h>
#include <imapx_dev_mmu.h>
#include <imapx800_g2d.h>
#include <scaler.h>
#include "api.h"

#define DSS_LAYER	"[display]"
#define DSS_SUB1	""
#define DSS_SUB2	""

#define IM_BUFFER_FLAG_DEVADDR	(1 << 2)
#define DMMU_DEV_OSD1			(13)
#define DMMU_DEV_OSD0          (12)
#define DMMU_DEV_IDS1_W1           (6)

#define HDMI_IOCTL_VSYNC		0x10004
#define HDMI_IOCTL_SWAP			0x10002
#define HDMI_IOCTL_DEVINFO		0x18001
#define HDMI_IOCTL_GETVIC		0x18002
#define HDMI_IOCTL_SETVIC 		0x18003
#define HDMI_IOCTL_ENABLE		0x18004
#define HDMI_IOCTL_QUERY		0x10005
#define HDMI_IOCTL_EVENT		0x10003
#define CVBS_IOCTL_EVENT		0x10006
#define HDMI_IOCTL_CVBS_SWAP_BUFFER       0x18006
#define HDMI_IOCTL_GET_CVBS_TYPE       0x18008
#define HDMI_IOCTL_GET_PRODUCT_TYPE       0x18005
#define HDMI_IOCTL_DISABLE_CVBS       0x18007


struct hdmi_vic_user {
	int count;
	int vic[40];
};

struct dmmu_buffer {
    int		uid;
    void *	vir_addr;
    int		phy_addr;
    int		dev_addr;
    int		size;
    int		flag;
};

struct hdmi_device {
	int hplg;
	int enabled;
	int cvbs_enabled;
	int saved_state;
	int vic;
	int cvbs_vic;
	int enable_request;
	struct work_struct work;
#ifdef CONFIG_IMAPX15_G2D
	struct delayed_work sync_work;
#endif
	struct cdev *cdev;
	dev_t dev;
	wait_queue_head_t wait;
	wait_queue_head_t cvbs_wait;
} * imapx_hdmi;

int dss_log_level = DSS_INFO;

extern int hdmi_update_state(int);
extern int hdmi_audio_update_state(int);
extern int hdmi_get_vic(int a[], int len);
extern int hdmi_get_monitor(char *, int);
extern int terminal_configure(char * terminal_name, int vic, int enable);
extern int ids_access_sysmgr_init(int idsx);
extern int swap_hdmi_buffer(u32 phy_addr);
extern int swap_cvbs_buffer(u32 phy_addr);
static struct mutex dmmu_mutex;
extern int pt;        
extern int cvbs_style;
extern int pre_vic;
extern dma_addr_t      map_dma_cvbs;   /* physical */
extern u_char *            map_cpu_cvbs;   /* virtual */
extern int map_cvbs_size;

int display_atoi(char *s)
{
	int num=0,flag=0;
	int i;
	for(i=0;i<=strlen(s);i++)
	{
		if(s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] -'0';
		else if(s[0] == '-' && i==0)
			flag =1;
		else
			break;
	}

	if(flag == 1)
		num = num * -1;

	return(num);
}

static struct mutex dmmu_mutex;
#if 0
static int dmmu_update(struct dmmu_buffer *df, int refresh)
{
	static struct dmmu_buffer d;
	static imapx_dmmu_handle_t *h = NULL;

	mutex_lock(&dmmu_mutex);

	/* do disable */
	if((!df || refresh) && h) {
		dss_dbg("disable dumm\n");

        ids_writeword(1, OVCW0CMR, 0x1000000);
        msleep(20);
		terminal_configure("hdmi_sink", imapx_hdmi->vic, 0);

        msleep(20);

		imapx_dmmu_detach(h, d.uid);

		/* disable OVCMMU */
		ids_write(1, 0x6000, 0, 8, 0);

		msleep(20);

		imapx_dmmu_disable(h);
		imapx_dmmu_deinit (h);
		h = NULL;
		d.uid = 0;
	}
	
	/* do enabled */
	if (df && !h) {
		dss_dbg("enable dmmu process...\n");
		h = imapx_dmmu_init(DMMU_DEV_OSD1);
		if(!h) {
			dss_err("failed to init dmmu for ods1.\n");
			mutex_unlock(&dmmu_mutex);
			return -1;
		}

		imapx_dmmu_enable(h);

		/* enable OVCMMU */
		ids_write(1, 0x6000, 0, 8, 0xff);
	}

	/* do attach */
	if(h) {
		dss_trace("attach new uid: %d\n", df->uid);
		imapx_dmmu_attach(h, df->uid);
		if(d.uid) {
			dss_trace("detach new uid: %d\n", d.uid);
			imapx_dmmu_detach(h, d.uid);
		}

		/* save df */
		memcpy(&d, df, sizeof(struct dmmu_buffer));
	}
    
	mutex_unlock(&dmmu_mutex);

	return 0;
}
#endif

int hdmi_hpd_event(int hpd)
{
	dss_trace("%d\n", hpd);

	imapx_hdmi->hplg = !!hpd;

	/* invoke the queue */
//	wake_up_interruptible(&imapx_hdmi->wait);
//	wake_up_interruptible(&imapx_hdmi->cvbs_wait);

	/* invoke hdmi_work */
	schedule_work(&imapx_hdmi->work);
	return 0;
}

static int hdmi_enable(int en)
{
	dss_dbg("%d\n", en);

	if(imapx_hdmi->enabled  == en)
		return 0;

	if(en) {
		/* hdmi will be acturly enabled when the first
		 * swap buffer is available.
		 *
		 * or else, ids1 will be enabled with an invalid
		 * memory, which result in a black or crash screen.
		 */
		imapx_hdmi->enable_request = 1;
	} else {
		imapx_hdmi->enable_request = 0;
		imapx_hdmi->enabled = 0;
	}

	return 0;
}

#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
extern struct g2d * g2d_data_global;
static int fbx = 0;
#else
extern dma_addr_t gmap_dma_f1;
extern u_char * gmap_cpu_f1;
#endif
static void hdmi_output(void)
{
#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
	if (!g2d_data_global) {
		dss_err("g2d data null\n");
		return;
	}
	swap_hdmi_buffer(g2d_data_global->main_fb_phyaddr + 
			fbx * g2d_data_global->main_fb_size/2);
#else
	swap_hdmi_buffer(gmap_dma_f1);
#endif

	dss_info("enable HDMI on the first swap.\n");
	terminal_configure("hdmi_sink", 4/*imapx_hdmi->vic*/, 1);
	ids_writeword(1, OVCW0CMR, 0x0000000);//dma work
	msleep(20);

	imapx_hdmi->enabled = 1;
}

#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
extern int g2d_work(int idsx, int VIC, int factor_width, 
		int factor_height, int main_fbx, int * g2d_fbx_p);
#endif
static void hdmi_sync_work_func(struct work_struct *work)
{
#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
	int fb_num;
	if (!g2d_data_global) {
		dss_err("g2d data null\n");
		return;
	}
#endif

	wait_vsync_timeout(0, IRQ_LCDINT, TIME_SWAP_INT);
#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
	fb_num = (ids_readword(0, OVCW0CR) >> 17) & 0x3;
	g2d_work(1, 4, 100, 100, fb_num, &fbx);
	swap_hdmi_buffer(g2d_data_global->main_fb_phyaddr + 
			fbx * g2d_data_global->main_fb_size/2);
#else
	swap_hdmi_buffer(gmap_dma_f1);
#endif

	imapx_hdmi->enabled = 1;

#ifdef CONFIG_IMAPX15_G2D
	schedule_delayed_work(&imapx_hdmi->sync_work, msecs_to_jiffies(10));
#endif
}

#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
extern int g2d_convert_logo(unsigned int bmp_width, unsigned int bmp_height, int fbx);
#endif

static void hdmi_work_func(struct work_struct *work)
{
	char name[64];

	if(!imapx_hdmi->hplg) {
#ifdef CONFIG_IMAPX15_G2D
		cancel_work_sync(&imapx_hdmi->sync_work);
#endif
		hdmi_enable(0);
		hdmi_update_state(0);
		hdmi_audio_update_state(0);
	} else {//if(imapx_hdmi->saved_state) {
		hdmi_get_monitor(name, 64);
		hdmi_enable(1);
		hdmi_update_state(1);
		hdmi_audio_update_state(1);
#if defined(CONFIG_IMAPX15_G2D) && defined(CONFIG_MACH_IMAPX15)
		g2d_convert_logo(800,480,0);
#endif
		hdmi_output();
//		schedule_delayed_work(&imapx_hdmi->sync_work, msecs_to_jiffies(0));
	}
}

static int hdmi_open(struct inode *inode, struct file *filp)
{
	dss_dbg("\n");
	return 0;
}

static int hdmi_release(struct inode *inode, struct file *filp)
{
	dss_dbg("\n");
	return 0;
}

#if 0
static long hdmi_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	static int ret, hplg_reported;
	struct hdmi_vic_user vl;
	struct dmmu_buffer df;
	char name[64];

	switch(cmd) {

	case HDMI_IOCTL_ENABLE:
		hdmi_enable(arg);
		hdmi_update_state(arg);
		break;

	case HDMI_IOCTL_DEVINFO:
		ret = hdmi_get_monitor(name, 64);
		if(ret < 0)
			return -EINVAL;

		printk(KERN_ERR "monitor name: %s\n", name);
		ret = copy_to_user((void __user *)arg, name, 64);
		return 0;

	case HDMI_IOCTL_GETVIC:
		memset(&vl, 0, sizeof(struct hdmi_vic_user));
		ret = hdmi_get_vic(vl.vic, 64);
		vl.count = ret;
		ret = copy_to_user((void __user *)arg, &vl,
				sizeof(struct hdmi_vic_user));
		break;

	case HDMI_IOCTL_SETVIC:
		imapx_hdmi->vic = arg;
		break;

	case HDMI_IOCTL_QUERY:
		put_user(imapx_hdmi->hplg, (int __user *)arg);
		hplg_reported = imapx_hdmi->hplg;


        /* a new event will clear the saved_state
         * which was set during suspend.
         */
        imapx_hdmi->saved_state = 0;
        dss_info("report query %d\n", hplg_reported);
        break;

    case HDMI_IOCTL_EVENT:
        wait_event_interruptible(imapx_hdmi->wait,
                imapx_hdmi->hplg != hplg_reported);
        put_user(imapx_hdmi->hplg, (int __user *)arg);
        hplg_reported = imapx_hdmi->hplg;


        /* a new event will clear the saved_state
         * which was set during suspend.
         */
        imapx_hdmi->saved_state = 0;
        dss_info("report event %d\n", hplg_reported);

		break;

	case HDMI_IOCTL_SWAP:
		ret = copy_from_user((void *)&df, (void __user *)arg,
				sizeof(struct dmmu_buffer));

        if(df.uid == -1) {
            dss_info("disable dmmu on uid = -1\n");
            dmmu_update(NULL, 0);
            return 0;
        }

		if(df.flag & IM_BUFFER_FLAG_DEVADDR) {
			dss_trace("SWAPING !!! 0x%08x\n", df.dev_addr);

			/* this is the right condition to enable
			 * ids1 and HDMI output
			 */
			if(!imapx_hdmi->enabled && imapx_hdmi->vic
					&& imapx_hdmi->enable_request) {

				dmmu_update(&df, 0);
                swap_hdmi_buffer(df.dev_addr);

				dss_info("enable HDMI on the first swap.\n");
				terminal_configure("hdmi_sink", imapx_hdmi->vic, 1);
				ids_writeword(1, OVCW0CMR, 0x0000000);//dma work
				msleep(20);

				imapx_hdmi->enabled = 1;
			} else if (imapx_hdmi->enabled && imapx_hdmi->vic
					&& imapx_hdmi->enable_request) {

				/* every dmmu_buffer passed from upper layer
				 * must be saved. the dmmu_disable operation
				 * will require the last uid.
				 *
				 * if dmmu is not disable properly, it won't
				 * work the next time.
				 */
				dmmu_update(&df, 0);
				swap_hdmi_buffer(df.dev_addr);
			}

		} else {
			dss_err("USING phy address !!! 0x%08x\n", df.phy_addr);
			break ;
		}

		break;

	case HDMI_IOCTL_VSYNC:
		wait_vsync_timeout(0, IRQ_LCDINT, TIME_SWAP_INT);
		break;

	default:
		dss_err("Invalid cmd %d\n", cmd);
		return -EINVAL;
	}

	return 0;
}
#endif

static const struct file_operations hdmi_fops = {
	.owner = THIS_MODULE,
	.open = hdmi_open,
	.release = hdmi_release,
//	.unlocked_ioctl = hdmi_ioctl,
};

static int display_probe(struct platform_device *pdev)
{
	int ret, i;
	struct resource *res;
	struct hdmi_device *hdmi;
	struct class *c;

	dss_trace("line: %d\n", __LINE__);

	hdmi = kzalloc(sizeof(struct hdmi_device), GFP_KERNEL);
	if(!hdmi) {
		dss_err("failed to alloc device buffer.\n");
		return -1;
	}
	imapx_hdmi = hdmi;

	ret = alloc_chrdev_region(&hdmi->dev, 0, 1, "hdmi");
	if (ret) {
		dss_err("alloc chrdev region failed\n");
		return -1;
	}

	hdmi->cdev = cdev_alloc();
	cdev_init(hdmi->cdev, &hdmi_fops);
	hdmi->cdev->owner = hdmi_fops.owner;

	ret = cdev_add(hdmi->cdev, hdmi->dev, 1);
	if (ret)
		dss_err("cdev add failed %d\n", ret);

	c = class_create (THIS_MODULE, "hdmi");
	if(IS_ERR(c)) {
		dss_err("failed to create class.\n");
		return -1;
	}
	device_create(c, NULL, hdmi->dev, NULL, "hdmi");

	init_waitqueue_head(&hdmi->wait);
	INIT_WORK(&hdmi->work, hdmi_work_func);
#ifdef CONFIG_IMAPX15_G2D
	INIT_DELAYED_WORK(&hdmi->sync_work, hdmi_sync_work_func);
#endif
	for (i = 0; i < 2; i++) {
		/* get irq */
		res = platform_get_resource(pdev, IORESOURCE_IRQ, i);
		ids_irq_set(i, res->start);
		dss_dbg("ids%d: irq = %d\n", i, res->start);

		/* init memory */
		res = platform_get_resource(pdev, IORESOURCE_MEM, i);
		ids_access_Initialize(i, res->start, (res->end - res->start + 1));
	}

	mutex_init(&dmmu_mutex);

	/* initialization */
	/* 
	 *(1)Author:summer
	 *(2)Date:2014-6-26
	 *(3)Reason:
	 *  The logo from uboot to kernel can't be continous,the modification is 
	 *  required by alex.
	 * */
	implementation_init();
	abstraction_init();
	//summer modification end here

	return 0;
}

extern int backlight_en_control(int enable);
extern int blanking;
extern struct clk *hdmi_clk, *clk_ids1, *clk_ids1_osd, *clk_ids1_eitf;
static int display_suspend(struct device *dev)
{
	dss_trace("%d\n", __LINE__);


	/* disable ids1 and HDMI and dmmu to
	 * let it work properly after resume.
	 *
	 * a flag called @saved_state is set.
	 * if this flag is detected during an HPD_IN
	 * event, ids1 and HDMI will be automatically
	 * enabled in regardless with upper-layer's
	 * instruction.
	 */
	if((imapx_hdmi->enabled ) && (pt == PRODUCT_MID)) {
		imapx_hdmi->saved_state = 1;
		hdmi_enable(0);
	//	dmmu_update(NULL, 0);
	}

	if (blanking == 0) {
		backlight_en_control(0);                    

		terminal_configure("lcd_panel", LCD_VIC, 0);
		ids_writeword(1, 0x1090, 0x1000000);        
		blanking = 1;
	}
	
	disable_irq(97);
	return 0;
}

extern void imapx_bl_power_on(void);
static int display_resume(struct device *dev)
{
	int vic[40];
	dss_trace("%d\n", __LINE__);

	ids_access_sysmgr_init(0);
	implementation_init();
	if (pt == PRODUCT_BOX) {
		clk_prepare_enable(clk_ids1);     
		clk_prepare_enable(clk_ids1_osd); 
		clk_prepare_enable(clk_ids1_eitf);
		clk_prepare_enable(hdmi_clk);     

		module_power_on(SYSMGR_IDS1_BASE);
		ids_writeword(1, 0x5c, 0);        
		msleep(10);
		api_Initialize(0, 1, 2500, 1);
		hdmi_get_vic(vic, 64);
		terminal_configure("hdmi_sink", 4, 1);
	} else if (pt == PRODUCT_MID) {
		if (blanking == 1) {
			ids_writeword(1, 0x1090, 0x0);              

			terminal_configure("lcd_panel", LCD_VIC, 1);
			backlight_en_control(1);                    
			blanking = 0;
		}
	}

	enable_irq(97);
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
MODULE_AUTHOR("warits");
MODULE_LICENSE("GPL v2");

