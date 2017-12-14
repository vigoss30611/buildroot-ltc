#include <linux/rbtree.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/freezer.h>

#include "spi_blk.h"

#define list_to_free_node(l)	container_of(l, struct free_list_t, list)
#define list_to_gc_node(l)	container_of(l, struct gc_blk_list, list)

static void add_free(struct spi_wl_t * wl, addr_blk_t blk)
{
	struct spi_info_t *spi_info = wl->spi_info;
	struct phyblk_node_t *phy_blk_node;
	struct free_list_t *free_add_list, *free_cur_list, *free_prev_list = NULL;
	struct list_head *l, *n;
	uint32_t  add_ec_blk, free_ec_blk;

	phy_blk_node = &spi_info->phypmt[blk];
	add_ec_blk = MAKE_EC_BLK(phy_blk_node->ec, blk);
	if (!list_empty(&wl->free_blk_list)) {
		list_for_each_safe(l, n, &wl->free_blk_list) {
			free_cur_list = list_to_free_node(l);
			phy_blk_node = &spi_info->phypmt[free_add_list->phy_blk];
			free_ec_blk = MAKE_EC_BLK(free_cur_list->ec, free_cur_list->phy_blk);
			if (free_cur_list->phy_blk == blk) {
				return;
			} else if (add_ec_blk < free_ec_blk) {
				free_cur_list = free_prev_list;
				break;
			} else {
				free_prev_list = free_cur_list;
			}
		}
	} else {
		free_cur_list = NULL;
	}

	free_add_list = spi_malloc(sizeof(struct free_list_t));
	if (!free_add_list)
		return;
	phy_blk_node = &spi_info->phypmt[blk];
	free_add_list->phy_blk = blk;
	free_add_list->ec = phy_blk_node->ec;

	if (free_cur_list != NULL)
		list_add(&free_add_list->list, &free_cur_list->list);
	else
		list_add(&free_add_list->list, &wl->free_blk_list);
	wl->free_cnt++;
	//spi_dbg("add free %d %d %d\n", blk, free_add_list->ec, wl->free_cnt);

	return;
}

static void add_gc(struct spi_wl_t* spi_wl, addr_blk_t gc_blk_addr)
{
	struct gc_blk_list *gc_add_list, *gc_cur_list, *gc_prev_list = NULL;
	struct list_head *l, *n;

	if (!list_empty(&spi_wl->gc_blk_list)) {
		list_for_each_safe(l, n, &spi_wl->gc_blk_list) {
			gc_cur_list = list_to_gc_node(l);
			if (gc_cur_list->gc_blk_addr == gc_blk_addr) {
				return;
			} else if (gc_blk_addr < gc_cur_list->gc_blk_addr) {
				gc_cur_list = gc_prev_list;
				break;
			} else {
				gc_prev_list = gc_cur_list;
			}
		}
	} else {
		gc_cur_list = NULL;
	}

	gc_add_list = spi_malloc(sizeof(struct gc_blk_list));
	if (!gc_add_list)
		return;
	gc_add_list->gc_blk_addr = gc_blk_addr;

	if (gc_cur_list != NULL)
		list_add(&gc_add_list->list, &gc_cur_list->list);
	else
		list_add(&gc_add_list->list, &spi_wl->gc_blk_list);

	return;
}

