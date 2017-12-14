/* display chip params */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/display_device.h>
#include <mach/ip2906.h>

#define DISPLAY_MODULE_LOG		"ip2906"

/* before ip2906 chip fix CVBS plug/draw detection, do NOT open interrupt */
#define IRQ_PLUG		0
#define IRQ_DRAW	0

extern struct i2c_client *ip2906_client;

static int ip2906_i2c_read(struct i2c_client *client, unsigned char reg, unsigned short *data)
{
	int ret;
	unsigned char buf[2] = {0};
	struct i2c_msg msg[2];

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].buf = &reg;
	msg[0].len = 1;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = buf;
	msg[1].len = 2;

	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret < 0) {
		dlog_err("i2c_transfer read reg 0x%x failed %d\n", reg, ret);
		return -EIO;
	}

	*data = ((unsigned int)buf[0] << 8) | (unsigned int)buf[1];

	return 0;
}


static int ip2906_i2c_write(struct i2c_client *client, unsigned char reg, unsigned short data)
{
	int ret;
	uint8_t buf[3];
	struct i2c_msg msg;

	buf[0] = reg;
	buf[1] = (unsigned char)((data >> 8) & 0x00FF);
	buf[2] = (unsigned char)(data & 0x00FF);

	msg.addr = client->addr;
	msg.flags = 0;
	msg.buf = buf;
	msg.len = 3;

	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret < 0) {
		dlog_err("i2c_transfer write reg 0x%x failed %d\n", reg, ret);
		return -EIO;
	}

	return ret;
}

static int ip2906_i2c_writebits(struct i2c_client *client, unsigned char reg, int bit, int width, unsigned short data)
{
	unsigned short buf;
	ip2906_i2c_read(client, reg, &buf);
	buf &= (~(((1<<width)-1)<<bit));
	data &= ((1<<width)-1);
	buf |= (data<<bit);
	return ip2906_i2c_write(client, reg, buf);
}

static ssize_t brightness_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ip2906_i2c_read(pdev->i2c, IP2906_CVBS_BRIGHT_CTRL, &data);

	return sprintf(buf, "%d\n", data);
}

static ssize_t brightness_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ret = kstrtoul(buf, 0, &val);
	if (ret) {
		dlog_err("NOT a valid number\n");
		return ret;
	}

	if (val > 0xFF) {
		dlog_err("data 0x%X overflow\n", (int)val);
		return -EINVAL;
	}

	data = val;
	dlog_dbg("data=0x%X\n", data);

	ip2906_i2c_write(pdev->i2c, IP2906_CVBS_BRIGHT_CTRL, data);

	return count;
}

static ssize_t contrast_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ip2906_i2c_read(pdev->i2c, IP2906_CVBS_COLOR_CTRL, &data);

	return sprintf(buf, "%d\n", data & 0x0F);
}

static ssize_t contrast_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ret = kstrtoul(buf, 0, &val);
	if (ret) {
		dlog_err("NOT a valid number\n");
		return ret;
	}

	if (val > 0x0E) {
		dlog_err("data 0x%X overflow\n", (int)val);
		return -EINVAL;
	}

	data = val;
	dlog_dbg("data=0x%X\n", data);

	ip2906_i2c_writebits(pdev->i2c, IP2906_CVBS_COLOR_CTRL, 0, 4, data);

	return count;
}

static ssize_t saturation_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ip2906_i2c_read(pdev->i2c, IP2906_CVBS_COLOR_CTRL, &data);

	return sprintf(buf, "%d\n", (data >> 8) & 0x0F);
}

static ssize_t saturation_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ret = kstrtoul(buf, 0, &val);
	if (ret) {
		dlog_err("NOT a valid number\n");
		return ret;
	}

	if (val > 0x0E) {
		dlog_err("data 0x%X overflow\n", (int)val);
		return -EINVAL;
	}

	data = val;
	dlog_dbg("data=0x%X\n", data);

	ip2906_i2c_writebits(pdev->i2c, IP2906_CVBS_COLOR_CTRL, 8, 4, data);

	return count;
}

static ssize_t colorbar_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ip2906_i2c_read(pdev->i2c, IP2906_CVBS_CTRL, &data);

	return sprintf(buf, "%d\n", (data >> 9) & 0x01);
}

static ssize_t colorbar_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;
	unsigned short data;
	struct display_device *pdev = to_display_device(dev);

	ret = kstrtoul(buf, 0, &val);
	if (ret) {
		dlog_err("NOT a valid number\n");
		return ret;
	}

	if (val > 1) {
		dlog_err("data 0x%X overflow\n", (int)val);
		return -EINVAL;
	}

	data = val;
	dlog_dbg("data=0x%X\n", data);

	ip2906_i2c_writebits(pdev->i2c, IP2906_CVBS_CTRL, 9, 1, data);

	return count;
}

static struct device_attribute cvbs_attrs[] = {
	__ATTR(brightness, 0666, brightness_show, brightness_store),
	__ATTR(contrast, 0666, contrast_show, contrast_store),
	__ATTR(saturation, 0666, saturation_show, saturation_store),
	__ATTR(colorbar, 0666, colorbar_show, colorbar_store),
	__ATTR_NULL,
};

