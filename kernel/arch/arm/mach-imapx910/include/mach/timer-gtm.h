#ifndef __ASMARM_SMP_GTIMER_H
#define __ASMARM_SMP_GTIMER_H

#define G_TIMER_COUNTER_L	0x00
#define G_TIMER_COUNTER_H	0x04
#define G_TIMER_CONTROL		0x08
#define G_TIMER_INTSTAT		0x0c
#define G_TIMER_COMPARE_L	0x10
#define G_TIMER_COMPARE_H	0x14
#define G_TIMER_AUTOINC		0x18

extern int imapx910_gtimer_init(void __iomem *reg, const char *name);
#ifdef CONFIG_CPU_FREQ
extern int imapx910_cmn_timer_update(long rate);
extern int twd_update_frequency(void *data);
#endif

#endif


