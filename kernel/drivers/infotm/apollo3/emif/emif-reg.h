#ifndef __EMIF_REG_H__
#define __EMIF_REG_H__

#include <linux/io.h>
#include <mach/imap-emif.h>

#define __emif_readl(base, off)       __raw_readl(base + off)
#define __emif_writel(val, base, off) __raw_writel(val, base + off)

#define PORT0                0
#define PORT1                1
#define PORT2                2
#define PORT3                3
#define PORT4                4
#define PORT5                5
#define PORT6                6
#define PORT7                7

#define PORT_BE              0x0
#define PORT_LL              0x1

/*
 * set port 
 * 
 * n : port n
 */
static inline void emif_port_set(void __iomem *base, uint32_t n, uint32_t val)
{
    __emif_writel(val, base, UMCTL_PCFG(n));
}

static inline uint32_t emif_port_get(void __iomem *base, uint32_t n)
{
    return __emif_readl(base, UMCTL_PCFG(n));
}

#endif /* emif-reg.h */
