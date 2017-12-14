/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based vaguely on the Linux code
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <linux/config.h>
//#include <common.h>
//#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <linux/list.h>
#include <linux/types.h>
#include <div64.h>
#include <asm-arm/byteorder.h>
#include <preloader.h>

#define memcpy irf->memcpy
#define module_enable irf->module_enable
#define pads_chmod irf->pads_chmod
#define pads_pull irf->pads_pull
#define sprintf irf->sprintf
#define printf irf->printf
#define malloc irf->malloc
#define udelay irf->udelay
#define strncmp irf->strncmp
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern uint32_t __div64_32(uint64_t *dividend, uint32_t divisor);

/* The unnecessary pointer compare is there
 * to check for type safety (n must be 64bit)
 */
# define do_div(n,base) ({                              \
        uint32_t __base = (base);                       \
        uint32_t __rem;                                 \
        (void)(((typeof((n)) *)0) == ((uint64_t *)0));  \
        if (((n) >> 32) == 0) {                 \
                __rem = (uint32_t)(n) % __base;         \
                (n) = (uint32_t)(n) / __base;           \
        } else                                          \
                __rem = __div64_32(&(n), __base);       \
        __rem;                                          \
 })
 
	/* Wrapper for do_div(). Doesn't modify dividend and returns
	 * the result, not reminder.
	 */
	static inline uint64_t lldiv(uint64_t dividend, uint32_t divisor)
	{
		uint64_t __res = dividend;
		do_div(__res, divisor);
		return(__res);
	}
	
//#define CONFIG_MMC_TRACE
/* Set block count limit because of 16 bit register limit on some hardware*/
#ifndef CONFIG_SYS_MMC_MAX_BLK_COUNT
#define CONFIG_SYS_MMC_MAX_BLK_COUNT   512 // 65535  
#endif

extern int board_mmc_init(void);

static struct list_head mmc_devices;
static int cur_dev_num = -1;

int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
#ifdef CONFIG_MMC_TRACE
	int ret;
	int i;
	u8 *ptr;

	printf("CMD_SEND:%d\n", cmd->cmdidx);
	printf("\t\tARG\t\t\t 0x%08X\n", cmd->cmdarg);
	printf("\t\tFLAG\t\t\t %d\n", cmd->flags);
	ret = mmc->send_cmd(mmc, cmd, data);
	switch (cmd->resp_type) {
		case MMC_RSP_NONE:
			printf("\t\tMMC_RSP_NONE\n");
			break;
		case MMC_RSP_R1:
			printf("\t\tMMC_RSP_R1,5,6,7 \t 0x%08X \n",
				cmd->response[0]);
			break;
		case MMC_RSP_R1b:
			printf("\t\tMMC_RSP_R1b\t\t 0x%08X \n",
				cmd->response[0]);
			break;
		case MMC_RSP_R2:
			printf("\t\tMMC_RSP_R2\t\t 0x%08X \n",
				cmd->response[0]);
			printf("\t\t          \t\t 0x%08X \n",
				cmd->response[1]);
			printf("\t\t          \t\t 0x%08X \n",
				cmd->response[2]);
			printf("\t\t          \t\t 0x%08X \n",
				cmd->response[3]);
			printf("\n");
			printf("\t\t\t\t\tDUMPING DATA\n");
			for (i = 0; i < 4; i++) {
				int j;
				printf("\t\t\t\t\t%03d - ", i*4);
				ptr = &cmd->response[i];
				ptr += 3;
				for (j = 0; j < 4; j++)
					printf("%02X ", *ptr--);
				printf("\n");
			}
			break;
		case MMC_RSP_R3:
			printf("\t\tMMC_RSP_R3,4\t\t 0x%08X \n",
				cmd->response[0]);
			break;
		default:
			printf("\t\tERROR MMC rsp not supported\n");
			break;
	}
	return ret;
