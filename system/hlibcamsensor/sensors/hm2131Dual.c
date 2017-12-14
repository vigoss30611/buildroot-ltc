/**
 ******************************************************************************
@file hm2131Dual.c

@brief HM2131Dual camera implementation

To add a new mode declare a new array containing 8b values:imager
- 2 first values are addresses
- last 8b is the value

The array MUST finish with STOP_REG, STOP_REG, STOP_REG so that the size can
be computed by the HM2131Dual_GetRegisters()

The array can have delay of X ms using DELAY_REG, X, DELAY_REG

Add the array to hm2131Dual_modes[] with the relevant flipping and a size of 0

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

#include "sensors/hm2131Dual.h"
#include "sensors/xc9080_driver.h"
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

//#define DISABLE_INFOTM_AE

#define HM2131Dual_FLIP_MIRROR_FRIST

#ifdef INFOTM_ISP
static IMG_RESULT HM2131Dual_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag);
static IMG_RESULT HM2131Dual_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
static IMG_RESULT HM2131Dual_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed);
static IMG_RESULT HM2131Dual_SetFPS(SENSOR_HANDLE hHandle, double fps);

//#define OTP_DBG_LOG
static IMG_UINT8* HM2131Dual_read_sensor_calibration_version(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/);
static IMG_UINT8* HM2131Dual_read_sensor_calibration_data(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain);
static IMG_RESULT HM2131Dual_update_sensor_wb_gain(SENSOR_HANDLE hHandle, int infotm_method, IMG_FLOAT awb_convert_gain, IMG_UINT16 version);

#endif //INFOTM_ISP

/* disabled as i2c drivers locks up if device is not present */
#define NO_DEV_CHECK

#define LOG_TAG "HM2131Dual_SENSOR"
#include <felixcommon/userlog.h>

/** @ the setup for mode does not work if enabled is not call just after
 * - temporarily do the whole of the setup in in enable function
 * - can remove that define and its checking once fixed
 */
// if defined writes all registers at enable time instead of configure
#define DO_SETUP_IN_ENABLE

#define I2C_HM2131_ADDR 0x42
#define I2C_HM2131_ADDR_1 0x44

#define HM2131Dual_BC_WRITE_ADDR (0x42 >> 1)

#define AUTOFOCUS_WRITE_ADDR (0x18 >> 1)
/* ---------upper registers are not verified ---------------*/

#define HM2131Dual_SENSOR_VERSION "not-verified"

#define I2C_HM2131_ADDR_DUAL (0x48 >> 1)

#define HM2131Dual_ID_HIGH		0x21
#define HM2131Dual_ID_LOW		0x31

#define HM2131Dual_ID_HIGH_REG	0x0000
#define HM2131Dual_ID_LOW_REG	0x0001

// fake register value to interpret next value as delay in ms
#define DELAY_REG 0xff
// not a real register - marks the end of register array
#define STOP_REG 0xfe

/** @brief sensor number of IMG PHY */
#define HM2131Dual_PHY_NUM 1

/*
 * Choice:
 * copy of the registers to apply because we want to apply them all at
 * once because some registers need 2 writes while others don't (may have
 * an effect on stability but was not showing in testing)
 * 
 * If not defined will apply values register by register and not need a copy
 * of the registers for the active mode.
 */

// uncomment to write i2c commands to file
//#define hm2131Dual_debug "hm2131Dual_write.txt"

#ifdef WIN32 // on windows we do not need to sleep to wait for the bus
static void usleep(int t)
{
    (void)t;
}
#endif

typedef struct _HM2131Dualcam_struct
{
    SENSOR_FUNCS sFuncs;

    // in MHz
    double refClock;

    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
    IMG_UINT8 ui8CurrentFlipping;
    IMG_BOOL8 bEnabled;

    SENSOR_MODE currMode;
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
}HM2131DualCAM_STRUCT;

static void HM2131Dual_ApplyMode(HM2131DualCAM_STRUCT *psCam);
static void HM2131Dual_ComputeExposure(IMG_UINT32 ui32Exposure,
    IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureSequence);
static IMG_RESULT HM2131Dual_GetModeInfo(HM2131DualCAM_STRUCT *psCam,
    IMG_UINT16 ui16Mode, SENSOR_MODE *info);
static IMG_UINT16 HM2131Dual_CalculateFocus(const IMG_UINT16 *pui16DACDist,
        const IMG_UINT16 *pui16DACValues, IMG_UINT8 ui8Entries,
        IMG_UINT16 ui16Requested);
// used before implementation therefore declared here
static IMG_RESULT HM2131Dual_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus);

// from omnivision data sheet

#define REG_H_OUTPUT_SIZE_0 0x034C // bits [3;0] size [11;8]
#define REG_H_OUTPUT_SIZE_1 0x034D // bits [7;0] size [7;0]
#define REG_V_OUTPUT_SIZE_0 0x034E // bits [3;0] size [11;8]
#define REG_V_OUTPUT_SIZE_1 0x034F // bits [7;0] size [7;0]

#define REG_TIMING_HTS_0 0x0342 // bits [7;0] size [14;8]
#define REG_TIMING_HTS_1 0x0343 // bits [7;0] size [7;0]
#define REG_TIMING_VTS_0 0x0340 // bits [7;0] size [14;8]
#define REG_TIMING_VTS_1 0x0341 // bits [7;0] size [7;0]

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

static IMG_UINT8 g_HM2131Dual_ui8LinesValHiBackup = 0xff;
static IMG_UINT8 g_HM2131Dual_ui8LinesValLoBackup = 0xff;

static IMG_UINT8 g_HM2131Dual_ui8GainValBackup = 0xff;

// minimum focus in millimetres
static const unsigned int hm2131Dual_focus_dac_min = 50;

// maximum focus in millimetres, if >= then focus is infinity
static const unsigned int hm2131Dual_focus_dac_max = 600;

// focus values for the hm2131Dual_focus_dac_dist
static const IMG_UINT16 hm2131Dual_focus_dac_val[] = {
    0x3ff, 0x180, 0x120, 0x100, 0x000
};

// distances in millimetres for the hm2131Dual_focus_dac_val
static const IMG_UINT16 hm2131Dual_focus_dac_dist[] = {
    50, 150, 300, 500, 600
};

//
#ifdef INFOTM_ISP
static IMG_UINT32 i2c_addr = 0;
#endif

#define SENSOR_SLAVE_NUM 1
char hm2131_i2c_slave_addr[SENSOR_SLAVE_NUM] = {I2C_HM2131_ADDR_DUAL/*HM2131Dual_BC_WRITE_ADDR*/};

