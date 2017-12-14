/*********************************************************************************
@file sc3035dvp.c

@brief SC3035 camera sensor implementation

@copyright InfoTM Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
InfoTM Limited.

@author:  feng.qu@infotm.com    2016.12.21
@author:  zhongyuan.zhang@infotm.com    2017.3.31

******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#include <pthread.h>
#include <unistd.h>

#include <img_types.h>
#include <img_errors.h>
#include <ci/ci_api_structs.h>
#include <felixcommon/userlog.h>

#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"
#include "sensors/ddk_sensor_driver.h"
#include <pthread.h>

/* Sensor specific configuration */
#include "sensors/sc3035dvp.h"
#define SNAME        "SC3035"
#define LOG_TAG    SNAME
#define SENSOR_I2C_ADDR            0x30            // in 7-bits
#define SENSOR_BAYER_FORMAT        MOSAIC_BGGR
#define SENSOR_MAX_GAIN            124
#define SENSOR_MAX_GAIN_REG_VAL            0x7c0
#define SENSOR_MIN_GAIN_REG_VAL            0x82
#define SENSOR_GAIN_REG_ADDR_H        0x3e08
#define SENSOR_GAIN_REG_ADDR_L        0x3e09
#define SENSOR_EXPOSURE_REG_ADDR_H    0x3e01
#define SENSOR_EXPOSURE_REG_ADDR_L    0x3e02
#define SENSOR_EXPOSURE_DEFAULT        31111

// in us. used for delay. consider the lowest fps
#define SENSOR_MAX_FRAME_TIME            100000
#define SENSOR_FRAME_LENGTH_H        0x320e
#define SENSOR_FRAME_LENGTH_L        0x320f

#define SENSOR_VERSION_NOT_VERIFYED    "not-verified"

#define ARRAY_SIZE(n)        (sizeof(n)/sizeof(n[0]))

#define SENSOR_TUNNING        0
#define TUNE_SLEEP(n)    {if (SENSOR_TUNNING) sleep(n);}

/* Assert */
#define ASSERT_INITIALIZED(p)        \
{if (!p) {LOG_ERROR("Sensor not initialised\n");return IMG_ERROR_NOT_INITIALISED;}}
#define ASSERT_MODE_RANGE(n)        \
{if (n >= mode_num) {\
LOG_ERROR("Invalid mode_id %d, there is only %d modes\n", n, mode_num);\
return IMG_ERROR_INVALID_ID;}}


//#undef LOG_DEBUG
//#define LOG_DEBUG(...)    LOG_DEBUG_TAG(LOG_TAG, __VA_ARGS__)

/* SmartSens sc3035 denoise logic */
//#define NOISE_LOGIC_ENABLE
//#define HIGH_TEMPERATURE_LOGIC

#ifdef INFOTM_ISP
#define DEV_PATH    ("/dev/ddk_sensor")
#define EXTRA_CFG_PATH  ("/root/.ispddk/")
static IMG_UINT32 i2c_addr = 0;
static IMG_UINT32 imgr = 1;
static char extra_cfg[64];
#endif

typedef struct _sensor_cam {
    SENSOR_FUNCS funcs;
    SENSOR_PHY *sensor_phy;
    int i2c;

    /* Sensor status */
    IMG_BOOL enabled;            // in using or not
    IMG_UINT16 mode_id;        // index of current mode
    IMG_UINT8 flipping;
    IMG_UINT32 exposure;
    IMG_UINT32 expo_reg_val;        // current exposure register value
    double gain;
    IMG_UINT16 gain_reg_val_limit;
    IMG_UINT16 gain_reg_val;        // current gain register value
    double cur_fps;

    /* Sensor config params */
    IMG_UINT8 imager;            // 0: MIPI0   1: MIPI1   2: DVP
#if 0
    pthread_t tid;
    pthread_mutexattr_t attr;
    pthread_mutex_t lock;
#endif
}SENSOR_CAM_STRUCT;

/* Predefined sensor mode */
typedef struct _sensor_mode {
    SENSOR_MODE mode;
    IMG_UINT8 hsync;
    IMG_UINT8 vsync;
    IMG_UINT32 line_time;        // in ns. for exposure
    IMG_UINT16 *registers;
    IMG_UINT32 reg_num;
} SENSOR_MODE_STRUCT;

