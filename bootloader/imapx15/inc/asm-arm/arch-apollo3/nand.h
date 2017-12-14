#include <malloc.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
/* #include <linux/mtd/partitions.h> */

#ifndef __ARM_IMAPX_REGS_NAND
#define __ARM_IMAPX_REGS_NAND

/* Register Layout */
#define RESERVED_RRTB_SIZE					0x4000
#define NF2ECCLOCA			(0x110)
#define IMAPX800_DEFAULT_CLK			888
#define IMAPX800_NF_DELAY				5
#define NF_TRXF_FINISH_INT				(1)
#define NF_SDMA_FINISH_INT				(1<<5)
#define NF_RB_TIMEOUT_INT				(1<<13)
#define NF_SECC_UNCORRECT_INT			(1<<21)

#define NF_DMA_SDMA				(0)
#define NF_DMA_ADMA				(1)

#define NF_TRXF_START			(1<<0)
#define NF_ECC_START			(1<<1)
#define NF_DMA_START			(1<<2)

#define NF_CMD_CMD				(0)
#define NF_CMD_ADDR				(1)
#define NF_CMD_DATA_OUT			(2)
#define NF_CMD_DATA_IN			(3)

#define NF_FLAG_EXEC_DIRECTLY				(0)
#define NF_FLAG_EXEC_READ_RDY_WHOLE_PAGE	(1)
#define NF_FLAG_EXEC_READ_STATUS			(2)
#define NF_FLAG_EXEC_WRITE_WHOLE_PAGE		(3)
#define NF_FLAG_EXEC_CHECK_RB				(4)
#define NF_FLAG_EXEC_READ_WHOLE_PAGE		(5)
#define NF_FLAG_EXEC_CHECK_STATUS			(6)
#define NF_FLAG_EXEC_WAIT_RDY				(7)
#define NF_FLAG_DATA_FROM_INTER				(8)

#define NF_RANDOMIZER_ENABLE				(1)
#define NF_RANDOMIZER_DISABLE				(0)

#define NF_RESET				(1<<0)
#define TRXF_RESET				(1<<1)
#define ECC_RESET				(1<<2)
#define DMA_RESET				(1<<3)
#define FIFO_RESET				(1<<4)

#define ECCRAM_TWO_BUFFER		(0)
#define ECCRAM_FOUR_BUFFER		(1)
#define ECCRAM_EIGHT_BUFFER		(2)
#define ECC_DECODER				(0)
#define ECC_ENCODER				(1)

#define SECC0_RESULT_FLAG		(1<<6)
#define SECC0_RESULT_BITS_MASK	(0xf)

#define CONTROLLER_READY		0
#define CONTROLLER_BUSY			1
#define CONTROLLER_SUSPEND		2

#define DEFAULT_TIMING			0x31ffffff
#define DEFAULT_RNB_TIMEOUT		0xffffff
#define DEFAULT_PHY_READ		0x4023
#define DEFAULT_PHY_DELAY		0x400
#define DEFAULT_PHY_DELAY2		0x3818
#define	BUSW_8BIT				0
#define	BUSW_16BIT				1
#define ASYNC_INTERFACE			0
#define ONFI_SYNC_INTERFACE		1
#define TOGGLE_INTERFACE		2

#define NAND_BOOT_NAME		  "bootloader"
#define NAND_NORMAL_NAME	  "nandnormal"
#define NAND_NFTL_NAME		  "nandftl"

#define NAND_DEFAULT_OPTIONS			(NAND_ECC_BCH8_MODE)

#define INFOTM_NORMAL						0
#define INFOTM_MULTI_CHIP					1
#define INFOTM_MULTI_CHIP_SHARE_RB			2
#define INFOTM_CHIP_NONE_RB					4
#define INFOTM_INTERLEAVING_MODE			8
#define INFOTM_RANDOMIZER_MODE				16
#define INFOTM_RETRY_MODE					32
#define INFOTM_ESLC_MODE					64

#define INFOTM_BADBLK_POS					0
#define NAND_ECC_UNIT_SIZE					1024
#define NAND_SECC_UNIT_SIZE					16

