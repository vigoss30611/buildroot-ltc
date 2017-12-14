#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>

struct power_domain_dev {
	struct platform_device *pdev;
	struct kobject *kobj;
};

static ssize_t isp_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *isp_sysmgr = IO_ADDRESS(SYSMGR_ISP_BASE);

	val = readl(isp_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t isp_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_ISP_BASE);
	}else {
		module_power_down(SYSMGR_ISP_BASE);
	}
	return count;
}

static ssize_t mipi_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *mipidsi_sysmgr = IO_ADDRESS(SYSMGR_MIPI_BASE);

	val = readl(mipidsi_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t mipi_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_MIPI_BASE);
	}else {
		module_power_down(SYSMGR_MIPI_BASE);
	}
	return count;
}

static ssize_t h1_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *h1_sysmgr = IO_ADDRESS(SYSMGR_VENC264_BASE);

	val = readl(h1_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t h1_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_VENC264_BASE);
	}else {
		module_power_down(SYSMGR_VENC264_BASE);
	}
	return count;
}

static ssize_t crypto_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *crypto_sysmgr = IO_ADDRESS(SYSMGR_CRYPTO_BASE);

	val = readl(crypto_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t crypto_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_CRYPTO_BASE);
	}else {
		module_power_down(SYSMGR_CRYPTO_BASE);
	}
	return count;
}

static ssize_t ids0_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *gpio_sysmgr = IO_ADDRESS(SYSMGR_IDS0_BASE);

	tmp = readl(gpio_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t ids0_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_IDS0_BASE);
	else
		module_power_down(SYSMGR_IDS0_BASE);
	return count;
}

static ssize_t usbh_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *usbh_sysmgr = IO_ADDRESS(SYSMGR_USBH_BASE);

	tmp = readl(usbh_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t usbh_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_USBH_BASE);
	else
		module_power_down(SYSMGR_USBH_BASE);
	return count;
}

static ssize_t otg_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *otg_sysmgr = IO_ADDRESS(SYSMGR_OTG_BASE);

	tmp = readl(otg_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t otg_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_OTG_BASE);
	else
		module_power_down(SYSMGR_OTG_BASE);
	return count;
}

static ssize_t sd1_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *sd1_sysmgr = IO_ADDRESS(SYSMGR_MMC1_BASE);

	tmp = readl(sd1_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd1_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_MMC1_BASE);
	else
		module_power_down(SYSMGR_MMC1_BASE);
	return count;
}

static ssize_t sd2_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *sd2_sysmgr = IO_ADDRESS(SYSMGR_MMC2_BASE);

	tmp = readl(sd2_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd2_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_MMC2_BASE);
	else
		module_power_down(SYSMGR_MMC2_BASE);
	return count;
}

static ssize_t sd0_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *sd0_sysmgr = IO_ADDRESS(SYSMGR_MMC0_BASE);

	tmp = readl(sd0_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd0_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_MMC0_BASE);
	else
		module_power_down(SYSMGR_MMC0_BASE);
	return count;
}

static ssize_t eth_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_MAC_BASE);

	tmp = readl(eth_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t eth_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_MAC_BASE);
	else
		module_power_down(SYSMGR_MAC_BASE);
	return count;
}

static ssize_t gdma_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_GDMA_BASE);

	tmp = readl(eth_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t gdma_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_GDMA_BASE);
	else
		module_power_down(SYSMGR_GDMA_BASE);
	return count;
}

static ssize_t dsp_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_DSP_BASE);

	tmp = readl(eth_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t dsp_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_DSP_BASE);
	else
		module_power_down(SYSMGR_DSP_BASE);
	return count;
}

static ssize_t ispost_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_ISPOST_BASE);

	tmp = readl(eth_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t ispost_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_ISPOST_BASE);
	else
		module_power_down(SYSMGR_ISPOST_BASE);
	return count;
}

static ssize_t fodet_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_FODET_BASE);

	tmp = readl(eth_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t fodet_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_FODET_BASE);
	else
		module_power_down(SYSMGR_FODET_BASE);
	return count;
}

static ssize_t camif_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_CAMIF_BASE);

	tmp = readl(eth_sysmgr + 0x8);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t camif_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_CAMIF_BASE);
	else
		module_power_down(SYSMGR_CAMIF_BASE);
	return count;
}

struct module_info {
	char *name;
	uint32_t base;
};

static struct module_info power_info[] = {
	{"isp", SYSMGR_ISP_BASE},
	{"mipi", SYSMGR_MIPI_BASE},
	{"crypto", SYSMGR_CRYPTO_BASE},
	{"h1", SYSMGR_VENC264_BASE},
	{"ids0", SYSMGR_IDS0_BASE},
	{"usbhost", SYSMGR_USBH_BASE},
	{"usbotg", SYSMGR_OTG_BASE},
	{"sd1", SYSMGR_MMC1_BASE},
	{"sd2", SYSMGR_MMC2_BASE},
	{"sd0", SYSMGR_MMC0_BASE},
	{"eth", SYSMGR_MAC_BASE},
	{"gdma", SYSMGR_GDMA_BASE},
	{"dsp", SYSMGR_DSP_BASE},
	{"ispost", SYSMGR_ISPOST_BASE},
	{"fodet", SYSMGR_FODET_BASE},
	{"camif", SYSMGR_EMIF_BASE},
};

