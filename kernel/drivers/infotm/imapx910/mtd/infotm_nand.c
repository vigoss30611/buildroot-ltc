// linux/drivers/infotmogic/nand/infotm_nand.c


#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/crc32.h>
#include <linux/fs.h>
#include <linux/reboot.h>
#include <linux/notifier.h>
#include <asm/uaccess.h>
#include <mach/rtcbits.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <mach/nand.h>
#include <mach/pad.h>

//#define CONFIG_INFOTM_NAND_ENV

static struct nand_ecclayout infotm_nand_oob = {
	.oobfree = {
		{.offset = 4,
		 .length = 16}}
};

static void infotm_nand_get_onfi_features(struct infotm_nand_chip *infotm_chip,  uint8_t *buf, int addr, int chipnr)
{
	struct nand_chip *chip = &infotm_chip->chip;
	struct mtd_info *mtd = &infotm_chip->mtd;
	int j;

	infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
	chip->cmd_ctrl(mtd, NAND_CMD_GET_FEATURES, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
	chip->cmd_ctrl(mtd, addr, NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
	infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);

	for (j=0; j<4; j++)
		buf[j] = chip->read_byte(mtd);

	return;
}

static void infotm_nand_set_onfi_features(struct infotm_nand_chip *infotm_chip,  uint8_t *buf, int addr, int chipnr)
{
	struct nand_chip *chip = &infotm_chip->chip;
	struct mtd_info *mtd = &infotm_chip->mtd;
	int j;

	infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
	chip->cmd_ctrl(mtd, NAND_CMD_SET_FEATURES, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
	chip->cmd_ctrl(mtd, addr, NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);

	for (j=0; j<4; j++)
		infotm_chip->infotm_nand_write_byte(infotm_chip, buf[j]);
	infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
	infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
	return;
}

static void infotm_nand_base_set_retry_level(struct infotm_nand_chip *infotm_chip, int chipnr, int retry_level)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	struct infotm_nand_para_save *infotm_nand_save;
	struct infotm_nand_retry_parameter *retry_parameter;
	int i = 0, set_level;

	set_level = retry_level;
	infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	if (infotm_chip->valid_chip[chipnr]) {
		retry_parameter = &infotm_chip->retry_parameter[chipnr];
		if (infotm_chip->mfr_type == NAND_MFR_HYNIX) {
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			chip->cmd_ctrl(mtd, HYNIX_SET_PARAMETER, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			//NAND_DEBUG("page %x retry level%d: ", infotm_chip->page_addr, set_level);
			for (i=0; i<retry_parameter->register_num; i++) {
				chip->cmd_ctrl(mtd, retry_parameter->register_addr[i], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
				infotm_chip->infotm_nand_write_byte(infotm_chip, retry_parameter->parameter_offset[set_level][i]);
				//NAND_DEBUG(" %x ", (uint8_t)retry_parameter->parameter_offset[set_level][i]);
			}
			//NAND_DEBUG("\n");
			chip->cmd_ctrl(mtd, 0x16, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
		} else if (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) {

			infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
			for (i=0; i<retry_parameter->register_num; i++) {
				chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
				chip->cmd_ctrl(mtd, SAMSUNG_SET_PARAMETER, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				infotm_chip->infotm_nand_write_byte(infotm_chip, (retry_parameter->register_addr[i] & 0xff));
				infotm_chip->infotm_nand_write_byte(infotm_chip, ((retry_parameter->register_addr[i] >> 8 ) & 0xff));
				infotm_chip->infotm_nand_write_byte(infotm_chip, retry_parameter->parameter_offset[set_level][i]);
			}
			infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
		} else if (infotm_chip->mfr_type == NAND_MFR_SANDISK) {

			infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
			if (set_level) {
				for (i=0; i<retry_parameter->register_num; i++) {
					chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
					chip->cmd_ctrl(mtd, SANDISK_DYNAMIC_PARAMETER1, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
					chip->cmd_ctrl(mtd, SANDISK_DYNAMIC_PARAMETER2, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
					chip->cmd_ctrl(mtd, SANDISK_SWITCH_LEGACY, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
					chip->cmd_ctrl(mtd, retry_parameter->register_addr[i], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
					infotm_chip->infotm_nand_write_byte(infotm_chip, retry_parameter->parameter_offset[set_level][i]);
				}
				chip->cmd_ctrl(mtd, SANDISK_DYNAMIC_READ_ENABLE, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			} else {
				chip->cmd_ctrl(mtd, SANDISK_DYNAMIC_READ_DISABLE, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			}
			infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
		} else if (infotm_chip->mfr_type == NAND_MFR_TOSHIBA) {

			infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
			if (set_level == 1) {
				chip->cmd_ctrl(mtd, TOSHIBA_RETRY_PRE_CMD1, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				chip->cmd_ctrl(mtd, TOSHIBA_RETRY_PRE_CMD2, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			}
			for (i=0; i<retry_parameter->register_num; i++) {
				chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
				chip->cmd_ctrl(mtd, TOSHIBA_SET_PARAMETER, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				chip->cmd_ctrl(mtd, retry_parameter->register_addr[i], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
				infotm_chip->infotm_nand_write_byte(infotm_chip, retry_parameter->parameter_offset[set_level][i]);
			}
			if (set_level == 0) {
				chip->cmd_ctrl(mtd, NAND_CMD_RESET, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			} else {
				chip->cmd_ctrl(mtd, TOSHIBA_READ_RETRY_START1, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				chip->cmd_ctrl(mtd, TOSHIBA_READ_RETRY_START2, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			}
			infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
		} else if (infotm_chip->mfr_type == NAND_MFR_MICRON) {
			infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
			chip->cmd_ctrl(mtd, NAND_CMD_SET_FEATURES, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

			chip->cmd_ctrl(mtd, retry_parameter->register_addr[0], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
			for (i=0; i<retry_parameter->register_num; i++)
				infotm_chip->infotm_nand_write_byte(infotm_chip, retry_parameter->parameter_offset[set_level][i]);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
		}
		if (imapx800_chip->vitual_fifo_start == 0)
			infotm_nand_save->retry_level[chipnr] = set_level;
	}
	return;
}

static int infotm_nand_get_eslc_para(struct infotm_nand_chip *infotm_chip)
{
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct nand_chip *chip = &infotm_chip->chip;
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct infotm_nand_eslc_parameter *eslc_parameter;
	int error = 0, i;
	uint8_t data;
	uint32_t pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));

	if ((infotm_nand_dev == NULL) || (infotm_nand_dev->resv3 == 0))
		return 0;
	eslc_parameter = (struct infotm_nand_eslc_parameter *)infotm_nand_dev->resv3;
	if (strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
		list_for_each_safe(l, n, &imapx800_chip->chip_list) {
			infotm_chip_temp = chip_list_to_imapx800(l);
			if (infotm_chip_temp != infotm_chip) {
				memcpy(&infotm_chip->eslc_parameter[0], &infotm_chip_temp->eslc_parameter[0], sizeof(struct infotm_nand_eslc_parameter));
				return 0;
			}
		}
	}
	NAND_DEBUG(KERN_WARNING,"infotm nand eslc table from uboot\n");
	eslc_parameter = &infotm_chip->eslc_parameter[0];
	memcpy(eslc_parameter, (struct infotm_nand_eslc_parameter *)infotm_nand_dev->resv3, sizeof(struct infotm_nand_eslc_parameter));
	if (eslc_parameter->parameter_set_mode == START_END) {
		infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
		chip->cmd_ctrl(mtd, HYNIX_GET_PARAMETER, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		for (i=0; i<eslc_parameter->register_num; i++) {
			chip->cmd_ctrl(mtd, eslc_parameter->register_addr[i], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
			data = chip->read_byte(mtd);
			eslc_parameter->parameter_offset[0][i] += data;
			eslc_parameter->parameter_offset[1][i] += data;
			NAND_DEBUG(KERN_WARNING,"para[%02d] %02x %02x\n", i, eslc_parameter->parameter_offset[0][i], eslc_parameter->parameter_offset[1][i]);
		}
		for(i=0; i<(pages_per_blk>>1); i++) {
			if(i < 2)
				eslc_parameter->paired_page_addr[i][0] = i;
			else
				eslc_parameter->paired_page_addr[i][0] = 2 * i - ((i % 2)? 3 : 2);
		}
	} else if (eslc_parameter->parameter_set_mode == START_ONLY) {
		infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
		chip->cmd_ctrl(mtd, HYNIX_GET_PARAMETER, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		for (i=0; i<eslc_parameter->register_num; i++) {
			chip->cmd_ctrl(mtd, eslc_parameter->register_addr[i], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
			data = chip->read_byte(mtd);
			eslc_parameter->parameter_offset[0][i] += data;
			NAND_DEBUG(KERN_WARNING,"para[%02d] %02x\n", i, eslc_parameter->parameter_offset[0][i]);
		}

		for(i=0; i<(pages_per_blk>>1); i++) {
			if(i < 2)
				eslc_parameter->paired_page_addr[i][0] = i;
			else
				eslc_parameter->paired_page_addr[i][0] = 2 * i - ((i % 2)? 3 : 2);
		}
	}

	return error;
}

static void infotm_nand_set_eslc_para(struct infotm_nand_chip *infotm_chip, int chipnr, int start)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	struct infotm_nand_eslc_parameter *eslc_parameter;
	uint8_t i;

	if (infotm_chip->valid_chip[chipnr]) {
		eslc_parameter = &infotm_chip->eslc_parameter[chipnr];
		if (infotm_chip->mfr_type == NAND_MFR_HYNIX) {
			if((eslc_parameter->parameter_set_mode == START_ONLY) && start ==0)
				return;
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			chip->cmd_ctrl(mtd, HYNIX_SET_PARAMETER, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			for (i=0; i<eslc_parameter->register_num; i++) {
				chip->cmd_ctrl(mtd, eslc_parameter->register_addr[i], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
				if(start == 1)
					infotm_chip->infotm_nand_write_byte(infotm_chip, eslc_parameter->parameter_offset[0][i]);
				else
					infotm_chip->infotm_nand_write_byte(infotm_chip, eslc_parameter->parameter_offset[1][i]);
			}
			chip->cmd_ctrl(mtd, HYNIX_ENABLE_PARAMETER, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
		}
	}
}

static int infotm_platform_hw_init(struct infotm_nand_chip *infotm_chip)
{
	return 0;
}

static void infotm_platform_adjust_timing(struct infotm_nand_chip *infotm_chip)
{
	return;
}

#if 0
static int infotm_nand_add_partition(struct infotm_nand_chip *infotm_chip)
{
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct mtd_partition *parts;
	int nr;

	parts = plat->platform_nand_data.chip.partitions;
	nr = plat->platform_nand_data.chip.nr_partitions;

	return add_mtd_partitions(mtd, parts, nr);
}
#endif

static void infotm_nand_select_chip(struct mtd_info *mtd, int chipnr)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);

	switch (chipnr) {
		case -1:
			infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
			infotm_chip->infotm_nand_release_controller(infotm_chip);
			break;
		case 0:
		case 1:
		case 2:
		case 3:
			infotm_chip->infotm_nand_get_controller(infotm_chip);
			infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
			break;

		default:
			BUG();
	}
	return;
}

static void infotm_platform_select_chip(struct infotm_nand_chip *infotm_chip, int chipnr)
{
	return;
}

static void infotm_platform_get_controller(struct infotm_nand_chip *infotm_chip)
{
	return;
}

static void infotm_platform_release_controller(struct infotm_nand_chip *infotm_chip)
{
	return;
}

static void infotm_platform_cmd_ctrl(struct infotm_nand_chip *infotm_chip, int cmd,  unsigned int ctrl)
{
	return;
}

static void infotm_platform_write_byte(struct infotm_nand_chip *infotm_chip, uint8_t data)
{
	return;
}

static int infotm_platform_wait_devready(struct infotm_nand_chip *infotm_chip, int chipnr)
{
	struct nand_chip *chip = &infotm_chip->chip;
	struct mtd_info *mtd = &infotm_chip->mtd;
	unsigned time_out_cnt = 0;
	int status;

	/* wait until command is processed or timeout occures */
	infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
	do {
		if (infotm_chip->ops_mode & INFOTM_MULTI_CHIP_SHARE_RB) {
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_STATUS, -1, -1, chipnr);
			status = (int)chip->read_byte(mtd);
			if (status & NAND_STATUS_READY)
				break;
		}
		else if (infotm_chip->ops_mode & INFOTM_CHIP_NONE_RB) {
			udelay(chip->chip_delay);
			break;
		}
		else {
			if (chip->dev_ready(mtd))
				break;
		}

	} while (time_out_cnt++ <= INFOTM_NAND_BUSY_TIMEOUT);

	if (time_out_cnt > INFOTM_NAND_BUSY_TIMEOUT)
		return 0;

	return 1;
}

static int infotm_nand_dev_ready(struct mtd_info *mtd)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	int rb_status = 1;

	if (infotm_chip->infotm_nand_wait_devready) {
		rb_status = infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
		infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
	} else {
		mdelay(10);
	}

	return rb_status;
}

static int infotm_nand_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;

	chip->pagebuf = -1;
	chip->read_buf(mtd, chip->buffers->databuf, len);
	if (memcmp(buf, chip->buffers->databuf, len))
		return -EFAULT;

	return 0;
}

static void infotm_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,  unsigned int ctrl)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);

	infotm_chip->infotm_nand_cmd_ctrl(infotm_chip, cmd, ctrl);
}

static int infotm_nand_wait(struct mtd_info *mtd, struct nand_chip *chip)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	int status[MAX_CHIP_NUM], state = chip->state, i = 0, valid_chip_num = 0, time_cnt = 0, chip_nr = 1, retry_cnt = 0, ret = 0;
	int tmp;

	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
		}
	}

	//if (state != FL_ERASING)
	chip_nr = infotm_chip->chip_num;
	for (i=0; i<chip_nr; i++) {
		if (infotm_chip->valid_chip[i]) {
RETRY:
			//active ce for operation chip and send cmd
			infotm_chip->infotm_nand_select_chip(infotm_chip, i);

			if (infotm_chip->ops_mode & INFOTM_MULTI_CHIP_SHARE_RB) {

				time_cnt = 0;
				while (time_cnt++ < 0x10000) {
					if (state == FL_ERASING)
						infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_STATUS_MULTI, -1, -1, i);
					else
						infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_STATUS, -1, -1, i);
					ret = infotm_chip->infotm_nand_read_byte(infotm_chip, (uint8_t *)(&status[i]), 1, 10);
					if (status[i] & NAND_STATUS_READY_MULTI)
						break;
					udelay(2);
				}
			} else {
				infotm_chip->infotm_nand_wait_devready(infotm_chip, i);

				if ((state == FL_ERASING) && (chip->options & NAND_IS_AND))
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_STATUS_MULTI, -1, -1, i);
				else
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_STATUS, -1, -1, i);

				if (valid_chip_num > 1) {
					if (i == 0)
						time_cnt = 0;
					else
						time_cnt = 100;
				} else {
					time_cnt = 100;
				}
				ret = infotm_chip->infotm_nand_read_byte(infotm_chip, (uint8_t *)(&status[i]), 1, time_cnt);
			}

			if ((ret == -EBUSY) && (!(status[i] & NAND_STATUS_READY))) {
				NAND_DEBUG(KERN_WARNING,"infotm nand wait status %x %x %d \n", status[i], infotm_chip->page_addr, i);
				if (++retry_cnt < 3)
					goto RETRY;
			}
			status[0] |= status[i];
		}
	}
	if (status[0] & NAND_STATUS_FAIL){
		NAND_DEBUG(KERN_WARNING,"infotm nand wait status %x %x \n", status[0], infotm_chip->page_addr);
#if NAND_TEST 
			tmp = imapx_pad_get_mode(PADSRANGE(0, 15));
			NAND_DEBUG(KERN_DEBUG,"[nand debug %d] pad mode 0x%x\n",__LINE__, tmp);
			NAND_DEBUG(KERN_DEBUG,"infotm_chip->clk = %ld, bus clk %ld\n", clk_get_rate(imapx800_chip->ecc_clk),  clk_get_rate(imapx800_chip->sys_clk));
#endif 
	}
	return status[0];
}

static void infotm_nand_base_command(struct infotm_nand_chip *infotm_chip, unsigned command, int column, int page_addr, int chipnr)
{
	struct nand_chip *chip = &infotm_chip->chip;
	struct mtd_info *mtd = &infotm_chip->mtd;
	int command_temp, pages_per_blk_shift, plane_page_addr = -1, plane_blk_addr = 0;
	pages_per_blk_shift = (chip->phys_erase_shift - chip->page_shift);

	if (page_addr != -1) {
		page_addr /= infotm_chip->plane_num;
		plane_page_addr = (page_addr & ((1 << pages_per_blk_shift) - 1));
		plane_blk_addr = (page_addr >> pages_per_blk_shift);
		plane_blk_addr = (plane_blk_addr << 1);
	}

	switch (command) {

		case NAND_CMD_STATUS_MULTI:
		case NAND_CMD_STATUS:
		case NAND_CMD_PAGEPROG:
		case NAND_CMD_ERASE1:
		case NAND_CMD_SEQIN:
		case NAND_CMD_READ0:
			chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
			break;

		default:
			break;
	}

	if (infotm_chip->plane_num == 2) {

		switch (command) {

			case NAND_CMD_READ0:
				if ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL) || (infotm_chip->mfr_type == NAND_MFR_SANDISK)) {
					command_temp = command;
				} else {
					command_temp = NAND_CMD_TWOPLANE_PREVIOS_READ;
					column = -1;
				}
				plane_page_addr |= (plane_blk_addr << pages_per_blk_shift);
				break;

			case NAND_CMD_TWOPLANE_READ1:
				if ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL)) {
					command_temp = -1;
					page_addr = -1;
					column = -1;
				} else {
					command_temp = NAND_CMD_READ0;
					plane_page_addr |= (plane_blk_addr << pages_per_blk_shift);
				}
				break;

			case NAND_CMD_TWOPLANE_READ2:
				if ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL)) {
					command_temp = NAND_CMD_PLANE2_READ_START;
				} else {
					command_temp = NAND_CMD_READ0;
				}
				plane_page_addr |= ((plane_blk_addr + 1) << pages_per_blk_shift);
				if (infotm_chip->mfr_type == NAND_MFR_SANDISK) {
					chip->cmd_ctrl(mtd, NAND_CMD_PLANE2_READ_START, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
					chip->cmd_ctrl(mtd, plane_page_addr, NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE);
					chip->cmd_ctrl(mtd, plane_page_addr >> 8, NAND_NCE | NAND_ALE);
					chip->cmd_ctrl(mtd, plane_page_addr >> 16, NAND_NCE | NAND_ALE);
					chip->cmd_ctrl(mtd, NAND_CMD_READSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
					infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
				}
				break;

			case NAND_CMD_SEQIN:
				command_temp = command;
				plane_page_addr |= (plane_blk_addr << pages_per_blk_shift);
				break;

			case NAND_CMD_TWOPLANE_WRITE2:
				if ((infotm_chip->mfr_type == NAND_MFR_HYNIX) || (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) || (infotm_chip->mfr_type == NAND_MFR_SANDISK))
					command_temp = command;
				else
					command_temp = NAND_CMD_TWOPLANE_WRITE2_MICRO;
				plane_page_addr |= ((plane_blk_addr + 1) << pages_per_blk_shift);
				break;

			case NAND_CMD_ERASE1:
				command_temp = command;
				plane_page_addr |= (plane_blk_addr << pages_per_blk_shift);
				break;

			case NAND_CMD_MULTI_CHIP_STATUS:
				command_temp = command;
				plane_page_addr |= (plane_blk_addr << pages_per_blk_shift);
				break;

			default:
				command_temp = command;
				break;

		}
		if (command_temp >= 0)
			chip->cmd_ctrl(mtd, command_temp & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		//if ((command_temp == NAND_CMD_SEQIN) || (command_temp == NAND_CMD_TWOPLANE_WRITE2) || (command_temp == NAND_CMD_READ0))
		//NAND_DEBUG(" NAND plane_page_addr: %x plane_blk_addr %x command: %x \n", plane_page_addr, plane_blk_addr, command);

		if (column != -1 || page_addr != -1) {
			int ctrl = NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE;

			/* Serially input address */
			if (column != -1) {
				/* Adjust columns for 16 bit buswidth */
				if (chip->options & NAND_BUSWIDTH_16)
					column >>= 1;
				chip->cmd_ctrl(mtd, column, ctrl);
				ctrl &= ~NAND_CTRL_CHANGE;
				chip->cmd_ctrl(mtd, column >> 8, ctrl);
			}
			if (page_addr != -1) {

				infotm_chip->real_page = plane_page_addr;
				chip->cmd_ctrl(mtd, plane_page_addr, ctrl);
				chip->cmd_ctrl(mtd, plane_page_addr >> 8, NAND_NCE | NAND_ALE);
				/* One more address cycle for devices > 128MiB */
				if (chip->chipsize > (128 << 20))
					chip->cmd_ctrl(mtd, plane_page_addr >> 16, NAND_NCE | NAND_ALE);
			}
		}

		if ((command == NAND_CMD_READ0) && ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL) || (infotm_chip->mfr_type == NAND_MFR_SANDISK))) {
			chip->cmd_ctrl(mtd, NAND_CMD_TWOPLANE_READ1_MICRO, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
		}

		switch (command) {

			case NAND_CMD_READ0:
				plane_page_addr = page_addr % (1 << pages_per_blk_shift);
				if ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL) || (infotm_chip->mfr_type == NAND_MFR_SANDISK)) {
					plane_page_addr |= ((plane_blk_addr + 1) << pages_per_blk_shift);
					command_temp = command;
					chip->cmd_ctrl(mtd, command_temp & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				} else {
					command_temp = NAND_CMD_TWOPLANE_PREVIOS_READ;
					column = -1;
					plane_page_addr |= ((plane_blk_addr + 1) << pages_per_blk_shift);
					chip->cmd_ctrl(mtd, command_temp & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				}
				break;

			case NAND_CMD_TWOPLANE_READ1:
				if ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL)) {
					command_temp = -1;
					page_addr = -1;
					column = -1;
				} else {
					command_temp = NAND_CMD_RNDOUT;
					page_addr = -1;
					chip->cmd_ctrl(mtd, command_temp & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				}
				break;

			case NAND_CMD_TWOPLANE_READ2:
				if ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL)) {
					page_addr = -1;
					column = -1;
				} else {
					command_temp = NAND_CMD_RNDOUT;
					page_addr = -1;
					chip->cmd_ctrl(mtd, command_temp & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				}
				break;

			case NAND_CMD_ERASE1:
				if ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_INTEL)) {
					command_temp = NAND_CMD_ERASE1_END;
					chip->cmd_ctrl(mtd, command_temp & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
					infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
				}

				command_temp = command;
				chip->cmd_ctrl(mtd, command_temp & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
				plane_page_addr = page_addr % (1 << pages_per_blk_shift);
				plane_page_addr |= ((plane_blk_addr + 1) << pages_per_blk_shift);
				break;

			default:
				column = -1;
				page_addr = -1;
				break;
		}

		if (column != -1 || page_addr != -1) {
			int ctrl = NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE;

			if (column != -1) {
				if (chip->options & NAND_BUSWIDTH_16)
					column >>= 1;
				chip->cmd_ctrl(mtd, column, ctrl);
				ctrl &= ~NAND_CTRL_CHANGE;
				if (command != NAND_CMD_READID)
					chip->cmd_ctrl(mtd, column >> 8, ctrl);
			}
			if (page_addr != -1) {

				infotm_chip->real_page = plane_page_addr;
				chip->cmd_ctrl(mtd, plane_page_addr, ctrl);
				chip->cmd_ctrl(mtd, plane_page_addr >> 8, NAND_NCE | NAND_ALE);
				if (chip->chipsize > (128 << 20))
					chip->cmd_ctrl(mtd, plane_page_addr >> 16, NAND_NCE | NAND_ALE);
			}
		}

		if ((command == NAND_CMD_RNDOUT) || (command == NAND_CMD_TWOPLANE_READ2))
			chip->cmd_ctrl(mtd, NAND_CMD_RNDOUTSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		else if ((command == NAND_CMD_TWOPLANE_READ1) && (command_temp >= 0)) {
			chip->cmd_ctrl(mtd, NAND_CMD_RNDOUTSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		} else if (command == NAND_CMD_READ0) {
			chip->cmd_ctrl(mtd, NAND_CMD_READSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		}
	} else {
		chip->cmd_ctrl(mtd, command & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

		if (column != -1 || page_addr != -1) {
			int ctrl = NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE;

			/* Serially input address */
			if (column != -1) {
				/* Adjust columns for 16 bit buswidth */
				if (chip->options & NAND_BUSWIDTH_16)
					column >>= 1;
				chip->cmd_ctrl(mtd, column, ctrl);
				ctrl &= ~NAND_CTRL_CHANGE;
				if (command != NAND_CMD_READID)
					chip->cmd_ctrl(mtd, column >> 8, ctrl);
			} 
			if (page_addr != -1) {

				infotm_chip->real_page = page_addr;
				chip->cmd_ctrl(mtd, page_addr, ctrl);
				chip->cmd_ctrl(mtd, page_addr >> 8, NAND_NCE | NAND_ALE);
				/* One more address cycle for devices > 128MiB */
				if (chip->chipsize > (128 << 20))
					chip->cmd_ctrl(mtd, page_addr >> 16, NAND_NCE | NAND_ALE);
			}
		}
		if (command == NAND_CMD_RNDOUT)
			chip->cmd_ctrl(mtd, NAND_CMD_RNDOUTSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		else if (command == NAND_CMD_READ0)
			chip->cmd_ctrl(mtd, NAND_CMD_READSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

		chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
	}

	/*
	 * program and erase have their own busy handlers
	 * status, sequential in, and deplete1 need no delay
	 */
	switch (command) {

		case NAND_CMD_CACHEDPROG:
		case NAND_CMD_PAGEPROG:
		case NAND_CMD_ERASE2:
			if (infotm_chip->infotm_nand_cmdfifo_start)
				infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 100);
		case NAND_CMD_ERASE1:
		case NAND_CMD_SEQIN:
		case NAND_CMD_RNDIN:
		case NAND_CMD_STATUS:
		case NAND_CMD_DEPLETE1:
			return;

			/*
			 * read error status commands require only a short delay
			 */
		case NAND_CMD_STATUS_ERROR:
		case NAND_CMD_STATUS_ERROR0:
		case NAND_CMD_STATUS_ERROR1:
		case NAND_CMD_STATUS_ERROR2:
		case NAND_CMD_STATUS_ERROR3:
			udelay(chip->chip_delay);
			return;

		case NAND_CMD_RESET:
			if (!infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr))
				NAND_DEBUG (KERN_WARNING,"couldn`t found selected chip: %d ready\n", chipnr);
			//chip->cmd_ctrl(mtd, NAND_CMD_STATUS, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			//chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
			if (infotm_chip->infotm_nand_cmdfifo_start)
				infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
			//while (!(chip->read_byte(mtd) & NAND_STATUS_READY)) ;
			return;

		default:
			/*
			 * If we don't have access to the busy pin, we apply the given
			 * command delay
			 */
			break;
	}

	/* Apply this short delay always to ensure that we do wait tWB in
	 * any case on any machine. */
	ndelay(100);
}

static void infotm_nand_command(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct nand_chip *chip = &infotm_chip->chip;
	int i = 0, valid_page_num = 1, internal_chip;

	if (page_addr != -1) {
		valid_page_num = (mtd->writesize >> chip->page_shift);
		valid_page_num /= infotm_chip->plane_num;

		infotm_chip->page_addr = page_addr / valid_page_num;
		if (unlikely(infotm_chip->page_addr >= infotm_chip->internal_page_nums)) {
			internal_chip = infotm_chip->page_addr / infotm_chip->internal_page_nums; 
			infotm_chip->page_addr -= infotm_chip->internal_page_nums;
			infotm_chip->page_addr |= (1 << infotm_chip->internal_chip_shift) * internal_chip;
		}
	}

	/* Emulate NAND_CMD_READOOB */
	/*if (command == NAND_CMD_READOOB) {
	  command = NAND_CMD_READ0;
	  infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
	  infotm_chip->infotm_nand_command(infotm_chip, command, column, infotm_chip->page_addr, 0);
	  return;
	  }*/
	if ((command == NAND_CMD_PAGEPROG) || (command == NAND_CMD_READ0)
			|| (command == NAND_CMD_SEQIN) || (command == NAND_CMD_READOOB))
		return;

	//for (i=0; i<infotm_chip->chip_num; i++) {
	if (infotm_chip->valid_chip[i]) {
		//active ce for operation chip and send cmd
		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		infotm_chip->infotm_nand_command(infotm_chip, command, column, page_addr, i);
	}
	//}

	return;
}

static void infotm_nand_erase_cmd(struct mtd_info *mtd, int page)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct nand_chip *chip = mtd->priv;
	unsigned pages_per_blk_shift = (chip->phys_erase_shift - chip->page_shift);
	unsigned vt_page_num, i = 0, j = 0, internal_chipnr = 1, page_addr, valid_page_num;

	vt_page_num = (mtd->writesize / (1 << chip->page_shift));
	vt_page_num *= (1 << pages_per_blk_shift);
	if (page % vt_page_num)
		return;

	if ((infotm_chip->subpage_cache_page >= (page / vt_page_num)) && (infotm_chip->subpage_cache_page < (page / vt_page_num + (1 << pages_per_blk_shift)))) {
		infotm_chip->subpage_cache_page = -1;
		memset(infotm_chip->subpage_cache_status, 0, MAX_CHIP_NUM*MAX_PLANE_NUM);
	}

	/* Send commands to erase a block */
	valid_page_num = (mtd->writesize >> chip->page_shift);
	valid_page_num /= infotm_chip->plane_num;

	infotm_chip->page_addr = page / valid_page_num;
	if (unlikely(infotm_chip->page_addr >= infotm_chip->internal_page_nums)) {
		internal_chipnr = infotm_chip->page_addr / infotm_chip->internal_page_nums;
		infotm_chip->page_addr -= infotm_chip->internal_page_nums;
		infotm_chip->page_addr |= (1 << infotm_chip->internal_chip_shift) * internal_chipnr;
	}

	if (unlikely(infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE))
		internal_chipnr = infotm_chip->internal_chipnr;
	else
		internal_chipnr = 1;

	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {

			infotm_chip->infotm_nand_select_chip(infotm_chip, i);
			page_addr = infotm_chip->page_addr;
			for (j=0; j<internal_chipnr; j++) {
				if (j > 0) {
					page_addr = infotm_chip->page_addr;
					page_addr |= (1 << infotm_chip->internal_chip_shift) * j;
				}

				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE1, -1, page_addr, i);
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE2, -1, -1, i);
			}
		}
	}

	return ;
}

static int infotm_platform_hwecc_correct(struct infotm_nand_chip *infotm_chip, unsigned size, unsigned oob_size)
{
	return 0;
}

static int infotm_platform_dma_write(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode,  unsigned char *sys_info_buf, int sys_info_len, int secc_mode, int prog_type)
{
	return 0;		 
}

static int infotm_platform_dma_read(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode,  unsigned char *sys_info_buf, int sys_info_len, int secc_mode)
{
	return 0;
}

static int infotm_nand_oob_check(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *oob_check_buf, int oob_copy_num)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	int crc_oob, i, j, oob_fixed = 0;
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;
	uint8_t *oob_buf = oob_check_buf;

	for (i=0; i<oob_copy_num; i++) {

		oob_buf = oob_check_buf + i * nand_oobavail_size;
		crc_oob = (crc32((0 ^ 0xffffffffL), oob_buf + PAGE_MANAGE_SIZE, mtd->oobavail) ^ 0xffffffffL);
		if ((crc_oob != *(unsigned *)oob_buf)) {
			NAND_DEBUG(KERN_WARNING,"infotm oob not same plane%d %x %x %x \n", i, infotm_chip->page_addr, crc_oob, *(unsigned *)oob_buf);
			for (j=0; j<nand_oobavail_size; j++)
				NAND_DEBUG(KERN_WARNING,"%02hhx ", oob_buf[j]);
			NAND_DEBUG(KERN_WARNING,"\n");
			mtd->ecc_stats.failed++;
		} else {
			if (i > 0)
				memcpy(oob_check_buf, oob_buf, nand_oobavail_size);
			oob_fixed = 1;
			break;
		}
	}

	return oob_fixed ? 0 : -EFAULT;
}

static void infotm_nand_dma_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);

	infotm_chip->infotm_nand_dma_read(infotm_chip, buf, len, -1, NULL, 0, -1);
}

