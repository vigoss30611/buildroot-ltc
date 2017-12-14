/***************************************************************************** 
 ** file		: seperate.c
 ** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
 ** 
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 ** 
 ** Description: Scan source media. Load seperate images from source media 
 **	acoording to the list predefined and write them to their apropriate address 
 **	A valid source media must be a fat formated media.
 ** Author	: John zEng
 **      
 ** Date		: Apr 27 2012
 ** Version	: init
 *****************************************************************************/

#include <common.h>
#include <command.h>
#include <linux/types.h>
#include <bootlist.h>
#include <fat.h>
#include  <malloc.h>
#include <ius.h>
#include <items.h>
#include <asm/io.h>
#include <vstorage.h>
#include <storage.h>
#include <bootl.h>
#include <burn.h>
#include <display.h>
#define DRIVING     0
//#define IR_MAX_BUFFER 0x8000
#define IMAP_PCB_TEST_FILE "pcbtest.itm"
#define IMAP_PCB_TEST_DIR    "pcb_test"
#define IMAP_IMAGE_SRC_DIREC "__init_imapx820"//"imapx8820images/"
#define IMAP_PRELOAD_SRC_DIREC "preload"//"imapx8820images/"
#define IMAP_PRELOAD_SRC_FILE "infotm_burn_preload_to_misc.ini"//"imapx8820images/"
#ifdef CONFIG_BOOT_AUTO_ADJUST
struct oem_freq_lvl {

