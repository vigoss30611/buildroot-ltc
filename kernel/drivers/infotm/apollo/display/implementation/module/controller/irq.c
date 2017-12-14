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

#define DSS_LAYER	"[implementation]"
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
extern int pt;


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

//	if (((pt == PRODUCT_MID) && (idsx == 1)) ||
//			((pt == PRODUCT_BOX) && (idsx == 0)))
//		return 0;                              

	init_completion(&swap[idsx]);
	init_completion(&vsync[idsx]);
	sprintf(name, "ids%d", idsx);
	if (!irq_init[idsx]) {
		ret = request_irq(irq, dss_irq_handler, IRQF_DISABLED, "ids", &idsx_priv[idsx]);	// IRQF_ONESHOT ?
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
	//dss_trace("ids%d, irq %d\n", idsx, irq);

	status = ids_readword(idsx, IDSINTPND);
	if (swap_require[idsx] && (status & swap_irq_type[idsx])) {
		swap_require[idsx] = 0;
		//dss_dbg("IDS%d LCDINT complete swap\n", idsx);
		complete(&swap[idsx]);
	}
	if (vsync_require[idsx] && (status & vsync_irq_type[idsx])) {
//        if(ids_readword(idsx, OVCDCR)& 0x800){//enable real vsync point
            vsync_require[idsx] = 0;
            //dss_dbg("IDS%d LCDINT complete vsync\n", idsx);
			osd_set_parameter();
            complete(&vsync[idsx]);
//        }
//			if (idsx == 1) {
//				ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
//			}
    }

	if (status & (1 << IDSINTPND_VCLKINT))
		;//dss_err("VCLKINT: VCLK frequency auto change.\n");
	if (status & (1 << IDSINTPND_OSDERR))
		dss_err("OSDERR: OSD occur error, need to reset.\n");
	if (irq_error) {
		if (status & (1 << IDSINTPND_OSDW0))
			dss_err("OSDW0: OSD Win0 Timeslot synchronized interrupt.\n");
		if (status & (1 << IDSINTPND_OSDW1))
			dss_err("OSDW1: OSD Win1 Timeslot synchronized interrupt.\n");
		if (status & (1 << IDSINTPND_OSDW2))
			dss_err("OSDW2: OSD Win2 Timeslot synchronized interrupt.\n");
	}

	vclkfsr = ids_readword(idsx, LCDVCLKFSR);
	vclkfac = vclkfsr & 0x1;
	if (vclkfac) {
		cdown = (vclkfsr >> 24) & 0xF;
		rfrm_num = (vclkfsr >> 16) & 0x3F;
		clkval = (vclkfsr >> 4) & 0x3FF;
		dss_err("IDS%d: insufficient bandwidth. CDOWN = 0x%X, RFRM_NUM = 0x%X, CLKVAL = 0x%X\n", 
				idsx, cdown, rfrm_num, clkval);
		ids_writeword(idsx, LCDVCLKFSR, vclkfsr);	// clear VCLKFAC
	}

	ids_writeword(idsx, IDSSRCPND, status);
	ids_writeword(idsx, IDSINTPND, status);

	return IRQ_HANDLED;
}

int wait_vsync_timeout(int idsx, int irqtype, unsigned long timeout)
{
	int ret;
	
	//dss_trace("ids%d, irqtype 0x%02X, timeout %ld\n", idsx, irqtype, timeout);
	vsync_require[idsx] = 1;
	vsync_irq_type[idsx] = irqtype;
	//ret = wait_for_completion_interruptible_timeout(&vsync[idsx], timeout);
	ret = wait_for_completion_timeout(&vsync[idsx], timeout);
	if (!ret) {
		dss_err("IDS%d vsync irq type 0x%X %s\n", idsx, irqtype, (ret == -ERESTARTSYS? "interrupted": "time out"));
		return -1;
	}
	return 0;
}

int wait_swap_timeout(int idsx, int irqtype, unsigned long timeout)
{
	//int ret;
	
	//dss_trace("ids%d, irqtype 0x%02X, timeout %ld\n", idsx, irqtype, timeout);
	swap_require[idsx] = 1;
	swap_irq_type[idsx] = irqtype;
#if 0  //workaround ,as DispSync vsync wait has problem
	ret = wait_for_completion_interruptible_timeout(&swap[idsx], timeout);
	if (!ret) {
		dss_err("IDS%d swap irq type 0x%X %s\n", idsx, irqtype, (ret == -ERESTARTSYS? "interrupted": "time out"));
		return -1;
	}
#endif
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display interrupt driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
