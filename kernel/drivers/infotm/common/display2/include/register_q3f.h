/*
 * register_q3f.h - q3f display register defination
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

#define LCDVCLKFSR			0x030
#define IDSINTPND			0x054	//LCD Interrupt pending
#define IDSSRCPND			0x058	//LCD Interrupt source
#define IDSINTMSK			0x05c	//LCD Interrupt mask

#define IDSINTPND_LCDINT		0	/* LCD frame synchronized interrupt pending bit */
#define IDSINTPND_VCLKINT		1	/*VCLK frequency auto change interrupt pending bit*/
#define IDSINTPND_OSDW0		2	/*OSD Win0 Timeslot synchronized interrupt pending bit*/
#define IDSINTPND_OSDW1		3	/*OSD Win1 Timeslot synchronized interrupt pending bit*/
#define IDSINTPND_OSDERR		4	/*OSD error interrupt pending bit, need to reset OSD*/
#define IDSINTMSK_I80INT			5
#define IDSINTPND_RSVD			6
#define IDSINTPND_TVINT			7	/* TVIF frame synchronized interrupt pending bit */

#define LCDCON1_LINECNT			18	// [29:18]
#define LCDCON1_CLKVAL			8	// [17:8]
#define LCDCON1_ENVID			0	// [0:0]

#define LCDCON2_VBPD			16	// [26:16]
#define LCDCON2_VFPD			0	// [11:0]

#define LCDCON3_VSPW			16	// [26:16]
#define LCDCON3_HSPW			0	// [10:0]

#define LCDCON4_HBPD 			16	// [26:16]
#define LCDCON4_HFPD			0	// [10:0]

#define LCDCON5_RGB565IF 		31	// [31]
#define LCDCON5_BT656_MODE	30
#define LCDCON5_RGBORDER		24	/* [29:24] RGB output order of TFT LCD display interface.*/

/*Note: SRGB TYPE 00 = RGB Dummy 01 = Dummy RGB 10 = RGB */
#define LCDCON5_SRGB_TYPE		18	/*[19:18]*/
#define LCDCON5_SRGB_ENABLE		17

#define LCDCON5_VSTATUS			15	// [16:15]
#define LCDCON5_HSTATUS 		13	// [14:13]
#define LCDCON5_DSPTYPE			11	// [12:11]
#define LCDCON5_INVVCLK			10	// [10:10]
#define LCDCON5_INVVLINE		9	// [9:9]
#define LCDCON5_INVVFRAME		8	// [8:8]
#define LCDCON5_INVVD			7	// [7:7]
#define LCDCON5_INVVDEN			6	// [6:6]

#define LCDCON6_LINEVAL 		16	// [26:16]
#define LCDCON6_HOZVAL			0	// [10:0]

#define OVCDCR			0x1000
#define OVCPCR			0x1004	/* Overlay Controller Palette Control Register */
#define OVCBKCOLOR	0x1008	/*Overlay Controller Background Color Register*/
#define	OVCWPR		0x100C	/*: Overlay Controller Windows Parameter Register */
#define OVCW0CR		0x1080	/* : Overlay Controller Window 0 Control Register */
#define OVCW0PCAR		0x1084	/*: Overlay Controller Window 0 Position Control A Register*/
#define OVCW0PCBR		0x1088	/* Overlay Controller Window 0 Position Control B Register */
#define OVCW0VSSR		0x108c	/* Virtual screen page width in the unit: pixel */
#define OVCW0CMR		0x1090	/* Overlay Controller Window 0 Color Map Register */

#define OVCW0B0SAR	0x1094	/* Overlay Controller Window 0 Buffer 0 Start Address Register */
#define OVCW0B1SAR	0x1098
#define OVCW0B2SAR	0x109c
#define OVCW0B3SAR	0x10a0

#define OVCW1CR		0x1100	/* Overlay Controller Window 1 Control Register */
#define OVCW1PCAR		0x1104	/* Overlay Controller Window 1 Position Control A Register */
#define OVCW1PCBR		0x1108	/* Overlay Controller Window 1 Position Control B Register */
#define OVCW1PCCR		0x110c	/* Overlay Controller Window 1 Position Control C Register */
#define OVCW1VSSR		0x1110	/* Overlay Controller Window 1 Virtual Screen Size Register */

