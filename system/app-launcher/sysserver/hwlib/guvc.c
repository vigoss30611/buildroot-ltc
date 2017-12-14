#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

#include "spv_common.h"
#include "sys_server.h"
#include "spv_config_keys.h"
#include "spv_utils.h"
#include "camera_spv.h"
#include <qsdk/videobox.h>
#include <guvc.h>

#include <time.h>

#define CONTROL_ENTRY "isp"

static SPVSTATE* g_spv_camera_state = NULL;
pthread_t uvc_h264_t;
struct uvc_device *udev;
char dev_name[20];
static int g_video_is_reseting = 0;
extern struct v_bitrate_info LOCAL_MEDIA_BITRATE_INFO;

#define MIN_BITRATE 		6*1024*1024
#define MAX_BITRATE 		14*1024*1024

#define INFOTM_PACKET_START		0
#define INFOTM_PACKET_PENDING	1
#define INFOTM_PACKET_END			2

union infotm_packet_data {
	uint8_t d[52];
	struct {
		uint16_t pid;	/* 0 tot 65536  */
		uint8_t flag;	/* flag = 0 : start flag = 1: data, flag =2 : end */
		uint8_t len;	/*max 48 bytes = 48*/
		uint8_t data[48];
	} packet;
};

struct infotm_packet_status {
	uint16_t status;
	uint16_t pid;
	uint8_t flag;
};

struct infotm_data_receiver{
	uint16_t cur_pid;
	uint16_t status;
	uint8_t cur_flag;
};

union uvc_infotm_extension_unit_control_u {
	uint32_t wSnapParam;						//UVC_INFOTM_EU_CONTROL_SNAP
	uint32_t wVideoRecording;				//UVC_INFOTM_EU_CONTROL_VIDEO_RECORDING
};

struct uvc_infotm_extension_unit_control {
	uint32_t wSnapParam;						//UVC_INFOTM_EU_CONTROL_SNAP
	uint32_t wVideoRecording;				//UVC_INFOTM_EU_CONTROL_VIDEO_RECORDING
};

struct uvc_control_request_param {
	struct uvc_camera_terminal_control_obj ct_ctrl_def;
	struct uvc_camera_terminal_control_obj ct_ctrl_max;
	struct uvc_camera_terminal_control_obj ct_ctrl_min;
	struct uvc_camera_terminal_control_obj ct_ctrl_cur;
	struct uvc_camera_terminal_control_obj ct_ctrl_res;

	struct uvc_processing_unit_control_obj pu_ctrl_def;
	struct uvc_processing_unit_control_obj pu_ctrl_max;
	struct uvc_processing_unit_control_obj pu_ctrl_min;
	struct uvc_processing_unit_control_obj pu_ctrl_cur;
	struct uvc_processing_unit_control_obj pu_ctrl_res;

	uvcx_video_config_probe_commit_t eu_h264_ctrl_def;
	uvcx_video_config_probe_commit_t eu_h264_ctrl_max;
	uvcx_video_config_probe_commit_t eu_h264_ctrl_min;
	uvcx_video_config_probe_commit_t eu_h264_ctrl_cur;
	uvcx_video_config_probe_commit_t eu_h264_ctrl_res;
};

static struct uvc_control_request_param g_uvc_param;
static infotm_data_t g_infotm_ctrl_state;

static struct infotm_data_receiver g_receiver = {0};
char g_files[256] = {0};

/* ---------------------------------------------------------------------------
 * main
 */
static void image_load(struct uvc_device *dev, const char *img)
{
	void *imgdata = NULL;
	uint32_t imgsize = 0;
	int fd = -1;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s =>\n", __func__);
	if (img == NULL) {
		UVC_PRINTF(UVC_LOG_ERR,"MJPEG image is NULL\n");
		exit(-1);
	}

	fd = open(img, O_RDONLY);
	if (fd == -1) {
		UVC_PRINTF(UVC_LOG_ERR,"Unable to open MJPEG image '%s'\n", img);
		exit(-1);
	}

	imgsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	imgdata = malloc(imgsize);
	if (imgdata == NULL) {
		UVC_PRINTF(UVC_LOG_ERR,"Unable to allocate memory for MJPEG image\n");
		imgsize = 0;
		exit(-1);
	}

	read(fd, imgdata, imgsize);

	uvc_set_def_img(dev,
		imgdata, imgsize,UVC_DEF_IMG_FORM_PIC);

	close(fd);
	UVC_PRINTF(UVC_LOG_TRACE,"# %s  image len:%d => \n", __func__, imgsize);
}

int _is_vaild_resolution(struct uvc_device *dev, char *channel)
{
	int vw, vh;
	int uw, uh;
	int fps;

	video_get_resolution(channel, &vw, &vh);
	uvc_get_cur_resolution(&uw, &uh, &fps);

	UVC_PRINTF(UVC_LOG_TRACE, "# uvc v(%d x %d) "
		"u(%d x %d)\n",
		vw, vh, uw, uh);
	if (vw != uw || vh != uh)
		return 0;

	return 1;
}

static void print_uvcx_probe_commit_data(uvcx_video_config_probe_commit_t *data)
{
	if(data == NULL) {
		UVC_PRINTF(UVC_LOG_ERR, "#uvcx_video_config_probe_commit pointer is null\n");
		return;
	}
	UVC_PRINTF(UVC_LOG_INFO, "uvcx_video_config_probe_commit:\n");
	UVC_PRINTF(UVC_LOG_INFO, "\tFrameInterval: %d\n", data->dwFrameInterval);
	UVC_PRINTF(UVC_LOG_INFO, "\tBitRate: %d\n", data->dwBitRate);
	UVC_PRINTF(UVC_LOG_INFO, "\tHints: 0x%X\n", data->bmHints);
	UVC_PRINTF(UVC_LOG_INFO, "\tConfigurationIndex: %d\n", data->wConfigurationIndex);
	UVC_PRINTF(UVC_LOG_INFO, "\tWidth: %d\n", data->wWidth);
	UVC_PRINTF(UVC_LOG_INFO, "\tHeight: %d\n", data->wHeight);
	UVC_PRINTF(UVC_LOG_INFO, "\tSliceUnits: %d\n", data->wSliceUnits);
	UVC_PRINTF(UVC_LOG_INFO, "\tSliceMode: %d\n", data->wSliceMode);
	UVC_PRINTF(UVC_LOG_INFO, "\tProfile: %d\n", data->wProfile);
	UVC_PRINTF(UVC_LOG_INFO, "\tIFramePeriod: %d\n", data->wIFramePeriod);
	UVC_PRINTF(UVC_LOG_INFO, "\tEstimatedVideoDelay: %d\n",data->wEstimatedVideoDelay);
	UVC_PRINTF(UVC_LOG_INFO, "\tEstimatedMaxConfigDelay: %d\n",data->wEstimatedMaxConfigDelay);
	UVC_PRINTF(UVC_LOG_INFO, "\tUsageType: %d\n",data->bUsageType);
	UVC_PRINTF(UVC_LOG_INFO, "\tRateControlMode: %d\n",data->bRateControlMode);
	UVC_PRINTF(UVC_LOG_INFO, "\tTemporalScaleMode: %d\n",data->bTemporalScaleMode);
	UVC_PRINTF(UVC_LOG_INFO, "\tSpatialScaleMode: %d\n",data->bSpatialScaleMode);
	UVC_PRINTF(UVC_LOG_INFO, "\tSNRScaleMode: %d\n",data->bSNRScaleMode);
	UVC_PRINTF(UVC_LOG_INFO, "\tStreamMuxOption: %d\n",data->bStreamMuxOption);
	UVC_PRINTF(UVC_LOG_INFO, "\tStreamFormat: %d\n",data->bStreamFormat);
	UVC_PRINTF(UVC_LOG_INFO, "\tEntropyCABAC: %d\n",data->bEntropyCABAC);
	UVC_PRINTF(UVC_LOG_INFO, "\tTimestamp: %d\n",data->bTimestamp);
	UVC_PRINTF(UVC_LOG_INFO, "\tNumOfReorderFrames: %d\n",data->bNumOfReorderFrames);
	UVC_PRINTF(UVC_LOG_INFO, "\tPreviewFlipped: %d\n",data->bPreviewFlipped);
	UVC_PRINTF(UVC_LOG_INFO, "\tView: %d\n",data->bView);
	UVC_PRINTF(UVC_LOG_INFO, "\tReserved1: %d\n",data->bReserved1);
	UVC_PRINTF(UVC_LOG_INFO, "\tReserved2: %d\n",data->bReserved2);
	UVC_PRINTF(UVC_LOG_INFO, "\tStreamID: %d\n",data->bStreamID);
	UVC_PRINTF(UVC_LOG_INFO, "\tSpatialLayerRatio: %d\n",data->bSpatialLayerRatio);
	UVC_PRINTF(UVC_LOG_INFO, "\tLeakyBucketSize: %d\n",data->wLeakyBucketSize);
}
/*----------------------------------------------------------------------------*/
/* control request register */
void  uvc_ct_ae_mode_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret;
	uint8_t ae_mode, mode;
	union uvc_camera_terminal_control_u* ctrl;

	ctrl = (union uvc_camera_terminal_control_u*)&d->data;
	if (d->length != sizeof ctrl->AEMode.d8) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	ae_mode = ctrl->AEMode.d8;
	UVC_PRINTF(UVC_LOG_ERR, "### %s [CT][AE-Mode]: %d\n", __func__,
		ae_mode);

	if (ae_mode == UVC_AUTO_EXPOSURE_MODE_AUTO ||
		ae_mode == UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY)
		mode = 1;
	else 
		mode = 0;

	ret = camera_ae_enable(CONTROL_ENTRY, mode);
	if (ret <0)
		UVC_PRINTF(UVC_LOG_ERR, "#camera_ae_enable failed! ae mode: %d\n", mode);
	else {
		UVC_PRINTF(UVC_LOG_ERR, "#camera_ae_enable successful! ae mode: %d\n", mode);
		g_uvc_param.ct_ctrl_cur.AEMode.d8 = ae_mode;
	}
}

