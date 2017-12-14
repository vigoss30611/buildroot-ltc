/*
 * dss_log.h - display log defination
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */

#ifndef __DISPLAY_LOG_HEADER__
#define __DISPLAY_LOG_HEADER__


enum dss_print_level {
	DSS_ERR = 1,
	DSS_INFO_SHORT,
	DSS_INFO,
	DSS_DBG,
	DSS_TRACE,
};

extern int dss_log_level;


#define dss_trace(format, arg...)	\
	 ({ if (dss_log_level >= DSS_TRACE) 	\
	 	printk(KERN_WARNING "[dss]%s%s%s <trace> %s: " format, \
	 		DSS_LAYER, DSS_SUB1, DSS_SUB2 ,__func__, ## arg); 0; })

#define dss_dbg(format, arg...)	\
	 ({ if (dss_log_level >= DSS_DBG) 	\
	 	printk(KERN_WARNING "[dss]%s%s%s <debug> %s: " format, \
	 		DSS_LAYER, DSS_SUB1, DSS_SUB2 ,__func__, ## arg); 0; })
	 		
#define dss_info(format, arg...)	\
	 ({ if (dss_log_level == DSS_INFO_SHORT) 	\
		printk(KERN_WARNING "[dss] <info> %s: " format, __func__, ## arg); \
	 else if (dss_log_level >= DSS_INFO) \
	 	printk(KERN_WARNING "[dss]%s%s%s <info> %s: " format, \
	 		DSS_LAYER, DSS_SUB1, DSS_SUB2 ,__func__, ## arg); 0; })
	 		
#define dss_err(format, arg...)	\
	 ({ if (dss_log_level >= DSS_ERR)	\
		printk(KERN_WARNING "[dss]%s%s%s <error> %s: " format, \
	 		DSS_LAYER, DSS_SUB1, DSS_SUB2 ,__func__, ## arg); 0; })


#endif