// modes declaration
//
static IMG_UINT8 ui8ModeRegs_2160_1080_60_4lane[] =
{
#if 0
	0x01, 0x03, 0x00, // software reset
	0x03, 0x04, 0x2A, // ADCPLL_CFG [1:0] pll_cp_bias_d<1:0>  [3:2]pll_rp_ctrl_d<1:0> [4]pll_bypass_d [5]pll_en_d
	0x03, 0x05, 0x0C, // PRE_PLL_CLK_DIV [4:0]pll_pre_div_d<4:0> [5]pll_post_div_type_d
	0x03, 0x07, 0x55, // PLL_MULTIPLIER [7:0]pll_post_div_d
	0x03, 0x03, 0x04, // VT_SYS_CLK_DIV
	0x03, 0x09, 0x00, // OP_PIX_CLK_DIV [1:0]op_pix_div_d
	0x03, 0x0A, 0x0A, // MIPI PLL configuration [7:0]mipi_pll3_d
	0x03, 0x0D, 0x02, // PRE_MIPI_PLL_CLK_DIV [4:0]mipi_pll_clk_div_d  mipi_pll_clk_div_d= Reg.0x030D[4:0]
	0x03, 0x0F, 0x14, // PLL_MIPI_MULTIPLIER [6:0]pll_mipi_multiplier_d  pll_mipi_multiplier_d= {Reg.0x030F[7],Reg.0x030F[6:0]}
	
	0x52, 0x68, 0x01, // SSCG Control Registers [0x5264 \u2013 0x5269]
	0x52, 0x64, 0x24,
	0x52, 0x65, 0x92,
	0x52, 0x66, 0x23,
	0x52, 0x67, 0x07,
	0x52, 0x69, 0x02,
	
	0x01, 0x00, 0x02, // MODE_SELECT [1:0] Sensor mode selection 00 : Standby 10 : Power up 11 : Streaming
	0x01, 0x00, 0x02,
	
	0x01, 0x11, 0x01, // CSI_LANE_MODE MIPI Lane Selection 0  1: Lanes, 1:  2 Lanes
	0x01, 0x12, 0x0A, // CSI_DATA_FORMAT_H [15:8]  Data format selection RAW8 : 0x0808  RAW10 : 0x0A0A
	0x01, 0x13, 0x0A, // CSI_DATA_FORMAT_L [7:0]  RAW12 : 0x0C0C
	
	0x4B, 0x20, 0x8E, // MIPI Programming Registers 
	                // [3:0] : Reserved [4] : USE_LS_LE 1 : Use LS/LE 0 : Not use LS/LE [6:5] : 
	                // CLK_LANE_OPTION 00 : Clock is always turn on 01 : Clock is on while 
	                // sending packet 10 : Clock is on during whole frame 11 : Clock is on between LS 
	                // & LE [7] : CLK_LANE_ON
	0x4B, 0x18, 0x12,
	0x4B, 0x02, 0x05,
	0x4B, 0x43, 0x07,
	0x4B, 0x05, 0x1C,
	0x4B, 0x0E, 0x00,
	0x4B, 0x0F, 0x0D,
	0x4B, 0x06, 0x06,
	0x4B, 0x39, 0x0B,
	0x4B, 0x42, 0x07,
	0x4B, 0x03, 0x0C,
	0x4B, 0x04, 0x07,
	0x4B, 0x3A, 0x0B,
	0x4B, 0x51, 0x80,
	0x4B, 0x52, 0x09,
	
	0xFF, 0xFF, 0x05, // ??? delay ?
	
	0x4B, 0x52, 0xC9,
	0x4B, 0x57, 0x07,
	0x4B, 0x68, 0x6B,
	
	0x03, 0x50, 0x37, // Digital Window CFG [5] : Analog Dgain Enable [4] : Digital Gain  Enable [1] : hcropping enable [0] : Digital window enable
	
	0x50, 0x30, 0x10,
	0x50, 0x32, 0x02,
	0x50, 0x33, 0xD1,
	0x50, 0x34, 0x01,
	0x50, 0x35, 0x67,
	
	0x52, 0x29, 0x90, // Output enable [0] : Output enable
	
	0x50, 0x61, 0x00, // Parallel output control byte [0] : Parallel enable
	
	0x50, 0x62, 0x94,
	0x50, 0xF5, 0x06,
	0x52, 0x30, 0x00,
	0x52, 0x6C, 0x00,
	0x52, 0x0B, 0x41,
	0x52, 0x54, 0x08,
	0x52, 0x2B, 0x00,
	
	0x41, 0x44, 0x08,
	0x41, 0x48, 0x03,
	
	0x40, 0x24, 0x40, // PCKCTRL [6] : MIPI enable
	
	0x4B, 0x66, 0x00,
	0x4B, 0x31, 0x06,
	
	0x02, 0x02, 0x01, //0x04, // exposure H
	0x02, 0x03, 0x08, //0x50, // exposure L
	
	0x03, 0x40, 0x04,//0x04 // frame_length_lines H
	0x03, 0x41, 0x52,//0x52 // frame_length_lines L
	
	0x03, 0x42, 0x05, // line_length_pck H
	0x03, 0x43, 0x00, // line_length_pck L
	
	0x03, 0x4C, 0x07, // x_output_size
	0x03, 0x4D, 0x88,
	
	0x03, 0x4E, 0x04, // y_output_size
	0x03, 0x4F, 0x40,
	
	0x01, 0x01, 0x00, // flip&mirror : Image Orientation [0]:Horizontal mirror En [1]:Vertical flip En
	
	0x40, 0x20, 0x10, // [4]: pclk display parallel
                    // 0: pclk = pclk
                    // 1: pclk = ~pclk
                    // [6]:vsync display parallel
                    // 0: vsync = vsync
                    // 1: vsync = ~vsync
                    // [7]hsync display parallel
                    // 0: hsync = hsync
                    // 1: hsync = ~hsync
	
	0x50, 0xDD, 0x00, //0x01, // GAIN STRATEGY 0 : SMIA gain code 1 : HII gain code
	0x03, 0x50, 0x37, // Digital Window CFG
	
	0x41, 0x31, 0x01, // RAWCtrlByte4 [0] BLI enable for gain blocks [7] Reorder memory bypass enable (DISABLE REORDER)
	0x41, 0x32, 0x20,	//BLI target *** 0x4132 should be programmed to the same value as BLCTGT 0x501F
	
	0x50, 0x11, 0x00,
	0x50, 0x15, 0x00,
	0x50, 0x1D, 0x1C,
	0x50, 0x1E, 0x00,
	
	0x50, 0x1F, 0x20,	// BLCTGT blc level
	
	0x50, 0xD5, 0xF0,
	0x50, 0xD7, 0x12,
	0x50, 0xBB, 0x14,
	
	0x50, 0x40, 0x07, // Thermal Signal Register (temp enable)
	
	0x50, 0xB7, 0x00,
	0x50, 0xB8, 0x10,
	0x50, 0xB9, 0xFF,
	0x50, 0xBA, 0xFF,
	
	0x52, 0x00, 0x26,
	0x52, 0x01, 0x00,
	0x52, 0x02, 0x00,
	0x52, 0x03, 0x00,
	0x52, 0x17, 0x01,
	0x52, 0x19, 0x01,
	0x52, 0x34, 0x01,
	0x52, 0x6B, 0x03,
	
	0x4C, 0x00, 0x00,
	
	0x03, 0x10, 0x00,
	
	0x4B, 0x31, 0x06,
	0x4B, 0x3B, 0x02,
	0x4B, 0x44, 0x0C,
	0x4B, 0x45, 0x01,
	
	0x50, 0xA1, 0x00,
	0x50, 0xAA, 0x2E,
	0x50, 0xAC, 0x44,
	0x50, 0xAB, 0x04,
	0x50, 0xA0, 0xB0,
	0x50, 0xA2, 0x1B,
	0x50, 0xAF, 0x00,
	
	0x52, 0x08, 0x55,
	0x52, 0x09, 0x03,
	0x52, 0x0D, 0x40,
	0x52, 0x14, 0x18,
	0x52, 0x15, 0x03,
	0x52, 0x16, 0x00,
	0x52, 0x1A, 0x10,
	0x52, 0x1B, 0x24,
	0x52, 0x32, 0x04,
	0x52, 0x33, 0x03,
	
	0x51, 0x06, 0xF0,
	0x51, 0x0E, 0xC1,
	0x51, 0x66, 0xF0,
	0x51, 0x6E, 0xC1,
	0x51, 0x96, 0xF0,
	0x51, 0x9E, 0xC1,
	0x51, 0xC0, 0x80,
	0x51, 0xC4, 0x80,
	0x51, 0xC8, 0x80,
	0x51, 0xCC, 0x80,
	0x51, 0xD0, 0x80,
	0x51, 0xD4, 0x80,
	0x51, 0xD8, 0x80,
	0x51, 0xDC, 0x80,
	0x51, 0xC1, 0x03,
	0x51, 0xC5, 0x13,
	0x51, 0xC9, 0x17,
	0x51, 0xCD, 0x27,
	0x51, 0xD1, 0x27,
	0x51, 0xD5, 0x2B,
	0x51, 0xD9, 0x2B,
	0x51, 0xDD, 0x2B,
	0x51, 0xC2, 0x4B,
	0x51, 0xC6, 0x4B,
	0x51, 0xCA, 0x4B,
	0x51, 0xCE, 0x49,
	0x51, 0xD2, 0x49,
	0x51, 0xD6, 0x49,
	0x51, 0xDA, 0x49,
	0x51, 0xDE, 0x49,
	0x51, 0xC3, 0x10,
	0x51, 0xC7, 0x18,
	0x51, 0xCB, 0x10,
	0x51, 0xCF, 0x08,
	0x51, 0xD3, 0x08,
	0x51, 0xD7, 0x08,
	0x51, 0xDB, 0x08,
	0x51, 0xDF, 0x00,
	0x51, 0xE0, 0x94,
	0x51, 0xE2, 0x94,
	0x51, 0xE4, 0x94,
	0x51, 0xE6, 0x94,
	0x51, 0xE1, 0x00,
	0x51, 0xE3, 0x00,
	0x51, 0xE5, 0x00,
	0x51, 0xE7, 0x00,
	
	0x52, 0x64, 0x23, // SSCG Control Registers
	0x52, 0x65, 0x07,
	0x52, 0x66, 0x24,
	0x52, 0x67, 0x92,
	0x52, 0x68, 0x01,
	
	0xBA, 0xA2, 0xC0,
	0xBA, 0xA2, 0x40,
	0xBA, 0x90, 0x01,
	0xBA, 0x93, 0x02,
	
	0x31, 0x10, 0x0B, // Bad Pixel Correction [0] : Static BPC enable [1] : Dynamic BPC enable
	
	0x37, 0x3E, 0x8A,
	0x37, 0x3F, 0x8A,
	
	0x37, 0x01, 0x0A,
	0x37, 0x09, 0x0A,
	0x37, 0x03, 0x08,
	0x37, 0x0B, 0x08,
	0x37, 0x05, 0x0F,
	0x37, 0x0D, 0x0F,
	0x37, 0x13, 0x00,
	0x37, 0x17, 0x00,
	
	0x50, 0x43, 0x01,
	0x50, 0x40, 0x05, // Thermal Signal Register
	0x50, 0x44, 0x07,
	
	0x60, 0x00, 0x0F,
	0x60, 0x01, 0xFF,
	0x60, 0x02, 0x1F,
	0x60, 0x03, 0xFF,
	0x60, 0x04, 0xC2,
	0x60, 0x05, 0x00,
	0x60, 0x06, 0x00,
	0x60, 0x07, 0x00,
	0x60, 0x08, 0x00,
	0x60, 0x09, 0x00,
	0x60, 0x0A, 0x00,
	0x60, 0x0B, 0x00,
	0x60, 0x0C, 0x00,
	0x60, 0x0D, 0x20,
	0x60, 0x0E, 0x00,
	0x60, 0x0F, 0xA1,
	0x60, 0x10, 0x01,
	0x60, 0x11, 0x00,
	0x60, 0x12, 0x06,
	0x60, 0x13, 0x00,
	0x60, 0x14, 0x0B,
	0x60, 0x15, 0x00,
	0x60, 0x16, 0x14,
	0x60, 0x17, 0x00,
	0x60, 0x18, 0x25,
	0x60, 0x19, 0x00,
	0x60, 0x1A, 0x43,
	0x60, 0x1B, 0x00,
	0x60, 0x1C, 0x82,

    // 16-bit sensor Part number MODEL_ID_H
	0x00, 0x00, 0x00,

	0x01, 0x04, 0x01, // GRP_PARAM_HOLD Group parameter hold    0  consume  1 hold
	0x01, 0x04, 0x00,
	
	0x01, 0x00, 0x03, // MODE_SELECT [1:0] Sensor mode selection   00 : Standby 10 : Power up 11 : Streamings
#else
    0x01, 0x03,0x00,
    0x03, 0x04,0x2A,
    0x03, 0x05,0x0C,
    0x03, 0x07,0x55,
    0x03, 0x03,0x04,
    0x03, 0x09,0x00,
    0x03, 0x0A,0x0A,
    0x03, 0x0D,0x02,
    0x03, 0x0F,0x14,
    0x52, 0x68,0x01,
    0x52, 0x64,0x24,
    0x52, 0x65,0x92,
    0x52, 0x66,0x23,
    0x52, 0x67,0x07,
    0x52, 0x69,0x02,
    0x01, 0x00,0x02,
    0x01, 0x00,0x02,
    0x01, 0x11,0x01,
    0x01, 0x12,0x0A,
    0x01, 0x13,0x0A,
    0x4B, 0x20,0x8E,
    0x4B, 0x18,0x12,
    0x4B, 0x02,0x05,
    0x4B, 0x43,0x07,
    0x4B, 0x05,0x1C,
    0x4B, 0x0E,0x00,
    0x4B, 0x0F,0x0D,
    0x4B, 0x06,0x06,
    0x4B, 0x39,0x0B,
    0x4B, 0x42,0x07,
    0x4B, 0x03,0x0C,
    0x4B, 0x04,0x07,
    0x4B, 0x3A,0x0B,
    0x4B, 0x51,0x80,
    0x4B, 0x52,0x09,
    0xFF, 0xFF,0x05,
    0x4B, 0x52,0xC9,
    0x4B, 0x57,0x07,
    0x4B, 0x68,0x6B,
    0x03, 0x50,0x37,
    0x50, 0x30,0x10,
    0x50, 0x32,0x02,
    0x50, 0x33,0xD1,
    0x50, 0x34,0x01,
    0x50, 0x35,0x67,
    0x52, 0x29,0x90,
    0x50, 0x61,0x00,
    0x50, 0x62,0x94,
    0x50, 0xF5,0x06,
    0x52, 0x30,0x00,
    0x52, 0x6C,0x00,
    0x52, 0x0B,0x41,
    0x52, 0x54,0x08,
    0x52, 0x2B,0x00,
    0x41, 0x44,0x08,
    0x41, 0x48,0x03,
    0x40, 0x24,0x40,
    0x4B, 0x66,0x00,
    0x4B, 0x31,0x06,
    
    0x02, 0x02,0x01,//0x04,
    0x02, 0x03,0x08,//0x50,
    
    0x03, 0x40,0x04,    //VTS
    0x03, 0x41,0x52,
    0x03, 0x42,0x05,    //HTS
    0x03, 0x43,0x00,
    0x03, 0x4C,0x07,
    0x03, 0x4D,0x88,
    0x03, 0x4E,0x04,
    0x03, 0x4F,0x40,
    0x01, 0x01,0x00,
    0x40, 0x20,0x10,
    0x50, 0xDD,0x00,  //0x01  Gain Strategy
    0x03, 0x50,0x37,
    0x41, 0x31,0x01,
    0x41, 0x32,0x20,    //BLI target
    0x50, 0x11,0x00,
    0x50, 0x15,0x00,
    0x50, 0x1D,0x1C,
    0x50, 0x1E,0x00,
    0x50, 0x1F,0x20,
    0x50, 0xD5,0xF0,
    0x50, 0xD7,0x12,
    0x50, 0xBB,0x14,
    0x50, 0x40,0x07,
    0x50, 0xB7,0x00,
    0x50, 0xB8,0x10,
    0x50, 0xB9,0xFF,
    0x50, 0xBA,0xFF,
    0x52, 0x00,0x26,
    0x52, 0x01,0x00,
    0x52, 0x02,0x00,
    0x52, 0x03,0x00,
    0x52, 0x17,0x01,
    0x52, 0x19,0x01,
    0x52, 0x34,0x01,
    0x52, 0x6B,0x03,
    0x4C, 0x00,0x00,
    0x03, 0x10,0x00,
    0x4B, 0x31,0x06,
    0x4B, 0x3B,0x02,
    0x4B, 0x44,0x0C,
    0x4B, 0x45,0x01,
    0x50, 0xA1,0x00,
    0x50, 0xAA,0x2E,
    0x50, 0xAC,0x44,
    0x50, 0xAB,0x04,
    0x50, 0xA0,0xB0,
    0x50, 0xA2,0x1B,
    0x50, 0xAF,0x00,
    0x52, 0x08,0x55,
    0x52, 0x09,0x03,
    0x52, 0x0D,0x40,
    0x52, 0x14,0x18,
    0x52, 0x15,0x03,
    0x52, 0x16,0x00,
    0x52, 0x1A,0x10,
    0x52, 0x1B,0x24,
    0x52, 0x32,0x04,
    0x52, 0x33,0x03,
    0x51, 0x06,0xF0,
    0x51, 0x0E,0xC1,
    0x51, 0x66,0xF0,
    0x51, 0x6E,0xC1,
    0x51, 0x96,0xF0,
    0x51, 0x9E,0xC1,
    0x51, 0xC0,0x80,
    0x51, 0xC4,0x80,
    0x51, 0xC8,0x80,
    0x51, 0xCC,0x80,
    0x51, 0xD0,0x80,
    0x51, 0xD4,0x80,
    0x51, 0xD8,0x80,
    0x51, 0xDC,0x80,
    0x51, 0xC1,0x03,
    0x51, 0xC5,0x13,
    0x51, 0xC9,0x17,
    0x51, 0xCD,0x27,
    0x51, 0xD1,0x27,
    0x51, 0xD5,0x2B,
    0x51, 0xD9,0x2B,
    0x51, 0xDD,0x2B,
    0x51, 0xC2,0x4B,
    0x51, 0xC6,0x4B,
    0x51, 0xCA,0x4B,
    0x51, 0xCE,0x49,
    0x51, 0xD2,0x49,
    0x51, 0xD6,0x49,
    0x51, 0xDA,0x49,
    0x51, 0xDE,0x49,
    0x51, 0xC3,0x10,
    0x51, 0xC7,0x18,
    0x51, 0xCB,0x10,
    0x51, 0xCF,0x08,
    0x51, 0xD3,0x08,
    0x51, 0xD7,0x08,
    0x51, 0xDB,0x08,
    0x51, 0xDF,0x00,
    0x51, 0xE0,0x94,
    0x51, 0xE2,0x94,
    0x51, 0xE4,0x94,
    0x51, 0xE6,0x94,
    0x51, 0xE1,0x00,
    0x51, 0xE3,0x00,
    0x51, 0xE5,0x00,
    0x51, 0xE7,0x00,
    0x52, 0x64,0x23,
    0x52, 0x65,0x07,
    0x52, 0x66,0x24,
    0x52, 0x67,0x92,
    0x52, 0x68,0x01,
    0xBA, 0xA2,0xC0,
    0xBA, 0xA2,0x40,
    0xBA, 0x90,0x01,
    0xBA, 0x93,0x02,
    0x31, 0x10,0x0B,
    0x37, 0x3E,0x8A,
    0x37, 0x3F,0x8A,
    0x37, 0x01,0x0A,
    0x37, 0x09,0x0A,
    0x37, 0x03,0x08,
    0x37, 0x0B,0x08,
    0x37, 0x05,0x0F,
    0x37, 0x0D,0x0F,
    0x37, 0x13,0x00,
    0x37, 0x17,0x00,
    0x50, 0x43,0x01,
    0x50, 0x40,0x05,
    0x50, 0x44,0x07,
    0x60, 0x00,0x0F,
    0x60, 0x01,0xFF,
    0x60, 0x02,0x1F,
    0x60, 0x03,0xFF,
    0x60, 0x04,0xC2,
    0x60, 0x05,0x00,
    0x60, 0x06,0x00,
    0x60, 0x07,0x00,
    0x60, 0x08,0x00,
    0x60, 0x09,0x00,
    0x60, 0x0A,0x00,
    0x60, 0x0B,0x00,
    0x60, 0x0C,0x00,
    0x60, 0x0D,0x20,
    0x60, 0x0E,0x00,
    0x60, 0x0F,0xA1,
    0x60, 0x10,0x01,
    0x60, 0x11,0x00,
    0x60, 0x12,0x06,
    0x60, 0x13,0x00,
    0x60, 0x14,0x0B,
    0x60, 0x15,0x00,
    0x60, 0x16,0x14,
    0x60, 0x17,0x00,
    0x60, 0x18,0x25,
    0x60, 0x19,0x00,
    0x60, 0x1A,0x43,
    0x60, 0x1B,0x00,
    0x60, 0x1C,0x82,
    0x00, 0x00,0x00,
    0x01, 0x04,0x01,
    0x01, 0x04,0x00,
    0x01, 0x00,0x03,   
#endif
	STOP_REG, STOP_REG, STOP_REG,
};

