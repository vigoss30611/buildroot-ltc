/*
 *  infotm adc driver implementation 
 *  We won't support touchscreen in this version
 *  This version only surport Apollo & Apollo2
 */
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/adc.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/pad.h>
#include <mach/imap-adc.h>
#include "adc-imapx.h"


static inline struct imapx_adc_chip *to_imapx_adc_chip(struct adc_chip *chip) {
	return container_of(chip, struct imapx_adc_chip, chip);
}

static void imapx_adc_irq_en(struct imapx_adc_chip *chip, int chan, int en) {
	uint32_t intr_mask;
	intr_mask = adc_readl(chip, TSC_INT_MASK);
	if (en)
		intr_mask &= ~(1<<chan);
	else
		intr_mask |= (1<<chan);
	adc_writel(chip, TSC_INT_MASK, intr_mask);
}

static void imapx_adc_mask_all(struct imapx_adc_chip *chip) {
	uint32_t intr_mask;
	intr_mask = adc_readl(chip, TSC_INT_MASK);
	intr_mask |= 0x3FFF;
	adc_writel(chip, TSC_INT_MASK, intr_mask);
}

static int iampx_adc_get_chnirq_state(struct imapx_adc_chip *chip, int chan) {
	uint32_t irq_state;
	int state = 0;
	irq_state = adc_readl(chip, TSC_INT_CLEAR);
//	irq_mask = adc_readl(chip, TSC_INT_MASK);
//	irq_state &= ~irq_mask;
	state = (irq_state >> chan) & 0x1; 
	if (state) {
		adc_writel(chip, TSC_INT_CLEAR, irq_state & (1 << chan));
	}
	
	return state;
}


static int imapx_adc_request(struct adc_chip *chip, struct adc_device *adc) {
	struct imapx_adc_chip *pc = to_imapx_adc_chip(chip);

	if (adc->mode == ADC_DEV_IRQ_SAMPLE) {
		imapx_adc_irq_en(pc, adc->hwadc, 1);
	} else if (adc->mode == ADC_DEV_SAMPLE)
		imapx_adc_irq_en(pc, adc->hwadc, 0);
	adc->irq = pc->irq;

	return 0;
}

static void imapx_adc_free(struct adc_chip *chip, struct adc_device *adc) {
	struct imapx_adc_chip *pc = to_imapx_adc_chip(chip);

	if (adc->mode == ADC_DEV_IRQ_SAMPLE) {
		imapx_adc_irq_en(pc, adc->hwadc, 0);
	}
	adc->irq = 0;

	return;
}

static int imapx_adc_enable(struct adc_chip *chip, struct adc_device *adc) {
	struct imapx_adc_chip *pc = to_imapx_adc_chip(chip);
	if (adc->mode == ADC_DEV_IRQ_SAMPLE)
		imapx_adc_irq_en(pc, adc->hwadc, 1);
	return 0;
}

static void imapx_adc_disbale(struct adc_chip *chip, struct adc_device *adc) {
	struct imapx_adc_chip *pc = to_imapx_adc_chip(chip);
	if (adc->mode == ADC_DEV_IRQ_SAMPLE)
		imapx_adc_irq_en(pc, adc->hwadc, 0);
	
}

static int imapx_adc_get_irq_state(struct adc_chip *chip, struct adc_device *adc) {
	struct imapx_adc_chip *pc = to_imapx_adc_chip(chip);
	return iampx_adc_get_chnirq_state(pc, adc->hwadc);
}



static int imapx_adc_read_raw(struct adc_chip *chip, struct adc_device *adc,
				int *val, int *val1) {
	struct imapx_adc_chip *pc = to_imapx_adc_chip(chip);
	if(val)
		*val = adc_readl(pc, TSC_PPOUT(adc->hwadc));
	//pr_err("%s [%d] --val-[%d]--\n", __func__, adc->hwadc, *val);
	return ADC_VAL_INT;
}

static int imapx_adc_set_irq_watermark(struct adc_chip *chip, struct adc_device *adc,
					int irq_watermark) {
	struct imapx_adc_chip *pc = to_imapx_adc_chip(chip);
	adc_writel(pc, TSC_IRQ_PAR(adc->hwadc), irq_watermark | ( 0x000 << 16));
	return 0;
}


