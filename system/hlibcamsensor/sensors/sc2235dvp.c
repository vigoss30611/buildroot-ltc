/*********************************************************************************
@file sc2235dvp.c

@brief SC2235 camera sensor implementation

@copyright InfoTM Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
InfoTM Limited.

@author:  minghui.zhao  2017/9/5

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
#include "sensors/ddk_sensor_driver.h"


/* Sensor specific configuration */
#include "sensors/sc2235dvp.h"
#define SNAME		"SC2235"
#define LOG_TAG	SNAME
#define SENSOR_I2C_ADDR			    0x30        // in 7-bits
#define SENSOR_BAYER_FORMAT		    MOSAIC_BGGR
#define SENSOR_MAX_GAIN				240.25
#define SENSOR_MAX_GAIN_REG_VAL			0xf40
#define SENSOR_MIN_GAIN_REG_VAL			0x82

#define SENSOR_GAIN_REG_ADDR_H		0x3e08
#define SENSOR_GAIN_REG_ADDR_L		0x3e09

#define SENSOR_EXPOSURE_REG_ADDR_H	0x3e01
#define SENSOR_EXPOSURE_REG_ADDR_L	0x3e02

#define SENSOR_MAX_FRAME_TIME			100000		// in us. used for delay. consider the lowest fps
#define SENSOR_FRAME_LENGTH_H		0x320e
#define SENSOR_FRAME_LENGTH_L		0x320f

#define SENSOR_EXPOSURE_DEFAULT		31111
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


#ifdef INFOTM_ISP
#define DEV_PATH    ("/dev/ddk_sensor")
#define EXTRA_CFG_PATH  ("/root/.ispddk/")
static IMG_UINT32 i2c_addr = 0;
static IMG_UINT32 imgr = 1;
static char extra_cfg[64];
#endif


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
	double gain_bk;		// current gain register value
	double cur_fps;

    	IMG_UINT16 gain_reg_val_limit;
	IMG_UINT16 gain_reg_val;
	
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