struct hm2131Dual_mode {
    IMG_UINT8 ui8Flipping;
    const IMG_UINT8 *modeRegisters;
    IMG_UINT32 nRegisters; // initially 0 then computed the 1st time
};

struct hm2131Dual_mode hm2131Dual_modes[] =
{
	{ SENSOR_FLIP_BOTH, ui8ModeRegs_2160_1080_60_4lane, 0 }, //0
};

static const IMG_UINT8* HM2131Dual_GetRegisters(HM2131DualCAM_STRUCT *psCam,
    unsigned int uiMode, unsigned int *nRegisters)
{
	uiMode = 0;
    if (uiMode < sizeof(hm2131Dual_modes) / sizeof(struct hm2131Dual_mode))
    {
        const IMG_UINT8 *registerArray = hm2131Dual_modes[uiMode].modeRegisters;
        if (0 == hm2131Dual_modes[uiMode].nRegisters)
        {
            // should be done only once per mode
            int i = 0;
            while (STOP_REG != registerArray[3 * i])
            {
                i++;
            }
            hm2131Dual_modes[uiMode].nRegisters = i;
        }
        *nRegisters = hm2131Dual_modes[uiMode].nRegisters;
        
        return registerArray;
    }
    // if it is an invalid mode returns NULL

    return NULL;
}