static IMG_UINT16 registers_1536p_25fps[] = {
    //0x0103,0x01,// soft reset
    0x0100,0x00,

    0x4500,0x31, // rnc sel
    0x3416,0x11,
    0x4501,0xa4, // bit ctrl

    0x3e03,0x03, // aec
    0x3e08,0x00,
    0x3e09,0x10,
    0x3e01,0x30,

    //old timing
    0x322e,0x00,
    0x322f,0xaf,
    0x3306,0x20,
    0x3307,0x17,
    0x330b,0x54,
    0x3303,0x20,
    0x3309,0x20,
    0x3308,0x08,
    0x331e,0x16,
    0x331f,0x16,
    0x3320,0x18,
    0x3321,0x18,
    0x3322,0x18,
    0x3323,0x18,
    0x330c,0x0b,
    0x330f,0x07,
    0x3310,0x42,
    0x3324,0x07,
    0x3325,0x07,
    0x335b,0xca,
    0x335e,0x07,
    0x335f,0x10,
    0x3334,0x00,

    //mem write
    0x3F01,0x04,
    0x3F04,0x01,
    0x3F05,0x30,
    0x3626,0x01,

    //analog
    0x3635,0x60,
    0x3631,0x84,
    0x3636,0x8d, //0607
    0x3633,0x3f,
    0x3639,0x80,
    0x3622,0x1e,
    0x3627,0x02,
    0x3038,0xa4,
    0x3621,0x18,
    0x363a,0x1c,

    //ramp
    0x3637,0xbe,
    0x3638,0x85,
    0x363c,0x48, // ramp cur

    0x5780,0xff, // dpc
    0x5781,0x04,
    0x5785,0x10,

    //close mipi
    0x301c,0xa4, // dig
    0x301a,0xf8, // ana
    0x3019,0xff,
    0x3022,0x13,

    0x301e,0xe0, // [4] 0:close tempsens
    0x3662,0x82,
    0x3d0d,0x00, // close random code

    //2048x1536
    0x3039,0x20,
    0x303a,0x35, //74.25M pclk
    0x303b,0x00,

    0x3306,0x46,
    0x330b,0xa0,
    0x3038,0xf8, //pump clk div

    0x320c,0x05,  //hts=3000
    0x320d,0xdc,
    0x320e,0x06,  //vts=1584
    0x320f,0x30,

    0x3202,0x00, // ystart=48
    0x3203,0x00,
    0x3206,0x06, // yend=1545    1545 rows selected
    0x3207,0x08,

    0x3200,0x01, // xstart= 264
    0x3201,0x08,
    0x3204,0x09, // xend = 2319  2056 cols selected
    0x3205,0x0f,

    0x3211,0x04,  // xstart
    0x3213,0x04,  // ystart

    0x3208,0x08,  //2048x1536
    0x3209,0x00,
    0x320a,0x06,
    0x320b,0x00,

    //0513
    0x3312,0x06, // sa1 timing
    0x3340,0x04,
    0x3341,0xd2,
    0x3342,0x01,
    0x3343,0x80,
    0x335d,0x2a, // cmp timing
    0x3348,0x04,
    0x3349,0xd2,
    0x334a,0x01,
    0x334b,0x80,
    0x3368,0x03, // auto precharge
    0x3369,0x30,
    0x336a,0x06,
    0x336b,0x30,
    0x3367,0x05,
    0x330e,0x17,

    0x3d08,0x03, // pclk inv

    //fifo
    0x303f,0x82,
    0x3c03,0x28, //fifo sram read position
    0x3c00,0x45, // Dig SRAM reset

    //0607
    0x3c03,0x02, //anti smear
    0x3211,0x06,
    0x3213,0x06,
    0x3620,0x82,


    //logic change@ gain<2
    0x3630,0xb1, //0x67
    0x3635,0x60, //0x66

    //0704
    0x3630,0x67,
    0x3626,0x11,

    //0910
    0x363c,0x88, //fine gain correction
    0x3312,0x00,
    0x3333,0x80,
    0x3334,0xa0,
    0x3620,0x62,  //0xd2
    0x3300,0x10,

    //0912
    0x3627,0x06,
    0x3312,0x06,
    0x3340,0x03,
    0x3341,0x80,
    0x3334,0x20,

    0x331e,0x10,
    0x331f,0x13,
    0x3320,0x18,
    0x3321,0x18,

    //118.8M pclk 30fps
    0x3039,0x30,
    0x303a,0x2a, //118.8M pclk
    0x303b,0x00,
    0x3640,0x02,
    0x3641,0x01,
    0x5000,0x21,

    0x3340,0x04,
    0x3342,0x02,
    0x3343,0x60,
    0x334a,0x02,
    0x334b,0x60,
    0x3306,0x66,
    0x3367,0x01,
    0x330b,0xff,
    0x3300,0x20,
    0x331f,0x10,
    0x3f05,0xe0,
    0x3635,0x62,


    0x3620,0x63, //d2

    0x3630,0x67, //a9,

    //0926
    0x3633,0x3c,
    0x363a,0x04,

    //1022
    0x3633,0x3d,
    0x3300,0x30,
    0x3f05,0xf8,
    0x3343,0x70,
    0x334b,0x70,

    //1028
    0x3211,0x04,

    0x0100,0x00, //stream output off
};