static int gc_get_dirty_block(struct spi_wl_t *spi_wl, uint8_t gc_flag)
{
	struct spi_info_t *spi_info = spi_wl->spi_info;
	struct gc_blk_list *gc_cur_list, *gc_copy_list = NULL;
	struct list_head *l, *n;
	struct vtblk_node_t  *vt_blk_node;
	struct phyblk_node_t *phy_blk_node;
	addr_blk_t dest_blk;
	int16_t vt_blk_num, start_free_num;
	int node_length, node_length_max = 0, valid_page, k;

	start_free_num = spi_wl->free_cnt;
	for (vt_blk_num = 0; vt_blk_num < spi_info->accessibleblocks; vt_blk_num++) {

		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
		if (vt_blk_node == NULL)
			continue;

		spi_check_node(spi_info, vt_blk_num);
	}
	if ((spi_wl->free_cnt - start_free_num) > 0) {
		spi_wl->wait_gc_block = BLOCK_INIT_VALUE;
		goto exit;
	}

	if (list_empty(&spi_wl->gc_blk_list)) {
		for (vt_blk_num=spi_info->accessibleblocks - 1; vt_blk_num>=0; vt_blk_num--) {

			vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
			if (vt_blk_node == NULL)
				continue;

			node_length = spi_get_node_length(spi_info, vt_blk_node);
			if (node_length >= BASIC_BLK_NUM_PER_NODE)
				spi_wl->add_gc(spi_wl, vt_blk_num);
		}
	}

    list_for_each_safe(l, n, &spi_wl->gc_blk_list) {
        gc_cur_list = list_to_gc_node(l);
        vt_blk_num = gc_cur_list->gc_blk_addr;

        vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
        node_length = spi_get_node_length(spi_info, vt_blk_node);
        if (node_length < BASIC_BLK_NUM_PER_NODE) {
            list_del(&gc_cur_list->list);
            spi_free(gc_cur_list);
            continue;
        }

        spi_check_node(spi_info, vt_blk_num);
        vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
        node_length = spi_get_node_length(spi_info, vt_blk_node);
        if (node_length < BASIC_BLK_NUM_PER_NODE) {
            list_del(&gc_cur_list->list);
            spi_free(gc_cur_list);
            continue;
        }

		if ((spi_wl->free_cnt- start_free_num) > 0) {
			spi_wl->wait_gc_block = BLOCK_INIT_VALUE;
			break;
		}

		if (spi_wl->free_cnt <= INFOTM_LIMIT_FACTOR) {
			vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk_num));
			dest_blk = vt_blk_node->phy_blk_addr;
			phy_blk_node = &spi_info->phypmt[dest_blk];
			spi_info->get_phy_sect_map(spi_info, dest_blk);
			valid_page = 0;
			for (k=0; k<(spi_wl->pages_per_blk-1); k++) {
				if (phy_blk_node->phy_page_map[k] > 0)
					valid_page++;
			}
			if (((valid_page < (phy_blk_node->last_write + 1)) && (gc_flag != DO_COPY_PAGE))
				&& (spi_wl->free_cnt > 0))
				continue;

			node_length = spi_get_node_length(spi_info, vt_blk_node);
			if (node_length > node_length_max) {
				node_length_max = node_length;
				spi_wl->wait_gc_block = vt_blk_num;
				gc_copy_list = gc_cur_list;
			}
			continue;
		}

		spi_wl->wait_gc_block = vt_blk_num;
		gc_copy_list = gc_cur_list;
		break;
    }
    if ((spi_wl->wait_gc_block > 0) && (gc_copy_list != NULL)) {
		list_del(&gc_copy_list->list);
		spi_free(gc_copy_list);
    }

exit:
	return (spi_wl->free_cnt - start_free_num);
}

static int32_t gc_copy_one(struct spi_wl_t* spi_wl, addr_blk_t vt_blk, uint8_t gc_flag)
{
	int gc_free = 0, node_length, node_length_cnt, k, writed_pages = 0;
	struct spi_info_t *spi_info = spi_wl->spi_info;
	struct phyblk_node_t *phy_blk_src_node, *phy_blk_node_dest;
	struct vtblk_node_t  *vt_blk_node, *vt_blk_node_free;
	addr_blk_t dest_blk, src_blk;
	addr_page_t dest_page, src_page;

	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk));
	if (vt_blk_node == NULL)
		return -ENOMEM;

	dest_blk = vt_blk_node->phy_blk_addr;
	phy_blk_node_dest = &spi_info->phypmt[dest_blk];
	node_length = spi_get_node_length(spi_info, vt_blk_node);

	for (k=0; k<(spi_wl->pages_per_blk-1); k++) {

		if (phy_blk_node_dest->phy_page_map[k] > 0)
			continue;

		node_length_cnt = 0;
		src_blk = -1;
		src_page = -1;
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk));
		while (vt_blk_node != NULL) {

			node_length_cnt++;
			src_blk = vt_blk_node->phy_blk_addr;
			phy_blk_src_node = &spi_info->phypmt[src_blk];
			if ((phy_blk_src_node->phy_page_map[k] > 0) && (phy_blk_src_node->phy_page_delete[k>>3] & (1 << (k % 8)))) {
				src_page = phy_blk_src_node->phy_page_map[k];
				break;
			}
			vt_blk_node = vt_blk_node->next;
		}
		if ((src_page < 0) || (src_blk < 0) || ((node_length_cnt < node_length) && (gc_flag == 0)))
			continue;

		dest_page = phy_blk_node_dest->last_write + 1;
		spi_info->copy_page(spi_info, dest_blk, dest_page, src_blk, src_page, k);

		writed_pages++;
	}

	node_length_cnt = 0;
	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk));
	while (vt_blk_node->next != NULL) {
		node_length_cnt++;
		vt_blk_node_free = vt_blk_node->next;
		if (((node_length_cnt == (node_length - 1)) && (gc_flag == 0))
			|| ((node_length_cnt > 0) && (gc_flag != 0))) {
			spi_wl->add_free(spi_wl, vt_blk_node_free->phy_blk_addr);
			vt_blk_node->next = vt_blk_node_free->next;
			spi_free(vt_blk_node_free);
			gc_free++;
			continue;
		}
		if (vt_blk_node->next != NULL)
			vt_blk_node = vt_blk_node->next;
	}
	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk));
	spi_dbg("gc copy: %d %d %d %d\n", vt_blk_node->phy_blk_addr, vt_blk, writed_pages, spi_wl->free_cnt);

	return gc_free;
}

