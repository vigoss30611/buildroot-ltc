/*
 * irq.c - display controller interrupt driver
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
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <dss_common.h>
#include <ids_access.h>

#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[controller]"
#define DSS_SUB2	"[irq]"

irqreturn_t dss_irq_handler(int irq, void *data);

static int ids_irq[2] = {0};
static int irq_error;
static int swap_irq_type[2] = {0};
static int vsync_irq_type[2] = {0};
static int swap_require[2] = {0};
static int vsync_require[2] = {0};
static struct completion swap[2];
static struct completion vsync[2];
static int idsx_priv[2] = {0, 1};

int ids_irq_set(int idsx, int irq)
{
	ids_irq[idsx] = irq;
	return 0;
}

int ids_irq_initialize(int idsx)
{
	int ret;
	int irq = ids_irq[idsx];
	char name[10] = {0};
	static int irq_init[2] = {0};
	dss_trace("ids%d, irq %d\n", idsx, irq);

	if (!irq) {
		dss_err("ids%d zero irq.\n", idsx);
		return -1;
	}

	init_completion(&swap[idsx]);
	init_completion(&vsync[idsx]);

	sprintf(name, "ids%d", idsx);
	if (!irq_init[idsx]) {
		ret = request_irq(irq, dss_irq_handler, IRQF_DISABLED, "ids", &idsx_priv[idsx]);
		if (ret) {
			dss_err("request irq%d failed %d\n", idsx, ret);
			return -1;
		}
		irq_init[idsx] = 1;
	}
	ids_writeword(idsx, IDSINTMSK, 0);

	return 0;
}

extern void osd_set_parameter(void);
irqreturn_t dss_irq_handler(int irq, void *data)
{
	int idsx = *((int *)data);
	unsigned int status = 0, vclkfsr;
	unsigned int vclkfac, cdown, rfrm_num, clkval;

	status = ids_readword(idsx, IDSINTPND);
	if (swap_require[idsx] && (status & swap_irq_type[idsx])) {
		swap_require[idsx] = 0;
		dss_dbg("IDS%d LCDINT complete swap\n", idsx);
	}

	if (status & IRQ_LCDINT ||status & IRQ_TVINT){
		core.frame_cur_cnt++;
		core.cur_timestamp = jiffies_to_usecs(jiffies);
	}

	if (vsync_require[idsx] && (status & vsync_irq_type[idsx])) {
		vsync_require[idsx] = 0;
		complete(&vsync[idsx]);
		dss_dbg("IDS%d %s complete vsync\n", idsx,
			((vsync_irq_type[idsx] == IRQ_TVINT) ? "TVINT":"LCDINIT"));
	}

	if (status & (1 << IDSINTPND_VCLKINT)) {
		dss_err("VCLKINT: VCLK frequency auto change.\n");
	}

	if (status & (1 << IDSINTPND_OSDERR)) {
		dss_err("OSDERR: OSD occur error, need to reset.\n");
	}

	if (irq_error) {
		if (status & (1 << IDSINTPND_OSDW0))
			dss_err("OSDW0: OSD Win0 Timeslot synchronized interrupt.\n");
		if (status & (1 << IDSINTPND_OSDW1))
			dss_err("OSDW1: OSD Win1 Timeslot synchronized interrupt.\n");
	}

	vclkfsr = ids_readword(idsx, LCDVCLKFSR);
	vclkfac = vclkfsr & 0x1;
	if (vclkfac) {
		cdown = (vclkfsr >> 24) & 0xF;
		rfrm_num = (vclkfsr >> 16) & 0x3F;
		clkval = (vclkfsr >> 4) & 0x3FF;
		dss_info("IDS%d: insufficient bandwidth. CDOWN = 0x%X, "
				"RFRM_NUM = 0x%X, CLKVAL = 0x%X\n",
				idsx, cdown, rfrm_num, clkval);
		ids_writeword(idsx, LCDVCLKFSR, vclkfsr);	// clear VCLKFAC

		ids_write(0, OVCDCR, OVCDCR_UpdateReg, 1, 0);
		if (core.lcd_interface == DSS_INTERFACE_LCDC ||
			core.lcd_interface == DSS_INTERFACE_SRGB) {
			dss_info("IDS%d: Setting LCDC CLKVAL = 0x%X ", idsx,clkval);
			ids_write(idsx, LCDCON1 , 0, 1, 0);
			ids_write(idsx, LCDCON1 , 8, 10, clkval);
			ids_write(idsx, LCDCON1 , 0, 1, 1);
		} else if (core.lcd_interface == DSS_INTERFACE_TVIF) {
			dss_info("IDS%d: Setting TVIF CLKVAL = 0x%X ", idsx,clkval);
			ids_write(idsx, IMAP_TVICR, 31, 1, 0);
			ids_write(idsx, IMAP_TVCCR, 0, 8, clkval);
			ids_write(idsx, IMAP_TVICR, 31, 1, 1);
		}
		ids_write(0, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	}

	ids_writeword(idsx, IDSSRCPND, status);
	ids_writeword(idsx, IDSINTPND, status);

	return IRQ_HANDLED;
}

int wait_vsync_timeout(int idsx, int irqtype, unsigned long timeout)
{
	int ret;

	vsync_require[idsx] = 1;
	vsync_irq_type[idsx] = irqtype;
	dss_trace("ids%d, irqtype = 0x%02X, timeout = %ld\n", idsx, irqtype, timeout);

	ret = wait_for_completion_interruptible_timeout(&vsync[idsx], timeout);
	if (!ret) {
		dss_trace("IDS%d vsync irq type 0x%X %s\n",
			idsx, irqtype, (ret == -ERESTARTSYS? "interrupted": "time out"));
		return -1;
	}
	dss_trace("ids%d, irqtype = 0x%02X, wait successfully \n", idsx, irqtype);
	return 0;
}

int wait_swap_timeout(int idsx, int irqtype, unsigned long timeout)
{
	dss_trace("ids%d, irqtype 0x%02X, timeout %ld\n", idsx, irqtype, timeout);
	swap_require[idsx] = 1;
	swap_irq_type[idsx] = irqtype;
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display interrupt driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
