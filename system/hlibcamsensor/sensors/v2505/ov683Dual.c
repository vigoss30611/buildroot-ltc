/**
 ******************************************************************************
@file ov683Dual.c

@brief OV683Dual camera implementation

To add a new mode declare a new array containing 8b values:
- 2 first values are addresses
- last 8b is the value

The array MUST finish with STOP_REG, STOP_REG, STOP_REG so that the size can
be computed by the OV683Dual_GetRegisters()

The array can have delay of X ms using DELAY_REG, X, DELAY_REG

Add the array to ov683Dual_modes[] with the relevant flipping and a size of 0

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

 *****************************************************************************/

#include "sensors/ov683Dual.h"
#include "sensors/ddk_sensor_driver.h"
#include <img_types.h>
#include <img_errors.h>

#include <ci/ci_api_structs.h> // access to CI_GASKET
#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if !file_i2c
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#endif

#define SUPPORT_4_LANE

#define OV683Dual_FLIP_MIRROR_FRIST

#ifdef INFOTM_ISP
static IMG_RESULT OV683Dual_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag);
static IMG_RESULT OV683Dual_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
static IMG_RESULT OV683Dual_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed);
static IMG_RESULT OV683Dual_SetFPS(SENSOR_HANDLE hHandle, double fps);

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
static IMG_UINT8* OV683Dual_read_sensor_calibration_version(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version);
static IMG_UINT8* OV683Dual_read_sensor_calibration_data(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version);
static IMG_RESULT OV683Dual_update_sensor_wb_gain(SENSOR_HANDLE hHandle, int infotm_method, IMG_FLOAT awb_convert_gain, IMG_UINT16 version);
static IMG_RESULT OV683Dual_exchange_calib_direction(SENSOR_HANDLE hHandle, int flag);
#endif

#endif //INFOTM_ISP

/* disabled as i2c drivers locks up if device is not present */
#define NO_DEV_CHECK

//#define I2C_NO_SWITCH

#define LOG_TAG "OV683Dual_SENSOR"
#include <felixcommon/userlog.h>

/** @ the setup for mode does not work if enabled is not call just after
 * - temporarily do the whole of the setup in in enable function
 * - can remove that define and its checking once fixed
 */
// if defined writes all registers at enable time instead of configure
#define DO_SETUP_IN_ENABLE

#define I2C_OV683_ADDR 0x42
#define I2C_OV683_ADDR_1 0x44

#define OV683Dual_BC_WRITE_ADDR (0x42 >> 1)

#define OV683Dual_WRITE_ADDR_1 (0x6C >> 1)
#define OV683Dual_READ_ADDR_1 (0x6D >> 1)

#define OV683Dual_WRITE_ADDR_2 (0x20 >> 1)
#define OV683Dual_READ_ADDR_2 (0x21 >> 1)

#define AUTOFOCUS_WRITE_ADDR (0x18 >> 1)
#define AUTOFOCUS_READ_ADDR (0x19 >> 1)

#define OV683Dual_SENSOR_VERSION "not-verified"

#define OV683Dual_CHIP_VERSION 0x4688

// fake register value to interpret next value as delay in ms
#define DELAY_REG 0xff
// not a real register - marks the end of register array
#define STOP_REG 0xfe

/** @brief sensor number of IMG PHY */
#define OV683Dual_PHY_NUM 1

/*
 * Choice:
 * copy of the registers to apply because we want to apply them all at
 * once because some registers need 2 writes while others don't (may have
 * an effect on stability but was not showing in testing)
 * 
 * If not defined will apply values register by register and not need a copy
 * of the registers for the active mode.
 */
//#define OV683Dual_COPY_REGS

// uncomment to write i2c commands to file
//#define ov683Dual_debug "ov683Dual_write.txt"

#ifdef WIN32 // on windows we do not need to sleep to wait for the bus
static void usleep(int t)
{
    (void)t;
}
#endif

typedef struct _OV683Dualcam_struct
{
    SENSOR_FUNCS sFuncs;

    // in MHz
    double refClock;

    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
    IMG_UINT8 ui8CurrentFlipping;
    IMG_BOOL8 bEnabled;

    SENSOR_MODE currMode;
    /*
     * if OV683Dual_COPY_REGS is defined:
     * copy of the registers to apply because we want to apply them all at
     * once because some registers need 2 writes while others don't
     * 
     * otherwise just a pointer to the registers
     */
    IMG_UINT8 *currModeReg;
    /* number of registers in the mode
     * (each registers uses 3 bytes: first 2 are address, last one is value)
     */
    IMG_UINT32 nRegisters;
    IMG_UINT32 ui32ExposureMax;
    IMG_UINT32 ui32Exposure;
    IMG_UINT32 ui32ExposureMin;
    IMG_DOUBLE dbExposureMin;
    double flGain;
    IMG_UINT16 ui16CurrentFocus;
    IMG_UINT8 imager;
#ifdef INFOTM_ISP
    double fCurrentFps;
    IMG_UINT32 fixedFps;
    IMG_UINT32 initClk;
#endif
    int i2c;
    SENSOR_PHY *psSensorPhy;
}OV683DualCAM_STRUCT;

static void OV683Dual_ApplyMode(OV683DualCAM_STRUCT *psCam);
static void OV683Dual_ComputeGains(double flGain, IMG_UINT8 *gainSequence);
static void OV683Dual_ComputeExposure(IMG_UINT32 ui32Exposure,
    IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureSequence);
static IMG_RESULT OV683Dual_GetModeInfo(OV683DualCAM_STRUCT *psCam,
    IMG_UINT16 ui16Mode, SENSOR_MODE *info);
static IMG_UINT16 OV683Dual_CalculateFocus(const IMG_UINT16 *pui16DACDist,
        const IMG_UINT16 *pui16DACValues, IMG_UINT8 ui8Entries,
        IMG_UINT16 ui16Requested);
// used before implementation therefore declared here
static IMG_RESULT OV683Dual_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus);

// from omnivision data sheet
#define SPLIT0(R) ((R>>8)&0xFF)
#define SPLIT1(R) (R&0xFF)
#define IS_REG(arr, off, name) \
    ((SPLIT0(name) == arr[off+0]) && (SPLIT1(name) == arr[off+1]))

#define REG_FORMAT1 0x3820 // used for flipping
#define REG_FORMAT2 0x3821 // used for flipping

#define REG_H_CROP_START_0 0x3800 // bits [4;0] address [12;8]
#define REG_H_CROP_START_1 0x3801 // bits [4;0] address [7;0]
#define REG_V_CROP_START_0 0x3802 // bits [3;0] address [11;8]
#define REG_V_CROP_START_1 0x3803 // bits [7;0] address [7;0]
#define REG_H_CROP_END_0 0x3804 // bits [4;0] address [12;8]
#define REG_H_CROP_END_1 0x3805 // bits [7;0] address [7;0]
#define REG_V_CROP_END_0 0x3806 // bits [3;0] address [11;8]
#define REG_V_CROP_END_1 0x3807 // bits [7;0] address [7;0]

#define REG_H_OUTPUT_SIZE_0 0x3808 // bits [4;0] size [12;8]
#define REG_H_OUTPUT_SIZE_1 0x3809 // bits [7;0] size [7;0]
#define REG_V_OUTPUT_SIZE_0 0x380A // bits [4;0] size [12;8]
#define REG_V_OUTPUT_SIZE_1 0x380B // bits [7;0] size [7;0]

#define REG_TIMING_HTS_0 0x380C // bits [6;0] size [14;8]
#define REG_TIMING_HTS_1 0x380D // bits [7;0] size [7;0]
#define REG_TIMING_VTS_0 0x380E // bits [6;0] size [14;8]
#define REG_TIMING_VTS_1 0x380F // bits [7;0] size [7;0]

#define REG_SC_CMMN_BIT_SEL 0x3031 // bits [4;0] for bitdepth
#define REG_SC_CMMN_MIPI_SC_CTRL 0x3018 // bits [7;5] for mipi lane mode

#define REG_PLL_CTRL_B 0x030B // PLL2_prediv (values see pll_ctrl_b_val)
#define REG_PLL_CTRL_C 0x030C // PLL2_mult [1;0] multiplier [9;8]
#define REG_PLL_CTRL_D 0x030D // PLL2_mult [7;0] multiplier [7;0]
#define REG_PLL_CTRL_E 0x030E // PLL2_divs (values see pll_ctrl_c_val)
#define REG_PLL_CTRL_F 0x030F // PLL2_divsp (values is 1/(r+1))
#define REG_PLL_CTRL_11 0x0311 // PLL2_predivp (values is 1/(r+1)

#define REG_SC_CMMN_CHIP_ID_0 0x300A // high bits for chip version
#define REG_SC_CMMN_CHIP_ID_1 0x300B // low bits for chip version

#define REG_EXPOSURE 0x3500
#define REG_GAIN 0x3507

// values for PLL2_prediv 1/x, see sensor data-sheet
static const double pll_ctrl_b_val[] = {
    1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 6.0, 8.0
};

// values for PLL2_divs 1/x, see sensor data-sheet
static const double pll_ctrl_c_val[] = {
    1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0
};

static const double aec_min_gain = 1.0;
static const double aec_max_gain = 16.0;//16.0;
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

// minimum focus in millimetres
static const unsigned int ov683Dual_focus_dac_min = 50;

// maximum focus in millimetres, if >= then focus is infinity
static const unsigned int ov683Dual_focus_dac_max = 600;

// focus values for the ov683Dual_focus_dac_dist
static const IMG_UINT16 ov683Dual_focus_dac_val[] = {
    0x3ff, 0x180, 0x120, 0x100, 0x000
};

// distances in millimetres for the ov683Dual_focus_dac_val
static const IMG_UINT16 ov683Dual_focus_dac_dist[] = {
    50, 150, 300, 500, 600
};

//
#ifdef INFOTM_ISP
static IMG_UINT32 i2c_addr = 0;
#endif

#define SENSOR_SLAVE_NUM 1
char i2c_slave_addr[SENSOR_SLAVE_NUM] = {OV683Dual_BC_WRITE_ADDR};

// modes declaration
//

// 4lane 60fps
static IMG_UINT8 ui8ModeRegs_1920_1088_60_4lane[]=
{

	0x01, 0x03, 0x01,
	0x36, 0x38, 0x00,
	0x03, 0x00, 0x04,//0x02,
	0x03, 0x02, 0x40,//0x32,
	0x03, 0x03, 0x00,
	0x03, 0x04, 0x03,

	0x03, 0x05, 0x02,
	0x03, 0x06, 0x02,

	0x03, 0x0a, 0x00,  // pll1 prediv
	0x03, 0x0b, 0x00,
	0x03, 0x0c, 0x00,
	0x03, 0x0d, 0x1e,
	0x03, 0x0e, 0x04,
	0x03, 0x0f, 0x01,
	0x03, 0x11, 0x00,
	0x03, 0x12, 0x01,
	0x03, 0x1e, 0x00,
	0x30, 0x00, 0x20,
	0x30, 0x02, 0x00,
	0x30, 0x18, 0x72, //0x72,
	0x30, 0x20, 0x93,
	0x30, 0x21, 0x03,
	0x30, 0x22, 0x01,
	0x30, 0x31, 0x0a,
	0x30, 0x3f, 0x0c,
	0x33, 0x05, 0xf1,
	0x33, 0x07, 0x04,
	0x33, 0x09, 0x29,
	0x35, 0x00, 0x00,
	0x35, 0x01, 0x20,
	0x35, 0x02, 0x00,
	0x35, 0x03, 0x04,
	0x35, 0x04, 0x00,
	0x35, 0x05, 0x00,
	0x35, 0x06, 0x00,
	0x35, 0x07, 0x00,
	0x35, 0x08, 0x00,
	0x35, 0x09, 0x80,
	0x35, 0x0a, 0x00,
	0x35, 0x0b, 0x00,
	0x35, 0x0c, 0x00,
	0x35, 0x0d, 0x00,
	0x35, 0x0e, 0x00,
	0x35, 0x0f, 0x80,
	0x35, 0x10, 0x00,
	0x35, 0x11, 0x00,
	0x35, 0x12, 0x00,
	0x35, 0x13, 0x00,
	0x35, 0x14, 0x00,
	0x35, 0x15, 0x80,
	0x35, 0x16, 0x00,
	0x35, 0x17, 0x00,
	0x35, 0x18, 0x00,
	0x35, 0x19, 0x00,
	0x35, 0x1a, 0x00,
	0x35, 0x1b, 0x80,
	0x35, 0x1c, 0x00,
	0x35, 0x1d, 0x00,
	0x35, 0x1e, 0x00,
	0x35, 0x1f, 0x00,
	0x35, 0x20, 0x00,
	0x35, 0x21, 0x80,
	0x35, 0x22, 0x08,
	0x35, 0x24, 0x08,
	0x35, 0x26, 0x08,
	0x35, 0x28, 0x08,
	0x35, 0x2a, 0x08,
	0x36, 0x02, 0x00,
	0x36, 0x03, 0x40,
	0x36, 0x04, 0x02,
	0x36, 0x05, 0x00,
	0x36, 0x06, 0x00,
	0x36, 0x07, 0x00,
	0x36, 0x09, 0x12,
	0x36, 0x0a, 0x40,
	0x36, 0x0c, 0x08,
	0x36, 0x0f, 0xe5,
	0x36, 0x08, 0x8f,
	0x36, 0x11, 0x00,
	0x36, 0x13, 0xf7,
	0x36, 0x16, 0x58,
	0x36, 0x19, 0x99,
	0x36, 0x1b, 0x60,
	0x36, 0x1c, 0x7a,
	0x36, 0x1e, 0x79,
	0x36, 0x1f, 0x02,
	0x36, 0x32, 0x00,
	0x36, 0x33, 0x10,
	0x36, 0x34, 0x10,
	0x36, 0x35, 0x10,
	0x36, 0x36, 0x15,
	0x36, 0x46, 0x86,
	0x36, 0x4a, 0x0b,
	0x37, 0x00, 0x17,
	0x37, 0x01, 0x22,
	0x37, 0x03, 0x10,
	0x37, 0x0a, 0x37,
	0x37, 0x05, 0x00,
	0x37, 0x06, 0x63,
	0x37, 0x09, 0x3c,
	0x37, 0x0b, 0x01,
	0x37, 0x0c, 0x30,
	0x37, 0x10, 0x24,
	0x37, 0x11, 0x0c,
	0x37, 0x16, 0x00,
	0x37, 0x20, 0x28,
	0x37, 0x29, 0x7b,
	0x37, 0x2a, 0x84,
	0x37, 0x2b, 0xbd,
	0x37, 0x2c, 0xbc,
	0x37, 0x2e, 0x52,
	0x37, 0x3c, 0x0e,
	0x37, 0x3e, 0x33,
	0x37, 0x43, 0x10,
	0x37, 0x44, 0x88,
	0x37, 0x45, 0xc0,
	0x37, 0x4a, 0x43,
	0x37, 0x4c, 0x00,
	0x37, 0x4e, 0x23,
	0x37, 0x51, 0x7b,
	0x37, 0x52, 0x84,
	0x37, 0x53, 0xbd,
	0x37, 0x54, 0xbc,
	0x37, 0x56, 0x52,
	0x37, 0x5c, 0x00,
	0x37, 0x60, 0x00,
	0x37, 0x61, 0x00,
	0x37, 0x62, 0x00,
	0x37, 0x63, 0x00,
	0x37, 0x64, 0x00,
	0x37, 0x67, 0x04,
	0x37, 0x68, 0x04,
	0x37, 0x69, 0x08,
	0x37, 0x6a, 0x08,
	0x37, 0x6b, 0x20,
	0x37, 0x6c, 0x00,
	0x37, 0x6d, 0x00,
	0x37, 0x6e, 0x00,
	0x37, 0x73, 0x00,
	0x37, 0x74, 0x51,
	0x37, 0x76, 0xbd,
	0x37, 0x77, 0xbd,
	0x37, 0x81, 0x18,
	0x37, 0x83, 0x25,
	0x37, 0x98, 0x1b,
	0x38, 0x00, 0x01,
	0x38, 0x01, 0x88,
	0x38, 0x02, 0x00,
	0x38, 0x03, 0xdc,//e0
	0x38, 0x04, 0x09,
	0x38, 0x05, 0x17,
	0x38, 0x06, 0x05,
	0x38, 0x07, 0x23,//1f
	0x38, 0x08, 0x07,
	0x38, 0x09, 0x80,
	0x38, 0x0a, 0x04,
	0x38, 0x0b, 0x40,//38
	0x38, 0x0c, 0x06,//03
	0x38, 0x0d, 0xf3,//5c
	0x38, 0x0e, 0x04,
	0x38, 0x0f, 0x64,
	0x38, 0x10, 0x00,
	0x38, 0x11, 0x08,
	0x38, 0x12, 0x00,
	0x38, 0x13, 0x04,
	0x38, 0x14, 0x01,
	0x38, 0x15, 0x01,
	0x38, 0x19, 0x01,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x06,
	0x38, 0x29, 0x00,
	0x38, 0x2a, 0x01,
	0x38, 0x2b, 0x01,
	0x38, 0x2d, 0x7f,
	0x38, 0x30, 0x04,
	0x38, 0x36, 0x01,
	0x38, 0x37, 0x00,
	0x38, 0x41, 0x02,
	0x38, 0x46, 0x08,
	0x38, 0x47, 0x07,
	0x3d, 0x85, 0x36,
	0x3d, 0x8c, 0x71,
	0x3d, 0x8d, 0xcb,
	0x3f, 0x0a, 0x00,
	0x40, 0x00, 0xf1,
	0x40, 0x01, 0x40,
	0x40, 0x02, 0x04,
	0x40, 0x03, 0x14,
	0x40, 0x0e, 0x00,
	0x40, 0x11, 0x00,
	0x40, 0x1a, 0x00,
	0x40, 0x1b, 0x00,
	0x40, 0x1c, 0x00,
	0x40, 0x1d, 0x00,
	0x40, 0x1f, 0x00,
	0x40, 0x20, 0x00,
	0x40, 0x21, 0x10,
	0x40, 0x22, 0x06,
	0x40, 0x23, 0x13,
	0x40, 0x24, 0x07,
	0x40, 0x25, 0x40,
	0x40, 0x26, 0x07,
	0x40, 0x27, 0x50,
	0x40, 0x28, 0x00,
	0x40, 0x29, 0x02,
	0x40, 0x2a, 0x06,
	0x40, 0x2b, 0x04,
	0x40, 0x2c, 0x02,
	0x40, 0x2d, 0x02,
	0x40, 0x2e, 0x0e,
	0x40, 0x2f, 0x04,
	0x43, 0x02, 0xff,
	0x43, 0x03, 0xff,
	0x43, 0x04, 0x00,
	0x43, 0x05, 0x00,
	0x43, 0x06, 0x00,
	0x43, 0x08, 0x02,
	0x45, 0x00, 0x6c,
	0x45, 0x01, 0xc4,
	0x45, 0x02, 0x40,
	0x45, 0x03, 0x01,
	0x46, 0x01, 0x77,
	0x48, 0x00, 0x04,
	0x48, 0x13, 0x08,
	0x48, 0x1f, 0x40,
	0x48, 0x29, 0x78,
	0x48, 0x37, 0x1b,
	0x4b, 0x00, 0x2a,
	0x4b, 0x0d, 0x00,
	0x4d, 0x00, 0x04,
	0x4d, 0x01, 0x42,
	0x4d, 0x02, 0xd1,
	0x4d, 0x03, 0x93,
	0x4d, 0x04, 0xf5,
	0x4d, 0x05, 0xc1,
	0x50, 0x00, 0xf3,
	0x50, 0x01, 0x11,
	0x50, 0x04, 0x00,
	0x50, 0x0a, 0x00,
	0x50, 0x0b, 0x00,
	0x50, 0x32, 0x00,
	0x50, 0x40, 0x00,
	0x50, 0x50, 0x0c,
	0x55, 0x00, 0x00,
	0x55, 0x01, 0x10,
	0x55, 0x02, 0x01,
	0x55, 0x03, 0x0f,
	0x80, 0x00, 0x00,
	0x80, 0x01, 0x00,
	0x80, 0x02, 0x00,
	0x80, 0x03, 0x00,
	0x80, 0x04, 0x00,
	0x80, 0x05, 0x00,
	0x80, 0x06, 0x00,
	0x80, 0x07, 0x00,
	0x80, 0x08, 0x00,
	0x36, 0x38, 0x00,
	//0x01, 0x00, 0x01,
	STOP_REG, STOP_REG, STOP_REG,
};

