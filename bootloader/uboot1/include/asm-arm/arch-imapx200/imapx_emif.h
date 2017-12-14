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


#ifndef __IMAPX200_EMIF_H__
#define __IMAPX200_EMIF_H__

/* EMIF-0-DRAM */

#define DSCONR0_OFFSET				(0x00)
#define DSTMG0R0_OFFSET				(0x04)
#define DSTMG1R0_OFFSET				(0x08)
#define DSCTLR0_OFFSET				(0x0c)
#define DSREFR0_OFFSET				(0x10)
#define DSCSLR0_0_LOW_OFFSET		(0x14)
#define DSCSLR0_1_LOW_OFFSET		(0x18)
#define DSMSKR0_0_OFFSET			(0x54)
#define DSMSKR0_1_OFFSET			(0x58)
#define DEXN_MODE_REG0_OFFSET		(0xac)
#define DCOMP_VERSION0_OFFSET		(0xf8)
#define DCOMP_TYPE0_OFFSET			(0xfc)
#define FLASH_TRPDR0_OFFSET			(0xa0)

/* EMIF-0-SRAM */

#define SSMCTLR0_OFFSET				(0xa4)
#define SSMTMGR0_SET0_OFFSET		(0x94)


/* EMIF-1-DRAM */

#define DSCONR1_OFFSET				(0x00)
#define DSTMG0R1_OFFSET				(0x04)
#define DSTMG1R1_OFFSET				(0x08)
#define DSCTLR1_OFFSET				(0x0c)
#define DSREFR1_OFFSET				(0x10)
#define DSCSLR1_0_LOW_OFFSET		(0x14)
#define DSCSLR1_1_LOW_OFFSET		(0x18)
#define DSMSKR1_0_OFFSET			(0x54)
#define DSMSKR1_1_OFFSET			(0x58)
#define DEXN_MODE_REG1_OFFSET		(0xac)
#define DCOMP_VERSION1_OFFSET		(0xf8)
#define DCOMP_TYPE1_OFFSET			(0xfc)
#define FLASH_TRPDR1_OFFSET			(0xa0)

/* EMIF-1-SRAM */

#define SSMCTLR1_OFFSET				(0xa4)
#define SSMTMGR1_SET0_OFFSET		(0x94)

/* SDRAM Value for EMIF */
#define SCONRV32            (0x1c3188)
#define SCONRV              (0x1c1188)
#define STMG0RV             (0x2261696)
#define STMG1RV             (0x70008)
#define SREFRV              (0x100)
#define SCSLR0_LOWV         (0x40000000)
#define SCSLR1_LOWV         (0x44000000)
#define SCSLR0_LOWV1        (0x50000000)
#define SCSLR1_LOWV1        (0x54000000)
#define SMSKR0V             (0xc)
#define SMSKR1V             (0xc)
#define SMSKR2V             (0x2c)
#define SMSKR3V             (0x42c)
#define SMSKR4V             (0x82c)
#define SMSKR5V             (0x42c)
#define SMSKR6V             (0x40c)
#define SMSKR7V             (0x40c)
#define CSALIAS0_LOWV       (0x80000000)
#define CSALIAS1_LOWV       (0x18000000)
#define CSREMAP0_LOWV       (0x80000000)
#define CSREMAP1_LOWV       (0x12000000)
#define SMTMGR_SET0V        (0x11d4a)
#define SMTMGR_SET1V        (0x791d4a)
#define SMTMGR_SET2V        (0x191d4a)
#define FLASH_TRPDRV        (0xc8)
#define SMCTLRV             (0x2480)
#define SYNC_FLASH_OPCODEV  (0x0)
#define ETN_MODE_REGV       (0x0)
#define SFCONRV             (0x22d)
#define SFCTLRV             (0x10)
#define SFTMGRV             (0x252)
#define SCTLRV              (0x3089)


#endif /*__IMAPX200_EMIF_H__*/