static void infotm_nand_dma_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);

	infotm_chip->infotm_nand_dma_write(infotm_chip, (unsigned char *)buf, len, -1, NULL, 0, -1, -1);
}

static int infotm_nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int page)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned nand_page_size = infotm_chip->page_size;
	unsigned nand_oob_size = infotm_chip->oob_size;
	uint8_t *oob_buf = chip->oob_poi;
	int i, error = 0, j = 0, page_addr, internal_chipnr = 1;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	chip->pagebuf = -1;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {

			page_addr = infotm_chip->page_addr;
			infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, 0, page_addr, i);

			for (j=0; j<internal_chipnr; j++) {

				if (j > 0) {
					page_addr = infotm_chip->page_addr;
					page_addr |= (1 << infotm_chip->internal_chip_shift) * j;
					infotm_chip->infotm_nand_select_chip(infotm_chip, i);
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, 0, page_addr, i);
				}

				if (!infotm_chip->infotm_nand_wait_devready(infotm_chip, i)) {
					NAND_DEBUG (KERN_WARNING,"couldn`t found selected chip: %d ready\n", i);
					error = -EBUSY;
					goto exit;
				}

				if (infotm_chip->plane_num == 2) {

					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ1, 0x00, page_addr, i);
					chip->read_buf(mtd, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					memcpy(buf, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					memcpy(oob_buf, chip->buffers->databuf + nand_page_size, nand_oob_size);

					oob_buf += nand_oob_size;
					buf += (nand_page_size + nand_oob_size);

					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ2, 0x00, page_addr, i);
					chip->read_buf(mtd, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					memcpy(buf, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					memcpy(oob_buf, chip->buffers->databuf + nand_page_size, nand_oob_size);

					oob_buf += nand_oob_size;
					buf += (nand_page_size + nand_oob_size);
				} else if (infotm_chip->plane_num == 1) {

					chip->read_buf(mtd, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					memcpy(buf, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					memcpy(oob_buf, chip->buffers->databuf + nand_page_size, nand_oob_size);

					oob_buf += nand_oob_size;
					buf += nand_page_size;
				} else {
					error = -ENODEV;
					goto exit;
				}
			}
		}
	}

exit:
	return error;
}

static void infotm_nand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned nand_page_size = infotm_chip->page_size;
	unsigned nand_oob_size = infotm_chip->oob_size;
	uint8_t *oob_buf = chip->oob_poi;
	int i, j = 0, page_addr, internal_chipnr = 1;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	chip->pagebuf = -1;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {

			if (i == 0)
				infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
			else
				infotm_chip->infotm_nand_select_chip(infotm_chip, i);

			page_addr = infotm_chip->page_addr;
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_SEQIN, 0, page_addr, i);
			for (j=0; j<internal_chipnr; j++) {

				if (j > 0) {
					page_addr = infotm_chip->page_addr;
					page_addr |= (1 << infotm_chip->internal_chip_shift) * j;
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_SEQIN, 0, page_addr, i);
				}

				if (infotm_chip->plane_num == 2) {

					memcpy(chip->buffers->databuf, buf, nand_page_size);
					memcpy(chip->buffers->databuf + nand_page_size, oob_buf, nand_oob_size);
					chip->write_buf(mtd, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_DUMMY_PROGRAM, -1, -1, i);

					oob_buf += nand_oob_size;
					buf += nand_page_size;

					memcpy(chip->buffers->databuf, buf, nand_page_size);
					memcpy(chip->buffers->databuf + nand_page_size, oob_buf, nand_oob_size);
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_WRITE2, 0x00, page_addr, i);
					chip->write_buf(mtd, chip->buffers->databuf, (nand_page_size + nand_oob_size));
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_PAGEPROG, -1, -1, i);

					oob_buf += nand_oob_size;
					buf += nand_page_size;
				} else if (infotm_chip->plane_num == 1) {

					memcpy(chip->buffers->databuf, buf, nand_page_size);
					memcpy(chip->buffers->databuf + nand_page_size, oob_buf, nand_oob_size);
					infotm_chip->infotm_nand_dma_write(infotm_chip, (unsigned char *)chip->buffers->databuf, (nand_page_size + nand_oob_size), -1, NULL, 0, -1, NAND_CMD_PAGEPROG);

					oob_buf += nand_oob_size;
					buf += nand_page_size;
				} else {
					goto exit;
				}
			}
		}
	}

exit:
	return ;
}

static int infotm_nand_read_oob_raw_onechip(struct mtd_info *mtd, struct nand_chip *chip, unsigned char *oob_buffer, int page, int readlen, int chipnr)
{
	int32_t i = chipnr, j = 0, page_addr, internal_chipnr = 1;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = (mtd->oobsize <= BASIC_RAW_OOB_SIZE) ? mtd->oobsize : BASIC_RAW_OOB_SIZE;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	if (infotm_chip->valid_chip[i]) {

		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		if (chip->cmdfunc == infotm_nand_command) {
			page_addr = page;
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, nand_page_size, page_addr, i);
		} else {
			if (i == 0)
				page_addr = page;
			else
				page_addr = infotm_chip->page_addr;
			chip->cmdfunc(mtd, NAND_CMD_READ0, nand_page_size, page_addr);
		}

		for (j=0; j<internal_chipnr; j++) {

			if (j > 0) {
				page_addr = page;
				page_addr |= (1 << infotm_chip->internal_chip_shift) * j;
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, nand_page_size, page_addr, i);
			}

			if (!infotm_chip->infotm_nand_wait_devready(infotm_chip, i)) {
				NAND_DEBUG (KERN_WARNING,"read oob couldn`t found selected chip: %d ready\n", i);
				goto exit;
			}

			if (infotm_chip->plane_num == 2) {

				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ1, nand_page_size, page_addr, i);
				chip->read_buf(mtd, oob_buffer, nand_oobavail_size);

				oob_buffer += nand_oobavail_size;

				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ2, nand_page_size, page_addr, i);
				chip->read_buf(mtd, oob_buffer, nand_oobavail_size);
			} else {
				chip->read_buf(mtd, oob_buffer, nand_oobavail_size);
			}

			oob_buffer += nand_oobavail_size;
		}
	} else {
		mdelay(100);
		goto exit;
	}

exit:
	return readlen;
}

static int infotm_nand_read_page_hwecc_onechip(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, uint8_t *oob_buf, int page, int chipnr)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;
	int error = 0, i = chipnr, stat = 0, j = 0, page_addr, internal_chipnr = 1, column;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;
	if (nand_page_size > chip->ecc.steps * chip->ecc.size) {
		nand_page_size = chip->ecc.steps * chip->ecc.size;
	}

	if (infotm_chip->valid_chip[i]) {

		page_addr = page;
		column = infotm_chip->page_size + chip->ecc.layout->oobavail + PAGE_MANAGE_SIZE;

		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, column, page_addr, i);

		for (j=0; j<internal_chipnr; j++) {

			if (j > 0) {
				page_addr = page;
				page_addr |= (1 << infotm_chip->internal_chip_shift) * j;
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, column, page_addr, i);
			}

			if (!infotm_chip->infotm_nand_wait_devready(infotm_chip, i)) {
				NAND_DEBUG (KERN_WARNING,"read couldn`t found selected chip: %d ready\n", i);
				error = -EBUSY;
				goto exit;
			}

			if (infotm_chip->plane_num == 2) {

				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ1, column, page_addr, i);
				error = infotm_chip->infotm_nand_dma_read(infotm_chip, buf, nand_page_size, infotm_chip->ecc_mode, oob_buf, ((oob_buf != NULL)?nand_oobavail_size:0), ((oob_buf != NULL)?infotm_chip->secc_mode:-1));
				stat = infotm_chip->infotm_nand_hwecc_correct(infotm_chip, nand_page_size, ((oob_buf != NULL)?nand_oobavail_size:0));
				if (stat < 0) {
					mtd->ecc_stats.failed++;
					//NAND_DEBUG("infotm nand read data ecc plane0 failed at page %d chip %d\n", page_addr, i);
				} else {
					mtd->ecc_stats.corrected += stat;
				}
				if (error) {
					//NAND_DEBUG("infotm nand read data ecc plane0 failed at page %d chip %d\n", page_addr, i);
					goto exit;
				}

				if (oob_buf)
					oob_buf += nand_oobavail_size;
				buf += nand_page_size;

				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ2, column, page_addr, i);
				error = infotm_chip->infotm_nand_dma_read(infotm_chip, buf, nand_page_size, infotm_chip->ecc_mode, oob_buf, ((oob_buf != NULL)?nand_oobavail_size:0), ((oob_buf != NULL)?infotm_chip->secc_mode:-1));
				stat = infotm_chip->infotm_nand_hwecc_correct(infotm_chip, nand_page_size, ((oob_buf != NULL)?nand_oobavail_size:0));
				if (stat < 0) {
					mtd->ecc_stats.failed++;
					//NAND_DEBUG("infotm nand read data ecc plane1 failed at page %d chip %d\n", page_addr, i);
				} else {
					mtd->ecc_stats.corrected += stat;
				}
				if (error) {
					//NAND_DEBUG("infotm nand read data dma plane1 failed at page %d chip %d\n", page_addr, i);
					goto exit;
				}

				if (oob_buf)
					oob_buf += nand_oobavail_size;
				buf += nand_page_size;

			} else if (infotm_chip->plane_num == 1) {

				error = infotm_chip->infotm_nand_dma_read(infotm_chip, buf, nand_page_size, infotm_chip->ecc_mode, oob_buf, ((oob_buf != NULL)?nand_oobavail_size:0), ((oob_buf != NULL)?infotm_chip->secc_mode:-1));
				stat = infotm_chip->infotm_nand_hwecc_correct(infotm_chip, nand_page_size, ((oob_buf != NULL)?nand_oobavail_size:0));
				if (stat < 0) {
					mtd->ecc_stats.failed++;
					//NAND_DEBUG("infotm nand read data ecc failed at blk %d chip %d\n", (page_addr >> pages_per_blk_shift), i);
				} else {
					mtd->ecc_stats.corrected += stat;
				}
				if (error) {
					//NAND_DEBUG("infotm nand read data dma plane failed at page %d chip %d\n", page_addr, i);
					goto exit;
				}

				if (oob_buf)
					oob_buf += nand_oobavail_size;
				buf += nand_page_size;
			}
			else {
				error = -ENODEV;
				mdelay(100);
				goto exit;
			}
		}
	}

exit:
	return error;  //do not return error when failed
}

