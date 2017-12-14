#include <common.h>
#include <command.h>
#include <watchdog.h>
#include <malloc.h>
#include <cpuid.h>
#include <rballoc.h>
#include <nand.h>
#include <storage.h>
#include <preloader.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/bitops.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/blktrans.h>

#include <asm/errno.h>
#include <asm/io.h>
#include <asm/sizes.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <rballoc.h>
#include <asm/arch/nand.h>

#define FND_TRANS_NAME  "nandblk"
extern struct part_sz default_cfg[];

int bnd_vs_align(void) 
{
	nand_info_t *nand;
	nand = get_mtd_device_nm(NAND_BOOT_NAME);
	if (IS_ERR(nand))
		return 0;
	return nand->writesize;
}

int bnd_vs_read(uint8_t *buf, loff_t start, int length, int extra)
{
	nand_info_t *nand;
	size_t len = length;
	size_t retlen;

	nand = get_mtd_device_nm(NAND_BOOT_NAME);
	if (IS_ERR(nand))
		return 0;

	nand->read(nand, start, len, &retlen, buf);
	return retlen;
}

int bnd_vs_write(uint8_t *buf, loff_t start, int length, int extra)
{
	nand_info_t *nand;
	size_t len = length;
	size_t retlen;

	nand = get_mtd_device_nm(NAND_BOOT_NAME);
	if (IS_ERR(nand))
		return 0;

	/*add for compate new irom V2.1*/
	if (IROM_IDENTITY == IX_CPUID_X15_NEW && start == 0)
		memcpy(buf, buf + IMAPX800_PARA_SAVE_SIZE, IMAPX800_UBOOT0_SIZE);

	nand->write(nand, start, len, &retlen, buf);
	return retlen;
}

int bnd_vs_reset(void)
{
	return 0;
}

int bnd_vs_erase(loff_t start, uint64_t size)
{
	struct erase_info bnd_erase_info;
	nand_info_t *nand;
	size_t len = size;

	nand = get_mtd_device_nm(NAND_BOOT_NAME);
	if (IS_ERR(nand))
		return 0;

	memset(&bnd_erase_info, 0, sizeof(struct erase_info));
	bnd_erase_info.mtd = nand;
	bnd_erase_info.addr = start;
	bnd_erase_info.len = len;

	return nand->erase(nand, &bnd_erase_info);
}

int bnd_vs_isbad(loff_t offset)
{
	nand_info_t *nand;
	nand = get_mtd_device_nm(NAND_BOOT_NAME);
	if (IS_ERR(nand))
		return 0;
	return nand->block_isbad(nand, offset);
}

int nnd_vs_align(void) 
{
	nand_info_t *nand;
	nand = get_mtd_device_nm(NAND_NORMAL_NAME);
	if (IS_ERR(nand))
		return 0;
	return nand->writesize;
}

int nnd_vs_read(uint8_t *buf, loff_t start, int length, int extra) 
{
	nand_info_t *nand;
	size_t retlen;

	nand = get_mtd_device_nm(NAND_NORMAL_NAME);
	if (IS_ERR(nand))
		return 0;

	nand->read(nand, start, length, &retlen, buf);
	return retlen;
}

int nnd_vs_write(uint8_t *buf, loff_t start, int length, int extra)
{
	nand_info_t *nand;
	size_t retlen;

	nand = get_mtd_device_nm(NAND_NORMAL_NAME);
	if (IS_ERR(nand))
		return 0;

	nand->write(nand, start, length, &retlen, buf);
	return retlen;
}

int nnd_vs_reset(void)
{
	nand_info_t *nand;
	struct infotm_nand_chip *infotm_chip;
	struct imapx800_nand_chip *imapx800_chip;
	struct infotm_nand_para_save *infotm_nand_save;

	nand = get_mtd_device_nm(NAND_NORMAL_NAME);
	if (IS_ERR(nand)) {
		printk("nnd dev not found for reset\n");
		return 0;
	}

	infotm_chip = mtd_to_nand_chip(nand);
	imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;

	return (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC) ? 0 : 1;
}

