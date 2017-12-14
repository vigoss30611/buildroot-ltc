
#include <linux/kernel.h>
#include <linux/platform_data/camera-imapx.h>
#include <mach/hw_cfg.h>
#include <mach/audio.h>


struct gpio_key_cfg  imapx_gpio_key_cfg = {
	.compatible = "none",
	.menu = -1,
	.home = -1,
	.back = -1,
	.volup = -1,
	.voldown = -1,
	.reset = -1,
};

struct pmu_cfg	imapx_pmu_cfg = {
	.name		= "none",
	.ctrl_bus	= "none",
	.chan		= -1,
	.drvbus		= -1,
	.extraid	= -1,
	.acboot		= -1,
	.batt_cap	= -1,
	.batt_rdc	= -1,
};

struct wifi_cfg imapx_wifi_cfg = {
	.model_name	= "NOWIFI",
	.model_bus		= -1,
	.power_pmu		= "NOPMU",
	.power_pad		= -1,
};
struct rtc_cfg imapx_rtc_cfg = {
	.model_name	= "imapx",
};

struct bl_cfg imapx_bl_cfg = {
	.power_pad		= -1,
	.power_pmu		= "NOPMU",
	.pwm_chn		= -1,

};

struct usb_cfg imapx_usb_cfg = {
	.exist_otg      = -1,
};

struct otg_cfg imapx_otg_cfg = {
	.exist_otg      = -1,
	.extraid        = -1,
	.drvvbus        = -1,
};

struct codec_cfg subaudio_cfg = {
	.power_pmu		= "nopmu",
	.spkvolume		= -1,
	.micvolume		= -1,
	.hp_io			= -1,
	.spk_io			= -1,
	.highlevel		= -1,
	.i2s_mode		= -1,
};

struct audio_cfg imapx_audio_cfg = {
	.name			= "none",
	.ctrl_bus		= "nobus",
	.data_bus		= "nobus",
	.ctrl_busnum	= -1,
	.data_busnum	= -1,
    .spdif_en       = -1,
	.codec			= &subaudio_cfg,
};

struct sd_cfg imapx_sd_cfg = {
	.sd1_Rdelayline_val = -1,
	.sd1_Wdelayline_val = -1,
	.sd2_Rdelayline_val = -1,
	.sd2_Wdelayline_val = -1,
};


#if 0
struct sensor_info camera_front_cfg = {
	.exist = 0,
};

struct sensor_info camera_rear_cfg = {
	.exist = 0,
};
#endif

struct eth_cfg imapx_eth_cfg = {
	.isolate		= -1,
};

struct led_cfg_data imapx_led_cfg_data[BOARD_LED_NUMS] = {
	[0] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[1] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
};

struct led_cfg imapx_led_cfg = {
	.num_leds = 0,
	.led_data = imapx_led_cfg_data,
};

/*
 *  parse WIFI
 */
static int __init imapx_item_parse_wifi(void)
{
	if(item_exist("wifi.model")) {
		if(item_equal("wifi.model","sdio",0))
		{
			printk("==============wifi \n");
			imapx_wifi_cfg.model_bus = 2;
		}
		else if(item_equal("wifi.model","usb",0))
			imapx_wifi_cfg.model_bus = 1;
	}
	return PARSE_OK;
}
/*
 *  parse Backlight
 */

static int __init imapx_item_parse_bl(void)
{
	if (item_exist("bl.power")) {
		if (item_equal("bl.power", "pads", 0))
			imapx_bl_cfg.power_pad = item_integer("bl.power", 1);
		if (item_equal("bl.power", "pmu", 0))
			item_string(imapx_bl_cfg.power_pmu, "bl.power", 1);
	}
	imapx_bl_cfg.pwm_chn =
		item_exist("bl.ctrl") ? item_integer("bl.ctrl", 1) : -ITEMS_EINT;
	return PARSE_OK;
}

/*
 *pasre gpio key
 */
