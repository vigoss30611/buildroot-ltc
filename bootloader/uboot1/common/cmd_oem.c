/***************************************************************************** 
** common/cmd_oem.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: OEM commands that make things more convenient.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 02/02/2010 XXX	Warits
*****************************************************************************/

#include <common.h>
#include <command.h>
#include <oem_func.h>
#include <asm/io.h>
#include <nand.h>
#include <oem_inand.h>
#ifdef CONFIG_SYS_DISK_iNAND
int do_ls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return 0;

}
U_BOOT_CMD(
	ls, CONFIG_SYS_MAXARGS, 1,	do_ls,
	"ls\n",
	""
);

int do_bootl (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char cmd[64];
	char boot_arg[256];

	sprintf(cmd,"mmc rescan %d",iNAND_CHANNEL);
	run_command(cmd,0);
	if(oem_load_LK()){
		//set flag for uImage error
		oem_set_maintain();
		reset_cpu(0);
	}	
/*	if(argc<2){	
		sprintf(cmd,"bootm %x",CONFIG_SYS_PHY_LK_BASE);
		run_command(cmd,0);
	}
*/
	if(oem_is_recover())
	{
		if(oem_load_RD_())
		{
			//set maintain flag
			oem_set_maintain();
			reset_cpu(0);
		}
		sprintf(boot_arg, "%s bmagic=0x%08x hwver=%d.%d.%d.%d",
				CONFIG_LINUX_RECOVER_BOOTARGS, CONFIG_BOARD_MAGIC,
				CONFIG_BOARD_OEM, CONFIG_BOARD_HWVER >> 16,
				(CONFIG_BOARD_HWVER >> 8) & 0xff, CONFIG_BOARD_HWVER & 0xff);
		setenv("bootargs", boot_arg);

//		setenv("bootargs", CONFIG_LINUX_RECOVER_BOOTARGS);
	}else{
		if(oem_load_RD())
		{
			//set maintain flag
			oem_set_maintain();
			reset_cpu(0);
		}
		char memcap[32];
#ifdef CONFIG_HIBERNATE
		sprintf(memcap,"mem=%dM", CONFIG_SYS_SDRAM - 12);
#endif

		sprintf(boot_arg, "%s %s bmagic=0x%08x hwver=%d.%d.%d.%d",
				CONFIG_LINUX_DEFAULT_BOOTARGS,memcap, CONFIG_BOARD_MAGIC,
				CONFIG_BOARD_OEM, CONFIG_BOARD_HWVER >> 16,
				(CONFIG_BOARD_HWVER >> 8) & 0xff, CONFIG_BOARD_HWVER & 0xff);
		setenv("bootargs", boot_arg);

//		setenv("bootargs", CONFIG_LINUX_DEFAULT_BOOTARGS);
	}	

	sprintf(cmd,"bootm %x %x",CONFIG_SYS_PHY_LK_BASE,CONFIG_SYS_PHY_RD_BASE);
	run_command(cmd,0);
/*
 * hope never reach here.
 *
 */
	printf("Error:Something error occoured when boot android.\n");
	oem_simpro_finish("Error occoured during booting process, system halted.");
	for(;;);

	return 0; //Make compiler happy.

}
U_BOOT_CMD(
	bootl, CONFIG_SYS_MAXARGS, 1,	do_bootl,
	"start uImage at address 'addr'",
	"zImage is unsafe, using of this is not recommended.\n"
);
#if 0
int do_read_image(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char filename[64];
	
	printf("Read image.\n");

	if(argc<4||argc>5)
	    goto __exit_usage__; 
	 
	if(!strcmp(argv[1],"inand")){
	 	//read image from indand through prefixed addr	
		if(!strcmp(argv[3],"uImage")||!strcmp(argv[3],"uimage")){                
			sprintf(filename,"backup uImage");
                }else if(!strncmp(argv[3],"ramdisk",7)){
			sprintf(filename,"backup ramdisk.img");
                }else if(!strncmp(argv[3],"recovery",8)){
			sprintf(filename,"recovery_rd.img");
                }else if(!strcmp(argv[3],"updater")){
			sprintf(filename,"backup updater");
                }else{
			printf("Error: wrong image name %s.\n",argv[3]);
			goto __exit_usage__;
		}
	 }else{
		printf("Error:Invalid device name %s.",argv[1]);
	}
__exit_usage__:
	cmd_usage(cmdtp);
	return 1;
}
U_BOOT_CMD(
        read, CONFIG_SYS_MAXARGS, 1,       do_read_image,
        "read image from pregaven device according to file name to gaven addr.",
 	"read devicename(inand,sdcard) memaddr filename(uImage,ramdisk,recovery,updater...)\n"
	"Example:\n"
	"read sdcard 40008000 uImage\n"
	"read inand 40008000 ramdisk 1\n"	
);
#endif
int do_write_image(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	
	uint8_t * addr;

	if(argc<2||argc>4){
		printf("Error: Command format error.\n");
		goto __exit_usage__;
	}
	if(argc==2) addr=(uint8_t *)0x40008000;
		else addr=(uint8_t *)simple_strtoul(argv[2], NULL, 16);
	if(((uint32_t)addr < CONFIG_SYS_SDRAM_BASE)
	   || ((uint32_t)addr > CONFIG_SYS_SDRAM_END))
	{
		printf("Invalid memory address!\n");
		return -1;
	}

	if((!strcmp(argv[1],"lk"))||
	   (!strcmp(argv[1],"uImage"))||
	   (!strcmp(argv[1],"uimage"))){

		printf("Updating linux kernel image ...\n");
		return oem_burn_LK(addr );
		
	}else if((!strcmp(argv[1],"rd"))||
		(!strcmp(argv[1],"ramdisk"))||
		(!strcmp(argv[1],"ramdisk.img"))){	

		printf("Updating ramdisk image ...\n");
		return oem_burn_RD(addr );
		
	}else if((!strcmp(argv[1],"recovery"))||
		(!strcmp(argv[1],"r0"))||
		(!strcmp(argv[1],"recovery_rd.img")||
		(!strcmp(argv[1],"recovery_rd")))){	

		printf("Updating recovery ramdisk image ...\n");
		return oem_burn_RD_(addr );
	
	}else if(!strcmp(argv[1],"updater")){	

		printf("Backup updater ...\n");
		return oem_burn_UPDT(addr );
	}else if(!strcmp(argv[1],"u0")||!strcmp(argv[1],"uboot")){
			printf("Updating uboot image at 0x%x...\n",(unsigned int)addr);
			return oem_burn_U0(addr);
	
	} else if(!strncmp(argv[1], "as", 2)){

            printf("Updating backup android system image ...\n");
			return oem_burn_zAS(addr);
    }else if(!strncmp(argv[1], "user", 4)){
		printf("Erasing user partition.\n");
		return oem_disk_clear();
	}else{
		printf("Error:Invalid image name %s\n",argv[1]);
		goto __exit_usage__;
	}
	
 __exit_usage__:
        cmd_usage(cmdtp);
        return 1;	
}
U_BOOT_CMD(
        adr, CONFIG_SYS_MAXARGS, 1,       do_write_image,
        "write image at the gaven memory address to inand according to file name to prefixed addr.",
        "adr filename(uImage,ramdisk,recovery,updater...)  memaddr isbackup(1 backup image, 0 do not backup, if this paramete be omited,it means backup the image.)\n"
	"Example:\n"
	"adr u0 <40008000>\n"
	"adr ramdisk.img <40008000> <0>"
);
int do_put_data(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int length = 0x20;
	int i =0;
	char *addr =( char * ) 0x40008000;
	if(argc>1) addr = (char *)simple_strtoul(argv[1], NULL, 16);
	if(argc>2) length=simple_strtoul(argv[2], NULL, 16);
	length +=32-(length&0x1F);
	while(length>=32){
		i =0;
		while(i<32)
		{
			printf("%x%x ",(addr[i]&0xF0)>>4,(addr[i]&0xF));
			i++;
		}
		printf("\n");
		addr+=i;
		length -=i;
	}
        return 0;
}
U_BOOT_CMD(
    	put, CONFIG_SYS_MAXARGS, 1,      do_put_data,
     	"put data at the gaven memory address.",
       	"Example:\n"
        "put 40008000 400\n"
);

