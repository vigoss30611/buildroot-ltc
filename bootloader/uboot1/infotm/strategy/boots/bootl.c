/***********************************************************************************
File Name	:	bootl.c
Description	:	This file will supply interfaces needed when booting a linux system.
Author		:	John Zeng
Data		:	Apr 26  2012
Version		:	init
************************************************************************************/


#include <common.h>
#include <linux/types.h>
#include <malloc.h>
#include <bootl.h>
#include <storage.h>
#include <vstorage.h>
#include <asm/io.h>
#include <bootlist.h>
#include <items.h>
#include <isi.h>
#include <ius.h>
#include <rballoc.h>
#include <rtcbits.h>
#include <board.h>
#include <asm/cache.h>
#include <asm/arch/nand.h>
#include <asm/sizes.h>

static int __boottype = 0;

/*(1)Author:summer
 *(2)Date:2014-6-23
 *(3)Reason:
 * Boot paths:
 *			 (a)boot to normal:		uboot->kernel->filesystem : __boot_to_where=0
 *			 (b)boot to charger:	uboot->kernel->charger    : __boot_to_where=1
 *			 (c)boot to ius-burn:   uboot->kernel->recovery   : __boot_to_where=2
 * */
int __boot_to_where = 0;

int set_boot_type(int type)
{
    return __boottype = type;
}
int got_boot_type(void)
{
    return __boottype;
}

uint8_t *load_image(char *name , char* alternate, char* rambase, int src_devid)
{
	int tryed = 0, l = 0;
	char* imagename = name;
	uint8_t* dst = NULL; 
	int t =0, ret;
	loff_t image =0;
retry:

	image = storage_offset(imagename);
	
	printf("fetch %s@0x%llx ...\n",imagename , image);

	t = get_timer(0);

	/* Load the first 4K */
	if ((src_devid == DEV_NAND) || (src_devid == DEV_BND))
		src_devid = DEV_FND;
	vs_assign_by_id(src_devid, 1);
	ret = vs_read((uint8_t *)rambase, image, 0x1000, 0);
	if(0x1000 > ret)
	{
		printf("read header failed 0x%x\n",ret);
		goto failed;
	}

	image_header_t *uhdr = (image_header_t *)rambase;
	struct ius_hdr *ihdr = (struct ius_hdr *)rambase;
        dst = (uint8_t *)rambase;

	/* Check Magic */
	if(image_check_hcrc(uhdr)) {
            l = image_get_size(uhdr) + sizeof(image_header_t);
            if(image_get_comp(uhdr) == 1){
                dst += 0x6000000;
                cfg_mmu_table();
            }
        }
        else if(ius_check_header(ihdr) == 0)
            l = ius_count_size(ihdr);
        else
            goto failed;

        ret = vs_read( dst , image , l, 0);
        if(l > ret){
            printf("failed 0x%x 0x%x\n",ret , l);
            goto failed;
        }
        printf("%dms\n",(int)get_timer(t));
	return dst;

failed:
        printf("Wrong Image type!\n");
	if(tryed) return NULL;
	tryed = 1;
	if(NULL == alternate) return NULL;
	imagename = alternate;
	goto retry;
}

int calc_dram_M(void)
{
	int cs = 1, ch = 8, ca = 1, rd = 0;
	int sz;

	if((sz = rbgetint("dramsize")) != 0)
		return sz;

	if(item_exist("memory.size")) {
		sz = item_integer("memory.size", 0);
		printf("Size: %lld MB (memory.size)\n", sz);
		sz <<= 20;
		goto __ok;
	}

	if(item_exist("memory.cscount")
			&& item_exist("memory.density")
			&& item_exist("memory.io_width")
			&& item_exist("memory.reduce_en")
	  ) {
		cs = item_integer("memory.cscount", 0);
		ch = item_integer("memory.io_width", 0);
		rd = item_integer("memory.reduce_en", 0);
		ca = item_integer("memory.density", 0);

        cs = (cs == 3)?2: 1;
	}

	/* calculation */
	sz = (ca > 20)?
		(ca << 17): /* MB */
		(ca << 27); /* GB */
	sz *= (ch == 8)? 4: 2;
	sz >>= !!rd;
	sz *= cs;

	printf("Size: %lld MB (cs|%d bw|%d ca|%d rd|%d)\n", sz >> 20,
			cs, ch, ca, rd);

__ok:
	return (int)((sz >> 20) - 1);
}

