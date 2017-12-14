/**
 ******************************************************************************
@file ov2710.c

@brief OV2710 camera implementation

To add a new mode declare a new array containing 8b values:
- 2 first values are addresses
- last 8b is the value

The array MUST finish with STOP_REG, STOP_REG, STOP_REG so that the size can
be computed by the OV2710_GetRegisters()

The array can have delay of X ms using DELAY_REG, X, DELAY_REG

Add the array to ov2710_modes[] with the relevant flipping and a size of 0

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

#include "sensors/ov2710.h"

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


//#define SUPPORT_15FPS
//#define SUPPORT_20FPS
//#define SUPPORT_30FPS
//#define SUPPORT_4_LANE
#define SUPPORT_1_LANE

//#define BAND_SENSOR_DRIVER // added by linyun.xiong @2015-08-06

//#ifdef SUPPORT_1_LANE
//#define SUPPORT_30FPS
//#endif

#define ENABLE_GROUP
#define MIN_EXPOSURE_LINE  8
#if !defined(ZT201_PROJECT)// && !defined(ZT202_PROJECT)
#define FLIP_MIRROR_FIRST
#endif


static IMG_UINT32 g_OV2710_ui32LinesBackup = 0;
static IMG_UINT8 g_OV2710_ui8GainValHiBackup = 0xff;
static IMG_UINT8 g_OV2710_ui8GainValLoBackup = 0xff;

#ifdef BAND_SENSOR_DRIVER

//#include <linux/camera/ov2710_mipi.h>

#include <linux/ioctl.h>

#define OV2710_DEV_IOC_MAGIC  'o'

/* \u5b9a\u4e49\u547d\u4ee4 */
#define OV2710_DEV_PRINT_INFO   _IO(OV2710_DEV_IOC_MAGIC, 1)
#define OV2710_DEV_OPEN_SENSOR  _IO(OV2710_DEV_IOC_MAGIC, 2)
#define OV2710_DEV_CLOSE_SENSOR _IO(OV2710_DEV_IOC_MAGIC, 3)

#define ov2710_dev "/dev/ov2710_mipi"
#endif

static IMG_RESULT OV2710_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 
ui8Context);
static IMG_RESULT OV2710_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag);

/* disabled as i2c drivers locks up if device is not present */
#define NO_DEV_CHECK

#define LOG_TAG "OV2710_SENSOR"
#include <felixcommon/userlog.h>
static int sensor_fd = -1;


/** @ the setup for mode does not work if enabled is not call just after
 * - temporarily do the whole of the setup in in enable function
 * - can remove that define and its checking once fixed
 */
// if defined writes all registers at enable time instead of configure
#define DO_SETUP_IN_ENABLE

#define OV2710_WRITE_ADDR (0x6C >> 1)
#define OV2710_READ_ADDR (0x6D >> 1)
#define AUTOFOCUS_WRITE_ADDR (0x18 >> 1)
#define AUTOFOCUS_READ_ADDR (0x19 >> 1)

#define OV2710_SENSOR_VERSION "not-verified"

#define OV2710_CHIP_VERSION 0x4688

// fake register value to interpret next value as delay in ms
#define DELAY_REG 0xff
// not a real register - marks the end of register array
#define STOP_REG 0xfe

/** @brief sensor number of IMG PHY */
#define OV2710_PHY_NUM 1

/*
 * Choice:
 * copy of the registers to apply because we want to apply them all at
 * once because some registers need 2 writes while others don't (may have
 * an effect on stability but was not showing in testing)
 * 
 * If not defined will apply values register by register and not need a copy
 * of the registers for the active mode.
 */
//#define OV2710_COPY_REGS

// uncomment to write i2c commands to file
//#define ov2710_debug "ov2710_write.txt"




#ifdef WIN32 // on windows we do not need to sleep to wait for the bus
static void usleep(int t)
{
    (void)t;
}
#endif

typedef struct _OV2710cam_struct
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
     * if OV2710_COPY_REGS is defined:
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
}OV2710CAM_STRUCT;

static void OV2710_ApplyMode(OV2710CAM_STRUCT *psCam);
static void OV2710_ComputeGains(double flGain, IMG_UINT8 *gainSequence);
static void OV2710_ComputeExposure(IMG_UINT32 ui32Exposure,
    IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureSequence);
static IMG_RESULT OV2710_GetModeInfo(OV2710CAM_STRUCT *psCam,
    IMG_UINT16 ui16Mode, SENSOR_MODE *info);
static IMG_UINT16 OV2710_CalculateFocus(const IMG_UINT16 *pui16DACDist,
        const IMG_UINT16 *pui16DACValues, IMG_UINT8 ui8Entries,
        IMG_UINT16 ui16Requested);
// used before implementation therefore declared here
static IMG_RESULT OV2710_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus);

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

//#define REG_SC_CMMN_BIT_SEL 0x3031 // bits [4;0] for bitdepth
//#define REG_SC_CMMN_MIPI_SC_CTRL 0x3018 // bits [7;5] for mipi lane mode

//#define REG_PLL_CTRL_B 0x030B // PLL2_prediv (values see pll_ctrl_b_val)
//#define REG_PLL_CTRL_C 0x030C // PLL2_mult [1;0] multiplier [9;8]
//#define REG_PLL_CTRL_D 0x030D // PLL2_mult [7;0] multiplier [7;0]
//#define REG_PLL_CTRL_E 0x030E // PLL2_divs (values see pll_ctrl_c_val)
//#define REG_PLL_CTRL_F 0x030F // PLL2_divsp (values is 1/(r+1))
//#define REG_PLL_CTRL_11 0x0311 // PLL2_predivp (values is 1/(r+1)

/*
Bit[7:6]: Not used
Bit[5:3]: Charge pump control
Bit[2]: Not used
Bit[1:0]: PLL SELD5 divider
0x: Bypass
10: Divided by 4 when in 8-bit mode
11: Divided by 5 when in 10-bit mode
*/
#define REG_PLL_CTRL_00 0x300F 

/*
Bit[7:4]: PLL DIVS divider
System divider ratio
Bit[3:0]: PLL DIVM divider
MIPI divider ratio
*/
#define REG_PLL_CTRL_01 0x3010

/*
Bit[7]: PLL bypass
Bit[5:0]: PLL DIVP
*/
#define REG_PLL_CTRL_02 0x3011 

#define REG_PLL_PREDIVIDER 0x3012 //Bit[2:0]: PLL pre-divider ratio

#define REG_SC_CMMN_CHIP_ID_0 0x300A // high bits for chip version
#define REG_SC_CMMN_CHIP_ID_1 0x300B // low bits for chip version

#define REG_EXPOSURE 0x3500
#define REG_GAIN 0x350A

// values for PLL_prediv 1/x, see sensor data-sheet
static const double pll_ctrl_predivider_val[] = {
    1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 6.0, 8.0
};

static const double pll_ctrl_div45_val[] = {
    1.0, 1.0, 4.0, 5.0
};

static const double pll_ctrl_div1to2p5_val[] = {
    1.0, 1.0, 2.0, 2.5
};

static const double aec_min_gain = 1.0;

static const double aec_max_gain = 14.0;

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
static const unsigned int ov2710_focus_dac_min = 50;

// maximum focus in millimetres, if >= then focus is infinity
static const unsigned int ov2710_focus_dac_max = 600;

// focus values for the ov2710_focus_dac_dist
static const IMG_UINT16 ov2710_focus_dac_val[] = {
    0x3ff, 0x180, 0x120, 0x100, 0x000
};

// distances in millimetres for the ov2710_focus_dac_val
static const IMG_UINT16 ov2710_focus_dac_dist[] = {
    50, 150, 300, 500, 600
};

//
// modes declaration
//


