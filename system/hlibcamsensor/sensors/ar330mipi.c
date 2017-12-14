/**
 ******************************************************************************
 @file ar330.c

 @brief AR330 camera implementation

 ******************************************************************************/
#include "sensors/ar330mipi.h"
#include "sensors/ddk_sensor_driver.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define MICROS 1000000

#define LOG_TAG "AR330MIPI_SENSOR"

#define AR330MIPI_WRITE_ADDR		(0x20 >> 1 )
#define AR330MIPI_READ_ADDR			(0x21 >> 1 )
#define AR330_SENSOR_VERSION 	"not-verified" // until read from sensor

#ifdef WIN32 // on windows we do not need to sleep to wait for the bus
static void usleep(int t)
{
    (void)t;
}
#endif

#ifdef INFOTM_ISP
#define DEV_PATH    ("/dev/ddk_sensor")
#define EXTRA_CFG_PATH  ("/root/.ispddk/")
static IMG_UINT32 i2c_addr = 0;
static IMG_UINT32 imgr = 0;
static char extra_cfg[64];
#endif

static IMG_RESULT AR330MIPI_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
static IMG_RESULT AR330MIPI_ChangeRES(SENSOR_HANDLE hHandle, int imgW, int imgH);
static IMG_RESULT AR330MIPI_GetSnapRes(SENSOR_HANDLE hHandle, reslist_t *presList);
typedef struct _ar330cam_struct
{
    SENSOR_FUNCS sFuncs;
    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
#if defined(INFOTM_ISP)
    double fCurrentFps;
    IMG_UINT8 fixedFps;
    IMG_UINT8 initClk;//current mode initialize pixel clk;
#endif
    IMG_UINT8 ui8CurrentFlipping;
    IMG_BOOL bEnabled;
    IMG_UINT32 ui32ExposureMax;
    IMG_UINT32 ui32Exposure;
    IMG_UINT32 ui32ExposureMin;
	IMG_DOUBLE dbExposureMin;
    IMG_UINT8 ui8MipiLanes;
    double flGain;
    int i2c;
    SENSOR_PHY *psSensorPhy;
}AR330CAM_STRUCT;

#define NUM_MODES 5
#define REGISTERS_PER_MODE 40

static SENSOR_MODE asModes[]=
{
    {10, 1920, 1088, 30.0, 1248, 2208, 1156, SENSOR_FLIP_BOTH}, // 1 lane 30fps
    //{10, 1280, 720,  60.0, 730 , SENSOR_FLIP_BOTH},
    {10, 1920, 1088, 60.0, 1248, SENSOR_FLIP_BOTH},
    {10, 1920, 1088, 30.0, 1248, SENSOR_FLIP_BOTH},
    {10, 1920, 1088, 20.0, 1308, SENSOR_FLIP_BOTH},
    {10, 1920, 1088, 15.0, 1248, SENSOR_FLIP_BOTH}, // 1 lane 15fps
    {10, 1920, 1088, 30.0, 1248, SENSOR_FLIP_BOTH}, // 1 lane 30fps
    {10, 1920, 1088, 30.0, 1248, SENSOR_FLIP_BOTH}, // 2 lane 30fps
    {10, 1296, 1296, 30.0, 1248, SENSOR_FLIP_BOTH}, // 2 lane 30fps
    {10, 1536, 1536, 30.0, 1248, SENSOR_FLIP_BOTH}, // 2 lane 30fps

};

static reslist_t SnapRes =
{
		{{1920, 1088}, {1280, 720}, {1536, 1536}, {1296, 1296}}, 4
};

static IMG_UINT16 ui16ModeRegs_0[] = {
#if 0
    // 1280x720x??
    0x301A, 0x0058,     // RESET_REGISTER
    0x3064, 0x1802,     // SMIA TEST
    0x31AE, 0x0202,     // SERIAL_FORMAT
    0x31AC, 0x0A0A,     // DATA_FORMAT_BITS
    0x31B0, 0x0028,     // FRAME_PREAMBLE
    0x31B2, 0x000E,     // LINE_PREAMBLE
    0x31B4, 0x2743,     // MIPI_TIMING_0
    0x31B6, 0x114E,     // MIPI_TIMING_1
    0x31B8, 0x2049,     // MIPI_TIMING_2
    0x31BA, 0x0186,     // MIPI_TIMING_3
    0x31BC, 0x8005,     // MIPI_TIMING_4
    0x31BE, 0x2003,     // MIPI_CONFIG_STATUS
    0x302A, 0x0005,     // VT_PIX_CLK_DIV
    0x302C, 0x0002,     // VT_SYS_CLK_DIV
    0x302E, 0x0004,     // PRE_PLL_CLK_DIV
    0x3030, 0x00B5,     // PLL_MULTIPLIER
    0x3036, 0x000A,     // OP_PIX_CLK_DIV
    0x3038, 0x0001,     // OP_SYS_CLK_DIV
    0x31AC, 0x0A0A,     // DATA_FORMAT_BITS
    0x3004, 0x0186,     // X_ADDR_START
    0x3008, 0x0905,     // X_ADDR_END
    0x3002, 0x019C,     // Y_ADDR_START
    0x3006, 0x05D3,     // Y_ADDR_END
    0x30A2, 0x0001,     // X_ODD_INC
    0x30A6, 0x0001,     // Y_ODD_INC
    0x3040, 0x0000,     // READ_MODE
    0x3040, 0x0000,     // READ_MODE
    0x3040, 0x0000,     // READ_MODE
    0x300C, 0x0620,     // LINE_LENGTH_PCK  //Blanking
    0x300A, 0x04E0,     // FRAME_LENGTH_LINES
    0x3014, 0x0000,     // FINE_INTEGRATION_TIME
    0x3012, 0x04E0,     // COARSE_INTEGRATION_TIME
    0x3042, 0x0000,     // EXTRA_DELAY
    0x30BA, 0x002C,     // DIGITAL_CTRL
    0x3088, 0x80BA,     // SEQ_CTRL_PORT
    0x3086, 0x0253,     // SEQ_DATA_PORT
#else
    //[AR330-1088p 30fps 1lane Fvco = ? MHz] for qiwo
    0x301A, 0x0058,     //RESET_REGISTER = 88
    0x302A, 0x0005,     //VT_PIX_CLK_DIV = 5
    0x302C, 0x0004,  // VT_SYS_CLK_DIV -------------------[2 - enable 2lanes], [4 - enable 1lane]
    0x302E, 0x0001,     //PRE_PLL_CLK_DIV = 1
    0x3030, 0x0020,     //PLL_MULTIPLIER = 32
    0x3036, 0x000A,     //OP_PIX_CLK_DIV = 10
    0x3038, 0x0001,     //OP_SYS_CLK_DIV = 1
    0x31AC, 0x0A0A,     //DATA_FORMAT_BITS = 2570
    0x31AE, 0x0201/*0x0202*/,     //SERIAL_FORMAT = 514 ------ [0x201 1 lane] [0x202 2 lanes] [0x204 4 lanes]
    0x31B0, 0x003E,     //FRAME_PREAMBLE = 62
    0x31B2, 0x0018,     //LINE_PREAMBLE = 24
    0x31B4, 0x4F66,     //MIPI_TIMING_0 = 20326
    0x31B6, 0x4215,     //MIPI_TIMING_1 = 16917
    0x31B8, 0x308B,     //MIPI_TIMING_2 = 12427
    0x31BA, 0x028A,     //MIPI_TIMING_3 = 650
    0x31BC, 0x8008,     //MIPI_TIMING_4 = 32776
    0x3002, 0x00EA,     //Y_ADDR_START = 234
    0x3004, 0x00C6,     //X_ADDR_START = 198
    0x3006, 0x0529,     //Y_ADDR_END = 1313 //21->29 fengwu
    0x3008, 0x0845,     //X_ADDR_END = 2117
    0x300A, 0x0450,     //0x4bcFRAME_LENGTH_LINES = 1212
    0x300C, 0x0484,     //0x840LINE_LENGTH_PCK = 2112//484
    0x3012, 0x02D7,     //COARSE_INTEGRATION_TIME = 727
    0x3014, 0x0000,     //FINE_INTEGRATION_TIME = 0
    0x30A2, 0x0001,     //X_ODD_INC = 1
    0x30A6, 0x0001,     //Y_ODD_INC = 1
    0x3040, 0x0000,     //READ_MODE = 0
    0x3042, 0x0080,     //EXTRA_DELAY = 128
    0x30BA, 0x002C,     //DIGITAL_CTRL = 44
    0x3088, 0x80BA,     // SEQ_CTRL_PORT
    0x3086, 0x0253,     // SEQ_DATA_PORT
    //
    0x31E0, 0x0303,
    0x3064, 0x1802,
    0x3ED2, 0x0146,
    0x3ED4, 0x8F6C,
    0x3ED6, 0x66CC,
    0x3ED8, 0x8C42,
    0x3EDA, 0x88BC,
    0x3EDC, 0xAA63,
    0x305E, 0x00A0,
#endif
};

