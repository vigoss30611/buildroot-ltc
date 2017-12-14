/*
 *    arch/arm/mach-imapx910/include/mach/hw_cfg.h
 *
 *
 */
#include <mach/items.h>

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
/*external temprature monitor*/
struct tempmon_cfg {
	char model[ITEM_MAX_LEN];
};

/* SDMMC */

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
};

/* WIFI */

struct wifi_cfg {
	char model_name[ITEM_MAX_LEN];
	int  model_bus; /* 1:USB 2:SDIO*/
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
	int pwm_chn;
};

struct usb_cfg {
	int exist_otg;
};

struct otg_cfg {
	int exist_otg;
	int extraid;
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

struct eth_cfg {
	int isolate;
};

enum  imapx_module_type {
	IMAPX_GPIO_KEY = 0,
	IMAPX_TEMPMON,
	IMAPX_WIFI,
	IMAPX_PMU,
	IMAPX_SDIO,
	IMAPX_RTC,
	IMAPX_BL,
	IMAPX_USB,
	IMAPX_DWC_OTG,
	IMAPX_AUDIO,
	IMAPX_CAMERA,
	IMAPX_ETH,
	IMAPX_END,
};

void __init imapx_item_parse(struct module_parse_status *mps, int nr);
extern struct wifi_cfg imapx_wifi_cfg;
extern struct gpio_key_cfg  imapx_gpio_key_cfg;
extern struct tempmon_cfg  imapx_tempmon_cfg;
extern struct rtc_cfg  imapx_rtc_cfg;
extern struct bl_cfg	imapx_bl_cfg;
extern struct usb_cfg   imapx_usb_cfg;
extern struct otg_cfg   imapx_otg_cfg;
extern struct pmu_cfg imapx_pmu_cfg;
extern struct audio_cfg	imapx_audio_cfg;
extern struct sensor_info camera_front_cfg;
extern struct sensor_info camera_rear_cfg;
extern struct eth_cfg	imapx_eth_cfg;
