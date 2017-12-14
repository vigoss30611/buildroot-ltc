#ifndef _MODE3_H_
#define _MODE3_H_

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/power_supply.h>
#include <mach/pad.h>
#include <mach/items.h>
#include <mach/imap-iomap.h>

#include "axp-cfg.h"
#include "axp-mfd.h"
#include "axp-regu.h"
//#include "axp-irq.h"
#include "axp-struct.h"

extern int axp_get_batt_voltage(struct device *);

static void mode3_drvbus_ctrl(int val)
{
    //imapx_pad_set_mode(1, 1, 61);
    //imapx_pad_set_dir(0, 1, 61);
    //imapx_pad_set_outdat(!!val, 1, 61);
    gpio_direction_output(61, !!val);
}

static void mode3_vbusen_ctrl(int val)
{
    //imapx_pad_set_mode(1, 1, 54);
    //imapx_pad_set_dir(0, 1, 54);
    //imapx_pad_set_outdat(!!val, 1, 54);
    gpio_direction_output(54, !!val);
}

static int mode3_irq_used(void)
{
    return 0;
}

static void mode3_irq_init(uint32_t *low, uint32_t *high)
{
    *low = 0;
    *high = 0;
}

static int mode3_pwrkey_used(void)
{
    return 0;
}

static void mode3_mfd_init(struct device *dev)
{
    int vol;
    uint8_t tmp;

    vol = axp_get_batt_voltage(dev);
    if(vol < 1000)
    {
	printk("battery_voltage < 1000, input no limit\n");
	udelay(1000);
	axp_write(dev, 0x30, 0x83);
    }
    else
    {
	;
    }

    if(item_exist("board.cpu") && item_equal("board.cpu", "x15", 0))
    {
	axp_write(dev, 0x23, 0x16);/*1.25V*/
    }
    else
    {
	axp_write(dev, 0x23, 0x12);/*1.15V*/
    }

    /*close UVP*/
    axp_read(dev, 0x81, &tmp);
    tmp &= ~(0x1c);
    axp_write(dev, 0x81, tmp);

    /*N_OE set to 128ms*/
    axp_clr_bits(dev, 0x32, 0x3);

    /*set Voff 2.9V*/
    axp_read(dev, 0x31, &tmp);
    tmp &= ~0x7;
    tmp |= 0x3;
    axp_write(dev, 0x31, tmp);

    /*set charging target voltage 4.2V*/
    axp_read(dev, 0x33, &tmp);
    tmp &= ~0x60;
    tmp |= 0x40;
    axp_write(dev, 0x33, tmp);

    /*disable batt monitor*/
    axp_clr_bits(dev, 0x32, (1<<6));

    /*set charging current*/
    axp_set_bits(dev, 0x30, 0x3);//vbus current no limit
    axp_write(dev, 0x33, 0xC9);//1200mA

    /*set timeout*/
    axp_write(dev, 0x34, 0xC3);

    /*ADC setting*/
    axp_write(dev, 0x82, 0xfe);
    axp_write(dev, 0x83, 0x80);
    axp_write(dev, 0x84, 0xf4);

    /*OT shutdown*/
    axp_set_bits(dev, 0x8f, (1<<2));
    
    axp_write(dev, 0x91, 0xf5);
    axp_write(dev, 0x90, 0x3);/*gpio0 3.3V*/
}

static void mode3_mfd_suspend(struct device *dev)
{
    return;
}

static void mode3_mfd_resume(struct device *dev)
{
    return;
}

static void mode3_batt_init(struct axp_charger *charger)
{
	int ret;

	if (gpio_is_valid(61)) {
		ret = gpio_request(61, "mode3-drvbus");
		if (ret) {
			pr_err("failed request gpio for mode3-drvbus\n");
			goto err;
		}
	}

	if (gpio_is_valid(54)) {
		ret = gpio_request(54, "mode3-vbusen");
		if (ret) {
			pr_err("failed request gpio for mode3-vbusen\n");
			goto err2;
		}
	}
	return;

err2:
	if (gpio_is_valid(61))
		gpio_free(61);
err:
    return;
}

static void mode3_batt_suspend(struct axp_charger *charger)
{
    cancel_delayed_work(&charger->work);
    if(!mode3_irq_used()) {
	cancel_delayed_work(&charger->workstate);
    }
    cancel_delayed_work(&charger->check_id_low);
    cancel_delayed_work(&charger->check_id_high);
    mode3_drvbus_ctrl(0);
}

static void mode3_batt_resume(struct axp_charger *charger)
{
    int vol;
    uint8_t val;

    vol = axp_get_batt_voltage(charger->master);
    if(vol < 1000) {
	printk("battery_voltage < 1000, input no limit\n");
	udelay(1000);
	axp_write(charger->master, 0x30, 0x83);
    }
    else
    {
	;
    }

    charger->flag = 0;

    /*core voltage setting*/
    if((item_exist("board.cpu") && item_equal("board.cpu", "x15", 0)))
    {
	axp_write(charger->master, 0x23, 0x16);/*set core vol to 1.25*/
    }
    else
    {
	axp_write(charger->master, 0x23, 0x12);/*set core vol to 1.15V*/
    }

    /*close UVP*/
    axp_read(charger->master, 0x81, &val);
    val &= ~(0x1c);
    axp_write(charger->master, 0x81, val);

    /*output voltage setting*/
    axp_write(charger->master, 0x29, 0x54);
    axp_set_bits(charger->master, 0x12, (1<<6));/*ldo3 2.8V*/
    axp_update(charger->master, 0x28, 0x6, 0xf);
    axp_set_bits(charger->master, 0x12, (1<<3));/*ldo2 1.8V*/
    axp_write(charger->master, 0x91, 0xf5);
    axp_write(charger->master, 0x90, 0x3);/*gpio0 3.3V*/

    /*N_OE set to 128ms*/
    axp_clr_bits(charger->master, 0x32, 0x3);

    /*set Voff 2.9V*/
    axp_read(charger->master, 0x31, &val);
    val &= ~0x7;
    val |= 0x3;
    axp_write(charger->master, 0x31, val);

    /*set charging target voltage 4.2V*/
    axp_read(charger->master, 0x33, &val);
    val &= ~0x60;
    val |= 0x40;
    axp_write(charger->master, 0x33, val);

    /*disable batt monitor*/
    axp_clr_bits(charger->master, 0x32, (1<<6));

    /*set charging current*/
    axp_set_bits(charger->master, 0x30, 0x3);//vbus current no limit
    axp_write(charger->master, 0x33, 0xC9);/*1200mA*/

    /*set timeout*/
    axp_write(charger->master, 0x34, 0xC3);

    /*ADC setting*/
    axp_write(charger->master, 0x82, 0xfe);
    axp_write(charger->master, 0x83, 0x80);
    axp_write(charger->master, 0x84, 0xf4);

    /*OT shutdown*/
    axp_set_bits(charger->master, 0x8f, (1<<2));

    /*reschedule delay work*/
    if(!mode3_irq_used())
    {
	schedule_delayed_work(&charger->workstate, 0);
    }
    schedule_delayed_work(&charger->check_id_low, msecs_to_jiffies(100));
}

#endif
