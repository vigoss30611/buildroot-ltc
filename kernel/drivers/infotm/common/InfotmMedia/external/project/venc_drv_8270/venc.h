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
 *
 * Revision History:
 *  v1.0.2	leo@2012/06/05: tidy up the code.
 */

#ifndef __VENC_H__
#define __VENC_H__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/mman.h>

#include <asm/io.h>
#include <asm/atomic.h>



/* registers related macros */
#define VENC_ACT_REG_SIZE		(96 * 4)
#ifdef SOC_IMAPX800
#define VENC_ACT_REG_BASE		(0x25100000)
#endif // SOC_IMAPX800
#ifdef SOC_IMAPX900
#define VENC_ACT_REG_BASE		(0x28500000)
#endif // SOC_IMAPX900

#define VENC_ACT_INTR_ID		(129)

/* device number related macros */
#define ENCODE_DYNAMIC_MAJOR		0
#define ENCODE_DYNAMIC_MINOR		255
#define ENCODE_DEFAULT_MAJOR		113	/* better not use default major or minor */
#define ENCODE_DEFAULT_MINOR		113

/* Ioctl commands related macros */
#define HX280ENC_IOCGHWOFFSET       0x1000
#define HX280ENC_IOCGHWIOSIZE       0x1001
#define HX280ENC_IOC_RESERVE_HW     0x1002
#define HX280ENC_IOC_RELEASE_HW     0x1003
#define HX280ENC_IOC_CHECK_MMU      0x1004
#undef EWL_DEBUG
#ifdef EWL_DEBUG
#define HX280ENC_IOC_DEBUG          0x1005
#endif
#define HX280ENC_IOC_DVFS           0x1006



#define DVFS_ENABLE_ITEM	"DVFS.enable"
#define DVFS_VPU_ENABLE_ITEM	"dvfs.vpu.enable"
#define DVFS_VPU  "soc.vpu.dvfs"

typedef struct {
	int major;
	int minor;
	dev_t dev;
	unsigned int enc_instance;
	void __iomem *reg_base_virt_addr;
	unsigned int reg_base_phys_addr;
	unsigned int reg_reserved_size;
	
	struct clk *	busClock;
	bool		dvfsEna;

    bool        mmu_inited;
    bool        mmu_enabled;
    int         clk_enable;
    struct imapx_dmmu_handle_t *mmu_handle;
} venc_param_t;


#endif  /* __VENC_H__ */
