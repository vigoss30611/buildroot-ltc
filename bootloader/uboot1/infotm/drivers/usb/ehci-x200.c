/*
 * (C) Copyright 2008, Michael Trimarchi <trimarchimichael@yahoo.it>
 *
 * Author: Michael Trimarchi <trimarchimichael@yahoo.it>
 * This code is based on ehci freescale driver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <usb.h>
#include <asm/io.h>

#include "ehci.h"
#include "ehci-core.h"
/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(void)
{
	unsigned int dwTmp;

	hccr = (struct ehci_hccr *)(0x20c81000);
	hcor = (struct ehci_hcor *)((uint32_t) hccr
			+ HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	printf("x200 echi_init. %p\n", hcor);

	// USB PORT 3 PHY SETTING HOST
	dwTmp = readl(MEM_CFG);
	dwTmp |= (1<<8);
	writel(dwTmp,MEM_CFG);
	
	// VBUS GPIO CONFIG
	dwTmp = readl(GPECON);
	dwTmp &=~(3<<30);
	dwTmp |= (2<<30);
	writel(dwTmp,GPECON);
	
	udelay(10);
	
	// config epll value	
	dwTmp = readl(DIV_CFG2);
	dwTmp &=~(3<<16);
	dwTmp |= (2<<16);
	dwTmp &=~(0x1f<<18);
	dwTmp |=(9<<18);
	writel(dwTmp,DIV_CFG2);

	// config usb gate
	dwTmp = readl(SCLK_MASK);
	dwTmp &=~(1<<13);
	dwTmp &=~(1<<4);
	writel(dwTmp,SCLK_MASK);

	// set usb power
	dwTmp = readl(PAD_CFG);
	dwTmp &=~(0xe);
	writel(dwTmp,PAD_CFG);

	// set usb reset
	writel(0x0,USB_SRST);

	udelay(20);
	
	dwTmp = readl(PAD_CFG);
	dwTmp |= 0xe;
	writel(dwTmp,PAD_CFG);
	
	udelay(500);
	
	writel(0x5,USB_SRST);

	udelay(20);

	writel(0xf,USB_SRST);	

	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(void)
{
	writel(0xf,USB_SRST);
	writel(0,OTGL_BCWR);
	return 0;
	return 0;
}