// 4lanes 30fps
static IMG_UINT8 ui8ModeRegs_1920_1088_30_4lane[]=
{
	0x01,0x03,0x01,
	0x36,0x38,0x00,
	0x03,0x00,0x00,
	0x03,0x02,0x2a,
	0x03,0x03,/*0x01*/0x01, // ??? use 0x01 for test pll1_divdm 1/2 
	0x03,0x04,0x03,
	0x03,0x0a,0x00, // pll1 prediv
	0x03,0x0b,0x00,
	0x03,0x0d,0x1e,
	0x03,0x0e,0x04,
	0x03,0x0f,0x01,
	0x03,0x11,0x00, // pll2 prediv
	0x03,0x12,0x01,
	0x03,0x1e,0x00,
	0x30,0x00,0x20,
	0x30,0x02,0x00,
	0x30,0x18,0x72,
	0x30,0x20,0x93,
	0x30,0x21,0x03,
	0x30,0x22,0x01,
	0x30,0x31,0x0a,
	0x30,0x3f,0x0c,
	0x33,0x05,0xf1,
	0x33,0x07,0x04,
	0x33,0x09,0x29,
	0x35,0x00,0x00,
	0x35,0x01,0x4c,
	0x35,0x02,0x00,
	0x35,0x03,0x04,
	0x35,0x04,0x00,
	0x35,0x05,0x00,
	0x35,0x06,0x00,
	0x35,0x07,0x00,
	0x35,0x08,0x00/*0x01*/,
	0x35,0x09,0x80/*0xb8*/,
	0x35,0x0a,0x00,
	0x35,0x0b,0x00,
	0x35,0x0c,0x00,
	0x35,0x0d,0x00,
	0x35,0x0e,0x00,
	0x35,0x0f,0x80,
	0x35,0x10,0x00,
	0x35,0x11,0x00,
	0x35,0x12,0x00,
	0x35,0x13,0x00,
	0x35,0x14,0x00,
	0x35,0x15,0x80,
	0x35,0x16,0x00,
	0x35,0x17,0x00,
	0x35,0x18,0x00,
	0x35,0x19,0x00,
	0x35,0x1a,0x00,
	0x35,0x1b,0x80,
	0x35,0x1c,0x00,
	0x35,0x1d,0x00,
	0x35,0x1e,0x00,
	0x35,0x1f,0x00,
	0x35,0x20,0x00,
	0x35,0x21,0x80,
	0x35,0x22,0x08,
	0x35,0x24,0x08,
	0x35,0x26,0x08,
	0x35,0x28,0x08,
	0x35,0x2a,0x08,
	0x36,0x02,0x00,
	0x36,0x03,0x40,
	0x36,0x04,0x02,
	0x36,0x05,0x00,
	0x36,0x06,0x00,
	0x36,0x07,0x00,
	0x36,0x09,0x12,
	0x36,0x0a,0x40,
	0x36,0x0c,0x08,
	0x36,0x0f,0xe5,
	0x36,0x08,0x8f,
	0x36,0x11,0x00,
	0x36,0x13,0xf7,
	0x36,0x16,0x58,
	0x36,0x19,0x99,
	0x36,0x1b,0x60,
	0x36,0x1c,0x7a,
	0x36,0x1e,0x79,
	0x36,0x1f,0x02,
	0x36,0x32,0x00,
	0x36,0x33,0x10,
	0x36,0x34,0x10,
	0x36,0x35,0x10,
	0x36,0x36,0x15,
	0x36,0x46,0x86,
	0x36,0x4a,0x0b,
	0x37,0x00,0x17,
	0x37,0x01,0x22,
	0x37,0x03,0x10,
	0x37,0x0a,0x37,
	0x37,0x05,0x00,
	0x37,0x06,0x63,
	0x37,0x09,0x3c,
	0x37,0x0b,0x01,
	0x37,0x0c,0x30,
	0x37,0x10,0x24,
	0x37,0x11,0x0c,
	0x37,0x16,0x00,
	0x37,0x20,0x28,
	0x37,0x29,0x7b,
	0x37,0x2a,0x84,
	0x37,0x2b,0xbd,
	0x37,0x2c,0xbc,
	0x37,0x2e,0x52,
	0x37,0x3c,0x0e,
	0x37,0x3e,0x33,
	0x37,0x43,0x10,
	0x37,0x44,0x88,
	0x37,0x45,0xc0,
	0x37,0x4a,0x43,
	0x37,0x4c,0x00,
	0x37,0x4e,0x23,
	0x37,0x51,0x7b,
	0x37,0x52,0x84,
	0x37,0x53,0xbd,
	0x37,0x54,0xbc,
	0x37,0x56,0x52,
	0x37,0x5c,0x00,
	0x37,0x60,0x00,
	0x37,0x61,0x00,
	0x37,0x62,0x00,
	0x37,0x63,0x00,
	0x37,0x64,0x00,
	0x37,0x67,0x04,
	0x37,0x68,0x04,
	0x37,0x69,0x08,
	0x37,0x6a,0x08,
	0x37,0x6b,0x20,
	0x37,0x6c,0x00,
	0x37,0x6d,0x00,
	0x37,0x6e,0x00,
	0x37,0x73,0x00,
	0x37,0x74,0x51,
	0x37,0x76,0xbd,
	0x37,0x77,0xbd,
	0x37,0x81,0x18,
	0x37,0x83,0x25,
	0x37,0x98,0x1b,
	0x38,0x00,0x01,
	0x38,0x01,0x88,
	0x38,0x02,0x00,
	0x38,0x03,0xdc,//0xe0,
	0x38,0x04,0x09,
	0x38,0x05,0x17,
	0x38,0x06,0x05,
	0x38,0x07,0x23,//0x1f,
	0x38,0x08,0x07,
	0x38,0x09,0x80,
	0x38,0x0a,0x04,
	0x38,0x0b,0x40,//0x38,

	0x38,0x0c,0x0d, // 30fps
	0x38,0x0d,0xf0,

	0x38,0x0e,0x04,
	0x38,0x0f,0x64,//0x8A,
	0x38,0x10,0x00,
	0x38,0x11,0x08,
	0x38,0x12,0x00,
	0x38,0x13,0x04,
	0x38,0x14,0x01,
	0x38,0x15,0x01,
	0x38,0x19,0x01,
#ifndef OV683Dual_FLIP_MIRROR_FRIST
	0x38,0x20,0x00,
	0x38,0x21,0x06,
#else
	0x38,0x20,0x06,
	0x38,0x21,0x00,
#endif
	0x38,0x29,0x00,
	0x38,0x2a,0x01,
	0x38,0x2b,0x01,
	0x38,0x2d,0x7f,
	0x38,0x30,0x04,
	0x38,0x36,0x01,
	0x38,0x37,0x00,
	0x38,0x41,0x02,
	0x38,0x46,0x08,
	0x38,0x47,0x07,
	0x3d,0x85,0x36,
	0x3d,0x8c,0x71,
	0x3d,0x8d,0xcb,
	0x3f,0x0a,0x00,
	0x40,0x00,0xf1,
	0x40,0x01,0x40,
	0x40,0x02,0x04,
	0x40,0x03,0x14,
	0x40,0x0e,0x00,
	0x40,0x11,0x00,
	0x40,0x1a,0x00,
	0x40,0x1b,0x00,
	0x40,0x1c,0x00,
	0x40,0x1d,0x00,
	0x40,0x1f,0x00,
	0x40,0x20,0x00,
	0x40,0x21,0x10,
	0x40,0x22,0x06,
	0x40,0x23,0x13,
	0x40,0x24,0x07,
	0x40,0x25,0x40,
	0x40,0x26,0x07,
	0x40,0x27,0x50,
	0x40,0x28,0x00,
	0x40,0x29,0x02,
	0x40,0x2a,0x06,
	0x40,0x2b,0x04,
	0x40,0x2c,0x02,
	0x40,0x2d,0x02,
	0x40,0x2e,0x0e,
	0x40,0x2f,0x04,
	0x43,0x02,0xff,
	0x43,0x03,0xff,
	0x43,0x04,0x00,
	0x43,0x05,0x00,
	0x43,0x06,0x00,
	0x43,0x08,0x02,
	0x45,0x00,0x6c,
	0x45,0x01,0xc4,
	0x45,0x02,0x40,
	0x45,0x03,0x01,
	0x46,0x01,0x77,
	0x48,0x00,0x04, // timing
	0x48,0x13,0x08,
	0x48,0x1f,0x40,
	// HS-prepare//
	//0x48,0x26,0x28,// - 0x10, //HS_PREPARE_MIN
	//0x48,0x27,0x55,// + 0x10,     //HS_PREPARE_MAX
	//0x48,0x31,0x6C - 0x35, //UI_HS_PREPARE_MIN
	///////////////
	0x48,0x29,0x78,
	0x48,0x37,0x10,
	0x4b,0x00,0x2a,
	0x4b,0x0d,0x00,
	0x4d,0x00,0x04,
	0x4d,0x01,0x42,
	0x4d,0x02,0xd1,
	0x4d,0x03,0x93,
	0x4d,0x04,0xf5,
	0x4d,0x05,0xc1,
	0x50,0x00,0xf3,
	0x50,0x01,0x11,
	0x50,0x04,0x00,
	0x50,0x0a,0x00,
	0x50,0x0b,0x00,
	0x50,0x32,0x00,
	0x50,0x40,0x00,
	0x50,0x50,0x0c,
	0x55,0x00,0x00,
	0x55,0x01,0x10,
	0x55,0x02,0x01,
	0x55,0x03,0x0f,
	0x80,0x00,0x00,
	0x80,0x01,0x00,
	0x80,0x02,0x00,
	0x80,0x03,0x00,
	0x80,0x04,0x00,
	0x80,0x05,0x00,
	0x80,0x06,0x00,
	0x80,0x07,0x00,
	0x80,0x08,0x00,
	0x36,0x38,0x00,
//	0x01,0x00,0x01,
//	0x03,0x00,0x60,
	STOP_REG, STOP_REG, STOP_REG,
};

// 2lane 60fps
static IMG_UINT8 ui8ModeRegs_1920_1088_60_2lane[]=
{
	0x01, 0x03, 0x01,
	0x36, 0x38, 0x00,
	0x03, 0x00, 0x04,//0x02,
	0x03, 0x02, 0x40,//0x32,
	0x03, 0x03, 0x00,
	0x03, 0x04, 0x03,

	0x03, 0x05, 0x02,
	0x03, 0x06, 0x02,

	0x03, 0x0a, 0x00,  // pll1 prediv
	0x03, 0x0b, 0x00,
	0x03, 0x0c, 0x00,
	0x03, 0x0d, 0x1e,
	0x03, 0x0e, 0x04,
	0x03, 0x0f, 0x01,
	0x03, 0x11, 0x00,
	0x03, 0x12, 0x01,
	0x03, 0x1e, 0x00,
	0x30, 0x00, 0x20,
	0x30, 0x02, 0x00,
	0x30, 0x18, 0x32, //0x72,
	0x30, 0x20, 0x93,
	0x30, 0x21, 0x03,
	0x30, 0x22, 0x01,
	0x30, 0x31, 0x0a,
	0x30, 0x3f, 0x0c,
	0x33, 0x05, 0xf1,
	0x33, 0x07, 0x04,
	0x33, 0x09, 0x29,
	0x35, 0x00, 0x00,
	0x35, 0x01, 0x20,
	0x35, 0x02, 0x00,
	0x35, 0x03, 0x04,
	0x35, 0x04, 0x00,
	0x35, 0x05, 0x00,
	0x35, 0x06, 0x00,
	0x35, 0x07, 0x00,
	0x35, 0x08, 0x00,
	0x35, 0x09, 0x80,
	0x35, 0x0a, 0x00,
	0x35, 0x0b, 0x00,
	0x35, 0x0c, 0x00,
	0x35, 0x0d, 0x00,
	0x35, 0x0e, 0x00,
	0x35, 0x0f, 0x80,
	0x35, 0x10, 0x00,
	0x35, 0x11, 0x00,
	0x35, 0x12, 0x00,
	0x35, 0x13, 0x00,
	0x35, 0x14, 0x00,
	0x35, 0x15, 0x80,
	0x35, 0x16, 0x00,
	0x35, 0x17, 0x00,
	0x35, 0x18, 0x00,
	0x35, 0x19, 0x00,
	0x35, 0x1a, 0x00,
	0x35, 0x1b, 0x80,
	0x35, 0x1c, 0x00,
	0x35, 0x1d, 0x00,
	0x35, 0x1e, 0x00,
	0x35, 0x1f, 0x00,
	0x35, 0x20, 0x00,
	0x35, 0x21, 0x80,
	0x35, 0x22, 0x08,
	0x35, 0x24, 0x08,
	0x35, 0x26, 0x08,
	0x35, 0x28, 0x08,
	0x35, 0x2a, 0x08,
	0x36, 0x02, 0x00,
	0x36, 0x03, 0x40,
	0x36, 0x04, 0x02,
	0x36, 0x05, 0x00,
	0x36, 0x06, 0x00,
	0x36, 0x07, 0x00,
	0x36, 0x09, 0x12,
	0x36, 0x0a, 0x40,
	0x36, 0x0c, 0x08,
	0x36, 0x0f, 0xe5,
	0x36, 0x08, 0x8f,
	0x36, 0x11, 0x00,
	0x36, 0x13, 0xf7,
	0x36, 0x16, 0x58,
	0x36, 0x19, 0x99,
	0x36, 0x1b, 0x60,
	0x36, 0x1c, 0x7a,
	0x36, 0x1e, 0x79,
	0x36, 0x1f, 0x02,
	0x36, 0x32, 0x00,
	0x36, 0x33, 0x10,
	0x36, 0x34, 0x10,
	0x36, 0x35, 0x10,
	0x36, 0x36, 0x15,
	0x36, 0x46, 0x86,
	0x36, 0x4a, 0x0b,
	0x37, 0x00, 0x17,
	0x37, 0x01, 0x22,
	0x37, 0x03, 0x10,
	0x37, 0x0a, 0x37,
	0x37, 0x05, 0x00,
	0x37, 0x06, 0x63,
	0x37, 0x09, 0x3c,
	0x37, 0x0b, 0x01,
	0x37, 0x0c, 0x30,
	0x37, 0x10, 0x24,
	0x37, 0x11, 0x0c,
	0x37, 0x16, 0x00,
	0x37, 0x20, 0x28,
	0x37, 0x29, 0x7b,
	0x37, 0x2a, 0x84,
	0x37, 0x2b, 0xbd,
	0x37, 0x2c, 0xbc,
	0x37, 0x2e, 0x52,
	0x37, 0x3c, 0x0e,
	0x37, 0x3e, 0x33,
	0x37, 0x43, 0x10,
	0x37, 0x44, 0x88,
	0x37, 0x45, 0xc0,
	0x37, 0x4a, 0x43,
	0x37, 0x4c, 0x00,
	0x37, 0x4e, 0x23,
	0x37, 0x51, 0x7b,
	0x37, 0x52, 0x84,
	0x37, 0x53, 0xbd,
	0x37, 0x54, 0xbc,
	0x37, 0x56, 0x52,
	0x37, 0x5c, 0x00,
	0x37, 0x60, 0x00,
	0x37, 0x61, 0x00,
	0x37, 0x62, 0x00,
	0x37, 0x63, 0x00,
	0x37, 0x64, 0x00,
	0x37, 0x67, 0x04,
	0x37, 0x68, 0x04,
	0x37, 0x69, 0x08,
	0x37, 0x6a, 0x08,
	0x37, 0x6b, 0x20,
	0x37, 0x6c, 0x00,
	0x37, 0x6d, 0x00,
	0x37, 0x6e, 0x00,
	0x37, 0x73, 0x00,
	0x37, 0x74, 0x51,
	0x37, 0x76, 0xbd,
	0x37, 0x77, 0xbd,
	0x37, 0x81, 0x18,
	0x37, 0x83, 0x25,
	0x37, 0x98, 0x1b,
	0x38, 0x00, 0x01,
	0x38, 0x01, 0x88,
	0x38, 0x02, 0x00,
	0x38, 0x03, 0xdc,//e0
	0x38, 0x04, 0x09,
	0x38, 0x05, 0x17,
	0x38, 0x06, 0x05,
	0x38, 0x07, 0x23,//1f
	0x38, 0x08, 0x07,
	0x38, 0x09, 0x80,
	0x38, 0x0a, 0x04,
	0x38, 0x0b, 0x40,//38
	0x38, 0x0c, 0x06,//03
	0x38, 0x0d, 0xf3,//5c
	0x38, 0x0e, 0x04,
	0x38, 0x0f, 0x64,
	0x38, 0x10, 0x00,
	0x38, 0x11, 0x08,
	0x38, 0x12, 0x00,
	0x38, 0x13, 0x04,
	0x38, 0x14, 0x01,
	0x38, 0x15, 0x01,
	0x38, 0x19, 0x01,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x06,
	0x38, 0x29, 0x00,
	0x38, 0x2a, 0x01,
	0x38, 0x2b, 0x01,
	0x38, 0x2d, 0x7f,
	0x38, 0x30, 0x04,
	0x38, 0x36, 0x01,
	0x38, 0x37, 0x00,
	0x38, 0x41, 0x02,
	0x38, 0x46, 0x08,
	0x38, 0x47, 0x07,
	0x3d, 0x85, 0x36,
	0x3d, 0x8c, 0x71,
	0x3d, 0x8d, 0xcb,
	0x3f, 0x0a, 0x00,
	0x40, 0x00, 0xf1,
	0x40, 0x01, 0x40,
	0x40, 0x02, 0x04,
	0x40, 0x03, 0x14,
	0x40, 0x0e, 0x00,
	0x40, 0x11, 0x00,
	0x40, 0x1a, 0x00,
	0x40, 0x1b, 0x00,
	0x40, 0x1c, 0x00,
	0x40, 0x1d, 0x00,
	0x40, 0x1f, 0x00,
	0x40, 0x20, 0x00,
	0x40, 0x21, 0x10,
	0x40, 0x22, 0x06,
	0x40, 0x23, 0x13,
	0x40, 0x24, 0x07,
	0x40, 0x25, 0x40,
	0x40, 0x26, 0x07,
	0x40, 0x27, 0x50,
	0x40, 0x28, 0x00,
	0x40, 0x29, 0x02,
	0x40, 0x2a, 0x06,
	0x40, 0x2b, 0x04,
	0x40, 0x2c, 0x02,
	0x40, 0x2d, 0x02,
	0x40, 0x2e, 0x0e,
	0x40, 0x2f, 0x04,
	0x43, 0x02, 0xff,
	0x43, 0x03, 0xff,
	0x43, 0x04, 0x00,
	0x43, 0x05, 0x00,
	0x43, 0x06, 0x00,
	0x43, 0x08, 0x02,
	0x45, 0x00, 0x6c,
	0x45, 0x01, 0xc4,
	0x45, 0x02, 0x40,
	0x45, 0x03, 0x01,
	0x46, 0x01, 0x77,
	0x48, 0x00, 0x04,
	0x48, 0x13, 0x08,
	0x48, 0x1f, 0x40,
	0x48, 0x29, 0x78,
	0x48, 0x37, 0x1b,
	0x4b, 0x00, 0x2a,
	0x4b, 0x0d, 0x00,
	0x4d, 0x00, 0x04,
	0x4d, 0x01, 0x42,
	0x4d, 0x02, 0xd1,
	0x4d, 0x03, 0x93,
	0x4d, 0x04, 0xf5,
	0x4d, 0x05, 0xc1,
	0x50, 0x00, 0xf3,
	0x50, 0x01, 0x11,
	0x50, 0x04, 0x00,
	0x50, 0x0a, 0x00,
	0x50, 0x0b, 0x00,
	0x50, 0x32, 0x00,
	0x50, 0x40, 0x00,
	0x50, 0x50, 0x0c,
	0x55, 0x00, 0x00,
	0x55, 0x01, 0x10,
	0x55, 0x02, 0x01,
	0x55, 0x03, 0x0f,
	0x80, 0x00, 0x00,
	0x80, 0x01, 0x00,
	0x80, 0x02, 0x00,
	0x80, 0x03, 0x00,
	0x80, 0x04, 0x00,
	0x80, 0x05, 0x00,
	0x80, 0x06, 0x00,
	0x80, 0x07, 0x00,
	0x80, 0x08, 0x00,
	0x36, 0x38, 0x00,
	//0x01, 0x00, 0x01,
	STOP_REG, STOP_REG, STOP_REG,
};

// 2lane 30fps
static IMG_UINT8 ui8ModeRegs_1920_1088_30_2lane[]=
{
	0x01,0x03,0x01,
	0x36,0x38,0x00,
	0x03,0x00,0x00,
	0x03,0x02,0x2a, // 2_LANE
	0x03,0x03,0x00, // ??? use 0x01 for test pll1_divdm 1/2 
	0x03,0x04,0x03,
	0x03,0x0b,0x00,
	0x03,0x0d,0x1e,
	0x03,0x0e,0x04,
	0x03,0x0f,0x01,
	0x03,0x12,0x01,
	0x03,0x1e,0x00,
	0x30,0x00,0x20,
	0x30,0x02,0x00,
	0x30,0x18,0x32, // 2_LANE
	0x30,0x20,0x93,
	0x30,0x21,0x03,
	0x30,0x22,0x01,
	0x30,0x31,0x0a,
	0x30,0x3f,0x0c,
	0x33,0x05,0xf1,
	0x33,0x07,0x04,
	0x33,0x09,0x29,
	0x35,0x00,0x00,
	0x35,0x01,0x4c,
	0x35,0x02,0x00,
	0x35,0x03,0x04,
	0x35,0x04,0x00,
	0x35,0x05,0x00,
	0x35,0x06,0x00,
	0x35,0x07,0x00,
	0x35,0x08,0x00,
	0x35,0x09,0x80,
	0x35,0x0a,0x00,
	0x35,0x0b,0x00,
	0x35,0x0c,0x00,
	0x35,0x0d,0x00,
	0x35,0x0e,0x00,
	0x35,0x0f,0x80,
	0x35,0x10,0x00,
	0x35,0x11,0x00,
	0x35,0x12,0x00,
	0x35,0x13,0x00,
	0x35,0x14,0x00,
	0x35,0x15,0x80,
	0x35,0x16,0x00,
	0x35,0x17,0x00,
	0x35,0x18,0x00,
	0x35,0x19,0x00,
	0x35,0x1a,0x00,
	0x35,0x1b,0x80,
	0x35,0x1c,0x00,
	0x35,0x1d,0x00,
	0x35,0x1e,0x00,
	0x35,0x1f,0x00,
	0x35,0x20,0x00,
	0x35,0x21,0x80,
	0x35,0x22,0x08,
	0x35,0x24,0x08,
	0x35,0x26,0x08,
	0x35,0x28,0x08,
	0x35,0x2a,0x08,
	0x36,0x02,0x00,
	0x36,0x03,0x40,
	0x36,0x04,0x02,
	0x36,0x05,0x00,
	0x36,0x06,0x00,
	0x36,0x07,0x00,
	0x36,0x09,0x12,
	0x36,0x0a,0x40,
	0x36,0x0c,0x08,
	0x36,0x0f,0xe5,
	0x36,0x08,0x8f,
	0x36,0x11,0x00,
	0x36,0x13,0xf7,
	0x36,0x16,0x58,
	0x36,0x19,0x99,
	0x36,0x1b,0x60,
	0x36,0x1c,0x7a,
	0x36,0x1e,0x79,
	0x36,0x1f,0x02,
	0x36,0x32,0x00,
	0x36,0x33,0x10,
	0x36,0x34,0x10,
	0x36,0x35,0x10,
	0x36,0x36,0x15,
	0x36,0x46,0x86,
	0x36,0x4a,0x0b,
	0x37,0x00,0x17,
	0x37,0x01,0x22,
	0x37,0x03,0x10,
	0x37,0x0a,0x37,
	0x37,0x05,0x00,
	0x37,0x06,0x63,
	0x37,0x09,0x3c,
	0x37,0x0b,0x01,
	0x37,0x0c,0x30,
	0x37,0x10,0x24,
	0x37,0x11,0x0c,
	0x37,0x16,0x00,
	0x37,0x20,0x28,
	0x37,0x29,0x7b,
	0x37,0x2a,0x84,
	0x37,0x2b,0xbd,
	0x37,0x2c,0xbc,
	0x37,0x2e,0x52,
	0x37,0x3c,0x0e,
	0x37,0x3e,0x33,
	0x37,0x43,0x10,
	0x37,0x44,0x88,
	0x37,0x45,0xc0,
	0x37,0x4a,0x43,
	0x37,0x4c,0x00,
	0x37,0x4e,0x23,
	0x37,0x51,0x7b,
	0x37,0x52,0x84,
	0x37,0x53,0xbd,
	0x37,0x54,0xbc,
	0x37,0x56,0x52,
	0x37,0x5c,0x00,
	0x37,0x60,0x00,
	0x37,0x61,0x00,
	0x37,0x62,0x00,
	0x37,0x63,0x00,
	0x37,0x64,0x00,
	0x37,0x67,0x04,
	0x37,0x68,0x04,
	0x37,0x69,0x08,
	0x37,0x6a,0x08,
	0x37,0x6b,0x20,
	0x37,0x6c,0x00,
	0x37,0x6d,0x00,
	0x37,0x6e,0x00,
	0x37,0x73,0x00,
	0x37,0x74,0x51,
	0x37,0x76,0xbd,
	0x37,0x77,0xbd,
	0x37,0x81,0x18,
	0x37,0x83,0x25,
	0x37,0x98,0x1b,
	0x38,0x00,0x01,
	0x38,0x01,0x88,
	0x38,0x02,0x00,
	0x38,0x03,0xdc,//0xe0,
	0x38,0x04,0x09,
	0x38,0x05,0x17,
	0x38,0x06,0x05,
	0x38,0x07,0x23,//0x1f,
	0x38,0x08,0x07,
	0x38,0x09,0x80,
	0x38,0x0a,0x04,
	0x38,0x0b,0x40,//0x38,
	///////////////
	0x38,0x0c,0x0c, // 30fps
	0x38,0x0d,0xb4,
	///////////////
	0x38,0x0e,0x04,
	0x38,0x0f,0x64,//0x8A,
	0x38,0x10,0x00,
	0x38,0x11,0x08,
	0x38,0x12,0x00,
	0x38,0x13,0x04,
	0x38,0x14,0x01,
	0x38,0x15,0x01,
	0x38,0x19,0x01,
#ifndef OV683Dual_FLIP_MIRROR_FRIST
	0x38,0x20,0x00,
	0x38,0x21,0x06,
#else
	0x38,0x20,0x06,
	0x38,0x21,0x00,
#endif

	0x38,0x29,0x00,
	0x38,0x2a,0x01,
	0x38,0x2b,0x01,
	0x38,0x2d,0x7f,
	0x38,0x30,0x04,
	0x38,0x36,0x01,
	0x38,0x37,0x00,
	0x38,0x41,0x02,
	0x38,0x46,0x08,
	0x38,0x47,0x07,
	0x3d,0x85,0x36,
	0x3d,0x8c,0x71,
	0x3d,0x8d,0xcb,
	0x3f,0x0a,0x00,
	0x40,0x00,0xf1,
	0x40,0x01,0x40,
	0x40,0x02,0x04,
	0x40,0x03,0x14,
	0x40,0x0e,0x00,
	0x40,0x11,0x00,
	0x40,0x1a,0x00,
	0x40,0x1b,0x00,
	0x40,0x1c,0x00,
	0x40,0x1d,0x00,
	0x40,0x1f,0x00,
	0x40,0x20,0x00,
	0x40,0x21,0x10,
	0x40,0x22,0x06,
	0x40,0x23,0x13,
	0x40,0x24,0x07,
	0x40,0x25,0x40,
	0x40,0x26,0x07,
	0x40,0x27,0x50,
	0x40,0x28,0x00,
	0x40,0x29,0x02,
	0x40,0x2a,0x06,
	0x40,0x2b,0x04,
	0x40,0x2c,0x02,
	0x40,0x2d,0x02,
	0x40,0x2e,0x0e,
	0x40,0x2f,0x04,
	0x43,0x02,0xff,
	0x43,0x03,0xff,
	0x43,0x04,0x00,
	0x43,0x05,0x00,
	0x43,0x06,0x00,
	0x43,0x08,0x02,
	0x45,0x00,0x6c,
	0x45,0x01,0xc4,
	0x45,0x02,0x40,
	0x45,0x03,0x01,
	0x46,0x01,0x77,
	0x48,0x00,0x04, // timing
	0x48,0x13,0x08,
	0x48,0x1f,0x40,
	// HS-prepare//
	//0x48,0x26,0x28,// - 0x10, //HS_PREPARE_MIN
	//0x48,0x27,0x55,// + 0x10,     //HS_PREPARE_MAX
	//0x48,0x31,0x6C - 0x35, //UI_HS_PREPARE_MIN
	///////////////
	0x48,0x29,0x78,
	0x48,0x37,0x10,
	0x4b,0x00,0x2a,
	0x4b,0x0d,0x00,
	0x4d,0x00,0x04,
	0x4d,0x01,0x42,
	0x4d,0x02,0xd1,
	0x4d,0x03,0x93,
	0x4d,0x04,0xf5,
	0x4d,0x05,0xc1,
	0x50,0x00,0xf3,
	0x50,0x01,0x11,
	0x50,0x04,0x00,
	0x50,0x0a,0x00,
	0x50,0x0b,0x00,
	0x50,0x32,0x00,
	0x50,0x40,0x00,
	0x50,0x50,0x0c,
	0x55,0x00,0x00,
	0x55,0x01,0x10,
	0x55,0x02,0x01,
	0x55,0x03,0x0f,
	0x80,0x00,0x00,
	0x80,0x01,0x00,
	0x80,0x02,0x00,
	0x80,0x03,0x00,
	0x80,0x04,0x00,
	0x80,0x05,0x00,
	0x80,0x06,0x00,
	0x80,0x07,0x00,
	0x80,0x08,0x00,
	0x36,0x38,0x00,
	//0x01, 0x00, 0x01,
	//0x03, 0x00, 0x60,
	STOP_REG, STOP_REG, STOP_REG,
};

