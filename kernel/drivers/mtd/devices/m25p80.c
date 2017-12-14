/*
 * MTD SPI driver for ST M25Pxx (and similar) serial flash chips
 *
 * Author: Mike Lavender, mike@steroidmicros.com
 *
 * Copyright (c) 2005, Intec Automation Inc.
 *
 * Some parts are based on lart.c by Abraham Van Der Merwe
 *
 * Cleaned up and generalized based on mtd_dataflash.c
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/math64.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mod_devicetable.h>

#include <linux/mtd/cfi.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/of_platform.h>

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <mach/items.h>

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#endif

#define MTD_CMDLINE_NAME	"nor_flash"
/* Flash opcodes. */
#define	OPCODE_WREN		0x06	/* Write enable */
#define	OPCODE_RDSR		0x05	/* Read status register */
#define	OPCODE_WRSR		0x01	/* Write status register 1 byte */
#define	OPCODE_NORM_READ	0x03	/* Read data bytes (low frequency) */
#define	OPCODE_FAST_READ	0x0b	/* Read data bytes (high frequency) */
#define OPCODE_DREAD		0x3b
#define OPCODE_QREAD		0x6b
#define	OPCODE_PP		0x02	/* Page program (up to 256 bytes) */
#define	OPCODE_QPP			0x32	/* Quad Page program (up to 256 bytes) */
#define	OPCODE_4PP			0x38	/* 4 x IO Page program (up to 256 bytes) */
#define	OPCODE_BE_4K		0x20	/* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52	/* Erase 32KiB block */
#define	OPCODE_CHIP_ERASE	0xc7	/* Erase whole flash chip */
#define	OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_RDID		0x9f	/* Read JEDEC ID */
#define OPCODE_EN_RESET     0x66
#define OPCODE_RESET_CHIP   0x99

/* Used for Winbond & Gigadevice flashes only. */
#define	OPCODE_RDSR2	0x35	/* Read status register2 */
#define	OPCODE_WRSR2	0x31	/* Write status register2 */

/* Used for SST flashes only. */
#define	OPCODE_BP		0x02	/* Byte program */
#define	OPCODE_WRDI		0x04	/* Write disable */
#define	OPCODE_AAI_WP		0xad	/* Auto address increment word program */

/* Used for SST flashes SST25VF010A only. */
#define OPCODE_EWSR            0x50    /* Enable write status register */
#define OPCODE_READ_ID         0x90    /* Read ID */
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
#define	SR_SRWD			0x80	/* SR write protect */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_JIFFIES	(40 * HZ)	/* M25P16 specs 40s max chip erase */
#define	MAX_CMD_SIZE		5
/* meaning of SR2 bits may differ by vendors */
#define SR_QUAD_EN_MX           BIT(6)
#define SR_QUAD_EN_WB           BIT(1)
#define	SR_CMP					0x40	/* Complement protect */

#define JEDEC_W25Q64			(0xef4017)

#define JEDEC_MFR(_jedec_id)	((_jedec_id) >> 16)

/****************************************************************************/

enum transfer_type {
	CMD = 0,
	ADDR,
	DUMMY,
	DATA,
};

enum read_mode {
	M25P80_RX_NORMAL = 0,
	M25P80_RX_FAST,
	M25P80_RX_DUAL,
	M25P80_RX_QUAD,
};

enum write_mode {
	M25P80_TX_NORMAL = 0,
	M25P80_TX_DUAL,
	M25P80_TX_QUAD,
};

struct m25p {
	struct spi_device	*spi;
	struct mutex		lock;
	struct mtd_info		mtd;
	u16			page_size;
	u16			addr_width;
	u8			erase_opcode;
	u8			*command;
	bool			fast_read;
	enum read_mode		flash_read;
	enum write_mode		flash_write;
#ifdef CONFIG_DEBUG_FS
	struct dentry                   *debugfs;
#endif
	int			set4byte;
	u32 	    g_jedec_id;
};

/*
 * Read an address range from the flash chip.  The address range
 * may be any size provided it is within the physical boundaries.
 */
static int sub_m25p80_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf);

static int m25p80_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf);

static int m25p80_erase(struct mtd_info *mtd, struct erase_info *instr);

#ifdef CONFIG_DEBUG_FS
#define Flash_PAGE 4096
static ssize_t flash_wr_byte(struct file *file, const char __user *user_buf,
		size_t count, loff_t *ppos) {
	struct mtd_info * mtd = file->private_data;
	size_t wretlen, rretlen;
	size_t buf_size;
	u_char *wbuf, *rbuf;
	struct erase_info *instr;
	long long phy_off;
	char buf[16];
	int i, k;
	bool flag = false;

	instr = kmalloc(sizeof(struct erase_info), GFP_KERNEL);
	rbuf = kmalloc(Flash_PAGE, GFP_KERNEL); 
	wbuf = kmalloc(Flash_PAGE, GFP_KERNEL);
	memset(wbuf, 0, Flash_PAGE);
	for(i = 0; i < Flash_PAGE; i++)
		wbuf[i] = i % 256;

	buf_size = min(count, (sizeof(buf)-1));
	if (copy_from_user(buf, user_buf, buf_size))
		return -EFAULT;
	phy_off = simple_strtoul(buf, NULL, 16);
	instr->addr = phy_off;

	/* flash read test */
	m25p80_write(mtd, phy_off, Flash_PAGE, &wretlen, wbuf);
	for(i = Flash_PAGE; i <= Flash_PAGE; i++){
		memset(rbuf, 0, Flash_PAGE);
		sub_m25p80_read(mtd, phy_off, i, &rretlen, rbuf);
		for(k = 0; k < i; k++){
			if(wbuf[k] != rbuf[k]){
				print_hex_dump(KERN_ERR, "R TEST: ",
						DUMP_PREFIX_OFFSET,
						16,
						1,
						rbuf,
						i,
						1);
				flag = true;
				break;
			}
		}
		if(!flag)
			printk("%d byte r SUCCESS\n",i);
		printk("\n");
	}
	wretlen = 0;
	rretlen = 0;
	flag = false;

	/* flash write test */
	for(i = Flash_PAGE; i <= Flash_PAGE; i++){
		memset(rbuf, 0, Flash_PAGE);
		instr->len = mtd->erasesize;
		m25p80_erase(mtd, instr);
		m25p80_write(mtd, phy_off, i, &wretlen, wbuf);
		sub_m25p80_read(mtd, phy_off, Flash_PAGE, &rretlen, rbuf);
		for(k = 0; k < i; k++){
			if(wbuf[k] != rbuf[k]){
				print_hex_dump(KERN_ERR, "W TEST: ",
						DUMP_PREFIX_OFFSET,
						16,
						1,
						rbuf,
						i,
						1);
				flag = true;
				break;
			}
		}
		if(!flag)
			printk("%d byte w SUCCESS\n",i);
		printk("\n");
	}
	return count; 
}

static const struct file_operations flash_wr_ops = {
	.owner          = THIS_MODULE,
	.open           = simple_open,
	.write          = flash_wr_byte,
	.llseek         = default_llseek,
};

static int flash_debugfs_init(struct mtd_info * mtd){
	 struct dentry   *debugfs;
	 debugfs = debugfs_create_dir("flash-wr",NULL);
	 if(!debugfs)
		 return -ENOMEM;
	 debugfs_create_file("wrbyte", S_IFREG | S_IRUGO | S_IWUGO,
			 debugfs, mtd, &flash_wr_ops);
	 return 0;
}
#endif

static inline struct m25p *mtd_to_m25p(struct mtd_info *mtd)
{
	return container_of(mtd, struct m25p, mtd);
}

static inline unsigned int m25p80_rx_nbits(struct m25p *flash) {
	switch (flash->flash_read) {
		case M25P80_RX_QUAD:
			return 4;
		case M25P80_RX_DUAL:
			return 2;
		default:
			return 1;
	}
}

static inline unsigned int m25p80_tx_nbits(struct m25p *flash) {
	switch (flash->flash_write) {
		case M25P80_TX_QUAD:
			return 4;
		case M25P80_TX_DUAL:
			return 2;
		default:
			return 1;
	}
};

