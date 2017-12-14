/*
 *
 * Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
 *
 * Use of Infotm's code is governed by terms and conditions
 * stated in the accompanying licensing statement.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Revision History:
 *  v1.0.2	leo@2012/06/05: tidy up the code.
 *  v1.0.3	leo@2012/06/18: add suspend&resume support.
 *  		leo@2012/07/05: add utlz support.
 */

#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/types.h>    
#include <linux/init.h>      
#include <linux/string.h>
#include <linux/mm.h>             
#include <linux/wait.h>
#include <linux/ioctl.h>
#include <linux/clk.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include "venc.h"

#include <mach/pad.h>
#include <mach/io.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/items.h>

#include <InfotmMedia.h>
#include <IM_utlzapi.h>
#include <IM_devmmuapi.h>
#include "imapx_dev_mmu.h"
#include <utlz_lib.h>

#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"VENCDRV_I:"
#define WARNHEAD	"VENCDRV_W:"
#define ERRHEAD		"VENCDRV_E:"
#define TIPHEAD		"VENCDRV_T:"


static int venc_open_count; 
static struct mutex venc_lock;
static spinlock_t dvfs_lock;
static struct semaphore venc_sema;
static venc_param_t venc_param;		/* global variables structure */
/* for poll system call */
static wait_queue_head_t wait_encode;
static volatile unsigned int encode_poll_mark = 0;
static bool jpegDvfsClamp = 0;

typedef struct{
 	bool reservedHW;
}venc_instance_t;

/* 
 * Function declaration.
 */
static int venc_open(struct inode *inode, struct file *file);
static int venc_release(struct inode *inode, struct file *file);
static long venc_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int venc_mmap(struct file *file, struct vm_area_struct *vma);
static unsigned int venc_poll(struct file *file, poll_table *wait);
static int venc_probe(struct platform_device *pdev);
static int venc_remove(struct platform_device *pdev);
static int venc_suspend(struct platform_device *pdev, pm_message_t state);
static int venc_resume(struct platform_device *pdev);
static void venc_module_enable(void);
static void venc_module_disable(void);
static void venc_module_reset(void);

static int venc_mmu_enable(void);
static int venc_mmu_disable(void);
//
// DVFS
//
#ifdef CONFIG_VENC_DVFS_SUPPORT
extern utlz_handle_t utlzlib_register_module(utlz_module_t *um);
extern IM_RET utlzlib_unregister_module(utlz_handle_t hdl);
extern IM_RET utlzlib_notify(utlz_handle_t hdl, IM_INT32 code);

typedef struct{
	int	coeffi;		// N/256
	int	up_wnd;
	int	down_wnd;

	long	clkfreq;	// Khz
}dvfs_clock_table_t;

static dvfs_clock_table_t gDvfsClkTbl[] = 
{
	{60, 60, 20, 70000000},
	{90, 10, 20, 100000000},
	{120, 10, 20, 120000000},
	{150, 10, 20, 150000000},
	{180, 10, 20, 180000000},
	{210, 10, 20, 200000000},
	{240, 10, 26, 250000000},
};
#define DVFS_LEVEL_MAX	(sizeof(gDvfsClkTbl) / sizeof(gDvfsClkTbl[0]) - 1)
#define DVFS_LEVEL_MIN	(0)

static utlz_handle_t gUtlzHdl = NULL;
static int gDvfsLevel;

extern void imapx800_freq_message(int module, int freq);

inline static IM_RET dvfs_set_level(IM_INT32 lx)
{
    long real_rate;
	IM_INFOMSG((IM_STR("%s(lx=%d)"), _IM_FUNC_, lx));

    if (jpegDvfsClamp) {
        // the dvfs is clamped by jpeg
        return IM_RET_OK;
    }

    real_rate = utlz_arbitrate_request(gDvfsClkTbl[lx].clkfreq, ARB_VENC);
	//IM_TIPMSG((IM_STR("level=%d, rate=%ldKhz"), gDvfsLevel, real_rate/1000));

    if (real_rate > gDvfsClkTbl[lx].clkfreq) {
        int level = DVFS_LEVEL_MIN;
        while (level <= DVFS_LEVEL_MAX) {
            if (gDvfsClkTbl[level].clkfreq + gDvfsClkTbl[level].up_wnd >= real_rate) {
                break;
            }
            level++;
        }
        if (level > DVFS_LEVEL_MAX) {
            level = DVFS_LEVEL_MAX;
        }
        gDvfsLevel = level;
    } else if (real_rate <= gDvfsClkTbl[lx].clkfreq) {
        spin_unlock(&dvfs_lock);
        real_rate = clk_set_rate(venc_param.busClock, gDvfsClkTbl[lx].clkfreq);
        spin_lock(&dvfs_lock);
        if(real_rate < 0){
            IM_ERRMSG((IM_STR("clk_set_rate(%ldKhz) failed)"), gDvfsClkTbl[lx].clkfreq));
            return IM_RET_FAILED;
        }
        // record the actual clock for this level.
        real_rate = clk_get_rate(venc_param.busClock);
        //gDvfsClkTbl[lx].clkfreq = real_rate;
        gDvfsLevel = lx;	
    }

	IM_INFOMSG((IM_STR("level=%d, rate=%ldKhz"), gDvfsLevel, real_rate/1000));
    //imapx800_freq_message(2, real_rate/1000000);

	return IM_RET_OK;
}

