#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <pthread.h>
#include <dirent.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <qsdk/videobox.h>
#include <guvc.h>

#include <time.h>

#define CONTROL_ENTRY "isp"

static struct uvc_device *g_dev = NULL;


#define INFOTM_PACKET_START		0
#define INFOTM_PACKET_PENDING	1
#define INFOTM_PACKET_END			2
#define CHANNEL_NUM 2

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

static struct infotm_data_receiver g_receiver;
char g_files[256] = {0};

/*signal flag for uvc_server*/
static pthread_mutex_t uvc_server_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
static int volatile is_uvc_only = 0;
static int volatile is_uvc_composite = 0;
int is_mjpeg = 0;

static void usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s [options]\n", argv0);
	fprintf(stderr, "Available options are\n");
	fprintf(stderr, " -c		videobox channel\n");
	fprintf(stderr, " -b		Use bulk mode\n");
	fprintf(stderr, " -d device	Video device\n");
	fprintf(stderr, " -i image	MJPEG image\n");
	fprintf(stderr, " -h		Print this help screen and exit\n");
}

void uvc_kill_handler(int signo)
{
	UVC_PRINTF(UVC_LOG_ERR,"Exiting signo:%d\n", signo);
	uvc_exit(g_dev);
	exit(0);
}

void uvc_ctrl_serv_handler(int signo)
{
	UVC_PRINTF(UVC_LOG_ERR,"Exiting signo:%d\n", signo);

	pthread_mutex_lock(&uvc_server_flag_mutex);
	if (signo == SIGUSR1)
		is_uvc_composite = 1;
	else
		is_uvc_only = 1;
	pthread_mutex_unlock(&uvc_server_flag_mutex);
}

void uvc_signal()
{
	// Setup kill handler
	signal(SIGINT, uvc_kill_handler);
	signal(SIGTERM, uvc_kill_handler);
	signal(SIGHUP, uvc_kill_handler);
	signal(SIGQUIT, uvc_kill_handler);
	signal(SIGABRT, uvc_kill_handler);
	signal(SIGKILL, uvc_kill_handler);
	signal(SIGSEGV, uvc_kill_handler);

	signal(SIGUSR1, uvc_ctrl_serv_handler);
	signal(SIGUSR2, uvc_ctrl_serv_handler);
}

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
	union uvc_camera_terminal_control_u* ctrl;

	ctrl = (union uvc_camera_terminal_control_u*)&d->data;
	if (d->length != sizeof ctrl->dwExposureTimeAbsolute) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute = ctrl->dwExposureTimeAbsolute;
	UVC_PRINTF(UVC_LOG_ERR, "### %s [CT][dwExposureTimeAbsolute]: %d\n",__func__,
		g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute);

	//TODO: callback isp control unit
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

	ctrl = (infotm_ctrl_package_t *)&d->data;
	if (d->length != sizeof *ctrl) {
		UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
			__func__, d->length);
		return;
	}

	cmd = ctrl->cmd_id;
	DBG("cmd is %d\n", cmd);

	switch (cmd) {
		case UVCX_INFOTM_CMD_TAKE_PIC:
			DBG("####infotm set: UVCX_INFOTM_CMD_TAKE_PIC\n");
			break;
		case UVCX_INFOTM_CMD_START_VIDEO:
			DBG("####infotm set: UVCX_INFOTM_CMD_START_VIDEO\n");
			g_infotm_ctrl_state.bVideoState = 0xff;
			break;
		case UVCX_INFOTM_CMD_STOP_VIDEO:
			DBG("####infotm set: UVCX_INFOTM_CMD_STOP_VIDEO\n");
			g_infotm_ctrl_state.bVideoState = 0x00;
			break;
		case UVCX_INFOTM_CMD_VIDEO_LOOP:
			DBG("####infotm set: UVCX_INFOTM_CMD_VIDEO_LOOP\n");
			g_infotm_ctrl_state.bVideoLoopTime = ctrl->data.bVideoLoopTime;
			DBG("####[loop time]\tctrl : %d\n", ctrl->data.bVideoLoopTime);
			DBG("####[loop time]\tg_infotm_ctrl_state: %d\n", 
					g_infotm_ctrl_state.bVideoLoopTime);
			break;
		case UVCX_INFOTM_CMD_SYNC_TIME:
			DBG("####infotm set: UVCX_INFOTM_CMD_SYNC_TIME\n");
			g_infotm_ctrl_state.utc_seconds = ctrl->data.utc_seconds;
			DBG("####[sync time]\n"); 
			DBG("g_inftom_ctrl_state time: %s\n", ctime(&g_infotm_ctrl_state.utc_seconds));
			break;

		case UVCX_INFOTM_CMD_GET_BATTERY_INFO:
			DBG("####infotm set: UVCX_INFOTM_CMD_GET_BATTERY_INFO\n");
			break;
		case UVCX_INFOTM_CMD_GET_SD_INFO:
			DBG("####infotm set: UVCX_INFOTM_CMD_GET_SD_INFO\n");
			break;
		case UVCX_INFOTM_CMD_GET_VERSION:
			DBG("####infotm set: UVCX_INFOTM_CMD_GET_VERSION\n");
			break;
		case UVCX_INFOTM_CMD_GET_VIDEO_STATE:
			DBG("####infotm set: UVCX_INFOTM_CMD_GET_VIDEO_STATE\n");
			break;
		default:
			DBG("#unknown infotm ctrl cmd: %d\n", cmd);	
			break;
	}
	DBG("ctrl: ");
	for(i = 0; i < sizeof(*ctrl); i++)
		printf(" 0x%02x", *((char*)ctrl + i));
	printf("\n");
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
		ret  = camera_ae_is_enabled(CONTROL_ENTRY);
		if (!ret)
			g_uvc_param.ct_ctrl_cur.AEMode.d8 = UVC_AUTO_EXPOSURE_MODE_AUTO;
		else
			g_uvc_param.ct_ctrl_cur.AEMode.d8 = UVC_AUTO_EXPOSURE_MODE_MANUAL;

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
		resp->data[0] = cs;
		UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
			__func__, cs);
		break;
	}
}