// previously mode 7
static IMG_UINT16 ui16ModeRegs_1[] = {
    //[AR330-1088p60 2lane Fvco = 768 MHz]
    0x301A, 0x0058,     //RESET_REGISTER = 88
    0x302A, 0x0005,     //VT_PIX_CLK_DIV = 5
    0x302C, 0x0002,     //VT_SYS_CLK_DIV = 2
    0x302E, 0x0001,     //PRE_PLL_CLK_DIV = 1
    0x3030, 0x0020,     //PLL_MULTIPLIER = 32
    0x3036, 0x000A,     //OP_PIX_CLK_DIV = 10
    0x3038, 0x0001,     //OP_SYS_CLK_DIV = 1
    0x31AC, 0x0A0A,     //DATA_FORMAT_BITS = 2570
    0x31AE, 0x0202,     //SERIAL_FORMAT = 514
    0x31B0, 0x003E,     //FRAME_PREAMBLE = 62
    0x31B2, 0x0018,     //LINE_PREAMBLE = 24
    0x31B4, 0x4F66,     //MIPI_TIMING_0 = 20326
    0x31B6, 0x4215,     //MIPI_TIMING_1 = 16917
    0x31B8, 0x308B,     //MIPI_TIMING_2 = 12427
    0x31BA, 0x028A,     //MIPI_TIMING_3 = 650
    0x31BC, 0x8008,     //MIPI_TIMING_4 = 32776
    0x3002, 0x00EA,     //Y_ADDR_START = 234
    0x3004, 0x00C6,     //X_ADDR_START = 198
    0x3006, 0x0529,     //Y_ADDR_END = 1313 //21->29 fengwu
    0x3008, 0x0845,     //X_ADDR_END = 2117
    0x300A, 0x04BC,     //FRAME_LENGTH_LINES = 1212
    0x300C, 0x0420,     //LINE_LENGTH_PCK = 1056
    0x3012, 0x02D7,     //COARSE_INTEGRATION_TIME = 727
    0x3014, 0x0000,     //FINE_INTEGRATION_TIME = 0
    0x30A2, 0x0001,     //X_ODD_INC = 1
    0x30A6, 0x0001,     //Y_ODD_INC = 1
    0x3040, 0x0000,     //READ_MODE = 0
    0x3042, 0x0080,     //EXTRA_DELAY = 128
    0x30BA, 0x002C,     //DIGITAL_CTRL = 44
    0x3088, 0x80BA,     // SEQ_CTRL_PORT
    0x3086, 0x0253,     // SEQ_DATA_PORT
    //
    0x31E0, 0x0303,
    0x3064, 0x1802,
    0x3ED2, 0x0146,
    0x3ED4, 0x8F6C,
    0x3ED6, 0x66CC,
    0x3ED8, 0x8C42,
    0x3EDA, 0x88BC,
    0x3EDC, 0xAA63,
    0x305E, 0x00A0,
};

// similar to mode 7 but slower
static IMG_UINT16 ui16ModeRegs_2[] = {
    //[AR330-1088p30 2lane Fvco = 768 MHz]
    0x301A, 0x0058,     //RESET_REGISTER = 88
    0x302A, 0x0005,     //VT_PIX_CLK_DIV = 5
    0x302C, 0x0002,  // VT_SYS_CLK_DIV -------------------[2 - enable 2lanes], [4 - enable 1lane]
    0x302E, 0x0001,     //PRE_PLL_CLK_DIV = 1
    0x3030, 0x0020,     //PLL_MULTIPLIER = 32
    0x3036, 0x000A,     //OP_PIX_CLK_DIV = 10
    0x3038, 0x0001,     //OP_SYS_CLK_DIV = 1
    0x31AC, 0x0A0A,     //DATA_FORMAT_BITS = 2570
    0x31AE, 0x0202/*0x0202*/,     //SERIAL_FORMAT = 514 ------ [0x201 1 lane] [0x202 2 lanes] [0x204 4 lanes]
    0x31B0, 0x003E,     //FRAME_PREAMBLE = 62
    0x31B2, 0x0018,     //LINE_PREAMBLE = 24
    0x31B4, 0x4F66,     //MIPI_TIMING_0 = 20326
    0x31B6, 0x4215,     //MIPI_TIMING_1 = 16917
    0x31B8, 0x308B,     //MIPI_TIMING_2 = 12427
    0x31BA, 0x028A,     //MIPI_TIMING_3 = 650
    0x31BC, 0x8008,     //MIPI_TIMING_4 = 32776
    0x3002, 0x00EA,     //Y_ADDR_START = 234
    0x3004, 0x00C6,     //X_ADDR_START = 198
    0x3006, 0x0529,     //Y_ADDR_END = 1313 //21->29 fengwu
    0x3008, 0x0845,     //X_ADDR_END = 2117
    0x300A, 0x04BC,     //FRAME_LENGTH_LINES = 1212
    0x300C, 0x0840,     //LINE_LENGTH_PCK = 2112
    0x3012, 0x02D7,     //COARSE_INTEGRATION_TIME = 727
    0x3014, 0x0000,     //FINE_INTEGRATION_TIME = 0
    0x30A2, 0x0001,     //X_ODD_INC = 1
    0x30A6, 0x0001,     //Y_ODD_INC = 1
    0x3040, 0x0000,     //READ_MODE = 0
    0x3042, 0x0080,     //EXTRA_DELAY = 128
    0x30BA, 0x002C,     //DIGITAL_CTRL = 44
    0x3088, 0x80BA,     // SEQ_CTRL_PORT
    0x3086, 0x0253,     // SEQ_DATA_PORT
    //
    0x31E0, 0x0303,
    0x3064, 0x1802,
    0x3ED2, 0x0146,
    0x3ED4, 0x8F6C,
    0x3ED6, 0x66CC,
    0x3ED8, 0x8C42,
    0x3EDA, 0x88BC,
    0x3EDC, 0xAA63,
    0x305E, 0x00A0,

};

static IMG_UINT16 ui16ModeRegs_3[] = {
    //1920x1088x20
    0x301A, 0x0058,     // RESET_REGISTER
    0x3064, 0x1802,     // SMIA TEST
    0x31AE, 0x0202,     // SERIAL_FORMAT
    0x31AC, 0x0A0A,     // DATA_FORMAT_BITS
    0x31B0, 0x0028,     // FRAME_PREAMBLE
    0x31B2, 0x000E,     // LINE_PREAMBLE
    0x31B4, 0x2743,     // MIPI_TIMING_0
    0x31B6, 0x114E,     // MIPI_TIMING_1
    0x31B8, 0x2049,     // MIPI_TIMING_2
    0x31BA, 0x0186,     // MIPI_TIMING_3
    0x31BC, 0x8005,     // MIPI_TIMING_4
    0x31BE, 0x2003,     // MIPI_CONFIG_STATUS
    0x302A, 0x0005,     // VT_PIX_CLK_DIV
    0x302C, 0x0002,  // VT_SYS_CLK_DIV -------------------[2 - enable 2lanes], [4 - enable 1lane]
    0x302E, 0x0008,     // PRE_PLL_CLK_DIV
    0x3030, 0x0060,     // PLL_MULTIPLIER // 20 fps
    0x3036, 0x000A,     // OP_PIX_CLK_DIV
    0x3038, 0x0001,     // OP_SYS_CLK_DIV
    0x31AC, 0x0A0A,     // DATA_FORMAT_BITS
    0x3004, 0x0186,     // X_ADDR_START
    0x3008, 0x0905,     // X_ADDR_END
    0x3002, 0x019C,     // Y_ADDR_START
    //0x3006, 0x05D3,     // Y_ADDR_END
    0x3006, 0x05DB,     // Y_ADDR_END
    0x30A2, 0x0001,     // X_ODD_INC
    0x30A6, 0x0001,     // Y_ODD_INC
    0x3040, 0x0000,     // READ_MODE
    0x3040, 0x0000,     // READ_MODE
    0x3040, 0x0000,     // READ_MODE
    0x300C, 0x0450,     // LINE_LENGTH_PCK
    0x300A, 0x04E0,     // FRAME_LENGTH_LINES
    0x3014, 0x0000,     // FINE_INTEGRATION_TIME
    0x3012, 0x04E0,     // COARSE_INTEGRATION_TIME
    0x3042, 0x0000,     // EXTRA_DELAY
    0x30BA, 0x002C,     // DIGITAL_CTRL
    0x3088, 0x80BA,     // SEQ_CTRL_PORT
    0x3086, 0x0253,     // SEQ_DATA_PORT
};