#define OVCW1CKCR		0x1114	/* Overlay Controller Window 1 Color Key Control Register */
#define OVCW1CKR		0x1118	/*Overlay Controller Window 1 Color Key Register */
#define OVCW1CMR		0x111c	/*Overlay Controller Window 1 Color MAP Register*/

#define OVCW1B0SAR	0x1120	/*Overlay Controller Window 1 Buffer 0 Start Address Register */
#define OVCW1B1SAR	0x1124
#define OVCW1B2SAR	0x1128
#define OVCW1B3SAR	0x112c

#define OVCBRB0SAR	0x1300	/*Overlay Controller CbCr Buffer 0 Start Address Register*/
#define OVCBRB1SAR	0x1304
#define OVCBRB2SAR	0x1308
#define OVCBRB3SAR	0x130c

#define OVCOMC			0x1310	/* Overlay Controller Color Matrix Configure Register */
#define OVCOEF11		0x1314	/* Overlay Controller Color Matrix Coefficient11 Register */
#define OVCOEF12		0x1318
#define OVCOEF13		0x131c
#define OVCOEF21		0x1320
#define OVCOEF22		0x1324
#define OVCOEF23		0x1328
#define OVCOEF31		0x132c
#define OVCOEF32		0x1330
#define OVCOEF33		0x1334

#define OSDREGNUM		(OVCOEF33 + 1)

#define OVCPSCCR		0x13C0	/* Overlay Controller PreScaler Channel Control Register  */
#define OVCPSCPCR		0x13C4	/* Overlay Controller PreScaler Channel Position Control Register */
#define OVCPSVSSR		0x13C8	/* Overlay Controller PreScaler Channel Virtual Screen Size Register */

#define OVCPSB0SAR		0x13CC	/* Overlay Controller PreScaler Channel Buffer 0 Start Address Register */
#define OVCPSB1SAR		0x13D0
#define OVCPSB2SAR		0x13D4
#define OVCPSB3SAR		0x13D8

#define OVCPSCB0SAR	0x13DC	/* Overlay Controller PreScaler Channel CBCR Buffer 0 Start Address Register */
#define OVCPSCB1SAR	0x13E0
#define OVCPSCB2SAR	0x13E4
#define OVCPSCB3SAR	0x13E8

/*Note: Overlay Controller Window 0 Palette RAM, 256 entries*/
#define OVCW0PAL0		0x1400
#define OVCW0PAL255	0x17fc

/*Note: Overlay Controller Window 1 Palette RAM, 256 entries*/
#define OVCW1PAL0		0x1800
#define OVCW1PAL255	0x1bfc


// OVCDCR 0x1000 Overlay Controller Display Control Register

/*Bypass image enhance module include ACC/ACM/GAMMA*/
#define OVCDCR_Image_bypass	(22)

/* load channel1 parameter, only valid when sp_load_en =1 */
#define OVCDCR_w1_sp_load	(14)

/* load channel0 parameter, only valid when sp_load_en =1 */
#define OVCDCR_w0_sp_load	(13)

#define OVCDCR_sp_load_en	(12)
#define OVCDCR_UpdateReg	(11)	/* Global load parameter enable */
#define OVCDCR_ScalerMode	(10)
#define OVCDCR_Enrefetch	(9)
#define OVCDCR_Enrelax		(8)
#define OVCDCR_WaitTime	(4)
#define OVCDCR_SafeBand	(3)
#define OVCDCR_AllFetch		(2)
#define OVCDCR_IfType		(1)
#define OVCDCR_Interlace	(0)

#define OVCDCR_SCALER_BEFORE_OSD	(1)	/* Scaler used for individual channel*/
#define OVCDCR_SCALER_AFTER_OSD		(0)	/* Scaler used for osd output */

/* OVCPCR 0x1004 Overlay Controller Palette Control Register */
#define OVCPCR_UPDATE_PAL	(15)
#define OVCPCR_W1PALFM		(3)
#define OVCPCR_W0PALFM		(0)

/* OVCWPR 0x100C Overlay Controller Windows Parameter Register */
#define OVCWPR_OSDWINX		(0)
#define OVCWPR_OSDWINY		(12)