static int __init imapx_item_parse_gpio_key(void)
{
	if (item_exist("board.cpu"))
		item_string(imapx_gpio_key_cfg.compatible, "board.cpu", 0);
	imapx_gpio_key_cfg.menu =
		item_exist("keys.menu") && item_equal("keys.menu", "pads", 0) ? item_integer("keys.menu", 1) : -ITEMS_EINT;
	imapx_gpio_key_cfg.back =
		item_exist("keys.back") && item_equal("keys.back", "pads", 0) ? item_integer("keys.back", 1) : -ITEMS_EINT;
	imapx_gpio_key_cfg.home =
		item_exist("keys.home") && item_equal("keys.home", "pads", 0) ? item_integer("keys.home", 1) : -ITEMS_EINT;
	imapx_gpio_key_cfg.volup =
		item_exist("keys.volup") && item_equal("keys.volup", "pads", 0) ? item_integer("keys.volup", 1) : -ITEMS_EINT;
	imapx_gpio_key_cfg.voldown =
		item_exist("keys.voldown") && item_equal("keys.voldown", "pads", 0) ? item_integer("keys.voldown", 1) : -ITEMS_EINT;
	imapx_gpio_key_cfg.reset =
		item_exist("keys.reset") && item_equal("keys.reset", "pads", 0) ? item_integer("keys.reset", 1) : -ITEMS_EINT;
	return PARSE_OK;
}

/*
 *pasre PMU
 */
static int __init imapx_item_parse_pmu(void)
{
	if (item_exist("pmu.model"))
		item_string(imapx_pmu_cfg.name, "pmu.model", 0);

	if (item_exist("pmu.ctrl")) {
		item_string(imapx_pmu_cfg.ctrl_bus, "pmu.ctrl", 0);
		imapx_pmu_cfg.chan = item_integer("pmu.ctrl", 1);
	}

	if (item_exist("config.otg.drvbus"))
		imapx_pmu_cfg.drvbus = item_integer("config.otg.drvbus", 1);

	if (item_exist("config.otg.extraid"))
		imapx_pmu_cfg.extraid = item_integer("config.otg.extraid", 1);

	if (item_exist("power.acboot"))
		imapx_pmu_cfg.acboot = item_integer("power.acboot", 0);

	if (item_exist("batt.capacity"))
		imapx_pmu_cfg.batt_cap = item_integer("batt.capacity", 0);

	if (item_exist("batt.rdc"))
		imapx_pmu_cfg.batt_rdc = item_integer("batt.rdc", 0);

	return PARSE_OK;
}

/*
 * pasre RTC
 */
static int __init imapx_item_parse_rtc(void)
{
	if (item_exist("rtc.model"))
		item_string(imapx_rtc_cfg.model_name, "rtc.model", 0);
	return PARSE_OK;
}

static int __init imapx_item_parse_usb(void)
{
	imapx_usb_cfg.exist_otg =
		item_exist("soc.usb.otg") ? item_integer("soc.usb.otg", 0) : -ITEMS_EINT;
	return PARSE_OK;
}

static int __init imapx_item_parse_otg(void)
{
	imapx_otg_cfg.exist_otg =
		item_exist("soc.usb.otg") ? item_integer("soc.usb.otg", 0) : -ITEMS_EINT;
	imapx_otg_cfg.extraid	=
		item_exist("config.otg.extraid") ?
		item_integer("config.otg.extraid", 1) : -ITEMS_EINT;

	return PARSE_OK;
}

