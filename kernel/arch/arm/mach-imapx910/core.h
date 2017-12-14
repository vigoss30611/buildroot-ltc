
#ifndef __IMAP_CORE_H__
#define __IMAP_CORE_H__

#include <mach/sram.h>
#include <linux/smp.h>
/* Below is for imapx910 */

extern struct smp_operations imapx910_smp_ops;

void imapx910_secondary_startup(void);

/* this is implemented in irq_dispatch.c */
void __init imapx910_irq_dispatch_init(void);

/* this is implemented in core.c */
void __init imapx910_init_devices(void);

/* this is implemented in clock-ops.c */
void __init imapx910_init_clocks(void);

/* this is implemented in gpio.c */
void __init imapx910_init_gpio(void);

/* this is implemented in mem_reserve.c */
void __init imapx910_mem_reserve(void);

/* this is implemented in common.c */
void imapx910_power_off(void);

/* this is implemented in common.c */
void imapx910_enter_lowpower(void);

/* this is implemented in common.c */
void __init imapx910_init_l2x0(void);

/* this is implemented in common.c */
void __init imapx910_twd_init(void);

/* this is implemented in pm.c */
void __init imapx910_init_pm(void);

void a5pv10_reset(char mode, const char *cmd);
void a5pv10_poweroff(void);
void a5pv10_enter_lowpower(void);

void a5pv20_reset(char mode, const char *cmd);
void a5pv20_poweroff(void);
void __sramfunc a5pv20_enter_lowpower(void);

void a9pv10_reset(char mode, const char *cmd);
void a9pv10_poweroff(void);
void __sramfunc a9pv10_enter_lowpower(void);

#define core_msg(x...) pr_err(KERN_ERR "imapx: " x)

#endif /* __IMAP_CORE_H__ */