/****************************************************************************/

/*
 * Internal helper functions
 */

/*
 * Read the status register, returning its value in the location
 * Return the status register value.
 * Returns negative if error occurred.
 */
static int read_sr(struct m25p *flash)
{
	ssize_t retval;
	u8 code = OPCODE_RDSR;
	u8 val;


	retval = spi_write_then_read(flash->spi, &code, 1, &val, 1);

	if (retval < 0) {
		dev_err(&flash->spi->dev, "error %d reading SR\n",
				(int) retval);
		return retval;
	}

	return val;
}

/*
 * Read the status register 2, returning its value in the location
 * Return the status register 2 value.
 * Returns negative if error occurred.
 */
static int read_sr2_wb(struct m25p *flash)
{
	ssize_t retval;
	u8 code = OPCODE_RDSR2;
	u8 val;

	retval = spi_write_then_read(flash->spi, &code, 1, &val, 1);

	if (retval < 0) {
		dev_err(&flash->spi->dev, "error %d reading SR2\n",
				(int) retval);
		return retval;
	}

	return val;
}

/*
 * Write status register 1 byte
 * Returns negative if error occurred.
 */
static int write_sr(struct m25p *flash, u8 val)
{
	struct spi_message m;
	struct spi_transfer t[2];

	flash->command[0] = OPCODE_WRSR;
	flash->command[1] = val;

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[0].tx_buf = &flash->command[0];
	t[0].len = 1;
	t[0].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = &flash->command[1];
	t[1].len = 1;
	t[1].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[1], &m);

	return spi_sync(flash->spi, &m);
}

/*
 * Write status register 2 byte
 * Returns negative if error occurred.
 */
static int write_sr_2bytes(struct m25p *flash, u8 val1, u8 val2)
{
	struct spi_message m;
	struct spi_transfer t[2];


	flash->command[0] = OPCODE_WRSR;
	flash->command[1] = val1;
	flash->command[2] = val2;

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[0].tx_buf = &flash->command[0];
	t[0].len = 1;
	t[0].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = &flash->command[1];
	t[1].len = 2;
	t[1].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[1], &m);

	return spi_sync(flash->spi, &m);
}

/*
 * Write status register2 1 byte
 * Returns negative if error occurred.
 */
static int write_sr2_wb(struct m25p *flash, u8 val)
{
	struct spi_message m;
	struct spi_transfer t[2];

	flash->command[0] = OPCODE_WRSR2;
	flash->command[1] = val;

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[0].tx_buf = &flash->command[0];
	t[0].len = 1;
	t[0].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = &flash->command[1];
	t[1].len = 1;
	t[1].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[1], &m);

	return spi_sync(flash->spi, &m);
}

/*
 * Set write enable latch with Write Enable command.
 * Returns negative if error occurred.
 */
static inline int write_enable(struct m25p *flash)
{
	u8	code = OPCODE_WREN;
	unsigned long deadline;
	int sr;

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
	    spi_write_then_read(flash->spi, &code, 1, NULL, 0);

		if ((sr = read_sr(flash)) < 0) {
			pr_err("%s: read sr failed!\n", __func__);
			return 1;
		}
		else if ((sr & SR_WEL)) {
			return 0;
		}

		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return 1;
}

/*
 * Send write disble instruction to the chip.
 */
static inline int write_disable(struct m25p *flash)
{
	u8	code = OPCODE_WRDI;

	return spi_write_then_read(flash->spi, &code, 1, NULL, 0);
}

/*
 * Enable/disable 4-byte addressing mode.
 */
static inline int set_4byte(struct m25p *flash, u32 jedec_id, int enable)
{
	switch (JEDEC_MFR(jedec_id)) {
	case CFI_MFR_MACRONIX:
	case 0xEF /* winbond */:
		flash->command[0] = enable ? OPCODE_EN4B : OPCODE_EX4B;
		return spi_write(flash->spi, flash->command, 1);
	default:
		/* Spansion style */
		flash->command[0] = OPCODE_BRWR;
		flash->command[1] = enable << 7;
		return spi_write(flash->spi, flash->command, 2);
	}
}

/*
 * Service routine to read status register until ready, or timeout occurs.
 * Returns non-zero if error.
 */
static int wait_till_ready(struct m25p *flash)
{
	unsigned long deadline;
	int sr;

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		if ((sr = read_sr(flash)) < 0)
			return sr;
		else if (!(sr & SR_WIP))
			return 0;

		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout", __func__);
	return -EIO;
}

