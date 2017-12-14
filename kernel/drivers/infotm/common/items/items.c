/***************************************************************************** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Infotm board configuration system.
**
** Author:
**     warits <warits.wang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.1  04/09/2012
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
#include <mach/items.h>
#include <mach/rballoc.h>
#include <mach/mem-reserve.h>

static void __iomem *items_dtra = 0;

static long items_dev_ioctl(struct file *file,
			unsigned int cmd, unsigned long arg)                       
{
	struct items_query qt;
	int ret;
	int init_vals[2] = {0, 0};
	if(_IOC_TYPE(cmd) != ITEMS_MAGIC
	   || _IOC_NR(cmd) > ITEMS_IOCMAX)
	  return -ENOTTY;

	//printk("--->items_dev_ioctl\n");
	/* take the return value just to make gcc happy */
	if(cmd != ITEMS_REINIT && cmd != ITEMS_GETTR)
	    ret = copy_from_user(&qt, (struct items_query *)arg,
		    sizeof(struct items_query));

	switch(cmd)
	{
		case ITEMS_EXIST:
			ret = item_exist(qt.key);
			//printk(KERN_ERR "items: exist: key=%s, ret=%d\n", qt.key, ret);
			return ret;
		case ITEMS_INTEGER:
			ret = item_integer(qt.key, qt.section);
			//printk(KERN_ERR "items: integer: key=%s, ret=%d\n", qt.key, ret);
			return ret;
		case ITEMS_EQUAL:
			ret = item_equal(qt.key, qt.value, qt.section);
			//printk(KERN_ERR "items: equal: key=%s, v=%s, sec=%d, ret=%d\n", qt.key, qt.value, qt.section, ret);
			return ret;
		case ITEMS_ITEM:
			ret = item_string_item(qt.fb, qt.key, qt.section);
			//printk(KERN_ERR "items: items: fb=%s, key=%s, sec=%d, ret=%d\n", qt.fb, qt.key, qt.section, ret);
			if(ret) goto error;
			else break ;
		case ITEMS_STRING:
			//printk("-->qt.fb = %x, qt.key = %s, qt.section = %d\n", qt.fb, qt.key, qt.section);
			ret = item_string(qt.fb, qt.key, qt.section);
			//printk(KERN_ERR "items: string: fb=%s, key=%s, sec=%d, ret=%d\n", qt.fb, qt.key, qt.section, ret);
			if(ret) goto error;
			else break ;
		case ITEMS_REINIT:
			//ret = copy_from_user((char *)init_vals , arg , 8);
			//ret = item_init((char *)init_vals[0] , init_vals[1]);
			//printk(KERN_ERR "reinit items at: 0x%8x, len:0x%x\n", init_vals[0], init_vals[1]);
			//if(ret) goto error;
                          printk(KERN_ERR "ERR: ITEMS_REINIT func. not open\n");
                          goto error;
			break;
		case ITEMS_GETTR:
			ret = copy_to_user((char *)arg, items_dtra, 0x1000);
			return 0;
		default:
			goto error;
	}
#if 1
	return copy_to_user((void *)arg, &qt, sizeof(struct items_query));
#else// ??? for debug
	{
		long ret = (long)copy_to_user((void *)arg, &qt, sizeof(struct items_query));
		struct items_query* xx = (struct items_query *)arg;
		printk("******>arg=%x %x=%s, %s, %d ret = %d\n", arg, xx->fb, xx->fb, xx->key, xx->section, ret);
		return ret;
	}
#endif
error:
	return -EFAULT;
}

int items_dev_open(struct inode *fi, struct file *fp)
{
//	printk(KERN_ERR "items opened.\n");
    /* reset export pos */
    item_export(NULL, 0);
    return 0;
}

#if 0
static DEFINE_SPINLOCK(ddf_lock);
extern void __ddf(void *, void *, uint32_t, uint32_t);
#endif
static int items_dev_read(struct file *filp,
   char __user *buf, size_t length, loff_t *offset)
{
#if 0
	unsigned long flags;
	void (*ddf)(void *, void *, uint32_t, uint32_t)
		= ioremap_nocache(IMAP_IRAM_BASE + 0x18000, 0x2000);

	printk(KERN_ERR "prepare function to GRAM: Gx%p, Vx%p\n", ddf, __ddf);
	memcpy((void *)ddf, (void *)__ddf, 0x2000);
	printk(KERN_ERR "reset DDR to 333M\n");

	spin_lock_init(&ddf_lock);

	spin_lock_irqsave(&ddf_lock, flags);
	__ddf((IMAP_EMIF_BASE), (SYSMGR_CLKGEN_BASE)
			+ 3 * 0x18, 0x21, 0x1036);
	spin_unlock_irqrestore(&ddf_lock, flags);

	return 0;
#endif
	return item_export(buf, length);
}

static ssize_t items_dev_write(struct file *filp,
   const char __user *buf, size_t length, loff_t *offset)
{
    return 0;
}

static const struct file_operations items_dev_fops = {
	.read = items_dev_read,
	.write = items_dev_write,
	.open = items_dev_open,
	.unlocked_ioctl = items_dev_ioctl,
};

static int __init items_dev_init(void)
{
	struct class *cls;
	int ret;

	printk(KERN_INFO "items driver (c) 2009, 2014 InfoTM\n");

	/* create gps dev node */
	ret = register_chrdev(ITEMS_MAJOR, ITEMS_NAME, &items_dev_fops);
	if(ret < 0)
	{
		printk(KERN_ERR "Register char device for items failed.\n");
		return -EFAULT;
	}

	cls = class_create(THIS_MODULE, ITEMS_NAME);
	if(!cls)
	{
		printk(KERN_ERR "Can not register class for items .\n");
		return -EFAULT;
	}

	/* create device node */
	device_create(cls, NULL, MKDEV(ITEMS_MAJOR, 0), NULL, ITEMS_NAME);

	items_dtra = kmalloc(0x1000, GFP_KERNEL);
	if(!items_dtra)
	    printk(KERN_ERR "Alloc items_dtra buffer failed.\n");
	else {
	    void *__b = rbget("dramdtra");
	    if(__b) memcpy(items_dtra, __b, 0x1000);
	}

	return 0;
}

static void __exit items_dev_exit(void) {}

module_init(items_dev_init);
module_exit(items_dev_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("warits <warits.wang@infotmic.com.cn>");
MODULE_DESCRIPTION("items interface device export");

