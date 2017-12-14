#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/blktrans.h>
#include <linux/mutex.h>
#include <mach/items.h>
#include "spi_blk.h"

#define mbd_to_spi_part(l)	    container_of(l, struct spi_part_t, mbd)
#define env_to_spi_block(l)	    container_of(l, struct spi_blk_t, spi_env)

static void spi_erase_sect_map(struct spi_info_t * spi_info, addr_blk_t blk_addr)
{
	struct phyblk_node_t *phy_blk_node;
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_prev;
	phy_blk_node = &spi_info->phypmt[blk_addr];

	if ((phy_blk_node->vtblk >= 0) && (phy_blk_node->vtblk < spi_info->accessibleblocks)) {
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + phy_blk_node->vtblk));
		while (vt_blk_node != NULL) {
			vt_blk_node_prev = vt_blk_node;
			vt_blk_node = vt_blk_node->next;

			if ((vt_blk_node != NULL) && (vt_blk_node->phy_blk_addr == blk_addr)) {
				spi_dbg("free blk had mapped vt blk: %d phy blk: %d\n", phy_blk_node->vtblk, blk_addr);
				vt_blk_node_prev->next = vt_blk_node->next;
				spi_free(vt_blk_node);
			} else if ((vt_blk_node_prev != NULL) && (vt_blk_node_prev->phy_blk_addr == blk_addr)) {
				spi_dbg("free blk had mapped vt blk: %d phy blk: %d\n", phy_blk_node->vtblk, blk_addr);
				if (*(spi_info->vtpmt + phy_blk_node->vtblk) == vt_blk_node_prev)
					*(spi_info->vtpmt + phy_blk_node->vtblk) = vt_blk_node;

				spi_free(vt_blk_node_prev);
			}
		}
	}

	phy_blk_node->ec++;
	phy_blk_node->valid_sects = 0;
	phy_blk_node->vtblk = BLOCK_INIT_VALUE;
	phy_blk_node->last_write = 0;
	phy_blk_node->timestamp = MAX_TIMESTAMP_NUM;
	memset(phy_blk_node->phy_page_map, 0xff, (sizeof(uint8_t)*MAX_SECTS_IN_BLOCK));
	memset(phy_blk_node->phy_page_delete, 0xff, (MAX_SECTS_IN_BLOCK>>3));

	return;
}

static int spi_erase_block(struct spi_info_t * spi_info, addr_blk_t blk_addr)
{
	struct mtd_info *mtd = spi_info->mtd;
	struct phyblk_node_t *phy_blk_node = &spi_info->phypmt[blk_addr];
	struct erase_info spi_erase_info;
	unsigned char head_buf[HEAD_LENGTH + sizeof(erase_count_t)];
	struct sect_map_head_t *sect_map_head;

	memset(&spi_erase_info, 0, sizeof(struct erase_info));
	spi_erase_info.mtd = mtd;
	spi_erase_info.addr = mtd->erasesize;
	spi_erase_info.addr *= blk_addr;
	spi_erase_info.len = mtd->erasesize;

	mtd->_erase(mtd, &spi_erase_info);

	spi_erase_sect_map(spi_info, blk_addr);

	memcpy(head_buf, SPI_SECT_MAP_HEAD, strlen(SPI_SECT_MAP_HEAD));
	sect_map_head = (struct sect_map_head_t *)head_buf;
	sect_map_head->ec = phy_blk_node->ec;
	spi_info->write_head(spi_info, blk_addr, 0, 0, head_buf, HEAD_LENGTH + sizeof(erase_count_t));
	return 0;
}

static void spi_update_sectmap(struct spi_info_t *spi_info, addr_blk_t phy_blk_addr, addr_page_t logic_page_addr, addr_page_t phy_page_addr)
{
	struct phyblk_node_t *phy_blk_node;
	phy_blk_node = &spi_info->phypmt[phy_blk_addr];

	phy_blk_node->valid_sects++;
	phy_blk_node->phy_page_map[logic_page_addr] = phy_page_addr;
	phy_blk_node->phy_page_delete[logic_page_addr>>3] |= (1 << (logic_page_addr % 8));
	phy_blk_node->last_write = phy_page_addr;
}

static int spi_write_head(struct spi_info_t * spi_info, addr_blk_t blk_addr, addr_page_t page_addr,
								int page_offset, unsigned char *head_buf, int head_len)
{
	struct mtd_info *mtd = spi_info->mtd;
	loff_t from;
	size_t len, retlen;

	from = mtd->erasesize;
	from *= blk_addr;
	from += page_addr * spi_info->writesize;
	from += page_offset;
	len = head_len;

	mtd->_write(mtd, from, len, &retlen, head_buf);

	return 0;
}

static int spi_write_page(struct spi_info_t * spi_info, addr_blk_t blk_addr, addr_page_t page_addr,
								addr_page_t logic_page_addr, unsigned char *data_buf)
{
	struct mtd_info *mtd = spi_info->mtd;
	loff_t from;
	size_t len, retlen;

	from = mtd->erasesize;
	from *= blk_addr;
	from += page_addr * spi_info->writesize;
	len = spi_info->writesize;

	mtd->_write(mtd, from, len, &retlen, data_buf);

	spi_update_sectmap(spi_info, blk_addr, logic_page_addr, page_addr);
	return 0;
}

static int spi_read_page(struct spi_info_t * spi_info, addr_blk_t blk_addr, addr_page_t page_addr,
								int read_len, unsigned char *data_buf)
{
	struct mtd_info *mtd = spi_info->mtd;
	loff_t from;
	size_t len, retlen;

	from = mtd->erasesize;
	from *= blk_addr;
	from += page_addr * spi_info->writesize;
	len = read_len;

	return mtd->_read(mtd, from, len, &retlen, data_buf);
}

static int spi_copy_page(struct spi_info_t * spi_info, addr_blk_t dest_blk_addr, addr_page_t dest_page, 
				addr_blk_t src_blk_addr, addr_page_t src_page, addr_page_t logic_page_addr)
{
	unsigned char *spi_data_buf;
	uint8_t head_buf[8];
	int page_offset;

	spi_data_buf = spi_info->copy_page_buf;
	spi_info->read_page(spi_info, src_blk_addr, src_page, spi_info->writesize, spi_data_buf);

	*head_buf = logic_page_addr;
	page_offset = HEAD_LENGTH + sizeof(erase_count_t) + sizeof(addr_vtblk_t) + sizeof(int16_t) + dest_page * sizeof(uint8_t);
	spi_info->write_head(spi_info, dest_blk_addr, 0, page_offset, head_buf, 1);
	spi_info->write_page(spi_info, dest_blk_addr, dest_page, logic_page_addr, spi_data_buf);

	return 0;
}

static int spi_creat_sectmap(struct spi_info_t *spi_info, addr_blk_t phy_blk_addr)
{
	uint32_t page_per_blk;
	int16_t valid_sects = 0, i;
	struct phyblk_node_t *phy_blk_node;
	struct sect_map_head_t *sect_map_head;

	phy_blk_node = &spi_info->phypmt[phy_blk_addr];
	spi_info->read_page(spi_info, phy_blk_addr, 0, spi_info->writesize, spi_info->copy_page_buf);
	page_per_blk = spi_info->pages_per_blk;
	sect_map_head = (struct sect_map_head_t *)spi_info->copy_page_buf;
	if (!strncmp(sect_map_head->head, SPI_SECT_MAP_HEAD, strlen((const char*)SPI_SECT_MAP_HEAD))) {
		for (i=1; i<page_per_blk; i++) {

			if ((sect_map_head->phy_page_map[i] >= 0) && (sect_map_head->phy_page_map[i] < page_per_blk)) {
				phy_blk_node->phy_page_map[sect_map_head->phy_page_map[i]] = i;
				phy_blk_node->last_write = i;
				valid_sects++;
			} else {
				break;
			}
		}
		phy_blk_node->valid_sects = valid_sects;
	} else {
		phy_blk_node->last_write = 0;
		phy_blk_node->valid_sects = 0;
	}

	return 0;
}

static int spi_get_phy_sect_map(struct spi_info_t * spi_info, addr_blk_t blk_addr)
{
	int status;
	struct phyblk_node_t *phy_blk_node;
	phy_blk_node = &spi_info->phypmt[blk_addr];

	if (phy_blk_node->valid_sects < 0) {
		status = spi_creat_sectmap(spi_info, blk_addr);
		if (status)
			return SPI_FAILURE;
	}

	return 0;	
}

