#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <mach/imap-iomap.h>
#include <mach/pad.h>
#include <mach/pad_info.h>
#include <mach/items.h>
#include <mach/power-gate.h>
#include <mach/rballoc.h>
#include <mach/io.h>
#include "core.h"


void __iomem *gpio_base;

void __init apollo_init_gpio(void)
{
	gpio_base = ioremap_nocache(IMAP_GPIO_BASE, 0x4400);
	module_power_on(SYSMGR_GPIO_BASE);
	pr_err("%s: ngpio = %d\n", __func__, ngpio);
}

static int index_analysis(uint32_t index, uint16_t *h, uint16_t *l)
{
	int minus;
//	uint16_t high;
//	uint16_t low;

//	pi->high = ITEMS_EINT;
//	pi->low = ITEMS_EINT;

	minus = index & 0x80000000;
//	high = (index >> 16) & 0xFFFF;
//	low = index & 0xFFFF;
	*h = (index >> 16) & 0xffff;
	*l = index & 0xffff;

	if (minus) {
		if (*h == 0xffff) {
			*h = 0;
			*l = ~(*l) + 1;
		} else {
			*h &= 0xff;
			*l &= 0xff;
		}
		return -1;
	} else {
		*h &= 0xff;
		*l &= 0xff;
		return 0;
	}
}

static int mode_type(const char *mode)
{
	if (!strcmp(mode, "function"))
		return MODE_FUNCTION;
	else if (!strcmp(mode, "input"))
		return MODE_INPUT;
	else if (!strcmp(mode, "output"))
		return MODE_OUTPUT;
	else
		return -EINVAL;
}

static int pull_type(const char *pull)
{
	if (!strcmp(pull, "float"))
		return PULL_FLOAT;
	else if (!strcmp(pull, "up"))
		return PULL_UP;
	else if (!strcmp(pull, "down"))
		return PULL_DOWN;
	else
		return 0;
}

static int eint_type(const char *trigger)
{
	if (!strcmp(trigger, "low"))
		return EINT_TRIGGER_LOW;
	else if (!strcmp(trigger, "high"))
		return EINT_TRIGGER_HIGH;
	else if (!strcmp(trigger, "fall"))
		return EINT_TRIGGER_FALLING;
	else if (!strcmp(trigger, "rise"))
		return EINT_TRIGGER_RISING;
	else if (!strcmp(trigger, "double"))
		return EINT_TRIGGER_DOUBLE;
	else
		return -EINVAL;
}

