/*
 * Driver for NAND support, Rick Bronson
 * borrowed heavily from:
 * (c) 1999 Machine Vision Holdings, Inc.
 * (c) 1999, 2000 David Woodhouse <dwmw2@infradead.org>
 *
 * Added 16-bit nand support
 * (C) 2004 Texas Instruments
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <bootlist.h>
#include <malloc.h>
#include <cdbg.h>
#include <nand.h>
#include <linux/err.h>
#include <linux/mtd/nand.h>
#include <asm/arch/nand.h>

#define INFOTM_RTL_WRITESIZE	0x2000
#define INFOTM_RTL_OOBSIZE		0x280

static inline int str2long(char *p, ulong *num)
{
	char *endptr;

	*num = simple_strtoul(p, &endptr, 16);
	return (*p != '\0' && *endptr == '\0') ? 1 : 0;
}

static inline int str2longlong(char *p, uint64_t *num)
{
	char *endptr;
    
	*num = simple_strtoull(p, &endptr, 16);
	if(*endptr!='\0')
	{
	    switch(*endptr)
	    {
	        case 'g':
	        case 'G':
	            *num<<=10;
	        case 'm':
	        case 'M':
	            *num<<=10;
	        case 'k':
	        case 'K':
	            *num<<=10;
	            endptr++;
	            break;
	    }
	}
	
	return (*p != '\0' && *endptr == '\0') ? 1 : 0;
}

static int arg_off_size(int argc, char *argv[], struct mtd_info *mtd, uint64_t *off, uint64_t *size)
{
	if (argc >= 1) {
		if (!(str2longlong(argv[0], (uint64_t *)off))) {
			printf("'%s' is not a number\n", argv[0]);
			return -1;
		}
	} else {
		*off = 0;
	}

	if (argc >= 2) {
		if (!(str2longlong(argv[1], (uint64_t *)size))) {
			printf("'%s' is not a number\n", argv[1]);
			return -1;
		}
	} else {
		*size = mtd->size - *off;
	}

	if (*size == mtd->size)
		puts("whole chip\n");
	else
		printf("offset 0x%llx, size 0x%llx\n", *off, (uint64_t)*size);


	return 0;
}

static void infotm_boot0_write_ops(struct infotm_nand_chip *infotm_chip, unsigned char *buf, loff_t offs, int length)
{
	struct mtd_info *mtd= &infotm_chip->mtd;
	struct nand_chip *chip = &infotm_chip->chip;
	int page_addr, page_cnt, i;

	page_addr = (int)(offs >> chip->page_shift);
	page_cnt = (int)(length >> (fls(infotm_chip->uboot0_unit) - 1));

	infotm_chip->infotm_nand_select_chip(infotm_chip, 0);
	for (i=0; i<page_cnt; i++) {

		chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page_addr + i);
		chip->ecc.write_page(mtd, chip, (const uint8_t *)(buf + infotm_chip->uboot0_unit * i));
		chip->waitfunc(mtd, chip);
	}
	infotm_chip->infotm_nand_select_chip(infotm_chip, -1);

	return;
}

static int infotm_rtldata_convert(struct mtd_info *mtd, unsigned char *src, unsigned char *dest, int size)
{
	struct infotm_nand_chip *infotm_chip = mtd_to_nand_chip(mtd);
	struct imapx800_nand_chip *imapx800_chip = infotm_chip_to_imapx800(infotm_chip);
	struct nand_chip *chip = &infotm_chip->chip;
	struct infotm_nand_para_save *infotm_nand_save;
	struct infotm_nand_chip *infotm_chip_temp, *infotm_chip_uboot = NULL;
	struct infotm_nand_platform *plat;
	struct mtd_oob_ops infotm_oob_ops;
	struct list_head *l, *n;
	int i, ops_cnt, status;
	loff_t offs;

	infotm_nand_save = nand_zalloc(IMAPX800_PARA_SAVE_SIZE, 0);
	if (infotm_nand_save == NULL) {
		printf("no memory for rtl par save\n");
		return -ENOMEM;
	}

	infotm_nand_save->head_magic = IMAPX800_UBOOT_PARA_MAGIC;
	infotm_nand_save->chip0_para_size = infotm_chip->uboot0_unit;
	infotm_nand_save->boot_copy_num = 1;
	infotm_nand_save->uboot0_blk[0] = 1;
	infotm_nand_save->pages_per_blk_shift = 0;

	infotm_chip->infotm_nand_select_chip(infotm_chip, 0);
	offs = 0;//(infotm_chip->internal_page_nums << chip->page_shift);
	do {
		status |= NAND_STATUS_FAIL;
		if (chip->block_bad(mtd, offs, 0))
			continue;

		infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE1, -1, (offs >> chip->page_shift), 0);
		infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE2, -1, -1, 0);
		status = chip->waitfunc(mtd, chip);
		if (!(status & NAND_STATUS_FAIL))
			break;
		offs += mtd->erasesize;
	} while (1);

	list_for_each_safe(l, n, &imapx800_chip->chip_list) {
		infotm_chip_temp = chip_list_to_imapx800(l);
		plat = infotm_chip_temp->platform;
		if (!strncmp((char*)plat->name, NAND_BOOT_NAME, strlen((const char*)NAND_BOOT_NAME))) {
        	infotm_chip_uboot = infotm_chip_temp;
        	break;
        }
	}
	if (infotm_chip_uboot == NULL) {
		kfree(infotm_nand_save);
		return -ENODEV;
	}

	infotm_chip_uboot->ops_mode &= ~INFOTM_RANDOMIZER_MODE;
	infotm_boot0_write_ops(infotm_chip_uboot, (unsigned char *)infotm_nand_save, offs, infotm_chip_uboot->uboot0_unit);
	infotm_boot0_write_ops(infotm_chip_uboot, src, offs + mtd->writesize, size);
	infotm_chip_uboot->ops_mode |= INFOTM_RANDOMIZER_MODE;

	ops_cnt = ((int)(size >> (fls(infotm_chip->uboot0_unit) - 1)) + 1);
	infotm_oob_ops.mode = MTD_OOB_RAW;
	infotm_oob_ops.len = mtd->writesize;
	infotm_oob_ops.ooblen = mtd->oobsize;
	infotm_oob_ops.ooboffs = 0;
	printf("rtl convert temp addr %llx %x %x size %x %d \n", offs, (unsigned)src, (unsigned)dest, size, ops_cnt);

	memset(dest, 0xff, size);
	for (i=0; i<ops_cnt; i++) {
		infotm_oob_ops.datbuf = (unsigned char *)(dest + i * (INFOTM_RTL_WRITESIZE + INFOTM_RTL_OOBSIZE));
		infotm_oob_ops.oobbuf = infotm_oob_ops.datbuf + INFOTM_RTL_WRITESIZE;
		mtd->read_oob(mtd, (offs + i * mtd->writesize), &infotm_oob_ops);
	}

	infotm_chip->infotm_nand_wait_devready(infotm_chip, 0);
	infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE1, -1, (offs >> chip->page_shift), 0);
	infotm_chip->infotm_nand_command(infotm_chip, NAND_CMD_ERASE2, -1, -1, 0);
	chip->waitfunc(mtd, chip);
	//infotm_boot0_write_ops(infotm_chip_uboot, (unsigned char *)infotm_nand_save, offs, infotm_chip_uboot->uboot0_unit);
	//infotm_boot0_write_ops(infotm_chip_uboot, src, offs + mtd->writesize, size);

	kfree(infotm_nand_save);
	return 0;
}

int do_nand(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i, ops_cnt, phys_write_shift, ret = 0, read;
	ulong addr, src, dest;
	uint64_t off;
	uint64_t size;
	char *cmd;
	struct mtd_info *mtd;
	struct mtd_oob_ops infotm_oob_ops;

	if(argc < 2)
	  return -1;

	cmd = argv[1];
	if (strcmp(cmd, "erase") == 0 || strcmp(cmd, "scrub") == 0) {
		nand_erase_options_t opts;
		/* "clean" at index 2 means request to write cleanmarker */
		//int clean = argc > 2 && !strcmp("clean", argv[2]);
		int o = 2;
		int scrub = !strcmp(cmd, "scrub");

		mtd = get_mtd_device_nm(NAND_NFTL_NAME);
		if (IS_ERR(mtd)) {
			printf("none nand device \n");
			return -1;
		}

		printf("\nNAND %s: ", scrub ? "scrub" : "erase");
		/* skip first two or three arguments, look for offset and size */
		if (arg_off_size(argc - o, argv + o, mtd, &off, &size) != 0)
			return 1;

		memset(&opts, 0, sizeof(opts));
		opts.offset = off;
		opts.length = size;
		opts.jffs2  = 0;
		opts.quiet  = 0;

		if (scrub) {
			puts("Warning: "
			     "scrub option will erase all factory set "
			     "bad blocks!\n"
			     "         "
			     "There is no reliable way to recover them.\n"
			     "         "
			     "Use this command only for testing purposes "
			     "if you\n"
			     "         "
			     "are sure of what you are doing!\n"
			     "\nReally scrub this NAND flash? <y/N>\n");

			if (getc() == 'y' && getc() == '\r') {
				opts.scrub = 1;
			} else {
				puts("scrub aborted\n");
				return -1;
			}
		}
		ret = nand_erase_opts(mtd, &opts);
		printf("%s\n", ret ? "ERROR" : "OK");

		return ret == 0 ? 0 : 1;
	} else if ((strcmp(cmd, "readraw") == 0) || (strcmp(cmd, "writeraw") == 0)){
		if (argc < 4)
			goto usage;

		read = strncmp(cmd, "readraw", 7) == 0;
		mtd = get_mtd_device_nm(NAND_NORMAL_NAME);
		if (IS_ERR(mtd)) {
			printf("none nand device \n");
			return -1;
		}

		addr = (ulong)simple_strtoul(argv[2], NULL, 16);
		//if (arg_off_size(argc - 3, argv + 3, mtd, &off, (size_t *)(&size)) != 0)
			//return 1;
		off = (ulong)simple_strtoul(argv[3], NULL, 16);
		size = (ulong)simple_strtoul(argv[4], NULL, 16);
		printf("offset 0x%llx, size 0x%llx\n", off, (long long unsigned int)size);

		phys_write_shift = ffs(mtd->writesize) - 1;
		ops_cnt = (size >> phys_write_shift);
		infotm_oob_ops.mode = MTD_OOB_RAW;
		infotm_oob_ops.len = mtd->writesize;
		infotm_oob_ops.ooblen = mtd->oobsize;
		infotm_oob_ops.ooboffs = 0;

		for (i=0; i<ops_cnt; i++) {
			infotm_oob_ops.datbuf = (unsigned char *)(addr + i * (mtd->writesize + mtd->oobsize));
			infotm_oob_ops.oobbuf = infotm_oob_ops.datbuf + mtd->writesize;

			if (read)
				ret = mtd->read_oob(mtd, (off + i * mtd->writesize), &infotm_oob_ops);
			else
				ret = mtd->write_oob(mtd, (off + i * mtd->writesize), &infotm_oob_ops);
		}

		printf(" %llx bytes %s: %s\n", size, read ? "read" : "written", ret ? "ERROR" : "OK");
		return 0;
	} else if (strcmp(cmd, "rtldata") == 0) {

		src = (ulong)simple_strtoul(argv[2], NULL, 16);
		dest = (ulong)simple_strtoul(argv[3], NULL, 16);
		size = (ulong)simple_strtoul(argv[4], NULL, 16);
		/*if (!(str2longlong(argv[4], (unsigned long long *)size))) {
			printf("'%s' is not a number\n", argv[4]);
			return -1;
		}*/
		mtd = get_mtd_device_nm(NAND_NORMAL_NAME);
		if (IS_ERR(mtd)) {
			printf("none nand device \n");
			return -1;
		}

		ret = infotm_rtldata_convert(mtd, (unsigned char *)src, (unsigned char *)dest, (int)size);
		if (ret)
			printf("rtl data convert failed %d \n", ret);
		return 0;
	}

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(nand, CONFIG_SYS_MAXARGS, 1, do_nand,
	"nand utilities:",
	"readraw memaddr offset length\n"
	"writeraw memaddr offset length\n"
	"nand erase [clean] [off size] - erase 'size' bytes from\n"
	"    offset 'off' (entire device if not specified)\n"
	"nand scrub - really clean NAND erasing bad blocks (UNSAFE)\n"
	"rtldata src dest length\n");

