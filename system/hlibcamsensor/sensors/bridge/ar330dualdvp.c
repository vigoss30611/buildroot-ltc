#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/ar330dualdvp.h"

#define LOG_TAG "AR330_DUAL_DVP"
#include <felixcommon/userlog.h>

//================================AR0330DVP===================================
#define COARSE_INTEGRATION_TIME 0x3012
#define SERIAL_FORMAT 0x31AE
#define FRAME_LENGTH_LINES 0x300A
#define READ_MODE 0x3040
#define LINE_LENGTH_PCK 0x300C
#define VT_PIX_CLK_DIV 0x302A
#define VT_SYS_CLK_DIV 0x302C
#define PLL_MULTIPLIER 0x3030
#define PRE_PLL_CLK_DIV 0x302E
#define REVISION_NUMBER 0x300E
#define TEST_DATA_RED 0x3072
#define DATAPATH_STATUS 0x306A
#define FRAME_COUNT 0x303A
#define RESET_REGISTER 0x301A
#define FLASH 0x3046
#define FLASH2 0x3048
#define GLOBAL_GAIN 0x305E
#define ANALOG_GAIN 0x3060
#define X_ADDR_START 0x3004
#define X_ADDR_END 0x3008
#define Y_ADDR_START 0x3002
#define Y_ADDR_END 0x3006
#define DATA_FORMAT_BITS 0x31AC
#define CHIP_VERSION 0x3000

#define AR330DVP_CHIP_VERSION 0x2604

IMG_RESULT ar330dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
IMG_RESULT ar330dvp_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
IMG_RESULT ar330dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info);
IMG_RESULT ar330dvp_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info);
IMG_RESULT ar330dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info);
IMG_RESULT ar330dvp_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT ar330dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);
IMG_RESULT ar330dvp_check_id(BRIDGE_I2C_CLIENT* i2c);
IMG_RESULT ar330dvp_enable(BRIDGE_I2C_CLIENT* i2c, int on);

BRIDGE_SENSOR ar330dvp_sensor = {
    .name = "ar330dvp",
    .set_exposure = ar330dvp_set_exposure,
    .set_gain = ar330dvp_set_gain,
    .get_exposure = ar330dvp_get_exposure,
    .get_gain = ar330dvp_get_gain,
    .get_exposure_range = ar330dvp_get_exposure_range,
    .get_gain_range = ar330dvp_get_gain_range,
    .set_exposure_gain = ar330dvp_set_exposure_gain,
    .check_id = ar330dvp_check_id,
    .enable = ar330dvp_enable,
    .next =NULL,
    .prev = NULL,
};

void ar330dvp_init(void)
{
    bridge_sensor_register(&ar330dvp_sensor);
}

IMG_RESULT ar330dvp_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info)
{
    IMG_RESULT ret;
    IMG_UINT16 aui16Regs[2];

    aui16Regs[0] = COARSE_INTEGRATION_TIME;
    aui16Regs[1] = exp / info->ui32ExposureMin;

    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    #ifdef INFOTM_ENABLE_AE_DEBUG
    printf("AR330DVP_SetExposure: Exposure = 0x%08X, ExposureMin = 0x%08X\n", 
        exp, info->ui32ExposureMin);
    printf("AR330DVP_SetExposure: Change COARSE_INTEGRATION_TIME 0x3012 = 0x%04X\n",
        aui16Regs[1]);
    #endif //INFOTM_ENABLE_AE_DEBUG

    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set exposure\n");
    }
    return ret;
}

typedef struct _argain_ {
    double flGain;
    unsigned char ui8GainRegVal;
}AR330DVP_GAINTABLE;

static AR330DVP_GAINTABLE asAR330DVPGainValues[]=
{
    {1.00,0x00},
    {1.03,0x01},
    {1.07,0x02},
    {1.10,0x03},
    {1.14,0x04},
    {1.19,0x05},
    {1.23,0x06},
    {1.28,0x07},
    {1.33,0x08},
    {1.39,0x09},
    {1.45,0x0a},
    {1.52,0x0b},
    {1.60,0x0c},
    {1.68,0x0d},
    {1.78,0x0e},
    {1.88,0x0f},
    {2.00,0x10},
    {2.13,0x12},
    {2.29,0x14},
    {2.46,0x16},
    {2.67,0x18},
    {2.91,0x1a},
    {3.20,0x1c},
    {3.56,0x1e},
    {4.00,0x20},
    {4.57,0x24},
    {5.33,0x28},
    {6.40,0x2c},
    {8.0,0x30}
};

