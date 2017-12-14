#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <mach/imap-iic.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <mach/items.h>

//#define I2C_DEBUG
#ifdef I2C_DEBUG
#define i2c_dbg(x...)	printk(KERN_ERR "IIC INFO:" x)
#else
#define i2c_dbg(x...)
#endif

#define IMAPX800_PAD_CFG_ADDR	(0x21e09000)
#define IIC_XFER_CMD	(0x0 << 8)
#define DEFAULT_SLAVE_ADDR	(0x88 >> 1)

int slave_addr;


static char *abort_sources[] = {
	[ABRT_7B_ADDR_NOACK] =
	    "the address sent was not acknowledged by any slave(7-bit mode)",
	[ABRT_10BADDR1_NOACK] =
	    "the first 10-bit address byte was not acknowledged by any slave(10-bit mode)",
	[ABRT_10BADDR2_NOACK] =
	    "the second address byte of 10-bit address was not acknowledged by any slave(10-bit mode)",
	[ABRT_TXDATA_NOACK] = "data was not acknowledged",
	[ABRT_GCALL_NOACK] = "no acknowledgement for a general call",
	[ABRT_GCALL_READ] = "read after general call",
	[ABRT_HS_ACKDET] = "the high speed master code was acknowledged",
	[ABRT_SBYTE_ACKDET] = "start byte was acknowledged",
	[ABRT_HS_NORSTRT] =
	    "trying to use the master to transfer data in high speed mode when restart was disabled",
	[ABRT_SBYTE_NORSTRT] =
	    "trying to send start byte when restart is disabled",
	[ABRT_10B_RD_NORSTRT] =
	    "trying to read when restart is disabled (10bit mode)",
	[ABRT_MASTER_DIS] = "trying to use disabled adapter",
	[ARB_LOST] = "lost arbitration",
	[ABRT_SLVFLUSH_TXFIFO] =
	    "slave received read command when data exist in TX FIFO",
	[ABRT_SLV_ARBLOST] = "slave lost bus when transmitting data",
	[ABRT_SLVRD_INTX] =
	    "write 1 to bit8 of IIC_DATA_CMD register when processor responds to a slave mode request for data to be transmit",
};

#define MAX_DATA_SIZE	0xff
/*
 * struct ima_i2c_dev - private imapx800_i2c data
 * @dev: driver model device node
 * @base: IO registers pointer
 * @abort_source: copy of the TX_ABRT_SOURCE register
 * @irq: interrupt number for the i2c master
 * @adapter: i2c subsystem adapter node
 * @tx_fifo_depth: depth of the hardware tx fifo
 * @rx_fifo_depth: depth of the hardware rx fifo
 */
struct imapx_i2c_slave_dev {
	struct device *dev;
	void __iomem *base;
	uint8_t read_buf[MAX_DATA_SIZE];
	uint32_t buf_index;
	int read_cmd;
	uint32_t abort_source;
	int irq;
	int suspended;

	int id;
};

static void imapx_i2c_reg_init(struct imapx_i2c_slave_dev *dev)
{
	uint32_t i2c_con = 0;
	uint16_t hcnt;
	uint16_t lcnt;

	writel(0, dev->base + IIC_ENABLE);

	writel(slave_addr, dev->base + IIC_SAR);

	writel(0, dev->base + IIC_RX_TL);

	i2c_con = IMAP_IIC_CON_MASTER_DISALBE | IMAP_IIC_CON_SLAVE_ENABLE;
	writel(i2c_con, dev->base + IIC_CON);
	writel(1, dev->base + IIC_ENABLE);
}

static int imapx_i2c_handle_tx_abort(struct imapx_i2c_slave_dev *dev)
{
	unsigned long abort_source = dev->abort_source;
	int i;

	if (abort_source & IMAP_IIC_TX_ABRT_NOACK) {
		for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
		    pr_info("%s: %s\n", __func__, abort_sources[i]);
		return -EREMOTEIO;
	}

	for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
	    pr_info("%s: %s\n", __func__, abort_sources[i]);

	if (abort_source & IMAP_IIC_TX_ARB_LOST)
		return -EAGAIN;
	else if (abort_source & IMAP_IIC_TX_ABRT_GCALL_READ)
		return -EINVAL;
	else
		return -EIO;
}

