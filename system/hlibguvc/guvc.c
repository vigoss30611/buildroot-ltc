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

#include <list.h>
#include <guvc.h>

static pthread_mutex_t uvc_list_mutex = PTHREAD_MUTEX_INITIALIZER;

static struct uvc_ext_cs_ops uvc_ext_ctrl_list;

struct uvc_cs_ops camera_terminal_cs_ops[] ={
	{UVC_CT_CONTROL_UNDEFINED, 0, 0, 0},
	{UVC_CT_SCANNING_MODE_CONTROL, sizeof(uint8_t) , 0, 0},
	{UVC_CT_AE_MODE_CONTROL, sizeof(union bAutoExposureMode) , 0, 0},

	{UVC_CT_AE_PRIORITY_CONTROL, sizeof(int8_t) , 0, 0},

	{UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL, sizeof(int32_t) , 0, 0},
	{UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL, sizeof(uint8_t) , 0, 0},

	{UVC_CT_FOCUS_ABSOLUTE_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_CT_FOCUS_RELATIVE_CONTROL, sizeof(struct FocusRelativeControl) , 0, 0},
	{UVC_CT_FOCUS_AUTO_CONTROL, sizeof(uint8_t) , 0, 0},
	{UVC_CT_IRIS_ABSOLUTE_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_CT_IRIS_RELATIVE_CONTROL, sizeof(int8_t) , 0, 0},

	{UVC_CT_ZOOM_ABSOLUTE_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_CT_ZOOM_RELATIVE_CONTROL, sizeof(struct ZoomRelativeControl) , 0, 0},

	{UVC_CT_PANTILT_ABSOLUTE_CONTROL, sizeof(struct PanTiltAbsoluteControl) , 0, 0},
	{UVC_CT_PANTILT_RELATIVE_CONTROL, sizeof(struct PanTiltRelativeControl) , 0, 0},
	{UVC_CT_ROLL_ABSOLUTE_CONTROL, sizeof(uint16_t) , 0, 0},
	{UVC_CT_ROLL_RELATIVE_CONTROL, sizeof(struct RollRelativeControl) , 0, 0},
	{UVC_CT_PRIVACY_CONTROL, sizeof(uint8_t) , 0, 0},
};

struct uvc_cs_ops processing_unit_cs_ops[] ={
	{UVC_PU_CONTROL_UNDEFINED, 0, 0, 0},
	{UVC_PU_BACKLIGHT_COMPENSATION_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_PU_BRIGHTNESS_CONTROL, sizeof(uint16_t) , 0, 0},
	{UVC_PU_CONTRAST_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_PU_GAIN_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_PU_POWER_LINE_FREQUENCY_CONTROL, sizeof(int8_t) , 0, 0},

	{UVC_PU_HUE_CONTROL, sizeof(uint16_t) , 0, 0},

	{UVC_PU_SATURATION_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_PU_SHARPNESS_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_PU_GAMMA_CONTROL, sizeof(int16_t) , 0, 0},

	{UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL, sizeof(int8_t) , 0, 0},
	{UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL, sizeof(struct WhiteBalanceComponentControl) , 0, 0},
	{UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL, sizeof(int8_t) , 0, 0},

	{UVC_PU_DIGITAL_MULTIPLIER_CONTROL, sizeof(int16_t) , 0, 0},
	{UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL, sizeof(int16_t) , 0, 0},

	{UVC_PU_HUE_AUTO_CONTROL, sizeof(int8_t) , 0, 0},
	{UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL, sizeof(int8_t) , 0, 0},
	{UVC_PU_ANALOG_LOCK_STATUS_CONTROL, sizeof(int8_t) , 0, 0},
};

#define UVC_CT_CS_OPS_NUM (sizeof(camera_terminal_cs_ops)/sizeof(camera_terminal_cs_ops[0]))
#define UVC_PU_CS_OPS_NUM (sizeof(processing_unit_cs_ops)/sizeof(processing_unit_cs_ops[0]))

/* 
	Frame Interval = 10000000/ fps
		60 : 166666
		30 : 333333
		15 : 666666
		10: 1000000
		2: 5000000
 */
static const struct uvc_frame_info uvc_frames_yuyv[] = {
	{  640, 360, { 666666, 1000000, 5000000, 0 }, }, /*Note: 360p */
	{ 1280, 720, { 5000000, 0 }, },	/*Note: 720p */
	{ 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames_mjpeg[] = {
	{ 640, 360, { 166666, 333333, 666666, 1000000, 5000000, 0 }, },
	{ 1280, 720, { 166666, 333333, 666666, 1000000, 5000000, 0 }, },
	{ 1920, 1080, { 166666, 333333, 666666, 1000000, 5000000, 0 }, },
#if 0
	{ 3840, 1080, { 333333, 666666, 1000000, 5000000, 0 }, },
	{ 768, 768, { 166666, 333333, 666666, 1000000, 5000000, 0 }, },
	{ 1088, 1088, { 166666, 333333, 666666, 1000000, 5000000, 0 }, },
	{ 1920, 1920, { 166666, 333333, 666666, 1000000, 5000000, 0 }, },
#endif
	{ 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames_h264[] = {
	{ 640, 480, { 166666, 333333, 666666, 1000000, 5000000, 0 }, }, // 1
	{ 960, 480, { 166666, 333333, 666666,1000000, 5000000, 0 }, }, // 2
	{ 1472, 736, { 166666, 333333, 666666,1000000, 5000000, 0 }, }, // 3
	{ 1920, 960, { 166666, 333333, 666666,1000000, 5000000, 0 }, }, // 4
	{ 0, 0, { 0, }, },
};

static const struct uvc_format_info uvc_formats[] = {
	{ V4L2_PIX_FMT_YUYV, uvc_frames_yuyv},
	{ V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg},
	{ V4L2_PIX_FMT_H264, uvc_frames_h264},
};

static struct uvc_device *g_dev = NULL;
/* ---------------------------------------------------------------------------
 * Video streaming
 */
void uvc_video_fill_buffer(struct uvc_device *dev, struct v4l2_buffer *buf)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s\n", __func__);

	if (!dev->imgdata) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s imgdata is null \n", __func__);
		return;
	}

	switch (dev->fcc) {
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_MJPEG:
	case V4L2_PIX_FMT_H264:
		memcpy(dev->mem[buf->index], dev->imgdata, dev->imgsize);
		buf->bytesused = dev->imgsize;

		UVC_PRINTF(UVC_LOG_DEBUG,"# %s imgdata is %p imgsize:%d bufsize:%d, max:%d\n",
			__func__, dev->imgdata, dev->imgsize, dev->bufsize, dev->maxsize);

		//memset(dev->mem[buf->index], 0, dev->bufsize);
		//buf->bytesused = dev->bufsize;
		break;
	default:
		break;
	}
	UVC_PRINTF(UVC_LOG_TRACE,"# %s => end\n", __func__);
}

int uvc_video_process_pic(struct uvc_device *dev)
{
	static int fno = 0;
	struct v4l2_buffer buf;
	int ret;

	if (dev->stream_status == UVC_STREAM_OFF)
		return -1;

	memset(&buf, 0, sizeof buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;

	if ((ret = ioctl(dev->fd, VIDIOC_DQBUF, &buf)) < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"#%s: Unable to dequeue buffer: %s (%d).\n", 
			__func__, strerror(errno), errno);
		return ret;
	}

	uvc_video_fill_buffer(dev, &buf);
	if ((ret = ioctl(dev->fd, VIDIOC_QBUF, &buf)) < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"#%s Unable to requeue buffer: %s (%d).\n",
			__func__, strerror(errno),errno);
		return ret;
	}
	fno++;
	return 0;
}

static int __uvc_video_process(struct uvc_device *dev, void * vbuf, int size, void * hdbuf, int hdsize)
{
	struct v4l2_buffer buf;
	int ret;

	if (dev->stream_status == UVC_STREAM_OFF)
		return -1;

	memset(&buf, 0, sizeof buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;

	if ((ret = ioctl(dev->fd, VIDIOC_DQBUF, &buf)) < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"#%s: Unable to dequeue buffer: %s (%d).\n", 
			__func__, strerror(errno), errno);
		return ret;
	}

	if (vbuf) {
		//TODO, MMAP to USERPTR
		if(hdbuf) {
			memcpy(dev->mem[buf.index], hdbuf, hdsize);
			memcpy(dev->mem[buf.index] + hdsize, vbuf, size);
			buf.bytesused = size + hdsize;
		} else {
			memcpy(dev->mem[buf.index], vbuf, size);
			buf.bytesused = size;
		}
	} else {
		uvc_video_fill_buffer(dev, &buf);
	}

	if ((ret = ioctl(dev->fd, VIDIOC_QBUF, &buf)) < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"#%s Unable to requeue buffer: %s (%d).\n",
			__func__, strerror(errno),errno);
		return ret;
	}
	return 0;
}

