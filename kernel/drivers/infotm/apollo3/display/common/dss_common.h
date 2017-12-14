/*
 * dss_common.h - display common defination
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */
#include <linux/jiffies.h> /*for jiffies*/
#include <mach/items.h>
#include <dss_item.h>
#include <dss_log.h>
#include <dss_time.h>

#ifndef __DISPLAY_COMMON_HEADER__
#define __DISPLAY_COMMON_HEADER__

#define LCD_VIC	2000

#define DSS_IDS0 0
#define DSS_IDS1 1

#define DSS_IDS_OSD0 0
#define DSS_IDS_OSD1 1

#define DSS_DISABLE 0
#define DSS_ENABLE 1

#define DSS_INTERFACE_LCDC		(1)
#define DSS_INTERFACE_TVIF		(2)
#define DSS_INTERFACE_I80		(3)
#define DSS_INTERFACE_HDMI		(4)
#define DSS_INTERFACE_DSI		(5)
#define DSS_INTERFACE_SRGB	(6)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define IRQ_TVINT		(1<<7)
#define IRQ_OSDW2INT	(1<<6)
#define IRQ_I80INT		(1<<5)
#define IRQ_OSDERR		(1<<4)
#define IRQ_OSDW1INT	(1<<3)
#define IRQ_OSDW0INT	(1<<2)
#define IRQ_VCLKINT		(1<<1)
#define IRQ_LCDINT		(1<<0)

#define IMAP_IDS_0_CLK "imap-ids.0"
#define IMAP_IDS0_OSD_CLK "imap-ids0-osd"
#define IMAP_IDS0_EITF_CLK "imap-ids0-eitf"
#define IMAP_IDS0_PARENT_CLK "imap-ids0"

#define IMAP_IDS_1_CLK "imap-ids.1"
#define IMAP_IDS1_OSD_CLK "imap-ids1-osd"
#define IMAP_IDS1_EITF_CLK "imap-ids1-eitf"
#define IMAP_IDS1_PARENT_CLK "imap-ids1"

enum lcd_module_vic {//don not redefined in vic.c
	LCD_VIC_START = 3000,
	LCD_VIC_KR070PB2S_800_480,	//3001
	LCD_VIC_KR070LB0S_1024_600,	//3002
	LCD_VIC_KR079LA1S_768_1024,	//3003
	LCD_VIC_GM05004001Q_800_480,	//3004
	LCD_VIC_TXDT200LER_720_576,	//3005
	LCD_VIC_OTA5180A_480_272,		//3006
	LCD_VIC_ILI9806S_480_800,		//3006
	LCD_VIC_ILI8961_360_240,
	LCD_VIC_OTA5180A_PAL,
	LCD_VIC_IP2906_PAL,
	LCD_VIC_DONGLE,
};

enum product_types {
	PRODUCT_MID,
	PRODUCT_BOX,
	PRODUCT_DONGLE,
};

enum board_types {
	BOARD_LCD1,
	BOARD_LCD2,
	BOARD_MIPI,
};

enum rgb_bpps {
	ARGB0888,
	RGB888,
	RGB565,
	RGB666,
};

enum rgb_sequences {
	SEQ_RGB,
	SEQ_RBG,
	SEQ_GRB,
	SEQ_GBR,
	SEQ_BRG,
	SEQ_BGR,
};

enum attribute_types {
	ATTR_ENABLE,
	ATTR_VIC,
	ATTR_GET_CONNECT,
	ATTR_POSITION = 3,
	ATTR_MULTI_LAYER,
	ATTR_BL_BRIGHTNESS,

	/* Internal attributes */
	ATTR_INTERNAL,
	ATTR_RGB_BPP = 7,
	ATTR_DMA_ADDR,
	ATTR_SWAP,
	ATTR_GET_RGB_SEQ,
	ATTR_SET_RGB_SEQ,
	ATTR_GET_RGB_BPP,
	ATTR_SET_RGB_BPP,
	ATTR_GET_BT656,
	ATTR_SET_BT656,

	ATTR_END,
};

enum lcd_power_sequences {
	LCDPWR_MASTER_EN,
	LCDPWR_OUTPUT_EN,
	LCDPWR_DVDD_EN,
	LCDPWR_AVDD_EN,
	LCDPWR_VGH_EN,
	LCDPWR_VGL_EN,
	LCDPWR_BACKLIGHT_EN,
	LCDPWR_END,
};

enum ids_input_pixel_format {
	ARGB8888 = 0,
	NV12,
	NV21,
	_IDS_RGB565,
	_IDS_RGB888,
	NOTPixel,
};

enum srgb_iftype {
	SRGB_IF_RGBDUMMY,
	SRGB_IF_DUMMYRGB,
	SRGB_IF_RGB,
};

enum external_display_type {
	TFT_LCD_DISPLAY,
	I80_LCD_DISPLAY,
	EXT_TV_DISPLAY,
};

enum osd_image_format {
	 OSD_IMAGE_PAL_BPP1  =0,
	OSD_IMAGE_PAL_BPP2 =1,
	OSD_IMAGE_PAL_BPP4 =2,
	OSD_IMAGE_PAL_BPP8  =3,
	OSD_IMAGE_RGB_BPP8_1A232 =4,
	OSD_IMAGE_RGB_BPP16_565 =5,
	OSD_IMAGE_RGB_BPP16_1A555 =6,
	OSD_IMAGE_RGB_BPP16_I555  =7,
	OSD_IMAGE_RGB_BPP18_666  =8,
	OSD_IMAGE_RGB_BPP18_1A665 =9,
	OSD_IMAGE_RGB_BPP19_1A666 =10,
	OSD_IMAGE_RGB_BPP24_888  =11,
	OSD_IMAGE_RGB_BPP24_1A887 =12,
	OSD_IMAGE_RGB_BPP25_1A888 =13,
	OSD_IMAGE_RGB_BPP28_4A888 =14,
	OSD_IMAGE_RGB_BPP16_4A444 =15,
	OSD_IMAGE_RGB_BPP32_8A888  =16,
	OSD_IMAGE_YUV_420SP =17,
	OSD_IMAGE_RGB_BPP32_888A  =18,
	OSD_IMAGE_RGB_BPP16_555A =19,
	OSD_IMAGE_RGB_BPP16_555I =20,
	OSD_IMAGE_PAL_DEFAULT =0xff,
};

