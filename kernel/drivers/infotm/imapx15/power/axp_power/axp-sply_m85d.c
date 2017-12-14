/*
 * Battery charger driver for X-Powers AXP
 *
 * Copyright (C) 2011 X-Powers, Ltd.
 *  Zhang Donglu <zhangdonglu@x-powers.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>

#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <linux/seq_file.h>
#include <linux/input.h>
#include <asm/div64.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>
#include <mach/pad.h>
#include <mach/items.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define DBG_AXP_PSY 1
#if  DBG_AXP_PSY
#define DBG_PSY_MSG(format,args...)   printk(KERN_ERR "[AXP]"format,##args)
#else
#define DBG_PSY_MSG(format,args...)   do {} while (0)
#endif

#include "axp-sply.h"
static void axp_charger_update_state(struct axp_charger *charger);
static void axp_charger_update(struct axp_charger *charger);
static int axp_ibat_to_mA(uint16_t reg);
static void axp_close(struct axp_charger *charger);
static int pwrkey_used, irq_used;
static uint32_t irq_low, irq_high;
#include "axp-init.h"
#include "axp-debug.h"
//#include "axp-noirq.h"

#include "axp-calocv.h"
#include "axp-calcou.h"
#include "axp-common.h"

#ifdef CONFIG_HAS_EARLYSUSPEND
static struct early_suspend axp_early_suspend;
#endif

extern int read_rtc_gpx(int);
extern int32_t get_otg_charge_mode(void);
//extern void vbus_en_pull(int);
//extern int batt_export_axp202_cap;

extern int pcf8563_get_alarm_int_state(void);
extern void pcf8563_clear_alarm_int(void);
extern int pcf8563_rtc_alarmirq(void);

int ac_state;
//static int monitor_times_ac;
//static int monitor_times_usb;

/*
 * TODO:add for compiling, will be removed after OTG driver complete.
 */
/*static int get_otg_charge_mode(void)
{
	return 0;
}*/

static inline int axp_vbat_to_mV(uint16_t reg)
{
	return (reg) * 1100 / 1000;
}

static inline int axp_vdc_to_mV(uint16_t reg)
{
	return (reg) * 1700 / 1000;
}

static inline int axp_ibat_to_mA(uint16_t reg)
{
	return (reg) * 500 / 1000;
}

static inline int axp_iac_to_mA(uint16_t reg)
{
	return (reg) * 625 / 1000;
}

static inline int axp_iusb_to_mA(uint16_t reg)
{
	return (reg) * 375 / 1000;
}

static inline void axp_read_adc(struct axp_charger *charger,
					struct axp_adc_res *adc)
{
	uint8_t tmp[8];
	axp_reads(charger->master,AXP_VACH_RES,8,tmp);
	adc->vac_res = ((uint16_t) tmp[0] << 4 )| (tmp[1] & 0x0f);
	adc->iac_res = ((uint16_t) tmp[2] << 4 )| (tmp[3] & 0x0f);
	adc->vusb_res = ((uint16_t) tmp[4] << 4 )| (tmp[5] & 0x0f);
	adc->iusb_res = ((uint16_t) tmp[6] << 4 )| (tmp[7] & 0x0f);
	axp_reads(charger->master,AXP_VBATH_RES,6,tmp);
	adc->vbat_res = ((uint16_t) tmp[0] << 4 )| (tmp[1] & 0x0f);
#if defined (CONFIG_KP_AXP20)
	adc->ichar_res = ((uint16_t) tmp[2] << 4 )| (tmp[3] & 0x0f);
#endif
#if defined (CONFIG_KP_AXP19)
	adc->ichar_res = ((uint16_t) tmp[2] << 5 )| (tmp[3] & 0x1f);
#endif
	adc->idischar_res = ((uint16_t) tmp[4] << 5 )| (tmp[5] & 0x1f);
}

