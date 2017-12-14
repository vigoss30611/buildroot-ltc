/* display hdmi driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/display_device.h>
#include <mach/pad.h>
#include <mach/items.h>
#include <asm/io.h>

#include "display_register.h"
#include "display_items.h"

#include "access.h"
#include "api.h"
#include "dtd.h"
#include "videoParams.h"
#include "audioParams.h"
#include "productParams.h"
#include "hdcpParams.h"
#include "control.h"
#include "hdcp.h"
#include "log.h"

#define DISPLAY_MODULE_LOG		"hdmi_tx"

/* item not config hdmi, hdmi not present on product */
static int hdmi_not_available = 0;

/* use gpio irq instead of hdmi phy irq, hdmi clk can be disabled when draw out */
static int gpio_irq = 0;

/* enable hdcp or not */
static int hdmi_hdcp = 0;

/* refers to enum display_hdmidvi_set */
static int hdmi_dvi = 0;

/* hdmi output ycc instead of rgb */
static int hdmi_ycc = DISPLAY_HDMI_YCC_DISABLE;

static unsigned char src_type = 0x0D;	// PMP

/* old hpd state */
static int old_hpd_state = 0;

extern verbose_t log_Verbose;


static videoParams_t video_param;
static audioParams_t audio_param;
static productParams_t product_param;
static hdcpParams_t hdcp_param;

static int hdmi_tx_enable(struct display_device *pdev, int en);



static ssize_t hdmi_hpd_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	struct display_device *pdev = to_display_device(dev);
	return sprintf(buf, "%d\n", pdev->hpd?1:0);
}

static ssize_t hdmi_log_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", log_Verbose);
}

static ssize_t hdmi_log_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long data;

	ret = kstrtoul(buf, 10, &data);
	if (ret) {
		dlog_err("NOT a valid number\n");
		return ret;
	}

	dlog_dbg("log_Verbose=%ld\n", data);

	log_Verbose = data;

	return count;
}

static ssize_t hdmi_ycc_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", hdmi_ycc);
}

static ssize_t hdmi_ycc_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct display_device *pdev = to_display_device(dev);
	int ret;
	unsigned long data;

	ret = kstrtoul(buf, 10, &data);
	if (ret) {
		dlog_err("NOT a valid number\n");
		return ret;
	}
	if (data > DISPLAY_HDMI_YCC444_FORCE) {
		dlog_err("Invalid param %ld\n", data);
		return ret;
	}

	dlog_dbg("hdmi_ycc=%ld\n", data);

	hdmi_ycc = data;

	if (pdev->enable) {
		hdmi_tx_enable(pdev, 0);
		hdmi_tx_enable(pdev, 1);
	}

	return count;
}

static struct device_attribute hdmi_tx_attrs[] = {
	__ATTR(hdmi_hpd, 0444, hdmi_hpd_show, NULL),
	__ATTR(hdmi_log, 0666, hdmi_log_show, hdmi_log_store),
	__ATTR(hdmi_ycc, 0666, hdmi_ycc_show, hdmi_ycc_store),
	__ATTR_NULL,
};

static inline bool __hdmi_is_4k_vic(int vic)
{
	if (vic == 95 || vic == 94 || vic == 93 || vic == 98)
		return TRUE;
	else
		return FALSE;
}

static int __hdmi_get_format(void)
{
	encoding_t format;
	
	if (hdmi_ycc == DISPLAY_HDMI_YCC422_FORCE) {
		dlog_dbg("Force TMDS YCC422\n");
		format = YCC422;
	}
	else if (hdmi_ycc == DISPLAY_HDMI_YCC422_AUTO) {
		if (api_EdidSupportsYcc422()) {
			dlog_dbg("Auto select TMDS YCC422\n");
			format = YCC422;
		}
		else {
			dlog_dbg("Auto select TMDS RGB while sink NOT support YCC422\n");
			format = RGB;
		}
	}
	else if (hdmi_ycc == DISPLAY_HDMI_YCC444_FORCE) {
		dlog_dbg("Force TMDS YCC444\n");
		format = YCC444;
	}
	else if (hdmi_ycc == DISPLAY_HDMI_YCC444_AUTO) {
		if (api_EdidSupportsYcc444()) {
			dlog_dbg("Auto select TMDS YCC444\n");
			format = YCC444;
		}
		else {
			dlog_dbg("Auto select TMDS RGB while sink NOT support YCC444\n");
			format = RGB;
		}
	}
	else {
		dlog_dbg("TMDS RGB\n");
		format = RGB;
	}

	return format;
}

