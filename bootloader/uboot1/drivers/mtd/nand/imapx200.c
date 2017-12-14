/***************************************************************************** 
** imapx200.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: NAND driver for iMAPx200.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0  XXX 12/16/2009 XXX	Initialized by warits
*****************************************************************************/


/* 
 * Change default ecc layout schemes in nand_base.c is much better than
 * define it here. Or else you'll have to do "nand_scan" yourself before
 * really nand_scan.
 */

#include <common.h>
#include <nand.h>
#include <asm/errno.h>
#include <asm/io.h>

#if defined(CONFIG_NAND_SPL) || defined(CONFIG_MEM_POOL) 
#include "preloader.h"
#endif

#undef DEBUG_OOB

struct nand_chip_setting {

	u_char tacls;
	u_char twrph0;
	u_char twrph1;

	u_char number;
	u_char eccbytes;
	u_short eccsize;
};

/* iMAP NFCON support a maximum of 2 chips */
struct nand_chip_setting chip_setting[] = {
	{
		/* settings of chip 0 */
		.tacls = 7,
		.twrph0 = 7,
		.twrph1 = 7,
		.number = 0,
		.eccsize = CONFIG_SYS_NAND_ECCSIZE,
		.eccbytes = CONFIG_SYS_NAND_ECCBYTES,
	},{
		/* settings of chip 1 */
		.tacls = 7,
		.twrph0 = 7,
		.twrph1 = 7,
		.number = 1,
		.eccsize = CONFIG_SYS_NAND_ECCSIZE,
		.eccbytes = CONFIG_SYS_NAND_ECCBYTES,
	}
};

uint32_t nand_dirtmark_loc = 0;

#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_MEM_POOL)
#define __nand_msg(args...) printf("iMAP NAND: " args)
/*#define __nand_msg(args...) printf(args)*/
#else
#define __nand_msg(args...) writel(readl(CONFIG_SYS_SDRAM_BASE) + 1, CONFIG_SYS_SDRAM_BASE)
#endif

#ifndef CONFIG_NAND_SPL
static void imap_nand_wait_enc(void)
{
	while(!(readl(NFSTAT) & NFSTAT_ECCEncDone));  
	writel(NFSTAT_ECCEncDone, NFSTAT);
	udelay(20);

	return ;
}
#endif

static void imap_nand_wait_dec(void)
{
	while(!(readl(NFSTAT) & NFSTAT_ECCDecDone));  
	writel(NFSTAT_ECCDecDone, NFSTAT);
	while(!(readl(NFESTAT0) & NFESTAT0_Ready_ro));
//	udelay(20);

	return ;
}

static void imap_nand_hwinit(void)
{
	/* Setting time args */
	u_long nfconf, nfcont;

	nfconf = readl(NFCONF);
	nfconf &= ~NFCONF_TACLS_(0x07);
	nfconf &= ~NFCONF_TWRPH0_(0x07);
	nfconf &= ~NFCONF_TWRPH1_(0x07);
	nfconf |= NFCONF_TACLS_(0x01);
	nfconf |= NFCONF_TWRPH0_(0x03);
	nfconf |= NFCONF_TWRPH1_(0x01);

	/* 
	 * If there are more than one chips with different buswidth on board,
	 * I'll have to move this into select_chip func.
	 */
#if defined(CONFIG_SYS_NAND_BUSW16)
	nfconf |= NFCONF_BusWidth16;
#else
	nfconf &= ~NFCONF_BusWidth16;
#endif
	writel(nfconf, NFCONF);

	nfcont = readl(NFCONT);
	nfcont |= NFCONT_MODE;
	nfcont &= ~NFCONT_SoftLock;
	nfcont &= ~NFCONT_LockTight;
	writel(nfcont, NFCONT);
	return ;
}

#if defined(CONFIG_SYS_NAND_MLC)
static int imap_nand_mlc_bitfix(uint8_t * dat, u_short loc, u_short pt)
{
	int index = 0;
	while(1)
	{
		if (index > 8) break;
		if (pt & 0x01)
		  dat[(index + loc * 9) / 8] ^= (1 << ((index + loc * 9) % 8));
		pt >>= 1;
		index++;
	}
	return 0;
}
#else
static int imap_nand_slc_bitfix(u_char *dat, u_short byteid, ushort bitid)
{
	dat[byteid] ^= (1 << bitid);
	return 0;
}
#endif