int uvc_video_process(struct uvc_device *dev, void * vbuf, int size, void * hdbuf, int hdsize)
{
	int ret = 0;
	if (size < dev->maxsize)
		ret = __uvc_video_process(dev, vbuf, size, hdbuf, hdsize);
	else {
		if (dev->maxsize > 0)
			UVC_PRINTF(UVC_LOG_WARRING,
					"# guvc WARINNING: "
					"V:%d KB > U:%u KB "
					"B:0x%x \n",
					size >> 10, dev->maxsize >> 10,
					(uint32_t)vbuf);
	}
	return ret;
}

int uvc_video_reqbufs(struct uvc_device *dev, int nbufs)
{
	struct v4l2_requestbuffers rb;
	struct v4l2_buffer buf;
	unsigned int i;
	int ret;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s\n", __func__);
	for (i = 0; i < dev->nbufs; ++i)
		munmap(dev->mem[i], dev->bufsize);

	free(dev->mem);
	dev->mem = 0;
	dev->nbufs = 0;

	if (!nbufs)
		return 0;

	memset(&rb, 0, sizeof rb);
	rb.count = nbufs;
	rb.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	rb.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(dev->fd, VIDIOC_REQBUFS, &rb);
	if (ret < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"# Unable to allocate buffers: %s (%d).\n",
			strerror(errno), errno);
		return ret;
	}

	UVC_PRINTF(UVC_LOG_DEBUG,"# %u buffers allocated.\n", rb.count);

	/* Map the buffers. */
	dev->mem = malloc(rb.count * sizeof dev->mem[0]);

	for (i = 0; i < rb.count; ++i) {
		memset(&buf, 0, sizeof buf);
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(dev->fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			UVC_PRINTF(UVC_LOG_ERR,"# Unable to query buffer %u: %s (%d).\n", i,
				strerror(errno), errno);
			return -1;
		}
		UVC_PRINTF(UVC_LOG_DEBUG,"# length: %u offset: %u\n",
			buf.length, buf.m.offset);

		dev->mem[i] = mmap(0, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, 
				dev->fd, buf.m.offset);
		if (dev->mem[i] == MAP_FAILED) {
			UVC_PRINTF(UVC_LOG_ERR,"# Unable to map buffer %u: %s (%d)\n", i,
				strerror(errno), errno);
			return -1;
		}
		UVC_PRINTF(UVC_LOG_DEBUG,"# Buffer %u mapped at address %p.\n", i,
			dev->mem[i]);
	}

	dev->bufsize = buf.length;
	dev->nbufs = rb.count;

	return 0;
}

int uvc_video_stream(struct uvc_device *dev, int enable)
{
	struct v4l2_buffer buf;
	unsigned int i;
	int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	int ret = 0;

	if (!enable) {
		UVC_PRINTF(UVC_LOG_ERR,"# Stopping video stream.\n");
		ioctl(dev->fd, VIDIOC_STREAMOFF, &type);
		return 0;
	}

	UVC_PRINTF(UVC_LOG_INFO,"# Starting video stream.\n");

	for (i = 0; i < dev->nbufs; ++i) {
		memset(&buf, 0, sizeof buf);

		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;

		uvc_video_fill_buffer(dev, &buf);

		UVC_PRINTF(UVC_LOG_INFO,"# Queueing buffer %u.\n", i);
		if ((ret = ioctl(dev->fd, VIDIOC_QBUF, &buf)) < 0) {
			UVC_PRINTF(UVC_LOG_DEBUG,"# Unable to queue buffer: %s (%d).\n",
				strerror(errno), errno);
			break;
		}
	}

	ioctl(dev->fd, VIDIOC_STREAMON, &type);
	return ret;
}

