#ifndef _MODE5_H_
#define _MODE5_H_

#include <linux/kernel.h>         
//#include <linux/module.h>         
//#include <linux/interrupt.h>      
#include <linux/platform_device.h>
//#include <linux/i2c.h>            
//#include <linux/clk.h>            
#include <linux/delay.h>          
//#include <linux/slab.h>           
#include <linux/wakelock.h>
#include <linux/power_supply.h>
#include <mach/pad.h>             
#include <mach/items.h>           
//#include <mach/irqs.h>            
#include <mach/imap-iomap.h>      
//#include <asm/io.h>       

#include "axp-cfg.h"
#include "axp-mfd.h"
#include "axp-regu.h"
#include "axp-irq.h"
#include "axp-struct.h"

extern int axp_get_batt_voltage(struct device *);

static void mode5_drvbus_ctrl(int val)
{
    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	//imapx_pad_set_mode(1, 1, 61);
	//imapx_pad_set_dir(0, 1, 61);
	//imapx_pad_set_outdat(!!val, 1, 61);
	gpio_direction_output(61, !!val);
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	//imapx_pad_set_mode(1, 1, 61);
	//imapx_pad_set_dir(0, 1, 61);
	//imapx_pad_set_outdat(!!val, 1, 61);
	gpio_direction_output(61, !!val);
    }
}

static void mode5_vbusen_ctrl(int val)
{
    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	//imapx_pad_set_mode(1, 1, 54);
	//imapx_pad_set_dir(0, 1, 54);
	//imapx_pad_set_outdat(!!val, 1, 54);
	gpio_direction_output(54, !!val);
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	//imapx_pad_set_mode(1, 1, 61);
	//imapx_pad_set_dir(0, 1, 61);
	//imapx_pad_set_outdat(!!val, 1, 61);
	gpio_direction_output(61, !!val);
    }
}

static int mode5_irq_used(void)
{
    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	return 0;
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	return 1;
    }
    else
    {
	return 0;
    }
}

static void mode5_irq_init(uint32_t *low, uint32_t *high)
{
    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	*low = 0;
	*high = 0;
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	*low = (AXP_IRQ_USBIN | AXP_IRQ_USBRE | AXP_IRQ_ACIN | AXP_IRQ_USBOV | AXP_IRQ_ACRE |
		AXP_IRQ_ACOV | AXP_IRQ_CHAOV | AXP_IRQ_CHAST | AXP_IRQ_CHACURLO | AXP_IRQ_ICTEMOV);
	*high = (AXP_IRQ_PEKFE | AXP_IRQ_PEKRE | AXP_IRQ_GPIO2TG | AXP_IRQ_GPIO3TG);
    }
    else
    {
	*low = 0;
	*high = 0;
    }
}

static int mode5_pwrkey_used(void)
{
    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	return 0;
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	return 1;
    }
    else
    {
	return 0;
    }
}

