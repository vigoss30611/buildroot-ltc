/*********************************************************************************
@file sc1135dvp.c

@brief SC1135 camera sensor implementation

@copyright InfoTM Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
InfoTM Limited.

@author:  feng.qu@infotm.com	2016.8.12

******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>

#include <img_types.h>
#include <img_errors.h>
#include <ci/ci_api_structs.h>
#include <felixcommon/userlog.h>

#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"


/* Sensor specific configuration */
#include "sensors/sc1135dvp.h"
#define SNAME		"SC1135"
#define LOG_TAG	SNAME
#define SENSOR_I2C_ADDR			0x30			// in 7-bits
#define SENSOR_BAYER_FORMAT		MOSAIC_BGGR
#define SENSOR_MAX_GAIN			60
#define SENSOR_GAIN_REG_ADDR			0x3e09
#define SENSOR_DIGITAL_GAIN_REG_ADDR	0x3e08
#define SENSOR_EXPOSURE_REG_ADDR_H	0x3e01
#define SENSOR_EXPOSURE_REG_ADDR_L	0x3e02
#define SENSOR_EXPOSURE_DEFAULT		31111
#define SENSOR_MAX_FRAME_TIME			100000		// in us. used for delay. consider the lowest fps
#define SENSOR_FRAME_LENGTH_H		0x320e
#define SENSOR_FRAME_LENGTH_L		0x320f
#define SENSOR_GAIN_STEP				0.05883
#define DEFAULT_USE_AEC_AGC			0



#define SENSOR_VERSION_NOT_VERIFYED	"not-verified"

#define ARRAY_SIZE(n)		(sizeof(n)/sizeof(n[0]))

#define SENSOR_TUNNING		0
#define TUNE_SLEEP(n)	{if (SENSOR_TUNNING) sleep(n);}

/* Assert */
#define ASSERT_INITIALIZED(p)		\
{if (!p) {LOG_ERROR("Sensor not initialised\n");return IMG_ERROR_NOT_INITIALISED;}}
#define ASSERT_MODE_RANGE(n)		\
{if (n >= mode_num) {LOG_ERROR("Invalid mode_id %d, there is only %d modes\n", n, mode_num);return IMG_ERROR_INVALID_ID;}}


//#undef LOG_DEBUG
//#define LOG_DEBUG(...)    LOG_INFO_TAG(LOG_TAG, __VA_ARGS__)

/* SmartSens sc1135 denoise logic */
#define NOISE_LOGIC_ENABLE

typedef struct _sensor_cam
{
	SENSOR_FUNCS funcs;
	SENSOR_PHY *sensor_phy;
	int i2c;

	/* Sensor status */
	IMG_BOOL enabled;			// in using or not
	IMG_UINT16 mode_id;		// index of current mode
	IMG_UINT8 flipping;
	IMG_UINT32 exposure;
	IMG_UINT32 expo_reg_val;		// current exposure register value
	double gain;
	IMG_UINT16 coarse_gain_reg_val;		// current coarse gain register value
	IMG_UINT16 fine_gain_reg_val;		// current fine gain register value
	IMG_UINT16 digital_gain_reg_val;		// current digital gain register value
	double cur_fps;

	/* Sensor config params */
	IMG_UINT8 imager;			// 0: MIPI0   1: MIPI1   2: DVP
	IMG_UINT8 aec_agc;		// Use sensor AEC & AGC function

}SENSOR_CAM_STRUCT;

/* Predefined sensor mode */
typedef struct _sensor_mode
{
	SENSOR_MODE mode;
	IMG_UINT8 hsync;
	IMG_UINT8 vsync;
	IMG_UINT32 line_time;		// in ns. for exposure
	IMG_UINT16 *registers;
	IMG_UINT32 reg_num;

} SENSOR_MODE_STRUCT;

static IMG_UINT16 registers_base[] = {
	0x3000,0x01, //manualstreamenbale
	0x3003,0x01, //softreset
	0x3400,0x53,
	0x3416,0xc0,
	0x3d08,0x03,
	0x3e03,0x03,
	0x3928,0x00,
	0x3630,0x58,
	0x3612,0x00,
	0x3632,0x41,
	0x3635,0x00, //20160328
	0x3620,0x44,
	0x3633,0x7f, //20160422
	0x3780,0x0b,
	0x3300,0x33,
	0x3301,0x38,
	0x3302,0x30,
	0x3303,0x80, //20160307B  20160412
	0x3304,0x18,
	0x3305,0x72,
	0x331e,0x30, //20160608 
	0x321e,0x00,
	0x321f,0x0a,
	0x3216,0x0a,
	0x3332,0x38,
	0x5054,0x82,
	0x3622,0x26,
	0x3907,0x02,
	0x3908,0x00,
	0x3601,0x1a, //20160422
	0x3315,0x44,
	0x3308,0x40,
	0x3223,0x22, //vysncmode[5]
	0x3e0e,0x50,
	/*DPC*/
	0x3211,0x60,
	0x5780,0xff,
	0x5781,0x04, //20160328
	0x5785,0x0c, //20160328
	0x5000,0x66,
	0x3e0f,0x90,	
	0x3631,0x80,
	0x3310,0x83,
	0x3336,0x01,
	0x3337,0x00,
	0x3338,0x03,
	0x3339,0xe8,
	0x3335,0x06, //20160418
	0x3880,0x00, 
	//power reduction 20160606
	0x3620,0x42,
	0x3610,0x03,
	0x3600,0x64,
	0x3636,0x0d,
	0x3323,0x80,
};

