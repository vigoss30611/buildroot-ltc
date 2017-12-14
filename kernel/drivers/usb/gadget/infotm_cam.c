/*
 *	infotm-cam.c -- USB infotm-cam gadget driver
 *
 *	Copyright (C) 2009-2016
 *	    davinci liang (davinci.liang@infotm.com)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/usb/video.h>
#include <linux/usb/ch9.h>
#include <linux/module.h>

#define INFOTM_UVC
#define INFOTM_MASS_STORAGE
//#define INFOTM_NET

#ifdef INFOTM_UVC
#include "f_uvc.h"

/*
 * Kbuild is not very cooperative with respect to linking separately
 * compiled library objects into one module.  So for now we won't use
 * separate compilation ... ensuring init/exit sections work to shrink
 * the runtime footprint, and giving us at least some parts of what
 * a "gcc --combine ... part1.c part2.c part3.c ... " build would.
 */
#include "uvc_queue.c"
#include "uvc_video.c"
#include "uvc_v4l2.c"

#include "f_uvc.c"
#endif

#ifdef INFOTM_MASS_STORAGE
#include "f_mass_storage.c"
#endif

#ifdef INFOTM_NET

#if defined USB_ETH_RNDIS
#  undef USB_ETH_RNDIS
#endif

#  define USB_ETH_RNDIS y

#ifdef USB_ETH_RNDIS
#  include "f_rndis.c"
#  include "rndis.c"
#endif
#include "u_ether.c"

#endif /*end for INFOTM_NET*/

USB_GADGET_COMPOSITE_OPTIONS();

/* --------------------------------------------------------------------------
 * Device Descriptor
 */
#define INFOTM_CAM_VENDOR_ID		0x0525	/* NetChip */
#define INFOTM_CAM_PRODUCT_ID	0xa4a2	/* Ethernet/RNDIS Gadget */
#define INFOTM_CAM_DEVICE_BCD		0x0100	/* 1.00 */

static char infotm_cam_vendor_label[] = "Infotm Electronics.Corp";
static char infotm_cam_product_label[] = "Infotm Camera";
static char infotm_cam_config_label[] = "Video && Net";

/* string IDs are assigned dynamically */
enum {
	__MULTI_NO_CONFIG = 0,
	MULTI_UVC_CONFIG_NUM = 1,
};

#define STRING_DESCRIPTION_IDX		USB_GADGET_FIRST_AVAIL_IDX
enum {
	MULTI_STRING_UVC_CONFIG_IDX = USB_GADGET_FIRST_AVAIL_IDX,
};

static struct usb_string infotm_cam_strings[] = {
	[USB_GADGET_MANUFACTURER_IDX].s = infotm_cam_vendor_label,
	[USB_GADGET_PRODUCT_IDX].s = infotm_cam_product_label,
	[USB_GADGET_SERIAL_IDX].s = "",
	[STRING_DESCRIPTION_IDX].s = infotm_cam_config_label,
	{  }
};

static struct usb_gadget_strings infotm_cam_stringtab = {
	.language = 0x0409,	/* en-us */
	.strings = infotm_cam_strings,
};

static struct usb_gadget_strings *infotm_cam_device_strings[] = {
	&infotm_cam_stringtab,
	NULL,
};

static struct usb_device_descriptor infotm_cam_device_descriptor = {
	.bLength		= USB_DT_DEVICE_SIZE,
	.bDescriptorType	= USB_DT_DEVICE,
	.bcdUSB			= cpu_to_le16(0x0200),
	.bDeviceClass		= USB_CLASS_MISC,
	.bDeviceSubClass	= 0x02,
	.bDeviceProtocol	= 0x01,
	.bMaxPacketSize0	= 0, /* dynamic */
	.idVendor		= cpu_to_le16(INFOTM_CAM_VENDOR_ID),
	.idProduct		= cpu_to_le16(INFOTM_CAM_PRODUCT_ID),
	.bcdDevice		= cpu_to_le16(INFOTM_CAM_DEVICE_BCD),
	.iManufacturer		= 0, /* dynamic */
	.iProduct		= 0, /* dynamic */
	.iSerialNumber		= 0, /* dynamic */
	.bNumConfigurations	= 1,
};

