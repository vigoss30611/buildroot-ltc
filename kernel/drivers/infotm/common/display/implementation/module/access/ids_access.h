/*
 * ids_access.h - display ids access header file
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */

#ifndef __DISPLAY_ACCESS_HEADER__
#define __DISPLAY_ACCESS_HEADER__

#if defined(CONFIG_MACH_IMAPX800) || defined(CONFIG_MACH_IMAPX910) || defined(CONFIG_MACH_IMAPX15)
#include <register_x15_x9.h>
#endif

extern int ids_access_Initialize(int idsx, resource_size_t addr, unsigned int size);
extern int ids_access_uninstall(void);
extern u32 ids_readword(int idsx, u32 addr);
extern u32 ids_read(int idsx, u32 addr, u8 shift, u8 width);
extern void ids_writeword(int idsx, u32 addr, u32 data);
extern void ids_write(int idsx, u32 addr, u8 shift, u8 width, u32 data);


#endif
