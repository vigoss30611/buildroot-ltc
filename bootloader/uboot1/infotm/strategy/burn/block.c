/*******************************************************************************************************************
file		: part.c
description : This file is only for block devices such as mmc, sd, usb storage devices and so on. It will init partition 
			  table for the system disk according to the partcfg table,  wich was formed according to configuration file 
			  supplied by vendor.
author		: John
data		: Apr24 2012
version     : init	
 *******************************************************************************************************************/
#include <common.h>
#include <oem_func.h>
#include <storage.h>
#include <linux/types.h>
#include <malloc.h>
#include <items.h>
#include <vstorage.h>
#define readl(addr) (*(volatile unsigned int*)(addr)) 
#define writel(b,addr) ((*(volatile unsigned int *) (addr)) = (b))
int partnum=0;
int tablecnt=0;
/*
 unsigned int sectors[INFO_MAX_PARTITIONS]={
	0,
	0,
	0,
	0,	
	0,
	0,
	0,
	0,
};
*/
unsigned int sectors[INFO_MAX_PARTITIONS]={
	256,
	256,
	256,
	256,	
	0,
	0,
	0,
	0,
};

loff_t spare_area=128;
static int reformat_part_size(int sectorcount , int blksz)
{
	int multip=(0x1<<20)/blksz;//we change the sector size unit from Mbytes to Blocks
	spare_area=spare_area*multip + (INFO_PART_CFG_BASE+1);//configuration file never be more than a block
	spare_area = ((spare_area+2)&(~(0x40-1)))+0x40;
	printf("spare_area:0x%llx mutilp:0x%x\n",spare_area,multip);
	partnum +=1;//add a extension part
	int index=0;
	for(index=0;index<INFO_MAX_PARTITIONS;index++)
		sectors[index]*=multip;
	index=INFO_MAX_PARTITIONS-1;
	while(index>3){
		sectors[index] =sectors[index-1];
		index-=1;
	}
	int count=0;
	while(index){
		index-=1;
		count+=sectors[index];
	}
	if(sectorcount<=count){
		printf("Erro:size assigned for the first 3 part was bigger than the disk size\n");
		return -1;
	}
	sectors[3]=sectorcount-count - spare_area;
	
	if(sectors[5]==0){
		index=4;
		count=0;
		while(index<partnum){
			count = count+ sectors[index] ;
			index++;
		}
		if(sectors[3]<=count){
			printf("Erro:extended partitions capacity defined too large to overflow the disk capacity, exit!\n");
			return -1;
		}
		if((sectors[3]>count)&&(sectors[3]-count )> (multip<<5)){//here we set the least local part size as 32M
			sectors[5]=sectors[3]-count;
		}else{
			index=5;
			while(index<partnum){
				sectors[index]=sectors[index+1];
				index+=1;
			}
			sectors[index]=0;
		}
	}
	
	index=4;
	while(index<partnum){
		if(sectors[index]!=0)
			sectors[index]-=0x10;
		index++;
	}
	printf("sector[]={\n");
	for(index=0;index<INFO_MAX_PARTITIONS;index++){
		printf("0x%8x\n",sectors[index]);
	}
	
	 printf("};\n");
	 index=INFO_MAX_PARTITIONS-1;
	while(sectors[index]==0) index-=1;
	partnum=index+1;
	return 0;
}
int init_part_location(struct part_table* locationtable)
{
	unsigned int i=0;

	for(i=0;i<partnum;){
		locationtable[i].sizeinsectors=sectors[i];
		locationtable[i].parttype=0x83;
		locationtable[i].startCHS[0]=0x03;
		locationtable[i].startCHS[1]=0xd0;
		locationtable[i].startCHS[2]=0xFF;
		locationtable[i].endCHS[0]=0x03;
		locationtable[i].endCHS[1]=0xd0;
		locationtable[i].endCHS[2]=0xFF;
		locationtable[i].bootableflag=0;
		if(sectors[++i]==0) break;
	}
	//locationtable[0].parttype=0x0C;
//	locationtable[partnum-1].parttype=0x0C;
	if(partnum>4) locationtable[3].parttype=0x05;	 

	locationtable[0].startCHS[0]=0x01;
	locationtable[0].startCHS[1]=0x01;
	locationtable[0].startCHS[2]=0x00;
	locationtable[0].startLBA= spare_area ; //The 1st part start from the end of spare image area

	printf("system offs:%x\n",locationtable[0].startLBA);

	for(i=1;(i<partnum)&&(i<4);i++){
		locationtable[i].startLBA=locationtable[i-1].startLBA+locationtable[i-1].sizeinsectors;
		if(locationtable[i].startLBA%0x40){
			locationtable[i].startLBA+=0x40-locationtable[i].startLBA%0x40;
			locationtable[i].sizeinsectors-=0x40;
		}
	}

	for(;i<partnum;i++)
		locationtable[i].startLBA=0x000010;

	return 0;
}
int init_part_table(struct part_table* locatition, struct part_table* parttable, int* tableaddr, char step)
{
	static int lasttableaddr;

	memset(parttable,0,sizeof(struct part_table)<<2);
	if(!step){
		memcpy(parttable,locatition, sizeof(struct part_table)<<2) ;
		*tableaddr=0 ;
		lasttableaddr=*tableaddr;
	}else{		
		if(step>(partnum-3)){
			printf("Error: outof partition range. step %d partnum:%d partnum-3:%d\n",step,partnum,partnum-3);
			return -1;
		}
		parttable[0]= locatition[step+3];
		if(step<(partnum-4)){

			printf("step:%d  partnum:%d\n",step,partnum);
			int i=1;
			parttable[1]= locatition[step+4];
			if((step+1)<(partnum-3))parttable[1].parttype=0x05;
			parttable[1].startLBA=0;
			for(;i<=step;i++){
				parttable[1].startLBA+=0x0010;
				parttable[1].startLBA+=locatition[i+3].sizeinsectors;
			}
		}
		if(step==1) *tableaddr= locatition[3].startLBA;
		else *tableaddr=lasttableaddr+ 0x000010 + locatition[step+2].sizeinsectors;
		lasttableaddr=*tableaddr;
	}	
	return 0 ;
}

