/***************************************************************************** 
** led.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: simple led driver.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 07/22/2011 XXX	Initialized by Warits
*****************************************************************************/

#include <common.h>
#include <lowlevel_api.h>
#include <asm/io.h>

#define _LED_GPIO 56		/* put i2c1_scl low */
#define  LED_GPIO 57		/* use i2c1_sda as led pad */

int led_stat;

void inline led_on(void) { pads_setpin(LED_GPIO, led_stat = 1); }
void inline led_off(void) { pads_setpin(LED_GPIO, led_stat = 0); }
void inline led_ctrl(int on) { on? led_on(): led_off(); }
void inline led_toggle(void) { led_ctrl(!led_stat); }

void init_led(void) {
	pads_chmod(PADSRANGE(_LED_GPIO, LED_GPIO),
				PADS_MODE_OUT, led_stat = 0);
}

void led_pulse(int n) {

	led_on();
	udelay(5);
	led_off();
	udelay(2);

	while(n-- > 0) {
		led_on();
		udelay(2);
		led_off();
		udelay(2);
	}
}