static int infotm_nand_read_subpage(struct mtd_info *mtd, struct nand_chip *chip, uint32_t data_offs, uint32_t readlen, uint8_t *bufpoi)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	uint8_t *oob_buf = chip->oob_poi;
	struct mtd_ecc_stats stats;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;
	int error = 0, i = 0, j, k, internal_chipnr = 1, retry_level_support = 0, retry_level = 0, retry_count = 0;
	int chip_page_start[MAX_CHIP_NUM], chip_page_num[MAX_CHIP_NUM], page_cached = 0;
	int page_start, page_end, page_addr, chip_start, chip_end, valid_chip_num, data_offset = 0, chip_valid_page;
	int pages_per_blk_shift, plane_page_addr = -1, plane_blk_addr = 0, plane_num, read_page;
	unsigned char *data_buf;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		retry_level_support = infotm_chip->retry_level_support;

	chip->pagebuf = -1;
	page_start = data_offs / nand_page_size;
	page_end = (readlen + nand_page_size - 1) / nand_page_size;
	if (data_offs % nand_page_size)
		page_end += page_start;
	else
		page_end += page_start - 1;
	chip_start = page_start / infotm_chip->plane_num * internal_chipnr;
	chip_end = page_end / infotm_chip->plane_num * internal_chipnr;
	chip_valid_page = infotm_chip->plane_num * internal_chipnr;

	//NAND_DEBUG("infotm_nand_read_subpage %d %d %d %d\n", chip_start, chip_end, page_start, page_end);
	valid_chip_num = -1;
	for (i=0; i<infotm_chip->chip_num; i++) {
		chip_page_start[i] = 0;
		chip_page_num[i] = 0;
		if (infotm_chip->valid_chip[i] && (chip_start <= chip_end)) {
			valid_chip_num++;
			if (valid_chip_num == chip_start) {
				chip_page_start[i] = page_start % chip_valid_page;
				if (page_end >= chip_valid_page * (valid_chip_num + 1))
					chip_page_num[i] = (chip_valid_page - chip_page_start[i]);
				else
					chip_page_num[i] = (page_end + 1 - chip_valid_page * valid_chip_num - chip_page_start[i]);
				page_start += chip_page_num[i];
				chip_start++;
			}
		}
	}

	page_addr = infotm_chip->page_addr * (mtd->writesize >> chip->page_shift) / infotm_chip->plane_num;
	if (infotm_chip->subpage_cache_page == page_addr / (mtd->writesize >> chip->page_shift)) {
		page_cached = 1;
		for (i=0; i<infotm_chip->chip_num; i++) {

			if (infotm_chip->valid_chip[i] && chip_page_num[i]) {
				for (j=chip_page_start[i]; j<chip_page_start[i]+chip_page_num[i]; j++) {
					if (infotm_chip->subpage_cache_status[i][j] == 0)
						break;
				}
				if (j < chip_page_start[i]+chip_page_num[i]) {
					page_cached = 0;
					break;
				}
			}
		}
	}
	if (page_cached == 1) {
		//NAND_DEBUG("_read_subpage found cached %d %d %d \n", page_start, page_end, infotm_chip->subpage_cache_page);
		memcpy(bufpoi, infotm_chip->subpage_cache_buf, mtd->writesize);
		return 0;
	}

	memset(infotm_chip->subpage_cache_status, 0, MAX_CHIP_NUM*MAX_PLANE_NUM);
	for (i=0; i<infotm_chip->chip_num; i++) {

		if (infotm_chip->valid_chip[i] && chip_page_num[i]) {
			for (j=chip_page_start[i]; j<chip_page_start[i]+chip_page_num[i]; j++) {
				infotm_chip->subpage_cache_status[i][j] = 1;
			}
		}
	}

	data_buf = infotm_chip->subpage_cache_buf;
	memset(data_buf, 0xff, mtd->writesize);
	stats = mtd->ecc_stats;

	page_addr = infotm_chip->page_addr;
	pages_per_blk_shift = (chip->phys_erase_shift - chip->page_shift);
	plane_num = infotm_chip->plane_num;
	if (infotm_chip->plane_num == 2) {
		page_addr /= infotm_chip->plane_num;
		plane_page_addr = (page_addr & ((1 << pages_per_blk_shift) - 1));
		plane_blk_addr = (page_addr >> pages_per_blk_shift);
		plane_blk_addr = (plane_blk_addr << 1);
		infotm_chip->plane_num = 1;
	} else {
		plane_page_addr = (page_addr & ((1 << pages_per_blk_shift) - 1));
		plane_blk_addr = (page_addr >> pages_per_blk_shift);
	}

	for (i=0; i<infotm_chip->chip_num; i++) {

		if (infotm_chip->valid_chip[i]) {
			data_offset = chip_page_start[i] * nand_page_size;
			if (!chip_page_num[i]) {
				data_buf += nand_page_size * plane_num * internal_chipnr;
				//oob_buf += nand_oobavail_size * plane_num * internal_chipnr;
				continue;
			}

			for (read_page=0; read_page<chip_page_num[i]; read_page++) {

				retry_count = 0;
				do {
					mtd->ecc_stats.failed = stats.failed;
					//error = infotm_nand_read_subpage_onechip(mtd, chip, page_addr, chip_page_start[i], chip_page_num[i], data_buf+data_offset, i);
					page_addr = (plane_page_addr | ((plane_blk_addr + chip_page_start[i] + read_page) << pages_per_blk_shift));
					error = infotm_nand_read_page_hwecc_onechip(mtd, chip, data_buf + data_offset, oob_buf, page_addr, i);
					if (error) {
						NAND_DEBUG(KERN_WARNING,"infotm nand read page error %d\n", error);
						goto exit;
					}
					if ((mtd->ecc_stats.failed - stats.failed) && (retry_count == 0)) {
						memset(oob_buf, 0x00, nand_oobavail_size);
						infotm_nand_read_oob_raw_onechip(mtd, chip, oob_buf, page_addr, nand_oobavail_size, i);
						for (j=0; j<nand_oobavail_size; j++) {
							//NAND_DEBUG("%02hhx ", oob_buf[j]);
							if (oob_buf[j] != 0xff)
								break;
						}
						//NAND_DEBUG("\n\n ");
						if (j >= nand_oobavail_size) {
							mtd->ecc_stats.failed = stats.failed;
							memset(data_buf, 0xff, nand_page_size * infotm_chip->plane_num * internal_chipnr);
							memset(oob_buf, 0xff, nand_oobavail_size * infotm_chip->plane_num * internal_chipnr);
						} else if (retry_level_support == 0) {
							NAND_DEBUG(KERN_WARNING,"read page ecc error %x %d \n ", page_addr, i);
							for (k=0; k<nand_oobavail_size; k++) {
								NAND_DEBUG(KERN_WARNING,"%02hhx ", oob_buf[k]);
							}
							NAND_DEBUG(KERN_WARNING,"\n ");
						}
					}
					if ((mtd->ecc_stats.failed - stats.failed) && retry_level_support) {
						if (retry_count > retry_level_support)
							break;
						retry_level = infotm_chip->retry_level[i] + 1;
						if (retry_level > retry_level_support)
							retry_level = 0;
						infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
						list_for_each_safe(l, n, &imapx800_chip->chip_list) {
							infotm_chip_temp = chip_list_to_imapx800(l);
							infotm_chip_temp->retry_level[i] = retry_level;
						}
						if (retry_count == retry_level_support)
							break;
						retry_count++;
					}

				} while (retry_level_support && (mtd->ecc_stats.failed - stats.failed));
				if (mtd->ecc_stats.failed - stats.failed) {
					NAND_DEBUG(KERN_WARNING,"read subpage ecc retry failed %x %d %d %d %d \n ", page_addr, i, retry_level, infotm_chip->ecc_status, infotm_chip->secc_status);
					break;
				}
				if ((retry_count > 0) && ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) || (infotm_chip->mfr_type == NAND_MFR_SANDISK) || (infotm_chip->mfr_type == NAND_MFR_TOSHIBA))) {
					retry_level = 0;
					infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
					list_for_each_safe(l, n, &imapx800_chip->chip_list) {
						infotm_chip_temp = chip_list_to_imapx800(l);
						infotm_chip_temp->retry_level[i] = retry_level;
					}
				}

				data_offset += nand_page_size * infotm_chip->plane_num * internal_chipnr;
				//oob_buf += nand_oobavail_size * infotm_chip->plane_num * internal_chipnr;
			}
			data_buf += nand_page_size * plane_num * internal_chipnr;
		}
	}

	memcpy(bufpoi, infotm_chip->subpage_cache_buf, mtd->writesize);
	infotm_chip->subpage_cache_page = page_addr / (mtd->writesize >> chip->page_shift);
exit:
	infotm_chip->plane_num = plane_num;
	return error;
}

static int infotm_nand_read_page_hwecc_multiplane(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int page, int oob_len)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	uint8_t *oob_buf = chip->oob_poi;
	uint8_t *data_buf = buf;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct mtd_ecc_stats stats;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;
	int error = 0, i = 0, j, k, valid_chip_num = 0, page_addr, internal_chipnr = 1, retry_level_support = 0, retry_level = 0, retry_count = 0;
	//int pages_per_blk_shift, plane_page_addr = -1, plane_blk_addr = 0, plane_num, read_plane = 0;
	//struct timespec ts_write_start, ts_write_end;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		retry_level_support = infotm_chip->retry_level_support;

	page_addr = infotm_chip->page_addr;
	stats = mtd->ecc_stats;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
			retry_count = 0;
			do {
				mtd->ecc_stats.failed = stats.failed;
				error = infotm_nand_read_page_hwecc_onechip(mtd, chip, data_buf, ((oob_len > 0) ? oob_buf : NULL), page_addr, i);
				if (error) {
					NAND_DEBUG(KERN_WARNING,"infotm nand read page error %d\n", error);
					goto exit;
				}
				if ((mtd->ecc_stats.failed - stats.failed) && (retry_count == 0)) {
					memset(oob_buf, 0x00, nand_oobavail_size);
					infotm_nand_read_oob_raw_onechip(mtd, chip, oob_buf, page_addr, nand_oobavail_size, i);
					for (j=0; j<nand_oobavail_size; j++) {
						//NAND_DEBUG("%02hhx ", oob_buf[j]);
						if (oob_buf[j] != 0xff)
							break;
					}
					//NAND_DEBUG("\n\n ");
					if (j >= nand_oobavail_size) {
						mtd->ecc_stats.failed = stats.failed;
						memset(data_buf, 0xff, nand_page_size * infotm_chip->plane_num * internal_chipnr);
						memset(oob_buf, 0xff, nand_oobavail_size * infotm_chip->plane_num * internal_chipnr);
					} else if (retry_level_support == 0) {
						NAND_DEBUG(KERN_WARNING,"read page ecc error %x %d \n ", page, i);
						for (k=0; k<nand_oobavail_size; k++) {
							NAND_DEBUG(KERN_WARNING,"%02hhx ", oob_buf[k]);
						}
						NAND_DEBUG(KERN_WARNING,"\n ");
					}
				}
				if ((mtd->ecc_stats.failed - stats.failed) && retry_level_support) {
					if (retry_count > retry_level_support)
						break;
					retry_level = infotm_chip->retry_level[i] + 1;
					if (retry_level > retry_level_support)
						retry_level = 0;
					infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
					list_for_each_safe(l, n, &imapx800_chip->chip_list) {
						infotm_chip_temp = chip_list_to_imapx800(l);
						infotm_chip_temp->retry_level[i] = retry_level;
					}
					if (retry_count == retry_level_support)
						break;
					retry_count++;
				}

			} while (retry_level_support && (mtd->ecc_stats.failed - stats.failed));
			if (mtd->ecc_stats.failed - stats.failed) {
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, i);
				infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
				infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
				NAND_DEBUG(KERN_WARNING,"read page multiplane retry failed %x %d %d %d \n ", page_addr, i, infotm_chip->ecc_status, infotm_chip->secc_status);
				goto exit;
			} else if (retry_count > 0) {
				mtd->ecc_stats.corrected++;
				NAND_DEBUG(KERN_WARNING,"read page multiplane retry success %x %d \n ", page_addr, i);
			}
			if ((retry_count > 0) && ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) || (infotm_chip->mfr_type == NAND_MFR_SANDISK) || (infotm_chip->mfr_type == NAND_MFR_TOSHIBA))) {
				retry_level = 0;
				infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
				list_for_each_safe(l, n, &imapx800_chip->chip_list) {
					infotm_chip_temp = chip_list_to_imapx800(l);
					infotm_chip_temp->retry_level[i] = retry_level;
				}
			}

			if (oob_len > 0)
				oob_buf += nand_oobavail_size * infotm_chip->plane_num * internal_chipnr;
			data_buf += nand_page_size * infotm_chip->plane_num * internal_chipnr;
		}
	}
	if (oob_len > 0)
		infotm_nand_oob_check(mtd, chip, chip->oob_poi, infotm_chip->plane_num * internal_chipnr * valid_chip_num);

	//ktime_get_ts(&ts_write_end);
	//NAND_DEBUG("infotm nand read %d %d \n", (ts_write_end.tv_sec - ts_write_start.tv_sec), (ts_write_end.tv_nsec - ts_write_start.tv_nsec));
exit:
	return error;
}

static int infotm_nand_read_page_hwecc_singleplane(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int page, int oob_len)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	uint8_t *oob_buffer = chip->oob_poi;
	uint8_t *data_buf = buf ;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct mtd_ecc_stats stats;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;
	int error = 0, i = 0, j, k, valid_chip_num = 0, page_addr, internal_chipnr = 1, retry_level_support = 0, retry_level = 0, retry_count = 0;
	int pages_per_blk_shift, plane_page_addr = -1, plane_blk_addr = 0, plane_num, read_plane = 0;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		retry_level_support = infotm_chip->retry_level_support;

	page_addr = infotm_chip->page_addr;
	pages_per_blk_shift = (chip->phys_erase_shift - chip->page_shift);
	plane_num = infotm_chip->plane_num;
	if (infotm_chip->plane_num == 2) {
		page_addr /= infotm_chip->plane_num;
		plane_page_addr = (page_addr & ((1 << pages_per_blk_shift) - 1));
		plane_blk_addr = (page_addr >> pages_per_blk_shift);
		plane_blk_addr = (plane_blk_addr << 1);
		infotm_chip->plane_num = 1;
	} else {
		plane_page_addr = (page_addr & ((1 << pages_per_blk_shift) - 1));
		plane_blk_addr = (page_addr >> pages_per_blk_shift);
	}

	stats = mtd->ecc_stats;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
			for (read_plane=0; read_plane<plane_num; read_plane++) {
				retry_count = 0;
				do {
					mtd->ecc_stats.failed = stats.failed;
					page_addr = (plane_page_addr | ((plane_blk_addr + read_plane) << pages_per_blk_shift));
					error = infotm_nand_read_page_hwecc_onechip(mtd, chip, data_buf, ((oob_len > 0) ? oob_buffer : NULL), page_addr, i);
					if (error) {
						NAND_DEBUG(KERN_WARNING,"infotm nand read page error %d\n", error);
						goto exit;
					}
					if ((mtd->ecc_stats.failed - stats.failed) && (retry_count == 0)) {
						memset(oob_buffer, 0x00, nand_oobavail_size);
						infotm_nand_read_oob_raw_onechip(mtd, chip, oob_buffer, page_addr, nand_oobavail_size, i);
						for (j=0; j<nand_oobavail_size; j++) {
							//NAND_DEBUG("%02hhx ", oob_buffer[j]);
							if (oob_buffer[j] != 0xff)
								break;
						}
						//NAND_DEBUG("\n\n ");
						if (j >= nand_oobavail_size) {
							mtd->ecc_stats.failed = stats.failed;
							memset(data_buf, 0xff, nand_page_size * infotm_chip->plane_num * internal_chipnr);
							memset(oob_buffer, 0xff, nand_oobavail_size * infotm_chip->plane_num * internal_chipnr);
						} else if (retry_level_support == 0) {
							NAND_DEBUG(KERN_WARNING,"read page ecc error %x %d \n ", page, i);
							for (k=0; k<nand_oobavail_size; k++) {
								NAND_DEBUG(KERN_WARNING,"%02hhx ", oob_buffer[k]);
							}
							NAND_DEBUG(KERN_WARNING,"\n ");
						}
					}
					if ((mtd->ecc_stats.failed - stats.failed) && retry_level_support) {
						if (retry_count > retry_level_support)
							break;
						retry_level = infotm_chip->retry_level[i] + 1;
						if (retry_level > retry_level_support)
							retry_level = 0;
						infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
						list_for_each_safe(l, n, &imapx800_chip->chip_list) {
							infotm_chip_temp = chip_list_to_imapx800(l);
							infotm_chip_temp->retry_level[i] = retry_level;
						}
						if (retry_count == retry_level_support)
							break;
						retry_count++;
					}

				} while (retry_level_support && (mtd->ecc_stats.failed - stats.failed));
				if (mtd->ecc_stats.failed - stats.failed) {
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, i);
					infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
					infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
					NAND_DEBUG(KERN_WARNING,"read page singleplane retry failed 0x%x %d %d %d 0x%x \n ", page_addr, i, read_plane, infotm_chip->ecc_status, infotm_chip->secc_status);
					goto exit;
				} else if (retry_count > 0) {
					mtd->ecc_stats.corrected++;
					NAND_DEBUG(KERN_WARNING,"read page singleplane retry success %x %d %d \n ", page_addr, i, read_plane);
				}
				if ((retry_count > 0) && ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) || (infotm_chip->mfr_type == NAND_MFR_SANDISK) || (infotm_chip->mfr_type == NAND_MFR_TOSHIBA))) {
					retry_level = 0;
					infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
					list_for_each_safe(l, n, &imapx800_chip->chip_list) {
						infotm_chip_temp = chip_list_to_imapx800(l);
						infotm_chip_temp->retry_level[i] = retry_level;
					}
				}
				data_buf += nand_page_size * infotm_chip->plane_num * internal_chipnr;
				if (oob_len > 0)
					oob_buffer += nand_oobavail_size * infotm_chip->plane_num * internal_chipnr;
			}
		}
	}
	if ((oob_len > 0) && infotm_nand_oob_check(mtd, chip, chip->oob_poi, plane_num * internal_chipnr * valid_chip_num)) {
		for (j=0; j<nand_oobavail_size; j++)
			NAND_DEBUG(KERN_WARNING,"%02hhx ", data_buf[j]);
		NAND_DEBUG(KERN_WARNING,"\n");
	}

exit:
	infotm_chip->plane_num = plane_num;
	return error;
}

static int infotm_nand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int page, int oob_len)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	int error = 0;

	if (infotm_chip->plane_num == 1)
		error = infotm_nand_read_page_hwecc_singleplane(mtd, chip, buf, page, oob_len);
	else
		error = infotm_nand_read_page_hwecc_multiplane(mtd, chip, buf, page, oob_len);

	return error;
}

/* 
 *(1)Author:summer
 *(2)Date:2014-8-4
 *(3)For solving the bug:INFOTM_DMA_BUSY_TIMEOUT
 *(3.1)Modify the return type void to int
 *(3.2)Modify return to "return error"
 * */
static int infotm_nand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	uint8_t *oob_buf = chip->oob_poi;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;
	int error = 0, i = 0, j = 0, page_addr, internal_chipnr = 1;
	//struct timespec ts_write_start, ts_write_end;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	for (i=0; i<infotm_chip->chip_num; i++) {

		//ktime_get_ts(&ts_write_start);
		if (infotm_chip->valid_chip[i]) {

			if (i == 0)
				infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
			else
				infotm_chip->infotm_nand_select_chip(infotm_chip, i);

			page_addr = infotm_chip->page_addr;
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_SEQIN, 0, page_addr, i);

			for (j=0; j<internal_chipnr; j++) {

				if (j > 0) {
					page_addr = infotm_chip->page_addr;
					page_addr |= (1 << infotm_chip->internal_chip_shift) * j;
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_SEQIN, 0, page_addr, i);
				}

				if (infotm_chip->plane_num == 2) {

					//oob_buf[chip->badblockpos+1] = 0x0;
					//oob_buf[0] = 0xff;
					*(unsigned *)oob_buf = (crc32((0 ^ 0xffffffffL), oob_buf+PAGE_MANAGE_SIZE, mtd->oobavail) ^ 0xffffffffL);
					error = infotm_chip->infotm_nand_dma_write(infotm_chip, (unsigned char *)buf, nand_page_size, infotm_chip->ecc_mode, oob_buf, nand_oobavail_size, infotm_chip->secc_mode, NAND_CMD_DUMMY_PROGRAM);
					if (error) {
						NAND_DEBUG (KERN_WARNING,"nand_dma_write fail %d: plane0 %x \n", i, page_addr);
						goto exit;
					}
					udelay(10);

					//oob_buf += nand_oobavail_size;
					buf += nand_page_size;

					//oob_buf[chip->badblockpos+1] = 0x0;
					//oob_buf[0] = 0xff;
					infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_WRITE2, 0x00, page_addr, i);
					error = infotm_chip->infotm_nand_dma_write(infotm_chip, (unsigned char *)buf, nand_page_size, infotm_chip->ecc_mode, oob_buf, nand_oobavail_size, infotm_chip->secc_mode, NAND_CMD_PAGEPROG);
					if (error) {
						NAND_DEBUG (KERN_WARNING,"nand_dma_write fail %d: plane1 %x \n", i, page_addr);
						goto exit;
					}
					buf += nand_page_size;
				} else if (infotm_chip->plane_num == 1) {

					//oob_buf[chip->badblockpos+1] = 0x0;
					//oob_buf[0] = 0xff;
					*(unsigned *)oob_buf = (crc32((0 ^ 0xffffffffL), oob_buf+PAGE_MANAGE_SIZE, mtd->oobavail) ^ 0xffffffffL);
					error = infotm_chip->infotm_nand_dma_write(infotm_chip, (unsigned char *)buf, nand_page_size, infotm_chip->ecc_mode, oob_buf, nand_oobavail_size, infotm_chip->secc_mode, NAND_CMD_PAGEPROG);
					if (error)
						goto exit;

					buf += nand_page_size;
				} else {
					error = -ENODEV;
					goto exit;
				}
			}
		}
	}

exit:
	return error;
}

/* 
 *(1)Author:summer
 *(2)Date:2014-8-4
 *(3)Reason:
 *(3.1) For solving the bug:INFOTM_DMA_BUSY_TIMEOUT
 *(3.2) We must get the return value from chip->write_page
 * */
