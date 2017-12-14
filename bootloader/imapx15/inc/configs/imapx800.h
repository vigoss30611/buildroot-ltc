/*****************************************************************************
 *
** include/asm/arch/imapx.h
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: Configuration file for iMAPx200.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**
** Revision History:
** -----------------
** 1.1  XXX 12/25/2009 XXX	Warits
*****************************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

/* XXX For easy to developing XXX */
#define CONFIG_CODE_DEBUG
//#define CONFIG_IMAPX_FPGATEST	/* To get all 40MHz CLKs */

#ifdef CONFIG_CODE_DEBUG
#undef CONFIG_SKIP_LOWLEVEL_INIT	/* To skip lowlevel_init and boot in RAM */
#undef CONFIG_SKIP_RELOCATE_UBOOT	/* To skip relocate u-boot */
#ifdef CONFIG_NAND_SPL
#define imap_dbg(args...)
#else
#define imap_dbg(args...) do {	\
	printf(args);	\
}\
	while(0)
#endif
#else
#undef CONFIG_SKIP_LOWLEVEL_INIT
#undef CONFIG_SKIP_RELOCATE_UBOOT	/* To skip relocate u-boot */
#define imap_dbg(args...)
#endif


/*
 * If HAVE_PRELOADER lowlevel_init will not be compiled when compiling
 * ram part u-boot. a preloader may refer to a NAND_SPL or ONENAND_SPL
 * If boot from NOR this should be undefined.
 */
#define CONFIG_HAVE_PRELOADER
#define CONFIG_IMAPX		1	/* CPU is iMAPx800 */
#define CONFIG_IMAPX800		1	/* CPU is iMAPx800 */

#define CONFIG_PRODUCT_WINCE_FB0		(0x80000000)
#define CONFIG_PRODUCT_WINCE_IMAGE_NAME	"zNK.img"
#define CONFIG_PRODUCT_LINUX_IMAGE_NAME	"uImage"
#define CONFIG_PRODUCT_ANDROID_SYSIMG_NAME	"zSYS.img"
#define CONFIG_PRODUCT_LINUX_RVFLAG		(0xad000001)
#define CONFIG_IDENT_STRING				"Shanghai InfoTM Microelectronics Co., Ltd."
#define CONFIG_SYS_PROMPT				"[3. LocalDefence]: "	/* Monitor Command Prompt */
/* +-------------------------------+ */
/* | iMAPx800 Memory configuration | */
/* +----------------------------------------------------------+ */
/* | You must define and must define the memory type here !   | */
/* +----------------------------------------------------------+ */
#define CONFIG_SYS_SDRAM_BASE	0x80000000
#define CONFIG_NR_DRAM_BANKS 1
#define PHYS_SDRAM_1			CONFIG_SYS_SDRAM_BASE

#define PHYS_SDRAM_1_SIZE		0x10000000		/* 256 MB in Bank #1 */
#define CONFIG_SYS_SDRAM_END	(CONFIG_SYS_SDRAM_BASE + PHYS_SDRAM_1_SIZE)	/* 256 MB total */

#ifndef CONFIG_SYS_SDRAM_END
#ifdef  CONFIG_TEST_VERSION
#define CONFIG_SYS_SDRAM_END    0x88000000
#else
#define CONFIG_SYS_SDRAM_END    (CONFIG_SYS_SDRAM_BASE+(CONFIG_SYS_SDRAM<<20))             /* 256 MB total */
#endif
#endif
/* | iMAPx800 Memory Map | */
/* +----------------------------------------------------------+ */
/* | Here defines the layout of memory when u-boot is running | */
/* |                                                          | */
/* | The last 16MB mem is reserved for u-boot:                | */
/* | <----10M----> | <----2M---->|<----3M----> | <----1M----> | */
/* |     resv      |    malloc   | u-boot code |      env     | */
/* +----------------------------------------------------------+ */
#define CONFIG_RESV_START			(CONFIG_SYS_SDRAM_END - 0x1000000)
#define CONFIG_SYS_PHY_UBOOT_BASE	(CONFIG_RESV_START + 0xc00000 - 0x40) /* -4MB */
#define CONFIG_NAND_ENV_DST			(CONFIG_RESV_START + 0xf00000) /* -1MB */

