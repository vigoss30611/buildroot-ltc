/***************************************************************************** 
** iuw_udc.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: IUW udc wrap to link with VS.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0  XXX 06/18/2011 XXX	Initialized by warits
*****************************************************************************/

#include <common.h>
#include <vstorage.h>
#include <malloc.h>
#include <iuw.h>
#include <net.h>
#include <udc.h>

int udc_vs_reset(void)
{
	int ret;

	iuw_set_dev(TDEV_UDC);

	/* try connect via udc */
	ret = iuw_connect();
	if(ret)
	  printf("try connect via udc failed (%d)\n", ret);

	return ret;
}

int udc_vs_align(void) {
	return 0x400;
}

int udc_vs_special(void *x)
{
	int ret;
	iuw_set_dev(TDEV_UDC);
	ret = iuw_connect();

	if(ret) {
		printf("try connect via udc failed (%d)\n", ret);
		return ret;
	} else
       printf("connect with PC is successful\n"); 

	ret = iuw_server();

	/* service finished */
	iuw_close();
	return ret;
}