static void axp_charger_update_state(struct axp_charger *charger)
{
	uint8_t val[2];
	uint16_t tmp;

	axp_reads(charger->master,AXP_CHARGE_STATUS,2,val);
	tmp = (val[1] << 8 )+ val[0];
	charger->is_on 					= (tmp & AXP_STATUS_INCHAR)?1:0;
	charger->bat_det 				= (tmp & AXP_STATUS_BATEN)?1:0;
	charger->ac_det 				= (tmp & AXP_STATUS_ACEN)?1:0;
	charger->usb_det 				= (tmp & AXP_STATUS_USBEN)?1:0;
	charger->usb_valid 				= (tmp & AXP_STATUS_USBVA)?1:0;
	charger->ac_valid 				= (tmp & AXP_STATUS_ACVA)?1:0;
	charger->ext_valid 				= charger->ac_valid | charger->usb_valid;
	charger->bat_current_direction 	= (tmp & AXP_STATUS_BATCURDIR)?1:0;
	charger->in_short 				= (tmp& AXP_STATUS_ACUSBSH)?1:0;
	charger->batery_active 			= (tmp & AXP_STATUS_BATINACT)?1:0;
	charger->low_charge_current 	= (tmp & AXP_STATUS_CHACURLOEXP)?1:0;
	charger->int_over_temp 			= (tmp & AXP_STATUS_ICTEMOV)?1:0;
	axp_read(charger->master,AXP_CHARGE_CONTROL1,val);
	charger->charge_on 				= ((val[0] & 0x80)?1:0);
}

static void axp_charger_update(struct axp_charger *charger)
{
	uint16_t tmp;
	axp_read_adc(charger, &charger->adc);
	tmp = charger->adc.vbat_res;
	charger->vbat = axp_vbat_to_mV(tmp);
	charger->ibat = ABS(axp_ibat_to_mA(charger->adc.ichar_res)-axp_ibat_to_mA(charger->adc.idischar_res));
	tmp = charger->adc.vac_res;
	charger->vac = axp_vdc_to_mV(tmp);
	tmp = charger->adc.iac_res;
	charger->iac = axp_iac_to_mA(tmp);
	tmp = charger->adc.vusb_res;
	charger->vusb = axp_vdc_to_mV(tmp);
	tmp = charger->adc.iusb_res;
	charger->iusb = axp_iusb_to_mA(tmp);
	if(!charger->ext_valid){
		charger->disvbat =  charger->vbat;
		charger->disibat =  charger->ibat;
	}
}

static enum power_supply_property axp_battery_props[] = {
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
};

static enum power_supply_property axp_ac_props[] = {
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
};

static enum power_supply_property axp_usb_props[] = {
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
};

static void axp_battery_check_status(struct axp_charger *charger,
						union power_supply_propval *val)
{
	if (charger->bat_det) {
		if (charger->ext_valid){
			if( charger->rest_cap == 100)
				val->intval = POWER_SUPPLY_STATUS_FULL;
			else if(charger->charge_on)
				val->intval = POWER_SUPPLY_STATUS_CHARGING;
			else
				val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
		} else {
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		}
	} else {
		val->intval = POWER_SUPPLY_STATUS_FULL;
	}
}

static void axp_battery_check_health(struct axp_charger *charger,
						union power_supply_propval *val)
{
	val->intval = POWER_SUPPLY_HEALTH_GOOD;
}

static int axp_battery_get_property(struct power_supply *psy,
						enum power_supply_property psp,
						union power_supply_propval *val)
{
	struct axp_charger *charger;
	int ret = 0;
	charger = container_of(psy, struct axp_charger, batt);

