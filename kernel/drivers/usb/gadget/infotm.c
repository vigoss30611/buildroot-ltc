/*
 * Gadget Driver for Infotm
 *
 * Copyright (C) 2106 Infotm, Inc.
 * Author: Davinci liang <davinci.liang@infotm.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/utsname.h>
#include <linux/platform_device.h>

#include <linux/usb/ch9.h>
#include <linux/usb/composite.h>
#include <linux/usb/gadget.h>

#include "gadget_chips.h"
#include "f_fs.c"
#include "f_uvc.h"
#include "uvc_queue.c"
#include "uvc_video.c"
#include "uvc_v4l2.c"
#include "f_uvc.c"

#include "f_mass_storage.c"

#define USB_ETH_RNDIS y
#include "f_rndis.c"
#include "rndis.c"
#include "u_ether.c"
#include "f_hid_infotm.c"

MODULE_AUTHOR("Davinci Liang");
MODULE_DESCRIPTION("Infotm Composite USB Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static const char longname[] = "Gadget Infotm";

/* Default vendor and product IDs, overridden by userspace */
#define VENDOR_ID		0x18D1
#define PRODUCT_ID		0x0001

struct infotm_usb_function {
	char *name;
	void *config;

	struct device *dev;
	char *dev_name;
	struct device_attribute **attributes;

	/* for .enabled_functions */
	struct list_head enabled_list;

	/* application trigger usb connect (enumeration) flag */
	bool enum_no_ready;

	/* Optional: initialization during gadget bind */
	int (*init)(struct infotm_usb_function *, struct usb_composite_dev *);
	/* Optional: cleanup during gadget unbind */
	void (*cleanup)(struct infotm_usb_function *);
	/* Optional: called when the function is added the list of
	 *		enabled functions */
	void (*enable)(struct infotm_usb_function *);
	/* Optional: called when it is removed */
	void (*disable)(struct infotm_usb_function *);

	int (*bind_config)(struct infotm_usb_function *,
			   struct usb_configuration *);

	/* Optional: called when the configuration is removed */
	void (*unbind_config)(struct infotm_usb_function *,
			      struct usb_configuration *);
	/* Optional: handle ctrl requests before the device is configured */
	int (*ctrlrequest)(struct infotm_usb_function *,
					struct usb_composite_dev *,
					const struct usb_ctrlrequest *);
};

struct infotm_dev {
	struct infotm_usb_function **functions;
	struct list_head enabled_functions;
	struct usb_composite_dev *cdev;
	struct device *dev;

	bool enabled;
	int disable_depth;
	struct mutex mutex;
	bool connected;
	bool sw_connected;
	struct work_struct work;
	char ffs_aliases[256];
};

static struct class *infotm_class;
static struct infotm_dev *_infotm_dev;
static int infotm_bind_config(struct usb_configuration *c);
static void infotm_unbind_config(struct usb_configuration *c);

static struct file * g_ep0fp = NULL;
/* string IDs are assigned dynamically */
#define STRING_MANUFACTURER_IDX		0
#define STRING_PRODUCT_IDX		1
#define STRING_SERIAL_IDX		2

static char manufacturer_string[256];
static char product_string[256];
static char serial_string[256];

/* String Table */
static struct usb_string strings_dev[] = {
	[STRING_MANUFACTURER_IDX].s = manufacturer_string,
	[STRING_PRODUCT_IDX].s = product_string,
	[STRING_SERIAL_IDX].s = serial_string,
	{  }			/* end of list */
};

static struct usb_gadget_strings stringtab_dev = {
	.language	= 0x0409,	/* en-us */
	.strings	= strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
	&stringtab_dev,
	NULL,
};

static struct usb_device_descriptor device_desc = {
	.bLength              = sizeof(device_desc),
	.bDescriptorType      = USB_DT_DEVICE,
	.bcdUSB               = __constant_cpu_to_le16(0x0200),
	.bDeviceClass         = USB_CLASS_PER_INTERFACE,
	.idVendor             = __constant_cpu_to_le16(VENDOR_ID),
	.idProduct            = __constant_cpu_to_le16(PRODUCT_ID),
	.bcdDevice            = __constant_cpu_to_le16(0xffff),
	.bNumConfigurations   = 1,
};

static struct usb_configuration infotm_config_driver = {
	.label		= "infotm",
	.unbind		= infotm_unbind_config,
	.bConfigurationValue = 1,
	.bmAttributes	= USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.MaxPower	= 500, /* 500ma */
};

static void infotm_work(struct work_struct *data)
{
	struct infotm_dev *dev = container_of(data, struct infotm_dev, work);
	struct usb_composite_dev *cdev = dev->cdev;
	char *disconnected[2] = { "USB_STATE=DISCONNECTED", NULL };
	char *connected[2]    = { "USB_STATE=CONNECTED", NULL };
	char *configured[2]   = { "USB_STATE=CONFIGURED", NULL };
	char **uevent_envp = NULL;
	unsigned long flags;

	spin_lock_irqsave(&cdev->lock, flags);
	if (cdev->config)
		uevent_envp = configured;
	else if (dev->connected != dev->sw_connected)
		uevent_envp = dev->connected ? connected : disconnected;
	dev->sw_connected = dev->connected;
	spin_unlock_irqrestore(&cdev->lock, flags);

	if (uevent_envp) {
		kobject_uevent_env(&dev->dev->kobj, KOBJ_CHANGE, uevent_envp);
		pr_info("%s: sent uevent %s\n", __func__, uevent_envp[0]);
	} else {
		pr_info("%s: did not send uevent (%d %d %p)\n", __func__,
			 dev->connected, dev->sw_connected, cdev->config);
	}
}

static void infotm_enable(struct infotm_dev *dev, bool enum_ready)
{
	struct usb_composite_dev *cdev = dev->cdev;

	if (WARN_ON(!dev->disable_depth))
		return;

	pr_info("# %s enum_ready:%d\n", __func__, enum_ready);
	if (--dev->disable_depth == 0) {
		usb_add_config(cdev, &infotm_config_driver,
					infotm_bind_config);
		if (enum_ready)
			usb_gadget_connect(cdev->gadget);
		else
			pr_info("%s: waiting usb gadget connect\n", __func__);
	}
}

static void infotm_disable(struct infotm_dev *dev)
{
	struct usb_composite_dev *cdev = dev->cdev;

	if (dev->disable_depth++ == 0) {
		usb_gadget_disconnect(cdev->gadget);
		/* Cancel pending control requests */
		usb_ep_dequeue(cdev->gadget->ep0, cdev->req);
		usb_remove_config(cdev, &infotm_config_driver);
	}
}

