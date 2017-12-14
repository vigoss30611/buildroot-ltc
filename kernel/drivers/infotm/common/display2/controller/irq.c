/* display irq driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <mach/power-gate.h>
#include <linux/display_device.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"irq"

extern int display_log_level;

static char *irq_pending_str[] = {
	"LCDINT", "VCLKINT", "OSDW0INT", "OSDW1INT",
	"OSDERR", "I80INT", "RSVD", "TVINT"
};

static int irq_enable(struct display_device *pdev, int en)
{
	pdev->enable = en;
	if (en)
		enable_irq(pdev->irq);
	else
		disable_irq(pdev->irq);

	return 0;
}

static int irq_set_mask(struct display_device *pdev, unsigned char mask)
{
	pdev->irq_mask = mask;
	ids_writeword(pdev->path, IDSINTMSK, mask);

	return 0;
}

static int irq_wait_vsync(struct display_device *pdev, unsigned long timeout)
{
	int ret;

	dlog_irq("waiting for vsync irq ...\n");
	ret = wait_for_completion_interruptible_timeout(&pdev->vsync, timeout);
	if (ret <= 0) {
		dlog_err("waiting %s !!!\n", (ret == -ERESTARTSYS? "interrupted": "time out"));
		return -EIO;
	}

	return 0;
}

irqreturn_t __controller_irq_handler(int irq, void *data)
{
	struct display_device *pdev = (struct display_device *)data;
	int status, vclkfsr, i;

	status = ids_readword(pdev->path, IDSINTPND);
	if (status & IDSINTPND_LCDINT || status & IDSINTMSK_I80INT || status & IDSINTPND_TVINT) {
		complete(&pdev->vsync);
		dlog_irq("complete !\n");
	}

	if (display_log_level >= LOG_LEVEL_IRQ) {
		for (i = 0; i < 8; i++) {
			if (status & (1 << i))
				dlog_irq("pending: %s\n", irq_pending_str[i]);
		}
	}

	if (status & (1<<IDSINTPND_VCLKINT)) {
		dlog_err("VCLKINT\n");
		vclkfsr = ids_readword(pdev->path, LCDVCLKFSR);
		if (vclkfsr & 0x01) {
			dlog_err("VCLKFAC, CDOWN=%d, RFRM_NUM=%d, CLKVAL=%d\n",
							(vclkfsr>>24)&0x0F, (vclkfsr>>16)&0x3F, (vclkfsr>>4)&0x3FF);
			ids_writeword(pdev->path, LCDVCLKFSR, vclkfsr | 0x01);
		}
	}

	if (status & (1<<IDSINTPND_OSDERR)) {
		dlog_err("OSDERR\n");
	}

	ids_writeword(pdev->path, IDSSRCPND, status);
	ids_writeword(pdev->path, IDSINTPND, status);

	return IRQ_HANDLED;
}

static int irq_probe(struct display_device *pdev)
{
	int ret;
	struct resource *res_irq;
	struct resource *res_mem;
	char name[16] = {0};

	dlog_trace();

	module_power_on(__ids_sysmgr(pdev->path));

	init_completion(&pdev->vsync);

	res_irq = display_get_resource(pdev, IORESOURCE_IRQ, pdev->path);
	pdev->irq = res_irq->start;		// store
	res_mem = display_get_resource(pdev, IORESOURCE_MEM, pdev->path);
	dlog_info("path%d: irq=%d, ids register mem base=0x%X\n",
						pdev->path, res_irq->start, res_mem->start);
	snprintf(name, sizeof(name), "ids%d", pdev->path);

	if (pdev->path == 0) {
		ret = request_irq(pdev->irq, __controller_irq_handler, IRQF_ONESHOT,
			"ids0", pdev);
	} else {
		ret = request_irq(pdev->irq, __controller_irq_handler, IRQF_ONESHOT,
			"ids1", pdev);
	}

	if (ret) {
		dlog_err("path%d: request irq%d failed %d\n", pdev->path, res_irq->start, ret);
		return -1;
	}

	disable_irq(pdev->irq);
	ret = ids_access_init(pdev->path, res_mem);
	if (ret) {
		dlog_err("ids memory access init failed %d\n", ret);
		return ret;
	}

	return 0;
}

static int irq_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver irq_driver = {
	.probe  = irq_probe,
	.remove = irq_remove,
	.driver = {
		.name = "irq",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_IRQ,
	.function = {
		.enable = irq_enable,
		.set_irq_mask = irq_set_mask,
		.wait_vsync = irq_wait_vsync,
	}
};

module_display_driver(irq_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display controller irq driver");
MODULE_LICENSE("GPL");
