/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <bootlist.h>
#include <malloc.h>
#include <cdbg.h>
#include <flash.h>
#include <burn.h>
#include <vstorage.h>
#include <ius.h>
#include <efuse.h>
#include <lowlevel_api.h>
#include <bootlist.h>
#include <rballoc.h>
#include <battery.h>
#include <pmu_command.h>

int do_pmu(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int vol;
	uint8_t reg, buf;

	if (argc < 2 || argc > 4) {
		pmu_help_info();
		return -1;
	}

	if (!strncmp(argv[1], "set", 3)) {
		if (argc < 4) {
			printf("Please specify the voltage, uint is mv\n");
			return -1;
		}

		vol = simple_strtoul(argv[3], NULL, 10);

		pmu_set_vol(argv[2], vol);
	} else if (!strncmp(argv[1], "get", 3)) {
		if (argc < 3) {
			printf("Please specify the regulator name\n");
			return -1;
		}

		vol = pmu_get_vol(argv[2]);
		if (vol != -1)
			printf("%dmv\n", vol);
	} else if (!strncmp(argv[1], "enable", 6)) {
		if (argc < 3) {
			printf("Please specify the regulator name\n");
			return -1;
		}

		pmu_enable(argv[2]);
	} else if (!strncmp(argv[1], "disable", 7)) {
		if (argc < 3) {
			printf("Please specify the regulator name\n");
			return -1;
		}

		pmu_disable(argv[2]);
	} else if (!strncmp(argv[1], "status", 6)) {
		if (argc < 3) {
			printf("Please specify the regulator name\n");
			return -1;
		}

		vol = pmu_is_enabled(argv[2]);
		printf("%s\n", (vol==0)?"Off":(vol>0?"On":"none"));
	} else if (!strncmp(argv[1], "help", 4)) {
		pmu_help_info();
	} else if (!strncmp(argv[1], "cw", 2)) {
		reg = simple_strtoul(argv[2], NULL, 16);
		buf = simple_strtoul(argv[3], NULL, 16);
		pmu_write(reg, buf);
	} else if (!strncmp(argv[1], "cr", 2)) {
		reg = simple_strtoul(argv[2], NULL, 16);
		vol = pmu_read(reg);
		printf("Reg[0x%x]=[0x%x]\n", reg, vol);
	} else {
		pmu_help_info();
	}

    return 0;
}

U_BOOT_CMD(
	pmu, 4, 1, do_pmu,
	"pmu",
	"");

int do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int index;

	if (argc < 2 || argc > 4) {
		gpio_help();
		return -1;
	}

	if (!strcmp(argv[1], "func")) {
		index = simple_strtoul(argv[2], NULL, 10);
		gpio_mode_set(0, index);
	} else if (!strcmp(argv[1], "input")) {
		index = simple_strtoul(argv[2], NULL, 10);
		gpio_mode_set(1, index);
		gpio_dir_set(0, index);
	} else if (!strcmp(argv[1], "high")) {
		index = simple_strtoul(argv[2], NULL, 10);
		gpio_mode_set(1, index);
		gpio_dir_set(1, index);
		gpio_output_set(1, index);
	} else if (!strcmp(argv[1], "low")) {
		index = simple_strtoul(argv[2], NULL, 10);
		gpio_mode_set(1, index);
		gpio_dir_set(1, index);
		gpio_output_set(0, index);
	} else if (!strcmp(argv[1], "pull")) {
		index = simple_strtoul(argv[2], NULL, 10);
		if (!strcmp(argv[3], "enable"))
			gpio_pull_en(1, index);
		else if (!strcmp(argv[3], "disable"))
			gpio_pull_en(0, index);
		else
			gpio_help();
	} else if (!strcmp(argv[1], "stat")) {
		int mode, dir, val, pull;

		index = simple_strtoul(argv[2], NULL, 10);
		gpio_status(index, &mode, &dir, &val, &pull);
	} else
		gpio_help();

	return 0;
}

U_BOOT_CMD(
	gpio, 4, 1, do_gpio,
	"gpio",
	"");

int do_bc(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	batt_calc();
	return 0;
}

U_BOOT_CMD(
	bc, 1, 1, do_bc,
	"bc",
	"");

int do_ls(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("%s\n", __irom_ver);
	return 0;
}

U_BOOT_CMD(
	ls, 1, 1, do_ls,
	"ls",
	"");

int do_rblist(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    rblist();
    return 0;
}

U_BOOT_CMD(
	rblist, 1, 1, do_rblist,
	"rblist",
	"");

#if 0
int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	writel(0x3, RTC_SYSM_ADDR);
	return 0;
}

U_BOOT_CMD(
	reset, 1, 1, do_reset,
	"ls",
	"");
#endif

int do_setboottype(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint16_t info = 0;

	if(argc > 1) {
	  info = simple_strtoul(argv[1], NULL, 16);
	}

        set_boot_type(info);
	return 0;
}

U_BOOT_CMD(
	boottype, 2, 1, do_setboottype,
	"set boot type",
	"");


int do_bp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint16_t info;

	if(argc > 1) {
	  info = simple_strtoul(argv[1], NULL, 16);
	  set_xom(info);
	  init_mux(0);
	}

	printf("xom state (hw): 0x%x\n", get_xom(0));
	printf("xom state (sw): 0x%x\n", get_xom(1));
	printf("16-bit information: 0x%x\n", boot_info());


	return 0;
}

U_BOOT_CMD(
	bp, 2, 1, do_bp,
	"show xom info",
	"");

