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



//static u32 ids_ioread(int idsx, u32 addr);
//static void ids_iowrite(int idsx, u32 addr, u32 data);


struct resource *mem_source[2];
static unsigned int register_start[2] = {0};
static unsigned int register_size[2] = {0};
static unsigned int  base_addr[2] = {0};
static unsigned int  sysmgr_ids[2]  ={0};
//static unsigned int * gpio_ids_s = 0;
//static unsigned int * gpio_ids = 0;
//static int once = 0;

static char * ids_parent[2] = {"imap-ids0", "imap-ids1"};
static char * ids_clk[2][4] = {{"imap-ids.0", "imap-ids0-ods", "imap-ids0-eitf", "imap-ids0-tvif"}, 
						{"imap-ids.1", "imap-ids1-ods", "imap-ids1-eitf", "imap-ids1-tvif"},};
static unsigned long sysmgr_ids_base[2] = {SYSMGR_IDS0_BASE, SYSMGR_IDS1_BASE};

int ids_access_sysmgr_init(int idsx)
{
	struct clk * clksrc;
	int i, num;
	static int init = 0;
    int ubootLogoState = 1;//0,disabled; 1-enabled                                      

	/* ids1 will be operated separately */
	if(idsx)
		return 0;
		
	/* Enable clk */
	num = sizeof(ids_clk)/sizeof(char *)/2;
	for (i = 0; i < num; i++) {
		clksrc = clk_get_sys(ids_clk[idsx][i], ids_parent[idsx]);
		if (clksrc == NULL) {
			dss_err("get clk %s failed.\n", ids_clk[idsx][i]);
			return -1;
		}
		clk_prepare_enable(clksrc);
		dss_dbg("clk %s rate is %ld\n", ids_clk[idsx][i], clk_get_rate(clksrc));
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
	if ( 0 == ubootLogoState || init != 0)
		module_power_on_lite(sysmgr_ids_base[idsx]);

	init = 1;

#if 0
	// FPGA X9 tune
	sysmgr_ids[idsx] = (u32)ioremap_nocache(0x21e24000 + 0x4000*idsx, 30);
	writel(0xff, (u32 *)(sysmgr_ids[idsx] + 0x00));	// reset
	writel(0xff, (u32 *)(sysmgr_ids[idsx] + 0x04));	// clock enable
	writel(0x1, (u32 *)(sysmgr_ids[idsx] + 0x08));	// power on
	writel(0xff, (u32 *)(sysmgr_ids[idsx] + 0x0C));	// bus oe
	writel(0xff, (u32 *)(sysmgr_ids[idsx] + 0x18));	// io en
	writel(0xff, (u32 *)(sysmgr_ids[idsx] + 0x1C));	// clock enable1
	writel(0x0, (u32 *)(sysmgr_ids[idsx] + 0x00));	// out of reset

	msleep(1);
	iounmap((u32 *)sysmgr_ids[idsx]);
#endif
	
	return 0;
}

/* HDMI and MIPI use their own read/write function and address */
int ids_access_Initialize(int idsx, resource_size_t addr, unsigned int size)
{
	char str[20] = {0};
	dss_trace("ids%d, addr 0x%x, size 0x%x\n", idsx, (u32)addr, size);
	
	sprintf(str, "ids%d_base", idsx);
	mem_source[idsx] = request_mem_region(addr, size, str);
	if (!mem_source[idsx]) {
		dss_err("ids%d, request mem region failed.\n", idsx);
		return -1;
	}
	register_start[idsx] = addr;
	register_size[idsx] = size;
	base_addr[idsx] = (u32)ioremap_nocache(addr, size);
	if (!base_addr[idsx]) {
		dss_err("ids%d, ioremap failed.\n", idsx);
		return -1;
	}
	printk("base_addr[%d] = 0x%X\n", idsx, base_addr[idsx]);
	ids_access_sysmgr_init(idsx);

#if 0
	if (!once) {
		once = 1;
		dss_dbg("Initialize LCD1 GPIO.\n");
		gpio_ids_s = ioremap_nocache(0x21e09000, 4);
		if (!gpio_ids_s) {
			dss_err("ioremap 0x21e09000 failed.\n");
			return -1;
		}
		writel(0x00000000, (unsigned int)gpio_ids_s);		// IO MODE set
		gpio_ids = ioremap_nocache(0x21e09064, 100);
		if (!gpio_ids) {
			dss_err("ioremap 0x21e09064 failed.\n");
			return -1;
		}
		for (i = 0; i < 18; i++)
			writel(0x00000000, (unsigned int)gpio_ids + i*4);	//all io is set in function mode
	}
	
	msleep(1);
#endif
	return 0;
}

int ids_access_uninstall(void)
{
	dss_trace("~\n");
	iounmap((u32 *)base_addr[0]);
	iounmap((u32 *)base_addr[1]);
	release_mem_region(register_start[0], register_size[0]);
	release_mem_region(register_start[1], register_size[1]);
	return 0;
}

int ids_readword(int idsx, unsigned int addr)
{
	int ret;
	ret = readl((u32 *)(base_addr[idsx] + addr));
	//printk("readl: base[0x%X], addr[0x%X] = 0x%X\n", base_addr[idsx], addr, ret);
	return ret;
}

int ids_writeword(int idsx, unsigned int addr, unsigned int val)
{
	//printk("writel: base[0x%X], addr[0x%X] = 0x%X\n", base_addr[idsx], addr, val);
	writel(val, (u32 *)(base_addr[idsx] + addr));
	return 0;
}

int ids_write(int idsx, unsigned int addr, int bit, int width, unsigned int val)
{
	unsigned int tmp;
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
