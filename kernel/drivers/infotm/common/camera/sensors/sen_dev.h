#include <linux/i2c.h>

struct i2c_board_info  imap_camera_info[] =
{
	[0] = {
		.type = "gc0308_demo",
		.addr = 0x42 >> 1,
	},
	[1] = {
		.type = "gc2035_demo",
		.addr = 0x78 >> 1,
	},
	[2] = {
		.type = "gc0329_front",
		.addr = (0x62 >> 1),
	},
	[3] = {
		.type = "gc0329_rear",
		.addr = (0x62 >> 1) | (0x80),
		.flags = I2C_CLIENT_TEN,
	},
	[4] = {
		.type = "ar330mipi",
		.addr = (0x20 >> 1),
	},
    [5] = {
         .type = "xc9080",
         .addr = (0x36 >> 1),
    }
};

#define N_SUPPORT_SENSORS ARRAY_SIZE(imap_camera_info)