static IMG_UINT16 ui16ModeRegs_4[] = {
    //[AR330-1088p 15fps 1lane Fvco = ? MHz] for qiwo304 15fps OK.
	0x301A, 0x0058,     //RESET_REGISTER = 88
	0x302A, 0x0005,     //VT_PIX_CLK_DIV = 5
	0x302C, 0x0004,  // VT_SYS_CLK_DIV -------------------[2 - enable 2lanes], [4 - enable 1lane]
	0x302E, 0x0002/*3*/,     //PRE_PLL_CLK_DIV = 1
	0x3030, 0x0020/*5f*/,     //PLL_MULTIPLIER = 32
	0x3036, 0x000a,     //OP_PIX_CLK_DIV = 10
	0x3038, 0x0001,     //OP_SYS_CLK_DIV = 1
	0x31AC, 0x0A0A,     //DATA_FORMAT_BITS = 2570
	0x31AE, 0x0201/*0x0202*/,     //SERIAL_FORMAT = 514 ------ [0x201 1 lane] [0x202 2 lanes] [0x204 4 lanes]
	0x31B0, 0x003E,     //FRAME_PREAMBLE = 62
	0x31B2, 0x0018,     //LINE_PREAMBLE = 24
	0x31B4, 0x3366,     //MIPI_TIMING_0 = 2326
	0x31B6, 0x4215,     //MIPI_TIMING_1 = 16917
	0x31B8, 0x308B,     //MIPI_TIMING_2 = 12427
	0x31BA, 0x028A,     //MIPI_TIMING_3 = 650
	0x31BC, 0x8008,     //MIPI_TIMING_4 = 32776
	0x3002, 0x00EA,     //Y_ADDR_START = 234
	0x3004, 0x00C6,     //X_ADDR_START = 198
	0x3006, 0x0529,     //Y_ADDR_END = 1313 //21->29 fengwu
	0x3008, 0x0845,     //X_ADDR_END = 2117
	0x300A, 0x0450/*4BC*/,     //FRAME_LENGTH_LINES = 1212
	0x300C, 0x0486/*4da*/,     //LINE_LENGTH_PCK = 2112
	0x3012, 0x02D7,     //COARSE_INTEGRATION_TIME = 727
	0x3014, 0x0000,     //FINE_INTEGRATION_TIME = 0
	0x30A2, 0x0001,     //X_ODD_INC = 1
	0x30A6, 0x0001,     //Y_ODD_INC = 1
	0x3040, 0x0000,     //READ_MODE = 0
	0x3042, 0x0080/*40a*/,     //EXTRA_DELAY = 128
	0x30BA, 0x002C,     //DIGITAL_CTRL = 44
	0x3088, 0x80BA,     // SEQ_CTRL_PORT
	0x3086, 0x0253,     // SEQ_DATA_PORT
	//
	0x31E0, 0x0303,
	0x3064, 0x1802,
	0x3ED2, 0x0146,
	0x3ED4, 0x8F6C,
	0x3ED6, 0x66CC,
	0x3ED8, 0x8C42,
	0x3EDA, 0x88BC,
	0x3EDC, 0xAA63,
	0x305E, 0x00A0,
};

static IMG_UINT16 ui16ModeRegs_5[] = {
    //[AR330-1088p 30fps 1lane Fvco = ? MHz] for qiwo
    0x301A, 0x0058,     //RESET_REGISTER = 88
    0x302A, 0x0005,     //VT_PIX_CLK_DIV = 5
    0x302C, 0x0004,  // VT_SYS_CLK_DIV -------------------[2 - enable 2lanes], [4 - enable 1lane]
    0x302E, 0x0001,     //PRE_PLL_CLK_DIV = 1
    0x3030, 0x0020,     //PLL_MULTIPLIER = 32
    0x3036, 0x000A,     //OP_PIX_CLK_DIV = 10
    0x3038, 0x0001,     //OP_SYS_CLK_DIV = 1
    0x31AC, 0x0A0A,     //DATA_FORMAT_BITS = 2570
    0x31AE, 0x0201/*0x0202*/,     //SERIAL_FORMAT = 514 ------ [0x201 1 lane] [0x202 2 lanes] [0x204 4 lanes]
    0x31B0, 0x003E,     //FRAME_PREAMBLE = 62
    0x31B2, 0x0018,     //LINE_PREAMBLE = 24
    0x31B4, 0x4F66,     //MIPI_TIMING_0 = 20326
    0x31B6, 0x4215,     //MIPI_TIMING_1 = 16917
    0x31B8, 0x308B,     //MIPI_TIMING_2 = 12427
    0x31BA, 0x028A,     //MIPI_TIMING_3 = 650
    0x31BC, 0x8008,     //MIPI_TIMING_4 = 32776
    0x3002, 0x00EA,     //Y_ADDR_START = 234
    0x3004, 0x00C6,     //X_ADDR_START = 198
    0x3006, 0x0529,     //Y_ADDR_END = 1313 //21->29 fengwu
    0x3008, 0x0845,     //X_ADDR_END = 2117
    0x300A, 0x0450,     //0x4bcFRAME_LENGTH_LINES = 1212
    0x300C, 0x0484,     //0x840LINE_LENGTH_PCK = 2112//484
    0x3012, 0x02D7,     //COARSE_INTEGRATION_TIME = 727
    0x3014, 0x0000,     //FINE_INTEGRATION_TIME = 0
    0x30A2, 0x0001,     //X_ODD_INC = 1
    0x30A6, 0x0001,     //Y_ODD_INC = 1
    0x3040, 0x0000,     //READ_MODE = 0
    0x3042, 0x0080,     //EXTRA_DELAY = 128
    0x30BA, 0x002C,     //DIGITAL_CTRL = 44
    0x3088, 0x80BA,     // SEQ_CTRL_PORT
    0x3086, 0x0253,     // SEQ_DATA_PORT
    //
    0x31E0, 0x0303,
    0x3064, 0x1802,
    0x3ED2, 0x0146,
    0x3ED4, 0x8F6C,
    0x3ED6, 0x66CC,
    0x3ED8, 0x8C42,
    0x3EDA, 0x88BC,
    0x3EDC, 0xAA63,
    0x305E, 0x00A0,

};

