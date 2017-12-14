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
#include <lowlevel_api.h>

#include "ehci.h"
#include "ehci-core.h"
/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(void)
{
	uint32_t val;
#if defined(CONFIG_BUILD_FPGA) 
	int i,j;
#endif	

	hccr = (struct ehci_hccr *)(0x27001000);
	hcor = (struct ehci_hcor *)((uint32_t) hccr
		+ HC_LENGTH(ehci_readl(&hccr->cr_capbase)));
	printf("x800 echi_init. %p\n", hcor);

	module_enable(USBH_SYSM_ADDR);
	/* configure phy rf clock*/
	val = readl(USBH_SYSM_ADDR + 0x20);
	val &= ~0x7;
	val |= 0x5;
	writel(val, USBH_SYSM_ADDR + 0x20);
	/* configure usb utmi+ data_len */
	val = readl(USBH_SYSM_ADDR + 0x24);
	val |= 0x1;
	writel(val, USBH_SYSM_ADDR + 0x24);
	/* configure port0 sleep mode to resume */
	val = readl(USBH_SYSM_ADDR + 0x30);
	val |= 0x1;
	writel(val, USBH_SYSM_ADDR + 0x30);
	/* configure port1 sleep mode to resume */
	val = readl(USBH_SYSM_ADDR + 0x74);
	val |= 0x1;
	writel(val, USBH_SYSM_ADDR + 0x74);

	/* reset usb-host module */
	val = readl(USBH_SYSM_ADDR);
	val |= 0x1f;
	writel(val, USBH_SYSM_ADDR);
	udelay(1);
	val &=~0x1e;
	writel(val, USBH_SYSM_ADDR);
	udelay(1);
	val &=~0x1;
	writel(val, USBH_SYSM_ADDR);
	udelay(1);
#if defined(CONFIG_BUILD_FPGA) 
	writel(0, USBH_SYSM_ADDR + 0x30);
	for(i=0;i<2000;i++)
	{
		writel(1, USBH_SYSM_ADDR + 0x34);
		for(j=0;j<100;j++);
		writel(0, USBH_SYSM_ADDR + 0X34);
		for(j=0;j<100;j++);
	}
#endif	
	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(void)
{
	uint32_t val;
	
	/* reset usb-host module */
	val = readl(USBH_SYSM_ADDR);
	val |= 0x1f;
	writel(val, USBH_SYSM_ADDR);
	udelay(1);
	val &=~0x1e;
	writel(val, USBH_SYSM_ADDR);
	udelay(1);
	val &=~0x1;
	writel(val, USBH_SYSM_ADDR);
	udelay(1);
	return 0;
}

