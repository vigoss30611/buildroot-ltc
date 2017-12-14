/*****************************************************************************
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: Fodet driver.
**
*****************************************************************************/
#include "drv_fodet.h"

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
#include "ci_fodet.h"

#define IRQ_FODET GIC_FODET_ID

wait_queue_head_t fodet_wq;
static struct mutex fodet_trigger_lock;	/* mainly lock for decode instance count */
static struct mutex fodet_inst_lock;
static int fodet_inst = 0;
static int fodet_eventflag = 0;
//int fodet_eventflag = 0;

static struct class *chrdev_sys_class = NULL;


void fodet_wait_event(void)
{
     fodet_eventflag = 0;
     wait_event(fodet_wq, fodet_eventflag);
}

void fodet_wait_event_timeout(FODET_UINT32 time)
{
     fodet_eventflag = 0;
     wait_event_timeout(fodet_wq, fodet_eventflag, time);
}

static int fodet_open(struct inode *fi, struct file *fp)
{
    mutex_lock(&fodet_trigger_lock);
	
    if(fodet_inst == 0)
        fodet_module_enable();
    fodet_inst++;
    mutex_unlock(&fodet_trigger_lock);

    FODET_INFO("%s\n", __func__);
    return 0;
}

static int fodet_release(struct inode *fi, struct file *fp)
{
    mutex_lock(&fodet_trigger_lock);
    fodet_inst--;	
    if(fodet_inst == 0)
        fodet_module_disable();
    mutex_unlock(&fodet_trigger_lock);
    
    FODET_INFO("%s\n", __func__);
    return 0;
}

static long fodet_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    FODET_USER* pFodetUser;
    FODET_USER FodetUser;

    if (_IOC_TYPE(cmd) != FODET_MAGIC || _IOC_NR(cmd) > FODET_IOCMAX){
        FODET_ERR("cmd error !!!\n");
        return -ENOTTY;
    }
    switch (cmd)
    {
    case FODET_ENV:
        FODET_INFO("Q3F Fodet 2.0 irq support\n");
        break;
    
    case FODET_INITIAL:
        if (copy_from_user(&FodetUser, (FODET_USER*)arg, sizeof(FODET_USER)))
        {
            FODET_ERR("FODET_INITIAL: from user space error\n");
            return -EFAULT;
        }
        if (FodetUserSetup(&FodetUser) != FODET_TRUE)
        {
            FODET_ERR("FODET_INITIAL: Call FodetUserSetup() fail\n");
            return -EFAULT;
        }
        break;
    
    case FODET_OBJECT_DETECT:
        if (copy_from_user(&FodetUser, (FODET_USER*)arg, sizeof(FODET_USER)))
        {
            FODET_ERR("FODET_OBJECT_DETECT : from user space error\n");
            return -EFAULT;
        }
        //mutex_lock(&fodet_trigger_lock);
        FodetUserObjectRecongtion(&FodetUser);
        //mutex_unlock(&fodet_trigger_lock);
        break;
    
    case FODET_GET_COORDINATE:
        pFodetUser = (FODET_USER*)arg;
        if(iCandidateCount)
            pFodetUser->ui16CoordinateNum = (FODET_UINT16)iCandidateCount;
        if(copy_to_user((void*)pFodetUser->ui32CoordinateAddr, (void*)avgComps1, (iCandidateCount * sizeof(CvAvgComp))) != 0){
            FODET_ERR("copy array of coordinate to user fail!\n");
            return -EFAULT;
        }
        break;
    
    default:
        FODET_ERR("Do not support this cmd: %d\n", cmd);
        break;
    }
    return 0;
}

static int fodet_read(struct file *filp, char __user *buf, size_t length, loff_t *offset)
{
    FODET_INFO("%s\n", __func__);
    return 0;
}

static ssize_t fodet_write(struct file *filp, const char __user *buf, size_t length, loff_t *offset)
{
    FODET_INFO("%s\n", __func__);
    return length;
}

static const struct file_operations fodet_fops =
{
    .open  = fodet_open,
    .release = fodet_release,
    .read  = fodet_read,
    .write = fodet_write,
    .unlocked_ioctl = fodet_ioctl,
};

static int fodet_major = 0;
int int_cnt=0;

static irqreturn_t fodet_irqHandler(int irq, void *dev_id)
{
    FodetIntStatusClear();
    
    fodet_eventflag = 1;
    wake_up(&fodet_wq);
    
    return (IRQ_HANDLED);
}

static int __init fodet_init(void)
{
    struct class *cls;
    int ret = -1;
    
    FODET_INFO("fodet driver (c) 2012, 2017 InfoTM\n");
    
    /* create fodet dev node */
    fodet_major = register_chrdev(fodet_major, FODET_NAME, &fodet_fops);
    if (fodet_major < 0) {
        FODET_ERR("Register char device for fodet failed.\n");
        return -EFAULT;
    }

    cls = class_create(THIS_MODULE, FODET_NAME);
    if(!cls) {
        FODET_ERR("Can not register class for fodet .\n");
        return -EFAULT;
    }
    chrdev_sys_class = cls;

    /* create device node */
    device_create(cls, NULL, MKDEV(fodet_major, 0), NULL, FODET_NAME);
    
    init_waitqueue_head(&fodet_wq);
    
    mutex_init(&fodet_trigger_lock);
    mutex_init(&fodet_inst_lock);
    ret = 0;

    ret = request_irq(IRQ_FODET, fodet_irqHandler, IRQF_DISABLED, FODET_NAME, NULL);
    if (ret < 0){
        FODET_ERR("Request fodet irq failed\n");
        return -EFAULT;
    }
    return ret;
}

static void __exit fodet_exit(void)
{
    dev_t dev = MKDEV(fodet_major,0);
    // class
    device_destroy(chrdev_sys_class, dev);
    class_destroy(chrdev_sys_class);
    // chrdev
    //cdev_del(&fodet_cdev);
        
    unregister_chrdev(fodet_major, FODET_NAME);
    mutex_destroy(&fodet_trigger_lock);
    mutex_destroy(&fodet_inst_lock);
}

module_init(fodet_init);
module_exit(fodet_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("JazzChang <jazz.chang@infotm.com>, GodspeedKuo <godspeed.kuo@infotm.com>");
MODULE_DESCRIPTION("fodet driver");