static IMG_UINT16 registers_720p_30fps[] = {
	/*27Minput54Moutputpixelclockfrequency*/
	0x3010,0x07,
	0x3011,0x46,
	0x3004,0x04,
	//0x3610,0x2b, //201600606
	
	/*configFramelengthandwidth*/
	0x320c,0x07, //1800
	0x320d,0x08, //1000
	0x320e,0x03,
	0x320f,0xe8,
	
	/*configOutputwindowposition*/
	0x3210,0x00,
	0x3211,0x60,
	0x3212,0x00,
	0x3213,0x80,  //for BGGR out format 20160412
	
	/*configOutputimagesize*/
	0x3208,0x05,
	0x3209,0x00,
	0x320a,0x02,
	0x320b,0xd0,
	
	/*configFramestartphysicalposition*/
	0x3202,0x00,
	0x3203,0x08,
	0x3206,0x03,
	0x3207,0xcf,
	
	/*powerconsumptionreduction*/
	0x3330,0x0d,
	0x3320,0x06,
	0x3321,0xd8,
	0x3322,0x01,
	//0x3323,0x80, //20160606
	//0x3600,0x54, //20160606

	0x3000,0x00, // sleep
};

static IMG_UINT16 registers_960p_30fps[] = {
	/*27Minput54Moutputpixelclockfrequency*/
	0x3010,0x07,
	0x3011,0x46,
	0x3004,0x04,
	//0x3610,0x2b,	//20160606
	
	/*configFramelengthandwidth*/
	0x320c,0x07, //1800
	0x320d,0x08, //1000
	0x320e,0x03,
	0x320f,0xe8,
	
	/*configOutputwindowposition*/
	0x3210,0x00,
	0x3211,0x60,
	0x3212,0x00,
	0x3213,0x04, //for BGGR out format 20160412
	
	/*configOutputimagesize*/
	0x3208,0x05,
	0x3209,0x00,
	0x320a,0x03,
	0x320b,0xc0,
	
	/*configFramestartphysicalposition*/
	0x3202,0x00,
	0x3203,0x08,
	0x3206,0x03,
	0x3207,0xcf,
	
	/*powerconsumptionreduction*/
	0x3330,0x0d,
	0x3320,0x06,
	0x3321,0xd8,
	0x3322,0x01,
	//0x3323,0x80, //20160606
	//0x3600,0x54, //20160606

	0x3000,0x00, // sleep
};

static SENSOR_MODE_STRUCT sensor_modes[] = {
	{{10, 1280, 960, 30, 54, 1800, 1000, SENSOR_FLIP_BOTH, 33, 33333}, 1, 1, 33333, 
		registers_960p_30fps, ARRAY_SIZE(registers_960p_30fps)},
	{{10, 1280, 720, 30, 54, 1800, 1000, SENSOR_FLIP_BOTH, 33, 33333}, 1, 1, 33333, 
		registers_720p_30fps, ARRAY_SIZE(registers_720p_30fps)},
};
static IMG_UINT8 mode_num = 0;

/*==============================================================================*/


static IMG_RESULT sensor_i2c_write(int i2c, IMG_UINT16 reg, IMG_UINT8 data)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[1];
	unsigned char buf[3] = {0};
	
	buf[0] = (reg>>8) & 0xFF;
	buf[1] = reg & 0xFF;
	buf[2] = data;

	messages[0].addr  = SENSOR_I2C_ADDR;
	messages[0].flags = 0;
	messages[0].len   = 3;
	messages[0].buf   = buf;

	packets.msgs = messages;
	packets.nmsgs = 1;

	if(ioctl(i2c, I2C_RDWR, &packets) < 0) {
		LOG_ERROR("Unable to write reg 0x%04x with data 0x%02x.\n", reg, data);
		return IMG_ERROR_FATAL;
	}
	
	//printf("write ->[0x%04x] = 0x%02x\n", reg, data);

	return IMG_SUCCESS;
}

static IMG_RESULT sensor_i2c_read(int i2c, IMG_UINT16 reg, IMG_UINT8 *data)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];
	unsigned char buf[2];
	
	buf[0] = (reg >> 8) & 0xFF;
	buf[1] = reg & 0xFF;

	messages[0].addr = SENSOR_I2C_ADDR;
	messages[0].flags = 0;
	messages[0].len = 2;
	messages[0].buf = buf;

	messages[1].addr = SENSOR_I2C_ADDR;
	messages[1].flags = I2C_M_RD;
	messages[1].len = 1;
	messages[1].buf = data;

	packets.msgs = messages;
	packets.nmsgs = 2;

	if(ioctl(i2c, I2C_RDWR, &packets) < 0) {
		LOG_ERROR("Unable to read reg 0x%04X.\n", reg);
		return IMG_ERROR_FATAL;
	}

	//printf("read  <-[0x%04x] = 0x%02x\n", reg,  *data);

	return IMG_SUCCESS;
}

