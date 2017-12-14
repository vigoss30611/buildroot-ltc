
/*
 * Infotm nftl core
 *
 * (C) 2013 2
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/crc32.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/blktrans.h>
#include <linux/mutex.h>

#include "infotm_nftl.h"
#include <mach/nand.h>
#include <mach/rballoc.h>

#if 0
static void infotm_nftl_move_bad_block(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t phy_blk_addr)
{
    struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
    struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
    struct phyblk_node_t *phy_blk_src_node, *phy_blk_node_dest;
    struct vtblk_node_t  *vt_blk_node;
    addr_blk_t phy_dest_blk, vt_blk_addr;
	int status = 0, oob_len, i;
	addr_page_t dest_page = 0, src_page = 0;
	unsigned char *nftl_data_buf;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;

    status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_dest_blk);
    if (status) {
        infotm_nftl_dbg("infotm nftl move blk need garbage: %d \n", phy_blk_addr);
        status = infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
        if (status == 0) 
            return;

        status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_dest_blk);
        if (status) 
            return;
    }

	oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
	nftl_data_buf = infotm_nftl_info->copy_page_buf;
    phy_blk_src_node = &infotm_nftl_info->phypmt[phy_blk_addr];
    phy_blk_node_dest = &infotm_nftl_info->phypmt[phy_dest_blk];
    vt_blk_addr = phy_blk_src_node->vtblk;
    phy_blk_node_dest->vtblk = vt_blk_addr;
    phy_blk_node_dest->timestamp = phy_blk_src_node->timestamp;
    phy_blk_node_dest->status_page = phy_blk_src_node->status_page;
    infotm_nftl_dbg("infotm nftl move blk: %d %x\n", phy_blk_addr, phy_blk_src_node->last_write);
    for(i=0; i<(phy_blk_src_node->last_write+1); i++) {

    	status = infotm_nftl_info->read_page(infotm_nftl_info, phy_blk_addr, src_page, nftl_data_buf, nftl_oob_buf, oob_len);
    	if (status < 0) {
    		infotm_nftl_dbg("move blk read failed: %d %x status: %d\n", phy_blk_addr, src_page, status);
    		src_page++;
    		continue;
    	}
    	src_page++;

    	if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC))) {
    		if (memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC))) {
    			infotm_nftl_dbg("move blk invalid magic vtblk: %d %d %x \n", nftl_oob_info->vtblk, phy_blk_addr, src_page);
    		}
    	}

    	nftl_oob_info->ec = phy_blk_node_dest->ec;
    	nftl_oob_info->timestamp = phy_blk_src_node->timestamp;
    	nftl_oob_info->status_page = 1;
    	infotm_nftl_info->write_page(infotm_nftl_info, phy_dest_blk, dest_page, nftl_data_buf, nftl_oob_buf, oob_len);
    	dest_page++;
    }

    status = infotm_nftl_ops->erase_block(infotm_nftl_info, phy_blk_addr);
    if (status) {
    	memset((unsigned char *)phy_blk_src_node, 0xff, sizeof(struct phyblk_node_t));
    	phy_blk_src_node->status_page = STATUS_BAD_BLOCK;
    	infotm_nftl_wl->add_bad(infotm_nftl_wl, phy_blk_addr);
        infotm_nftl_ops->blk_mark_bad(infotm_nftl_info, phy_blk_addr);	
    } else {
    	phy_blk_src_node->ec++;
    	phy_blk_src_node->valid_sects = 0;
    	phy_blk_src_node->vtblk = BLOCK_INIT_VALUE;
    	phy_blk_src_node->last_write = BLOCK_INIT_VALUE;
    	phy_blk_src_node->timestamp = MAX_TIMESTAMP_NUM;
    	memset(phy_blk_src_node->phy_page_map, 0xff, (sizeof(addr_sect_t)*MAX_PAGES_IN_BLOCK));
    	memset(phy_blk_src_node->phy_page_delete, 0xff, (MAX_PAGES_IN_BLOCK>>3));
        infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_addr); 
    }

    infotm_nftl_wl->add_used(infotm_nftl_wl, phy_dest_blk);
    vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_addr));
	while (vt_blk_node != NULL) {
		if (vt_blk_node->phy_blk_addr == phy_blk_addr) {
			vt_blk_node->phy_blk_addr = phy_dest_blk;
			break;
		}
		vt_blk_node = vt_blk_node->next;
	}
    return;
}
#endif

int infotm_nftl_check_node(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr)
{
	struct infotm_nftl_wl_t *infotm_nftl_wl;
	struct phyblk_node_t *phy_blk_node;
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_prev, *vt_blk_node_free;
	addr_blk_t phy_blk_num, vt_blk_num;
	int16_t *valid_page;
	int k, node_length, node_len_actual, status = 0;

	infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	vt_blk_num = blk_addr;
	vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
	if (vt_blk_node == NULL)
		return -ENOMEM;
    node_len_actual = infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node);
    valid_page = infotm_nftl_malloc(sizeof(int16_t)*node_len_actual);
    if (valid_page == NULL)
        return -ENOMEM;

#if 0
	node_len_max = (MAX_BLK_NUM_PER_NODE + 1);
	node_len_actual = infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node);
	if (node_len_actual > node_len_max) {
		node_length = 0;
		while (vt_blk_node != NULL) {
			node_length++;
			if ((vt_blk_node != NULL) && (node_length > node_len_max)) {
				vt_blk_node_free = vt_blk_node->next;
				if (vt_blk_node_free != NULL) {
					infotm_nftl_wl->add_free(infotm_nftl_wl, vt_blk_node_free->phy_blk_addr);
					vt_blk_node->next = vt_blk_node_free->next;
					infotm_nftl_free(vt_blk_node_free);
					continue;
				}
			}
			vt_blk_node = vt_blk_node->next;
		}
	}
#endif

	memset((unsigned char *)valid_page, 0x0, sizeof(int16_t)*node_len_actual);
	for (k=0; k<infotm_nftl_info->pages_per_blk; k++) {

		node_length = 0;
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		while (vt_blk_node != NULL) {

			phy_blk_num = vt_blk_node->phy_blk_addr;
			phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_num];
			if (!(phy_blk_node->phy_page_delete[k>>3] & (1 << (k % 8))))
				break;
			infotm_nftl_info->get_phy_sect_map(infotm_nftl_info, phy_blk_num);
			if ((phy_blk_node->phy_page_map[k] >= 0) && (phy_blk_node->phy_page_delete[k>>3] & (1 << (k % 8)))) {
				valid_page[node_length]++;
				break;
			}
			node_length++;
			vt_blk_node = vt_blk_node->next;
		}
	}

	//infotm_nftl_dbg("NFTL check node logic blk: %d  %d %d %d %d %d\n", vt_blk_num, valid_page[0], valid_page[1], valid_page[2], valid_page[3], valid_page[4]);
	node_length = 0;
	vt_blk_node_prev = NULL;
	vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
	while (vt_blk_node != NULL) {

		if (valid_page[node_length] == 0) {
			infotm_nftl_wl->add_free(infotm_nftl_wl, vt_blk_node->phy_blk_addr);
			if (vt_blk_node == *(infotm_nftl_info->vtpmt + vt_blk_num))
				*(infotm_nftl_info->vtpmt + vt_blk_num) = vt_blk_node->next;
			else if (vt_blk_node_prev != NULL)
				vt_blk_node_prev->next = vt_blk_node->next;

			vt_blk_node_free = vt_blk_node;
			vt_blk_node = vt_blk_node->next;
			node_length++;
			infotm_nftl_free(vt_blk_node_free);
			continue;
		}

		vt_blk_node_prev = vt_blk_node;
		vt_blk_node = vt_blk_node->next;
		node_length++;
	}

#if 0
	vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
	phy_blk_num = vt_blk_node->phy_blk_addr;
	phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_num];
	node_length = infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node);
	if ((node_length == node_len_max) && (valid_page[node_len_max-1] == (infotm_nftl_info->pages_per_blk - (phy_blk_node->last_write + 1)))) {
		status = INFOTM_NFTL_STRUCTURE_FULL;
	}
#endif
	infotm_nftl_free(valid_page);

	return status;
}

static void infotm_nftl_update_sectmap(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t phy_blk_addr, addr_page_t logic_page_addr, addr_page_t phy_page_addr)
{
	struct phyblk_node_t *phy_blk_node;
	phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];

	phy_blk_node->valid_sects++;
	phy_blk_node->phy_page_map[logic_page_addr] = phy_page_addr;
	phy_blk_node->phy_page_delete[logic_page_addr>>3] |= (1 << (logic_page_addr % 8));
	phy_blk_node->last_write = phy_page_addr;
}

static int infotm_nftl_write_page(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr,
								unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len)
{
	int status;
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;	

	status = infotm_nftl_ops->write_page(infotm_nftl_info, blk_addr, page_addr, data_buf, nftl_oob_buf, oob_len);
	if (status)
	    return status;

	infotm_nftl_update_sectmap(infotm_nftl_info, blk_addr, nftl_oob_info->sect, page_addr);
	return 0;
}

static int infotm_nftl_read_page(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr,
								unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len)
{
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;

	return infotm_nftl_ops->read_page(infotm_nftl_info, blk_addr, page_addr, data_buf, nftl_oob_buf, oob_len);
}

static int infotm_nftl_copy_page(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t dest_blk_addr, addr_page_t dest_page, 
				addr_blk_t src_blk_addr, addr_page_t src_page)
{
	int status = 0, oob_len;
	unsigned char *nftl_data_buf;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	struct phyblk_node_t *phy_blk_node = &infotm_nftl_info->phypmt[dest_blk_addr];

	oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
	nftl_data_buf = infotm_nftl_info->copy_page_buf;
	status = infotm_nftl_info->read_page(infotm_nftl_info, src_blk_addr, src_page, nftl_data_buf, nftl_oob_buf, oob_len);
	if (status < 0) {
		infotm_nftl_dbg("copy page read failed: %d %d status: %d\n", src_blk_addr, src_page, status);
		status = INFOTM_NFTL_FAILURE;
		goto exit;
	}

	if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC))) {
		if (memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC))) {
			infotm_nftl_dbg("nftl read invalid magic vtblk: %d %d %x \n", phy_blk_node->vtblk, src_blk_addr, src_page);
		}
	}
	nftl_oob_info->ec = phy_blk_node->ec;
	nftl_oob_info->timestamp = phy_blk_node->timestamp;
	nftl_oob_info->status_page = 1;
	status = infotm_nftl_info->write_page(infotm_nftl_info, dest_blk_addr, dest_page, nftl_data_buf, nftl_oob_buf, oob_len);
	if (status) {
		infotm_nftl_dbg("copy page write failed: %d status: %d\n", dest_blk_addr, status);
		status = INFOTM_NFTL_UNWRITTEN_SECTOR;
		goto exit;
	}

exit:
	return status;
}

static int infotm_nftl_get_page_info(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, unsigned char *nftl_oob_buf, int oob_len)
{
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;

	return infotm_nftl_ops->read_page_oob(infotm_nftl_info, blk_addr, page_addr, nftl_oob_buf, oob_len);	
}

static int infotm_nftl_update_bbt(struct infotm_nftl_info_t *infotm_nftl_info)
{
	struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_bbt_info *nand_bbt_info = &infotm_nftl_info->nand_bbt_info;
	struct phyblk_node_t *phy_blk_node;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	int error = 0, status, oob_len, len;
	size_t amount_saved = 0;
	addr_blk_t phy_blk_addr, phy_page_addr;

	if (infotm_nand_save->bbt_blk[0] < 0) {
		status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
		if (status) {
			status = infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
			if (status == 0) {
				infotm_nftl_dbg("nftl couldn`t found free block: %d %d\n", infotm_nftl_wl->free_root.count, infotm_nftl_wl->wait_gc_block);
				return -ENOENT;
			}
			status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
			if (status)
				return status;
		}
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
		infotm_nand_save->bbt_blk[0] = phy_blk_addr;
		infotm_nftl_info->para_rewrite_flag = 1;
		phy_blk_node->timestamp++;
		phy_blk_node->ec++;
		phy_blk_node->vtblk = BBT_VTBLK;
	} else {
		oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
		phy_blk_addr = infotm_nand_save->bbt_blk[0];
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
		infotm_nftl_info->get_phy_sect_map(infotm_nftl_info, phy_blk_addr);
		if (((sizeof(struct infotm_nand_bbt_info) + mtd->writesize - 1) / mtd->writesize) > (infotm_nftl_info->pages_per_blk - phy_blk_node->last_write - 1)) {
			status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
			if (status) {
				status = infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
				if (status == 0) {
					infotm_nftl_dbg("nftl couldn`t found free block: %d %d\n", infotm_nftl_wl->free_root.count, infotm_nftl_wl->wait_gc_block);
					return -ENOENT;
				}
				status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
				if (status)
					return status;
			}
			phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
			infotm_nand_save->bbt_blk[0] = phy_blk_addr;
			infotm_nftl_info->para_rewrite_flag = 1;
			phy_blk_node->timestamp++;
			phy_blk_node->ec++;
			phy_blk_node->vtblk = BBT_VTBLK;
		}
	}

	nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	nftl_oob_info->ec = phy_blk_node->ec;
	nftl_oob_info->vtblk = phy_blk_node->vtblk;
	nftl_oob_info->timestamp = phy_blk_node->timestamp;
	nftl_oob_info->status_page = 1;

	oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_BBT_STRING)));
	if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_BBT_STRING)))
		memcpy((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_BBT_STRING, strlen(INFOTM_BBT_STRING));

	while (amount_saved < sizeof(struct infotm_nand_bbt_info)) {

		nftl_oob_info->sect = amount_saved / mtd->writesize;
		phy_page_addr = phy_blk_node->last_write + 1;
		len = min(mtd->writesize, sizeof(struct infotm_nand_bbt_info) - amount_saved);
		memcpy(infotm_nftl_info->copy_page_buf, (unsigned char *)nand_bbt_info + amount_saved, len);
		status = infotm_nftl_info->write_page(infotm_nftl_info, phy_blk_addr, phy_page_addr, infotm_nftl_info->copy_page_buf, nftl_oob_buf, oob_len);
		if (status) {
			infotm_nftl_dbg("nftl update bbt faile blk: %d page: %d status: %d\n", phy_blk_addr, phy_page_addr, status);
			return status;
		}

		amount_saved += mtd->writesize;
	}

	return error;
}

static int infotm_nftl_blk_isbad(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr)
{
	struct infotm_nand_bbt_info *nand_bbt_info = &infotm_nftl_info->nand_bbt_info;
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
	struct nand_bbt_t *nand_bbt;
	int j;

	if ((!memcmp(nand_bbt_info->bbt_head_magic, BBT_HEAD_MAGIC, 4)) && (!memcmp(nand_bbt_info->bbt_tail_magic, BBT_TAIL_MAGIC, 4))) {
		for (j=0; j<nand_bbt_info->total_bad_num; j++) {
			nand_bbt = &nand_bbt_info->nand_bbt[j];
			if (blk_addr == nand_bbt->blk_addr) {
				printk("infotm nftl bbt detect bad blk %d %d %d \n", nand_bbt->blk_addr, nand_bbt->chip_num, nand_bbt->bad_type);
				return EFAULT;
			}
		}
	} else {
		return infotm_nftl_ops->blk_isbad(infotm_nftl_info, blk_addr);
	}

	return 0;
}

static int infotm_nftl_blk_mark_bad(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr)
{
    struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	struct infotm_nand_bbt_info *nand_bbt_info = &infotm_nftl_info->nand_bbt_info;
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
	struct nand_bbt_t *nand_bbt;
	struct phyblk_node_t *phy_blk_node = &infotm_nftl_info->phypmt[blk_addr];
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_prev;
	int j;

	if (phy_blk_node->vtblk >= 0) {
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + phy_blk_node->vtblk));
		do {
			vt_blk_node_prev = vt_blk_node;
			vt_blk_node = vt_blk_node->next;

			if ((vt_blk_node != NULL) && (vt_blk_node->phy_blk_addr == blk_addr)) {
				vt_blk_node_prev->next = vt_blk_node->next;
				infotm_nftl_free(vt_blk_node);
				break;
			} else if ((vt_blk_node_prev != NULL) && (vt_blk_node_prev->phy_blk_addr == blk_addr)) {
				if (*(infotm_nftl_info->vtpmt + phy_blk_node->vtblk) == vt_blk_node_prev)
					*(infotm_nftl_info->vtpmt + phy_blk_node->vtblk) = vt_blk_node;

				infotm_nftl_free(vt_blk_node_prev);
				break;
			}

		} while (vt_blk_node != NULL);
	}
	memset((unsigned char *)phy_blk_node, 0xff, sizeof(struct phyblk_node_t));
	phy_blk_node->status_page = STATUS_BAD_BLOCK;
    infotm_nftl_wl->add_bad(infotm_nftl_wl, blk_addr);

	if ((!memcmp(nand_bbt_info->bbt_head_magic, BBT_HEAD_MAGIC, 4)) && (!memcmp(nand_bbt_info->bbt_tail_magic, BBT_TAIL_MAGIC, 4))) {
		for (j=0; j<nand_bbt_info->total_bad_num; j++) {
			nand_bbt = &nand_bbt_info->nand_bbt[j];
			if (blk_addr == nand_bbt->blk_addr)
				break;
		}
		if (j >= nand_bbt_info->total_bad_num) {

			nand_bbt = &nand_bbt_info->nand_bbt[nand_bbt_info->total_bad_num++];
			nand_bbt->blk_addr = blk_addr;
			nand_bbt->plane_num = 0;
			nand_bbt->chip_num = 1;
			nand_bbt->bad_type = USELESS_BAD;

			if (infotm_nftl_update_bbt(infotm_nftl_info))
				infotm_nftl_dbg("update bbt failed %d \n", blk_addr);
		}
	} else {
		return infotm_nftl_ops->blk_mark_bad(infotm_nftl_info, blk_addr);
	}

	return 0;
}

static int infotm_nftl_creat_sectmap(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t phy_blk_addr)
{
	int i, status, sect_test = 0, oob_len;
	uint32_t page_per_blk;
	int16_t valid_sects = 0;
	struct phyblk_node_t *phy_blk_node;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info;
	phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
	nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;

    oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
	page_per_blk = infotm_nftl_info->pages_per_blk;
	for (i=0; i<page_per_blk; i++) {

		if (i == 0) {
    		status = infotm_nftl_info->get_page_info(infotm_nftl_info, phy_blk_addr, i, nftl_oob_buf, oob_len);
    		if (status) {
                infotm_nftl_dbg("nftl creat sect map faile at:%d  %d %d\n", phy_blk_node->vtblk, phy_blk_addr, i);
                continue;
    		}		    
			phy_blk_node->ec = nftl_oob_info->ec;
			phy_blk_node->vtblk = nftl_oob_info->vtblk;
			phy_blk_node->timestamp = nftl_oob_info->timestamp;
		} else {
            status = infotm_nftl_info->get_page_info(infotm_nftl_info, phy_blk_addr, i, nftl_oob_buf, oob_len);
    		if (status) {
        		infotm_nftl_dbg("nftl creat sect map faile at: %d %d %d\n", phy_blk_node->vtblk, phy_blk_addr, i);
        		continue;
    		}		    
		}

    	if (((phy_blk_node->vtblk >= 0) && (phy_blk_node->vtblk < infotm_nftl_info->accessibleblocks)) 
    		&& (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)))) {
    		if ((nftl_oob_info->sect >= 0) && memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC))) {
    			infotm_nftl_dbg("nftl sect invalid magic: %d %d %x \n", nftl_oob_info->vtblk, phy_blk_addr, i);
    		}
    	}

		if (nftl_oob_info->sect >= 0) {
			phy_blk_node->phy_page_map[nftl_oob_info->sect] = i;
			phy_blk_node->last_write = i;
			valid_sects++;
			if (sect_test == 1)
			    infotm_nftl_dbg("nftl creat map seq error: %d %d %d\n", phy_blk_node->vtblk, phy_blk_addr, i);
		} else {
			sect_test = 1;
		}
	}
	phy_blk_node->valid_sects = valid_sects;

	return 0;
}

static int infotm_nftl_get_phy_sect_map(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr)
{
	int status;
	struct phyblk_node_t *phy_blk_node;
	phy_blk_node = &infotm_nftl_info->phypmt[blk_addr];

	if (phy_blk_node->valid_sects < 0) {
		status = infotm_nftl_creat_sectmap(infotm_nftl_info, blk_addr);
		if (status)
			return INFOTM_NFTL_FAILURE;
	}

	return 0;	
}

static void infotm_nftl_erase_sect_map(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr)
{
	struct phyblk_node_t *phy_blk_node;
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_prev;
	phy_blk_node = &infotm_nftl_info->phypmt[blk_addr];

	if ((phy_blk_node->vtblk >= 0) && (phy_blk_node->vtblk < infotm_nftl_info->accessibleblocks)) {
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + phy_blk_node->vtblk));
		while (vt_blk_node != NULL) {
			vt_blk_node_prev = vt_blk_node;
			vt_blk_node = vt_blk_node->next;

			if ((vt_blk_node != NULL) && (vt_blk_node->phy_blk_addr == blk_addr)) {
				infotm_nftl_dbg("free blk had mapped vt blk: %d phy blk: %d\n", phy_blk_node->vtblk, blk_addr);
				vt_blk_node_prev->next = vt_blk_node->next;
				infotm_nftl_free(vt_blk_node);
			} else if ((vt_blk_node_prev != NULL) && (vt_blk_node_prev->phy_blk_addr == blk_addr)) {
				infotm_nftl_dbg("free blk had mapped vt blk: %d phy blk: %d\n", phy_blk_node->vtblk, blk_addr);
				if (*(infotm_nftl_info->vtpmt + phy_blk_node->vtblk) == vt_blk_node_prev)
					*(infotm_nftl_info->vtpmt + phy_blk_node->vtblk) = vt_blk_node;

				infotm_nftl_free(vt_blk_node_prev);
			}
		}
	}

	phy_blk_node->ec++;
	phy_blk_node->valid_sects = 0;
	phy_blk_node->vtblk = BLOCK_INIT_VALUE;
	phy_blk_node->last_write = BLOCK_INIT_VALUE;
	phy_blk_node->timestamp = MAX_TIMESTAMP_NUM;
	memset(phy_blk_node->phy_page_map, 0xff, (sizeof(addr_sect_t)*MAX_PAGES_IN_BLOCK));
	memset(phy_blk_node->phy_page_delete, 0xff, (MAX_PAGES_IN_BLOCK>>3));

	return;
}

static int infotm_nftl_erase_block(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr)
{
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
	int status;

	status = infotm_nftl_ops->erase_block(infotm_nftl_info, blk_addr);
	if (status)
		return INFOTM_NFTL_FAILURE;

	infotm_nftl_erase_sect_map(infotm_nftl_info, blk_addr);
	return 0;
}

static int infotm_nftl_erase_logic_block(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr)
{
    struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
    struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
    struct phyblk_node_t *phy_blk_node;
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_tmp;
	int status = 0;

	vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + blk_addr));
	if (vt_blk_node == NULL)
		return 0;

	while (vt_blk_node != NULL) {

        phy_blk_node = &infotm_nftl_info->phypmt[vt_blk_node->phy_blk_addr];
    	status = infotm_nftl_ops->erase_block(infotm_nftl_info, vt_blk_node->phy_blk_addr);
    	if (status) {
            infotm_nftl_wl->add_bad(infotm_nftl_wl, vt_blk_node->phy_blk_addr);
        	memset((unsigned char *)phy_blk_node, 0xff, sizeof(struct phyblk_node_t));
        	phy_blk_node->status_page = STATUS_BAD_BLOCK;
    	    infotm_nftl_ops->blk_mark_bad(infotm_nftl_info, vt_blk_node->phy_blk_addr);
    	} else {
    	    infotm_nftl_wl->add_erased(infotm_nftl_wl, vt_blk_node->phy_blk_addr);
    	    phy_blk_node->valid_sects = 0;
        	phy_blk_node->vtblk = BLOCK_INIT_VALUE;
        	phy_blk_node->last_write = BLOCK_INIT_VALUE;
        	phy_blk_node->timestamp = MAX_TIMESTAMP_NUM;
        	memset(phy_blk_node->phy_page_map, 0xff, (sizeof(addr_sect_t)*MAX_PAGES_IN_BLOCK));
        	memset(phy_blk_node->phy_page_delete, 0xff, (MAX_PAGES_IN_BLOCK>>3));
    	}

        vt_blk_node_tmp = vt_blk_node;
		vt_blk_node = vt_blk_node->next;
		infotm_nftl_free(vt_blk_node_tmp);
	}
	*(infotm_nftl_info->vtpmt + blk_addr) = NULL;

	return 0;
}

int infotm_nftl_add_node(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t logic_blk_addr, addr_blk_t phy_blk_addr)
{
	struct phyblk_node_t *phy_blk_node_curt = NULL, *phy_blk_node_add, *phy_blk_node_temp;
	struct vtblk_node_t  *vt_blk_node_curt, *vt_blk_node_add;
	addr_blk_t phy_blk_cur, phy_blk_num;
	int status = 0;

	vt_blk_node_add = (struct vtblk_node_t *)infotm_nftl_malloc(sizeof(struct vtblk_node_t));
	if (vt_blk_node_add == NULL)
		return INFOTM_NFTL_FAILURE;

	vt_blk_node_add->phy_blk_addr = phy_blk_addr;
	vt_blk_node_add->next = NULL;
	phy_blk_node_add = &infotm_nftl_info->phypmt[phy_blk_addr];
	phy_blk_node_add->vtblk = logic_blk_addr;
	vt_blk_node_curt = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + logic_blk_addr));
	phy_blk_cur = BLOCK_INIT_VALUE;

	for (phy_blk_num=0; phy_blk_num<infotm_nftl_info->endblock; phy_blk_num++) {
		if ((phy_blk_num == phy_blk_addr) || ((vt_blk_node_curt != NULL) && (phy_blk_num == vt_blk_node_curt->phy_blk_addr)))
			continue;
		phy_blk_node_temp = &infotm_nftl_info->phypmt[phy_blk_num];
		if (phy_blk_node_temp->vtblk == logic_blk_addr) {
			if (phy_blk_cur >= 0) {
				phy_blk_node_curt = &infotm_nftl_info->phypmt[phy_blk_cur];
				if (phy_blk_node_temp->timestamp > phy_blk_node_curt->timestamp)
					phy_blk_cur = phy_blk_num;
			} else {
				phy_blk_cur = phy_blk_num;
			}
		}
	}

	if (vt_blk_node_curt == NULL) {
		if (phy_blk_cur >= 0) {
			phy_blk_node_curt = &infotm_nftl_info->phypmt[phy_blk_cur];
			if (phy_blk_node_curt->timestamp >= MAX_TIMESTAMP_NUM)
				phy_blk_node_add->timestamp = 0;
			else
				phy_blk_node_add->timestamp = phy_blk_node_curt->timestamp + 1;
		} else {
			phy_blk_node_add->timestamp = 0;
		}
	} else {
		if (phy_blk_cur >= 0) {
			phy_blk_node_temp = &infotm_nftl_info->phypmt[phy_blk_cur];
			phy_blk_node_curt = &infotm_nftl_info->phypmt[vt_blk_node_curt->phy_blk_addr];
			if (phy_blk_node_temp->timestamp > phy_blk_node_curt->timestamp)
				phy_blk_node_curt = phy_blk_node_temp;
		} else {
			phy_blk_cur = vt_blk_node_curt->phy_blk_addr;
			phy_blk_node_curt = &infotm_nftl_info->phypmt[phy_blk_cur];
		}
		if (phy_blk_node_curt->timestamp >= MAX_TIMESTAMP_NUM)
			phy_blk_node_add->timestamp = 0;
		else
			phy_blk_node_add->timestamp = phy_blk_node_curt->timestamp + 1;
		vt_blk_node_add->next = vt_blk_node_curt;
	}
	*(infotm_nftl_info->vtpmt + logic_blk_addr) = vt_blk_node_add;

	return status;
}

static int infotm_nftl_calculate_last_write(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t phy_blk_addr)
{
	int status;
	struct phyblk_node_t *phy_blk_node;
	phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];

	if (phy_blk_node->valid_sects < 0) {
		status = infotm_nftl_creat_sectmap(infotm_nftl_info, phy_blk_addr);
		if (status)
			return INFOTM_NFTL_FAILURE;
	}

	return 0;
}

static int infotm_nftl_get_valid_pos(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t logic_blk_addr, addr_blk_t *phy_blk_addr,
									 addr_page_t logic_page_addr, addr_page_t *phy_page_addr, uint8_t flag )
{
	struct phyblk_node_t *phy_blk_node;
	struct vtblk_node_t  *vt_blk_node;
	int status;
	uint32_t page_per_blk;

	page_per_blk = infotm_nftl_info->pages_per_blk;
	*phy_blk_addr = BLOCK_INIT_VALUE;
	vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + logic_blk_addr));
	if (vt_blk_node == NULL)
		return INFOTM_NFTL_BLKNOTFOUND;

	*phy_blk_addr = vt_blk_node->phy_blk_addr;
	phy_blk_node = &infotm_nftl_info->phypmt[*phy_blk_addr];
	status = infotm_nftl_get_phy_sect_map(infotm_nftl_info, *phy_blk_addr);
	if (status)
		return INFOTM_NFTL_FAILURE;

	if (flag == WRITE_OPERATION) {
		if (phy_blk_node->last_write < 0)
			infotm_nftl_calculate_last_write(infotm_nftl_info, *phy_blk_addr);

		*phy_page_addr = phy_blk_node->last_write + 1;
		if (*phy_page_addr >= page_per_blk)
			return INFOTM_NFTL_PAGENOTFOUND;

		return 0;
	} else if (flag == READ_OPERATION) {
		do {

			*phy_blk_addr = vt_blk_node->phy_blk_addr;
			phy_blk_node = &infotm_nftl_info->phypmt[*phy_blk_addr];

			status = infotm_nftl_get_phy_sect_map(infotm_nftl_info, *phy_blk_addr);
			if (status)
				return INFOTM_NFTL_FAILURE;

			if (phy_blk_node->phy_page_map[logic_page_addr] >= 0) {
				*phy_page_addr = phy_blk_node->phy_page_map[logic_page_addr];
				return 0;
			}

			vt_blk_node = vt_blk_node->next;
		} while (vt_blk_node != NULL);

		return INFOTM_NFTL_PAGENOTFOUND;
	}

	return INFOTM_NFTL_FAILURE;
}

static int infotm_nftl_write_sect_once(struct infotm_nftl_info_t *infotm_nftl_info, addr_page_t sect_addr, unsigned char *buf, addr_blk_t *phy_blk_write)
{
	struct vtblk_node_t  *vt_blk_node;
	struct phyblk_node_t *phy_blk_node;
	int status = 0, oob_len;
	struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	uint32_t page_per_blk;
	addr_page_t logic_page_addr, phy_page_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;

	page_per_blk = infotm_nftl_info->pages_per_blk;
	logic_page_addr = sect_addr % page_per_blk;
	logic_blk_addr = sect_addr / page_per_blk;

	status = infotm_nftl_get_valid_pos(infotm_nftl_info, logic_blk_addr, &phy_blk_addr, logic_page_addr, &phy_page_addr, WRITE_OPERATION);
	if (status == INFOTM_NFTL_FAILURE) {
	    infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_addr);
        return INFOTM_NFTL_FAILURE;
   	}

	if ((status == INFOTM_NFTL_PAGENOTFOUND) || (status == INFOTM_NFTL_BLKNOTFOUND)) {

		if ((infotm_nftl_wl->free_root.count < (infotm_nftl_info->fillfactor / 4)) && (!infotm_nftl_wl->erased_root.count) && (infotm_nftl_wl->wait_gc_block < 0))
			infotm_nftl_wl->garbage_collect(infotm_nftl_wl, 0);

		status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
		if (status) {
			status = infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
			if (status == 0) {
				infotm_nftl_dbg("nftl couldn`t found free block: %d %d\n", infotm_nftl_wl->free_root.count, infotm_nftl_wl->wait_gc_block);
				return -ENOENT;
			}
			status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
			if (status)
				return status;
		}

		infotm_nftl_add_node(infotm_nftl_info, logic_blk_addr, phy_blk_addr);
		infotm_nftl_wl->add_used(infotm_nftl_wl, phy_blk_addr);
		phy_page_addr = 0;
	}

	phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
	nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	nftl_oob_info->ec = phy_blk_node->ec;
	nftl_oob_info->vtblk = logic_blk_addr;
	nftl_oob_info->timestamp = phy_blk_node->timestamp;
	nftl_oob_info->status_page = 1;
	nftl_oob_info->sect = logic_page_addr;
	oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
	if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)))
		memcpy((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC));

	status = infotm_nftl_info->write_page(infotm_nftl_info, phy_blk_addr, phy_page_addr, buf, nftl_oob_buf, oob_len);
	if (status) {
		infotm_nftl_dbg("nftl write page faile blk: %d page: %d status: %d\n", phy_blk_addr, phy_page_addr, status);
		*phy_blk_write = phy_blk_addr;
    	phy_blk_node->valid_sects++;
    	phy_blk_node->last_write = phy_page_addr;
		status = INFOTM_NFTL_WTITE_RETRY;
		return status;
	}

	vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + logic_blk_addr));
	if (phy_page_addr == (page_per_blk - 1)) {
		status = infotm_nftl_check_node(infotm_nftl_info, logic_blk_addr);
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + logic_blk_addr));
		if (status == INFOTM_NFTL_STRUCTURE_FULL) {
			//infotm_nftl_dbg("infotm nftl structure full at logic : %d phy blk: %d %d \n", logic_blk_addr, phy_blk_addr, phy_blk_node->last_write);
			status = infotm_nftl_wl->garbage_one(infotm_nftl_wl, logic_blk_addr, 0);
			if (status < 0)
				return status;
		} else if (infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node) >= BASIC_BLK_NUM_PER_NODE) {
			infotm_nftl_wl->add_gc(infotm_nftl_wl, logic_blk_addr);
		}
	}

	return status;
}

static void infotm_nftl_move_valid_data(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t phy_blk_addr)
{
    struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
    struct phyblk_node_t *phy_blk_node, *phy_blk_node_valid;
    struct vtblk_node_t  *vt_blk_node;
    addr_blk_t vt_blk_addr, phy_blk_valid, phy_blk_write;
    addr_page_t page_valid, sect_addr;
    int k, status, oob_len;
	unsigned char *nftl_data_buf;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;

    phy_blk_valid = phy_blk_addr;
    phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
    phy_blk_node_valid = &infotm_nftl_info->phypmt[phy_blk_valid];
    vt_blk_addr = phy_blk_node->vtblk;
    nftl_data_buf = infotm_nftl_info->copy_page_buf;
    oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));

	for (k=0; k<infotm_nftl_wl->pages_per_blk; k++) {

        page_valid = -1;
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_addr));
		while (vt_blk_node != NULL) {

			phy_blk_valid = vt_blk_node->phy_blk_addr;
			phy_blk_node_valid = &infotm_nftl_info->phypmt[phy_blk_valid];
			if ((phy_blk_node_valid->phy_page_map[k] >= 0) && (phy_blk_node_valid->phy_page_delete[k>>3] & (1 << (k % 8)))) {
				page_valid = phy_blk_node_valid->phy_page_map[k];
				break;
			}
			vt_blk_node = vt_blk_node->next;
		}
		if ((phy_blk_valid == phy_blk_addr) && (page_valid >= 0)) {
        	status = infotm_nftl_info->read_page(infotm_nftl_info, phy_blk_addr, page_valid, nftl_data_buf, nftl_oob_buf, oob_len);
        	if (status < 0) {
        	    phy_blk_node_valid->phy_page_map[k] = -1;
        		infotm_nftl_dbg("move data read failed: %d %d %x status: %d\n", vt_blk_addr, phy_blk_addr, page_valid, status);
        		continue;
        	}

        	if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC))) {
        		if (memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC))) {
        			infotm_nftl_dbg("move data invalid magic vtblk: %d %d %x \n", nftl_oob_info->vtblk, phy_blk_addr, page_valid);
        		}
        	}

            sect_addr = vt_blk_addr*infotm_nftl_wl->pages_per_blk + k;
            infotm_nftl_write_sect_once(infotm_nftl_info, sect_addr, nftl_data_buf, &phy_blk_write);
		}
	}
    return;
}

static int infotm_nftl_read_sect(struct infotm_nftl_info_t *infotm_nftl_info, addr_page_t sect_addr, unsigned char *buf)
{
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	uint32_t page_per_blk;
	addr_page_t logic_page_addr, phy_page_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;
	int status = 0, oob_len;
	//struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;

    oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
	page_per_blk = infotm_nftl_info->pages_per_blk;
	logic_page_addr = sect_addr % page_per_blk;
	logic_blk_addr = sect_addr / page_per_blk;

	status = infotm_nftl_get_valid_pos(infotm_nftl_info, logic_blk_addr, &phy_blk_addr, logic_page_addr, &phy_page_addr, READ_OPERATION);
	if ((status == INFOTM_NFTL_PAGENOTFOUND) || (status == INFOTM_NFTL_BLKNOTFOUND)) {
		memset(buf, 0xff, infotm_nftl_info->writesize);
		return 0;
	}

	if (status == INFOTM_NFTL_FAILURE)
        return INFOTM_NFTL_FAILURE;

	status = infotm_nftl_info->read_page(infotm_nftl_info, phy_blk_addr, phy_page_addr, buf, nftl_oob_buf, oob_len);
	if (status < 0) {
		infotm_nftl_move_valid_data(infotm_nftl_info, phy_blk_addr);
		return status;
	}
	if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC))) {
		if (memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC))) {
			infotm_nftl_dbg("nftl read sect invalid magic vtblk: %d %d %x \n", logic_blk_addr, phy_blk_addr, phy_page_addr);
		}
	}

	return 0;
}

static void infotm_nftl_delete_sect(struct infotm_nftl_info_t *infotm_nftl_info, addr_page_t sect_addr)
{
	struct vtblk_node_t  *vt_blk_node;
	struct phyblk_node_t *phy_blk_node;
	uint32_t page_per_blk;
	addr_page_t logic_page_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;

	page_per_blk = infotm_nftl_info->pages_per_blk;
	logic_page_addr = sect_addr % page_per_blk;
	logic_blk_addr = sect_addr / page_per_blk;
	vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + logic_blk_addr));
	if (vt_blk_node == NULL)
		return;

	while (vt_blk_node != NULL) {

		phy_blk_addr = vt_blk_node->phy_blk_addr;
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
		phy_blk_node->phy_page_delete[logic_page_addr>>3] &= (~(1 << (logic_page_addr % 8)));
		vt_blk_node = vt_blk_node->next;
	}
	return;
}

static int infotm_nftl_write_sect(struct infotm_nftl_info_t *infotm_nftl_info, addr_page_t sect_addr, unsigned char *buf)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	int status = 0, i, j = 0, phys_erase_shift;
	struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	uint32_t page_per_blk;
	addr_page_t logic_page_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;

	phys_erase_shift = ffs(mtd->erasesize) - 1;
	page_per_blk = infotm_nftl_info->pages_per_blk;

	logic_page_addr = sect_addr % page_per_blk;
	logic_blk_addr = sect_addr / page_per_blk;
	for (i=0; i<CURRENT_WRITED_NUM; i++) {
		if (infotm_nftl_info->current_write_block[i] == BLOCK_INIT_VALUE) {
			infotm_nftl_info->current_write_block[i] = logic_blk_addr;
			infotm_nftl_info->current_write_num[i] = 0;
			break;
		}
	}
	if (i >= CURRENT_WRITED_NUM) {
		status = infotm_nftl_info->current_write_num[0];
		j = 0;
		for (i=0; i<CURRENT_WRITED_NUM; i++) {
			if (infotm_nftl_info->current_write_num[i] < status) {
				status = infotm_nftl_info->current_write_num[i];
				j = i;
			}
			if (infotm_nftl_info->current_write_block[i] == logic_blk_addr) {
				infotm_nftl_info->current_write_num[i]++;
				break;
			}
		}
		if (i >= CURRENT_WRITED_NUM) {
			infotm_nftl_info->current_write_block[j] = logic_blk_addr;
			infotm_nftl_info->current_write_num[j] = 0;
		}
	}
	if (infotm_nftl_wl->wait_gc_block >= 0) {
		infotm_nftl_info->continue_writed_sects++;
		if (infotm_nftl_wl->wait_gc_block == logic_blk_addr) {
			infotm_nftl_dbg("write gc blk %d %d \n", logic_blk_addr, infotm_nftl_info->continue_writed_sects);
			infotm_nftl_wl->wait_gc_block = BLOCK_INIT_VALUE;
			infotm_nftl_wl->garbage_collect(infotm_nftl_wl, 0);
			infotm_nftl_info->continue_writed_sects = 0;
			infotm_nftl_wl->add_gc(infotm_nftl_wl, logic_blk_addr);
		}
	}

    status = infotm_nftl_write_sect_once(infotm_nftl_info, sect_addr, buf, &phy_blk_addr);
    if (status == INFOTM_NFTL_WTITE_RETRY) {
        infotm_nftl_move_valid_data(infotm_nftl_info, phy_blk_addr);
        status = infotm_nftl_write_sect_once(infotm_nftl_info, sect_addr, buf, &phy_blk_addr);
        if (status)
            return status;
    }

	if (infotm_nftl_wl->wait_gc_block >= 0) {
		if (((infotm_nftl_info->continue_writed_sects % 8) == 0) || (infotm_nftl_wl->free_root.count < (infotm_nftl_info->fillfactor / 4))) {
			infotm_nftl_wl->garbage_collect(infotm_nftl_wl, 0);
			if (infotm_nftl_wl->wait_gc_block == BLOCK_INIT_VALUE)
				infotm_nftl_info->continue_writed_sects = 0;
		}
	}

	return 0;
}

static void infotm_nftl_add_block(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t phy_blk, struct nftl_oobinfo_t *nftl_oob_info)
{
	struct phyblk_node_t *phy_blk_node_curt, *phy_blk_node_add;
	struct vtblk_node_t  *vt_blk_node_curt, *vt_blk_node_prev, *vt_blk_node_add;

	vt_blk_node_add = (struct vtblk_node_t *)infotm_nftl_malloc(sizeof(struct vtblk_node_t));
	if (vt_blk_node_add == NULL)
		return;
	vt_blk_node_add->phy_blk_addr = phy_blk;
	vt_blk_node_add->next = NULL;
	phy_blk_node_add = &infotm_nftl_info->phypmt[phy_blk];
	phy_blk_node_add->ec = nftl_oob_info->ec;
	phy_blk_node_add->vtblk = nftl_oob_info->vtblk;
	phy_blk_node_add->timestamp = nftl_oob_info->timestamp;
	vt_blk_node_curt = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + nftl_oob_info->vtblk));

	if (vt_blk_node_curt == NULL) {
		vt_blk_node_curt = vt_blk_node_add;
		*(infotm_nftl_info->vtpmt + nftl_oob_info->vtblk) = vt_blk_node_curt;
		return;
	}

	vt_blk_node_prev = vt_blk_node_curt;
	while(vt_blk_node_curt != NULL) {

		phy_blk_node_curt = &infotm_nftl_info->phypmt[vt_blk_node_curt->phy_blk_addr];
		if (((phy_blk_node_add->timestamp > phy_blk_node_curt->timestamp)
			 && ((phy_blk_node_add->timestamp - phy_blk_node_curt->timestamp) < (MAX_TIMESTAMP_NUM - infotm_nftl_info->accessibleblocks)))
			|| ((phy_blk_node_add->timestamp < phy_blk_node_curt->timestamp)
			 && ((phy_blk_node_curt->timestamp - phy_blk_node_add->timestamp) >= (MAX_TIMESTAMP_NUM - infotm_nftl_info->accessibleblocks)))) {

			vt_blk_node_add->next = vt_blk_node_curt;
			if (*(infotm_nftl_info->vtpmt + nftl_oob_info->vtblk) == vt_blk_node_curt)
				*(infotm_nftl_info->vtpmt + nftl_oob_info->vtblk) = vt_blk_node_add;
			else
				vt_blk_node_prev->next = vt_blk_node_add;
			break;
		} else if (phy_blk_node_add->timestamp == phy_blk_node_curt->timestamp) {
			infotm_nftl_dbg("NFTL timestamp err logic blk: %d phy blk: %d %d %d\n", nftl_oob_info->vtblk, vt_blk_node_curt->phy_blk_addr, vt_blk_node_add->phy_blk_addr, phy_blk_node_add->timestamp);
			if (phy_blk_node_add->ec < phy_blk_node_curt->ec) {

				if (vt_blk_node_prev == vt_blk_node_curt) {
					vt_blk_node_add->next = vt_blk_node_curt;
					*(infotm_nftl_info->vtpmt + nftl_oob_info->vtblk) = vt_blk_node_add;
				} else {
					vt_blk_node_prev->next = vt_blk_node_add;
					vt_blk_node_add->next = vt_blk_node_curt;
				}
				break;
			}
			if (vt_blk_node_curt == (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + nftl_oob_info->vtblk))) {
				vt_blk_node_add->next = vt_blk_node_curt;
				*(infotm_nftl_info->vtpmt + nftl_oob_info->vtblk) = vt_blk_node_add;
			} else {
				vt_blk_node_add->next = vt_blk_node_curt->next;
				vt_blk_node_curt->next = vt_blk_node_add;
			}
			break;
		} else {
			if (vt_blk_node_curt->next != NULL) {
				vt_blk_node_prev = vt_blk_node_curt;
				vt_blk_node_curt = vt_blk_node_curt->next;
			} else {
				vt_blk_node_curt->next = vt_blk_node_add;
				vt_blk_node_curt = vt_blk_node_curt->next;
				break;
			}
		}
	}

	return;
}

static void infotm_nftl_check_conflict_node(struct infotm_nftl_info_t *infotm_nftl_info)
{
	struct vtblk_node_t  *vt_blk_node;
	struct infotm_nftl_wl_t *infotm_nftl_wl;
	addr_blk_t vt_blk_num;
	int node_length = 0, status = 0;

	infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
    if (infotm_nftl_wl->erased_root.count)
        return;
	for (vt_blk_num=0; vt_blk_num<infotm_nftl_info->accessibleblocks; vt_blk_num++) {

        if (infotm_nftl_wl->free_root.count >= INFOTM_LIMIT_FACTOR)
            break;

		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		if (vt_blk_node == NULL)
			continue;

		node_length = infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node);
		if (node_length < BASIC_BLK_NUM_PER_NODE)
			continue;

        //infotm_nftl_dbg("need check conflict node vt blk: %d and node_length:%d\n", vt_blk_num, node_length);
		status = infotm_nftl_check_node(infotm_nftl_info, vt_blk_num);
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		if (infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node) >= BASIC_BLK_NUM_PER_NODE)
			infotm_nftl_wl->add_gc(infotm_nftl_wl, vt_blk_num);
#if 0
		if (status == INFOTM_NFTL_STRUCTURE_FULL) {
			infotm_nftl_dbg("found conflict node vt blk: %d \n", vt_blk_num);
			infotm_nftl_wl->garbage_one(infotm_nftl_wl, vt_blk_num, 0);
		}
#endif
	}
	infotm_nftl_info->cur_split_blk = vt_blk_num;

	return;
}

static void infotm_nftl_creat_structure(struct infotm_nftl_info_t *infotm_nftl_info)
{
	struct vtblk_node_t  *vt_blk_node;
	struct infotm_nftl_wl_t *infotm_nftl_wl;
	addr_blk_t vt_blk_num;

	infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	for (vt_blk_num=infotm_nftl_info->cur_split_blk; vt_blk_num<infotm_nftl_info->accessibleblocks; vt_blk_num++) {

		if ((vt_blk_num - infotm_nftl_info->cur_split_blk) >= infotm_nftl_info->default_split_unit)
			break;

		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		if (vt_blk_node == NULL)
			continue;
#if 0
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		if (infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node) < BASIC_BLK_NUM_PER_NODE)
			continue;
#endif

		infotm_nftl_check_node(infotm_nftl_info, vt_blk_num);
		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		if (infotm_nftl_get_node_length(infotm_nftl_info, vt_blk_node) >= BASIC_BLK_NUM_PER_NODE)
			infotm_nftl_wl->add_gc(infotm_nftl_wl, vt_blk_num);
#if 0
		if (status == INFOTM_NFTL_STRUCTURE_FULL) {
			infotm_nftl_dbg("found conflict node vt blk: %d \n", vt_blk_num);
			infotm_nftl_wl->garbage_one(infotm_nftl_wl, vt_blk_num, 0);
		}
#endif
	}

	infotm_nftl_info->cur_split_blk = vt_blk_num;
	if (infotm_nftl_info->cur_split_blk >= infotm_nftl_info->accessibleblocks) {
		infotm_nftl_info->isinitialised = 1;
		infotm_nftl_wl->gc_start_block = infotm_nftl_info->accessibleblocks - 1;
		infotm_nftl_dbg("nftl creat stucture completely free blk: %d erased blk: %d\n", infotm_nftl_wl->free_root.count, infotm_nftl_wl->erased_root.count);
	}

	return;
}

static void infotm_nftl_save_phy_node(struct infotm_nftl_info_t *infotm_nftl_info)
{
	struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
    struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
    struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)infotm_nftl_info->resved_mem;
    struct mtd_info *mtd;
    struct infotm_nand_chip *infotm_chip;
	int error = 0, node_crc, oob_len, i, j, k, status, phy_node_size, phy_node_compressed = 0, node_size_per_write;
	uint32_t size_in_blk;
	addr_blk_t phy_blk_num;
	unsigned char *data_buf;
	struct phyblk_node_t *phy_blk_node;
	struct phyblk_node_save_t *phy_blk_node_save;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;

    oob_len = infotm_nftl_info->oobsize;
    size_in_blk = infotm_nftl_info->endblock;
    phy_node_size = (sizeof(struct phyblk_node_t) * size_in_blk);
    phy_node_size = (phy_node_size + sizeof(uint32_t) + infotm_nftl_info->writesize - 1) / infotm_nftl_info->writesize;
    if ((phy_node_size > infotm_nftl_info->pages_per_blk) && (infotm_nftl_info->pages_per_blk < 0x100)) {
        phy_node_size = (sizeof(struct phyblk_node_save_t) * size_in_blk);
        phy_node_size = (phy_node_size + sizeof(uint32_t) + infotm_nftl_info->writesize - 1) / infotm_nftl_info->writesize;
        if (phy_node_size > infotm_nftl_info->pages_per_blk)
                return;
        phy_node_compressed = 1;
    }
	if (phy_node_size > infotm_nftl_info->pages_per_blk)
		return;

    phy_blk_num = -1;
	status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_num);
	if (status) {
		status = infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
		if (status == 0) {
			infotm_nftl_dbg("nftl couldn`t found free block: %d %d\n", infotm_nftl_wl->free_root.count, infotm_nftl_wl->wait_gc_block);
			return;
		}
		status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_num);
		if (status)
			return;
	}

	phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_num];
	phy_blk_node->timestamp++;
	phy_blk_node->ec++;
	phy_blk_node->vtblk = SAVE_NODE_VTBLK;
	node_size_per_write = infotm_nftl_info->writesize / sizeof(struct phyblk_node_t);
    if (phy_node_compressed) {
        for (j=0; j<size_in_blk; j+=node_size_per_write) {
        	data_buf = infotm_nftl_info->copy_page_buf;
        	memcpy(data_buf, &infotm_nftl_info->phypmt[j], node_size_per_write*sizeof(struct phyblk_node_t));
        	for (i=0; i<node_size_per_write; i++) {
        		if ((j+i) >= size_in_blk)
        			break;
	            phy_blk_node = (struct phyblk_node_t *)(data_buf + i*sizeof(struct phyblk_node_t));
	            phy_blk_node_save = (struct phyblk_node_save_t *)((uint8_t *)infotm_nftl_info->phypmt + ((j+i)*sizeof(struct phyblk_node_save_t)));
	            phy_blk_node_save->ec = phy_blk_node->ec;
	            phy_blk_node_save->valid_sects = phy_blk_node->valid_sects;
	            phy_blk_node_save->vtblk = phy_blk_node->vtblk;
	            phy_blk_node_save->last_write = phy_blk_node->last_write;
	            phy_blk_node_save->status_page = phy_blk_node->status_page;
	            phy_blk_node_save->timestamp = phy_blk_node->timestamp;
	            for (k=0; k<infotm_nftl_info->pages_per_blk; k++) {
	                if (phy_blk_node->phy_page_map[k] >= 0)
	                    phy_blk_node_save->phy_page_map[k] = (uint8_t)(phy_blk_node->phy_page_map[k]&0xff);
	                else
	                    phy_blk_node_save->phy_page_map[k] = 0xff;
	            }
	        }
        }
        data_buf = (unsigned char *)infotm_nftl_info->phypmt;
		node_crc = (crc32((0 ^ 0xffffffffL), data_buf, (sizeof(struct phyblk_node_save_t) * size_in_blk)) ^ 0xffffffffL);
        *(unsigned *)(data_buf + (sizeof(struct phyblk_node_save_t) * size_in_blk)) = node_crc;
    } else {
        data_buf = (unsigned char *)infotm_nftl_info->phypmt;
        node_crc = (crc32((0 ^ 0xffffffffL), data_buf, (sizeof(struct phyblk_node_t) * size_in_blk)) ^ 0xffffffffL);
		*(unsigned *)(data_buf + (sizeof(struct phyblk_node_t) * size_in_blk)) = node_crc;
    }

	infotm_nftl_dbg("nftl save phy node: %d compress: %d free: %d \n", phy_blk_num, phy_node_compressed, infotm_nftl_wl->free_root.count);
	infotm_nftl_info->current_save_node_block = phy_blk_num;
	phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_num];
	nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	nftl_oob_info->ec = phy_blk_node->ec;
	nftl_oob_info->vtblk = SAVE_NODE_VTBLK;
	nftl_oob_info->timestamp = phy_blk_node->timestamp;
	nftl_oob_info->status_page = 1;
	oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_PHY_NODE_SAVE_MAGIC)));
	if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_PHY_NODE_SAVE_MAGIC)))
		memcpy((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_PHY_NODE_SAVE_MAGIC, strlen(INFOTM_PHY_NODE_SAVE_MAGIC));

	for (i=0; i<phy_node_size; i++) {

		nftl_oob_info->sect = i;
		error = infotm_nftl_ops->write_page(infotm_nftl_info, phy_blk_num, i, (data_buf+i*infotm_nftl_info->writesize), (uint8_t *)nftl_oob_buf, oob_len);
		if (error) {
			infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
			infotm_nftl_dbg("write reboot save phy: %d %x\n", phy_blk_num, i);
			break;
		}
	}
	infotm_nand_save->nftl_node_magic = RRTB_NFTL_NODESAVE_MAGIC;
	infotm_nand_save->nftl_node_blk = phy_blk_num;

	if (infotm_nftl_info->para_rewrite_flag) {
	    mtd = get_mtd_device_nm(NAND_BOOT_NAME);
	    if (IS_ERR(mtd))
	        return;
	    infotm_chip = mtd_to_nand_chip(mtd);
		infotm_chip->infotm_nand_save_para(infotm_chip);
		infotm_nftl_info->para_rewrite_flag = 0;
	}

    return;
}

static int infotm_nftl_write_mini_part(struct infotm_nftl_info_t *infotm_nftl_info, loff_t from, size_t len, unsigned char *buf)
{
	struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct nand_chip *chip = mtd->priv;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	struct phyblk_node_t *phy_blk_node;
	struct mtd_oob_ops infotm_oob_ops;
	addr_blk_t phy_blk_addr;
	int status, page_cnt, i, oob_len, error = 0;
	loff_t offs;

	if (from < INFOTM_MAGIC_PART_SIZE) {
	    mtd = get_mtd_device_nm(NAND_BOOT_NAME);
	    if (IS_ERR(mtd))
	        return -ENOENT;

		status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
		if (status) {
			status = infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
			if (status == 0) {
				infotm_nftl_dbg("nftl couldn`t found free block: %d %d\n", infotm_nftl_wl->free_root.count, infotm_nftl_wl->wait_gc_block);
				return -ENOENT;
			}
			status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
			if (status)
				return status;
		}

		oob_len = mtd->oobavail;
		if (infotm_nand_save->uboot0_blk[0] > 0) {
			infotm_nftl_info->erase_block(infotm_nftl_info, (infotm_nand_save->uboot0_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk)));
			infotm_nftl_wl->add_erased(infotm_nftl_wl, (infotm_nand_save->uboot0_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk)));
		}
		infotm_nand_save->uboot0_blk[0] = phy_blk_addr * infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk;
		offs = infotm_nand_save->uboot0_blk[0];
		offs *= mtd->writesize;
		offs += (from & (INFOTM_MAGIC_PART_SIZE - 1));
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
		phy_blk_node->timestamp++;
		phy_blk_node->ec++;
		phy_blk_node->vtblk = UBOOT0_VTBLK;
		*(uint16_t *)nftl_oob_buf = IMAPX800_VERSION_MAGIC;
		*(uint16_t *)(nftl_oob_buf + 2) = IMAPX800_UBOOT_MAGIC;
		infotm_nftl_info->para_rewrite_flag = 1;
	} else {
	    mtd = get_mtd_device_nm(NAND_NORMAL_NAME);
	    if (IS_ERR(mtd))
	        return -ENOENT;

		status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
		if (status) {
			status = infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
			if (status == 0) {
				infotm_nftl_dbg("nftl couldn`t found free block: %d %d\n", infotm_nftl_wl->free_root.count, infotm_nftl_wl->wait_gc_block);
				return -ENOENT;
			}
			status = infotm_nftl_wl->get_best_free(infotm_nftl_wl, &phy_blk_addr);
			if (status)
				return status;
		}

		oob_len = infotm_nftl_info->oobsize;
		if (from >= INFOTM_ITEM_OFFSET) {
			if (infotm_nand_save->item_blk[0] > 0)
				infotm_nftl_wl->add_free(infotm_nftl_wl, (infotm_nand_save->item_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk)));
			infotm_nand_save->item_blk[0] = phy_blk_addr * infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk;
			offs = infotm_nand_save->item_blk[0];
			offs *= mtd->writesize;
			offs += (from & (INFOTM_MAGIC_PART_SIZE - 1));
			nftl_oob_info->vtblk = ITEM_VTBLK;
			memcpy((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_UBOOT1_STRING, strlen(INFOTM_UBOOT1_STRING));
		} else {
			if (infotm_nand_save->uboot1_blk[0] > 0)
				infotm_nftl_wl->add_free(infotm_nftl_wl, (infotm_nand_save->uboot1_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk)));
			infotm_nand_save->uboot1_blk[0] = phy_blk_addr * infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk;
			offs = infotm_nand_save->uboot1_blk[0];
			offs *= mtd->writesize;
			offs += (from & (INFOTM_MAGIC_PART_SIZE - 1));
			nftl_oob_info->vtblk = UBOOT1_VTBLK;
			memcpy((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_ITEM_STRING, strlen(INFOTM_ITEM_STRING));
		}
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];
		phy_blk_node->timestamp++;
		phy_blk_node->ec++;
		phy_blk_node->vtblk = nftl_oob_info->vtblk;
		nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
		nftl_oob_info->ec = phy_blk_node->ec;
		nftl_oob_info->timestamp = phy_blk_node->timestamp;
		nftl_oob_info->status_page = 1;
		infotm_nftl_info->para_rewrite_flag = 1;
	}

	if (len & (mtd->writesize - 1))
		len += (mtd->writesize - (len & (mtd->writesize - 1)));
	page_cnt = (len / mtd->writesize);

	chip = mtd->priv;
	infotm_nftl_dbg("nftl write mini %llx %x \n", offs, (int)(offs >> chip->page_shift));
	infotm_oob_ops.mode = MTD_OOB_AUTO;
	infotm_oob_ops.len = mtd->writesize;
	infotm_oob_ops.ooblen = oob_len;
	infotm_oob_ops.ooboffs = 0;
	infotm_oob_ops.oobbuf = nftl_oob_buf;
	for (i=0; i<page_cnt; i++) {
		if (from >= INFOTM_MAGIC_PART_SIZE)
			nftl_oob_info->sect = i;
		infotm_oob_ops.datbuf = buf + i * mtd->writesize;
		error = mtd->_write_oob(mtd, offs, &infotm_oob_ops);
		offs += mtd->writesize;
	}

	return error;
}

static int infotm_nftl_read_mini_part(struct infotm_nftl_info_t *infotm_nftl_info, loff_t from, size_t len, unsigned char *buf)
{
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	int error = 0;
	loff_t offs;
	size_t retlen;

	infotm_nftl_dbg("nftl read mini: %llx %x %x %x %x \n", from, len, infotm_nand_save->uboot0_blk[0], infotm_nand_save->uboot1_blk[0], infotm_nand_save->item_blk[0]);
	if (from < INFOTM_MAGIC_PART_SIZE) {
	    mtd = get_mtd_device_nm(NAND_BOOT_NAME);
	    if (IS_ERR(mtd))
	        return -ENOENT;

		if (infotm_nand_save->uboot0_blk[0] > 0) {
			offs = infotm_nand_save->uboot0_blk[0];
			offs *= mtd->writesize;
			offs += (from & (INFOTM_MAGIC_PART_SIZE - 1));
			error = mtd->_read(mtd, offs, len, &retlen, buf);
		} else {
			memset(buf, 0xff, len);
		}
	} else {
	    mtd = get_mtd_device_nm(NAND_NORMAL_NAME);
	    if (IS_ERR(mtd))
	        return -ENOENT;

		if (from >= INFOTM_ITEM_OFFSET) {
			if (infotm_nand_save->item_blk[0] > 0) {
				offs = infotm_nand_save->item_blk[0];
				offs *= mtd->writesize;
				offs += (from & (INFOTM_MAGIC_PART_SIZE - 1));
				error = mtd->_read(mtd, offs, len, &retlen, buf);
			} else {
				memset(buf, 0xff, len);
			}
		} else {
			if (infotm_nand_save->uboot1_blk[0] > 0) {
				offs = infotm_nand_save->uboot1_blk[0];
				offs *= mtd->writesize;
				offs += (from & (INFOTM_MAGIC_PART_SIZE - 1));
				error = mtd->_read(mtd, offs, len, &retlen, buf);
			} else {
				memset(buf, 0xff, len);
			}
		}
	}

	return error;
}

static int infotm_nftl_block_bad_scrub(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	return 0;
}

static void infotm_nftl_scan_bbt(struct infotm_nftl_info_t *infotm_nftl_info)
{
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
	struct mtd_info *mtd = infotm_nftl_info->mtd;
	struct nand_chip *chip = mtd->priv;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_bbt_info *nand_bbt_info = &infotm_nftl_info->nand_bbt_info;
	struct nand_bbt_t *nand_bbt;
	struct phyblk_node_t *phy_blk_node;
	unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	int (*nand_block_bad_old)(struct mtd_info *, loff_t, int) = NULL;
	int error, phy_blk, oob_len, phy_page, len;
	size_t amount_loaded = 0;

	if (infotm_nand_save->bbt_blk[0] < 0) {
		if (imapx800_chip->drv_ver_changed) {
			nand_block_bad_old = chip->block_bad;
			chip->block_bad = infotm_nftl_block_bad_scrub;
		}
		nand_bbt_info->total_bad_num = 0;
		for (phy_blk=0; phy_blk<infotm_nftl_info->endblock; phy_blk++) {
			if (imapx800_chip->drv_ver_changed) {
				error = infotm_nftl_ops->erase_block(infotm_nftl_info, phy_blk);
				if (error) {
					nand_bbt = &nand_bbt_info->nand_bbt[nand_bbt_info->total_bad_num++];
					nand_bbt->blk_addr = phy_blk;
					nand_bbt->plane_num = 0;
					nand_bbt->chip_num = 0;
					nand_bbt->bad_type = SHIPMENT_BAD;
				}
			} else {
				error = infotm_nftl_ops->blk_isbad(infotm_nftl_info, phy_blk);
				if (error) {
					nand_bbt = &nand_bbt_info->nand_bbt[nand_bbt_info->total_bad_num++];
					nand_bbt->blk_addr = phy_blk;
					nand_bbt->plane_num = 0;
					nand_bbt->chip_num = 0;
					nand_bbt->bad_type = SHIPMENT_BAD;
				}
			}
		}
		if (imapx800_chip->drv_ver_changed)
			chip->block_bad = nand_block_bad_old;

		memcpy(nand_bbt_info->bbt_head_magic, BBT_HEAD_MAGIC, 4);
		memcpy(nand_bbt_info->bbt_tail_magic, BBT_TAIL_MAGIC, 4);
		infotm_chip->nand_bbt_info = nand_bbt_info;
	} else {
		oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_BBT_STRING)));
		phy_blk = infotm_nand_save->bbt_blk[0];
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk];
		infotm_nftl_info->get_phy_sect_map(infotm_nftl_info, phy_blk);
		while (amount_loaded < sizeof(struct infotm_nand_bbt_info)) {

			phy_page = phy_blk_node->phy_page_map[amount_loaded / mtd->writesize];
			error = infotm_nftl_info->read_page(infotm_nftl_info, phy_blk, phy_page, infotm_nftl_info->copy_page_buf, nftl_oob_buf, oob_len);
			if (error < 0)
				return;

			if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_BBT_STRING))) {
				if (memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_BBT_STRING, strlen(INFOTM_BBT_STRING))) {
					infotm_nftl_dbg("nftl scan bbt invalid magic vtblk: %d %x \n", phy_blk, phy_page);
				}
			}
			len = min(mtd->writesize, sizeof(struct infotm_nand_bbt_info) - amount_loaded);
			memcpy((unsigned char *)nand_bbt_info + amount_loaded, infotm_nftl_info->copy_page_buf, sizeof(struct infotm_nand_bbt_info));
			amount_loaded += mtd->writesize;
		}
		infotm_chip->nand_bbt_info = nand_bbt_info;
	}
	infotm_nftl_dbg("nftl scan bbt: %d %d \n", infotm_nand_save->bbt_blk[0], nand_bbt_info->total_bad_num);

	return;
}

static ssize_t infotm_blk_map_table(struct class *class, struct class_attribute *attr,	const char *buf, size_t count)
{
    struct infotm_nftl_info_t *infotm_nftl_info = container_of(class, struct infotm_nftl_info_t, cls);
    struct vtblk_node_t  *vt_blk_node;
    int vt_blk_num;

	for (vt_blk_num=0; vt_blk_num<infotm_nftl_info->accessibleblocks; vt_blk_num++) {

		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		if (vt_blk_node == NULL)
			continue;

		infotm_nftl_dbg("nftl logic blk %d map to", vt_blk_num);
		while (vt_blk_node != NULL) {

			infotm_nftl_dbg(" %d ", vt_blk_node->phy_blk_addr);
			vt_blk_node = vt_blk_node->next;
		}
		infotm_nftl_dbg("\n");
	}
	return count;
}

static ssize_t infotm_sect_map_table(struct class *class, struct class_attribute *attr,	const char *buf, size_t count)
{
    struct infotm_nftl_info_t *infotm_nftl_info = container_of(class, struct infotm_nftl_info_t, cls);
    struct vtblk_node_t  *vt_blk_node;
    struct phyblk_node_t *phy_blk_node;
	unsigned int address;
    unsigned blks_per_sect, sect_addr;
    addr_page_t logic_page_addr, phy_page_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;
    uint32_t page_per_blk;
    unsigned char nftl_oob_buf[infotm_nftl_info->oobsize];
	struct nftl_oobinfo_t *nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
    int ret, i, oob_len;

	ret =  sscanf(buf, "%x", &address);
    blks_per_sect = infotm_nftl_info->writesize / 512;
	sect_addr = address / blks_per_sect;

    page_per_blk = infotm_nftl_info->pages_per_blk;
    //logic_page_addr = sect_addr % page_per_blk;
	//logic_blk_addr = sect_addr / page_per_blk;
    logic_blk_addr = address;

    oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
    vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + logic_blk_addr));
    if (vt_blk_node == NULL) {
        infotm_nftl_dbg("invalid logic blk addr %d \n", logic_blk_addr);
        return count;
    }
    do {

		phy_blk_addr = vt_blk_node->phy_blk_addr;
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_addr];

    	infotm_nftl_dbg("logic: %d phy: %d \n", logic_blk_addr, phy_blk_addr);
    	for (i=0; i<page_per_blk; i++) {
    		ret = infotm_nftl_info->get_page_info(infotm_nftl_info, phy_blk_addr, i, nftl_oob_buf, oob_len);
    		if (ret) {
    			infotm_nftl_dbg("nftl creat sect map faile at: %d\n", phy_blk_addr);
    		}
            infotm_nftl_dbg("%02hhx ", nftl_oob_info->sect);
            if ((i%32 == 0) && (i != 0))
                infotm_nftl_dbg("\n");
    	}
    	infotm_nftl_dbg("\n");

		vt_blk_node = vt_blk_node->next;
	} while (vt_blk_node != NULL);

	return count;

    ret = infotm_nftl_get_valid_pos(infotm_nftl_info, logic_blk_addr, &phy_blk_addr, logic_page_addr, &phy_page_addr, READ_OPERATION);
	if (ret == INFOTM_NFTL_FAILURE){
        return INFOTM_NFTL_FAILURE;
	}

    if ((ret == INFOTM_NFTL_PAGENOTFOUND) || (ret == INFOTM_NFTL_BLKNOTFOUND)) {
		printk("the phy address not found\n");
		return 1;
	}

    printk("address %x map phy address:blk addr %x page addr %x\n", address, logic_blk_addr, logic_page_addr);

	return count;
}

static ssize_t infotm_start_creat_structure(struct class *class, struct class_attribute *attr,	const char *buf, size_t count)
{
    struct infotm_nftl_info_t *infotm_nftl_info = container_of(class, struct infotm_nftl_info_t, cls);
    int split_num = 0;

    sscanf(buf, "%x", &split_num);
    infotm_nftl_info->default_split_unit = split_num;

    return count;
}

static ssize_t infotm_start_idle_garbage(struct class *class, struct class_attribute *attr,	const char *buf, size_t count)
{
    struct infotm_nftl_info_t *infotm_nftl_info = container_of(class, struct infotm_nftl_info_t, cls);
	struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
    int start = 0;

    sscanf(buf, "%x", &start);
    if (start) {
		if ((infotm_nftl_wl->free_root.count <= DEFAULT_IDLE_FREE_BLK) && (!infotm_nftl_wl->erased_root.count))
			infotm_nftl_wl->gc_need_flag = start;
    } else {
    	infotm_nftl_wl->gc_need_flag = 0;
    }
    	
    return count;
}

static struct class_attribute nftl_class_attrs[] = {
    __ATTR(map_table,  S_IRUGO | S_IWUSR, NULL,    infotm_sect_map_table),
    __ATTR(blk_table,  S_IRUGO | S_IWUSR, NULL,    infotm_blk_map_table),
    __ATTR(creat_start,  S_IRUGO | S_IWUSR, NULL,    infotm_start_creat_structure),
    __ATTR(garbage_start,  (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH), NULL,    infotm_start_idle_garbage),
    __ATTR_NULL
};

int infotm_nftl_initialize(struct infotm_nftl_blk_t *infotm_nftl_blk)
{
	struct mtd_info *mtd = infotm_nftl_blk->mtd;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_bbt_info *nand_bbt_info;
	struct infotm_nftl_info_t *infotm_nftl_info;
	struct infotm_nftl_ops_t *infotm_nftl_ops;
	struct nftl_oobinfo_t *nftl_oob_info;
	struct infotm_nftl_wl_t *infotm_nftl_wl;
	struct phyblk_node_t *phy_blk_node;
	struct phyblk_node_save_t *phy_blk_node_save;
	struct infotm_nand_para_save *infotm_nand_save;
	int error = 0, phy_blk_num, oob_len, i, j, k, phy_node_size, phy_node_saved = 0, phy_node_compressed = 0;
	uint32_t phy_page_addr, size_in_blk, crc32_phy_save, crc32_phy_cacl;
	uint32_t phys_erase_shift, phys_write_shift;
	unsigned char *data_buf = NULL, *nftl_data_buf;
	unsigned char nftl_oob_buf[mtd->oobavail];
	unsigned char nftl_oob_cmp_buf[mtd->oobavail];

	if (mtd->oobavail < sizeof(struct nftl_oobinfo_t))
		return -EPERM;
	infotm_nftl_info = infotm_nftl_malloc(sizeof(struct infotm_nftl_info_t));
	if (!infotm_nftl_info)
		return -ENOMEM;

	infotm_nftl_blk->infotm_nftl_info = infotm_nftl_info;
	infotm_nftl_info->mtd = mtd;
	infotm_nftl_info->writesize = mtd->writesize;
	infotm_nftl_info->oobsize = mtd->oobavail;
	phys_erase_shift = ffs(mtd->erasesize) - 1;
	phys_write_shift = ffs(mtd->writesize) - 1;
	size_in_blk =  (mtd->size >> phys_erase_shift);
	if (size_in_blk <= INFOTM_LIMIT_FACTOR)
		return -EPERM;

	//nand_bbt_info = &infotm_chip->infotm_nandenv_info->nand_bbt_info;
	infotm_nftl_info->pages_per_blk = mtd->erasesize / mtd->writesize;
	infotm_nftl_info->startblock = 0;
	infotm_nftl_info->endblock = size_in_blk;
	//infotm_nftl_info->fillfactor = nand_bbt_info->total_bad_num;
	//infotm_nftl_info->fillfactor += NFTL_BASIC_FILLFACTOR;
	//infotm_nftl_info->accessibleblocks = size_in_blk - infotm_nftl_info->fillfactor;
	memset(infotm_nftl_info->current_write_block, 0xff, CURRENT_WRITED_NUM * sizeof(addr_blk_t));
	phy_node_size = (sizeof(struct phyblk_node_t) * size_in_blk);
	phy_node_size = ((phy_node_size + sizeof(uint32_t) + infotm_nftl_info->writesize - 1) >> phys_write_shift);

	infotm_nand_save = (struct infotm_nand_para_save *)infotm_nftl_info->resved_mem;
	infotm_nftl_info->copy_page_buf = infotm_nftl_malloc(infotm_nftl_info->writesize);
	if (!infotm_nftl_info->copy_page_buf)
		return -ENOMEM;
	infotm_nftl_info->phypmt = (struct phyblk_node_t *)infotm_nftl_malloc((phy_node_size * infotm_nftl_info->writesize));
	if (!infotm_nftl_info->phypmt)
		return -ENOMEM;
	infotm_nftl_info->vtpmt = (void **)infotm_nftl_malloc((sizeof(void*) * size_in_blk));
	if (!infotm_nftl_info->vtpmt)
		return -ENOMEM;
	memset((unsigned char *)infotm_nftl_info->phypmt, 0xff, phy_node_size * infotm_nftl_info->writesize);

	infotm_nftl_ops_init(infotm_nftl_info);

	infotm_nftl_info->read_page = infotm_nftl_read_page;
	infotm_nftl_info->write_page = infotm_nftl_write_page;
	infotm_nftl_info->copy_page = infotm_nftl_copy_page;
	infotm_nftl_info->get_page_info = infotm_nftl_get_page_info;
	infotm_nftl_info->blk_mark_bad = infotm_nftl_blk_mark_bad;
	infotm_nftl_info->blk_isbad = infotm_nftl_blk_isbad;
	infotm_nftl_info->get_phy_sect_map = infotm_nftl_get_phy_sect_map;
	infotm_nftl_info->erase_block = infotm_nftl_erase_block;
	infotm_nftl_info->erase_logic_block = infotm_nftl_erase_logic_block;

	infotm_nftl_info->read_sect = infotm_nftl_read_sect;
	infotm_nftl_info->write_sect = infotm_nftl_write_sect;
	infotm_nftl_info->delete_sect = infotm_nftl_delete_sect;
	infotm_nftl_info->creat_structure = infotm_nftl_creat_structure;
	infotm_nftl_info->save_phy_node = infotm_nftl_save_phy_node;
	infotm_nftl_info->read_mini_part = infotm_nftl_read_mini_part;
	infotm_nftl_info->write_mini_part = infotm_nftl_write_mini_part;

	error = infotm_nftl_wl_init(infotm_nftl_info);
	if (error)
		return error;

	infotm_nftl_info->resved_mem = imapx800_chip->resved_mem;
	infotm_nand_save = (struct infotm_nand_para_save *)infotm_nftl_info->resved_mem;
    infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
	infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));
	infotm_nftl_scan_bbt(infotm_nftl_info);
	nand_bbt_info = &infotm_nftl_info->nand_bbt_info;
	infotm_nftl_info->fillfactor = nand_bbt_info->total_bad_num;
	infotm_nftl_info->fillfactor += NFTL_BASIC_FILLFACTOR;
	infotm_nftl_info->accessibleblocks = size_in_blk - infotm_nftl_info->fillfactor;

	if (infotm_nand_save->nftl_node_magic == RRTB_NFTL_NODESAVE_MAGIC) {

		phy_blk_num = infotm_nand_save->nftl_node_blk;
		if ((phy_node_size > infotm_nftl_info->pages_per_blk) && (infotm_nftl_info->pages_per_blk < 0x100)) {
		    phy_node_size = (sizeof(struct phyblk_node_save_t) * size_in_blk);
		    phy_node_size = (phy_node_size + sizeof(uint32_t) + infotm_nftl_info->writesize - 1) / infotm_nftl_info->writesize;
		    if (phy_node_size <= infotm_nftl_info->pages_per_blk) {
			    phy_node_compressed = 1;
		       	data_buf = infotm_nftl_malloc(phy_node_size*mtd->writesize);
		       	if (data_buf == NULL) {
		       	    infotm_nftl_dbg("nftl not enough mem for savenode: %d \n", phy_blk_num);
		       	}
		    }
		} else {
    	    data_buf = (unsigned char *)infotm_nftl_info->phypmt;
    	}
		if ((data_buf != NULL) && (phy_node_size <= infotm_nftl_info->pages_per_blk)) {

			for (i=0; i<phy_node_size; i++) {
				error = infotm_nftl_ops->read_page(infotm_nftl_info, phy_blk_num, i, data_buf+i*infotm_nftl_info->writesize, nftl_oob_buf, oob_len);
				if (error < 0) {
					infotm_nftl_dbg("get reboot save phy bad : %d %x\n", phy_blk_num, i);
		       		error = infotm_nftl_ops->erase_block(infotm_nftl_info, phy_blk_num);
		       		if (error)
		       		    infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
		       		break;
				}
			}
			if (i >= phy_node_size) {
			    if (phy_node_compressed) {
			    	crc32_phy_cacl = (crc32((0 ^ 0xffffffffL), data_buf, (sizeof(struct phyblk_node_save_t) * size_in_blk)) ^ 0xffffffffL);
			        crc32_phy_save = *(unsigned *)(data_buf + (sizeof(struct phyblk_node_save_t) * size_in_blk));
			    } else {
			    	crc32_phy_cacl = (crc32((0 ^ 0xffffffffL), data_buf, (sizeof(struct phyblk_node_t) * size_in_blk)) ^ 0xffffffffL);
			        crc32_phy_save = *(unsigned *)(data_buf + (sizeof(struct phyblk_node_t) * size_in_blk));
			    }
			    if (crc32_phy_save == crc32_phy_cacl) {
			        if (phy_node_compressed) {
			            for (j=0; j<size_in_blk; j++) {
			                phy_blk_node = &infotm_nftl_info->phypmt[j];
			                phy_blk_node_save = (struct phyblk_node_save_t *)(data_buf + j*sizeof(struct phyblk_node_save_t));
			                phy_blk_node->ec = phy_blk_node_save->ec;
			                phy_blk_node->valid_sects = phy_blk_node_save->valid_sects;
			                phy_blk_node->vtblk = phy_blk_node_save->vtblk;
			                phy_blk_node->last_write = phy_blk_node_save->last_write;
			                phy_blk_node->timestamp = phy_blk_node_save->timestamp;
							phy_blk_node->status_page = phy_blk_node_save->status_page;
			                for (k=0; k<infotm_nftl_info->pages_per_blk; k++) {
			                    if (phy_blk_node_save->phy_page_map[k] >= 0)
			                        phy_blk_node->phy_page_map[k] = (phy_blk_node_save->phy_page_map[k]&0xff);
			                }
			            }
			        }
			        phy_node_saved = 1;
			        infotm_nftl_info->current_save_node_block = phy_blk_num;
			        if (phy_node_compressed)
			            infotm_nftl_free(data_buf);
			        infotm_nftl_ops->erase_block(infotm_nftl_info, phy_blk_num);

					phy_blk_node = &infotm_nftl_info->phypmt[infotm_nftl_info->current_save_node_block];
					phy_blk_node->ec++;
					WARN_ON(phy_blk_node->vtblk != SAVE_NODE_VTBLK);
					nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
					nftl_oob_info->ec = phy_blk_node->ec;
					nftl_oob_info->vtblk = SAVE_NODE_FREE_VTBLK;
					nftl_oob_info->timestamp = phy_blk_node->timestamp;
					nftl_oob_info->status_page = 1;
					nftl_oob_info->sect = 0;
					phy_blk_node->vtblk = SAVE_NODE_FREE_VTBLK;
					oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_PHY_NODE_FREE_MAGIC)));
					if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_PHY_NODE_FREE_MAGIC)))
						memcpy((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_PHY_NODE_FREE_MAGIC, strlen(INFOTM_PHY_NODE_FREE_MAGIC));

					nftl_data_buf = infotm_nftl_info->copy_page_buf;
					memcpy(nftl_data_buf, infotm_nftl_info->phypmt, infotm_nftl_info->writesize);
					infotm_nftl_ops->write_page(infotm_nftl_info, infotm_nftl_info->current_save_node_block, 0, nftl_data_buf, nftl_oob_buf, oob_len);
					phy_blk_node->valid_sects++;
					infotm_nand_save->nftl_node_magic = 0;
					infotm_nand_save->nftl_node_blk = -1;
			        infotm_nftl_dbg("nftl found phy node save : %d %d \n", phy_blk_num, phy_node_compressed);
			    }
			} else {
			    memset((unsigned char *)infotm_nftl_info->phypmt, 0xff, phy_node_size * infotm_nftl_info->writesize);
			    if (phy_node_compressed)
			        infotm_nftl_free(data_buf);
			}
		}
	}

	for (phy_blk_num=(infotm_chip->reserved_pages / (infotm_chip->plane_num * infotm_nftl_info->pages_per_blk)); phy_blk_num<size_in_blk; phy_blk_num++) {

		phy_page_addr = 0;
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_num];

		if (((infotm_nand_save->uboot0_blk[0] > 0) && (phy_blk_num == (infotm_nand_save->uboot0_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk))))
			|| ((infotm_nand_save->uboot1_blk[0] > 0) && (phy_blk_num == (infotm_nand_save->uboot1_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk))))
			|| ((infotm_nand_save->item_blk[0] > 0) && (phy_blk_num == (infotm_nand_save->item_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk))))
			|| ((infotm_nand_save->bbt_blk[0] > 0) && (phy_blk_num == infotm_nand_save->bbt_blk[0])))
			continue;

        if (phy_node_saved) {
            if (phy_blk_node->valid_sects == 0) {
                infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);
            } else if (phy_blk_node->vtblk >= 0) {
                nftl_oob_info->status_page = 1;
                nftl_oob_info->ec = phy_blk_node->ec;
	            nftl_oob_info->vtblk = phy_blk_node->vtblk;
                nftl_oob_info->timestamp = phy_blk_node->timestamp;
                if ((nftl_oob_info->vtblk == SAVE_NODE_VTBLK)
                	 || (nftl_oob_info->vtblk == SAVE_NODE_FREE_VTBLK)
                	 || (nftl_oob_info->vtblk == UBOOT0_VTBLK)
                	 || (nftl_oob_info->vtblk == UBOOT1_VTBLK)
                	 || (nftl_oob_info->vtblk == ITEM_VTBLK)
                	 || (nftl_oob_info->vtblk == BBT_VTBLK)) {
					infotm_nftl_wl->add_free(infotm_nftl_wl, phy_blk_num);
                } else {
	    			infotm_nftl_add_block(infotm_nftl_info, phy_blk_num, nftl_oob_info);
	    			infotm_nftl_wl->add_used(infotm_nftl_wl, phy_blk_num);
	    		}
            }
        } else {
    		error = infotm_nftl_info->blk_isbad(infotm_nftl_info, phy_blk_num);
    		if (error) {
    			//infotm_nftl_info->accessibleblocks--;
    			phy_blk_node->status_page = STATUS_BAD_BLOCK;
    			//infotm_nftl_dbg("nftl detect bad blk at : %d \n", phy_blk_num);
    			continue;
    		}

    		error = infotm_nftl_info->get_page_info(infotm_nftl_info, phy_blk_num, phy_page_addr, nftl_oob_buf, oob_len);
    		if (error) {
    			//infotm_nftl_info->accessibleblocks--;
    			phy_blk_node->status_page = STATUS_BAD_BLOCK;
    			infotm_nftl_dbg("get status error at blk: %d \n", phy_blk_num);
    			continue;
    		}

			if (!strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_PHY_NODE_FREE_MAGIC, strlen((const char*)INFOTM_PHY_NODE_FREE_MAGIC))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_UBOOT0_STRING, strlen((const char*)INFOTM_UBOOT0_STRING))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_UBOOT1_STRING, strlen((const char*)INFOTM_UBOOT1_STRING))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_ITEM_STRING, strlen((const char*)INFOTM_ITEM_STRING))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_BBT_STRING, strlen((const char*)INFOTM_BBT_STRING))) {
				phy_blk_node->vtblk = nftl_oob_info->vtblk;
				phy_blk_node->ec = nftl_oob_info->ec;
				phy_blk_node->timestamp = nftl_oob_info->timestamp;
				infotm_nftl_wl->add_free(infotm_nftl_wl, phy_blk_num);
				continue;
			}

			if (!strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_PHY_NODE_SAVE_MAGIC, strlen((const char*)INFOTM_PHY_NODE_SAVE_MAGIC))) {
		        infotm_nftl_info->current_save_node_block = phy_blk_num;
		        infotm_nand_save->nftl_node_magic = RRTB_NFTL_NODESAVE_MAGIC;
		        infotm_nand_save->nftl_node_blk = phy_blk_num;
				continue;
			}

    		if (nftl_oob_info->status_page == 0) {
    			//infotm_nftl_info->accessibleblocks--;
    			phy_blk_node->status_page = STATUS_BAD_BLOCK;
    			infotm_nftl_dbg("get status faile at blk: %d \n", phy_blk_num);
    			infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
    			continue;
    		}

            memset(nftl_oob_cmp_buf, 0xff, mtd->oobavail);
            if (!memcmp(nftl_oob_buf, nftl_oob_cmp_buf, oob_len)) {
    			phy_blk_node->valid_sects = 0;
    			phy_blk_node->ec = 0;
    			infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);	
    		} else if ((nftl_oob_info->vtblk < 0) || (nftl_oob_info->vtblk >= (size_in_blk - infotm_nftl_info->fillfactor))) {
    			infotm_nftl_dbg("nftl invalid vtblk: %d \n", nftl_oob_info->vtblk);
    			error = infotm_nftl_info->erase_block(infotm_nftl_info, phy_blk_num);
    			if (error) {
    				//infotm_nftl_info->accessibleblocks--;
    				phy_blk_node->status_page = STATUS_BAD_BLOCK;
    				infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
    			} else {
    				phy_blk_node->valid_sects = 0;
    				infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);
    			}
    		} else {
    			if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC))) {
    				if (memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC))) {
    					infotm_nftl_dbg("nftl invalid magic vtblk: %d \n", nftl_oob_info->vtblk);
    					error = infotm_nftl_info->erase_block(infotm_nftl_info, phy_blk_num);
    					if (error) {
    						//infotm_nftl_info->accessibleblocks--;
    						phy_blk_node->status_page = STATUS_BAD_BLOCK;
    						infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
    					} else {
    						phy_blk_node->valid_sects = 0;
    						phy_blk_node->ec = 0;
    						infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);
    					}
    					continue;
    				}
    			}
    			infotm_nftl_add_block(infotm_nftl_info, phy_blk_num, nftl_oob_info);
    			infotm_nftl_wl->add_used(infotm_nftl_wl, phy_blk_num);
    		}
    	}
	}

	if (imapx800_chip->drv_ver_changed) {
		infotm_nftl_update_bbt(infotm_nftl_info);
		imapx800_chip->drv_ver_changed = 0;
	}
	infotm_nftl_info->isinitialised = 0;
	infotm_nftl_info->cur_split_blk = 0;
	if (phy_node_saved) {
	    infotm_nftl_info->default_split_unit = infotm_nftl_info->accessibleblocks / 8;
    } else {
        infotm_nftl_info->default_split_unit = 4;
        infotm_nftl_check_conflict_node(infotm_nftl_info);
    }
	infotm_nftl_wl->gc_start_block = infotm_nftl_info->accessibleblocks - 1;

	infotm_nftl_blk->actual_size = infotm_nftl_info->accessibleblocks;
	infotm_nftl_blk->actual_size *= mtd->erasesize;
	infotm_nftl_dbg("infotm nftl initilize completely dev size: 0x%llx %d\n", infotm_nftl_blk->actual_size, infotm_nftl_wl->free_root.count);

    /*setup class*/
	infotm_nftl_info->cls.name = kzalloc(strlen((const char*)INFOTM_NFTL_MAGIC)+1, GFP_KERNEL);

    strcpy((char *)infotm_nftl_info->cls.name, INFOTM_NFTL_MAGIC);
    infotm_nftl_info->cls.class_attrs = nftl_class_attrs;
   	error = class_register(&infotm_nftl_info->cls);
	if(error)
		infotm_nftl_dbg(" class register nand_class fail!\n");

	return 0;
}