static void venc_utlz_listener(int coeffi)
{
	int level = gDvfsLevel;
	IM_INFOMSG((IM_STR("%s(coeffi=%d), current level %d"), _IM_FUNC_, coeffi, level));

#if 0
    // fast up smooth down
    if(coeffi > gDvfsClkTbl[gDvfsLevel].coeffi + gDvfsClkTbl[gDvfsLevel].up_wnd){
        level = gDvfsLevel;
        if(level < DVFS_LEVEL_MAX){     // fast up.
            dvfs_set_level(DVFS_LEVEL_MAX);
        }
        else if (level == DVFS_LEVEL_MAX){
			IM_TIPMSG((IM_STR("fslevel is already the max value")));
        }
    }else if((coeffi < gDvfsClkTbl[gDvfsLevel].coeffi - gDvfsClkTbl[gDvfsLevel].down_wnd) && (gDvfsLevel > DVFS_LEVEL_MIN)){
        dvfs_set_level(gDvfsLevel - 1); // smooth down.
    }
#else
    if (coeffi > gDvfsClkTbl[level].coeffi + gDvfsClkTbl[level].up_wnd) {
        while (++level <= DVFS_LEVEL_MAX) {
            if (coeffi <= gDvfsClkTbl[level].coeffi + gDvfsClkTbl[level].up_wnd)
                break;
        }
        if (level > DVFS_LEVEL_MAX) {
            level = DVFS_LEVEL_MAX;
			IM_TIPMSG((IM_STR("fslevel is already the max value")));
        }

        spin_lock(&dvfs_lock);
        dvfs_set_level(level);
        spin_unlock(&dvfs_lock);
    } else if ((coeffi < gDvfsClkTbl[level].coeffi - gDvfsClkTbl[level].down_wnd) && (level > DVFS_LEVEL_MIN)){
        spin_lock(&dvfs_lock);
        dvfs_set_level(level-1);
        spin_unlock(&dvfs_lock);
    }
#endif
}

static utlz_module_t gUtlzModule = {
	.name = "venc",
	.period = 1,
	.callback = venc_utlz_listener
};

static IM_RET dvfs_init(void)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	gUtlzHdl = utlzlib_register_module(&gUtlzModule);
	if(gUtlzHdl == IM_NULL){
		IM_ERRMSG((IM_STR("utlzlib_register_module() failed")));
		return IM_RET_FAILED;
	}

    spin_lock(&dvfs_lock);
	dvfs_set_level(DVFS_LEVEL_MAX);
    spin_unlock(&dvfs_lock);
	return IM_RET_OK;
}

static IM_RET dvfs_deinit(void)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if(gUtlzHdl){
		utlzlib_unregister_module(gUtlzHdl);
		gUtlzHdl = IM_NULL;
	}
	return IM_RET_OK;
}
#endif



/***********************************************************************************/
static struct file_operations venc_fops = {
    .owner = THIS_MODULE,
    .open = venc_open,
    .release = venc_release,
    .unlocked_ioctl = venc_ioctl,
    .mmap = venc_mmap,
    .poll = venc_poll,
};
static struct miscdevice venc_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &venc_fops,
    .name = "venc",
};

static irqreturn_t venc_irq_handle(int irq, void *dev_id)
{
	venc_param_t *dev = (venc_param_t *)dev_id;
	unsigned int irq_status = 0;

	/* get interrupt register status */
	irq_status = readl(dev->reg_base_virt_addr + 0x04);

	if (irq_status & 0x01) {
		writel(irq_status & (~0x01), dev->reg_base_virt_addr + 0x04);

		encode_poll_mark = 1;
		wake_up(&wait_encode);
		return IRQ_HANDLED;
	} else {
		IM_ERRMSG((IM_STR("venc interrupt has a error!")));
		return IRQ_HANDLED;
	}
}