static IMG_RESULT hm2131Dual_i2c1_write8(int i2c, const IMG_UINT8 *data,
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
/*
        if(DELAY_REG == data[0])
        {
            usleep((int)data[1]*1000);
            continue;
        }
*/
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

typedef enum __I2cChannelType_
{
	I2C_SENSOR_CHL,		
	I2C_HM2131_CHL,
	I2C_HM2131_1_CHL
}I2cChannelType;

static IMG_UINT8 sensor_online[9] = {
	0xff, 0xfd, 0x80,
	0xff, 0xfe, 0x50,
	0x00, 0x4d, 0x03,
};

static IMG_UINT8 sensor_offline[9] = {
	0xff, 0xfd, 0x80,
	0xff, 0xfe, 0x50,
	0x00, 0x4d, 0x00,
};

static IMG_RESULT hm2131Dual_i2c_switch(int i2c, I2cChannelType i2c_chl)
{
	if (i2c_chl == I2C_SENSOR_CHL)
	{// switch to sensor i2c channel
		return hm2131Dual_i2c1_write8(i2c, sensor_online, sizeof(sensor_online), i2c_addr);
	}
	else if (i2c_chl == I2C_HM2131_CHL)
	{// switch to hm2131 i2c channel
		return hm2131Dual_i2c1_write8(i2c, sensor_offline, sizeof(sensor_offline), i2c_addr);
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

static IMG_RESULT hm2131Dual_i2c_write8(int i2c, const IMG_UINT8 *data, unsigned int len)
{
#if !file_i2c
    unsigned int i = 0, j = 0;
    IMG_UINT8 ret_data = 0;
    IMG_UINT8 buff[2];
    const IMG_UINT8 *ori_data_ptr =data ;

    static int running_flag = 0;
    int write_len = 0;

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
		printf("waiting hm2131Dual_i2c_write8\n");
		//usleep(5*1000);
	}
	running_flag = 1;
	
	
	//for (j = 0; j < SENSOR_SLAVE_NUM; j++)
	{
		data = ori_data_ptr;
	    /* Set I2C slave address */
	    for (i = 0; i < len; data += 3, i += 3)
	    {
/*
	        if(DELAY_REG == data[0])
	        {
	            usleep((int)data[1]*1000);
	            continue;
	        }
*/			
    		for (j = 0; j < SENSOR_SLAVE_NUM; j++)
    		{
    		    if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, hm2131_i2c_slave_addr[j]))
    	        {
    	            LOG_ERROR("Failed to write I2C slave address %x!\n", hm2131_i2c_slave_addr[j]);
                    running_flag = 0;	
    	            return IMG_ERROR_BUSY;
    	        }
    		   
    	        write_len = write(i2c, data, 3);

    	        if (write_len != 3)
    	        {
    	            LOG_ERROR("Failed to write I2C(%x) data! write_len = %d, index = %d\n", hm2131_i2c_slave_addr[j], write_len, i);
               	    running_flag = 0;	
    	            return IMG_ERROR_BUSY;
    	        }
            }
	    }
	}
	
#endif //file_i2c
    running_flag = 0;
    return IMG_SUCCESS;
}