static IMG_UINT16 ui16ModeRegs_6[] = {
    //mipi 30fps lanes = 2 for evb1.1
    0x301A, 0x0058, 	// Disable Streaming
	0x31AE, 0x0202, 	// SERIAL_FORMAT
	0x31AC, 0x0A0A, 	// DATA_FORMAT_BITS = 10-bit

	0x31B0, 0x0028, 	// FRAME_PREAMBLE = 40 clocks
	0x31B2, 0x000E, 	// LINE_PREAMBLE = 14 clocks
	0x31B4, 0x2743, 	// MIPI_TIMING_0
	0x31B6, 0x124E, 	// MIPI_TIMING_1 //0x114E
	0x31B8, 0x2049, 	// MIPI_TIMING_2
	0x31BA, 0x0186, 	// MIPI_TIMING_3
	0x31BC, 0x8005, 	// MIPI_TIMING_4
	0x31BE, 0x2003, 	// MIPI_CONFIG_STATUS

	0x302A, 0x0005,		//VT_PIX_CLK_DIV = 5
	0x302C, 0x0002, 	//VT_SYS_CLK_DIV = 2
	0x302E, 0x0002, 	//PRE_PLL_CLK_DIV = 3
	0x3030, 0x0020, 	//PLL_MULTIPLIER = 80
	0x3036, 0x000A, 	//OP_PIX_CLK_DIV = 10
	0x3038, 0x0001, 	//OP_SYS_CLK_DIV = 1

	0x3002, 0x00EA, 	//Y_ADDR_START = 234
	0x3004, 0x00C6, 	//X_ADDR_START = 198
	0x3006, 0x0529, 	//Y_ADDR_END = 1313
	0x3008, 0x0845, 	//X_ADDR_END = 2117
	0x300A, 0x0450, 	//FRAME_LENGTH_LINES = 1717
	0x300C, 0x0486, 	//LINE_LENGTH_PCK = 1242
//	0x300A, 0x0444, 	//FRAME_LENGTH_LINES = 1092
//	0x300C, 0x07A1, 	//LINE_LENGTH_PCK = 1953
	0x3012, 0x02d7,   //1091
	//0x3012, 0x0612, 	//COARSE_INTEGRATION_TIME = 1554
	0x3014, 0x0000,		//FINE_INTEGRATION_TIME = 0
	0x30A2, 0x0001,		//X_ODD_INC = 1
	0x30A6, 0x0001,		//Y_ODD_INC = 1
	0x3040, 0x0000,		//READ_MODE = 0
	0x30BA, 0x002C,		//DIGITAL_CTRL = 44
	//0x3070, 0x02,
	0x3088, 0x80BA,     // SEQ_CTRL_PORT
    0x3086, 0x0253,     // SEQ_DATA_PORT
//	/Recommended Cnfiguration
	0x31E0, 0x0303,
	0x3064, 0x1802,
	0x3ED2, 0x0146,
	0x3ED4, 0x8F6C,
	0x3ED6, 0x66CC,
	0x3ED8, 0x8C42,
	0x3EDA, 0x88BC,
	0x3EDC, 0xAA63,
	0x305E, 0x00A0,
	//0x3064, 0x1802,
	0x301A, 0x0058,
};

//1296*1296@30fps
static IMG_UINT16 ui16ModeRegs_7[] = {
	0x301A, 0x0058,		//RESET_REGISTER = 88
	0x302A, 0x0006,		//VT_PIX_CLK_DIV = 6
	0x302C, 0x0002,		//VT_SYS_CLK_DIV = 2
	0x302E, 0x0001,		//PRE_PLL_CLK_DIV = 1
	0x3030, 0x0020,		//PLL_MULTIPLIER = 32
	0x3036, 0x000C,		//OP_PIX_CLK_DIV = 12
	0x3038, 0x0001,		//OP_SYS_CLK_DIV = 1
	0x31AC, 0x0C0C,		//DATA_FORMAT_BITS = 3084
	0x31AE, 0x0202,		//SERIAL_FORMAT = 514

	//MIPI Port Timing
	0x31B0, 0x0038,		//FRAME_PREAMBLE = 56
	0x31B2, 0x0016,		//LINE_PREAMBLE = 22
	0x31B4, 0x3E55,		//MIPI_TIMING_0 = 15957
	0x31B6, 0x41D1,		//MIPI_TIMING_1 = 16849
	0x31B8, 0x208A,		//MIPI_TIMING_2 = 8330
	0x31BA, 0x0288,		//MIPI_TIMING_3 = 648
	0x31BC, 0x8007,		//MIPI_TIMING_4 = 32775


	//Timing_settings
	0x3002, 0x007E,		//Y_ADDR_START = 126
	0x3004, 0x01FE,		//X_ADDR_START = 510
	0x3006, 0x058D,		//Y_ADDR_END = 1421
	0x3008, 0x070D,		//X_ADDR_END = 1805
	0x300A, 0x06B5,		//FRAME_LENGTH_LINES = 1717
	0x300C, 0x04DA,		//LINE_LENGTH_PCK = 1242
	0x3012, 0x035A,		//COARSE_INTEGRATION_TIME = 858
	0x3014, 0x0000,		//FINE_INTEGRATION_TIME = 0
	0x30A2, 0x0001,		//X_ODD_INC = 1
	0x30A6, 0x0001,		//Y_ODD_INC = 1
	0x308C, 0x0006,		//Y_ADDR_START_CB = 6
	0x308A, 0x0006,		//X_ADDR_START_CB = 6
	0x3090, 0x0605,		//Y_ADDR_END_CB = 1541
	0x308E, 0x0905,		//X_ADDR_END_CB = 2309
	0x30AA, 0x060C,		//FRAME_LENGTH_LINES_CB = 1548
	0x303E, 0x04E0,		//LINE_LENGTH_PCK_CB = 1248
	0x3016, 0x060B,		//COARSE_INTEGRATION_TIME_CB = 1547
	0x3018, 0x0000,		//FINE_INTEGRATION_TIME_CB = 0
	0x30AE, 0x0001,		//X_ODD_INC_CB = 1
	0x30A8, 0x0001,		//Y_ODD_INC_CB = 1
	0x3040, 0x0000,		//READ_MODE = 0
	0x3042, 0x0333,		//EXTRA_DELAY = 819
	0x30BA, 0x002C,		//DIGITAL_CTRL = 44



	//Recommended Configuration
	0x31E0, 0x0303,
	0x3064, 0x1802,
	0x3ED2, 0x0146,
	0x3ED4, 0x8F6C,
	0x3ED6, 0x66CC,
	0x3ED8, 0x8C42,
	0x3EDA, 0x88BC,
	0x3EDC, 0xAA63,
	0x305E, 0x00A0,
};

//1536*1536@30fps
static IMG_UINT16 ui16ModeRegs_8[] = {
	0x301A, 0x0058,		//RESET_REGISTER = 88
	0x302A, 0x0006,		//VT_PIX_CLK_DIV = 6
	0x302C, 0x0002,		//VT_SYS_CLK_DIV = 2
	0x302E, 0x0001,		//PRE_PLL_CLK_DIV = 1
	0x3030, 0x0020,		//PLL_MULTIPLIER = 32
	0x3036, 0x000C,		//OP_PIX_CLK_DIV = 12
	0x3038, 0x0001,		//OP_SYS_CLK_DIV = 1
	0x31AC, 0x0C0C,		//DATA_FORMAT_BITS = 3084
	0x31AE, 0x0202,		//SERIAL_FORMAT = 514
	
	//MIPI Port Timing
	0x31B0, 0x0038,		//FRAME_PREAMBLE = 56
	0x31B2, 0x0016,		//LINE_PREAMBLE = 22
	0x31B4, 0x3E55,		//MIPI_TIMING_0 = 15957
	0x31B6, 0x41D1,		//MIPI_TIMING_1 = 16849
	0x31B8, 0x208A,		//MIPI_TIMING_2 = 8330
	0x31BA, 0x0288,		//MIPI_TIMING_3 = 648
	0x31BC, 0x8007,		//MIPI_TIMING_4 = 32775
	
	
	//Timing_settings
	0x3002, 0x0006,		//Y_ADDR_START = 6
	0x3004, 0x0186,		//X_ADDR_START = 390
	0x3006, 0x0605,		//Y_ADDR_END = 1541
	0x3008, 0x0785,		//X_ADDR_END = 1925
	0x300A, 0x06B5,		//FRAME_LENGTH_LINES = 1717
	0x300C, 0x04DA,		//LINE_LENGTH_PCK = 1242
	0x3012, 0x035A,		//COARSE_INTEGRATION_TIME = 858
	0x3014, 0x0000,		//FINE_INTEGRATION_TIME = 0
	0x30A2, 0x0001,		//X_ODD_INC = 1
	0x30A6, 0x0001,		//Y_ODD_INC = 1
	0x308C, 0x0006,		//Y_ADDR_START_CB = 6
	0x308A, 0x0006,		//X_ADDR_START_CB = 6
	0x3090, 0x0605,		//Y_ADDR_END_CB = 1541
	0x308E, 0x0905,		//X_ADDR_END_CB = 2309
	0x30AA, 0x060C,		//FRAME_LENGTH_LINES_CB = 1548
	0x303E, 0x04E0,		//LINE_LENGTH_PCK_CB = 1248
	0x3016, 0x060B,		//COARSE_INTEGRATION_TIME_CB = 1547
	0x3018, 0x0000,		//FINE_INTEGRATION_TIME_CB = 0
	0x30AE, 0x0001,		//X_ODD_INC_CB = 1
	0x30A8, 0x0001,		//Y_ODD_INC_CB = 1
	0x3040, 0x0000,		//READ_MODE = 0
	0x3042, 0x0333,		//EXTRA_DELAY = 819
	0x30BA, 0x002C,		//DIGITAL_CTRL = 44
	
	
	
	//Recommended Configuration
	0x31E0, 0x0303,
	0x3064, 0x1802,
	0x3ED2, 0x0146,
	0x3ED4, 0x8F6C,
	0x3ED6, 0x66CC,
	0x3ED8, 0x8C42,
	0x3EDA, 0x88BC,
	0x3EDC, 0xAA63,
	0x305E, 0x00A0,

};



