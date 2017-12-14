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

struct imapx910_irq_desc {
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

static struct imapx910_irq_desc imapx910_irq[] = {
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
};

#define IRQS_NR ARRAY_SIZE(imapx910_irq) 

static void irq_dispatch_notify(int cpu)
{
	cpumask_var_t mask;
	int i = 0, ret;

	mask->bits[0] = (0x1 << cpu);

	for (i = 0 ; i < IRQS_NR; i++) {
		if(imapx910_irq[i].cpu != cpu)
			continue;
		if ( imapx910_irq[i].irq > 0 && imapx910_irq[i].cpu > 0) {
			ret = irq_set_affinity(imapx910_irq[i].irq, mask);
			if(ret < 0) 
				pr_err("IRQ %d affinit to cpu%d err:%d\n",
						imapx910_irq[i].irq, imapx910_irq[i].cpu, ret);
		}
	}
	return;
}


static void __init irq_dispatch_default(void)
{
	cpumask_var_t mask;
	int i = 0, ret;
	for (i = 0 ; i < IRQS_NR; i++) {
		if ( imapx910_irq[i].irq > 0 && imapx910_irq[i].cpu > 0 &&
			cpu_online(imapx910_irq[i].cpu)) {
			mask->bits[0] = 1 << imapx910_irq[i].cpu;
			ret = irq_set_affinity(imapx910_irq[i].irq, mask);
			if(ret < 0) 
				pr_err("IRQ %d affinit to cpu%d err:%d\n",
						imapx910_irq[i].irq, imapx910_irq[i].cpu, ret);
				pr_info("set affinity irq %d to cpu%d sucesse!\n", 
						imapx910_irq[i].irq, imapx910_irq[i].cpu);
		}

	}
}


static int __cpuinit 
cpu_irq_callback(struct notifier_block *nfb, unsigned long action, void *hcpu)
{
	unsigned int cpu_id = (unsigned int)hcpu;
	int hotcpu = action & 0xf;
	switch (hotcpu) {
	case CPU_UP_PREPARE:
	case CPU_UP_PREPARE_FROZEN:
		break;
	case CPU_ONLINE:
	case CPU_ONLINE_FROZEN:
		irq_dispatch_notify(cpu_id);
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
	return NOTIFY_OK;
}

static struct notifier_block __cpuinitdata cpu_irq_nfb = {
	.notifier_call = cpu_irq_callback
};

void __init imapx910_irq_dispatch_init(void)
{
	pr_err("%s\n", __func__);
	irq_dispatch_default();
	register_cpu_notifier(&cpu_irq_nfb);
}
EXPORT_SYMBOL(imapx910_irq_dispatch_init);
