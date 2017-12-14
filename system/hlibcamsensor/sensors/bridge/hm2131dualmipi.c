#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/hm2131dualmipi.h"

#define LOG_TAG "HM2131_DUAL_MIPI"
#include <felixcommon/userlog.h>

#define HM2131_MODEL_ID_H_REG (0x0000)
#define HM2131_MODEL_ID_L_REG (0x0001)

#define HM2131_MODEL_ID_H (0x21)
#define HM2131_MODEL_ID_L (0x31)

#define HM2131_MODE_SELECT_REG (0x0100)

#define HM2131_MODE_STANDBY (0x00)
#define HM2131_MODE_POWER_UP (0x2)
#define HM2131_MODE_STREAMING (0x3)

#define HM2131_SW_RESET_REG (0x0103)

IMG_RESULT hm2131mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_check_id(BRIDGE_I2C_CLIENT* i2c);
IMG_RESULT hm2131mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on);

IMG_UINT8* hm2131mipi_read_calib_data(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info);
IMG_UINT8* hm2131mipi_read_calib_version(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info);
IMG_RESULT hm2131mipi_update_wb_gain(BRIDGE_I2C_CLIENT* i2c,
    int product, IMG_FLOAT gain, IMG_UINT16 version);
IMG_RESULT hm2131mipi_exchange_calib_direction(BRIDGE_I2C_CLIENT* i2c, int flag);


BRIDGE_SENSOR hm2131mipi_sensor = {
    .name = "hm2131mipi",
    .set_exposure = hm2131mipi_set_exposure,
    .set_gain = hm2131mipi_set_gain,
    .get_exposure = hm2131mipi_get_exposure,
    .get_gain = hm2131mipi_get_gain,
    .get_exposure_range = hm2131mipi_get_exposure_range,
    .get_gain_range = hm2131mipi_get_gain_range,
    .set_exposure_gain = hm2131mipi_set_exposure_gain,
    .check_id = hm2131mipi_check_id,
    .enable = hm2131mipi_enable,
    .read_calib_data = hm2131mipi_read_calib_data,
    .read_calib_version = hm2131mipi_read_calib_version,
    .update_wb_gain = hm2131mipi_update_wb_gain,
    .exchange_calib_direction = hm2131mipi_exchange_calib_direction,

    .next =NULL,
    .prev = NULL,
};

void hm2131mipi_init(void)
{
    LOG_DEBUG("%s\n", __func__);
    bridge_sensor_register(&hm2131mipi_sensor);
}

static IMG_UINT8 g_HM2131Dual_ui8LinesValHiBackup = 0xff;
static IMG_UINT8 g_HM2131Dual_ui8LinesValLoBackup = 0xff;
static IMG_UINT8 g_HM2131Dual_ui8GainValBackup = 0xff;
static const double aec_min_gain = 1.0;
static const double aec_max_gain = 16.0;//16.0;
static int runing_flag = 0;
static uint32_t g_exposure_time = 3240;
static double g_gain = 1.0;

IMG_RESULT hm2131mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    IMG_RESULT ret;
    IMG_UINT32 ui32Lines;
    IMG_UINT16 exposureRegs[] =
    {
        0x0202, 0x01, // exposure register
        0x0203, 0x08,
        0x0104, 0x01,
        0x0104, 0x00,
    };

    ui32Lines = (IMG_UINT32)exp / info->ui32ExposureMin;
    exposureRegs[1] = (ui32Lines >> 8);
    exposureRegs[3] = (ui32Lines) & 0xff;

    if ((g_HM2131Dual_ui8LinesValHiBackup != exposureRegs[1]) ||
        (g_HM2131Dual_ui8LinesValLoBackup != exposureRegs[3]))
    {
        g_HM2131Dual_ui8LinesValHiBackup = exposureRegs[1];
        g_HM2131Dual_ui8LinesValLoBackup = exposureRegs[3];
    }
    else
    {
        // don't need to write the same exposure val again
        return IMG_SUCCESS;
    }

    LOG_DEBUG("Exposure Val %x\n",exp/info->ui32ExposureMin);

#ifndef DISABLE_INFOTM_AE
    ret = bridge_i2c_write(i2c, exposureRegs[0], exposureRegs[1]);
    ret = bridge_i2c_write(i2c, exposureRegs[2], exposureRegs[3]);
    ret = bridge_i2c_write(i2c, exposureRegs[4], exposureRegs[5]);
    ret = bridge_i2c_write(i2c, exposureRegs[6], exposureRegs[7]);
