/*
 * control to dispatch irqs to any online cpu manually
 *
 * started by sun, Copyright (C) 2012 Infotm.
 */
#include <linux/mm.h>
#include <linux/cpu.h>
#include <linux/nmi.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/freezer.h>
#include <linux/kthread.h>
#include <linux/lockdep.h>
#include <linux/notifier.h>
#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/interrupt.h>
#include <asm/irq_regs.h>

#ifdef IMAPX_IRQ_DEBUG
#define irq_dbg(x...)      printk(x)
#else
#define irq_dbg(x...)
#endif

struct imapx820_irq_desc {
	int irq;
	int cpu;
};

#define IMAPX_IRQ_DISPATCH_DESC(_irq, _cpu)       \
{\
	.irq = _irq, \
	.cpu = _cpu, \
}

#define CPU_NONE        -1
#define CPU0            0
#define CPU1            1
#define CPU2            2
#define CPU3            3

static struct imapx820_irq_desc imapx_irq[] = {
#ifdef CONFIG_PL330_DMA
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA0_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA1_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA2_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA3_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA4_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA5_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA6_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA7_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA8_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA9_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA10_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMA11_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_DMABT_ID, CPU1),
#endif
	IMAPX_IRQ_DISPATCH_DESC(GIC_IIC0_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_IIC1_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_IIC2_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_IIC3_ID, CPU1),
#ifdef CONFIG_SERIAL_AMBA_PL011
	IMAPX_IRQ_DISPATCH_DESC(GIC_UART0_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_UART2_ID, CPU1),
	IMAPX_IRQ_DISPATCH_DESC(GIC_UART3_ID, CPU1),
#endif
	IMAPX_IRQ_DISPATCH_DESC(NO_IRQ, CPU_NONE),
};

void irq_dispatch_enable(int cpu)
{
/*
	cpumask_var_t mask;
	int i = 0, ret;

	mask->bits[0] = (0x1 << cpu);
	while (imapx_irq[i].irq != NO_IRQ) {
		if (imapx_irq[i].cpu != cpu) {
			irq_dbg("Irq %d can't be set to cpu %d,
					Online cpu now %d\n",
			     imapx_irq[i].irq, imapx_irq[i].cpu, NR_CPUS);
			continue;
		}

		ret = irq_set_affinity(imapx_irq[i].irq, mask);
		if (ret < 0)
			irq_dbg("Irq %d set to cpu%d err: %d\n",
				imapx_irq[i].irq, imapx_irq[i].cpu, ret);
		i++;
	}
*/
}

int cpu_id = CPU1;

static int __cpuinit
cpu_irq_callback(struct notifier_block *nfb, unsigned long action, void *hcpu)
{
/*
	int hotcpu = action & 0xf;

	switch (hotcpu) {
	case CPU_UP_PREPARE:
	case CPU_UP_PREPARE_FROZEN:
		break;
	case CPU_ONLINE:
	case CPU_ONLINE_FROZEN:
		if (cpu_id == NR_CPUS)
			cpu_id = CPU1;

		irq_dispatch_enable(cpu_id);
		cpu_id++;
		break;
#ifdef CONFIG_HOTPLUG_CPU
	case CPU_UP_CANCELED:
	case CPU_UP_CANCELED_FROZEN:
		break;
	case CPU_DEAD:
	case CPU_DEAD_FROZEN:
		break;
#endif
	default:
		break;
	}
*/

	return NOTIFY_OK;
}

static struct notifier_block __cpuinitdata cpu_irq_nfb = {
	.notifier_call = cpu_irq_callback
};

void __init irq_dispatch_init(void)
{
/*
	if (NR_CPUS == 1)
		return;
	register_cpu_notifier(&cpu_irq_nfb);
*/
}
EXPORT_SYMBOL(irq_dispatch_init);
