#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <mach/gpio.h>
#include "battery.h"

#include <mach/items.h>

//extern int read_rtc_gpx(int io_num);
int batt_export_1180_a_cap=0xff;
int batt_export_1180_a_vol=0xff;
int batt_export_1180_cap=0xff;
int batt_export_1180_vol=0xff;
int batt_export_axp202_cap=0xff;
int batt_export_axp202_vol=0xff;
EXPORT_SYMBOL(batt_export_1180_a_cap);
EXPORT_SYMBOL(batt_export_1180_a_vol);
EXPORT_SYMBOL(batt_export_1180_cap);
EXPORT_SYMBOL(batt_export_1180_vol);
EXPORT_SYMBOL(batt_export_axp202_cap);
EXPORT_SYMBOL(batt_export_axp202_vol);

/* return 
	1: charger is on
	0: charger is off*/
int isacon(void)
{
	unsigned long tmp = 0;
	int index, abs_index, pin_val;

	
    	//tmp = read_rtc_gpx(2);
   if(item_exist("power.dcdetect")){
		index = item_integer("power.dcdetect", 1);
		abs_index = (index>0)? index : (-1*index);
		if (gpio_is_valid(abs_index)) {
#if CONFIG_INFT_HAS_NO_BATTERY
                        pin_val = 1;
#else
			pin_val = (index>0)? gpio_get_value(abs_index) : (!gpio_get_value(abs_index));
#endif
		}
		else {
			pin_val = 0;
		}

		return pin_val;
	}
    
	return !!tmp; 
}
EXPORT_SYMBOL(isacon); //for TP: zet6221  suixing 2012-11-26

//int (*batt_get_adc_val)(void);
void __imapx_register_batt(int (*func)(void))
{
//	batt_get_adc_val = func;
}
EXPORT_SYMBOL(__imapx_register_batt);

int batt_export_cap(void)
{
	if(item_exist("pmu.model") && 
		item_equal("pmu.model", "axp202", 0))
	{
	    return batt_export_axp202_cap;
	}
	else
	{
	    return batt_export_1180_cap;
	}
	
}
EXPORT_SYMBOL(batt_export_cap);


/* ver 0 */
struct battery_curve discharge_curve[] = {
	[0] = {100, 4100},
	[1] = {90, 4000},
	[2] = {80, 3950},
	[3] = {70, 3900},
	[4] = {60, 3850},
	[5] = {50, 3800},
	[6] = {40, 3700},
	[7] = {30, 3600},
	[8] = {20, 3500},
	[9] = {15, 3450},
	[10] = {5, 3300},
	[11] = {0, 3200},
};
EXPORT_SYMBOL(discharge_curve);

struct battery_curve charge_curve[] = {
	[0] = {100, 4100},
	[1] = {90, 4060},
	[2] = {80, 4010},
	[3] = {70, 4000},
	[4] = {60, 3950},
	[5] = {50, 3900},
	[6] = {40, 3850},
	[7] = {30, 3800},
	[8] = {20, 3700},
	[9] = {10, 3600},
	[10] = {5, 3500},
	[11] = {0, 3400},	
};
EXPORT_SYMBOL(charge_curve);