#define CONFIG_RESV_PG_TB			(0x3fffc000)				/* 16K */
#define CONFIG_RESV_GMAC_BASE		(CONFIG_RESV_START + 0x0)      /* 0~1M */
#define CONFIG_RESV_LCD_BASE		(CONFIG_RESV_START + 0x200000) /* 2~3M */
#define CONFIG_RESV_LCD_BASE_1	(CONFIG_RESV_START + 0x400000) /* 4~5M */
#define CONFIG_RESV_LOGO			(CONFIG_RESV_START + 0x600000) /* 8~9M */
#define CONFIG_RESV_SCRIPT			(CONFIG_RESV_START + 0x800000) /* 10 M */
#define CONFIG_RESV_PTBUFFER		(CONFIG_RESV_START + 0x800000) /* 10 M */

#define CONFIG_SYS_PHY_NK_BASE		(CONFIG_SYS_SDRAM_BASE + 0x200000 - 0x40)
#define CONFIG_SYS_PHY_NK_SWAP		(CONFIG_SYS_SDRAM_BASE + 0x6200000)
#define CONFIG_SYS_PHY_AS_SWAP		(CONFIG_SYS_SDRAM_BASE + 0x6008000)
#define CONFIG_SYS_PHY_UBOOT_SWAP	    (CONFIG_SYS_SDRAM_BASE + 0x8000)
#define CONFIG_RESV_LOCTABLE		    (CONFIG_SYS_PHY_NK_SWAP)
#define CONFIG_SYS_PHY_LK_BASE		(CONFIG_SYS_SDRAM_BASE + 0x8000 - 0x40)
#define CONFIG_SYS_PHY_RD_BASE		(CONFIG_SYS_SDRAM_BASE + 0x1000000 - 0x40)

/*For logo use only*/
/*NOTICE address space from 0X86000000-0X40 to 0X86000000-0X40+ 0xA00000 will never be
  used for any other usage but for cinpressed kernel image tmp buffer in booting process*/
#define  CONFIG_RESERVED_SKIP_BUFFER 	(CONFIG_SYS_SDRAM_BASE + 0x7000000)
#define ___RBASE (CONFIG_SYS_SDRAM_BASE + 0x10800000)
#define ___RLENGTH                      0x4000000

#define CONFIG_SKIP_MAGIC		0x22330233
#define CONFIG_SYS_PHY_BOOT_STAT	(CONFIG_SYS_SDRAM_BASE + 0x4)


/* Env configuration: */
/* Boot options */
//#define CONFIG_BOOT_NAND

/* Allow to overwrite serial and ethaddr */
#define CONFIG_ENV_SIZE		0x4000	/* Total Size of Environment Sector */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_IS_IN_INFOTM

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(1024 * 1024 * 20)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* size in bytes for initial data */
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x7fc0)	/* default load address	*/

/* Console configuration */
#define CONFIG_SYS_CONSOLE_IS_IN_ENV 1
#define CONFIG_SYS_HUSH_PARSER			/* use "hush" command parser	*/
#ifdef CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#endif

/* U-boot features */
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_DISPLAY_CPUINFO
#undef CONFIG_DISPLAY_BOARDINFO
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_SYS_64BIT_VSPRINTF		/* needed for nand_util.c */
#define CONFIG_SYS_LONGHELP				/* undef to save memory	      */
#define CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size    */
#define CONFIG_SYS_PBSIZE		384		/* Print Buffer Size          */
#define CONFIG_SYS_MAXARGS		16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size  */
#define CONFIG_SYS_HZ			100000
#define CONFIG_DOS_PARTITION	1
#define CONFIG_SUPPORT_VFAT
#define CONFIG_SYS_NO_FLASH
#define CONFIG_OEM_FEATURES				/* This will compile OEM features like Sys recovery and LOGO */
#undef CONFIG_OEM_MULTI_OS				/* OEM OS switch functions */

/* Board specific */
#define MACH_TYPE				0x08f9				/* 2296 in dec */
#define CONFIG_BOOTSTAT_U0		0xffff
#define CONFIG_BOOTSTAT_U1		0x0001
#define CONFIG_BOOTSTAT_U2		0x0002
#define CONFIG_BOOTSTAT_USB		0x000b
#define CONFIG_OEM_UPDATE_PASSWORD "infotm"
#define CONFIG_SYS_PHY_NK_MAXLEN 0x6000000			/* Maximum 96M */
#define CONFIG_SYS_PHY_AS_MAXLEN 0xe000000			/* Maximum 224M */

