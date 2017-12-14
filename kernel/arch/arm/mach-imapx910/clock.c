/*************************************************************************
 * ** linux/arch/arm/mach-imapx200/clock.c
 * **
 * ** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
 * **
 * ** Use of Infotm's code is governed by terms and conditions
 * ** stated in the accompanying licensing statement.
 * ** Author:
 * **     Alex Zhang   <tao.zhang@infotmic.com.cn>
 * ** Revision History:
 * ** -----------------
 * ** 1.2  25/11/2009  Alex Zhang
 * ** 1.3  21/01/2010  Raymond Wang
 * ** 1.4              Larry Liu
 * ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/serial_core.h>
#include <linux/io.h>

#include <asm/mach/map.h>

#include <mach/hardware.h>

#include <mach/clock.h>
/* clock information */

static LIST_HEAD(clocks);
DEFINE_MUTEX(clocks_mutex);


/*
 * clk enable
 *
 * if the clock source don have enable func, it also return 0
 *
 * return : 0       clk enable ok
 *others  struct clk error; clk->parent is not CLK_ON;
 *something wrong with the process of enable clk
 */
int clk_enable(struct clk *clk)
{
	int ret = 0;
	unsigned long flags;

	struct clk *c = clk;

	if (IS_ERR(c))
		return ret;

	if (!clk)
		return 0;

#if  defined(CONFIG_IMAPX910_FPGA_PLATFORM)
	return 0;
#endif

	spin_lock_irqsave(&c->spinlock, flags);

	if (c->refcnt == 0) {
		if (c->parent) {
			ret = clk_enable(c->parent);
			if (ret)
				goto error;
		}

		if (c->ops && c->ops->enable) {
			ret = c->ops->enable(c);
			if (ret) {
				if (c->parent)
					/* as code above has enable once the clk
					 * so here need to disable
					 * */
					clk_disable(c->parent);
				goto error;
			}
			c->state = CLK_ON;
			/* TODO
			 *
			 * in the source code "c->set = true"
			 * do not understand how to use it
			 */
		}
	}
	c->refcnt++;
error:
	spin_unlock_irqrestore(&c->spinlock, flags);

	return ret;
}
EXPORT_SYMBOL(clk_enable);

/*
 * clk disable
 *
 * clock source is no longer required
 */
void clk_disable(struct clk *clk)
{
#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
#else
	unsigned long flags;

	struct clk *c = clk;

	if (!clk)
		return;

	spin_lock_irqsave(&c->spinlock, flags);

	if (c->refcnt == 1) {
		if (c->ops && c->ops->disable)
			c->ops->disable(c);

		if (c->parent)
			clk_disable(c->parent);

		c->state = CLK_OFF;
	}
	if (c->refcnt > 0)
		c->refcnt--;

	spin_unlock_irqrestore(&c->spinlock, flags);
#endif
}
EXPORT_SYMBOL(clk_disable);

#if defined(CONFIG_IMAPX910_FPGA_PLATFORM)
static unsigned long clk_get_rate_from_parent(struct clk *clk, struct clk *p)
{
	unsigned long rate = clk_get_rate(p);

	return rate;
}
#else
/*
 * clk get rate from parent
 *
 * this mainly using to comput current clk referened to clk->parent
 */
static unsigned long clk_get_rate_from_parent(struct clk *clk, struct clk *p)
{
	unsigned long rate = clk_get_rate(p);
	struct clk *c = clk;

	if (c->ops && c->ops->get_rate)
		rate = c->ops->get_rate(c);

	return rate;
}
#endif
/*
 * clk get rate sub
 *
 * this is the sub func of clk get rate
 */
static unsigned long clk_get_rate_sub(struct clk *c)
{
	if (c->parent)
		return clk_get_rate_from_parent(c, c->parent);
	else
		return c->rate;
}

/*
 * clk get rate
 *
 * this func uses to get clk rate
 */
unsigned long clk_get_rate(struct clk *c)
{
	unsigned long rate = 0;
	unsigned long flags;

	if (IS_ERR(c) || !c)
		return 0;
	/* TODO
	 * here is referened to the code of tegra
	 * acturally I don`t really why using spin_lock_irqsave
	 * maybe here we can use spin_lock instead
	 */
	spin_lock_irqsave(&c->spinlock, flags);

	rate = clk_get_rate_sub(c);

	spin_unlock_irqrestore(&c->spinlock, flags);

	return rate;
}
EXPORT_SYMBOL(clk_get_rate);