int uvc_video_set_format(struct uvc_device *dev)
{
	struct v4l2_format fmt;
	int ret;

	UVC_PRINTF(UVC_LOG_DEBUG,"# Setting format to 0x%08x %ux%u\n",
		dev->fcc, dev->width, dev->height);

	memset(&fmt, 0, sizeof fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	fmt.fmt.pix.width = dev->width;
	fmt.fmt.pix.height = dev->height;
	fmt.fmt.pix.pixelformat = dev->fcc;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	if (dev->fcc == V4L2_PIX_FMT_MJPEG || dev->fcc == V4L2_PIX_FMT_H264)
		fmt.fmt.pix.sizeimage = dev->maxsize ;

	if ((ret = ioctl(dev->fd, VIDIOC_S_FMT, &fmt)) < 0)
		UVC_PRINTF(UVC_LOG_ERR,"# Unable to set format: %s (%d).\n",
			strerror(errno), errno);

	return ret;
}

int uvc_video_init(struct uvc_device *dev __attribute__((__unused__)))
{
	return 0;
}

/* ---------------------------------------------------------------------------
 * Request processing
 */
void uvc_fill_streaming_control(struct uvc_device *dev,
			   struct uvc_streaming_control *ctrl,
			   int iframe, int iformat)
{
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	unsigned int nframes;

	if (iformat < 0)
		iformat = ARRAY_SIZE(uvc_formats) + iformat;
	if (iformat < 0 || iformat >= (int)ARRAY_SIZE(uvc_formats))
		return;
	format = &uvc_formats[iformat];

	nframes = 0;
	while (format->frames[nframes].width != 0)
		++nframes;

	if (iframe < 0)
		iframe = nframes + iframe;
	if (iframe < 0 || iframe >= (int)nframes)
		return;
	frame = &format->frames[iframe];

	memset(ctrl, 0, sizeof *ctrl);

	ctrl->bmHint = 1;
	ctrl->bFormatIndex = iformat + 1;
	ctrl->bFrameIndex = iframe + 1;
	ctrl->dwFrameInterval = frame->intervals[0];
	switch (format->fcc) {
	case V4L2_PIX_FMT_YUYV:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case V4L2_PIX_FMT_MJPEG:
		ctrl->dwMaxVideoFrameSize = dev->maxsize;//dev->imgsize;
		break;
	case V4L2_PIX_FMT_H264:
		ctrl->dwMaxVideoFrameSize = dev->maxsize;//dev->imgsize;
		break;
	default:
		break;
	}

	/* TODO this should be filled by the driver. */
	if (dev->bulk) {
		UVC_PRINTF(UVC_LOG_ERR,"# uvc bulk transfer mode\n");
		ctrl->dwMaxPayloadTransferSize = 1024;//16*1024;
	} else {
		UVC_PRINTF(UVC_LOG_ERR,"# uvc isoc transfer mode\n");
		ctrl->dwMaxPayloadTransferSize = 1024;
	}

	ctrl->bmFramingInfo = 3;
	ctrl->bPreferedVersion = 1;
	ctrl->bMaxVersion = 1;
}

void uvc_events_process_standard(struct uvc_device *dev, struct usb_ctrlrequest *ctrl,
			    struct uvc_request_data *resp)
{
	UVC_PRINTF(UVC_LOG_INFO,"# standard request\n");
	(void)dev;
	(void)ctrl;
	(void)resp;
}

int uvc_cs_ops_register(int id, int cs,
	uvc_setup_callback setup_f, uvc_data_callback data_f, int data_len)
{
	struct uvc_ext_cs_ops *ext_ops = NULL;
	if (!setup_f || !data_f)
		goto err;

	if (id == UVC_CTRL_CAMERAL_TERMINAL_ID) {
		if (cs > UVC_CT_CS_OPS_NUM -1)
			goto err;
		else {
			camera_terminal_cs_ops[cs].setup_phase_f= setup_f;
			camera_terminal_cs_ops[cs].data_phase_f= data_f;
		}
	} else if ( id == UVC_CTRL_PROCESSING_UNIT_ID) {
		if (cs > UVC_PU_CS_OPS_NUM -1)
			goto err;
		else {
			processing_unit_cs_ops[cs].setup_phase_f= setup_f;
			processing_unit_cs_ops[cs].data_phase_f= data_f;
		}
	} else if (id == UVC_CTRL_INFOTM_EXTENSION_UNIT_ID ||
			id == UVC_CTRL_H264_EXTENSION_UNIT_ID) {
		ext_ops = (struct uvc_ext_cs_ops *)malloc (sizeof(struct uvc_ext_cs_ops));
		if (!ext_ops) {
			UVC_PRINTF(UVC_LOG_ERR,"# [%s] malloc failed !", __func__);
			exit(-1);
		}

		ext_ops->cs = cs;
		ext_ops->id = id;
		ext_ops->len = data_len;
		ext_ops->setup_phase_f = setup_f;
		ext_ops->data_phase_f = data_f;

		pthread_mutex_lock(&uvc_list_mutex);
		list_add_tail(ext_ops, &uvc_ext_ctrl_list);
		pthread_mutex_unlock(&uvc_list_mutex);
	}
	return 0;
err:
	return -1;
}

static void* uvc_get_cs_ops(uint8_t cs, uint8_t id)
{
	struct uvc_ext_cs_ops* ext_ops = NULL;
	if (id == UVC_CTRL_CAMERAL_TERMINAL_ID) {
		if (cs > UVC_CT_CS_OPS_NUM -1)
			return NULL;
		else
			return &camera_terminal_cs_ops[cs];
	} else if ( id == UVC_CTRL_PROCESSING_UNIT_ID) {
		if (cs > UVC_PU_CS_OPS_NUM -1)
			return NULL;
		else
			return &processing_unit_cs_ops[cs];
	} else if (id == UVC_CTRL_INFOTM_EXTENSION_UNIT_ID ||
			id == UVC_CTRL_H264_EXTENSION_UNIT_ID) {
		pthread_mutex_lock(&uvc_list_mutex);
		list_for_each (ext_ops, uvc_ext_ctrl_list) {
			if (ext_ops->id == id && ext_ops->cs == cs) {
				pthread_mutex_unlock(&uvc_list_mutex);
				return ext_ops;
			}
		}
		pthread_mutex_unlock(&uvc_list_mutex);
	}
	return NULL;
}

void uvc_cs_dump(uint8_t cs, uint8_t id, uint8_t intf, const char* call)
{
	UVC_PRINTF(UVC_LOG_INFO,"# [%s]", call);
	if (intf == UVC_INTF_CONTROL) {
		UVC_PRINTF(UVC_LOG_INFO,"[UVC_INTF_CONTROL]");
		if (id == UVC_CTRL_CAMERAL_TERMINAL_ID) {
			switch(cs) {
				case UVC_CT_CONTROL_UNDEFINED:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_CONTROL_UNDEFINED]\n");
					break;
				case UVC_CT_SCANNING_MODE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_SCANNING_MODE]\n");
					break;
				case UVC_CT_AE_MODE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_AE_MODE]\n");
					break;
				case UVC_CT_AE_PRIORITY_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_AE_PRIORITY]\n");
					break;
				case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_EXPOSURE_TIME_ABSOLUTE]\n");
					break;
				case UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_EXPOSURE_TIME_RELATIVE]\n");
					break;
				case UVC_CT_FOCUS_ABSOLUTE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_FOCUS_ABSOLUTE]\n");
					break;
				case UVC_CT_FOCUS_RELATIVE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_FOCUS_RELATIVE]\n");
					break;
				case UVC_CT_FOCUS_AUTO_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_FOCUS_AUTO]\n");
					break;
				case UVC_CT_IRIS_ABSOLUTE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_IRIS_ABSOLUTE]\n");
					break;
				case UVC_CT_IRIS_RELATIVE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_IRIS_RELATIVE]\n");
					break;
				case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_ZOOM_ABSOLUTE]\n");
					break;
				case UVC_CT_ZOOM_RELATIVE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_ZOOM_RELATIVE]\n");
					break;
				case UVC_CT_PANTILT_ABSOLUTE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_PANTILT_ABSOLUTE]\n");
					break;
				case UVC_CT_PANTILT_RELATIVE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_PANTILT_RELATIVE]\n");
					break;
				case UVC_CT_ROLL_ABSOLUTE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_ROLL_ABSOLUTE]\n");
					break;
				case UVC_CT_ROLL_RELATIVE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_ROLL_RELATIVE]\n");
					break;
				case UVC_CT_PRIVACY_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_PRIVACY]\n");
					break;
				default:
					UVC_PRINTF(UVC_LOG_INFO,"[CT_UNKNOWN]\n");
					break;
			}
		}
		else if (id == UVC_CTRL_PROCESSING_UNIT_ID) {
			switch(cs) {
			case UVC_PU_CONTROL_UNDEFINED:
				UVC_PRINTF(UVC_LOG_INFO,"[UVC_PU_CONTROL_UNDEFINED]\n");
				break;
			case UVC_PU_BACKLIGHT_COMPENSATION_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_BACKLIGHT_COMPENSATION]\n");
				break;
			case UVC_PU_BRIGHTNESS_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_BRIGHTNESS]\n");
				break;
			case UVC_PU_CONTRAST_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_CONTRAST]\n");
				break;
			case UVC_PU_GAIN_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_GAIN]\n");
				break;
			case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_POWER_LINE_FREQUENCY]\n");
				break;
			case UVC_PU_HUE_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_HUE]\n");
				break;
			case UVC_PU_SATURATION_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_SATURATION]\n");
				break;
			case UVC_PU_SHARPNESS_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_SHARPNESS]\n");
				break;
			case UVC_PU_GAMMA_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_GAMMA]\n");
				break;
			case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_WHITE_BALANCE_TEMPERATURE]\n");
				break;
			case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_WHITE_BALANCE_TEMPERATURE_AUTO\n");
				break;
			case UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_WHITE_BALANCE_COMPONENT]\n");
				break;
			case UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_WHITE_BALANCE_COMPONENT_AUTO]\n");
				break;
			case UVC_PU_DIGITAL_MULTIPLIER_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_DIGITAL_MULTIPLIER]\n");
				break;
			case UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_DIGITAL_MULTIPLIER_LIMIT]\n");
				break;
			case UVC_PU_HUE_AUTO_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_HUE_AUTO]\n");
				break;
			case UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_ANALOG_VIDEO_STANDARD]\n");
				break;
			case UVC_PU_ANALOG_LOCK_STATUS_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_ANALOG_LOCK_STATUS]\n");
				break;
			default:
				UVC_PRINTF(UVC_LOG_INFO,"[PU_UNKNOWN]\n");
				break;
			}
		}
		else if (id == UVC_CTRL_OUTPUT_TERMINAL_ID){
			UVC_PRINTF(UVC_LOG_INFO,"[OUTPUT_TERMINAL_ID]\n");
		} else if (id == UVC_CTRL_INTERFACE_ID) {
			switch(cs){
				case UVC_VC_CONTROL_UNDEFINED:
					UVC_PRINTF(UVC_LOG_INFO,"[VC_CONTROL_UNDEFINED]\n");
					break;
				case UVC_VC_VIDEO_POWER_MODE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[VC_VIDEO_POWER_MODE]\n");
					break;
				case UVC_VC_REQUEST_ERROR_CODE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[VC_REQUEST_ERROR_CODE]\n");
					break;
				default:
					UVC_PRINTF(UVC_LOG_INFO,"[VC_UNKNOWN]\n");
					break;
			}
		} else if (id == UVC_CTRL_H264_EXTENSION_UNIT_ID) {
			switch(cs) {
				case UVCX_VIDEO_CONFIG_PROBE:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_VIDEO_CONFIG_PROBE]\n");
					break;
				case UVCX_VIDEO_CONFIG_COMMIT:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_VIDEO_CONFIG_COMMIT]\n");
					break;
				case UVCX_RATE_CONTROL_MODE:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_RATE_CONTROL_MODE]\n");
					break;
				case UVCX_TEMPORAL_SCALE_MODE:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_TEMPORAL_SCALE_MODE]\n");
					break;
				case UVCX_SPATIAL_SCALE_MODE:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_SPATIAL_SCALE_MODE]\n");
					break;
				case UVCX_SNR_SCALE_MODE:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_SNR_SCALE_MODE]\n");
					break;
				case UVCX_LTR_BUFFER_SIZE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_LTR_BUFFER_SIZE_CONTROL]\n");
					break;
				case UVCX_LTR_PICTURE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_LTR_PICTURE_CONTROL]\n");
					break;
				case UVCX_PICTURE_TYPE_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_PICTURE_TYPE_CONTROL]\n");
					break;
				case UVCX_VERSION:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_VERSION]\n");
					break;
				case UVCX_ENCODER_RESET:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_ENCODER_RESET]\n");
					break;
				case UVCX_FRAMERATE_CONFIG:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_FRAMERATE_CONFIG]\n");
					break;
				case UVCX_VIDEO_ADVANCE_CONFIG:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_VIDEO_ADVANCE_CONFIG]\n");
					break;
				case UVCX_BITRATE_LAYERS:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_BITRATE_LAYERS]\n");
					break;
				case UVCX_QP_STEPS_LAYERS:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_QP_STEPS_LAYERS]\n");
					break;
					//put here for temporary
				case UVC_INFOTM_EU_CONTROL:
					UVC_PRINTF(UVC_LOG_INFO,"[UVC_INFOTM_EU_CONTROL]\n");
					break;
				case UVC_INFOTM_EU_CONTROL_DATA:
					UVC_PRINTF(UVC_LOG_INFO,"[UVC_INFOTM_EU_CONTROL_DATA]\n");
					break;
				default:
					UVC_PRINTF(UVC_LOG_INFO,"[UVCX_UNKNOWN]\n");
					break;
			}
		} else {
			UVC_PRINTF(UVC_LOG_INFO,"[ID_UNKNOWN] id: %d, cs: %d\n", id, cs);
		}
	} 
	else if (intf  == UVC_INTF_STREAMING){
		UVC_PRINTF(UVC_LOG_INFO,"[UVC_INTF_STREAMING]");
		if (id == UVC_CTRL_INTERFACE_ID) {
			switch(cs){
			case UVC_VS_CONTROL_UNDEFINED:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_CONTROL_UNDEFINED]\n");
				break;
			case UVC_VS_PROBE_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_PROBE]\n");
				break;
			case UVC_VS_COMMIT_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_COMMIT]\n");
				break;
			case UVC_VS_STILL_PROBE_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_STILL_PROBE]\n");
				break;
			case UVC_VS_STILL_COMMIT_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_STILL_COMMIT]\n");
				break;
			case UVC_VS_STILL_IMAGE_TRIGGER_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_STILL_IMAGE_TRIGGER]\n");
				break;
			case UVC_VS_STREAM_ERROR_CODE_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_STREAM_ERROR_CODE]\n");
				break;
			case UVC_VS_GENERATE_KEY_FRAME_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_GENERATE_KEY_FRAME]\n");
				break;
			case UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_UPDATE_FRAME_SEGMENT]\n");
				break;
			case UVC_VS_SYNC_DELAY_CONTROL:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_SYNC_DELAY]\n");
				break;
			default:
				UVC_PRINTF(UVC_LOG_INFO,"[VS_UNKNOWN]\n");
				break;
			}
		} else {
			UVC_PRINTF(UVC_LOG_INFO,"[ID_UNKNOWN %d]\n", id);
		}
	} else {
		UVC_PRINTF(UVC_LOG_INFO,"[INTF_UNKNOWN %d]\n", intf);
	}
}