// 2lane 20fps
static IMG_UINT8 ui8ModeRegs_1920_1088_20_2lane[]=
{
	0x01,0x03,0x01,
	0x36,0x38,0x00,
	0x03,0x00,0x00,
	0x03,0x02,0x2a, // 2_LANE
	0x03,0x03,0x00, // ??? use 0x01 for test pll1_divdm 1/2 
	0x03,0x04,0x03,
	0x03,0x0b,0x00,
	0x03,0x0d,0x1e,
	0x03,0x0e,0x04,
	0x03,0x0f,0x01,
	0x03,0x12,0x01,
	0x03,0x1e,0x00,
	0x30,0x00,0x20,
	0x30,0x02,0x00,
	0x30,0x18,0x32, // 2_LANE
	0x30,0x20,0x93,
	0x30,0x21,0x03,
	0x30,0x22,0x01,
	0x30,0x31,0x0a,
	0x30,0x3f,0x0c,
	0x33,0x05,0xf1,
	0x33,0x07,0x04,
	0x33,0x09,0x29,
	0x35,0x00,0x00,
	0x35,0x01,0x4c,
	0x35,0x02,0x00,
	0x35,0x03,0x04,
	0x35,0x04,0x00,
	0x35,0x05,0x00,
	0x35,0x06,0x00,
	0x35,0x07,0x00,
	0x35,0x08,0x00,
	0x35,0x09,0x80,
	0x35,0x0a,0x00,
	0x35,0x0b,0x00,
	0x35,0x0c,0x00,
	0x35,0x0d,0x00,
	0x35,0x0e,0x00,
	0x35,0x0f,0x80,
	0x35,0x10,0x00,
	0x35,0x11,0x00,
	0x35,0x12,0x00,
	0x35,0x13,0x00,
	0x35,0x14,0x00,
	0x35,0x15,0x80,
	0x35,0x16,0x00,
	0x35,0x17,0x00,
	0x35,0x18,0x00,
	0x35,0x19,0x00,
	0x35,0x1a,0x00,
	0x35,0x1b,0x80,
	0x35,0x1c,0x00,
	0x35,0x1d,0x00,
	0x35,0x1e,0x00,
	0x35,0x1f,0x00,
	0x35,0x20,0x00,
	0x35,0x21,0x80,
	0x35,0x22,0x08,
	0x35,0x24,0x08,
	0x35,0x26,0x08,
	0x35,0x28,0x08,
	0x35,0x2a,0x08,
	0x36,0x02,0x00,
	0x36,0x03,0x40,
	0x36,0x04,0x02,
	0x36,0x05,0x00,
	0x36,0x06,0x00,
	0x36,0x07,0x00,
	0x36,0x09,0x12,
	0x36,0x0a,0x40,
	0x36,0x0c,0x08,
	0x36,0x0f,0xe5,
	0x36,0x08,0x8f,
	0x36,0x11,0x00,
	0x36,0x13,0xf7,
	0x36,0x16,0x58,
	0x36,0x19,0x99,
	0x36,0x1b,0x60,
	0x36,0x1c,0x7a,
	0x36,0x1e,0x79,
	0x36,0x1f,0x02,
	0x36,0x32,0x00,
	0x36,0x33,0x10,
	0x36,0x34,0x10,
	0x36,0x35,0x10,
	0x36,0x36,0x15,
	0x36,0x46,0x86,
	0x36,0x4a,0x0b,
	0x37,0x00,0x17,
	0x37,0x01,0x22,
	0x37,0x03,0x10,
	0x37,0x0a,0x37,
	0x37,0x05,0x00,
	0x37,0x06,0x63,
	0x37,0x09,0x3c,
	0x37,0x0b,0x01,
	0x37,0x0c,0x30,
	0x37,0x10,0x24,
	0x37,0x11,0x0c,
	0x37,0x16,0x00,
	0x37,0x20,0x28,
	0x37,0x29,0x7b,
	0x37,0x2a,0x84,
	0x37,0x2b,0xbd,
	0x37,0x2c,0xbc,
	0x37,0x2e,0x52,
	0x37,0x3c,0x0e,
	0x37,0x3e,0x33,
	0x37,0x43,0x10,
	0x37,0x44,0x88,
	0x37,0x45,0xc0,
	0x37,0x4a,0x43,
	0x37,0x4c,0x00,
	0x37,0x4e,0x23,
	0x37,0x51,0x7b,
	0x37,0x52,0x84,
	0x37,0x53,0xbd,
	0x37,0x54,0xbc,
	0x37,0x56,0x52,
	0x37,0x5c,0x00,
	0x37,0x60,0x00,
	0x37,0x61,0x00,
	0x37,0x62,0x00,
	0x37,0x63,0x00,
	0x37,0x64,0x00,
	0x37,0x67,0x04,
	0x37,0x68,0x04,
	0x37,0x69,0x08,
	0x37,0x6a,0x08,
	0x37,0x6b,0x20,
	0x37,0x6c,0x00,
	0x37,0x6d,0x00,
	0x37,0x6e,0x00,
	0x37,0x73,0x00,
	0x37,0x74,0x51,
	0x37,0x76,0xbd,
	0x37,0x77,0xbd,
	0x37,0x81,0x18,
	0x37,0x83,0x25,
	0x37,0x98,0x1b,
	0x38,0x00,0x01,
	0x38,0x01,0x88,
	0x38,0x02,0x00,
	0x38,0x03,0xdc,//0xe0,
	0x38,0x04,0x09,
	0x38,0x05,0x17,
	0x38,0x06,0x05,
	0x38,0x07,0x23,//0x1f,
	0x38,0x08,0x07,
	0x38,0x09,0x80,
	0x38,0x0a,0x04,
	0x38,0x0b,0x40,//0x38,
	///////////////
	0x38,0x0c,0x11, // 20fps
	0x38,0x0d,0x00,
	///////////////
	0x38,0x0e,0x04,
	0x38,0x0f,0x64,//0x8A,
	0x38,0x10,0x00,
	0x38,0x11,0x08,
	0x38,0x12,0x00,
	0x38,0x13,0x04,
	0x38,0x14,0x01,
	0x38,0x15,0x01,
	0x38,0x19,0x01,
#ifndef OV683Dual_FLIP_MIRROR_FRIST
	0x38,0x20,0x00,
	0x38,0x21,0x06,
#else
	0x38,0x20,0x06,
	0x38,0x21,0x00,
#endif
	0x38,0x29,0x00,
	0x38,0x2a,0x01,
	0x38,0x2b,0x01,
	0x38,0x2d,0x7f,
	0x38,0x30,0x04,
	0x38,0x36,0x01,
	0x38,0x37,0x00,
	0x38,0x41,0x02,
	0x38,0x46,0x08,
	0x38,0x47,0x07,
	0x3d,0x85,0x36,
	0x3d,0x8c,0x71,
	0x3d,0x8d,0xcb,
	0x3f,0x0a,0x00,
	0x40,0x00,0xf1,
	0x40,0x01,0x40,
	0x40,0x02,0x04,
	0x40,0x03,0x14,
	0x40,0x0e,0x00,
	0x40,0x11,0x00,
	0x40,0x1a,0x00,
	0x40,0x1b,0x00,
	0x40,0x1c,0x00,
	0x40,0x1d,0x00,
	0x40,0x1f,0x00,
	0x40,0x20,0x00,
	0x40,0x21,0x10,
	0x40,0x22,0x06,
	0x40,0x23,0x13,
	0x40,0x24,0x07,
	0x40,0x25,0x40,
	0x40,0x26,0x07,
	0x40,0x27,0x50,
	0x40,0x28,0x00,
	0x40,0x29,0x02,
	0x40,0x2a,0x06,
	0x40,0x2b,0x04,
	0x40,0x2c,0x02,
	0x40,0x2d,0x02,
	0x40,0x2e,0x0e,
	0x40,0x2f,0x04,
	0x43,0x02,0xff,
	0x43,0x03,0xff,
	0x43,0x04,0x00,
	0x43,0x05,0x00,
	0x43,0x06,0x00,
	0x43,0x08,0x02,
	0x45,0x00,0x6c,
	0x45,0x01,0xc4,
	0x45,0x02,0x40,
	0x45,0x03,0x01,
	0x46,0x01,0x77,
	0x48,0x00,0x04, // timing
	0x48,0x13,0x08,
	0x48,0x1f,0x40,
	// HS-prepare//
	//0x48,0x26,0x28,// - 0x10, //HS_PREPARE_MIN
	//0x48,0x27,0x55,// + 0x10,     //HS_PREPARE_MAX
	//0x48,0x31,0x6C - 0x35, //UI_HS_PREPARE_MIN
	///////////////
	0x48,0x29,0x78,
	0x48,0x37,0x10,
	0x4b,0x00,0x2a,
	0x4b,0x0d,0x00,
	0x4d,0x00,0x04,
	0x4d,0x01,0x42,
	0x4d,0x02,0xd1,
	0x4d,0x03,0x93,
	0x4d,0x04,0xf5,
	0x4d,0x05,0xc1,
	0x50,0x00,0xf3,
	0x50,0x01,0x11,
	0x50,0x04,0x00,
	0x50,0x0a,0x00,
	0x50,0x0b,0x00,
	0x50,0x32,0x00,
	0x50,0x40,0x00,
	0x50,0x50,0x0c,
	0x55,0x00,0x00,
	0x55,0x01,0x10,
	0x55,0x02,0x01,
	0x55,0x03,0x0f,
	0x80,0x00,0x00,
	0x80,0x01,0x00,
	0x80,0x02,0x00,
	0x80,0x03,0x00,
	0x80,0x04,0x00,
	0x80,0x05,0x00,
	0x80,0x06,0x00,
	0x80,0x07,0x00,
	0x80,0x08,0x00,
	0x36,0x38,0x00,
	//0x01, 0x00, 0x01,
	//0x03, 0x00, 0x60,
	STOP_REG, STOP_REG, STOP_REG,
};

// 1lane 30fps
static IMG_UINT8 ui8ModeRegs_1920_1088_30_1lane[]=
{
	0x01, 0x03, 0x01,
	0x36, 0x38, 0x00,
	0x03, 0x00, 0x00,
	0x03, 0x02, 0x2a,  // 1_LANE
	0x03, 0x03, 0x00,  // ??? use 0x01 for test pll1_divdm 1/2
	0x03, 0x04, 0x03,
	0x03, 0x0b, 0x00,
	0x03, 0x0d, 0x1e,
	0x03, 0x0e, 0x04,
	0x03, 0x0f, 0x01,
	0x03, 0x11, 0x00,  // pll2 prediv
	0x03, 0x12, 0x01,
	0x03, 0x1e, 0x00,
	0x30, 0x00, 0x20,
	0x30, 0x02, 0x00,
	0x30, 0x18, 0x12,  // 1_LANE
	0x30, 0x20, 0x93,
	0x30, 0x21, 0x03,
	0x30, 0x22, 0x01,
	0x30, 0x31, 0x0a,
	0x30, 0x3f, 0x0c,
	0x33, 0x05, 0xf1,
	0x33, 0x07, 0x04,
	0x33, 0x09, 0x29,
	0x35, 0x00, 0x00,
	0x35, 0x01, 0x4c,
	0x35, 0x02, 0x00,
	0x35, 0x03, 0x04,
	0x35, 0x04, 0x00,
	0x35, 0x05, 0x00,
	0x35, 0x06, 0x00,
	0x35, 0x07, 0x00,
	0x35, 0x08, 0x00,
	0x35, 0x09, 0x80,
	0x35, 0x0a, 0x00,
	0x35, 0x0b, 0x00,
	0x35, 0x0c, 0x00,
	0x35, 0x0d, 0x00,
	0x35, 0x0e, 0x00,
	0x35, 0x0f, 0x80,
	0x35, 0x10, 0x00,
	0x35, 0x11, 0x00,
	0x35, 0x12, 0x00,
	0x35, 0x13, 0x00,
	0x35, 0x14, 0x00,
	0x35, 0x15, 0x80,
	0x35, 0x16, 0x00,
	0x35, 0x17, 0x00,
	0x35, 0x18, 0x00,
	0x35, 0x19, 0x00,
	0x35, 0x1a, 0x00,
	0x35, 0x1b, 0x80,
	0x35, 0x1c, 0x00,
	0x35, 0x1d, 0x00,
	0x35, 0x1e, 0x00,
	0x35, 0x1f, 0x00,
	0x35, 0x20, 0x00,
	0x35, 0x21, 0x80,
	0x35, 0x22, 0x08,
	0x35, 0x24, 0x08,
	0x35, 0x26, 0x08,
	0x35, 0x28, 0x08,
	0x35, 0x2a, 0x08,
	0x36, 0x02, 0x00,
	0x36, 0x03, 0x40,
	0x36, 0x04, 0x02,
	0x36, 0x05, 0x00,
	0x36, 0x06, 0x00,
	0x36, 0x07, 0x00,
	0x36, 0x09, 0x12,
	0x36, 0x0a, 0x40,
	0x36, 0x0c, 0x08,
	0x36, 0x0f, 0xe5,
	0x36, 0x08, 0x8f,
	0x36, 0x11, 0x00,
	0x36, 0x13, 0xf7,
	0x36, 0x16, 0x58,
	0x36, 0x19, 0x99,
	0x36, 0x1b, 0x60,
	0x36, 0x1c, 0x7a,
	0x36, 0x1e, 0x79,
	0x36, 0x1f, 0x02,
	0x36, 0x32, 0x00,
	0x36, 0x33, 0x10,
	0x36, 0x34, 0x10,
	0x36, 0x35, 0x10,
	0x36, 0x36, 0x15,
	0x36, 0x46, 0x86,
	0x36, 0x4a, 0x0b,
	0x37, 0x00, 0x17,
	0x37, 0x01, 0x22,
	0x37, 0x03, 0x10,
	0x37, 0x0a, 0x37,
	0x37, 0x05, 0x00,
	0x37, 0x06, 0x63,
	0x37, 0x09, 0x3c,
	0x37, 0x0b, 0x01,
	0x37, 0x0c, 0x30,
	0x37, 0x10, 0x24,
	0x37, 0x11, 0x0c,
	0x37, 0x16, 0x00,
	0x37, 0x20, 0x28,
	0x37, 0x29, 0x7b,
	0x37, 0x2a, 0x84,
	0x37, 0x2b, 0xbd,
	0x37, 0x2c, 0xbc,
	0x37, 0x2e, 0x52,
	0x37, 0x3c, 0x0e,
	0x37, 0x3e, 0x33,
	0x37, 0x43, 0x10,
	0x37, 0x44, 0x88,
	0x37, 0x45, 0xc0,
	0x37, 0x4a, 0x43,
	0x37, 0x4c, 0x00,
	0x37, 0x4e, 0x23,
	0x37, 0x51, 0x7b,
	0x37, 0x52, 0x84,
	0x37, 0x53, 0xbd,
	0x37, 0x54, 0xbc,
	0x37, 0x56, 0x52,
	0x37, 0x5c, 0x00,
	0x37, 0x60, 0x00,
	0x37, 0x61, 0x00,
	0x37, 0x62, 0x00,
	0x37, 0x63, 0x00,
	0x37, 0x64, 0x00,
	0x37, 0x67, 0x04,
	0x37, 0x68, 0x04,
	0x37, 0x69, 0x08,
	0x37, 0x6a, 0x08,
	0x37, 0x6b, 0x20,
	0x37, 0x6c, 0x00,
	0x37, 0x6d, 0x00,
	0x37, 0x6e, 0x00,
	0x37, 0x73, 0x00,
	0x37, 0x74, 0x51,
	0x37, 0x76, 0xbd,
	0x37, 0x77, 0xbd,
	0x37, 0x81, 0x18,
	0x37, 0x83, 0x25,
	0x37, 0x98, 0x1b,
	0x38, 0x00, 0x01,
	0x38, 0x01, 0x88,
	0x38, 0x02, 0x00,
	0x38, 0x03, 0xdc, //0xe0,
	0x38, 0x04, 0x09,
	0x38, 0x05, 0x17,
	0x38, 0x06, 0x05,
	0x38, 0x07, 0x23, //0x1f,
	0x38, 0x08, 0x07,
	0x38, 0x09, 0x80,
	0x38, 0x0a, 0x04,
	0x38, 0x0b, 0x40, //0x38,
	///////////////
	0x38, 0x0c, 0x0c,  // 30fps
	0x38, 0x0d, 0xb4,
	///////////////
	0x38, 0x0e, 0x04,
	0x38, 0x0f, 0x64, //0x8A,
	0x38, 0x10, 0x00,
	0x38, 0x11, 0x08,
	0x38, 0x12, 0x00,
	0x38, 0x13, 0x04,
	0x38, 0x14, 0x01,
	0x38, 0x15, 0x01,
	0x38, 0x19, 0x01,
#ifndef OV683Dual_FLIP_MIRROR_FRIST
	0x38,0x20,0x00,
	0x38,0x21,0x06,
#else
	0x38,0x20,0x06,
	0x38,0x21,0x00,
#endif
	0x38, 0x29, 0x00,
	0x38, 0x2a, 0x01,
	0x38, 0x2b, 0x01,
	0x38, 0x2d, 0x7f,
	0x38, 0x30, 0x04,
	0x38, 0x36, 0x01,
	0x38, 0x37, 0x00,
	0x38, 0x41, 0x02,
	0x38, 0x46, 0x08,
	0x38, 0x47, 0x07,
	0x3d, 0x85, 0x36,
	0x3d, 0x8c, 0x71,
	0x3d, 0x8d, 0xcb,
	0x3f, 0x0a, 0x00,
	0x40, 0x00, 0xf1,
	0x40, 0x01, 0x40,
	0x40, 0x02, 0x04,
	0x40, 0x03, 0x14,
	0x40, 0x0e, 0x00,
	0x40, 0x11, 0x00,
	0x40, 0x1a, 0x00,
	0x40, 0x1b, 0x00,
	0x40, 0x1c, 0x00,
	0x40, 0x1d, 0x00,
	0x40, 0x1f, 0x00,
	0x40, 0x20, 0x00,
	0x40, 0x21, 0x10,
	0x40, 0x22, 0x06,
	0x40, 0x23, 0x13,
	0x40, 0x24, 0x07,
	0x40, 0x25, 0x40,
	0x40, 0x26, 0x07,
	0x40, 0x27, 0x50,
	0x40, 0x28, 0x00,
	0x40, 0x29, 0x02,
	0x40, 0x2a, 0x06,
	0x40, 0x2b, 0x04,
	0x40, 0x2c, 0x02,
	0x40, 0x2d, 0x02,
	0x40, 0x2e, 0x0e,
	0x40, 0x2f, 0x04,
	0x43, 0x02, 0xff,
	0x43, 0x03, 0xff,
	0x43, 0x04, 0x00,
	0x43, 0x05, 0x00,
	0x43, 0x06, 0x00,
	0x43, 0x08, 0x02,
	0x45, 0x00, 0x6c,
	0x45, 0x01, 0xc4,
	0x45, 0x02, 0x40,
	0x45, 0x03, 0x01,
	0x46, 0x01, 0x77,
	0x48, 0x00, 0x04,  // timing
	0x48, 0x13, 0x08,
	0x48, 0x1f, 0x40,
	// HS-prepare//
	//0x48, 0x26, 0x28, // - 0x10,  //HS_PREPARE_MIN
	//0x48, 0x27, 0x55, // + 0x10,      //HS_PREPARE_MAX
	//0x48, 0x31, 0x6C - 0x35,  //UI_HS_PREPARE_MIN
	///////////////
	0x48, 0x29, 0x78,
	0x48, 0x37, 0x10,
	0x4b, 0x00, 0x2a,
	0x4b, 0x0d, 0x00,
	0x4d, 0x00, 0x04,
	0x4d, 0x01, 0x42,
	0x4d, 0x02, 0xd1,
	0x4d, 0x03, 0x93,
	0x4d, 0x04, 0xf5,
	0x4d, 0x05, 0xc1,
	0x50, 0x00, 0xf3,
	0x50, 0x01, 0x11,
	0x50, 0x04, 0x00,
	0x50, 0x0a, 0x00,
	0x50, 0x0b, 0x00,
	0x50, 0x32, 0x00,
	0x50, 0x40, 0x00,
	0x50, 0x50, 0x0c,
	0x55, 0x00, 0x00,
	0x55, 0x01, 0x10,
	0x55, 0x02, 0x01,
	0x55, 0x03, 0x0f,
	0x80, 0x00, 0x00,
	0x80, 0x01, 0x00,
	0x80, 0x02, 0x00,
	0x80, 0x03, 0x00,
	0x80, 0x04, 0x00,
	0x80, 0x05, 0x00,
	0x80, 0x06, 0x00,
	0x80, 0x07, 0x00,
	0x80, 0x08, 0x00,
	0x36, 0x38, 0x00,
	//0x01, 0x00, 0x01,
	//0x03, 0x00, 0x60,
	STOP_REG, STOP_REG, STOP_REG,
};

