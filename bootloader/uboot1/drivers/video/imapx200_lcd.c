#include <linux/types.h>
#include <asm/io.h>
#include <common.h>

#include <lcd.h>

#include <spi.h>
#include <oem_func.h>
#include <malloc.h>


struct vidinfo panel_info;

short console_col;
short console_row;

int lcd_color_fg;
int lcd_color_bg;

int lcd_line_length;
unsigned short BATTERY_VAL;

/* Frame buffer memory information */
void *lcd_base;	/* Start of framebuffer memory */
void *lcd_console_address;	/* Start of console buffer */

#ifdef CONFIG_IMAPX200_SPI
extern ssize_t spi_read (uchar *addr, int alen, uchar *buffer, int len);
#endif

static void lcd_panel_enable(void)
{
	unsigned long reg_val, gpio_addr, gpio_num;

	if(!getenv("lcdpgpx"))
	{
		gpio_addr = CONFIG_LCD_PANEL_GPIO;
		gpio_num = CONFIG_LCD_PANEL_GPIO_Bit;
	}
	else
	{
		gpio_addr = simple_strtoul(getenv("lcdpgpx"), NULL, 16);
		gpio_num  = simple_strtoul(getenv("lcdpgpxbit"), NULL, 16);

		if((gpio_addr & 0xffff0000) != 0x20e10000)
		{
			printf("Invilade LCD GPIO address!\n");
		}

		if(gpio_num > 0xf)
		{
			printf("LCD GPIO bit exceed 15!\n");
		}
	}

	reg_val = readl(gpio_addr + 4);
	reg_val &= ~(0x3 << (gpio_num * 2));
	reg_val |= 0x1 << (gpio_num * 2);
	writel(reg_val, gpio_addr + 4);

	writel(readl(gpio_addr) | (1 << (gpio_num)), gpio_addr);

	//Set RGB IF Data Line and Control Singal Line
	writel(0xaaaaaaaa, GPMCON);
	writel(0x2aaaaaa, GPNCON);
	writel(0, GPMPUD);
	writel(0, GPNPUD);

	udelay(10000);

#ifdef CONFIG_LCD_RESET_GPIO
	reg_val = readl(CONFIG_LCD_RESET_GPIO + 4);
	reg_val &= ~(3 << (CONFIG_LCD_RESET_GPIO_Bit * 2));
	reg_val |= (1 << (CONFIG_LCD_RESET_GPIO_Bit * 2));
	writel(reg_val, CONFIG_LCD_RESET_GPIO + 4);

	writel(readl(CONFIG_LCD_RESET_GPIO) &
	   ~(1 << CONFIG_LCD_RESET_GPIO_Bit),
	   CONFIG_LCD_RESET_GPIO);
	udelay(100000);
	writel(readl(CONFIG_LCD_RESET_GPIO) |
	   (1 << CONFIG_LCD_RESET_GPIO_Bit), CONFIG_LCD_RESET_GPIO);
	udelay(10000);
#endif

}

void lcd_panel_disable(void)
{
	/* Not supported here */
}

ulong calc_fbsize (void)
{
	ulong size;
	int line_length = (panel_info.vl_col * NBITS(panel_info.vl_bpix)) / 8;

	size = line_length * panel_info.vl_row;

	return size;
}

void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
{
#if 0
	unsigned long val;
	
	if (regno < 256)
	{
		/* Only Support RGB565 */
		val = ((red >> 0) & 0xf800);
		val |= ((green >> 5) & 0x07e0);
		val |= ((blue >> 11) & 0x001f);

		writel(val, IMAP_OVCW0PAL + 0x4 * regno);
	}
#endif
}

void lcd_disable(void)
{
	unsigned long tmp;
	
	//Enable Video Output
	tmp = readl(IMAP_LCDCON1);
	writel(tmp & ~IMAP_LCDCON1_ENVID_ENABLE, IMAP_LCDCON1);
}

void lcd_enable(void)
{
	unsigned long tmp;
	
	//Enable Video Output
	tmp = readl(IMAP_LCDCON1);
	writel(tmp | IMAP_LCDCON1_ENVID_ENABLE, IMAP_LCDCON1);
}

void lcd_win_onoff(int win, int onoff)
{
	if (onoff)
		writel(readl(IMAP_OVCW0CR + 0x80 * win) | IMAP_OVCWxCR_ENWIN_ENABLE, (IMAP_OVCW0CR + 0x80 * win));
	else
		writel(readl(IMAP_OVCW0CR + 0x80 * win) &~ IMAP_OVCWxCR_ENWIN_ENABLE, (IMAP_OVCW0CR + 0x80 * win));
}

