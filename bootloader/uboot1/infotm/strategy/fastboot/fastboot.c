/*
 * (C) Copyright 2008 - 2009
 * Windriver, <www.windriver.com>
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <environment.h>
#include <command.h>
#include <fastboot.h>
#include <udc.h>
#define CONFUSED() printf ("How did we get here %s %d ? \n", __FILE__, __LINE__)

//static u8 fastboot_fifo[MUSB_EP0_FIFOSIZE];
static u8 faddr = 0xff;
static unsigned int high_speed = 0;
static struct cmd_fastboot_interface *fastboot_interface = NULL;
static int fastboot_rx (unsigned char* buf, unsigned int len)
{
	if(len == 0) {
		return 0;
	}
	/**********************FIXME****************************/
	/************************The rx count is not sure now*******************************/
	udc_read(buf, len);
	if (fastboot_interface &&
			fastboot_interface->rx_handler) {
		fastboot_interface->rx_handler ((const char*)buf, len);
	}
	return 0;
}
#if 0
static int fastboot_suspend (void)
{
	/* No suspending going on here! 
	   We are polling for all its worth */

	return 0;
}
#endif
int fastboot_poll(unsigned char* buffer ,  unsigned int download_size) 
{
//	unsigned char* buffer = NULL;
	printf("fastboot_poll\n");
	/********************FIXME********************/
	/****************check if the cable is disconnected***************/
	if(udc_get_cable_state() == 0) return -1;;

	if(buffer == NULL){
		printf("Fastboot Error: NULL transfer buffer\n");
		return -1;
	}

	//memset(buffer , 0 , download_size);
	return fastboot_rx( buffer ,download_size);
	 
}



void fastboot_shutdown(void)
{
	/* Clear the SOFTCONN bit to disconnect */
//	*pwr &= ~MUSB_POWER_SOFTCONN;

	/* Reset some globals */
	faddr = 0xff;
	fastboot_interface = NULL;
	high_speed = 0;
}

int fastboot_tx_status(unsigned char *buffer, unsigned int buffer_size)
{
	return udc_write( buffer , buffer_size);
}

int fastboot_getvar(const char *rx_buffer, char *tx_buffer)
{
	/* Place board specific variables here */
	return 0;
}
int fastboot_init(struct cmd_fastboot_interface *interface) 
{
	fastboot_interface = interface;
	return udc_connect();
}

