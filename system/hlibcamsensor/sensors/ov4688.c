/**
 ******************************************************************************
@file ov4688.c

@brief OV4688 camera implementation

To add a new mode declare a new array containing 8b values:
- 2 first values are addresses
- last 8b is the value

The array MUST finish with STOP_REG, STOP_REG, STOP_REG so that the size can
be computed by the OV4688_GetRegisters()

The array can have delay of X ms using DELAY_REG, X, DELAY_REG

Add the array to ov4688_modes[] with the relevant flipping and a size of 0

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

#include "sensors/ov4688.h"

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

#ifdef INFOTM_ISP
static IMG_RESULT OV4688_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag); //The function is not support now.
static IMG_RESULT OV4688_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
#endif //INFOTM_ISP


/* disabled as i2c drivers locks up if device is not present */
#define NO_DEV_CHECK

#define LOG_TAG "OV4688_SENSOR"
#include <felixcommon/userlog.h>

/** @ the setup for mode does not work if enabled is not call just after
 * - temporarily do the whole of the setup in in enable function
 * - can remove that define and its checking once fixed
 */
// if defined writes all registers at enable time instead of configure
#define DO_SETUP_IN_ENABLE

#define OV4688_WRITE_ADDR (0x6C >> 1)
#define OV4688_READ_ADDR (0x6D >> 1)
#define AUTOFOCUS_WRITE_ADDR (0x18 >> 1)
#define AUTOFOCUS_READ_ADDR (0x19 >> 1)

#define OV4688_SENSOR_VERSION "not-verified"

#define OV4688_CHIP_VERSION 0x4688

// fake register value to interpret next value as delay in ms
#define DELAY_REG 0xff
// not a real register - marks the end of register array
#define STOP_REG 0xfe

/** @brief sensor number of IMG PHY */
#define OV4688_PHY_NUM 1

/*
 * Choice:
 * copy of the registers to apply because we want to apply them all at
 * once because some registers need 2 writes while others don't (may have
 * an effect on stability but was not showing in testing)
 * 
 * If not defined will apply values register by register and not need a copy
 * of the registers for the active mode.
 */
//#define OV4688_COPY_REGS

// uncomment to write i2c commands to file
//#define ov4688_debug "ov4688_write.txt"

#ifdef WIN32 // on windows we do not need to sleep to wait for the bus
static void usleep(int t)
{
    (void)t;
}
#endif

typedef struct _OV4688cam_struct
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
     * if OV4688_COPY_REGS is defined:
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

    IMG_UINT32 ui32Exposure;
    double flGain;
    IMG_UINT16 ui16CurrentFocus;
    IMG_UINT8 imager;

    int i2c;
    SENSOR_PHY *psSensorPhy;
}OV4688CAM_STRUCT;

static void OV4688_ApplyMode(OV4688CAM_STRUCT *psCam);
static void OV4688_ComputeGains(double flGain, IMG_UINT8 *gainSequence);
static void OV4688_ComputeExposure(IMG_UINT32 ui32Exposure,
    IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureSequence);
static IMG_RESULT OV4688_GetModeInfo(OV4688CAM_STRUCT *psCam,
    IMG_UINT16 ui16Mode, SENSOR_MODE *info);
static IMG_UINT16 OV4688_CalculateFocus(const IMG_UINT16 *pui16DACDist,
        const IMG_UINT16 *pui16DACValues, IMG_UINT8 ui8Entries,
        IMG_UINT16 ui16Requested);
// used before implementation therefore declared here
static IMG_RESULT OV4688_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus);

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
static const double aec_max_gain = 16.0;
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
static const unsigned int ov4688_focus_dac_min = 50;

// maximum focus in millimetres, if >= then focus is infinity
static const unsigned int ov4688_focus_dac_max = 600;

// focus values for the ov4688_focus_dac_dist
static const IMG_UINT16 ov4688_focus_dac_val[] = {
    0x3ff, 0x180, 0x120, 0x100, 0x000
};

// distances in millimetres for the ov4688_focus_dac_val
static const IMG_UINT16 ov4688_focus_dac_dist[] = {
    50, 150, 300, 500, 600
};

//
// modes declaration
//