void uvc_ct_exposure_abs_setup(struct uvc_device *dev, 
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	union uvc_camera_terminal_control_u* ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);

	ctrl = (union uvc_camera_terminal_control_u*)&resp->data;
	resp->length =sizeof ctrl->dwExposureTimeAbsolute;

	memset(ctrl, 0, sizeof *ctrl);

	switch (req) {
	case UVC_GET_CUR:
		/*TODO : get current value form isp or camera*/
		ctrl->dwExposureTimeAbsolute =
			g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute;
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
	void * p;
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
	p = (void *)&resp->data;
	memset(p, 0, sizeof(WORD));
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
}

void uvc_eu_h264_bitrate_setup(struct uvc_device *dev,
			   struct uvc_request_data *resp, uint8_t req, uint8_t cs)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, req);
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
	UVC_PRINTF(UVC_LOG_ERR,"# uvc set resolution :%dx%d@%dfps ,format=%08x\n", width, height, fps, dev->fcc);
	if (dev->fcc == V4L2_PIX_FMT_H264) {
		is_mjpeg = 0;
	} else if (dev->fcc == V4L2_PIX_FMT_MJPEG){
		is_mjpeg = 1;
	}
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

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_VIDEO_CONFIG_PROBE,
		uvc_eu_h264_probe_setup,
		uvc_eu_h264_probe_data,
		sizeof(struct _uvcx_video_config_probe_commit_t));

	uvc_cs_ops_register(UVC_CTRL_H264_EXTENSION_UNIT_ID,
		UVCX_VIDEO_CONFIG_COMMIT,
		uvc_eu_h264_commit_setup,
		uvc_eu_h264_commit_data, sizeof(WORD));

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

	g_uvc_param.eu_h264_ctrl_def.dwBitRate = 1500000;
	g_uvc_param.eu_h264_ctrl_cur.dwBitRate = 1500000;
	g_uvc_param.eu_h264_ctrl_res.dwBitRate = 0;
	g_uvc_param.eu_h264_ctrl_min.dwBitRate = 18432000;
	g_uvc_param.eu_h264_ctrl_max.dwBitRate = 55296000;

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
	g_uvc_param.eu_h264_ctrl_min.wWidth = 640;
	g_uvc_param.eu_h264_ctrl_max.wWidth = 1920;

	g_uvc_param.eu_h264_ctrl_def.wHeight = 960;
	g_uvc_param.eu_h264_ctrl_cur.wHeight = 960;
	g_uvc_param.eu_h264_ctrl_res.wHeight = 0;
	g_uvc_param.eu_h264_ctrl_min.wHeight = 480;
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
	g_uvc_param.ct_ctrl_def.AEMode.d8 = UVC_AUTO_EXPOSURE_MODE_MANUAL; /* def 2 */
	g_uvc_param.ct_ctrl_cur.AEMode.d8 = UVC_AUTO_EXPOSURE_MODE_MANUAL;/*cur 4 */
	g_uvc_param.ct_ctrl_res.AEMode.d8 = UVC_AUTO_EXPOSURE_MODE_MANUAL; /*res 1 */
	g_uvc_param.ct_ctrl_min.AEMode.d8=  UVC_AUTO_EXPOSURE_MODE_MANUAL;/* min 1 */
	g_uvc_param.ct_ctrl_max.AEMode.d8  = UVC_AUTO_EXPOSURE_MODE_MANUAL; /*max 8 */

	/*2. exposure tims abs */
	g_uvc_param.ct_ctrl_def.dwExposureTimeAbsolute  = 5200;
	g_uvc_param.ct_ctrl_cur.dwExposureTimeAbsolute  = 6400;
	/*  Resolution  cur_val = 200*n + 200*/
	g_uvc_param.ct_ctrl_res.dwExposureTimeAbsolute  = 200; /* res: 20 ms*/
	g_uvc_param.ct_ctrl_min.dwExposureTimeAbsolute  = 200; /* min: 20 ms*/
	g_uvc_param.ct_ctrl_max.dwExposureTimeAbsolute  = 100000; /*max 10 sec*/

	/*-----------------------------------------------------------*/
	/* processing uinit */
	g_uvc_param.pu_ctrl_def.wBrightness = 127;
	g_uvc_param.pu_ctrl_cur.wBrightness  = 127;
	g_uvc_param.pu_ctrl_res.wBrightness  = 10;
	g_uvc_param.pu_ctrl_min.wBrightness  = 0; 
	g_uvc_param.pu_ctrl_max.wBrightness  = 255;

	g_uvc_param.pu_ctrl_def.wContrast= 127;
	g_uvc_param.pu_ctrl_cur.wContrast  = 127;
	g_uvc_param.pu_ctrl_res.wContrast  = 10;
	g_uvc_param.pu_ctrl_min.wContrast  = 0;
	g_uvc_param.pu_ctrl_max.wContrast  = 255;

	g_uvc_param.pu_ctrl_def.wHue= 127;
	g_uvc_param.pu_ctrl_cur.wHue  = 127;
	g_uvc_param.pu_ctrl_res.wHue  = 10;
	g_uvc_param.pu_ctrl_min.wHue  = 0;
	g_uvc_param.pu_ctrl_max.wHue  = 255;

	g_uvc_param.pu_ctrl_def.wSaturation= 30;
	g_uvc_param.pu_ctrl_cur.wSaturation  = 30;
	g_uvc_param.pu_ctrl_res.wSaturation  = 10;
	g_uvc_param.pu_ctrl_min.wSaturation  = 0;
	g_uvc_param.pu_ctrl_max.wSaturation  = 255;

	g_uvc_param.pu_ctrl_def.wSharpness= 127;
	g_uvc_param.pu_ctrl_cur.wSharpness  = 127;
	g_uvc_param.pu_ctrl_res.wSharpness  = 10;
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

