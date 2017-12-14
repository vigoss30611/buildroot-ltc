#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <mach/nand.h>

static char OffsetFileName[] = "/dev/block/nandblk0";
#define ENV_AREA 0x3c00000

int nand_env_save(uint8_t *data, int len)
{
#if 0
	struct erase_info instr;
	struct mtd_info *mtd = get_mtd_device_nm("nandnormal");
	struct mtd_oob_ops nnd_oob_ops;
	unsigned char nnd_oob_buf[64];

	loff_t offs, start = BL_LOC_ENV, from;
	int ret, page_cnt, i, buf_offset;

	printk("===kernel save env %s, len 0x%x\n", mtd->name, len);
	if(!mtd) {
		printk(KERN_ERR "store: can not find mtd partition.\n");
		return -1;
	}

	for(offs = 0; offs < ENV_AREA; offs += mtd->erasesize) {

		if(mtd->_block_isbad(mtd, start + offs)) {
			printk(KERN_ERR "store: skip bad 0x%llx\n", start + offs);
			continue;
		}

		instr.mtd = mtd;
		instr.addr = start + offs;
		instr.len  = mtd->erasesize;
		instr.callback = NULL;

		ret = mtd->_erase(mtd, &instr);

		printk("nand erase instr.addr 0x%llx\n ", instr.addr);
		if(ret) {
			printk(KERN_ERR "store: erase failed 0x%llx\n", instr.addr);
			continue;
		} else {
			strcpy((char *)nnd_oob_buf, INFOTM_ENV_STRING);
			break;
		}
	}

	if(offs >= BL_LOC_ENV) {
		printk(KERN_ERR "store failed, nand no more space.\n");
		return -1;
	}

	/* erase OK, write data */
	nnd_oob_ops.mode = MTD_OOB_AUTO;
	nnd_oob_ops.oobbuf = nnd_oob_buf;
	nnd_oob_ops.len = mtd->writesize;
	nnd_oob_ops.ooblen = mtd->oobsize;
	nnd_oob_ops.ooboffs = 0;

	page_cnt = len / mtd->writesize;
	printk("write oob len 0x%x, uboot 0x%x, page 0x%x\n", nnd_oob_ops.ooblen, mtd->oobavail, mtd->writesize);
	for (i=0; i<page_cnt; i++) {
		buf_offset = i * mtd->writesize;
		nnd_oob_ops.datbuf = data + buf_offset;
		from =  instr.addr +  buf_offset;
		ret = mtd->_write_oob(mtd, from, &nnd_oob_ops);
		if(ret)
			printk(KERN_ERR "store: write env failed 0x%llx\n", from);
		printk("ret is 0x%x, from 0x%llx\n", ret, from);
	}
#endif
	struct mtd_info *mtd = get_mtd_device_nm(NAND_NFTL_NAME);
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	unsigned int orgfs;
	struct file *filep;
	int ret;

	orgfs = get_fs();
	set_fs(KERNEL_DS);

	filep = filp_open(OffsetFileName, O_WRONLY, 0);
	if (IS_ERR(filep)) {
		printk("=======sys_open %s error 0x%lx!!.\n", OffsetFileName, IS_ERR(filep));
		ret =  -1;
		infotm_chip->env_need_save = 1;
	} else {
		filep->f_pos = ENV_AREA;
		ret = filep->f_op->write(filep, data, len, &filep->f_pos);
		printk("env save ret %d\n", ret);
		filp_close(filep, NULL);
		ret = 0;
	}

	set_fs(orgfs);
	return 0;
}

EXPORT_SYMBOL(nand_env_save);