struct ov683Dual_mode {
    IMG_UINT8 ui8Flipping;
    const IMG_UINT8 *modeRegisters;
    IMG_UINT32 nRegisters; // initially 0 then computed the 1st time
};

struct ov683Dual_mode ov683Dual_modes[] = 
{
    { SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1088_60_4lane, 0 }, //0
    { SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1088_30_4lane, 0 }, //1
    { SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1088_60_2lane, 0 }, //2
    { SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1088_30_2lane, 0 }, //3
	{ SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1088_20_2lane, 0 }, //4
    { SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1088_30_1lane, 0 }, //5
};

static const IMG_UINT8* OV683Dual_GetRegisters(OV683DualCAM_STRUCT *psCam,
    unsigned int uiMode, unsigned int *nRegisters)
{
    if (uiMode < sizeof(ov683Dual_modes) / sizeof(struct ov683Dual_mode))
    {
        const IMG_UINT8 *registerArray = ov683Dual_modes[uiMode].modeRegisters;
        if (0 == ov683Dual_modes[uiMode].nRegisters)
        {
            // should be done only once per mode
            int i = 0;
            while (STOP_REG != registerArray[3 * i])
            {
                i++;
            }
            ov683Dual_modes[uiMode].nRegisters = i;
        }
        *nRegisters = ov683Dual_modes[uiMode].nRegisters;
        
        return registerArray;
    }
    // if it is an invalid mode returns NULL

    return NULL;
}

static IMG_RESULT ov683Dual_i2c1_write8(int i2c, const IMG_UINT8 *data,
    unsigned int len, char slave_addr)
{
#if !file_i2c
    unsigned i;

    /* Every write sequence needs to have 3 elements:
     * 1) slave address high bits [15:8]
     * 2) slave address low bits [7:0]
     * 3) data
     */
    if (len % 3)
    {
        LOG_ERROR("Wrong len of data array, len = %d", len);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE_FORCE, slave_addr))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }

    for (i = 0; i < len; data += 3, i += 3)
    {
        int write_len = 0;

        if(DELAY_REG == data[0])
        {
            usleep((int)data[1]*1000);
            continue;
        }

        write_len = write(i2c, data, 3);

        if (write_len != 3)
        {
            LOG_ERROR("Failed to write I2C data! write_len = %d, index = %d\n", write_len, i);
            return IMG_ERROR_BUSY;
        }
    }
#endif //file_i2c
    return IMG_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
#ifdef I2C_NO_SWITCH
static IMG_RESULT ov683Dual_i2c_sensor_write8(int i2c, const IMG_UINT8 *data, unsigned int len, char slave_addr)
{
#if !file_i2c
    unsigned i, j;
    IMG_UINT8 write_data[24] = {
	0x15, 0x00, 0xe5,
	0x15, 0x01, 0x04,
	0x15, 0x02, 0x10,
	0x15, 0x10, 0x00, // write sccb0/1
	0x15, 0x11, 0x00, // write reg addr H
	0x15, 0x12, 0x00, // write reg addr L
	0x15, 0x13, 0x00, // write reg val
	0x15, 0x0f,  0x80, 
    };

    if (len % 3)
    {
        LOG_ERROR("Wrong len of data array, len = %d", len);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Set I2C slave address */
    if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, slave_addr))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }

    for (j = 0; j < SENSOR_SLAVE_NUM; j++)
    {
	    for (i = 0; i < len; data += 3, i += 3)
	    {
		int write_len = 0;

		if(DELAY_REG == data[0])
		{
		    usleep((int)data[1]*1000);
		    continue;
		}

		write_data[11] = (i2c_slave_addr[j]<<1);
		write_data[14] = data[0];
		write_data[17] = data[1];
		write_data[20] = data[2];

    int k = 0;
    for (k = 0; k < sizeof(write_data); k += 3)
    {
    	printf("<%x, %x, %x>\n", write_data[k], write_data[k+1], write_data[k+2]);
    }

		write_len = write(i2c, write_data, sizeof(write_data));

		if (write_len != sizeof(write_data))
		{
		    LOG_ERROR("Failed to write I2C data! write_len = %d, index = %d\n", write_len, i);
		    return IMG_ERROR_BUSY;
		}
	    }
   }
#endif //file_i2c
    return IMG_SUCCESS;
}

static IMG_RESULT ov683Dual_i2c_sensor_write8_by_addr(int i2c, char i2c_addr, const IMG_UINT8 *data, unsigned int len)
{
#if !file_i2c
    unsigned i, j;
    IMG_UINT8 write_data[24] = {
	0x15, 0x00, 0xe5,
	0x15, 0x01, 0x04,
	0x15, 0x02, 0x10,
	0x15, 0x10, 0x00, // write sccb0/1
	0x15, 0x11, 0x00, // write reg addr H
	0x15, 0x12, 0x00, // write reg addr L
	0x15, 0x13, 0x00, // write reg val
	0x15, 0x0f,  0x80, 
    };

    if (len % 3)
    {
        LOG_ERROR("Wrong len of data array, len = %d", len);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Set I2C slave address */
    if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, I2C_OV683_ADDR))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }

    for (i = 0; i < len; data += 3, i += 3)
    {
	int write_len = 0;

	if(DELAY_REG == data[0])
	{
	    usleep((int)data[1]*1000);
	    continue;
	}

	write_data[11] = (i2c_addr<<1);
	write_data[14] = data[0];
	write_data[17] = data[1];
	write_data[20] = data[2];

        int k = 0;
        for (k = 0; k < sizeof(write_data); k += 3)
        {
    	    printf("<%x, %x, %x>\n", write_data[k], write_data[k+1], write_data[k+2]);
        }

	write_len = write(i2c, write_data, sizeof(write_data));

	if (write_len != sizeof(write_data))
	{
	    LOG_ERROR("Failed to write I2C data! write_len = %d, index = %d\n", write_len, i);
	    return IMG_ERROR_BUSY;
	}
    }
#endif //file_i2c
    return IMG_SUCCESS;
}

static IMG_RESULT ov683Dual_i2c_sensor_read8(int i2c, char i2c_addr, IMG_UINT16 offset, IMG_UINT8 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2] = {0x15, 0x14};
     IMG_UINT8 write_data[21] = {
	0x15, 0x00, 0xe4,
	0x15, 0x01, 0x04,
	0x15, 0x02, 0x10,
	0x15, 0x10, 0x00, // write sccb0/1
	0x15, 0x11, 0x00, // write reg addr H
	0x15, 0x12, 0x00, // write reg addr L
	0x15, 0x0f,  0x80, 
    };

    IMG_ASSERT(data);  // null pointer forbidden

    /* Set I2C slave address */
    if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, I2C_OV683_ADDR))
    {
        LOG_ERROR("Failed to write I2C read address!\n");
		//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
        return IMG_ERROR_BUSY;
    }
    
    write_data[11] = (i2c_addr<<1);
    write_data[14] = (offset >> 8) & 0xFF;
    write_data[17] = offset & 0xFF;

    ret = write(i2c, write_data, sizeof(write_data));
    if (ret != sizeof(write_data))
    {
        LOG_WARNING("Wrote i2c error!\n");
    }

    ret = write(i2c, buff, sizeof(buff));
    if (ret != sizeof(buff))
    {
        LOG_WARNING("Wrote %dB instead of %luB before reading\n", ret, sizeof(buff));
    }

    ret = read(i2c, data, 1);
    if (1 != ret)
    {
        LOG_ERROR("Failed to read I2C at 0x%x\n", offset);
		//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
        return IMG_ERROR_FATAL;
    }
	
	//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);	
    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif //file_i2c
}
#endif
///////////////////////////////////////////////////////////////////////////

typedef enum __I2cChannelType_
{
	I2C_SENSOR_CHL,		
	I2C_OV683_CHL,
	I2C_OV683_1_CHL
}I2cChannelType;

static IMG_RESULT ov683Dual_i2c_switch(int i2c, I2cChannelType i2c_chl)
{
	IMG_UINT8 aui8Regs[3] = {0x02, 0x0b, 0x09};

#ifdef I2C_NO_SWITCH
	return IMG_SUCCESS;
#endif	
	if (i2c_chl == I2C_SENSOR_CHL)
	{// switch to sensor i2c channel
		return ov683Dual_i2c1_write8(i2c, aui8Regs, 3, I2C_OV683_ADDR);
	}
	else if (i2c_chl == I2C_OV683_CHL)
	{// switch to ov683 i2c channel
		aui8Regs[2] = 0x08;
		return ov683Dual_i2c1_write8(i2c, aui8Regs, 3, I2C_OV683_ADDR);	
	}
	else
	{
		printf("==>unknown i2c chl type\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	return IMG_SUCCESS;
}


static int xx = 0;
#define SKIP_CNT 0
#if 0
static IMG_RESULT ov683Dual_i2c_write8(int i2c, const IMG_UINT8 *data, unsigned int len)
{
    unsigned int i, j, index = 0;
	
    struct i2c_rdwr_ioctl_data packets; 
    struct i2c_msg messages[1];

	static int running_flag = 0;

	while (running_flag == 1)
	{
		usleep(5*1000);
	}
	running_flag = 1;
	

	for (j = 0; j < SENSOR_SLAVE_NUM; j++)
	{
		index = 0;
		for (i = 0; i < len; i += 3, index++)
		{
	        if(DELAY_REG == data[i])
	        {
	        	index--;
	            usleep((int)data[1]*1000);
	            continue;
	        }
			
		    messages[0].addr  = i2c_slave_addr[j];
		    messages[0].flags = 0;  
		    messages[0].len   = 3;  
		    messages[0].buf   = (data + i);

			packets.msgs  = messages;  
			packets.nmsgs = 1;
				
			if(ioctl(i2c, I2C_RDWR, &packets) < 0)
			{  
				LOG_ERROR("ov683Dual_i2c_write8: Unable to write data to sensor 0\n");  
				running_flag = 0;
				return 1;  
			}						
		}
	}

	running_flag = 0;
	return IMG_SUCCESS;
	
}
#else
static IMG_RESULT ov683Dual_i2c_write8(int i2c, const IMG_UINT8 *data, unsigned int len)
{
#if !file_i2c
    unsigned int i = 0, j = 0;
    IMG_UINT8 ret_data = 0;
    IMG_UINT8 buff[2];
    const IMG_UINT8 *ori_data_ptr =data ;

    static int running_flag = 0;
    int write_len = 0;

#ifdef I2C_NO_SWITCH
	return ov683Dual_i2c_sensor_write8(i2c, data, len, I2C_OV683_ADDR);
#endif

    /* Every write sequence needs to have 3 elements:
     * 1) slave address high bits [15:8]
     * 2) slave address low bits [7:0]
     * 3) data
     */
    //return IMG_SUCCESS;
    if (len % 3)
    {
        LOG_ERROR("Wrong len of data array, len = %d", len);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
	
	if (xx < SKIP_CNT)
	{
		xx++;
		return IMG_SUCCESS;
	}

	while (running_flag == 1)
	{
		usleep(200);
		printf("waiting ov683Dual_i2c_write8\n");
		//usleep(5*1000);
	}
	running_flag = 1;
	
	//ov683Dual_i2c_switch(i2c, I2C_SENSOR_CHL);
	
	//for (j = 0; j < SENSOR_SLAVE_NUM; j++)
	{
		data = ori_data_ptr;
	    /* Set I2C slave address */
	    for (i = 0; i < len; data += 3, i += 3)
	    {
	        if(DELAY_REG == data[0])
	        {
	            usleep((int)data[1]*1000);
	            continue;
	        }
			
		for (j = 0; j < SENSOR_SLAVE_NUM; j++)
		{
//again:
		    if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, i2c_slave_addr[j]))
	        {
	            LOG_ERROR("Failed to write I2C slave address %x!\n", i2c_slave_addr[j]);
                //ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
                running_flag = 0;	
	            return IMG_ERROR_BUSY;
	        }
		   
	        write_len = write(i2c, data, 3);

	        if (write_len != 3)
	        {
	            LOG_ERROR("Failed to write I2C(%x) data! write_len = %d, index = %d\n", i2c_slave_addr[j], write_len, i);
				//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
           	    running_flag = 0;	
	            return IMG_ERROR_BUSY;
	        }
			//////////////////////////////////
#if 0
					if (ioctl(i2c, I2C_SLAVE_FORCE,  i2c_slave_addr[j]/*(i2c_slave_addr[j]<<1 + 1)>>1*/))
					{
						LOG_ERROR("Failed to write I2C read address!\n");
						running_flag = 0;	
						return IMG_ERROR_BUSY;
					}
			
					buff[0] =data[0];
					buff[1] = data[1];
			
					write_len = write(i2c,	buff,  sizeof(buff));
					if (write_len != sizeof(buff))
					{
						LOG_ERROR("Wrote %dB instead of %luB before reading\n", write_len, sizeof(buff));
					}
			
					write_len = read(i2c, &ret_data, 1);
					if (write_len != 1)
					{
						printf("==>read [%02x%02x ] failed\n", data[0], data[1]);
					}
					if (ret_data != data[2])
					{
						printf("==>sensor %d : [%02x%02x ]ret_data(%02x) != data[2](%02x)\n", j, data[0], data[1], ret_data, data[2]);		
						goto again;
					}
					//printf("==>okokokokokokok[%02x, %02x, %02x]\n", data[0], data[1], data[2]);
#endif
			//////////////////////////////////

        }
		//printf("==>write [%02x%02x, %02x] ok\n", data[0], data[1], data[2]);

	    }
	}
	//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
	
#endif //file_i2c
    running_flag = 0;
	//i2c_slave_addr[0] = i2c_slave_addr[0]^i2c_slave_addr[1];
	//i2c_slave_addr[1] = i2c_slave_addr[0]^i2c_slave_addr[1];
	//i2c_slave_addr[0] = i2c_slave_addr[0]^i2c_slave_addr[1];
    return IMG_SUCCESS;
}
#endif

static IMG_RESULT ov683Dual_i2c_write8_by_addr(int i2c, char i2c_addr, const IMG_UINT8 *data, unsigned int len)
{
#if !file_i2c
    unsigned i;
    IMG_UINT8 ret_data = 0;
    IMG_UINT8 buff[2];

    /* Every write sequence needs to have 3 elements:
     * 1) slave address high bits [15:8]
     * 2) slave address low bits [7:0]
     * 3) data
     */
    //return IMG_SUCCESS;
    if (len % 3)
    {
        LOG_ERROR("Wrong len of data array, len = %d", len);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
#ifdef I2C_NO_SWITCH
	return ov683Dual_i2c_sensor_write8_by_addr(i2c, i2c_addr, data, len);
#endif
	if (xx < SKIP_CNT)
	{
		xx++;
		return IMG_SUCCESS;
	}
	
	//ov683Dual_i2c_switch(i2c, I2C_SENSOR_CHL);
	
    /* Set I2C slave address */
	if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave address %x!\n", i2c_addr);
		//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
        return IMG_ERROR_BUSY;
    }

    for (i = 0; i < len; data += 3, i += 3)
    {
        int write_len = 0;

        if(DELAY_REG == data[0])
        {
//	            usleep((int)data[1]*1000);
            continue;
        }
//again:
        write_len = write(i2c, data, 3);

        if (write_len != 3)
        {
            LOG_ERROR("Failed to write I2C(%x) data! write_len = %d, index = %d\n", i2c_addr, write_len, i);
			//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
            return IMG_ERROR_BUSY;
        }

	//printf("==>write [%02x%02x, %02x] ok\n", data[0], data[1], data[2]);

//////////////////////////////////
#if 0
	if (ioctl(i2c, I2C_SLAVE_FORCE, i2c_addr))
	{
		LOG_ERROR("Failed to write I2C read address!\n");
		return IMG_ERROR_BUSY;
	}

	buff[0] =data[0];
	buff[1] = data[1];

	write_len = write(i2c,  buff,  sizeof(buff));
	if (write_len != sizeof(buff))
	{
		LOG_ERROR("Wrote %dB instead of %luB before reading\n", write_len, sizeof(buff));
	}

	write_len = read(i2c, &ret_data, 1);
	if (write_len != 1)
	{
		printf("==>read [%02x%02x ] failed\n", data[0], data[1]);
	}
	if (ret_data != data[2])
        {
		printf("==>[%02x%02x ]ret_data(%02x) != data[2](%02x)\n", data[0], data[1], ret_data, data[2]);		
		goto again;
        }
	//printf("==>okokokokokokok[%02x, %02x, %02x]\n", data[0], data[1], data[2]);
#endif
//////////////////////////////////
    }
	
	//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
#endif //file_i2c
    return IMG_SUCCESS;
}


static IMG_RESULT ov683Dual_i2c_read8(int i2c, char i2c_addr, IMG_UINT16 offset,
    IMG_UINT8 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
#ifdef I2C_NO_SWITCH 
    return ov683Dual_i2c_sensor_read8(i2c, i2c_addr, offset, data);
#endif
    //return IMG_SUCCESS;
	
	//ov683Dual_i2c_switch(i2c, I2C_SENSOR_CHL);
	
    /* Set I2C slave address */
	if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C read address!\n");
		//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
        return IMG_ERROR_BUSY;
    }
    
    buff[0] = (offset >> 8) & 0xFF;
    buff[1] = offset & 0xFF;
    
    ret = write(i2c, buff, sizeof(buff));
    if (ret != sizeof(buff))
    {
        LOG_WARNING("Wrote %dB instead of %luB before reading\n",
            ret, sizeof(buff));
    }
    
    ret = read(i2c, data, 1);
    if (1 != ret)
    {
        LOG_ERROR("Failed to read I2C at 0x%x\n", offset);
		//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
        return IMG_ERROR_FATAL;
    }
	
	//ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);	
    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif //file_i2c
}


static IMG_RESULT ov683Dual_i2c_read16(int i2c, char addr, IMG_UINT16 offset,
    IMG_UINT16 *data)
{
#if !file_i2c
    int ret = IMG_SUCCESS;
    IMG_UINT8 buff[2];

		
    IMG_ASSERT(data);  // null pointer forbidden
#if 0
	if (addr == I2C_SENSOR_CHL)
	{
		ov683Dual_i2c_switch(i2c, I2C_SENSOR_CHL);
	}
	else
	{
		ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
	}
#endif
    /* Set I2C slave address */
	if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, addr))
    {
        LOG_ERROR("Failed to write I2C read address!\n");
        ret = IMG_ERROR_BUSY;
		goto fail;
    }
    
    buff[0] = (offset >> 8) & 0xFF;
    buff[1] = offset & 0xFF;
    
    ret = write(i2c, buff, sizeof(buff));
    if (ret != sizeof(buff))
    {
        LOG_WARNING("Wrote %dB instead of %luB before reading\n",
            ret, sizeof(buff));
    }
    
    ret = read(i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_ERROR("Failed to read I2C at 0x%x\n", offset);
        ret = IMG_ERROR_FATAL;
		goto fail;
    }

    *data = (buff[0] << 8) | buff[1];

fail:
#if 0
	if (addr == I2C_SENSOR_CHL)
	{
		ov683Dual_i2c_switch(i2c, I2C_OV683_CHL);
	}
	else
	{
		ov683Dual_i2c_switch(i2c, I2C_SENSOR_CHL);
	}
#endif
    return ret;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif //file_i2c
}

static IMG_RESULT OV683Dual_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
	IMG_RESULT ret;
	IMG_UINT16 v = 0;
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_ASSERT(strlen(OV683Dual_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(OV683Dual_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);

    psInfo->eBayerOriginal = MOSAIC_BGGR;//MOSAIC_RGGB;
    psInfo->eBayerEnabled = psInfo->eBayerOriginal;
    // assumes that when flipping changes the bayer pattern
    /*if (SENSOR_FLIP_NONE != psInfo->sStatus.ui8Flipping)
    {
        psInfo->eBayerEnabled = MosaicFlip(psInfo->eBayerOriginal,
            0, //psInfo->sStatus.ui8Flipping&SENSOR_FLIP_HORIZONTAL?1:0,
            psInfo->sStatus.ui8Flipping&SENSOR_FLIP_VERTICAL?1:0);
        // bayer format is not affected by flipping since we change
        // registers to 6 instead of 2
    }*/
    sprintf(psInfo->pszSensorName, OV683Dual_SENSOR_INFO_NAME);
    
#ifndef NO_DEV_CHECK
    ret = ov683Dual_i2c_read16(psCam->i2c, i2c_slave_addr[0], REG_SC_CMMN_CHIP_ID_0, &v);
#else
    ret = 1;
#endif //NO_DEV_CHECK
    if (!ret)
    {
        sprintf(psInfo->pszSensorVersion, "0x%x", v);
    }
    else
    {
        sprintf(psInfo->pszSensorVersion, OV683Dual_SENSOR_VERSION);
    }

    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 6040;
    // bitdepth is a mode information
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = 0;
    psInfo->bBackFacing = IMG_TRUE;
    // other information should be filled by Sensor_GetInfo()
#ifdef INFOTM_ISP
    psInfo->ui32ModeCount = sizeof(ov683Dual_modes) / sizeof(struct ov683Dual_mode);
#endif //INFOTM_ISP

    LOG_DEBUG("provided BayerOriginal=%d\n", psInfo->eBayerOriginal);
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    /*if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    ret = OV683Dual_GetModeInfo(psCam, nIndex, psModes);
    return ret;
}

static IMG_RESULT OV683Dual_GetState(SENSOR_HANDLE hHandle,
        SENSOR_STATUS *psStatus)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_WARNING("sensor not initialised\n");
        psStatus->eState = SENSOR_STATE_UNINITIALISED;
        psStatus->ui16CurrentMode = 0;
    }
    else
    {
        psStatus->eState= (psCam->bEnabled?
            SENSOR_STATE_RUNNING:SENSOR_STATE_IDLE);
        psStatus->ui16CurrentMode=psCam->ui16CurrentMode;
        psStatus->ui8Flipping=psCam->ui8CurrentFlipping;
#ifdef INFOTM_ISP
        psStatus->fCurrentFps = psCam->fCurrentFps;
#endif

    }
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
          IMG_UINT8 ui8Flipping)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = NULL;
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    modeReg = OV683Dual_GetRegisters(psCam, nMode, &nRegisters);

    if (modeReg)
    {
        if((ui8Flipping&(ov683Dual_modes[nMode].ui8Flipping))!=ui8Flipping)
        {
            LOG_ERROR("sensor mode does not support selected flipping "\
                "0x%x (supports 0x%x)\n",
                ui8Flipping, ov683Dual_modes[nMode].ui8Flipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }

#if defined(OV683Dual_COPY_REGS)
        if (psCam->currModeReg)
        {
            IMG_FREE(psCam->currModeReg);
        }
#endif //OV683Dual_COPY_REGS
        psCam->nRegisters = nRegisters;
        
#if defined(OV683Dual_COPY_REGS)
        /* we want to apply all the registers at once - need to copy them */
        psCam->currModeReg = (IMG_UINT8*)IMG_MALLOC(psCam->nRegisters * 3);
        IMG_MEMCPY(psCam->currModeReg, modeReg, psCam->nRegisters * 3);
#else
        /* no need to copy the register as we will apply them one by one */
        psCam->currModeReg = (IMG_UINT8*)modeReg;
#endif //OV683Dual_COPY_REGS

        ret = OV683Dual_GetModeInfo(psCam, nMode, &(psCam->currMode));
        if (ret)
        {
            LOG_ERROR("failed to get mode %d initial information!\n", nMode);
            return IMG_ERROR_FATAL;
        }

#ifdef DO_SETUP_IN_ENABLE
        /* because we don't do the actual setup we do not have real
         * information about the sensor - fake it */
        psCam->ui32Exposure = psCam->currMode.ui32ExposureMin * 302;

        psCam->ui32Exposure = IMG_MIN_INT(psCam->currMode.ui32ExposureMax, 
            IMG_MAX_INT(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin));
#else
        OV683Dual_ApplyMode(psCam);
#endif //DO_SETUP_IN_ENABLE

        psCam->ui16CurrentFocus = ov683Dual_focus_dac_min; // minimum focus
        psCam->ui16CurrentMode = nMode;
        psCam->ui8CurrentFlipping = ui8Flipping;
        return IMG_SUCCESS;
    }


    return IMG_ERROR_INVALID_PARAMETERS;
}

