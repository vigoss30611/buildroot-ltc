/***************************************************************************** 
** iuw_base.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** ** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: IUW protocal base.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0  XXX 08/01/2011 XXX	Initialized by warits
*****************************************************************************/

#include <common.h>
#include <vstorage.h>
#include <malloc.h>
#include <iuw.h>
#include <ius.h>
#include <hash.h>
#include <net.h>
#include <udc.h>
#include <led.h>
#include <nand.h>
#include <cdbg.h>
#include <crypto.h>
#include <sparse.h>
#include <irerrno.h>
#include <storage.h>
#include <yaffs.h>

#define MAX_BURN_BUFFER 0x40000

struct part_iuw {
	char name[16];
	int  iuwtype;
};

struct part_iuw p_iuw[] = {
	{ "misc", IUW_CMD_MISC },
	{ "system", IUW_CMD_SYSM },
	{ "cache", IUW_CMD_CACH },
	{ "user", IUW_CMD_USER },
};

int (*iuw_get)(uint8_t *, int) = NULL;
int iuw_remain = 0;

const char *iuw2part(int d) {
	int i;

	for (i = 0; i < ARRAY_SIZE(p_iuw); i++)
	  if(p_iuw[i].iuwtype == d)
		return p_iuw[i].name;

	return NULL;
}

#define EXTRA_BUFFER_SIZE 0x40000
#if 0
static int iuw_sparse_data(void *buf)
{
	int step = EXTRA_BUFFER_SIZE, ret;

	if(!iuw_remain) {
		printf("no data for sparse.\n");
		return 0;
	}

	ret = iuw_get(buf, min(iuw_remain, step));
	iuw_remain -= min(step, iuw_remain);

	return ret;
}
#endif

static int iuw_get_page(uint8_t *buf)
{
	if(!iuw_remain) {
		printf("page data end.\n");
		return -1;
	}

	iuw_get(buf, 2064);
	iuw_remain -= 2064;

	return 0;
}

int iuw_extra(void *pack, int (*tget)(uint8_t *, int))
{
	uint8_t *buf;
	int id_o, size, step = EXTRA_BUFFER_SIZE;
	int ret = 0, opt = iuw_cmd_cmd(pack);
	loff_t offs;

	buf = malloc(EXTRA_BUFFER_SIZE);
	if(!buf) {
		printf("burn extra alloc buffer failed.\n");
		return -1;
	}

	id_o = storage_device();
	offs = storage_offset(iuw2part(opt));
	size = storage_size(iuw2part(opt));

	if(!offs || !size) {
		printf("%s: get part info failed. offs=0x%llx, size=0x%x\n",
					__func__, offs, size);
		ret = -2;
		goto __out;
	}

	/* this will be treat as a sparse image */
	ret =  iuw_shake_hand(IUW_SUB_STEP, step);
	if(ret) {
		printf("did not got ready from PC before sending data.\n");
		return -2;
	}

	iuw_remain = iuw_cmd_len(pack);
	iuw_get    = tget;

	if(id_o == DEV_NAND)
	  ret = yaffs_burn(offs, iuw_cmd_max(pack),
				  iuw_get_page);
	else
	  //ret = sparse_burn(id_o, offs, iuw_remain, step,
		//		  iuw_sparse_data);

	if(ret == 0)
	  /* final hand shake */
	  ret = iuw_shake_hand(0, 0);

__out:
	free(buf);
	return ret;
}

