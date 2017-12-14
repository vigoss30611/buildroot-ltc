/*
 * lcdc.c - display lcd panel driver
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <mach/pad.h>
#include <dss_common.h>
#include <lcd_panel_params.h>


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[terminal]"
#define DSS_SUB2	"[lcd panel]"


extern void imapx_bl_power_on(void);
extern int lcdc_pwren_polarity(int idsx, int polarity);
extern int lcdc_pwren(int idsx, int enable);
extern int lcdc_output_en(int idsx, int enable);

static int master_en_control(int enable);
static int lcdc_output_control(int enable);
static int dvdd_en_control(int enable);
static int avdd_en_control(int enable);
static int vgh_en_control(int enable);
static int vgl_en_control(int enable);
int backlight_en_control(int enable);

static int get_connect(int idsx, int *params, int num);
static int set_enable(int idsx, int *params, int num);
static int set_bl_brightness(int idsx, int *params, int num);
static int get_rgb_seq(int idsx, int *params, int num);
static int get_rgb_bpp(int idsx, int *params, int num);
static int dss_init_module(int idsx);
static int obtain_information(int idsx);
static int lcd_exist = 1;






static int lcd_rgb_seq;
static int lcd_rgb_bpp;
static int lcd_idsx;
static char panel_name[64] = {0};
struct lcd_gpio_control lcd_gpio = {0};
static int pwren_polarity = 1;
char master_pmu[ITEM_MAX_LEN];
struct regulator *master_regulator = NULL;

static struct lcd_panel_param * panel = NULL;
static struct lcd_panel_param * lcd_panels[] = {
	&panel_KR070PB2S_800_480,
	&panel_KR070LB0S_1024_600,
    &panel_KR079LA1S_768_1024,
    &panel_GM05004001Q_800_480,
	&panel_TXDT200LER_720_576,
    &panel_OTA5180A_480_272,//xiongbiao
    &panel_ILI9806S_480_800,//xiongbiao
    &panel_ST7567_132_65,//xiongbiao
};



// TODO: terminal attributes should also be set when switching terminal device in abstration layer
static struct module_attr lcd_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_GET_CONNECT, get_connect},
	{TRUE, ATTR_BL_BRIGHTNESS, set_bl_brightness},
	{TRUE, ATTR_GET_RGB_SEQ, get_rgb_seq},
	{TRUE, ATTR_GET_RGB_BPP, get_rgb_bpp},
	{FALSE, ATTR_END, NULL},
};

struct module_node lcd_panel_module = {
	.name = "lcd_panel",
	.terminal = 1,
	.attributes = lcd_attr,
	.init = dss_init_module,
	.obtain_info = obtain_information,
};

/* sequential LCDPWR_*  */
lcd_power_control lcd_power[] = {
	master_en_control,
	lcdc_output_control,
	dvdd_en_control,
	avdd_en_control,
	vgh_en_control,
	vgl_en_control,
	backlight_en_control,
};

static int get_connect(int idsx, int *params, int num)
{
	assert_num(1);

	dss_err("lcd on \n");
	*params = lcd_exist; //always on

	return 0;
}

static int enable_operation(int operation, int enable)
{
	if (operation < LCDPWR_END)
		lcd_power[operation](enable);
	else
		if (panel->power_specific)
			panel->power_specific(operation, enable);
		else {
			dss_err("Undefined lcd power sequence operation %d\n", operation);
			return -1;
		}
	return 0;
}