#ifndef CONFIG_NAND_SPL
static int imap_nand_freepage(u_char *oob)
{
	if(*(uint8_t *)(oob + nand_dirtmark_loc + 3)
//	   == 0xffffffff)
	   == 0xff)
	  return 1;

	/* Not all 0xff values ,this is not a free page */
	return 0;
}
#endif

/***************************
 * BOARD SPECIFIC FUNCTIONS
 ***************************/

static void imap_nand_select_chip(struct mtd_info *mtd, int chip)
{
	u_long ctrl = readl(NFCONT);

	if ((chip * chip) > 1) /* We only support -1, 0, 1 three options */
	  return;

	ctrl |= 6;
	if (chip != -1)
	  ctrl &= ~(1 << (chip + 1));

	writel(ctrl, NFCONT);
}

/* 
 * Hardware specific function for controlling ALE/CLE/nCE.
 * Also used to write command and address.
 */
static void imap_nand_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_chip_setting *cset = (struct nand_chip_setting *)chip->priv;

	if (ctrl & NAND_CTRL_CHANGE)
	{
		if (ctrl & NAND_NCE)
		  chip->select_chip(mtd, cset->number);
		else
		  chip->select_chip(mtd, -1);
	}

	if (cmd != NAND_CMD_NONE)
	{
		while(!chip->dev_ready(mtd));
		if (ctrl & NAND_CLE)
		  writeb(cmd, NFCMD);
		else if (ctrl & NAND_ALE)
		  writeb(cmd, NFADDR);
	}
}


static int imap_nand_dev_ready(struct mtd_info *mtd)
{
	return (readl(NFSTAT) & NFSTAT_RnB_ro);
}


/* iMAPx200 use hardware ECC by default */
static void imap_nand_ecc_hwctl(struct mtd_info *mtd, int mode)
{
	u_long nfcont, nfconf;

	nfconf = readl(NFCONF);
	/* nand_type == 0 indicates that this is a SLC Flash */
	/* We use 1bit ECC method for SLC Flash, 4bit for MLC */
#if defined(CONFIG_SYS_NAND_MLC)
	  nfconf |= NFCONF_ECCTYPE4;
#else
	  nfconf &= ~NFCONF_ECCTYPE4;
#endif
	writel(nfconf, NFCONF);

	/* Init main ECC and unlcok */
	nfcont = readl(NFCONT);
	nfcont |= NFCONT_InitMECC;
	nfcont &= ~NFCONT_MainECCLock;

#if defined(CONFIG_SYS_NAND_MLC)
	if (mode == NAND_ECC_WRITE)
	  nfcont |= NFCONT_ECCDirectionEnc;
	else if (mode == NAND_ECC_READ)
	  nfcont &= ~NFCONT_ECCDirectionEnc;
#endif

	writel(nfcont, NFCONT);
}

/* SPL do not need ecc calulation */
#ifndef CONFIG_NAND_SPL
/* 
 * ECC calculate function:
 * when decoding, this function does nothing.
 * when encoding, this function copy ECC code from NFMECC0/1/2 to ecc_code
 */
static int imap_nand_ecc_calculate(struct mtd_info *mtd, const u_char *dat,
   u_char *ecc_code)
{
	u_long nfcont;
#if defined(CONFIG_SYS_NAND_MLC)
	u_long nfmecc0, nfmecc1, nfmecc2;
	int i;
#endif
	struct nand_chip *chip = mtd->priv;

	/* We do "lock engine" and "wait ecc decoding done" in .correct but not here */
	if (chip->state != FL_WRITING && chip->state != FL_CACHEDPRG)
	  return 0;

	/* Lock the ECC engine */
	nfcont = readl(NFCONT);
	nfcont |= NFCONT_MainECCLock;
	writel(nfcont, NFCONT);

	/* FIXME: GUESSING ... */
	/* We get 4/8bytes ECC code for x8/16 SLC chip, and 9 bytes for MLC */
		/* Wait encoding done */
#if defined(CONFIG_SYS_NAND_MLC)
	imap_nand_wait_enc();

	nfmecc0 = readl(NFMECC0);
	nfmecc1 = readl(NFMECC1);
	nfmecc2 = readl(NFMECC2);

	/*
	 * iMAP use 8 parity codes per 512 Bytes in 4 bit ECC mode, each 9 bits
	 */
	for (i = 0; i < chip->ecc.bytes; i++)
	{
		if(i < 4)
		{
			*(ecc_code + i) = nfmecc0 & 0xff;
			nfmecc0 >>= 8;
		}
		else if(i < 8)
		{
			*(ecc_code + i) = nfmecc1 & 0xff;
			nfmecc1 >>= 8;
		}
		else
		  *(ecc_code + i) = nfmecc2 & 0xff;
	}
#else
	/* SLC */

	*(u_long *)(ecc_code)		= cpu_to_le32(readl(NFMECC0));
#if defined(CONFIG_SYS_NAND_BUSW16)
	*(u_long *)(ecc_code + 4)	= cpu_to_le32(readl(NFMECC1));
#endif
#endif
	return 0;
}
#endif /* CONFIG_NAND_SPL */

