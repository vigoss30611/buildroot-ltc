/*
 * display_device.h - generic, centralized driver model
 *
 * Copyright (c) 2001-2003 Patrick Mochel <mochel@osdl.org>
 *
 * This file is released under the GPLv2
 *
 * See Documentation/driver-model/ for more information.
 */

#ifndef _DISPLAY_DEVICE_H_
#define _DISPLAY_DEVICE_H_

#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>

#include <linux/delay.h>

#define DISPLAY_DEVID_NONE	(-1)
#define DISPLAY_DEVID_AUTO	(-2)

#define MAX_PATH_PERIPHERAL_NUM		4

/* Internal vic defination, not exposure to userspace */
#define DISPLAY_SPECIFIC_VIC_PERIPHERAL			3000
#define DISPLAY_SPECIFIC_VIC_OSD							3100
#define DISPLAY_SPECIFIC_VIC_BUFFER_START		3200
#define DISPLAY_SPECIFIC_VIC_OVERLAY_START		3300

#define DISPLAY_HDMI_BOOT_DELAY							(HZ*3)
#define DISPLAY_HDMI_HPD_DELAY							(HZ/2)
#define DISPLAY_HDMI_EDID_DELAY							(HZ)

enum display_data_format_bpp {
	DISPLAY_DATA_FORMAT_16BPP_RGB565 = 5,
	DISPLAY_DATA_FORMAT_24BPP_RGB888 = 11,
	DISPLAY_DATA_FORMAT_32BPP_ARGB8888 = 16,
	DISPLAY_DATA_FORMAT_YCBCR_420,
	DISPLAY_DATA_FORMAT_YUV_422,						// for hdmi
};

enum display_lcd_rgb_order {
	DISPLAY_LCD_RGB_ORDER_RGB = 0x06,
	DISPLAY_LCD_RGB_ORDER_RBG = 0x09,
	DISPLAY_LCD_RGB_ORDER_GRB = 0x12,
	DISPLAY_LCD_RGB_ORDER_GBR = 0x18,
	DISPLAY_LCD_RGB_ORDER_BRG = 0x21,
	DISPLAY_LCD_RGB_ORDER_BGR = 0x24,
};

enum display_yuv_format {
	DISPLAY_YUV_FORMAT_NOT_USE = 0,		// not yuv, for buffer format
	DISPLAY_YUV_FORMAT_NV12,			// yuv420sp, for buffer format
	DISPLAY_YUV_FORMAT_NV21,			// yuv420sp, for buffer format
	DISPLAY_YUV_FORMAT_YCBCR,		// 24bits, 4:2:2
	DISPLAY_YUV_FORMAT_YCRCB,		// 24bits, 4:2:2
	DISPLAY_YUV_FORMAT_CBCRY,		// 24bits, 4:2:2
	DISPLAY_YUV_FORMAT_CRCBY,		// 24bits, 4:2:2
	DISPLAY_YUV_FORMAT_YCBYCR,	// 8 or 16bits, 4:2:0
	DISPLAY_YUV_FORMAT_YCRYCB,	// 8 or 16bits, 4:2:0
	DISPLAY_YUV_FORMAT_CBYCRY,	// 8 or 16bits, 4:2:0
	DISPLAY_YUV_FORMAT_CRYCBY,	// 8 or 16bits, 4:2:0
};

enum display_bt656_pad_map {
	DISPLAY_BT656_PAD_DATA0_TO_DATA7 = 0,
	DISPLAY_BT656_PAD_DATA8_TO_DATA15,
};

enum display_alpha_type {
	DISPLAY_ALPHA_TYPE_PLANE = 0,
	DISPLAY_ALPHA_TYPE_PIXEL,
};

enum display_scaler_mode {
	DISPLAY_SCALER_MODE_POSTSCALER = 0,
	DISPLAY_SCALER_MODE_PRESCALER,
};

enum display_interface_type {
	DISPLAY_INTERFACE_TYPE_PARALLEL_RGB888 = 0,
	DISPLAY_INTERFACE_TYPE_PARALLEL_RGB565,
	DISPLAY_INTERFACE_TYPE_SERIAL_RGB_DUMMY,
	DISPLAY_INTERFACE_TYPE_SERIAL_DUMMY_RGB,
	DISPLAY_INTERFACE_TYPE_SERIAL_RGB,
	DISPLAY_INTERFACE_TYPE_I80,
	DISPLAY_INTERFACE_TYPE_BT656,
	DISPLAY_INTERFACE_TYPE_BT601,
	DISPLAY_INTERFACE_TYPE_HDMI,
	DISPLAY_INTERFACE_TYPE_MIPI_DSI,
	DISPLAY_INTERFACE_TYPE_LVDS,
	DISPLAY_INTERFACE_TYPE_I2C,
	DISPLAY_INTERFACE_TYPE_SPI,
};

