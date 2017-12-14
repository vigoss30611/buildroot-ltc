/*
 * vdec.c
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
 * IMAPX video dec driver process.
 *
 * Author:
 *	ayakashi<eric.xu@infotmic.com.cn>.
 *
 * Revision History:
 * 1.0.0   04/05/2012  Created for kernel3.0.8 by ayakashi.
 * 1.0.1   04/14/2012 Released by ayakashi .
 * 1.0.2   05/03/2012 This version support mmu
 * 1.0.3   06/05/2012 The latest stable version,up sema if it needed when release. 
 * 1.0.4   06/29/2012 workaround for mmu disable failed.
 * 1.0.5   07/03/2012 add mutex for mmu resource
 * 1.0.6   07/09/2012 add  devfs
 * 1.0.7   07/12/2012 add ioctl for rest mmu
 * 1.0.8   09/02/2012 optimize for dvfs
 * 1.0.9   10/31/2013 optimize dvfs by latency 
 * 1.1.0   11/07/2013 support imapx900 
 * 1.1.1   11/22/2013 add control interface for sysfs  
 * 2.0.0   12/25/2013 for new mmu api and  arrange code
 * 2.0.1   01/07/2014 remove unused code
 * 2.0.2   02/18/2014 optimize dvfs for x9
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
#include <linux/version.h>
#include <linux/platform_device.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/items.h>

#include <InfotmMedia.h>
#include <IM_utlzapi.h>
#include <utlz_lib.h>
#include "vdec_g1.h"
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
#include <linux/cpufreq.h>
#endif

#define INFO	0
#define ERR		1
#define TIP		1

#define DEBUG
#ifdef DEBUG
#define DBGMSG(on, msg, ...)	if(on)	printk(KERN_ALERT msg, ##__VA_ARGS__)
#else
#define DBGMSG(on, msg, ...)
#endif

#define VDEC_DEV_NAME "vdec_g1"

//#define MMU_SUPPORT
//#define PMM_USE
#ifdef MMU_SUPPORT
#ifdef PMM_USE
#include <IM_buffallocapi.h>
#include <IM_devmmuapi.h>
#include <pmm_lib.h>
#else
#include "imapx_dev_mmu.h"
#endif 
#endif
enum {
    USE_NONE  = 0x0,
    USE_DEC   = 0x1,
    USE_PP    = 0x2,
    USE_MMU   = 0x4,
};  //inst state

enum {
    DEC_INST,
    PP_INST,
}; //inst type
struct vdec_filp_private {
    int inst_type;
    int inst_state;
    int mmu_enable;
};

vdec_param_t vdec_param;			/* global variables group */
static unsigned int vdec_open_count;		/* record open syscall times */
static struct mutex vdec_lock;		/* mainly lock for decode instance count */

static void vdec_module_enable(void);
static void vdec_module_disable(void);
static void vdec_module_reset(void);
static void vdec_reset_asic(void);

#ifdef CONFIG_VDEC_POLL_MODE
static wait_queue_head_t wait_vdec;		/* a wait queue for poll system call */
static volatile unsigned int dec_poll_mark = 0;
static volatile unsigned int pp_poll_mark = 0;
#endif	/* CONFIG_IMAP_DECODE_POLL_MODE */

static int g_g1_clk = 150;
module_param(g_g1_clk, int, 0);

static int external_enable_power = 0;

#ifdef MMU_SUPPORT

#ifdef PMM_USE
typedef pmm_handle_t vdec_mmu_handle;
#else
typedef imapx_dmmu_handle_t vdec_mmu_handle_t;
#endif
typedef struct mmu_params {
    int                 inited;
    int                 state;
    int                 working;
    int                 wait;
    vdec_mmu_handle_t   mmu_handle;
}mmu_params_t;

static mmu_params_t mmu_params;
#ifdef PMM_USE
extern IM_RET pmmdrv_open(OUT pmm_handle_t *phandle, IN char *owner);
extern IM_RET pmmdrv_release(IN pmm_handle_t handle);
extern IM_RET pmmdrv_dmmu_init(IN pmm_handle_t handle, IN IM_UINT32 devid);
extern IM_RET pmmdrv_dmmu_deinit(IN pmm_handle_t handle);
extern IM_RET pmmdrv_dmmu_reset(IN pmm_handle_t handle);
extern IM_RET pmmdrv_dmmu_enable(IN pmm_handle_t handle);
extern IM_RET pmmdrv_dmmu_disable(IN pmm_handle_t handle);
#else
#endif

static int vdec_mmu_init(vdec_mmu_handle_t *phandle);
static int vdec_mmu_deinit(vdec_mmu_handle_t phandle);
static int vdec_mmu_enable(vdec_mmu_handle_t phandle);
static int vdec_mmu_disable(vdec_mmu_handle_t phandle);
static int vdec_mmu_reset(vdec_mmu_handle_t phandle);
static int get_mmu_resource(int mmu_enable);
static void release_mmu_resource(void);


#endif //MMU_SUPPORT

// DVFS
#ifdef CONFIG_VDEC_DVFS_SUPPORT
extern utlz_handle_t utlzlib_register_module(utlz_module_t *um);
extern IM_RET utlzlib_unregister_module(utlz_handle_t hdl);
extern IM_RET utlzlib_notify(utlz_handle_t hdl, IM_INT32 code);

#define DVFS_FIXED  1
#define DVFS_AUTO   0
#define SET_MIN     0
#define SET_MAX     1
#define SET_FIXED   2
typedef struct vpu_dvfs_params
{
    spinlock_t  dvfslock;
    unsigned long flags;
    utlz_handle_t utlz_handle;
    int         enable;
    int         mode; //FIXED OR AUTO

    int         freq_min;
    int         freq_max;
    int         level_min;
    int         level_max;
    int         current_level;
    int         request;
    int         request_level;

    int         total_latency;
    int         latency_count;
}vpu_dvfs_params_t;

typedef struct{
    int	coeffi;		// N/256
    int	up_wnd;
    int	down_wnd;

    long	clkfreq;	// Khz
}dvfs_clock_table_t;

#ifdef SOC_IMAPX900
static dvfs_clock_table_t gDvfsClkTbl[] = 
{
    //	{40, 10, 10, 50000},
    {20, 20, 20, 33000000},
    {65, 15, 15, 54000000},
    {95, 15, 15, 66000000},
    {120, 10, 10, 102400000},
    {140, 10, 10, 128000000},
    {160, 10, 10, 153600000},
    {180, 10, 10, 171000000},
    {200, 10, 10, 192000000},
    {215, 10, 5, 220000000},
    {230, 10, 5, 256000000},
    {250, 6, 10, 297000000},
};
#else
static dvfs_clock_table_t gDvfsClkTbl[] = 
{
    //	{40, 10, 10, 50000},
    {20, 20, 20, 33000},
    {65, 15, 15, 54000},
    {95, 15, 15, 66000},
    {120, 10, 10, 84000},
    {140, 10, 10, 118800},
    {160, 10, 10, 148500},
    {180, 10, 10, 169700},
    {200, 10, 10, 198000},
    {220, 10, 10, 237600},
    {240, 16, 10, 297000},
};
#endif 

