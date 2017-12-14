/*************************************************************************
    > File Name: drivers/infotm/common/adc/adc-ip6303.c
    > Author: tsico
    > Mail: can.chai@infotm.com 
    > Created Time: 2017年09月29日 星期五 09时33分44秒
 ************************************************************************/
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
#include <mach/ip6303.h>
#include "adc-ip6303.h"


static inline struct ip6303_adc_chip *to_ip6303_adc_chip(struct adc_chip *chip) {
	return container_of(chip, struct ip6303_adc_chip, chip);
}

static void ip6303_adc_en(struct device *dev, int chan, int en) {
	uint8_t intr_mask;
	ip6303_adc_readl(dev, IP6303_ADC_ANA_CTL0, &intr_mask);
	if(!en)
		intr_mask &= ~(1<<(chan + 3));
	else
		intr_mask |= (1<<(chan + 3));
	ip6303_adc_writel(dev, IP6303_ADC_ANA_CTL0, intr_mask);
}

static void ip6303_adc_mask(struct device *dev) {
	uint8_t intr_mask;
	ip6303_adc_readl(dev, IP6303_INT_MASK1, &intr_mask);
	intr_mask |= 0x2;
	ip6303_adc_writel(dev, IP6303_INT_MASK1, intr_mask);
}

static int ip6303_adc_request(struct adc_chip *chip, struct adc_device *adc) {
	struct ip6303_adc_chip *pc = to_ip6303_adc_chip(chip); 
	uint8_t intr_mask;
	ip6303_adc_en(chip->dev->parent, adc->hwadc, 1);
	ip6303_adc_readl(chip->dev->parent, IP6303_INT_MASK1, &intr_mask);
	intr_mask |= 0x1;
	ip6303_adc_writel(chip->dev->parent, IP6303_INT_MASK1, intr_mask);
	adc->irq = pc->irq;

	return 0;
}

static void ip6303_adc_free(struct adc_chip *chip, struct adc_device *adc) {

	uint8_t intr_mask;
	ip6303_adc_en(chip->dev->parent, adc->hwadc, 0);
	ip6303_adc_readl(chip->dev->parent, IP6303_INT_MASK1, &intr_mask);
	intr_mask |= 0x2;
	ip6303_adc_writel(chip->dev->parent, IP6303_INT_MASK1, intr_mask);
	adc->irq = 0;

	return;
}

static int ip6303_adc_enable(struct adc_chip *chip, struct adc_device *adc) {
	uint8_t intr_mask;
	ip6303_adc_en(chip->dev->parent, adc->hwadc, 1);
	ip6303_adc_readl(chip->dev->parent, IP6303_INT_MASK1, &intr_mask);
	intr_mask &= 0xfd;
	ip6303_adc_writel(chip->dev->parent, IP6303_INT_MASK1, intr_mask);
	return 0;
}

static void ip6303_adc_disbale(struct adc_chip *chip, struct adc_device *adc) {
	uint8_t intr_mask;
	ip6303_adc_en(chip->dev->parent, adc->hwadc, 0);
	ip6303_adc_readl(chip->dev->parent, IP6303_INT_MASK1, &intr_mask);
	intr_mask &= 0x2;
	ip6303_adc_writel(chip->dev->parent, IP6303_INT_MASK1, intr_mask);
}

static int ip6303_adc_get_irq_state(struct adc_chip *chip, struct adc_device *adc) {
	uint8_t state = 0;
	ip6303_adc_readl(chip->dev->parent, IP6303_INT_FLAG1, &state);
	return (int)state;
}

static void ip6303_adc_clear_irq(struct adc_chip *chip, struct adc_device *adc) {
	uint8_t state = 0;
	ip6303_adc_readl(chip->dev->parent, IP6303_INT_FLAG1, &state);
	state |= 0x2;
	ip6303_adc_writel(chip->dev->parent, IP6303_INT_FLAG1, state);
}

static int ip6303_adc_read_raw(struct adc_chip *chip, struct adc_device *adc,
		int *val, int *val1) {
	uint8_t value;
	if(adc->hwadc == 0)
		ip6303_adc_readl(chip->dev->parent, IP6303_ADC_DATA_GP1, &value);
	else if(adc->hwadc == 1)
		ip6303_adc_readl(chip->dev->parent, IP6303_ADC_DATA_GP2, &value);
	return (int)value;
}

static int imapx_adc_set_irq_watermark(struct adc_chip *chip, struct adc_device *adc,
		int irq_watermark) {
	uint8_t value;
	ip6303_adc_readl(chip->dev->parent, IP6303_INT_MASK1, &value);
	if(irq_watermark)
		value |= (irq_watermark << 1);
	else
		value &= ~(1 << 1);
	ip6303_adc_writel(chip->dev->parent, IP6303_INT_MASK1, value);
	return 0;
}


