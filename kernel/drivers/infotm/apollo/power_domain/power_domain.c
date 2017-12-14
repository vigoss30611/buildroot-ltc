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

static ssize_t g1_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *g1_sysmgr = IO_ADDRESS(SYSMGR_VDEC264_BASE);

	val = readl(g1_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t g1_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_VDEC264_BASE);
	}else {
		module_power_down(SYSMGR_VDEC264_BASE);
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

static ssize_t g2_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *g2_sysmgr = IO_ADDRESS(SYSMGR_VDEC265_BASE);

	val = readl(g2_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t g2_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_VDEC265_BASE);
	}else {
		module_power_down(SYSMGR_VDEC265_BASE);
	}
	return count;
}

static ssize_t h2_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *h2_sysmgr = IO_ADDRESS(SYSMGR_VENC265_BASE);

	val = readl(h2_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t h2_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_VENC265_BASE);
	}else {
		module_power_down(SYSMGR_VENC265_BASE);
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

static ssize_t hdmi_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *hdmi_sysmgr = IO_ADDRESS(SYSMGR_IDS1_BASE);

	val = readl(hdmi_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t hdmi_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_IDS1_BASE);
	}else {
		module_power_down(SYSMGR_IDS1_BASE);
	}
	return count;
}

static ssize_t iis_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *iis_sysmgr = IO_ADDRESS(SYSMGR_IIS_BASE);

	tmp = readl(iis_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t iis_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp)
		module_power_on(SYSMGR_IIS_BASE);
	else
		module_power_down(SYSMGR_IIS_BASE);
	return count;
}

static ssize_t cmn_timer_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *ctimer_sysmgr = IO_ADDRESS(SYSMGR_CMNTIMER_BASE);

	tmp = readl(ctimer_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t cmn_timer_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp)
		module_power_on(SYSMGR_CMNTIMER_BASE);
	else
		module_power_down(SYSMGR_CMNTIMER_BASE);
	return count;
}

static ssize_t pwm_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *pwm_sysmgr = IO_ADDRESS(SYSMGR_PWM_BASE);

	tmp = readl(pwm_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t pwm_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_PWM_BASE);
	else
		module_power_down(SYSMGR_PWM_BASE);
	return count;
}

static ssize_t iic_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *iic_sysmgr = IO_ADDRESS(SYSMGR_IIC_BASE);

	tmp = readl(iic_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t iic_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_IIC_BASE);
	else
		module_power_down(SYSMGR_IIC_BASE);
	return count;
}

static ssize_t pcm_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *pcm_sysmgr = IO_ADDRESS(SYSMGR_PCM_BASE);

	tmp = readl(pcm_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t pcm_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_PCM_BASE);
	else
		module_power_down(SYSMGR_PCM_BASE);
	return count;
}

static ssize_t ssp_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *ssp_sysmgr = IO_ADDRESS(SYSMGR_SSP_BASE);

	tmp = readl(ssp_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t ssp_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_SSP_BASE);
	else
		module_power_down(SYSMGR_SSP_BASE);
	return count;
}

static ssize_t codec_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *codec_sysmgr = IO_ADDRESS(SYSMGR_TQLP9624_BASE);

	tmp = readl(codec_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t codec_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_TQLP9624_BASE);
	else
		module_power_down(SYSMGR_TQLP9624_BASE);
	return count;
}

static ssize_t gpio_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *gpio_sysmgr = IO_ADDRESS(SYSMGR_GPIO_BASE);

	tmp = readl(gpio_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t gpio_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_GPIO_BASE);
	else
		module_power_down(SYSMGR_GPIO_BASE);
	return count;
}

static ssize_t wdt_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *wdt_sysmgr = IO_ADDRESS(SYSMGR_PWDT_BASE);

	tmp = readl(wdt_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t wdt_mem_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if (tmp)
		module_power_on(SYSMGR_PWDT_BASE);
	else
		module_power_down(SYSMGR_PWDT_BASE);
	return count;
}

