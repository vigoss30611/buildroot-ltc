#ifndef _AXP_STRUCT_
#define _AXP_STRUCT_

#include <linux/power_supply.h>
#include <linux/wakelock.h>

struct axp_adc_res {//struct change
	uint16_t vbat_res;
	uint16_t ibat_res;
	uint16_t ichar_res;
	uint16_t idischar_res;
	uint16_t vac_res;
	uint16_t iac_res;
	uint16_t vusb_res;
	uint16_t iusb_res;
};
struct axp_charger {
	/*power supply sysfs*/
	struct power_supply batt;
	struct power_supply	ac;
	struct power_supply	usb;
	struct power_supply bubatt;

	/*i2c device*/
	struct device *master;

	/* adc */
	struct axp_adc_res adc;
	unsigned int adc_freq;

	/*monitor*/
	struct delayed_work work;
	struct delayed_work check_work;
	struct delayed_work check_vbus_high;
	struct delayed_work check_vbus_low;
	struct delayed_work check_id_low;
	struct delayed_work check_id_high;
	struct delayed_work check_ac_work;
	struct delayed_work check_usb_work;
	unsigned int interval;
//#ifndef CONFIG_KP_USEIRQ
	struct delayed_work workstate;
	unsigned int intervalstate;
//#endif

	/*battery info*/
	struct power_supply_info *battery_info;

	/*charger control*/
	bool chgen;
	bool limit_on;
	unsigned int chgcur;
	unsigned int chgearcur;
	unsigned int chgsuscur;
	unsigned int chgclscur;
	unsigned int chgvol;
	unsigned int chgend;

	/*charger time */
	int chgpretime;
	int chgcsttime;

	/*external charger*/
	bool chgexten;
	int chgextcur;

	/* charger status */
	bool bat_det;
	bool is_on;
	bool is_finish;
	bool ac_not_enough;
	bool ac_det;
	bool usb_det;
	bool ac_valid;
	bool usb_valid;
	bool ext_valid;
	bool bat_current_direction;
	bool in_short;
	bool batery_active;
	bool low_charge_current;
	bool int_over_temp;
	bool charge_on;

	int vbat;
	int ibat;
	int pbat;
	int vac;
	int iac;
	int vusb;
	int iusb;
	int ocv;

	int disvbat;
	int disibat;

	/*rest time*/
	int rest_cap;
	int ocv_rest_cap;
	int base_restcap;
	int rest_time;

	/*ic temperature*/
	int ic_temp;

	/*irq*/
	struct notifier_block nb;

	int	probe_over;
	struct wake_lock	axp_wake_lock;

	int flag;
	int pre_sts;
	int update_flag;
	int delta;
	int batt_cap_low_design;
	int batt_low_flag;
};
#endif
