#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/interrupt.h>
#include <mach/pad.h>
#include <mach/hw_cfg.h>
#include <linux/clk.h>

static struct irq_domain *irq_domain;

struct imapx_gpio_bank {
	int bank;
	int irq;
	int nr;

	uint8_t dir[16];
	uint8_t out[16];
	uint8_t flt[16];
	uint8_t clkdiv[16];
	uint8_t int_msk[16];
	uint8_t int_gmsk[16];
	uint8_t int_pend[16];
	uint8_t int_type[16];
};

struct imapx_gpio_chip {
	const char compatible[128];
	void __iomem *base;
	struct gpio_chip *chip;
	struct irq_chip *irq_chip;
	int bankcount;
	struct imapx_gpio_bank *bank;
};

static struct imapx_gpio_chip *to_imapx_gpiochip(struct gpio_chip *chip)
{
	return container_of(&chip, struct imapx_gpio_chip, chip);
}

static int apollo_gpio_direction(struct gpio_chip *chip, unsigned offset)
{
	uint8_t tmp;
	struct imapx_gpio_chip *igc = to_imapx_gpiochip(chip);
	//pr_err("%s: in\n", __func__);

	tmp = readl(GPIO_DIR(offset));

	/* In apollo, return 1 means gpio direction output. But
	 * 1 means input in gpio system. So we reverse here. */
	return !tmp;
}

static int apollo_gpio_input(struct gpio_chip *chip, unsigned offset)
{
	struct imapx_gpio_chip *igc = to_imapx_gpiochip(chip);
	uint8_t tmp;
	int cnt = 0;
	int num = 0;
	//pr_err("%s: in\n", __func__);
	
	cnt = offset / 8;
	num = offset % 8;

	tmp = readl(PAD_IO_GPIOEN + cnt * 4);
	tmp |= 1 << num;
	writel(tmp, (PAD_IO_GPIOEN + cnt * 4));
	writel(0, GPIO_DIR(offset));

	return 0;
}

static int apollo_gpio_output(struct gpio_chip *chip,
					unsigned offset, int value)
{
	struct imapx_gpio_chip *igc = to_imapx_gpiochip(chip);
	uint8_t tmp;
	int cnt = 0;
	int num = 0;
	//pr_err("%s: in\n", __func__);
	
	cnt = offset / 8;
	num = offset % 8;

	writel(value, GPIO_WDAT(offset));

	tmp = readl(PAD_IO_GPIOEN + cnt * 4);
	tmp |= 1 << num;
	writel(tmp, (PAD_IO_GPIOEN + cnt * 4));
	writel(1, GPIO_DIR(offset));

	return 0;
}

static int apollo_gpio_get_value(struct gpio_chip *chip, unsigned offset)
{
	struct imapx_gpio_chip *igc = to_imapx_gpiochip(chip);
	uint8_t tmp;
	int cnt = 0;
	int num = 0;
	int value = 0;
	//pr_err("%s: in\n", __func__);
	
	cnt = offset / 8;
	num = offset % 8;

	tmp = readl(PAD_IO_GPIOEN + cnt * 4);
	if (tmp & (1 << num)) {
		tmp = readl(GPIO_DIR(offset));
		if (tmp)
			value = readl(GPIO_WDAT(offset));
		else
			value = readl(GPIO_RDAT(offset));
	}

	return value;
}

static void apollo_gpio_set_value(struct gpio_chip *chip,
					unsigned offset, int value)
{
	struct imapx_gpio_chip *igc = to_imapx_gpiochip(chip);
	//pr_err("%s: in, offset=%d\n", __func__, offset);

	writel(value, GPIO_WDAT(offset));
}

static int apollo_gpio_to_irq(struct gpio_chip *chip, unsigned offset)
{
	struct imapx_gpio_chip *igc = to_imapx_gpiochip(chip);

	return irq_find_mapping(irq_domain, offset);
}

static void apollo_gpio_irq_ack(struct irq_data *d)
{
	unsigned long gpio = d->hwirq;
	//pr_err("%s: gpio=%d\n", __func__, gpio);

	if ((d->irq > 15 && d->irq < 256) || gpio < 62 || gpio > 159)
		return;

	writel(0, GPIO_INTPEND(gpio));
}

static void apollo_gpio_irq_mask(struct irq_data *d)
{
	unsigned long gpio = d->hwirq;
	//pr_err("%s: gpio=%d\n", __func__, gpio);

	if ((d->irq > 15 && d->irq < 256) || gpio < 62 || gpio > 159)
		return;

	writel(1,GPIO_INTMSK(gpio));
	writel(1,GPIO_INTGMSK(gpio));
}

static void apollo_gpio_irq_unmask(struct irq_data *d)
{
	unsigned long gpio = d->hwirq;
	//pr_err("%s: gpio=%d\n", __func__, gpio);

	if ((d->irq > 15 && d->irq < 256) || gpio < 62 || gpio > 159)
		return;

	writel(0,GPIO_INTMSK(gpio));
	writel(0,GPIO_INTGMSK(gpio));
}