#else
	return mmc->send_cmd(mmc, cmd, data);
#endif
}

int mmc_send_status(struct mmc *mmc, int timeout)
{
	struct mmc_cmd cmd;
	int err;
#ifdef CONFIG_MMC_TRACE
	int status;
#endif

	cmd.cmdidx = MMC_CMD_SEND_STATUS;
	cmd.resp_type = MMC_RSP_R1;
	//cmd.cmdarg = 0;
	cmd.cmdarg = (mmc->rca << 16);
	cmd.flags = 0;

	do {
		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (err)
			return err;
		else if (cmd.response[0] & MMC_STATUS_RDY_FOR_DATA)
			break;

		udelay(5);

		if (cmd.response[0] & MMC_STATUS_MASK) {
			printf("Status Error: 0x%08X\n", cmd.response[0]);
			return COMM_ERR;
		}
	} while (timeout--);

#ifdef CONFIG_MMC_TRACE
	status = (cmd.response[0] & MMC_STATUS_CURR_STATE) >> 9;
	printf("CURR STATE:%d\n", status);
#endif
	if (!timeout) {
		printf("Timeout waiting card ready\n");
		return TIMEOUT;
	}

	return 0;
}

int mmc_set_blocklen(struct mmc *mmc, int len)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;
	cmd.flags = 0;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

struct mmc *find_mmc_device(int dev_num)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		if (m->block_dev.dev == dev_num)
			return m;
	}

	printf("MMC Device %d not found\n", dev_num);

	return NULL;
}
#if 0
static ulong
mmc_write_blocks(struct mmc *mmc, ulong start, lbaint_t blkcnt, const void*src)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int timeout = 1000;

	if ((start + blkcnt) > mmc->block_dev.lba) {
		printf("MMC: block number 0x%lx exceeds max(0x%lx)\n",
			start + blkcnt, mmc->block_dev.lba);
		return 0;
	}

	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->write_bl_len;

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.src = src;
	data.blocks = blkcnt;
	data.blocksize = mmc->write_bl_len;
	data.flags = MMC_DATA_WRITE;

	if (mmc_send_cmd(mmc, &cmd, &data)) {
		printf("mmc write failed\n");
		return 0;
	}

	/* SPI multiblock writes terminate using a special
	 * token, not a STOP_TRANSMISSION request.
	 */
	if (!mmc_host_is_spi(mmc) && blkcnt > 1) {
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		cmd.flags = 0;
		if (mmc_send_cmd(mmc, &cmd, NULL)) {
			printf("mmc fail to send stop cmd\n");
			return 0;
		}

		/* Waiting for the ready status */
		mmc_send_status(mmc, timeout);
	}
	else {
		mmc_send_status(mmc, timeout);
	}

	return blkcnt;
}

static ulong
mmc_bwrite(int dev_num, ulong start, lbaint_t blkcnt, const void*src)
{
	lbaint_t cur, blocks_todo = blkcnt;

	struct mmc *mmc = find_mmc_device(dev_num);
	if (!mmc)
		return 0;

	if (mmc_set_blocklen(mmc, mmc->write_bl_len))
	{
		printf("wbl:0x%x\n", mmc->write_bl_len);
		return 0;
	}
	do {
		cur = (blocks_todo > mmc->b_max) ?  mmc->b_max : blocks_todo;
		//printf("w:%d %d\n", start, blocks_todo);
		if(mmc_write_blocks(mmc, start, cur, src) != cur)
			return 0;
		blocks_todo -= cur;
		start += cur;
		src += cur * mmc->write_bl_len;
	} while (blocks_todo > 0);
	return blkcnt;
}
#endif
int mmc_read_blocks(struct mmc *mmc, void *dst, ulong start, lbaint_t blkcnt)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int timeout = 1000;

	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->read_bl_len;

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = mmc->read_bl_len;
	data.flags = MMC_DATA_READ;

	if (mmc_send_cmd(mmc, &cmd, &data))
		return 0;

	if (blkcnt > 1) {
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		cmd.flags = 0;
		if (mmc_send_cmd(mmc, &cmd, NULL)) {
			printf("mmc fail to send stop cmd\n");
			return 0;
		}

		/* Waiting for the ready status */
		mmc_send_status(mmc, timeout);
	}
	else {
		mmc_send_status(mmc, timeout);
	}

	return blkcnt;
}