static const struct adc_ops imapx_adc_ops = {
	.request = imapx_adc_request,
	.free = imapx_adc_free,
	.enable = imapx_adc_enable,
	.disable = imapx_adc_disbale,
	.read_raw = imapx_adc_read_raw,
	.get_irq_state = imapx_adc_get_irq_state,
	.clear_irq = NULL,
	.set_irq_watermark = imapx_adc_set_irq_watermark,
	.owner = THIS_MODULE,
};

static uint32_t imapx_adc_emit_endcode (volatile uint32_t buf[] ) {
	buf[0] = CMD_INSTR_END;
	return SZ_ENDCODE;
}

static uint32_t imapx_adc_emit_sample (volatile uint32_t buf[], int chan) {
	buf[0] = 0x7fe;
	buf[1] = CMD_CH_SAMPLE(chan) | (1<<NML_STCOV_BIT) | (1<<NML_AVG_BIT);
	buf[2] = CMD_CH_SAMPLE(chan) | (1<<NML_AVG_BIT);
	buf[3] = CMD_INSTR_NOP | (SAMPLE_DLYCLK & JMP_CYCLE_MSK);
	buf[4] = CMD_CH_SAMPLE (chan) | ( 1 << NML_ADC_OUT_BIT) | (1 << NML_AVG_BIT) | (1 << NML_CNINTR_BIT);
	buf[5] = (1 << NML_PPOUT_BIT) | ( 1 << NML_CNINTR_BIT);
	buf[6] = CMD_CH_SAMPLE (chan) | (1 << NML_ADC_OUT_BIT) | ( 0 << NML_AVG_BIT ) | ( 1 << NML_CNINTR_BIT );
	buf[7] = 0x7fe;
	return SZ_CHSAMPLE;
}

static int imapx_adc_find_chan_by_bitmap(struct imapx_adc_chip *chip, int index) {
	int i, flag = 0;
	for(i = 0; i <= CHN_INAUX2; i++)
		if(chip->chan_bitmap & (0x1 << i)) {	
			if(index == flag)
				return i;
			flag++;
		}
	return  i;
		
}

static void imapx_adc_mc_setup(struct imapx_adc_chip *chip) {
	unsigned int i, off = 0x8;
	uint32_t task_start[8];
	volatile u32 *buf = (volatile u32 *)(chip->base + TSC_MEM_BASE_ADDR);
	int chan;
	
	for(i = 0; i < 8; i++) {
		/*we don't support touch-screen*/
		if (chip->event_bitmap & ( 0x1 << i)) {
			task_start[i] = off;
			chan = imapx_adc_find_chan_by_bitmap(chip, i);	
			off += imapx_adc_emit_sample(&buf[off], imapx_adc_find_chan_by_bitmap(chip, i));
			off += imapx_adc_emit_endcode(&buf[off]);
		} else {
			task_start[i] = 0x40000000;
		}
	} 
	
	for(i = 0; i < 8; i++) {
		buf[i] = CMD_INSTR_JMP | CMD_JMP_UNCOND | task_start[i];
	}
	
}

static int imapx_adc_ctrl_init(struct imapx_adc_chip *chip) {

	unsigned int i;

	imapx_adc_mc_setup(chip);
	adc_writel(chip, TSC_CNTL, (chip->event_bitmap << 8));

	adc_writel(chip, TSC_EVTPAR0, 1000);
	adc_writel(chip, TSC_EVTPAR1, 1000);
	adc_writel(chip, TSC_EVTPAR2, 1000);
	adc_writel(chip, TSC_EVTPAR3, 1000);
	adc_writel(chip, TSC_EVTPAR4, 1000);
	adc_writel(chip, TSC_EVTPAR5, 1000);
	adc_writel(chip, TSC_EVTPAR6, 1000);
	adc_writel(chip, TSC_EVTPAR7, 1000);

	adc_writel(chip, TSC_PP_PAR, 0x0|(0x80<<16));
	adc_writel(chip, TSC_FIFO_TH, 0x4 | (chip->chan_bitmap) <<16);

#if 0
	adc_writel(chip, TSC_INT_PAR, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR1, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR2, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR3, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR4, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR5, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR6, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR7, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR8, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR9, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR10, 0x0390|(0x000<<16));
	adc_writel(chip, TSC_INT_PAR11, 0x0390|(0x000<<16));
#endif
	
	adc_writel(chip, TSC_INT_MASK, ~(chip->chan_bitmap) | (0x1 << 12));
	adc_writel(chip, TSC_PRES_SEL, 20<<8);
	
	adc_writel(chip, TSC_CNTL, 0x13| (chip->event_bitmap<<8));
	adc_writel(chip, TSC_CNTL, 0x13| (chip->event_bitmap<<8));
	adc_writel(chip, TSC_CNTL, 0x17| (chip->event_bitmap<<8));
	adc_writel(chip, TSC_CNTL, 0x11);
	for(i=0; i<800; i++);
	adc_writel(chip, TSC_CNTL, 0x13| (chip->event_bitmap<<8));

	return 0;
}