static ssize_t power_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	int i;

	pr_err("module\t\tdomain\n");
	for (i = 0; i < ARRAY_SIZE(power_info); i++) {
		tmp = readl(IO_ADDRESS(power_info[i].base) + 0x8);
		pr_err("%-15s  %-15s\n", power_info[i].name, ((tmp&0x2)?"on":"off"));
	}

	return i;
}

static DEVICE_ATTR(isp, 0666, isp_domain_show, isp_domain_store);
static DEVICE_ATTR(mipi, 0666, mipi_domain_show, mipi_domain_store);
static DEVICE_ATTR(crypto, 0666, crypto_domain_show, crypto_domain_store);
static DEVICE_ATTR(h1, 0666, h1_domain_show, h1_domain_store);
static DEVICE_ATTR(ids0, 0666, ids0_domain_show, ids0_domain_store);
static DEVICE_ATTR(usbhost, 0666, usbh_domain_show, usbh_domain_store);
static DEVICE_ATTR(usbotg, 0666, otg_domain_show, otg_domain_store);
static DEVICE_ATTR(sd1, 0666, sd1_domain_show, sd1_domain_store);
static DEVICE_ATTR(sd2, 0666, sd2_domain_show, sd2_domain_store);
static DEVICE_ATTR(sd0, 0666, sd0_domain_show, sd0_domain_store);
static DEVICE_ATTR(eth, 0666, eth_domain_show, eth_domain_store);
static DEVICE_ATTR(gdma, 0666, gdma_domain_show, gdma_domain_store);
static DEVICE_ATTR(dsp, 0666, dsp_domain_show, dsp_domain_store);
static DEVICE_ATTR(ispost, 0666, ispost_domain_show, ispost_domain_store);
static DEVICE_ATTR(fodet, 0666, fodet_domain_show, fodet_domain_store);
static DEVICE_ATTR(camif, 0666, camif_domain_show, camif_domain_store);
static DEVICE_ATTR(domain, 0444, power_domain_show, NULL);

static ssize_t ids0_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_IDS0_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t ids0_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_IDS0_BASE) + 0x14);
	return count;
}

static ssize_t crypto_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_CRYPTO_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t crypto_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_CRYPTO_BASE) + 0x14);
	return count;
}

static ssize_t sd1_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_MMC1_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd1_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_MMC1_BASE) + 0x14);
	return count;
}

static ssize_t sd2_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_MMC2_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd2_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_MMC2_BASE) + 0x14);
	return count;
}

static ssize_t sd0_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_MMC0_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd0_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_MMC0_BASE) + 0x14);
	return count;
}

static ssize_t eth_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_MAC_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t eth_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_MAC_BASE) + 0x14);
	return count;
}

static ssize_t adc_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_ADC_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t adc_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_ADC_BASE) + 0x14);
	return count;
}

static ssize_t camif_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_CAMIF_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t camif_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_CAMIF_BASE) + 0x14);
	return count;
}

static ssize_t mipi_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_MIPI_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t mipi_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_MIPI_BASE) + 0x14);
	return count;
}

static ssize_t dsp_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_DSP_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t dsp_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_DSP_BASE) + 0x14);
	return count;
}

static ssize_t ispost_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_ISPOST_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t ispost_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_ISPOST_BASE) + 0x14);
	return count;
}

static ssize_t fodet_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_FODET_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t fodet_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_FODET_BASE) + 0x14);
	return count;
}

static ssize_t h1_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_VENC264_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t h1_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_VENC264_BASE) + 0x14);
	return count;
}

static ssize_t isp_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_ISP_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t isp_memory_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	writel(tmp,IO_ADDRESS(SYSMGR_ISP_BASE) + 0x14);
	return count;
}

static struct module_info info[] = {
	{"ids", SYSMGR_IDS0_BASE},
	{"crypto", SYSMGR_CRYPTO_BASE},
	{"sd1", SYSMGR_MMC1_BASE},
	{"sd2", SYSMGR_MMC2_BASE},
	{"sd0", SYSMGR_MMC0_BASE},
	{"eth", SYSMGR_MAC_BASE},
	{"adc", SYSMGR_ADC_BASE},
	{"camif", SYSMGR_CAMIF_BASE},
	{"mipi", SYSMGR_MIPI_BASE},
	{"dsp", SYSMGR_DSP_BASE},
	{"ispost", SYSMGR_ISPOST_BASE},
	{"fodet", SYSMGR_FODET_BASE},
	{"h1", SYSMGR_VENC264_BASE},
	{"isp", SYSMGR_ISP_BASE},
};

