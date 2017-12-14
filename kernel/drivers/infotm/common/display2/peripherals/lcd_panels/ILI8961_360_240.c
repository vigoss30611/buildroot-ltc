/* display lcd panel params */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/display_device.h>
#include <linux/amba/pl022.h>
#include <mach/pad.h>


#define DISPLAY_MODULE_LOG		"ili8961"

static unsigned char s_brightness = 0x40;
static unsigned char s_constrast = 0x40;



static int ili8961_spi_write(struct spi_device *spidev, unsigned short data)
{
	u16 ret;
	u16 buf = data;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len		= 2,
		.tx_buf		= &buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	ret = spi_sync(spidev, &msg);
	if(ret)
		dlog_err("spi write failed %d\n", ret);

	return ret;
}

static ssize_t brightness_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", s_brightness);
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

	s_brightness = val;
	data = val;
	dlog_dbg("data=0x%X\n", data);
	data |= 0x0300;

	ili8961_spi_write(pdev->spi, data);

	return count;
}

static ssize_t contrast_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", s_constrast);
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

	if (val > 0xFF) {
		dlog_err("data 0x%X overflow\n", (int)val);
		return -EINVAL;
	}

	s_constrast = val;
	data = val;
	dlog_dbg("data=0x%X\n", data);
	data |= 0x0D00;

	ili8961_spi_write(pdev->spi, data);

	return count;
}

static ssize_t spi_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long data;
	struct display_device *pdev = to_display_device(dev);

	ret = kstrtoul(buf, 16, &data);
	if (ret) {
		dlog_err("NOT a valid HEX number\n");
		return ret;
	}

	if (data > 0xFFFF) {
		dlog_err("data 0x%X overflow\n", (int)data);
		return -EINVAL;
	}

	dlog_dbg("data=0x%X\n", (int)data);

	ili8961_spi_write(pdev->spi, data);

	return count;
}

static struct device_attribute spi_attrs[] = {
	__ATTR(brightness, 0666, brightness_show, brightness_store),
	__ATTR(contrast, 0666, contrast_show, contrast_store),
	__ATTR(spi, 0222, NULL, spi_store),
	__ATTR_NULL,
};

static int __ili8961_common_config(struct spi_device *spi)
{
	int ret;
	
	ret = ili8961_spi_write(spi, 0x055f); 
	if (ret) {
		dlog_err("spi write failed %d\n", ret);
		return ret;
	}
	/*Note:0x5[6]: global reset, 0: reset all resigster to default value*/
	ili8961_spi_write(spi, 0x051f);
	ili8961_spi_write(spi, 0x055f);

	ili8961_spi_write(spi, 0x01ac);

	ili8961_spi_write(spi, 0x2f69);

	ili8961_spi_write(spi, 0x1604); 	/* Auto set to gamma2.2 */
	ili8961_spi_write(spi, 0x0340); 	/* RGB brightness level control 40h: center(0) */
	ili8961_spi_write(spi, 0x0D40);	/*RGB contrast level setting */
	ili8961_spi_write(spi, 0x0e40); 	/*Red sub-pixel contrast level setting */
	ili8961_spi_write(spi, 0x0f40);		/*Red sub-pixel brightness level setting */
	ili8961_spi_write(spi, 0x1040);		/*Blue sub-pixel contrast level setting */
	ili8961_spi_write(spi, 0x1140);		/*Blue sub-pixel brightness level setting */

	return 0;
}

static int ili8961_serialrgbdummy_enable(struct display_device *pdev, int en)
{
	struct spi_device *spi = pdev->spi;
	int ret;

	dlog_dbg("%s\n", en?"enable":"disable");

	if (!spi) {
		dlog_err("spi_device NULL pointer\n");
		return -EINVAL;
	}

	if (en) {
		ret = __ili8961_common_config(spi);
		if (ret)
			return ret;
		ili8961_spi_write(spi, 0x0007);		/* disable CCIR601 */
		ili8961_spi_write(spi, 0x042B);		/* 8-bit RGB Dummy 360x240 */
		ili8961_spi_write(spi, 0x2B01);		/* normal */
	}
	 else {
		ili8961_spi_write(spi, 0x2B00);		/* standby */
	}

	msleep(20);

	return 0;
}

