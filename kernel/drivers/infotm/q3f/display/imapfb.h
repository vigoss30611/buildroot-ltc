/***************************************************************************** 
** drivers/video/infotm/imapfb.h
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description: Head file of Display Controller.
**
** Author:
**     Sam Ye<weize_ye@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0.1	 Sam@2012/3/20 :  first commit
**
*****************************************************************************/

#ifndef _IMAPFB_H_
#define _IMAPFB_H_

#include <linux/interrupt.h>

/* Debug macros */
#define DEBUG 0

#if DEBUG
#define dbg_info(fmt, args...)	printk(KERN_INFO "%s: " fmt, __FUNCTION__ , ## args)
#else
#define dbg_info(fmt, args...)
#endif
#define dbg_err(fmt, args...)	printk(KERN_ERR "%s: " fmt, __FUNCTION__ , ## args)

/* Definitions */
#ifndef MHZ
#define MHZ 				(1000 * 1000)
#define PRINT_MHZ(m)	((m) / MHZ), ((m / 1000) % 1000)
#endif

/* Open/Close framebuffer window */
#define ON 	1
#define OFF	0

#define CONFIG_FB_IMAP_BUFFER_NUM 3  //the buffer number to sync

#define IMAPFB_MAX_NUM			2 /* max framebuffer device number */
#define IMAPFB_MAX_ALPHA_LEVEL	0xf	/* max alpha0/1 value */

#define IMAPFB_PALETTE_BUFF_CLEAR		0x80000000	/* palette entry is clear/invalid */

#define IMAPFB_COLOR_KEY_DIR_BG	0	/* match back-ground pixel in color key function */
#define IMAPFB_COLOR_KEY_DIR_FG	1	/* match fore-ground pixel in color key function */

#define IMAPFB_MIN_BACKLIGHT_LEVEL	0	/* default backlight brightness: 75% */
#define IMAPFB_DEFAULT_BACKLIGHT_LEVEL	0x2ff	/* default backlight brightness: 75% */

/* Macros */
#define FB_MIN_NUM(x, y)		((x) < (y) ? (x) : (y))
#define IMAPFB_NUM			IMAPFB_MAX_NUM

/* IO controls */
#define IMAPFB_OSD_START			_IO  ('F', 201)
#define IMAPFB_OSD_STOP			_IO  ('F', 202)
#define IMAPFB_SWITCH_ON 	_IO ('F', 207)
#define IMAPFB_SWITCH_OFF      _IO ('F', 208)
#ifdef CONFIG_LCD_ENABLE_IRQ
/*
 * This ioctl command added by Sololz.
 * This ioctl will block till the irq mark is set.
 */
#define IMAPFB_OSD_WAIT_IRQ		_IO('F', 203)
#endif	/* CONFIG_LCD_ENABLE_IRQ */

#define IMAPFB_OSD_FLUSH_LEVEL		_IO('F', 204)

#define IMAPFB_OSD_SET_POS_SIZE	_IOW ('F', 209, imapfb_win_pos_size)

#define IMAPFB_OSD_ALPHA0_SET		_IOW ('F', 210, UINT32)
#define IMAPFB_OSD_ALPHA1_SET		_IOW ('F', 211, UINT32)
//#define IMAPFB_CONFIG_VIDEO_SIZE 	_IOW ('F', 212, imapfb_video_size)
#define IMAPFB_CONFIG_VIDEO_SIZE     20005 
#define IMAPFB_LCD_SUSPEND           20006
#define IMAPFB_LCD_UNSUSPEND         20007

#define IMAPFB_COLOR_KEY_START			_IO  ('F', 300)
#define IMAPFB_COLOR_KEY_STOP				_IO  ('F', 301)
#define IMAPFB_COLOR_KEY_ALPHA_START		_IO  ('F', 302)
#define IMAPFB_COLOR_KEY_ALPHA_STOP		_IO  ('F', 303)
#define IMAPFB_COLOR_KEY_SET_INFO			_IOW ('F', 304, imapfb_color_key_info_t)

#define IMAPFB_POWER_SUPPLY_CHANGE	_IO ('F', 324)

#define IMAPFB_GET_LCD_EDID_INFO		_IOR ('F', 327, void*)
#define IMAPFB_LCD_DETECT_CONNECT	_IO ('F', 328)
#define IMAPFB_GET_BUF_PHY_ADDR		_IOR ('F', 329, imapfb_dma_info_t)

