#ifndef __TIMER_HR__
#define __TIMER_HR__

#include <linux/clocksource.h>

struct imap_time {
	uint32_t time;
	cycle_t subtotal;
};

enum hr_status {
	HR_NOUSED,
	HR_USED,
};

struct clocksource_hr {
	void __iomem *reg;
	struct clocksource cs;
	cycle_t	fake_sec;
	cycle_t least;
	uint32_t load;
	struct imap_time caltime;
	int flags;
};


extern int imapx_hr_init(void __iomem *reg,
			unsigned int irq, const char *name);

#endif
