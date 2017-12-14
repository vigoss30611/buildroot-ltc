/*
 * (C) Copyright 2005
 * 2N Telekomunikace, a.s. <www.2n.cz>
 * Ladislav Michl <michl@2n.cz>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _NAND_H_
#define _NAND_H_
#include <cdbg.h>

extern void nand_init(void);
extern void nftl_init(void);

#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>

typedef struct mtd_info nand_info_t;

extern int nand_curr_device;
extern nand_info_t *nand_info[];

static inline int nand_read(nand_info_t *info, loff_t ofs, size_t *len, u_char *buf)
{
	return info->read(info, ofs, *len, (size_t *)len, buf);
}

static inline int nand_write(nand_info_t *info, loff_t ofs, size_t *len, u_char *buf)
{
	return info->write(info, ofs, *len, (size_t *)len, buf);
}

static inline int nand_block_isbad(nand_info_t *info, loff_t ofs)
{
	return info->block_isbad(info, ofs);
}

static inline int nand_erase(nand_info_t *info, loff_t off, size_t size)
{
	struct erase_info instr;

	instr.mtd = info;
	instr.addr = off;
	instr.len = size;
	instr.callback = 0;

	return info->erase(info, &instr);
}


/*****************************************************************************
 * declarations from nand_util.c
 ****************************************************************************/

struct nand_write_options {
	u_char *buffer;		/* memory block containing image to write */
	loff_t length;		/* number of bytes to write */
	loff_t offset;		/* start address in NAND */
	int quiet;		/* don't display progress messages */
	int autoplace;		/* if true use auto oob layout */
	int forcejffs2;		/* force jffs2 oob layout */
	int forceyaffs;		/* force yaffs oob layout */
	int noecc;		/* write without ecc */
	int writeoob;		/* image contains oob data */
	int pad;		/* pad to page size */
	int blockalign;		/* 1|2|4 set multiple of eraseblocks
				 * to align to */
};

typedef struct nand_write_options nand_write_options_t;
typedef struct mtd_oob_ops mtd_oob_ops_t;

struct nand_read_options {
	u_char *buffer;		/* memory block in which read image is written*/
	loff_t length;		/* number of bytes to read */
	loff_t offset;		/* start address in NAND */
	int quiet;		/* don't display progress messages */
	int readoob;		/* put oob data in image */
};

typedef struct nand_read_options nand_read_options_t;

struct nand_erase_options {
	loff_t length;		/* number of bytes to erase */
	loff_t offset;		/* first address in NAND to erase */
	int quiet;		/* don't display progress messages */
	int jffs2;		/* if true: format for jffs2 usage
				 * (write appropriate cleanmarker blocks) */
	int scrub;		/* if true, really clean NAND by erasing
				 * bad blocks (UNSAFE) */
};

typedef struct nand_erase_options nand_erase_options_t;

int nand_read_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,
		       u_char *buffer, unsigned read_with_oob);
int nand_write_skip_bad(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer, unsigned write_with_oob);
int nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts);

int romboot_nand_write(nand_info_t *nand, loff_t offset, size_t *length,
		       u_char *buffer, int protect_flag);
int romboot_nand_read(nand_info_t *nand, loff_t offset, size_t *length,
			u_char *buffer);
extern void aml_nand_stupid_dectect_badblock(struct mtd_info *mtd);

#define NAND_LOCK_STATUS_TIGHT	0x01
#define NAND_LOCK_STATUS_LOCK	0x02
#define NAND_LOCK_STATUS_UNLOCK 0x04

int nand_lock( nand_info_t *meminfo, int tight );
int nand_unlock( nand_info_t *meminfo, ulong start, ulong length );
int nand_get_lock_status(nand_info_t *meminfo, loff_t offset);

#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
void board_nand_select_device(struct nand_chip *nand, int chip);
#endif

/* __attribute__((noreturn))*/ void nand_boot(void);

extern void init_nand(void);

extern int bnd_vs_align(void) ;
extern int bnd_vs_read(uint8_t *buf, loff_t start, int length, int extra);
extern int bnd_vs_write(uint8_t *buf, loff_t start, int length, int extra);
extern int bnd_vs_reset(void);
extern int bnd_vs_erase(loff_t start, uint64_t size);
extern int bnd_vs_isbad(loff_t offset);

extern int nnd_vs_align(void) ;
extern int nnd_vs_read(uint8_t *buf, loff_t start, int length, int extra);
extern int nnd_vs_write(uint8_t *buf, loff_t start, int length, int extra);
extern int nnd_vs_reset(void);
extern int nnd_vs_erase(loff_t start, uint64_t size);
extern int nnd_vs_scrub(loff_t start, uint64_t size);
extern int nnd_vs_isbad(loff_t offset);

extern int fnd_vs_align(void) ;
extern uint32_t fnd_align(void);
extern int fnd_size_match(uint32_t size);
extern int fnd_vs_read(uint8_t *buf, loff_t start, int length, int extra);
extern int fnd_vs_write(uint8_t *buf, loff_t start, int length, int extra);
extern int fnd_vs_reset(void);
extern int fnd_vs_scrub(loff_t start, uint64_t size);
extern int fnd_vs_erase(loff_t start, uint64_t size);
extern int fnd_vs_isbad(loff_t offset);
extern uint32_t fnd_size_shrink(uint32_t size);

#define NAND_CFG_BOOT		0
#define NAND_CFG_NORMAL		1

struct nand_config {
	uint8_t		interface;		/* legacy, toggle, ONFI2 sync */
	uint8_t		randomizer;
	uint8_t		cycle;			/* address cycle */
	uint8_t		mecclvl;
	uint8_t		secclvl;
	uint8_t		eccen;			/* wheather use ECC */
	uint8_t		busw;			/* busw */
	uint8_t		resv;			/* resv */
	/*uint8_t		inited;*/
	uint32_t	badpagemajor;	/* first page offset when checking block validity */
	uint32_t	badpageminor;	/* second page offset when checking block validity */
	uint32_t	sysinfosize;
	uint32_t	pagesize;
	uint32_t	sparesize;
	uint32_t	blocksize;
	uint32_t	pagesperblock;
	uint32_t	blockcount;
	uint32_t	timing;
	uint32_t	rnbtimeout;
	uint32_t	phyread;
	uint32_t	phydelay;
	uint32_t	seed[4];
	uint32_t	polynomial;
	uint16_t	sysinfo_seed[8];
	uint16_t	ecc_seed[8];
	uint16_t	secc_seed[8];
	uint16_t	data_last1K_seed[8];
	uint16_t	ecc_last1K_seed[8];
	uint32_t	data_seed_1k;
	uint32_t	ecc_seed_1k;
	uint32_t	infotm_nand_save;
#if !defined(CONFIG_PRELOADER)
	uint32_t	read_retry;
	uint32_t	retry_level;
	uint32_t	nand_param0;
	uint32_t	nand_param1;
	uint32_t	nand_param2;
#endif
};
#endif
