#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/nt99142dualdvp.h"

#define LOG_TAG "NT99142_DUAL_DVP"
#include <felixcommon/userlog.h>

#define NT99142_CHIP_ID_REG (0x3000)
#define NT99142_CHIP_ID     (0x14)

IMG_RESULT nt99142dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
IMG_RESULT  nt99142dvp_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
IMG_RESULT nt99142dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info);
IMG_RESULT nt99142dvp_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info);
IMG_RESULT nt99142dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info);
IMG_RESULT nt99142dvp_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min,IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT nt99142dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);
IMG_RESULT nt99142dvp_check_id(BRIDGE_I2C_CLIENT* i2c);
IMG_RESULT nt99142dvp_enable(BRIDGE_I2C_CLIENT* i2c, int on);

BRIDGE_SENSOR nt99142dvp_sensor = {
    .name = "nt99142dvp",
    .set_exposure = nt99142dvp_set_exposure,
    .set_gain = nt99142dvp_set_gain,
    .get_exposure = nt99142dvp_get_exposure,
    .get_gain = nt99142dvp_get_gain,
    .get_exposure_range = nt99142dvp_get_exposure_range,
    .get_gain_range = nt99142dvp_get_gain_range,
    .set_exposure_gain = nt99142dvp_set_exposure_gain,
    .check_id = nt99142dvp_check_id,
    .enable = nt99142dvp_enable,
    .next =NULL,
    .prev = NULL,
};

void nt99142dvp_init(void)
{
    bridge_sensor_register(&nt99142dvp_sensor);
}

IMG_RESULT nt99142dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c, IMG_UINT32 exp, SENSOR_MODE *info)
{
    IMG_UINT16 aui16Regs[] = {
        0x3012, 0x00,
        0x3013, 0x01,
        0x3060, 0x02,
    };

    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT32 ui32Lines;

    ui32Lines = exp / info->ui32ExposureMin;
    aui16Regs[1] = (IMG_UINT8)(ui32Lines >> 8);
    aui16Regs[3] = (IMG_UINT8)(ui32Lines & 0xff);

    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    ret = bridge_i2c_write(i2c, aui16Regs[2], aui16Regs[3]);
    ret = bridge_i2c_write(i2c, aui16Regs[4], aui16Regs[5]);

    LOG_DEBUG("SX2_SetExposure psCam->ui32Exposure=%d,"
        " info.ui32ExposureMin=%d, ui32Lines=%d\n",
        exp, info->ui32ExposureMin, ui32Lines);
    return ret;
}

IMG_RESULT  nt99142dvp_set_gain(BRIDGE_I2C_CLIENT* i2c, double gain, SENSOR_MODE *info)
{
    IMG_RESULT ret = 0;
    IMG_UINT16 tmp = 0, nIndex = 0, val = 0x10;
    IMG_UINT16 aui16Regs[2];

    double aflRegs[16]= {
        1.0625,
        1.1250,
        1.1875,
        1.2500,
        1.3125,
        1.3750,
        1.4375,
        1.5000,
        1.5625,
        1.6250,
        1.6875,
        1.7500,
        1.8125,
        1.8750,
        1.9375,
        2.0000,
    };

    while (gain >= 2.0)
    {
        tmp = tmp | val; 
        gain = gain / 2.0;
        val = val << 1;
    }

    while(gain > aflRegs[nIndex])
    {
        nIndex++;
    }
    tmp = tmp + nIndex;

    aui16Regs[0] = 0x32cf;
    aui16Regs[1] = tmp;

    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    return ret;
}

typedef struct _NT99142DVP_gain_ {
    double flGain;
    IMG_UINT16 ui8GainRegValHi;
    IMG_UINT16 ui8GainRegValLo;
} NT99142DVP_GAIN_TABLE;