static IMG_RESULT hm2131Dual_i2c_read8(int i2c, char i2c_addr, IMG_UINT16 offset, IMG_UINT8 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
	
    /* Set I2C slave address */
	if (ioctl(i2c, /*I2C_SLAVE*/I2C_SLAVE_FORCE, i2c_addr))
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

static IMG_RESULT HM2131Dual_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
	IMG_RESULT ret;
	IMG_UINT16 v = 0;
	HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_ASSERT(strlen(HM2131Dual_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(HM2131Dual_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);

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
    sprintf(psInfo->pszSensorName, HM2131Dual_SENSOR_INFO_NAME);
    sprintf(psInfo->pszSensorVersion, HM2131Dual_SENSOR_VERSION);

    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 6040;
    // bitdepth is a mode information
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = psCam->imager;
    psInfo->bBackFacing = IMG_TRUE;
    // other information should be filled by Sensor_GetInfo()
#ifdef INFOTM_ISP
    psInfo->ui32ModeCount = sizeof(hm2131Dual_modes) / sizeof(struct hm2131Dual_mode);
#endif //INFOTM_ISP

    LOG_DEBUG("provided BayerOriginal=%d\n", psInfo->eBayerOriginal);
    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    /*if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    ret = HM2131Dual_GetModeInfo(psCam, nIndex, psModes);
    return ret;
}

static IMG_RESULT HM2131Dual_GetState(SENSOR_HANDLE hHandle,
        SENSOR_STATUS *psStatus)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

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

static IMG_RESULT HM2131Dual_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
          IMG_UINT8 ui8Flipping)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = NULL;
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    modeReg = HM2131Dual_GetRegisters(psCam, nMode, &nRegisters);

    if (modeReg)
    {
/*    
        if((ui8Flipping&(hm2131Dual_modes[nMode].ui8Flipping))!=ui8Flipping)
        {
            LOG_ERROR("sensor mode does not support selected flipping "\
                "0x%x (supports 0x%x)\n",
                ui8Flipping, hm2131Dual_modes[nMode].ui8Flipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }
*/
        psCam->nRegisters = nRegisters;
        
        /* no need to copy the register as we will apply them one by one */
        psCam->currModeReg = (IMG_UINT8*)modeReg;

        ret = HM2131Dual_GetModeInfo(psCam, nMode, &(psCam->currMode));
        if (ret)
        {
            LOG_ERROR("failed to get mode %d initial information!\n", nMode);
            return IMG_ERROR_FATAL;
        }

#ifdef DO_SETUP_IN_ENABLE
        /* because we don't do the actual setup we do not have real  information about the sensor - fake it */
        //psCam->ui32Exposure = psCam->currMode.ui32ExposureMin * 302;
        psCam->ui32Exposure = IMG_MIN_INT(psCam->currMode.ui32ExposureMax, IMG_MAX_INT(psCam->ui32Exposure, psCam->currMode.ui32ExposureMin));
#else
        HM2131Dual_ApplyMode(psCam);
#endif //DO_SETUP_IN_ENABLE

        psCam->ui16CurrentFocus = hm2131Dual_focus_dac_min; // minimum focus
        psCam->ui16CurrentMode = nMode;
        psCam->ui8CurrentFlipping = ui8Flipping;
        return IMG_SUCCESS;
    }


    return IMG_ERROR_INVALID_PARAMETERS;
}

static IMG_RESULT HM2131Dual_Enable(SENSOR_HANDLE hHandle)
{
	IMG_UINT16 chipV = 0;
	IMG_UINT8 aui8Regs[3];
	IMG_UINT32 i = 0;

    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling HM2131Dual Camera!\n");
    psCam->bEnabled=IMG_TRUE;

    psCam->psSensorPhy->psGasket->bParallel = IMG_FALSE;
    psCam->psSensorPhy->psGasket->uiGasket = psCam->imager;

    //printf("~~~psCam->currMode.ui8MipiLanes=%d\n", psCam->currMode.ui8MipiLanes);

    SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->currMode.ui8MipiLanes, HM2131Dual_PHY_NUM);

#ifdef DO_SETUP_IN_ENABLE
    // because we didn't do the actual setup in setMode we have to do it now
    // it may start the sensor so we have to do it after the gasket enable
    /// @ fix conversion from float to uint
    HM2131Dual_ApplyMode(psCam);
#endif //DO_SETUP_IN_ENABLE

    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_Disable(SENSOR_HANDLE hHandle)
{
    IMG_UINT8 aui8Regs[3];
    static const IMG_UINT8 disableRegs[]=
	{
		0x01, 0x00, 0x00
    };

    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(psCam->bEnabled)
    {
        int delay = 0;
        LOG_INFO("Disabling HM2131Dual camera\n");
        psCam->bEnabled = IMG_FALSE;

        hm2131Dual_i2c_write8(psCam->i2c, disableRegs, sizeof(disableRegs));

        // delay of a frame period to ensure sensor has stopped
        // flFrameRate in Hz, change to MHz to have micro seconds
        delay = (int)floor((1.0 / psCam->currMode.flFrameRate) * 1000 * 1000);
        LOG_DEBUG("delay of %uus between disabling sensor/phy\n", delay);
        
        usleep(delay);

try_again:
        hm2131Dual_i2c_switch(psCam->i2c, I2C_HM2131_CHL);
        aui8Regs[0] = 0x6f;
        aui8Regs[1] = 0x01;
        aui8Regs[2] = 0x00;
        while (hm2131Dual_i2c1_write8(psCam->i2c, aui8Regs, 3, I2C_HM2131_ADDR) != IMG_SUCCESS)
        {
            usleep(10*1000);
            goto try_again;
        }
		
        SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, HM2131Dual_PHY_NUM);
    }

    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_Destroy(SENSOR_HANDLE hHandle)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_DEBUG("Destroying HM2131Dual camera\n");
	
    /* remember to free anything that might have been allocated dynamically * (like extended params...)*/
    if(psCam->bEnabled)
    {
        HM2131Dual_Disable(hHandle);
    }
	
    SensorPhyDeinit(psCam->psSensorPhy);
	
    if (psCam->currModeReg)
    {
        psCam->currModeReg = NULL;
    }
	
#if !file_i2c
    close(psCam->i2c);
#endif //file_i2c

    IMG_FREE(psCam);

    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_GetFocusRange(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Min, IMG_UINT16 *pui16Max)
{
    *pui16Min = hm2131Dual_focus_dac_min;
    *pui16Max = hm2131Dual_focus_dac_max;
    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_GetCurrentFocus(SENSOR_HANDLE hHandle,
        IMG_UINT16 *pui16Current)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui16Current=psCam->ui16CurrentFocus;
    return IMG_SUCCESS;
}

// declared beforehand
IMG_RESULT HM2131Dual_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus)
{
    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_GetExposureRange(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui32Min=psCam->currMode.ui32ExposureMin;
    *pui32Max=psCam->currMode.ui32ExposureMax;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current=psCam->ui32Exposure;
    return IMG_SUCCESS;
}

void HM2131Dual_ComputeExposure(IMG_UINT32 ui32Exposure, IMG_UINT32 ui32ExposureMin, IMG_UINT8 *exposureRegs)
{
}

static IMG_RESULT HM2131Dual_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
    IMG_UINT32 ui32Lines;
    
    IMG_UINT8 exposureRegs[] =
    {
        0x02, 0x02, 0x01, // exposure register
        0x02, 0x03, 0x08, // 
        0x01, 0x04, 0x01,
        0x01, 0x04, 0x00,
    };

    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    printf("===>HM2131Dual_SetExposure\n");

    psCam->ui32Exposure = ui32Current;

	ui32Lines = (IMG_UINT32)psCam->ui32Exposure / psCam->currMode.ui32ExposureMin;
/*
	if (ui32Lines > psCam->currMode.ui16VerticalTotal - 4)
       {
           ui32Lines = psCam->currMode.ui16VerticalTotal - 4;
       }
*/

    exposureRegs[2] = (ui32Lines >> 8);
    exposureRegs[5] = (ui32Lines) & 0xff;

    if ((g_HM2131Dual_ui8LinesValHiBackup != exposureRegs[2]) || (g_HM2131Dual_ui8LinesValLoBackup != exposureRegs[5]))
    {
        g_HM2131Dual_ui8LinesValHiBackup = exposureRegs[2];
        g_HM2131Dual_ui8LinesValLoBackup = exposureRegs[5];
    }
    else
    {
        // don't need to write the same exposure val again
        return IMG_SUCCESS; 
    }
    
	LOG_DEBUG("Exposure Val %x\n",(psCam->ui32Exposure/psCam->currMode.ui32ExposureMin));

#ifndef DISABLE_INFOTM_AE
    hm2131Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL);
    hm2131Dual_i2c1_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs), I2C_HM2131_ADDR_DUAL);
    hm2131Dual_i2c_switch(psCam->i2c, I2C_HM2131_CHL);
