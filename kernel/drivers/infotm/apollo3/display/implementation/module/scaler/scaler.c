/*
 * scaler.c - scaler driver
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
#include <scaler.h>

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[scaler]"
#define DSS_SUB2	""

static int get_scaler_mode(int idsx)
{
#if defined(CONFIG_MACH_IMAPX910)
	if (idsx == 1)
		return SCALER_IDS;
	else if (idsx == 0)
		return SCALER_CPU;
#elif defined(CONFIG_MACH_IMAPX15)
#if defined(CONFIG_INFOTM_G2D)
	return SCALER_G2D;
#else
	return SCALER_CPU;
#endif
#endif
	return SCALER_CPU;
}

int dss_scale(int idsx, int src_vic, int dst_vic, int num)
{
	int scaler_mode;

	assert_num(1);

	dss_trace("scaler %d -> %d\n", src_vic, dst_vic);

	scaler_mode = get_scaler_mode(idsx);

	if (scaler_mode == SCALER_IDS)
		dss_scaler_ids(src_vic, dst_vic);
	else if (scaler_mode == SCALER_G2D)
		//TODO ...
		dss_scaler_g2d(src_vic, dst_vic);
	else if (scaler_mode == SCALER_CPU)
		//TODO ...
		dss_scaler_cpu(src_vic, dst_vic);

	return 0;
}