static char ov683Dual_regs[] = {
	0x38,0x00, 0x02,
	0x38,0x01, 0x78,
	0x38,0x02, 0x00,
	0x38,0x03, 0x2C,
	0x38,0x04, 0x08,
	0x38,0x05, 0x27,
	0x38,0x06, 0x05,
	0x38,0x07, 0xD3,
	0x38,0x08, 0x02,
	0x38,0x09, 0xD0,
	0x38,0x0A, 0x02,
	0x38,0x0B, 0xD0,
	0x38,0x0C, 0x03,
	0x38,0x0D, 0x5C,
	0x38,0x0E, 0x02,
	0x38,0x0F, 0xF1,
	0x38,0x10, 0x00,
	0x38,0x11, 0x04,
	0x38,0x12, 0x00,
	0x38,0x13, 0x02,
	0x40,0x20, 0x00,
	0x40,0x21, 0x10,
	0x40,0x22, 0x01,
	0x40,0x23, 0x63,
	0x40,0x24, 0x02,
	0x40,0x25, 0x90,
	0x40,0x26, 0x02,
	0x40,0x27, 0xA0,
	0x46,0x00, 0x00,
	0x46,0x01, 0x2C,
	0x0,
};

static IMG_RESULT OV683Dual_Enable(SENSOR_HANDLE hHandle)
{
	IMG_UINT16 chipV = 0;
	IMG_UINT8 aui8Regs[3];
	IMG_UINT32 i = 0;
    IMG_UINT8 aui18EnableRegs[]=
    {
#ifndef DO_SETUP_IN_ENABLE
        //set sensor to streaming mode
        0x01, 0x00, 0x01, // SC_CTRL0100 streaming
        0x30, 0x1A, 0xF9, // SC_CMMN_CLKRST0 as recommended by omnivision
        DELAY_REG, 15, DELAY_REG,// 15 ms
        0x30, 0x1A, 0xF1, // SC_CMMN_CLKRST0
        0x48, 0x05, 0x00, // MIPI_CTRL_05 
        0x30, 0x1A, 0xF0, // SC_CMMN_CLKRST0
#endif //DO_SETUP_IN_ENABLE
        //0x32, 0x0b, 0x0, // GRP_SWCTRL select group 0
        //0x32, 0x08, 0xe0 // GROUP_ACCESS quick launch group 0

		0x01, 0x00, 0x01 // streaming enable
    };
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling OV683Dual Camera!\n");
    psCam->bEnabled=IMG_TRUE;

    psCam->psSensorPhy->psGasket->bParallel = IMG_FALSE;
    psCam->psSensorPhy->psGasket->uiGasket = psCam->imager;

    //printf("~~~psCam->currMode.ui8MipiLanes=%d\n", psCam->currMode.ui8MipiLanes);

    SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->currMode.ui8MipiLanes,
        OV683Dual_PHY_NUM);

#ifdef DO_SETUP_IN_ENABLE
    // because we didn't do the actual setup in setMode we have to do it now
    // it may start the sensor so we have to do it after the gasket enable
    /// @ fix conversion from float to uint
    OV683Dual_ApplyMode(psCam);
#endif //DO_SETUP_IN_ENABLE

    LOG_DEBUG("camera enabled\n");
//	system("echo 1 > /sys/class/ddk_sensor_debug/enable");
	usleep(30*1000);
try_again:
	ov683Dual_i2c_switch(psCam->i2c, I2C_OV683_CHL);
	aui8Regs[0] = 0x6f;
	aui8Regs[1] = 0x01;
	aui8Regs[2] = 0x90;
	while (ov683Dual_i2c1_write8(psCam->i2c, aui8Regs, 3, I2C_OV683_ADDR) != IMG_SUCCESS)
	{
		usleep(10*1000);
		goto try_again;
	}
try_again_1:
	while (	ov683Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL) != IMG_SUCCESS)
	{
		usleep(10*1000);
		goto try_again_1;
	}

    ov683Dual_i2c_write8(psCam->i2c, aui18EnableRegs, sizeof(aui18EnableRegs));
    //usleep(200*1000);
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_Disable(SENSOR_HANDLE hHandle)
{
    IMG_UINT8 aui8Regs[3];
    static const IMG_UINT8 disableRegs[]=
	{
	#if 0
        0x01, 0x00, 0x00, // SC_CTRL0100 set to sw standby
        DELAY_REG, 200, DELAY_REG, // 200ms delay
        0x30, 0x1a, 0xf9, // SC_CMMN_CLKRST0 as recommended by omnivision
        0x48, 0x05, 0x03, // MIPI_CTRL_05 retime manu/sel to manual
//        DELAY_REG, 10, DELAY_REG // 10ms delay
	#else
		0x01, 0x00, 0x00
	#endif
    };

    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(psCam->bEnabled)
    {
        int delay = 0;
        LOG_INFO("Disabling OV683Dual camera\n");
        psCam->bEnabled = IMG_FALSE;

        ov683Dual_i2c_write8(psCam->i2c, disableRegs, sizeof(disableRegs));

        // delay of a frame period to ensure sensor has stopped
        // flFrameRate in Hz, change to MHz to have micro seconds
        delay = (int)floor((1.0 / psCam->currMode.flFrameRate) * 1000 * 1000);
        LOG_DEBUG("delay of %uus between disabling sensor/phy\n", delay);
        
        usleep(delay);

try_again:
	ov683Dual_i2c_switch(psCam->i2c, I2C_OV683_CHL);
	aui8Regs[0] = 0x6f;
	aui8Regs[1] = 0x01;
	aui8Regs[2] = 0x00;
	while (ov683Dual_i2c1_write8(psCam->i2c, aui8Regs, 3, I2C_OV683_ADDR) != IMG_SUCCESS)
	{
		usleep(10*1000);
		goto try_again;
	}

        SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, OV683Dual_PHY_NUM);
    }

    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_Destroy(SENSOR_HANDLE hHandle)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_DEBUG("Destroying OV683Dual camera\n");
    /* remember to free anything that might have been allocated dynamically
     * (like extended params...)*/
    if(psCam->bEnabled)
    {
        OV683Dual_Disable(hHandle);
    }
    SensorPhyDeinit(psCam->psSensorPhy);
    if (psCam->currModeReg)
    {
#if defined(OV683Dual_COPY_REGS)
        IMG_FREE(psCam->currModeReg);
#endif //OV683Dual_COPY_REGS
        psCam->currModeReg = NULL;
    }
#if !file_i2c
    close(psCam->i2c);
#endif //file_i2c
    IMG_FREE(psCam);
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetFocusRange(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Min, IMG_UINT16 *pui16Max)
{
    /*OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    *pui16Min = ov683Dual_focus_dac_min;
    *pui16Max = ov683Dual_focus_dac_max;
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetCurrentFocus(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Current)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui16Current=psCam->ui16CurrentFocus;
    return IMG_SUCCESS;
}

/*
 * calculate focus position from points and return DAC value
 */
IMG_UINT16 OV683Dual_CalculateFocus(const IMG_UINT16 *pui16DACDist,
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
IMG_RESULT OV683Dual_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus)
{
#if !file_i2c
    unsigned i;
#endif //file_i2c
    IMG_UINT8 ui8Regs[4];
    IMG_UINT16 ui16DACVal;

	//return IMG_SUCCESS;

    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui16CurrentFocus = ui16Focus;
    if (psCam->ui16CurrentFocus >= ov683Dual_focus_dac_max)
    {
        // special case the infinity as it doesn't fit in with the rest
        ui16DACVal = 0;
    }
    else
    {
        ui16DACVal = OV683Dual_CalculateFocus(ov683Dual_focus_dac_dist,
            ov683Dual_focus_dac_val,
            sizeof(ov683Dual_focus_dac_dist) / sizeof(IMG_UINT16),
            ui16Focus);
    }

    ui8Regs[0] = 4;
    ui8Regs[1] = ui16DACVal >> 8;
    ui8Regs[2] = 5;
    ui8Regs[3] = ui16DACVal & 0xff;

#if !file_i2c
    if (ioctl(psCam->i2c, I2C_SLAVE, AUTOFOCUS_WRITE_ADDR))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }

    for (i = 0; i < sizeof(ui8Regs); i += 2)
    {
        IMG_UINT8 *data = ui8Regs + i;
        if (write(psCam->i2c, data, 2) != 2)
        {
            LOG_ERROR("Failed to write I2C data!\n");
            return IMG_ERROR_BUSY;
        }
    }
#endif //file_i2c

    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetExposureRange(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui32Min=psCam->currMode.ui32ExposureMin;
    *pui32Max=psCam->currMode.ui32ExposureMax;
    *pui8Contexts=1;
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current=psCam->ui32Exposure;
    return IMG_SUCCESS;
}

void OV683Dual_ComputeExposure(IMG_UINT32 ui32Exposure,
    IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureRegs)
{
    IMG_UINT32 exposureRatio = (ui32Exposure)/ui32ExposureMin;

    LOG_DEBUG("Exposure Val 0x%x\n", exposureRatio);

    exposureRegs[2]=((exposureRatio)>>12) & 0xf;
    exposureRegs[5]=((exposureRatio)>>4) & 0xff;
    exposureRegs[8]=((exposureRatio)<<4) & 0xff;
}

static IMG_RESULT OV683Dual_SetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
    IMG_UINT8 exposureRegs[] =
    {
//        0x32, 0x08, 0x00, // GROUP_ACCESS start hold of group 0
        
        0x35, 0x00, 0x00, // exposure register
        0x35, 0x01, 0x2E, //
        0x35, 0x02, 0x80, //
        
//        0x32, 0x08, 0x10, // end hold of group 0 register writes
//        0x32, 0x0b, 0x00, // set quick manual mode
//        0x32, 0x08, 0xe0, // quick launch group 0
    };

    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    printf("===>OV683Dual_SetExposure\n");

    psCam->ui32Exposure = ui32Current;

#if 1
	IMG_UINT32 ui32Lines;

	ui32Lines = psCam->ui32Exposure / psCam->currMode.dbExposureMin;

//	if (ui32Lines > psCam->currMode.ui16VerticalTotal - 4)
//		ui32Lines = psCam->currMode.ui16VerticalTotal - 4;

	exposureRegs[5-3] = (ui32Lines >> 12) & 0xf;
	exposureRegs[8-3] = (ui32Lines >> 4) & 0xff;
	exposureRegs[11-3] = (ui32Lines << 4) & 0xff;
#else
	
	exposureRegs[5-3] = (((psCam->ui32Exposure)/psCam->currMode.ui32ExposureMin)>>12) & 0xf;
	exposureRegs[8-3] = (((psCam->ui32Exposure)/psCam->currMode.ui32ExposureMin)>>4) & 0xff;
	exposureRegs[11-3] = (((psCam->ui32Exposure)/psCam->currMode.ui32ExposureMin)<<4) & 0xff;
#endif
	LOG_DEBUG("Exposure Val %x\n",(psCam->ui32Exposure/psCam->currMode.ui32ExposureMin));

//    if (psCam->bEnabled || 1)
//    {
//        OV683Dual_ComputeExposure(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin,
//            &(exposureRegs[3]));

//        ov683Dual_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));
//    }

	ov683Dual_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));

    return IMG_SUCCESS;
}

static IMG_UINT16 asGainSettings[] =
{
	0x0080,
	0x0088,
	0x0090,
	0x0098,
	0x00A0,
	0x00A8,
	0x00B0,
	0x00B8,
	0x00C0,
	0x00C8,
	0x00D0,
	0x00D8,
	0x00E0,
	0x00E8,
	0x00F0,
	0x00F8,
	0x0178,
	0x017C,
	0x0180,
	0x0184,
	0x0188,
	0x018C,
	0x0190,
	0x0194,
	0x0198,
	0x019C,
	0x01A0,
	0x01A4,
	0x01A8,
	0x01AC,
	0x01B0,
	0x01B4,
	0x01B8,
	0x01BC,
	0x01C0,
	0x01C4,
	0x01C8,
	0x01CC,
	0x01D0,
	0x01D4,
	0x01D8,
	0x01DC,
	0x01E0,
	0x01E4,
	0x01E8,
	0x01EC,
	0x01F0,
	0x01F4,
	0x0374,
	0x0376,
	0x0378,
	0x037A,
	0x037C,
	0x037E,
	0x0380,
	0x0382,
	0x0384,
	0x0386,
	0x0388,
	0x038A,
	0x038C,
	0x038E,
	0x0390,
	0x0392,
	0x0394,
	0x0396,
	0x0398,
	0x039A,
	0x039C,
	0x039E,
	0x03A0,
	0x03A2,
	0x03A4,
	0x03A6,
	0x03A8,
	0x03AA,
	0x03AC,
	0x03AE,
	0x03B0,
	0x03B2,
	0x03B4,
	0x03B6,
	0x03B8,
	0x03BA,
	0x03BC,
	0x03BE,
	0x03C0,
	0x03C2,
	0x03C4,
	0x03C6,
	0x03C8,
	0x03CA,
	0x03CC,
	0x03CE,
	0x03D0,
	0x03D2,
	0x03D4,
	0x03D6,
	0x03D8,
	0x03DA,
	0x03DC,
	0x03DE,
	0x03E0,
	0x03E2,
	0x03E4,
	0x03E6,
	0x03E8,
	0x03EA,
	0x03EC,
	0x03EE,
	0x03F0,
	0x03F2,
	0x0778,
	0x0779,
	0x077A,
	0x077B,
	0x077C,
	0x077D,
	0x077E,
	0x077F,
	0x0780,
	0x0781,
	0x0782,
	0x0783,
	0x0784,
	0x0785,
	0x0786,
	0x0787,
	0x0788,
	0x0789,
	0x078A,
	0x078B,
	0x078C,
	0x078D,
	0x078E,
	0x078F,
	0x0790,
	0x0791,
	0x0792,
	0x0793,
	0x0794,
	0x0795,
	0x0796,
	0x0797,
	0x0798,
	0x0799,
	0x079A,
	0x079B,
	0x079C,
	0x079D,
	0x079E,
	0x079F,
	0x07A0,
	0x07A1,
	0x07A2,
	0x07A3,
	0x07A4,
	0x07A5,
	0x07A6,
	0x07A7,
	0x07A8,
	0x07A9,
	0x07AA,
	0x07AB,
	0x07AC,
	0x07AD,
	0x07AE,
	0x07AF,
	0x07B0,
	0x07B1,
	0x07B2,
	0x07B3,
	0x07B4,
	0x07B5,
	0x07B6,
	0x07B7,
	0x07B8,
	0x07B9,
	0x07BA,
	0x07BB,
	0x07BC,
	0x07BD,
	0x07BE,
	0x07BF,
	0x07C0,
	0x07C1,
	0x07C2,
	0x07C3,
	0x07C4,
	0x07C5,
	0x07C6,
	0x07C7,
	0x07C8,
	0x07C9,
	0x07CA,
	0x07CB,
	0x07CC,
	0x07CD,
	0x07CE,
	0x07CF,
	0x07D0,
	0x07D1,
	0x07D2,
	0x07D3,
	0x07D4,
	0x07D5,
	0x07D6,
	0x07D7,
	0x07D8,
	0x07D9,
	0x07DA,
	0x07DB,
	0x07DC,
	0x07DD,
	0x07DE,
	0x07DF,
	0x07E0,
	0x07E1,
	0x07E2,
	0x07E3,
	0x07E4,
	0x07E5,
	0x07E6,
	0x07E7,
	0x07E8,
	0x07E9,
	0x07EA,
	0x07EB,
	0x07EC,
	0x07ED,
	0x07EE,
	0x07EF,
	0x07F0,
	0x07F1,
	0x07F2,
	0x07F3,
	0x07F4,
	0x07F5,
	0x07F6,
	0x07F7,
};

void OV683Dual_ComputeGains(double flGain, IMG_UINT8 *gainSequence)
{
    unsigned int n = 0; // if flGain <= 1.0
#if 0    
    if (flGain > aec_min_gain)
    {
        n = (unsigned int)floor((flGain - aec_min_gain) * aec_max_gain);
    }
    n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

    LOG_DEBUG("gain n=%u from %lf\n", n, (flGain-aec_min_gain)*aec_max_gain);
#else
	if (flGain > 1)
	{
		n = (unsigned int)floor((flGain - 1) * 16);
	}
	n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

	LOG_DEBUG("gain n=%u from %lf\n", n, (flGain-1)*16);
#endif
    gainSequence[2] = 0;
    gainSequence[5] = (aec_long_gain_val[n] >> 8) & 0xff;
    gainSequence[8] = aec_long_gain_val[n] & 0xff;
}

static IMG_RESULT OV683Dual_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
#ifdef INFOTM_ISP
	IMG_UINT32 n = 0;
#endif //INFOTM_ISP

	static int runing_flag = 0;

    IMG_UINT8 gainRegs[] =
    {
 //       0x32, 0x08, 0x00,  // start hold of group 0
        
        0x35, 0x07, 0x00,  // Gain register
        0x35, 0x08, 0x00,  //
        0x35, 0x09, 0x00,  //
        
//        0x32, 0x08, 0x10,  // end hold of group 1 register writes
//        0x32, 0x0b, 0x00,  // set quick manual mode
//        0x32, 0x08, 0xe0,  // quick launch group 1
    };

	if (runing_flag == 1)
	{
		return IMG_SUCCESS;
	}

	runing_flag = 1;
	
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
		runing_flag = 0;
        return IMG_ERROR_NOT_INITIALISED;
    }

    printf("===>OV683Dual_SetGain\n");
    psCam->flGain = flGain;

#ifndef INFOTM_ISP
    if (psCam->bEnabled || 1)
    {
        OV683Dual_ComputeGains(flGain, &(gainRegs[3]));

        ov683Dual_i2c_write8(psCam->i2c, gainRegs, sizeof(gainRegs));
    }
#else
	if (flGain > aec_min_gain)
	{
		n = (IMG_UINT32)floor((flGain - 1) * 16);
	}
	
	n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

	gainRegs[5-3] = 0;
	gainRegs[8-3] = (aec_long_gain_val[n] >> 8) & 0xff;
	gainRegs[11-3] = aec_long_gain_val[n] & 0xff;

	ov683Dual_i2c_write8(psCam->i2c, gainRegs, sizeof(gainRegs));
#endif //INFOTM_ISP	

	runing_flag = 0;
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    *pflMin = aec_min_gain;
    *pflMax = aec_max_gain;
    *pui8Contexts = 1;
    
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetGain(SENSOR_HANDLE hHandle, double *pflGain,
    IMG_UINT8 ui8Context)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain=psCam->flGain;
    return IMG_SUCCESS;
}

