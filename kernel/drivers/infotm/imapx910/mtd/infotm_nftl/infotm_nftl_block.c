/*
 * Infotm nftl block device access
 *
 * (C) 2013 2
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/mii.h>
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/device.h>
#include <linux/pagemap.h>

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mutex.h>

#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/freezer.h>
#include <linux/spinlock.h>
#include <linux/hdreg.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>

#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/suspend.h>
#include <linux/reboot.h>

#include <linux/kmod.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/scatterlist.h>
#include <mach/items.h>
#include <mach/nand.h>

#include "infotm_nftl.h"

static const char *infotm_nftl_parts[] = {
	"part.reserved", 
    "part.system", 
    "part.misc", 
    "part.cache", 
    "part.userdata",
    "part.ums" 
};

static const int infotm_nftl_parts_size[6] = {112, 512, 32, 32, 512, -1};
static struct infotm_nftl_part_t *infotm_nftl_part_save[16];

#define nftl_notifier_to_blk(l)	container_of(l, struct infotm_nftl_blk_t, nb)
#define cache_list_to_node(l)	container_of(l, struct write_cache_node, list)
#define free_list_to_node(l)	container_of(l, struct free_sects_list, list)
#define mbd_to_nftl_part(l)	    container_of(l, struct infotm_nftl_part_t, mbd)

static int infotm_nftl_search_cache_list(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, uint32_t sect_addr,
										uint32_t blk_pos, uint32_t blk_num, unsigned char *buf)
{
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct write_cache_node *cache_node;
	int err = 0, i, j;
	uint32_t blks_per_sect;
	struct list_head *l, *n;

	blks_per_sect = infotm_nftl_info->writesize / 512;
	list_for_each_safe(l, n, &infotm_nftl_blk->cache_list) {
		cache_node = cache_list_to_node(l);

		if (cache_node->vt_sect_addr == sect_addr) {
			for (i=blk_pos; i<(blk_pos+blk_num); i++) {
				if (!cache_node->cache_fill_status[i])
					break;
			}
			if (i < (blk_pos + blk_num)) {
				infotm_nftl_blk->cache_sect_addr = -1;
				err = infotm_nftl_info->read_sect(infotm_nftl_info, sect_addr, infotm_nftl_blk->cache_buf);
				if (err)
					return err;
				infotm_nftl_blk->cache_sect_addr = sect_addr;

				for (j=0; j<blks_per_sect; j++) {
					if (cache_node->cache_fill_status[j])
						memcpy(infotm_nftl_blk->cache_buf + j*512, cache_node->buf + j*512, 512);
				}
				memcpy(buf, infotm_nftl_blk->cache_buf + blk_pos * 512, blk_num * 512);
			} else {
				memcpy(buf, cache_node->buf + blk_pos * 512, blk_num * 512);
			}

			return 0;
		}
	}

	return 1;
}

static void infotm_nftl_search_free_list(struct infotm_nftl_blk_t *infotm_nftl_blk, uint32_t sect_addr,
										uint32_t blk_pos, uint32_t blk_num)
{
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct free_sects_list *free_list;
	int i;
	uint32_t blks_per_sect;
	struct list_head *l, *n;

	blks_per_sect = infotm_nftl_info->writesize / 512;
	list_for_each_safe(l, n, &infotm_nftl_blk->free_list) {
		free_list = free_list_to_node(l);

		if (free_list->vt_sect_addr == sect_addr) {
			for (i=blk_pos; i<(blk_pos+blk_num); i++) {
				if (free_list->free_blk_status[i])
					free_list->free_blk_status[i] = 0;
			}

			for (i=0; i<blks_per_sect; i++) {
				if (free_list->free_blk_status[i])
					break;;
			}
			if (i >= blks_per_sect) {
				list_del(&free_list->list);
				infotm_nftl_free(free_list);
				return;
			}
			return;
		}
	}

	return;
}

static int infotm_nftl_flush_cache(struct write_cache_node *cache_node)
{
	struct infotm_nftl_part_t *infotm_nftl_part = (struct infotm_nftl_part_t *)cache_node->priv;
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	uint32_t blks_per_sect;
	int i, j, err = 0;

	blks_per_sect = infotm_nftl_info->writesize / 512;
	for (i=0; i<blks_per_sect; i++) {
		if (!cache_node->cache_fill_status[i])
			break;
	}

	if (i < blks_per_sect) {
		infotm_nftl_blk->cache_sect_addr = -1;
		err = infotm_nftl_info->read_sect(infotm_nftl_info, cache_node->vt_sect_addr, infotm_nftl_blk->cache_buf);
		if (err)
			goto exit;

		for (j=0; j<blks_per_sect; j++) {
			if (!cache_node->cache_fill_status[j]) {
				memcpy(cache_node->buf + j*512, infotm_nftl_blk->cache_buf + j*512, 512);
				cache_node->cache_fill_status[j] = 1;
			}
		}
	}
	if (infotm_nftl_blk->cache_sect_addr == cache_node->vt_sect_addr)
		memcpy(infotm_nftl_blk->cache_buf, cache_node->buf, 512 * blks_per_sect);

	err = infotm_nftl_info->write_sect(infotm_nftl_info, cache_node->vt_sect_addr, cache_node->buf);
	if (err)
		goto exit;

	cache_node->node_status = NFTL_NODE_WRITED;
	infotm_nftl_part->cache_buf_cnt--;
	infotm_nftl_blk->total_cache_buf_cnt--;
	if (infotm_nftl_blk->total_cache_buf_cnt == 0) {
	    del_timer(&infotm_nftl_blk->cache_timer);
	    infotm_nftl_blk->cache_time_expired = 0;
	}

exit:
	return err;
}

static void infotm_nftl_flush_timer(unsigned long data)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)data;

	infotm_nftl_blk->cache_time_expired = 1;
	wake_up_process(infotm_nftl_blk->nftl_thread);

	return;
}

static int infotm_nftl_add_cache_list(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, uint32_t sect_addr,
										uint32_t blk_pos, uint32_t blk_num, unsigned char *buf)
{
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct write_cache_node *cache_node, *cache_node_temp;
	int err = 0, i, j, cache_found = 0, cache_flush_cnt = 0;
	uint32_t blks_per_sect;
	struct list_head *l, *n;

	blks_per_sect = infotm_nftl_info->writesize / 512;
	list_for_each_safe(l, n, &infotm_nftl_blk->cache_list) {
		cache_node = cache_list_to_node(l);

		if (cache_node->vt_sect_addr == sect_addr) {

			if (cache_node->node_status == NFTL_NODE_WRITED) {
				list_del(&cache_node->list);
				goto add_node;
			}

			for (j=0; j<blks_per_sect; j++) {
				if (!cache_node->cache_fill_status[j])
					break;
			}

			for (i=blk_pos; i<(blk_pos+blk_num); i++)
				cache_node->cache_fill_status[i] = 1;
			memcpy(cache_node->buf + blk_pos * 512, buf, blk_num * 512);

			for (j=0; j<blks_per_sect; j++) {
				if (!cache_node->cache_fill_status[j])
					break;
			}

			if (j >= blks_per_sect) {
				if (cache_node->node_status == NFTL_NODE_WAIT_WRITE)
					infotm_nftl_flush_cache(cache_node);
			} else {
				mod_timer(&infotm_nftl_blk->cache_timer, jiffies + infotm_nftl_blk->cache_timer.expires);
			}

			goto exit;
		}
	}

	if ((blk_pos == 0) && (blk_num == blks_per_sect)) {
		err = CACHE_LIST_NOT_FOUND;
		goto exit;
	}

    if (infotm_nftl_part->cache_buf_cnt >= infotm_nftl_part->bounce_cnt) {
        list_for_each_safe(l, n, &infotm_nftl_blk->cache_list) {
            cache_node = cache_list_to_node(l);
            if (cache_node->node_status == NFTL_NODE_WAIT_WRITE) {
				if (!infotm_nftl_flush_cache(cache_node))
					cache_flush_cnt++;
			}
            if (cache_flush_cnt >= NFTL_CACHE_FLUSH_UNIT)
                break;
		} 
    }

	cache_found = 0;
	list_for_each_safe(l, n, &infotm_nftl_blk->cache_list) {
		cache_node = cache_list_to_node(l);
		if (cache_node->node_status == NFTL_NODE_FREE) {
			cache_node->priv = infotm_nftl_part;
			list_del(&cache_node->list);
			cache_found = 1;
			break;
		}
	}
	if (cache_found == 0) {
		list_for_each_safe(l, n, &infotm_nftl_blk->cache_list) {
			cache_node = cache_list_to_node(l);
			if (cache_node->node_status == NFTL_NODE_WRITED) {
				cache_node->priv = infotm_nftl_part;
				list_del(&cache_node->list);
				cache_found = 2;
				break;
			}
		}	
	}
	if (cache_found == 0) {
		infotm_nftl_dbg("nftl haven't cache node %d %d \n", infotm_nftl_part->cache_buf_cnt, infotm_nftl_blk->total_cache_buf_cnt);
		err = -ENOMEM;
		goto exit;
	}

add_node:

    if ((cache_found == 0) && (infotm_nftl_blk->total_cache_buf_cnt >= NFTL_MEDIA_FORCE_WRITE_LEN)) {
        list_for_each_safe(l, n, &infotm_nftl_blk->cache_list) {
            cache_node_temp = cache_list_to_node(l);
            if (cache_node_temp->node_status == NFTL_NODE_WAIT_WRITE) {
				if (!infotm_nftl_flush_cache(cache_node_temp))
					cache_flush_cnt++;
			}
            if (cache_flush_cnt >= NFTL_CACHE_FLUSH_UNIT)
                break;
		} 
    }

	memset(cache_node->cache_fill_status, 0, MAX_BLKS_PER_SECTOR);
	cache_node->vt_sect_addr = sect_addr;
	for (i=blk_pos; i<(blk_pos+blk_num); i++)
		cache_node->cache_fill_status[i] = 1;
	memcpy(cache_node->buf + blk_pos * 512, buf, blk_num * 512);

    if (infotm_nftl_blk->total_cache_buf_cnt == 0) {
	    infotm_nftl_blk->cache_timer.expires = NFTL_FLUSH_SEQUENCE_TIME;
    	infotm_nftl_blk->cache_timer.function = infotm_nftl_flush_timer;
    	infotm_nftl_blk->cache_timer.data = (unsigned long)infotm_nftl_blk;
    	add_timer(&infotm_nftl_blk->cache_timer);
	} else {
	    mod_timer(&infotm_nftl_blk->cache_timer, jiffies + infotm_nftl_blk->cache_timer.expires);
	}

    list_add_tail(&cache_node->list, &infotm_nftl_blk->cache_list);
	cache_node->node_status = NFTL_NODE_WAIT_WRITE;
	infotm_nftl_part->cache_buf_cnt++;
	infotm_nftl_blk->total_cache_buf_cnt++;

exit:
	return err;
}

static int infotm_nftl_read_data(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf)
{
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	unsigned blks_per_sect, read_sect_addr, read_misalign_num, buf_offset = 0;
	int i, error = 0, status = 0, total_blk_count = 0, read_sect_num;

	blks_per_sect = infotm_nftl_info->writesize / 512;
	read_sect_addr = block / blks_per_sect;
	total_blk_count = nblk;
	read_misalign_num = ((blks_per_sect - block % blks_per_sect) > nblk ?  nblk : (blks_per_sect - block % blks_per_sect));

	if (block % blks_per_sect) {
		status = infotm_nftl_blk->search_cache_list(infotm_nftl_blk, infotm_nftl_part, read_sect_addr, block % blks_per_sect, read_misalign_num, buf);
		if (status) {
			if (read_sect_addr != infotm_nftl_blk->cache_sect_addr) {
				infotm_nftl_blk->cache_sect_addr = -1;
				error = infotm_nftl_info->read_sect(infotm_nftl_info, read_sect_addr, infotm_nftl_blk->cache_buf);
				if (error)
					return error;
				infotm_nftl_blk->cache_sect_addr = read_sect_addr;
			}
			memcpy(buf + buf_offset, infotm_nftl_blk->cache_buf + (block % blks_per_sect) * 512, read_misalign_num * 512);
		}
		read_sect_addr++;
		total_blk_count -= read_misalign_num;
		buf_offset += read_misalign_num * 512;
	}

	if (total_blk_count >= blks_per_sect) {
		read_sect_num = total_blk_count / blks_per_sect;
		for (i=0; i<read_sect_num; i++) {
			status = infotm_nftl_blk->search_cache_list(infotm_nftl_blk, infotm_nftl_part, read_sect_addr, 0, blks_per_sect, buf + buf_offset);
			if (status) {
				error = infotm_nftl_info->read_sect(infotm_nftl_info, read_sect_addr, buf + buf_offset);
				if (error)
					return error;
			}
			read_sect_addr += 1;
			total_blk_count -= blks_per_sect;
			buf_offset += infotm_nftl_info->writesize;
		}
	}

	if (total_blk_count > 0) {
		status = infotm_nftl_blk->search_cache_list(infotm_nftl_blk, infotm_nftl_part, read_sect_addr, 0, total_blk_count, buf + buf_offset);
		if (status) {
			if (read_sect_addr != infotm_nftl_blk->cache_sect_addr) {
				infotm_nftl_blk->cache_sect_addr = -1;
				error = infotm_nftl_info->read_sect(infotm_nftl_info, read_sect_addr, infotm_nftl_blk->cache_buf);
				if (error)
					return error;
				infotm_nftl_blk->cache_sect_addr = read_sect_addr;
			}	
			memcpy(buf + buf_offset, infotm_nftl_blk->cache_buf, total_blk_count * 512);
		}
	}

	return 0;
}

static int infotm_nftl_write_data(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf)
{
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	unsigned blks_per_sect, write_sect_addr, write_misalign_num, buf_offset = 0;
	int error = 0, status = 0, total_blk_count = 0, write_sect_num, i;

	blks_per_sect = infotm_nftl_info->writesize / 512;
	write_sect_addr = block / blks_per_sect;
	total_blk_count = nblk;
	write_misalign_num = ((blks_per_sect - block % blks_per_sect) > nblk ? nblk : (blks_per_sect - block % blks_per_sect));
	//infotm_nftl_dbg("nftl write data %s blk: %d page: %d block: %ld count %d\n", infotm_nftl_part->part_name, (write_sect_addr / infotm_nftl_info->pages_per_blk), (write_sect_addr % infotm_nftl_info->pages_per_blk), block, nblk);

	if (block % blks_per_sect) {
		infotm_nftl_search_free_list(infotm_nftl_blk, write_sect_addr, block % blks_per_sect, write_misalign_num);
		status = infotm_nftl_blk->add_cache_list(infotm_nftl_blk, infotm_nftl_part, write_sect_addr, block % blks_per_sect, write_misalign_num, buf);
		if (status)
			return status;

		write_sect_addr++;
		total_blk_count -= write_misalign_num;
		buf_offset += write_misalign_num * 512;
	}

	if (total_blk_count >= blks_per_sect) {
		write_sect_num = total_blk_count / blks_per_sect;
		for (i=0; i<write_sect_num; i++) {
			infotm_nftl_search_free_list(infotm_nftl_blk, write_sect_addr, 0, blks_per_sect);
			status = infotm_nftl_blk->add_cache_list(infotm_nftl_blk, infotm_nftl_part, write_sect_addr, 0, blks_per_sect, buf + buf_offset);
			if ((status) && (status != CACHE_LIST_NOT_FOUND))
				return status;

			if (status == CACHE_LIST_NOT_FOUND) {
				if (infotm_nftl_blk->cache_sect_addr == write_sect_addr)
					memcpy(infotm_nftl_blk->cache_buf, buf + buf_offset, 512 * blks_per_sect);

				error = infotm_nftl_info->write_sect(infotm_nftl_info, write_sect_addr, buf + buf_offset);
				if (error)
					return error;
			}
			write_sect_addr += 1;
			total_blk_count -= blks_per_sect;
			buf_offset += infotm_nftl_info->writesize;
		}
	}

	if (total_blk_count > 0) {
		infotm_nftl_search_free_list(infotm_nftl_blk, write_sect_addr, 0, total_blk_count);
		status = infotm_nftl_blk->add_cache_list(infotm_nftl_blk, infotm_nftl_part, write_sect_addr, 0, total_blk_count, buf + buf_offset);
		if (status)
			return status;
	}

	return 0;
}

static int infotm_nftl_normal_part_read_data(struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;

	return infotm_nftl_blk->read_data(infotm_nftl_blk, infotm_nftl_part, block, nblk, buf);
}

static int infotm_nftl_normal_part_write_data(struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;

	return infotm_nftl_blk->write_data(infotm_nftl_blk, infotm_nftl_part, block, nblk, buf);
}

static int infotm_nftl_boot_part_read_data(struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct mtd_blktrans_ops *tr = infotm_nftl_part->mbd.tr;
	loff_t from;
	int error = 0;

	from = (block << tr->blkshift);
	if (from < (INFOTM_ITEM_OFFSET + INFOTM_MAGIC_PART_SIZE)) {

		error = infotm_nftl_info->read_mini_part(infotm_nftl_info, from, (nblk << tr->blkshift), buf);
	} else if (from >= (INFOTM_ITEM_OFFSET + INFOTM_MAGIC_PART_SIZE)) {

		error = infotm_nftl_blk->read_data(infotm_nftl_blk, infotm_nftl_part, block, nblk, buf);
	}

	return error;
}

static int infotm_nftl_boot_part_write_data(struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct mtd_blktrans_ops *tr = infotm_nftl_part->mbd.tr;
	loff_t from;
	int error = 0, erase_shift;

	from = (block << tr->blkshift);
    if (from < (INFOTM_ITEM_OFFSET + INFOTM_MAGIC_PART_SIZE)) {

        if (infotm_nftl_blk->uboot_write_buf == NULL) {
            infotm_nftl_blk->uboot_write_buf = infotm_nftl_malloc(INFOTM_MAGIC_PART_SIZE);
            if (infotm_nftl_blk->uboot_write_buf == NULL)
                return -ENOMEM;
            infotm_nftl_blk->uboot_write_blk = -1;
            infotm_nftl_blk->uboot_write_offs = 0;
        }

		//printk("uboot write %llx %x \n", from, (nblk << tr->blkshift));
		erase_shift = fls(NFTL_MINI_PART_SIZE) - 1;
		if ((infotm_nftl_blk->uboot_write_blk >= 0) && ((from >> erase_shift) != infotm_nftl_blk->uboot_write_blk)) {

		    error = infotm_nftl_info->write_mini_part(infotm_nftl_info, (infotm_nftl_blk->uboot_write_blk << erase_shift), infotm_nftl_blk->uboot_write_offs, infotm_nftl_blk->uboot_write_buf);
            infotm_nftl_blk->uboot_write_blk = -1;
            infotm_nftl_blk->uboot_write_offs = 0;
		}

        memcpy(infotm_nftl_blk->uboot_write_buf + (from & (INFOTM_MAGIC_PART_SIZE - 1)), buf, (nblk << tr->blkshift));
        infotm_nftl_blk->uboot_write_blk = (int)(from >> erase_shift);
        if (((from & (INFOTM_MAGIC_PART_SIZE - 1)) + (nblk << tr->blkshift)) > infotm_nftl_blk->uboot_write_offs)
        	infotm_nftl_blk->uboot_write_offs = (from & (INFOTM_MAGIC_PART_SIZE - 1)) + (nblk << tr->blkshift);

	} else if (from >= (INFOTM_ITEM_OFFSET + INFOTM_MAGIC_PART_SIZE)) {

		error = infotm_nftl_blk->write_data(infotm_nftl_blk, infotm_nftl_part, block, nblk, buf);
	}

	return error;
}

int infotm_nftl_part_write_data(loff_t offset, size_t size, unsigned char *buf)
{
	int ret = 0;
	unsigned long block,  nblk;
	struct infotm_nftl_part_t *infotm_nftl_part = (struct infotm_nftl_part_t *)infotm_nftl_part_save[0];
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	block = (int)(offset >> 9);
	nblk = (int)(size >> 9);
	ret = infotm_nftl_part->write_data(infotm_nftl_part, block, nblk, buf);
	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return ret;
}


static int infotm_nftl_flush(struct mtd_blktrans_dev *dev)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(dev);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	int error = 0;

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	infotm_nftl_dbg("nftl flush all cache data: %s %d\n", infotm_nftl_part->part_name, infotm_nftl_part->cache_buf_cnt);
	if (infotm_nftl_part->cache_buf_cnt)
		infotm_nftl_blk->write_cache_data(infotm_nftl_blk, CACHE_CLEAR_ALL);

	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return error;
}

static int infotm_nftl_write_cache_data(struct infotm_nftl_blk_t *infotm_nftl_blk, uint8_t cache_flag)
{
	struct list_head *l, *n;
	struct write_cache_node *cache_node;
	int flush_cnt = 0;

	list_for_each_safe(l, n, &infotm_nftl_blk->cache_list) {
		cache_node = cache_list_to_node(l);
		if (cache_node->node_status == NFTL_NODE_WAIT_WRITE) {
			infotm_nftl_flush_cache(cache_node);
			flush_cnt++;
			if ((cache_flag == 0) && (flush_cnt >= NFTL_CACHE_FLUSH_UNIT))
			    break;
		}
	} 

	return 0;
}

static unsigned int infotm_nftl_map_sg(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part)
{
	unsigned int sg_len;
	size_t buflen;
	struct scatterlist *sg;
	int i;

	if (!infotm_nftl_part->bounce_buf)
		return blk_rq_map_sg(infotm_nftl_part->queue, infotm_nftl_part->req, infotm_nftl_part->sg);

	sg_len = blk_rq_map_sg(infotm_nftl_part->queue, infotm_nftl_part->req, infotm_nftl_part->bounce_sg);

	infotm_nftl_part->bounce_sg_len = sg_len;

	buflen = 0;
	for_each_sg(infotm_nftl_part->bounce_sg, sg, sg_len, i)
		buflen += sg->length;

	sg_init_one(infotm_nftl_part->sg, infotm_nftl_part->bounce_buf, buflen);

	return sg_len;
}

static int infotm_nftl_calculate_sg(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, size_t buflen, unsigned **buf_addr, unsigned *offset_addr)
{
	struct scatterlist *sgl;
	unsigned int offset = 0, segments = 0, buf_start = 0;
	struct sg_mapping_iter miter;
	unsigned long flags;
	unsigned int nents;
	unsigned int sg_flags = SG_MITER_ATOMIC;

	nents = infotm_nftl_part->bounce_sg_len;
	sgl = infotm_nftl_part->bounce_sg;

	if (rq_data_dir(infotm_nftl_part->req) == WRITE)
		sg_flags |= SG_MITER_FROM_SG;
	else
		sg_flags |= SG_MITER_TO_SG;

	sg_miter_start(&miter, sgl, nents, sg_flags);

	local_irq_save(flags);

	while (offset < buflen) {
		unsigned int len;
		if(!sg_miter_next(&miter))
			break;

		if (!buf_start) {
			segments = 0;
			*(buf_addr + segments) = (unsigned *)miter.addr;
			*(offset_addr + segments) = offset;
			buf_start = 1;
		}
		else {
			if ((unsigned char *)(*(buf_addr + segments)) + (offset - *(offset_addr + segments)) != miter.addr) {
				segments++;
				*(buf_addr + segments) = (unsigned *)miter.addr;
				*(offset_addr + segments) = offset;
			}
		}

		len = min(miter.length, buflen - offset);
		offset += len;
	}
	*(offset_addr + segments + 1) = offset;

	sg_miter_stop(&miter);

	local_irq_restore(flags);

	return segments;
}

/*
 * Alloc bounce buf for read/write numbers of pages in one request
 */
