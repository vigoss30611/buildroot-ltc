/* **************************************************
 * **  arch/arm/mach-imapx200/pwm.c
 * **
 * **
 * ** Copyright (c) 2009~2014 ShangHai Infotm .Ltd all rights reserved. 
 * **
 * ** Use of infoTM's code  is governed by terms and conditions 
 * ** stated in the accompanying licensing statment.
 * **   
 * ** Description: PWM TIMER driver for imapx200 SOC
 * **
 * ** Author:
 * **
 * **   Haixu Fu       <haixu_fu@infotm.com>
 * ** Revision History:
 * ** ----------------
 * **  1.1  03/06/2010   Haixu Fu 
 * **  1.2  07/17/2014   David Sun (add pwm regulator)
 * **************************************************/

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include <mach/irqs.h>
#include <linux/io.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
//#include <linux/sysdev.h>

#include <mach/imap-pwm.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/pad.h>

#include <mach/items.h>
#include <linux/cpufreq.h>
#include <linux/regulator/machine.h>

#define	IMAP_PWM_CHANNEL  5

#define PWM_DEBUG 0

/* debug code */
#define pwm_dbg(debug_level, msg...) do\
{\
    if (debug_level > 0)\
    {\
        printk(msg);\
    }\
} while (0)

typedef struct imap_pwm_chan_s imap_pwm_chan_t;
static uint32_t save_tcfg0, save_tcfg1, save_tcntb2, save_tcmpb2, save_tcon,
    save_tcntb0, save_tcmpb0;

/* struct imap_pwm_chan_s */

struct imap_pwm_chan_s {
	void __iomem *base;

	unsigned char channel;
	int irq;

	//  struct sys_device       sysdev;

};

struct pwm_subdev_info {
	int id;
	const char *name;
	void *platform_data;
};

static struct resource imap_pwm_resource[1] = {
	[0] = {
	       .start = IMAP_PWM_BASE,
	       .end = IMAP_PWM_BASE + IMAP_PWM_SIZE - 1,
	       .flags = IORESOURCE_MEM,
	       }
};

imap_pwm_chan_t imap_chans[IMAP_PWM_CHANNEL];

void __iomem *ioaddr;
struct clk *apb_clk;
struct notifier_block freq_transition;
long grate;
long gtcnt, gtcmp;
int gintensity = -1;
int gstart;
int glow_intensity;
int ghigh_intensity;
int intensity_range;
int gstep;
int gcur_tcnt, gcur_tcmp;
int gpwmchn = 0;

spinlock_t infotm_pwm_lock;

int pwm_change_width(int channel, int intensity);

#ifdef CONFIG_PM
int imap_pwm_suspend(struct platform_device *dev, pm_message_t pm)
{
	printk("++ %s ++\n", __func__);
	/* Save the registers */
	save_tcfg0 = readl(ioaddr + IMAP_TCFG0);
	save_tcfg1 = readl(ioaddr + IMAP_TCFG1);
	save_tcon = readl(ioaddr + IMAP_TCON);
	save_tcntb2 = readl(ioaddr + IMAP_TCNTB2);
	save_tcmpb2 = readl(ioaddr + IMAP_TCMPB2);
	save_tcntb0 = readl(ioaddr + IMAP_TCNTB0);
	save_tcmpb0 = readl(ioaddr + IMAP_TCMPB0);

	return 0;
}

#define PAD_SYSM_VA1  (IO_ADDRESS(IMAP_SYSMGR_BASE + 0x9000))
extern int imap_timer_setup(int channel, unsigned long g_tcnt,
			    unsigned long gtcmp);
extern int imap_back_poweron(void);
int gintensity_init = -1;
int imap_back_poweron(void)
{
	int tmp;

	if (gintensity == -1) {
		gintensity = item_integer("bl.def_intensity", 0);
		if (gintensity > 255) {
			gintensity = 255;
		}
	}

	printk("++ %s ++ gintensity = %d\n", __func__, gintensity);

	tmp = (gintensity - glow_intensity) * 0xff / intensity_range + 1;
	pwm_change_width(item_integer("bl.ctrl", 1), tmp);
	if (gintensity_init == -1) {
		gintensity_init++;
	}

	mdelay(20);

	return 0;

}