static int __init imapx_item_parse_audio(void)
{
	if (item_exist("codec.model"))
		item_string(imapx_audio_cfg.name, "codec.model", 0);
	if (item_exist("codec.ctrl")) {
		item_string(imapx_audio_cfg.ctrl_bus, "codec.ctrl", 0);
		imapx_audio_cfg.ctrl_busnum = item_integer("codec.ctrl", 1);
	}
	if (item_exist("codec.data")) {
		item_string(imapx_audio_cfg.data_bus, "codec.data", 0);
		imapx_audio_cfg.data_busnum = item_integer("codec.data", 1);
	}
    imapx_audio_cfg.spdif_en = item_exist("codec.spdif_en") ?
                            item_integer("codec.spdif_en", 0) : -1;

	if (item_exist("codec.power"))
		item_string(subaudio_cfg.power_pmu, "codec.power", 1);
	if (item_exist("codec.clk"))
		item_string(subaudio_cfg.clk_src, "codec.clk", 0);
	subaudio_cfg.spkvolume =
		item_exist("sound.volume.speaker") ?
		item_integer("sound.volume.speaker", 0) : -1;
	subaudio_cfg.micvolume =
		item_exist("sound.volume.mic") ?
		item_integer("sound.volume.mic", 0) : -1;
	subaudio_cfg.hp_io =
		item_exist("sound.hp") ? item_integer("sound.hp", 1) : -1;
	subaudio_cfg.spk_io =
		item_exist("sound.speaker") ?
		item_integer("sound.speaker", 1) : -1;
	subaudio_cfg.highlevel =
		item_exist("codec.hp_highlevel_en") ?
		item_integer("codec.hp_highlevel_en", 0) : -1;
	/*
	 * if i2s_mode = 0, codec in i2s mode
	 * if i2s_mode = -1, codec in pcm mode
	 **/
	if (!strcmp(imapx_audio_cfg.data_bus, "i2s"))
		subaudio_cfg.i2s_mode = IIS_MODE;
	else if (!strcmp(imapx_audio_cfg.data_bus, "pcm"))
		subaudio_cfg.i2s_mode = PCM_MODE;
	else
		subaudio_cfg.i2s_mode = SPDIF_MODE;

	return PARSE_OK;
}

static int __init imapx_item_parse_camera(void)
{
#if 0
	/* front camera sensor */
	if(item_exist("camera.front.interface")){
		item_string(camera_front_cfg.interface ,
				"camera.front.interface", 0);
		camera_front_cfg.exist = 1;
		strcpy(camera_front_cfg.face, "front");

	}
	else{
		camera_front_cfg.exist = 0;
	}

	/*parse front sensor */
	if(camera_front_cfg.exist) {

		if(item_exist("camera.front.model")){
			item_string(camera_front_cfg.name,
				"camera.front.model", 0);
		}

		if(item_exist("camera.front.ctrl")){
			item_string(camera_front_cfg.ctrl_bus,
				"camera.front.ctrl", 0);
			camera_front_cfg.bus_chn =
				item_integer("camera.front.ctrl", 1);
		}

		if(item_exist("camera.front.power.iovdd")){

			if(item_equal("camera.front.power.iovdd", "pmu", 0)) {
				item_string(camera_front_cfg.iovdd,
					"camera.front.power.iovdd", 1);
				camera_front_cfg.iovdd_pad = -1;
			}
			else if(item_equal("camera.front.power.iovdd", "pads", 0)) {
				camera_front_cfg.iovdd_pad =
					item_integer("camera.front.power.iovdd", 1);
			}
	    }
		else {
			camera_front_cfg.iovdd_pad = -1;
		}

		if(item_exist("camera.front.power.dvdd")){

			if(item_equal("camera.front.power.dvdd", "pmu", 0)) {
				item_string(camera_front_cfg.dvdd,
					"camera.front.power.dvdd", 1);
				camera_front_cfg.dvdd_pad = -1;
			}
			else if(item_equal("camera.front.power.dvdd", "pads", 0)) {
				camera_front_cfg.dvdd_pad =
					item_integer("camera.front.power.dvdd", 1);
			}

		}
		else {
			camera_front_cfg.dvdd_pad = -1;
		}

		camera_front_cfg.pdown_pin = item_exist("camera.front.power.down") ?
				item_integer("camera.front.power.down", 1) : -1;

		camera_front_cfg.reset_pin = item_exist("camera.front.reset") ?
				item_integer("camera.front.reset", 1) : -1;

		camera_front_cfg.orientation =
			item_exist("camera.front.orientation") ?
			item_integer("camera.front.orientation", 0) : -1;
	}
	/*  rear */

	if(item_exist("camera.rear.interface")){
		item_string(camera_rear_cfg.interface, "camera.rear.interface", 0);
		camera_rear_cfg.exist = 1;
		strcpy(camera_rear_cfg.face, "rear");
	}
	else{
		camera_rear_cfg.exist = 0;
	}

	if(camera_rear_cfg.exist){

		if(item_exist("camera.rear.model")){
			item_string(camera_rear_cfg.name, "camera.rear.model", 0);
		}

		if(item_exist("camera.rear.ctrl")){
			item_string(camera_rear_cfg.ctrl_bus,
				"camera.rear.ctrl", 0);
			camera_rear_cfg.bus_chn = item_integer("camera.rear.ctrl", 1);
		}

		if(item_exist("camera.rear.power.iovdd")){

			if(item_equal("camera.rear.power.iovdd", "pmu", 0)) {
				item_string(camera_rear_cfg.iovdd,
					"camera.rear.power.iovdd", 1);
				camera_rear_cfg.iovdd_pad = -1;
			}
			else if(item_equal("camera.rear.power.iovdd", "pads", 0)) {
				camera_rear_cfg.iovdd_pad =
					item_integer("camera.rear.power.iovdd", 1);
			}

		}
		else {
			camera_rear_cfg.iovdd_pad = -1;
		}

		if(item_exist("camera.rear.power.dvdd")){

			if(item_equal("camera.rear.power.dvdd", "pmu", 0)) {
				item_string(camera_rear_cfg.dvdd,
					"camera.rear.power.dvdd", 1);
				camera_rear_cfg.dvdd_pad = -1;
			}
			else if(item_equal("camera.rear.power.dvdd", "pads", 0)) {
				camera_rear_cfg.dvdd_pad =
					item_integer("camera.rear.power.dvdd", 1);
			}

		}
		else {
			camera_rear_cfg.dvdd_pad = -1;
		}

		camera_rear_cfg.pdown_pin = item_exist("camera.rear.power.down") ?
			item_integer("camera.rear.power.down", 1) : -1;

		camera_rear_cfg.reset_pin =	item_exist("camera.rear.reset") ?
			item_integer("camera.rear.reset", 1) : -1;

		camera_rear_cfg.orientation =
			item_exist("camera.rear.orientation") ?
			item_integer("camera.rear.orientation", 0) : -1;
	}

#endif

	return PARSE_OK;

}