static IMG_RESULT sensor_config_register(int i2c, IMG_UINT16 mode_id)
{
	IMG_UINT16 *registers;
	IMG_UINT32 reg_num;
	IMG_RESULT ret;
	int i;

	ASSERT_MODE_RANGE(mode_id);

	registers = registers_base;
	reg_num = ARRAY_SIZE(registers_base);

	for (i = 0; i < reg_num; i+=2) {
		ret = sensor_i2c_write(i2c, registers[i], registers[i+1]);
		if (ret != IMG_SUCCESS) {
			LOG_ERROR("Set base mode failed\n");
			return ret;
		}
	}

	registers = sensor_modes[mode_id].registers;
	reg_num = sensor_modes[mode_id].reg_num;

	LOG_INFO("mode_id=%d reg_num=%d >>>>>>>>>>>>\n", mode_id, reg_num);

	for (i = 0; i < reg_num; i+=2) {
		ret = sensor_i2c_write(i2c, registers[i], registers[i+1]);
		if (ret != IMG_SUCCESS) {
			LOG_ERROR("Set mode %d failed\n", mode_id);
			return ret;
		}
	}

	return IMG_SUCCESS;
}

#if 0
static IMG_RESULT sensor_verify_register(int i2c, IMG_UINT16 mode_id)
{
	IMG_UINT16 *registers;
	IMG_UINT32 reg_num;
	IMG_UINT8 value;
	IMG_RESULT ret;
	int i, diff = 0;

	ASSERT_MODE_RANGE(mode_id);

	registers = sensor_modes[mode_id].registers;
	reg_num = sensor_modes[mode_id].reg_num;

	LOG_INFO("@Checking registers comparing to mode %d, reg_num=%d ...\n", mode_id, reg_num);

	for (i = 0; i < reg_num; i+=2) {
		ret = sensor_i2c_read(i2c, registers[i], &value);
		if (ret != IMG_SUCCESS) {
			LOG_ERROR("Verify mode %d failed\n", mode_id);
			return ret;
		}
		if (value != (IMG_UINT8)registers[i+1]) {
			diff = 1;
			LOG_INFO("@Check failed! reg[0x%04x]=0x%02x, but initial value is 0x%02x\n", registers[i], value, registers[i+1]);
		}
	}

	if (!diff)
		LOG_INFO("@Checking PASS! Registers are all equal!\n");

	return IMG_SUCCESS;
}
#endif

static IMG_RESULT sensor_aec_agc_ctrl(int i2c, IMG_UINT8 enable)
{
	if (enable)
		sensor_i2c_write(i2c, 0x3e03, 0x00);
	else
		sensor_i2c_write(i2c, 0x3e03, 0x03);
	return IMG_SUCCESS;
}

static IMG_RESULT sensor_enable_ctrl(int i2c, IMG_UINT8 enable)
{
	/* Sensor specific operation */
	if (enable) {	
		sensor_i2c_write(i2c, 0x3000, 0x01);
		usleep(2000);
	}
	else {
		sensor_i2c_write(i2c, 0x3000, 0x00);
		usleep(2000);
	}
	return IMG_SUCCESS;
}

static IMG_RESULT sensor_flip_image(SENSOR_CAM_STRUCT * psCam, IMG_UINT8 hflip, IMG_UINT8 vflip)
{
	IMG_UINT8 buf;
	int i2c = psCam->i2c;
	LOG_INFO("hflip=%d, vflip=%d\n", hflip, vflip);

	if (hflip) {
		sensor_i2c_write(i2c, 0x321d, 0x01);
		sensor_i2c_write(i2c, 0x3400, 0x52);
		sensor_i2c_write(i2c, 0x3907, 0x00);
		sensor_i2c_write(i2c, 0x3908, 0xc0);
		sensor_i2c_write(i2c, 0x3781, 0x08);
		if (sensor_modes[psCam->mode_id].mode.ui16Height == 720)
			sensor_i2c_write(i2c, 0x3211, 0x07);
		else if (sensor_modes[psCam->mode_id].mode.ui16Height == 960)
			sensor_i2c_write(i2c, 0x3211, 0x0d);
		else
			LOG_ERROR("Invalid mode %d Height %d\n", psCam->mode_id, 
					sensor_modes[psCam->mode_id].mode.ui16Height);
	}
	else {
		sensor_i2c_write(i2c, 0x321d, 0x00);
		sensor_i2c_write(i2c, 0x3400, 0x53);
		sensor_i2c_write(i2c, 0x3907, 0x02);
		sensor_i2c_write(i2c, 0x3908, 0x00);
		sensor_i2c_write(i2c, 0x3781, 0x10);
		sensor_i2c_write(i2c, 0x3211, 0x60);
	}

	if (vflip) {
		sensor_i2c_write(i2c, 0x321c, 0x40);
		sensor_i2c_write(i2c, 0x3213, 0x07);
	}
	else {
		sensor_i2c_write(i2c, 0x321c, 0x00);
		if (sensor_modes[psCam->mode_id].mode.ui16Height == 720)
			sensor_i2c_write(i2c, 0x3213, 0x80);
		else if (sensor_modes[psCam->mode_id].mode.ui16Height == 960)
			sensor_i2c_write(i2c, 0x3213, 0x04);
		else
			LOG_ERROR("Invalid mode %d Height %d\n", psCam->mode_id, 
					sensor_modes[psCam->mode_id].mode.ui16Height);
	}

	return IMG_SUCCESS;
}

