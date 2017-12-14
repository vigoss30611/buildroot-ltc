#ifndef __IMAPX_SYSMGR_H__
#define __IMAPX_SYSMGR_H__


#define IMAP_PLL_EN     (1<<31)
#define IMAP_PLLVAL(_o,_d,_m) ((_o) << 12 | (_d) << 7 | ((_m)))

#define PLL_LOCKED					(SYSMGR_BASE_REG_PA+0x004)
#define PLL_OCLKSEL					(SYSMGR_BASE_REG_PA+0x008)
#define PLL_CLKSEL					(SYSMGR_BASE_REG_PA+0x00C)
#define APLL_CFG					(SYSMGR_BASE_REG_PA+0x010)
#define DPLL_CFG					(SYSMGR_BASE_REG_PA+0x014)
#define EPLL_CFG					(SYSMGR_BASE_REG_PA+0x018)
#define DIV_CFG0					(SYSMGR_BASE_REG_PA+0x01C)
#define DIV_CFG1					(SYSMGR_BASE_REG_PA+0x020)
#define HCLK_MASK					(SYSMGR_BASE_REG_PA+0x024)
#define PCLK_MASK					(SYSMGR_BASE_REG_PA+0x028)
#define SCLK_MASK					(SYSMGR_BASE_REG_PA+0x02C)
#define CLKOUT0_CFG					(SYSMGR_BASE_REG_PA+0x030)		
#define CLKOUT1_CFG					(SYSMGR_BASE_REG_PA+0x034)
#define CPUSYNC_CFG					(SYSMGR_BASE_REG_PA+0x038)
#define DIV_CFG2					(SYSMGR_BASE_REG_PA+0x03C)
#define USB_SRST					(SYSMGR_BASE_REG_PA+0x040)
#define PERSIM_CFG					(SYSMGR_BASE_REG_PA+0x044)
#define PAD_CFG						(SYSMGR_BASE_REG_PA+0x048)
#define GPU_CFG						(SYSMGR_BASE_REG_PA+0x04C)
#define DIV_CFG3					(SYSMGR_BASE_REG_PA+0x05C)
#define DIV_CFG4					(SYSMGR_BASE_REG_PA+0x060)

#define SW_RST            			     (SYSMGR_BASE_REG_PA+0x100)
#define MEM_CFG           		 	     (SYSMGR_BASE_REG_PA+0x104)
#define MEM_SWAP          			     (SYSMGR_BASE_REG_PA+0x108)
#define BOOT_MD          	 		     (SYSMGR_BASE_REG_PA+0x10C)
#define RST_ST            			     (SYSMGR_BASE_REG_PA+0x110)
#define PORT_PS_CFG       			     (SYSMGR_BASE_REG_PA+0x114)
#define IVA_PS_CFG	         	   	     (SYSMGR_BASE_REG_PA+0x118)			
#define SM_PS_CFG	         		     (SYSMGR_BASE_REG_PA+0x11C)	
#define MP_ACCESS_CFG				     (SYSMGR_BASE_REG_PA+0x120)			
#define GPOW_CFG          			     (SYSMGR_BASE_REG_PA+0x200)
#define WP_MASK           			     (SYSMGR_BASE_REG_PA+0x204)
#define POW_STB         			     (SYSMGR_BASE_REG_PA+0x208)
#define WP_ST             			     (SYSMGR_BASE_REG_PA+0x20C)
#define NPOW_CFG          			     (SYSMGR_BASE_REG_PA+0x210)
#define POW_ST            			     (SYSMGR_BASE_REG_PA+0x214)
#define MD_ISO           			     (SYSMGR_BASE_REG_PA+0x218)
#define MD_RST            			     (SYSMGR_BASE_REG_PA+0x21C)
#define AHBP_RST            		     (SYSMGR_BASE_REG_PA+0x220)
#define APBP_RST            		     (SYSMGR_BASE_REG_PA+0x224)
#define AHBP_EN            			     (SYSMGR_BASE_REG_PA+0x228)

#define INFO0            			     (SYSMGR_BASE_REG_PA+0x22C)
#define INFO1            			     (SYSMGR_BASE_REG_PA+0x230)
#define INFO2            			     (SYSMGR_BASE_REG_PA+0x234)
#define INFO3            			     (SYSMGR_BASE_REG_PA+0x238)