static int imapx_adc_probe(struct platform_device *pdev) {
	struct adc_platform_data *pdata = pdev->dev.platform_data;
	struct imapx_adc_chip *chip;
	struct resource *res;
	unsigned int i, j;
	int ret;

	pr_err("imapx [Analog Digital Convertor]adc driver probe!\n");
	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}
	memset(chip, 0x0, sizeof(*chip));
	
	chip->dev = &pdev->dev;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	chip->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(chip->base))
		return PTR_ERR(chip->base);
	chip->irq = platform_get_irq(pdev, 0);
	if (chip->irq < 0)
		return chip->irq;

	chip->bus_clk = clk_get_sys("imap-tsc",NULL);
	if ((IS_ERR(chip->bus_clk)))
		return PTR_ERR(chip->bus_clk);
	ret = clk_prepare_enable(chip->bus_clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to enable adc bus clock\n");
		return ret;
	}

	chip->clk = clk_get_sys("imap-tsc-ctrl","imap-tsc");
	if ((IS_ERR(chip->clk)))
		return PTR_ERR(chip->clk);
	ret = clk_prepare_enable(chip->clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to enable adc clock\n");
		return ret;
	}

	platform_set_drvdata(pdev, chip);

	module_power_on(SYSMGR_ADC_BASE);

	chip->chan_bitmap = pdata->chan_bitmap;
	for (i = 0, j = 0; i <= CHN_INAUX2; i++) {
		if (chip->chan_bitmap & (0x1 << i)) {
			chip->event_bitmap |= (0x1 << j);
			j++;
		}
	}
	
	imapx_adc_ctrl_init(chip);
	imapx_adc_mask_all(chip);	

	chip->chip.priority = 1;
	chip->chip.name = "imap-adc";
	chip->chip.dev = &pdev->dev;
	chip->chip.ops = &imapx_adc_ops;
	chip->chip.base = -1;
	chip->chip.nadc = NUM_ADC_CHNS;
	ret = adc_chip_add(&chip->chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "adc_chip_add() failed: %d\n", ret);
		return ret;
	}
	pr_info("imapx adc probe success with channel map 0x%lx.\n", chip->chan_bitmap);
	return 0;
}

static int imapx_adc_remove(struct platform_device *pdev)
{
	struct imapx_adc_chip *chip = platform_get_drvdata(pdev);
	if (WARN_ON(!chip))
		return -ENODEV;
	clk_disable_unprepare(chip->clk);
	clk_disable_unprepare(chip->bus_clk);
	return adc_chip_remove(&chip->chip);
	
}

#ifdef CONFIG_PM_SLEEP
static int imapx_adc_suspend(struct device *dev) {
		return 0;
}

static int imapx_adc_resume(struct device *dev) {
		return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(imapx_adc_pm_ops, imapx_adc_suspend,
		imapx_adc_resume);

static struct platform_driver imapx_adc_driver = {
	.driver = {
		.name = "imap-adc",
		.owner = THIS_MODULE,
		.pm = &imapx_adc_pm_ops	
	},
	.probe = imapx_adc_probe,
	.remove = imapx_adc_remove,
	
};

static int __init imapx_adc_init(void)
{
	pr_info("imapx Analog-To-Digital(ADC) converter init ...\n");
	return platform_driver_register(&imapx_adc_driver);
}
arch_initcall(imapx_adc_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("InfotmIC");
MODULE_ALIAS("platform:imap-adc");