/*-------------------------------------------------------------------------*/
/* uvc function */

/* --------------------------------------------------------------------------*/
/* UVC Descriptor */

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
	.bmControls[0]		= 0xff,
	.bmControls[1]		= 0xff,
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
	.bControlSize		= 18 ,
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
	.bNumFrameDescriptors	= 3, /* 7 to 3 */
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

#if 0
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
#endif

#define UVC_GUID_FORMAT_H264 \
	{ 'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00, \
	 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

static const struct uvc_format_frameBased uvc_format_h264 = {
	.bLength		= UVC_DT_FORMAT_FRAMEBASED_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FORMAT_FRAME_BASED,
	.bFormatIndex		= 3,
	.bNumFrameDescriptors	= 4,
	.guidFormat			= UVC_GUID_FORMAT_H264,
	.bBitsPerPixel		= 0,
	.bDefaultFrameIndex	= 1,
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
	.bVariableSize		= 1,
};

DECLARE_UVC_FRAME_FRAMEBASED(1);
DECLARE_UVC_FRAME_FRAMEBASED(5);

static const struct UVC_FRAME_FRAMEBASED(5) uvc_frame_h264_480p = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth				= cpu_to_le16(640),
	.wHeight			= cpu_to_le16(480),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType = 5,
	.dwBytesPerLine		= 0,
	//when bFrameIntervalType = 0 which means continuous frame interval
	//.dwMinFrameInterval	= 0,
	//.dwMaxFrameInterval	= 0,
	//.dwFrameIntervalStep= 0,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_FRAMEBASED(5) uvc_frame_h264_960x480 = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth				= cpu_to_le16(960),
	.wHeight			= cpu_to_le16(480),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType = 5,
	.dwBytesPerLine		= 0,
	//when bFrameIntervalType = 0 which means continuous frame interval
	//.dwMinFrameInterval	= 0,
	//.dwMaxFrameInterval	= 0,
	//.dwFrameIntervalStep= 0,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};


static const struct UVC_FRAME_FRAMEBASED(5) uvc_frame_h264_1472x736 = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 3,
	.bmCapabilities		= 0,
	.wWidth				= cpu_to_le16(1472),
	.wHeight			= cpu_to_le16(736),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType = 5,
	.dwBytesPerLine		= 0,
	//when bFrameIntervalType = 0 which means continuous frame interval
	//.dwMinFrameInterval	= 0,
	//.dwMaxFrameInterval	= 0,
	//.dwFrameIntervalStep= 0,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_FRAMEBASED(5) uvc_frame_h264_1920x960 = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(5),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType = UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 4,
	.bmCapabilities		= 0,
	.wWidth				= cpu_to_le16(1920),
	.wHeight			= cpu_to_le16(960),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType = 5,
	.dwBytesPerLine		= 0,
	//when bFrameIntervalType = 0 which means continuous frame interval
	//.dwMinFrameInterval	= 0,
	//.dwMaxFrameInterval	= 0,
	//.dwFrameIntervalStep= 0,
	.dwFrameInterval[0]	= cpu_to_le32(166666),
	.dwFrameInterval[1]	= cpu_to_le32(333333),
	.dwFrameInterval[2]	= cpu_to_le32(666666),
	.dwFrameInterval[3]	= cpu_to_le32(1000000),
	.dwFrameInterval[4]	= cpu_to_le32(5000000),
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
	(const struct uvc_descriptor_header *) &uvc_format_yuv,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
#if 0
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_3840x1080,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_768x768,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1088x1088,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1920x1920,
#endif
	(const struct uvc_descriptor_header *) &uvc_format_h264,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_480p,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_960x480,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1472x736,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1920x960,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_hs_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
	(const struct uvc_descriptor_header *) &uvc_format_yuv,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
#if 0
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_3840x1080,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_768x768,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1088x1088,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1920x1920,
#endif
	(const struct uvc_descriptor_header *) &uvc_format_h264,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_480p,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_960x480,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1472x736,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1920x960,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_ss_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
	(const struct uvc_descriptor_header *) &uvc_format_yuv,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
#if 0
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_3840x1080,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_768x768,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1088x1088,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1920x1920,
#endif
	(const struct uvc_descriptor_header *) &uvc_format_h264,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_480p,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_960x480,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1472x736,
	(const struct uvc_descriptor_header *) &uvc_frame_h264_1920x960,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

struct uvc_function_config {
	/* Descriptors */
	struct {
		const struct uvc_descriptor_header * const *fs_control;
		const struct uvc_descriptor_header * const *ss_control;
		const struct uvc_descriptor_header * const *fs_streaming;
		const struct uvc_descriptor_header * const *hs_streaming;
		const struct uvc_descriptor_header * const *ss_streaming;
	} desc;
};

static int uvc_function_init(struct infotm_usb_function *f,
					struct usb_composite_dev *cdev)
{
	struct uvc_function_config *config = NULL;

	config = kzalloc(sizeof(struct uvc_function_config),
								GFP_KERNEL);
	if (!config)
		return -ENOMEM;

	config->desc.fs_control = uvc_fs_control_cls;
	config->desc.ss_control = uvc_ss_control_cls;
	config->desc.fs_streaming = uvc_fs_streaming_cls;
	config->desc.hs_streaming = uvc_hs_streaming_cls;
	config->desc.ss_streaming = uvc_ss_streaming_cls;

	f->enum_no_ready = true; /* enable and don't connect */
	f->config = config;
	return 0;
}

static void uvc_function_cleanup(struct infotm_usb_function *f)
{
	kfree(f->config);
	f->config = NULL;
}

static int uvc_function_bind_config(struct infotm_usb_function *f,
						struct usb_configuration *c)
{
	struct uvc_function_config *config = f->config;
	return uvc_bind_config(c,	config->desc.fs_control,
						config->desc.ss_control,
						config->desc.fs_streaming,
						config->desc.hs_streaming,
						config->desc.ss_streaming);
}

static struct infotm_usb_function uvc_function = {
	.name		= "uvc",
	.init		= uvc_function_init,
	.cleanup	= uvc_function_cleanup,
	.bind_config	= uvc_function_bind_config,
	.attributes	= NULL,
};

/*-------------------------------------------------------------------------*/
/* rndis function */

struct rndis_function_config {
	u8      ethaddr[ETH_ALEN];
	u32     vendorID;
	char	manufacturer[256];
	/* "Wireless" RNDIS; auto-detected by Windows */
	bool	wceis;
	struct eth_dev *dev;
};

static int
rndis_function_init(struct infotm_usb_function *f,
		struct usb_composite_dev *cdev)
{
	f->config = kzalloc(sizeof(struct rndis_function_config), GFP_KERNEL);
	if (!f->config)
		return -ENOMEM;
	f->enum_no_ready = false; /* enable and connect */
	return 0;
}

static void rndis_function_cleanup(struct infotm_usb_function *f)
{
	kfree(f->config);
	f->config = NULL;
}

static int
rndis_function_bind_config(struct infotm_usb_function *f,
		struct usb_configuration *c)
{
	int ret;
	struct eth_dev *dev;
	struct rndis_function_config *rndis = f->config;

	if (!rndis) {
		pr_err("%s: rndis_pdata\n", __func__);
		return -1;
	}

	pr_info("%s MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", __func__,
		rndis->ethaddr[0], rndis->ethaddr[1], rndis->ethaddr[2],
		rndis->ethaddr[3], rndis->ethaddr[4], rndis->ethaddr[5]);

	dev = gether_setup_name(c->cdev->gadget, rndis->ethaddr, "usb"); /* rndis -> usb */
	if (IS_ERR(dev)) {
		ret = PTR_ERR(dev);
		pr_err("%s: gether_setup failed\n", __func__);
		return ret;
	}
	rndis->dev = dev;

	if (rndis->wceis) {
		/* "Wireless" RNDIS; auto-detected by Windows */
		rndis_iad_descriptor.bFunctionClass =
						USB_CLASS_WIRELESS_CONTROLLER;
		rndis_iad_descriptor.bFunctionSubClass = 0x01;
		rndis_iad_descriptor.bFunctionProtocol = 0x03;
		rndis_control_intf.bInterfaceClass =
						USB_CLASS_WIRELESS_CONTROLLER;
		rndis_control_intf.bInterfaceSubClass =	 0x01;
		rndis_control_intf.bInterfaceProtocol =	 0x03;
	}

	return rndis_bind_config_vendor(c, rndis->ethaddr, rndis->vendorID,
					   rndis->manufacturer, rndis->dev);
}

static void rndis_function_unbind_config(struct infotm_usb_function *f,
						struct usb_configuration *c)
{
	struct rndis_function_config *rndis = f->config;
	gether_cleanup(rndis->dev);
}

static ssize_t rndis_manufacturer_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *config = f->config;
	return sprintf(buf, "%s\n", config->manufacturer);
}

