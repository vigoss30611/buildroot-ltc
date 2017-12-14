#ifndef __IMPAX_WTD_H__
#define __IMPAX_WTD_H__

#include <linux/watchdog.h>

#define WATCHDOG_TIMEOUT        24000

#define WDT_SYS_RST                     0
#define WDT_SYS_RST_WITH_INTER          1

#define WDT_RESTART_DEFAULT_VALUE       0x76

#define ENABLE  1
#define DISABLE 0

#define WDT_CR                     0x00           // receive buffer register
#define WDT_TORR                   0x04           // interrupt enable register
#define WDT_CCVR                   0x08           // interrupt identity register
#define WDT_CRR                    0x0C           // line control register
#define WDT_STAT                   0x10           // modem control register
#define WDT_EOI                    0x14           // line status register

#endif
