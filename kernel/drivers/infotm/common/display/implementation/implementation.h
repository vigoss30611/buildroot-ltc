/*
 * implementation.h - display implementation header file
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */

#ifndef __DISPLAY_IMPLEMENTATION_HEADER__
#define __DISPLAY_IMPLEMENTATION_HEADER__


extern int implementation_init(void);
extern int set_attr(struct module_node * module, int * types, int num);
extern int module_attr_find(struct module_node *module, int attr, struct module_attr **m_attr);
extern int module_attr_ctrl(struct module_node *module, int attr, int *param, int num);
extern int platform_init(void);
extern int product_init(void);
extern int board_init(void);


#endif
