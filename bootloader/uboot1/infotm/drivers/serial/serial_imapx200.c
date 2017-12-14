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
void serial_setbrg(void)
{
	u32 clk = get_PCLK();
	
#ifdef CONFIG_IMAPX200_FPGATEST
	writel(IMAPX200_CKSR_CKSEL_UEXTCLK,IMAPX200_CKSR);
#else
	writel(IMAPX200_CKSR_CKSEL_PCLK,IMAPX200_CKSR);
#endif
	writel(readl(IMAPX200_LCR) | IMAPX200_LCR_DLAB_ENABLE, IMAPX200_LCR);

	u32 baudrate = 115200;
	int i;

	i = (clk / baudrate) / 16;

	writel(IMAPX200_DLL_DIVISOR_LOW_BYTE(i),IMAPX200_DLL);
	writel(IMAPX200_DLH_DIVISOR_HIGH_BYTE(0),IMAPX200_DLH);

	writel(readl(IMAPX200_LCR) & (~IMAPX200_LCR_DLAB_ENABLE), IMAPX200_LCR);

	writel(readl(IMAPX200_LCR) | IMAPX200_LCR_DLS_MASK, IMAPX200_LCR);

	for (i = 0; i < 100; i++)
		barrier();
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
int init_serial(void)
{
#ifdef CONFIG_SERIAL1 
	writel((readl(GPACON) & ~(0xf)) | 0x0a, GPACON);

#elif defined(CONFIG_SERIAL2)
	writel((readl(GPACON) & ~(0xf<<8)) | (0x0a<<8), GPACON);

#elif defined(CONFIG_SERIAL3)
	writel((readl(GPBCON) & ~(0xf)) | (0x0a), GPBCON);

#elif defined(CONFIG_SERIAL4)
	writel((readl(GPBCON) & ~(0xfff)) | (0xaaa), GPBCON);
#endif
	
	writel(readl(IMAPX200_FCR) | IMAPX200_FCR_FIFOE_FIFO_ENABLE, IMAPX200_FCR);

	serial_setbrg();

	return 0 ;
}

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
	while(!(readl(IMAPX200_LSR) & 0x1));

	return readl(IMAPX200_RBR) & 0xff;
}


#ifdef CONFIG_MODEM_SUPPORT
static int be_quiet;
void disable_putc(void)
{
	be_quiet = 1;
}

void enable_putc(void)
{
	be_quiet = 0;
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
#ifdef CONFIG_MODEM_SUPPORT
	if (be_quiet)
		return;
#endif
	
	while (!(readl(IMAPX200_LSR) & ( IMAPX200_LSR_TEMT_XMITER_EMPTY| IMAPX200_LSR_THRE_MASK))) 
	{
		barrier();
	}

	writel(c, IMAPX200_THR);
}

int serial_tstc(void)
{
	return readl(IMAPX200_LSR) & 0x1;	
}

void serial_puts(const char *s)
{
	while (*s) {
		if(*s == 10)
		  serial_putc(13);
		serial_putc(*s++);
	}
}

