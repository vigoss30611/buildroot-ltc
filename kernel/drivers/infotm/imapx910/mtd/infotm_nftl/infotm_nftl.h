#ifndef __INFOTM_NFTL_H
#define __INFOTM_NFTL_H

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
#include <mach/nand.h>

typedef int16_t         addr_sect_t;		//-1 , free , 0~page , valid data
typedef int16_t         addr_blk_t;
typedef int16_t         erase_count_t;
typedef int32_t         addr_page_t;
typedef int16_t      	addr_vtblk_t;
typedef int32_t         addr_logic_t;
typedef int32_t         addr_physical_t;
typedef int16_t     	addr_linearblk_t;

#define infotm_nftl_malloc(n)		kzalloc(n, GFP_KERNEL)
#define infotm_nftl_free			kfree
#define infotm_nftl_dbg(x...)			printk(KERN_WARNING "[imapx_nftl_x9] "x)
 
/**
*/
//addr_sect_t nftl_get_sector(addr_sect_t addr,sect_map_t logic_map);

//#define NFTL_DONT_CACHE_DATA

#define INFOTM_NFTL_MAGIC			 "infotm"
#define INFOTM_PHY_NODE_SAVE_MAGIC	  "ndsave"
#define INFOTM_PHY_NODE_FREE_MAGIC	  "ndfree"
#define INFOTM_NFTL_MAJOR			 250
#define TIMESTAMP_LENGTH         16
#define MAX_TIMESTAMP_NUM        ((1<<(TIMESTAMP_LENGTH-1))-1)
#define MAX_PAGES_IN_BLOCK       256
#define MAX_BLKS_PER_SECTOR		 128
#define MAX_BLK_NUM_PER_NODE	 8
#define BASIC_BLK_NUM_PER_NODE	 2
#define DEFAULT_SPLIT_UNIT		 8
#define NFTL_BASIC_FILLFACTOR	 32

#define DEFAULT_SPLIT_UNIT_GARBAGE		8
#define NFTL_NODE_FREE		 			0
#define NFTL_NODE_WAIT_WRITE		 	1
#define NFTL_NODE_WRITED		 		2
#define NFTL_MAX_SCHEDULE_TIMEOUT		1000
#define NFTL_FLUSH_DATA_TIME			8
#define NFTL_FLUSH_RANDOM_TIME			100
#define NFTL_FLUSH_SEQUENCE_TIME		200
#define NFTL_CACHE_STATUS_IDLE			0
#define NFTL_CACHE_STATUS_READY			1
#define NFTL_CACHE_STATUS_READY_DONE	2
#define NFTL_CACHE_STATUS_DONE			3
#define NFTL_CACHE_FLUSH_UNIT			4
#define NFTL_DATA_FORCE_WRITE_LEN		8
#define NFTL_MEDIA_FORCE_WRITE_LEN		16
#define CACHE_CLEAR_ALL					1
#define INFOTM_NFTL_BOUNCE_SIZE	 		0x40000
#define INFOTM_LIMIT_FACTOR		 4
#define DO_COPY_PAGE			 1
#define DO_COPY_PAGE_TOTAL		 2
#define DO_COPY_PAGE_AVERAGELY	 3
#define AVERAGELY_COPY_NUM		 2
#define READ_OPERATION			 0
#define WRITE_OPERATION			 1
#define CACHE_LIST_NOT_FOUND	 1
#define PART_NAME_LEN            32
#define PART_SIZE_FULL           -1
#define UBOOT_WRITE_SIZE         0x80000
#define NFTL_MINI_PART_SIZE      0x800000

#define WL_DELTA		128

#define BLOCK_INIT_VALUE		0xffff

#define MAKE_EC_BLK(ec, blk) ((ec<<16)|blk)

#define SECTOR_GAP		0x1000
#define SECTOR_GAP_MASK	(SECTOR_GAP-1)
#define ROOT_SECT		(0*SECTOR_GAP)
#define LEAF_SECT		(1*SECTOR_GAP)
#define SROOT_SECT		(2*SECTOR_GAP)
#define SLEAF_SECT		(3*SECTOR_GAP)
#define SEND_SECT		(4*SECTOR_GAP)