static int gc_copy_special(struct spi_wl_t* spi_wl, addr_blk_t vt_blk)
{
	int node_length, k, head_len;
	struct spi_info_t *spi_info = spi_wl->spi_info;
	struct phyblk_node_t *phy_blk_src_node, *phy_blk_node_dest;
	struct vtblk_node_t  *vt_blk_node;
	addr_blk_t dest_blk, src_blk;
	addr_page_t dest_page, src_page;
	unsigned temp_data_buf[spi_wl->pages_per_blk];
	struct sect_map_head_t *sect_map_head;
	uint8_t head_buf[64];

	for (k=0; k<(spi_wl->pages_per_blk-1); k++) {

		temp_data_buf[k] = 0;
		src_blk = -1;
		src_page = -1;
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk));
		while (vt_blk_node != NULL) {

			src_blk = vt_blk_node->phy_blk_addr;
			phy_blk_src_node = &spi_info->phypmt[src_blk];
			if ((phy_blk_src_node->phy_page_map[k] > 0) && (phy_blk_src_node->phy_page_delete[k>>3] & (1 << (k % 8)))) {
				src_page = phy_blk_src_node->phy_page_map[k];
				break;
			}
			vt_blk_node = vt_blk_node->next;
		}
		if ((src_page < 0) || (src_blk < 0))
			continue;

		temp_data_buf[k] = (unsigned)spi_malloc(spi_info->writesize);
		if (temp_data_buf[k] == 0)
			return -ENOMEM;
		spi_info->read_page(spi_info, src_blk, src_page, spi_info->writesize, (unsigned char *)temp_data_buf[k]);
	}

	vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk));
	node_length = spi_get_node_length(spi_info, vt_blk_node);
	while (vt_blk_node != NULL) {

		spi_wl->add_free(spi_wl, vt_blk_node->phy_blk_addr);
		*(spi_info->vtpmt + vt_blk) = vt_blk_node->next;
		spi_free(vt_blk_node);
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + vt_blk));
	}

	if(spi_wl->get_best_free(spi_wl, &dest_blk)) {
		spi_dbg("copy special not free %d \n", vt_blk);
		return -ENOMEM;
	}

	spi_add_node(spi_info, vt_blk, dest_blk);

	phy_blk_node_dest = &spi_info->phypmt[dest_blk];
	memcpy(head_buf, SPI_SECT_MAP_HEAD, strlen(SPI_SECT_MAP_HEAD));
	sect_map_head = (struct sect_map_head_t *)head_buf;
	sect_map_head->ec = phy_blk_node_dest->ec;
	sect_map_head->vtblk = phy_blk_node_dest->vtblk;
	sect_map_head->timestamp = phy_blk_node_dest->timestamp;
	head_len = sizeof(addr_vtblk_t) + sizeof(int16_t);
	spi_info->write_head(spi_info, dest_blk, 0, HEAD_LENGTH + sizeof(erase_count_t), head_buf + HEAD_LENGTH + sizeof(erase_count_t), head_len);

	for (k=0; k<(spi_wl->pages_per_blk-1); k++) {
		if (temp_data_buf[k] != 0) {
			dest_page = phy_blk_node_dest->last_write + 1;
			spi_info->write_page(spi_info, dest_blk, dest_page, k, (unsigned char *)temp_data_buf[k]);
			spi_free((unsigned char *)temp_data_buf[k]);
		}
	}
	spi_dbg("copy special free %d  %d \n", spi_wl->free_cnt, node_length);

	return node_length;
}

