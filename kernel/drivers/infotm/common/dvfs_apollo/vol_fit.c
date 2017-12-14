/*****************************************************************************
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: Infotm dvfs driver.
**
** Author:
**     alex<alex.zhang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.0  25/11/2013
*****************************************************************************/

#include <linux/kernel.h>            
#include <linux/clk.h>               
#include <linux/clkdev.h>            
#include <linux/debugfs.h>           
#include <linux/init.h>              
#include <asm/io.h>
#include <linux/list.h>              
#include <linux/list_sort.h>         
#include <linux/module.h>            
#include <linux/regulator/consumer.h>
#include <linux/seq_file.h>          
#include <linux/slab.h>              
#include <linux/delay.h>             
#include <linux/cpufreq.h>
#include <mach/items.h>


#include "dvfs.h"
#include "cpu_fit.h"

#define DVFS_DEBUG
#ifdef DVFS_DEBUG
#define dvfs_dbg(x...)	printk("imapx_dvfs:" x)
#else
#define dvfs_dbg(x...)	do{} while(0)
#endif

#define dvfs_err(x...) printk(KERN_ERR "imapx_dvfs:" x)


static DEFINE_MUTEX(cpu_dvfs_lock); 
static LIST_HEAD(imapx_dvfs_cpu_list);
static LIST_HEAD(imapx_dvfs_core_list);
#define MAX_DVFS_FREQS	27
#define IMAPX_CPU_MAX_VOL	1300

int imapx_cpu_dvfs_notify(struct notifier_block *notify_block,
		unsigned long val, void *data);

static struct imapx_dvfs_rail imapx_dvfs_rail_cpu = {
//	.reg_id	= "vdd_cpu",
	.reg_id	= "dc1",
	.max_millivolt	=	1200,
	.min_millivolt	=	900,
	.step			=	0,//100,
	.notify_list	=	CPUFREQ_TRANSITION_NOTIFIER,
	.dvfs_notify	= {
		.notifier_call	=	imapx_cpu_dvfs_notify,	
	},
	.registr_dvfs_notifier = cpufreq_register_notifier,
};

static struct imapx_dvfs_rail imapx_dvfs_rail_core = {
//	.reg_id	= "vdd_core",
	.reg_id	= "dc1",
	.max_millivolt	=	1200,
	.min_millivolt	=	900,
	.registr_dvfs_notifier = NULL,
};

static struct imapx_dvfs_rail *imapx_dvfs_rails[] = {
	&imapx_dvfs_rail_cpu,
	&imapx_dvfs_rail_core,
};

#define IMAPX_CPU_DVFS(_cpu_name, _mult, _delay...)	\
	{												\
		.name	=	_cpu_name,					\
		.freq_mult	=	_mult,						\
		.delay		=	_delay,						\
		.rail = &imapx_dvfs_rail_cpu,	\ 
	}	

static int  imapx_cpu_millivolts[] = {0};//{925,950,950,950, 950, 950, 950, 950, 950, 950, 950, 950, 950, 950, 950, 950, };
static struct imapx_dvfs_dev	cpu_dvfs_table[] = {
	IMAPX_CPU_DVFS("cpu0", MHz, 0),
	IMAPX_CPU_DVFS("cpu1", MHz, 0),
	IMAPX_CPU_DVFS("cpu2", MHz, 0),
	IMAPX_CPU_DVFS("cpu3", MHz, 0),

/*
	IMAPX_CPU_DVFS("cpu0", MHz, 0, 132, 180, 228, 276, 324, 372, 420, 468, 516, 564, 612, 660, 708, 756, 804, 852, 900, 948, 996, 1032, 1080, 1128, 1176, 1224, 1272, 1320, 1368),
	IMAPX_CPU_DVFS("cpu1", MHz, 0, 132, 180, 228, 276, 324, 372, 420, 468, 516, 564, 612, 660, 708, 756, 804, 852, 900, 948, 996, 1032, 1080, 1128, 1176, 1224, 0, 0, 0),
	IMAPX_CPU_DVFS("cpu2", MHz, 0, 132, 180, 228, 276, 324, 372, 420, 468, 516, 564, 612, 660, 708, 756, 804, 852, 900, 948, 996, 1032, 1080, 1128, 1176, 1224, 0, 0, 0),
	IMAPX_CPU_DVFS("cpu3", MHz, 0, 132, 180, 228, 276, 324, 372, 420, 468, 516, 564, 612, 660, 708, 756, 804, 852, 900, 948, 996, 1032, 1080, 1128, 1176, 1224, 1272, 1320, 1368),
*/
};