#define NODE_ROOT		0x1
#define NODE_LEAF		0x2
#define DEFAULT_IDLE_FREE_BLK	12
#define STATUS_BAD_BLOCK	0
#define STATUS_GOOD_BLOCK	1
#define OPS_SEQUENCE	    0
#define OPS_RANDOM	        0x5a
#define OPS_RANDOM_BOUNCE	16
#define RESERVED_SAVE_NODE_NUM  8
#define CURRENT_WRITED_NUM	16
#define SAVE_NODE_VTBLK 		0x7a5a
#define SAVE_NODE_FREE_VTBLK 	0x7a6a
#define UBOOT0_VTBLK 			0x7a4a
#define UBOOT1_VTBLK 			0x7a3a
#define ITEM_VTBLK 				0x7a2a
#define BBT_VTBLK 				0x7a1a

typedef enum {
	INFOTM_NFTL_SUCCESS				=0,
	INFOTM_NFTL_FAILURE				=1,
	INFOTM_NFTL_INVALID_PARTITION		=2,
	INFOTM_NFTL_INVALID_ADDRESS		=3,
	INFOTM_NFTL_DELETED_SECTOR			=4,
	INFOTM_NFTL_FLUSH_ERROR			=5,
	INFOTM_NFTL_UNFORMATTED			=6,
	INFOTM_NFTL_UNWRITTEN_SECTOR		=7,
	INFOTM_NFTL_PAGENOTFOUND			=0x08,
	INFOTM_NFTL_NO_FREE_BLOCKS			=0x10,
	INFOTM_NFTL_STRUCTURE_FULL			=0x11,
	INFOTM_NFTL_NO_INVALID_BLOCKS		=0x12,
	INFOTM_NFTL_SECTORDELETED			=0x50,
	INFOTM_NFTL_ABT_FAILURE 			=0x51,
	INFOTM_NFTL_MOUNTED_PARTITION		=0,
	INFOTM_NFTL_UNMOUNTED_PARTITION	=1,
	INFOTM_NFTL_SPAREAREA_ERROR      	=0x13,
	INFOTM_NFTL_STATIC_WL_FINISH      	=0x14,
	INFOTM_NFTL_BLKNOTFOUND      		=0x15,
	INFOTM_NFTL_WTITE_RETRY      		=0x16,
}t_INFOTM_NFTL_error;

struct nftl_oobinfo_t{
	addr_sect_t    sect;
    erase_count_t  ec;
    addr_vtblk_t   vtblk;
    int16_t        timestamp;
    int8_t       status_page;
};

struct phyblk_node_t{
    erase_count_t  ec;
    int16_t       valid_sects;
    addr_vtblk_t   vtblk;
    addr_sect_t    last_write;
    int16_t		  status_page;
    int16_t       timestamp;
    uint32_t 	page_map_crc;
    addr_sect_t    phy_page_map[MAX_PAGES_IN_BLOCK];
    uint8_t		   phy_page_delete[MAX_PAGES_IN_BLOCK>>3];
};

struct vtblk_node_t {
	addr_blk_t	phy_blk_addr;
	struct vtblk_node_t *next;
};

struct phyblk_node_save_t {
    erase_count_t  ec;
    int16_t        valid_sects;
    addr_vtblk_t   vtblk;
    addr_sect_t    last_write;//offset_validation
    int16_t		   status_page;
    int16_t        timestamp;
    uint32_t 	page_map_crc;
    int8_t    	   phy_page_map[MAX_PAGES_IN_BLOCK];
    //uint8_t		   phy_page_delete[MAX_PAGES_IN_BLOCK>>3];
};

/*struct vtblk_special_node_t {
	struct vtblk_node_t *vtblk_node;
	addr_blk_t	ext_phy_blk_addr;
};*/

struct write_cache_node {

	struct list_head list;
	unsigned char *buf;
	int8_t node_status;
   	uint32_t vt_sect_addr;
   	unsigned char cache_fill_status[MAX_BLKS_PER_SECTOR];		//blk unit fill status every vitual sector
   	void *priv;
};

struct free_sects_list {

	struct list_head list;
   	uint32_t vt_sect_addr;	
   	unsigned char free_blk_status[MAX_BLKS_PER_SECTOR];
};

struct wl_rb_t {
	struct rb_node	rb_node;
	uint16_t			blk;		/*linear block*/
	uint16_t			ec;		/*erase count*/
};

struct wl_list_t {
	struct list_head 	list;
	addr_blk_t 	vt_blk;		/*logical block*/
	addr_blk_t 	phy_blk;
};

struct gc_blk_list {
	struct list_head list;
   	addr_blk_t	gc_blk_addr;	
};

