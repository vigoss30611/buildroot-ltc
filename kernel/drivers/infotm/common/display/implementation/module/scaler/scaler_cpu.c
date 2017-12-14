/*
 * scaler_cpu.c - display cpu scaler driver
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

#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[scaler]"
#define DSS_SUB2	"cpu"

int dss_scaler_cpu(int src_vic, int dst_vic, char* src_buf, char* dst_buf)
{
#if 0
	int lineSize, desBufSize, desLineSize, i, j, k;
	dtd_t src_dtd;
	dtd_t dst_dtd;

	vic_fill(&src_dtd, src_vic, 60000);
	vic_fill(&dst_dtd, dst_vic, 60000);

	lineSize = (32 * src_dtd.mHActive) / 8;
	desBufSize = (dst_dtd.mHActive * 32) * 4 * dst_dtd.mVActive;
	desLineSize = (dst_dtd.mHActive * 32) * 4;

	double rateW = (double)src_dtd.mHActive / dst_dtd.mHActive;
	double rateH = (double)src_dtd.mVActive / dst_dtd.mVActive;
	
	for (i = 0; i < dst_dtd.mVActive; i++) {
		int tH = (int)(rateH * i);
		int tH1 = min(tH + 1, src_dtd.mVActive - 1);
		float u = (float)(rateH * i - tH);

		for (j = 0; j < dst_dtd.mHActive; j++) {
			int tW = (int)(rateW * i);
			int tW1 = min(tW + 1, src_dtd.mHActive - 1);
			float v = (float)(rateW * j - tW);

			for (k = 0; k < 3; k++) {
				 dst_buf[i * desLineSize + (j * 32) / 8 + k] = 
					 (1 - u) * (1 - v) * srcBuf[tH * lineSize + tW * 32 / 8 + k] +
					 (1 - u) * v * src_buf[tH1 * lineSize + tW * 32 / 8 + k] +
					 u * (1 - v) * src_buf[tH * lineSize + tW1 * 32 / 8 + k] +
					 u * v * src_buf[tH1 * lineSize + tW1 * 32 / 8 + k];
			}

		}
	}
#endif

	return 0;
}