/* Default envrionments */
#define CONFIG_BOOTDELAY		0


/* Clock configuration: */
#define CONFIG_SYS_CLK_FREQ	12000000			/* input clock of PLL: SMDK6400 has 12MHz input clock, what about IMAP? XXX */


/**********************************
 Support Clock Settings
 **********************************
 Setting	SYNC	ASYNC
 ----------------------------------
 600_200	 O
 800_200	 	  O
 800_266	 O
 1056_266	 	  O
 **********************************/
#if	defined(CONFIG_CLK_800_266)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000041)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x000166c2)
#	define CONFIG_SYS_CPUSYNC		(0x2)		/* sync */
#elif	defined(CONFIG_CLK_1008_252)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000029)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014481)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* sync */
#elif	defined(CONFIG_CLK_804_268)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000042)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x000166c2)
#	define CONFIG_SYS_CPUSYNC		(0x2)		/* sync */
#elif	defined(CONFIG_CLK_1056_266)
#	define CONFIG_SYS_IMAPX200_APLL		(0x8000002b)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014481)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* sync */
#elif	defined(CONFIG_CLK_1104_276)
#	define CONFIG_SYS_IMAPX200_APLL		(0x8000002d)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014481)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* sync */
#elif	defined(CONFIG_CLK_828_276)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000044)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x000166c2)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* sync */
#elif	defined(CONFIG_CLK_564_282)
#	define CONFIG_SYS_IMAPX200_APLL		(0x8000002e)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014482)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* sync */
#elif	defined(CONFIG_CLK_576_288)
#	define CONFIG_SYS_IMAPX200_APLL		(0x8000002f)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014482)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* sync */
#elif	defined(CONFIG_CLK_1080_266)
#	define CONFIG_SYS_IMAPX200_APLL		(0x8000002c)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014481)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* sync */
#elif	defined(CONFIG_CLK_800_200)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80001041)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014481)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* async */
#elif	defined(CONFIG_CLK_800_133)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80001041)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00016661)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* async */
#elif	defined(CONFIG_CLK_840_210)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000022) /* real val 0x80000022 */
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014481)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* async */
#elif	defined(CONFIG_CLK_600_200)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000031)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x000166c2)
#	define CONFIG_SYS_CPUSYNC		(0x2)		/* sync */
#elif	defined(CONFIG_CLK_450_150)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000024)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x000166c2)
#	define CONFIG_SYS_CPUSYNC		(0x2)		/* sync */
#elif	defined(CONFIG_CLK_936_234)
#	define CONFIG_SYS_IMAPX200_APLL		(0x80000026)
#	define CONFIG_SYS_IMAPX200_DCFG0	(0x00014481)
#	define CONFIG_SYS_CPUSYNC		(0x0)		/* async */
#elif	defined(CONFIG_CLK_MANUAL_MODE)
#	define CONFIG_SYS_APLL_MV1			(0x80000031)
#	define CONFIG_SYS_DCFG_MV1			(0x000166c2)
#	define CONFIG_SYS_APLL_MV2			(0x80001041)
#	define CONFIG_SYS_DCFG_MV2			(0x00014481)
#	define CONFIG_SYS_APLL_MV3			(0x80000041)
#	define CONFIG_SYS_DCFG_MV3			(0x000166c2)
#	define CONFIG_SYS_APLL_MV4			(0x8000002b)
#	define CONFIG_SYS_DCFG_MV4			(0x00014481)
#	define CONFIG_SYS_CPUSYNC			(0x0)		/* sync */
#endif
/* END OF CLK CFG */


/* Drivers configuration: */
/* UART */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }	/* valid baudrates */
#define CONFIG_BAUDRATE			115200

/*   Memory pool
 *
 *   |-(0x3ffd8000)coda and stack(48k)(0x3ffe4000)-|-command(16k)-|-reserved-|-Tx Buffer(16k)-|-Rx Buffer(16k)-|-record(16k)-|-MMU table(16k)-|
 *       16k now                                  0x3ffe4000
 */
#define CONFIG_SYS_U_BOOT_MMP_CODE_START        CONFIG_SYS_NAND_U_BOOT_DDRTEST_DST  /* MMP load-addr */
#define CONFIG_SYS_U_BOOT_MMP_SIZE              (0x00004000)    /* 16k */

		/* NOR ENV CFG */
