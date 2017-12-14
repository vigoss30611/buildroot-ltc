/*------------------------------------------------------------------------------
* (c) Copyright, Augusta Technology, Inc., 2006-present.
* (c) Copyright, Augusta Technology USA, Inc., 2006-present.
*
* This software, document, web pages, or material (the "Work") is copyrighted
* by its respective copyright owners.  The Work may be confidential and
* proprietary.  The Work may be further protected by one or more patents and
* be protected as a part of a trade secret package.
*
* No part of the Work may be copied, photocopied, reproduced, translated, or
* reduced to any electronic medium or machine-readable form, in whole or in
* part, without prior written consent of the copyright owner. Any other
* reproduction in any form without the permission of the copyright owner is
* prohibited.
*
* All Work are protected by the copyright laws of all relevant jurisdictions,
* including protection under the United States copyright laws, and may not be
* reproduced, distributed, transmitted, displayed, published, or broadcast
* without the prior written permission of the copyright owner.
*
------------------------------------------------------------------------------*/

#ifndef __MACH_SRAM_H
#define __MACH_SRAM_H

#include <linux/compiler.h>

/* sram memory, 64KB */
#define IMAP_SRAM_PHYS_ADDR			0x8018000
#define IMAP_SRAM_VIRTUAL_ADDR		0xffff2000
#define IMAP_SRAM_SIZE				0x8000

#define IMAP_SRAM_STACK_SIZE  512   /* byte */


/* Tag with this the constants */
#define __sramconst __section(.sram.rodata)

/* Tag with this the data */
#define __sramdata __section(.sram.data)

/* Tag with this the functions inside SRAM called from outside SRAM */
#define __sramfunc  __attribute__((long_call)) __section(.sram.text) noinline

/* Tag with this the functions inside SRAM called from inside SRAM */
#define __sramlocalfunc __section(.sram.text)


#endif /* __MACH_SRAM_H */