static int spi_get_valid_pos(struct spi_info_t *spi_info, addr_blk_t logic_blk_addr, addr_blk_t *phy_blk_addr,
									 addr_page_t logic_page_addr, addr_page_t *phy_page_addr, uint8_t flag )
{
	struct phyblk_node_t *phy_blk_node;
	struct vtblk_node_t  *vt_blk_node;
	uint32_t page_per_blk;

	page_per_blk = spi_info->pages_per_blk;
	*phy_blk_addr = BLOCK_INIT_VALUE;
	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + logic_blk_addr));
	if (vt_blk_node == NULL)
		return SPI_BLKNOTFOUND;

	*phy_blk_addr = vt_blk_node->phy_blk_addr;
	phy_blk_node = &spi_info->phypmt[*phy_blk_addr];

	if (flag == WRITE_OPERATION) {
		*phy_page_addr = phy_blk_node->last_write + 1;
		if (*phy_page_addr >= page_per_blk)
			return SPI_PAGENOTFOUND;

		return 0;
	} else if (flag == READ_OPERATION) {
		do {

			*phy_blk_addr = vt_blk_node->phy_blk_addr;
			phy_blk_node = &spi_info->phypmt[*phy_blk_addr];

			if (phy_blk_node->phy_page_map[logic_page_addr] > 0) {
				*phy_page_addr = phy_blk_node->phy_page_map[logic_page_addr];
				return 0;
			}

			vt_blk_node = vt_blk_node->next;
		} while (vt_blk_node != NULL);

		return SPI_PAGENOTFOUND;
	}

	return SPI_FAILURE;
}

int spi_add_node(struct spi_info_t *spi_info, addr_blk_t logic_blk_addr, addr_blk_t phy_blk_addr)
{
	struct phyblk_node_t *phy_blk_node_curt = NULL, *phy_blk_node_add, *phy_blk_node_temp;
	struct vtblk_node_t  *vt_blk_node_curt, *vt_blk_node_add;
	addr_blk_t phy_blk_cur, phy_blk_num;
	int status = 0;

	vt_blk_node_add = (struct vtblk_node_t *)spi_malloc(sizeof(struct vtblk_node_t));
	if (vt_blk_node_add == NULL)
		return SPI_FAILURE;

	vt_blk_node_add->phy_blk_addr = phy_blk_addr;
	vt_blk_node_add->next = NULL;
	phy_blk_node_add = &spi_info->phypmt[phy_blk_addr];
	phy_blk_node_add->vtblk = logic_blk_addr;
	vt_blk_node_curt = (struct vtblk_node_t *)(*(spi_info->vtpmt + logic_blk_addr));
	phy_blk_cur = BLOCK_INIT_VALUE;

	for (phy_blk_num=0; phy_blk_num<spi_info->endblock; phy_blk_num++) {
		if ((phy_blk_num == phy_blk_addr) || ((vt_blk_node_curt != NULL) && (phy_blk_num == vt_blk_node_curt->phy_blk_addr)))
			continue;
		phy_blk_node_temp = &spi_info->phypmt[phy_blk_num];
		if (phy_blk_node_temp->vtblk == logic_blk_addr) {
			if (phy_blk_cur >= 0) {
				phy_blk_node_curt = &spi_info->phypmt[phy_blk_cur];
				if (phy_blk_node_temp->timestamp > phy_blk_node_curt->timestamp)
					phy_blk_cur = phy_blk_num;
			} else {
				phy_blk_cur = phy_blk_num;
			}
		}
	}

	if (vt_blk_node_curt == NULL) {
		if (phy_blk_cur >= 0) {
			phy_blk_node_curt = &spi_info->phypmt[phy_blk_cur];
			if (phy_blk_node_curt->timestamp >= MAX_TIMESTAMP_NUM)
				phy_blk_node_add->timestamp = 0;
			else
				phy_blk_node_add->timestamp = phy_blk_node_curt->timestamp + 1;
		} else {
			phy_blk_node_add->timestamp = 0;
		}
	} else {
		if (phy_blk_cur >= 0) {
			phy_blk_node_temp = &spi_info->phypmt[phy_blk_cur];
			phy_blk_node_curt = &spi_info->phypmt[vt_blk_node_curt->phy_blk_addr];
			if (phy_blk_node_temp->timestamp > phy_blk_node_curt->timestamp)
				phy_blk_node_curt = phy_blk_node_temp;
		} else {
			phy_blk_cur = vt_blk_node_curt->phy_blk_addr;
			phy_blk_node_curt = &spi_info->phypmt[phy_blk_cur];
		}
		if (phy_blk_node_curt->timestamp >= MAX_TIMESTAMP_NUM)
			phy_blk_node_add->timestamp = 0;
		else
			phy_blk_node_add->timestamp = phy_blk_node_curt->timestamp + 1;
		vt_blk_node_add->next = vt_blk_node_curt;
	}
	*(spi_info->vtpmt + logic_blk_addr) = vt_blk_node_add;

	return status;
}