static IMG_UINT16 registers_1080p_25fps[] = {
#if 1
    0x0103,0x01,

    0x0100,0x00,
    0x3621,0x28,
    0x3309,0x60,
    0x331f,0x4d,

    0x3321,0x4f,
    0x33b5,0x10,
    0x3303,0x20,
    0x331e,0x0d,

    0x3320,0x0f,
    0x3622,0x02,
    0x3633,0x42,
    0x3634,0x42,

    0x3306,0x66,
    0x330b,0xd1,
    0x3301,0x0e,
    0x320c,0x0a, //eric 0x0a

    0x320d,0x50, //eric 0x50
    0x3364,0x05,
    0x363c,0x28,
    0x363b,0x0a,
    0x3635,0xa0,
    0x4500,0x59,
    0x3d08,0x03, //eric 0x00
    0x3908,0x11,

    0x363c,0x08,
    0x3e03,0x03,
    0x3e01,0x46,
    0x3381,0x0a,

    0x3348,0x09,
    0x3349,0x50,
    0x334a,0x02,
    0x334b,0x60,

    0x3380,0x04,
    0x3340,0x06,
    0x3341,0x50,
    0x3342,0x02,

    0x3343,0x60,
    0x3632,0x88,
    0x3309,0xa0,
    0x331f,0x8d,

    0x3321,0x8f,
    0x335e,0x01,
    0x335f,0x03,
    0x337c,0x04,

    0x337d,0x06,
    0x33a0,0x05,
    0x3301,0x05,
    0x3670,0x08,

    0x367e,0x07,
    0x367f,0x0f,
    0x3677,0x2f,
    0x3678,0x22,

    0x3679,0x42,
    0x337f,0x03,
    0x3368,0x02,
    0x3369,0x00,

    0x336a,0x00,
    0x336b,0x00,
    0x3367,0x08,
    0x330e,0x30,

    0x3366,0x7c,
    0x3635,0xc1,
    0x363b,0x09,
    0x363c,0x07,

    0x391e,0x00,
    0x3637,0x14,
#if 0
    0x3306,0x54,
    0x330b,0xd8,
#else// fix edge color shift issue
    0x3306,0x40,
    0x330b,0xc8,
#endif
    0x366e,0x08,
    0x366f,0x2f,
    0x3631,0x84,
    0x3630,0x48,
    0x3622,0x06,

    0x3638,0x1f,
    0x3625,0x02,
    0x3636,0x24,
    0x3348,0x08,

    0x3e03,0x0b,
    0x3342,0x03,
    0x3343,0xa0,
    0x334a,0x03,

    0x334b,0xa0,
    0x3343,0xb0,
    0x334b,0xb0,
    0x3679,0x43,

    0x3802,0x01,
    0x3235,0x04,
    0x3236,0x63,
    0x3343,0xd0,

    0x334b,0xd0,
    0x3348,0x07,
    0x3349,0x80,
    0x391b,0x4d,
#if 0
    0x3641,0x01,
#else // increase io device power and plk clk delay
    0x3640,0x03,
    0x3641,0x03,
#endif
    0x3d08,0x03,  //eric 0x01
    0x3e01,0x00,
    0x3e02,0x10,
    0x3e03,0x0b,
    0x3e08,0x00,
    0x3e09,0x10,

    //0804-0808
    0x3222,0x29,
    0x3901,0x02,
    
    // auto blc 0919 linyun.xiong
    //0x3900,0x01,
    //0x3902,0x01,
    
    0x3f00,0x07,
    0x3f04,0x0a,
    0x3f05,0x2c,
/*
    //[gain < 2]
    0x3301,0x05,
    0x3631,0x84,
    0x366f,0x2f,

    //[gain<8.0]
    0x3301,0x15,
    0x3631,0x88,
    0x366f,0x2f,    
    
    //[gain<15.5]
    0x3301,0x19,
    0x3631,0x88,
    0x366f,0x2f,

    //[gain<15.5]
    0x3301,0xff,
    0x3631,0x88,
    0x366f,0x3c,
*/
    0x0100,0x01
#else
    0x0103,0x01,
    0x0100,0x00,
    0x3621,0x28,
    0x33b5,0x10,
    0x3303,0x20,
    0x331e,0x0d,
    0x3320,0x0f,
    0x3633,0x42,
    0x3634,0x42,
#if 1 //original    
    0x320c,0x08,
    0x320d,0x98,
#else
    0x320c,0x0a, //eric 0x0a
    0x320d,0x50, //eric 0x50
#endif    
    0x3364,0x05,
    0x4500,0x59,
#if 0 //original
    0x3d08,0x00,
#else
    0x3d08,0x03, //eric 0x00
#endif
    0x3908,0x11,
    0x3e01,0x46,
    0x3381,0x0a,
    0x3380,0x04,
    0x3340,0x06,
    0x3341,0x50,
    0x3632,0x88,
    0x3309,0xa0,
    0x331f,0x8d,
    0x3321,0x8f,
    0x335e,0x01,
    0x335f,0x03,
    0x337c,0x04,
    0x337d,0x06,
    0x33a0,0x05,
    0x3301,0x05,
    0x3670,0x08,
    0x367e,0x07,
    0x367f,0x0f,
    0x3677,0x2f,
    0x3679,0x43,
    0x337f,0x03,
    0x3368,0x02,
    0x3369,0x00,
    0x336a,0x00,
    0x336b,0x00,
    0x3367,0x08,
    0x330e,0x30,
    0x3366,0x7c,
    0x3635,0xc1,
    0x363b,0x09,
    0x363c,0x07,
    0x391e,0x00,
    0x3637,0x14,
    0x366e,0x08,
    0x366f,0x2f,
    0x3631,0x84,
    0x3630,0x48,
    0x3622,0x06,
    0x3638,0x1f,
    0x3625,0x02,
    0x3636,0x24,
    0x3e03,0x03,
    0x3802,0x01,
    0x3235,0x04,
    0x3236,0x63,
    0x3348,0x07,
    0x3349,0x80,
    0x391b,0x4d,
    0x3342,0x04,
    0x3343,0x20,
    0x334a,0x04,
    0x334b,0x20,
    0x3222,0x29,
    0x3901,0x02,
    0x3f00,0x07,
    0x3f04,0x08,
    0x3f05,0x74,
    0x3639,0x09,
    0x5780,0xff,
    0x5781,0x04,
    0x5785,0x18,
    0x3313,0x05,
    0x3678,0x42,
    0x330b,0xc8,
    0x3306,0x40,

    // increase io device power and plk clk delay
    0x3640,0x03,
    //0x3641,0x03,
    0x3641,0x01,
    0x0100,0x01,
#endif

};