static inline void
uvc_set_control_context(struct uvc_device *dev, uint8_t cs, uint8_t id, uint8_t intf)
{
	dev->context.cs = cs;
	dev->context.id = id;
	dev->context.intf = intf;
}

static inline void
uvc_get_control_context(struct uvc_device *dev, uint8_t *cs, uint8_t *id, uint8_t *intf)
{
	*cs = dev->context.cs;
	*id = dev->context.id;
	*intf = dev->context.intf;
}

static void uvc_ct_setup_phase(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp)
{
	uint16_t len = 0;
	struct uvc_cs_ops *ops = (struct uvc_cs_ops *)uvc_get_cs_ops(cs, 
			UVC_CTRL_CAMERAL_TERMINAL_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_DEBUG,"# %s (req %02x cs %02x)\n", __func__, req, cs);
	resp->length = ops->len;

	switch (req) {
	case UVC_SET_CUR:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_SET_CUR\n", __func__);
		uvc_set_control_context(dev, cs, UVC_CTRL_CAMERAL_TERMINAL_ID,
			UVC_INTF_CONTROL);
		break;

	case UVC_GET_LEN:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET_LEN len = %d \n", __func__, ops->len);
		len = ops->len;
		memcpy(resp->data, &len, sizeof(len));
		resp->length = sizeof(len);
		break;

	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
	case UVC_GET_INFO:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET(%d)\n", __func__, req);
		if(ops->setup_phase_f)
			(*ops->setup_phase_f)(dev, resp, req, cs);
		break;
	default:
		UVC_PRINTF(UVC_LOG_ERR,"# %s error: no-valid request(0x%x)\n", __func__, req);
		break;
	}
}