static int ili8961_bt656_enable(struct display_device *pdev, int en)
{
	struct spi_device *spi = pdev->spi;
	int ret;

	dlog_dbg("%s\n", en?"enable":"disable");

	if (!spi) {
		dlog_err("spi_device NULL pointer\n");
		return -EINVAL;
	}

	if (en) {
		ret = __ili8961_common_config(spi);
		if (ret)
			return ret;
		ili8961_spi_write(spi, 0x0007);		/* disable CCIR601 */
		ili8961_spi_write(spi, 0x044B);		/* CCIR656, PAL/NTSC auto detect */
		ili8961_spi_write(spi, 0x2B01);		/* normal */
	}
	 else {
		ili8961_spi_write(spi, 0x2B00);		/* standby */
	}

	msleep(20);

	return 0;
}

static int ili8961_bt601_enable(struct display_device *pdev, int en)
{
	struct spi_device *spi = pdev->spi;
	int ret;

	dlog_dbg("%s\n", en?"enable":"disable");

	if (!spi) {
		dlog_err("spi_device NULL pointer\n");
		return -EINVAL;
	}

	if (en) {
		ret = __ili8961_common_config(spi);
		if (ret)
			return ret;
		ili8961_spi_write(spi, 0x0047);		/* Enable CCIR601 */
		ili8961_spi_write(spi, 0x046B);		/* YUV720 */
		ili8961_spi_write(spi, 0x2B01);		/* normal */
	}
	 else {
		ili8961_spi_write(spi, 0x2B00);		/* standby */
	}

	msleep(20);

	return 0;
}

struct pl022_config_chip ili8961_chip_info = {
	.com_mode = POLLING_TRANSFER,
	.iface = SSP_INTERFACE_MOTOROLA_SPI,
	.hierarchy = SSP_MASTER,
	.slave_tx_disable = 1,
	.rx_lev_trig = SSP_RX_4_OR_MORE_ELEM,
	.tx_lev_trig = SSP_TX_4_OR_MORE_EMPTY_LOC,
	.ctrl_len = SSP_BITS_8,
	.wait_state = SSP_MWIRE_WAIT_ZERO,
	.duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
};

struct peripheral_param_config ILI8961_SerialRGBDummy_360_240 = {
	.name = "ILI8961_SerialRGBDummy_360_240",
	.interface_type = DISPLAY_INTERFACE_TYPE_SERIAL_RGB_DUMMY,
	.rgb_order = DISPLAY_LCD_RGB_ORDER_BGR,		// For SerialRGB, low byte is sent first
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_SPI,
	.dtd = {
		.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL,
		.mHActive = 360,
		.mVActive = 240,
		.mHBlanking = 276,
		.mVBlanking = 23,
		.mHSyncOffset = 35,
		.mVSyncOffset = 2,
		.mHSyncPulseWidth = 1,
		.mVSyncPulseWidth = 1,
		.mHSyncPolarity = 0,
		.mVSyncPolarity = 0,
		.mVclkInverse	= 0,
		.mPixelClock = 27000,		// 30 fps  27000
		.mInterlaced = 0,
	},
	.spi_bits_per_word = 16,
	.spi_info = {
		 .modalias = "lcd_panel_spi",
		 .max_speed_hz = 1000000,
		 .bus_num = 1,		/* select spi bus 1 */
		 .chip_select = 0,		/* only one device in spi1*/
		 .mode = SPI_CPHA | SPI_CPOL | SPI_MODE_3 |SPI_CS_HIGH,
		 .controller_data = &ili8961_chip_info,
		 .platform_data = NULL,
	},
	.enable = ili8961_serialrgbdummy_enable,
	.attrs = spi_attrs,
};

struct peripheral_param_config ILI8961_BT656NTSC_720_480 = {
	.name = "ILI8961_BT656NTSC_720_480",
	.interface_type = DISPLAY_INTERFACE_TYPE_BT656,
	.bt656_pad_map = DISPLAY_BT656_PAD_DATA0_TO_DATA7,
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_SPI,
	.dtd = {
		.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL,
		.mHActive = 720,			// mHActive and mVActive will be use frequently, so keep the same with buffer size
		.mVActive = 480,
		.mHBlanking = 276,		// for sav&eav, 268+8=276
		.mVBlanking = 22,		// floor(22.5)
		.mHSyncOffset = 32,
		.mVSyncOffset = 3,
		.mHSyncPulseWidth = 124,
		.mVSyncPulseWidth = 3,
		.mHSyncPolarity = 0,
		.mVSyncPolarity = 0,
		.mVclkInverse	= 0,
		.mPixelClock = 27000,		// 30 fps  27000
		.mInterlaced = 1,						// TVUNBA & TVLNBA = mVActive / 2 
		.mPixelRepetitionInput = 1,		// TVVLEN = mHActive * 2   ({Y,Cb},{Y,Cr})
	},
	.spi_bits_per_word = 16,
	.spi_info = {
		 .modalias = "lcd_panel_spi",
		 .max_speed_hz = 1000000,
		 .bus_num = 1,		/* select spi bus 1 */
		 .chip_select = 0,		/* only one device in spi1*/
		 .mode = SPI_CPHA | SPI_CPOL | SPI_MODE_3 |SPI_CS_HIGH,
		 .controller_data = &ili8961_chip_info,
		 .platform_data = NULL,
	},
	.enable = ili8961_bt656_enable,
	.attrs = spi_attrs,
};

