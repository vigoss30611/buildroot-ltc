#ifndef _MODE4_H_
#define _MODE4_H_

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
extern int read_rtc_gpx(int);

static void mode4_drvbus_ctrl(int val)
{
    return;
}

static void mode4_vbusen_ctrl(int val)
{
    //imapx_pad_set_mode(1, 1, 61);
    //imapx_pad_set_dir(0, 1, 61);
    //imapx_pad_set_outdat(!!val, 1, 61);
    gpio_direction_output(61, !!val);
}

static int mode4_irq_used(void)
{
    return 0;
}

static void mode4_irq_init(uint32_t *low, uint32_t *high)
{
    *low = 0;
    *high = 0;
}

static int mode4_pwrkey_used(void)
{
    return 0;
}

static void mode4_mfd_init(struct device *dev)
{
    int vol;
    uint8_t tmp;
	int ret;

	if (gpio_is_valid(61)) {
		ret = gpio_request(61, "mode4-vbusen");
		if (ret) {
			pr_err("failed request gpio for mode4-vbusen\n");
			goto err;
		}
	}

    vol = axp_get_batt_voltage(dev);
    if(vol < 1000)
    {
	printk("battery_voltage < 1000, input no limit\n");
	udelay(1000);
	axp_write(dev, 0x30, 0x83);
    }
    else
    {
	mode4_vbusen_ctrl(1);
    }

    if(item_exist("board.cpu") && item_equal("board.cpu", "x15", 0))
    {
	axp_write(dev, 0x23, 0x16);/*1.25V*/
    }
    else
    {
	axp_write(dev, 0x23, 0x12);/*1.15V*/
    }

    /* close UVP */
    axp_read(dev, 0x81, &tmp);
    tmp &= ~(0x1c);
    axp_write(dev, 0x81, tmp);

    /* N_OE set to 128ms */
    axp_clr_bits(dev, 0x32, 0x3);

    /* set Voff 3.2V */
    axp_read(dev, 0x31, &tmp);
    tmp &= ~0x7;
    tmp |= 0x6;
    axp_write(dev, 0x31, tmp);

    /* set charging target voltage 4.2V */
    axp_read(dev, 0x33, &tmp);
    tmp &= ~0x60;
    tmp |= 0x40;
    axp_write(dev, 0x33, tmp);

    /* disable batt monitor */
    axp_clr_bits(dev, 0x32, (1<<6));

    /* set charging current */
    axp_write(dev,0x33,0xc2); // set charger battery 500mA default

    /* set timeout */
    axp_write(dev, 0x34, 0xC3);

    /* ADC setting */
    axp_write(dev, 0x82, 0xfe);
    axp_write(dev, 0x83, 0x80);
    axp_write(dev, 0x84, 0xf4);

    /* OT shutdown */
    axp_set_bits(dev, 0x8f, (1<<2));

    /* gpio0 3.3V */
    axp_write(dev, 0x91, 0xf5);
    axp_write(dev, 0x90, 0x3);

err:
	return;
}

static void mode4_mfd_suspend(struct device *dev)
{
    return;
}

static void mode4_mfd_resume(struct device *dev)
{
    return;
}

static void mode4_batt_init(struct axp_charger *charger)
{
    return;
}

static void mode4_batt_suspend(struct axp_charger *charger)
{
    /* cancel workqueue */
    cancel_delayed_work(&charger->work);
    if(!mode4_irq_used()) {
	cancel_delayed_work(&charger->workstate);
    }
    cancel_delayed_work(&charger->check_vbus_high);
    cancel_delayed_work(&charger->check_vbus_low);
    mode4_vbusen_ctrl(1);
}

static void mode4_batt_resume(struct axp_charger *charger)
{
    int vol;
    uint8_t tmp;
    int sta;

    vol = axp_get_batt_voltage(charger->master);
    if(vol < 1000)
    {
	printk("battery_voltage < 1000, input no limit\n");
	udelay(1000);
	axp_write(charger->master, 0x30, 0x83);
    }
    else
    {
	mode4_vbusen_ctrl(1);
    }

    charger->flag = 0;

    /* core voltage setting */
    if((item_exist("board.cpu") && item_equal("board.cpu", "x15", 0)))
    {
	axp_write(charger->master, 0x23, 0x16);/*set core vol to 1.25*/
    }
    else
    {
	axp_write(charger->master, 0x23, 0x12);/*set core vol to 1.15V*/
    }

    /* close UVP */
    axp_read(charger->master, 0x81, &tmp);
    tmp &= ~(0x1c);
    axp_write(charger->master, 0x81, tmp);

    axp_write(charger->master, 0x29, 0x54);
    axp_set_bits(charger->master, 0x12, (1<<6));/*ldo3 2.8V*/
    axp_update(charger->master, 0x28, 0x6, 0xf);
    axp_set_bits(charger->master, 0x12, (1<<3));/*ldo2 1.8V*/
    axp_write(charger->master, 0x91, 0xf5);
    axp_write(charger->master, 0x90, 0x3);/*gpio0 3.3V*/

    /* N_OE set to 128ms */
    axp_clr_bits(charger->master, 0x32, 0x3);

    /* set Voff 3.2V */
    axp_read(charger->master, 0x31, &tmp);
    tmp &= ~0x7;
    tmp |= 0x6;
    axp_write(charger->master, 0x31, tmp);

    /* set charging target voltage 4.2V */
    axp_read(charger->master, 0x33, &tmp);
    tmp &= ~0x60;
    tmp |= 0x40;
    axp_write(charger->master, 0x33, tmp);

    /* disable batt monitor */
    axp_clr_bits(charger->master, 0x32, (1<<6));

    /* set charging current */
    axp_write(charger->master, 0x33, 0xC2);/*set charging current 500mA*/

    /* set timeout */
    axp_write(charger->master, 0x34, 0xC3);

    /* ADC setting */
    axp_write(charger->master, 0x82, 0xfe);
    axp_write(charger->master, 0x83, 0x80);
    axp_write(charger->master, 0x84, 0xf4);

    /* OT shutdown */
    axp_set_bits(charger->master, 0x8f, (1<<2));

    /* reschedule workqueue */
    sta = read_rtc_gpx(2);
    if(sta != charger->pre_sts)
    {
	charger->ac_valid = !!sta;
	power_supply_changed(&charger->ac);
	schedule_delayed_work(&charger->check_vbus_high, msecs_to_jiffies(50));
	if(!mode4_irq_used()) {
	    schedule_delayed_work(&charger->workstate, charger->intervalstate);
	}
    }
    else
    {
	schedule_delayed_work(&charger->check_vbus_high, msecs_to_jiffies(50));
	if(!mode4_irq_used()) {
	    schedule_delayed_work(&charger->workstate, charger->intervalstate);
	}
    }
}

#endif