int block_partition(struct part_sz *psz)
{
	
	int dev_id=storage_device();
	int blksz=vs_align(dev_id);
	
	printf("==================================================================================================");
	lbaint_t lba=vs_capacity(dev_id);
	if(lba<=0){
		printf("Erro: Got an invalid disk capacity for system disk partition, exit\n");
			return -1;
	}
	printf("SYS disk:0x%llx bloks(%d bytes per block), devce id:%d\n",(unsigned long long int)lba, blksz, dev_id);
	vs_assign_by_id(dev_id, 1);

	struct part_table partitiontable[INFO_MAX_PARTITIONS];
	struct part_table *locationtable=(struct part_table *)malloc(INFO_MAX_PARTITIONS*sizeof(struct part_table));
	if(NULL==locationtable){
		printf("Erro: Failed to alloc memory for lpartitio location table\n");
		return -1;
	}
	reformat_part_size(lba , blksz);
	init_part_location(locationtable);

	loff_t imagebase[partnum];
	memset(imagebase,0, partnum*sizeof(loff_t));
	uint8_t * buffer=(uint8_t *)malloc(blksz);
	if(NULL==buffer){
		printf("failed to alloc memory for part table buffer\n");
		return -1;
	}

	unsigned int i=0;
	for(i=0;i<4&&i<partnum;i++) imagebase[i]=locationtable[i].startLBA;

	tablecnt=(partnum>3)? (partnum-3): 1;
	printf("need to create %d part tables\n ",tablecnt);
	int tableaddr=0;

	int table=0;
	loff_t offs=0;
	for(i=1;i<tablecnt;){
		memset(buffer,0,blksz);
		init_part_table(locationtable,partitiontable,&tableaddr,i);
		memcpy(buffer+446 , partitiontable , sizeof(struct part_table)<<2);

		table=0;
		while(table<4){
			printf(" 0x%x  0x%8x 0x%8x \n",
					partitiontable[table].parttype ,partitiontable[table].startLBA ,partitiontable[table].sizeinsectors);
			table++;
		}
		buffer[510]=0x55;
		buffer[511]=0xaa;
		vs_assign_by_id(dev_id, 0);
		printf("index:0x%llx\n",offs);
		offs=tableaddr;
		vs_write(buffer,offs*blksz ,blksz,0);
		imagebase[i+3]=tableaddr+0x10;
		printf("table:%d tablecnt:%d\n",i,tablecnt);
		i++;
	}
	//	int ret=tableaddr;
	
	memset(buffer,0,blksz);
	init_part_table(locationtable,partitiontable,&tableaddr,0);
	memcpy(buffer+446 , partitiontable , sizeof(struct part_table)<<2);
	int *parted= (int *)buffer;
	parted[0x48]=DISK_PARTITIONED;//mark the disk has been partitioned
	table=0;
	 while(table<4){
		 printf(" 0x%x  0x%8x 0x%8x \n",
				 partitiontable[table].parttype ,partitiontable[table].startLBA ,partitiontable[table].sizeinsectors);
	                    table++;
	 }

	buffer[510]=0x55;
	buffer[511]=0xaa;
	vs_assign_by_id(dev_id, 0);
	vs_write(buffer , 0,blksz,0);

	memcpy(buffer, psz, (INFO_MAX_PARTITIONS+INFO_SPARE_IMAGES)*sizeof(struct part_sz));
	struct part_sz *p=(struct part_sz *)(buffer);
	p+=INFO_SPARE_IMAGES;
	
	for(i=0;i<3;){
		p[i].offs=imagebase[i];
		i++;
	}

	if(partnum>4){
		p[3].offs=imagebase[4]; //partnum will never be 5
		switch(partnum){
			case 8:
				p[5].offs=imagebase[6];
			case 7:
				p[4].offs=imagebase[5];
			case 6:
				p[INFO_MAX_PARTITIONS-2].offs=imagebase[partnum-1];
				break;
			default:
				break;
		}
	}
	
	vs_assign_by_id(dev_id, 0);
	vs_write(buffer , blksz , blksz,0);
	free(buffer);
	free(locationtable);
	printf("==================================================================================================");
	return 0;
}
loff_t block_get_base(const char* target)
{
	int dev_id=storage_device();
	int blksz=vs_align(dev_id);
//	printf("%s enter %s blksz:%d\n",__func__,target,blksz);
	struct part_sz  *buffer=(struct part_sz*)malloc(blksz);  
	if(NULL==buffer){                       
		printf("Failed to malloc buffer\n");
		return -1;                          
	}         
	vs_assign_by_id(dev_id , 1);
	vs_read((uint8_t *) buffer, blksz ,blksz,0);
	/*
	   printf("failed to got disk partition infomation. dev_id:%d\n",dev_id);
	   goto erro_exit;
	   }
	   */
	int index = 0;
	loff_t base=0;
	printf("%s enter %s  1\n",__func__,target);
	while(index<(INFO_MAX_PARTITIONS+INFO_SPARE_IMAGES)){
		if(buffer[index].name==NULL){
			printf("Erro: can not find infomation of part:%s\n",target);
			goto erro_exit;
		}
		//	printf("%s\n",buffer[index].name);

		if(!strcmp(buffer[index].name,target)){
			printf("%d\n",index);
			base = buffer[index].offs;
			break;
		}
		index++;
	}
	//printf("%s enter %s   0x%8x 2\n",__func__,target,base);
	loff_t capacity=vs_capacity(dev_id);
	printf("%s enter %s 3\n",__func__,target);

	base = (index<INFO_SPARE_IMAGES)? (base<<20) : base*blksz; //unit Bytes
	if(base>(capacity*blksz)){
		printf("Erro: Got an invalid part base, overflow the disk range\n");
		goto erro_exit;
	}
	free(buffer);
	return base;
erro_exit:
	free(buffer);
	return -1; //if base little than or equal to 0, it is invalid.

}
int block_is_parted(void)
{
	int dev_id=storage_device();
	int blksz=vs_align(dev_id);
	int *buffer=(int *)malloc(blksz);  
	int ret=0;
	if(NULL==buffer){                       
		printf("Failed to malloc buffer\n");
		return -1;                          
	}         
	vs_assign_by_id(dev_id , 1);
	vs_read((uint8_t *) buffer,0, blksz,0);
	/*){   
		printf("failed to got disk partition state.\n");
		ret = -1;
		goto exit;
	} */                                                  
	ret= buffer[0x48] ==DISK_PARTITIONED ? 1:0;
//exit:
	if(ret)printf("The disk has been parted\n");
	free(buffer);
	return ret;
}
/***************************************************************************************
 ** load the latest part info from system disk
 **************************************************************************************/
