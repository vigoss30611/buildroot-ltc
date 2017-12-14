//#include <bootlist.h>
#include <malloc.h>
#include <linux/types.h>
#include <vstorage.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <linux/byteorder/swab.h>
#include <efuse.h>
#include <preloader.h>
#include <ssp.h>
#include <asm/arch/ssp.h>
#include <gdma.h>
#include <cpuid.h>
#include <asm/io.h>

/* Flash opcodes. */
#define	OPCODE_WREN		0x06	/* Write enable */
#define	OPCODE_RDSR		0x05	/* Read status register */
#define	OPCODE_RDSR2		0x35	/* Read status register2, only for devices of winbond */
#define	OPCODE_WRSR		0x01	/* Write status register 1 byte */
#define	OPCODE_WRSR2		0x31	/* Write status register2 1 byte, only for devices of winbond */
#define	OPCODE_NORM_READ	0x03	/* Read data bytes (low frequency) */
#define	OPCODE_FAST_READ	0x0b	/* Read data bytes (high frequency) */
#define   OPCODE_DREAD		0x3b
#define   OPCODE_QREAD		0x6b
#define	OPCODE_PP			0x02	/* Page program (up to 256 bytes) */
#define	OPCODE_QPP		0x32	/* Quad Page program (up to 256 bytes) */
#define	OPCODE_4PP		0x38	/* 4 x IO Page program (up to 256 bytes) */
#define	OPCODE_BE_4K		0x20	/* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52	/* Erase 32KiB block */
#define	OPCODE_CHIP_ERASE	0xc7	/* Erase whole flash chip */
#define	OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_RDID		0x9f	/* Read JEDEC ID */
#define   OPCODE_EN_RESET     0x66
#define   OPCODE_RESET_CHIP   0x99

/* Used for SST flashes only. */
#define	OPCODE_BP		0x02	/* Byte program */
#define	OPCODE_WRDI		0x04	/* Write disable */
#define	OPCODE_AAI_WP	0xad	/* Auto address increment word program */

/* Used for SST flashes SST25VF010A only. */
#define OPCODE_EWSR            0x50    /* Enable write status register */
#define OPCODE_READ_ID       0x90    /* Read ID */
#define OPCODE_AAI             0xaf    /* AAI program */

/* Used for Macronix flashes only. */
#define	OPCODE_EN4B		0xb7	/* Enter 4-byte mode */
#define	OPCODE_EX4B		0xe9	/* Exit 4-byte mode */

/* Used for Spansion flashes only. */
#define	OPCODE_BRWR		0x17	/* Bank register write */

/* Status Register bits. */
#define	SR_WIP			1	/* Write in progress */
#define	SR_WEL			2	/* Write enable latch */
/* meaning of other SR_* bits may differ between vendors */
#define	SR_BP0			4	/* Block protect 0 */
#define	SR_BP1			8	/* Block protect 1 */
#define	SR_BP2			0x10	/* Block protect 2 */
#define	SR_TB			0x20	/* Top/Bottom protect */
#define	SR_SEC			0x40	/* Sector protect */
#define	SR_SRWD		0x80	/* SR write protect */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_JIFFIES	(40 * HZ)	/* M25P16 specs 40s max chip erase */
#define	MAX_CMD_SIZE		5
#define   SR_QUAD_EN_MX           BIT(6)
#define   SR_QUAD_EN_WB           BIT(1)
#define	SR_CMP					0x40	/* Complement protect */

enum read_mode {
	FLASH_RX_NORMAL = 0,
	FLASH_RX_FAST,
	FLASH_RX_DUAL,
	FLASH_RX_QUAD,
};

enum write_mode {
	FLASH_TX_NORMAL = 0,
	FLASH_TX_DUAL,
	FLASH_TX_QUAD,
};
#define SPI_NAME_SIZE	32
#define FLASH_LARGE_SIZE	0x1000000  //16M
#define FLASH_ERASE_SIZE (64*1024)

struct spi_device_id {
	char name[SPI_NAME_SIZE];
	unsigned long driver_data;	/* Data private to the driver */
};

#define BIT(x)	(1 << (x))

struct flash_info {
	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32	jedec_id;
	u16  ext_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16	n_sectors;

	u16  page_size;
	u16	addr_width;

	u16	flags;
#define	SECT_4K		0x01		/* OPCODE_BE_4K works uniformly */
#define	FLASH_NO_ERASE	0x02		/* No erase command needed */
#define	SST_WRITE	0x04		/* use SST byte programming */

#define SPI_NOR_DUAL_READ BIT(5)
#define SPI_NOR_QUAD_READ BIT(6)
#define SPI_NOR_DUAL_WRITE BIT(7)
#define SPI_NOR_QUAD_WRITE BIT(8)

};
struct flash_data {
	struct flash_info	flash_ic;
	u16			page_size;
	u16			addr_width;
	u8			erase_opcode;
	uint32_t erasesize;
	u8			*command;
	u8			fast_read;
	enum read_mode		flash_read;
	enum write_mode		flash_write;
	int			set4byte;
	u32 	    g_jedec_id;
	int flash_inited;
	uint64_t flash_size;	 // Total size of the MTD
};

