#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/k02dualmipi.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#define LOG_TAG "K02_DUAL_MIPI"
#include <felixcommon/userlog.h>

#define K02_MODEL_ID_H_REG (0x0A)
#define K02_MODEL_ID_L_REG (0x0B)

#define K02_MODEL_ID_H (0x04)
#define K02_MODEL_ID_L (0x03)

IMG_RESULT k02mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
IMG_RESULT k02mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
IMG_RESULT k02mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info);
IMG_RESULT k02mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info);
IMG_RESULT k02mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info);
IMG_RESULT k02mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT k02mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);
IMG_RESULT k02mipi_check_id(BRIDGE_I2C_CLIENT* i2c);
IMG_RESULT k02mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on);

BRIDGE_SENSOR k02mipi_sensor = {
    .name = "k02mipi",
    .set_exposure = k02mipi_set_exposure,
    .set_gain = k02mipi_set_gain,
    .get_exposure = k02mipi_get_exposure,
    .get_gain = k02mipi_get_gain,
    .get_exposure_range = k02mipi_get_exposure_range,
    .get_gain_range = k02mipi_get_gain_range,
    .set_exposure_gain = k02mipi_set_exposure_gain,
    .check_id = k02mipi_check_id,
    .enable = k02mipi_enable,
    .next =NULL,
    .prev = NULL,
};

void k02mipi_init(void)
{
    LOG_DEBUG("%s\n", __func__);
    bridge_sensor_register(&k02mipi_sensor);
}

static IMG_UINT8 g_K02Dual_ui8LinesValHiBackup = 0xff;
static IMG_UINT8 g_K02Dual_ui8LinesValLoBackup = 0xff;
static IMG_UINT8 g_K02Dual_ui8GainValBackup = 0xff;
static const double aec_min_gain = 1.0;
static const double aec_max_gain = 16.0;//16.0;
static uint32_t g_exposure_time = 48608;
static double g_gain = 1.0;
static int runing_flag = 0;

IMG_RESULT k02mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info)
{
    IMG_UINT32 ui32Lines;
    IMG_UINT8 exposureRegs[3];
    IMG_UINT8 Temp[2];

    printf("===>XC9080_K02_SetExposure\n");

    g_exposure_time = exp;
    if(g_exposure_time < info->ui32ExposureMin)
        g_exposure_time = info->ui32ExposureMin;
    if(g_exposure_time > info->ui32ExposureMax)
        g_exposure_time = info->ui32ExposureMax;

    ui32Lines = g_exposure_time /(info->ui32ExposureMin);
    if(ui32Lines < 2)
        ui32Lines=2;
    if(ui32Lines > (0x620 - 4))
        ui32Lines = 0x620 - 4;

    exposureRegs[0] = 0x01;
    exposureRegs[1] = (ui32Lines)&0xff;
    exposureRegs[2] = (ui32Lines >> 8);

    if ((g_K02Dual_ui8LinesValHiBackup != exposureRegs[2]) ||
            (g_K02Dual_ui8LinesValLoBackup != exposureRegs[1]))
    {
        g_K02Dual_ui8LinesValHiBackup = exposureRegs[2];
        g_K02Dual_ui8LinesValLoBackup = exposureRegs[1];
    }
    else
    {
        // don't need to write the same exposure val again
        return IMG_SUCCESS;
    }

    printf("Exposure Val %x\n",ui32Lines);

#ifndef DISABLE_INFOTM_AE
    Temp[0] =0x02;
    Temp[1] =exposureRegs[2];
    bridge_i2c_write(i2c,Temp[0], Temp[1]);

    Temp[0] =0x01;
    Temp[1] =exposureRegs[1];
    bridge_i2c_write(i2c,Temp[0], Temp[1]);
#endif

    //	xc9080_k02Dual_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));

    return IMG_SUCCESS;
}

static IMG_UINT8 gainindex[]={
    1,2,4,8,16,32
};

