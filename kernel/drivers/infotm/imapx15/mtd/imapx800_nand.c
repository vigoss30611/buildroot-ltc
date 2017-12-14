/*****************************************************************************
 ** regs-nand.h
 **
 ** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
 **
 ** Use of Infotm's code is governed by terms and conditions
 ** stated in the accompanying licensing statement.
 **
 ** Description:	iMAP NAND Flash platform driver
 ** Layout of the NAND Flash controller registers on iMAPx800.
 **
 ** Author:
 **     xiaojun_yoyo
 **
 ** Revision History:
 ** -----------------
 ** 1.1  02/10/2009  Initialized by warits.
 *****************************************************************************/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/crc32.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <mach/rtcbits.h>
#include <linux/gpio.h>


#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <mach/items.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <mach/mem-reserve.h>
#include <mach/nand.h>
#include <mach/rballoc.h>
#include <mach/imap-rtc.h>

static struct nand_ecclayout imapx800_boot_oob = {
	.oobfree = {
		{.offset = 0,
			.length = 4}}
};

const char *part_cmdline[] = { "cmdlinepart", NULL };

static char *infotm_nand_plane_string[]={
	"NAND_SINGLE_PLANE_MODE",
	"NAND_TWO_PLANE_MODE",
};

static char *infotm_nand_internal_string[]={
	"NAND_NONE_INTERLEAVING_MODE",
	"NAND_INTERLEAVING_MODE",
};

#define ECC_INFORMATION(name_a, ecc_a, size_a, bytes_a, secc_a, unit_size_a, secc_bytes_a, page_manage_bytes_a, max_correct_bits_a) {                \
	.name=name_a, .ecc_mode=ecc_a, .ecc_unit_size=size_a, .ecc_bytes=bytes_a, .secc_mode=secc_a, .secc_unit_size=unit_size_a, .secc_bytes=secc_bytes_a, .page_manage_bytes=page_manage_bytes_a, .max_correct_bits=max_correct_bits_a \
}
static struct infotm_nand_ecc_desc imapx800_ecc_list[] = {
	[0]=ECC_INFORMATION("NAND_ECC_BCH2_MODE", NAND_ECC_BCH2_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH2_ECC_SIZE, NAND_SECC_BCH4_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH4_SECC_SIZE, 4, 2),
	[1]=ECC_INFORMATION("NAND_ECC_BCH4_MODE", NAND_ECC_BCH4_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH4_ECC_SIZE, NAND_SECC_BCH4_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH4_SECC_SIZE, 4, 4),
	[2]=ECC_INFORMATION("NAND_ECC_BCH8_MODE", NAND_ECC_BCH8_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH8_ECC_SIZE, NAND_SECC_BCH4_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH4_SECC_SIZE, 4, 8),
	[3]=ECC_INFORMATION("NAND_ECC_BCH16_MODE", NAND_ECC_BCH16_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH16_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 12),
	[4]=ECC_INFORMATION("NAND_ECC_BCH24_MODE", NAND_ECC_BCH24_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH24_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 18),
	[5]=ECC_INFORMATION("NAND_ECC_BCH32_MODE", NAND_ECC_BCH32_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH32_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 24),
	[6]=ECC_INFORMATION("NAND_ECC_BCH40_MODE", NAND_ECC_BCH40_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH40_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 30),
	[7]=ECC_INFORMATION("NAND_ECC_BCH48_MODE", NAND_ECC_BCH48_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH48_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 38),
	[8]=ECC_INFORMATION("NAND_ECC_BCH56_MODE", NAND_ECC_BCH56_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH56_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 46),
	[9]=ECC_INFORMATION("NAND_ECC_BCH60_MODE", NAND_ECC_BCH60_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH60_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 50),
	[10]=ECC_INFORMATION("NAND_ECC_BCH64_MODE", NAND_ECC_BCH64_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH64_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 52),
	[11]=ECC_INFORMATION("NAND_ECC_BCH72_MODE", NAND_ECC_BCH72_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH72_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 60),
	[12]=ECC_INFORMATION("NAND_ECC_BCH80_MODE", NAND_ECC_BCH80_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH80_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 70),
	[13]=ECC_INFORMATION("NAND_ECC_BCH96_MODE", NAND_ECC_BCH96_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH96_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 84),
	[14]=ECC_INFORMATION("NAND_ECC_BCH112_MODE", NAND_ECC_BCH112_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH112_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 100),
	[15]=ECC_INFORMATION("NAND_ECC_BCH127_MODE", NAND_ECC_BCH127_MODE, NAND_ECC_UNIT_SIZE, NAND_BCH127_ECC_SIZE, NAND_SECC_BCH8_MODE, NAND_SECC_UNIT_SIZE, NAND_BCH8_SECC_SIZE, 4, 112),
};

static inline struct infotm_nand_device *to_nand_dev(struct platform_device *pdev)
{
	return pdev->dev.platform_data;
}

static inline void  trx_afifo_ncmd(struct imapx800_nand_chip *imapx800_chip, int flag, int type, int data)
{
	if (imapx800_chip->vitual_fifo_start) {
		*(uint16_t *)(imapx800_chip->resved_mem + imapx800_chip->vitual_fifo_offset) = ((0x1<<14) | ((flag & 0xf) <<10) | (type<<8) | (data & 0xff));
		imapx800_chip->vitual_fifo_offset += sizeof(uint16_t);
	} else {
		writel(((0x1<<14) | ((flag & 0xf) <<10) | (type<<8) | (data & 0xff)), imapx800_chip->regs + NF2TFPT);
	}
}

static inline void  trx_afifo_nop(struct imapx800_nand_chip *imapx800_chip, int flag, int nop_cnt)
{
	if (imapx800_chip->vitual_fifo_start) {
		*(uint16_t *)(imapx800_chip->resved_mem + imapx800_chip->vitual_fifo_offset) = (((flag & 0xf) <<10) | (nop_cnt & 0x3ff));
		imapx800_chip->vitual_fifo_offset += sizeof(uint16_t);
	} else {
		writel((((flag & 0xf) <<10) | (nop_cnt & 0x3ff)), imapx800_chip->regs + NF2TFPT);
	}
}

static inline void  trx_afifo_manual_cmd(struct imapx800_nand_chip *imapx800_chip, int data)
{
	if (imapx800_chip->vitual_fifo_start) {
		*(uint16_t *)(imapx800_chip->resved_mem + imapx800_chip->vitual_fifo_offset) = data;
		imapx800_chip->vitual_fifo_offset += sizeof(uint16_t);
	} else {
		writel((data & 0xffff), imapx800_chip->regs + NF2TFPT);
	}
}

static inline void trx_afifo_start(struct imapx800_nand_chip *imapx800_chip)
{
	writel(NF_TRXF_START, imapx800_chip->regs + NF2STRR);
}

static inline void trx_dma_start(struct imapx800_nand_chip *imapx800_chip)
{
	writel(NF_DMA_START, imapx800_chip->regs + NF2STRR);
}

static inline void trx_ecc_start(struct imapx800_nand_chip *imapx800_chip)
{
	writel(NF_ECC_START, imapx800_chip->regs + NF2STRR);
}

static inline void trx_engine_start(struct imapx800_nand_chip *imapx800_chip, int type)
{
	writel(type, imapx800_chip->regs + NF2STRR);
}

static inline void trx_afifo_irq_enable(struct imapx800_nand_chip *imapx800_chip, int irq)
{
	writel(irq, imapx800_chip->regs + NF2INTE);
}

static inline void trx_afifo_intr_clear(struct imapx800_nand_chip *imapx800_chip)
{
	writel(0xffffffff, imapx800_chip->regs + NF2INTR);
}

static inline void trx_afifo_inte_clear(struct imapx800_nand_chip *imapx800_chip)
{
	writel(0x0, imapx800_chip->regs + NF2INTE);
}

static inline void trx_afifo_enable(struct imapx800_nand_chip *imapx800_chip)
{
	writel((readl(imapx800_chip->regs + NF2FFCE) | 0x1), imapx800_chip->regs + NF2FFCE);
}

static inline void trx_afifo_disable(struct imapx800_nand_chip *imapx800_chip)
{
	writel(0x0, imapx800_chip->regs + NF2FFCE);
}

static inline int trx_afifo_read_status_reg(struct imapx800_nand_chip *imapx800_chip, int index)
{
	switch(index) {
		case 0:
			return readl(imapx800_chip->regs + NF2STSR0);
		case 1:
			return readl(imapx800_chip->regs + NF2STSR1);
		case 2:
			return readl(imapx800_chip->regs + NF2STSR2);
		case 3:
			return readl(imapx800_chip->regs + NF2STSR3);
	}

	return -1;
}

static void nf2_set_busw(struct imapx800_nand_chip *imapx800_chip, int busw)
{
	return writel((busw == 16)?0x1:0, imapx800_chip->regs + NF2BSWMODE);
}

static void nf2_addr_cfg(struct imapx800_nand_chip *imapx800_chip, int row_addr0, int col_addr, int ecc_offset, int busw)
{
	unsigned int tmp;

	if(busw == 16){
		col_addr = col_addr >> 1;
		ecc_offset = ecc_offset >> 1;
	}

	tmp = (ecc_offset<<16) | col_addr;

	writel(row_addr0, imapx800_chip->regs + NF2RADR0);
	writel(0x0, imapx800_chip->regs + NF2RADR1);
	writel(0x0, imapx800_chip->regs + NF2RADR2);
	writel(0x0, imapx800_chip->regs + NF2RADR3);
	writel(tmp, imapx800_chip->regs + NF2CADR);
}

static void nf2_timing_init(struct imapx800_nand_chip *imapx800_chip, int interface, int timing, int rnbtimeout, int phyread, int phydelay, int busw)
{
	unsigned int pclk_cfg = 0;

	nf2_set_busw(imapx800_chip, busw);

	pclk_cfg = readl(imapx800_chip->regs + NF2PCLKM);
	pclk_cfg &= ~(0x3<<4);
	pclk_cfg |= (0x3<<4); //ecc_clk/4
	writel(pclk_cfg, imapx800_chip->regs + NF2PCLKM);

	if(interface == ASYNC_INTERFACE)//async
	{
		writel(0x0, imapx800_chip->regs + NF2PSYNC);
		pclk_cfg = readl(imapx800_chip->regs + NF2PCLKM);
		pclk_cfg |= (0x1); //sync clock gate
		writel(pclk_cfg, imapx800_chip->regs + NF2PCLKM);
	}
	else if(interface == ONFI_SYNC_INTERFACE)// onfi sync
	{
		writel(0x1, imapx800_chip->regs + NF2PSYNC);
		pclk_cfg = readl(imapx800_chip->regs + NF2PCLKM);
		pclk_cfg &= ~(0x1); //sync clock pass
		writel(pclk_cfg, imapx800_chip->regs + NF2PCLKM);
	}
	else	//toggle
	{
		writel(0x2, imapx800_chip->regs + NF2PSYNC);
		pclk_cfg = readl(imapx800_chip->regs + NF2PCLKM);
		pclk_cfg &= ~(0x1); //sync clock pass
		writel(pclk_cfg, imapx800_chip->regs + NF2PCLKM);
	}

	writel((timing & 0x1fffff), imapx800_chip->regs + NF2AFTM);
	writel(((timing & 0xff000000) >> 24), imapx800_chip->regs + NF2SFTM);
	writel(0xe001, imapx800_chip->regs + NF2STSC);
	//writel(0x0001, imapx800_chip->regs + NF2STSC);
	//writel(0x4023, NF2PRDC);
	writel(phyread, imapx800_chip->regs + NF2PRDC);
	//writel(0xFFFA00, NF2TOUT);
	writel(rnbtimeout, imapx800_chip->regs + NF2TOUT);
	//writel(0x3818, NF2PDLY);
	writel(phydelay, imapx800_chip->regs + NF2PDLY);

	return;
}

static unsigned int nf2_ecc_cap(unsigned int ecc_type)
{
	unsigned int ecc_cap = 0;

	switch (ecc_type) {
		case 0 : {  ecc_cap = 2;} break;		//2bit
		case 1 : {  ecc_cap = 4;} break;		//4bit
		case 2 : {  ecc_cap = 8;} break;		//8bit
		case 3 : {  ecc_cap = 16;} break;		//16bit
		case 4 : {  ecc_cap = 24;} break;		//24bit
		case 5 : {  ecc_cap = 32;} break;		//32bit
		case 6 : {  ecc_cap = 40;} break;		//40bit
		case 7 : {  ecc_cap = 48;} break;		//48bit
		case 8 : {  ecc_cap = 56;} break;		//56bit
		case 9 : {  ecc_cap = 60;} break;		//60bit
		case 10 : {  ecc_cap = 64;} break;		//64bit
		case 11 : {  ecc_cap = 72;} break;		//72bit
		case 12 : {  ecc_cap = 80;} break;		//80bit
		case 13 : {  ecc_cap = 96;} break;		//96bit
		case 14 : {  ecc_cap = 112;} break;		//112bit
		case 15 : {  ecc_cap = 127;} break;		//127bit
		default : break;
	}

	return ecc_cap;
}

static unsigned int nf2_secc_cap(unsigned secc_type)
{
	unsigned int secc_cap = 0;

	switch(secc_type)
	{
		case 0:
			secc_cap = 4;
			break;

		case 1:
			secc_cap = 8;
			break;

		default: break;
	}

	return secc_cap;
}

static inline void nf2_ecc_cfg(struct imapx800_nand_chip *imapx800_chip, int ecc_enable, int page_mode, int ecc_type, int eram_mode,
		int tot_ecc_num, int tot_page_num, int trans_dir, int half_page_en)
{
	unsigned int tmp = 0;
	unsigned int ecc_cap = 0;
	unsigned int val = 0;

	val  = (half_page_en<<18) | (eram_mode <<12) | (page_mode<<11) | (3<<8) | (ecc_type<<4) | (trans_dir<<1) | ecc_enable;
	writel(val, imapx800_chip->regs + NF2ECCC);

	val  = ((tot_ecc_num)<<16) | (tot_page_num);
	writel(val, imapx800_chip->regs + NF2PGEC);

	ecc_cap = nf2_ecc_cap(ecc_type);
	tmp = readl(imapx800_chip->regs + NF2ECCLEVEL);
	tmp &= ~(0x7f<<8);
	tmp |= (ecc_cap << 8);
	writel(tmp, imapx800_chip->regs + NF2ECCLEVEL);
}

static inline void nf2_secc_cfg(struct imapx800_nand_chip *imapx800_chip, int secc_used, int secc_type, int rsvd_ecc_en)
{
	volatile unsigned int tmp;
	unsigned int secc_cap;

	tmp = readl(imapx800_chip->regs + NF2ECCC);
	tmp |= (secc_type <<17) | (secc_used <<16) | (rsvd_ecc_en<<3);
	writel(tmp, imapx800_chip->regs + NF2ECCC);

	secc_cap = nf2_secc_cap(secc_type);
	tmp = readl(imapx800_chip->regs + NF2ECCLEVEL);
	tmp &= ~(0xf<<16);
	tmp |= (secc_cap << 16);
	writel(tmp, imapx800_chip->regs + NF2ECCLEVEL);
}

