#ifndef __GUVC_LIB_H__
#define __GUVC_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include <ch9.h>
#include "video.h"
#include <videodev2.h>

#include <uvc.h>
#include <UvcH264.h>

#define clamp(val, min, max) ({                 \
        typeof(val) __val = (val);              \
        typeof(min) __min = (min);              \
        typeof(max) __max = (max);              \
        (void) (&__val == &__min);              \
        (void) (&__val == &__max);              \
        __val = __val < __min ? __min: __val;   \
        __val > __max ? __max: __val; })

#define ARRAY_SIZE(a)	((sizeof(a) / sizeof(a[0])))

#define UVC_STREAM_ON 1
#define UVC_STREAM_OFF 0

#define UVC_LOG_ERR 5
#define UVC_LOG_WARRING 4
#define UVC_LOG_USER_INFO 3
#define UVC_LOG_INFO 2
#define UVC_LOG_DEBUG 1
#define UVC_LOG_TRACE 0

static int g_log_level = UVC_LOG_TRACE; /* Note: every module has a definition by me */

#define UVC_PRINTF(level,format, arg...)	\
	({ if (level >= g_log_level) 	\
		printf("" format, \
			 ## arg); 0; })

#define DBG(fmt, x...) printf("--------->%s::%d\t"fmt, __func__, __LINE__, ##x)

#define HEADER_LENGTH	128

/*
 * Note: terminal and unit id is form terminal and unit desciptor in uvc dev driver.
 * so each uvc device has different uvc unit and termial id.
 */
#define UVC_CTRL_INTERFACE_ID 0
#define UVC_CTRL_CAMERAL_TERMINAL_ID 1
#define UVC_CTRL_PROCESSING_UNIT_ID 2
#define UVC_CTRL_INFOTM_EXTENSION_UNIT_ID 3
#define UVC_CTRL_H264_EXTENSION_UNIT_ID 3
#define UVC_CTRL_OUTPUT_TERMINAL_ID 4

/* user define control request */
#define UVC_INFOTM_EU_CONTROL		UVCX_END + 0x01
#define UVC_INFOTM_EU_CONTROL_DATA	UVCX_END + 0x02
#define UVC_INFOTM_EU_CALIBRATION 	UVCX_END + 0x03

#define UVC_INTF_UNKNOWN 0xff

#define UVC_FORMAT_YUYV 0
#define UVC_FORMAT_MJPEG 1
#define UVC_FORMAT_H264 2

#define UVC_RESOLUTION_H264_640x480 0
#define UVC_RESOLUTION_H264_1472x736 1
#define UVC_RESOLUTION_H264_1920x960 2

#define UVC_RESOLUTION_360P 0
#define UVC_RESOLUTION_720P 1
#define UVC_RESOLUTION_1080P 2
#define UVC_RESOLUTION_3840x1080 3
#define UVC_RESOLUTION_768x768 4
#define UVC_RESOLUTION_1088x1088 5
#define UVC_RESOLUTION_1920x1920 6

#define UVC_DEF_IMG_FORM_FRBUF 1
#define UVC_DEF_IMG_FORM_PIC 0

/*
 *Note: v4l2 buffer alloc macro value ,
 *for example: In 720p case  v4l2 alloc 800 KB
 */
#define UVC_V4L2_BUF_SIZE_360P (500 * 1024)			/* 600KB */
#define UVC_V4L2_BUF_SIZE_720P (800  * 1024)			/* 800KB */
#define UVC_V4L2_BUF_SIZE_1080P (1200 * 1024) 		/* 1200KB */
#define UVC_V4L2_BUF_SIZE_3840x1080 (1600 * 1024)	/* 1600KB */
#define UVC_V4L2_BUF_SIZE_768x768 (800 * 1024) 		/* 800KB */
#define UVC_V4L2_BUF_SIZE_1088x1088 (1200 * 1024) 	/* 1200KB */
#define UVC_V4L2_BUF_SIZE_1920x1920 (1400 * 1024) 	/* 1400KB */

#define UVC_V4L2_BUF_SIZE_DEF (1400 * 1024) 	/* 1400KB */

/*----------------------------------------------------------------------------*/

/* uvc standard control request define */

#define UVC_AUTO_EXPOSURE_MODE_MANUAL (1)				//- manual exposure time, manual iris
#define UVC_AUTO_EXPOSURE_MODE_AUTO (2) 				//- auto exposure time, auto iris
#define UVC_AUTO_EXPOSURE_MODE_SHUTTER_PRIORITY (4)	//- manual exposure time, auto iris
#define  UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY (8)	//- auto exposure time, manual iris
union bAutoExposureMode {
	uint8_t d8;
	struct {
	uint8_t manual:1;
	uint8_t auto_exposure:1;
	uint8_t shutter_priority:1;
	uint8_t aperture_priority:1;
	uint8_t reserved:4;
	}b;
};

struct FocusRelativeControl {
	uint8_t bFocusRelative;
	int8_t bSpeed;
};

struct ZoomRelativeControl {
	uint8_t bZoom;
	uint8_t bDigitalZoom;
	int8_t bSpeed;
};

struct PanTiltAbsoluteControl {
	uint32_t dwPanAbsolute;
	uint32_t dwTiltAbsolute;
};

struct PanTiltRelativeControl {
	int8_t bPanRelative;
	int8_t bPanSpeed;
	int8_t bTiltRelative;
	int8_t bTiltSpeed;
};

struct RollRelativeControl {
	uint8_t bRollRelative;
	int8_t bSpeed;
};

struct WhiteBalanceComponentControl {
	int16_t wWhiteBalanceBlue;
	int16_t wWhiteBalanceRed;
};

union bDevicePowerMode {
	uint8_t d8;
	struct {
		uint8_t PowerModesetting:4;
		uint8_t DependentPower:1;
		uint8_t ubs:1;
		uint8_t battery:1;
		uint8_t ac:1;
	} b;
};

union uvc_video_control_interface_u {
	union bDevicePowerMode DevicePowerMode; 	//VC_VIDEO_POWER_MODE_CONTROL
	uint8_t bRequestErrorCode; 					//VC_REQUEST_ERROR_CODE_CONTROL
};

union uvc_camera_terminal_control_u {
	uint8_t bScanningMode; 					//CT_SCANNING_MODE_CONTROL 

	union bAutoExposureMode AEMode; 		//CT_AE_MODE_CONTROL
	int8_t bAutoExposurePriority; 				//CT_AE_PRIORITY_CONTROL

	int32_t dwExposureTimeAbsolute;			//CT_EXPOSURE_TIME_ABSOLUTE_CONTROL
	uint8_t bExposureTimeRelative;				//CT_EXPOSURE_TIME_RELATIVE_CONTROL

	int16_t wFocusAbsolute;					//CT_FOCUS_ABSOLUTE_CONTROL
	struct FocusRelativeControl FocusRelative;	//CT_FOCUS_RELATIVE_CONTROL
	uint8_t bFocusAuto;						//CT_FOCUS_AUTO_C ONTROL

	int16_t wIrisAbsolute;						//CT_IRIS_ABSOLUTE_CONTROL
	int8_t bIrisRelative;						//CT_IRIS_RELATIVE_CONTROL

	int16_t wObjectiveFocalLength;			//CT_ZOOM_ABSOLUTE_CONTROL
	struct ZoomRelativeControl ZoomRelative;	//CT_ZOOM_RELATIVE_CONTROL

	struct PanTiltAbsoluteControl PanTiltAbsolute;	//CT_PANTILT_ABSOLUTE_CONTROL
	struct PanTiltRelativeControl PanTiltRelative;		//CT_PANTILT_RE LATIVE _CO N TRO L

	uint16_t wRollAbsolute;					//CT_ROLL_ABSOLUTE_CONTROL
	struct RollRelativeControl RollRelative;		//CT_ROLL_RELATIVE_CONTROL
	uint8_t bPrivacy;							//CT_PRIVACY_CONTROL

	uint8_t buf[8];
};

union uvc_processing_unit_control_u {
	int16_t wBacklightCompensation;		//PU_BACKLIGHT_COMPENSATION_CONTROL
	uint16_t wBrightness;					//PU_BRIGHTNESS_CONTROL

	int16_t wContrast;					//PU_CONTRAST_CONTROL
	int16_t wGain;						//PU_GAIN_CONTROL
	int8_t bPowerLineFrequency;			// PU_POWER_LINE_FR EQUENCY_CONTROL

	uint16_t wHue;						//PU_HUE_CONTROL
	int8_t bHueAuto;						//PU_HUE_AUTO_CONTROL

	int16_t wSaturation;					//PU_SATURATION_CONTROL
	int16_t wSharpness;					//PU_SHARPNESS_CONTROL
	int16_t wGamma;						//PU_GAMMA_CONTROL

	int16_t wWhiteBalanceTemperature; 	//PU_WHITE_BALANCE_TEMPERATURE_CONTROL
	int8_t bWhiteBalanceTemperatureAuto;	//PU_WHITE_BALANCE_TEMPERATURE_AUT O _ CONTROL
	struct WhiteBalanceComponentControl WhiteBalanceComponent; //PU_WHITE_BALANCE_COMPONENT_CONTROL
	int8_t bWhiteBalanceComponentAuto;	//PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL

	int16_t wMultiplierStep;				//PU_DIGITAL_MULTIPLIER_CONTROL
	int16_t wMultiplierLimit;				//PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL

	int8_t bVideoStandard; 				//PU_ANALOG_VIDEO_STANDARD_CONTROL
	int8_t bStatus;						//PU_ANALOG_LOCK_STATUS_CONTROL

	uint8_t buf[8];
};

struct uvc_video_control_interface_obj {
	union bDevicePowerMode DevicePowerMode; 	//VC_VIDEO_POWER_MODE_CONTROL
	uint8_t bRequestErrorCode; 					//VC_REQUEST_ERROR_CODE_CONTROL
};

struct uvc_camera_terminal_control_obj {
	uint8_t bScanningMode; 					//CT_SCANNING_MODE_CONTROL 

	union bAutoExposureMode AEMode; 		//CT_AE_MODE_CONTROL
	int8_t bAutoExposurePriority; 				//CT_AE_PRIORITY_CONTROL

	int32_t dwExposureTimeAbsolute;			//CT_EXPOSURE_TIME_ABSOLUTE_CONTROL
	uint8_t bExposureTimeRelative;				//CT_EXPOSURE_TIME_RELATIVE_CONTROL

	int16_t wFocusAbsolute;					//CT_FOCUS_ABSOLUTE_CONTROL
	struct FocusRelativeControl FocusRelative;	//CT_FOCUS_RELATIVE_CONTROL
	uint8_t bFocusAuto;						//CT_FOCUS_AUTO_C ONTROL

	int16_t wIrisAbsolute;						//CT_IRIS_ABSOLUTE_CONTROL
	int8_t bIrisRelative;						//CT_IRIS_RELATIVE_CONTROL

	int16_t wObjectiveFocalLength;			//CT_ZOOM_ABSOLUTE_CONTROL
	struct ZoomRelativeControl ZoomRelative;	//CT_ZOOM_RELATIVE_CONTROL

	struct PanTiltAbsoluteControl PanTiltAbsolute;	//CT_PANTILT_ABSOLUTE_CONTROL
	struct PanTiltRelativeControl PanTiltRelative;		//CT_PANTILT_RE LATIVE _CO N TRO L

	uint16_t wRollAbsolute;					//CT_ROLL_ABSOLUTE_CONTROL
	struct RollRelativeControl RollRelative;		//CT_ROLL_RELATIVE_CONTROL
	uint8_t bPrivacy;							//CT_PRIVACY_CONTROL
};

struct uvc_processing_unit_control_obj {
	int16_t wBacklightCompensation;		//PU_BACKLIGHT_COMPENSATION_CONTROL
	int16_t wBrightness;					//PU_BRIGHTNESS_CONTROL

	int16_t wContrast;					//PU_CONTRAST_CONTROL
	int16_t wGain;						//PU_GAIN_CONTROL
	int8_t bPowerLineFrequency;			// PU_POWER_LINE_FR EQUENCY_CONTROL

	uint16_t wHue;						//PU_HUE_CONTROL
	int8_t bHueAuto;						//PU_HUE_AUTO_CONTROL

	int16_t wSaturation;					//PU_SATURATION_CONTROL
	int16_t wSharpness;					//PU_SHARPNESS_CONTROL
	int16_t wGamma;						//PU_GAMMA_CONTROL

	int16_t wWhiteBalanceTemperature; 	//PU_WHITE_BALANCE_TEMPERATURE_CONTROL
	int8_t bWhiteBalanceTemperatureAuto;	//PU_WHITE_BALANCE_TEMPERATURE_AUT O _ CONTROL
	struct WhiteBalanceComponentControl WhiteBalanceComponent; //PU_WHITE_BALANCE_COMPONENT_CONTROL
	int8_t bWhiteBalanceComponentAuto;	//PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL

	int16_t wMultiplierStep;				//PU_DIGITAL_MULTIPLIER_CONTROL
	int16_t wMultiplierLimit;				//PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL

	int8_t bVideoStandard; 				//PU_ANALOG_VIDEO_STANDARD_CONTROL
	int8_t bStatus;						//PU_ANALOG_LOCK_STATUS_CONTROL
};
/*----------------------------------------------------------------------------*/

/* guvc object */
struct uvc_control_context {
	uint8_t cs;
	uint8_t id;
	uint8_t intf;
};

typedef void (*uvc_event_callback)(struct uvc_device *dev, uint32_t event);
typedef void (*uvc_resolution_callback)(struct uvc_device *dev,
	int width, int height, int fps);

struct uvc_device {
	int fd;

	struct uvc_streaming_control probe;
	struct uvc_streaming_control commit;

	int control;

	struct uvc_control_context context;

	unsigned int fcc;
	unsigned int width;
	unsigned int height;
	unsigned int iframe;
	unsigned int finterval;

	void **mem;
	unsigned int nbufs;
	unsigned int bufsize;

	unsigned int bulk; /* bulk mode select */
	unsigned int bulk_stream_on;

	uint8_t color;
	unsigned int imgsize;
	void *imgdata;
	uint8_t imgsrc;
	uint8_t stream_status;

	unsigned int maxsize;
	uint8_t sformat;

	uvc_event_callback user_event_f;
	uvc_resolution_callback set_res_f;
};

typedef void (*uvc_setup_callback)(struct uvc_device *dev,
		struct uvc_request_data *r,
		uint8_t req, uint8_t cs);

typedef void (*uvc_data_callback)(struct uvc_device *dev,
		struct uvc_request_data *r, uint8_t cs);

struct uvc_cs_ops{
	uint8_t cs;
	uint8_t len;

	/*setup phase*/
	uvc_setup_callback setup_phase_f;	/*data: device to host */

	/*data phase*/
	uvc_data_callback data_phase_f;	/* data: host ot device */
};

struct uvc_ext_cs_ops{
	uint8_t cs;
	uint8_t len;

	/*setup phase*/
	uvc_setup_callback setup_phase_f;	/*data: device to host */

	/*data phase*/
	uvc_data_callback data_phase_f;	/* data: host ot device */

	uint8_t id;
	struct uvc_ext_cs_ops* next,*prev;
};

struct uvc_frame_info {
	unsigned int width;
	unsigned int height;
	unsigned int intervals[8];
};

struct uvc_format_info {
	unsigned int fcc;
	const struct uvc_frame_info *frames;
};

/* put INFOTM struct and function here for temporary */
/* ================================================= */
/* infotm utc time */
typedef struct _infotm_utc_time_t
{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
} infotm_utc_time_t;

/* infotm storage info */
typedef struct _infotm_storage_info_t
{
	uint32_t total;
	uint32_t free;
} infotm_storage_info_t;

/* infotm cmd enum */
typedef enum _infotm_cmd_e
{
	UVCX_INFOTM_CMD_TAKE_PIC			= 0x01,
	UVCX_INFOTM_CMD_START_VIDEO,
	UVCX_INFOTM_CMD_STOP_VIDEO,
	UVCX_INFOTM_CMD_VIDEO_LOOP,
	UVCX_INFOTM_CMD_GET_VIDEO_STATE,
	UVCX_INFOTM_CMD_SYNC_TIME,
	UVCX_INFOTM_CMD_GET_BATTERY_INFO,
	UVCX_INFOTM_CMD_GET_SD_INFO,
	UVCX_INFOTM_CMD_GET_VERSION,
	UVCX_INFOTM_CMD_SWITCH_TO_UMS,
	UVCX_INFOTM_CMD_SWITCH_TO_UVC,
	UVCX_INFOTM_CMD_DEVICE_MODE,
	UVCX_INFOTM_CMD_SD_STATE,
} infotm_cmd_e;

/* infotm data */
typedef struct _infotm_data_t
{
	uint8_t					bVideoLoopTime;		//unit second
	uint8_t					bVideoState;
	uint8_t					bDeviceMode;
	uint8_t					bIsSDMounted;
	uint16_t				wBatteryInfo;
	uint8_t					version[8];
	time_t					utc_seconds;
	infotm_storage_info_t 	storage_info;
} infotm_data_t;

/* infotm calibration data */
typedef struct _infotm_calib_data_t
{
	uint8_t					calib_data[64];
} infotm_calib_data;

/* infotm data */
typedef struct _infotm_ctrl_package_t
{
	infotm_cmd_e	cmd_id;
	infotm_data_t	data;
} infotm_ctrl_package_t;

/*----------------------------------------------------------------------------*/

/*guvc user api */
struct uvc_device * uvc_open(const char *devname, int bulk_mode);
void uvc_close(struct uvc_device *dev);
void uvc_exit(struct uvc_device *dev);

void uvc_events_process(struct uvc_device *dev);
int uvc_video_process(struct uvc_device *dev, void * vbuf, int size, void * hdbuf, int hdsize);

void uvc_get_cur_resolution(int* w, int *h, int *fps);

int uvc_cs_ops_register(int id, int cs,
	uvc_setup_callback setup_f, uvc_data_callback data_f, int data_len);

inline void uvc_event_callback_register(struct uvc_device *dev,
		uvc_event_callback event_f, uvc_resolution_callback set_res_f);

inline void uvc_set_def_img(struct uvc_device *dev, void *img, int size, int src);
inline void uvc_set_log_level(int level);
int uvc_server(char *channel);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __GUVC_LIB_H__ */

