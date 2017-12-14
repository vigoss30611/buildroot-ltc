#ifndef _AXP_IRQ_H_
#define _AXP_IRQ_H_

#include <asm/io.h>
#include <mach/imap-iomap.h>

#define base    IO_ADDRESS(SYSMGR_RTC_BASE)

static void axp_rtcgp_irq_config(int io)
{
    uint8_t val;

    /*set RTCGPn to input mode*/
    val = readl(base + 0x4c);
    val |= 1<<io;
    writel(val, base + 0x4c);

    /*enable RTCGPn pulldown*/
    val = readl(base + 0x54);
    val |= 1<<io;
    writel(val, base + 0x54);

    /*set intr level*/
    val = readl(base + 0x5c);
    val |= 1<<(4+io);/*low*/
    writel(val, base + 0x5c);
}

/*
static void axp_rtcgp_irq_mask(int io)
{
    uint8_t val;

    val = readl(base + 0xC);
    val |= 1<<(3+io);
    writel(val, base + 0xC);
}

static void axp_rtcgp_irq_unmask(int io)
{                                
    uint8_t val;             

    val = readl(base + 0xC); 
    val &= ~(1<<(3+io));     
    writel(val, base + 0xC); 
}
*/

static void axp_rtcgp_irq_clr(int io)                  
{                                               
    uint8_t val;                            

    val = readl(base + 0x8);                
    if(val & (1<<(3+io))) {                 
	writel((1<<(3+io)), base + 0x4);
    }                                       
}

static void axp_rtcgp_irq_enable(int io)
{                                
    uint8_t val;             

    val = readl(base + 0x5c);
    val |= 1 << io;          
    writel(val, base + 0x5c);
}                                

static void axp_rtcgp_irq_disable(int io)
{                                 
    uint8_t val;              

    val = readl(base + 0x5c); 
    val &= ~(1<<io);          
    writel(val, base + 0x5c); 
}

#endif
