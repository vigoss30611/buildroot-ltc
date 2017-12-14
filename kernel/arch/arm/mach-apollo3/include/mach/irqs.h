/* arch/arm/mach-imap/include/mach/irqs.h
 *
 * Copyright (c) 2003-2005 Simtec Electronics
 *   Ben Dooks <ben@simtec.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H __FILE__

#include <mach/imap-iomap.h>

#ifndef __ASM_ARM_IRQ_H
#error "Do not include this directly, instead #include <asm/irq.h>"
#endif

/* APOLLO3 IRQ */

#define NR_IRQS			512

#define IRQ_LOCALTIMER  (29)
#define IRQ_GLOBLTIMER  (27)

#define GIC_MIIS_ID		(32)
#define GIC_CMNT0_ID		(35)
#define GIC_CMNT1_ID		(36)
#define GIC_PWM0_ID		(37)
#define GIC_PWM1_ID		(38)
#define GIC_PWM2_ID		(39)
#define GIC_PWM3_ID		(40)
#define GIC_PWM4_ID		(41)
#define GIC_IIC0_ID		(42)
#define GIC_IIC1_ID		(43)
#define GIC_IIC2_ID		(44)
#define GIC_IIC3_ID		(45)
#define GIC_IIC4_ID		(46)
#define GIC_PCM0_ID		(48)
#define GIC_SSP0_ID		(50)
#define GIC_SSP1_ID		(51)
#define GIC_SPIMUL_ID		(50)
#define GIC_UART0_ID		(56)
#define GIC_UART2_ID		(58)
#define GIC_UART3_ID		(59)
#define GIC_IR_ID		(63)
#define GIC_ADC_ID		(66)
#define GIC_WDT_ID		(67)
#define GIC_GPG_ID		(68)
#define GIC_GP0_ID		(69)
#define GIC_GP1_ID		(70)
#define GIC_GP2_ID		(71)
#define GIC_GP3_ID		(72)
#define GIC_GP4_ID		(73)
#define GIC_GP5_ID		(74)
#define GIC_GP6_ID		(75)
#define GIC_GP7_ID		(76)
#define GIC_ISP_ID		(80)
#define GIC_CAMIF_ID	(82)
#define GIC_CSI0_ID     (84)
#define GIC_CSI1_ID     (85)
#define GIC_CSI2_ID     (86)
#define GIC_IDS0_ID		(87)
#define GIC_CRYPTO_ID		(181)
#define GIC_FODET_ID		(112)
#define GIC_ISPOST0_ID		(114)
#define GIC_ISPOST1_ID		(115)
#define GIC_ISPOST2_ID		(116)
#define GIC_ISPOST3_ID		(117)
#define GIC_VENC264_ID		(130)
#define GIC_EHCI_ID     (160)
#define GIC_OHCI_ID     (161)
#define GIC_OTG_ID      (162)
#define GIC_MMC1_ID     (164)
#define GIC_MMC2_ID     (165)
#define GIC_MMC0_ID     (166)
#define GIC_USBCHRG_ID	(166)
#define GIC_MAC_ID      (180)
#define GIC_DMA0_ID     (167)    
#define GIC_DMA1_ID     (168)    
#define GIC_DMA2_ID     (169)    
#define GIC_DMA3_ID     (170)    
#define GIC_DMA4_ID     (171)    
#define GIC_DMA5_ID     (172)    
#define GIC_DMA6_ID     (173)    
#define GIC_DMA7_ID     (174)    
#define GIC_DMA8_ID     (175)    
#define GIC_DMA9_ID     (176)    
#define GIC_DMA10_ID        (177)
#define GIC_DMA11_ID        (178)
#define GIC_DMABT_ID        (179)

//#define GIC_RTCGP3_ID       (208)
#define GIC_RTCINT_ID       (209)
#define GIC_RTCSLP_ID       (210)
#define GIC_MGRSD_ID        (211)
//#define GIC_MGRPW_ID        (212)
#define GIC_RTCGP0_ID       (213)
#define GIC_RTCGP1_ID       (214)
//#define GIC_RTCGP2_ID       (215)
//#define GIC_MGRPM_ID        (216)
#define GIC_CPUPMU0_ID		(224)
#define GIC_CPU0CMTX_ID		(228)
#define GIC_CPU0CMRX_ID		(232)
#define GIC_EMIFPMU0_ID     	(248)
#define GIC_DSP0_ID	(250)
#define GIC_DSP1_ID	(251)
#define GIC_DSP2_ID	(252)
#define GIC_DSP3_ID	(253)

#define IMAP_GIC_CPU_BASE	(IMAP_SCU_BASE + 0x100)		
#define IMAP_GIC_DIST_BASE	(IMAP_SCU_BASE + 0x1000)


#endif /* __ASM_ARCH_IRQ_H */