static void mode5_mfd_init(struct device *dev)
{
    int vol;
    uint8_t tmp;
	int ret;

	if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0)) {
		if (gpio_is_valid(61)) {
			ret = gpio_request(61, "mode5-drvbus");
			if (ret) {
				pr_err("failed request gpio for mode5-drvbus\n");
				goto err;
			}
		}

		if (gpio_is_valid(54)) {
			ret = gpio_request(61, "mode5-vbusen");
			if (ret) {
				pr_err("failed request gpio for mode5-vbusen\n");
				goto err2;
			}
		}
	} else if(item_equal("board.arch", "a5pv20", 0)) {
		if (gpio_is_valid(61)) {
			ret = gpio_request(61, "mode5-drvbus");
			if (ret) {
				pr_err("failed request gpio for mode5-drvbus\n");
				goto err;
			}
		}
	}


    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	vol = axp_get_batt_voltage(dev);
	if(vol < 1000)
	{
	    printk("battery_voltage < 1000, input no limit\n");
	    udelay(1000);
	    axp_write(dev, 0x30, 0x83);
	}
	else
	{
	    mode5_vbusen_ctrl(0);
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
	axp_update(dev, 0x30, 0x1, 0x3);//vbus current limit 500mA

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
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	vol = axp_get_batt_voltage(dev);
	if(vol < 1000)
	{
	    printk("battery_voltage < 1000, input no limit\n");
	    udelay(1000);
	    axp_write(dev, 0x30, 0x83);
	}
	else
	{
	    mode5_vbusen_ctrl(0);
	}

	/* set DCDC2 ot 1.5v for DDR3 */
	axp_write(dev, AXP_BUCK2, 0x20);

	/*close UVP*/
	axp_read(dev, 0x81, &tmp);
	tmp &= ~(0x1c);
	axp_write(dev, 0x81, tmp);

	/*N_OE set to 1s*/
	axp_set_bits(dev, 0x32, 0x1);
	axp_clr_bits(dev, 0x32, 0x2);

	/*set Voff as 2.6v */
	axp_read(dev, 0x31, &tmp);
	tmp &= ~0x7;
	axp_write(dev, 0x31, tmp);

	/*set charging target voltage 4.2V*/
	axp_read(dev, 0x33, &tmp);
	tmp &= ~0x60;
	tmp |= 0x40;
	axp_write(dev, 0x33, tmp);

	/* enable batt monitor*/
	axp_set_bits(dev, 0x32, (1<<6));

	/* power off timing control */
	axp_set_bits(dev, AXP_OFF_CTL, (0x1 << 2));

	/*set charging current*/
	axp_update(dev, 0x30, 0x1, 0x3);//vbus current limit 500mA

	/*set timeout*/
	axp_write(dev, 0x34, 0xC3);

	/*ADC setting*/
	axp_write(dev, 0x82, 0xfe);
	axp_write(dev, 0x83, 0x80);
	axp_write(dev, 0x84, 0xf4);

	/*OT shutdown*/
	axp_set_bits(dev, 0x8f, (1<<2));

	/* set GPIO0 as LDO5  */
	axp_set_bits(dev, 0x90, 0x3);
	axp_clr_bits(dev, 0x90, 0x4);
	/* ldo5 output 3.3v */
	axp_set_bits(dev, 0x91, 0xf0);

	/* set GPIO3 to input mode */
	axp_read(dev, POWER20_GPIO3_CTL, &tmp);
	tmp |= 0x1 << 2;
	axp_write(dev, POWER20_GPIO3_CTL, tmp);

	/* set gpio2 to input mode, as rtc alarm interrupt, falling edge trig */
	axp_write(dev, POWER20_GPIO2_CTL, 0x42);

	axp_rtcgp_irq_clr(1);
	axp_rtcgp_irq_enable(1);
    }

err2:
	if (gpio_is_valid(61))
		gpio_free(61);
err:
	return;
}

static void mode5_mfd_suspend(struct device *dev)
{
    uint8_t tmp;

    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	return;
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	/* set GPIO3 for waking up system */
	axp_write(dev, POWER20_GPIO3_CTL, 0xc6);

	/* turn off ldio, set to gpio input */
	axp_read(dev, POWER20_GPIO0_CTL, &tmp);
	tmp &= ~0x7;
	tmp |= 0x2;
	axp_write(dev,POWER20_GPIO0_CTL, tmp);

	axp_rtcgp_irq_disable(1);
    }
}

static void mode5_mfd_resume(struct device *dev)
{
    uint8_t tmp;

    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	return;
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	/* set DCDC2 ot 1.5v for DDR3 */
	axp_write(dev, AXP_BUCK2, 0x20);

	/* turn on ldio */
	axp_read(dev, POWER20_GPIO0_CTL, &tmp);
	tmp &= ~0x7;
	tmp |= 0x3;
	axp_write(dev, POWER20_GPIO0_CTL, tmp);


	axp_write(dev, POWER20_GPIO3_CTL, 0x06);

	axp_rtcgp_irq_config(1);
	axp_rtcgp_irq_enable(1);
    }
}

static void mode5_batt_init(struct axp_charger *charger)
{
	return;
}

static void mode5_batt_suspend(struct axp_charger *charger)
{
    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	/* cancel workqueue */
	cancel_delayed_work(&charger->work);
	cancel_delayed_work(&charger->workstate);
	cancel_delayed_work(&charger->check_ac_work);
	cancel_delayed_work(&charger->check_usb_work);
	mode5_drvbus_ctrl(0);
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	/* cancel workqueue */
	cancel_delayed_work(&charger->work);
	cancel_delayed_work(&charger->check_ac_work);
	cancel_delayed_work(&charger->check_usb_work);
    }
}