static SENSOR_MODE_STRUCT sensor_modes[] = {
	{{10, 1920, 1080, /*30*/25, 74.25, 2640/*2200*/, 1125, SENSOR_FLIP_BOTH, 29, 33333}, 1, 1, 29630, 
		registers_1080p_25fps, ARRAY_SIZE(registers_1080p_25fps)},
};
static IMG_UINT8 mode_num = 0;


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

    registers = sensor_modes[mode_id].registers;
    reg_num = sensor_modes[mode_id].reg_num;

    LOG_INFO("**sc2235 dvp sensor mode_id=%d reg_num=%d**\n", mode_id, reg_num);

    for (i = 0; i < reg_num; i+=2) {
        ret = sensor_i2c_write(i2c, registers[i], registers[i+1]);
        if (ret != IMG_SUCCESS) {
            LOG_ERROR("sc2235 dvp sensor Set mode %d failed\n", mode_id);
            return ret;
        }
    }

    return IMG_SUCCESS;
}

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
        sensor_i2c_write(i2c, 0x0100, 0x01);
        usleep(2000);
    }
    else {
        sensor_i2c_write(i2c, 0x0100, 0x00);
        usleep(2000);
    }
    return IMG_SUCCESS;
}

static IMG_RESULT sensor_flip_image(int i2c, IMG_UINT8 hflip, IMG_UINT8 vflip)
{
	LOG_INFO("hflip=%d, vflip=%d\n", hflip, vflip);

    IMG_UINT8 value = 0;

    if (hflip)
        value |= 0x6;
    else
        value |= 0x0;

    if (vflip)
        value |= 0x60;
    else
        value |= 0x0;

    sensor_i2c_write(i2c, 0x3221, value);

	return IMG_SUCCESS;
}

static IMG_RESULT sensor_calculate_gain(double flGain)
{
	IMG_UINT32 gain_integer, val_integer, val_decimal, val_gain;
	double gain_decimal;

	gain_integer = floor(flGain);
	gain_decimal = flGain - gain_integer;
	//val_decimal = round(gain_decimal * 16);
	val_decimal = floor(gain_decimal * 16);
	val_integer = gain_integer * 16;
	val_gain = val_integer+val_decimal;
   // printf("val_gain=%x\n",val_gain);
	return val_gain;
}

static IMG_UINT32 sensor_calculate_exposure(SENSOR_CAM_STRUCT *psCam, IMG_UINT32 expo_time)
{
    IMG_UINT32 exposure_lines;

    exposure_lines = expo_time * 1000/sensor_modes[psCam->mode_id].line_time;
    if (exposure_lines < 1)
        exposure_lines = 1;

    return exposure_lines;
}

static void sensor_write_gain(SENSOR_CAM_STRUCT *psCam, IMG_UINT32 val_gain)
{
    if(val_gain > psCam->gain_reg_val_limit){
        val_gain = psCam->gain_reg_val_limit;
    }

	if (val_gain == psCam->gain_reg_val)
		return;
	sensor_i2c_write(psCam->i2c, SENSOR_GAIN_REG_ADDR_H, (val_gain>>8) & 0xFF);
	sensor_i2c_write(psCam->i2c, SENSOR_GAIN_REG_ADDR_L, val_gain & 0xFF);
	psCam->gain_reg_val = val_gain;
}

static void sensor_write_exposure(SENSOR_CAM_STRUCT *psCam, IMG_UINT32 exposure_lines)
{
    if (exposure_lines == psCam->expo_reg_val)
        return;
    sensor_i2c_write(psCam->i2c, SENSOR_EXPOSURE_REG_ADDR_H, (exposure_lines >> 4) & 0xFF);
    sensor_i2c_write(psCam->i2c, SENSOR_EXPOSURE_REG_ADDR_L, (exposure_lines<<4) & 0xFF);
    psCam->expo_reg_val = exposure_lines;
}

static IMG_BOOL in_range(double gain, int bit)
{
    if (bit < 1) {
        LOG_ERROR("Invalid param bit %d\n", bit);
        return IMG_FALSE;
    }
    
    if (gain < (1<<bit) && gain >= (1<<(bit-1)))
        return IMG_TRUE;
    else
        return IMG_FALSE;
}