static int infotm_nand_write_page(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int page, int cached, int raw)
{
	int status,tmp;
	int dma_write_status = 0;//summer
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);

	if (infotm_chip->subpage_cache_page == page / (mtd->writesize >> chip->page_shift)) {
		memcpy(infotm_chip->subpage_cache_buf, buf, mtd->writesize);
		memset(infotm_chip->subpage_cache_status, 0x1, MAX_CHIP_NUM*MAX_PLANE_NUM);
	}

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page);

	/* 
	 *(1)For the function: chip->ecc.write_page(mtd, chip, buf);
	 *(2)If no error,return 0;Else return !0
	 * */
	if (unlikely(raw))
		chip->ecc.write_page_raw(mtd, chip, buf);
	else
		dma_write_status = chip->ecc.write_page(mtd, chip, buf);

	if( dma_write_status )
		return dma_write_status;

	if (!cached || !(chip->options & NAND_CACHEPRG)) {

		status = chip->waitfunc(mtd, chip);
		/*
		 * See if operation failed and additional status checks are
		 * available
		 */
		if ((status & NAND_STATUS_FAIL) && (chip->errstat))
			status = chip->errstat(mtd, chip, FL_WRITING, status, page);

		if (status & NAND_STATUS_FAIL) {
			NAND_DEBUG(KERN_WARNING,"infotm nand write failed at %d \n", page);
#if NAND_TEST
			tmp = imapx_pad_get_mode(PADSRANGE(0, 15));
			NAND_DEBUG(KERN_DEBUG,"[nand debug %d] pad mode 0x%x\n",__LINE__, tmp);
			NAND_DEBUG(KERN_DEBUG,"infotm_chip->clk = %ld, bus clk %ld\n", clk_get_rate(imapx800_chip->ecc_clk),  clk_get_rate(imapx800_chip->sys_clk));
#endif 
			return -EIO;
		}
	} else {
		status = chip->waitfunc(mtd, chip);
	}

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	chip->cmdfunc(mtd, NAND_CMD_READ0, 0x00, page);
	status = chip->ecc.read_page(mtd, chip, chip->buffers->databuf, page);
	if (status == -EUCLEAN)
		status = 0;

	if (memcmp(buf, chip->buffers->databuf, mtd->writesize)) {
		NAND_DEBUG(KERN_WARNING,"nand verify failed at %d \n", page);
		memcpy(chip->buffers->databuf, buf, mtd->writesize);
		return -EFAULT;
	}
#endif

	return 0;
}

static int infotm_nand_read_oob_onechip(struct mtd_info *mtd, struct nand_chip *chip, unsigned char *oob_buffer, int page, int chipnr)
{
	int32_t error = 0, i = chipnr, j = 0, stat = 0, page_addr, internal_chipnr = 1, column; 
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = chip->ecc.layout->oobavail + PAGE_MANAGE_SIZE;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	if (infotm_chip->valid_chip[i]) {

		column = nand_page_size + nand_oobavail_size + (infotm_chip->page_size/chip->ecc.size) * chip->ecc.bytes;
		infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
		if (chip->cmdfunc == infotm_nand_command) {
			page_addr = page;
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, column, page_addr, i);
		} else {
			if (i == 0)
				page_addr = page;
			else
				page_addr = infotm_chip->page_addr;
			chip->cmdfunc(mtd, NAND_CMD_READ0, column, page_addr);
		}

		for (j=0; j<internal_chipnr; j++) {

			if (j > 0) {
				page_addr = page;
				page_addr |= (1 << infotm_chip->internal_chip_shift) * j;
				infotm_chip->infotm_nand_select_chip(infotm_chip, i);
				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READ0, column, page_addr, i);
			}

			if (!infotm_chip->infotm_nand_wait_devready(infotm_chip, i)) {
				NAND_DEBUG (KERN_WARNING,"read oob couldn`t found selected chip: %d ready\n", i);
				error = -EBUSY;
				goto exit;
			}

			if (infotm_chip->plane_num == 2) {

				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ1, column, page_addr, i);
				error = infotm_chip->infotm_nand_dma_read(infotm_chip, NULL, 0, -1, oob_buffer, nand_oobavail_size, infotm_chip->secc_mode);
				stat = infotm_chip->infotm_nand_hwecc_correct(infotm_chip, 0, nand_oobavail_size);
				if (stat < 0) {
					mtd->ecc_stats.failed++;
					//NAND_DEBUG("infotm nand read oob ecc plane0 failed at page %d chip %d\n", page_addr, i);
				} else {
					mtd->ecc_stats.corrected += stat;
				}
				if (error) {
					//NAND_DEBUG("infotm nand read oob ecc plane0 failed at page %d chip %d\n", page_addr, i);
					goto exit;
				}

				oob_buffer += nand_oobavail_size;

				infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_TWOPLANE_READ2, column, page_addr, i);
				error = infotm_chip->infotm_nand_dma_read(infotm_chip, NULL, 0, -1, oob_buffer, nand_oobavail_size, infotm_chip->secc_mode);
				stat = infotm_chip->infotm_nand_hwecc_correct(infotm_chip, 0, nand_oobavail_size);
				if (stat < 0) {
					mtd->ecc_stats.failed++;
					//NAND_DEBUG("infotm nand read oob ecc plane1 failed at page %d chip %d\n", page_addr, i);
				} else {
					mtd->ecc_stats.corrected += stat;
				}
				if (error) {
					//NAND_DEBUG("infotm nand read oob dma plane1 failed at page %d chip %d\n", page_addr, i);
					goto exit;
				}

			} else {
				error = infotm_chip->infotm_nand_dma_read(infotm_chip, NULL, 0, -1, oob_buffer, nand_oobavail_size, infotm_chip->secc_mode);
				stat = infotm_chip->infotm_nand_hwecc_correct(infotm_chip, 0, nand_oobavail_size);
				if (stat < 0) {
					mtd->ecc_stats.failed++;
					//NAND_DEBUG("infotm nand read oob ecc failed at blk %d chip %d \n", (page_addr >> pages_per_blk_shift), i);
				} else {
					mtd->ecc_stats.corrected += stat;
				}
				if (error) {
					//NAND_DEBUG("infotm nand read oob dma failed at page %d chip %d\n", page_addr, i);
					goto exit;
				}
			}

			oob_buffer += nand_oobavail_size;
		}
	} else {
		error = -ENODEV;
		mdelay(100);
		goto exit;
	}

exit:
	return error;
}

static int infotm_nand_read_oob_singleplane(struct mtd_info *mtd, struct nand_chip *chip, int page, int readlen)
{
	int32_t error = 0, i, j = 0, pages_per_blk_shift, page_addr, column, internal_chipnr = 1, retry_level_support = 0, retry_level = 0, retry_count = 0; 
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	unsigned char *oob_buffer = chip->oob_poi;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct mtd_ecc_stats stats;
	int plane_page_addr = -1, plane_blk_addr = 0, plane_num, read_plane = 0;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		retry_level_support = infotm_chip->retry_level_support;

	pages_per_blk_shift = (chip->phys_erase_shift - chip->page_shift);
	page_addr = page;
	if (chip->cmdfunc == infotm_nand_command) {
		column = nand_page_size + nand_oobavail_size + (infotm_chip->page_size/chip->ecc.size) * chip->ecc.bytes;
		chip->cmdfunc(mtd, NAND_CMD_READOOB, column, page_addr);
		page_addr = infotm_chip->page_addr;
	}

	plane_num = infotm_chip->plane_num;
	if (infotm_chip->plane_num == 2) {
		page_addr /= infotm_chip->plane_num;
		plane_page_addr = (page_addr & ((1 << pages_per_blk_shift) - 1));
		plane_blk_addr = (page_addr >> pages_per_blk_shift);
		plane_blk_addr = (plane_blk_addr << 1);
		infotm_chip->plane_num = 1;
	} else {
		plane_page_addr = (page_addr & ((1 << pages_per_blk_shift) - 1));
		plane_blk_addr = (page_addr >> pages_per_blk_shift);
	}

	stats = mtd->ecc_stats;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			for (read_plane=0; read_plane<plane_num; read_plane++) {
				retry_count = 0;
				do {
					mtd->ecc_stats.failed = stats.failed;
					page_addr = (plane_page_addr | ((plane_blk_addr + read_plane) << pages_per_blk_shift));
					error = infotm_nand_read_oob_onechip(mtd, chip, oob_buffer, page_addr, i);
					if (error) {
						NAND_DEBUG(KERN_WARNING,"infotm nand read oob error %d\n", error);
						mtd->ecc_stats.failed++;
						goto exit;
					}
					if ((mtd->ecc_stats.failed - stats.failed) && (retry_count == 0)) {
						memset(oob_buffer, 0x00, nand_oobavail_size);
						infotm_nand_read_oob_raw_onechip(mtd, chip, oob_buffer, page_addr, nand_oobavail_size, i);
						for (j=0; j<nand_oobavail_size; j++) {
							//NAND_DEBUG("%02hhx ", oob_buffer[j]);
							if (oob_buffer[j] != 0xff)
								break;
						}
						//NAND_DEBUG("\n\n ");
						if (j >= nand_oobavail_size) {
							mtd->ecc_stats.failed = stats.failed;
							memset(oob_buffer, 0xff, nand_oobavail_size * plane_num * internal_chipnr);
							goto exit;
						} else if (retry_level_support) {
							infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, i);
							infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
							infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
							//NAND_DEBUG("infotm nand read oob ecc error %x \n", page_addr);
						}
					}
					if ((mtd->ecc_stats.failed - stats.failed) && retry_level_support) {
						if (retry_count > retry_level_support)
							break;
						retry_level = infotm_chip->retry_level[i] + 1;
						if (retry_level > retry_level_support)
							retry_level = 0;
						infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
						list_for_each_safe(l, n, &imapx800_chip->chip_list) {
							infotm_chip_temp = chip_list_to_imapx800(l);
							infotm_chip_temp->retry_level[i] = retry_level;
						}
						if (retry_count == retry_level_support)
							break;
						retry_count++;
					}

				} while (retry_level_support && (mtd->ecc_stats.failed - stats.failed));
				if (mtd->ecc_stats.failed - stats.failed) {
					NAND_DEBUG(KERN_WARNING,"read oob ecc retry failed %x %d %d %d \n ", page_addr, i, read_plane, retry_level);
					goto exit;
				}
				if ((retry_count > 0) && ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) || (infotm_chip->mfr_type == NAND_MFR_SANDISK) || (infotm_chip->mfr_type == NAND_MFR_TOSHIBA))) {
					retry_level = 0;
					infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
					list_for_each_safe(l, n, &imapx800_chip->chip_list) {
						infotm_chip_temp = chip_list_to_imapx800(l);
						infotm_chip_temp->retry_level[i] = retry_level;
					}
				}
				error = infotm_nand_oob_check(mtd, chip, oob_buffer, 1);
				if (error == 0) {
					if (oob_buffer != chip->oob_poi)
						memcpy(chip->oob_poi, oob_buffer, nand_oobavail_size);
					break;
				}
				oob_buffer += nand_oobavail_size * infotm_chip->plane_num * internal_chipnr;
			}
			if (error == 0)
				break;
		}
	}

exit:
	infotm_chip->plane_num = plane_num;
	return readlen;
}

static int infotm_nand_read_oob_multiplane(struct mtd_info *mtd, struct nand_chip *chip, int page, int readlen)
{
	int32_t error = 0, i, j = 0, page_addr, column, internal_chipnr = 1, retry_level_support = 0, retry_level = 0, retry_count = 0; 
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	unsigned char *oob_buffer = chip->oob_poi;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct mtd_ecc_stats stats;
	unsigned nand_page_size = (1 << chip->page_shift);
	unsigned nand_oobavail_size = mtd->oobavail + PAGE_MANAGE_SIZE;

	if (infotm_chip->ops_mode & INFOTM_INTERLEAVING_MODE)
		internal_chipnr = infotm_chip->internal_chipnr;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		retry_level_support = infotm_chip->retry_level_support;

	page_addr = page;
	if (chip->cmdfunc == infotm_nand_command) {
		column = nand_page_size + nand_oobavail_size + (infotm_chip->page_size/chip->ecc.size) * chip->ecc.bytes;
		chip->cmdfunc(mtd, NAND_CMD_READOOB, column, page_addr);
		page_addr = infotm_chip->page_addr;
	}

	stats = mtd->ecc_stats;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {

			retry_count = 0;
			do {
				mtd->ecc_stats.failed = stats.failed;
				error = infotm_nand_read_oob_onechip(mtd, chip, oob_buffer, page_addr, i);
				if (error) {
					NAND_DEBUG(KERN_WARNING,"infotm nand read oob error %d\n", error);
					mtd->ecc_stats.failed++;
					goto exit;
				}
				if ((mtd->ecc_stats.failed - stats.failed) && (retry_count == 0)) {
					memset(oob_buffer, 0x00, nand_oobavail_size);
					infotm_nand_read_oob_raw_onechip(mtd, chip, oob_buffer, page_addr, nand_oobavail_size, i);
					for (j=0; j<nand_oobavail_size; j++) {
						//NAND_DEBUG("%02hhx ", oob_buffer[j]);
						if (oob_buffer[j] != 0xff)
							break;
					}
					//NAND_DEBUG("\n\n ");
					if (j >= nand_oobavail_size) {
						mtd->ecc_stats.failed = stats.failed;
						memset(oob_buffer, 0xff, nand_oobavail_size * infotm_chip->plane_num * internal_chipnr);
						goto exit;
					} else if (retry_level_support) {
						infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, i);
						infotm_chip->infotm_nand_wait_devready(infotm_chip, i);
						infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);
						//NAND_DEBUG("infotm nand read oob ecc error %x \n", page_addr);
					}
				}
				if ((mtd->ecc_stats.failed - stats.failed) && retry_level_support) {
					if (retry_count > retry_level_support)
						break;
					retry_level = infotm_chip->retry_level[i] + 1;
					if (retry_level > retry_level_support)
						retry_level = 0;
					infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
					list_for_each_safe(l, n, &imapx800_chip->chip_list) {
						infotm_chip_temp = chip_list_to_imapx800(l);
						infotm_chip_temp->retry_level[i] = retry_level;
					}
					if (retry_count == retry_level_support)
						break;
					retry_count++;
				}

			} while (retry_level_support && (mtd->ecc_stats.failed - stats.failed));
			if (mtd->ecc_stats.failed - stats.failed) {
				NAND_DEBUG(KERN_WARNING,"read oob ecc retry failed %x %d %d \n ", page_addr, i, retry_level);
				goto exit;
			}
			if ((retry_count > 0) && ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) || (infotm_chip->mfr_type == NAND_MFR_SANDISK) || (infotm_chip->mfr_type == NAND_MFR_TOSHIBA))) {
				retry_level = 0;
				infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
				list_for_each_safe(l, n, &imapx800_chip->chip_list) {
					infotm_chip_temp = chip_list_to_imapx800(l);
					infotm_chip_temp->retry_level[i] = retry_level;
				}
			}
			error = infotm_nand_oob_check(mtd, chip, oob_buffer, infotm_chip->plane_num);
			if (error == 0) {
				if (oob_buffer != chip->oob_poi)
					memcpy(chip->oob_poi, oob_buffer, nand_oobavail_size);
				break;
			}
			oob_buffer += nand_oobavail_size * infotm_chip->plane_num * internal_chipnr;
		}
	}

exit:
	return readlen;
}

static int infotm_nand_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page, int readlen)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		infotm_nand_read_oob_singleplane(mtd, chip, page, readlen);
	else
		infotm_nand_read_oob_multiplane(mtd, chip, page, readlen);

	return readlen;
}

static int infotm_nand_write_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	NAND_DEBUG(KERN_WARNING,"our host controller`s structure couldn`t support oob write\n");
	BUG();
	return 0;
}

static int infotm_nand_phy_block_bad(struct infotm_nand_chip *infotm_chip, int blk_addr, int chipnr)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	unsigned char *oob_buffer = chip->oob_poi;
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct mtd_ecc_stats stats;
	int error = 0, page, pages_per_blk, i = chipnr, column;
	int retry_level_support = 0, retry_level = 0, retry_count = 0;
	unsigned nand_oobavail_size = chip->ecc.layout->oobavail + PAGE_MANAGE_SIZE;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE)
		retry_level_support = infotm_chip->retry_level_support;

	stats = mtd->ecc_stats;
	pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));
	column = (1 << chip->page_shift) + nand_oobavail_size + (infotm_chip->page_size/chip->ecc.size) * chip->ecc.bytes;
	if (infotm_chip->valid_chip[i]) {

		retry_count = 0;
		do {
			mtd->ecc_stats.failed = stats.failed;
			page = blk_addr*pages_per_blk + infotm_chip->infotm_nand_dev->bad0;
			if (chip->cmdfunc == infotm_nand_command) {
				chip->cmdfunc(mtd, NAND_CMD_READOOB, column, page);
				page = infotm_chip->page_addr;
			}
			error = infotm_nand_read_oob_onechip(mtd, chip, oob_buffer, page, i);
			if (error) {
				NAND_DEBUG(KERN_WARNING,"infotm nand bbt creat read error %d %d %d\n", error, page, i);
				break;
			}

			if ((mtd->ecc_stats.failed - stats.failed) && (retry_count == 0)) {
				memset(oob_buffer, 0x00, nand_oobavail_size);
				infotm_nand_read_oob_raw_onechip(mtd, chip, oob_buffer, page, nand_oobavail_size, i);
				if (oob_buffer[0] == 0xff) {
					if (infotm_chip->infotm_nand_dev->bad1 != infotm_chip->infotm_nand_dev->bad0) {
						page += (infotm_chip->infotm_nand_dev->bad1 - infotm_chip->infotm_nand_dev->bad0);
						memset(oob_buffer, 0x00, nand_oobavail_size);
						infotm_nand_read_oob_raw_onechip(mtd, chip, (unsigned char *)oob_buffer, page, nand_oobavail_size, i);
						infotm_nand_read_oob_raw_onechip(mtd, chip, (unsigned char *)oob_buffer, page, nand_oobavail_size, i);
					}
					if (oob_buffer[0] == 0xff) {
						mtd->ecc_stats.failed = stats.failed;
						break;
					}
				}
			}
			if ((mtd->ecc_stats.failed - stats.failed) && retry_level_support) {
				if (retry_count > retry_level_support)
					break;
				retry_level = infotm_chip->retry_level[i] + 1;
				if (retry_level > retry_level_support)
					retry_level = 0;
				infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
				list_for_each_safe(l, n, &imapx800_chip->chip_list) {
					infotm_chip_temp = chip_list_to_imapx800(l);
					infotm_chip_temp->retry_level[i] = retry_level;
				}
				if (retry_count == retry_level_support)
					break;
				retry_count++;
			}

		} while (retry_level_support && (mtd->ecc_stats.failed - stats.failed));
		if (mtd->ecc_stats.failed - stats.failed) {
			error = EFAULT;
			//NAND_DEBUG("infotm nand bbt creat badblk %d %d %x %x \n ", page, retry_level, oob_buffer[0], oob_buffer[1]);
			//goto exit;
		} /*else if ((!raw_flag) && (oob_buffer[1] != 0)) {
		    NAND_DEBUG("infotm nand bbt creat test badblk %d %d %x \n ", page, i, oob_buffer[1]);
		    }*/
		if ((retry_count > 0) && ((infotm_chip->mfr_type == NAND_MFR_MICRON) || (infotm_chip->mfr_type == NAND_MFR_SAMSUNG) || (infotm_chip->mfr_type == NAND_MFR_SANDISK) || (infotm_chip->mfr_type == NAND_MFR_TOSHIBA))) {
			retry_level = 0;
			infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, retry_level);
			list_for_each_safe(l, n, &imapx800_chip->chip_list) {
				infotm_chip_temp = chip_list_to_imapx800(l);
				infotm_chip_temp->retry_level[i] = retry_level;
			}
		}
	}

	return error;
}

static int infotm_nand_logic_block_bad(struct infotm_nand_chip *infotm_chip, int blk_addr)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	int error = 0, blk_num, i;
	unsigned plane_num, valid_chip_num = 0;

	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
		}
	}

	plane_num = infotm_chip->plane_num;
	mtd->erasesize /= plane_num*valid_chip_num;
	mtd->writesize /= plane_num*valid_chip_num;
	mtd->writebufsize /= plane_num*valid_chip_num;
	mtd->oobsize /= plane_num*valid_chip_num;
	infotm_chip->plane_num = 1;

	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			for (blk_num=blk_addr * plane_num; blk_num<(blk_addr * plane_num+plane_num); blk_num++) {
				error = infotm_nand_phy_block_bad(infotm_chip, blk_num, i);
				if (error) {
					NAND_DEBUG(KERN_WARNING,"infotm nand found logic bad blk %d %d chip %d \n", blk_addr, blk_num, i);
					break;
				}
			}
		}
	}

	valid_chip_num = 0;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
		}
	}
	mtd->erasesize *= plane_num*valid_chip_num;
	mtd->writesize *= plane_num*valid_chip_num;
	mtd->writebufsize *= plane_num*valid_chip_num;
	mtd->oobsize *= plane_num*valid_chip_num;
	infotm_chip->plane_num = plane_num;
	return error;
}

static int infotm_nand_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_chip *infotm_chip_bbt = infotm_chip;
	struct infotm_nand_bbt_info *nand_bbt_info = NULL;
	struct list_head *l, *n;
	struct nand_bbt_t *nand_bbt;
	int32_t ret = 0, mtd_erase_shift, blk_addr;
	int j;

	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip_bbt = chip_list_to_imapx800(l);
		if (infotm_chip_bbt->nand_bbt_info != NULL) {
			nand_bbt_info = infotm_chip_bbt->nand_bbt_info;
			if (nand_bbt_info != NULL)
				break;
		}
	}

	mtd_erase_shift = fls(mtd->erasesize) - 1;
	blk_addr = (int)(ofs >> mtd_erase_shift);

	if (nand_bbt_info != NULL) {
		if (infotm_chip->plane_num == 1)
			blk_addr /= infotm_chip_bbt->plane_num;
		for (j=0; j<nand_bbt_info->total_bad_num; j++) {
			nand_bbt = &nand_bbt_info->nand_bbt[j];
			if (blk_addr == nand_bbt->blk_addr) {
				NAND_DEBUG(KERN_WARNING,"infotm nand bbt detect bad blk %llx %d %d %d \n", ofs, nand_bbt->blk_addr, nand_bbt->chip_num, nand_bbt->bad_type);
				return EFAULT;
			}
		}
	} else {
		ret = infotm_nand_logic_block_bad(infotm_chip, blk_addr);
		if (ret)
			return EFAULT;
	}

	return 0;
}

