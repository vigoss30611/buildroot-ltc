/*****************************************************************************
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: Ispost driver.
**
*****************************************************************************/
#include "drv_ispost.h"

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
#include <mach/items.h>
#include <linux/time.h>
#include <mach/nvedit.h>
#include <mach/pad.h>
#include "ci_ispost.h"

#define IRQ_ISPOST 116

#define ISPOST_DEBUGFS

#ifdef ISPOST_DEBUGFS

#include <mach/imap-ispost.h>
#include <linux/debugfs.h>
#include "ci_ispost_reg.h"

enum ISPOST_DEBUGFS_TYPE {
	DEBUGFS_STATUS = 0x0,
	DEBUGFS_DN_STATUS,
};

#define ISPOST_HW_VERSION 0x1 /* this version for ispostv2 */
#define DEBUGFS_MOD 0555

#define ISPOST_DEBUGFS_HW_VERSION "hw_version"
#define ISPOST_DEBUGFS_DN_STATUS "dn_status"
#define ISPOST_DEBUGFS_STATUS "status"

static struct dentry *ispost_debugfs_dir = NULL;

static struct dentry *ispost_status = NULL;
static struct dentry *ispost_hw_version = NULL;
static struct dentry *ispost_dn_status = NULL;

static int g_ispost_hw_version = ISPOST_HW_VERSION;
static int g_ispost_dn_status = 0;

ISPOST_UINT32 g_ui32IspostCount = 0;
ISPOST_UINT32 g_ui32Ctrl0 = 0;
#endif
wait_queue_head_t ispost_wq;
static struct mutex trigger_lock;	/* mainly lock for decode instance count */
static struct mutex inst_lock;
static int inst = 0;
static int eventflag = 0;

#ifdef ISPOST_DEBUGFS
void print_ispost_status(char *buf, int* sz)
{
	*sz = 0;

	if(inst == 0)
		return ;

	sprintf(buf+strlen(buf), "IspostCount:%d\n"
				"UserCtrl0:0x%08x\n"
				"Ctrl0:0x%08x\n"
				"Stat0:0x%08x\n",
				g_ui32IspostCount,
				g_ui32Ctrl0,
				IspostInterruptGet().ui32Ctrl0,
				IspostStatusGet().ui32Stat0);

	*sz = strlen(buf);
	/* printk("sz=%d, %s\n", *sz, buf); */
}

void print_dn_stauts(char*buf, int *sz)
{
	ISPOST_UINT32 ref_y = 0x0;
	ISPOST_UINT32 en_dn = 0x0;

	if(inst == 0)
		return ;

	ref_y = IspostRegRead(ISPOST_DNRIAY);
	en_dn = IspostInterruptGet().ui1EnDN;

	if (en_dn && (ref_y != 0x00000000 && ref_y !=0xffffffff))
		g_ispost_dn_status = 1;
	else
		g_ispost_dn_status = 0;

	sprintf(buf+strlen(buf),
		"Stat:%d\nDnEn:%d\nRefY:0x%08x\n",
		g_ispost_dn_status, en_dn, ref_y);
	*sz = strlen(buf);

	/* printk("sz=%d, %s\n", *sz, buf); */
}

static ssize_t debugfs_read(char __user *buff,
    size_t count, loff_t *ppos,enum ISPOST_DEBUGFS_TYPE type)
{
	char *str;
	ssize_t len = 0;

	str = kzalloc(count, GFP_KERNEL);
	if (!str)
		return -ENOMEM;

	switch (type) {
	case DEBUGFS_STATUS:
		print_ispost_status(str , &len);
		break;
	case DEBUGFS_DN_STATUS:
		print_dn_stauts(str, &len);
	default:
		break;
	}

	if (len < 0)
		ISPOST_ERR("Can't read data\n");
	else
		len = simple_read_from_buffer(buff, count, ppos, str, len);

	kfree(str);

	return len;
}