void uvc_ct_exposure_abs_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int exposure = 0;
	int ret = 0;
	union uvc_camera_terminal_control_u* ctrl;

	ctrl = (union uvc_camera_terminal_control_u*)&d->data;
	if (d->length != sizeof ctrl->dwExposureTimeAbsolute) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	exposure = ctrl->dwExposureTimeAbsolute - 5;
	if(exposure >= -4 && exposure <= 4) {
		int ret = camera_set_exposure(CONTROL_ENTRY, exposure);
		if (ret < 0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_exposure failed! exposure mode: %d\n", exposure);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_exposure successful! exposure mode: %d\n", exposure);
			g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute = ctrl->dwExposureTimeAbsolute;
		}
	} else
		UVC_PRINTF(UVC_LOG_TRACE, "exposure is out of range: %d\n", exposure);
}

void uvc_pu_brightness_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int brightness;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wBrightness) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	brightness = ctrl->wBrightness;
	UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wBrightness]: %d\n",__func__,
		brightness);

	if (brightness >= 0 && brightness < 256) {
		ret = camera_set_brightness(CONTROL_ENTRY, brightness);
		if (ret <0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_brightness failed! "
				"brightness: %d\n", brightness);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_brightness successful! "
					"brightness: %d\n", brightness);
			g_uvc_param.pu_ctrl_cur.wBrightness = brightness;
		}
	} else {
		UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wBrightness]: %d\n",__func__,
			brightness);
	}
}

void uvc_pu_contrast_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int contrast;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wContrast) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	contrast = ctrl->wContrast;
	UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wContrast]: %d\n",__func__,
		contrast);

	if (contrast >= 0 && contrast < 256) {
		ret = camera_set_contrast(CONTROL_ENTRY, contrast);
		if (ret <0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_contrast failed! "
				"contrast: %d\n", contrast);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_contrast successful! "
				"contrast: %d\n", contrast);
			g_uvc_param.pu_ctrl_cur.wContrast= contrast;
		}
	} else {
		UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wContrast]: %d\n",__func__,
			contrast);
	}
}

void uvc_pu_gain_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int gain;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wGain) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	gain = ctrl->wGain;
	UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wGain]: %d\n",__func__,
		gain);

	if (gain >= 0 && gain < 256) {
		ret = camera_set_ISOLimit(CONTROL_ENTRY, gain);
		if (ret <0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_ISOLimit failed! "
				"gain: %d\n", gain);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_ISOLimit successful! "
				"gain: %d\n", gain);
			g_uvc_param.pu_ctrl_cur.wGain = gain;
		}
	} else {
		UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wGain]: %d\n",__func__,
			gain);
	}
}

void uvc_pu_wb_temperature_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int wb;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wWhiteBalanceTemperature) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	wb = ctrl->wWhiteBalanceTemperature;
	UVC_PRINTF(UVC_LOG_TRACE, "wb: %d\n", wb);

	ret = camera_set_wb(CONTROL_ENTRY, wb);
	if (ret <0)
		UVC_PRINTF(UVC_LOG_ERR, "#camera_set_wb failed! "
			"wb: %d\n", wb);
	else {
		UVC_PRINTF(UVC_LOG_ERR, "#camera_set_wb successful! "
			"wb: %d\n", wb);
		g_uvc_param.pu_ctrl_cur.wWhiteBalanceTemperature = wb;
	}
}

void uvc_pu_wb_temperature_auto_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int wb;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->bWhiteBalanceTemperatureAuto) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}
	/* 1: auto , 0: manual */
	wb = ctrl->bWhiteBalanceTemperatureAuto;
	UVC_PRINTF(UVC_LOG_TRACE, "wb auto: %d\n", wb);
	if(wb) {
		/* for infotm api set 0 means auto */
		ret = camera_set_wb(CONTROL_ENTRY, 0);
		if (ret <0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_wb failed! "
					"wb: %d\n", wb);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_wb successful! "
					"wb: %d\n", wb);
			g_uvc_param.pu_ctrl_cur.bWhiteBalanceTemperatureAuto = wb;
		}
	} else
		g_uvc_param.pu_ctrl_cur.bWhiteBalanceTemperatureAuto = wb;
}

void uvc_pu_hue_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int hue;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wHue) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	hue = ctrl->wHue;
	UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wHue]: %d\n",__func__,
		hue);

	if (hue >= 0 && hue < 256) {
		/*camera_set_hue do not realize at current Qsdk*/
		ret = camera_set_hue(CONTROL_ENTRY, hue);
		if (ret <0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_hue failed! hue: %d\n", hue);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_hue successful! hue: %d\n", hue);
			g_uvc_param.pu_ctrl_cur.wHue= hue;
		}
	} else {
		UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wHue]: %d\n",__func__,
			hue);
	}
}

void uvc_pu_saturation_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int saturation;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wSaturation) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	saturation = ctrl->wSaturation;
	UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wSaturation]: %d\n",__func__,
		saturation);

	if (saturation > 0 && saturation < 256) {
		ret = camera_set_saturation(CONTROL_ENTRY, saturation);
		if (ret <0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_saturation failed! "
				"saturation: %d\n", saturation);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_saturation successful! "
				"saturation: %d\n", saturation);
			g_uvc_param.pu_ctrl_cur.wSaturation= saturation;
		}
	} else {
		UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wSaturation]: %d\n",__func__,
			saturation);
	}
}

void uvc_pu_sharpness_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int sharpness;
	union uvc_processing_unit_control_u* ctrl;

	ctrl = (union uvc_processing_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wSharpness) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	sharpness = ctrl->wSharpness;
	UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wSharpness]: %d\n",__func__,
		sharpness);

	if (sharpness >= 0 && sharpness < 256) {
		ret = camera_set_sharpness(CONTROL_ENTRY, sharpness);
		if (ret <0)
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_sharpness failed! "
				"sharpness: %d\n", sharpness);
		else {
			UVC_PRINTF(UVC_LOG_ERR, "#camera_set_sharpness successful! "
				"sharpness: %d\n", sharpness);
			g_uvc_param.pu_ctrl_cur.wSharpness= sharpness;
		}
	} else {
		UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wSharpness] is novalid: %d\n",__func__,
			sharpness);
	}
}

void uvc_eu_infotm_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	int cmd = 0;
	int i = 0;
	infotm_ctrl_package_t *ctrl = NULL;
	time_t local_time;
	char looptime[4] = {0};

	ctrl = (infotm_ctrl_package_t *)&d->data;
	if (d->length != sizeof *ctrl) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	cmd = ctrl->cmd_id;
	UVC_PRINTF(UVC_LOG_TRACE, "cmd is %d\n", cmd);

	switch (cmd) {
		case UVCX_INFOTM_CMD_TAKE_PIC:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_TAKE_PIC\n");
			if(g_spv_camera_state->mode == MODE_PHOTO) {
				g_spv_camera_state->isPicturing = 1;
			} else {
				UVC_PRINTF(UVC_LOG_TRACE, "Not in PHOTO mode, can't take pic\n");
			}
			break;
		case UVCX_INFOTM_CMD_START_VIDEO:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_START_VIDEO\n");
			if(g_spv_camera_state->mode == MODE_VIDEO && !g_spv_camera_state->isRecording) {
				if(SendMsgToServer(MSG_SHUTTER_ACTION, 0, 0, TYPE_FROM_UVC_HOST))
					UVC_PRINTF(UVC_LOG_TRACE, "start video failed\n");
			} else {
				UVC_PRINTF(UVC_LOG_TRACE, "Not in VIDEO mode , or already start video record\n");
				UVC_PRINTF(UVC_LOG_TRACE, "video state: %d\n", g_spv_camera_state->isRecording);
			}
			break;
		case UVCX_INFOTM_CMD_STOP_VIDEO:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_STOP_VIDEO\n");
			if(g_spv_camera_state->mode == MODE_VIDEO && g_spv_camera_state->isRecording) {
				if(SendMsgToServer(MSG_SHUTTER_ACTION, 0, 0, TYPE_FROM_UVC_HOST))
					UVC_PRINTF(UVC_LOG_TRACE, "stop video failed\n");
			} else {
				UVC_PRINTF(UVC_LOG_TRACE, "Not in VIDEO mode , or already stop video record\n");
				UVC_PRINTF(UVC_LOG_TRACE, "video state: %d\n", g_spv_camera_state->isRecording);
			}
			break;
		case UVCX_INFOTM_CMD_VIDEO_LOOP:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_VIDEO_LOOP\n");
			UVC_PRINTF(UVC_LOG_TRACE, "bVideoLoopTime get from PC: %d\n", ctrl->data.bVideoLoopTime);
			sprintf(&looptime, "%d", ctrl->data.bVideoLoopTime);
			UVC_PRINTF(UVC_LOG_TRACE, "looptime: %s\n", looptime);
			if(SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_VIDEO_LOOP_RECORDING), 
					looptime, TYPE_FROM_GUI))
				UVC_PRINTF(UVC_LOG_TRACE, "set loop time failed\n");
			UVC_PRINTF(UVC_LOG_TRACE, "####[loop time]\tctrl : %d\n", ctrl->data.bVideoLoopTime);
			break;
		case UVCX_INFOTM_CMD_SYNC_TIME:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_SYNC_TIME\n");
			UVC_PRINTF(UVC_LOG_TRACE, "get utc_seconds from PC: %ld\n", ctrl->data.utc_seconds);
			if(SetTimeByRawData(ctrl->data.utc_seconds))
				UVC_PRINTF(UVC_LOG_TRACE, "set time failed\n");
			local_time = time(NULL);
			UVC_PRINTF(UVC_LOG_TRACE, "local time: %s\n", ctime(&local_time));
			break;

		case UVCX_INFOTM_CMD_DEVICE_MODE:
			if(SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_CURRENT_MODE), g_mode_name[ctrl->data.bDeviceMode % MODE_COUNT], TYPE_FROM_GUI))
				UVC_PRINTF(UVC_LOG_TRACE, "send device mode failed! key: %s, value: %s\n", GETKEY(ID_CURRENT_MODE), g_mode_name[ctrl->data.bDeviceMode % MODE_COUNT]);
			break;

		case UVCX_INFOTM_CMD_SD_STATE:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_SD_STATE\n");
			break;

		case UVCX_INFOTM_CMD_SWITCH_TO_UMS:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_SWITCH_TO_UMS\n");
			if(SpvRunSystemCmd("umount /mnt/sd0")) {
				UVC_PRINTF(UVC_LOG_TRACE, "umount /mnt/sd0 failed\n");
			}
			if(SpvRunSystemCmd("eventsend \"umount\"")) {
				DBG("event send /mnt/sd0 failed\n");
			}
