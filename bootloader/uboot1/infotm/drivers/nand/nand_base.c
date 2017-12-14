/*
 * drivers/mtd/nand/nand_util.c
 *
 * Copyright (C) 2006 by Weiss-Electronic GmbH.
 * All rights reserved.
 *
 * @author:	Guido Classen <clagix@gmail.com>
 * @descr:	NAND Flash support
 * @references: borrowed heavily from Linux mtd-utils code:
 *		flash_eraseall.c by Arcom Control System Ltd
 *		nandwrite.c by Steven J. Hill (sjhill@realitydiluted.com)
 *			       and Thomas Gleixner (tglx@linutronix.de)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <common.h>
#include <command.h>
#include <watchdog.h>
#include <malloc.h>
#include <div64.h>

#include <asm/errno.h>
#include <linux/mtd/mtd.h>
#include <nand.h>
#include <jffs2/jffs2.h>
#include <vstorage.h>
#include <cdbg.h>
#include <hash.h>
#include <efuse.h>
#include <lowlevel_api.h>
#include <asm/io.h>
#include <cpuid.h>
#include <rballoc.h>

//#define DEBUG_WITHOUT_CONFIG

/* two way to distinguish valid bnd data
 * the 1st is crc16, which is very slow but more secure
 * magic runs faster with less secure
 */
#define BND_USE_MAGIC	0x0318

#define MAX_BAD_BLK_NUM 				1000
#define BBT_START 					0x7000000
#define BBT_SIZE 					(0x1000 - (sizeof(uint32_t)))
#define BBT_HEAD_MAGIC                                  "bbts"
#define BBT_TAIL_MAGIC                                  "bbte"

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

typedef struct environment_s {
	uint32_t        crc;            /* CRC32 over data bytes        */
	unsigned char   data[BBT_SIZE]; /* Environment data             */
} env_t;


struct infotm_nand_bbt_info *infotm_nand_bbi;

int g_retrylevel;
int g_normalparam;
unsigned char *g_otp_buf, *rrtb_buf = NULL;
unsigned char *data_buf = NULL;
int bbt_flag;
int g_eslc_param;
unsigned int * gp_retrylevel;
unsigned char retry_reg_buf[8];

struct nand_bbt_t *nand_bbt;
struct nand_cfg nand_cfg;

void print_nand_cfg(struct nand_config *cfg) {
	printf("NAND config got:\n");
	printf("IF: %d\n", cfg->interface);
	printf("Randomizer: %d\n", cfg->randomizer);
	printf("Cycle: %d\n", cfg->cycle);
	printf("MECC level: %d\n", cfg->mecclvl);
	printf("SECC level: %d\n", cfg->secclvl);
	printf("ECC enable: %d\n", cfg->eccen);
	printf("Bad0: %x\n", cfg->badpagemajor);
	printf("Bad1: %x\n", cfg->badpageminor);
	printf("Sysinfo: %x\n", cfg->sysinfosize);
	printf("Pagesize: %x\n", cfg->pagesize);
	printf("Sparesize: %x\n", cfg->sparesize);
	printf("Blocksize: %x\n", cfg->blocksize);
	printf("Timing: %x\n", cfg->timing);
	printf("busw: %x\n", cfg->busw);
}

#if 0
void print_cdbg_cfg(struct cdbg_cfg_nand *t)
{
	printf("\n\n");
	printf("interface: %d\n", t->interface);
	printf("randomizer: %d\n", t->randomizer);
	printf("cycle: %d\n", t->cycle);
	printf("mecclvl: %d\n", t->mecclvl);
	printf("secclvl: %d\n", t->secclvl);
	printf("eccen: %d\n", t->eccen);
	printf("badpagemajor: %d\n", t->badpagemajor);
	printf("badpageminor: %d\n", t->badpageminor);
	printf("sysinfosize: %d\n", t->sysinfosize);
	printf("pagesize: 0x%x\n", t->pagesize);
	printf("sparesize: %d\n", t->sparesize);
	printf("blocksize: 0x%x\n", t->blocksize);
	printf("timing: 0x%x\n", t->timing);
	printf("rnbtimeout: 0x%x\n", t->rnbtimeout);
	printf("phyread: 0x%x\n", t->phyread);
	printf("phydelay: 0x%x\n", t->phydelay);
	printf("seed0: 0x%x\n", t->seed[0]);
	printf("seed1: 0x%x\n", t->seed[1]);
	printf("seed2: 0x%x\n", t->seed[2]);
	printf("seed3: 0x%x\n", t->seed[3]);
	printf("poly: 0x%x\n", t->polynomial);
	printf("id: %02x %02x %02x %02x %02x %02x %02x %02x\n",
				t->id[0], t->id[1], t->id[2],
				t->id[3], t->id[4], t->id[5],
				t->id[6], t->id[7]);
}
#endif

int nand_init_randomizer(struct nand_config *cfg)
{
	int ecc_num, i;
	int pagesize, sysinfosize;

	if(cfg->randomizer) {
		ecc_num = nf2_ecc_num(cfg->mecclvl, cfg->pagesize, 0);
		printf("i: randomizer (%d)\n", ecc_num);
//		printf("ecc_num = %d\n", ecc_num);
	
		pagesize = cfg->pagesize;
		sysinfosize = cfg->sysinfosize;
		if(cfg->busw == 16){
			pagesize = (cfg->pagesize) >> 1;
			sysinfosize = (cfg->sysinfosize) >> 1;
			ecc_num = ecc_num >> 1;	
		}

		/* here we need nand controller to calculate randmizer:
		 * incase module not enabled */
		module_enable(NF2_SYSM_ADDR);

		nf2_set_polynomial(cfg->polynomial);
		for(i = 0; i < 8; i++)
		{
#if 0			
			cfg->sysinfo_seed[i] = nf2_randc_seed(*(((uint16_t *)cfg->seed) + i) & 0x7fff,
						(pagesize - 1), cfg->polynomial);
			cfg->ecc_seed[i] = nf2_randc_seed(cfg->sysinfo_seed[i],
						(sysinfosize + 4 - 1), cfg->polynomial);
			cfg->secc_seed[i] = nf2_randc_seed(cfg->ecc_seed[i], (ecc_num - 1), cfg->polynomial);
#else	
			cfg->sysinfo_seed[i] = nf2_randc_seed_hw(*(((uint16_t *)cfg->seed) + i) & 0x7fff,
						(pagesize - 1), cfg->polynomial);
			if(cfg->busw == 16){
				cfg->ecc_seed[i] = nf2_randc_seed_hw(cfg->sysinfo_seed[i],
							(sysinfosize + 2 - 1), cfg->polynomial);	
			}else{
				cfg->ecc_seed[i] = nf2_randc_seed_hw(cfg->sysinfo_seed[i],
							(sysinfosize + 4 - 1), cfg->polynomial);
			}
			cfg->secc_seed[i] = nf2_randc_seed_hw(cfg->ecc_seed[i], (ecc_num - 1), cfg->polynomial);
#endif
		}

	}

	return 0;
}

int nand_rescan_bootcfg(void)
{
	uint32_t pagesize[8] = {512, 0x800, 0x1000, 0x2000, 0x4000,
							0x800, 0x1000, 0x2000};
	uint16_t info = boot_info();
	struct efuse_paras *ecfg = ecfg_paras();

//	printf("info got: 0x%x\n", info);
	if(boot_device() != DEV_BND) {
		printf("get bootcfg failed, device is not bnd.\n");
		return -1;
	}

	nand_cfg.boot_en		 = 1;
	nand_cfg.boot.mecclvl    = (info >> 8) & 0xf;
	nand_cfg.boot.eccen      = (info >> 7) & 0x1;
	nand_cfg.boot.pagesize   = (IROM_IDENTITY == IX_CPUID_X15)? 0x400: pagesize[(info >> 4) & 0x7];
	nand_cfg.boot.interface  = (info >> 2) & 0x3;
	nand_cfg.boot.cycle      = ((info >> 1) & 0x1)?5:4;
	nand_cfg.boot.randomizer = (info >> 0) & 0x1;
	if(IROM_IDENTITY == IX_CPUID_X15)
		nand_cfg.boot.busw       = ((info >> 4) & 0x7)? 16: 8;
	else
		nand_cfg.boot.busw       = (((info >> 4) & 0x7) > 4)? 16: 8;
	nand_cfg.bnd_page_shift  = ffs(nand_cfg.boot.pagesize) - 1;
	nand_cfg.bnd_size_shift  = ffs(nand_cfg.normal.pagesize / nand_cfg.boot.pagesize) - 1;


	if(ecfg_check_flag(ECFG_DEFAULT_TGTIMING)) {
		nand_cfg.boot.timing	 = 0x51ffffff;  //sync timing for toggle, async timing for async.
		nand_cfg.boot.rnbtimeout = 0xfffaf0;
		nand_cfg.boot.phyread	 = 0x4023;     //sync timing for toggle.
		nand_cfg.boot.phydelay   = 192<<8 | 192;
	} else {
		/* use e-fuse timing */
		nand_cfg.boot.timing     = ecfg->tg_timing;
		nand_cfg.boot.timing   <<= 24;
		nand_cfg.boot.timing    |= 0xffffff;
		nand_cfg.boot.phyread    = ecfg->phy_read;
		nand_cfg.boot.phydelay   = ecfg->phy_delay;
	}

	if(ecfg_check_flag(ECFG_DEFAULT_RANDOMIZER)) {
		nand_cfg.boot.seed[0]	 = 0x1c723550;
		nand_cfg.boot.seed[1]	 = 0x20fe074e;
		nand_cfg.boot.seed[2]	 = 0x1698347e;
		nand_cfg.boot.seed[3]	 = 0x3fff2c80;
		nand_cfg.boot.polynomial = 0x994;
	} else {
		nand_cfg.boot.seed[0]    = ecfg->seed[0];
		nand_cfg.boot.seed[1]    = ecfg->seed[1];
		nand_cfg.boot.seed[2]    = ecfg->seed[2];
		nand_cfg.boot.seed[3]    = ecfg->seed[3];
		nand_cfg.boot.polynomial = ecfg->polynomial;
	}

	return 0;
}