static ssize_t rndis_manufacturer_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *config = f->config;

	if (size >= sizeof(config->manufacturer))
		return -EINVAL;
	if (sscanf(buf, "%s", config->manufacturer) == 1)
		return size;
	return -1;
}

static DEVICE_ATTR(manufacturer, S_IRUGO | S_IWUSR, rndis_manufacturer_show,
						    rndis_manufacturer_store);

static ssize_t rndis_wceis_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *config = f->config;
	return sprintf(buf, "%d\n", config->wceis);
}

static ssize_t rndis_wceis_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *config = f->config;
	int value;

	if (sscanf(buf, "%d", &value) == 1) {
		config->wceis = value;
		return size;
	}
	return -EINVAL;
}

static DEVICE_ATTR(wceis, S_IRUGO | S_IWUSR, rndis_wceis_show,
					     rndis_wceis_store);

static ssize_t rndis_ethaddr_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *rndis = f->config;
	return sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		rndis->ethaddr[0], rndis->ethaddr[1], rndis->ethaddr[2],
		rndis->ethaddr[3], rndis->ethaddr[4], rndis->ethaddr[5]);
}

static ssize_t rndis_ethaddr_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *rndis = f->config;

	if (sscanf(buf, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		    (int *)&rndis->ethaddr[0], (int *)&rndis->ethaddr[1],
		    (int *)&rndis->ethaddr[2], (int *)&rndis->ethaddr[3],
		    (int *)&rndis->ethaddr[4], (int *)&rndis->ethaddr[5]) == 6)
		return size;
	return -EINVAL;
}

static DEVICE_ATTR(ethaddr, S_IRUGO | S_IWUSR, rndis_ethaddr_show,
					       rndis_ethaddr_store);

static ssize_t rndis_vendorID_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *config = f->config;
	return sprintf(buf, "%04x\n", config->vendorID);
}

static ssize_t rndis_vendorID_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct rndis_function_config *config = f->config;
	int value;

	if (sscanf(buf, "%04x", &value) == 1) {
		config->vendorID = value;
		return size;
	}
	return -EINVAL;
}

static DEVICE_ATTR(vendorID, S_IRUGO | S_IWUSR, rndis_vendorID_show,
						rndis_vendorID_store);

static struct device_attribute *rndis_function_attributes[] = {
	&dev_attr_manufacturer,
	&dev_attr_wceis,
	&dev_attr_ethaddr,
	&dev_attr_vendorID,
	NULL
};

static struct infotm_usb_function rndis_function = {
	.name		= "rndis",
	.init		= rndis_function_init,
	.cleanup	= rndis_function_cleanup,
	.bind_config	= rndis_function_bind_config,
	.unbind_config	= rndis_function_unbind_config,
	.attributes	= rndis_function_attributes,
};

/*-------------------------------------------------------------------------*/
/* mass storage function */

struct mass_storage_function_config {
	struct fsg_config fsg;
	struct fsg_common *common;
};

static int mass_storage_function_init(struct infotm_usb_function *f,
					struct usb_composite_dev *cdev)
{
	struct mass_storage_function_config *config;
	struct fsg_common *common;
	int err;

	config = kzalloc(sizeof(struct mass_storage_function_config),
								GFP_KERNEL);
	if (!config)
		return -ENOMEM;

	config->fsg.nluns = 1;
	config->fsg.luns[0].removable = 1;

	common = fsg_common_init(NULL, cdev, &config->fsg);
	if (IS_ERR(common)) {
		kfree(config);
		return PTR_ERR(common);
	}

	err = sysfs_create_link(&f->dev->kobj,
				&common->luns[0].dev.kobj,
				"lun");
	if (err) {
		kfree(config);
		return err;
	}

	config->common = common;
	f->config = config;
	f->enum_no_ready = false; /* enable and connect */
	return 0;
}

static void mass_storage_function_cleanup(struct infotm_usb_function *f)
{
	kfree(f->config);
	f->config = NULL;
}

static int mass_storage_function_bind_config(struct infotm_usb_function *f,
						struct usb_configuration *c)
{
	struct mass_storage_function_config *config = f->config;
	return fsg_bind_config(c->cdev, c, config->common);
}

static ssize_t mass_storage_inquiry_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct mass_storage_function_config *config = f->config;
	return sprintf(buf, "%s\n", config->common->inquiry_string);
}

