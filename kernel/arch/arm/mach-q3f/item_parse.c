
#include <linux/kernel.h>
#include <linux/platform_data/camera-imapx.h>
#include <linux/input.h>
#include <mach/hw_cfg.h>
#include <mach/audio.h>
#include <linux/slab.h>


#define strtol simple_strtol

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
	.core_voltage = -1,
};

struct wifi_cfg imapx_wifi_cfg = {
	.model_name	= "NOWIFI",
	.model_bus		= 0, //"NOBUS",
	.power_pmu		= "NOPMU",
	.power_pad		= -1,
};

struct sd_cfg imapx_sd_cfg = {
	.sd1_Rdelayline_val = -1,
	.sd1_Wdelayline_val = -1,
	.sd2_Rdelayline_val = -1,
	.sd2_Wdelayline_val = -1,
};

struct rtc_cfg imapx_rtc_cfg = {
	.model_name	= "imapx",
};

struct bl_cfg imapx_bl_cfg = {
	.pwm_id			= -1,
	.pwm_period_ns	= -1,
	.max_brightness = -1,
	.dft_brightness = -1,
	.lth_brightness = -1,
	.polarity		= -1,

};

struct led_cfg_data imapx_led_cfg_data[10] = {
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
	[2] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[3] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[4] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[5] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[6] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[7] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[8] = {
			.model_name	= "NOLED",
			.gpio = -1,
			.polarity = -1,
			.default_state = -1,
			.delay_on = -1,
			.delay_off = -1,
		 },
	[9] = {
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

struct battery_cfg imapx_battery_cfg = {
	//.model = "NOBATTVOLT",
	.adc_chan = -1,
	.divider_ratio = -1,
	//.ref_volt = -1,
	//.adc_bits = -1,
};

struct lightsen_hw_cfg imapx_lightsensor_cfg = {
	.model = "NOLIGHTSEN",
	.bus = "NOBUS",
	.chan = -1,
};

struct usb_cfg imapx_usb_cfg = {
	.exist_otg      = -1,
};

struct otg_cfg imapx_otg_cfg = {
	.exist_otg      = -1,
	.extraid        = 0,
	.drvvbus        = 0,
};

struct codec_cfg subaudio_cfg[2] = {
	[0] = {
		.power_pmu		= "nopmu",
		.spkvolume		= -1,
		.micvolume		= -1,
		.hp_io			= -1,
		.spk_io			= -1,
		.highlevel		= -1,
		.i2s_mode		= -1,
	},
	[1] = {
		.power_pmu		= "nopmu",
		.spkvolume		= -1,
		.micvolume		= -1,
		.hp_io			= -1,
		.spk_io			= -1,
		.highlevel		= -1,
		.i2s_mode		= -1,
	},
};

struct audio_cfg imapx_audio_cfg[2] = {
	[0] = {
		.name			= "none",
		.ctrl_bus		= "nobus",
		.data_bus		= "nobus",
		.ctrl_busnum	= -1,
		.data_busnum	= -1,
    	.spdif_en       = -1,
		.codec			= &subaudio_cfg[0],
	},
	[1] = {
		.name			= "none",
		.ctrl_bus		= "nobus",
		.data_bus		= "nobus",
		.ctrl_busnum	= -1,
		.data_busnum	= -1,
    	.spdif_en       = -1,
		.codec			= &subaudio_cfg[1],
	}
};

struct adc_keys_button adc_buttons_cfg[MAX_ADC_KEYS_NUM];

struct eth_cfg imapx_eth_cfg = {
	.isolate		= -1,
	.clk_provide    = -1,
	.reset_gpio		= -1,
	.model			= "none",
};

/*
 *  parse WIFI
 */
static int __init imapx_item_parse_wifi(void)
{
	if(item_exist("wifi.model")) 
	{
		if(item_equal("wifi.model","sdio",0))
		{
			//printk("-----model_bus = 2\n");
			imapx_wifi_cfg.model_bus = 2;
		}
		else if(item_equal("wifi.model","usb",0))
			imapx_wifi_cfg.model_bus = 1;
	}

	return PARSE_OK;
}

/*
 *  parse battery voltage adc
 */

static int __init imapx_item_parse_battery(void) 
{
	/* 
	if(item_exist("power.model")) {
		item_string(imapx_battery_cfg.model, "power.model", 0);
	}
	*/
	
	if(item_exist("power.ctrl") && item_equal("power.ctrl", "adc", 0)) {
		imapx_battery_cfg.adc_chan = item_integer("power.ctrl", 1);
		imapx_battery_cfg.divider_ratio = item_integer("power.ctrl", 2);
		//imapx_battery_cfg.ref_volt = item_integer("power.ctrl", 3);
		//imapx_battery_cfg.adc_bits = item_integer("power.ctrl", 4);
		return PARSE_OK;
 	}
	else {
		return PARSE_FAILED;
	}
}

/*
 *  parse Backlight
 */

static int __init imapx_item_parse_lightsensor(void) 
{
	if(item_exist("lightsensor.model")) {
		item_string(imapx_lightsensor_cfg.model, "lightsensor.model", 0);
	}
	
	if(item_exist("lightsensor.bus")) {
		item_string(imapx_lightsensor_cfg.bus, "lightsensor.bus", 0);
		imapx_lightsensor_cfg.chan = item_integer("lightsensor.bus", 1);
	}
	return PARSE_OK;

}

static int __init imapx_item_parse_bl(void)
{
	if (item_exist("bl.power")) {
		if (item_equal("bl.power", "pads", 0))
			imapx_bl_cfg.power_pad = item_integer("bl.power", 1);
		if (item_equal("bl.power", "pmu", 0))
			item_string(imapx_bl_cfg.power_pmu, "bl.power", 1);
	}
	imapx_bl_cfg.pwm_id =
		item_exist("bl.ctrl") ? item_integer("bl.ctrl", 1) : -ITEMS_EINT;
	imapx_bl_cfg.pwm_period_ns =
		item_exist("bl.periodns") ? item_integer("bl.periodns", 0) : -ITEMS_EINT;
	imapx_bl_cfg.max_brightness =
		item_exist("bl.max_brightness") ? item_integer("bl.max_brightness", 0): -ITEMS_EINT;
	imapx_bl_cfg.dft_brightness =
		item_exist("bl.dft_brightness") ? item_integer("bl.dft_brightness", 0): -ITEMS_EINT;
	imapx_bl_cfg.lth_brightness =
		item_exist("bl.min_brightness") ? item_integer("bl.min_brightness", 0): -ITEMS_EINT;
	imapx_bl_cfg.polarity =
		item_exist("bl.polarity") ? item_integer("bl.polarity", 0): -ITEMS_EINT;
	return PARSE_OK;
}

/*
 *  parse Led
 */

static int __init imapx_item_parse_led(void)
{
	int i,j,pwm_id;
	char pwm[32],gpio[32],polarity[32],state[32],sts[32];
	struct led_cfg_data tmp;

	for(i=0;i<10;i++){

		sprintf(pwm, "led%d.pwm", i);
		sprintf(gpio, "led%d.gpio", i);
		sprintf(polarity, "led%d.polarity", i);
		sprintf(state, "led%d.dft_state", i);

		if (!(item_exist(pwm)) && !(item_exist(gpio))) {
			pr_err("no led%d control pad exists\n",i);
			continue;
		}

		if(item_exist(pwm)||item_exist(gpio)) imapx_led_cfg.num_leds++;

		if(item_exist(gpio)) {

			item_string(imapx_led_cfg.led_data[i].model_name, gpio, 0);
			imapx_led_cfg.led_data[i].gpio = item_integer(gpio, 1);

			}else if(item_exist(pwm)){

			item_string(imapx_led_cfg.led_data[i].model_name, pwm, 0);
			pwm_id = item_integer(pwm, 1);

			switch(pwm_id)
				{
					case 0:
						imapx_led_cfg.led_data[i].gpio = 107;
						break;
					case 1:
						imapx_led_cfg.led_data[i].gpio = 108;
						break;
					case 2:
						imapx_led_cfg.led_data[i].gpio = 120;
						break;
					case 3:
						imapx_led_cfg.led_data[i].gpio = 121;
						break;
				}

			}
		if(item_exist(polarity))
			imapx_led_cfg.led_data[i].polarity = item_integer(polarity, 0);
		if(item_exist(state)){
			item_string(sts, state, 0);

			if (!strcmp(sts, "off"))
				imapx_led_cfg.led_data[i].default_state = 0;
			else if (!strcmp(sts, "on"))
				imapx_led_cfg.led_data[i].default_state = 1;
			else if (!strcmp(sts, "blink")){
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
	for(i =0 ; i< 4; i++) {
        for(j = 0; j < 4-i; j++) {
            if(imapx_led_cfg.led_data[j].gpio < imapx_led_cfg.led_data[j+1].gpio) {
                tmp = imapx_led_cfg.led_data[j];
				imapx_led_cfg.led_data[j] = imapx_led_cfg.led_data[j+1];
				imapx_led_cfg.led_data[j+1] = tmp;
            }
        }
    }

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


static int __init imapx_item_parse_adc_key(void) {
	memset(adc_buttons_cfg, 0x0, MAX_ADC_KEYS_NUM * sizeof(struct adc_keys_button)); 

	if(item_exist("keys.menu") && item_equal("keys.menu", "adc", 0)) {
		adc_buttons_cfg[0].valid = true;
		adc_buttons_cfg[0].code = KEY_MENU; 
		adc_buttons_cfg[0].chan = item_integer("keys.menu", 1);
		adc_buttons_cfg[0].ref_val = item_integer("keys.menu", 2);
		adc_buttons_cfg[0].desc = "menu";
	}
	if(item_exist("keys.up") && item_equal("keys.up", "adc", 0)) {
		adc_buttons_cfg[1].valid = true;
		adc_buttons_cfg[1].code = KEY_UP; 
		adc_buttons_cfg[1].chan = item_integer("keys.up", 1);
		adc_buttons_cfg[1].ref_val = item_integer("keys.up", 2);
		adc_buttons_cfg[1].desc = "up";
	}
	if(item_exist("keys.down") && item_equal("keys.down", "adc", 0)) {
		adc_buttons_cfg[2].valid = true;
		adc_buttons_cfg[2].code = KEY_DOWN; 
		adc_buttons_cfg[2].chan = item_integer("keys.down", 1);
		adc_buttons_cfg[2].ref_val = item_integer("keys.down", 2);
		adc_buttons_cfg[2].desc = "down";
	}
	if(item_exist("keys.ok") && item_equal("keys.ok", "adc", 0)) {
		adc_buttons_cfg[3].valid = true;
		adc_buttons_cfg[3].code = KEY_OK; 
		adc_buttons_cfg[3].chan = item_integer("keys.ok", 1);
		adc_buttons_cfg[3].ref_val = item_integer("keys.ok", 2);
		adc_buttons_cfg[3].desc = "ok";
	}
	if(item_exist("keys.reset") && item_equal("keys.reset", "adc", 0)) {
		adc_buttons_cfg[4].valid = true;
		adc_buttons_cfg[4].code = KEY_RESTART; 
		adc_buttons_cfg[4].chan = item_integer("keys.reset", 1);
		adc_buttons_cfg[4].ref_val = item_integer("keys.reset", 2);
		adc_buttons_cfg[4].desc = "reset";
	}
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

	if(item_exist("core.voltage"))
		imapx_pmu_cfg.core_voltage = item_integer("core.voltage", 0);

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
		item_integer("config.otg.extraid", 1) : 0;
	imapx_otg_cfg.drvvbus =
		item_exist("config.otg.drvbus") ?
		item_integer("config.otg.drvbus", 1) : 0;
	return PARSE_OK;
}

static int __init imapx_item_parse_audio(void)
{
	if (item_exist("codec.model"))
		item_string(imapx_audio_cfg[0].name, "codec.model", 0);
	if (item_exist("codec.ctrl")) {
		item_string(imapx_audio_cfg[0].ctrl_bus, "codec.ctrl", 0);
		imapx_audio_cfg[0].ctrl_busnum = item_integer("codec.ctrl", 1);
	}
	if (item_exist("codec.data")) {
		item_string(imapx_audio_cfg[0].data_bus, "codec.data", 0);
		imapx_audio_cfg[0].data_busnum = item_integer("codec.data", 1);
	}
    imapx_audio_cfg[0].spdif_en = item_exist("codec.spdif_en") ?
                            item_integer("codec.spdif_en", 0) : -1;

	if (item_exist("codec.power"))
		item_string(subaudio_cfg[0].power_pmu, "codec.power", 1);
	if (item_exist("codec.clk"))
		item_string(subaudio_cfg[0].clk_src, "codec.clk", 0);
	subaudio_cfg[0].spkvolume =
		item_exist("sound.volume.speaker") ?
		item_integer("sound.volume.speaker", 0) : -1;
	subaudio_cfg[0].micvolume =
		item_exist("sound.volume.mic") ?
		item_integer("sound.volume.mic", 0) : -1;
	subaudio_cfg[0].hp_io =
		item_exist("sound.hp") ? item_integer("sound.hp", 1) : -1;
	subaudio_cfg[0].spk_io =
		item_exist("sound.speaker") ?
		item_integer("sound.speaker", 1) : -1;
	subaudio_cfg[0].highlevel =
		item_exist("codec.hp_highlevel_en") ?
		item_integer("codec.hp_highlevel_en", 0) : -1;
				 
					
	/*
	 * if i2s_mode = 0, codec in i2s mode
	 * if i2s_mode = -1, codec in pcm mode
	 **/
	if (!strcmp(imapx_audio_cfg[0].data_bus, "i2s"))
		subaudio_cfg[0].i2s_mode = IIS_MODE;
	else if (!strcmp(imapx_audio_cfg[0].data_bus, "pcm"))
		subaudio_cfg[0].i2s_mode = PCM_MODE;
	else
		subaudio_cfg[0].i2s_mode = SPDIF_MODE;
		
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////		
		if (item_exist("codec.model1"))
		item_string(imapx_audio_cfg[1].name, "codec.model1", 0);
	if (item_exist("codec.ctrl1")) {
		item_string(imapx_audio_cfg[1].ctrl_bus, "codec.ctrl1", 0);
		imapx_audio_cfg[1].ctrl_busnum = item_integer("codec.ctrl1", 1);
	}
	if (item_exist("codec.data1")) {
		item_string(imapx_audio_cfg[1].data_bus, "codec.data1", 0);
		imapx_audio_cfg[1].data_busnum = item_integer("codec.data1", 1);
	}
    imapx_audio_cfg[1].spdif_en = item_exist("codec.spdif_en1") ?
                            item_integer("codec.spdif_en1", 0) : -1;

	if (item_exist("codec.power1"))
		item_string(subaudio_cfg[1].power_pmu, "codec.power1", 1);
	subaudio_cfg[1].spkvolume =
		item_exist("sound.volume.speaker1") ?
		item_integer("sound.volume.speaker1", 0) : -1;
	subaudio_cfg[1].micvolume =
		item_exist("sound.volume.mic1") ?
		item_integer("sound.volume.mic1", 0) : -1;
	subaudio_cfg[1].hp_io =
		item_exist("sound.hp1") ? item_integer("sound.hp1", 1) : -1;
	subaudio_cfg[1].spk_io =
		item_exist("sound.speaker1") ?
		item_integer("sound.speaker1", 1) : -1;
	subaudio_cfg[1].highlevel =
		item_exist("codec.hp_highlevel_en1") ?
		item_integer("codec.hp_highlevel_en1", 0) : -1;
	 
					
	/*
	 * if i2s_mode = 0, codec in i2s mode
	 * if i2s_mode = -1, codec in pcm mode
	 **/
	if (!strcmp(imapx_audio_cfg[1].data_bus, "i2s"))
		subaudio_cfg[1].i2s_mode = IIS_MODE;
	else if (!strcmp(imapx_audio_cfg[1].data_bus, "pcm"))
		subaudio_cfg[1].i2s_mode = PCM_MODE;
	else
		subaudio_cfg[1].i2s_mode = SPDIF_MODE;

	return PARSE_OK;
}

static int __init imapx_item_parse_camera(void)
{
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
	if (item_exist("eth.clk")) {
		imapx_eth_cfg.clk_provide = item_integer("eth.clk", 0);
	}
	if (item_exist("eth.reset")) {
		imapx_eth_cfg.reset_gpio = item_integer("eth.reset", 0);
	}
	if (item_exist("eth.model")) {
		item_string(imapx_eth_cfg.model, "eth.model", 0); 
	}
	return PARSE_OK;
}

static int __init imapx_item_parse_sd(void)
{
	char rdwd0[4];
	char rdwd1[4];
	int ret;
	if(item_exist("sd1.delayline")){
		memset(rdwd0, 4, 0);
		memset(rdwd1, 4, 0);
		ret = item_string(rdwd0, "sd1.delayline", 0);
		if(ret)
			return -ITEMS_EINT;
		ret = item_string(rdwd1, "sd1.delayline", 2);
		if(ret)
			return -ITEMS_EINT;
		if(!strcmp(rdwd0, "R"))
			imapx_sd_cfg.sd1_Rdelayline_val = item_integer("sd1.delayline",1);
		else
			imapx_sd_cfg.sd1_Wdelayline_val = item_integer("sd1.delayline",1);
		if(!strcmp(rdwd1, "R"))
			imapx_sd_cfg.sd1_Rdelayline_val = item_integer("sd1.delayline",3);
		else
			imapx_sd_cfg.sd1_Wdelayline_val = item_integer("sd1.delayline",3);
	}
	if(item_exist("sd2.delayline")){
		memset(rdwd0, 4, 0);
		memset(rdwd1, 4, 0);
		ret = item_string(rdwd0, "sd2.delayline", 0);
		if(ret)
			return -ITEMS_EINT;
		ret = item_string(rdwd1, "sd2.delayline", 2);
		if(ret)
			return -ITEMS_EINT;
		if(!strcmp(rdwd0, "R"))
			imapx_sd_cfg.sd2_Rdelayline_val = item_integer("sd2.delayline",1);
		else
			imapx_sd_cfg.sd2_Wdelayline_val = item_integer("sd2.delayline",1);
		if(!strcmp(rdwd1, "R"))
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
			case IMAPX_KEY:
				mps->parse_done = imapx_item_parse_gpio_key();
				mps->parse_done = imapx_item_parse_adc_key();
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
			case IMAPX_LED:
				mps->parse_done = imapx_item_parse_led();
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
			case IMAPX_LS_HWMON:
				mps->parse_done = imapx_item_parse_lightsensor();
				break;
			case IMAPX_BATTERY:
				mps->parse_done = imapx_item_parse_battery();
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