static void mode5_batt_resume(struct axp_charger *charger)
{
    int vol;
    uint8_t val;

    if(!item_exist("board.arch") || item_equal("board.arch", "a5pv10", 0))
    {
	extern int ac_state;

	vol = axp_get_batt_voltage(charger->master);
	if(vol < 1000)
	{
	    printk("battery_voltage < 1000, input no limit\n");
	    udelay(1000);
	    axp_write(charger->master, 0x30, 0x83);
	}
	else
	{
	    mode5_vbusen_ctrl(0);
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
	axp_read(charger->master, 0x81, &val);
	val &= ~(0x1c);
	axp_write(charger->master, 0x81, val);

	axp_write(charger->master, 0x29, 0x54);
	axp_set_bits(charger->master, 0x12, (1<<6));/*ldo3 2.8V*/
	axp_update(charger->master, 0x28, 0x6, 0xf);
	axp_set_bits(charger->master, 0x12, (1<<3));/*ldo2 1.8V*/
	axp_write(charger->master, 0x91, 0xf5);
	axp_write(charger->master, 0x90, 0x3);/*gpio0 3.3V*/

	/* N_OE set to 128ms */
	axp_clr_bits(charger->master, 0x32, 0x3);

	/* set Voff 2.9V */
	axp_read(charger->master, 0x31, &val);
	val &= ~0x7;
	val |= 0x3;
	axp_write(charger->master, 0x31, val);

	/* set charging target voltage 4.2V */
	axp_read(charger->master, 0x33, &val);
	val &= ~0x60;
	val |= 0x40;
	axp_write(charger->master, 0x33, val);

	/* disable batt monitor */
	axp_clr_bits(charger->master, 0x32, (1<<6));

	/* set charging current */
	axp_update(charger->master, 0x30, 0x1, 0x3);//vbus current limit 500mA default

	/* set timeout */
	axp_write(charger->master, 0x34, 0xC3);

	/* ADC setting */
	axp_write(charger->master, 0x82, 0xfe);
	axp_write(charger->master, 0x83, 0x80);
	axp_write(charger->master, 0x84, 0xf4);

	/* OT shutdown */
	axp_set_bits(charger->master, 0x8f, (1<<2));

	/* reschedule workqueue */
	ac_state = 0;
	schedule_delayed_work(&charger->workstate, 0);
	schedule_delayed_work(&charger->check_ac_work, msecs_to_jiffies(500));
	schedule_delayed_work(&charger->check_usb_work, msecs_to_jiffies(500));
    }
    else if(item_equal("board.arch", "a5pv20", 0))
    {
	vol = axp_get_batt_voltage(charger->master);
	if(vol < 1000)
	{
	    printk("battery_voltage < 1000, input no limit\n");
	    udelay(1000);
	    axp_write(charger->master, 0x30, 0x83);
	}
	else
	{
	    mode5_vbusen_ctrl(0);
	}

	/* if usb host mode, reset flag */
	if(charger->flag == 3)
	    charger->flag = 0;

	axp_write(charger->master, 0x29, 0x54);
	axp_set_bits(charger->master, 0x12, (1<<6));/*ldo3 2.8V*/
	axp_update(charger->master, 0x28, 0x6, 0xf);
	axp_set_bits(charger->master, 0x12, (1<<3));/*ldo2 1.8V*/
	axp_write(charger->master, 0x91, 0xf5);
	axp_write(charger->master, 0x90, 0x3);/*gpio0 3.3V*/

	/* N_OE set to 1s */
	axp_set_bits(charger->master, 0x32, 0x1);
	axp_clr_bits(charger->master, 0x32, 0x2);

	/* set Voff 2.6V */
	axp_read(charger->master, 0x31, &val);
	val &= ~0x7;
	axp_write(charger->master, 0x31, val);

	/* set charging target voltage 4.2V */
	axp_read(charger->master, 0x33, &val);
	val &= ~0x60;
	val |= 0x40;
	axp_write(charger->master, 0x33, val);

	/* enable batt monitor */
	axp_set_bits(charger->master, 0x32, (1<<6));

	/* set charging current */
	axp_update(charger->master, 0x30, 0x1, 0x3);//vbus current limit 500mA

	/* set timeout */
	axp_write(charger->master, 0x34, 0xC3);

	/* ADC setting */
	axp_write(charger->master, 0x82, 0xfe);
	axp_write(charger->master, 0x83, 0x80);
	axp_write(charger->master, 0x84, 0xf4);

	/* OT shutdown */
	axp_set_bits(charger->master, 0x8f, (1<<2));

	/* reschedule delay work */
	schedule_delayed_work(&charger->check_ac_work, msecs_to_jiffies(500));
	schedule_delayed_work(&charger->check_usb_work, msecs_to_jiffies(500));
    }
}

#endif