static void imapx_dvfs_timer(unsigned long data)
{
	struct imapx_dvfs_rail *rail = (struct imapx_dvfs_rail*)data;
	int ret;

	ret = regulator_set_voltage(rail->reg,
			rail->new_millivolts * 1000,      
			rail->max_millivolt * 1000);     
	if(ret){
		dvfs_err("Failed to set regulator %s\n",rail->reg_id);
		return;
	}
	rail->millivolts = rail->new_millivolts;
}

extern int in_hotplug;
static struct imapx_dvfs_dev *__cpu_dvfs_get_vol(const char *dvfs_name, 
				unsigned long rate)
{
	int i;
	int vol = 0;
	int ret = 0;
	struct imapx_dvfs_dev *dvfs = NULL;
	struct imapx_dvfs_dev *d;

	for(i = 0; i < ARRAY_SIZE(cpu_dvfs_table); i++)    
    if(!strcmp(cpu_dvfs_table[i].name, dvfs_name)){
        dvfs = &cpu_dvfs_table[i];                 
        break;                                     
    }                                              
	
	if(dvfs == NULL){                                                     
		dvfs_err("can not find name\n");                                  
		return NULL;                                                   
	}                                                                     

	dvfs_dbg("driver[%s] set rate %u\n",dvfs->name, rate);                

	for(i = 0; i < dvfs->num_freq; i++){                                  
		if(rate == dvfs->freqs[i])                                        
			vol = dvfs->millivolts[i];                                    
	}                                                                     

	dvfs->cur_rate = rate;                                                

	if(!vol){                                                             
		dvfs_err("cannot find right vol for dev %s rate %uKHz\n",
				dvfs->name, rate);        
		vol = dvfs->rail->max_millivolt;
//		return NULL;                                                   
	}                                                                     

	if (in_hotplug == 1) {
		dvfs_dbg("%s in cpu hotplug\n",__func__);
		vol = dvfs->rail->max_millivolt;
	}

	if((vol > dvfs->rail->max_millivolt) ||                               
			(vol < dvfs->rail->min_millivolt)) {                      
		dvfs_err("vol[%d] to set exceed range [%d -> %d]\n", vol,         
				dvfs->rail->min_millivolt, dvfs->rail->min_millivolt);
	}                                                                     

//	vol = 1150;
	dvfs_dbg("want set vol %d for dev %s\n",vol, dvfs->name);             

	dvfs->cur_millivolt = vol;                                            

	return dvfs;
}

static int __imapx_dvfs_set_vol(struct imapx_dvfs_dev *dvfs, int delay)
{
	int vol = 0;
	int ret = 0;
	int step = (dvfs->cur_millivolt > dvfs->rail->millivolts) ? 
						dvfs->rail->step : -dvfs->rail->step;

	while(dvfs->cur_millivolt != dvfs->rail->millivolts) {
		if(dvfs->rail->step == 0) {
			dvfs->rail->new_millivolts = dvfs->cur_millivolt;
		} else if(abs(dvfs->cur_millivolt - dvfs->rail->millivolts) 
									>= dvfs->rail->step) {
			dvfs->rail->new_millivolts = dvfs->rail->millivolts + step;
		} else {
			dvfs->rail->new_millivolts = dvfs->cur_millivolt;
		}

		dvfs_dbg("%s dev %s set vol %d\n",__func__,
						dvfs->name, dvfs->rail->new_millivolts);
		if(delay > 0){
			if(timer_pending(&dvfs->rail->dvfs_timer) == 0)
				mod_timer(&dvfs->rail->dvfs_timer, usecs_to_jiffies(delay));
		}else{
			ret = regulator_set_voltage(dvfs->rail->reg,
					dvfs->rail->new_millivolts * 1000,      
					dvfs->rail->max_millivolt * 1000);     
			if(ret){
				dvfs_err("Failed to set regulator %s\n",dvfs->rail->reg_id);
				return ret;
			}
			dvfs->rail->millivolts = dvfs->rail->new_millivolts;
		}
		udelay(500);
	}

	return ret;
}