#if 0
			if(SpvRunSystemCmd("umount /mnt/sd0")) {
				DBG("umount /mnt/sd0 failed\n");
			}
#endif
			SpvPostMessage(GetLauncherHandler(), MSG_SWITCH_TO_UMS, 0, 0);
			break;
		case UVCX_INFOTM_CMD_SWITCH_TO_UVC:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_SWITCH_TO_UVC\n");
			if(SpvRunSystemCmd("mount /dev/mmcblk0p1 /mnt/sd0")) {
				DBG("mount /mnt/sd0 failed\n");
			}
			if(SpvRunSystemCmd("eventsend \"mount\" \"/dev/mmcblk0p1\"")) {
				DBG("mount /mnt/sd0 failed\n");
			}
#if 0
			if(SpvRunSystemCmd("mount /dev/mmcblk0p1 /mnt/sd0")) {
				DBG("mount /dev/mmcblk0p1  to /mnt/sd0 failed\n");
			}
#endif
			SpvPostMessage(GetLauncherHandler(), MSG_SWITCH_TO_UVC, 0, 0);
			break;

		case UVCX_INFOTM_CMD_GET_BATTERY_INFO:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_GET_BATTERY_INFO\n");
			break;
		case UVCX_INFOTM_CMD_GET_SD_INFO:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_GET_SD_INFO\n");
			break;
		case UVCX_INFOTM_CMD_GET_VERSION:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_GET_VERSION\n");
			break;
		case UVCX_INFOTM_CMD_GET_VIDEO_STATE:
			UVC_PRINTF(UVC_LOG_TRACE, "####infotm set: UVCX_INFOTM_CMD_GET_VIDEO_STATE\n");
			break;
		default:
			UVC_PRINTF(UVC_LOG_TRACE, "#unknown infotm ctrl cmd: %d\n", cmd);
			break;
	}
#if 0
	UVC_PRINTF(UVC_LOG_TRACE, "ctrl: ");
	for(i = 0; i < sizeof(*ctrl); i++)
		printf(" 0x%02x", *((char*)ctrl + i));
	printf("\n");
#endif
}

void uvc_eu_infotm_calib_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_INFO, "#infotm calibration set data\n");
}

/*
void uvc_eu_video_recording_data(struct uvc_device *dev,
			   struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	uint32_t video_recording = 0;
	union uvc_infotm_extension_unit_control_u* ctrl = NULL;

	ctrl = (union uvc_infotm_extension_unit_control_u*)&d->data;
	if (d->length != sizeof ctrl->wVideoRecording) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	video_recording = ctrl->wVideoRecording;
	UVC_PRINTF(UVC_LOG_INFO, "### %s [EU][video_recording]: %u\n",__func__,
		video_recording);

	g_uvc_param.eu_ctrl_cur.wVideoRecording= video_recording;
	//TODO , set 
}*/