#define IMAPFB_GET_PHY_ADDR		0x121115
#define IMAPFB_GET_PMM_DEVADDR 			0x120514

/* Data type definition */
typedef unsigned long long	UINT64;
typedef unsigned int		UINT32;
typedef unsigned short		UINT16;
typedef unsigned char		UINT8;
typedef signed long long	SINT64;
typedef signed int			SINT32;
typedef signed short		SINT16;
typedef signed char		SINT8;

/******** Structures ********/
/* Framebuffer video size for window0 */
typedef struct {
	UINT32 width;
	UINT32 height;
	UINT32 win_width;
	UINT32 win_height;
	UINT32 scaler_left_x;
	UINT32 scaler_top_y;
	dma_addr_t scaler_buff;
	int format;
} imapfb_video_size;

/* Framebuffer window position and size */
typedef struct {
	SINT32 left_x;
	SINT32 top_y;
	UINT32 width;
	UINT32 height;
} imapfb_win_pos_size;

/* Color key function info. */
typedef struct {
	UINT32 direction;
	UINT32 colval;
} imapfb_color_key_info_t;

/* Framebuffer window video memory physical addr (4 buffers) */
typedef struct {
	dma_addr_t map_dma_f1;
	dma_addr_t map_dma_f2;
	dma_addr_t map_dma_f3;
	dma_addr_t map_dma_f4;
} imapfb_dma_info_t;

/* RGB and transparency bit field assignment */
typedef struct {
	struct fb_bitfield red;
	struct fb_bitfield green;
	struct fb_bitfield blue;
	struct fb_bitfield transp;
} imapfb_rgb_t;

const static imapfb_rgb_t imapfb_a1rgb232_8 = {
	.red		= {.offset = 5, .length = 2,},
	.green	= {.offset = 2, .length = 3,},
	.blue	= {.offset = 0, .length = 2,},
	.transp	= {.offset = 7, .length = 1,},
};

const static imapfb_rgb_t imapfb_rgb565_16 = {
	.red		= {.offset = 11, .length = 5,},
	.green	= {.offset = 5, .length = 6,},
	.blue	= {.offset = 0, .length = 5,},
	.transp	= {.offset = 0, .length = 0,},
};

const static imapfb_rgb_t imapfb_rgb666_18 = {
	.red		= {.offset = 12, .length = 6,},
	.green	= {.offset = 6, .length = 6,},
	.blue	= {.offset = 0, .length = 6,},
	.transp	= {.offset = 0, .length = 0,},
};

const static imapfb_rgb_t imapfb_a1rgb666_19 = {
	.red		= {.offset = 12, .length = 6,},
	.green	= {.offset = 6, .length = 6,},
	.blue	= {.offset = 0, .length = 6,},
	.transp	= {.offset = 18, .length = 1,},
};

const static imapfb_rgb_t imapfb_rgb888_24 = {
	.red		= {.offset = 16, .length = 8,},
	.green	= {.offset = 8, .length = 8,},
	.blue	= {.offset = 0, .length = 8,},
	.transp	= {.offset = 0, .length = 0,},
};

const static imapfb_rgb_t imapfb_a1rgb888_25 = {
	.red		= {.offset = 16, .length = 8,},
	.green	= {.offset = 8, .length = 8,},
	.blue	= {.offset = 0, .length = 8,},
	.transp	= {.offset = 24, .length = 1,},
};

const static imapfb_rgb_t imapfb_a4rgb888_28 = {
	.red		= {.offset = 16,.length = 8,},
	.green	= {.offset = 8, .length = 8,},
	.blue	= {.offset = 0, .length = 8,},
	.transp	= {.offset = 24, .length = 4,},
};

const static imapfb_rgb_t imapfb_a8rgb888_32 = {
	.red		= {.offset = 16, .length = 8,},
	.green	= {.offset = 8, .length = 8,},
	.blue	= {.offset = 0, .length = 8,},
	.transp	= {.offset = 24, .length = 8,},
};