static ulong mmc_bread(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	lbaint_t cur, blocks_todo = blkcnt;

	if (blkcnt == 0)
		return 0;

	struct mmc *mmc = find_mmc_device(dev_num);
	if (!mmc)
		return 0;

	if ((start + blkcnt) > mmc->block_dev.lba) {
		printf("MMC%d: block number 0x%lx exceeds max(0x%lx)\n",
			dev_num, start + blkcnt, mmc->block_dev.lba);
		return 0;
	}
	if (mmc_set_blocklen(mmc, mmc->read_bl_len))
	{
		printf("rbl:0x%x\n",mmc->read_bl_len);
		return 0;
	}
	do {
		cur = (blocks_todo > mmc->b_max) ?  mmc->b_max : blocks_todo;
		if(mmc_read_blocks(mmc, dst, start, cur) != cur)
			return 0;
		blocks_todo -= cur;
		start += cur;
		dst += cur * mmc->read_bl_len;
	} while (blocks_todo > 0);

	return blkcnt;
}

int mmc_go_idle(struct mmc* mmc)
{
	struct mmc_cmd cmd;
	int err;

	udelay(5);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	udelay(5);

	return 0;
}

int
sd_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	int err;
	struct mmc_cmd cmd;

	do {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;

		/*
		 * Most cards do not answer if some reserved bits
		 * in the ocr are set. However, Some controller
		 * can set bit 7 (reserved for low voltages), but
		 * how to manage low voltages SD card is not yet
		 * specified.
		 */
		cmd.cmdarg = (mmc->voltages & 0xff8000);

		if (mmc->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		udelay(5);
	} while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	if (mmc->version != SD_VERSION_2)
		mmc->version = SD_VERSION_1_0;

	if (mmc_host_is_spi(mmc)) { /* read OCR for spi */
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}

	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}

int mmc_send_op_cond(struct mmc *mmc)
{
	int timeout = 10000;
	struct mmc_cmd cmd;
	int err;

	/* Some cards seem to need this */
	mmc_go_idle(mmc);

 	/* Asking to the card its capabilities */
 	cmd.cmdidx = MMC_CMD_SEND_OP_COND;
 	cmd.resp_type = MMC_RSP_R3;
 	cmd.cmdarg = 0;
 	cmd.flags = 0;

 	err = mmc_send_cmd(mmc, &cmd, NULL);

 	if (err)
 		return err;

 	udelay(5);

	do {
		cmd.cmdidx = MMC_CMD_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;
#if 0		
		cmd.cmdarg = (mmc_host_is_spi(mmc) ? 0 :
				(mmc->voltages &
				(cmd.response[0] & OCR_VOLTAGE_MASK)) |
				(cmd.response[0] & OCR_ACCESS_MODE));
#else
		cmd.cmdarg = mmc->voltages | (1<<30);
#endif		
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		udelay(5);
	} while (!(cmd.response[0] & OCR_BUSY) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	if (mmc_host_is_spi(mmc)) { /* read OCR for spi */
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}

	mmc->version = MMC_VERSION_UNKNOWN;
	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}


int mmc_send_ext_csd(struct mmc *mmc, char *ext_csd)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	/* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	data.dest = ext_csd;
	data.blocks = 1;
	data.blocksize = 512;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	return err;
}


int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	int ret;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
				 (index << 16) |
				 (value << 8);
	cmd.flags = 0;

	ret = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	mmc_send_status(mmc, timeout);

	return ret;

}

int mmc_change_freq(struct mmc *mmc)
{
	char ext_csd[512];
	char cardtype;
	int err;

	mmc->card_caps = 0;

	if (mmc_host_is_spi(mmc))
		return 0;

	/* Only version 4 supports high-speed */
	if (mmc->version < MMC_VERSION_4)
		return 0;

	mmc->card_caps |= MMC_MODE_4BIT;

	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	cardtype = ext_csd[196] & 0xf;

	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

	if (err)
		return err;

	/* Now check to see that it worked */
	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	/* No high-speed support */
	if (!ext_csd[185])
		return 0;

	/* High Speed is set, there are two types: 52MHz and 26MHz */
	if (cardtype & MMC_HS_52MHZ)
		mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	else
		mmc->card_caps |= MMC_MODE_HS;

	return 0;
}