IMG_RESULT OV683Dual_Create(SENSOR_HANDLE *phHandle, int index)
{

#if !file_i2c
	IMG_UINT8 aui8Regs[3];
    char i2c_dev_path[NAME_MAX];
    IMG_UINT16 chipV;
    IMG_RESULT ret;

    char dev_nm[64];
    char adaptor[64];
    int chn = 0, imgr = 0;
#if defined(INFOTM_ISP)
    int fd = 0;
    memset((void *)dev_nm, 0, sizeof(dev_nm));
    memset((void *)adaptor, 0, sizeof(adaptor));
	//sprintf(dev_nm, "%s", "/dev/ddk_sensor"/*, index*/ );
    sprintf(dev_nm, "%s%d", "/dev/ddk_sensor", index);
	
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
	printf("====>%s opened OK, i2c-addr=0x%x, chn = %d, imgr = %d\n", dev_nm, i2c_addr, chn, imgr);
	sprintf(adaptor, "%s-%d", "i2c", chn);
//	memset((void *)extra_cfg, 0, sizeof(dev_nm));
//	sprintf(extra_cfg, "%s%s%d-config.txt", EXTRA_CFG_PATH, "sensor" , index );
	//printf("extra_cfg = %s \n", extra_cfg);
#endif

#endif //file_i2c
    OV683DualCAM_STRUCT *psCam;
	LOG_DEBUG("-->OV683DualCamCreate\n");

    psCam=(OV683DualCAM_STRUCT *)IMG_CALLOC(1, sizeof(OV683DualCAM_STRUCT));
    if(!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    *phHandle=&(psCam->sFuncs);
    psCam->sFuncs.GetMode = OV683Dual_GetMode;
    psCam->sFuncs.GetState = OV683Dual_GetState;
    psCam->sFuncs.SetMode = OV683Dual_SetMode;
    psCam->sFuncs.Enable = OV683Dual_Enable;
    psCam->sFuncs.Disable = OV683Dual_Disable;
    psCam->sFuncs.Destroy = OV683Dual_Destroy;
    psCam->sFuncs.GetInfo = OV683Dual_GetInfo;

    psCam->sFuncs.GetCurrentGain = OV683Dual_GetGain;
    psCam->sFuncs.GetGainRange = OV683Dual_GetGainRange;
    psCam->sFuncs.SetGain = OV683Dual_SetGain;
    psCam->sFuncs.SetExposure = OV683Dual_SetExposure;
    psCam->sFuncs.GetExposureRange = OV683Dual_GetExposureRange;
    psCam->sFuncs.GetExposure = OV683Dual_GetExposure;

    psCam->sFuncs.GetCurrentFocus = OV683Dual_GetCurrentFocus;
    psCam->sFuncs.GetFocusRange = OV683Dual_GetFocusRange;
    psCam->sFuncs.SetFocus = OV683Dual_SetFocus;
#ifdef INFOTM_ISP
    psCam->sFuncs.SetFlipMirror = OV683Dual_SetFlipMirror;
	psCam->sFuncs.SetGainAndExposure = OV683Dual_SetGainAndExposure;
	psCam->sFuncs.SetFPS = OV683Dual_SetFPS;
	psCam->sFuncs.GetFixedFPS =  OV683Dual_GetFixedFPS;

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
	psCam->sFuncs.ReadSensorCalibrationData = OV683Dual_read_sensor_calibration_data;
	psCam->sFuncs.ReadSensorCalibrationVersion = OV683Dual_read_sensor_calibration_version;
	psCam->sFuncs.UpdateSensorWBGain = OV683Dual_update_sensor_wb_gain;
#endif

#endif //INFOTM_ISP

    // customers should change the clock!
    psCam->refClock = 24 * 1000 * 1000;

    psCam->bEnabled = IMG_FALSE;
    psCam->ui16CurrentMode = 0;
    psCam->currModeReg = NULL;
    psCam->nRegisters = 0;

    psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    psCam->flGain = aec_min_gain;
    psCam->ui32Exposure = 89*302;
    psCam->imager = imgr; // gasket to use - customer should change it if needed

#if !file_i2c
    if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), i2c_addr, adaptor))
    {
        LOG_ERROR("Failed to find I2C device(%d)!\n", i2c_addr);
        IMG_FREE(psCam);
        *phHandle=NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    psCam->i2c = open(i2c_dev_path, O_RDWR);
    if (psCam->i2c < 0)
    {
        LOG_ERROR("Failed to open I2C device: \"%s\", err = %d\n",
            i2c_dev_path, psCam->i2c);
        IMG_FREE(psCam);
        *phHandle=NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
 
	#if 0//ndef NO_DEV_CHECK  //  for debug
		ov683Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL);
		ret = ov683Dual_i2c_read16(psCam->i2c, i2c_slave_addr[0], REG_SC_CMMN_CHIP_ID_0, &chipV);
		ov683Dual_i2c_switch(psCam->i2c, I2C_OV683_CHL);
		if(ret || OV683Dual_CHIP_VERSION != chipV)
		{
			LOG_ERROR("Failed to ensure that the i2c device has a compatible "\
				"OV683Dual sensor! ret=%d chip_version=0x%x (expect chip 0x%x)\n",
				ret, chipV, OV683Dual_CHIP_VERSION);
			close(psCam->i2c);
			IMG_FREE(psCam);
			*phHandle = NULL;
			return IMG_ERROR_DEVICE_NOT_FOUND;
		}
	#endif //NO_DEV_CHECK

	#ifdef ov683Dual_debug
		// clean file
		{
			FILE *f = fopen(ov683Dual_debug, "w");
			fclose(f);
		}
	#endif //ov683Dual_debug
#endif //file_i2c

	//ov683Dual_i2c_switch(psCam->i2c, I2C_OV683_CHL);
#if 0 // for debug
	ret = ov683Dual_i2c_read16(psCam->i2c, I2C_OV683_ADDR, REG_SC_CMMN_CHIP_ID_0, &chipV);
	ov683Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL);
	
	printf("check ov683Dual chip version: 0x%x\n", chipV);
#endif

    // need i2c to have been found to read mode
    OV683Dual_GetModeInfo(psCam, psCam->ui16CurrentMode, &(psCam->currMode));
    psCam->psSensorPhy = SensorPhyInit();
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("Failed to create sensor phy!\n");
#if !file_i2c
        close(psCam->i2c);
#endif //file_i2c
        IMG_FREE(psCam);
        *phHandle = NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    LOG_DEBUG("Sensor initialised\n");
    return IMG_SUCCESS;
}

void OV683Dual_ApplyMode(OV683DualCAM_STRUCT *psCam)
{
    unsigned i, ret = 0;
    // index for flipping register
    unsigned flipIndex0 = psCam->nRegisters * 3;
    // index for flipping register
    unsigned flipIndex1 = psCam->nRegisters * 3;
    // index for exposure register
    unsigned exposureIndex = psCam->nRegisters * 3;
    // index for gain register
    unsigned gainIndex = psCam->nRegisters * 3;
#if !defined(OV683Dual_COPY_REGS)
    IMG_UINT8 values[3];
#endif //OV683Dual_COPY_REGS

    if (!psCam->currModeReg)
    {
        LOG_ERROR("current register modes not available!\n");
        return;
    }
        
    
    LOG_DEBUG("Writing I2C\n");

    printf("OV683Dual_ApplyMode()\n");

    for(i = 0 ; i < psCam->nRegisters * 3 ; i += 3)
    {
#if !defined(OV683Dual_COPY_REGS)
        values[0] = psCam->currModeReg[i+0];
        values[1] = psCam->currModeReg[i+1];
        values[2] = psCam->currModeReg[i+2];
#endif //OV683Dual_COPY_REGS

        if (IS_REG(psCam->currModeReg, i, REG_FORMAT1))
        {
            flipIndex0 = i;
            if (psCam->ui8CurrentFlipping&SENSOR_FLIP_VERTICAL)
            {
#if defined(OV683Dual_COPY_REGS)
                psCam->currModeReg[i+2] = 6;
#else
                values[2] = 6;
#endif //OV683Dual_COPY_REGS
            }
            else
            {
#if defined(OV683Dual_COPY_REGS)
                psCam->currModeReg[i+2] = 0;
#else
                values[2] = 0;
#endif //OV683Dual_COPY_REGS
            }
        }
        if (IS_REG(psCam->currModeReg, i, REG_FORMAT2))
        {
            flipIndex1 = i;
            if (psCam->ui8CurrentFlipping&SENSOR_FLIP_HORIZONTAL)
            {
#if defined(OV683Dual_COPY_REGS)
                psCam->currModeReg[i+2] = 6;
#else
                values[2] = 6;
#endif //OV683Dual_COPY_REGS

            }
            else
            {
#if defined(OV683Dual_COPY_REGS)
                psCam->currModeReg[i+2] = 0;
#else
                values[2] = 0;
#endif //OV683Dual_COPY_REGS
            }
        }

        if (IS_REG(psCam->currModeReg, i, REG_EXPOSURE))
        {
            exposureIndex = i;
        }

        if (IS_REG(psCam->currModeReg, i, REG_GAIN))
        {
            gainIndex = i;
        }

#if 0//!defined(OV683Dual_COPY_REGS)
        ret = ov683Dual_i2c_write8(psCam->i2c, values, 3);
        if(ret != IMG_SUCCESS)
        {
        	break;
        }
#endif //OV683Dual_COPY_REGS
    }

    // if it was parallel
    //psCam->psSensorPhy->psGasket->uiWidth = psCam->currMode.ui16Width;
    //psCam->psSensorPhy->psGasket->uiHeight = psCam->currMode.ui16Height;
    //psCam->psSensorPhy->psGasket->bVSync = IMG_TRUE;
    //psCam->psSensorPhy->psGasket->bHSync = IMG_TRUE;
    //psCam->psSensorPhy->psGasket->ui8ParallelBitdepth = 10;

    psCam->ui32Exposure = psCam->currMode.ui32ExposureMin * 302;
    psCam->ui32Exposure = IMG_MIN_INT(psCam->currMode.ui32ExposureMax,
        psCam->ui32Exposure);

    //
    // before writing the registers, patch the flip horizontal/flipvertical
    // fields to match the required values
    //
    // should be found and fit in array
    if (flipIndex0 >= psCam->nRegisters * 3)
    {
        LOG_WARNING("Did not find flipIndex0 in registers for mode %d\n",
            psCam->ui16CurrentMode);
    }
    if (flipIndex1 >= psCam->nRegisters * 3)
    {
        LOG_WARNING("Did not find flipIndex1 in registers for mode %d\n",
            psCam->ui16CurrentMode);
    }

    if (gainIndex >= psCam->nRegisters * 3)
    {
        LOG_WARNING("Did not find gainIndex in registers for mode %d\n",
            psCam->ui16CurrentMode);
    }
    if (exposureIndex >= psCam->nRegisters * 3)
    {
        LOG_WARNING("Did not find exposureIndex in registers for mode %d\n",
            psCam->ui16CurrentMode);
    }
#if 0
    // should be found and fit in array
    IMG_ASSERT(gainIndex+8 < psCam->nRegisters * 3);
    {
        OV683Dual_ComputeExposure(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin,
            &(mode1280x720[exposureIndex]));
    }

    // should be found and fit in array
    IMG_ASSERT(exposureIndex+8 < psCam->nRegisters * 3);
    {
        OV683Dual_ComputeGains(psCam->flGain, &(mode1280x720[gainIndex]));
    }
#endif //0
    
#if 0//defined(OV683Dual_COPY_REGS)
    ov683Dual_i2c_write8(psCam->i2c, psCam->currModeReg, psCam->nRegisters * 3);
#endif //OV683Dual_COPY_REGS
    
    LOG_DEBUG("Sensor ready to go\n");
    // setup the exposure setting information
}

IMG_RESULT OV683Dual_GetModeInfo(OV683DualCAM_STRUCT *psCam, IMG_UINT16 ui16Mode,
    SENSOR_MODE *info)
{
    unsigned int n;
    IMG_RESULT ret = IMG_SUCCESS;
    
    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = OV683Dual_GetRegisters(psCam, ui16Mode,
        &nRegisters);

    double sclk = 0;
    
    unsigned int sc_cmmm_bit_sel = 0;
    unsigned int sc_cmmn_mipi_sc_ctrl = 0;
    unsigned int hts[2] = { 0, 0 };
    unsigned int hts_v = 0;
    unsigned int vts[2] = { 0, 0 };
    unsigned int vts_v = 0;
    unsigned int frame_t = 0;
    unsigned int h_output[2] = { 0, 0 };
    unsigned int v_output[2] = { 0, 0 };
    unsigned int pll_ctrl_d[2] = { 0, 0 };

    double pll2_predivp = 0;
    double pll2_mult = 0;
    double pll2_prediv = 0;
    double pll2_divsp = 0;
    double pll2_divs = 0;

    double trow = 0; // in micro seconds

	IMG_UINT8 reg_val = 0;
	
    if (!modeReg)
    {
        LOG_ERROR("invalid mode %d\n", modeReg);
        return IMG_ERROR_NOT_SUPPORTED;
    }
#if 0  //for
	static int flag = 0;
	if (flag == 0)
       {
	flag = 1;
	ov683Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL);

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_SC_CMMN_BIT_SEL, &reg_val);
	sc_cmmm_bit_sel = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_SC_CMMN_MIPI_SC_CTRL, &reg_val);
    sc_cmmn_mipi_sc_ctrl = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_TIMING_VTS_0, &reg_val);
	vts[0] = reg_val;
	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_TIMING_VTS_1, &reg_val);
	vts[1] = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_TIMING_HTS_0, &reg_val);
	hts[0] = reg_val;
	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_TIMING_HTS_1, &reg_val);
	hts[1] = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_H_OUTPUT_SIZE_0, &reg_val);
	h_output[0] = reg_val;
	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_H_OUTPUT_SIZE_1, &reg_val);
	h_output[1] = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_V_OUTPUT_SIZE_0, &reg_val);
	v_output[0] = reg_val;
	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_V_OUTPUT_SIZE_1, &reg_val);
	v_output[1] = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_PLL_CTRL_11, &reg_val);
	pll2_predivp = reg_val + 1.0;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_PLL_CTRL_B, &reg_val);
	pll2_prediv = pll_ctrl_b_val[reg_val];

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_PLL_CTRL_C, &reg_val);
	pll_ctrl_d[0] = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_PLL_CTRL_D, &reg_val);
	pll_ctrl_d[1] = reg_val;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_PLL_CTRL_F, &reg_val);
	pll2_divsp = reg_val + 1.0;

	ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], REG_PLL_CTRL_E, &reg_val);
	pll2_divs = pll_ctrl_c_val[reg_val];

printf("<%d, %d,  %d, %d,  %d, %d, %d, %d,  %d, %d,  %d, %d, %d, %d, %d, %d>\n", 
	sc_cmmm_bit_sel , sc_cmmn_mipi_sc_ctrl, 
        vts[0] , vts[1], 
        hts[0], hts[1], 
        h_output[0], h_output[1], 
        v_output[0] , v_output[1], 
        pll2_predivp, pll2_prediv, 
        pll_ctrl_d[0], pll_ctrl_d[1], 
        pll2_divsp, pll2_divs);
	}
#else
	sc_cmmm_bit_sel = 10;
	sc_cmmn_mipi_sc_ctrl = 50;
	vts[0] = 6;
	vts[1] = 20;  
	hts[0] = 10; 
	hts[1] = 8; 
	h_output[0] = 5; 
	h_output[1] = 240;  
	v_output[0] = 5; 
	v_output[1] = 240;
	pll2_predivp = 30;
	pll2_prediv = 0;
	pll_ctrl_d[0] =1072693248;
	pll_ctrl_d[1] = 0;
	pll2_divsp = 1072693248;
	pll2_divs = 0;
#endif

    //ov683Dual_i2c_switch(psCam->i2c, I2C_OV683_CHL);

    pll2_mult = (double)((pll_ctrl_d[0] << 8) | pll_ctrl_d[1]);

    if (0.0 == pll2_predivp || 0.0 == pll2_mult || 0.0 == pll2_prediv
        || 0.0 == pll2_divsp || 0.0 == pll2_divs)
    {
        sclk = 120.0 * 1000 * 1000;//120.0 * 1000 * 1000;
        LOG_WARNING("Did not find all PLL registers - assumes "\
            "sclk of %ld MHz\n", sclk);
#ifdef INFOTM_ISP
        LOG_WARNING("pll2_predivp=%f, pll2_mult=%f, pll2_prediv=%f, pll2_divsp=%f, pll2_divs=%f\n",
        				pll2_predivp, pll2_mult, pll2_prediv, pll2_divsp, pll2_divs);
#endif //INFOTM_ISP
    }
    else
    {
        // xclk is the refClock
        // vco = xclk/PLL2_predivp * PLL2_mult/PLL2_prediv
        double vco = 0;
        // sclk = vco/PLL2_divsp/PLL2_divs

        vco = ((psCam->refClock / pll2_predivp) / pll2_prediv)*pll2_mult;
        sclk = vco / pll2_divsp / pll2_divs;

        LOG_DEBUG("xclk=%.2lfMHz / pll2_predivp=%.2lf / pll2_prediv=%.2lf "\
            "* pll2_mult=%.2lf\n",
            psCam->refClock/1000000, pll2_predivp, pll2_prediv, pll2_mult);
        LOG_DEBUG("vco=%.2lfMHz / pll2_divsp=%.2lf / pll2_divs=%.2lf "\
            "= sclk=%.2lfMHz\n",
            vco/1000000, pll2_divsp, pll2_divs, sclk/1000000);
    }

    hts_v = (hts[1] | (hts[0] << 8));
    vts_v = (vts[1] | (vts[0] << 8));
    trow = (hts_v / sclk) * 1000 * 1000; // sclk in Hz, trow in micro seconds
    frame_t = vts_v * hts_v;
    
    //printf("==>hts=%u, vts=%u, w=%d, h=%d\n", hts_v, vts_v, h_output[1] | (h_output[0] << 8), v_output[1] | (v_output[0] << 8));
   // printf("==>trow=%lf, frame_t=%u\n", trow, frame_t);
    info->ui16Width = 3040;//h_output[1] | (h_output[0] << 8);
    info->ui16Height = 1520;//v_output[1] | (v_output[0] << 8);
    info->ui16VerticalTotal = vts_v;

    info->flFrameRate = 30;//sclk / frame_t;

	
    info->ui8SupportFlipping = SENSOR_FLIP_BOTH;
    info->ui32ExposureMax = (IMG_UINT32)floor(trow *  info->ui16VerticalTotal); //(IMG_UINT32)(100.0 / info->flFrameRate) * 10000.0;
    info->ui32ExposureMin = (IMG_UINT32)floor(trow);
	
#ifdef INFOTM_ISP
	info->dbExposureMin = trow;
	psCam->ui32Exposure = trow * info->ui16VerticalTotal/2; //set initial exposure.
#endif //INFOTM_ISP
		
    
    info->ui8MipiLanes = 4;//((sc_cmmn_mipi_sc_ctrl >> 5) & 0x3) + 1;
	printf("info->ui8MipiLanes = %d\n", info->ui8MipiLanes);
	
    info->ui8BitDepth = (sc_cmmm_bit_sel & 0x1F);
#if defined(INFOTM_ISP)
    psCam->fCurrentFps = info->flFrameRate;
    printf("the flFrameRate = %f\n", info->flFrameRate);
    psCam->fixedFps = psCam->fCurrentFps;
    psCam->initClk = pll2_divsp - 1;
#endif
    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
static IMG_RESULT OV683Dual_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
    IMG_UINT16 ui16Regs;
    IMG_UINT8 aui18FlipMirrorRegs[]=
    {
        0x38, 0x21, 0x00,
        0x38, 0x20, 0x00
    };
    IMG_RESULT ret;

	///////////////////
		if (0)
		{
			IMG_UINT8 aui18FlipMirrorRegs[]=
			{
				0x38, 0x21, 0x00,
				0x38, 0x20, 0x00
			};
	
			IMG_UINT8 ret_data[4] = {0};
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], 0x3820, &ret_data[0]);
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], 0x3821, &ret_data[1]);
	
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[1], 0x3820, &ret_data[2]);
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[1], 0x3821, &ret_data[3]);
	
			printf("========>old reg:(%x, %x) (%x, %x)\n", ret_data[0], ret_data[1], ret_data[2], ret_data[3]);
	
	
			ov683Dual_i2c_write8(psCam->i2c, aui18FlipMirrorRegs, sizeof(aui18FlipMirrorRegs));
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], 0x3820, &ret_data[0]);
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], 0x3821, &ret_data[1]);
	
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[1], 0x3820, &ret_data[2]);
			ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[1], 0x3821, &ret_data[3]);
	
			printf("========>new reg:(%x, %x) (%x, %x)\n", ret_data[0], ret_data[1], ret_data[2], ret_data[3]);
			psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
		}
	///////////////////

    return IMG_SUCCESS;

    switch (flag)
    {
    	default:
    	case SENSOR_FLIP_NONE:
    		ui16Regs = 0x3821;
    		ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[2]);
    		aui18FlipMirrorRegs[2] &= ~0x06; //clear mirror bit.

    		ui16Regs = 0x3820;
    		ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[5]);
    		aui18FlipMirrorRegs[5] &= ~0x06; //clear flip bit.

    		ov683Dual_i2c_write8(psCam->i2c, aui18FlipMirrorRegs, sizeof(aui18FlipMirrorRegs));
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    		break;

    	case SENSOR_FLIP_HORIZONTAL:
       		ui16Regs = 0x3821;
        	ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[2]);
        	aui18FlipMirrorRegs[2] |= 0x06; //set mirror bit.

    		ui16Regs = 0x3820;
    		ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[5]);
    		aui18FlipMirrorRegs[5] &= ~0x06; //clear flip bit.

    		ov683Dual_i2c_write8(psCam->i2c, aui18FlipMirrorRegs, sizeof(aui18FlipMirrorRegs));
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_HORIZONTAL;
    		break;

    	case SENSOR_FLIP_VERTICAL:
    		ui16Regs = 0x3821;
    		ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[2]);
    		aui18FlipMirrorRegs[2] &= ~0x06; //clear mirror bit.

    		ui16Regs = 0x3820;
    		ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[5]);
    		aui18FlipMirrorRegs[5] |= 0x06; //set flip bit.

    		ov683Dual_i2c_write8(psCam->i2c, aui18FlipMirrorRegs, sizeof(aui18FlipMirrorRegs));
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_VERTICAL;
    		break;

    	case SENSOR_FLIP_BOTH:
    		ui16Regs = 0x3821;
    		ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[2]);
    		aui18FlipMirrorRegs[2] |= 0x06; //set mirror bit.

    		ui16Regs = 0x3820;
    		ret = ov683Dual_i2c_read8(psCam->i2c, i2c_slave_addr[0], ui16Regs, &aui18FlipMirrorRegs[5]);
    		aui18FlipMirrorRegs[5] |= 0x06; //set flip bit.

    		ov683Dual_i2c_write8(psCam->i2c, aui18FlipMirrorRegs, sizeof(aui18FlipMirrorRegs));
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
    		break;

    }
    	return IMG_SUCCESS;
}


static IMG_UINT8 g_OV683Dual_ui8LinesValHiBackup = 0xff;
static IMG_UINT8 g_OV683Dual_ui8LinesValMidBackup = 0xff;
static IMG_UINT8 g_OV683Dual_ui8LinesValLoBackup = 0xff;

static IMG_UINT8 g_OV683Dual_ui8GainValHiBackup = 0xff;
static IMG_UINT8 g_OV683Dual_ui8GainValMidBackup = 0xff;
static IMG_UINT8 g_OV683Dual_ui8GainValLoBackup = 0xff;

static IMG_RESULT OV683Dual_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
#if 0
    OV683Dual_SetExposure(hHandle, ui32Exposure, ui8Context);
    OV683Dual_SetGain(hHandle, flGain, ui8Context);
	
#else
	static int runing_flag = 0;
	IMG_UINT32 ui32Lines;
	IMG_UINT32 n = 0, L = 6;
	IMG_UINT8 write_type = 0;

	IMG_UINT8 ui8Regs[] =
	{
//		0x32, 0x08, 0x00, // GROUP_ACCESS start hold of group 0

        //0x35, 0x07, 0x00, // Gain register
        0x35, 0x08, 0x00, //
        0x35, 0x09, 0x00, //

		0x35, 0x00, 0x00, // Exposure register
		0x35, 0x01, 0x2E, //
		0x35, 0x02, 0x80, //
		
//		0x32, 0x08, 0x10, // end hold of group 0 register writes
//		0x32, 0x0b, 0x00, // set quick manual mode
//		0x32, 0x08, 0xe0, // quick launch group 0
	};

	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

	if (!psCam->psSensorPhy)
	{
		LOG_ERROR("sensor not initialised\n");
		runing_flag = 0;
		return IMG_ERROR_NOT_INITIALISED;
	}
	
#if 0
	while (runing_flag == 1)
	{
		//printf("====>waiting update AE...\n");
		usleep(5*1000);
	}
#else
	if (runing_flag == 1)
	{
		return IMG_SUCCESS;
	}
#endif

	runing_flag = 1;

	//01.Exposure part
	//
	psCam->ui32Exposure = ui32Exposure;

	ui32Lines = psCam->ui32Exposure / psCam->currMode.dbExposureMin;

//	if (ui32Lines > psCam->currMode.ui16VerticalTotal - 4)
//		ui32Lines = psCam->currMode.ui16VerticalTotal - 4;

	ui8Regs[14-L] = (ui32Lines >> 12) & 0xf;
	ui8Regs[17-L] = (ui32Lines >> 4) & 0xff;
	ui8Regs[20-L] = (ui32Lines << 4) & 0xff;
	LOG_DEBUG("Exposure Val %x\n",(psCam->ui32Exposure/psCam->currMode.dbExposureMin));

	//02.Gain part
	//
	psCam->flGain = flGain;
#if 1
	if (flGain > aec_min_gain)
	{
		n = (IMG_UINT32)floor((flGain - 1) * 16);
	}
	n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

	//ui8Regs[5-L] = 0;
	ui8Regs[8-L] = (aec_long_gain_val[n] >> 8) & 0xff;
	ui8Regs[11-L] = aec_long_gain_val[n] & 0xff;
	printf("==>gain %f n=%u from %lf\n", flGain, n, (flGain - 1) * 16);