static IMG_RESULT doStaticSetup(AR330CAM_STRUCT *psCam, IMG_INT16 ui16Width,
        IMG_UINT16 ui16Height, IMG_UINT16 ui16Mode, IMG_BOOL bFlipHorizontal,
        IMG_BOOL bFlipVertical);

#ifdef INFOTM_ISP
static IMG_RESULT AR330MIPI_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag);
static IMG_RESULT AR330MIPI_SetFPS(SENSOR_HANDLE hHandle, double fps);
static IMG_RESULT AR330MIPI_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed);
static IMG_RESULT AR330MIPI_Reset(SENSOR_HANDLE hHandle);
#endif //INFOTM_ISP
static int AR330MIPI_I2C_WRITE(int i2c,  IMG_UINT16 reg, IMG_UINT16 val)
{
    struct i2c_rdwr_ioctl_data packets; 
    struct i2c_msg messages[1];

    uint8_t buf[4];
    buf[0] = (uint8_t)((reg >> 8) & 0xff);
    buf[1] = (uint8_t)(reg & 0xff);
    buf[2] = (uint8_t)((val >> 8) & 0xff);
    buf[3] = (uint8_t)(val & 0xff);

    messages[0].addr  = i2c_addr;
    messages[0].flags = 0;  
    messages[0].len   = sizeof(buf);  
    messages[0].buf   = buf;  

    packets.msgs  = messages;  
    packets.nmsgs = 1;  

    if(ioctl(i2c, I2C_RDWR, &packets) < 0) {  
        LOG_ERROR("Unable to write data: 0x%04x 0x%04x.\n", reg, val);  
        return 1;  
    }

    //usleep(5);

    //LOG_DEBUG("WR:0x%04x 0x%04x\n", reg, val);

    return 0;  
}  

static uint16_t AR330MIPI_I2C_READ(int i2c, IMG_UINT16 reg)
{
    uint8_t buf[2];

    struct i2c_rdwr_ioctl_data packets;  
    struct i2c_msg messages[2];  

    buf[0] = (uint8_t)((reg >> 8) & 0xff);
    buf[1] = (uint8_t)(reg & 0xff);

    messages[0].addr  = i2c_addr;
    messages[0].flags = 0;  
    messages[0].len   = sizeof(buf);  
    messages[0].buf   = buf;  

    messages[1].addr  = i2c_addr;
    messages[1].flags = I2C_M_RD;  
    messages[1].len   = sizeof(buf);  
    messages[1].buf   = buf;  

    packets.msgs      = messages;  
    packets.nmsgs     = 2;  

    if(ioctl(i2c, I2C_RDWR, &packets) < 0) {  
        LOG_ERROR("Unable to read data: 0x%04x.\n", reg);  
        return 1;  
    }

    usleep(5);

    LOG_DEBUG("RD:0x%04x 0x%04x\n", reg, (buf[0] << 8) + buf[1]);
    return (buf[0] << 8) + buf[1];  

}
#if 1

static IMG_RESULT ar330mipi_i2c_write16(int i2c, IMG_UINT16 *data, size_t len)
{
#ifdef INFOTM_ISP
    AR330MIPI_I2C_WRITE(i2c, data[0], data[1]);
#else
#if !file_i2c
    size_t i;
    IMG_UINT8 buf[len * sizeof(*data)];
    IMG_UINT8 *buf_p = buf;

    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }

    /* Prepare 8-bit data using 16-bit input data. */
    for (i = 0; i < len; ++i)
    {
        *buf_p++ = data[i] >> 8;
        *buf_p++ = data[i] & 0xff;
    }

    write(i2c, buf, len * sizeof(*data));
#endif
#endif //INFOTM_ISP
    return IMG_SUCCESS;
}
#endif
static IMG_RESULT AR330MIPI_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
//    int g = 0;
//    CI_HWINFO *pInfo = NULL;
    //AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
    /*
       if (!psCam->psSensorPhy)
       {
       LOG_ERROR("sensor not initialised\n");
       return IMG_ERROR_NOT_INITIALISED;
       }*/
/*
    if (psCam->bIsMIPI)
        IMG_ASSERT(strlen(AR330MIPI_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    else
        IMG_ASSERT(strlen(AR330DVP_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(AR330_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);
*/
    //pInfo = &(psCam->psSensorPhy->psConnection->sHWInfo);

	psInfo->eBayerOriginal = MOSAIC_GRBG;
    psInfo->eBayerEnabled = MOSAIC_GRBG;
    // because it seems AR330 conserves same pattern - otherwise uncomment
    /*if (SENSOR_FLIP_NONE!=psInfo->sStatus.ui8Flipping)
      {
      psInfo->eBayerEnabled = MosaicFlip(psInfo->eBayerOriginal,
      psInfo->sStatus.ui8Flipping&SENSOR_FLIP_HORIZONTAL?1:0,
      psInfo->sStatus.ui8Flipping&SENSOR_FLIP_VERTICAL?1:0);
      }*/

    //sprintf(psInfo->pszSensorName, AR330MIPI_SENSOR_INFO_NAME);
    sprintf(psInfo->pszSensorName, AR330_SENSOR_VERSION); /// @ could we read that from sensor?
    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 7500;
    //psInfo->ui8BitDepth = 10;
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = imgr;
    psInfo->bBackFacing = IMG_TRUE;
	// other fields should be filled by calling function Sensor_GetInfo() before this is called
/*
    for ( g = 0 ; g < pInfo->config_ui8NImagers ; g++ )
    {
        if ( (pInfo->gasket_aType[g]&CI_GASKET_PARALLEL) != 0 )
        {
            break;
        }
    }
    if ( g != pInfo->config_ui8NImagers )
    {
        psInfo->ui8Imager = g;
        LOG_DEBUG("selecting gasket %d (first Parallel found)\n", g);
    }
    else
    {
        LOG_ERROR("could not find a parallel gasket!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
*/
    return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes)
{
	//AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
    if(nIndex < sizeof(asModes) / sizeof(SENSOR_MODE))
    {
		IMG_MEMCPY(psModes, &(asModes[nIndex]), sizeof(SENSOR_MODE));
        return IMG_SUCCESS;
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

static IMG_RESULT AR330MIPI_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
{
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_WARNING("sensor not initialised\n");
        psStatus->eState = SENSOR_STATE_UNINITIALISED;
        psStatus->ui16CurrentMode = 0;
    }
    else
    {
        psStatus->eState = (psCam->bEnabled ? SENSOR_STATE_RUNNING : SENSOR_STATE_IDLE);
        psStatus->ui16CurrentMode = psCam->ui16CurrentMode;
		psStatus->ui8Flipping = psCam->ui8CurrentFlipping;
		psStatus->fCurrentFps = psCam->fCurrentFps;
    }
    return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Mode,
        IMG_UINT8 ui8Flipping)
{
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(ui16Mode < (sizeof(asModes) / sizeof(SENSOR_MODE)))
    {
        psCam->ui16CurrentMode = ui16Mode;
		if((ui8Flipping&(asModes[ui16Mode].ui8SupportFlipping))!=ui8Flipping)
        {
            LOG_ERROR("sensor mode does not support selected flipping 0x%x (supports 0x%x)\n", 
                    ui8Flipping, asModes[ui16Mode].ui8SupportFlipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        ret = doStaticSetup(psCam, asModes[ui16Mode].ui16Width, asModes[ui16Mode].ui16Height,
                ui16Mode, ui8Flipping&SENSOR_FLIP_HORIZONTAL, ui8Flipping&SENSOR_FLIP_VERTICAL);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to configure mode %d\n", ui16Mode);
        }
		else
        {
            psCam->ui16CurrentMode = ui16Mode;
            psCam->ui8CurrentFlipping = ui8Flipping;
        }
        return ret;
    }

    LOG_ERROR("invalid mode %d\n", ui16Mode);
    return IMG_ERROR_INVALID_PARAMETERS;
}

static IMG_RESULT AR330MIPI_Enable(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    IMG_RESULT ret;
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_DEBUG("Enabling AR330 Camera!:psCam->psSensorPhy=0x%x, psConnection=0x%x\n",
                psCam->psSensorPhy, psCam->psSensorPhy->psConnection);
    psCam->bEnabled = IMG_TRUE;
    aui16Regs[0] = 0x301a; // RESET_REGISTER
    aui16Regs[1] = 0x005C;
    // setup the sensor number differently when using multiple sensors
    ret = SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->ui8MipiLanes, 0);
    if (ret == IMG_SUCCESS)
    {
        ret = AR330MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to enable sensor\n");
        }
        else
        {
            LOG_DEBUG("camera enabled\n");
        }
    }
    return ret;
}

