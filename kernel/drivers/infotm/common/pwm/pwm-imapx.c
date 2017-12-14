/*
 * drivers/pwm/pwm-imapx.c
 * Author: ps_cv@infotm.com
 */
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/pwm.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <mach/pad.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>

#include "pwm-imapx.h"

#define PWM_MAX_TCNT 0xFFF0
#define PWM_MIN_TCNT 100

struct clk_div {
	int *mux; /*each channel has its mux*/
	int prescaler[2];  /* chn 0-1  chn 2 3 4 */
	unsigned long prescaler_flag;
	unsigned long *pwm_clk_rate;
};

struct imapx_pwm_chip {
	struct pwm_chip     chip;
	struct device       *dev;
	struct clk       *clk;
	unsigned long    clk_rate;
	struct  clk_div    clk_div;
	void __iomem        *base;
};                             

static inline struct imapx_pwm_chip *to_imapx_pwm_chip(struct pwm_chip *chip)
{
	return container_of(chip, struct imapx_pwm_chip, chip);
}

static inline u32 pwm_readl(struct imapx_pwm_chip *chip, int offset, int sub_chip)
{
	/*
	 * Todo: if you want to use pwm4 on apollo chip, you should change 
	 * some codes here refer to data spec.
	 */
	int imapx_pwm_base = chip->base + sub_chip * 0x1000;
	return readl(imapx_pwm_base +  offset);
}

static inline void pwm_writel(struct imapx_pwm_chip *chip, int offset,
								u32 val, int sub_chip)
{	/*
	 * Todo: if you want to use pwm4 on apollo chip, you should change 
	 * some codes here refer to data spec.
	 */
	int imapx_pwm_base = chip->base + sub_chip * 0x1000;
	writel(val, imapx_pwm_base + offset);
}

static void imapx_pwm_ctrl_enable(struct imapx_pwm_chip *chip, int chn, int enable, int sub_chip) 
{
	u32 tcon;
	u32 tcfg1;
	tcon = pwm_readl(chip, IMAP_TCON, sub_chip);
	tcfg1 = pwm_readl(chip, IMAP_TCFG1, sub_chip);
	switch (chn % 4) {
		case 0:
			if(enable) {
				tcon |= IMAP_TCON_T0START;
				tcon |= IMAP_TCON_T0RL_ON;
				tcon |= IMAP_TCON_T0MU_ON;
			} else {
				tcon &= ~IMAP_TCON_T0START;
				tcfg1 &= ~0xf;
			}
			break;
		case 1:
			if(enable) {
				tcon |= IMAP_TCON_T1START;
				tcon |= IMAP_TCON_T1RL_ON;
				tcon |= IMAP_TCON_T1MU_ON;
			} else {
				tcon &= ~IMAP_TCON_T1START;
				tcfg1 &= ~0xf0;
			}
			break;
		case 2:
			if(enable) {
				tcon |= IMAP_TCON_T2START;
				tcon |= IMAP_TCON_T2RL_ON;
				tcon |= IMAP_TCON_T2MU_ON;
			} else {
				tcon &= ~IMAP_TCON_T2START;
				tcfg1 &= ~0xf00;
			}
			break;
		case 3:
			if(enable) {
				tcon |= IMAP_TCON_T3START;
				tcon |= IMAP_TCON_T3RL_ON;
				tcon |= IMAP_TCON_T3MU_ON;
			} else {
				tcon &= ~IMAP_TCON_T3START;
				tcfg1 &= ~0xf000;
			}
			break;
		case 4:
			if(enable) {
				tcon |= IMAP_TCON_T4START;
				tcon |= IMAP_TCON_T4RL_ON;
				tcon |= IMAP_TCON_T4MU_ON;
			} else {
				tcon &= ~IMAP_TCON_T4START;
				tcfg1 &= ~0xf0000;
			}
			break;
	}                                    

	if(!enable)
		pwm_writel(chip,  IMAP_TCFG1, tcfg1, sub_chip); 
	pwm_writel(chip, IMAP_TCON, tcon, sub_chip); 
}



