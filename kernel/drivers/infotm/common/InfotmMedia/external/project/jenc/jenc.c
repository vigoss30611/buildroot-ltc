/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Description: Jpeg encoder module
 *
 * Author:
 *     devin <devin.zhu@infotm.com>
 *
 */
/*include header*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <asm/irq.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
/* needed for virt_to_phys() */
#include <asm/io.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>

#include <mach/power-gate.h>
#include "mach/clock.h"
#include <linux/clk.h>



#include "jenc.h"
/* module description */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Infotm");
MODULE_DESCRIPTION("Infotm Jpeg Encoder driver");

static int jenc_probe(struct platform_device *pdev);
static int jenc_remove(struct platform_device *pdev);
static int jenc_suspend(struct platform_device *pdev, pm_message_t state);
static int jenc_resume(struct platform_device *pdev);
static long jenc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int jenc_open(struct inode *inode, struct file *filp);
static int jenc_release(struct inode *inode, struct file *filp);
static unsigned int jenc_poll(struct file *file, struct poll_table_struct *wait);
static int __init jenc_init(void);

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
static irqreturn_t jenc_isr(int irq, void *dev_id, struct pt_regs *regs);
#else
static irqreturn_t jenc_isr(int irq, void *dev_id);
#endif


static wait_queue_head_t wait_encode;
static volatile unsigned int encode_poll_mark = 0;
static struct mutex lock;
unsigned int clkRate = 12000000;
u32 jenc_reg_base_adr = 0x25100000;
u32 jenc_reg_sys_adr = 0x2D030400;//0x2000 0000 + 0xD00 0000 + 0x3 0400
u8 reg_size = 0x30;
u16 irq = 131;

static struct clk * driverClk = NULL;
static struct clk * driverbusClk = NULL;
static unsigned int driverOpenCount = 0;

#define DRIVER_POWER_BASE 0x2D030400

#define __pow_readl(x)           __raw_readl(IO_ADDRESS(x))
#define __pow_writel(val, x)     __raw_writel(val, IO_ADDRESS(x))

module_param(clkRate, uint, 0);


typedef struct
{
    char *buffer;
    unsigned int buffsize;
    unsigned int iobaseaddr;
    void __iomem *jenc_virt;
    unsigned int iosize;
    volatile u8 *hwregs;
    unsigned int irq;
} jenc_t;


static jenc_t jenc_data;


static struct file_operations jenc_fops = {
  open:jenc_open,
  release:jenc_release,
  unlocked_ioctl:jenc_ioctl,
  poll: jenc_poll,
};


static struct miscdevice jenc_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &jenc_fops,
    .name = JENC_DEV_NAME,
};

static struct platform_driver jenc_plat = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "imax-jenc",
    },
    .probe = jenc_probe,
    .remove = jenc_remove,
    .suspend = jenc_suspend,
    .resume = jenc_resume,
};

static void powerOnDriver(void)
{
    mutex_lock(&lock);
    if(clk_prepare_enable(driverClk)) {
        printk("ERROR: ~~~~driverClk clk_prepare_enable failed...\n");
    }
    module_power_on(SYSMGR_JENC_BASE);
    mutex_unlock(&lock);
}

static void powerOffDriver(void)
{
    mutex_lock(&lock);
    module_power_down(SYSMGR_JENC_BASE);
    clk_disable_unprepare(driverClk);
    mutex_unlock(&lock);
}
static int jenc_open(struct inode *inode, struct file *filp) {
    //power on
    //powerOnDriver();
    jenc_t *dev = &jenc_data;
    filp->private_data = (void *)dev;
    return 0;
}

static int jenc_release(struct inode *inode, struct file *filp) {
    //power down
    return 0;
}

static unsigned int jenc_poll(struct file *file, struct poll_table_struct *wait) {
    int mask;
    int val;
    printk("jenc_poll 1\n");
    poll_wait(file, &wait_encode, wait);
    //while( !(__pow_readl(jenc_reg_base_adr) & (1 << 24))) {

    //}
    val = readl(jenc_data.jenc_virt);
    printk("\njenc_poll 1-2, val:0x%x\n", val);
    if (encode_poll_mark != 0) {
        mask = POLLIN | POLLRDNORM;
        printk("jenc_poll finish encode\n");
        encode_poll_mark = 0;
    }
    return 0;
}

static int jenc_probe(struct platform_device *pdev) {
    printk("jenc_probe\n");
    return 0;
}

static int jenc_remove(struct platform_device *pdev) {
    printk("jenc_remove\n");
    return 0;
}

