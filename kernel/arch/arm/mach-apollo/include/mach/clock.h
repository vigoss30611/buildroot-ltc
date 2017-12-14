/********************************************************************************
** linux-2.6.28.5/arch/arm/plat-imap/include/plat/clock.h
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** Use of Infotm's code is governed by terms and conditions
** stated in the accompanying licensing statement.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Author:
**     Raymond Wang   <raymond.wang@infotmic.com.cn>
**
** Revision History:
**     1.1  09/15/2009    Raymond Wang
**     1.2  01/21/2010    Raymond Wang
**     1.3                Larry Liu
********************************************************************************/

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <linux/clkdev.h>

struct clk;

struct clk_ops {
	void           (*init)(struct clk *);
	int            (*enable)(struct clk *);
	void           (*disable)(struct clk *);
	int            (*set_parent)(struct clk *, struct clk *);
	int            (*set_rate)(struct clk *, unsigned long);
	long           (*round_rate)(struct clk *, unsigned long);
	unsigned long  (*get_rate)(struct clk *);
};

struct imapx800_clk_init_table {
    const char *name;
    const char *parent;
    unsigned long rate;      
    bool enabled;
};

struct clk {
	struct list_head     node;           /* all of the clks get together by using a list, each struct look as a node */
	struct clk_lookup    lookup;         /* actrually do not know how it uses */
	struct clk           *parent;        /* parent of current clk */
	struct clk_ops       *ops;           /* operation of clk */
	const char           *name;          /* module name  */
	int		             id;
	int		             usage;
	unsigned long        rate;
	unsigned long        refcnt;         /* count, clk source enable and disable should be blance, and I`v still not got it */
	int                  state;          /* state of clk, it can be CLK_UNINITIAL,CLK_INITIAL, CLK_ON, CLK_OFF */
	unsigned int         m_name;         /* macro name */
	void                 *info;          /* it is used to point to the info struct of diff clk */

	spinlock_t spinlock;                 /* each clk struct has a spinlock */
};

/* define state of clk */
#define CLK_UNINITIAL        0
#define CLK_ON               1
#define CLK_OFF              2

void clk_init(struct clk *clk);

extern int imapx200_clkcon_enable(struct clk *clk, int enable);

extern int imap_register_clock(struct clk *clk);
extern int imap_register_clocks(struct clk **clk, int nr_clks);

extern int imap_register_coreclks(unsigned long xtal);

void imap_reset_clock(void);

#endif /* __CLOCK_H__ */ 