static ssize_t mass_storage_inquiry_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct infotm_usb_function *f = dev_get_drvdata(dev);
	struct mass_storage_function_config *config = f->config;
	if (size >= sizeof(config->common->inquiry_string))
		return -EINVAL;
	if (sscanf(buf, "%s", config->common->inquiry_string) != 1)
		return -EINVAL;
	return size;
}

static DEVICE_ATTR(inquiry_string, S_IRUGO | S_IWUSR,
					mass_storage_inquiry_show,
					mass_storage_inquiry_store);

static struct device_attribute *mass_storage_function_attributes[] = {
	&dev_attr_inquiry_string,
	NULL
};

static struct infotm_usb_function mass_storage_function = {
	.name		= "mass_storage",
	.init		= mass_storage_function_init,
	.cleanup	= mass_storage_function_cleanup,
	.bind_config	= mass_storage_function_bind_config,
	.attributes	= mass_storage_function_attributes,
};

static int hid_function_init(struct infotm_usb_function *f, struct usb_composite_dev *cdev)
{
   int status = 0;

    /* set up HID */
    status = infotm_hid_setup();
    if (status < 0) {
        printk(KERN_ERR"%s,%d: infotm_hid_setup failed\n", __FUNCTION__, __LINE__);
        return status;
    }   

	f->enum_no_ready = false; /* enable and connect */
    return 0;
}

static void hid_function_cleanup(struct infotm_usb_function *f)
{
    infotm_hid_cleanup(); 
}

static int hid_function_bind_config(struct infotm_usb_function *f, 
        struct usb_configuration *c)
{
    return infotm_hid_bind_config(c->cdev, c);
}

static struct infotm_usb_function hid_function = {
    .name = "hid",
    .init = hid_function_init,
    .cleanup = hid_function_cleanup,
    .bind_config = hid_function_bind_config,
    .attributes = NULL,
};

#define FFS_INJECT

#ifdef FFS_INJECT
struct usb_endpoint_descriptor_no_audio {
    __u8  bLength;
    __u8  bDescriptorType;

    __u8  bEndpointAddress;
    __u8  bmAttributes;
    __le16 wMaxPacketSize;
    __u8  bInterval;
} __attribute__((packed));


/*
 * All numbers must be in little endian order.
 */

struct usb_ffs_descs_head {
    __le32 magic;
    __le32 length;
    __le32 fs_count;
    __le32 hs_count;
} __attribute__((packed));

/*
 * Descriptors format:
 *
 * | off | name      | type         | description                          |
 * |-----+-----------+--------------+--------------------------------------|
 * |   0 | magic     | LE32         | FUNCTIONFS_{FS,HS}_DESCRIPTORS_MAGIC |
 * |   4 | length    | LE32         | length of the whole data chunk       |
 * |   8 | fs_count  | LE32         | number of full-speed descriptors     |
 * |  12 | hs_count  | LE32         | number of high-speed descriptors     |
 * |  16 | fs_descrs | Descriptor[] | list of full-speed descriptors       |
 * |     | hs_descrs | Descriptor[] | list of high-speed descriptors       |
 *
 * descs are just valid USB descriptors and have the following format:
 *
 * | off | name            | type | description              |
 * |-----+-----------------+------+--------------------------|
 * |   0 | bLength         | U8   | length of the descriptor |
 * |   1 | bDescriptorType | U8   | descriptor type          |
 * |   2 | payload         |      | descriptor's payload     |
 */

struct usb_ffs_strings_head {
    __le32 magic;
    __le32 length;
    __le32 str_count;
    __le32 lang_count;
} __attribute__((packed));

/*
 * Strings format:
 *
 * | off | name       | type                  | description                |
 * |-----+------------+-----------------------+----------------------------|
 * |   0 | magic      | LE32                  | FUNCTIONFS_STRINGS_MAGIC   |
 * |   4 | length     | LE32                  | length of the data chunk   |
 * |   8 | str_count  | LE32                  | number of strings          |
 * |  12 | lang_count | LE32                  | number of languages        |
 * |  16 | stringtab  | StringTab[lang_count] | table of strings per lang  |
 *
 * For each language there is one stringtab entry (ie. there are lang_count
 * stringtab entires).  Each StringTab has following format:
 *
 * | off | name    | type              | description                        |
 * |-----+---------+-------------------+------------------------------------|
 * |   0 | lang    | LE16              | language code                      |
 * |   2 | strings | String[str_count] | array of strings in given language |
 *
 * For each string there is one strings entry (ie. there are str_count
 * string entries).  Each String is a NUL terminated string encoded in
 * UTF-8.
 */

#define STR_INTERFACE_ "Source/Sink"

static const struct {
    struct usb_ffs_strings_head header;
    struct {
        __le16 code;
        const char str1[sizeof(STR_INTERFACE_)];
    } __attribute__((packed)) lang0;
} __attribute__((packed)) ffs_strings = {
    .header = {
        .magic = cpu_to_le32(FUNCTIONFS_STRINGS_MAGIC),
        .length = cpu_to_le32(sizeof(ffs_strings)),
        .str_count = cpu_to_le32(1),
        .lang_count = cpu_to_le32(1),
    },
    .lang0 = {
        cpu_to_le16(0x0409), /* en-us */
        STR_INTERFACE_,
    },
};

#define FASTBOOT_CLASS         0xff                                             
#define FASTBOOT_SUBCLASS      0x42                                             
#define FASTBOOT_PROTOCOL      0x3                                              
#define MAX_PACKET_SIZE_FS     64                                               
#define MAX_PACKET_SIZE_HS     512