static IMG_UINT32 sensor_calculate_gain(double flGain, 
		IMG_UINT32 *coarse_gain, IMG_UINT32 *fine_gain, IMG_UINT32 *digital_gain)
{
	IMG_UINT32 digital = 1, coarse = 1, fine, i = 0;
	double gain;

	while (flGain > 15) {
		digital *= 2;
		flGain = flGain /digital;
		if (digital == 4)
			break;
	}

	while (flGain >= coarse) {
		i++;
		coarse = 1 << i;
	}
	i--;
	coarse = 1 << i;
	gain = flGain / coarse;
	fine = round((gain - 1)/SENSOR_GAIN_STEP);
	if (fine > 15)
		fine = 15;

	*digital_gain = digital;
	*coarse_gain = coarse;
	*fine_gain = fine;
	return 0;
}

static IMG_UINT32 sensor_calculate_exposure(SENSOR_CAM_STRUCT *psCam, IMG_UINT32 expo_time)
{
	IMG_UINT32 exposure_lines;
	double ratio = (double)sensor_modes[psCam->mode_id].mode.flFrameRate / psCam->cur_fps;
	IMG_UINT32 max_lines = sensor_modes[psCam->mode_id].mode.ui16VerticalTotal * ratio;
	
	exposure_lines = expo_time * 1000/sensor_modes[psCam->mode_id].line_time;
	if (exposure_lines < 1)
		exposure_lines = 1;
	if (exposure_lines > max_lines)
		LOG_WARNING("SC1135 limitation: Exposure lines Must NOT exceed frame length\n");
	if (exposure_lines >= max_lines -4)
		exposure_lines = max_lines -5;
	
	return exposure_lines;
}

static void sensor_write_gain(SENSOR_CAM_STRUCT *psCam, 
		IMG_UINT32 coarse_gain, IMG_UINT32 fine_gain, IMG_UINT32 digital_gain)
{
	if (coarse_gain == psCam->coarse_gain_reg_val
		&& fine_gain == psCam->fine_gain_reg_val
		&& digital_gain == psCam->digital_gain_reg_val)
		return;
	sensor_i2c_write(psCam->i2c, SENSOR_GAIN_REG_ADDR, ((coarse_gain -1)&0x07)<<4 | (fine_gain&0x0F));
	sensor_i2c_write(psCam->i2c, SENSOR_DIGITAL_GAIN_REG_ADDR, (digital_gain -1) & 0x03);
	psCam->coarse_gain_reg_val = coarse_gain;
	psCam->fine_gain_reg_val = fine_gain;
	psCam->digital_gain_reg_val = digital_gain;
}

static void sensor_write_exposure(SENSOR_CAM_STRUCT *psCam, IMG_UINT32 exposure_lines)
{
	if (exposure_lines == psCam->expo_reg_val)
		return;
	sensor_i2c_write(psCam->i2c, SENSOR_EXPOSURE_REG_ADDR_H, (exposure_lines >> 4) & 0xFF);
	sensor_i2c_write(psCam->i2c, SENSOR_EXPOSURE_REG_ADDR_L, (exposure_lines<<4) & 0xFF);
	psCam->expo_reg_val = exposure_lines;
}

#ifdef NOISE_LOGIC_ENABLE
static void noise_logic_gain(SENSOR_CAM_STRUCT *psCam, double flGain)
{
	if (flGain < 2)
		sensor_i2c_write(psCam->i2c, 0x3630, 0xb8);
	else
		sensor_i2c_write(psCam->i2c, 0x3630, 0x70);

	if (flGain < 2)
		sensor_i2c_write(psCam->i2c, 0x3631, 0x82);
	else if (flGain >= 2 && flGain <= 16)
		sensor_i2c_write(psCam->i2c, 0x3631, 0x8e);
	else
		sensor_i2c_write(psCam->i2c, 0x3631, 0x8c);

}
#endif

static IMG_RESULT sGetMode(SENSOR_HANDLE hHandle, IMG_UINT16 mode_id, SENSOR_MODE *smode)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	LOG_INFO("mode_id=%d >>>>>>>>>>>>\n", mode_id);
	TUNE_SLEEP(1);
	ASSERT_INITIALIZED(psCam->sensor_phy);
	IMG_ASSERT(smode);
	ASSERT_MODE_RANGE(mode_id);
	IMG_MEMCPY(smode, &sensor_modes[mode_id].mode, sizeof(SENSOR_MODE));
	return IMG_SUCCESS;
}