static int venc_open(struct inode *inode, struct file *file)
{
    venc_instance_t *inst = NULL;

	IM_INFOMSG((IM_STR("%s(), open count %d"), IM_STR(_IM_FUNC_), venc_open_count));

    inst = (venc_instance_t *)kmalloc(sizeof(venc_instance_t), GFP_KERNEL);
    if (inst == IM_NULL) {
        IM_ERRMSG((IM_STR("venc_open failed because of no enough memory.")));
        return -ENOMEM;
    }
    memset((void *)inst, 0x00, sizeof(venc_instance_t));

    mutex_lock(&venc_lock);

    if (venc_open_count == 0) {
        struct imapx_dmmu_handle_t *tmp_handle;
		venc_module_enable();
        if((tmp_handle = imapx_dmmu_init(DMMU_DEV_VENC)) == NULL){
            IM_ERRMSG((IM_STR("venc dmmu initialize failed")));
		    venc_module_disable();
            mutex_unlock(&venc_lock);
            kfree((void *)inst);
            return -EFAULT;
        }
        venc_param.mmu_handle = tmp_handle;
        venc_param.mmu_inited = true;
	}

    file->private_data = (void *)inst;
    venc_open_count++;
    mutex_unlock(&venc_lock);
	
	return 0;
}

static int checkStatus(void)
{
    return 0;
}

static int venc_release(struct inode *inode, struct file *file)
{
	venc_instance_t *inst = (venc_instance_t *)file->private_data;
    int err;
	IM_INFOMSG((IM_STR("%s(), open count %d"), IM_STR(_IM_FUNC_), venc_open_count));

	mutex_lock(&venc_lock);
	venc_open_count--;

	if (inst->reservedHW == true) {
		inst->reservedHW = false;
		up(&venc_sema);
	}
	
    if (venc_open_count == 0) {
        venc_module_disable();
        if (venc_param.mmu_enabled) {
            venc_mmu_disable();
        }
        venc_param.mmu_enabled = false;
        //FIXME: when would ion_dmmu_deinit fail? what if it failed?
        imapx_dmmu_deinit(venc_param.mmu_handle);
        venc_param.mmu_inited = false;
        venc_param.mmu_handle = NULL;
        jpegDvfsClamp = false;
        if ((err=checkStatus()) != 0) {
            printk("Wrong status %d after releasing. Resources should be cleaned up!\n", err);
        }
    }

    mutex_unlock(&venc_lock);

    kfree((void *)inst);
    return 0;
}

static int venc_mmu_enable(void)
{
    if (imapx_dmmu_enable(venc_param.mmu_handle) != 0) {
        IM_ERRMSG((IM_STR("venc dmmu enable failed")));
        return -EFAULT;
    }
    writel(0x1, IO_ADDRESS(SYSMGR_VENC_BASE + 0x20));   // enable mmu
    venc_param.mmu_enabled = true;
    return 0;
}

static int venc_mmu_disable(void)
{
    if (imapx_dmmu_disable(venc_param.mmu_handle) != 0) {
        IM_ERRMSG((IM_STR("venc dmmu disable failed")));
        return -EFAULT;
    }
    writel(0x0, IO_ADDRESS(SYSMGR_VENC_BASE + 0x20));   // disable mmu
    venc_param.mmu_enabled = false;
    return 0;
}

