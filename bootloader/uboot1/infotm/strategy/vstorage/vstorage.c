/***************************************************************************** 
** vstorage.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Virtual storage layer for iROM
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0  XXX 06/18/2011 XXX	Initialized by warits
*****************************************************************************/

#include <common.h>
#include <nand.h>
#include <flash.h>
#include <ramdisk.h>
#include <eeprom.h>
#include <mmc.h>
#include <vstorage.h>
#include <malloc.h>
#include <udc.h>
#include <net.h>
#include <usbapi.h>
#include <sata.h>


const struct vstorage vs_list[] = {
	/* boot nandflash */
	{	.name  = "bnd",
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| DEVICE_BOOT0 | DEVICE_ID(DEV_BND)
			| DEVICE_ERASABLE,
#if !defined(MODE) || (MODE != SIMPLE)
		.align = bnd_vs_align,
		.read  = bnd_vs_read,
		.write = bnd_vs_write,
		.reset = bnd_vs_reset,
		.erase = bnd_vs_erase,
		.isbad = bnd_vs_isbad,
		.capacity = NULL,
#endif
	},
	/* normal nandflash */
	{	.name  = "nnd",
		.flag  = DEVICE_BURNDST | DEVICE_ID(DEV_NAND)
			| DEVICE_ERASABLE,
#if !defined(MODE) || (MODE != SIMPLE)
		.align = nnd_vs_align,
		.read  = nnd_vs_read,
		.write = nnd_vs_write,
		.reset = nnd_vs_reset,
		.erase = nnd_vs_erase,
		.scrub = nnd_vs_scrub,
		.isbad = nnd_vs_isbad,
		.capacity = NULL,
#endif
	},
	/* normal nandflash */
	{	.name  = "fnd",
		.flag  = DEVICE_BURNDST | DEVICE_ID(DEV_FND)
			| DEVICE_ERASABLE,
#if !defined(MODE) || (MODE != SIMPLE)
		.align = fnd_vs_align,
		.write = fnd_vs_write,
		.read  = fnd_vs_read,
		.reset = fnd_vs_reset,
		.erase = fnd_vs_erase,
		.scrub = fnd_vs_scrub,
		.isbad = fnd_vs_isbad,
		.capacity = NULL,
#endif
	},
	/* eeprom */
	{
		.name = "eeprom",
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| DEVICE_BOOT0 | DEVICE_ID(DEV_EEPROM),
		.align = e2p_vs_align,
		.read = e2p_vs_read,
		.write = e2p_vs_write,
		.reset = e2p_vs_reset,
		.capacity = NULL,
	},
	/* spi flash */
	{
		.name = "flash",
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| DEVICE_BOOT0 | DEVICE_ID(DEV_FLASH)
			| DEVICE_ERASABLE,
		.align = flash_vs_align,
		.read = flash_vs_read,
		.write = flash_vs_write,
		.reset = flash_vs_reset,
		.erase = flash_vs_erase,
		.capacity = NULL,
	},
	/* sata2 hard disk */
	{
		.name = "hd",
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| DEVICE_ID(DEV_HD),
		.capacity = NULL,
	},
	/* mmc channel 0 */
	{
		.name  = "mmc0",
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| DEVICE_BURNSRC | DEVICE_ID(DEV_MMC(0)),
		.align = mmc0_vs_align,
		.read  = mmc0_vs_read,
		.write = mmc0_vs_write,
		.reset = mmc0_vs_reset,
		.capacity = mmc0_vs_capacity,
	},
	/* mmc channel 1 */
	{
		.name  = "mmc1",
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| DEVICE_BURNSRC | DEVICE_ID(DEV_MMC(1)),
		.align = mmc1_vs_align,
		.read  = mmc1_vs_read,
		.write = mmc1_vs_write,
		.reset = mmc1_vs_reset,
		.capacity = mmc1_vs_capacity,
	},
	/* mmc channel 2 */
	{
		.name  = "mmc2",
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| DEVICE_BURNSRC | DEVICE_ID(DEV_MMC(2)),
		.align = mmc2_vs_align,
		.read  = mmc2_vs_read,
		.write = mmc2_vs_write,
		.reset = mmc2_vs_reset,
		.capacity = mmc2_vs_capacity,
	},
	/* u disk */
	{
		.name = "udisk0",
#ifdef CONFIG_ENABLE_UDISK
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| /* DEVICE_BURNSRC | */DEVICE_ID(DEV_UDISK(0)),
		.align = udisk0_vs_align,
		.read = udisk0_vs_read,
		.write = udisk0_vs_write,
		.reset = udisk0_vs_reset,
		.capacity = udisk0_vs_capacity,
#endif
	},
	{
		.name = "udisk1",
#ifdef CONFIG_ENABLE_UDISK
		.flag  = DEVICE_BOOTABLE | DEVICE_BURNDST
			| /* DEVICE_BURNSRC | */DEVICE_ID(DEV_UDISK(1)),
		.align = udisk1_vs_align,
		.read = udisk1_vs_read,
		.write = udisk1_vs_write,
		.reset = udisk1_vs_reset,
		.capacity = udisk1_vs_capacity,
#endif
	},
	/* udc */
	{
		.name  = "udc",
		.flag  = /* DEVICE_BURNSRC | */DEVICE_ID(DEV_UDC),
#if !defined(MODE) || (MODE != SIMPLE)
		.reset = udc_vs_reset,
		.align = udc_vs_align,
#endif
		.capacity = NULL,
	},
	/* ethernet */
	{
		.name  = "eth",
		.flag  = /* DEVICE_BURNSRC | */DEVICE_ID(DEV_ETH),
#if !defined(MODE) || (MODE != SIMPLE)
		.reset = eth_vs_reset,
		.align = eth_vs_align,
#endif
		.capacity = NULL,
	},
	/* ram */
	{
		.name  = "ram",
		.flag  = DEVICE_ID(DEV_RAM),
		.read  = ram_vs_read,
		.write = ram_vs_write,
		.reset = ram_vs_reset,
		.align = ram_vs_align,
		.capacity = NULL,
	}
};