/* Imap framebuffer struct */
typedef struct {
	struct fb_info		fb;		/* linux framebuffer struct */
	struct device		*dev;	/* linux framebuffer device */

	struct clk		*clk1;	/* clock resource for imapx200 display controller subsystem (IDS) */
	struct clk		*clk2;	/* clock resource for imapx200 display controller subsystem (IDS) */

	struct resource	*mem;	/* memory resource for IDS mmio */
	void __iomem		*io;		/* mmio for IDS */

	UINT32			win_id;	/* framebuffer window number */

	/* buf0 raw memory addresses */
	dma_addr_t		map_dma_f1;	/* physical */
	u_char *			map_cpu_f1;	/* virtual */
	UINT32			map_size_f1;	/* size */

	/* buf1 raw memory addresses */
	dma_addr_t		map_dma_f2;	/* physical */
	u_char *			map_cpu_f2;	/* virtual */
	unsigned int		map_size_f2;	/* size */

	/* buf2 raw memory addresses */
	dma_addr_t		map_dma_f3;	/* physical */
	u_char *			map_cpu_f3;	/* virtual */
	UINT32			map_size_f3;	/* size */

	/* buf3 raw memory addresses */
	dma_addr_t		map_dma_f4;	/* physical */
	u_char *			map_cpu_f4;	/* virtual */
	UINT32			map_size_f4;	/* size */

	/* keep these registers in case we need to re-write palette */
	UINT32			palette_buffer[256];	/* real palette buffer */
	UINT32			pseudo_pal[256];		/* pseudo palette buffer */

	void *fbhandle;
} imapfb_info_t;

typedef struct {
	/* Screen physical resolution */
	UINT32 xres;
	UINT32 yres;

	/* OSD Screen info */
	UINT32 osd_xres;			/* Visual OSD x resolution */
	UINT32 osd_yres;			/* Visual OSD y resolution */
	UINT32 osd_xres_virtual;	/* Virtual OSD x resolution */
	UINT32 osd_yres_virtual;	/* Virtual OSD y resolution */
	UINT32 osd_xoffset;		/* Visual to Virtual OSD x offset */
	UINT32 osd_yoffset;		/* Visual to Virtual OSD y offset */

	UINT32 bpp;				/* Bits per pixel */
	UINT32 bytes_per_pixel;	/* Bytes per pixel for true color */
	UINT32 pixclock;			/* Pixel clock */

	UINT32 hsync_len;		/* Horizontal sync length */
	UINT32 left_margin;		/* Horizontal back porch */
	UINT32 right_margin;		/* Horizontal front porch */
	UINT32 vsync_len;		/* Vertical sync length */
	UINT32 upper_margin;	/* Vertical back porch */
	UINT32 lower_margin;	/* Vertical front porch */
	UINT32 sync;				/* whether sync signal is used or not */

	UINT32 cmap_grayscale:1;
	UINT32 cmap_inverse:1;
	UINT32 cmap_static:1;
	UINT32 unused:29;

	UINT32 pixfmt;
}imapfb_fimd_info_t;

/* #################################################################################### */

#ifdef CONFIG_LCD_ENABLE_IRQ
/* i notice that above structure for register is something problem. added by sololz */
typedef struct
{
	volatile unsigned int dmp[21];
	volatile unsigned int pending;	/* interrupt register of pending */
	volatile unsigned int source;	/* interrupt register of source */
	volatile unsigned int mask;	/* interrupt register of mask */
}ids_reg_t;
#endif	/* CONFIG_LCD_ENABLE_IRQ */

/* #################################################################################### */

void imapfb_lcd_power_supply(UINT32);
void imapfb_backlight_power_supply(UINT32);
void imapfb_set_brightness(UINT32);
void imapfb_set_gpio(int);
void imapfb_set_clk(void);

extern const char imapfb_oem_logo_data[];

typedef struct{
        void * vir_addr;
        unsigned int phy_addr;     // if flag has IM_BUFFER_FLAG_PHY_NONLINEAR, phy_addr is the first block physical memory address.
        unsigned int size;
        unsigned int flag; // [15:0] reserved, user cannot write to.
}IM_Buffer;

#define IM_BUFFER_FLAG_PHY                      (1<<0)  // linear physical memory.
#define ALC_FLAG_PHY_MUST		(1<<2)	// physical and linear requirement.
#define ALC_FLAG_DEVADDR		(1<<12)	// request the devaddr, used for dev-mmu.


typedef struct{
        IM_Buffer       buffer;
        unsigned int       devAddr;        // devmmu viewside address.
        int        		pageNum;
        unsigned int       attri;  // attributes.
        void *          privData;
}alc_buffer_t;


#endif

