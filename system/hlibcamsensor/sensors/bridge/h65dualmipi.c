#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/h65dualmipi.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#define LOG_TAG "H65_DUAL_MIPI"
#include <felixcommon/userlog.h>

#define H65_MODEL_ID_H_REG (0x0A)
#define H65_MODEL_ID_L_REG (0x0B)

#define H65_MODEL_ID_H (0x0A)
#define H65_MODEL_ID_L (0x65)

IMG_RESULT h65mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
IMG_RESULT h65mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
IMG_RESULT h65mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info);
IMG_RESULT h65mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info);
IMG_RESULT h65mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info);
IMG_RESULT h65mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT h65mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);
IMG_RESULT h65mipi_check_id(BRIDGE_I2C_CLIENT* i2c);
IMG_RESULT h65mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on);

IMG_UINT8* h65mipi_read_calib_data(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info);
IMG_UINT8* h65mipi_read_calib_version(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info);
IMG_RESULT h65mipi_update_wb_gain(BRIDGE_I2C_CLIENT* i2c,
    int product, IMG_FLOAT gain, IMG_UINT16 version);
IMG_RESULT h65mipi_exchange_calib_direction(BRIDGE_I2C_CLIENT* i2c, int flag);
BRIDGE_SENSOR h65mipi_sensor = {
    .name = "h65mipi",
    .set_exposure = h65mipi_set_exposure,
    .set_gain = h65mipi_set_gain,
    .get_exposure = h65mipi_get_exposure,
    .get_gain = h65mipi_get_gain,
    .get_exposure_range = h65mipi_get_exposure_range,
    .get_gain_range = h65mipi_get_gain_range,
    .set_exposure_gain = h65mipi_set_exposure_gain,
    .check_id = h65mipi_check_id,
    .enable = h65mipi_enable,
    .read_calib_data = h65mipi_read_calib_data,
    .read_calib_version = h65mipi_read_calib_version,
    .update_wb_gain = h65mipi_update_wb_gain,
    .exchange_calib_direction = h65mipi_exchange_calib_direction,
    .next =NULL,
    .prev = NULL,
};

typedef struct h65_reg_value {
	IMG_UINT8 reg;
	IMG_UINT8 value;
}H65_REG_VALUE;

static IMG_UINT8 ui8ModeRegs_1920_960_30_2lane[] =
{
    0x12,0x40, ////sensor 1
    0x0E,0x11,
    0x0F,0x04,
    0x10,0x24,
    0x11,0x80,
    0x5F,0x01,
    0x60,0x10,
    0x19,0x44,////0x44
    0x48,0x25,
    0x20,0xD0,
    0x21,0x02,
    0x22,0xb0,////0xb0
    0x23,0x04,////04
    0x24,0x80,
    0x25,0xC0,
    0x26,0x32,
    0x27,0xa0,//5c
    0x28,0x1C,
    0x29,0x01,
    0x2A,0x48,
    0x2B,0x25,
    0x2C,0x00,
    0x2D,0x00,
    0x2E,0xF8,
    0x2F,0x40,
    0x41,0x90,
    0x42,0x12,
    0x39,0x90,
    0x1D,0x00,
    0x1E,0x00,////0x00
    0x6C,0x50,////0x50
    0x70,0x89,
    0x71,0x8A,
    0x72,0x68,
    0x73,0x33,
    0x74,0x52,
    0x75,0x2B,
    0x76,0x40,
    0x77,0x06,
    0x78,0x0E,
    0x6E,0x2C,
    0x1F,0x10,
    0x31,0x0C,
    0x32,0x20,
    0x33,0x0C,
    0x34,0x4F,
    0x36,0x06,
    0x38,0x39,
    0x3A,0x08,
    0x3B,0x50,
    0x3C,0xA0,
    0x3D,0x00,
    0x3E,0x00,
    0x3F,0x00,
    0x40,0x00,
    0x0D,0x50,
    0x5A,0x43,
    0x5B,0xB3,
    0x5C,0x0C,
    0x5D,0x7E,
    0x5E,0x24,
    0x62,0x40,
    0x67,0x48,
    0x6A,0x1F,
    0x68,0x00,
    0x8F,0x9F,
    0x0C,0x00,
    0x59,0x97,
    0x4A,0x05,
    0x50,0x03,
    0x47,0x62,
    0x7E,0xCD,
    0x8D,0x87,
    0x49,0x10,
    0x7F,0x16,
    0x8E,0x00,
    0x8C,0xFF,
    0x8B,0x01,
    0x57,0x02,
    0x62,0x44,
    0x63,0x80,
    0x7B,0x46,
    0x7C,0x2D,
    0x90,0x00,
    0x79,0x00,
    0x13,0x81,
    0x12,0x00,
    0x45,0x89,
    0x93,0x68,
    0x45,0x19,
    0x1F,0x11,
    //0xFF,0xFF, //stop
};