static IMG_UINT16 registers_1536_1536_25fps[] = {
    //0x0103,0x01,// soft reset
    0x0100,0x00,

    0x4500,0x31, // rnc sel
    0x3416,0x11,
    0x4501,0xa4, // bit ctrl

    0x3e03,0x03, // aec
    0x3e08,0x00,
    0x3e09,0x10,
    0x3e01,0x30,

    //old timing
    0x322e,0x00,
    0x322f,0xaf,
    0x3306,0x20,
    0x3307,0x17,
    0x330b,0x54,
    0x3303,0x20,
    0x3309,0x20,
    0x3308,0x08,
    0x331e,0x16,
    0x331f,0x16,
    0x3320,0x18,
    0x3321,0x18,
    0x3322,0x18,
    0x3323,0x18,
    0x330c,0x0b,
    0x330f,0x07,
    0x3310,0x42,
    0x3324,0x07,
    0x3325,0x07,
    0x335b,0xca,
    0x335e,0x07,
    0x335f,0x10,
    0x3334,0x00,

    //mem write
    0x3F01,0x04,
    0x3F04,0x01,
    0x3F05,0x30,
    0x3626,0x01,

    //analog
    0x3635,0x65, //0x60,
    0x3631,0x84,
    0x3636,0x8d, //0607
    0x3633,0x3f,
    0x3639,0x80,
    0x3622,0x1e,
    0x3627,0x02,
    0x3038,0xa4,
    0x3621,0x18,
    0x363a,0x1c,

    //ramp
    0x3637,0xbe,
    0x3638,0x85,
    0x363c,0x48, // ramp cur

    0x5780,0xff, // dpc
    0x5781,0x04,
    0x5785,0x10,

    //close mipi
    0x301c,0xa4, // dig
    0x301a,0xf8, // ana
    0x3019,0xff,
    0x3022,0x13,

    0x301e,0xe0, // [4] 0:close tempsens
    0x3662,0x82,
    0x3d0d,0x00, // close random code

    //2048x1536
    0x3039,0x00,
    0x303a,0x32, //75.6M pclk
    0x303b,0x02,

    0x3306,0x46,
    0x330b,0xa0,
    0x3038,0xf8, //pump clk div

    0x320c,0x03,  // hts=1920
    0x320d,0xc0,
    0x320e,0x06,  // vts=1575
    0x320f,0x27,

    0x3202,0x00, // ystart=0
    0x3203,0x00,
    0x3206,0x06, // yend=1544    1545 rows selected
    0x3207,0x08,

    0x3200,0x02, // xstart= 520
    0x3201,0x08,
    0x3204,0x08, // xend = 2063  1544 cols selected
    0x3205,0x0f,

    0x3211,0x04,  // xstart
    0x3213,0x04,  // ystart

    0x3208,0x06,  // 1536x1536
    0x3209,0x00,
    0x320a,0x06,
    0x320b,0x00,

    //0513
    0x3312,0x06, // sa1 timing
    0x3340,0x03,
    0x3341,0x74,
    0x3342,0x01,
    0x3343,0x80,
    0x335d,0x2a, // cmp timing
    0x3348,0x03,
    0x3349,0x74,
    0x334a,0x01,
    0x334b,0x80,
    0x3368,0x03, // auto precharge
    0x3369,0x27,
    0x336a,0x06,
    0x336b,0x27,
    0x3367,0x02,
    0x330e,0x17,

    0x3d08,0x03, // pclk inv

    //fifo
    0x303f,0x82,
    0x3c03,0x28, //fifo sram read position
    0x3c00,0x45, // Dig SRAM reset

    //0607
    0x3c03,0x02, //anti smear
    0x3211,0x06,
    0x3213,0x06,
    0x3620,0x82,


    //logic change@ gain<2
    0x3630,0xb1, //0x67
    0x3635,0x65, //3D //0x60, //0x66

    //0704
    0x3630,0x67,
    0x3626,0x11,

    //0910
    0x363c,0x88, //fine gain correction
    0x3312,0x00,
    0x3333,0x80,
    0x3334,0xa0,
    0x3620,0x62,  //0xd2
    0x3300,0x10,

    //0912
    0x3627,0x06,
    0x3312,0x06,
    0x3340,0x02,
    0x3341,0x80,
    0x3348,0x03,
    0x3349,0xb0,
    0x3334,0x20,

    0x331e,0x10,
    0x331f,0x13,
    0x3320,0x18,
    0x3321,0x18,

    //0922
    0x3300,0x18,
    0x3343,0xa8,
    0x334b,0xa8,
    0x331f,0x10,
    0x3633,0x3d, //for better dark current
    0x363a,0x04,

    //1536X1536
    0x3208,0x06,
    0x3209,0x00,

    0x3640,0x01,

    0x3f05,0x60,

    0x3211,0x04,

    0x0100,0x00,    //stream output off
};