NT99142DVP_GAIN_TABLE asNT99142DVPGainValues[]=
{
    { 1,      0x00, 0x00 },
    { 1.0625, 0x00, 0x01 },
    { 1.125,  0x00, 0x02 },
    { 1.1875, 0x00, 0x03 },
    { 1.25,   0x00, 0x04 },
    { 1.3125, 0x00, 0x05 },
    { 1.375,  0x00, 0x06 },
    { 1.4375, 0x00, 0x07 },
    { 1.5, 0x00,    0x08 },
    { 1.5625, 0x00, 0x09 },
    { 1.625,  0x00, 0x0A },
    { 1.6875, 0x00, 0x0B },
    { 1.75,   0x00, 0x0C },
    { 1.8125, 0x00, 0x0D },
    { 1.875,  0x00, 0x0E },
    { 1.9375, 0x00, 0x0F },
    { 2,    0x00,0x10 },//2x
    { 2.125,  0x00, 0x11 },
    { 2.25,   0x00, 0x12 },
    { 2.375,  0x00, 0x13 },
    { 2.5,    0x00,    0x14 },
    { 2.625,  0x00, 0x15 },
    { 2.75,   0x00, 0x16 },
    { 2.875,  0x00, 0x17 },
    { 3,      0x00, 0x18 },
    { 3.125,  0x00, 0x19 },
    { 3.25,   0x00, 0x1A },
    { 3.375,  0x00, 0x1B },
    { 3.5,    0x00,    0x1C },
    { 3.625,  0x00, 0x1D },
    { 3.75,   0x00, 0x1E },
    { 3.875,  0x00, 0x1F },
    { 4,      0x00, 0x30 }, //4x
    { 4.25,   0x00, 0x31 },
    { 4.5,    0x00,    0x32 },
    { 4.75,   0x00, 0x33 },
    { 5,      0x00, 0x34 },
    { 5.25,   0x00, 0x35 },
    { 5.5,0x00, 0x36 },
    { 5.75,   0x00, 0x37 },
    { 6,      0x00, 0x38 },
    { 6.25,   0x00, 0x39 },
    { 6.5,    0x00,    0x3A },
    { 6.75,   0x00, 0x3B },
    { 7,      0x00, 0x3C },
    { 7.25,   0x00, 0x3D },
    { 7.5, 0x00,    0x3E },
    { 7.75,   0x00, 0x3F },
    { 8,      0x00, 0x70 }, //8x
    { 8.5,    0x00,    0x71 },
    { 9,      0x00, 0x72 },
    { 9.5,    0x00,    0x73 },
    { 10,      0x00, 0x74 },
    { 10.5,   0x00, 0x75 },
    { 11,      0x00, 0x76 },
    { 11.5,   0x00, 0x77 },
    { 12,      0x00, 0x78 },
    { 12.5,   0x00, 0x79 },
    { 13,      0x00, 0x7A },
    { 13.5,   0x00, 0x7B },
    { 14,      0x00, 0x7C },
    { 14.5,   0x00, 0x7D },
    { 15.5, 0x00,    0x7F },
    { 16,      0x01, 0x70 }, //16x
    { 17,      0x01, 0x71 },
    { 18,      0x01, 0x72 },
    { 19,      0x01, 0x73 },
    { 20,      0x01, 0x74 },
    { 21,      0x01, 0x75 },
    { 22,      0x01, 0x76 },
    { 23,      0x01, 0x77 },
    { 24,      0x01, 0x78 },
    { 25,      0x01, 0x79 },
    { 26,      0x01, 0x7A },
    { 27,      0x01, 0x7B },
    { 28,      0x01, 0x7C },
    { 29,      0x01, 0x7D },
    { 30,      0x01, 0x7E },
    { 31,      0x01, 0x7F },
    { 32,      0x03, 0x70 }, //32x
    { 34,      0x03, 0x71 },
    { 36,      0x03, 0x72 },
    { 38,      0x03, 0x73 },
    { 40,      0x03, 0x74 },
    { 42,      0x03, 0x75 },
    { 44,      0x03, 0x76 },
    { 46,      0x03, 0x77 },
    { 48,      0x03, 0x78 },
    { 50,      0x03, 0x79 },
    { 52,      0x03, 0x7A },
    { 54,      0x03, 0x7B },
    { 56,      0x03, 0x7C },
    { 58,      0x03, 0x7D },
    { 60,      0x03, 0x7E },
    { 62,      0x03, 0x7F }
};

static IMG_UINT16 g_NT99142DVP_ui8LinesValHiBackup = 0xff;
static IMG_UINT16 g_NT99142DVP_ui8LinesValLoBackup = 0xff;
static IMG_UINT16 g_NT99142DVP_ui8GainValHiBackup = 0xff;
static IMG_UINT16 g_NT99142DVP_ui8GainValLoBackup = 0xff;

