/* arch/arm/mach-imapx800/include/mach/uncompress.h
 *
 * Copyright (c) 2003, 2007 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_UNCOMPRESS_H
#define __ASM_ARCH_UNCOMPRESS_H

#include <mach/imap-iomap.h>
#include <mach/imap-uart.h>

#define IMAP_DEBUG_UART_BASE	IMAP_UART3_BASE
#define IMAP_DEBUG_BAUDRATE	115200
#define IMAP_DEBUG_UART_CLK	23076923
/* working in physical space... */

#define __raw_writel(d,ad) do { *((volatile unsigned int *)(ad)) = (d); } while(0)
#define __raw_readl(ad) *(volatile unsigned int *)(ad)

static void gpio_func_mode(int io_index)
{
    int32_t cnt, num;
    int32_t val;

    cnt = io_index/8;
    num = io_index&0x7;
    val = __raw_readl(SYSMGR_PAD_BASE + 0x64 + cnt*4);
    val &=~(1<<num);
    __raw_writel(val, SYSMGR_PAD_BASE + 0x64 + cnt*4);
}

static void module_init_cfg(uint32_t io_addr)
{
    uint32_t val;
    volatile uint8_t *reg = (volatile uint8_t *)io_addr;
    
    reg[0xc] = 0;
    reg[0x0] = 0xff;
    reg[0x4] = 0xff;
    reg[0x8] = 0x11;
    while(1)
    {
	val = reg[0x8];;
	if(val & 0x2)
	    break;
    }
    val = reg[0x8];
    val&=~0x10;
    reg[0x8] = val;
    reg[0x18] = 0xff;
    reg[0x0] = 0;
    reg[0xc] = 0xff;
}

static void putc(int ch)
{
    	volatile uint8_t *uart_reg = (volatile uint8_t *)IMAP_DEBUG_UART_BASE;
	while(uart_reg[UART_FR] & 0x20)
	    barrier();
	uart_reg[UART_DR] = ch;
}
static inline void flush(void)
{
}

static inline void arch_decomp_wdog(void)
{
}

static inline void arch_decomp_setup(void)
{
	/* we may need to setup the uart(s) here if we are not running
	 * on an BAST... the BAST will have left the uarts configured
	 * after calling linux.
	 */
	//volatile uint8_t *uart_reg = (volatile uint8_t *)IMAP_DEBUG_UART_BASE;
	int32_t val, clk = IMAP_DEBUG_UART_CLK, baudrate = IMAP_DEBUG_BAUDRATE;
	int32_t ibr, fbr;

	module_init_cfg(SYSMGR_UART_BASE);
	gpio_func_mode(68);
	gpio_func_mode(69);

	ibr = (clk/(16*baudrate)) & 0xffff;
	fbr = (((clk - ibr*(16*baudrate)) *64)/(16*baudrate)) & 0x3f;
	__raw_writel(ibr, IMAP_DEBUG_UART_BASE + UART_IBRD);
	__raw_writel(fbr, IMAP_DEBUG_UART_BASE + UART_FBRD);

	/* set format: data->8bit stop->1-bit */
	val = __raw_readl(IMAP_DEBUG_UART_BASE + UART_LCRH);
	val &= ~0xff;
	val |= 0x60;
	__raw_writel(val, IMAP_DEBUG_UART_BASE + UART_LCRH);

	/* setup rx||tx */
	val = __raw_readl(IMAP_DEBUG_UART_BASE + UART_CR);
	val &=~0xff87;
	val |=0x301;
	__raw_writel(val, IMAP_DEBUG_UART_BASE + UART_CR);

	/* set format: data->8bit stop->1-bit */
	val = __raw_readl(IMAP_DEBUG_UART_BASE + UART_LCRH);
	val |= 0x10;
	__raw_writel(val, IMAP_DEBUG_UART_BASE + UART_LCRH);
	return ;
}

#endif /* __ASM_ARCH_UNCOMPRESS_H */
