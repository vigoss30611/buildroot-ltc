/***************************************************************************** 
** cpu/arm1136/imapx200/speed.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: PLL and clock APIs for u-boot
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 12/18/2009 XXX	Initialized by Warits
*****************************************************************************/

#include <common.h>
#include <asm/io.h>

#define APLL 0
#define DPLL 1
#define EPLL 2

/*
 * NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */

#if defined(CONFIG_IMAPX200_FPGATEST)
ulong get_ARMCLK(void) {	return 48000000; }
ulong get_FCLK(void) {		return 48000000; }
ulong get_HCLK(void) {		return 48000000; }
ulong get_PCLK(void) {		return 48000000; }
ulong get_UCLK(void) {		return 48000000; }
#else
static inline ulong
imapx200_get_pll(int plltype)
{
	ulong cfg, odiv, ddiv, mdiv, baseclk;

	/*To prevent overflow in calculation*/
	baseclk = CONFIG_SYS_CLK_FREQ / 1000;

	switch (plltype) {
	case APLL:
		cfg = readl(APLL_CFG);
		break;
	case DPLL:
		cfg = readl(DPLL_CFG);
		break;
	case EPLL:
		cfg = readl(EPLL_CFG);
		break;
	default:
		return 0;
	}

	odiv = (cfg >> 12) & 0x03;
	ddiv = (cfg >> 7)  & 0x1f;
	mdiv = (cfg) & 0x7f;

	return (baseclk*(2*(mdiv + 1))) / ((ddiv + 1)*(1<<(odiv)))*1000;
}

/* return ARMCORE frequency */
ulong get_ARMCLK(void)
{
	ulong ratio;
	ulong clk;

	/*
	 * According to iMAP datasheet, cpu clk can be generated from
	 * APLL, DPLL, EPLL, we here assume it is generated from APLL
	 */

	clk = imapx200_get_pll(APLL);
	/* FIXME: 1~8 ? 1~4 */
	ratio = (readl(DIV_CFG0) & 0xf);
	return clk / ratio;
}

/* return FCLK frequency */
ulong get_FCLK(void)
{
	return imapx200_get_pll(APLL);
}

/* return HCLK frequency */
ulong get_HCLK(void)
{
	ulong ratio;

	ratio = (readl(DIV_CFG0) >> 4) & 0xf;
	return imapx200_get_pll(APLL) / ratio;
}

/* return PCLK frequency */
ulong get_PCLK(void)
{
	ulong ratio;

	ratio = 1 << ((readl(DIV_CFG0) >> 16) & 0x03);
	return get_HCLK() / ratio;
}

/* return UCLK frequency */
ulong get_UCLK(void)
{
	return imapx200_get_pll(EPLL);
}
#endif

int print_cpuinfo(void)
{
#if 0
	printf("APLL-%3lu, DPLL-%3lu, EPLL-%3lu (MHz)\n",
	   get_FCLK()/1000000, imapx200_get_pll(DPLL)/1000000,
	   imapx200_get_pll(EPLL)/1000000);
	printf("CPU-%3lu, Hx2-%3lu, H-%3lu, P-%2lu (MHz)\n",
	   get_ARMCLK()/1000000, get_HCLK()/500000, get_HCLK()/1000000,
	   get_PCLK()/1000000);
#endif
	return 0;
}
