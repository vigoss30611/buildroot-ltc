

#include <linux/types.h>
#include <asm/io.h>
#include <common.h>
//#include <imap.h>

int usb_cpu_init(void)
{
	uint32_t val;
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

int usb_cpu_stop(void)
{
	return 0;
}

void usb_cpu_init_fail(void)
{
}