static const struct {
    struct usb_ffs_descs_head header;
    struct {
        struct usb_interface_descriptor intf;
        struct usb_endpoint_descriptor_no_audio source;
        struct usb_endpoint_descriptor_no_audio sink;
    } __attribute__((packed)) fs_descs, hs_descs;
} __attribute__((packed)) descriptors = {
    .header = {
        .magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC),
        .length = cpu_to_le32(sizeof(descriptors)),
        .fs_count = 3,
        .hs_count = 3,
    },
    .fs_descs = {
        .intf = {
            .bLength = sizeof(descriptors.fs_descs.intf),
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 0,
            .bNumEndpoints = 2,
            .bInterfaceClass = FASTBOOT_CLASS,
            .bInterfaceSubClass = FASTBOOT_SUBCLASS,
            .bInterfaceProtocol = FASTBOOT_PROTOCOL,
            .iInterface = 1, /* first string from the provided table */
        },
        .source = {
            .bLength = sizeof(descriptors.fs_descs.source),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = 1 | USB_DIR_OUT,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = MAX_PACKET_SIZE_FS,
        },
        .sink = {
            .bLength = sizeof(descriptors.fs_descs.sink),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = 2 | USB_DIR_IN,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = MAX_PACKET_SIZE_FS,
        },
    },
    .hs_descs = {
        .intf = {
            .bLength = sizeof(descriptors.hs_descs.intf),
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 0,
            .bNumEndpoints = 2,
            .bInterfaceClass = FASTBOOT_CLASS,
            .bInterfaceSubClass = FASTBOOT_SUBCLASS,
            .bInterfaceProtocol = FASTBOOT_PROTOCOL,
            .iInterface = 1, /* first string from the provided table */
        },
        .source = {
            .bLength = sizeof(descriptors.hs_descs.source),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = 1 | USB_DIR_OUT,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = MAX_PACKET_SIZE_HS,
        },
        .sink = {
            .bLength = sizeof(descriptors.hs_descs.sink),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = 2 | USB_DIR_IN,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = MAX_PACKET_SIZE_HS,
        },
    },
};

static int ffs_force_inject(void) {
	char * mnt_path = "/dev/usb-ffs/fastboot";
	char * ep0file = "/dev/usb-ffs/fastboot/ep0";
	struct file * ep0fp = NULL;
    loff_t offset = 0;
	ssize_t nwcount = 0;
	int err;
	int ret = -1;

	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);

	if (g_ep0fp != NULL) {
		//only save the first filp, need close when unbind ffs  
		printk(KERN_ERR"%s: ffs force inject only once.\n", __FUNCTION__);
		goto ret1;
	}
	if (sys_access("/dev/usb-ffs", 0) != 0) {
		err = sys_mkdir("/dev/usb-ffs/", 0700);
		if (err<0) {
			printk(KERN_ERR"mkdir usb-ffs failed(%i)\n", err);
			goto ret1;
		}
	}
	if (sys_access(mnt_path, 0) != 0) {	
		err = sys_mkdir(mnt_path, 0700);
		if (err<0) {
			printk(KERN_ERR"mkdir usb-ffs/fastboot failed(%i)\n", err);
			goto ret1;
		}
		err = sys_mount("functionfs", mnt_path, "functionfs", MS_SILENT, NULL);
		if (err) {
			printk(KERN_INFO"funcitonfs: error mounting %i\n", err);
			goto ret1;
		} else {
			printk(KERN_INFO"functionfs: mounted\n");
		}	
	}

	ep0fp = filp_open(ep0file, O_RDWR, 0);
	if (IS_ERR(ep0fp)) {
		ep0fp = NULL;
		goto err1;
	}

	nwcount = vfs_write(ep0fp, (void*)&descriptors, sizeof(descriptors), &offset);
	if (nwcount < 0) {
		printk(KERN_ERR"error in file write descriptor: %d\n", nwcount);
		goto err1;
	} else {
		printk(KERN_ERR"file write descriptor ok: %d\n", nwcount);
	}
	offset = 0;
	nwcount = vfs_write(ep0fp, (void*)&ffs_strings, sizeof(ffs_strings), &offset);
	if (nwcount < 0) {
		printk(KERN_ERR"error in file write string: %d\n", nwcount);
		goto err1;
	}else {
		printk(KERN_ERR"file write string ok: %d\n", nwcount);
	}
	ret = 0;
	g_ep0fp = ep0fp;
	
	goto ret1;

err1:
	if (ep0fp!=NULL && IS_ERR(ep0fp)==0) {
		filp_close(ep0fp, NULL);
		ep0fp = NULL;
	}
	g_ep0fp = ep0fp;
	
ret1:
	set_fs(fs);
	return ret;
}
#endif

/*-------------------------------------------------------------------------*/
/* Supported functions initialization */

struct functionfs_config {
	bool opened;
	bool enabled;
	struct ffs_data *data;
};

static int ffs_function_init(struct infotm_usb_function *f,
			     struct usb_composite_dev *cdev)
{
	f->config = kzalloc(sizeof(struct functionfs_config), GFP_KERNEL);
	if (!f->config)
		return -ENOMEM;

	f->enum_no_ready = false; /* enable and connect */
	return functionfs_init();
}

static void ffs_function_cleanup(struct infotm_usb_function *f)
{
	functionfs_cleanup();
	kfree(f->config);
}

#if 0
static void ffs_function_enable(struct infotm_usb_function *f)
{
	struct infotm_dev *dev = _infotm_dev;
	struct functionfs_config *config = f->config;

	config->enabled = true;

	/* Disable the gadget until the function is ready */
	if (!config->opened)
		infotm_disable(dev);
}

static void ffs_function_disable(struct infotm_usb_function *f)
{
	struct infotm_dev *dev = _infotm_dev;
	struct functionfs_config *config = f->config;

	config->enabled = false;

	/* Balance the disable that was called in closed_callback */
	if (!config->opened)
		infotm_enable(dev);
}
#endif

static int ffs_function_bind_config(struct infotm_usb_function *f,
				    struct usb_configuration *c)
{
	struct functionfs_config *config = f->config;
	return functionfs_bind_config(c->cdev, c, config->data);
}

static ssize_t
ffs_aliases_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
	struct infotm_dev *dev = _infotm_dev;
	int ret;

	mutex_lock(&dev->mutex);
	ret = sprintf(buf, "%s\n", dev->ffs_aliases);
	mutex_unlock(&dev->mutex);

	return ret;
}

static ssize_t
ffs_aliases_store(struct device *pdev, struct device_attribute *attr,
					const char *buf, size_t size)
{
	struct infotm_dev *dev = _infotm_dev;
	char buff[256];

	mutex_lock(&dev->mutex);

	if (dev->enabled) {
		mutex_unlock(&dev->mutex);
		return -EBUSY;
	}

	strlcpy(buff, buf, sizeof(buff));
	strlcpy(dev->ffs_aliases, strim(buff), sizeof(dev->ffs_aliases));

	mutex_unlock(&dev->mutex);

	return size;
}

static DEVICE_ATTR(aliases, S_IRUGO | S_IWUSR, ffs_aliases_show,
					       ffs_aliases_store);


static ssize_t 
ffs_inject_desc_store(struct device *pdev, struct device_attribute *attr,
									const char *buff, size_t size) {
	struct infotm_dev *dev = _infotm_dev;
	int flag;
	int err;
	ssize_t ret = -1;
	mutex_lock(&dev->mutex);
	
#ifndef CONFIG_USB_FFS_INJECT_EARLY
	if (sscanf(buff, "%d\n", &flag)==1) {
		if (flag == 1) {
			err = ffs_force_inject();
			if (err >= 0) {
				ret = size;
			}
		} else if (flag == 0) {
			if (g_ep0fp!=NULL) {
				filp_close(g_ep0fp, NULL);
				g_ep0fp = NULL;
			}
		}
	}
#endif

	mutex_unlock(&dev->mutex);
	return size;
}