#define SLP_ORST                        	     (SYSMGR_BASE_REG_PA+0x300)
#define SLP_GPADAT                      	     (SYSMGR_BASE_REG_PA+0x304)
#define SLP_GPAPUD                      	     (SYSMGR_BASE_REG_PA+0x308)
#define SLP_GPACON                      	     (SYSMGR_BASE_REG_PA+0x30C)
#define SLP_GPBDAT                      	     (SYSMGR_BASE_REG_PA+0x310)
#define SLP_GPBPUD                      	     (SYSMGR_BASE_REG_PA+0x314)
#define SLP_GPBCON                      	     (SYSMGR_BASE_REG_PA+0x318)
#define SLP_GPODAT                      	     (SYSMGR_BASE_REG_PA+0x31C)
#define SLP_GPOPUD                      	     (SYSMGR_BASE_REG_PA+0x320)
#define SLP_GPOCON                      	     (SYSMGR_BASE_REG_PA+0x324)
#define GPA_SLP_CTRL                    	     (SYSMGR_BASE_REG_PA+0x328)
#define GPB_SLP_CTRL                    	     (SYSMGR_BASE_REG_PA+0x32C)
#define GPO_SLP_CTRL                    	     (SYSMGR_BASE_REG_PA+0x330)
#define RTC_INT_CFG                     	     (SYSMGR_BASE_REG_PA+0x334)                                                          

/*clock div CFG bit*/
#define IMAP_DIV_CFG0_PCLK	(0x3<<16)
#define IMAP_DIV_CFG0_HCLKX2	(0xf<<8)
#define IMAP_DIV_CFG0_HCLK	(0xf<<4)
#define IMAP_DIV_CFG0_CPU	(0xf<<0)


/*pll locked status bit field*/
#define IMAP_EPLL_LOCKED	(1<<2)
#define IMAP_DPLL_LOCKED	(1<<1)
#define IMAP_APLL_LOCLED	(1<<0)

/*Pll output selection*/
#define IMAP_EPiLL_OCLK_SEL	(1<<2)		//pll put
#define IMAP_DPLL_OCLK_SEL	(1<<1)		//pll out
#define IMAP_APLL_OCLK_SEL	(1<<0)		//pll out

/*pll clock source select*/
#define IMAP_EPLL_SEL		(1<<2)		//external clock
#define IMAP_DPLL_SEL		(1<<1)		//external clock
#define IMAP_APLL_SEL		(1<<0)		//external clock

/*PLL configuration*/
#define IMAP_PLL_EN_BIT	(1<<31)	
#define IMAP_PLL_BYPASS_EN_BIT	(1<<15)

/*GPU Ram Clock Mode CFG*/
#define IMAP_RAMCLKGDSB		(1<<0)		//memory clock gate disable

/*soft reset control bit*/
#define IMAP_SWRST		(0x6565)

/*HCLK gating control*/
#define IMAP_CLKCON_HCLK_IVA_AXI	(1<<24)
#define IMAP_CLKCON_HCLK_DSP_AHB	(1<<23)
#define IMAP_CLKCON_HCLK_MEMPOOL	(1<<22)
#define IMAP_CLKCON_HCLK_SDIO2		(1<<21)
#define IMAP_CLKCON_HCLK_SDIO1		(1<<20)
#define IMAP_CLKCON_HCLK_GPS		(1<<19)
#define IMAP_CLKCON_HCLK_ETH		(1<<18)
#define IMAP_CLKCON_HCLK_NAND		(1<<17)
#define IMAP_CLKCON_HCLK_NORFLASH	(1<<16)
#define IMAP_CLKCON_HCLK_INTC		(1<<15)
#define IMAP_CLKCON_HCLK_SDIO0		(1<<14)
#define IMAP_CLKCON_HCLK_CF_IDE		(1<<13)
#define IMAP_CLKCON_HCLK_APB		(1<<12)
#define IMAP_CLKCON_HCLK_OTG		(1<<11)
#define IMAP_CLKCON_HCLK_MONITOR_DEBUG	(1<<10)
#define IMAP_CLKCON_HCLK_EMIF		(1<<9)
#define IMAP_CLKCON_HCLK_USBH		(1<<8)
#define IMAP_CLKCON_HCLK_CAMIF		(1<<7)
#define IMAP_CLKCON_HCLK_IDS		(1<<6)
#define IMAP_CLKCON_HCLK_DMA		(1<<5)
#define IMAP_CLKCON_HCLK_DSP		(1<<4)
#define IMAP_CLKCON_HCLK_GPU		(1<<3)
#define IMAP_CLKCON_HCLK_VDEC		(1<<2)
#define IMAP_CLKCON_HCLK_VENC		(1<<1)

