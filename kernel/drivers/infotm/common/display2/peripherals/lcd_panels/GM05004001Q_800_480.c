/* display lcd panel params */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/display_device.h>

struct peripheral_param_config GM05004001Q_RGB_800_480 = {
	.name = "GM05004001Q_RGB_800_480",
	.interface_type = DISPLAY_INTERFACE_TYPE_PARALLEL_RGB888,
	.rgb_order = DISPLAY_LCD_RGB_ORDER_RGB,
	.control_interface = DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE,
	.dtd = {
		.mCode = DISPLAY_SPECIFIC_VIC_PERIPHERAL,
		.mHActive = 800,
		.mVActive = 480,
		.mHBlanking = 256,
		.mVBlanking = 45,
		.mHSyncOffset = 210,
		.mVSyncOffset = 22,
		.mHSyncPulseWidth = 30,
		.mVSyncPulseWidth = 13,
		.mHSyncPolarity = 1,
		.mVSyncPolarity = 1,
		.mVclkInverse	= 1,
		.mPixelClock = 33264,		// 60 fps  33264
		.mInterlaced = 0,
	},
};

MODULE_DESCRIPTION("display specific lcd panel param driver");
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_LICENSE("GPL v2");