int do_boot(void* kernel , void* ramdisk)
{
	char spi_parts[64], fs_type[16];
	char args[1024], buf[64];
	memset(args,0,1024);
	int mm, boot_id;

	mm = calc_dram_M();

	boot_id = storage_device();
	switch(got_boot_type())
	{
		case BOOT_NORMAL:
			if (boot_id == DEV_FLASH) {
				sprintf(spi_parts, "part%d", IMAGE_SYSTEM);
				if (item_exist(spi_parts)) {
					if (!item_string(fs_type, spi_parts, 3))
						sprintf(args,"%s rootfstype=%s root=/dev/spiblock1 rw serialno=%s", CONFIG_LINUX_DEFAULT_BOOTARGS, fs_type, getenv("serialex"));
					else
						sprintf(args,"%s rootfstype=%s root=/dev/spiblock1 rw serialno=%s", CONFIG_LINUX_DEFAULT_BOOTARGS, "squashfs", getenv("serialex"));
				} else {
					sprintf(args,"%s rootfstype=%s root=/dev/spiblock1 rw serialno=%s", CONFIG_LINUX_DEFAULT_BOOTARGS, "squashfs", getenv("serialex"));
				}
			} else if (boot_id == DEV_MMC(0)) {
				sprintf(args,"%s rootfstype=ext4 root=/dev/mmcblk0p1 rw serialno=%s", CONFIG_LINUX_DEFAULT_BOOTARGS, getenv("serialex"));
			} else {
				if (vs_assign_by_id(DEV_MMC(0), 1) == 0)
					sprintf(args,"%s rootfstype=ext4 root=/dev/mmcblk1p2 rw serialno=%s", CONFIG_LINUX_DEFAULT_BOOTARGS, getenv("serialex"));
				else
					sprintf(args,"%s rootfstype=ext4 root=/dev/mmcblk0p2 rw serialno=%s", CONFIG_LINUX_DEFAULT_BOOTARGS, getenv("serialex"));
			}
			break;
		case BOOT_FACTORY_INIT:
		case BOOT_RECOVERY_MMC:
		case BOOT_RECOVERY_IUS:
			sprintf(args,"%s mode=ius cma=32M serialno=%s", CONFIG_LINUX_DEFAULT_BOOTARGS, getenv("serialex"));
			break;
		case BOOT_CHARGER:
			sprintf(args,"%s androidboot.mode=charger androidboot.serialno=%s",
                                        CONFIG_LINUX_DEFAULT_BOOTARGS, getenv("serialex"));
			break;
	}

#if 0
	if(item_exist("board.hwid")) {
		strncat(args, " androidboot.hardware=", 64);
		item_string(buf, "board.hwid", 0);
		strncat(args, buf, 64);
	}
#endif

	char cmd[128];
	//sprintf(cmd,"fatload %s %d:1 %x %s", "mmc", 1, 0x87c00000, "irom.bin");
	//run_command(cmd, 0);
	//run_command("go 0x87c00000", 0);
	setenv("bootargs", args);
	memset(cmd,0,64);

	if(ramdisk)
		sprintf(cmd,"bootm %x %x",(unsigned int)kernel ,(unsigned int)ramdisk);
	else
		sprintf(cmd,"bootm %x",(unsigned int)kernel);
	
	run_command(cmd,0);

	/*
	 * hope never reach here.
	 *   
	 */
	printf("Failed to recover system since there is something wrong when bootting recovery, system halted..\n");
	for(;;);
	return 0; //Make compiler happy.
}