static int infotm_nand_suspend(struct mtd_info *mtd)
{
	return 0;
}

static void infotm_nand_resume(struct mtd_info *mtd)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct infotm_nand_platform *plat = infotm_chip->platform;
	int i;

	if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
		if (infotm_chip->onfi_mode) {
			for (i=0; i<infotm_chip->chip_num; i++) {
				if (infotm_chip->valid_chip[i])
					infotm_nand_set_onfi_features(infotm_chip, (uint8_t *)(&infotm_chip->onfi_mode), ONFI_TIMING_ADDR, i);

			}
		}
		if (mtd->_resume_last)
			mtd->_resume_last(mtd);
	}

	return;
}

static struct cdbg_cfg_nand *infotm_nand_get_flash_type(struct mtd_info *mtd,
		struct nand_chip *chip,
		int busw, int *maf_id)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct cdbg_cfg_nand *type = NULL;
	int maf_idx, i;
	u8 dev_id[MAX_ID_LEN * 2];
	//int tmp_id, tmp_manf;

	/* Send the command for reading device ID */
	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

	/* Read manufacturer and device IDs */
	infotm_chip->infotm_nand_read_byte(infotm_chip, dev_id, MAX_ID_LEN, 0);
	*maf_id = dev_id[0];
	NAND_DEBUG(KERN_INFO, " NAND device id: %x %x %x %x %x %x %x %x \n", dev_id[0], dev_id[1], dev_id[2], dev_id[3], dev_id[4], dev_id[5], dev_id[6], dev_id[7]);

	for (i = 0; infotm_nand_flash_ids[i].magic == CDBG_MAGIC_NAND; i++) {
		if(!memcmp(infotm_nand_flash_ids[i].id, dev_id, strlen((const char*)infotm_nand_flash_ids[i].id))){
			type = &infotm_nand_flash_ids[i];
			break;
		}
	}

	if (!type) {
		if (plat->infotm_nand_dev) {
			if(!strncmp((char*)plat->infotm_nand_dev->id, (char*)dev_id, strlen((const char*)plat->infotm_nand_dev->id))){
				type = plat->infotm_nand_dev;
			}
		}

		if (!type)
			return ERR_PTR(-ENODEV);
	}

	if (!mtd->name)
		mtd->name = (const char *)type->name;

	/* Newer devices have all the information in additional id bytes */
	if (!type->pagesize) {
		int extid;
		/* The 3rd id byte holds MLC / multichip data */
		chip->cellinfo = chip->read_byte(mtd);
		/* The 4th id byte is the important one */
		extid = chip->read_byte(mtd);
		/* Calc pagesize */
		mtd->writesize = 1024 << (extid & 0x3);
		extid >>= 2;
		/* Calc oobsize */
		mtd->oobsize = (8 << (extid & 0x01)) * (mtd->writesize >> 9);
		extid >>= 2;
		/* Calc blocksize. Blocksize is multiples of 64KiB */
		mtd->erasesize = (64 * 1024) << (extid & 0x03);
		extid >>= 2;
		/* Get buswidth information */
		busw = (extid & 0x01) ? NAND_BUSWIDTH_16 : 0;

	} else {
		/*
		 * Old devices have chip data hardcoded in the device id table
		 */
		mtd->oobsize = type->pagesize - (1 << (fls(type->pagesize) - 1));
		mtd->writesize = type->pagesize - mtd->oobsize;
		mtd->erasesize = mtd->writesize*type->blockpages;
		chip->chipsize = mtd->erasesize;
		chip->chipsize *= type->blockcount;
		busw = CDBG_NF_BUSW(type->parameter);
		infotm_chip->block_size = mtd->erasesize;
	}

	/* Try to identify manufacturer */
	for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++) {
		if (nand_manuf_ids[maf_idx].id == *maf_id)
			break;
	}

	/*
	 * Check, if buswidth is correct. Hardware drivers should set
	 * chip correct !
	 */
	if (busw != (chip->options & NAND_BUSWIDTH_16)) {
		//NAND_DEBUG(KERN_INFO "NAND device: Manufacturer ID:" " 0x%02x, Chip ID: 0x%02x (%s %s)\n", *maf_id, dev_id[0], nand_manuf_ids[maf_idx].name, mtd->name);
		NAND_DEBUG(KERN_WARNING, "NAND bus width %d instead %d bit\n", (chip->options & NAND_BUSWIDTH_16) ? 16 : 8, busw ? 16 : 8);
		//return ERR_PTR(-EINVAL);
		chip->options |= NAND_BUSWIDTH_16;
	}

	infotm_chip->infotm_nand_dev = type;
	/* Calculate the address shift from the page size */
	chip->page_shift = ffs(mtd->writesize) - 1;
	/* Convert chipsize to number of pages per chip -1. */
	chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;

	chip->bbt_erase_shift = chip->phys_erase_shift = ffs(mtd->erasesize) - 1;
	chip->chip_shift = ffs(chip->chipsize) - 1;

	/* Set the bad block position */
	chip->badblockpos = INFOTM_BADBLK_POS;

	/* Get chip options, preserve non chip based options */
	//chip->options &= ~NAND_CHIPOPTIONS_MSK;
	//chip->options |= type->options & NAND_CHIPOPTIONS_MSK;

	/*
	 * Set chip as a default. Board drivers can override it, if necessary
	 */
	chip->options |= NAND_NO_AUTOINCR;

	/* Check if chip is a not a samsung device. Do not clear the
	 * options for chips which are not having an extended id.
	 */
	//if (*maf_id != NAND_MFR_SAMSUNG && !type->pagesize)
	//chip->options &= ~NAND_SAMSUNG_LP_OPTIONS;

	NAND_DEBUG(KERN_INFO, " NAND device: Manufacturer ID:"
			" 0x%02x, Chip ID: 0x%02x (%s %s)\n", *maf_id, dev_id[0],
			nand_manuf_ids[maf_idx].name, type->name);

	return type;
}

static int infotm_nand_scan_ident(struct mtd_info *mtd, int maxchips)
{
	int i, busw, nand_maf_id, valid_chip_num = 1;
	struct nand_chip *chip = mtd->priv;
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	struct infotm_nand_platform *plat = infotm_chip->platform;
	//struct infotm_nand_chip *infotm_chip_temp;
	struct cdbg_cfg_nand *infotm_type = NULL;
	u8 dev_id[MAX_ID_LEN * 2], onfi_features[4];
	unsigned temp_chip_shift;
	//struct list_head *l, *n;

	/* Get buswidth to select the correct functions */
	busw = chip->options & NAND_BUSWIDTH_16;

	if (plat->infotm_nand_dev) {
		infotm_type = plat->infotm_nand_dev;
	} /*else {
		list_for_each_safe(l, n, &imapx800_chip->chip_list) {
			infotm_chip_temp = chip_list_to_imapx800(l);
			plat = infotm_chip_temp->platform;
			if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
				infotm_type = infotm_chip_temp->infotm_nand_dev;
				break;
			}
		}
	}*/
	if (infotm_type == NULL) {
		/* Select the device */
		chip->select_chip(mtd, 0);

		//reset chip for some nand need reset after power up
		chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
		infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);

		/* Read the flash type */
		infotm_type = infotm_nand_get_flash_type(mtd, chip, busw, &nand_maf_id);

		if (IS_ERR(infotm_type)) {
			NAND_DEBUG(KERN_WARNING, "No NAND device found!!!\n");
			chip->select_chip(mtd, -1);
			return PTR_ERR(infotm_type);
		}

		memset(dev_id, 0, MAX_ID_LEN);
		chip->cmdfunc(mtd, NAND_CMD_READID, 0x20, -1);
		infotm_chip->infotm_nand_read_byte(infotm_chip, dev_id, MAX_ID_LEN, 0);
		if(!memcmp((char*)dev_id, "ONFI", 4)) {
			infotm_chip->onfi_mode = (unsigned)CDBG_NF_ONFIMODE(infotm_type->parameter);
		}
		infotm_chip->mfr_type = infotm_type->id[0];

		/* Check for a chip array */
		for (i = 1; i < maxchips; i++) {
			infotm_chip->infotm_nand_select_chip(infotm_chip, i);
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, i);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, i);

			/* Send the command for reading device ID */
			infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_READID, 0x00, -1, i);
			/* Read manufacturer and device IDs */

			infotm_chip->infotm_nand_read_byte(infotm_chip, dev_id, MAX_ID_LEN, 0);
			//if (nand_maf_id != chip->read_byte(mtd) || infotm_type->id[1] != chip->read_byte(mtd))
			if (nand_maf_id != dev_id[0] || infotm_type->id[1] != dev_id[1])
				infotm_chip->valid_chip[i] = 0;
			else
				valid_chip_num ++;
		}
	} else {
		nand_maf_id = infotm_type->id[0];
		mtd->oobsize = infotm_type->pagesize - (1 << (fls(infotm_type->pagesize) - 1));
		mtd->writesize = infotm_type->pagesize - mtd->oobsize;
		mtd->erasesize = mtd->writesize * infotm_type->blockpages;
		chip->chipsize = mtd->erasesize;
		chip->chipsize *= infotm_type->blockcount;
		busw = CDBG_NF_BUSW(infotm_type->parameter);
		if (busw)
			chip->options |= NAND_BUSWIDTH_16;
		infotm_chip->block_size = mtd->erasesize;
		if (!mtd->name)
			mtd->name = (const char *)infotm_type->name;
		infotm_chip->infotm_nand_dev = infotm_type;
		chip->page_shift = ffs(mtd->writesize) - 1;
		chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;
		chip->bbt_erase_shift = chip->phys_erase_shift = ffs(mtd->erasesize) - 1;
		chip->chip_shift = ffs(chip->chipsize) - 1;
		chip->badblockpos = INFOTM_BADBLK_POS;
		chip->options |= NAND_NO_AUTOINCR;
		infotm_chip->mfr_type = infotm_type->id[0];
		infotm_chip->onfi_mode = (unsigned)CDBG_NF_ONFIMODE(infotm_type->parameter);
		if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME)))
			valid_chip_num = infotm_nand_save->valid_chip_num;
		else
			valid_chip_num = 1;
		for (i = valid_chip_num; i < maxchips; i++)
			infotm_chip->valid_chip[i] = 0;
	}
	if (valid_chip_num > 1)
		NAND_DEBUG(KERN_ALERT," %d NAND chips detected\n", valid_chip_num);
	if (infotm_chip->onfi_mode) {
		for (i=0; i<infotm_chip->chip_num; i++) {
			if (infotm_chip->valid_chip[i]) {
				infotm_nand_set_onfi_features(infotm_chip, (uint8_t *)(&infotm_chip->onfi_mode), ONFI_TIMING_ADDR, i);
				infotm_nand_get_onfi_features(infotm_chip, onfi_features, ONFI_TIMING_ADDR, i);
				if (onfi_features[0] != infotm_chip->onfi_mode) {
					NAND_DEBUG(KERN_WARNING," onfi timing mode set failed: %x\n", onfi_features[0]);		
				}
			}
		}
	}

	/* Store the number of chips and calc total size for mtd */
	chip->numchips = 1;
	if ((chip->chipsize >> 32) & 0xffffffff)
		chip->chip_shift = fls((unsigned)(chip->chipsize >> 32)) * valid_chip_num + 32 - 1;
	else 
		chip->chip_shift = fls((unsigned)chip->chipsize) * valid_chip_num - 1;

	chip->pagemask = ((chip->chipsize * valid_chip_num) >> chip->page_shift) - 1;
	chip->options &= ~NAND_CACHEPRG;
	infotm_chip->internal_chipnr = CDBG_NF_INTERNR(infotm_type->parameter);
	infotm_chip->internal_page_nums = (chip->chipsize >> chip->page_shift);
	infotm_chip->internal_page_nums /= infotm_chip->internal_chipnr;
	infotm_chip->internal_chip_shift = fls((unsigned)infotm_chip->internal_page_nums) - 1;
	temp_chip_shift = ffs((unsigned)infotm_chip->internal_page_nums) - 1;
	if (infotm_chip->internal_chip_shift != temp_chip_shift) {
		infotm_chip->internal_chip_shift += 1;
		chip->chip_shift += 1;
		chip->pagemask = ((1 << (chip->chip_shift + 1)) >> chip->page_shift) - 1;
	}

	infotm_chip->options = infotm_type->resv0;
	infotm_chip->page_size = mtd->writesize;
	infotm_chip->block_size = infotm_chip->page_size*infotm_type->blockpages;
	infotm_chip->oob_size = mtd->oobsize;
	mtd->erasesize = valid_chip_num * infotm_chip->block_size;
	mtd->writesize = valid_chip_num * infotm_chip->page_size;
	mtd->oobsize = valid_chip_num * infotm_chip->oob_size;
	mtd->size = valid_chip_num * chip->chipsize;

	chip->cmdfunc = infotm_nand_command;
	chip->waitfunc = infotm_nand_wait;
	if (!chip->erase_cmd)
		chip->erase_cmd = infotm_nand_erase_cmd;
	if (!chip->write_page)
		chip->write_page = infotm_nand_write_page;

	return 0;
}

int infotm_nand_scan(struct mtd_info *mtd, int maxchips)
{
	int ret = 0;;

	ret = infotm_nand_scan_ident(mtd, maxchips);
	if (!ret)
		ret = nand_scan_tail(mtd);
	return ret;
}

static int infotm_platform_options_confirm(struct infotm_nand_chip *infotm_chip)
{
	return 0;
}

static uint8_t infotm_nand_read_byte(struct mtd_info *mtd)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	uint8_t data = 0;

	if (infotm_chip->infotm_nand_read_byte)
		infotm_chip->infotm_nand_read_byte(infotm_chip, &data, 1, 0);

	return data;
}

static int infotm_nand_read_otp_block(struct infotm_nand_chip *infotm_chip, int chipnr, unsigned char *otp_buf)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct infotm_nand_retry_parameter *retry_parameter = (struct infotm_nand_retry_parameter *)infotm_nand_dev->resv1;

	chip->pagebuf = -1;
	if (infotm_chip->valid_chip[chipnr]) {
		if (infotm_chip->mfr_type == NAND_MFR_HYNIX) {
			infotm_chip->infotm_nand_select_chip(infotm_chip, chipnr);
			//infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_RESET, -1, -1, chipnr);
			chip->cmd_ctrl(mtd, NAND_CMD_RESET, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);

			chip->cmd_ctrl(mtd, 0x36, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			chip->cmd_ctrl(mtd, retry_parameter->otp_parameter[0], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_write_byte(infotm_chip, (uint8_t)retry_parameter->otp_parameter[1]);
			chip->cmd_ctrl(mtd, retry_parameter->otp_parameter[2], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_write_byte(infotm_chip, (uint8_t)retry_parameter->otp_parameter[3]);

			chip->cmd_ctrl(mtd, 0x16, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			chip->cmd_ctrl(mtd, 0x17, NAND_NCE | NAND_CLE);
			chip->cmd_ctrl(mtd, 0x04, NAND_NCE | NAND_CLE);
			chip->cmd_ctrl(mtd, 0x19, NAND_NCE | NAND_CLE);
			chip->cmd_ctrl(mtd, 0x00, NAND_NCE | NAND_CLE);
			chip->cmd_ctrl(mtd, 0x00, NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
			chip->cmd_ctrl(mtd, 0x00, NAND_NCE | NAND_ALE);
			chip->cmd_ctrl(mtd, 0x00, NAND_NCE | NAND_ALE);
			chip->cmd_ctrl(mtd, 0x02, NAND_NCE | NAND_ALE);
			chip->cmd_ctrl(mtd, 0x00, NAND_NCE | NAND_ALE);
			chip->cmd_ctrl(mtd, NAND_CMD_READSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);

			chip->read_buf(mtd, chip->buffers->databuf, HYNIX_OTP_SIZE);

			chip->cmd_ctrl(mtd, NAND_CMD_RESET, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			chip->cmd_ctrl(mtd, 0x38, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			infotm_chip->infotm_nand_cmdfifo_start(infotm_chip, 0);

			memcpy(otp_buf, chip->buffers->databuf, HYNIX_OTP_SIZE);
		}
	}
	return 0;
}

static int infotm_nand_read_default_para(struct infotm_nand_chip *infotm_chip, int chipnr, unsigned char *otp_buf)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	struct infotm_nand_retry_parameter *retry_parameter = (struct infotm_nand_retry_parameter *)infotm_nand_dev->resv1;
	int i;

	if (infotm_chip->valid_chip[chipnr]) {
		if (infotm_chip->mfr_type == NAND_MFR_HYNIX) {
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			chip->cmd_ctrl(mtd, NAND_CMD_RESET, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			infotm_chip->infotm_nand_wait_devready(infotm_chip, chipnr);
			chip->cmd_ctrl(mtd, 0x37, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
			for (i=0; i<retry_parameter->register_num; i++) {
				chip->cmd_ctrl(mtd, retry_parameter->register_addr[i], NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);
				otp_buf[i] = chip->read_byte(mtd);
			}
		}
	}

	return 0;
}

static int infotm_nand_base_get_retry_table(struct infotm_nand_chip *infotm_chip)
{
	struct nand_chip *chip = &infotm_chip->chip;
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct cdbg_cfg_nand *infotm_nand_dev = infotm_chip->infotm_nand_dev;
	char *otp_buf = chip->buffers->databuf;
	//struct infotm_nand_para_save *infotm_nand_save;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	struct infotm_nand_retry_parameter *retry_parameter;
	int error = 0, i = 0, j, para_offset = 0, para_num, otp_copy, chipnr, chip_start = 0, read_chip_num, cur_retry_level = 0;

	if ((infotm_nand_dev == NULL) || (infotm_nand_dev->resv1 == 0))
		return 0;

	chip->pagebuf = -1;
	//infotm_nand_save = (struct infotm_nand_para_save *)imapx800_chip->resved_mem;
	if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
		read_chip_num = infotm_chip->chip_num;
		list_for_each_safe(l, n, &imapx800_chip->chip_list) {
			infotm_chip_temp = chip_list_to_imapx800(l);
			if ((infotm_chip_temp != infotm_chip) && (infotm_chip_temp->retry_level_support > 0)) {
				memcpy(&infotm_chip->retry_parameter[0], &infotm_chip_temp->retry_parameter[0], sizeof(struct infotm_nand_retry_parameter));
				infotm_chip->retry_level[0] = infotm_chip_temp->retry_level[0];
				chip_start = 1;
				break;
			}
		}
	} else {
		chip_start = 0;
		read_chip_num = 1;
		list_for_each_safe(l, n, &imapx800_chip->chip_list) {
			infotm_chip_temp = chip_list_to_imapx800(l);
			if ((infotm_chip_temp != infotm_chip) && (infotm_chip_temp->retry_level_support > 0)) {
				memcpy(&infotm_chip->retry_parameter[0], &infotm_chip_temp->retry_parameter[0], sizeof(struct infotm_nand_retry_parameter));
				infotm_chip->retry_level[0] = infotm_chip_temp->retry_level[0];
				chip_start = 1;
				break;
			}
		}
	}

	NAND_DEBUG(KERN_WARNING,"infotm nand retry table read from nand \n");
	retry_parameter = (struct infotm_nand_retry_parameter *)infotm_nand_dev->resv1;
	infotm_chip->retry_level_support = retry_parameter->retry_level - 1;
	for (chipnr=chip_start; chipnr<read_chip_num; chipnr++) {
		if (infotm_chip->valid_chip[chipnr]) {
			i = 0;
			retry_parameter = &infotm_chip->retry_parameter[chipnr];
			memcpy(retry_parameter, (unsigned char *)infotm_nand_dev->resv1, sizeof(struct infotm_nand_retry_parameter));
			if (retry_parameter->parameter_source == PARAMETER_FROM_NAND_OTP) {
				if (infotm_chip->mfr_type == NAND_MFR_HYNIX)
					para_offset = HYNIX_OTP_OFFSET;
				error = infotm_nand_read_otp_block(infotm_chip, chipnr, otp_buf);
				if (error) {
					NAND_DEBUG(KERN_WARNING,"infotm nand read otp failed %d\n", error);
					return error;
				}

				para_num = (retry_parameter->retry_level*retry_parameter->register_num);
				otp_copy = 0;
				do {
					for (i=para_offset; i<para_num; i++) {
						if (((otp_buf[i] | otp_buf[i + para_num]) != 0xFF) || ((otp_buf[i] & otp_buf[i + para_num]) != 0)) {
							para_offset += (2 * para_num);
							break;
						}
					}
					if (i >= para_num)
						break;
					otp_copy++;
				} while (otp_copy < (HYNIX_OTP_SIZE / (2 * para_num)));
				if (otp_copy >= (HYNIX_OTP_SIZE / (2 * para_num)))
					return -ENOENT;

				for (i=0; i<retry_parameter->retry_level; i++) {
					NAND_DEBUG(KERN_WARNING,"level%d: ", i);
					for (j=0; j<retry_parameter->register_num; j++) {
						retry_parameter->parameter_offset[i][j] = otp_buf[para_offset+i*retry_parameter->register_num+j];
						NAND_DEBUG(KERN_WARNING," %x ", (uint8_t)retry_parameter->parameter_offset[i][j]);
					}
					NAND_DEBUG(KERN_WARNING,"\n");
				}
			} else if (retry_parameter->parameter_source == PARAMETER_FROM_NAND) {
				error = infotm_nand_read_default_para(infotm_chip, chipnr, otp_buf);
				if (error) {
					NAND_DEBUG(KERN_WARNING,"infotm nand read default failed %d\n", error);
					return error;
				}
				cur_retry_level = infotm_chip->retry_level[chipnr];
				NAND_DEBUG(KERN_WARNING,"level%d: ", i);
				for (j=0; j<retry_parameter->register_num; j++) {
					retry_parameter->parameter_offset[0][j] = otp_buf[j] - retry_parameter->parameter_offset[cur_retry_level][j];
					NAND_DEBUG(KERN_WARNING," %x ", (uint8_t)retry_parameter->parameter_offset[0][j]);
				}
				NAND_DEBUG(KERN_WARNING,"\n");
				for (i=1; i<retry_parameter->retry_level; i++) {
					NAND_DEBUG(KERN_WARNING,"level%d: ", i);
					for (j=0; j<retry_parameter->register_num; j++) {
						retry_parameter->parameter_offset[i][j] = retry_parameter->parameter_offset[0][j] + retry_parameter->parameter_offset[i][j];
						NAND_DEBUG(KERN_WARNING," %x ", (uint8_t)retry_parameter->parameter_offset[i][j]);
					}
					NAND_DEBUG(KERN_WARNING,"\n");
				}
			}
		}
	}

	return error;
}

#if 0
static int infotm_nand_scrub_all_chip(struct mtd_info *mtd)
{
	struct erase_info infotm_erase_info;

	memset(&infotm_erase_info, 0, sizeof(struct erase_info));
	infotm_erase_info.mtd = mtd;
	infotm_erase_info.addr = 0;
	infotm_erase_info.addr *= mtd->erasesize;
	infotm_erase_info.len = mtd->size;
	NAND_DEBUG("scrub_all_chip %llx \n", infotm_erase_info.addr);
	mtd->_erase(mtd, &infotm_erase_info);

	return 0;
}

static void infotm_nand_creat_bbt(struct infotm_nand_chip *infotm_chip)
{
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct infotm_nand_bbt_info *nand_bbt_info;
	struct nand_bbt_t *nand_bbt;
	int error = 0, total_blk_per_chip, blk_num, phys_erase_shift, i;
	unsigned plane_num, valid_chip_num = 0;

	nand_bbt_info = &infotm_chip->infotm_nandenv_info->nand_bbt_info;
	nand_bbt_info->total_bad_num = 0;
	phys_erase_shift = fls(mtd->erasesize) - 1;
	total_blk_per_chip = (int)(mtd->size >> phys_erase_shift);
	total_blk_per_chip *= infotm_chip->plane_num;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
		}
	}
	plane_num = infotm_chip->plane_num;
	mtd->erasesize /= plane_num*valid_chip_num;
	mtd->writesize /= plane_num*valid_chip_num;
	mtd->writebufsize /= plane_num*valid_chip_num;
	mtd->oobsize /= plane_num*valid_chip_num;
	infotm_chip->plane_num = 1;

	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			for (blk_num=0; blk_num<total_blk_per_chip; blk_num++) {
				error = infotm_nand_phy_block_bad(infotm_chip, blk_num, i);
				if (error) {
					nand_bbt = &nand_bbt_info->nand_bbt[nand_bbt_info->total_bad_num++];
					nand_bbt->blk_addr = blk_num;
					nand_bbt->plane_num = blk_num % plane_num;
					nand_bbt->chip_num = i;
					nand_bbt->bad_type = SHIPMENT_BAD;
					NAND_DEBUG("infotm nand found shipment bad blk %d chip %d \n", blk_num, i);
				}
			}
		}
	}

	valid_chip_num = 0;
	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
		}
	}
	mtd->erasesize *= plane_num*valid_chip_num;
	mtd->writesize *= plane_num*valid_chip_num;
	mtd->writebufsize *= plane_num*valid_chip_num;
	mtd->oobsize *= plane_num*valid_chip_num;
	infotm_chip->plane_num = plane_num;

	memcpy(nand_bbt_info->bbt_head_magic, BBT_HEAD_MAGIC, 4);
	memcpy(nand_bbt_info->bbt_tail_magic, BBT_TAIL_MAGIC, 4);
	return;
}
#endif

