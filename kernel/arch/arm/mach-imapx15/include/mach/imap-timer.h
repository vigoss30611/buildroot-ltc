#ifndef __IMAPX_TIMER__
#define __IMAPX_TIMER__

#define TIMER_USEC_SHIFT 16

#define TIMER0_LOADCOUNT		0x00
#define TIMER0_CURRENTVAL		0x04
#define TIMER0_CTRLREG			0x08
#define TIMER0_EOI			0x0c
#define TIMER0_INTSTS			0x10

#define TIMER1_LOADCOUNT		0x14
#define TIMER1_CURRENTVAL		0x18
#define TIMER1_CTRLREG			0x1c
#define TIMER1_EOI			0x20
#define TIMER1_INTSTS			0x24

#define TIMERS_INTSTS			0xa0
#define TIMERS_EOI			0xa4
#define TIMERS_RAWINTSTS		0xa8
#define TIMERS_COMPVER			0xac

#define IMAP_TCR_TIMER_EN		(1<<0)
#define IMAP_TCR_TIMER_MD		(1<<1)
#define IMAP_TCR_TIMER_INTMSK		(1<<2)


#endif