#endif

//	hm2131Dual_i2c_write8(psCam->i2c, exposureRegs, sizeof(exposureRegs));

    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
    IMG_UINT16 aui16Regs[9];
    
    unsigned int nIndex = 0;
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    static int runing_flag = 0;
    
    //printf("-->HM2131Dual_SetGain %f\n", flGain);
    
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (runing_flag == 1)
	{
        return IMG_SUCCESS;
	}

    psCam->flGain = flGain;

    while(nIndex < (sizeof(asHM2131GainValues) / sizeof(ARGAINTABLE))-1)
    {
        if(asHM2131GainValues[nIndex].flGain > flGain)
        {
            break;
        }
        nIndex++;
    }

    if(nIndex > 0)
    {
        nIndex = nIndex - 1;
    }
    
    if (g_HM2131Dual_ui8GainValBackup == asHM2131GainValues[nIndex].ui8GainRegVal)
    {
        runing_flag = 0;
        return IMG_SUCCESS;
    }
    
    g_HM2131Dual_ui8GainValBackup = asHM2131GainValues[nIndex].ui8GainRegVal; 

    aui16Regs[0] = 0x02;// Global Gain register
    aui16Regs[1] = 0x05;
    aui16Regs[2] = (IMG_UINT16)asHM2131GainValues[nIndex].ui8GainRegVal;

    aui16Regs[3] = 0x01;
    aui16Regs[4] = 0x04;
    aui16Regs[5] = 0x01;

    aui16Regs[6] = 0x01;
    aui16Regs[7] = 0x04;
    aui16Regs[8] = 0x00;
    
#ifndef DISABLE_INFOTM_AE
    hm2131Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL);
    hm2131Dual_i2c1_write8(psCam->i2c, aui16Regs, sizeof(aui16Regs), I2C_HM2131_ADDR_DUAL);
    hm2131Dual_i2c_switch(psCam->i2c, I2C_HM2131_CHL);
#endif

#if defined(ENABLE_AE_DEBUG_MSG)
    printf("asHM2131GainValues Change Global Gain 0x0205 = 0x%04X\n", aui16Regs[1]);
#endif

    runing_flag = 0;

    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set global gain register\n");
        return ret;
    }

    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

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

static IMG_RESULT HM2131Dual_GetGain(SENSOR_HANDLE hHandle, double *pflGain,
    IMG_UINT8 ui8Context)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain=psCam->flGain;
    return IMG_SUCCESS;
}

