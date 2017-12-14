
#ifndef _RTC_BIT_H_
#define _RTC_BIT_H_

enum board_bootst {
    BOOT_ST_NORMAL = 0,
    BOOT_ST_RESET,
    BOOT_ST_RECOVERY,
    BOOT_ST_RECOVERY_OTA,
    BOOT_ST_RECOVERY_DEV,
    BOOT_ST_WATCHDOG,
    BOOT_ST_RESUME,
    BOOT_ST_CHARGER,
    BOOT_ST_FASTBOOT,
    BOOT_ST_BURN = 0x37,
    BOOT_ST_RESETCORE = 0x3b,
};

extern int rtcbit_init(void);
extern int rtcbit_set(const char *name, uint32_t pattern);
extern int rtcbit_clear(const char *name);
extern uint32_t rtcbit_get(const char *name);
extern void rtcbit_print(void);
extern void rtcbit_set_rbmem(void);

#endif