int boot_verify_type(void)
{
	int pwr_st = 0;
        if (board_bootst() == BOOT_ST_RECOVERY){
            set_boot_type(BOOT_FACTORY_INIT);
			__boot_to_where = 2;
		}else if ((item_equal("power.acboot", "1", 0)
                    && board_acon()
                    && board_bootst() == BOOT_ST_NORMAL)
                || board_bootst() == BOOT_ST_CHARGER) {
            set_boot_type(BOOT_CHARGER);
			__boot_to_where = 1;
        }

        if (board_acon()) {
            pwr_st = readl(SYS_POWUP_ST);
            writel(0x2, SYS_POWUP_CLR);
        }
	
        if(readl(RTC_SYSM_ADDR + 0x60) & 0x2) {
            if(board_bootst() == BOOT_ST_NORMAL)
                rbsetint("forceshut", 1);
        } else {
            writel(readl(RTC_SYSM_ADDR + 0x60) | 0x2,
                    (RTC_SYSM_ADDR + 0x60));
        }

	writel(0xff, RTC_SYSM_ADDR + 0x04);

	return pwr_st;
}

static int load_file(char *filename, unsigned int addr, int bootdev)
{
	char cmd[64];
	vs_assign_by_id(bootdev, 0);
	sprintf(cmd,"fatload %s %d:1 %x %s", "mmc", 1, addr, filename);
	printf("%s\n",cmd);
	if(!run_command(cmd,0)){
		return 0;
	}
	return 1;
}

static int ius_check_head(struct ius_hdr *hdr)
{
	uint32_t crc_ori, crc_calc;

	if(hdr->magic != IUS_MAGIC) {
		printf("magic do not match1. 0x%x\n", hdr->magic);
		return -1;
	}

	printf("header check passed.\n");
	return 0;
}

static int load_ius(char *part_name, char *addr, int bootdev)
{
    int ret = 0, count, type, i;
	struct ius_hdr *ius;
	struct ius_desc *desc;
	uint32_t max;
	loff_t	offs_i, offs_o;

	ius = malloc(sizeof(struct ius_hdr));
	if(!ius) {
		printf("malloc ius header space failed.\n");
		return -1;
	}

    vs_assign_by_id(DEV_MMC(1), 1);
    ret = vs_read((uint8_t *)ius, IUS_DEFAULT_LOCATION, sizeof(struct ius_hdr), 0);
    if(ius_check_head(ius) == 0) {
		ret = -1;
        count = ius_get_count(ius);
        for(i = 0; i < count; i++) {
            desc = ius_get_desc(ius, i);
            max  = ius_desc_maxsize(desc);
            type  = ius_desc_offo(desc);

            if (type == storage_number(part_name)) {
                offs_i = ius_desc_offi(desc);
                vs_read((uint8_t *)addr, offs_i, ius_desc_length(desc), 0);
                ret = 0;
            } else if (!strcmp(part_name, "ramdisk")) {
            	if (type == IMAGE_RAMDISK) {
	                offs_i = ius_desc_offi(desc);
	                vs_read((uint8_t *)addr, offs_i, ius_desc_length(desc), 0);
	                ret = 0;
            	}
            }
        }
    }

    free(ius);
    return ret;
}

