#ifndef __QSDK_SYS_H__
#define __QSDK_SYS_H__

#define SETTING_EINT 		88887777
#define UPGRADE_PARA_SIZE	1024
#define RESERVED_ENV_SIZE	0xfc00
#define RESERVED_ENV_UBOOT_SIZE 0x400

#define UPGRADE_SAVE_FLAG   "upgrade_flag=ota"

enum {
	UPGRADE_MODE_NULL = 1,
	UPGRADE_MODE_OTA = 2
};

enum boot_reason {
	BOOT_REASON_ACIN,
	BOOT_REASON_EXTINT,
	BOOT_REASON_POWERKEY,
	BOOT_REASON_UNKNOWN,
};

enum externel_device_type {
	EXT_DEVICE_GSENSOR,
	EXT_DEVICE_WIFI,
	EXT_DEVICE_4G,
	EXT_DEVICE_UNKNOWN,
};

typedef struct {
	unsigned int total;
	unsigned int free;
} storage_info_t;

typedef struct {
	unsigned int  chipid;
	unsigned int  macid_h;
	unsigned int  macid_l;
} soc_info_t;

typedef struct {
	char *name;
	int inserted;
	int mounted;
} disk_info_t;

typedef struct {
	int current_now;
	int online;
	int present;
	int voltage_now;
} ac_info_t;

typedef struct {
	int capacity;
	int online;
	int voltage_now;
	char status[16];
} battery_info_t;

#ifdef __cplusplus
extern "C" {

#endif /*__cplusplus */
extern int system_get_soc_type(soc_info_t *info);
extern int system_get_soc_mac(soc_info_t *info);
extern void system_get_storage_info(char *mount_point, storage_info_t *si);
extern int system_get_disk_status(disk_info_t *info);
extern int system_format_storage(char *mount_point);
extern int system_enable_watchdog(void);
extern int system_disable_watchdog(void);
extern int system_feed_watchdog(void);
extern int system_set_watchdog(int interval);
extern char *system_get_ledlist(void);
extern int system_get_led(char *led_name);
extern int system_set_led(const char *led, int time_on, int time_off, int blink_on);
extern int system_get_boot_reason();
extern int system_get_backlight();
extern int system_set_backlight(int);
extern int system_enable_backlight(int enable);
extern int system_request_pin(int index);
extern int system_get_pin_value(int index);
extern int system_set_pin_value(int index, int value);
extern void system_get_ac_info(ac_info_t *aci);
extern void system_get_battry_info(battery_info_t *bati);
extern int system_get_version(char *version);
extern int system_check_upgrade(char *name, char *value);
extern int system_set_upgrade(char *name, char *value);
extern int system_clearall_upgrade(void);
extern int system_check_process(char *pid_name);
extern void system_kill_process(char *pid_name);
extern int system_get_env(char *name, char *value, int count);
extern int system_set_env(char *name, char *value);
extern int system_enable_externel_powerup(enum externel_device_type dev_type, char *dev_name, int enable);

extern const char * setting_get_string(const char *owner, const char *key, char *buf, int len);
extern int setting_set_string(const char *owner, const char *key, const char *value);
extern double setting_get_double(const char *owner, const char *key);
extern int setting_set_double(const char *owner, const char *key, double value);
extern int setting_get_int(const char *owner, const char *key);
extern int setting_set_int(const char *owner, const char *key, int value);
extern int setting_remove_key(const char *owner, const char *key);
extern int setting_remove_all(const char *owner);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __QSDK_SYS_H__ */