#if 0
int main(int argc, char *argv[])
{
	char *device = "/dev/video0";
	char *channel = CHANNELNAME;
	char header_buf[HEADER_LENGTH] = {0};

	struct uvc_device *dev;
	int bulk_mode = 0;
	char *mjpeg_image= NULL;
	fd_set fds;
	int ret, opt;
	int format = 0;
	static uint32_t fno = 0;
	int header_size = 0;
	int is_mjpeg = 0;
	void * imgdata = NULL;
	int imgsize = 0;

	struct fr_buf_info vbuf = FR_BUFINITIALIZER;

	uvc_signal();

	while ((opt = getopt(argc, argv, "c:bd:hi:j")) != -1) {
		switch (opt) {
		case 'c':
			channel = optarg;
			break;

		case 'b':
			bulk_mode = 1;
			break;

		case 'd':
			device = optarg;
			break;

		case 'h':
			usage(argv[0]);
			return 0;

		case 'i':
			mjpeg_image = optarg;
			break;

		case 'j':
			is_mjpeg = 1;
			break;

		default:
			fprintf(stderr, "Invalid option '-%c'\n", opt);
			usage(argv[0]);
			return 1;
		}
	}

	dev = uvc_open(device, 0);
	if (dev == NULL) {
		UVC_PRINTF(UVC_LOG_ERR, " guvc open device %s failed \n", device);
		exit(-1);
	}

	g_dev = dev; /* for signal handler */

	uvc_event_callback_register(dev,
			uvc_event_user_process,
			uvc_set_user_resolution);
	uvc_control_request_param_init();
	uvc_control_request_callback_init();

	/* Note: init first frame data */
	if (!mjpeg_image) {
		if (!is_mjpeg) {
			header_size = video_get_header(channel, header_buf, HEADER_LENGTH);
			ret = video_get_frame(channel, &vbuf);
			if (!ret && header_size > 0) {

				imgdata = malloc(vbuf.size + header_size);
				imgsize = header_size + vbuf.size;

				uvc_set_def_img(dev,
					imgdata,
					imgsize,
					UVC_DEF_IMG_FORM_PIC);
			}

		} else {
			ret = video_get_frame(channel, &vbuf);
			if (!ret) {

				uvc_set_def_img(dev,
					vbuf.virt_addr,
					vbuf.size,
					UVC_DEF_IMG_FORM_FRBUF);
			}
		}

		video_put_frame(channel, &vbuf);
	} else {
		image_load(dev, mjpeg_image);
	}

	FD_ZERO(&fds);
	FD_SET(dev->fd, &fds);

	while (1) {
		fd_set efds = fds;
		fd_set wfds = fds;

		ret = select(dev->fd + 1, NULL, &wfds, &efds, NULL);

		if (FD_ISSET(dev->fd, &efds))
			uvc_events_process(dev);

		if (FD_ISSET(dev->fd, &wfds)) {
			fno++;
			if (!is_mjpeg) {
				header_size = video_get_header(channel, header_buf, HEADER_LENGTH);
				ret = video_get_frame(channel, &vbuf);
				if (!ret) {
					ret = uvc_video_process(dev, vbuf.virt_addr, vbuf.size, header_buf, header_size);
					(void)ret;
				} else
					UVC_PRINTF(UVC_LOG_WARRING, "# guvc WARINNING: "
							"video_get_frame << %d KB F:%d\n",
							vbuf.size >> 10, fno);
			} else {
				ret = video_get_frame(channel, &vbuf);
				if (!ret) {
					ret = uvc_video_process(dev, vbuf.virt_addr, vbuf.size, NULL, 0);
					(void)ret;
				} else
					UVC_PRINTF(UVC_LOG_WARRING, "# guvc WARINNING: "
							"video_get_frame << %d KB F:%d\n",
							vbuf.size >> 10, fno);
			}
			video_put_frame(channel, &vbuf);
		}
	}

	uvc_close(dev);
	return 0;
}
#else

