#include <common.h>
#include <command.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <div64.h>
#include <linux/mtd/compat.h>
#include <bootlist.h>
#include <tt.h>
#include <asm-generic/errno.h>

#define tt_msg(x...) printf("ttnd: " x)
#define REDUNDANT_TEST
#define TROLLING_MAX	0x00800000
#define TROLLING_MIN	0x00100000
#define TROLLING_SIZE	0x10000000
#define TROLLING_ERRCOUNT	12
#define TROLLING_ON_BLOCK

enum troll_type {
	TROLL_BND = 0,
	TROLL_NND,
	TROLL_FND,
};

static uint64_t all;
loff_t be[32], bec = 0; // block exclude
#if 0
int tt_nand_cross_cmp(uint8_t *a, uint8_t *b, int len,  int type,
			struct nand_config *nc)
{
	int step_a = nc->pagesize + (type?0:nc->sysinfosize);
	int step_b = nc->pagesize + (type?nc->sysinfosize:0);
	int ret;

	for(ret = 0;;) {

		if(len <= 0)
		  break;

		ret += tt_cmp_buffer(a, b, nc->pagesize, 0);
		a += step_a;
		b += step_b;
		len -= step_b;
	}

	return ret;
}

int tt_nand_trolling(int type, struct nand_config *bc,
			struct nand_config *nc) {
	int len, ret, ws, rs = 0, cnt, i;
	uint8_t *data, *cmpbuf = (uint8_t *)TT_CMP_BUFF;
	loff_t offset;
	uint32_t t;
	char *name[4] = {"bnd", "nnd", "fnd"};

#ifdef TROLLING_ON_BLOCK
	/* get random data */
	cnt = tt_rnd_get_int(1, 100);

	/* there are 50% to do full block testing
	 * and 50% to do a random page testing */
	if(cnt > 50) cnt = nc->pagesperblock;
	else cnt = tt_rnd_get_int(1, nc->pagesperblock);
#else
	/* get random data */
	cnt  = tt_rnd_get_int(0, TROLLING_MAX / nc->pagesize);
#endif

	if(type == TROLL_FND)
		ret = cnt * (nc->pagesize + nc->sysinfosize);
	else
		ret = cnt * (nc->pagesize);

	data = tt_rnd_get_data(&len, ret, ret);

	if(!data) {
		tt_msg("get random data failed.\n");
		return -1;
	}

#ifdef TROLLING_ON_BLOCK
	ret = tt_rnd_get_int(0, nc->blockcount);
	ret = ret * nc->pagesperblock + 
		tt_rnd_get_int(0, nc->pagesperblock - cnt);
#else
	/* get a random page */
	ret = tt_rnd_get_int(0, nc->blockcount * nc->pagesperblock - cnt);
#endif

	if(type == TROLL_BND)
		offset = (uint64_t)ret * (uint64_t)bc->pagesize;
	else
		offset = (uint64_t)ret * (uint64_t)nc->pagesize;

	for(i = 0; i < bec; i++) {
	    if((offset & ~(nc->blocksize - 1))
		    == be[i]) {
		tt_msg("case sekip because of block exclude\n");
		return -EAGAIN;
	    }
	}

	ret = vs_assign_by_name(name[type], 1);
	if(ret) {
		tt_msg("assign dev %s failed.\n", name[type]);
		return -1;
	}

	tt_msg("trolling on %s: mem=0x%p (0x%x @ 0x%llx)\n",
			name[type], data, len, offset);

	if(vs_isbad(offset)) {
		tt_msg("case skip because off bad block.\n");
		return -EAGAIN;
	}

	/* erase nand device */
	vs_erase(offset, len);

	/* write data */
	t = get_timer(0);
	vs_write(data, offset, len, 0);
	t = get_timer(t);
	ws = (len >> 10) * 1000 / t;

	/* read data:
	 * if test case is TROLL_NND or TROLL_FND, we
	 * should read the test area with both "nnd" and "fnd"
	 */
	t = get_timer(0);
	vs_read(cmpbuf, offset, len, 0);
	t = get_timer(t);
	rs = (len >> 10) * 1000 / t;

	/* cmp data */
	ret = tt_cmp_buffer(data, cmpbuf, len, TROLLING_ERRCOUNT);
	if(ret) {
	  tt_msg("[%s]: FAILED, Vw=%d KB/s, Vr=%d KB/s, errbytes %d\n",
				  name[type], ws, rs, ret);
	  be[bec++] = (offset & ~(nc->blocksize - 1));
	  if(bec >= 32) {
	      tt_msg("error happened more than 32 times, test stoped\n");
	      while(1);
	  }
	}
	else
	  tt_msg("[%s]: PASSED: Vw=%d KB/s, Vr=%d KB/s\n",
				  name[type], ws, rs);

	all += len;
	if(type != TROLL_BND) {
		/* switch type */
		type = 3 - type;

		ret = vs_assign_by_name(name[type], 1);
		if(ret) {
			tt_msg("assign dev %s failed.\n", name[type]);
			return -1;
		}

		len = cnt * (nc->pagesize + ((type == 1)?0:nc->sysinfosize));
		t = get_timer(0);
		vs_read(cmpbuf, offset, len, 0);
		t = get_timer(t);
		rs = (len >> 10) * 1000 / t;

		ret = tt_nand_cross_cmp(data, cmpbuf, len, (type == 2), nc);
		if(ret)
			tt_msg("[%s]: FAILED: Vr=%d KB/s, errbytes %d\n", name[type], rs, ret);
		else
			tt_msg("[%s]: PASSED: Vr=%d KB/s\n", name[type], rs);
	}

	printf("\n");

	return 0;
}