static void spi_add_block(struct spi_info_t *spi_info, addr_blk_t phy_blk, struct sect_map_head_t *sect_map_head)
{
	struct phyblk_node_t *phy_blk_node_curt, *phy_blk_node_add;
	struct vtblk_node_t  *vt_blk_node_curt, *vt_blk_node_prev, *vt_blk_node_add;
	int valid_sects = 0, i;

	vt_blk_node_add = (struct vtblk_node_t *)spi_malloc(sizeof(struct vtblk_node_t));
	if (vt_blk_node_add == NULL)
		return;
	vt_blk_node_add->phy_blk_addr = phy_blk;
	vt_blk_node_add->next = NULL;
	phy_blk_node_add = &spi_info->phypmt[phy_blk];
	phy_blk_node_add->ec = sect_map_head->ec;
	phy_blk_node_add->vtblk = sect_map_head->vtblk;
	phy_blk_node_add->timestamp = sect_map_head->timestamp;
	vt_blk_node_curt = (struct vtblk_node_t *)(*(spi_info->vtpmt + sect_map_head->vtblk));

	for (i=1; i<spi_info->pages_per_blk; i++) {

		if ((sect_map_head->phy_page_map[i] >= 0) && (sect_map_head->phy_page_map[i] < spi_info->pages_per_blk)) {
			phy_blk_node_add->phy_page_map[sect_map_head->phy_page_map[i]] = i;
			phy_blk_node_add->last_write = i;
			valid_sects++;
		} else {
			break;
		}
	}
	phy_blk_node_add->valid_sects = valid_sects;

	if (vt_blk_node_curt == NULL) {
		vt_blk_node_curt = vt_blk_node_add;
		*(spi_info->vtpmt + sect_map_head->vtblk) = vt_blk_node_curt;
		return;
	}

	vt_blk_node_prev = vt_blk_node_curt;
	while(vt_blk_node_curt != NULL) {

		phy_blk_node_curt = &spi_info->phypmt[vt_blk_node_curt->phy_blk_addr];
		if (((phy_blk_node_add->timestamp > phy_blk_node_curt->timestamp)
			 && ((phy_blk_node_add->timestamp - phy_blk_node_curt->timestamp) < (MAX_TIMESTAMP_NUM - spi_info->accessibleblocks)))
			|| ((phy_blk_node_add->timestamp < phy_blk_node_curt->timestamp)
			 && ((phy_blk_node_curt->timestamp - phy_blk_node_add->timestamp) >= (MAX_TIMESTAMP_NUM - spi_info->accessibleblocks)))) {

			vt_blk_node_add->next = vt_blk_node_curt;
			if (*(spi_info->vtpmt + sect_map_head->vtblk) == vt_blk_node_curt)
				*(spi_info->vtpmt + sect_map_head->vtblk) = vt_blk_node_add;
			else
				vt_blk_node_prev->next = vt_blk_node_add;
			break;
		} else if (phy_blk_node_add->timestamp == phy_blk_node_curt->timestamp) {
			spi_dbg("spi timestamp err logic blk: %d phy blk: %d %d %d\n", sect_map_head->vtblk, vt_blk_node_curt->phy_blk_addr, vt_blk_node_add->phy_blk_addr, phy_blk_node_add->timestamp);
			if (phy_blk_node_add->ec < phy_blk_node_curt->ec) {

				if (vt_blk_node_prev == vt_blk_node_curt) {
					vt_blk_node_add->next = vt_blk_node_curt;
					*(spi_info->vtpmt + sect_map_head->vtblk) = vt_blk_node_add;
				} else {
					vt_blk_node_prev->next = vt_blk_node_add;
					vt_blk_node_add->next = vt_blk_node_curt;
				}
				break;
			}
			if (vt_blk_node_curt == (struct vtblk_node_t *)(*(spi_info->vtpmt + sect_map_head->vtblk))) {
				vt_blk_node_add->next = vt_blk_node_curt;
				*(spi_info->vtpmt + sect_map_head->vtblk) = vt_blk_node_add;
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

int spi_check_node(struct spi_info_t *spi_info, addr_blk_t blk_addr)
{
	struct spi_wl_t *spi_wl;
	struct phyblk_node_t *phy_blk_node;
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_prev, *vt_blk_node_free;
	addr_blk_t phy_blk_num, vt_blk_num;
	int16_t *valid_page;
	int k, node_length, node_len_actual, status = 0;

	spi_wl = spi_info->spi_wl;
	vt_blk_num = blk_addr;
	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
	if (vt_blk_node == NULL)
		return -ENOMEM;
    node_len_actual = spi_get_node_length(spi_info, vt_blk_node);
    valid_page = spi_malloc(sizeof(int16_t)*node_len_actual);
    if (valid_page == NULL)
        return -ENOMEM;

	memset((unsigned char *)valid_page, 0x0, sizeof(int16_t)*node_len_actual);
	for (k=0; k<(spi_info->pages_per_blk-1); k++) {

		node_length = 0;
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
		while (vt_blk_node != NULL) {

			phy_blk_num = vt_blk_node->phy_blk_addr;
			phy_blk_node = &spi_info->phypmt[phy_blk_num];
			if (!(phy_blk_node->phy_page_delete[k>>3] & (1 << (k % 8))))
				break;
			spi_info->get_phy_sect_map(spi_info, phy_blk_num);
			if ((phy_blk_node->phy_page_map[k] > 0) && (phy_blk_node->phy_page_delete[k>>3] & (1 << (k % 8)))) {
				valid_page[node_length]++;
				break;
			}
			node_length++;
			vt_blk_node = vt_blk_node->next;
		}
	}

	node_length = 0;
	vt_blk_node_prev = NULL;
	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
	while (vt_blk_node != NULL) {

		if (valid_page[node_length] == 0) {
			spi_wl->add_free(spi_wl, vt_blk_node->phy_blk_addr);
			if (vt_blk_node == *(spi_info->vtpmt + vt_blk_num))
				*(spi_info->vtpmt + vt_blk_num) = vt_blk_node->next;
			else if (vt_blk_node_prev != NULL)
				vt_blk_node_prev->next = vt_blk_node->next;

			vt_blk_node_free = vt_blk_node;
			vt_blk_node = vt_blk_node->next;
			node_length++;
			spi_free(vt_blk_node_free);
			continue;
		}

		vt_blk_node_prev = vt_blk_node;
		vt_blk_node = vt_blk_node->next;
		node_length++;
	}

	spi_free(valid_page);

	return status;
}

static void spi_check_conflict_node(struct spi_info_t *spi_info)
{
	struct vtblk_node_t  *vt_blk_node;
	struct spi_wl_t *spi_wl;
	addr_blk_t vt_blk_num;
	int node_length = 0, status = 0;

	spi_wl = spi_info->spi_wl;
	for (vt_blk_num=0; vt_blk_num<spi_info->accessibleblocks; vt_blk_num++) {

		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
		if (vt_blk_node == NULL)
			continue;

		node_length = spi_get_node_length(spi_info, vt_blk_node);
		if (node_length < BASIC_BLK_NUM_PER_NODE)
			continue;

		status = spi_check_node(spi_info, vt_blk_num);
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
		if (spi_get_node_length(spi_info, vt_blk_node) >= BASIC_BLK_NUM_PER_NODE)
			spi_wl->add_gc(spi_wl, vt_blk_num);
	}

	return;
}

static void spi_delete_sect(struct spi_info_t *spi_info, addr_page_t sect_addr)
{
	struct vtblk_node_t  *vt_blk_node;
	struct phyblk_node_t *phy_blk_node;
	uint32_t page_per_blk;
	addr_page_t logic_page_addr, del_sect_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;

	page_per_blk = spi_info->pages_per_blk;
	del_sect_addr = sect_addr - spi_info->startblock * page_per_blk;
	logic_page_addr = del_sect_addr % (page_per_blk - 1);
	logic_blk_addr = del_sect_addr / (page_per_blk - 1);
	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + logic_blk_addr));
	if (vt_blk_node == NULL)
		return;

	while (vt_blk_node != NULL) {

		phy_blk_addr = vt_blk_node->phy_blk_addr;
		phy_blk_node = &spi_info->phypmt[phy_blk_addr];
		phy_blk_node->phy_page_delete[logic_page_addr>>3] &= (~(1 << (logic_page_addr % 8)));
		vt_blk_node = vt_blk_node->next;
	}
	return;
}

static int spi_read_sect(struct spi_info_t *spi_info, addr_page_t sect_addr, unsigned long nsect, unsigned char *buf)
{
	uint32_t page_per_blk;
	addr_page_t logic_page_addr, phy_page_addr = -1, phy_page_addr_prev = -1;
	addr_blk_t logic_blk_addr, phy_blk_addr = -1, phy_blk_addr_prev = -1;
	int i, read_sect = 0, read_sect_addr, status = 0, offset = 0;

	page_per_blk = spi_info->pages_per_blk;
	read_sect_addr = sect_addr - spi_info->startblock * page_per_blk;

	logic_blk_addr = (read_sect_addr) / (page_per_blk - 1);

	for (i=0; i<nsect; i++) {
		logic_page_addr = (read_sect_addr + i) % (page_per_blk - 1);
		logic_blk_addr = (read_sect_addr + i) / (page_per_blk - 1);

		status = spi_get_valid_pos(spi_info, logic_blk_addr, &phy_blk_addr, logic_page_addr, &phy_page_addr, READ_OPERATION);
		if ((status == SPI_PAGENOTFOUND) || (status == SPI_BLKNOTFOUND)) {
			if (read_sect) {
				spi_info->read_page(spi_info, phy_blk_addr_prev, phy_page_addr_prev, read_sect * spi_info->writesize, buf + offset);
				read_sect = 0;
			}
			memset(buf + i * spi_info->writesize, 0xff, spi_info->writesize);
			phy_page_addr_prev = -1;
			phy_blk_addr_prev = -1;
			offset = (i + 1) * spi_info->writesize;
			continue;
		}

		if ((phy_blk_addr == phy_blk_addr_prev) && (phy_page_addr == (phy_page_addr_prev + read_sect))) {
			read_sect++;
			continue;
		} else if (read_sect > 0) {
			spi_info->read_page(spi_info, phy_blk_addr_prev, phy_page_addr_prev, read_sect * spi_info->writesize, buf + offset);
			read_sect = 0;
		}
		phy_page_addr_prev = phy_page_addr;
		phy_blk_addr_prev = phy_blk_addr;
		offset = i * spi_info->writesize;
		read_sect++;
	}
	if (read_sect > 0)
		spi_info->read_page(spi_info, phy_blk_addr_prev, phy_page_addr_prev, read_sect * spi_info->writesize, buf + offset);

	return 0;
}

static int spi_write_sect(struct spi_info_t *spi_info, addr_page_t sect_addr, unsigned char *buf)
{
	struct vtblk_node_t  *vt_blk_node;
	struct phyblk_node_t *phy_blk_node;
	struct spi_wl_t *spi_wl = spi_info->spi_wl;
	int status = 0, page_offset, head_len, write_sect_addr;
	uint32_t page_per_blk;
	addr_page_t logic_page_addr, phy_page_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;
	struct sect_map_head_t *sect_map_head;
	uint8_t *head_buf;

	page_per_blk = spi_info->pages_per_blk;
	write_sect_addr = sect_addr - spi_info->startblock * page_per_blk;
	logic_page_addr = write_sect_addr % (page_per_blk - 1);
	logic_blk_addr = write_sect_addr / (page_per_blk - 1);

	status = spi_get_valid_pos(spi_info, logic_blk_addr, &phy_blk_addr, logic_page_addr, &phy_page_addr, WRITE_OPERATION);
	if ((status == SPI_PAGENOTFOUND) || (status == SPI_BLKNOTFOUND)) {

		if ((spi_wl->free_cnt < spi_info->fillfactor) && (spi_wl->wait_gc_block < 0))
			spi_wl->garbage_collect(spi_wl, 0);

		status = spi_wl->get_best_free(spi_wl, &phy_blk_addr);
		if (status) {
			status = spi_wl->garbage_collect(spi_wl, DO_COPY_PAGE);
			if (status == 0) {
				spi_dbg("spi couldn`t found free block: %d %d\n", spi_wl->free_cnt, spi_wl->wait_gc_block);
				return -ENOENT;
			}
			status = spi_wl->get_best_free(spi_wl, &phy_blk_addr);
			if (status)
				return status;
		}

		spi_add_node(spi_info, logic_blk_addr, phy_blk_addr);
		//spi_wl->add_used(spi_wl, phy_blk_addr);

		phy_blk_node = &spi_info->phypmt[phy_blk_addr];
		head_buf = spi_info->copy_page_buf;
		memcpy(head_buf, SPI_SECT_MAP_HEAD, strlen(SPI_SECT_MAP_HEAD));
		sect_map_head = (struct sect_map_head_t *)head_buf;
		sect_map_head->ec = phy_blk_node->ec;
		sect_map_head->vtblk = logic_blk_addr;
		sect_map_head->timestamp = phy_blk_node->timestamp;
		head_len = sizeof(addr_vtblk_t) + sizeof(int16_t);
		spi_info->write_head(spi_info, phy_blk_addr, 0, HEAD_LENGTH + sizeof(erase_count_t), head_buf + HEAD_LENGTH + sizeof(erase_count_t), head_len);
		phy_page_addr = phy_blk_node->last_write + 1;
	}

	head_buf = spi_info->copy_page_buf;
	*head_buf = logic_page_addr;
	page_offset = HEAD_LENGTH + sizeof(erase_count_t) + sizeof(addr_vtblk_t) + sizeof(int16_t) + phy_page_addr * sizeof(uint8_t);
	spi_info->write_head(spi_info, phy_blk_addr, 0, page_offset, head_buf, 1);
	spi_info->write_page(spi_info, phy_blk_addr, phy_page_addr, logic_page_addr, buf);

	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + logic_blk_addr));
	if (phy_page_addr == (page_per_blk - 1)) {
		spi_check_node(spi_info, logic_blk_addr);
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + logic_blk_addr));
		if (spi_get_node_length(spi_info, vt_blk_node) >= BASIC_BLK_NUM_PER_NODE) {
			spi_wl->add_gc(spi_wl, logic_blk_addr);
		}
	}

	return 0;
}

