/*
 * ids_access.c - display ids register access driver
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
#include <linux/clk.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <dss_common.h>

#include <mach/pad.h>


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[access]"
#define DSS_SUB2	""

static char * ids_parent = "imap-ids1";
static char * ids_clk[4] = {"imap-ids.1", "imap-ids1-ods", "imap-ids1-eitf", "imap-ids1-tvif"};
static void __iomem *base_addr;
extern int blanking;

int ids_access_sysmgr_init(void)
{
	struct clk * clksrc;
	int i, num;
	static int init = 0;
    int ubootLogoState = 1;//0,disabled; 1-enabled                                      

	/* Enable clk */
	num = sizeof(ids_clk)/sizeof(char *);
	for (i = 0; i < num; i++) {
		clksrc = clk_get_sys(ids_clk[i], ids_parent);
		if (clksrc == NULL) {
			dss_err("get clk %s failed.\n", ids_clk[i]);
			return -1;
		}
		clk_prepare_enable(clksrc);
		dss_dbg("clk %s rate is %ld\n", ids_clk[i], clk_get_rate(clksrc));
	}
	msleep(1);
	if(item_exist("config.uboot.logo")){                                              
             unsigned char str[ITEM_MAX_LEN] = {0};                                            
             item_string(str,"config.uboot.logo",0);                                       
             if(strncmp(str,"0",1) == 0 )                                           
                 ubootLogoState = 0;//disabled                                      
    }        
		
	/* Power on ids module */
	/* FIXME: this is incompitable with MIPI patch */
	if (!ubootLogoState || init != 0)
		module_power_on(SYSMGR_IDS1_BASE);

	init = 1;
	return 0;
}

/* HDMI and MIPI use their own read/write function and address */
int ids_access_Initialize(int idsx, struct resource *res)
{
	if (!res)
		return -1;

	base_addr = ioremap_nocache(res->start, res->end - res->start);
	if (!base_addr) {
		dss_err("%s: ioremap err\n", __func__);
		return -1;
	}
	ids_access_sysmgr_init();
	return 0;
}

int ids_access_uninstall(void)
{
	dss_trace("%s\n", __func__);
	return 0;
}

extern int sync_by_manual;
int ids_readword(int idsx, unsigned int addr)
{
	int ret;
	if (blanking == 1 && !sync_by_manual)
		return 0;
	ret = readl((u32 *)((uint32_t)base_addr + addr));
	return ret;
}

int ids_writeword(int idsx, unsigned int addr, unsigned int val)
{
	if (blanking == 1 && !sync_by_manual)
		return 0;
	writel(val, (u32 *)((uint32_t)base_addr + addr));
	return 0;
}

int ids_write(int idsx, unsigned int addr, int bit, int width, unsigned int val)
{
	unsigned int tmp;

	if (blanking == 1 && !sync_by_manual)
		return 0;
	tmp = ids_readword(idsx, addr);
	barrier();
	tmp &= ~(((0xFFFFFFFF << (32 - width)) >> (32 - width)) << bit);
	tmp |= val << bit;
	ids_writeword(idsx, addr, tmp);
	return 0;
}


MODULE_DESCRIPTION("InfoTM iMAP display ids register access driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