int do_tt_nand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int i, xom, ret;
	struct nand_config *nc, *bc;

	if(argc < 2) {
		tt_msg("please provide at least one bnd configuration.\n");
		return -1;
	}

	for(i = 1; i < argc; i++) {
		xom = simple_strtoul(argv[i], NULL, 10);
		set_xom(xom);
		nand_rescan_config(NULL, 0);

		bc = nand_get_config(NAND_CFG_BOOT);
		nc = nand_get_config(NAND_CFG_NORMAL);

		if(!bc || !nc) {
			tt_msg("can not get nand configurations correctly.\n");
			return -2;
		}

		tt_msg("bnd case: %d\n", xom);

		do {
			ret = tt_nand_trolling(TROLL_BND, bc, nc);
		} while (ret == -EAGAIN);

#ifndef REDUNDANT_TEST
		if(i == 1) {
#endif
			do {
				ret = tt_nand_trolling(TROLL_NND, bc, nc);
			} while (ret == -EAGAIN);

			do {
				tt_nand_trolling(TROLL_FND, bc, nc);
			} while (ret == -EAGAIN);
#ifndef REDUNDANT_TEST
		}
#endif
	}

    return 0;
}

U_BOOT_CMD(ttnd, CONFIG_SYS_MAXARGS, 1, do_tt_nand,
	"ttnd\n",
	"info - show available command\n"
	);


int do_tt_ndloop(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int i, t, ret;
	struct nand_config *nc;

	be[0] = 0;
	bec = 1;

	nc = nand_get_config(NAND_CFG_NORMAL);

	if(!nc) {
	    tt_msg("can not get nand configurations correctly.\n");
	    return -2;
	}

	t = get_timer(0);
	for(i = 0, all = 0;; i++) {
		printf("loop: %d\n", i);
		ret = tt_nand_trolling(TROLL_NND, NULL, nc);
		if(ret == 0)
		  tt_nand_trolling(TROLL_FND, NULL, nc);
		if(tstc() && (getc() == 0x3)) break;
	}
	t = get_timer(t);

	printf("%d MB data tested in %d minutes.\n", (int)(all >> 20), t / 60000);

    return 0;
}

U_BOOT_CMD(ttndloop, CONFIG_SYS_MAXARGS, 1, do_tt_ndloop,
	"ttndloop\n",
	"info - show available command\n"
	);