static int quad_enable_mx(struct m25p *flash) {

	int val;
	unsigned long deadline;

	val = read_sr(flash);
	if (val < 0) {
		pr_err("%s: read sr failed!\n", __func__);
		return val;
	}
	else if (val & SR_QUAD_EN_MX) {
		pr_err("Macronix Quad bit already set!\n");
		return 0;
	}

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		write_enable(flash);
		write_sr(flash, val | SR_QUAD_EN_MX);
		if (wait_till_ready(flash))
			return -EBUSY;

		val = read_sr(flash);
		if (val >= 0 && (val & SR_QUAD_EN_MX)) {
			return 0;
		}
		else {
			pr_err("Macronix Quad bit not set, try again\n");
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}

static int quad_disable_mx(struct m25p *flash) {

	int val;
	unsigned long deadline;

	val = read_sr(flash);
	if (val < 0) {
		pr_err("%s: read sr failed!\n", __func__);
		return val;
	}
	else if (!(val & SR_QUAD_EN_MX)) {
		pr_err("Macronix Quad bit already cleared!, val = 0x%x(%d)\n", val, val);
		return 0;
	}

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		write_enable(flash);
		write_sr(flash, val & (~(SR_QUAD_EN_MX)));
		if (wait_till_ready(flash))
			return -EBUSY;

		val = read_sr(flash);
		if (val >= 0 && !(val & SR_QUAD_EN_MX)) {
			return 0;
		}
		else {
			pr_err("Macronix Quad bit is not cleared, try again, val = 0x%x(%d)\n", val, val);
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}

static int quad_enable_wb(struct m25p *flash) {
	int val;
	unsigned long deadline;

	val = read_sr2_wb(flash);
	if (val < 0) {
		pr_err("%s: read sr2 failed!\n", __func__);
		return val;
	}
	else if (val & SR_QUAD_EN_WB) {
		pr_err("Winbond Quad bit already set!\n");
		return 0;
	}

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		write_enable(flash);
		write_sr2_wb(flash, val | SR_QUAD_EN_WB);
		if (wait_till_ready(flash))
			return -EBUSY;

		val = read_sr2_wb(flash);
		if (val >= 0 && (val & SR_QUAD_EN_WB)) {
			return 0;
		}
		else {
			pr_err("Winbond Quad bit is not set, try again\n");
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}

static int quad_disable_wb(struct m25p *flash) {
	int val;
	unsigned long deadline;

	val = read_sr2_wb(flash);
	if (val < 0) {
		pr_err("%s: read sr2 failed!\n", __func__);
		return val;
	}
	else if (!(val & SR_QUAD_EN_WB)) {
		pr_err("Winbond Quad bit already cleared!\n");
		return 0;
	}

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		write_enable(flash);
		write_sr2_wb(flash, val & (~(SR_QUAD_EN_WB)));
		if (wait_till_ready(flash))
			return -EBUSY;

		val = read_sr2_wb(flash);
		if (val >= 0 && !(val & SR_QUAD_EN_WB)) {
			return 0;
		}
		else {
			pr_err("Winbond Quad bit is not cleared, try again\n");
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}

static int quad_enable_wb_w25q64(struct m25p *flash) {
	int val1, val2;
	unsigned long deadline;

	val2 = read_sr2_wb(flash);
	if (val2 < 0) {
		pr_err("%s: read sr2 failed!\n", __func__);
		return val2;
	}
	else if (val2 & SR_QUAD_EN_WB) {
		pr_err("Winbond Quad bit already set!\n");
		return 0;
	}

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		val1 = read_sr(flash);
		val2 = read_sr2_wb(flash);

		write_enable(flash);
		write_sr_2bytes(flash, val1, (val2 | SR_QUAD_EN_WB));
		if (wait_till_ready(flash))
			return -EBUSY;

		val2 = read_sr2_wb(flash);
		if (val2 >= 0 && (val2 & SR_QUAD_EN_WB)) {
			return 0;
		}
		else {
			pr_err("Winbond Quad bit not set, try again\n");
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}

static int quad_disable_wb_w25q64(struct m25p *flash) {
	int val1, val2;
	unsigned long deadline;

	val2 = read_sr2_wb(flash);
	if (val2 < 0) {
		pr_err("%s: read sr2 failed!\n", __func__);
		return val2;
	}
	else if (!(val2 & SR_QUAD_EN_WB)) {
		pr_err("Winbond Quad bit already clear!\n");
		return 0;
	}

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {
		val1 = read_sr(flash);
		val2 = read_sr2_wb(flash);

		write_enable(flash);
		write_sr_2bytes(flash, val1, (val2 & (~(SR_QUAD_EN_WB))));
		if (wait_till_ready(flash))
			return -EBUSY;

		val2 = read_sr2_wb(flash);
		if (val2 >= 0 && !(val2 & SR_QUAD_EN_WB)) {
			return 0;
		}
		else {
			pr_err("Winbond Quad bit is not clear, try again\n");
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}

static int protect_disable_w25q64(struct m25p *flash) {
	int val1, val2;
	unsigned long deadline;

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {

		val1 = read_sr(flash);
		if (val1 < 0) {
			pr_err("%s: read sr1 failed!\n", __func__);
			return val1;
		}

		val2 = read_sr2_wb(flash);
		if (val2 < 0) {
			pr_err("%s: read sr2 failed!\n", __func__);
			return val2;
		}

		write_enable(flash);
		write_sr_2bytes(flash, (val1 & ~(SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC)), (val2 & ~(SR_CMP)));
		if (wait_till_ready(flash))
			return -EBUSY;

		val1 = read_sr(flash);
		val2 = read_sr2_wb(flash);
		if (((val1 >= 0) && !(val1 & (SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC))) && (((val2) >= 0) && !(val2 & (SR_CMP))) ) {
			return 0;
		}
		else {
			pr_err("SR protect bit still set, try again\n");
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}

static int protect_disable_wb(struct m25p *flash) {
	int val1, val2;
	unsigned long deadline;

	deadline = jiffies + MAX_READY_WAIT_JIFFIES;

	do {

		val1 = read_sr(flash);
		if (val1 < 0) {
			pr_err("%s: read sr1 failed!\n", __func__);
			return val1;
		}

		write_enable(flash);
		write_sr(flash, (val1 & ~(SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC)));
		if (wait_till_ready(flash))
			return -EBUSY;

		val2 = read_sr2_wb(flash);
		if (val2 < 0) {
			pr_err("%s: read sr2 failed!\n", __func__);
			return val2;
		}

		write_enable(flash);
		write_sr2_wb(flash, (val2 & ~(SR_CMP)));
		if (wait_till_ready(flash))
			return -EBUSY;

		val1 = read_sr(flash);
		val2 = read_sr2_wb(flash);
		if (((val1 >= 0) && !(val1 & (SR_BP0 | SR_BP1 | SR_BP2 | SR_TB | SR_SEC))) && (((val2) >= 0) && !(val2 & (SR_CMP))) ) {
			return 0;
		}
		else {
			pr_err("SR protect bit still set, try again\n");
		}
relax:
		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	pr_err("%s: timeout\n", __func__);
	return -EIO;
}


/*
 * Erase the whole flash memory
 *
 * Returns 0 if successful, non-zero otherwise.
 */
static int erase_chip(struct m25p *flash)
{
	pr_debug("%s: %s %lldKiB\n", dev_name(&flash->spi->dev), __func__,
			(long long)(flash->mtd.size >> 10));

	/* Wait until finished previous write command. */
	if (wait_till_ready(flash))
		return 1;

	/* Send write enable, then erase commands. */
	write_enable(flash);

	/* Set up command buffer. */
	flash->command[0] = OPCODE_CHIP_ERASE;

	spi_write(flash->spi, flash->command, 1);

	return 0;
}

static int reset_chip(struct m25p *flash)
{
    flash->command[0] = OPCODE_EN_RESET;
    spi_write(flash->spi, flash->command, 1);

    flash->command[0] = OPCODE_RESET_CHIP;
    spi_write(flash->spi, flash->command, 1);
    return 0;
}

static void m25p_addr2cmd(struct m25p *flash, unsigned int addr, u8 *cmd)
{
	/* opcode is in cmd[0] */
	cmd[1] = addr >> (flash->addr_width * 8 -  8);
	cmd[2] = addr >> (flash->addr_width * 8 - 16);
	cmd[3] = addr >> (flash->addr_width * 8 - 24);
	cmd[4] = addr >> (flash->addr_width * 8 - 32);
}

static int m25p_cmdsz(struct m25p *flash)
{
	return 1 + flash->addr_width;
}

/*
 * Erase one sector of flash memory at offset ``offset'' which is any
 * address within the sector which should be erased.
 *
 * Returns 0 if successful, non-zero otherwise.
 */
static int erase_sector(struct m25p *flash, u32 offset)
{
	struct spi_transfer t[2];
	struct spi_message m;

	pr_debug("%s: %s %dKiB at 0x%08x\n", dev_name(&flash->spi->dev),
			__func__, flash->mtd.erasesize / 1024, offset);

	/* Wait until finished previous write command. */
	if (wait_till_ready(flash))
		return 1;

	/* Send write enable, then erase commands. */
	write_enable(flash);

	/* Set up command buffer. */
	flash->command[0] = flash->erase_opcode;
	m25p_addr2cmd(flash, offset, flash->command);

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[0].tx_buf = &flash->command[0];
	t[0].len = 1;
	t[0].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = &flash->command[1];
	t[1].len = flash->addr_width;
	t[1].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[1], &m);

	spi_sync(flash->spi, &m);

	return 0;
}

/****************************************************************************/

/*
 * MTD implementation
 */
/*
 * Erase an address range on the flash chip.  The address range may extend
 * one or more erase sectors.  Return an error is there is a problem erasing.
 */
static int m25p80_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	u32 addr,len;
	uint32_t rem;


	pr_debug("%s: %s at 0x%llx, len %lld\n", dev_name(&flash->spi->dev),
			__func__, (long long)instr->addr,
			(long long)instr->len);

	div_u64_rem(instr->len, mtd->erasesize, &rem);
	if (rem)
		return -EINVAL;

	addr = instr->addr;
	len = instr->len;

	mutex_lock(&flash->lock);

	if(mtd->size > 0x1000000 && flash->set4byte)
		set_4byte(flash, flash->g_jedec_id, 1);

	/* whole-chip erase? */
	if (len == flash->mtd.size) {
		if (erase_chip(flash)) {
			instr->state = MTD_ERASE_FAILED;
			if(mtd->size > 0x1000000 && flash->set4byte)
				set_4byte(flash, flash->g_jedec_id, 0);
			mutex_unlock(&flash->lock);
			return -EIO;
		}

	/* REVISIT in some cases we could speed up erasing large regions
	 * by using OPCODE_SE instead of OPCODE_BE_4K.  We may have set up
	 * to use "small sector erase", but that's not always optimal.
	 */

	/* "sector"-at-a-time erase */
	} else {
		while (len) {
			if (erase_sector(flash, addr)) {
				instr->state = MTD_ERASE_FAILED;
				if(mtd->size > 0x1000000 && flash->set4byte)
					set_4byte(flash, flash->g_jedec_id, 0);
				mutex_unlock(&flash->lock);
				return -EIO;
			}

			addr += mtd->erasesize;
			len -= mtd->erasesize;
		}
	}

	if(mtd->size > 0x1000000 && flash->set4byte)
		set_4byte(flash, flash->g_jedec_id, 0);
	mutex_unlock(&flash->lock);

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);
	return 0;
}

#include "sc.c"
static struct simple_cache ssr_sc;
static long long __tall = 0, __dall = 0, __p = 1000;

static int m25p80_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	int least = len;
	int baklength, hit = 0;
	size_t ret;
	struct simple_cache *sc = &ssr_sc;
	long long t = __getns(), td, tt;
	
//	goto cannot_use_cache;
	if (sc_hit(sc, from, len)) { hit = 1;}
	else if (sc_fit(sc, from, len)) {
		sub_m25p80_read(mtd, sc_set(sc, from, len),
				  sc->size, &ret, sc->data);
	} else goto cannot_use_cache;

	*retlen = sc_get(sc, buf, from, len);
	goto read_done;

cannot_use_cache:

	*retlen = 0;	
	while (least > 0){
		if (least > PAGE_SIZE)
			baklength = PAGE_SIZE;
		else
			baklength = least;
		
		sub_m25p80_read(mtd, from + (len - least), baklength, &ret, buf + (len - least));

		least = least - baklength;
		*retlen += ret;
	}

read_done:
#if 0
	printk(KERN_ERR "sc: %d from %llx (%s)\n", len, from,
		hit == 2? "reset": hit == 1? "hit": "nocache");
#endif
	__dall += len;
	__tall += __getns() - t;
	if((__dall >> 10) > __p) {
		tt = __tall;
		td = __dall;
		do_div(tt, 1000000);
		do_div(td, tt);
		printk(KERN_ERR "sc::%lld KB data, %lld KB/s avg\n",
		 __dall >> 10, td );
		__p += 512;
	}

	return 0;
}

static int sub_m25p80_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	struct spi_transfer t[4];
	struct spi_message m;
	uint8_t opcode;

	pr_debug("%s: %s from 0x%08x, len %zd\n", dev_name(&flash->spi->dev),
			__func__, (u32)from, len);

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	/* NOTE:
	 * OPCODE_FAST_READ (if available) is faster.
	 * Should add 1 byte DUMMY_BYTE.
	 */
	t[CMD].tx_buf = &flash->command[0];
	t[CMD].len = 1;
	t[CMD].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[CMD], &m);

	t[ADDR].tx_buf = &flash->command[1];
	t[ADDR].len = flash->addr_width;
	t[ADDR].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[ADDR], &m);

	if(flash->fast_read) {
		t[DUMMY].tx_buf = &flash->command[m25p_cmdsz(flash)];
		t[DUMMY].len = 1;
		t[DUMMY].tx_nbits = SPI_NBITS_SINGLE;
		spi_message_add_tail(&t[DUMMY], &m);
	}

	t[DATA].rx_buf = buf;
	t[DATA].len = len;
	t[DATA].rx_nbits = m25p80_rx_nbits(flash);
	spi_message_add_tail(&t[DATA], &m);

	mutex_lock(&flash->lock);
	if(mtd->size > 0x1000000 && flash->set4byte)
		set_4byte(flash, flash->g_jedec_id, 1);

	/* Wait till previous write/erase is done. */
	if (wait_till_ready(flash)) {
		/* REVISIT status return?? */
		if(mtd->size > 0x1000000 && flash->set4byte)
			set_4byte(flash, flash->g_jedec_id, 0);
		mutex_unlock(&flash->lock);
		return 1;
	}

	/* FIXME switch to OPCODE_FAST_READ.  It's required for higher
	 * clocks; and at this writing, every chip this driver handles
	 * supports that opcode.
	 */

	/* Set up the write data buffer. */
	switch (flash->flash_read) {
		case M25P80_RX_NORMAL:
			opcode = OPCODE_NORM_READ;
			break;
		case M25P80_RX_FAST:
			opcode = OPCODE_FAST_READ;
			break;
		case M25P80_RX_DUAL:
			opcode = OPCODE_DREAD;
			break;
		case M25P80_RX_QUAD:
			opcode = OPCODE_QREAD;
			break;
		default:
			opcode = OPCODE_NORM_READ;
			break;
	}

	flash->command[0] = opcode;
	m25p_addr2cmd(flash, from, flash->command);

	spi_sync(flash->spi, &m);

	*retlen = m.actual_length - m25p_cmdsz(flash) -
			(flash->fast_read ? 1 : 0);

	if(mtd->size > 0x1000000 && flash->set4byte)
		set_4byte(flash, flash->g_jedec_id, 0);
	mutex_unlock(&flash->lock);
	return 0;
}

/*
 * Write an address range to the flash chip.  Data must be written in
 * FLASH_PAGESIZE chunks.  The address range may be any size provided
 * it is within the physical boundaries.
 */
static int m25p80_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	u32 page_offset, page_size;
	struct spi_transfer t[4];
	struct spi_message m;
	uint8_t opcode;
	
	pr_debug("%s: %s to 0x%08x, len %zd\n", dev_name(&flash->spi->dev),
			__func__, (u32)to, len);

	if(ssr_sc.valid && sc_overlap(&ssr_sc, to, len)) {
		sc_inv(&ssr_sc);
		printk(KERN_ERR "sc: set invalid\n");
	}


	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[CMD].tx_buf = &flash->command[0];
	t[CMD].len = 1;
	t[CMD].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[CMD], &m);

	t[ADDR].tx_buf = &flash->command[1];
	t[ADDR].len = flash->addr_width;
	t[ADDR].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[ADDR], &m);

	t[DATA].tx_buf = buf;
	t[DATA].len = len;
	t[DATA].tx_nbits = m25p80_rx_nbits(flash);
	spi_message_add_tail(&t[DATA], &m);


	mutex_lock(&flash->lock);
	if(mtd->size > 0x1000000 && flash->set4byte)
		set_4byte(flash, flash->g_jedec_id, 1);

	/* Wait until finished previous write command. */
	if (wait_till_ready(flash)) {
		if(mtd->size > 0x1000000 && flash->set4byte)
			set_4byte(flash, flash->g_jedec_id, 0);
		mutex_unlock(&flash->lock);
		return 1;
	}

	write_enable(flash);

	/* Set up the opcode in the write buffer. */
	switch (flash->flash_write) {
		case M25P80_TX_NORMAL:
			opcode = OPCODE_PP;
			break;
		case M25P80_TX_QUAD:
			if(JEDEC_MFR(flash->g_jedec_id) == CFI_MFR_WINBOND || JEDEC_MFR(flash->g_jedec_id) == CFI_MFR_XT || JEDEC_MFR(flash->g_jedec_id) == CFI_MFR_PN || JEDEC_MFR(flash->g_jedec_id) == CFI_MFR_GIGADEVICE) {
				opcode = OPCODE_QPP;
			}
			else if(JEDEC_MFR(flash->g_jedec_id) == CFI_MFR_MACRONIX) {
				opcode = OPCODE_4PP;
				t[ADDR].tx_nbits = SPI_NBITS_QUAD;
			}
			break;
		default:
			opcode = OPCODE_PP;
			break;
	}
	flash->command[0] = opcode;
	m25p_addr2cmd(flash, to, flash->command);

	page_offset = to & (flash->page_size - 1);

	/* do all the bytes fit onto one page? */
	if (page_offset + len <= flash->page_size) {
		t[DATA].len = len;

		spi_sync(flash->spi, &m);

		*retlen = m.actual_length - m25p_cmdsz(flash);
	} else {
		u32 i;

		/* the size of data remaining on the first page */
		page_size = flash->page_size - page_offset;

		t[DATA].len = page_size;
		spi_sync(flash->spi, &m);

		*retlen = m.actual_length - m25p_cmdsz(flash);

		/* write everything in flash->page_size chunks */
		for (i = page_size; i < len; i += page_size) {
			page_size = len - i;
			if (page_size > flash->page_size)
				page_size = flash->page_size;

			/* write the next page to flash */
			m25p_addr2cmd(flash, to + i, flash->command);

			t[DATA].tx_buf = buf + i;
			t[DATA].len = page_size;

			wait_till_ready(flash);

			write_enable(flash);

			spi_sync(flash->spi, &m);

			*retlen += m.actual_length - m25p_cmdsz(flash);
		}
	}

	if(mtd->size > 0x1000000 && flash->set4byte)
		set_4byte(flash, flash->g_jedec_id, 0);
	mutex_unlock(&flash->lock);
	return 0;
}