static int __hdmi_config_video_params(videoParams_t * param_video, 
					int vic, enum display_hdmidvi_set hdmi_dvi, encoding_t format)
{
	dtd_t dtd;
	hdmivsdb_t vsdb;
	int hdmi;
	
	if (hdmi_dvi == DISPLAY_HDMIDVI_AUTO_DETECT) {
		if (api_GetEdidState() != EDID_IDLE) {
			dlog_dbg("EDID read failed, use HDMI mode\n");
			hdmi = 1;
		}
		else if (api_EdidHdmivsdb(&vsdb) && api_EdidSupportsBasicAudio()) {
			dlog_dbg("EDID support audio, use HDMI mode\n");
			hdmi = 1;
		}
		else {
			dlog_dbg("Other cases, use DVI mode\n");
			hdmi = 0;
		}
	}
	else if (hdmi_dvi == DISPLAY_HDMIDVI_FORCE_HDMI) {
		dlog_dbg("Force HDMI mode\n");
		hdmi = 1;
	}
	else if (hdmi_dvi == DISPLAY_HDMIDVI_FORCE_DVI) {
		dlog_dbg("Force DVI mode\n");
		hdmi = 0;
	}
	else {
		dlog_err("Invalid hdmi_dvi mode %d\n", hdmi_dvi);
		return -EINVAL;
	}

#if 0
	if (format == YCC422)
		dlog_dbg("YCC422\n");
	else if (format == YCC444)
		dlog_dbg("YCC444\n");
	else
		dlog_dbg("RGB\n");
#endif

	videoParams_Reset(param_video);
	dtd_Fill(&dtd, vic, 60000);	
	videoParams_SetHdmi(param_video, hdmi);
	videoParams_SetEncodingIn(param_video, RGB);
	videoParams_SetEncodingOut(param_video, format); 
	videoParams_SetColorResolution(param_video, 8);
	videoParams_SetDtd(param_video, &dtd);
	videoParams_SetPixelRepetitionFactor(param_video, 0);
	videoParams_SetPixelPackingDefaultPhase(param_video, 0);
	videoParams_SetColorimetry(param_video, ITU709);

	if (__hdmi_is_4k_vic(vic))
		videoParams_SetHdmiVideoFormat(param_video, 1);
	else
		videoParams_SetHdmiVideoFormat(param_video, 0);

	//videoParams_SetHdmiVideoFormat(param_video, 0);	// 3D relation
	videoParams_Set3dStructure(param_video, 0);				// 3D relation
	videoParams_Set3dExtData(param_video, 0);					// 3D relation

	return 0;
}

static int __hdmi_config_audio_params(audioParams_t * param_audio)
{
	audioParams_Reset(param_audio);
	audioParams_SetInterfaceType(param_audio, I2S);
	audioParams_SetCodingType(param_audio, PCM);
	audioParams_SetChannelAllocation(param_audio, 0);
	audioParams_SetPacketType(param_audio, AUDIO_SAMPLE);
	audioParams_SetSampleSize(param_audio, 16);
	audioParams_SetSamplingFrequency(param_audio, 48000);
	audioParams_SetLevelShiftValue(param_audio, 0);
	audioParams_SetDownMixInhibitFlag(param_audio, 0);
	audioParams_SetClockFsFactor(param_audio, 64);		// 64:I2S, 128:SPDIF

	return 0;
}

static int __hdmi_config_product_params(productParams_t * param_product, 
			char *vendor_name, char *product_name, unsigned char source_type)
{
	productParams_Reset(param_product);
	productParams_SetVendorName(param_product, vendor_name, strlen(vendor_name));
	productParams_SetProductName(param_product, product_name, strlen(product_name));
	productParams_SetSourceType(param_product, source_type);

	return 0;
}

static int __hdmi_config_product_payload(productParams_t * param_product, int vic)
{
	unsigned char data[2];
	int ext_vic = 0;

	productParams_SetOUI(param_product, 0x000C03);

	if (vic == 95)
		ext_vic = 1;
	else if (vic == 94)
		ext_vic = 2;
	else if (vic == 93)
		ext_vic = 3;
	else if (vic == 98)
		ext_vic = 4;
	
	if (ext_vic) {
		data[0] = (1<<5);	// 4K * 2K
		data[1] = ext_vic;
		productParams_SetVendorPayload(param_product, data, sizeof(data));
	}
	else {
		data[0] = 0;
		data[1] = 0;
		productParams_SetVendorPayload(param_product, data, 0);
	}

	return 0;
}