static void spiblock_get_part_info(struct spi_blk_t *spi_blk)
{
	struct spi_info_t *spi_info = spi_blk->spi_info;
	struct mtd_info *mtd = spi_blk->mtd;
	struct spi_part_t *env_part = &spi_blk->env_part;
	unsigned char spi_parts[64];
	unsigned char part_type[16];
	int i = 0, part_size = 0, phys_erase_shift;

	phys_erase_shift = ffs(mtd->erasesize) - 1;
	while (i < 64) {
		sprintf(spi_parts, "part%d", i);
		if (item_exist(spi_parts)) {
			part_size = item_integer(spi_parts, 1);
			item_string(part_type, spi_parts, 2);
			if (!strncmp(part_type, "boot", strlen((const char*)"boot"))) {
				if (part_size == PART_SIZE_FULL) {
					spi_info->bootblock = spi_info->endblock;
					goto end;
				} else {
					spi_info->bootblock += part_size;
					if (i == 1) {
						env_part->offset = spi_info->bootblock;
						env_part->offset *= SZ_1K;
						env_part->offset += UPGRADE_PARA_SIZE;
						env_part->size = (RESERVED_SIZE - UPGRADE_PARA_SIZE);
						sprintf(env_part->part_name, "%s", "reserved");
					}
				}
			} else if (!strncmp(part_type, "normal", strlen((const char*)"normal"))) {
				if (part_size == PART_SIZE_FULL) {
					spi_info->normalblock = spi_info->endblock;
					goto end;
				} else {
					spi_info->normalblock += part_size;
				}
			}
		}
		i++;
	}

	spi_info->bootblock += (RESERVED_SIZE / SZ_1K);
	spi_info->bootblock *= SZ_1K;
	if (spi_info->bootblock & (mtd->erasesize - 1)) {
		spi_info->bootblock += (mtd->erasesize - 1);
		spi_info->bootblock &= ~(mtd->erasesize - 1);
	}
	spi_info->bootblock = (spi_info->bootblock >> phys_erase_shift);

	spi_info->normalblock *= SZ_1K;
	if (spi_info->normalblock & (mtd->erasesize - 1)) {
		spi_info->normalblock += (mtd->erasesize - 1);
		spi_info->normalblock &= ~(mtd->erasesize - 1);
	}
	spi_info->normalblock = (spi_info->normalblock >> phys_erase_shift);

end:
	spi_info->startblock = spi_info->bootblock + spi_info->normalblock;
	printk("spi get part %d %d \n", spi_info->bootblock, spi_info->normalblock);

	return;
}

static ssize_t spiblock_blk_map_table(struct class *class, struct class_attribute *attr,	const char *buf, size_t count)
{
    struct spi_info_t *spi_info = container_of(class, struct spi_info_t, cls);
    struct vtblk_node_t  *vt_blk_node;
    int vt_blk_num;

	for (vt_blk_num=0; vt_blk_num<spi_info->accessibleblocks; vt_blk_num++) {

		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
		if (vt_blk_node == NULL)
			continue;

		spi_dbg("spiblock logic blk %d map to", vt_blk_num);
		while (vt_blk_node != NULL) {

			spi_dbg(" %d ", vt_blk_node->phy_blk_addr);
			vt_blk_node = vt_blk_node->next;
		}
		spi_dbg("\n");
	}
	return count;
}

static ssize_t spiblock_sect_map_table(struct class *class, struct class_attribute *attr,	const char *buf, size_t count)
{
    struct spi_info_t *spi_info = container_of(class, struct spi_info_t, cls);
    struct sect_map_head_t *sect_map_head;
    struct vtblk_node_t  *vt_blk_node;
    struct phyblk_node_t *phy_blk_node;
	unsigned int address;
    unsigned blks_per_sect, sect_addr;
	addr_blk_t logic_blk_addr, phy_blk_addr;
    uint32_t page_per_blk;
    int ret, i;
    unsigned char spi_data_buf[spi_info->writesize];

	ret =  sscanf(buf, "%x", &address);
    blks_per_sect = spi_info->writesize / 512;
	sect_addr = address / blks_per_sect;

    page_per_blk = spi_info->pages_per_blk;
    //logic_page_addr = sect_addr % page_per_blk;
	//logic_blk_addr = sect_addr / page_per_blk;
    logic_blk_addr = address;

    vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + logic_blk_addr));
    if (vt_blk_node == NULL) {
        spi_dbg("invalid logic blk addr %d \n", logic_blk_addr);
        return count;
    }
    do {

		phy_blk_addr = vt_blk_node->phy_blk_addr;
		phy_blk_node = &spi_info->phypmt[phy_blk_addr];

    	spi_dbg("logic: %d phy: %d \n", logic_blk_addr, phy_blk_addr);
    	spi_info->read_page(spi_info, phy_blk_addr, 0, sizeof(struct sect_map_head_t), spi_data_buf);

		sect_map_head = (struct sect_map_head_t *)spi_data_buf;
    	for (i=0; i<page_per_blk; i++) {
            spi_dbg("%02hhx ", sect_map_head->phy_page_map[i]);
            if ((i%32 == 0) && (i != 0))
                spi_dbg("\n");
    	}
    	spi_dbg("\n");

		spi_dbg("\n");
		for (i=0; i<(page_per_blk>>3); i++){

            spi_dbg("%02hhx ", phy_blk_node->phy_page_delete[i]);
            if ((i%32 == 0) && (i != 0))
                spi_dbg("\n");
		}
		spi_dbg("\n");

		vt_blk_node = vt_blk_node->next;
	} while (vt_blk_node != NULL);

	return count;
}

static struct class_attribute spiblock_class_attrs[] = {
    __ATTR(map_table,  S_IRUGO | S_IWUSR, NULL,    spiblock_sect_map_table),
    __ATTR(blk_table,  S_IRUGO | S_IWUSR, NULL,    spiblock_blk_map_table),
    __ATTR_NULL
};