IMG_RESULT k02mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info)
{
    IMG_UINT8 aui8Regs[2];
    unsigned int Integer = 0;
    unsigned int fraction =0;
    unsigned int index=0;
    IMG_RESULT ret;
    static int runing_flag = 0;

    //printf("-->XC9080_K02_SetGain %f\n", flGain);

    if (runing_flag == 1)
    {
    return IMG_SUCCESS;
    }

    g_gain = gain;
    if(g_gain < aec_min_gain) g_gain=aec_min_gain;
    if(g_gain > aec_max_gain) g_gain = aec_max_gain;

    Integer = (unsigned int)(g_gain);
    printf("XC9080_K02_SetGain =%f \r\n",g_gain);

    for(index=0; index < sizeof(gainindex)-1;index++)
    {
        if(Integer < gainindex[index+1]) break;
    }

    fraction=(unsigned int)((g_gain - gainindex[index])*16/gainindex[index]);

    printf("XC9080_K02_SetGain Integer=%d index=%d fraction=%d \r\n",Integer,index,fraction);

    aui8Regs[0] = 0x00;// Global Gain register
    aui8Regs[1] = index<<4 |fraction;

    if (g_K02Dual_ui8GainValBackup == aui8Regs[1])
    {
        runing_flag = 0;
        return IMG_SUCCESS;
    }

    g_K02Dual_ui8GainValBackup = aui8Regs[1];
#ifndef DISABLE_INFOTM_AE
    bridge_i2c_write(i2c,aui8Regs[0], aui8Regs[1]);
 #endif

#if 1 //defined(ENABLE_AE_DEBUG_MSG)
    printf("asxc9080_k02GainValues Change Global Gain 0x00 = 0x%2x\n", aui8Regs[1]);
#endif

    runing_flag = 0;

    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set global gain register\n");
        return ret;
    }

    return IMG_SUCCESS;
}


IMG_RESULT k02mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    *exp = g_exposure_time;
    return IMG_SUCCESS;
}
IMG_RESULT k02mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    *gain = g_gain;
    return IMG_SUCCESS;
}

IMG_RESULT k02mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    *min = aec_min_gain;
    *max = aec_max_gain;
    return IMG_SUCCESS;

}
IMG_RESULT k02mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    return IMG_SUCCESS;
}

IMG_RESULT k02mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    k02mipi_set_exposure(i2c,exp,info);
    k02mipi_set_gain(i2c,gain,info);
    return IMG_SUCCESS;
}

IMG_RESULT k02mipi_check_id(BRIDGE_I2C_CLIENT* i2c)
{
    LOG_DEBUG("%s\n", __func__);

    IMG_RESULT ret;
    IMG_UINT16 model_id_h = 0;
    IMG_UINT16 model_id_l = 0;

    ret = bridge_i2c_read(i2c, K02_MODEL_ID_H_REG, &model_id_h);
    ret = bridge_i2c_read(i2c, K02_MODEL_ID_L_REG, &model_id_l);

    if (ret || K02_MODEL_ID_H != model_id_h || K02_MODEL_ID_L != model_id_l)
    {
        LOG_ERROR("[K02] Failed to ensure that the i2c device has a compatible "
            "sensor! ret=%d model =0x%x (expect model 0x%x)\r\n",
            ret, (model_id_h << 8 | model_id_l),
            (K02_MODEL_ID_H << 8 |K02_MODEL_ID_L));
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        LOG_INFO("Check Model = 0x%x\n",
            ( model_id_h << 8 | model_id_l));
    }
    return IMG_SUCCESS;
}

IMG_RESULT k02mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    LOG_ERROR("%s\n", __func__);

    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 aui16Regs[] = {
        0x12, 0x00,
    };

    if (on)
    {
        aui16Regs[1] = 0x00;
    }
    else
    {
        aui16Regs[1] = 0x40;
    }
    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    return ret;
}