#if 0
int mmc_switch_part(int dev_num, unsigned int part_num)
{
	struct mmc *mmc = find_mmc_device(dev_num);

	if (!mmc)
		return -1;

	return mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONF,
			  (mmc->part_config & ~PART_ACCESS_MASK)
			  | (part_num & PART_ACCESS_MASK));
}
#endif

int sd_switch(struct mmc *mmc, int mode, int group, u8 value, u8 *resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);
	cmd.flags = 0;

	data.dest = (char *)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}

/*send scr to verify 4_bit_line, if failed, send cmd12 to switch card to transfer state*/
int sd_verify_4_bit_line(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;
	uint scr[2];
	struct mmc_data data;

	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	data.dest = (char *)&scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err) {
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.resp_type = MMC_RSP_R1b;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		mmc_send_cmd(mmc, &cmd, NULL);
		return err;
	}

	return 0;
}

int sd_change_freq(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;
	uint scr[2];
	uint switch_status[16];
	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	if (mmc_host_is_spi(mmc))
		return 0;

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	timeout = 3;

retry_scr:
	data.dest = (char *)&scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err) {
		if (timeout--)
			goto retry_scr;

		return err;
	}

	mmc->scr[0] = __be32_to_cpu(scr[0]);
	mmc->scr[1] = __be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) {
		case 0:
			mmc->version = SD_VERSION_1_0;
			break;
		case 1:
			mmc->version = SD_VERSION_1_10;
			break;
		case 2:
			mmc->version = SD_VERSION_2;
			break;
		default:
			mmc->version = SD_VERSION_1_0;
			break;
	}

	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0)
		return 0;

	if(!(mmc->host_caps & MMC_MODE_HS))
		return 0;
	
	timeout = 4;
	while (timeout--) {
		err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,
				(u8 *)&switch_status);

		if (err)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
			break;
	}

	/* If high-speed isn't supported, we return */
	if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
		return 0;

	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)&switch_status);

	if (err)
		return err;

	if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
		mmc->card_caps |= MMC_MODE_HS;

	return 0;
}

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
static const int fbase[] = {
	10000,
	100000,
	1000000,
	10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
static const int multipliers[] = {
	0,	/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

void mmc_set_ios(struct mmc *mmc)
{
	mmc->set_ios(mmc);
}

void mmc_set_clock(struct mmc *mmc, uint clock)
{
	if (clock > mmc->f_max)
		clock = mmc->f_max;

	if (clock < mmc->f_min)
		clock = mmc->f_min;

	mmc->clock = clock;

	mmc_set_ios(mmc);
}

void mmc_set_bus_width(struct mmc *mmc, uint width)
{
	mmc->bus_width = width;

	mmc_set_ios(mmc);
}

int mmc_startup(struct mmc *mmc)
{
	int err;
	uint mult, freq;
	u64 cmult;
	u32 csize;
	struct mmc_cmd cmd;
	char ext_csd[512];
	int timeout = 1000;

#ifdef CONFIG_MMC_SPI_CRC_ON
	if (mmc_host_is_spi(mmc)) { /* enable CRC check for spi */
		cmd.cmdidx = MMC_CMD_SPI_CRC_ON_OFF;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 1;
		cmd.flags = 0;
		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}
#endif

	/* Put the Card in Identify Mode */
	cmd.cmdidx = mmc_host_is_spi(mmc) ? MMC_CMD_SEND_CID :
		MMC_CMD_ALL_SEND_CID; /* cmd not supported in spi */
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	memcpy(mmc->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	if (!mmc_host_is_spi(mmc)) { /* cmd not supported in spi */
		cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
		cmd.cmdarg = mmc->rca << 16;
		cmd.resp_type = MMC_RSP_R6;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		if (IS_SD(mmc))
			mmc->rca = (cmd.response[0] >> 16) & 0xffff;
	}

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	mmc_send_status(mmc, timeout);
	if (err)
		return err;

	mmc->csd[0] = cmd.response[0];
	mmc->csd[1] = cmd.response[1];
	mmc->csd[2] = cmd.response[2];
	mmc->csd[3] = cmd.response[3];

	if (mmc->version == MMC_VERSION_UNKNOWN) {
		int version = (cmd.response[0] >> 26) & 0xf;

		switch (version) {
			case 0:
				mmc->version = MMC_VERSION_1_2;
				break;
			case 1:
				mmc->version = MMC_VERSION_1_4;
				break;
			case 2:
				mmc->version = MMC_VERSION_2_2;
				break;
			case 3:
				mmc->version = MMC_VERSION_3;
				break;
			case 4:
				mmc->version = MMC_VERSION_4;
				break;
			default:
				mmc->version = MMC_VERSION_1_2;
				break;
		}
	}

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[(cmd.response[0] & 0x7)];
	mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

	mmc->tran_speed = freq * mult;

	mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);
	if (IS_SD(mmc))
		mmc->write_bl_len = mmc->read_bl_len;
	else
		mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);

	if (mmc->high_capacity) {
		csize = (mmc->csd[1] & 0x3f) << 16
			| (mmc->csd[2] & 0xffff0000) >> 16;
		cmult = 8;
	} else {
		csize = (mmc->csd[1] & 0x3ff) << 2
			| (mmc->csd[2] & 0xc0000000) >> 30;
		cmult = (mmc->csd[2] & 0x00038000) >> 15;
	}

	mmc->capacity = (csize + 1)<<(cmult + 2);
	mmc->capacity *= mmc->read_bl_len;
	if (mmc->read_bl_len > 512)
		mmc->read_bl_len = 512;

	if (mmc->write_bl_len > 512)
		mmc->write_bl_len = 512;

	/* Select the card, and put it into Transfer Mode */
	if (!mmc_host_is_spi(mmc)) { /* cmd not supported in spi */
		cmd.cmdidx = MMC_CMD_SELECT_CARD;
		cmd.resp_type = MMC_RSP_R1b;
		cmd.cmdarg = mmc->rca << 16;
		cmd.flags = 0;
		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}

	mmc->part_config = MMCPART_NOAVAILABLE;
	if (!IS_SD(mmc) && (mmc->version >= MMC_VERSION_4)) {
		/* check  ext_csd version and capacity */
		err = mmc_send_ext_csd(mmc, ext_csd);
		if (!err & (ext_csd[192] >= 2)) {
			mmc->capacity = ext_csd[212] << 0 | ext_csd[213] << 8 |
					ext_csd[214] << 16 | ext_csd[215] << 24;
			mmc->capacity *= 512;
		}

		/* store the partition info of emmc */
		if (ext_csd[160] & PART_SUPPORT)
			mmc->part_config = ext_csd[179];
	}
#if 1	
	if (IS_SD(mmc))
		err = sd_change_freq(mmc);
	else
		err = mmc_change_freq(mmc);

	if (err)
		return err;
#endif
	/* Restrict card's capabilities by what the host can do */
	mmc->card_caps &= mmc->host_caps;

	if (IS_SD(mmc)) {
		if (mmc->card_caps & MMC_MODE_4BIT) {
			cmd.cmdidx = MMC_CMD_APP_CMD;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = mmc->rca << 16;
			cmd.flags = 0;

			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
				return err;

			cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 2;
			cmd.flags = 0;
			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
				return err;

			mmc_set_bus_width(mmc, 4);

			if(0 != sd_verify_4_bit_line(mmc)){
				mmc->card_caps &= ~MMC_MODE_4BIT;
				mmc->host_caps &= ~MMC_MODE_4BIT;

				cmd.cmdidx = MMC_CMD_APP_CMD;
				cmd.resp_type = MMC_RSP_R1;
				cmd.cmdarg = mmc->rca << 16;
				cmd.flags = 0;

				err = mmc_send_cmd(mmc, &cmd, NULL);
				if (err)
					return err;

				cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
				cmd.resp_type = MMC_RSP_R1;
				cmd.cmdarg = 0;
				cmd.flags = 0;
				err = mmc_send_cmd(mmc, &cmd, NULL);
				if (err)
					return err;

				mmc_set_bus_width(mmc, 1);
			}
		}

		if (mmc->card_caps & MMC_MODE_HS)
			mmc_set_clock(mmc, 50000000);
		else
			mmc_set_clock(mmc, 25000000);
	} else {
		if (mmc->card_caps & MMC_MODE_4BIT) {
			/* Set the card to use 4 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_4);

			if (err)
				return err;

			mmc_set_bus_width(mmc, 4);
		} else if (mmc->card_caps & MMC_MODE_8BIT) {
			/* Set the card to use 8 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_8);

			if (err)
				return err;

			mmc_set_bus_width(mmc, 8);
		}

		if (mmc->card_caps & MMC_MODE_HS) {
			if (mmc->card_caps & MMC_MODE_HS_52MHz)
				mmc_set_clock(mmc, 52000000);
			else
			{	
				mmc_set_clock(mmc, 52000000);//26000000
			}
		} else
			mmc_set_clock(mmc, 20000000);
	}

	/* fill in device description */
	mmc->block_dev.lun = 0;
	mmc->block_dev.type = 0;
	mmc->block_dev.blksz = mmc->read_bl_len;
	mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);
	sprintf(mmc->block_dev.vendor, "Man %06x Snr %08x", mmc->cid[0] >> 8,
			(mmc->cid[2] << 8) | (mmc->cid[3] >> 24));
	sprintf(mmc->block_dev.product, "%c%c%c%c%c", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);
	sprintf(mmc->block_dev.revision, "%d.%d", mmc->cid[2] >> 28,
			(mmc->cid[2] >> 24) & 0xf);
	init_part(&mmc->block_dev);

	return 0;
}

int mmc_send_if_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = ((mmc->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return UNUSABLE_ERR;
	else
		mmc->version = SD_VERSION_2;

	return 0;
}

int mmc_register(struct mmc *mmc)
{
	/* Setup the universal parts of the block interface just once */
	mmc->block_dev.if_type = IF_TYPE_MMC;
	mmc->block_dev.dev = cur_dev_num++;
	mmc->block_dev.removable = 1;
	mmc->block_dev.block_read = mmc_bread;
	//mmc->block_dev.block_write = mmc_bwrite;
	if (!mmc->b_max)
		mmc->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	INIT_LIST_HEAD (&mmc->link);

	list_add_tail (&mmc->link, &mmc_devices);

	return 0;
}
/*
block_dev_desc_t *mmc_get_dev(int dev)
{
	struct mmc *mmc = find_mmc_device(dev);

	return mmc ? &mmc->block_dev : NULL;
}
*/

int mmc_init(int dev_num)
{
	int err;
	struct mmc *mmc = find_mmc_device(dev_num);
#if 0
	if (mmc->has_init)
		return 0;
#endif	
	//err = mmc->init(mmc);
	//if (err)
	//	return err;
	mmc_set_bus_width(mmc, 1);
	mmc_set_clock(mmc, 400000);
	/* Reset the Card */
	err = mmc_go_idle(mmc);
	if (err)
		return err;
	/* The internal partition reset to user partition(0) at every CMD0*/
	mmc->part_num = 0;
	/* Test for SD version 2 */
	err = mmc_send_if_cond(mmc);
	/* Now try to get the SD card's operating condition */
	err = sd_send_op_cond(mmc);
	/* If the command timed out, we check for an MMC card */
	if (err == TIMEOUT) {
//		printf("mmc_send_op_cond\n");
		err = mmc_send_op_cond(mmc);

		if (err) {
			printf("Card did not respond to voltage select!\n");
			return UNUSABLE_ERR;
		}
	}
	err = mmc_startup(mmc);

	//mmc_init_param(dev_num);
#if 0	
	if (err)
		mmc->has_init = 0;
	else
		mmc->has_init = 1;
#endif	
	return err;
}

/*
void print_mmc_devices(char separator)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		printf("%s: %d", m->name, m->block_dev.dev);

		if (entry->next != &mmc_devices)
			printf("%c ", separator);
	}

	printf("\n");
}
*/
int get_mmc_num(void)
{
	return cur_dev_num;
}

int init_mmc()
{
	INIT_LIST_HEAD (&mmc_devices);
	struct mmc *mmc;
	uint8_t *buf;
	int ret,i;

	cur_dev_num = 0;
	buf = malloc(0x800);
#if 1
	for(i = 0; i < 3; i++) {
		mmc = (struct mmc *)(buf + sizeof(struct mmc) * i);
		mmc->dma_desc_addr = (uint32_t)(buf + 0x400);
		ret = imap_sdhc_initialize(i, mmc);
		if(ret) return ret;
	}
#else
		mmc = (struct mmc *)(buf + sizeof(struct mmc) * 0);
		mmc->dma_desc_addr = (uint32_t)(buf + 0x400);
		ret = imap_sdhc_initialize(0, mmc);
		if(ret) return ret;
#endif
	return 0;
}
/*
int mmc_capacity(int dev_num){
	struct mmc* mmc = find_mmc_device(dev_num); 
	if(mmc) return  mmc->block_dev.lba;  //blocks in capacity
	else return -1;
}
*/
int  mmc_blksz(int dev_num){                            
	    struct mmc* mmc = find_mmc_device(dev_num);           
		    if(mmc) return  mmc->block_dev.blksz;  //blocks in capacity
			    else return -1;                                       
} 
                                                     
int mmc0_vs_align(void) { return  mmc_blksz(0);}
int mmc1_vs_align(void) { return  mmc_blksz(1);}
//int mmc2_vs_align(void) { return  mmc_blksz(2);}
int mmc0_vs_reset(void) { return mmc_init(0); }
int mmc1_vs_reset(void) { return mmc_init(1); }
//int mmc2_vs_reset(void) { return mmc_init(2); }
//loff_t mmc0_vs_capacity(void){return mmc_capacity(0);}
//loff_t mmc1_vs_capacity(void){return mmc_capacity(1);}
//loff_t mmc2_vs_capacity(void){return mmc_capacity(2);}
int mmc0_vs_read(uint8_t *buf, loff_t start,
			int length, int x) { return (length >> 9) * mmc_bread(0, start >> 9, length >> 9, buf); }
int mmc1_vs_read(uint8_t *buf, loff_t start,
			int length, int x) { return (length >> 9) * mmc_bread(1, start >> 9, length >> 9, buf); }
int mmc2_vs_read(uint8_t *buf, loff_t start,
			int length, int x) { return (length >> 9) * mmc_bread(2, start >> 9, length >> 9, buf); }

//int mmc0_vs_write(uint8_t *buf, loff_t start,
//			int length, int x) { return (length >> 9) * mmc_bwrite(0, start >> 9, length >> 9, buf); }
//int mmc1_vs_write(uint8_t *buf, loff_t start,
//			int length, int x) { return (length >> 9) * mmc_bwrite(1, start >> 9, length >> 9, buf); }
//int mmc2_vs_write(uint8_t *buf, loff_t start,
//			int length, int x) { return (length >> 9) * mmc_bwrite(2, start >> 9, length >> 9, buf); }

struct vstorage {
	char            name[8];
	int             (* align)(void);
	int             (* read)(uint8_t *buf, loff_t start, int length, int extra);
	int             (* reset)(void);
};

static struct vstorage support_vs[] = {
	/* mmc channel 0 */
	{
		.name  = "mmc0",
		.align = mmc0_vs_align,
		.read  = mmc0_vs_read,
		.reset = mmc0_vs_reset,
	},
	/* mmc channel 1 */
	{
		.name  = "mmc1",
		.align = mmc1_vs_align,
		.read  = mmc1_vs_read,
		.reset = mmc1_vs_reset,
	},
};

static struct vstorage *current_vs = NULL;

static int dev_num_valid(int dev_num)
{
	if(dev_num >= 0 && dev_num < ARRAY_SIZE(support_vs))
		return 1;
	return 0;
}

int vs_mmc_init_controller(int dev_num)
{
	struct mmc*	mmc;

	if(dev_num_valid(dev_num)) {
		mmc = find_mmc_device(dev_num);
		mmc->init(mmc);
	}

	return -1;
}

int vs_mmc_assign_by_devnum(int dev_num, int reset)  /* get current device and reset if need */
{
	int			ret = -1;
	
	current_vs = NULL;
	
	if(dev_num_valid(dev_num)) {
		current_vs = &support_vs[dev_num];
		if(reset){
			if(current_vs->reset){
				ret = current_vs->reset();
			}
			return ret;
		}else
			return 0;
	}

	printf("vs assign failed. id=%d\n", dev_num);
	return -1;
}

int support_vs_read(uint8_t *buf, int start,int length, int extra)
{
	uint32_t align, prelen, postlen;
	uint8_t *tmp = NULL;
	int ret = 0, read = 0;
				
	if(!length) return 0;
		
	align  = current_vs->align();
	prelen  = (start & (align - 1));
	if(prelen)
	  prelen = align - prelen;
	postlen = (length - prelen) & (align - 1);

	//printf("align: 0x%x, prelen: 0x%x, postlen: 0x%x, start: 0x%x, len: 0x%x\n",
				//align, prelen, postlen, start, length);
	if(prelen || postlen) {
		/* alloc buffer to hanle not aligned data */
		tmp = malloc(align);
		if(!tmp)
		  printf("no buffer to handle not aligned reading request\n");
	}

	if(prelen) {
		/* read the first align B */
		ret = current_vs->read(tmp, start & ~(align - 1), align, extra);
		if(ret < 0) {
			read = ret;
			goto __read_end__;
		}
		memcpy(buf, tmp + align - prelen, prelen);
		buf    += prelen;
		start  += prelen;
		length -= prelen;
		read  += ret;
	}

	/* middle */
	length -= postlen;
	if(length) {
		ret = current_vs->read(buf, start, length, extra);
		if(ret < 0) {
			read = ret;
			goto __read_end__;
		}
		buf   += length;
		start += length;
		read  += ret;
	}

	if(postlen) {
		ret = current_vs->read(tmp, start, align, extra);
		if(ret < 0) {
			read = ret;
			goto __read_end__;
		}
		memcpy(buf, tmp, postlen);
		read  += ret;
	}

__read_end__:
	if(prelen || postlen)
	  irf->free(tmp);
	return read;
}