static int infotm_nand_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	rtcbit_set("retry_reboot", 0);
	return 0;
}

static int infotm_nand_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct nand_chip *chip = mtd->priv;
	spinlock_t *lock = &chip->controller->lock;

	spin_lock(lock);
	chip->state = FL_READY;
	spin_unlock(lock);

	return 0;
}

#if 0
static int infotm_nand_reboot_notifier(struct notifier_block *nb, unsigned long priority, void * arg)
{
	struct infotm_nand_chip *infotm_chip = container_of(nb, struct infotm_nand_chip, nb);
	int i;

	for (i=0; i<infotm_chip->chip_num; i++) {
		if ((infotm_chip->valid_chip[i]) && (infotm_chip->retry_level[i] != 0)) {
			infotm_chip->infotm_nand_set_retry_level(infotm_chip, i, 0);
		}
	}
	NAND_DEBUG("infotm_nand_reboot_notifier \n");

	return 0;
}
#endif

#ifdef CONFIG_INFOTM_NAND_ENV
static int infotm_nand_write_env(struct mtd_info *mtd, loff_t offset, u_char *buf)
{
	struct env_oobinfo_t *env_oobinfo;
	int error = 0;
	loff_t addr = 0;
	size_t amount_saved = 0;
	size_t len;
	struct mtd_oob_ops infotm_oob_ops;
	unsigned char *data_buf;
	unsigned char env_oob_buf[sizeof(struct env_oobinfo_t)];

	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	data_buf = kzalloc(mtd->writesize, GFP_KERNEL);
	if (data_buf == NULL)
		return -ENOMEM;

	addr = offset;
	env_oobinfo = (struct env_oobinfo_t *)env_oob_buf;
	memcpy(env_oobinfo->name, ENV_NAND_MAGIC, 4);
	env_oobinfo->ec = infotm_chip->infotm_nandenv_info->env_valid_node->ec;
	env_oobinfo->timestamp = infotm_chip->infotm_nandenv_info->env_valid_node->timestamp;
	env_oobinfo->status_page = 1;

	while (amount_saved < CONFIG_ENV_BBT_SIZE ) {

		infotm_oob_ops.mode = MTD_OOB_AUTO;
		infotm_oob_ops.len = mtd->writesize;
		infotm_oob_ops.ooblen = sizeof(struct env_oobinfo_t);
		infotm_oob_ops.ooboffs = 0;
		infotm_oob_ops.datbuf = data_buf;
		infotm_oob_ops.oobbuf = env_oob_buf;

		memset((unsigned char *)infotm_oob_ops.datbuf, 0x0, mtd->writesize);
		len = min(mtd->writesize, CONFIG_ENV_BBT_SIZE - amount_saved);
		memcpy((unsigned char *)infotm_oob_ops.datbuf, buf + amount_saved, len);

		NAND_DEBUG(KERN_WARNING,"infotm nand save env %llx \n", addr);
		error = mtd->_write_oob(mtd, addr, &infotm_oob_ops);
		if (error) {
			NAND_DEBUG(KERN_WARNING,"blk check good but write failed: %llx, %d\n", (uint64_t)addr, error);
			kfree(data_buf);
			return 1;
		}

		addr += mtd->writesize;;
		amount_saved += mtd->writesize;
	}
	if (amount_saved < CONFIG_ENV_BBT_SIZE) {
		kfree(data_buf);
		return 1;
	}

	infotm_chip->infotm_nandenv_info->env_valid = 1;
	kfree(data_buf);
	return 0;
}

static int infotm_nand_save_env(struct mtd_info *mtd, u_char *buf)
{
	struct infotm_nand_bbt_info *nand_bbt_info;
	struct env_free_node_t *env_free_node, *env_tmp_node;
	int error = 0, pages_per_blk, i = 1;
	loff_t addr = 0;
	struct erase_info infotm_env_erase_info;
	env_t *env_ptr = (env_t *)buf;

	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	if (!infotm_chip->infotm_nandenv_info->env_init) 
		return 1;

	pages_per_blk = mtd->erasesize / mtd->writesize;
	if ((mtd->writesize < CONFIG_ENV_BBT_SIZE) && (infotm_chip->infotm_nandenv_info->env_valid == 1))
		i = (CONFIG_ENV_BBT_SIZE + mtd->writesize - 1) / mtd->writesize;

	if (infotm_chip->infotm_nandenv_info->env_valid) {
		infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr += i;
		if ((infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr + i) > pages_per_blk) {

			env_free_node = kzalloc(sizeof(struct env_free_node_t), GFP_KERNEL);
			if (env_free_node == NULL)
				return -ENOMEM;

			env_free_node->phy_blk_addr = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
			env_free_node->ec = infotm_chip->infotm_nandenv_info->env_valid_node->ec;
			env_tmp_node = infotm_chip->infotm_nandenv_info->env_free_node;
			while (env_tmp_node->next != NULL) {
				env_tmp_node = env_tmp_node->next;
			}
			env_tmp_node->next = env_free_node;

			env_tmp_node = infotm_chip->infotm_nandenv_info->env_free_node;
			infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr = env_tmp_node->phy_blk_addr;
			infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr = 0;
			infotm_chip->infotm_nandenv_info->env_valid_node->ec = env_tmp_node->ec;
			infotm_chip->infotm_nandenv_info->env_valid_node->timestamp += 1;
			infotm_chip->infotm_nandenv_info->env_free_node = env_tmp_node->next;
			kfree(env_tmp_node);
		}
	} else {

		env_tmp_node = infotm_chip->infotm_nandenv_info->env_free_node;
		infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr = env_tmp_node->phy_blk_addr;
		infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr = 0;
		infotm_chip->infotm_nandenv_info->env_valid_node->ec = env_tmp_node->ec;
		infotm_chip->infotm_nandenv_info->env_valid_node->timestamp += 1;
		infotm_chip->infotm_nandenv_info->env_free_node = env_tmp_node->next;
		kfree(env_tmp_node);
	}

	addr = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
	addr *= mtd->erasesize;
	addr += infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr * mtd->writesize;
	if (infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr == 0) {

		memset(&infotm_env_erase_info, 0, sizeof(struct erase_info));
		infotm_env_erase_info.mtd = mtd;
		infotm_env_erase_info.addr = addr;
		infotm_env_erase_info.len = mtd->erasesize;

		error = mtd->_erase(mtd, &infotm_env_erase_info);
		if (error) {
			NAND_DEBUG(KERN_WARNING,"env free blk erase failed %d\n", error);
			mtd->_block_markbad(mtd, addr);
			return error;
		}
		infotm_chip->infotm_nandenv_info->env_valid_node->ec++;
	}

	nand_bbt_info = &infotm_chip->infotm_nandenv_info->nand_bbt_info;
	if ((!memcmp(nand_bbt_info->bbt_head_magic, BBT_HEAD_MAGIC, 4)) && (!memcmp(nand_bbt_info->bbt_tail_magic, BBT_TAIL_MAGIC, 4))) {
		memcpy(env_ptr->data, infotm_chip->infotm_nandenv_info->nand_bbt_info.bbt_head_magic, sizeof(struct infotm_nand_bbt_info));
		env_ptr->crc = (crc32((0 ^ 0xffffffffL), env_ptr->data, ENV_BBT_SIZE) ^ 0xffffffffL);
	}

	if (infotm_nand_write_env(mtd, addr, (u_char *) env_ptr)) {
		NAND_DEBUG(KERN_WARNING,"update nand env FAILED!\n");
		return 1;
	}

	return error;
}

static int infotm_nand_read_env (struct mtd_info *mtd, size_t offset, u_char * buf)
{
	struct env_oobinfo_t *env_oobinfo;
	int error = 0;
	size_t addr = 0;
	size_t amount_loaded = 0;
	size_t len;
	struct mtd_oob_ops infotm_oob_ops;
	unsigned char *data_buf;
	unsigned char env_oob_buf[sizeof(struct env_oobinfo_t)];

	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	if (!infotm_chip->infotm_nandenv_info->env_valid)
		return 1;

	addr = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
	addr *= mtd->erasesize;
	addr += infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr * mtd->writesize;

	data_buf = kzalloc(mtd->writesize, GFP_KERNEL);
	if (data_buf == NULL)
		return -ENOMEM;

	env_oobinfo = (struct env_oobinfo_t *)env_oob_buf;
	while (amount_loaded < CONFIG_ENV_BBT_SIZE ) {

		infotm_oob_ops.mode = MTD_OOB_AUTO;
		infotm_oob_ops.len = mtd->writesize;
		infotm_oob_ops.ooblen = sizeof(struct env_oobinfo_t);
		infotm_oob_ops.ooboffs = 0;
		infotm_oob_ops.datbuf = data_buf;
		infotm_oob_ops.oobbuf = env_oob_buf;

		memset((unsigned char *)infotm_oob_ops.datbuf, 0x0, mtd->writesize);
		memset((unsigned char *)infotm_oob_ops.oobbuf, 0x0, infotm_oob_ops.ooblen);

		error = mtd->_read_oob(mtd, addr, &infotm_oob_ops);
		if ((error != 0) && (error != -EUCLEAN)) {
			NAND_DEBUG(KERN_WARNING,"blk check good but read failed: %llx, %d\n", (uint64_t)addr, error);
			kfree(data_buf);
			return 1;
		}

		if (memcmp(env_oobinfo->name, ENV_NAND_MAGIC, 4)) 
			NAND_DEBUG(KERN_WARNING,"invalid nand env magic: %llx\n", (uint64_t)addr);

		addr += mtd->writesize;
		len = min(mtd->writesize, CONFIG_ENV_BBT_SIZE - amount_loaded);
		memcpy(buf + amount_loaded, data_buf, len);
		amount_loaded += mtd->writesize;
	}
	if (amount_loaded < CONFIG_ENV_BBT_SIZE) {
		kfree(data_buf);
		return 1;
	}

	kfree(data_buf);
	return 0;
}
#endif

#if 0
static int infotm_nand_check_uboot_item(struct infotm_nand_chip *infotm_chip)
{
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct nand_chip *chip = &infotm_chip->chip;
	struct infotm_nand_chip *infotm_chip_temp;
	struct list_head *l, *n;
	int error = 0, start_blk, para_reserved_blks, uboot0_blk = 0, uboot1_blk = 0, item_blk = 0, bad_blk_cnt = 0, pages_per_blk;

	if (imapx800_chip->drv_ver_changed)
		return 0;
	pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));
	para_reserved_blks = IMAPX800_PARA_SAVE_PAGES / pages_per_blk;
	start_blk = para_reserved_blks;
	do {

		if (uboot0_blk < IMAPX800_BOOT_COPY_NUM) {
			error = infotm_nand_phy_block_bad(infotm_chip, start_blk, 0);
			if (error) {
				NAND_DEBUG("infotm nand uboot0 found bad blk %d \n", start_blk);
				start_blk++;
				bad_blk_cnt++;
				continue;
			}

			list_for_each_safe(l, n, &imapx800_chip->chip_list) {
				infotm_chip_temp = chip_list_to_imapx800(l);
				infotm_chip_temp->uboot0_blk[uboot0_blk] = start_blk;
			}
			uboot0_blk++;
			start_blk++;
			continue;
		}

		if (uboot1_blk < IMAPX800_BOOT_COPY_NUM) {
			error = infotm_nand_phy_block_bad(infotm_chip, start_blk, 0);
			if (error) {
				NAND_DEBUG("infotm nand uboot1 found bad blk %d \n", start_blk);
				start_blk++;
				bad_blk_cnt++;
				continue;
			}

			list_for_each_safe(l, n, &imapx800_chip->chip_list) {
				infotm_chip_temp = chip_list_to_imapx800(l);
				infotm_chip_temp->uboot1_blk[uboot1_blk] = start_blk;
			}
			uboot1_blk++;
			start_blk++;
			continue;
		}

		if (item_blk < IMAPX800_BOOT_COPY_NUM) {
			error = infotm_nand_phy_block_bad(infotm_chip, start_blk, 0);
			if (error) {
				NAND_DEBUG("infotm nand item found bad blk %d \n", start_blk);
				start_blk++;
				bad_blk_cnt++;
				continue;
			}

			list_for_each_safe(l, n, &imapx800_chip->chip_list) {
				infotm_chip_temp = chip_list_to_imapx800(l);
				infotm_chip_temp->item_blk[item_blk] = start_blk;
			}
			item_blk++;
			start_blk++;
			continue;
		}

		if ((uboot0_blk >= IMAPX800_BOOT_COPY_NUM) && (uboot1_blk >= IMAPX800_BOOT_COPY_NUM) && (item_blk >= IMAPX800_BOOT_COPY_NUM))
			break;

		start_blk++;
	} while (bad_blk_cnt < IMAPX800_UBOOT_MAX_BAD);

	if (bad_blk_cnt >= IMAPX800_UBOOT_MAX_BAD)
		NAND_DEBUG("too much bad blks for uboot1 and item \n");

	return 0;
}
#endif

