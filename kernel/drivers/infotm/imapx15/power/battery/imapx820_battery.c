/***************************************************************************** 
** XXX driver/power/imapx820_battery.c XXX
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: imapx820 battery driver.
**
** Author:
**     jack   <jack.zhang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 0.1  XXX 12/11/2012 XXX	jack
*****************************************************************************/

#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/gpio.h>

#include <asm/io.h>

#include <mach/pad.h>
#include <mach/items.h>
#include <mach/rtcbits.h>

#include <mach/belt.h>

#include "imapx820_battery.h"
//#include "imapx820_charger.h"

#define ADC_LSB 244

#define IMAP_BATT_DEBUG
#ifdef IMAP_BATT_DEBUG
	#define BATT_DBG(x...) do{\
		if(item_exist("power.debug"))\
			if(!item_equal("power.debug", "nohalt", 0))\
				printk(KERN_ERR "[imapx battery]: " x);\
		}while(0)
#else
	#define BATT_DBG(x...) do{} while(0)
#endif

#define AVE_NUM 4
#define IMAP_MON_INTVAL 5000;

static struct imap_battery_info batt_info;

//extern int read_rtc_gpx(int io_num);
extern int rtc_pwrwarning_mask(int en);
//extern int tps65910_bkreg_wr(int reg, int val);
//extern int tps65910_bkreg_rd(int reg);
extern int rd_get_time_sec(void);
extern void rtc_alarm_set_ext(u32 type, u32 data);
extern u32 rtc_alarm_get_ext(u32 type);
extern int imapx15_unexpect_shut(void);
/*
 *
 *[KP-junery]:FIXME 没有移植pcbtest
 *
 */
//extern void __imapx_register_pcbtest_batt_v(int (*func1)(void), int (*func2)(void));
//extern void __imapx_register_pcbtest_chg_state(int (*func1)(void));

static long resume_time;
static long suspend_time;
//static long old_time;
//static long last_time;
extern int batt_export_1180_cap;
extern int batt_export_1180_vol;

#ifdef IMAP_BATT_DEBUG
static void battery_debug_disp(void)
{
	if(batt_info.batt_state == POWER_SUPPLY_STATUS_CHARGING)
		BATT_DBG("batt_info.batt_state == POWER_SUPPLY_STATUS_CHARGING\n");
	else if(batt_info.batt_state == POWER_SUPPLY_STATUS_DISCHARGING)
		BATT_DBG("batt_info.batt_state == POWER_SUPPLY_STATUS_DISCHARGING\n");
	else if(batt_info.batt_state == POWER_SUPPLY_STATUS_NOT_CHARGING)
		BATT_DBG("batt_info.batt_state == POWER_SUPPLY_STATUS_NOT_CHARGING\n");
	else if(batt_info.batt_state == POWER_SUPPLY_STATUS_FULL)
		BATT_DBG("batt_info.batt_state == POWER_SUPPLY_STATUS_FULL\n");
	else 
		BATT_DBG("batt_info.batt_state == ERROR\n");

	if(batt_info.charger_state == BATT_CHARGER_OFF)
		BATT_DBG("batt_info.charger_state == BATT_CHARGER_OFF\n");
	else if(batt_info.charger_state == BATT_CHARGER_ON)
		BATT_DBG("batt_info.charger_state == BATT_CHARGER_ON\n");
	else
		BATT_DBG("batt_info.charger_state == ERR\n");
}
#else
static void battery_debug_disp(void){}
#endif

static void battery_api_disp(void)
{
	if(batt_info.batt_api_version == BATT_API_S_0_0)
		BATT_DBG("batt_info.batt_api_version == BATT_API_S_0_0\n");
	else if(batt_info.batt_api_version == BATT_API_S_0_1)
		BATT_DBG("batt_info.batt_api_version == BATT_API_S_0_1\n");
}

#if 0
int (*batt_get_adc_val)(void);
void __imapx_register_batt(int (*func)(void))
{
	batt_get_adc_val = func;
}
EXPORT_SYMBOL(__imapx_register_batt);
#endif
#if 0
static int imapx_get_rtc(void)
{
	return rd_get_time_sec();
}
#endif
#define imapx_get_rtc rd_get_time_sec

#if 0
static void imap_batt_save_time(u32 batt_time)
{
	rtc_alarm_set_ext(1, batt_time);
}

static long imap_batt_resume_time(void)
{
	return rtc_alarm_get_ext(1);
}

static void imap_batt_save_batt(void)
{
	u32 data, data2;
	
	data = batt_info.batt_voltage;
	if((data >= 3100)&&(data <=3600))
	{
		data -= 3000;
		data /= 10;
		BATT_DBG("save batt is %d\n", data);
		data = data << 8;
		data = data & 0xff00;
	}
	else
		data = 0;
	data2 = batt_info.batt_capacity;
	data2 &= 0x00ff;
	data2 |= 0x0080;

	data |= data2;

	rtc_alarm_set_ext(2, data);
}

static long imap_batt_resume_batt(void)
{
	return rtc_alarm_get_ext(2);
}
#endif

#if 0
/* return 
	1: charger is on
	0: charger is off*/