void lcd_win_alpha_set(int win, int val)
{
	unsigned long reg_val;

	reg_val = IMAP_OVCWxPCCR_ALPHA1_R(val) | IMAP_OVCWxPCCR_ALPHA1_G(val) | IMAP_OVCWxPCCR_ALPHA1_B(val);
	if ((val >= 0 && val < 16) && win > 0)
		writel(reg_val, IMAP_OVCW1PCCR + (win - 1) * 0x80);
}

#if 0
static void lcd_show_logo(void *fb_base)
{
	int i, nX, nY;
	unsigned short *pFB;
	unsigned short *pDB1;	
	pFB = (unsigned short *)lcd_base;
	pDB1 = (unsigned short *)logo_300x120;
	
	memset(lcd_base, 0xff, IMAPFB_HRES * IMAPFB_VRES * 2);

	for (i = 0; i < IMAPFB_HRES * IMAPFB_VRES; i++)
	{
	     	nX = i % IMAPFB_HRES;
		nY = i / IMAPFB_HRES;

		if((nX >= ((IMAPFB_HRES - 300) / 2)) && (nX < ((IMAPFB_HRES + 300) / 2)) && (nY >= ((IMAPFB_VRES - 120) / 2)) && (nY < ((IMAPFB_VRES + 120) / 2)))
			*pFB++ = *pDB1++;
		else
			*pFB++;
	}
}
#endif

#define PWMCLK 37000
#define CLK_DIV		2

#if 0
static void pwm_timer_setup(unsigned int percent)
{
	unsigned long reg_val;
	unsigned long temp;

#if 0
	reg_val = readl(GPFCON);
	reg_val &= ~(0x3<<16);
	reg_val |= 0x2<<16;
	writel(reg_val,GPFCON);
#endif

	u32 clk = get_PCLK();
//	printf("clk is %d\n",clk);
	reg_val = clk/PWMCLK/CLK_DIV;
	writel(reg_val,TCNTB2);
	temp = reg_val*percent/100;
	writel(temp, TCMPB2);
//	printf("tcntb1111 is %d\n",readl(TCNTB2));
//	printf("tcmpb1111111 is %d\n",readl(TCMPB2));

#if 0
	writel(500,TCNTB2);
	reg_val = 500*percent/100;
	writel(reg_val,TCMPB2);
#endif
	reg_val = readl(TCFG1);
	reg_val |= 0x1<<11;
	writel(reg_val, TCFG1);
	
//	writel(0,TCFG0);

	reg_val = readl(TCON);
	reg_val &= ~(0x1<<15);
	writel(reg_val, TCON);

	reg_val = readl(TCON);
	reg_val |= 0x1<<13;
	writel(reg_val, TCON);

	reg_val = readl(TCON);
	reg_val |= 0x1<<15;
	writel(reg_val, TCON);

	reg_val = readl(TCON);
	reg_val &= ~(0x1<<14);
	writel(reg_val,TCON);

	reg_val = readl(TCON);
	reg_val |= 0x1<<12;
	writel(reg_val,TCON);
}
#endif

static void backlight_power_on(void)
{
	unsigned long reg_val, gpio_addr, gpio_num, tout;

	if(!getenv("lcdpgpx"))
	{
		gpio_num = CONFIG_LCD_BL_GPIO_Bit;
		gpio_addr = CONFIG_LCD_BL_GPIO;
		tout = CONFIG_LCD_TOUT_PIN;
	}
	else
	{
		gpio_addr = simple_strtoul(getenv("lcdbgpx"), NULL, 16);
		gpio_num  = simple_strtoul(getenv("lcdbgpxbit"), NULL, 16);
		tout  = simple_strtoul(getenv("pwmout")  , NULL, 16);

		if((gpio_addr & 0xffff0000) != 0x20e10000)
		{
			printf("Invilade LCD GPIO address!\n");
		}

		if(gpio_num > 0xf)
		{
			printf("LCD GPIO bit exceed 15!\n");
		}

		if(tout > 3)
		{
			printf("Invilade PWM out pin\n");
			tout = CONFIG_LCD_TOUT_PIN;
		}

	}

	/* Turn on backlight */
	reg_val = readl(gpio_addr + 4);
	reg_val &= ~(0x3 << (gpio_num * 2));
	reg_val |= 0x1 << (gpio_num * 2);
	writel(reg_val, gpio_addr + 4);

	writel(readl(gpio_addr) | (1 << (gpio_num)), gpio_addr);

	udelay(3000);
	lcd_enable();
#ifdef CONFIG_LCD_BL_DELAY
	udelay(CONFIG_LCD_BL_DELAY);
#else
	udelay(160000);
#endif

	/* Turn on pwmout */
	reg_val = readl(GPFCON);
	reg_val &= ~(0x3<<(tout*2 + 0xc));

#ifdef CONFIG_LCD_ENBLCTRL
	  reg_val |= 0x2<<(tout*2 + 0xc);
	  writel(reg_val,GPFCON);
#else
	  reg_val |= 0x1<<(tout*2 + 0xc);
	  writel(reg_val,GPFCON);
#ifdef CONFIG_LCD_BL_LOW
	  writel(readl(GPFDAT) & ~(1 << (tout + 6)), GPFDAT);
#else
	  writel(readl(GPFDAT) | (1 << (tout + 6)), GPFDAT);
#endif
#endif
}

