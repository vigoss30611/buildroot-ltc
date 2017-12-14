#include <math.h>
#include <string.h>
#include <unistd.h>

#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/ov4689dualmipi.h"

#define LOG_TAG "OV4689_DUAL_MIPI"
#include <felixcommon/userlog.h>

#define OV4689_SLAVE_ID_REG (0x3006)
#define OV4689_SLAVE_ID (0x42)

#define OV4689_MODE_SELECT_REG (0x0100)

#define OV4689_MODE_STANDBY (0x00)
#define OV4689_MODE_STREAMING (0x01)

#define OV4689_COMMON_ADDR (0x42 >> 1)
#define OV4689_RIGHT_ADDR (0x6C >> 1)
#define OV4689_LEFT_ADDR (0x20 >> 1)
#define OV4689_AUTOFOCUS_ADDR (0x18 >> 1)

IMG_RESULT ov4689mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_check_id(BRIDGE_I2C_CLIENT* i2c);
IMG_RESULT ov4689mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on);

IMG_RESULT ov4689mipi_get_cur_focus(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* focus, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_get_focus_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_set_focus(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 focus, SENSOR_MODE *info);

IMG_UINT8* ov4689mipi_read_calib_data(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info);
IMG_UINT8* ov4689mipi_read_calib_version(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info);
IMG_RESULT ov4689mipi_update_wb_gain(BRIDGE_I2C_CLIENT* i2c,
    int product, IMG_FLOAT gain, IMG_UINT16 version);

IMG_RESULT ov4689mipi_exchange_calib_direction(BRIDGE_I2C_CLIENT* i2c, int flag);

BRIDGE_SENSOR ov4689mipi_sensor = {
    .name = "ov4689mipi",
    .set_exposure = ov4689mipi_set_exposure,
    .set_gain = ov4689mipi_set_gain,
    .get_exposure = ov4689mipi_get_exposure,
    .get_gain = ov4689mipi_get_gain,
    .get_exposure_range = ov4689mipi_get_exposure_range,
    .get_gain_range = ov4689mipi_get_gain_range,
    .set_exposure_gain = ov4689mipi_set_exposure_gain,
    .check_id = ov4689mipi_check_id,
    .enable = ov4689mipi_enable,
    .get_cur_focus = ov4689mipi_get_cur_focus,
    .get_focus_range = ov4689mipi_get_focus_range,
    .set_focus = ov4689mipi_set_focus,
    .read_calib_data = ov4689mipi_read_calib_data,
    .read_calib_version = ov4689mipi_read_calib_version,
    .update_wb_gain = ov4689mipi_update_wb_gain,
    .exchange_calib_direction = ov4689mipi_exchange_calib_direction,
    .next =NULL,
    .prev = NULL,
};

static const double aec_min_gain = 1.0;
static const double aec_max_gain = 16.0;

static IMG_UINT8 g_OV4689_ui8LinesValHiBackup = 0xff;
static IMG_UINT8 g_OV4689_ui8LinesValMidBackup = 0xff;
static IMG_UINT8 g_OV4689_ui8LinesValLoBackup = 0xff;

static IMG_UINT8 g_OV4689_ui8GainValMidBackup = 0xff;
static IMG_UINT8 g_OV4689_ui8GainValLoBackup = 0xff;
/*
 * from spreadsheet from omnivision (applied to registers 0x3507 0x3508)
 * formula is:
 * if gain < 2.0
 *    reg = gain*128
 * elif gain < 4.0
 *    reg = 256 + (gain*64 -8)
 * elif gain < 8.0
 *    reg = 256*3 + (gain*32 -12)
 * else
 *    reg = 256*7 + (gain*16 -8)
 */
static const IMG_UINT16 aec_long_gain_val[] = {
    // 1.0
    0x0080, 0x0088, 0x0090, 0x0098, 0x00A0, 0x00A8, 0x00B0, 0x00B8,
    // 1.5
    0x00C0, 0x00C8, 0x00D0, 0x00D8, 0x00E0, 0x00E8, 0x00F0, 0x00F8,
    // 2.0
    0x0178, 0x017C, 0x0180, 0x0184, 0x0188, 0x018C, 0x0190, 0x0194,
    // 2.5
    0x0198, 0x019C, 0x01A0, 0x01A4, 0x01A8, 0x01AC, 0x01B0, 0x01B4,
    // 3.0
    0x01B8, 0x01BC, 0x01C0, 0x01C4, 0x01C8, 0x01CC, 0x01D0, 0x01D4,
    // 3.5
    0x01D8, 0x01DC, 0x01E0, 0x01E4, 0x01E8, 0x01EC, 0x01F0, 0x01F4,
    // 4.0
    0x0374, 0x0376, 0x0378, 0x037A, 0x037C, 0x037E, 0x0380, 0x0382,
    // 4.5
    0x0384, 0x0386, 0x0388, 0x038A, 0x038C, 0x038E, 0x0390, 0x0392,
    // 5.0
    0x0394, 0x0396, 0x0398, 0x039A, 0x039C, 0x039E, 0x03A0, 0x03A2,
    // 5.5
    0x03A4, 0x03A6, 0x03A8, 0x03AA, 0x03AC, 0x03AE, 0x03B0, 0x03B2,
    // 6.0
    0x03B4, 0x03B6, 0x03B8, 0x03BA, 0x03BC, 0x03BE, 0x03C0, 0x03C2,
    // 6.5
    0x03C4, 0x03C6, 0x03C8, 0x03CA, 0x03CC, 0x03CE, 0x03D0, 0x03D2,
    // 7.0
    0x03D4, 0x03D6, 0x03D8, 0x03DA, 0x03DC, 0x03DE, 0x03E0, 0x03E2,
    // 7.5
    0x03E4, 0x03E6, 0x03E8, 0x03EA, 0x03EC, 0x03EE, 0x03F0, 0x03F2,
    // 8.0
    0x0778, 0x0779, 0x077A, 0x077B, 0x077C, 0x077D, 0x077E, 0x077F,
    // 8.5
    0x0780, 0x0781, 0x0782, 0x0783, 0x0784, 0x0785, 0x0786, 0x0787,
    // 9.0
    0x0788, 0x0789, 0x078A, 0x078B, 0x078C, 0x078D, 0x078E, 0x078F,
    // 9.5
    0x0790, 0x0791, 0x0792, 0x0793, 0x0794, 0x0795, 0x0796, 0x0797,
    // 10.0
    0x0798, 0x0799, 0x079A, 0x079B, 0x079C, 0x079D, 0x079E, 0x079F,
    // 10.5
    0x07A0, 0x07A1, 0x07A2, 0x07A3, 0x07A4, 0x07A5, 0x07A6, 0x07A7,
    // 11.0
    0x07A8, 0x07A9, 0x07AA, 0x07AB, 0x07AC, 0x07AD, 0x07AE, 0x07AF,
    // 11.5
    0x07B0, 0x07B1, 0x07B2, 0x07B3, 0x07B4, 0x07B5, 0x07B6, 0x07B7,
    // 12.0
    0x07B8, 0x07B9, 0x07BA, 0x07BB, 0x07BC, 0x07BD, 0x07BE, 0x07BF,
    // 12.5
    0x07C0, 0x07C1, 0x07C2, 0x07C3, 0x07C4, 0x07C5, 0x07C6, 0x07C7,
    // 13.0
    0x07C8, 0x07C9, 0x07CA, 0x07CB, 0x07CC, 0x07CD, 0x07CE, 0x07CF,
    // 13.5
    0x07D0, 0x07D1, 0x07D2, 0x07D3, 0x07D4, 0x07D5, 0x07D6, 0x07D7,
    // 14.0
    0x07D8, 0x07D9, 0x07DA, 0x07DB, 0x07DC, 0x07DD, 0x07DE, 0x07DF,
    // 14.5
    0x07E0, 0x07E1, 0x07E2, 0x07E3, 0x07E4, 0x07E5, 0x07E6, 0x07E7,
    // 15.0
    0x07E8, 0x07E9, 0x07EA, 0x07EB, 0x07EC, 0x07ED, 0x07EE, 0x07EF,
    // 15.5
    0x07F0, 0x07F1, 0x07F2, 0x07F3, 0x07F4, 0x07F5, 0x07F6, 0x07F7,
};

static double flGain = 1.0;
static unsigned long dbExposure = 21;

void ov4689mipi_init(void)
{
    LOG_DEBUG("%s\n", __func__);
    bridge_sensor_register(&ov4689mipi_sensor);
}

IMG_RESULT ov4689mipi_set_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    IMG_RESULT ret;

    IMG_UINT16 exposureRegs[] = {
        0x3500, 0x00, //exposure register
        0x3501, 0x2E,
        0x3502, 0x80,
    };

    IMG_UINT32 ui32Lines;

    ui32Lines = exp/info->ui32ExposureMin;
    exposureRegs[1] = (ui32Lines >> 12) & 0xf;
    exposureRegs[3] = (ui32Lines >> 4) & 0xff;
    exposureRegs[5] = (ui32Lines << 4) & 0xff;

    dbExposure = exp;

    LOG_DEBUG("Exposure Val %x\n", ui32Lines);
    ret = bridge_i2c_write(i2c, exposureRegs[0], exposureRegs[1]);
    ret = bridge_i2c_write(i2c, exposureRegs[2], exposureRegs[3]);
    ret = bridge_i2c_write(i2c, exposureRegs[4], exposureRegs[5]);

    return ret;
}