static int is_prescaler_busy(struct imapx_pwm_chip *pc, int chn){
	int busy = 1;
	switch( chn % 4 ) {
		case 0:
			busy = test_bit(1, &pc->clk_div.prescaler_flag);
			break;
		case 1:
			busy = test_bit(0, &pc->clk_div.prescaler_flag);
			break;
		case 2:
			busy = test_bit(3, &pc->clk_div.prescaler_flag) || test_bit(4, &pc->clk_div.prescaler_flag);
			break;
		case 3:
			busy = test_bit(2, &pc->clk_div.prescaler_flag) || test_bit(4, &pc->clk_div.prescaler_flag);
			break;
		case 4:
			busy = test_bit(2, &pc->clk_div.prescaler_flag) || test_bit(3, &pc->clk_div.prescaler_flag);
			break;

	}
	return busy;
}

static void imapx_pwm_update_config(struct imapx_pwm_chip *pc, int chn, int tcntb, int tcmpb, int prescaler, int mux, int sub_chip) {
	u32 tmp;
	pwm_writel(pc, IMAP_TCNTB(chn), tcntb, sub_chip);
	pwm_writel(pc, IMAP_TCMPB(chn), tcmpb, sub_chip);

	/*prescaler*/
	tmp = pwm_readl(pc, IMAP_TCFG0, sub_chip);
	if(chn < 2) {
		tmp &= ~0xff;
		tmp |= prescaler;
	} else {
		tmp &= ~0xff00;
		tmp |= prescaler << 8;
	}
	pwm_writel(pc, IMAP_TCFG0, tmp, sub_chip);

	/*mux*/
	tmp = pwm_readl(pc, IMAP_TCFG1, sub_chip);
	tmp &= ~(0xf << (chn * 4));
	tmp |= ((mux + 7) << (chn * 4));
	pwm_writel(pc, IMAP_TCFG1, tmp, sub_chip);

}


static int imapx_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
		int duty_ns, int period_ns)
{
	struct imapx_pwm_chip *pc = to_imapx_pwm_chip(chip);
	int chn = pwm->hwpwm;
	int sub_chip = pwm->hwpwm / 4;
	int prescaler = 0, mux = 0, tcntb = 0, tcmpb = 0;
	int fcur_period;
	int found = 0;
	unsigned long fcur = 0, fmin = 0, fmax = 0;
	if(period_ns == 0)
		return -1;


	fmin = 1000000000ul / period_ns * PWM_MIN_TCNT;
	fmax = 1000000000ul / period_ns * PWM_MAX_TCNT;

	chn = chn % 4;
	if(is_prescaler_busy(pc, chn)) {
		/*pwm chn busy*/
		//pr_err("[pwm] chn %d busy!\n", chn);
		prescaler = (chn < 2) ? pc->clk_div.prescaler[0]: pc->clk_div.prescaler[1];
		for(mux = 4; mux > 0; mux--) {
			fcur = pc->clk_rate / (1 << mux) / (prescaler + 1);
			if(fmin < fcur && fcur < fmax) {
				fcur_period = 1000000000ul / fcur; 
				tcntb = period_ns / fcur_period;
				tcmpb = duty_ns / fcur_period;
				found = 1;
			}
			if(found)
				break;
		}
	
	} else {
		/*pwm chn free*/
		for (prescaler = 255; prescaler >=0; prescaler-=16) {
			for(mux = 4; mux > 0; mux--) {
				fcur = pc->clk_rate / (1 << mux) / (prescaler + 1);
				if(fmin < fcur && fcur < fmax) {
					fcur_period = 1000000000ul / fcur; 
					tcntb = period_ns / fcur_period;
					tcmpb = duty_ns / fcur_period;
					found = 1;
				}
				if(found)
					break;
			}
			if(found)
				break;
		}

	}
	if(found){ 
		pc->clk_div.mux[chn] = mux;
		if(chn < 2) 
			pc->clk_div.prescaler[0] = prescaler;
		else
			pc->clk_div.prescaler[1] = prescaler;

		pc->clk_div.pwm_clk_rate[chn] = fcur;
		/*
		   pr_err("[pwm] Success to set pwm.%d: period_ns-%d, duty_ns-%d,"
		   "mux-%d, prescaler-%d, freq-%ld, tcntb-%d,tcmpd-%d\n",
		   chn, period_ns, duty_ns, mux, prescaler, fcur,
		   tcntb, tcmpb);
		 */
	} else {
		pr_err("[pwm] Failed to set pwm.%d: period_ns-%d, duty_ns-%d.\n",
				chn, period_ns, duty_ns);
		return -1;
		
	}

	imapx_pwm_update_config(pc, chn, tcntb, tcmpb, prescaler, mux, sub_chip);
	return 0;
}