#define CONFIG_AU_CLK_MAGIC_OFFS		(0x1007fc00)		/* 512K - 1K */
#define CONFIG_BOARD_MAGIC_OFFS			(0x1007fc10)		/* 512K - 1K + 16*/
#define CONFIG_AU_MEM_MAGIC_OFFS		(0x1007fc1c)		/* 512K - 1K + 28*/

#define CONFIG_AU_CLK_MAGIC			(0xbdbeed00)
#define CONFIG_AU_MEM_MAGIC			(0xbdbeed01)



/* NAND */
//#define CONFIG_NAND
//#define CONFIG_NAND_IMAPX200
/* NAND ENV CFG */
#define CONFIG_SYS_NAND_U_BOOT_DDRTEST_DST	(0x3ffd8000)
#define CONFIG_SYS_NAND_U_BOOT_DST		CONFIG_SYS_PHY_UBOOT_BASE	/* NUB load-addr      */
#define CONFIG_SYS_NAND_U_BOOT_START		CONFIG_SYS_NAND_U_BOOT_DST	/* NUB start-addr     */
#define CONFIG_SYS_NAND_SPL_OFFS                (0x0000000)		/* 0K */
#define CONFIG_SYS_NAND_U_OFFS			(0x0004000)		/* 16K */
#define CONFIG_SYS_NAND_U0_OFFS			(0x0008000)		/* 32K */
#define CONFIG_SYS_NAND_U1_OFFS			(0x0100000)		/* 1M */
#define CONFIG_ENV_OFFSET				(0x0400000)		/* 4M */
#define CONFIG_SYS_NAND_RD__OFFS		(0x2000000)		/* 5M */
#define CONFIG_SYS_NAND_RD2_OFFS		(0x0800000)		/* 7M */
#define CONFIG_SYS_NAND_LK2_OFFS		(0x1000000)		/* 9M */
#define CONFIG_SYS_NAND_U2_OFFS			(0x1400000)		/* 16M */
#define CONFIG_SYS_NAND_iCK_OFFS		(0x1400000)		/* 20M */
#define CONFIG_SYS_NAND_RD1_OFFS		(0x1800000)		/* 24M */
#define CONFIG_SYS_NAND_LK1_OFFS		(0x1a00000)		/* 26M */
#define CONFIG_SYS_NAND_NK1_OFFS		(0x2800000)		/* 40M */
#define CONFIG_SYS_NAND_NK2_OFFS		(0xa400000)		/* 164M */
#define CONFIG_SYS_NAND_U_BOOT_SIZE		(512 * 1024)	/* Load 512K into RAM, change it accordingly XXX MUST BE BLOCK ALIGNED XXX*/
#define CONFIG_SYS_NAND_RELOC_OFFS		(0x02800000)	/* 40M */
#define CONFIG_SYS_NAND_BACK1_OFFS		(0x02800000)	/* 168M */
//#define CONFIG_SYS_NAND_BACK2_OFFS		(0x0e800000)	/* 232M */ /* This is removed from program due to 64M limit, by warits, Dec 6, 2010 */
#define CONFIG_SYS_NAND_SYSTEM_OFFS		(0x12800000)	/* 296M */
#define CONFIG_SYS_NAND_UDATA_OFFS		(0x1e800000)	/* 488M */
#define CONFIG_SYS_NAND_CACHE_OFFS		(0x5c000000)	/* 1472M */
#define CONFIG_SYS_NAND_NDISK_OFFS		(0x60000000)	/* 1536M */
#define CONFIG_SYS_NAND_SYSTEM_LEN		(0x0c000000)	/* 192M */
#define CONFIG_SYS_NAND_UDATA_LEN		(0x3d800000)	/* 984M */
#define CONFIG_SYS_NAND_NDISK_LEN		(0x1ff00000)	/* 511M */
#define CONFIG_SYS_NAND_BACK_LEN		(0x0c000000)	/* 128M */
#define CONFIG_SYS_NAND_NK_LEN			(0x07c00000)	/* 124M */
#define CONFIG_SYS_NAND_DDRTEST_LEN		(0x00004000)	/* 16K */
#define CONFIG_SYS_NAND_BLOCK_LEN		(0x00080000)	/* 512K */
		/* NOR ENV CFG */