static int __hdmi_config_hdcp_params(hdcpParams_t * param_hdcp)
{
	hdcpParams_Reset(param_hdcp);
	hdcpParams_SetEnable11Feature(param_hdcp, 1);
	hdcpParams_SetEnhancedLinkVerification(param_hdcp, 1);
	hdcpParams_SetI2cFastMode(param_hdcp, 1);
	hdcpParams_SetRiCheck(param_hdcp, 1);

	return 0;
}

static int __hdmi_default_config(void)
{
	char str[64] = {0};
	int vic = 4;
	
	dlog_trace();

	__hdmi_config_video_params(&video_param, vic, hdmi_dvi, RGB);
	__hdmi_config_audio_params(&audio_param);
	__hdmi_config_hdcp_params(&hdcp_param);

	if (item_exist(DISPLAY_ITEM_PRODUCT_NAME))
		item_string(str, DISPLAY_ITEM_PRODUCT_NAME, 0);
	else
		strcpy(str, "InfoTM");
	__hdmi_config_product_params(&product_param, "InfoTM", str, src_type);

	return 0;
}

static int __hdmi_config_video(struct display_device *pdev)
{
	encoding_t format;
	int vic = pdev->vic;
	
	format = __hdmi_get_format();
	__hdmi_config_video_params(&video_param, vic, hdmi_dvi, format);
	__hdmi_config_product_payload(&product_param, vic);
	
	return 0;
}

int hdmi_set_audio_basic_config(audioParams_t *audio_t)
{
	dlog_dbg("SampleSize=%d, SamplingFrequency=%d\n", 
						audio_t->mSampleSize, audio_t->mSamplingFrequency);
   	audio_param.mSampleSize = audio_t->mSampleSize;
    	audio_param.mSamplingFrequency = audio_t->mSamplingFrequency;
	api_Audio_Cfg(&video_param, &audio_param, &product_param,
								(hdmi_hdcp? &hdcp_param : NULL));
	
	return 0;
}
EXPORT_SYMBOL(hdmi_set_audio_basic_config);

static int __hdmi_clk(int en)
{
	struct clk *clkp;
	int ret;

	dlog_dbg("hdmi clk %s\n", en?"enable":"disable");
	
	clkp = clk_get_sys(HDMI_BUS_CLK_SRC, HDMI_BUS_CLK_SRC_PARENT);
	if (IS_ERR(clkp)) {
		dlog_err("Failed to get clock %s\n", HDMI_BUS_CLK_SRC);
		return PTR_ERR(clkp);
	}
	if (en)
		clk_prepare_enable(clkp);
	else
		clk_disable_unprepare(clkp);

	clkp = clk_get_sys(HDMI_CLK_SRC, HDMI_CLK_SRC_PARENT);
	if (IS_ERR(clkp)) {
		dlog_err("Failed to get clock %s\n", HDMI_BUS_CLK_SRC);
		return PTR_ERR(clkp);
	}
	ret = clk_set_rate(clkp, HDMI_CLK_FREQ);
    if (ret < 0) {
        dlog_err("Fail to set hdmi clk to freq %d\n", HDMI_CLK_FREQ);
        return ret;
    }
	if (en)
		clk_prepare_enable(clkp);
	else
		clk_disable_unprepare(clkp);

	return 0;
}

static int __hdmi_hpd_event(struct display_device *pdev, int hpd)
{
	sysfs_notify(&pdev->dev.kobj, NULL, "hdmi_hpd");
	return kobject_uevent(&pdev->dev.kobj, (hpd ? KOBJ_ONLINE : KOBJ_OFFLINE));
}

static int __hdmi_hpd_state(void)
{
	if (gpio_irq)
		return gpio_get_value(gpio_irq);
	else
		return api_HotPlugDetected();
}

static int __hdmi_hpd_changed(void)
{
	if (__hdmi_hpd_state() != old_hpd_state)
		return TRUE;
	else
		return FALSE;
}

static void __hdmi_hpd_update(void)
{	
	old_hpd_state = __hdmi_hpd_state();
}