static const struct adc_ops ip6303_adc_ops = {
	.request = ip6303_adc_request,
	.free = ip6303_adc_free,
	.enable = ip6303_adc_enable,
	.disable = ip6303_adc_disbale,
	.read_raw = ip6303_adc_read_raw,
	.get_irq_state = ip6303_adc_get_irq_state,
	.clear_irq = ip6303_adc_clear_irq,
	.set_irq_watermark = imapx_adc_set_irq_watermark,
	.owner = THIS_MODULE,
};

static void ip6303_adc_ctrl_init(struct device *dev) {
	uint8_t val;
	/*MFP : set GPIO1 to GP1ADC and GPIO2 to GP2ADC*/
	ip6303_adc_readl(dev, IP6303_MFP_CTL0, &val);
	val &= 0xf8;
	val |= 0x10;
	ip6303_adc_writel(dev, IP6303_MFP_CTL0, val);
	/*enable GP1ADC and P2ADC*/
	ip6303_adc_readl(dev, IP6303_ADC_ANA_CTL0, &val);
	val |= 0x18;
	ip6303_adc_writel(dev, IP6303_ADC_ANA_CTL0, val);

#if 1
	/* set gpio3 to cpuirq function*/
	ip6303_read(dev, IP6303_MFP_CTL0, &val);
	val &= 0x9f;
	ip6303_write(dev, IP6303_MFP_CTL0, val);

	ip6303_read(dev, IP6303_PAD_CTL, &val); 
	val |= 0x8; 
	ip6303_write(dev, IP6303_PAD_CTL, val);

	/* set cpuirq high valid*/
	ip6303_read(dev, IP6303_INTS_CTL, &val);
	val |= 0x1;
	ip6303_write(dev, IP6303_INTS_CTL, val);

	/* clear flags */
	val = 0x0; 
	ip6303_write(dev, IP6303_INT_FLAG1, val);
#endif
}

static int ip6303_adc_probe(struct platform_device *pdev) {
	struct adc_platform_data *pdata = pdev->dev.platform_data;
	struct ip6303_adc_chip *chip;
	unsigned int i, j;
	int ret;

	pr_err("ip6303 [Analog Digital Convertor]adc driver probe!\n");
	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}
	memset(chip, 0x0, sizeof(*chip));

	chip->dev = &pdev->dev;

	chip->irq =	IP6303_ADC_KEY_IRQ; 

	platform_set_drvdata(pdev, chip);

	chip->chan_bitmap = pdata->chan_bitmap;
	for (i = 0, j = 0; i < IP6303_ADC_CHNS_NUM; i++) {
		if (chip->chan_bitmap & (0x1 << i)) {
			chip->event_bitmap |= (0x1 << j);
			j++;
		}
	}

	ip6303_adc_ctrl_init(pdev->dev.parent);
	ip6303_adc_mask(pdev->dev.parent);	

	chip->chip.priority = 0;
	chip->chip.name = "ip6303-adc";
	chip->chip.dev = &pdev->dev;
	chip->chip.ops = &ip6303_adc_ops;
	chip->chip.base = -1;
	chip->chip.nadc = IP6303_ADC_CHNS_NUM;
	ret = adc_chip_add(&chip->chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "adc_chip_add() failed: %d\n", ret);
		return ret;
	}
	pr_info("ip6303 adc chip probe success with channel map 0x%lx.\n", chip->chan_bitmap);
	return 0;
}

static int ip6303_adc_remove(struct platform_device *pdev)
{
	struct ip6303_adc_chip *chip = platform_get_drvdata(pdev);
	if (WARN_ON(!chip))
		return -ENODEV;
	return adc_chip_remove(&chip->chip);

}

#ifdef CONFIG_PM_SLEEP
static int ip6303_adc_suspend(struct device *dev) {
	return 0;
}

static int ip6303_adc_resume(struct device *dev) {
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ip6303_adc_pm_ops, ip6303_adc_suspend,
		ip6303_adc_resume);

static struct platform_driver ip6303_adc_driver = {
	.driver = {
		.name = "ip6303-adc",
		.owner = THIS_MODULE,
		.pm = &ip6303_adc_pm_ops	
	},
	.probe = ip6303_adc_probe,
	.remove = ip6303_adc_remove,

};

static int __init ip6303_adc_init(void)
{
	pr_info("imapx Analog-To-Digital(ADC) converter init ...\n");
	return platform_driver_register(&ip6303_adc_driver);
}
arch_initcall(ip6303_adc_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("InfotmIC-PS");
MODULE_ALIAS("platform:ip6303-adc");
