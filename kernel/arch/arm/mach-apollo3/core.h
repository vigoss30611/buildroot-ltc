
#ifndef __IMAP_CORE_H__
#define __IMAP_CORE_H__

#include <mach/sram.h>
#include <linux/smp.h>
/* Below is for apollo3 */

extern struct smp_operations apollo3_smp_ops;

void apollo3_secondary_startup(void);

/* this is implemented in core.c */
void __init apollo3_init_devices(void);

/* this is implemented in clock-ops.c */
void __init apollo3_init_clocks(void);

/* this is implemented in gpio.c */
void __init apollo_init_gpio(void);

/* this is implemented in mem_reserve.c */
void __init apollo3_mem_reserve(void);

/* this is implemented in common.c */
void apollo3_power_off(void);

/* this is implemented in common.c */
void apollo3_enter_lowpower(void);

/* this is implemented in common.c */
void __init apollo3_init_l2x0(void);

/* this is implemented in common.c */
void __init apollo3_twd_init(void);

/* this is implemented in pm.c */
void __init apollo3_init_pm(void);

void apollo3_ip6313_reset(char mode, const char *cmd);
void apollo3_ip6313_poweroff(void);
void apollo3_ip6313_enter_lowpower(void);
void apollo3_ip6303_reset(char mode, const char *cmd);
void apollo3_ip6303_poweroff(void);
void apollo3_ip6303_enter_lowpower(void);

#define core_msg(x...) pr_err(KERN_ERR "imapx: " x)

#endif /* __IMAP_CORE_H__ */