static int infotm_nftl_init_bounce_buf(struct mtd_blktrans_dev *dev, struct request_queue *rq)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(dev);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct write_cache_node *cache_node;
	int ret=0, i;
	unsigned int bouncesz;
	unsigned long flags;

	infotm_nftl_part->queue = rq;
	if (infotm_nftl_part->mbd.devnum == (ARRAY_SIZE(infotm_nftl_parts) - 2)) {
    	bouncesz = (infotm_nftl_info->writesize * NFTL_DATA_FORCE_WRITE_LEN);
    	infotm_nftl_part->bounce_cnt = NFTL_DATA_FORCE_WRITE_LEN;
    } else if (infotm_nftl_part->mbd.devnum == (ARRAY_SIZE(infotm_nftl_parts) - 1)) {
    	bouncesz = (infotm_nftl_info->writesize * NFTL_MEDIA_FORCE_WRITE_LEN);
    	infotm_nftl_part->bounce_cnt = NFTL_MEDIA_FORCE_WRITE_LEN;
	} else {
    	bouncesz = (infotm_nftl_info->writesize);
    	infotm_nftl_part->bounce_cnt = 1;
    }

    if (bouncesz >= PAGE_CACHE_SIZE) {
        infotm_nftl_part->bounce_buf = infotm_nftl_malloc(bouncesz);
        if (!infotm_nftl_part->bounce_buf) {
            infotm_nftl_dbg("not enough memory for bounce buf\n");
            return -1;
        }
    }

    for (i=0; i<infotm_nftl_part->bounce_cnt; i++) {
		cache_node = infotm_nftl_malloc(sizeof(struct write_cache_node));
		if (!cache_node)
			return -ENOMEM;

		cache_node->buf = (infotm_nftl_part->bounce_buf + i*infotm_nftl_info->writesize);
		cache_node->node_status = NFTL_NODE_FREE;
		cache_node->vt_sect_addr = -1;
		list_add_tail(&cache_node->list, &infotm_nftl_blk->cache_list);
    }

	spin_lock_irqsave(rq->queue_lock, flags);
	queue_flag_test_and_set(QUEUE_FLAG_NONROT, rq);
	blk_queue_bounce_limit(infotm_nftl_part->queue, BLK_BOUNCE_HIGH);
	blk_queue_max_hw_sectors(infotm_nftl_part->queue, bouncesz / 512);
	blk_queue_physical_block_size(infotm_nftl_part->queue, bouncesz);
	blk_queue_max_segments(infotm_nftl_part->queue, bouncesz / PAGE_CACHE_SIZE);
	blk_queue_max_segment_size(infotm_nftl_part->queue, bouncesz);
	spin_unlock_irqrestore(rq->queue_lock, flags);

	infotm_nftl_part->req = NULL;
	infotm_nftl_part->sg = infotm_nftl_malloc(sizeof(struct scatterlist));
	if (!infotm_nftl_part->sg) {
		ret = -ENOMEM;
		blk_cleanup_queue(infotm_nftl_part->queue);
		return ret;
	}
	sg_init_table(infotm_nftl_part->sg, 1);

	infotm_nftl_part->bounce_sg = infotm_nftl_malloc(sizeof(struct scatterlist) * bouncesz / PAGE_CACHE_SIZE);
	if (!infotm_nftl_part->bounce_sg) {
		ret = -ENOMEM;
		infotm_nftl_free(infotm_nftl_part->sg);
		infotm_nftl_part->sg = NULL;
		blk_cleanup_queue(infotm_nftl_part->queue);
		return ret;
	}
	sg_init_table(infotm_nftl_part->bounce_sg, bouncesz / PAGE_CACHE_SIZE);

	return 0;
}

