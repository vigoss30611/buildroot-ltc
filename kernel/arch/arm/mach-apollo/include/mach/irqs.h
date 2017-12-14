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

/* IMAPX800 IRQ */

#define NR_IRQS		490	

#define IRQ_LOCALTIMER  (29)
#define IRQ_GLOBLTIMER  (27)

#define GIC_MIIS_ID		(32)
#define GIC_IISM_ID		(33)

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
#define GIC_IIC4_ID		(72)
#define GIC_PCM0_ID		(46)
#define GIC_SSP0_ID		(47)
#define GIC_SSP1_ID		(48)
#define GIC_SSP2_ID		(49)
#define GIC_UART0_ID		(53)
#define GIC_UART1_ID		(54)
#define GIC_UART2_ID		(55)
#define GIC_UART3_ID		(56)
#define GIC_ADC_ID		(62)
#define GIC_GPG_ID		(63)
#define GIC_GP0_ID		(64)
#define GIC_GP1_ID		(65)
#define GIC_GP2_ID		(66)
#define GIC_GP3_ID		(67)
#define GIC_GP4_ID		(68)
#define GIC_GP5_ID		(69)
#define GIC_GP6_ID		(74)
#define GIC_GP7_ID		(75)
#define GIC_GP8_ID		(76)
#define GIC_GP9_ID		(77)
#define GIC_WTD_ID      (78)
#define GIC_PCM1_ID     (79)
#define GIC_ISP_ID		(80)
#define GIC_CAMIF_ID	(82)
#define GIC_CSI0_ID     (83)
#define GIC_CSI1_ID     (84)
#define GIC_CSI2_ID     (85)
#define GIC_IDS1_ID		(111)
#define GIC_HDMITX_ID		(112)
#define GIC_HDMIWK_ID		(113)
#define GIC_CRYPTO_ID		(114)
#define GIC_FODET_ID		(115)
#define GIC_ISPOST_ID		(116)
#define GIC_VDEC264_ID		(128)
#define GIC_VENC264_ID		(129)
#define GIC_VDEC265_ID		(130)
#define GIC_VENC265_ID		(131)
#define GIC_EHCI_ID     (160)
#define GIC_OHCI_ID     (161)
#define GIC_OTG_ID      (162)
#define GIC_MMC1_ID     (163)
#define GIC_MMC2_ID     (164)
#define GIC_MMC0_ID     (165)
#define GIC_USBCHRG_ID	(166)
#define GIC_MAC_ID      (167)
#define GIC_DMA0_ID     (192)    
#define GIC_DMA1_ID     (193)    
#define GIC_DMA2_ID     (194)    
#define GIC_DMA3_ID     (195)    
#define GIC_DMA4_ID     (196)    
#define GIC_DMA5_ID     (197)    
#define GIC_DMA6_ID     (198)    
#define GIC_DMA7_ID     (199)    
#define GIC_DMA8_ID     (200)    
#define GIC_DMA9_ID     (201)    
#define GIC_DMA10_ID        (202)
#define GIC_DMA11_ID        (203)
#define GIC_DMABT_ID        (204)
#define GIC_RTCGP3_ID       (208)
#define GIC_RTCINT_ID       (209)
#define GIC_RTCSLP_ID       (210)
#define GIC_MGRSD_ID        (211)
#define GIC_MGRPW_ID        (212)
#define GIC_RTCGP0_ID       (213)
#define GIC_RTCGP1_ID       (214)
#define GIC_RTCGP2_ID       (215)
#define GIC_MGRPM_ID        (216)
#define GIC_CAMIF_DMMU_ID	(217)
#define GIC_IDS1_DMMU_ID	(218)
#define GIC_FODET_DMMU_ID	(219)
#define GIC_ISPOST_DMMU_ID	(220)
#define GIC_VDEC_DMMU_ID	(221)
#define GIC_VENC264_DMMU_ID (222)
#define GIC_VENC265_DMMU_ID (223)
#define GIC_CPUPMU0_ID		(224)
#define GIC_CPU0CMTX_ID		(228)
#define GIC_CPU0CMRX_ID		(232)
#define GIC_L2CH_ID			(240)
#define GIC_EMIFPMU0_ID     (248)

#define IMAP_GIC_CPU_BASE	(IMAP_SCU_BASE + 0x100)		
#define IMAP_GIC_DIST_BASE	(IMAP_SCU_BASE + 0x1000)


#endif /* __ASM_ARCH_IRQ_H */