static IMG_UINT16 registers_1536_1536_15fps[] = {
    //0x0103,0x01,// soft reset
    0x0100,0x00,

    0x4500,0x31, // rnc sel
    0x3416,0x11,
    0x4501,0xa4, // bit ctrl

    0x3e03,0x03, // aec
    0x3e08,0x00,
    0x3e09,0x10,
    0x3e01,0x30,

    //old timing
    0x322e,0x00,
    0x322f,0xaf,
    0x3306,0x20,
    0x3307,0x17,
    0x330b,0x54,
    0x3303,0x20,
    0x3309,0x20,
    0x3308,0x08,
    0x331e,0x16,
    0x331f,0x16,
    0x3320,0x18,
    0x3321,0x18,
    0x3322,0x18,
    0x3323,0x18,
    0x330c,0x0b,
    0x330f,0x07,
    0x3310,0x42,
    0x3324,0x07,
    0x3325,0x07,
    0x335b,0xca,
    0x335e,0x07,
    0x335f,0x10,
    0x3334,0x00,

    //mem write
    0x3F01,0x04,
    0x3F04,0x01,
    0x3F05,0x30,
    0x3626,0x01,

    //analog
    0x3635,0x60,
    0x3631,0x84,
    0x3636,0x8d, //0607
    0x3633,0x3f,
    0x3639,0x80,
    0x3622,0x1e,
    0x3627,0x02,
    0x3038,0xa4,
    0x3621,0x18,
    0x363a,0x1c,

    //ramp
    0x3637,0xbe,
    0x3638,0x85,
    0x363c,0x48, // ramp cur

    0x5780,0xff, // dpc
    0x5781,0x04,
    0x5785,0x10,

    //close mipi
    0x301c,0xa4, // dig
    0x301a,0xf8, // ana
    0x3019,0xff,
    0x3022,0x13,

    0x301e,0xe0, // [4] 0:close tempsens
    0x3662,0x82,
    0x3d0d,0x00, // close random code

    //2048x1536
    0x3039,0x00,
    0x303a,0x32, //75.6M pclk
    0x303b,0x02,

    0x3306,0x46,
    0x330b,0xa0,
    0x3038,0xf8, //pump clk div

    0x320c,0x03,  // hts=1600
    0x320d,0x20,
    0x320e,0x06,  // vts=1575
    0x320f,0x27,

    0x3202,0x00, // ystart=0
    0x3203,0x00,
    0x3206,0x06, // yend=1544    1545 rows selected
    0x3207,0x08,

    0x3200,0x02, // xstart= 520
    0x3201,0x08,
    0x3204,0x08, // xend = 2063  1544 cols selected
    0x3205,0x0f,

    0x3211,0x04,  // xstart
    0x3213,0x04,  // ystart

    0x3208,0x06,  // 1536x1536
    0x3209,0x00,
    0x320a,0x06,
    0x320b,0x00,

    //0513
    0x3312,0x06, // sa1 timing
    0x3340,0x03,
    0x3341,0x74,
    0x3342,0x01,
    0x3343,0x80,
    0x335d,0x2a, // cmp timing
    0x3348,0x03,
    0x3349,0x74,
    0x334a,0x01,
    0x334b,0x80,
    0x3368,0x03, // auto precharge
    0x3369,0x27,
    0x336a,0x06,
    0x336b,0x27,
    0x3367,0x02,
    0x330e,0x17,

    0x3d08,0x03, // pclk inv

    //fifo
    0x303f,0x82,
    0x3c03,0x28, //fifo sram read position
    0x3c00,0x45, // Dig SRAM reset

    //0607
    0x3c03,0x02, //anti smear
    0x3211,0x06,
    0x3213,0x06,
    0x3620,0x82,


    //logic change@ gain<2
    0x3630,0xb1, //0x67
    0x3635,0x60, //0x66

    //0704
    0x3630,0x67,
    0x3626,0x11,

    //0910
    0x363c,0x88, //fine gain correction
    0x3312,0x00,
    0x3333,0x80,
    0x3334,0xa0,
    0x3620,0x62,  //0xd2
    0x3300,0x10,

    //0912
    0x3627,0x06,
    0x3312,0x06,
    0x3340,0x02,
    0x3341,0x80,
    0x3348,0x03,
    0x3349,0x10,
    0x3334,0x20,

    0x331e,0x10,
    0x331f,0x13,
    0x3320,0x18,
    0x3321,0x18,

    //0922
    0x3300,0x18,
    0x3343,0xa8,
    0x334b,0xa8,
    0x331f,0x10,
    0x3633,0x3d, //for better dark current
    0x363a,0x04,

    //1536X1536
    0x3208,0x06,
    0x3209,0x00,

    //20170321
    0x303b,0x12,
    0x330b,0x48,
    0x3306,0x46,

    0x3640,0x01,

    0x3f05,0x60,

    0x0100,0x00, //stream output off

};


static SENSOR_MODE_STRUCT sensor_modes[] = {
    {{10, 2048, 1536, 25, 118.8, 3000, 1584, SENSOR_FLIP_BOTH, 25, 40000}, 1, 1, 25253,
        registers_1536p_25fps, ARRAY_SIZE(registers_1536p_25fps)},
    {{10, 1536, 1536, 25, 75.6, 1920, 1575, SENSOR_FLIP_BOTH, 25, 40000}, 1, 1, 25397,
        registers_1536_1536_25fps, ARRAY_SIZE(registers_1536_1536_25fps)},
    {{10, 1536, 1536, 15, 37.8, 1600, 1575, SENSOR_FLIP_BOTH, 42, 66666}, 1, 1, 42328,
        registers_1536_1536_15fps, ARRAY_SIZE(registers_1536_1536_15fps)},
};

SENSOR_MODE_STRUCT currentSensorMode;
static IMG_UINT8 mode_num = 0;