/**
* tree root & node count 
*/
struct wl_tree_t {
	struct rb_root  root;	/*tree root*/
	uint16_t		count;	/*number of nodes in this tree*/
};

struct infotm_nftl_blk_t;
struct infotm_nftl_ops_t;
struct infotm_nftl_info_t;
struct infotm_nftl_wl_t;

struct infotm_nftl_ops_t{
    int		(*read_page)(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len);
    int		(*write_page)(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len);
    int     (*read_page_oob)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, unsigned char * nftl_oob_buf, int oob_len);
    int    (* blk_isbad)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
    int    (* blk_mark_bad)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
    int    (* erase_block)(struct infotm_nftl_info_t * infotm_nftl_info, addr_blk_t blk_addr);
};

struct infotm_nftl_part_t {
    struct mtd_blktrans_dev mbd;
	struct request *req;
	struct request_queue *queue;
	struct scatterlist	*sg;
	struct scatterlist	*bounce_sg;
	unsigned int		bounce_sg_len;
	char			*bounce_buf;
	int          	bounce_cnt;
	int cache_buf_cnt;

	char part_name[PART_NAME_LEN];
	uint64_t size;
	uint64_t offset;
	u_int32_t mask_flags;
	void *priv;
	int (*read_data)(struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf);
	int (*write_data)(struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf);
};

/**
 * weal leveling structure
 */
struct infotm_nftl_wl_t {
	/*tree for erased free blocks, dirty free blocks, used blocks*/
	struct wl_tree_t		erased_root;
	struct wl_tree_t		free_root;
	struct wl_tree_t		used_root;
	/*list for dynamic wl(leaf blocks), read ecc error blocks*/
	struct wl_list_t		readerr_head;
	struct list_head 		gc_blk_list;
	
	/*static wl threshold*/
	uint32_t		wl_delta;
	uint32_t		cur_delta;
	uint8_t			gc_need_flag;
	addr_blk_t		gc_start_block;
	addr_blk_t	  	wait_gc_block;
	addr_sect_t		page_copy_per_gc;

	/*
	 * pages per physical block & total blocks, get from hardware layer,
	 * init in function nftl_wl_init
	 */
	uint16_t		pages_per_blk;
	struct infotm_nftl_info_t *infotm_nftl_info;

	void (*add_free)(struct infotm_nftl_wl_t *wl, addr_blk_t blk);
	void (*add_erased)(struct infotm_nftl_wl_t *wl, addr_blk_t blk);
	void (*add_used)(struct infotm_nftl_wl_t *wl, addr_blk_t blk);
	void (*add_bad)(struct infotm_nftl_wl_t *wl, addr_blk_t blk);
	void (*add_gc)(struct infotm_nftl_wl_t *wl, addr_blk_t blk);
	int32_t (*get_best_free)(struct infotm_nftl_wl_t *wl, addr_blk_t *blk);

	/*nftl node info adapter function*/
	int (*garbage_one)(struct infotm_nftl_wl_t *infotm_nftl_wl, addr_blk_t vt_blk, uint8_t gc_flag);
	//int (*gc_structure_full)(struct infotm_nftl_wl_t *infotm_nftl_wl, addr_page_t logic_page_addr, unsigned char *buf);
	int (*garbage_collect)(struct infotm_nftl_wl_t *infotm_nftl_wl, uint8_t gc_flag);
};

struct infotm_nftl_blk_t{
    struct mtd_info *mtd;
    uint64_t actual_size;

	struct notifier_block nb;
	struct notifier_block nftl_pm_notifier;
	struct timer_list   cache_timer;
	struct timespec ts_write_start;

    struct list_head cache_list;
    struct list_head free_list;
    spinlock_t thread_lock;
    struct mutex nftl_lock;

    int write_shift;
    int erase_shift;
    int pages_shift;
    int total_cache_buf_cnt;
	int	nftl_pm_state;
	int cache_sect_addr;
	unsigned char *cache_buf;
	int uboot_write_blk;
	int uboot_write_offs;
	unsigned char *uboot_write_buf;
	uint8_t cache_time_expired;
	struct task_struct *nftl_thread;
	struct infotm_nftl_info_t *infotm_nftl_info;

	int (*read_data)(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf);
	int (*write_data)(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, unsigned long block, unsigned nblk, unsigned char *buf);
	int (*write_cache_data)(struct infotm_nftl_blk_t *infotm_nftl_blk, uint8_t cache_flag);
	int (*search_cache_list)(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, uint32_t sect_addr, uint32_t blk_pos, uint32_t blk_num, unsigned char *buf);
	int (*add_cache_list)(struct infotm_nftl_blk_t *infotm_nftl_blk, struct infotm_nftl_part_t *infotm_nftl_part, uint32_t sect_addr, uint32_t blk_pos, uint32_t blk_num, unsigned char *buf);
};

