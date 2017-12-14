/*
 * dss_common.h - display common defination
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 * This file is released under the GPLv2
 */




#if 0
// Temp for reference
#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define	EINTR		 4	/* Interrupted system call */
#define	EIO		 5	/* I/O error */
#define	ENXIO		 6	/* No such device or address */
#define	E2BIG		 7	/* Argument list too long */
#define	ENOEXEC		 8	/* Exec format error */
#define	EBADF		 9	/* Bad file number */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Device or resource busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define	ERANGE		34	/* Math result not representable */
#endif







#include <mach/items.h>
#include <dss_item.h>
#include <dss_log.h>
#include <dss_time.h>


#ifndef __DISPLAY_COMMON_HEADER__
#define __DISPLAY_COMMON_HEADER__


#define LCD_VIC	2000

#define DSS_INTERFACE_LCDC	(1)
#define DSS_INTERFACE_TVIF	(2)
#define DSS_INTERFACE_I80	(3)
#define DSS_INTERFACE_HDMI	(4)
#define DSS_INTERFACE_DSI	(5)

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

//extern int LCD_VIC;

enum lcd_module_vic {    //don not redefined in vic.c
	LCD_VIC_START = 3000,
	LCD_VIC_KR070PB2S_800_480,      //3001
	LCD_VIC_KR070LB0S_1024_600,     //3002
    LCD_VIC_KR079LA1S_768_1024,     //3003
    LCD_VIC_GM05004001Q_800_480,    //3004
	LCD_VIC_TXDT200LER_720_576,     //3005
	LCD_VIC_OTA5180A_480_272,     //3006
	LCD_VIC_ILI9806S_480_800,     //3006
	LCD_VIC_ST7567_132_65,     //3006
	LCD_VIC_DONGLE,
};
enum product_types {
	PRODUCT_MID,
	PRODUCT_BOX,
	PRODUCT_DONGLE,
};

extern int pt;

enum board_types {
	BOARD_LCD1,
	BOARD_LCD2,
	BOARD_MIPI,
	BOARD_I80,
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
	ATTR_POSITION,
	ATTR_MULTI_LAYER,
	ATTR_BL_BRIGHTNESS,

	/* Internal attributes */
	ATTR_INTERNAL,
	ATTR_RGB_BPP,
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
	int delay;		// ms
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
	if (num != x) {dss_err("assert_num(%d) failed. num = %d.\n", x, num); return -1;}


extern int vic_fill(dtd_t *dtd, u32 code, u32 refreshRate);



#endif