	switch (psp) {
		case POWER_SUPPLY_PROP_STATUS:
			axp_battery_check_status(charger, val);
			break;
		case POWER_SUPPLY_PROP_HEALTH:
			axp_battery_check_health(charger, val);
			break;
		case POWER_SUPPLY_PROP_TECHNOLOGY:
			val->intval = charger->battery_info->technology;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
			val->intval = charger->battery_info->voltage_max_design;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
			val->intval = charger->battery_info->voltage_min_design;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = charger->ocv * 1000;
			break;
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = charger->ibat * 1000;
			break;
		case POWER_SUPPLY_PROP_MODEL_NAME:
			val->strval = charger->batt.name;
			break;
		case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
			val->intval = charger->battery_info->energy_full_design;
			break;
		case POWER_SUPPLY_PROP_CAPACITY:
			//batt_export_axp202_cap = charger->rest_cap;
			val->intval = charger->rest_cap;
			break;
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = (!charger->is_on)&&(charger->bat_det) && (! charger->ext_valid);
			break;
		case POWER_SUPPLY_PROP_PRESENT:
			val->intval = charger->bat_det;
			break;
		case POWER_SUPPLY_PROP_TEMP:
			val->intval =  300;
			break;
		default:
			ret = -EINVAL;
			break;
	}

	return ret;
}

static int axp_ac_get_property(struct power_supply *psy,
						enum power_supply_property psp,
						union power_supply_propval *val)
{
	struct axp_charger *charger;
	int ret = 0;
	charger = container_of(psy, struct axp_charger, ac);

	switch(psp) {
		case POWER_SUPPLY_PROP_MODEL_NAME:
			val->strval = charger->ac.name;
			break;
		case POWER_SUPPLY_PROP_PRESENT:
			val->intval = charger->ac_det;
			break;
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = (ac_state || (charger->flag == 1)) ? 1 : 0;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = charger->vac * 1000;
			break;
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = charger->iac * 1000;
			break;
		default:
			ret = -EINVAL;
			break;
	}
	return ret;
}

static int axp_usb_get_property(struct power_supply *psy,
						enum power_supply_property psp,
						union power_supply_propval *val)
{
	struct axp_charger *charger;
	int ret = 0;
	charger = container_of(psy, struct axp_charger, usb);

	switch(psp){
		case POWER_SUPPLY_PROP_MODEL_NAME:
			val->strval = charger->usb.name;
			break;
		case POWER_SUPPLY_PROP_PRESENT:
			val->intval = charger->usb_det;
			break;
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = (charger->flag == 2) ? 1 : 0;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = charger->vusb * 1000;
			break;
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = charger->iusb * 1000;
			break;
		default:
			ret = -EINVAL;
			break;
	}
	return ret;
}

static void axp_charging_update(struct axp_charger *charger)
{
	DBG_PSY_MSG("battery status update\n");
	axp_charger_update_state(charger);
	axp_charger_update(charger);
	state_count = 1;
}
static void axp_change(struct axp_charger *charger)
{
	DBG_PSY_MSG("battery state change\n");

	power_supply_changed(&charger->batt);
	power_supply_changed(&charger->ac);
	power_supply_changed(&charger->usb);
}

static void axp_powerkey_press(struct axp_charger *charger)
{
	DBG_PSY_MSG("axp powerkey press\n");
//#if defined (CONFIG_AXP_PEKINFO)
	input_report_key(powerkeydev, KEY_POWER, 1);
	input_sync(powerkeydev);
//#endif
}

static void axp_powerkey_release(struct axp_charger *charger)
{
	DBG_PSY_MSG("axp powerkey release\n");
//#if defined (CONFIG_AXP_PEKINFO)
	input_report_key(powerkeydev, KEY_POWER, 0);
	input_sync(powerkeydev);
//#endif
}

static void axp_close(struct axp_charger *charger)
{
	DBG_PSY_MSG("axp_close\n");
	charger->rest_cap = SHUTDOWNRATE;
#if defined (CONFIG_KP_AXP20)
	axp_write(charger->master, AXP_DATA_BUFFER1, (0x80 | SHUTDOWNRATE));
#endif
	DBG_PSY_MSG("\n==================event in close==============\n");
	power_supply_changed(&charger->batt);
}

static void axp_close_vbus_ipsout(struct axp_charger *charger)
{
	DBG_PSY_MSG("------axp close vbus-ipsout------\n");
	axp_clr_bits(charger->master, AXP_IPS_SET, (1<<7));
	//vbus_en_pull(1);/*VBUS_EN high*/
	axp_vbusen_ctrl(1);
}

