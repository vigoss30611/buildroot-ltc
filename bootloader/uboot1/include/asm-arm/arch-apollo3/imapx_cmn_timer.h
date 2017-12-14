#ifndef __IMAPX_CMN_TIMER_H__
#define __IMAPX_CMN_TIMER_H__

#define rTimerLoadCount(x)     (*(volatile unsigned *)(CMN_TIMER_BASE_ADDR + 0x00 + 0x14 * x))     //timer Load Count register
#define rTimerCurrentValue(x)  (*(volatile unsigned *)(CMN_TIMER_BASE_ADDR + 0x04 + 0x14 * x))     //timer Current Value register
#define rTimerControlReg(x)    (*(volatile unsigned *)(CMN_TIMER_BASE_ADDR + 0x08 + 0x14 * x))     //timer Control register
#define rTimerEOI(x)           (*(volatile unsigned *)(CMN_TIMER_BASE_ADDR + 0x0C + 0x14 * x))     //timer End-of-Interrupt register
#define rTimerIntStatus(x)     (*(volatile unsigned *)(CMN_TIMER_BASE_ADDR + 0x10 + 0x14 * x))     //timer Interrupt Status register

#endif /* imapx_cmn_timer.h */