#endif
    g_exposure_time = exp;

    return ret;
}

typedef struct _argain_ {
    double flGain;
    unsigned char ui8GainRegVal;
}ARGAINTABLE;

static ARGAINTABLE asHM2131GainValues[]=
{
    {1.0,    0x00},
    {1.0625, 0x01},
    {1.125,  0x02},
    {1.1875, 0x03},
    {1.25,   0x04},
    {1.3125, 0x05},
    {1.375,  0x06},
    {1.4375, 0x07},
    {1.5,    0x08},
    {1.5625, 0x09},
    {1.625,  0x0A},
    {1.6875, 0x0B},
    {1.75,   0x0C},
    {1.8125, 0x0D},
    {1.875,  0x0E},
    {1.9375, 0x0F},
    {2.0,   0x10},
    {2.125, 0x12},
    {2.25,  0x14},
    {2.375, 0x16},
    {2.5,   0x18},
    {2.625, 0x1A},
    {2.75,  0x1C},
    {2.875, 0x1E},
    {3.0,   0x20},
    {3.125, 0x22},
    {3.25,  0x24},
    {3.375, 0x26},
    {3.5,   0x28},
    {3.625, 0x2A},
    {3.75,  0x2C},
    {3.875, 0x2E},
    {4.0,   0x30},
    {4.25,  0x34},
    {4.5,   0x38},
    {4.75,  0x3C},
    {5.0,   0x40},
    {5.25,  0x44},
    {5.5,   0x48},
    {5.75,  0x4C},
    {6.0,   0x50},
    {6.25,  0x54},
    {6.5,   0x58},
    {6.75,  0x5C},
    {7.0,   0x60},
    {7.25,  0x64},
    {7.5,   0x68},
    {7.75,  0x6C},
    {8.0,   0x70},
    {8.5,   0x78},
    {9.0,   0x80},
    {9.5,   0x88},
    {10.0,  0x90},
    {10.5,  0x98},
    {11.0,  0xA0},
    {11.5,  0xA8},
    {12.0,  0xB0},
    {12.5,  0xB8},
    {13.0,  0xC0},
    {13.5,  0xC8},
    {14.0,  0xD0},
    {14.5,  0xD8},
    {15.0,  0xE0},
    {15.5,  0xE8},
    {16.0,  0xF0},
};

IMG_RESULT hm2131mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    IMG_UINT16 aui16Regs[6];
    unsigned int nIndex = 0;
    IMG_RESULT ret;

    /*
    * First search the gain table for the lowest analog gain we can use,
    * then use the digital gain to get more accuracy/range.
    */
    while(nIndex < (sizeof(asHM2131GainValues) / sizeof(asHM2131GainValues[0])) - 1)
    {
        if(asHM2131GainValues[nIndex].flGain > gain)
            break;
        nIndex++;
    }

    if(nIndex > 0)
        nIndex = nIndex - 1;

    if (g_HM2131Dual_ui8GainValBackup == asHM2131GainValues[nIndex].ui8GainRegVal)
    {
        runing_flag = 0;
        return IMG_SUCCESS;
    }

    g_HM2131Dual_ui8GainValBackup = asHM2131GainValues[nIndex].ui8GainRegVal;

    aui16Regs[0] = 0x0205;// Global Gain register;
    aui16Regs[1] = (IMG_UINT16)asHM2131GainValues[nIndex].ui8GainRegVal;

    aui16Regs[2] = 0x0104;
    aui16Regs[3] = 0x01;

    aui16Regs[4] = 0x0104;
    aui16Regs[5] = 0x00;

#ifndef DISABLE_INFOTM_AE
    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    ret = bridge_i2c_write(i2c, aui16Regs[2], aui16Regs[3]);
    ret = bridge_i2c_write(i2c, aui16Regs[4], aui16Regs[5]);
#endif

#if defined(ENABLE_AE_DEBUG_MSG)
    printf("asHM2131GainValues Change Global Gain 0x0205 = 0x%04X\n", aui16Regs[1]);
#endif

    g_gain = gain;
    runing_flag = 0;

    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set global gain register\n");
        return ret;
    }

    return IMG_SUCCESS;
}

IMG_RESULT hm2131mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    *exp = g_exposure_time;
    return IMG_SUCCESS;
}
IMG_RESULT hm2131mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    *gain = g_gain;
    return IMG_SUCCESS;
}

