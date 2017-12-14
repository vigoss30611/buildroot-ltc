#ifndef __PAD_NEW_H__
#define __PAD_NEW_H__

#define PAD_BASE		(IO_ADDRESS(SYSMGR_PAD_BASE))
#define PADS_SDIO_IN_EN		(PAD_BASE + 0x08)
#define PADS_SDIO_PULL		(PAD_BASE + 0x0c)
#define PAD_IO_PULLEN		(PAD_BASE + 0x14)
#define PAD_IO_GPIOEN		(PAD_BASE + 0x64)
#define PAD_IO_PULLUP		(PAD_BASE + 0x1A4)

extern void __iomem *gpio_base;

#define GPIO_RDAT(n)		(gpio_base + (n)*(0x40) + 0x0)
#define GPIO_WDAT(n)		(gpio_base + (n)*(0x40) + 0x4)
#define GPIO_DIR(n)		(gpio_base + (n)*(0x40) + 0x8)
#define GPIO_INTMSK(n)		(gpio_base + (n)*(0x40) + 0xC)
#define GPIO_INTGMSK(n)		(gpio_base + (n)*(0x40) + 0x10)
#define GPIO_INTPEND(n)		(gpio_base + (n)*(0x40) + 0x14)
#define GPIO_INTTYPE(n)		(gpio_base + (n)*(0x40) + 0x18)
#define GPIO_FILTER(n)		(gpio_base + (n)*(0x40) + 0x1C)
#define GPIO_CLKDIV(n)		(gpio_base + (n)*(0x40) + 0x20)
#define GPIO_INTPENDGLB(n)	(gpio_base + (n)*4 + 0x4000)

#define MODE_FUNCTION		(0)
#define MODE_INPUT		(1)
#define MODE_OUTPUT		(3)
#define PULL_FLOAT		(0)
#define PULL_DOWN		(1)
#define PULL_UP			(3)
#define EINT_TRIGGER_LOW	(0)
#define EINT_TRIGGER_HIGH	(1)
#define EINT_TRIGGER_FALLING	(3)
#define EINT_TRIGGER_RISING	(5)
#define EINT_TRIGGER_DOUBLE	(7)

#define PADSRANGE(a, b)	\
	((abs(b) << 16) | abs(a) | (a < 0 && b < 0 && a < b) ? 0x80000000 : 0)

#define PAD_INIT(_name, _owner, _index, _bank, _mode, _pull, _level)	\
	{								\
		.name	= _name,					\
		.owner	= _owner,					\
		.index	= _index,					\
		.bank	= _bank,					\
		.mode	= _mode,					\
		.pull	= _pull,					\
		.level	= _level,					\
	}

struct pad_index {
	uint16_t high;
	uint16_t low;
};

struct pad_init_info {
	const char name[16];
	const char owner[16];
	const char mode[16];
	const char pull[16];
	int index;
	int bank;
	int level;
};

struct imapx_pad_init {
	const char soc[64];
	int mode;
	int ngpio;
	struct pad_init_info *info;
};

void __init imapx15_init_gpio(void);
int imapx_pad_set_mode(uint32_t index, const char *mode);
uint32_t imapx_pad_get_mode(uint32_t index);
int imapx_pad_set_pull(uint32_t index, const char *pull);
uint32_t imapx_pad_get_pull(uint32_t index);
int imapx_pad_set_value(uint32_t index, int level);
uint32_t imapx_pad_get_value(uint32_t index);
int imapx_pad_to_irq(uint32_t index);
int imapx_pad_irq_config(uint32_t index, const char *trigger, uint16_t filter);
int imapx_pad_irq_pending(uint32_t index);
int imapx_pad_irq_clear(uint32_t index);
int imapx_pad_irq_mask(uint32_t index, int mask);
int imapx_pad_init(const char *module);

#endif
