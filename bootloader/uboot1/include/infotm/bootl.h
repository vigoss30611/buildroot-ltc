#ifndef __BOOTL_H
#define __BOOTL_H

typedef enum {
	BOOT_NORMAL	= 0,
	BOOT_RECOVERY_IUS =	1,
	BOOT_RECOVERY_DEV =	2,
	BOOT_RECOVERY_OTA =	3,
	BOOT_FACTORY_INIT = 4,
	BOOT_CHARGER = 5,
	BOOT_RECOVERY_MMC = 6,
	BOOT_HALT_SYSTEM = 0xFF
} boot_mode_t;

extern int set_boot_type(int type);
extern int got_boot_type(void);
extern int bootl(void);
extern uint8_t *load_image(char *name , char* alternate, char* rambase, int src_devid);
extern int do_boot(void* , void*);
extern int boot_verify_type(void);
#endif
