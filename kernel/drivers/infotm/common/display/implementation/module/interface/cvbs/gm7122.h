#ifndef __GM7122_H__
#define __GM7122_H__

#include <linux/i2c.h>

#define GM7122_NAME 		"gm7122"
#define GM7122_I2C_NAME		"gm7122"
#define GM7122_I2C_ADDR		0x44
#define GM7122_NR	1
#define SYSFS_LINK_NAME	"cvbs"
#define P_CVBS				"smtv.ids.cvbs"
#define P_CODEC_MUTE		"codec.spk"
#define P_GM7122_RESET		"cvbs.reset"

#define CVBS_NTSC	"CVBS_NTSC"
#define CVBS_PAL	"CVBS_PAL"
#define CVBS_YPbPr	"CVBS_YPbPr"
#define HIGH true
#define LOW false

enum {
	f_PAL = 0,
	f_NTSC,
	f_YPbPr,		/* 720P. Future add 1080I */
	f_END
};

/* f_string is comparing to enum f_NTSC ... */
static char *f_string[] = {
	"CVBS_PAL",
	"CVBS_NTSC",
	"YPbPr",
	"Invalid"
};
struct gm7122 {
	struct i2c_client *client;
	int format;
	int gm7122_on;
	int i2c_forbidden;
	int reset_gpio;
	int codec_mute_gpio;
};

static int gm7122_suspend(struct device *);
static int gm7122_resume(struct device *);
static int gm7122_registers_dump(struct gm7122 *gm7122_data);
static int gm7122_init_registers(struct gm7122 *gm7122_data);
static int gm7122_disable_register(struct gm7122 *gm7122_data, int immediately);
static int gm7122_codec_mute(struct gm7122 * gm7122_data, int mute);
int gm7122_init(void);

#endif