static int flash_log_state = 0;
struct flash_data	flash_info_data;

#define flash_debug(format,args...)  \
	if(flash_log_state)\
		printf(format,##args)

#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
	((unsigned long)&(struct flash_info) {				\
		.jedec_id = (_jedec_id),				\
		.ext_id = (_ext_id),					\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),					\
	})

static const struct spi_device_id flash_ids[] = {

	/* Macronix */
	{ "mx25l2005a",  INFO(0xc22012, 0, 64 * 1024,   4, SECT_4K) },
	{ "mx25l4005a",  INFO(0xc22013, 0, 64 * 1024,   8, SECT_4K) },
	{ "mx25l8005",   INFO(0xc22014, 0, 64 * 1024,  16, 0) },
	{ "mx25l1606e",  INFO(0xc22015, 0, 64 * 1024,  32, SECT_4K) },
	{ "mx25l3205d",  INFO(0xc22016, 0, 64 * 1024,  64, 0) },
	{ "mx25l6405d",  INFO(0xc22017, 0, 64 * 1024, 128, 0) },
	{ "mx25l12805d", INFO(0xc22018, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },
	{ "mx25l12855e", INFO(0xc22618, 0, 64 * 1024, 256, 0) },
	{ "mx25l25635e", INFO(0xc22019, 0, 64 * 1024, 512, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },
	//{ "mx25l25635e", INFO(0xc22019, 0, 64 * 1024, 512, 0) },
	{ "mx25l25655e", INFO(0xc22619, 0, 64 * 1024, 512, 0) },
	{ "mx66l51235l", INFO(0xc2201a, 0, 64 * 1024, 1024, 0) },

	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{ "w25x10", INFO(0xef3011, 0, 64 * 1024,  2,  SECT_4K) },
	{ "w25x20", INFO(0xef3012, 0, 64 * 1024,  4,  SECT_4K) },
	{ "w25x40", INFO(0xef3013, 0, 64 * 1024,  8,  SECT_4K) },
	{ "w25x80", INFO(0xef3014, 0, 64 * 1024,  16, SECT_4K) },
	{ "w25x16", INFO(0xef3015, 0, 64 * 1024,  32, SECT_4K) },
	{ "w25x32", INFO(0xef3016, 0, 64 * 1024,  64, SECT_4K) },
	{ "w25q32", INFO(0xef4016, 0, 64 * 1024,  64, SECT_4K) },
	{ "w25q32dw", INFO(0xef6016, 0, 64 * 1024,  64, SECT_4K) },
	{ "w25x64", INFO(0xef3017, 0, 64 * 1024, 128, SECT_4K) },
	{ "w25q64", INFO(0xef4017, 0, 64 * 1024, 128, SECT_4K) },
	{ "w25q80", INFO(0xef5014, 0, 64 * 1024,  16, SECT_4K) },
	{ "w25q80bl", INFO(0xef4014, 0, 64 * 1024,  16, SECT_4K) },
	{ "w25q128", INFO(0xef4018, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },
	//{ "w25q128", INFO(0xef4018, 0, 64 * 1024, 256, 0) },
	{ "w25q256", INFO(0xef4019, 0, 64 * 1024, 512, SECT_4K) },

	{ },
};

/*
 *(1)Must use one line.
 * */
int32_t  flash_write_enable(void)
{
	spi_protl_set  spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = OPCODE_WREN ;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_read_write_data( &spi_trans );

	return SPI_SUCCESS;
}

int32_t  flash_write_disable(void)
{
	spi_protl_set  spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = OPCODE_WRDI ;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_read_write_data( &spi_trans );

	return SPI_SUCCESS;
}

int flash_wait_ready(void)
{
	spi_protl_set spi_trans ;
	uint8_t buf[4];

	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = OPCODE_RDSR;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 1;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	while(1)
	{
		memset(buf,0x0,2);
		spi_trans.read_buf = buf;
		spi_read_write_data( &spi_trans );
		//ready
		if (!(buf[0] & SR_WIP))
		{
			flash_debug("ready,status:[0x%x]!\n",buf[0]);
			break;
		}

		flash_debug("flash status:[0x%x]!\n",buf[0]);
		udelay(50);
	}
	return SPI_SUCCESS;
 }

static int flash_read_cmd(uint8_t cmd)
{
	spi_protl_set spi_trans ;
	uint8_t buf[4];

	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = cmd;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 1;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	memset(buf,0x0,2);
	spi_trans.read_buf = buf;
	spi_read_write_data( &spi_trans );
	return buf[0];
}

int32_t  flash_write_cmd(uint8_t cmd)
{
	spi_protl_set spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = cmd ;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_read_write_data( &spi_trans );

	return SPI_SUCCESS;
}

/*
 * Write status register 1 byte
 * Returns negative if error occurred.
 */
static int flash_write_cmd_data(uint8_t cmd,uint8_t val)
{
	spi_protl_set  spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = cmd;
	spi_trans.commands[1] = val;
	spi_trans.cmd_count = 2;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_read_write_data( &spi_trans );

	return SPI_SUCCESS;
}

static int quad_enable_mx() {
	int ret, val;

	val = flash_read_cmd(OPCODE_RDSR);
	if (val < 0) {
		flash_debug("%s: read sr failed!\n", __func__);
		return val;
	}
	else if (val & SR_QUAD_EN_MX) {
		flash_debug("Macronix Quad bit already set!\n");
		return 0;
	}

	while(1) {
		flash_write_enable();
		flash_write_cmd_data(OPCODE_WRSR,val | SR_QUAD_EN_MX);
		flash_wait_ready();

		ret = flash_read_cmd(OPCODE_RDSR);
		if (ret > 0 && (ret & SR_QUAD_EN_MX)) {
			break;
		}
		else {
			flash_debug("flash status:[0x%x]!\n",ret);
			udelay(50);
		}
	}

	return SPI_SUCCESS;
}
static int quad_disable_mx() {
	int ret, val;

	val = flash_read_cmd(OPCODE_RDSR);
	if (val < 0) {
		flash_debug("%s: read sr failed!\n", __func__);
		return val;
	}
	else if (!(val & SR_QUAD_EN_MX)) {
		flash_debug("Macronix Quad bit already clear!\n");
		return 0;
	}

	while(1) {
		flash_write_enable();
		flash_write_cmd_data(OPCODE_WRSR,val & ~(SR_QUAD_EN_MX));
		flash_wait_ready();

		ret = flash_read_cmd(OPCODE_RDSR);
		if (ret > 0 && !(ret & SR_QUAD_EN_MX)) {
			break;
		}
		else {
			flash_debug("flash status:[0x%x]!\n",ret);
			udelay(50);
		}
	}

	return SPI_SUCCESS;
}


static int quad_enable_wb() {
	int ret, val;

	val = flash_read_cmd(OPCODE_RDSR2);
	if (val < 0) {
		flash_debug("%s: read sr2 failed!\n", __func__);
		return val;
	}
	else if (val & SR_QUAD_EN_WB) {
		flash_debug("Winbond Quad bit already set!\n");
		return 0;
	}
	while(1) {
		flash_write_enable();
		flash_write_cmd_data(OPCODE_WRSR2,val | SR_QUAD_EN_WB);
		flash_wait_ready();

		ret = flash_read_cmd(OPCODE_RDSR2);
		if (ret > 0 && (ret & SR_QUAD_EN_WB)) {
			break;
		}
		else {
			flash_debug("flash status:[0x%x]!\n",ret);
			udelay(50);
		}
	}

	return SPI_SUCCESS;
}

static int quad_disable_wb() {
	int ret, val;

	val = flash_read_cmd(OPCODE_RDSR2);
	if (val < 0) {
		flash_debug("%s: read sr2 failed!\n", __func__);
		return val;
	}
	else if (!(val & SR_QUAD_EN_WB)) {
		flash_debug("Winbond Quad bit already clear!\n");
		return 0;
	}
	while(1) {
		flash_write_enable();
		flash_write_cmd_data(OPCODE_WRSR2,val & ~(SR_QUAD_EN_WB));
		flash_wait_ready();

		ret = flash_read_cmd(OPCODE_RDSR2);
		if (ret > 0 && !(ret & SR_QUAD_EN_WB)) {
			break;
		}
		else {
			flash_debug("flash status:[0x%x]!\n",ret);
			udelay(50);
		}
	}

	return SPI_SUCCESS;
}

static int quad_enable_wb_w25q64() {
	int ret, val1, val2;
	spi_protl_set  spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = OPCODE_WRSR;
	spi_trans.commands[1] = 0;
	spi_trans.commands[2] = 0;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;


	val2 = flash_read_cmd(OPCODE_RDSR2);
	if (val2 < 0) {
		flash_debug("%s: read sr2 failed!\n", __func__);
		return val2;
	}
	else if (val2 & SR_QUAD_EN_WB) {
		flash_debug("Winbond Quad bit already set!\n");
		return 0;
	}
	while(1) {

		val1 = flash_read_cmd(OPCODE_RDSR);
		val2 = flash_read_cmd(OPCODE_RDSR2);

		flash_write_enable();
		spi_trans.commands[1] = val1;
		spi_trans.commands[2] = val2 | SR_QUAD_EN_WB;
	    spi_read_write_data( &spi_trans );
		flash_wait_ready();

		ret = flash_read_cmd(OPCODE_RDSR2);
		if (ret > 0 && (ret & SR_QUAD_EN_WB)) {
			break;
		}
		else {
			flash_debug("flash status:[0x%x]!\n",ret);
			udelay(50);
		}
	}

	return SPI_SUCCESS;
}

static int quad_disable_wb_w25q64() {
	int ret, val1, val2;
	spi_protl_set  spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = OPCODE_WRSR;
	spi_trans.commands[1] = 0;
	spi_trans.commands[2] = 0;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;


	val2 = flash_read_cmd(OPCODE_RDSR2);
	if (val2 < 0) {
		flash_debug("%s: read sr2 failed!\n", __func__);
		return val2;
	}
	else if (!(val2 & SR_QUAD_EN_WB)) {
		flash_debug("Winbond Quad bit already clear!\n");
		return 0;
	}
	while(1) {

		val1 = flash_read_cmd(OPCODE_RDSR);
		val2 = flash_read_cmd(OPCODE_RDSR2);

		flash_write_enable();
		spi_trans.commands[1] = val1;
		spi_trans.commands[2] = val2 & ~(SR_QUAD_EN_WB);
	    spi_read_write_data( &spi_trans );
		flash_wait_ready();

		ret = flash_read_cmd(OPCODE_RDSR2);
		if (ret > 0 && !(ret & SR_QUAD_EN_WB)) {
			break;
		}
		else {
			flash_debug("flash status:[0x%x]!\n",ret);
			udelay(50);
		}
	}

	return SPI_SUCCESS;
}
static int protect_disable_w25q64() {
	int val1, val2;
	spi_protl_set  spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = OPCODE_WRSR;
	spi_trans.commands[1] = 0;
	spi_trans.commands[2] = 0;
	spi_trans.cmd_count = 3;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;


	while(1) {

		val1 = flash_read_cmd(OPCODE_RDSR);
		val2 = flash_read_cmd(OPCODE_RDSR2);
		if (((val1 >= 0) && !(val1 & (SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC))) && (((val2) >= 0) && !(val2 & (SR_CMP))) ) {
			flash_debug("Winbond protect bit clear!\n");
			return 0;
		}
		else {
			flash_debug("flash status1:[0x%x]!\n",val1);
			flash_debug("flash status2:[0x%x]!\n",val2);
			udelay(50);
		}

		flash_write_enable();
		spi_trans.commands[1] = (val1 & ~(SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC));
		spi_trans.commands[2] = (val2 & ~(SR_CMP));
	    spi_read_write_data( &spi_trans );
		flash_wait_ready();

	}

	return SPI_SUCCESS;
}

static int protect_disable_wb() {
	int val1, val2;


	while(1) {

		val1 = flash_read_cmd(OPCODE_RDSR);
		val2 = flash_read_cmd(OPCODE_RDSR2);
		if (((val1 >= 0) && !(val1 & (SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC))) && (((val2) >= 0) && !(val2 & (SR_CMP))) ) {
			flash_debug("Winbond protect bit clear!\n");
			return 0;
		}
		else {
			flash_debug("flash status1:[0x%x]!\n",val1);
			flash_debug("flash status2:[0x%x]!\n",val2);
			udelay(50);
		}

		flash_write_enable();
		flash_write_cmd_data(OPCODE_WRSR,(val1 & ~(SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC)));
		flash_write_cmd_data(OPCODE_WRSR2,(val2 & ~(SR_CMP)));
		flash_wait_ready();

	}

	return SPI_SUCCESS;
}

/*
 * Enable/disable 4-byte addressing mode.
 */
static  int set_4byte(int enable)
{
	flash_debug("=====set_4byte====\n");
	switch (JEDEC_MFR(flash_info_data.flash_ic.jedec_id)) {
	case MFR_MACRONIX:
	case 0xEF /* winbond */:
		return flash_write_cmd(enable ? OPCODE_EN4B : OPCODE_EX4B);
	default:
		/* Spansion style */
		return flash_write_cmd_data(OPCODE_BRWR, enable << 7);
	}
}

static void addr2cmd(unsigned int addr, u8 *cmd)
{
	cmd[0] = addr >> (flash_info_data.flash_ic.addr_width * 8 -  8);
	cmd[1] = addr >> (flash_info_data.flash_ic.addr_width * 8 - 16);
	cmd[2] = addr >> (flash_info_data.flash_ic.addr_width * 8 - 24);
	cmd[3] = addr >> (flash_info_data.flash_ic.addr_width * 8 - 32);
}

static void display_data(uint8_t* buf,int buf_len)
{
#if 1
	int i = 0;
	flash_debug("\n");
	flash_debug("==============display data===============\n");
//	for ( i = 0 ; i < buf_len ; i ++ ) {
	for ( i = 0 ; i < 50 ; i ++ ) {
		if( (i%16) == 0 )
			flash_debug("\n");
		flash_debug("0x%x ",buf[i]);
		//flash_debug("%-4d",buf[i]);
	}
	flash_debug("\n");
#endif
}

int flash_vs_align(void)
{
	return 4096;
}

 void spi_set_bytes(int bytes) {
    flash_info_data.flash_ic.page_size = bytes;
    flash_debug("spi: bytes set to: %d\n", bytes);
}

/*
 *(1)Must use one line.
 * */
static const struct spi_device_id * flash_read_id(void) {

	uint8_t	buf[5];
	uint32_t	jedec;
	int	tmp;
	u16  ext_jedec;
	struct flash_info	*info;

	//flash_debug("========Read JEDEC start======\n");
	spi_protl_set       spi_trans;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );

	spi_trans.commands[0] = OPCODE_RDID;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.read_buf = buf;
	spi_trans.read_count = 5;
	spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_read_write_data( &spi_trans );

	jedec = (buf[0] << 16) | (buf[1] << 8) | (buf[2]);
	flash_debug("jedec = 0x%x\n", jedec);
	ext_jedec = buf[3] << 8 | buf[4];

	for (tmp = 0; tmp < ARRAY_SIZE(flash_ids) - 1; tmp++) {
		info = (void *)flash_ids[tmp].driver_data;
		if (info->jedec_id == jedec) {
			if (info->ext_id != 0 && info->ext_id != ext_jedec)
				continue;
			return &flash_ids[tmp];
		}
	}
	printf("unrecognized JEDEC id %06x\n", jedec);
	return NULL;
}

int flash_vs_reset(void)
{
	const struct spi_device_id *jid;
	struct flash_info	*info;

	flash_debug("Flash device reset...!\n");
	/*
	 *(1)Author:summer
	 *(2)Date:2015-05-13
	 *(3)We should write command to flash device,let it reset
	 */

	otf_module_init();
	flash_debug("otf_module_init =ok\n");
	//gpio_switch_func_by_module("ssp0",GPIO_FUNC_MODE_MUX0);

	memset( &flash_info_data, 0x0 ,sizeof(flash_info_data));
	jid = flash_read_id();
	if(jid == NULL){
		printf("flash init error!\n");
		while(1);
	}
	printf("found flash: %s \n",jid->name);

	info = (void *)jid->driver_data;
	flash_info_data.flash_ic.jedec_id = info->jedec_id;
	flash_info_data.flash_ic.ext_id = info->ext_id;
	flash_info_data.flash_ic.sector_size = info->sector_size;
	flash_info_data.flash_ic.n_sectors = info->n_sectors;
	flash_info_data.flash_ic.page_size = info->page_size;
	flash_info_data.flash_ic.addr_width = info->addr_width;
	flash_info_data.flash_ic.flags = info->flags;

	/* prefer "small sector" erase if possible */
	if (info->flags & SECT_4K) {
		flash_info_data.erase_opcode = OPCODE_BE_4K;
		flash_info_data.erasesize = 4096;
	} else {
		flash_info_data.erase_opcode = OPCODE_SE;
		flash_info_data.erasesize = info->sector_size;
	}
	flash_info_data.flash_size = info->sector_size * info->n_sectors;
	flash_info_data.flash_read = FLASH_RX_NORMAL;
	flash_info_data.flash_write = FLASH_TX_NORMAL;

	if (IROM_IDENTITY==IX_CPUID_Q3F || IROM_IDENTITY==IX_CPUID_APOLLO3) {
		if (flash_info_data.flash_ic.flags & SPI_NOR_QUAD_READ) {
			flash_info_data.flash_read = FLASH_RX_QUAD;
		}
		if (flash_info_data.flash_ic.flags & SPI_NOR_QUAD_WRITE) {
			flash_info_data.flash_write = FLASH_TX_QUAD;
		}
	}

	//TODO: complement disable protect function for flash from other manufacturers
	if((JEDEC_MFR(info->jedec_id) == MFR_WINBOND) || (JEDEC_MFR(info->jedec_id) == MFR_GIGADEVICE)) {
		if(flash_info_data.flash_ic.jedec_id) {
			protect_disable_w25q64();
		}
		else {
			protect_disable_wb();
		}
	}

	if(flash_info_data.flash_read== FLASH_RX_QUAD || flash_info_data.flash_write == FLASH_TX_QUAD) {
		printf("flash will use 4-line mode\n");
		if (JEDEC_MFR(flash_info_data.flash_ic.jedec_id) == MFR_MACRONIX) {
			quad_enable_mx();
		}
		else if (JEDEC_MFR(flash_info_data.flash_ic.jedec_id) == MFR_WINBOND) {
			if(flash_info_data.flash_ic.jedec_id == W25Q64) {
				quad_enable_wb_w25q64();
			}
			else {
				quad_enable_wb();
			}
		}
	}
	else {
		printf("flash will use 1-line mode\n");
		if (JEDEC_MFR(info->jedec_id) == MFR_MACRONIX) {
			quad_disable_mx();
		}
		else if (JEDEC_MFR(flash_info_data.flash_ic.jedec_id) == MFR_WINBOND) {
			if(info->jedec_id == W25Q64) {
				quad_disable_wb_w25q64();
			}
			else {
				quad_disable_wb();
			}
		}

	}

	if (info->addr_width)
		flash_info_data.flash_ic.addr_width = info->addr_width;
	else {
		/* enable 4-byte addressing if the device exceeds 16MiB */
		if (flash_info_data.flash_size > FLASH_LARGE_SIZE) {
			printf("flash will use 4 width addr  mode.\n");
			flash_info_data.flash_ic.addr_width = 4;
			set_4byte(1);
		} else
			flash_info_data.flash_ic.addr_width = 3;
	}

	flash_info_data.set4byte = 1;

	flash_info_data.flash_inited = 1;
	return 0;
}

/*
 *(1)offs: erase begining addr, must be 64K aligned
 *(2)len: erase total size, must be 64K aligned
 *(3)extra: reserved
 */
int flash_vs_erase(loff_t offs, uint64_t len)
{
	struct spi_transfer_set	spi_trans ;
	uint32_t	sector_size	  = 0;
	uint32_t	sectors	  = 0;
	uint32_t	i = 0;

	flash_debug("offs=%lld,len=%lld\n", offs, len);
	if (len & (FLASH_ERASE_SIZE - 1)) {
		len += (FLASH_ERASE_SIZE - 1);
		len &= ~(FLASH_ERASE_SIZE - 1);
	}
	if (offs & (FLASH_ERASE_SIZE - 1))
		offs &= ~(FLASH_ERASE_SIZE - 1);
	
	flash_debug("==offs=%lld,len=%lld\n", offs, len);
	if(offs % FLASH_ERASE_SIZE || len % FLASH_ERASE_SIZE) {
		printf("Erase addr and len must be %d aligned!\n", FLASH_ERASE_SIZE);
		return SPI_OPERATE_FAILED;
	}

	if ( flash_info_data.flash_inited != 1) {
		flash_debug("spi manager is not initiated!\n");
		return SPI_OPERATE_FAILED;
	}
	if(flash_info_data.flash_size > FLASH_LARGE_SIZE && flash_info_data.set4byte)
		set_4byte(1);

	if( len >= flash_info_data.flash_size)
	{
		flash_debug("Erase whole chip !\n");

		flash_wait_ready();
		flash_write_enable();
		memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
		spi_trans.commands[0] = OPCODE_CHIP_ERASE;
		spi_trans.cmd_count = 1;
		spi_trans.cmd_type = CMD_ONLY;
		spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
		spi_read_write_data( &spi_trans );

		if(flash_info_data.flash_size > FLASH_LARGE_SIZE && flash_info_data.set4byte)
			set_4byte(0);
		return 0;
	}

	//sector size
	sector_size = flash_info_data.erasesize;
	sectors = ((uint32_t)len) / sector_size;
#if 0
	if(sectors <= 0) {
		sectors = 1;
	}
#endif
	flash_debug("Erase %d from spi, sector_size = %d\n", len, sector_size);

	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	spi_trans.commands[0] = flash_info_data.erase_opcode;
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_ONLY;
	spi_trans.flash_addr = offs;
	spi_trans.addr_count = flash_info_data.flash_ic.addr_width;
	spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
	spi_trans.dummy_count= 0;
	spi_trans.dummy_lines = SSP_MODULE_ONE_WIRE_NEW;

	flash_debug("Sectors:[%d]\n",sectors);
	for ( i = 0 ; i < sectors ; i ++ )
	{
		addr2cmd(spi_trans.flash_addr,spi_trans.addr);

		flash_debug("Erase [%d]bytes from addr:[0x%x] !\n", sector_size, spi_trans.flash_addr);
		flash_debug("Sector:[%d] is handling...!\n",i);

		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
		spi_trans.flash_addr += sector_size;
	}

	if(flash_info_data.flash_size > FLASH_LARGE_SIZE && flash_info_data.set4byte)
		set_4byte(0);
	return 0;
}

/*
 *(1)extra:high 8,addr lines
 *(2)extra:next 8,modules lines
 *(3)offs must be absolute addr.
 *(4)addr = page_id*256
 *(5)If you want to divice one nand block into 8 frame,channel_in_sector is 8
 *   if channel_in_sector is 0,one nand block is one frame
 */
int flash_vs_read(uint8_t *buf, loff_t start, int length, int extra)
{
	//uint8_t	 line_mode = 0;
	//uint8_t	 dma_mode  = 0;
	struct spi_transfer_set	spi_trans ;
	uint32_t	sectors	  = 0;
	uint32_t	rest_in_sector = 0;
	uint32_t	i = 0;
	//int page_offs= 0;
	//uint32_t channel_in_sector = 8;
	uint32_t	channel_in_sector = 0;
	uint32_t	channels = 0;

	uint32_t	channel_size = 0;
	uint32_t	channel_rest = 0;

	uint32_t sector_size = 0;

	if ( flash_info_data.flash_inited != 1) {
		printf("spi manager is not initiated!\n");
		return SPI_OPERATE_FAILED;
	}

	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	switch(flash_info_data.flash_read) {
		/* Four lines */
		case FLASH_RX_QUAD:
			spi_trans.commands[0] = OPCODE_QREAD;
			spi_trans.module_lines = SSP_MODULE_FOUR_WIRE;
			break;

		/* Two lines */
		case FLASH_RX_DUAL:
			spi_trans.commands[0] = OPCODE_DREAD;
			spi_trans.module_lines = SSP_MODULE_TWO_WIRE;
			break;

		/* One lines */
		case FLASH_RX_NORMAL:
			spi_trans.commands[0] = FLASH_CMD_RD_FT;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;

		/* One lines */
		default:
			spi_trans.commands[0] = FLASH_CMD_RD_FT;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;
	}
	sector_size = flash_info_data.flash_ic.sector_size;

	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_READ_DATA;
	spi_trans.dummy_count = 1;
	spi_trans.dummy_lines = SSP_MODULE_ONE_WIRE_NEW;

	spi_trans.read_buf = buf;
	spi_trans.read_count = sector_size;
	spi_trans.flash_addr = start;
	spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;

	if(flash_info_data.flash_size > FLASH_LARGE_SIZE && flash_info_data.set4byte)
		set_4byte(1);

	//sector size
	flash_debug("read %d from spi, FLASH_SECTOR_SIZE = %d\n", length, sector_size);
	sectors = ((uint32_t)length) / sector_size;
	rest_in_sector = ((uint32_t)length) - (sectors * sector_size);

	//if(spi_trans.module_lines == SSP_MODULE_FOUR_WIRE) {
	//		flash_qe_enable();
	//}

	flash_debug("Sectors:[%d], rest_in_sector:[%d]\n",sectors, rest_in_sector);
	for ( i = 0 ; i < sectors ; i ++ )
	{
		addr2cmd(spi_trans.flash_addr,spi_trans.addr);

		spi_trans.addr_count = flash_info_data.flash_ic.addr_width;
		spi_trans.dummy_count= 1;

		flash_debug("Read [%d]bytes from addr:[0x%x] !\n",spi_trans.read_count,spi_trans.flash_addr);
		flash_debug("Sector:[%d] is handling...!\n",i);
		//read buffer
		if( 0 == channel_in_sector ){
			spi_read_write_data(&spi_trans);
			spi_trans.flash_addr += sector_size;
		}else{
			/* To make channel_rest to be a zero value, you must carefully choose the channel_in_sector */
			channel_size = sector_size / channel_in_sector;
			channel_rest = sector_size - (channel_size * channel_in_sector);
			for( channels = 0; channels < channel_in_sector ; channels ++ )
			{
				spi_trans.read_count = channel_size;
				spi_read_write_data(&spi_trans);
				spi_trans.flash_addr += channel_size;
				spi_trans.addr[0] = (1+channels);
			}
			if(!channel_rest) {
				spi_trans.read_count = channel_rest;
				spi_read_write_data(&spi_trans);
				spi_trans.flash_addr += channel_rest;
			}
		}

		spi_trans.read_count = sector_size;
	}
	if ( rest_in_sector )
	{
		uint32_t	rest_in_channel = 0;
		addr2cmd(spi_trans.flash_addr,spi_trans.addr);

		spi_trans.addr_count = flash_info_data.flash_ic.addr_width;
		spi_trans.dummy_count= 1;

		//read buffer
		spi_trans.read_count = rest_in_sector;
		flash_debug("Read [%d]bytes from addr:[0x%x] with [%d] lines!\n",spi_trans.read_count,spi_trans.flash_addr,spi_trans.module_lines);
		if( 0 == channel_in_sector ){
			spi_read_write_data(&spi_trans);
		}else{
			uint32_t	t = 0;
			channel_in_sector = rest_in_sector/channel_size;
			rest_in_channel   = rest_in_sector%channel_size;
			for( t = 0 ; t < channel_in_sector ; t ++ )
			{
				spi_trans.read_count = channel_size;
				spi_read_write_data(&spi_trans);
				spi_trans.flash_addr += channel_size;
				spi_trans.addr[0] = (t+1);
			}
			if( rest_in_channel ){
				spi_trans.read_count = rest_in_channel;
				spi_read_write_data(&spi_trans);
			}
		}
	}
	//flash_qe_disable();
	display_data(buf,length);
	if(flash_info_data.flash_size > FLASH_LARGE_SIZE && flash_info_data.set4byte)
		set_4byte(0);
	return length;
}

/*
 *(1)extra:high 8,addr lines
 *(2)extra:next 8,modules lines
 *(3)offs must be absolute addr.
 *(4)addr = page_id*256
 *(5)If you want to divice one nand block into 8 frame,channel_in_sector is 8
 *   if channel_in_sector is 0,one nand block is one frame
  */
int flash_vs_write(uint8_t *buf, loff_t offs, int len, int extra)
{
	struct spi_transfer_set	spi_trans ;
	uint32_t	page_size	  = 0;
	uint32_t	pages	  = 0;
	uint32_t	rest_in_page = 0;
	uint32_t	i = 0;
	int	page_offs= 0;
	int	length = len;

	if( flash_info_data.flash_inited != 1) 
	{
		printf("spi manager is not initiated!\n");
		return SPI_OPERATE_FAILED;
	}

	page_size = flash_info_data.flash_ic.page_size;
	memset( &spi_trans, 0x0 , sizeof(spi_protl_set) );
	switch(flash_info_data.flash_write) 
	{
		/* Four lines */
		case FLASH_TX_QUAD:
			spi_trans.module_lines = SSP_MODULE_FOUR_WIRE;
			if (JEDEC_MFR(flash_info_data.flash_ic.jedec_id) == MFR_MACRONIX)  {
				spi_trans.commands[0] = FLASH_CMD_4PP;
				spi_trans.addr_lines = SSP_MODULE_FOUR_WIRE;
			}
			else if (JEDEC_MFR(flash_info_data.flash_ic.jedec_id) == MFR_WINBOND) {
				spi_trans.commands[0] = FLASH_CMD_PP_QUAD;
				spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
			}
			else {
				printf("unknown flash chip\n");
				return SPI_OPERATE_FAILED;
			}
			break;

		/* One lines */
		case FLASH_TX_NORMAL:
			spi_trans.commands[0] = FLASH_CMD_PP;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;

		/* One lines */
		default:
			spi_trans.commands[0] = FLASH_CMD_PP;
			spi_trans.module_lines = SSP_MODULE_ONE_WIRE_NEW;
			spi_trans.addr_lines = SSP_MODULE_ONE_WIRE_NEW;
			break;
	}
	spi_trans.cmd_count = 1;
	spi_trans.cmd_type = CMD_WRITE_DATA;
	spi_trans.addr_count = flash_info_data.flash_ic.addr_width;
	spi_trans.dummy_count = 0;
	spi_trans.flash_addr = offs ;
	spi_trans.write_buf = buf;

	flash_debug("write %d to spi, page_size = %d\n", length, page_size);

	//if(spi_trans.module_lines == SSP_MODULE_FOUR_WIRE) {
	//	flash_qe_enable();
	//}
	if(flash_info_data.flash_size > FLASH_LARGE_SIZE && flash_info_data.set4byte)
		set_4byte(1);

	page_offs = offs & (page_size - 1);
	if(page_offs) {

		spi_trans.write_count = length < page_size ? length : page_size - page_offs;
		flash_debug("page_offs = %d, write_count = %d\n", page_offs, spi_trans.write_count);
		addr2cmd(spi_trans.flash_addr,spi_trans.addr);

		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
		length -= spi_trans.write_count;
		spi_trans.flash_addr += spi_trans.write_count;
	}
	//page size
	pages = ((uint32_t)length) / page_size;
	rest_in_page = ((uint32_t)length) - (pages * page_size);

	flash_debug("Pages:[%d], rest_in_page:[%d]\n",pages, rest_in_page);
	for ( i = 0 ; i < pages ; i ++ )
	{
		spi_trans.write_count = page_size;
		addr2cmd(spi_trans.flash_addr,spi_trans.addr);

		flash_debug("Write [%d]bytes from addr[0x%x] to addr:[0x%x] !\n",spi_trans.write_count, spi_trans.write_buf, spi_trans.flash_addr);
		flash_debug("Page:[%d] is handling...!\n",i);
		//read buffer
		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
		spi_trans.flash_addr += page_size;
	}
	if ( rest_in_page )
	{
		spi_trans.write_count = rest_in_page;
		addr2cmd(spi_trans.flash_addr,spi_trans.addr);

		//write buffer
		flash_debug("Write [%d]bytes from addr[0x%x] to addr:[0x%x] !\n",spi_trans.write_count, spi_trans.write_buf, spi_trans.flash_addr);

		flash_wait_ready();
		flash_write_enable();
		spi_read_write_data(&spi_trans);
	}
	//flash_qe_disable();
	display_data(buf,len);

	if(flash_info_data.flash_size > FLASH_LARGE_SIZE && flash_info_data.set4byte)
		set_4byte(0);
	return len;
}
