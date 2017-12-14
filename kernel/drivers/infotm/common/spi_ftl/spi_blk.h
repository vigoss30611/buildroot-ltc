#ifndef __SPI_BLK_H
#define __SPI_BLK_H

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/rbtree.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/vmalloc.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/mtd/blktrans.h>
#include <linux/time.h>
#include <linux/scatterlist.h>
#include <mach/nvedit.h>

typedef int16_t         addr_sect_t;
typedef int16_t         addr_blk_t;
typedef int16_t         erase_count_t;
typedef int32_t         addr_page_t;
typedef int16_t      	addr_vtblk_t;
typedef int32_t         addr_logic_t;
typedef int32_t         addr_physical_t;
typedef int16_t     	addr_linearblk_t;

#define spi_malloc(n)		kzalloc(n, GFP_KERNEL)
#define spi_free			kfree
#define spi_dbg				printk

#define UPGRADE_PARA_SIZE		0x400
#define RESERVED_SIZE			0x10000
#define TIMESTAMP_LENGTH         16
#define MAX_TIMESTAMP_NUM        ((1<<(TIMESTAMP_LENGTH-1))-1)
#define SPI_BLK_NAME			"root"
#define INFOTM_SPI_MAGIC		"infotm"
#define	SPI_SECT_MAP_HEAD		"spi_sect_map"
#define SPI_BLK_MAJOR			251
#define MAX_SECTS_IN_BLOCK		128
#define HEAD_LENGTH				16
#define BLOCK_INIT_VALUE		0xffff
#define SPI_BASIC_FILLFACTOR	 8
#define PART_SIZE_FULL           -1

#define DO_COPY_PAGE			 1
#define DO_COPY_PAGE_TOTAL		 2
#define DO_COPY_PAGE_AVERAGELY	 3

#define READ_OPERATION			 0
#define WRITE_OPERATION			 1

#define PART_NAME_LEN            32
#define INFOTM_LIMIT_FACTOR		 4
#define BASIC_BLK_NUM_PER_NODE	 2
#define MAKE_EC_BLK(ec, blk) ((ec<<16)|blk)

typedef enum {
	SPI_SUCCESS				=0,
	SPI_FAILURE				=1,
	SPI_PAGENOTFOUND		=2,
	SPI_BLKNOTFOUND      	=3,
}t_SPI_error;

struct sect_map_head_t {
	char head[HEAD_LENGTH];
    erase_count_t  ec;
    addr_vtblk_t   vtblk;
    int16_t        timestamp;
    int8_t    	   phy_page_map[MAX_SECTS_IN_BLOCK];
};

struct phyblk_node_t {
    erase_count_t  ec;
    addr_vtblk_t   vtblk;
    int8_t        valid_sects;
    int8_t    	   last_write;
    int16_t         timestamp;
    int8_t    	   phy_page_map[MAX_SECTS_IN_BLOCK];
    int8_t		   phy_page_delete[MAX_SECTS_IN_BLOCK>>3];
};

struct vtblk_node_t {
	addr_blk_t	phy_blk_addr;
	struct vtblk_node_t *next;
};

struct wl_rb_t {
	struct rb_node	rb_node;
	uint16_t			blk;
	uint16_t			ec;
};

struct free_list_t {
	struct list_head 	list;
	addr_blk_t 	phy_blk;
	erase_count_t  ec;
};

struct gc_blk_list {
	struct list_head list;
   	addr_blk_t	gc_blk_addr;	
};

struct wl_tree_t {
	struct rb_root  root;
	uint16_t		count;
};

struct spi_blk_t;
struct spi_info_t;
struct spi_wl_t;

struct spi_part_t {
    struct mtd_blktrans_dev mbd;
	struct request *req;
	struct request_queue *queue;
	struct scatterlist	*sg;
	struct scatterlist	*bounce_sg;
	unsigned int		bounce_sg_len;
	char			*bounce_buf;
	int          	bounce_cnt;

	char part_name[PART_NAME_LEN];
	uint64_t size;
	uint64_t offset;
	u_int32_t mask_flags;
	void *priv;
};

struct spi_wl_t {
	//struct wl_tree_t		free_root;
	//struct wl_tree_t		used_root;

	struct list_head		free_blk_list;
	struct list_head 		gc_blk_list;

	uint8_t			gc_need_flag;
	addr_blk_t		gc_start_block;
	addr_blk_t	  	wait_gc_block;
	addr_sect_t		page_copy_per_gc;

