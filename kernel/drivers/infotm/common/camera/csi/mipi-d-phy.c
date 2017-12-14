/*
 * 
 * INFOTM MIPI PHY
 */

#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <mach/regs-clock.h>
#include "mipi-csi-interface.h"

static int __infotm_mipi_phy_control(int id, bool on, u32 reset)
{
#if 0
	static DEFINE_SPINLOCK(lock);
	void __iomem *addr;
	unsigned long flags;
	u32 cfg;

	id = max(0, id);
	if (id > 1)
		return -EINVAL;

	addr = INFOTM_MIPI_DPHY_CONTROL(id);

	spin_lock_irqsave(&lock, flags);

	cfg = __raw_readl(addr);
	cfg = on ? (cfg | reset) : (cfg & ~reset);
	__raw_writel(cfg, addr);

	if (on) {
		cfg |= INFOTM_MIPI_DPHY_ENABLE;
	} else if (!(cfg & (INFOTM_MIPI_DPHY_SRESETN |
			    INFOTM_MIPI_DPHY_MRESETN) & ~reset)) {
		cfg &= ~INFOTM_MIPI_DPHY_ENABLE;
	}

	__raw_writel(cfg, addr);
	spin_unlock_irqrestore(&lock, flags);
#endif
	return 0;
}

int infotm_csi_phy_enable(int id, bool on)
{
	return __infotm_mipi_phy_control(id, on, INFOTM_MIPI_DPHY_SRESETN);
}
EXPORT_SYMBOL(infotm_csi_phy_enable);

int infotm_dsim_phy_enable(struct platform_device *pdev, bool on)
{
	return __infotm_mipi_phy_control(pdev->id, on, INFOTM_MIPI_DPHY_MRESETN);
}
EXPORT_SYMBOL(infotm_dsim_phy_enable);
