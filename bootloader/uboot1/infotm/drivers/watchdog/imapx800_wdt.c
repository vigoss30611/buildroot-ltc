#include <common.h>      
#include <asm/io.h>      
#include <lowlevel_api.h>
#include <imapx800_wdt.h>
#include <preloader.h>

void imapx_enable_clock(void)
{
    uint32_t ret;

    ret = readl(0x21e0ac08);
    ret |= (0x1 << 4);
    writel(ret, 0x21e0ac08);
}

void imapx_wdt_ctrl(uint32_t mode)
{
    uint32_t ret = 0;

    ret = wdt_readl(WDT_CR);
    ret &= ~(0x1 << 0);
    ret |= mode << 0;
    wdt_writel(ret, WDT_CR);
}

void imapx_wdt_mode(uint32_t mode)
{
    uint32_t ret = 0;

    ret = wdt_readl(WDT_CR);
    ret &= ~(1 << 1);
    ret |= mode << 1;
    wdt_writel(ret, WDT_CR);
}

void imapx_wdt_set_time(uint32_t value)
{
    wdt_writel(value, WDT_TORR);
}

void imapx_wdt_restart(void)
{
    wdt_writel(WDT_RESTART_DEFAULT_VALUE, WDT_CRR);
}

int imapx_wdt_set_heartbeat(int ms)
{
    int timeout = 0;
    int freq = WDT_MAX_FREQ_VALUE / 6;
    uint64_t count = (uint64_t)(ms) * freq;

    while(count >>= 1)
        timeout++;

    timeout++;  //for greater than what you set time
    if (timeout < 16)
        timeout = 0;
    if (timeout > 31)
        timeout = 15;
    else
        timeout -= 16;

    spl_printf("wdt %d\n", timeout);
    imapx_wdt_set_time(timeout);
    imapx_wdt_restart();
    imapx_wdt_ctrl(ENABLE);
    return 0;
}

void set_up_watchdog(int ms)
{
//    module_enable(WDT_SYSM_ADDR);

    if (ms <= 0)
        ms = WATCHDOG_TIMEOUT;

    imapx_wdt_ctrl(DISABLE);
    imapx_enable_clock();
    imapx_wdt_mode(WDT_SYS_RST);
    imapx_wdt_set_heartbeat(ms);
}