static inline void nf2_sdma_cfg(struct imapx800_nand_chip *imapx800_chip, unsigned ecc_size, unsigned ch0_adr, int ch0_len, unsigned ch1_adr, int ch1_len, int sdma_2ch_mode)
{
	int dma_bst_len     = ecc_size;
	int blk_number		= 1;
	int adma_mode		= NF_DMA_SDMA;
	int bufbound        = 0;
	int bufbound_chk    = 0;
	int dma_enable      = 1;
	unsigned int value = 0;

	writel(ch0_adr, imapx800_chip->regs + NF2SADR0);
	writel(ch1_adr, imapx800_chip->regs + NF2SADR1);
	writel(((ch1_len << 16) | ch0_len), imapx800_chip->regs + NF2SBLKS);

	value = ((dma_bst_len << 16) | blk_number);
	writel(value, imapx800_chip->regs + NF2SBLKN);

	value = ((bufbound<<4) | (bufbound_chk<<3) | (sdma_2ch_mode<<2) | (adma_mode<<1) | dma_enable);
	writel(value, imapx800_chip->regs + NF2DMAC);

	return;
}

static inline void nf2_randomizer_ops(struct imapx800_nand_chip *imapx800_chip, int enable)
{
	writel(enable, imapx800_chip->regs + NF2RANDME);
}

static inline void nf2_randomizer_seed_init(struct infotm_nand_chip *infotm_chip, struct imapx800_nand_chip *imapx800_chip, int data, int data_ecc, int sysinfo, int sysinfo_ecc)
{
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct nand_chip *chip = &infotm_chip->chip;
	int seed_pattern = 0, blk_plane = 0, pages_per_blk_shift, pages_per_blk_mask;

	pages_per_blk_shift = (chip->phys_erase_shift - chip->page_shift);
	pages_per_blk_mask = ((1 << pages_per_blk_shift) - 1);
	if (strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
		if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
			if (infotm_chip->max_seed_cycle > 1) {
				blk_plane = ((infotm_chip->real_page >> pages_per_blk_shift) % infotm_chip->real_plane);
				if ((blk_plane) && (infotm_chip->real_page & pages_per_blk_mask))
					seed_pattern = infotm_chip->max_seed_cycle - ((infotm_chip->real_page & pages_per_blk_mask) % infotm_chip->max_seed_cycle);
				else
					seed_pattern = ((infotm_chip->real_page & pages_per_blk_mask) % infotm_chip->max_seed_cycle);
			}
		} else {
			seed_pattern = (infotm_chip->real_page % infotm_chip->max_seed_cycle);
		}
	} else {
		seed_pattern = (infotm_chip->real_page % infotm_chip->max_seed_cycle);
		/*if ((infotm_chip->real_page & pages_per_blk_mask) >= (IMAPX800_UBOOT00_SIZE / infotm_chip->uboot0_unit)) {
		  seed_pattern = (((infotm_chip->real_page & pages_per_blk_mask) - (IMAPX800_UBOOT00_SIZE / infotm_chip->uboot0_unit)) % infotm_chip->max_seed_cycle);
		  seed_pattern += 1;
		  }*/
	}

	if (data) {
		writel((((infotm_chip->data_seed[seed_pattern][1] & 0xffff)<<16) | (infotm_chip->data_seed[seed_pattern][0] & 0xffff)) ,(imapx800_chip->regs + NF2DATSEED0));
		writel((((infotm_chip->data_seed[seed_pattern][3] & 0xffff)<<16) | (infotm_chip->data_seed[seed_pattern][2] & 0xffff)) ,(imapx800_chip->regs + NF2DATSEED1));
		writel((((infotm_chip->data_seed[seed_pattern][5] & 0xffff)<<16) | (infotm_chip->data_seed[seed_pattern][4] & 0xffff)) ,(imapx800_chip->regs + NF2DATSEED2));
		writel((((infotm_chip->data_seed[seed_pattern][7] & 0xffff)<<16) | (infotm_chip->data_seed[seed_pattern][6] & 0xffff)) ,(imapx800_chip->regs + NF2DATSEED3));
	}
	if (data_ecc) {
		writel((infotm_chip->ecc_seed[seed_pattern][1]<<16 | infotm_chip->ecc_seed[seed_pattern][0]) ,imapx800_chip->regs + NF2ECCSEED0);
		writel((infotm_chip->ecc_seed[seed_pattern][3]<<16 | infotm_chip->ecc_seed[seed_pattern][2]) ,imapx800_chip->regs + NF2ECCSEED1);
		writel((infotm_chip->ecc_seed[seed_pattern][5]<<16 | infotm_chip->ecc_seed[seed_pattern][4]) ,imapx800_chip->regs + NF2ECCSEED2);
		writel((infotm_chip->ecc_seed[seed_pattern][7]<<16 | infotm_chip->ecc_seed[seed_pattern][6]) ,imapx800_chip->regs + NF2ECCSEED3);
	}
	if ((sysinfo) && (!data)) {
		writel((infotm_chip->sysinfo_seed[seed_pattern][1]<<16 | infotm_chip->sysinfo_seed[seed_pattern][0]) ,imapx800_chip->regs + NF2DATSEED0);
		writel((infotm_chip->sysinfo_seed[seed_pattern][3]<<16 | infotm_chip->sysinfo_seed[seed_pattern][2]) ,imapx800_chip->regs + NF2DATSEED1);
		writel((infotm_chip->sysinfo_seed[seed_pattern][5]<<16 | infotm_chip->sysinfo_seed[seed_pattern][4]) ,imapx800_chip->regs + NF2DATSEED2);
		writel((infotm_chip->sysinfo_seed[seed_pattern][7]<<16 | infotm_chip->sysinfo_seed[seed_pattern][6]) ,imapx800_chip->regs + NF2DATSEED3);
	}
	if ((sysinfo_ecc) && (!data_ecc)) {
		writel((infotm_chip->secc_seed[seed_pattern][1]<<16 | infotm_chip->secc_seed[seed_pattern][0]) ,imapx800_chip->regs + NF2ECCSEED0);
		writel((infotm_chip->secc_seed[seed_pattern][3]<<16 | infotm_chip->secc_seed[seed_pattern][2]) ,imapx800_chip->regs + NF2ECCSEED1);
		writel((infotm_chip->secc_seed[seed_pattern][5]<<16 | infotm_chip->secc_seed[seed_pattern][4]) ,imapx800_chip->regs + NF2ECCSEED2);
		writel((infotm_chip->secc_seed[seed_pattern][7]<<16 | infotm_chip->secc_seed[seed_pattern][6]) ,imapx800_chip->regs + NF2ECCSEED3);
	}
	return;
}

static void nf2_set_polynomial(struct imapx800_nand_chip *imapx800_chip, int polynomial)
{
	writel(polynomial, imapx800_chip->regs + NF2RANDMP);
	return;
}

static unsigned int nf2_randc_seed_hw (struct imapx800_nand_chip *imapx800_chip, unsigned int raw_seed, unsigned int cycle, int polynomial)
{
	unsigned int result;
	int timeout = 0;

	writel(cycle << 16 | raw_seed, imapx800_chip->regs + NF2RAND0);
	writel(0x1, imapx800_chip->regs + NF2RAND1);
	writel(0x0, imapx800_chip->regs + NF2RAND1);

	while (timeout++ < 20) {
		result = readl(imapx800_chip->regs + NF2RAND2);
		if((result & 0x10000) == 0)
			break;
		msleep(1);
	}

	result = readl(imapx800_chip->regs + NF2RAND2) & 0xffff;

	if (timeout >= 20) {
		result = -1;
		printk("nf2_randc_seed_hw timeout\n");
	}
	return result;

}

static int nf2_soft_reset(struct imapx800_nand_chip *imapx800_chip, int num)
{
	volatile unsigned int tmp = 0;
	int timeout = 0;

	writel(num, imapx800_chip->regs + NF2SFTR);
	while (timeout++ < INFOTM_DMA_BUSY_TIMEOUT) {
		tmp = readl(imapx800_chip->regs + NF2SFTR) & (0x1f);
		if(tmp == 0)
			break;
	}

	if (timeout >= INFOTM_DMA_BUSY_TIMEOUT) {
		printk("nf2_soft_reset timeout, tmp = 0x%x, num = %d\n", tmp, num);
		return -1;
	}
	nf2_randomizer_ops(imapx800_chip, NF_RANDOMIZER_DISABLE);

	return 0;
}

static int imapx800_nand_get_item(struct imapx800_nand_chip *imapx800_chip)
{
	char buf[64];
	int ret = 0;
	if (item_exist("nand.cs")) {
		imapx800_chip->ce1_gpio_index = item_integer("nand.cs", 1);
		imapx800_chip->ce2_gpio_index = item_integer("nand.cs", 2);
		imapx800_chip->ce3_gpio_index = item_integer("nand.cs", 3);

		if(imapx800_chip->ce1_gpio_index){
			if (gpio_is_valid(imapx800_chip->ce1_gpio_index)) {
				ret = gpio_request(imapx800_chip->ce1_gpio_index,"ce1");
				if(ret){
					pr_err("failed request gpio for nand ce1\n");
					goto err_ce1;
				}
				gpio_direction_output(imapx800_chip->ce1_gpio_index,1);
			}
			/*  
			imapx_pad_set_mode(1, 1, imapx800_chip->ce1_gpio_index);
			imapx_pad_set_outdat(1, 1, imapx800_chip->ce1_gpio_index);
			imapx_pad_set_dir(0, 1, imapx800_chip->ce1_gpio_index);
			*/
		}

		if (imapx800_chip->ce2_gpio_index) {
			if (gpio_is_valid(imapx800_chip->ce2_gpio_index)) {
				ret = gpio_request(imapx800_chip->ce2_gpio_index,"ce2");
				if(ret){
					pr_err("failed request gpio for nand ce2\n");
					goto err_ce2;
				}
				gpio_direction_output(imapx800_chip->ce2_gpio_index,1);
			}
			/* 
			imapx_pad_set_mode(1, 1, imapx800_chip->ce2_gpio_index);
			imapx_pad_set_outdat(1, 1, imapx800_chip->ce2_gpio_index);
			imapx_pad_set_dir(0, 1, imapx800_chip->ce2_gpio_index);
			*/
		}

		if (imapx800_chip->ce3_gpio_index) {
			if (gpio_is_valid(imapx800_chip->ce3_gpio_index)) {
				ret = gpio_request(imapx800_chip->ce3_gpio_index,"ce3");
				if(ret){
					pr_err("failed request gpio for nand ce3\n");
					goto err_ce3;
				}
				gpio_direction_output(imapx800_chip->ce3_gpio_index,1);
			}
			/* 
			imapx_pad_set_mode(1, 1, imapx800_chip->ce3_gpio_index);
			imapx_pad_set_outdat(1, 1, imapx800_chip->ce3_gpio_index);
			imapx_pad_set_dir(0, 1, imapx800_chip->ce3_gpio_index);
			*/
		}
	}

	if (item_exist("nand.power")){
		item_string(buf, "nand.power", 0);
		if(!strncmp(buf,"pads",4)){
			imapx800_chip->power_gpio_index = item_integer("nand.power", 1);
			if (imapx800_chip->power_gpio_index) {
				if(gpio_is_valid(imapx800_chip->power_gpio_index)){
					ret = gpio_request(imapx800_chip->power_gpio_index,"nand_power");
					if(ret){
						pr_err("failed request gpio for nand power\n");
						goto err_power;
					}
				}
				/*  
				imapx_pad_set_mode(1, 1, imapx800_chip->power_gpio_index);
				imapx_pad_set_outdat(1, 1, imapx800_chip->power_gpio_index);
				imapx_pad_set_dir(0, 1, imapx800_chip->power_gpio_index);
				*/
			}
		}
	}
	printk("imapx800_nand_get_item %d %d %d %d\n", imapx800_chip->ce1_gpio_index, imapx800_chip->ce2_gpio_index, imapx800_chip->ce3_gpio_index, imapx800_chip->power_gpio_index);

	return 0;

err_ce1:
	return ret;
err_ce2:
	if(gpio_is_valid(imapx800_chip->ce1_gpio_index))
		gpio_free(imapx800_chip->ce1_gpio_index);
err_ce3:
	if(gpio_is_valid(imapx800_chip->ce2_gpio_index))
		gpio_free(imapx800_chip->ce2_gpio_index);
err_power:
	if(gpio_is_valid(imapx800_chip->ce3_gpio_index))
		gpio_free(imapx800_chip->ce3_gpio_index);
}

static void imapx800_nand_set_retry_level(struct infotm_nand_chip *infotm_chip, int chipnr, int retry_level)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct infotm_nand_retry_parameter *retry_parameter = (struct infotm_nand_retry_parameter *)infotm_nand_dev->resv1;
	struct infotm_nand_para_save *infotm_nand_save;
	unsigned char *retry_buf;
	int retry_cmd_cnt, i;

	infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	//for (i=0; i<retry_parameter->retry_level; i++)
	//printk("imapx800%d %x \n", i, (unsigned)*(infotm_chip->retry_cmd_list + chipnr * retry_parameter->retry_level + i));
	retry_buf = *(infotm_chip->retry_cmd_list + chipnr * retry_parameter->retry_level + retry_level);
	if ((*(uint32_t *)(retry_buf) == RETRY_PARAMETER_CMD_MAGIC)) {
		retry_cmd_cnt = *(uint16_t *)(retry_buf + sizeof(uint32_t));
		infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
		for (i=0; i<retry_cmd_cnt; i++)
			trx_afifo_manual_cmd(imapx800_chip, *(uint16_t *)(retry_buf + sizeof(uint32_t) + sizeof(uint16_t) + i * sizeof(uint16_t)));
		infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
		infotm_nand_save->retry_level[chipnr] = retry_level;
	}

	return;
}

static int imapx800_nand_get_eslc_para(struct infotm_nand_chip *infotm_chip)
{
	struct nand_chip *chip = &infotm_chip->chip;
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_eslc_parameter *eslc_parameter;
	int error = 0, i;
	uint32_t pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));

	if ((infotm_nand_dev == NULL) || (infotm_nand_dev->resv3 == 0))
		return 0;

	eslc_parameter = &infotm_chip->eslc_parameter[0];
	if (infotm_nand_save->eslc_para_magic == IMAPX800_ESLC_PARA_MAGIC) {
		memcpy(eslc_parameter, (struct infotm_nand_eslc_parameter *)infotm_nand_dev->resv3, sizeof(struct infotm_nand_eslc_parameter));
		if (eslc_parameter->parameter_set_mode == START_END) {
			for (i=0; i<eslc_parameter->register_num; i++) {
				eslc_parameter->parameter_offset[0][i] = infotm_nand_save->eslc_para[0][i];
				eslc_parameter->parameter_offset[1][i] = infotm_nand_save->eslc_para[1][i];
				printk("imapx para[%02d] %02x %02x\n", i, eslc_parameter->parameter_offset[0][i], eslc_parameter->parameter_offset[1][i]);
			}
			for(i=0; i<(pages_per_blk>>1); i++) {
				if(i < 2)
					eslc_parameter->paired_page_addr[i][0] = i;
				else
					eslc_parameter->paired_page_addr[i][0] = 2 * i - ((i % 2)? 3 : 2);
			}
		} else if (eslc_parameter->parameter_set_mode == START_ONLY) {
			for (i=0; i<eslc_parameter->register_num; i++) {
				eslc_parameter->parameter_offset[0][i] = infotm_nand_save->eslc_para[0][i];
				printk("imapx para[%02d] %02x\n", i, eslc_parameter->parameter_offset[0][i]);
			}

			for(i=0; i<(pages_per_blk>>1); i++) {
				if(i < 2)
					eslc_parameter->paired_page_addr[i][0] = i;
				else
					eslc_parameter->paired_page_addr[i][0] = 2 * i - ((i % 2)? 3 : 2);
			}
		}
	} else {
		infotm_nand_save->eslc_para_magic = IMAPX800_ESLC_PARA_MAGIC;
		for (i=0; i<eslc_parameter->register_num; i++) {
			infotm_nand_save->eslc_para[0][i] = eslc_parameter->parameter_offset[0][i];
			infotm_nand_save->eslc_para[1][i] = eslc_parameter->parameter_offset[1][i];
			printk("imapx para[%02d] %02x %02x\n", i, eslc_parameter->parameter_offset[0][i], eslc_parameter->parameter_offset[1][i]);
		}
	}

	return error;

}