int spi_initialize(struct spi_blk_t *spi_blk)
{
	struct mtd_info *mtd = spi_blk->mtd;
	struct spi_info_t *spi_info;
	struct spi_wl_t *spi_wl;
	struct phyblk_node_t *phy_blk_node;
	struct sect_map_head_t *sect_map_head;
	unsigned char spi_cmp_buf[(1 << spi_blk->write_shift)];
	int phy_blk_num, phy_page_addr, size_in_blk, phys_erase_shift, error = 0;
	//unsigned char head_buf[HEAD_LENGTH];

	spi_info = spi_malloc(sizeof(struct spi_info_t));
	if (!spi_info)
		return -ENOMEM;

	spi_blk->spi_info = spi_info;
	spi_info->mtd = mtd;
	spi_info->writesize = (1 << spi_blk->write_shift);
	phys_erase_shift = ffs(mtd->erasesize) - 1;
	size_in_blk =  (mtd->size >> phys_erase_shift);
	if (size_in_blk <= INFOTM_LIMIT_FACTOR)
		return -EPERM;

	spi_info->pages_per_blk = mtd->erasesize / spi_info->writesize;
	spi_info->startblock = 0;
	spi_info->endblock = size_in_blk;

	spi_info->copy_page_buf = spi_malloc(spi_info->writesize);
	if (!spi_info->copy_page_buf)
		return -ENOMEM;
	spi_info->phypmt = (struct phyblk_node_t *)spi_malloc((size_in_blk * sizeof(struct phyblk_node_t)));
	if (!spi_info->phypmt)
		return -ENOMEM;
	spi_info->vtpmt = (void **)spi_malloc((sizeof(void*) * size_in_blk));
	if (!spi_info->vtpmt)
		return -ENOMEM;
	memset((unsigned char *)spi_info->phypmt, 0xff, size_in_blk * sizeof(struct phyblk_node_t));

	spi_info->read_page = spi_read_page;
	spi_info->write_head = spi_write_head;
	spi_info->write_page = spi_write_page;
	spi_info->copy_page = spi_copy_page;
	spi_info->get_phy_sect_map = spi_get_phy_sect_map;
	spi_info->erase_block = spi_erase_block;

	spi_info->read_sect = spi_read_sect;
	spi_info->write_sect = spi_write_sect;
	spi_info->delete_sect = spi_delete_sect;

	error = spi_wl_init(spi_info);
	if (error)
		return error;

	spiblock_get_part_info(spi_blk);
	spi_wl = spi_info->spi_wl;
	spi_info->fillfactor = SPI_BASIC_FILLFACTOR;
	if (spi_info->startblock == size_in_blk)
		spi_info->accessibleblocks = 0;
	else
		spi_info->accessibleblocks = size_in_blk - spi_info->fillfactor - spi_info->startblock;
	memset(spi_cmp_buf, 0xff, spi_info->writesize);

	for (phy_blk_num=spi_info->startblock; phy_blk_num<size_in_blk; phy_blk_num++) {

		phy_page_addr = 0;
		phy_blk_node = &spi_info->phypmt[phy_blk_num];

		error = spi_info->read_page(spi_info, phy_blk_num, phy_page_addr, sizeof(struct sect_map_head_t), spi_info->copy_page_buf);
		if (error) {
			spi_dbg("get status error at blk: %d \n", phy_blk_num);
			continue;
		}

		sect_map_head = (struct sect_map_head_t *)spi_info->copy_page_buf;
		if (!strncmp(sect_map_head->head, SPI_SECT_MAP_HEAD, strlen((const char*)SPI_SECT_MAP_HEAD))) {
			if ((sect_map_head->vtblk >= (addr_blk_t)spi_info->accessibleblocks) || (sect_map_head->vtblk < 0)) {
				spi_dbg("add blk err: %d %d %d\n", phy_blk_num, sect_map_head->vtblk, spi_info->accessibleblocks);
		    	phy_blk_node->valid_sects = 0;
		    	phy_blk_node->last_write = 0;
		    	phy_blk_node->ec = sect_map_head->ec;
				spi_wl->add_free(spi_wl, phy_blk_num);
			} else {
    			spi_add_block(spi_info, phy_blk_num, sect_map_head);
    		}
			//for (i=0; i<spi_info->pages_per_blk; i+=8)
				//spi_dbg(" %x %x %x %x %x %x %x %x\n", phy_blk_node->phy_page_map[i], phy_blk_node->phy_page_map[i+1], phy_blk_node->phy_page_map[i+2], phy_blk_node->phy_page_map[i+3], phy_blk_node->phy_page_map[i+4], phy_blk_node->phy_page_map[i+5], phy_blk_node->phy_page_map[i+6], phy_blk_node->phy_page_map[i+7]);
		} else {
	    	phy_blk_node->valid_sects = 0;
	    	phy_blk_node->ec = 0;
	    	phy_blk_node->last_write = 0;
			spi_wl->add_free(spi_wl, phy_blk_num);
		}
	}

	spi_info->isinitialised = 0;
	spi_check_conflict_node(spi_info);
	spi_wl->gc_start_block = spi_info->accessibleblocks - 1;
	spi_info->cls.name = kzalloc(strlen((const char*)INFOTM_SPI_MAGIC)+1, GFP_KERNEL);

    strcpy((char *)spi_info->cls.name, INFOTM_SPI_MAGIC);
    spi_info->cls.class_attrs = spiblock_class_attrs;
   	error = class_register(&spi_info->cls);

	//spi_blk->fs_size = spi_info->accessibleblocks;
	//spi_blk->fs_size *= (mtd->erasesize - spi_info->writesize);
	spi_dbg("infotm spi initilize completely dev size: %d %d\n", spi_info->accessibleblocks, spi_wl->free_cnt);

	return 0;
}

static int spiblock_normal_readsect(struct mtd_blktrans_dev *dev, unsigned long block, char *buf)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct spi_info_t *spi_info = spi_blk->spi_info;
	uint32_t page_per_blk;
	addr_page_t phy_page_addr;
	addr_blk_t phy_blk_addr;
	int ret = 0, sect_addr;

	mutex_lock(&spi_blk->spi_lock);
	sect_addr = ((int)spi_part->offset >> spi_blk->write_shift) + (int)block;

	page_per_blk = spi_info->pages_per_blk;
	phy_page_addr = sect_addr % page_per_blk;
	phy_blk_addr = sect_addr / page_per_blk;

	if ((phy_blk_addr == spi_blk->cache_write_blk) && (spi_blk->cache_write_status[phy_page_addr]))
		memcpy(buf, spi_blk->cache_write_buf + phy_page_addr * spi_info->writesize, spi_info->writesize);
	else
		spi_info->read_page(spi_info, phy_blk_addr, phy_page_addr, spi_info->writesize, buf);
	mutex_unlock(&spi_blk->spi_lock);

	return ret;
}

static int spiblock_fs_readsect(struct mtd_blktrans_dev *dev, unsigned long block, char *buf)
{
	int ret = 0, sect_addr;
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct spi_info_t *spi_info = spi_blk->spi_info;

	if (!strncmp(spi_part->part_name, "boot", strlen((const char*)"boot"))
		|| !strncmp(spi_part->part_name, "normal", strlen((const char*)"normal"))) {
		spiblock_normal_readsect(dev, block, buf);
	} else  {
		mutex_lock(&spi_blk->spi_lock);
		sect_addr = ((int)spi_part->offset >> spi_blk->write_shift) + (int)block;
		ret = spi_info->read_sect(spi_info, sect_addr, 1, buf);
		mutex_unlock(&spi_blk->spi_lock);
	}

	return ret;
}

static int spiblock_normal_read_multisect(struct mtd_blktrans_dev *dev, unsigned long block, unsigned long nblock, char *buf)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct spi_info_t *spi_info = spi_blk->spi_info;
	struct mtd_info *mtd = spi_info->mtd;
	uint32_t page_per_blk;
	addr_page_t phy_page_addr;
	addr_blk_t phy_blk_addr;
	int ret = 0, i;
	loff_t from;
	size_t len, retlen;

	page_per_blk = spi_info->pages_per_blk;
	phy_page_addr = block % page_per_blk;
	phy_blk_addr = block / page_per_blk;

	from = mtd->erasesize;
	from *= phy_blk_addr;
	from += phy_page_addr * spi_info->writesize;
	len = nblock * spi_info->writesize;
	ret = mtd->_read(mtd, from, len, &retlen, buf);
	for (i=0; i<nblock; i++) {
		if ((phy_blk_addr == spi_blk->cache_write_blk) && (spi_blk->cache_write_status[phy_page_addr + i]))
			memcpy(buf + i * spi_info->writesize, spi_blk->cache_write_buf + (phy_page_addr + i) * spi_info->writesize, spi_info->writesize);
	}

	return ret;
}