#ifdef CONFIG_INFOTM_NAND_ENV
static int infotm_nand_env_init(struct mtd_info *mtd)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct nand_chip *chip = &infotm_chip->chip;
	struct infotm_nand_chip *infotm_chip_temp;
	struct infotm_nand_platform *plat;
	struct list_head *l, *n;
	struct env_oobinfo_t *env_oobinfo;
	struct env_free_node_t *env_free_node, *env_tmp_node, *env_prev_node;
	int error = 0, start_blk, env_blk, i, pages_per_blk, bad_blk_cnt = 0, magic_blk_found, max_env_blk;
	loff_t offset;
	unsigned char *data_buf;
	struct mtd_oob_ops infotm_oob_ops;
	unsigned char env_oob_buf[sizeof(struct env_oobinfo_t)];
	unsigned valid_chip_num = 0;

	if (imapx800_chip->drv_ver_changed) {
		//infotm_nand_scrub_all_chip(mtd);
		list_for_each_safe(l, n, &imapx800_chip->chip_list) {
			infotm_chip_temp = chip_list_to_imapx800(l);
			plat = infotm_chip_temp->platform;
			if (!strncmp((char*)plat->name, NAND_NORMAL_NAME, strlen((const char*)NAND_NORMAL_NAME))) {
				imapx800_chip->drv_ver_changed = 0;
				error = infotm_nand_check_uboot_item(infotm_chip_temp);
				if (error)
					NAND_DEBUG(KERN_WARNING,"env have not found uboot1 item blks\n");
				break;
			}
		}
	} else {
		list_for_each_safe(l, n, &imapx800_chip->chip_list) {
			infotm_chip_temp = chip_list_to_imapx800(l);
			if (infotm_chip_temp != infotm_chip) {
				for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++) {
					infotm_chip->uboot0_blk[i] = infotm_chip_temp->uboot0_blk[i];
					infotm_chip->uboot1_blk[i] = infotm_chip_temp->uboot1_blk[i];
					infotm_chip->item_blk[i] = infotm_chip_temp->item_blk[i];
				}
				break;
			}
		}
	}

	for (i=0; i<infotm_chip->chip_num; i++) {
		if (infotm_chip->valid_chip[i]) {
			valid_chip_num++;
		}
	}

	data_buf = kzalloc(mtd->writesize, GFP_KERNEL);
	if (data_buf == NULL)
		return -ENOMEM;

	infotm_chip->infotm_nandenv_info = kzalloc(sizeof(struct infotm_nandenv_info_t), GFP_KERNEL);
	if (infotm_chip->infotm_nandenv_info == NULL) {
		kfree(data_buf);
		return -ENOMEM;
	}

	infotm_chip->infotm_nandenv_info->mtd = mtd;
	infotm_chip->infotm_nandenv_info->env_valid_node = kzalloc(sizeof(struct env_valid_node_t), GFP_KERNEL);
	if (infotm_chip->infotm_nandenv_info->env_valid_node == NULL) {
		kfree(data_buf);
		return -ENOMEM;
	}
	infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr = -1;

	max_env_blk = NAND_BBT_BLK_NUM;
	pages_per_blk = (1 << (chip->phys_erase_shift - chip->page_shift));
	start_blk = IMAPX800_PARA_SAVE_PAGES / pages_per_blk;
	env_oobinfo = (struct env_oobinfo_t *)env_oob_buf;

	env_blk = 0;
	bad_blk_cnt = 0;
	do {

		magic_blk_found = 0;
		for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++) {
			if ((start_blk+env_blk) == (infotm_chip->uboot0_blk[i] / infotm_chip->plane_num)) {
				start_blk++;
				magic_blk_found = 1;
				break;
			}
		}
		if (magic_blk_found)
			continue;
		for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++) {
			if ((start_blk+env_blk) == (infotm_chip->uboot1_blk[i] / infotm_chip->plane_num)) {
				start_blk++;
				magic_blk_found = 1;
				break;
			}
		}
		if (magic_blk_found)
			continue;
		for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++) {
			if ((start_blk+env_blk) == (infotm_chip->item_blk[i] / infotm_chip->plane_num)) {
				start_blk++;
				magic_blk_found = 1;
				break;
			}
		}
		if (magic_blk_found)
			continue;

		error = infotm_nand_logic_block_bad(infotm_chip, (start_blk + env_blk));
		if (error) {
			bad_blk_cnt++;
			start_blk++;
			continue;
		}

		offset = mtd->erasesize;
		offset *= (start_blk+env_blk);
		infotm_oob_ops.mode = MTD_OOB_AUTO;
		infotm_oob_ops.len = 0;
		infotm_oob_ops.ooblen = sizeof(struct env_oobinfo_t);
		infotm_oob_ops.ooboffs = 0;
		infotm_oob_ops.datbuf = NULL;
		infotm_oob_ops.oobbuf = env_oob_buf;
		memset((unsigned char *)infotm_oob_ops.oobbuf, 0x0, infotm_oob_ops.ooblen);

		error = mtd->_read_oob(mtd, offset, &infotm_oob_ops);
		if ((error != 0) && (error != -EUCLEAN)) {
			NAND_DEBUG(KERN_WARNING,"blk check good but read failed: %llx, %d\n", (uint64_t)offset, error);
			bad_blk_cnt++;
			env_blk++;
			continue;
		}

		infotm_chip->infotm_nandenv_info->env_init = 1;
		if (!memcmp(env_oobinfo->name, ENV_NAND_MAGIC, 4)) {
			infotm_chip->infotm_nandenv_info->env_valid = 1;
			if (infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr >= 0) {
				env_free_node = kzalloc(sizeof(struct env_free_node_t), GFP_KERNEL);
				if (env_free_node == NULL)
					return -ENOMEM;

				env_free_node->dirty_flag = 1;
				if (env_oobinfo->timestamp > infotm_chip->infotm_nandenv_info->env_valid_node->timestamp) {

					env_free_node->phy_blk_addr = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
					env_free_node->ec = infotm_chip->infotm_nandenv_info->env_valid_node->ec;
					infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr = (start_blk+env_blk);
					infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr = 0;
					infotm_chip->infotm_nandenv_info->env_valid_node->ec = env_oobinfo->ec;
					infotm_chip->infotm_nandenv_info->env_valid_node->timestamp = env_oobinfo->timestamp;	
				} else {
					env_free_node->phy_blk_addr = (start_blk + env_blk);
					env_free_node->ec = env_oobinfo->ec;
				}
				if (infotm_chip->infotm_nandenv_info->env_free_node == NULL)
					infotm_chip->infotm_nandenv_info->env_free_node = env_free_node;
				else {
					env_tmp_node = infotm_chip->infotm_nandenv_info->env_free_node;
					while (env_tmp_node->next != NULL) {
						env_tmp_node = env_tmp_node->next;
					}
					env_tmp_node->next = env_free_node;
				}
			} else {

				infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr = (start_blk+env_blk);
				infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr = 0;
				infotm_chip->infotm_nandenv_info->env_valid_node->ec = env_oobinfo->ec;
				infotm_chip->infotm_nandenv_info->env_valid_node->timestamp = env_oobinfo->timestamp;	
			}
		} else if (env_blk < max_env_blk) {
			env_free_node = kzalloc(sizeof(struct env_free_node_t), GFP_KERNEL);
			if (env_free_node == NULL)
				return -ENOMEM;

			env_free_node->phy_blk_addr = (start_blk+env_blk);
			env_free_node->ec = env_oobinfo->ec;
			if (infotm_chip->infotm_nandenv_info->env_free_node == NULL)
				infotm_chip->infotm_nandenv_info->env_free_node = env_free_node;
			else {
				env_tmp_node = infotm_chip->infotm_nandenv_info->env_free_node;
				env_prev_node = env_tmp_node;
				while (env_tmp_node != NULL) {
					if (env_tmp_node->dirty_flag == 1)
						break;
					env_prev_node = env_tmp_node;
					env_tmp_node = env_tmp_node->next;
				}
				if (env_prev_node == env_tmp_node) {
					env_free_node->next = env_tmp_node;
					infotm_chip->infotm_nandenv_info->env_free_node = env_free_node;
				} else {
					env_prev_node->next = env_free_node;
					env_free_node->next = env_tmp_node;
				}
			}
		}
		env_blk++;

	} while ((env_blk < max_env_blk) && (bad_blk_cnt < MAX_ENV_BAD_BLK_NUM));
	if ((bad_blk_cnt >= MAX_ENV_BAD_BLK_NUM) && (infotm_chip->infotm_nandenv_info->env_init == 0)) {
		kfree(data_buf);
		NAND_DEBUG(KERN_WARNING,"error too much sequence bad blk for env write\n");
		return -EFAULT;
	}

	bad_blk_cnt = 0;
	if ((infotm_chip->infotm_nandenv_info->env_valid == 0)
			&& (infotm_chip->infotm_nandenv_info->env_init)) {
		infotm_nand_creat_bbt(infotm_chip);

	} else if (infotm_chip->infotm_nandenv_info->env_valid == 1) {

		infotm_oob_ops.mode = MTD_OOB_AUTO;
		infotm_oob_ops.len = 0;
		infotm_oob_ops.ooblen = sizeof(struct env_oobinfo_t);
		infotm_oob_ops.ooboffs = 0;
		infotm_oob_ops.datbuf = NULL;
		infotm_oob_ops.oobbuf = env_oob_buf;

		for (i=0; i<pages_per_blk; i++) {

			memset((unsigned char *)infotm_oob_ops.oobbuf, 0x0, infotm_oob_ops.ooblen);
			offset = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
			offset *= mtd->erasesize;
			offset += i * mtd->writesize;
			error = mtd->_read_oob(mtd, offset, &infotm_oob_ops);
			if ((error != 0) && (error != -EUCLEAN)) {
				NAND_DEBUG(KERN_WARNING,"blk check good but read failed: %llx, %d\n", (uint64_t)offset, error);
				continue;
			}

			if (!memcmp(env_oobinfo->name, ENV_NAND_MAGIC, 4))
				infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr = i;
			else
				break;
		}
	}
	if ((mtd->writesize < CONFIG_ENV_BBT_SIZE) && (infotm_chip->infotm_nandenv_info->env_valid == 1)) {
		i = (CONFIG_ENV_BBT_SIZE + mtd->writesize - 1) / mtd->writesize;
		infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr -= (i - 1);
	}

	offset = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
	offset *= mtd->erasesize;
	offset += infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr * mtd->writesize;
	if (infotm_chip->infotm_nandenv_info->env_valid == 1)
		NAND_DEBUG(KERN_WARNING,"infotm nand env valid addr: %llx \n", (uint64_t)offset);
	else
		NAND_DEBUG(KERN_WARNING,"bbt creat first time\n");

	kfree(data_buf);
	return 0;
}

static int infotm_nand_update_env(struct mtd_info *mtd)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	env_t *env_ptr;
	loff_t offset;
	int error = 0;

	env_ptr = kzalloc(sizeof(env_t), GFP_KERNEL);
	if (env_ptr == NULL)
		return -ENOMEM;

	if (infotm_chip->infotm_nandenv_info->env_valid == 1) {

		offset = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
		offset *= mtd->erasesize;
		offset += infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr * mtd->writesize;

		error = infotm_nand_read_env (mtd, offset, (u_char *)env_ptr);
		if (error) {
			NAND_DEBUG(KERN_WARNING,"nand env read failed: %llx, %d\n", (uint64_t)offset, error);
			return error;
		}

		error = infotm_nand_save_env(mtd, (u_char *)env_ptr);
		if (error) {
			NAND_DEBUG(KERN_WARNING,"update env bbt failed %d \n", error);
			return error;
		}
	}

	return error;
}

static int infotm_nand_env_check(struct mtd_info *mtd)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct infotm_nand_bbt_info *nand_bbt_info;
	env_t *env_ptr;
	int error = 0;
	loff_t offset;

	error = infotm_nand_env_init(mtd);
	if (error)
		return error;

	env_ptr = kzalloc(sizeof(env_t), GFP_KERNEL);
	if (env_ptr == NULL)
		return -ENOMEM;

	if (infotm_chip->infotm_nandenv_info->env_valid == 1) {

		offset = infotm_chip->infotm_nandenv_info->env_valid_node->phy_blk_addr;
		offset *= mtd->erasesize;
		offset += infotm_chip->infotm_nandenv_info->env_valid_node->phy_page_addr * mtd->writesize;

		error = infotm_nand_read_env(mtd, offset, (u_char *)env_ptr);
		if (error) {
			NAND_DEBUG(KERN_WARNING,"nand env read failed: %llx, %d\n", (uint64_t)offset, error);
			goto exit;
		}
		if (env_ptr->crc != (crc32((0 ^ 0xffffffffL), env_ptr->data, ENV_BBT_SIZE) ^ 0xffffffffL))
			NAND_DEBUG(KERN_WARNING,"nand env crc error %x \n", env_ptr->crc);

		nand_bbt_info = (struct infotm_nand_bbt_info *)(env_ptr->data);
		if ((!memcmp(nand_bbt_info->bbt_head_magic, BBT_HEAD_MAGIC, 4)) && (!memcmp(nand_bbt_info->bbt_tail_magic, BBT_TAIL_MAGIC, 4)))
			memcpy((unsigned char *)infotm_chip->infotm_nandenv_info->nand_bbt_info.bbt_head_magic, (unsigned char *)nand_bbt_info, sizeof(struct infotm_nand_bbt_info));

	} else if (infotm_chip->infotm_nandenv_info->env_init) {

		memset((u_char *)env_ptr, 0xff, sizeof(env_t));
		error = infotm_nand_save_env(mtd, (u_char *)env_ptr);
		if (error) {
			NAND_DEBUG(KERN_WARNING,"nand env save failed: %d\n", error);
			goto exit;
		}
	}

exit:
	kfree(env_ptr);
	return 0;
}
#endif

#if 0
static int infotm_nand_check_drv_version(struct mtd_info *mtd)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct erase_info uboot0_erase_info;
	unsigned char *data_buf;
	unsigned char oob_buf[PAGE_MANAGE_SIZE];
	struct mtd_oob_ops infotm_oob_ops;
	int error = 0, i = 0, uboot0_rewrite = 0, start_cnt;
	loff_t offset;
	size_t retlen = 0;

	data_buf = kzalloc(infotm_chip->uboot0_size + mtd->writesize, GFP_KERNEL);
	if (data_buf == NULL)
		return -ENOMEM;

	infotm_oob_ops.mode = MTD_OOB_AUTO;
	infotm_oob_ops.len = mtd->writesize;
	infotm_oob_ops.ooblen = PAGE_MANAGE_SIZE;
	infotm_oob_ops.ooboffs = 0;
	infotm_oob_ops.oobbuf = oob_buf;
	do {
		offset = i * mtd->writesize;
		infotm_oob_ops.datbuf = data_buf + offset;

		memset(oob_buf, 0, PAGE_MANAGE_SIZE);
		error = mtd->_read_oob(mtd, offset, &infotm_oob_ops);
		if ((error != 0) && (error != -EUCLEAN)) {
			NAND_DEBUG("uboot 0 read failed %llx %d \n", offset, error);
			imapx800_chip->drv_ver_changed = 1;
		}

		if (*(uint16_t *)oob_buf != IMAPX800_VERSION_MAGIC) {
			NAND_DEBUG("infotm nand version %x \n", *(uint16_t *)oob_buf);
			imapx800_chip->drv_ver_changed = 1;
		} else {
			imapx800_chip->drv_ver_changed = 0;
			break;
		}
		i++;
	} while (i < (infotm_chip->uboot0_size / mtd->writesize));

	if (imapx800_chip->drv_ver_changed == 0) {

		offset = infotm_chip->uboot0_size + mtd->writesize;
		infotm_oob_ops.datbuf = data_buf;

		memset(oob_buf, 0, PAGE_MANAGE_SIZE);
		error = mtd->_read_oob(mtd, offset, &infotm_oob_ops);
		if ((error != 0) && (error != -EUCLEAN))
			NAND_DEBUG("uboot0 rewrite flag read failed %llx %d\n", offset, error);

		if (!strncmp((char*)(data_buf + sizeof(int)), INFOTM_UBOOT0_REWRITE_MAGIC, strlen((const char*)INFOTM_UBOOT0_REWRITE_MAGIC))) {
			start_cnt = (*(int *)(data_buf));
			NAND_DEBUG("infotm uboot0 %d %d \n", start_cnt, infotm_chip->uboot0_rewrite_count);
			if ((start_cnt % infotm_chip->uboot0_rewrite_count) == 0)
				uboot0_rewrite = 1;
		}

		if (uboot0_rewrite) {
			i = 0;
			do {
				offset = i * mtd->writesize;
				infotm_oob_ops.datbuf = data_buf + offset;

				memset(oob_buf, 0, PAGE_MANAGE_SIZE);
				error = mtd->_read_oob(mtd, offset, &infotm_oob_ops);
				if ((error != 0) && (error != -EUCLEAN)) {
					NAND_DEBUG("uboot0 rewrite read failed %llx %d\n", offset, error);
					break;;
				}

				i++;
			} while (i < ((infotm_chip->uboot0_size / mtd->writesize) + 1));

			memset(&uboot0_erase_info, 0, sizeof(struct erase_info));
			uboot0_erase_info.mtd = mtd;
			uboot0_erase_info.addr = 0;
			uboot0_erase_info.len = mtd->erasesize;

			mtd->_erase(mtd, &uboot0_erase_info);
			mtd->_write(mtd, 0, (infotm_chip->uboot0_size + mtd->writesize), &retlen, data_buf);
		}
	}

	kfree(data_buf);
	return error;
}
#endif

static int infotm_nand_scan_bbt(struct mtd_info *mtd)
{
	return 0;
}

#ifdef CONFIG_INFOTM_NAND_ENV
static struct mtd_info *nand_env_mtd = NULL;
#define NAND_ENV_DEVICE_NAME	"nand_env"
static int nand_env_open(struct inode * inode, struct file * filp)
{
	return 0;
}

static ssize_t nand_env_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	env_t *env_ptr = NULL;
	ssize_t read_size;
	int error = 0;
	if(*ppos == CONFIG_ENV_BBT_SIZE)
		return 0;

	if(*ppos >= CONFIG_ENV_BBT_SIZE) {
		NAND_DEBUG(KERN_ERR, "nand env: data access violation!\n");
		return -EFAULT;
	}

	env_ptr = kzalloc(sizeof(env_t), GFP_KERNEL);
	if (env_ptr == NULL)
		return -ENOMEM;

	error = infotm_nand_read_env (nand_env_mtd, 0, (u_char *)env_ptr);
	if (error) {
		NAND_DEBUG("nand_env_read: nand env read failed: %llx, %d\n", (uint64_t)*ppos, error);
		kfree(env_ptr);
		return -EFAULT;
	}

	if((*ppos + count) > CONFIG_ENV_BBT_SIZE)
		read_size = CONFIG_ENV_BBT_SIZE - *ppos;
	else
		read_size = count;

	if (copy_to_user(buf, (env_ptr + *ppos), read_size))
		return -EFAULT;

	*ppos += read_size;

	kfree(env_ptr);
	return read_size;
}

static ssize_t nand_env_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	u_char *env_ptr = NULL;
	ssize_t write_size;
	int error = 0;

	if(*ppos == CONFIG_ENV_BBT_SIZE)
		return 0;

	if(*ppos >= CONFIG_ENV_BBT_SIZE) {
		NAND_DEBUG(KERN_ERR, "nand env: data access violation!\n");
		return -EFAULT;
	}

	env_ptr = kzalloc(sizeof(env_t), GFP_KERNEL);
	if (env_ptr == NULL)
		return -ENOMEM;

	error = infotm_nand_read_env (nand_env_mtd, 0, (u_char *)env_ptr);
	if (error)  {
		NAND_DEBUG(KERN_WARNING,"nand_env_read: nand env read failed: %llx, %d\n", (uint64_t)*ppos, error);
		kfree(env_ptr);
		return -EFAULT;
	}

	if((*ppos + count) > CONFIG_ENV_BBT_SIZE)
		write_size = CONFIG_ENV_BBT_SIZE - *ppos;
	else
		write_size = count;

	if (copy_from_user((env_ptr + *ppos), buf, write_size))
		return -EFAULT;

	error = infotm_nand_save_env(nand_env_mtd, env_ptr);
	if (error)  {
		NAND_DEBUG(KERN_WARNING,"nand_env_read: nand env read failed: %llx, %d\n", (uint64_t)*ppos, error);
		kfree(env_ptr);
		return -EFAULT;
	}

	*ppos += write_size;

	kfree(env_ptr);
	return write_size;
}

static int nand_env_close(struct inode *inode, struct file *file)
{
	return 0;
}

static int nand_env_cls_suspend(struct device *dev, pm_message_t state)
{
	return 0;
}

static int nand_env_cls_resume(struct device *dev)
{
	return 0;
}


static struct class nand_env_class = {

	.name = "nand_env",
	.owner = THIS_MODULE,
	.suspend = nand_env_cls_suspend,
	.resume = nand_env_cls_resume,
};

static struct file_operations nand_env_fops = {
	.owner	= THIS_MODULE,
	.open	= nand_env_open,
	.read	= nand_env_read,
	.write	= nand_env_write,
	.release	= nand_env_close,
	//.ioctl	= nand_env_ioctl,
};

#endif

#ifdef CONFIG_INFOTM_NAND_ENV
static ssize_t show_nand_info(struct class *class, struct class_attribute *attr,	char *buf)
{
	struct infotm_nand_chip *infotm_chip = container_of(class, struct infotm_nand_chip, cls);

	NAND_DEBUG(KERN_WARNING,"mfr_type:\t\t0x%x\n", infotm_chip->mfr_type);
	NAND_DEBUG(KERN_WARNING,"onfi_mode:\t\t0x%x\n", infotm_chip->onfi_mode);
	NAND_DEBUG(KERN_WARNING,"options:\t\t0x%x\n", infotm_chip->options);
	NAND_DEBUG(KERN_WARNING,"page_size:\t\t0x%x\n", infotm_chip->page_size);
	NAND_DEBUG(KERN_WARNING,"block_size:\t\t0x%x\n", infotm_chip->block_size);
	NAND_DEBUG(KERN_WARNING,"oob_size:\t\t0x%x\n", infotm_chip->oob_size);
	NAND_DEBUG(KERN_WARNING,"virtual_page_size:\t\t0x%x\n", infotm_chip->virtual_page_size);
	NAND_DEBUG(KERN_WARNING,"virtual_block_size:\t\t0x%x\n", infotm_chip->virtual_block_size);
	NAND_DEBUG(KERN_WARNING,"plane_num:\t\t0x%x\n", infotm_chip->plane_num);
	NAND_DEBUG(KERN_WARNING,"chip_num:\t\t0x%x\n", infotm_chip->chip_num);
	NAND_DEBUG(KERN_WARNING,"internal_chipnr:\t\t0x%x\n", infotm_chip->internal_chipnr);
	NAND_DEBUG(KERN_WARNING,"internal_page_nums:\t\t0x%x\n", infotm_chip->internal_page_nums);
	NAND_DEBUG(KERN_WARNING,"internal_chip_shift:\t\t0x%x\n", infotm_chip->internal_chip_shift);
	NAND_DEBUG(KERN_WARNING,"ecc_mode:\t\t0x%x\n", infotm_chip->ecc_mode);
	NAND_DEBUG(KERN_WARNING,"ops_mode:\t\t0x%x\n", infotm_chip->ops_mode);
	NAND_DEBUG(KERN_WARNING,"cached_prog_status:\t\t0x%x\n", infotm_chip->cached_prog_status);
	NAND_DEBUG(KERN_WARNING,"max_ecc_mode:\t\t0x%x\n", infotm_chip->max_ecc_mode);

	return 0;
}

static ssize_t show_bbt_table(struct class *class, struct class_attribute *attr,	char *buf)
{
	struct infotm_nand_chip *infotm_chip = container_of(class, struct infotm_nand_chip, cls);
	struct infotm_nand_bbt_info *nand_bbt_info;
	struct nand_bbt_t *nand_bbt;
	int i;

	nand_bbt_info = &infotm_chip->infotm_nandenv_info->nand_bbt_info;

	if (infotm_chip->infotm_nandenv_info->env_valid == 1) {

		if (nand_bbt_info != NULL) {
			for (i=0; i<nand_bbt_info->total_bad_num; i++) {
				nand_bbt = &nand_bbt_info->nand_bbt[i];
				NAND_DEBUG(KERN_WARNING,"infotm nand bad blk %d chip %d type %d \n", nand_bbt->blk_addr, nand_bbt->chip_num, nand_bbt->bad_type);
			}
		}
	} else {
		NAND_DEBUG(KERN_WARNING,"infotm not found bbt \n");
	}

	return 0;
}

static ssize_t nand_page_dump(struct class *class, struct class_attribute *attr,	const char *buf, size_t count)
{
	struct infotm_nand_chip *infotm_chip = container_of(class, struct infotm_nand_chip, cls);
	struct mtd_info *mtd = &infotm_chip->mtd;

	struct mtd_oob_ops ops;
	loff_t off;
	loff_t addr;
	u_char *datbuf, *oobbuf, *p;
	int ret, i;
	NAND_DEBUG(KERN_DEBUG, "enter %s\n", __FUNCTION__);
	ret = sscanf(buf, "%llx", &off);

	datbuf = kmalloc(mtd->writesize + mtd->oobsize, GFP_KERNEL);
	oobbuf = kmalloc(mtd->oobsize, GFP_KERNEL);
	if (!datbuf || !oobbuf) {
		NAND_DEBUG(KERN_WARNING,"No memory for page buffer\n");
		return 1;
	}
	addr = ~((loff_t)mtd->writesize - 1);
	addr &= off;

	memset(&ops, 0, sizeof(ops));
	ops.datbuf = datbuf;
	ops.oobbuf = NULL; /* must exist, but oob data will be appended to ops.datbuf */
	ops.len = mtd->writesize;
	ops.ooblen = mtd->oobsize;
	ops.mode = MTD_OOB_RAW;
	i = mtd->_read_oob(mtd, addr, &ops);
	if (i < 0) {
		NAND_DEBUG(KERN_WARNING,"Error (%d) reading page %09llx\n", i, off);
		kfree(datbuf);
		kfree(oobbuf);
		return 1;
	}
	NAND_DEBUG(KERN_WARNING,"Page %09llx dump,page size %d:\n", off,mtd->writesize);
	i = (mtd->writesize + mtd->oobsize) >> 4;
	p = datbuf;

	while (i--) {
		NAND_DEBUG(KERN_WARNING,"\t%02x %02x %02x %02x %02x %02x %02x %02x"
				"  %02x %02x %02x %02x %02x %02x %02x %02x\n",
				p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
				p[8], p[9], p[10], p[11], p[12], p[13], p[14],
				p[15]);
		p += 16;
	}
	/*printf("OOB oob size %d:\n",nand->oobsize);
	  i = nand->oobsize >> 3;
	  while (i--) {
	  printf("\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
	  p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
	  p += 8;
	  }*/
	kfree(datbuf);
	kfree(oobbuf);
	NAND_DEBUG(KERN_DEBUG, "exit %s\n", __FUNCTION__);
	return count;
}