void backlight_power_off(void)
{
	/* Not supported here */
	unsigned long gpio_addr, gpio_num, tout;
	uint32_t reg_val;

	if(!getenv("lcdpgpx"))
	{
		gpio_num = CONFIG_LCD_BL_GPIO_Bit;
		gpio_addr = CONFIG_LCD_BL_GPIO;
		tout = CONFIG_LCD_TOUT_PIN;
	}
	else
	{
		gpio_addr = simple_strtoul(getenv("lcdbgpx"), NULL, 16);
		gpio_num  = simple_strtoul(getenv("lcdbgpxbit"), NULL, 16);
		tout  = simple_strtoul(getenv("pwmout")  , NULL, 16);

		if((gpio_addr & 0xffff0000) != 0x20e10000)
		{
			printf("Invilade LCD GPIO address!\n");
		}

		if(gpio_num > 0xf)
		{
			printf("LCD GPIO bit exceed 15!\n");
		}

		if(tout > 3)
		{
			printf("Invilade PWM out pin\n");
			tout = CONFIG_LCD_TOUT_PIN;
		}

	}

	writel(readl(gpio_addr) & ~(1 << (gpio_num)), gpio_addr);

	writel(readl(GPFDAT) & ~(1 << (tout + 6)), GPFDAT);

	reg_val = readl(GPFCON);
	reg_val &= ~(0x3<<(tout*2 + 0xc));

#ifdef CONFIG_LCD_ENBLCTRL
	  reg_val |= 0x2<<(tout*2 + 0xc);
	  writel(reg_val,GPFCON);
#else
	  reg_val |= 0x1<<(tout*2 + 0xc);
	  writel(reg_val,GPFCON);
#ifdef CONFIG_LCD_BL_LOW
	  writel(readl(GPFDAT) | (1 << (tout + 6)), GPFDAT);
#else
	  writel(readl(GPFDAT) & ~(1 << (tout + 6)), GPFDAT);
#endif
#endif


}