static int ip2906_init(struct display_device *pdev)
{
	int ret;
	int system_ok, audio_clk_en;
	unsigned short data;
	struct i2c_client *client;

	if (!pdev->i2c)
		pdev->i2c = ip2906_client;

	client = pdev->i2c;

	if (!client) {
		dlog_err("i2c_client NULL pointer\n");
		return -EINVAL;
	}
	
	ret = ip2906_i2c_read(client, IP2906_INT_CTRL, &data);
	if (ret) {
		dlog_err("i2c read error\n");
		return -EINVAL;
	}
	system_ok = (data>>2)&0x01;
	dlog_dbg("System %s OK\n", system_ok?"":"NOT");
	ip2906_i2c_read(client, IP2906_CMU_CLKEN, &data);
	audio_clk_en = data & 0x01;
	dlog_dbg("audio clk %s enable\n", audio_clk_en?"":"NOT");
	if (!system_ok || !audio_clk_en) {
		dlog_err("System is NOT OK or audio clk NOT enable.\n");
		return -EBUSY;
	}

	/* select bt656 interface ,Reg0x01 bit6/bit[3:0] */
	ip2906_i2c_read(client, IP2906_MFP_PAD, &data);
	data &= 0xFF00;
	data |= 0x4A;
	ip2906_i2c_write(client, IP2906_MFP_PAD, data);

	/* config CVBS PLL*/
	ip2906_i2c_write(client, IP2906_CMU_ANALOG, 0x0A53);

	/* CVBS clk enable */
	ip2906_i2c_writebits(client, IP2906_CMU_CLKEN, 2, 1, 1);

	/* CVBS reset */
	ip2906_i2c_write(client, IP2906_DEV_RST, 0x03);
	msleep(20);
	ip2906_i2c_write(client, IP2906_DEV_RST, 0x07);

	/* config BT clockphase and delay time */
	ip2906_i2c_write(client, IP2906_CVBS_CLK_CTRL, 0x00);

	/* config irq */
	ip2906_i2c_writebits(client, IP2906_CVBS_ANALOG_REG, 0, 1, IRQ_PLUG);
	ip2906_i2c_writebits(client, IP2906_CVBS_ANALOG_REG, 1, 1, IRQ_DRAW);
	/* here will also clear system ok pending */
	if (IRQ_PLUG || IRQ_DRAW)
		ip2906_i2c_writebits(client, IP2906_INT_CTRL, 8, 1, 0);
	else
		ip2906_i2c_writebits(client, IP2906_INT_CTRL, 8, 1, 1);
	ip2906_i2c_writebits(client, IP2906_CVBS_ANALOG_REG, 12, 2, 0x03);

	return 0;
}

static int ip2906_set_vic(struct display_device *pdev, int vic)
{
	struct i2c_client *client = pdev->i2c;
	int ntsc;
	unsigned short data;

	if (!client) {
		dlog_err("i2c_client NULL pointer\n");
		return -EINVAL;
	}

	if (vic == 6 || vic == 7)
		ntsc = 1;
	else if (vic == 21 || vic == 22)
		ntsc = 0;
	else {
		dlog_err("Invalid vic %d. NTSC vic is 6 or 7, PAL vic is 21 or 22\n", vic);
		return -EINVAL;
	}

	dlog_dbg("vic=%d, %s\n", vic, ntsc?"NTSC":"PAL");

	ip2906_i2c_read(client, IP2906_CVBS_CTRL, &data);
	data &= 0xFF00;
	if (ntsc)
		data |= 0;
	else
		data |= 0x24;
	ip2906_i2c_write(client, IP2906_CVBS_CTRL, data);

	return 0;
}


static int ip2906_enable(struct display_device *pdev, int en)
{
	struct i2c_client *client = pdev->i2c;

	dlog_dbg("%s\n", en?"enable":"disable");

	if (!client) {
		dlog_err("i2c_client NULL pointer\n");
		return -EINVAL;
	}

	ip2906_i2c_writebits(client, IP2906_CVBS_CTRL, 11, 1, en?1:0);
	ip2906_i2c_writebits(client, IP2906_CVBS_CTRL, 15, 1, en?1:0);

	msleep(20);

	return 0;
}

static int ip2906_irq_handler(struct display_device *pdev)
{
	struct i2c_client *client = pdev->i2c;
	unsigned short data;

	dlog_trace();

	ip2906_i2c_read(client, IP2906_INT_CTRL, &data);
	if (data & (1<<0))
		dlog_dbg("CVBS IRQ pending\n");
	if (data & (1<<1))
		dlog_dbg("Ethernet IRQ pending\n");
	if (data & (1<<2)) {
		dlog_dbg("System OK pending\n");
		ip2906_i2c_writebits(client, IP2906_INT_CTRL, 2, 1, 1);
	}
	
	ip2906_i2c_read(client, IP2906_CVBS_ANALOG_REG, &data);
	if (data & (1<<12))
		dlog_info("CVBS PLUG IN interrupt\n");
	if (data & (1<<13))
		dlog_info("CVBS DRAW OUT interrupt\n");

	if (data & (1<<12) || data & (1<<13))
		ip2906_i2c_write(client, IP2906_CVBS_ANALOG_REG, data);

	return 0;
}

struct peripheral_param_config CVBS_IP2906 = {
	.name = "CVBS_IP2906",
	.interface_type = DISPLAY_INTERFACE_TYPE_BT656,
	.bt656_pad_map = DISPLAY_BT656_PAD_DATA8_TO_DATA15,
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_I2C,
	.i2c_info = {
		I2C_BOARD_INFO("ip2906_cvbs", 0x75),
	},
	.init = ip2906_init,
	.set_vic = ip2906_set_vic,
	.enable = ip2906_enable,
	.irq_handler = ip2906_irq_handler,
	.default_vic = 22,
	//.osc_clk = 24000000,		osc clk is init by audio codec driver
	.attrs = cvbs_attrs,
};


MODULE_DESCRIPTION("display specific chip param driver");
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_LICENSE("GPL v2");
