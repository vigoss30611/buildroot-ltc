#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/imx291dualdvp.h"
#include <felixcommon/userlog.h>
#include <math.h>

//#ifdef LOG_DEBUG
//#undef LOG_DEBUG
//#define LOG_DEBUG LOG_ERROR
//#endif

#define IMX291_USING_REGHOLD

#define LOG_TAG		"IMX291DualDvp"

#define SENSOR_MAX_GAIN				3981
#define SENSOR_GAIN_REG_ADDR			0x3014
#define SENSOR_GAIN_DBM				3 // 0.3
#define SENSOR_EXPOSURE_REG_ADDR_H	0x3022
#define SENSOR_EXPOSURE_REG_ADDR_M	0x3021
#define SENSOR_EXPOSURE_REG_ADDR_L	0x3020
#define SENSOR_FRAME_LENGTH_H		0x301A
#define SENSOR_FRAME_LENGTH_M		0x3019
#define SENSOR_FRAME_LENGTH_L		0x3018
#define SENSOR_EXPOSURE_DEFAULT		31111

static IMG_RESULT imx291dvp_enable(BRIDGE_I2C_CLIENT* i2c,int on);
static IMG_RESULT imx291dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
static IMG_RESULT imx291dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *exp, SENSOR_MODE *info);
static IMG_RESULT imx291dvp_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
static IMG_RESULT imx291dvp_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double *gain, SENSOR_MODE *info);
static IMG_RESULT imx291dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min, double *max, SENSOR_MODE *info);
static IMG_RESULT imx291dvp_set_fps(BRIDGE_I2C_CLIENT* i2c,
    double fps, SENSOR_MODE *info);
static IMG_RESULT imx291dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);

static BRIDGE_SENSOR imx291dvp_sensor = {
    .name = "imx291dvp",
    .enable = imx291dvp_enable,
    .set_exposure = imx291dvp_set_exposure,
    .get_exposure = imx291dvp_get_exposure,
    .set_gain = imx291dvp_set_gain,
    .get_gain = imx291dvp_get_gain,
    .get_gain_range = imx291dvp_get_gain_range,
    .set_fps = imx291dvp_set_fps,
    .set_exposure_gain = imx291dvp_set_exposure_gain,
    NULL,
};

typedef struct _sensor_cam
{
    /* Sensor status */
    IMG_UINT8 flipping;
    IMG_UINT32 exposure;
    IMG_UINT32 expo_reg_val;		// current exposure register value
    double gain;
    IMG_UINT8 gain_reg_val;		// current gain register value
#if defined(INFOTM_ISP)
    double fCurrentFps;
#endif
}SENSOR_CAM_STRUCT;

static SENSOR_CAM_STRUCT gsCam;

void imx291dvp_init(void)
{
    bridge_sensor_register(&imx291dvp_sensor);

    gsCam.flipping = SENSOR_FLIP_NONE;
    gsCam.exposure = SENSOR_EXPOSURE_DEFAULT;
    gsCam.gain = 1;
    gsCam.gain_reg_val = 0;
    gsCam.fCurrentFps = 30;
}

static void imx291dvp_reghold(BRIDGE_I2C_CLIENT* i2c, int hold)
{
#ifdef IMX291_USING_REGHOLD
    IMG_UINT8 val = hold ? 0x01 : 0x00;
    bridge_i2c_write(i2c, 0x3001, val);
#endif
}

IMG_RESULT imx291dvp_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    return IMG_SUCCESS;
}

static IMG_UINT32 imx291dvp_calculate_exposure(
    const SENSOR_MODE *info, IMG_UINT32 expo_time)
{
    IMG_UINT32 exposure_lines = expo_time / info->ui32ExposureMin;
    if (exposure_lines < 1)
    {
        exposure_lines = 1;
    }

    IMG_UINT32 max_lines = info->ui16VerticalTotal / 2;
    if (exposure_lines > max_lines - 1)
    {
        exposure_lines = max_lines - 1;
    }
    return exposure_lines;
}

static void imx291dvp_write_exposure(BRIDGE_I2C_CLIENT* i2c,
    const SENSOR_MODE *info, IMG_UINT32 exposure_lines)
{
    double ratio = (double)info->flFrameRate / gsCam.fCurrentFps;
    IMG_UINT32 max_lines = info->ui16VerticalTotal * ratio;

    IMG_UINT32 SHS1 = max_lines - exposure_lines -1;
    if (SHS1 > max_lines - 2 || SHS1 > 262144)
    {
        LOG_ERROR("Invalid exposure SHS1 value %d, "
            "exposure_lines=%d, max_lines=%d\n", SHS1, exposure_lines, max_lines);
        return;
    }

    if (SHS1 == gsCam.expo_reg_val)
    {
        return;
    }
    bridge_i2c_write(i2c, SENSOR_EXPOSURE_REG_ADDR_H, (SHS1>>16) & 0x03);
    bridge_i2c_write(i2c, SENSOR_EXPOSURE_REG_ADDR_M, (SHS1>>8) & 0xFF);
    bridge_i2c_write(i2c, SENSOR_EXPOSURE_REG_ADDR_L, (SHS1>>0) & 0xFF);

    gsCam.expo_reg_val = SHS1;
}