static void uvc_ct_ae_mode_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  =0;
	union uvc_camera_terminal_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
	
	ctrl = (union uvc_camera_terminal_control_u*)&resp->data;
	resp->length = sizeof ctrl->AEMode.d8;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		/* Current setting attribute */
		/* 0: auto
		 * 1: manual
		 * 2: shutter_priority
		 * 3: aperture_priority
		 * */
		ctrl->AEMode.d8 = g_uvc_param.ct_ctrl_cur.AEMode.d8;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->AEMode.d8);
		/*TODO : get current value form isp or camera*/
		break;

	case UVC_GET_MIN:
		/* Minimum setting attribute */
		ctrl->AEMode.d8 = g_uvc_param.ct_ctrl_min.AEMode.d8;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->AEMode.d8);
		break;

	case UVC_GET_MAX:
		/* Maximum setting attribute */
		ctrl->AEMode.d8 = g_uvc_param.ct_ctrl_max.AEMode.d8;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->AEMode.d8);
		break;

	case UVC_GET_DEF:
		/* Default setting attribute */
		ctrl->AEMode.d8 = g_uvc_param.ct_ctrl_def.AEMode.d8;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->AEMode.d8);
		break;

	case UVC_GET_RES:
		/* Resolution attribute */
		ctrl->AEMode.d8 = g_uvc_param.ct_ctrl_res.AEMode.d8;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->AEMode.d8);
		break;

	case UVC_GET_INFO:
		/* Information attribute */
		resp->data[0] = 0xff;
		resp->length = 1;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_ct_exposure_abs_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int exposure = 0;
	union uvc_camera_terminal_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_camera_terminal_control_u*)&resp->data;
	resp->length =sizeof ctrl->dwExposureTimeAbsolute;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		/*TODO : get current value form isp or camera*/
		if(camera_get_exposure(CONTROL_ENTRY, &exposure)) {
			UVC_PRINTF(UVC_LOG_TRACE, "camera_get_exposure failed\n");
			ctrl->dwExposureTimeAbsolute =
				g_uvc_param.ct_ctrl_def.dwExposureTimeAbsolute;
				break;
		} else {
			g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute = exposure;
			ctrl->dwExposureTimeAbsolute =
				g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute;
		}
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->dwExposureTimeAbsolute);
		break;

	case UVC_GET_MIN:
		ctrl->dwExposureTimeAbsolute =
			g_uvc_param.ct_ctrl_min.dwExposureTimeAbsolute;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->dwExposureTimeAbsolute);
		break;

	case UVC_GET_MAX:
		ctrl->dwExposureTimeAbsolute =
			g_uvc_param.ct_ctrl_max.dwExposureTimeAbsolute;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->dwExposureTimeAbsolute);
		break;

	case UVC_GET_DEF:
		ctrl->dwExposureTimeAbsolute =
			g_uvc_param.ct_ctrl_def.dwExposureTimeAbsolute;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->dwExposureTimeAbsolute);
		break;

	case UVC_GET_RES:
		ctrl->dwExposureTimeAbsolute =
			g_uvc_param.ct_ctrl_res.dwExposureTimeAbsolute;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->dwExposureTimeAbsolute);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_brightness_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret = 0;
	int brightness = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wBrightness;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret = camera_get_brightness(CONTROL_ENTRY, brightness);
		if (ret < 0)
			UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_brightness failed \n",
				__func__);

		if (brightness >=  0 &&  brightness < 255) {
			g_uvc_param.pu_ctrl_cur.wBrightness = brightness;
		}
		ctrl->wBrightness =
			g_uvc_param.pu_ctrl_cur.wBrightness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wBrightness);
		break;

	case UVC_GET_MIN:
		ctrl->wBrightness =
			g_uvc_param.pu_ctrl_min.wBrightness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wBrightness);
		break;

	case UVC_GET_MAX:
		ctrl->wBrightness =
			g_uvc_param.pu_ctrl_max.wBrightness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wBrightness);
		break;

	case UVC_GET_DEF:
		ctrl->wBrightness =
			g_uvc_param.pu_ctrl_def.wBrightness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wBrightness);
		break;

	case UVC_GET_RES:
		ctrl->wBrightness =
			g_uvc_param.pu_ctrl_res.wBrightness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wBrightness);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_contrast_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret = 0;
	int contrast = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wContrast;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret  = camera_get_contrast(CONTROL_ENTRY, contrast);
		if (ret < 0)
			UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_contrast failed \n",
				__func__);

		if (contrast >= 0 && contrast < 255) {
			g_uvc_param.pu_ctrl_cur.wContrast = contrast;
		}
		ctrl->wContrast =
			g_uvc_param.pu_ctrl_cur.wContrast;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wContrast);
		break;

	case UVC_GET_MIN:
		ctrl->wContrast =
			g_uvc_param.pu_ctrl_min.wContrast;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wContrast);
		break;

	case UVC_GET_MAX:
		ctrl->wContrast =
			g_uvc_param.pu_ctrl_max.wContrast;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wContrast);
		break;

	case UVC_GET_DEF:
		ctrl->wContrast =
			g_uvc_param.pu_ctrl_def.wContrast;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wContrast);
		break;

	case UVC_GET_RES:
		ctrl->wContrast =
			g_uvc_param.pu_ctrl_res.wContrast;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wContrast);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_gain_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret = 0;
	int gain = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wGain;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret  = camera_get_ISOLimit(CONTROL_ENTRY, &gain);
		if (ret < 0)
			UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_gain failed \n",
				__func__);

		if (gain >= 0 && gain < 255) {
			g_uvc_param.pu_ctrl_cur.wGain = gain;
		}
		ctrl->wGain =
			g_uvc_param.pu_ctrl_cur.wGain;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wGain);
		break;

	case UVC_GET_MIN:
		ctrl->wGain =
			g_uvc_param.pu_ctrl_min.wGain;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wGain);
		break;

	case UVC_GET_MAX:
		ctrl->wGain =
			g_uvc_param.pu_ctrl_max.wGain;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wGain);
		break;

	case UVC_GET_DEF:
		ctrl->wGain =
			g_uvc_param.pu_ctrl_def.wGain;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wGain);
		break;

	case UVC_GET_RES:
		ctrl->wGain =
			g_uvc_param.pu_ctrl_res.wGain;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wGain);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_wb_temperature_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret = 0;
	int wb = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wWhiteBalanceTemperature;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret  = camera_get_wb(CONTROL_ENTRY, &wb);
		if (ret < 0) {
			UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_wb failed \n",
				__func__);
		} else {
			if (wb > 0 && wb < 5) {
				g_uvc_param.pu_ctrl_cur.wWhiteBalanceTemperature = wb;
			}
		}

		ctrl->wWhiteBalanceTemperature  =
			g_uvc_param.pu_ctrl_cur.wWhiteBalanceTemperature ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wWhiteBalanceTemperature );
		break;

	case UVC_GET_MIN:
		ctrl->wWhiteBalanceTemperature  =
			g_uvc_param.pu_ctrl_min.wWhiteBalanceTemperature ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wWhiteBalanceTemperature );
		break;

	case UVC_GET_MAX:
		ctrl->wWhiteBalanceTemperature  =
			g_uvc_param.pu_ctrl_max.wWhiteBalanceTemperature ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wWhiteBalanceTemperature );
		break;

	case UVC_GET_DEF:
		ctrl->wWhiteBalanceTemperature  =
			g_uvc_param.pu_ctrl_def.wWhiteBalanceTemperature ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wWhiteBalanceTemperature );
		break;

	case UVC_GET_RES:
		ctrl->wWhiteBalanceTemperature  =
			g_uvc_param.pu_ctrl_res.wWhiteBalanceTemperature ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wWhiteBalanceTemperature );
		break;

	case UVC_GET_INFO:
		resp->data[0] = 0xff;
		resp->length = 1;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_wb_temperature_auto_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret = 0;
	int wb = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->bWhiteBalanceTemperatureAuto;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret  = camera_get_wb(CONTROL_ENTRY, &wb);
		if (ret < 0) {
			UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_wb failed \n",
				__func__);
		} else {
			if (wb == 0)
				g_uvc_param.pu_ctrl_cur.bWhiteBalanceTemperatureAuto = 1;
			else
				g_uvc_param.pu_ctrl_cur.bWhiteBalanceTemperatureAuto = 0;
		}

		ctrl->bWhiteBalanceTemperatureAuto  =
			g_uvc_param.pu_ctrl_cur.bWhiteBalanceTemperatureAuto ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->bWhiteBalanceTemperatureAuto );
		break;

	case UVC_GET_MIN:
		ctrl->bWhiteBalanceTemperatureAuto  =
			g_uvc_param.pu_ctrl_min.bWhiteBalanceTemperatureAuto ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->bWhiteBalanceTemperatureAuto );
		break;

	case UVC_GET_MAX:
		ctrl->bWhiteBalanceTemperatureAuto  =
			g_uvc_param.pu_ctrl_max.bWhiteBalanceTemperatureAuto ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->bWhiteBalanceTemperatureAuto );
		break;

	case UVC_GET_DEF:
		ctrl->bWhiteBalanceTemperatureAuto  =
			g_uvc_param.pu_ctrl_def.bWhiteBalanceTemperatureAuto ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->bWhiteBalanceTemperatureAuto );
		break;

	case UVC_GET_RES:
		ctrl->bWhiteBalanceTemperatureAuto  =
			g_uvc_param.pu_ctrl_res.bWhiteBalanceTemperatureAuto ;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->bWhiteBalanceTemperatureAuto );
		break;

	case UVC_GET_INFO:
		resp->data[0] = 0xff;
		resp->length = 1;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_hue_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wHue;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret = camera_get_hue(CONTROL_ENTRY, 
				&g_uvc_param.pu_ctrl_cur.wHue);
		if (ret < 0) {
			g_uvc_param.pu_ctrl_cur.wHue = g_uvc_param.pu_ctrl_def.wHue;
			UVC_PRINTF(UVC_LOG_INFO,"# camera_get_hue failed \n");
		}
		ctrl->wHue =
			g_uvc_param.pu_ctrl_cur.wHue;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wHue);
		break;

	case UVC_GET_MIN:
		ctrl->wHue =
			g_uvc_param.pu_ctrl_min.wHue;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wHue);
		break;

	case UVC_GET_MAX:
		ctrl->wHue =
			g_uvc_param.pu_ctrl_max.wHue;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wHue);
		break;

	case UVC_GET_DEF:
		ctrl->wHue =
			g_uvc_param.pu_ctrl_def.wHue;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wHue);
		break;

	case UVC_GET_RES:
		ctrl->wHue =
			g_uvc_param.pu_ctrl_res.wHue;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wHue);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_saturation_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret = 0;
	int saturation = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wSaturation;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret  = camera_get_saturation(CONTROL_ENTRY, saturation);
		if (ret < 0)
			UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_saturation failed \n",
				__func__);

		if (saturation >= 0 && saturation < 255) {
			g_uvc_param.pu_ctrl_cur.wSaturation = saturation;
		}
		ctrl->wSaturation =
			g_uvc_param.pu_ctrl_cur.wSaturation;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wSaturation);
		break;

	case UVC_GET_MIN:
		ctrl->wSaturation =
			g_uvc_param.pu_ctrl_min.wSaturation;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wSaturation);
		break;

	case UVC_GET_MAX:
		ctrl->wSaturation =
			g_uvc_param.pu_ctrl_max.wSaturation;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wSaturation);
		break;

	case UVC_GET_DEF:
		ctrl->wSaturation =
			g_uvc_param.pu_ctrl_def.wSaturation;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wSaturation);
		break;

	case UVC_GET_RES:
		ctrl->wSaturation =
			g_uvc_param.pu_ctrl_res.wSaturation;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wSaturation);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_pu_sharpness_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int sharpness = 0;
	int ret  = 0;
	union uvc_processing_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_processing_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wSharpness;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ret = camera_get_sharpness(CONTROL_ENTRY, sharpness);
		if (ret < 0)
			UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_sharpness failed \n",
			__func__);

		if (sharpness  >= 0 && sharpness < 255) {
			g_uvc_param.pu_ctrl_cur.wSharpness = sharpness;
		}
		ctrl->wSharpness =
					g_uvc_param.pu_ctrl_cur.wSharpness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wSharpness);
		break;

	case UVC_GET_MIN:
		ctrl->wSharpness =
			g_uvc_param.pu_ctrl_min.wSharpness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wSharpness);
		break;

	case UVC_GET_MAX:
		ctrl->wSharpness =
			g_uvc_param.pu_ctrl_max.wSharpness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wSharpness);
		break;

	case UVC_GET_DEF:
		ctrl->wSharpness =
			g_uvc_param.pu_ctrl_def.wSharpness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wSharpness);
		break;

	case UVC_GET_RES:
		ctrl->wSharpness =
			g_uvc_param.pu_ctrl_res.wSharpness;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wSharpness);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void printf_infotm_packet_data()
{
	UVC_PRINTF(UVC_LOG_TRACE, "infotm local data: \n");
	UVC_PRINTF(UVC_LOG_TRACE, "=====================\n");
	UVC_PRINTF(UVC_LOG_TRACE, "bVideoLoopTime:\t%d\n", g_infotm_ctrl_state.bVideoLoopTime);
	UVC_PRINTF(UVC_LOG_TRACE, "bVideoState:\t%d\n", g_infotm_ctrl_state.bVideoState);
	UVC_PRINTF(UVC_LOG_TRACE, "wBatteryInfo:\t%d\n", g_infotm_ctrl_state.wBatteryInfo);
	UVC_PRINTF(UVC_LOG_TRACE, "utc_seconds:\t%ld\n", g_infotm_ctrl_state.utc_seconds);
	UVC_PRINTF(UVC_LOG_TRACE, "version:\t%s\n", g_infotm_ctrl_state.version);
	UVC_PRINTF(UVC_LOG_TRACE, "storage_info:\tTotal:%d, Free:%d\n", g_infotm_ctrl_state.storage_info.total,
			g_infotm_ctrl_state.storage_info.free);
}

void load_infotm_data()
{
	/* fill data */
    char value[VALUE_SIZE] = {0};

	memset(&g_infotm_ctrl_state, 0, sizeof(g_infotm_ctrl_state));
	GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
	g_infotm_ctrl_state.bVideoLoopTime = (uint8_t)atoi(value);
	g_infotm_ctrl_state.bVideoState = (uint8_t)g_spv_camera_state->isRecording;	
	g_infotm_ctrl_state.wBatteryInfo = (uint16_t)SpvGetBattery();
	g_infotm_ctrl_state.utc_seconds = time(NULL);
	/* version size 8 */
	memset(&value, 0, VALUE_SIZE);
	if(SpvGetVersion(value))
		snprintf(&g_infotm_ctrl_state.version, sizeof(SPV_VERSION), "%s", SPV_VERSION);
	else
		snprintf(&g_infotm_ctrl_state.version, 8, "%s", value);

	memset(&value, 0, VALUE_SIZE);
    realpath(EXTERNAL_STORAGE_PATH, value);
    system_get_storage_info(value, &g_infotm_ctrl_state.storage_info);
	g_infotm_ctrl_state.bIsSDMounted = SpvGetSdcardStatus();
	g_infotm_ctrl_state.bDeviceMode = g_spv_camera_state->mode;
}

void uvc_eu_infotm_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  = 0;
	infotm_ctrl_package_t *ctrl = NULL;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (infotm_ctrl_package_t *)&resp->data;
	resp->length = sizeof *ctrl;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
		load_infotm_data();
		printf_infotm_packet_data();
		memcpy(&ctrl->data, &g_infotm_ctrl_state, sizeof(g_infotm_ctrl_state));
		break;
	case UVC_GET_INFO:
		resp->data[0] = 0xff;
		resp->length = 1;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, resp->data[0]);
		break;
	}
}