/*===========================================================================*/
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

    if(access(extra_cfg, F_OK) == 0){
  //  if(0){
        FILE * fp = NULL;
        IMG_UINT16 val[2];
        char ln[256];
        char *pstr = NULL;
        char *pt = NULL; //for macro delete_space used
        int  i = 0;

        memset(&currentSensorMode, 0, sizeof(currentSensorMode));
        fp = fopen(extra_cfg, "r");
        if (NULL == fp) {
            LOG_ERROR("open %s failed\n", extra_cfg);
            return IMG_ERROR_FATAL;
        }
        printf("use configuration file configure sensor!\n");
        memset(ln, 0, sizeof(ln));
        while(fgets(ln, sizeof(ln), fp) != NULL) {
            i = 0;
            pt = ln;
            delete_space(pt);
 //         printf("ln = %s\n", ln);
            if (NULL != strstr(ln, "0x")) {
                pstr = strtok(ln, ",");
                while(pstr != NULL) {

                    if(NULL != strstr(pstr, "0x")) {
                        val[i] = strtoul(pstr, NULL, 0);
                        i++;
                    }

                    pstr = strtok(NULL, ",");

                }
      //        printf("val[0] = 0x%x, val[1]=0x%x,\n", val[0], val[1]);
                ret = sensor_i2c_write(i2c, val[0], val[1]);
                if(ret != IMG_SUCCESS)
                {
                   return ret;
                }

            }else if (0 == strncmp(ln, "hsync", 5)) {
                pstr = strtok(ln, ",");
                currentSensorMode.hsync = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("hsync = %d\n", currentSensorMode.hsync);
            }else if (0 == strncmp(ln, "vsync", 5)) {
                pstr = strtok(ln, ",");
                currentSensorMode.vsync = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("vsync = %d\n", currentSensorMode.vsync);
            }else if (0 == strncmp(ln, "line_time", 5)) {
                pstr = strtok(ln, ",");
                currentSensorMode.line_time = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("line_time = %d\n", currentSensorMode.line_time);
            }else if (0 == strncmp(ln, "bitDepth", 8)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui8BitDepth = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui8BitDepth = %d\n", currentSensorMode.mode.ui8BitDepth);
            }else if (0 == strncmp(ln, "width", 5)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui16Width= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui16Width = %d\n", currentSensorMode.mode.ui16Width);
            }else if (0 == strncmp(ln, "height", 6)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui16Height = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui16Height = %d\n", currentSensorMode.mode.ui16Height);
            }else if (0 == strncmp(ln, "frameRate", 9)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.flFrameRate = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("flFrameRate = %f\n", currentSensorMode.mode.flFrameRate);
            }else if (0 == strncmp(ln, "pixelRate", 9)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.flPixelRate = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("flPixelRate = %f\n", currentSensorMode.mode.flPixelRate);
            }else if (0 == strncmp(ln, "horizontalTotal", 9)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui16HorizontalTotal = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui16HorizontalTotal = %d\n", currentSensorMode.mode.ui16HorizontalTotal);
            }else if (0 == strncmp(ln, "verticalTotal", 13)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui16VerticalTotal= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui16VerticalTotal = %d\n", currentSensorMode.mode.ui16VerticalTotal);
            }else if (0 == strncmp(ln, "flipping", 8)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui8SupportFlipping = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui8SupportFlipping = %d\n", currentSensorMode.mode.ui8SupportFlipping);
            }else if (0 == strncmp(ln, "exposureMin", 11)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui32ExposureMin= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui32ExposureMin = %d\n", currentSensorMode.mode.ui32ExposureMin);
            }else if (0 == strncmp(ln, "exposureMax", 11)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui32ExposureMax= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui32ExposureMax = %d\n", currentSensorMode.mode.ui32ExposureMax);
            }else if (0 == strncmp(ln, "mipiLanes", 9)) {
                pstr = strtok(ln, ",");
                currentSensorMode.mode.ui8MipiLanes = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
                LOG_INFO("ui8MipiLanes = %d\n", currentSensorMode.mode.ui8MipiLanes);
            }
        }
    }else{
        memcpy((void*)&currentSensorMode,
            (void*)&(sensor_modes[mode_id]), sizeof(currentSensorMode));
        registers = currentSensorMode.registers;
        reg_num = currentSensorMode.reg_num;

        LOG_INFO("mode_id=%d reg_num=%d >>>>>>>>>>>>\n", mode_id, reg_num);

        for (i = 0; i < reg_num; i+=2) {
            ret = sensor_i2c_write(i2c, registers[i], registers[i+1]);
            if (ret != IMG_SUCCESS) {
                LOG_ERROR("Set mode %d failed\n", mode_id);
                return ret;
            }
        }

    }

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

    if (hflip) {
        sensor_i2c_write(i2c, 0x3221, 0x06);
    }
    else {
        sensor_i2c_write(i2c, 0x3221, 0x00);
    }

    if (vflip) {
        sensor_i2c_write(i2c, 0x3220, 0x06);
    }
    else {
        sensor_i2c_write(i2c, 0x3220, 0x00);
    }

    return IMG_SUCCESS;
}

static IMG_UINT32 sensor_calculate_gain(double flGain)
{
    IMG_UINT32 gain_integer, val_integer, val_decimal, val_gain;
    double gain_decimal;

    gain_integer = floor(flGain);
    gain_decimal = flGain - gain_integer;
    val_decimal = round(gain_decimal * 16);
    val_integer = gain_integer * 16;
    val_gain = val_integer+val_decimal;

    return val_gain;
}

static IMG_UINT32 sensor_calculate_exposure(
    SENSOR_CAM_STRUCT *psCam, IMG_UINT32 expo_time)
{
    IMG_UINT32 exposure_lines;

    exposure_lines = expo_time * 1000/currentSensorMode.line_time;
    if (exposure_lines < 1)
        exposure_lines = 1;
    else if (exposure_lines >= 4096) {
        LOG_WARNING("SC3035 Limit: exposure lines %d(%dus) "
                         "must NOT exceed 12bits register max value 4096\n",
                        exposure_lines, expo_time);
        exposure_lines = 4095;
    }

    return exposure_lines;
}

static void sensor_write_gain(SENSOR_CAM_STRUCT *psCam,
    IMG_UINT32 val_gain)
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