static void hdmi_edid_delay_work(struct work_struct *work)
{
	struct display_device *pdev = container_of(work, struct display_device, edid_dwork.work);
	int ret;

	if (api_GetState() != API_EDID_READ || api_GetEdidState() != EDID_IDLE) {
		dlog_err("EDID read failed, api_mCurrentState=%d, edid_mStatus=%d\n",
			api_GetState(), api_GetEdidState());
	}
	
	if (api_GetState() == API_HPD) {
		if (api_GetEdidState() == EDID_READING)
			dlog_err("EDID read timeout\n");
		else
			dlog_err("EDID read error\n");
	}

	if (pdev->enable) {
		dlog_dbg("Configure\n");
		__hdmi_config_video(pdev);
		ret = api_Configure(&video_param, &audio_param, 
					&product_param, (hdmi_hdcp? &hdcp_param : NULL));
		if (ret == FALSE) {
			dlog_info("api_Configure failed %d\n", ret);
			return;
		}
	}
	pdev->hpd = __hdmi_hpd_state();
	__hdmi_hpd_event(pdev, pdev->hpd);

	return;
}

static void hdmi_hpd_delay_work(struct work_struct *work)
{
	struct display_device *pdev = container_of(work, struct display_device, hpd_dwork.work);
	int hpd;

	if (__hdmi_hpd_changed()) {
		hpd = __hdmi_hpd_state();
		if (hpd) {
			dlog_info("Plug in +++\n");
			if (gpio_irq) {
				__hdmi_clk(1);
				api_Initialize(0, 1, 2500, 0);
			}
			api_EventHandler(gpio_irq);
			schedule_delayed_work(&pdev->edid_dwork, DISPLAY_HDMI_EDID_DELAY);
			api_EdidRead();
		}
		else {
			cancel_delayed_work(&pdev->edid_dwork);
			dlog_info("Draw out ---\n");
			api_EventHandler(gpio_irq);
			api_SetEdidState(EDID_IDLE);
			api_Initialize(0, 1, 2500, 0);
			if (gpio_irq)
				__hdmi_clk(0);
			pdev->hpd = hpd;
			__hdmi_hpd_event(pdev, hpd);
		}
		__hdmi_hpd_update();
	}
	else {
		api_EventHandler(gpio_irq);
		if (api_GetState() == API_EDID_READ && api_GetEdidState() == EDID_IDLE) {
			dlog_dbg("EDID read done.\n");
			schedule_delayed_work(&pdev->edid_dwork, 0);
		}
	}

	return;
}

static irqreturn_t hdmi_hpd_handler(int irq, void *data)
{
	struct display_device *pdev = data;
	unsigned long delay = 0;

	if (__hdmi_hpd_changed()) {
		delay = DISPLAY_HDMI_HPD_DELAY;
		dlog_dbg("schedule delayed work\n");
	}
	schedule_delayed_work(&pdev->hpd_dwork, delay);
	
	return IRQ_HANDLED;
}

static int hdmi_tx_enable(struct display_device *pdev, int en)
{
	int ret, hpd;

	if (hdmi_not_available)
		return 0;

	hpd = __hdmi_hpd_state();
	if (!hpd)
		goto out;
	
	if (!pdev->enable && en) {
		__hdmi_config_video(pdev);
		ret = api_Configure(&video_param, &audio_param, 
					&product_param, (hdmi_hdcp? &hdcp_param : NULL));
		if (ret == FALSE) {
			dlog_info("api_Configure failed\n");	// enable should not failed when has logo
		}
	}
	else if (pdev->enable && !en) {
		api_Initialize(0, 1, 2500, 0);
		/* fix state */
		if (api_GetEdidState() == EDID_IDLE)
			api_SetState(API_EDID_READ);
		else
			api_SetState(API_HPD);
	}
	else
		dlog_dbg("skip duplicated hdmi enable operation\n");

out:
	pdev->enable = en;

	return 0;
}

static int hdmi_tx_set_vic(struct display_device *pdev, int vic)
{
	dlog_dbg("set vic %d\n", vic);

	if (hdmi_not_available)
		return 0;

	pdev->vic = vic;
	
	imapx_pad_init("hdmi");

	return 0;
}