static IMG_RESULT sGetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);

	LOG_INFO(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);
	IMG_ASSERT(psStatus);

	if (!psCam->sensor_phy) {
		LOG_WARNING("sensor not initialised\n");
		psStatus->eState = SENSOR_STATE_UNINITIALISED;
		psStatus->ui16CurrentMode = 0;
	}
	else {
		psStatus->eState = (psCam->enabled ? SENSOR_STATE_RUNNING : SENSOR_STATE_IDLE);
		psStatus->ui16CurrentMode = psCam->mode_id;
		psStatus->ui8Flipping = psCam->flipping;
#if defined(INFOTM_ISP)
		psStatus->fCurrentFps = psCam->cur_fps;
#endif
	}
	return IMG_SUCCESS;
}

static IMG_RESULT sSetMode(SENSOR_HANDLE hHandle, IMG_UINT16 mode_id, IMG_UINT8 flipping)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	int i;
	IMG_RESULT ret;

	LOG_INFO("mode=%d, flipping=0x%02X >>>>>>>>>>>>\n", mode_id, flipping);
	TUNE_SLEEP(1);

	ASSERT_INITIALIZED(psCam->sensor_phy);
	ASSERT_MODE_RANGE(mode_id);

	if(flipping& sensor_modes[mode_id].mode.ui8SupportFlipping != flipping) {
		LOG_ERROR("Sensor mode %d does not support selected flipping 0x%x (supports 0x%x)\n", 
			mode_id, flipping, sensor_modes[mode_id].mode.ui8SupportFlipping);
		return IMG_ERROR_NOT_SUPPORTED;
	}

	/* Config registers */
	ret = sensor_config_register(psCam->i2c, mode_id);
	if (ret)
		goto out_failed;
	usleep(2000);
#if 0
	ret = sensor_verify_register(psCam->i2c, mode_id);
	if (ret)
		goto out_failed;
	usleep(2000);
#endif
	/* Config sensor AEC/AGC ctrl */
	sensor_aec_agc_ctrl(psCam->i2c, psCam->aec_agc);

	/* Config flipping */
	ret = sensor_flip_image(psCam, flipping & SENSOR_FLIP_HORIZONTAL, flipping & SENSOR_FLIP_VERTICAL);
	if (ret)
		goto out_failed;

	/* Init sensor status */
	psCam->enabled = IMG_FALSE;
	psCam->mode_id = mode_id;
	psCam->flipping = flipping;
	psCam->exposure = SENSOR_EXPOSURE_DEFAULT;
	psCam->gain = 1;
	psCam->coarse_gain_reg_val = 0;
	psCam->fine_gain_reg_val = 0;
	psCam->cur_fps = sensor_modes[mode_id].mode.flFrameRate;

	/* Init ISP gasket params */
	psCam->sensor_phy->psGasket->uiGasket  = psCam->imager;
	psCam->sensor_phy->psGasket->bParallel = IMG_TRUE;
	psCam->sensor_phy->psGasket->uiWidth   = sensor_modes[mode_id].mode.ui16Width-1;
	psCam->sensor_phy->psGasket->uiHeight  = sensor_modes[mode_id].mode.ui16Height-1;
	psCam->sensor_phy->psGasket->bVSync    = sensor_modes[mode_id].vsync;
	psCam->sensor_phy->psGasket->bHSync    = sensor_modes[mode_id].hsync;
	psCam->sensor_phy->psGasket->ui8ParallelBitdepth = sensor_modes[mode_id].mode.ui8BitDepth;

	LOG_DEBUG("gasket=%d, width=%d, height=%d, vsync=%d, hsync=%d,bitdepth=%d\n",
		psCam->sensor_phy->psGasket->uiGasket, psCam->sensor_phy->psGasket->uiWidth,
		psCam->sensor_phy->psGasket->uiHeight, psCam->sensor_phy->psGasket->bVSync,
		psCam->sensor_phy->psGasket->bHSync, psCam->sensor_phy->psGasket->ui8ParallelBitdepth);

	usleep(1000);

	return IMG_SUCCESS;

out_failed:
	LOG_ERROR("Set mode %d failed\n", mode_id);
	return ret;
}

#if defined(IQ_TUNING)
static IMG_RESULT sSetFPS(SENSOR_HANDLE hHandle, float down_ratio);
#endif
static IMG_RESULT sEnable(SENSOR_HANDLE hHandle)
{
	IMG_RESULT ret;
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);

	LOG_INFO(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);

	ASSERT_INITIALIZED(psCam->sensor_phy);

	ret = SensorPhyCtrl(psCam->sensor_phy, IMG_TRUE, 0, 0);
	if (ret) {
		LOG_ERROR("SensorPhyCtrl failed\n");
		return ret;
	}

	sensor_enable_ctrl(psCam->i2c, 1);
	//usleep(SENSOR_MAX_FRAME_TIME);

	psCam->enabled = IMG_TRUE;

	LOG_INFO("Sensor enabled!\n");
#if defined(IQ_TUNING)
	sSetFPS(hHandle, 30/10);
#endif

	return IMG_SUCCESS;
}