static int spiblock_fs_read_multisect(struct mtd_blktrans_dev *dev, unsigned long block, unsigned long nblock, char *buf)
{
	int ret = 0, sect_addr, read_blks, reverse_flag = 0, adjust_sect_cnt = 0;
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct spi_info_t *spi_info = spi_blk->spi_info;
	char *read_buf;

	sect_addr = ((int)spi_part->offset >> spi_blk->write_shift) + (int)block;

	if ((spi_blk->cache_read_sect > 0) && (sect_addr >= spi_blk->cache_read_sect) && ((sect_addr - spi_blk->cache_read_sect + nblock) <= spi_part->bounce_cnt)) {
		memcpy(buf, spi_blk->cache_read_buf + (sect_addr - spi_blk->cache_read_sect) * spi_info->writesize, nblock * spi_info->writesize);
		return ret;
	}

	if (nblock < spi_part->bounce_cnt) {
		read_blks = spi_part->bounce_cnt;
		if ((spi_blk->cache_read_sect > 0) && (spi_blk->cache_read_sect > sect_addr) && ((spi_blk->cache_read_sect - sect_addr) < 16)) {
			sect_addr = (sect_addr + ((nblock + 7) & 0x8) - spi_part->bounce_cnt);
			if (sect_addr < ((int)spi_part->offset >> spi_blk->write_shift)) {
				adjust_sect_cnt = ((int)spi_part->offset >> spi_blk->write_shift) - sect_addr;
				sect_addr = ((int)spi_part->offset >> spi_blk->write_shift);
			}
			reverse_flag = 1;
		}
		read_buf = spi_blk->cache_read_buf;
	} else {
		read_blks = nblock;
		read_buf = buf;
	}

	if (!strncmp(spi_part->part_name, "boot", strlen((const char*)"boot"))
		|| !strncmp(spi_part->part_name, "normal", strlen((const char*)"normal"))) {
		spiblock_normal_read_multisect(dev, sect_addr, read_blks, read_buf);
	} else  {
		ret = spi_info->read_sect(spi_info, sect_addr, read_blks, read_buf);
	}
	if (nblock < spi_part->bounce_cnt) {
		if (reverse_flag)
			memcpy(buf, spi_blk->cache_read_buf + (spi_part->bounce_cnt - ((nblock + 7) & 0x8) - adjust_sect_cnt) * spi_info->writesize, nblock * spi_info->writesize);
		else
			memcpy(buf, spi_blk->cache_read_buf, nblock * spi_info->writesize);
		spi_blk->cache_read_sect = sect_addr;
	}

	return ret;
}

static int spiblock_normal_writesect(struct mtd_blktrans_dev *dev, unsigned long block, char *buf)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct mtd_info *mtd = spi_blk->mtd;
	struct spi_info_t *spi_info = spi_blk->spi_info;
	struct erase_info spi_erase_info;
	size_t retlen;
	uint32_t page_per_blk;
	addr_page_t phy_page_addr;
	addr_blk_t phy_blk_addr;
	int ret = 0, sect_addr, i;

	sect_addr = ((int)spi_part->offset >> spi_blk->write_shift) + (int)block;

	page_per_blk = spi_info->pages_per_blk;
	phy_page_addr = sect_addr % page_per_blk;
	phy_blk_addr = sect_addr / page_per_blk;

	if (spi_blk->cache_write_buf == NULL) {
		spi_blk->cache_write_buf = spi_malloc(mtd->erasesize);
		if (spi_blk->cache_write_buf == NULL) {
			mutex_unlock(&spi_blk->spi_lock);
			return -ENOMEM;
		}
		spi_blk->cache_write_blk = -1;
	}

	if ((spi_blk->cache_write_blk >= 0) && (phy_blk_addr != spi_blk->cache_write_blk)) {

		for (i=0; i<page_per_blk; i++) {
			if (spi_blk->cache_write_status[i] == 0)
				spi_info->read_page(spi_info, spi_blk->cache_write_blk, i, spi_info->writesize, spi_blk->cache_write_buf + i * spi_info->writesize);
		}
		memset(&spi_erase_info, 0, sizeof(struct erase_info));
		spi_erase_info.mtd = mtd;
		spi_erase_info.addr = mtd->erasesize;
		spi_erase_info.addr *= spi_blk->cache_write_blk;
		spi_erase_info.len = mtd->erasesize;

		mtd->_erase(mtd, &spi_erase_info);
		mtd->_write(mtd, spi_erase_info.addr, mtd->erasesize, &retlen, spi_blk->cache_write_buf);

		spi_blk->cache_write_blk = -1;
		memset(spi_blk->cache_write_status, 0, page_per_blk);
	}
	memcpy(spi_blk->cache_write_buf + phy_page_addr * spi_info->writesize, buf, spi_info->writesize);
	spi_blk->cache_write_blk = phy_blk_addr;
	spi_blk->cache_write_status[phy_page_addr] = 1;

	return ret;
}

static int spiblock_fs_writesect(struct mtd_blktrans_dev *dev, unsigned long block, char *buf)
{
	int ret = 0, sect_addr;
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct spi_info_t *spi_info = spi_blk->spi_info;

	if (!strncmp(spi_part->part_name, "boot", strlen((const char*)"boot"))
		|| !strncmp(spi_part->part_name, "normal", strlen((const char*)"normal"))) {
		spiblock_normal_writesect(dev, block, buf);
	} else {
		sect_addr = (int)(spi_part->offset >> spi_blk->write_shift) + (int)block;
		ret = spi_info->write_sect(spi_info, sect_addr, buf);
		if ((spi_blk->cache_read_sect > 0) && (sect_addr >= spi_blk->cache_read_sect) && ((sect_addr - spi_blk->cache_read_sect) < spi_part->bounce_cnt))
			memcpy(spi_blk->cache_read_buf + (sect_addr - spi_blk->cache_read_sect) * spi_info->writesize, buf, spi_info->writesize);
	}

	return ret;
}

static unsigned int spiblock_map_sg(struct spi_blk_t *spi_blk, struct spi_part_t *spi_part)
{
	unsigned int sg_len;
	size_t buflen;
	struct scatterlist *sg;
	int i;

	if (!spi_part->bounce_buf)
		return blk_rq_map_sg(spi_part->queue, spi_part->req, spi_part->sg);

	sg_len = blk_rq_map_sg(spi_part->queue, spi_part->req, spi_part->bounce_sg);

	spi_part->bounce_sg_len = sg_len;

	buflen = 0;
	for_each_sg(spi_part->bounce_sg, sg, sg_len, i)
		buflen += sg->length;

	sg_init_one(spi_part->sg, spi_part->bounce_buf, buflen);

	return sg_len;
}

static int spiblock_calculate_sg(struct spi_blk_t *spi_blk, struct spi_part_t *spi_part, size_t buflen, unsigned **buf_addr, unsigned *offset_addr)
{
	struct scatterlist *sgl;
	unsigned int offset = 0, segments = 0, buf_start = 0;
	struct sg_mapping_iter miter;
	unsigned long flags;
	unsigned int nents;
	unsigned int sg_flags = SG_MITER_ATOMIC;

	nents = spi_part->bounce_sg_len;
	sgl = spi_part->bounce_sg;

	if (rq_data_dir(spi_part->req) == WRITE)
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

static int spiblock_init_bounce_buf(struct mtd_blktrans_dev *dev, struct request_queue *rq)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct spi_info_t *spi_info = spi_blk->spi_info;
	int ret = 0;
	unsigned int bouncesz;
	unsigned long flags;

	spi_part->queue = rq;
#ifdef CONFIG_IMAPX_SPIMUL
    bouncesz = (spi_info->writesize * 32);
    spi_part->bounce_cnt = 32;
#else
    bouncesz = (spi_info->writesize * 8);
    spi_part->bounce_cnt = 8;
#endif

	spi_part->bounce_buf = spi_malloc(bouncesz);
	if (!spi_part->bounce_buf) {
		spi_dbg("not enough memory for bounce buf\n");
		 return -1;
	}

	spin_lock_irqsave(rq->queue_lock, flags);
	queue_flag_test_and_set(QUEUE_FLAG_NONROT, rq);
	blk_queue_bounce_limit(spi_part->queue, BLK_BOUNCE_HIGH);
	blk_queue_max_hw_sectors(spi_part->queue, bouncesz / 512);
	blk_queue_physical_block_size(spi_part->queue, bouncesz);
	blk_queue_max_segments(spi_part->queue, bouncesz / PAGE_CACHE_SIZE);
	blk_queue_max_segment_size(spi_part->queue, bouncesz);
	blk_queue_max_discard_sectors(spi_part->queue, ((spi_info->writesize * spi_info->pages_per_blk) >> 9));
	spin_unlock_irqrestore(rq->queue_lock, flags);

	spi_part->req = NULL;
	spi_part->sg = spi_malloc(sizeof(struct scatterlist));
	if (!spi_part->sg) {
		ret = -ENOMEM;
		blk_cleanup_queue(spi_part->queue);
		return ret;
	}
	sg_init_table(spi_part->sg, 1);

	spi_part->bounce_sg = spi_malloc(sizeof(struct scatterlist) * bouncesz / PAGE_CACHE_SIZE);
	if (!spi_part->bounce_sg) {
		ret = -ENOMEM;
		spi_free(spi_part->sg);
		spi_part->sg = NULL;
		blk_cleanup_queue(spi_part->queue);
		return ret;
	}
	sg_init_table(spi_part->bounce_sg, bouncesz / PAGE_CACHE_SIZE);

	return 0;
}