static int imapx800_nand_get_retry_table(struct infotm_nand_chip *infotm_chip)
{
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct infotm_nand_retry_parameter *retry_parameter = (struct infotm_nand_retry_parameter *)infotm_nand_dev->resv1;
	unsigned char *data_buf;
	int error = 0, i = 0, k, buf_offset = 0, fifo_cnt_offset = 0;

	if ((infotm_nand_dev == NULL) || (infotm_nand_dev->resv1 == 0))
		return 0;

	if (!(infotm_chip->ops_mode & INFOTM_RETRY_MODE))
		return 0;

	infotm_chip->retry_cmd_list = (void **)kzalloc((sizeof(void*) * retry_parameter->retry_level * infotm_chip->chip_num), GFP_KERNEL);
	if (infotm_chip->retry_cmd_list == NULL)
		return -ENOMEM;

	infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	infotm_nand_save->retry_level_support = retry_parameter->retry_level - 1;
	if (infotm_nand_save->retry_para_magic == IMAPX800_RETRY_PARA_MAGIC) {
		infotm_chip->infotm_nand_set_retry_level = imapx800_nand_set_retry_level;

		data_buf = imapx800_chip->resved_mem + sizeof(struct infotm_nand_para_save);
		for (k=0; k<infotm_chip->chip_num; k++) {
			if (infotm_chip->valid_chip[k]) {
				retry_parameter = &infotm_chip->retry_parameter[k];
				memcpy(retry_parameter, (struct infotm_nand_retry_parameter *)infotm_nand_dev->resv1, sizeof(struct infotm_nand_retry_parameter));
				infotm_chip->retry_level_support = retry_parameter->retry_level - 1;
				for (i=0; i<retry_parameter->retry_level; i++) {
					do {
						if ((*(uint32_t *)(data_buf + buf_offset)) == RETRY_PARAMETER_CMD_MAGIC) {
							*(infotm_chip->retry_cmd_list + k * retry_parameter->retry_level + i) = (data_buf + buf_offset);
							//printk("level %d offs %d %d \n", i, buf_offset, sizeof(struct infotm_nand_para_save));
							buf_offset += sizeof(uint32_t);
							break;
						}
						buf_offset += sizeof(uint32_t);
					} while (buf_offset < (IMAPX800_PARA_SAVE_SIZE - sizeof(struct infotm_nand_para_save)));
					if (buf_offset >= (IMAPX800_PARA_SAVE_SIZE - sizeof(struct infotm_nand_para_save)))
						break;
				}
				infotm_chip->retry_level[k] = infotm_nand_save->retry_level[k];
			}
		}
		list_for_each_safe(l, n, &imapx800_chip->chip_list) {
			infotm_chip_temp = chip_list_to_imapx800(l);
			if ((infotm_chip->mfr_type == NAND_MFR_HYNIX) && (infotm_chip_temp != infotm_chip) && (infotm_chip_temp->retry_level_support > 0)) {
				for (k=0; k<infotm_chip->chip_num; k++) {
					if (infotm_chip->valid_chip[k])
						infotm_chip->retry_level[k] = infotm_chip_temp->retry_level[k];
				}
				break;
			}
		}
	} else {
		infotm_nand_save->retry_para_magic = IMAPX800_RETRY_PARA_MAGIC;
		imapx800_chip->vitual_fifo_offset = sizeof(struct infotm_nand_para_save);
		imapx800_chip->vitual_fifo_start = 1;
		memset(imapx800_chip->resved_mem + imapx800_chip->vitual_fifo_offset, 0, IMAPX800_PARA_SAVE_SIZE - imapx800_chip->vitual_fifo_offset);
		for (k=0; k<infotm_chip->chip_num; k++) {
			if (infotm_chip->valid_chip[k]) {
				retry_parameter = &infotm_chip->retry_parameter[k];
				for (i=0; i<retry_parameter->retry_level; i++) {
					if (imapx800_chip->vitual_fifo_offset & (sizeof(uint32_t) - 1))
						imapx800_chip->vitual_fifo_offset += (sizeof(uint32_t) - (imapx800_chip->vitual_fifo_offset & (sizeof(uint32_t) - 1)));
					*(uint32_t *)(imapx800_chip->resved_mem + imapx800_chip->vitual_fifo_offset) = RETRY_PARAMETER_CMD_MAGIC;
					*(infotm_chip->retry_cmd_list + k * retry_parameter->retry_level + i) = (imapx800_chip->resved_mem + imapx800_chip->vitual_fifo_offset);
					imapx800_chip->vitual_fifo_offset += sizeof(uint32_t);
					fifo_cnt_offset = imapx800_chip->vitual_fifo_offset;
					imapx800_chip->vitual_fifo_offset += sizeof(uint16_t);
					infotm_chip->infotm_nand_set_retry_level(infotm_chip, k, i);
					*(uint16_t *)(imapx800_chip->resved_mem + fifo_cnt_offset) = (imapx800_chip->vitual_fifo_offset - fifo_cnt_offset - sizeof(uint16_t)) / sizeof(uint16_t);
				}
				if (k == 0)
					infotm_nand_save->chip0_para_size = imapx800_chip->vitual_fifo_offset;
			}
		}
		imapx800_chip->vitual_fifo_start = 0;
		if (infotm_chip->mfr_type == NAND_MFR_HYNIX)
			infotm_nand_save->retry_level_circulate = 1;
		infotm_chip->infotm_nand_set_retry_level = imapx800_nand_set_retry_level;
	}

	return error;
}

static void imapx800_nand_save_para(struct infotm_nand_chip *infotm_chip)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	int page_addr, pages_per_blk, para_reserved_blks, blk_num, status;
	unsigned char *oob_buffer = chip->oob_poi;
	uint16_t seed_save[4][8];
	int i, j, busw;

	pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));
	para_reserved_blks = infotm_chip->reserved_pages / pages_per_blk;
	*(uint16_t *)oob_buffer = IMAPX800_VERSION_MAGIC;
	*(uint16_t *)(oob_buffer + 2) = IMAPX800_UBOOT_MAGIC;
	for (blk_num=0; blk_num<para_reserved_blks; blk_num++) {

		if (((blk_num * pages_per_blk) & (IMAPX800_BOOT_PAGES_PER_COPY - 1)) == 0)
			continue;
		infotm_chip->infotm_nand_select_chip(infotm_chip, 0);
		infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE1, -1, blk_num * pages_per_blk, 0);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE2, -1, -1, 0);
		status = chip->waitfunc(mtd, chip);
		if (status & NAND_STATUS_FAIL)
			continue;

		if(rbgetint("irom_new") != IMAPX15_NEW_IROM)
		{
		busw = (imapx800_chip->busw == 16) ? 1 : 0;
		i = infotm_chip->max_seed_cycle;
		infotm_chip->max_seed_cycle = 1;
		for (j=0; j<8; j++) {
			seed_save[0][j] = infotm_chip->data_seed[0][j];
			seed_save[1][j] = infotm_chip->sysinfo_seed[0][j];
			seed_save[2][j] = infotm_chip->ecc_seed[0][j];
		}
		for (j=0; j<8; j++) {
			infotm_chip->data_seed[0][j] = uboot0_default_seed[j];
			infotm_chip->sysinfo_seed[0][j] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->data_seed[0][j] & 0x7fff, ((infotm_chip->uboot0_unit >> busw) - 1), infotm_chip->infotm_nand_dev->polynomial);
			infotm_chip->ecc_seed[0][j] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->sysinfo_seed[0][j], (((PAGE_MANAGE_SIZE) >> busw) - 1), infotm_chip->infotm_nand_dev->polynomial);
		}
		}

		for (page_addr=0; page_addr<IMAPX800_PARA_ONESEED_SAVE_PAGES; page_addr++) {
			chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page_addr + blk_num * pages_per_blk);
			chip->ecc.write_page(mtd, chip, (const uint8_t *)infotm_nand_save);
			status = chip->waitfunc(mtd, chip);
			if (status & NAND_STATUS_FAIL)
				printk("infotm nand para write failed at %d \n", page_addr);
		}
		infotm_chip->infotm_nand_select_chip(infotm_chip, -1);

		if(rbgetint("irom_new") != IMAPX15_NEW_IROM)
		{
			infotm_chip->max_seed_cycle = i;
			for (j=0; j<8; j++) {
				infotm_chip->data_seed[0][j] = seed_save[0][j];
				infotm_chip->sysinfo_seed[0][j] = seed_save[1][j];
				infotm_chip->ecc_seed[0][j] = seed_save[2][j];
			}
		}
	}
	return;
}

static void imapx800_nand_get_save_para(struct infotm_nand_chip *infotm_chip)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_platform *plat;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	int i, j;

	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip_temp = chip_list_to_imapx800(l);
		plat = infotm_chip_temp->platform;
		if (!strncmp((char*)plat->name, NAND_NORMAL_NAME, strlen((const char*)NAND_NORMAL_NAME))) {
			infotm_nand_save->head_magic = IMAPX800_UBOOT_PARA_MAGIC;
			infotm_nand_save->chip0_para_size = sizeof(struct infotm_nand_para_save);
			infotm_nand_save->boot_copy_num = 1;
			infotm_nand_save->mfr_type = infotm_chip_temp->mfr_type;
			infotm_nand_save->valid_chip_num = (mtd->writesize >> chip->page_shift) / infotm_chip->plane_num;
			infotm_nand_save->page_shift = chip->page_shift;
			infotm_nand_save->erase_shift = chip->phys_erase_shift;
			infotm_nand_save->pages_per_blk_shift = 0;//(chip->phys_erase_shift - chip->page_shift);
			infotm_nand_save->oob_size = mtd->oobsize;
			infotm_nand_save->ecc_mode = infotm_chip_temp->ecc_mode;
			infotm_nand_save->secc_mode = infotm_chip_temp->secc_mode;
			infotm_nand_save->ecc_bytes = chip->ecc.bytes;
			infotm_nand_save->secc_bytes = infotm_chip_temp->secc_bytes;
			infotm_nand_save->ecc_bits = nf2_ecc_cap(infotm_nand_save->ecc_mode);
			infotm_nand_save->secc_bits = nf2_secc_cap(infotm_nand_save->secc_mode);
			infotm_nand_save->seed_cycle = infotm_chip_temp->max_seed_cycle;
			for (j=0; j<infotm_nand_save->seed_cycle; j++) {
				for(i = 0; i < 8; i++) {
					infotm_nand_save->data_seed[j][i] = infotm_chip_temp->data_seed[j][i];
					infotm_nand_save->ecc_seed[j][i] = infotm_chip_temp->ecc_seed[j][i];
				}
			}
			if (infotm_nand_save->retry_para_magic != IMAPX800_RETRY_PARA_MAGIC)
				imapx800_nand_get_retry_table(infotm_chip);
		} else if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
			if (infotm_chip_temp->ops_mode & INFOTM_ESLC_MODE) {
				if (infotm_nand_save->eslc_para_magic != IMAPX800_ESLC_PARA_MAGIC)
					imapx800_nand_get_eslc_para(infotm_chip_temp);
			}
		}
	}
	return;
}

#if 0
static int imapx800_nand_check_uboot_item(struct infotm_nand_chip *infotm_chip)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	int i;

	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip_temp = chip_list_to_imapx800(l);
		infotm_chip_temp->uboot0_unit = infotm_nand_save->uboot0_unit;
		for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++) {
			infotm_chip_temp->uboot0_blk[i] = infotm_nand_save->uboot0_blk[i];
			infotm_chip_temp->uboot1_blk[i] = infotm_nand_save->uboot1_blk[i];
			infotm_chip_temp->item_blk[i] = infotm_nand_save->item_blk[i];
		}
	}

	return 0;
}
#endif

static int imapx800_nand_hw_init(struct infotm_nand_chip *infotm_chip)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct infotm_nand_para_save *infotm_nand_save;
	unsigned char *reserved_buf;

	infotm_chip->uboot0_size = IMAPX800_UBOOT0_SIZE;
	infotm_chip->reserved_pages = IMAPX800_PARA_SAVE_PAGES;
	plat->infotm_nand_dev = 0;
	if (imapx800_chip->hw_inited) {
		infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
		infotm_chip->uboot0_unit = infotm_nand_save->uboot0_unit;
		if (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC) {
			infotm_chip->infotm_nand_get_retry_table = imapx800_nand_get_retry_table;
			//infotm_chip->infotm_nand_check_uboot_item = imapx800_nand_check_uboot_item;
		}
		/*plat->infotm_nand_dev = (struct cdbg_cfg_nand *)(imapx800_chip->resved_mem + IMAPX800_PARA_SAVE_SIZE);
		if (plat->infotm_nand_dev->resv1)
			plat->infotm_nand_dev->resv1 = (unsigned)((unsigned char *)plat->infotm_nand_dev + sizeof(struct cdbg_cfg_nand));
		if (plat->infotm_nand_dev->resv2)
			plat->infotm_nand_dev->resv2 = (unsigned)((unsigned char *)plat->infotm_nand_dev + sizeof(struct cdbg_cfg_nand) + sizeof(struct infotm_nand_retry_parameter) * MAX_CHIP_NUM);
		if (plat->infotm_nand_dev->resv3)
			plat->infotm_nand_dev->resv3 = (unsigned)((unsigned char *)plat->infotm_nand_dev + sizeof(struct cdbg_cfg_nand) + sizeof(struct infotm_nand_retry_parameter) * MAX_CHIP_NUM + sizeof(struct infotm_nand_extend_para));*/
		if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME)))
			imapx800_nand_get_item(imapx800_chip);
		return 0;
	}

	module_power_on(SYSMGR_NAND_BASE);
	module_reset(SYSMGR_NAND_BASE, 0);

	if(imapx_pad_init("nand") != 0){
		printk("nand io config error\n");
		return -ENOENT;
	}

	imapx800_chip->sys_clk = clk_get_sys("imap-nandflash", "imap-nand");
	if (IS_ERR(imapx800_chip->sys_clk)) {
		printk("iMAP NAND: Failed to get bus clock.\n");
		return -ENOENT;
	}

	clk_prepare_enable(imapx800_chip->sys_clk);
	imapx800_chip->ecc_clk = clk_get_sys("imap-nand-ecc", "imap-nand");
	if(IS_ERR(imapx800_chip->ecc_clk)) {
		printk("iMAP NAND: Failed to get ecc clock.\n");
		return -ENOENT;
	}
	clk_prepare_enable(imapx800_chip->ecc_clk);

	imapx800_chip->sys_rtc_regs = ioremap(SYSMGR_RTC_BASE, 0x1000);
	reserved_buf = rbget(RESERVED_NAND_RRTB_NAME);
	if (reserved_buf == NULL) {
        printk("=====> imapx800_nand get reserved memory(%s) failed \n", RESERVED_NAND_RRTB_NAME);
		return -ENOMEM;
    }
	memcpy(imapx800_chip->resved_mem, reserved_buf, RESERVED_RRTB_SIZE);
	memset(reserved_buf, 0x0, RESERVED_RRTB_SIZE);
	//imapx800_chip->resved_mem = reserved_buf;
	infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	infotm_chip->uboot0_unit = infotm_nand_save->uboot0_unit;
	if (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC) {
		infotm_chip->infotm_nand_get_retry_table = imapx800_nand_get_retry_table;
		//infotm_chip->infotm_nand_check_uboot_item = imapx800_nand_check_uboot_item;
		if (infotm_nand_save->eslc_para_magic == IMAPX800_ESLC_PARA_MAGIC)
			infotm_chip->infotm_nand_get_eslc_para = imapx800_nand_get_eslc_para;
		//for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++)
			//infotm_chip->uboot0_blk[i] = infotm_nand_save->uboot0_blk[i];
	} //else {
		//imapx800_chip->drv_ver_changed = 1;
		//for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++)
			//infotm_chip->uboot0_blk[i] = 0;
	//}
	/*plat->infotm_nand_dev = (struct cdbg_cfg_nand *)(imapx800_chip->resved_mem + IMAPX800_PARA_SAVE_SIZE);
	if (plat->infotm_nand_dev->resv1)
		plat->infotm_nand_dev->resv1 = (unsigned)((unsigned char *)plat->infotm_nand_dev + sizeof(struct cdbg_cfg_nand));
	if (plat->infotm_nand_dev->resv2)
		plat->infotm_nand_dev->resv2 = (unsigned)((unsigned char *)plat->infotm_nand_dev + sizeof(struct cdbg_cfg_nand) + sizeof(struct infotm_nand_retry_parameter) * MAX_CHIP_NUM);
	if (plat->infotm_nand_dev->resv3)
		plat->infotm_nand_dev->resv3 = (unsigned)((unsigned char *)plat->infotm_nand_dev + sizeof(struct cdbg_cfg_nand) + sizeof(struct infotm_nand_retry_parameter) * MAX_CHIP_NUM + sizeof(struct infotm_nand_extend_para));*/
	//nf2_timing_init(imapx800_chip, ASYNC_INTERFACE, DEFAULT_TIMING, DEFAULT_RNB_TIMEOUT, DEFAULT_PHY_READ, DEFAULT_PHY_DELAY, BUSW_8BIT);
	if (nf2_soft_reset(imapx800_chip, NF_RESET)) {
		printk("nf2_soft_reset error\n");
		return -1;
	}
	imapx800_chip->hw_inited = 1;
	trx_afifo_intr_clear(imapx800_chip);
	if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME)))
		imapx800_nand_get_item(imapx800_chip);

	printk("infotm_chip->clk = %ld \n", clk_get_rate(imapx800_chip->ecc_clk));
	return 0;
}