static int sst_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	struct spi_transfer t[4];
	struct spi_message m;
	size_t actual;
	int cmd_sz, ret;

	pr_debug("%s: %s to 0x%08x, len %zd\n", dev_name(&flash->spi->dev),
			__func__, (u32)to, len);

	spi_message_init(&m);
	memset(t, 0, (sizeof t));

	t[CMD].tx_buf = &flash->command[0];
	t[CMD].len = 1;
	t[CMD].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[CMD], &m);

	t[ADDR].tx_buf = &flash->command[1];
	t[ADDR].len = flash->addr_width;
	t[ADDR].tx_nbits = SPI_NBITS_SINGLE;
	spi_message_add_tail(&t[ADDR], &m);

	t[DATA].tx_buf = buf;
	t[DATA].len = len;
	t[DATA].tx_nbits = m25p80_tx_nbits(flash);
	spi_message_add_tail(&t[DATA], &m);

	mutex_lock(&flash->lock);

	/* Wait until finished previous write command. */
	ret = wait_till_ready(flash);
	if (ret)
		goto time_out;

	write_enable(flash);

	actual = to % 2;
	/* Start write from odd address. */
	if (actual) {
		flash->command[0] = OPCODE_BP;
		m25p_addr2cmd(flash, to, flash->command);

		/* write one byte. */
		t[DATA].len = 1;
		spi_sync(flash->spi, &m);
		ret = wait_till_ready(flash);
		if (ret)
			goto time_out;
		*retlen += m.actual_length - m25p_cmdsz(flash);
	}
	to += actual;

	flash->command[0] = OPCODE_AAI_WP;
	m25p_addr2cmd(flash, to, flash->command);

	/* Write out most of the data here. */
	cmd_sz = m25p_cmdsz(flash);
	for (; actual < len - 1; actual += 2) {
		t[ADDR].len = cmd_sz - 1;
		/* write two bytes. */
		t[DATA].len = 2;
		t[DATA].tx_buf = buf + actual;

		spi_sync(flash->spi, &m);
		ret = wait_till_ready(flash);
		if (ret)
			goto time_out;
		*retlen += m.actual_length - cmd_sz;
		cmd_sz = 1;
		to += 2;
	}
	write_disable(flash);
	ret = wait_till_ready(flash);
	if (ret)
		goto time_out;

	/* Write out trailing byte if it exists. */
	if (actual != len) {
		write_enable(flash);
		flash->command[0] = OPCODE_BP;
		m25p_addr2cmd(flash, to, flash->command);
		t[DATA].len = 1;
		t[DATA].tx_buf = buf + actual;

		spi_sync(flash->spi, &m);
		ret = wait_till_ready(flash);
		if (ret)
			goto time_out;
		*retlen += m.actual_length - m25p_cmdsz(flash);
		write_disable(flash);
	}