#ifdef CONFIG_WINCE_FEATURE
int do_bootw (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
//	oem_load_NK();
//	oem_bootw();
	return 0;
}

U_BOOT_CMD(
	bootw, CONFIG_SYS_MAXARGS, 1,	do_bootw,
	"Boot pre-burned wince.\n",
	""
);
#endif
#else
int do_ls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if 0
	uint32_t size = 0x2000, bsize = 0x80000, i, j;
	nand_info_t *nand;

	nand = &nand_info[nand_curr_device];
	for(j = 0; j < 1024; j++)
	{
		for(i = 0; i < 128; i++)
		{
			size = 0x2000;
			nand_read_skip_bad(nand, 0, &size,
			   (u_char *)(0x40008000));
		}

		bsize = 0x80000;
		nand_read_skip_bad(nand, 0, &bsize,
		   (u_char *)(0x40008000));

		bsize = 0x80000;
		oem_erase_markbad(0, 0x80000);
		nand_write_skip_bad(nand, 0, &bsize,
		   (u_char *)(0x40008000));

		printf("%d MB\n", j);
	}
#endif
	printf("   ...\n");
	return 0;
}

U_BOOT_CMD(
	ls, CONFIG_SYS_MAXARGS, 1,	do_ls,
	"ls.\n",
	""
);