static ssize_t nand_page_read(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	struct infotm_nand_chip *infotm_chip = container_of(class, struct infotm_nand_chip, cls);
	struct mtd_info *mtd = &infotm_chip->mtd;
	struct mtd_oob_ops infotm_oob_ops;

	int page;
	loff_t addr;
	u_char *datbuf, *oobbuf, *p;
	size_t ret;
	int i;
	NAND_DEBUG(KERN_DEBUG, "enter %s\n", __FUNCTION__);
	ret = sscanf(buf, "%x", &page);

	datbuf = kmalloc(mtd->writesize + mtd->oobsize, GFP_KERNEL);
	oobbuf = kmalloc(mtd->oobsize, GFP_KERNEL);
	if (!datbuf || !oobbuf) {
		NAND_DEBUG(KERN_WARNING,"No memory for page buffer\n");
		return 1;
	}
	addr = page;
	addr *= mtd->writesize ;

	infotm_oob_ops.mode = MTD_OOB_AUTO;
	infotm_oob_ops.len = mtd->writesize;
	infotm_oob_ops.ooblen = mtd->oobavail;
	infotm_oob_ops.ooboffs = 0;
	infotm_oob_ops.datbuf = datbuf;
	infotm_oob_ops.oobbuf = oobbuf;

	mtd->_read_oob(mtd, addr, &infotm_oob_ops);
	if (ret < 0) {
		NAND_DEBUG(KERN_WARNING,"Error (%d) reading page %x\n", i, page);
		kfree(datbuf);
		kfree(oobbuf);
		return 1;
	}

	NAND_DEBUG(KERN_WARNING,"addr %llx page %x size %d:\n", addr, page, mtd->writesize);
	i = (mtd->writesize ) >> 4;
	p = datbuf;

	while (i--) {
		NAND_DEBUG(KERN_WARNING,"\t%02x %02x %02x %02x %02x %02x %02x %02x"
				"  %02x %02x %02x %02x %02x %02x %02x %02x\n",
				p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
				p[8], p[9], p[10], p[11], p[12], p[13], p[14],
				p[15]);
		p += 16;
	}

	NAND_DEBUG(KERN_WARNING,"\n\n");
	i = 1;
	p = oobbuf;
	while (i--) {
		NAND_DEBUG(KERN_WARNING,"\t%02x %02x %02x %02x %02x %02x %02x %02x"
				"  %02x %02x %02x %02x %02x %02x %02x %02x\n",
				p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
				p[8], p[9], p[10], p[11], p[12], p[13], p[14],
				p[15]);
		p += 16;
	}

	kfree(datbuf);
	kfree(oobbuf);
	NAND_DEBUG(KERN_DEBUG, "exit %s\n", __FUNCTION__);
	return count;
}

static ssize_t show_startup_cnt(struct class *class, struct class_attribute *attr,	char *buf)
{
	struct infotm_nand_chip *infotm_chip = container_of(class, struct infotm_nand_chip, cls);

	return sprintf(buf, "%d\n", infotm_chip->startup_cnt);
}

static ssize_t store_startup_cnt(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	struct infotm_nand_chip *infotm_chip = container_of(class, struct infotm_nand_chip, cls);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct mtd_info *mtd = NULL;
	struct infotm_nand_chip *infotm_chip_uboot = NULL;
	struct infotm_nand_platform *plat;
	struct list_head *l, *n;
	unsigned char *data_buf;
	size_t retlen = 0;
	loff_t from;

	sscanf(buf, "%d", &infotm_chip->startup_cnt);
	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip_uboot = chip_list_to_imapx800(l);
		plat = infotm_chip_uboot->platform;
		if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME)))
			break;
		else
			infotm_chip_uboot = NULL;
	}
	if (infotm_chip_uboot == NULL)
		return 1;

	NAND_DEBUG(KERN_WARNING,"infotm start cnt %d \n", infotm_chip->startup_cnt);
	if (infotm_chip_uboot && (infotm_chip_uboot->uboot0_rewrite_count > 0) && (infotm_chip->startup_cnt >= infotm_chip_uboot->uboot0_rewrite_count)) {
		if ((infotm_chip->startup_cnt % infotm_chip_uboot->uboot0_rewrite_count) == 0) {
			mtd = &infotm_chip_uboot->mtd;
			data_buf = kzalloc(mtd->writesize, GFP_KERNEL);
			if (data_buf == NULL) {
				NAND_DEBUG(KERN_WARNING,"No memory for uboot rewrite buffer\n");
				return -1;
			}

			memcpy(data_buf, (unsigned char *)infotm_chip, mtd->writesize);
			strcpy((char*)(data_buf + sizeof(int)), INFOTM_UBOOT0_REWRITE_MAGIC);
			*(int *)(data_buf) = infotm_chip->startup_cnt;
			from = infotm_chip_uboot->uboot0_size + mtd->writesize;
			mtd->_write(mtd, from, mtd->writesize, &retlen, data_buf);
			kfree(data_buf);
		}
	}
	if (infotm_chip->env_need_save) {
		infotm_chip->infotm_nand_saveenv();
		infotm_chip->env_need_save = 0;
	}

	/*imapx_pad_set_mode(1, 1, 25);
	imapx_pad_set_outdat(1, 1, 25);
	imapx_pad_set_dir(0, 1, 25);
	mdelay(100);
	imapx_pad_set_outdat(0, 1, 25);*/

	return count;
}

static struct class_attribute nand_class_attrs[] = {
	__ATTR(info,       S_IRUGO | S_IWUSR, show_nand_info,    NULL),
	__ATTR(bbt_table,       S_IRUGO | S_IWUSR, show_bbt_table,    NULL),
	__ATTR(page_dump,  S_IRUGO | S_IWUSR, NULL,    nand_page_dump),
	__ATTR(page_read,  S_IRUGO | S_IWUSR, NULL,    nand_page_read),
	__ATTR(startup_cnt,  (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH), show_startup_cnt,    store_startup_cnt),
	__ATTR_NULL
};
#endif

static int infotm_nand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{ 	
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct infotm_nand_bbt_info *nand_bbt_info = NULL;
	struct nand_bbt_t *nand_bbt = NULL;
	int blk_addr, mtd_erase_shift, i, j;

	if (infotm_chip->nand_bbt_info)
		nand_bbt_info = infotm_chip->nand_bbt_info;
	mtd_erase_shift = fls(mtd->erasesize) - 1;
	blk_addr = (int)(ofs >> mtd_erase_shift);

	if (nand_bbt_info != NULL) {

		for (j=0; j<nand_bbt_info->total_bad_num; j++) {
			nand_bbt = &nand_bbt_info->nand_bbt[j];
			if (blk_addr == nand_bbt->blk_addr/infotm_chip->plane_num)
				break;
		}
		if (j >= nand_bbt_info->total_bad_num) {
			blk_addr *= infotm_chip->plane_num;

			for (i=0; i<infotm_chip->chip_num; i++) {
				if (infotm_chip->valid_chip[i]) {

					nand_bbt = &nand_bbt_info->nand_bbt[nand_bbt_info->total_bad_num++];
					nand_bbt->blk_addr = blk_addr;
					nand_bbt->plane_num = blk_addr % infotm_chip->plane_num;
					nand_bbt->chip_num = i;
					nand_bbt->bad_type = USELESS_BAD;

					//if (infotm_nand_update_env(mtd))
						//NAND_DEBUG("update env bbt failed %d \n", blk_addr);
				}
			}
		} else {
			return 0;
		}
	}

	return 0;
}

int infotm_nand_init(struct infotm_nand_chip *infotm_chip)
{
	struct infotm_nand_platform *plat = infotm_chip->platform;
	struct infotm_nand_ecc_desc *ecc_supports = infotm_chip->ecc_desc;
	struct nand_chip *chip = &infotm_chip->chip;
	struct mtd_info *mtd = &infotm_chip->mtd;
	int err = 0, i = 0, options_selected;
#ifdef CONFIG_INFOTM_NAND_ENV
	int ret;
	struct device *devp;
	static dev_t nand_env_devno;
#endif

	options_selected = (plat->platform_nand_data.chip.options & NAND_ECC_OPTIONS_MASK);
	for (i=0; i<infotm_chip->max_ecc_mode; i++) {
		if (ecc_supports[i].ecc_mode == options_selected) {
			break;
		}
	}
	if (i < infotm_chip->max_ecc_mode) {
		chip->write_buf = infotm_nand_dma_write_buf;
		chip->read_buf = infotm_nand_dma_read_buf;
		if (!chip->block_bad)
			chip->block_bad = infotm_nand_block_bad;
		chip->block_markbad = infotm_nand_block_markbad;
		chip->ecc.read_page_raw = infotm_nand_read_page_raw;
		chip->ecc.write_page_raw = infotm_nand_write_page_raw;
		if (!chip->ecc.read_page)
			chip->ecc.read_page = infotm_nand_read_page_hwecc;
		if (!chip->ecc.write_page)
			chip->ecc.write_page = infotm_nand_write_page_hwecc;
		chip->ecc.read_oob  = infotm_nand_read_oob;
		chip->ecc.write_oob = infotm_nand_write_oob;

		chip->ecc.mode = NAND_ECC_HW;
		chip->ecc.size = NAND_ECC_UNIT_SIZE;
		chip->ecc.bytes = ecc_supports[i].ecc_bytes;
		infotm_chip->ecc_mode = ecc_supports[i].ecc_mode;
		infotm_chip->secc_mode = ecc_supports[i].secc_mode;
		infotm_chip->secc_bytes = ecc_supports[i].secc_bytes;
	} else {
		NAND_DEBUG(KERN_WARNING, "haven`t found any ecc mode just selected NAND_ECC_NONE\n");
		chip->write_buf = infotm_nand_dma_write_buf;
		chip->read_buf = infotm_nand_dma_read_buf;
		chip->ecc.read_page_raw = infotm_nand_read_page_raw;
		chip->ecc.write_page_raw = infotm_nand_write_page_raw;
		chip->ecc.mode = NAND_ECC_NONE;
		infotm_chip->ecc_mode = -1;
	}

	if (!infotm_chip->infotm_nand_hw_init)
		infotm_chip->infotm_nand_hw_init = infotm_platform_hw_init;
	if (!infotm_chip->infotm_nand_adjust_timing)
		infotm_chip->infotm_nand_adjust_timing = infotm_platform_adjust_timing;
	if (!infotm_chip->infotm_nand_options_confirm)
		infotm_chip->infotm_nand_options_confirm = infotm_platform_options_confirm;
	if (!infotm_chip->infotm_nand_cmd_ctrl)
		infotm_chip->infotm_nand_cmd_ctrl = infotm_platform_cmd_ctrl;
	if (!infotm_chip->infotm_nand_select_chip)
		infotm_chip->infotm_nand_select_chip = infotm_platform_select_chip;
	if (!infotm_chip->infotm_nand_get_controller)
		infotm_chip->infotm_nand_get_controller = infotm_platform_get_controller;
	if (!infotm_chip->infotm_nand_release_controller)
		infotm_chip->infotm_nand_release_controller = infotm_platform_release_controller;
	if (!infotm_chip->infotm_nand_write_byte)
		infotm_chip->infotm_nand_write_byte = infotm_platform_write_byte;
	if (!infotm_chip->infotm_nand_wait_devready)
		infotm_chip->infotm_nand_wait_devready = infotm_platform_wait_devready;
	if (!infotm_chip->infotm_nand_command)
		infotm_chip->infotm_nand_command = infotm_nand_base_command;
	if (!infotm_chip->infotm_nand_set_retry_level)
		infotm_chip->infotm_nand_set_retry_level = infotm_nand_base_set_retry_level;
	if (!infotm_chip->infotm_nand_set_eslc_para)
		infotm_chip->infotm_nand_set_eslc_para = infotm_nand_set_eslc_para;
	if (!infotm_chip->infotm_nand_get_retry_table)
		infotm_chip->infotm_nand_get_retry_table = infotm_nand_base_get_retry_table;
	if(!infotm_chip->infotm_nand_get_eslc_para)
		infotm_chip->infotm_nand_get_eslc_para = infotm_nand_get_eslc_para;
	if (!infotm_chip->infotm_nand_dma_read)
		infotm_chip->infotm_nand_dma_read = infotm_platform_dma_read;
	if (!infotm_chip->infotm_nand_dma_write)
		infotm_chip->infotm_nand_dma_write = infotm_platform_dma_write;
	if (!infotm_chip->infotm_nand_hwecc_correct)
		infotm_chip->infotm_nand_hwecc_correct = infotm_platform_hwecc_correct;
	//if (!infotm_chip->infotm_nand_check_uboot_item)
		//infotm_chip->infotm_nand_check_uboot_item = infotm_nand_check_uboot_item;

	chip->options |=  NAND_SKIP_BBTSCAN;
	chip->options |= NAND_NO_SUBPAGE_WRITE;
	chip->options |= NAND_BROKEN_XD;
	chip->options |= NAND_NO_READRDY;
	chip->options |= NAND_SUBPAGE_READ_HW;
	if (!chip->ecc.layout)
		chip->ecc.layout = &infotm_nand_oob;

	chip->select_chip = infotm_nand_select_chip;
	chip->cmd_ctrl = infotm_nand_cmd_ctrl;
	chip->dev_ready = infotm_nand_dev_ready;
	chip->verify_buf = infotm_nand_verify_buf;
	chip->read_byte = infotm_nand_read_byte;
	chip->ecc.read_subpage = infotm_nand_read_subpage;

	infotm_chip->chip_num = plat->platform_nand_data.chip.nr_chips;
	if (infotm_chip->chip_num > MAX_CHIP_NUM) {
		dev_err(&infotm_chip->device, "couldn`t support for so many chips\n");
		err = -ENXIO;
		goto exit_error;
	}
	for (i=0; i<infotm_chip->chip_num; i++) {
		infotm_chip->valid_chip[i] = 1;
		infotm_chip->chip_enable[i] = i;
		infotm_chip->rb_enable[i] = i;
	}
	if (!infotm_chip->rb_enable[0]) {
		infotm_chip->ops_mode |= INFOTM_CHIP_NONE_RB;
		//chip->dev_ready = NULL;
		chip->chip_delay = 200;
	}

	infotm_chip->infotm_nand_hw_init(infotm_chip);

	if (nand_scan(mtd, infotm_chip->chip_num) == -ENODEV) {
		chip->options = 0;
		chip->options |=  NAND_SKIP_BBTSCAN;
		chip->options |= NAND_NO_SUBPAGE_WRITE;
		chip->options |= NAND_BROKEN_XD;
		chip->options |= NAND_NO_READRDY;
		//chip->options |= NAND_SUBPAGE_READ_HW;
		if (infotm_nand_scan(mtd, infotm_chip->chip_num)) {
			err = -ENXIO;
			goto exit_error;
		}
	}
	else {
		NAND_DEBUG(KERN_WARNING,"NAND device found in old nand_base.c, need debug! PANIC!!!\n");
		BUG();
		for (i=1; i<infotm_chip->chip_num; i++) {
			infotm_chip->valid_chip[i] = 0;
			infotm_chip->chip_enable[i] = i;
			infotm_chip->rb_enable[i] = i;
		}
		infotm_chip->options = NAND_DEFAULT_OPTIONS;
		infotm_chip->page_size = mtd->writesize;
		infotm_chip->block_size = mtd->erasesize;
		infotm_chip->oob_size = mtd->oobsize;
		infotm_chip->plane_num = 1;
		infotm_chip->internal_chipnr = 1;
		chip->ecc.read_page_raw = infotm_nand_read_page_raw;
		chip->ecc.write_page_raw = infotm_nand_write_page_raw;
	}
	chip->scan_bbt = infotm_nand_scan_bbt;

	mtd->_lock = infotm_nand_lock;
	mtd->_unlock = infotm_nand_unlock;
	mtd->_suspend = infotm_nand_suspend;
	mtd->_resume = infotm_nand_resume;

	if (chip->ecc.mode != NAND_ECC_SOFT) {
		if (infotm_chip->infotm_nand_options_confirm(infotm_chip)) {
			err = -ENXIO;
			goto exit_error;
		}
	}
	if (infotm_chip->infotm_nand_adjust_timing)
		infotm_chip->infotm_nand_adjust_timing(infotm_chip);

	if (plat->platform_nand_data.chip.ecclayout) {
		chip->ecc.layout = plat->platform_nand_data.chip.ecclayout;
	} else {
		chip->ecc.layout = &infotm_nand_oob;
	}

	/*
	 * The number of bytes available for a client to place data into
	 * the out of band area
	 */
	chip->ecc.layout->oobavail = 0;
	for (i = 0; chip->ecc.layout->oobfree[i].length && i < ARRAY_SIZE(chip->ecc.layout->oobfree); i++)
		chip->ecc.layout->oobavail += chip->ecc.layout->oobfree[i].length;
	mtd->oobavail = chip->ecc.layout->oobavail;
	mtd->ecclayout = chip->ecc.layout;

	infotm_chip->virtual_page_size = mtd->writesize;
	infotm_chip->virtual_block_size = mtd->erasesize;

	if (chip->buffers)
		kfree(chip->buffers);
	if (mtd->oobsize >= NAND_MAX_OOBSIZE)
		chip->buffers = kzalloc((mtd->writesize + 3*mtd->oobsize), GFP_KERNEL);
	else
		chip->buffers = kzalloc((mtd->writesize + 3*NAND_MAX_OOBSIZE), GFP_KERNEL);
	if (chip->buffers == NULL) {
		NAND_DEBUG(KERN_WARNING,"no memory for flash data buf\n");
		err = -ENOMEM;
		goto exit_error;
	}
	chip->oob_poi = chip->buffers->databuf + mtd->writesize;
	chip->options |= NAND_OWN_BUFFERS;
	chip->pagebuf = -1;

	infotm_chip->subpage_cache_buf = kzalloc(mtd->writesize, GFP_KERNEL);
	if (infotm_chip->subpage_cache_buf == NULL) {
		NAND_DEBUG(KERN_WARNING,"no memory for flash subpage cache buf\n");
		err = -ENOMEM;
		goto exit_error;
	}
	infotm_chip->subpage_cache_page = -1;

	if (infotm_chip->ops_mode & INFOTM_RETRY_MODE) {
		err = infotm_chip->infotm_nand_get_retry_table(infotm_chip);
		if (err) {
			NAND_DEBUG(KERN_WARNING,"get read retry table failed\n");
		}
	}

	if (infotm_chip->ops_mode & INFOTM_ESLC_MODE) {
		err = infotm_chip->infotm_nand_get_eslc_para(infotm_chip);
		if (err) {
			NAND_DEBUG(KERN_WARNING,"get eslc param failed\n");
		}
	}

#if 0
	if (!strncmp((char*)plat->name, NAND_NFTL_NAME, strlen((const char*)NAND_NFTL_NAME))) {
		err = infotm_nand_env_check(mtd);
		if (err)
			NAND_DEBUG("invalid nand env\n");

#ifdef CONFIG_INFOTM_NAND_ENV
		pr_info("nand env: nand_env_probe. \n");
		nand_env_mtd = mtd;

		ret = alloc_chrdev_region(&nand_env_devno, 0, 1, NAND_ENV_DEVICE_NAME);

		if (ret < 0) {
			pr_err("nand_env: failed to allocate chrdev. \n");
			return 0;
		}
		/* connect the file operations with cdev */
		cdev_init(&infotm_chip->nand_env_cdev, &nand_env_fops);
		infotm_chip->nand_env_cdev.owner = THIS_MODULE;

		/* connect the major/minor number to the cdev */
		ret = cdev_add(&infotm_chip->nand_env_cdev, nand_env_devno, 1);
		if (ret) {
			pr_err("nand env: failed to add device. \n");
			/* @todo do with error */
			return ret;
		}

		ret = class_register(&nand_env_class);
		if (ret < 0) {
			NAND_DEBUG(KERN_NOTICE "class_register(&nand_env_class) failed!\n");
		}
		devp = device_create(&nand_env_class, NULL, nand_env_devno, NULL, "nand_env");
		if (IS_ERR(devp)) {
			NAND_DEBUG(KERN_ERR "nand_env: failed to create device node\n");
			ret = PTR_ERR(devp);
		}
#endif
		/*setup class*/
		infotm_chip->cls.name = kzalloc(strlen((const char*)NAND_NFTL_NAME)+1, GFP_KERNEL);

		strcpy((char *)infotm_chip->cls.name, NAND_NFTL_NAME);
		infotm_chip->cls.class_attrs = nand_class_attrs;
		err = class_register(&infotm_chip->cls);
		if(err)
			NAND_DEBUG(" class register nand_class fail!\n");
	} else if (!strncmp((char*)plat->name, NAND_NORMAL_NAME, strlen((const char*)NAND_NORMAL_NAME))) {
		err = infotm_chip->infotm_nand_check_uboot_item(infotm_chip);
		if (err)
			NAND_DEBUG("have not found uboot1 item blks\n");
	}
#endif
	plat->infotm_chip->mtd.name = plat->name;
	add_mtd_device(&plat->infotm_chip->mtd);

	dev_dbg(&infotm_chip->device, "initialized ok\n");
	return 0;

exit_error:
	if (chip->buffers) {
		kfree(chip->buffers);
		chip->buffers = NULL;
	}

	if (infotm_chip->subpage_cache_buf) {
		kfree(infotm_chip->subpage_cache_buf);
		infotm_chip->subpage_cache_buf = NULL;
	}

	return err;
}

#define DRV_NAME	"infotm-nand"
#define DRV_VERSION	"1.0"
#define DRV_AUTHOR	"xiaojun_yoyo"
#define DRV_DESC	"infotm nand flash Spec driver "

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_ALIAS("platform:" DRV_NAME);