/* --------------------------------------------------------------------------
 * UVC Descriptor
 */
#ifdef INFOTM_UVC

DECLARE_UVC_HEADER_DESCRIPTOR(1);

static const struct UVC_HEADER_DESCRIPTOR(1) uvc_control_header = {
	.bLength		= UVC_DT_HEADER_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_HEADER,
	.bcdUVC			= cpu_to_le16(0x0100),
	.wTotalLength		= 0, /* dynamic */
	.dwClockFrequency	= cpu_to_le32(48000000),
	.bInCollection		= 0, /* dynamic */
	.baInterfaceNr[0]	= 0, /* dynamic */
};

static const struct uvc_camera_terminal_descriptor uvc_camera_terminal = {
	.bLength		= UVC_DT_CAMERA_TERMINAL_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_INPUT_TERMINAL,
	.bTerminalID		= 1,
	.wTerminalType		= cpu_to_le16(0x0201),
	.bAssocTerminal		= 0,
	.iTerminal		= 0,
	.wObjectiveFocalLengthMin	= cpu_to_le16(0),
	.wObjectiveFocalLengthMax	= cpu_to_le16(0),
	.wOcularFocalLength		= cpu_to_le16(0),
	.bControlSize		= 3,
	.bmControls[0]		= 0xff,
	.bmControls[1]		= 0xff,
	.bmControls[2]		= 0x0f,
};

static const struct uvc_processing_unit_descriptor uvc_processing = {
	.bLength		= UVC_DT_PROCESSING_UNIT_SIZE(2),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_PROCESSING_UNIT,
	.bUnitID		= 2,
	.bSourceID		= 1,
	.wMaxMultiplier		= cpu_to_le16(16*1024),
	.bControlSize		= 2,
	.bmControls[0]		= 0x7f,
	.bmControls[1]		= 0xdf,
	.iProcessing		= 0,
};

DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3);

/*
static const struct UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3) infotm_extension_uint = {
	.bLength		= UVC_DT_EXTENSION_UNIT_SIZE(1,3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_EXTENSION_UNIT,
	.bUnitID		= 3,
	.guidExtensionCode = { 0x6a, 0xd1, 0x49, 0x2c, 0xb8, 0x32, 0x85, 0x44, 0x3e, 
			0xa8, 0x64, 0x3a, 0x15, 0x23, 0x62, 0xf2},
	.bNumControls = 20,
	.bNrInPins = 1,
	.baSourceID[0]		= 2,
	.bControlSize		= 3,
	.bmControls[0]		= 0xff,
	.bmControls[1]		= 0xff,
	.bmControls[2]		= 0xff,
	.iExtension		= 0,
};*/

DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(1, 18);

static const struct UVC_EXTENSION_UNIT_DESCRIPTOR(1, 18) h264_extension_unit = {
	.bLength		= UVC_DT_EXTENSION_UNIT_SIZE(1,18),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_EXTENSION_UNIT,
	.bUnitID		= 3,
	.guidExtensionCode = { 0x41, 0x76, 0x9e, 0xa2, 0x04, 0xde, 0xe3, 0x47, 0x8b,
			0x2b, 0xf4, 0x34, 0x1a, 0xff, 0x00, 0x3b},
	.bNumControls = 17,
	.bNrInPins = 1,
	.baSourceID[0]		= 2,
	.bControlSize		= 18,
	.bmControls[0]		= 0xff,
	.bmControls[1]		= 0xff,
	.bmControls[2]		= 0xff,
	.bmControls[3]		= 0xff,
	.bmControls[4]		= 0xff,
	.bmControls[5]		= 0xff,
	.bmControls[6]		= 0xff,
	.bmControls[7]		= 0xff,
	.bmControls[8]		= 0xff,
	.bmControls[9]		= 0xff,
	.bmControls[10]		= 0xff,
	.bmControls[11]		= 0xff,
	.bmControls[12]		= 0xff,
	.bmControls[13]		= 0xff,
	.bmControls[14]		= 0xff,
	.bmControls[15]		= 0xff,
	.bmControls[16]		= 0xff,
	.iExtension		= 0,
};

