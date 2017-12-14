/* arch/arm/mach-imap/include/mach/gpio.hi
 *
 * copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
 * 
 * IMAPX200 - GPIO lib support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#define gpio_get_value	__gpio_get_value
#define gpio_set_value	__gpio_set_value
#define gpio_cansleep	__gpio_cansleep
#define gpio_to_irq	__gpio_to_irq

/* define the number of gpios we need to the one after the GPQ() range */
#define ARCH_NR_GPIOS	(153)

#include <asm-generic/gpio.h>

extern int imapx910_gpio_save(void);
extern int imapx910_gpio_restore(void);
