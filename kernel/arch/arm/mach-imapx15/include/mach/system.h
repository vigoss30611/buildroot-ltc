/* arch/arm/mach-imap/include/mach/system.h
 *
 * Copyright (c) 2003 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * IMAPX200 - System function defines and includes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/io.h>
#include <mach/io.h>
#include <mach/idle.h>
#include <mach/rtcbits.h>

void (*imap_idle)(void);
void (*imap_reset_hook) (void);
extern int read_rtc_gpx(int io_num);
extern void imap_poweroff(void);
extern void imap_reset(char mode, const char *cmd);
extern void a5pv10_reset(char mode, const char *cmd);
extern void a5pv10_poweroff(void);
void imap_default_idle(void)
{
	/* idle the system by using the idle mode which will wait for an
	 * interrupt to happen before restarting the system.
	 */

	/* Warning: going into idle state upsets jtag scanning */
//    printk(KERN_ERR "idle: \n");
       asm("wfi");
}

static void arch_idle(void)
{
	if (imap_idle != NULL)
		(imap_idle)();
	else
		imap_default_idle();
}

//#include <mach/system-reset.h>
static void arch_reset(char mode, const char *cmd)
{
    printk(KERN_ERR "ENTER:%s\n",__func__);
    if(imap_reset)
        imap_reset(mode, cmd);
//    a5pv10_reset(mode,cmd);
    return;

}

static void arch_shutdown(void)
{
    printk(KERN_ERR "ENTER:%s\n",__func__);
    rtcbit_set("retry_reboot", 0);
    if(imap_poweroff)
        imap_poweroff();
    //a5pv10_poweroff();
    return;
}

