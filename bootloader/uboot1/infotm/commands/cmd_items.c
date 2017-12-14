/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <bootlist.h>
#include <malloc.h>
#include <cdbg.h>
#include <flash.h>
#include <burn.h>
#include <vstorage.h>
#include <ius.h>
#include <efuse.h>
#include <items.h>
#include <lowlevel_api.h>

int do_items(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	item_list(NULL);
	return 0;
}

U_BOOT_CMD(
	items, 1, 1, do_items,
	"items",
	"");