static const IMG_UINT8 mode720p[] =
{
    0x01, 0x03, 0x01, // SW RESET
    0x36, 0x38, 0x00, // H_CROP_START
    0x03, 0x00, 0x00,
    0x03, 0x02, 0x2a,
    0x03, 0x04, 0x03,
    0x03, 0x0b, 0x00,
    0x03, 0x0d, 0x1e,
    0x03, 0x0e, 0x04,
    0x03, 0x0f, 0x01,
    0x03, 0x12, 0x01,
    0x03, 0x1e, 0x00,
    0x30, 0x00, 0x20,
    0x30, 0x02, 0x00,
    0x30, 0x18, 0x72,
    0x30, 0x20, 0x93,
    0x30, 0x21, 0x03,
    0x30, 0x22, 0x01,
    0x30, 0x31, 0x0a,
    0x33, 0x05, 0xf1,
    0x33, 0x07, 0x04,
    0x33, 0x09, 0x29,
    //0x35, 0x00, 0x00, // exposure value, this is written further down
    //0x35, 0x01, 0x5F,  // exposure value this is written further down
    //0x35, 0x02, 0x80,  // exposure value, this is written further down
    0x35, 0x03, 0x04,  // AEC MANUAL set sensor gain and no delay
    0x35, 0x04, 0x00,
    0x35, 0x05, 0x00,
    0x35, 0x06, 0x00,
    //0x35, 0x07, 0x00, // gain in group 0
    //0x35, 0x08, 0x00,
    //0x35, 0x09, 0x80,
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
    0x38, 0x00, 0x00,
    0x38, 0x01, 0x08,
    0x38, 0x02, 0x00,
    0x38, 0x03, 0x04,
    0x38, 0x04, 0x0a,
    0x38, 0x05, 0x97,
    0x38, 0x06, 0x05,
    0x38, 0x07, 0xfb,
    0x38, 0x08, 0x0a,
    0x38, 0x09, 0x80,
    0x38, 0x0a, 0x05,
    0x38, 0x0b, 0xf0,
    0x38, 0x0c, 0x03,
    0x38, 0x0d, 0x5c,
    0x38, 0x0e, 0x06,
    0x38, 0x0f, 0x12,
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
    0x38, 0x41, 0x02,
    0x38, 0x46, 0x08,
    0x38, 0x47, 0x07,
    0x3d, 0x85, 0x36,
    0x3d, 0x8c, 0x71,
    0x3d, 0x8d, 0xcb,
    0x3f, 0x0a, 0x00,
    0x40, 0x00, 0x71,
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
    0x40, 0x22, 0x07,
    0x40, 0x23, 0xcf,
    0x40, 0x24, 0x09,
    0x40, 0x25, 0x60,
    0x40, 0x26, 0x09,
    0x40, 0x27, 0x6f,
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
    0x45, 0x03, 0x02,
    0x46, 0x01, 0x04,
    0x48, 0x00, 0x04,
    0x48, 0x13, 0x08,
    0x48, 0x1f, 0x40,
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
    0x31, 0x05, 0x31,
    0x30, 0x1a, 0xf9,
    DELAY_REG, 15, DELAY_REG,
    0x35, 0x08, 0x07,
    0x48, 0x4b, 0x05,
    0x48, 0x05, 0x03,
    0x36, 0x01, 0x01,
    // ;; From Re,solution sheet
    0x38, 0x00, 0x02,
    0x38, 0x01, 0xC8,
    0x38, 0x02, 0x01,
    0x38, 0x03, 0x94,
    0x38, 0x04, 0x07,
    0x38, 0x05, 0xD7,
    0x38, 0x06, 0x04,
    0x38, 0x07, 0x6B,
    0x38, 0x08, 0x05,
    0x38, 0x09, 0x00,
    0x38, 0x0A, 0x02,
    0x38, 0x0B, 0xD0,
    0x38, 0x0C, 0x14,
    0x38, 0x0D, 0xC0,
    0x38, 0x0E, 0x02,
    0x38, 0x0F, 0xF4,
    0x38, 0x10, 0x00,
    0x38, 0x11, 0x08,
    0x38, 0x12, 0x00,
    0x38, 0x13, 0x04,
    0x40, 0x20, 0x00,
    0x40, 0x21, 0x10,
    0x40, 0x22, 0x03,
    0x40, 0x23, 0x93,
    0x40, 0x24, 0x04,
    0x40, 0x25, 0xC0,
    0x40, 0x26, 0x04,
    0x40, 0x27, 0xD0,
    0x46, 0x00, 0x00,
    0x46, 0x01, 0x4F,
    //;;  From Clock, adjust sheet
    0x30, 0x18, 0x12,
    0x30, 0x19, 0x0e,
    0x30, 0x31, 0x0a,
    0x03, 0x00, 0x04,
    0x03, 0x01, 0x00,
    0x03, 0x02, 0x50, // PLL multiplier
    0x03, 0x03, 0x03,
    0x03, 0x04, 0x03,
    0x03, 0x05, 0x01,
    0x03, 0x06, 0x01,
    0x03, 0x0A, 0x00,
    0x03, 0x0B, 0x00,
    0x03, 0x0C, 0x00,
    0x03, 0x0D, 0x1E,
    0x03, 0x0E, 0x04,
    0x03, 0x0F, 0x03,
    0x03, 0x11, 0x00,
    0x03, 0x12, 0x01,
    0x38, 0x0E, 0x02,
    0x38, 0x0F, 0xF4,
    0x46, 0x00, 0x00,
    0x46, 0x01, 0x4F,
    0x48, 0x37, 0x64, //; Adjust for UI = 6.25ns
    0x01, 0x00, 0x01,
    0x31, 0x05, 0x11,
    //DELAY_REG, 10, DELAY_REG, // delay 10ms
    0x30, 0x1a, 0xf1,
    0x48, 0x05, 0x00,
    0x30, 0x1a, 0xf0,
    0x32, 0x08, 0x00, // start hold group 0
    0x30, 0x2a, 0x00,
    0x30, 0x2a, 0x00,
    0x30, 0x2a, 0x00,
    0x30, 0x2a, 0x00,
    0x30, 0x2a, 0x00,
    0x36, 0x01, 0x00,
    0x36, 0x38, 0x00,
    0x35, 0x00, 0x00,
    0x35, 0x01, 0x2a,
    0x35, 0x02, 0x60,
    0x35, 0x07, 0x00,
    0x35, 0x08, 0x03,
    0x35, 0x09, 0x74,
    0x32, 0x08, 0x10, // end hold group 0
    STOP_REG, STOP_REG, STOP_REG,
};