IMG_RESULT hm2131mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    *min = aec_min_gain;
    *max = aec_max_gain;
    return IMG_SUCCESS;

}
IMG_RESULT hm2131mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    return IMG_SUCCESS;
}

IMG_RESULT hm2131mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info)
{
#if 0
    IMG_RESULT ret;
    ret = hm2131mipi_set_exposure(i2c, exp, info);
    if (ret != IMG_SUCCESS)
        return ret;

    return hm2131mipi_set_gain(i2c, gain, info);
#else
    IMG_RESULT ret = IMG_SUCCESS;

    static IMG_UINT32 delay_loops =0;
    IMG_UINT32 ui32Lines;
    IMG_UINT32 nIndex = 0;

    IMG_UINT16 ui16Regs[] =
    {
        0x0205, 0x00, // gain register
        0x0202, 0x00, // Exposure register
        0x0203, 0x00,
        0x0104, 0x01,
        0x0104, 0x00,
    };

    delay_loops++;
    if (delay_loops % 3 != 0)
        return 0;

    LOG_DEBUG("exp=%d gain=%f\n", exp, gain);
    delay_loops = 0;

    if ((g_exposure_time == exp) && (g_gain == gain))
        return 0;

    ui32Lines = (IMG_UINT32)exp / info->ui32ExposureMin;
    ui16Regs[3] = (ui32Lines >> 8);
    ui16Regs[5] = (ui32Lines) & 0xff;

    while(nIndex < (sizeof(asHM2131GainValues) / sizeof(ARGAINTABLE)) - 1)
    {
        if(asHM2131GainValues[nIndex].flGain > gain)
            break;
        nIndex++;
    }

    if(nIndex > 0)
        nIndex = nIndex - 1;

    ui16Regs[1] = asHM2131GainValues[nIndex].ui8GainRegVal;

    if ((g_HM2131Dual_ui8LinesValHiBackup != ui16Regs[3]) ||
        (g_HM2131Dual_ui8LinesValLoBackup != ui16Regs[5]))
    {
        g_HM2131Dual_ui8LinesValHiBackup = ui16Regs[3];
        g_HM2131Dual_ui8LinesValLoBackup = ui16Regs[5];
    }

    if ((g_HM2131Dual_ui8GainValBackup != ui16Regs[1]))
        g_HM2131Dual_ui8GainValBackup = ui16Regs[1];

#ifndef DISABLE_INFOTM_AE
    ret = bridge_i2c_write(i2c, ui16Regs[0], ui16Regs[1]);
    ret = bridge_i2c_write(i2c, ui16Regs[2], ui16Regs[3]);
    ret = bridge_i2c_write(i2c, ui16Regs[4], ui16Regs[5]);
    ret = bridge_i2c_write(i2c, ui16Regs[6], ui16Regs[7]);
    ret = bridge_i2c_write(i2c, ui16Regs[8], ui16Regs[9]);
#endif

    g_exposure_time = exp;
    g_gain = gain;

    return ret;
#endif
}
IMG_RESULT hm2131mipi_check_id(BRIDGE_I2C_CLIENT* i2c)
{
    LOG_DEBUG("%s\n", __func__);

    IMG_RESULT ret;
    IMG_UINT16 model_id_h = 0;
    IMG_UINT16 model_id_l = 0;

    ret = bridge_i2c_read(i2c, HM2131_MODEL_ID_H_REG, &model_id_h);
    ret = bridge_i2c_read(i2c, HM2131_MODEL_ID_L_REG, &model_id_l);

    if (ret || HM2131_MODEL_ID_H!= model_id_h || HM2131_MODEL_ID_L!= model_id_l)
    {
        LOG_ERROR("[HM2131] Failed to ensure that the i2c device has a compatible "
            "sensor! ret=%d model =0x%x (expect model 0x%x)\n",
            ret, (model_id_h<< 8 | model_id_l),
            (HM2131_MODEL_ID_H << 8 |HM2131_MODEL_ID_L));
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        LOG_INFO("Check Model = 0x%x\n",
            ( model_id_h<< 8 | model_id_l));
    }
    return IMG_SUCCESS;
}

IMG_RESULT hm2131mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    LOG_ERROR("%s\n", __func__);

    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 aui16Regs[] = {
        HM2131_MODE_SELECT_REG, 0x00,
    };

    if (on)
    {
        aui16Regs[1] = HM2131_MODE_STREAMING;
    }
    else
    {
        aui16Regs[1] = HM2131_MODE_STANDBY;
    }
    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    return ret;
}