time_out:
	mutex_unlock(&flash->lock);
	return ret;
}

static int m25p80_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	uint32_t offset = ofs;
	uint8_t status_old, status_new;
	int res = 0;

	mutex_lock(&flash->lock);
	/* Wait until finished previous command */
	if (wait_till_ready(flash)) {
		res = 1;
		goto err;
	}

	status_old = read_sr(flash);

	if (offset < flash->mtd.size-(flash->mtd.size/2))
		status_new = status_old | SR_BP2 | SR_BP1 | SR_BP0;
	else if (offset < flash->mtd.size-(flash->mtd.size/4))
		status_new = (status_old & ~SR_BP0) | SR_BP2 | SR_BP1;
	else if (offset < flash->mtd.size-(flash->mtd.size/8))
		status_new = (status_old & ~SR_BP1) | SR_BP2 | SR_BP0;
	else if (offset < flash->mtd.size-(flash->mtd.size/16))
		status_new = (status_old & ~(SR_BP0|SR_BP1)) | SR_BP2;
	else if (offset < flash->mtd.size-(flash->mtd.size/32))
		status_new = (status_old & ~SR_BP2) | SR_BP1 | SR_BP0;
	else if (offset < flash->mtd.size-(flash->mtd.size/64))
		status_new = (status_old & ~(SR_BP2|SR_BP0)) | SR_BP1;
	else
		status_new = (status_old & ~(SR_BP2|SR_BP1)) | SR_BP0;

	/* Only modify protection if it will not unlock other areas */
	if ((status_new&(SR_BP2|SR_BP1|SR_BP0)) >
					(status_old&(SR_BP2|SR_BP1|SR_BP0))) {
		write_enable(flash);
		if (write_sr(flash, status_new) < 0) {
			res = 1;
			goto err;
		}
	}

err:	mutex_unlock(&flash->lock);
	return res;
}

static int m25p80_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct m25p *flash = mtd_to_m25p(mtd);
	uint32_t offset = ofs;
	uint8_t status_old, status_new;
	int res = 0;

	mutex_lock(&flash->lock);
	/* Wait until finished previous command */
	if (wait_till_ready(flash)) {
		res = 1;
		goto err;
	}

	status_old = read_sr(flash);

	if (offset+len > flash->mtd.size-(flash->mtd.size/64))
		status_new = status_old & ~(SR_BP2|SR_BP1|SR_BP0);
	else if (offset+len > flash->mtd.size-(flash->mtd.size/32))
		status_new = (status_old & ~(SR_BP2|SR_BP1)) | SR_BP0;
	else if (offset+len > flash->mtd.size-(flash->mtd.size/16))
		status_new = (status_old & ~(SR_BP2|SR_BP0)) | SR_BP1;
	else if (offset+len > flash->mtd.size-(flash->mtd.size/8))
		status_new = (status_old & ~SR_BP2) | SR_BP1 | SR_BP0;
	else if (offset+len > flash->mtd.size-(flash->mtd.size/4))
		status_new = (status_old & ~(SR_BP0|SR_BP1)) | SR_BP2;
	else if (offset+len > flash->mtd.size-(flash->mtd.size/2))
		status_new = (status_old & ~SR_BP1) | SR_BP2 | SR_BP0;
	else
		status_new = (status_old & ~SR_BP0) | SR_BP2 | SR_BP1;

	/* Only modify protection if it will not lock other areas */
	if ((status_new&(SR_BP2|SR_BP1|SR_BP0)) <
					(status_old&(SR_BP2|SR_BP1|SR_BP0))) {
		write_enable(flash);
		if (write_sr(flash, status_new) < 0) {
			res = 1;
			goto err;
		}
	}

err:	mutex_unlock(&flash->lock);
	return res;
}

/****************************************************************************/

/*
 * SPI device driver setup and teardown
 */

struct flash_info {
	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32		jedec_id;
	u16             ext_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16		n_sectors;

	u16		page_size;
	u16		addr_width;

	u16		flags;
#define	SECT_4K		0x01		/* OPCODE_BE_4K works uniformly */
#define	M25P_NO_ERASE	0x02		/* No erase command needed */
#define	SST_WRITE	0x04		/* use SST byte programming */
#define SPI_NOR_DUAL_READ BIT(5)
#define SPI_NOR_QUAD_READ BIT(6)
#define SPI_NOR_DUAL_WRITE BIT(7)
#define SPI_NOR_QUAD_WRITE BIT(8)

};

#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
	((kernel_ulong_t)&(struct flash_info) {				\
		.jedec_id = (_jedec_id),				\
		.ext_id = (_ext_id),					\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),					\
	})

#define CAT25_INFO(_sector_size, _n_sectors, _page_size, _addr_width)	\
	((kernel_ulong_t)&(struct flash_info) {				\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = (_page_size),				\
		.addr_width = (_addr_width),				\
		.flags = M25P_NO_ERASE,					\
	})