static IMG_RESULT sDisable(SENSOR_HANDLE hHandle)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);

	LOG_INFO(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);

	ASSERT_INITIALIZED(psCam->sensor_phy);

	psCam->enabled = IMG_FALSE;

	sensor_enable_ctrl(psCam->i2c, 0);

	usleep(SENSOR_MAX_FRAME_TIME);

	SensorPhyCtrl(psCam->sensor_phy, IMG_FALSE, 0, 0);

	LOG_INFO("Sensor disabled!\n");

	return IMG_SUCCESS;
}

static IMG_RESULT sDestroy(SENSOR_HANDLE hHandle)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);

	LOG_INFO(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);

	ASSERT_INITIALIZED(psCam->sensor_phy);

	if (psCam->enabled)
		sDisable(hHandle);

	SensorPhyDeinit(psCam->sensor_phy);

	close(psCam->i2c);
	IMG_FREE(psCam);

	return IMG_SUCCESS;
}

static IMG_RESULT sGetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);

	LOG_INFO(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);

	ASSERT_INITIALIZED(psCam->sensor_phy);
	IMG_ASSERT(psInfo);
    
	IMG_ASSERT(strlen(SNAME) < SENSOR_INFO_NAME_MAX);
	IMG_ASSERT(strlen(SENSOR_VERSION_NOT_VERIFYED) < SENSOR_INFO_VERSION_MAX);

	psInfo->eBayerOriginal = SENSOR_BAYER_FORMAT;
	psInfo->eBayerEnabled = psInfo->eBayerOriginal;
	sprintf(psInfo->pszSensorVersion, SENSOR_VERSION_NOT_VERIFYED);
	psInfo->fNumber = 1.2;
	psInfo->ui16FocalLength = 30;
//	psInfo->ui32WellDepth = 7500;
//	psInfo->flReadNoise = 5.0;
	psInfo->ui32WellDepth = 25000;
	psInfo->flReadNoise = 1.25;
	psInfo->ui8Imager = psCam->imager;
	psInfo->bBackFacing = IMG_TRUE;
#ifdef INFOTM_ISP
	psInfo->ui32ModeCount = mode_num;
#endif

	return IMG_SUCCESS;
}

static IMG_RESULT sGetGainRange(SENSOR_HANDLE hHandle, double *pflMin, double *pflMax, IMG_UINT8 *pui8Contexts)
{
	LOG_INFO(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);
	IMG_ASSERT(pflMin);
	IMG_ASSERT(pflMax);
	IMG_ASSERT(pui8Contexts);
	*pflMin = 1.0;
	*pflMax = SENSOR_MAX_GAIN;
	*pui8Contexts = 1;
	return IMG_SUCCESS;
}

static IMG_RESULT sGetCurrentGain(SENSOR_HANDLE hHandle, double *pflGain, IMG_UINT8 ui8Context)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	LOG_DEBUG(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);
	ASSERT_INITIALIZED(psCam->sensor_phy);
	IMG_ASSERT(pflGain);
	*pflGain = psCam->gain;
	return IMG_SUCCESS;
}

static IMG_RESULT sSetGain(SENSOR_HANDLE hHandle, double flGain, IMG_UINT8 ui8Context)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	IMG_UINT32 digital_gain, coarse_gain, fine_gain;

	//LOG_INFO("flGain=%lf >>>>>>>>>>>>\n", flGain);
	TUNE_SLEEP(1);
	
	ASSERT_INITIALIZED(psCam->sensor_phy);

	if (flGain > SENSOR_MAX_GAIN)
		flGain = SENSOR_MAX_GAIN;
	else if (flGain < 1)
		flGain = 1;

	psCam->gain = flGain;

	sensor_calculate_gain(flGain, &coarse_gain , &fine_gain, &digital_gain);

	sensor_write_gain(psCam, coarse_gain, fine_gain, digital_gain);
    
#ifdef NOISE_LOGIC_ENABLE
	noise_logic_gain(psCam, flGain);
#endif
	LOG_DEBUG("flGain=%lf, coarse_gain=%d, fine_gain=%d, digital_gain=%d, actual gain=%lf\n", 
			flGain, coarse_gain, fine_gain, digital_gain, (coarse_gain*((double)fine_gain*SENSOR_GAIN_STEP+1))*digital_gain);

	return IMG_SUCCESS;
}

static IMG_RESULT sGetExposureRange(SENSOR_HANDLE hHandle, 
	IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	LOG_INFO(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);
	ASSERT_INITIALIZED(psCam->sensor_phy);
	IMG_ASSERT(pui32Min);
	IMG_ASSERT(pui32Max);
	IMG_ASSERT(pui8Contexts);
	*pui32Min = sensor_modes[psCam->mode_id].mode.ui32ExposureMin;
#if !defined(IQ_TUNING)
	*pui32Max = sensor_modes[psCam->mode_id].mode.ui32ExposureMax;
#else
	*pui32Max = 1000000;
#endif
	*pui8Contexts = 1;
	return IMG_SUCCESS;
}