static int axp_battery_event(struct notifier_block *nb, unsigned long event,
							void *data)
{
	struct axp_charger *charger = container_of(nb, struct axp_charger, nb);
	uint32_t irqs_low = ((uint32_t *)data)[0];
	uint32_t irqs_high = ((uint32_t *)data)[1];
	uint8_t val;

	if(irqs_low & AXP_IRQ_CHAOV) {
		DBG_PSY_MSG("charging over\n");
		axp_charging_update(charger);
		axp_change(charger);
	}

	if(irqs_low & AXP_IRQ_CHAST) {
		DBG_PSY_MSG("charging on\n");
		axp_charging_update(charger);
		axp_change(charger);
	}

	if(irqs_low & AXP_IRQ_CHACURLO) {
		DBG_PSY_MSG("charging current low\n");

		axp_read(charger->master, AXP_IPS_SET, &val);
		DBG_PSY_MSG("VBUS-IPSOUT reg=0x%x\n", val);
		axp_read(charger->master, 0x33, &val);
		DBG_PSY_MSG("CHARGING CTRL = 0x%x\n", val);
	}

	if(irqs_low & (AXP_IRQ_ACIN | AXP_IRQ_ACRE)) {
		axp_charging_update(charger);
		schedule_delayed_work(&charger->check_ac_work, msecs_to_jiffies(500));
	}

	if(irqs_low & AXP_IRQ_USBIN) {
		if (charger->probe_over == 1)
			wake_lock_timeout(&charger->axp_wake_lock, 5 * HZ);

		axp_charging_update(charger);
		schedule_delayed_work(&charger->check_usb_work, msecs_to_jiffies(1500));
	}

	if(irqs_low & AXP_IRQ_USBRE) {
		axp_charging_update(charger);
		schedule_delayed_work(&charger->check_usb_work, msecs_to_jiffies(500));
	}

	if(irqs_low & (AXP_IRQ_ACOV|AXP_IRQ_USBOV|AXP_IRQ_ICTEMOV)) {
		if(irqs_low & (AXP_IRQ_ACOV|AXP_IRQ_USBOV))
			DBG_PSY_MSG("charger voltage too high!\n");
		else if(irqs_low & AXP_IRQ_ICTEMOV)
			DBG_PSY_MSG("IC internal over heat!\n");

		axp_close_vbus_ipsout(charger);

		axp_charging_update(charger);
		axp_change(charger);
	}

	if(irqs_high & AXP_IRQ_PEKFE) {
		axp_powerkey_press(charger);
	}

	if(irqs_high & AXP_IRQ_PEKRE) {
		axp_powerkey_release(charger);
	}

	if (irqs_high & AXP_IRQ_GPIO2TG) {
		if (pcf8563_get_alarm_int_state()) {
			pcf8563_rtc_alarmirq();
			pcf8563_clear_alarm_int();
		}
	}

	DBG_PSY_MSG("m85d   irq_low = 0x%x, irq_high = 0x%x\n", irqs_low, irqs_high);

	return 0;
}

static char *supply_list[] = {
	"axp-battery",
};  

static void axp_battery_setup_psy(struct axp_charger *charger)
{
	struct power_supply *batt = &charger->batt;
	struct power_supply *ac = &charger->ac;
	struct power_supply *usb = &charger->usb;
	struct power_supply_info *info = charger->battery_info;

	batt->name = "battery";
	batt->use_for_apm = info->use_for_apm;
	batt->type = POWER_SUPPLY_TYPE_BATTERY;
	batt->get_property = axp_battery_get_property;

	batt->properties = axp_battery_props;
	batt->num_properties = ARRAY_SIZE(axp_battery_props);

	ac->name = "ac";
	ac->type = POWER_SUPPLY_TYPE_MAINS;
	ac->get_property = axp_ac_get_property;

	ac->supplied_to = supply_list;
	ac->num_supplicants = ARRAY_SIZE(supply_list);

	ac->properties = axp_ac_props;
	ac->num_properties = ARRAY_SIZE(axp_ac_props);

	usb->name = "usb";
	usb->type = POWER_SUPPLY_TYPE_USB;
	usb->get_property = axp_usb_get_property;

	usb->supplied_to = supply_list;
	usb->num_supplicants = ARRAY_SIZE(supply_list);

	usb->properties = axp_usb_props;
	usb->num_properties = ARRAY_SIZE(axp_usb_props);
};