/* Assume disable operation is the reverse sequence of enable operation. */
//static int lcd_default_enable(int enable)
int lcd_default_enable(int enable)
{
	int size = panel->power_seq_num;
	struct lcd_power_sequence *pwr_seq = panel->power_seq;
	int i;
	static int backlight_start_flag = 0 ;
	static int init = 0;
	int ubootLogoState = 1;

	if(item_exist("config.uboot.logo")){
		unsigned char str[ITEM_MAX_LEN] = {0};
		item_string(str,"config.uboot.logo",0);
		if(strncmp(str,"0",1) == 0 )
			ubootLogoState = 0;//disabled
	}
	/*0 : firtst start,fb is controled  by kernel driver ,
	 		run backlight_en_control once
	  1 : not first start ,fb is controled by android ,
	        the driver do not the backlight_en_control
	*/
	dss_trace("enable %d\n", enable);
	
	if (enable) {
		for (i = 0; i < size; i++) {
			if(pwr_seq[i].operation == LCDPWR_BACKLIGHT_EN 
					&& (backlight_start_flag != 0 || 
						(backlight_start_flag == 0 && 
						 (1 == ubootLogoState )&& init == 0))){ 
				dss_err("++++++android control fd,no need run backlight\n");
				continue;
			}
			enable_operation(pwr_seq[i].operation, enable);
			msleep(pwr_seq[i].delay);
		}
	}
	else {
		for (i = size -1; i >= 0; i--) {
			msleep(pwr_seq[i].delay);
			enable_operation(pwr_seq[i].operation, enable);
		}
	}
	backlight_start_flag = 1 ;
	init = 1;
	return 0;
}
EXPORT_SYMBOL(lcd_default_enable);


static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);
	
	if (!panel) {
		dss_err("panel NULL.\n");
		return -1;
	}
	
	if (panel->lcd_enable)
		panel->lcd_enable(enable);
	else
		lcd_default_enable(enable);
	
	return 0;
}

static int set_bl_brightness(int idsx, int *params, int num)
{
	int value = *params;
	assert_num(1);
	dss_trace("ids%d, brightness %d\n", idsx, value);
	
	return 0;
}

static int get_rgb_seq(int idsx, int *params, int num)
{
	assert_num(1);
	dss_trace("ids%d\n", idsx);
	*params = lcd_rgb_seq;
	return 0;
}

static int get_rgb_bpp(int idsx, int *params, int num)
{
	assert_num(1);
	dss_trace("ids%d\n", idsx);
	*params = lcd_rgb_bpp;
	return 0;
}

#if 1
int imap_lcd_timing_fill(dtd_t *dtd, int vic)
{
	dss_trace("vic %d\n", vic);
	
	if (!panel) {
		dss_err("panel NULL.\n");
		return -1;
	}
	*dtd = panel->dtd;
	return 0;
}
#else

int imap_lcd_timing_fill(dtd_t *dtd, int vic)
{
	dss_trace("vic %d\n", vic);
	
	*dtd = panel_KR070PB2S_800_480.dtd;
	return 0;
}
#endif



#ifdef CONFIG_LCD_SPI_ILI8961
extern struct ili8961 *ili8961_spidev;
extern int ili8961_init(struct ili8961 *spidev);
#endif
#ifdef CONFIG_LCD_SPI_ILI9806S
extern struct ili8961 *ili9806s_spidev;
extern int ili9806s_init_by_gpio();
#endif
#ifdef CONFIG_LCD_SPI_OTA5180A
extern struct ota5182a *ota5182a_spidev;
extern int ota5182a_init(struct ota5182a *spidev);
#endif
/* Operation control for lcd panel */
static int master_en_control(int enable)
{
	int en = enable ? lcd_gpio.master_polarity : (!lcd_gpio.master_polarity);
	dss_err("enable %d\n", enable);
	if (lcd_gpio.master_gpio)
//		imapx_pad_set_outdat(en , 1, lcd_gpio.master_gpio);
		gpio_set_value(lcd_gpio.master_gpio, en);
	else if (master_regulator) {
		if (enable) {
			dss_err("%s line %d\n", __func__, __LINE__);
			regulator_enable(master_regulator);
		}
		else {
			dss_err("%s line %d\n", __func__, __LINE__);
			regulator_disable(master_regulator);
		}
	} else
		lcdc_pwren(lcd_idsx, enable);
#ifdef CONFIG_LCD_SPI_ILI8961
	ili8961_init(ili8961_spidev);
#endif
#ifdef CONFIG_LCD_SPI_ILI9806S
	ili9806s_init_by_gpio();
#endif
#ifdef CONFIG_LCD_SPI_OTA5180A
	ota5182a_init(ota5182a_spidev);
#endif

	return 0;
}