static IMG_RESULT sGetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	LOG_DEBUG(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);
	ASSERT_INITIALIZED(psCam->sensor_phy);
	IMG_ASSERT(pi32Current);
	*pi32Current = psCam->exposure;
	return IMG_SUCCESS;
}

static IMG_RESULT sSetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
	IMG_UINT32 exposure_lines, integration_adjust, vtotal;
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);

	//LOG_INFO("exposure=%d >>>>>>>>>>>>\n", ui32Current);
	TUNE_SLEEP(1);
	
	ASSERT_INITIALIZED(psCam->sensor_phy);

	psCam->exposure = ui32Exposure;

	exposure_lines = sensor_calculate_exposure(psCam, ui32Exposure);

	sensor_write_exposure(psCam, exposure_lines);
    
	LOG_DEBUG("SetExposure. time=%d us, lines = %d\n", psCam->exposure, exposure_lines);

	return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
static IMG_RESULT sSetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Flag)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	IMG_UINT32 line_time;	// in ns
	IMG_UINT32 delay;

	LOG_INFO("flip=0x%02X >>>>>>>>>>>>\n", ui8Flag);
	TUNE_SLEEP(1);
	
	ASSERT_INITIALIZED(psCam->sensor_phy);

	if (ui8Flag != psCam->flipping) {
		sensor_flip_image(psCam, ui8Flag & SENSOR_FLIP_HORIZONTAL, ui8Flag & SENSOR_FLIP_VERTICAL);
		psCam->flipping = ui8Flag;
	}

#if 0
	line_time = sensor_modes[psCam->mode_id].line_time;
	delay = line_time * sensor_modes[psCam->mode_id].mode.ui16VerticalTotal  / 2 / 1000;
	usleep(delay);

	/* Change ISP bayer format instead of modifying CROP X,Y position */
	psCam->bayerFormat = MosaicFlip(SC1135_BAYER_FORMAT, bFlipHorizontal?1:0, bFlipVertical?1:0);
	SensorPhyBayerFormat(psCam->sensor_phy, psCam->bayerFormat);
#endif

	return IMG_SUCCESS;
}

static IMG_RESULT sSetGainAndExposure(SENSOR_HANDLE hHandle, 
	double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	IMG_UINT32 digital_gain, coarse_gain , fine_gain, exposure_lines;

	LOG_DEBUG("gain=%lf, exposure=%d >>>>>>>>>>>>\n", flGain, ui32Exposure);
	TUNE_SLEEP(1);

	if (flGain > SENSOR_MAX_GAIN)
		flGain = SENSOR_MAX_GAIN;
	else if (flGain < 1)
		flGain = 1;

	psCam->gain = flGain;
	psCam->exposure = ui32Exposure;

	if (psCam->aec_agc) {
		LOG_DEBUG("Using sensor AEC & AGC function.\n");
		return IMG_SUCCESS;
	}

	sensor_calculate_gain(flGain, &coarse_gain , &fine_gain, &digital_gain);
	exposure_lines = sensor_calculate_exposure(psCam, ui32Exposure);

	sensor_write_gain(psCam, coarse_gain , fine_gain, digital_gain);
	sensor_write_exposure(psCam, exposure_lines);
    
#ifdef NOISE_LOGIC_ENABLE
	noise_logic_gain(psCam, flGain);
#endif
	LOG_DEBUG("flGain=%lf, coarse_gain=%d, fine_gain=%d, digital_gain=%d, actual gain=%lf\n", 
			flGain, coarse_gain, fine_gain, digital_gain, (coarse_gain*((double)fine_gain*SENSOR_GAIN_STEP+1))*digital_gain);
	LOG_DEBUG("SetExposure. time=%d us, lines = %d\n", psCam->exposure, exposure_lines);

	return IMG_SUCCESS;
}

static IMG_RESULT sGetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	LOG_DEBUG(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);
	if (pfixed != NULL) {
		*pfixed = sensor_modes[psCam->mode_id].mode.flFrameRate;
		LOG_DEBUG("Fixed FPS=%d\n", *pfixed);
	}
	return IMG_SUCCESS;
}

