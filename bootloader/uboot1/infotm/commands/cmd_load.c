#include <common.h>  
#include <command.h> 
#include <asm/io.h>  
#include <bootl.h>   
#include  <burn.h>
#include <vstorage.h>
#include <storage.h>

int do_load (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{   
	if(argc<2){
		printf("Erro: Omitted important argument(s)\n");
		goto __exit_usage__;
	}
	int bootdev=storage_device();                                         
	if(!vs_device_burndst(bootdev)){                                      
		printf("Erro: Invalid system disk specified, it not a burndst\n");
		return -1;                                                        
	}                                                                     
	int ret = vs_assign_by_id(bootdev, 1);                                
	if(ret){                                                              
		printf("Erro: Can not get the system disk ready\n");              
		return -1;                                                        
	}                                                                     
	char *imgname; 
	char* membase=NULL;
	char* alternate =NULL;
	if(argc>2){
		membase=(char*) simple_strtoul(argv[2],0,16);
		if(((int)membase<= CONFIG_SYS_SDRAM_BASE)||((int)membase>=CONFIG_RESV_START)){
			printf("Erro: Invalid load addr specified, it will cause the system crruput\n");
			goto __exit_usage__;
		}
	}
	imgname = argv[1];
	if((!strcmp(imgname, "kernel0"))||(!strcmp(imgname, "kernel1"))){
		if(!strcmp(imgname, "kernel0")) alternate = "kernel1";
		else alternate = "kernel0";
		membase= (membase==NULL)? (char*)CONFIG_SYS_PHY_LK_BASE : membase;
	}else if((!strcmp(imgname, "ramdisk"))||(!strcmp(imgname, "recovery"))){
		if(!strcmp(imgname, "ramdisk"))alternate = "recovery";
		membase = (membase==NULL)? (char*)CONFIG_SYS_PHY_RD_BASE : membase;
	}else{
		printf("Erro: Invalid image name\n");
		goto __exit_usage__;
	}
	if(!load_image(imgname, alternate , membase , bootdev))
		return 0;
__exit_usage__:         
	cmd_usage(cmdtp);
	return 1;        
}                                                         
U_BOOT_CMD(                                                      
		load, CONFIG_SYS_MAXARGS, 1,   do_load,                
		"load image(name) to the adress you specified.",                
		"If not specify the dest addr, "
		"it will be load to addr where it will be when boot system\n"  
		"surported names\n"
		"uimage\n"
		"ramdisk\n"
		"recovery\n"
		"kernel0\n"
		"kernel1\n"
		"example:\n"
		"load kernel0 80008000\n"
		"load ramdisk\n"
		);                                                       
int do_adr(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if(argc<2){
		printf("Erro: Omitted important argument(s)\n");
		goto __exit_usage__;
	}
	char imgname[64];
	int repeat=1;
	strcpy(imgname,argv[1]);
	int imgnum=	0;
	if((!strcmp(imgname,"uimage"))
		||(!strcmp(imgname,"uImage"))
		||(!strcmp(imgname,"lk"))
		)
	{	
		strcpy(imgname,"kernel0");
		repeat +=1;
	}else if((!strcmp(imgname,"rd"))||(!strcmp(imgname,"ramdisk.img"))){
		strcpy(imgname,"ramdisk");
	}else if(!strcmp(imgname,"recovery")){
	    strcpy(imgname,"recovery-rd");
	}

	for(;imgnum<17;){
		//if(!strcmp(default_cfg[imgnum].name,imgname))break;
		imgnum++;
	}
	if(imgnum==17)
	{
		printf("Erro: Invalid image name\n");
		goto __exit_usage__;
	}

	uint8_t *img= (uint8_t *)(DRAM_BASE_PA+0x8000);
	if(argc>2){
		img=(uint8_t *) simple_strtoul(argv[2],0,16);
		if(((int)img<= CONFIG_SYS_SDRAM_BASE)||((int) img>=CONFIG_RESV_START)){
			printf("Erro: Invalid load addr specified, it will cause the system crruput\n");
			goto __exit_usage__;
		}
	}
	while(repeat){
//	    	printf("image base 0x%8x\n",img);
		if(burn_single_image(img, imgnum+repeat-1)){
			//printf("Erro: failed to burn image %s\n",default_cfg[imgnum].name);
			return -1;
		}
		strcpy(imgname,"kernel1");
		repeat--;
	}
	return 0;

__exit_usage__:
	cmd_usage(cmdtp);
	return 1;
}
U_BOOT_CMD(                                                      
		adr, CONFIG_SYS_MAXARGS, 1,   do_adr,                
		"load image(name) to the adress you specified.",                
		"If not specify the dest addr, "
		"it will be load to 0x40008000\n"
		"surported image\n"
		"uboot0\n"
		"uboot1\n"
		"items\n"
		"logo\n"
		"ramdisk\n"
		"flags\n"
		"reserved\n"
		"uimage\n"
		"system\n"
		"..."
		"example:\n"
		"adr uimage 40008000\n"
		"adr u0\n"
		);