// 1lane 30fps
static IMG_UINT8 ui8ModeRegs_1920_1088_30_1lane[]=
{
//    0x31, 0x03, 0x03,
//    0x30, 0x08, 0x82,
    //delay 5ms?
//    0x30, 0x08, 0x42,
    //delay 5ms?
//    0x30, 0x17, 0x7f,
//    0x30, 0x18, 0xfc,
    0x37, 0x06, 0x61,
    0x37, 0x12, 0x0c,
    0x36, 0x30, 0x6d,
    0x38, 0x01, 0xb4,
#ifdef FLIP_MIRROR_FIRST
	0x36, 0x21, 0x14,
#else
	0x36, 0x21, 0x04, // 0x14
#endif
    0x36, 0x04, 0x60,
    0x36, 0x03, 0xa7,
    0x36, 0x31, 0x26,
    0x36, 0x00, 0x04,
    0x36, 0x20, 0x37,
    0x36, 0x23, 0x00,
    0x37, 0x02, 0x9e,
    0x37, 0x03, 0x74,
    0x37, 0x04, 0x10,
    0x37, 0x0d, 0x0f,
    0x37, 0x13, 0x8b,
    0x37, 0x14, 0x74,
    0x37, 0x10, 0x9e,
    0x38, 0x01, 0xc4,
    0x36, 0x05, 0x05,
    0x36, 0x06, 0x12,
    0x30, 0x2d, 0x90,
    0x37, 0x0b, 0x40,
    0x37, 0x16, 0x31,
    0x37, 0x07, 0x52, //0728
    0x38, 0x0d, 0x74,
    0x51, 0x81, 0x20,
    0x51, 0x8f, 0x00,
    0x43, 0x01, 0xff,
    0x43, 0x03, 0x00,
    0x3a, 0x00, 0x78,
    0x30, 0x0f, 0x88, //0728
    0x30, 0x11, 0x28, //0728
    0x3a, 0x1a, 0x06,
    0x3a, 0x18, 0x00,
    0x3a, 0x19, 0x7a,
    0x3a, 0x13, 0x54,
    0x38, 0x2e, 0x0f,
    0x38, 0x1a, 0x1a,
    0x40, 0x1d, 0x02, //0728
    0x56, 0x88, 0x03,
    0x56, 0x84, 0x07,
    0x56, 0x85, 0xa0,
    0x56, 0x86, 0x04,
    0x56, 0x87, 0x43,
    //0x30, 0x11, 0x0a,//0x0a,
    //0x30, 0x12, 0x01,
    0x30, 0x0f, 0x8a,		//1000,1010
    0x30, 0x17, 0x00,
    0x30, 0x18, 0x00,
    0x30, 0x0e, 0x04,
    0x48, 0x01, 0x0f,
    0x30, 0x0f, 0xc3/*0xc3*/, //0728		//1100,0011
    0x30, 0x11, 0x0a,//0x09/*0x0a*/,		//0000,1010
    0x30, 0x12, 0x01,
    0x3a, 0x0f, 0x40,
    0x3a, 0x10, 0x38,
    0x3a, 0x1b, 0x48,
    0x3a, 0x1e, 0x30,
    0x3a, 0x11, 0x90,
    0x3a, 0x1f, 0x10,
    //1920x1088
    0x30, 0x10, 0x00,
    0x38, 0x20, 0x00,
    0x38, 0x20, 0x00,
    0x38, 0x21, 0x07,
    0x38, 0x21, 0x98,
    0x38, 0x1c, 0x00,
    0x38, 0x1d, 0x02,
    0x38, 0x1e, 0x04,
    0x38, 0x1f, 0x38,
    
    0x38, 0x00, 0x01,	//0x01c8 = 456
    0x38, 0x01, 0xc8,
    0x38, 0x04, 0x07,	//0x0790 = 1936
    0x38, 0x05, 0x90,
    
    0x38, 0x02, 0x00,	//0x0009 = 9
#ifdef FLIP_MIRROR_FIRST
    0x38, 0x03, 0x09,
#else
	0x38, 0x03, 0x0a, //0x09
#endif
    0x38, 0x06, 0x04,	// 0x0440 = 1088
    0x38, 0x07, 0x40,
    
    0x38, 0x08, 0x07,	//0x0780 = 1920
    0x38, 0x09, 0x80,
    0x38, 0x0a, 0x04,	//0x0440 = 1088
    0x38, 0x0b, 0x40,

    0x38, 0x0c, 0x09,	//0x0970 = 2416
    0x38, 0x0d, 0x70,
    0x38, 0x0e, 0x04,	//0x0450 = 1104 
    0x38, 0x0f, 0x50,

    0x38, 0x10, 0x08,
    0x38, 0x11, 0x02,
    //0x35, 0x03, 0x07,
    0x35, 0x03, 0x37,

    0x35, 0x0a, 0x00,
    0x35, 0x0b, 0x00,
#if 1 // 29.99fps
    0x35, 0x0c, 0x00, // control fps
    0x35, 0x0d, 0x00,
#elif 0 // 28.40fps
    0x35, 0x0c, 0x00, // control fps
    0x35, 0x0d, 0x40,
#else //20fps
    0x35, 0x0c, 0x02, // control fps
    0x35, 0x0d, 0x20,
#endif
    //0x35, 0x0c, 0x00, // control fps
    //0x35, 0x0c, 0x00,
    //0x35, 0x0d, 0x10,

    0x35, 0x01, 0x00,
    0x35, 0x02, 0x1e,
    0x35, 0x0b, 0x10, //close awb
    0x50, 0x01, 0x4e,//0x4e,
    //0x50, 0x01, 0x4e,
    0x34, 0x06, 0x01,
    0x34, 0x00, 0x04,
    0x34, 0x01, 0x00,
    0x34, 0x02, 0x04,
    0x34, 0x03, 0x00,
    0x34, 0x04, 0x04,
    0x34, 0x05, 0x00, //modified blc
    0x40, 0x00, 0x05, //drive capacity
    0x30, 0x2c, 0x00, //close lenc              
#if defined(USE_ISP_LENS_SHADING_CORRECTION)
    // disable lens shading correction.
    0x50, 0x00, 0x5f,//0x5b,
#else
  #if 0
    // 60%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x14,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x13,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x0F,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 0
    // 65%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x18,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x17,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x12,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 0
    // 70%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x1C,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x1B,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x16,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 0
    // 75%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x21,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x1F,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x19,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 0
    // 80%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x26,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x24,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x1D,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 0
    // 85%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x2A,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x28,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x20,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 0
	// 90%
    0x50, 0x00, 0xDF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x2F,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x2C,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x24,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 0
    // 95%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x33,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x30,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x27,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
#elif 1
    // 100%
    0x50, 0x00, 0xFF,

    0x58, 0x00, 0x03,
    0x58, 0x01, 0xEF,
    0x58, 0x02, 0x02,
    0x58, 0x03, 0x52,
    0x58, 0x04, 0x37,
    0x58, 0x05, 0x06,
    0x58, 0x06, 0x00,
    0x58, 0x07, 0x00,

    0x58, 0x08, 0x03,
    0x58, 0x09, 0xE1,
    0x58, 0x0A, 0x02,
    0x58, 0x0B, 0x4D,
    0x58, 0x0C, 0x34,
    0x58, 0x0D, 0x06,
    0x58, 0x0E, 0x00,
    0x58, 0x0F, 0x00,

    0x58, 0x10, 0x03,
    0x58, 0x11, 0xE1,
    0x58, 0x12, 0x02,
    0x58, 0x13, 0x4B,
    0x58, 0x14, 0x2A,
    0x58, 0x15, 0x06,
    0x58, 0x16, 0x00,
    0x58, 0x17, 0x00,
  #endif
#endif

///////flip & mirror
////    0x38, 0x03, 0x09,
////    0x36, 0x21, 0x14,
#ifdef FLIP_MIRROR_FIRST
    0x38, 0x18, 0xe0,
#else
	0x38, 0x18, 0x80,
#endif
//////

	STOP_REG, STOP_REG, STOP_REG
};

static IMG_UINT8 ui8ModeRegs_1920_1080_30_1lane[]=
{
    0x31, 0x03, 0x03,
    0x30, 0x08, 0x82,
    //delay 5ms?
    0x30, 0x08, 0x42,
    //delay 5ms?
    0x30, 0x17, 0x7f,
    0x30, 0x18, 0xfc,
    0x37, 0x06, 0x61,
    0x37, 0x12, 0x0c,
    0x36, 0x30, 0x6d,
    0x38, 0x01, 0xb4,
    0x36, 0x21, 0x04,
    0x36, 0x04, 0x60,
    0x36, 0x03, 0xa7,
    0x36, 0x31, 0x26,
    0x36, 0x00, 0x04,
    0x36, 0x20, 0x37,
    0x36, 0x23, 0x00,
    0x37, 0x02, 0x9e,
    0x37, 0x03, 0x74,
    0x37, 0x04, 0x10,
    0x37, 0x0d, 0x07,
    0x37, 0x13, 0x8b,
    0x37, 0x14, 0x74,
    0x37, 0x10, 0x9e,
    0x38, 0x01, 0xc4,
    0x36, 0x05, 0x05,
    0x36, 0x06, 0x12,
    0x30, 0x2d, 0x90,
    0x37, 0x0b, 0x40,
    0x37, 0x16, 0x31,
    0x37, 0x07, 0x52, //0728
    0x38, 0x0d, 0x74,
    0x51, 0x81, 0x20,
    0x51, 0x8f, 0x00,
    0x43, 0x01, 0xff,
    0x43, 0x03, 0x00,
    0x3a, 0x00, 0x78,
    0x30, 0x0f, 0x88,
    0x30, 0x11, 0x28,
    0x3a, 0x1a, 0x06,
    0x3a, 0x18, 0x00,
    0x3a, 0x19, 0x7a,
    0x3a, 0x13, 0x54,
    0x38, 0x2e, 0x0f,
    0x38, 0x1a, 0x1a,
    0x40, 0x1d, 0x02, //0728
    0x56, 0x88, 0x03,
    0x56, 0x84, 0x07,
    0x56, 0x85, 0xa0,
    0x56, 0x86, 0x04,
    0x56, 0x87, 0x43,

    0x30, 0x10, 0x00,	// add for get OV2710_GetModeInfo
    0x30, 0x11, 0x08,//0x0a,
    0x30, 0x12, 0x01,
    0x30, 0x0f, 0x8a,
    0x30, 0x17, 0x00,
    0x30, 0x18, 0x00,
    0x30, 0x0e, 0x04,
    0x48, 0x01, 0x0f,
    0x30, 0x0f, 0xc3, //0728

    0x3a, 0x0f, 0x40,
    0x3a, 0x10, 0x38,
    0x3a, 0x1b, 0x48,
    0x3a, 0x1e, 0x30,
    0x3a, 0x11, 0x90,
    0x3a, 0x1f, 0x10,
    0x35, 0x03, 0x07,
    0x35, 0x01, 0x2e,
    0x35, 0x02, 0x00,

    0x35, 0x0b, 0x10, //close awb
    //0x50, 0x01, 0x4f,
    0x50, 0x01, 0x4e,
    0x34, 0x06, 0x01,
    //0x34, 0x00, 0x04,
    //0x34, 0x01, 0x00,
    //0x34, 0x02, 0x04,
    //0x34, 0x03, 0x00,
    //0x34, 0x04, 0x04,
    //0x34, 0x05, 0x00, //modified blc
    0x40, 0x00, 0x05, //drive capacity
    //0x30, 0x2c, 0x00, //close lenc
    0x30, 0x2c, 0xc0, //close lenc
    0x50, 0x00, 0x5b,

	STOP_REG, STOP_REG, STOP_REG
};

