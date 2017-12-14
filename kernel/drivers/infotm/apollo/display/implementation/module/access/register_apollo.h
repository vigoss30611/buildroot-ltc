/*
 * x15_register.h - x15 display register defination
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */

#ifndef __DISPLAY_ACCESS_REGISTER_HEADER__
#define __DISPLAY_ACCESS_REGISTER_HEADER__



#define LCDCON1			0x000	//LCD control 1
#define LCDCON2			0x004	//LCD control 2
#define LCDCON3			0x008	//LCD control 3
#define LCDCON4			0x00c	//LCD control 4
#define LCDCON5			0x010	//LCD control 5
#define LCDCON6			0x018	
#define LCDVCLKFSR		0x030	
#define IDSINTPND		0x054	//LCD Interrupt pending
#define IDSSRCPND		0x058	//LCD Interrupt source
#define IDSINTMSK		0x05c	//LCD Interrupt mask

#define IDSINTPND_LCDINT 		0
#define IDSINTPND_VCLKINT 		1
#define IDSINTPND_OSDW0 		2
#define IDSINTPND_OSDW1 		3
#define IDSINTPND_OSDERR 		4
#define IDSINTMSK_I80INT 		5
#define IDSINTPND_OSDW2 		6
#define IDSINTPND_TVINT 			7

#define LCDCON1_LINECNT			18	// [29:18]
#define LCDCON1_CLKVAL			8	// [17:8]
#define LCDCON1_VMMODE			7	// [7:7]
#define LCDCON1_PNRMODE			5
#define LCDCON1_STNBPP			1
#define LCDCON1_ENVID			0	// [0:0]

#define LCDCON2_VBPD			16	// [26:16]
#define LCDCON2_VFPD			0	// [10:0]

#define LCDCON3_VSPW			16	// [26:16]
#define LCDCON3_HSPW			0	// [10:0]

#define LCDCON4_HBPD 			16	// [26:16]
#define LCDCON4_HFPD			0	// [10:0]

#define LCDCON5_RGBORDER		24	// [29:24]
#define LCDCON5_CONFIGORDER		20	// [22:20] 0->dsi24bpp, 1->dsi16bpp1, 2->dsi16bpp2,3->dsi16bpp3,4->dsi18bpp1,5->dsi18bpp2
#define LCDCON5_VSTATUS			15	// [16:15]
#define LCDCON5_HSTATUS 		13	// [14:13]
#define LCDCON5_DSPTYPE			11	// [12:11]
#define LCDCON5_INVVCLK			10	// [10:10]
#define LCDCON5_INVVLINE		9	// [9:9]
#define LCDCON5_INVVFRAME		8	// [8:8]
#define LCDCON5_INVVD			7	// [7:7]
#define LCDCON5_INVVDEN			6	// [6:6]
#define LCDCON5_INVPWREN		5	// [5:5]
#define LCDCON5_PWREN			3	// [3:3]

#define LCDCON6_LINEVAL 		16	// [26:16]
#define LCDCON6_HOZVAL			0	// [10:0]