static void uvc_pu_setup_phase(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp)
{
	uint16_t len = 0;
	struct uvc_cs_ops *ops = (struct uvc_cs_ops *)uvc_get_cs_ops(cs, 
			UVC_CTRL_PROCESSING_UNIT_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_DEBUG,"# %s (req %02x cs %02x)\n", __func__, req, cs);
	resp->length = ops->len;

	switch (req) {
	case UVC_SET_CUR:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_SET_CUR\n", __func__);
		uvc_set_control_context(dev, cs, UVC_CTRL_PROCESSING_UNIT_ID,
			UVC_INTF_CONTROL);
		break;

	case UVC_GET_LEN:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET_LEN len = %d \n", __func__, ops->len);
		len = ops->len;
		memcpy(resp->data, &len, sizeof(len));
		resp->length = sizeof(len);
		break;

	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
	case UVC_GET_INFO:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET(%d)\n", __func__, req);
		if(ops->setup_phase_f)
			(*ops->setup_phase_f)(dev, resp, req, cs);
		break;
	default:
		UVC_PRINTF(UVC_LOG_ERR,"# %s error: no-valid request(0x%x)\n", __func__, req);
		break;
	}
}

static void uvc_eu_setup_phase(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp)
{
	uint16_t len = 0;
	struct uvc_ext_cs_ops *ops = (struct uvc_ext_cs_ops *)uvc_get_cs_ops(cs, 
			UVC_CTRL_INFOTM_EXTENSION_UNIT_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_DEBUG,"# %s (req %02x cs %02x)\n", __func__, req, cs);
	resp->length = ops->len;

	switch (req) {
	case UVC_SET_CUR:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_SET_CUR\n", __func__);
		uvc_set_control_context(dev, cs, UVC_CTRL_INFOTM_EXTENSION_UNIT_ID,
			UVC_INTF_CONTROL);
		break;

	case UVC_GET_LEN:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET_LEN len = %d \n", __func__, ops->len);
		len = ops->len;
		memcpy(resp->data, &len, sizeof(len));
		resp->length = sizeof(len);
		break;

	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
	case UVC_GET_INFO:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET(%d)\n", __func__, req);
		if(ops->setup_phase_f)
			(*ops->setup_phase_f)(dev, resp, req, cs);
		break;
	default:
		UVC_PRINTF(UVC_LOG_ERR,"# %s error: no-valid request(0x%x)\n", __func__, req);
		break;
	}
}

static void uvc_eu_h264_setup_phase(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp)
{
	uint16_t len = 0;
	struct uvc_ext_cs_ops *ops = (struct uvc_ext_cs_ops *)uvc_get_cs_ops(cs,
			UVC_CTRL_H264_EXTENSION_UNIT_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_DEBUG,"# %s (req %02x cs %02x)\n", __func__, req, cs);
	resp->length = ops->len;

	switch (req) {
	case UVC_SET_CUR:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_SET_CUR\n", __func__);
		uvc_set_control_context(dev, cs, UVC_CTRL_H264_EXTENSION_UNIT_ID,
			UVC_INTF_CONTROL);
		break;

	case UVC_GET_LEN:
		len = ops->len;
		memcpy(resp->data, &len, sizeof(len));
		resp->length = sizeof(len);
		break;

	case UVC_GET_CUR:
	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
	case UVC_GET_RES:
	case UVC_GET_INFO:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET(%d)\n", __func__, req);
		if(ops->setup_phase_f)
			(*ops->setup_phase_f)(dev, resp, req, cs);
		break;
	}
}

static void
uvc_camera_terminal_control(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp)
{
	uvc_ct_setup_phase(dev, req, cs, resp);
}

static void
uvc_processing_unit_control(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp) 
{
	uvc_pu_setup_phase(dev, req, cs, resp);
}

static void
uvc_extension_unit_control(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp) 
{
	uvc_eu_setup_phase(dev, req, cs, resp);
}


static void
uvc_interface_control(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp)
{
	(void)dev;
	(void)req;
	(void)cs;
	(void)resp;
}

static void
uvc_output_terminal_control(struct uvc_device *dev, uint8_t req, uint8_t cs,
			   struct uvc_request_data *resp)
{
	(void)dev;
	(void)req;
	(void)cs;
	(void)resp;
}

static void
uvc_events_process_control(struct uvc_device *dev, uint8_t req, uint8_t cs, uint8_t id,
			   struct uvc_request_data *resp)
{
	UVC_PRINTF(UVC_LOG_TRACE,"#%s control request (req %02x cs %02x)\n", __func__, req, cs);

	if (id > dev->bulk_stream_on)
		dev->bulk_stream_on = id;

	switch (id) {
	case UVC_CTRL_INTERFACE_ID:
		uvc_interface_control(dev, req, cs, resp);
		break;
	case UVC_CTRL_CAMERAL_TERMINAL_ID:
		uvc_camera_terminal_control(dev, req, cs, resp);
		break;
	case UVC_CTRL_PROCESSING_UNIT_ID:
		uvc_processing_unit_control(dev, req, cs, resp);
		break;
//	case UVC_CTRL_H264_EXTENSION_UNIT_ID:
//		uvc_eu_h264_setup_phase(dev, req, cs, resp);
//		break;
	case UVC_CTRL_INFOTM_EXTENSION_UNIT_ID:
		uvc_extension_unit_control(dev, req, cs, resp);
		break;
	case UVC_CTRL_OUTPUT_TERMINAL_ID:
		uvc_output_terminal_control(dev, req, cs, resp);
		break;
	default:
		UVC_PRINTF(UVC_LOG_ERR,"# %s can not support termianl or UNIT id %d\n", __func__, id);
		break;
	}
}

static void
uvc_events_process_streaming(struct uvc_device *dev, uint8_t req, uint8_t cs,
			     struct uvc_request_data *resp)
{
	struct uvc_streaming_control *ctrl;

	UVC_PRINTF(UVC_LOG_TRACE,"#%s streaming request (req %02x cs %02x)\n", __func__, req, cs);

	if (cs != UVC_VS_PROBE_CONTROL && cs != UVC_VS_COMMIT_CONTROL)
		return;

	ctrl = (struct uvc_streaming_control *)&resp->data;
	resp->length = sizeof *ctrl;

	switch (req) {
	case UVC_SET_CUR:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s  UVC_SET_CUR\n", __func__);
		uvc_set_control_context(dev, cs,UVC_CTRL_INTERFACE_ID ,
			UVC_INTF_STREAMING);
		resp->length = 34;
		break;

	case UVC_GET_CUR:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET_CUR\n", __func__);
		if (cs == UVC_VS_PROBE_CONTROL)
			memcpy(ctrl, &dev->probe, sizeof *ctrl);
		else
			memcpy(ctrl, &dev->commit, sizeof *ctrl);
		break;

	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET_MIN | UVC_GET_MAX |"
			"UVC_GET_DEF\n", __func__);
		uvc_fill_streaming_control(dev, ctrl,
					req == UVC_GET_MAX ? -1 : 0, /* iframe */
					req == UVC_GET_MAX ? -1 : 0); /* iformat */
		break;

