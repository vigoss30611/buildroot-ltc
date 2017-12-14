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
#include <mach/irqs.h>
#include "ci_ispost.h"

#define IRQ_ISPOST GIC_ISPOST0_ID

wait_queue_head_t ispost_wq;
static struct mutex trigger_lock;	/* mainly lock for decode instance count */
static struct mutex inst_lock;
static int inst = 0;
static int eventflag = 0;

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
	ISPOST_USER IspostUser;
	int IsBusy = ISPOST_FALSE, ret;

	if (_IOC_TYPE(cmd) != ISPOST_MAGIC || _IOC_NR(cmd) > ISPOST_IOCMAX)
		return -ENOTTY;

	switch (cmd){
		case ISPOST_ENV:
			ISPOST_INFO("Ispostv2 1.0\n");
			break;

		case ISPOST_INIT:
			if (copy_from_user(&IspostUser, (ISPOST_USER*)arg, sizeof(ISPOST_USER))) {
				ISPOST_ERR("ISPOST_INIT: from user space error\n");
				return -EFAULT;
			}
			mutex_lock(&trigger_lock);
			if (ISPOST_TRUE != IspostUserInit(&IspostUser)) {
				ISPOST_ERR("ISPOST_INIT: Call IspostUserTrigger() fail\n");
				mutex_unlock(&trigger_lock);
				return -EFAULT;
			}
			mutex_unlock(&trigger_lock);
			break;

		case ISPOST_UPDATE:
			if (copy_from_user(&IspostUser, (ISPOST_USER*)arg, sizeof(ISPOST_USER))) {
				ISPOST_ERR("ISPOST_UPDATE: from user space error\n");
				return -EFAULT;
			}
			mutex_lock(&trigger_lock);
			if (ISPOST_TRUE != IspostUserUpdate(&IspostUser)) {
				ISPOST_ERR("ISPOST_UPDATE: Call IspostUserTrigger() fail\n");
				mutex_unlock(&trigger_lock);
				return -EFAULT;
			}
			mutex_unlock(&trigger_lock);
			break;

		case ISPOST_TRIGGER:
			if (copy_from_user(&IspostUser, (ISPOST_USER*)arg, sizeof(ISPOST_USER))) {
				ISPOST_ERR("ISPOST_TRIGGER: from user space error\n");
				return -EFAULT;
			}
			mutex_lock(&trigger_lock);
			if (ISPOST_TRUE != IspostUserTrigger(&IspostUser)) {
				ISPOST_ERR("ISPOST_TRIGGER: Call IspostUserTrigger() fail\n");
				mutex_unlock(&trigger_lock);
				return -EFAULT;
			}
			eventflag = 0;
			mutex_unlock(&trigger_lock);
			return IsBusy;

		case ISPOST_WAIT_CPMPLETE:
			if (copy_from_user(&IspostUser, (ISPOST_USER*)arg, sizeof(ISPOST_USER))) {
				ISPOST_ERR("ISPOST_WAIT_CPMPLETE: from user space error\n");
				return -EFAULT;
			}
			mutex_lock(&trigger_lock);
			//eventflag = 0;
			wait_event(ispost_wq, eventflag);
			IsBusy = IspostUserIsBusy();
			if (IspostUserCompleteCheck(&IspostUser)) {
				IsBusy = ISPOST_TRUE;
			}
			mutex_unlock(&trigger_lock);
			return IsBusy;
		case ISPOST_FULL_TRIGGER:
			if (copy_from_user(&IspostUser, (ISPOST_USER*)arg, sizeof(ISPOST_USER)))
			{
				ISPOST_ERR("ISPOST_FULL_TRIGGER: from user space error\n");
				return -EFAULT;
			}
			mutex_lock(&trigger_lock);

			if (IspostUserFullTrigger(&IspostUser) != ISPOST_TRUE)
			{
				ISPOST_ERR("ISPOST_FULL_TRIGGER: Call IspostUserFullTrigger() fail\n");
				mutex_unlock(&trigger_lock);
				return -EFAULT;
			}

			eventflag = 0;
			ret = wait_event_timeout(ispost_wq, eventflag, HZ);
			if (ret == 0) {
				printk("ispost timeout\n");
				printk("@@@ IspostCount = %d. stCtrl0 = 0x%08X, RegCtrl0 = 0x%08X, RegStat0 = 0x%08X @@@\n",
					IspostUser.ui32IspostCount,
					IspostUser.stCtrl0.ui32Ctrl0,
					IspostInterruptGet().ui32Ctrl0,
					IspostStatusGet().ui32Stat0
					);
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

static ssize_t ispost_write(struct file *filp, const char __user *buf, size_t length, loff_t *offset)
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
	//ISPOST_INFO("--- Ispostv2 irq ---\n");

	if(!IspostUserIsBusy())
	{
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
	if (ispost_major < 0)
	{
		ISPOST_ERR("Register char device for ispost failed.\n");
		return -EFAULT;
	}

	cls = class_create(THIS_MODULE, ISPOST_NAME);
	if(!cls)
	{
		ISPOST_ERR("Can not register class for ispost .\n");
		return -EFAULT;
	}
	/* create device node */
	device_create(cls, NULL, MKDEV(ispost_major, 0), NULL, ISPOST_NAME);

	init_waitqueue_head(&ispost_wq);

	mutex_init(&trigger_lock);
	mutex_init(&inst_lock);
	ret = request_irq(IRQ_ISPOST, ispost_irqHandler, IRQF_DISABLED, ISPOST_NAME, NULL);
 	if (ret < 0)
 	{
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
}

module_init(ispost_init);
module_exit(ispost_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("JazzChang <jazz.chang@infotm.com>, GodspeedKuo <godspeed.kuo@infotm.com>");
MODULE_DESCRIPTION("ispostv2 driver");