IMG_RESULT ov4689mipi_set_gain(BRIDGE_I2C_CLIENT* i2c,
    double gain, SENSOR_MODE *info)
{
    IMG_UINT32 n = 0;
    IMG_RESULT ret;
    static int runing_flag = 0;

    IMG_UINT16 gainRegs[] =
    {
        0x3507, 0x00, // Gain register
        0x3508, 0x00,
        0x3509, 0x00,
    };

    if (runing_flag == 1)
    {
        return IMG_SUCCESS;
    }

    runing_flag = 1;

    if (gain > aec_min_gain)
    {
        n = (IMG_UINT32)floor((gain - 1) * 16);
    }

    n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

    gainRegs[1] = 0;
    gainRegs[3] = (aec_long_gain_val[n] >> 8) & 0xff;
    gainRegs[5] = aec_long_gain_val[n] & 0xff;

    ret = bridge_i2c_write(i2c, gainRegs[0], gainRegs[1]);
    ret = bridge_i2c_write(i2c, gainRegs[2], gainRegs[3]);
    ret = bridge_i2c_write(i2c, gainRegs[4], gainRegs[5]);

    flGain = gain;

    runing_flag = 0;
    return ret;
}

IMG_RESULT ov4689mipi_get_exposure(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32* exp, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    *exp = dbExposure;
    return IMG_SUCCESS;
}

IMG_RESULT ov4689mipi_get_gain(BRIDGE_I2C_CLIENT* i2c,
    double* gain, SENSOR_MODE *info)
{
    LOG_DEBUG("%s gain %f\n", __func__, flGain);
    *gain = flGain;
    return IMG_SUCCESS;
}

IMG_RESULT ov4689mipi_get_gain_range(BRIDGE_I2C_CLIENT* i2c,
    double *min,double *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);

    *min = aec_min_gain;
    *max = aec_max_gain;
    return IMG_SUCCESS;

}
IMG_RESULT ov4689mipi_get_exposure_range(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 *min,IMG_UINT32 *max, SENSOR_MODE *info)
{
    LOG_DEBUG("%s\n", __func__);
    *max = (int)floor(info->dbExposureMin * info->ui16VerticalTotal);
    *min  = (int)floor(info->dbExposureMin);

    return IMG_SUCCESS;
}