static void imapx800_nand_adjust_timing(struct infotm_nand_chip *infotm_chip)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_platform *plat = infotm_chip->platform;
	int interface, timing = 0, rnbtimeout, phyread = 0, phydelay = 0, busw = 0, sys_cycle, temp;

	if (strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME)))
		return;
	if (!infotm_nand_dev->timing[NF_tREA])
		infotm_nand_dev->timing[NF_tREA] = 20;
	if (!infotm_nand_dev->timing[NF_tWP])
		infotm_nand_dev->timing[NF_tWP] = 15;
	if (!infotm_nand_dev->timing[NF_tWH])
		infotm_nand_dev->timing[NF_tWH] = 15;
	if (!infotm_nand_dev->timing[NF_tREH])
		infotm_nand_dev->timing[NF_tREH] = 15;
	if (!infotm_nand_dev->timing[NF_tCLS])
		infotm_nand_dev->timing[NF_tCLS] = 25;
	if (!infotm_nand_dev->timing[NF_tCLH])
		infotm_nand_dev->timing[NF_tCLH] = 15;

	interface = CDBG_NF_INTERFACE(infotm_nand_dev->parameter);
	sys_cycle = clk_get_rate(imapx800_chip->ecc_clk);
	sys_cycle /= 1000000;
	sys_cycle = 10*1000 / sys_cycle;

	temp = infotm_nand_dev->timing[NF_tCLS] - infotm_nand_dev->timing[NF_tWP];
	temp = ((temp * 100) / sys_cycle);
	if (temp % 10)
		temp = temp / 10 + 1;
	else
		temp = temp / 10;
	if (temp <= 0)
		temp = 1;
	timing |= (temp << 16);

	temp = (((infotm_nand_dev->timing[NF_tREA] + IMAPX800_NF_DELAY) * 100) / sys_cycle);
	if (temp % 10)
		temp = temp / 10 + 1;
	else
		temp = temp / 10;
	if (temp <= 0)
		temp = 1;
	timing |= (temp << 12);

	temp = ((infotm_nand_dev->timing[NF_tWP] * 100) / sys_cycle);
	if (temp % 10)
		temp = temp / 10 + 1;
	else
		temp = temp / 10;
	if (temp <= 0)
		temp = 1;
	timing |= ((temp - 1) << 8);

	temp = ((infotm_nand_dev->timing[NF_tCLH] * 100) / sys_cycle);
	if (temp % 10)
		temp = temp / 10 + 1;
	else
		temp = temp / 10;
	if (temp <= 0)
		temp = 1;
	timing |= (temp << 4);

	temp = ((max(infotm_nand_dev->timing[NF_tWH], infotm_nand_dev->timing[NF_tREH])* 100) / sys_cycle);
	if (temp % 10)
		temp = temp / 10 + 1;
	else
		temp = temp / 10;
	if (temp <= 0)
		temp = 1;
	timing |= (temp - 1);

	busw = imapx800_chip->busw;
	if (interface) {
		phyread = DEFAULT_PHY_READ;
		//phyread = nand_flash_dev->phydelay;
	} else {
		phyread = DEFAULT_PHY_READ;
		phydelay = DEFAULT_PHY_DELAY2;
		rnbtimeout = DEFAULT_RNB_TIMEOUT;
	}
	printk("adjust timing %x %x %x %x %x %x \n", interface, timing, rnbtimeout, phyread, phydelay, busw);

	nf2_timing_init(imapx800_chip, interface, timing, rnbtimeout, phyread, phydelay, busw);
	nf2_soft_reset(imapx800_chip, NF_RESET);
	infotm_nand_save->timing = timing;
	return;
}

static int imapx800_nand_options_confirm(struct infotm_nand_chip *infotm_chip)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct infotm_nand_extend_para *infotm_nand_para = (struct infotm_nand_extend_para *)infotm_nand_dev->resv2;
	//struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct infotm_nand_ecc_desc *ecc_supports = infotm_chip->ecc_desc;
	unsigned max_ecc_mode = infotm_chip->max_ecc_mode;
	unsigned options_selected = 0, options_define;
	int error = 0, i, j, busw = 0;

	options_selected = (plat->platform_nand_data.chip.options & NAND_ECC_OPTIONS_MASK);
	options_define = (infotm_chip->options & NAND_ECC_OPTIONS_MASK);

	for (i=0; i<max_ecc_mode; i++) {
		if (ecc_supports[i].ecc_mode == options_selected) {
			break;
		}
	}
	j = i;

	/*for(i=max_ecc_mode-1; i>0; i--) {
		ecc_bytes = (infotm_chip->page_size / ecc_supports[i].ecc_unit_size);
		if (infotm_chip->oob_size >= (ecc_supports[i].ecc_bytes * ecc_bytes + ecc_supports[i].secc_unit_size + ecc_supports[i].secc_bytes + ecc_supports[i].page_manage_bytes)) {
			options_support = ecc_supports[i].ecc_mode;
			break;
		}
	}

	if (options_define != options_support) {
		options_define = options_support;
		printk("define oob size: %d could support ecc mode: %s\n", infotm_chip->oob_size, ecc_supports[options_support].name);
	}*/

	if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
		infotm_chip->short_mode = 1;
		mtd->writesize = infotm_chip->uboot0_unit;
		mtd->erasesize = (infotm_chip->uboot0_unit << (chip->phys_erase_shift - chip->page_shift)) ;
		mtd->writebufsize = mtd->writesize;
		chip->page_shift = ffs(mtd->writesize) - 1;
		chip->bbt_erase_shift = chip->phys_erase_shift = ffs(mtd->erasesize) - 1;
		chip->subpagesize = mtd->writesize >> mtd->subpage_sft;
		infotm_chip->page_size = mtd->writesize;
		if(rbgetint("irom_new") == IMAPX15_NEW_IROM && options_selected == NAND_ECC_BCH127_MODE){
			options_selected = NAND_ECC_BCH80_MODE;
			infotm_chip->half_page_en = 1;
		}
	} else {
		options_selected = options_define;
	}

	for (i=0; i<infotm_chip->max_ecc_mode; i++) {
		if (ecc_supports[i].ecc_mode == options_selected) {
			break;
		}
	}

	if (i < max_ecc_mode) {
		chip->ecc.mode = NAND_ECC_HW;
		chip->ecc.size = NAND_ECC_UNIT_SIZE;
		chip->ecc.bytes = ecc_supports[i].ecc_bytes;
		infotm_chip->ecc_mode = ecc_supports[i].ecc_mode;
		if (infotm_chip->short_mode) {
			if(rbgetint("irom_new") == IMAPX15_NEW_IROM)
			{
				infotm_chip->secc_mode = -1;
				infotm_chip->secc_bytes = 0;
			} else {
				infotm_chip->secc_mode = NAND_SECC_BCH4_MODE;
				infotm_chip->secc_bytes = NAND_BCH8_SECC_SIZE;
			}
		} else {
			infotm_chip->secc_mode = ecc_supports[i].secc_mode;
			infotm_chip->secc_bytes = ecc_supports[i].secc_bytes;
		}
		infotm_chip->max_correct_bits = ecc_supports[i].max_correct_bits;
	} else {
		printk("soft ecc or none ecc just support in linux self nand base please selected it at platform options\n");
		error = -ENXIO;
	}

	options_selected = (plat->platform_nand_data.chip.options & NAND_PLANE_OPTIONS_MASK);
	options_define = (infotm_chip->options & NAND_PLANE_OPTIONS_MASK);
	if (options_selected > options_define) {
		printk("multi plane error for selected plane mode: %s force plane to : %s\n", infotm_nand_plane_string[options_selected >> 4], infotm_nand_plane_string[options_define >> 4]);
		options_selected = options_define;
	}

	switch (options_selected) {

		case NAND_TWO_PLANE_MODE:
			infotm_chip->plane_num = 2;
			infotm_chip->real_plane = 2;
			mtd->erasesize *= 2;
			mtd->writesize *= 2;
			mtd->writebufsize *= 2;
			mtd->oobsize *= 2;
			break;

		default:
			infotm_chip->plane_num = 1;
			infotm_chip->real_plane = 1;
			break;
	}

	options_selected = (plat->platform_nand_data.chip.options & NAND_INTERLEAVING_OPTIONS_MASK);
	options_define = (infotm_chip->options & NAND_INTERLEAVING_OPTIONS_MASK);
	if (options_selected > options_define) {
		printk("internal mode error for selected internal mode: %s force internal mode to : %s\n", infotm_nand_internal_string[options_selected >> 16], infotm_nand_internal_string[options_define >> 16]);
		options_selected = options_define;
	}

	switch (options_selected) {

		case NAND_INTERLEAVING_MODE:
			infotm_chip->ops_mode |= INFOTM_INTERLEAVING_MODE;
			mtd->erasesize *= infotm_chip->internal_chipnr;
			mtd->writesize *= infotm_chip->internal_chipnr;
			mtd->writebufsize *= infotm_chip->internal_chipnr;
			mtd->oobsize *= infotm_chip->internal_chipnr;
			break;

		default:
			break;
	}

	options_selected = (plat->platform_nand_data.chip.options & NAND_BUSW_OPTIONS_MASK);
	options_define = (infotm_chip->options & NAND_BUSW_OPTIONS_MASK);
	if (options_selected > options_define) {
		options_selected = options_define;
	}

	switch (options_selected) {

		case NAND_BUS16_MODE:
			busw = 1;
			imapx800_chip->busw = 16;
			break;

		default:
			imapx800_chip->busw = (chip->options & NAND_BUSWIDTH_16) ? 16 : 8;
			busw = (chip->options & NAND_BUSWIDTH_16) ? 1 : 0;
			break;
	}

	options_selected = (plat->platform_nand_data.chip.options & NAND_RANDOMIZER_OPTIONS_MASK);
	options_define = (infotm_chip->options & NAND_RANDOMIZER_OPTIONS_MASK);
	if (options_selected > options_define) {
		options_selected = options_define;
	}

	switch (options_selected) {

		case NAND_RANDOMIZER_MODE:
			infotm_chip->ops_mode |= INFOTM_RANDOMIZER_MODE;
			infotm_chip->max_seed_cycle = 1;
			nf2_set_polynomial(imapx800_chip, infotm_nand_dev->polynomial);

			if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
				if (infotm_nand_para != NULL)
					infotm_chip->uboot0_rewrite_count = infotm_nand_para->uboot0_rewrite_count;
#if 0
				ecfg = ecfg_paras();
				for(i = 0; i < 8; i++) {
					if(ecfg_check_flag(ECFG_DEFAULT_RANDOMIZER)) {
						infotm_chip->data_seed[0][i] = uboot0_default_seed[i];
					} else {
						infotm_chip->data_seed[0][i] = ((ecfg->seed[i/2] >> ((i % 2) * 16)) & 0xffff);
						imapx800_chip->polynomial = ecfg->polynomial;
					}
					//infotm_nand_save->uboot0_data_seed[i] = infotm_chip->data_seed[0][i];
					infotm_chip->sysinfo_seed[0][i] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->data_seed[0][i] & 0x7fff, ((infotm_chip->uboot0_unit>>busw) - 1), infotm_nand_dev->polynomial);
					//infotm_nand_save->uboot0_sysinfo_seed[i] = infotm_chip->sysinfo_seed[0][i];
					infotm_chip->ecc_seed[0][i] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->sysinfo_seed[0][i], (((PAGE_MANAGE_SIZE)>>busw) - 1), infotm_nand_dev->polynomial);
					//infotm_nand_save->uboot0_ecc_seed[i] = infotm_chip->ecc_seed[0][i];
					infotm_chip->secc_seed[0][i] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->ecc_seed[0][i], (chip->ecc.bytes*(infotm_chip->uboot0_unit/chip->ecc.size) - 1), infotm_nand_dev->polynomial);
					//infotm_nand_save->uboot0_secc_seed[i] = infotm_chip->secc_seed[0][i];
				}
#else
				infotm_chip->max_seed_cycle = 256;
				if (busw) {
					for (j=0; j<infotm_chip->max_seed_cycle; j++) {
						for(i = 0; i < 8; i++) {
							infotm_chip->data_seed[j][i] = *(unsigned *)((unsigned *)imapx800_seed + j * 8 + i);
							infotm_chip->ecc_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_ecc_seed_1k_16bit + j * 8 + i);
						}
					}
				}else {
					for (j=0; j<infotm_chip->max_seed_cycle; j++) {
						for(i = 0; i < 8; i++) {
							infotm_chip->data_seed[j][i] = *(unsigned *)((unsigned *)imapx800_seed + j * 8 + i);
							infotm_chip->sysinfo_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_sysinfo_seed_1k + j * 8 + i);
							infotm_chip->ecc_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_ecc_seed_1k + j * 8 + i);
							infotm_chip->secc_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_secc_seed_1k + j * 8 + i);
						}
					}
				}