static IMG_RESULT sGetMode(SENSOR_HANDLE hHandle, IMG_UINT16 mode_id, SENSOR_MODE *smode)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    //LOG_INFO("sc2235 dvp sensor  mode_id = %d\n", mode_id);
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

    //LOG_INFO("**sc2235 dvp sensor sGetState** \n");
    TUNE_SLEEP(1);
    IMG_ASSERT(psStatus);

    if (!psCam->sensor_phy) {
        LOG_WARNING("sensor not initialised\n");
        psStatus->eState = SENSOR_STATE_UNINITIALISED;
        psStatus->ui16CurrentMode = 0;
    } else {
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

    LOG_INFO("**sc2235 dvp sensor mode=%d, flipping=0x%02X**\n", mode_id, flipping);
    TUNE_SLEEP(1);

    ASSERT_INITIALIZED(psCam->sensor_phy);
    ASSERT_MODE_RANGE(mode_id);

    if(flipping & sensor_modes[mode_id].mode.ui8SupportFlipping != flipping) {
        LOG_ERROR("Sensor mode %d does not support selected flipping 0x%x (supports 0x%x)\n", 
              mode_id, flipping, sensor_modes[mode_id].mode.ui8SupportFlipping);
        return IMG_ERROR_NOT_SUPPORTED;
    }

	/* Config registers */
	ret = sensor_config_register(psCam->i2c, mode_id);
	if (ret)
		goto out_failed;
	usleep(2000);

    /* Config sensor AEC/AGC ctrl */
    sensor_aec_agc_ctrl(psCam->i2c, 1/*psCam->aec_agc*/);

    /* Config flipping */
    ret = sensor_flip_image(psCam->i2c, flipping & SENSOR_FLIP_HORIZONTAL, flipping & SENSOR_FLIP_VERTICAL);
    if (ret)
        goto out_failed;

    /* Init sensor status */
    psCam->enabled = IMG_FALSE;
    psCam->mode_id = mode_id;
    psCam->flipping = flipping;
    psCam->exposure = SENSOR_EXPOSURE_DEFAULT;
    psCam->gain = 1.0;
    psCam->gain_bk = 1.0;
    psCam->cur_fps = sensor_modes[mode_id].mode.flFrameRate;

    /* Init ISP gasket params */
    psCam->sensor_phy->psGasket->uiGasket  = psCam->imager;
    psCam->sensor_phy->psGasket->bParallel = IMG_TRUE;
    psCam->sensor_phy->psGasket->uiWidth   = sensor_modes[mode_id].mode.ui16Width-1;
    psCam->sensor_phy->psGasket->uiHeight  = sensor_modes[mode_id].mode.ui16Height-1;
    psCam->sensor_phy->psGasket->bVSync    = sensor_modes[mode_id].vsync;
    psCam->sensor_phy->psGasket->bHSync    = sensor_modes[mode_id].hsync;
    psCam->sensor_phy->psGasket->ui8ParallelBitdepth = sensor_modes[mode_id].mode.ui8BitDepth;

    LOG_DEBUG("gasket=%d, width=%d, height=%d, vsync=%d, hsync=%d,bitdepth=%d, aec_agc=%d\n",
        psCam->sensor_phy->psGasket->uiGasket, psCam->sensor_phy->psGasket->uiWidth,
        psCam->sensor_phy->psGasket->uiHeight, psCam->sensor_phy->psGasket->bVSync,
        psCam->sensor_phy->psGasket->bHSync, 
        psCam->sensor_phy->psGasket->ui8ParallelBitdepth, psCam->aec_agc);

    usleep(1000);

    return IMG_SUCCESS;

out_failed:
    LOG_ERROR("Set mode %d failed\n", mode_id);
    return ret;
}

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

    //LOG_INFO(">>>>>>>>>>>>\n");
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
    psInfo->ui32WellDepth = 7500;
    psInfo->flReadNoise = 5.0;
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

    TUNE_SLEEP(1);

    ASSERT_INITIALIZED(psCam->sensor_phy);

    if (flGain > SENSOR_MAX_GAIN)
    {
        flGain = SENSOR_MAX_GAIN;
    }
    else if (flGain < 1)
    {
        flGain = 1;
    }
    
    psCam->gain = flGain;

    IMG_UINT32 val_gain = sensor_calculate_gain(flGain);

    sensor_write_gain(psCam, val_gain);

    LOG_DEBUG("flGain=%lf, register value=0x%02x, actual gain=%lf\n", flGain, val_gain, ((double)val_gain)/16);

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
    *pui32Max = 600000;
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

    TUNE_SLEEP(1);

    ASSERT_INITIALIZED(psCam->sensor_phy);

    if (ui8Flag != psCam->flipping) {
        sensor_flip_image(psCam->i2c, ui8Flag & SENSOR_FLIP_HORIZONTAL, ui8Flag & SENSOR_FLIP_VERTICAL);
        psCam->flipping = ui8Flag;
    }

    return IMG_SUCCESS;
}