static ssize_t usbh_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *usbh_sysmgr = IO_ADDRESS(SYSMGR_USBH_BASE);

	tmp = readl(usbh_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t usbh_mem_store(struct device *dev,
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

static ssize_t otg_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *otg_sysmgr = IO_ADDRESS(SYSMGR_OTG_BASE);

	tmp = readl(otg_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t otg_mem_store(struct device *dev,
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

static ssize_t sd1_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *sd1_sysmgr = IO_ADDRESS(SYSMGR_MMC1_BASE);

	tmp = readl(sd1_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd1_mem_store(struct device *dev,
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

static ssize_t sd2_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *sd2_sysmgr = IO_ADDRESS(SYSMGR_MMC2_BASE);

	tmp = readl(sd2_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd2_mem_store(struct device *dev,
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

static ssize_t sd0_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *sd0_sysmgr = IO_ADDRESS(SYSMGR_MMC0_BASE);

	tmp = readl(sd0_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t sd0_mem_store(struct device *dev,
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

static ssize_t eth_mem_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t tmp;
	void __iomem *eth_sysmgr = IO_ADDRESS(SYSMGR_MAC_BASE);

	tmp = readl(eth_sysmgr + 0x14);
	return sprintf(buf, "%s\n", (tmp?"off":"on"));
}

static ssize_t eth_mem_store(struct device *dev,
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

struct module_info {
	char *name;
	uint32_t base;
};

static struct module_info info[] = {
	{"iis", SYSMGR_IIS_BASE},
	{"cmn-timer", SYSMGR_CMNTIMER_BASE},
	{"pwm", SYSMGR_PWM_BASE},
	{"iic", SYSMGR_IIC_BASE},
	{"pcm", SYSMGR_PCM_BASE},
	{"ssp", SYSMGR_SSP_BASE},
	{"codec", SYSMGR_TQLP9624_BASE},
	{"gpio", SYSMGR_GPIO_BASE},
	{"watchdog", SYSMGR_PWDT_BASE},
//	{"ids0", SYSMGR_IDS0_BASE},
	{"ids1", SYSMGR_IDS1_BASE},
// changed by linyun.xiong@2015-09-04 ???  for test
//	{"usb-host", SYSMGR_USBH_BASE}, 
//	{"usb-otg", SYSMGR_OTG_BASE},
	{"sd1", SYSMGR_MMC1_BASE},
	{"sd2", SYSMGR_MMC2_BASE},
	{"sd0", SYSMGR_MMC0_BASE},
	{"isp", SYSMGR_ISP_BASE},
	{"eth", SYSMGR_MAC_BASE},
	{"mipi-csi", SYSMGR_MIPI_BASE},
	{"g1", SYSMGR_VDEC264_BASE},
	{"h1", SYSMGR_VENC264_BASE},
	{"g2", SYSMGR_VDEC265_BASE},
	{"h2", SYSMGR_VENC265_BASE},

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

static struct module_info power_info[] = {
	{"isp", SYSMGR_ISP_BASE},
	{"mipi", SYSMGR_MIPI_BASE},
	{"hdmi", SYSMGR_IDS1_BASE},/*ids1*/
	{"crypto", SYSMGR_CRYPTO_BASE},
	{"g1", SYSMGR_VDEC264_BASE},
	{"g2", SYSMGR_VDEC265_BASE},
	{"h1", SYSMGR_VENC264_BASE},
	{"h2", SYSMGR_VENC265_BASE},
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
#ifdef CONFIG_INFT_CAR_ACC_ENABLE	
#include <linux/gpio.h>
#include <linux/delay.h>
#define CAR_ACC_GPIO       154 
static ssize_t car_acc_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int ret = 0;
	int value =0;

	
	ret = gpio_request(CAR_ACC_GPIO, "car_acc");
	if (ret) {
		printk("failed request gpio for car_acc_show\n");
	}
	
	gpio_direction_input(CAR_ACC_GPIO);
	msleep(300);
	value=gpio_get_value(CAR_ACC_GPIO);
	msleep(5);
	gpio_free(CAR_ACC_GPIO);
	pr_err("get car acc state=%d\n",value);
	return value;
}
#endif
static DEVICE_ATTR(isp, 0666, isp_domain_show, isp_domain_store);
static DEVICE_ATTR(mipi, 0666, mipi_domain_show, mipi_domain_store);
static DEVICE_ATTR(hdmi, 0666, hdmi_domain_show, hdmi_domain_store);
static DEVICE_ATTR(crypto, 0666, crypto_domain_show, crypto_domain_store);
static DEVICE_ATTR(g1, 0666, g1_domain_show, g1_domain_store);
static DEVICE_ATTR(h1, 0666, h1_domain_show, h1_domain_store);
static DEVICE_ATTR(g2, 0666, g2_domain_show, g2_domain_store);
static DEVICE_ATTR(h2, 0666, h2_domain_show, h2_domain_store);
static DEVICE_ATTR(domain, 0444, power_domain_show, NULL);
static DEVICE_ATTR(iis, 0666, iis_mem_show, iis_mem_store);
static DEVICE_ATTR(ctimer, 0666, cmn_timer_mem_show, cmn_timer_mem_store);
static DEVICE_ATTR(pwm, 0666, pwm_mem_show, pwm_mem_store);
static DEVICE_ATTR(iic, 0666, iic_mem_show, iic_mem_store);
static DEVICE_ATTR(pcm, 0666, pcm_mem_show, pcm_mem_store);
static DEVICE_ATTR(ssp, 0666, ssp_mem_show, ssp_mem_store);
static DEVICE_ATTR(codec, 0666, codec_mem_show, codec_mem_store);
static DEVICE_ATTR(gpio, 0666, gpio_mem_show, gpio_mem_store);
static DEVICE_ATTR(wdt, 0666, wdt_mem_show, wdt_mem_store);
static DEVICE_ATTR(usbhost, 0666, usbh_mem_show, usbh_mem_store);
static DEVICE_ATTR(usbotg, 0666, otg_mem_show, otg_mem_store);
static DEVICE_ATTR(sd1, 0666, sd1_mem_show, sd1_mem_store);
static DEVICE_ATTR(sd2, 0666, sd2_mem_show, sd2_mem_store);
static DEVICE_ATTR(sd0, 0666, sd0_mem_show, sd0_mem_store);
static DEVICE_ATTR(eth, 0666, eth_mem_show, eth_mem_store);
static DEVICE_ATTR(mem, 0444, module_memory_show, NULL);
#ifdef CONFIG_INFT_CAR_ACC_ENABLE	
static DEVICE_ATTR(caracc, 0666, car_acc_show, NULL);
#endif
static struct attribute *apollo_power_domain_attributes[] = {
	&dev_attr_isp.attr,
	&dev_attr_mipi.attr,
	&dev_attr_hdmi.attr,
	&dev_attr_crypto.attr,
	&dev_attr_g1.attr,
	&dev_attr_h1.attr,
	&dev_attr_g2.attr,
	&dev_attr_h2.attr,
	&dev_attr_domain.attr,
	&dev_attr_iis.attr,
	&dev_attr_ctimer.attr,
	&dev_attr_pwm.attr,
	&dev_attr_iic.attr,
	&dev_attr_pcm.attr,
	&dev_attr_ssp.attr,
	&dev_attr_codec.attr,
	&dev_attr_gpio.attr,
	&dev_attr_wdt.attr,
// changed by linyun.xiong@2015-09-04 ???  for test
//	&dev_attr_usbhost.attr,
//	&dev_attr_usbotg.attr,
	&dev_attr_sd1.attr,
	&dev_attr_sd2.attr,
	&dev_attr_sd0.attr,
	&dev_attr_eth.attr,
	&dev_attr_mem.attr,
#ifdef CONFIG_INFT_CAR_ACC_ENABLE
	&dev_attr_caracc.attr,//xiongbiao
#endif	
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