#endif
			} else {
				for(i = 0; i < 8; i++) {
					infotm_chip->data_seed[0][i] = ((infotm_nand_dev->seed[i/2] >> ((i % 2) * 16)) & 0xffff);
					infotm_chip->sysinfo_seed[0][i] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->data_seed[0][i] & 0x7fff, (((1 << chip->page_shift)>>busw) - 1), infotm_nand_dev->polynomial);
					infotm_chip->ecc_seed[0][i] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->sysinfo_seed[0][i], (((chip->ecc.layout->oobavail+PAGE_MANAGE_SIZE)>>busw) - 1), infotm_nand_dev->polynomial);
					infotm_chip->secc_seed[0][i] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->ecc_seed[0][i], ((chip->ecc.bytes * ((1 << chip->page_shift)/chip->ecc.size) >> busw) - 1), infotm_nand_dev->polynomial);
				}

				if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
					//if (infotm_nand_para != NULL)
						//infotm_chip->max_seed_cycle = infotm_nand_para->max_seed_cycle;
					if ((infotm_chip->ecc_mode == NAND_ECC_BCH40_MODE) && ((infotm_chip->page_size == 0x2000) || (infotm_chip->page_size == 0x4000)))
						infotm_chip->max_seed_cycle = (1 << (chip->phys_erase_shift - chip->page_shift));
				} else if (!strncmp((char*)plat->name, NAND_NORMAL_NAME, strlen((const char*)NAND_NORMAL_NAME))) {
					infotm_chip->max_seed_cycle = IMAPX800_NORMAL_SEED_CYCLE;
				} else {
					infotm_chip->max_seed_cycle = 1;
				}
				for (j=1; j<infotm_chip->max_seed_cycle; j++) {
					for(i = 0; i < 8; i++) {
						if (infotm_chip->page_size == 0x2000) {
							infotm_chip->data_seed[j][i] = *(unsigned *)((unsigned *)imapx800_seed + (j - 1) * 8 + i);
							infotm_chip->sysinfo_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_sysinfo_seed_8k + (j - 1) * 8 + i);
							infotm_chip->ecc_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_ecc_seed_8k + (j - 1) * 8 + i);
							infotm_chip->secc_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_secc_seed_8k + (j - 1) * 8 + i);
						} else if (infotm_chip->page_size == 0x4000) {
							infotm_chip->data_seed[j][i] = *(unsigned *)((unsigned *)imapx800_seed + (j - 1) * 8 + i);
							infotm_chip->sysinfo_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_sysinfo_seed_16k + (j - 1) * 8 + i);
							infotm_chip->ecc_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_ecc_seed_16k + (j - 1) * 8 + i);
							infotm_chip->secc_seed[j][i] =  *(unsigned *)((unsigned *)imapx800_secc_seed_16k + (j - 1) * 8 + i);
						} else {
							infotm_chip->max_seed_cycle = 1;
						}
					}
				}
			}
			break;

		default:
			break;
	}

	options_selected = (plat->platform_nand_data.chip.options & NAND_READ_RETRY_OPTIONS_MASK);
	options_define = (infotm_chip->options & NAND_READ_RETRY_OPTIONS_MASK);
	if (options_selected > options_define) {
		options_selected = options_define;
	}

	switch (options_selected) {

		case NAND_READ_RETRY_MODE:
			infotm_chip->ops_mode |= INFOTM_RETRY_MODE;
			break;

		default:
			break;
	}

	options_selected = (plat->platform_nand_data.chip.options & NAND_ESLC_OPTIONS_MASK);
	options_define = (infotm_chip->options & NAND_ESLC_OPTIONS_MASK);
	if (options_selected > options_define) {
		options_selected = options_define;
	}

	switch (options_selected) {

		case NAND_ESLC_MODE:
			infotm_chip->ops_mode |= INFOTM_ESLC_MODE;
			break;

		default:
			break;
	}

	return error;
}

static void imapx800_nand_cmd_ctrl(struct infotm_nand_chip *infotm_chip, int cmd,  unsigned int ctrl)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	if (cmd == NAND_CMD_NONE) {
		trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, 100);
		return;
	}

	if (ctrl & NAND_CLE)
		trx_afifo_ncmd(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, NF_CMD_CMD, (cmd & 0xff));
	else
		trx_afifo_ncmd(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, NF_CMD_ADDR, (cmd & 0xff));

	return;
}

static void imapx800_nand_enable_chip(struct imapx800_nand_chip *imapx800_chip, int chipnr)
{
	//printk("imapx800_nand_enable_chip %d\n", chipnr);

	switch (chipnr) {
		case 0:
			writel((~(1<<0)), imapx800_chip->regs + NF2CSR);
			if (imapx800_chip->ce1_gpio_index)
				gpio_set_value(imapx800_chip->ce1_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce1_gpio_index);
			if (imapx800_chip->ce2_gpio_index)
				gpio_set_value(imapx800_chip->ce2_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce2_gpio_index);
			if (imapx800_chip->ce3_gpio_index)
				gpio_set_value(imapx800_chip->ce3_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce3_gpio_index);
			break;

		case 1:
			if (imapx800_chip->ce1_gpio_index) {
				writel(0xf, imapx800_chip->regs + NF2CSR);
				//imapx_pad_set_outdat(0, 1, imapx800_chip->ce1_gpio_index);
				gpio_set_value(imapx800_chip->ce1_gpio_index,0);
			} else {
				writel((~(1<<1)), imapx800_chip->regs + NF2CSR);
			}
			if (imapx800_chip->ce2_gpio_index)
				gpio_set_value(imapx800_chip->ce2_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce2_gpio_index);
			if (imapx800_chip->ce3_gpio_index)
				gpio_set_value(imapx800_chip->ce3_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce3_gpio_index);
			break;

		case 2:
			writel(0xf, imapx800_chip->regs + NF2CSR);
			if (imapx800_chip->ce1_gpio_index)
				gpio_set_value(imapx800_chip->ce1_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce1_gpio_index);
			if (imapx800_chip->ce2_gpio_index)
				gpio_set_value(imapx800_chip->ce2_gpio_index,0);
				//imapx_pad_set_outdat(0, 1, imapx800_chip->ce2_gpio_index);
			if (imapx800_chip->ce3_gpio_index)
				gpio_set_value(imapx800_chip->ce3_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce3_gpio_index);
			break;

		case 3:
			writel(0xf, imapx800_chip->regs + NF2CSR);
			if (imapx800_chip->ce1_gpio_index)
				gpio_set_value(imapx800_chip->ce1_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce1_gpio_index);
			if (imapx800_chip->ce2_gpio_index)
				gpio_set_value(imapx800_chip->ce2_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce2_gpio_index);
			if (imapx800_chip->ce3_gpio_index)
				gpio_set_value(imapx800_chip->ce3_gpio_index,0);
				//imapx_pad_set_outdat(0, 1, imapx800_chip->ce3_gpio_index);
			break;

		case -1:
			writel(0xf, imapx800_chip->regs + NF2CSR);
			if (imapx800_chip->ce1_gpio_index)
				gpio_set_value(imapx800_chip->ce1_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce1_gpio_index);
			if (imapx800_chip->ce2_gpio_index)
				gpio_set_value(imapx800_chip->ce2_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce2_gpio_index);
			if (imapx800_chip->ce3_gpio_index)
				gpio_set_value(imapx800_chip->ce3_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce3_gpio_index);
			break;

		default:
			BUG();
			break;
	}
	return;
}

static void imapx800_get_controller(struct infotm_nand_chip *infotm_chip)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	spinlock_t *lock = &imapx800_chip->controller_lock;
	wait_queue_head_t *wq = &imapx800_chip->wq;
	DECLARE_WAITQUEUE(wait, current);

	spin_lock(lock);
	imapx800_chip->controller_claim++;
	while (1) {
		if (imapx800_chip->controller_state == CONTROLLER_READY)
			break;

		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(wq, &wait);
		spin_unlock(lock);
		schedule();
		remove_wait_queue(wq, &wait);
		spin_lock(lock);
	}
	imapx800_chip->controller_state = CONTROLLER_BUSY;
	spin_unlock(lock);
	return;
}

static void imapx800_release_controller(struct infotm_nand_chip *infotm_chip)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);

	spin_lock(&imapx800_chip->controller_lock);
	imapx800_chip->controller_state = CONTROLLER_READY;
	if (imapx800_chip->controller_claim > 0)
		imapx800_chip->controller_claim--;
	if (imapx800_chip->controller_claim > 0)
		wake_up(&imapx800_chip->wq);
	spin_unlock(&imapx800_chip->controller_lock);
	return;
}

static void imapx800_nand_select_chip(struct infotm_nand_chip *infotm_chip, int chipnr)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	//printk("imapx800_nand_select_chip %d\n", chipnr);

	switch (chipnr) {
		case 0:
		case 1:
		case 2:
		case 3:
			trx_afifo_enable(imapx800_chip);
			imapx800_chip->chip_selected = infotm_chip->chip_enable[chipnr];
			imapx800_chip->rb_received = infotm_chip->rb_enable[chipnr];
			break;

		case -1:
			trx_afifo_intr_clear(imapx800_chip);
			trx_afifo_disable(imapx800_chip);
			writel(0xf, imapx800_chip->regs + NF2CSR);
			if (imapx800_chip->ce1_gpio_index)
				gpio_set_value(imapx800_chip->ce1_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce1_gpio_index);
			if (imapx800_chip->ce2_gpio_index)
				gpio_set_value(imapx800_chip->ce2_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce2_gpio_index);
			if (imapx800_chip->ce3_gpio_index)
				gpio_set_value(imapx800_chip->ce3_gpio_index,1);
				//imapx_pad_set_outdat(1, 1, imapx800_chip->ce3_gpio_index);
			break;

		default:
			BUG();
			break;
	}
	return;
}

static int imapx800_nand_read_byte(struct infotm_nand_chip *infotm_chip, uint8_t *data, int count, int timeout)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	unsigned temp;
	int ret = 0, i;

	trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, 100);
	trx_afifo_ncmd(imapx800_chip, (NF_FLAG_DATA_FROM_INTER | (count - 1)), NF_CMD_DATA_IN, 0);
	ret = infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, timeout);

	if (count > sizeof(int)) {
		temp = trx_afifo_read_status_reg(imapx800_chip, 1);
		for (i=0; i<sizeof(int); i++)
			*(data + i) = (temp >> ((sizeof(int) - 1 - i) * 8)) & 0xff;
		temp = trx_afifo_read_status_reg(imapx800_chip, 0);
		for (i=sizeof(int); i<count; i++)
			*(data + i) = (temp >> ((2 * sizeof(int) - 1 - i) * 8)) & 0xff;
	} else {
		temp = trx_afifo_read_status_reg(imapx800_chip, 0);
		for (i=0; i<count; i++)
			*(data + i) = (temp >> ((count - 1) * 8)) & 0xff;
	}

	return ret;
}

static void imapx800_nand_write_byte(struct infotm_nand_chip *infotm_chip, uint8_t data)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);

	trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, 100);
	trx_afifo_ncmd(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, NF_CMD_DATA_OUT, (data & 0xff));
	//infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);

	return;
}

static int imapx800_nand_cmdfifo_start(struct infotm_nand_chip *infotm_chip, int timeout)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	int time_out_cnt = 0, value_int = 0, ret = 0;

	if (imapx800_chip->vitual_fifo_start)
		return ret;
	if ((imapx800_chip->nand_irq_no) && (timeout > 0)) {
		imapx800_chip->nand_irq_type = NF_TRXF_FINISH_INT;
		imapx800_chip->nand_irq_condition = 0;
		trx_afifo_irq_enable(imapx800_chip, NF_TRXF_FINISH_INT);
	}
	trx_afifo_disable(imapx800_chip);
	imapx800_nand_enable_chip(imapx800_chip, imapx800_chip->chip_selected);
	trx_afifo_start(imapx800_chip);

	if ((imapx800_chip->nand_irq_no) && (timeout > 0)) {
		time_out_cnt = wait_event_timeout(imapx800_chip->irq_wq, imapx800_chip->nand_irq_condition, timeout * HZ / 1000);
		if (time_out_cnt == 0) {
			printk("imapx800_nand_cmdfifo_start int %d %d \n", imapx800_chip->nand_irq_type, imapx800_chip->nand_irq_condition);
			if (imapx800_chip->nand_irq_condition == 0)
				ret = -EIO;
		}
		imapx800_chip->nand_irq_condition = 0;
		trx_afifo_inte_clear(imapx800_chip);
	} else {
		do {
			value_int = readl(imapx800_chip->regs + NF2INTR);
			if (value_int & NF_TRXF_FINISH_INT)
				break;
		}while (time_out_cnt++ < INFOTM_NAND_BUSY_TIMEOUT);

		if (time_out_cnt >= INFOTM_NAND_BUSY_TIMEOUT) {
			printk("imapx800_nand_cmdfifo_start %x \n", value_int);
			ret = -EIO;
		}

		if (value_int & NF_RB_TIMEOUT_INT) {
			printk("imapx800_nand_cmdfifo_start RB_TIMEOUT %x \n", value_int);
			ret = -EBUSY;
		}
	}

	trx_afifo_intr_clear(imapx800_chip);
	imapx800_nand_enable_chip(imapx800_chip, -1);
	nf2_soft_reset(imapx800_chip, NF_RESET);
	trx_afifo_enable(imapx800_chip);
	return ret;
}

static int imapx800_nand_wait_devready(struct infotm_nand_chip *infotm_chip, int chipnr)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);

	infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
	trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_WAIT_RDY, 100);
	trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, 20);
	trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_WAIT_RDY, 100);

	return 1;
}

static void trx_afifo_reg (struct imapx800_nand_chip *imapx800_chip, unsigned int flag, unsigned int addr, unsigned int data)
{
	unsigned int value = 0;

	if (flag==0){
		value = (0xf<<12) | (flag<<8) | addr;
		writel(value, imapx800_chip->regs + NF2TFPT);
		value = data & 0xffff;
		writel(value, imapx800_chip->regs + NF2TFPT);
		value = (data>>16) & 0xffff;
		writel(value, imapx800_chip->regs + NF2TFPT);
	} else if (flag==1) {
		value = (0xf<<12) | (flag<<8) | addr;
		writel(value, imapx800_chip->regs + NF2TFPT);
		value = data & 0xffff;
		writel(value, imapx800_chip->regs + NF2TFPT);
	} else {
		value = (0xf<<12) | (flag<<8) | addr;
		writel(value, imapx800_chip->regs + NF2TFPT);
	}
}