#if 0 /* bootz, bootl removed */
int do_bootz (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u_long	addr;
	void	(*theKernel)(int zero, int arch, uint params);
	DECLARE_GLOBAL_DATA_PTR;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	theKernel = (void (*)(int, int, uint))addr;

	theKernel(0, MACH_TYPE, gd->bd->bi_boot_params);
	/* Do not return */

	return 0;
}

U_BOOT_CMD(
	bootz, CONFIG_SYS_MAXARGS, 1,	do_bootz,
	"start zImage at address 'addr'",
	"zImage is unsafe, using of this is not recommended.\n"
);



#endif

// Boot default linux
int do_bootl (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char boot_arg[1024];
	oem_load_LK();
	
	if(argc < 2)
	  oem_bootl(NULL);
	else
	{
		sprintf(boot_arg, "%s hwver=%d.%d.%d.%d",
		   CONFIG_LINUX_DEFAULT_BOOTARGS,
		   CONFIG_BOARD_OEM, CONFIG_BOARD_HWVER >> 16,
		   (CONFIG_BOARD_HWVER >> 8) & 0xff, CONFIG_BOARD_HWVER & 0xff);

#ifdef CONFIG_CMDLINE_PARTITIONS
		strcat(boot_arg, CONFIG_CMDLINE_PARTITIONS);
#endif

		if(oem_is_recover())
		{
			printf("Booting linux(recovery) ...\n");
			oem_load_RD_();
			strcat(boot_arg, " androidboot.mode=recovery");
		}
		else
		{
			printf("Booting linux(normal) ...\n");
			oem_load_RD();
			strcat(boot_arg, " androidboot.mode=normal");
		}
		setenv("bootargs", boot_arg);
		run_command("bootm 40007fc0 40ffffc0", 0);
	}
	return 0;
}

U_BOOT_CMD(
   bootl, CONFIG_SYS_MAXARGS, 1,	do_bootl,
   "Boot pre-burned linux.\n",
   ""
   );

#ifdef CONFIG_WINCE_FEATURE
int do_bootw (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	oem_load_NK();
	oem_bootw();
	return 0;
}

U_BOOT_CMD(
   bootw, CONFIG_SYS_MAXARGS, 1,	do_bootw,
   "Boot pre-burned wince.\n",
   ""
   );
#endif

#if 0
int do_wup (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("Launching update tools ...\n");
	oem_update_wince();
	return 0;
}

U_BOOT_CMD(
   wup, CONFIG_SYS_MAXARGS, 1,	do_wup,
   "WinCE update tools\n",
   ""
   );
#endif

/* XXX This command is removed */
#if 0
int do_kup (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("Launching update tools ...\n");
	oem_basic_up_uimage();
	return 0;
}

U_BOOT_CMD(
   kup, CONFIG_SYS_MAXARGS, 1,	do_kup,
   "uImage update tools\n",
   ""
   );
#endif