#define OVCDCR			0x1000
#define OVCPCR			0x1004
#define OVCBKCOLOR	0x1008
#define	OVCWPR		0x100C
#define OVCW0CR		0x1080
#define OVCW0PCAR		0x1084
#define OVCW0PCBR		0x1088	
#define OVCW0VSSR		0x108c
#define OVCW0CMR		0x1090
#define OVCW0B0SAR	0x1094
#define OVCW0B1SAR	0x1098
#define OVCW0B2SAR	0x109c
#define OVCW0B3SAR	0x10a0
#define OVCW1CR		0x1100
#define OVCW1PCAR		0x1104
#define OVCW1PCBR		0x1108
#define OVCW1PCCR		0x110c
#define OVCW1VSSR		0x1110
#define OVCW1CKCR		0x1114
#define OVCW1CKR		0x1118
#define OVCW1CMR		0x111c
#define OVCW1B0SAR	0x1120
#define OVCW1B1SAR	0x1124
#define OVCW1B2SAR	0x1128
#define OVCW1B3SAR	0x112c	
#define OVCW2CR		0x1180
#define OVCW2PCAR		0x1184
#define OVCW2PCBR		0x1188
#define OVCW2PCCR		0x118c
#define OVCW2VSSR		0x1190
#define OVCW2CKCR		0x1194
#define OVCW2CKR		0x1198
#define OVCW2CMR		0x119c
#define OVCW2B0SAR	0x11a0
#define OVCW2B1SAR	0x11a4
#define OVCW2B2SAR	0x11a8
#define OVCW2B3SAR	0x11ac	
#define OVCW3CR		0x1200
#define OVCW3PCAR		0x1204
#define OVCW3PCBR		0x1208
#define OVCW3PCCR		0x120c
#define OVCW3VSSR		0x1210
#define OVCW3CKCR		0x1214
#define OVCW3CKR		0x1218
#define OVCW3CMR		0x121c
#define OVCW3B0SAR	0x1220
#define OVCW3SABSAR	0x1224	
#define OVCBRB0SAR		0x1300
#define OVCBRB1SAR		0x1304
#define OVCBRB2SAR		0x1308
#define OVCBRB3SAR		0x130c
#define OVCOMC			0x1310
#define OVCOEF11		0x1314
#define OVCOEF12		0x1318
#define OVCOEF13		0x131c
#define OVCOEF21		0x1320
#define OVCOEF22		0x1324
#define OVCOEF23		0x1328
#define OVCOEF31		0x132c
#define OVCOEF32		0x1330
#define OVCOEF33		0x1334
#define OVCPSCCR        0x13C0
#define OVCPSCPCR       0x13C4
#define OVCPSVSSR       0x13C8
#define OVCPSB0SAR      0x13CC
#define OVCPSB1SAR      0x13D0
#define OVCPSB2SAR      0x13D4
#define OVCPSB3SAR      0x13D8
#define OVCPSCB0SAR     0x13DC
#define OVCPSCB1SAR     0x13E0
#define OVCPSCB2SAR     0x13E4
#define OVCPSCB3SAR     0x13E8
#define SCLIFSR         0x5000
#define SCLOFSR         0x5004
#define RHSCLR          0x5008
#define RVSCLR          0x500c
#define SCLCR           0x5010
#define SCLAZIR			0x5014
#define SCLAZOR			0x5018
#define CSCLAR0			0x501C
#define CSCLAR1			0x5020
#define CSCLAR2			0x5024
#define CSCLAR3			0x5028


#define OVCW0PAL0       0x1400
#define OVCW0PAL255     0x17fc
#define OVCW1PAL0       0x1800
#define OVCW1PAL255     0x1bfc
                              
#define OVCPSPAL0       0x1f80
#define OVCPSPAL255     0x1fbc



#define OSDREGNUM		(OVCOEF33 + 1)

// OVCDCR
#define OVCDCR_W2_AUTOBUFFER_SOURCE	(20)
#define OVCDCR_W1_AUTOBUFFER_SOURCE	(18)
#define OVCDCR_W0_AUTOBUFFER_SOURCE	(16)
#define OVCDCR_UpdateReg	(11)
#define OVCDCR_ScalerMode	(10)
#define OVCDCR_Enrefetch	(9)
#define OVCDCR_Enrelax		(8)
#define OVCDCR_WaitTime		(4)
#define OVCDCR_SafeBand		(3)
#define OVCDCR_AllFetch		(2)
#define OVCDCR_IfType		(1)
#define OVCDCR_Interlace	(0)

// OVCPCR
#define OVCPCR_UPDATE_PAL	(15)
#define OVCPCR_W3PALFM		(9)
#define OVCPCR_W2PALFM		(6)
#define OVCPCR_W1PALFM		(3)
#define OVCPCR_W0PALFM		(0)

//OVCWPR
#define OVCWPR_OSDWINX		(0)
#define OVCWPR_OSDWINY		(12)

// OVCWxCR
#define OVCWxCR_BUFSEL		(17)
#define OVCWxCR_BUFAUTOEN	(16)
#define OVCWxCR_ALPHA_SEL   (8)
#define OVCWxCR_BLD_PIX     (7)
#define OVCWxCR_RBEXG       (6)
#define OVCWxCR_BPPMODE		(1)
#define OVCWxCR_ENWIN		(0)

// OVCWxPCAR
#define OVCWxPCAR_FIELDSTATUS	(31)
#define OVCWxPCAR_BUFSTATUS	(29)
#define OVCWxPCAR_LeftTopY	(16)
#define OVCWxPCAR_LeftTopX	(0)

// OVCWxPCBR
#define OVCWxPCBR_RightBotY	(16)
#define OVCWxPCBR_RightBotX	(0)

// OVCOMC
#define OVCOMC_ToRGB		(31)
#define OVCOMC_OddEven		(24)
#define OVCOMC_BitSwap		(20)
#define OVCOMC_Bits2Swap	(19)
#define OVCOMC_Bits4Swap	(18)
#define OVCOMC_ByteSwap		(17)
#define OVCOMC_HawSwap		(16)
#define OVCOMC_oft_b		(8)
#define OVCOMC_oft_a		(0)