static int mode_set(uint16_t index, int md)
{
	uint8_t tmp;
	int cnt = 0;
	int num = 0;

	cnt = index / 8;
	num = index % 8;

	switch (md) {
	case MODE_FUNCTION:
		tmp = readl(PAD_IO_GPIOEN + cnt * 4);
		tmp &= ~(1 << num);
		writel(tmp, (PAD_IO_GPIOEN + cnt * 4));
		break;

	case MODE_INPUT:
		tmp = readl(PAD_IO_GPIOEN + cnt * 4);
		tmp |= 1 << num;
		writel(tmp, (PAD_IO_GPIOEN + cnt * 4));
		writel(0, GPIO_DIR(index));
		break;

	case MODE_OUTPUT:
		tmp = readl(PAD_IO_GPIOEN + cnt * 4);
		tmp |= 1 << num;
		writel(tmp, (PAD_IO_GPIOEN + cnt * 4));
		writel(1, GPIO_DIR(index));
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int mode_get(uint16_t index)
{
	uint8_t tmp;
	int cnt = 0;
	int num = 0;

	cnt = index / 8;
	num = index % 8;

	tmp = readl(PAD_IO_GPIOEN + cnt * 4);
	if (tmp & (1 << num)) {
		tmp = readl(GPIO_DIR(index));
		if (tmp)
			return MODE_OUTPUT;
		else
			return MODE_INPUT;
	} else {
		return MODE_FUNCTION;
	}
}

static int pull_set(uint16_t index, int pl) {
	/*
	 * pl  = 0 means disable pull  
	 * pl  = 1 means enable pull
	 */
	writel((pl & 0x1), GPIO_PULLEN(index));	
	return 0;
}

static int pull_get(uint16_t index) {
	/*
 	 * coronampw has fixed pull mode, software
	   can only enable/disable it 
 	 */
	return readl(GPIO_PULLEN(index)) & 0x1;
}

static int level_set(uint16_t index, int level)
{
	writel(level, GPIO_WDAT(index));

	return 0;
}

static int level_get(uint16_t index)
{
	uint8_t tmp;
	int cnt = 0;
	int num = 0;
	int level = 0;

	cnt = index / 8;
	num = index % 8;

	tmp = readl(PAD_IO_GPIOEN + cnt * 4);
	if (tmp & (1 << num)) {
		tmp = readl(GPIO_DIR(index));
		if (tmp)
			level = readl(GPIO_WDAT(index));
		else
			level = readl(GPIO_RDAT(index));
	}

	return level;
}

int imapx_pad_set_mode(uint32_t index, const char *mode)
{
	int md;
	int ret = 0;
	int i;
	uint16_t high, low;

	md = mode_type(mode);
	if (md < 0)
		return -EINVAL;

	index_analysis(index, &high, &low);
	if (high > ngpio || low > ngpio)
		return -EINVAL;

	if (high >= low) {
		for (i = low; i <= high; i++)
			ret |= mode_set(i, md);
	} else {
		ret |= mode_set(low, md);
	}

	return ret;
}
EXPORT_SYMBOL(imapx_pad_set_mode);

uint32_t imapx_pad_get_mode(uint32_t index)
{
	int md;
	int i;
	uint32_t value = 0;
	uint16_t high, low;

	index_analysis(index, &high, &low);
	if (high > ngpio || low > ngpio)
		goto done;

	if (high >= low) {
		for (i = low; i < high; i++) {
			if (i > low + 15)
				break;

			md = mode_get(i);
			value |= md << ((i - low) * 2);
		}
	} else {
		md = mode_get(low);
		value = md;
	}

done:
	return value;
}
EXPORT_SYMBOL(imapx_pad_get_mode);

int imapx_pad_set_pull(uint32_t index, const char *pull)
{
	int pl;
	int ret = 0;
	int i;
	uint16_t high, low;

	pl = pull_type(pull);
	if (pl < 0)
		return -EINVAL;

	index_analysis(index, &high, &low);
	if (high > ngpio || low > ngpio)
		return -EINVAL;

	if (high >= low) {
		for (i = low; i <= high; i++)
			ret |= pull_set(i, pl);
	} else {
		ret = pull_set(low, pl);
	}

	return ret;
}
EXPORT_SYMBOL(imapx_pad_set_pull);

uint32_t imapx_pad_get_pull(uint32_t index)
{
	uint32_t value = 0;
	int pl;
	int i;
	uint16_t high, low;

	index_analysis(index, &high, &low);
	if (high > ngpio || low > ngpio)
		goto done;

	if (high >= low) {
		for (i = low; i < high; i++) {
			if (i > low + 15)
				break;

			pl = pull_get(i);
			value |= pl << ((i - low) * 2);
		}
	} else {
		pl = pull_get(low);
		value = pl;
	}

done:
	return value;
}
EXPORT_SYMBOL(imapx_pad_get_pull);

int imapx_pad_set_value(uint32_t index, int level)
{
	int ret = 0;
	int minus;
	int i;
	uint16_t high, low;

	minus = index_analysis(index, &high, &low);
	if (high > ngpio || low > ngpio)
		return -EINVAL;

	if (minus)
		level = !level;

	if (high >= low) {
		for (i = low; i <= high; i++)
			ret |= level_set(i, level);
	} else {
		ret = level_set(low, level);
	}

	return ret;
}
EXPORT_SYMBOL(imapx_pad_set_value);

uint32_t imapx_pad_get_value(uint32_t index)
{
	uint32_t value = 0;
	int minus;
	int level;
	int i;
	uint16_t high, low;

	minus = index_analysis(index, &high, &low);
	if (high > ngpio || low > ngpio)
		goto done;

	if (high >= low) {
		for (i = low; i <= high; i++) {
			if (i > low + 31)
				break;

			level = level_get(i);
			if (minus)
				level = !level;
			value |= level << (i - low);
		}
	} else {
		level = level_get(low);
		if (minus)
			level = !level;
		value = level;
	}

done:
	return value;
}
EXPORT_SYMBOL(imapx_pad_get_value);

int imapx_pad_to_irq(uint32_t index)
{
	int bank;
	int irq = -1;
	uint16_t high, low;

	index_analysis(index, &high, &low);

	bank = low / 16;
	switch (bank) {
//	case 0:
//	case 1:
//	case 2:
	case 3:
	case 4:
	case 5:
		irq = bank + 64;
		break;

	case 6:
	case 7:
	case 8:
	case 9:
		irq = bank + 68;
		break;

	default:
		goto done;
	}

done:
	return irq;
}
EXPORT_SYMBOL(imapx_pad_to_irq);

int imapx_pad_irq_config(uint32_t index, const char *trigger, uint16_t filter)
{
	int type;
	uint16_t high, low;

	type = eint_type(trigger);
	if (type < 0)
		return -EINVAL;

	index_analysis(index, &high, &low);

	writel(1, GPIO_INTMSK(low));
	writel(1, GPIO_INTGMSK(low));

	writel(type, GPIO_INTTYPE(low));

	writel(filter & 0xff, GPIO_FILTER(low));
	writel((filter >> 8) & 0xff, GPIO_CLKDIV(low));

	writel(0, GPIO_INTPEND(low));
	writel(0, GPIO_INTMSK(low));
	writel(0, GPIO_INTGMSK(low));

	return 0;
}
EXPORT_SYMBOL(imapx_pad_irq_config);

int imapx_pad_irq_pending(uint32_t index)
{
	uint8_t value = 0;
	uint16_t high, low;

	index_analysis(index, &high, &low);

	value = readl(GPIO_INTPEND(low));

	return value;
}
EXPORT_SYMBOL(imapx_pad_irq_pending);

int imapx_pad_irq_clear(uint32_t index)
{
	uint16_t high, low;

	index_analysis(index, &high, &low);

	writel(0, GPIO_INTPEND(low));

	return 0;
}
EXPORT_SYMBOL(imapx_pad_irq_clear);

int imapx_pad_irq_mask(uint32_t index, int mask)
{
	uint16_t high, low;

	index_analysis(index, &high, &low);

	if (mask) {
		writel(1,GPIO_INTMSK(low));
		writel(1,GPIO_INTGMSK(low));
	} else {
		writel(0,GPIO_INTMSK(low));
		writel(0,GPIO_INTGMSK(low));
	}

	return 0;
}
EXPORT_SYMBOL(imapx_pad_irq_mask);




static int imapx_module_set_iomux(const char *module) {
	int i = 0;
	uint8_t tmp;
	struct module_mux_info *muxinfo = imapx_module_mux_info;
	for(i = 0 ; i < ARRAY_SIZE(imapx_module_mux_info); i++) {
		if(!strcmp(module, muxinfo[i].name)) {
			tmp =  readb(PAD_IOMUX_MODE);
			if(muxinfo[i].muxval)
				tmp |= (1  << muxinfo[i].muxbit);
			else
				tmp &= ~(1 << muxinfo[i].muxbit);
			pr_info("[%s] set iomux %x\n", muxinfo[i].name, tmp);
			writeb(tmp, PAD_IOMUX_MODE);
			return 0;
		}
	}

	return -1;
}

static int imapx_module_set_mode(const char *module) {
	const char *mode;
	const char *pull;
	int level;
	uint32_t index;
	int i,j;
	struct module_pads_info *moduleinfo = imapx_module_pads_info;
	struct pad_init_info *padsinfo = NULL;

	for(i = 0 ; i < ARRAY_SIZE(imapx_module_pads_info); i++) {
		if(!strcmp(module, moduleinfo[i].name)) {
			padsinfo = moduleinfo[i].info;
			for(j = 0 ; j < moduleinfo[i].ngpio; j++) {
				mode = padsinfo[j].mode;
				pull = padsinfo[j].pull;
				level = padsinfo[j].level;
				index = padsinfo[j].index;
				pr_info("[%s] set gpio%d to %s\n", moduleinfo[i].name, index, mode);
				if (!strcmp(mode, "output"))
					imapx_pad_set_value(index, level);
				imapx_pad_set_mode(index, mode);
				imapx_pad_set_pull(index, pull);
			}
			return 0;
		}
	}
	if(i == ARRAY_SIZE(imapx_module_pads_info))
		return -1;
	return 0;
}

int imapx_pad_init(const char *module)
{
	if(imapx_module_set_mode(module)) {
		pr_err("Module [%s] init failed\n", module);
		return -1;
	}
	imapx_module_set_iomux(module);
	return 0;
}
EXPORT_SYMBOL(imapx_pad_init);