const struct vstorage *vs_current = NULL;

static int id_valid(int id)
{
	if(id >= 0 && id < ARRAY_SIZE(vs_list))
	  return 1;
	return 0;
}

int vs_is_device(int id, const char *name)
{
	const struct vstorage *p = NULL;

	if(id == DEV_CURRENT)
	  p = vs_current;
	else if(id_valid(id))
	  p = &vs_list[id];

	if(p)
	  return !strncmp(name, p->name, 16);
	return 0;
}

int vs_is_assigned(void)
{
	return !!vs_current;
}

int vs_is_opt_available(int id, int opt)
{
	const struct vstorage *p;

	p = id_valid(id)?&vs_list[id]:vs_current;

	if(!p) return 0;
	switch(opt)
	{
		case VS_OPT_ALIGN:
			return !!p->align;
		case VS_OPT_READ:
			return !!p->read;
		case VS_OPT_WRITE:
			return !!p->write;
		case VS_OPT_RESET:
			return !!p->reset;
		case VS_OPT_ERASE:
			return !!p->erase;
		case VS_OPT_SCRUB:
			return !!p->scrub;
	}
	return 0;
}
int vs_reset(void)
{
	if(vs_current) {
		if (vs_current->reset)
			return vs_current->reset();
		else
			return -3;
	}

	return -2;
}

int vs_read(uint8_t *buf, loff_t start,
			size_t length, int extra)
{
	uint32_t align, prelen, postlen;
	uint8_t *tmp = NULL;
	int ret = 0, read = 0;
	if(!vs_current) return -2;
	if(!vs_current->read) return -3;
	if(!length) return 0;

	/* pass fnd request directly to lower layer */
	if(strncmp(vs_current->name, "fnd", 3) == 0)
		return vs_current->read(buf, start, length, extra);

	align  = vs_align(DEV_CURRENT);
	if(strncmp(vs_current->name, "nnd", 3) == 0) {
		length = (length + (align - 1)) & ~(align - 1);
		//printf("nnd length forced to: 0x%x\n", length);
	}

	prelen  = (start & (align - 1));
	if(prelen)
	  prelen = align - prelen;
	postlen = (length - prelen) & (align - 1);

//	printf("align: 0x%x, prelen: 0x%x, postlen: 0x%x, start: 0x%llx, len: 0x%x\n",
//				align, prelen, postlen, start, length);
	if(prelen || postlen) {
		/* alloc buffer to hanle not aligned data */
		tmp = malloc(align);
		if(!tmp)
		  panic("no buffer to handle not aligned reading request\n");
	}

	if(prelen) {
		/* read the first align B */
		ret = vs_current->read(tmp, start & ~(align - 1), align, extra);
		if(ret < 0) {
			read = ret;
			goto __read_end__;
		}
		memcpy(buf, tmp + align - prelen, prelen);
		buf    += prelen;
		start  += prelen;
		length -= prelen;
		read  += ret;
	}

	/* middle */
	length -= postlen;
	if(length) {
		ret = vs_current->read(buf, start, length, extra);
		if(ret < 0) {
			read = ret;
			goto __read_end__;
		}
		buf   += length;
		start += length;
		read  += ret;
	}

	if(postlen) {
		ret = vs_current->read(tmp, start, align, extra);
		if(ret < 0) {
			read = ret;
			goto __read_end__;
		}
		memcpy(buf, tmp, postlen);
		read  += ret;
	}

__read_end__:
	if(prelen || postlen)
	  free(tmp);
	return read;
}