#define NAND_BCH2_ECC_SIZE				4
#define NAND_BCH4_ECC_SIZE				8
#define NAND_BCH8_ECC_SIZE				16
#define NAND_BCH16_ECC_SIZE				28
#define NAND_BCH24_ECC_SIZE				44
#define NAND_BCH32_ECC_SIZE				56
#define NAND_BCH40_ECC_SIZE				72
#define NAND_BCH48_ECC_SIZE				84
#define NAND_BCH56_ECC_SIZE				100
#define NAND_BCH60_ECC_SIZE				108
#define NAND_BCH64_ECC_SIZE				112
#define NAND_BCH72_ECC_SIZE				128
#define NAND_BCH80_ECC_SIZE				140
#define NAND_BCH96_ECC_SIZE				168
#define NAND_BCH112_ECC_SIZE			196
#define NAND_BCH127_ECC_SIZE			224
#define NAND_BCH4_SECC_SIZE				8
#define NAND_BCH8_SECC_SIZE				12

#define NAND_ECC_OPTIONS_MASK			0x0000000f
#define NAND_PLANE_OPTIONS_MASK			0x000000f0
#define NAND_TIMING_OPTIONS_MASK		0x00000f00
#define NAND_BUSW_OPTIONS_MASK			0x0000f000
#define NAND_INTERLEAVING_OPTIONS_MASK	0x000f0000
#define NAND_RANDOMIZER_OPTIONS_MASK	0x00f00000
#define NAND_READ_RETRY_OPTIONS_MASK	0x0f000000
#define NAND_ESLC_OPTIONS_MASK			0xf0000000
#define NAND_ECC_BCH2_MODE				0x00000000
#define NAND_ECC_BCH4_MODE				0x00000001
#define NAND_ECC_BCH8_MODE				0x00000002
#define NAND_ECC_BCH16_MODE				0x00000003
#define NAND_ECC_BCH24_MODE				0x00000004
#define NAND_ECC_BCH32_MODE				0x00000005
#define NAND_ECC_BCH40_MODE				0x00000006
#define NAND_ECC_BCH48_MODE				0x00000007
#define NAND_ECC_BCH56_MODE				0x00000008
#define NAND_ECC_BCH60_MODE				0x00000009
#define NAND_ECC_BCH64_MODE				0x0000000a
#define NAND_ECC_BCH72_MODE				0x0000000b
#define NAND_ECC_BCH80_MODE				0x0000000c
#define NAND_ECC_BCH96_MODE				0x0000000d
#define NAND_ECC_BCH112_MODE			0x0000000e
#define NAND_ECC_BCH127_MODE			0x0000000f
#define NAND_SECC_BCH4_MODE				0x00000000
#define NAND_SECC_BCH8_MODE				0x00000001
#define NAND_TWO_PLANE_MODE				0x00000010
#define NAND_TIMING_MODE0				0x00000000
#define NAND_TIMING_MODE1				0x00000100
#define NAND_TIMING_MODE2				0x00000200
#define NAND_TIMING_MODE3				0x00000300
#define NAND_TIMING_MODE4				0x00000400
#define NAND_TIMING_MODE5				0x00000500
#define NAND_BUS16_MODE					0x00001000
#define NAND_INTERLEAVING_MODE			0x00010000
#define NAND_RANDOMIZER_MODE			0x00100000
#define NAND_READ_RETRY_MODE			0x01000000
#define NAND_ESLC_MODE					0x10000000

#define DEFAULT_T_REA					40
#define DEFAULT_T_RHOH					0

#define INFOTM_NAND_BUSY_TIMEOUT		0x60000
#define INFOTM_DMA_BUSY_TIMEOUT			0x100000
#define MAX_ID_LEN						8
#define MAX_REGISTER_LEN				32
#define MAX_RETRY_LEVEL					32
#define MAX_HYNIX_REGISTER_LEN			16
#define MAX_HYNIX_RETRY_LEVEL			16
#define MAX_ESLC_PARA_LEN				8