static IMG_UINT8 ui8ModeRegs_master_1920_960_30_2lane[] =
{
    0x12,0x40,
    0x0E,0x11,
    0x0F,0x04,
    0x10,0x24,
    0x11,0x80,
    0x5F,0x01,
    0x60,0x10,
    0x19,0x44,
    0x48,0x25,
    0x20,0xD0,
    0x21,0x02,
    0x22,0xb0,////0xb0
    0x23,0x04,////0x04
    0x24,0x80,
    0x25,0xC0,
    0x26,0x32,
    0x27,0xa0,//5c
    0x28,0x1C,
    0x29,0x01,
    0x2A,0x48,
    0x2B,0x25,
    0x2C,0x00,
    0x2D,0x00,
    0x2E,0xF8,
    0x2F,0x40,
    0x41,0x90,
    0x42,0x12,
    0x39,0x90,
    0x1D,0x00,
    0x1E,0x04,////0x04
    0x6C,0x50,////0x50
    0x70,0x89,
    0x71,0x8A,
    0x72,0x68,
    0x73,0x33,
    0x74,0x52,
    0x75,0x2B,
    0x76,0x40,
    0x77,0x06,
    0x78,0x0E,
    0x6E,0x2C,
    0x1F,0x10,
    0x31,0x0C,
    0x32,0x20,
    0x33,0x0C,
    0x34,0x4F,
    0x36,0x06,
    0x38,0x39,
    0x3A,0x08,
    0x3B,0x50,
    0x3C,0xA0,
    0x3D,0x00,
    0x3E,0x00,
    0x3F,0x00,
    0x40,0x00,
    0x0D,0x50,
    0x5A,0x43,
    0x5B,0xB3,
    0x5C,0x0C,
    0x5D,0x7E,
    0x5E,0x24,
    0x62,0x40,
    0x67,0x48,
    0x6A,0x1F,
    0x68,0x00,
    0x8F,0x9F,
    0x0C,0x00,
    0x59,0x97,
    0x4A,0x05,
    0x50,0x03,
    0x47,0x62,
    0x7E,0xCD,
    0x8D,0x87,
    0x49,0x10,
    0x7F,0x16,
    0x8E,0x00,
    0x8C,0xFF,
    0x8B,0x01,
    0x57,0x02,
    0x62,0x44,
    0x63,0x80,
    0x7B,0x46,
    0x7C,0x2D,
    0x90,0x00,
    0x79,0x00,
    0x13,0x81,
    0x12,0x00,
    0x45,0x89,
    0x93,0x68,
    0x45,0x19,
    0x1F,0x11,
};

void h65mipi_init(void)
{
    LOG_DEBUG("%s\n", __func__);
    bridge_sensor_register(&h65mipi_sensor);
}

static IMG_UINT8 g_H65Dual_ui8LinesValHiBackup = 0xff;
static IMG_UINT8 g_H65Dual_ui8LinesValLoBackup = 0xff;
static IMG_UINT8 g_H65Dual_ui8GainValBackup = 0xff;
static const double aec_min_gain = 1.0;
static const double aec_max_gain = 16.0;//16.0;
static int runing_flag = 0;

static IMG_RESULT h65mipi_i2c_read(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
	    IMG_UINT16* data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[1];

    IMG_ASSERT(data);  // null pointer forbidden

    if (ioctl(client->i2c, I2C_SLAVE_FORCE, client->i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave read address!\n");
        return IMG_ERROR_BUSY;
    }

    buff[0] = offset & 0xFF;
    LOG_DEBUG("Reading 0x%04x\n", offset);

    // write to set the address
    ret = write(client->i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_WARNING("Wrote %dB instead of %luB before reading\n",
            ret, sizeof(buff));
        return IMG_ERROR_FATAL;
    }

    // read to get the data
    ret = read(client->i2c, buff, 1);
    if (1 != ret)
    {
        LOG_ERROR("Failed to read I2C at 0x%x\n", offset);
        return IMG_ERROR_FATAL;
    }

    *data = buff[0];

    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif  /* file_i2c */
}

static IMG_RESULT h65mipi_i2c_write(BRIDGE_I2C_CLIENT* client, IMG_UINT16 offset,
    IMG_UINT16 data)
{
#if !file_i2c
    IMG_UINT8 buff[2];
    int ret;

    /* Set I2C slave address */
    if (ioctl(client->i2c, I2C_SLAVE_FORCE, client->i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave write address!\n");
        return IMG_ERROR_BUSY;
    }

    buff[0] = offset & 0xFF;
    buff[1] = data & 0xFF;

    LOG_DEBUG("[%d] Writing 0x%04x, 0x%04X\n", client->i2c, offset, data);
    ret = write(client->i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_WARNING("Failed to write I2C at 0x%x\n", offset);
        return IMG_ERROR_FATAL;
    }
#endif
    return IMG_SUCCESS;
}

IMG_RESULT h65mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    return IMG_SUCCESS;
}

typedef struct _argain_ {
    double flGain;
    unsigned char ui8GainRegVal;
}ARGAINTABLE;

IMG_RESULT h65mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    return IMG_SUCCESS;
}