#else
	if (flGain <= 1.0) 
	{
		// take care of possible underflow
		// unlikely to happen because minimum gain is 1.0
		n = 0;
	} 
	else 
	{
		n = (unsigned int)floor((flGain - 1.0) * 16); // always >=0
	}
	
	if (n >= sizeof(asGainSettings) / sizeof(IMG_UINT16))
	{
		n = sizeof(asGainSettings) / sizeof(IMG_UINT16) - 1;
	}	
	ui8Regs[14-L] = 0;//asGainSettings[n] >> 16; 
	ui8Regs[17-L] = (asGainSettings[n] >> 8) & 0xff;
	ui8Regs[20-L] = asGainSettings[n] & 0xff;
	LOG_DEBUG("gain n=%u from %lf\n", n, (flGain-1)*16);
#endif

#if 1
	{
		static float pre_gain = 0.0;
		//static IMG_UINT32 pre_exposure = 0;

		//if (psCam->flGain > 1.1 && (psCam->flGain - pre_gain) < 0.25 && (psCam->flGain - pre_gain) > -0.25)
		if (psCam->flGain >= 15.0 && (psCam->flGain - pre_gain) <= 0.25 && (psCam->flGain - pre_gain) >= -0.25)
		{
			runing_flag = 0;
			return IMG_SUCCESS;
		}
#if 0
		else if (psCam->flGain >= 15.0 && (psCam->flGain - pre_gain) < 0.35 && (psCam->flGain - pre_gain) > -0.35)
		{
			runing_flag = 0;
			return IMG_SUCCESS;			
		}	
#endif
		pre_gain = psCam->flGain;
	}
#endif


	if ((g_OV683Dual_ui8LinesValHiBackup != ui8Regs[14-L]) ||
		(g_OV683Dual_ui8LinesValMidBackup != ui8Regs[17-L]) ||
		(g_OV683Dual_ui8LinesValLoBackup != ui8Regs[20-L])) 
	{
		//ov683Dual_i2c_write8(psCam->i2c, ui8Regs + 6, sizeof(ui8Regs) - 6);
		write_type |= 0x2;
			
		g_OV683Dual_ui8LinesValHiBackup = ui8Regs[14-L];
		g_OV683Dual_ui8LinesValMidBackup = ui8Regs[17-L];
		g_OV683Dual_ui8LinesValLoBackup = ui8Regs[20-L];
	}

	if (
		/*(g_OV683Dual_ui8GainValHiBackup != ui8Regs[5-L]) ||*/	
		(g_OV683Dual_ui8GainValMidBackup != ui8Regs[8-L]) || 
		(g_OV683Dual_ui8GainValLoBackup != ui8Regs[11-L]) )
	{
		write_type |= 0x1;
		//ov683Dual_i2c_write8(psCam->i2c, ui8Regs, 6);
		
		//g_OV683Dual_ui8GainValHiBackup = ui8Regs[5-L];
		g_OV683Dual_ui8GainValMidBackup = ui8Regs[8-L];
		g_OV683Dual_ui8GainValLoBackup = ui8Regs[11-L];
	}

	switch (write_type)
	{
	case 0x3:
		ov683Dual_i2c_write8(psCam->i2c, ui8Regs, sizeof(ui8Regs));
		break;
	case 0x2:
		ov683Dual_i2c_write8(psCam->i2c, ui8Regs + 6, sizeof(ui8Regs) - 6);
		break;
	case 0x1:
		ov683Dual_i2c_write8(psCam->i2c, ui8Regs, 6);
		break;
	default:
		break;
	}
#endif
    runing_flag = 0;
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
	if (pfixed != NULL)
	{
		OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

		*pfixed = (int)psCam->fixedFps;
	}

	return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_SetFPS(SENSOR_HANDLE hHandle, double fps)
{
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
	IMG_UINT8 aui8Regs[3];
    float down_ratio = 0.0;


    //printf("current fps =%f\n", fps);

	psCam->fCurrentFps = 30;
	
	return IMG_SUCCESS;
}

/////////////////////////////////////////////////////////////
// read otp data from sensor register
#ifdef INFOTM_SENSOR_OTP_DATA_MODULE

static IMG_RESULT ov683Dual_sensor_i2c_write8(int i2c, char i2c_addr, const IMG_UINT8 *data, unsigned int len)
{
#if !file_i2c
    unsigned int i, j;
    IMG_UINT8 ret_data = 0;
    IMG_UINT8 buff[2];
    const IMG_UINT8 *ori_data_ptr =data ;

    static int running_flag = 0;
    int write_len = 0;

    if (len % 3)
    {
        LOG_ERROR("Wrong len of data array, len = %d", len);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

	while (running_flag == 1)
	{
		usleep(200);
		printf("waiting ov683Dual_i2c_write8\n");
	}
	running_flag = 1;

	{
		data = ori_data_ptr;
		
	    /* Set I2C slave address */
	    for (i = 0; i < len; data += 3, i += 3)
	    {
		    if (ioctl(i2c, I2C_SLAVE_FORCE, i2c_addr))
	        {
	            LOG_ERROR("Failed to write I2C slave address %x!\n", i2c_addr);
                running_flag = 0;	
	            return IMG_ERROR_BUSY;
	        }
		   
	        write_len = write(i2c, data, 3);

	        if (write_len != 3)
	        {
	            LOG_ERROR("Failed to write I2C(%x) data! write_len = %d, index = %d\n", i2c_addr, write_len, i);
           	    running_flag = 0;	
	            return IMG_ERROR_BUSY;
	        }
	    }
	}	
#endif //file_i2c
    running_flag = 0;
    return IMG_SUCCESS;
}

static int calib_direction = 0;
static IMG_UINT8 sensor_i2c_addr[2] = {(0x6C >> 1), (0x20 >> 1)}; // left, right

#ifdef INFOTM_E2PROM_METHOD
#define E2PROM_DBG_LOG
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

static IMG_RESULT OV683Dual_update_sensor_wb_gain_e2prom(SENSOR_HANDLE hHandle, IMG_FLOAT awb_convert_gain, IMG_UINT16 version);

//  + sizeof(E2PROM_HEADER)

#define E2PROM_CALIBR_REG_CNT (RING_CNT*RING_SECTOR_CNT*3 + 9)

static IMG_UINT8 sensor_e2prom_calibration_data[2][E2PROM_CALIBR_REG_CNT] = {0};
static IMG_UINT8 sensor_e2prom_version_info[2][4] = {0};
static IMG_RESULT ov683Dual_read_e2prom_data(int i2c, char i2c_addr, IMG_UINT16 start_addr, int count, IMG_UINT8 *data)
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

#ifdef E2PROM_DBG_LOG
    printf("===>e2prom offset %d and read %d bytes\n", offset, read_bytes);
{
    int i = 0, j = 0, k = 0, start = 9;//9 + RING_CNT*RING_SECTOR_CNT*3 - 16*3;
    printf("offsetx(%d, %d), offsety(%d, %d), ring_cnt(%d, %d)\n", data[0], data[1], data[2], data[3], data[4], data[5]);

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
}   
#endif

	return IMG_SUCCESS;
}


static IMG_UINT8* OV683Dual_read_sensor_e2prom_calibration_data(SENSOR_HANDLE hHandle, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version)
{
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
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
	printf("===>OV683Dual_read_sensor_e2prom_calibration_data %d\n", sensor_index);
#endif

#ifdef INFOTM_SKIP_OTP_DATA
	return NULL;
#endif

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return NULL;
    }

	if ((calib_direction == 1 && (sensor_index == 1 && sensor_inited_flag_L) || (sensor_index == 0 && sensor_inited_flag_R)) || 
        (calib_direction == 0 && (sensor_index == 0 && sensor_inited_flag_L) || (sensor_index == 1 && sensor_inited_flag_R)))
	{
		return sensor_e2prom_calibration_data[sensor_index];
	}
	
	ov683Dual_read_e2prom_data(psCam->i2c, sensor_index, 0, reg_cnt, sensor_e2prom_calibration_data[sensor_index]);

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
		//OV683Dual_update_sensor_wb_gain_e2prom(hHandle, awb_convert_gain, *otp_calibration_version);
	}
	
	return sensor_e2prom_calibration_data[sensor_index];
}

static IMG_UINT8* OV683Dual_read_sensor_e2prom_calibration_version(SENSOR_HANDLE hHandle, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version)
{
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
	
	int group_index = 0;
	IMG_UINT16 start_addr = 0x7111;
	
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return NULL;
    }

	if (otp_calibration_version != NULL)
	{
		*otp_calibration_version = 0x1;
	}
	return sensor_e2prom_version_info[sensor_index];
}

#define WB_CALIBRATION_METHOD_FROM_LIPIN

static IMG_RESULT OV683Dual_update_sensor_wb_gain_e2prom(SENSOR_HANDLE hHandle, IMG_FLOAT awb_convert_gain, IMG_UINT16 version)
{
    int offset = 6/* + sizeof(E2PROM_HEADER)*/;
    
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
    IMG_FLOAT temp; 
	IMG_UINT16 RGain[2] = {0x400, 0x400};
	IMG_UINT16 GGain[2] = {0x400, 0x400};
	IMG_UINT16 BGain[2] = {0x400, 0x400};

	IMG_UINT8 ui8Regs[2][18/**3*/] = 
	{
		{
			0x50, 0x0c, 0x04,
			0x50, 0x0d, 0x00, // long r gain
			0x50, 0x0e, 0x04,
			0x50, 0x0f, 0x00, // long g gain
			0x50, 0x10, 0x04,
			0x50, 0x11, 0x00, // long b gain
		},  
		{
			0x50, 0x0c, 0x04,
			0x50, 0x0d, 0x00, // long r gain
			0x50, 0x0e, 0x04,
			0x50, 0x0f, 0x00, // long g gain
			0x50, 0x10, 0x04,
			0x50, 0x11, 0x00, // long b gain
		}
	};
	
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    printf("ROI_L<%x, %x, %x>, ROI_R<%x, %x, %x>\n", sensor_e2prom_calibration_data[0][0+offset], sensor_e2prom_calibration_data[0][1+offset], sensor_e2prom_calibration_data[0][2+offset], sensor_e2prom_calibration_data[1][0+offset], sensor_e2prom_calibration_data[1][1+offset], sensor_e2prom_calibration_data[1][2+offset]);
    
#ifdef WB_CALIBRATION_METHOD_FROM_LIPIN
	// g gain
	if (sensor_e2prom_calibration_data[0][1+offset] > sensor_e2prom_calibration_data[1][1+offset])
	{
		temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[0][1+offset]*awb_convert_gain)/(sensor_e2prom_calibration_data[1][1+offset]);
        GGain[1] = (IMG_UINT16)temp;
	}
	else
	{
		temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[1][1+offset]*awb_convert_gain)/(sensor_e2prom_calibration_data[0][1+offset]);
        GGain[0] = 0x400;//(IMG_UINT16)temp;
	}

	// r gain
	if (sensor_e2prom_calibration_data[0][0+offset] > sensor_e2prom_calibration_data[1][0+offset])
	{
	    temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[0][0+offset]*awb_convert_gain)/(sensor_e2prom_calibration_data[1][0+offset]);
        
		RGain[1] = (IMG_UINT16)temp;
	}
	else
	{
		temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[1][0+offset]*awb_convert_gain)/(sensor_e2prom_calibration_data[0][0+offset]);
        
        RGain[0] = (IMG_UINT16)temp;
	}

	// b gain
	if (sensor_e2prom_calibration_data[0][2+offset] > sensor_e2prom_calibration_data[1][2+offset]) 
	{
		temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[0][2+offset]*awb_convert_gain)/(sensor_e2prom_calibration_data[1][2+offset]);
        BGain[1] = (IMG_UINT16)temp;
	}
	else
	{
		temp = (0x400*(IMG_FLOAT)sensor_e2prom_calibration_data[1][2+offset]*awb_convert_gain)/(sensor_e2prom_calibration_data[0][2+offset]);
        
        BGain[0] = 0x400;//(IMG_UINT16)temp;
	}
#ifdef OTP_DBG_LOG
	printf("===>RGain_0 = %x, GGain_0 = %x, BGain_0 = %x, RGain_1 = %x, GGain_1 = %x, BGain_1 = %x\n", RGain[0], GGain[0], BGain[0], RGain[1], GGain[1], BGain[1]);	
#endif

    ui8Regs[0][2] = (RGain[0]>>8); ui8Regs[0][5] = (RGain[0]&0xFF);
    ui8Regs[0][8] = (GGain[0]>>8); ui8Regs[0][11] = (GGain[0]&0xFF);
    ui8Regs[0][14] = (BGain[0]>>8); ui8Regs[0][17] = (BGain[0]&0xFF);

    ui8Regs[1][2] = RGain[1]>>8; ui8Regs[1][5] = RGain[1]&0xFF;
    ui8Regs[1][8] = GGain[1]>>8; ui8Regs[1][11] = GGain[1]&0xFF;
    ui8Regs[1][14] = BGain[1]>>8; ui8Regs[1][17] = BGain[1]&0xFF;

	if (ov683Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL) != IMG_SUCCESS)
	{
		printf("===>switch to sensor i2c error\n");
	}

	ov683Dual_sensor_i2c_write8(psCam->i2c, sensor_i2c_addr[0], ui8Regs[0], sizeof(ui8Regs[0]));
	ov683Dual_sensor_i2c_write8(psCam->i2c, sensor_i2c_addr[1], ui8Regs[1], sizeof(ui8Regs[1]));

    if (ov683Dual_i2c_switch(psCam->i2c, I2C_OV683_CHL) != IMG_SUCCESS)
    {
        printf("===>switch to ov683 i2c error\n");
    }
#endif


    if (0)
    {
        int x = 0;
        for (x = offset; x < E2PROM_CALIBR_REG_CNT; x += 3)
        {
            sensor_e2prom_calibration_data[0][x] = (RGain[0]*sensor_e2prom_calibration_data[0][x])/0x400;
            sensor_e2prom_calibration_data[1][x] = (RGain[1]*sensor_e2prom_calibration_data[1][x])/0x400;
#if 0 //def OTP_DBG_LOG
            printf("R[%d]=%d,%d\n", x, sensor_e2prom_calibration_data[0][x], sensor_e2prom_calibration_data[1][x]);
#endif

        }

        for (x = offset+1; x < E2PROM_CALIBR_REG_CNT; x += 3)
        {
            sensor_e2prom_calibration_data[0][x] = (GGain[0]*sensor_e2prom_calibration_data[0][x])/0x400;
            sensor_e2prom_calibration_data[1][x] = (GGain[1]*sensor_e2prom_calibration_data[1][x])/0x400;
#if 0 //def OTP_DBG_LOG
            printf("G[%d]=%d,%d\n", x, sensor_e2prom_calibration_data[0][x], sensor_e2prom_calibration_data[1][x]);
#endif
        }
        
        for (x = offset+2; x < E2PROM_CALIBR_REG_CNT; x += 3)
        {
            sensor_e2prom_calibration_data[0][x] = (BGain[0]*sensor_e2prom_calibration_data[0][x])/0x400;
            sensor_e2prom_calibration_data[1][x] = (BGain[1]*sensor_e2prom_calibration_data[1][x])/0x400;
#if 0 //def OTP_DBG_LOG
            printf("B[%d]=%d,%d\n", x, sensor_e2prom_calibration_data[0][x], sensor_e2prom_calibration_data[1][x]);
#endif

        }
    }
    
	return IMG_SUCCESS;
}

#endif

static IMG_RESULT ov683Dual_read_otp_data(int i2c, char i2c_addr, IMG_UINT16 start_addr, int count, IMG_UINT8 *data)
{
#if !file_i2c
    int ret, i = 0;
    IMG_UINT8 buff[2], otp_reg_val = 0;
	IMG_UINT8 ui8Regs[18] = 
	{
		0x50, 0x00, 0x00,
		0x3d, 0x84, 0xc0,
		0x3d, 0x88, 0x00,
		0x3d, 0x89, 0x00,
		0x3d, 0x8a, 0x00,
		0x3d, 0x8b, 0x00,
	};

	ui8Regs[8]  = start_addr>>8;
	ui8Regs[11]  = start_addr&0xff;
	ui8Regs[14]  = (start_addr + count - 1)>>8;
	ui8Regs[17] = (start_addr + count - 1)&0xff;
    
    IMG_ASSERT(data != NULL);  // null pointer forbidden

	//printf("===>ov683Dual_read_otp_data start addr %x, count %d\n", start_addr, count);
	
    /* Set I2C slave address */
	if (ioctl(i2c, I2C_SLAVE_FORCE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C read address!\n");
        return IMG_ERROR_BUSY;
    }

	// enable otp
	ov683Dual_i2c_read8(i2c, i2c_addr, 0x5000, &otp_reg_val);
	ui8Regs[2] = (otp_reg_val | 0x20);

	//printf("===>0x5000 reg val is %x\n", otp_reg_val);

	// set address range to otp
	ov683Dual_sensor_i2c_write8(i2c, i2c_addr, ui8Regs, sizeof(ui8Regs));

	//printf("===>set address range to otp done\n");

	// clear otp buffer

	for (i = 0; i < count; i++)
	{
		ui8Regs[0] = ((start_addr + i) >> 8) & 0xFF;
		ui8Regs[1] = (start_addr + i) & 0xFF;
		ui8Regs[2] = 0x00;
		ov683Dual_sensor_i2c_write8(i2c, i2c_addr, ui8Regs, 3);
	}
	//printf("===>clear otp buffer done\n");
		
	// load otp data

	ui8Regs[0] = 0x3d;
	ui8Regs[1] = 0x81;
	ui8Regs[2] = 0x01;
	ov683Dual_sensor_i2c_write8(i2c, i2c_addr, ui8Regs, 3);

	usleep(20*1000);

	//printf("===>load otp data done\n");

	for (i = 0; i < count; i++)
	{
#if 0
	    buff[0] = ((start_addr + i) >> 8) & 0xFF;
	    buff[1] = (start_addr + i) & 0xFF;
/*
		if (ioctl(i2c, I2C_SLAVE_FORCE, i2c_addr))
	    {
	        LOG_ERROR("Failed to write I2C read address!\n");
	        return IMG_ERROR_BUSY;
	    }
*/			
	    ret = write(i2c, buff, sizeof(buff));
	    if (ret != sizeof(buff))
	    {
	        LOG_WARNING("Wrote %dB instead of %luB before reading\n", ret, sizeof(buff));
	    }
	    
	    ret = read(i2c, &data[i], 1);
	    if (1 != ret)
	    {
	        LOG_ERROR("Failed to read I2C at 0x%x\n", start_addr);
	        return IMG_ERROR_FATAL;
	    }
#else
		ov683Dual_i2c_read8(i2c, i2c_addr, start_addr + i, &data[i]);
#endif

#ifdef OTP_DBG_LOG
		printf("%d ", data[i]);
		
		if (i == (count - 1) || (i > 0 && i%9 == 0))
		{
			printf("\n", data[i]);			
		}
#endif
	}

	
	// clear otp buffer
	for (i = 0; i < count; i++)
	{
		ui8Regs[0] = ((start_addr + i) >> 8) & 0xFF;
		ui8Regs[1] = (start_addr + i) & 0xFF;
		ui8Regs[2] = 0x00;
		ov683Dual_sensor_i2c_write8(i2c, i2c_addr, ui8Regs, 3);
	}

	// disable otp
//	ui8Regs[0] = 0x50;
//	ui8Regs[1] = 0x00;
//	ui8Regs[2] = (otp_reg_val & (~0x20));
//	ov683Dual_sensor_i2c_write8(i2c, i2c_addr, ui8Regs, 3);
	
    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif //file_i2c
}

static int ov683Dual_get_valid_otp_group(int i2c, char i2c_addr, IMG_UINT16 group_flag_addr)
{
	int group_index = -1;
	IMG_UINT8 flag_ret = 0;
	
	ov683Dual_read_otp_data(i2c, i2c_addr, group_flag_addr, 1, &flag_ret);

    //printf("==>ov683Dual_get_valid_otp_group(%x, %x, %x, %x)\n", flag_ret&0x3, flag_ret&0x0c, flag_ret&0x30, flag_ret&0xc0);
    
    if ((flag_ret&0x3) == 0x01)
    {// group 4
        group_index = 4;
    }
    else if ((flag_ret&0x0c) == 0x04)
    {// group 3
        group_index = 3;
    }
    else if((flag_ret&0x30) == 0x10)
    {// group 2
        group_index = 2;
    }
    else if((flag_ret&0xc0) == 0x40)
    {// group 1
        group_index = 1;
    }
    else
    {
        group_index = 0xFF;
    }
/*
	if (flag_ret&0x0c)
	{
		if (!(flag_ret&0x08))
		{
			group_index = 3;
		}
	}
	else if (flag_ret&0x30)
	{
		if (!(flag_ret&0x20))
		{
			group_index = 2;
		}
	}
	else if (flag_ret&0xc0)
	{
		
		if (!(flag_ret&0x80))
		{
			group_index = 1;
		}
	}
*/
	return group_index;	
}

#define CALIBR_REG_CNT (29*3+2)
static IMG_UINT8 sensor_calibration_data[2][CALIBR_REG_CNT] = {0};
static IMG_UINT8 sensor_version_info[2][4] = {0};

static IMG_UINT16 sensor_version_no[2] = {0x1400, 0x1400};


static IMG_UINT8* OV683Dual_read_sensor_calibration_data(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version)
{
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
	int group_index = 0;
	IMG_UINT16 start_addr = 0x7115;
	int reg_cnt = 0;

	static IMG_BOOL sensor_inited_flag_L = IMG_FALSE, sensor_inited_flag_R = IMG_FALSE;

	if (infotm_method == 2) // e2prom
	{
		return OV683Dual_read_sensor_e2prom_calibration_data(hHandle, sensor_index, awb_convert_gain, otp_calibration_version);
	}

    if (infotm_method == 0 || infotm_method == 1)
    {
        sensor_version_no[0] = 0x1400;
        otp_calibration_version = sensor_version_no[0];
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
	printf("===>OV683Dual_read_sensor_calibration_data %d\n", sensor_index);
#endif

#ifdef INFOTM_SKIP_OTP_DATA
	return NULL;
#endif

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return NULL;
    }


	if ((calib_direction == 1 && ((sensor_index == 1 && sensor_inited_flag_L) || (sensor_index == 0 && sensor_inited_flag_R))) || 
        (calib_direction == 0 && ((sensor_index == 0 && sensor_inited_flag_L) || (sensor_index == 1 && sensor_inited_flag_R))))
	{
		return sensor_calibration_data[sensor_index];
	}

	if (ov683Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL) != IMG_SUCCESS)
	{
		printf("===>switch to sensor i2c error\n");
	}

	group_index = ov683Dual_get_valid_otp_group(psCam->i2c, sensor_i2c_addr[sensor_index], 0x7110);

	switch (group_index)
	{
	case 1:
		start_addr = 0x7115;
		break;
	case 2:
		start_addr = 0x7172;
        break;       
    // otp v1.5//////
    case 3:
        start_addr = 0x71f8;
		break;
    case 4:
        start_addr = 0x71fb;
        break;
    ///////////////
	default:
#ifdef OTP_DBG_LOG
		LOG_ERROR("invalid group index %d\n", group_index);
#endif
		return NULL;
	}

    if (group_index > 2)
    {
        reg_cnt = 3;
        *otp_calibration_version = 0x1500;
    }
    
    ov683Dual_read_otp_data(psCam->i2c, sensor_i2c_addr[sensor_index], start_addr, reg_cnt, sensor_calibration_data[sensor_index]);
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
		OV683Dual_update_sensor_wb_gain(hHandle, infotm_method, awb_convert_gain, *otp_calibration_version);
	}
	
	if (ov683Dual_i2c_switch(psCam->i2c, I2C_OV683_CHL) != IMG_SUCCESS)
	{
		printf("===>switch to ov683 i2c error\n");
	}
	return sensor_calibration_data[sensor_index];
}