/*  OVCWxCR 0: 0x1080 1:0x1100 Overlay Controller Window x Control Register */
#define OVCWxCR_BUFSEL		(17)
#define OVCWxCR_BUFAUTOEN	(16)
#define OVCWxCR_BUFNUM		(14)

/*Note:
	Select Alpha value.

	case1: When Per plane blending case( BLD_PIX ==0) 
		0 = using ALPHA0_R/G/B values 
		1 = using ALPHA1_R/G/B values 

	case2: When Per pixel blending ( BLD_PIX ==1) 
		0 = selected by AEN (A value) or Chroma key 
		1 = using DATA[27:24] data (when BPPMODE=5'b01110 &5'b01111),
			using DATA[31:24] data (when BPPMODE=5'b10000) 
*/
#define OVCWxCR_ALPHA_SEL	(8)
#define OVCWxCR_BLD_PIX		(7)	/*Select blending category */

#define OVCWxCR_RBEXG		(6)
#define OVCWxCR_BPPMODE		(1)
#define OVCWxCR_ENWIN		(0)

/*  OVCWxPCAR Overlay Controller Window 0 Position Control A Register */
#define OVCWxPCAR_FIELDSTATUS	(31)
#define OVCWxPCAR_BUFSTATUS	(29)
#define OVCWxPCAR_LeftTopY	(16)
#define OVCWxPCAR_LeftTopX	(0)

/* OVCWxPCBR Overlay Controller Window 0 Position Control B Register */
#define OVCWxPCBR_RightBotY	(16)
#define OVCWxPCBR_RightBotX	(0)

/* OVCOMC 0x1310 Overlay Controller Color Matrix Configure Register */
#define OVCOMC_ToRGB		(31)
#define OVCOMC_OddEven		(24)
#define OVCOMC_BitSwap		(20)
#define OVCOMC_Bits2Swap	(19)
#define OVCOMC_Bits4Swap	(18)
#define OVCOMC_ByteSwap		(17)
#define OVCOMC_HawSwap		(16)
#define OVCOMC_CBCR_CH 		(13)
#define OVCOMC_oft_b		(8)
#define OVCOMC_oft_a		(0)

/* OVCPSCCR 0x13C0 Overlay Controller PreScaler Channel Control Register */
#define OVCPSCCR_WINSEL		(20)

/*PreScaler channel use CbCr dma channel select */
#define OVCPSCCR_CBRCHSEL	(19)

#define OVCPSCCR_BUFSEL		(17)
#define OVCPSCCR_BPPMODE		(1)
#define OVCPSCCR_EN			(0)

/* Overlay Controller Window 0 Control Register OVCW0CR,offset=0x1080 */

#define OVCW0CR_HAWSWP		(9)
#define OVCW0CR_BYTSWP		(10)

#define OVCW1CR_HAWSWP		(9)
#define OVCW1CR_BYTSWP		(10)

/* : Overlay Controller Window 1 Color Key Control C Register, offset=0x1114*/
#define OVCW1CKCR_KEYBLEN	(26)
#define OVCW1CKCR_KEYEN		(25)
#define OVCW1CKCR_DIRCON		(24)
#define OVCW1CKCR_COMPKEY	(0)

/* Overlay Controller PreScaler Channel Virtual Screen Size Register, OVCPSVSSR,offset=0x13C8 */
#define OVCPSVSSR_BUF3_BITADR	(28)
#define OVCPSVSSR_BUF2_BITADR	(24)
#define OVCPSVSSR_BUF1_BITADR	(20)
#define OVCPSVSSR_BUF0_BITADR	(16)

#define OVCPSVSSR_VW_WIDTH		(0)
#define OVCPSVSSR_VW_WIDTH_SZ	(16)

#define OVCW1CKCR_KEYBLEN	(26)
#define OVCW1CKCR_KEYEN		(25)
#define OVCW1CKCR_DIRCON		(24)
#define OVCW1CKCR_COMPKEY	(0)

#define OVCW1CKCR_DIRCON_BACK_DISPLAY 0
#define OVCW1CKCR_DIRCON_FORE_DISPLAY 1


