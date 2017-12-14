/*
 * scaler_ids.c - display ids scaler driver
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
#include <dss_common.h>
#include <implementation.h>
#include <ids_access.h>
#include <controller.h>
#include <mach/items.h>

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[scaler]"
#define DSS_SUB2	"ids"

int dss_scaler_ids(int src_vic, int dst_vic)
{
	int scale_w;
	int scale_h;
	int scaled_width;
	int scaled_height;	
	dtd_t src_dtd;
	dtd_t dst_dtd;
	int ret;

	dss_err("using ids internal scaler src_vic %d dst_vic %d\n", src_vic, dst_vic);

	ret = vic_fill(&src_dtd, src_vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", src_vic, ret);
		return ret;
	}

	ret = vic_fill(&dst_dtd, dst_vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", dst_vic, ret);
		return ret;
	}

	scaled_width = dst_dtd.mHActive & (~0x3);
	scaled_height = dst_dtd.mVActive & (~0x3);	
	if ((dst_dtd.mHActive - scaled_width) % 4  || (dst_dtd.mVActive - scaled_height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", scaled_width, scaled_height);
		return -1;
	}

	ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 0);
	ids_write(1, OVCDCR, 10, 1, 0);
	ids_write(1, OVCDCR, 22, 1, 0);
	ids_writeword(1, 0x100C, (src_dtd.mVActive-1) << 12 | (src_dtd.mHActive-1));
	ids_writeword(1, 0x5100, (src_dtd.mVActive-1) << 16 | (src_dtd.mHActive-1));
	ids_writeword(1, 0x5104, (scaled_height-1) << 16 | (scaled_width-1));
	ids_writeword(1, 0x5108, (0) << 16 | (0));

	scale_w = src_dtd.mHActive*1024/dst_dtd.mHActive;
	scale_h = src_dtd.mVActive*1024/dst_dtd.mVActive;

	ids_writeword(1, 0x510C, (scale_h<< 16) | scale_w | (0<<30) | (0<<14));

	return 0;
}
