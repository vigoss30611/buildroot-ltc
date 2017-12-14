/* arch/arm/mach-imapx800/include/mach/debug-macro.S
 *
 * Debugging macro include header
 *
 *  Copyright (C) infotmic
 *
 *  Moved from linux/arch/arm/kernel/debug.S by Ben Dooks
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <mach/imap-iomap.h>


	.macro addruart, rx
	.endm

	.macro fifo_full_imap rd, rx
	.endm

	.macro fifo_level_imap rd, rx
	.endm

#define fifo_full  fifo_full_imap
#define fifo_level fifo_level_imap

/* include the reset of the code which will do the work */

#include <plat/debug-macro.S>