struct infotm_nftl_info_t{
	struct mtd_info *mtd;

	uint32_t	  writesize;
	uint32_t	  oobsize;
	uint32_t	  pages_per_blk;

    uint32_t	  startblock;
    uint32_t	  endblock;
    uint32_t      accessibleblocks;
    uint8_t       isinitialised;
    uint16_t	  cur_split_blk;
    uint16_t      default_split_unit;
    uint32_t      fillfactor;
    uint32_t	  current_write_num[CURRENT_WRITED_NUM];
    addr_blk_t	  current_write_block[CURRENT_WRITED_NUM];
    addr_sect_t	  continue_writed_sects;
    addr_blk_t	  current_save_node_block;
    int           shutdown_flag;
    uint8_t para_rewrite_flag;

    unsigned char *copy_page_buf;
    void   		**vtpmt;
    struct phyblk_node_t  *phypmt;
	unsigned char   *resved_mem;

    struct infotm_nftl_ops_t  *infotm_nftl_ops;
    struct infotm_nftl_wl_t	*infotm_nftl_wl;
    struct infotm_nand_bbt_info nand_bbt_info;
    struct class      cls;

    int		(*read_page)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len);
    int		(*write_page)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, unsigned char *data_buf, unsigned char *nftl_oob_buf, int oob_len);
    int    (*copy_page)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t dest_blk_addr, addr_page_t dest_page, addr_blk_t src_blk_addr, addr_page_t src_page);
    int    (*get_page_info)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr, addr_page_t page_addr, unsigned char * nftl_oob_buf, int oob_len);
    int    (*blk_isbad)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
    int    (*blk_mark_bad)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
    int    (*get_phy_sect_map)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
    int    (*erase_block)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
    int    (*erase_logic_block)(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
    int	   (*read_mini_part)(struct infotm_nftl_info_t *infotm_nftl_info, loff_t from, size_t len, unsigned char *buf);
    int	   (*write_mini_part)(struct infotm_nftl_info_t *infotm_nftl_info, loff_t from, size_t len, unsigned char *buf);

    int (* delete_sector)(struct infotm_nftl_info_t *infotm_nftl_info,addr_page_t page,uint32_t len);
	int (*read_sect)(struct infotm_nftl_info_t *infotm_nftl_info, addr_page_t sect_addr, unsigned char *buf);
	int (*write_sect)(struct infotm_nftl_info_t *infotm_nftl_info, addr_page_t sect_addr, unsigned char *buf);
	void (*delete_sect)(struct infotm_nftl_info_t *infotm_nftl_info, addr_page_t sect_addr);
	void (*creat_structure)(struct infotm_nftl_info_t *infotm_nftl_info);
	void (*save_phy_node)(struct infotm_nftl_info_t *infotm_nftl_info);
};

static inline unsigned int infotm_nftl_get_node_length(struct infotm_nftl_info_t *infotm_nftl_info, struct vtblk_node_t  *vt_blk_node)
{
	unsigned int node_length = 0;

	while (vt_blk_node != NULL) {
		node_length++;
		vt_blk_node = vt_blk_node->next;
	}
	return node_length;
}

extern void infotm_nftl_ops_init(struct infotm_nftl_info_t *infotm_nftl_info);
extern int infotm_nftl_initialize(struct infotm_nftl_blk_t *infotm_nftl_blk);
extern void infotm_nftl_reinitialize(struct infotm_nftl_blk_t *infotm_nftl_blk, addr_blk_t start_blk, addr_blk_t end_blk);
extern void infotm_nftl_info_release(struct infotm_nftl_info_t *infotm_nftl_info);
extern int infotm_nftl_wl_init(struct infotm_nftl_info_t *infotm_nftl_info);
extern void infotm_nftl_wl_reinit(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t start_blk, addr_blk_t end_blk);
extern int infotm_nftl_check_node(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t blk_addr);
extern int infotm_nftl_add_node(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t logic_blk_addr, addr_blk_t phy_blk_addr);
extern int infotm_nftl_badblock_handle(struct infotm_nftl_info_t *infotm_nftl_info, addr_blk_t phy_blk_addr, addr_blk_t logic_blk_addr);
#endif