#define NAND_CMD_RANDOM_DATA_READ		0x05
#define NAND_CMD_PLANE2_READ_START		0x06
#define NAND_CMD_TWOPLANE_READ1_MICRO	0x32
#define NAND_CMD_TWOPLANE_PREVIOS_READ	0x60
#define NAND_CMD_TWOPLANE_READ1			0x5a
#define NAND_CMD_TWOPLANE_READ2			0xa5
#define NAND_CMD_TWOPLANE_WRITE2_MICRO	0x80
#define NAND_CMD_TWOPLANE_WRITE2		0x81
#define NAND_CMD_DUMMY_PROGRAM			0x11
#define NAND_CMD_ERASE1_END				0xd1
#define NAND_CMD_MULTI_CHIP_STATUS		0x78
#define NAND_CMD_SET_FEATURES			0xEF
#define NAND_CMD_GET_FEATURES			0xEE
#define SYNC_FEATURES			        0x10
#define ONFI_TIMING_ADDR				0x01
#define ONFI_READ_RETRY_ADDR			0x89
#define HYNIX_GET_PARAMETER				0x37
#define HYNIX_SET_PARAMETER				0x36
#define HYNIX_ENABLE_PARAMETER			0x16
#define SAMSUNG_SET_PARAMETER			0xA1
#define SANDISK_DYNAMIC_PARAMETER1		0x3B
#define SANDISK_DYNAMIC_PARAMETER2		0xB9
#define SANDISK_SWITCH_LEGACY			0x53
#define SANDISK_DYNAMIC_READ_ENABLE		0xB6
#define SANDISK_DYNAMIC_READ_DISABLE	0xD6
#define TOSHIBA_RETRY_PRE_CMD1			0x5C
#define TOSHIBA_RETRY_PRE_CMD2			0xC5
#define TOSHIBA_SET_PARAMETER			0x55
#define TOSHIBA_READ_RETRY_START1		0x26
#define TOSHIBA_READ_RETRY_START2		0x5D

#define HYNIX_OTP_SIZE					1026
#define HYNIX_OTP_OFFSET				2

#define PARAMETER_FROM_NONE				0
#define PARAMETER_FROM_NAND				1
#define PARAMETER_FROM_NAND_OTP			2
#define PARAMETER_FROM_IDS				3
#define PARAMETER_FROM_UBOOT			4

#define MAX_DMA_DATA_SIZE				0x8000
#define MAX_DMA_OOB_SIZE				0x2000
#define PAGE_MANAGE_SIZE				0x4
#define BASIC_RAW_OOB_SIZE				128

#define MAX_CHIP_NUM		4
#define MAX_PLANE_NUM		4
#define USER_BYTE_NUM		4
#define PARA_TYPE_LEN		4
#define PARA_SERIALS_LEN	64

#define NAND_STATUS_READY_MULTI			0x20

#define NAND_BLOCK_GOOD					0x5a
#define NAND_BLOCK_BAD					0xa5
#define NAND_MINI_PART_SIZE				0x1000000
#define MAX_ENV_BAD_BLK_NUM				128
#define NAND_MINI_PART_NUM				4
#define NAND_BBT_BLK_NUM				2
#define MAX_BAD_BLK_NUM					2000
#define MAX_MTD_PART_NUM				16
#define MAX_MTD_PART_NAME_LEN			24
#define ENV_NAND_MAGIC					"envx"
#define BBT_HEAD_MAGIC					"bbts"
#define BBT_TAIL_MAGIC					"bbte"
#define MTD_PART_MAGIC					"anpt"
#define SHIPMENT_BAD                    1
#define USELESS_BAD                     0
#define CONFIG_ENV_BBT_SIZE         	0x2000
#define ENV_BBT_SIZE                    (CONFIG_ENV_BBT_SIZE - (sizeof(uint32_t)))
#define NAND_SYS_PART_SIZE				0x10000000

#define RESERVED_PART_NAME		"reserved"
#define RESERVED_PART_SIZE		8

#define MAX_SEED_CYCLE				256
#define IMAPX800_NORMAL_SEED_CYCLE	8
#define MAX_UBOOT0_BLKS				64
#define IMAPX800_BOOT_COPY_NUM  	  4
#define IMAPX800_BOOT_PAGES_PER_COPY  512
#define IMAPX800_UBOOT0_SIZE    	  0x8000
#define IMAPXX15_UBOOT0_UNIT    	  0x400
#define IMAPX800_UBOOT00_SIZE  	      0x1000
#define IMAPX800_PARA_SAVE_SIZE		  0x800
#define IMAPX800_PARA_SAVE_PAGES	  2048
#define IMAPX800_UBOOT_MAGIC    	  0x0318
#define IMAPX800_VERSION_MAGIC    	  0x1375
#define IMAPX800_UBOOT_MAX_BAD   	  200
#define IMAPX800_RETRY_PARA_OFFSET	  1020
#define IMAPX800_UBOOT_PARA_MAGIC	  0x616f6f71
#define IMAPX800_RETRY_PARA_MAGIC	  0x349a8756
#define IMAPX800_PARA_ONESEED_SAVE_PAGES	  4

