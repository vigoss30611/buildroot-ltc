/***************************************************************************** 
** burn.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: iROM main burn process.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0  XXX 07/18/2011 XXX	Initialized by warits
*****************************************************************************/

#include <common.h>
#include <cdbg.h>
#include <isi.h>
#include <ius.h>
#include <hash.h>
#include <crypto.h>
#include <ius.h>
#include <bootlist.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <led.h>
#include <irerrno.h>
#include <burn.h>
#include <bootl.h>
#include <display.h>
#include <asm/arch/nand.h>
#include <linux/err.h>
//#define IR_MAX_BUFFER 0x8000

int ius_card_found = 0;

void set_ius_card_state(int val)
{
	ius_card_found = val;
}
int get_ius_card_state(void)
{
	return ius_card_found;
}

/*
*Author:summer
*Date:2014-5-23
*Function:Given a param,and by checking it,we can know whether ius card insterted?
*/
static int ius_check_head(struct ius_hdr *hdr)
{
	uint32_t crc_ori, crc_calc;

	if(hdr->magic != IUS_MAGIC) {
		printf("magic do not match3. 0x%x\n", hdr->magic);
		return -1;
	}

	printf("header check passed.\n");
	return 0;
}


/*
*(1)Author:summer
*(2)Date:2014-5-23
*(3)Function:If ius card inserted,return 1,else return 0
*(4)V1
*/
/*
int check_whether_ius_card_inserted(void)
{
	int card_exist = 0;
	int ret = 0;
	struct ius_hdr ius;
	//loop 3,read sizeof(ius_header) data,check it
	
	memset(&ius,0,sizeof(ius));
	vs_assign_by_id(DEV_MMC(1), 1);
	ret = vs_read((uint8_t *)&ius, IUS_DEFAULT_LOCATION, sizeof(struct ius_hdr), 0);
	if(ius_check_head(&ius) == 0) {
		printf("\n========== ius card found!\n");
		return 1;
	}
	
	return 0;
}
*/


int burn_image_loop(struct ius_desc *desc, int id_i, uint32_t channel,
			int id_o, uint32_t max, uint8_t *buf)
{
	loff_t	offs_i, offs_o;
	uint32_t length, remain, step, progress;
	int type, encrypt = 0, led = 0;
	struct isi_hdr *isi;

	/* just in case */
	if(!buf)
	  panic("not buffer provided.\n");

	if(id_i == id_o) {
		printf("burn source and target are the same device.\n");
		return -2;
	}

	offs_i  = ius_desc_offi(desc);
	offs_o  = ius_desc_offo(desc);
	type   = ius_desc_type(desc);
	remain = length = ius_desc_length(desc);

	if(vs_device_erasable(id_o) && (!vs_is_device(id_o, "fnd"))) {
		vs_assign_by_id(id_o, 0);
		vs_erase(offs_o, max);
	}

	/* set id_o and encrypt mark according to image type */
	vs_assign_by_id(id_i, 0);

	/* only these two kinds may contain bootloaders */
	if(type == IUS_DESC_STNB || type == IUS_DESC_STRW) {
		vs_read(buf, offs_i, ISI_SIZE, channel);
		isi = (struct isi_hdr *)buf;
		if(isi_check_header(isi) == 0) {
			if(isi_is_secure(isi)) {
				printf("data will be encrypted.\n");
				ss_aes_init(ENCRYPTION, NULL, __irom_ver);
				encrypt = 1;
			}
		}
	}

        step = IR_MAX_BUFFER; 

	//leeway = max - remain;
	printf("%d max=%x, bs=%x  %x \n", type, max, step, remain);

	progress = remain;
	while(remain > 0) { 
		/* if the header passed verification,
		 * read from device with this id must
		 * be successful, so here do not need
		 * to check the returning value of assign.
		 */
//		printf("desc info: id=%d, offs_i=%llx, offs_o=%llx, type=%d, remain=%u.\n",
//					id_o, offs_i, offs_o, type, remain);
		if((progress - remain) >> 20)
		  printf("%dM remains\n", (progress = remain) >> 20);

		led_ctrl(led++ > 0);
		if(led > 50) led -= 100;

		vs_assign_by_id(id_i, 0);
		vs_read(buf, offs_i, step, channel);

		/* decrypt data if needed */
		if(encrypt)
		  ss_aes_data(buf, buf, step);

		if(0 == vs_assign_by_id(id_o, 0)) {
			/* skip bad blocks for nandflash
			 * check bad block at every block starting baudary
			 * if a bad block is found, the offset is increased
			 * by a blocksize. There should always be a leeway leaves
			 * there for nandflashes. if the leeway used up
			 * the write process is failed.
			 *
			 * FIXME: if the offset start at a not block-aligned
			 * value, the first address will not be checked. so
			 * error may happnens if the first relavant block is
			 * bad. thus: XXX burns start at a not block-aligned
			 * address is strongly NOT recommanded!
			 */
#if 0
			if(skip && !(offs_o & (skip - 1))) {
				for(; vs_isbad(offs_o); ) {
					if(leeway > skip) {
						printf("burn skip bad block at 0x%llx\n", offs_o);
						offs_o += skip;
						leeway -= skip;
					} else {
						printf("no more good block availible\n");
						return -EBADBLK;
					}
				}
			}
#endif

			vs_write(buf, offs_o, step, 0);
		}

		buf    += ((type == IUS_DESC_EXEC)? step: 0);
		offs_o += step;
		offs_i += step;
		remain -= min(remain, step);
	}

	return 0;
}