struct ov2710_mode
{
    IMG_UINT8 ui8Flipping;
    const IMG_UINT8 *modeRegisters;
    IMG_UINT32 nRegisters; // initially 0 then computed the 1st time
};

struct ov2710_mode ov2710_modes[] =
{
    {SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1088_30_1lane, 0}, //0
    {SENSOR_FLIP_BOTH, ui8ModeRegs_1920_1080_30_1lane, 0}, //1
};

static IMG_UINT8* OV2710_GetRegisters(OV2710CAM_STRUCT *psCam,
    unsigned int uiMode, unsigned int *nRegisters)
{
    if (uiMode < sizeof(ov2710_modes) / sizeof(struct ov2710_mode))
    {
        const IMG_UINT8 *registerArray = ov2710_modes[uiMode].modeRegisters;
        if (0 == ov2710_modes[uiMode].nRegisters)
        {
            // should be done only once per mode
            int i = 0;
            while (STOP_REG != registerArray[3 * i])
            {
                i++;
            }
            ov2710_modes[uiMode].nRegisters = i;
        }
        *nRegisters = ov2710_modes[uiMode].nRegisters;
        
        return (IMG_UINT8 *)registerArray;
    }
    // if it is an invalid mode returns NULL

    return NULL;
}

static IMG_RESULT ov2710_i2c_write8(int i2c, const IMG_UINT8 *data,
    unsigned int len)
{
#if !file_i2c
    unsigned i;
#ifdef ov2710_debug
    FILE *f=NULL;
    static unsigned nwrite=0;
#endif //ov2710_debug

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
	if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, OV2710_WRITE_ADDR))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }
#ifdef ov2710_debug
    f = fopen("/tmp/ov2710_write.txt", "a");
    fprintf(f, "write %d\n", nwrite++);
#endif //ov2710_debug

    for (i = 0; i < len; data += 3, i += 3)
    {
        int write_len = 0;

        if(DELAY_REG == data[0])
        {
            usleep((int)data[1]*1000);
#ifdef ov2710_debug
        fprintf(f, "delay %dms\n", data[1]);
#endif //ov2710_debug
            continue;
        }

        write_len = write(i2c, data, 3);
#ifdef ov2710_debug
        fprintf(f, "0x%02X 0x%02X 0x%02X\n", data[0], data[1], data[2]);
#endif //ov2710_debug

        if (write_len != 3)
        {
            LOG_ERROR("Failed to write I2C data! write_len = %d, index = %d\n",
                    write_len, i);
#ifdef ov2710_debug
            fclose(f);
#endif //ov2710_debug
            return IMG_ERROR_BUSY;
        }
    }
#ifdef ov2710_debug
    fclose(f);
#endif //ov2710_debug
#endif //file_i2c
    return IMG_SUCCESS;
}

static IMG_RESULT ov2710_i2c_read8(int i2c, IMG_UINT16 offset,
    IMG_UINT8 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
    
    /* Set I2C slave address */
	if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, OV2710_READ_ADDR))
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
#endif //file_i2c
}

static IMG_RESULT ov2710_i2c_read16(int i2c, IMG_UINT16 offset,
    IMG_UINT16 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
    
    /* Set I2C slave address */
    if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, OV2710_READ_ADDR))
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
#endif //file_i2c
}

static IMG_RESULT OV2710_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    IMG_RESULT ret;
    IMG_UINT16 v = 0;
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_ASSERT(strlen(OV2710_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(OV2710_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);

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
    sprintf(psInfo->pszSensorName, OV2710_SENSOR_INFO_NAME);
    
#ifndef NO_DEV_CHECK
    ret = ov2710_i2c_read16(psCam->i2c, REG_SC_CMMN_CHIP_ID_0, &v);
#else
    ret = 1;
#endif //NO_DEV_CHECK
    if (!ret)
    {
        sprintf(psInfo->pszSensorVersion, "0x%x", v);
    }
    else
    {
        sprintf(psInfo->pszSensorVersion, OV2710_SENSOR_VERSION);
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
    psInfo->ui32ModeCount = sizeof(ov2710_modes) / sizeof(struct ov2710_mode);
#endif //INFOTM_ISP

    LOG_DEBUG("provided BayerOriginal=%d\n", psInfo->eBayerOriginal);
    return IMG_SUCCESS;
}

static IMG_RESULT OV2710_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    /*if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    ret = OV2710_GetModeInfo(psCam, nIndex, psModes);
    return ret;
}

static IMG_RESULT OV2710_GetState(SENSOR_HANDLE hHandle,
        SENSOR_STATUS *psStatus)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

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

static IMG_RESULT OV2710_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
          IMG_UINT8 ui8Flipping)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);
    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = NULL;
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    modeReg = OV2710_GetRegisters(psCam, nMode, &nRegisters);

    if (modeReg)
    {
        if((ui8Flipping&(ov2710_modes[nMode].ui8Flipping))!=ui8Flipping)
        {
            LOG_ERROR("sensor mode does not support selected flipping "\
                "0x%x (supports 0x%x)\n",
                ui8Flipping, ov2710_modes[nMode].ui8Flipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }

#if defined(OV2710_COPY_REGS)
        if (psCam->currModeReg)
        {
            IMG_FREE(psCam->currModeReg);
        }
#endif //OV2710_COPY_REGS
        psCam->nRegisters = nRegisters;
        
#if defined(OV2710_COPY_REGS)
        /* we want to apply all the registers at once - need to copy them */
        psCam->currModeReg = (IMG_UINT8*)IMG_MALLOC(psCam->nRegisters * 3);
        IMG_MEMCPY(psCam->currModeReg, modeReg, psCam->nRegisters * 3);
#else
        /* no need to copy the register as we will apply them one by one */
        psCam->currModeReg = (IMG_UINT8*)modeReg;
#endif //OV2710_COPY_REGS

        ret = OV2710_GetModeInfo(psCam, nMode, &(psCam->currMode));
        if (ret)
        {
            LOG_ERROR("failed to get mode %d initial information!\n", nMode);
            return IMG_ERROR_FATAL;
        }

#ifdef DO_SETUP_IN_ENABLE
	#ifndef INFOTM_ISP
        /* because we don't do the actual setup we do not have real
         * information about the sensor - fake it */
        psCam->ui32Exposure = psCam->currMode.ui32ExposureMin * 302;

        psCam->ui32Exposure = IMG_MIN_INT(psCam->currMode.ui32ExposureMax, 
            IMG_MAX_INT(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin));
	#else
		// because we don't do the actual setup we do not have real information about the sensor - fake it
		psCam->flGain = 1.0;

		psCam->psSensorPhy->psGasket->bParallel = IMG_FALSE;
		psCam->psSensorPhy->psGasket->uiWidth = psCam->currMode.ui16Width;
		psCam->psSensorPhy->psGasket->uiHeight = psCam->currMode.ui16Height - 1;
		psCam->psSensorPhy->psGasket->bVSync = IMG_TRUE;
		psCam->psSensorPhy->psGasket->bHSync = IMG_TRUE;
		psCam->psSensorPhy->psGasket->ui8ParallelBitdepth = 10;

		// line length in micros assuming a 24MHz clock.
		psCam->currMode.dbExposureMin = 2416.0 / 80;//75;


		psCam->currMode.ui32ExposureMin = (IMG_UINT32)floor(psCam->currMode.dbExposureMin);
		#if !defined(IQ_TUNING)
		psCam->currMode.ui32ExposureMax = psCam->currMode.ui32ExposureMin * psCam->currMode.ui16VerticalTotal;
		#else
		psCam->currMode.ui32ExposureMax = 600000;
		#endif //!IQ_TUNING
		//psCam->ui32Exposure = psCam->ui32ExposureMin * 302;
		psCam->ui32Exposure = psCam->currMode.ui32ExposureMin;
	#endif //INFOTM_ISP
#else
        OV2710_ApplyMode(psCam);
#endif //DO_SETUP_IN_ENABLE

        psCam->ui16CurrentFocus = ov2710_focus_dac_min; // minimum focus
        psCam->ui16CurrentMode = nMode;
        psCam->ui8CurrentFlipping = ui8Flipping;
        return IMG_SUCCESS;
    }


    return IMG_ERROR_INVALID_PARAMETERS;
}