static const struct uvc_output_terminal_descriptor uvc_output_terminal = {
	.bLength		= UVC_DT_OUTPUT_TERMINAL_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_OUTPUT_TERMINAL,
	.bTerminalID		= 4,
	.wTerminalType		= cpu_to_le16(0x0101),
	.bAssocTerminal		= 0,
	.bSourceID		= 3,
	.iTerminal		= 0,
};

DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 1);

static const struct UVC_INPUT_HEADER_DESCRIPTOR(1, 1) uvc_input_header = {
	.bLength		= UVC_DT_INPUT_HEADER_SIZE(1, 1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_INPUT_HEADER,
	.bNumFormats		= 1,
	.wTotalLength		= 0, /* dynamic */
	.bEndpointAddress	= 0, /* dynamic */
	.bmInfo			= 0,
	.bTerminalLink		= 4, /* Note: maybe fixed windows bluescreen bug */
	.bStillCaptureMethod	= 0,
	.bTriggerSupport	= 0,
	.bTriggerUsage		= 0,
	.bControlSize		= 1,
//	.bmaControls[0][0]	= 0,
//	.bmaControls[1][0]	= 4,
	.bmaControls[0][0]	= 4,
};

static const struct uvc_format_uncompressed uvc_format_yuv = {
	.bLength		= UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FORMAT_UNCOMPRESSED,
	.bFormatIndex		= 1,
	.bNumFrameDescriptors	= 2,
	.guidFormat		=
		{ 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00,
		 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71},
	.bBitsPerPixel		= 16,
	.bDefaultFrameIndex	= 1,
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
};

DECLARE_UVC_FRAME_UNCOMPRESSED(1);
DECLARE_UVC_FRAME_UNCOMPRESSED(3);

static const struct UVC_FRAME_UNCOMPRESSED(3) uvc_frame_yuv_360p = {
	.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(360),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(460800),
	.dwDefaultFrameInterval	= cpu_to_le32(666666),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(666666),
	.dwFrameInterval[1]	= cpu_to_le32(1000000),
	.dwFrameInterval[2]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_UNCOMPRESSED(1) uvc_frame_yuv_720p = {
	.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1280),
	.wHeight		= cpu_to_le16(720),
	.dwMinBitRate		= cpu_to_le32(29491200),
	.dwMaxBitRate		= cpu_to_le32(29491200),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(1843200),
	.dwDefaultFrameInterval	= cpu_to_le32(5000000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(5000000),
};

static const struct uvc_format_mjpeg uvc_format_mjpg = {
	.bLength		= UVC_DT_FORMAT_MJPEG_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FORMAT_MJPEG,
	.bFormatIndex		= 2,
	.bNumFrameDescriptors	= 7, /*2 to 7*/
	.bmFlags		= 0,
	.bDefaultFrameIndex	= 1,
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
};

DECLARE_UVC_FRAME_MJPEG(1);
DECLARE_UVC_FRAME_MJPEG(4);
DECLARE_UVC_FRAME_MJPEG(5);

static const struct UVC_FRAME_MJPEG(5) uvc_frame_mjpg_360p = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(360),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(460800),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 5,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_MJPEG(5) uvc_frame_mjpg_720p = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1280),
	.wHeight		= cpu_to_le16(720),
	.dwMinBitRate		= cpu_to_le32(29491200),
	.dwMaxBitRate		= cpu_to_le32(884736000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(1843200),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 5,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_MJPEG(5) uvc_frame_mjpg_1080p = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 3,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1920),
	.wHeight		= cpu_to_le16(1080),
	.dwMinBitRate		= cpu_to_le32(66355200),
	.dwMaxBitRate		= cpu_to_le32(1990656000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(4147200),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 5,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_MJPEG(4) uvc_frame_mjpg_3840x1080 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(4),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 4,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(3840),
	.wHeight		= cpu_to_le16(1080),
	.dwMinBitRate		= cpu_to_le32(132710400),
	.dwMaxBitRate		= cpu_to_le32(1990656000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(8294400),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 4,
	.dwFrameInterval[0]	= cpu_to_le32(333333),
	.dwFrameInterval[1]	= cpu_to_le32(666666),
	.dwFrameInterval[2]	= cpu_to_le32(1000000),
	.dwFrameInterval[3]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_MJPEG(5) uvc_frame_mjpg_768x768  = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 5,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(768),
	.wHeight		= cpu_to_le16(768),
	.dwMinBitRate		= cpu_to_le32(2359296),
	.dwMaxBitRate		= cpu_to_le32(70778880), /* 768 * 768 *2 * 60 */
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(1179648), /* 768 * 768 *2 */
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 5,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_MJPEG(5) uvc_frame_mjpg_1088x1088  = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 6,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1088),
	.wHeight		= cpu_to_le16(1088),
	.dwMinBitRate		= cpu_to_le32(66355200),
	.dwMaxBitRate		= cpu_to_le32(1990656000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(4147200),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 5,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_MJPEG(5) uvc_frame_mjpg_1920x1920  = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 7,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1920),
	.wHeight		= cpu_to_le16(1920),
	.dwMinBitRate		= cpu_to_le32(66355200),
	.dwMaxBitRate		= cpu_to_le32(1990656000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(4147200),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 5,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct uvc_format_frameBased uvc_format_h264 = {
	.bLength		= UVC_DT_FORMAT_FRAMEBASED_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FORMAT_FRAME_BASED,
	.bFormatIndex		= 1,
	.bNumFrameDescriptors	= 2,
	.guidFormat			=
		 { 'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00,
			0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71},
	.bBitsPerPixel		= 0,
	.bDefaultFrameIndex	= 1,
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
	.bVariableSize		= 1,
};

DECLARE_UVC_FRAME_FRAMEBASED(3);

static const struct UVC_FRAME_FRAMEBASED(3) uvc_frame_h264_720p = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth				= cpu_to_le16(1472),
	.wHeight			= cpu_to_le16(736),
	.dwMinBitRate		= cpu_to_le32(10485760),
	.dwMaxBitRate		= cpu_to_le32(16777216),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType = 3,
	.dwBytesPerLine		= 0,
	//when bFrameIntervalType = 0 which means continuous frame interval
	//.dwMinFrameInterval	= 0,
	//.dwMaxFrameInterval	= 0,
	//.dwFrameIntervalStep= 0,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
};

static const struct UVC_FRAME_FRAMEBASED(3) uvc_frame_h264_1920x960 = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth				= cpu_to_le16(1920),
	.wHeight			= cpu_to_le16(960),
	.dwMinBitRate		= cpu_to_le32(10485760),
	.dwMaxBitRate		= cpu_to_le32(16777216),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType = 3,
	.dwBytesPerLine		= 0,
	//when bFrameIntervalType = 0 which means continuous frame interval
	//.dwMinFrameInterval	= 0,
	//.dwMaxFrameInterval	= 0,
	//.dwFrameIntervalStep= 0,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
};

static const struct uvc_color_matching_descriptor uvc_color_matching = {
	.bLength		= UVC_DT_COLOR_MATCHING_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_COLORFORMAT,
	.bColorPrimaries	= 1,
	.bTransferCharacteristics	= 1,
	.bMatrixCoefficients	= 4,
};

static const struct uvc_descriptor_header * const uvc_fs_control_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_control_header,
	(const struct uvc_descriptor_header *) &uvc_camera_terminal,
	(const struct uvc_descriptor_header *) &uvc_processing,
//	(const struct uvc_descriptor_header *) &infotm_extension_uint,
	(const struct uvc_descriptor_header *) &h264_extension_unit,
	(const struct uvc_descriptor_header *) &uvc_output_terminal,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_ss_control_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_control_header,
	(const struct uvc_descriptor_header *) &uvc_camera_terminal,
	(const struct uvc_descriptor_header *) &uvc_processing,
//	(const struct uvc_descriptor_header *) &infotm_extension_uint,
	(const struct uvc_descriptor_header *) &h264_extension_unit,
	(const struct uvc_descriptor_header *) &uvc_output_terminal,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_fs_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
//	(const struct uvc_descriptor_header *) &uvc_format_yuv,
//	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
//	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
//	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_3840x1080,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_768x768,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1088x1088,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1920x1920,
	(const struct uvc_descriptor_header *) &uvc_format_h264,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_720p,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1920x960,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_hs_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
//	(const struct uvc_descriptor_header *) &uvc_format_yuv,
//	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
//	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
//	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_3840x1080,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_768x768,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1088x1088,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1920x1920,
	(const struct uvc_descriptor_header *) &uvc_format_h264,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_720p,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1920x960,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_ss_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
//	(const struct uvc_descriptor_header *) &uvc_format_yuv,
//	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
//	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
//	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_3840x1080,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_768x768,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1088x1088,
//	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1920x1920,
	(const struct uvc_descriptor_header *) &uvc_format_h264,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_720p,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1920x960,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};