struct ov4688_mode {
    IMG_UINT8 ui8Flipping;
    const IMG_UINT8 *modeRegisters;
    IMG_UINT32 nRegisters; // initially 0 then computed the 1st time
};

struct ov4688_mode ov4688_modes[] = {
    { SENSOR_FLIP_BOTH, mode720p, 0 },
};

static const IMG_UINT8* OV4688_GetRegisters(OV4688CAM_STRUCT *psCam,
    unsigned int uiMode, unsigned int *nRegisters)
{
    if (uiMode < sizeof(ov4688_modes) / sizeof(struct ov4688_mode))
    {
        const IMG_UINT8 *registerArray = ov4688_modes[uiMode].modeRegisters;
        if (0 == ov4688_modes[uiMode].nRegisters)
        {
            // should be done only once per mode
            int i = 0;
            while (STOP_REG != registerArray[3 * i])
            {
                i++;
            }
            ov4688_modes[uiMode].nRegisters = i;
        }
        *nRegisters = ov4688_modes[uiMode].nRegisters;
        
        return registerArray;
    }
    // if it is an invalid mode returns NULL

    return NULL;
}

static IMG_RESULT ov4688_i2c_write8(int i2c, const IMG_UINT8 *data,
    unsigned int len)
{
#if !file_i2c
    unsigned i;
#ifdef ov4688_debug
    FILE *f=NULL;
    static unsigned nwrite=0;
#endif

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
    if (ioctl(i2c, I2C_SLAVE, OV4688_WRITE_ADDR))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }
#ifdef ov4688_debug
    f = fopen(ov4688_debug, "a");
    fprintf(f, "write %d\n", nwrite++);
#endif

    for (i = 0; i < len; data += 3, i += 3)
    {
        int write_len = 0;

        if(DELAY_REG == data[0])
        {
            usleep((int)data[1]*1000);
#ifdef ov4688_debug
        fprintf(f, "delay %dms\n", data[1]);
#endif
            continue;
        }

        write_len = write(i2c, data, 3);
#ifdef ov4688_debug
        fprintf(f, "0x%x 0x%x 0x%x\n", data[0], data[1], data[2]);
#endif

        if (write_len != 3)
        {
            LOG_ERROR("Failed to write I2C data! write_len = %d, index = %d\n",
                    write_len, i);
#ifdef ov4688_debug
            fclose(f);
#endif
            return IMG_ERROR_BUSY;
        }
    }
#ifdef ov4688_debug
    fclose(f);
#endif
#endif
    return IMG_SUCCESS;
}

static IMG_RESULT ov4688_i2c_read8(int i2c, IMG_UINT16 offset,
    IMG_UINT8 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
    
    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE, OV4688_READ_ADDR))
    {
        LOG_ERROR("Failed to write I2C read address!\n");
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
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif  /* file_i2c */
}

static IMG_RESULT ov4688_i2c_read16(int i2c, IMG_UINT16 offset,
    IMG_UINT16 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
    
    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE, OV4688_READ_ADDR))
    {
        LOG_ERROR("Failed to write I2C read address!\n");
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
    
    ret = read(i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_ERROR("Failed to read I2C at 0x%x\n", offset);
        return IMG_ERROR_FATAL;
    }

    *data = (buff[0] << 8) | buff[1];

    return IMG_SUCCESS;
#else
    return IMG_ERROR_NOT_SUPPORTED;
#endif  /* file_i2c */
}