#define DVFS_LEVEL_MAX	(sizeof(gDvfsClkTbl) / sizeof(gDvfsClkTbl[0]) - 1)
#define DVFS_LEVEL_MIN	(0)
static vpu_dvfs_params_t vpu_dvfs;
static int gVdecDvfsCount;
extern void imapx800_freq_message(int module, int freq);



inline static IM_RET dvfs_set_level(int lock, IM_INT32 lx)
{
    unsigned long real_f;
    long rate;
    int level;
    DBGMSG(INFO,"%s(lx=%d)\n", _IM_FUNC_, lx);
    if(lock) 
	    spin_unlock_irqrestore(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
    //arbitrate frequency
    real_f = utlz_arbitrate_request(gDvfsClkTbl[lx].clkfreq, ARB_VDEC);
   
    if(real_f > gDvfsClkTbl[lx].clkfreq) {
        //update level;
        for (level = (int)DVFS_LEVEL_MIN; level <= (int)DVFS_LEVEL_MAX; level++)
            if(real_f<=gDvfsClkTbl[level].clkfreq)
                break;
        lx = level;
    } else {
        rate = clk_set_rate(vdec_param.bus_clock, gDvfsClkTbl[lx].clkfreq);
        if(rate < 0){
            DBGMSG(ERR, "clk_set_rate(%ldkHz) failed\n", gDvfsClkTbl[lx].clkfreq);
            if(lock)
            spin_lock_irqsave(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
            return IM_RET_FAILED;
        }
    }

    // record the actual clock for this level.
    gDvfsClkTbl[lx].clkfreq = clk_get_rate(vdec_param.bus_clock);
    vpu_dvfs.current_level = lx;
    if(lock)
        spin_lock_irqsave(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
    //printk("vpufreq-->%ld kHz \n", rate);
    imapx800_freq_message(2, rate/1000000);

    return IM_RET_OK;
}

int imapx800_vpufreq_getval(void)
{
    int hz;
    spin_lock_irqsave(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
    if(vpu_dvfs.mode == DVFS_FIXED)
    {    
        if(vdec_open_count > 0 && vpu_dvfs.request_level >= 0)
            hz = gDvfsClkTbl[vpu_dvfs.request_level].clkfreq / 1000;
        else 
            hz = 0; 
    }    
    else 
    {    
        if(vdec_open_count > 0 && vpu_dvfs.current_level >= 0)
            hz = gDvfsClkTbl[vpu_dvfs.current_level].clkfreq / 1000;
        else 
            hz = 0; 
    }    
    spin_unlock_irqrestore(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
    return hz;
}
EXPORT_SYMBOL(imapx800_vpufreq_getval);

void imapx_vpufreq_setlevel(int mode, int level)
{
    //printk("imapx800_vpufreq_setlvl %d \n", level);
    spin_lock_irqsave(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
    if(level < 0)
    {
        if(vpu_dvfs.enable) {
            vpu_dvfs.mode  = DVFS_AUTO;
            vpu_dvfs.level_min = DVFS_LEVEL_MIN;
            vpu_dvfs.level_max = DVFS_LEVEL_MAX;
        } else {
            vpu_dvfs.request_level = -1;
            dvfs_set_level(1, DVFS_LEVEL_MAX - 1);
        }
    }
    else
    {
        level = level > DVFS_LEVEL_MAX ? DVFS_LEVEL_MAX : level;

        if(mode == SET_MIN){
            printk("set min level %d \n", level);
            if(level > vpu_dvfs.level_max) {
                printk("the request min level is bigger than current level max, set level max as min \n");
                level = vpu_dvfs.level_max;
            }
            vpu_dvfs.level_min = level;
        }else if (mode == SET_MAX) {
            printk("set max level %d \n", level);
            if(level < vpu_dvfs.level_min) {
                printk("the request max level is smaller than current level min, set level min as max \n");
                level = vpu_dvfs.level_min;
            }
            vpu_dvfs.level_max = level;
        } else {
            vpu_dvfs.mode = DVFS_FIXED;
            vpu_dvfs.request_level = level;
            if(vpu_dvfs.current_level != level) {
                dvfs_set_level(1, level);
            }
            imapx800_freq_message(2, gDvfsClkTbl[vpu_dvfs.request_level].clkfreq / 1000);
        }
    }
    spin_unlock_irqrestore(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
    return;
}

void imapx800_vpufreq_setlvl(int level)
{
    imapx_vpufreq_setlevel(SET_FIXED, level);
}
EXPORT_SYMBOL(imapx800_vpufreq_setlvl);

static void vdec_utlz_listener(int coeffi)
{
    int level;
    DBGMSG(INFO,"%s(coeffi=%d)\n", _IM_FUNC_, coeffi);
    spin_lock_irqsave(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
    if(vpu_dvfs.mode == DVFS_AUTO)
    {
        vpu_dvfs.request = 1;
        if(coeffi == 0)
        {
            if(vpu_dvfs.current_level != vpu_dvfs.level_min)
            {
                vpu_dvfs.request_level = vpu_dvfs.level_max;
                dvfs_set_level(1, vpu_dvfs.level_min);
            }
        }else if(coeffi > gDvfsClkTbl[vpu_dvfs.current_level].coeffi + gDvfsClkTbl[vpu_dvfs.current_level].up_wnd)
        {
            level = vpu_dvfs.current_level + 1;
            while(level < vpu_dvfs.level_max)
            {	// fast up.
                if(gDvfsClkTbl[level].coeffi + gDvfsClkTbl[level].up_wnd >= coeffi)
                {
                    break;
                }
                level++;
            }	

            if(level > vpu_dvfs.level_max)
            {
                level = vpu_dvfs.level_max;
            }
            if(level > vpu_dvfs.current_level)
            {
                vpu_dvfs.request_level = level;
            }
        }
        else if((coeffi < gDvfsClkTbl[vpu_dvfs.current_level].coeffi - gDvfsClkTbl[vpu_dvfs.current_level].down_wnd) &&  vpu_dvfs.current_level > vpu_dvfs.level_min)
        {
            vpu_dvfs.request_level = vpu_dvfs.current_level - 1;
        }
    }
    spin_unlock_irqrestore(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
}

static utlz_module_t gUtlzModule = {
    .name = VDEC_DEV_NAME,
    .period = 1,
    .callback = vdec_utlz_listener
};

static IM_RET dvfs_init(void)
{
    vpu_dvfs.utlz_handle = utlzlib_register_module(&gUtlzModule);
    if(vpu_dvfs.utlz_handle == IM_NULL){
        DBGMSG(ERR, "utlzlib_register_module() failed\n");
        return IM_RET_FAILED;
    }
    if(vpu_dvfs.mode == DVFS_AUTO) {
        vpu_dvfs.request_level = vpu_dvfs.level_max;
        dvfs_set_level(0, vpu_dvfs.level_max); 
    } else {
        if(vpu_dvfs.request_level < 0)
            vpu_dvfs.request_level = -1/*vpu_dvfs.level_max*/;
        if(vpu_dvfs.current_level < 0) {
            vpu_dvfs.current_level = -1/*vpu_dvfs.level_min*/;
        }
    }
    return IM_RET_OK;
}

static IM_RET dvfs_deinit(void)
{
    if(vpu_dvfs.utlz_handle != IM_NULL){
        utlzlib_unregister_module(vpu_dvfs.utlz_handle);
        vpu_dvfs.utlz_handle = IM_NULL;
    }

    if(vpu_dvfs.mode == DVFS_AUTO) {
        vpu_dvfs.request_level= vpu_dvfs.level_min;
        dvfs_set_level(0, vpu_dvfs.level_min); 
    }

    utlz_arbitrate_clear(ARB_VDEC);
    return IM_RET_OK;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
static ssize_t show_vpu_freq_min(struct kobject *kobj,
				      struct attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", (unsigned int )gDvfsClkTbl[vpu_dvfs.level_min].clkfreq); //kHz
}

static ssize_t show_vpu_freq_max(struct kobject *kobj,
				      struct attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", (unsigned int)gDvfsClkTbl[vpu_dvfs.level_max].clkfreq); //kHz
}

static ssize_t show_vpu_freq_fix(struct kobject *kobj,
				      struct attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", (unsigned int)gDvfsClkTbl[vpu_dvfs.current_level].clkfreq); //kHz
}

static ssize_t store_vpu_freq_min(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	int level;

	ret = sscanf(buf, "%u", &input); //kHz
	if (ret != 1)
		return -EINVAL;

    printk("extern request min freq = %u kHz \n", input);
	for (level = (int)DVFS_LEVEL_MIN; level <= (int)DVFS_LEVEL_MAX; level++)
		if(input<=gDvfsClkTbl[level].clkfreq)
			break;

    
	imapx_vpufreq_setlevel(SET_MIN, level);

	return count;
}

static ssize_t store_vpu_freq_max(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	int level;
    

	ret = sscanf(buf, "%u", &input); //kHz
	if (ret != 1)
		return -EINVAL;

    printk("extern request max freq = %u kHz \n", input);
	for (level= DVFS_LEVEL_MAX; level>=0; level--)
		if(input>=gDvfsClkTbl[level].clkfreq)
			break;
    if(level < 0)
        level = 0;
	imapx_vpufreq_setlevel(SET_MAX, level);

	return count;
}

static ssize_t store_vpu_freq_fix(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	unsigned int input;
    int level;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

    printk("extern request fixed freq = %d kHz \n", input);
	if (input > 0) {
	    for (level= DVFS_LEVEL_MAX; level>=0; level--)
		    if(input>=gDvfsClkTbl[level].clkfreq)
			    break;
        if(level < 0)
            level = 0;
	    imapx_vpufreq_setlevel(SET_FIXED, level);
	} else {
        printk("extern exit fixed mode \n");
	    imapx_vpufreq_setlevel(SET_FIXED, -1);
	}

	return count;
}

define_one_global_rw(vpu_freq_fix); // kHz
define_one_global_rw(vpu_freq_max); // kHz
define_one_global_rw(vpu_freq_min); // kHz

static struct attribute *vpu_dvfs_attributes[] = {
	&vpu_freq_fix.attr,
	&vpu_freq_max.attr,
	&vpu_freq_min.attr,
	NULL
};

static struct attribute_group vpu_dvfs_attr_group = {
	.attrs = vpu_dvfs_attributes,
	.name  = "vpu_dvfs",
};
#endif
#endif //CONFIG_VDEC_DVFS_SUPPORT

/***********************************************************************************/




/* 
 * This irq handle function should never be reexecute at the same
 * time. Pp and decode share the same interrupt signal thread, in 
 * pp external mode, a finished interruption refer to be a pp irq, 
 * but in pp pipeline mode(decode pipeline with decode), recieved 
 * irq refers to be a decode interruption.
 * In this driver, we use System V signal asynchronization thread
 * communication. So you should set System V in you kernel make 
 * menuconfig.
 * ATTENTION: if your application runs in multi-thread mode, be 
 * aware that if you set your application getting this signal
 * as process mode, the signal will be send to your application's
 * main thread, and signal will never be processed twice. So in 
 * this case you won't get anything in your derived thread.
 */
static irqreturn_t vdec_irq_handle(int irq, void *dev_id)
{
    unsigned int handled;
    unsigned int irq_status_dec;
    unsigned int irq_status_pp;
    vdec_param_t *dev;

    handled = 0;
    dev = (vdec_param_t *)dev_id;

    /* interrupt status register read */
    irq_status_dec = readl(dev->reg_base_virt_addr + VDEC_IRQ_STAT_DEC * 4);
    irq_status_pp = readl(dev->reg_base_virt_addr + VDEC_IRQ_STAT_PP * 4);

    /* this structure is to enable irq in irq */
    if((irq_status_dec & VDEC_IRQ_BIT_DEC) || (irq_status_pp & VDEC_IRQ_BIT_PP))
    {
        if(irq_status_dec & VDEC_IRQ_BIT_DEC)
        {
#ifdef CONFIG_VDEC_HW_PERFORMANCE
            do_gettimeofday(&(dev->end_time));
#endif
            /* clear irq */
            writel(irq_status_dec & (~VDEC_IRQ_BIT_DEC), \
                    dev->reg_base_virt_addr + VDEC_IRQ_STAT_DEC * 4);

#ifdef CONFIG_VDEC_SIGNAL_MODE
            /* fasync kill for decode instances to send signal */
            if(dev->async_queue_dec != NULL)
                kill_fasync(&(dev->async_queue_dec), SIGIO, POLL_IN);
            else
                DBGMSG(ERR,"IMAPX vdec received w/o anybody waiting for it\n");
#endif	/* CONFIG_VDEC_SIGNAL_MODE */

#ifdef CONFIG_VDEC_POLL_MODE
            dec_poll_mark = 1;
            wake_up(&wait_vdec);
#endif	/* CONFIG_VDEC_POLL_MODE */

            DBGMSG(INFO,"IMAPX vdec get dec irq\n");
        }

        /* In pp pipeline mode, only one decode interrupt will be set */
        if(irq_status_pp & VDEC_IRQ_BIT_PP)
        {
#ifdef CONFIG_VDEC_HW_PERFORMANCE
            do_gettimeofday(&(dev->end_time));
#endif
            /* clear irq */
            writel(irq_status_dec & (~VDEC_IRQ_BIT_PP), \
                    dev->reg_base_virt_addr + VDEC_IRQ_STAT_PP * 4);

#ifdef CONFIG_VDEC_SIGNAL_MODE
            /* fasync kill for pp instance */
            if(dev->async_queue_pp != NULL)
                kill_fasync(&dev->async_queue_pp, SIGIO, POLL_IN);
            else
                DBGMSG(ERR,"IMAPX vdec pp received w/o anybody waiting for it\n");
#endif	/* CONFIG_VDEC_SIGNAL_MODE */

#ifdef CONFIG_VDEC_POLL_MODE
            pp_poll_mark = 1;
            wake_up(&wait_vdec);
#endif	/* CONFIG_VDEC_POLL_MODE */

            //DBGMSG(INFO,"IMAPX vdec get pp irq\n");
        }

        handled = 1;
    }
    else 
    {
        DBGMSG(ERR,"IMAPX vdec Driver get an unknown irq\n");
        handled = 0;
    }

    return IRQ_RETVAL(handled);
}

#ifdef CONFIG_IMAP_DECODE_SIGNAL_MODE
/*
 * File operations, this system call should be called before
 * any interrupt occurs. You can call fcntl system call to set 
 * driver node file property in order to get asynchronization
 * signal.
 */
static int vdec_fasync(int fd, struct file *file, int mode)
{
    vdec_param_t *dev;
    struct fasync_struct **async_queue;
    struct vdec_filp_private *fpriv = NULL;

    dev = &vdec_param;
    async_queue = NULL;

    /* select whitch interrupt this instance will sign up for */
    if((unsigned int *)(file->private_data) == &(vdec_param.dec_instance))
    {
        DBGMSG(INFO,"IMAPX vdec fasync called\n");
        async_queue = &(vdec_param.async_queue_dec);
    }
    else if((unsigned int *)(file->private_data) == &(vdec_param.pp_instance))
    {
        DBGMSG(INFO,"IMAPX vdec pp fasync called\n");
        async_queue = &(vdec_param.async_queue_pp);
    }
    else
        DBGMSG(ERR,"IMAPX vdec wrong fasync called\n");

    return fasync_helper(fd, file, mode, async_queue);
}
#endif	/* CONFIG_VDEC_SIGNAL_MODE */

#define   REG_SWITCH_BETWEEN_G1_G2          0x21E30024
#define   REG_SWITCH_BETWEEN_CPU_OTHER      0x21E0C820
#define   REG_EFUSE_GRIGGER                 0x21E0D000

/*
 * Tutorial:
 * Each open() system call returns a file descriptor if success. And as a
 * user space application, open() is a must step to access further kernel
 * module interfaces.
 */
static int vdec_open(struct inode *inode, struct file *file)
{
    struct vdec_filp_private *fpriv = NULL;
    DBGMSG(INFO, "%s(vdec_open_count=%d)\n", __func__, vdec_open_count);
    
    fpriv = kmalloc(sizeof(*fpriv), GFP_KERNEL);
	if (!fpriv)
		return -ENOMEM;

    fpriv->inst_type = DEC_INST;
    fpriv->inst_state = USE_NONE; //
    file->private_data = fpriv;
    mutex_lock(&vdec_lock);
    
    if(vdec_open_count == 0)
    {
        vdec_module_enable();
#ifdef MMU_SUPPORT
        if(vdec_mmu_init(&(mmu_params.mmu_handle)) == 0)
        {
            mmu_params.inited = 1;
            mmu_params.state = VDEC_MMU_OFF;
            mmu_params.wait = 0;
            mmu_params.working = 0;
        }
#endif 
    }
    vdec_open_count++;
    mutex_unlock(&vdec_lock);

    writel(0x1, vdec_param.g1g2_switch_virt_addr);	// g1g2 switch ,g1 enable
    DBGMSG(INFO,"IMAPX vdec open OK\n");

    return VDEC_RET_OK;
}

/*
 * Tutorial:
 * After open() a driver node, the file descriptor must be closed if you
 * don't use it anymore.
 */
static int vdec_release(struct inode *inode, struct file *file)
{
    struct vdec_filp_private *fpriv = (struct vdec_filp_private *)file->private_data;
    DBGMSG(INFO, "%s(vdec_open_count=%d)\n", __func__, vdec_open_count);
    

#ifdef CONFIG_VDEC_SIGNAL_MODE
    /* reset decode driver node file property */
    if(file->f_flags & FASYNC)
        vdec_fasync(-1, file, 0);
#endif	/* CONFIG_VDEC_SIGNAL_MODE */

    mutex_lock(&vdec_lock);
    if(fpriv->inst_state & USE_DEC)
    {
        DBGMSG(ERR, "application exception, vdec drv release dec source\n");
        up(&vdec_param.dec_sema);
    }

    if(fpriv->inst_state & USE_PP) 
    {
        DBGMSG(ERR, "application exception, vdec drv release pp source\n");
        up(&vdec_param.pp_sema);
    }

    if(fpriv->inst_state & USE_MMU) 
    {
        DBGMSG(ERR, "application data abort, vdec drv release mmu source\n");
        up(&vdec_param.mmu_sema);
    }
    vdec_open_count--;

    if(vdec_open_count == 0)
    {
#ifdef MMU_SUPPORT
        if(mmu_params.state == VDEC_MMU_ON)
        {
            vdec_mmu_disable(mmu_params.mmu_handle);
            mmu_params.state = 	VDEC_MMU_OFF;
        }
        if(vdec_mmu_deinit(mmu_params.mmu_handle) == 0)
        {
            mmu_params.inited = 0;
            mmu_params.mmu_handle = NULL;
        }
#endif 
        vdec_module_disable();
    }
    mutex_unlock(&vdec_lock);

    if(file->private_data) {
        kfree(file->private_data);
    }

    DBGMSG(INFO,"IMAPX vdec release OK\n");
    return VDEC_RET_OK;
}

/*
 * Tutorial:
 * Normally, the *read* and *write* apis are not widely used because
 * most driver use *mmap* to map a kernel memory region to user space
 * in order to avoid memory copy. 
 * And something special with read & write is the return value is the
 * actual accessed data length.
 */
static ssize_t vdec_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    DBGMSG(INFO, "%s()\n", __func__);
    return VDEC_RET_OK;
}

static ssize_t vdec_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    DBGMSG(INFO, "%s()\n", __func__);
    return VDEC_RET_OK;
}

/*
 * Tutorial:
 * IO control is the most widely used system call to driver because it
 * can almost implement all functionalities provided by others.
 */
#ifdef HAVE_UNLOCKED_IOCTL
static long vdec_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
#else
static int vdec_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg) 
#endif
{
    //DBGMSG(INFO, "%s() cmd:%d\n", __func__, cmd);
    int ret = 0;
    int latency = 0;
    struct vdec_filp_private *fpriv = (struct vdec_filp_private *)file->private_data;

#ifdef CONFIG_VDEC_HW_PERFORMANCE
    int out;	/* for copy from/to user */
    struct timeval end_time_arg;
    out = -1;
#endif
    //DBGMSG(INFO, "%s()\n", __func__);
    
    /* cmd check */
    if(_IOC_TYPE(cmd) != VDEC_MAGIC)
        return -ENOTTY;
    if(_IOC_NR(cmd) > VDEC_MAX_CMD)
        return -ENOTTY;

    /* check command by command feature READ/WRITE */
    if(_IOC_DIR(cmd) & _IOC_READ)
        ret = !access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd));
    else if(_IOC_DIR(cmd) & _IOC_WRITE)
        ret = !access_ok(VERIFY_READ, (void *) arg, _IOC_SIZE(cmd));
    if(ret)
        return -EFAULT;

    switch(cmd)
    {
        case VDEC_IRQ_DISABLE:
            disable_irq(IRQ_VDEC);
            break;

        case VDEC_IRQ_ENABLE:
            enable_irq(IRQ_VDEC);
            break;

            /* we should return a physics address to application level */
        case VDEC_REG_BASE:
            __put_user(vdec_param.reg_base_phys_addr, (unsigned long *)arg);
            break;

            /* this is 101 * 4 by default */
        case VDEC_REG_SIZE:
            __put_user(VDEC_ACT_REG_SIZE, (unsigned int *)arg);
            break;

        case NV21_REG_BASE:
            __put_user(vdec_param.nv21_reg_base, (unsigned long *)arg);
            break;

        case VDEC_PP_INSTANCE:
            fpriv->inst_type = PP_INST;
            break;
#ifdef CONFIG_VDEC_HW_PERFORMANCE	/* this ioctl command call is for hardware decode performance detect */
        case VDEC_HW_PERFORMANCE:
            end_time_arg.tv_sec	= vdec_param.end_time.tv_sec;
            end_time_arg.tv_usec	= vdec_param.end_time.tv_usec;

            out = copy_to_user((struct timeval *)arg, &end_time_arg, sizeof(struct timeval));
            break;
#endif
        case VDEC_INSTANCE_LOCK:
            if(fpriv->inst_type == DEC_INST)
            {
                if(down_interruptible(&vdec_param.dec_sema))
                {
                    return -1;
                }
                mutex_lock(&vdec_lock);
                fpriv->inst_state |= USE_DEC;
                mutex_unlock(&vdec_lock);
            }
            if(fpriv->inst_type == PP_INST)
            {
                if(down_interruptible(&vdec_param.pp_sema))
                {
                    return -1;
                }
                mutex_lock(&vdec_lock);
                fpriv->inst_state |= USE_PP;
                mutex_unlock(&vdec_lock);
            }
#ifdef MMU_SUPPORT
            get_mmu_resource(fpriv->mmu_enable);
#endif
            break;
        case VDEC_INSTANCE_UNLOCK:
            mutex_lock(&vdec_lock);
#ifdef MMU_SUPPORT 
            release_mmu_resource();
#endif
            if(fpriv->inst_type == DEC_INST)
            {
                fpriv->inst_state &= ~USE_DEC;
                up(&vdec_param.dec_sema);
            }
            if(fpriv->inst_type == PP_INST)
            {
                fpriv->inst_state &= ~USE_PP;
                up(&vdec_param.pp_sema);
            }
            mutex_unlock(&vdec_lock);

            break;
        case VDEC_MMU_SET:
            __get_user(fpriv->mmu_enable, (int *)arg);
            break;
        case VDEC_RESET:
            vdec_module_reset();
#ifdef MMU_SUPPORT
            vdec_mmu_reset(mmu_params.mmu_handle);
#endif
	    mutex_lock(&vdec_lock);
#ifdef MMU_SUPPORT
	    if(mmu_params.state == VDEC_MMU_ON)
            {
                vdec_mmu_enable(mmu_params.mmu_handle);
            }
#endif
            mutex_unlock(&vdec_lock);
            break;
#ifdef CONFIG_VDEC_DVFS_SUPPORT
        case VDEC_NOTIFY_HW_ENABLE:
            if(vpu_dvfs.utlz_handle != IM_NULL)
            {
                __get_user(latency, (int *)arg);
                spin_lock_irqsave(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
                if(vdec_open_count == 1) {
                    vpu_dvfs.request_level = vpu_dvfs.level_max;
                }
                
                if(fpriv->inst_type == DEC_INST) {
                    //printk("current latencyMS = %d   \n", latency);
                    vpu_dvfs.total_latency +=  latency;
                    vpu_dvfs.latency_count++ ;
                }
                if(vpu_dvfs.request > 0 && vpu_dvfs.mode == DVFS_AUTO) {
                    vpu_dvfs.request = 0;
                    //compensation with latency
                    if(vpu_dvfs.latency_count > 0) {
                        vpu_dvfs.total_latency = vpu_dvfs.total_latency/vpu_dvfs.latency_count; 
                    }

                    vpu_dvfs.total_latency += 30;
                    vpu_dvfs.request_level += vpu_dvfs.total_latency > 0 ? vpu_dvfs.total_latency / 30 : vpu_dvfs.total_latency / 60;
                    //printk("request level = %d, vpu_dvfs.average_latency = %d   \n", vpu_dvfs.request_level, vpu_dvfs.total_latency);
                 
                    vpu_dvfs.total_latency = 0;
                    vpu_dvfs.latency_count = 0;
                }
                if(vpu_dvfs.request_level > vpu_dvfs.level_max) {
                    vpu_dvfs.request_level = vpu_dvfs.level_max;
                }

                if(vpu_dvfs.request_level < vpu_dvfs.level_min) {
                    vpu_dvfs.request_level = vpu_dvfs.level_min;
                }

                if(vpu_dvfs.request_level != vpu_dvfs.current_level) {
                    dvfs_set_level(1, vpu_dvfs.request_level);
                    //gDvfsRequestLevel = -1;
                }
                spin_unlock_irqrestore(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
                mutex_lock(&vdec_lock);
                gVdecDvfsCount++;
                if(gVdecDvfsCount == 1) {
                    mutex_unlock(&vdec_lock);
                    utlzlib_notify(vpu_dvfs.utlz_handle, UTLZ_NOTIFY_CODE_RUN);
                } else {
                    mutex_unlock(&vdec_lock);
                }
            }
            break;
        case VDEC_NOTIFY_HW_DISABLE:
            if(vpu_dvfs.utlz_handle != IM_NULL){
                mutex_lock(&vdec_lock);
                gVdecDvfsCount--;
                if(gVdecDvfsCount == 0) {
                    mutex_unlock(&vdec_lock);
                    utlzlib_notify(vpu_dvfs.utlz_handle, UTLZ_NOTIFY_CODE_STOP);
                } else {
                    mutex_unlock(&vdec_lock);
                }
            }
            break;

        case VDEC_UPDATE_CLK:
            if(vpu_dvfs.utlz_handle != IM_NULL){
                spin_lock_irqsave(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
                if(vpu_dvfs.mode == DVFS_AUTO) {
                    vpu_dvfs.request_level = vpu_dvfs.level_max;
                }
                spin_unlock_irqrestore(&vpu_dvfs.dvfslock, vpu_dvfs.flags);
            }
            break;
        case VDEC_GETDVFS:
            __put_user(vpu_dvfs.enable, (unsigned int *)arg);
            break;
#endif
        default:
            DBGMSG(ERR,"IMAPX vdec unknown ioctl command\n");
            return -EFAULT;
    }

    return VDEC_RET_OK;
}

/*
 * Tutorial:
 * Map private memory to user space, you can define the operation of 
 * as you wish.
 */
static int vdec_mmap(struct file *file, struct vm_area_struct *vma)
{
    DBGMSG(INFO, "%s() \n", __func__ );
    size_t size = vma->vm_end - vma->vm_start;

    DBGMSG(INFO, "%s(), size= %u, pgoff = %lu\n", __func__, size, vma->vm_pgoff);
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    //return remap_pfn_range(vma, vdec_param.reg_base_phys_addr,vma->vm_pgoff, size, vma->vm_page_prot);
    return remap_pfn_range(vma, vma->vm_start,vma->vm_pgoff, size, vma->vm_page_prot);
}

/*
 * Tutorial:
 * Poll is most used for user space application to wait for a kernel
 * driver event, like interrupt or harware mission completion.
 */

#ifdef CONFIG_VDEC_POLL_MODE
static unsigned int vdec_poll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;
    struct vdec_filp_private *fpriv = (struct vdec_filp_private *)file->private_data;
    //	DBGMSG(INFO, "%s()\n", __func__);

    poll_wait(file, &wait_vdec, wait);

    if(fpriv->inst_type == DEC_INST)
    {
        if(dec_poll_mark != 0)
        {
            DBGMSG(INFO, "dec irq coming\n");
            mask = POLLIN | POLLRDNORM;
            dec_poll_mark = 0;
        }
    }
    else if(fpriv->inst_type == PP_INST)
    {
        if(pp_poll_mark != 0)
        {
            //DBGMSG(INFO, "pp irq coming\n");
            mask = POLLIN | POLLRDNORM;
            pp_poll_mark = 0;
        }
    }
    else
    {
        dec_poll_mark = 0;
        pp_poll_mark = 0;
        DBGMSG(INFO, "wait nothing\n");
        mask = POLLERR;
    }

    return mask;
}
#endif

static void vdec_reset_asic()
{
    DBGMSG(INFO, "%s() \n", __func__ );
    int i;

    writel(0, vdec_param.reg_base_virt_addr + 0x04);

    for(i = 4; i < vdec_param.reg_reserved_size; i += 4)
    {
        writel(0x0, vdec_param.reg_base_virt_addr + i);
    }
    return;
}
#include <mach/clock.h>
static void  g1_set_clk()
{
    struct clk * clk_bus = clk_get_sys("imap-vdec264-ctrl","imap-vdec264-ctrl");
    if (IS_ERR(clk_bus)) {
	    pr_err("imap-vdec264-ctrl clock not found!\n");
	    return -1;
    }
    int err = clk_prepare_enable(clk_bus);
    if (err) {
	    pr_err("imap-vdec264-ctrl clock prepare enable fail!\n");
	    clk_disable_unprepare(clk_bus);
	    clk_put(clk_bus);
	    return err;
    }
    struct clk * clk = clk_get_sys("imap-vdec264", "imap-vdec264");
    if (IS_ERR(clk)) {
	    pr_err("imap-vdec264 clock not found!\n");
	    return -1;
    }
    int err = clk_prepare_enable(clk);
    if (err) {
	    pr_err("imap-vdec264 clock prepare enable fail!\n");
	    clk_disable_unprepare(clk);
	    clk_put(clk);
	    return err;
    }
    int clkFre  = clk_get_rate(clk);
    printk("g1 clock frequency:%d\n", clkFre); 

    clk_set_rate(clk, g_g1_clk * 1000000);
    //clk_prepare_enable(clk);

    clkFre  = clk_get_rate(clk);
    printk("new g1 clock frequency:%d\n", clkFre);
}

static int vdec_probe(struct platform_device *pdev)
{
    int		ret;

    //struct resource	res;
    DBGMSG(INFO, "%s()\n", __func__);
    /* initualize open count */
    vdec_open_count = 0;

#ifdef CONFIG_VDEC_POLL_MODE
    /* initualize wait queue */
    init_waitqueue_head(&wait_vdec);
#endif	/* CONFIG_IMAP_DECODE_POLL_MODE */

#ifdef CONFIG_IMAP_DECODE_SIGNAL_MODE
    /* initualize async queue */
    vdec_param.async_queue_dec	= NULL;
    vdec_param.async_queue_pp	= NULL;
#endif	/* CONFIG_IMAP_DECODE_SIGNAL_MODE */

    /* request memory region for registers */
    vdec_param.reg_reserved_size = VDEC_ACT_REG_SIZE;
    vdec_param.reg_base_phys_addr = VDEC_BASE_REG_PA;

    vdec_param.nv21_reg_size = NV21_REG_SIZE;
    vdec_param.nv21_reg_base = VDEC_BASE_REG_PA + NV21_OFFSET;
    if(!request_mem_region
            (vdec_param.reg_base_phys_addr, vdec_param.reg_reserved_size, "vdec_drv_g1"))
    {   
        printk(KERN_ALERT "vdec_drv_g1: failed to reserve HW regs\n");
        return -EBUSY;
    }   

    vdec_param.reg_base_virt_addr =
        (volatile u8 *) ioremap_nocache(vdec_param.reg_base_phys_addr,
                vdec_param.reg_reserved_size);

    if(vdec_param.reg_base_virt_addr == NULL)
    {   
        printk(KERN_ALERT  "vdec_drv_g1: failed to ioremap HW regs\n");
        return -EBUSY;
    }   

    if(!request_mem_region
            (vdec_param.nv21_reg_base, vdec_param.nv21_reg_size, "vdec_drv_g1"))
    {   
        printk(KERN_ALERT "vdec_drv_g1: failed to reserve HW regs\n");
        return -EBUSY;
    }   

    vdec_param.nv21_reg_base_virt_addr =
        (volatile u8 *) ioremap_nocache(vdec_param.nv21_reg_base,
                vdec_param.nv21_reg_size);

    if(vdec_param.nv21_reg_base_virt_addr == NULL)
    {   
        printk(KERN_ALERT  "vdec_drv_g1: failed to ioremap HW regs\n");
        return -EBUSY;
    }   

    vdec_param.g1g2_switch_virt_addr = (volatile u8 *) ioremap_nocache(REG_SWITCH_BETWEEN_G1_G2,4);
    /*
     * decoder and pp shared one irq line, so we must register irq in share mode
     */
    ret = request_irq(IRQ_VDEC, vdec_irq_handle, IRQF_DISABLED | IRQF_SHARED, pdev->name, (void *)(&vdec_param));
    if(ret)
    {
        DBGMSG(ERR,"Fail to request irq for IMAPX vdec device\n");
        return ret;
    }

    //init vdec lock
    mutex_init(&vdec_lock);

    //init sema for race condition
    sema_init(&vdec_param.dec_sema, 1);
    sema_init(&vdec_param.pp_sema, 1);
    sema_init(&vdec_param.mmu_sema, 0);

    //map mempool addr
    vdec_param.mempool_virt_addr = ioremap_nocache(VDEC_MEMPOOL_BASE_PA, VDEC_MEMPOOL_SIZE);

    vdec_param.clk_enable = 0;
    vdec_module_disable();

#ifdef CONFIG_VDEC_DVFS_SUPPORT
    //init dvfs params
    vpu_dvfs.enable = VDEC_DVFS_OFF;
    if((item_exist(DVFS_ENABLE_ITEM) == 1 && item_exist(DVFS_VPU_ENABLE_ITEM) == 1) || item_exist(DVFS_VPU) == 1){
        if((item_integer(DVFS_ENABLE_ITEM, 0) == 1 && item_integer(DVFS_VPU_ENABLE_ITEM, 0) == 1) || item_integer(DVFS_VPU, 0) == 1){
            vpu_dvfs.enable = VDEC_DVFS_ON;
            DBGMSG(TIP, "vdec DVFS enabled\n");
        }
    }
    vpu_dvfs.mode = vpu_dvfs.enable ? DVFS_AUTO : DVFS_FIXED;
    vpu_dvfs.utlz_handle = IM_NULL;
    vpu_dvfs.level_min = DVFS_LEVEL_MIN;
    vpu_dvfs.level_max = DVFS_LEVEL_MAX;

    vpu_dvfs.request = 0;
    vpu_dvfs.request_level = -1/*vpu_dvfs.level_max*/;
    vpu_dvfs.current_level = -1/*vpu_dvfs.level_min*/;
    spin_lock_init(&vpu_dvfs.dvfslock);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
    if (sysfs_create_group(cpufreq_global_kobject,
                &vpu_dvfs_attr_group))
    {
        DBGMSG(ERR, "vpu failed to creat vpu_dvfs_attr_group \n");
        return -EFAULT;
    }
#endif
#endif /*CONFIG_VDEC_DVFS_SUPPORT*/

    DBGMSG(INFO,"IMAPX vdec Driver probe OK\n");

    return VDEC_RET_OK;
}


static int vdec_remove(struct platform_device *pdev)
{
    DBGMSG(INFO, "%s()\n", __func__);

    /* unmap register base */
    iounmap((void *)(vdec_param.mempool_virt_addr));
    iounmap((void *)(vdec_param.reg_base_virt_addr));
    iounmap((void *)(vdec_param.nv21_reg_base_virt_addr));
    iounmap((void *)(vdec_param.g1g2_switch_virt_addr));
    release_mem_region(vdec_param.reg_base_phys_addr, vdec_param.reg_reserved_size);
    release_mem_region(vdec_param.nv21_reg_base, vdec_param.nv21_reg_size);

    printk("vdec_remove done!\n");

    return VDEC_RET_OK;
}

static int vdec_suspend(struct platform_device *pdev, pm_message_t state)
{
    DBGMSG(INFO, "%s(vdec_open_count=%d)\n", __func__, vdec_open_count);

    mutex_lock(&vdec_lock);
    if(vdec_open_count > 0)
    {
#ifdef MMU_SUPPORT
        if(mmu_params.state == VDEC_MMU_ON)
        {
            vdec_mmu_disable(mmu_params.mmu_handle);
            //vdec_param.mmu_state = 	VDEC_MMU_OFF;
        }

        if(vdec_mmu_deinit(mmu_params.mmu_handle) == 0)
        {
            mmu_params.inited = 0;
            mmu_params.mmu_handle = NULL;
        }
#endif 
        vdec_module_disable();
    }
    mutex_unlock(&vdec_lock);
    return VDEC_RET_OK;
}

static int vdec_resume(struct platform_device *pdev)
{
    DBGMSG(INFO, "%s(vdec_open_count=%d)\n", __func__, vdec_open_count);
    mutex_lock(&vdec_lock);
    if(vdec_open_count > 0)
    {
        vdec_module_enable();
#ifdef MMU_SUPPORT
        if(vdec_mmu_init(&(mmu_params.mmu_handle)) == 0)
        {
            mmu_params.inited = 1;
        }
        // assume vdec_param.mmu_state = VDEC_MMU_OFF;
        if(mmu_params.state == VDEC_MMU_ON){
            DBGMSG(ERR, "vdec_resume(), the mmu_state = VDEC_MMU_ON");
            vdec_mmu_enable(mmu_params.mmu_handle);
        }
#endif 
    }
    mutex_unlock(&vdec_lock);

    return VDEC_RET_OK;
}

static struct resource imap_vdec_resource[] = {
    [0] = {
        .start = 0,
        .end = 0,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = 0,
        .end = 0,
        .flags = IORESOURCE_IRQ,
    }
};

static void platform_vdec_release(struct device * dev) { return ; }

struct platform_device imap_vdec_device = {
    .name = "imap-vdec",
    .id = 0,
    .dev = {
        .release = platform_vdec_release,
    },
    .num_resources = ARRAY_SIZE(imap_vdec_resource),
};

/*
 * Tutorial:
 * Platform device driver provides suspend/resume support.
 */
static struct platform_driver vdec_plat = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "imap-vdec",
    },
    .probe = vdec_probe,
    .remove = vdec_remove,
    .suspend = vdec_suspend,
    .resume = vdec_resume,
};

void vdec_module_enable(void)
{
    DBGMSG(INFO, "vdec_module_enable()\n");
    if(clk_prepare_enable(vdec_param.bus_clock) != 0){
        DBGMSG(ERR, "clk_enable(vdec) failed");
    }
    vdec_param.clk_enable = 1;

    module_power_on(SYSMGR_VDEC264_BASE);

    writel(0x1, IO_ADDRESS(SYSMGR_VDEC264_BASE + 0x20));  //enable vdec mmu
    writel(0x1, vdec_param.mempool_virt_addr + 0xc820);	// mempool vdec mode.
    writel(0x1, vdec_param.mempool_virt_addr + 0xd000);	// trigger efuse read.



    // modle reset
    vdec_module_reset();

#ifdef CONFIG_VDEC_DVFS_SUPPORT
    if(vpu_dvfs.enable == VDEC_DVFS_ON){
        if(dvfs_init() != IM_RET_OK){
            DBGMSG(ERR, "dvfs_init() failed");
        }
    }
#endif
}

void vdec_module_disable(void)
{
    DBGMSG(INFO, "vdec_module_disable()\n");
    //
#ifdef CONFIG_VDEC_DVFS_SUPPORT
    if(vpu_dvfs.enable == VDEC_DVFS_ON){
        if(dvfs_deinit() != IM_RET_OK){
            DBGMSG(ERR, "dvfs_deinit() failed");
        }
    }
#endif

    writel(0x0, vdec_param.mempool_virt_addr + 0xd000);	// trigger efuse read.
   
    if(external_enable_power == 0) { 
        writel(0x0, vdec_param.mempool_virt_addr + 0xc820);	// mempool vdec mode.
        module_power_down(SYSMGR_VDEC264_BASE);
    }


    //
    if(vdec_param.clk_enable) {
        clk_disable_unprepare(vdec_param.bus_clock);
        vdec_param.clk_enable = 0;
    }
}

void enable_g1_power(void) 
{
    mutex_lock(&vdec_lock);
    //printk("enable_g1_power(%d)++ \n", vdec_open_count);
    external_enable_power = 1;
    //if(vdec_open_count == 0) {
        module_power_on(SYSMGR_VDEC264_BASE);
    //}
    //printk("enable_g1_power-- \n");
    mutex_unlock(&vdec_lock);
}

void disable_g1_power(void) 
{
    mutex_lock(&vdec_lock);
    //printk("disable_g1_power(%d)++ \n", vdec_open_count);
    external_enable_power = 0;
    if(vdec_open_count == 0) {
        module_power_down(SYSMGR_VDEC264_BASE);
    }
    mutex_unlock(&vdec_lock);
    //printk("disable_g1_power-- \n");

}
EXPORT_SYMBOL(enable_g1_power);
EXPORT_SYMBOL(disable_g1_power);

void vdec_module_reset(void)
{
    DBGMSG(INFO, "vdec_module_reset()\n");
    vdec_reset_asic();
    module_reset(SYSMGR_VDEC264_BASE, 0x7);
}
#ifdef MMU_SUPPORT
int get_mmu_resource(int mmu_enable)
{
    mutex_lock(&vdec_lock);
#ifdef MMU_SUPPORT
    if(mmu_params.state != mmu_enable) {
        if(mmu_params.working > 0) {
            mmu_params.wait++;
            mutex_unlock(&vdec_lock);

            if(down_interruptible(&vdec_param.mmu_sema)){
                return -1;
            }
            mutex_lock(&vdec_lock);
        }

       if(mmu_params.state == VDEC_MMU_ON) {
           if(0 != vdec_mmu_disable(mmu_params.mmu_handle)) {
                vdec_module_reset();
                vdec_mmu_reset(mmu_params.mmu_handle);
            }
           mmu_params.state = VDEC_MMU_OFF;
       } else {
           vdec_mmu_enable(mmu_params.mmu_handle);
           mmu_params.state = VDEC_MMU_ON;
       }
       mmu_params.working++;
    }
#endif
    mutex_unlock(&vdec_lock);

    return 0;
}

void release_mmu_resource()
{
#ifdef MMU_SUPPORT
    mmu_params.working--;
    if(mmu_params.working == 0 && mmu_params.wait > 0) {
        while(mmu_params.wait--) {
            up(&vdec_param.mmu_sema);
        }
    }
#endif
    return;
}

int vdec_mmu_init(vdec_mmu_handle_t *pphandle)
{
#ifdef PMM_USE
    if(pmmdrv_open(pphandle, VDEC_MODULE_NAME) != IM_RET_OK)
    {
        DBGMSG(ERR,"vdec open pmmdrv failed \n");
        return -1;
    }

    if(pmmdrv_dmmu_init(*pphandle, DMMU_DEV_VDEC) != IM_RET_OK)
    {
        DBGMSG(ERR,"vdec  pmmdrv_dmmu_init failed \n");
        return -1;
    }
#else
    DBGMSG(INFO, "vdec_mmu_init()\n");
    *pphandle = imapx_dmmu_init(9);
    if(*pphandle == NULL) {
        DBGMSG(ERR,"vdec  imapx_dmmu_init failed \n");
        return -1;
    }

#endif
    return 0;
}

int vdec_mmu_deinit(vdec_mmu_handle_t phandle)
{
#ifdef PMM_USE
    if(pmmdrv_dmmu_deinit(phandle) != IM_RET_OK)
    {
        DBGMSG(ERR,"vdec  pmmdrv_dmmu_init failed \n");
    }

    if(pmmdrv_release(phandle) != IM_RET_OK)
    {
        DBGMSG(ERR,"vdec open pmmdrv failed \n");
        return -1;
    }
#else
    DBGMSG(INFO, "vdec_mmu_deinit()\n");
    if(imapx_dmmu_deinit(phandle)) {
        DBGMSG(ERR,"vdec  imapx_dmmu_deinit failed \n");
        return -1;
    }
#endif

    return 0;
}

int vdec_mmu_enable(vdec_mmu_handle_t phandle)
{
#ifdef PMM_USE
    if(pmmdrv_dmmu_enable(phandle) != IM_RET_OK)
#else
    DBGMSG(INFO, "vdec_mmu_enable()\n");
    if(imapx_dmmu_enable(phandle)) 
#endif
    {
        DBGMSG(ERR, "vdec Enable dmmu failed\n");
        return -1;
    }
    //writel(1, vdec_param.mempool_virt_addr + 0x30020);	// mmu enable

    return 0;
}

int vdec_mmu_disable(vdec_mmu_handle_t phandle)
{
    DBMSG(INFO, "%s() \n", __func__);
    //writel(0, vdec_param.mempool_virt_addr + 0x30020);	// mmu disable

#ifdef PMM_USE
    if(pmmdrv_dmmu_disable(phandle) != IM_RET_OK)
#else
    DBGMSG(INFO, "vdec_mmu_disable()\n");
    if(imapx_dmmu_disable(phandle)) 
#endif
    {
        DBGMSG(ERR, "vdec Disable dmmu failed\n");
        return -1;
    }

    return 0;
}


int vdec_mmu_reset(vdec_mmu_handle_t phandle)
{
#ifdef PMM_USE
    if(pmmdrv_dmmu_reset(phandle) != IM_RET_OK)
#else
    DBGMSG(INFO, "vdec_mmu_reset()\n");
    if(imapx_dmmu_reset(phandle)) 
#endif
    {
        DBGMSG(ERR, "vdec reset dmmu failed\n");
        return -1;
    }

    return 0;

}
#endif
/*
 * Tutorial:
 * File operation driver interface in Linux kernel. Most api of *struct
 * file_operations* are system calls which are called by user space
 * application entering kernel trap. And most of them have corresponding
 * user space POSIX api, etc(first kernel, then POSIX), open() via open(), 
 * release() via close(), write() via write(), read() via read(), ioctl()
 * via ioctl().
 *
 * And also attention that, most apis in kernel return 0 means success,
 * else return a negative number(normally, return errno type, EINVAL etc).
 *
 * The misc device driver provides convenient driver node register.
 */
static struct file_operations vdec_fops = {
    .owner = THIS_MODULE,
    .open = vdec_open,
    .release = vdec_release,

#ifdef HAVE_UNLOCKED_IOCTL
    .unlocked_ioctl = vdec_ioctl,
#else
    .ioctl = vdec_ioctl,
#endif
    .read = vdec_read,
    .write = vdec_write,
    .mmap = vdec_mmap,
    .poll = vdec_poll,
};

static struct miscdevice vdec_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &vdec_fops,
    .name = VDEC_DEV_NAME,
};

/*
 * Tutorial:
 * An init function with prefix *__init* and called with *module_init*
 * represents it's a kernel module entrance. This function is called 
 * at kernel booting, while the corresponding function with *__exit*
 * prefix is called at current module removed from kernel.
 * If current kernel module is compiled into kernel image, the exit
 * api might never be called. BUT, DON'T ignore it's importance cuz
 * when your module is compiled to be a dynamic one(*.ko), the exit
 st
 * will be called at removing module by command *rmmod xxx*.
 */
static int __init vdec_init(void)
{
    int ret = 0;
    
    g1_set_clk();

    /*Register vdec device */
    ret = platform_device_add_resources(&imap_vdec_device, imap_vdec_resource, ARRAY_SIZE(imap_vdec_resource));
    if(ret != 0) {
        printk( "add vdec device resource error \n");
        return ret;
    }
#if 0
    ret = platform_device_add_data(&imap_vdec_device, NULL, 0);
    if(ret != 0) {
        DBGMSG(ERR, "add vdec device data error \n");
        return ret;
    }
#endif
    ret = platform_device_register(&imap_vdec_device);
    if(ret != 0) {
        printk( "register vdec device error \n");
        return ret;
    }

    /* Register platform device driver. */
    ret = platform_driver_register(&vdec_plat);
    if (unlikely(ret != 0)) {
        printk("Register vdec platform device driver error, %d.\n", ret);
        return ret;
    }

    /* Register misc device driver. */
    ret = misc_register(&vdec_miscdev);
    if (unlikely(ret != 0)) {
        printk("Register vdec misc device driver error, %d.\n", ret);
        return ret;
    }
    printk( "%s() done.\n", __func__);
    return 0;
}


static void __exit vdec_exit(void)
{
    printk( "%s()\n", __func__);

    /* release irq */
    free_irq(IRQ_VDEC, (void *)(&vdec_param));

    misc_deregister(&vdec_miscdev);
    platform_driver_unregister(&vdec_plat);
    platform_device_unregister(&imap_vdec_device);
    mutex_destroy(&vdec_lock);

}

void fw_vdec_x15_0327(){}

module_init(vdec_init);
module_exit(vdec_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ayakashi of InfoTM");
MODULE_DESCRIPTION("vdec subsystem basic level driver");