void lcd_ctrl_init(void *lcd_base)
{
	unsigned long reg_val, a=0;
#ifdef CONFIG_IMAPX200_SPI
	uchar *addr = malloc(sizeof(uchar));
	uchar *buffer = malloc(sizeof(uchar));
#endif

	//Set motor GPIO
#ifdef CONFIG_MOTOR_GPIO
	reg_val = readl(CONFIG_MOTOR_GPIO + 4);
	reg_val &= ~(0x3 << (CONFIG_MOTOR_GPIO_NUM * 2));
	reg_val |= 0x1 << (CONFIG_MOTOR_GPIO_NUM * 2);
	writel(reg_val, CONFIG_MOTOR_GPIO + 4);

	reg_val = readl(CONFIG_MOTOR_GPIO);
#ifdef CONFIG_MOTOR_GPIO_LOW
	reg_val &= ~(0x1 << CONFIG_MOTOR_GPIO_NUM);
#else
	reg_val |= (0x1 << CONFIG_MOTOR_GPIO_NUM);
#endif
	writel(reg_val, CONFIG_MOTOR_GPIO);
#endif
	panel_info.vl_col = IMAPFB_HRES;
	panel_info.vl_row = IMAPFB_VRES;
	panel_info.vl_bpix = LCD_BPP;

#ifdef CONFIG_DIV_CFG4_VAL
	writel(CONFIG_DIV_CFG4_VAL, DIV_CFG4);
#else
	writel((4 << 10) | (2 << 8) | (4 << 2) | (2 << 0), DIV_CFG4);
#endif
	writel(readl(SCLK_MASK) & ~((1 << 14) | (1 << 15)), SCLK_MASK);

	//Disable Video Output
	lcd_disable();

	//Set VCLK Divider
	reg_val = readl(IMAP_LCDCON1);
	reg_val &= IMAP_LCDCON1_CLKVAL_MSK;
#ifdef CONFIG_LCDCON1_CLKVAL
	reg_val |= IMAP_LCDCON1_CLKVAL(CONFIG_LCDCON1_CLKVAL);
#else
	reg_val |= IMAP_LCDCON1_CLKVAL(1);
#endif

	//Set LCDCON1
	reg_val |= IMAP_LCDCON1_PNRMODE_TFTLCD;
	writel(reg_val, IMAP_LCDCON1);

	//Set LCDCON2 & LCDCON3 & LCDCON4
	reg_val = IMAP_LCDCON2_VBPD(IMAPFB_VBP - 1) | IMAP_LCDCON2_LINEVAL(IMAPFB_VRES - 1)
		| IMAP_LCDCON2_VFPD(IMAPFB_VFP - 1) | IMAP_LCDCON2_VSPW(IMAPFB_VSW - 1);
	writel(reg_val, IMAP_LCDCON2);
	reg_val = IMAP_LCDCON3_HBPD(IMAPFB_HBP - 1) | IMAP_LCDCON3_HOZVAL(IMAPFB_HRES - 1) | IMAP_LCDCON3_HFPD(IMAPFB_HFP - 1);
	writel(reg_val, IMAP_LCDCON3);
	reg_val = IMAP_LCDCON4_HSPW(IMAPFB_HSW - 1);
	writel(reg_val, IMAP_LCDCON4);

	//Set LCDCON5
#ifdef CONFIG_LCDCON5
	reg_val = CONFIG_LCDCON5;
#else
	reg_val = (0x6 << 24) | IMAP_LCDCON5_INVVCLK_FALLING_EDGE | IMAP_LCDCON5_INVVLINE_INVERTED | IMAP_LCDCON5_INVVFRAME_INVERTED
		| IMAP_LCDCON5_INVVD_NORMAL | IMAP_LCDCON5_INVVDEN_NORMAL | IMAP_LCDCON5_INVPWREN_NORMAL | IMAP_LCDCON5_PWREN_ENABLE;
#endif
	/* See if it is in env */
	if(getenv("lcdcon5"))
	  reg_val = simple_strtoul(getenv("lcdcon5"), NULL, 16);

	writel(reg_val, IMAP_LCDCON5);

	//Set OVCDCR
	writel(IMAP_OVCDCR_IFTYPE_RGB, IMAP_OVCDCR);

	//Set OVCPCR
	reg_val = IMAP_OVCPCR_W3PALFM_RGB565 | IMAP_OVCPCR_W2PALFM_RGB565 | IMAP_OVCPCR_W1PALFM_RGB565 | IMAP_OVCPCR_W0PALFM_RGB565;
	writel(reg_val, IMAP_OVCPCR);

	//Set OVCW0CR && OVCW1CR
	reg_val = IMAP_OVCWxCR_BUFSEL_BUF0 | IMAP_OVCWxCR_BUFAUTOEN_DISABLE | IMAP_OVCWxCR_BITSWP_DISABLE | IMAP_OVCWxCR_BIT2SWP_DISABLE
		| IMAP_OVCWxCR_BIT4SWP_DISABLE | IMAP_OVCWxCR_BYTSWP_DISABLE | IMAP_OVCWxCR_HAWSWP_DISABLE
		| IMAP_OVCWxCR_BPPMODE_16BPP_RGB565 | IMAP_OVCWxCR_ENWIN_DISABLE;
	writel(reg_val, IMAP_OVCW0CR);
	reg_val = IMAP_OVCWxCR_BUFSEL_BUF0 | IMAP_OVCWxCR_BUFAUTOEN_DISABLE | IMAP_OVCWxCR_BITSWP_DISABLE | IMAP_OVCWxCR_BIT2SWP_DISABLE
		| IMAP_OVCWxCR_BIT4SWP_DISABLE | IMAP_OVCWxCR_BYTSWP_DISABLE | IMAP_OVCWxCR_HAWSWP_DISABLE | IMAP_OVCWxCR_ALPHA_SEL_1
		| IMAP_OVCWxCR_BLD_PIX_PLANE | IMAP_OVCWxCR_BPPMODE_16BPP_RGB565 | IMAP_OVCWxCR_ENWIN_DISABLE;
	writel(reg_val, IMAP_OVCW1CR);

	//Set OVCW0PCAR & OVCW0PCBR && OVCW1PCAR & OVCW1PCBR
	reg_val = IMAP_OVCWxPCAR_LEFTTOPX(0) | IMAP_OVCWxPCAR_LEFTTOPY(0);
	writel(reg_val, IMAP_OVCW0PCAR);
	writel(reg_val, IMAP_OVCW1PCAR);
	reg_val = IMAP_OVCWxPCBR_RIGHTBOTX(IMAPFB_HRES - 1) | IMAP_OVCWxPCBR_RIGHTBOTY(IMAPFB_VRES - 1);
	writel(reg_val, IMAP_OVCW0PCBR);
	writel(reg_val, IMAP_OVCW1PCBR);

	//Set OVCW0B0SAR & OVCW1B0SAR
	writel((u_long)lcd_base, IMAP_OVCW0B0SAR);
	writel(CONFIG_RESV_LCD_BASE_1, IMAP_OVCW1B0SAR);

	//Set OVCW0VSSR && OVCW1VSSR
	writel(IMAPFB_HRES, IMAP_OVCW0VSSR);
	writel(IMAPFB_HRES, IMAP_OVCW1VSSR);

	//Set OVCW0CMR && OVCW1CMR
	writel(IMAP_OVCWxCMR_MAPCOLEN_DISABLE, IMAP_OVCW0CMR);
	writel(IMAP_OVCWxCMR_MAPCOLEN_DISABLE, IMAP_OVCW1CMR);

	//Close Win1 && Win0
	writel(readl(IMAP_OVCW0CR) &~ IMAP_OVCWxCR_ENWIN_ENABLE, IMAP_OVCW0CR);
	writel(readl(IMAP_OVCW1CR) &~ IMAP_OVCWxCR_ENWIN_ENABLE, IMAP_OVCW1CR);


#ifdef CONFIG_IMAPX200_SPI
	spi_init();
	BATTERY_VAL = spi_read(addr,1,buffer,8);
	
	//hardware power_off
	if (readl(POW_STB) & 0x4) 
	{
		udelay(1000000);
		power_off();
	}

#ifdef CONFIG_POWER_GPIO
	if(!(__raw_readl(CONFIG_POWER_GPIO) & CONFIG_POWER_GPIO_NUM) )
	{
#endif
		if (BATTERY_VAL < CONFIG_LOW_BATTERY_VAL && BATTERY_VAL > CONFIG_LOGO_BATTERY_VAL)
		{   
#ifdef    CONFIG_GZIP_BATT_FILE
			oem_show_batt();
#endif
			a=1;
		}
		else if(BATTERY_VAL <= CONFIG_LOGO_BATTERY_VAL)
		{
			power_off();
		}
		else
		{
			//Fill Win0 with oem logo
#ifndef CONFIG_DISABLE_LOGO
			oem_show_logo();
#endif
		}
#ifdef CONFIG_POWER_GPIO
	}
	else
	{
#ifndef CONFIG_DISABLE_LOGO
		//Fiil Win0 with oem logo
		oem_show_logo();
#endif
	}
#endif

#else
 #ifndef CONFIG_DISABLE_LOGO
	oem_show_logo();
#endif
#endif

	backlight_power_off();
	lcd_panel_disable();

	//LCD Power Supply
	lcd_panel_enable();

#ifdef CONFIG_LCD_EXTRA_GPIO
	uint32_t xxtm;
	xxtm = readl(CONFIG_LCD_EXTRA_GPIO + 4);
	xxtm &= ~(3 << (CONFIG_LCD_EXTRA_GPIO_Bit * 2));
	xxtm |= (1 << (CONFIG_LCD_EXTRA_GPIO_Bit * 2));
	writel(xxtm, CONFIG_LCD_EXTRA_GPIO + 4);

	writel(readl(CONFIG_LCD_EXTRA_GPIO) |
	   (1 << CONFIG_LCD_EXTRA_GPIO_Bit), CONFIG_LCD_EXTRA_GPIO);
#endif

	backlight_power_on();
#ifdef CONFIG_MOTOR_GPIO
#ifdef CONFIG_MOTOR_GPIO_LOW
	writel(readl(CONFIG_MOTOR_GPIO) | (0x1 << CONFIG_MOTOR_GPIO_NUM),
	   CONFIG_MOTOR_GPIO);
#else
	writel(readl(CONFIG_MOTOR_GPIO) & ~(0x1 << CONFIG_MOTOR_GPIO_NUM),
	   CONFIG_MOTOR_GPIO);
#endif
#endif
	if(a==1)
	{
		udelay(5000000);
		power_off();
	}
}