void uvc_eu_infotm_calib_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  = 0;
	FILE *fb = NULL;
	infotm_calib_data *ctrl = NULL;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (infotm_calib_data *)&resp->data;
	resp->length = sizeof *ctrl;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
		fb = fopen(CALIBRATION_FILE, "r");
		if(fb == NULL) {
			UVC_PRINTF(UVC_LOG_TRACE, "open %s failed\n", CALIBRATION_FILE);
			break;
		}
		ret = fread(ctrl, 64, 1, fb);
		if(!ret) {
			UVC_PRINTF(UVC_LOG_TRACE, "read %s failed\n", CALIBRATION_FILE);
		}
		UVC_PRINTF(UVC_LOG_TRACE, "read length: %d\n", ret);
		fclose(fb);
		break;
	case UVC_GET_INFO:
		resp->data[0] = 0xff;
		resp->length = 1;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, resp->data[0]);
		break;
	}
}
/*
void uvc_eu_video_recording_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  = 0;
	union uvc_infotm_extension_unit_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_infotm_extension_unit_control_u*)&resp->data;
	resp->length =sizeof ctrl->wVideoRecording;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		ctrl->wVideoRecording = g_uvc_param.eu_ctrl_cur.wVideoRecording;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
			__func__, ctrl->wVideoRecording);
		break;

	case UVC_GET_MIN:
		ctrl->wVideoRecording = 
			g_uvc_param.eu_ctrl_min.wVideoRecording;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
			__func__, ctrl->wVideoRecording);
		break;

	case UVC_GET_MAX:
		ctrl->wVideoRecording =
			g_uvc_param.eu_ctrl_max.wVideoRecording;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
			__func__, ctrl->wVideoRecording);
		break;

	case UVC_GET_DEF:
		ctrl->wVideoRecording =
			g_uvc_param.eu_ctrl_def.wVideoRecording;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
			__func__, ctrl->wVideoRecording);
		break;

	case UVC_GET_RES:
		ctrl->wVideoRecording =
			g_uvc_param.eu_ctrl_res.wVideoRecording;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
			__func__, ctrl->wVideoRecording);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}*/

int intotm_data_save(char* name, char * buffer, int size, int first)
{
	int ret = 0;
	FILE* fp = NULL;

	if (first)
		fp = fopen(name, "w+");
	else
		fp = fopen(name , "a");

	if (!fp ) {
		UVC_PRINTF(UVC_LOG_ERR, "# open file: %s failed !\n", name);
		return -1;
	}

	ret = fwrite(buffer, size, 1, fp);
	if (!ret) {
		UVC_PRINTF(UVC_LOG_ERR, "# write file: %s failed !\n", name);
	}

	fflush(fp);

	fclose(fp);
	return 0;
}

void uvc_eu_infotm_packet_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	int ret = 0;
	union infotm_packet_data* p = NULL;
	p = (union infotm_packet_data*)&d->data;

#if 0
	if (g_receiver.cur_pid >= p->packet.pid) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s len = %d pid = %d recv no-valid packet\n",
			__func__, p->packet.len, p->packet.pid);
		return 0;
	}
#endif

	g_receiver.cur_flag = p->packet.flag;
	g_receiver.cur_pid = p->packet.pid;
	g_receiver.status = 0;

	if (p->packet.flag == INFOTM_PACKET_START) {

		UVC_PRINTF(UVC_LOG_INFO,"# %s len = %d pid = %d start\n",
			__func__, p->packet.len, p->packet.pid);

#if 0
		/*save only file*/
		snprintf(g_files, sizeof(g_files), "infotm_data_%d.dat", time(NULL));
#else
		snprintf(g_files, sizeof(g_files), "infotm_data.dat");
#endif

		ret = intotm_data_save(g_files, p->packet.data, p->packet.len, 1);
		if (ret < 0) {
			UVC_PRINTF(UVC_LOG_ERR,"# %s len = %d pid = %d save file failed\n",
			__func__, p->packet.len, p->packet.pid);
		}

	} else if (p->packet.flag == INFOTM_PACKET_PENDING) {

		UVC_PRINTF(UVC_LOG_INFO,"# %s len = %d pid = %d pending\n",
			__func__, p->packet.len, p->packet.pid);

		ret = intotm_data_save(g_files, p->packet.data, p->packet.len, 0);
		if (ret < 0) {
			UVC_PRINTF(UVC_LOG_ERR,"# %s len = %d pid = %d  save file failed\n",
			__func__, p->packet.len, p->packet.pid);
		}

	} else if (p->packet.flag == INFOTM_PACKET_END) {

		UVC_PRINTF(UVC_LOG_INFO,"# %s len = %d pid = %d end\n",
			__func__, p->packet.len, p->packet.pid);

		g_receiver.status = 1;
		ret = intotm_data_save(g_files, p->packet.data, p->packet.len, 0);
		if (ret < 0) {
			UVC_PRINTF(UVC_LOG_ERR,"# %s len = %d pid = %d save file failed\n",
			__func__, p->packet.len, p->packet.pid);
		}

	}
}

void uvc_eu_infotm_packet_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  = 0;
	struct infotm_packet_status* s;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	s = (struct infotm_packet_status*)&resp->data;
	memset( s, 0, sizeof *s);

	switch (req) {
	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:

		/* get current transfer status and result */
		s->flag = g_receiver.cur_flag;
		s->pid = g_receiver.cur_pid;
		s->status = g_receiver.status;

		resp->length =sizeof *s;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_STATUS val = %d\n",
			__func__, s->status);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_eu_h264_probe_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_probe_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  = 0;
	uvcx_video_config_probe_commit_t *s;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	s = (uvcx_video_config_probe_commit_t *)&resp->data;
	memset( s, 0, sizeof *s);

	switch (req) {
	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
		memcpy(s, &g_uvc_param.eu_h264_ctrl_cur, sizeof *s);
		resp->length =sizeof *s;
		print_uvcx_probe_commit_data(s);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_eu_h264_commit_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_commit_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_rate_control_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_rate_control_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  = 0;
	uvcx_rate_control_mode_t *s;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	s = (uvcx_rate_control_mode_t *)&resp->data;
	memset( s, 0, sizeof *s);

	switch (req) {
	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:

		s->bRateControlMode = RATECONTROL_CBR;
		s->wLayerID = xLayerID(7, 0, 0, 0);

		resp->length =sizeof *s;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_STATUS val = %d\n",
			__func__, s->bRateControlMode);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_eu_h264_temporal_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_temporal_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_spatial_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_spatial_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_snr_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_snr_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_LTR_bufsize_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_LTR_bufsize_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_LTR_pic_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_LTR_pic_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_pic_type_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_pic_type_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	int ret  = 0;
	uvcx_picture_type_control_t *s;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	s = (uvcx_picture_type_control_t *)&resp->data;
	memset( s, 0, sizeof *s);

	switch (req) {
	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:

		/* 0x0000: I-frame */
		/* 0x0001: Generate an IDR frame*/
		/* 0x0002: Generate an IDR frame with new SPS and PPS*/
		s->wPicType = 0;
		s->wLayerID = xLayerID(7, 0, 0, 0);

		resp->length =sizeof *s;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_STATUS val = %d\n",
			__func__, s->wPicType);
		break;

	case UVC_GET_INFO:
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_eu_h264_version_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_version_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_encoder_reset_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_encoder_reset_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_framerate_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_framerate_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_video_advance_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_video_advance_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}

void uvc_eu_h264_bitrate_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
	uint32_t bitrate = 0;
	uvcx_bitrate_layers_t *ctrl = NULL;

	ctrl = (uvcx_bitrate_layers_t *)&d->data;
	if (d->length != sizeof *ctrl) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	bitrate = ctrl->dwAverageBitrate;
	bitrate = clamp(bitrate, MIN_BITRATE, MAX_BITRATE);
	UVC_PRINTF(UVC_LOG_TRACE, "set bitrate to : %d\n", bitrate);
	g_uvc_param.eu_h264_ctrl_cur.dwBitRate = bitrate;

	LOCAL_MEDIA_BITRATE_INFO.bitrate = bitrate;
	g_video_is_reseting = 1;
	stop_p2p_stream();
	start_p2p_stream();
	UVC_PRINTF(UVC_LOG_TRACE, "leave set bitrate\n");
}

void uvc_eu_h264_bitrate_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
	int ret  = 0;
	uvcx_bitrate_layers_t *ctrl = NULL;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (uvcx_bitrate_layers_t *)&resp->data;
	resp->length = sizeof *ctrl;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
		ctrl->dwAverageBitrate = g_uvc_param.eu_h264_ctrl_cur.dwBitRate;
		UVC_PRINTF(UVC_LOG_TRACE, "get bitrate : %d\n", ctrl->dwAverageBitrate);
		break;
	case UVC_GET_INFO:
		resp->data[0] = 0xff;
		resp->length = 1;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, resp->data[0]);
		break;
	}
}

void uvc_eu_h264_QP_steps_data(struct uvc_device *dev,
			struct uvc_request_data *d, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s cs: %d\n", __func__, cs);
}

void uvc_eu_h264_QP_steps_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
}


void uvc_set_user_resolution(struct uvc_device *dev, int width, int height, int fps)
{
	/*Note: get current resolution to set videobox*/
	/* now only support : 1472x736, 1920x960 */
	UVC_PRINTF(UVC_LOG_INFO,"# uvc set resolution :%dx%d@%dfps \n", width, height, fps);
	g_video_is_reseting = 1;
	stop_p2p_stream();
	switch (width) {
		case 1472:
		case 1280:
			if(SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_VIDEO_RESOLUTION),
				"736", TYPE_FROM_GUI))
				UVC_PRINTF(UVC_LOG_TRACE, "change resolution : %dx%d failed\n", width, height);
			break;
		case 1920:
			if(SendMsgToServer(MSG_CONFIG_CHANGED, GETKEY(ID_VIDEO_RESOLUTION),
				"960", TYPE_FROM_GUI))
				UVC_PRINTF(UVC_LOG_TRACE, "change resolution : %dx%d failed\n", width, height);
			break;
		default:
			UVC_PRINTF(UVC_LOG_TRACE, "default: %dx%d\n", width, height);
			break;
	}
    //video_set_fps(VPCHN, fps);
	start_p2p_stream();
}