	uint16_t		pages_per_blk;
	int 			free_cnt;
	struct spi_info_t *spi_info;

	void (*add_free)(struct spi_wl_t *wl, addr_blk_t blk);
	void (*add_erased)(struct spi_wl_t *wl, addr_blk_t blk);
	void (*add_used)(struct spi_wl_t *wl, addr_blk_t blk);
	void (*add_bad)(struct spi_wl_t *wl, addr_blk_t blk);
	void (*add_gc)(struct spi_wl_t *wl, addr_blk_t blk);
	int32_t (*get_best_free)(struct spi_wl_t *wl, addr_blk_t *blk);

	int (*garbage_one)(struct spi_wl_t *spi_wl, addr_blk_t vt_blk, uint8_t gc_flag);
	int (*garbage_collect)(struct spi_wl_t *spi_wl, uint8_t gc_flag);
};

struct spi_blk_t {
    struct mtd_info *mtd;

    struct mutex spi_lock;
    int write_shift;
    int erase_shift;
    int pages_shift;

	unsigned char *cache_read_buf;
	unsigned char *cache_write_buf;
	int cache_read_sect;
	int cache_write_blk;
	int8_t cache_write_status[MAX_SECTS_IN_BLOCK];
	struct spi_info_t *spi_info;
	struct spi_part_t env_part;
	struct env_t spi_env;

	int (*read_data)(struct spi_blk_t *spi_blk, unsigned long block, unsigned nblk, unsigned char *buf);
	int (*write_data)(struct spi_blk_t *spi_blk, unsigned long block, unsigned nblk, unsigned char *buf);
};

struct spi_info_t{
	struct mtd_info *mtd;

	uint32_t	  writesize;
	uint32_t	  oobsize;
	uint32_t	  pages_per_blk;

	uint32_t	  bootblock;
	uint32_t	  normalblock;
    uint32_t	  startblock;
    uint32_t	  endblock;
    uint32_t      accessibleblocks;
    uint8_t       isinitialised;
    uint32_t      fillfactor;

    unsigned char *copy_page_buf;
    void   		**vtpmt;
    struct phyblk_node_t  *phypmt;

    struct spi_wl_t	*spi_wl;
    struct class      cls;

    int		(*read_page)(struct spi_info_t *spi_info, addr_blk_t blk_addr, addr_page_t page_addr, int read_len, unsigned char *data_buf);
    int 	(*write_head)(struct spi_info_t * spi_info, addr_blk_t blk_addr, addr_page_t page_addr, int page_offset, unsigned char *head_buf, int head_len);
    int		(*write_page)(struct spi_info_t *spi_info, addr_blk_t blk_addr, addr_page_t page_addr, addr_page_t logic_page_addr, unsigned char *data_buf);
    int    (*copy_page)(struct spi_info_t *spi_info, addr_blk_t dest_blk_addr, addr_page_t dest_page, addr_blk_t src_blk_addr, addr_page_t src_page, addr_page_t logic_page_addr);
    int    (*get_phy_sect_map)(struct spi_info_t *spi_info, addr_blk_t blk_addr);
    int    (*erase_block)(struct spi_info_t *spi_info, addr_blk_t blk_addr);

	int (*read_sect)(struct spi_info_t *spi_info, addr_page_t sect_addr, unsigned long nsect, unsigned char *buf);
	int (*write_sect)(struct spi_info_t *spi_info, addr_page_t sect_addr, unsigned char *buf);
	void (*delete_sect)(struct spi_info_t *spi_info, addr_page_t sect_addr);
};

static inline unsigned int spi_get_node_length(struct spi_info_t *spi_info, struct vtblk_node_t  *vt_blk_node)
{
	unsigned int node_length = 0;

	while (vt_blk_node != NULL) {
		node_length++;
		vt_blk_node = vt_blk_node->next;
	}
	return node_length;
}

extern int spi_initialize(struct spi_blk_t *spi_blk);
extern void spi_info_release(struct spi_info_t *spi_info);
extern int spi_wl_init(struct spi_info_t *spi_info);
extern int spi_check_node(struct spi_info_t *spi_info, addr_blk_t blk_addr);
extern int spi_add_node(struct spi_info_t *spi_info, addr_blk_t logic_blk_addr, addr_blk_t phy_blk_addr);
#endif