IMG_RESULT ov4689mipi_set_exposure_gain(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 exp, double gain, SENSOR_MODE *info)
{
    static int runing_flag = 0;
    IMG_UINT32 ui32Lines;
    IMG_UINT32 n = 0;
    IMG_UINT8 write_type = 0;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_UINT16 ui16Regs[] =
    {
        0x3508, 0x00,
        0x3509, 0x00,
        0x3500, 0x00, // Exposure register
        0x3501, 0x2E,
        0x3502, 0x80,
    };

    if (runing_flag == 1)
    {
        return IMG_SUCCESS;
    }
    runing_flag = 1;

    //01.Exposure part
    //
    ui32Lines = exp / info->dbExposureMin;
    ui16Regs[5] = (ui32Lines >> 12) & 0xf;
    ui16Regs[7] = (ui32Lines >> 4) & 0xff;
    ui16Regs[9] = (ui32Lines << 4) & 0xff;
    LOG_DEBUG("Exposure exp = %u dbExp=%f  Lines = %u \n",
        exp,  info->dbExposureMin, ui32Lines);

    //02.Gain part
    //
    if (gain > aec_min_gain)
    {
        n = (IMG_UINT32)floor((gain - 1) * 16);
    }
    n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

    ui16Regs[1] = (aec_long_gain_val[n] >> 8) & 0xff;
    ui16Regs[3] = aec_long_gain_val[n] & 0xff;
    //printf("==>Gain %f n=%u from %lf\n", gain, n, (gain - 1) * 16);

    flGain = gain;
    dbExposure = exp;

    if ((g_OV4689_ui8LinesValHiBackup != ui16Regs[5]) ||
        (g_OV4689_ui8LinesValMidBackup != ui16Regs[7]) ||
        (g_OV4689_ui8LinesValLoBackup != ui16Regs[9]))
    {
        write_type |= 0x2;
        g_OV4689_ui8LinesValHiBackup = ui16Regs[5];
        g_OV4689_ui8LinesValMidBackup = ui16Regs[7];
        g_OV4689_ui8LinesValLoBackup = ui16Regs[9];
    }

    if ((g_OV4689_ui8GainValMidBackup != ui16Regs[1]) ||
        (g_OV4689_ui8GainValLoBackup != ui16Regs[3]) )
    {
        write_type |= 0x1;
        g_OV4689_ui8GainValMidBackup = ui16Regs[1];
        g_OV4689_ui8GainValLoBackup = ui16Regs[3];
    }

    switch(write_type)
    {
        case 0x3:
            ret = bridge_i2c_write(i2c, ui16Regs[0], ui16Regs[1]);
            ret = bridge_i2c_write(i2c, ui16Regs[2], ui16Regs[3]);
            ret = bridge_i2c_write(i2c, ui16Regs[4], ui16Regs[5]);
            ret = bridge_i2c_write(i2c, ui16Regs[6], ui16Regs[7]);
            ret = bridge_i2c_write(i2c, ui16Regs[8], ui16Regs[9]);
            break;

        case 0x2:
            ret = bridge_i2c_write(i2c, ui16Regs[4], ui16Regs[5]);
            ret = bridge_i2c_write(i2c, ui16Regs[6], ui16Regs[7]);
            ret = bridge_i2c_write(i2c, ui16Regs[8], ui16Regs[9]);
            break;

       case 0x1:
           ret = bridge_i2c_write(i2c, ui16Regs[0], ui16Regs[1]);
           ret = bridge_i2c_write(i2c, ui16Regs[2], ui16Regs[3]);
           break;

        default:
            break;
    }

    runing_flag = 0;
    return ret;
}

IMG_RESULT ov4689mipi_check_id(BRIDGE_I2C_CLIENT* i2c)
{
    LOG_DEBUG("%s\n", __func__);

    IMG_RESULT ret;
    IMG_UINT16 slave_id = 0;

    ret = bridge_i2c_read(i2c, OV4689_SLAVE_ID_REG, &slave_id);
    if (ret || OV4689_SLAVE_ID!= slave_id)
    {
        LOG_ERROR("[OV4689] Failed to ensure that the i2c device has a compatible "
            "sensor! ret=%d model =0x%x (expect model 0x%x)\n",
            ret, slave_id, OV4689_SLAVE_ID);
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
       LOG_INFO("Check Model = 0x%x\n", slave_id);
    }
    return IMG_SUCCESS;
}

IMG_RESULT ov4689mipi_enable(BRIDGE_I2C_CLIENT* i2c, int on)
{
    LOG_DEBUG("%s\n", __func__);

    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 aui16Regs[] = {
        OV4689_MODE_SELECT_REG, 0x00,
    };

    if (on)
        aui16Regs[1] = OV4689_MODE_STREAMING;
    else
        aui16Regs[1] = OV4689_MODE_STANDBY;

    ret = bridge_i2c_write(i2c, aui16Regs[0], aui16Regs[1]);
    return ret;
}

// minimum focus in millimetres
static const unsigned int ov4689mipi_focus_dac_min = 50;

// maximum focus in millimetres, if >= then focus is infinity
static const unsigned int ov4689mipi_focus_dac_max = 600;

// focus values for the ov683Dual_focus_dac_dist
static const IMG_UINT16 ov4689mipi_focus_dac_val[] = {
    0x3ff, 0x180, 0x120, 0x100, 0x000
};

// distances in millimetres for the ov683Dual_focus_dac_val
static const IMG_UINT16 ov4689mipi_focus_dac_dist[] = {
    50, 150, 300, 500, 600
};

IMG_RESULT ov4689mipi_get_focus_range(BRIDGE_I2C_CLIENT* i2c,
        IMG_UINT32 *min, IMG_UINT32 *max, SENSOR_MODE *info)
{
    *min = ov4689mipi_focus_dac_min;
    *max = ov4689mipi_focus_dac_max;
    return IMG_SUCCESS;
}

IMG_RESULT ov4689mipi_get_cur_focus(BRIDGE_I2C_CLIENT* i2c,
        IMG_UINT32 *focus,  SENSOR_MODE *info)
{
    return IMG_SUCCESS;
}

/*
 * calculate focus position from points and return DAC value
 */
IMG_UINT16 ov4689mipi_calculate_focus(const IMG_UINT16 *pui16DACDist,
    const IMG_UINT16 *pui16DACValues, IMG_UINT8 ui8Entries,
    IMG_UINT16 ui16Requested)
{
    double flMinDistRcp,flMaxDistRcp,flRequestedRcp;
    double M,C;
    int i;
    /* find the entries between which the requested distance fits
     * calculate the equation of the line between those point for the
     * reciprocal of the distance
     * special casing the case where the requested point lies on one of the
     * points of the curve*/
    if (ui16Requested == pui16DACDist[0])
    {
        return pui16DACValues[0];
    }

    for(i=0;i<ui8Entries-1;i++)
    {
        if (ui16Requested == pui16DACDist[i + 1])
        {
            return pui16DACValues[i + 1];
        }
        if (ui16Requested >= pui16DACDist[i]
            && ui16Requested <= pui16DACDist[i + 1])
        {
            break;
        }
    }

    flMinDistRcp = 1.0 / pui16DACDist[i];
    flMaxDistRcp = 1.0 / pui16DACDist[i + 1];
    flRequestedRcp = 1.0 / ui16Requested;

    M = (pui16DACValues[i] - pui16DACValues[i + 1]) /
        (flMinDistRcp - flMaxDistRcp);
    C = -(M * flMinDistRcp - pui16DACValues[i]);
    return (IMG_UINT16)(M * flRequestedRcp + C);
}