static int calib_direction = 0;
static IMG_UINT8 sensor_i2c_addr[2] = {(0x6C >> 1), (0x20 >> 1)}; // left, right

///////////////////////////////////////////////////////////////////////////////////
#ifdef INFOTM_E2PROM_METHOD
#ifdef OTP_DBG_LOG
#define E2PROM_DBG_LOG
#endif
//#define E2PROM_FILE "/mnt/config/opt.data"

#define E2PROM_FILE_L "/mnt/config/circleL.data"
#define E2PROM_FILE_R "/mnt/config/circleR.data"

#define RING_CNT 19
#define RING_SECTOR_CNT 16

#pragma pack(push)
#pragma pack(1)

typedef struct _E2PROM_HEADER
{
	unsigned char fid[2];
	unsigned char version[4];
	unsigned char burn_time[4];
}E2PROM_HEADER;

typedef struct _E2PROM_DATA
{
	unsigned short center_offset[2];
	unsigned short ring_cnt;
	unsigned char center_rgb[3];
	unsigned char *ring_rgb;
}E2PROM_DATA;

#pragma pack(pop)

static IMG_RESULT hm2131mipi_update_sensor_wb_gain_e2prom(
    BRIDGE_I2C_CLIENT* i2c, IMG_FLOAT awb_convert_gain, IMG_UINT16 version);

#define E2PROM_CALIBR_REG_CNT (RING_CNT*RING_SECTOR_CNT*3 + 9)

static IMG_UINT8 sensor_e2prom_calibration_data[2][E2PROM_CALIBR_REG_CNT] = {0};
static IMG_UINT8 sensor_e2prom_version_info[2][4] = {0};
static IMG_RESULT hm2131mipi_read_e2prom_data(
    int i2c, char i2c_addr, IMG_UINT16 start_addr, int count, IMG_UINT8 *data)
{
    int read_bytes = 0;
    int offset = 0;

    FILE *f = NULL;
    if (i2c_addr == 0)
    {
        if (calib_direction == 0)
        {
            f = fopen(E2PROM_FILE_L, "rb");
        }
        else
        {
            f = fopen(E2PROM_FILE_R, "rb");
        }
    }
    else
    {
        if (calib_direction == 1)
        {
            f = fopen(E2PROM_FILE_L, "rb");
        }
        else
        {
            f = fopen(E2PROM_FILE_R, "rb");
        }
    }

    if (f == NULL)
    {
        printf("==>can not open e2prom file!\n");
        return IMG_ERROR_FATAL;
    }

    fseek(f, offset, SEEK_CUR);

    //data[0] = 0;data[1] = 0;data[2] = 0;data[3] = 0;data[4] = 0;data[5] = 19;
    read_bytes = fread(data, 1, E2PROM_CALIBR_REG_CNT, f);
    fclose(f);

    printf("===>e2prom offset %d and read %d bytes\n", offset, read_bytes);
{
    int i = 0, j = 0, k = 0, start = 9;// + RING_CNT*RING_SECTOR_CNT*3 - 16*3;

    // for debug disable offset
    data[0] = 0;data[1] = 0;data[2] = 0;data[3] = 0;
    printf("offsetx(%d, %d), offsety(%d, %d), ring_cnt(%d, %d)\n",
        data[0], data[1], data[2], data[3], data[4], data[5]);

    printf("ROI(%d, %d, %d)\n", data[6], data[7], data[8]);

    int fixed_r = 0, fixed_g = 0, fixed_b = 0;
    for (i = 0; i < RING_SECTOR_CNT; i++)
    {
        fixed_r += data[start + i*3];
        fixed_g += data[start + i*3 + 1];
        fixed_b += data[start + i*3 + 2];
    }

    data[6] = fixed_r/RING_SECTOR_CNT;
    data[7] = fixed_g/RING_SECTOR_CNT;
    data[8] = fixed_b/RING_SECTOR_CNT;

    printf("fixed ROI(%d, %d, %d)\n", data[6], data[7], data[8]);

#ifdef E2PROM_DBG_LOG
    for (k = 0; k < RING_SECTOR_CNT; k++)
    {
        for (i = 0; i < RING_CNT; i++)
        {
            j = start + RING_SECTOR_CNT*3*i + k*3;
            printf("%d, %d, %d\n", data[j], data[j + 1], data[j+2]);
        }
        //printf("=================================\n");
    }
    printf("\n");
#endif
}
	return IMG_SUCCESS;
}