#define UVC_THREAD_STOPPING 0
#define UVC_THREAD_EXIT 1
#define UVC_THREAD_RUNNING 2

pthread_t uvc_thread_t;
char device[128] = {"/dev/video0"};
char channel_h264[128] = "h264-stream";
char channel_mjpeg[128] = "jpeg-out"; 
int is_bulk = 0;
volatile int uvc_thread_state = 1;

void uvc_looping(void *c)
{
	struct uvc_device *dev;

	fd_set fds;
	int ret, opt;
	static uint32_t fno = 0;
	char **channel_tbl = (char**)c;
	
	char *channel;

	int header_size = 0;
	char header_buf[HEADER_LENGTH] = {0};

	void * imgdata = NULL;
	int imgsize = 0;

	struct fr_buf_info vbuf = FR_BUFINITIALIZER;
	static int prev_mode = 0;

	channel = is_mjpeg?channel_tbl[1]:channel_tbl[0];
	dev = uvc_open(device, is_bulk);
	if (dev == NULL) {
		UVC_PRINTF(UVC_LOG_ERR, " guvc open device %s failed \n", device);
		exit(-1);
	}

	UVC_PRINTF(UVC_LOG_ERR, " #guvc open device %s, channel-h264 %s, channle-mjpg %s, is_mjpeg %d\n",
		device, channel_tbl[0], channel_tbl[1], is_mjpeg);
	g_dev = dev; /* for signal handler */

	uvc_event_callback_register(dev,
			uvc_event_user_process,
			uvc_set_user_resolution);
	uvc_control_request_param_init();
	uvc_control_request_callback_init();

	/* Note: init first frame data */
	if (!is_mjpeg) {
		header_size = video_get_header(channel, header_buf, HEADER_LENGTH);
		ret = video_get_frame(channel, &vbuf);
		if (!ret && header_size > 0) {

			imgdata = malloc(vbuf.size + header_size);
			imgsize = header_size + vbuf.size;

			uvc_set_def_img(dev,
				imgdata,
				imgsize,
				UVC_DEF_IMG_FORM_PIC);
		}

	} else {
		ret = video_get_frame(channel, &vbuf);
		if (!ret) {

			uvc_set_def_img(dev,
				vbuf.virt_addr,
				vbuf.size,
				UVC_DEF_IMG_FORM_FRBUF);
		}
	}

	video_put_frame(channel, &vbuf);

	FD_ZERO(&fds);
	FD_SET(dev->fd, &fds);

	prev_mode = is_mjpeg;
	while (uvc_thread_state == UVC_THREAD_RUNNING) {
		fd_set efds = fds;
		fd_set wfds = fds;

		ret = select(dev->fd + 1, NULL, &wfds, &efds, NULL);

		if (FD_ISSET(dev->fd, &efds))
			uvc_events_process(dev);

		channel = is_mjpeg?channel_tbl[1]:channel_tbl[0];
		if (FD_ISSET(dev->fd, &wfds)) {
			if (prev_mode != is_mjpeg) {
				UVC_PRINTF(UVC_LOG_ERR, "# guvc prev mode =%d, mode=%d, channel=%s \n", prev_mode, is_mjpeg, channel);
				prev_mode= is_mjpeg;
				memset(&vbuf, 0, sizeof(vbuf));
			}
			fno++;
			if (!is_mjpeg) {
				header_size = video_get_header(channel, header_buf, HEADER_LENGTH);
				ret = video_get_frame(channel, &vbuf);
				if (!ret) {
					ret = uvc_video_process(dev, vbuf.virt_addr, vbuf.size, header_buf, header_size);
					(void)ret;
				} else
					UVC_PRINTF(UVC_LOG_WARRING, "# guvc WARINNING: "
							"video_get_frame << %d KB F:%d\n",
							vbuf.size >> 10, fno);
			} else {
				ret = video_get_frame(channel, &vbuf);
				if (!ret) {
					ret = uvc_video_process(dev, vbuf.virt_addr, vbuf.size, NULL, 0);
					(void)ret;
				} else
					UVC_PRINTF(UVC_LOG_WARRING, "# guvc WARINNING: "
							"video_get_frame << %d KB F:%d\n",
							vbuf.size >> 10, fno);
			}
			video_put_frame(channel, &vbuf);
		}
	}

	UVC_PRINTF(UVC_LOG_ERR, "# %s uvc_thread_state = %d\n", __func__, uvc_thread_state);
	uvc_exit(dev);
	uvc_thread_state = UVC_THREAD_STOPPING;

	return 0;
}