// declared beforehand
IMG_RESULT ov4689mipi_set_focus(BRIDGE_I2C_CLIENT* i2c,
    IMG_UINT32 focus, SENSOR_MODE *info)
{
    IMG_RESULT ret;
    IMG_UINT16 ui16Regs[4];
    IMG_UINT16 ui16DACVal;
    BRIDGE_I2C_CLIENT auto_focus_i2c;

    if (focus >= ov4689mipi_focus_dac_max)
    {
        // special case the infinity as it doesn't fit in with the rest
        ui16DACVal = 0;
    }
    else
    {
        ui16DACVal = ov4689mipi_calculate_focus(ov4689mipi_focus_dac_dist,
            ov4689mipi_focus_dac_val, sizeof(ov4689mipi_focus_dac_dist) / sizeof(IMG_UINT16),
            focus);
    }

    bridge_i2c_client_copy(&auto_focus_i2c, OV4689_AUTOFOCUS_ADDR, i2c);

    ui16Regs[0] = 0x0004;
    ui16Regs[1] = ui16DACVal >> 8;

    ui16Regs[2] = 0x0005;
    ui16Regs[3] = ui16DACVal & 0xff;

    ret = bridge_i2c_write(&auto_focus_i2c, ui16Regs[0], ui16Regs[1]);
    ret = bridge_i2c_write(&auto_focus_i2c, ui16Regs[2], ui16Regs[3]);

    return ret;
}

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE

static int calib_direction = 0;
static IMG_UINT8 sensor_i2c_addr[2] = {(0x6C >> 1), (0x20 >> 1)}; // left, right

///////////////////////////////////////////////////////////////////////////////////
#ifdef INFOTM_E2PROM_METHOD

#ifdef OTP_DBG_LOG
#define E2PROM_DBG_LOG
#endif

#define E2PROM_FILE "/mnt/config/opt.data"
//#define E2PROM_FILE "/mnt/sd0/opt.data"
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

static IMG_RESULT ov4689mipi_update_sensor_wb_gain_e2prom(
    BRIDGE_I2C_CLIENT* i2c, IMG_FLOAT awb_convert_gain, IMG_UINT16 version);

#define E2PROM_CALIBR_REG_CNT (RING_CNT*RING_SECTOR_CNT*3 + 9)

static IMG_UINT8 sensor_e2prom_calibration_data[2][E2PROM_CALIBR_REG_CNT] = {0};
static IMG_UINT8 sensor_e2prom_version_info[2][4] = {0};
static IMG_RESULT ov4689mipi_read_e2prom_data(
    int i2c, char i2c_addr, IMG_UINT16 start_addr, int count, IMG_UINT8 *data)
{
    int read_bytes = 0;
    int offset = sizeof(E2PROM_HEADER);

    if (i2c_addr == 1)
    {
#if 0
        offset += CALIBR_REG_CNT;
#else // for temp debug
        offset = 1024 + 10;
#endif
    }

    FILE *f = fopen(E2PROM_FILE, "rb");
    if (f == NULL)
    {
        printf("==>can not open e2prom file!\n");
        return IMG_ERROR_FATAL;
    }

    fseek(f, offset, SEEK_CUR);
    read_bytes = fread(data, 1, E2PROM_CALIBR_REG_CNT, f);
    fclose(f);

    printf("===>e2prom offset %d and read %d bytes\n", offset, read_bytes);
    {
    int i = 0, j = 0, k = 0, start = 9;//9 + RING_CNT*RING_SECTOR_CNT*3 - 16*3;
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


static IMG_UINT8* ov4689mipi_read_sensor_e2prom_calibration_data(
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
    printf("===>ov4689mipi_read_sensor_e2prom_calibration_data %d\n", sensor_index);
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

    if (ov4689mipi_read_e2prom_data(i2c, sensor_index, 0, reg_cnt,
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
    //ov4689mipi_update_sensor_wb_gain_e2prom(hHandle, awb_convert_gain, *otp_calibration_version);
    }

    return sensor_e2prom_calibration_data[sensor_index];
}

static IMG_UINT8* ov4689mipi_read_sensor_e2prom_calibration_version(
    BRIDGE_I2C_CLIENT* i2c, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version)
{
    int group_index = 0;
    IMG_UINT16 start_addr = 0x7111;

    if (otp_calibration_version != NULL)
    {
        *otp_calibration_version = 0x1;
    }
    return sensor_e2prom_version_info[sensor_index];
}

#define WB_CALIBRATION_METHOD_FROM_LIPIN

static IMG_RESULT ov4689mipi_update_sensor_wb_gain_e2prom(
    BRIDGE_I2C_CLIENT* i2c, IMG_FLOAT awb_convert_gain, IMG_UINT16 version)
{
    int x, ret;
    int offset = 6/* + sizeof(E2PROM_HEADER)*/;

    IMG_FLOAT temp;
    IMG_UINT16 RGain[2] = {0x400, 0x400};
    IMG_UINT16 GGain[2] = {0x400, 0x400};
    IMG_UINT16 BGain[2] = {0x400, 0x400};

    IMG_UINT16 ui16Regs[2][18/**3*/] =
    {
        {
            0x500c, 0x04,
            0x500d, 0x00, // long r gain
            0x500e, 0x04,
            0x500f, 0x00, // long g gain
            0x5010, 0x04,
            0x5011, 0x00, // long b gain
        },
        {
            0x500c, 0x04,
            0x500d, 0x00, // long r gain
            0x500e, 0x04,
            0x500f, 0x00, // long g gain
            0x5010, 0x04,
            0x5011, 0x00, // long b gain
        }
    };

    BRIDGE_I2C_CLIENT single_chn[2];

    bridge_i2c_client_copy(&single_chn[0], sensor_i2c_addr[0], i2c);// right i2c channel
    bridge_i2c_client_copy(&single_chn[1], sensor_i2c_addr[1], i2c);// left i2c channel

    printf("ROI_L<%x, %x, %x>, ROI_R<%x, %x, %x>\n",
        sensor_e2prom_calibration_data[0][0+offset],
        sensor_e2prom_calibration_data[0][1+offset],
        sensor_e2prom_calibration_data[0][2+offset],
        sensor_e2prom_calibration_data[1][0+offset],
        sensor_e2prom_calibration_data[1][1+offset],
        sensor_e2prom_calibration_data[1][2+offset]);

#ifdef WB_CALIBRATION_METHOD_FROM_LIPIN
    // g gain
    if (sensor_e2prom_calibration_data[0][1+offset] > sensor_e2prom_calibration_data[1][1+offset])
    {
        temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[0][1+offset]*awb_convert_gain)/
                            (sensor_e2prom_calibration_data[1][1+offset]);
        GGain[1] = (IMG_UINT16)temp;
    }
    else
    {
        temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[1][1+offset]*awb_convert_gain)/
                            (sensor_e2prom_calibration_data[0][1+offset]);
        GGain[0] = 0x400;//(IMG_UINT16)temp;
    }

    // r gain
    if (sensor_e2prom_calibration_data[0][0+offset] > sensor_e2prom_calibration_data[1][0+offset])
    {
        temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[0][0+offset]*awb_convert_gain)/
                            (sensor_e2prom_calibration_data[1][0+offset]);
        RGain[1] = (IMG_UINT16)temp;
    }
    else
    {
        temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[1][0+offset]*awb_convert_gain)/
                                    (sensor_e2prom_calibration_data[0][0+offset]);
        RGain[0] = (IMG_UINT16)temp;
    }

    // b gain
    if (sensor_e2prom_calibration_data[0][2+offset] > sensor_e2prom_calibration_data[1][2+offset])
    {
        temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[0][2+offset]*awb_convert_gain)/
                                    (sensor_e2prom_calibration_data[1][2+offset]);
        BGain[1] = (IMG_UINT16)temp;
    }
    else
    {
        temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[1][2+offset]*awb_convert_gain)/
                                    (sensor_e2prom_calibration_data[0][2+offset]);
        BGain[0] = 0x400;//(IMG_UINT16)temp;
    }