static IMG_RESULT AR330MIPI_DisableSensor(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
    IMG_RESULT ret = IMG_SUCCESS;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Disabling AR330 camera\n");
    psCam->bEnabled = IMG_FALSE;
    // if we are generating a file we don't want to disable!
    aui16Regs[0] = 0x301a; // RESET_REGISTER
    aui16Regs[1] = 0x0058;
    ret = AR330MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to disable sensor!\n");
    }
    usleep(100000);
    SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, 0);
    return ret;
}

static IMG_RESULT AR330MIPI_DestroySensor(SENSOR_HANDLE hHandle)
{
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Destroying AR330 camera\n");
    // remember to free anything that might have been allocated dynamically (like extended params...)
    if(psCam->bEnabled)
        AR330MIPI_DisableSensor(hHandle);
    SensorPhyDeinit(psCam->psSensorPhy);

    close(psCam->i2c);

    IMG_FREE(psCam);
    return IMG_SUCCESS;
}

#if 0
static IMG_RESULT AR330MIPI_ConfigureFlash(SENSOR_HANDLE hHandle,
        IMG_BOOL bAlwaysOn, IMG_INT16 i16FrameDelay, IMG_INT16 i16Frames,
        IMG_UINT16 ui16FlashPulseWidth)
{
    IMG_UINT16 aui16Regs[4];
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (i16Frames > 6)
        i16Frames = 6;
    if(i16FrameDelay > 3)
        i16FrameDelay = 3;

    aui16Regs[0] = 0x3046; // Global Gain register
    aui16Regs[1] = (IMG_UINT16)(bAlwaysOn?((1<<8)|(7<<3)):(i16Frames<<3)|i16FrameDelay);
    //ret = AR330MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    aui16Regs[0] = 0x3048; // Global Gain register
    aui16Regs[1] = ui16FlashPulseWidth;
    ret = AR330MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#if defined(ENABLE_AE_DEBUG_MSG)
    printf("AR330MIPI_ConfigureFlash Change Global Gain 0x3046 = 0x%04X\n", aui16Regs[1]);
#endif
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to configure flash!\n");
    }
    return ret;
}
#endif
static IMG_RESULT AR330MIPI_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current = psCam->ui32Exposure;
    return IMG_SUCCESS;
}


static IMG_RESULT AR330MIPI_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Current,
        IMG_UINT8 ui8Context)
{
    IMG_RESULT ret = IMG_SUCCESS;
#if !defined(DISABLE_AE)
    IMG_UINT16 aui16Regs[2];
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui32Exposure = ui32Current;

    aui16Regs[0] = 0x3012; // COARSE_INTEGRATION_TIME
    //aui16Regs[1] = psCam->ui32Exposure / psCam->ui32ExposureMin;
    aui16Regs[1] = psCam->ui32Exposure / psCam->dbExposureMin;
    //aui16Regs[1] = ((aui16Regs[1] > 0) ? aui16Regs[1] : 1);
    ret = AR330MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
  #if defined(ENABLE_AE_DEBUG_MSG)
	printf("AR330DVP_SetExposure Exposure = 0x%08X, ExposureMin = 0x%08X\n", psCam->ui32Exposure, psCam->ui32ExposureMin);
    printf("AR330DVP_SetExposure Change COARSE_INTEGRATION_TIME 0x3012 = 0x%04X\n", aui16Regs[1]);
  #endif
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set exposure\n");
    }

#endif
    return ret;
}

static IMG_RESULT AR330MIPI_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
        double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

      if (!psCam->psSensorPhy)
      {
      LOG_ERROR("sensor not initialised\n");
      return IMG_ERROR_NOT_INITIALISED;
      }*/

    *pflMin = 1.0;// / 128;
    *pflMax = 15.0;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_GetGain(SENSOR_HANDLE hHandle,
        double *pflGain, IMG_UINT8 ui8Context)
{
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain = psCam->flGain;
    return IMG_SUCCESS;
}

typedef struct _argain_ {
    double flGain;
    unsigned char ui8GainRegVal;
}ARGAINTABLE;

static ARGAINTABLE asARGainValues[]=
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


static IMG_RESULT AR330MIPI_SetGain(SENSOR_HANDLE hHandle, double flGain,
        IMG_UINT8 ui8Context)
{
    IMG_UINT16 aui16Regs[4];
    unsigned int nIndex = 0;
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->flGain = flGain;

    while(nIndex < (sizeof(asARGainValues) / sizeof(ARGAINTABLE)) - 1)
    {
        if(asARGainValues[nIndex].flGain > flGain)
            break;
        nIndex++;
    }

    if(nIndex > 0)
        nIndex = nIndex - 1;

    flGain = psCam->flGain / asARGainValues[nIndex].flGain;

    aui16Regs[0] = 0x305e;// Global Gain register
    aui16Regs[1] = (IMG_UINT16)(flGain*128);
    ret = AR330MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#if defined(ENABLE_AE_DEBUG_MSG)
    printf("AR330DVP_SetGain Change Global Gain 0x305E = 0x%04X\n", aui16Regs[1]);
#endif
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set global gain register\n");
        return ret;
    }

    aui16Regs[0] = 0x3060; // AnalogGainRegisters
    aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)asARGainValues[nIndex].ui8GainRegVal|((IMG_UINT16)asARGainValues[nIndex].ui8GainRegVal<<8));
    ret = AR330MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#if defined(ENABLE_AE_DEBUG_MSG)
    printf("AR330DVP_SetGain Change Analog Gain 0x3060 = 0x%04X\n", aui16Regs[1]);
#endif
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set analogue gain register\n");
    }
    return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
        IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui32Min = psCam->ui32ExposureMin;
    *pui32Max = psCam->ui32ExposureMax;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