static IMG_RESULT OV4688_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    IMG_RESULT ret;
    IMG_UINT16 v = 0;
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_ASSERT(strlen(OV4688_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(OV4688_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);

    psInfo->eBayerOriginal = MOSAIC_BGGR;
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
    sprintf(psInfo->pszSensorName, OV4688_SENSOR_INFO_NAME);
    
#ifndef NO_DEV_CHECK
    ret = ov4688_i2c_read16(psCam->i2c, REG_SC_CMMN_CHIP_ID_0, &v);
#else
    ret = 1;
#endif
    if (!ret)
    {
        sprintf(psInfo->pszSensorVersion, "0x%x", v);
    }
    else
    {
        sprintf(psInfo->pszSensorVersion, OV4688_SENSOR_VERSION);
    }

    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 6040;
    // bitdepth is a mode information
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = 0;
    psInfo->bBackFacing = IMG_TRUE;
    // other information should be filled by Sensor_GetInfo()

    LOG_DEBUG("provided BayerOriginal=%d\n", psInfo->eBayerOriginal);
    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    /*if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    ret = OV4688_GetModeInfo(psCam, nIndex, psModes);
    return ret;
}

static IMG_RESULT OV4688_GetState(SENSOR_HANDLE hHandle,
        SENSOR_STATUS *psStatus)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

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
    }
    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
          IMG_UINT8 ui8Flipping)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);
    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = NULL;
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    modeReg = OV4688_GetRegisters(psCam, nMode, &nRegisters);

    if (modeReg)
    {
        if((ui8Flipping&(ov4688_modes[nMode].ui8Flipping))!=ui8Flipping)
        {
            LOG_ERROR("sensor mode does not support selected flipping "\
                "0x%x (supports 0x%x)\n",
                ui8Flipping, ov4688_modes[nMode].ui8Flipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }

#if defined(OV4688_COPY_REGS)
        if (psCam->currModeReg)
        {
            IMG_FREE(psCam->currModeReg);
        }
#endif
        psCam->nRegisters = nRegisters;
        
#if defined(OV4688_COPY_REGS)
        /* we want to apply all the registers at once - need to copy them */
        psCam->currModeReg = (IMG_UINT8*)IMG_MALLOC(psCam->nRegisters * 3);
        IMG_MEMCPY(psCam->currModeReg, modeReg, psCam->nRegisters * 3);
#else
        /* no need to copy the register as we will apply them one by one */
        psCam->currModeReg = (IMG_UINT8*)modeReg;
#endif

        ret = OV4688_GetModeInfo(psCam, nMode, &(psCam->currMode));
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
        OV4688_ApplyMode(psCam);
#endif

        psCam->ui16CurrentFocus = ov4688_focus_dac_min; // minimum focus
        psCam->ui16CurrentMode = nMode;
        psCam->ui8CurrentFlipping = ui8Flipping;
        return IMG_SUCCESS;
    }


    return IMG_ERROR_INVALID_PARAMETERS;
}

static IMG_RESULT OV4688_Enable(SENSOR_HANDLE hHandle)
{
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
#endif
        0x32, 0x0b, 0x0, // GRP_SWCTRL select group 0
        0x32, 0x08, 0xe0 // GROUP_ACCESS quick launch group 0
    };
    /*    {    0x32, 0x08, 0x10,
        0x32, 0x08, 0xa0,
        };*/
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling OV4688 Camera!\n");
    psCam->bEnabled=IMG_TRUE;

    psCam->psSensorPhy->psGasket->bParallel = IMG_FALSE;
    psCam->psSensorPhy->psGasket->uiGasket = psCam->imager;
    SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->currMode.ui8MipiLanes,
        OV4688_PHY_NUM);

#ifdef DO_SETUP_IN_ENABLE
    // because we didn't do the actual setup in setMode we have to do it now
    // it may start the sensor so we have to do it after the gasket enable
    /// @ fix conversion from float to uint
    OV4688_ApplyMode(psCam);
#endif
    ov4688_i2c_write8(psCam->i2c, aui18EnableRegs, sizeof(aui18EnableRegs));