static int imapx_pwm_set_polarity(struct pwm_chip *chip, struct pwm_device *pwm,
						enum pwm_polarity polarity)
{
	struct imapx_pwm_chip *pc = to_imapx_pwm_chip(chip);
	int chn = pwm->hwpwm;
	int sub_chip = pwm->hwpwm / 4;

	/*
	 * Todo: if you want to use pwm4 on apollo chip, you should change 
	 * some codes here refer to data spec.
	 */
	unsigned long tcon = pwm_readl(pc, IMAP_TCON, sub_chip);
	switch (chn % 4) {
		case 0 :
			if(polarity == PWM_POLARITY_NORMAL)
				tcon &= ~IMAP_TCON_T0OI_ON;
			else if(polarity == PWM_POLARITY_INVERSED)
				tcon |= IMAP_TCON_T0OI_ON;
			break;
		case 1 :
			if(polarity == PWM_POLARITY_NORMAL)
				tcon &= ~IMAP_TCON_T1OI_ON;
			else if(polarity == PWM_POLARITY_INVERSED)
				tcon |= IMAP_TCON_T1OI_ON;
			break;
		case 2 :
			if(polarity == PWM_POLARITY_NORMAL)
				tcon &= ~IMAP_TCON_T2OI_ON;
			else if(polarity == PWM_POLARITY_INVERSED)
				tcon |= IMAP_TCON_T2OI_ON;
			break;
		case 3 :
			if(polarity == PWM_POLARITY_NORMAL)
				tcon &= ~IMAP_TCON_T3OI_ON;
			else if(polarity == PWM_POLARITY_INVERSED)
				tcon |= IMAP_TCON_T3OI_ON;
			break;
		default:
			break;
			
	}
	pwm_writel(pc, IMAP_TCON, tcon, sub_chip);
	return 0;
}

static int imapx_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct imapx_pwm_chip *pc = to_imapx_pwm_chip(chip);
	int ret, chn = pwm->hwpwm;
	int sub_chip = pwm->hwpwm / 4;
	char con_id[5] = {0}; /* pwm0 pwm1 pwm2 pwm3*/

	sprintf(con_id, "pwm%d", chn);
	ret = imapx_pad_init(con_id);
	if(ret < 0) {
		pr_err("[pwm] config pad -%s- return err %d\n", con_id, ret);
		return -1;
	}

	set_bit(chn, &pc->clk_div.prescaler_flag);
	imapx_pwm_ctrl_enable(pc, chn, 1, sub_chip); 

	return 0;
}

static void imapx_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct imapx_pwm_chip *pc = to_imapx_pwm_chip(chip);
	int chn = pwm->hwpwm;
	int sub_chip = pwm->hwpwm / 4;


	//pr_err("[pwm] disable %d\n", chn);
	clear_bit(chn, &pc->clk_div.prescaler_flag);
	imapx_pwm_ctrl_enable(pc, chn, 0, sub_chip); 
	return;
}

