#include <math.h>
#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/sc3035dualdvp.h"

#define LOG_TAG "SC3035_DUAL_DVP"
#include <felixcommon/userlog.h>

#define SENSOR_MAX_GAIN			124
#define SENSOR_MAX_GAIN_REG_VAL			0x7c0
#define SENSOR_MIN_GAIN_REG_VAL			0x82

#define SENSOR_GAIN_REG_ADDR_H		0x3e08
#define SENSOR_GAIN_REG_ADDR_L		0x3e09

#define SENSOR_EXPOSURE_REG_ADDR_H	0x3e01
#define SENSOR_EXPOSURE_REG_ADDR_L	0x3e02
#define SENSOR_EXPOSURE_DEFAULT		31111

// in us. used for delay. consider the lowest fps
#define SENSOR_MAX_FRAME_TIME			100000

#define SENSOR_FRAME_LENGTH_H		0x320e
#define SENSOR_FRAME_LENGTH_L		0x320f

IMG_RESULT sc3035dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
IMG_RESULT  sc3035dvp_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
IMG_RESULT sc3035dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info);
IMG_RESULT sc3035dvp_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info);
IMG_RESULT sc3035dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info);
IMG_RESULT sc3035dvp_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min,IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT sc3035dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);
IMG_RESULT sc3035dvp_check_id(BRIDGE_I2C_CLIENT* i2c);
IMG_RESULT sc3035dvp_enable(BRIDGE_I2C_CLIENT* i2c, int on);

IMG_RESULT sc3035dvp_set_fps(BRIDGE_I2C_CLIENT* i2c,
    double fps, SENSOR_MODE *info);
IMG_RESULT sc3035dvp_get_fixedfps(BRIDGE_I2C_CLIENT* i2c,
    double* fps, SENSOR_MODE *info);


BRIDGE_SENSOR sc3035dvp_sensor = {
    .name = "sc3035dvp",
    .set_exposure = sc3035dvp_set_exposure,
    .set_gain = sc3035dvp_set_gain,
    .get_exposure = sc3035dvp_get_exposure,
    .get_gain = sc3035dvp_get_gain,
    .get_exposure_range = sc3035dvp_get_exposure_range,
    .get_gain_range = sc3035dvp_get_gain_range,
    .set_exposure_gain = sc3035dvp_set_exposure_gain,
    .check_id = sc3035dvp_check_id,
    .enable = sc3035dvp_enable,
    .set_fps = sc3035dvp_set_fps,
    .get_fixedfps = sc3035dvp_get_fixedfps,
    .next =NULL,
    .prev = NULL,
};

void sc3035dvp_init(void)
{
    bridge_sensor_register(&sc3035dvp_sensor);
}

IMG_RESULT sc3035dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info)
{
    IMG_RESULT ret = IMG_SUCCESS;
    //TODO
    return ret;
}

IMG_RESULT  sc3035dvp_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info)
{
    IMG_RESULT ret = IMG_SUCCESS;
    //TODO
    return ret;
}

IMG_RESULT sc3035dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp ,double gain, SENSOR_MODE *info)
{
    IMG_RESULT ret = IMG_SUCCESS;
    //TODO
    return ret;
}

IMG_RESULT sc3035dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info)
{
    //TODO
    return IMG_SUCCESS;
}
IMG_RESULT sc3035dvp_get_gain(BRIDGE_I2C_CLIENT* i2c, double* gain,
    SENSOR_MODE *info)
{
    //TODO
    return IMG_SUCCESS;
}

IMG_RESULT sc3035dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info)
{
    //TODO
    return IMG_SUCCESS;
}
IMG_RESULT sc3035dvp_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info)
{
    //TODO
    return IMG_SUCCESS;
}

IMG_RESULT sc3035dvp_check_id(BRIDGE_I2C_CLIENT* i2c)
{
    //TODO
    return IMG_SUCCESS;
}

IMG_RESULT sc3035dvp_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 aui16Regs[] = {
        0x0100, 0x00,
    };

    if (on)
    {
        aui16Regs[1] = 0x01; /* Stream on */
    }
    else
    {
        aui16Regs[1] = 0x00; /* Stream off */
    }

    LOG_INFO("@ stream %s\n", (on ? "on":"off"));
    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);

    return ret;
}

IMG_RESULT sc3035dvp_set_fps(BRIDGE_I2C_CLIENT* i2c,
    double fps, SENSOR_MODE *info)
{
    IMG_UINT32 fixed_fps = info->flFrameRate;
    IMG_UINT32 fixed_frame_length = info->ui16VerticalTotal;
    IMG_UINT32 frame_length;
    double down_ratio;

    if (fps < 1)
    {
        LOG_ERROR("Invalid fps %lf, force to 1, check fps param!\n", fps);
        fps = 1;
    }

    if (fps > fixed_fps)
    {
        LOG_WARNING("Invalid fps %lf, force to fixed_fps %d, check fps param!\n",
        fps, fixed_fps);
        fps = fixed_fps;
    }

    down_ratio = fixed_fps / fps;

    frame_length = round(fixed_frame_length * down_ratio);
    bridge_i2c_write(i2c, SENSOR_FRAME_LENGTH_H, (frame_length>>8) & 0xFF);
    bridge_i2c_write(i2c, SENSOR_FRAME_LENGTH_L, frame_length & 0xFF);

    LOG_INFO("down_ratio = %lf, frame_length = %d, fixed_fps = %d, cur_fps = %lf\n",
        down_ratio, frame_length, fixed_fps, fps);

    return IMG_SUCCESS;
}

IMG_RESULT sc3035dvp_get_fixedfps(BRIDGE_I2C_CLIENT* i2c,
    double* fps, SENSOR_MODE *info)
{
    //TODO
    return IMG_SUCCESS;
}