/*TVIF*/
#define IMAP_TVCCR		0x2000	/* TVIF Clock Configuration Register */
#define IMAP_TVICR		0x2004	/* TVIF Configuration Register */
#define IMAP_TVCOEF11	0x2008	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF12	0x200C	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF13	0x2010	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF21	0x2014	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF22	0x2018	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF23	0x201C	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF31	0x2020	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF32	0x2024	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCOEF33	0x2028	/* TVIF Color Space Convert Matrix Coefficient Register */
#define IMAP_TVCMCR	0x202C	/* TVIF Color Matrix Configure Register */
#define IMAP_TVUBA1	0x2030	/* TVIF Upper Blanking Area 1 Line Register */
#define IMAP_TVUNBA	0x2034	/* TVIF Upper Non-blanking Area Line Register */
#define IMAP_TVUBA2	0x2038	/* TVIF Upper Blanking Area 2 Line Register */
#define IMAP_TVLBA1	0x203C	/* TVIF Lower Blanking Area 1 Line Register */
#define IMAP_TVLNBA	0x2040	/* TVIF Lower Non-blanking Area Line Register */
#define IMAP_TVLBA2	0x2044	/* TVIF Lower Blanking Area 2 Line Register */
#define IMAP_TVBLEN	0x2048	/* TVIF Line Blanking Length Register */
#define IMAP_TVVLEN	0x204c	/* TVIF Line Video Length Register */
#define IMAP_TVHSCR	0x2050	/* TVIF Hsync Configure Register */
#define IMAP_TVVSHCR	0x2054	/* TVIF Vsync Upper Configure Register */
#define IMAP_TVVSLCR	0x2058	/* TVIF Vsync lower Configure Register */
#define IMAP_TVXSIZE	0x205C	/* TVIF Display Horizontal Size Register */
#define IMAP_TVYSIZE	0x2060	/* TVIF Display Vertical Size Register */
#define IMAP_TVSTSR		0x2064	/* TVIF status Register */

#define SCLIFSR			0x5000	/* Scaler Input Frame Size Register */
#define SCLOFSR			0x5004	/* Scaler Output Frame Size Register */
#define RHSCLR			0x5008	/* Ratio of Horizontal Scaling Register */
#define RVSCLR			0x500c	/* Ratio of Vertical Scaling Register */
#define SCLCR			0x5010	/* Scaler Control Register */
#define SCLAZIR			0x5014	/* Scaling Algorithm of Zoom-in Register */
#define SCLAZOR			0x5018	/* Scaling Algorithm of Zoom-out Register */

#define SCLCR_BYPASS	1
#define SCLCR_ENABLE	0

#define SCLIFSR_IVRES	16		/* Scaler Input Frame Vertical Resolution [28:16] */
#define SCLIFSR_IHRES	0		/* Scaler Input Frame Horizontal Resolution [12:0] */

#define SCLIFSR_OVRES	16		/* Scaler Output Frame Vertical Resolution [28:16] */
#define SCLIFSR_OHRES	0		/* Scaler Output Frame Horizontal Resolution [12:0] */

/*Note: CSCLAR 0 ~ 63*/
#define CSCLAR0			0x501C
#define CSCLAR1			0x5020
#define CSCLAR2			0x5024
#define CSCLAR3			0x5028
/*Note: 4 ~ 62*/
#define CSCLAR63		0x5118

#define DITFSR			0x5C00	/* Dither Frame Size Register */
#define DITCR			0x5C04	/*Dither Control Register */
#define DITCOEF0		0x5C08	/*Dither Matrix 00 - 03 Register */
#define DITCOEF1		0x5C0C	/*Dither Matrix 04 - 07 Register */
#define DITCOEF2		0x5C10	/* Dither Matrix 10 -13 Register */
#define DITCOEF3		0x5C14	/*Dither Matrix 14 - 17 Register */
#define DITCOEF4		0x5C18	/*Dither Matrix 20 - 23 Register */
#define DITCOEF5		0x5C1C	/*Dither Matrix 24 - 27 Register */
#define DITCOEF6		0x5C20	/*Dither Matrix 30 - 33 Register */
#define DITCOEF7		0x5C24	/*Dither Matrix 34 - 37 Register */

/*Note: In Q3F platform, IDS0 REG BASE is 0x22300000 and the IDS1 is no exist */
#define IDS0_REG_BASE	0x22300000
#define IDS0_REG_SIZE	0x7000
#define IDS0_IRQID		87

#define IDS_PATH_NUM	1
#define IDS_OVERLAY_NUM		2

#endif
