/*
 * board.c - display board driver
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <dss_common.h>
#include "board.h"

#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[board]"
#define DSS_SUB2	""


extern int pt;
int board_type;


int board_init(void)
{
	char name[64] = {0};
	dss_trace("~\n");

	if (!item_exist(P_BOARD_TYPE)) {
		dss_err("#item# Lack of board type item.\n");
		return -1;
	}
	item_string(name, P_BOARD_TYPE, 0);
	
	if (pt== PRODUCT_MID) {
		if (strcmp(name, "lcd1") == 0) {
			board_type = BOARD_LCD1;
			board_x15_mid_lcd1_init();
		}
		else if (strcmp(name, "lcd2") == 0) {
			board_type = BOARD_LCD2;
			//board_x15_mid_lcd2_init();
		}
		else if (strcmp(name, "mipi") == 0) {
			board_type = BOARD_MIPI;
			//board_x15_mid_mipi_init();
		}
		else if (strcmp(name, "i80") == 0) {
			board_type = BOARD_I80;
			board_x15_mid_i80_init();
		}
		else {
			dss_err("#item# Invalid board type item %s.\n", name);
			return -1;
		}
	}
	else if (pt == PRODUCT_BOX) {
		if (strcmp(name, "lcd1") == 0) {
			board_type = BOARD_LCD1;
			board_box_lcd1_init();
		}
		else if (strcmp(name, "lcd2") == 0) {
			board_type = BOARD_LCD2;
			//board_x15_mid_lcd2_init();
		}
		else if (strcmp(name, "mipi") == 0) {
			board_type = BOARD_MIPI;
			//board_x15_mid_mipi_init();
		}
		else {
			dss_err("#item# Invalid board type item %s.\n", name);
			return -1;
		}

		// TODO: ...
	}

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display board driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
