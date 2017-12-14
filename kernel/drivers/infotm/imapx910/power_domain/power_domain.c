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

static ssize_t gpu_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *gpu_sysmgr = IO_ADDRESS(SYSMGR_GPU_BASE);

	val = readl(gpu_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t gpu_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_GPU_BASE);
	}else {
		module_power_down(SYSMGR_GPU_BASE);
	}

	return count;
}

static ssize_t vdec_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *vdec_sysmgr = IO_ADDRESS(SYSMGR_VDEC_BASE);

	val = readl(vdec_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t vdec_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_VDEC_BASE);
	}else {
		module_power_down(SYSMGR_VDEC_BASE);
	}
	return count;
}

static ssize_t venc_domain_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint8_t val;
	void __iomem *venc_sysmgr = IO_ADDRESS(SYSMGR_VENC_BASE);

	val = readl(venc_sysmgr + 0x8);
	return sprintf(buf, "%d\n", (val&0x2)?1:0);
}

static ssize_t venc_domain_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	uint8_t tmp;

	tmp = simple_strtoul(buf, NULL, 10);
	if(tmp) {
		module_power_on(SYSMGR_VENC_BASE);
	}else {
		module_power_down(SYSMGR_VENC_BASE);
	}
	return count;
}

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
	void __iomem *mipidsi_sysmgr = IO_ADDRESS(SYSMGR_IDS0_BASE);

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
		module_power_on(SYSMGR_IDS0_BASE);
	}else {
		module_power_down(SYSMGR_IDS0_BASE);
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
	{"spdif", SYSMGR_SPDIF_BASE},
	{"uart", SYSMGR_UART_BASE},
	{"ps2", SYSMGR_PIC_BASE},
	{"codec", SYSMGR_TQLP9624_BASE},
	{"gpio", SYSMGR_GPIO_BASE},
	{"watchdog", SYSMGR_PWDT_BASE},
	{"ids0", SYSMGR_IDS0_BASE},
	{"ids1", SYSMGR_IDS1_BASE},
	{"gpu", SYSMGR_GPU_BASE},
	{"usb-host", SYSMGR_USBH_BASE},
	{"usb-otg", SYSMGR_OTG_BASE},
	{"sd1", SYSMGR_MMC1_BASE},
	{"sd2", SYSMGR_MMC2_BASE},
	{"sd0", SYSMGR_MMC0_BASE},
	{"sd3", SYSMGR_MMC3_BASE},
	{"nand", SYSMGR_NAND_BASE},
	{"vdec", SYSMGR_VDEC_BASE},
	{"isp", SYSMGR_ISP_BASE},
	{"eth", SYSMGR_MAC_BASE},
	{"mipi-csi", SYSMGR_MIPI_BASE},
	{"venc", SYSMGR_VENC_BASE},
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

static DEVICE_ATTR(gpu, 0666, gpu_domain_show, gpu_domain_store);
static DEVICE_ATTR(vdec, 0666, vdec_domain_show, vdec_domain_store);
static DEVICE_ATTR(venc, 0666, venc_domain_show, venc_domain_store);
static DEVICE_ATTR(isp, 0666, isp_domain_show, isp_domain_store);
static DEVICE_ATTR(mipi, 0666, mipi_domain_show, mipi_domain_store);
static DEVICE_ATTR(hdmi, 0666, hdmi_domain_show, hdmi_domain_store);
static DEVICE_ATTR(mem, 0444, module_memory_show, NULL);

static struct attribute *imapx910_power_domain_attributes[] = {
	&dev_attr_gpu.attr,
	&dev_attr_vdec.attr,
	&dev_attr_venc.attr,
	&dev_attr_isp.attr,
	&dev_attr_mipi.attr,
	&dev_attr_hdmi.attr,
	&dev_attr_mem.attr,
	NULL
};

static const struct attribute_group imapx910_power_domain_attr_group = {
	.attrs = imapx910_power_domain_attributes,
};

static struct power_domain_dev *dev = NULL;

static int imapx910_power_domain_probe(struct platform_device *pdev)
{
	int ret;

	dev->kobj = kobject_create_and_add("power-domain", NULL);
	if (dev->kobj == NULL) {
		ret = -ENODEV;
		goto kobj_err;
	}

	ret = sysfs_create_group(&dev->pdev->dev.kobj,
					&imapx910_power_domain_attr_group);
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

static struct platform_driver imapx910_power_domain_driver = {
	.probe	= imapx910_power_domain_probe,
	.driver	= {
		.owner	= THIS_MODULE,
		.name	= "power_domain",
	},
};

static int __init imapx910_power_domain_init(void)
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

	ret = platform_driver_register(&imapx910_power_domain_driver);
	if (ret < 0) {
		pr_err("%s: register driver error\n", __func__);
		return ret;
	}

	return 0;
}

static void __exit imapx910_power_domain_exit(void)
{
	return;
}

module_init(imapx910_power_domain_init);
module_exit(imapx910_power_domain_exit);
MODULE_LICENSE("GPL");
