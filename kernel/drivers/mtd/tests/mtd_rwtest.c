/*
 * Copyright (C) 2006-2008 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING. If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Test random 1 eraseblock reads, writes and erases on MTD device.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/random.h>

static int dev = -EINVAL;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");

static int count = 1;
module_param(count, int, S_IRUGO);
MODULE_PARM_DESC(count, "Number of test loop (default is 1)");

static struct mtd_info *mtd;
static unsigned char *writebuf;
static unsigned char *readbuf;
static unsigned char *bbt;
static int *offsets;

static int pgsize;
static int bufsize;
static int ebcnt;
static int pgcnt;

static int rand_eb(void)
{
	unsigned int eb;

	eb = prandom_u32();
	eb %= ebcnt;

	return eb;
}

static int rand_offs(void)
{
	unsigned int offs=0;
	return offs;
}

static int rand_len(int offs)
{
	unsigned int len=0;
	return len;
}

static int blk_erase(int ebnum)
{
	int err;
	struct erase_info ei;
	loff_t addr = ebnum * mtd->erasesize;

	memset(&ei, 0, sizeof(struct erase_info));
	ei.mtd  = mtd;
	ei.addr = addr;
	ei.len  = mtd->erasesize;

	err = mtd_erase(mtd, &ei);
	if (unlikely(err)) {
		pr_err("error %d while erasing EB %d\n", err, ebnum);
		return err;
	}

	if (unlikely(ei.state == MTD_ERASE_FAILED)) {
		pr_err("some erase error occurred at EB %d\n", ebnum);
		return -EIO;
	}

	return 0;
}

static int blk_read(int ebnum)
{
	size_t read;
	int err;
	int len = mtd->erasesize;
	loff_t addr = ebnum * mtd->erasesize;

	err = mtd_read(mtd, addr, len, &read, readbuf);
	if (mtd_is_bitflip(err))
		err = 0;
	if (unlikely(err)) {
		pr_err("error: read failed at 0x%llx\n", (long long)addr);
		return err;
	}
	
	if (unlikely(read != len)) {
		pr_err("error: read num %d, but expect %d\n", read, len);
		if (!err)
			err = -EINVAL;
		return err;
	}

	return 0;
}

static int blk_write(int ebnum)
{
	size_t written;
	int err;
	int len = mtd->erasesize;
	loff_t addr = ebnum * mtd->erasesize;

	//prandom_bytes(writebuf, len);

	err = mtd_write(mtd, addr, len, &written, writebuf);
	if (unlikely(err)) {
		pr_err("error: written failed at 0x%llx\n", (long long)addr);
		return err;
	}
	
	if (unlikely(written != len)) {
		pr_err("error: written num %d, but expect %d\n", written, len);
		if (!err)
			err = -EINVAL;
		return err;
	}

	return 0;
}

static int blkncmp(unsigned char *src, unsigned char *dst, int len)
{
	int i;
	unsigned char *p = src;
	unsigned char *q = dst;

	for (i = 0; i < len; i++) {
		if (*q++ != *p++) {
			pr_err("byte %d error: src=0x%02x, dst=0x%02x\n",
					i, *p, *q);
			return -1;
		}
	}

	return 0;
}

static int is_blk_bad(int ebnum)
{
	size_t read;
	int i, err;
	unsigned char *p;
	int len = mtd->erasesize;
	loff_t addr = ebnum * mtd->erasesize;

	err = mtd_read(mtd, addr, len, &read, readbuf);
	if (mtd_is_bitflip(err))
		err = 0;
	if (unlikely(err)) {
		pr_err("error: read failed at 0x%llx\n", (long long)addr);
		return err;
	}
	
	if (unlikely(read != len)) {
		pr_err("error: read num %d, but expect %d\n", read, len);
		if (!err)
			err = -EINVAL;
		return err;
	}

	p = readbuf;
	for (i = 0; i < len; i++) {
		if (*p++ != 0xff) {
			pr_err("erasebank %d, byte %d : 0x%02x, but expect 0xff\n", ebnum, i, *p);
			return -1;;
		}
	}

	cond_resched();

	return 0;
}

static int __init mtd_rwtest_init(void)
{
	int err, eb, i;
	uint64_t tmp;

	printk(KERN_INFO "\n");
	printk(KERN_INFO "=================================================\n");

	if (dev < 0) {
		pr_info("Please specify a valid mtd-device via module parameter\n");
		pr_crit("CAREFUL: This test wipes all data on the specified MTD device!\n");
		return -EINVAL;
	}

	pr_info("MTD device: %d\n", dev);

	mtd = get_mtd_device(NULL, dev);
	if (IS_ERR(mtd)) {
		err = PTR_ERR(mtd);
		pr_err("error: cannot get MTD device\n");
		return err;
	}

	if (mtd->writesize == 1) {
		pr_info("not NAND flash, assume page size is 512 bytes.\n");
		pgsize = 512;
	} else {
		pgsize = mtd->writesize;
	}

	tmp = mtd->size;
	do_div(tmp, mtd->erasesize);
	ebcnt = tmp;
	pgcnt = mtd->erasesize / pgsize;

	pr_info("MTD device size %llu, eraseblock size %u, "
		"page size %u, count of eraseblocks %u, pages per "
		"eraseblock %u, OOB size %u\n",
		(unsigned long long)mtd->size, mtd->erasesize,
		pgsize, ebcnt, pgcnt, mtd->oobsize);

	err = -ENOMEM;
	readbuf = vmalloc(mtd->erasesize);
	writebuf = vmalloc(mtd->erasesize);
	offsets = kmalloc(ebcnt * sizeof(int), GFP_KERNEL);
	if (!readbuf || !writebuf || !offsets) {
		pr_err("error: cannot allocate memory\n");
		goto out;
	}

	for (i = 0; i < count; i++) {
		if ((i % 100) == 0)
			pr_err("%d rwtest loop done\n", i);

		cond_resched();

		eb = rand_eb();
		prandom_bytes(writebuf, mtd->erasesize);

		err = blk_erase(eb);
		if (err)
			goto out;

		err = is_blk_bad(eb);
//		if (err)
//			goto out;

		err = blk_write(eb);
		if (err)
			goto out;

		err = blk_read(eb);
		if (err)
			goto out;

		err = blkncmp(writebuf, readbuf, mtd->erasesize);
		if (err)
			goto out;
	}

	pr_info("finished test, loop %d done\n", i);

out:
	kfree(offsets);
	vfree(writebuf);
	vfree(readbuf);

	put_mtd_device(mtd);
	if (err)
		pr_info("error %d occurred\n", err);
	printk(KERN_INFO "=================================================\n");

	return err;
}
module_init(mtd_rwtest_init);

static void __exit mtd_rwtest_exit(void)
{
	return;
}
module_exit(mtd_rwtest_exit);

MODULE_DESCRIPTION("Read&Write test module");
MODULE_AUTHOR("David Sun");
MODULE_LICENSE("GPL");