#define RESERVED_NAND_RRTB_NAME		"nandrrtb"
#define RESERVED_NAND_IDS_NAME		"idsrrtb"
#define RETRY_PARAMETER_MAGIC		0x8a7a6a5a
#define RETRY_PARAMETER_PAGE_MAGIC	0x8a6a4a2a
#define RRTB_NFTL_NODESAVE_MAGIC 	0x6c74666e
#define RETRY_PARAMETER_CMD_MAGIC	0xa56aa78a
#define IMAPX800_ESLC_PARA_MAGIC    0x45534c43

#define RRTB_RETRY_LEVEL_OFFSET		  	4
#define RRTB_MAGIC_OFFSET		  	  	8
#define RRTB_UBOOT0_PARAMETER_OFFSET  	0x14

#define RRTB_RETRY_PARAMETER_OFFSET		0x100
#define RRTB_RETRY_UBOOT_PARA_OFFSET	0x1000

#define INFOTM_PARA_STRING			"nandpara"
#define INFOTM_UBOOT0_STRING		"uboot0"
#define INFOTM_UBOOT1_STRING		"uboot1"
#define INFOTM_ITEM_STRING			"item"
#define INFOTM_BBT_STRING			"bbtt"
#define INFOTM_ENV_VALID_STRING		"envvalid"
#define INFOTM_ENV_FREE_STRING		"envfree"
#define INFOTM_UBOOT0_REWRITE_MAGIC	"infotm_uboot0_rewrite"

#define INFOTM_UBOOT0_OFFSET	 0
#define INFOTM_UBOOT1_OFFSET	 0x800000
#define INFOTM_ITEM_OFFSET	 	 0x1000000
#define INFOTM_MAGIC_PART_SIZE	 0x100000
#define INFOTM_MAGIC_PART_PAGES	 0x100

#define CDBG_NAND_ID_COUNT 			8
#define CDBG_MAGIC_NAND				0xdfbda458

#define CDBG_NF_DELAY(a, b) (a | (b << 8))
#define CDBG_NF_PARAMETER(a, b, c, d, e, f, g, h)	(a | (b << 2)	 \
		| (c << 4) | (d << 7)	 \
		| (e << 8) | (f << 16) | (g << 24) | (h << 28))

#define CDBG_NF_INTERFACE(x)	(x & 0x3)
#define CDBG_NF_BUSW(x)			((x >> 2) & 0x3)
#define CDBG_NF_CYCLE(x)		((x >> 4) & 0x7)
#define CDBG_NF_RANDOMIZER(x)	((x >> 7) & 1)
#define CDBG_NF_MLEVEL(x)		({int __t = (x >> 8) & 0xff;	 \
			(__t >= 80)? 12:	 \
			(__t >= 72)? 11:	 \
			(__t >= 64)? 10:	 \
			(__t >= 60)?  9:	 \
			(__t >= 56)?  8:	 \
			(__t >= 48)?  7:	 \
			(__t >= 40)?  6:	 \
			(__t >= 32)?  5:	 \
			(__t >= 24)?  4:	 \
			(__t >= 16)?  3:	 \
			(__t >=  8)?  2:	 \
			(__t >=  4)?  1: 0;})
#define CDBG_NF_SLEVEL(x) ((((x >> 16) & 0xff) >= 8)? 1: 0)
#define CDBG_NF_ECCEN(x) !!((x >> 8) & 0xffff)
#define CDBG_NF_ONFIMODE(x) ((x >> 24) & 0xf)
#define CDBG_NF_INTERNR(x) ((x >> 28) & 0x3)

struct infotm_nand_retry_parameter {
	int retry_level;
	int register_num;
	int parameter_source;
	int register_addr[MAX_REGISTER_LEN];
	int8_t parameter_offset[MAX_RETRY_LEVEL][MAX_REGISTER_LEN];
	int8_t otp_parameter[MAX_RETRY_LEVEL];
};

#define MAX_PAIRED_PAGES 256
#define START_ONLY	0
#define START_END	1

struct infotm_nand_eslc_parameter {
	int register_num;
	int parameter_set_mode;
	int register_addr[MAX_ESLC_PARA_LEN];
	int8_t parameter_offset[2][MAX_ESLC_PARA_LEN];
	uint8_t paired_page_addr[MAX_PAIRED_PAGES][2];
};