static int garbage_collect(struct spi_wl_t *spi_wl, uint8_t gc_flag)
{
	struct vtblk_node_t  *vt_blk_node;
	struct phyblk_node_t *phy_blk_node;
	addr_blk_t dest_blk;
	int gc_num = 0, copy_page_total_num, valid_page, k, head_len;
	struct spi_info_t *spi_info = spi_wl->spi_info;
	struct sect_map_head_t *sect_map_head;
	uint8_t head_buf[64];


	if ((spi_info->fillfactor / 4) >= INFOTM_LIMIT_FACTOR)
		copy_page_total_num = spi_info->fillfactor / 4;
	else
		copy_page_total_num = INFOTM_LIMIT_FACTOR;

	if (spi_wl->wait_gc_block < 0) {
		if (gc_get_dirty_block(spi_wl, 0) == 0) {
    		if (spi_wl->wait_gc_block < 0)
    			gc_get_dirty_block(spi_wl, DO_COPY_PAGE);
		}
	}
	if (spi_wl->wait_gc_block >= 0) {

		spi_wl->page_copy_per_gc = (spi_wl->pages_per_blk - 1);
		vt_blk_node = (struct vtblk_node_t *)(*(spi_info->vtpmt + spi_wl->wait_gc_block));
		dest_blk = vt_blk_node->phy_blk_addr;
		phy_blk_node = &spi_info->phypmt[dest_blk];
		spi_info->get_phy_sect_map(spi_info, dest_blk);
		valid_page = 0;
		for (k=0; k<(spi_wl->pages_per_blk-1); k++) {
			if (phy_blk_node->phy_page_map[k] > 0)
				valid_page++;
		}

		if (valid_page < (phy_blk_node->last_write + 1)) {
			if(spi_wl->get_best_free(spi_wl, &dest_blk)) {
				spi_dbg("spi garbage couldn`t found free block: %d %d\n", spi_wl->wait_gc_block, spi_wl->free_cnt);
				gc_num = gc_copy_special(spi_wl, spi_wl->wait_gc_block);
				if (gc_num > 0)
					spi_wl->wait_gc_block = BLOCK_INIT_VALUE;
				return 0;
			}

			spi_add_node(spi_info, spi_wl->wait_gc_block, dest_blk);
			//spi_wl->add_used(spi_wl, dest_blk);

			phy_blk_node = &spi_info->phypmt[dest_blk];
			memcpy(head_buf, SPI_SECT_MAP_HEAD, strlen(SPI_SECT_MAP_HEAD));
			sect_map_head = (struct sect_map_head_t *)head_buf;
			sect_map_head->ec = phy_blk_node->ec;
			sect_map_head->vtblk = phy_blk_node->vtblk;
			sect_map_head->timestamp = phy_blk_node->timestamp;
			head_len = sizeof(addr_vtblk_t) + sizeof(int16_t);
			spi_info->write_head(spi_info, dest_blk, 0, HEAD_LENGTH + sizeof(erase_count_t), head_buf + HEAD_LENGTH + sizeof(erase_count_t), head_len);
		}

		gc_num = spi_wl->garbage_one(spi_wl, spi_wl->wait_gc_block, DO_COPY_PAGE_AVERAGELY);
		if (gc_num > 0)
			spi_wl->wait_gc_block = BLOCK_INIT_VALUE;

		return gc_num;
	}

    return 0;
}

static int32_t get_best_free(struct spi_wl_t *wl, addr_blk_t* blk)
{
	struct spi_info_t *spi_info = wl->spi_info;
	struct free_list_t *free_get_list = NULL;
	struct list_head *l;

	if (!list_empty(&wl->free_blk_list)) {
		l = &wl->free_blk_list;
		free_get_list = list_to_free_node(l->next);
	} else {
		spi_dbg("get free failed empty \n");
		return -ENOENT;
	}

	if (free_get_list != NULL) {
		*blk = free_get_list->phy_blk;
		list_del(&free_get_list->list);
		spi_free(free_get_list);
	} else {
		spi_dbg("get free failed \n");
		return -ENOENT;
	}
	wl->free_cnt--;

	spi_info->erase_block(spi_info, *blk);
	//spi_dbg("get_best_free %d %d \n", *blk, wl->free_cnt);

	return 0;
}

int spi_wl_init(struct spi_info_t *spi_info)
{
	struct spi_wl_t *spi_wl = spi_malloc(sizeof(struct spi_wl_t));
	if(!spi_wl)
		return -1;

	spi_wl->spi_info = spi_info;
	spi_info->spi_wl = spi_wl;

	spi_wl->pages_per_blk = spi_info->pages_per_blk;
	spi_wl->gc_start_block = spi_info->accessibleblocks - 1;

	INIT_LIST_HEAD(&spi_wl->gc_blk_list);
	INIT_LIST_HEAD(&spi_wl->free_blk_list);
	spi_wl->wait_gc_block = BLOCK_INIT_VALUE;

	/*init function pointer*/
	spi_wl->add_free = add_free;
	spi_wl->add_gc = add_gc;
	spi_wl->get_best_free = get_best_free;

	spi_wl->garbage_one = gc_copy_one;
	spi_wl->garbage_collect = garbage_collect;

	return 0;
}