int vs_write(uint8_t *buf, loff_t start,
			size_t length, int extra)
{
	/* zero is always ok */
	if(!length)
		return 0;

	if(vs_current) {
		if (vs_current->write)
			return vs_current->write(buf, start, length, extra);
		else 
			return -3;
	}

	return -2;
}

#if 0
int vs_w2(uint8_t *buf, loff_t start,
		size_t length, int extra)
{
	if(vs_current) {
		if (vs_current->w2)
			return vs_current->w2(buf, start, length, extra);
		else
			return -3;
	}

	return -2;
}
#endif

int vs_erase(loff_t start, uint64_t len)
{
	if(vs_current) { if (vs_current->erase)
			return vs_current->erase(start, len);
		else
			return -3;
	}

	return -2;
}

int vs_isbad(loff_t start)
{
	if(vs_current) { if (vs_current->isbad)
			return vs_current->isbad(start);
		else
			return -3;
	}

	return -2;
}

int vs_scrub(loff_t start, uint64_t len)
{
	if(vs_current) {
		if (vs_current->scrub)
			return vs_current->scrub(start, len);
		else
			return -3;
	}

	return -2;
}

int vs_special(void * arg)
{
	if(vs_current) {
		if (vs_current->special)
			return vs_current->special(arg);
		else
			return -3;
	}

	return -2;
}

int vs_align(int id)
{
	const struct vstorage *p = NULL;
	if(id_valid(id))
	  p = &vs_list[id];
	else if(id == DEV_CURRENT)
	  p = vs_current;

	if(p && p->align)
	  return p->align();

	return 0;
}
loff_t vs_capacity(int id)
{   
	const struct vstorage *p = NULL;
	if(id_valid(id))
		p = &vs_list[id];
	else if(id == DEV_CURRENT)
		p = vs_current;

	if(p && p->capacity)
		return p->capacity();

	return 0;
}   
int vs_assign_by_name(const char *name, int reset)
{
	int i;
	printf("vs assign . name=%s\n", name);

	vs_current = NULL;
	for(i = 0; i < ARRAY_SIZE(vs_list); i++)
	  if(strncmp(vs_list[i].name, name, 16) == 0) {
		  vs_current = &vs_list[i];
		  return (reset)? vs_reset(): 0;
	  }

	printf("vs assign failed. name=%s\n", name);
	return -1;
}

int vs_assign_by_id(int id, int reset)
{
	printf("vs assign . id=%d\n", id);
	vs_current = NULL;
	if(id_valid(id)) {
		vs_current = &vs_list[id];
		return (reset)? vs_reset(): 0;
	}

	printf("vs assign failed. id=%d\n", id);
	return -1;
}

int vs_device_id(const char *name)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(vs_list); i++)
	{
		if(strncmp(name, vs_list[i].name, 16) == 0)
		  return i;
	}

	return DEV_NONE;
}

int vs_device_bootable(int id)
{
	if(id_valid(id)) 
	  return !!(vs_list[id].flag & DEVICE_BOOTABLE);
	return 0;
}

int vs_device_burnsrc(int id)
{
	if(id_valid(id)) 
	  return !!(vs_list[id].flag & DEVICE_BURNSRC);
	return 0;
}

int vs_device_burndst(int id)
{
	if(id_valid(id)) 
	  return !!(vs_list[id].flag & DEVICE_BURNDST);
	return 0;
}

int vs_device_boot0(int id)
{
	if(id_valid(id)) 
	  return !!(vs_list[id].flag & DEVICE_BOOT0);
	return 0;
}

const char * vs_device_name(int id) {
	if(id_valid(id)) 
	  return vs_list[id].name;

	return NULL;
}

int vs_device_erasable(int id)
{
	if(id_valid(id)) 
	  return !!(vs_list[id].flag & DEVICE_ERASABLE);

	return 0;
}

int vs_device_count(void)
{
	return ARRAY_SIZE(vs_list);
}