static int infotm_nftl_add_free_list(struct infotm_nftl_blk_t *infotm_nftl_blk, uint32_t sect_addr,
										uint32_t blk_pos, uint32_t blk_num)
{
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct free_sects_list *free_list;
	int i;
	uint32_t blks_per_sect;
	struct list_head *l, *n;

	blks_per_sect = infotm_nftl_info->writesize / 512;
	list_for_each_safe(l, n, &infotm_nftl_blk->free_list) {
		free_list = free_list_to_node(l);

		if (free_list->vt_sect_addr == sect_addr) {
			for (i=blk_pos; i<(blk_pos+blk_num); i++)
				free_list->free_blk_status[i] = 1;

			for (i=0; i<blks_per_sect; i++) {
				if (free_list->free_blk_status[i] == 0)
					break;;
			}
			if (i >= blks_per_sect) {
				list_del(&free_list->list);
				infotm_nftl_free(free_list);
				return 0;
			}

			return 1;
		}
	}

	if ((blk_pos == 0) && (blk_num == blks_per_sect))
		return 0;

	free_list = infotm_nftl_malloc(sizeof(struct free_sects_list));
	if (!free_list)
		return -ENOMEM;

	free_list->vt_sect_addr = sect_addr;
	for (i=blk_pos; i<(blk_pos+blk_num); i++)
		free_list->free_blk_status[i] = 1;
	list_add_tail(&free_list->list, &infotm_nftl_blk->free_list);

	return 1;
}