static int apollo_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	unsigned long gpio = d->hwirq;
	int trigger;
	//pr_err("%s: gpio=%d, d->irq=%d\n", __func__, gpio, d->irq);

	if ((d->irq > 15 && d->irq < 256) || gpio < 62 || gpio > 159)
		return -EINVAL;

	switch (type & IRQ_TYPE_SENSE_MASK) {
	case IRQ_TYPE_EDGE_RISING:
		trigger = 5;
		__irq_set_handler_locked(d->irq, handle_edge_irq);
		break;

	case IRQ_TYPE_EDGE_FALLING:
		trigger = 3;
		__irq_set_handler_locked(d->irq, handle_edge_irq);
		break;

	case IRQ_TYPE_EDGE_BOTH:
		trigger = 7;
		__irq_set_handler_locked(d->irq, handle_edge_irq);
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		trigger = 1;
		__irq_set_handler_locked(d->irq, handle_level_irq);
		break;

	case IRQ_TYPE_LEVEL_LOW:
		trigger = 0;
		__irq_set_handler_locked(d->irq, handle_level_irq);
		break;

	default:
		return -EINVAL;
	}

	writel(1, GPIO_INTMSK(gpio));
	writel(1, GPIO_INTGMSK(gpio));

	writel(trigger, GPIO_INTTYPE(gpio));

	writel(0xff, GPIO_FILTER(gpio));
	writel(0, GPIO_CLKDIV(gpio));

	writel(0, GPIO_INTPEND(gpio));
	writel(0, GPIO_INTMSK(gpio));
	writel(0, GPIO_INTGMSK(gpio));

	return 0;
}

static void apollo_gpio_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	struct imapx_gpio_bank *bank;
	int gpio, pin, type;
	unsigned long pending;
	int unmasked = 0;
	struct irq_chip *chip = irq_desc_get_chip(desc);
	//pr_err("%s: irq=%d\n", __func__, irq);

	chained_irq_enter(chip, desc);

	bank = irq_get_handler_data(irq);

	pending = readl(GPIO_INTPENDGLB(bank->bank));
	for_each_set_bit(pin, &pending, 16) {
		gpio = bank->bank * 16 + pin;
		writel(0, GPIO_INTPEND(gpio));

		type = readl(GPIO_INTTYPE(gpio));
		if (type != 0 && type != 1) {
			unmasked = 1;
			chained_irq_exit(chip, desc);
		}

		generic_handle_irq(gpio_to_irq(gpio));
	}

	if (!unmasked)
		chained_irq_exit(chip, desc);
}

struct gpio_chip apollo_gpio_chip = {
	.label	= "apollo-gpiochip",
	.ngpio	= 169,
	.direction_input = apollo_gpio_input,
	.direction_output = apollo_gpio_output,
	.get_direction = apollo_gpio_direction,
	.get = apollo_gpio_get_value,
	.set = apollo_gpio_set_value,
	.to_irq = apollo_gpio_to_irq,
};

struct irq_chip apollo_irq_chip = {
	.name = "apollo-irqchip",
	.irq_ack = apollo_gpio_irq_ack,
	.irq_mask = apollo_gpio_irq_mask,
	.irq_unmask = apollo_gpio_irq_unmask,
	.irq_set_type = apollo_gpio_irq_set_type,
};

#define BANK_INFO(_bank, _irq, _nr)	\
{					\
	.bank	= _bank,		\
	.irq	= _irq,			\
	.nr	= _nr,			\
}					

struct imapx_gpio_bank apollo_gpio_bank[] = {
	BANK_INFO(0, 64, 16),
	BANK_INFO(1, 65, 16),
	BANK_INFO(2, 66, 16),
	BANK_INFO(3, 67, 16),
	BANK_INFO(4, 68, 16),
	BANK_INFO(5, 69, 16),
	BANK_INFO(6, 74, 16),
	BANK_INFO(7, 75, 16),
	BANK_INFO(8, 76, 16),
	BANK_INFO(9, 77, 16),
	BANK_INFO(10, -1, 9),
};

struct imapx_gpio_chip imapx_gpio_chip[] = {
	[0] = {
		.compatible = "apollo",
		.chip = &apollo_gpio_chip,
		.irq_chip = &apollo_irq_chip,
		.bankcount = 11,
		.bank = apollo_gpio_bank,
	},
};

static struct imapx_gpio_chip *imapx_gpiochip_get(const char *src)
{
	int i;
	struct imapx_gpio_chip *igc = NULL;
	const char *dst;

	for (i = 0; i < ARRAY_SIZE(imapx_gpio_chip); i++) {
		dst = imapx_gpio_chip[i].compatible;
		if (!strcmp(src, dst)) {
			igc = &imapx_gpio_chip[i];
			break;
		}
	}

	return igc;
}

static struct lock_class_key gpio_lock_class;