/*PCLK gating control */
#define IMAP_CLKCON_PCLK_RTC		(1<<20)
#define IMAP_CLKCON_PCLK_SPI		(1<<19)
#define IMAP_CLKCON_PCLK_SLAVESSI	(1<<18)
#define IMAP_CLKCON_PCLK_MASTERSSI2	(1<<17)
#define IMAP_CLKCON_PCLK_PS2_1		(1<<16)
#define IMAP_CLKCON_PCLK_PS2_0		(1<<15)
#define IMAP_CLKCON_PCLK_KB		(1<<14)
#define IMAP_CLKCON_PCLK_UART3		(1<<13)
#define IMAP_CLKCON_PCLK_UART2		(1<<12)
#define IMAP_CLKCON_PCLK_UART1		(1<<11)
#define IMAP_CLKCON_PCLK_UART0		(1<<10)
#define IMAP_CLKCON_PCLK_GPIO		(1<<9)
#define IMAP_CLKCON_PCLK_MASTERSSI1	(1<<8)
#define IMAP_CLKCON_PCLK_MASTERSSI0	(1<<7)
#define IMAP_CLKCON_PCLK_AC97		(1<<6)
#define IMAP_CLKCON_PCLK_IIS		(1<<5)
#define IMAP_CLKCON_PCLK_IIC1		(1<<4)
#define IMAP_CLKCON_PCLK_IIC0		(1<<3)
#define IMAP_CLKCON_PCLK_WATCHDOG	(1<<2)
#define IMAP_CLKCON_PCLK_PWM_TIEMR	(1<<1)
#define IMAP_CLKCON_PCLK_CMN_TIMER	(1<<0)

/*SCLK gating control*/
#define IMAP_CLKCON_SCLK_BOOTSRAM	(1<<17)
#define IMAP_CLKCON_SCLK_IDS_EITF	(1<<15)
#define IMAP_CLKCON_SCLK_IDS		(1<<14)
#define IMAP_CLKCON_SCLK_USB_PHY	(1<<13)
#define IMAP_CLKCON_SCLK_SD2		(1<<12)
#define IMAP_CLKCON_SCLK_SDIO1		(1<<11)
#define IMAP_CLKCON_SCLK_SDIO0		(1<<10)
#define IMAP_CLKCON_SCLK_TV		(1<<9)
#define IMAP_CLKCON_SCLK_TIM1		(1<<8)
#define IMAP_CLKCON_SCLK_TIM0		(1<<7)
#define IMAP_CLKCON_SCLK_CAM		(1<<6)
#define IMAP_CLKCON_SCLK_OTG		(1<<5)
#define IMAP_CLKCON_SCLK_USBH		(1<<4)
#define IMAP_CLKCON_SCLK_IIS		(1<<3)
#define IMAP_CLKCON_SCLK_GPU		(1<<2)
#define IMAP_CLKCON_SCLK_UART		(1<<0)

#if 0
/*Wakeup source Mask*/

static inline unsigned int
imapx200_get_pll(unsigned long pllval, unsigned long baseclk)
{
	unsigned long odiv, ddiv, mdiv;

	/*To prevent overflow in calculation*/
	baseclk /= 1000;

	odiv = (pllval & (0x3<<12))>>12;
	ddiv = (pllval & (0x1f<<7))>>7;
	mdiv = (pllval & (0x7f<<0))>>0;
	return (baseclk*(2*(mdiv + 1))) / ((ddiv + 1)*(2<<(odiv)))*1000;
}
#endif
#endif /*__IMAPX_SYSMGR_H__*/