static IMG_RESULT OV2710_Enable(SENSOR_HANDLE hHandle)
{
    IMG_UINT8 aui18EnableRegs[] =
    {
		0x30, 0x08, 0x02,
    };
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling OV2710 Camera!\n");
    psCam->bEnabled=IMG_TRUE;

    psCam->psSensorPhy->psGasket->bParallel = IMG_FALSE;
    psCam->psSensorPhy->psGasket->uiGasket = psCam->imager;
    SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->currMode.ui8MipiLanes,
        OV2710_PHY_NUM);

#ifdef DO_SETUP_IN_ENABLE
    // because we didn't do the actual setup in setMode we have to do it now
    // it may start the sensor so we have to do it after the gasket enable
    /// @ fix conversion from float to uint
    OV2710_ApplyMode(psCam);
#endif //DO_SETUP_IN_ENABLE

    ov2710_i2c_write8(psCam->i2c, aui18EnableRegs, sizeof(aui18EnableRegs));


    OV2710_SetFocus(hHandle, psCam->ui16CurrentFocus);

    LOG_DEBUG("camera enabled\n");
    return IMG_SUCCESS;
}

static IMG_RESULT OV2710_Disable(SENSOR_HANDLE hHandle)
{
    static const IMG_UINT8 disableRegs[] =
    {
		0x30, 0x08, 0x42,
    };

    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(psCam->bEnabled)
    {
        int delay = 0;
        LOG_INFO("Disabling OV2710 camera\n");
        psCam->bEnabled = IMG_FALSE;

#ifdef INFOTM_ISP
        g_OV2710_ui32LinesBackup = 0;
        g_OV2710_ui8GainValHiBackup = 0xff;
        g_OV2710_ui8GainValLoBackup = 0xff;
#endif //INFOTM_ISP		

        ov2710_i2c_write8(psCam->i2c, disableRegs, sizeof(disableRegs));
//#ifdef INFOTM_ISP
        //usleep(psCam->currMode.ui32ExposureMax);
//#endif //INFOTM_ISP
        // delay of a frame period to ensure sensor has stopped
        // flFrameRate in Hz, change to MHz to have micro seconds
        delay = (int)floor((1.0 / psCam->currMode.flFrameRate) * 1000 * 1000);
        LOG_DEBUG("delay of %uus between disabling sensor/phy\n", delay);
        
        usleep(delay);

        SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, OV2710_PHY_NUM);
    }

    return IMG_SUCCESS;
}

static IMG_RESULT OV2710_Destroy(SENSOR_HANDLE hHandle)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_DEBUG("Destroying OV2710 camera\n");
    /* remember to free anything that might have been allocated dynamically
     * (like extended params...)*/
    if(psCam->bEnabled)
    {
        OV2710_Disable(hHandle);
    }
    SensorPhyDeinit(psCam->psSensorPhy);
    if (psCam->currModeReg)
    {
#if defined(OV2710_COPY_REGS)
        IMG_FREE(psCam->currModeReg);
#endif //OV2710_COPY_REGS
        psCam->currModeReg = NULL;
    }
#if !file_i2c
    close(psCam->i2c);
#endif //file_i2c
    IMG_FREE(psCam);

#ifdef BAND_SENSOR_DRIVER
	{
      sensor_fd = open(ov2710_dev,O_RDONLY);
      if (sensor_fd >= 0)
      {
          usleep(3000);
          int args = 0;
	      if (ioctl(sensor_fd, OV2710_DEV_CLOSE_SENSOR, &args) < 0)
	      {
	          printf("Call cmd OV2710_DEV_CLOSE_SENSOR fail\n");
	      }

          close(sensor_fd);
      }

      sensor_fd = -1;
	      //system("rmmod /ko/ov2710_mipi.ko &");
	}
#endif //BAND_SENSOR_DRIVER
    return IMG_SUCCESS;
}

static IMG_RESULT OV2710_GetFocusRange(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Min, IMG_UINT16 *pui16Max)
{
    /*OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    *pui16Min = ov2710_focus_dac_min;
    *pui16Max = ov2710_focus_dac_max;
    return IMG_SUCCESS;
}

static IMG_RESULT OV2710_GetCurrentFocus(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Current)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

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
IMG_UINT16 OV2710_CalculateFocus(const IMG_UINT16 *pui16DACDist,
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
IMG_RESULT OV2710_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus)
{
#if !file_i2c
    unsigned i;
#endif //file_i2c
    IMG_UINT8 ui8Regs[4];
    IMG_UINT16 ui16DACVal;

    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui16CurrentFocus = ui16Focus;
    if (psCam->ui16CurrentFocus >= ov2710_focus_dac_max)
    {
        // special case the infinity as it doesn't fit in with the rest
        ui16DACVal = 0;
    }
    else
    {
        ui16DACVal = OV2710_CalculateFocus(ov2710_focus_dac_dist,
            ov2710_focus_dac_val,
            sizeof(ov2710_focus_dac_dist) / sizeof(IMG_UINT16),
            ui16Focus);
    }

    ui8Regs[0] = 4;
    ui8Regs[1] = ui16DACVal >> 8;
    ui8Regs[2] = 5;
    ui8Regs[3] = ui16DACVal & 0xff;

#if !file_i2c
	if (ioctl(psCam->i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, OV2710_WRITE_ADDR/*AUTOFOCUS_WRITE_ADDR*/))
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

static IMG_RESULT OV2710_GetExposureRange(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

#ifndef INFOTM_ISP
    *pui32Min=psCam->currMode.ui32ExposureMin;
#else
	*pui32Min = psCam->currMode.ui32ExposureMin * MIN_EXPOSURE_LINE;
#endif //INFOTM_ISP
    *pui32Max=psCam->currMode.ui32ExposureMax;
    *pui8Contexts=1;
    return IMG_SUCCESS;
}

static IMG_RESULT OV2710_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current=psCam->ui32Exposure;
    return IMG_SUCCESS;
}

void OV2710_ComputeExposure(IMG_UINT32 ui32Exposure,
    IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureRegs)
{
    IMG_UINT32 exposureRatio = (ui32Exposure)/ui32ExposureMin;

    LOG_DEBUG("Exposure Val 0x%x\n", exposureRatio);

    exposureRegs[2]=((exposureRatio)>>12) & 0xf;
    exposureRegs[5]=((exposureRatio)>>4) & 0xff;
    exposureRegs[8]=((exposureRatio)<<4) & 0xff;
}

static IMG_RESULT OV2710_SetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
#if defined(ENABLE_GROUP)
    static IMG_UINT8 group_id = 0;
#endif //ENABLE_GROUP
    IMG_UINT32 ui32Lines;
    IMG_UINT8 exposureRegs[]={
	#if defined(ENABLE_GROUP)
        0x32, 0x12, 0x00,  // enable group 0
        0x35, 0x00, 0x00,  //
        0x35, 0x01, 0x2E,  //
        0x35, 0x02, 0x80,  //
        0x32, 0x12, 0x10,  // end hold of group 0 register writes
        0x32, 0x12, 0xa0,  // quick launch group 0
	#else
        0x35, 0x00, 0x00,  //
        0x35, 0x01, 0x2E,  //
        0x35, 0x02, 0x80,  //
	#endif //ENABLE_GROUP
    };

    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui32Exposure=ui32Current;

#if 0//ndef INFOTM_ISP // ??? for debug
    if (psCam->bEnabled || 1)
    {
        OV2710_ComputeExposure(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin,
            &(exposureRegs[3]));

		printf("OV2710_SetExposure()\n");
        ov2710_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));
    }
#else

	ui32Lines = psCam->ui32Exposure / psCam->currMode.dbExposureMin;
