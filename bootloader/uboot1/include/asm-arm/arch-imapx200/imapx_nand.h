/***************************************************************************** 
** imapx200.h 
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: All registers used in U-BOOT on imapx200.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 12/16/2009 XXX	Initialized by warits
*****************************************************************************/


#ifndef __IMAPX200_NAND_H__
#define __IMAPX200_NAND_H__


/*! NAND_Flash_Controller */
#define	NFCONF					(NAND_BASE_REG_PA+0x00)
#define	NFCONT					(NAND_BASE_REG_PA+0x04)
#define	NFCMD					(NAND_BASE_REG_PA+0x08)
#define	NFADDR					(NAND_BASE_REG_PA+0x0c)
#define	NFDATA					(NAND_BASE_REG_PA+0x10)
#define	NFMECCD0				(NAND_BASE_REG_PA+0x14)
#define	NFMECCD1				(NAND_BASE_REG_PA+0x18)
#define	NFMECCD2				(NAND_BASE_REG_PA+0x1c)
#define	NFSBLK					(NAND_BASE_REG_PA+0x20)
#define	NFEBLK					(NAND_BASE_REG_PA+0x24)
#define	NFSTAT					(NAND_BASE_REG_PA+0x28)
#define	NFESTAT0				(NAND_BASE_REG_PA+0x2c)
#define	NFESTAT1				(NAND_BASE_REG_PA+0x30)
#define	NFMECC0					(NAND_BASE_REG_PA+0x34)
#define	NFMECC1					(NAND_BASE_REG_PA+0x38)
#define	NFMECC2					(NAND_BASE_REG_PA+0x3c)
#define	NFESTAT2				(NAND_BASE_REG_PA+0x40)
#define	NFSECCD					(NAND_BASE_REG_PA+0x44)
#define	NFSECC					(NAND_BASE_REG_PA+0x48)
#define	NFDMAADDR_A				(NAND_BASE_REG_PA+0x80)
#define	NFDMAC_A				(NAND_BASE_REG_PA+0x84)
#define	NFDMAADDR_B				(NAND_BASE_REG_PA+0x88)
#define	NFDMAC_B				(NAND_BASE_REG_PA+0x8c)


/* Defination of NFCONF */
#define NFCONF_TWRPH0_(x)		((x)<<8)
#define NFCONF_TWRPH1_(x)		((x)<<4)
#define NFCONF_TACLS_(x)		((x)<<12)
#define NFCONF_TMSK				(0x07)
#define NFCONF_ECCTYPE4			(1<<24)		/* 1 for MLC(4-bit), 0 for SLC(1-bit). */
#define NFCONF_BusWidth16		(1<<0)		/* 1 for 16bit, 0 for 8bit. */

/* Defination of NFCONT */
#define NFCONT_MODE				(1<<0)
#define NFCONT_Reg_nCE0			(1<<1)
#define NFCONT_Reg_nCE1			(1<<2)
#define NFCONT_InitSECC			(1<<4)
#define NFCONT_InitMECC			(1<<5)
#define NFCONT_SpareECCLock		(1<<6)
#define NFCONT_MainECCLock		(1<<7)
#define NFCONT_RnB_TransMode	(1<<8)
#define NFCONT_EnRnBINT			(1<<9)
#define NFCONT_EnIllegalAccINT	(1<<10)
#define NFCONT_EnECCDecINT		(1<<12)
#define NFCONT_EnECCEncINT		(1<<13)
#define NFCONT_DMACompleteINT	(1<<14)
#define NFCONT_DMABlockEndINT	(1<<15)
#define NFCONT_SoftLock			(1<<16)
#define NFCONT_LockTight		(1<<17)
#define NFCONT_ECCDirectionEnc	(1<<18)

/* Defination of NFSTAT */
#define NFSTAT_RnB_ro				(1<<0)		/* 0 busy, 1 ready */
#define NFSTAT_nFCE_ro				(1<<2)
#define NFSTAT_nCE_ro				(1<<3)
#define NFSTAT_RnB_TransDetect		(1<<4)		/* write 1 to clear */
#define NFSTAT_IllegalAccess		(1<<5)		/* write 1 to clear(to be confirmed) */
#define NFSTAT_ECCDecDone			(1<<6)		/* write 1 to clear */
#define NFSTAT_ECCEncDone			(1<<7)		/* write 1 to clear */

/* Defination of SLC NFESTAT0 */
#define NFESTAT0_SLC_MErrType_ro_		(0)
#define NFESTAT0_SLC_SErrType_ro_		(2)
#define NFESTAT0_SLC_Err_MSK			(0x03)
#define NFESTAT0_SLC_MByte_Loc_ro_		(7)
#define NFESTAT0_SLC_SByte_Loc_ro_		(21)
#define NFESTAT0_SLC_MByte_Loc_MSK		(0x7ff)
#define NFESTAT0_SLC_SByte_Loc_MSK		(0xf)
#define NFESTAT0_SLC_MBit_Loc_ro_		(4)
#define NFESTAT0_SLC_SBit_Loc_ro_		(18)
#define NFESTAT0_SLC_Bit_MSK			(0x7)

/* Defination of MLC NFESTAT0 & NFESTAT1 & NFESTAT2*/
#define NFESTAT0_Busy_ro				(1<<31)
#define NFESTAT0_Ready_ro				(1<<30)
#define NFESTAT0_MLC_MErrType_ro_		(27)
#define NFESTAT0_MLC_MErrType_MSK		(0x7)
#define	NFESTAT0_MLC_Loc1_ro_			(0)
#define	NFESTAT1_MLC_Loc2_ro_			(0)
#define	NFESTAT1_MLC_Loc3_ro_			(9)
#define	NFESTAT1_MLC_Loc4_ro_			(18)
#define NFESTAT0_MLC_PT1_ro_			(16)
#define NFESTAT2_MLC_PT2_ro_			(0)
#define NFESTAT2_MLC_PT3_ro_			(9)
#define NFESTAT2_MLC_PT4_ro_			(18)
#define NFESTATX_MLC_PT_MSK				(0x1ff)
#define NFESTATX_MLC_Loc_MSK			(0x1ff)

/* Defination of DMAC */
#define NFDMAC_DMADIROut						(1<<25)
#define NFDMAC_DMAAUTO							(1<<26)
#define NFDMAC_DMAALT							(1<<27)
#define NFDMAC_DMARPT							(1<<28)
#define NFDMAC_DMAWIND							(1<<29)
#define NFDMAC_DMARST							(1<<30)
#define NFDMAC_DMAEN							(1<<31)

/*!END NAND_Flash_Controller */

#endif /*__IMAPX200_NAND_H__*/
