/*
 * controller.h - display controller header file
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */

#ifndef __DISPLAY_CONTROLLER_HEADER__
#define __DISPLAY_CONTROLLER_HEADER__

extern int ids_irq_set(int idsx, int irq);
extern int ids_irq_initialize(int idsx);
extern int wait_vsync_timeout(int idsx, int irqtype, unsigned long timeout);
extern int wait_swap_timeout(int idsx, int irqtype, unsigned long timeout);

#endif
