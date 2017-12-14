#include <linux/kernel.h>   
#include <linux/module.h>   
#include <linux/clk.h>      
#include <linux/delay.h>    
#include <linux/gpio.h>     
#include <asm/io.h>         
#include <linux/slab.h>     
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <dss_common.h>     
#include <ids_access.h>     
#include <mach/pad.h>       
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include "i80_panel/i80.h"

#define DSS_LAYER   "[implementation]"
#define DSS_SUB1    "[interface]"     
#define DSS_SUB2    "[i80]" 

static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);
extern int lcdc_output_en(int idsx, int enable);
extern int lcd_default_enable(int enable);      
int sync_by_manual = 0;
static unsigned char i80_name[128];

static struct module_attr i80_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node i80_module = {
	.name = "i80",
	.attributes = i80_attr,
	.init = dss_init_module,
	.idsx = 1,
};

static int set_vic(int idsx, int *params, int num)
{
	struct i80_cmd cmd;
	dtd_t dtd;
	int vic = *params;

#if 1
//	lcd_default_enable(1);

	lcdc_output_en(idsx, 0);

	vic_fill(&dtd, vic, 60000);
	//printk("i80 open: %s\n", i80_name);
	if (i80_init(1) || i80_open(i80_name))
		    return -1;                    
	if (i80_dev_cfg(NULL, 5) < 0)     
		    return -1;                    
	                                  
#if 0
	cmd.num = 0;                      
	//cmd.cmd = 0x0022 << 1;            
	cmd.cmd = 0x2C;            
	cmd.type = CMD_NORMAL_AUTO;       
	cmd.rs_ctrl = I80_ACTIVE_LOW;     
	i80_set_cmd(&cmd);                
#endif
	                                  
//	i80_enable(1);                    
#endif
//	lcdc_output_en(idsx, 1);	

	return 0;
}

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;                         
	assert_num(1);                                
	dss_trace("ids%d, enable %d\n", idsx, enable);
	                                              
	if (enable) {
		i80_enable(enable);
		panel_enable(enable);
	} else {
		panel_enable(enable);
		i80_enable(enable);
	}
	return 0;                                     
}

static int init = 0;
static int dss_init_module(int idsx)
{
	if (init == 0) {
		if (item_exist(P_LCD_NAME)) {

			if(imapx_pad_init(("rgb0")) == -1 ){
			    dss_err("pad config IMAPX_RGB%d failed.\n", idsx);
			    return -1;
			}
			
			if (item_exist(P_I80_MANUAL_SYNC)) {
				sync_by_manual = item_integer(P_I80_MANUAL_SYNC, 0);
			}
			item_string(i80_name, P_LCD_NAME, 0);
			if (!strcmp(i80_name, "TFT3P3469_220_176")) {
				if (!tft3p3469_init())
					return 0;
			} else if (!strcmp(i80_name, "ST7789S_240_320")) {
				if (!st7789s_init())
					return 0;
			} else if (!strcmp(i80_name, "ST7567_132_65")) {
				if (!st7567_init())
					return 0;
			}


		} else
			return -1;

		init = 1;
	}
	return -1;
}