static void sensor_write_exposure(SENSOR_CAM_STRUCT *psCam,
    IMG_UINT32 exposure_lines)
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
    IMG_UINT8 val;
    sensor_i2c_read(psCam->i2c, 0x3109, &val);
    if (flGain < 2) {
        sensor_i2c_write(psCam->i2c, 0x3630, 0xa9);
        sensor_i2c_write(psCam->i2c, 0x3627, 0x02);
    }
    else if (flGain >= 2 && flGain <= 16) {
        sensor_i2c_write(psCam->i2c, 0x3630, 0x67);
        sensor_i2c_write(psCam->i2c, 0x3627, 0x02);
    }
    else {
        sensor_i2c_write(psCam->i2c, 0x3630, 0x67);
        sensor_i2c_write(psCam->i2c, 0x3627, 0x06);
    }
    if(val == 0x02)
    {
        if(flGain < 2){
            sensor_i2c_write(psCam->i2c, 0x3620, 0xd2);
        }else{
            sensor_i2c_write(psCam->i2c, 0x3620, 0x62);
        }
    }
    else if(val == 0x03)
    {
        sensor_i2c_write(psCam->i2c, 0x3620, 0x62);
    }
}
#endif

static IMG_RESULT sGetMode(SENSOR_HANDLE hHandle,
    IMG_UINT16 mode_id, SENSOR_MODE *smode)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    LOG_INFO("mode_id=%d >>>>>>>>>>>>\n", mode_id);
    TUNE_SLEEP(1);
    ASSERT_INITIALIZED(psCam->sensor_phy);
    IMG_ASSERT(smode);
    ASSERT_MODE_RANGE(mode_id);
    IMG_MEMCPY(smode, &currentSensorMode.mode, sizeof(SENSOR_MODE));
    return IMG_SUCCESS;
}

static IMG_RESULT sGetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);

    //LOG_INFO(">>>>>>>>>>>>\n");
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

static IMG_RESULT sSetMode(SENSOR_HANDLE hHandle,
    IMG_UINT16 mode_id, IMG_UINT8 flipping)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    IMG_RESULT ret;

    LOG_INFO("mode=%d, flipping=0x%02X >>>>>>>>>>>>\n", mode_id, flipping);
    TUNE_SLEEP(1);

    ASSERT_INITIALIZED(psCam->sensor_phy);
    ASSERT_MODE_RANGE(mode_id);

    /* Config registers */
    ret = sensor_config_register(psCam->i2c, mode_id);
    if (ret)
        goto out_failed;
    usleep(2000);


    /* Config flipping */
    ret = sensor_flip_image(psCam->i2c, flipping & SENSOR_FLIP_HORIZONTAL,
        flipping & SENSOR_FLIP_VERTICAL);
    if (ret)
        goto out_failed;

    /* Init sensor status */
    psCam->enabled = IMG_FALSE;
    psCam->mode_id = mode_id;
    psCam->flipping = flipping;
    psCam->exposure = SENSOR_EXPOSURE_DEFAULT;
    psCam->gain = 1;
    psCam->gain_reg_val = 0;
    psCam->cur_fps = currentSensorMode.mode.flFrameRate;

    /* Init ISP gasket params */
    psCam->sensor_phy->psGasket->uiGasket  = psCam->imager;
    psCam->sensor_phy->psGasket->bParallel = IMG_TRUE;
    psCam->sensor_phy->psGasket->uiWidth   = currentSensorMode.mode.ui16Width-1;
    psCam->sensor_phy->psGasket->uiHeight  = currentSensorMode.mode.ui16Height-1;
    psCam->sensor_phy->psGasket->bVSync    = currentSensorMode.vsync;
    psCam->sensor_phy->psGasket->bHSync    = currentSensorMode.hsync;
    psCam->sensor_phy->psGasket->ui8ParallelBitdepth = \
        currentSensorMode.mode.ui8BitDepth;

    LOG_DEBUG("gasket=%d, width=%d, height=%d, vsync=%d, hsync=%d,bitdepth=%d\n",
        psCam->sensor_phy->psGasket->uiGasket, psCam->sensor_phy->psGasket->uiWidth,
        psCam->sensor_phy->psGasket->uiHeight, psCam->sensor_phy->psGasket->bVSync,
        psCam->sensor_phy->psGasket->bHSync,
        psCam->sensor_phy->psGasket->ui8ParallelBitdepth);

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
    psInfo->ui32WellDepth = 7500;
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = imgr;
    psInfo->bBackFacing = IMG_TRUE;
#ifdef INFOTM_ISP
    psInfo->ui32ModeCount = mode_num;
#endif

    return IMG_SUCCESS;
}

static IMG_RESULT sGetGainRange(SENSOR_HANDLE hHandle,
    double *pflMin, double *pflMax, IMG_UINT8 *pui8Contexts)
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

static IMG_RESULT sGetCurrentGain(SENSOR_HANDLE hHandle,
    double *pflGain, IMG_UINT8 ui8Context)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    LOG_DEBUG(">>>>>>>>>>>>\n");
    TUNE_SLEEP(1);
    ASSERT_INITIALIZED(psCam->sensor_phy);
    IMG_ASSERT(pflGain);
    *pflGain = psCam->gain;
    return IMG_SUCCESS;
}

static IMG_RESULT sSetGain(SENSOR_HANDLE hHandle,
    double flGain, IMG_UINT8 ui8Context)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    IMG_UINT32 val_gain;
    int ret;

    //LOG_INFO("flGain=%lf >>>>>>>>>>>>\n", flGain);
    TUNE_SLEEP(1);

    ASSERT_INITIALIZED(psCam->sensor_phy);

    if (flGain > SENSOR_MAX_GAIN)
        flGain = SENSOR_MAX_GAIN;
    else if (flGain < 1)
        flGain = 1;