static int hdmi_tx_probe(struct display_device *pdev)
{
	int plug = 0, hdmi_irq, i, ret;
	
	dlog_trace();

	if (hdmi_not_available) {
		iowrite32(0x59, IO_ADDRESS(SYSMGR_IDS1_BASE + 0x04));
		return 0;
	}

	iowrite32(0xFF, IO_ADDRESS(SYSMGR_IDS1_BASE + 0x04));
	iowrite32(0x01, IO_ADDRESS(SYSMGR_IDS1_BASE + 0x1C));
	access_Initialize((unsigned char *)IMAP_HDMI_REG_BASE);

	if (gpio_irq) {
		dlog_dbg("hdmi irq gpio %d\n", gpio_irq);
		ret = gpio_request(gpio_irq, "hdmi_hpd");
		if (ret) {
			dlog_err("request gpio %d failed\n", gpio_irq);
			return ret;
		}
		gpio_direction_input(gpio_irq);
		/* ensure gpio mode */
		imapx_pad_set_mode(gpio_irq, "input");
		imapx_pad_set_pull(gpio_irq, "float");
		plug = gpio_get_value(gpio_irq);
	}

	if (!gpio_irq || (gpio_irq && plug)) {
		__hdmi_clk(1);
		api_Initialize(0, 1, 2500, 0);
	}

	__hdmi_default_config();

	INIT_DELAYED_WORK(&pdev->hpd_dwork, hdmi_hpd_delay_work);
	INIT_DELAYED_WORK(&pdev->edid_dwork, hdmi_edid_delay_work);

	if (gpio_irq) {
		hdmi_irq = gpio_to_irq(gpio_irq);
	}
	else {
		hdmi_irq = GIC_HDMITX_ID;
	}
	pdev->irq = hdmi_irq;
	ret = request_threaded_irq(hdmi_irq, NULL, hdmi_hpd_handler,
			IRQF_ONESHOT | IRQF_TRIGGER_RISING,
			"hdmi_hpd", pdev);
	if (ret) {
		dlog_err("request irq failed %d\n", ret);
		return ret;
	}
	disable_irq(hdmi_irq);

	for (i = 0; hdmi_tx_attrs[i].attr.name != NULL; i++)
			sysfs_create_file(&pdev->dev.kobj, &hdmi_tx_attrs[i].attr);

	enable_irq(hdmi_irq);

	if (__hdmi_hpd_changed())
		schedule_delayed_work(&pdev->hpd_dwork, DISPLAY_HDMI_BOOT_DELAY);

	return 0;
}

static int hdmi_tx_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver hdmi_tx_driver = {
	.probe  = hdmi_tx_probe,
	.remove = hdmi_tx_remove,
	.driver = {
		.name = "hdmi_tx",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_HDMI,
	.function = {
		.enable = hdmi_tx_enable,
		.set_vic = hdmi_tx_set_vic,
	}
};

static int __init hdmi_tx_driver_init(void)
{
	int ret;
	int i, j;
	char item[64] = {0};
	char str[64] = {0};
	
	ret = display_driver_register(&hdmi_tx_driver);
	if (ret) {
		dlog_err("register hdmi tx driver failed %d\n", ret);
		return ret;
	}

	for (i = 0; i < IDS_PATH_NUM; i++) {
		for (j = 0; j < MAX_PATH_PERIPHERAL_NUM; j++) { 
			sprintf(item, DISPLAY_ITEM_PERIPHERAL_TYPE, i, j);
			if (!item_exist(item))
				break;
			item_string(str, item, 0);
			if (sysfs_streq(str, "hdmi")) {
				sprintf(item, DISPLAY_ITEM_PERIPHERAL_IRQ, i, str);
				if (item_exist(item)) {
					gpio_irq = item_integer(item, 0);
					if (!gpio_is_valid(gpio_irq)) {
						dlog_err("gpio %d of item %s is not valid\n", gpio_irq, item);
						return -EINVAL;
					}
				}
				sprintf(item, DISPLAY_ITEM_PERIPHERAL_HDMI_HDCP, i, str);
				if (item_exist(item))
					hdmi_hdcp = item_integer(item, 0);
				sprintf(item, DISPLAY_ITEM_PERIPHERAL_HDMI_DVI, i, str);
				if (item_exist(item))
					hdmi_dvi = item_integer(item, 0);
				goto out;
			} 
		}
	}
	dlog_info("no hdmi device\n");
	hdmi_not_available = 1;

out:
	return 0;
}
module_init(hdmi_tx_driver_init);

static void __exit hdmi_tx_driver_exit(void)
{
	display_driver_unregister(&hdmi_tx_driver);
}
module_exit(hdmi_tx_driver_exit);

MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display hdmi transmit controller driver");
MODULE_LICENSE("GPL");