#ifdef OTP_DBG_LOG
    printf("===>RGain_0 = %x, GGain_0 = %x, BGain_0 = %x, RGain_1 = %x, GGain_1 = %x, BGain_1 = %x\n",
        RGain[0], GGain[0], BGain[0], RGain[1], GGain[1], BGain[1]);
#endif

    ui16Regs[0][2] = (RGain[0]>>8); ui16Regs[0][5] = (RGain[0]&0xFF);
    ui16Regs[0][8] = (GGain[0]>>8); ui16Regs[0][11] = (GGain[0]&0xFF);
    ui16Regs[0][14] = (BGain[0]>>8); ui16Regs[0][17] = (BGain[0]&0xFF);

    ui16Regs[1][2] = RGain[1]>>8; ui16Regs[1][5] = RGain[1]&0xFF;
    ui16Regs[1][8] = GGain[1]>>8; ui16Regs[1][11] = GGain[1]&0xFF;
    ui16Regs[1][14] = BGain[1]>>8; ui16Regs[1][17] = BGain[1]&0xFF;

    for (x = 0; x < 2; x++)
    {
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][0], ui16Regs[x][1]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][2], ui16Regs[x][3]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][4], ui16Regs[x][5]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][6], ui16Regs[x][7]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][8], ui16Regs[x][9]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][10], ui16Regs[x][11]);
    }
#endif
	return IMG_SUCCESS;
}
#endif
///////////////////////////////////////////////////////////////////////////////////

#define CALIBR_REG_CNT (29*3+2)

static IMG_UINT8 sensor_calibration_data[2][CALIBR_REG_CNT] = {{0}};
static IMG_UINT8 sensor_version_info[2][4] = {{0}};
static IMG_UINT16 sensor_version_no[2] = {0x1400, 0x1400};

static IMG_RESULT read_otp_data(
    BRIDGE_I2C_CLIENT* i2c , char i2c_addr,
    IMG_UINT16 start_addr, int count, IMG_UINT8 *data)
{
    int i = 0;
    IMG_UINT16 otp_reg_val = 0;

    IMG_UINT16 ui16Regs[12] =
    {
        0x5000, 0x00,
        0x3d84, 0xc0,
        0x3d88, 0x00,
        0x3d89, 0x00,
        0x3d8a, 0x00,
        0x3d8b, 0x00,
    };

    BRIDGE_I2C_CLIENT opt_i2c_chn;

    bridge_i2c_client_copy(&opt_i2c_chn, i2c_addr, i2c);

    /* start address settings */
    ui16Regs[5] = start_addr>>8;
    ui16Regs[7] = start_addr&0xff;

    ui16Regs[9] = (start_addr + count - 1)>>8;
    ui16Regs[11] = (start_addr + count - 1)&0xff;

    IMG_ASSERT(data != NULL);  // null pointer forbidden

    // enable otp
    bridge_i2c_read(&opt_i2c_chn, 0x5000, &otp_reg_val);
    ui16Regs[1] = ((otp_reg_val&0xff) | 0x20);

    // set address range to otp
    bridge_i2c_write(&opt_i2c_chn, ui16Regs[0], ui16Regs[1]);
    bridge_i2c_write(&opt_i2c_chn, ui16Regs[2], ui16Regs[3]);
    bridge_i2c_write(&opt_i2c_chn, ui16Regs[4], ui16Regs[5]);
    bridge_i2c_write(&opt_i2c_chn, ui16Regs[6], ui16Regs[7]);
    bridge_i2c_write(&opt_i2c_chn, ui16Regs[8], ui16Regs[9]);
    bridge_i2c_write(&opt_i2c_chn, ui16Regs[10], ui16Regs[11]);

    // clear otp buffer
    for (i = 0; i < count; i++)
    {
        ui16Regs[0] = start_addr + i;
        ui16Regs[1] = 0x00;
        bridge_i2c_write(&opt_i2c_chn, ui16Regs[0], ui16Regs[1]);
    }

    // load otp data
    ui16Regs[0] = 0x3d81;
    ui16Regs[1] = 0x01;
    bridge_i2c_write(&opt_i2c_chn, ui16Regs[0], ui16Regs[1]);

    usleep(20*1000);

    for (i = 0; i < count; i++)
    {
        bridge_i2c_read(&opt_i2c_chn, start_addr + i, (IMG_UINT16*)&data[i]);
#ifdef OTP_DBG_LOG
        LOG_INFO("%d\n ", data[i]);
        if (i == (count - 1) || (i > 0 && i%9 == 0))
        {
            LOG_INFO("\n", data[i]);
        }
#endif
    }

    // clear otp buffer
    for (i = 0; i < count; i++)
    {
        ui16Regs[0] = start_addr + i;
        ui16Regs[1] = 0x00;
        bridge_i2c_write(&opt_i2c_chn, ui16Regs[0], ui16Regs[1]);
    }

    return IMG_SUCCESS;
}