#endif

/* --------------------------------------------------------------------------
 *  Infotm Cam Configuration
 */

static struct usb_configuration infotm_cam_config_driver = {
	.label			= infotm_cam_config_label,
	.bConfigurationValue	= MULTI_UVC_CONFIG_NUM,
	.bmAttributes		= USB_CONFIG_ATT_SELFPOWER |USB_CONFIG_ATT_WAKEUP,
	.MaxPower		= CONFIG_USB_GADGET_VBUS_DRAW,
};

/* --------------------------------------------------------------------------
 *  Mess Storage Class
 */

static struct usb_otg_descriptor otg_descriptor = {
	.bLength =		sizeof otg_descriptor,
	.bDescriptorType =	USB_DT_OTG,

	/*
	 * REVISIT SRP-only hardware is possible, although
	 * it would not be called "OTG" ...
	 */
	.bmAttributes =		USB_OTG_SRP | USB_OTG_HNP,
};

static const struct usb_descriptor_header *otg_desc[] = {
	(struct usb_descriptor_header *) &otg_descriptor,
	NULL,
};

#ifdef INFOTM_MASS_STORAGE
static struct fsg_common fsg_common;

static struct fsg_module_parameters mod_data = {
	.stall = 1
};
FSG_MODULE_PARAMETERS(/* no prefix */, mod_data);
#endif