static long venc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    venc_instance_t *inst = (venc_instance_t *)file->private_data;
    int l;
    int use_mmu;
    IM_INFOMSG((IM_STR("%s(cmd=0x%x)"), IM_STR(_IM_FUNC_), cmd));

    switch (cmd) {
        case HX280ENC_IOCGHWOFFSET:
            __put_user(venc_param.reg_base_phys_addr, (unsigned int *)arg);
            break;
        case HX280ENC_IOCGHWIOSIZE:
            __put_user(VENC_ACT_REG_SIZE, (unsigned int *)arg);
            break;
        case HX280ENC_IOC_RESERVE_HW:
        {
            IM_INFOMSG(("HX280ENC_IOC_RESERVE_HW, use_mmu: %lu\n", arg));
            l = down_interruptible(&venc_sema);
            inst->reservedHW = true;
            use_mmu = arg;
            if (use_mmu && !venc_param.mmu_enabled) {
                if (venc_mmu_enable())
                    return -EFAULT;
            } else if (!use_mmu && venc_param.mmu_enabled) {
                if (venc_mmu_disable())
                    return -EFAULT;
            }
#ifdef CONFIG_VENC_DVFS_SUPPORT
            if(gUtlzHdl != IM_NULL){
                utlzlib_notify(gUtlzHdl, UTLZ_NOTIFY_CODE_RUN);
            }
#endif	
            break;
        }
        case HX280ENC_IOC_RELEASE_HW:
            inst->reservedHW = false;
            up(&venc_sema);
#ifdef CONFIG_VENC_DVFS_SUPPORT
            if(gUtlzHdl != IM_NULL){
                utlzlib_notify(gUtlzHdl, UTLZ_NOTIFY_CODE_STOP);
            }
#endif	
            break;
#ifdef CONFIG_VENC_DVFS_SUPPORT
        case HX280ENC_IOC_DVFS:
            {
                int mode = (arg >> 28) & 0xf;
                if (mode == 0x0) { // H264
                    int ratio = (int)arg; // divide 1024 to get real ratio
                    long new_freq = 0;
                    int level = -1;
                    new_freq = (gDvfsClkTbl[gDvfsLevel].clkfreq >> 10) * ratio;
                    // printk("HX280ENC_IOC_DVFS, ratio %d old_freq %ld new_freq %ld\n", ratio, gDvfsClkTbl[gDvfsLevel].clkfreq, new_freq);
                    while (++level <= DVFS_LEVEL_MAX) {
                        if (gDvfsClkTbl[level].clkfreq > new_freq)
                            break;
                    }
                    if (level > DVFS_LEVEL_MAX) {
                        level = DVFS_LEVEL_MAX;
                        IM_TIPMSG((IM_STR("fslevel is already the max value")));
                    }
                    spin_lock(&dvfs_lock);
                    dvfs_set_level(level);
                    spin_unlock(&dvfs_lock);
                } else if (mode == 0x2) { // JPEG
                    // Jpeg clam
                    jpegDvfsClamp = true;
                    spin_lock(&dvfs_lock);
                    dvfs_set_level(DVFS_LEVEL_MAX);
                    spin_unlock(&dvfs_lock);
                } else if (mode == 0x3) {
                    // Clear Jpeg clam
                    jpegDvfsClamp = false;
                } else {
                    IM_ERRMSG((IM_STR("invalid mode %x"), mode));
                }
                break;
            }
#endif
        case HX280ENC_IOC_CHECK_MMU:
            __put_user(venc_param.mmu_inited, (unsigned int *)arg);
            break;
#undef EWL_DEBUG
#ifdef EWL_DEBUG
        case HX280ENC_IOC_DEBUG:
            {
                char str[128];
                int ret = copy_from_user(str, (unsigned char *)arg, 128);
                printk("\n######################################## %s ########################################\n", str);
                if (ret)
                    return -EFAULT;
                break;
            }
#endif
        default:
            IM_ERRMSG((IM_STR("unknown cmd 0x%x"), cmd));
            return -EFAULT;
    }

    return 0;
}

static int venc_mmap(struct file *file, struct vm_area_struct *vma)
{
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    if(unlikely(remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start, vma->vm_page_prot))){
        return -EAGAIN;
    }

    return 0;
}

static unsigned int venc_poll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

    poll_wait(file, &wait_encode, wait);

    if (encode_poll_mark != 0) {
        mask = POLLIN | POLLRDNORM;
        encode_poll_mark = 0;
    }

    return mask;
}

static struct platform_driver venc_plat = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "imap-venc",
    },
    .probe = venc_probe,
    .remove = venc_remove,
    .suspend = venc_suspend,
    .resume = venc_resume,
};