struct module_node {
	char * name;				/* module name */
	int terminal;				/* is a terminal */
	struct list_head serial;
	struct list_head parallel;
	char * parallel_select;		/* current active parallel module */
	int (*limitation)(void);
	int (*attr_ctrl)(struct module_node * module, int attr, int *param, int num);	/* attributes manager */
	struct module_attr *attributes;	/* attributes */
	int idsx;					/* module path index */
	int (*init)(int idsx);			/* module init */
	int (*obtain_info)(int idsx);	/* obtain info after init */
};

struct module_attr {
	int exist;
	int type;
	int (*func)(int idsx, int *params, int num);
};

#ifndef DTD_TYPE
#define DTD_TYPE

typedef struct {
	u16 mCode;	// VIC
	u16 mPixelRepetitionInput;
	u16 mPixelClock;
	u8 mInterlaced;
	u16 mHActive;
	u16 mHBlanking;
	u16 mHBorder;
	u16 mHImageSize;
	u16 mHSyncOffset;
	u16 mHSyncPulseWidth;
	u8 mHSyncPolarity;
	u8 mVclkInverse;
	u16 mVActive;
	u16 mVBlanking;
	u16 mVBorder;
	u16 mVImageSize;
	u16 mVSyncOffset;
	u16 mVSyncPulseWidth;
	u8 mVSyncPolarity;
} dtd_t;
#endif

struct lcd_gpio_control {
	int back_light_gpio;
	int back_light_polarity;
	int back_light_pwm;
	/* Optional */
	int master_gpio;
	int master_polarity;
	int dvdd_gpio;
	int dvdd_polarity;
	int avdd_gpio;
	int avdd_polarity;
	int vgh_gpio;
	int vgh_polarity;
	int vgl_gpio;
	int vgl_polarity;
};

typedef int (*lcd_power_control)(int enable);

struct lcd_power_sequence {
	int operation;
	int delay;	// ms
};

struct lcd_panel_param {
	char * name;
	dtd_t dtd;
	int rgb_seq;
	int rgb_bpp;
	struct lcd_power_sequence * power_seq;
	int power_seq_num;				// be convient for reverse enumerate
	int (*lcd_enable)(int enable);		// Specific enable sequence
	int (*power_specific)(int operation, int enable);	// Specific power operation
};

/* abstract terminal device */
struct abstract_terminal {
	char *name;
	int idsx;
	int *attrs;		// available attributes
	int attr_num;		// available attributes num
	int rgb_seq;
	int rgb_bpp;
	int bt656;
	int connect;
	int main_sink;
	int enabled;
	int vic;
};

#define assert_num(x)	\
	if (num != x) { dss_err("assert_num(%d) failed. num = %d.\n", x, num); return -1;}

extern int vic_fill(dtd_t *dtd, u32 code, u32 refreshRate);

#define ILI8961_SPI_DEVICE_NAME "ILI8961_360_240"
#define OTA5182A_SPI_DEVICE_NAME "OTA5180A_PAL"
#define IP2906_I2C_DEVICE_NAME "IP2906_PAL"

struct ids_core {
	int idsx;
	int lcd_interface;//=0;

	int srgb_iftype;
	int dev_reset;
	char dev_name[64];
	int bt656_pad_mode;

	unsigned int main_VIC;//=0; 	/* what meaning about main_VIC? it may be osd configs*/
	unsigned int lcd_vic;//=0; 	/* lcd input(soc display module output) paremeters */
	unsigned int prescaler_vic;//=0;	 /* prescaler input parameters */
	unsigned int main_scaler_factor;

	int cvbs_style;
	int blanking;

	int ubootlogo_status;

	int rgb_order;
	int rgb_bpp;

	int wait_irq_timeout;

	/*get lcdc frame of refresh rate*/
	u32 fps;
	u32 frame_last_cnt;
	u32 frame_cur_cnt;
	u32 last_timestamp; /* us */
	u32 cur_timestamp; /* us */

	/*Note: for UI Layer*/
	u32 colorkey;
	u32 alpha;
	u32 osd1_en;

	/*for logo*/
	int logo_type;

	int osd0_rbexg;
	int osd1_rbexg;

	u32 osd_clk_bak;
	u32 eitf_clk_bak;
};

enum __LOGO_TYPE {
	LOGO_360_240 = 0,
	LOGO_480_272_RAW,
	LOGO_720_576,
	LOGO_720_480_RAW,
	LOGO_800_480,
	LOGO_1024_600,
};

extern struct ids_core core;

/*for Test macro*/
//#define TEST_NO_PRESCALER_RGB 1
//#define TEST_PRESCALER_RGB 1
//#define TEST_NO_PRESCALER_NV 1
#define TEST_2_OSD_BLENDING 1

//#define TEST_LOGO_1024_600_BMP 1
//#define TEST_LOGO_800_480_BMP 1
#define TEST_LOGO_360_240_BMP 1
//#define TEST_LOGO_480_272_RAW 1
//#define TEST_LOGO_720_576_BMP 1
//#define TEST_LOGO_720_480_RAW 1

#endif