#if 0
static void infotm_nftl_update_blktrans_sysinfo(struct mtd_blktrans_dev *dev, unsigned int cmd, unsigned long arg)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(dev);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct fat_sectors *infotm_nftl_sects = (struct fat_sectors *)arg;
	unsigned sect_addr, blks_per_sect, nblk, blk_pos, blk_num;
	int part_sect_offset, write_size_hift;

    if (infotm_nftl_info->shutdown_flag)
        return;

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	write_size_hift = ffs(infotm_nftl_info->writesize) - 1;
	part_sect_offset = (int)(infotm_nftl_part->offset >> write_size_hift);
	nblk = infotm_nftl_sects->sectors;
	blks_per_sect = infotm_nftl_info->writesize / 512;
	sect_addr = (uint32_t)infotm_nftl_sects->start / blks_per_sect;
	blk_pos = (uint32_t)infotm_nftl_sects->start % blks_per_sect;
	//infotm_nftl_dbg("nftl_update_blktrans : %d %d %d \n", sect_addr, blk_pos, nblk);
	while (nblk > 0){
		blk_pos = blk_pos % blks_per_sect;
		blk_num = ((blks_per_sect - blk_pos) > nblk ? nblk : (blks_per_sect - blk_pos ));
		if (cmd == BLKGETSECTS) {
			infotm_nftl_search_free_list(infotm_nftl_blk, (sect_addr + part_sect_offset), blk_pos, blk_num);
		} else if (cmd == BLKFREESECTS) {
			if (!infotm_nftl_add_free_list(infotm_nftl_blk, (sect_addr + part_sect_offset), blk_pos, blk_num))
				infotm_nftl_info->delete_sect(infotm_nftl_info, (sect_addr + part_sect_offset));
		} else {
			break;
		}
		nblk -= blk_num;
		blk_pos += blk_num;
		sect_addr++;
	};

	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return;
}
#endif