#if 0
    ret = pthread_mutex_lock(&psCam->lock);
    if (ret) {
        LOG_ERROR("pthread mutex lock failed %d\n", ret);
        return IMG_ERROR_BUSY;
    }
#endif
    psCam->gain = flGain;

    val_gain = sensor_calculate_gain(flGain);

    sensor_write_gain(psCam, val_gain);

#ifdef NOISE_LOGIC_ENABLE
    noise_logic_gain(psCam, flGain);
#endif
    LOG_DEBUG("flGain=%lf, register value=0x%02x, actual gain=%lf\n",
        flGain, val_gain, ((double)val_gain)/16);
//    pthread_mutex_unlock(&psCam->lock);

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
    *pui32Min = currentSensorMode.mode.ui32ExposureMin;
#if !defined(IQ_TUNING)
    *pui32Max = currentSensorMode.mode.ui32ExposureMax;
#else
    *pui32Max = 600000;
#endif
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

static IMG_RESULT sGetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    LOG_DEBUG(">>>>>>>>>>>>\n");
    TUNE_SLEEP(1);
    ASSERT_INITIALIZED(psCam->sensor_phy);
    IMG_ASSERT(pi32Current);
    *pi32Current = psCam->exposure;
    return IMG_SUCCESS;
}

static IMG_RESULT sSetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    IMG_UINT32 exposure_lines;
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

    LOG_INFO("flip=0x%02X >>>>>>>>>>>>\n", ui8Flag);
    TUNE_SLEEP(1);

    ASSERT_INITIALIZED(psCam->sensor_phy);

    if (ui8Flag != psCam->flipping) {
        sensor_flip_image(psCam->i2c, ui8Flag & SENSOR_FLIP_HORIZONTAL,
            ui8Flag & SENSOR_FLIP_VERTICAL);
        psCam->flipping = ui8Flag;
    }

    return IMG_SUCCESS;
}

static IMG_RESULT sSetGainAndExposure(SENSOR_HANDLE hHandle,
    double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    IMG_UINT32 val_gain, exposure_lines;

    LOG_DEBUG("gain=%lf, exposure=%d >>>>>>>>>>>>\n", flGain, ui32Exposure);
    TUNE_SLEEP(1);

    psCam->gain = flGain;
    psCam->exposure = ui32Exposure;

    val_gain = sensor_calculate_gain(flGain);
    exposure_lines = sensor_calculate_exposure(psCam, ui32Exposure);

    sensor_write_gain(psCam, val_gain);
    sensor_write_exposure(psCam, exposure_lines);

#ifdef NOISE_LOGIC_ENABLE
    noise_logic_gain(psCam, flGain);
#endif
    LOG_DEBUG("flGain=%lf, register value=0x%02x, actual gain=%lf\n",
        flGain, val_gain, ((double)val_gain)/16);
    LOG_DEBUG("SetExposure. time=%d us, lines = %d\n",
        psCam->exposure, exposure_lines);

    return IMG_SUCCESS;
}

static IMG_RESULT sGetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
    LOG_DEBUG(">>>>>>>>>>>>\n");
    TUNE_SLEEP(1);
    if (pfixed != NULL) {
        *pfixed = currentSensorMode.mode.flFrameRate;
        LOG_DEBUG("Fixed FPS=%d\n", *pfixed);
    }
    return IMG_SUCCESS;
}

static IMG_RESULT sSetFPS(SENSOR_HANDLE hHandle, double fps)
{
    SENSOR_CAM_STRUCT *psCam = container_of(hHandle, SENSOR_CAM_STRUCT, funcs);
    IMG_UINT32 fixed_fps = currentSensorMode.mode.flFrameRate;
    IMG_UINT32 fixed_frame_length = currentSensorMode.mode.ui16VerticalTotal;
    IMG_UINT32 frame_length, denoise_length;
    double down_ratio;
    LOG_DEBUG(">>>>>>>>>>>>\n");
    TUNE_SLEEP(1);

    if (fps == psCam->cur_fps) {
        LOG_INFO("fps %lf = cur_fps, skip operation\n", fps);
        return IMG_SUCCESS;
    }
    if (fps < 1) {
        LOG_ERROR("Invalid fps %lf, force to 1, check fps param!\n", fps);
        fps = 1;
    }
    if (fps > fixed_fps) {
        LOG_WARNING("Invalid fps %lf, force to fixed_fps %d, check fps param!\n",
            fps, fixed_fps);
        fps = fixed_fps;
    }

    down_ratio = fixed_fps / fps;

    frame_length = round(fixed_frame_length * down_ratio);
    sensor_i2c_write(psCam->i2c, SENSOR_FRAME_LENGTH_H, (frame_length>>8) & 0xFF);
    sensor_i2c_write(psCam->i2c, SENSOR_FRAME_LENGTH_L, frame_length & 0xFF);
#ifdef NOISE_LOGIC_ENABLE
    denoise_length = frame_length - 0x300;
    if (denoise_length < 0)
        denoise_length = 0;
    sensor_i2c_write(psCam->i2c, 0x336a, (frame_length>>8) & 0xFF);
    sensor_i2c_write(psCam->i2c, 0x336b, frame_length & 0xFF);
    sensor_i2c_write(psCam->i2c, 0x3368, (denoise_length>>8) & 0xFF);
    sensor_i2c_write(psCam->i2c, 0x3369, denoise_length & 0xFF);
#endif
    psCam->cur_fps = fps;
    LOG_INFO("down_ratio = %lf, frame_length = %d, "
                "fixed_fps = %d, cur_fps = %lf >>>>>>>>>>>>\n",
                down_ratio, frame_length, fixed_fps, psCam->cur_fps);

    return IMG_SUCCESS;
}
#endif

