/*
 * mipi-csi-receiver-interface.h
 * INFOTM MIPI CSI
 */
#ifndef INFOTM_MIPI_CSI_RECEIVER_H_
#define INFOTM_MIPI_CSI_RECEIVER_H_

#define CSI_DRIVER_NAME			"infotm-mipi-csi-receiver"
#define CSI_SUBDEV_NAME			CSI_DRIVER_NAME
#define CSI_MAX_ENTITIES		2
#define CSI0_MAX_LANES			4
#define CSI1_MAX_LANES			2

#define CSI_PAD_SINK			0
#define CSI_PAD_SOURCE			1
#define CSI_PADS_NUM			2

#define INFOTMCSI_DEF_PIX_WIDTH		640
#define INFOTMCSI_DEF_PIX_HEIGHT	480

#endif