	case UVC_GET_RES:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET_RES\n", __func__);
		memset(ctrl, 0, sizeof *ctrl);
		break;

	case UVC_GET_LEN:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s  UVC_GET_LEN\n", __func__);
		resp->data[0] = 0x00;
		resp->data[1] = 0x22;
		resp->length = 2;
		break;

	case UVC_GET_INFO:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s UVC_GET_INFO\n", __func__);
		resp->data[0] = 0x03;
		resp->length = 1;
		break;
	}
}

static void
uvc_events_process_class(struct uvc_device *dev, struct usb_ctrlrequest *ctrl,
			 struct uvc_request_data *resp)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s\n", __func__);
	if ((ctrl->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE)
		return;

	uvc_cs_dump(ctrl->wValue >> 8,
		ctrl->wIndex >> 8,
		ctrl->wIndex & 0xff, __func__);

	switch (ctrl->wIndex & 0xff) {
	case UVC_INTF_CONTROL:
		uvc_events_process_control(dev, ctrl->bRequest, ctrl->wValue >> 8,
				ctrl->wIndex >> 8, resp);
		break;

	case UVC_INTF_STREAMING:
		uvc_events_process_streaming(dev, ctrl->bRequest, ctrl->wValue >> 8, resp);
		break;

	default:
		UVC_PRINTF(UVC_LOG_ERR,"# %s intf %d is no valid\n", __func__, ctrl->wIndex & 0xff);
		break;
	}
}

/* setup transaction*/
static void
uvc_events_process_setup(struct uvc_device *dev, struct usb_ctrlrequest *ctrl,
			 struct uvc_request_data *resp)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# bRequestType %02x bRequest %02x wValue %04x wIndex %04x "
		"wLength %04x\n", ctrl->bRequestType, ctrl->bRequest,
		ctrl->wValue, ctrl->wIndex, ctrl->wLength);

	switch (ctrl->bRequestType & USB_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		uvc_events_process_standard(dev, ctrl, resp);
		break;

	case USB_TYPE_CLASS:
		uvc_events_process_class(dev, ctrl, resp);
		break;

	case USB_TYPE_VENDOR:
		//TODO, for Infotm vendor
		break;

	default:
		break;
	}
}

static void uvc_ct_data_phase(struct uvc_device *dev, uint8_t cs,
			   struct uvc_request_data *d)
{
	struct uvc_cs_ops *ops = (struct uvc_cs_ops *)uvc_get_cs_ops(cs,
			UVC_CTRL_CAMERAL_TERMINAL_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (cs %02x)\n", __func__, cs);
	if (d->length != ops->len) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s (cs %02x) data length %d is no valid\n",
			__func__, cs, d->length);
		return ;
	}

	if(ops->data_phase_f)
		(*ops->data_phase_f)(dev, d, cs);
}

static void uvc_pu_data_phase(struct uvc_device *dev, uint8_t cs,
			   struct uvc_request_data *d)
{
	struct uvc_cs_ops *ops = (struct uvc_cs_ops *)uvc_get_cs_ops(cs,
			UVC_CTRL_PROCESSING_UNIT_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (cs %02x)\n", __func__, cs);
	if (d->length != ops->len) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s (cs %02x) data length %d is no valid\n", 
			__func__, cs, d->length);
		return ;
	}

	if(ops->data_phase_f)
		(*ops->data_phase_f)(dev, d, cs);
}

static void uvc_eu_data_phase(struct uvc_device *dev, uint8_t cs,
			   struct uvc_request_data *d)
{
	struct uvc_ext_cs_ops *ops = (struct uvc_ext_cs_ops *)uvc_get_cs_ops(cs,
			UVC_CTRL_INFOTM_EXTENSION_UNIT_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (cs %02x)\n", __func__, cs);
	/*Note: The getting data of length is handle by upper level and not here */
#if 0
	if (d->length != ops->len) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s (cs %02x) data length %d is no valid\n", 
			__func__, cs, d->length);
		return ;
	}
#endif
	if(ops->data_phase_f)
		(*ops->data_phase_f)(dev, d, cs);
	else
		UVC_PRINTF(UVC_LOG_ERR,"# %s (cs %02x) data_phase_f is null\n", __func__, cs);
}

static void uvc_eu_h264_data_phase(struct uvc_device *dev, uint8_t cs,
			   struct uvc_request_data *d)
{
	struct uvc_ext_cs_ops *ops = (struct uvc_ext_cs_ops *)uvc_get_cs_ops(cs,
			UVC_CTRL_H264_EXTENSION_UNIT_ID);
	if (!ops) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s get cs[%d] ops failed!\n", __func__, cs);
		return ;
	}

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (cs %02x)\n", __func__, cs);
	/*Note: The getting data of length is handle by upper level and not here */
#if 0
	if (d->length != ops->len) {
		UVC_PRINTF(UVC_LOG_ERR,"# %s (cs %02x) data length %d is no valid\n",
			__func__, cs, d->length);
		return ;
	}
#endif
	if(ops->data_phase_f)
		(*ops->data_phase_f)(dev, d, cs);
}

static void 
uvc_interface_data(struct uvc_device *dev, uint8_t cs,
	struct uvc_request_data *data)
{
	(void)dev;
	(void)cs;
	(void)data;
}

static void 
uvc_camera_terminal_data(struct uvc_device *dev, uint8_t cs,
	struct uvc_request_data *data)
{
	uvc_ct_data_phase(dev, cs,data);
}

static void 
uvc_processing_unit_data(struct uvc_device *dev, uint8_t cs,
	struct uvc_request_data *data)
{
	uvc_pu_data_phase(dev, cs,data);
}

static void 
uvc_extension_unit_data(struct uvc_device *dev, uint8_t cs,
	struct uvc_request_data *data)
{
	uvc_eu_data_phase(dev, cs,data);
}

static void 
uvc_output_terminal_data(struct uvc_device *dev, uint8_t cs,struct uvc_request_data *data)
{
	(void)dev;
	(void)cs;
	(void)data;
}

static void
uvc_events_process_vc_data(struct uvc_device *dev, struct uvc_request_data *data)
{
	int8_t cs;
	int8_t id;
	int8_t intf;

	uvc_get_control_context(dev, &cs, &id, &intf);
	(void)intf;

	UVC_PRINTF(UVC_LOG_TRACE,"# %s (id:%02x cs %02x)\n", 
		__func__, id, cs);

	switch (id) {
	case UVC_CTRL_INTERFACE_ID:
		uvc_interface_data(dev, cs, data);
		break;
	case UVC_CTRL_CAMERAL_TERMINAL_ID:
		uvc_camera_terminal_data(dev, cs, data);
		break;
	case UVC_CTRL_PROCESSING_UNIT_ID:
		uvc_processing_unit_data(dev, cs, data);
		break;
	case UVC_CTRL_INFOTM_EXTENSION_UNIT_ID:
		uvc_extension_unit_data(dev, cs, data);
		break;
	//case UVC_CTRL_H264_EXTENSION_UNIT_ID:
	//	uvc_eu_h264_data_phase(dev, cs, data);
	//	break;
	case UVC_CTRL_OUTPUT_TERMINAL_ID:
		uvc_output_terminal_data(dev, cs, data);
		break;
	default:
		UVC_PRINTF(UVC_LOG_ERR,"# %s can not support termianl or UNIT id %d\n",
				__func__, id);
		break;
	}
}