#define CONFIG_AU_CLK_MAGIC_OFFS		(0x1007fc00)		/* 512K - 1K */
#define CONFIG_BOARD_MAGIC_OFFS			(0x1007fc10)		/* 512K - 1K + 16*/
#define CONFIG_AU_MEM_MAGIC_OFFS		(0x1007fc1c)		/* 512K - 1K + 28*/

#define CONFIG_AU_CLK_MAGIC				(0xbdbeed00)
#define CONFIG_AU_MEM_MAGIC				(0xbdbeed01)


/* NAND CHIP CFG */
#if defined(CONFIG_SYS_NAND_2K_PAGE)
#	define CONFIG_SYS_NAND_PAGE_SIZE	2048			/* Page size */
#	define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)	/* Block size */
#	define CONFIG_SYS_NAND_PAGE_COUNT	64				/* Page per block */
#	define CONFIG_SYS_NAND_OOBSIZE		64				/* Size of OOB */
#	define CONFIG_SYS_NAND_DIRTYMARK	24				/* Dirty mark position */
#elif defined(CONFIG_SYS_NAND_4K_PAGE)
#	define CONFIG_SYS_NAND_PAGE_SIZE	4096
#	define CONFIG_SYS_NAND_BLOCK_SIZE	(512 * 1024)
#	define CONFIG_SYS_NAND_PAGE_COUNT	128
#	define CONFIG_SYS_NAND_OOBSIZE		128
#	define CONFIG_SYS_NAND_DIRTYMARK	44
#elif defined(CONFIG_SYS_NAND_8K_PAGE)
#	define CONFIG_SYS_NAND_PAGE_SIZE	8192
#if 1
#	define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 8192)
#	define CONFIG_SYS_NAND_PAGE_COUNT	128
/* For H27UAG8T2BTR */
#else
#	define CONFIG_SYS_NAND_BLOCK_SIZE	(256 * 8192)
#	define CONFIG_SYS_NAND_PAGE_COUNT	256
#endif
#	define CONFIG_SYS_NAND_OOBSIZE		436
#	define CONFIG_SYS_NAND_DIRTYMARK	280
#endif

#if defined(CONFIG_SYS_NAND_MLC)
#	define CONFIG_SYS_NAND_ECCSIZE		512				/* bytes ECC module can dealwith each time */
#	define CONFIG_SYS_NAND_ECCBYTES		9				/* bytes per eccsize */
#elif defined(CONFIG_SYS_NAND_SLC)
#	define CONFIG_SYS_NAND_ECCSIZE		2048
#	define CONFIG_SYS_NAND_ECCBYTES		8				/* Actually 4, but 8 bytes is used */
#else
//#	error you must defined NAND cell type!
#endif
		/* NAND DRV CFG */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x20c50010		/* NFDATA */
#define CONFIG_SYS_NAND_5_ADDR_CYCLE				/* Extra address cycle for > 128MiB */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0			/* Location of the bad-block label */
#define CONFIG_SYS_NAND_SDMA

#define CONFIG_SYS_NAND_ECCSTEPS	(CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
#define CONFIG_SYS_NAND_ECCTOTAL	(CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
#define CONFIG_SYS_NAND_ECCPOS		{0};
#undef  CONFIG_SYS_NAND_BUSW16						/* Turn on this is bus width is 16 */

#if 0
#define CONFIG_SYS_NAND_SKIP_BAD_DOT_I	1  /* ".i" read skips bad blocks	      */
#define CONFIG_SYS_NAND_WP		1
#define CONFIG_SYS_NAND_YAFFS_WRITE	1  /* support yaffs write		      */
#define CONFIG_SYS_NAND_BBT_2NDPAGE	1  /* bad-block markers in 1st and 2nd pages  */
#endif
/* END OF DRV CFG */


/* U-BOOT command configruration:*/
#include <config_cmd_default.h>
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_LOADB
//#define CONFIG_CMD_SAVEENV
//#define CONFIG_CMD_NAND
//#define CONFIG_CMD_PING
#undef CONFIG_CMD_NET
//#define CONFIG_NET_MULTI
//#define CONFIG_CMD_ELF
#define CONFIG_CMD_FAT
//#define CONFIG_CMD_OEM
//#define CONFIG_CMD_MMC
/* XXX Do not use the following command XXX */
#undef	CONFIG_CMD_EXT2
#undef	CONFIG_CMD_IMLS

/* values for command mtest(common/cmd_mem.c) */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_RESV_START - 0x1000000)
/* END OF CMD CFG */