#if 0
int do_ml(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	__sOINFo();
	return 0;
}

U_BOOT_CMD(
	ml, 1, 1, do_ml,
	"display malloc layout",
	"");

int do_go(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	cdbg_jump((void *)simple_strtoul(argv[1], NULL, 16));
	return 0;
}

U_BOOT_CMD(
	go, 2, 1, do_go,
	"set PC to address",
	"");

#if 0
int do_udc(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if(strncmp(argv[1], "read", 4) == 0) {
		printf("udc read.\n");
		udc_read((uint8_t *)0x40007fc0, 0x300000);
	}
	else if(strncmp(argv[1], "write", 5) == 0) {
		printf("udc write.\n");
		udc_write((uint8_t *)0x40007fc0, 0x300000);
	} else 
	  udc_connect();

	return 0;
}

U_BOOT_CMD(
	udc, 2, 1, do_udc,
	"set PC to address",
	"");
#endif

int do_sleep(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned long sec = simple_strtoul(argv[1], NULL, 10);
	udelay(sec * 1000000);
	printf("sleep OK.\n");
	return 0;
}

U_BOOT_CMD(
	sleep, 2, 1, do_sleep,
	"sleep",
	"");

int do_cfg(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32_t addr = simple_strtoul(argv[1], NULL, 16);
	cdbg_config_isi((struct cdbg_cfg_table *)addr);
	return 0;
}

U_BOOT_CMD(
	cfg, 2, 1, do_cfg,
	"read and apply configuration table",
	"");
#endif

int do_spbyte(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int bytes = simple_strtoul(argv[1], NULL, 10);

	spi_set_bytes(bytes);
	return 0;
}

U_BOOT_CMD(
	spbyte, 2, 1, do_spbyte,
	"set spi flash bytes.(decimo)",
	"");

/* program an .ius image */
int do_pi(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret;
	struct ius_hdr *ius = (struct ius_hdr *)(DRAM_BASE_PA
				+ IUS_DEFAULT_LOCATION);

	if(argc > 1) {
		if(strncmp(argv[1], "scan", 4) == 0) {
			uint32_t mask = 0;

			if(argc > 2)
			  mask = simple_strtoul(argv[2], NULL, 16);

			printf("scan ius in every device (mask: 0x%x)...\n", mask);
			burn(mask); /* do not mask any device */
		}

		return 0;
	}

	ret = ius_check_header(ius);
	if(ret) {
		printf("verify IUS package failed (%d)\n", ret);
		return ret;
	}

	ret = vs_assign_by_id(DEV_RAM, 1);
	if(ret) {
		printf("reset ramdisk failed (%d)\n", ret);
		return ret;
	}

	burn_images(ius, DEV_RAM, 0);
	return 0;
}

U_BOOT_CMD(
	pi, 3, 1, do_pi,
	"program images contain in a .ius image to their relavant devices",
	"");

#if 0
int do_efuse(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	efuse_info(argc > 1 && strncmp(argv[1], "-h", 2) == 0);
	return 0;
}

U_BOOT_CMD(
	efuse, 2, 1, do_efuse,
	"print e-fuse information",
	"");

int do_boot(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	try_boot();
	return 0;
}

U_BOOT_CMD(
	boot, 2, 1, do_boot,
	"enter the boot process",
	"");

int do_mac(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("mac address: 0x%012llx\n", efuse_mac());
	return 0;
}

U_BOOT_CMD(
	mac, 2, 1, do_mac,
	"show mac address",
	"");

int do_mux(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("mux: 0x%08x\n", readl(PAD_SYSM_ADDR)); 
	return 0;
}

U_BOOT_CMD(
	mux, 2, 1, do_mux,
	"show mux info",
	"");

int do_pads(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int index;
	if(argc < 2) return 0;

	index = simple_strtoul(argv[1], NULL, 10);
	printf("pads(%d) states: %d\n", index,
			pads_states(index));
	return 0;
}

U_BOOT_CMD(
	pads, 2, 1, do_pads,
	"show pads info",
	"");

#endif

#define FR_NUM 15
extern void __ddf(void *, void *, uint32_t, uint32_t);
const uint32_t fr_val[FR_NUM] = {
	600, 576, 552, 528, 504,
	468, 444, 420, 396, 372,
	348, 330, 312, 288, 264 
};
const uint32_t para_val[FR_NUM] = {
	0x0031, 0x002f, 0x002d, 0x002b, 0x0029,
	0x104d, 0x1049, 0x1045, 0x1041, 0x103d,
	0x1039, 0x1036, 0x1033, 0x102f, 0x102b
};

int do_ddf(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	void (*ddf)(void *, void *, uint32_t, uint32_t) = (void *)0x3c000000;
	int f, i, p = 0;

	if(argc < 2)  {
		printf("ddf frequencyM\n");
		return 0;
	}

	f = simple_strtoul(argv[1], NULL, 10);

	for(i = 0; i < FR_NUM; i++) {
		if(f >= fr_val[i] || i == (FR_NUM - 1)) {
			f = fr_val[i];
			p = para_val[i];
			break;
		}
	}

	printf("change DDR to %dM (0x%04x)\n", f, p);
	memcpy(ddf, __ddf, 0x8000);
	ddf((void *)EMIF_BASE_ADDR, (void *)CLK_SYSM_ADDR + 3 * 0x18, f / 10, p);
	return 0;
}

U_BOOT_CMD(
	ddf, 2, 1, do_ddf,
	"dynamic DDR frequency",
	"");