enum display_module_type {
	DISPLAY_MODULE_TYPE_FB = 0,
	DISPLAY_MODULE_TYPE_CLK,
	DISPLAY_MODULE_TYPE_SCALER,
	DISPLAY_MODULE_TYPE_OVERLAY,
	DISPLAY_MODULE_TYPE_IMAGE_ENHANCE,
	DISPLAY_MODULE_TYPE_DITHER,
	DISPLAY_MODULE_TYPE_LCDC,		// controller mutual exclusion
	DISPLAY_MODULE_TYPE_TVIF,		// controller mutual exclusion
	DISPLAY_MODULE_TYPE_HDMI,		// need lcd timing
	DISPLAY_MODULE_TYPE_MIPI_DSI,		// need lcd timing
	DISPLAY_MODULE_TYPE_IRQ,
	DISPLAY_MODULE_TYPE_PERIPHERAL,		// the last physical module

	DISPLAY_MODULE_TYPE_VIC,			// virtual module
};

/* bit shift */
enum display_irq_mask {
	DISPLAY_IRQ_MASK_LCDINT = 0,
	DISPLAY_IRQ_MASK_VCLKINT,
	DISPLAY_IRQ_MASK_OSDW0INT,		// Reserved
	DISPLAY_IRQ_MASK_OSDW1INT,		// Reserved
	DISPLAY_IRQ_MASK_OSDERR,
	DISPLAY_IRQ_MASK_I80INT,
	DISPLAY_IRQ_MASK_RSVD,		// Reserved
	DISPLAY_IRQ_MASK_TVINT,
};

/* logo are always centered */
enum display_logo_scale {
	DISPLAY_LOGO_SCALE_NONSCALE = 0,		// not scale
	DISPLAY_LOGO_SCALE_FIT,							// fit screen, keep ratio, may have black area
	DISPLAY_LOGO_SCALE_STRETCH,				// stretch to cover full screen, not keep ratio
};

/* peripheral control interface */
enum display_peripheral_control_interface {
	DISPLAY_PERIPHERAL_CONTROL_INTERFACE_NONE = 0,
	DISPLAY_PERIPHERAL_CONTROL_INTERFACE_SPI,
	DISPLAY_PERIPHERAL_CONTROL_INTERFACE_I2C,
};

enum display_hdmidvi_set {
	DISPLAY_HDMIDVI_AUTO_DETECT,
	DISPLAY_HDMIDVI_FORCE_HDMI,
	DISPLAY_HDMIDVI_FORCE_DVI,
};

#define MAX_GPIO_NUM		16
#define MAX_REGULATOR_NUM		8
#define REGULATOR_NAME_LEN		16