//#endif

    OV4688_SetFocus(hHandle, psCam->ui16CurrentFocus);

    LOG_DEBUG("camera enabled\n");
    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_Disable(SENSOR_HANDLE hHandle)
{
    static const IMG_UINT8 disableRegs[]={
        0x01, 0x00, 0x00, // SC_CTRL0100 set to sw standby
        DELAY_REG, 200, DELAY_REG, // 200ms delay
        0x30, 0x1a, 0xf9, // SC_CMMN_CLKRST0 as recommended by omnivision
        0x48, 0x05, 0x03, // MIPI_CTRL_05 retime manu/sel to manual
//        DELAY_REG, 10, DELAY_REG // 10ms delay
    };

    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(psCam->bEnabled)
    {
        int delay = 0;
        LOG_INFO("Disabling OV4688 camera\n");
        psCam->bEnabled = IMG_FALSE;

        ov4688_i2c_write8(psCam->i2c, disableRegs, sizeof(disableRegs));

        // delay of a frame period to ensure sensor has stopped
        // flFrameRate in Hz, change to MHz to have micro seconds
        delay = (int)floor((1.0 / psCam->currMode.flFrameRate) * 1000 * 1000);
        LOG_DEBUG("delay of %uus between disabling sensor/phy\n", delay);
        
        usleep(delay);

        SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, OV4688_PHY_NUM);
    }

    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_Destroy(SENSOR_HANDLE hHandle)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_DEBUG("Destroying OV4688 camera\n");
    /* remember to free anything that might have been allocated dynamically
     * (like extended params...)*/
    if(psCam->bEnabled)
    {
        OV4688_Disable(hHandle);
    }
    SensorPhyDeinit(psCam->psSensorPhy);
    if (psCam->currModeReg)
    {
#if defined(OV4688_COPY_REGS)
        IMG_FREE(psCam->currModeReg);
#endif
        psCam->currModeReg = NULL;
    }
#if !file_i2c
    close(psCam->i2c);