static void uvc_v4l2_buf_size_init(struct uvc_device *dev, unsigned int iframe)
{
#if 0
	switch(iframe) {
		case UVC_RESOLUTION_360P:
			dev->maxsize  = UVC_V4L2_BUF_SIZE_360P;
			break;

		case UVC_RESOLUTION_720P:
			dev->maxsize = UVC_V4L2_BUF_SIZE_720P;
			break;

		case UVC_RESOLUTION_1080P:
			dev->maxsize = UVC_V4L2_BUF_SIZE_1080P;
			break;

		case UVC_RESOLUTION_3840x1080:
			dev->maxsize =UVC_V4L2_BUF_SIZE_3840x1080;
			break;

		case UVC_RESOLUTION_768x768:
			dev->maxsize =UVC_V4L2_BUF_SIZE_768x768;
			break;

		case UVC_RESOLUTION_1088x1088:
			dev->maxsize =UVC_V4L2_BUF_SIZE_1088x1088;
			break;

		case UVC_RESOLUTION_1920x1920:
			dev->maxsize =UVC_V4L2_BUF_SIZE_1920x1920;
			break;

		default:
			dev->maxsize = UVC_V4L2_BUF_SIZE_720P;
	}
#else
	dev->maxsize = UVC_V4L2_BUF_SIZE_DEF;
#endif
}

/*data transaction*/
void uvc_events_process_vs_data(struct uvc_device *dev, 
	struct uvc_request_data *data)
{
	UVC_PRINTF(UVC_LOG_TRACE, "#%s \n",__func__);
	struct uvc_streaming_control *target;
	struct uvc_streaming_control *ctrl;
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	const unsigned int *interval;
	unsigned int iformat, iframe;
	unsigned int nframes;
	unsigned int fps;

	switch(dev->context.cs) {
	case UVC_VS_PROBE_CONTROL:
		UVC_PRINTF(UVC_LOG_INFO,"# setting probe control, length = %d\n", data->length);
		target = &dev->probe;
		break;

	case UVC_VS_COMMIT_CONTROL:
		UVC_PRINTF(UVC_LOG_ERR,"# setting commit control, length = %d\n", data->length);
		target = &dev->commit;
		break;

	default:
		UVC_PRINTF(UVC_LOG_DEBUG,"# setting unknown control, length = %d\n", data->length);
		return;
	}

	ctrl = (struct uvc_streaming_control *)&data->data;
	iformat = clamp((unsigned int)ctrl->bFormatIndex, 1U,
			(unsigned int)ARRAY_SIZE(uvc_formats));
	format = &uvc_formats[iformat-1];

	nframes = 0;
	while (format->frames[nframes].width != 0)
		++nframes;

	iframe = clamp((unsigned int)ctrl->bFrameIndex, 1U, nframes);
	frame = &format->frames[iframe-1];
	interval = frame->intervals;

	while (interval[0] < ctrl->dwFrameInterval && interval[1])
		++interval;
	UVC_PRINTF(UVC_LOG_TRACE, "#%s dwFrameInterval=%d\n",__func__,
			ctrl->dwFrameInterval);

	target->bFormatIndex = iformat;
	target->bFrameIndex = iframe;
	switch (format->fcc) {
	case V4L2_PIX_FMT_YUYV:
		target->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case V4L2_PIX_FMT_MJPEG:
		uvc_v4l2_buf_size_init(dev, iframe-1); /*v4l2 buffer size init*/

		if (dev->imgsize == 0)
			UVC_PRINTF(UVC_LOG_WARRING,"# WARNING: MJPEG requested and "
					"no image loaded.\n");
		target->dwMaxVideoFrameSize = frame->width * frame->height;
		break;
	case V4L2_PIX_FMT_H264:
		//uvc_v4l2_buf_size_init(dev, iframe-1); /*v4l2 buffer size init*/
		dev->maxsize = 1024 * 1024;//FIXME
		target->dwMaxVideoFrameSize = frame->width * frame->height;
		break;
	}
	target->dwFrameInterval = *interval;

	if (dev->context.cs == UVC_VS_COMMIT_CONTROL) {
		dev->fcc = format->fcc;
		dev->width = frame->width;
		dev->height = frame->height;
		dev->iframe = iframe-1; /* save current resolution */
		dev->finterval = *interval;
		UVC_PRINTF(UVC_LOG_INFO,"# commit control => set_format\n");
		uvc_video_set_format(dev);

		if (dev->set_res_f) {
			if (dev->finterval) {
				fps = 10000000/dev->finterval;
			} else {
				fps = 30;
			}
			(*dev->set_res_f)(dev, dev->width, dev->height, fps);
		}

		if (dev->bulk && dev->bulk_stream_on >= 2 &&
			dev->stream_status != UVC_STREAM_ON) {
			UVC_PRINTF(UVC_LOG_INFO, "# bulk stream_on status:%d\n",
				dev->bulk_stream_on);

			dev->stream_status = UVC_STREAM_ON;
			uvc_video_reqbufs(dev, 4);
			uvc_video_stream(dev, 1);
			dev->bulk_stream_on = 0;
		}
	}
}

static void
uvc_events_process_data(struct uvc_device *dev, struct uvc_request_data *data)
{
	uvc_cs_dump(dev->context.cs,
		dev->context.id,
		dev->context.intf,__func__);

	switch (dev->context.intf) {
	case UVC_INTF_CONTROL:
		uvc_events_process_vc_data(dev, data);
		break;
	case UVC_INTF_STREAMING:
		uvc_events_process_vs_data(dev, data);
		break;
	default:
		UVC_PRINTF(UVC_LOG_ERR, "#%s interface %d is not exist\n",
			__func__, dev->context.intf);
		break;
	}
}

void uvc_events_process(struct uvc_device *dev)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s\n", __func__);
	struct v4l2_event v4l2_event;
	struct uvc_event *uvc_event = (void *)&v4l2_event.u.data;
	struct uvc_request_data resp;
	int ret;

	ret = ioctl(dev->fd, VIDIOC_DQEVENT, &v4l2_event);
	if (ret < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"# VIDIOC_DQEVENT failed: %s (%d)\n", strerror(errno),
			errno);
		return;
	}

	if (dev->user_event_f)
		(*dev->user_event_f)(dev, v4l2_event.type);

	memset(&resp, 0, sizeof resp);
	//resp.length = -EL2HLT;
	resp.length = 0; /* all control request don't stall usb interface*/

	switch (v4l2_event.type) {
	case UVC_EVENT_CONNECT:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s => connect \n", __func__);
		return;

	case UVC_EVENT_DISCONNECT:
		if(dev->stream_status != UVC_STREAM_OFF) {
			UVC_PRINTF(UVC_LOG_INFO,"# %s => disconnect %d\n", __func__, dev->stream_status);
			dev->stream_status = UVC_STREAM_OFF;
			uvc_video_stream(dev, 0);
			uvc_video_reqbufs(dev, 0);
		}
		break;

	case UVC_EVENT_SETUP:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s => setup phase\n", __func__);
		uvc_events_process_setup(dev, &uvc_event->req, &resp);
		break;

	case UVC_EVENT_DATA:
		UVC_PRINTF(UVC_LOG_DEBUG,"# %s => data phase\n", __func__);
		uvc_events_process_data(dev, &uvc_event->data);
		return;

	case UVC_EVENT_STREAMON:
		UVC_PRINTF(UVC_LOG_INFO,"# %s => stream on\n", __func__);
		if(dev->stream_status != UVC_STREAM_ON) {
			if (!dev->bulk) { /* isoc transfer  */
				dev->stream_status = UVC_STREAM_ON;
				uvc_video_reqbufs(dev, 4);
				uvc_video_stream(dev, 1);
			}
		}
		break;

	case UVC_EVENT_STREAMOFF:
		UVC_PRINTF(UVC_LOG_INFO,"%s => stream off\n", __func__);
		if (dev->stream_status != UVC_STREAM_OFF){
			dev->stream_status = UVC_STREAM_OFF;
			uvc_video_stream(dev, 0);
			uvc_video_reqbufs(dev, 0);
			if (dev->bulk) {/* bulk transfer mode*/
				dev->bulk_stream_on = 0;
				UVC_PRINTF(UVC_LOG_INFO,
					"#  stream off=> bulk_stream_on=%d\n", dev->bulk_stream_on);
			}
		}
		break;
	default:
		break;
	}

	ioctl(dev->fd, UVCIOC_SEND_RESPONSE, &resp);
	if (ret < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"# UVCIOC_S_EVENT failed: %s (%d)\n", strerror(errno),
			errno);
		return;
	}
}