static int do_spiblktrans_request(struct mtd_blktrans_ops *tr, struct mtd_blktrans_dev *dev, struct request *req)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	int ret = 0, segments, i, j;
	unsigned long block, nblk, blk_addr, blk_cnt;
	unsigned short max_segm = queue_max_segments(spi_part->mbd.rq);
	unsigned *buf_addr[max_segm+1];
	unsigned offset_addr[max_segm+1];
	size_t buflen;
	char *buf;
	//struct timespec ts_write_start, ts_write_end;

	memset((unsigned char *)buf_addr, 0, (max_segm+1)*4);
	memset((unsigned char *)offset_addr, 0, (max_segm+1)*4);
	spi_part->req = req;
	block = blk_rq_pos(req) << 9 >> tr->blkshift;
	nblk = blk_rq_sectors(req);
	buflen = (nblk << tr->blkshift);

	if (req->cmd_type != REQ_TYPE_FS)
		return -EIO;

	if ((blk_rq_pos(req) + blk_rq_cur_sectors(req)) > get_capacity(req->rq_disk))
		return -EIO;

	if (req->cmd_flags & REQ_DISCARD)
		return tr->discard(dev, block, nblk);

	spi_part->bounce_sg_len = spiblock_map_sg(spi_blk, spi_part);
	segments = spiblock_calculate_sg(spi_blk, spi_part, buflen, buf_addr, offset_addr);
	if (offset_addr[segments+1] != (nblk << tr->blkshift))
		return -EIO;

	mutex_lock(&spi_blk->spi_lock);
	switch(rq_data_dir(req)) {
		case READ:
			for(i=0; i<(segments+1); i++) {
				//ktime_get_ts(&ts_write_start);
				blk_addr = (block + (offset_addr[i] >> tr->blkshift));
				blk_cnt = ((offset_addr[i+1] - offset_addr[i]) >> tr->blkshift);
				buf = (char *)buf_addr[i];
				if (tr->read_multisect(dev, blk_addr, blk_cnt, buf)) {
					ret = -EIO;
					break;
				}
				//ktime_get_ts(&ts_write_end);
				//printk("infotm %x spi %d %d read %d %d \n", buf, blk_addr, blk_cnt, (ts_write_end.tv_sec - ts_write_start.tv_sec), (ts_write_end.tv_nsec - ts_write_start.tv_nsec));
			}
			bio_flush_dcache_pages(spi_part->req->bio);
			break;

		case WRITE:
			bio_flush_dcache_pages(spi_part->req->bio);
			for(i=0; i<(segments+1); i++) {
				blk_addr = (block + (offset_addr[i] >> tr->blkshift));
				blk_cnt = ((offset_addr[i+1] - offset_addr[i]) >> tr->blkshift);
				buf = (char *)buf_addr[i];
				for (j=0; j<blk_cnt; j++) {
					if (tr->writesect(dev, blk_addr + j, buf + (j << tr->blkshift))) {
						ret = -EIO;
						break;
					}
				}
			}
			break;

		default:
			spi_dbg(KERN_NOTICE "Unknown request %u\n", rq_data_dir(req));
			break;
	}

	mutex_unlock(&spi_blk->spi_lock);

	return ret;
}

static void spiblock_erase_data(struct mtd_blktrans_dev *dev, unsigned int cmd, unsigned long arg)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
    struct mtd_info *mtd = spi_blk->mtd;
    void __user *argp = (void __user *)arg;
    struct erase_info *instr;
    loff_t from;
    size_t len;
    int erase_shift;

	if (strncmp(spi_part->part_name, "normal", strlen((const char*)"normal")))
		return;

	mutex_lock(&spi_blk->spi_lock);
	erase_shift = ffs(mtd->erasesize) - 1;

	instr = spi_malloc(sizeof(struct erase_info));
	if (!instr)
		goto exit;

	instr->mtd = mtd;
	if (cmd == MEMERASE64) {
		struct erase_info_user64 einfo64;

		if (copy_from_user(&einfo64, argp, sizeof(struct erase_info_user64))) {
			goto exit;
		}
		instr->addr = einfo64.start;
		instr->len = einfo64.length;
	} else {
		struct erase_info_user einfo32;

		if (copy_from_user(&einfo32, argp, sizeof(struct erase_info_user))) {
			goto exit;
		}
		instr->addr = einfo32.start;
		instr->len = einfo32.length;
	}

    from = instr->addr + spi_part->offset;
    len = instr->len;
    if (from & (mtd->erasesize - 1)) {
        spi_dbg("nftl erase addr not aligned %llx erasesize %x ", (uint64_t)from, mtd->erasesize);
        goto exit;
    }
    if (len & (mtd->erasesize - 1)) {
        spi_dbg("nftl erase length not aligned %llx erasesize %x ", (uint64_t)len, mtd->erasesize);
        len = (len + (mtd->erasesize - 1)) / (mtd->erasesize - 1);
    }

	mtd->_erase(mtd, instr);

exit:
	if (instr)
		spi_free(instr);
	mutex_unlock(&spi_blk->spi_lock);
	return;
}

static int spiblock_read_env(struct env_t *env, loff_t from, size_t len, uint8_t *buf)
{
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)env_to_spi_block(env);
	struct spi_info_t *spi_info = spi_blk->spi_info;
	struct spi_part_t *spi_part = &spi_blk->env_part;
	uint32_t page_per_blk;
	addr_page_t phy_page_addr;
	addr_blk_t phy_blk_addr;
	int ret = 0, sect_addr, sect_num, i;

	mutex_lock(&spi_blk->spi_lock);
	sect_addr = (int)((spi_part->offset + from) >> spi_blk->write_shift);
	sect_num = ((len + ((1 << spi_blk->write_shift) - 1)) >> spi_blk->write_shift);

	page_per_blk = spi_info->pages_per_blk;
	phy_page_addr = sect_addr % page_per_blk;
	phy_blk_addr = sect_addr / page_per_blk;

	for (i=0; i<sect_num; i++)
		ret = spi_info->read_page(spi_info, phy_blk_addr, phy_page_addr + i, spi_info->writesize, buf + i*spi_info->writesize);
	mutex_unlock(&spi_blk->spi_lock);

	return ret;
}

static int spiblock_write_env(struct env_t *env, loff_t from, size_t len, uint8_t *buf)
{
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)env_to_spi_block(env);
	struct spi_info_t *spi_info = spi_blk->spi_info;
	struct spi_part_t *spi_part = &spi_blk->env_part;
	struct mtd_info *mtd = spi_blk->mtd;
	struct erase_info spi_erase_info;
	size_t retlen;
	uint32_t page_per_blk;
	addr_page_t phy_page_addr;
	addr_blk_t phy_blk_addr;
	int sect_addr, sect_num, i;
	uint8_t *temp_data_buf = NULL;

	mutex_lock(&spi_blk->spi_lock);
	temp_data_buf = spi_malloc(1 << spi_blk->erase_shift);
	if (temp_data_buf == NULL) {
		mutex_unlock(&spi_blk->spi_lock);
		return -ENOMEM;
	}

	sect_addr = (int)((spi_part->offset + from) >> spi_blk->write_shift);
	sect_num = ((len + ((1 << spi_blk->write_shift) - 1)) >> spi_blk->write_shift);

	page_per_blk = spi_info->pages_per_blk;
	phy_page_addr = sect_addr % page_per_blk;
	phy_blk_addr = sect_addr / page_per_blk;

	for (i=0; i<page_per_blk; i++)
		spi_info->read_page(spi_info, phy_blk_addr, i, spi_info->writesize, temp_data_buf + i*spi_info->writesize);

	memcpy((temp_data_buf + phy_page_addr * spi_info->writesize), buf, len);
	memset(&spi_erase_info, 0, sizeof(struct erase_info));
	spi_erase_info.mtd = mtd;
	spi_erase_info.addr = mtd->erasesize;
	spi_erase_info.addr *= phy_blk_addr;
	spi_erase_info.len = mtd->erasesize;

	mtd->_erase(mtd, &spi_erase_info);
	mtd->_write(mtd, spi_erase_info.addr, mtd->erasesize, &retlen, temp_data_buf);

	spi_free(temp_data_buf);
	mutex_unlock(&spi_blk->spi_lock);

	return 0;
}