static int jenc_suspend(struct platform_device *pdev, pm_message_t state) {
    printk("jenc_suspend\n");
    return 0;
}

static int jenc_resume(struct platform_device *pdev) {
    printk("jenc_resume\n");
    return 0;
}

static long jenc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    unsigned offset = 0;
    switch (cmd)
    {
        case JENC_IOCGHWOFFSET:
            __put_user(jenc_data.iobaseaddr, (unsigned long *) arg);
            break;
        case JENC_IOCGHWIOSIZE:
            __put_user(jenc_data.iosize, (unsigned int *) arg);
            break;
        case JENC_IOCG_CORE_OFF:
            {
                powerOffDriver();
                break;
            }
        case JENC_IOCG_CORE_ON:
            {
                powerOnDriver();
                break;
            }
        default:
            printk("jenc ioctl default::%d\n", cmd);
            break;
    }
    return 0;
}

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
static irqreturn_t jenc_isr(int irq, void *dev_id, struct pt_regs *regs)
#else
static irqreturn_t jenc_isr(int irq, void *dev_id)
#endif
{
    jenc_t *dev = (jenc_t *) dev_id;
    u32 irq_status;

    irq_status = readl(dev->jenc_virt);
    printk("enter jenc_isr irq_status:0x%x\n", irq_status);
    if(irq_status & (0x01 << 24)) {

        /* clear jenc IRQ */
        //writel(irq_status & (~(0x01 << 24)), dev->jenc_virt);
        printk("enter jenc_isr irq_status2:0x%x\n", readl(dev->jenc_virt));
        encode_poll_mark = 1;
        wake_up(&wait_encode);
        printk("IRQ handled!\n");
        return IRQ_HANDLED;
    }

    printk("IRQ received, but NOT handled!\n");
    return IRQ_NONE;
}

static int __init jenc_init(void)
{
    int ret = 0;
    mutex_init(&lock);
    driverClk = clk_get_sys("imap-jenc", "imap-jenc");
    printk("clk_get_rate:%ld\n",clk_get_rate(driverClk));
    //ret = clk_set_rate(driverClk, clkRate);
    clk_prepare_enable(driverClk);
    printk("ret:%d clk_get_rate:%ld\n",ret, clk_get_rate(driverClk));
    module_power_on(SYSMGR_JENC_BASE);
    /* Register platform device driver. */
    ret = platform_driver_register(&jenc_plat);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register jenc platform device driver error, %d.\n", ret);
        return ret;
    }

    /* Register misc device driver. */
    ret = misc_register(&jenc_miscdev);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register venc misc device driver error, %d.\n", ret);
        return ret;
    }

    jenc_data.iobaseaddr = jenc_reg_base_adr;
    jenc_data.iosize = reg_size;
    jenc_data.jenc_virt = ioremap_nocache(jenc_reg_base_adr, reg_size);
    jenc_data.irq = -1;//irq;
    //init_waitqueue_head(&wait_encode);
    printk("read jenc_virt jenc_data.jenc_virt:0x%x\n",jenc_data.jenc_virt);
    //printk("kernel 0x00 value:0x%x\n",readl(jenc_data.jenc_virt));
#if 0
    if(irq != -1) {
        ret = request_irq(irq, jenc_isr,
        #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
                                 SA_INTERRUPT | SA_SHIRQ,
        #else
                                 IRQF_DISABLED | IRQF_SHARED,
        #endif
                                 JENC_DEV_NAME, (void *) &jenc_data);
        if(ret == -EINVAL) {
            printk(KERN_ERR "jenc: Bad irq number or handler\n");
            //ReleaseIO();
            goto err;
        } else if(ret == -EBUSY) {
            printk(KERN_ERR "jenc: IRQ <%d> busy, change your config\n", jenc_data.irq);
            //ReleaseIO();
            goto err;
        }
    } else {
            printk(KERN_INFO "jenc: IRQ not in use!\n");
    }
#endif
   return 0;
err:
    printk(KERN_INFO "jenc: module not inserted\n");
    return ret;
}

static void __exit jenc_exit(void)
{
    misc_deregister(&jenc_miscdev);
    platform_driver_unregister(&jenc_plat);
    /* free the encoder IRQ */
    if(jenc_data.irq != -1) {
        free_irq(jenc_data.irq, (void *) &jenc_data);
    }
    powerOffDriver();
    mutex_destroy(&lock);
    printk("jenc_exit\n");
}

module_init(jenc_init);
module_exit(jenc_exit);