#ifdef INFOTM_NET
static u8 hostaddr[ETH_ALEN];
static struct eth_dev *the_dev;
#endif

/* --------------------------------------------------------------------------
 * UVC & UMC function register
 */

static int
infotm_cam_do_config(struct usb_configuration *c)
{
	int ret = 0;
	if (gadget_is_otg(c->cdev->gadget)) {
		c->descriptors = otg_desc;
		c->bmAttributes |= USB_CONFIG_ATT_WAKEUP;
	}

#ifdef INFOTM_NET
	ret = rndis_bind_config(c, hostaddr, the_dev);
	if (ret < 0)
		goto err;
#endif

#ifdef INFOTM_UVC
	ret = uvc_bind_config(c, uvc_fs_control_cls, uvc_ss_control_cls,
		uvc_fs_streaming_cls, uvc_hs_streaming_cls,
		uvc_ss_streaming_cls);
	if (ret < 0)
		goto err;
#endif

#ifdef INFOTM_MASS_STORAGE
	ret = fsg_bind_config(c->cdev, c, &fsg_common);
	if (ret < 0)
		goto err;
#endif
	return 0;

err:
	return ret;
}

static int
infotm_cam_config_register(struct usb_composite_dev *cdev)
{
	int ret;
	/* Register our configuration. */
	if ((ret = usb_add_config(cdev, &infotm_cam_config_driver,
					infotm_cam_do_config)) < 0)
		goto error;

	dev_info(&cdev->gadget->dev, "Infotm cam Gadget\n");
	return 0;

error:
	return ret;
}