static uint32_t imapx_i2c_read_clear_intrbits(struct imapx_i2c_slave_dev *dev)
{
	uint32_t stat;

	stat = readl(dev->base + IIC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts,
	 * instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & IMAP_IIC_INTR_RX_UNDER)
		readl(dev->base + IIC_CLR_RX_UNDER);
	if (stat & IMAP_IIC_INTR_RX_OVER)
		readl(dev->base + IIC_CLR_RX_OVER);
	if (stat & IMAP_IIC_INTR_TX_OVER)
		readl(dev->base + IIC_CLR_TX_OVER);
	if (stat & IMAP_IIC_INTR_RD_REQ)
		readl(dev->base + IIC_CLR_RD_REQ);
	if (stat & IMAP_IIC_INTR_TX_ABRT) {
		/* read IIC_CLR_TX_ABRT register will clear IIC_TX_ABRT_SOURCE
		 * register,so we have to read IIC_TX_ABRT_SOURCE first*/
		dev->abort_source = readl(dev->base + IIC_TX_ABRT_SOURCE);
		readl(dev->base + IIC_CLR_TX_ABRT);
	}
	if (stat & IMAP_IIC_INTR_RX_DONE)
		readl(dev->base + IIC_CLR_RX_DONE);
	if (stat & IMAP_IIC_INTR_ACTIVITY)
		readl(dev->base + IIC_CLR_ACTIVITY);
	if (stat & IMAP_IIC_INTR_STOP_DET)
		readl(dev->base + IIC_CLR_STOP_DET);
	if (stat & IMAP_IIC_INTR_START_DET)
		readl(dev->base + IIC_CLR_START_DET);
	if (stat & IMAP_IIC_INTR_GEN_CALL)
		readl(dev->base + IIC_CLR_GEN_CALL);

	return stat;
}

static uint8_t xfer_buf[MAX_DATA_SIZE];

static void imapx_i2c_slave_xfer_data(struct imapx_i2c_slave_dev *dev)
{
	if (dev->buf_index > MAX_DATA_SIZE) {
		printk("read too much\n");
		dev->read_cmd = 0;
		dev->buf_index = 0;
		return;
	}
	writel(IIC_XFER_CMD | xfer_buf[dev->read_cmd], dev->base + IIC_DATA_CMD);
	dev->read_cmd++;
	dev->buf_index++;
}

static void imapx_i2c_slave_read(struct imapx_i2c_slave_dev *dev)
{

	while(readl(dev->base + IIC_RXFLR))
	{
		if (dev->buf_index == 0) {
			dev->read_cmd = readl(dev->base + IIC_DATA_CMD);
		}
		else
		{
		//	dev->read_buf[dev->buf_index] = readl(dev->base + IIC_DATA_CMD);
			xfer_buf[dev->read_cmd] = readl(dev->base + IIC_DATA_CMD);
			dev->read_cmd++;
		}

		dev->buf_index++;

		if(dev->buf_index > MAX_DATA_SIZE + 1) {
			printk("receive buffer overflow\n");
			dev->buf_index = 0;
			dev->read_cmd= 0;
			break;
		}
	}
	writel(readl(dev->base + IIC_INTR_MASK) | IMAP_IIC_INTR_RD_REQ,
	       dev->base + IIC_INTR_MASK);
}

static irqreturn_t imapx_i2c_slave_isr(int irq, void *dev_id)
{
	struct imapx_i2c_slave_dev *dev = dev_id;
	uint32_t stat;

	stat = imapx_i2c_read_clear_intrbits(dev);
	//pr_info("%s line %d stat %x\n", __func__, __LINE__, stat);

	if (stat & IMAP_IIC_INTR_TX_ABRT) {
		pr_err("%s: irq=%d stat=0x%x\n", __func__, irq, stat);
		imapx_i2c_handle_tx_abort(dev);
		writel(0, dev->base + IIC_INTR_MASK);
		return IRQ_HANDLED;
	}

	if (stat & IMAP_IIC_INTR_STOP_DET) {
		dev->buf_index = 0;
		dev->read_cmd = 0;
		writel(readl(dev->base + IIC_INTR_MASK) & ~IMAP_IIC_INTR_RD_REQ,
		       dev->base + IIC_INTR_MASK);
	}

	if (stat & IMAP_IIC_INTR_RD_REQ)
		imapx_i2c_slave_xfer_data(dev);

	if (stat & IMAP_IIC_INTR_RX_FULL)
		imapx_i2c_slave_read(dev);

	return IRQ_HANDLED;
}

static int imapx_i2c_module_init(int num)
{
	int ret = -1;
	switch (num) {
	case 0:
		ret = imapx_pad_init("iic0");
		break;

	case 1:
		ret = imapx_pad_init("iic1");
		break;

	case 2:
		ret = imapx_pad_init("iic2");
		break;

	case 3:
		ret = imapx_pad_init("iic3");
		break;
    case 4:
		ret = imapx_pad_init("iic4");
		break;
	}

	return ret;
}