#if defined CONFIG_HAS_EARLYSUSPEND
static void axp_earlysuspend(struct early_suspend *h)
{
	DBG_PSY_MSG("======early suspend=======\n");
//#if defined (CONFIG_AXP_CHGCURCHG) || (CONFIG_AXP_DEBUG)
	if ((ac_state == 1) || (axpcharger->flag == 1)) {
		printk("[AXP]set a more larger current\n");
		axp_set_chargecurrent(axpcharger->chgearcur);
	}
//#endif
}

static void axp_lateresume(struct early_suspend *h)
{
	DBG_PSY_MSG("======late resume=======\n");
//#if defined (CONFIG_AXP_CHGCURCHG) || (CONFIG_AXP_DEBUG)
	if ((ac_state == 1) || (axpcharger->flag == 1)) {
		printk("[AXP]set a more smaller current\n");
		axp_set_chargecurrent(axpcharger->chgcur);
	}
//#endif
}
#endif

static void axp_check_ac_work(struct work_struct *work)
{
	struct axp_charger *charger =
		container_of(work, struct axp_charger, check_ac_work.work);

	if(charger->ac_valid) {
		if(ac_state == 0) {
			DBG_PSY_MSG("+++ AC IN +++\n");
			ac_state = 1;
			axp_set_chargecurrent(charger->chgcur);
			DBG_PSY_MSG("AC IN: set charging current 1200mA\n");
			axp_charging_update(charger);
			axp_change(charger);
		}
	} else {
		if(ac_state == 1) {
			DBG_PSY_MSG("--- AC OUT ---\n");
			ac_state = 0;
			if((charger->usb_valid == 0) || 
					(charger->usb_valid == 1 && charger->flag == 2)) {
				axp_set_chargecurrent(500);
				DBG_PSY_MSG("AC OUT: no usb charging, or usb charger only, set current 500mA\n");
			}
			axp_charging_update(charger);
			axp_change(charger);
		}
	}
	schedule_delayed_work(&charger->check_ac_work, msecs_to_jiffies(1000));
}

/* cur suport 0, 100, 500 and 900 */
static void axp_vbus_current_limit(struct axp_charger *charger, int cur)
{
	uint8_t reg_val;

	axp_read(charger->master, AXP_IPS_SET, &reg_val);
	reg_val &= ~0x03;

	switch (cur) {
		case 0:
			reg_val |= 0x3;
			break;
		case 100:
			reg_val |= 0x2;
			break;
		case 500:
			reg_val |= 0x1;
			break;
		case 900:
			reg_val |= 0x0;
			break;
		default:
			reg_val |= 0x1;
			break;
	}

	axp_write(charger->master, AXP_IPS_SET, reg_val);

	return ;
}