/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static const struct spi_device_id m25p_ids[] = {
	/* Atmel -- some are (confusingly) marketed as "DataFlash" */
	{ "at25fs010",  INFO(0x1f6601, 0, 32 * 1024,   4, SECT_4K) },
	{ "at25fs040",  INFO(0x1f6604, 0, 64 * 1024,   8, SECT_4K) },

	{ "at25df041a", INFO(0x1f4401, 0, 64 * 1024,   8, SECT_4K) },
	{ "at25df321a", INFO(0x1f4701, 0, 64 * 1024,  64, SECT_4K) },
	{ "at25df641",  INFO(0x1f4800, 0, 64 * 1024, 128, SECT_4K) },

	{ "at26f004",   INFO(0x1f0400, 0, 64 * 1024,  8, SECT_4K) },
	{ "at26df081a", INFO(0x1f4501, 0, 64 * 1024, 16, SECT_4K) },
	{ "at26df161a", INFO(0x1f4601, 0, 64 * 1024, 32, SECT_4K) },
	{ "at26df321",  INFO(0x1f4700, 0, 64 * 1024, 64, SECT_4K) },

	{ "at45db081d", INFO(0x1f2500, 0, 64 * 1024, 16, SECT_4K) },

	/* EON -- en25xxx */
	{ "en25f32", INFO(0x1c3116, 0, 64 * 1024,  64, SECT_4K) },
	{ "en25p32", INFO(0x1c2016, 0, 64 * 1024,  64, 0) },
	{ "en25q32b", INFO(0x1c3016, 0, 64 * 1024,  64, 0) },
	{ "en25p64", INFO(0x1c2017, 0, 64 * 1024, 128, 0) },
	{ "en25q64", INFO(0x1c3017, 0, 64 * 1024, 128, SECT_4K) },
	{ "en25qh256", INFO(0x1c7019, 0, 64 * 1024, 512, 0) },

	/* Everspin */
	{ "mr25h256", CAT25_INFO(  32 * 1024, 1, 256, 2) },

	/* GigaDevice */
	{ "gd25q32", INFO(0xc84016, 0, 64 * 1024,  64, SECT_4K) },
	//{ "gd25q64", INFO(0xc84017, 0, 64 * 1024, 128, SECT_4K) },
	{ "gd25q64", INFO(0xc84017, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },
	{ "gd25q127c", INFO(0xc84018, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },

	/* Intel/Numonyx -- xxxs33b */
	{ "160s33b",  INFO(0x898911, 0, 64 * 1024,  32, 0) },
	{ "320s33b",  INFO(0x898912, 0, 64 * 1024,  64, 0) },
	{ "640s33b",  INFO(0x898913, 0, 64 * 1024, 128, 0) },

	/* Macronix */
	{ "mx25l2005a",  INFO(0xc22012, 0, 64 * 1024,   4, SECT_4K) },
	{ "mx25l4005a",  INFO(0xc22013, 0, 64 * 1024,   8, SECT_4K) },
	{ "mx25l8005",   INFO(0xc22014, 0, 64 * 1024,  16, 0) },
	{ "mx25l1606e",  INFO(0xc22015, 0, 64 * 1024,  32, SECT_4K) },
	{ "mx25l3205d",  INFO(0xc22016, 0, 64 * 1024,  64, 0) },
	{ "mx25l6405d",  INFO(0xc22017, 0, 64 * 1024, 128, 0) },
	{ "mx25l12805d", INFO(0xc22018, 0, 64 * 1024, 256, 0) },
	{ "mx25l12855e", INFO(0xc22618, 0, 64 * 1024, 256, 0) },
	{ "mx25l25635e", INFO(0xc22019, 0, 64 * 1024, 512, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },
	//{ "mx25l25635e", INFO(0xc22019, 0, 64 * 1024, 512, 0) },
	{ "mx25l25655e", INFO(0xc22619, 0, 64 * 1024, 512, 0) },
	{ "mx66l51235l", INFO(0xc2201a, 0, 64 * 1024, 1024, 0) },

	/* Micron */
	{ "n25q064",  INFO(0x20ba17, 0, 64 * 1024, 128, 0) },
	{ "n25q128a11",  INFO(0x20bb18, 0, 64 * 1024, 256, 0) },
	{ "n25q128a13",  INFO(0x20ba18, 0, 64 * 1024, 256, 0) },
	{ "n25q256a", INFO(0x20ba19, 0, 64 * 1024, 512, SECT_4K) },

	/* Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl032p",  INFO(0x010215, 0x4d00,  64 * 1024,  64, 0) },
	{ "s25sl064p",  INFO(0x010216, 0x4d00,  64 * 1024, 128, 0) },
	{ "s25fl256s0", INFO(0x010219, 0x4d00, 256 * 1024, 128, 0) },
	{ "s25fl256s1", INFO(0x010219, 0x4d01,  64 * 1024, 512, 0) },
	{ "s25fl512s",  INFO(0x010220, 0x4d00, 256 * 1024, 256, 0) },
	{ "s70fl01gs",  INFO(0x010221, 0x4d00, 256 * 1024, 256, 0) },
	{ "s25sl12800", INFO(0x012018, 0x0300, 256 * 1024,  64, 0) },
	{ "s25sl12801", INFO(0x012018, 0x0301,  64 * 1024, 256, 0) },
	{ "s25fl129p0", INFO(0x012018, 0x4d00, 256 * 1024,  64, 0) },
	{ "s25fl129p1", INFO(0x012018, 0x4d01,  64 * 1024, 256, 0) },
	{ "s25sl004a",  INFO(0x010212,      0,  64 * 1024,   8, 0) },
	{ "s25sl008a",  INFO(0x010213,      0,  64 * 1024,  16, 0) },
	{ "s25sl016a",  INFO(0x010214,      0,  64 * 1024,  32, 0) },
	{ "s25sl032a",  INFO(0x010215,      0,  64 * 1024,  64, 0) },
	{ "s25sl064a",  INFO(0x010216,      0,  64 * 1024, 128, 0) },
	{ "s25fl016k",  INFO(0xef4015,      0,  64 * 1024,  32, SECT_4K) },
	//jedec = 0xef4017? but "w25q64" have the same jedec? something wrong here
//	{ "s25fl064k",  INFO(0xef4017,      0,  64 * 1024, 128, SECT_4K) },

	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf010a", INFO(0, 0xbf, 32 * 1024,  4, SECT_4K | SST_WRITE) },
	{ "sst25vf040b", INFO(0xbf258d, 0, 64 * 1024,  8, SECT_4K | SST_WRITE) },
	{ "sst25vf080b", INFO(0xbf258e, 0, 64 * 1024, 16, SECT_4K | SST_WRITE) },
	{ "sst25vf016b", INFO(0xbf2541, 0, 64 * 1024, 32, SECT_4K | SST_WRITE) },
	{ "sst25vf032b", INFO(0xbf254a, 0, 64 * 1024, 64, SECT_4K | SST_WRITE) },
	{ "sst25vf064c", INFO(0xbf254b, 0, 64 * 1024, 128, SECT_4K) },
	{ "sst25wf512",  INFO(0xbf2501, 0, 64 * 1024,  1, SECT_4K | SST_WRITE) },
	{ "sst25wf010",  INFO(0xbf2502, 0, 64 * 1024,  2, SECT_4K | SST_WRITE) },
	{ "sst25wf020",  INFO(0xbf2503, 0, 64 * 1024,  4, SECT_4K | SST_WRITE) },
	{ "sst25wf040",  INFO(0xbf2504, 0, 64 * 1024,  8, SECT_4K | SST_WRITE) },

	/* ST Microelectronics -- newer production may have feature updates */
	{ "m25p05",  INFO(0x202010,  0,  32 * 1024,   2, 0) },
	{ "m25p10",  INFO(0x202011,  0,  32 * 1024,   4, 0) },
	{ "m25p20",  INFO(0x202012,  0,  64 * 1024,   4, 0) },
	{ "m25p40",  INFO(0x202013,  0,  64 * 1024,   8, 0) },
	{ "m25p80",  INFO(0x202014,  0,  64 * 1024,  16, 0) },
	{ "m25p16",  INFO(0x202015,  0,  64 * 1024,  32, 0) },
	{ "m25p32",  INFO(0x202016,  0,  64 * 1024,  64, 0) },
	{ "m25p64",  INFO(0x202017,  0,  64 * 1024, 128, 0) },
	{ "m25p128", INFO(0x202018,  0, 256 * 1024,  64, 0) },
	{ "m25p12835", INFO(0xc22018,  0, 32 * 1024,  512, 0) },
	{ "n25q032", INFO(0x20ba16,  0,  64 * 1024,  64, 0) },

	{ "m25p05-nonjedec",  INFO(0, 0,  32 * 1024,   2, 0) },
	{ "m25p10-nonjedec",  INFO(0, 0,  32 * 1024,   4, 0) },
	{ "m25p20-nonjedec",  INFO(0, 0,  64 * 1024,   4, 0) },
	{ "m25p40-nonjedec",  INFO(0, 0,  64 * 1024,   8, 0) },
	{ "m25p80-nonjedec",  INFO(0, 0,  64 * 1024,  16, 0) },
	{ "m25p16-nonjedec",  INFO(0, 0,  64 * 1024,  32, 0) },
	{ "m25p32-nonjedec",  INFO(0, 0,  64 * 1024,  64, 0) },
	{ "m25p64-nonjedec",  INFO(0, 0,  64 * 1024, 128, 0) },
	{ "m25p128-nonjedec", INFO(0, 0, 256 * 1024,  64, 0) },

	{ "m45pe10", INFO(0x204011,  0, 64 * 1024,    2, 0) },
	{ "m45pe80", INFO(0x204014,  0, 64 * 1024,   16, 0) },
	{ "m45pe16", INFO(0x204015,  0, 64 * 1024,   32, 0) },

	{ "m25pe20", INFO(0x208012,  0, 64 * 1024,  4,       0) },
	{ "m25pe80", INFO(0x208014,  0, 64 * 1024, 16,       0) },
	{ "m25pe16", INFO(0x208015,  0, 64 * 1024, 32, SECT_4K) },

	{ "m25px32",    INFO(0x207116,  0, 64 * 1024, 64, SECT_4K) },
	{ "m25px32-s0", INFO(0x207316,  0, 64 * 1024, 64, SECT_4K) },
	{ "m25px32-s1", INFO(0x206316,  0, 64 * 1024, 64, SECT_4K) },
	{ "m25px64",    INFO(0x207117,  0, 64 * 1024, 128, 0) },

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
	//the n_sector of w25q64 is actually 128, however the spi_ftl only accept 256. We need to check the spi_flt.
//	{ "w25q64", INFO(0xef4017, 0, 64 * 1024, 128, SECT_4K) },
	{ "w25q64", INFO(0xef4017, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },
	{ "w25q80", INFO(0xef5014, 0, 64 * 1024,  16, SECT_4K) },
	{ "w25q80bl", INFO(0xef4014, 0, 64 * 1024,  16, SECT_4K) },
//  { "w25q128", INFO(0xef4018, 0, 64 * 1024, 256, 0) },
	{ "w25q128", INFO(0xef4018, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },
//	{ "w25q256", INFO(0xef4019, 0, 64 * 1024, 512, SECT_4K) },
	{ "w25q256", INFO(0xef4019, 0, 64 * 1024, 512, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },

	/* Catalyst / On Semiconductor -- non-JEDEC */
	{ "cat25c11", CAT25_INFO(  16, 8, 16, 1) },
	{ "cat25c03", CAT25_INFO(  32, 8, 16, 2) },
	{ "cat25c09", CAT25_INFO( 128, 8, 32, 2) },
	{ "cat25c17", CAT25_INFO( 256, 8, 32, 2) },
	{ "cat25128", CAT25_INFO(2048, 8, 64, 2) },

	/* pn -- non-JEDEC */
	{ "pn25f128b", INFO(0x1c7018, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },

	/* xt -- non-JEDEC */
	{ "xt25f128a", INFO(0x207018, 0, 64 * 1024, 256, SPI_NOR_QUAD_READ | SPI_NOR_QUAD_WRITE) },

	{ },
};
MODULE_DEVICE_TABLE(spi, m25p_ids);

static const struct spi_device_id *jedec_probe(struct spi_device *spi)
{
	int			tmp;
	u8			code = OPCODE_RDID;
	u8			id[5];
	u32			jedec;
	u16                     ext_jedec;
	struct flash_info	*info;

	/* JEDEC also defines an optional "extended device information"
	 * string for after vendor-specific data, after the three bytes
	 * we use here.  Supporting some chips might require using it.
	 */
	tmp = spi_write_then_read(spi, &code, 1, id, 5);
	if (tmp < 0) {
		pr_debug("%s: error %d reading JEDEC ID\n",
				dev_name(&spi->dev), tmp);
		return ERR_PTR(tmp);
	}
	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	ext_jedec = id[3] << 8 | id[4];

	for (tmp = 0; tmp < ARRAY_SIZE(m25p_ids) - 1; tmp++) {
		info = (void *)m25p_ids[tmp].driver_data;
		if (info->jedec_id == jedec) {
			if (info->ext_id != 0 && info->ext_id != ext_jedec)
				continue;
			dev_err(&spi->dev, "recognized JEDEC id %06x\n", jedec);
			return &m25p_ids[tmp];
		}
	}
	dev_err(&spi->dev, "unrecognized JEDEC id %06x\n", jedec);
	return ERR_PTR(-ENODEV);
}


/*
 * board specific setup should have ensured the SPI clock used here
 * matches what the READ command supports, at least until this driver
 * understands FAST_READ (for clocks over 25 MHz).
 */
static int m25p_probe(struct spi_device *spi)
{
	const struct spi_device_id	*id = spi_get_device_id(spi);
	struct flash_platform_data	*data;
	struct m25p			*flash;
	struct flash_info		*info;
	unsigned			i;
	struct mtd_part_parser_data	ppdata;
	struct device_node __maybe_unused *np = spi->dev.of_node;

#ifdef CONFIG_MTD_OF_PARTS
	if (!of_device_is_available(np))
		return -ENODEV;
#endif

	/* Platform data helps sort out which chip type we have, as
	 * well as how this board partitions it.  If we don't have
	 * a chip ID, try the JEDEC id commands; they'll work for most
	 * newer chips, even if we don't recognize the particular chip.
	 */
	data = spi->dev.platform_data;
	if (data && data->type) {
		const struct spi_device_id *plat_id;

		for (i = 0; i < ARRAY_SIZE(m25p_ids) - 1; i++) {
			plat_id = &m25p_ids[i];
			if (strcmp(data->type, plat_id->name))
				continue;
			break;
		}

		if (i < ARRAY_SIZE(m25p_ids) - 1)
			id = plat_id;
		else
			dev_warn(&spi->dev, "unrecognized id %s\n", data->type);
	}

	info = (void *)id->driver_data;

	if (info->jedec_id) {
		const struct spi_device_id *jid;

		jid = jedec_probe(spi);
		if (IS_ERR(jid)) {
			return PTR_ERR(jid);
		} else if (jid != id) {
			/*
			 * JEDEC knows better, so overwrite platform ID. We
			 * can't trust partitions any longer, but we'll let
			 * mtd apply them anyway, since some partitions may be
			 * marked read-only, and we don't want to lose that
			 * information, even if it's not 100% accurate.
			 */
			dev_warn(&spi->dev, "found %s, expected %s\n",
				 jid->name, id->name);
			id = jid;
			info = (void *)jid->driver_data;
		}
	}

	flash = kzalloc(sizeof *flash, GFP_KERNEL);
	if (!flash)
		return -ENOMEM;
	flash->command = kmalloc(MAX_CMD_SIZE + (flash->fast_read ? 1 : 0),
					GFP_KERNEL);
	if (!flash->command) {
		kfree(flash);
		return -ENOMEM;
	}

	flash->spi = spi;
	mutex_init(&flash->lock);
	dev_set_drvdata(&spi->dev, flash);

	/*
	 * Atmel, SST and Intel/Numonyx serial flash tend to power
	 * up with the software protection bits set
	 */

	if (JEDEC_MFR(info->jedec_id) == CFI_MFR_ATMEL ||
	    JEDEC_MFR(info->jedec_id) == CFI_MFR_INTEL ||
	    JEDEC_MFR(info->jedec_id) == CFI_MFR_SST) {
		write_enable(flash);
		write_sr(flash, 0);
	}

	if (data && data->name) {
		flash->mtd.name = data->name;
	} else {
		if (item_exist("part0"))
			flash->mtd.name = dev_name(&spi->dev);
		else
			flash->mtd.name = MTD_CMDLINE_NAME;
	}

	flash->mtd.type = MTD_NORFLASH;
	flash->mtd.writesize = 1;
	flash->mtd.flags = MTD_CAP_NORFLASH;
	flash->mtd.size = info->sector_size * info->n_sectors;
	flash->mtd._erase = m25p80_erase;
	flash->mtd._read = sub_m25p80_read;

	/* flash protection support for STmicro chips */
	if (JEDEC_MFR(info->jedec_id) == CFI_MFR_ST) {
		flash->mtd._lock = m25p80_lock;
		flash->mtd._unlock = m25p80_unlock;
	}

	/* sst flash chips use AAI word program */
	if (info->flags & SST_WRITE)
		flash->mtd._write = sst_write;
	else
		flash->mtd._write = m25p80_write;

	/* prefer "small sector" erase if possible */
	if (info->flags & SECT_4K) {
		flash->erase_opcode = OPCODE_BE_4K;
		flash->mtd.erasesize = 4096;
	} else {
		flash->erase_opcode = OPCODE_SE;
		flash->mtd.erasesize = info->sector_size;
	}

	if (info->flags & M25P_NO_ERASE)
		flash->mtd.flags |= MTD_NO_ERASE;

	ppdata.of_node = spi->dev.of_node;
	flash->mtd.dev.parent = &spi->dev;
	flash->page_size = info->page_size;
	flash->mtd.writebufsize = flash->page_size;

	flash->fast_read = false;
	flash->flash_read = M25P80_RX_NORMAL;
	flash->flash_write = M25P80_TX_NORMAL;
#ifdef CONFIG_OF
	if (np && of_property_read_bool(np, "m25p,fast-read")) {
		flash->fast_read = true;
		flash->flash_read = M25P80_RX_FAST; 
	}
#endif

#ifdef CONFIG_M25PXX_USE_FAST_READ
	flash->fast_read = true;
	flash->flash_read = M25P80_RX_FAST;
#endif

	pr_info("[m25p80] spi mode - %x, flash mode -%x\n", spi->mode, info->flags);
	if ((spi->mode & SPI_RX_QUAD) && (info->flags & SPI_NOR_QUAD_READ)) {
			flash->flash_read = M25P80_RX_QUAD;
	}
	if ((spi->mode & SPI_TX_QUAD) && (info->flags & SPI_NOR_QUAD_WRITE)) {
			flash->flash_write = M25P80_TX_QUAD;
	}


	flash->g_jedec_id = info->jedec_id;

	if (info->addr_width)
		flash->addr_width = info->addr_width;
	else {
		/* enable 4-byte addressing if the device exceeds 16MiB */
		if (flash->mtd.size > 0x1000000) {
			flash->addr_width = 4;
			set_4byte(flash, info->jedec_id, 1);
		} else
			flash->addr_width = 3;
	}

#ifdef CONFIG_ARCH_APOLLO
	flash->set4byte = 1;
#else
	flash->set4byte = 0;
#endif

	dev_info(&spi->dev, "%s (%lld Kbytes)\n", id->name,
			(long long)flash->mtd.size >> 10);

	pr_debug("mtd .name = %s, .size = 0x%llx (%lldMiB) "
			".erasesize = 0x%.8x (%uKiB) .numeraseregions = %d\n",
		flash->mtd.name,
		(long long)flash->mtd.size, (long long)(flash->mtd.size >> 20),
		flash->mtd.erasesize, flash->mtd.erasesize / 1024,
		flash->mtd.numeraseregions);

	if (flash->mtd.numeraseregions)
		for (i = 0; i < flash->mtd.numeraseregions; i++)
			pr_debug("mtd.eraseregions[%d] = { .offset = 0x%llx, "
				".erasesize = 0x%.8x (%uKiB), "
				".numblocks = %d }\n",
				i, (long long)flash->mtd.eraseregions[i].offset,
				flash->mtd.eraseregions[i].erasesize,
				flash->mtd.eraseregions[i].erasesize / 1024,
				flash->mtd.eraseregions[i].numblocks);

	//TODO: complement disable protect function for flash from other manufacturers
	if((JEDEC_MFR(info->jedec_id) == CFI_MFR_WINBOND) || (JEDEC_MFR(info->jedec_id) == CFI_MFR_GIGADEVICE)) {
		if(info->jedec_id == JEDEC_W25Q64) {
			protect_disable_w25q64(flash);
		}
		else {
			protect_disable_wb(flash);
		}
	}

	if(flash->flash_read == M25P80_RX_QUAD || flash->flash_write == M25P80_TX_QUAD) {
		pr_info("[m25p80] will use 4-line mode\n");
		if (JEDEC_MFR(info->jedec_id) == CFI_MFR_MACRONIX) {
			quad_enable_mx(flash);
		}
		else if (JEDEC_MFR(info->jedec_id) == CFI_MFR_WINBOND || JEDEC_MFR(info->jedec_id) == CFI_MFR_GIGADEVICE) {
			if(info->jedec_id == JEDEC_W25Q64) {
				quad_enable_wb_w25q64(flash);
			}
			else {
				quad_enable_wb(flash);
			}
		}
#if 0
		else if(JEDEC_MFR(info->jedec_id) == CFI_MFR_XT) {
			//do nothing
		}

		else if(JEDEC_MFR(info->jedec_id) == CFI_MFR_PN) {
			//do nothing
		}
#endif
	}
	else {
		pr_info("[m25p80] will use 1-line mode\n");
		if (JEDEC_MFR(info->jedec_id) == CFI_MFR_MACRONIX) {
			quad_disable_mx(flash);
		}
		else if (JEDEC_MFR(info->jedec_id) == CFI_MFR_WINBOND || JEDEC_MFR(info->jedec_id) == CFI_MFR_GIGADEVICE) {
			if(info->jedec_id == JEDEC_W25Q64) {
				quad_disable_wb_w25q64(flash);
			}
			else {
				quad_disable_wb(flash);
			}
		}

	}
#ifdef CONFIG_DEBUG_FS
	flash_debugfs_init(&flash->mtd);
#endif

	//sc_init(&ssr_sc, 0x4000);
	/* partitions should match sector boundaries; and it may be good to
	 * use readonly partitions for writeprotected sectors (BP2..BP0).
	 */
	return mtd_device_parse_register(&flash->mtd, NULL, &ppdata,
			data ? data->parts : NULL,
			data ? data->nr_parts : 0);
}


static int m25p_remove(struct spi_device *spi)
{
	struct m25p	*flash = dev_get_drvdata(&spi->dev);
	int		status;

	/* Clean up MTD stuff. */
	status = mtd_device_unregister(&flash->mtd);
	if (status == 0) {
		kfree(flash->command);
		kfree(flash);
	}
	return 0;
}

static void m25p_shutdown(struct spi_device *spi)
{
    struct m25p *flash = dev_get_drvdata(&spi->dev);

    reset_chip(flash);
    return;
}

static struct spi_driver m25p80_driver = {
	.driver = {
		.name	= "m25p80",
		.owner	= THIS_MODULE,
	},
	.id_table	= m25p_ids,
	.probe	= m25p_probe,
	.remove	= m25p_remove,
    .shutdown   = m25p_shutdown,

	/* REVISIT: many of these chips have deep power-down modes, which
	 * should clearly be entered on suspend() to minimize power use.
	 * And also when they're otherwise idle...
	 */
};

module_spi_driver(m25p80_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mike Lavender");
MODULE_DESCRIPTION("MTD SPI driver for ST M25Pxx flash chips");