static int __init imapx_item_parse_eth(void)
{
	char key[] = "eth.isolate";
	char value[ITEM_MAX_LEN];
	if (item_exist(key)) {
		item_string(value, key, 1);
		imapx_eth_cfg.isolate = simple_strtol(value, NULL, 10);
	}
	if (imapx_eth_cfg.isolate < 0)
		imapx_eth_cfg.isolate = 0;
	return PARSE_OK;
}

static int __init imapx_item_parse_led(void)
{
	int i, pwm_id;
	char pwm[32],gpio[32],polarity[32],state[32],sts[32];

    imapx_led_cfg.num_leds = 0;
	for(i = 0; i < ARRAY_SIZE(imapx_led_cfg_data); i++) {

		sprintf(pwm, "led%d.pwm", i);
		sprintf(gpio, "led%d.gpio", i);
		sprintf(polarity, "led%d.polarity", i);
		sprintf(state, "led%d.dft_state", i);

		if (!(item_exist(pwm)) && !(item_exist(gpio))) {
			pr_err("no led%d control pad exists\n",i);
			continue;
		}
		if(item_exist(pwm) || item_exist(gpio)) {
            imapx_led_cfg.num_leds++;
        }
		if(item_exist(gpio)) {
			item_string(imapx_led_cfg.led_data[i].model_name, gpio, 0);
			imapx_led_cfg.led_data[i].gpio = item_integer(gpio, 1);
		}

        if(item_exist(pwm)) {
			item_string(imapx_led_cfg.led_data[i].model_name, pwm, 0);
			pwm_id = item_integer(pwm, 1);
			switch(pwm_id) { /* PWM_TOUT0: PWM_TOU1, PWM_TOU2 */
				case 0:
					imapx_led_cfg.led_data[i].gpio = 59; /* PWM_TOUT0 */
					break;
				case 1:
					imapx_led_cfg.led_data[i].gpio = 58; /* PWM_TOU1 */
					break;
				case 2:
					imapx_led_cfg.led_data[i].gpio = 57; /* PWM_TOU2 */
					break;
                default:
                    imapx_led_cfg.led_data[i].gpio = -ITEMS_EINT;
                    break;
		    }
		}

		if(item_exist(polarity))
			imapx_led_cfg.led_data[i].polarity = item_integer(polarity, 0);

		if(item_exist(state)) {
			item_string(sts, state, 0);
			if (!strcmp(sts, "off"))
				imapx_led_cfg.led_data[i].default_state = 0;
			else if (!strcmp(sts, "on"))
				imapx_led_cfg.led_data[i].default_state = 1;
			else if (!strcmp(sts, "blink")) {
				imapx_led_cfg.led_data[i].default_state = 2;
				imapx_led_cfg.led_data[i].delay_on = item_integer(state, 1);
				imapx_led_cfg.led_data[i].delay_off = item_integer(state, 2);
		    }
			else if (!strcmp(sts, "nocare"))
				imapx_led_cfg.led_data[i].default_state = 3;
			else
				imapx_led_cfg.led_data[i].default_state = 0;
			}
		}

	return PARSE_OK;
}