#ifndef DTD_TYPE
#define DTD_TYPE
typedef struct {
	u16 mCode;		// VIC
	u16 mPixelRepetitionInput;
	u32 mPixelClock;		// in KHz (different with hdmi)
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

struct display_device;

struct peripheral_param_config {
	char *name;
	enum display_interface_type interface_type;
	enum display_lcd_rgb_order rgb_order;		// for lcdc controller interface
	enum display_bt656_pad_map bt656_pad_map;		// for tvif bt656
	enum display_yuv_format bt601_yuv_format;			// for tvif bt601
	int bt601_data_width;												// for tvif bt601
	enum display_peripheral_control_interface control_interface;
	dtd_t dtd;			// panel resolution and timing params, for fixed-resolution device
	int spi_bits_per_word;					// for spi control interface
	struct spi_board_info spi_info;		// for spi control interface
	struct i2c_board_info i2c_info;		// for i2c control interface
	int (*init)(struct display_device *);					// for complex device initialize, e.g. chip
	int (*set_vic)(struct display_device *, int);		// only valid for variable resolution device, e.g. chip
	int (*enable)(struct display_device *, int);
	int (*irq_handler)(struct display_device *);		// for chip
	int (*i80_refresh)(struct display_device *, int);	// for i80 manual refresh rgb data, bypass lcdc
	int i80_mem_phys_addr;			// for i80 manual refresh, buffer physical address, status variable
	int i80_format;								// for i80 format
	int default_vic;													// for variable resolution device, e.g. chip, hdmi
	unsigned long osc_clk;										// for chip, in Hz
	struct device_attribute *attrs;			// optional, peripheral device specific
};

struct peripherial_resource {
	/* power on sequence resources */
	int gpios[MAX_GPIO_NUM];
	struct regulator * regulators[MAX_REGULATOR_NUM];
	char regulators_name[MAX_REGULATOR_NUM][REGULATOR_NAME_LEN];
};

struct display_overlay {
	int enable;			// overlay enable
	int vic;					// overlay vic
	int left_top_x;		// overlay output LeftTop
	int left_top_y;		// overlay output LeftTop
	enum display_data_format_bpp bpp;		// overlay bpp
	enum display_yuv_format yuv_format;		// overlay yuv format
	unsigned int buffer_addr;		// overlay input buffer physical address
	int vw_width;				// overlay input buffer virtual width
	int mapcolor_en;
	int mapcolor;		// overlay map color in RGB888
	int rb_exchange;	// overlay red and blue channel exchange
	int ck_enable;		// use chroma key. chroma_key function's priority is higher than alpha function
	int ck_value;			// chroma key value
	int ck_alpha_nonkey_area;		// alpha for non-key area (useful area)
	int ck_alpha_key_area;		// alpha for key area (useless area)
	enum display_alpha_type alpha_type;		// alpha function: type	(when chroma key is disable)
	int alpha_value;				// alpha function: value	(when chroma key is disable)
};

struct display_device {
	const char	*name;
	int		id;
	bool		id_auto;
	struct device	dev;
	u32		num_resources;
	struct resource	*resource;
	const struct platform_device_id	*id_entry;

	/* config parameters, set before register */
	int path;								// current device in which path.
	int fixed_resolution;			// for peripheral device
	enum display_module_type module_type;		// set when init
	struct peripheral_param_config peripheral_config;		// peripheral param config

	/* status variable */
	int enable;							// overlay device use struct display_overlay.enable
	int vic;									// overlay device use struct display_overlay.vic; scaler use it as input vic
	struct display_overlay * overlays;		// for overlay
	int osd_background_color;				// for overlay
	int cdown;							//lcdc: auto decrease vclk
	int rfrm_num;						//lcdc: auto decrease vclk

	int peripheral_selected;	// select as target peripheral device. only valid for peripheral device.
	enum display_interface_type interface_type;		// for peripheral device
	enum display_lcd_rgb_order rgb_order;			// for lcd panel and lcdc
	enum display_bt656_pad_map bt656_pad_map;		// for tvif bt656
	enum display_yuv_format bt601_yuv_format;			// for tvif bt601
	int bt601_data_width;												// for tvif bt601
	int i80_format;															// for i80
	
	int scale_needed;				// scaler: whether scale operation is needed
	enum display_scaler_mode scaler_mode;		// scaler mode
	int prescaler_chn;				// prescaler: which overlay use prescaler
	int prescaler_vw_width;		// prescaler: input buffer virtual width
	enum display_data_format_bpp scaler_bpp;		// scaler: bpp
	enum display_yuv_format scaler_yuv_format;		// scaler: yuv format
	unsigned int scaler_buffer_addr;			// scaler: buffer physical addr

	struct completion vsync;			// irq vsync
	unsigned char irq_mask;				// irq mask
	struct peripherial_resource power_resource;		// peripheral power sequence resources
	int uboot_logo;							// has uboot logo or not
	int kernel_logo;							// has kernel logo or not
	char *logo_virt_addr;					// logo virtual addr
	
	struct delayed_work hpd_dwork;		// for hdmi hot plug detect
	struct delayed_work edid_dwork;		// for hdmi edid read
	int hpd;										// for hdmi hot plug detect

	/* resources */
	struct clk *clk_src;
	int irq;

	/* data */
	struct spi_device *spi;		// for lcd panel with spi control interface
	struct i2c_client *i2c;		// for lcd panel with i2c control interface
	int buf_idx; // for ping-pong buffer method
};

#define display_get_device_id(pdev)	((pdev)->id_entry)

#define to_display_device(x) container_of((x), struct display_device, dev)

extern int display_device_register(struct display_device *);
extern void display_device_unregister(struct display_device *);

extern struct bus_type display_bus_type;
extern struct device display_bus;

extern struct resource *display_get_resource(struct display_device *,
					      unsigned int, unsigned int);
extern int display_get_irq(struct display_device *, unsigned int);
extern struct resource *display_get_resource_byname(struct display_device *,
						     unsigned int,
						     const char *);
extern int display_get_irq_byname(struct display_device *, const char *);
extern int display_add_devices(struct display_device **, int);


extern struct display_device *display_device_alloc(const char *name, int id);
extern int display_device_add_resources(struct display_device *pdev,
					 const struct resource *res,
					 unsigned int num);
extern int display_device_add_data(struct display_device *pdev,
				    const void *data, size_t size);
extern int display_device_add(struct display_device *pdev);
extern void display_device_del(struct display_device *pdev);
extern void display_device_put(struct display_device *pdev);


struct display_function {
	/* common */
	int (*enable)(struct display_device *, int);		// module enable
	int (*set_vic)(struct display_device *, int);		// exclude overlay and scaler
	/* overlay */
	int (*set_background_color)(struct display_device *, int);		// set osd back ground color
	int (*overlay_enable)(struct display_device *, int, int);		// instead of enable
	int (*set_overlay_vic)(struct display_device *, int, int);		// instead of set_vic
	int (*set_overlay_position)(struct display_device *, int, int, int);		// =prescaler output size
	int (*set_overlay_vw_width)(struct display_device *, int, int);		// =prescaler vw_width
	int (*set_overlay_bpp)(struct display_device *, int, enum display_data_format_bpp, enum display_yuv_format, int);		// osd has only one cbcr dma channel
	int (*set_overlay_buffer_addr)(struct display_device *, int, int);
	int (*set_overlay_mapcolor)(struct display_device *, int, int, int);
	int (*set_overlay_red_blue_exchange)(struct display_device *, int, int);
	int (*set_overlay_chroma_key)(struct display_device *, int, int, int, int, int);
	int (*set_overlay_alpha)(struct display_device *, int , enum display_alpha_type, int);		// alpha value: in RGB888 format, valid in plane blending
	/* scaler */
	int (*set_scaler_mode)(struct display_device *, enum display_scaler_mode);		// select prescaler or postscaler
	int (*set_scaler_size)(struct display_device *, int);		// set scaler input and output size. must be called after scaler mode is set
	int (*set_scaler_bpp)(struct display_device *, enum display_data_format_bpp, enum display_yuv_format);
	int (*set_prescaler_channel)(struct display_device *, int);		// which overlay use prescaler
	int (*set_prescaler_vw_width)(struct display_device *, int);
	int (*set_prescaler_buffer_addr)(struct display_device *, int);
	/* lcdc & tvif */
	int (*set_interface_type)(struct display_device *, enum display_interface_type);
	int (*set_rgb_order)(struct display_device *, enum display_lcd_rgb_order);
	int (*set_bt656_pad_map)(struct display_device *, enum display_bt656_pad_map);
	int (*set_bt601_yuv_format)(struct display_device *, int, enum display_yuv_format);
	int (*set_auto_vclk)(struct display_device *, int, int);
	/* irq */
	int (*set_irq_mask)(struct display_device *, unsigned char);
	int (*wait_vsync)(struct display_device *, unsigned long);
	/* vic */
	int (*set_dtd)(struct display_device *, int, dtd_t *);

// TODO below haven't consider details

	/* scaler algorithm */
	int (*set_algorithm)(struct display_device *, int);
	/* image enhance */
	int (*set_gamma)(struct display_device *, int);
	int (*set_acm)(struct display_device *, int);
	int (*set_acc)(struct display_device *, int);
	/* dither */
	int (*set_dither_input)(struct display_device *, int);
	int (*set_dither_output)(struct display_device *, int);
	/* hdmi */
	int (*set_hdcp_enable)(struct display_device *, int);
};

struct display_driver {
	int (*probe)(struct display_device *);
	int (*remove)(struct display_device *);
	void (*shutdown)(struct display_device *);
	int (*suspend)(struct display_device *, pm_message_t state);
	int (*resume)(struct display_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
	enum display_module_type module_type;
	struct display_function function;
};

extern int display_driver_register(struct display_driver *);
extern void display_driver_unregister(struct display_driver *);


static inline void *display_get_drvdata(const struct display_device *pdev)
{
	return dev_get_drvdata(&pdev->dev);
}

static inline void display_set_drvdata(struct display_device *pdev,
					void *data)
{
	dev_set_drvdata(&pdev->dev, data);
}

/* module_display_driver() - Helper macro for drivers that don't do
 * anything special in module init/exit.  This eliminates a lot of
 * boilerplate.  Each module may only use this macro once, and
 * calling it replaces module_init() and module_exit()
 */
#define module_display_driver(__display_driver) \
	module_driver(__display_driver, display_driver_register, \
			display_driver_unregister)


#ifdef CONFIG_SUSPEND
extern int display_pm_suspend(struct device *dev);
extern int display_pm_resume(struct device *dev);
#else
#define display_pm_suspend		NULL
#define display_pm_resume		NULL
#endif

/* additional */

extern int resource_find_regulator(struct peripherial_resource *power_resource, char *name);
extern int display_get_function(int path, enum display_module_type type,
		struct display_function **func, struct display_device  **pdev);
extern int display_get_device(int path, enum display_module_type type,
			struct display_device  **pdev);

extern int display_set_logo(int path, int buf_addr, int logo_width, int logo_height,
				enum display_logo_scale logo_scale, int background);

extern int display_get_peripheral_vic(int path);
extern int display_set_peripheral_vic(int path, int vic);
extern int display_select_peripheral(int path, char *target_peripheral);

extern int display_path_enable(int path);
extern int display_path_disable(int path);

extern int display_peripheral_resource_init(struct display_device *pdev);
extern int display_peripheral_sequential_power(struct display_device *pdev, int en);

extern int display_set_clk_freq(struct display_device *pdev, struct clk *clk_src,
																		unsigned long target_freq, int *sub_divider);
extern int display_set_clk(struct display_device *pdev, struct clk *clk_src, int vic, int *sub_divider);
extern int vic_fill(int path, dtd_t *dtd, unsigned int code);
extern int internal_vic_fill(int path, dtd_t *dtd, unsigned int code);

extern int kernel_logo(void);
extern int logo_get_width(void);
extern int logo_get_height(void);
extern int fill_logo_buffer(unsigned int buffer_addr);

extern bool display_support(void);


#ifndef __DISPLAY_LOG_HEADER__
#define __DISPLAY_LOG_HEADER__

enum display_print_level {
	LOG_LEVEL_ERR = 0,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DBG,
	LOG_LEVEL_TRACE,
	LOG_LEVEL_IRQ,
};

extern int display_log_level;

#define dlog_irq(format, arg...)	\
	({ if (display_log_level >= LOG_LEVEL_IRQ) 	\
		printk(KERN_INFO "[ids|%s] irq: %s(): " format, DISPLAY_MODULE_LOG, __func__, ## arg); 0; })

#define dlog_trace()	\
	({ if (display_log_level >= LOG_LEVEL_TRACE) 	\
		printk(KERN_INFO "[ids|%s] trace: %s()\n", DISPLAY_MODULE_LOG, __func__); 0;})

#define dlog_dbg(format, arg...)	\
	({ if (display_log_level >= LOG_LEVEL_DBG) 	\
		printk(KERN_INFO "[ids|%s] dbg: %s(): " format, DISPLAY_MODULE_LOG, __func__, ## arg); 0; })

#define dlog_info(format, arg...)	\
	({ if (display_log_level >= LOG_LEVEL_INFO) 	\
		printk(KERN_INFO "[ids|%s] info %s: " format, DISPLAY_MODULE_LOG, \
				(display_log_level >= LOG_LEVEL_DBG?__func__:""), ## arg); 0; })

#define dlog_warn(format, arg...)	\
	({ if (display_log_level >= LOG_LEVEL_WARN) 	\
		printk(KERN_WARNING "[ids|%s] warn %s: " format, DISPLAY_MODULE_LOG, \
				(display_log_level >= LOG_LEVEL_DBG?__func__:""), ## arg); 0; })

#define dlog_err(format, arg...)	\
	({ if (display_log_level >= LOG_LEVEL_ERR)	\
		printk(KERN_ERR "[ids|%s] error %s: " format, DISPLAY_MODULE_LOG, \
				(display_log_level >= LOG_LEVEL_DBG?__func__:""), ## arg); 0; })
#endif


#endif /* _DISPLAY_DEVICE_H_ */