static int do_nftltrans_request(struct mtd_blktrans_ops *tr,
			       struct mtd_blktrans_dev *dev,
			       struct request *req)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(dev);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	int ret = 0, segments, i;
	unsigned long block, nblk, blk_addr, blk_cnt, part_blk_offset;
	unsigned short max_segm = queue_max_segments(infotm_nftl_part->mbd.rq);
	unsigned *buf_addr[max_segm+1];
	unsigned offset_addr[max_segm+1];
	size_t buflen;
	char *buf;

	memset((unsigned char *)buf_addr, 0, (max_segm+1)*4);
	memset((unsigned char *)offset_addr, 0, (max_segm+1)*4);
	infotm_nftl_part->req = req;
	block = blk_rq_pos(req) << 9 >> tr->blkshift;
	nblk = blk_rq_sectors(req);
	buflen = (nblk << tr->blkshift);

	if (req->cmd_type != REQ_TYPE_FS)
		return -EIO;

	if ((blk_rq_pos(req) + blk_rq_cur_sectors(req)) > get_capacity(req->rq_disk))
		return -EIO;

	if (req->cmd_flags & REQ_DISCARD)
		return tr->discard(dev, block, nblk);

	infotm_nftl_part->bounce_sg_len = infotm_nftl_map_sg(infotm_nftl_blk, infotm_nftl_part);
	segments = infotm_nftl_calculate_sg(infotm_nftl_blk, infotm_nftl_part, buflen, buf_addr, offset_addr);
	if (offset_addr[segments+1] != (nblk << tr->blkshift))
		return -EIO;

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	if ((infotm_nftl_info->shutdown_flag == 1) && (rq_data_dir(req) == WRITE)) {
	    infotm_nftl_dbg("nftl shutdown err blkrequest %s \n", (rq_data_dir(req)==READ)?"read":"write");
		mutex_unlock(&infotm_nftl_blk->nftl_lock);
		return 0;
	}
	part_blk_offset = (int)(infotm_nftl_part->offset >> tr->blkshift);
	switch(rq_data_dir(req)) {
		case READ:
			for(i=0; i<(segments+1); i++) {
				blk_addr = (block + (offset_addr[i] >> tr->blkshift));
				blk_addr += part_blk_offset;
				blk_cnt = ((offset_addr[i+1] - offset_addr[i]) >> tr->blkshift);
				buf = (char *)buf_addr[i];
				if (infotm_nftl_part->read_data(infotm_nftl_part, blk_addr, blk_cnt, buf)) {
					ret = -EIO;
					break;
				}
			}
			bio_flush_dcache_pages(infotm_nftl_part->req->bio);
			break;

		case WRITE:
			bio_flush_dcache_pages(infotm_nftl_part->req->bio);
			for(i=0; i<(segments+1); i++) {
				blk_addr = (block + (offset_addr[i] >> tr->blkshift));
				blk_addr += part_blk_offset;
				blk_cnt = ((offset_addr[i+1] - offset_addr[i]) >> tr->blkshift);
				buf = (char *)buf_addr[i];
				if (infotm_nftl_part->write_data(infotm_nftl_part, blk_addr, blk_cnt, buf)) {
					ret = -EIO;
					break;
				}
			}
			break;

		default:
			infotm_nftl_dbg(KERN_NOTICE "Unknown request %u\n", rq_data_dir(req));
			break;
	}

	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return ret;
}

