#include "mode2_config.h"
#include "mode3_config.h"
#include "mode4_config.h"
#include "mode5_config.h"


struct axp_function {
    char mode[16];
    void (* drvbus_ctrl)(int);
    void (* vbusen_ctrl)(int);
    int	 (* irq_used)(void);
    void (* irq_init)(uint32_t*, uint32_t *);
    int  (* pwrkey_used)(void);
    void (* mfd_minit)(struct device *);
    void (* mfd_msuspend)(struct device *);
    void (* mfd_mresume)(struct device *);
    void (* batt_minit)(struct axp_charger *);
    void (* batt_msuspend)(struct axp_charger *);
    void (* batt_mresume)(struct axp_charger *);
};

static struct axp_function axp_func[] = {
    {
	.mode           = "mode2",
	.drvbus_ctrl    = mode2_drvbus_ctrl,
	.vbusen_ctrl    = mode2_vbusen_ctrl,
	.irq_used       = mode2_irq_used,
	.irq_init       = mode2_irq_init,
	.pwrkey_used    = mode2_pwrkey_used,
	.mfd_minit      = mode2_mfd_init,
	.mfd_msuspend   = mode2_mfd_suspend,
	.mfd_mresume    = mode2_mfd_resume,
	.batt_minit     = mode2_batt_init,
	.batt_msuspend  = mode2_batt_suspend,
	.batt_mresume   = mode2_batt_resume,
    },
    {
	.mode 		= "mode3",
	.drvbus_ctrl	= mode3_drvbus_ctrl,
	.vbusen_ctrl	= mode3_vbusen_ctrl,
	.irq_used	= mode3_irq_used,
	.irq_init	= mode3_irq_init,
	.pwrkey_used	= mode3_pwrkey_used,
	.mfd_minit	= mode3_mfd_init,
	.mfd_msuspend	= mode3_mfd_suspend,
	.mfd_mresume	= mode3_mfd_resume,
	.batt_minit	= mode3_batt_init,
	.batt_msuspend	= mode3_batt_suspend,
	.batt_mresume	= mode3_batt_resume,
    },
    {
	.mode           = "mode4",
	.drvbus_ctrl    = mode4_drvbus_ctrl,
	.vbusen_ctrl    = mode4_vbusen_ctrl,
	.irq_used       = mode4_irq_used,
	.irq_init       = mode4_irq_init,
	.pwrkey_used    = mode4_pwrkey_used,
	.mfd_minit      = mode4_mfd_init,
	.mfd_msuspend   = mode4_mfd_suspend,
	.mfd_mresume    = mode4_mfd_resume,
	.batt_minit     = mode4_batt_init,
	.batt_msuspend  = mode4_batt_suspend,
	.batt_mresume   = mode4_batt_resume,
    },
    {
	.mode		= "mode5",
	.drvbus_ctrl	= mode5_drvbus_ctrl,
	.vbusen_ctrl	= mode5_vbusen_ctrl,
	.irq_used	= mode5_irq_used,
	.irq_init	= mode5_irq_init,
	.pwrkey_used	= mode5_pwrkey_used,
	.mfd_minit	= mode5_mfd_init,
	.mfd_msuspend	= mode5_mfd_suspend,
	.mfd_mresume	= mode5_mfd_resume,
	.batt_minit	= mode5_batt_init,
	.batt_msuspend	= mode5_batt_suspend,
	.batt_mresume	= mode5_batt_resume,
    },
};

static struct axp_function *axpf;

void axp_mode_scan(struct device *dev)
{
    int i;
    uint8_t tmp;

    if(item_exist("pmu.mode"))
    {
	for(i = 0; i < ARRAY_SIZE(axp_func); i++)
	{
	    if(item_equal("pmu.mode", axp_func[i].mode, 0))
	    {
		axpf = &axp_func[i];
	    }
	}
    }
    else
    {
	/* read axpreg to determine whether mode2 or mode5 */
	axp_read(dev, 0x00, &tmp);
	tmp &= (1 << 1);
	if(tmp)
	{
	    axpf = &axp_func[0];
	}
	else
	{
	    axpf = &axp_func[3];
	}
    }

    printk(KERN_ERR "[AXP]: %s\n", axpf->mode);
}

void axp_drvbus_ctrl(int val)
{
    axpf->drvbus_ctrl(val);
}

void axp_vbusen_ctrl(int val)
{
    axpf->vbusen_ctrl(val);
}

int axp_irq_used(void)
{
    return axpf->irq_used();
}

void axp_irq_init(uint32_t *low, uint32_t *high)
{
    axpf->irq_init(low, high);
}

int axp_pwrkey_used(void)
{
    return axpf->pwrkey_used();
}

void axp_mfd_minit(struct device *dev)
{
    axpf->mfd_minit(dev);
}

void axp_mfd_msuspend(struct device *dev)
{
    axpf->mfd_msuspend(dev);
}

void axp_mfd_mresume(struct device *dev)
{
    axpf->mfd_mresume(dev);
}

void axp_batt_minit(struct axp_charger *charger)
{
    axpf->batt_minit(charger);
}

void axp_batt_msuspend(struct axp_charger *charger)
{
    axpf->batt_msuspend(charger);
}

void axp_batt_mresume(struct axp_charger *charger)
{
    axpf->batt_mresume(charger);
}