static IMG_RESULT sSetGainAndExposure(SENSOR_HANDLE hHandle, 
	double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    IMG_UINT32 exposure_lines;

    
    LOG_DEBUG("gain=%lf, exposure=%d >>>>>>>>>>>>\n", flGain, ui32Exposure);
    TUNE_SLEEP(1);

    if (flGain > SENSOR_MAX_GAIN)
    {
        flGain = SENSOR_MAX_GAIN;
    }
    else if (flGain < 1)
    {
        flGain = 1;
    }

    psCam->gain = flGain;
    psCam->exposure = ui32Exposure;

    if (psCam->aec_agc) {
        LOG_DEBUG("Using sensor AEC & AGC function.\n");
        return IMG_SUCCESS;
    }

    IMG_UINT32 val_gain = sensor_calculate_gain(flGain);
    exposure_lines = sensor_calculate_exposure(psCam, ui32Exposure);
    
    sensor_write_exposure(psCam, exposure_lines);
    sensor_write_gain(psCam, val_gain);
    
    LOG_DEBUG("flGain=%lf, register value=0x%02x, actual gain=%lf\n", flGain, val_gain, val_gain/16.0);
    //LOG_DEBUG
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
    IMG_UINT32 frame_length, denoise_length;
    float down_ratio = 0.0;
    int fixedfps = 0;

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

	psCam->cur_fps = round((double)fixed_fps/down_ratio);
	LOG_INFO("down_ratio = %lf, frame_length = %d, fixed_fps = %d, cur_fps = %d >>>>>>>>>>>>\n", 
				down_ratio, frame_length, fixed_fps, psCam->cur_fps);

	return IMG_SUCCESS;
}
#endif

IMG_RESULT SC2235DVP_Create(struct _Sensor_Functions_ **phHandle, int index)
{
    char i2c_dev_path[NAME_MAX];
    SENSOR_CAM_STRUCT *psCam;
    IMG_UINT32 i2c_addr = SENSOR_I2C_ADDR;
    IMG_RESULT ret;

    char dev_nm[64];
    char adaptor[64];
    char path[128];
    int chn = 0;

#if 1 //defined(INFOTM_ISP)
		int fd = 0;
		memset((void *)dev_nm, 0, sizeof(dev_nm));
		memset((void *)adaptor, 0, sizeof(adaptor));
		memset((void *)path, 0, sizeof(path));
		sprintf(dev_nm, "%s%d", DEV_PATH, index );
		fd = open(dev_nm, O_RDWR);
		if(fd <0)
		{
			LOG_ERROR("open %s error\n", dev_nm);
			return IMG_ERROR_FATAL;
		}
	
		ioctl(fd, GETI2CADDR, &i2c_addr);
		ioctl(fd, GETI2CCHN, &chn);
		ioctl(fd, GETIMAGER, &imgr);
		ioctl(fd, GETSENSORPATH, path);
	
		close(fd);
		printf("%s opened OK, i2c-addr=0x%x, chn = %d\n", dev_nm, i2c_addr, chn);
		sprintf(adaptor, "%s-%d", "i2c", chn);
		memset((void *)extra_cfg, 0, sizeof(dev_nm));
		if (path[0] == 0) {
			sprintf(extra_cfg, "%s%s%d-config.txt", EXTRA_CFG_PATH, "sensor" , index );
		} else {
			strcpy(extra_cfg, path);
		}
#endif

    LOG_INFO("**SC2235DVP SENSOR**\n");
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
    psCam->gain = 1.0;
    psCam->gain_bk = 1.0;
    psCam->aec_agc = DEFAULT_USE_AEC_AGC;
    psCam->gain_reg_val_limit = SENSOR_MAX_GAIN_REG_VAL;
    psCam->gain_reg_val = 0;
    
    /* Init i2c */
    LOG_ERROR("i2c_addr = 0x%X\n", i2c_addr);
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