static int imapx800_nand_dma_waiting(struct infotm_nand_chip *infotm_chip, struct imapx800_nand_chip *imapx800_chip, int ecc_enable, int secc_used, int trans_dir, int prog_type, int timeout)
{
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	unsigned time_out_cnt = 0, value_int = 0, ret = 0, int_finish, temp, timing, sys_cycle;

	sys_cycle = clk_get_rate(imapx800_chip->ecc_clk);
	sys_cycle /= 1000000;
	sys_cycle = 10*1000 / sys_cycle;
	timing = readl(imapx800_chip->regs + NF2AFTM);
	timing &= ~0xf;
	if (trans_dir == ECC_DECODER) {
		int_finish = NF_SDMA_FINISH_INT;
		trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_WAIT_RDY, 50);
		trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_READ_WHOLE_PAGE, 10);
		temp = ((infotm_nand_dev->timing[NF_tREH]* 100) / sys_cycle);
		if (temp % 10)
			temp = temp / 10 + 1;
		else
			temp = temp / 10;
		if (temp <= 0)
			temp = 1;
		timing |= (temp - 1);
		writel((timing & 0x1fffff), imapx800_chip->regs + NF2AFTM);
	} else {
		int_finish = NF_TRXF_FINISH_INT;
		trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, 100);
		trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_WRITE_WHOLE_PAGE, 10);
		if (prog_type > 0) {
			trx_afifo_nop(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, 50);
			trx_afifo_ncmd(imapx800_chip, NF_FLAG_EXEC_DIRECTLY, NF_CMD_CMD, prog_type);
		}
		temp = ((infotm_nand_dev->timing[NF_tWH]* 100) / sys_cycle);
		if (temp % 10)
			temp = temp / 10 + 1;
		else
			temp = temp / 10;
		if (temp <= 0)
			temp = 1;
		timing |= (temp - 1);
		writel((timing & 0x1fffff), imapx800_chip->regs + NF2AFTM);
	}
	if ((imapx800_chip->nand_irq_no) && (timeout > 0)) {
		imapx800_chip->nand_irq_type = int_finish;
		imapx800_chip->nand_irq_condition = 0;
		trx_afifo_irq_enable(imapx800_chip, int_finish);
	}
	trx_afifo_reg(imapx800_chip, 0x8, 1, 0);
	trx_afifo_disable(imapx800_chip);
	imapx800_nand_enable_chip(imapx800_chip, imapx800_chip->chip_selected);
	trx_engine_start(imapx800_chip, (NF_TRXF_START | NF_DMA_START | ((ecc_enable | secc_used)?NF_ECC_START:0)));

	if ((imapx800_chip->nand_irq_no) && (timeout > 0)) {
		time_out_cnt = wait_event_timeout(imapx800_chip->irq_wq, imapx800_chip->nand_irq_condition, timeout * HZ / 1000);
		if (time_out_cnt == 0) {
			printk("INFOTM_DMA_BUSY_TIMEOUT int %d %d \n", imapx800_chip->nand_irq_type, imapx800_chip->nand_irq_condition);
			if (imapx800_chip->nand_irq_condition == 0)
				ret = -EBUSY;
		}
		imapx800_chip->nand_irq_condition = 0;
		trx_afifo_inte_clear(imapx800_chip);
	} else {
		do {
			value_int = readl(imapx800_chip->regs + NF2INTR);
			if (value_int & int_finish)
				break;
		}while (time_out_cnt++ < INFOTM_DMA_BUSY_TIMEOUT);

		if (time_out_cnt >= INFOTM_DMA_BUSY_TIMEOUT) {
			printk("INFOTM_DMA_BUSY_TIMEOUT \n");
			ret = -EBUSY;
		}

		if (value_int & NF_RB_TIMEOUT_INT) {
			printk("INFOTM_DMA_BUSY_TIMEOUT RB_TIMEOU \n");
			ret = -EBUSY;
		}
		if (value_int & NF_SECC_UNCORRECT_INT)
			infotm_chip->secc_status = 1;

		trx_afifo_intr_clear(imapx800_chip);
	}

	imapx800_nand_enable_chip(imapx800_chip, -1);
	if (ret || (!(ecc_enable | secc_used)) || (trans_dir == ECC_ENCODER)) {
		/*if (trans_dir == ECC_ENCODER) {
		  ktime_get_ts(&ts_write_end);
		  printk("infotm nand dma reset %d %d \n", imapx800_chip->chip_selected, (ts_write_end.tv_nsec - ts_write_start.tv_nsec));
		  }*/
		nf2_soft_reset(imapx800_chip, NF_RESET);
		trx_afifo_enable(imapx800_chip);
	}

	return ret;
}

static int imapx800_nand_dma_read(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode,  unsigned char *sys_info_buf, int sys_info_len, int secc_mode)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct nand_chip *chip = &infotm_chip->chip;
	int ecc_enable, page_mode, ecc_type, eram_mode, tot_ecc_num, tot_page_num, trans_dir, half_page_en,
	    ecc_num, secc_num, secc_used, secc_type, rsvd_ecc_en, data_len, oob_len, sdma_2ch_mode = 0, ret, ecc_offset, timeout;
	dma_addr_t data_dma_addr, oob_dma_addr;

	if (buf != NULL)
		data_dma_addr = imapx800_chip->data_dma_addr;//dma_map_single(NULL, (void *)buf, len, DMA_FROM_DEVICE);
	if (sys_info_buf != NULL)
		oob_dma_addr = imapx800_chip->oob_dma_addr;//dma_map_single(NULL, (void *)sys_info_buf, sys_info_len, DMA_FROM_DEVICE);
	if (ecc_mode < 0) {
		ecc_enable = 0;
		ecc_type = 0;
		ecc_num = 0;
		ecc_offset = 0;
	} else {
		ecc_enable = 1;
		ecc_type = ecc_mode;
		ecc_num = (len/chip->ecc.size) * chip->ecc.bytes;
		if (infotm_chip->short_mode)
			ecc_offset = len + sys_info_len;
		else
			ecc_offset = infotm_chip->page_size + chip->ecc.layout->oobavail + PAGE_MANAGE_SIZE;
	}
	if (secc_mode < 0) {
		secc_used = 0;
		secc_type = 0;
		secc_num = 0;
		rsvd_ecc_en = 0;
		if (infotm_chip->short_mode)
			rsvd_ecc_en = 1;
	} else {
		secc_used = 1;
		secc_type = secc_mode;
		secc_num = infotm_chip->secc_bytes;
		rsvd_ecc_en = 1;
	}
	if(infotm_chip->half_page_en)
		ecc_num = ecc_num << 1;
	eram_mode = ECCRAM_TWO_BUFFER;
	tot_page_num = len + sys_info_len;
	tot_ecc_num = ecc_num + secc_num;
	trans_dir = ECC_DECODER;
	half_page_en = infotm_chip->half_page_en;
	if ((len == 0) || ((ecc_mode < 0) && (secc_mode < 0) && (len != HYNIX_OTP_SIZE)))
		page_mode = 1;
	else
		page_mode = 0;
	if (len == 0) {
		data_dma_addr = imapx800_chip->oob_dma_addr;
		oob_dma_addr = 0;
		data_len = sys_info_len;
		oob_len = 0;
		sdma_2ch_mode = 0;
		timeout = 0;
	} else {
		data_dma_addr = imapx800_chip->data_dma_addr;
		sdma_2ch_mode = sys_info_len ? 1 : 0;
		oob_dma_addr = sys_info_len ? imapx800_chip->oob_dma_addr : 0;
		data_len = len;
		oob_len = sys_info_len;
		if (len >= 1024)
			timeout = 100;
		else
			timeout = 0;
	}
	if (len == HYNIX_OTP_SIZE) {
		page_mode = 0;
		tot_page_num = 1024;
		data_len = 1024;
	}

	if (likely(infotm_chip->ops_mode & INFOTM_RANDOMIZER_MODE) && (ecc_enable | secc_used)) {
		nf2_randomizer_ops(imapx800_chip, NF_RANDOMIZER_ENABLE);
		nf2_randomizer_seed_init(infotm_chip, imapx800_chip, len, ecc_enable, sys_info_len, secc_used);
	} else {
		nf2_randomizer_ops(imapx800_chip, NF_RANDOMIZER_DISABLE);
	}

	if ((page_mode == 1) && (secc_used == 1)) {
		ecc_num = (infotm_chip->page_size/chip->ecc.size) * chip->ecc.bytes;
		ecc_offset = infotm_chip->page_size + chip->ecc.layout->oobavail + PAGE_MANAGE_SIZE;
		ecc_offset += ecc_num;
		ecc_enable = 1;
		ecc_type = infotm_chip->ecc_mode;
		nf2_addr_cfg(imapx800_chip, 0, infotm_chip->page_size, ecc_offset, imapx800_chip->busw);
	} else {
		nf2_addr_cfg(imapx800_chip,  0, 0, ecc_offset, imapx800_chip->busw);
	}

	//nf2_addr_cfg(imapx800_chip, infotm_chip->page_addr, 0, ecc_offset, 8);
	nf2_ecc_cfg(imapx800_chip, ecc_enable, page_mode, ecc_type, eram_mode,
			tot_ecc_num, tot_page_num, trans_dir, half_page_en);
	nf2_secc_cfg(imapx800_chip, secc_used, secc_type, rsvd_ecc_en);
	nf2_sdma_cfg(imapx800_chip, chip->ecc.size, data_dma_addr, data_len, oob_dma_addr, oob_len, sdma_2ch_mode);

	/*printk("nf2_ecc_cfg %d, %d, %d, %d, %d, %d, %d, %d\n", ecc_enable, page_mode, ecc_type, eram_mode, tot_ecc_num, tot_page_num, trans_dir, half_page_en);
	  printk("nf2_secc_cfg %d, %d, %d\n", secc_used, secc_type, rsvd_ecc_en);
	  printk("nf2_sdma_cfg 0x%x, %d, 0x%x, %d, %d\n", data_dma_addr, data_len, oob_dma_addr, oob_len, sdma_2ch_mode);
	  printk("nf2_addr_cfg %d\n", ecc_offset);*/

	ret = imapx800_nand_dma_waiting(infotm_chip, imapx800_chip, ecc_enable, secc_used, trans_dir, -1, timeout);
	if (ret)
		return ret;

	if (buf != NULL)
		//dma_unmap_single(NULL, data_dma_addr, len, DMA_FROM_DEVICE);
		memcpy(buf, imapx800_chip->nand_data_buf, len);
	if (sys_info_buf != NULL)
		//dma_unmap_single(NULL, oob_dma_addr, sys_info_len, DMA_FROM_DEVICE);
		memcpy(sys_info_buf, imapx800_chip->sys_info_buf, sys_info_len);
	wmb();

	return 0;
}

static int imapx800_nand_dma_write(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode,  unsigned char *sys_info_buf, int sys_info_len, int secc_mode, int prog_type)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct nand_chip *chip = &infotm_chip->chip;
	int ecc_enable, page_mode, ecc_type, eram_mode, tot_ecc_num, tot_page_num, trans_dir, half_page_en,
	    ecc_num, secc_num, secc_used, secc_type, rsvd_ecc_en, data_len, oob_len, sdma_2ch_mode = 0, ret, ecc_offset, timeout = 0;
	dma_addr_t data_dma_addr, oob_dma_addr;

	if ((buf != NULL) && (buf != imapx800_chip->nand_data_buf))
		memcpy(imapx800_chip->nand_data_buf, buf, len);
	if ((sys_info_buf != NULL) && (sys_info_buf != imapx800_chip->sys_info_buf))
		memcpy(imapx800_chip->sys_info_buf, sys_info_buf, sys_info_len);
	wmb();

	if (ecc_mode < 0) {
		ecc_enable = 0;
		ecc_type = 0;
		ecc_num = 0;
		ecc_offset = 0;
	} else {
		ecc_enable = 1;
		ecc_type = ecc_mode;
		ecc_num = (len/chip->ecc.size) * chip->ecc.bytes;
		if (infotm_chip->short_mode)
			ecc_offset = len + sys_info_len;
		else
			ecc_offset = infotm_chip->page_size + chip->ecc.layout->oobavail + PAGE_MANAGE_SIZE;
	}
	if (secc_mode < 0) {
		secc_used = 0;
		secc_type = 0;
		secc_num = 0;
		rsvd_ecc_en = 0;
		if (infotm_chip->short_mode)
			rsvd_ecc_en = 1;
	} else {
		secc_used = 1;
		secc_type = secc_mode;
		secc_num = infotm_chip->secc_bytes;
		rsvd_ecc_en = 1;
	}
	if(infotm_chip->half_page_en)
		ecc_num = ecc_num << 1;
	eram_mode = ECCRAM_TWO_BUFFER;
	tot_page_num = len + sys_info_len;
	tot_ecc_num = ecc_num + secc_num;
	half_page_en = infotm_chip->half_page_en;
	trans_dir = ECC_ENCODER;
	if ((len == 0) || ((ecc_mode < 0) && (secc_mode < 0)))
		page_mode = 1;
	else
		page_mode = 0;
	if (len == 0) {
		data_dma_addr = imapx800_chip->oob_dma_addr;
		oob_dma_addr = 0;
		data_len = sys_info_len;
		oob_len = 0;
		sdma_2ch_mode = 0;
		timeout = 0;
	} else {
		data_dma_addr = imapx800_chip->data_dma_addr;
		sdma_2ch_mode = sys_info_len ? 1 : 0;
		oob_dma_addr = sys_info_len ? imapx800_chip->oob_dma_addr : 0;
		data_len = len;
		oob_len = sys_info_len;
		timeout = 100;
	}
	if (likely(infotm_chip->ops_mode & INFOTM_RANDOMIZER_MODE) && (ecc_enable | secc_used)) {
		nf2_randomizer_ops(imapx800_chip, NF_RANDOMIZER_ENABLE);
		nf2_randomizer_seed_init(infotm_chip, imapx800_chip, len, ecc_enable, sys_info_len, secc_used);
	} else {
		nf2_randomizer_ops(imapx800_chip, NF_RANDOMIZER_DISABLE);
	}

	nf2_addr_cfg(imapx800_chip, 0, 0, ecc_offset, imapx800_chip->busw);
	nf2_ecc_cfg(imapx800_chip, ecc_enable, page_mode, ecc_type, eram_mode,
			tot_ecc_num, tot_page_num, trans_dir, half_page_en);
	nf2_secc_cfg(imapx800_chip, secc_used, secc_type, rsvd_ecc_en);
	nf2_sdma_cfg(imapx800_chip, chip->ecc.size, data_dma_addr, data_len, oob_dma_addr, oob_len, sdma_2ch_mode);

	ret = imapx800_nand_dma_waiting(infotm_chip, imapx800_chip, ecc_enable, secc_used, trans_dir, prog_type, timeout);
	if (ret)
		return ret;

	return 0;
}

static irqreturn_t imapx800_nand_irq(int irq, void *arg)
{
	struct imapx800_nand_chip *imapx800_chip = (struct imapx800_nand_chip *)arg;
	unsigned val_int_en = 0, val_int_status = 0;

	val_int_status = readl(imapx800_chip->regs + NF2INTR);
	val_int_en = readl(imapx800_chip->regs + NF2INTE);

	trx_afifo_intr_clear(imapx800_chip);
	if (imapx800_chip->nand_irq_type & NF_TRXF_FINISH_INT) {
		if(val_int_status & NF_TRXF_FINISH_INT) {
			imapx800_chip->nand_irq_condition = 1;
			wake_up(&imapx800_chip->irq_wq);
		}
	} else if (imapx800_chip->nand_irq_type & NF_SDMA_FINISH_INT) {
		if(val_int_status & NF_SDMA_FINISH_INT) {
			imapx800_chip->nand_irq_condition = 1;
			wake_up(&imapx800_chip->irq_wq);
		}
	}

	return IRQ_HANDLED;
}