int uvc_server_stop(void)
{
	int retry = 10;
	UVC_PRINTF(UVC_LOG_ERR, "#1 uvc_thread_state = %d\n", uvc_thread_state);
	if (uvc_thread_state ==UVC_THREAD_RUNNING) {

		uvc_thread_state = UVC_THREAD_EXIT;

		/* it wait for thread exit successfully */
		while (uvc_thread_state != UVC_THREAD_STOPPING && retry > 0) {
			UVC_PRINTF(UVC_LOG_ERR, "#2 uvc_thread_state = %d\n", uvc_thread_state);
			sleep(1);
			retry --;
		}

		if (uvc_thread_state == UVC_THREAD_STOPPING) {
			UVC_PRINTF(UVC_LOG_ERR, "#3 uvc_thread_state = %d\n", uvc_thread_state);
			pthread_cancel(uvc_thread_t);
			//uvc_exit(g_dev);
		}
	}

	if (retry == 0) {
		pthread_cancel(uvc_thread_t);
		uvc_exit(g_dev);
	}

	sleep(1);
	UVC_PRINTF(UVC_LOG_ERR, "#4  uvc_thread_state = %d\n", uvc_thread_state);
	return 0;
}

int find_valid_node(char *dev_name)
{
	struct dirent *ent = NULL;
	DIR *pDir = NULL;
	char dir[512] = {0};

	if ((pDir = opendir("/dev")) == NULL) {
		UVC_PRINTF(UVC_LOG_ERR,"Cannot open dir dev\n");
		return -1;
	}

	while ((ent = readdir(pDir)) != NULL) {
		if (!strncmp("video", ent->d_name, 4)) {
			sprintf(dev_name, "/dev/%s", ent->d_name);
			UVC_PRINTF(UVC_LOG_ERR,"dev name %s\n", dev_name);
			return 0;
		}
	}

	UVC_PRINTF(UVC_LOG_ERR,"cannot find video node\n");
	return -1;
}