static ssize_t debugfs_status_read(struct file *file, char __user *buff,
		size_t count, loff_t *ppos)
{
	ssize_t len = 0;

	if (*ppos < 0 || !count)
		return -EINVAL;

	len = debugfs_read(buff, count, ppos, DEBUGFS_STATUS);
	return len;
}

static const struct file_operations debugfs_status_ops = {
	.open        = nonseekable_open,
	.read        = debugfs_status_read,
	.llseek        = no_llseek,
};

static ssize_t debugfs_dn_status_read(struct file *file, char __user *buff,
		size_t count, loff_t *ppos)
{
	ssize_t len = 0;

	if (*ppos < 0 || !count)
		return -EINVAL;

	len = debugfs_read(buff, count, ppos, DEBUGFS_DN_STATUS);
	return len;
}

static const struct file_operations debugfs_dn_status_ops = {
	.open        = nonseekable_open,
	.read        = debugfs_dn_status_read,
	.llseek        = no_llseek,
};

static void ispost_debugfs_create(void)
{
	if (!ispost_debugfs_dir) {
		ispost_debugfs_dir = debugfs_create_dir("ispost", NULL);
	}

	if (!ispost_debugfs_dir) {
		ISPOST_ERR("Failed to create debugfs directory '%s'\n", "ispost");
		ispost_debugfs_dir = NULL;
	} else {
		ispost_hw_version = debugfs_create_x32(ISPOST_DEBUGFS_HW_VERSION,
			DEBUGFS_MOD, ispost_debugfs_dir, &g_ispost_hw_version);
		if (!ispost_hw_version) {
			ISPOST_ERR("failed to create %s\n", ISPOST_DEBUGFS_HW_VERSION);
		}

		ispost_status = debugfs_create_file(ISPOST_DEBUGFS_STATUS,
					S_IFREG | S_IRUSR | S_IRGRP,
					ispost_debugfs_dir, NULL, &debugfs_status_ops);
		if (!ispost_status)
			ISPOST_ERR("cannot create debugfs %s\n", ISPOST_DEBUGFS_STATUS);

		ispost_dn_status = debugfs_create_file(ISPOST_DEBUGFS_DN_STATUS,
					S_IFREG | S_IRUSR | S_IRGRP,
					ispost_debugfs_dir, NULL, &debugfs_dn_status_ops);
		if (!ispost_dn_status )
			ISPOST_ERR("cannot create debugfs %s\n", ISPOST_DEBUGFS_DN_STATUS);
	}
}

static void ispost_debugfs_clean(void)
{
	if (ispost_debugfs_dir != NULL) {
		ISPOST_INFO("clean ispost debugfs\n");
		debugfs_remove_recursive(ispost_debugfs_dir);
	}
}
#endif

static int ispost_open(struct inode *fi, struct file *fp)
{
	int ret = 0;
	mutex_lock(&inst_lock);

	if (inst == 0) {
		ret = ispost_module_enable();
		if (ret)
			goto fail;
	}
	inst++;
fail:
	mutex_unlock(&inst_lock);

	ISPOST_INFO("%s\n", __func__);
	return ret;
}

static int ispost_release(struct inode *fi, struct file *fp)
{
	mutex_lock(&inst_lock);
	inst--;	
	if(inst == 0)
		ispost_module_disable();
	mutex_unlock(&inst_lock);

	ISPOST_INFO("%s\n", __func__);
	return 0;
}