struct peripheral_param_config ILI8961_BT656PAL_720_576 = {
	.name = "ILI8961_BT656PAL_720_576",
	.interface_type = DISPLAY_INTERFACE_TYPE_BT656,
	.bt656_pad_map = DISPLAY_BT656_PAD_DATA0_TO_DATA7,
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_SPI,
	.dtd = {
		.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL,
		.mHActive = 720,			// mHActive and mVActive will be use frequently, so keep the same with buffer size
		.mVActive = 576,
		.mHBlanking = 288,		// for sav&eav, 280+8=288
		.mVBlanking = 24,		// floor(22.5)
		.mHSyncOffset = 24,
		.mVSyncOffset = 2,
		.mHSyncPulseWidth = 126,
		.mVSyncPulseWidth = 3,
		.mHSyncPolarity = 0,
		.mVSyncPolarity = 0,
		.mVclkInverse	= 0,
		.mPixelClock = 27000,		// 30 fps  27000
		.mInterlaced = 1,						// TVUNBA & TVLNBA = mVActive / 2 
		.mPixelRepetitionInput = 1,		// TVVLEN = mHActive * 2   ({Y,Cb},{Y,Cr})
	},
	.spi_bits_per_word = 16,
	.spi_info = {
		 .modalias = "lcd_panel_spi",
		 .max_speed_hz = 1000000,
		 .bus_num = 1,		/* select spi bus 1 */
		 .chip_select = 0,		/* only one device in spi1*/
		 .mode = SPI_CPHA | SPI_CPOL | SPI_MODE_3 |SPI_CS_HIGH,
		 .controller_data = &ili8961_chip_info,
		 .platform_data = NULL,
	},
	.enable = ili8961_bt656_enable,
	.attrs = spi_attrs,
};

struct peripheral_param_config ILI8961_BT601_720_480 = {
	.name = "ILI8961_BT601_720_480",
	.interface_type = DISPLAY_INTERFACE_TYPE_BT601,
	.bt601_yuv_format = DISPLAY_YUV_FORMAT_CBYCRY,
	.bt601_data_width = 8,
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_SPI,
	.dtd = {
		.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL,
		.mHActive = 720,			// mHActive and mVActive will be use frequently, so keep the same with buffer size
		.mVActive = 480,
		.mHBlanking = 276,
		.mVBlanking = 22, 	// floor(22.5)
		.mHSyncOffset = 32,
		.mVSyncOffset = 3,
		.mHSyncPulseWidth = 124,
		.mVSyncPulseWidth = 3,
		.mHSyncPolarity = 0,
		.mVSyncPolarity = 0,
		.mVclkInverse = 0,
		.mPixelClock = 27000, 	// 30 fps  27000
		.mInterlaced = 1, 					// TVUNBA & TVLNBA = mVActive / 2 
		.mPixelRepetitionInput = 1, 	// TVVLEN = mHActive * 2	 ({Y,Cb},{Y,Cr})
	},
	.spi_bits_per_word = 16,
	.spi_info = {
		 .modalias = "lcd_panel_spi",
		 .max_speed_hz = 1000000,
		 .bus_num = 1,		/* select spi bus 1 */
		 .chip_select = 0,		/* only one device in spi1*/
		 .mode = SPI_CPHA | SPI_CPOL | SPI_MODE_3 |SPI_CS_HIGH,
		 .controller_data = &ili8961_chip_info,
		 .platform_data = NULL,
	},
	.enable = ili8961_bt601_enable,
	.attrs = spi_attrs,
};



MODULE_DESCRIPTION("display specific lcd panel param driver");
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_LICENSE("GPL v2");