static ssize_t module_memory_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	int i;

	pr_err("module\t\tmemory\n");
	for (i = 0; i < ARRAY_SIZE(info); i++) {
		tmp = readl(IO_ADDRESS(info[i].base + 0x14));
		pr_err("%-15s  %-15s\n", info[i].name, (tmp?"off":"on"));
	}

	return i;
}
static DEVICE_ATTR(mem_isp, 0666, isp_memory_show, isp_memory_store);
static DEVICE_ATTR(mem_h1, 0666, h1_memory_show, h1_memory_store);
static DEVICE_ATTR(mem_fodet, 0666, fodet_memory_show, fodet_memory_store);
static DEVICE_ATTR(mem_ispost, 0666, ispost_memory_show, ispost_memory_store);
static DEVICE_ATTR(mem_dsp, 0666, dsp_memory_show, dsp_memory_store);
static DEVICE_ATTR(mem_mipi, 0666, mipi_memory_show, mipi_memory_store);
static DEVICE_ATTR(mem_camif, 0666, camif_memory_show, camif_memory_store);
static DEVICE_ATTR(mem_adc, 0666, adc_memory_show, adc_memory_store);
static DEVICE_ATTR(mem_eth, 0666, eth_memory_show, eth_memory_store);
static DEVICE_ATTR(mem_sd1, 0666, sd1_memory_show, sd1_memory_store);
static DEVICE_ATTR(mem_sd2, 0666, sd2_memory_show, sd2_memory_store);
static DEVICE_ATTR(mem_sd0, 0666, sd0_memory_show, sd0_memory_store);
static DEVICE_ATTR(mem_crypto, 0666, crypto_memory_show, crypto_memory_store);
static DEVICE_ATTR(mem_ids0, 0666, ids0_memory_show, ids0_memory_store);
static DEVICE_ATTR(mem, 0444, module_memory_show, NULL);

static struct attribute *apollo_power_domain_attributes[] = {
	&dev_attr_isp.attr,
	&dev_attr_mipi.attr,
	&dev_attr_crypto.attr,
	&dev_attr_h1.attr,
	&dev_attr_ids0.attr,
	&dev_attr_usbhost.attr,
	&dev_attr_usbotg.attr,
	&dev_attr_sd1.attr,
	&dev_attr_sd2.attr,
	&dev_attr_sd0.attr,
	&dev_attr_eth.attr,
	&dev_attr_gdma.attr,
	&dev_attr_dsp.attr,
	&dev_attr_ispost.attr,
	&dev_attr_fodet.attr,
	&dev_attr_camif.attr,
	&dev_attr_domain.attr,
	&dev_attr_mem_isp.attr,
	&dev_attr_mem_h1.attr,
	&dev_attr_mem_fodet.attr,
	&dev_attr_mem_ispost.attr,
	&dev_attr_mem_dsp.attr,
	&dev_attr_mem_mipi.attr,
	&dev_attr_mem_camif.attr,
	&dev_attr_mem_adc.attr,
	&dev_attr_mem_eth.attr,
	&dev_attr_mem_sd1.attr,
	&dev_attr_mem_sd2.attr,
	&dev_attr_mem_sd0.attr,
	&dev_attr_mem_crypto.attr,
	&dev_attr_mem_ids0.attr,
	&dev_attr_mem.attr,
	NULL
};

static const struct attribute_group apollo_power_domain_attr_group = {
	.attrs = apollo_power_domain_attributes,
};

static struct power_domain_dev *dev = NULL;

static int apollo_power_domain_probe(struct platform_device *pdev)
{
	int ret;

	dev->kobj = kobject_create_and_add("power-domain", NULL);
	if (dev->kobj == NULL) {
		ret = -ENODEV;
		goto kobj_err;
	}

	ret = sysfs_create_group(&dev->pdev->dev.kobj,
					&apollo_power_domain_attr_group);
	if (ret < 0) {
		pr_err("create power-domain device file failed!\n");
		goto sysfs_err;
	}

	return 0;

sysfs_err:
	kobject_del(dev->kobj);

kobj_err:
	return ret;
}

static struct platform_driver apollo_power_domain_driver = {
	.probe	= apollo_power_domain_probe,
	.driver	= {
		.owner	= THIS_MODULE,
		.name	= "power_domain",
	},
};

static int __init apollo_power_domain_init(void)
{
	int ret;

	dev = kzalloc(sizeof(struct power_domain_dev), GFP_KERNEL);
	if (dev == NULL) {
		pr_err("%s: alloc power_domain dev error\n", __func__);
		return -ENODEV;
	}

	dev->pdev = platform_device_register_simple("power_domain", -1, NULL, 0);
	if (IS_ERR(dev->pdev)) {
		pr_err("%s: register device error\n", __func__);
		return -1;
	}

	ret = platform_driver_register(&apollo_power_domain_driver);
	if (ret < 0) {
		pr_err("%s: register driver error\n", __func__);
		return ret;
	}

	return 0;
}

static void __exit apollo_power_domain_exit(void)
{
	return;
}

module_init(apollo_power_domain_init);
module_exit(apollo_power_domain_exit);
MODULE_LICENSE("GPL");