enum cdbg_nand_timing {
	NF_tREA = 0,
	NF_tREH,
	NF_tCLS,
	NF_tCLH,
	NF_tWH,
	NF_tWP,
	NF_tRP,
	NF_tDS,
	NF_tCAD,
	NF_tDQSCK,
};

struct infotm_nand_extend_para {
	unsigned max_seed_cycle;
	unsigned uboot0_rewrite_count;
};

struct cdbg_cfg_nand_x9 {
	uint32_t	magic;

	uint8_t		name[24];
	uint8_t		id[CDBG_NAND_ID_COUNT * 2];
	uint8_t		timing[12];

	uint16_t	pagesize;
	uint16_t	blockpages;		/* pages per block */
	uint16_t	blockcount;		/* blocks per device */
	uint16_t	bad0;			/* first bad block mark */
	uint16_t	bad1;			/* second bad block mark */

	uint16_t	sysinfosize;
	uint16_t	rnbtimeout;		/* ms */
	uint16_t	syncdelay;		/* percentage of a peroid [7:0] , phy cycle time (ns) [15:8]*/
	uint16_t	syncread;

	uint16_t	polynomial;
	uint32_t	seed[4];

	/* [1:  0]: interface: 0: legacy, 1: ONFI sync, 2: toggle
	 * [3:  2]: bus width: 0: 8-bit, 1: 16-bit
	 * [6:  4]: cycle
	 * [    7]: randomizer
	 * [15: 8]: main ECC bits
	 * [23:16]: spare ECC bits
	 * [27:24]: onfi timing mode
	 * [29:28]: inter chip nr
	 */
	uint32_t	parameter;
	uint32_t	resv0;		/* options */
	uint32_t	resv1;		/* retry parameter */
	uint32_t	resv2;		/* max seed cycle */
	uint32_t    resv3;          /* eslc parameter */
};

struct infotm_nand_part_info {
	char mtd_part_magic[4];
	char mtd_part_name[MAX_MTD_PART_NAME_LEN];
	uint64_t size;
	uint64_t offset;
	u_int32_t mask_flags;
};

struct nand_bbt_t {
    unsigned        reserved: 2;
    unsigned        bad_type: 2;
    unsigned        chip_num: 2;
    unsigned        plane_num: 2;
    int16_t         blk_addr;
};

struct infotm_nand_bbt_info {
	char bbt_head_magic[4];
	int  total_bad_num;
	struct nand_bbt_t nand_bbt[MAX_BAD_BLK_NUM];
	char bbt_tail_magic[4];
};

struct env_valid_node_t {
	int16_t  ec;
	int16_t	phy_blk_addr;
	int16_t	phy_page_addr;
	int timestamp;
};

struct env_free_node_t {
	int16_t  ec;
	int16_t	phy_blk_addr;
	int dirty_flag;
	struct env_free_node_t *next;
};

struct env_oobinfo_t {
	char name[4];
    int16_t  ec;
    unsigned        timestamp: 15;
    unsigned       status_page: 1;
};

struct infotm_nandenv_info_t {
	struct mtd_info *mtd;
	struct env_valid_node_t *env_valid_node;
	struct env_free_node_t *env_free_node;
	u_char env_valid;
	u_char env_init;
	u_char part_num_before_sys;
	struct infotm_nand_bbt_info nand_bbt_info;
};

struct infotm_bbt_t{
	uint32_t	crc;		/* CRC32 over data bytes	*/
	unsigned char	data[ENV_BBT_SIZE]; /* Environment data		*/
};

struct infotm_nand_ecc_desc{
    char * name;
    unsigned ecc_mode;
    unsigned ecc_unit_size;
    unsigned ecc_bytes;
    unsigned secc_mode;
    unsigned secc_unit_size;
    unsigned secc_bytes;
    unsigned page_manage_bytes;
    unsigned max_correct_bits;
};

struct infotm_nand_chip {
	struct list_head list;

	u8 mfr_type;
	unsigned onfi_mode;
	unsigned options;
	unsigned page_size;
	unsigned block_size;
	unsigned oob_size;
	unsigned virtual_page_size;
	unsigned virtual_block_size;
	u8 plane_num;
	u8 real_plane;
	u8 chip_num;
	u8 internal_chipnr;
	u8 half_page_en;
	unsigned internal_page_nums;