static int infotm_nftl_writesect(struct mtd_blktrans_dev *dev, unsigned long block, char *buf)
{
	return 0;
}

static int infotm_nftl_discard(struct mtd_blktrans_dev *dev, unsigned long block, unsigned nr_blocks)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(dev);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	unsigned sect_addr, blks_per_sect, nblk, blk_pos, blk_num;
	int part_sect_offset, write_size_hift;

    if (infotm_nftl_info->shutdown_flag)
        return 0;

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	write_size_hift = ffs(infotm_nftl_info->writesize) - 1;
	part_sect_offset = (int)(infotm_nftl_part->offset >> write_size_hift);
	nblk = nr_blocks;
	blks_per_sect = infotm_nftl_info->writesize / 512;
	sect_addr = (uint32_t)block / blks_per_sect;
	blk_pos = (uint32_t)block % blks_per_sect;
	//infotm_nftl_dbg("infotm_nftl_discard : %d %d %d \n", sect_addr, blk_pos, nblk);
	while (nblk > 0){
		blk_pos = blk_pos % blks_per_sect;
		blk_num = ((blks_per_sect - blk_pos) > nblk ? nblk : (blks_per_sect - blk_pos ));
		if (!infotm_nftl_add_free_list(infotm_nftl_blk, (sect_addr + part_sect_offset), blk_pos, blk_num))
			infotm_nftl_info->delete_sect(infotm_nftl_info, (sect_addr + part_sect_offset));
		nblk -= blk_num;
		blk_pos += blk_num;
		sect_addr++;
	};
	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return 0;
}

static int infotm_nftl_thread(void *arg)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = arg;
	unsigned long period = NFTL_MAX_SCHEDULE_TIMEOUT / 10;

	while (!kthread_should_stop()) {
		struct infotm_nftl_info_t *infotm_nftl_info;
		struct infotm_nftl_wl_t *infotm_nftl_wl;
		infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
		infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;

		mutex_lock(&infotm_nftl_blk->nftl_lock);
		if (infotm_nftl_info->shutdown_flag) {
		    infotm_nftl_dbg("nftl reboot thread kill \n");
		    mutex_unlock(&infotm_nftl_blk->nftl_lock);
		    break;
		}

		if (infotm_nftl_blk->total_cache_buf_cnt) {
			if (infotm_nftl_blk->cache_time_expired)
				infotm_nftl_blk->write_cache_data(infotm_nftl_blk, 0);
			mutex_unlock(&infotm_nftl_blk->nftl_lock);
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(2);
			continue;
		}

		if (!infotm_nftl_info->isinitialised) {

			mutex_unlock(&infotm_nftl_blk->nftl_lock);
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(10);

            if (infotm_nftl_blk->total_cache_buf_cnt)
                continue;
			mutex_lock(&infotm_nftl_blk->nftl_lock);
			if (!infotm_nftl_info->isinitialised)
			    infotm_nftl_info->creat_structure(infotm_nftl_info);
			mutex_unlock(&infotm_nftl_blk->nftl_lock);
			continue;
		}

		if (!infotm_nftl_wl->gc_need_flag) {
			mutex_unlock(&infotm_nftl_blk->nftl_lock);
			set_current_state(TASK_INTERRUPTIBLE);
			schedule();
			continue;
		}

		infotm_nftl_wl->garbage_collect(infotm_nftl_wl, DO_COPY_PAGE);
		if (infotm_nftl_wl->free_root.count >= (infotm_nftl_info->fillfactor / 2)) {
			infotm_nftl_dbg("nftl initilize garbage completely: %d \n", infotm_nftl_wl->free_root.count);
			infotm_nftl_wl->gc_need_flag = 0;
		}

		mutex_unlock(&infotm_nftl_blk->nftl_lock);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(period);
	}

	return 0;
}

static int infotm_nftl_reboot_notifier(struct notifier_block *nb, unsigned long priority, void * arg)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = nftl_notifier_to_blk(nb);
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	struct infotm_nftl_wl_t *infotm_nftl_wl = infotm_nftl_info->infotm_nftl_wl;
	int error = 0;

    mutex_lock(&infotm_nftl_blk->nftl_lock);
    if (infotm_nftl_blk->uboot_write_blk >=0) {
    	infotm_nftl_info->write_mini_part(infotm_nftl_info, (infotm_nftl_blk->uboot_write_blk * NFTL_MINI_PART_SIZE), infotm_nftl_blk->uboot_write_offs, infotm_nftl_blk->uboot_write_buf);
		infotm_nftl_blk->uboot_write_blk = -1;
		infotm_nftl_blk->uboot_write_offs = 0;
	}

	infotm_nftl_wl->gc_need_flag = 0;
	//if (infotm_nftl_blk->total_cache_buf_cnt)
	    //error = infotm_nftl_blk->write_cache_data(infotm_nftl_blk, CACHE_CLEAR_ALL);
	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return error;
}

static void infotm_nftl_shutdown(struct mtd_info *mtd)
{
    struct infotm_nftl_blk_t *infotm_nftl_blk = NULL;
	struct infotm_nftl_info_t *infotm_nftl_info = NULL;

    if (mtd) {
        infotm_nftl_blk = (struct infotm_nftl_blk_t *)mtd->uplayer_priv;
        infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
        mutex_lock(&infotm_nftl_blk->nftl_lock);
        infotm_nftl_info->shutdown_flag = 1;
        if (!infotm_nftl_info->isinitialised) {
    		infotm_nftl_info->default_split_unit = infotm_nftl_info->accessibleblocks - infotm_nftl_info->cur_split_blk + 1;
    		infotm_nftl_info->creat_structure(infotm_nftl_info);
    	}
    	if (infotm_nftl_info->isinitialised) {
			if (infotm_nftl_blk->total_cache_buf_cnt)
		    	infotm_nftl_blk->write_cache_data(infotm_nftl_blk, CACHE_CLEAR_ALL);
    		infotm_nftl_info->save_phy_node(infotm_nftl_info);
    	}
        mutex_unlock(&infotm_nftl_blk->nftl_lock);
    }
    return;
}

static int infotm_nftl_suspend(struct mtd_info *mtd)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)mtd->uplayer_priv;
	//struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	int error = 0;

	if (infotm_nftl_blk != NULL) {
		mutex_lock(&infotm_nftl_blk->nftl_lock);
		infotm_nftl_dbg("infotm_nftl_suspend: %d\n", infotm_nftl_blk->total_cache_buf_cnt);
		if (infotm_nftl_blk->total_cache_buf_cnt > 0)
			infotm_nftl_blk->write_cache_data(infotm_nftl_blk, CACHE_CLEAR_ALL);

    	/*if (infotm_nftl_blk->nftl_pm_state == 0) {
	        if (!infotm_nftl_info->isinitialised) {
	    		infotm_nftl_info->default_split_unit = infotm_nftl_info->accessibleblocks - infotm_nftl_info->cur_split_blk + 1;
	    		infotm_nftl_info->creat_structure(infotm_nftl_info);
	    	}
	    	infotm_nftl_info->save_phy_node(infotm_nftl_info);
    		infotm_nftl_blk->nftl_pm_state = 1;
    	}*/

		mutex_unlock(&infotm_nftl_blk->nftl_lock);	
	}

	return error;
}

