/*
 * arch/arm/mach-imap/include/mach/io.h
 *
 * Copyright (C) 1997 Russell King
 *	     (C) 2003 Simtec Electronics
*/

#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#include <mach/hardware.h>
/***********This file is used for static map of peripheral address**************/

#define IMAP_IO_BASE			0xFE000000UL

#ifndef __ASSEMBLY__
#define IMAP_IO_MEM(x)     ((void __iomem __force *)IMAP_IO_BASE + (x))
#else
#define IMAP_IO_MEM(x)     (IMAP_IO_BASE + (x))
#endif

/************************Virtual address for peripheral*************************/
#define IMAP_SYSMGR_VA       			IMAP_IO_MEM(0x00000000)
#define IMAP_SCU_VA            	 		IMAP_IO_MEM(0x00100000)
#define IMAP_TIMER_VA				IMAP_IO_MEM(0x00200000)
#define IMAP_IRAM_VA				IMAP_IO_MEM(0x00300000)
#define IMAP_EMIF_VA				IMAP_IO_MEM(0x00400000)
#define IMAP_IIC0_VA				IMAP_IO_MEM(0x00500000)

#define IO_TO_VIRT_BETWEEN(p, st, sz) ((p) >= (st) && (p) < ((st) + (sz)))
#define IO_TO_VIRT_XLATE(p, pst, vst) (((p) - (pst) + (vst)))

#define IO_TO_VIRT(n) (\
	IO_TO_VIRT_BETWEEN((n), IMAP_SYSMGR_BASE, IMAP_SYSMGR_SIZE) ?		\
		IO_TO_VIRT_XLATE((n), IMAP_SYSMGR_BASE, IMAP_SYSMGR_VA) :	\
	IO_TO_VIRT_BETWEEN((n), IMAP_SCU_BASE, IMAP_SCU_SIZE) ?			\
		IO_TO_VIRT_XLATE((n), IMAP_SCU_BASE, IMAP_SCU_VA) :		\
	IO_TO_VIRT_BETWEEN((n), IMAP_TIMER_BASE, IMAP_TIMER_SIZE) ?		\
		IO_TO_VIRT_XLATE((n), IMAP_TIMER_BASE, IMAP_TIMER_VA) :		\
	IO_TO_VIRT_BETWEEN((n), IMAP_IRAM_BASE, IMAP_IRAM_SIZE) ?		\
		IO_TO_VIRT_XLATE((n), IMAP_IRAM_BASE, IMAP_IRAM_VA) :		\
	IO_TO_VIRT_BETWEEN((n), IMAP_EMIF_BASE, IMAP_EMIF_SIZE) ?               \
		IO_TO_VIRT_XLATE((n), IMAP_EMIF_BASE, IMAP_EMIF_VA) :           \
	IO_TO_VIRT_BETWEEN((n), IMAP_IIC0_BASE, IMAP_IIC0_SIZE) ?               \
		IO_TO_VIRT_XLATE((n), IMAP_IIC0_BASE, IMAP_IIC0_VA) :           \
	NULL)

#define IO_ADDRESS(n)	(IO_TO_VIRT(n))
//#define IMAPX800_SYS_CALLED_1			(IMAP_VA_SRAM + 0x1700C)
//#define IMAPX800_SYS_PCADDR			(IMAP_VA_SRAM + 0x17000)
//#define IMAPX800_L2_CACHE_ADDR_L0		(IMAP_VA_SYSMGR + 0x884C)
//#define IMAPX800_L2_WAY_CFG			(IMAP_VA_SYSMGR + 0x8840)
//#define PERIPHERAL_BASE_ADDR_PA			(0x20000000)


#define IO_SPACE_LIMIT 0xffffffff


/*
 * 1:1 mapping for ioremapped regions.
 */
#define __io(a)		__typesafe_io(a)
#define __mem_pci(x)	(x)

#endif