static void uvc_events_init(struct uvc_device *dev)
{
	UVC_PRINTF(UVC_LOG_TRACE,"# %s =>\n", __func__);
	struct v4l2_event_subscription sub;

	/*
	if(!dev->sformat) {
		uvc_fill_streaming_control(dev, &dev->probe, 
			UVC_RESOLUTION_360P, UVC_FORMAT_YUYV);
		uvc_fill_streaming_control(dev, &dev->commit, 
			UVC_RESOLUTION_360P, UVC_FORMAT_YUYV);
	} else {
		uvc_fill_streaming_control(dev, &dev->probe, 
			UVC_RESOLUTION_360P, UVC_FORMAT_MJPEG);
		uvc_fill_streaming_control(dev, &dev->commit, 
			UVC_RESOLUTION_360P, UVC_FORMAT_MJPEG);
	}*/
	uvc_fill_streaming_control(dev, &dev->probe,
		UVC_RESOLUTION_H264_1920x960, UVC_FORMAT_H264);
	uvc_fill_streaming_control(dev, &dev->commit,
		UVC_RESOLUTION_H264_1920x960, UVC_FORMAT_H264);

	if (dev->bulk) {
		/* FIXME Crude hack, must be negotiated with the driver. */
		dev->probe.dwMaxPayloadTransferSize = 1024;//16 * 1024;
		dev->commit.dwMaxPayloadTransferSize = 1024;//16 * 1024;
	}

	memset(&sub, 0, sizeof sub);
	sub.type = UVC_EVENT_SETUP;
	ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);

	sub.type = UVC_EVENT_DATA;
	ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);

	sub.type = UVC_EVENT_STREAMON;
	ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);

	sub.type = UVC_EVENT_STREAMOFF;
	ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);

#if 0
	sub.type = UVC_EVENT_CONNECT;
	ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
#endif

	sub.type = UVC_EVENT_DISCONNECT;
	ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);

	UVC_PRINTF(UVC_LOG_TRACE,"# %s <=\n", __func__);
}

void uvc_exit(struct uvc_device *dev)
{
	if (dev != NULL) {
		if (dev->bulk)
			dev->bulk_stream_on = 0;
		uvc_video_stream(dev, 0);
		uvc_video_reqbufs(dev, 0);
		uvc_close(dev);
	}
}

void uvc_get_cur_resolution(int* w, int *h, int *fps)
{
	if (!w || !h || !fps || !g_dev)
		return;

	switch(g_dev->iframe) {
	case UVC_RESOLUTION_360P:
		*w = 640;
		*h = 360;
		break;

	case UVC_RESOLUTION_720P:
		*w = 1280;
		*h = 720;
		break;

	case UVC_RESOLUTION_1080P:
		*w = 1920;
		*h = 1080;
		break;

	case UVC_RESOLUTION_3840x1080:
		*w = 3840;
		*h = 1080;
		break;

	case UVC_RESOLUTION_768x768:
		*w = *h = 768;
		break;

	case UVC_RESOLUTION_1088x1088:
		*w = *h = 1088;
		break;

	case UVC_RESOLUTION_1920x1920:
		*w = *h = 1920;
		break;

	default:
		*w = 0;
		*h = 0;
		break;
	}

	if (g_dev->finterval) {
		*fps = 10000000/g_dev->finterval;
		UVC_PRINTF(UVC_LOG_INFO,"# %s fps=%d interval=%d\n",
			__func__, *fps, g_dev->finterval);
	}
	else
		*fps  = 30;
}

inline void uvc_set_def_img(struct uvc_device *dev, void *img,
		int size, int src)
{
	dev->imgdata = img;
	dev->imgsize = size;
	dev->imgsrc = src;
}

inline void uvc_set_log_level(int level)
{
	g_log_level = level;
}

inline void uvc_event_callback_register(struct uvc_device *dev,
		uvc_event_callback event_f, uvc_resolution_callback set_res_f)
{
	dev->user_event_f = event_f;
	dev->set_res_f = set_res_f;
}

struct uvc_device * uvc_open(const char *devname, int bulk_mode)
{
	struct uvc_device *dev;
	struct v4l2_capability cap;
	int ret;
	int fd;
	int retry_cnt = 0;

retry:
	UVC_PRINTF(UVC_LOG_TRACE,"# %s =>\n", __func__);
	fd = open(devname, O_RDWR | O_NONBLOCK);
	if (fd == -1 && retry_cnt < 10) {
		UVC_PRINTF(UVC_LOG_ERR,"# v4l2 open failed: %s (%d)\n", strerror(errno), errno);
		sleep(1);
		retry_cnt ++;
		goto retry;
	}

	if (retry_cnt >=10) {
		UVC_PRINTF(UVC_LOG_ERR,"# retry v4l2 open failed: %s (%d)\n", strerror(errno), errno);
		return NULL;
	}

	UVC_PRINTF(UVC_LOG_DEBUG,"# open succeeded, file descriptor = %d\n", fd);

	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0) {
		UVC_PRINTF(UVC_LOG_ERR,"# unable to query device: %s (%d)\n", strerror(errno),
			errno);
		close(fd);
		return NULL;
	}

	UVC_PRINTF(UVC_LOG_USER_INFO,"# device is %s on bus %s\n", cap.card, cap.bus_info);

	dev = malloc(sizeof *dev);
	if (dev == NULL) {
		close(fd);
		return NULL;
	}

	g_dev = dev;
	dev->maxsize = UVC_V4L2_BUF_SIZE_DEF;

	memset(dev, 0, sizeof *dev);
	dev->fd = fd;
	dev->bulk = bulk_mode;
	dev->stream_status = UVC_STREAM_OFF;
	if (dev->bulk)
		dev->bulk_stream_on = 0;

	uvc_events_init(dev);

	list_init(uvc_ext_ctrl_list);

	UVC_PRINTF(UVC_LOG_TRACE,"# %s <= \n", __func__);
	return dev;
}

void uvc_close(struct uvc_device *dev)
{
	struct uvc_ext_cs_ops *_p,* _q;
	list_del_all(_p, _q, uvc_ext_ctrl_list);

	close(dev->fd);

	if (!dev->imgsrc)
		free(dev->imgdata);

	free(dev->mem);
	free(dev);
	g_dev = NULL;
}