EXPORT_SYMBOL(imap_back_poweron);
int imap_pwm_resume(struct platform_device *dev)
{
	module_power_on(SYSMGR_PWM_BASE);
	imapx_pad_init("pwm0");

	/* Restore the registers */
	writel(save_tcfg0, ioaddr + IMAP_TCFG0);
	writel(save_tcfg1, ioaddr + IMAP_TCFG1);
	writel(save_tcntb2, ioaddr + IMAP_TCNTB2);
	writel(save_tcmpb2, ioaddr + IMAP_TCMPB2);
	writel(save_tcntb0, ioaddr + IMAP_TCNTB0);
	writel(0x0, ioaddr + IMAP_TCMPB0);
	writel(save_tcon | 0xb, ioaddr + IMAP_TCON);

	return 0;
}
#else
#define imap_pwm_suspend   NULL
#define	imap_pwm_resume    NULL
#endif

void pwm_writel(void __iomem * base, int offset, int value)
{
	__raw_writel(value, base + offset);
}

u32 pwm_readl(void __iomem * base, int offset)
{
	return __raw_readl(base + offset);
}

/* imap PWM initialisation */

int imap_pwm_start(int chan)
{
	unsigned long tcon;

	tcon = pwm_readl(ioaddr, IMAP_TCON);

	switch (chan) {
	case 0:
		tcon |= IMAP_TCON_T0START;
		tcon &= ~IMAP_TCON_T0MU_ON;
		break;
	case 1:
		tcon |= IMAP_TCON_T1START;
		tcon &= ~IMAP_TCON_T1MU_ON;
		break;
	case 2:
		tcon |= IMAP_TCON_T2START;
		tcon &= ~IMAP_TCON_T2MU_ON;
		break;
	case 3:
		tcon |= IMAP_TCON_T3START;
		tcon &= ~IMAP_TCON_T3MU_ON;
		break;
	case 4:
		tcon |= IMAP_TCON_T4START;
		tcon &= ~IMAP_TCON_T4MU_ON;
	}

	pwm_writel(ioaddr, IMAP_TCON, tcon);
	return 0;
}

int imap_timer_setup(int channel, unsigned long g_tcnt, unsigned long g_tcmp)
{
	unsigned long tcon;

	//printk("++ %s ++\n", __func__);
	tcon = pwm_readl(ioaddr, IMAP_TCON);

	switch (channel) {
	case 3:
	case 4:
		printk(KERN_INFO "Only Timer 2 supported right now\n.");
		return -1;
	case 1:
		tcon &= ~(7 << 8);
		tcon |= IMAP_TCON_T1RL_ON;
		break;
	case 0:
		tcon &= ~(7);
		tcon |= IMAP_TCON_T0RL_ON;
		break;
	case 2:
		tcon &= ~(7 << 12);
		tcon |= IMAP_TCON_T2RL_ON;
		break;
	default:
		printk(KERN_ERR "segment invalid!\n");
		break;
	}

	pwm_writel(ioaddr, IMAP_TCON, tcon);
	pwm_writel(ioaddr, IMAP_TCNTB(channel), g_tcnt);
	pwm_writel(ioaddr, IMAP_TCMPB(channel), g_tcmp);
	switch (channel) {
	case 0:
		tcon |= IMAP_TCON_T0MU_ON;
		break;
	case 1:
		tcon |= IMAP_TCON_T1MU_ON;
		break;
	case 2:
		tcon |= IMAP_TCON_T2MU_ON;
		break;
	case 3:
		tcon |= IMAP_TCON_T3MU_ON;
		break;
	case 4:
		tcon |= IMAP_TCON_T4MU_ON;
		break;
	default:
		printk(KERN_ERR "segment invalid\n");
		break;
	}

	pwm_writel(ioaddr, IMAP_TCON, tcon);

	imap_pwm_start(channel);

	//printk("rate %ld, %ld\n", g_tcnt, g_tcmp);        
	return 0;
}

EXPORT_SYMBOL(imap_timer_setup);