static ssize_t 
ffs_inject_desc_show(struct device *pdev, struct device_attribute *attr, char *buf){
	return 0;
}

static DEVICE_ATTR(inject_desc, S_IRUGO | S_IWUSR, ffs_inject_desc_show,
					       ffs_inject_desc_store);

static struct device_attribute *ffs_function_attributes[] = {
	&dev_attr_aliases,
	&dev_attr_inject_desc,
	NULL
};

static struct infotm_usb_function ffs_function = {
	.name		= "ffs",
	.init		= ffs_function_init,
	//.enable		= ffs_function_enable,
	//.disable	= ffs_function_disable,
	.cleanup	= ffs_function_cleanup,
	.bind_config	= ffs_function_bind_config,
	.attributes	= ffs_function_attributes,
};

static int functionfs_ready_callback(struct ffs_data *ffs)
{
	struct infotm_dev *dev = _infotm_dev;
	struct functionfs_config *config = ffs_function.config;
	int ret = 0;

	//mutex_lock(&dev->mutex);

	ret = functionfs_bind(ffs, dev->cdev);
	if (ret)
		goto err;

	config->data = ffs;
	config->opened = true;

	//if (config->enabled)
	//	infotm_enable(dev);

err:
	//mutex_unlock(&dev->mutex);
	return ret;
}

static void functionfs_closed_callback(struct ffs_data *ffs)
{
	struct infotm_dev *dev = _infotm_dev;
	struct functionfs_config *config = ffs_function.config;

	//mutex_lock(&dev->mutex);

	if (config->enabled)
		infotm_disable(dev);

	config->opened = false;
	config->data = NULL;

	functionfs_unbind(ffs);

	//mutex_unlock(&dev->mutex);
}

static void *functionfs_acquire_dev_callback(const char *dev_name)
{
	return 0;
}

static void functionfs_release_dev_callback(struct ffs_data *ffs_data)
{
}

static struct infotm_usb_function *supported_functions[] = {
	&uvc_function,
	&rndis_function,
	&mass_storage_function,
	&hid_function,
	&ffs_function,
	NULL
};

static int infotm_init_functions(struct infotm_usb_function **functions,
				  struct usb_composite_dev *cdev)
{
	struct infotm_dev *dev = _infotm_dev;
	struct infotm_usb_function *f;
	struct device_attribute **attrs;
	struct device_attribute *attr;
	int err;
	int index = 0;

	for (; (f = *functions++); index++) {
		f->dev_name = kasprintf(GFP_KERNEL, "f_%s", f->name);
		f->dev = device_create(infotm_class, dev->dev,
				MKDEV(0, index), f, f->dev_name);
		if (IS_ERR(f->dev)) {
			pr_err("%s: Failed to create dev %s", __func__,
							f->dev_name);
			err = PTR_ERR(f->dev);
			goto err_create;
		}

		if (f->init) {
			err = f->init(f, cdev);
			if (err) {
				pr_err("%s: Failed to init %s", __func__,
								f->name);
				goto err_out;
			}
		}

		attrs = f->attributes;
		if (attrs) {
			while ((attr = *attrs++) && !err)
				err = device_create_file(f->dev, attr);
		}
		if (err) {
			pr_err("%s: Failed to create function %s attributes",
					__func__, f->name);
			goto err_out;
		}
	}
	return 0;

err_out:
	device_destroy(infotm_class, f->dev->devt);
err_create:
	kfree(f->dev_name);
	return err;
}

static void infotm_cleanup_functions(struct infotm_usb_function **functions)
{
	struct infotm_usb_function *f;

	while (*functions) {
		f = *functions++;

		if (f->dev) {
			device_destroy(infotm_class, f->dev->devt);
			kfree(f->dev_name);
		}

		if (f->cleanup)
			f->cleanup(f);
	}
}

static int
infotm_bind_enabled_functions(struct infotm_dev *dev,
			       struct usb_configuration *c)
{
	struct infotm_usb_function *f;
	int ret;

	list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
		ret = f->bind_config(f, c);
		if (ret) {
			pr_err("%s: %s failed", __func__, f->name);
			return ret;
		}
	}
	return 0;
}

static void
infotm_unbind_enabled_functions(struct infotm_dev *dev,
			       struct usb_configuration *c)
{
	struct infotm_usb_function *f;

	list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
		if (f->unbind_config)
			f->unbind_config(f, c);
	}
}

static int infotm_enable_function(struct infotm_dev *dev, char *name)
{
	struct infotm_usb_function **functions = dev->functions;
	struct infotm_usb_function *f;
	while ((f = *functions++)) {
		if (!strcmp(name, f->name)) {
			list_add_tail(&f->enabled_list,
						&dev->enabled_functions);
			return 0;
		}
	}
	return -EINVAL;
}

/*-------------------------------------------------------------------------*/
/* /sys/class/infotm_usb/infotm%d/ interface */

static ssize_t
functions_show(struct device *pdev, struct device_attribute *attr, char *buf)
{
	struct infotm_dev *dev = dev_get_drvdata(pdev);
	struct infotm_usb_function *f;
	char *buff = buf;

	mutex_lock(&dev->mutex);

	list_for_each_entry(f, &dev->enabled_functions, enabled_list)
		buff += sprintf(buff, "%s,", f->name);

	mutex_unlock(&dev->mutex);

	if (buff != buf)
		*(buff-1) = '\n';
	return buff - buf;
}

static ssize_t
functions_store(struct device *pdev, struct device_attribute *attr,
			       const char *buff, size_t size)
{
	struct infotm_dev *dev = dev_get_drvdata(pdev);
	char *name;
	char buf[256], *b;
	int err;

	mutex_lock(&dev->mutex);

	if (dev->enabled) {
		mutex_unlock(&dev->mutex);
		return -EBUSY;
	}

	INIT_LIST_HEAD(&dev->enabled_functions);

	strlcpy(buf, buff, sizeof(buf));
	b = strim(buf);

	while (b) {
		name = strsep(&b, ",");
		if (!name)
			continue;
		err = infotm_enable_function(dev, name);
		if (err)
			pr_err("infotm_usb: Cannot enable '%s' (%d)",
							   name, err);
	}

	mutex_unlock(&dev->mutex);

	return size;
}