void infotm_nftl_reinitialize(struct infotm_nftl_blk_t *infotm_nftl_blk, addr_blk_t start_blk, addr_blk_t end_blk)
{
	struct mtd_info *mtd = infotm_nftl_blk->mtd;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct infotm_nftl_ops_t *infotm_nftl_ops = infotm_nftl_info->infotm_nftl_ops;
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_free;
	struct phyblk_node_t *phy_blk_node;
	struct nftl_oobinfo_t *nftl_oob_info;
	struct infotm_nftl_wl_t *infotm_nftl_wl;
	struct phyblk_node_save_t *phy_blk_node_save;
	struct infotm_nand_para_save *infotm_nand_save;
	int error = 0, phy_blk_num, vt_blk_num, oob_len, i, j, k, phy_node_size, phy_node_saved = 0, phy_node_compressed = 0;
	uint32_t phy_page_addr, size_in_blk, crc32_phy_save, crc32_phy_cacl;
	uint32_t phys_erase_shift, phys_write_shift;
	unsigned char *data_buf = NULL, *nftl_data_buf;
	unsigned char nftl_oob_buf[mtd->oobavail];
	unsigned char nftl_oob_cmp_buf[mtd->oobavail];

	phys_erase_shift = ffs(mtd->erasesize) - 1;
	phys_write_shift = ffs(mtd->writesize) - 1;
	size_in_blk =  (mtd->size >> phys_erase_shift);
	phy_node_size = (sizeof(struct phyblk_node_t) * size_in_blk);
	phy_node_size = ((phy_node_size + sizeof(uint32_t) + infotm_nftl_info->writesize - 1) >> phys_write_shift);
	infotm_nand_save = (struct infotm_nand_para_save *)infotm_nftl_info->resved_mem;
	infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;

	nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
	oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC)));

	if (infotm_nand_save->nftl_node_magic == RRTB_NFTL_NODESAVE_MAGIC) {

		phy_blk_num = infotm_nand_save->nftl_node_blk;
		if ((phy_node_size > infotm_nftl_info->pages_per_blk) && (infotm_nftl_info->pages_per_blk < 0x100)) {
		    phy_node_size = (sizeof(struct phyblk_node_save_t) * size_in_blk);
		    phy_node_size = (phy_node_size + sizeof(uint32_t) + infotm_nftl_info->writesize - 1) / infotm_nftl_info->writesize;
		    if (phy_node_size <= infotm_nftl_info->pages_per_blk) {
			    phy_node_compressed = 1;
		       	data_buf = infotm_nftl_malloc(phy_node_size*mtd->writesize);
		       	if (data_buf == NULL) {
		       	    infotm_nftl_dbg("nftl not enough mem for savenode: %d \n", phy_blk_num);
		       	}
		    }
		} else {
    	    data_buf = (unsigned char *)infotm_nftl_info->phypmt;
    	}
		if ((data_buf != NULL) && (phy_node_size <= infotm_nftl_info->pages_per_blk)) {

			for (i=0; i<phy_node_size; i++) {
				error = infotm_nftl_ops->read_page(infotm_nftl_info, phy_blk_num, i, data_buf+i*infotm_nftl_info->writesize, nftl_oob_buf, oob_len);
				if (error < 0) {
					infotm_nftl_dbg("get reboot save phy bad : %d %x\n", phy_blk_num, i);
		       		error = infotm_nftl_ops->erase_block(infotm_nftl_info, phy_blk_num);
		       		if (error)
		       		    infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
		       		break;
				}
			}
			if (i >= phy_node_size) {
			    if (phy_node_compressed) {
			    	crc32_phy_cacl = (crc32((0 ^ 0xffffffffL), data_buf, (sizeof(struct phyblk_node_save_t) * size_in_blk)) ^ 0xffffffffL);
			        crc32_phy_save = *(unsigned *)(data_buf + (sizeof(struct phyblk_node_save_t) * size_in_blk));
			    } else {
			    	crc32_phy_cacl = (crc32((0 ^ 0xffffffffL), data_buf, (sizeof(struct phyblk_node_t) * size_in_blk)) ^ 0xffffffffL);
			        crc32_phy_save = *(unsigned *)(data_buf + (sizeof(struct phyblk_node_t) * size_in_blk));
			    }
			    if (crc32_phy_save == crc32_phy_cacl) {
			        if (phy_node_compressed) {
			            for (j=0; j<size_in_blk; j++) {
			                phy_blk_node = &infotm_nftl_info->phypmt[j];
			                phy_blk_node_save = (struct phyblk_node_save_t *)(data_buf + j*sizeof(struct phyblk_node_save_t));
			                phy_blk_node->ec = phy_blk_node_save->ec;
			                phy_blk_node->valid_sects = phy_blk_node_save->valid_sects;
			                phy_blk_node->vtblk = phy_blk_node_save->vtblk;
			                phy_blk_node->last_write = phy_blk_node_save->last_write;
			                phy_blk_node->timestamp = phy_blk_node_save->timestamp;
							phy_blk_node->status_page = phy_blk_node_save->status_page;
			                for (k=0; k<infotm_nftl_info->pages_per_blk; k++) {
			                    if (phy_blk_node_save->phy_page_map[k] >= 0)
			                        phy_blk_node->phy_page_map[k] = (phy_blk_node_save->phy_page_map[k]&0xff);
			                }
			            }
			        }
			        phy_node_saved = 1;
			        infotm_nftl_info->current_save_node_block = phy_blk_num;
			        if (phy_node_compressed)
			            infotm_nftl_free(data_buf);
			        infotm_nftl_ops->erase_block(infotm_nftl_info, phy_blk_num);

					phy_blk_node = &infotm_nftl_info->phypmt[infotm_nftl_info->current_save_node_block];
					phy_blk_node->ec++;
					WARN_ON(phy_blk_node->vtblk != SAVE_NODE_VTBLK);
					nftl_oob_info = (struct nftl_oobinfo_t *)nftl_oob_buf;
					nftl_oob_info->ec = phy_blk_node->ec;
					nftl_oob_info->vtblk = SAVE_NODE_FREE_VTBLK;
					nftl_oob_info->timestamp = phy_blk_node->timestamp;
					nftl_oob_info->status_page = 1;
					nftl_oob_info->sect = 0;
					phy_blk_node->vtblk = SAVE_NODE_FREE_VTBLK;
					oob_len = min(infotm_nftl_info->oobsize, (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_PHY_NODE_FREE_MAGIC)));
					if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_PHY_NODE_FREE_MAGIC)))
						memcpy((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_PHY_NODE_FREE_MAGIC, strlen(INFOTM_PHY_NODE_FREE_MAGIC));

					nftl_data_buf = infotm_nftl_info->copy_page_buf;
					memcpy(nftl_data_buf, infotm_nftl_info->phypmt, infotm_nftl_info->writesize);
					infotm_nftl_ops->write_page(infotm_nftl_info, infotm_nftl_info->current_save_node_block, 0, nftl_data_buf, nftl_oob_buf, oob_len);
					phy_blk_node->valid_sects++;
					infotm_nand_save->nftl_node_magic = 0;
					infotm_nand_save->nftl_node_blk = -1;
			        infotm_nftl_dbg("nftl found phy node save : %d %d \n", phy_blk_num, phy_node_compressed);
			    }
			} else {
			    memset((unsigned char *)infotm_nftl_info->phypmt, 0xff, phy_node_size * infotm_nftl_info->writesize);
			    if (phy_node_compressed)
			        infotm_nftl_free(data_buf);
			}
		}
	}

	for (vt_blk_num=0; vt_blk_num<infotm_nftl_info->accessibleblocks; vt_blk_num++) {

		vt_blk_node = (struct vtblk_node_t *)(*(infotm_nftl_info->vtpmt + vt_blk_num));
		while (vt_blk_node != NULL) {
	
			vt_blk_node_free = vt_blk_node;
			vt_blk_node = vt_blk_node->next;
			infotm_nftl_free(vt_blk_node_free);
		}

		*(infotm_nftl_info->vtpmt + vt_blk_num) = NULL;
	}
	memset((unsigned char *)infotm_nftl_info->phypmt, 0xff, sizeof(struct phyblk_node_t)*size_in_blk);
	infotm_nftl_wl_reinit(infotm_nftl_info, start_blk, end_blk);
	for (phy_blk_num=(infotm_chip->reserved_pages / (infotm_chip->plane_num * infotm_nftl_info->pages_per_blk)); phy_blk_num<size_in_blk; phy_blk_num++) {

		phy_page_addr = 0;
		phy_blk_node = &infotm_nftl_info->phypmt[phy_blk_num];

		if (((infotm_nand_save->uboot0_blk[0] > 0) && (phy_blk_num == (infotm_nand_save->uboot0_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk))))
			|| ((infotm_nand_save->uboot1_blk[0] > 0) && (phy_blk_num == (infotm_nand_save->uboot1_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk))))
			|| ((infotm_nand_save->item_blk[0] > 0) && (phy_blk_num == (infotm_nand_save->item_blk[0] / (infotm_chip->plane_num * infotm_nftl_wl->pages_per_blk))))
			|| ((infotm_nand_save->bbt_blk[0] > 0) && (phy_blk_num == infotm_nand_save->bbt_blk[0])))
			continue;

        if (phy_node_saved) {
            if (phy_blk_node->valid_sects == 0) {
                infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);
            } else if (phy_blk_node->vtblk >= 0) {
                nftl_oob_info->status_page = 1;
                nftl_oob_info->ec = phy_blk_node->ec;
	            nftl_oob_info->vtblk = phy_blk_node->vtblk;
                nftl_oob_info->timestamp = phy_blk_node->timestamp;
                if ((nftl_oob_info->vtblk == SAVE_NODE_VTBLK)
                	 || (nftl_oob_info->vtblk == SAVE_NODE_FREE_VTBLK)
                	 || (nftl_oob_info->vtblk == UBOOT0_VTBLK)
                	 || (nftl_oob_info->vtblk == UBOOT1_VTBLK)
                	 || (nftl_oob_info->vtblk == ITEM_VTBLK)
                	 || (nftl_oob_info->vtblk == BBT_VTBLK)) {
					infotm_nftl_wl->add_free(infotm_nftl_wl, phy_blk_num);
                } else {
	    			infotm_nftl_add_block(infotm_nftl_info, phy_blk_num, nftl_oob_info);
	    			infotm_nftl_wl->add_used(infotm_nftl_wl, phy_blk_num);
	    		}
            }
        } else {
    		error = infotm_nftl_info->blk_isbad(infotm_nftl_info, phy_blk_num);
    		if (error) {
    			//infotm_nftl_info->accessibleblocks--;
    			phy_blk_node->status_page = STATUS_BAD_BLOCK;
    			//infotm_nftl_dbg("nftl detect bad blk at : %d \n", phy_blk_num);
    			continue;
    		}

    		error = infotm_nftl_info->get_page_info(infotm_nftl_info, phy_blk_num, phy_page_addr, nftl_oob_buf, oob_len);
    		if (error) {
    			//infotm_nftl_info->accessibleblocks--;
    			phy_blk_node->status_page = STATUS_BAD_BLOCK;
    			infotm_nftl_dbg("get status error at blk: %d \n", phy_blk_num);
    			continue;
    		}

			if (!strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_PHY_NODE_FREE_MAGIC, strlen((const char*)INFOTM_PHY_NODE_FREE_MAGIC))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_UBOOT0_STRING, strlen((const char*)INFOTM_UBOOT0_STRING))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_UBOOT1_STRING, strlen((const char*)INFOTM_UBOOT1_STRING))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_ITEM_STRING, strlen((const char*)INFOTM_ITEM_STRING))
				|| !strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_BBT_STRING, strlen((const char*)INFOTM_BBT_STRING))) {
				phy_blk_node->vtblk = nftl_oob_info->vtblk;
				phy_blk_node->ec = nftl_oob_info->ec;
				phy_blk_node->timestamp = nftl_oob_info->timestamp;
				infotm_nftl_wl->add_free(infotm_nftl_wl, phy_blk_num);
				continue;
			}

			if (!strncmp((char*)nftl_oob_buf + sizeof(struct nftl_oobinfo_t), INFOTM_PHY_NODE_SAVE_MAGIC, strlen((const char*)INFOTM_PHY_NODE_SAVE_MAGIC))) {
		        infotm_nftl_info->current_save_node_block = phy_blk_num;
		        infotm_nand_save->nftl_node_magic = RRTB_NFTL_NODESAVE_MAGIC;
		        infotm_nand_save->nftl_node_blk = phy_blk_num;
				continue;
			}

    		if (nftl_oob_info->status_page == 0) {
    			//infotm_nftl_info->accessibleblocks--;
    			phy_blk_node->status_page = STATUS_BAD_BLOCK;
    			infotm_nftl_dbg("get status faile at blk: %d \n", phy_blk_num);
    			infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
    			continue;
    		}

            memset(nftl_oob_cmp_buf, 0xff, mtd->oobavail);
            if (!memcmp(nftl_oob_buf, nftl_oob_cmp_buf, oob_len)) {
    			phy_blk_node->valid_sects = 0;
    			phy_blk_node->ec = 0;
    			infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);	
    		} else if ((nftl_oob_info->vtblk < 0) || (nftl_oob_info->vtblk >= (size_in_blk - infotm_nftl_info->fillfactor))) {
    			infotm_nftl_dbg("nftl invalid vtblk: %d \n", nftl_oob_info->vtblk);
    			error = infotm_nftl_info->erase_block(infotm_nftl_info, phy_blk_num);
    			if (error) {
    				//infotm_nftl_info->accessibleblocks--;
    				phy_blk_node->status_page = STATUS_BAD_BLOCK;
    				infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
    			} else {
    				phy_blk_node->valid_sects = 0;
    				infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);
    			}
    		} else {
    			if (infotm_nftl_info->oobsize >= (sizeof(struct nftl_oobinfo_t) + strlen(INFOTM_NFTL_MAGIC))) {
    				if (memcmp((nftl_oob_buf + sizeof(struct nftl_oobinfo_t)), INFOTM_NFTL_MAGIC, strlen(INFOTM_NFTL_MAGIC))) {
    					infotm_nftl_dbg("nftl invalid magic vtblk: %d \n", nftl_oob_info->vtblk);
    					error = infotm_nftl_info->erase_block(infotm_nftl_info, phy_blk_num);
    					if (error) {
    						//infotm_nftl_info->accessibleblocks--;
    						phy_blk_node->status_page = STATUS_BAD_BLOCK;
    						infotm_nftl_info->blk_mark_bad(infotm_nftl_info, phy_blk_num);
    					} else {
    						phy_blk_node->valid_sects = 0;
    						phy_blk_node->ec = 0;
    						infotm_nftl_wl->add_erased(infotm_nftl_wl, phy_blk_num);
    					}
    					continue;
    				}
    			}
    			infotm_nftl_add_block(infotm_nftl_info, phy_blk_num, nftl_oob_info);
    			infotm_nftl_wl->add_used(infotm_nftl_wl, phy_blk_num);
    		}
    	}
	}

	infotm_nftl_info->isinitialised = 0;
	infotm_nftl_info->cur_split_blk = 0;
	infotm_nftl_info->default_split_unit = 8;
	return;
}

void infotm_nftl_info_release(struct infotm_nftl_info_t *infotm_nftl_info)
{
	if (infotm_nftl_info->vtpmt)
		infotm_nftl_free(infotm_nftl_info->vtpmt);
	if (infotm_nftl_info->phypmt)
		infotm_nftl_free(infotm_nftl_info->phypmt);
	if (infotm_nftl_info->infotm_nftl_wl)
		infotm_nftl_free(infotm_nftl_info->infotm_nftl_wl);
}