//	if ((1102 == ui32Lines) || (1103 == ui32Lines))
//	{
//		//ui32Lines = asModes[psCam->ui16CurrentMode].ui16VerticalTotal;
//		ui32Lines = 1104;
//	}
	if ((1098 < ui32Lines) && (1104 >= ui32Lines))
	{
		ui32Lines = 1098;
	}
	if (g_OV2710_ui32LinesBackup == ui32Lines)
	{
	    return IMG_SUCCESS;
	}
	g_OV2710_ui32LinesBackup = ui32Lines;

	#if defined(ENABLE_GROUP)
		if (group_id == 0)
		{
			group_id = 1;
		}
		else
		{
			exposureRegs[2] = 0x01;
			exposureRegs[14] = 0x11;
			exposureRegs[17] = 0xa1;
			group_id = 0;
		}

		exposureRegs[5] = (ui32Lines >> 12) & 0xf;
		exposureRegs[8] = (ui32Lines >> 4) & 0xff;
		exposureRegs[11] = (ui32Lines << 4) & 0xff;
	#else
		exposureRegs[2] = (ui32Lines >> 12) & 0xf;
		exposureRegs[5] = (ui32Lines >> 4) & 0xff;
		exposureRegs[8] = (ui32Lines << 4) & 0xff;
	#endif //ENABLE_GROUP

    LOG_DEBUG("Exposure Val %x\n", ui32Lines);
	#if defined(ENABLE_AE_DEBUG_MSG)
		printf("OV2710_SetExposure Exposure = 0x%08X (%d)\n", psCam->ui32Exposure, psCam->ui32Exposure);
	#endif


    ov2710_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));

#if defined(ENABLE_GROUP)
//    usleep(33*1000);
#endif //ENABLE_GROUP

    /*
    aui16Regs[0]=0x3012;// COARSE_INTEGRATION_TIME
    SensorI2CWrite16(psCam->psI2C,aui16Regs,2);
    */
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

typedef struct _ov2710_gain_
{
    double flGain;
    unsigned char ui8GainRegValHi;
    unsigned char ui8GainRegValLo;
} OV2710_GAIN_TABLE;

OV2710_GAIN_TABLE asOV2710GainValues[] =
{
    { 1,      0x00, 0x00 },
    { 1.0625, 0x00, 0x01 },
    { 1.125,  0x00, 0x02 },
    { 1.1875, 0x00, 0x03 },
    { 1.25,   0x00, 0x04 },
    { 1.3125, 0x00, 0x05 },
    { 1.375,  0x00, 0x06 },
    { 1.4375, 0x00, 0x07 },
    { 1.5,    0x00, 0x08 },
    { 1.5625, 0x00, 0x09 },
    { 1.625,  0x00, 0x0A },
    { 1.6875, 0x00, 0x0B },
    { 1.75,   0x00, 0x0C },
    { 1.8125, 0x00, 0x0D },
    { 1.875,  0x00, 0x0E },
    { 1.9375, 0x00, 0x0F },
    { 2,      0x00, 0x10 },
    { 2.125,  0x00, 0x11 },
    { 2.25,   0x00, 0x12 },
    { 2.375,  0x00, 0x13 },
    { 2.5,    0x00, 0x14 },
    { 2.625,  0x00, 0x15 },
    { 2.75,   0x00, 0x16 },
    { 2.875,  0x00, 0x17 },
    { 3,      0x00, 0x18 },
    { 3.125,  0x00, 0x19 },
    { 3.25,   0x00, 0x1A },
    { 3.375,  0x00, 0x1B },
    { 3.5,    0x00, 0x1C },
    { 3.625,  0x00, 0x1D },
    { 3.75,   0x00, 0x1E },
    { 3.875,  0x00, 0x1F },
    { 4,      0x00, 0x30 },
    { 4.25,   0x00, 0x31 },
    { 4.5,    0x00, 0x32 },
    { 4.75,   0x00, 0x33 },
    { 5,      0x00, 0x34 },
    { 5.25,   0x00, 0x35 },
    { 5.5,    0x00, 0x36 },
    { 5.75,   0x00, 0x37 },
    { 6,      0x00, 0x38 },
    { 6.25,   0x00, 0x39 },
    { 6.5,    0x00, 0x3A },
    { 6.75,   0x00, 0x3B },
    { 7,      0x00, 0x3C },
    { 7.25,   0x00, 0x3D },
    { 7.5,    0x00, 0x3E },
    { 7.75,   0x00, 0x3F },
    { 8,      0x00, 0x70 },
    { 8.5,    0x00, 0x71 },
    { 9,      0x00, 0x72 },
    { 9.5,    0x00, 0x73 },
    { 10,     0x00, 0x74 },
    { 10.5,   0x00, 0x75 },
    { 11,     0x00, 0x76 },
    { 11.5,   0x00, 0x77 },
    { 12,     0x00, 0x78 },
    { 12.5,   0x00, 0x79 },
    { 13,     0x00, 0x7A },
    { 13.5,   0x00, 0x7B },
    { 14,     0x00, 0x7C },
    { 14.5,   0x00, 0x7D },
    { 15,     0x00, 0x7E },
    { 15.5,   0x00, 0x7F },
    { 16,     0x00, 0xF0 },
    { 17,     0x00, 0xF1 },
    { 18,     0x00, 0xF2 },
    { 19,     0x00, 0xF3 },
    { 20,     0x00, 0xF4 },
    { 21,     0x00, 0xF5 },
    { 22,     0x00, 0xF6 },
    { 23,     0x00, 0xF7 },
    { 24,     0x00, 0xF8 },
    { 25,     0x00, 0xF9 },
    { 26,     0x00, 0xFA },
    { 27,     0x00, 0xFB },
    { 28,     0x00, 0xFC },
    { 29,     0x00, 0xFD },
    { 30,     0x00, 0xFE },
    { 31,     0x00, 0xFF },
    { 32,     0x01, 0xF0 },
    { 34,     0x01, 0xF1 },
    { 36,     0x01, 0xF2 },
    { 38,     0x01, 0xF3 },
    { 40,     0x01, 0xF4 },
    { 42,     0x01, 0xF5 },
    { 44,     0x01, 0xF6 },
    { 46,     0x01, 0xF7 },
    { 48,     0x01, 0xF8 },
    { 50,     0x01, 0xF9 },
    { 52,     0x01, 0xFA },
    { 54,     0x01, 0xFB },
    { 56,     0x01, 0xFC },
    { 58,     0x01, 0xFD },
    { 60,     0x01, 0xFE },
    { 62,     0x01, 0xFF },
};

void OV2710_ComputeGains(double flGain, IMG_UINT8 *gainSequence)
{
    unsigned int n = 0; // if flGain <= 1.0
    if (flGain > aec_min_gain)
    {
        n = (unsigned int)floor((flGain - aec_min_gain) * aec_max_gain);
    }
    n = IMG_MIN_INT(n, sizeof(aec_long_gain_val) / sizeof(IMG_UINT16) -1);

    LOG_DEBUG("gain n=%u from %lf\n", n, (flGain-aec_min_gain)*aec_max_gain);
    gainSequence[2] = 0;
    gainSequence[5] = (aec_long_gain_val[n] >> 8) & 0xff;
    gainSequence[8] = aec_long_gain_val[n] & 0xff;
}

static IMG_RESULT OV2710_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
#if defined(ENABLE_GROUP)
    static IMG_UINT8 group_id = 0;
#endif //ENABLE_GROUP
    unsigned int nIndex;
    IMG_UINT8 gainRegs[] = 
	{
	#if defined(ENABLE_GROUP)
        0x32, 0x12, 0x02,  // enable group 0
        0x35, 0x0a, 0x00,  //
        0x35, 0x0b, 0x00,  //
        0x32, 0x12, 0x12,  // end hold of group 0 register writes
        0x32, 0x12, 0xa2,  // quick launch group 0
	#else
        0x35, 0x0a, 0x00,  //
        0x35, 0x0b, 0x00,  //
	#endif //ENABLE_GROUP
    };

    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->flGain = flGain;

#if 0//ndef INFOTM_ISP // ??? for debug
    if (psCam->bEnabled || 1)
    {
        OV2710_ComputeGains(flGain, &(gainRegs[3]));

        ov2710_i2c_write8(psCam->i2c, gainRegs, sizeof(gainRegs));
    }
#else
	nIndex = 0;

    /*
     * First search the gain table for the lowest analog gain we can use,
     * then use the digital gain to get more accuracy/range.
     */
    while(nIndex < (sizeof(asOV2710GainValues) / sizeof(OV2710_GAIN_TABLE)))
    {
        if(asOV2710GainValues[nIndex].flGain > flGain)
            break;
        nIndex++;
    }

    if(nIndex > 0)
        nIndex = nIndex - 1;

    if ((g_OV2710_ui8GainValHiBackup == asOV2710GainValues[nIndex].ui8GainRegValHi) &&
        (g_OV2710_ui8GainValLoBackup == asOV2710GainValues[nIndex].ui8GainRegValLo)) {
        return IMG_SUCCESS;
    }
    g_OV2710_ui8GainValHiBackup = asOV2710GainValues[nIndex].ui8GainRegValHi;
    g_OV2710_ui8GainValLoBackup = asOV2710GainValues[nIndex].ui8GainRegValLo;

	#if defined(ENABLE_GROUP)
		if (group_id == 0)
		{
			group_id = 1;
		}
		else
		{
			gainRegs[2] = 0x03;
			gainRegs[11] = 0x13;
			gainRegs[14] = 0xa3;
			group_id = 0;
		}

		gainRegs[5] = asOV2710GainValues[nIndex].ui8GainRegValHi;
		gainRegs[8] = asOV2710GainValues[nIndex].ui8GainRegValLo;
	#else
		gainRegs[2] = asOV2710GainValues[nIndex].ui8GainRegValHi;
		gainRegs[5] = asOV2710GainValues[nIndex].ui8GainRegValLo;
	#endif //ENABLE_GROUP

	#if defined(ENABLE_AE_DEBUG_MSG)
		printf("OV2710_SetGain flGian = %f, Gain = 0x%02X%02X\n", psCam->flGain, asOV2710GainValues[nIndex].ui8GainRegValHi, asOV2710GainValues[nIndex].ui8GainRegValLo);
	#endif //ENABLE_AE_DEBUG_MSG

	ov2710_i2c_write8(psCam->i2c, gainRegs, sizeof(gainRegs));

	#if defined(ENABLE_GROUP)
	//    usleep(33*1000);
	#endif //ENABLE_GROUP