static ssize_t enable_show(struct device *pdev, struct device_attribute *attr,
			   char *buf)
{
	struct infotm_dev *dev = dev_get_drvdata(pdev);
	return sprintf(buf, "%d\n", dev->enabled);
}

static ssize_t enable_store(struct device *pdev, struct device_attribute *attr,
			    const char *buff, size_t size)
{
	struct infotm_dev *dev = dev_get_drvdata(pdev);
	struct usb_composite_dev *cdev = dev->cdev;
	struct infotm_usb_function *f;
	int enabled = 0;
	bool enum_ready = true;


	if (!cdev)
		return -ENODEV;

	mutex_lock(&dev->mutex);

	sscanf(buff, "%d", &enabled);
	if (enabled && !dev->enabled) {
		/*
		 * Update values in composite driver's copy of
		 * device descriptor.
		 */
		cdev->desc.idVendor = device_desc.idVendor;
		cdev->desc.idProduct = device_desc.idProduct;
		cdev->desc.bcdDevice = device_desc.bcdDevice;
		cdev->desc.bDeviceClass = device_desc.bDeviceClass;
		cdev->desc.bDeviceSubClass = device_desc.bDeviceSubClass;
		cdev->desc.bDeviceProtocol = device_desc.bDeviceProtocol;
		list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
			if (f->enable)
				f->enable(f);
			enum_ready &= ~f->enum_no_ready; /* fiexed uvc enum bug */
		}
		infotm_enable(dev, enum_ready);
		dev->enabled = true;
	} else if (!enabled && dev->enabled) {
		infotm_disable(dev);
		list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
			if (f->disable)
				f->disable(f);
		}
		dev->enabled = false;
	} else {
		pr_err("infotm_usb: already %s\n",
				dev->enabled ? "enabled" : "disabled");
	}

	mutex_unlock(&dev->mutex);
	return size;
}

static ssize_t state_show(struct device *pdev, struct device_attribute *attr,
			   char *buf)
{
	struct infotm_dev *dev = dev_get_drvdata(pdev);
	struct usb_composite_dev *cdev = dev->cdev;
	char *state = "DISCONNECTED";
	unsigned long flags;

	if (!cdev)
		goto out;

	spin_lock_irqsave(&cdev->lock, flags);
	if (cdev->config)
		state = "CONFIGURED";
	else if (dev->connected)
		state = "CONNECTED";
	spin_unlock_irqrestore(&cdev->lock, flags);
out:
	return sprintf(buf, "%s\n", state);
}

#define DESCRIPTOR_ATTR(field, format_string)				\
static ssize_t								\
field ## _show(struct device *dev, struct device_attribute *attr,	\
		char *buf)						\
{									\
	return sprintf(buf, format_string, device_desc.field);		\
}									\
static ssize_t								\
field ## _store(struct device *dev, struct device_attribute *attr,	\
		const char *buf, size_t size)				\
{									\
	int value;							\
	if (sscanf(buf, format_string, &value) == 1) {			\
		device_desc.field = value;				\
		return size;						\
	}								\
	return -1;							\
}									\
static DEVICE_ATTR(field, S_IRUGO | S_IWUSR, field ## _show, field ## _store);

#define DESCRIPTOR_STRING_ATTR(field, buffer)				\
static ssize_t								\
field ## _show(struct device *dev, struct device_attribute *attr,	\
		char *buf)						\
{									\
	return sprintf(buf, "%s", buffer);				\
}									\
static ssize_t								\
field ## _store(struct device *dev, struct device_attribute *attr,	\
		const char *buf, size_t size)				\
{									\
	if (size >= sizeof(buffer))					\
		return -EINVAL;						\
	return strlcpy(buffer, buf, sizeof(buffer));			\
}									\
static DEVICE_ATTR(field, S_IRUGO | S_IWUSR, field ## _show, field ## _store);


DESCRIPTOR_ATTR(idVendor, "%04x\n")
DESCRIPTOR_ATTR(idProduct, "%04x\n")
DESCRIPTOR_ATTR(bcdDevice, "%04x\n")
DESCRIPTOR_ATTR(bDeviceClass, "%d\n")
DESCRIPTOR_ATTR(bDeviceSubClass, "%d\n")
DESCRIPTOR_ATTR(bDeviceProtocol, "%d\n")
DESCRIPTOR_STRING_ATTR(iManufacturer, manufacturer_string)
DESCRIPTOR_STRING_ATTR(iProduct, product_string)
DESCRIPTOR_STRING_ATTR(iSerial, serial_string)

static DEVICE_ATTR(functions, S_IRUGO | S_IWUSR, functions_show,
						 functions_store);
static DEVICE_ATTR(enable, S_IRUGO | S_IWUSR, enable_show, enable_store);
static DEVICE_ATTR(state, S_IRUGO, state_show, NULL);

static struct device_attribute *infotm_usb_attributes[] = {
	&dev_attr_idVendor,
	&dev_attr_idProduct,
	&dev_attr_bcdDevice,
	&dev_attr_bDeviceClass,
	&dev_attr_bDeviceSubClass,
	&dev_attr_bDeviceProtocol,
	&dev_attr_iManufacturer,
	&dev_attr_iProduct,
	&dev_attr_iSerial,
	&dev_attr_functions,
	&dev_attr_enable,
	&dev_attr_state,
	NULL
};

/*-------------------------------------------------------------------------*/
/* Composite driver */

static int infotm_bind_config(struct usb_configuration *c)
{
	struct infotm_dev *dev = _infotm_dev;
	int ret = 0;

	ret = infotm_bind_enabled_functions(dev, c);
	if (ret)
		return ret;

	return 0;
}

static void infotm_unbind_config(struct usb_configuration *c)
{
	struct infotm_dev *dev = _infotm_dev;

	infotm_unbind_enabled_functions(dev, c);
}

static int infotm_bind(struct usb_composite_dev *cdev)
{
	struct infotm_dev *dev = _infotm_dev;
	struct usb_gadget	*gadget = cdev->gadget;
	int			id, ret;

	/*
	 * Start disconnected. Userspace will connect the gadget once
	 * it is done configuring the functions.
	 */
	usb_gadget_disconnect(gadget);

	ret = infotm_init_functions(dev->functions, cdev);
	if (ret)
		return ret;

	/* Allocate string descriptor numbers ... note that string
	 * contents can be overridden by the composite_dev glue.
	 */
	id = usb_string_id(cdev);
	if (id < 0)
		return id;
	strings_dev[STRING_MANUFACTURER_IDX].id = id;
	device_desc.iManufacturer = id;

	id = usb_string_id(cdev);
	if (id < 0)
		return id;
	strings_dev[STRING_PRODUCT_IDX].id = id;
	device_desc.iProduct = id;

	/* Default strings - should be updated by userspace */
	strncpy(manufacturer_string, "Infotm", sizeof(manufacturer_string)-1);
	strncpy(product_string, "Intotm", sizeof(product_string) - 1);
	strncpy(serial_string, "0123456789ABCDEF", sizeof(serial_string) - 1);

	id = usb_string_id(cdev);
	if (id < 0)
		return id;
	strings_dev[STRING_SERIAL_IDX].id = id;
	device_desc.iSerialNumber = id;

	usb_gadget_set_selfpowered(gadget);
	dev->cdev = cdev;

	return 0;
}

static int infotm_usb_unbind(struct usb_composite_dev *cdev)
{
	struct infotm_dev *dev = _infotm_dev;

	cancel_work_sync(&dev->work);
	infotm_cleanup_functions(dev->functions);
	return 0;
}

/* HACK: infotm needs to override setup for accessory to work */
static int (*composite_setup_func)(struct usb_gadget *gadget, const struct usb_ctrlrequest *c);

static int
infotm_setup(struct usb_gadget *gadget, const struct usb_ctrlrequest *c)
{
	struct infotm_dev		*dev = _infotm_dev;
	struct usb_composite_dev	*cdev = get_gadget_data(gadget);
	struct usb_request		*req = cdev->req;
	struct infotm_usb_function	*f;
	int value = -EOPNOTSUPP;
	unsigned long flags;

	req->zero = 0;
	req->length = 0;
	gadget->ep0->driver_data = cdev;

	list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
		if (f->ctrlrequest) {
			value = f->ctrlrequest(f, cdev, c);
			if (value >= 0)
				break;
		}
	}

	if (value < 0)
		value = composite_setup_func(gadget, c);

	spin_lock_irqsave(&cdev->lock, flags);
	if (!dev->connected) {
		dev->connected = 1;
		schedule_work(&dev->work);
	} else if (c->bRequest == USB_REQ_SET_CONFIGURATION &&
						cdev->config) {
		schedule_work(&dev->work);
	}
	spin_unlock_irqrestore(&cdev->lock, flags);

	return value;
}