IMG_RESULT nt99142dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c, IMG_UINT32 exp ,double gain,
        SENSOR_MODE *info)
{
    IMG_RESULT ret;
    unsigned int nIndex;
    IMG_UINT32 ui32Lines;
    IMG_UINT16 aui16Regs[] = {
        0x3012, 0x00,    //exposure 1 line
        0x3013, 0x01,
        0x301C, 0x00,    //gain 1x
        0x301D, 0x00,
        0x3060, 0x02
    };

    if (exp >= info->ui32ExposureMin)
        ui32Lines = exp/ info->ui32ExposureMin;
    else
        ui32Lines = 1;

    if (ui32Lines >= info->ui16VerticalTotal)
    {
        ui32Lines = info->ui16VerticalTotal - 2;
        //printf("ui32Lines >= VerticalTotal, set to %d\n", ui32Lines);
    }
    aui16Regs[1] = (IMG_UINT8)(ui32Lines >> 8);
    aui16Regs[3] = (IMG_UINT8)(ui32Lines & 0xff);

    LOG_DEBUG("psCam->ui32Exposure=%d, info.ui32ExposureMin=%d, ui32Lines=%d\n",
            exp, info->ui32ExposureMin, ui32Lines);

    nIndex = 0;
    /*
    * First search the gain table for the lowest analog gain we can use,
    * then use the digital gain to get more accuracy/range.
    */
    while (nIndex < (sizeof(asNT99142DVPGainValues) / sizeof(NT99142DVP_GAIN_TABLE)))
    {
        if (asNT99142DVPGainValues[nIndex].flGain > gain)
            break;
        nIndex++;
    }

    if (nIndex > 0)
        nIndex = nIndex - 1;

    aui16Regs[5] = asNT99142DVPGainValues[nIndex].ui8GainRegValHi;
    aui16Regs[7] = asNT99142DVPGainValues[nIndex].ui8GainRegValLo;

    LOG_DEBUG("SetGain flGian = %f, Gain = 0x%02X, 0x%02X\n",
        gain,
        asNT99142DVPGainValues[nIndex].ui8GainRegValHi,
        asNT99142DVPGainValues[nIndex].ui8GainRegValLo);

    //03.check the new gain & exposure is the same as the last set or not.
    //
    if ((g_NT99142DVP_ui8LinesValHiBackup == aui16Regs[1]) &&
        (g_NT99142DVP_ui8LinesValLoBackup == aui16Regs[3]) &&
        (g_NT99142DVP_ui8GainValHiBackup == aui16Regs[5]) &&
        (g_NT99142DVP_ui8GainValLoBackup == aui16Regs[7]) )
    {
        return IMG_SUCCESS;
    }

    g_NT99142DVP_ui8LinesValHiBackup = aui16Regs[1];
    g_NT99142DVP_ui8LinesValLoBackup = aui16Regs[3];
    g_NT99142DVP_ui8GainValHiBackup = aui16Regs[5];
    g_NT99142DVP_ui8GainValLoBackup = aui16Regs[7];

    //04.set to sensor
    //
    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    ret = bridge_i2c_write(i2c, aui16Regs[2], aui16Regs[3]);
    ret = bridge_i2c_write(i2c, aui16Regs[4], aui16Regs[5]);
    ret = bridge_i2c_write(i2c, aui16Regs[6], aui16Regs[7]);
    ret = bridge_i2c_write(i2c,  aui16Regs[8], aui16Regs[9]);
    return ret;
}

IMG_RESULT nt99142dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c, IMG_UINT32* exp,
    SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    *exp = info->ui32ExposureMin*720;
    return IMG_SUCCESS;
}
IMG_RESULT nt99142dvp_get_gain(BRIDGE_I2C_CLIENT* i2c, double* gain,
    SENSOR_MODE *info)
{
    return IMG_SUCCESS;
}

IMG_RESULT nt99142dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info)
{
    return IMG_SUCCESS;
}
IMG_RESULT nt99142dvp_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info)
{
    return IMG_SUCCESS;
}

IMG_RESULT nt99142dvp_check_id(BRIDGE_I2C_CLIENT* i2c)
{
    IMG_RESULT ret;
    IMG_UINT16 chipV = 0;

    // we now verify that the i2c device has an SX2 sensor
    ret = bridge_i2c_read(i2c, NT99142_CHIP_ID_REG, &chipV);
    if (ret || NT99142_CHIP_ID != chipV)
    {
        LOG_ERROR("[NT99412DVP] Failed to ensure that the i2c device has a compatible "\
            "SX2 sensor! ret=%d chip_version=0x%x (expect chip 0x%x)\n",
            ret, chipV, NT99142_CHIP_ID);
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
       LOG_INFO("Check ID = 0x%x\n", i2c, chipV);
    }
    return IMG_SUCCESS;
}

IMG_RESULT nt99142dvp_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 aui16Regs[] = {
        0x3021, 0x02,
        0x3060, 0x01,
    };

    if (on)
    {
        aui16Regs[1] = 0x02;
        aui16Regs[3] = 0x01;
    }
    else
    {
    //TODO
    }
    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    ret = bridge_i2c_write(i2c, aui16Regs[2], aui16Regs[3]);

    return ret;
}

