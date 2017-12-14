

#include <linux/types.h>
#include <asm/io.h>
#include <common.h>
//#include <imap.h>

int usb_cpu_init(void)
{
	unsigned int dwTmp;

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
	writel(0x3,USB_SRST);

	udelay(20);
	
	dwTmp = readl(PAD_CFG);
	dwTmp |= 0xe;
	writel(dwTmp,PAD_CFG);
	
	udelay(500);
	
	writel(0x7,USB_SRST);

	udelay(20);

	writel(0xf,USB_SRST);	

	// config otg
	dwTmp = 0x2;
	writel(dwTmp,OTGL_BCWR);
	return 0;
}

int usb_cpu_stop(void)
{
	writel(0xf,USB_SRST);
	writel(0,OTGL_BCWR);
	return 0;
}

void usb_cpu_init_fail(void)
{
	writel(0xf,USB_SRST);
	writel(0,OTGL_BCWR);	
}
