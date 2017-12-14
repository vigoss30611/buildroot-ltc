#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include<asm-arm/arch-imapx800/imapx_base_reg.h>

#define WDT_SYS_RST                     0
#define WDT_SYS_RST_WITH_INTER          1

#define WATCHDOG_TIMEOUT                8

#define WDT_RESTART_DEFAULT_VALUE       0x76
#define WDT_MAX_FREQ_VALUE              804000000       //apll freq (HZ)

#define ENABLE  1
#define DISABLE 0

#define WDT_CR                     0x00           // receive buffer register
#define WDT_TORR                   0x04           // interrupt enable register
#define WDT_CCVR                   0x08           // interrupt identity register
#define WDT_CRR                    0x0C           // line control register
#define WDT_STAT                   0x10           // modem control register
#define WDT_EOI                    0x14           // line status register

#define wdt_writel(value, reg)     writel(value, reg + PWDT_BASE_ADDR)
#define wdt_readl(reg)              readl(reg + PWDT_BASE_ADDR) 

#endif