/*
 * Error correct function
 * correct a maximum of 4bits error per 512 when using 4bit ECC mode
 * correct a maximum of 1bit error per 2048 when using 1bit ECC mode
 * return err corrected, if uncorrectable err occured, return -1
 */
static int imap_nand_ecc_correct(struct mtd_info *mtd, u_char *dat,
   u_char *read_ecc, u_char *calc_ecc)
{
	u_long nfestat0, nfestat1, nfestat2, nfcont;
	u_char err_type;

	/* Lock the ECC engine */
	nfcont = readl(NFCONT);
	nfcont |= NFCONT_MainECCLock;
	writel(nfcont, NFCONT);

#if defined(CONFIG_SYS_NAND_MLC)
	/*FIXME: write ECC into nfmeccd* if needed */
#if 0 
	writel(cpu_to_le32(*(u_long *)(calc_ecc)), NFMECCD0);
	writel(cpu_to_le32(*(u_long *)(calc_ecc + 4)), NFMECCD1);
	writel(cpu_to_le32(*(u_long *)(calc_ecc + 8)), NFMECCD2);
#endif

	imap_nand_wait_dec();

	/* FIXME: Do I need to clear the dec done bit AND
	 * wait the ECC busy bit & check the ECC ready bit?
	 */

	nfestat0 = readl(NFESTAT0);
	nfestat1 = readl(NFESTAT1);
	nfestat2 = readl(NFESTAT2);

	err_type = (nfestat0 >> NFESTAT0_MLC_MErrType_ro_) & NFESTAT0_MLC_MErrType_MSK;

	switch(err_type)
	{   
		case 4: /* 4 bits error, correctable */
			imap_nand_mlc_bitfix(dat,
			   (nfestat1 >> NFESTAT1_MLC_Loc4_ro_) & NFESTATX_MLC_Loc_MSK,
			   (nfestat2 >> NFESTAT2_MLC_PT4_ro_) & NFESTATX_MLC_PT_MSK);
		case 3: /* 3 bits error, correctable */
			imap_nand_mlc_bitfix(dat,
			   (nfestat1 >> NFESTAT1_MLC_Loc3_ro_) & NFESTATX_MLC_Loc_MSK,
			   (nfestat2 >> NFESTAT2_MLC_PT3_ro_) & NFESTATX_MLC_PT_MSK);
		case 2: /* 2 bits error, correctalbe */
			imap_nand_mlc_bitfix(dat,
			   (nfestat1 >> NFESTAT1_MLC_Loc2_ro_) & NFESTATX_MLC_Loc_MSK,
			   (nfestat2 >> NFESTAT2_MLC_PT2_ro_) & NFESTATX_MLC_PT_MSK);
		case 1: /* 1 bits error, correctable */
			imap_nand_mlc_bitfix(dat,
			   (nfestat0 >> NFESTAT0_MLC_Loc1_ro_) & NFESTATX_MLC_Loc_MSK,
			   (nfestat0 >> NFESTAT0_MLC_PT1_ro_) & NFESTATX_MLC_PT_MSK);
			__nand_msg("%d bit ECC error, corrected.\n", err_type);
		case 0:
			return (int)err_type;
		default:
			__nand_msg("Uncorrectable ECC error.\n");
			return -1; 
	}   
#else
	/* SLC */
	/*FIXME: write ECC into nfmeccd* if needed */
#if 0
	writel(cpu_to_le32(*(u_long *)(calc_ecc)), NFMECCD0);
#if defined(CONFIG_SYS_NAND_BUSW16)
	writel(cpu_to_le32(*(u_long *)(calc_ecc + 4)), NFMECCD1);
#endif
#endif
	nfestat0 = readl(NFESTAT0);
	err_type = (nfestat0 >> NFESTAT0_SLC_MErrType_ro_) & NFESTAT0_SLC_Err_MSK;

	switch(err_type)
	{
		case 0: /* No error detected */
			return 0;
		case 1: /* 1 bit error detected */
			imap_nand_slc_bitfix(dat,
			   (nfestat0 >> NFESTAT0_SLC_MByte_Loc_ro_) & NFESTAT0_SLC_MByte_Loc_MSK,
			   (nfestat0 >> NFESTAT0_SLC_MBit_Loc_ro_) & NFESTAT0_SLC_Bit_MSK);
			return 1;
		case 2: /* More than one err */
		case 3: /* ECC error */
			__nand_msg("Uncorrectable ECC error.\n");
			return -1; 
	}
#endif
	return 0;
}