static int get_valid_otp_group(BRIDGE_I2C_CLIENT* i2c,
    char i2c_addr, IMG_UINT16 group_flag_addr)
{
    int group_index = -1;
    IMG_UINT8 flag_ret = 0;

    read_otp_data(i2c, i2c_addr, group_flag_addr, 1, &flag_ret);

    if ((flag_ret&0x3) == 0x01) // group 4
    {
        group_index = 4;
    }
    else if ((flag_ret&0x0c) == 0x04)// group 3
    {
        group_index = 3;
    }
    else if((flag_ret&0x30) == 0x10) // group 2
    {
        group_index = 2;
    }
    else if((flag_ret&0xc0) == 0x40) // group 1
    {
        group_index = 1;
    }
    else
    {
        group_index = 0xFF;
    }

    return group_index;
}

IMG_UINT8* ov4689mipi_read_calib_data(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info)
{
    int group_index = 0;
    IMG_UINT16 start_addr = 0x7115;
    int reg_cnt = 0;

    static IMG_BOOL sensor_inited_flag_L = IMG_FALSE, sensor_inited_flag_R = IMG_FALSE;

    if (product == 2)
    {// e2prom calib
        return ov4689mipi_read_sensor_e2prom_calibration_data(i2c, idx, gain, version);
    }

    if (product == 0 || product == 1)
    {
        sensor_version_no[0] = 0x1400;
        *version = sensor_version_no[0];
    }

    switch (sensor_version_no[0] & 0xFF00)
    {
    case 0x1400: // v1.4
        reg_cnt = 89;
        break;
    case 0x1300: // v1.3
        reg_cnt = 75;
        break;
    default: // v1.2
        reg_cnt = 60;
        break;
    }

#ifdef OTP_DBG_LOG
    LOG_INFO("===>read_sensor_calibration_data %d\n", idx);
#endif

#ifdef INFOTM_SKIP_OTP_DATA
    return NULL;
#endif

    if ((calib_direction == 1 && ((idx == 1 && sensor_inited_flag_L) || (idx == 0 && sensor_inited_flag_R))) ||
        (calib_direction == 0 && ((idx == 0 && sensor_inited_flag_L) || (idx == 1 && sensor_inited_flag_R))))
    {
        return sensor_calibration_data[idx];
    }

    group_index = get_valid_otp_group(i2c, sensor_i2c_addr[idx], 0x7110);
    switch (group_index)
    {
    case 1:
        start_addr = 0x7115;
        break;
    case 2:
        start_addr = 0x7172;
        break;

    case 3:
        start_addr = 0x71f8;
        break;
    case 4:
        start_addr = 0x71fb;
        break;

    default:
#ifdef OTP_DBG_LOG
        LOG_ERROR("invalid group index %d\n", group_index);
#endif
        return NULL;
    }

    if (group_index > 2)
    {
        reg_cnt = 3;
        *version = 0x1500;
    }

    read_otp_data(i2c,
        sensor_i2c_addr[idx], start_addr, reg_cnt,
        sensor_calibration_data[idx]);
    if (idx == 0)
    {
        if (calib_direction)
        {
            sensor_inited_flag_R = IMG_TRUE;
        }
        else
        {
            sensor_inited_flag_L = IMG_TRUE;
        }
    }
    else if (idx == 1)
    {
        if (calib_direction)
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
        ov4689mipi_update_wb_gain(i2c, product, gain, *version);
    }

    return sensor_calibration_data[idx];
}

IMG_UINT8* ov4689mipi_read_calib_version(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info)
{
    int group_index = 0;
    IMG_UINT16 start_addr = 0x7111;

    if (product == 2)
    {// e2prom calib
        return ov4689mipi_read_sensor_e2prom_calibration_version(i2c, idx, version);
    }

    group_index = get_valid_otp_group(i2c, sensor_i2c_addr[idx], 0x7110);
    switch (group_index)
    {
    case 1:
        start_addr = 0x7111;
        break;
    case 2:
        start_addr = 0x716e;
        break;
    case 3:
    case 4:
        start_addr = 0x716e;
        break;
    default:
        return NULL;
    }

    read_otp_data(i2c, sensor_i2c_addr[idx], start_addr, 4, sensor_version_info[idx]);

    LOG_INFO("===>sensor-%d: mid-%d, year-%d, month-%d, day-%d<===\n",
        sensor_version_info[idx][0], sensor_version_info[idx][1],
        sensor_version_info[idx][2], sensor_version_info[idx][3]);

    if (version != NULL)
    {
        *version = sensor_version_no[0];
    }
    return sensor_version_info[idx];
}