static long ispost_ioctl(struct file *file,	unsigned int cmd, unsigned long arg)
{
	int ret;
	ISPOST_USER IspostUser;
	int IsBusy = ISPOST_FALSE;

	if (_IOC_TYPE(cmd) != ISPOST_MAGIC || _IOC_NR(cmd) > ISPOST_IOCMAX)
		return -ENOTTY;

	switch (cmd){
		case ISPOST_ENV:
			ISPOST_INFO("Ispost 1.0\n");
			break;

		case ISPOST_FULL_TRIGGER:
			if (copy_from_user(&IspostUser, (ISPOST_USER*)arg, sizeof(ISPOST_USER))){
				ISPOST_ERR("ISPOST_INITIAL: from user space error\n");
				return -EFAULT;
			}
#ifdef ISPOST_DEBUGFS
			g_ui32IspostCount = IspostUser.ui32IspostCount;
			g_ui32Ctrl0 = IspostUser.stCtrl0.ui32Ctrl0;
#endif
			mutex_lock(&trigger_lock);

			if (IspostUserSetup(&IspostUser) != ISPOST_TRUE){
				ISPOST_ERR("ISPOST_INITIAL: Call IspostUserSetup() fail\n");
				mutex_unlock(&trigger_lock);
				return -EFAULT;
			}
			if (IspostUserTrigger(&IspostUser) != ISPOST_TRUE){
				ISPOST_ERR("ISPOST_TRIGGER: Call IspostUserTrigger() fail\n");
				mutex_unlock(&trigger_lock);
				return -EFAULT;
			}
			eventflag = 0;
			ret = wait_event_timeout(ispost_wq, eventflag, HZ);
			if (ret == 0) {
				printk("ispost timeout\n");
				mutex_unlock(&trigger_lock);
				return -EFAULT;
			}
			IsBusy = IspostUserIsBusy();
			mutex_unlock(&trigger_lock);

			return IsBusy;
		default:
			ISPOST_ERR("Do not support this cmd: %d\n", cmd);
	}
	return 0;
}

static int ispost_read(struct file *filp, char __user *buf, size_t length, loff_t *offset)
{
	ISPOST_INFO("%s\n", __func__);
	return 0;
}

static ssize_t ispost_write(struct file *filp, const char __user *buf, size_t length,
		loff_t *offset)
{
	ISPOST_INFO("%s\n", __func__);
	return length;
}

static const struct file_operations ispost_fops =
{
	.open  = ispost_open,
	.release = ispost_release,
	.read  = ispost_read,
	.write = ispost_write,
	.unlocked_ioctl = ispost_ioctl,
};

static int ispost_major = 0;

static irqreturn_t ispost_irqHandler(int irq, void *dev_id)
{
	IspostInterruptClear();

	if(!IspostUserIsBusy()){
		eventflag = 1;
		wake_up(&ispost_wq);
	}
	return (IRQ_HANDLED);
}

static int __init ispost_init(void)
{
	struct class *cls;
	int ret = -1;

	ISPOST_INFO("ispost driver (c) 2012, 2017 InfoTM\n");

	/* create ispost dev node */
	ispost_major = register_chrdev(ispost_major, ISPOST_NAME, &ispost_fops);
	if (ispost_major < 0) {
		ISPOST_ERR("Register char device for ispost failed.\n");
		return -EFAULT;
	}

	cls = class_create(THIS_MODULE, ISPOST_NAME);
	if(!cls) {
		ISPOST_ERR("Can not register class for ispost .\n");
		return -EFAULT;
	}
	/* create device node */
	device_create(cls, NULL, MKDEV(ispost_major, 0), NULL, ISPOST_NAME);

#ifdef ISPOST_DEBUGFS
	ispost_debugfs_create();
#endif
	init_waitqueue_head(&ispost_wq);

	mutex_init(&trigger_lock);
	mutex_init(&inst_lock);
	ret = request_irq(IRQ_ISPOST, ispost_irqHandler, IRQF_DISABLED, ISPOST_NAME, NULL);
 	if (ret < 0){
		ISPOST_ERR("Request ispost irq failed\n");
 		return -EFAULT;
 	}
	return 0;
}

static void __exit ispost_exit(void)
{
	unregister_chrdev(ispost_major, ISPOST_NAME);
	mutex_destroy(&trigger_lock);
	mutex_destroy(&inst_lock);
#ifdef ISPOST_DEBUGFS
	ispost_debugfs_clean();
#endif
}

module_init(ispost_init);
module_exit(ispost_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("JazzChang <jazz.chang@infotm.com>, GodspeedKuo <godspeed.kuo@infotm.com>");
MODULE_DESCRIPTION("ispost driver");

