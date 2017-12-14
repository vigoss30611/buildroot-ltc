/*
 * ids_access.h - display ids access header file
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */

#ifndef __DISPLAY_ACCESS_HEADER__
#define __DISPLAY_ACCESS_HEADER__

#include <register_coronampw.h>
#include <linux/ioport.h>

extern int ids_access_Initialize(int idsx, struct resource *res);
extern int ids_access_uninstall(void);
extern u32 ids_readword(int idsx, u32 addr);
extern u32 ids_read(int idsx, u32 addr, u8 shift, u8 width);
extern void ids_writeword(int idsx, u32 addr, u32 data);
extern void ids_write(int idsx, u32 addr, u8 shift, u8 width, u32 data);
extern int ids_get_base_addr(int idsx);
extern int ids_access_sysmgr_init(void);

#endif