static int __init imapx_item_parse_sd(void){
	char rdwd[4];
	int ret;
	if(item_exist("sd1.delayline")){
		ret = item_string(&rdwd[0], "sd1.delayline", 0);
		if(ret)
			return -ITEMS_EINT;
		ret = item_string(&rdwd[2], "sd1.delayline", 2);
		if(ret)
			return -ITEMS_EINT;
		if(!strcmp(&rdwd[0], "R"))
			imapx_sd_cfg.sd1_Rdelayline_val = item_integer("sd1.delayline",1);
		else
			imapx_sd_cfg.sd1_Wdelayline_val = item_integer("sd1.delayline",1);
		if(!strcmp(&rdwd[2], "R"))
			imapx_sd_cfg.sd1_Rdelayline_val = item_integer("sd1.delayline",3);
		else
			imapx_sd_cfg.sd1_Wdelayline_val = item_integer("sd1.delayline",3);
	}
	if(item_exist("sd2.delayline")){
		memset(rdwd, 4, 0);
		ret = item_string(&rdwd[0], "sd2.delayline", 0);
		if(ret)
			return -ITEMS_EINT;
		ret = item_string(&rdwd[2], "sd2.delayline", 2);
		if(ret)
			return -ITEMS_EINT;
		if(!strcmp(&rdwd[0], "R"))
			imapx_sd_cfg.sd2_Rdelayline_val = item_integer("sd2.delayline",1);
		else
			imapx_sd_cfg.sd2_Wdelayline_val = item_integer("sd2.delayline",1);
		if(!strcmp(&rdwd[2], "R"))
			imapx_sd_cfg.sd2_Rdelayline_val = item_integer("sd2.delayline",3);
		else
			imapx_sd_cfg.sd2_Wdelayline_val = item_integer("sd2.delayline",3);
	}

	return PARSE_OK;
}


void __init imapx_item_parse(struct module_parse_status *mps, int nr)
{
	int i;
	for (i = 0; i < nr; i++) {
		if (mps->need_parse == 1) {
			switch (mps->imapx_module_type) {
			case IMAPX_GPIO_KEY:
				mps->parse_done = imapx_item_parse_gpio_key();
				break;
			case IMAPX_WIFI:
				mps->parse_done = imapx_item_parse_wifi();
					break;
			case IMAPX_RTC:
				mps->parse_done = imapx_item_parse_rtc();
					break;
			case IMAPX_BL:
				mps->parse_done = imapx_item_parse_bl();
					break;
			case IMAPX_PMU:
				mps->parse_done = imapx_item_parse_pmu();
				break;
			case IMAPX_USB:
				mps->parse_done = imapx_item_parse_usb();
				break;
			case IMAPX_DWC_OTG:
				mps->parse_done = imapx_item_parse_otg();
				break;
			case IMAPX_AUDIO:
				mps->parse_done = imapx_item_parse_audio();
				break;
			case IMAPX_CAMERA:
				mps->parse_done = imapx_item_parse_camera();
				break;
			case IMAPX_ETH:
				mps->parse_done = imapx_item_parse_eth();
				break;
            case IMAPX_LED:
                mps->parse_done = imapx_item_parse_led();
                break;
            case IMAPX_SD:
				mps->parse_done = imapx_item_parse_sd();
				break;
			default:
				break;
			}
		}
		mps++;
	}
	return;
}
