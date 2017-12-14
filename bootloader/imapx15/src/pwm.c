#include <linux/types.h>
#include <preloader.h>
#include <asm-arm/io.h>
#include <isi.h>
#include <asm-arm/arch-apollo3/imapx_pads.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>

extern struct irom_export *irf;

static int pwm_channel = -1;

#define PWM_TCFG0 (PWM_TIMER_BASE_ADDR + 0)
#define PWM_TCFG1 (PWM_TIMER_BASE_ADDR + 4)
#define PWM_TCON  (PWM_TIMER_BASE_ADDR + 8)
#define PWM_TCNTB(x) (PWM_TIMER_BASE_ADDR + (x * 0xc + 0x0c))
#define PWM_TCMPB(x) (PWM_TIMER_BASE_ADDR + (x * 0xc + 0x10))

int pwm_init(int channel)
{
    if(channel < 0 || channel > 2) {
        irf->printf("pwm: channel %d is invalid.\n", channel);
        return -1;
    }

    irf->module_enable(PWM_SYSM_ADDR);
    if(IROM_IDENTITY == IX_CPUID_X15)
        irf->pads_chmod(59 - channel, PADS_MODE_CTRL, 0);
    else                        /*X9 the same as X800*/
        irf->pads_chmod(72 - channel, PADS_MODE_CTRL, 0);

    writel(0x0000, PWM_TCFG0);  /* do not use any prescalers */
    writel(0x8888, PWM_TCFG1);  /* divid by 2 from PCLK */

    writel(readl(PWM_TCON) & ~(1 << 4), PWM_TCON); /* shutdown deadzone */

    pwm_channel = channel;

    return 0;
}

int pwm_set(int freq, int duty)
{
    uint32_t base = 0, tcon;

    if(pwm_channel < 0) {
        irf->printf("pwm: not initialized.\n");
        return -1;
    }

    if(pwm_channel)
        base = (pwm_channel + 1) * 4;

    tcon = readl(PWM_TCON);
    tcon &= ~(7 << base);
    tcon |= (0x8 << base);
    writel(tcon, PWM_TCON);  /* enable autoreload */

    /* update frequency & duty */
    writel(freq, PWM_TCNTB(pwm_channel));
    writel(freq * duty / 255, PWM_TCMPB(pwm_channel));

    tcon |= (0x2 << base);
    writel(tcon, PWM_TCON);  /* update tcnt */

    tcon &= ~(0x2 << base);
    tcon |= (0x1 << base);
    writel(tcon, PWM_TCON);  /* start timer */

    return 0;
}
