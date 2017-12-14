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
#include <linux/gpio.h>

#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <linux/seq_file.h>
#include <linux/input.h>
#include <asm/div64.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>
#include <mach/pad.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define DBG_AXP_PSY 1
#if  DBG_AXP_PSY
#define DBG_PSY_MSG(format,args...)   printk("[AXP]"format,##args)
#else
#define DBG_PSY_MSG(format,args...)   do {} while (0)
#endif

#include "axp-sply.h"
static void axp_charger_update_state(struct axp_charger *charger);
static void axp_charger_update(struct axp_charger *charger);
static int axp_ibat_to_mA(uint16_t reg);
static void axp_close(struct axp_charger *charger);
static int irq_used, pwrkey_used;
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
extern int axp_charger_in(void);
extern int32_t get_otg_charge_mode(void);
//extern void vbus_en_pull(int);
//extern void drvbus_pull(int);
//extern int batt_export_axp202_cap;

extern int pcf8563_get_alarm_int_state(void);
extern void pcf8563_clear_alarm_int(void);
extern int pcf8563_rtc_alarmirq(void);

static int count;
static int stat;

/*
 * TODO:add for compiling, will be removed after OTG driver complete.
 */
int get_otg_charge_mode(void)
{
	return 0;
}

static int check_adapter_in(int val)
{
    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	return read_rtc_gpx(val);
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	return axp_charger_in();
    }
    else
    {
	return 0;
    }
}

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
			axp_battery_check_status(charger, val);break;
		case POWER_SUPPLY_PROP_HEALTH:
			axp_battery_check_health(charger, val);break;
		case POWER_SUPPLY_PROP_TECHNOLOGY:
			val->intval = charger->battery_info->technology;break;
		case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
			val->intval = charger->battery_info->voltage_max_design;break;
		case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
			val->intval = charger->battery_info->voltage_min_design;break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = charger->ocv * 1000;break;
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = charger->ibat * 1000;break;
		case POWER_SUPPLY_PROP_MODEL_NAME:
			val->strval = charger->batt.name;break;
		case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
			val->intval = charger->battery_info->energy_full_design;break;
		case POWER_SUPPLY_PROP_CAPACITY:
            //printk("===%s: POWER_SUPPLY_PROP_CAPACITY===\n", __func__);
	    		//batt_export_axp202_cap = charger->rest_cap;
			val->intval = charger->rest_cap;
            //printk("---%s: charger->rest_cap==%d\n", __func__, charger->rest_cap);
            break;
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = (!charger->is_on)&&(charger->bat_det) && (! charger->ext_valid);break;
		case POWER_SUPPLY_PROP_PRESENT:
			val->intval = charger->bat_det;break;
		case POWER_SUPPLY_PROP_TEMP:
			val->intval =  300;break;
		default:
			ret = -EINVAL;break;
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

	switch(psp){
		case POWER_SUPPLY_PROP_MODEL_NAME:
			val->strval = charger->ac.name;break;
		case POWER_SUPPLY_PROP_PRESENT:
			//val->intval = charger->ac_det;break;
		case POWER_SUPPLY_PROP_ONLINE:
			//val->intval = charger->ac_valid;
            if(count == 1) {
                val->intval = 1;
            }else {
                if(charger->flag == 1)
                    val->intval = 1;
                else
                    val->intval = 0;
            }
            break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = charger->vac * 1000;break;
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = charger->iac * 1000;break;
		default:
			ret = -EINVAL;break;
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
			val->strval = charger->usb.name;break;
		case POWER_SUPPLY_PROP_PRESENT:
			//val->intval = charger->usb_det;break;
		case POWER_SUPPLY_PROP_ONLINE:
			//val->intval = charger->usb_valid;
            if(count == 1) {
                val->intval = 1;
            }else {
                if(charger->flag == 2)
                    val->intval = 1;
                else
                    val->intval = 0;
            }
            break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = charger->vusb * 1000;break;
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = charger->iusb * 1000;break;
		default:
			ret = -EINVAL;break;
	}
	return ret;
}