int isacon(void)
{
	unsigned long tmp;

    	tmp = read_rtc_gpx(2);
    
	return !!tmp; 
}
EXPORT_SYMBOL(isacon); //for TP: zet6221  suixing 2012-11-26
#endif
/*
1- FULL
0- EMPTY
*/
static int imap_batt_chg_batt_isfull(void)
{
	unsigned long tmp;
	int index;
	int abs_index;
	int pin_val;
	
#ifdef CONFIG_CHARGER_BQ
	return atomic_read(&chargingfull);
#else
	if(isacon())
	{
		msleep(300);
		if(item_exist("power.full"))
		{
			index = item_integer("power.full", 1);
			abs_index = (index>0)? index : (-1*index);
			//imapx_pad_set_mode(1,1,abs_index);
			//imapx_pad_set_dir(1,1,abs_index);
			if (gpio_is_valid(abs_index))
				pin_val = (index>0)? gpio_get_value(abs_index) : (!gpio_get_value(abs_index));
			else
				pin_val = 0;
			BATT_DBG("pin_val = 0x%x\n", pin_val);
			if(pin_val > 0)
			{
				BATT_DBG("index is %d. abs_index = %d\n", index, abs_index);
				if(batt_info.batt_voltage > charge_curve[1].vol)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		}
		else
		{
	            if(batt_info.batt_voltage > charge_curve[0].vol)
	                return 1;
	            else
	                return 0;
        		//mdelay(300);
        //		msleep(300);
		//	tmp = (read_rtc_gpx(1));
		//	tmp = !tmp;
		//	BATT_DBG("read_rtc_gpx(1) tmp = %d\n", tmp);
		}
	}
    	else 
        	tmp = 0;

	BATT_DBG("%s, retutn is %lx\n", __func__, tmp);
	return tmp;
#endif
}

void imap_batt_sta_pin_cfg(void)
{
	int index;
	int abs_index;
	int ret;

	BATT_DBG("%s\n", __func__);
	
	if(item_exist("power.full")){
		index = item_integer("power.full", 1);
		abs_index = (index>0)? index : (-1*index);

		if (gpio_is_valid(abs_index)) {
			ret = gpio_request(abs_index, "power-full");
			if (ret) {
				pr_err("failed request gpio for power-full\n");
				return;
			}
		}
//		imapx_pad_set_mode(1,1,abs_index);
  //          	imapx_pad_set_dir(1,1,abs_index);
		gpio_direction_input(abs_index);
//		imapx_pad_set_pull(abs_index, 0, 1);
		imapx_pad_set_pull(abs_index, "float");
	}
}

/*
  parameter:
  return:
 	bit 7: 1- value is exist, 0-value is error
 	bit 6-0: 0%-100%
*/
static int imap_get_priv_batt_cap(void)
{
	int reg_data = 0x7F;

	reg_data = rtcbit_get("batterycap");
	
	return reg_data;
}

static void imap_set_last_batt_cap(int value)
{
	int reg_data;

	if((value >= 0)&&(value <= 100))
	{
		reg_data = 0x80|value;
		rtcbit_set("batterycap", reg_data);
                reg_data = rtcbit_get("batterycap");
		BATT_DBG("+++ batterycap RTCINFO  +++ is %d\n", reg_data);
	}
}

static int imap_get_batt_v(int adc_val)
{
	int voltage;
	/* unit : mV */
	//BATT_DBG("%s, adc_val =%d\n", __func__, adc_val);
	
	voltage = (int)(adc_val * 5000 / 1024);
	BATT_DBG("%s, voltage = %d\n", __func__, voltage);
	return voltage;
}

static int imap_get_batt_v2cap(int batv, struct battery_curve *tbl, int count)
{
	int cnt, cap;

	BATT_DBG("%s, batv = %d\n", __func__, batv);
	if(batv >= tbl[0].vol)
	{
		cap = tbl[0].cap;	
	}
	else if(batv <= tbl[count-1].vol)
	{
		cap = tbl[count -1].cap;
	}
	else
	{
		for(cnt = 1; cnt <= count - 2; cnt ++)
		{
			if(batv >= tbl[cnt].vol)
			{
				break;
			}
		}
		cap = (tbl[cnt -1].cap - tbl[cnt].cap) * (batv - tbl[cnt].vol)/(tbl[cnt -1].vol - tbl[cnt].vol)\
			+ tbl[cnt].cap;
		BATT_DBG("%s, tbl[cnt-1].cap = %d, tbl[cnt-1].vol = %d, tbl[cnt].cap=%d, tbl[cnt].vol=%d, batv=%d, cnt=%d\n",\
			__func__, tbl[cnt-1].cap, tbl[cnt-1].vol, tbl[cnt].cap, tbl[cnt].vol, batv, cnt);
	}

	BATT_DBG("%s cap = %d\n", __func__, cap);
	return cap;
}

static int imap_get_batt_cap(int batt_v)
{
	static int cap_new = 0;
	static int cap_disp = 0;
	int delt_cap = 0;
	//long int delt_time = 0;

	BATT_DBG("%s\n", __func__);

	if(batt_info.batt_init_flag == 0)
	{
		BATT_DBG("%s, batt_info.batt_init_flag == 0, batt_info.batt_old_capacity = %d\n", __func__, batt_info.batt_old_capacity);
		if(batt_info.batt_old_capacity <= 0)
			batt_info.batt_old_capacity = 0;
		if(batt_info.batt_old_capacity >= 100)
			batt_info.batt_old_capacity = 100;
		
		return batt_info.batt_old_capacity;
	}

	if(batt_info.batt_init_flag == 5)
	{
		/* resume from suspend, first get new battery capacity to display */
		return batt_info.batt_capacity;
	}
	
	if(batt_info.charger_state == BATT_CHARGER_OFF)
	{
		cap_new = imap_get_batt_v2cap(batt_v,  discharge_curve, sizeof(discharge_curve)/sizeof(struct battery_curve));
	}
	else
	{
		cap_new = imap_get_batt_v2cap(batt_v,  charge_curve, sizeof(charge_curve)/sizeof(struct battery_curve));
		if(batt_info.batt_state == POWER_SUPPLY_STATUS_FULL)
			cap_new = 100;
	}
	
	if(batt_info.batt_init_flag == 1)
	{
		cap_disp = batt_info.batt_old_capacity;
		BATT_DBG("%s, batt_info.batt_init_flag == 1, cap_new = %d, cap_disp = %d, old_capacity = %d\n",\
			__func__, cap_new, cap_disp, batt_info.batt_old_capacity);
		battery_debug_disp();
		BATT_DBG("batt_info.batt_update_flag = %d\n", batt_info.batt_update_flag);
		

		if(batt_info.charger_state == BATT_CHARGER_ON)
		{
			//battery is charging use ac			
			if(cap_new > cap_disp)
			{	
				if(batt_info.batt_update_flag)
				{	
					batt_info.batt_update_flag = 0;
					delt_cap = cap_new - cap_disp;
					BATT_DBG("delt_cap = %d\n", delt_cap);
					if(cap_disp < 99)
						cap_disp ++;
					else if(cap_disp >= 100)
						cap_disp = 100;
					
					if(batt_info.batt_state == POWER_SUPPLY_STATUS_FULL)
						cap_disp = 100;
					
					batt_info.batt_old_capacity = cap_disp;
					if(delt_cap > 1)
					{
						if(batt_info.cap_timer_cnt_design > batt_info.cap_timer_cnt_min)
							batt_info.cap_timer_cnt_design --;
					}
					else
					{
						batt_info.cap_timer_cnt_design = batt_info.cap_timer_cnt_max;
					}
				}
			}
            
			if(batt_info.batt_low_flag == 1)
			{
				batt_info.batt_low_flag = 0;
				/*belt_scene_unset(SCENE_LOWPOWER);*/
			}
		}
		else
		{
			//battery is discharging				
			if(cap_new < cap_disp)
			{
				if(batt_info.batt_update_flag)
				{
					batt_info.batt_update_flag = 0;
					delt_cap = cap_disp - cap_new;
					BATT_DBG("delt_cap = %d\n", delt_cap);
					cap_disp --;
					batt_info.batt_old_capacity = cap_disp;
					if(delt_cap > 1)
					{
						if(batt_info.cap_timer_cnt_design > batt_info.cap_timer_cnt_min)
							batt_info.cap_timer_cnt_design -=2;
					}
					else
					{
						batt_info.cap_timer_cnt_design = batt_info.cap_timer_cnt_max;
					}
				}
			}
			if(batt_info.batt_low_flag == 0)
			{
				if(cap_disp <= batt_info.batt_param->batt_cap_low_design)
				{
					batt_info.batt_low_flag = 1;
					/*belt_scene_set(SCENE_LOWPOWER);*/
				}
			}
		}

	}

	BATT_DBG("%s, cap_disp = %d, cap_new = %d, cnt = %d\n", __func__, cap_disp, cap_new, batt_info.cap_timer_cnt_design);

	if(cap_disp <= 0)
		cap_disp = 0;
	if(cap_disp >= 100)
		cap_disp = 100;
	
	return cap_disp;
}

static struct imap_battery_param def_batt_param = {
	.batt_vol_max_design = 4200,
	.batt_vol_min_design = 3600,
	.batt_cap_low_design = 15,

	.batt_ntc_design = 10,
	.batt_temp_max_design = 60,
	.batt_temp_min_design = -20,

	.batt_technology = POWER_SUPPLY_TECHNOLOGY_LIPO,
	.batt_version = BATT_VER_0,
};

static int imap_ac_get_prop(struct power_supply *psy,
   enum power_supply_property psp,
   union power_supply_propval *val)
{	
	switch(psp){
		case POWER_SUPPLY_PROP_ONLINE:
		case POWER_SUPPLY_PROP_PRESENT:
		    if(batt_info.charger_state != BATT_CHARGER_OFF)
			val->intval = 1;
		    else
			val->intval = 0;
		    battery_debug_disp();
		    break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = batt_info.batt_voltage;
			break;
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = 2000;
			break;
		case POWER_SUPPLY_PROP_TYPE:
			val->intval = POWER_SUPPLY_TYPE_MAINS;
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static enum power_supply_property imap_ac_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_TYPE,
};

static struct power_supply imap_psy_ac = {
	.name = "ac",
	.type = POWER_SUPPLY_TYPE_MAINS,
	.properties = imap_ac_props,
	.num_properties = ARRAY_SIZE(imap_ac_props),
	.get_property = imap_ac_get_prop,
};

static int imap_batt_get_prop(struct power_supply *psy,
   enum power_supply_property psp,
   union power_supply_propval *val)
{	
	//u32 save;
	switch(psp){
		case POWER_SUPPLY_PROP_STATUS:
			val->intval = batt_info.batt_state;
			battery_debug_disp();
			break;
		case POWER_SUPPLY_PROP_HEALTH:
			val->intval = batt_info.batt_health;
			break;
		case POWER_SUPPLY_PROP_PRESENT:
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = batt_info.batt_valid;
			break;
		case POWER_SUPPLY_PROP_TECHNOLOGY:
			val->intval = batt_info.batt_param->batt_technology;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
			val->intval = batt_info.batt_param->batt_vol_max_design;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
			val->intval = batt_info.batt_param->batt_vol_min_design;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = batt_info.batt_voltage;
			break;
		//case POWER_SUPPLY_PROP_CURRENT_AVG:
		//case POWER_SUPPLY_PROP_CURRENT_NOW:
		//case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
		//case POWER_SUPPLY_PROP_ENERGY_NOW:
		case POWER_SUPPLY_PROP_CAPACITY:
			if(item_exist("power.debug") && item_equal("power.debug", "nohalt", 0))
			{
				BATT_DBG("testing batt curvetest ......  please wait!\n"); 
				BATT_DBG("[charger state] = %d, [batt state] = %d\n", batt_info.charger_state, batt_info.batt_state);
				batt_export_1180_cap = 100;
				val->intval = 100;
			}
			else
			{
				if(batt_info.batt_init_flag == 0)
				{
				    	batt_export_1180_cap = batt_info.batt_old_capacity;
					val->intval = batt_info.batt_old_capacity;
				}
				else if(batt_info.batt_init_flag == 5)
				{
					BATT_DBG("batt_info.batt_init_flag == 5, cap is %d\n", batt_info.batt_capacity);
					batt_export_1180_cap = batt_info.batt_capacity;
					val->intval = batt_info.batt_capacity;
                    batt_info.batt_old_capacity = batt_info.batt_capacity;
					batt_info.batt_init_flag = 1;
				}
				else
				{
					batt_info.batt_capacity = imap_get_batt_cap(batt_info.batt_voltage);
					batt_export_1180_cap = batt_info.batt_capacity;
					val->intval = batt_info.batt_capacity;
					imap_set_last_batt_cap(batt_info.batt_capacity);
					//imap_batt_save_batt();
					//imap_batt_save_time(imapx_get_rtc());
					if(item_exist("power.debug"))
					{
						if(!item_equal("power.debug", "nohalt", 0))
						{
							BATT_DBG("battery voltage is [%d], display_capacity is [%d]\n",\
								batt_info.batt_voltage, batt_info.batt_capacity);
						}
					}
				}
			}
			break;
			//#endif
		//case POWER_SUPPLY_PROP_TEMP:
		//case POWER_SUPPLY_PROP_TEMP_AMBIENT:

		case POWER_SUPPLY_PROP_MODEL_NAME:
			val->strval = "imap820_battery";
			break;
		case POWER_SUPPLY_PROP_MANUFACTURER:
			val->strval = "infotm";
			break;
		case POWER_SUPPLY_PROP_TYPE:
			val->intval = POWER_SUPPLY_TYPE_BATTERY;
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static enum power_supply_property imap_batt_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_TECHNOLOGY,

	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,

	//POWER_SUPPLY_PROP_CURRENT_AVG,
	//POWER_SUPPLY_PROP_CURRENT_NOW,

	//POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN,
	//POWER_SUPPLY_PROP_ENERGY_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	
	//POWER_SUPPLY_PROP_TEMP,
	//POWER_SUPPLY_PROP_TEMP_AMBIENT,

	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_MANUFACTURER,
	POWER_SUPPLY_PROP_TYPE,
};

static struct power_supply imap_psy_batt = {
	.name = "battery",
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.properties = imap_batt_props,
	.num_properties = ARRAY_SIZE(imap_batt_props),
	.get_property = imap_batt_get_prop,
};

static void imap_charger_mon(void)
{
	int tmp, tmp2;
	BATT_DBG("+++++++++++\n");
	BATT_DBG("in %s\n", __func__);
	battery_debug_disp();
	if(batt_info.charger_state == BATT_CHARGER_ON)
	{
		tmp = isacon();
		BATT_DBG("tmp = %d\n",tmp);
		if(tmp == 0)
		{
			batt_info.charger_state = BATT_CHARGER_OFF;
			batt_info.batt_state = POWER_SUPPLY_STATUS_DISCHARGING;
			power_supply_changed(&imap_psy_ac);
		}
		else
		{

			tmp2 = imap_batt_chg_batt_isfull();
			BATT_DBG("tmp2 = %d\n",tmp2);
			if(batt_info.batt_state == POWER_SUPPLY_STATUS_FULL)
			{
				if(tmp2 == 0)
				{
					BATT_DBG("batt is charging\n");
					batt_info.batt_state = POWER_SUPPLY_STATUS_CHARGING;
					power_supply_changed(&imap_psy_ac);
				}
			}
			else
			{	
				if(tmp2)
				{
					BATT_DBG("batt is full\n");
                    			batt_info.batt_capacity = 100;
                    			batt_info.batt_old_capacity = 100;
					batt_info.batt_state = POWER_SUPPLY_STATUS_FULL;
				}
			}
			power_supply_changed(&imap_psy_batt);
			power_supply_changed(&imap_psy_ac);
		}
	}
	else
	{
		if(isacon() == 1)
		{
			BATT_DBG("AC Charger is on\n");
			batt_info.charger_state = BATT_CHARGER_ON;
			batt_info.batt_state = POWER_SUPPLY_STATUS_CHARGING;
			power_supply_changed(&imap_psy_ac); 
		}
	}
	
	BATT_DBG("out %s\n", __func__);
	BATT_DBG("----------\n");
	battery_debug_disp();
}

static int average(int *p_val, int ave_num)
{
	int i = 0;
	int max, min, sum, temp;

	if(ave_num <= 0)
		return 0;
	else if(ave_num == 1)
		return *p_val;
	else if(ave_num == 2)
		return (*p_val + *(p_val+1))/2;
	else
	{
		sum = 0;
		max = min = *p_val;
		for(i = 0; i<ave_num; i++)
		{
			temp = (*p_val + i);
			sum += temp;
			if(temp > max)
				max = temp;
			if(temp < min)
				min = temp;
		}	
		sum -= (max + min);
		return (int)(sum /(ave_num -2));
	}
}

static void imap_battery_mon_update(void)
{
	static int count = 0;
	static int bat_v[AVE_NUM];
	static int adc_temp;
	static int batt_val_temp;

	adc_temp = batt_get_adc_val();
	bat_v[count] = adc_temp;
	batt_val_temp = (int)(adc_temp * 5000/1024);
	//bat_v[count] = batt_get_adc_val();
	if(item_exist("power.debug"))
		if(item_equal("power.debug", "nohalt", 0))
			BATT_DBG(" ****** adc = [%d],    battery = [%d] ****** for test only\n", adc_temp, batt_val_temp);
	count ++;

	if(count == AVE_NUM)
	{
		if(batt_info.batt_init_flag == 0)
		{
			batt_info.batt_init_flag = 1;
		}
		count = 0;
		batt_info.batt_voltage = imap_get_batt_v(average(bat_v, AVE_NUM));
	}
}

static int imap_battery_mon(void *p)
{
	static int batt_old_v = 0;
	unsigned long time, num, cnt;
	int batt_v;
	
	BATT_DBG("%s", __func__);
	
	time = msecs_to_jiffies(1000);

	num = 0;
	cnt = 0;
	batt_v = imap_get_batt_v(batt_get_adc_val());

	if(isacon()) {//charging
	    if(batt_v >= 4050)
		num = 1;
	    else if(batt_v >= 4000)
		num = 2;
	    else
		num = 5;
	}
	else
	{//discharging
            if(batt_v <= 3600)
	        num = 1;
	    else if(batt_v <= 3650)
	        num = 2;
	    else
	        num = 5;
	}
		
	while(1)
	{
		schedule_timeout_uninterruptible(time);
		down(&batt_info.mon_sem);

		imap_charger_mon();

		if(cnt == num)
		{
		    BATT_DBG("cnt == num\n");
			batt_info.cap_cnt ++;
			if(batt_info.cap_cnt >= batt_info.cap_timer_cnt_design)
			{
				batt_info.batt_update_flag = 1;
				batt_info.cap_cnt = 0;
			}
			imap_battery_mon_update();
			if(batt_old_v != batt_info.batt_voltage)
			{
				batt_old_v = batt_info.batt_voltage;
				power_supply_changed(&imap_psy_batt);
			}

			batt_v = imap_get_batt_v(batt_get_adc_val());

			if(isacon()) 
			{//charging
		    		if(batt_v >= 4050)
					num = 1;
		    		else if(batt_v >= 4000)
					num = 2;
		    		else
					num = 5;
			}
			else
			{//discharging
		    		if(batt_v <= 3600)
					num = 1;
		    		else if(batt_v <= 3650)
					num = 2;
		    		else
					num = 5;
			}
		
			cnt = 0;
		}
		else
		{
			cnt ++;
		}

		up(&batt_info.mon_sem);
	}

	return 0;
}

/*
static void imap_battery_debug(void)
{
	batt_info.batt_health = POWER_SUPPLY_HEALTH_GOOD;
	batt_info.batt_capacity = 50;
	batt_info.batt_voltage = 3684;
	batt_info.batt_valid = 1;
}
*/

static void imap_battery_item_curve (void)
{
	int i;
	
	BATT_DBG("%s\n", __func__);
	
	if(item_exist("power.curve.charge"))
	{
		for (i = 0;i < 12;i++)
		{
			charge_curve[i].vol = item_integer("power.curve.charge", i);
			BATT_DBG("i=%d, cap [%d] = vol [%d]\n", i, charge_curve[i].cap, charge_curve[i].vol);
		}
	}

	if(item_exist("power.curve.discharge"))
	{
		for (i = 0;i < 12;i++)
		{
			discharge_curve[i].vol = item_integer("power.curve.discharge", i);
			BATT_DBG("i=%d, cap [%d] = vol [%d]\n", i,  discharge_curve[i].cap, discharge_curve[i].vol);
		}
	}

    if(item_exist("batt.icc"))
    {
        batt_info.batt_icc = item_integer("batt.icc", 0);
    }
    else
    {
        batt_info.batt_icc = 1500;
    }
}

static void imap_battery_get_first_voltage(void)
{
	int i, reg_data;
	int batt_v[4], batt_ave, cap_new, cap_priv;

	BATT_DBG("imap_battery_get_first_voltage() \n");
	/* Get new voltage */
	for(i = 0; i<4; i++)
	{
		batt_v[i] = imap_get_batt_v(batt_get_adc_val());
	}

	batt_ave = average(batt_v, 4);
    	if(batt_info.charger_state == BATT_CHARGER_OFF)
	{
		cap_new = imap_get_batt_v2cap(batt_ave,  discharge_curve, sizeof(discharge_curve)/sizeof(struct battery_curve));
	}
	else
	{
		cap_new = imap_get_batt_v2cap(batt_ave,  charge_curve, sizeof(charge_curve)/sizeof(struct battery_curve));
	}	

    	if(imapx15_unexpect_shut() != 0)
    	{
        	BATT_DBG("Unexpect_shut down is occur, we will calculator again!/n");
        	reg_data = 0;
                rtcbit_set("batterycap", reg_data);
                reg_data = rtcbit_get("batterycap");
        	BATT_DBG("+++ batterycap RTCINFO +++ is %d\n", reg_data);
    	}
    	else
    	{
        	BATT_DBG("Expect shut down is occur, we will use the private capacity!/n");
    	}

	/* Get old cap and check it */
	cap_priv = imap_get_priv_batt_cap();
	//BATT_DBG("get prive cap: reg_data = %d\n", (cap_priv-0x80));
	if(cap_priv & 0x80)
	{
		cap_priv &= 0x7F;
		//BATT_DBG("%s, reg_data is %d\n", cap_priv);
		if((cap_priv > 0)&&(cap_priv <= 100))
		{
			batt_info.batt_init_flag = 0;
			batt_info.batt_old_capacity= cap_priv;
		}
		else if(cap_priv == 0)
		{
		    	batt_info.batt_init_flag = 0;	
			batt_info.batt_old_capacity= cap_new/20;
		}
		else
		{
			batt_info.batt_init_flag = 4;
		}
	}
	else
	{
		batt_info.batt_init_flag = 4;
	}
	BATT_DBG("batt_info.batt_old_capacity = %d, cap_new = %d\n", batt_info.batt_old_capacity, cap_new);

	/* Calculate the last cap to display */	
	if(batt_info.batt_init_flag == 4)
	{	
		BATT_DBG("batt_info.batt_init_flag == 4\n");
		batt_info.batt_old_capacity = cap_new;
		batt_info.batt_init_flag =0;
	}
	
	if(batt_info.batt_old_capacity <= 0)
		batt_info.batt_old_capacity = 0;
	if(batt_info.batt_old_capacity >=100)
		batt_info.batt_old_capacity = 100;

	batt_info.batt_capacity = batt_info.batt_old_capacity;
	BATT_DBG("imap_battery_get_first_voltage *** batt_info.batt_capacity = %d\n", batt_info.batt_capacity);
}

static void imap_battery_get_initialization(void)
{
	//int reg_data;
	//int usb_state;	

	BATT_DBG("%s\n", __func__);

	batt_info.batt_init_flag = 0;
	batt_info.batt_param = &def_batt_param;

	/* check start mode */
	if(strstr(boot_command_line, "charger"))
	{
		BATT_DBG("%s sys_mode = charger mode\n", __func__);
		batt_info.sys_mode = 1;
	}
	else
	{
		BATT_DBG("%s sys_mode = NOT charger mode\n", __func__);
		batt_info.sys_mode = 0;
	}

	/* get battery curve */
	imap_battery_item_curve();

	/* get charger state */
	if(isacon())
	{
	        BATT_DBG("Charger is on\n");
		batt_info.charger_state = BATT_CHARGER_ON;
		batt_info.batt_state = POWER_SUPPLY_STATUS_CHARGING;
		battery_debug_disp();
	}
	else
	{
		BATT_DBG("Charger is off\n");
		batt_info.batt_state = POWER_SUPPLY_STATUS_DISCHARGING;
		batt_info.charger_state = BATT_CHARGER_OFF;
		battery_debug_disp();
	}

	/* initialize the battery info */
	batt_info.batt_health = POWER_SUPPLY_HEALTH_GOOD;
	batt_info.batt_old_capacity = 100;
	batt_info.cap_timer_cnt_max = 10 * 2;// 2 sec = 10 * 2 * 0.5 sec
	batt_info.cap_timer_cnt_min = 5;
	batt_info.cap_timer_cnt_design = batt_info.cap_timer_cnt_max;
	batt_info.cap_cnt = 0;
	batt_info.batt_update_flag = 1;
	batt_info.batt_low_flag = 0;

	/* get battery voltage and cap value first */
	imap_battery_get_first_voltage();	

	suspend_time = 0;
	resume_time = 0;

	/* check charger is full */
	if(isacon())
	{
		BATT_DBG("AC Charger is on\n");
		if(imap_batt_chg_batt_isfull())
		{
			batt_info.charger_state = BATT_CHARGER_ON;
			batt_info.batt_state = POWER_SUPPLY_STATUS_FULL;	
			batt_info.batt_capacity = 100;
			batt_info.batt_old_capacity = 100;
		}
		else
		{
			batt_info.charger_state = BATT_CHARGER_ON;
			batt_info.batt_state = POWER_SUPPLY_STATUS_CHARGING;
			if(batt_info.batt_capacity >= 100)
			{
				 batt_info.batt_capacity = 99;
				 batt_info.batt_old_capacity = 99;
			}
		}
	}
	else
	{
		batt_info.charger_state = BATT_CHARGER_OFF;
		batt_info.batt_state = POWER_SUPPLY_STATUS_DISCHARGING;
	}
	battery_debug_disp();

	BATT_DBG("%s, batt_info.batt_old_capacity is %d\n", __func__, batt_info.batt_old_capacity);
}

int bat_pcbtest_voltage;

int imap_battery_pcbtest_get_voltage(void)
{
     int adc_pcb;
     adc_pcb = batt_get_adc_val();
     bat_pcbtest_voltage = (int)(adc_pcb * 5000/1024);
     return bat_pcbtest_voltage;
}
 
int imap_battery_pcbtest_get_cap(void)
{
     int bat_cap;

     if(isacon())
     {
         bat_cap = imap_get_batt_v2cap(bat_pcbtest_voltage, charge_curve, sizeof(charge_curve)/sizeof(struct battery_curve));
     }
     else
     {
         bat_cap = imap_get_batt_v2cap(bat_pcbtest_voltage, discharge_curve, sizeof(discharge_curve)/sizeof(struct battery_curve));
     }
 
     return bat_cap;
}

static struct task_struct *batt_mon_task;
static int imap_battery_probe(struct platform_device *pdev)
{
	int ret = 0;

	BATT_DBG("%s\n", __func__);

	batt_info.batt_api_version = BATT_API_S_0_0;
	battery_api_disp();
	
	batt_info.imap_psy_batt = &imap_psy_batt;
	batt_info.imap_psy_ac = &imap_psy_ac;	
	imap_batt_sta_pin_cfg();
	imap_battery_get_initialization();
	
	ret = power_supply_register(&pdev->dev, batt_info.imap_psy_ac);
	if (ret)
		goto ac_psy_failed;

	ret = power_supply_register(&pdev->dev, batt_info.imap_psy_batt);
	if (ret)
		goto battery_psy_failed;
	sema_init(&batt_info.mon_sem,1);
	batt_info.monitor_battery = (struct task_struct *)imap_battery_mon;
	batt_info.mon_battery_interval = IMAP_MON_INTVAL;
	batt_mon_task = kthread_run(imap_battery_mon, NULL, "IMAP BATTERY MONITOR");
	if(IS_ERR(batt_mon_task))
		goto mon_failed;

	//imap_battery_debug();
	power_supply_changed(&imap_psy_batt);
	power_supply_changed(&imap_psy_ac);

	//__imapx_register_pcbtest_batt_v(imap_battery_pcbtest_get_voltage, imap_battery_pcbtest_get_cap);
	//__imapx_register_pcbtest_chg_state(isacon);
	
	BATT_DBG("%s success\n", __func__);
	goto success;
	
mon_failed:
	if(batt_info.monitor_battery){
		release_thread(batt_info.monitor_battery);
		batt_info.monitor_battery = NULL;
	}
battery_psy_failed:
	power_supply_unregister(&imap_psy_batt);
ac_psy_failed:
	power_supply_unregister(&imap_psy_ac);
	platform_device_unregister(pdev);
	BATT_DBG("%s fail\n", __func__);
success:
	return ret;

}

static int imap_battery_remove(struct platform_device *pdev)
{
	BATT_DBG("%s\n", __func__);

	if(batt_mon_task)
		kthread_stop(batt_mon_task);
		
	power_supply_unregister(&imap_psy_batt);
	power_supply_unregister(&imap_psy_ac);	
	platform_device_unregister(pdev);
	
	return 0;
}

#ifdef CONFIG_PM
static int imap_battery_suspend(struct platform_device *pdev, pm_message_t state) 
{
	BATT_DBG("+++ %s +++\n", __func__);
	suspend_time = imapx_get_rtc();
	imap_set_last_batt_cap(batt_info.batt_capacity);
	//imap_batt_save_batt();
	//imap_batt_save_time(suspend_time);
	
	return 0;
}

static int imap_battery_resume(struct platform_device *pdev) 
{
	int re_batt_v, re_cap, re_charger_state;
	u32 re_delt_time, bat_cap, delt_cap, new_cap;
	BATT_DBG("+++  %s +++\n", __func__);

	/* get now time */
	resume_time = imapx_get_rtc();

	/* calcute the delt time */
	if(resume_time < suspend_time)
		re_delt_time = resume_time + suspend_time;
	else
		re_delt_time = resume_time - suspend_time;
	BATT_DBG("time :[%d] = [%ld] - [%ld]\n:", re_delt_time, resume_time, suspend_time);
	re_delt_time = re_delt_time / 60;
	
	/* get charger state */
	re_charger_state = isacon();
	BATT_DBG("re_charger_state = %d\n", re_charger_state);

	/* get new voltage */
	re_batt_v = imap_get_batt_v(batt_get_adc_val());
	BATT_DBG("re_batt_v = %d\n", re_batt_v);

	batt_info.batt_old_capacity = batt_info.batt_capacity;
	
	if(batt_info.charger_state == BATT_CHARGER_OFF)
	{
		re_cap = imap_get_batt_v2cap(re_batt_v,  discharge_curve, sizeof(discharge_curve)/sizeof(struct battery_curve));
	}
	else
	{
	        re_cap = imap_get_batt_v2cap(re_batt_v,  charge_curve, sizeof(charge_curve)/sizeof(struct battery_curve));
	}

	if(batt_info.charger_state == BATT_CHARGER_ON)
	{
	    	if(item_exist("batt.capacity"))
	    	{
	        	bat_cap = item_integer("batt.capacity", 0);

	    		bat_cap *= 60;
	    		new_cap = batt_info.batt_icc * re_delt_time;
	    		delt_cap = new_cap * 100 / bat_cap;
	    		batt_info.batt_capacity += delt_cap;
	    		BATT_DBG("bat_cap = %d mAmin\n", bat_cap);
	    		BATT_DBG("new_cap = %d mAmin, re_delt_time= %d\n", new_cap, re_delt_time);
	    		BATT_DBG("delt_cap = %d mAmin\n", delt_cap);
	    		BATT_DBG("re_cap = %d mAmin\n", re_cap);
		}
		else
		{
			if(re_cap > batt_info.batt_old_capacity)
				batt_info.batt_capacity = re_cap;
		}

		if(re_charger_state)
		{
			if(imap_batt_chg_batt_isfull())
			{
				batt_info.batt_capacity = 100;
			}
			else
			{
				if(batt_info.batt_capacity == 100)
					batt_info.batt_capacity = 99;
			}
		}
	}
	else
	{
		batt_info.batt_capacity = batt_info.batt_old_capacity;
	}
	
	if(batt_info.batt_capacity <= 0)
		batt_info.batt_capacity = 0;
	if(batt_info.batt_capacity >=100)
		batt_info.batt_capacity = 100;

	if(re_charger_state)
	{
	    	batt_info.charger_state = BATT_CHARGER_ON;
		if(batt_info.batt_capacity == 100)
	        	batt_info.batt_state = POWER_SUPPLY_STATUS_FULL;
		else
	        	batt_info.batt_state = POWER_SUPPLY_STATUS_CHARGING;
	}
	else
	{
		batt_info.charger_state = BATT_CHARGER_OFF;
	    	batt_info.batt_state = POWER_SUPPLY_STATUS_DISCHARGING;
	}

	imap_set_last_batt_cap(batt_info.batt_capacity);
	//imap_batt_save_batt();
	//imap_batt_save_time(imapx_get_rtc());

	power_supply_changed(&imap_psy_batt);
	power_supply_changed(&imap_psy_ac);
	batt_info.batt_init_flag = 5;
	BATT_DBG("batt_info.batt_capacity is %d\n", batt_info.batt_capacity);
	BATT_DBG("---  %s ---\n\n", __func__);
	return 0;
}
#else
#define  imap_battery_suspend NULL
#define  imap_battery_resume  NULL
#endif


static struct platform_driver imap_battery_driver = {
	.probe = imap_battery_probe,
	.remove = imap_battery_remove,
	.suspend = imap_battery_suspend,
	.resume = imap_battery_resume,
	.driver = {
		.name = "imap820_battery",
		.owner = THIS_MODULE,
	}
};

static int __init imap_bat_init(void)
{
	int ret = 0;
	int flag_chg_pwron, flag_chg_enable;
	flag_chg_pwron = 0;
	flag_chg_enable = 0;

	if(item_exist("pmu.model")) 
	{
            	if(!item_equal("pmu.model", "tps65910", 0) && !item_equal("pmu.model", "axp152", 0)) 
	    	{
        	    	BATT_DBG("********** PMU isn't tps65910 or axp152, iMAPX820_BATTERY Driver Not install **********\n");
			return 0;
        	}
    	}

	if(item_exist("pmu.mode"))
	{
		if(item_equal("pmu.mode", "noFGmode2", 0))
		{
		    	BATT_DBG("********** PMU is tps65910 or axp152, but pmu.mode is noFGmode2, Imapx820_battery Driver is not run!\n");
			return 0;
		}
	}
	
	if(item_exist("power.acboot"))
	{
		if(item_equal("power.acboot", "1", 0))
			flag_chg_pwron = 1;
	}

	if(item_exist("power.acboot"))
	{
		if(item_equal("power.acboot", "1", 0))
			flag_chg_enable = 1;
	}

	if((flag_chg_enable == 0)&&(flag_chg_pwron == 0))
	{
		BATT_DBG("********** iMAPX820_BATTERY Driver Not install**********\n");
		return 0;
	}
	
	BATT_DBG("********** iMAPX820_BATTERY Driver **********\n");
	BATT_DBG("%s\n", __func__);

	rtc_pwrwarning_mask(1);
	ret = platform_driver_register(&imap_battery_driver);
	if(ret < 0)
	{
		BATT_DBG("ERR: drvier register fail\n");
		return ret;
	}
	
	BATT_DBG("init success\n");
	return 0;
}

static void __exit imap_bat_exit(void)
{
	BATT_DBG("%s\n", __func__);

	platform_driver_unregister(&imap_battery_driver);
}

module_init(imap_bat_init);
module_exit(imap_bat_exit);


MODULE_AUTHOR("jack <jack.zhang@infotmic.com.cn>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Battery driver for iMAPx820");