static int imapx800_nand_hwecc_correct(struct infotm_nand_chip *infotm_chip, unsigned size, unsigned oob_size)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct nand_chip *chip = &infotm_chip->chip;
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct mtd_ecc_stats stats;
	unsigned ecc_step_num, ecc_result, ecc_fix_bits;
	int ret = 0;

	stats = mtd->ecc_stats;
	infotm_chip->ecc_status = 0;
	infotm_chip->secc_status = 0;
	if (size % chip->ecc.size) {
		printk("error parameter size for ecc correct %x\n", size);
		ret = -EINVAL;
		goto exit2;
	}

	if (size) {
		ecc_result = readl(imapx800_chip->regs + NF2ECCINFO8);
		if (ecc_result & ((1 << (size / chip->ecc.size)) - 1)) {
			//printk ("nand communication have uncorrectable ecc error %x\n", ecc_result);
			ret = -EIO;
			infotm_chip->ecc_status = 1;
			goto exit1;
		}
		for (ecc_step_num = 0; ecc_step_num < (size / chip->ecc.size); ecc_step_num++) {
			if ((ecc_step_num % 4) == 0) {
				ecc_result = readl(imapx800_chip->regs + NF2ECCINFO0 + ecc_step_num);
				//printk ("nand communication ecc %x %d\n", ecc_result, ecc_step_num);
			}
			ecc_fix_bits = ((ecc_result >> ((ecc_step_num % 4) * 8)) & 0x7f);
			if (ecc_fix_bits >= infotm_chip->max_correct_bits) {
				//printk ("nand communication ecc euclean %d %x %x %d\n", infotm_chip->page_addr>>pages_per_blk_shift, infotm_chip->page_addr&((1<<pages_per_blk_shift) -1), ecc_result, ecc_fix_bits);
				//mtd->ecc_stats.corrected++;
			}
		}
	}

exit1:
	if (oob_size) {
		ecc_result = readl(imapx800_chip->regs + NF2ECCINFO9);
		if (ecc_result & SECC0_RESULT_FLAG) {
			//printk ("nand communication have uncorrectable secc error %x\n", ecc_result);
			ret = -EIO;
			infotm_chip->secc_status = 1;
			goto exit2;
		} else if (ecc_result & (1<<7)) {
			WARN_ON(infotm_chip->secc_status == 1);
			infotm_chip->secc_status = (ecc_result & SECC0_RESULT_BITS_MASK);
			infotm_chip->secc_status |= (readl(imapx800_chip->regs + NF2ECCLOCA)&0xffff0000);
		}
	}

exit2:
	if (ret)
		mtd->ecc_stats.corrected = stats.corrected;
	nf2_soft_reset(imapx800_chip, NF_RESET);
	trx_afifo_enable(imapx800_chip);
	return ret;
}

#if 0
static int imapx800_nand_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_chip *infotm_chip_bbt = infotm_chip;
	struct infotm_nand_bbt_info *nand_bbt_info = NULL;
	struct list_head *l, *n;
	struct nand_bbt_t *nand_bbt;
	int32_t mtd_erase_shift, blk_addr;
	int j;

	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip_bbt = chip_list_to_imapx800(l);
		if ((infotm_chip_bbt->infotm_nandenv_info != NULL) && (infotm_chip_bbt->infotm_nandenv_info->env_valid == 1)) {
			nand_bbt_info = &infotm_chip_bbt->infotm_nandenv_info->nand_bbt_info;
			if (nand_bbt_info != NULL)
				break;
		}
	}
	mtd_erase_shift = fls(mtd->erasesize) - 1;
	blk_addr = (int)(ofs >> mtd_erase_shift);
	if (nand_bbt_info != NULL) {
		for (j=0; j<nand_bbt_info->total_bad_num; j++) {
			nand_bbt = &nand_bbt_info->nand_bbt[j];
			if (blk_addr == nand_bbt->blk_addr) {
				printk("imapx800 nand bbt detect bad blk %llx %d %d %d \n", ofs, nand_bbt->blk_addr, nand_bbt->chip_num, nand_bbt->bad_type);
				return EFAULT;
			}
		}
	}
	return 0;
}
#endif

static void imapx800_nand_boot_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)buf;
	uint8_t *oob_buf = chip->oob_poi;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail;
	int error = 0, i = 0, j, page_addr, dma_cycle = 1;

	if (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC) {
		dma_cycle = (IMAPX800_PARA_SAVE_SIZE >> chip->page_shift);
		if (!(infotm_chip->ops_mode & INFOTM_RETRY_MODE))
			dma_cycle = 1;
	}
	if (infotm_chip->valid_chip[i]) {

		page_addr = infotm_chip->page_addr;
		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_SEQIN, 0, page_addr, i);

		for (j=0; j<dma_cycle; j++) {
			//if (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC)
				//*(uint32_t *)oob_buf = crc32(0, buf + j * nand_page_size, nand_page_size);
			if (j == (dma_cycle - 1))
				infotm_chip->infotm_nand_dma_write(infotm_chip, (unsigned char *)(buf + j * nand_page_size), nand_page_size, infotm_chip->ecc_mode, oob_buf, nand_oobavail_size, infotm_chip->secc_mode, NAND_CMD_PAGEPROG);
			else
				infotm_chip->infotm_nand_dma_write(infotm_chip, (unsigned char *)(buf + j * nand_page_size), nand_page_size, infotm_chip->ecc_mode, oob_buf, nand_oobavail_size, infotm_chip->secc_mode, -1);
		}
		if (error)
			goto exit;

		buf += nand_page_size;
	} else {
		error = -ENODEV;
		goto exit;
	}

exit:
	return;
}

static int imapx800_nand_boot_write_page(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int page, int cached, int raw)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	//struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	int status, i, pages_per_blk;
	uint16_t seed_save[4][8];
	int j, k, write_page, busw;

	pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));
	if(rbgetint("irom_new") != IMAPX15_NEW_IROM)
	{
	busw = (imapx800_chip->busw == 16) ? 1 : 0;
	if (likely(!raw)) {
		write_page = (page & (pages_per_blk - 1));
		i = infotm_chip->max_seed_cycle;
		infotm_chip->max_seed_cycle = 1;
		for (j=0; j<8; j++) {
			seed_save[0][j] = infotm_chip->data_seed[0][j];
			seed_save[1][j] = infotm_chip->sysinfo_seed[0][j];
			seed_save[2][j] = infotm_chip->ecc_seed[0][j];
		}
		for (j=0; j<8; j++) {
			infotm_chip->data_seed[0][j] = uboot0_default_seed[j];
			infotm_chip->sysinfo_seed[0][j] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->data_seed[0][j] & 0x7fff, ((infotm_chip->uboot0_unit >> busw) - 1), infotm_chip->infotm_nand_dev->polynomial);
			infotm_chip->ecc_seed[0][j] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->sysinfo_seed[0][j], (((PAGE_MANAGE_SIZE) >> busw) - 1), infotm_chip->infotm_nand_dev->polynomial);
		}
		for (k=0; k<IMAPX800_BOOT_COPY_NUM; k++) {
			if (write_page == 0) {
				infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE1, -1, k * IMAPX800_BOOT_PAGES_PER_COPY, 0);
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE2, -1, -1, 0);
				status = chip->waitfunc(mtd, chip);
				if (status & NAND_STATUS_FAIL)
					continue;
			}

			chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, (k * IMAPX800_BOOT_PAGES_PER_COPY + write_page));
			chip->ecc.write_page(mtd, chip, buf);
			status = chip->waitfunc(mtd, chip);
		}
		infotm_chip->max_seed_cycle = i;
		for (j=0; j<8; j++) {
			infotm_chip->data_seed[0][j] = seed_save[0][j];
			infotm_chip->sysinfo_seed[0][j] = seed_save[1][j];
			infotm_chip->ecc_seed[0][j] = seed_save[2][j];
		}
	}
	}

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page);

	if (unlikely(raw))
		chip->ecc.write_page_raw(mtd, chip, buf);
	else
		chip->ecc.write_page(mtd, chip, buf);

	if (!cached || !(chip->options & NAND_CACHEPRG)) {

		status = chip->waitfunc(mtd, chip);
		if ((status & NAND_STATUS_FAIL) && (chip->errstat))
			status = chip->errstat(mtd, chip, FL_WRITING, status, page);

		if (status & NAND_STATUS_FAIL)
			printk("infotm nand write failed at %d \n", page);
	} else {
		status = chip->waitfunc(mtd, chip);
	}

	return 0;
}

static int imapx800_nand_boot_read_page_hwecc_onece(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, uint8_t *oob_buf, int page, int chipnr)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail;
	int error = 0, i = chipnr, stat = 0, page_addr, column;

	if (infotm_chip->valid_chip[i]) {

		page_addr = page;
		column = nand_page_size + nand_oobavail_size;

		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, column, page_addr, i);
		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);

		error = infotm_chip->infotm_nand_dma_read(infotm_chip, buf, nand_page_size, infotm_chip->ecc_mode, oob_buf, ((oob_buf != NULL)?nand_oobavail_size:0), ((oob_buf != NULL)?infotm_chip->secc_mode:-1));
		stat = infotm_chip->infotm_nand_hwecc_correct(infotm_chip, nand_page_size, (infotm_chip->secc_mode < 0) ? 0 : nand_oobavail_size);
		if (stat < 0) {
			mtd->ecc_stats.failed++;
		} else {
			mtd->ecc_stats.corrected += stat;
		}
		if (error) {
			goto exit;
		}
	} else {
		error = -ENODEV;
		mdelay(10);
		goto exit;
	}

exit:
	return error;
}

static int imapx800_nand_boot_read_oob_raw_onechip(struct mtd_info *mtd, struct nand_chip *chip, unsigned char *oob_buffer, int page, int readlen, int chipnr)
{
	int32_t i = chipnr, page_addr;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = (mtd->oobsize <= BASIC_RAW_OOB_SIZE) ? mtd->oobsize : BASIC_RAW_OOB_SIZE;

	if (infotm_chip->valid_chip[i]) {

		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		page_addr = page;
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, nand_page_size, page_addr, i);

		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		chip->read_buf(mtd, oob_buffer, nand_oobavail_size);

		oob_buffer += nand_oobavail_size;

	} else {
		mdelay(10);
		goto exit;
	}

exit:
	return readlen;
}

static int imapx800_nand_boot_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int page, int oob_len)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	uint8_t *oob_buf = chip->oob_poi;
	uint8_t *data_buf = buf;
	struct mtd_ecc_stats stats;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail;
	int error = 0, i = 0, j, k, page_addr, retry_level_support = 0, retry_level = 0, retry_count = 0;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		retry_level_support = infotm_chip->retry_level_support;

	stats = mtd->ecc_stats;
	if (infotm_chip->valid_chip[i]) {

		retry_count = 0;
		page_addr = page;
		do {
			mtd->ecc_stats.failed = stats.failed;
			error = imapx800_nand_boot_read_page_hwecc_onece(mtd, chip, data_buf, oob_buf, page_addr, i);
			if (error) {
				printk("imapx800 nand boot read page error %d\n", error);
				goto exit;
			}

			if ((mtd->ecc_stats.failed - stats.failed) && (retry_count == 0)) {
				memset(oob_buf, 0x00, nand_oobavail_size);
				imapx800_nand_boot_read_oob_raw_onechip(mtd, chip, oob_buf, page_addr, nand_oobavail_size, i);
				for (j=0; j<nand_oobavail_size; j++) {
					//printk("%02hhx ", oob_buf[j]);
					if (oob_buf[j] != 0xff)
						break;
				}
				//printk("\n\n ");
				if (j >= nand_oobavail_size) {
					mtd->ecc_stats.failed = stats.failed;
					memset(data_buf, 0xff, nand_page_size);
					memset(oob_buf, 0xff, nand_oobavail_size);
				} else if (retry_level_support == 0) {
					printk("read page ecc error %x %d \n ", page, i);
					for (k=0; k<nand_oobavail_size; k++) {
						printk("%02x ", oob_buf[k]);
					}
					printk("\n ");
				}
			}

			if ((mtd->ecc_stats.failed - stats.failed) && retry_level_support) {
				if (retry_count > retry_level_support)
					break;
				retry_level = infotm_chip->retry_level[i] + 1;
				if (retry_level > retry_level_support)
					retry_level = 0;
				infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
				list_for_each_safe(l, n, &imapx800_chip->chip_list) {
					infotm_chip_temp = chip_list_to_imapx800(l);
					infotm_chip_temp->retry_level[i] = retry_level;
				}
				if (retry_count == retry_level_support)
					break;
				retry_count++;
			}

		} while (retry_level_support && (mtd->ecc_stats.failed - stats.failed));

		if ((retry_count > 0) && (retry_level != 0) && (infotm_chip->mfr_type != NAND_MFR_HYNIX)) {
			retry_level = 0;
			infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
			list_for_each_safe(l, n, &imapx800_chip->chip_list) {
				infotm_chip_temp = chip_list_to_imapx800(l);
				infotm_chip_temp->retry_level[i] = retry_level;
			}
		}

		if (mtd->ecc_stats.failed - stats.failed) {
			printk("imapx800 read page retry failed %x %d \n ", page_addr, infotm_chip->retry_level[i]);
			goto exit;
		} else {
			if (retry_count > 0)
				printk("imapx800 read page retry success %x %d \n ", page_addr, retry_count);
			goto exit;
		}

		oob_buf += nand_oobavail_size;
		data_buf += nand_page_size;
	}

exit:
	return error;
}

static void imapx800_nand_boot_erase_cmd(struct mtd_info *mtd, int page)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct nand_chip *chip = mtd->priv;
	int i, page_addr, pages_per_blk;
	loff_t offs;

	pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));
	if(rbgetint("irom_new") != IMAPX15_NEW_IROM)
{
	if ((page && (pages_per_blk - 1)) == 0) {
		for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++) {
			page_addr = i * IMAPX800_BOOT_PAGES_PER_COPY;
			offs = (page_addr * mtd->writesize);
			if (chip->block_bad(mtd, offs, 0))
				continue;

			infotm_chip->infotm_nand_select_chip(infotm_chip, 0);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE1, -1, page_addr, 0);
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE2, -1, -1, 0);
			chip->waitfunc(mtd, chip);
		}
	}
}

	if (infotm_chip->valid_chip[0]) {

		page_addr = page;
		infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE1, -1, page_addr, 0);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE2, -1, -1, 0);
		chip->waitfunc(mtd, chip);
	}

	return;
}

static void imapx800_nand_check_uboot0_status(struct mtd_info *mtd, struct nand_chip *chip)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct mtd_ecc_stats stats;
	unsigned char *data_buf;
	int error = 0, i = 0;
	unsigned char oob_buf[PAGE_MANAGE_SIZE];
	struct mtd_oob_ops infotm_oob_ops;
	loff_t offset;
	uint16_t seed_save[4][8];
	int busw, j, save_cycle;

	data_buf = kzalloc(infotm_chip->uboot0_size, GFP_KERNEL);
	if (data_buf == NULL)
		return;

	if(rbgetint("irom_new") != IMAPX15_NEW_IROM)
{
	busw = (imapx800_chip->busw == 16) ? 1 : 0;
	save_cycle = infotm_chip->max_seed_cycle;
	infotm_chip->max_seed_cycle = 1;
	for (j=0; j<8; j++) {
		seed_save[0][j] = infotm_chip->data_seed[0][j];
		seed_save[1][j] = infotm_chip->sysinfo_seed[0][j];
		seed_save[2][j] = infotm_chip->ecc_seed[0][j];
	}
	for (j=0; j<8; j++) {
		infotm_chip->data_seed[0][j] = uboot0_default_seed[j];
		infotm_chip->sysinfo_seed[0][j] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->data_seed[0][j] & 0x7fff, ((infotm_chip->uboot0_unit >> busw) - 1), infotm_chip->infotm_nand_dev->polynomial);
		infotm_chip->ecc_seed[0][j] = nf2_randc_seed_hw(imapx800_chip, infotm_chip->sysinfo_seed[0][j], (((PAGE_MANAGE_SIZE) >> busw) - 1), infotm_chip->infotm_nand_dev->polynomial);
	}
}

	stats = mtd->ecc_stats;
	infotm_oob_ops.mode = MTD_OOB_AUTO;
	infotm_oob_ops.len = mtd->writesize;
	infotm_oob_ops.ooblen = PAGE_MANAGE_SIZE;
	infotm_oob_ops.ooboffs = 0;
	infotm_oob_ops.oobbuf = oob_buf;
	do {

		//mtd->ecc_stats.failed = stats.failed;
		//error = imapx800_nand_boot_read_page_hwecc(mtd, chip, data_buf, i, PAGE_MANAGE_SIZE);
		offset = i * mtd->writesize;
		infotm_oob_ops.datbuf = data_buf;

		memset(oob_buf, 0, PAGE_MANAGE_SIZE);
		error = mtd->_read_oob(mtd, offset, &infotm_oob_ops);
		if ((error != 0) || (mtd->ecc_stats.failed - stats.failed))
			printk("page %d read failed %d\n", i, error);

		i++;
	} while (i < ((infotm_chip->uboot0_size / mtd->writesize) + 2));

	if(rbgetint("irom_new") != IMAPX15_NEW_IROM)
{
	infotm_chip->max_seed_cycle = save_cycle;
	for (j=0; j<8; j++) {
		infotm_chip->data_seed[0][j] = seed_save[0][j];
		infotm_chip->sysinfo_seed[0][j] = seed_save[1][j];
		infotm_chip->ecc_seed[0][j] = seed_save[2][j];
	}
}
	kfree(data_buf);
	return;
}

