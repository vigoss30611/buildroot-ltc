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

static int ngpio = -1;
static struct pad_init_info *info = NULL;
static struct pad_index *pi = NULL;

void __iomem *gpio_base;
void __init imapx15_init_gpio(void)
{
	int i;
	int mode;
	char soc[ITEM_MAX_LEN];
	int ubootLogoState = 1;

	gpio_base = ioremap_nocache(IMAP_GPIO_BASE, 0x4400);
	if(item_exist("config.uboot.logo")){
		unsigned char str[ITEM_MAX_LEN] = {0};
		item_string(str,"config.uboot.logo",0);
		if(strncmp(str,"0",1) == 0 )
			ubootLogoState = 0;//disabled
	}

	if (!ubootLogoState)
		module_power_on(SYSMGR_GPIO_BASE);

	pi = kzalloc(sizeof(struct pad_index), GFP_KERNEL);
	if (!pi) {
		pr_err("%s: Alloc struct pad_index fail!\n", __func__);
		return;
	}

	info = kzalloc(sizeof(struct pad_init_info), GFP_KERNEL);
	if (!info) {
		pr_err("%s: Alloc struct pad_init_info fail!\n", __func__);
		kfree(pi);
		return;
	}

	if (item_exist("board.cpu"))
		item_string(soc, "board.cpu", 0);
	else
		strcpy(soc, "x15");

	mode = readl(PAD_BASE);

	for (i = 0; i < ARRAY_SIZE(imapx_pad_init_info); i++) {
		if (!strcmp(soc, imapx_pad_init_info[i].soc)) {
			info = imapx_pad_init_info[i].info;
			ngpio = imapx_pad_init_info[i].ngpio;
			break;
		}
	}

	pr_err("%s: ngpio = %d\n", __func__, ngpio);
}