#endif //INFOTM_ISP	

    return IMG_SUCCESS;
}

static IMG_RESULT OV2710_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

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

static IMG_RESULT OV2710_GetGain(SENSOR_HANDLE hHandle, double *pflGain,
    IMG_UINT8 ui8Context)
{
    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain=psCam->flGain;
    return IMG_SUCCESS;
}

IMG_RESULT OV2710_Create(SENSOR_HANDLE *phHandle)
{
#if !file_i2c
    char i2c_dev_path[NAME_MAX];
    IMG_UINT16 chipV;
    IMG_RESULT ret;
    char adaptor[64];
#endif //file_i2c
    OV2710CAM_STRUCT *psCam;
	LOG_DEBUG("-->OV2710CamCreate\n");

#ifdef BAND_SENSOR_DRIVER
	{
		  int  args =0;

		  //system("insmod ov2710_mipi.ko");
		  sensor_fd = open(ov2710_dev, O_RDONLY);
		  if (sensor_fd >= 0)
		  {
			  if (ioctl(sensor_fd, OV2710_DEV_OPEN_SENSOR, &args) < 0)
			  {
				  printf("Call cmd OV2710_DEV_OPEN_SENSOR fail\n");
				  //close(fd);
				  //return -1;
			  }
			  close(sensor_fd);
		  } 
	}
#endif //BAND_SENSOR_DRIVER

    psCam=(OV2710CAM_STRUCT *)IMG_CALLOC(1, sizeof(OV2710CAM_STRUCT));
    if(!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    *phHandle=&(psCam->sFuncs);
    psCam->sFuncs.GetMode = OV2710_GetMode;
    psCam->sFuncs.GetState = OV2710_GetState;
    psCam->sFuncs.SetMode = OV2710_SetMode;
    psCam->sFuncs.Enable = OV2710_Enable;
    psCam->sFuncs.Disable = OV2710_Disable;
    psCam->sFuncs.Destroy = OV2710_Destroy;
    psCam->sFuncs.GetInfo = OV2710_GetInfo;

    psCam->sFuncs.GetCurrentGain = OV2710_GetGain;
    psCam->sFuncs.GetGainRange = OV2710_GetGainRange;
    psCam->sFuncs.SetGain = OV2710_SetGain;
    psCam->sFuncs.SetExposure = OV2710_SetExposure;
    psCam->sFuncs.GetExposureRange = OV2710_GetExposureRange;
    psCam->sFuncs.GetExposure = OV2710_GetExposure;

    psCam->sFuncs.GetCurrentFocus = OV2710_GetCurrentFocus;
    psCam->sFuncs.GetFocusRange = OV2710_GetFocusRange;
    psCam->sFuncs.SetFocus = OV2710_SetFocus;
#ifdef INFOTM_ISP	
	psCam->sFuncs.SetGainAndExposure = OV2710_SetGainAndExposure;
	psCam->sFuncs.SetFlipMirror = OV2710_SetFlipMirror;
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
    if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), OV2710_WRITE_ADDR, adaptor))
    {
        LOG_ERROR("Failed to find I2C device!\n");
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

	#ifndef NO_DEV_CHECK
		ret = ov2710_i2c_read16(psCam->i2c, REG_SC_CMMN_CHIP_ID_0, &chipV);
		if(ret || OV2710_CHIP_VERSION != chipV)
		{
			LOG_ERROR("Failed to ensure that the i2c device has a compatible "\
				"OV2710 sensor! ret=%d chip_version=0x%x (expect chip 0x%x)\n",
				ret, chipV, OV2710_CHIP_VERSION);
			close(psCam->i2c);
			IMG_FREE(psCam);
			*phHandle = NULL;
			return IMG_ERROR_DEVICE_NOT_FOUND;
		}
	#endif //NO_DEV_CHECK

	#ifdef ov2710_debug
		// clean file
		{
			FILE *f = fopen(ov2710_debug, "w");
			fclose(f);
		}
	#endif //ov2710_debug
#endif //file_i2c

    // need i2c to have been found to read mode
    OV2710_GetModeInfo(psCam, psCam->ui16CurrentMode, &(psCam->currMode));

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

	LOG_DEBUG("Sensor initialised i2c path is %s\n", i2c_dev_path);
	
    return IMG_SUCCESS;
}

void OV2710_ApplyMode(OV2710CAM_STRUCT *psCam)
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
#if !defined(OV2710_COPY_REGS)
    IMG_UINT8 values[3];
#endif //OV2710_COPY_REGS
#ifdef INFOTM_ISP
	IMG_UINT8 ResetRegs[] =
	{
		0x31, 0x03, 0x03,
		0x30, 0x08, 0x82,
		0x30, 0x08, 0x42,
	};
#endif //INFOTM_ISP

    if (!psCam->currModeReg)
    {
        LOG_ERROR("current register modes not available!\n");
        return;
    }
        
    
    LOG_DEBUG("Writing I2C\n");

#ifdef INFOTM_ISP

	ov2710_i2c_write8(psCam->i2c, &ResetRegs[0], 6);
	usleep(6000);
	ov2710_i2c_write8(psCam->i2c, &ResetRegs[6], 3);
	usleep(6000);