static void infotm_nftl_resume(struct mtd_info *mtd)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)mtd->uplayer_priv;

	if (infotm_nftl_blk != NULL) {
		mutex_lock(&infotm_nftl_blk->nftl_lock);
		infotm_nftl_dbg("infotm_nftl_resume: %d\n", infotm_nftl_blk->nftl_pm_state);
		/*if (infotm_nftl_blk->nftl_pm_state == 1) {
			infotm_nftl_reinitialize(infotm_nftl_blk, (infotm_nftl_parts_size[0] / (mtd->erasesize >> 20)), (infotm_nftl_parts_size[1] / (mtd->erasesize >> 20)));
			infotm_nftl_blk->nftl_pm_state = 2;
		}*/
		//if (infotm_nftl_blk->total_cache_buf_cnt > 0)
			//error = infotm_nftl_blk->write_cache_data(infotm_nftl_blk, CACHE_CLEAR_ALL);
		mutex_unlock(&infotm_nftl_blk->nftl_lock);	
	}

	return;
}

static int nftl_pm_notify(struct notifier_block *nb, unsigned long event, void *arg)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk = container_of(nb, struct infotm_nftl_blk_t, nftl_pm_notifier);

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	if (event == PM_HIBERNATION_PREPARE) {
		infotm_nftl_blk->nftl_pm_state = 0;
	} else if (event == PM_POST_HIBERNATION) {
		infotm_nftl_dbg("nftl_pm_notify: %d\n", infotm_nftl_blk->total_cache_buf_cnt);
		//infotm_nftl_reinitialize(infotm_nftl_blk);
		infotm_nftl_blk->nftl_pm_state = 0;
	}
	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return NOTIFY_OK;
}

static void infotm_nftl_erase_data(struct mtd_blktrans_dev *dev, unsigned int cmd, unsigned long arg)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(dev);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
    struct mtd_info *mtd = infotm_nftl_blk->mtd;
    void __user *argp = (void __user *)arg;
    struct erase_info *instr;
    loff_t from;
    size_t len;
    int blk_addr, i, blk_cnt, erase_shift, part_blk_offset;

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	erase_shift = ffs(mtd->erasesize) - 1;
	part_blk_offset = (int)(infotm_nftl_part->offset >> erase_shift);

	instr = infotm_nftl_malloc(sizeof(struct erase_info));
	if (!instr)
		goto exit;

	if (cmd == MEMERASE64) {
		struct erase_info_user64 einfo64;

		if (copy_from_user(&einfo64, argp, sizeof(struct erase_info_user64))) {
			infotm_nftl_free(instr);
			goto exit;
		}
		instr->addr = einfo64.start;
		instr->len = einfo64.length;
	} else {
		struct erase_info_user einfo32;

		if (copy_from_user(&einfo32, argp, sizeof(struct erase_info_user))) {
			infotm_nftl_free(instr);
			goto exit;
		}
		instr->addr = einfo32.start;
		instr->len = einfo32.length;
	}

    from = instr->addr;
    len = instr->len;
    if (from & (mtd->erasesize - 1)) {
        infotm_nftl_dbg("nftl erase addr not aligned %llx erasesize %x ", (uint64_t)from, mtd->erasesize);
        goto exit;
    }
    if (len & (mtd->erasesize - 1)) {
        infotm_nftl_dbg("nftl erase length not aligned %llx erasesize %x ", (uint64_t)len, mtd->erasesize);
        len = (len + (mtd->erasesize - 1)) / (mtd->erasesize - 1);
    }

    blk_addr = (int)(from >> erase_shift);
    blk_cnt = (int)(len >> erase_shift);

    for (i=0; i<blk_cnt; i++)
        infotm_nftl_info->erase_logic_block(infotm_nftl_info, blk_addr + i + part_blk_offset);

exit:
	mutex_unlock(&infotm_nftl_blk->nftl_lock);
	return;
}

static void infotm_nftl_creat_part(struct infotm_nftl_blk_t *infotm_nftl_blk, struct mtd_blktrans_ops *tr)
{
    struct infotm_nftl_part_t *infotm_nftl_part = NULL;
	int i, part_size = 0, part_num = 0;
	uint64_t offset = 0;

	for(i = 0; i < ARRAY_SIZE(infotm_nftl_parts); i++) {
		if (item_exist(infotm_nftl_parts[i]) && (i < (ARRAY_SIZE(infotm_nftl_parts)-1)))
            part_size = item_integer(infotm_nftl_parts[i], 0);
        else
        	part_size = infotm_nftl_parts_size[i];
        if (part_size == PART_SIZE_FULL)
            part_size = (int)((infotm_nftl_blk->actual_size - offset) >> 20);

        if (part_size > 0) {
            infotm_nftl_part = infotm_nftl_malloc(sizeof(struct infotm_nftl_part_t));  
            if (!infotm_nftl_part)
                continue;

            if (!(infotm_nftl_blk->mtd->flags & MTD_WRITEABLE))
            	infotm_nftl_part->mbd.readonly = 0;

            infotm_nftl_part->size = SZ_1M;
            infotm_nftl_part->size *= part_size;
            infotm_nftl_part->offset = offset;
            infotm_nftl_part->mbd.mtd = infotm_nftl_blk->mtd;
            infotm_nftl_part->mbd.devnum = part_num++;
            infotm_nftl_part->mbd.tr = tr;
            infotm_nftl_part->mbd.size = (infotm_nftl_part->size >> 9);
            infotm_nftl_part->priv = infotm_nftl_blk;
            infotm_nftl_part->cache_buf_cnt = 0;
            strcpy(infotm_nftl_part->part_name, infotm_nftl_parts[i]+5);
            offset += infotm_nftl_part->size;
            if (i == (ARRAY_SIZE(infotm_nftl_parts)-1))
                tr->part_bits = 1;
            else
                tr->part_bits = 0;
            if (i == 0) {
            	infotm_nftl_part->read_data = infotm_nftl_boot_part_read_data;
            	infotm_nftl_part->write_data = infotm_nftl_boot_part_write_data;
            } else {
            	infotm_nftl_part->read_data = infotm_nftl_normal_part_read_data;
            	infotm_nftl_part->write_data = infotm_nftl_normal_part_write_data;
            }
			infotm_nftl_part_save[i] = infotm_nftl_part;
            printk(KERN_INFO "nftl add disk dev %s %llx %llx \n", infotm_nftl_part->part_name, infotm_nftl_part->offset, infotm_nftl_part->size);

            if (add_mtd_blktrans_dev(&infotm_nftl_part->mbd))
            	printk(KERN_ALERT "nftl add blk disk dev failed %s \n", infotm_nftl_part->part_name);
		}
	}

    return;
}

