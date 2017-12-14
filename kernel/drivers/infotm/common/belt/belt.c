/***************************************************************************** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Belt connect driver & OS.
**
** Author:
**     warits <warits.wang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.1  09/12/2012
*****************************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/cpufreq.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/poll.h>
#include <linux/rtc.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <mach/belt.h>
#include <mach/items.h>
#include <mach/nvedit.h>

#include "register.h"

//extern int battery_level(int level);
#if 0
static int infotm_scene = 0, __dvfs = 0;
extern void belt_notify_cpufreq(void * x);
static int rebooton_flag = 0;
extern void axp_power_off(void);

int belt_rebooton_flag(void)
{
    	return rebooton_flag;
}
EXPORT_SYMBOL(belt_rebooton_flag);

void (* bn_func)(void * x) = NULL;
int belt_notifier_register(void * func)
{
        bn_func = func;
        return 0;
}
EXPORT_SYMBOL(belt_notifier_register);
#endif
#if 0
static int belt_scene(int scene, int add)
{
	if(add) {
		if(scene & 0xffff0000)
			infotm_scene &= 0xffff;
		infotm_scene |= scene;
		if((scene & SCENE_HARD_FIX) && __dvfs) {
			printk(KERN_ERR "irq_disabled: %d\n", irqs_disabled());
			belt_notify_cpufreq(NULL);
			if(bn_func) bn_func(NULL);
		}
	} else
		infotm_scene &= ~scene;

	printk(KERN_ERR "infotm scene update: 0x%08x\n", infotm_scene);
	return infotm_scene;
}
#endif
#if 0
int belt_scene_get(void) { return infotm_scene;}
int belt_scene_set(int scene) { return belt_scene(scene, 1);}
int belt_scene_unset(int scene) { return belt_scene(scene, 0);}
EXPORT_SYMBOL(belt_scene_get);
EXPORT_SYMBOL(belt_scene_set);
EXPORT_SYMBOL(belt_scene_unset);
#endif

int belt_rtc_data = 4;
#if 0
int belt_get_rtc(void) {
	return belt_rtc_data;
}
#endif


static long belt_dev_ioctl(struct file *file,
			unsigned int cmd, unsigned long arg)                       
{
    char p[256], *q;
    int ret;
    struct belt_register reg;

    //printk("enter func %s at line %d \n", __func__, __LINE__);
	if(_IOC_TYPE(cmd) != BELT_MAGIC
	   || _IOC_NR(cmd) > BELT_IOCMAX)
	  return -ENOTTY;

	switch(cmd)
	{
#if 0
		case BELT_SCENE_GET:
			return infotm_scene;
		case BELT_SCENE_SET:
		case BELT_SCENE_UNSET:
			return belt_scene((int)arg, cmd == BELT_SCENE_SET);
#endif
                case BELT_ENV_GET:
                        ret = copy_from_user(p, (char *)arg, 256);
                        q = getenv(p);
                        if(q) strncpy(p, q, 256);
                        ret = copy_to_user((char *)arg, p, 256);
                        return 0;
		case BELT_ENV_SET:
			return 0;
		case BELT_ENV_CLEAN:
			cleanenv();
			return 0;
                case BELT_REG:
                        ret = copy_from_user((char *)&reg, (char *)arg,
                                sizeof(struct belt_register));
                        belt_register_smp(&reg);
                        ret = copy_to_user((char *)arg, (char *)&reg,
                                sizeof(struct belt_register));
                        return 0;
                default:
                        goto error;
	}

error:
	return -EFAULT;
}

int belt_dev_open(struct inode *fi, struct file *fp)
{
	return 0;
}

static int belt_dev_read(struct file *filp,
   char __user *buf, size_t length, loff_t *offset)
{
	return 0;
}

extern void imapx800_lvl_up(void);
extern void imapx800_lvl_down(void);
extern void imap_time_twist(int);
extern void start_trace_hitrate(void);
extern void trace_hitrate_end(void);
extern void twist_imapx_hr(int);
extern void twist_twd(int);
extern void trace_hitrate_end(void);

static ssize_t belt_dev_write(struct file *filp,
		const char __user *buf, size_t length,
		loff_t *offset)
{
	char str[256];
	int ret;

	ret = copy_from_user(str, buf, min(256, (int)length));

#if 0
	if(strncmp(str, "performance", 11) == 0) {
		imapx800_lvl_up();
		belt_scene_set(SCENE_PERFORMANCE);
	} else if(strncmp(str, "ondemand", 8) == 0) {
		imapx800_lvl_down();
		belt_scene_unset(SCENE_PERFORMANCE);
	} else if(strncmp(str, "top", 3) == 0) {
		belt_scene_set(SCENE_GPS);
	} else if(strncmp(str, "normal", 6) == 0) {
		belt_scene_unset(SCENE_GPS);
	} else if(strncmp(str, "wakeon", 6) == 0) {
		belt_rtc_data = simple_strtol(str + 6, NULL, 10);
		belt_scene_set(SCENE_RTC_WAKEUP);
		printk(KERN_ERR "rtc data: %d\n", belt_rtc_data);
	} else if(strncmp(str, "rebooton", 8) == 0) {
		belt_rtc_data = simple_strtol(str + 8, NULL, 10);
		rebooton_flag = (belt_rtc_data > 0) ? 1 : 0;
		if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0)) {
		    belt_scene_set(SCENE_RTC_WAKEUP);
		}
		printk(KERN_ERR "rtc data: %d\n", belt_rtc_data);
	} else if (strncmp(str, "reset", 5) == 0) {
	    	if(item_exist("board.arch") && item_equal("board.arch", "a5pv20", 0)) {
		    axp_power_off();
		}
	} else if(strncmp(str, "wakeoff", 7) == 0 ||
			strncmp(str, "rebootoff", 9 == 0)) {
		belt_scene_unset(SCENE_RTC_WAKEUP);
	} else if(strncmp(str, "bug", 3) == 0) {
		printk(KERN_EMERG "User require panic state.\n");
		BUG();
	} else if(strncmp(str, "klog", 4) == 0) {
		*(char *)(str + length) = 0;
		printk(KERN_EMERG "%s", str + 4);
#endif

		printk(KERN_ERR "HZ: %d\n", HZ);

	if(strncmp(str, "tt", 2) == 0){
		int t = simple_strtol(str + 2, NULL, 10);
		if(t < 10 || t > 200) {
			printk(KERN_ERR "<<< time twist region [10, 200]\n");
			return -EINVAL;
		}
		twist_imapx_hr(t);
		//twist_twd(t);
	} else if(strncmp(str, "env", 3) == 0){
		str[length] = 0;
		setenv_compact(str);
	}

#if 0
	else if(strncmp(str, "cachehitrateon", 14) == 0) {
		printk("start trace cpu hitrate\n");
		start_trace_hitrate();
	}else if(strncmp(str, "cachehitrateoff", 15) == 0) {
		printk("trace cpu hitrate end\n");
		trace_hitrate_end();
	}
#endif

	return length;
}

static const struct file_operations belt_dev_fops = {
	.read  = belt_dev_read,
	.write = belt_dev_write,
	.open  = belt_dev_open,
	.unlocked_ioctl = belt_dev_ioctl,
};

static int __init belt_dev_init(void)
{
	struct class *cls;
	int ret;

	printk(KERN_INFO "belt driver (c) 2012, 2017 InfoTM\n");

	/* create gps dev node */
	ret = register_chrdev(BELT_MAJOR, BELT_NAME, &belt_dev_fops);
	if(ret < 0)
	{
		printk(KERN_ERR "Register char device for belt failed.\n");
		return -EFAULT;
	}

	cls = class_create(THIS_MODULE, BELT_NAME);
	if(!cls)
	{
		printk(KERN_ERR "Can not register class for belt .\n");
		return -EFAULT;
	}

	/* create device node */
	device_create(cls, NULL, MKDEV(BELT_MAJOR, 0), NULL, BELT_NAME);

#if 0
	if(item_exist("dvfs.enable") && item_equal("dvfs.enable", "1", 0))
	  __dvfs = 1;
#endif

	return 0;
}

static void __exit belt_dev_exit(void) {}

module_init(belt_dev_init);
module_exit(belt_dev_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("warits <warits.wang@infotmic.com.cn>");
MODULE_DESCRIPTION("belt connecting driver & OS");