int do_tt_sn(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
    int c = 0, id, len;
    char cmd[64];

    if(argc < 2)
	printf("ttsn device\n");

    c = vs_assign_by_name(argv[1], 1);
    if(c) return -1;

    if(strncmp(argv[1], "fnd", 3) == 0)
	len = fnd_align() * 1024;
    else
	len = 0x400000;

    id = vs_device_id(argv[1]);
    if(argc > 2)
	len = simple_strtoul(argv[2], NULL, 10) * vs_align(id);
    printf("\ntest device: %s, align=0x%x\n", argv[1], vs_align(id));

    for(c = 0;;) {
	printf("loop: %d\n", c++);
	if(vs_device_erasable(id)) {
	    printf("erasing ...\n");
	    sprintf(cmd, "vs erase 600000 %x\n", len);
	    run_command(cmd, 0);
	    if(tstc() && (getc() == 0x3)) break;
	}

	printf("writing ...\n");
	sprintf(cmd, "vs write 40008000 600000 %x\n", len);
	run_command(cmd, 0);
	if(tstc() && (getc() == 0x3)) break;

	printf("reading ...\n");
	sprintf(cmd, "vs read 50008000 600000 %x\n", len);
	run_command(cmd, 0);
	if(tstc() && (getc() == 0x3)) break;

	printf("comparing ...\n");
	tt_cmp_buffer((uint8_t *)0x40008000, (uint8_t *)0x50008000,
		len, TROLLING_ERRCOUNT);
	printf("\n");
	if(tstc() && (getc() == 0x3)) break;
    }

    return 0;
}

U_BOOT_CMD(ttsn, CONFIG_SYS_MAXARGS, 1, do_tt_sn,
	"ttsn\n",
	"info - show available command\n"
	);


int do_tt_sysinfo(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
    int c = 0, len, sysinfo;
    char cmd[64];
    struct nand_config *nc;

    if(argc < 2)
	printf("ttsysinfo sysinfosize\n");

    nc = nand_get_config(NAND_CFG_NORMAL);
    if(!nc) {
	    printf("get normal config failed.\n");
	    return -1;
    }

    c = vs_assign_by_name("fnd", 1);
    if(c) return -1;


    sysinfo = simple_strtoul(argv[1], NULL, 10);

    nc->sysinfosize = sysinfo;
    len = fnd_align() * 512;
    printf("\nsysinfosize: 0x%x, length=0x%x\n", sysinfo, len);

    for(c = 0;;) {
	printf("loop: %d\n", c++);
	printf("erasing ...\n");
	sprintf(cmd, "vs erase 600000 %x\n", len);
	run_command(cmd, 0);
	if(tstc() && (getc() == 0x3)) break;

	printf("writing ...\n");
	sprintf(cmd, "vs write 40008000 600000 %x\n", len);
	run_command(cmd, 0);
	if(tstc() && (getc() == 0x3)) break;

	printf("reading ...\n");
	sprintf(cmd, "vs read 50008000 600000 %x\n", len);
	run_command(cmd, 0);
	if(tstc() && (getc() == 0x3)) break;

	printf("comparing ...\n");
	tt_cmp_buffer((uint8_t *)0x40008000, (uint8_t *)0x50008000,
		len, TROLLING_ERRCOUNT);
	printf("\n");
	if(tstc() && (getc() == 0x3)) break;
    }

    return 0;
}

U_BOOT_CMD(ttsysinfo, CONFIG_SYS_MAXARGS, 1, do_tt_sysinfo,
	"ttsysinfo\n",
	"info - show available command\n"
	);


int tt_simple_test(const char *dev, int read)
{
    int c = 0, id, len;
    char cmd[64];

    c = vs_assign_by_name(dev, 1);
    if(c) return -1;

    if(strncmp(dev, "fnd", 3) == 0)
	  len = fnd_align() * 256;
    else
	  len = vs_align(DEV_CURRENT) * 256;

    id = vs_device_id(dev);
	printf("test device: %s, align=0x%x\n", dev, vs_align(id));

	if(!read) {
		if(vs_device_erasable(id)) {
			printf("erasing ...\n");
			sprintf(cmd, "vs erase 3000000 %x\n", len);
			run_command(cmd, 0);
		}

		printf("writing ...\n");
		sprintf(cmd, "vs write 40008000 3000000 %x\n", len);
		run_command(cmd, 0);
	} else {
		printf("reading ...\n");
		sprintf(cmd, "vs read 50008000 3000000 %x\n", len);
		run_command(cmd, 0);

		printf("comparing ...\n");
		tt_cmp_buffer((uint8_t *)0x40008000, (uint8_t *)0x50008000,
					len, 3);
		printf("\n");
	}

    return 0;
}