/*
 * XXX BIG NOTE : XXX
 * The read/write_buf function applied here read/write "wrong" endine
 * data into/from NAND, so the "fault" becomes invisible after one
 * write-read-go. I don't want to spend time on endine change, and will
 * test if this method can cause any problem.
 * 
 * T.T but I can't ..., the 1st 8K in nand is copied by hardware.
 * Maybe I will test this in kernel.
 */

/* Apply a 32bit read_buf here to speed up */
#if !defined(CONFIG_SYS_NAND_SDMA)
static void imap_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{       
	int i;
	struct nand_chip *chip = mtd->priv;

	for (i = 0; i < (len >> 2); i++)
	  *(u_long *)(buf + i * 4) = cpu_to_le32(readl(chip->IO_ADDR_R));
}

/* Apply a 32bit write_buf here to speed up */
static void imap_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	int i;
	struct nand_chip *chip = mtd->priv;

	for (i = 0; i < (len >> 2); i++)
	  writel(cpu_to_le32(*(u_long *)(buf + i * 4)), chip->IO_ADDR_W);
}

#else /* Use SDMA method for transportation */
static void imap_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{       
	int i;
	struct nand_chip *chip = mtd->priv;

	if(len < 512) /* using poll method if len is short */
	  for (i = 0; i < (len >> 2); i++)
		*(u_long *)(buf + i * 4) = cpu_to_le32(readl(chip->IO_ADDR_R));
	else {
		writel((u_long)buf, NFDMAADDR_A);
		writel(len | NFDMAC_DMADIROut | NFDMAC_DMAEN, NFDMAC_A);
		/* Flush D-Cache before transmission */
		inv_cache((uint32_t)buf, (uint32_t)(buf + len));
	}

	/* Wait DMA to finish */
	while(readl(NFDMAC_A) & NFDMAC_DMAEN);
}

/* Apply a 32bit write_buf here to speed up */
static void imap_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	int i;
	struct nand_chip *chip = mtd->priv;

	if(len < 512) /* using poll method if len is short */
	  for (i = 0; i < (len >> 2); i++)
		writel(cpu_to_le32(*(u_long *)(buf + i * 4)), chip->IO_ADDR_W);
	else {
		clean_cache((uint32_t)buf, (uint32_t)(buf + len));
		writel((u_long)buf, NFDMAADDR_A);
		writel(len | NFDMAC_DMAEN, NFDMAC_A);
	}

	/* Wait DMA to finish */
	while(readl(NFDMAC_A) & NFDMAC_DMAEN);
}
#endif	/* SDMA */

/* SPL do not need read/write_page */
#ifndef CONFIG_NAND_SPL
/*
 * Science we now need to read the ECC codes out of OOB before any
 * page data, so none of the default read_page method(HW, SW, SYND)
 * can be used. This special odd function is applied here.
 */
static int imap_nand_read_page(struct mtd_info *mtd, struct nand_chip *chip,
   u_char *buf)
{
	int i, stat;
	int eccsize  = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int eccoffs  = mtd->oobsize - eccbytes * eccsteps;
	int col      = mtd->writesize + eccoffs;
#if defined(CONFIG_SYS_NAND_MLC)
	u_long nfmeccd0, nfmeccd1, nfmeccd2;
#endif
	u_char *p = buf;

	/* Read the ECC codes in OOB */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, mtd->writesize, -1);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

#if defined(DEBUG_OOB) && !defined(CONFIG_NAND_SPL)
	for (i = 0; i < 128; i++)
	{
		if ( i % 16 == 0) printf("\n");
		printf("%02x ", *(chip->oob_poi + i));
	}
#endif

	/* If this is a free page, return 0xff values without reading */
	if(imap_nand_freepage(chip->oob_poi))
	{
		for (i = 0; i < (mtd->writesize >> 2); i++)
		  *(u_long *)(buf + i * 4) = 0xffffffff;
		return 0;
	}

	col = 0;
	/* Read data main page data and do error correction */
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize)
	{                         
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, col, -1);
		/* Write relative ECC into NFMECCDX */