static int lcdc_output_control(int enable)
{
	lcdc_output_en(lcd_idsx, enable);
	return 0;
}

static int dvdd_en_control(int enable)
{
	int en = enable ? lcd_gpio.dvdd_polarity : (!lcd_gpio.dvdd_polarity);
	if (lcd_gpio.dvdd_gpio)
//		imapx_pad_set_outdat(en, 1, lcd_gpio.dvdd_gpio);
		gpio_set_value(lcd_gpio.dvdd_gpio, en);
	return 0;
}

static int avdd_en_control(int enable)
{
	int en = enable ? lcd_gpio.avdd_polarity : (!lcd_gpio.avdd_polarity);
	if (lcd_gpio.avdd_gpio)
//		imapx_pad_set_outdat(en, 1, lcd_gpio.avdd_gpio);
		gpio_set_value(lcd_gpio.avdd_gpio, en);
	return 0;
}

static int vgh_en_control(int enable)
{
	int en = enable ? lcd_gpio.vgh_polarity : (!lcd_gpio.vgh_polarity);
	if (lcd_gpio.vgh_gpio)
//		imapx_pad_set_outdat(en, 1, lcd_gpio.vgh_gpio);
		gpio_set_value(lcd_gpio.vgh_gpio, en);
	return 0;
}

static int vgl_en_control(int enable)
{
	int en = enable ? lcd_gpio.vgl_polarity : (!lcd_gpio.vgl_polarity);
	if (lcd_gpio.vgl_gpio)
//		imapx_pad_set_outdat(en, 1, lcd_gpio.vgl_gpio);
		gpio_set_value(lcd_gpio.vgl_gpio, en);
	return 0;
}

int backlight_en_control(int enable)
{
	// TODO: Temp
	int en = enable ? lcd_gpio.back_light_polarity : (!lcd_gpio.back_light_polarity);
	if (lcd_gpio.back_light_gpio) {
		if (enable){
			imapx_bl_power_on();
			msleep(30);
//			imapx_pad_set_outdat(en, 1, lcd_gpio.back_light_gpio);
			gpio_set_value(lcd_gpio.back_light_gpio, en);
		}
		else {
//			imapx_pad_set_outdat(en, 1, lcd_gpio.back_light_gpio);
			gpio_set_value(lcd_gpio.back_light_gpio, en);
			msleep(20);
		}
	}
	return 0;
}


static int obtain_information(int idsx)
{
	// TODO: Obtain lcd_port_module bpp and choose small one for attribute
	return 0;
}




// knockoff temp for compile
int get_lcd_width(void)
{
	return panel->dtd.mHActive;
}
int get_lcd_height(void)
{
	return panel->dtd.mVActive;
}





static int get_item(void)
{
	dss_trace("~\n");
	
	if (item_exist(P_LCD_RGBSEQ))
		lcd_rgb_seq = item_integer(P_LCD_RGBSEQ, 0);
	else
		lcd_rgb_seq = panel->rgb_seq;

	if (item_exist(P_LCD_RGBBPP))
		lcd_rgb_bpp = item_integer(P_LCD_RGBBPP, 0);
	else
		lcd_rgb_bpp = panel->rgb_bpp;
	
	return 0;
}

/* Find out specific panel */
static int look_for_panel(void)
{
	int i, num;
	dss_trace("~\n");
	
	num = sizeof(lcd_panels)/sizeof(struct lcd_panel_param *);

	for (i = 0; i < num; i++) {
		if (strcmp(lcd_panels[i]->name, panel_name) == 0) {
			panel = lcd_panels[i];
			get_item();
			break;
		}
	}
	if (i == num) {
		dss_err("#item# Lcd panel %s not found.\n", panel_name);
		return -1;
	}

	return 0;
}