int uvc_server_start(char *channel[])
{
	int ret  = 0;
	int retry_cnt = 0;
	if(channel == NULL) {
		UVC_PRINTF(UVC_LOG_ERR, "# input channel is NULL\n");
		return -1;
	}

retry:
	find_valid_node(device);
	ret = access(device, F_OK);
	if(ret < 0 && retry_cnt < 10) {
		UVC_PRINTF(UVC_LOG_ERR, "# uvc access %s failed\n", device);
		retry_cnt ++;
		sleep(1);
		goto retry;
	}

	if (retry_cnt >= 10) {
		UVC_PRINTF(UVC_LOG_ERR, "# uvc access %s retry failed \n", device);
		return -3;
	}

	pthread_create(&uvc_thread_t, NULL, uvc_looping, channel);

	UVC_PRINTF(UVC_LOG_INFO, "# uvc_server_start finished\n");
	sleep(1);
	return 0;
}

int exec_shell_cmd(const char * cmd)
{
	printf("%s\n", cmd);

	FILE * fp = NULL;
	fp=popen(cmd, "r");
	if (!fp) {
		printf("popen error \n");
		return -1;
	}

	pclose(fp);
	return 0;
}

int main(int argc, char *argv[])
{
	char functions[128] = {0};
	char cmd[256] = {0};
	static int fno = 0;
	char *channel[CHANNEL_NUM] = {NULL, NULL};
	int ret, opt;

	uvc_signal();

	while ((opt = getopt(argc, argv, "b:c:d:f:m:hj")) != -1) {
		switch (opt) {
		case 'b':
			is_bulk = atoi(optarg);
			UVC_PRINTF(UVC_LOG_ERR, "is_bulk = %d\n", is_bulk);
			break;

		case 'c':
			strncpy(channel_h264 , optarg ,128);
			break;

		case 'd':
			strncpy(device , optarg, 128);
			break;
		case 'f':
			strncpy(functions , optarg, 128);
			UVC_PRINTF(UVC_LOG_ERR,"functions %s\n", functions);
			break;
		case 'm':
			strncpy(channel_mjpeg, optarg, 128);
			break;
		case 'h':
			usage(argv[0]);
			return 0;

		case 'j':
			is_mjpeg = 1;
			break;

		default:
			fprintf(stderr, "Invalid option '-%c'\n", opt);
			usage(argv[0]);
			return 1;
		}
	}

	uvc_set_log_level(UVC_LOG_ERR);
	g_log_level = UVC_LOG_ERR;

	exec_shell_cmd("echo 0 > /sys/class/infotm_usb/infotm0/enable");
	snprintf(cmd, sizeof(cmd),
		"echo \"%s\" > /sys/class/infotm_usb/infotm0/functions",
		"uvc");
	exec_shell_cmd(cmd);
	exec_shell_cmd("echo 1 > /sys/class/infotm_usb/infotm0/enable");

	exec_shell_cmd(
		"echo \"/dev/mmcblk0p1\" > /sys/devices/platform/dwc_otg/gadget/lun0/file");

	UVC_PRINTF(UVC_LOG_ERR, "# uvc_server_start %d\n", fno++);
	uvc_thread_state = UVC_THREAD_RUNNING;
	channel[0] = channel_h264;
	channel[1] = channel_mjpeg;
	ret = uvc_server_start(channel);
	if (ret < 0)
		goto err;

	while (1) {

		if (is_uvc_composite == is_uvc_only) {
			sleep(3);
			continue;
		} else {

			UVC_PRINTF(UVC_LOG_ERR, "# uvc_server_stop\n");
			exec_shell_cmd("echo \"\" >  /sys/devices/platform/dwc_otg/gadget/lun0/file");
			uvc_server_stop();

			if (is_uvc_only) { // event 12

				exec_shell_cmd("echo 0 > /sys/class/infotm_usb/infotm0/enable");

				snprintf(cmd, sizeof(cmd),
				"echo \"%s\" > /sys/class/infotm_usb/infotm0/functions", "uvc");
				exec_shell_cmd(cmd);
				exec_shell_cmd("echo 1 > /sys/class/infotm_usb/infotm0/enable");

			} else if (is_uvc_composite) { // event 10

				exec_shell_cmd("echo 0 > /sys/class/infotm_usb/infotm0/enable");
				snprintf(cmd, sizeof(cmd),
					"echo \"%s\" > /sys/class/infotm_usb/infotm0/functions",
					"uvc,mass_storage");
				exec_shell_cmd(cmd);
				exec_shell_cmd("echo 1 > /sys/class/infotm_usb/infotm0/enable");

				exec_shell_cmd(
					"echo \"/dev/mmcblk0p1\" > /sys/devices/platform/dwc_otg/gadget/lun0/file");
			}

			UVC_PRINTF(UVC_LOG_ERR, "# uvc_server_start %d\n", fno++);
			uvc_thread_state = UVC_THREAD_RUNNING;
			ret = uvc_server_start(channel);
			if (ret < 0)
				break;

			pthread_mutex_lock(&uvc_server_flag_mutex);
			is_uvc_only = is_uvc_composite = 0;
			pthread_mutex_unlock(&uvc_server_flag_mutex);
		}
		sleep(3);
	}

err:
	return 0;
}

#endif
