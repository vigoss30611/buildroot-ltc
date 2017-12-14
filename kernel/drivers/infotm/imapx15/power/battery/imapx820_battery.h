#ifndef __IMAPX820_BATTERY_H__
#define __IMAPX820_BATTERY_H__

#define BATT_CURVE_NUM_MAX 15

enum imap_batt_version_e{
	BATT_VER_0 = 0,
	BATT_VER_1,
};

enum imap_batt_otg_e{
	BATT_OTG_IDLE = 0,
	BATT_OTG_AC,
	BATT_OTG_USB,
	BATT_OTG_HOST,
};

enum imap_batt_charger_state_e{
	BATT_CHARGER_OFF = 0,
	BATT_CHARGER_ON,
	BATT_CHARGER_UNKNOW,
	BATT_CHARGER_AC,
	BATT_CHARGER_USB,
};

struct imap_battery_param{
	int batt_vol_max_design;
	int batt_vol_min_design;
	int batt_cap_low_design;

	int batt_ntc_design;
	int batt_temp_max_design;
	int batt_temp_min_design;
	
	int batt_technology;
	
	enum imap_batt_version_e batt_version;
};

enum batt_api_version_e {
	BATT_API_S_0_0 = 0,
	BATT_API_S_0_1,
};

struct imap_current_level{
	char name[20];
	int level;
	int currents;
};

struct battery_curve{
	int cap;
	int vol;
};

struct batt_curve_tbl{
	int level;
	struct battery_curve batt_curve[BATT_CURVE_NUM_MAX];
};


#if 0
struct battery_curve discharge_curve[] = {
	[0] = {100, 4100},
	[1] = {90, 4000},
	[2] = {80, 3900},
	[3] = {70, 3800},
	[4] = {60, 3770},
	[5] = {50, 3750},
	[6] = {40, 3730},
	[7] = {30, 3700},
	[8] = {20, 3680},
	[9] = {10, 3650},
	[10] = {5, 3630},
	[11] = {0, 3600},
};

struct battery_curve charge_curve[] = {
	[0] = {100, 4150},
	[1] = {90, 4100},
	[2] = {80, 4050},
	[3] = {70, 4010},
	[4] = {60, 3990},
	[5] = {50, 3960},
	[6] = {40, 3930},
	[7] = {30, 3900},
	[8] = {20, 3850},
	[9] = {10, 3800},
	[10] = {5, 3750},
	[11] = {0, 3700},
};
#endif
/* unit: sec*/
//int battery_timer [] = {
//	30, 20, 10, 8, 6, 4, 3, 2, 1
//};

struct imap_battery_info{
	struct imap_battery_param *batt_param;

	struct power_supply *imap_psy_ac;
	struct power_supply *imap_psy_batt;
	 struct power_supply *imap_psy_usb;

	int mon_battery_interval;
	struct semaphore mon_sem;
	struct task_struct *monitor_battery;
	
	int cap_timer_cnt_max;
	int cap_timer_cnt_min;
	int cap_timer_cnt_design;
	int cap_cnt;
	
	int batt_update_flag;
	int batt_init_flag;
	int batt_low_flag;
	int batt_exchange_flag;

	int batt_state;
	int batt_health;
	int batt_valid;
	int batt_voltage;
	int batt_old_voltage;
	int batt_current;
	int batt_capacity;
	int batt_old_capacity;
	int batt_temperature;
    int batt_icc;
	
	struct imap_current_level *current_level;
	int current_level_num;
	int current_state;

	int (*display_capacity)(int);

	enum imap_batt_charger_state_e charger_state;

	int sys_mode;// 1-charger mode, 0-other mode
	int batt_api_version;
};

extern struct battery_curve discharge_curve[12];
extern struct battery_curve charge_curve[12];
extern int isacon(void);
extern int (*batt_get_adc_val)(void);

#endif /* __IMAPX820_BATTERY_H__ */