//int charger->flag;/*0 - not initialize, 1 - ac_charger, 2 - usb_charger, 3 - usb_host*/
static void axp_check_usb_work(struct work_struct *work)
{
	int state;
	struct axp_charger *charger =
		container_of(work, struct axp_charger, check_usb_work.work);

	state = get_otg_charge_mode();
	if (((state == 2) || (state == 1))
			&& (charger->usb_valid != 1))
		state = 0;

	switch(state) {
		case 0:		/* idle */
			if(charger->flag != 0) {
				if (charger->flag == 1) {
					DBG_PSY_MSG("--- USB-AC CHARGER OUT ---\n");
					if(charger->ac_valid == 1) {
						axp_vbus_current_limit(charger, 500);
						axp_set_chargecurrent(charger->chgcur);
						DBG_PSY_MSG("USB-AC CHARGER OUT: but AC is valid, vbus limit 500mA\n");
					} else {
						axp_vbus_current_limit(charger, 500);
						axp_set_chargecurrent(500);		/*set charging current 500mA*/
						DBG_PSY_MSG("USB-AC CHARGER OUT: and AC is not valid, vbus limit 500mA, set current 500mA\n");
					}
				} else if (charger->flag == 2) {
					DBG_PSY_MSG("--- USB CHARGER OUT ---\n");
					if(charger->ac_valid == 1) {
						axp_vbus_current_limit(charger, 500);
						axp_set_chargecurrent(charger->chgcur);
						DBG_PSY_MSG("USB CHARGER OUT: but AC is valid, vbus limit 500mA\n");
					} else {
						axp_vbus_current_limit(charger, 500);
						axp_set_chargecurrent(500);		/*set charging current 500mA*/
						DBG_PSY_MSG("USB CHARGER OUT: and AC is not valid, vbus limit 500mA, set current 500mA\n");
					}
				} else if (charger->flag == 3) {
					DBG_PSY_MSG("--- USB HOST OUT ---\n");
					//drvbus_pull(0);
					axp_drvbus_ctrl(0);
					msleep(10);
					//vbus_en_pull(0);
					axp_vbusen_ctrl(0);
				}
				charger->flag = 0;
				axp_charging_update(charger);
				axp_change(charger);
			}
			break;
		case 1:		/* ac charger */
			if(charger->flag != 1) {
				DBG_PSY_MSG("+++ USB-AC CHAGER IN +++");
				charger->flag = 1;
				axp_vbus_current_limit(charger, 0);		//no limit
				axp_set_chargecurrent(charger->chgcur);
				DBG_PSY_MSG("USB-AC CHAGER IN: vbus no limit\n");
				axp_charging_update(charger);
				axp_change(charger);
			}
			break;
		case 2:		/* usb charger */
			if(charger->flag != 2) {
				DBG_PSY_MSG("+++ USB CHARGER IN +++\n");
				charger->flag = 2;
				axp_vbus_current_limit(charger, 500);
				if(charger->ac_valid == 1) {
					axp_set_chargecurrent(charger->chgcur);
					DBG_PSY_MSG("USB CHARGER IN: and AC is valid too, vbus limit 500mA\n");
				} else {
					axp_set_chargecurrent(500);		/* set charging current 500mA */
					DBG_PSY_MSG("USB CHARGER IN: and AC is not valid, vbus limit 500mA, set current 500mA\n");
				}
				axp_charging_update(charger);
				axp_change(charger);
			}
			break;
		case 3:		/* usb host */
			if(charger->flag != 3) {
				DBG_PSY_MSG("+++ USB HOST IN +++\n");
				charger->flag = 3;
				//vbus_en_pull(1);
				axp_vbusen_ctrl(1);
				msleep(10);
				//drvbus_pull(1);
				axp_drvbus_ctrl(1);
				axp_charging_update(charger);
				axp_change(charger);
			}
			break;
		default:
			break;
	}
	schedule_delayed_work(&charger->check_usb_work, msecs_to_jiffies(1000));
}

static void axp_chargingstate_monitor(struct work_struct *work)
{
    struct axp_charger *charger;
    static int pre_ac, pre_usb;
    charger = container_of(work, struct axp_charger, workstate.work);

    axp_charger_update_state(charger);
    if((pre_ac != charger->ac_valid) || (pre_usb != charger->usb_valid)) {
	DBG_PSY_MSG("battery state change: acvalid:%d->%d,usbvalid:%d->%d\n", 
		pre_ac, charger->ac_valid, pre_usb, charger->usb_valid);
	pre_ac = charger->ac_valid;
	pre_usb = charger->usb_valid;
	state_count = 1;
	power_supply_changed(&charger->batt);
	power_supply_changed(&charger->ac);
	power_supply_changed(&charger->usb);
    }

    schedule_delayed_work(&charger->workstate, charger->intervalstate);
}