static int __init imapx_i2c_slave_probe(struct platform_device *pdev)
{
	struct imapx_i2c_slave_dev *dev;
	struct resource *mem, *ioarea;
	int irq, ret, i;

	pr_info("+++%s invoked by %p+++ dev id %d\n", __func__, pdev,
	       pdev->id);

	/* driver uses the static register mapping */
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		pr_err("no mem resource!\n");
		return -EINVAL;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		pr_err("no irq resource!\n");
		return irq;
	}

	ioarea = request_mem_region(mem->start, resource_size(mem), pdev->name);
	if (!ioarea) {
		pr_err("I2C region already claimed!\n");
		return -EBUSY;
	}

	dev = kzalloc(sizeof(struct imapx_i2c_slave_dev), GFP_KERNEL);
	if (!dev) {
		ret = -ENOMEM;
		goto release_region;
	}

	dev->dev = get_device(&pdev->dev);
	dev->irq = irq;
	platform_set_drvdata(pdev, dev);

	if (imapx_i2c_module_init(pdev->id) == -1) {
		pr_info("imapx800 i2c module init failed, please sure case configure\n");
		ret = -EINVAL;
		goto free_mem;
	}

	dev->base = ioremap(mem->start, resource_size(mem));
	if (dev->base == NULL) {
		pr_err("failure mapping io resources!\n");
		ret = -EBUSY;
		goto release_region;
	}

	dev->id = pdev->id;

	for (i = 0; i < MAX_DATA_SIZE; i++) {
		xfer_buf[i] = i;
	}

	if (item_exist("iic.inner.test")) {
		slave_addr = item_integer("iic.slave.addr", 0);
	} else {
		slave_addr = DEFAULT_SLAVE_ADDR;
	}

	imapx_i2c_reg_init(dev);

	writel(IMAP_IIC_SLAVE_INTR_DEFAULT_MASK, dev->base + IIC_INTR_MASK);
	ret =
	    request_irq(dev->irq, imapx_i2c_slave_isr, IRQF_DISABLED, pdev->name,
			dev);
	if (ret) {
		pr_err("failure requesting irq %i!\n", dev->irq);
		goto iounmap;
	}

	return 0;

free_irq:
	free_irq(dev->irq, dev);

iounmap:
	iounmap(dev->base);

free_mem:
	platform_set_drvdata(pdev, NULL);
	put_device(&pdev->dev);
	kfree(dev);

release_region:
	release_mem_region(mem->start, resource_size(mem));

	return ret;
}

static int imapx_i2c_slave_remove(struct platform_device *pdev)
{
	struct imapx_i2c_slave_dev *dev = platform_get_drvdata(pdev);
	struct resource *mem;

	platform_set_drvdata(pdev, NULL);
	put_device(&pdev->dev);

	writel(0, dev->base + IIC_ENABLE);
	free_irq(dev->irq, dev);
	kfree(dev);

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(mem->start, resource_size(mem));

	return 0;
}

static int imapx_i2c_slave_suspend(struct platform_device *pdev,
				  pm_message_t msg)
{
	struct imapx_i2c_slave_dev *dev = platform_get_drvdata(pdev);

	dev->suspended = 1;

	return 0;
}

static int imapx_i2c_slave_resume(struct platform_device *pdev)
{
	struct imapx_i2c_slave_dev *dev = platform_get_drvdata(pdev);

	if (pdev->id == 0)
		module_power_on(SYSMGR_IIC_BASE);
	if (imapx_i2c_module_init(pdev->id) == -1) {
		pr_err("i2c-%d resume failed\n", pdev->id);
		return -1;
	}

	imapx_i2c_reg_init(dev);

	dev->suspended = 0;

	return 0;
}

MODULE_ALIAS("platform:imapx_i2c_slave");

static struct platform_driver imapx_i2c_slave_driver = {
	.probe = imapx_i2c_slave_probe,
	.remove = imapx_i2c_slave_remove,
	.suspend = imapx_i2c_slave_suspend,
	.resume = imapx_i2c_slave_resume,
	.driver = {
		   .name = "imap-iic-slave",
		   .owner = THIS_MODULE,
		   },
};

static int __init imapx_i2c_slave_init(void)
{
	return platform_driver_register(&imapx_i2c_slave_driver);
}

arch_initcall(imapx_i2c_slave_init);

static void __exit imapx_i2c_slave_exit(void)
{
	platform_driver_unregister(&imapx_i2c_slave_driver);
}

module_exit(imapx_i2c_slave_exit);

MODULE_AUTHOR("Baruch Siach <baruch@tkos.co.il>");
MODULE_DESCRIPTION("IMAPX800 I2C slave driver");
MODULE_LICENSE("GPL");