IMG_RESULT imx291dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info)
{
    LOG_ERROR("curfps: %.2lf, exp: %u\n", gsCam.fCurrentFps, exp);
    IMG_UINT32 exposure_lines = imx291dvp_calculate_exposure(info, exp);

    imx291dvp_reghold(i2c, 1);
    imx291dvp_write_exposure(i2c, info,  exposure_lines);
    imx291dvp_reghold(i2c, 0);

    gsCam.exposure = exp;
    LOG_INFO("imx291DVP_SetExposure: Exposure = 0x%X, "
        "ExposureMin = 0x%X\n", exp, info->ui32ExposureMin);
    return IMG_SUCCESS;
}

IMG_RESULT imx291dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *exp, SENSOR_MODE *info)
{
    *exp = gsCam.exposure;
    return IMG_SUCCESS;
}

static IMG_UINT8 imx291dvp_calculate_gain(double flGain)
{
    double gain_db, gain_mod;
    IMG_UINT8 value;

    gain_db = 20 * log10(flGain);
    value = gain_db * 10 / SENSOR_GAIN_DBM;
    gain_mod = gain_db * 10 - (value * SENSOR_GAIN_DBM);
    if (gain_mod * 2 >= SENSOR_GAIN_DBM)
    {
        value++;
    }

    if (value > 240)
    {
        value = 240;	// max value 0xF0, 72 dB
        LOG_WARNING("Sensor max gain is 72dB(Analog 30dB+Digital 42dB), "
            "max reg value is 240(0xF0).\n");
    }

    return value;
}

static void imx291dvp_write_gain(BRIDGE_I2C_CLIENT* i2c, IMG_UINT8 val_gain)
{
    bridge_i2c_write(i2c, SENSOR_GAIN_REG_ADDR, val_gain);
}

IMG_RESULT imx291dvp_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info)
{
    LOG_ERROR("gain: %.2lf\n", gain);
    LOG_INFO("flGain=%lf >>>>>>>>>>>>\n", gain);
    IMG_UINT8 val_gain = imx291dvp_calculate_gain(gain);

    imx291dvp_reghold(i2c, 1);
    imx291dvp_write_gain(i2c, val_gain);
    imx291dvp_reghold(i2c, 0);

    gsCam.gain  = gain;

    return IMG_SUCCESS;
}

IMG_RESULT imx291dvp_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info)
{
    *gain = gsCam.gain;
    return IMG_SUCCESS;
}

IMG_RESULT imx291dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min, double *max, SENSOR_MODE *info)
{
    *min = 1.0;
    *max = SENSOR_MAX_GAIN;
    return IMG_SUCCESS;
}

IMG_RESULT imx291dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info)
{
    LOG_DEBUG("gain=%lf, exposure=%d >>>>>>>>>>>>\n", gain, exp);

    IMG_UINT32 val_gain = imx291dvp_calculate_gain(gain);
    IMG_UINT32 exposure_lines = imx291dvp_calculate_exposure(info, exp);

    imx291dvp_reghold(i2c, 1);
    imx291dvp_write_gain(i2c, val_gain);
    imx291dvp_write_exposure(i2c, info, exposure_lines);
    imx291dvp_reghold(i2c, 0);

    gsCam.exposure = exp;
    gsCam.gain = gain;

    LOG_DEBUG("flGain=%lf, register value=0x%02x\n", gain, val_gain);
    LOG_DEBUG("SetExposure. time=%d us, lines = %d\n", exp, exposure_lines);

    return IMG_SUCCESS;
}

static IMG_RESULT imx291dvp_set_fps(BRIDGE_I2C_CLIENT* i2c,
    double fps, SENSOR_MODE *info)
{
    LOG_DEBUG(">>>>>>>>>>>>\n");

    if (fps < 1.0)
    {
         LOG_WARNING("Invalid fps=%d, force to 1, check fps param in json file\n", fps);
         fps = 1;
         return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (fps > 30.0)
    {
        LOG_ERROR("Invalid down_ratio %d, too big\n", fps);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_UINT32 frame_length = round(info->ui16VerticalTotal * info->flFrameRate / fps);
    imx291dvp_reghold(i2c, 1);
    bridge_i2c_write(i2c, SENSOR_FRAME_LENGTH_H, (frame_length>>16) & 0x03);
    bridge_i2c_write(i2c, SENSOR_FRAME_LENGTH_M, (frame_length>>8) & 0xFF);
    bridge_i2c_write(i2c, SENSOR_FRAME_LENGTH_L, (frame_length>>0) & 0xFF);
    imx291dvp_reghold(i2c, 0);

    gsCam.fCurrentFps = fps;
    LOG_DEBUG("set fps = %lf, frame_length = %d, flFrameRate = %lf >>>>>>>>>>>>\n",
    fps, frame_length, info->flFrameRate);
    return IMG_SUCCESS;
}