void uvc_event_user_process(struct uvc_device *dev, uint32_t event)
{
	int w, h, fps;
	switch (event) {
	case UVC_EVENT_CONNECT:
		UVC_PRINTF(UVC_LOG_INFO,"# %s => connect \n", __func__);
		return;

	case UVC_EVENT_DISCONNECT:
		UVC_PRINTF(UVC_LOG_INFO,"# %s => disconnect %d\n", __func__,
			dev->stream_status);
		break;

	case UVC_EVENT_SETUP:
		UVC_PRINTF(UVC_LOG_INFO,"# %s => setup phase\n", __func__);
		break;

	case UVC_EVENT_DATA:
		UVC_PRINTF(UVC_LOG_INFO,"# %s => data phase\n", __func__);
		return;

	case UVC_EVENT_STREAMON:
		UVC_PRINTF(UVC_LOG_INFO,"# %s => stream on\n", __func__);
		break;

	case UVC_EVENT_STREAMOFF:
		UVC_PRINTF(UVC_LOG_INFO,"# %s => stream off\n", __func__);
		break;
	}
}

void uvc_control_request_callback_init()
{
	uvc_cs_ops_register(UVC_CTRL_CAMERAL_TERMINAL_ID,
		UVC_CT_AE_MODE_CONTROL,
		uvc_ct_ae_mode_setup,
		uvc_ct_ae_mode_data, 0);

	uvc_cs_ops_register(UVC_CTRL_CAMERAL_TERMINAL_ID,
		UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL,
		uvc_ct_exposure_abs_setup,
		uvc_ct_exposure_abs_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_BRIGHTNESS_CONTROL,
		uvc_pu_brightness_setup,
		uvc_pu_brightness_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_CONTRAST_CONTROL,
		uvc_pu_contrast_setup,
		uvc_pu_contrast_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_GAIN_CONTROL,
		uvc_pu_gain_setup,
		uvc_pu_gain_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL,
		uvc_pu_wb_temperature_setup,
		uvc_pu_wb_temperature_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL,
		uvc_pu_wb_temperature_auto_setup,
		uvc_pu_wb_temperature_auto_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_HUE_CONTROL,
		uvc_pu_hue_setup,
		uvc_pu_hue_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_SATURATION_CONTROL,
		uvc_pu_saturation_setup,
		uvc_pu_saturation_data, 0);

	uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
		UVC_PU_SHARPNESS_CONTROL,
		uvc_pu_sharpness_setup,
		uvc_pu_sharpness_data, 0);

	uvc_cs_ops_register(UVC_CTRL_INFOTM_EXTENSION_UNIT_ID,
		UVC_INFOTM_EU_CONTROL,
		uvc_eu_infotm_setup,
		uvc_eu_infotm_data, 
		sizeof(struct _infotm_ctrl_package_t));