static int axp_battery_probe(struct platform_device *pdev)
{
	struct axp_charger *charger;
	struct axp_supply_init_data *pdata = pdev->dev.platform_data;
	int ret;

	DBG_PSY_MSG("axp_battery_probe_m85d\n");

	pwrkey_used = axp_pwrkey_used();
	irq_used = axp_irq_used();
	axp_irq_init(&irq_low, &irq_high);

	if (pdata == NULL)
	    return -EINVAL;

	if(irq_used && pwrkey_used) {
	    /* set PEK-key infomation */
	    ret = axp_set_pekinfo(pdev);
	    if(ret){
		DBG_PSY_MSG("[AXP]Unable to set pek input device\n");
		return ret;
	    }
	}

	charger = kzalloc(sizeof(*charger), GFP_KERNEL);
	if (charger == NULL)
		return -ENOMEM;

	charger->probe_over = 0;
	charger->master = pdev->dev.parent;

	charger->chgcur			= pdata->chgcur;
	charger->chgearcur		= pdata->chgearcur;
	charger->chgsuscur		= pdata->chgsuscur;
	charger->chgclscur		= pdata->chgclscur;
	charger->chgvol			= pdata->chgvol;
	charger->chgend			= pdata->chgend;
	charger->adc_freq		= pdata->adc_freq;
	charger->chgpretime		= pdata->chgpretime;
	charger->chgcsttime		= pdata->chgcsttime;
	charger->battery_info	= pdata->battery_info;
	if(BATT_VOL_OPT){
		charger->update_flag    = 0;
		charger->delta          = 0;
	}
	charger->batt_low_flag          = 0;
	charger->batt_cap_low_design    = 15;

	charger->battery_info->energy_full_design =
		item_exist("batt.capacity")?item_integer("batt.capacity", 0):2300;

	axp_batt_minit(charger);

	/* for debugging */
	axpcharger = charger;

	ret = axp_battery_first_init(charger);
	if (ret)
		goto err_charger_init;

	if(irq_used) {
	    charger->nb.notifier_call = axp_battery_event;
	    ret = axp_register_notifier(charger->master, &charger->nb, irq_low, irq_high);
	    if (ret)
		goto err_notifier;
	}

	axp_battery_setup_psy(charger);
	ret = power_supply_register(&pdev->dev, &charger->batt);
	if (ret)
		goto err_ps_register;

	ret = power_supply_register(&pdev->dev, &charger->ac);
	if (ret){
		power_supply_unregister(&charger->batt);
		goto err_ps_register;
	}

	ret = power_supply_register(&pdev->dev, &charger->usb);
	if (ret){
		power_supply_unregister(&charger->ac);
		power_supply_unregister(&charger->batt);
		goto err_ps_register;
	}

#if defined (CONFIG_AXP_CHGCURCHG) || defined (CONFIG_AXP_DEBUG)
	ret = axp_charger_create_attrs(&charger->batt);
	if(ret){
		return ret;
	}
#endif

	platform_set_drvdata(pdev, charger);

	/* initialization of charger information */
	axp_charger_update_state(charger);
	axp_charger_update(charger);
	if(charger->bat_det == 1)
		axp_battery_infoinit(charger);

	/* create workqueue for updating battery status */
	charger->interval = msecs_to_jiffies(TIMER1 * 1000);
	INIT_DELAYED_WORK(&charger->work, axp_charging_monitor);
	if(charger->bat_det == 1){
		schedule_delayed_work(&charger->work, charger->interval);
	} else {
		charger->rest_cap = 100;
	}

	charger->flag = 0;
	ac_state = 0;
	INIT_DELAYED_WORK(&charger->check_ac_work, axp_check_ac_work);
	INIT_DELAYED_WORK(&charger->check_usb_work, axp_check_usb_work);
	
	if(irq_used)
	{
	    schedule_delayed_work(&charger->check_ac_work, msecs_to_jiffies(500));
	    schedule_delayed_work(&charger->check_usb_work, msecs_to_jiffies(1500));
	}
	else
	{
	    schedule_delayed_work(&charger->check_ac_work, msecs_to_jiffies(100));
	    schedule_delayed_work(&charger->check_usb_work, msecs_to_jiffies(2500));

	    charger->intervalstate = msecs_to_jiffies(TIMER6 * 100);
	    INIT_DELAYED_WORK(&charger->workstate, axp_chargingstate_monitor);
	    schedule_delayed_work(&charger->workstate, charger->intervalstate);
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	axp_early_suspend.suspend = axp_earlysuspend;
	axp_early_suspend.resume = axp_lateresume;
	axp_early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 2;
	register_early_suspend(&axp_early_suspend);
#endif
	/* register debugging interface */
	class_register(&axppower_class);

	wake_lock_init(&charger->axp_wake_lock, WAKE_LOCK_SUSPEND, "axp_splywake");
	charger->probe_over = 1;

	return ret;

err_ps_register:
	DBG_PSY_MSG("err_ps_register\n");
	if(irq_used) {
	    axp_unregister_notifier(charger->master, &charger->nb, AXP_NOTIFIER_ON_LOW, AXP_NOTIFIER_ON_HIGH);
	}

err_notifier:
	DBG_PSY_MSG("err_notifier\n");
	cancel_delayed_work_sync(&charger->work);

err_charger_init:
	DBG_PSY_MSG("err_charger_init\n");
	kfree(charger);
	if(irq_used && pwrkey_used) {
	    input_unregister_device(powerkeydev);
	    kfree(powerkeydev);
	}

	return ret;
}

static int axp_battery_remove(struct platform_device *dev)
{
	struct axp_charger *charger = platform_get_drvdata(dev);

	if(irq_used)
	{
	    wake_lock_destroy(&charger->axp_wake_lock);

	    axp_unregister_notifier(charger->master, &charger->nb, AXP_NOTIFIER_ON_LOW, AXP_NOTIFIER_ON_HIGH);
	}
	else
	{
	    cancel_delayed_work_sync(&charger->workstate);
	}

	cancel_delayed_work_sync(&charger->work);

	power_supply_unregister(&charger->usb);
	power_supply_unregister(&charger->ac);
	power_supply_unregister(&charger->batt);

	kfree(charger);
	if(irq_used && pwrkey_used)
	{
	    input_unregister_device(powerkeydev);
	    kfree(powerkeydev);
	}

	return 0;
}

static void axp_shutdown(struct platform_device *dev)
{
	struct axp_charger *charger = platform_get_drvdata(dev);

	cancel_delayed_work_sync(&charger->work);

	if(!irq_used)
	{
	    cancel_delayed_work_sync(&charger->workstate);
	}

	axp_set_rdcclose();

	/* set next booting with charger charging current  */
	axp_set_chargecurrent(charger->chgclscur);
}

static struct platform_driver axp_battery_driver = {
	.driver 	= {
		.name 	= "axp-supplyer",
		.owner	= THIS_MODULE,
	},
	.probe 		= axp_battery_probe,
	.remove 	= axp_battery_remove,
	.suspend	= axp_suspend,
	.resume 	= axp_resume,
	.shutdown	= axp_shutdown,
};

static int axp_battery_init(void)
{
	if(item_equal("pmu.mode", "mode5", 0)) {
		printk("[AXP]Using mode5!\n");
		return platform_driver_register(&axp_battery_driver);
	}else {
		return -1;
	}
}

static void axp_battery_exit(void)
{
	platform_driver_unregister(&axp_battery_driver);
}

subsys_initcall(axp_battery_init);
module_exit(axp_battery_exit);

MODULE_DESCRIPTION("Battery Driver for X-Powers AXP PMIC");
MODULE_AUTHOR("Donglu Zhang, <zhangdonglu@x-powers.com>");
MODULE_LICENSE("GPL");