static void axp_change(struct axp_charger *charger)
{
	printk("battery state change\n");
	axp_charger_update_state(charger);
	axp_charger_update(charger);
	state_count = 1;
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

static void axp_capchange(struct axp_charger *charger)
{
	printk("battery change\n");
	ssleep(2);
	axp_charger_update_state(charger);
	axp_charger_update(charger);
	if(charger->bat_det == 1){
		axp_battery_infoinit(charger);
		schedule_delayed_work(&charger->work, charger->interval);
	} else {
		charger->rest_cap = 100;
		charger->ocv = 0;
		cancel_delayed_work_sync(&charger->work);
	}
	power_supply_changed(&charger->batt);
}

static void axp_close(struct axp_charger *charger)
{
		printk("axp_close\n");
	charger->rest_cap = SHUTDOWNRATE;
#if defined (CONFIG_KP_AXP20)
	axp_write(charger->master,AXP_DATA_BUFFER1,(0x80|SHUTDOWNRATE));
#endif
	printk("\n==================event in close==============\n");
	power_supply_changed(&charger->batt);
}

static void axp_close_input(struct axp_charger *charger)
{
    printk("------axp close input------\n");
    axp_clr_bits(charger->master, 0x30, (1<<7));
    //drvbus_pull(0);
    axp_drvbus_ctrl(0);
    msleep(10);
    //vbus_en_pull(0);
    axp_vbusen_ctrl(0);
}

//#if defined (CONFIG_KP_USEIRQ)
static int axp_battery_event(struct notifier_block *nb, unsigned long event,
        void *data)
{
	struct axp_charger *charger =
					container_of(nb, struct axp_charger, nb);
	uint32_t irqs_low = ((uint32_t *)data)[0];
	uint32_t irqs_high = ((uint32_t *)data)[1];


	if(irqs_low & (AXP_IRQ_BATIN|AXP_IRQ_BATRE)) {
		axp_capchange(charger);
	}

	if(irqs_low & AXP_IRQ_CHAOV) {
			DBG_PSY_MSG("charging over\n");
		axp_change(charger);
	}

	if(irqs_low & AXP_IRQ_CHAST) {
			DBG_PSY_MSG("charging on\n");
		axp_change(charger);
	}

	if(irqs_low & AXP_IRQ_CHACURLO) {
		DBG_PSY_MSG("charging current low\n");

	//	axp_read(charger->master, 0x30, &val);
	//	DBG_PSY_MSG("VBUS-IPSOUT reg=0x%x\n", val);
	//	axp_read(charger->master, 0x33, &val);
	//	DBG_PSY_MSG("CHARGING CTRL = 0x%x\n", val);
		axp_change(charger);
	}

	if(irqs_low & (AXP_IRQ_ACOV|AXP_IRQ_USBOV|AXP_IRQ_ICTEMOV)) {
		    if(irqs_low & (AXP_IRQ_ACOV|AXP_IRQ_USBOV))
                DBG_PSY_MSG("charger voltage too high!\n");
            else if(irqs_low & AXP_IRQ_ICTEMOV)
                DBG_PSY_MSG("IC internal over heat!\n");
        axp_close_input(charger);
        schedule_delayed_work(&charger->check_vbus_low,
                                msecs_to_jiffies(500));
		//axp_change(charger);
	}

	//if(irqs_low & (AXP_IRQ_ACRE|AXP_IRQ_USBRE)) {
	//		DBG_PSY_MSG("charger remove\n");
	//	axp_clr_bits(charger->master, 0x30, 0x83);
	//	axp_set_bits(charger->master, 0x30, 0x2);/*VBUS current limit to 100mA*/
	//	axp_clr_bits(charger->master, 0x33, 0xf);/*charging current min*/
	//	vbus_en_pull(1);/*vbus en high*/
	//	schedule_delayed_work(&charger->check_ac,
	//		                        msecs_to_jiffies(500));
	//	axp_change(charger);
	//}

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
//#endif

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

	ac->supplied_to = supply_list,
	ac->num_supplicants = ARRAY_SIZE(supply_list),

	ac->properties = axp_ac_props;
	ac->num_properties = ARRAY_SIZE(axp_ac_props);

	usb->name = "usb";
	usb->type = POWER_SUPPLY_TYPE_USB;
	usb->get_property = axp_usb_get_property;

	usb->supplied_to = supply_list,
	usb->num_supplicants = ARRAY_SIZE(supply_list),

	usb->properties = axp_usb_props;
	usb->num_properties = ARRAY_SIZE(axp_usb_props);
};

#if defined CONFIG_HAS_EARLYSUSPEND
static void axp_earlysuspend(struct early_suspend *h)
{
	printk("======early suspend=======\n");
#if defined (CONFIG_AXP_CHGCURCHG)
	axp_set_chargecurrent(axpcharger->chgearcur);
#endif
}
static void axp_lateresume(struct early_suspend *h)
{
	printk("======late resume=======\n");
#if defined (CONFIG_AXP_CHGCURCHG)
	axp_set_chargecurrent(axpcharger->chgcur);
#endif
}
#endif

/* get the status and kind of charger */
static void axp_check_usb_status(struct work_struct *work)
{
	int status;
	uint8_t val;
	struct axp_charger *charger =
		container_of(work, struct axp_charger, check_work.work);

    status = get_otg_charge_mode();
	switch(status) {
	case 0:/*idle*/
			printk("######## IDLE ########\n");
            count = 0;
        schedule_delayed_work(&charger->check_vbus_low,
                                    msecs_to_jiffies(500));
		break;

	case 1:/*ac in*/
			printk("######## AC IN ########\n");
            count = 0;
        if(charger->flag != 1) {
            charger->flag = 1;
		    axp_read(charger->master, 0x30, &val);
		    val |= 0x3;
		    axp_write(charger->master, 0x30, val);/*vbus current no limit*/
		    axp_set_bits(charger->master, 0x33, 0xf);/*charging current 1800mA*/
            schedule_delayed_work(&charger->check_vbus_low,
                                        msecs_to_jiffies(500));
		    axp_change(charger);
        }

		break;

	case 2:
			printk("######## USB IN ########\n");
            count = 0;
		if(charger->flag != 2) {
			charger->flag = 2;

			axp_read(charger->master, 0x30, &val);
			val &= ~0x3;
			val |= 0x1;
			axp_write(charger->master, 0x30, val);/*vbus current limit to 500mA*/

			axp_read(charger->master, 0x33, &val);
			val &= ~0xf;
			val |= 0x2;
			axp_write(charger->master, 0x33, val);/*charging current 500mA*/

            schedule_delayed_work(&charger->check_vbus_low,
                                        msecs_to_jiffies(500));
			axp_change(charger);
		}

		break;

	case 3:/*in fact, this branch will not be reached*/
			printk("######## USB HOST ########\n");
            count = 0;
		if(charger->flag != 3) {
			charger->flag = 3;
            //vbus_en_pull(1);
	    axp_vbusen_ctrl(1);
            msleep(10);
            //drvbus_pull(1);
	    axp_drvbus_ctrl(1);
            //schedule_delayed_work(&charger->check_vbus_low,
            //                            msecs_to_jiffies(500));
            schedule_delayed_work(&charger->check_id_high,
                                        msecs_to_jiffies(500));
		}

		break;
	}

	return;
}

static void axp_check_vbus_low(struct work_struct *work)
{
    struct axp_charger *charger =
            container_of(work, struct axp_charger, check_vbus_low.work);
    //int ret =axp_charger_in();
    int ret = check_adapter_in(2);

    if(ret) {
        schedule_delayed_work(&charger->check_vbus_low,
                                msecs_to_jiffies(500));
    }else {
        printk("------VBUS -->> Low------\n");
        charger->flag = 0;
        axp_close_input(charger);
        cancel_delayed_work(&charger->check_vbus_low);
        schedule_delayed_work(&charger->check_vbus_high,
                                msecs_to_jiffies(500));
        axp_change(charger);
    }
}

/* check if charger in */
static void axp_check_vbus_high(struct work_struct *work)
{
    struct axp_charger *charger =
            container_of(work, struct axp_charger, check_vbus_high.work);
    //int ret = axp_charger_in();
    int ret = check_adapter_in(2);

    if(ret) {
        printk("------VBUS -->> High------\n");
        cancel_delayed_work(&charger->check_vbus_high);
        schedule_delayed_work(&charger->check_work,
                                msecs_to_jiffies(1000));
    }else {
        schedule_delayed_work(&charger->check_vbus_high,
                                msecs_to_jiffies(500));
    }
}

static int get_id_pin_state(void)
{
    int ret, id_index;

    id_index = item_integer("config.otg.extraid", 1);
    //ret = imapx_pad_get_indat(id_index);
    if ((-ITEMS_EINT) == id_index) {
        ret = 0;
    } else {
    ret = gpio_get_value(id_index);
    }

    return ret;
}

static void axp_check_id_low(struct work_struct *work)
{
    struct axp_charger *charger =
            container_of(work, struct axp_charger, check_id_low.work);
    int ret = get_id_pin_state();

    if(ret) {
        schedule_delayed_work(&charger->check_id_low,
                                msecs_to_jiffies(500));
    }else {
        printk("-----V3-ID -->> LOW------\n");
        cancel_delayed_work_sync(&charger->check_vbus_high);
        cancel_delayed_work(&charger->check_id_low);
        //charger->flag = 3;
        //vbus_en_pull(1);
        //msleep(10);
        //drvbus_pull(1);
        //schedule_delayed_work(&charger->check_id_high,
        //                        msecs_to_jiffies(500));
        schedule_delayed_work(&charger->check_work,
                                    msecs_to_jiffies(1000));
    }
}

static void axp_check_id_high(struct work_struct *work)
{
    struct axp_charger *charger =
            container_of(work, struct axp_charger, check_id_high.work);
    int ret = get_id_pin_state();

    if(ret) {
        printk("------ID -->> HIGH------\n");
        charger->flag = 0;
        //drvbus_pull(0);
	axp_drvbus_ctrl(0);
        msleep(10);
        //vbus_en_pull(0);
	axp_vbusen_ctrl(0);
        cancel_delayed_work(&charger->check_id_high);
        schedule_delayed_work(&charger->check_id_low,
                                msecs_to_jiffies(100));
        schedule_delayed_work(&charger->check_vbus_high,
                                msecs_to_jiffies(100));
    }else {
        schedule_delayed_work(&charger->check_id_high,
                                msecs_to_jiffies(500));
    }
}

static int axp_battery_probe(struct platform_device *pdev)
{
	struct axp_charger *charger;
	struct axp_supply_init_data *pdata = pdev->dev.platform_data;
	int ret;


	if (pdata == NULL)
		return -EINVAL;

	irq_used = axp_irq_used();
	pwrkey_used = axp_pwrkey_used();
	axp_irq_init(&irq_low, &irq_high);
	count = 0;
	stat = 0;

	if(irq_used && pwrkey_used)
	{
	    /* set PEK-key infomation */
	    ret = axp_set_pekinfo(pdev);
	    if(ret){
		printk("[AXP]Unable to set pek input device\n");
		return ret;
	    }
	}

	charger = kzalloc(sizeof(*charger), GFP_KERNEL);
	if (charger == NULL)
		return -ENOMEM;

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
		charger->update_flag	= 0;
		charger->delta		= 0;
	}
	charger->batt_low_flag          = 0;
	charger->batt_cap_low_design    = 15;

    charger->battery_info->energy_full_design =
        item_exist("batt.capacity")?item_integer("batt.capacity", 0):2300;

	/* used for debugging */
	axpcharger = charger;

	axp_batt_minit(charger);

	/* charging parameters initialization */
	ret = axp_battery_first_init(charger);
	if (ret)
		goto err_charger_init;

	if(irq_used)
	{
	    charger->nb.notifier_call = axp_battery_event;
	    ret = axp_register_notifier(charger->master, &charger->nb, irq_low, irq_high);
	    if (ret)
		goto err_notifier;
	}

	/* battery information initialization */
	axp_charger_update_state(charger);
	axp_charger_update(charger);
	if(charger->bat_det == 1)
		axp_battery_infoinit(charger);

    //stat = axp_charger_in();
    stat = check_adapter_in(2);
    if(stat) {
        count = 1;
        charger->ac_det = 1;
        charger->ac_valid = 1;
        charger->usb_det = 1;
        charger->usb_valid = 1;
    }else {
        charger->ac_det = 0;
        charger->ac_valid = 0;
        charger->usb_det = 0;
        charger->usb_valid = 0;
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

	/* create work-queue for updating battery information */
	charger->interval = msecs_to_jiffies(TIMER1 * 1000);
	INIT_DELAYED_WORK(&charger->work, axp_charging_monitor);
	if(charger->bat_det == 1){
		schedule_delayed_work(&charger->work, charger->interval);
	} else {
		charger->rest_cap = 100;
	}

	charger->flag = 0;
    INIT_DELAYED_WORK(&charger->check_work, axp_check_usb_status);
	INIT_DELAYED_WORK(&charger->check_vbus_high, axp_check_vbus_high);
    INIT_DELAYED_WORK(&charger->check_vbus_low, axp_check_vbus_low);
    INIT_DELAYED_WORK(&charger->check_id_high, axp_check_id_high);
    INIT_DELAYED_WORK(&charger->check_id_low, axp_check_id_low);
	schedule_delayed_work(&charger->check_vbus_high,
		                        msecs_to_jiffies(2500));
    schedule_delayed_work(&charger->check_id_low, msecs_to_jiffies(2500));

//#ifndef CONFIG_KP_USEIRQ
//	/* polling charging status if not use irq */
//	charger->intervalstate = msecs_to_jiffies(TIMER6 * 100);
//	INIT_DELAYED_WORK(&charger->workstate, axp_chargingstate_monitor);
//	schedule_delayed_work(&charger->workstate, charger->intervalstate);
//#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
	axp_early_suspend.suspend = axp_earlysuspend;
	axp_early_suspend.resume = axp_lateresume;
	axp_early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 2;
	register_early_suspend(&axp_early_suspend);
#endif
	/* debug interface register */
	class_register(&axppower_class);

	return ret;

err_ps_register:
	printk("err_ps_register\n");
	if(irq_used) {
	    axp_unregister_notifier(charger->master, &charger->nb, AXP_NOTIFIER_ON_LOW, AXP_NOTIFIER_ON_HIGH);
	}

//#if defined (CONFIG_KP_USEIRQ)
err_notifier:
//#endif
	printk("err_notifier\n");
	cancel_delayed_work_sync(&charger->work);

err_charger_init:
	printk("err_charger_init\n");
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

	if(irq_used) {
	    axp_unregister_notifier(charger->master, &charger->nb, irq_low, irq_high);
	}
	cancel_delayed_work_sync(&charger->work);
//#ifndef CONFIG_KP_USEIRQ
//	cancel_delayed_work_sync(&charger->workstate);
//#endif
	power_supply_unregister(&charger->usb);
	power_supply_unregister(&charger->ac);
	power_supply_unregister(&charger->batt);

	kfree(charger);

	if(irq_used && pwrkey_used) {
	    input_unregister_device(powerkeydev);
	    kfree(powerkeydev);
	}

	return 0;
}

static void axp_shutdown(struct platform_device *dev)
{
	struct axp_charger *charger = platform_get_drvdata(dev);

	cancel_delayed_work_sync(&charger->work);
//#ifndef CONFIG_KP_USEIRQ
//	cancel_delayed_work_sync(&charger->workstate);
//#endif
	axp_set_rdcclose();

	/* set power off charging current */
#if defined (CONFIG_AXP_CHGCURCHG)
	axp_set_chargecurrent(charger->chgclscur);
#endif

	axp_drvbus_ctrl(0);
	msleep(10);
	axp_vbusen_ctrl(0);
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
    if(item_exist("pmu.mode") == 0 ||
            item_equal("pmu.mode", "mode2", 0)) {
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