#endif
    IMG_FREE(psCam);
    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_GetFocusRange(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Min, IMG_UINT16 *pui16Max)
{
    /*OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    *pui16Min = ov4688_focus_dac_min;
    *pui16Max = ov4688_focus_dac_max;
    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_GetCurrentFocus(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Current)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

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
IMG_UINT16 OV4688_CalculateFocus(const IMG_UINT16 *pui16DACDist,
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
IMG_RESULT OV4688_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus)
{
#if !file_i2c
    unsigned i;
#endif
    IMG_UINT8 ui8Regs[4];
    IMG_UINT16 ui16DACVal;

    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui16CurrentFocus = ui16Focus;
    if (psCam->ui16CurrentFocus >= ov4688_focus_dac_max)
    {
        // special case the infinity as it doesn't fit in with the rest
        ui16DACVal = 0;
    }
    else
    {
        ui16DACVal = OV4688_CalculateFocus(ov4688_focus_dac_dist,
            ov4688_focus_dac_val,
            sizeof(ov4688_focus_dac_dist) / sizeof(IMG_UINT16),
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
#endif

    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_GetExposureRange(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

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

static IMG_RESULT OV4688_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current=psCam->ui32Exposure;
    return IMG_SUCCESS;
}

void OV4688_ComputeExposure(IMG_UINT32 ui32Exposure,
    IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureRegs)
{
    IMG_UINT32 exposureRatio = (ui32Exposure)/ui32ExposureMin;

    LOG_DEBUG("Exposure Val 0x%x\n", exposureRatio);

    exposureRegs[2]=((exposureRatio)>>12) & 0xf;
    exposureRegs[5]=((exposureRatio)>>4) & 0xff;
    exposureRegs[8]=((exposureRatio)<<4) & 0xff;
}

static IMG_RESULT OV4688_SetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
    IMG_UINT8 exposureRegs[]={
        0x32, 0x08, 0x02, // GROUP_ACCESS start hold of group 2
        0x35, 0x00, 0x00, //
        0x35, 0x01, 0x2E, //
        0x35, 0x02, 0x80, //
        0x32, 0x08, 0x12, // end hold of group 2 register writes
        0x32, 0x0b, 0x00, // set quick manual mode
        0x32, 0x08, 0xe2, // quick launch group 2
    };

    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui32Exposure=ui32Current;

    if (psCam->bEnabled || 1)
    {
        OV4688_ComputeExposure(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin,
            &(exposureRegs[3]));

        ov4688_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));
    }
    return IMG_SUCCESS;
}

void OV4688_ComputeGains(double flGain, IMG_UINT8 *gainSequence)
{
    unsigned int n = 0; // if flGain <= 1.0
    if (flGain > aec_min_gain) {
        n = (unsigned int)floor((flGain - aec_min_gain) * aec_max_gain);
    }
    n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

    LOG_DEBUG("gain n=%u from %lf\n", n, (flGain-aec_min_gain)*aec_max_gain);
    gainSequence[2] = 0;
    gainSequence[5] = (aec_long_gain_val[n] >> 8) & 0xff;
    gainSequence[8] = aec_long_gain_val[n] & 0xff;
}

static IMG_RESULT OV4688_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
    IMG_UINT8 gainRegs[] = {
        0x32, 0x08, 0x01,  // start hold of group 1
        0x35, 0x07, 0x00,  //
        0x35, 0x08, 0x00,  //
        0x35, 0x09, 0x00,  //
        0x32, 0x08, 0x11,  // end hold of group 1 register writes
        0x32, 0x0b, 0x00,  // set quick manual mode
        0x32, 0x08, 0xe1,  // quick launch group 1
    };

    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->flGain = flGain;

    if (psCam->bEnabled || 1)
    {
        OV4688_ComputeGains(flGain, &(gainRegs[3]));

        ov4688_i2c_write8(psCam->i2c, gainRegs, sizeof(gainRegs));
    }

    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

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

static IMG_RESULT OV4688_GetGain(SENSOR_HANDLE hHandle, double *pflGain,
    IMG_UINT8 ui8Context)
{
    OV4688CAM_STRUCT *psCam = container_of(hHandle, OV4688CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain=psCam->flGain;
    return IMG_SUCCESS;
}

IMG_RESULT OV4688_Create(SENSOR_HANDLE *phHandle)
{
#if !file_i2c
    char i2c_dev_path[NAME_MAX];
    IMG_UINT16 chipV;
    IMG_RESULT ret;
    char adaptor[64];
#endif
    OV4688CAM_STRUCT *psCam;

    psCam=(OV4688CAM_STRUCT *)IMG_CALLOC(1, sizeof(OV4688CAM_STRUCT));
    if(!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    *phHandle=&(psCam->sFuncs);
    psCam->sFuncs.GetMode = OV4688_GetMode;
    psCam->sFuncs.GetState = OV4688_GetState;
    psCam->sFuncs.SetMode = OV4688_SetMode;
    psCam->sFuncs.Enable = OV4688_Enable;
    psCam->sFuncs.Disable = OV4688_Disable;
    psCam->sFuncs.Destroy = OV4688_Destroy;
    psCam->sFuncs.GetInfo = OV4688_GetInfo;

    psCam->sFuncs.GetCurrentGain = OV4688_GetGain;
    psCam->sFuncs.GetGainRange = OV4688_GetGainRange;
    psCam->sFuncs.SetGain = OV4688_SetGain;
    psCam->sFuncs.SetExposure = OV4688_SetExposure;
    psCam->sFuncs.GetExposureRange = OV4688_GetExposureRange;
    psCam->sFuncs.GetExposure = OV4688_GetExposure;

    psCam->sFuncs.GetCurrentFocus = OV4688_GetCurrentFocus;
    psCam->sFuncs.GetFocusRange = OV4688_GetFocusRange;
    psCam->sFuncs.SetFocus = OV4688_SetFocus;

#ifdef INFOTM_ISP
	psCam->sFuncs.SetFlipMirror = OV4688_SetFlipMirror;
	psCam->sFuncs.SetGainAndExposure = OV4688_SetGainAndExposure;
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
    psCam->imager = 0; // gasket to use - customer should change it if needed

#if !file_i2c
    if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), OV4688_WRITE_ADDR, adaptor))
    {
        LOG_ERROR("Failed to find I2C device!\n");
        IMG_FREE(psCam);
        *phHandle=NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    psCam->i2c = open(i2c_dev_path, O_RDWR);
    if (psCam->i2c < 0) {
        LOG_ERROR("Failed to open I2C device: \"%s\", err = %d\n",
            i2c_dev_path, psCam->i2c);
        IMG_FREE(psCam);
        *phHandle=NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

#ifndef NO_DEV_CHECK
    ret = ov4688_i2c_read16(psCam->i2c, REG_SC_CMMN_CHIP_ID_0, &chipV);
    if(ret || OV4688_CHIP_VERSION != chipV)
    {
        LOG_ERROR("Failed to ensure that the i2c device has a compatible "\
            "OV4688 sensor! ret=%d chip_version=0x%x (expect chip 0x%x)\n",
            ret, chipV, OV4688_CHIP_VERSION);
        close(psCam->i2c);
        IMG_FREE(psCam);
        *phHandle = NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
#endif

#ifdef ov4688_debug
    // clean file
    {
        FILE *f = fopen(ov4688_debug, "w");
        fclose(f);
    }
#endif
#endif

    // need i2c to have been found to read mode
    OV4688_GetModeInfo(psCam, psCam->ui16CurrentMode, &(psCam->currMode));

    psCam->psSensorPhy = SensorPhyInit();
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("Failed to create sensor phy!\n");
#if !file_i2c
        close(psCam->i2c);
#endif
        IMG_FREE(psCam);
        *phHandle = NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    LOG_DEBUG("Sensor initialised\n");
    return IMG_SUCCESS;
}

void OV4688_ApplyMode(OV4688CAM_STRUCT *psCam)
{
    unsigned i;
    // index for flipping register
    unsigned flipIndex0 = psCam->nRegisters * 3;
    // index for flipping register
    unsigned flipIndex1 = psCam->nRegisters * 3;
    // index for exposure register
    unsigned exposureIndex = psCam->nRegisters * 3;
    // index for gain register
    unsigned gainIndex = psCam->nRegisters * 3;
#if !defined(OV4688_COPY_REGS)
    IMG_UINT8 values[3];
#endif

    if (!psCam->currModeReg)
    {
        LOG_ERROR("current register modes not available!\n");
        return;
    }
        
    
    LOG_DEBUG("Writing I2C\n");

    for(i = 0 ; i < psCam->nRegisters * 3 ; i += 3)
    {
#if !defined(OV4688_COPY_REGS)
        values[0] = psCam->currModeReg[i+0];
        values[1] = psCam->currModeReg[i+1];
        values[2] = psCam->currModeReg[i+2];
#endif

        if (IS_REG(psCam->currModeReg, i, REG_FORMAT1))
        {
            flipIndex0 = i;
            if (psCam->ui8CurrentFlipping&SENSOR_FLIP_VERTICAL)
            {
#if defined(OV4688_COPY_REGS)
                psCam->currModeReg[i+2] = 6;
#else
                values[2] = 6;
#endif
            }
            else
            {
#if defined(OV4688_COPY_REGS)
                psCam->currModeReg[i+2] = 0;
#else
                values[2] = 0;
#endif
            }
        }
        if (IS_REG(psCam->currModeReg, i, REG_FORMAT2))
        {
            flipIndex1 = i;
            if (psCam->ui8CurrentFlipping&SENSOR_FLIP_HORIZONTAL)
            {
#if defined(OV4688_COPY_REGS)
                psCam->currModeReg[i+2] = 6;
#else
                values[2] = 6;
#endif

            }
            else
            {
#if defined(OV4688_COPY_REGS)
                psCam->currModeReg[i+2] = 0;
#else
                values[2] = 0;
#endif
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

#if !defined(OV4688_COPY_REGS)
        ov4688_i2c_write8(psCam->i2c, values, 3);
#endif
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
        OV4688_ComputeExposure(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin,
            &(mode1280x720[exposureIndex]));
    }

    // should be found and fit in array
    IMG_ASSERT(exposureIndex+8 < psCam->nRegisters * 3);
    {
        OV4688_ComputeGains(psCam->flGain, &(mode1280x720[gainIndex]));
    }
#endif
    
#if defined(OV4688_COPY_REGS)
    ov4688_i2c_write8(psCam->i2c, psCam->currModeReg, psCam->nRegisters * 3);
#endif
    
    LOG_DEBUG("Sensor ready to go\n");
    // setup the exposure setting information
}

IMG_RESULT OV4688_GetModeInfo(OV4688CAM_STRUCT *psCam, IMG_UINT16 ui16Mode,
    SENSOR_MODE *info)
{
    unsigned int n;
    IMG_RESULT ret = IMG_SUCCESS;
    
    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = OV4688_GetRegisters(psCam, ui16Mode,
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

    if (!modeReg)
    {
        LOG_ERROR("invalid mode %d\n", modeReg);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    nRegisters = ov4688_modes[ui16Mode].nRegisters;

    for (n = 0 ; n < nRegisters * 3 ; n += 3)
    {
        if (IS_REG(modeReg, n, REG_SC_CMMN_BIT_SEL))
        {
            sc_cmmm_bit_sel = modeReg[n + 2];
            LOG_DEBUG("sc_cmmm_bit_sel = 0x%x\n", sc_cmmm_bit_sel);
        }

        if (IS_REG(modeReg, n, REG_SC_CMMN_MIPI_SC_CTRL))
        {
            sc_cmmn_mipi_sc_ctrl = modeReg[n + 2];
            LOG_DEBUG("sc_cmmn_mipi_sc_ctrl = 0x%x\n", sc_cmmn_mipi_sc_ctrl);
        }

        if (IS_REG(modeReg, n, REG_TIMING_VTS_0))
        {
            vts[0] = modeReg[n + 2];
            LOG_DEBUG("vts[0] = 0x%x\n", vts[0]);
        }
        if (IS_REG(modeReg, n, REG_TIMING_VTS_1))
        {
            vts[1] = modeReg[n + 2];
            LOG_DEBUG("vts[1] = 0x%x\n", vts[1]);
        }

        if (IS_REG(modeReg, n, REG_TIMING_HTS_0))
        {
            hts[0] = modeReg[n + 2];
            LOG_DEBUG("hts[0]=0x%x\n", hts[0]);
        }
        if (IS_REG(modeReg, n, REG_TIMING_HTS_1))
        {
            hts[1] = modeReg[n + 2];
            LOG_DEBUG("hts[1]=0x%x\n", hts[1]);
        }

        if (IS_REG(modeReg, n, REG_H_OUTPUT_SIZE_0))
        {
            h_output[0] = modeReg[n + 2];
            LOG_DEBUG("h_output[0] = 0x%x\n", h_output[0]);
        }
        if (IS_REG(modeReg, n, REG_H_OUTPUT_SIZE_1))
        {
            h_output[1] = modeReg[n + 2];
            LOG_DEBUG("h_output[1] = 0x%x\n", h_output[1]);
        }

        if (IS_REG(modeReg, n, REG_V_OUTPUT_SIZE_0))
        {
            v_output[0] = modeReg[n + 2];
            LOG_DEBUG("v_output[0] = 0x%x\n", v_output[0]);
        }
        if (IS_REG(modeReg, n, REG_V_OUTPUT_SIZE_1))
        {
            v_output[1] = modeReg[n + 2];
            LOG_DEBUG("v_output[1] = 0x%x\n", v_output[1]);
        }

        if (IS_REG(modeReg, n, REG_PLL_CTRL_11))
        {
            pll2_predivp = modeReg[n + 2] + 1.0;
            LOG_DEBUG("pll2_predivp = 0x%x -> %lf\n",
                modeReg[n + 2], pll2_predivp);
        }

        if (IS_REG(modeReg, n, REG_PLL_CTRL_B))
        {
            pll2_prediv = pll_ctrl_b_val[modeReg[n + 2]];
            LOG_DEBUG("pll2_prediv = 0x%x -> %lf\n",
                modeReg[n + 2], pll2_prediv);
        }

        if (IS_REG(modeReg, n, REG_PLL_CTRL_C))
        {
            pll_ctrl_d[0] = modeReg[n + 2];
            LOG_DEBUG("pll_ctrl_d[0] = 0x%x\n", pll_ctrl_d[0]);
        }
        if (IS_REG(modeReg, n, REG_PLL_CTRL_D))
        {
            pll_ctrl_d[1] = modeReg[n + 2];
            LOG_DEBUG("pll_ctrl_d[1] = 0x%x\n", pll_ctrl_d[1]);
        }

        if (IS_REG(modeReg, n, REG_PLL_CTRL_F))
        {
            pll2_divsp = modeReg[n + 2] + 1.0;
            LOG_DEBUG("pll2_divsp = 0x%x -> %lf\n",
                modeReg[n + 2], pll2_divsp);
        }

        if (IS_REG(modeReg, n, REG_PLL_CTRL_E))
        {
            pll2_divs = pll_ctrl_c_val[modeReg[n + 2]];
            LOG_DEBUG("pll2_divs = 0x%x -> %lf\n", modeReg[n + 2], pll2_divs);
        }
    }

    pll2_mult = (double)((pll_ctrl_d[0] << 8) | pll_ctrl_d[1]);

    if (0.0 == pll2_predivp || 0.0 == pll2_mult || 0.0 == pll2_prediv
        || 0.0 == pll2_divsp || 0.0 == pll2_divs)
    {
        sclk = 120.0 * 1000 * 1000;
        LOG_WARNING("Did not find all PLL registers - assumes "\
            "sclk of %ld MHz\n", sclk);
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
    
    LOG_DEBUG("hts=%u vts=%u\n", hts_v, vts_v);
    LOG_DEBUG("trow=%lf frame_t=%u\n", trow, frame_t);

    info->ui16Width = h_output[1] | (h_output[0] << 8);
    info->ui16Height = v_output[1] | (v_output[0] << 8);
    info->ui16VerticalTotal = vts_v;
    
    info->ui8SupportFlipping = SENSOR_FLIP_BOTH;
    info->ui32ExposureMax = (IMG_UINT32)floor(trow * vts_v);
    info->ui32ExposureMin = (IMG_UINT32)floor(trow);
    info->flFrameRate = sclk / frame_t;
    
    info->ui8MipiLanes = ((sc_cmmn_mipi_sc_ctrl << 5) & 0x3) + 1;
    info->ui8BitDepth = (sc_cmmm_bit_sel & 0x1F);

    return IMG_SUCCESS;
}


#ifdef INFOTM_ISP
static IMG_RESULT OV4688_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{
	//Now not support.
	//..
	
    return IMG_SUCCESS;
}

static IMG_RESULT OV4688_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    OV4688_SetExposure(hHandle, ui32Exposure, ui8Context);
    OV4688_SetGain(hHandle, flGain, ui8Context);
    return IMG_SUCCESS;
}
#endif //INFOTM_ISP

