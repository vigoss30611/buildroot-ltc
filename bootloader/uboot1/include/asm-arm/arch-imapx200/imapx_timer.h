#ifndef __IMAPX_TIMER_H__
#define __IMAPX_TIMER_H__

#define Timer0LoadCount      (TIMER_BASE_REG_PA+0x00)     /* timer0 Load Count register */
#define Timer1LoadCount      (TIMER_BASE_REG_PA+0x14)     /* timer1 Load Count register */
#define Timer0CurrentValue   (TIMER_BASE_REG_PA+0x04)     /* timer0 Current Value register */
#define Timer1CurrentValue   (TIMER_BASE_REG_PA+0x18)     /* timer1 Current Value register */
#define Timer0ControlReg     (TIMER_BASE_REG_PA+0x08)     /* timer0 Control register */
#define Timer1ControlReg     (TIMER_BASE_REG_PA+0x1C)     /* timer1 Control register */
#define Timer0EOI            (TIMER_BASE_REG_PA+0x0C)     /* timer0 End-of-Interrupt register */
#define Timer1EOI            (TIMER_BASE_REG_PA+0x20)     /* timer1 End-of-Interrupt register */
#define Timer0IntStatus      (TIMER_BASE_REG_PA+0x10)     /* timer0 Interrupt Status register */
#define Timer1IntStatus      (TIMER_BASE_REG_PA+0x24)     /* timer1 Interrupt Status register */
#define TimersIntStatus      (TIMER_BASE_REG_PA+0xA0)     /* Timers Interrupt Status register */
#define TimersEOI            (TIMER_BASE_REG_PA+0xA4)     /* Timers End-of-Interrupt register */
#define TimersRawIntStatus   (TIMER_BASE_REG_PA+0xA8)     /* Timers Raw Interrupt Status register */
#define TIMERS_COMP_VERSION  (TIMER_BASE_REG_PA+0xAC)     /* Timers Component Version */

#define IMAP_TCR_TIMER_EN          (1<<0)
#define IMAP_TCR_TIMER_MD          (1<<1)
#define IMAP_TCR_TIMER_INTMASK     (1<<2)

#endif /*__IMAPX_TIMER_H__*/