#if 0
int do_tt_randomizer(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int i, j, k, bp, t;
	extern uint32_t pp_list[];
	struct nand_config *cfg;
	uint32_t seed[5][4] = {
		{0x1c723550, 0x20fe074e, 0x1698347e, 0x2fff2c80},
		{1, 2, 3, 4},
		{0x22222222, 0x33333333, 0x11111111, 0x00220033},
		{0x23452345, 0x16750e9d, 0x2f3c3999, 0x20ff32a8},
		{0, 0, 0, 0},
	};

	if(argc < 2) {
		printf("ttrandomizer device [boot pin]\n");
		return -1;
	}

	if(strncmp(argv[1], "bnd", 3) == 0) {
		if(argc < 3) {
			printf("no boot pin provided.\n");
			return -1;
		}

		bp = simple_strtoul(argv[2], NULL, 16);
		set_xom(bp);
		nand_rescan_config(NULL, 0);

		cfg = nand_get_config(NAND_CFG_BOOT);
	} else
	  cfg = nand_get_config(NAND_CFG_NORMAL);

	if(!cfg) {
		printf("nand configuration is not availible\n");
		return -1;
	}

	for(i = 0; i < 5; i++) {

		/* set poly */
		cfg->polynomial = pp_list[i];
		tt_msg("\n\nset poly: 0x%x\n", cfg->polynomial);

		for(j = 0; j < 4; j++) {
			
			/* set seed */
			for(k = 0; k < 4; k++) {
				cfg->seed[k] = seed[j][k];
				tt_msg("set seed[%d]: 0x%x\n", k, cfg->seed[k]);
			}
			
			/* enable rndmizer */
			cfg->randomizer = 1;
			t = get_timer(0);
			nand_init_randomizer(cfg);
			t = get_timer(t);
			tt_msg("init randomizer cost: %d ms\n", t);

			tt_simple_test(argv[1], 0);

			/* test 1: */
			cfg->randomizer = 0;
			tt_msg("test 1: disable rdmz, expect FAIL\n");
			tt_simple_test(argv[1], 1);

			/* test 2: */
			cfg->randomizer = 1;
			tt_msg("test 2: enable rdmz, expect PASS\n");
			tt_simple_test(argv[1], 1);

			/* test 3: */
			tt_msg("test 3: change seed, expect FAIL\n");
			for(k = 0; k < 4; k++) {
				cfg->seed[k] = seed[(j + 1) % 5][k];
//				tt_msg("set seed[%d]: 0x%x\n", k, cfg->seed[k]);
			}
			nand_init_randomizer(cfg);
			tt_simple_test(argv[1], 1);

			/* test 4: */
			tt_msg("test 4: change seed back, expect PASS\n");
			for(k = 0; k < 4; k++) {
				cfg->seed[k] = seed[j][k];
//				tt_msg("set seed[%d]: 0x%x\n", k, cfg->seed[k]);
			}
			nand_init_randomizer(cfg);
			tt_simple_test(argv[1], 1);

			/* test 5: */
			tt_msg("test 5: change poly, expect FAIL\n");
			cfg->polynomial = pp_list[(i + 1) % 10];
			nand_init_randomizer(cfg);
			tt_simple_test(argv[1], 1);

			/* test 6: */
			tt_msg("test 6: change poly back, expect PASS\n");
			cfg->polynomial = pp_list[i];
			nand_init_randomizer(cfg);
			tt_simple_test(argv[1], 1);
		} /* seed loop */
	} /* poly loop */

	return 0;
}

U_BOOT_CMD(ttrandomizer, CONFIG_SYS_MAXARGS, 1, do_tt_randomizer,
	"ttrandomizer\n",
	"info - show available command\n"
	);
#endif

int do_tt_nbi(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int x = get_xom(1), i, el[] = {2, 4, 8, 16, 24, 32, 40,
		48, 56, 60, 64, 72, 80, 96, 112, 127};

	for(i = 10; i < 28; i++) {
		struct nand_config *cfg;
		set_xom(i);
		nand_rescan_bootcfg();
		cfg = nand_get_config(NAND_CFG_BOOT);
		printf("%d: %d, %x, %d, %d, %s, %d\n", i,
					cfg->busw, cfg->pagesize, cfg->eccen? el[cfg->mecclvl]: 0,
					cfg->cycle, (cfg->interface == 0)? "legacy":
					(cfg->interface == 1) ? "sync": "toggle",
					cfg->randomizer);
	}

	/* restore xom */
	set_xom(x);
	return 0;
}

U_BOOT_CMD(ttnbi, CONFIG_SYS_MAXARGS, 1, do_tt_nbi,
	"ttnbi\n",
	"info - show available command\n"
	);
#endif


