/***************************************************************************** 
** XXX nand_spl/board/infotm/imapx/boot_main.c XXX
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: The first C function in PRELOADER.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 03/17/2010 XXX	Warits
*****************************************************************************/

#include <common.h>
#include <mmc.h>
#include <isi.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <preloader.h>