static IMG_UINT8* OV683Dual_read_sensor_calibration_version(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version)
{
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
	
	int group_index = 0;
	IMG_UINT16 start_addr = 0x7111;
	
	if (infotm_method == 2)
	{
		return OV683Dual_read_sensor_e2prom_calibration_version(hHandle, sensor_index, otp_calibration_version);
	}
	
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return NULL;
    }

	group_index = ov683Dual_get_valid_otp_group(psCam->i2c, sensor_i2c_addr[sensor_index], 0x7110);

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

	ov683Dual_read_otp_data(psCam->i2c, sensor_i2c_addr[sensor_index], start_addr, 4, sensor_version_info[sensor_index]);
	printf("===>sensor-%d: mid-%d, year-%d, month-%d, day-%d<===\n", 
		   sensor_version_info[sensor_index][0], sensor_version_info[sensor_index][1], 
		   sensor_version_info[sensor_index][2], sensor_version_info[sensor_index][3]);

	if (otp_calibration_version != NULL)
	{
		*otp_calibration_version = sensor_version_no[0];
	}
	return sensor_version_info[sensor_index];
}

#define WB_CALIBRATION_METHOD_FROM_LIPIN

static IMG_RESULT OV683Dual_update_sensor_wb_gain(SENSOR_HANDLE hHandle, int infotm_method, IMG_FLOAT awb_convert_gain, IMG_UINT16 version)
{
	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);
	IMG_UINT16 RGain[2] = {0x400, 0x400};
	IMG_UINT16 GGain[2] = {0x400, 0x400};
	IMG_UINT16 BGain[2] = {0x400, 0x400};

	IMG_UINT8 ui8Regs[2][18/**3*/] = 
	{
		{
			0x50, 0x0c, 0x04,
			0x50, 0x0d, 0x00, // long r gain
	/*
			0x50, 0x12, 0x04,
			0x50, 0x13, 0x00, // middle r gain
			0x50, 0x18, 0x04,
			0x50, 0x19, 0x00, // short r gain
	*/
			0x50, 0x0e, 0x04,
			0x50, 0x0f, 0x00, // long g gain
	/*
			0x50, 0x14, 0x04,
			0x50, 0x15, 0x00, // middle g gain
			0x50, 0x1a, 0x04,
			0x50, 0x1b, 0x00, // short g gain
	*/
			0x50, 0x10, 0x04,
			0x50, 0x11, 0x00, // long b gain
	/*
			0x50, 0x16, 0x04,
			0x50, 0x17, 0x00, // middle b gain
			0x50, 0x1c, 0x04,
			0x50, 0x1d, 0x00, // short b gain
	*/
		},  
		{
			0x50, 0x0c, 0x04,
			0x50, 0x0d, 0x00, // long r gain
	/*		
			0x50, 0x12, 0x04,
			0x50, 0x13, 0x00, // middle r gain
			0x50, 0x18, 0x04,
			0x50, 0x19, 0x00, // short r gain
	*/
			0x50, 0x0e, 0x04,
			0x50, 0x0f, 0x00, // long g gain
	/*
			0x50, 0x14, 0x04,
			0x50, 0x15, 0x00, // middle g gain
			0x50, 0x1a, 0x04,
			0x50, 0x1b, 0x00, // short g gain
	*/
			0x50, 0x10, 0x04,
			0x50, 0x11, 0x00, // long b gain
	/*
			0x50, 0x16, 0x04,
			0x50, 0x17, 0x00, // middle b gain
			0x50, 0x1c, 0x04,
			0x50, 0x1d, 0x00, // short b gain
	*/
		}
	};


    printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

#ifdef WB_CALIBRATION_METHOD_FROM_LIPIN
    //printf("==>OV683Dual_update_sensor_wb_gain version %x\n", version);
    if (infotm_method == 0 && version >= 0x1500)
    {
        // g gain
    	if (sensor_calibration_data[0][1] > sensor_calibration_data[1][1])
    	{
    		GGain[1] = (0x400*sensor_calibration_data[0][1]*awb_convert_gain)/(sensor_calibration_data[1][1]);
    	}
    	else
    	{
    		GGain[0] = (0x400*sensor_calibration_data[1][1]*awb_convert_gain)/(sensor_calibration_data[0][1]);
    	}

    	// r gain
    	if (sensor_calibration_data[0][0] > sensor_calibration_data[1][0])
    	{
    		RGain[1] = (0x400*sensor_calibration_data[0][0]*awb_convert_gain)/(sensor_calibration_data[1][0]);
    	}
    	else
    	{
    		RGain[0] = (0x400*sensor_calibration_data[1][0]*awb_convert_gain)/(sensor_calibration_data[0][0]);
    	}		

    	// b gain
    	if (sensor_calibration_data[0][2] > sensor_calibration_data[1][2]) 
    	{
    		BGain[1] = (0x400*sensor_calibration_data[0][2]*awb_convert_gain)/(sensor_calibration_data[1][2]);
    	}
    	else
    	{
    		BGain[0] = (0x400*sensor_calibration_data[1][2]*awb_convert_gain)/(sensor_calibration_data[0][2]);
    	}
        
#ifdef OTP_DBG_LOG
        printf("===>RGain_0 = %x, GGain_0 = %x, BGain_0 = %x, RGain_1 = %x, GGain_1 = %x, BGain_1 = %x\n", RGain[0], GGain[0], BGain[0], RGain[1], GGain[1], BGain[1]);
#endif
        return IMG_SUCCESS;
    }
    
    if (infotm_method == 0)
    {
    	// g gain
    	if (sensor_calibration_data[0][1+9] > sensor_calibration_data[1][1+9])
    	{
    		GGain[1] = (0x400*sensor_calibration_data[0][1+9]*awb_convert_gain)/(sensor_calibration_data[1][1+9]);
    	}
    	else
    	{
    		GGain[0] = (0x400*sensor_calibration_data[1][1+9]*awb_convert_gain)/(sensor_calibration_data[0][1+9]);
    	}

    	// r gain
    	if (sensor_calibration_data[0][0+9] > sensor_calibration_data[1][0+9])
    	{
    		RGain[1] = (0x400*sensor_calibration_data[0][0+9]*awb_convert_gain)/(sensor_calibration_data[1][0+9]);
    	}
    	else
    	{
    		RGain[0] = (0x400*sensor_calibration_data[1][0+9]*awb_convert_gain)/(sensor_calibration_data[0][0+9]);
    	}		

    	// b gain
    	if (sensor_calibration_data[0][2+9] > sensor_calibration_data[1][2+9]) 
    	{
    		BGain[1] = (0x400*sensor_calibration_data[0][2+9]*awb_convert_gain)/(sensor_calibration_data[1][2+9]);
    	}
    	else
    	{
    		BGain[0] = (0x400*sensor_calibration_data[1][2+9]*awb_convert_gain)/(sensor_calibration_data[0][2+9]);
    	}
    }
    else
    {
#ifdef SMOOTH_CENTER_RGB
		float ratio = ((float)sensor_calibration_data[0][1] - sensor_calibration_data[0][4])/sensor_calibration_data[0][4];
		if (ratio >= 0.2)
		{
			sensor_calibration_data[0][0] = (sensor_calibration_data[0][0] + sensor_calibration_data[0][3])/2;
			sensor_calibration_data[0][1] = (sensor_calibration_data[0][1] + sensor_calibration_data[0][4])/2;
			sensor_calibration_data[0][2] = (sensor_calibration_data[0][2] + sensor_calibration_data[0][5])/2;
		}
		
		printf("===>ratioL %f\n", ratio);
		
		ratio = ((float)sensor_calibration_data[1][1] - sensor_calibration_data[1][4])/sensor_calibration_data[1][4];
		if (ratio >= 0.2)
		{
			sensor_calibration_data[1][0] = (sensor_calibration_data[1][0] + sensor_calibration_data[1][3])/2;
			sensor_calibration_data[1][1] = (sensor_calibration_data[1][1] + sensor_calibration_data[1][4])/2;
			sensor_calibration_data[1][2] = (sensor_calibration_data[1][2] + sensor_calibration_data[1][5])/2;
		}
		
		printf("===>ratioR %f\n", ratio);
#endif

    	// g gain
    	if (sensor_calibration_data[0][1] > sensor_calibration_data[1][1])
    	{
    		GGain[1] = (0x400*sensor_calibration_data[0][1]*awb_convert_gain)/(sensor_calibration_data[1][1]);
    	}
    	else
    	{
    		GGain[0] = (0x400*sensor_calibration_data[1][1]*awb_convert_gain)/(sensor_calibration_data[0][1]);
    	}

    	// r gain
    	if (sensor_calibration_data[0][0] > sensor_calibration_data[1][0])
    	{
    		RGain[1] = (0x400*sensor_calibration_data[0][0]*awb_convert_gain)/(sensor_calibration_data[1][0]);
    	}
    	else
    	{
    		RGain[0] = (0x400*sensor_calibration_data[1][0]*awb_convert_gain)/(sensor_calibration_data[0][0]);
    	}		

    	// b gain
    	if (sensor_calibration_data[0][2] > sensor_calibration_data[1][2]) 
    	{
    		BGain[1] = (0x400*sensor_calibration_data[0][2]*awb_convert_gain)/(sensor_calibration_data[1][2]);
    	}
    	else
    	{
    		BGain[0] = (0x400*sensor_calibration_data[1][2]*awb_convert_gain)/(sensor_calibration_data[0][2]);
    	}
    }

#ifdef OTP_DBG_LOG
	printf("===>RGain_0 = %x, GGain_0 = %x, BGain_0 = %x, RGain_1 = %x, GGain_1 = %x, BGain_1 = %x\n", RGain[0], GGain[0], BGain[0], RGain[1], GGain[1], BGain[1]);
#if 0
	IMG_UINT8 ret_data[4];
	ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[0], 0x3820, &ret_data[0]);
	ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[0], 0x3821, &ret_data[1]);

    ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[1], 0x3820, &ret_data[2]);
	ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[1], 0x3821, &ret_data[3]);
#endif
///////////////////////
	if (0)
	{
	    IMG_UINT8 aui18FlipMirrorRegs[]=
	    {
	        0x38, 0x21, 0x00,
	        0x38, 0x20, 0x06
	    };

		ov683Dual_i2c_write8(psCam->i2c, aui18FlipMirrorRegs, sizeof(aui18FlipMirrorRegs));
		psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
	}
///////////////////////
	
	if (0)
	{
		IMG_UINT8 ret_data[18], x = 0;
		for (x = 0; x < 2; x++)
		{
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500C, &ret_data[0]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500D, &ret_data[1]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5012, &ret_data[2]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5013, &ret_data[3]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5018, &ret_data[4]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5019, &ret_data[5]);

			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500E, &ret_data[6]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500F, &ret_data[7]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5014, &ret_data[8]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5015, &ret_data[9]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501a, &ret_data[10]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501b, &ret_data[11]);

			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5010, &ret_data[12]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5011, &ret_data[13]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5016, &ret_data[14]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5017, &ret_data[15]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501c, &ret_data[16]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501d, &ret_data[17]);
			
			printf("===>origial sensor_%x long RGain = %x, middle RGain = %x, short RGain = %x,\n\
	long GGain = %x, middle GGain = %x, short GGain = %x\n\
	long BGain = %x, middle BGain = %x, short BGain = %x\n", 
				   x,
				   (ret_data[0]<<8)|ret_data[1], (ret_data[2]<<8)|ret_data[3], (ret_data[4]<<8)|ret_data[5], 
				   (ret_data[6]<<8)|ret_data[7], (ret_data[8]<<8)|ret_data[9], (ret_data[10]<<8)|ret_data[11],
				   (ret_data[12]<<8)|ret_data[13], (ret_data[14]<<8)|ret_data[15], (ret_data[16]<<8)|ret_data[17]);
		}
	}
#endif

#if 0
	ui8Regs_L[2] = RGain_L>>8; ui8Regs_L[5] = RGain_L&0xFF;
	ui8Regs_L[8] = RGain_L>>8; ui8Regs_L[11] = RGain_L&0xFF;
	ui8Regs_L[14] = RGain_L>>8; ui8Regs_L[17] = RGain_L&0xFF;

	ui8Regs_L[20] = GGain_L>>8; ui8Regs_L[23] = GGain_L&0xFF;
	ui8Regs_L[26] = GGain_L>>8; ui8Regs_L[29] = GGain_L&0xFF;
	ui8Regs_L[32] = GGain_L>>8; ui8Regs_L[35] = GGain_L&0xFF;

	ui8Regs_L[38] = BGain_L>>8; ui8Regs_L[41] = BGain_L&0xFF;
	ui8Regs_L[44] = BGain_L>>8; ui8Regs_L[47] = BGain_L&0xFF;
	ui8Regs_L[50] = BGain_L>>8; ui8Regs_L[53] = BGain_L&0xFF;

	ui8Regs_R[2] = RGain_R>>8; ui8Regs_R[5] = RGain_R&0xFF;
	ui8Regs_R[8] = RGain_R>>8; ui8Regs_R[11] = RGain_R&0xFF;
	ui8Regs_R[14] = RGain_R>>8; ui8Regs_R[17] = RGain_R&0xFF;

	ui8Regs_R[20] = GGain_R>>8; ui8Regs_R[23] = GGain_R&0xFF;
	ui8Regs_R[26] = GGain_R>>8; ui8Regs_R[29] = GGain_R&0xFF;
	ui8Regs_R[32] = GGain_R>>8; ui8Regs_R[35] = GGain_R&0xFF;

	ui8Regs_R[38] = BGain_R>>8; ui8Regs_R[41] = BGain_R&0xFF;
	ui8Regs_R[44] = BGain_R>>8; ui8Regs_R[47] = BGain_R&0xFF;
	ui8Regs_R[50] = BGain_R>>8; ui8Regs_R[53] = BGain_R&0xFF;
#else
    ui8Regs[0][2] = (RGain[0]>>8); ui8Regs[0][5] = (RGain[0]&0xFF);
    ui8Regs[0][8] = (GGain[0]>>8); ui8Regs[0][11] = (GGain[0]&0xFF);
    ui8Regs[0][14] = (BGain[0]>>8); ui8Regs[0][17] = (BGain[0]&0xFF);

    ui8Regs[1][2] = RGain[1]>>8; ui8Regs[1][5] = RGain[1]&0xFF;
    ui8Regs[1][8] = GGain[1]>>8; ui8Regs[1][11] = GGain[1]&0xFF;
    ui8Regs[1][14] = BGain[1]>>8; ui8Regs[1][17] = BGain[1]&0xFF;

#endif

	ov683Dual_sensor_i2c_write8(psCam->i2c, sensor_i2c_addr[0], ui8Regs[0], sizeof(ui8Regs[0]));
	ov683Dual_sensor_i2c_write8(psCam->i2c, sensor_i2c_addr[1], ui8Regs[1], sizeof(ui8Regs[1]));

#ifdef OTP_DBG_LOG
	if (0)
	{
		IMG_UINT8 ret_data[18], x = 0;
		for (x = 0; x < 2; x++)
		{
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500C, &ret_data[0]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500D, &ret_data[1]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5012, &ret_data[2]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5013, &ret_data[3]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5018, &ret_data[4]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5019, &ret_data[5]);

			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500E, &ret_data[6]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x500F, &ret_data[7]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5014, &ret_data[8]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5015, &ret_data[9]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501a, &ret_data[10]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501b, &ret_data[11]);

			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5010, &ret_data[12]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5011, &ret_data[13]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5016, &ret_data[14]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x5017, &ret_data[15]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501c, &ret_data[16]);
			ov683Dual_i2c_read8(psCam->i2c, sensor_i2c_addr[x], 0x501d, &ret_data[17]);
			
			printf("===>new sensor_%x long RGain = %x, middle RGain = %x, short RGain = %x,\n\
	long GGain = %x, middle GGain = %x, short GGain = %x\n\
	long BGain = %x, middle BGain = %x, short BGain = %x\n", 
				   x,
				   (ret_data[0]<<8)|ret_data[1], (ret_data[2]<<8)|ret_data[3], (ret_data[4]<<8)|ret_data[5], 
				   (ret_data[6]<<8)|ret_data[7], (ret_data[8]<<8)|ret_data[9], (ret_data[10]<<8)|ret_data[11],
				   (ret_data[12]<<8)|ret_data[13], (ret_data[14]<<8)|ret_data[15], (ret_data[16]<<8)|ret_data[17]);
		}
    }
#endif

    if (1)
    {
        int x = 0;
        for (x = 0; x < CALIBR_REG_CNT - 2; x += 3)
        {
            sensor_calibration_data[0][x] = (RGain[0]*sensor_calibration_data[0][x])/0x400;
            sensor_calibration_data[1][x] = (RGain[1]*sensor_calibration_data[1][x])/0x400;
#if 0 //def OTP_DBG_LOG
			printf("R[%d]=%d,%d\n", x, sensor_calibration_data[0][x], sensor_calibration_data[1][x]);
#endif

        }

        for (x = 1; x < CALIBR_REG_CNT - 2; x += 3)
        {
            sensor_calibration_data[0][x] = (GGain[0]*sensor_calibration_data[0][x])/0x400;
            sensor_calibration_data[1][x] = (GGain[1]*sensor_calibration_data[1][x])/0x400;
#if 0 //def OTP_DBG_LOG
			printf("G[%d]=%d,%d\n", x, sensor_calibration_data[0][x], sensor_calibration_data[1][x]);
#endif
        }
        
        for (x = 2; x < CALIBR_REG_CNT - 2; x += 3)
        {
            sensor_calibration_data[0][x] = (BGain[0]*sensor_calibration_data[0][x])/0x400;
            sensor_calibration_data[1][x] = (BGain[1]*sensor_calibration_data[1][x])/0x400;
#if 0 //def OTP_DBG_LOG
			printf("B[%d]=%d,%d\n", x, sensor_calibration_data[0][x], sensor_calibration_data[1][x]);
#endif

        }
    }
	//ov683Dual_i2c_write8(psCam->i2c, ui8Regs, sizeof(ui8Regs));
#endif

	return IMG_SUCCESS;
}

IMG_RESULT OV683Dual_exchange_calib_direction(SENSOR_HANDLE hHandle, int flag)
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

#endif

/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#ifdef INFOTM_SUPPORT_DAUL_SENSOR_AE_PATCH 
static IMG_RESULT OV683Dual_GetDualGain(SENSOR_HANDLE hHandle, double *pflGain, IMG_UINT8 ui8Context, IMG_UINT8 ui8SensorIndex)
{
    OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain=psCam->flDualGain[ui8SensorIndex];
    return IMG_SUCCESS;
}

static IMG_RESULT OV683Dual_SetDualGainAndExposure(SENSOR_HANDLE hHandle, double flGain[2], IMG_UINT32 ui32Exposure[2], IMG_UINT8 ui8Context)
{
	static int runing_flag = 0;
	IMG_UINT32 ui32Lines;
	IMG_UINT32 n, L = 3;

	IMG_UINT8 ui8Regs[2][18] =
	{
		{
			0x35, 0x00, 0x00, // Exposure register
			0x35, 0x01, 0x2E, //
			0x35, 0x02, 0x80, //

			0x35, 0x07, 0x00, // Gain register
			0x35, 0x08, 0x00, //
			0x35, 0x09, 0x00, //	
		}, 
		{
			0x35, 0x00, 0x00, // Exposure register
			0x35, 0x01, 0x2E, //
			0x35, 0x02, 0x80, //

			0x35, 0x07, 0x00, // Gain register
			0x35, 0x08, 0x00, //
			0x35, 0x09, 0x00, //	
		}
	};

	OV683DualCAM_STRUCT *psCam = container_of(hHandle, OV683DualCAM_STRUCT, sFuncs);

	if (!psCam->psSensorPhy)
	{
		LOG_ERROR("sensor not initialised\n");
		runing_flag = 0;
		return IMG_ERROR_NOT_INITIALISED;
	}

	while (runing_flag == 1)
	{
		//printf("====>waiting update AE...\n");
		usleep(5*1000);
	}
	runing_flag = 1;
	//01.Exposure part
	//
	psCam->ui32Exposure = ui32Exposure;

	ui32Lines = psCam->ui32Exposure / psCam->currMode.dbExposureMin;

//	if (ui32Lines > psCam->currMode.ui16VerticalTotal - 4)
//		ui32Lines = psCam->currMode.ui16VerticalTotal - 4;

	ui8Regs[5-L] = (ui32Lines >> 12) & 0xf;
	ui8Regs[8-L] = (ui32Lines >> 4) & 0xff;
	ui8Regs[11-L] = (ui32Lines << 4) & 0xff;
	LOG_DEBUG("Exposure Val %x\n",(psCam->ui32Exposure/psCam->currMode.dbExposureMin));

	//02.Gain part
	//
	psCam->flGain = flGain;

	if (flGain > aec_min_gain)
	{
		n = (IMG_UINT32)floor((flGain - 1) * 16);
	}
	n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

	ui8Regs[14-L] = 0;
	ui8Regs[17-L] = (aec_long_gain_val[n] >> 8) & 0xff;
	ui8Regs[20-L] = aec_long_gain_val[n] & 0xff;
	//LOG_DEBUG
	
    printf("==>gain %f n=%u from %lf\n", flGain, n, (flGain - 1) * 16);

	//03.check the new gain & exposure is the same as the last set or not.
	//
	if ((g_OV683Dual_ui8LinesValHiBackup == ui8Regs[5-L]) &&
		(g_OV683Dual_ui8LinesValMidBackup == ui8Regs[8-L]) &&
		(g_OV683Dual_ui8LinesValLoBackup == ui8Regs[11-L]) &&
		(g_OV683Dual_ui8GainValHiBackup == ui8Regs[14-L]) &&	
		(g_OV683Dual_ui8GainValMidBackup == ui8Regs[17-L]) &&
		(g_OV683Dual_ui8GainValLoBackup == ui8Regs[20-L]) ) 
	{
		//printf("The same gain and exposure as last.\n");
		//printf("ui32Lines=%d, flGain=%f\n", ui32Lines, flGain);
		runing_flag = 0;
		return IMG_SUCCESS;
	}

	g_OV683Dual_ui8LinesValHiBackup = ui8Regs[5-L];
	g_OV683Dual_ui8LinesValMidBackup = ui8Regs[8-L];
	g_OV683Dual_ui8LinesValLoBackup = ui8Regs[11-L];
	g_OV683Dual_ui8GainValHiBackup = ui8Regs[14-L];
	g_OV683Dual_ui8GainValMidBackup = ui8Regs[17-L];
	g_OV683Dual_ui8GainValLoBackup = ui8Regs[20-L];

	//04.set to sensor
	//
	{
		ov683Dual_i2c_write8_dual_sensor(psCam->i2c, ui8Regs, sizeof(ui8Regs));
		if (xx >= SKIP_CNT)
		{
			IMG_UINT32 k = 0, loops = 0;
			IMG_UINT8 ret_data = 0, ret_data1 = 0, ret_data2 = 0;
		}
	}
    runing_flag = 0;
    return IMG_SUCCESS;
}
#endif
/////////////////////////////////////////////////////////////
#endif //INFOTM_ISP