#ifdef HIGH_TEMPERATURE_LOGIC
void * SC3035DVP_HighTemperatureLogic(void * arg)
{
    SENSOR_CAM_STRUCT *psCam = (SENSOR_CAM_STRUCT *)arg;
    int ret;
    IMG_UINT16 R_val;
    IMG_UINT8  tmp, tmp1;
    IMG_UINT32 curr_gain;
    int adjust;
    int i = 0;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    while(1) {
        if (psCam->enabled) {
            ret = pthread_mutex_trylock(&psCam->lock);
            if (ret) {
                usleep(10000);
                goto psleep;
            }
            //LOG_INFO("LOGIC EXECUTE!!!R=0x%02x @@@@@@@@@@@@@\n", R_val);
            adjust = 0;
            R_val = 0;
            sensor_i2c_read(psCam->i2c, 0x3911, &tmp1);
            R_val = tmp1<<8;
            sensor_i2c_read(psCam->i2c, 0x3912, &tmp);
            R_val += tmp;
            sensor_i2c_read(psCam->i2c, 0x3911, &tmp);
            if(tmp1 != tmp)
            {
                continue;
            }
            if(++i>10)
            {
                i=0;
                LOG_DEBUG("## R=0x%02x, limit=%d, current gain reg is 0x%02x\n",
                            R_val, psCam->gain_reg_val_limit, psCam->gain_reg_val);
            }
            if (R_val >= 0x11F0  && psCam->gain_reg_val_limit > SENSOR_MIN_GAIN_REG_VAL) {
                adjust = 1;
                LOG_INFO("### R=0x%02x, >= 0x11F0, limit=%d, current gain reg is 0x%02x, "
                        "flGain is %lf, decrease <-\n",
                        R_val, psCam->gain_reg_val_limit, psCam->gain_reg_val, psCam->gain);
                psCam->gain_reg_val_limit = psCam->gain_reg_val_limit - 0x4;

            }
            else if (R_val <= 0x1160 && psCam->gain_reg_val_limit < SENSOR_MAX_GAIN_REG_VAL) {
                adjust = 1;
                LOG_INFO("### R=0x%02x, <= 0x1160, limit=%d, current gain reg is 0x%02x,"
                        " flGain is %lf, increase ->\n",
                        R_val, psCam->gain_reg_val_limit, psCam->gain_reg_val, psCam->gain);
                psCam->gain_reg_val_limit = psCam->gain_reg_val_limit + 0x4;
                if (psCam->gain_reg_val_limit >= SENSOR_MAX_GAIN_REG_VAL)
                {
                    psCam->gain_reg_val_limit = SENSOR_MAX_GAIN_REG_VAL;
                }
            }
            LOG_DEBUG("psCam->gain_reg_val_limit = 0x%02X\n", psCam->gain_reg_val_limit);
            if (adjust) {
                sensor_write_gain(psCam, psCam->gain_reg_val);
            }
            ret = pthread_mutex_unlock(&psCam->lock);
            usleep(12000000/psCam->cur_fps);
        }

psleep:
        pthread_testcancel();
        sleep(1);
        pthread_testcancel();
    }
}
#endif

IMG_RESULT SC3035DVP_Create(struct _Sensor_Functions_ **phHandle, int index)
{
    char i2c_dev_path[NAME_MAX];
    SENSOR_CAM_STRUCT *psCam;
    IMG_RESULT ret;
    pthread_mutexattr_t * attrp;

    char dev_nm[64];
    char adaptor[64];
    int chn = 0;
#if defined(INFOTM_ISP)
    int fd = 0;
    memset((void *)dev_nm, 0, sizeof(dev_nm));
    memset((void *)adaptor, 0, sizeof(adaptor));
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
    close(fd);
    printf("%s opened OK, i2c-addr=0x%x, chn = %d\n", dev_nm, i2c_addr, chn);
    sprintf(adaptor, "%s-%d", "i2c", chn);
    memset((void *)extra_cfg, 0, sizeof(dev_nm));
    sprintf(extra_cfg, "%s%s%d-config.txt", EXTRA_CFG_PATH, "sensor" , index );
#endif

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
    psCam->imager = imgr;

    /* Init sensor state */
    psCam->enabled = IMG_FALSE;
    psCam->mode_id = 0;
    psCam->flipping = SENSOR_FLIP_NONE;
    psCam->exposure = SENSOR_EXPOSURE_DEFAULT;
    psCam->gain = 1;
    psCam->gain_reg_val = 0;
   psCam->gain_reg_val_limit = SENSOR_MAX_GAIN_REG_VAL;

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

#if 0
    /* Create thread for High Temperature Control */
    attrp = &psCam->attr;
    pthread_mutexattr_init(attrp);
    pthread_mutexattr_settype(attrp, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&psCam->lock, attrp);
#ifdef HIGH_TEMPERATURE_LOGIC
    pthread_create(&psCam->tid, NULL, SC3035DVP_HighTemperatureLogic, psCam);
    pthread_detach(psCam->tid);
#endif
#endif
    return IMG_SUCCESS;
}