static void spiblock_creat_part(struct spi_blk_t *spi_blk, struct mtd_blktrans_ops *tr)
{
	struct spi_info_t *spi_info = spi_blk->spi_info;
	struct mtd_info *mtd = spi_blk->mtd;
    struct spi_part_t *spi_part = NULL;
	int i = 0, dev_num = 0;
	uint64_t offset = 0, left_size = 0;
	char spi_parts[64];
	char part_name[64];
	char part_type[64];
	int part_size;

	if (spi_info->bootblock > 0) {
		spi_part = spi_malloc(sizeof(struct spi_part_t));  
		if (!spi_part)
			return;

		sprintf(spi_part->part_name, "boot");
		spi_part->size = spi_info->bootblock;
		spi_part->size *= mtd->erasesize;           
		spi_part->offset = 0;
		spi_part->mbd.mtd = spi_blk->mtd;
		spi_part->mbd.devnum = dev_num++;
		spi_part->mbd.tr = tr;
		spi_part->mbd.size = (spi_part->size >> 9);
		spi_part->priv = spi_blk;

		offset += spi_part->size;
		tr->part_bits = 0;
		printk(KERN_INFO "spi add disk dev %s %llx %llx \n", spi_part->part_name, spi_part->offset, spi_part->size);

		if (add_mtd_blktrans_dev(&spi_part->mbd))
			printk(KERN_ALERT "spi add blk disk dev failed %s \n", spi_part->part_name);
	}

	if (spi_info->normalblock > 0) {
		spi_part = spi_malloc(sizeof(struct spi_part_t));  
		if (!spi_part)
			return;

		sprintf(spi_part->part_name, "normal");
		spi_part->size = spi_info->normalblock;
		spi_part->size *= mtd->erasesize;           
		spi_part->offset = offset;
		spi_part->mbd.mtd = spi_blk->mtd;
		spi_part->mbd.devnum = dev_num++;
		spi_part->mbd.tr = tr;
		spi_part->mbd.size = (spi_part->size >> 9);
		spi_part->priv = spi_blk;

		offset += spi_part->size;
		tr->part_bits = 0;
		printk(KERN_INFO "spi add disk dev %s %llx %llx \n", spi_part->part_name, spi_part->offset, spi_part->size);

		if (add_mtd_blktrans_dev(&spi_part->mbd))
			printk(KERN_ALERT "spi add blk disk dev failed %s \n", spi_part->part_name);
	}

	if (spi_info->accessibleblocks > 0) {

		left_size = spi_info->accessibleblocks;
		left_size *= (mtd->erasesize - spi_info->writesize);
		while (i < 64) {
			sprintf(spi_parts, "part%d", i);
			if (item_exist(spi_parts)) {
				item_string(part_name, spi_parts, 0);
				part_size = item_integer(spi_parts, 1);
				item_string(part_type, spi_parts, 2);
				if (!strncmp(part_type, "fs", strlen((const char*)"fs"))) {
					if (left_size == 0) {
						printk(KERN_ALERT "spi storage used up for %s \n", part_name);
						break;
					}

					spi_part = spi_malloc(sizeof(struct spi_part_t));  
					if (!spi_part)
						return;

					sprintf(spi_part->part_name, part_name);
					if (part_size == PART_SIZE_FULL) {
						spi_part->size = left_size;
					} else {
						spi_part->size = SZ_1K;
						spi_part->size *= part_size;
						if (left_size < spi_part->size) {
							printk(KERN_ALERT "spi not enough size for %s left: %dK require: %dK \n", part_name, (int)(left_size >> 10), (int)(spi_part->size >> 10));
							spi_part->size = left_size;
						}
					}
					spi_part->offset = offset;
					spi_part->mbd.mtd = spi_blk->mtd;
					spi_part->mbd.devnum = dev_num++;
					spi_part->mbd.tr = tr;
					spi_part->mbd.size = (spi_part->size >> 9);
					spi_part->priv = spi_blk;

					offset += spi_part->size;
					left_size -= spi_part->size;
					tr->part_bits = 0;
					printk(KERN_INFO "spi add disk dev %s %llx %llx left size: %dK \n", spi_part->part_name, spi_part->offset, spi_part->size, (int)(left_size >> 10));

					if (add_mtd_blktrans_dev(&spi_part->mbd))
						printk(KERN_ALERT "spi add blk disk dev failed %s \n", spi_part->part_name);
				}
			}
			i++;
		}
	}

    return;
}

static int spiblock_discard(struct mtd_blktrans_dev *dev, unsigned long block, unsigned nr_blocks)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct spi_info_t *spi_info = spi_blk->spi_info;
	int i;

	if (!strncmp(spi_part->part_name, "boot", strlen((const char*)"boot"))
		|| !strncmp(spi_part->part_name, "normal", strlen((const char*)"normal")))
		return 0;

	mutex_lock(&spi_blk->spi_lock);
	for (i=0; i<nr_blocks; i++)
		spi_info->delete_sect(spi_info, (int)(spi_part->offset >> spi_blk->write_shift) + block + i);
	mutex_unlock(&spi_blk->spi_lock);

	return 0;
}

static int spiblock_open(struct mtd_blktrans_dev *mbd)
{
	return 0;
}

static void spiblock_release(struct mtd_blktrans_dev *mbd)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(mbd);
	struct spi_blk_t *spi_blk = (struct spi_blk_t *)spi_part->priv;
	struct mtd_info *mtd = spi_blk->mtd;
	struct spi_info_t *spi_info = spi_blk->spi_info;
	struct erase_info spi_erase_info;
	size_t retlen;
	int i;

	mutex_lock(&spi_blk->spi_lock);
	if (spi_blk->cache_write_blk >= 0) {

		for (i=0; i<spi_info->pages_per_blk; i++) {
			if (spi_blk->cache_write_status[i] == 0)
				spi_info->read_page(spi_info, spi_blk->cache_write_blk, i, spi_info->writesize, spi_blk->cache_write_buf + i * spi_info->writesize);
		}
		memset(&spi_erase_info, 0, sizeof(struct erase_info));
		spi_erase_info.mtd = mtd;
		spi_erase_info.addr = mtd->erasesize;
		spi_erase_info.addr *= spi_blk->cache_write_blk;
		spi_erase_info.len = mtd->erasesize;

		mtd->_erase(mtd, &spi_erase_info);
		mtd->_write(mtd, spi_erase_info.addr, mtd->erasesize, &retlen, spi_blk->cache_write_buf);
		printk("spiblock_release write %d \n", spi_blk->cache_write_blk);

		spi_blk->cache_write_blk = -1;
		memset(spi_blk->cache_write_status, 0, spi_info->pages_per_blk);
	}
	mutex_unlock(&spi_blk->spi_lock);

	return;
}

static int spiblock_flush(struct mtd_blktrans_dev *dev)
{
	return 0;
}

static void spiblock_add_mtd(struct mtd_blktrans_ops *tr, struct mtd_info *mtd)
{
	struct spi_blk_t *spi_blk;

	if (mtd->type != MTD_NORFLASH)
		return;

	if (strncmp(mtd->name, SPI_BLK_NAME, strlen((const char*)SPI_BLK_NAME)))
		return;

	spi_blk = spi_malloc(sizeof(struct spi_blk_t));
	if (!spi_blk)
		return;

	spi_blk->cache_read_buf = spi_malloc(tr->blksize * 32);
	if (!spi_blk->cache_read_buf) {
		spi_dbg("not enough memory for cache read buf\n");
		 return;
	}
	spi_blk->cache_read_sect = -1;

    spi_blk->write_shift = fls(tr->blksize) - 1;
    spi_blk->erase_shift = fls(mtd->erasesize) - 1;
    spi_blk->pages_shift = spi_blk->erase_shift - spi_blk->write_shift;
    spi_blk->cache_write_blk = -1;
	spi_blk->mtd = mtd;

	mutex_init(&spi_blk->spi_lock);

	if (spi_initialize(spi_blk))
		return;

	spiblock_creat_part(spi_blk, tr);
#ifdef CONFIG_INFOTM_ENV
	spi_blk->spi_env.read_data = spiblock_read_env;
	spi_blk->spi_env.write_data = spiblock_write_env;
	infotm_register_env(&spi_blk->spi_env);
#endif
	return;
}

static void spiblock_remove_dev(struct mtd_blktrans_dev *dev)
{
	struct spi_part_t *spi_part = mbd_to_spi_part(dev);

	del_mtd_blktrans_dev(dev);
	spi_free(spi_part);	
}

static struct mtd_blktrans_ops spiblock_tr = {
	.name		= "spiblock",
	.major		= SPI_BLK_MAJOR,
	.part_bits	= 0,
	.blksize 	= 512,
	.open		= spiblock_open,
	.flush		= spiblock_flush,
	.release	= spiblock_release,
	.init_bounce_buf = spiblock_init_bounce_buf,
	.do_blktrans_request = do_spiblktrans_request,
	.erase_data = spiblock_erase_data,
	.readsect	= spiblock_fs_readsect,
	.read_multisect	= spiblock_fs_read_multisect,
	.writesect	= spiblock_fs_writesect,
	.add_mtd	= spiblock_add_mtd,
	.discard 	= spiblock_discard,
	.remove_dev	= spiblock_remove_dev,
	.owner		= THIS_MODULE,
};

static int __init init_spiblock(void)
{
	return register_mtd_blktrans(&spiblock_tr);
}

static void __exit cleanup_spiblock(void)
{
	deregister_mtd_blktrans(&spiblock_tr);
}

module_init(init_spiblock);
module_exit(cleanup_spiblock);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("xiaojun_yoyo");
MODULE_DESCRIPTION("nor flash translation friver");