//	uvc_cs_ops_register(UVC_CTRL_INFOTM_EXTENSION_UNIT_ID,
//		UVC_INFOTM_EU_CONTROL_VIDEO_RECORDING,
//		uvc_eu_video_recording_setup,
//		uvc_eu_video_recording_data,
//		sizeof(g_uvc_param.eu_ctrl_def.wVideoRecording));
//
	uvc_cs_ops_register(UVC_CTRL_INFOTM_EXTENSION_UNIT_ID,
		UVC_INFOTM_EU_CONTROL_DATA,
		uvc_eu_infotm_packet_setup,
		uvc_eu_infotm_packet_data,
		sizeof(union infotm_packet_data));

	uvc_cs_ops_register(UVC_CTRL_INFOTM_EXTENSION_UNIT_ID,
		UVC_INFOTM_EU_CALIBRATION,
		uvc_eu_infotm_calib_setup,
		uvc_eu_infotm_calib_data,
		sizeof(struct _infotm_calib_data_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_VIDEO_CONFIG_PROBE,
		uvc_eu_h264_probe_setup,
		uvc_eu_h264_probe_data,
		sizeof(struct _uvcx_video_config_probe_commit_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_VIDEO_CONFIG_COMMIT,
		uvc_eu_h264_commit_setup,
		uvc_eu_h264_commit_data, 0);

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_RATE_CONTROL_MODE,
		uvc_eu_h264_rate_control_setup,
		uvc_eu_h264_rate_control_data,
		sizeof(struct _uvcx_rate_control_mode_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_TEMPORAL_SCALE_MODE,
		uvc_eu_h264_temporal_setup,
		uvc_eu_h264_temporal_data,
		sizeof(struct _uvcx_temporal_scale_mode_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_SPATIAL_SCALE_MODE,
		uvc_eu_h264_spatial_setup,
		uvc_eu_h264_spatial_data,
		sizeof(struct _uvcx_spatial_scale_mode_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_SNR_SCALE_MODE,
		uvc_eu_h264_snr_setup,
		uvc_eu_h264_snr_data,
		sizeof(struct _uvcx_snr_scale_mode_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_LTR_BUFFER_SIZE_CONTROL,
		uvc_eu_h264_LTR_bufsize_setup,
		uvc_eu_h264_LTR_bufsize_data,
		sizeof(struct _uvcx_ltr_buffer_size_control_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_LTR_PICTURE_CONTROL,
		uvc_eu_h264_LTR_pic_setup,
		uvc_eu_h264_LTR_pic_data,
		sizeof(struct _uvcx_ltr_picture_control_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_PICTURE_TYPE_CONTROL,
		uvc_eu_h264_pic_type_setup,
		uvc_eu_h264_pic_type_data,
		sizeof(struct _uvcx_picture_type_control_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_VERSION,
		uvc_eu_h264_version_setup,
		uvc_eu_h264_version_data,
		sizeof(struct _uvcx_version_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_ENCODER_RESET,
		uvc_eu_h264_encoder_reset_setup,
		uvc_eu_h264_encoder_reset_data,
		sizeof(struct _uvcx_encoder_reset_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_FRAMERATE_CONFIG,
		uvc_eu_h264_framerate_setup,
		uvc_eu_h264_framerate_data,
		sizeof(struct _uvcx_framerate_config_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_VIDEO_ADVANCE_CONFIG,
		uvc_eu_h264_video_advance_setup,
		uvc_eu_h264_video_advance_data,
		sizeof(struct _uvcx_video_advance_config_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_BITRATE_LAYERS,
		uvc_eu_h264_bitrate_setup,
		uvc_eu_h264_bitrate_data,
		sizeof(struct _uvcx_bitrate_layers_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_QP_STEPS_LAYERS,
		uvc_eu_h264_QP_steps_setup,
		uvc_eu_h264_QP_steps_data,
		sizeof(struct _uvcx_qp_steps_layers_t));
}

/*----------------------------------------------------------------------------*/

void uvcx_control_request_param_init()
{
	/* dwFrameInterval */
	/* this shall not be lower than the UVC_PROBE/COMMIT dwFrameInterval */
	g_uvc_param.eu_h264_ctrl_def.dwFrameInterval = 333333;
	g_uvc_param.eu_h264_ctrl_cur.dwFrameInterval = 333333;
	g_uvc_param.eu_h264_ctrl_res.dwFrameInterval = 333333;
	g_uvc_param.eu_h264_ctrl_min.dwFrameInterval = 166666;
	g_uvc_param.eu_h264_ctrl_max.dwFrameInterval = 5000000;

	g_uvc_param.eu_h264_ctrl_def.dwBitRate = 10*1024*1024;
	g_uvc_param.eu_h264_ctrl_cur.dwBitRate = 10*1024*1024;
	g_uvc_param.eu_h264_ctrl_res.dwBitRate = 0;
	g_uvc_param.eu_h264_ctrl_min.dwBitRate = MIN_BITRATE;
	g_uvc_param.eu_h264_ctrl_max.dwBitRate = MAX_BITRATE;

	g_uvc_param.eu_h264_ctrl_def.bmHints = 0;
	g_uvc_param.eu_h264_ctrl_cur.bmHints = 0;
	g_uvc_param.eu_h264_ctrl_res.bmHints = 0;
	g_uvc_param.eu_h264_ctrl_min.bmHints = 0;
	g_uvc_param.eu_h264_ctrl_max.bmHints = 0;

	g_uvc_param.eu_h264_ctrl_def.wConfigurationIndex = 1;
	g_uvc_param.eu_h264_ctrl_cur.wConfigurationIndex = 1;
	g_uvc_param.eu_h264_ctrl_res.wConfigurationIndex = 0;
	g_uvc_param.eu_h264_ctrl_min.wConfigurationIndex = 0;
	g_uvc_param.eu_h264_ctrl_max.wConfigurationIndex = 0;

	g_uvc_param.eu_h264_ctrl_def.wWidth = 1920;
	g_uvc_param.eu_h264_ctrl_cur.wWidth = 1920;
	g_uvc_param.eu_h264_ctrl_res.wWidth = 0;
	g_uvc_param.eu_h264_ctrl_min.wWidth = 1472;
	g_uvc_param.eu_h264_ctrl_max.wWidth = 1920;

	g_uvc_param.eu_h264_ctrl_def.wHeight = 960;
	g_uvc_param.eu_h264_ctrl_cur.wHeight = 960;
	g_uvc_param.eu_h264_ctrl_res.wHeight = 0;
	g_uvc_param.eu_h264_ctrl_min.wHeight = 736;
	g_uvc_param.eu_h264_ctrl_max.wHeight = 960;

	g_uvc_param.eu_h264_ctrl_def.wSliceUnits = 0;
	g_uvc_param.eu_h264_ctrl_cur.wSliceUnits = 0;
	g_uvc_param.eu_h264_ctrl_res.wSliceUnits = 0;
	g_uvc_param.eu_h264_ctrl_min.wSliceUnits = 0;
	g_uvc_param.eu_h264_ctrl_max.wSliceUnits = 0;

	g_uvc_param.eu_h264_ctrl_def.wSliceMode = 0;
	g_uvc_param.eu_h264_ctrl_cur.wSliceMode = 0;
	g_uvc_param.eu_h264_ctrl_res.wSliceMode = 0;
	g_uvc_param.eu_h264_ctrl_min.wSliceMode = 0;
	g_uvc_param.eu_h264_ctrl_max.wSliceMode = 0;

	/* def baseline profile without constrained flags */
	g_uvc_param.eu_h264_ctrl_def.wProfile = 0x4200;
	g_uvc_param.eu_h264_ctrl_cur.wProfile = 0x4200;
	g_uvc_param.eu_h264_ctrl_res.wProfile = 0x4200;
	g_uvc_param.eu_h264_ctrl_min.wProfile = 0x4200;
	g_uvc_param.eu_h264_ctrl_max.wProfile = 0x4200;

	g_uvc_param.eu_h264_ctrl_def.wIFramePeriod = 0;
	g_uvc_param.eu_h264_ctrl_cur.wIFramePeriod = 0;
	g_uvc_param.eu_h264_ctrl_res.wIFramePeriod = 0;
	g_uvc_param.eu_h264_ctrl_min.wIFramePeriod = 0;
	g_uvc_param.eu_h264_ctrl_max.wIFramePeriod = 0;

	/* FIXME */
	/* the value is estimated for tmep */
	g_uvc_param.eu_h264_ctrl_def.wEstimatedVideoDelay = 40;
	g_uvc_param.eu_h264_ctrl_cur.wEstimatedVideoDelay = 40;
	g_uvc_param.eu_h264_ctrl_res.wEstimatedVideoDelay = 40;
	g_uvc_param.eu_h264_ctrl_min.wEstimatedVideoDelay = 40;
	g_uvc_param.eu_h264_ctrl_max.wEstimatedVideoDelay = 40;

	/* FIXME */
	/* the value is estimated for tmep */
	g_uvc_param.eu_h264_ctrl_def.wEstimatedMaxConfigDelay = 120;
	g_uvc_param.eu_h264_ctrl_cur.wEstimatedMaxConfigDelay = 120;
	g_uvc_param.eu_h264_ctrl_res.wEstimatedMaxConfigDelay = 120;
	g_uvc_param.eu_h264_ctrl_min.wEstimatedMaxConfigDelay = 120;
	g_uvc_param.eu_h264_ctrl_max.wEstimatedMaxConfigDelay = 120;

	g_uvc_param.eu_h264_ctrl_def.bUsageType = 0x01;
	g_uvc_param.eu_h264_ctrl_cur.bUsageType = 0x01;
	g_uvc_param.eu_h264_ctrl_res.bUsageType = 0x01;
	g_uvc_param.eu_h264_ctrl_min.bUsageType = 0x01;
	g_uvc_param.eu_h264_ctrl_max.bUsageType = 0x01;

	g_uvc_param.eu_h264_ctrl_def.bRateControlMode = RATECONTROL_CBR;
	g_uvc_param.eu_h264_ctrl_cur.bRateControlMode = RATECONTROL_CBR;
	g_uvc_param.eu_h264_ctrl_res.bRateControlMode = RATECONTROL_CBR;
	g_uvc_param.eu_h264_ctrl_min.bRateControlMode = RATECONTROL_CBR;
	g_uvc_param.eu_h264_ctrl_max.bRateControlMode = RATECONTROL_CBR;

	g_uvc_param.eu_h264_ctrl_def.bTemporalScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_cur.bTemporalScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_res.bTemporalScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_min.bTemporalScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_max.bTemporalScaleMode = 0;

	g_uvc_param.eu_h264_ctrl_def.bSpatialScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_cur.bSpatialScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_res.bSpatialScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_min.bSpatialScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_max.bSpatialScaleMode = 0;

	g_uvc_param.eu_h264_ctrl_def.bSNRScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_cur.bSNRScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_res.bSNRScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_min.bSNRScaleMode = 0;
	g_uvc_param.eu_h264_ctrl_max.bSNRScaleMode = 0;

	/* support aux H264 YUY2 NV12 */
	g_uvc_param.eu_h264_ctrl_def.bStreamMuxOption = 0x0f;
	g_uvc_param.eu_h264_ctrl_cur.bStreamMuxOption = 0x0f;
	g_uvc_param.eu_h264_ctrl_res.bStreamMuxOption = 0x0f;
	g_uvc_param.eu_h264_ctrl_min.bStreamMuxOption = 0x0f;
	g_uvc_param.eu_h264_ctrl_max.bStreamMuxOption = 0x0f;

	g_uvc_param.eu_h264_ctrl_def.bStreamFormat = 0;
	g_uvc_param.eu_h264_ctrl_cur.bStreamFormat = 0;
	g_uvc_param.eu_h264_ctrl_res.bStreamFormat = 0;
	g_uvc_param.eu_h264_ctrl_min.bStreamFormat = 0;
	g_uvc_param.eu_h264_ctrl_max.bStreamFormat = 0;

	g_uvc_param.eu_h264_ctrl_def.bEntropyCABAC = 0;
	g_uvc_param.eu_h264_ctrl_cur.bEntropyCABAC = 0;
	g_uvc_param.eu_h264_ctrl_res.bEntropyCABAC = 0;
	g_uvc_param.eu_h264_ctrl_min.bEntropyCABAC = 0;
	g_uvc_param.eu_h264_ctrl_max.bEntropyCABAC = 0;

	g_uvc_param.eu_h264_ctrl_def.bTimestamp = 0;
	g_uvc_param.eu_h264_ctrl_cur.bTimestamp = 0;
	g_uvc_param.eu_h264_ctrl_res.bTimestamp = 0;
	g_uvc_param.eu_h264_ctrl_min.bTimestamp = 0;
	g_uvc_param.eu_h264_ctrl_max.bTimestamp = 0;

	g_uvc_param.eu_h264_ctrl_def.bNumOfReorderFrames = 0;
	g_uvc_param.eu_h264_ctrl_cur.bNumOfReorderFrames = 0;
	g_uvc_param.eu_h264_ctrl_res.bNumOfReorderFrames = 0;
	g_uvc_param.eu_h264_ctrl_min.bNumOfReorderFrames = 0;
	g_uvc_param.eu_h264_ctrl_max.bNumOfReorderFrames = 0;

	g_uvc_param.eu_h264_ctrl_def.bPreviewFlipped = 0;
	g_uvc_param.eu_h264_ctrl_cur.bPreviewFlipped = 0;
	g_uvc_param.eu_h264_ctrl_res.bPreviewFlipped = 0;
	g_uvc_param.eu_h264_ctrl_min.bPreviewFlipped = 0;
	g_uvc_param.eu_h264_ctrl_max.bPreviewFlipped = 0;

	g_uvc_param.eu_h264_ctrl_def.bView = 0;
	g_uvc_param.eu_h264_ctrl_cur.bView = 0;
	g_uvc_param.eu_h264_ctrl_res.bView = 0;
	g_uvc_param.eu_h264_ctrl_min.bView = 0;
	g_uvc_param.eu_h264_ctrl_max.bView = 0;

	g_uvc_param.eu_h264_ctrl_def.bReserved1 = 0;
	g_uvc_param.eu_h264_ctrl_cur.bReserved1 = 0;
	g_uvc_param.eu_h264_ctrl_res.bReserved1 = 0;
	g_uvc_param.eu_h264_ctrl_min.bReserved1 = 0;
	g_uvc_param.eu_h264_ctrl_max.bReserved1 = 0;

	g_uvc_param.eu_h264_ctrl_def.bReserved2 = 0;
	g_uvc_param.eu_h264_ctrl_cur.bReserved2 = 0;
	g_uvc_param.eu_h264_ctrl_res.bReserved2 = 0;
	g_uvc_param.eu_h264_ctrl_min.bReserved2 = 0;
	g_uvc_param.eu_h264_ctrl_max.bReserved2 = 0;

	g_uvc_param.eu_h264_ctrl_def.bStreamID = 0x07;
	g_uvc_param.eu_h264_ctrl_cur.bStreamID = 0x07;
	g_uvc_param.eu_h264_ctrl_res.bStreamID = 0x07;
	g_uvc_param.eu_h264_ctrl_min.bStreamID = 0x07;
	g_uvc_param.eu_h264_ctrl_max.bStreamID = 0x07;

	g_uvc_param.eu_h264_ctrl_def.bSpatialLayerRatio = 0;
	g_uvc_param.eu_h264_ctrl_cur.bSpatialLayerRatio = 0;
	g_uvc_param.eu_h264_ctrl_res.bSpatialLayerRatio = 0;
	g_uvc_param.eu_h264_ctrl_min.bSpatialLayerRatio = 0;
	g_uvc_param.eu_h264_ctrl_max.bSpatialLayerRatio = 0;

	g_uvc_param.eu_h264_ctrl_def.wLeakyBucketSize = 0;
	g_uvc_param.eu_h264_ctrl_cur.wLeakyBucketSize = 0;
	g_uvc_param.eu_h264_ctrl_res.wLeakyBucketSize = 0;
	g_uvc_param.eu_h264_ctrl_min.wLeakyBucketSize = 0;
	g_uvc_param.eu_h264_ctrl_max.wLeakyBucketSize = 0;
}

void uvc_control_request_param_init()
{
	memset(&g_uvc_param, 0, sizeof g_uvc_param);

	//put here for temporary
	memset(&g_infotm_ctrl_state, 0, sizeof g_infotm_ctrl_state);

	g_infotm_ctrl_state.bVideoLoopTime = 5;
	g_infotm_ctrl_state.bVideoState = 0x00;
	g_infotm_ctrl_state.wBatteryInfo = 90;
	sprintf(g_infotm_ctrl_state.version, "%s", "v1.0.1");
	g_infotm_ctrl_state.utc_seconds = time(NULL);
	g_infotm_ctrl_state.storage_info.free = 2000;
	g_infotm_ctrl_state.storage_info.total = 4000;

	/* camera terminal  */
	/*1. ae mode*/
	/* Current setting attribute */
	/* 0: auto
	 * 1: manual
	 * 2: shutter_priority
	 * 3: aperture_priority
	 * res is the bitmask of control abitlity
	 * 1: auto
	 * 2: manual
	 * 4: shutter_priority
	 * 8: aperture_priority
	 * */
	g_uvc_param.ct_ctrl_def.AEMode.d8 = 1;
	g_uvc_param.ct_ctrl_cur.AEMode.d8 = 1;
	g_uvc_param.ct_ctrl_res.AEMode.d8 = 0x03; /*enable auto and manual*/
	g_uvc_param.ct_ctrl_min.AEMode.d8 = 0;
	g_uvc_param.ct_ctrl_max.AEMode.d8 = 3;

	/*2. exposure tims abs */
	/* 1 ~ 9 correspond to -4 ~ 4 */
	g_uvc_param.ct_ctrl_def.dwExposureTimeAbsolute  = 5;
	g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute  = 5;
	g_uvc_param.ct_ctrl_res.dwExposureTimeAbsolute  = 1;
	g_uvc_param.ct_ctrl_min.dwExposureTimeAbsolute  = 1;
	g_uvc_param.ct_ctrl_max.dwExposureTimeAbsolute  = 9;

	/*-----------------------------------------------------------*/
	/* processing uinit */
	g_uvc_param.pu_ctrl_def.wBrightness = 127;
	g_uvc_param.pu_ctrl_cur.wBrightness  = 127;
	g_uvc_param.pu_ctrl_res.wBrightness  = 1;
	g_uvc_param.pu_ctrl_min.wBrightness  = 0; 
	g_uvc_param.pu_ctrl_max.wBrightness  = 255;

	g_uvc_param.pu_ctrl_def.wContrast= 127;
	g_uvc_param.pu_ctrl_cur.wContrast  = 127;
	g_uvc_param.pu_ctrl_res.wContrast  = 1;
	g_uvc_param.pu_ctrl_min.wContrast  = 0;
	g_uvc_param.pu_ctrl_max.wContrast  = 255;

	g_uvc_param.pu_ctrl_def.wGain = 0;
	g_uvc_param.pu_ctrl_cur.wGain  = 0;
	g_uvc_param.pu_ctrl_res.wGain  = 1;
	g_uvc_param.pu_ctrl_min.wGain  = 0;
	g_uvc_param.pu_ctrl_max.wGain  = 255;

	/* 1: daylight
	 * 2: cloudy
	 * 3: tungsten
	 * 4: fluoresent*/
	g_uvc_param.pu_ctrl_def.wWhiteBalanceTemperature  = 1;
	g_uvc_param.pu_ctrl_cur.wWhiteBalanceTemperature  = 1;
	g_uvc_param.pu_ctrl_res.wWhiteBalanceTemperature  = 1;
	g_uvc_param.pu_ctrl_min.wWhiteBalanceTemperature  = 1;
	g_uvc_param.pu_ctrl_max.wWhiteBalanceTemperature  = 4;

	g_uvc_param.pu_ctrl_def.bWhiteBalanceTemperatureAuto = 0;
	g_uvc_param.pu_ctrl_cur.bWhiteBalanceTemperatureAuto = 0;
	g_uvc_param.pu_ctrl_res.bWhiteBalanceTemperatureAuto = 1;
	g_uvc_param.pu_ctrl_min.bWhiteBalanceTemperatureAuto = 0;
	g_uvc_param.pu_ctrl_max.bWhiteBalanceTemperatureAuto = 1;

	g_uvc_param.pu_ctrl_def.wHue= 127;
	g_uvc_param.pu_ctrl_cur.wHue  = 127;
	g_uvc_param.pu_ctrl_res.wHue  = 1;
	g_uvc_param.pu_ctrl_min.wHue  = 0;
	g_uvc_param.pu_ctrl_max.wHue  = 255;

	g_uvc_param.pu_ctrl_def.wSaturation= 127;
	g_uvc_param.pu_ctrl_cur.wSaturation  = 127;
	g_uvc_param.pu_ctrl_res.wSaturation  = 1;
	g_uvc_param.pu_ctrl_min.wSaturation  = 0;
	g_uvc_param.pu_ctrl_max.wSaturation  = 255;

	g_uvc_param.pu_ctrl_def.wSharpness= 127;
	g_uvc_param.pu_ctrl_cur.wSharpness  = 127;
	g_uvc_param.pu_ctrl_res.wSharpness  = 1;
	g_uvc_param.pu_ctrl_min.wSharpness  = 0;
	g_uvc_param.pu_ctrl_max.wSharpness  = 255;

	/* h264 uvcx_param init */
	uvcx_control_request_param_init();
	/*TODO*/
	g_receiver.cur_flag = 0;
	g_receiver.cur_pid = 0;
	g_receiver.status = 0;
	memset(g_files, 0, sizeof(g_files));
}

int uvc_h264_looping(char *channel)
{
	char *device = "/dev/video0";
	char header_buf[HEADER_LENGTH] = {0};
	int header_size = 0;

	//struct uvc_device *dev;
	int bulk_mode = 0;
	char *mjpeg_image= NULL;
	fd_set fds;
	int ret, opt;
	int format = 0;

	struct fr_buf_info vbuf = FR_BUFINITIALIZER;
	struct fr_buf_info buf = FR_BUFINITIALIZER;

//	dev = uvc_open(device);
	udev = uvc_open(dev_name);
	if (udev == NULL) {
		UVC_PRINTF(UVC_LOG_ERR, " guvc open device %s failed \n", dev_name);
		exit(-1);
	}

	uvc_event_callback_register(udev,
			uvc_event_user_process,
			uvc_set_user_resolution);
	uvc_control_request_param_init();
	uvc_control_request_callback_init();

	FD_ZERO(&fds);
	FD_SET(udev->fd, &fds);

	/* get launcher global state */
	while(1) {
		usleep(20000);
		g_spv_camera_state = GetSpvState();
        if(g_spv_camera_state != NULL) {
        	if(g_spv_camera_state->initialized == 1)
            	break;
        }
	}

	while (1) {
		fd_set efds = fds;
		fd_set wfds = fds;

		ret = select(udev->fd + 1, NULL, &wfds, &efds, NULL);

		if (FD_ISSET(udev->fd, &efds))
			uvc_events_process(udev);

		if(g_video_is_reseting) {
			UVC_PRINTF(UVC_LOG_TRACE, "reset fr\n");
			fr_INITBUF(&buf, NULL);
			fr_INITBUF(&vbuf, NULL);
			g_video_is_reseting = 0;
		}

		if (FD_ISSET(udev->fd, &wfds) && !g_video_is_reseting) {
			if(!confirm_p2p_stream_started()) {
				if(g_spv_camera_state->mode == MODE_PHOTO && g_spv_camera_state->isPicturing) {

					ret = camera_snap_one_shot(FCAM);
					UVC_PRINTF(UVC_LOG_TRACE, "camera_snap_one_shot, ret: %d\n", ret);
					ret = video_get_snap(PFCHN, &buf);
					if(ret < 0) {
						UVC_PRINTF(UVC_LOG_TRACE, "video_get_snap failed, ret: %d\n", ret);
					}
					uvc_video_process(udev, buf.virt_addr, buf.size, NULL, 0);
					video_put_snap(PFCHN, &buf);
					usleep(50000);//for one frame not correct when continue snap no delay.
					ret = camera_snap_exit(FCAM);
					UVC_PRINTF(UVC_LOG_TRACE, "camera_snap_exit, ret: %d\n", ret);
					g_spv_camera_state->isPicturing = 0;

				} else {
					header_size = video_get_header(channel, header_buf, HEADER_LENGTH);
					ret = video_get_frame(channel, &vbuf);
					if (!ret) {
						ret = uvc_video_process(udev, vbuf.virt_addr, vbuf.size, header_buf, header_size);
						(void)ret;
					} else
						UVC_PRINTF(UVC_LOG_WARRING, "# guvc WARINNING: "
								"video_get_frame << %d KB\n",
								vbuf.size >> 10);
					video_put_frame(channel, &vbuf);
				}
			} else
				UVC_PRINTF(UVC_LOG_ERR, "#open stream failed\n");
		}
	}

	uvc_close(udev);
	return 0;
}

int uvc_server_stop(void)
{
	pthread_cancel(uvc_h264_t);
//	uvc_close(udev);
	uvc_exit(udev);
}

int find_node_name(void)
{
	struct dirent *ent = NULL;
	DIR *pDir;
	char dir[512];

	if ((pDir = opendir("/dev")) == NULL) {
		DBG("Cannot open dir dev\n");
		return -1;
	}

	while ((ent = readdir(pDir)) != NULL) {
		if (!strncmp("video", ent->d_name, 4)) {
			sprintf(dev_name, "/dev/%s", ent->d_name);
			DBG("dev name %s\n", dev_name);
			return 0;
		}
	}

	DBG("cannot fine video node\n");
	return -1;
}


int uvc_server(char *channel)
{
 //   pthread_t uvc_h264_t;

    if(channel == NULL) {
		UVC_PRINTF(UVC_LOG_ERR, "# input channel is NULL\n");
        return -1;
    }
    
	find_node_name();
	//if(access("/dev/video0", F_OK) && access("/dev/video1", F_OK)) {
	if(access(dev_name, F_OK)) {
		UVC_PRINTF(UVC_LOG_ERR, "# uvc access %s failed\n", dev_name);
		return -3;
	}
	pthread_create(&uvc_h264_t, NULL, uvc_h264_looping, channel);

    UVC_PRINTF(UVC_LOG_INFO, "# uvc calibration finished\n");
    sleep(1);
    return 0;
}