IMG_RESULT ar330dvp_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info)
{
    IMG_UINT16 aui16Regs[2];
    unsigned int nIndex = 0;
    IMG_RESULT ret;

    /*
    * First search the gain table for the lowest analog gain we can use,
    * then use the digital gain to get more accuracy/range.
    */
    while(nIndex < (sizeof(asAR330DVPGainValues) / sizeof(AR330DVP_GAINTABLE)) - 1)
    {
        if(asAR330DVPGainValues[nIndex].flGain > gain)
            break;
        nIndex++;
    }

    if(nIndex > 0)
    {
        nIndex = nIndex - 1;
    }

    gain  = gain / asAR330DVPGainValues[nIndex].flGain;

    aui16Regs[0] = GLOBAL_GAIN;
    aui16Regs[1] = (IMG_UINT16)(gain*128);

    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    #ifdef INFOTM_ENABLE_AE_DEBUG
    printf("AR330DVP_SetGain: Global Gain 0x%04X = 0x%04X\n", aui16Regs[0], aui16Regs[1]);
    #endif //INFOTM_ENABLE_AE_DEBUG
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set global gain register\n");
        return ret;
    }

    aui16Regs[0] = ANALOG_GAIN;
    aui16Regs[1] =
        (IMG_UINT16)((IMG_UINT16)asAR330DVPGainValues[nIndex].ui8GainRegVal
        |((IMG_UINT16)asAR330DVPGainValues[nIndex].ui8GainRegVal<<8));

    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    #ifdef INFOTM_ENABLE_AE_DEBUG
    LOG_DEBUG("AR330DVP_SetGain: Analog Gain 0x%04X = 0x%04X\n", aui16Regs[0], aui16Regs[1]);
    #endif //INFOTM_ENABLE_AE_DEBUG
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set analogue gain register\n");
    }

    return ret;
}

IMG_RESULT ar330dvp_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info)
{
    return IMG_SUCCESS;
}
IMG_RESULT ar330dvp_get_gain(BRIDGE_I2C_CLIENT* i2c, double* gain,
    SENSOR_MODE *info)
{
    return IMG_SUCCESS;
}

IMG_RESULT ar330dvp_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info)
{
    return IMG_SUCCESS;

}
IMG_RESULT ar330dvp_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min,IMG_UINT32 *max, SENSOR_MODE *info)
{
    return IMG_SUCCESS;
}

IMG_RESULT ar330dvp_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info)
{
    IMG_RESULT ret;
    ret = ar330dvp_set_exposure(i2c, exp, info);
    if (ret != IMG_SUCCESS)
        return ret;

    return ar330dvp_set_gain(i2c, gain, info);
}
IMG_RESULT ar330dvp_check_id(BRIDGE_I2C_CLIENT* i2c)
{
    IMG_RESULT ret;
    IMG_UINT16 chipV;

    ret = bridge_i2c_read(i2c, CHIP_VERSION, &chipV);
    if (ret || AR330DVP_CHIP_VERSION != chipV)
    {
        LOG_ERROR("Failed to ensure that the i2c device has a compatible "\
             "AR330DVP sensor! ret=%d chip_version=0x%x (expect chip 0x%x)\n",
             ret, chipV, AR330DVP_CHIP_VERSION);
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        LOG_INFO("CheckID = 0x%x\n", chipV);
    }
    return IMG_SUCCESS;
}

IMG_RESULT ar330dvp_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    IMG_RESULT ret;
    IMG_UINT16 aui16Regs[2];

    if (on)
    {
        aui16Regs[0] = RESET_REGISTER;
        aui16Regs[1] = 0x10DC;
    }
    else
    {
        //TODO
    }

    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to enable sensor\n");
    }
    return ret;
//TODO : will handle case on flags is off
}