static IMG_RESULT sSetFPS(SENSOR_HANDLE hHandle, double fps)
{
	SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
	IMG_UINT32 fixed_fps = sensor_modes[psCam->mode_id].mode.flFrameRate;
	IMG_UINT32 fixed_frame_length = sensor_modes[psCam->mode_id].mode.ui16VerticalTotal;
	IMG_UINT32 frame_length;
#ifdef NOISE_LOGIC_ENABLE
	IMG_INT32 denoise_length;
#endif
    float down_ratio = 0.0;
    int fixedfps = 0;


	LOG_DEBUG(">>>>>>>>>>>>\n");
	TUNE_SLEEP(1);


    sGetFixedFPS(hHandle, &fixedfps);
    if(fps > fixedfps)
    {
        fps = fixedfps;
    }
    else if (fps < 5)
    {
        fps = 5;
    }
    down_ratio = psCam->cur_fps / fps;


	if (down_ratio == 1 && psCam->cur_fps == fixed_fps) {
		LOG_INFO("ratio=1, cur_fps = fixed_fps, skip operation\n");
		return IMG_SUCCESS;
	}

	if (down_ratio < 1) {
		LOG_WARNING("Invalid down_ratio=%d, force to 1, check fps param in json file\n", down_ratio);
		down_ratio = 1;
	}
	if (down_ratio > 12) {
		LOG_ERROR("Invalid down_ratio %d, too big\n", down_ratio);
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	frame_length = round(fixed_frame_length * down_ratio);
	sensor_i2c_write(psCam->i2c, SENSOR_FRAME_LENGTH_H, (frame_length>>8) & 0xFF);
	sensor_i2c_write(psCam->i2c, SENSOR_FRAME_LENGTH_L, frame_length & 0xFF);
#ifdef NOISE_LOGIC_ENABLE
	denoise_length = frame_length - 0x2e8;
	if (denoise_length < 0)
		denoise_length = 0;
	sensor_i2c_write(psCam->i2c, 0x3338, (frame_length>>8) & 0xFF);
	sensor_i2c_write(psCam->i2c, 0x3339, frame_length & 0xFF);
	sensor_i2c_write(psCam->i2c, 0x3336, (denoise_length>>8) & 0xFF);
	sensor_i2c_write(psCam->i2c, 0x3337, denoise_length & 0xFF);
#endif
	psCam->cur_fps = round((double)fixed_fps/down_ratio);
	LOG_INFO("down_ratio = %lf, frame_length = %d, fixed_fps = %d, cur_fps = %d >>>>>>>>>>>>\n", 
				down_ratio, frame_length, fixed_fps, psCam->cur_fps);

	return IMG_SUCCESS;
}
#endif

IMG_RESULT SC1135DVP_Create(struct _Sensor_Functions_ **phHandle)
{
	char i2c_dev_path[NAME_MAX];
	SENSOR_CAM_STRUCT *psCam;
	IMG_UINT32 i2c_addr = SENSOR_I2C_ADDR;
	IMG_RESULT ret;
	char adaptor[64];
	LOG_INFO(" >>>>>>>>>>>>\n");
	TUNE_SLEEP(1);
	
	/* Init global variable */
	mode_num = ARRAY_SIZE(sensor_modes);
	LOG_DEBUG("mode_num=%d\n", mode_num);

	psCam = (SENSOR_CAM_STRUCT *)IMG_CALLOC(1, sizeof(SENSOR_CAM_STRUCT));
	if (!psCam)
		return IMG_ERROR_MALLOC_FAILED;

	/* Init function handle */
	*phHandle = &psCam->funcs;
	psCam->funcs.GetMode = sGetMode;
	psCam->funcs.GetState = sGetState;
	psCam->funcs.SetMode = sSetMode;
	psCam->funcs.Enable = sEnable;
	psCam->funcs.Disable = sDisable;
	psCam->funcs.Destroy = sDestroy;
	psCam->funcs.GetInfo = sGetInfo;
	psCam->funcs.GetGainRange = sGetGainRange;
	psCam->funcs.GetCurrentGain = sGetCurrentGain;
	psCam->funcs.SetGain = sSetGain;
	psCam->funcs.GetExposureRange = sGetExposureRange;
	psCam->funcs.GetExposure = sGetExposure;
	psCam->funcs.SetExposure = sSetExposure;
#ifdef INFOTM_ISP
	psCam->funcs.SetFlipMirror = sSetFlipMirror;
	psCam->funcs.SetGainAndExposure = sSetGainAndExposure;
	psCam->funcs.GetFixedFPS = sGetFixedFPS;
	psCam->funcs.SetFPS = sSetFPS;
#endif

	/* Init sensor config */
	psCam->imager = 1;
	
	/* Init sensor state */
	psCam->enabled = IMG_FALSE;
	psCam->mode_id = 0;
	psCam->flipping = SENSOR_FLIP_NONE;
	psCam->exposure = SENSOR_EXPOSURE_DEFAULT;
	psCam->gain = 1;
	psCam->coarse_gain_reg_val = 0;
	psCam->fine_gain_reg_val = 0;
	psCam->aec_agc = DEFAULT_USE_AEC_AGC;

	/* Init i2c */
	LOG_INFO("i2c_addr = 0x%X\n", i2c_addr);
	ret = find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path),  i2c_addr, adaptor);
	if (ret) {
		LOG_ERROR("Failed to find I2C device! ret=%d\n", ret);
		IMG_FREE(psCam);
		*phHandle=NULL;
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}
	psCam->i2c = open(i2c_dev_path, O_RDWR);
	if (psCam->i2c < 0) {
		LOG_ERROR("Failed to open I2C device: \"%s\", err = %d\n", i2c_dev_path, psCam->i2c);
		IMG_FREE(psCam);
		*phHandle=NULL;
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}

	/* Init ISP gasket phy */
	psCam->sensor_phy = SensorPhyInit();
	if (!psCam->sensor_phy) {
		LOG_ERROR("Failed to create sensor phy!\n");
		close(psCam->i2c);
		IMG_FREE(psCam);
		*phHandle = NULL;
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}

	return IMG_SUCCESS;
}