#if defined(CONFIG_SYS_NAND_MLC)
		nfmeccd0 =	*(chip->oob_poi + eccoffs + i) |
			*(chip->oob_poi + eccoffs + i + 1) << 8 |
			*(chip->oob_poi + eccoffs + i + 2) << 16 |
			*(chip->oob_poi + eccoffs + i + 3) << 24;

		nfmeccd1 =	*(chip->oob_poi + eccoffs + i + 4) |
			*(chip->oob_poi + eccoffs + i + 5) << 8 |
			*(chip->oob_poi + eccoffs + i + 6) << 16 |
			*(chip->oob_poi + eccoffs + i + 7) << 24;

		nfmeccd2 =	*(chip->oob_poi + eccoffs + i + 8);

		writel(nfmeccd0, NFMECCD0);
		writel(nfmeccd1, NFMECCD1);
		writel(nfmeccd2, NFMECCD2);
#else
			/* SLC */
		writel(cpu_to_le32(*(u_long *)(chip->oob_poi + eccoffs + i)), NFMECCD0);
#if defined(CONFIG_SYS_NAND_BUSW16)
		writel(cpu_to_le32(*(u_long *)(chip->oob_poi + eccoffs + i + 4)), NFMECCD1);
#endif
#endif
		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		chip->read_buf(mtd, p, eccsize);
		stat = chip->ecc.correct(mtd, p, NULL, chip->oob_poi + eccoffs + i);

		if (stat == -1)
		  mtd->ecc_stats.failed++;

		col = eccsize * (chip->ecc.steps + 1 - eccsteps);
	}  
	return 0;
}

static void imap_nand_write_page(struct mtd_info *mtd, struct nand_chip * chip,
   const u_char *buf)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int eccoffs  = mtd->oobsize - (eccbytes * eccsteps);
	const u_char * p = buf;

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize)
	{
		chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
		chip->write_buf(mtd, p, eccsize);
		chip->ecc.calculate(mtd, p, chip->oob_poi + eccoffs + i);
	}

	/* Mark this page as dirty
	 * or else, kernel will not read any data in this page */
//	*(u_long *)(chip->oob_poi + nand_dirtmark_loc) = 0;
	*(uint8_t *)(chip->oob_poi + nand_dirtmark_loc + 3) = 0x00;

#if defined(DEBUG_OOB) && !defined(CONFIG_NAND_SPL)
	for (i = 0; i < 128; i++)
	{
		if ( i % 16 == 0) printf("\n");
		printf("%02x ", *(chip->oob_poi + i));
	}
	printf("\n");
#endif
	/* Program the date in OOB buffer to spare area on NAND */
	chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);
}
#endif /* CONFIG_NAND_SPL */

/*
 * Actually the nand_chip structure is allocated in nand.c
 * nand_init_chip function calls this function to do board specific
 * initialization. Bootup sequence is:
 * nand_init() -> nand_init_chip() -> board_nand_init() -> nand_scan()
 */
int board_nand_init(struct nand_chip *chip)
{
	static int chip_n;

	if (chip_n >= ARRAY_SIZE(chip_setting))
	  return -ENODEV;

	imap_nand_hwinit();

	chip->cmd_ctrl		= imap_nand_cmd_ctrl;
	chip->dev_ready		= imap_nand_dev_ready;
	chip->select_chip	= imap_nand_select_chip;
	chip->read_buf		= imap_nand_read_buf;
	chip->write_buf		= imap_nand_write_buf;
	chip->options		= 0;
#if defined(CONFIG_SYS_NAND_BUSW16)
	chip->options	   |= NAND_BUSWIDTH_16;
#endif

	chip->ecc.hwctl		= imap_nand_ecc_hwctl;
	chip->ecc.correct	= imap_nand_ecc_correct;
#ifndef CONFIG_NAND_SPL
	chip->ecc.calculate = imap_nand_ecc_calculate;
	chip->ecc.read_page = imap_nand_read_page;
	chip->ecc.write_page= imap_nand_write_page;
#endif

	chip->ecc.mode		= NAND_ECC_HW;
	chip->ecc.size		= chip_setting[chip_n].eccsize;
	chip->ecc.bytes		= chip_setting[chip_n].eccbytes;

	chip->priv			= chip_setting + chip_n++;

	return 0;
}