int bootl(void)
{
	int boottype=got_boot_type();
	if(BOOT_HALT_SYSTEM==boottype){
		printf("Erro: something erro happened, system halted\n");
		for(;;);
	}
	int bootdev=storage_device();
	if(!vs_device_burndst(bootdev)){
		printf("Erro: Invalid system disk specified, it not a burndst\n");
		return -1;
	}

	if(boottype == BOOT_RECOVERY_IUS)
		bootdev=DEV_MMC(1);

	int ret = vs_assign_by_id(bootdev, 1);
	if(ret){
		printf("Erro: Can not get the system disk ready\n");
		return -1;
	}
	char *ramdisk = NULL;
	char * alternatedisk = NULL;
	char *kernel = NULL;
    char *alternatekernel = NULL;
#if defined(MODE) && (MODE == SIMPLE)
	ramdisk = "ramdisk";
	alternatedisk = NULL;
	kernel = "kernel0";
	alternatekernel = NULL;
#else
	if(boottype==BOOT_NORMAL || boottype==BOOT_CHARGER){
		ramdisk="ramdisk";
		alternatedisk = "recovery-rd";
		kernel="kernel0";
		alternatekernel = "kernel1" ;

	}else{
		ramdisk="recovery-rd";
		kernel="kernel1";
		alternatekernel = "kernel0" ;
	}
#endif

	void* kerneladd = NULL;
	void* ramdiskadd = NULL;

	if (boottype == BOOT_RECOVERY_MMC) {
#if defined(MODE) && (MODE == SIMPLE)
		ramdisk="__init_imapx820/ramdisk.img";
#else
		ramdisk="__init_imapx820/recovery-rd.img";
#endif
		kernel="__init_imapx820/uimage";
		if (load_file(ramdisk, CONFIG_SYS_PHY_RD_BASE, bootdev)) {
			printf("Erro: Failed to load %s from gaven mmc, try nand\n",ramdisk);
			 ramdisk="recovery-rd";
			 alternatekernel = "ramdisk" ;
			 ramdiskadd = load_image(ramdisk, alternatedisk,(char*)CONFIG_SYS_PHY_RD_BASE, bootdev);
			 if(ramdiskadd == NULL){
				 printf("Erro: Failed to load %s from gaven nand, system halted\n",ramdisk);
				 for(;;);
			 }
		}else
			ramdiskadd = (void *)CONFIG_SYS_PHY_RD_BASE;

		if (load_file(kernel, CONFIG_SYS_PHY_LK_BASE, bootdev)) {
			printf("Erro: Failed to load %s from gaven mmc, try nand\n",kernel);
			kernel = "kernel1";
			alternatekernel = "kernel0";
			kerneladd = load_image(kernel, alternatekernel, (char*)CONFIG_SYS_PHY_LK_BASE, bootdev);
			if(kerneladd == NULL){
				printf("Erro: Failed to load %s from gaven nand, system halted\n",kernel);
				for(;;);
			}
		}else
			kerneladd = (void *)CONFIG_SYS_PHY_LK_BASE;
	} else if (boottype == BOOT_RECOVERY_IUS) {

		if (load_ius(ramdisk, (char *)CONFIG_SYS_PHY_RD_BASE, bootdev)) {
			 printf("Erro: Failed to load %s from gaven disk, system halted\n",ramdisk);
			 for(;;);
		}
		ramdiskadd = (void *)CONFIG_SYS_PHY_RD_BASE;
		kernel = "kernel1";
		if (load_ius(kernel, (char *)CONFIG_SYS_PHY_LK_BASE, bootdev)) {
			kernel = "kernel0";
			if (load_ius(kernel, (char *)CONFIG_SYS_PHY_LK_BASE, bootdev)) {
				printf("Erro: Failed to load %s from gaven disk, try kernel1\n",kernel);
				for(;;);
			}
		}
		kerneladd = (void *)CONFIG_SYS_PHY_LK_BASE;
    }else {
		printf("bootl from NORMAL.\n");
		kerneladd = load_image(kernel, alternatekernel, (char*)CONFIG_SYS_PHY_LK_BASE, bootdev);
		if(kerneladd == NULL){
			printf("Erro: Failed to load %s from gaven disk, try kernel1\n",kernel);
			for(;;);
		}
	}

	 return do_boot(kerneladd, ramdiskadd);
}

