
/*
 * Infotm nftl ops
 *
 * (C) 2013 2
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <linux/leds.h>

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>

#include <linux/mtd/blktrans.h>
#include <linux/mutex.h>

#include "infotm_nftl.h"

static int infotm_ops_read_page(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, 
								unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct mtd_oob_ops infotm_oob_ops;
	loff_t from;
	size_t len, retlen;
	int ret, mtd_ecc_corrected;

	mtd_ecc_corrected = mtd->ecc_stats.corrected;
	from = mtd->erasesize;
	from *= blk_addr;
	from += page_addr * mtd->writesize;

	len = mtd->writesize;
	infotm_oob_ops.mode = MTD_OOB_AUTO;
	infotm_oob_ops.len = mtd->writesize;
	infotm_oob_ops.ooblen = oob_len;
	infotm_oob_ops.ooboffs = 0;
	infotm_oob_ops.datbuf = data_buf;
	infotm_oob_ops.oobbuf = nftl_oob_buf;

	if (nftl_oob_buf)
		ret = mtd->_read_oob(mtd, from, &infotm_oob_ops);
	else
		ret = mtd->_read(mtd, from, len, &retlen, data_buf);

	if (ret == -EUCLEAN) {
		//if (mtd->ecc_stats.corrected - mtd_ecc_corrected)
			//ret = EUCLEAN;
		ret = 0;
	}
	if (ret < 0)
		infotm_nftl_dbg("infotm_ops_read_page failed: %llx %d %x\n", from, blk_addr, page_addr);

	return ret;
}

static int infotm_ops_write_page(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, 
								unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct mtd_oob_ops infotm_oob_ops;
	loff_t from;
	size_t len, retlen;
	int ret;

	from = mtd->erasesize;
	from *= blk_addr;
	from += page_addr * mtd->writesize;

	len = mtd->writesize;
	infotm_oob_ops.mode = MTD_OOB_AUTO;
	infotm_oob_ops.len = mtd->writesize;
	infotm_oob_ops.ooblen = oob_len;
	infotm_oob_ops.ooboffs = 0;
	infotm_oob_ops.datbuf = data_buf;
	infotm_oob_ops.oobbuf = nftl_oob_buf;

	if (nftl_oob_buf)
		ret = mtd->_write_oob(mtd, from, &infotm_oob_ops);
	else
		ret = mtd->_write(mtd, from, len, &retlen, data_buf);

	return ret;
}

static int infotm_ops_read_page_oob(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr, 
										addr_page_t page_addr, unsigned char *nftl_oob_buf, int oob_len)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct mtd_oob_ops infotm_oob_ops;
	struct mtd_ecc_stats stats;
	loff_t from;
	int ret;

	stats = mtd->ecc_stats;
	from = mtd->erasesize;
	from *= blk_addr;
	from += page_addr * mtd->writesize;

	infotm_oob_ops.mode = MTD_OOB_AUTO;
	infotm_oob_ops.len = 0;
	infotm_oob_ops.ooblen = oob_len;
	infotm_oob_ops.ooboffs = 0;
	infotm_oob_ops.datbuf = NULL;
	infotm_oob_ops.oobbuf = nftl_oob_buf;

	ret = mtd->_read_oob(mtd, from, &infotm_oob_ops);

	if (ret == -EUCLEAN) {
		//if (mtd->ecc_stats.corrected >= 10)
			//do read err 
		ret = 0;
	}

	return ret;
}

static int infotm_ops_blk_isbad(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	loff_t from;

	from = mtd->erasesize;
	from *= blk_addr;

	return mtd->_block_isbad(mtd, from);
}

static int infotm_ops_blk_mark_bad(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	loff_t from;

	from = mtd->erasesize;
	from *= blk_addr;

	return mtd->_block_markbad(mtd, from);
}

static int infotm_ops_erase_block(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct erase_info infotm_nftl_erase_info;

	memset(&infotm_nftl_erase_info, 0, sizeof(struct erase_info));
	infotm_nftl_erase_info.mtd = mtd;
	infotm_nftl_erase_info.addr = mtd->erasesize;
	infotm_nftl_erase_info.addr *= blk_addr;
	infotm_nftl_erase_info.len = mtd->erasesize;

	return mtd->_erase(mtd, &infotm_nftl_erase_info);
}

void infotm_nftl_ops_init(struct infotm_nftl_info_t *infotm_nftl_info)
{
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_malloc(sizeof(struct infotm_nftl_ops_t));
	if (!infotm_nftl_ops)
		return;

	infotm_nftl_info->infotm_nftl_ops = infotm_nftl_ops;
	infotm_nftl_ops->read_page = infotm_ops_read_page;
	infotm_nftl_ops->write_page = infotm_ops_write_page;
	infotm_nftl_ops->read_page_oob = infotm_ops_read_page_oob;
	infotm_nftl_ops->blk_isbad = infotm_ops_blk_isbad;
	infotm_nftl_ops->blk_mark_bad = infotm_ops_blk_mark_bad;
	infotm_nftl_ops->erase_block = infotm_ops_erase_block;

	return;
}