IMG_RESULT h65mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    *exp = info->ui32ExposureMin*0x108;
    return IMG_SUCCESS;
}
IMG_RESULT h65mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    return IMG_SUCCESS;
}

IMG_RESULT h65mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    *min = aec_min_gain;
    *max = aec_max_gain;
    return IMG_SUCCESS;

}
IMG_RESULT h65mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    return IMG_SUCCESS;
}

IMG_RESULT h65mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    return IMG_SUCCESS;
}

IMG_RESULT h65mipi_check_id(BRIDGE_I2C_CLIENT* i2c)
{
    LOG_DEBUG("%s\n", __func__);

    IMG_RESULT ret;
    IMG_UINT16 model_id_h = 0;
    IMG_UINT16 model_id_l = 0;

    ret = h65mipi_i2c_read(i2c, H65_MODEL_ID_H_REG, &model_id_h);
    ret = h65mipi_i2c_read(i2c, H65_MODEL_ID_L_REG, &model_id_l);

    if (ret || H65_MODEL_ID_H != model_id_h || H65_MODEL_ID_L != model_id_l)
    {
        LOG_ERROR("[H65] Failed to ensure that the i2c device has a compatible "
            "sensor! ret=%d model =0x%x (expect model 0x%x)\n",
            ret, (model_id_h << 8 | model_id_l),
            (H65_MODEL_ID_H << 8 |H65_MODEL_ID_L));
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
        LOG_INFO("Check Model = 0x%x\n",
            ( model_id_h << 8 | model_id_l));
#if 1	//TODO do initial work here temporarily
        {
            IMG_UINT16 count = sizeof(ui8ModeRegs_master_1920_960_30_2lane)/sizeof(H65_REG_VALUE);
            IMG_UINT16 i;

            for(i = 0; i < count; ++i)
            {
                ret = h65mipi_i2c_write(i2c, ui8ModeRegs_master_1920_960_30_2lane[2 * i],
                ui8ModeRegs_master_1920_960_30_2lane[2 * i + 1]);
            }
        }
#endif
    }
    return IMG_SUCCESS;
}

IMG_RESULT h65mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    LOG_ERROR("%s\n", __func__);
    int ret = IMG_SUCCESS;
    return ret;
}

static int calib_direction = 0;
static IMG_UINT8 sensor_i2c_addr[2] = {(0x6C >> 1), (0x20 >> 1)}; // left, right

///////////////////////////////////////////////////////////////////////////////
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

static IMG_RESULT h65mipi_update_sensor_wb_gain_e2prom(
    BRIDGE_I2C_CLIENT* i2c, IMG_FLOAT awb_convert_gain, IMG_UINT16 version);

#define E2PROM_CALIBR_REG_CNT (RING_CNT*RING_SECTOR_CNT*3 + 9)

static IMG_UINT8 sensor_e2prom_calibration_data[2][E2PROM_CALIBR_REG_CNT] = {0};
static IMG_UINT8 sensor_e2prom_version_info[2][4] = {0};
static IMG_RESULT h65mipi_read_e2prom_data(int i2c, char i2c_addr,
    IMG_UINT16 start_addr, int count, IMG_UINT8 *data)
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
    }
    printf("\n");
#endif
}
	return IMG_SUCCESS;
}


static IMG_UINT8* h65mipi_read_sensor_e2prom_calibration_data(
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
    printf("===>h65mipi_read_sensor_e2prom_calibration_data %d\n", sensor_index);
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

    if (h65mipi_read_e2prom_data(i2c, sensor_index, 0, reg_cnt,
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
    //h65mipi_update_sensor_wb_gain_e2prom(hHandle, awb_convert_gain, *otp_calibration_version);
    }

    return sensor_e2prom_calibration_data[sensor_index];
}

static IMG_UINT8* h65mipi_read_sensor_e2prom_calibration_version(
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

static IMG_RESULT h65mipi_update_sensor_wb_gain_e2prom(
    BRIDGE_I2C_CLIENT* i2c, IMG_FLOAT awb_convert_gain, IMG_UINT16 version)
{
    return IMG_SUCCESS;
}
#endif
///////////////////////////////////////////////////////////////////////////////////

IMG_UINT8* h65mipi_read_calib_data(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info)
{
    if (product == 2)// e2prom
    {
        return h65mipi_read_sensor_e2prom_calibration_data(i2c, idx, gain, version);
    }
    return NULL;
}

IMG_UINT8* h65mipi_read_calib_version(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info)
{
    if (product == 2)// e2prom
    {
        return h65mipi_read_sensor_e2prom_calibration_version(i2c, idx, version);
    }
    return NULL;
}

IMG_RESULT h65mipi_update_wb_gain(BRIDGE_I2C_CLIENT* i2c,
    int product, IMG_FLOAT gain,  IMG_UINT16 version)
{
    return IMG_SUCCESS;
}

IMG_RESULT h65mipi_exchange_calib_direction(BRIDGE_I2C_CLIENT* i2c, int flag)
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