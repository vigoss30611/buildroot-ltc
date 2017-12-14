/*****************************************************************************
** nand_spl/board/infotm/imapx/denali.c
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: memory study code, seach for a proper eye to r/w DDR2.
**
** Author:
**     warits <warits.wang@infotmic.com.cn>
**
** Revision History:
** -----------------
** 1.1  08/23/2010
** 1.2  08/29/2010
*****************************************************************************/

#include <asm/arch/denali/denali_config.h>

void denali_set_eye(uint32_t sw, uint32_t data)
{
    uint32_t tmp;
    if(sw < 4)
    {
	    sw = sw + 66;
    }
    else
    {
	    sw = sw + 58;
    }
#ifndef __UBOOT_ASIC__
    writel(readl(CONFIG_LCD_BL_GPIO) & ~(1 << CONFIG_LCD_BL_GPIO_Bit),
	   CONFIG_LCD_BL_GPIO);
#endif
	// turn off denali
	tmp = readl(IMAP_LCDCON1);
	tmp &= ~0x1;
	writel(tmp, IMAP_LCDCON1);

	writel(0x00000101, DENALI_CTL_PA_09);

#ifdef __UBOOT_ASIC__
	tmp = readl(DENALI_CTL_PA_(sw));
	tmp &= ~(0x7f << 8);
	tmp |= (data & 0x7f) << 8;
	writel(tmp, DENALI_CTL_PA_(sw));
#else
	tmp = readl(DENALI_CTL_PA_62 + (sw - 62) * 4);
	tmp &= ~(0x7f << 8);
	tmp |= (data & 0x7f) << 8;
	writel(tmp, (DENALI_CTL_PA_62 + (sw - 62) * 4));
#endif

	// turn on deanli
	writel(0x01000101, DENALI_CTL_PA_09);

	udelay(0x100);
	tmp = readl(IMAP_LCDCON1);
	tmp |= 0x1;
	writel(tmp, IMAP_LCDCON1);
#ifndef __UBOOT_ASIC__
	writel(readl(CONFIG_LCD_BL_GPIO) | (1 << CONFIG_LCD_BL_GPIO_Bit),
	   CONFIG_LCD_BL_GPIO);


#endif
}



#ifdef __UBOOT_ASIC__
void denali_show_progress(int byte)
{
	return ;
}
#else
void denali_show_progress(int byte)
{
	int i;
#ifdef CONFIG_MOTOR_GPIO
	uint32_t tmp, gpio = CONFIG_MOTOR_GPIO, bit = CONFIG_MOTOR_GPIO_NUM;

	if(byte < 0)
	{
		tmp = readl(gpio + 4);
		tmp &= ~(0x3 << (bit * 2));
		tmp |= 0x1 << (bit * 2);
		writel(tmp, gpio + 4);
		tmp = readl(gpio);

		for(i = 0; i < 2; i++)
		{
			writel(tmp | (1 << bit), gpio);
			udelay(200000);
			writel(tmp & ~(1 << bit), gpio);
			udelay(300000);
		}

		return ;
	}

	tmp = readl(gpio);
	writel(tmp | (1 << bit), gpio);
	udelay(50000);
	writel(tmp & ~(1 << bit), gpio);
#else
#endif
}
#endif

#ifndef __UBOOT_ASIC__
void simple_lcd_init(void)
{

	writel(0xaaaaaaaa, 0x20E100c4);
	writel(0xaaaaaaaa, 0x20E100d4);


	writel(0, 0x20cd1000);
	writel(1, 0x20cd1004);
	writel(0x16, 0x20cd1080);
	writel(0, 0x20cd1084);
	writel(((IMAPFB_HRES - 1) << 16) |
	   ((IMAPFB_VRES - 1) << 0), 0x20cd1088);
	writel(CONFIG_RESV_LCD_BASE_1, 0x20cd108c);
	writel(IMAPFB_HRES, 0x20cd1094);
	writel(0x17, 0x20cd1080);
	writel((IMAP_LCDCON2_VBPD(IMAPFB_VBP - 1) |
		   IMAP_LCDCON2_LINEVAL(IMAPFB_VRES - 1) |
		   IMAP_LCDCON2_VFPD(IMAPFB_VFP - 1) |
		   IMAP_LCDCON2_VSPW(IMAPFB_VSW - 1)), 0x20cd0004);
	writel((IMAP_LCDCON3_HBPD(IMAPFB_HBP - 1) |
		   IMAP_LCDCON3_HOZVAL(IMAPFB_HRES - 1) |
		   IMAP_LCDCON3_HFPD(IMAPFB_HFP - 1)), 0x20cd0008);
	writel(IMAP_LCDCON4_HSPW(IMAPFB_HSW - 1), 0x20cd000c);
	writel(CONFIG_LCDCON5, 0x20cd0010);
	writel((1 << 8) | 1, 0x20cd0000);

	writel(readl(CONFIG_LCD_BL_GPIO + 4) | (1 << (CONFIG_LCD_BL_GPIO_Bit*2)),
	   CONFIG_LCD_BL_GPIO + 4);
	writel(readl(CONFIG_LCD_BL_GPIO) | (1 << CONFIG_LCD_BL_GPIO_Bit),
	   CONFIG_LCD_BL_GPIO);
	writel(readl(GPFCON) | (0x1 << (CONFIG_LCD_TOUT_PIN * 2 + 0xc)),
	   GPFCON);
#ifdef CONFIG_LCD_BL_LOW
	writel(readl(GPFDAT) & ~(1 << (CONFIG_LCD_TOUT_PIN + 6)), GPFDAT);
#else
	writel(readl(GPFDAT) | (1 << (CONFIG_LCD_TOUT_PIN + 6)), GPFDAT);
#endif

}
#endif