	unsigned internal_chip_shift;
	unsigned ecc_mode;
	unsigned secc_mode;
	unsigned secc_bytes;
	unsigned max_correct_bits;
	int retry_level_support;
	u8 ops_mode;
	u8 cached_prog_status;
	u8 max_ecc_mode;
	u8 prog_start;
	u8 short_mode;

	unsigned chip_enable[MAX_CHIP_NUM];
	unsigned rb_enable[MAX_CHIP_NUM];
	unsigned valid_chip[MAX_CHIP_NUM];
	unsigned retry_level[MAX_CHIP_NUM];
	void	**retry_cmd_list;
	struct infotm_nand_retry_parameter retry_parameter[MAX_CHIP_NUM];
	struct infotm_nand_eslc_parameter eslc_parameter[MAX_CHIP_NUM];
	unsigned page_addr;
	unsigned real_page;
	unsigned subpage_cache_page;
	unsigned char *subpage_cache_buf;
	unsigned char subpage_cache_status[MAX_CHIP_NUM][MAX_PLANE_NUM];
	int max_seed_cycle;
	uint16_t	data_seed[MAX_SEED_CYCLE][8];
	uint16_t	sysinfo_seed[MAX_SEED_CYCLE][8];
	uint16_t	ecc_seed[MAX_SEED_CYCLE][8];
	uint16_t	secc_seed[MAX_SEED_CYCLE][8];
	uint16_t	data_last1K_seed[MAX_SEED_CYCLE][8];
	uint16_t	ecc_last1K_seed[MAX_SEED_CYCLE][8];
	int ecc_status;
	int secc_status;
	int uboot0_size;
	int uboot0_unit;
	int uboot0_rewrite_count;
	int uboot0_blk[IMAPX800_BOOT_COPY_NUM];
	int uboot1_blk[IMAPX800_BOOT_COPY_NUM];
	int item_blk[IMAPX800_BOOT_COPY_NUM];
	int reserved_pages;

	struct mtd_info			mtd;
	struct nand_chip		chip;
	//struct infotm_nandenv_info_t *infotm_nandenv_info;
	struct infotm_nand_bbt_info *nand_bbt_info;
	struct cdbg_cfg_nand_x9 *infotm_nand_dev;
	struct infotm_nand_ecc_desc 	*ecc_desc;
	struct infotm_nand_platform	*platform;

	/* device info */
	void					*priv;

	//plateform operation function
	int		(*infotm_nand_hw_init)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_cmdfifo_start)(struct infotm_nand_chip *infotm_chip, int timeout);
	void	(*infotm_nand_adjust_timing)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_options_confirm)(struct infotm_nand_chip *infotm_chip);
	void 	(*infotm_nand_cmd_ctrl)(struct infotm_nand_chip *infotm_chip, int cmd,  unsigned int ctrl);
	void	(*infotm_nand_select_chip)(struct infotm_nand_chip *infotm_chip, int chipnr);
	void	(*infotm_nand_get_controller)(struct infotm_nand_chip *infotm_chip);
	void	(*infotm_nand_release_controller)(struct infotm_nand_chip *infotm_chip);
	int	    (*infotm_nand_read_byte)(struct infotm_nand_chip *infotm_chip, uint8_t *data, int count, int timeout);
	void	(*infotm_nand_write_byte)(struct infotm_nand_chip *infotm_chip, uint8_t data);
	void	(*infotm_nand_command)(struct infotm_nand_chip *infotm_chip, unsigned command, int column, int page_addr, int chipnr);
	void	(*infotm_nand_set_retry_level)(struct infotm_nand_chip *infotm_chip, int chipnr, int retry_level);
	void    (*infotm_nand_set_eslc_para)(struct infotm_nand_chip *infotm_chip, int chipnr, int start);
	int		(*infotm_nand_get_retry_table)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_get_eslc_para)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_wait_devready)(struct infotm_nand_chip *infotm_chip, int chipnr);
	int		(*infotm_nand_dma_read)(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode,  unsigned char *sys_info_buf, int sys_info_len, int secc_mode);
	int		(*infotm_nand_dma_write)(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode, unsigned char *sys_info_buf, int sys_info_len, int secc_mode, int prog_type);
	int		(*infotm_nand_hwecc_correct)(struct infotm_nand_chip *infotm_chip, unsigned size, unsigned oob_size);
	void	(*infotm_nand_save_para)(struct infotm_nand_chip *infotm_chip);
};