IMG_RESULT HM2131Dual_Create(SENSOR_HANDLE *phHandle, int index)
{
#if !file_i2c
        HM2131DualCAM_STRUCT *psCam;
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
    //  memset((void *)extra_cfg, 0, sizeof(dev_nm));
    //  sprintf(extra_cfg, "%s%s%d-config.txt", EXTRA_CFG_PATH, "sensor" , index );
        //printf("extra_cfg = %s \n", extra_cfg);
#endif
#endif

    psCam=(HM2131DualCAM_STRUCT *)IMG_CALLOC(1, sizeof(HM2131DualCAM_STRUCT));
    if(!psCam)
	{
        return IMG_ERROR_MALLOC_FAILED;
    }

    *phHandle=&(psCam->sFuncs);
    psCam->sFuncs.GetMode = HM2131Dual_GetMode;
    psCam->sFuncs.GetState = HM2131Dual_GetState;
    psCam->sFuncs.SetMode = HM2131Dual_SetMode;
    psCam->sFuncs.Enable = HM2131Dual_Enable;
    psCam->sFuncs.Disable = HM2131Dual_Disable;
    psCam->sFuncs.Destroy = HM2131Dual_Destroy;
    psCam->sFuncs.GetInfo = HM2131Dual_GetInfo;

    psCam->sFuncs.GetCurrentGain = HM2131Dual_GetGain;
    psCam->sFuncs.GetGainRange = HM2131Dual_GetGainRange;
    psCam->sFuncs.SetGain = HM2131Dual_SetGain;
    psCam->sFuncs.SetExposure = HM2131Dual_SetExposure;
    psCam->sFuncs.GetExposureRange = HM2131Dual_GetExposureRange;
    psCam->sFuncs.GetExposure = HM2131Dual_GetExposure;

    psCam->sFuncs.GetCurrentFocus = HM2131Dual_GetCurrentFocus;
    psCam->sFuncs.GetFocusRange = HM2131Dual_GetFocusRange;
    psCam->sFuncs.SetFocus = HM2131Dual_SetFocus;
	
#ifdef INFOTM_ISP
    psCam->sFuncs.SetFlipMirror = HM2131Dual_SetFlipMirror;
	psCam->sFuncs.SetGainAndExposure = HM2131Dual_SetGainAndExposure;
	psCam->sFuncs.SetFPS = HM2131Dual_SetFPS;
	psCam->sFuncs.GetFixedFPS =  HM2131Dual_GetFixedFPS;

	psCam->sFuncs.ReadSensorCalibrationData = HM2131Dual_read_sensor_calibration_data;
	psCam->sFuncs.ReadSensorCalibrationVersion = HM2131Dual_read_sensor_calibration_version;
	psCam->sFuncs.UpdateSensorWBGain = HM2131Dual_update_sensor_wb_gain;

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
#endif //file_i2c

    // need i2c to have been found to read mode
    HM2131Dual_GetModeInfo(psCam, psCam->ui16CurrentMode, &(psCam->currMode));
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

void HM2131Dual_ApplyMode(HM2131DualCAM_STRUCT *psCam)
{
    if (!psCam->currModeReg)
    {
        LOG_ERROR("current register modes not available!\n");
        return;
    }

    printf("HM2131Dual_ApplyMode()\n");

    hm2131Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL);
    hm2131Dual_i2c1_write8(psCam->i2c, psCam->currModeReg, psCam->nRegisters * 3, I2C_HM2131_ADDR_DUAL);
    hm2131Dual_i2c_switch(psCam->i2c, I2C_HM2131_CHL);

    psCam->ui32Exposure = psCam->currMode.ui32ExposureMin*0x0108;//psCam->currMode.ui32ExposureMin * 302;
    psCam->ui32Exposure = IMG_MIN_INT(psCam->currMode.ui32ExposureMax, psCam->ui32Exposure);

    LOG_DEBUG("Sensor ready to go\n");
    // setup the exposure setting information
}