/*
 * clk set rate
 *
 * this funciton only to set the exactly the value rate
 * if clk can not set the value, return others
 *
 * return : >0      clk set rate  all right
 *          others something error
 */
int clk_set_rate(struct clk *clk, unsigned long rate)
{
	int ret = -1;
	unsigned long new_rate;

	struct clk *c = clk;

	if (IS_ERR(c) || !c)
		return ret;

	spin_lock(&c->spinlock);

	if (c->ops && c->ops->round_rate) {
		new_rate = c->ops->round_rate(c, rate);
		if (new_rate <= 0) {
			spin_unlock(&c->spinlock);
			pr_err
			    ("round_rate: Unable to set clock %s to rate %ld\n",
			     clk->name, new_rate);
			return -EINVAL;
		}
		rate = new_rate;
	}

	if (c->ops && c->ops->set_rate) {
		ret = c->ops->set_rate(c, rate);
		if (ret < 0) {
			spin_unlock(&c->spinlock);
			pr_err("set_rate: Unable to set clock %s to rate %ld\n",
			       clk->name, rate);
			return -EINVAL;
		}
	}
	spin_unlock(&c->spinlock);

	return ret;
}
EXPORT_SYMBOL(clk_set_rate);

/*
 * clk round rate
 *
 * this funcion to set the clk rate value to around the rate
 * if return is 0, means error
 * if return has a value , it means the rate value has been set
 */
long clk_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long ret = 0;
	unsigned long flags;

	struct clk *c = clk;

	if (IS_ERR(c) || !c)
		return ret;

	spin_lock_irqsave(&c->spinlock, flags);

	if (c->ops && c->ops->round_rate)
		ret = c->ops->round_rate(c, rate);

	spin_unlock_irqrestore(&c->spinlock, flags);

	return ret;
}
EXPORT_SYMBOL(clk_round_rate);

/*
 * clk get parent
 *
 * get the parent of clk
 */
struct clk *clk_get_parent(struct clk *c)
{
	return c->parent;
}
EXPORT_SYMBOL(clk_get_parent);

/*
 * clk set parent
 */
int clk_set_parent(struct clk *clk, struct clk *p)
{
	int ret = -1;

	struct clk *c = clk;

	if (IS_ERR(c) || IS_ERR(p))
		return ret;
	if (!c)
		return ret;

	mutex_lock(&clocks_mutex);

	if (c->ops && c->ops->set_parent)
		ret = c->ops->set_parent(c, p);

	mutex_unlock(&clocks_mutex);

	return ret;
}
EXPORT_SYMBOL(clk_set_parent);

int imapx800_clk_init_one_from_table(struct imapx800_clk_init_table *table)
{
	struct clk *c;
	struct clk *p;
	int ret;

	c = clk_get(NULL, table->name);
	if (!c) {
		pr_err("Unable to initialize clock %s", table->name);
		return -ENODEV;
	}

	if (table->parent) {
		p = clk_get(NULL, table->parent);
		if (!p) {
			pr_err("Unable to find the parent %s of clock %s\n",
			       table->parent, table->name);
			return -ENODEV;
		}
		if (c->parent != p) {
			ret = clk_set_parent(c, p);
			if (ret < 0) {
				pr_err("Unable to set the parent %s of clock %s\n",
				     table->parent, table->name);
				return -EINVAL;
			}
		}
	}

	if (table->rate && table->rate != clk_get_rate(c)) {
		ret = clk_set_rate(c, table->rate);
		if (ret < 0) {
			pr_err("Unable to set clock %s to rate %lu\n",
			       table->name, table->rate);
			return -EINVAL;
		}
	}

	if (table->enabled) {
		ret = clk_enable(c);
		if (ret < 0) {
			pr_err("Unable to enable clock %s\n", table->name);
			return -EINVAL;
		}
	}

	return 0;
}

void imapx800_clk_init_from_table(struct imapx800_clk_init_table *table)
{
	for (; table->name; table++)
		imapx800_clk_init_one_from_table(table);
}
EXPORT_SYMBOL(imapx800_clk_init_from_table);

/*
 * clk init
 */
void clk_init(struct clk *clk)
{
	struct clk *c = clk;

	spin_lock_init(&c->spinlock);

	if (c->ops && c->ops->init)
		c->ops->init(c);

	if (!c->ops || !c->ops->enable) {
		c->refcnt++;
		if (c->parent)
			c->state = c->parent->state;
		else
			c->state = CLK_ON;
	}

	mutex_lock(&clocks_mutex);
	list_add(&c->node, &clocks);
	mutex_unlock(&clocks_mutex);
}
EXPORT_SYMBOL(clk_init);
