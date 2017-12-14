/***************************************************************************** 
** drivers/serial/serial_imapx200.c 
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description: File used for the standard function interface of output.
**
** Author:
**     Tao Zhang   <alex.zhang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.1  12/18/2009  Alex Zhang   
*****************************************************************************/ 

#include <common.h>
#include <asm/io.h>

/* timeout = 1ms */
#define UART_TIMEROUT_VAL  1000

/*!
 ***********************************************************************
 * -Function:
 *    serial_setbrd(NULL)
 *
 * -Description:
 *    This function is used for configure uart baudrate.The process is,
 *        1. get clock source of uart;
 *        2. Config baudrate dividor according to the global data baudrate;
 *        3. Enable uart fifo;
 *
 * -Input Param
 *    *pInput1        xxxxxxxxxxxxxxxxxxx
 *    *pInput2        xxxxxxxxxxxxxxxxxxx
 *
 * -Output Param
 *    *pOutput1       xxxxxxxxxxxxxxxxxxx
 *                
 * -Return
 *    none
 *
 * -Others
 *    
 ***********************************************************************
 */
void serial_setbrg()
{
	int32_t clk;
	int16_t ibr, fbr;
	int32_t baudrate = 115200;

	clk = module_get_clock(UART_CLK_BASE);
	ibr = (clk/(16*baudrate)) & 0xffff;
	fbr = (((clk - ibr*(16*baudrate)) *100)/(16*baudrate)) & 0x3f;
	writel(ibr, UART_IBRD);
	writel(fbr, UART_FBRD);
}

/*!
 ***********************************************************************
 * -Function:
 *    serial_init( Null)
 *
 * -Description:
 *    This function is used for initialize gpio port for uart,
 *        1. get the configured uart port number;
 *        2. Config the selected uart's gpio;
 *        3. xxxxxxxxxxxxxxxx;
 *
 * -Input Param
 *    *pInput1        xxxxxxxxxxxxxxxxxxx
 *    *pInput2        xxxxxxxxxxxxxxxxxxx
 *
 * -Output Param
 *    *pOutput1       xxxxxxxxxxxxxxxxxxx
 *                
 * -Return
 *    none
 *
 * -Others
 *    
 ***********************************************************************
 */
int serial_init(void)
{
	int32_t val;

	/* reset uart control*/
	module_enable(UART_SYSM_ADDR);
	pads_chmod(PADSRANGE(68, 69), PADS_MODE_CTRL, 0);

	/* configure uart gpio */

	/* set format: data->8bit stop->1-bit */
	val = readl(UART_LCRH);
	val &= ~0xff;
	val |= 0x60;
	writel(val, UART_LCRH);
	serial_setbrg();
	/* setup rx||tx */
	val = readl(UART_CR);
	val &=~0xff87;
	val |=0x301;
	writel(val, UART_CR);
	val = readl(UART_LCRH);
	val |= 0x10;
	writel(val, UART_LCRH);

	return 0 ;
}

#if !defined(CONFIG_PRELOADER)
/*!
 ***********************************************************************
 * -Function:
 *    serial_getc( INPUT1 *  pInput1, INPUT2 *pInput2, OUTPUT1 *pOutput1)
 *
 * -Description:
 *     
 *     Read a single byte from the serial port. Returns 1 on success, 0
 *     otherwise. When the function is succesfull, the character read is
 *     written into its argument c.
 *           
 *
 * -Input Param
 *    *pInput1        xxxxxxxxxxxxxxxxxxx
 *    *pInput2        xxxxxxxxxxxxxxxxxxx
 *
 * -Output Param
 *    *pOutput1       xxxxxxxxxxxxxxxxxxx
 *                
 * -Return
 *    byte from the serial port
 *
 * -Others
 *     
 ***********************************************************************
 */
int serial_getc(void)
{
	while(1)
	{
		if(!(readl(UART_FR) & UART_FR_RXFE))
			break;
	}
	return readl(UART_DR)&0xff;
}

int serial_tstc(void) {
	return !(readl(UART_FR) & UART_FR_RXFE);
}
#endif

/*!
 ***********************************************************************
 * -Function:
 *    serial_putc(const char c)
 *
 * -Description:
 *  Output a single byte to the serial port.
 *
 * -Input Param
 *  Input1	const char c
 *
 * -Output Param
 *    *pOutput1       xxxxxxxxxxxxxxxxxxx
 *                
 * -Return
 *    none
 *
 * -Others
 ***********************************************************************
 */
void serial_putc(const char c)
{
	uint64_t t;

	t = get_ticks();
	while(1) 
	{
		if(!(readl(UART_FR) & UART_FR_TXFF))
			break;
		if(get_ticks() > t + UART_TIMEROUT_VAL)
			break;
	}
	writel(c, UART_DR);
}

void serial_puts(const char *s)
{
        while (*s)
	{
		if(*s == '\n')
			serial_putc('\r');
		serial_putc(*s++);
	}
}