static int venc_probe(struct platform_device *pdev)
{
    int ret;
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

    venc_open_count = 0;
    memset(&venc_param, 0x00, sizeof(venc_param_t));
    init_waitqueue_head(&wait_encode);
    mutex_init(&venc_lock);
    spin_lock_init(&dvfs_lock);
    sema_init(&venc_sema, 1);
    venc_param.mmu_handle = NULL;
    venc_param.mmu_inited = false;
    venc_param.mmu_enabled = false;

    venc_param.reg_reserved_size = VENC_ACT_REG_SIZE;
    venc_param.reg_base_phys_addr = VENC_ACT_REG_BASE;
    venc_param.reg_base_virt_addr = ioremap_nocache(VENC_ACT_REG_BASE, VENC_ACT_REG_SIZE);
    if(venc_param.reg_base_virt_addr == NULL){
        IM_ERRMSG((IM_STR("ioremap_nocache() failed!")));
        return -EINVAL;
    }

    // request for irq.
    ret = request_irq(VENC_ACT_INTR_ID, venc_irq_handle, IRQF_DISABLED, pdev->name, (void *)(&venc_param));
    if (ret) {
        IM_ERRMSG((IM_STR("request_irq() failed!")));
        return ret;
    }

    // bus clock.
    venc_param.busClock = clk_get_sys("imap-venc", "imap-venc");
    if(venc_param.busClock == NULL){
        IM_ERRMSG((IM_STR("clk_get() failed!")));
        return -EFAULT;	
    }
    venc_param.clk_enable = 0;

    venc_param.dvfsEna = false;
#ifdef CONFIG_VENC_DVFS_SUPPORT
    if((item_exist(DVFS_ENABLE_ITEM) == 1 && item_exist(DVFS_VPU_ENABLE_ITEM) == 1) || item_exist(DVFS_VPU) == 1){
        if((item_integer(DVFS_ENABLE_ITEM, 0) == 1 && item_integer(DVFS_VPU_ENABLE_ITEM, 0) == 1) || item_integer(DVFS_VPU, 0) == 1){
            venc_param.dvfsEna = true;
            IM_TIPMSG((IM_STR("venc DVFS enabled")));
        }
    }
#endif

    venc_module_disable();

    return 0;
}

static int venc_remove(struct platform_device *pdev)
{
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

    iounmap((void *)(venc_param.reg_base_virt_addr));

    /* release irq */
    free_irq(VENC_ACT_INTR_ID, pdev);

    mutex_destroy(&venc_lock);

    return 0;
}

static int venc_suspend(struct platform_device *pdev, pm_message_t state)
{
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
    mutex_lock(&venc_lock);
    if(venc_open_count > 0){
        venc_module_disable();
    }
    mutex_unlock(&venc_lock);
    return 0;
}

static int venc_resume(struct platform_device *pdev)
{
    IM_INFOMSG((IM_STR("%s(venc_open_count=%d)"), IM_STR(_IM_FUNC_), venc_open_count));
    mutex_lock(&venc_lock);
    if(venc_open_count > 0){
        venc_module_enable();
    }
    mutex_unlock(&venc_lock);

    return 0;
}

void venc_module_enable(void)
{
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

    if(clk_prepare_enable(venc_param.busClock) != 0){
        IM_ERRMSG((IM_STR("clk_enable() failed")));
    }

    venc_param.clk_enable = 1;
    module_power_on(SYSMGR_VENC_BASE);
    venc_module_reset();

#ifdef CONFIG_VENC_DVFS_SUPPORT
    if(venc_param.dvfsEna){
        if(dvfs_init() != IM_RET_OK){
            IM_ERRMSG((IM_STR("dvfs_init() failed")));
        }
    }
    //dvfs_set_level(DVFS_LEVEL_MAX);
#endif
}

void venc_module_disable(void)
{
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

#ifdef CONFIG_VENC_DVFS_SUPPORT
    //dvfs_set_level(DVFS_LEVEL_MIN);
    if(venc_param.dvfsEna){
        utlz_arbitrate_clear(ARB_VENC);
        if(dvfs_deinit() != IM_RET_OK){
            IM_ERRMSG((IM_STR("dvfs_deinit() failed")));
        }
    }
#endif

    module_power_down(SYSMGR_VENC_BASE);
    if(venc_param.clk_enable) {
        clk_disable_unprepare(venc_param.busClock);
        venc_param.clk_enable = 0;
    }
}

static void ResetAsic(void)
{
    int i;

    writel(0, venc_param.reg_base_virt_addr + 0x38);   

    for(i = 4; i < venc_param.reg_reserved_size; i += 4)
    {
        writel(0, venc_param.reg_base_virt_addr + i);
    }
}

void venc_module_reset(void)
{
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
    ResetAsic();
    module_reset(SYSMGR_VENC_BASE, 1);	// 0--bus reset, 1--venc reset.
}

static int __init venc_init(void)
{
    int ret = 0;
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

    /* Register platform device driver. */
    ret = platform_driver_register(&venc_plat);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register venc platform device driver error, %d.\n", ret);
        return ret;
    }

    /* Register misc device driver. */
    ret = misc_register(&venc_miscdev);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register venc misc device driver error, %d.\n", ret);
        return ret;
    }

    return 0;
}


static void __exit venc_exit(void)
{
    IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
    misc_deregister(&venc_miscdev);
    platform_driver_unregister(&venc_plat);
}
module_init(venc_init);
module_exit(venc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rane of InfoTM");