/* Things Disabled in u1 */
#if defined(CONFIG_UBOOT_BASIC)
#	undef CONFIG_ENV_IS_IN_NAND
#	undef CONFIG_OEM_FEATURES
#	undef CONFIG_DRIVER_GMAC
#	undef CONFIG_LCD
#	undef CONFIG_LCD_IMAPX200
#	undef CONFIG_CMD_CACHE
#	undef CONFIG_CMD_REGINFO
#	undef CONFIG_CMD_LOADS
#	undef CONFIG_CMD_LOADB
#	undef CONFIG_CMD_PING
#	undef CONFIG_CMD_NET
#	undef CONFIG_CMD_OEM
#	undef CONFIG_CMD_FAT
#	undef CONFIG_NET_MULTI
#	undef CONFIG_SYS_MALLOC_LEN
#	undef CONFIG_SYS_PHY_UBOOT_BASE
#	undef CONFIG_ENV_SIZE
#	undef CONFIG_NAND_ENV_DST
#	undef CONFIG_SUPPORT_VFAT
#	undef CONFIG_SYS_LONGHELP				/* undef to save memory	      */
#	define CONFIG_ENV_IS_NOWHERE
#	define CONFIG_ENV_SIZE		0x400	/* Total Size of Environment Sector */
#	define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 16 * 1024)
#	define CONFIG_SYS_PHY_UBOOT_BASE  (CONFIG_SYS_SDRAM_BASE - 140 * 1024) /* -4MB */
#endif

#define CONFIG_IUW_MAGIC        0x00497557
#define CONFIG_IUW_MAGIC2       0x55497557
#define CONFIG_IUW_MAGIC3       0x66497557
#define CONFIG_IUW_MAGIC4       0x77497557
#define CONFIG_IUW_CLEARDISK	0xE7E7E7E7
#define CONFIG_HWVER_MASK		0xffffff00
#define UPDATE_PACKAGE_NAME		"system.ius"



#ifdef  CONFIG_SYS_DISK_iNAND
#define iNAND_BLOCK_SIZE      		512 //Bytes
#define MAX_iNAND_PARTITION   		7
#define BURN_STEP_LENGTH        0x2000

#define PARTITION_NUM                   6
#define CONFIG_BACK_PARTITITION		PARTITION_NUM

#define SYSTEM_PART     256//M
#define DATA_PART       512//M    capacity for data partition when chip capacity equak to 2G
#define CACHE_PART      64//M
#define BACK_PART       256//M
#define EXTEND_LOCATION  (CACHE_PART+BACK_PART)

#define iNAND_LEN_UIMG                 	0x3000       //6M
#define iNAND_LEN_RD                    0x1000       //2M
#define iNAND_LEN_RE                    0x1000       //2M
#define iNAND_LEN_UPDATER               0x6000       //12M
#define IMAGE_LOCATION 			64//M


#ifdef CONFIG_HIBERNATE

#define CONFIG_HIBERNATE_LOCATION    	(CONFIG_SYS_SDRAM-8)
#define SPARE_LOCATION  	     	(IMAGE_LOCATION+CONFIG_SYS_SDRAM)//M
#define CONFIG_HIBERNATE_START 		(oem_get_hibernatebase())
#else
#define SPARE_LOCATION  		(IMAGE_LOCATION)//M
#endif

#define iNAND_START_ADDR_UIMG           (oem_get_imagebase()+100)
#define iNAND_START_ADDR_RD             (iNAND_START_ADDR_UIMG+iNAND_LEN_UIMG)
#define iNAND_START_ADDR_RE             (iNAND_START_ADDR_RD+iNAND_LEN_RD)
#define iNAND_START_ADDR_UPDATER        (iNAND_START_ADDR_RE+iNAND_LEN_RE)

#define iNAND_BACK_IMAGE_OFFSET         0xC000

#define CONFIG_HIBERMARK_OFFSET         0x60
#define CONFIG_IMAGE_BASE	        0x50
#define CONFIG_SYSPT_BASE         	0x40
#define CONFIG_HIBER_ADDRBUF            0x30


#undef  CONFIG_ANDROID_RECOVERY
#endif

#ifndef SOURCE_CHANNEL
#define SOURCE_CHANNEL 0
#endif

#define CONFIG_PARTITIONS 1
#define CONFIG_FASTBOOT 1
#endif