//OVCPSCCR //fengwu alex 20150514
#define OVCPSCCR_WINSEL		(20)
#define OVCPSCCR_BUFSEL     (17)
#define OVCPSCCR_BPPMODE    (1)
#define OVCPSCCR_EN         (0)

#define OVCW0CR_HAWSWP     (9) 
#define OVCW0CR_BYTSWP     (10) 
                               
#define OVCW1CR_HAWSWP     (9) 
#define OVCW1CR_BYTSWP     (10)


/*TVIF*/
#define IMAP_TVCCR		0x2000		/* TVIF Clock Configuration Register */
#define IMAP_TVICR		0x2004		/* TVIF Configuration Register */
#define IMAP_TVCOEF11	0x2008		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF12	0x200C		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF13	0x2010		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF21	0x2014		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF22	0x2018		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF23	0x201C		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF31	0x2020		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF32	0x2024		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF33	0x2028		/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCMCR	0x202C		/* TVIF Color Matrix Configure Register */
#define IMAP_TVUBA1	0x2030		/* TVIF Upper Blanking Area 1 Line Register */
#define IMAP_TVUNBA	0x2034		/* TVIF Upper Non-blanking Area Line Register */
#define IMAP_TVUBA2	0x2038		/* TVIF Upper Blanking Area 2 Line Register */
#define IMAP_TVLBA1		0x203C		/* TVIF Lower Blanking Area 1 Line Register */
#define IMAP_TVLNBA	0x2040		/* TVIF Lower Non-blanking Area Line Register */
#define IMAP_TVLBA2		0x2044		/* TVIF Lower Blanking Area 2 Line Register */
#define IMAP_TVBLEN		0x2048		/* TVIF Line Blanking Length Register */
#define IMAP_TVVLEN	0x204c		/* TVIF Line Video Length Register */
#define IMAP_TVHSCR	0x2050		/* TVIF Hsync Configure Register */
#define IMAP_TVVSHCR	0x2054		/* TVIF Vsync Upper Configure Register */
#define IMAP_TVVSLCR	0x2058		/* TVIF Vsync lower Configure Register */
#define IMAP_TVXSIZE	0x205C		/* TVIF Display Horizontal Size Register */
#define IMAP_TVYSIZE	0x2060		/* TVIF Display Vertical Size Register */
#define IMAP_TVSTSR		0x2064		/* TVIF status Register */


#define ENH_CFG_BASE_OFFSET 	(0x5800)

#define ENH_FSR 			(ENH_CFG_BASE_OFFSET + 0x0000)
#define ENH_IECR 			(ENH_CFG_BASE_OFFSET + 0x0004)
#define ENH_ACCCOER 		(ENH_CFG_BASE_OFFSET + 0x0080)
#define ENH_ACCLUTR			(ENH_CFG_BASE_OFFSET + 0x0084)
#define ENH_ACMTHER     	(ENH_CFG_BASE_OFFSET + 0x008C)
#define ENH_RGAMMA 			(0x5900)
#define ENH_GGAMMA 			(0x5980)
#define ENH_BGAMMA 			(0x5A00)

#define ENH_IECR_GAROUND		20
#define ENH_IECR_GAPASSBY		19
#define ENH_IECR_ACMROUND		18
#define ENH_IECR_ACMPASSBY 		17
#define ENH_IECR_ACCREADY		16
#define ENH_IECR_ACCROUND		15
#define ENH_IECR_ACCPASSBY		14
#define ENH_IECR_HISTNFMS		9
#define ENH_IECR_HISTROUND		8
#define ENH_IECR_HISTPASSBY		7
#define ENH_IECR_Y2RROUND		6
#define ENH_IECR_Y2RPASSBY		5
#define ENH_IECR_R2YROUND		4
#define ENH_IECR_R2YPASSBY		3
#define ENH_IECR_EEDNOEN		2
#define ENH_IECR_EEROUND		1
#define ENH_IECR_EEPASSBY		0

#define ENH_ACCCOER_ACCCOE 		16

#define ENH_ACMCOEST			0
#define ENH_ACCLUTR_ACCLUTA 	16
#define ENH_FSR_VRES			16
#define ENH_FSR_HRES			0

#define ENH_IE_ACC_LUTB_LENGTH 1792
#define ENH_IE_GAMMA_COEF_LENGTH 32




#define DSI_REG_BASE	0x22040000
#define DSI_REG_SIZE	0x1000
#define DSI_IRQID	81

#endif