int nnd_vs_erase(loff_t start, uint64_t size)
{
	struct erase_info nnd_erase_info;
	nand_info_t *nand;

	nand = get_mtd_device_nm(NAND_NORMAL_NAME);
	if (IS_ERR(nand)) {
		printk("nnd dev not found for erase\n");
		return 0;
	}

	memset(&nnd_erase_info, 0, sizeof(struct erase_info));
	nnd_erase_info.mtd = nand;
	nnd_erase_info.addr = start;
	nnd_erase_info.len = nand->erasesize;
	return nand->erase(nand, &nnd_erase_info);
}

int nnd_vs_scrub(loff_t start, uint64_t size)
{
	nand_erase_options_t nnd_erase_info;
	nand_info_t *nand;
	size_t len = size;
	nand = get_mtd_device_nm(NAND_NORMAL_NAME);
	if (IS_ERR(nand))
		return 0;

	memset(&nnd_erase_info, 0, sizeof(nand_erase_options_t));
	nnd_erase_info.offset = start;
	nnd_erase_info.length = len;
	nnd_erase_info.scrub = 1;

	return nand_erase_opts(nand, &nnd_erase_info);
}

int nnd_vs_isbad(loff_t offset)
{
	nand_info_t *nand;

	nand = get_mtd_device_nm(NAND_NORMAL_NAME);
	if (IS_ERR(nand))
		return 0;

	return nand->block_isbad(nand, offset);
}

int fnd_vs_align(void) 
{
	struct mtd_blktrans_ops *fnd_tr = NULL;

	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
		return 0;

	return fnd_tr->blksize;
}

uint32_t fnd_align(void)
{
	struct mtd_blktrans_ops *fnd_tr = NULL;

	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
		return 0;

	return fnd_tr->blksize;
}

int fnd_size_match(uint32_t size)
{
	struct mtd_blktrans_ops *fnd_tr = NULL;

	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
		return 0;

	return fnd_tr->blksize;
}

int fnd_vs_read(uint8_t *buf, loff_t start, int length, int extra)
{
	struct mtd_blktrans_ops *fnd_tr = NULL;
	size_t len = length;
	size_t retlen;

	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
		return 0;

	return fnd_tr->read(fnd_tr, start, len, &retlen, buf);
}

int fnd_vs_write(uint8_t *buf, loff_t start, int length, int extra)
{
	struct mtd_blktrans_ops *fnd_tr = NULL;
	size_t len = length;
	size_t retlen;

	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
		return 0;

	return fnd_tr->write(fnd_tr, start, len, &retlen, buf);
}

int fnd_vs_reset(void)
{
	struct mtd_blktrans_ops *fnd_tr = NULL;

	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
        return 1;

	fnd_tr->reset(fnd_tr);
	return 0;
}

int fnd_vs_scrub(loff_t start, uint64_t size)
{
	nand_erase_options_t fnd_erase_info;
	nand_info_t *nand;
	uint64_t len = size;

	nand = get_mtd_device_nm(NAND_NFTL_NAME);
	if (IS_ERR(nand))
		return 0;

	memset(&fnd_erase_info, 0, sizeof(nand_erase_options_t));
	fnd_erase_info.offset = start;
	fnd_erase_info.length = len;
	fnd_erase_info.scrub = 1;
	return nand_erase_opts(nand, &fnd_erase_info);
}

int fnd_vs_erase(loff_t start, uint64_t size)
{
	struct mtd_blktrans_ops *fnd_tr = NULL;
	struct erase_info nnd_erase_info;
	uint64_t len = size;
	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
		return 0;

	memset(&nnd_erase_info, 0, sizeof(struct erase_info));
	nnd_erase_info.addr = start;
	nnd_erase_info.len = len;

	return fnd_tr->erase(fnd_tr, &nnd_erase_info);
}

int fnd_vs_isbad(loff_t offset)
{
	return 0;
}

uint32_t fnd_size_shrink(uint32_t size)
{
	struct mtd_blktrans_ops *fnd_tr = NULL;

	fnd_tr = get_mtd_blktrans_ops_nm(FND_TRANS_NAME);
	if (!fnd_tr)
		return 0;

	return (((size + fnd_tr->blksize - 1) / fnd_tr->blksize) * fnd_tr->blksize);
}