#endif //INFOTM_ISP

    for(i = 0 ; i < psCam->nRegisters * 3 ; i += 3)
    {
#if !defined(OV2710_COPY_REGS)
        values[0] = psCam->currModeReg[i+0];
        values[1] = psCam->currModeReg[i+1];
        values[2] = psCam->currModeReg[i+2];
#endif //OV2710_COPY_REGS

        if (IS_REG(psCam->currModeReg, i, REG_FORMAT1))
        {
            flipIndex0 = i;
            if (psCam->ui8CurrentFlipping&SENSOR_FLIP_VERTICAL)
            {
#if defined(OV2710_COPY_REGS)
                psCam->currModeReg[i+2] = 6;
#else
                values[2] = 6;
#endif //OV2710_COPY_REGS
            }
            else
            {
#if defined(OV2710_COPY_REGS)
                psCam->currModeReg[i+2] = 0;
#else
                values[2] = 0;
#endif //OV2710_COPY_REGS
            }
        }
        if (IS_REG(psCam->currModeReg, i, REG_FORMAT2))
        {
            flipIndex1 = i;
            if (psCam->ui8CurrentFlipping&SENSOR_FLIP_HORIZONTAL)
            {
#if defined(OV2710_COPY_REGS)
                psCam->currModeReg[i+2] = 6;
#else
                values[2] = 6;
#endif //OV2710_COPY_REGS

            }
            else
            {
#if defined(OV2710_COPY_REGS)
                psCam->currModeReg[i+2] = 0;
#else
                values[2] = 0;
#endif //OV2710_COPY_REGS
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

#if !defined(OV2710_COPY_REGS)
        ov2710_i2c_write8(psCam->i2c, values, 3);
#endif //OV2710_COPY_REGS
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
        OV2710_ComputeExposure(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin,
            &(mode1280x720[exposureIndex]));
    }

    // should be found and fit in array
    IMG_ASSERT(exposureIndex+8 < psCam->nRegisters * 3);
    {
        OV2710_ComputeGains(psCam->flGain, &(mode1280x720[gainIndex]));
    }
#endif //0
    
#if defined(OV2710_COPY_REGS)
    ov2710_i2c_write8(psCam->i2c, psCam->currModeReg, psCam->nRegisters * 3);
#endif //OV2710_COPY_REGS
    
    LOG_DEBUG("Sensor ready to go\n");
    // setup the exposure setting information
}

IMG_RESULT OV2710_GetModeInfo(OV2710CAM_STRUCT *psCam, IMG_UINT16 ui16Mode,
    SENSOR_MODE *info)
{
    unsigned int n;
    IMG_RESULT ret = IMG_SUCCESS;
    
    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = OV2710_GetRegisters(psCam, ui16Mode,
        &nRegisters);

	double mipiclk = 0;
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

	//For ov2710 pll control
	double pll_prediv = 0;
	double pll_divp = 0;
	double pll_div45 = 0;
	double pll_div1to2p5 = 0;
	double pll_divs = 0;
	double pll_divm = 0;

    double trow = 0; // in micro seconds

    if (!modeReg)
    {
        LOG_ERROR("invalid mode %d\n", modeReg);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    nRegisters = ov2710_modes[ui16Mode].nRegisters;

    for (n = 0 ; n < nRegisters * 3 ; n += 3)
    {
    	//ov2710 mipi interface only 1-lane.
#if 0
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
#endif //0

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
		
///////////////////////////////////////////////////////////

        if (IS_REG(modeReg, n, REG_PLL_PREDIVIDER))
        {
            pll_prediv = pll_ctrl_predivider_val[modeReg[n + 2] & 0x07];
            LOG_DEBUG("pll_ctrl_predivider = 0x%x -> %lf\n", modeReg[n + 2], pll_prediv);
        }

        if (IS_REG(modeReg, n, REG_PLL_CTRL_02))
        {
            pll_divp = (double)(modeReg[n + 2] & 0x3f);
            LOG_DEBUG("pll_divp = 0x%x -> %lf\n", modeReg[n + 2], pll_divp);
        }
		
        if (IS_REG(modeReg, n, REG_PLL_CTRL_00))
        {
            pll_div45 = pll_ctrl_div45_val[modeReg[n + 2] & 0x03];
            LOG_DEBUG("pll_div45 = 0x%x -> %lf\n", modeReg[n + 2], pll_div45);

            pll_div1to2p5 = pll_ctrl_div1to2p5_val[modeReg[n + 2] >> 6];
            LOG_DEBUG("pll_div1to2p5 = 0x%x -> %lf\n", modeReg[n + 2], pll_div1to2p5);
        }

        if (IS_REG(modeReg, n, REG_PLL_CTRL_01))
        {
            pll_divs = (double) ((modeReg[n + 2] >> 4) + 1);
            LOG_DEBUG("pll_divs = 0x%x -> %lf\n", modeReg[n + 2], pll_divs);

			pll_divm = (double) ((modeReg[n + 2] & 0x0f) + 1);
			LOG_DEBUG("pll_divm = 0x%x -> %lf\n", modeReg[n + 2], pll_divm);
        }	
    }

    if (0.0 == pll_prediv || 0.0 == pll_divp || 0.0 == pll_div45
        || 0.0 == pll_div1to2p5 || 0.0 == pll_divs || 0.0 == pll_divm)
    {
        sclk = 120.0 * 1000 * 1000;
        LOG_WARNING("Did not find all PLL registers - assumes "\
            "sclk of %lf MHz\n", sclk);
    }
    else
    {
        // xclk is the refClock
        // vco = (xclk/pll_prediv) * pll_divp * pll_div45
        double vco = 0;
        // sclk = vco / pll_div1to2p5 / pll_divs / pll_divm / 4;        

        vco = (psCam->refClock / pll_prediv) * pll_divp * pll_div45;
		mipiclk = vco / pll_divs / pll_divm;
        sclk = vco / pll_div1to2p5 / pll_divs / pll_divm / 4;

        LOG_DEBUG("(xclk=%.2lfMHz / pll_prediv=%.2lf) * pll_divp=%.2lf * pll_div45=%.2lf\n",
            psCam->refClock/1000000, pll_prediv, pll_divp, pll_div45);

        LOG_DEBUG("(vco=%.2lfMHz / pll_divs=%.2lf / pll_divm=&.21f) = mipiclk=%.2lfMHz\n",
            vco/1000000, pll_divs, pll_divm, mipiclk/1000000);
	
        LOG_DEBUG("(vco=%.2lfMHz / pll_div1to2p5=%.2lf / pll_divs=%.2lf / pll_divm=&.21f / 4) = sclk=%.2lfMHz\n",
            vco/1000000, pll_div1to2p5, pll_divs, pll_divm, sclk/1000000);
    }

    hts_v = (hts[1] | (hts[0] << 8));	//2416
    vts_v = (vts[1] | (vts[0] << 8));	//1104
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

#ifndef INFOTM_ISP
	info->ui8MipiLanes = ((sc_cmmn_mipi_sc_ctrl << 5) & 0x3) + 1;
	info->ui8BitDepth = (sc_cmmm_bit_sel & 0x1F);

#else

	info->ui8MipiLanes = 1;
	info->ui8BitDepth = 10;
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP

#define SUPPORT_FLIP_MIRROR_BY_RESET_SENSOR 

int OV2710_setDefFlipMirror(OV2710CAM_STRUCT *psCam, int mode, int flag)
{
	IMG_UINT8 *registerArray = NULL;
	int nRegisters = 0, i = 0;
	int x = -1, y = -1, z = -1;
	int get_pos_done = 0;
	IMG_RESULT ret = 0xfe;

	printf("-->OV2710_SetDefFlipMirror %d\n", flag);
		


    registerArray = OV2710_GetRegisters(psCam, mode, &nRegisters);

	for (i = 0; i < nRegisters - 2; i += 3)
	{
		if (registerArray[i] == 0x38 && registerArray[i + 1] == 0x03)
		{
			x = i + 2;
			get_pos_done++;
		}
		else if (registerArray[i] == 0x36 && registerArray[i + 1] == 0x21)
		{
			y = i + 2;
			get_pos_done++;
		}
		else if (registerArray[i] == 0x38 && registerArray[i + 1] == 0x18)
		{
			z = i + 2;
			get_pos_done++;
		}

		if (get_pos_done == 3)
		{
			break;
		}

	}

	if (get_pos_done < 3)
	{
		printf("get_pos_done %d, nRegisters = %d\n", get_pos_done, nRegisters);
		return IMG_ERROR_FATAL;
	}
	
	switch (flag)
    {
	case SENSOR_FLIP_HORIZONTAL:
		registerArray[x] = 0x0a;
		registerArray[y] = 0x14;
		registerArray[z] = 0xc0;
		break;
	case SENSOR_FLIP_VERTICAL:
		registerArray[x] = 0x09;
		registerArray[y] = 0x04;
		registerArray[z] = 0xa0;
		break;
	case SENSOR_FLIP_BOTH:
		registerArray[x] = 0x09;
		registerArray[y] = 0x14;
		registerArray[z] = 0xe0;
		break;
	case SENSOR_FLIP_NONE:
	default:
		registerArray[x] = 0x0a;
		registerArray[y] = 0x04;
		registerArray[z] = 0x80;
		break;
	}
	return ret;
}

static IMG_RESULT OV2710_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{	
	OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

	printf("-->OV2710_SetFlipMirror old %d, new %d\n", psCam->ui8CurrentFlipping, flag);
#if 0	
	if (psCam->ui8CurrentFlipping == flag)
	{
		printf("-->no need OV2710_SetFlipMirror\n");
		return IMG_SUCCESS;
	}
#endif
#ifdef SUPPORT_FLIP_MIRROR_BY_RESET_SENSOR
	IMG_RESULT ret = 0xfe;
	ret = OV2710_setDefFlipMirror(psCam, psCam->ui16CurrentMode, flag);
	return ret;
#endif
    IMG_UINT8 aui8Regs[2];
    IMG_UINT8 Idx[9];

    switch (flag)
    {
    	case SENSOR_FLIP_NONE:
//    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_NONE)
//    		{
//    			printf("the sensor has been SENSOR_FLIP_NONE.\n");
//    			return IMG_SUCCESS;
//    		}
#if 1
            Idx[0] = 0x38;
            Idx[1] = 0x03;
            Idx[2] = 0x0a;
            
            Idx[3] = 0x36;
            Idx[4] = 0x21;
            Idx[5] = 0x04;
            
            Idx[6] = 0x38;
            Idx[7] = 0x18;
            Idx[8] = 0x80;

    		ov2710_i2c_write8(psCam->i2c, Idx, sizeof(Idx));
#else
			Idx[0] = 0x32;
            Idx[1] = 0x12;
            Idx[2] = 0x00;
			
            Idx[3] = 0x38;
            Idx[4] = 0x03;
            Idx[5] = 0x0a;
            
            Idx[6] = 0x36;
            Idx[7] = 0x21;
            Idx[8] = 0x04;
            
            Idx[9] = 0x38;
            Idx[10] = 0x18;
            Idx[11] = 0x80;
			
            Idx[12] = 0x32;
            Idx[13] = 0x12;
            Idx[14] = 0x10;

            Idx[15] = 0x32;
            Idx[16] = 0x12;
            Idx[17] = 0xa0;

    		ov2710_i2c_write8(psCam->i2c, Idx, sizeof(Idx));
#endif
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    		break;

    	case SENSOR_FLIP_HORIZONTAL:
//    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_HORIZONTAL)
//    		{
//    			printf("the sensor has been SENSOR_FLIP_HORIZONTAL.\n");
//    			return IMG_SUCCESS;
//    		}
#if 1
            Idx[0] = 0x38;
            Idx[1] = 0x03;
            Idx[2] = 0x0a;
            
            Idx[3] = 0x36;
            Idx[4] = 0x21;
            Idx[5] = 0x14;
            
            Idx[6] = 0x38;
            Idx[7] = 0x18;
            Idx[8] = 0xc0;

    		ov2710_i2c_write8(psCam->i2c, Idx, sizeof(Idx));
#else
			Idx[0] = 0x32;
            Idx[1] = 0x12;
            Idx[2] = 0x00;

			Idx[3] = 0x38;
            Idx[4] = 0x03;
            Idx[5] = 0x0a;
            
            Idx[6] = 0x36;
            Idx[7] = 0x21;
            Idx[8] = 0x14;
            
            Idx[9] = 0x38;
            Idx[10] = 0x18;
            Idx[11] = 0xc0;

            Idx[12] = 0x32;
            Idx[13] = 0x12;
            Idx[14] = 0x10;

            Idx[15] = 0x32;
            Idx[16] = 0x12;
            Idx[17] = 0xa0;

    		ov2710_i2c_write8(psCam->i2c, Idx, sizeof(Idx));
#endif

    		psCam->ui8CurrentFlipping = SENSOR_FLIP_HORIZONTAL;
    		break;

    	case SENSOR_FLIP_VERTICAL:
//    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_VERTICAL)
//    		{
//    			printf("the sensor has been SENSOR_FLIP_VERTICAL.\n");
//    			return IMG_SUCCESS;
//    		}
#if 0
            Idx[0] = 0x38;
            Idx[1] = 0x03;
            Idx[2] = 0x09;

            Idx[3] = 0x36;
            Idx[4] = 0x21;
            Idx[5] = 0x04;

            Idx[6] = 0x38;
            Idx[7] = 0x18;
            Idx[8] = 0xa0;

    		ov2710_i2c_write8(psCam->i2c, Idx, sizeof(Idx));
#else
#if 1
            Idx[0] = 0x36;
            Idx[1] = 0x21;
            Idx[2] = 0x04;

            Idx[3] = 0x38;
            Idx[4] = 0x18;
            Idx[5] = 0x80;

            ov2710_i2c_write8(psCam->i2c, Idx, 6);

            Idx[0] = 0x38;
            Idx[1] = 0x03;
            Idx[2] = 0x09;

            Idx[3] = 0x38;
            Idx[4] = 0x18;
            Idx[5] = 0xa0;

            ov2710_i2c_write8(psCam->i2c, Idx, 6);
#else
			Idx[0] = 0x32;
            Idx[1] = 0x12;
            Idx[2] = 0x00;

            Idx[3] = 0x36;
            Idx[4] = 0x21;
            Idx[5] = 0x04;

            Idx[6] = 0x38;
            Idx[7] = 0x18;
            Idx[8] = 0x80;
			
            Idx[9] = 0x32;
            Idx[10] = 0x12;
            Idx[11] = 0x10;

            Idx[12] = 0x32;
            Idx[13] = 0x12;
            Idx[14] = 0xa0;

            ov2710_i2c_write8(psCam->i2c, Idx, 15);

			Idx[0] = 0x32;
            Idx[1] = 0x12;
            Idx[2] = 0x00;

            Idx[3] = 0x38;
            Idx[4] = 0x03;
            Idx[5] = 0x09;

            Idx[6] = 0x38;
            Idx[7] = 0x18;
            Idx[8] = 0xa0;

            Idx[9] = 0x32;
            Idx[10] = 0x12;
            Idx[11] = 0x10;

            Idx[12] = 0x32;
            Idx[13] = 0x12;
            Idx[14] = 0xa0;
			
            ov2710_i2c_write8(psCam->i2c, Idx, 15);
#endif
#endif
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_VERTICAL;
    		break;

    	case SENSOR_FLIP_BOTH:
//    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_BOTH)
//    		{
//    			printf("the sensor has been SENSOR_FLIP_BOTH.\n");
//    			return IMG_SUCCESS;
//    		}
#if 1
            Idx[0] = 0x38;
            Idx[1] = 0x03;
            Idx[2] = 0x09;
            
            Idx[3] = 0x36;
            Idx[4] = 0x21;
            Idx[5] = 0x14;
            
            Idx[6] = 0x38;
            Idx[7] = 0x18;
            Idx[8] = 0xe0;

    		ov2710_i2c_write8(psCam->i2c, Idx, sizeof(Idx));
#else
			Idx[0] = 0x32;
            Idx[1] = 0x12;
            Idx[2] = 0x00;

            Idx[0] = 0x38;
            Idx[1] = 0x03;
            Idx[2] = 0x09;
            
            Idx[3] = 0x36;
            Idx[4] = 0x21;
            Idx[5] = 0x14;
            
            Idx[6] = 0x38;
            Idx[7] = 0x18;
            Idx[8] = 0xe0;

            Idx[12] = 0x32;
            Idx[13] = 0x12;
            Idx[14] = 0x10;

            Idx[15] = 0x32;
            Idx[16] = 0x12;
            Idx[17] = 0xa0;

    		ov2710_i2c_write8(psCam->i2c, Idx, sizeof(Idx));
#endif
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
    		break;

    }
    	return IMG_SUCCESS;
}

static IMG_RESULT OV2710_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
#if defined(ENABLE_GROUP)
    static IMG_UINT8 group_id = 0;
#endif

    unsigned int nIndex = 0;
    IMG_UINT32 ui32Lines;
    IMG_UINT8 exposureRegs[]={
#if defined(ENABLE_GROUP)
        0x32, 0x12, 0x00,  // enable group 0

        0x35, 0x00, 0x00,  //
        0x35, 0x01, 0x2E,  //
        0x35, 0x02, 0x80,  //

        0x35, 0x0a, 0x00,  //
        0x35, 0x0b, 0x00,  //

        0x32, 0x12, 0x10,  // end hold of group 0 register writes
        0x32, 0x12, 0xa0,  // quick launch group 0
#else
        0x35, 0x00, 0x00,  //
        0x35, 0x01, 0x2E,  //
        0x35, 0x02, 0x80,  //

        0x35, 0x0a, 0x00,  //
        0x35, 0x0b, 0x00,  //
#endif
	};

    OV2710CAM_STRUCT *psCam = container_of(hHandle, OV2710CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui32Exposure = ui32Exposure;
    psCam->flGain = flGain;

	ui32Lines = psCam->ui32Exposure / psCam->currMode.dbExposureMin;
	//	if ((1102 == ui32Lines) || (1103 == ui32Lines)) {
	//		//ui32Lines = asModes[psCam->ui16CurrentMode].ui16VerticalTotal;
	//		ui32Lines = 1104;
	//	}
	if ((1098 < ui32Lines) && (1104 >= ui32Lines)) {
		ui32Lines = 1098;
	}

    while (nIndex < (sizeof(asOV2710GainValues) / sizeof(OV2710_GAIN_TABLE)))
    {
        if(asOV2710GainValues[nIndex].flGain > flGain)
            break;
        nIndex++;
    }

    if (nIndex > 0)
    {
        nIndex = nIndex - 1;
    }

#if defined(ENABLE_GROUP)
    if (group_id == 0)
    {
        group_id = 1;
    }
    else
    {
        exposureRegs[2] = 0x01;
        exposureRegs[20] = 0x11;
        exposureRegs[23] = 0xa1;
        group_id = 0;
    }

    exposureRegs[5] = (ui32Lines >> 12) & 0xf;
    exposureRegs[8] = (ui32Lines >> 4) & 0xff;
    exposureRegs[11] = (ui32Lines << 4) & 0xff;

    exposureRegs[14] = asOV2710GainValues[nIndex].ui8GainRegValHi;
    exposureRegs[17] = asOV2710GainValues[nIndex].ui8GainRegValLo;

#else
    exposureRegs[2] = (ui32Lines >> 12) & 0xf;
    exposureRegs[5] = (ui32Lines >> 4) & 0xff;
    exposureRegs[8] = (ui32Lines << 4) & 0xff;

    exposureRegs[11] = asOV2710GainValues[nIndex].ui8GainRegValHi;
    exposureRegs[14] = asOV2710GainValues[nIndex].ui8GainRegValLo;
#endif

    LOG_DEBUG("Exposure Val %x\n",(psCam->ui32Exposure/psCam->currMode.ui32ExposureMin));
#if defined(ENABLE_AE_DEBUG_MSG)
    printf("OV2710_SetExposure Exposure = 0x%08X (%d)\n", psCam->ui32Exposure, psCam->ui32Exposure);
#endif

    //printf("-->OV2710_SetGainAndExposure Gain = %f, Exposure = 0x%08X (%d)\n", psCam->flGain, psCam->ui32Exposure, psCam->ui32Exposure);
#if 1
    ov2710_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));
#else
    ov2710_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs[0])*9);
    usleep(1000);
    ov2710_i2c_write8(psCam->i2c, &exposureRegs[3], sizeof(exposureRegs[3])*6);
#endif

#if defined(ENABLE_GROUP)
    //usleep(33*1000);
#endif
    /*
    aui16Regs[0]=0x3012;// COARSE_INTEGRATION_TIME


    SensorI2CWrite16(psCam->psI2C,aui16Regs,2);
    */
    return IMG_SUCCESS;
}
#endif