static int index_analysis(uint32_t index, struct pad_index *pi)
{
	int minus;
	uint16_t high;
	uint16_t low;

	pi->high = ITEMS_EINT;
	pi->low = ITEMS_EINT;

	minus = index & 0x80000000;
	high = (index >> 16) & 0xFFFF;
	low = index & 0xFFFF;

	if (minus) {
		if (high == 0xFFFF) {
			pi->high = 0;
			pi->low = ~low + 1;
		} else {
			pi->high = high & 0xFFF;
			pi->low = low & 0xFFF;
		}
		return -1;
	} else {
		pi->high = high & 0xFFF;
		pi->low = low & 0xFFF;
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
		return -EINVAL;
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

static int pull_set(uint16_t index, int pl)
{
	uint8_t tmp;
	int cnt = 0;
	int num = 0;

	cnt = index / 8;
	num = index % 8;

	switch (pl) {
	case PULL_FLOAT:
		tmp = readl(PAD_IO_PULLEN + cnt * 4);
		tmp &= ~(1 << num);
		writel(tmp, (PAD_IO_PULLEN + cnt * 4));
		break;

	case PULL_UP:
		tmp = readl(PAD_IO_PULLEN + cnt * 4);
		tmp |= 1 << num;
		writel(tmp, (PAD_IO_PULLEN + cnt * 4));
		tmp = readl(PAD_IO_PULLUP + cnt * 4);
		tmp |= 1 << num;
		writel(tmp, (PAD_IO_PULLUP + cnt * 4));
		break;

	case PULL_DOWN:
		tmp = readl(PAD_IO_PULLEN + cnt * 4);
		tmp |= 1 << num;
		writel(tmp, (PAD_IO_PULLEN + cnt * 4));
		tmp = readl(PAD_IO_PULLUP + cnt * 4);
		tmp &= ~(1 << num);
		writel(tmp, (PAD_IO_PULLUP + cnt * 4));
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int pull_get(uint16_t index)
{
	uint8_t tmp;
	int cnt = 0;
	int num = 0;

	cnt = index / 8;
	num = index % 8;

	tmp = readl(PAD_IO_PULLEN + cnt * 4);
	if (tmp & (1 << num)) {
		tmp = readl(PAD_IO_PULLUP + cnt * 4);
		if (tmp & (1 << num))
			return PULL_UP;
		else
			return PULL_DOWN;
	} else {
		return PULL_FLOAT;
	}
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

	md = mode_type(mode);
	if (md < 0)
		return -EINVAL;

	index_analysis(index, pi);
	if (pi->high > ngpio || pi->low > ngpio)
		return -EINVAL;

	if (pi->high >= pi->low) {
		for (i = pi->low; i <= pi->high; i++)
			ret |= mode_set(i, md);
	} else {
		ret |= mode_set(pi->low, md);
	}

	printk("set gpio%d  to %s \n", index, mode);
	return ret;
}
EXPORT_SYMBOL(imapx_pad_set_mode);

uint32_t imapx_pad_get_mode(uint32_t index)
{
	int md;
	int i;
	uint32_t value = 0;

	index_analysis(index, pi);
	if (pi->high > ngpio || pi->low > ngpio)
		goto done;

	if (pi->high >= pi->low) {
		for (i = pi->low; i < pi->high; i++) {
			if (i > pi->low + 15)
				break;

			md = mode_get(i);
			value |= md << ((i - pi->low) * 2);
		}
	} else {
		md = mode_get(pi->low);
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

	pl = pull_type(pull);
	if (pl < 0)
		return -EINVAL;

	index_analysis(index, pi);
	if (pi->high > ngpio || pi->low > ngpio)
		return -EINVAL;

	if (pi->high >= pi->low) {
		for (i = pi->low; i <= pi->high; i++)
			ret |= pull_set(i, pl);
	} else {
		ret = pull_set(pi->low, pl);
	}

	return ret;
}
EXPORT_SYMBOL(imapx_pad_set_pull);

uint32_t imapx_pad_get_pull(uint32_t index)
{
	uint32_t value = 0;
	int pl;
	int i;

	index_analysis(index, pi);
	if (pi->high > ngpio || pi->low > ngpio)
		goto done;

	if (pi->high >= pi->low) {
		for (i = pi->low; i < pi->high; i++) {
			if (i > pi->low + 15)
				break;

			pl = pull_get(i);
			value |= pl << ((i - pi->low) * 2);
		}
	} else {
		pl = pull_get(pi->low);
		value = pl;
	}

done:
	return value;
}
EXPORT_SYMBOL(imapx_pad_get_pull);

int imapx_pad_set_value(uint32_t index, int level)
{
	int ret;
	int minus;
	int i;

	minus = index_analysis(index, pi);
	if (pi->high > ngpio || pi->low > ngpio)
		return -EINVAL;

	if (minus)
		level = !level;

	if (pi->high >= pi->low) {
		for (i = pi->low; i <= pi->high; i++)
			ret |= level_set(i, level);
	} else {
		ret = level_set(pi->low, level);
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

	minus = index_analysis(index, pi);
	if (pi->high > ngpio || pi->low > ngpio)
		goto done;

	if (pi->high >= pi->low) {
		for (i = pi->low; i <= pi->high; i++) {
			if (i > pi->low + 31)
				break;

			level = level_get(i);
			if (minus)
				level = !level;
			value |= level << (i - pi->low);
		}
	} else {
		level = level_get(pi->low);
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
	int irq = 0;

	index_analysis(index, pi);
	if (pi->low > ngpio)
		goto done;

	bank = pi->low / 16;
	switch (bank) {
	case 0:
	case 1:
	case 2:
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

	type = eint_type(trigger);
	if (type < 0)
		return -EINVAL;

	index_analysis(index, pi);
	if (pi->low > ngpio)
		return -EINVAL;

	writel(1, GPIO_INTMSK(pi->low));
	writel(1, GPIO_INTGMSK(pi->low));

	writel(type, GPIO_INTTYPE(pi->low));

	writel(filter & 0xff, GPIO_FILTER(pi->low));
	writel((filter >> 8) & 0xff, GPIO_CLKDIV(pi->low));

	writel(0, GPIO_INTPEND(pi->low));
	writel(0, GPIO_INTMSK(pi->low));
	writel(0, GPIO_INTGMSK(pi->low));

	return 0;
}
EXPORT_SYMBOL(imapx_pad_irq_config);

int imapx_pad_irq_pending(uint32_t index)
{
	uint8_t value = 0;

	index_analysis(index, pi);
	if (pi->low > ngpio)
		goto done;

	value = readl(GPIO_INTPEND(pi->low));

done:
	return value;
}
EXPORT_SYMBOL(imapx_pad_irq_pending);

int imapx_pad_irq_clear(uint32_t index)
{
	index_analysis(index, pi);
	if (pi->low > ngpio)
		return -EINVAL;

	writel(0, GPIO_INTPEND(pi->low));

	return 0;
}
EXPORT_SYMBOL(imapx_pad_irq_clear);

int imapx_pad_irq_mask(uint32_t index, int mask)
{
	index_analysis(index, pi);
	if (pi->low > ngpio)
		return -EINVAL;

	if (mask) {
		writel(1,GPIO_INTMSK(pi->low));
		writel(1,GPIO_INTGMSK(pi->low));
	} else {
		writel(0,GPIO_INTMSK(pi->low));
		writel(0,GPIO_INTGMSK(pi->low));
	}

	return 0;
}
EXPORT_SYMBOL(imapx_pad_irq_mask);

int imapx_pad_init(const char *module)
{
	int i;
	const char *owner;
	const char *mode;
	const char *pull;
	int level;
	uint32_t index;

	if (pi && info && ngpio >= 0)
		goto __init_start;

	if (pi)
		kfree(pi);

	if (info)
		kfree(info);

	return -1;

__init_start:
	for (i = 0; i < ngpio; i++) {
		owner = info[i].owner;
		if (!strcmp(owner, module)) {
			mode = info[i].mode;
			pull = info[i].pull;
			level = info[i].level;
			index = info[i].index;

			if (!strcmp(mode, "output"))
				imapx_pad_set_value(index, level);
			imapx_pad_set_mode(index, mode);
			imapx_pad_set_pull(index, pull);
		}
	}

	return 0;
}
EXPORT_SYMBOL(imapx_pad_init);