static int infotm_nand_probe(struct infotm_nand_platform *plat, struct imapx800_nand_chip *imapx800_chip)
{
	struct infotm_nand_chip *infotm_chip = NULL;
	struct nand_chip *chip = NULL;
	struct mtd_info *mtd = NULL;
	struct infotm_nand_para_save *infotm_nand_save;
	int err = 0;

	infotm_chip = kzalloc(sizeof(struct infotm_nand_chip), GFP_KERNEL);
	if (infotm_chip == NULL) {
		printk("no memory for flash infotm_chip\n");
		err = -ENOMEM;
		goto exit_error;
	}

	/* initialize mtd info data struct */
	infotm_chip->priv = imapx800_chip;
	infotm_chip->platform = plat;
	infotm_chip->ecc_desc = imapx800_ecc_list;
	infotm_chip->max_ecc_mode = sizeof(imapx800_ecc_list) / sizeof(imapx800_ecc_list[0]);
	plat->infotm_chip = infotm_chip;
	chip = &infotm_chip->chip;
	chip->priv = &infotm_chip->mtd;
	mtd = &infotm_chip->mtd;
	mtd->priv = chip;
	mtd->owner = THIS_MODULE;
	infotm_chip->device.coherent_dma_mask = DMA_BIT_MASK(32);

	list_add_tail(&infotm_chip->list, &imapx800_chip->chip_list);
	infotm_chip->infotm_nand_hw_init = imapx800_nand_hw_init;
	infotm_chip->infotm_nand_cmdfifo_start = imapx800_nand_cmdfifo_start;
	infotm_chip->infotm_nand_adjust_timing = imapx800_nand_adjust_timing;
	infotm_chip->infotm_nand_options_confirm = imapx800_nand_options_confirm;
	infotm_chip->infotm_nand_cmd_ctrl = imapx800_nand_cmd_ctrl;
	infotm_chip->infotm_nand_select_chip = imapx800_nand_select_chip;
	infotm_chip->infotm_nand_get_controller = imapx800_get_controller;
	infotm_chip->infotm_nand_release_controller = imapx800_release_controller;
	infotm_chip->infotm_nand_read_byte = imapx800_nand_read_byte;
	infotm_chip->infotm_nand_write_byte = imapx800_nand_write_byte;
	infotm_chip->infotm_nand_dma_read = imapx800_nand_dma_read;
	infotm_chip->infotm_nand_dma_write = imapx800_nand_dma_write;
	infotm_chip->infotm_nand_wait_devready = imapx800_nand_wait_devready;
	infotm_chip->infotm_nand_hwecc_correct = imapx800_nand_hwecc_correct;
	infotm_chip->infotm_nand_save_para = imapx800_nand_save_para;
	if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {

		chip->erase_cmd = imapx800_nand_boot_erase_cmd;
		chip->ecc.read_page = imapx800_nand_boot_read_page_hwecc;
		chip->ecc.write_page = imapx800_nand_boot_write_page_hwecc;
		chip->write_page = imapx800_nand_boot_write_page;
		plat->platform_nand_data.chip.ecclayout = &imapx800_boot_oob;
	}

	err = infotm_nand_init(infotm_chip);
	if (err)
		goto exit_error;

	if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
		chip->erase_cmd = imapx800_nand_boot_erase_cmd;
		chip->ecc.read_page = imapx800_nand_boot_read_page_hwecc;
		chip->ecc.write_page = imapx800_nand_boot_write_page_hwecc;
		chip->write_page = imapx800_nand_boot_write_page;
	} else if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
		infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
		if (infotm_nand_save->head_magic != IMAPX800_UBOOT_PARA_MAGIC)
			imapx800_nand_get_save_para(infotm_chip);
	}
	return 0;

exit_error:
	if (infotm_chip)
		kfree(infotm_chip);
	mtd->name = NULL;
	return err;
}

#ifdef CONFIG_PM
static int imapx800_nand_suspend(struct platform_device *pdev, pm_message_t pm)
{
	struct imapx800_nand_chip *imapx800_chip = to_nand_chip(pdev);
	struct infotm_nand_chip *infotm_chip = NULL;
	struct infotm_nand_platform *plat = NULL;
	struct mtd_info *mtd = NULL;
	struct list_head *l, *n;

	printk("imapx800_nand_suspend \n");
	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip = chip_list_to_imapx800(l);
		plat = infotm_chip->platform;
		if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))){
			mtd = &infotm_chip->mtd;
			if (mtd->_suspend_last)
				mtd->_suspend_last(mtd);
			infotm_chip->infotm_nand_get_controller(infotm_chip);
			clk_disable_unprepare(imapx800_chip->sys_clk);
			clk_disable_unprepare(imapx800_chip->ecc_clk);

			break;
		}
	}
	rtcbit_set("retry_reboot", 0);
	imapx800_chip->hw_inited = 0;
	return 0;
}

static int imapx800_nand_resume(struct platform_device *pdev)
{
	struct imapx800_nand_chip *imapx800_chip = to_nand_chip(pdev);
	struct infotm_nand_chip *infotm_chip = NULL;
	struct infotm_nand_platform *plat = NULL;
	struct list_head *l, *n;
	int ret = 0, i;

	module_power_on(SYSMGR_NAND_BASE);
	module_reset(SYSMGR_NAND_BASE, 0);

	if (imapx_pad_init("nand") != 0) {
		printk("nand io config error\n");
		return -ENOENT;
	}

	clk_prepare_enable(imapx800_chip->sys_clk);
	clk_prepare_enable(imapx800_chip->ecc_clk);

	if (imapx800_chip->power_gpio_index) {
		gpio_direction_output(imapx800_chip->power_gpio_index,1);
		/*
		imapx_pad_set_mode(1, 1, imapx800_chip->power_gpio_index);
		imapx_pad_set_outdat(1, 1, imapx800_chip->power_gpio_index);
		imapx_pad_set_dir(0, 1, imapx800_chip->power_gpio_index);
		*/
	}

	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip = chip_list_to_imapx800(l);
		plat = infotm_chip->platform;
		for (i = 0; i < infotm_chip->chip_num; i++)
			infotm_chip->retry_level[i] = 0;
		if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
			infotm_chip->infotm_nand_adjust_timing(infotm_chip);
			for (i = 0; i < infotm_chip->chip_num; i++) {
				infotm_chip->infotm_nand_select_chip(infotm_chip, i);
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, i);
				infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
				infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
			}
			infotm_chip->infotm_nand_release_controller(infotm_chip);
		}
	}

	return ret;
}
#endif

static void imapx800_nand_shutdown(struct platform_device *pdev)
{
	struct imapx800_nand_chip *imapx800_chip = to_nand_chip(pdev);
	struct infotm_nand_chip *infotm_chip = NULL;
	struct infotm_nand_platform *plat = NULL;
	struct mtd_info *mtd = NULL;
	struct nand_chip *chip= NULL;
	struct list_head *l, *n;
	int i;

	imapx800_chip->nand_irq_no = 0;
	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip = chip_list_to_imapx800(l);
		plat = infotm_chip->platform;
		if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
			mtd = &infotm_chip->mtd;
			if (mtd->_shutdown)
				mtd->_shutdown(mtd);
			infotm_chip->infotm_nand_get_controller(infotm_chip);
			for (i = 0; i < infotm_chip->chip_num; i++) {
				if (infotm_chip->valid_chip[i]) {
					infotm_chip->infotm_nand_select_chip(infotm_chip, i);
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, i);
					infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
					infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
				}
			}
			rtcbit_set("retry_reboot", ((infotm_chip->retry_level[0] & 0xf) | ((infotm_chip->retry_level[1] & 0xf) << 4)));
			printk("imapx800_nand_shutdown %d %d \n", infotm_chip->retry_level[0], infotm_chip->retry_level[1]);
			clk_disable_unprepare(imapx800_chip->sys_clk);
			clk_disable_unprepare(imapx800_chip->ecc_clk);

			if (imapx_pad_init("nand") != 0)
				printk("nand shutdown io config error\n");
			module_power_down(SYSMGR_NAND_BASE);

			break;
		} else if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
			mtd = &infotm_chip->mtd;
			chip = &infotm_chip->chip;
			imapx800_nand_check_uboot0_status(mtd, chip);
		}
	}

	return;
}

static int imapx800_nand_probe(struct platform_device *pdev)
{
	struct infotm_nand_device *infotm_nand_dev = to_nand_dev(pdev);
	struct imapx800_nand_chip *imapx800_chip = NULL;
	struct infotm_nand_platform *plat = NULL;
	struct device *dev = &pdev->dev;
	int err = 0, i;

	dev_dbg(&pdev->dev, "(%p)\n", pdev);

	if (infotm_nand_dev == NULL) {
		dev_err(&pdev->dev, "no platform specific information\n");
		err = -ENOMEM;
		goto exit_error;
	}

	imapx800_chip = kzalloc(sizeof(struct imapx800_nand_chip), GFP_KERNEL);
	if (imapx800_chip == NULL) {
		printk("no memory for flash info\n");
		err = -ENOMEM;
		goto exit_error;
	}
	imapx800_chip->resved_mem = kzalloc(RESERVED_RRTB_SIZE, GFP_KERNEL);
	if (imapx800_chip->resved_mem == NULL) {
		err = -ENOMEM;
		goto exit_error;
	}

	imapx800_chip->nand_irq_no = platform_get_irq(pdev, 0);
	if (imapx800_chip->nand_irq_no < 0) {
		dev_err(&pdev->dev, "imapx800 get nand IRQ No failed\n");
		return -ENOENT;
	}

	err = request_irq(imapx800_chip->nand_irq_no, imapx800_nand_irq, IRQF_DISABLED, "imapx800 nand", imapx800_chip);
	if (err) {
		dev_err(&pdev->dev, "imapx800 IRQ%d error %d\n", imapx800_chip->nand_irq_no, err);
		return err;
	}

	imapx800_chip->regs = ioremap(pdev->resource->start, pdev->resource->end - pdev->resource->start + 1);
	platform_set_drvdata(pdev, imapx800_chip);
	dev->coherent_dma_mask = DMA_BIT_MASK(32);
	spin_lock_init(&imapx800_chip->controller_lock);
	init_waitqueue_head(&imapx800_chip->wq);
	init_waitqueue_head(&imapx800_chip->irq_wq);
	imapx800_chip->controller_state = CONTROLLER_READY;
	INIT_LIST_HEAD(&imapx800_chip->chip_list);

	imapx800_chip->nand_data_buf = dma_alloc_coherent(dev, MAX_DMA_DATA_SIZE, &imapx800_chip->data_dma_addr, GFP_KERNEL);
	if (imapx800_chip->nand_data_buf == NULL) {
		printk("no memory for flash data buf\n");
		err = -ENOMEM;
		goto exit_error;
	}

	imapx800_chip->sys_info_buf = dma_alloc_coherent(dev, MAX_DMA_OOB_SIZE, &imapx800_chip->oob_dma_addr, GFP_KERNEL);
	if (imapx800_chip->sys_info_buf == NULL) {
		printk("no memory for flash info buf\n");
		err = -ENOMEM;
		goto exit_error;
	}

	for (i=0; i<infotm_nand_dev->dev_num; i++) {
		plat = &infotm_nand_dev->infotm_nand_platform[i];
		if (plat == NULL) {
			printk("error for not platform data\n");
			continue;
		}
		err = infotm_nand_probe(plat, imapx800_chip);
		if (err) {
			printk("%s dev probe failed %d\n", plat->name, err);
			continue;
		}
	}

	return 0;

exit_error:
	if (imapx800_chip->sys_info_buf) {
		dma_free_coherent(dev, MAX_DMA_OOB_SIZE, imapx800_chip->sys_info_buf, (dma_addr_t)imapx800_chip->oob_dma_addr);
		imapx800_chip->sys_info_buf = NULL;
	}

	if (imapx800_chip->nand_data_buf) {
		dma_free_coherent(dev, MAX_DMA_DATA_SIZE, imapx800_chip->nand_data_buf, (dma_addr_t)imapx800_chip->data_dma_addr);
		imapx800_chip->nand_data_buf = NULL;
	}
	return err;
}

static int imapx800_nand_remove(struct platform_device *pdev)
{
	struct infotm_nand_device *infotm_nand_dev = to_nand_dev(pdev);
	struct infotm_nand_platform *plat = NULL;
	struct infotm_nand_chip *infotm_chip = NULL;
	struct mtd_info *mtd = NULL;
	int i;

	platform_set_drvdata(pdev, NULL);
	for (i=0; i<infotm_nand_dev->dev_num; i++) {
		infotm_chip = plat->infotm_chip;
		if (infotm_chip) {
			mtd = &infotm_chip->mtd;
			if (mtd) {
				nand_release(mtd);
				kfree(mtd);
			}

			kfree(infotm_chip);
		}
	}

	return 0;
}

#define DRV_NAME	"infotm_imapx15_nand"
#define DRV_VERSION	"FTL V4.0 & NAND V5.0 2014.4.10"
#define DRV_AUTHOR	"xiaojun_yoyo"
#define DRV_DESC	"Infotm nand flash linux driver for imapx15"

/* driver device registration */
static struct platform_driver imapx800_nand_driver = {
	.probe		= imapx800_nand_probe,
	.remove		= imapx800_nand_remove,
	.shutdown	= imapx800_nand_shutdown,
#ifdef CONFIG_PM
	.suspend	= imapx800_nand_suspend,
	.resume		= imapx800_nand_resume,
#endif
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init imapx800_nand_init(void)
{
	printk(KERN_ALERT "%s, Version %s (c) 2014 InfotmIC.\n", DRV_DESC, DRV_VERSION);

	return platform_driver_register(&imapx800_nand_driver);
}

static void __exit imapx800_nand_exit(void)
{
	platform_driver_unregister(&imapx800_nand_driver);
}

module_init(imapx800_nand_init);
module_exit(imapx800_nand_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_ALIAS("platform:" DRV_NAME);