static int imap_pwm_div(void)
{
	uint32_t tmp, i = 0, j = 0;

	i |= (0x8);
	tmp = readl(ioaddr + IMAP_TCFG1);
	tmp &= ~0xffff;
	tmp |= ((i << 0) | (i << 4) | (i << 8) | (i << 12));
	writel(tmp, ioaddr + IMAP_TCFG1);

	save_tcfg1 = tmp;

	tmp = readl(ioaddr + IMAP_TCFG0);
	tmp &= ~(0xffff);
	tmp |= (j | (j << 8));
	writel(tmp, ioaddr + IMAP_TCFG0);

	save_tcfg0 = tmp;

	return 0;
}

static struct regulator_consumer_supply pwm0_supply[] = {
	REGULATOR_SUPPLY("pwm0", NULL),
};

static struct regulator_consumer_supply pwm1_supply[] = {
	REGULATOR_SUPPLY("pwm1", NULL),
};

static struct regulator_consumer_supply pwm2_supply[] = {
	REGULATOR_SUPPLY("pwm2", NULL),
};

static struct regulator_init_data pwm0_init_pdata = {
	.constraints = {
		.name = "pwm0",
		.min_uV = 900000,
		.max_uV = 1390000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = ARRAY_SIZE(pwm0_supply),
	.consumer_supplies = pwm0_supply,
};

static struct regulator_init_data pwm1_init_pdata = {
	.constraints = {
		.name = "pwm1",
		.min_uV = 900000,
		.max_uV = 1390000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = ARRAY_SIZE(pwm1_supply),
	.consumer_supplies = pwm1_supply,
};

static struct regulator_init_data pwm2_init_pdata = {
	.constraints = {
		.name = "pwm2",
		.min_uV = 900000,
		.max_uV = 1390000,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = ARRAY_SIZE(pwm2_supply),
	.consumer_supplies = pwm2_supply,
};

static struct pwm_subdev_info pwm_subdevs[] = {
	{
		.id = 0,
		.name = "pwm-regulator",
		.platform_data = &pwm0_init_pdata,
	},
	{
		.id = 1,
		.name = "pwm-regulator",
		.platform_data = &pwm1_init_pdata,
	},
	{
		.id = 2,
		.name = "pwm-regulator",
		.platform_data = &pwm2_init_pdata,
	},
};

static int pwm_create_regulator(void)
{
	struct pwm_subdev_info *subdev;
	struct platform_device *pdev;
	int chn;
	int ret = 0;

	chn = item_integer("config.vol.regulator", 1);

	subdev = &pwm_subdevs[chn];

	pdev = platform_device_alloc(subdev->name, subdev->id);

	pdev->dev.platform_data = subdev->platform_data;

	ret = platform_device_add(pdev);
	if (ret) {
		pr_err("failed add platform device %s\n", pdev->name);
		return -1;
	}

	return 0;
}

static int __init imap_init_pwm(void)
{
	imap_pwm_chan_t *cp;
	int channel;
	uint32_t tcon;
	struct resource *res0, *res1;

	spin_lock_init(&infotm_pwm_lock);

	if (item_equal("bl.ctrl", "pwm", 0)) {
		gpwmchn = item_integer("bl.ctrl", 1);
	} else
		gpwmchn = 0;

	gintensity = -1;

    if (gintensity == -1) {                                                                                   
          gintensity = item_integer("bl.def_intensity", 0);                       
          if (gintensity > 255) {                                                                               
              gintensity = 255;                                                                                 
          }                                                                                                     
    }      

	if (item_exist("bl.start")) {
		gstart = item_integer("bl.start", 0);
	} else {
		gstart = 6136;
	}
	if (item_exist("bl.low_intensity")) {
		glow_intensity = item_integer("bl.low_intensity", 0);
	} else {
		glow_intensity = 20;
	}
	if (item_exist("bl.high_intensity")) {
		ghigh_intensity = item_integer("bl.high_intensity", 0);
	} else {
		ghigh_intensity = 255;
	}

	intensity_range = ghigh_intensity - glow_intensity;

	gstep = (gstart * 8) / 134;
	printk(KERN_ERR "iMAP PWM Driver Init.\n");

	res0 = &imap_pwm_resource[0];

	res1 = request_mem_region(res0->start, resource_size(res0), "imap-pwm");
	if (!res1) {
		printk(KERN_ERR " [imap_pwm]-FAILED TO RESERVE MEM REGION \n");
		return -ENXIO;
	}

	ioaddr = ioremap_nocache(res0->start, resource_size(res0));
	if (!ioaddr) {
		printk(KERN_ERR "[imap_pwm]-FAILED TO MEM REGIDSTERS\n");
		return -ENXIO;
	}

	for (channel = 0; channel < IMAP_PWM_CHANNEL; channel++) {
		cp = &imap_chans[channel];

		memset(cp, 0, sizeof(imap_pwm_chan_t));
		cp->channel = channel;

		cp->irq = channel + GIC_PWM0_ID;
	}

	apb_clk = clk_get(NULL, "apb_pclk");

if(item_exist("config.uboot.logo")){
		unsigned char str[ITEM_MAX_LEN] = {0};
		item_string(str,"config.uboot.logo",0);
		if(strncmp(str,"0",1) == 0){
			module_power_on(SYSMGR_PWM_BASE);
			imap_pwm_div();
			tcon = pwm_readl(ioaddr, IMAP_TCON);
			tcon &= ~(0x1 << 4);
			pwm_writel(ioaddr, IMAP_TCON, tcon);
		}
	}


	if (item_exist("config.vol.regulator")
		&& item_equal("config.vol.regulator", "pwm", 0)) {
		pwm_create_regulator();
	}

	return 0;
}

int pwm_change_width(int channel, int intensity)
{

	int clk;
	int cur_tcnt;
	int cur_tcmp;

	spin_lock(&infotm_pwm_lock);
	//end = item_integer("bl.end",0);
	//printk("bl.low_intensity = %d,intensity=%d\n", glow_intensity,intensity);

	gintensity = glow_intensity + (intensity * intensity_range) / 0xff;	//intensity is from 0 to 0xff

	//printk("gintensity = %d\n", gintensity);

	clk = clk_get_rate(apb_clk);
	clk /= 1000000;
	cur_tcnt = gstart - (((134 - clk) / 8) * gstep);
	//cur_tcmp = (cur_tcnt - end) * intensity / 0xff + end;
	cur_tcmp = (cur_tcnt - 0) * gintensity / 0xff + 0;

	imap_timer_setup(channel, cur_tcnt, cur_tcmp);
	spin_unlock(&infotm_pwm_lock);

	return 0;
}

#ifdef CONFIG_CPU_FREQ

int pwm_cpufreq_prechange(int pre_clk)
{

	int clk;

	spin_lock(&infotm_pwm_lock);

	clk = pre_clk / 6;
	clk /= 1000000;
    if (gintensity == -1) {                                                                                   
        gintensity = item_integer("bl.def_intensity", 0);                       
        if (gintensity > 255) {                                                                               
            gintensity = 255;                                                                                 
        }                                                                                                     
    }      
	gcur_tcnt = gstart - (((134 - clk) / 8) * gstep);
	gcur_tcmp = (gcur_tcnt - 0) * gintensity / 0xff + 0;

	pwm_writel(ioaddr, IMAP_TCNTB(gpwmchn), gcur_tcnt);
	pwm_writel(ioaddr, IMAP_TCMPB(gpwmchn), gcur_tcmp);
	spin_unlock(&infotm_pwm_lock);

	return 0;
}

int pwm_cpufreq_trans(void)
{

	int tcon;
	//printk("gintensity = %x\n", gintensity);
	//pwm_change_width(gintensity);
	spin_lock(&infotm_pwm_lock);
	//imap_timer_setup(0, gcur_tcnt, gcur_tcmp);

	tcon = pwm_readl(ioaddr, IMAP_TCON);
	switch (gpwmchn) {
	case 0:
		tcon |= IMAP_TCON_T0MU_ON;
		break;
	case 1:
		tcon |= IMAP_TCON_T1MU_ON;
		break;
	case 2:
		tcon |= IMAP_TCON_T2MU_ON;
		break;
	case 3:
		tcon |= IMAP_TCON_T3MU_ON;
		break;
	case 4:
		tcon |= IMAP_TCON_T4MU_ON;
	}
	pwm_writel(ioaddr, IMAP_TCON, tcon);

	spin_unlock(&infotm_pwm_lock);
	return 0;
}
#endif