struct imapx800_nand_chip {
	struct list_head chip_list;

	int ce1_gpio_index;
	int ce2_gpio_index;
	int ce3_gpio_index;
	int power_gpio_index;

	unsigned chip_selected;
	unsigned rb_received;
	int drv_ver_changed;
	int hw_inited;

    unsigned		regs;
    unsigned char   *resved_mem;
    int     vitual_fifo_start;
    int     vitual_fifo_offset;

	unsigned char *nand_data_buf;
	unsigned char *sys_info_buf;

	u8 busw;
	int polynomial;
};

struct infotm_nand_platform {
	char *name;
	struct platform_nand_data platform_nand_data;
	struct infotm_nand_chip  *infotm_chip;
	struct cdbg_cfg_nand_x9 *infotm_nand_dev;
};

struct infotm_nand_device {
	struct infotm_nand_platform *infotm_nand_platform;
	u8 dev_num;
};

struct infotm_nand_para_save {
	uint32_t head_magic;
	uint32_t chip0_para_size;
	uint32_t timing;
	uint32_t boot_copy_num;
	int32_t uboot0_blk[IMAPX800_BOOT_COPY_NUM];
	int32_t uboot1_blk[IMAPX800_BOOT_COPY_NUM];
	int32_t item_blk[IMAPX800_BOOT_COPY_NUM];
	int32_t bbt_blk[NAND_BBT_BLK_NUM];
	int mfr_type;
	int valid_chip_num;
	int page_shift;
	int erase_shift;
	int pages_per_blk_shift;
	int oob_size;
	int seed_cycle;
	int uboot0_unit;
	uint16_t data_seed[IMAPX800_NORMAL_SEED_CYCLE][8];
	uint16_t ecc_seed[IMAPX800_NORMAL_SEED_CYCLE][8];
	uint32_t ecc_mode;
	uint32_t secc_mode;
	uint32_t ecc_bytes;
	uint32_t secc_bytes;
	uint32_t ecc_bits;
	uint32_t secc_bits;
	uint32_t nftl_node_magic;
	int32_t nftl_node_blk;
	uint32_t eslc_para_magic;
	int8_t eslc_para[2][MAX_ESLC_PARA_LEN];
	uint32_t retry_para_magic;
	int32_t retry_level_circulate;
	uint32_t retry_level_support;
	uint32_t retry_level[MAX_CHIP_NUM];
};


#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})



#define chip_list_to_imapx800(l)	container_of(l, struct infotm_nand_chip, list)
static inline struct imapx800_nand_chip *infotm_chip_to_imapx800(struct infotm_nand_chip *infotm_chip)
{
	return (struct imapx800_nand_chip *)infotm_chip->priv;
}

static inline struct infotm_nand_chip *mtd_to_nand_chip(struct mtd_info *mtd)
{
	return container_of(mtd, struct infotm_nand_chip, mtd);
}

static inline void *nand_zalloc(int n, int flags)
{
	int cnt;
	void *pointer = NULL;

	cnt = n + (0x20 - (n & 0x1f));
	pointer = kzalloc(cnt, flags);
	if (pointer != NULL)
		memset(pointer, 0, cnt);
	return pointer;
}

extern int16_t uboot0_default_seed[8];
extern unsigned imapx800_seed[][8];
extern unsigned imapx800_sysinfo_seed_1k[][8];
extern unsigned imapx800_ecc_seed_1k[][8];
extern unsigned imapx800_secc_seed_1k[][8];
extern unsigned imapx800_ecc_seed_1k_16bit[][8];
extern unsigned imapx800_sysinfo_seed_8k[][8];
extern unsigned imapx800_ecc_seed_8k[][8];
extern unsigned imapx800_secc_seed_8k[][8];
extern unsigned imapx800_sysinfo_seed_16k[][8];
extern unsigned imapx800_ecc_seed_16k[][8];
extern unsigned imapx800_secc_seed_16k[][8];

extern struct cdbg_cfg_nand_x9 infotm_nand_flash_ids[];
extern int infotm_nand_init(struct infotm_nand_chip *infotm_chip);
extern int add_mtd_device(struct mtd_info *mtd);
extern void gpio_output_set(int val, int pin);
extern void gpio_mode_set(int mode, int pin);
extern void gpio_dir_set(int dir, int pin);
#endif