	uint32_t apll;
	uint32_t dcfg;
	uint32_t lvl;
};
#endif
struct imagemedia_src {
	char media[16];
	int channel;
	int id;
};
struct imagemedia_src media_src;
char *suffixes[]={ ".isi",
	".isi", 
	".itm", 
	".isi",
	".img",
	"",
	".img",
	"",
	"",
	".img",//spare area 2-112M
	".img",
	".img",
	".img",
	".img",
	"",
	"",
	""
};
int check_media_src(uint32_t mask)
{
	int ret, id, idb;
	idb = boot_device();
	for(id = 0; id < vs_device_count(); id++)
	{
		if(!vs_device_burnsrc(id))
			/* this is not a burn media_src */
			continue;

		if(/* id == idb || */(mask & (1 << id)))
		  /* boot device will not be searched for burn source */
		  continue;


		if(id == idb)
			/* boot device will not be searched for burn media_src */
			continue;


		if(vs_is_device(id, "eth") || vs_is_device(id, "udc")) {
			ret = vs_assign_by_id(id, 0);
			if(ret == 0) {
				ret = vs_special(NULL);
				if(ret == 0)
					return -1;;
				continue;
			}
		} else
			ret = vs_assign_by_id(id, 1);
		if(ret) {     
			printf("assign device(%s) failed (%d)\n", vs_device_name(id), ret);
			continue;
		}
		if(id==storage_device()) continue;

		char *name = (char*)vs_device_name(id);
		if(name==NULL){
			printf("Erro: invalid device id, failed to got its' name\n");
			return -1;
		}
		int i=0;
		char channel[16];
		strcpy(channel,name);
		while(('a'<=name[i]&& name[i]<='z')||('A'<=name[i]&& name[i]<='Z')){
			i++;
		}
		channel[i]='\0';
		strcpy(media_src.media,name);
		strcpy(channel,name+i);
		media_src.channel=simple_strtoul(channel,0,10);
		media_src.id=id;
		if(!vs_device_burnsrc(id)){
			printf("Erro: the arranged device is not a valid burn media_src\n");
			return -1;
		}

		return 0;
	}
//	printf("No src card was detected\n");
	return -1;
}
int form_image_desc(struct ius_desc *desc,int imagenum,void const* image)
{
	uint32_t type, max =0;
	loff_t offs_o = 0;
	int ido=storage_device();
	char * size=getenv("filesize");
	int defaultsz[2]={
		0x80000,
		0x400000,
	};
//	if(NULL==size){
//		size=storage_size("default_cfg[imagenum].name");
//		printf("Erro: file %s not found in the android directory\n",default_cfg[imagenum].name);
//		return -1;
//	}
	/*******Here we create a image descriptor*******/
	if(DEV_NAND==ido) {
		type= imagenum<INFO_SPARE_IMAGES? IUS_DESC_STNR:IUS_DESC_STNF;
		if(imagenum<1){
			type=IUS_DESC_STNB;
		}else{
			type=IUS_DESC_STNR;
		}
		if(INFO_SPARE_IMAGES<=imagenum) type= 0x20+ imagenum-INFO_SPARE_IMAGES;
	}else{
		if(imagenum<INFO_SPARE_IMAGES){
			type=IUS_DESC_STRW;
		}else{
			type= 0x20+ imagenum-INFO_SPARE_IMAGES;
		}
	}
	desc->sector|=(type&0x3f)<<24;
	if(image<(void const*)DRAM_BASE_PA){
		printf("Erro: wrong image address, exit!!!!\n");
		return -1;
	}
	int addr=(int)image;
//	printf("image base:0x%8x\n",image);
	if(!(0x1ff&(addr-DRAM_BASE_PA)))
		desc->sector|=(addr-DRAM_BASE_PA)>>9;
	else{
	    printf("Erro: the image base should be 512bytes aligned\n ");
	    return -1;
	}
	//loff_t offs_o = storage_offset(default_cfg[imagenum].name);

	if(type==IUS_DESC_STNB)
	    offs_o=0;

	//printf("offs:0x%llx for image %s\n",offs_o,default_cfg[imagenum].name);
	desc->info0=(uint32_t) (offs_o&0xffffffff);
	desc->info1=(uint32_t)(((offs_o>>32)&0xffff));
	if(NULL!=size)
		desc->length =simple_strtoul(size,0,16);
	if(0==desc->length){
		desc->length=imagenum<=6?defaultsz[0]:defaultsz[1];
	}
	printf("desc length:0x%x\n",desc->length);
	//max=default_cfg[imagenum].size;//max space for each spare image
	if(type==IUS_DESC_STNB)
	    max=2;
	desc->info1|= (max<<20);
	return 0;

}
static int load_file(char *filename,unsigned int addr )
{
	char cmd[64];
	vs_assign_by_id(media_src.id,0);
	sprintf(cmd,"fatload %s %d:1 %x %s",media_src.media , media_src.channel , addr,filename);
	printf("%s\n",cmd);
	if(!run_command(cmd,0)){
		return 0;
	}
	return 1;
}
int	burn_single_image(uint8_t *img, int imagenum)
{
	int  idi ,ido, ret;
	struct ius_desc desc={
		.sector=0,
		.info0=0,
		.info1=0,
		.length=0,
	};
	printf("image number:%d\n",imagenum);
//	uint32_t  max=default_cfg[imagenum].size<<20;
	idi=vs_device_id("ram");
	printf("idi:%d\n",idi);
	ido=boot_device();
	
	printf("ido:%d\n",ido);
	if(ido==DEV_NONE) ido=storage_device();
	/*if(ido==DEV_BND&&imagenum!=0) {
		ido=DEV_NAND;
	}else if(ido==DEV_NAND&&imagenum==0) ido=DEV_BND;*/
	/*if (ido == DEV_BND) {
		if ((imagenum == 1) || (imagenum == 2))
			ido = DEV_NAND;
		else if (imagenum > 2) {
			ido = DEV_FND;
			desc.sector |= (IUS_DESC_STNF << 24);
		}
	} else if (ido == DEV_NAND) {
		if (imagenum == 0) {
			ido = DEV_BND;
		} else if (imagenum > 2) {
			ido = DEV_FND;
			desc.sector |= (IUS_DESC_STNF << 24);
		}
	} else if (ido == DEV_FND) {
		if (imagenum == 0) {
			ido = DEV_BND;
		} else if ((imagenum == 1) || (imagenum == 2))
			ido = DEV_NAND;
	}*/
	ido = DEV_FND;
	printf("ido:%d\n",ido);
	if(form_image_desc(&desc, imagenum,img)) return -1;
	//if(issparse((char*)img)) return burn_extra(&desc, idi);

	if(INFO_SPARE_IMAGES<=imagenum)
	    ret = burn_extra(&desc, idi);
	else {
	    uint8_t *buf = (uint8_t*)malloc(IR_MAX_BUFFER);
	    if(buf==NULL){
		printf("Erro: No memory space for image burn\n");
		return -1;
	    }

	    ret=burn_image_loop(&desc, idi, media_src.channel, ido,
					ius_desc_maxsize(&desc), buf);
	    free(buf);

	    if(!strcmp(suffixes[imagenum],".itm")){//when there is a new item file, repartition the system disk
		init_config();
		/*if(storage_part()<0){                                                               
		    printf("Erro: failed to  part system disk acoording to gaven configuration\n"); 
		    return -1;                                                                      
		}*/                                                                               
	    }
	}

	return ret;

}
int update_images(void)
{
	unsigned int rambase=CONFIG_SYS_SDRAM_BASE+IUS_DEFAULT_LOCATION;
	int imagenum=0 ;
	char filename[64];
	int ret = 0;


	for(;imagenum<17;imagenum++){
		    	if(imagenum==7){
			    imagenum = 8;
			    sprintf(filename,"%s/%s",IMAP_IMAGE_SRC_DIREC, "uimage");
			}//else
				//sprintf(filename,"%s/%s%s",IMAP_IMAGE_SRC_DIREC, default_cfg[imagenum].name , suffixes[imagenum]);

		if(imagenum<=9){
			if(load_file(filename,rambase)){
				continue;
			}

			/****************burn the image***************/
			ret = 1;
			set_boot_type(BOOT_RECOVERY_MMC);
			break;
			//burn_single_image((uint8_t *) rambase, imagenum);
			//if(imagenum==7||imagenum==8||imagenum==9)
				//set_boot_type(BOOT_FACTORY_INIT);
			
		}else{
			if(file_fat_ls(filename) >= 0){
				ret = 1;
				printf("file %s detected exist\n", filename);
				printf("break out the seperate image process and enter the recovery mode\n");
				set_boot_type(BOOT_FACTORY_INIT);
				break; //break out the seperate image process and enter the recovery mode
			}
		}
	}
	return ret;
}
int try_seperate_source(void)
{
	char cmd[64];
	
	/* skip seperate burn if MMC1 boot */
	if(boot_device() == DEV_MMC(1))
	    return 0;

	if(check_media_src((1 << DEV_MMC(0)) | (1 << DEV_MMC(2)))){
		printf("No media_src for seperate images detected\n");
		return 0;
	}
		/* check weither the media_src is a fat media_src, if not, ignore it*/
	sprintf(cmd,"fatls %s %d %s",media_src.media , media_src.channel,IMAP_IMAGE_SRC_DIREC);
	if(run_command(cmd,0)){
		if(update_images()) //if there is any image, system boot in recovery mode directly 
			                //or system will try pcb test if no image been found
			return 0;
	}	
	sprintf(cmd,"fatls %s %d %s",media_src.media , media_src.channel ,IMAP_PCB_TEST_DIR);
	if(run_command(cmd,0)){
		sprintf(cmd,"fatload %s %d:1 %x %s/%s",
				media_src.media , media_src.channel , 
				CONFIG_SYS_SDRAM_BASE+IUS_DEFAULT_LOCATION,IMAP_PCB_TEST_DIR,IMAP_PCB_TEST_FILE);
         if(!run_command(cmd,0)){
			 set_boot_type(BOOT_FACTORY_INIT); 
			 return 0;
		 }
	}

    sprintf(cmd,"fatls %s %d %s",media_src.media , media_src.channel ,IMAP_PRELOAD_SRC_DIREC);
    if(run_command(cmd,0)){
        sprintf(cmd,"fatload %s %d:1 %x %s/%s",
				media_src.media , media_src.channel , 
				CONFIG_SYS_SDRAM_BASE+IUS_DEFAULT_LOCATION,IMAP_PRELOAD_SRC_DIREC, IMAP_PRELOAD_SRC_FILE);
        if(!run_command(cmd,0)){
			set_boot_type(BOOT_FACTORY_INIT); 
			return 0;
        }
    }
	printf("Erro: not a fat card, exit\n");
	return 0;
}