static int __init
infotm_cam_bind(struct usb_composite_dev *cdev)
{
	int ret;
#ifdef INFOTM_NET
	if (!can_support_ecm(cdev->gadget)) {
		dev_err(&cdev->gadget->dev, "controller '%s' not usable\n", cdev->gadget->name);
		return -EINVAL;
	}

	/* set up network link layer */
	the_dev = gether_setup(cdev->gadget, hostaddr);
	if (IS_ERR(the_dev))
		return PTR_ERR(the_dev);
#endif

#ifdef INFOTM_MASS_STORAGE
	/* set up mass storage function */
	{
		void *retp;
		retp = fsg_common_from_params(&fsg_common, cdev, &mod_data);
		if (IS_ERR(retp)) {
			ret = PTR_ERR(retp);
			goto err_fsg;
		}
	}
#endif
	/* Allocate string descriptor numbers ... note that string contents
	 * can be overridden by the composite_dev glue.
	 */
	ret = usb_string_ids_tab(cdev, infotm_cam_strings);
	if (ret < 0)
		goto error;
	infotm_cam_device_descriptor.iManufacturer =
		infotm_cam_strings[USB_GADGET_MANUFACTURER_IDX].id;
	infotm_cam_device_descriptor.iProduct =
		infotm_cam_strings[USB_GADGET_PRODUCT_IDX].id;

	infotm_cam_config_driver.iConfiguration =
			infotm_cam_strings[MULTI_STRING_UVC_CONFIG_IDX].id;

	/* register configurations */
	ret = infotm_cam_config_register(cdev);
	if (unlikely(ret < 0))
		goto error;

	usb_composite_overwrite_options(cdev, &coverwrite);
#ifdef INFOTM_MASS_STORAGE
	fsg_common_put(&fsg_common);
	return 0;

err_fsg:
	fsg_common_put(&fsg_common);
#else
	return 0;
#endif
error:
	return ret;
}

static int /* __init_or_exit */
infotm_cam_unbind(struct usb_composite_dev *cdev)
{
#ifdef INFOTM_NET
	gether_cleanup(the_dev);
#endif
	return 0;
}

static void
infotm_cam_suspend(struct usb_composite_dev * cdev)
{
	dev_info(&cdev->gadget->dev, "%s\n", __func__);
}

static void
infotm_cam_resume(struct usb_composite_dev * cdev)
{
	dev_info(&cdev->gadget->dev, "%s\n", __func__);

}

static void infotm_cam_disconnect(struct usb_composite_dev *cdev)
{
	dev_info(&cdev->gadget->dev, "%s\n", __func__);
}
/* --------------------------------------------------------------------------
 * Driver
 */

static __refdata struct usb_composite_driver infotm_cam_driver = {
	.name		= "g_infotm_cam",
	.dev		= &infotm_cam_device_descriptor,
	.strings	= infotm_cam_device_strings,
	.max_speed	= USB_SPEED_SUPER,
	.bind		= infotm_cam_bind,
	.unbind		= infotm_cam_unbind,
	.disconnect	= infotm_cam_disconnect,
};

static int __init
infotm_cam_init(void)
{
	return usb_composite_probe(&infotm_cam_driver);
}

static void __exit
infotm_cam_cleanup(void)
{
	usb_composite_unregister(&infotm_cam_driver);
}

module_init(infotm_cam_init);
module_exit(infotm_cam_cleanup);

MODULE_AUTHOR("davinci liang");
MODULE_DESCRIPTION("Infotm Cam Gadget");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.0");