/* pad configuration locate here for the lcd panel controls the behavior of pad */
static int dss_init_module(int idsx)
{
	int ret;
	static int gpio_init[2] = {0};
	int ubootLogoState = 1;//1,ubootLOgo enabled; 0,ubootLogo disabled
	static int init = 0;
	dss_trace("ids%d\n", idsx);

	lcd_idsx = idsx;

	if (item_exist(P_LCD_BL_GPIO) && item_exist(P_LCD_BL_POLARITY)) {
		lcd_gpio.back_light_gpio = item_integer(P_LCD_BL_GPIO, 0);

		if (!gpio_init[idsx] && gpio_is_valid(lcd_gpio.back_light_gpio)) {
			ret = gpio_request(lcd_gpio.back_light_gpio, "lcd_bl");
			if (ret) {
				pr_err("failed request gpio for lcd_bl\n");
//				goto err_bl;
			}
		}

		lcd_gpio.back_light_polarity = item_integer(P_LCD_BL_POLARITY, 0);
		if(item_exist("config.uboot.logo")){
			unsigned char str[ITEM_MAX_LEN] = {0};
			item_string(str,"config.uboot.logo",0);
			if(strncmp(str,"0",1) == 0 )
				ubootLogoState = 0;//disabled
		}
		if ( 0 == ubootLogoState || init != 0)
			gpio_direction_output(lcd_gpio.back_light_gpio, !lcd_gpio.back_light_polarity);
	}
	/* Initialize lcd power gpio if any present */
	if (item_exist(P_LCD_MASTER_GPIO) && item_exist(P_LCD_MASTER_POLARITY)) {
		lcd_gpio.master_gpio = item_integer(P_LCD_MASTER_GPIO, 0);

		if (!gpio_init[idsx] && gpio_is_valid(lcd_gpio.master_gpio)) {
			ret = gpio_request(lcd_gpio.master_gpio, "lcd_master");
			if (ret) {
				pr_err("failed request gpio for lcd_master\n");
//				goto err_master;
			}
		}

		lcd_gpio.master_polarity = item_integer(P_LCD_MASTER_POLARITY, 0);
		if ( 0 == ubootLogoState || init != 0)
			gpio_direction_output(lcd_gpio.master_gpio, !lcd_gpio.master_polarity);
	} else if (item_exist(P_LCD_MASTER_PMU)) {
		item_string(master_pmu, P_LCD_MASTER_PMU, 1);
		master_regulator = regulator_get(NULL, master_pmu);
		if (IS_ERR(master_regulator)) {
			pr_err("%s get master pmu failed\b", __func__);
			return -1;
		} else {
#if 0 // replaced by linyun.xiong for reduce power @2015-09-01
			ret = regulator_set_voltage(master_regulator, 3300000, 3300000);
#else
			ret = regulator_set_voltage(master_regulator, 3000000, 3000000);
			printk("-->***set lcd panel voltage to 3.0v\n");
#endif
			if (ret) {
				dss_err("master regulator set failed\n");
				return  -1;
			}
		}
	}
	if (item_exist(P_LCD_DVDD_GPIO) && item_exist(P_LCD_DVDD_POLARITY)) {
		lcd_gpio.dvdd_gpio = item_integer(P_LCD_DVDD_GPIO, 0);

		if (!gpio_init[idsx] && gpio_is_valid(lcd_gpio.dvdd_gpio)) {
			ret = gpio_request(lcd_gpio.dvdd_gpio, "lcd_dvdd");
			if (ret) {
				pr_err("failed request gpio for lcd_dvdd\n");
//				goto err_dvdd;
			}
		}

		lcd_gpio.dvdd_polarity = item_integer(P_LCD_DVDD_POLARITY, 0);
		gpio_direction_output(lcd_gpio.dvdd_gpio, !lcd_gpio.dvdd_polarity);
	}
	if (item_exist(P_LCD_AVDD_GPIO) && item_exist(P_LCD_AVDD_POLARITY)) {
		lcd_gpio.avdd_gpio = item_integer(P_LCD_AVDD_GPIO, 0);

		if (!gpio_init[idsx] && gpio_is_valid(lcd_gpio.avdd_gpio)) {
			ret = gpio_request(lcd_gpio.avdd_gpio, "lcd_avdd");
			if (ret) {
				pr_err("failed request gpio for lcd_avdd\n");
//				goto err_avdd;
			}
		}

		lcd_gpio.avdd_polarity = item_integer(P_LCD_AVDD_POLARITY, 0);
		gpio_direction_output(lcd_gpio.avdd_gpio, !lcd_gpio.avdd_polarity);
	}
	if (item_exist(P_LCD_VGH_GPIO) && item_exist(P_LCD_VGH_POLARITY)) {
		lcd_gpio.vgh_gpio = item_integer(P_LCD_VGH_GPIO, 0);

		if (!gpio_init[idsx] && gpio_is_valid(lcd_gpio.vgh_gpio)) {
			ret = gpio_request(lcd_gpio.vgh_gpio, "lcd_vgh");
			if (ret) {
				pr_err("failed request gpio for lcd_vgh\n");
//				goto err_vgh;
			}
		}

		lcd_gpio.vgh_polarity = item_integer(P_LCD_VGH_POLARITY, 0);
		gpio_direction_output(lcd_gpio.vgh_gpio, !lcd_gpio.vgh_polarity);
	}
	if (item_exist(P_LCD_VGL_GPIO) && item_exist(P_LCD_VGL_POLARITY)) {
		lcd_gpio.vgl_gpio = item_integer(P_LCD_VGL_GPIO, 0);

		if (!gpio_init[idsx] && gpio_is_valid(lcd_gpio.vgl_gpio)) {
			ret = gpio_request(lcd_gpio.vgl_gpio, "lcd_vgl");
			if (ret) {
				pr_err("failed request gpio for lcd_vgl\n");
//				goto err_vgl;
			}
		}

		lcd_gpio.vgl_polarity = item_integer(P_LCD_VGL_POLARITY, 0);
		gpio_direction_output(lcd_gpio.vgl_gpio, !lcd_gpio.vgl_polarity);
	}
	if (item_exist(P_LCD_BL_PWM))
		lcd_gpio.back_light_pwm = item_integer(P_LCD_BL_PWM, 0);

	/* Config lcd pwren polarity */
	if (item_exist(P_LCD_PWREN_POLARITY)) {
		pwren_polarity = item_integer(P_LCD_PWREN_POLARITY, 0);
		lcdc_pwren_polarity(lcd_idsx, pwren_polarity);
	}

	/* Config lcd panel name */
	if (item_exist(P_LCD_NAME))
		item_string(panel_name, P_LCD_NAME, 0);
	else {
		dss_err("#item# Lack of lcd panel name item.\n");
		ret = -1;
		goto err_name;
	}
	dss_dbg("get lcd panel name %s\n", panel_name);
	
	ret = look_for_panel();
	if (ret) {
		ret = -1;
		goto err_name;
	}
	get_item();
	gpio_init[idsx] = 1;
	init = 1;

	return 0;

err_name:
	if (gpio_is_valid(lcd_gpio.vgl_gpio))
		gpio_free(lcd_gpio.vgl_gpio);

	if (gpio_is_valid(lcd_gpio.vgh_gpio))
		gpio_free(lcd_gpio.vgh_gpio);

	if (gpio_is_valid(lcd_gpio.avdd_gpio))
		gpio_free(lcd_gpio.avdd_gpio);

	if (gpio_is_valid(lcd_gpio.dvdd_gpio))
		gpio_free(lcd_gpio.dvdd_gpio);

	if (gpio_is_valid(lcd_gpio.master_gpio))
		gpio_free(lcd_gpio.master_gpio);

	if (gpio_is_valid(lcd_gpio.back_light_gpio))
		gpio_free(lcd_gpio.back_light_gpio);

	return ret;
}

MODULE_DESCRIPTION("InfoTM iMAP display lcd panel driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