IMG_RESULT HM2131Dual_GetModeInfo(HM2131DualCAM_STRUCT *psCam, IMG_UINT16 
ui16Mode, SENSOR_MODE *info)
{
    unsigned int n;
    IMG_RESULT ret = IMG_SUCCESS;

    unsigned int nRegisters = 0;
    const IMG_UINT8 *modeReg = HM2131Dual_GetRegisters(psCam, ui16Mode,
        &nRegisters);

    double pix_clk = 42.5*1000*1000;

    unsigned int hts[2] = { 0, 0 };
    unsigned int hts_v = 0;
    unsigned int vts[2] = { 0, 0 };
    unsigned int vts_v = 0;
    unsigned int frame_t = 0;

    double trow = 0; // in micro seconds

	IMG_UINT8 reg_val = 0;

    if (!modeReg)
    {
        LOG_ERROR("invalid mode %d\n", modeReg);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    /*
 	0x03, 0x40, 0x04, // frame_length_lines H
	0x03, 0x41, 0x52, // frame_length_lines L
	
	0x03, 0x42, 0x05, // line_length_pck H
	0x03, 0x43, 0x00, // line_length_pck L
        */
	vts[0] = 0x04; // reg[REG_TIMING_VTS_0]
	vts[1] = 0x52;// reg[REG_TIMING_VTS_1]
	hts[0] = 0x05;// reg[REG_TIMING_HTS_0]
	hts[1] = 0x00; // reg[REG_TIMING_HTS_1]


    hts_v = (hts[1] | (hts[0] << 8)); // // (reg[REG_TIMING_VTS_0]<<8)|reg[REG_TIMING_VTS_1]
    vts_v = (vts[1] | (vts[0] << 8)); // (reg[REG_TIMING_HTS_0]<<8)|reg[REG_TIMING_HTS_1]
    trow = (hts_v * 1000 * 1000) / pix_clk; 
    frame_t = vts_v * hts_v;

    info->ui16Width = 2160;//(reg[REG_H_OUTPUT_SIZE_0]<<8)|reg[REG_H_OUTPUT_SIZE_1]
    info->ui16Height = 1080;//(reg[REG_V_OUTPUT_SIZE_0]<<8)|reg[REG_V_OUTPUT_SIZE_1]
    
    info->ui16VerticalTotal = vts_v;

    info->flFrameRate = 30;//sclk / frame_t;

    // Integration time (seconds) = coarse_integration x line_length_pck / vt_pix_clk (MHz) x 1 x 10^6
    info->ui8SupportFlipping = SENSOR_FLIP_BOTH;
    info->ui32ExposureMax = (IMG_UINT32)floor(trow *  info->ui16VerticalTotal); //(IMG_UINT32)(100.0 / info->flFrameRate) * 10000.0;
    info->ui32ExposureMin = (IMG_UINT32)floor(trow);

    printf("==>trow=%lf, info->ui32ExposureMin=%d, info->ui32ExposureMax=%d\n", trow, info->ui32ExposureMin, info->ui32ExposureMax);

#ifdef INFOTM_ISP
	info->dbExposureMin = trow;

    /*
        default coarsetime
        0x02, 0x02, 0x04, // exposure H
	0x02, 0x03, 0x50, // exposure L
        */
	psCam->ui32Exposure = trow * 0x0108; //set initial exposure.
#endif //INFOTM_ISP


    info->ui8MipiLanes = 4; //2* 0x01, 0x11, 0x01, // CSI_LANE_MODE MIPI Lane Selection 0  1: Lanes, 1:  2 Lanes
	printf("info->ui8MipiLanes = %d\n", info->ui8MipiLanes);

    info->ui8BitDepth = 10; //0x01, 0x12, 0x0A, // CSI_DATA_FORMAT_H [15:8]  Data format selection RAW8 : 0x0808  RAW10 : 0x0A0A
#if defined(INFOTM_ISP)
    psCam->fCurrentFps = info->flFrameRate;
    printf("the flFrameRate = %f\n", info->flFrameRate);
    psCam->fixedFps = psCam->fCurrentFps;
    psCam->initClk = 0;
#endif
    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
static IMG_RESULT HM2131Dual_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{
    HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
    IMG_UINT16 ui16Regs;
    IMG_UINT8 aui18FlipMirrorRegs[]=
    {
        0x38, 0x21, 0x00,
        0x38, 0x20, 0x00
    };
    IMG_RESULT ret;

    return IMG_SUCCESS;
}



static IMG_RESULT HM2131Dual_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
	static int runing_flag = 0;
    
    //static IMG_UINT32 bk_ui32Exposure[3] = {0xFFFF, 0xFFFF, 0xFFFF};
    //static double bk_flGain[3] = {0.0, 0.0, 0.0};
    
    static IMG_UINT32 delay_loops =0;
	IMG_UINT32 ui32Lines;
	IMG_UINT32 nIndex = 0, L = 6;
	IMG_UINT8 write_type = 0;

	IMG_UINT8 ui8Regs[] =
    {
        0x02, 0x05, 0x00, // gain register
		0x02, 0x02, 0x00, // Exposure register
		0x02, 0x03, 0x00, // 
        0x01, 0x04, 0x01,
        0x01, 0x04, 0x00,
	};

	HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

	if (!psCam->psSensorPhy)
	{
		LOG_ERROR("sensor not initialised\n");
		runing_flag = 0;
		return IMG_ERROR_NOT_INITIALISED;
	}

	if (runing_flag == 1)
	{
		return IMG_SUCCESS;
	}

	runing_flag = 1;

    if (delay_loops%3 != 0)
    {
        runing_flag = 0;
        delay_loops++;

        psCam->ui32Exposure = ui32Exposure;
        psCam->flGain = flGain;
        return IMG_SUCCESS;
    }
    
    if (delay_loops == 0)
    {
        delay_loops++;
    }
    else
    {
        delay_loops = 1;
    }
    psCam->ui32Exposure = ui32Exposure;

	ui32Lines = (IMG_UINT32)psCam->ui32Exposure / psCam->currMode.ui32ExposureMin;

	//if (ui32Lines > psCam->currMode.ui16VerticalTotal - 4)
	//	ui32Lines = psCam->currMode.ui16VerticalTotal - 4;
	
	ui8Regs[5] = (ui32Lines >> 8);
	ui8Regs[8] = (ui32Lines) & 0xff;

	//printf("Exposure Val %x[%x, %x]\n",ui32Lines, ui8Regs[5], ui8Regs[8]);

	//02.Gain part
	//
#if 0
    psCam->flGain = bk_flGain[2];
#else
    psCam->flGain = flGain;
#endif
	while(nIndex < (sizeof(asHM2131GainValues) / sizeof(ARGAINTABLE)) - 1)
    {
        if(asHM2131GainValues[nIndex].flGain > flGain)
        {
            break;
        }
        nIndex++;
    }

    if(nIndex > 0)
    {
        nIndex = nIndex - 1;
    }
    
    ui8Regs[2] = asHM2131GainValues[nIndex].ui8GainRegVal;

	//printf("gain n=%u from %lf, %d\n", nIndex, flGain, asHM2131GainValues[nIndex].ui8GainRegVal);

    if (0) // ??? just for reduce write freq
	{
		static double pre_gain = 0.0;

		if (/*psCam->flGain >= 8.0 && */(abs(psCam->flGain - pre_gain) <= 0.25))
		{
			runing_flag = 0;
            printf("===>new gain %lf old gain %lf\n", psCam->flGain, pre_gain);
			return IMG_SUCCESS;
		}
		pre_gain = psCam->flGain;
	}

	if ((g_HM2131Dual_ui8LinesValHiBackup != ui8Regs[5]) || (g_HM2131Dual_ui8LinesValLoBackup != ui8Regs[8]))
	{
		write_type |= 0x2;

		g_HM2131Dual_ui8LinesValHiBackup = ui8Regs[5];
		g_HM2131Dual_ui8LinesValLoBackup = ui8Regs[8];
	}

	if ((g_HM2131Dual_ui8GainValBackup != ui8Regs[2]))
	{
		write_type |= 0x1;
		g_HM2131Dual_ui8GainValBackup = ui8Regs[2];
	}
#ifndef DISABLE_INFOTM_AE
    hm2131Dual_i2c_switch(psCam->i2c, I2C_SENSOR_CHL);
    hm2131Dual_i2c1_write8(psCam->i2c, ui8Regs, sizeof(ui8Regs), I2C_HM2131_ADDR_DUAL);
    hm2131Dual_i2c_switch(psCam->i2c, I2C_HM2131_CHL);
#endif
//    hm2131Dual_i2c_write8(psCam->i2c, ui8Regs, sizeof(ui8Regs));
/*
	switch (write_type)
	{
	case 0x3:
		hm2131Dual_i2c_write8(psCam->i2c, ui8Regs, sizeof(ui8Regs));
		break;
	case 0x2:
		hm2131Dual_i2c_write8(psCam->i2c, ui8Regs + 6, sizeof(ui8Regs) - 6);
		break;
	case 0x1:
		hm2131Dual_i2c_write8(psCam->i2c, ui8Regs, 3);
		break;
	default:
		break;
	}
*/

/*
{
    IMG_UINT8 data[3] = {0};
    hm2131Dual_i2c_read8(psCam->i2c, I2C_HM2131_ADDR_DUAL, 0x0205, &data[0]);
    hm2131Dual_i2c_read8(psCam->i2c, I2C_HM2131_ADDR_DUAL, 0x0202, &data[1]);
    hm2131Dual_i2c_read8(psCam->i2c, I2C_HM2131_ADDR_DUAL, 0x0203, &data[2]);
    printf("===>read i2c(%x) gain %x, exposure %x\n",i2c_addr, data[0], (data[1]<<8)|data[2]);
}  
*/
    runing_flag = 0;
    return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
	if (pfixed != NULL)
	{
		HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);

		*pfixed = (int)psCam->fixedFps;
	}

	return IMG_SUCCESS;
}

static IMG_RESULT HM2131Dual_SetFPS(SENSOR_HANDLE hHandle, double fps)
{
	HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
	IMG_UINT8 aui8Regs[3];
	//printf("current down_ratio =%f\n", down_ratio);

	psCam->fCurrentFps = 30;

	return IMG_SUCCESS;
}

/////////////////////////////////////////////////////////////

static int hm2131Dual_get_valid_otp_group(int i2c, char i2c_addr, IMG_UINT16 group_flag_addr)
{
	int group_index = -1;
	return group_index;	
}

static IMG_UINT8* HM2131Dual_read_sensor_calibration_data(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain)
{
	HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
	return NULL;
}

static IMG_UINT8* HM2131Dual_read_sensor_calibration_version(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/)
{
	HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
	return NULL;
}

#define WB_CALIBRATION_METHOD_FROM_LIPIN

static IMG_RESULT HM2131Dual_update_sensor_wb_gain(SENSOR_HANDLE hHandle, int infotm_method, IMG_FLOAT awb_convert_gain, IMG_UINT16 version)
{
	HM2131DualCAM_STRUCT *psCam = container_of(hHandle, HM2131DualCAM_STRUCT, sFuncs);
	return IMG_SUCCESS;
}

/////////////////////////////////////////////////////////////

#endif //INFOTM_ISP