#define WB_CALIBRATION_METHOD_FROM_LIPIN
IMG_RESULT ov4689mipi_update_wb_gain(BRIDGE_I2C_CLIENT* i2c,
    int product, IMG_FLOAT gain, IMG_UINT16 version)
{
    IMG_RESULT ret;
    int x= 0;
    IMG_UINT16 ret_data[18];

    BRIDGE_I2C_CLIENT single_chn[2];

    bridge_i2c_client_copy(&single_chn[0], sensor_i2c_addr[0], i2c);// right i2c channel
    bridge_i2c_client_copy(&single_chn[1], sensor_i2c_addr[1], i2c);// left i2c channel

    IMG_UINT16 RGain[2] = {0x400, 0x400};
    IMG_UINT16 GGain[2] = {0x400, 0x400};
    IMG_UINT16 BGain[2] = {0x400, 0x400};

    IMG_UINT16 ui16Regs[2][12] =
    {
        {
            0x500c, 0x04,
            0x500d, 0x00, // long r gain
            0x500e, 0x04,
            0x500f, 0x00, // long g gain
            0x5010, 0x04,
            0x5011, 0x00, // long b gain
        },
        {
            0x500c, 0x04,
            0x500d, 0x00, // long r gain
            0x500e, 0x04,
            0x500f, 0x00, // long g gain
            0x5010, 0x04,
            0x5011, 0x00, // long b gain
        }
    };

#ifdef WB_CALIBRATION_METHOD_FROM_LIPIN
    if (product == 0 && version >= 0x1500)
    {
        // g gain
        if (sensor_calibration_data[0][1] > sensor_calibration_data[1][1])
        {
            GGain[1] = (0x400*sensor_calibration_data[0][1]*gain)/
                (sensor_calibration_data[1][1]);
        }
        else
        {
            GGain[0] = (0x400*sensor_calibration_data[1][1]*gain)/
                (sensor_calibration_data[0][1]);
        }

        // r gain
        if (sensor_calibration_data[0][0] > sensor_calibration_data[1][0])
        {
            RGain[1] = (0x400*sensor_calibration_data[0][0]*gain)/
                (sensor_calibration_data[1][0]);
        }
        else
        {
            RGain[0] = (0x400*sensor_calibration_data[1][0]*gain)/
                (sensor_calibration_data[0][0]);
        }

        // b gain
        if (sensor_calibration_data[0][2] > sensor_calibration_data[1][2])
        {
            BGain[1] = (0x400*sensor_calibration_data[0][2]*gain)/
                (sensor_calibration_data[1][2]);
        }
        else
        {
            BGain[0] = (0x400*sensor_calibration_data[1][2]*gain)/
                (sensor_calibration_data[0][2]);
        }

#ifdef OTP_DBG_LOG
        LOG_INFO("===>RGain_0 = %x, GGain_0 = %x, BGain_0 = %x, "
            "RGain_1 = %x, GGain_1 = %x, BGain_1 = %x\n",
            RGain[0], GGain[0], BGain[0], RGain[1], GGain[1], BGain[1]);
#endif
        return IMG_SUCCESS;
    }

    if (product == 0)
    {
        // g gain
        if (sensor_calibration_data[0][1+9] > sensor_calibration_data[1][1+9])
        {
            GGain[1] = (0x400*sensor_calibration_data[0][1+9]*gain)/
                (sensor_calibration_data[1][1+9]);
        }
        else
        {
            GGain[0] = (0x400*sensor_calibration_data[1][1+9]*gain)/
                (sensor_calibration_data[0][1+9]);
        }

        // r gain
        if (sensor_calibration_data[0][0+9] > sensor_calibration_data[1][0+9])
        {
            RGain[1] = (0x400*sensor_calibration_data[0][0+9]*gain)/
                (sensor_calibration_data[1][0+9]);
        }
        else
        {
            RGain[0] = (0x400*sensor_calibration_data[1][0+9]*gain)/
                (sensor_calibration_data[0][0+9]);
        }

        // b gain
        if (sensor_calibration_data[0][2+9] > sensor_calibration_data[1][2+9])
        {
            BGain[1] = (0x400*sensor_calibration_data[0][2+9]*gain)/
                (sensor_calibration_data[1][2+9]);
        }
        else
        {
            BGain[0] = (0x400*sensor_calibration_data[1][2+9]*gain)/
                (sensor_calibration_data[0][2+9]);
        }
    }
    else
    {
#ifdef SMOOTH_CENTER_RGB
        float ratio =
        ((float)sensor_calibration_data[0][1] - sensor_calibration_data[0][4])/
            sensor_calibration_data[0][4];

        if (ratio >= 0.2)
        {
            sensor_calibration_data[0][0] =
                (sensor_calibration_data[0][0] + sensor_calibration_data[0][3])/2;
            sensor_calibration_data[0][1] =
                (sensor_calibration_data[0][1] + sensor_calibration_data[0][4])/2;
            sensor_calibration_data[0][2] =
                (sensor_calibration_data[0][2] + sensor_calibration_data[0][5])/2;
        }

        LOG_INFO("===>ratioL %f\n", ratio);
        ratio = ((float)sensor_calibration_data[1][1] - sensor_calibration_data[1][4])/
            sensor_calibration_data[1][4];
        if (ratio >= 0.2)
        {
            sensor_calibration_data[1][0] =
                (sensor_calibration_data[1][0] + sensor_calibration_data[1][3])/2;
            sensor_calibration_data[1][1] =
                (sensor_calibration_data[1][1] + sensor_calibration_data[1][4])/2;
            sensor_calibration_data[1][2] =
                (sensor_calibration_data[1][2] + sensor_calibration_data[1][5])/2;
        }

        LOG_INFO("===>ratioR %f\n", ratio);
#endif

        // g gain
        if (sensor_calibration_data[0][1] > sensor_calibration_data[1][1])
        {
            GGain[1] = (0x400*sensor_calibration_data[0][1]*gain)/
                (sensor_calibration_data[1][1]);
        }
        else
        {
            GGain[0] = (0x400*sensor_calibration_data[1][1]*gain)/
                (sensor_calibration_data[0][1]);
        }

        // r gain
        if (sensor_calibration_data[0][0] > sensor_calibration_data[1][0])
        {
            RGain[1] = (0x400*sensor_calibration_data[0][0]*gain)/
                (sensor_calibration_data[1][0]);
        }
        else
        {
            RGain[0] = (0x400*sensor_calibration_data[1][0]*gain)/
                (sensor_calibration_data[0][0]);
        }

        // b gain
        if (sensor_calibration_data[0][2] > sensor_calibration_data[1][2])
        {
            BGain[1] = (0x400*sensor_calibration_data[0][2]*gain)/
                (sensor_calibration_data[1][2]);
        }
        else
        {
            BGain[0] = (0x400*sensor_calibration_data[1][2]*gain)/
                (sensor_calibration_data[0][2]);
        }
    }

#ifdef OTP_DBG_LOG
    LOG_INFO("===>RGain_0 = %x, GGain_0 = %x, BGain_0 = %x, "
        "RGain_1 = %x, GGain_1 = %x, BGain_1 = %x\n",
        RGain[0], GGain[0], BGain[0], RGain[1], GGain[1], BGain[1]);

    for (x = 0; x < 2; x++)
    {
        bridge_i2c_read(&single_chn[x], 0x500C,&ret_data[0]);
        bridge_i2c_read(&single_chn[x], 0x500C, &ret_data[0]);
        bridge_i2c_read(&single_chn[x], 0x500D, &ret_data[1]);
        bridge_i2c_read(&single_chn[x], 0x5012, &ret_data[2]);
        bridge_i2c_read(&single_chn[x], 0x5013, &ret_data[3]);
        bridge_i2c_read(&single_chn[x], 0x5018, &ret_data[4]);
        bridge_i2c_read(&single_chn[x], 0x5019, &ret_data[5]);

        bridge_i2c_read(&single_chn[x], 0x500E, &ret_data[6]);
        bridge_i2c_read(&single_chn[x], 0x500F, &ret_data[7]);
        bridge_i2c_read(&single_chn[x], 0x5014, &ret_data[8]);
        bridge_i2c_read(&single_chn[x], 0x5015, &ret_data[9]);
        bridge_i2c_read(&single_chn[x], 0x501a, &ret_data[10]);
        bridge_i2c_read(&single_chn[x], 0x501b, &ret_data[11]);

        bridge_i2c_read(&single_chn[x], 0x5010, &ret_data[12]);
        bridge_i2c_read(&single_chn[x], 0x5011, &ret_data[13]);
        bridge_i2c_read(&single_chn[x], 0x5016, &ret_data[14]);
        bridge_i2c_read(&single_chn[x], 0x5017, &ret_data[15]);
        bridge_i2c_read(&single_chn[x], 0x501c, &ret_data[16]);
        bridge_i2c_read(&single_chn[x], 0x501d, &ret_data[17]);

        LOG_INFO("===>origial sensor_%x long RGain = %x,"
                " middle RGain = %x, short RGain = %x,\n"
                "long GGain = %x, middle GGain = %x, short GGain = %x\n"
                "long BGain = %x, middle BGain = %x, short BGain = %x\n",
                x, (ret_data[0]<<8)|ret_data[1],
                (ret_data[2]<<8)|ret_data[3],
                (ret_data[4]<<8)|ret_data[5],
                (ret_data[6]<<8)|ret_data[7],
                (ret_data[8]<<8)|ret_data[9],
                (ret_data[10]<<8)|ret_data[11],
                (ret_data[12]<<8)|ret_data[13],
                (ret_data[14]<<8)|ret_data[15],
                (ret_data[16]<<8)|ret_data[17]);
    }
#endif

    ui16Regs[0][1] = (RGain[0]>>8); ui16Regs[0][3] = (RGain[0]&0xFF);
    ui16Regs[0][5] = (GGain[0]>>8); ui16Regs[0][7] = (GGain[0]&0xFF);
    ui16Regs[0][9] = (BGain[0]>>8); ui16Regs[0][11] = (BGain[0]&0xFF);

    ui16Regs[1][1] = RGain[1]>>8; ui16Regs[1][3] = RGain[1]&0xFF;
    ui16Regs[1][5] = GGain[1]>>8; ui16Regs[1][7] = GGain[1]&0xFF;
    ui16Regs[1][9] = BGain[1]>>8; ui16Regs[1][11] = BGain[1]&0xFF;

    for (x = 0; x < 2; x++)
    {
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][0], ui16Regs[x][1]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][2], ui16Regs[x][3]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][4], ui16Regs[x][5]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][6], ui16Regs[x][7]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][8], ui16Regs[x][9]);
        ret = bridge_i2c_write(&single_chn[x], ui16Regs[x][10], ui16Regs[x][11]);
    }