static int apollo_gpiochip_probe(struct platform_device *pdev)
{
	struct imapx_gpio_chip *igc;
	struct resource *res;
	struct gpio_chip *chip;
	struct irq_chip *irq_chip;
	struct imapx_gpio_bank *bank;
	int gpio, i, ret;
	struct device *dev = &pdev->dev;
	struct gpio_key_cfg *pdata = pdev->dev.platform_data;
	struct clk *clk;
	pr_err("%s: in\n", __func__);

	igc = devm_kzalloc(dev, sizeof(struct imapx_gpio_chip), GFP_KERNEL);
	if (!igc) {
		dev_err(dev, "Memory alloc failed\n");
		return -ENOMEM;
	}

	igc = imapx_gpiochip_get(pdata->compatible);
	if (!igc) {
		dev_err(dev, "No compatible device\n");
		return -ENODEV;
	}
	chip = igc->chip;
	irq_chip = igc->irq_chip;
	bank = igc->bank;

	irq_domain = irq_domain_add_linear(pdev->dev.of_node,
						chip->ngpio,
						&irq_domain_simple_ops,
						NULL);
	clk = clk_get_sys("imap-gpio","imap-gpio");
	if (IS_ERR(clk)){
		dev_err(dev,"get gpio bus clk fail\n");
		ret = -ENODEV;
		return ret;
	}
	ret = clk_prepare_enable(clk);
	if(ret){
		dev_err(dev,"dev,prepare gpio bus fail\n");
		clk_disable_unprepare(clk);
		clk_put(clk);
		return -ENODEV;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(!res)) {
		dev_err(dev, "Invalid mem resource\n");
		return -ENODEV;
	}

	if (!devm_request_mem_region(dev, res->start,
					resource_size(res), pdev->name)) {
		dev_err(dev, "Region already claimed\n");
		return -EBUSY;
	}

	igc->base = devm_ioremap(dev, res->start, resource_size(res));
	if (!igc->base) {
		dev_err(dev, "Could not ioremap\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, igc);

	gpiochip_add(chip);

	for (gpio = 0; gpio < chip->ngpio; gpio++) {
		int irq = irq_create_mapping(irq_domain, gpio);
		irq_set_lockdep_class(irq, &gpio_lock_class);
		irq_set_chip_data(irq, chip);
		irq_set_chip_and_handler(irq, irq_chip, handle_simple_irq);
		set_irq_flags(irq, IRQF_VALID);
	}

	for (i = 0; i < igc->bankcount; i++) {
		if (bank[i].irq != -1) {
			irq_set_chained_handler(bank[i].irq, apollo_gpio_irq_handler);
			irq_set_handler_data(bank[i].irq, &bank[i]);
		}
	}

	gic_arch_extn.irq_ack = irq_chip->irq_ack;
	gic_arch_extn.irq_mask = irq_chip->irq_mask;
	gic_arch_extn.irq_unmask = irq_chip->irq_unmask;
	gic_arch_extn.irq_set_type = irq_chip->irq_set_type;

	pr_err("%s: done!\n", __func__);
	return 0;
}

int apollo_gpio_save(void)
{
	unsigned long flags;
	int i, j;

	local_irq_save(flags);
	for (i = 0; i < ARRAY_SIZE(apollo_gpio_bank); i++) {
		struct imapx_gpio_bank *bank = &apollo_gpio_bank[i];
		for (j = 0; j < bank->nr; j++) {
			int gpio = 16 * bank->bank + j;
			bank->int_msk[j] = readl(GPIO_INTMSK(gpio));
			bank->int_gmsk[j] = readl(GPIO_INTGMSK(gpio));
			bank->int_type[j] = readl(GPIO_INTTYPE(gpio));
			bank->flt[j] = readl(GPIO_FILTER(gpio));
			bank->clkdiv[j] = readl(GPIO_CLKDIV(gpio));
		}
	}
	local_irq_restore(flags);
	return 0;
}
EXPORT_SYMBOL(apollo_gpio_save);

int apollo_gpio_restore(void)
{
	unsigned long flags;
	int i, j;

	local_irq_save(flags);
	for (i = 0; i < ARRAY_SIZE(apollo_gpio_bank); i++) {
		struct imapx_gpio_bank *bank = &apollo_gpio_bank[i];
		for (j = 0; j < bank->nr; j++) {
			int gpio = 16 * bank->bank + j;
			writel(1, GPIO_INTMSK(gpio));
			writel(1, GPIO_INTGMSK(gpio));
			writel(bank->int_type[j], GPIO_INTTYPE(gpio));
			writel(bank->flt[j], GPIO_FILTER(gpio));
			writel(bank->clkdiv[j], GPIO_CLKDIV(gpio));
			writel(0, GPIO_INTPEND(gpio));
			writel(bank->int_msk[j], GPIO_INTMSK(gpio));
			writel(bank->int_gmsk[j], GPIO_INTGMSK(gpio));
		}
	}
	local_irq_restore(flags);
	return 0;
}
EXPORT_SYMBOL(apollo_gpio_restore);

static struct platform_driver apollo_gpiochip_driver = {
	.probe	= apollo_gpiochip_probe,
	.driver	= {
		.name = "imap-gpiochip",
		.owner = THIS_MODULE,
	},
};

static int __init apollo_gpiochip_init(void)
{
	return platform_driver_register(&apollo_gpiochip_driver);
}
arch_initcall(apollo_gpiochip_init);