int do_adr (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32_t size=0, start=0;
	uint8_t *dat;


	if(argc < 3)
	  goto __exit_usage__;

	dat = (uint8_t *)simple_strtoul(argv[2], NULL, 16);
	if(((uint32_t)dat < CONFIG_SYS_SDRAM_BASE)
	   || ((uint32_t)dat > CONFIG_SYS_SDRAM_END))
	{
		printf("Invalid memory address!\n");
		return -1;
	}
	if(argc > 3)
	  size = simple_strtoul(argv[3], NULL, 16);
	else
	  size = simple_strtoul(getenv("filesize"), NULL, 16);

	if(strncmp(argv[1], "rd", 2) == 0)
	{
		printf("Updating ramdisk image ...\n");
		return  oem_burn_RD(dat);
	}
	else if(strncmp(argv[1], "r0", 2) == 0)
	{
		printf("Updating recovery ramdisk image ...\n");
		return oem_burn_RD_(dat);
	}
	else if(strncmp(argv[1], "lk", 2) == 0)
	{
		printf("Updating linux kernel image ...\n");
		return oem_burn_LK(dat);
	}
	else if(strncmp(argv[1], "as", 2) == 0)
	{
		printf("Updating backup android system image ...\n");
		return oem_burn_zAS(dat);
	}
	else if(strncmp(argv[1], "nk", 2) == 0)
	{
		printf("Updating NK image ...\n");
		return oem_burn_NK(dat);
	}
	else if(strncmp(argv[1], "uboot", 5) == 0)
	{
		printf("Updating uboot image ...\n");
		return oem_burn_uboot(dat);
	}
	else if(strncmp(argv[1], "u0", 2) == 0)
	{
		printf("Updating uboot 0 image ...\n");
		return oem_burn_U0(dat);
	}
	else if(!strncmp(argv[1], "system", 6))
	{
		printf("Updating system image ...\n");
		return oem_burn_SYS(dat, size);
	}
	else if(!strncmp(argv[1], "user", 4))
	{
		printf("Updating user data image ...\n");
		return oem_burn_UDAT(dat, size);
	}
	else if(!strncmp(argv[1], "ndisk", 5))
	{
		printf("Updating ndisk image ...\n");
		return oem_burn_NDISK(dat, size);
	}

	if(argc > 4)
	  start = simple_strtoul(argv[4], NULL, 16);
	else
	  goto __exit_usage__;

	if(start < 0x80000)
	{
		printf("adr: Access to the first block is denied!\n");
		return -1;
	}

	if(!strncmp(argv[1], "burn", 4) && (argc > 3))
	{
		printf("Writing yaffs2 data from 0x%08x to 0x%08x ...\n",
		   (uint32_t)dat, start);
		if(oem_write_yaffs((uint8_t *)dat, size, start, size) == 0)
		  printf("Writing data finished.\n");
		else
		  printf("Writing data failed.\n");
	}

	return 0;

__exit_usage__:
	cmd_usage(cmdtp);
	return 0;
}

U_BOOT_CMD(
   adr, CONFIG_SYS_MAXARGS, 1, do_adr,
   "adr (OEM implanted image burnning tool.)",
   "burn data size nd_addr\n"
   "adr system mem_addr\n"
   "adr user mem_addr\n"
   "adr ndisk mem_addr\n"
   "adr rd mem_addr\n"
   "adr rd_ mem_addr\n"
   "adr lk mem_addr\n"
   "adr nk mem_addr\n"
   "adr as mem_addr\n"
   "adr uboot mem_addr\n"
   /*  "adr u0 mem_addr\n" */ // yi ban ren wo bu gao su ta
   );
#endif

#ifdef CONFIG_NOR
int do_nor (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32_t size, start;
	uint8_t *dat;

	if(argc < 2)
	  return 0;

	nor_hw_init();
	dat = (uint8_t *)simple_strtoul(argv[1], NULL, 16);

	if(dat ==  (uint8_t *)0x2b)
	{
		nor_erase_chip();
		return 0;
	}

	start = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);

	nor_program(start, dat, size);
	return 0;
}

U_BOOT_CMD(
   nor, CONFIG_SYS_MAXARGS, 1, do_nor,
   "nor (OEM implanted nor tool.)",
   ""
   );
#endif

#ifdef CONFIG_BOOT_AUTO_ADJUST
int do_mmmt (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	oem_test_denali();
	return 0;
}

U_BOOT_CMD(
   mmmt, CONFIG_SYS_MAXARGS, 1, do_mmmt,
   "mmmt (OEM implanted memory test tool.)",
   ""
   );
#endif

int do_iuw (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint8_t *dat;

	if(argc < 2)
	{
		printf("iuw wrap_addr\n");
		return 0;
	}

	dat = (uint8_t *)simple_strtoul(argv[1], NULL, 16);
	if(iuw_confirm(dat) == 0)
	  iuw_update(dat);

	return 0;
}

U_BOOT_CMD(
   iuw, CONFIG_SYS_MAXARGS, 1, do_iuw,
   "iuw (InfoTM Update Wrap tools)",
   ""
   );