static IMG_UINT8* hm2131mipi_read_sensor_e2prom_calibration_data(
    BRIDGE_I2C_CLIENT* i2c, int sensor_index/*0 or 1*/,
    IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version)
{
    int group_index = 0;
    IMG_UINT16 start_addr = 0x7115;
    int reg_cnt = 0;

    static IMG_BOOL sensor_inited_flag_L = IMG_FALSE, sensor_inited_flag_R = IMG_FALSE;

    if (otp_calibration_version != NULL)
    {
        *otp_calibration_version = 0x01;
    }

    reg_cnt = E2PROM_CALIBR_REG_CNT;

#ifdef OTP_DBG_LOG
    printf("===>hm2131mipi_read_sensor_e2prom_calibration_data %d\n", sensor_index);
#endif

#ifdef INFOTM_SKIP_OTP_DATA
    return NULL;
#endif

    if ((calib_direction == 1 && ((sensor_index == 1 && sensor_inited_flag_L) ||
            (sensor_index == 0 && sensor_inited_flag_R))) ||
            (calib_direction == 0 && ((sensor_index == 0 && sensor_inited_flag_L) ||
            (sensor_index == 1 && sensor_inited_flag_R))))
    {
        return sensor_e2prom_calibration_data[sensor_index];
    }

    if (hm2131mipi_read_e2prom_data(i2c, sensor_index, 0, reg_cnt,
        sensor_e2prom_calibration_data[sensor_index]) != IMG_SUCCESS)
    {
        return NULL;
    }

    if (sensor_index == 0)
    {
        if (calib_direction == 1)
        {
            sensor_inited_flag_R = IMG_TRUE;
        }
        else
        {
            sensor_inited_flag_L = IMG_TRUE;
        }
    }
    else if (sensor_index == 1)
    {
        if (calib_direction == 1)
        {
            sensor_inited_flag_L = IMG_TRUE;
        }
        else
        {
            sensor_inited_flag_R = IMG_TRUE;
        }
    }

    if (sensor_inited_flag_L && sensor_inited_flag_R)
    {
    //hm2131mipi_update_sensor_wb_gain_e2prom(hHandle, awb_convert_gain, *otp_calibration_version);
    }

    return sensor_e2prom_calibration_data[sensor_index];
}

static IMG_UINT8* hm2131mipi_read_sensor_e2prom_calibration_version(
    BRIDGE_I2C_CLIENT* i2c, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version)
{
    int group_index = 0;
    if (otp_calibration_version != NULL)
    {
        *otp_calibration_version = 0x1;
    }
    return sensor_e2prom_version_info[sensor_index];
}

#define WB_CALIBRATION_METHOD_FROM_LIPIN

static IMG_RESULT hm2131mipi_update_sensor_wb_gain_e2prom(
    BRIDGE_I2C_CLIENT* i2c, IMG_FLOAT awb_convert_gain, IMG_UINT16 version)
{
    return IMG_SUCCESS;
}
#endif
///////////////////////////////////////////////////////////////////////////////////


IMG_UINT8* hm2131mipi_read_calib_data(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info)
{
    if (product == 2)// e2prom
    {
        return hm2131mipi_read_sensor_e2prom_calibration_data(i2c, idx, gain, version);
    }
    return NULL;
}

IMG_UINT8* hm2131mipi_read_calib_version(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info)
{
    if (product == 2)// e2prom
    {
        return hm2131mipi_read_sensor_e2prom_calibration_version(i2c, idx, version);
    }
    return NULL;
}

IMG_RESULT hm2131mipi_update_wb_gain(BRIDGE_I2C_CLIENT* i2c,
    int product, IMG_FLOAT gain,  IMG_UINT16 version)
{
    return IMG_SUCCESS;
}

IMG_RESULT hm2131mipi_exchange_calib_direction(BRIDGE_I2C_CLIENT* i2c, int flag)
{
    static IMG_UINT8 bk_sensor_i2c_addr[2] = {0, 0};

    if (bk_sensor_i2c_addr[0] == 0 || bk_sensor_i2c_addr[1] == 0)
    {
        bk_sensor_i2c_addr[0] = sensor_i2c_addr[0];
        bk_sensor_i2c_addr[1] = sensor_i2c_addr[1];
    }

    if (calib_direction != flag)
    {
        IMG_UINT8 tmp = sensor_i2c_addr[0];
        sensor_i2c_addr[0] = sensor_i2c_addr[1];
        sensor_i2c_addr[1] = sensor_i2c_addr[0];
        calib_direction = flag;
    }
    return IMG_SUCCESS;
}