static void infotm_disconnect(struct usb_composite_dev *cdev)
{
	struct infotm_dev *dev = _infotm_dev;
	dev->connected = 0;
	schedule_work(&dev->work);
}

static struct usb_composite_driver infotm_usb_driver = {
	.name		= "infotm_usb",
	.dev		= &device_desc,
	.strings	= dev_strings,
	.bind		= infotm_bind,
	.unbind		= infotm_usb_unbind,
	.disconnect	= infotm_disconnect,
	.max_speed	= USB_SPEED_HIGH,
};

static int infotm_create_device(struct infotm_dev *dev)
{
	struct device_attribute **attrs = infotm_usb_attributes;
	struct device_attribute *attr;
	int err;

	dev->dev = device_create(infotm_class, NULL,
					MKDEV(0, 0), NULL, "infotm0");
	if (IS_ERR(dev->dev))
		return PTR_ERR(dev->dev);

	dev_set_drvdata(dev->dev, dev);

	while ((attr = *attrs++)) {
		err = device_create_file(dev->dev, attr);
		if (err) {
			device_destroy(infotm_class, dev->dev->devt);
			return err;
		}
	}
	return 0;
}

void ffs_force_init_flow(void) {
	struct infotm_dev *dev = _infotm_dev;
	struct usb_composite_dev *cdev = dev->cdev;
	struct infotm_usb_function *f;
	bool enum_ready = true;
	int err;

	if (!cdev) {
		pr_err("infotm_usb: cdev is null");
		return ;
	}
	mutex_lock(&dev->mutex);
	err = ffs_force_inject();
	if (err < 0) {
		mutex_unlock(&dev->mutex);
		return;
	}
	err = infotm_enable_function(dev, "ffs");
	if (err) {
		pr_err("infotm_usb: Cannot enable '%s' (%d)", "ffs", err);
		mutex_unlock(&dev->mutex);
		return ;
	}

	if (!dev->enabled) {
		/*
		 * Update values in composite driver's copy of
		 * device descriptor.
		 */
		cdev->desc.idVendor = device_desc.idVendor;
		cdev->desc.idProduct = device_desc.idProduct;
		cdev->desc.bcdDevice = device_desc.bcdDevice;
		cdev->desc.bDeviceClass = device_desc.bDeviceClass;
		cdev->desc.bDeviceSubClass = device_desc.bDeviceSubClass;
		cdev->desc.bDeviceProtocol = device_desc.bDeviceProtocol;
		list_for_each_entry(f, &dev->enabled_functions, enabled_list) {
			if (f->enable)
				f->enable(f);
			enum_ready &= ~f->enum_no_ready; /* fiexed uvc enum bug */
		}
		infotm_enable(dev, enum_ready);
		dev->enabled = true;
	} 
	mutex_unlock(&dev->mutex);
}

static int __init init(void)
{
	struct infotm_dev *dev;
	int err;

	infotm_class = class_create(THIS_MODULE, "infotm_usb");
	if (IS_ERR(infotm_class))
		return PTR_ERR(infotm_class);

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		err = -ENOMEM;
		goto err_dev;
	}

	dev->disable_depth = 1;
	dev->functions = supported_functions;
	INIT_LIST_HEAD(&dev->enabled_functions);
	INIT_WORK(&dev->work, infotm_work);
	mutex_init(&dev->mutex);

	err = infotm_create_device(dev);
	if (err) {
		pr_err("%s: failed to create infotm device %d", __func__, err);
		goto err_create;
	}

	_infotm_dev = dev;

	err = usb_composite_probe(&infotm_usb_driver);
	if (err) {
		pr_err("%s: failed to probe driver %d", __func__, err);
		goto err_probe;
	}

	/* HACK: exchange composite's setup with ours */
	composite_setup_func = infotm_usb_driver.gadget_driver.setup;
	infotm_usb_driver.gadget_driver.setup = infotm_setup;
#ifdef CONFIG_USB_FFS_INJECT_EARLY
	ffs_force_init_flow();
#endif

	return 0;

err_probe:
	device_destroy(infotm_class, dev->dev->devt);
err_create:
	kfree(dev);
err_dev:
	class_destroy(infotm_class);
	return err;
}
module_init(init);

static void __exit cleanup(void)
{
	usb_composite_unregister(&infotm_usb_driver);
	class_destroy(infotm_class);
	kfree(_infotm_dev);
	_infotm_dev = NULL;
}
module_exit(cleanup);