/*static struct device_driver infotm_nftl_driver = {
    .shutdown = infotm_nftl_shutdown,
    .owner	= THIS_MODULE,
};*/

static void infotm_nftl_add_mtd(struct mtd_blktrans_ops *tr, struct mtd_info *mtd)
{
	struct infotm_nftl_blk_t *infotm_nftl_blk;

	if (mtd->type != MTD_NANDFLASH)
		return;

	if (strncmp(mtd->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME)))
		return;

	infotm_nftl_blk = infotm_nftl_malloc(sizeof(struct infotm_nftl_blk_t));
	if (!infotm_nftl_blk)
		return;

    infotm_nftl_blk->cache_sect_addr = -1;
    infotm_nftl_blk->uboot_write_blk = -1;
    infotm_nftl_blk->write_shift = fls(mtd->writesize) - 1;
    infotm_nftl_blk->erase_shift = fls(mtd->erasesize) - 1;
    infotm_nftl_blk->pages_shift = infotm_nftl_blk->erase_shift - infotm_nftl_blk->write_shift;
	infotm_nftl_blk->mtd = mtd;
	infotm_nftl_blk->nb.notifier_call = infotm_nftl_reboot_notifier;
	infotm_nftl_blk->nftl_pm_notifier.notifier_call = nftl_pm_notify;
	mtd->uplayer_priv = infotm_nftl_blk;
	mtd->_suspend_last = infotm_nftl_suspend;
	mtd->_resume_last = infotm_nftl_resume;
	mtd->_shutdown = infotm_nftl_shutdown;
	/*if (mtd->dev.driver)
	    mtd->dev.driver->shutdown = infotm_nftl_shutdown;
	else
	    mtd->dev.driver = &infotm_nftl_driver;*/

	mutex_init(&infotm_nftl_blk->nftl_lock);
	init_timer(&infotm_nftl_blk->cache_timer);
	register_reboot_notifier(&infotm_nftl_blk->nb);
	register_pm_notifier(&infotm_nftl_blk->nftl_pm_notifier);
	spin_lock_init(&infotm_nftl_blk->thread_lock);
	INIT_LIST_HEAD(&infotm_nftl_blk->cache_list);
	INIT_LIST_HEAD(&infotm_nftl_blk->free_list);
	infotm_nftl_blk->read_data = infotm_nftl_read_data;
	infotm_nftl_blk->write_data = infotm_nftl_write_data;
	infotm_nftl_blk->write_cache_data = infotm_nftl_write_cache_data;
	infotm_nftl_blk->search_cache_list = infotm_nftl_search_cache_list;
	infotm_nftl_blk->add_cache_list = infotm_nftl_add_cache_list;

    infotm_nftl_blk->cache_buf = infotm_nftl_malloc(mtd->writesize);
    if (!infotm_nftl_blk->cache_buf)
    	return;

	if (infotm_nftl_initialize(infotm_nftl_blk))
		return;

	infotm_nftl_blk->nftl_thread = kthread_run(infotm_nftl_thread, infotm_nftl_blk, "%sd", "infotm_nftl");
	if (IS_ERR(infotm_nftl_blk->nftl_thread))
		return;

    infotm_nftl_creat_part(infotm_nftl_blk, tr);
	return;
}

static int infotm_nftl_open(struct mtd_blktrans_dev *mbd)
{
	return 0;
}

static int infotm_nftl_release(struct mtd_blktrans_dev *mbd)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(mbd);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	int error = 0;

	mutex_lock(&infotm_nftl_blk->nftl_lock);
	//infotm_nftl_dbg("nftl release flush cache data: %s %d\n", infotm_nftl_part->part_name, infotm_nftl_part->cache_buf_cnt);
	if ((infotm_nftl_part->cache_buf_cnt > 0) && (!infotm_nftl_info->shutdown_flag))
		error = infotm_nftl_blk->write_cache_data(infotm_nftl_blk, CACHE_CLEAR_ALL);
	mutex_unlock(&infotm_nftl_blk->nftl_lock);

	return error;
}

static void infotm_nftl_blk_release(struct infotm_nftl_blk_t *infotm_nftl_blk)
{
	struct infotm_nftl_info_t *infotm_nftl_info = infotm_nftl_blk->infotm_nftl_info;
	infotm_nftl_info_release(infotm_nftl_info);
	infotm_nftl_free(infotm_nftl_info);
	if (infotm_nftl_blk->cache_buf)
		infotm_nftl_free(infotm_nftl_blk->cache_buf);
}

static void infotm_nftl_remove_dev(struct mtd_blktrans_dev *dev)
{
	struct infotm_nftl_part_t *infotm_nftl_part = mbd_to_nftl_part(dev);
	struct infotm_nftl_blk_t *infotm_nftl_blk = (struct infotm_nftl_blk_t *)infotm_nftl_part->priv;

	unregister_reboot_notifier(&infotm_nftl_blk->nb);
	del_mtd_blktrans_dev(dev);
	infotm_nftl_free(dev);
	infotm_nftl_blk_release(infotm_nftl_blk);
	infotm_nftl_free(infotm_nftl_blk);
	if (infotm_nftl_part->bounce_buf)
		infotm_nftl_free(infotm_nftl_part->bounce_buf);
	infotm_nftl_free(infotm_nftl_part);	
}

static struct mtd_blktrans_ops infotm_nftl_tr = {
	.name		= "nandblk",
	.major		= INFOTM_NFTL_MAJOR,
	.part_bits	= 1,
	.blksize 	= 512,
	.open		= infotm_nftl_open,
	.release	= infotm_nftl_release,
	.init_bounce_buf = infotm_nftl_init_bounce_buf,
	//.update_blktrans_sysinfo = infotm_nftl_update_blktrans_sysinfo,
	.erase_data = infotm_nftl_erase_data,
	.do_blktrans_request = do_nftltrans_request,
	.writesect	= infotm_nftl_writesect,
	.discard 	= infotm_nftl_discard,
	.flush		= infotm_nftl_flush,
	.add_mtd	= infotm_nftl_add_mtd,
	.remove_dev	= infotm_nftl_remove_dev,
	.owner		= THIS_MODULE,
};

static int __init init_infotm_nftl(void)
{
	printk(KERN_ALERT "[imapx_nftl_x9] INFOTMIC NFTL V8.1 & 2014.8.15 env\n");
	return register_mtd_blktrans(&infotm_nftl_tr);
}

static void __exit cleanup_infotm_nftl(void)
{
	deregister_mtd_blktrans(&infotm_nftl_tr);
}

module_init(init_infotm_nftl);
module_exit(cleanup_infotm_nftl);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("infotm xiaojun_yoyo");
MODULE_DESCRIPTION("infotm nftl block interface");