static const struct pwm_ops imapx_pwm_ops = {
	.config = imapx_pwm_config,
	.set_polarity = imapx_pwm_set_polarity,
	.enable = imapx_pwm_enable,
	.disable = imapx_pwm_disable,
	.owner = THIS_MODULE,
};

static int imapx_pwm_probe(struct platform_device *pdev)
{

	struct imapx_pwm_platform_data *pdata = pdev->dev.platform_data;
	struct imapx_pwm_chip *pwm;
	struct resource *res;
	int ret;

	pwm = devm_kzalloc(&pdev->dev, sizeof(*pwm), GFP_KERNEL);
	pwm->clk_div.mux = devm_kzalloc(&pdev->dev, sizeof(int) * pdata->npwm, GFP_KERNEL); 
	pwm->clk_div.pwm_clk_rate = devm_kzalloc(&pdev->dev, sizeof(unsigned long) 
			* pdata->npwm, GFP_KERNEL);
	if (!pwm || !pwm->clk_div.mux || !pwm->clk_div.pwm_clk_rate) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}


	pwm->dev = &pdev->dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pwm->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(pwm->base))
		return PTR_ERR(pwm->base);

	platform_set_drvdata(pdev, pwm);

	pwm->clk = clk_get_sys("imap-pwm", "imap-pwm");
	if (IS_ERR(pwm->clk))
		return PTR_ERR(pwm->clk);

#if defined(CONFIG_APOLLO3_FPGA_PLATFORM)
	pwm->clk_rate = 40*1000*1000;
#else
	pwm->clk_rate = clk_get_rate(pwm->clk);
	ret = clk_prepare_enable(pwm->clk);
	if(ret < 0) {
		dev_err(&pdev->dev, "failed to enable pwm clock\n");
		return ret;
	}
#endif
	/* set default prescaler & mux */
	module_power_on(SYSMGR_PWM_BASE);
	pwm->chip.dev = &pdev->dev;
	pwm->chip.ops = &imapx_pwm_ops;
	pwm->chip.base = -1;
	pwm->chip.npwm = pdata->npwm;

	ret = pwmchip_add(&pwm->chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "pwmchip_add() failed: %d\n", ret);
		return ret;
	}
	pr_info("imapx pwm driver probe success!\n");
	return 0;
}

static int imapx_pwm_remove(struct platform_device *pdev)
{
	int i;
	struct imapx_pwm_chip *pc = platform_get_drvdata(pdev);
	struct imapx_pwm_platform_data *pdata = pdev->dev.platform_data;

	if (WARN_ON(!pc))
		return -ENODEV;
	for(i = 0; i < pdata->npwm; i++){
		clk_disable_unprepare(pc->clk);
	}
	return pwmchip_remove(&pc->chip);
}


#ifdef CONFIG_PM_SLEEP
static int imapx_pwm_suspend(struct device *dev)
{
	return 0;
}

static int imapx_pwm_resume(struct device *dev)
{
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(imapx_pwm_pm_ops, imapx_pwm_suspend,
		imapx_pwm_resume);


static struct platform_driver imapx_pwm_driver = {
	.driver = {
		.name = "imap-pwm",
		.owner = THIS_MODULE,
		.pm = &imapx_pwm_pm_ops
	},
	.probe = imapx_pwm_probe,
	.remove = imapx_pwm_remove,
};

static int __init imapx_pwm_init(void)                          
{                                                         
	int ret;                                              
	pr_info("imapx Pulse-Width Modulation(PWM) init ...\n ");
	ret = platform_driver_register(&imapx_pwm_driver);      
	if (ret)                                              
		pr_err("failed to add pwm driver\n");             
	return ret;                                           
}                                                         

arch_initcall(imapx_pwm_init);                                  

MODULE_LICENSE("GPL");              
MODULE_AUTHOR("InfotmIC");
MODULE_ALIAS("platform:imapx-pwm"); 