int block_get_part_info(struct part_sz *psz)
{
	if(psz==NULL){
		printf("Erro: Null pointer assigned when ask for part info\n");
		return -1;
	}
	int dev_id=storage_device();
	int blksz=vs_align(dev_id);
	int *buffer=(int *)malloc(blksz);  
	int ret=0;
	if(NULL==buffer){                       
		printf("Failed to malloc buffer\n");
		return -1;                          
	}         
	vs_assign_by_id(dev_id , 1);
	vs_read((uint8_t *) buffer, INFO_PART_CFG_BASE*blksz ,blksz,0);
	/*){   
		printf("failed to got disk partition state.\n");
		ret= -1;
		goto exit;
	} */                                                  
	ret=(int)memcpy(psz , buffer, (10+INFO_MAX_PARTITIONS)*sizeof(struct part_sz));
//exit:
	free(buffer);
	return ret;

}
#if 0
int oem_get_system_length(void)
{
	int *buffer=(int *)CONFIG_RESV_PTBUFFER;               
	memset((char *)buffer,0,512);                          
	if(oem_read_sys_disk((uint8_t *)buffer , 0 ,1)) return 0;

	return  buffer[CONFIG_SYSTEM_REAL_LEN];
}
int oem_set_system_length(int length)
{
	uint8_t *buffer=(uint8_t *)CONFIG_RESV_PTBUFFER;               
	memset((char *)buffer,0,512);                          
	if(oem_read_sys_disk((uint8_t *) buffer , 0 , 512)) return -1;

	buffer[CONFIG_SYSTEM_REAL_LEN]=length;

	if(oem_write_sys_disk(buffer , 0 , 512 , 512)) return -1;

	return 0;

}
#endif
#if 0
/********************************************************************************************
  This interface will only be used when enviroments be stored in sysdisk.
 *********************************************************************************************/
int oem_clear_env(void)
{
	uint8_t *buffer=(uint8_t *)CONFIG_RESV_PTBUFFER;
	memset((char *)buffer,0,CONFIG_ENV_SIZE);
	oem_write_sys_disk((uint8_t *) buffer , CONFIG_ENV_OFFSET , CONFIG_ENV_SIZE , CONFIG_ENV_SIZE);
	return 0;
}
#endif
#if 0
/*******************************************************************************************
  This interface will only be used when the system has hibernate feature
 *******************************************************************************************/
int oem_get_hibernatebase(void)
{
	return  CONFIG_SYS_DISK_KERNEL_OFFS+(CONFIG_KERNEL_PART_LEN<<11);
}
#endif