//int AR330_Create_MIPI(SENSOR_HANDLE *phHandle)
IMG_RESULT AR330_Create_MIPI(struct _Sensor_Functions_ **hHandle, int index)
{
    char i2c_dev_path[NAME_MAX];
    AR330CAM_STRUCT *psCam;
    char dev_nm[64];
    char adaptor[64];
    char path[128];
    int chn = 0;
#if defined(INFOTM_ISP)
    int fd = 0;
    memset((void *)dev_nm, 0, sizeof(dev_nm));
    memset((void *)adaptor, 0, sizeof(adaptor));
    memset(path, 0, sizeof(path));
    sprintf(dev_nm, "%s%d", DEV_PATH, index );
    fd = open(dev_nm, O_RDWR);
	if(fd <0)
	{
		LOG_ERROR("open %s error\n", dev_nm);
		return IMG_ERROR_FATAL;
	}

    ioctl(fd, GETI2CADDR, &i2c_addr);
    ioctl(fd, GETI2CCHN, &chn);
    ioctl(fd, GETIMAGER, &imgr);
    ioctl(fd, GETSENSORPATH, path);
	close(fd);
	//printf("%s opened OK, i2c-addr=0x%x, chn = %d\n", dev_nm, i2c_addr, chn);
	sprintf(adaptor, "%s-%d", "i2c", chn);
	memset((void *)extra_cfg, 0, sizeof(dev_nm));

    if (path[0] == 0) {
        sprintf(extra_cfg, "%s%s%d-config.txt", EXTRA_CFG_PATH, "sensor" , index );
    } else {
        strcpy(extra_cfg, path);
    }
#endif
    psCam = (AR330CAM_STRUCT *)IMG_CALLOC(1, sizeof(AR330CAM_STRUCT));
    if (!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    *hHandle = &psCam->sFuncs;
    psCam->sFuncs.GetMode = AR330MIPI_GetMode;
    psCam->sFuncs.GetState = AR330MIPI_GetState;
    psCam->sFuncs.SetMode = AR330MIPI_SetMode;
    psCam->sFuncs.Enable = AR330MIPI_Enable;
    psCam->sFuncs.Disable = AR330MIPI_DisableSensor;
    psCam->sFuncs.Destroy = AR330MIPI_DestroySensor;

    psCam->sFuncs.GetGainRange = AR330MIPI_GetGainRange;
    psCam->sFuncs.GetCurrentGain = AR330MIPI_GetGain;
    psCam->sFuncs.SetGain = AR330MIPI_SetGain;

    psCam->sFuncs.SetGainAndExposure = AR330MIPI_SetGainAndExposure;

    psCam->sFuncs.GetExposureRange = AR330MIPI_GetExposureRange;
    psCam->sFuncs.GetExposure = AR330MIPI_GetExposure;
    psCam->sFuncs.SetExposure = AR330MIPI_SetExposure;

    psCam->sFuncs.GetInfo = AR330MIPI_GetInfo;

#ifdef INFOTM_ISP
    psCam->sFuncs.SetFlipMirror = AR330MIPI_SetFlipMirror;
    psCam->sFuncs.SetFPS = AR330MIPI_SetFPS;
    psCam->sFuncs.GetFixedFPS =  AR330MIPI_GetFixedFPS;
    psCam->sFuncs.SetResolution = AR330MIPI_ChangeRES;
    psCam->sFuncs.GetSnapRes = AR330MIPI_GetSnapRes;
    psCam->sFuncs.Reset = AR330MIPI_Reset;
#endif
//    psCam->sFuncs.GetExtendedParamInfo = AR330MIPI_GetExtendedParamInfo;
//    psCam->sFuncs.GetExtendedParamValue = AR330MIPI_GetExtendedParamValue;
//    psCam->sFuncs.SetExtendedParamValue = AR330MIPI_SetExtendedParamValue;

    psCam->bEnabled = IMG_FALSE;
    psCam->ui16CurrentMode = 0;
    psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;

    if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), i2c_addr, adaptor))
    {
        LOG_ERROR("Failed to find I2C device!\n");
        IMG_FREE(psCam);
        *hHandle=NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    psCam->i2c = open(i2c_dev_path, O_RDWR);
    if (psCam->i2c < 0) {
        LOG_ERROR("Failed to open I2C device: \"%s\", err = %d\n",
                i2c_dev_path, psCam->i2c);
        IMG_FREE(psCam);
        *hHandle=NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    psCam->psSensorPhy = SensorPhyInit();
    LOG_DEBUG("in arr330mipi create function : psCam->psSensorPhy = 0x%x, psConnection =0x%x\n",
            psCam->psSensorPhy,
            psCam->psSensorPhy->psConnection);
    return IMG_SUCCESS;
}

#define CASE_MODE(N) \
case N:\
    registerArray = ui16ModeRegs_##N;\
    nRegisters = sizeof(ui16ModeRegs_##N)/(2*sizeof(IMG_UINT16)); \
    break;

static IMG_RESULT doStaticSetup(AR330CAM_STRUCT *psCam, IMG_INT16 ui16Width,
        IMG_UINT16 ui16Height, IMG_UINT16 ui16Mode, IMG_BOOL bFlipHorizontal,
        IMG_BOOL bFlipVertical)
{
    int n;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_UINT32 coarsetime = 0;
    IMG_UINT32 framelenghtlines = 0;
    IMG_UINT32 line_length_pck = 1; // default to 1 to avoid division by 0 if failed...
    IMG_UINT32 pre_pll_clk_div = 0, pll_multiplier = 0, vt_sys_clk_div = 0, vt_pix_clk_div = 0;
    IMG_UINT16 ui16read_mode;

    IMG_UINT16 *registerArray = NULL;
    int nRegisters = 0;
	
	IMG_ASSERT(sizeof(asModes)/sizeof(SENSOR_MODE)>=3);
	
    switch(ui16Mode)
    {
        CASE_MODE(0)
        CASE_MODE(1)
        CASE_MODE(2)
        CASE_MODE(3)
		CASE_MODE(4)
        CASE_MODE(5)
        CASE_MODE(6)
#if defined(OLD_SETTINGS)
        CASE_MODE(7)
#endif
         
        default:
            LOG_ERROR("unknown mode %d\n", ui16Mode);
            return IMG_ERROR_FATAL;
    }
    //if extra configuration file, now override
    if (access(extra_cfg, F_OK) == 0) {
        FILE * fp = NULL;
        IMG_UINT16 val[2];
        char ln[256];
        char *pstr = NULL;
        char *pt = NULL;  //for macro delete_space used
        int  i = 0;
        fp = fopen(extra_cfg, "r");
        if (NULL == fp) {
            LOG_ERROR("open %s failed\n", extra_cfg);
            return IMG_ERROR_FATAL;
        }
        printf("use configuration file configure sensor!\n");
        memset(ln, 0, sizeof(ln));
        while(fgets(ln, sizeof(ln), fp) != NULL) {
            i = 0;
            pt = ln;
            delete_space(pt);
            //printf("ln = %s\n", ln);
            if (0 == strncmp(ln, "0x", 2)) {
                pstr = strtok(ln, ",");
                while(pstr != NULL) {


                    if(0 == strncmp(pstr, "0x", 2)) {
                        val[i] = strtoul(pstr, NULL, 0);
                        i++;
                    }

                    pstr = strtok(NULL, ",");

                }
               // printf("val[0] = 0x%x, val[1]=0%x,\n", val[0], val[1]);
                ret = AR330MIPI_I2C_WRITE(psCam->i2c, val[0], val[1]);
                if(ret != IMG_SUCCESS)
                {
                   return ret;
                }

            }else if (ln[0] == 'w') {
                pstr = strtok(ln, ",");
                ui16Width = strtoul((pstr = strtok(NULL, ",")), NULL, 0);
            }else if (ln[0] == 'h') {
                pstr = strtok(ln, ",");
                ui16Height= strtoul((pstr = strtok(NULL, ",")), NULL, 0);
            }
            //printf("\n");
            //printf("w = %d, h = %d\n", ui16Width, ui16Height);
            if(val[0] == 0x3012)
            // store the coarse integration time, so we can report it correctly
            coarsetime = val[1];
            if(val[0] == 0x31ae)
            psCam->ui8MipiLanes = val[1] & 0xf;
            if(val[0] == 0x300A)
            framelenghtlines = val[1];
            if(val[0] == 0x3040)
            {
            ui16read_mode = val[1] & 0x3fff;
            val[1] = ui16read_mode | (bFlipHorizontal?0x4000:0) | (bFlipVertical?0x8000:0);
            //          registerArray[n+1] = 0x0000;
            }
            if(val[0] == 0x300c)
            // store the coarse line_length_pck time
            line_length_pck = val[1];
            if(val[0] == 0x302a)
            // store the coarse vt_pix_clk_div time
            vt_pix_clk_div = val[1];
            if(val[0] == 0x302c)
            // store the coarse vt_sys_clk_div time
            vt_sys_clk_div = val[1];
            if(val[0] == 0x3030)
            // store the coarse pll_multiplier time
            pll_multiplier = val[1];
            if(val[0] == 0x302e)
            // store the coarse pre_pll_clk_div time,
            pre_pll_clk_div = val[1];
        }

    } else {
        printf("the sensor config file not exist, use default!\n");
        for (n = 0; n < nRegisters * 2; n += 2)
        {
            LOG_DEBUG("Writing %04x,%04x\n", registerArray[n], registerArray[n+1]);
            if(registerArray[n] == 0x3012)
                // store the coarse integration time, so we can report it correctly
                coarsetime = registerArray[n+1];
            if(registerArray[n] == 0x31ae)
                psCam->ui8MipiLanes = registerArray[n+1] & 0xf;
            if(registerArray[n] == 0x300A)
                framelenghtlines = registerArray[n+1];
            if(registerArray[n] == 0x3040)
            {
                ui16read_mode = registerArray[n+1] & 0x3fff;
                registerArray[n+1] = ui16read_mode | (bFlipHorizontal?0x4000:0) | (bFlipVertical?0x8000:0);
                //			registerArray[n+1] = 0x0000;
            }
            if(registerArray[n] == 0x300c)
                // store the coarse line_length_pck time
                line_length_pck = registerArray[n+1];
            if(registerArray[n] == 0x302a)
                // store the coarse vt_pix_clk_div time
                vt_pix_clk_div = registerArray[n+1];
            if(registerArray[n] == 0x302c)
                // store the coarse vt_sys_clk_div time
                vt_sys_clk_div = registerArray[n+1];
            if(registerArray[n] == 0x3030)
                // store the coarse pll_multiplier time
                pll_multiplier = registerArray[n+1];
            if(registerArray[n] == 0x302e)
                // store the coarse pre_pll_clk_div time,
                pre_pll_clk_div = registerArray[n+1];

            ret = AR330MIPI_I2C_WRITE(psCam->i2c, registerArray[n], registerArray[n+1]);
            if(ret != IMG_SUCCESS)
            {
                return ret;
            }
        }
    }
    if (IMG_SUCCESS == ret)
    {
        double ref = 24 * 1000 * 1000;
        double clk_pix = (((ref / pre_pll_clk_div) * pll_multiplier) / vt_sys_clk_div) / vt_pix_clk_div;
        // line_length_pck in pixels, clk_pix in Hz, trow in seconds (time in a row)
        double trow = (line_length_pck * 1000 * 1000) / clk_pix; 

        LOG_DEBUG("Sensor ready to go\n");

		// setup the exposure setting information
        psCam->ui32ExposureMax = (IMG_UINT32)floor(trow * framelenghtlines);
        // Minimum exposure time in us (line period) (use the frmae length lines entry to calculate this...
        psCam->ui32ExposureMin = (IMG_UINT32)floor(trow);
		psCam->dbExposureMin = trow;
		// should be set based on the coarse integration time x line lenght in us.
        psCam->ui32Exposure = psCam->ui32ExposureMin * coarsetime;
		
#if defined(ENABLE_AE_DEBUG_MSG)
        printf("===========================\n");
        printf("ref             = %f\n", ref);
        printf("pre_pll_clk_div = %d\n", pre_pll_clk_div);
        printf("pll_multiplier  = %d\n", pll_multiplier);
        printf("vt_sys_clk_div  = %d\n", vt_sys_clk_div);
        printf("vt_pix_clk_div  = %d\n", vt_pix_clk_div);
        printf("clk_pix         = %f\n", clk_pix);
        printf("trow            = %f\n", trow);
        printf("ui32ExposureMax = %d\n", psCam->ui32ExposureMax);
        printf("ui32ExposureMin = %d\n", psCam->ui32ExposureMin);
        printf("ui32Exposure    = %d\n", psCam->ui32Exposure);
        printf("===========================\n");
#endif

        psCam->flGain = 1.0;
        psCam->psSensorPhy->psGasket->bParallel = IMG_FALSE;
        psCam->psSensorPhy->psGasket->uiGasket= imgr;
        // note dodgy value used for hw workaround
        psCam->psSensorPhy->psGasket->uiWidth = ui16Width;
        psCam->psSensorPhy->psGasket->uiHeight = ui16Height-1;
        psCam->psSensorPhy->psGasket->bVSync = IMG_TRUE;
        psCam->psSensorPhy->psGasket->bHSync = IMG_TRUE;

        usleep(1000);
    }
    else
    {
        LOG_ERROR("failed to setup register %d/%d\n", n, 37*2);
    }
#if defined(INFOTM_ISP)
    psCam->fCurrentFps = asModes[ui16Mode].flFrameRate;
    psCam->fixedFps = psCam->fCurrentFps;
    psCam->initClk = framelenghtlines;
#endif
    return ret;
}

#ifdef INFOTM_ISP
static IMG_RESULT AR330MIPI_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{
	AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
    IMG_UINT16 aui16Regs[2];

    switch (flag)
    {
    	default:
    	case SENSOR_FLIP_NONE:
    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_NONE)
    		{
    			printf("the sensor had been SENSOR_FLIP_NONE.\n");
    			return IMG_SUCCESS;
    		}

    		aui16Regs[0] = 0x3040;
    		aui16Regs[1] = AR330MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] &= ~0xC000; //clear mirror(0x4000) & flip(0x8000) bit.

    		ar330mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    		break;

    	case SENSOR_FLIP_HORIZONTAL:
    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_HORIZONTAL)
    		{
    			printf("the sensor had been SENSOR_FLIP_HORIZONTAL.\n");
    			return IMG_SUCCESS;
    		}

    		aui16Regs[0] = 0x3040;
    		aui16Regs[1] = AR330MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] &= ~0xC000; //clear mirror(0x4000) & flip(0x8000) bit.
    		aui16Regs[1] |= 0x4000;

    		ar330mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_HORIZONTAL;
    		break;

    	case SENSOR_FLIP_VERTICAL:
    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_VERTICAL)
    		{
    			printf("the sensor had been SENSOR_FLIP_VERTICAL.\n");
    			return IMG_SUCCESS;
    		}

    		aui16Regs[0] = 0x3040;
    		aui16Regs[1] = AR330MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] &= ~0xC000; //clear mirror(0x4000) & flip(0x8000) bit.
    		aui16Regs[1] |= 0x8000;

    		ar330mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_VERTICAL;
    		break;

    	case SENSOR_FLIP_BOTH:
    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_BOTH)
    		{
    			printf("the sensor had been SENSOR_FLIP_BOTH.\n");
    			return IMG_SUCCESS;
    		}

    		aui16Regs[0] = 0x3040;
    		aui16Regs[1] = AR330MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] |= 0xC000; //set mirror(0x4000) & flip(0x8000) bit.

    		ar330mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
    		break;

    }
    	return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
	AR330MIPI_SetGain(hHandle, flGain, ui8Context);
    AR330MIPI_SetExposure(hHandle, ui32Exposure, ui8Context);

    return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
	if (pfixed != NULL)
	{
		AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);

		*pfixed = (int)psCam->fixedFps;
	}

	return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_ChangeRES(SENSOR_HANDLE hHandle, int imgW, int imgH)
{


	return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_GetSnapRes(SENSOR_HANDLE hHandle, reslist_t *presList)
{
	*presList = SnapRes;
	return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_SetFPS(SENSOR_HANDLE hHandle, double fps)
{
	AR330CAM_STRUCT *psCam = container_of(hHandle, AR330CAM_STRUCT, sFuncs);
	IMG_UINT16 aui16Regs[2];

    double realFps = 0.0;
    IMG_UINT32 frameTime = 0;
    IMG_UINT32 frameLine = 0;


    if (fps > psCam->fixedFps)
        realFps = psCam->fixedFps;
    else
        realFps = fps;
    frameLine = ((double)1 * 1000 * 1000) / (realFps * psCam->ui32ExposureMin);

    //printf("(%s,L%d) current fps =%.10f, fixFps=%d  realFps=%f \n", __func__, __LINE__, fps, psCam->fixedFps, realFps);

    //printf("(%s,l%d) frameLine=%d min=%d \n", __func__, __LINE__,frameLine, psCam->ui32ExposureMin);

    aui16Regs[0] = 0x300A;
    aui16Regs[1] = frameLine & 0xFFFF;
    ar330mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    psCam->fCurrentFps = realFps;

	return IMG_SUCCESS;
}

static IMG_RESULT AR330MIPI_Reset(SENSOR_HANDLE hHandle)
{
    char dev_nm[64];
    int fd = 0;
    int index = 0;
    int phy_enable = 0;
    int ret = 0;
    memset((void *)dev_nm, 0, sizeof(dev_nm));
    sprintf(dev_nm, "%s%d", DEV_PATH, index );
    fd = open(dev_nm, O_RDWR);
    if(fd <0)
    {
        LOG_ERROR("open %s error\n", dev_nm);
        return IMG_ERROR_FATAL;
    }

    LOG_ERROR("first: disable AR330 mipi camera\n");
    AR330MIPI_DisableSensor(hHandle);

    phy_enable = 0;
    ioctl(fd, SETPHYENABLE, &phy_enable);

    LOG_ERROR("second: enable AR330 mipi camera\n");
    // first enable phy
    // second enable sensor

    usleep(100000); // the delay time used for phy staus change deinit to init

    phy_enable = 1;
    ioctl(fd, SETPHYENABLE, &phy_enable);

    AR330MIPI_Enable(hHandle);

    close(fd);
    return ret;
}
#endif