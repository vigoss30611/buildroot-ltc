/*
 *    arch/arm/mach-imapx15/include/mach/hw_cfg.h
 *
 *
 */
#include <mach/items.h>
#include <linux/adc_keys.h>

#define  PARSE_OK	1
#define  PARSE_FAILED -1
struct module_parse_status {
	int	imapx_module_type;
	int	need_parse;
	int	parse_done;
};

/* NAND */

/* GPIO KEY */
struct gpio_key_cfg {
	char compatible[ITEM_MAX_LEN];
	int menu;
	int home;
	int back;
	int volup;
	int voldown;
	int hp;
	int spk;
	int reset;
};

/* SDMMC */
struct sd_cfg{
	int sd1_Rdelayline_val;
	int sd1_Wdelayline_val;
	int sd2_Rdelayline_val;
	int sd2_Wdelayline_val;
};

/* PMU  */
struct pmu_cfg {
	char name[ITEM_MAX_LEN];
	char ctrl_bus[ITEM_MAX_LEN];
	int chan;
	int drvbus;
	int extraid;
	int acboot;
	int batt_cap;
	int batt_rdc;
	int core_voltage;
};

/* WIFI */

struct wifi_cfg {
	char model_name[ITEM_MAX_LEN];
	//char model_bus[ITEM_MAX_LEN];
	int model_bus;
	char power_pmu[ITEM_MAX_LEN];
	int	 power_pad;
};

/* RTC */
struct rtc_cfg {
	char model_name[ITEM_MAX_LEN];
};

struct bl_cfg {
	int power_pad;
	char power_pmu[ITEM_MAX_LEN];
	int pwm_id;
	unsigned int pwm_period_ns;
	unsigned int max_brightness;
	unsigned int dft_brightness;
	unsigned int lth_brightness;
	int polarity;
};

struct led_cfg_data {
	char model_name[ITEM_MAX_LEN];
	int gpio;
	int polarity;
	int default_state;
	unsigned long delay_on;
	unsigned long delay_off;
};

struct led_cfg {
	int num_leds;
	struct led_cfg_data *led_data;
};


struct usb_cfg {
	int exist_otg;
};

struct otg_cfg {
	int exist_otg;
	int extraid;
	int drvvbus;
};

struct codec_cfg {
	char power_pmu[ITEM_MAX_LEN];
	char clk_src[ITEM_MAX_LEN];
	int spkvolume;
	int micvolume;
	int hp_io;
	int spk_io;
	int highlevel;
	int i2s_mode;
};

struct audio_cfg {
	char name[ITEM_MAX_LEN];
	char ctrl_bus[ITEM_MAX_LEN];
	char data_bus[ITEM_MAX_LEN];
	int ctrl_busnum;
	int data_busnum;
    int spdif_en;
	struct codec_cfg *codec;
	int i2s_mode;
};

struct lightsen_hw_cfg {
	char model[ITEM_MAX_LEN];
	char bus[ITEM_MAX_LEN];
	int chan;
};

struct battery_cfg {
 	//char model[ITEM_MAX_LEN];
	int adc_chan;
 	int divider_ratio;
 	//int ref_volt;
 	//char adc_bits;
 };

struct eth_cfg {
	int isolate;
	int clk_provide;
	int reset_gpio;
	char model[ITEM_MAX_LEN];
};

enum  imapx_module_type {
	IMAPX_KEY = 0,
	IMAPX_WIFI,
	IMAPX_PMU,
	IMAPX_SDIO,
	IMAPX_RTC,
	IMAPX_BL,
	IMAPX_LED,
	IMAPX_USB,
	IMAPX_DWC_OTG,
	IMAPX_AUDIO,
	IMAPX_CAMERA,
	IMAPX_ETH,
	IMAPX_LS_HWMON,
	IMAPX_BATTERY,
	IMAPX_SD,
	IMAPX_END,
};


void __init imapx_item_parse(struct module_parse_status *mps, int nr);
extern struct wifi_cfg imapx_wifi_cfg;
extern struct sd_cfg imapx_sd_cfg;
extern struct gpio_key_cfg  imapx_gpio_key_cfg;
extern struct rtc_cfg  imapx_rtc_cfg;
extern struct bl_cfg	imapx_bl_cfg;
extern struct led_cfg	imapx_led_cfg;
extern struct usb_cfg   imapx_usb_cfg;
extern struct otg_cfg   imapx_otg_cfg;
extern struct pmu_cfg imapx_pmu_cfg;
extern struct audio_cfg	imapx_audio_cfg[2];
extern struct eth_cfg	imapx_eth_cfg;
extern struct lightsen_hw_cfg imapx_lightsensor_cfg;
extern struct battery_cfg imapx_battery_cfg;
extern struct adc_keys_button adc_buttons_cfg[MAX_ADC_KEYS_NUM];