/* ius header passed verification
 * load images from IUS package and do burn task
 * @channel is deprecated(Fri Sep 9, 2011)
 */
int burn_images(struct ius_hdr *hdr, int id, uint32_t channel)
{
	struct ius_desc *desc;
	int ret = 0, i, type, ido, count = ius_get_count(hdr);
	uint8_t *buf, *load = NULL;
	uint32_t max;

	/* alloc buffer for data transfering */
	buf = malloc(IR_MAX_BUFFER);
	if(!buf) {
		printf("alloc buffer for burning failed.\n");
		return -1;
	}

	printf("%d images found in package.\n", count);
	for(i = 0; i < count; i++) {
		desc = ius_get_desc(hdr, i);
		type = ius_desc_type(desc);
		max  = ius_desc_maxsize(desc);
		ido  = DEV_NONE;
		printf("processing image: %d, type=%d\n", i, type);
		if (ius_desc_mask(desc) == 0) {
			printf("skipped: image is masked as %d\n",
						ius_desc_mask(desc));
			continue ;
		}

		switch(type)
		{
			case IUS_DESC_STNB:
				//ido = vs_device_id("bnd");
				ido = vs_device_id("fnd");
				break;
			case IUS_DESC_STNR:
				//ido = vs_device_id("nnd");
				ido = vs_device_id("fnd");
				break;
			case IUS_DESC_STNF:
				ido = vs_device_id("fnd");
				break;
			case IUS_DESC_STRW:
				ido = ius_desc_id(desc);
				break;
			case IUS_DESC_EXEC:
				load = (uint8_t *)ius_desc_load(desc);
				printf("executalbe image load: 0x%x\n", (uint32_t)load);
				break;
			case IUS_DESC_CFG:
				break;
			default:
				burn_extra(desc, id);
				continue ;
		}

		if(ido != DEV_NONE)
		  /* reset the output device */
		  if(vs_assign_by_id(ido, 1)) {
			  printf("reset output device failed.\n");
			  ret = -EACCESS;
			  goto __loop_failed__;
		  }

		if(burn_image_loop(desc, id, channel, ido, max,
						((type == IUS_DESC_EXEC)? load: buf)) < 0) {
			printf("burn image: %d failed.\n", i);
			ret = -EFAILED;
			goto __loop_failed__;
		}

		/* load successful */
		if(type == IUS_DESC_EXEC)
		  cdbg_jump((void *)ius_desc_entry(desc));
		if(type == IUS_DESC_CFG)
		  cdbg_config_isi(buf);
	}

__loop_failed__:
	free(buf);
	return ret;
}

int burn(uint32_t mask)
{
	int ret, id;
	struct ius_hdr *ius;

	ius = malloc(sizeof(struct ius_hdr));
	if(!ius) {
		printf("alloc ius header space failed.\n");
		return -1;
	}

	//idb = boot_device();
	for(id = 0; id < vs_device_count(); id++)
	{
		if(!vs_device_burnsrc(id))
		  /* this is not a burn source */
		  continue;

		if(/* id == idb || */(mask & (1 << id)))
		  /* boot device will not be searched for burn source */
		  continue;


		if(vs_is_device(id, "eth") || vs_is_device(id, "udc")) {
			ret = vs_assign_by_id(id, 0);
			if(ret == 0) {
				ret = vs_special(NULL);
				if(ret == 0)
				  goto _finish;

				printf("iuw(%s) process failed (%d)\n", vs_device_name(id), ret);
				continue;
			}
		} else
		  ret = vs_assign_by_id(id, 1);

		if(ret) {
			printf("assign device(%s) failed (%d)\n", vs_device_name(id), ret);
			continue;
		}

		printf("read ius header addr=%x, loc=%x, size=%x, j=%d\n",
					(uint32_t)ius, IUS_DEFAULT_LOCATION,
					sizeof(struct ius_hdr), 0);

		/* 512k buffer is sufficient for vs to get data,
		 * thank god since nandflash is not burn source
		 */
		ret = vs_read((uint8_t *)ius, IUS_DEFAULT_LOCATION,
					sizeof(struct ius_hdr), 0);

		if(ius_check_header(ius) == 0) {
			printf("IUS found in device %d\n", id);
			goto _found;
		}
	}

	/* no ius found in all burn meadias */
	free(ius);
	return -1;

_found:
	cdbg_verify_burn_enable(0, NULL);
	//display_logo(-1);
	//burn_images(ius, id, 0);
	set_boot_type(BOOT_RECOVERY_IUS);
_finish:
	printf("images burn finished. enter shell\n");
	free(ius);
	/* temp __for__ uboot */
	//set_boot_type(BOOT_FACTORY_INIT);
	return 0;
}


