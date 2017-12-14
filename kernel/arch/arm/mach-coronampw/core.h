
#ifndef __IMAP_CORE_H__
#define __IMAP_CORE_H__

#include <mach/sram.h>
#include <linux/smp.h>
/* Below is for coronampw */

extern struct smp_operations coronampw_smp_ops;

void coronampw_secondary_startup(void);

/* this is implemented in core.c */
void __init coronampw_init_devices(void);

/* this is implemented in clock-ops.c */
void __init coronampw_init_clocks(void);

/* this is implemented in gpio.c */
void __init apollo_init_gpio(void);

/* this is implemented in mem_reserve.c */
void __init coronampw_mem_reserve(void);

/* this is implemented in common.c */
void coronampw_power_off(void);

/* this is implemented in common.c */
void coronampw_enter_lowpower(void);

/* this is implemented in common.c */
void __init coronampw_init_l2x0(void);

/* this is implemented in common.c */
void __init coronampw_twd_init(void);

/* this is implemented in pm.c */
void __init coronampw_init_pm(void);

void coronampw_ip6313_reset(char mode, const char *cmd);
void coronampw_ip6313_poweroff(void);
void coronampw_ip6313_enter_lowpower(void);

#define core_msg(x...) pr_err(KERN_ERR "imapx: " x)

#endif /* __IMAP_CORE_H__ */

