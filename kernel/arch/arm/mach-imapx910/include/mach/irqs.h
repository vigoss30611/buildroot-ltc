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

#define NR_IRQS			512

#define IRQ_LOCALTIMER  (29)
#define IRQ_GLOBLTIMER  (27)

#define GIC_IIS0_ID		(32)
#define GIC_IIS1_ID		(33)

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
#define GIC_PCM0_ID		(46)
//#define GIC_PCM2_ID		()
#define GIC_SSP0_ID		(47)
#define GIC_SSP1_ID		(48)
#define GIC_SSP2_ID		(49)
#define GIC_SPDIF_ID		(50)
#define GIC_SIMC0_ID		(51)
#define GIC_SIMC1_ID		(52)
#define GIC_UART0_ID		(53)
#define GIC_UART1_ID		(54)
#define GIC_UART2_ID		(55)
#define GIC_UART3_ID		(56)
#define GIC_PIC0_ID		(57)
#define GIC_PIC1_ID		(58)
#define GIC_AC97_ID		(59)
#define GIC_KEYBD_ID		(60)
#define GIC_KEYWK_ID		(61)
#define GIC_ADC_ID		(62)
#define GIC_GPG_ID		(63)
#define GIC_GP0_ID		(64)
#define GIC_GP1_ID		(65)
#define GIC_GP2_ID		(66)
#define GIC_GP3_ID		(67)
#define GIC_GP4_ID		(68)
#define GIC_GP5_ID		(69)
#define GIC_DMIC_ID     (70)       /* Larry */
#define GIC_PWMA_ID     (71)       /* sun */
#define GIC_IIC4_ID		(72)       /* Larry */
#define GIC_IIC5_ID		(73)       /* Larry */

#define GIC_GP6_ID		(74)
#define GIC_GP7_ID		(75)
#define GIC_GP8_ID		(76)
#define GIC_GP9_ID		(77)
#define GIC_WTD_ID      (78)

#define GIC_IDS0_ID		(80)
#define GIC_DSI_ID		(81)

#define GIC_IDS1_ID		(96)
#define GIC_HDMITX_ID		(97)
#define GIC_HDMIWK_ID		(98)
#define GIC_GPSACQ_ID		(99)
#define GIC_GPSTCK_ID		(100)
#define GIC_CRYPTO_ID		(101)

#define GIC_PPXXX0_ID		(112)
#define GIC_PPMMU0_ID		(113)
#define GIC_PPXXX1_ID		(114)
#define GIC_PPMMU1_ID		(115)
#define GIC_PPXXX2_ID		(116)
#define GIC_PPMMU2_ID		(117)
#define GIC_PPXXX3_ID		(118)
#define GIC_PPMMU3_ID		(119)
#define GIC_GPXXXG_ID		(120)
#define GIC_GPMMUG_ID		(121)
#define GIC_GPUPMU_ID		(122)

#define GIC_VDEC_ID		(128)
#define GIC_VENC_ID		(129)
#define GIC_VP8_ID		(130)

#define GIC_ISP_ID		(144)
#define GIC_CSI0_ID		(145)
#define GIC_CSI1_ID		(146)
#define GIC_CSI2_ID		(147)
#define GIC_TSIF0_ID		(148)
#define GIC_TSIF1_ID		(149)

#define GIC_EHCI_ID		(160)
#define GIC_OHCI_ID		(161)
#define GIC_OTG_ID		(162)
#define GIC_MMC1_ID		(163)
#define GIC_MMC2_ID		(164)
#define GIC_MMC0_ID		(165)

#define GIC_NAND_ID		(176)

#define GIC_SATA_ID		(178)
#define GIC_MAC_ID		(179)

#define GIC_DMA0_ID		(192)
#define GIC_DMA1_ID		(193)
#define GIC_DMA2_ID		(194)
#define GIC_DMA3_ID		(195)
#define GIC_DMA4_ID		(196)
#define GIC_DMA5_ID		(197)
#define GIC_DMA6_ID		(198)
#define GIC_DMA7_ID		(199)
#define GIC_DMA8_ID		(200)
#define GIC_DMA9_ID		(201)
#define GIC_DMA10_ID		(202)
#define GIC_DMA11_ID		(203)
#define GIC_DMABT_ID		(204)

#define GIC_RTCTCK_ID		(208)
#define GIC_RTCINT_ID		(209)
#define GIC_MGRPM_ID		(210)  /* system managment power-mode interrupt */
#define GIC_MGRPW_ID		(211)  /* system managment power-wake interrupt */
#define GIC_MGRSD_ID		(212)  /* system managment shut-down interrupt */
#define GIC_RTCGP1_ID		(214)  /* RTC GPIO1 interrupt*/

#define GIC_PMU0_ID		(224)
#define GIC_PMU1_ID		(225)
#define GIC_PMU2_ID		(226)
#define GIC_PMU3_ID		(227)

#define GIC_L2CH_ID		(240)
#define GIC_G2D_ID		(249)

#define IRQ_IDS0 		(80)
#define IRQ_IDS1 		(96)

#define IMAP_GIC_CPU_BASE	(IMAP_SCU_BASE + 0x100)		
#define IMAP_GIC_DIST_BASE	(IMAP_SCU_BASE + 0x1000)


#endif /* __ASM_ARCH_IRQ_H */