int imapx_cpu_dvfs_notify(struct notifier_block *notify_block,
		unsigned long val, void *data)
{
	int cpu_num = num_online_cpus();
	struct cpufreq_freqs *freq = data;
	char cpu_name[12] ;
	struct imapx_dvfs_dev *dvfs;

	/*
	if (in_hotplug == 1) {
		dvfs_dbg("%s in cpu hotplug\n",__func__);
		cpu_num += 1;
	}
	*/

	if (cpu_num > CONFIG_NR_CPUS)
		cpu_num = CONFIG_NR_CPUS;

	sprintf(cpu_name, "cpu%d", 
			(cpu_num == 4) ? 2 : ((cpu_num == 3) ? 3: (cpu_num - 1)));

	dvfs_dbg("%s:val %d cpu%d:freq->old %uHz freq->new %uHz cpu_name %s\n",
			__func__, val, freq->cpu, freq->old, freq->new, cpu_name); 

	dvfs = __cpu_dvfs_get_vol(cpu_name, freq->new);
	if (!dvfs) {
		dvfs_err("cannot find right dvfs dev %s\n",cpu_name);
		return NOTIFY_OK;
	}

	dvfs_dbg("vol %d -> %d\n", dvfs->rail->millivolts, dvfs->cur_millivolt);

	if (((val == CPUFREQ_PRECHANGE) && 
				(dvfs->cur_millivolt > dvfs->rail->millivolts)) ||
			((val == CPUFREQ_POSTCHANGE) && 
			 (dvfs->cur_millivolt < dvfs->rail->millivolts))) {
		__imapx_dvfs_set_vol(dvfs, dvfs->delay);
	}

	return NOTIFY_DONE;
}

static int imapx_dvfs_connect_to_regulator(struct imapx_dvfs_rail *rail)
{
	struct regulator *reg;
	int v;

	if (!rail->reg) {                                                  
		reg = regulator_get(NULL, rail->reg_id);                       
		if (IS_ERR(reg)) {                                             
			dvfs_err("failed to connect %s rail\n",          
					rail->reg_id);                                      
			return -EINVAL;                                            
		}                                                              
		rail->reg = reg;                                               
	}                                                                  

	v = regulator_enable(rail->reg);                                   
	if (v < 0) {                                                       
		dvfs_err("failed on enabling regulator %s\n, err %d",
				rail->reg_id, v);                                          
		return v;                                                      
	}                                                                  

	v = regulator_get_voltage(rail->reg);                              
	if (v < 0) {                                                       
		dvfs_err("failed initial get %s voltage\n",          
				rail->reg_id);                                          
		return v;                                                      
	}                                                                  

	dvfs_dbg("%s get voltage %d for %s\n",__func__,v,rail->reg_id);

	rail->boot_millivolt = v / 1000;                                       
	rail->new_millivolts = rail->boot_millivolt;
	return 0;                                                          
}

static int imapx_dvfs_init_rails(struct imapx_dvfs_rail *rail[], int n)
{
	int i;
	int ret;

	for(i = 0; i < n; i++){
		ret = imapx_dvfs_connect_to_regulator(rail[i]);
		if(ret)
			return ret;

		INIT_LIST_HEAD(&rail[i]->attached_dev);
		rail[i]->millivolts = rail[i]->boot_millivolt;
		rail[i]->dvfs_timer.function = imapx_dvfs_timer;
		rail[i]->dvfs_timer.data = (unsigned long)rail[i];

		if(item_exist("soc.vol.fit")) {
			if(rail[i]->registr_dvfs_notifier != NULL){
				rail[i]->registr_dvfs_notifier(&rail[i]->dvfs_notify, 
						rail[i]->notify_list);
			}
		}
	}

	return 0;
}

static void imapx_dvfs_init_dev(struct imapx_dvfs_dev *dvfs)
{
	dvfs_dbg("dvfs init for dev %s\n", dvfs->name);
	dvfs->cur_millivolt = dvfs->rail->boot_millivolt;
	dvfs->max_millivolt = dvfs->rail->boot_millivolt;
	list_add_tail(&dvfs->reg_node, &dvfs->rail->attached_dev);
}

static int imapx_dvfs_init(void)
{
	int i, ret;

	if (!item_exist("soc.dvfs.autotest") && item_exist("soc.vol.fit")) {
		ret = imapx_dvfs_init_rails(imapx_dvfs_rails,
			   	ARRAY_SIZE(imapx_dvfs_rails));
		if (ret) {
			dvfs_err("%s error %d\n",__func__, ret);
			return ret;
		}

		for(i = 0; i < ARRAY_SIZE(cpu_dvfs_table); i++) {
			imapx_set_cpu_dvfs_relation(&cpu_dvfs_table[i], i);
			imapx_dvfs_init_dev(&cpu_dvfs_table[i]);
		}
	}

	return 0;
}

__initcall(imapx_dvfs_init);