#ifdef OTP_DBG_LOG
    for (x = 0; x < 2; x++)
    {
        bridge_i2c_read(&single_chn[x], 0x500C, &ret_data[0]);
        bridge_i2c_read(&single_chn[x], 0x500D, &ret_data[1]);
        bridge_i2c_read(&single_chn[x], 0x5012, &ret_data[2]);
        bridge_i2c_read(&single_chn[x], 0x5013, &ret_data[3]);
        bridge_i2c_read(&single_chn[x], 0x5018, &ret_data[4]);
        bridge_i2c_read(&single_chn[x], 0x5019, &ret_data[5]);

        bridge_i2c_read(&single_chn[x], 0x500E, &ret_data[6]);
        bridge_i2c_read(&single_chn[x], 0x500F, &ret_data[7]);
        bridge_i2c_read(&single_chn[x], 0x5014, &ret_data[8]);
        bridge_i2c_read(&single_chn[x], 0x5015, &ret_data[9]);
        bridge_i2c_read(&single_chn[x], 0x501a, &ret_data[10]);
        bridge_i2c_read(&single_chn[x], 0x501b, &ret_data[11]);

        bridge_i2c_read(&single_chn[x], 0x5010, &ret_data[12]);
        bridge_i2c_read(&single_chn[x],  0x5011, &ret_data[13]);
        bridge_i2c_read(&single_chn[x],  0x5016, &ret_data[14]);
        bridge_i2c_read(&single_chn[x], 0x5017, &ret_data[15]);
        bridge_i2c_read(&single_chn[x], 0x501c, &ret_data[16]);
        bridge_i2c_read(&single_chn[x],  0x501d, &ret_data[17]);

        LOG_INFO("===>new sensor_%x long RGain = %x, "
                "middle RGain = %x, short RGain = %x,\n"
                "long GGain = %x, middle GGain = %x, short GGain = %x\n"
                "long BGain = %x, middle BGain = %x, short BGain = %x\n",
                x, (ret_data[0]<<8)|ret_data[1],
                (ret_data[2]<<8)|ret_data[3],
                (ret_data[4]<<8)|ret_data[5],
                (ret_data[6]<<8)|ret_data[7],
                (ret_data[8]<<8)|ret_data[9],
                (ret_data[10]<<8)|ret_data[11],
                (ret_data[12]<<8)|ret_data[13],
                (ret_data[14]<<8)|ret_data[15],
                (ret_data[16]<<8)|ret_data[17]);
    }
#endif

    for (x = 0; x < CALIBR_REG_CNT - 2; x += 3)
    {
        sensor_calibration_data[0][x] = (RGain[0]*sensor_calibration_data[0][x])/0x400;
        sensor_calibration_data[1][x] = (RGain[1]*sensor_calibration_data[1][x])/0x400;
    }

    for (x = 1; x < CALIBR_REG_CNT - 2; x += 3)
    {
        sensor_calibration_data[0][x] = (GGain[0]*sensor_calibration_data[0][x])/0x400;
        sensor_calibration_data[1][x] = (GGain[1]*sensor_calibration_data[1][x])/0x400;
    }

    for (x = 2; x < CALIBR_REG_CNT - 2; x += 3)
    {
        sensor_calibration_data[0][x] = (BGain[0]*sensor_calibration_data[0][x])/0x400;
        sensor_calibration_data[1][x] = (BGain[1]*sensor_calibration_data[1][x])/0x400;
    }
#endif

    return ret;
}

IMG_RESULT ov4689mipi_exchange_calib_direction(BRIDGE_I2C_CLIENT* i2c, int flag)
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

#else
IMG_UINT8* ov4689mipi_read_calib_data(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_FLOAT gain, IMG_UINT16 *version, SENSOR_MODE *info)
{
    return NULL;
}

IMG_UINT8* ov4689mipi_read_calib_version(BRIDGE_I2C_CLIENT* i2c,
    int product, int idx, IMG_UINT16 *version, SENSOR_MODE *info)
{
    return NULL;
}

IMG_RESULT ov4689mipi_update_wb_gain(BRIDGE_I2C_CLIENT* i2c,
    int product, IMG_FLOAT gain,  IMG_UINT16 version)
{
    return IMG_SUCCESS;
}

IMG_RESULT ov4689mipi_exchange_calib_direction(BRIDGE_I2C_CLIENT* i2c, int flag)
{
    return IMG_SUCCESS;
}
#endif