int nand_rescan_config(struct cdbg_cfg_nand t[], int count)
{
	int i, j, ret, k;
	uint8_t id[CDBG_NAND_ID_COUNT];
	int bad_mark = 0;
	int otp_start = 0;
	int otp_len = 0;
	uint8_t * retry_param;
	uint32_t retry_param_magic = 0;
	int page;
	int skip;
	int block;
	loff_t start;

	g_otp_buf = NULL;
	g_otp_buf = malloc(2048);

	if(g_otp_buf == NULL){
		printf("====OTP BUF ALLOC FAILED\n");
	}
	/* rescan boot config */
	ret = nand_rescan_bootcfg();
	
	if(ret == 0)
	  nand_init_randomizer(&nand_cfg.boot);

	if(!t) /* no normal config list, return */
	  return 0;

	/* rescan normal config */
	for(i = 0; i < count; i++) {
//		print_cdbg_cfg(&t[i]);
		ret = nand_readid(id, t[i].interface, t[i].timing, t[i].rnbtimeout, t[i].phyread, t[i].phydelay, t[i].busw);
		printf("got id: %02x %02x %02x %02x %02x %02x %02x %02x\n",
					id[0], id[1], id[2],
					id[3], id[4], id[5],
					id[6], id[7]);

		for(j = 0; j < CDBG_NAND_ID_COUNT; j++) {
			if(!t[i].id[j]) {
//				printf("config is 0, do not compare.\n");
				continue;
			}
			else if(id[j] != t[i].id[j]) {
				printf("id%d do not match, %x<=>%x\n", j, id[j], t[i].id[j]);
				break;
			}
		}

		if(j == CDBG_NAND_ID_COUNT) {

			printf("nand configuratino matched.\n");
			nand_cfg.normal.interface = t[i].interface;
			nand_cfg.normal.randomizer = t[i].randomizer;
			nand_cfg.normal.cycle = t[i].cycle;
			nand_cfg.normal.mecclvl = t[i].mecclvl;
			nand_cfg.normal.secclvl = t[i].secclvl;
			nand_cfg.normal.eccen = t[i].eccen;
			nand_cfg.normal.badpagemajor = t[i].badpagemajor;
			nand_cfg.normal.badpageminor = t[i].badpageminor;
			nand_cfg.normal.sysinfosize = t[i].sysinfosize;
			nand_cfg.normal.pagesize = t[i].pagesize;
			nand_cfg.normal.sparesize = t[i].sparesize;
			nand_cfg.normal.blocksize = t[i].blocksize;
			nand_cfg.normal.blockcount = t[i].blockcount;
			nand_cfg.normal.timing = t[i].timing;
			nand_cfg.normal.rnbtimeout = t[i].rnbtimeout;
			nand_cfg.normal.phyread = t[i].phyread;
			nand_cfg.normal.phydelay = t[i].phydelay;
			nand_cfg.normal.seed[0] = t[i].seed[0];
			nand_cfg.normal.seed[1] = t[i].seed[1];
			nand_cfg.normal.seed[2] = t[i].seed[2];
			nand_cfg.normal.seed[3] = t[i].seed[3];
			nand_cfg.normal.polynomial = t[i].polynomial;
			nand_cfg.normal.busw = t[i].busw ? t[i].busw: 8;
			nand_cfg.normal.pagesperblock = t[i].blocksize / t[i].pagesize;
			nand_cfg.normal.read_retry = t[i].read_retry;
			nand_cfg.normal.retry_level = t[i].retry_level;
			nand_cfg.normal.nand_param0 = t[i].nand_param0;
			nand_cfg.normal.nand_param1 = t[i].nand_param1;
			nand_cfg.normal.nand_param2 = t[i].nand_param2;

			nand_cfg.normal_en = 1;
			print_nand_cfg(&nand_cfg.normal);
			nand_cfg.nnd_page_shift = ffs(nand_cfg.normal.pagesize) - 1;
			if(boot_device() == DEV_BND)
				nand_cfg.bnd_size_shift = ffs(nand_cfg.normal.pagesize / nand_cfg.boot.pagesize) - 1;

			/* init boot block size accordingly, this will be used
			 * in the burn process
			 */
			nand_cfg.boot.blocksize = nand_cfg.normal.blocksize >> nand_cfg.bnd_size_shift;

			nand_init_randomizer(&nand_cfg.normal);
			break;
		}
	}

	if(i == count) {
		printf("no configuration matched.\n");
//		printf("NAND ID: %02x %02x %02x %02x %02x %02x %02x %02x\n",
//					id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
		return -1;
	}
	gp_retrylevel = (uint32_t *)(rrtb_buf + 0x4);
	
	if(nand_cfg.normal.read_retry == 1){
		if(nand_cfg.normal.nand_param0 == 0x55){
			if(nand_cfg.normal.nand_param1 == 20){

				if(nand_cfg.normal.nand_param2 == 35){
					printf("H27UCG8T2ATR\n");
					retry_reg_buf[0] = 0xcc;
					retry_reg_buf[1] = 0xbf;
					retry_reg_buf[2] = 0xaa;
					retry_reg_buf[3] = 0xab;
					retry_reg_buf[4] = 0xcd;
					retry_reg_buf[5] = 0xad;
					retry_reg_buf[6] = 0xae;
					retry_reg_buf[7] = 0xaf;
				}
				if(nand_cfg.normal.nand_param2 == 36){
					printf("H27UBG8T2CTR\n");
					retry_reg_buf[0] = 0xb0;
					retry_reg_buf[1] = 0xb1;
					retry_reg_buf[2] = 0xb2;
					retry_reg_buf[3] = 0xb3;
					retry_reg_buf[4] = 0xb4;
					retry_reg_buf[5] = 0xb5;
					retry_reg_buf[6] = 0xb6;
					retry_reg_buf[7] = 0xb7;
				}

				g_retrylevel = *((uint32_t *)(rrtb_buf + 0x4));
				retry_param = (uint8_t *)(rrtb_buf +0x100);
				retry_param_magic = *((uint32_t *)(rrtb_buf + 0x8));
				if(retry_param_magic != 0x8a7a6a5a){
					g_retrylevel = 0;
		
					start = (nand_cfg.normal.blockcount + 4)*256*8192ull;
					printf("uboot1 nand_cfg.normal.blockcount is 0x%x.\n", nand_cfg.normal.blockcount);
					block = nand_cfg.normal.blocksize;
					for(skip = 0; isbad(start & ~(block - 1));
							skip++, start += block) {
							printf("bad block skipped @ 0x%llx\n", start & ~(block - 1));
						if(skip > 50) {
							printf("two many bad blocks skipped"
									"before getting the corrent data.\n");
							return -1;
						}
					}
					page = start >> 13;
					imap_get_retry_param_20_from_page(page, g_otp_buf, 1024);

					otp_start = 0;
					otp_len = 64;
otp_check_again:				
					for(k = otp_start,j = (otp_start+otp_len); k<(otp_start + otp_len) && otp_start<1026; k++, j++){
						if((((g_otp_buf[k] | g_otp_buf[j]) & 0xff) != 0xff) && ((g_otp_buf[k] & g_otp_buf[j]) != 0x0)){
							printf("uboot1: param diff %d, %d, 0x%x, 0x%x\n", k, j, g_otp_buf[k], g_otp_buf[j]);
							otp_start += (otp_len * 2);	
							if(otp_start >= 896){
								printf("uboot1: get nand param failed\n");
								return -1;
							}
					
							goto otp_check_again;	
						}
					}
					printf("otp_start = %d, 0x%x, 0x%x, 0x%x, 0x%x\n", otp_start, g_otp_buf[0], g_otp_buf[1], g_otp_buf[2], g_otp_buf[3]);
					g_otp_buf += otp_start;
					if(bad_mark == 0){
						printf("default param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[0], g_otp_buf[1],g_otp_buf[2], g_otp_buf[3],g_otp_buf[4], g_otp_buf[5],g_otp_buf[6], g_otp_buf[7]);
						printf("1st     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[8], g_otp_buf[9],g_otp_buf[10], g_otp_buf[11],g_otp_buf[12], g_otp_buf[13],g_otp_buf[14], g_otp_buf[15]);
						printf("2nd     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[16], g_otp_buf[17],g_otp_buf[18], g_otp_buf[19],g_otp_buf[20], g_otp_buf[21],g_otp_buf[22], g_otp_buf[23]);
						printf("3th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[24], g_otp_buf[25],g_otp_buf[26], g_otp_buf[27],g_otp_buf[28], g_otp_buf[29],g_otp_buf[30], g_otp_buf[31]);
						printf("4th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[32], g_otp_buf[33],g_otp_buf[34], g_otp_buf[35],g_otp_buf[36], g_otp_buf[37],g_otp_buf[38], g_otp_buf[39]);
						printf("5th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[40], g_otp_buf[41],g_otp_buf[42], g_otp_buf[43],g_otp_buf[44], g_otp_buf[45],g_otp_buf[46], g_otp_buf[47]);
						printf("6th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[48], g_otp_buf[49],g_otp_buf[50], g_otp_buf[51],g_otp_buf[52], g_otp_buf[53],g_otp_buf[54], g_otp_buf[55]);
						printf("7th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[56], g_otp_buf[57],g_otp_buf[58], g_otp_buf[59],g_otp_buf[60], g_otp_buf[61],g_otp_buf[62], g_otp_buf[63]);
					}
				}else{
					for(i=0; i<64; i++){
						g_otp_buf[i] = *(retry_param + i);
					}
					printf("get param from uboot0\n");	
					printf("default param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[0], g_otp_buf[1],g_otp_buf[2], g_otp_buf[3],g_otp_buf[4], g_otp_buf[5],g_otp_buf[6], g_otp_buf[7]);
					printf("1st     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[8], g_otp_buf[9],g_otp_buf[10], g_otp_buf[11],g_otp_buf[12], g_otp_buf[13],g_otp_buf[14], g_otp_buf[15]);
					printf("2nd     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[16], g_otp_buf[17],g_otp_buf[18], g_otp_buf[19],g_otp_buf[20], g_otp_buf[21],g_otp_buf[22], g_otp_buf[23]);
					printf("3th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[24], g_otp_buf[25],g_otp_buf[26], g_otp_buf[27],g_otp_buf[28], g_otp_buf[29],g_otp_buf[30], g_otp_buf[31]);
					printf("4th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[32], g_otp_buf[33],g_otp_buf[34], g_otp_buf[35],g_otp_buf[36], g_otp_buf[37],g_otp_buf[38], g_otp_buf[39]);
					printf("5th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[40], g_otp_buf[41],g_otp_buf[42], g_otp_buf[43],g_otp_buf[44], g_otp_buf[45],g_otp_buf[46], g_otp_buf[47]);
					printf("6th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[48], g_otp_buf[49],g_otp_buf[50], g_otp_buf[51],g_otp_buf[52], g_otp_buf[53],g_otp_buf[54], g_otp_buf[55]);
					printf("7th     param 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", g_otp_buf[56], g_otp_buf[57],g_otp_buf[58], g_otp_buf[59],g_otp_buf[60], g_otp_buf[61],g_otp_buf[62], g_otp_buf[63]);

				}

				//imap_get_eslc_param_20(&g_eslc_param);
				//printf("eslc param 0x%x\n", g_eslc_param);
			}
			if(nand_cfg.normal.nand_param1 == 26){
				g_retrylevel = *gp_retrylevel;
				retry_param = (uint8_t *)(rrtb_buf);
				retry_param_magic = *((uint32_t *)(rrtb_buf + 0x8));
				g_otp_buf[0] = *(retry_param + 0);
				if((retry_param_magic != 0x8a7a6a5a) || (g_otp_buf[0] == 0x0)){				
					imap_get_retry_param_26(g_otp_buf);
					g_retrylevel = 0;
					printf("call imap_get_retry_param_26 func\n");
					*((uint32_t *)(rrtb_buf + 0x8)) = 0x8a7a6a5a;
					*(retry_param + 0) = g_otp_buf[0];
					*(retry_param + 1) = g_otp_buf[1];
					*(retry_param + 2) = g_otp_buf[2];
					*(retry_param + 3) = g_otp_buf[3];
					*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
				}else{	
					g_otp_buf[0] = *(retry_param + 0);
					g_otp_buf[1] = *(retry_param + 1);
					g_otp_buf[2] = *(retry_param + 2);
					g_otp_buf[3] = *(retry_param + 3);
				}
				printf("uboot1: imap_get_retry_param_26 0x%x, 0x%x, 0x%x, 0x%x, %d\n", g_otp_buf[0], g_otp_buf[1], g_otp_buf[2], g_otp_buf[3], g_retrylevel);
			}
			if(nand_cfg.normal.nand_param1 == 21)
			{
				imap_get_retry_param_21(g_otp_buf);
				g_retrylevel = 0;
				printf("SAMSUNG 32Gb K9GBG08U0B or 64Gb K9LCG08U0B retry find\n");
			}
		}
		if(nand_cfg.normal.nand_param0 == 0x95){
//			init_retry_param_sandisk19();
			 g_retrylevel = 0;
			printf("SDTNQFAMA-004G or SDTNQFAMA-008G retry find\n");
		}
	}

	return 0;
}

struct nand_config * nand_get_config(int type) {
	switch(type) {
		case NAND_CFG_NORMAL:
			if(nand_cfg.normal_en)
			  return &nand_cfg.normal;
			break;
		case NAND_CFG_BOOT:
			if(nand_cfg.boot_en)
				return &nand_cfg.boot;
			break;
		default:
			printf("unrecgonized config type: %d\n", type);
	}

	return NULL;
}

int bnd_vs_align(void) {
	struct nand_config *cfg = nand_get_config(NAND_CFG_BOOT);
	if(cfg)
	  return cfg->pagesize;
	return 0;
}

int nnd_vs_align(void) {
	struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);
	if(cfg)
	  return cfg->pagesize;
	return 0;
}

int fnd_vs_align(void) {
	return nnd_vs_align();
}

#if 0
loff_t fnd_extent_offs(loff_t offs) {
	struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);

	if(!cfg) return 0;
	return lldiv(offs, cfg->pagesize) * (cfg->pagesize
				+ cfg->sysinfosize);
}
#endif

uint32_t fnd_align(void) {
	struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);
	if(!cfg) return 0;
	return cfg->pagesize + cfg->sysinfosize;
}

int fnd_size_match(uint32_t size) {
	struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);
	if(!cfg) return 0;
	return !(size % (cfg->pagesize + cfg->sysinfosize));
}

uint32_t fnd_size_shrink(uint32_t size) {
	struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);
	if(!cfg) return 0;
	return size / (cfg->pagesize + cfg->sysinfosize)
		* cfg->pagesize;
}

int bnd_vs_erase(loff_t start, uint32_t size)
{
//	printf("bnd_size_shift: %d\n", nand_cfg.bnd_size_shift);

    	/* BND use randomizer different from NND
	 * areas wrote by BND is recognized as bad block
	 * sometimes by NND.
	 * So here use scrub instead.
	 */
	return nnd_vs_scrub(start << nand_cfg.bnd_size_shift,
			size << nand_cfg.bnd_size_shift);
}

int fnd_vs_erase(loff_t start, uint32_t size)
{
    struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);
    return nnd_vs_erase(start,
	    size / fnd_align() * cfg->pagesize);
}

int base_vs_erase(loff_t start, uint32_t size, int scrub)
{
	/* NOTE: extent start and size to block align
	 * skip bad blocks
	 * return -4 if normal_config is not enabled.
	 */
	int row_addr = 0;
	int cs = 0;
	int i = 0;
	int page_cnt = 0;
	int ret = 0;
	int result = 0, tmp;
	struct nand_config *normal_cfg;
//	printf("erase: offset:0x%llx, length:0x%x\n", start, size);

	normal_cfg = nand_get_config(NAND_CFG_NORMAL);

//	print_nand_cfg(normal_cfg);
	if(normal_cfg != NULL)
	{	
		/* set alignment issues */
		tmp = start & (normal_cfg->blocksize - 1);
		size += tmp;

		if(tmp || (size & (normal_cfg->blocksize - 1))) {
			start &= ~(loff_t)(normal_cfg->blocksize - 1);
			size = size + (normal_cfg->blocksize - 1);
			size &= ~(normal_cfg->blocksize - 1);
			printf("erase force-aligned to: 0x%x @ 0x%llx\n", size, start);
		}

		row_addr = start >> (nand_cfg.nnd_page_shift);
		page_cnt = size / (normal_cfg->blocksize);

		nf_dbg("page_cnt = %d\n",page_cnt);
		for(i=0; i<page_cnt; i++)
		{
			nf_dbg("row_addr = 0x%x\n", row_addr);

			if(scrub || !nnd_vs_isbad(start)) {
				ret = nf2_erase(cs, row_addr, normal_cfg->cycle);
				if(ret)	{
					printf("erase block 0x%x IO error (%d)\n",
					  row_addr / normal_cfg->pagesperblock, ret);
					result = -1;
				}
			}
			else
				printf("skip bad block: 0x%llx\n", start);
			row_addr += normal_cfg->pagesperblock;
			start    += normal_cfg->blocksize;
		}
	}
	else
	{
		printf("normal_cfg is NULL\n");
		return -1;
	}	

	return result;
}

int nnd_vs_erase(loff_t start, uint32_t size) {
	return base_vs_erase(start, size, 0);
}

int nnd_vs_scrub(loff_t start, uint32_t size) {
	return base_vs_erase(start, size, 1);
}

/* bad block handling */
int base_get_badmark(int row_addr)
{
	int page_mode = 1; // not whole page
	int main_part = 0; // spare part
	int trans_dir = 0; //read op
	int ecc_en = 0;
	int secc_used = 0;
	int secc_type = 1;
	int rsvd_ecc_en = 1;
	int half_page_en = 0;
	int ret = 0;
	int ecc_unfixed = 0;
	int secc_unfixed = 0;
	uint8_t *t_buf;
	int ch1_adr = 0;
	int ch1_len = 0;
	int sdma_2ch_mode = 0;
	struct nand_config *nc;

	nc = nand_get_config(NAND_CFG_NORMAL);
	if(!nc)
	  return -1;

	t_buf = malloc(nc->sysinfosize + 4);
	if(!t_buf) {
		printf("alloc memory for spare buffer failed.\n");
		return -1;
	}

	ecc_en = 0;
	secc_used = 0;
	ret = nf2_page_op(ecc_en, page_mode, nc->mecclvl, rsvd_ecc_en,
				nc->pagesize, nc->sysinfosize, trans_dir, row_addr,
				(int)t_buf, nc->sysinfosize, secc_used, secc_type,
				half_page_en, main_part, nc->cycle, 0,
				&(nc->seed[0]), &(nc->sysinfo_seed[0]), &(nc->ecc_seed[0]), &(nc->secc_seed[0]),
				&(nc->data_last1K_seed[0]), &(nc->ecc_last1K_seed[0]), &ecc_unfixed, &secc_unfixed,
				ch1_adr, ch1_len, sdma_2ch_mode, nc->busw);
	if(ret)
	  goto __exit_err__;

		//printf("bad mark found: 0x%x 0x%x 0x%x 0x%x, %d\n", t_buf[0], t_buf[1], t_buf[2], t_buf[3], row_addr);
	if(t_buf[0] == 0xff){
		goto __exit_ok__;
	}else {
		nf_dbg("bad mark found: 0x%x 0x%x 0x%x 0x%x\n", t_buf[0], t_buf[1], t_buf[2], t_buf[3]);
		ecc_en = nc->eccen;
		secc_used = ecc_en;
		secc_type = nc->secclvl;
		ret = nf2_page_op(ecc_en, page_mode, nc->mecclvl, rsvd_ecc_en,
					nc->pagesize, nc->sysinfosize, trans_dir, row_addr,
					(int)t_buf, nc->sysinfosize, secc_used, secc_type,
					half_page_en, main_part, nc->cycle, nc->randomizer,
					&(nc->seed[0]), &(nc->sysinfo_seed[0]), &(nc->ecc_seed[0]), &(nc->secc_seed[0]),
					&(nc->data_last1K_seed[0]), &(nc->ecc_last1K_seed[0]), &ecc_unfixed, &secc_unfixed,
					ch1_adr, ch1_len, sdma_2ch_mode, nc->busw);
		if(ret)
		  goto __exit_err__;

		if(!secc_unfixed && (t_buf[0] == 0xff))
		  goto __exit_ok__;
	}

	/* this is a bad block */
	ret = 1;
__exit_ok__:
__exit_err__:
	free(t_buf);
	return ret;
}
/* bad block handling */
int nnd_vs_isbad(loff_t offset)
{
	int row_addr, ret = 0, i;
	struct nand_config *nc;
	int retrylevel = 0;
	int blk_shift, blk_addr;

//	printf("nnd_vs_isbad called\n");
	nc = nand_get_config(NAND_CFG_NORMAL);
	if(!nc)
	  return -1;
	
	offset &= ~(loff_t)(nc->blocksize - 1);
	row_addr = offset >> nand_cfg.nnd_page_shift;
	blk_shift = ffs(nc->blocksize) - 1;
	blk_addr = (int)(offset >> blk_shift);
	//printk("offset 0x%llx nc->blocksize 0x%x, blk_shift %d, blk_addr %d\n", offset, nc->blocksize, blk_shift, blk_addr);

if (bbt_flag == 1)	//get markbad from bbt
{
	if (infotm_nand_bbi != NULL) {
		for (i=0; i<infotm_nand_bbi->total_bad_num; i++) {
			nand_bbt = &infotm_nand_bbi->nand_bbt[i];
			//printf("nnd_vs_isbad from bbt: %llx %d %d %d %d \n", offset, nand_bbt->blk_addr, nand_bbt->chip_num, nand_bbt->bad_type, nand_bbt->plane_num);
			if (nand_bbt->blk_addr == blk_addr && nand_bbt->chip_num == 0 ) {
				printk("detect bad blk in bbt %llx %d %d %d \n", offset, nand_bbt->blk_addr, nand_bbt->chip_num, nand_bbt->bad_type);
				return EFAULT;
			}
		}
	}

}
else			//get markbad from nand
{
	//printf("offs 0x%llx,row addr %x\n", offset, row_addr);
	retrylevel = g_retrylevel;
	
retry_4_start:
	ret = base_get_badmark(row_addr + nc->badpagemajor);

	if(ret < 0) {
		printf("get bad block failed: 0x%llx\n", offset);
		return -1;
	}
	else if(ret > 0) {
//		printf("block 0x%llx is bad (Mm: %d) \n", offset, nc->badpagemajor);
		if(nand_cfg.normal.read_retry == 1){
			if(nand_cfg.normal.nand_param0 == 0x55){
						
					retrylevel++;
					if(retrylevel > nand_cfg.normal.retry_level)
						retrylevel = 0;
					if(g_retrylevel == retrylevel){
						printf("read retry failed, badpagemajor 0x%llx\n", offset);
						if(nand_cfg.normal.nand_param1 == 26){
							imap_set_retry_param_26(retrylevel, g_otp_buf);	
						}
						if(nand_cfg.normal.nand_param1 == 21){
							imap_set_retry_param_21(retrylevel, g_otp_buf);	
						}
						if(nand_cfg.normal.nand_param1 == 20){
							imap_set_retry_param_20(retrylevel, g_otp_buf, retry_reg_buf);
							//udelay(500000);
						}
						*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
						goto retry_4_end;
					}
					//printf("read rerty level %d\n", retrylevel);
					if(nand_cfg.normal.nand_param1 == 26){
						imap_set_retry_param_26(retrylevel, g_otp_buf);	
					}
					if(nand_cfg.normal.nand_param1 == 21){
						//printf("nand badmark param 21 retry\n");
						imap_set_retry_param_21(retrylevel, g_otp_buf);	
					}
					if(nand_cfg.normal.nand_param1 == 20){
						imap_set_retry_param_20(retrylevel, g_otp_buf, retry_reg_buf);
						//udelay(500000);
					}
					goto retry_4_start;		
			}
			if(nand_cfg.normal.nand_param0 == 0x75){
						
					retrylevel++;
					if(retrylevel > nand_cfg.normal.retry_level)
						retrylevel = 0;
					if(g_retrylevel == retrylevel){
						printf("read retry failed, badpagemajor 0x%llx\n", offset);
						if(nand_cfg.normal.nand_param1 == 20){
							retrylevel = 0;
							imap_set_retry_param_micro_20(retrylevel);
							//udelay(500000);
						}
						*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
						//return 1;
						goto retry_4_end;
					}
					//printf("read rerty level %d\n", retrylevel);
					if(nand_cfg.normal.nand_param1 == 20){
						imap_set_retry_param_micro_20(retrylevel);
						//udelay(500000);
					}
					goto retry_4_start;		
			}
			if(nand_cfg.normal.nand_param0 == 0x95){		//sandisk retry
					retrylevel++;
					if(retrylevel > nand_cfg.normal.retry_level)
						retrylevel = 0;
					if(g_retrylevel == retrylevel){
						printf("read retry failed, badpagemajor 0x%llx\n", offset);
						if(nand_cfg.normal.nand_param1 == 20){
							retrylevel = 0;
							imap_set_retry_param_sandisk19(retrylevel);
							//udelay(500000);
						}
						*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
						//return 1;
						goto retry_4_end;
					}
					if(nand_cfg.normal.nand_param1 == 20){
						imap_set_retry_param_sandisk19(retrylevel);
						//udelay(500000);
					}
					goto retry_4_start;		
			}
			if(nand_cfg.normal.nand_param0 == 0x65){        //toshiba 19nm nandflash retry
				retrylevel++;
				if(retrylevel > nand_cfg.normal.retry_level)
					retrylevel = 0;
				if(g_retrylevel == retrylevel){
					if(nand_cfg.normal.nand_param1 == 19){
						retrylevel = 0;
						imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
					}
					*((uint32_t *)(CONFIG_RESERVED_RRTB_BUFFER + 0x4)) = g_retrylevel;
					//return 1;
					goto retry_4_end;
				}
				if(nand_cfg.normal.nand_param1 == 19){
					imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
					//udelay(500000);
				}
				goto retry_4_start;
			}
		}
	}

retry_4_end:
	g_retrylevel = retrylevel;
	*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
	if(ret != 0){
		return ret;
	}else{
		if(nand_cfg.normal.read_retry == 1){	
			if(nand_cfg.normal.nand_param0 == 0x75){
				if(nand_cfg.normal.nand_param1 == 20){
					if(retrylevel != 0){
						retrylevel = 0;
						g_retrylevel = 0;
						imap_set_retry_param_micro_20(retrylevel);
					}	
				}
			}
			if(nand_cfg.normal.nand_param1 == 21)
			{
				if(retrylevel != 0)
				{
					retrylevel = 0;
					g_retrylevel = 0;
					//printf("param 21 retry 4 end.\n");
					imap_set_retry_param_21(retrylevel, g_otp_buf);
				}	
			}
			if(nand_cfg.normal.nand_param0 == 0x95)
			{
				if(retrylevel != 0)
				{
					retrylevel = 0;
					g_retrylevel = 0;
					imap_set_retry_param_sandisk19(retrylevel);
				}	
			}
			if(nand_cfg.normal.nand_param0 == 0x65)
			{
				if(nand_cfg.normal.nand_param1 == 19 && retrylevel != 0){
					retrylevel = 0;
					g_retrylevel = 0;
					imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
				}
			}

		}
	}	
	
	if(nc->badpageminor != nc->badpagemajor) {
retry_5_start:
		ret = base_get_badmark(row_addr + nc->badpageminor);
		if(ret < 0) {
			printf("get bad block failed: 0x%llx\n", offset);
			return -1;
		}
		else if(ret > 0) {
			if(nand_cfg.normal.read_retry == 1){
				if(nand_cfg.normal.nand_param0 == 0x55){
							
						retrylevel++;
						if(retrylevel > nand_cfg.normal.retry_level)
							retrylevel = 0;
						if(g_retrylevel == retrylevel){
							printf("read retry failed, badpageminor 0x%llx\n", offset);
							if(nand_cfg.normal.nand_param1 == 26){
								imap_set_retry_param_26(retrylevel, g_otp_buf);
							}
						if(nand_cfg.normal.nand_param1 == 21){
							imap_set_retry_param_21(retrylevel, g_otp_buf);	
						}
							if(nand_cfg.normal.nand_param1 == 20){
								imap_set_retry_param_20(retrylevel, g_otp_buf, retry_reg_buf);
								//udelay(500000);
							}
							*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
							goto retry_5_end;
						}
						//printf("read rerty level %d\n", retrylevel);
						if(nand_cfg.normal.nand_param1 == 26){
							imap_set_retry_param_26(retrylevel, g_otp_buf);
						}
					if(nand_cfg.normal.nand_param1 == 21){
						imap_set_retry_param_21(retrylevel, g_otp_buf);	
					}
						if(nand_cfg.normal.nand_param1 == 20){
							imap_set_retry_param_20(retrylevel, g_otp_buf, retry_reg_buf);
							//udelay(500000);
						}
						goto retry_5_start;		
				}
			if(nand_cfg.normal.nand_param0 == 0x75){
						
					retrylevel++;
					if(retrylevel > nand_cfg.normal.retry_level)
						retrylevel = 0;
					if(g_retrylevel == retrylevel){
						printf("read retry failed, badpageminor 0x%llx\n", offset);
						if(nand_cfg.normal.nand_param1 == 20){
							retrylevel = 0;
							imap_set_retry_param_micro_20(retrylevel);
							//udelay(500000);
						}
						*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
						goto retry_5_end;
					}
					//printf("read rerty level %d\n", retrylevel);
					if(nand_cfg.normal.nand_param1 == 20){
						imap_set_retry_param_micro_20(retrylevel);
						//udelay(500000);
					}
					goto retry_5_start;		
			}
			if(nand_cfg.normal.nand_param0 == 0x95){		//sandisk retry
					retrylevel++;
					if(retrylevel > nand_cfg.normal.retry_level)
						retrylevel = 0;
					if(g_retrylevel == retrylevel){
						printf("read retry failed, badpageminor 0x%llx\n", offset);
						if(nand_cfg.normal.nand_param1 == 20){
							retrylevel = 0;
							imap_set_retry_param_sandisk19(retrylevel);
							//udelay(500000);
						}
						*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
						//return 1;
						goto retry_4_end;
					}
					if(nand_cfg.normal.nand_param1 == 20){
						imap_set_retry_param_sandisk19(retrylevel);
						//udelay(500000);
					}
					goto retry_4_start;
				}
				if(nand_cfg.normal.nand_param0 == 0x65){		//toshiba 19nm nandflash retry
					retrylevel++;
					if(retrylevel > nand_cfg.normal.retry_level)
						retrylevel = 0;
					if(g_retrylevel == retrylevel){
						if(nand_cfg.normal.nand_param1 == 19){
							retrylevel = 0;
							imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
						}
						*((uint32_t *)(CONFIG_RESERVED_RRTB_BUFFER + 0x4)) = g_retrylevel;
						//return 1;
						goto retry_5_end;
					}
					if(nand_cfg.normal.nand_param1 == 19){
						imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
						//udelay(500000);
					}
					goto retry_5_start;
				}
			}

		}
	}

retry_5_end:
	if(nand_cfg.normal.read_retry == 1){	
		if(nand_cfg.normal.nand_param0 == 0x75){
			if(nand_cfg.normal.nand_param1 == 20){
				if(retrylevel != 0){
					retrylevel = 0;
					g_retrylevel = 0;
					imap_set_retry_param_micro_20(retrylevel);
				}	
			}
		}
		if(nand_cfg.normal.nand_param1 == 21)
		{
			if(retrylevel != 0)
			{
				retrylevel = 0;
				g_retrylevel = 0;
//				printf("param 21 retry 5 end.\n");
				imap_set_retry_param_21(retrylevel, g_otp_buf);
			}	
		}
		if(nand_cfg.normal.nand_param0 == 0x95 && retrylevel != 0)
		{
			retrylevel = 0;
			g_retrylevel = 0;
			imap_set_retry_param_sandisk19(retrylevel);
		}
		if(nand_cfg.normal.nand_param0 == 0x65)
		{
			if(nand_cfg.normal.nand_param1 == 19 && retrylevel != 0){
				retrylevel = 0;
				g_retrylevel = 0;
				imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
			}
		}

	}
	g_retrylevel = retrylevel;
	*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
	//printf("nand scan bad is %d\n", ret);
}
	return ret;
}

int bnd_vs_isbad(loff_t offset) {
	return nnd_vs_isbad(offset << nand_cfg.bnd_size_shift);
}

int fnd_vs_isbad(loff_t offset) {
	return nnd_vs_isbad(offset);
}

#if 0
/* this two interface is deprecated, by warits Fri Nov 18, 2011 */
int nand_seek_good(loff_t *offs, int leeway)
{
	struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);
	int i;

	if(!cfg) return -1;

	/* align to block size */
	loff_t addr = *offs & ~(loff_t)(cfg->blocksize - 1);
	for(i = 0; i < leeway; i += cfg->blocksize) {
		if (!nnd_vs_isbad (addr)) {
			break;
		}
		printf ("Skipping bad block 0x%llx\n", addr);
		addr += cfg->blocksize;
	}

	if(i < leeway)
	  *offs += i;

	return i;
}

int nand_seek_good_absolute(loff_t *offs, int leeway)
{
	struct nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);
	int i;

	if(!cfg) return -1;

	loff_t cap = 0;

	if(*offs > leeway) {
		printf("offset beyond seek range.\n");
		return -1;
	}

	for(i = 0; i < leeway; i += cfg->blocksize) {
		if (!nnd_vs_isbad (i))
		  cap += cfg->blocksize;
		else
		  printf ("Skipping bad block 0x%x\n", i);

		if(cap > *offs)
		  break;
	}

	if(i < leeway)
	  *offs = i + (*offs & (loff_t)(cfg->blocksize - 1));

	return i;
}
#endif

/* bnd APIs */
int bnd_vs_reset(void) {
	struct nand_config *bc, *nc;
	int ret;
	
	nand_emu_init();
	/* rescan boot config */
	nand_rescan_config(NULL, 0);

//	printf("bnd_vs_reset\n");
	bc = nand_get_config(NAND_CFG_BOOT);
	nc = nand_get_config(NAND_CFG_NORMAL);

	if(!bc)
	  return -1;

	/* use normal config if availible */
	if(nc) bc = nc;
		
	/* always use async interface for bnd */
	ret = nf2_hardware_reset(bc, 0);
	if(ret) {
		printf("bnd hardware reset failed (%d)\n", ret);
		return ret;
	}
	//udelay(500000);

	return 0;
}

/**
 * return 0 successful, return -1 failed
 */
int bnd_vs_read(uint8_t *buf, loff_t start,
			int length, int extra) {
	/* skip bad blocks */
	int page_cnt = 0;
	int page_mode = 0; //whole page
	int main_part = 1; //main part
	int rsvd_ecc_en = 1;
	int secc_used = 0;
	int secc_type = 0;
	int half_page_en = 0;
	int sysinfo_num = 0;
	int trans_dir = 0; //read op
	int row_addr = 0;
	int cur_buf_addr = 0;
	int i = 0;
	int ret = 0;
	int ecc_unfixed = 0;
	int secc_unfixed = 0;
	unsigned int ch1_adr = 0;
        int ch1_len = 0;
        int sdma_2ch_mode = 0;
	int rsvdbuf = 0x0;
	int check_rsvdbuf = 0x0;
	int total_read_cnt = 0;
	struct nand_config *boot_cfg;


	boot_cfg = nand_get_config(NAND_CFG_BOOT);
	
	if(boot_cfg != NULL)
	{
		cur_buf_addr = (int)buf;
		row_addr = start >> (nand_cfg.bnd_page_shift);	
		page_cnt = length / boot_cfg->pagesize;

		secc_used = boot_cfg->eccen;
		secc_type = boot_cfg->secclvl;

		for(i=0; i<page_cnt; i++)
		{
//			printf("bnd read page: 0x%x\n", row_addr);
			sdma_2ch_mode = 1;
			ch1_adr = (unsigned int)&rsvdbuf;
			ch1_len = 4;
			nf_dbg("ch0_adr = %x, ch1_adr = %x, ch0_len = %d, ch1_len = %d, row_addr = %d\n", cur_buf_addr, ch1_adr, boot_cfg->pagesize, ch1_len, row_addr);
			ret = nf2_page_op(boot_cfg->eccen, page_mode, boot_cfg->mecclvl, rsvd_ecc_en,
        	            			boot_cfg->pagesize, sysinfo_num, trans_dir, row_addr,
			    			cur_buf_addr, boot_cfg->pagesize, secc_used, secc_type,
        	            			half_page_en, main_part, boot_cfg->cycle, boot_cfg->randomizer,
						&(boot_cfg->seed[0]), &(boot_cfg->sysinfo_seed[0]), &(boot_cfg->ecc_seed[0]), &(boot_cfg->secc_seed[0]),
						&(boot_cfg->data_last1K_seed[0]), &(boot_cfg->ecc_last1K_seed[0]), &ecc_unfixed, &secc_unfixed,
						ch1_adr, ch1_len, sdma_2ch_mode, boot_cfg->busw);
			if(ret != 0)
				return -1;

			nf_dbg("bnd_vs_read rsvdbuf = 0x%x\n", rsvdbuf);

			/* shift resverd buffer to get crc16 */
			rsvdbuf >>= 16;
			rsvdbuf &= 0xffff;

			if(ecc_unfixed == 0 && secc_unfixed == 0)
			{

#if defined(BND_USE_MAGIC)
				check_rsvdbuf = BND_USE_MAGIC;
#else
				check_rsvdbuf = cyg_crc16((uint8_t *)cur_buf_addr, boot_cfg->pagesize);
#endif
				if(check_rsvdbuf == rsvdbuf)
				{
					total_read_cnt += boot_cfg->pagesize;	
					cur_buf_addr += boot_cfg->pagesize;	
				} else {
//					printf("skip bnd page(ori:%04x<->calc:%04x): 0x%x\n",
//							rsvdbuf, check_rsvdbuf, row_addr);
				}
			}

			row_addr++;
		}
	}
	else
	{
		return -1;
	}	

	nf_dbg("bnd_vs_read total_read_cnt = %d\n", total_read_cnt);
	return total_read_cnt;
}

int bnd_vs_write(uint8_t *buf, loff_t start,
			int length, int extra) {
	/* skip bad blocks */
	
	int page_cnt = 0;
	int page_mode = 0; //whole page
	int main_part = 1; //main part
	int rsvd_ecc_en = 1;
	int secc_used = 1;
	int secc_type = 0;
	int half_page_en = 0;
	int sysinfo_num = 0;
	int trans_dir = 1; //write op
	int row_addr = 0;
	int cur_buf_addr = 0;
	int i = 0;
	int ret = 0;
	int ecc_unfixed = 0;
	int secc_unfixed = 0;
	unsigned int ch1_adr = 0;
	int ch1_len = 0;
	int sdma_2ch_mode = 0;
	int rsvdbuf = 0x0;
	int total_read_cnt = 0;
	struct nand_config *boot_cfg;
	struct nand_config *normal_cfg;

//	printf("bnd_vs_write\n");

	boot_cfg = nand_get_config(NAND_CFG_BOOT);
	normal_cfg = nand_get_config(NAND_CFG_NORMAL);

	if(boot_cfg->read_retry == 1){
		if(boot_cfg->nand_param0 == 0x55){
			if(boot_cfg->nand_param1 == 20){
				g_eslc_param += 0x0a0a0a0a;
				printf("set eslc param = 0x%x\n", g_eslc_param);
				//imap_set_eslc_param_20(&g_eslc_param);
			}
		}
	}

	if(boot_cfg != NULL && normal_cfg != NULL)
	{
		cur_buf_addr = (int)buf;
		row_addr = start >> (nand_cfg.bnd_page_shift);	
		page_cnt = length / boot_cfg->pagesize;

		secc_used = boot_cfg->eccen;
		secc_type = boot_cfg->secclvl;

		for(i=0; i<page_cnt; i++)
		{

			sdma_2ch_mode = 1;
			ch1_adr = (unsigned int)&rsvdbuf;
			ch1_len = 4;
			nf_dbg("ch0_adr = %x, ch1_adr = %x, ch0_len = %d, ch1_len = %d, row_addr = %d\n", cur_buf_addr, ch1_adr, boot_cfg->pagesize, ch1_len, row_addr);
		        
#if defined(BND_USE_MAGIC)
			rsvdbuf = BND_USE_MAGIC;
#else
			rsvdbuf = cyg_crc16((uint8_t *)cur_buf_addr, boot_cfg->pagesize);
#endif
			nf_dbg("bnd_vs_write rsvdbuf = 0x%x\n", rsvdbuf);

			/* the 4 reserved bytes should be: bad mark | dirty mark | crc16
			 * e.g. 0xff | 0x0 | 0x3746
			 * the dword format: 0x374600ff
			 */
			rsvdbuf = rsvdbuf<<16;
			rsvdbuf |= 0xff;

			nf_dbg("bnd_vs_write rsvdbuf = 0x%x\n", rsvdbuf);
			ret = nf2_page_op(boot_cfg->eccen, page_mode, boot_cfg->mecclvl, rsvd_ecc_en,
        	            			boot_cfg->pagesize, sysinfo_num, trans_dir, row_addr,
			    			cur_buf_addr, boot_cfg->pagesize, secc_used, secc_type,
        	            			half_page_en, main_part, boot_cfg->cycle, boot_cfg->randomizer, 
						&(boot_cfg->seed[0]), &(boot_cfg->sysinfo_seed[0]), &(boot_cfg->ecc_seed[0]), &(boot_cfg->secc_seed[0]),
						&(boot_cfg->data_last1K_seed[0]), &(boot_cfg->ecc_last1K_seed[0]), &ecc_unfixed, &secc_unfixed,
						ch1_adr, ch1_len, sdma_2ch_mode, boot_cfg->busw);
			if(ret != 0)
				return -1;

			row_addr++;
			cur_buf_addr += boot_cfg->pagesize;
			total_read_cnt += boot_cfg->pagesize;	
		}
	}
	else
	{
		return -1;
	}	
	
	if(boot_cfg->read_retry == 1){
		if(boot_cfg->nand_param0 == 0x55){
			if(boot_cfg->nand_param1 == 20){
				g_eslc_param -= 0x0a0a0a0a;
				printf("set eslc param = 0x%x\n", g_eslc_param);
				//imap_set_eslc_param_20(&g_eslc_param);
				//imap_eslc_end();
				printf("program eslc end\n");
			}
		}
	}

	return total_read_cnt;
}


/* nand APIs */
int nnd_vs_reset(void) {
	/* return -4 if normal config is not enabled. */
	int ret = 0;

	struct nand_config *normal_cfg;
//	printf("nand_vs_reset\n");

	nand_emu_init();
	normal_cfg = nand_get_config(NAND_CFG_NORMAL);
	
	if(normal_cfg == NULL)
	{	
		ret = -4;	
		goto nnd_vs_reset_end;
	}
	
	ret = nf2_hardware_reset(normal_cfg, 1);
	if(ret) {
		if(normal_cfg->interface != 1) {
			printf("chip set to interface: %d failed\n", normal_cfg->interface);
			return -4;
		}

		printf("try to set interface to async.\n");
		normal_cfg->interface = 0;
		ret = nf2_hardware_reset(normal_cfg, 0);

		if(ret) {
			printf("chip set to interface: %d failed\n", normal_cfg->interface);
			return -4;
		}
	}

	//udelay(500000);
nnd_vs_reset_end:
	return ret;
}

int base_scan_badblocks(void)
{
	int i, ret, count = 0;
	loff_t offset;
	struct nand_config *nc;

	nc = nand_get_config(NAND_CFG_NORMAL);
	if(!nc) {
		printf("can not get normal config.\n");
		return -1;
	}

	for(i = 0, offset = 0; i < nc->blockcount;
				i++, offset += nc->blocksize) {
		ret = nnd_vs_isbad(offset);
		if(ret) {
			if(ret < 0)
				printf("block scan failed:  0x%x\n", i);
			count++;
		}
	}

	printf("%d blocks in %d is bad (%d%%)\n", count,
			nc->blockcount, count * 100 / nc->blockcount);

	return 0;
}

int base_vs_read(uint8_t *buf, loff_t start,
			int length, int extra) {
	/* skip bad blocks */
	int page_cnt = 0;
	int page_mode = 0; //whole page
	int main_part = 1; //main part
	int rsvd_ecc_en = 1;
	int secc_used = 0;
	int secc_type = 0;
	int half_page_en = 0;
	int trans_dir = 0; //read op
	int row_addr = 0;
	int cur_buf_addr = 0;
	int i = 0, j = 0;
	int ret = 0;
	int ecc_unfixed = 0;
	int secc_unfixed = 0;
	unsigned int ch1_adr = 0;
	int ch1_len = 0;
        int sdma_2ch_mode = 0;
	uint8_t *prsvdbuf = NULL;
	int total_read_cnt = 0;
	struct nand_config *normal_cfg;

//	printf("nand_vs_read\n");

	normal_cfg = nand_get_config(NAND_CFG_NORMAL);
	prsvdbuf = (uint8_t *)malloc((normal_cfg->sysinfosize) + 4);
	if(prsvdbuf == NULL)
	{
		printf("nand_vs_read malloc %d fail\n", (normal_cfg->sysinfosize) + 4);
		return -1;
	}

	if(normal_cfg != NULL)
	{	
		cur_buf_addr = (int)buf;
		row_addr = start >> (nand_cfg.nnd_page_shift);	
		page_cnt = length / (normal_cfg->pagesize + (extra?normal_cfg->sysinfosize:0));
		secc_used = normal_cfg->eccen;
		secc_type = normal_cfg->secclvl;

		for(i=0; i<page_cnt; i++)
		{
			//printf("nand_vs_read row_addr = 0x%x\n", row_addr);
			sdma_2ch_mode = 1;
			ch1_adr = (unsigned int)prsvdbuf;
			ch1_len = 4 + normal_cfg->sysinfosize;
			ret = nf2_page_op(normal_cfg->eccen, page_mode, normal_cfg->mecclvl, rsvd_ecc_en,
        	            			normal_cfg->pagesize, normal_cfg->sysinfosize, trans_dir, row_addr,
			    			cur_buf_addr, normal_cfg->pagesize, secc_used, secc_type,
        	            			half_page_en, main_part, normal_cfg->cycle, normal_cfg->randomizer,
						&(normal_cfg->seed[0]), &(normal_cfg->sysinfo_seed[0]), &(normal_cfg->ecc_seed[0]), &(normal_cfg->secc_seed[0]),
						&(normal_cfg->data_last1K_seed[0]), &(normal_cfg->ecc_last1K_seed[0]), &ecc_unfixed, &secc_unfixed,
						ch1_adr, ch1_len, sdma_2ch_mode, normal_cfg->busw);
			
			
			if(ret != 0)
			{
				ret = -1;
				goto nand_vs_read_end;
			}
			
			nf_dbg("rsvdbuf = 0x%x, 0x%x, 0x%x, 0x%x\n", *prsvdbuf, *(prsvdbuf+1), *(prsvdbuf+2), *(prsvdbuf+3));
			if(*(prsvdbuf + 1) == 0xff)
			{
				if(extra == 0)
					memset((void *)cur_buf_addr, 0xff, normal_cfg->pagesize);
				else
					memset((void *)cur_buf_addr, 0xff, normal_cfg->pagesize + normal_cfg->sysinfosize);
			}
			else
			{
				if(ret || ecc_unfixed || secc_unfixed)
				{
					ret = -1;
					//printf("ecc unfixed: %d.%d\n", !!ecc_unfixed, !!secc_unfixed);
					goto nand_vs_read_end;
				}	
			}	
			
			if(extra == 1)
			{	
				for(j=0; j<normal_cfg->sysinfosize; j++)
				{
					*(uint8_t *)(cur_buf_addr + normal_cfg->pagesize + j) = prsvdbuf[j + 4];
				}
			}
			row_addr++;
			if(extra == 0)
			{	
				cur_buf_addr += normal_cfg->pagesize;	
				total_read_cnt += normal_cfg->pagesize;
			}
			else
			{	
				cur_buf_addr += (normal_cfg->pagesize + normal_cfg->sysinfosize);	
				total_read_cnt += (normal_cfg->pagesize + normal_cfg->sysinfosize);
			}
			ret = total_read_cnt;
		}
	}
	else
	{
		ret = -1;
		goto nand_vs_read_end;
	}

nand_vs_read_end:
	free(prsvdbuf);
	return ret;
}




int base_vs_write(uint8_t *buf, loff_t start,
			int length, int extra) {
	/* skip bad blocks */
	int page_cnt = 0;
	int page_mode = 0; //whole page
	int main_part = 1; //main part
	int rsvd_ecc_en = 1;
	int secc_used = 1;
	int secc_type = 0;
	int half_page_en = 0;
	int trans_dir = 1; //write op
	int row_addr = 0;
	int cur_buf_addr = 0;
	int i = 0, j = 0;
	int ret = 0;
	int ecc_unfixed = 0;
	int secc_unfixed = 0;
	unsigned int ch1_adr = 0;
        int ch1_len = 0;
        int sdma_2ch_mode = 0;
	uint8_t *prsvdbuf = NULL;
	uint8_t *t_prsvdbuf = NULL;
	struct nand_config *normal_cfg;
	int total_write_cnt = 0;
//	printf("nand_vs_write\n");
	
	normal_cfg = nand_get_config(NAND_CFG_NORMAL);
	prsvdbuf = (uint8_t *)malloc((normal_cfg->sysinfosize) + 4);

	if(prsvdbuf == NULL)
	{
		printf("nand_vs_write malloc %d fail\n", (normal_cfg->sysinfosize) + 4);
		return -1;
	}	
	if(extra == 0)
	{
		t_prsvdbuf = prsvdbuf;
		for(i = 0; i<((normal_cfg->sysinfosize) + 4); i++)
		{
			*t_prsvdbuf++ = 0xff;
		}
		*(prsvdbuf + 1) = 0x0; //write flag
	}
	if(normal_cfg != NULL)
	{	
		cur_buf_addr = (int)buf;
		row_addr = start >> (nand_cfg.nnd_page_shift);	
		page_cnt = length / (normal_cfg->pagesize + (extra?normal_cfg->sysinfosize:0));
		secc_used = normal_cfg->eccen;
		secc_type = normal_cfg->secclvl;

//		printf("write to page: 0x%x\n", row_addr);

		for(i=0; i<page_cnt; i++)
		{
			//printk("nand_vs_write row_addr = 0x%x\n", row_addr);
			sdma_2ch_mode = 1;
			if(extra == 1)
			{
				t_prsvdbuf = prsvdbuf;
				*(t_prsvdbuf + 0) = 0xff;
				*(t_prsvdbuf + 1) = 0x00;
				*(t_prsvdbuf + 2) = 0xff;
				*(t_prsvdbuf + 3) = 0xff; //write flag
				for(j=0; j<normal_cfg->sysinfosize; j++)
				{
					t_prsvdbuf[j+4] = *(uint8_t *)(cur_buf_addr + normal_cfg->pagesize + j);
				}		
			}

			ch1_adr = (unsigned int)prsvdbuf;
			ch1_len = 4 + normal_cfg->sysinfosize;
			
			ret = nf2_page_op(normal_cfg->eccen, page_mode, normal_cfg->mecclvl, rsvd_ecc_en,
        	            			normal_cfg->pagesize, normal_cfg->sysinfosize, trans_dir, row_addr,
			    			cur_buf_addr, normal_cfg->pagesize, secc_used, secc_type,
        	            			half_page_en, main_part, normal_cfg->cycle, normal_cfg->randomizer,
						&(normal_cfg->seed[0]), &(normal_cfg->sysinfo_seed[0]), &(normal_cfg->ecc_seed[0]), &(normal_cfg->secc_seed[0]),
						&(normal_cfg->data_last1K_seed[0]), &(normal_cfg->ecc_last1K_seed[0]), &ecc_unfixed, &secc_unfixed,
						ch1_adr, ch1_len, sdma_2ch_mode, normal_cfg->busw);
			if(ret != 0)
			{
				ret = -1;
				goto nand_vs_write_end;
			}	

			row_addr++;
			if(extra == 0)
			{	
				cur_buf_addr += normal_cfg->pagesize;	
				total_write_cnt += normal_cfg->pagesize;
			}
			else
			{	
				cur_buf_addr += (normal_cfg->pagesize + normal_cfg->sysinfosize);	
				total_write_cnt += (normal_cfg->pagesize + normal_cfg->sysinfosize);
			}
			ret = total_write_cnt;			
		}
	}
	else
	{
		ret = -1;
		goto nand_vs_write_end;
	}

nand_vs_write_end:
	free(prsvdbuf);	
	return ret;
}

int __base_block_markbad(loff_t offset)
{

	struct nand_config *nc = nand_get_config(NAND_CFG_NORMAL);
	int row_addr;
	uint8_t buf[4] = {0, 0, 0, 0};
	int meccstat, seccstat, ret;

	if(!nc)
	  return -1;

	row_addr = offset >> nand_cfg.nnd_page_shift;
	ret = nf2_page_op(nc->eccen, 1,		/* part write */
				nc->mecclvl, 1,			/* rsvd ecc on */
				nc->pagesize,
				nc->sysinfosize,
				1, row_addr,					/* write */
				0,
				nc->pagesize,
				nc->eccen,
				nc->secclvl,
				0,
				0,								/* write spare only */
				nc->cycle,
				nc->randomizer,
				&(nc->seed[0]),
				&(nc->sysinfo_seed[0]),
				&(nc->ecc_seed[0]),
				&(nc->secc_seed[0]),
				&(nc->data_last1K_seed[0]),
				&(nc->ecc_last1K_seed[0]),
				&meccstat,
				&seccstat,
				(uint32_t)buf,
				4, 1, nc->busw);

	return ret;
}

int base_block_markbad(loff_t offset)
{
	struct nand_config *nc = nand_get_config(NAND_CFG_NORMAL);
	int ret;

	if(!nc)
	  return -1;

	offset &= ~(loff_t)(nc->blocksize - 1);
	printf("marking offset: 0x%llx as bad.\n", offset);

	ret = nnd_vs_erase(offset, nc->blocksize);
	if(ret)
	  printf("warning: erase failed.\n");

	ret = __base_block_markbad(offset + nc->badpagemajor * nc->pagesize);

	if(nc->badpageminor != nc->badpagemajor)
	  ret |= __base_block_markbad(offset + nc->badpageminor * nc->pagesize);

	if(ret)
	  printf("warning: mark bad may not successful.\n");

	return ret;
}

int nnd_vs_write(uint8_t *buf, loff_t start,
			int length, int extra) {
	int total_write_cnt = 0;

	total_write_cnt = base_vs_write(buf, start, length, 0);
	return total_write_cnt;
}

int nnd_vs_read(uint8_t *buf, loff_t start,
			int length, int extra) {
	/* XXX experimental skip bad XXX */
	struct nand_config *nc = nand_get_config(NAND_CFG_NORMAL);
	int w, block = nc->blocksize, skip, remain;
	unsigned int ret = 0;
	unsigned int retrylevel = 0;

	for(remain = length; remain; ) {
		
#if 1
		for(skip = 0; vs_isbad(start & ~(block - 1));
					skip++, start += block) {
			printf("bad block skipped @ 0x%llx\n", start & ~(block - 1));
			if(skip > 30) {
				printf("two many bad blocks skipped"
						"before getting the corrent data.\n");
				return -1;
			}
		}
#endif
		w = min(nc->pagesize, 		// block ---> page, read retry need from page not block
			min(block - (start & (block - 1)), // block remain
				remain)); // remain

					
		retrylevel = g_retrylevel;
retry_1_start:			
		ret = base_vs_read(buf, start, w, 0);

		if(nc->read_retry == 1){
			if(nc->nand_param0 == 0x55){
					
					if(ret == -1){
						retrylevel++;
						if(retrylevel > nc->retry_level)
							retrylevel = 0;
						if(g_retrylevel == retrylevel){
							printf("read retry failed, vs read 0x%llx\n", start);
							if(nc->nand_param1 == 26){
								imap_set_retry_param_26(retrylevel, g_otp_buf);
							}
							if(nand_cfg.normal.nand_param1 == 21){
								imap_set_retry_param_21(retrylevel, g_otp_buf);	
							}
							if(nc->nand_param1 == 20){
								imap_set_retry_param_20(retrylevel, g_otp_buf, retry_reg_buf);
								//udelay(500000);
							}
							goto retry_1_end;
						}
						//printf("offset = 0x%llx, read rerty level %d\n", start, retrylevel);
						if(nc->nand_param1 == 26){
							imap_set_retry_param_26(retrylevel, g_otp_buf);
						}
						if(nand_cfg.normal.nand_param1 == 21){
							//printf("vs read param retry\n");
							imap_set_retry_param_21(retrylevel, g_otp_buf);	
						}
						if(nc->nand_param1 == 20){
							imap_set_retry_param_20(retrylevel, g_otp_buf, retry_reg_buf);
							//udelay(500000);
						}
						goto retry_1_start;	
					}else{
						g_retrylevel = retrylevel;
						*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
					}
			}
			if(nand_cfg.normal.nand_param0 == 0x75){
					
				if(ret == -1){	
					retrylevel++;
					if(retrylevel > nand_cfg.normal.retry_level)
						retrylevel = 0;
					if(g_retrylevel == retrylevel){
						printf("read retry failed, vs read 0x%llx\n", start);
						if(nand_cfg.normal.nand_param1 == 20){
							retrylevel = 0;
							imap_set_retry_param_micro_20(retrylevel);
							//udelay(500000);
						}
						//return 1;
						goto retry_1_end;
					}
					//printf("read rerty level %d\n", retrylevel);
					if(nand_cfg.normal.nand_param1 == 20){
						imap_set_retry_param_micro_20(retrylevel);
						//udelay(500000);
					}
					goto retry_1_start;	
				}else{
				
				}	
			}
			if(nand_cfg.normal.nand_param0 == 0x95 && ret == -1){
				retrylevel++;
				if(retrylevel > nand_cfg.normal.retry_level)
					retrylevel = 0;
				if(g_retrylevel == retrylevel){
					printf("read retry failed, vs read 0x%llx\n", start);
					if(nand_cfg.normal.nand_param1 == 20){
						retrylevel = 0;
						imap_set_retry_param_sandisk19(retrylevel);
						//udelay(500000);
					}
					//return 1;
					goto retry_1_end;
				}
				if(nand_cfg.normal.nand_param1 == 20){
					imap_set_retry_param_sandisk19(retrylevel);
					//udelay(500000);
				}
				goto retry_1_start;	
			}
			if(nand_cfg.normal.nand_param0 == 0x65 && ret == -1){		//toshiba 19nm nandflash retry
				retrylevel++;
				if(retrylevel > nand_cfg.normal.retry_level)
					retrylevel = 0;
				if(g_retrylevel == retrylevel){
					if(nand_cfg.normal.nand_param1 == 19){
						retrylevel = 0;
						imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
					}
					*((uint32_t *)(CONFIG_RESERVED_RRTB_BUFFER + 0x4)) = g_retrylevel;
					//return 1;
					goto retry_1_end;
				}
				if(nand_cfg.normal.nand_param1 == 19){
					imap_set_retry_param_toshiba_19(retrylevel, g_otp_buf);
					//udelay(500000);
				}
				goto retry_1_start;
			}
		}
retry_1_end:
		if(nand_cfg.normal.read_retry == 1){	
			if(nand_cfg.normal.nand_param0 == 0x75){
				if(nand_cfg.normal.nand_param1 == 20){
					if(retrylevel != 0){
						retrylevel = 0;
						g_retrylevel = 0;
						imap_set_retry_param_micro_20(retrylevel);
					}	
				}
			}
			if(nand_cfg.normal.nand_param1 == 21)
			{
				if(retrylevel != 0)
				{
					retrylevel = 0;
					g_retrylevel = 0;
//					printf("param 21 retry 1 end.\n");
					imap_set_retry_param_21(retrylevel, g_otp_buf);
				}	
			}
			if(nand_cfg.normal.nand_param0 == 0x95 && retrylevel != 0)
			{
				retrylevel = 0;
				g_retrylevel = 0;
				//printf("sandisk param 0x95 retry 1 end.\n");
				imap_set_retry_param_sandisk19(retrylevel);
			}
		}
		*((uint32_t *)(rrtb_buf + 0x4)) = g_retrylevel;
		if(ret == -1)
			return -1;

		buf    += w;
		start  += w;
		remain -= w;
	}

	return length;
}
/* fnd APIs */
int fnd_vs_reset(void) {
	/* return -4 if normal config is not enabled. */
	int ret = 0;

	ret = nnd_vs_reset();
	//udelay(500000);
	return ret;
}

int fnd_vs_write(uint8_t *buf, loff_t start,
			int length, int extra) {
	int total_write_cnt = 0;

	total_write_cnt = base_vs_write(buf, start, length, 1);
	return total_write_cnt;
}

int fnd_vs_read(uint8_t *buf, loff_t start,
			int length, int extra) {
	int total_read_cnt = 0;

	total_read_cnt = base_vs_read(buf, start, length, 1);
	return total_read_cnt;
}

int nand_init_done = 0;

int nand_emu_init(void)
{
	int ret;
//	printf("%s:\n", __func__);
	if(nand_init_done)
	  return 0;

//	printf("%s:\n", __func__);
	if(boot_device() != DEV_BND) {
		set_xom(0xd);
		init_mux(0);
//		printf("%s mux set:\n", __func__);
	}

	ret = nand_rescan_config(infotm_nand_idt, infotm_nand_count());
	if(ret) {
		printf("init NANDFLASH failed.\n");
		return ret;
	}

	nand_init_done = 1;
	return 0;
}

/* __init_call:
 * this functions will be call at boot time
 */
int bbt_check(void)
{
	int from, len, ret, i;
       	uint32_t crc;
	env_t *env_ptr;
	unsigned char *cmd[64];
	struct	nand_config *cfg = nand_get_config(NAND_CFG_NORMAL);

	from = BBT_START;
	len = sizeof(env_t);

	data_buf = malloc(cfg->pagesize);
	if(data_buf == NULL)
		return -ENOMEM;
	memset((void *)data_buf, 0xff, len);

	/*read bbt struct from nand*/
	printf("=========bbt test start=======\n");
	sprintf(cmd, "vs assign nnd");
	printf("%s\n", cmd);
	run_command(cmd, 0);
	
	ret = nnd_vs_read(data_buf, BBT_START, cfg->pagesize, 0);
	printf("bbt read len 0x%x\n", ret);
	if(ret != cfg->pagesize)
	{
		printf("bbt read err\n");
		goto bbt_err;
	}
	env_ptr = (struct env_t *)data_buf;
	crc = (crc32((0 ^ 0xffffffffL), env_ptr->data, BBT_SIZE) ^ 0xffffffffL);
	printf("infotm nand bbt crc %x ori %x \n", crc, env_ptr->crc);
	if(crc != env_ptr->crc)
	{
		printf("nand bbt crc is not match!");
		goto bbt_err;
	}			

	infotm_nand_bbi = (struct infotm_nand_bbt_info *)(env_ptr->data);
	printf("infotm_nand_bbi head %.4s, tail %.4s, bad_num %d\n", infotm_nand_bbi->bbt_head_magic, infotm_nand_bbi->bbt_tail_magic, infotm_nand_bbi->total_bad_num);
	
	/*analyze bbt*/
	if (memcmp(infotm_nand_bbi->bbt_head_magic, BBT_HEAD_MAGIC, 4) || memcmp(infotm_nand_bbi->bbt_tail_magic, BBT_TAIL_MAGIC, 4))
	{
		printf("bbt head tail magic not match!\n");
		goto bbt_err;
	}
	else
		for (i=0; i<infotm_nand_bbi->total_bad_num; i++) {
			nand_bbt = &infotm_nand_bbi->nand_bbt[i];
			printf("bad block from bbt: %d %d %d %d \n", nand_bbt->blk_addr, nand_bbt->chip_num, nand_bbt->bad_type, nand_bbt->plane_num);
		}


	printf("=========bbt test end=======\n");
	printf("read bbt OK, get markbad from bbt!\n");

	return 0;

bbt_err:
	free(data_buf);
	printf("read bbt err, get markbad from nand!\n");
	return -1;
}

int init_nand(void) {
	int ret;

        rrtb_buf = rballoc("nandrrtb", 0x8000);
	printf("nand init start\n");
	memset((void *)&nand_cfg, 0, sizeof(struct nand_cfg));
	if(boot_device() == DEV_BND) {
		ret = nand_rescan_config(infotm_nand_idt, infotm_nand_count());
		if(ret) {
			printf("init NANDFLASH failed.\n");
			return ret;
		} else
		  nand_init_done = 1;
	}
	printf("bbt check start\n");
	ret = bbt_check();
	if (ret == 0)
		bbt_flag = 1;
	else 
		bbt_flag = 0;
	
	return 0;
}
