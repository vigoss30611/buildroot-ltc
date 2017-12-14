/**
 ******************************************************************************
 @file IMX179.c

 @brief IMX179 camera implementation

 ******************************************************************************/
#include "sensors/imx179mipi.h"
#include "sensors/ddk_sensor_driver.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define MICROS 1000000

#define LOG_TAG "IMX179MIPI_SENSOR"

#define IMX179MIPI_WRITE_ADDR		(0x21 >> 1 )
#define IMX179MIPI_READ_ADDR			(0x20 >> 1 )
#define IMX179_SENSOR_VERSION 	"not-verified" // until read from sensor

#define MIN_PLUS_GAIN (double)8.0
#define MIN_DIGITAL_GAIN (double)8.01
#define MAX_GAIN (double)8.0


//#define LOG_DEBUG(...) printf(__VA_ARGS__)


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

static IMG_RESULT IMX179MIPI_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
static int IMX179MIPI_setDefFlipMirror(SENSOR_HANDLE hHandle, int mode, int flag);
static IMG_RESULT IMX179MIPI_SetFPS(SENSOR_HANDLE hHandle, double fps);
typedef struct _IMX179cam_struct
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
    IMG_UINT32 ui32LongTimeExposure;
    IMG_UINT8 ui8MipiLanes;
    double flGain;
    int i2c;
    SENSOR_PHY *psSensorPhy;
    IMG_UINT8 imager;
}IMX179CAM_STRUCT;

#define NUM_MODES 5
#define REGISTERS_PER_MODE 40 // fengwu

static IMG_UINT16 g_IMX179_ui32LinesBackup = 0;
static IMG_UINT8 g_IMX179_ui8GainValBackup = 0xff;


static SENSOR_MODE asModes[]=
{
    //{10, 3280, 2464,  30.0, 3280 , SENSOR_FLIP_BOTH},
    {10, 2464, 2464,  30.0, 2481, SENSOR_FLIP_BOTH},
    {10, 3280, 2464, 15.0, 3440, SENSOR_FLIP_BOTH},
    {10, 1640, 1232, 30.0, 2481, SENSOR_FLIP_BOTH},
    {10, 1920, 1080, 30.0, 1248, SENSOR_FLIP_BOTH},
    {10, 3264, 2448, 30.0, 2462, SENSOR_FLIP_BOTH},
	{10, 3264, 2448, 15.0, 3280, SENSOR_FLIP_BOTH},

};

#if 0
//30fps for Normal capture & ZSD
//5.1.2 FQPreview 3280x2464 30fps 24M MCLK 4lane 256Mbps/lane
static IMG_UINT16 ui16ModeRegs_0[] = {
	0x41C0, 0x01, 
	0x0104, 0x01, //group
	0x0100, 0x00, //STREAM OFF 
	//0x0101, 0x00,
	//0x0103, 0x01, //SW reset		 		   
	0x0202, 0x05,//0x09, // 0x0202, 0x09		 
	0x0203, 0x62,//0xcc, //0xAD, // (0x0203, 0xcc)
	//PLL setting
#if 0
	0x0301, 0x05,		 
	0x0303, 0x01,		 
	0x0305, 0x06,		 
	0x0309, 0x05,		 
	0x030B, 0x01,		 
	0x030C, 0x00,		 
	0x030D, 0xA1, // (0x030D, 0xA2)
#else
	0x0301, 0x05,		 
	0x0303, 0x01,		 
	0x0305, 0x06,		 
	0x0309, 0x05,		 
	0x030B, 0x01,		 
	0x030C, 0x00,		 
	0x030D, 0xA0, // (0x030D, 0xA2)
#endif
	0x030E, 0x01,

	0x0340, 0x09, // (0x0340, 0x09)
	0x0341, 0xB1,		 
	0x0342, 0x0D,		
	0x0343, 0x70, // (0x0343, 0x0D) 		 
	0x0344, 0x00,		 
	0x0345, 0x00,		 
	0x0346, 0x00,		 
	0x0347, 0x00, // (0x0347, 0x00)		 
	0x0348, 0x0C,		
	0x0349, 0xCF,		 
	0x034A, 0x09, // (0x034A, 0x09)		 
	0x034B, 0x9F, // (0x034B, 0x9F) 		 
	0x034C, 0x0C,		 
	0x034D, 0xD0,		 
	0x034E, 0x09,		 
	0x034F, 0xA0, // (0x034F, 0xD0)		 
	0x0383, 0x01,		 
	0x0387, 0x01,		
	0x0390, 0x00,		 
	0x0401, 0x00,		 
	0x0405, 0x10,		 
	0x3020, 0x10,		 
	0x3041, 0x15,		 
	0x3042, 0x87,		 
	0x3089, 0x4F,		 
	0x3309, 0x9A,		 
	0x3344, 0x57, // (0x3344, 0x57)		 
	0x3345, 0x1F,		 
	0x3362, 0x0A,		 
	0x3363, 0x0A,		 
	0x3364, 0x00,		
	0x3368, 0x18,		 
	0x3369, 0x00,		 
	0x3370, 0x77, // (0x3370, 0x77)		 	 
	0x3371, 0x2F, // (0x3371, 0x2F)             
	0x3372, 0x4F,            
	0x3373, 0x2F,            
	0x3374, 0x2F,            
	0x3375, 0x37,            
	0x3376, 0x9F,            
	0x3377, 0x37,            
	0x33C8, 0x00,            
	0x33D4, 0x0C,            
	0x33D5, 0xD0,            
	0x33D6, 0x09,            
	0x33D7, 0xA0,            
	0x4100, 0x0E,            
	0x4108, 0x01,            
	0x4109, 0x7C,
	0x3302, 0x01,
	0x0104, 0x00, // group
	//0x0100, 0x01, // STREAM ON	
};
#else
//video 3264x2464 30fps 24M MCLK 4lane MIPI speed 640
static IMG_UINT16 ui16ModeRegs_0[] = {
	0x0100,0x00,
	0x0101,0x00,
	0x0202,0x09,
	0x0203,0x9A,
	0x0301,0x05,
	0x0303,0x01,
	0x0305,0x06,
	0x0309,0x05,
	0x030B,0x01,
	0x030C,0x00,
	0x030D,0xA0,

    0x0340,0x09,
    0x0341,0xB1,

	0x0342,0x0D,
	0x0343,0x70,

	0x0344,0x01,
	0x0345,0x98,
	0x0346,0x00,
	0x0347,0x00,
	0x0348,0x0B,
	0x0349,0x37,
	0x034A,0x09,//0x09,
	0x034B,0x9F,//0x8F,//0x9F,

	0x034C,0x09,
	0x034D,0xA0,
	0x034E,0x09,//0x09,
	0x034F,0xA0,//0x90,//0x98,//0xA0,
	0x0383,0x01,
	0x0387,0x01,
	0x0390,0x00,
	0x0401,0x00,
	0x0405,0x10,
	0x3020,0x10,
	0x3041,0x15,
	0x3042,0x87,
	0x3089,0x4F,
	0x3309,0x9A,
	0x3344,0x57,
	0x3345,0x1F,
	0x3362,0x0A,
	0x3363,0x0A,
	0x3364,0x00,
	0x3368,0x18,
	0x3369,0x00,
	0x3370,0x77,
	0x3371,0x2F,
	0x3372,0x4F,
	0x3373,0x2F,
	0x3374,0x2F,
	0x3375,0x37,
	0x3376,0x9F,
	0x3377,0x37,
	0x33C8,0x00,

	0x33D4,0x0C,// SCL_SRC_X_SIZE
	0x33D5,0xC0,
	
	0x33D6,0x09, // SCL_SRC_Y_SIZE
	0x33D7,0xA0,
	
	0x4100,0x0E,
	0x4108,0x01,
	0x4109,0x7C,

};

#endif

///////////////imx179////////
//5.1.2 FQPreview 3280x2464 15fps 24M MCLK 4lane 256Mbps/lane
static IMG_UINT16 ui16ModeRegs_1[] = {
#if 1
	0x41C0, 0x01, 
	0x0104, 0x01, //group
	0x0100, 0x00, //STREAM OFF 
	//0x0101, 0x00,
	//0x0103, 0x01, //SW reset		 		   
	0x0202, 0x0A, // 0x0202, 0x09		 
	0x0203, 0xC4, // (0x0203, 0xcc)
	//PLL setting
	0x0301, 0x05,		 
	0x0303, 0x01,		 
	0x0305, 0x06,		 
	0x0309, 0x05,		 
	0x030B, 0x01,		 
	0x030C, 0x00,		 
	0x030D, 0xA0, // (0x030D, 0xA2)
	0x030E, 0x01,

	0x0340, 0x0A, // (0x0340, 0x09)
	0x0341, 0xC8,		 
	0x0342, 0x0D,		
	0x0343, 0x70, // (0x0343, 0x0D) 		 
	0x0344, 0x00,		 
	0x0345, 0x00,		 
	0x0346, 0x00,		 
	0x0347, 0x00, // (0x0347, 0x00)		 
	0x0348, 0x0C,		
	0x0349, 0xCF,		 
	0x034A, 0x09, // (0x034A, 0x09)		 
	0x034B, 0x9F, // (0x034B, 0x9F) 		 
	0x034C, 0x0C,		 
	0x034D, 0xD0,		 
	0x034E, 0x09,		 
	0x034F, 0xA0, // (0x034F, 0xD0)		 
	0x0383, 0x01,		 
	0x0387, 0x01,		
	0x0390, 0x00,		 
	0x0401, 0x00,		 
	0x0405, 0x10,		 
	0x3020, 0x10,		 
	0x3041, 0x15,		 
	0x3042, 0x87,		 
	0x3089, 0x4F,		 
	0x3309, 0x9A,		 
	0x3344, 0x57, // (0x3344, 0x57)		 
	0x3345, 0x1F,		 
	0x3362, 0x0A,		 
	0x3363, 0x0A,		 
	0x3364, 0x00,		
	0x3368, 0x18,		 
	0x3369, 0x00,		 
	0x3370, 0x77, // (0x3370, 0x77)		 	 
	0x3371, 0x2F, // (0x3371, 0x2F)             
	0x3372, 0x4F,            
	0x3373, 0x2F,            
	0x3374, 0x2F,            
	0x3375, 0x37,            
	0x3376, 0x9F,            
	0x3377, 0x37,            
	0x33C8, 0x00,            
	0x33D4, 0x0C,            
	0x33D5, 0xD0,            
	0x33D6, 0x09,            
	0x33D7, 0xA0,            
	0x4100, 0x0E,            
	0x4108, 0x01,            
	0x4109, 0x7C,
	0x3302, 0x01,
	0x0104, 0x00, // group
	//0x0100, 0x01, // STREAM ON	
#else
	0x41C0, 0x01, 
	0x0104, 0x01, //group
	0x0100, 0x00, 
	0x0101, 0x00, 
	0x0202, 0x0A, 
	0x0203, 0xC4, 
	0x0301, 0x0A, 
	0x0303, 0x01, 
	0x0305, 0x06, 
	0x0309, 0x0A, 
	0x030B, 0x01, 
	0x030C, 0x00, 
	0x030D, 0xB2, 
	0x0340, 0x0A, 
	0x0341, 0xC8, 
	0x0342, 0x0D, 
	0x0343, 0x70, 
	0x0344, 0x00, 
	0x0345, 0x08, 
	0x0346, 0x00, 
	0x0347, 0x08, 
	0x0348, 0x0C, 
	0x0349, 0xC7, 
	0x034A, 0x09, 
	0x034B, 0x97, 
	0x034C, 0x0C, 
	0x034D, 0xC0, 
	0x034E, 0x09, 
	0x034F, 0x90, 
	0x0383, 0x01, 
	0x0387, 0x01, 
	0x0390, 0x00, 
	0x0401, 0x00, 
	0x0405, 0x10, 
	0x3020, 0x10, 
	0x3041, 0x15, 
	0x3042, 0x87, 
	0x3089, 0x4F, 
	0x3309, 0x9A, 
	0x3344, 0x5F, 
	0x3345, 0x1F, 
	0x3362, 0x0A, 
	0x3363, 0x0A, 
	0x3364, 0x00,//0x02, 
	0x3368, 0x18, 
	0x3369, 0x00, 
	0x3370, 0x77, 
	0x3371, 0x2F, 
	0x3372, 0x5F, 
	0x3373, 0x37, 
	0x3374, 0x37, 
	0x3375, 0x37, 
	0x3376, 0xB7, 
	0x3377, 0x3F, 
	0x33C8, 0x00, 
	0x33D4, 0x0C, 
	0x33D5, 0xC0, 
	0x33D6, 0x09, 
	0x33D7, 0x90, 
	0x4100, 0x0E, 
	0x4108, 0x01, 
	0x4109, 0x7C, 
	0x3302, 0x01,
	0x0104, 0x00, // group
	//0x0100, 0x01, // STREAM ON
#endif
};

//5.1.2 FQPreview 1640x1232 30fps 24M MCLK 4lane 256Mbps/lane
static IMG_UINT16 ui16ModeRegs_2[] = {
#if 1
	0x41C0, 0x01,
	0x0104, 0x01,//group
	0x0100, 0x00,//STREAM OFF 
	0x0202, 0x09, //0x0202, 0x09,    integal time = 0x09AD = 2477
	0x0203, 0xAD,//(0x0203, 0xcc)
	//PLL setting
	0x0301, 0x05,	// vt_pix_clk_div 	 
	0x0303, 0x01,	// vt_sys_clk_div 	 
	0x0305, 0x06,   // pre_pll_clk_div	 
	0x0309, 0x05,		 
	0x030B, 0x01,		 
	0x030C, 0x00,		 
	0x030D, 0xA0, //(0x030D, 0xA2)
	0x030E, 0x01,
		
	0x0340, 0x09,//(0x0340, 0x09),  Frame_length = 0x09B1 = 2481
	0x0341, 0xB1,		 
	0x0342, 0x0D,//                   ,  Line_length = 0x0D70 = 3440
	0x0343, 0x70,//(0x0343, 0x0D)		 
	0x0344, 0x00,		 
	0x0345, 0x00,		 
	0x0346, 0x00,		 
	0x0347, 0x00,// (0x0347, 0x00)		 
	0x0348, 0x0C,//                     , X_add_End = 0x0CCF = 3279
	0x0349, 0xCF,		 
	0x034A, 0x09,// (0x034A, 0x09), Y_add_End = 0x099F = 2463
	0x034B, 0x9F,//(0x034B, 0x9F)		 
	0x034C, 0x06,//                     ,  X_Out_Size = 0x0668 = 1640
	0x034D, 0x68,		 
	0x034E, 0x04,//		    , Y_Out_Size = 0x04D0 = 1232
	0x034F, 0xD0,//(0x034F, 0xD0)		 
	0x0383, 0x01,		 
	0x0387, 0x01,		
	0x0390, 0x01,	// binning mode	 
	0x0401, 0x00,		 
	0x0405, 0x10,		 
	0x3020, 0x10,		 
	0x3041, 0x15,		 
	0x3042, 0x87,		 
	0x3089, 0x4F,		 
	0x3309, 0x9A,		 
	0x3344, 0x57,//(0x3344, 0x57)		 
	0x3345, 0x1F,		 
	0x3362, 0x0A,		 
	0x3363, 0x0A,		 
	0x3364, 0x00,		
	0x3368, 0x18,		 
	0x3369, 0x00,		 
	0x3370, 0x77,// (0x3370, 0x77)			 
	0x3371, 0x2F,//(0x3371, 0x2F)			  
	0x3372, 0x4F,			  
	0x3373, 0x2F,			  
	0x3374, 0x2F,			  
	0x3375, 0x37,			  
	0x3376, 0x9F,			  
	0x3377, 0x37,			  
	0x33C8, 0x00,			  
	0x33D4, 0x06,			  
	0x33D5, 0x68,			  
	0x33D6, 0x04,			  
	0x33D7, 0xD0,			  
	0x4100, 0x0E,			  
	0x4108, 0x01,			  
	0x4109, 0x7C,
	0x0104, 0x00,//group
	//0x0100, 0x01,//STREAM ON
#else
	0x41C0, 0x01,
	0x0104, 0x01,//group
	0x0100, 0x00,//STREAM OFF    
	0x0103, 0x01,//SW reset

	0x030E, 0x01, 		  
	0x0202, 0x09, //0x0202, 0x09		
	0x0203, 0xcc,//(0x0203, 0xcc)
	//PLL Setting
	0x0301, 0x05, 		
	0x0303, 0x01, 		
	0x0305, 0x06, 		
	0x0309, 0x05, 		
	0x030B, 0x01, 		
	0x030C, 0x00, 		
	0x030D, 0xA2, //(0x030D, 0xA2)
	0x030E, 0x01,

	0x0340, 0x09,//(0x0340, 0x09)
	0x0341, 0xD0, 		
	0x0342, 0x0D, 	   
	0x0343, 0x70,//(0x0343, 0x0D) 		
	0x0344, 0x00, 		
	0x0345, 0x00, 		
	0x0346, 0x00, 		
	0x0347, 0x00,// (0x0347, 0x00)		
	0x0348, 0x0C, 	   
	0x0349, 0xCF, 		
	0x034A, 0x09,// (0x034A, 0x09)		
	0x034B, 0x9F,//(0x034B, 0x9F) 		
	0x034C, 0x06, 		
	0x034D, 0x68, 		
	0x034E, 0x04, 		
	0x034F, 0xD0,//(0x034F, 0xD0) 		
	0x0383, 0x01, 		
	0x0387, 0x01, 	   
	0x0390, 0x01, 		
	0x0401, 0x00, 		
	0x0405, 0x10, 		
	0x3020, 0x10, 		
	0x3041, 0x15, 		
	0x3042, 0x87, 		
	0x3089, 0x4F, 		
	0x3309, 0x9A, 		
	0x3344, 0x57,//(0x3344, 0x57) 		
	0x3345, 0x1F, 		
	0x3362, 0x0A, 		
	0x3363, 0x0A, 		
	0x3364, 0x00, 	   
	0x3368, 0x18, 		
	0x3369, 0x00, 		
	0x3370, 0x77,// (0x3370, 0x77)			
	0x3371, 0x2F,//(0x3371, 0x2F) 			
	0x3372, 0x4F, 			
	0x3373, 0x2F, 			
	0x3374, 0x2F, 			
	0x3375, 0x37, 			
	0x3376, 0x9F, 			
	0x3377, 0x37, 			
	0x33C8, 0x00, 			
	0x33D4, 0x06, 			
	0x33D5, 0x68, 			
	0x33D6, 0x04, 			
	0x33D7, 0xD0, 			
	0x4100, 0x0E, 			
	0x4108, 0x01, 			
	0x4109, 0x7C,
	0x3302, 0x01,
	0x0104, 0x00,//group
	//0x0100, 0x01,//STREAM ON
#endif
};

//video 1920x1080 30fps 24M MCLK 4lane MIPI speed 648
#if 0
static IMG_UINT16 ui16ModeRegs_3[] = {
	0x41C0, 0x01,
	0x0104, 0x01,//group

	0x0100, 0x00,
	0x0101, 0x00,
	0x0202, 0x09,
	0x0203, 0xCC,
#if 0
	0x0301, 0x05,
	0x0303, 0x01,
	0x0305, 0x06,
	0x0309, 0x05,
	0x030B, 0x01,
	0x030C, 0x00,
	0x030D, 0xA2,
#else
	
	0x0301, 0x05,		 
	0x0303, 0x01,		 
	0x0305, 0x06,		 
	0x0309, 0x05,		 
	0x030B, 0x01,		 
	0x030C, 0x00,		 
	0x030D, 0xA0, //(0x030D, 0xA2)
	0x030E, 0x01,
#endif
	0x0340, 0x09,
	0x0341, 0xD0,
	0x0342, 0x0D,
	0x0343, 0x70,
	0x0344, 0x02,
	0x0345, 0xA8,
	0x0346, 0x02,
	0x0347, 0xB4,
	0x0348, 0x0A,
	0x0349, 0x27,
	0x034A, 0x06,
	0x034B, 0xEB,
	0x034C, 0x07,
	0x034D, 0x80,
	0x034E, 0x04,
	0x034F, 0x38,
	0x0383, 0x01,
	0x0387, 0x01,
	0x0390, 0x00,
	0x0401, 0x00,
	0x0405, 0x10,
	0x3020, 0x10,
	0x3041, 0x15,
	0x3042, 0x87,
	0x3089, 0x4F,
	0x3309, 0x9A,
	0x3344, 0x57,
	0x3345, 0x1F,
	0x3362, 0x0A,
	0x3363, 0x0A,
	0x3364, 0x00,
	0x3368, 0x18,
	0x3369, 0x00,
	0x3370, 0x77,
	0x3371, 0x2F,
	0x3372, 0x4F,
	0x3373, 0x2F,
	0x3374, 0x2F,
	0x3375, 0x37,
	0x3376, 0x9F,
	0x3377, 0x37,
	0x33C8, 0x00,
	0x33D4, 0x07,
	0x33D5, 0x80,
	0x33D6, 0x04,
	0x33D7, 0x38,
	0x4100, 0x0E,
	0x4108, 0x01,
	0x4109, 0x7C,
	0x0104, 0x00,//group
	//0x0100, 0x01,//STREAM ON
};
#else
//video 1920x1080 30fps 24M MCLK 4lane MIPI speed 480
static IMG_UINT16 ui16ModeRegs_3[] = {
	0x0100, 0x00,
	0x0101, 0x00,
	0x0202, 0x07,
	0x0203, 0x40,
	0x0301, 0x05,
	0x0303, 0x01,
	0x0305, 0x06,
	0x0309, 0x05,
	0x030B, 0x01,
	0x030C, 0x00,
	0x030D, 0x78,
	0x0340, 0x07,
	0x0341, 0x44,
	0x0342, 0x0D,
	0x0343, 0x70,
	0x0344, 0x02,
	0x0345, 0xA8,
	0x0346, 0x02,
	0x0347, 0xB4,
	0x0348, 0x0A,
	0x0349, 0x27,
	0x034A, 0x06,
	0x034B, 0xEB,
	0x034C, 0x07,
	0x034D, 0x80,
	0x034E, 0x04,
	0x034F, 0x38,
	0x0383, 0x01,
	0x0387, 0x01,
	0x0390, 0x00,
	0x0401, 0x00,
	0x0405, 0x10,
	0x3020, 0x10,
	0x3041, 0x15,
	0x3042, 0x87,
	0x3089, 0x4F,
	0x3309, 0x9A,
	0x3344, 0x47,
	0x3345, 0x1F,
	0x3362, 0x0A,
	0x3363, 0x0A,
	0x3364, 0x00,
	0x3368, 0x18,
	0x3369, 0x00,
	0x3370, 0x67,
	0x3371, 0x1F,
	0x3372, 0x47,
	0x3373, 0x27,
	0x3374, 0x1F,
	0x3375, 0x1F,
	0x3376, 0x7F,
	0x3377, 0x2F,
	0x33C8, 0x00,
	0x33D4, 0x07,
	0x33D5, 0x80,
	0x33D6, 0x04,
	0x33D7, 0x38,
	0x4100, 0x0E,
	0x4108, 0x01,
	0x4109, 0x7C,
};

#endif
/////////////////////////////

//video 3264x2448 30fps 24M MCLK 4lane MIPI speed 640
static IMG_UINT16 ui16ModeRegs_4[] = {
	0x0100,0x00,
	0x0101,0x00,
	0x0202,0x09,
	0x0203,0x9A,
	0x0301,0x05,
	0x0303,0x01,
	0x0305,0x06,
	0x0309,0x05,
	0x030B,0x01,
	0x030C,0x00,
	0x030D,0xA0,
	0x0340,0x09,
	0x0341,0x9E,
	0x0342,0x0D,
	0x0343,0x70,
	0x0344,0x00,
	0x0345,0x08,
	0x0346,0x00,
	0x0347,0x08,
	0x0348,0x0C,
	0x0349,0xC7,
	0x034A,0x09,
	0x034B,0x97,
	0x034C,0x0C,
	0x034D,0xC0,
	0x034E,0x09,
	0x034F,0x90,
	0x0383,0x01,
	0x0387,0x01,
	0x0390,0x00,
	0x0401,0x00,
	0x0405,0x10,
	0x3020,0x10,
	0x3041,0x15,
	0x3042,0x87,
	0x3089,0x4F,
	0x3309,0x9A,
	0x3344,0x57,
	0x3345,0x1F,
	0x3362,0x0A,
	0x3363,0x0A,
	0x3364,0x00,
	0x3368,0x18,
	0x3369,0x00,
	0x3370,0x77,
	0x3371,0x2F,
	0x3372,0x4F,
	0x3373,0x2F,
	0x3374,0x2F,
	0x3375,0x37,
	0x3376,0x9F,
	0x3377,0x37,
	0x33C8,0x00,
	0x33D4,0x0C,
	0x33D5,0xC0,
	0x33D6,0x09,
	0x33D7,0x90,
	0x4100,0x0E,
	0x4108,0x01,
	0x4109,0x7C,

};

//video 3264x2448 15fps 24M MCLK 4lane MIPI speed 480
static IMG_UINT16 ui16ModeRegs_5[] = {
	0x0100, 0x00,
	0x0101, 0x00,
	0x0202, 0x0E,
	0x0203, 0x85,
	0x0301, 0x05,
	0x0303, 0x01,
	0x0305, 0x06,
	0x0309, 0x05,
	0x030B, 0x01,
	0x030C, 0x00,
	0x030D, 0xF0,
	0x0340, 0x0E,
	0x0341, 0x89,
	0x0342, 0x0D,
	0x0343, 0x70,
	0x0344, 0x00,
	0x0345, 0x08,
	0x0346, 0x00,
	0x0347, 0x08,
	0x0348, 0x0C,
	0x0349, 0xC7,
	0x034A, 0x09,
	0x034B, 0x97,
	0x034C, 0x0C,
	0x034D, 0xC0,
	0x034E, 0x09,
	0x034F, 0x90,
	0x0383, 0x01,
	0x0387, 0x01,
	0x0390, 0x00,
	0x0401, 0x00,
	0x0405, 0x10,
	0x3020, 0x10,
	0x3041, 0x15,
	0x3042, 0x87,
	0x3089, 0x4F,
	0x3309, 0x9A,
	0x3344, 0x47,
	0x3345, 0x1F,
	0x3362, 0x0A,
	0x3363, 0x0A,
	0x3364, 0x00,
	0x3368, 0x0C,
	0x3369, 0x00,
	0x3370, 0x67,
	0x3371, 0x1F,
	0x3372, 0x47,
	0x3373, 0x27,
	0x3374, 0x1F,
	0x3375, 0x1F,
	0x3376, 0x7F,
	0x3377, 0x2F,
	0x33C8, 0x00,
	0x33D4, 0x0C,
	0x33D5, 0xC0,
	0x33D6, 0x09,
	0x33D7, 0x90,
	0x4100, 0x0E,
	0x4108, 0x01,
	0x4109, 0x7C,
};

static IMG_RESULT doStaticSetup(IMX179CAM_STRUCT *psCam, IMG_INT16 ui16Width,
        IMG_UINT16 ui16Height, IMG_UINT16 ui16Mode, IMG_BOOL bFlipHorizontal,
        IMG_BOOL bFlipVertical);

#ifdef INFOTM_ISP
static IMG_RESULT IMX179MIPI_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag);
#endif //INFOTM_ISP
static int IMX179MIPI_I2C_WRITE(int i2c,  IMG_UINT16 reg, IMG_UINT16 val)
{
#if 1
    struct i2c_rdwr_ioctl_data packets; 
    struct i2c_msg messages[1];

    uint8_t buf[3];
    buf[0] = (uint8_t)((reg >> 8) & 0xff);
    buf[1] = (uint8_t)(reg & 0xff);
    buf[2] = (uint8_t)(val);

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
#else
    size_t i;
    IMG_UINT8 buf[3];
    IMG_UINT8 *buf_p = buf;

    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE_FORCE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave address!\n");
        return IMG_ERROR_BUSY;
    }

    /* Prepare 8-bit data using 16-bit input data. */
    *buf_p++ = reg[0] >> 8;
    *buf_p++ = reg[1] & 0xff;
	*buf_p++ = val;
	
    write(i2c, buf, 3);
	
#endif
    return 0;  
}  

static int IMX179MIPI_I2C_WRITE_16Val(int i2c,  IMG_UINT16 reg, IMG_UINT16 val)
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

    return 0;  
}

static uint16_t IMX179MIPI_I2C_READ_16Val(int i2c, IMG_UINT16 reg)
{
    uint8_t buf[2], val[2] = {0, 0};

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
    messages[1].len   = sizeof(val);  
    messages[1].buf   = &val;  

    packets.msgs      = messages;  
    packets.nmsgs     = 2;  

    if(ioctl(i2c, I2C_RDWR, &packets) < 0) {
        LOG_ERROR("Unable to read data: 0x%04x.\n", reg);  
        return 1;  
    }

    //usleep(5);

//    LOG_DEBUG("RD:0x%04x 0x%04x\n", reg, buf[0]);
    return (uint16_t)((val[0]<<8)|val[1]);
}


static uint8_t IMX179MIPI_I2C_READ(int i2c, IMG_UINT16 reg)
{
    uint8_t buf[2], val = 0;

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
    messages[1].len   = sizeof(val);  
    messages[1].buf   = &val;  

    packets.msgs      = messages;  
    packets.nmsgs     = 2;  

    if(ioctl(i2c, I2C_RDWR, &packets) < 0) {  
        LOG_ERROR("Unable to read data: 0x%04x.\n", reg);  
        return 1;  
    }

    //usleep(5);

    LOG_DEBUG("RD:0x%04x 0x%04x\n", reg, buf[0]);
    return val;  

}
#if 1

static IMG_RESULT IMX179mipi_i2c_write16(int i2c, IMG_UINT16 *data, size_t len)
{
#ifdef INFOTM_ISP
    IMX179MIPI_I2C_WRITE(i2c, data[0], data[1]);
#else
#if !file_i2c
    size_t i;
    IMG_UINT8 buf[len * sizeof(*data)];
    IMG_UINT8 *buf_p = buf;

    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE_FORCE, i2c_addr))
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
static IMG_RESULT IMX179MIPI_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
//    int g = 0;
//    CI_HWINFO *pInfo = NULL;
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
    /*
       if (!psCam->psSensorPhy)
       {
       LOG_ERROR("sensor not initialised\n");
       return IMG_ERROR_NOT_INITIALISED;
       }*/
/*
    if (psCam->bIsMIPI)
        IMG_ASSERT(strlen(IMX179MIPI_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    else
        IMG_ASSERT(strlen(IMX179MIPI_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(IMX179_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);
*/
    //pInfo = &(psCam->psSensorPhy->psConnection->sHWInfo);

//    psInfo->eBayerOriginal = MOSAIC_GRBG;
//    psInfo->eBayerEnabled = MOSAIC_GRBG;
    psInfo->eBayerOriginal = MOSAIC_RGGB;
    psInfo->eBayerEnabled = MOSAIC_RGGB;


    // because it seems IMX179 conserves same pattern - otherwise uncomment
    /*if (SENSOR_FLIP_NONE!=psInfo->sStatus.ui8Flipping)
      {
      psInfo->eBayerEnabled = MosaicFlip(psInfo->eBayerOriginal,
      psInfo->sStatus.ui8Flipping&SENSOR_FLIP_HORIZONTAL?1:0,
      psInfo->sStatus.ui8Flipping&SENSOR_FLIP_VERTICAL?1:0);
      }*/

    //sprintf(psInfo->pszSensorName, IMX179MIPI_SENSOR_INFO_NAME);
    sprintf(psInfo->pszSensorName, IMX179_SENSOR_VERSION); /// @ could we read that from sensor?
    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 10000;//7500;
    //psInfo->ui8BitDepth = 10;
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = 0;
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

static IMG_RESULT IMX179MIPI_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes)
{
	//IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
    if(nIndex < sizeof(asModes) / sizeof(SENSOR_MODE))
    {
		IMG_MEMCPY(psModes, &(asModes[nIndex]), sizeof(SENSOR_MODE));
        return IMG_SUCCESS;
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

static IMG_RESULT IMX179MIPI_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
{
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

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

static IMG_RESULT IMX179MIPI_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Mode,
        IMG_UINT8 ui8Flipping)
{
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(ui16Mode < (sizeof(asModes) / sizeof(SENSOR_MODE)))
    {
        psCam->ui16CurrentMode = ui16Mode;
#if 0
		if((ui8Flipping&(asModes[ui16Mode].ui8SupportFlipping))!=ui8Flipping)
        {
            LOG_ERROR("sensor mode does not support selected flipping 0x%x (supports 0x%x)\n", 
                    ui8Flipping, asModes[ui16Mode].ui8SupportFlipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }
#else
		asModes[ui16Mode].ui8SupportFlipping = ui8Flipping;
#endif
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

static IMG_RESULT IMX179MIPI_Enable(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    IMG_RESULT ret;
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling IMX179 Camera!\n");
    psCam->bEnabled = IMG_TRUE;
    aui16Regs[0] = 0x0100; // RESET_REGISTER
    aui16Regs[1] = 0x01;
    // setup the sensor number differently when using multiple sensors
    ret = SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->ui8MipiLanes, 0);
	//usleep(1000);
    if (ret == IMG_SUCCESS)
    {
#if 1
        ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to enable sensor\n");
        }
        else
        {
            LOG_DEBUG("camera enabled\n");
        }
#endif
		g_IMX179_ui32LinesBackup = 0;
		g_IMX179_ui8GainValBackup = 0xff;
    }
	usleep(10000);
    return ret;
}

static IMG_RESULT IMX179MIPI_DisableSensor(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
    IMG_RESULT ret = IMG_SUCCESS;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Disabling IMX179 camera\n");
    psCam->bEnabled = IMG_FALSE;
    // if we are generating a file we don't want to disable!
    aui16Regs[0] = 0x0100; // RESET_REGISTER
    aui16Regs[1] = 0x00;
#if 1
    ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to disable sensor!\n");
    }
#endif
    usleep(10*1000);
    SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, 0);
    return ret;
}

static IMG_RESULT IMX179MIPI_DestroySensor(SENSOR_HANDLE hHandle)
{
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Destroying IMX179 camera\n");
    // remember to free anything that might have been allocated dynamically (like extended params...)
    if(psCam->bEnabled)
        IMX179MIPI_DisableSensor(hHandle);
    SensorPhyDeinit(psCam->psSensorPhy);

    close(psCam->i2c);

    IMG_FREE(psCam);
    return IMG_SUCCESS;
}


static IMG_RESULT IMX179MIPI_ConfigureFlash(SENSOR_HANDLE hHandle,
        IMG_BOOL bAlwaysOn, IMG_INT16 i16FrameDelay, IMG_INT16 i16Frames,
        IMG_UINT16 ui16FlashPulseWidth)
{
    IMG_UINT16 aui16Regs[4];
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
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
#if 0
    aui16Regs[0] = 0x3046; // Global Gain register
    aui16Regs[1] = (IMG_UINT16)(bAlwaysOn?((1<<8)|(7<<3)):(i16Frames<<3)|i16FrameDelay);
    //ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    aui16Regs[0] = 0x3048; // Global Gain register
    aui16Regs[1] = ui16FlashPulseWidth;
    ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#if defined(ENABLE_AE_DEBUG_MSG)
    printf("IMX179MIPI_ConfigureFlash Change Global Gain 0x3046 = 0x%04X\n", aui16Regs[1]);
#endif
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to configure flash!\n");
    }
#endif
    return ret;
}

static IMG_RESULT IMX179MIPI_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current = psCam->ui32Exposure;
    return IMG_SUCCESS;
}

static IMG_RESULT IMX179MIPI_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Current,
        IMG_UINT8 ui8Context)
{
//	static IMG_INT16 framelength = 2481;//2592;//0x09B1; // ??? need change
	IMG_RESULT ret = IMG_SUCCESS;
#if !defined(INFOTM_DISABLE_AE)
	IMG_INT32 i32Lines;
	IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

	if (!psCam->psSensorPhy)
	{
		LOG_ERROR("sensor not initialised\n");
		return IMG_ERROR_NOT_INITIALISED;
	}

	//sui32Current = 20000;
	psCam->ui32Exposure=ui32Current;

	//ui32Lines = (psCam->ui32ExposureMax - psCam->ui32Exposure) / psCam->ui32ExposureMin;
        //i32Lines = asModes[psCam->ui16CurrentMode].ui16VerticalTotal - (psCam->ui32Exposure / psCam->ui32ExposureMin);
        i32Lines = psCam->ui32Exposure / psCam->dbExposureMin;
        //printf("====IMX179MIPI_SetExposure time(%d), lines(%d)====\n", ui32Current, i32Lines);
        
	if (g_IMX179_ui32LinesBackup == i32Lines) {
		return IMG_SUCCESS;
	}

#if defined(IQ_TUNING)
        if(i32Lines <= psCam->ui32LongTimeExposure)
        {
        // Normal Exposure
        // exp_line <= 2592 => normal exposure => 30fps
        // corase integ time = exposure line
        }
        else if(i32Lines >  psCam->ui32LongTimeExposure ) //&& (i32Lines <= )
        {
        // Long time Exposure
        // exp_line > 2592 => long time exposure => small than 30fps
        // corase integ time = exposure line
        // frame length = exposure line +4
            IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0340, (( i32Lines + 4) >> 8)); 
            IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0341, ( i32Lines + 4) & 0x00ff);
        }
        else
        {
            LOG_WARNING("Func : SetExposure() ==> (%d)ExpTime, (%d)Lines, (%d)ExpTimeMin/Line\n", psCam->ui32Exposure, i32Lines, psCam->dbExposureMin);
        }
#endif

	g_IMX179_ui32LinesBackup = i32Lines;

#if defined(ENABLE_AE_DEBUG_MSG)
	//LOG_DEBUG("Exposure Val %x\n", i32Lines);
	printf("IMX179_SetExposure lines = 0x%08X (Exposure %d)\n", i32Lines, psCam->ui32Exposure);
#endif

	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x01); 
	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0202, ((i32Lines) >> 8)); 
	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0203, (i32Lines) & 0xff);  
	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x00);
#endif
    return ret;
}

static IMG_RESULT IMX179MIPI_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
        double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

      if (!psCam->psSensorPhy)
      {
      LOG_ERROR("sensor not initialised\n");
      return IMG_ERROR_NOT_INITIALISED;
      }*/

    *pflMin = 1.0;// / 128;
    *pflMax = MAX_GAIN;//15.0;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

static IMG_RESULT IMX179MIPI_GetGain(SENSOR_HANDLE hHandle,
        double *pflGain, IMG_UINT8 ui8Context)
{
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

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
    unsigned short ui8GainRegVal;
}ARGAINTABLE;

//#define MaxGainIndex (97)
static ARGAINTABLE asIMX179GainValues[]=
{
	1.00,  0,
	1.05, 12,
	1.10, 23,
	1.15, 33,
	1.20, 42,
	1.25, 52,
	1.30, 59,
	1.35, 66,
	1.40, 73,
	1.45, 79,
	1.50, 85,
	1.55, 91,
	1.60, 96,
	1.65, 101,
	1.70, 105,
	1.75, 110,
	1.80, 114,
	1.86, 118,
	1.90, 121,
	1.95, 125,
	2.00, 128,
	2.05, 131,
	2.10, 134,
	2.15, 137,
	2.19, 139,
	2.25, 142,
	2.31, 145,
	2.35, 147,
	2.39, 149, 
	2.45, 151,
	2.49, 153, 	 
	2.56, 156,
	2.61, 158,
	2.64, 159,
	2.69, 161,
	2.75, 163,
	2.81, 165, 
	2.84, 166,
	2.91, 168,
	2.94, 169,
	3.01, 171,
	3.05, 172,
	3.12, 174,
	3.16, 175, 
	3.20, 176,
	3.24, 177, 
	3.32, 179, 
	3.37, 180,  
	3.41, 181,
	3.46, 182,
	3.51, 183,  
	3.56, 184,
	3.61, 185,
	3.66, 186,
	3.71, 187,
	3.76, 188,
	3.82, 189,
	3.88,190,
	3.94,191,
	4.00,192, 
	4.06,193,
	4.13,194,
	4.20,195,
	4.27,196,
	4.34,197,
	4.41,198,
	4.49,199,
	4.57,200,
	4.65,201,
	4.74,202,
	4.83,203,
	4.92,204,
	5.02,205,
	5.12,206,
	5.22,207,
	5.33,208,
	5.45,209,
	5.57,210,
	5.69,211,
	5.82,212, 
	5.95,213,
	6.10,214, 	 
	6.24,215,
	6.40,216,
	6.56,217, 	 
	6.74,218,
	6.92,219,
	7.11,220,
	7.31,221,
	7.53,222, 	 
	7.76,223,
	8.00,224,
	// dight gain
	8.01, 0x0103,
	8.02, 0x0106,
	8.04, 0x0109,
	8.05, 0x010C,
	8.06, 0x010F,
	8.07, 0x0112,
	8.08, 0x0115,
	8.1, 0x0119,
/*	
	8.11, 0x011C,
	8.12, 0x011F,
	8.14, 0x0123,
	8.15, 0x0126,
	8.16, 0x0129,
	8.18, 0x012D,
	8.19, 0x0130,
	8.2, 0x0134,
	8.21, 0x0137,
	8.23, 0x013B,
	8.25, 0x013F,
	8.26, 0x0142,
	8.27, 0x0146,
	8.29, 0x014A,
	8.3, 0x014E,
	8.32, 0x0151,
	8.33, 0x0155,
	8.35, 0x0159,
	8.36, 0x015D,
	8.38, 0x0161,
	8.39, 0x0165,
	8.41, 0x016A,
	8.43, 0x016E,
	8.45, 0x0172,
	8.46, 0x0176,
	8.48, 0x017B,
	8.5, 0x017F,
	8.51, 0x0183,
	8.53, 0x0188,
	8.55, 0x018C,
	8.57, 0x0191,
	8.59, 0x0196,
	8.6, 0x019A,
	8.62, 0x019F,
	8.64, 0x01A4,
	8.66, 0x01A9,
	8.68, 0x01AE,
	8.7, 0x01B3,
	8.72, 0x01B8,
	8.74, 0x01BD,
	8.76, 0x01C2,
	8.78, 0x01C7,
	8.8, 0x01CD,
	8.82, 0x01D2,
	8.84, 0x01D7,
	8.86, 0x01DD,
	8.88, 0x01E2,
	8.91, 0x01E8,
	8.93, 0x01ED,
	8.95, 0x01F3,
	8.97, 0x01F9,
	9, 0x01FF,
	9.02, 0x0205,
	9.04, 0x020B,
	9.07, 0x0211,
	9.09, 0x0217,
	9.11, 0x021D,
	9.14, 0x0223,
	9.16, 0x022A,
	9.19, 0x0230,
	9.21, 0x0237,
	9.24, 0x023D,
	9.27, 0x0244,
	9.29, 0x024A,
	9.32, 0x0251,
	9.34, 0x0258,
	9.37, 0x025F,
	9.4, 0x0266,
	9.43, 0x026D,
	9.45, 0x0274,
	9.48, 0x027C,
	9.51, 0x0283,
	9.54, 0x028A,
	9.57, 0x0292,
	9.6, 0x029A,
	9.63, 0x02A1,
	9.66, 0x02A9,
	9.69, 0x02B1,
	9.72, 0x02B9,
	9.75, 0x02C1,
	9.79, 0x02C9,
	9.82, 0x02D2,
	9.85, 0x02DA,
	9.88, 0x02E2,
	9.92, 0x02EB,
	9.95, 0x02F4,
	9.98, 0x02FC,
	10.02, 0x0305,

	10.05, 0x030E,
	10.09, 0x0317,
	10.13, 0x0320,
	10.16, 0x032A,
	10.2, 0x0333,
	10.23, 0x033C,
	10.27, 0x0346,
	10.31, 0x0350,
	10.35, 0x035A,
	10.39, 0x0363,
	10.43, 0x036D,
	10.47, 0x0378,
	10.51, 0x0382,
	10.55, 0x038C,
	10.59, 0x0397,
	10.63, 0x03A1,
	10.67, 0x03AC,
	10.71, 0x03B7,
	10.76, 0x03C2,
	10.8, 0x03CD,
	10.85, 0x03D9,
	10.89, 0x03E4,
	10.93, 0x03EF,
	10.98, 0x03FB,
	11.03, 0x0407,
	11.07, 0x0413,
	11.12, 0x041F,
	11.17, 0x042B,
	11.22, 0x0438,
	11.27, 0x0444,
	11.32, 0x0451,
	11.36, 0x045D,
	11.41, 0x046A,
	11.47, 0x0478,
	11.52, 0x0485,
	11.57, 0x0492,
	11.63, 0x04A0,
	11.68, 0x04AD,
	11.73, 0x04BB,
	11.79, 0x04C9,
	11.84, 0x04D7,
	11.9, 0x04E6,
	11.95, 0x04F4,
	12.01, 0x0503,
	12.07, 0x0512,
	12.13, 0x0521,
	12.19, 0x0530,
	12.25, 0x0540,
	12.31, 0x054F,
	12.37, 0x055F,
	12.43, 0x056F,
	12.5, 0x057F,
	12.56, 0x058F,
	12.63, 0x05A0,
	12.69, 0x05B0,
	12.75, 0x05C1,
	12.82, 0x05D2,
	12.89, 0x05E3,
	12.96, 0x05F5,
	13.03, 0x0607,
	13.09, 0x0618,
	13.16, 0x062A,
	13.24, 0x063D,
	13.31, 0x064F,
	13.38, 0x0662,
	13.46, 0x0675,
	13.53, 0x0688,
	13.61, 0x069B,
	13.68, 0x06AF,
	13.76, 0x06C3,
	13.84, 0x06D7,
	13.92, 0x06EB,
	14, 0x0700,
	14.08, 0x0714,
	14.16, 0x0729,
	14.25, 0x073F,
	14.33, 0x0754,
	14.41, 0x076A,
	14.5, 0x0780,
	14.59, 0x0796,
	14.67, 0x07AC,
	14.76, 0x07C3,
	14.85, 0x07DA,
	14.94, 0x07F1,
	15.04, 0x0809,
	15.13, 0x0821,
	15.22, 0x0839,
	15.32, 0x0851,
	15.41, 0x086A,
	15.51, 0x0883,
	15.61, 0x089C,
	15.71, 0x08B6,
	15.81, 0x08CF,
	15.91, 0x08EA,
	16.02, 0x0904,
	16.12, 0x091F,
	16.23, 0x093A,
	16.33, 0x0955,
	16.44, 0x0971,
	16.55, 0x098D,
	16.66, 0x09A9,
	16.77, 0x09C6,
	16.89, 0x09E3,
	17, 0x0A00,
*/
};

static IMG_RESULT IMX179MIPI_SetGain(SENSOR_HANDLE hHandle, double flGain,
        IMG_UINT8 ui8Context)
{
    IMG_UINT16 aui16Regs[4];
    unsigned int nIndex = 0;
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
	
#if !defined(INFOTM_DISABLE_AE)
    psCam->flGain = flGain;

    while(nIndex < (sizeof(asIMX179GainValues) / sizeof(ARGAINTABLE)) - 1)
    {
        if(asIMX179GainValues[nIndex].flGain > flGain)
            break;
        nIndex++;
    }

    if(nIndex > 0)
        nIndex = nIndex - 1;

    flGain = psCam->flGain / asIMX179GainValues[nIndex].flGain;

	if (psCam->flGain > MIN_PLUS_GAIN)
	{
		//printf("====>skip IMX179MIPI_SetGain...\n");
		return IMG_SUCCESS;
	}

	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x01);

	if (psCam->flGain < MIN_DIGITAL_GAIN)
	{
	    aui16Regs[0] = 0x0204;
	    aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);

		aui16Regs[0] = 0x0205;
	    aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
	}
	else
	{
	    aui16Regs[0] = 0x020E;
	    aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);

		aui16Regs[0] = 0x020F;
	    aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);	

		aui16Regs[0] = 0x0210;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
		
		aui16Regs[0] = 0x0211;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]); 

		aui16Regs[0] = 0x0212;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
		
		aui16Regs[0] = 0x0213;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]); 

		aui16Regs[0] = 0x0214;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
		
		aui16Regs[0] = 0x0215;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]); 
	}
	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x00);
	
#if defined(ENABLE_AE_DEBUG_MSG)
    printf("====>IMX179MIPI_SetGain Change Analog Gain 0x3060 = 0x%04X\n", asIMX179GainValues[nIndex].ui8GainRegVal);
#endif
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set analogue gain register\n");
    }
#endif
    return IMG_SUCCESS;
}

static IMG_RESULT IMX179MIPI_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
        IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

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

//int IMX179_Create_MIPI(SENSOR_HANDLE *phHandle)
IMG_RESULT IMX179_Create_MIPI(struct _Sensor_Functions_ **hHandle,int index)
{
    IMX179CAM_STRUCT *psCam;
    
    IMG_UINT8 aui8Regs[3];
    char i2c_dev_path[NAME_MAX];
    IMG_UINT16 chipV;
    IMG_RESULT ret;

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
    printf("====>%s opened OK, i2c-addr=0x%x, chn = %d, imgr = %d\n", dev_nm, i2c_addr, chn, imgr);
    sprintf(adaptor, "%s-%d", "i2c", chn);
    memset((void *)extra_cfg, 0, sizeof(dev_nm));

    if (path[0] == 0) {
        sprintf(extra_cfg, "%s%s%d-config.txt", EXTRA_CFG_PATH, "sensor" , index );
    } else {
        strcpy(extra_cfg, path);
    }
#endif

	psCam = (IMX179CAM_STRUCT *)IMG_CALLOC(1, sizeof(IMX179CAM_STRUCT));
	if (!psCam)
		return IMG_ERROR_MALLOC_FAILED;

	*hHandle = &psCam->sFuncs;
	psCam->sFuncs.GetMode = IMX179MIPI_GetMode;
	psCam->sFuncs.GetState = IMX179MIPI_GetState;
    psCam->sFuncs.SetMode = IMX179MIPI_SetMode;
    psCam->sFuncs.Enable = IMX179MIPI_Enable;
    psCam->sFuncs.Disable = IMX179MIPI_DisableSensor;
    psCam->sFuncs.Destroy = IMX179MIPI_DestroySensor;

    psCam->sFuncs.GetGainRange = IMX179MIPI_GetGainRange;
    psCam->sFuncs.GetCurrentGain = IMX179MIPI_GetGain;
    psCam->sFuncs.SetGain = IMX179MIPI_SetGain;

    psCam->sFuncs.SetGainAndExposure = IMX179MIPI_SetGainAndExposure;

    psCam->sFuncs.GetExposureRange = IMX179MIPI_GetExposureRange;
    psCam->sFuncs.GetExposure = IMX179MIPI_GetExposure;
    psCam->sFuncs.SetExposure = IMX179MIPI_SetExposure;

    psCam->sFuncs.GetInfo = IMX179MIPI_GetInfo;

#ifdef INFOTM_ISP
    psCam->sFuncs.SetFlipMirror = IMX179MIPI_SetFlipMirror;
    psCam->sFuncs.SetFPS = IMX179MIPI_SetFPS;
#endif //INFOTM_ISP
//    psCam->sFuncs.GetExtendedParamInfo = IMX179MIPI_GetExtendedParamInfo;
//    psCam->sFuncs.GetExtendedParamValue = IMX179MIPI_GetExtendedParamValue;
//    psCam->sFuncs.SetExtendedParamValue = IMX179MIPI_SetExtendedParamValue;

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
        LOG_ERROR("Failed to open I2C device: \"%s\", err = %d\n", i2c_dev_path, psCam->i2c);
        IMG_FREE(psCam);
        *hHandle=NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    psCam->psSensorPhy = SensorPhyInit();

    return IMG_SUCCESS;
}

#define CASE_MODE(N) \
case N:\
    registerArray = ui16ModeRegs_##N;\
    nRegisters = sizeof(ui16ModeRegs_##N)/(2*sizeof(IMG_UINT16)); \
    break;

IMG_UINT16* IMX179MIPI_GetRegisters(int mode, int* cnt)
{
	IMG_UINT16* registerArray;
	int nRegisters;
	switch(mode)
    {
        CASE_MODE(0)
        CASE_MODE(1)
        CASE_MODE(2)  
		CASE_MODE(3)
		CASE_MODE(4)
		CASE_MODE(5)
        default:
            LOG_ERROR("unknown mode %d\n", mode);
            return NULL;
    }
	*cnt = nRegisters;
	return registerArray;
}

static IMG_RESULT doStaticSetup(IMX179CAM_STRUCT *psCam, IMG_INT16 ui16Width,
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
        default:
            LOG_ERROR("unknown mode %d\n", ui16Mode);
            return IMG_ERROR_FATAL;
    }

#if 1 // fix sensor blk val to 0
{
	IMG_UINT16 tmpVal = IMX179MIPI_I2C_READ_16Val(psCam->i2c, 0x30E5);
	tmpVal |= 0x0800;
    IMX179MIPI_I2C_WRITE_16Val(psCam->i2c, 0x30E5, tmpVal);
	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x300B, 0x0);
}
#endif

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


                    if (0 == strncmp(pstr, "0x", 2)) {
                        val[i] = strtoul(pstr, NULL, 0);
                        i++;
                    }

                    pstr = strtok(NULL, ",");

                }
               // printf("val[0] = 0x%x, val[1]=0%x,\n", val[0], val[1]);
                ret = IMX179MIPI_I2C_WRITE_16Val(psCam->i2c, val[0], val[1]);
                if (ret != IMG_SUCCESS)
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
            if (val[0] == 0x0202)
            {
            // store the coarse integration time, so we can report it correctly
                coarsetime = val[1];
            }
            if (val[0] == 0x0203)
            {
                coarsetime = (coarsetime << 8) | val[1];
            }
            if (val[0] == 0x3364)
            {
                psCam->ui8MipiLanes = val[1] ? 2:4;
            }

            if (val[0] == 0x0340)
            {
                framelenghtlines = val[1];
            }
            if (val[0] == 0x0341)
            {
                framelenghtlines = (framelenghtlines << 8) | val[1];
            }



            if (val[0] == 0x0342)
            // store the coarse line_length_pck time
                line_length_pck = val[1];
            if (val[0] == 0x0343)
                line_length_pck = (line_length_pck << 8) | val[1];
            if (val[0] == 0x0301)
            // store the coarse vt_pix_clk_div time
                vt_pix_clk_div = (val[1] & 0x000f);
            if (val[0] == 0x0303)
            // store the coarse vt_sys_clk_div time
                vt_sys_clk_div = (val[1] & 0x0003);
            if (val[0] == 0x030c)
                pll_multiplier = val[1];
            if (val[0] == 0x030d){
                // store the coarse pll_multiplier time
                pll_multiplier = (pll_multiplier << 8) | val[1];
            }
            if (val[0] == 0x0305)
            // store the coarse pre_pll_clk_div time,
                pre_pll_clk_div = (val[1] & 0x000f);
        }

    } else {
        printf("the sensor config file not exist, use default!\n");
        for (n = 0; n < nRegisters * 2; n += 2)
        {
            LOG_DEBUG("Writing %04x,%04x\n", registerArray[n], registerArray[n+1]);
            if((registerArray[n] == 0x0202) && (registerArray[n+2] == 0x0203)){
                // store the coarse integration time, so we can report it correctly
                coarsetime = registerArray[n+1];
                coarsetime = coarsetime << 8;
                coarsetime |= registerArray[n+3];
            }
            if(registerArray[n] == 0x3364)
                psCam->ui8MipiLanes = registerArray[n+1]? 2:4;
            if((registerArray[n] == 0x0340) && (registerArray[n+2] == 0x0341)){
                framelenghtlines = registerArray[n+1];
                framelenghtlines = framelenghtlines << 8;
                framelenghtlines |= registerArray[n+3];
            }
            // if(registerArray[n] == 0x3040)
            // {
            //    ui16read_mode = registerArray[n+1] & 0x3fff;
            //    registerArray[n+1] = ui16read_mode | (bFlipHorizontal?0x4000:0) | (bFlipVertical?0x8000:0);
            //    registerArray[n+1] = 0x0000;
            // }
            if((registerArray[n] == 0x0342) && (registerArray[n+2] == 0x0343)){
                // store the coarse line_length_pck time
                line_length_pck = registerArray[n+1];
                line_length_pck = line_length_pck << 8;
                line_length_pck |= registerArray[n+3];
            }
            if(registerArray[n] == 0x0301)
                // store the coarse vt_pix_clk_div time
                vt_pix_clk_div = (registerArray[n+1] & 0x000f);
            if(registerArray[n] == 0x0303)
                // store the coarse vt_sys_clk_div time
                vt_sys_clk_div = (registerArray[n+1] & 0x0003);
            if((registerArray[n] == 0x030c) && (registerArray[n+2] == 0x030d)){
                // store the coarse pll_multiplier time
                pll_multiplier = registerArray[n+1];
                pll_multiplier = pll_multiplier << 8;
                pll_multiplier |= registerArray[n+3];
            }
            if(registerArray[n] == 0x0305)
                // store the coarse pre_pll_clk_div time,
                pre_pll_clk_div = (registerArray[n+1] & 0x000f);

            ret = IMX179MIPI_I2C_WRITE(psCam->i2c, registerArray[n], registerArray[n+1]);
            if(ret != IMG_SUCCESS)
            {
                return ret;
            }

            //printf("address = 0x%x, value = 0x%x\n", registerArray[n], registerArray[n+1]);
        }
    }

    if (IMG_SUCCESS == ret)
    {
// trow = Tsh, The storage time (T_SH )
// Tsh = (Coarse_Integration_Time x Line_Length_Pck + �\ ) x pix_clk_period 
//Where �\ = offset time = readdable from fine_integration_time register. 
//pix_clk_period [s] = 1 / CK_PIXEL. 
//CK_PIXEL = EXTCLK frequency �� (pll_multiplier / pre_pll_clk_div) / (vt_sys_clk_div �� vt_pix_clk_div) �� 2. 

        double EXT_clk = 24 * 1000 * 1000;
        double pix_clk = (EXT_clk * (pll_multiplier / pre_pll_clk_div)) / (vt_sys_clk_div * vt_pix_clk_div) * 2;
        // line_length_pck in pixels, clk_pix in Hz, trow in seconds (time in a row)
        // double trow = ( line_length_pck + 488) * pix_clk; 
        double trow = 13.5391279;//(( line_length_pck/* + 488*/)*1000*1000) / pix_clk; 

        // 15~38744
        LOG_DEBUG("Sensor ready to go\n");

	// setup the exposure setting information
#ifdef IQ_TUNING
		psCam->ui32ExposureMax = 600000;
#else
        psCam->ui32ExposureMax = (IMG_UINT32)floor(trow * framelenghtlines/* * 2*/);
#endif
        // Minimum exposure time in us (line period) (use the frmae length lines entry to calculate this...
        psCam->ui32ExposureMin = (IMG_UINT32)floor(trow);
	psCam->dbExposureMin = trow;
	// should be set based on the coarse integration time x line lenght in us.
        psCam->ui32Exposure = 30000;//;psCam->ui32ExposureMin * coarsetime;
	psCam->ui32LongTimeExposure = framelenghtlines;
    
#if defined(ENABLE_AE_DEBUG_MSG)
        printf("===========================\n");
        printf("ExtClk             = %f\n", EXT_clk);
        printf("pre_pll_clk_div = %d\n", pre_pll_clk_div);
        printf("pll_multiplier  = %d\n", pll_multiplier);
        printf("vt_sys_clk_div  = %d\n", vt_sys_clk_div);
        printf("vt_pix_clk_div  = %d\n", vt_pix_clk_div);
        printf("clk_pix         = %f\n", pix_clk);
        printf("trow            = %f\n", trow);
        printf("ui32ExposureMax = %d\n", psCam->ui32ExposureMax);
        printf("ui32ExposureMin = %d\n", psCam->ui32ExposureMin);
        printf("ui32Exposure    = %d\n", psCam->ui32Exposure);
        printf("===========================\n");
        printf("ui32LongTimeExposure   = %d\n", psCam->ui32LongTimeExposure);
        printf("Mipi Lanes = %d\n", psCam->ui8MipiLanes);
        printf("===========================\n");
#endif

        psCam->flGain = 1.0;
        psCam->psSensorPhy->psGasket->bParallel = IMG_FALSE;
        psCam->psSensorPhy->psGasket->uiGasket= psCam->imager;
        // note dodgy value used for hw workaround
        psCam->psSensorPhy->psGasket->uiWidth = ui16Width;
        psCam->psSensorPhy->psGasket->uiHeight = ui16Height-1;
        psCam->psSensorPhy->psGasket->bVSync = IMG_TRUE;
        psCam->psSensorPhy->psGasket->bHSync = IMG_TRUE;
        psCam->psSensorPhy->psGasket->ui8ParallelBitdepth = 10;

        //usleep(1000);
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
static int IMX179MIPI_setDefFlipMirror(SENSOR_HANDLE hHandle, int mode, int flag)
{
	IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
	IMG_UINT16 *registerArray = NULL;
	int nRegisters = 0, i = 0;
	int x = -1, y = -1, z = -1;
	int get_pos_done = 0;

	//printf("-->IMX179MIPI_setDefFlipMirror %d\n", flag);

    registerArray = IMX179MIPI_GetRegisters(mode, &nRegisters);

	nRegisters *= 2;

	for (i = 0; i < nRegisters - 1; i += 2)
	{
		if (registerArray[i] == 0x0101)
		{
			switch (flag)
			{
			case SENSOR_FLIP_HORIZONTAL:
				registerArray[i+1] &= ~0x03;
				registerArray[i+1] |= 0x01;
				psCam->ui8CurrentFlipping = SENSOR_FLIP_HORIZONTAL;
				break;
			case SENSOR_FLIP_VERTICAL:
				registerArray[i+1] &= ~0x03;
				registerArray[i+1] |= 0x02;
				psCam->ui8CurrentFlipping = SENSOR_FLIP_VERTICAL;
				break;
			case SENSOR_FLIP_BOTH:
				registerArray[i+1] |= 0x03;
				psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
				break;
			case SENSOR_FLIP_NONE:
			default:
				registerArray[i+1] &= ~0x03;
				psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
				break;
			}
		}
	}

	return 0;

}

static IMG_RESULT IMX179MIPI_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{
	IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
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

    		aui16Regs[0] = 0x0101;
    		aui16Regs[1] = IMX179MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] &= ~0x03;

    		IMX179mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    		break;

    	case SENSOR_FLIP_HORIZONTAL:
    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_HORIZONTAL)
    		{
    			printf("the sensor had been SENSOR_FLIP_HORIZONTAL.\n");
    			return IMG_SUCCESS;
    		}

    		aui16Regs[0] = 0x0101;
    		aui16Regs[1] = IMX179MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] &= ~0x03;
    		aui16Regs[1] |= 0x01;

    		IMX179mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_HORIZONTAL;
    		break;

    	case SENSOR_FLIP_VERTICAL:
    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_VERTICAL)
    		{
    			printf("the sensor had been SENSOR_FLIP_VERTICAL.\n");
    			return IMG_SUCCESS;
    		}

    		aui16Regs[0] = 0x0101;
    		aui16Regs[1] = IMX179MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] &= ~0x03;
    		aui16Regs[1] |= 0x02;

    		IMX179mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_VERTICAL;
    		break;

    	case SENSOR_FLIP_BOTH:
    		if (psCam->ui8CurrentFlipping == SENSOR_FLIP_BOTH)
    		{
    			printf("the sensor had been SENSOR_FLIP_BOTH.\n");
    			return IMG_SUCCESS;
    		}

    		aui16Regs[0] = 0x0101;
    		aui16Regs[1] = IMX179MIPI_I2C_READ(psCam->i2c, aui16Regs[0]);

    		aui16Regs[1] |= 0x03;
			
    		IMX179mipi_i2c_write16(psCam->i2c, aui16Regs, 2);
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
    		break;

    }
    	return IMG_SUCCESS;
}

static IMG_RESULT IMX179MIPI_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
#if 0
    IMX179MIPI_SetExposure(hHandle, ui32Exposure, ui8Context);
    IMX179MIPI_SetGain(hHandle, flGain, ui8Context);
#else
	IMG_RESULT ret = IMG_SUCCESS;
#if !defined(INFOTM_DISABLE_AE)
	IMG_INT32 i32Lines;
	IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);

	IMG_UINT16 aui16Regs[4];
    unsigned int nIndex = 0;
	unsigned int grouped_flag = 0;

	if (!psCam->psSensorPhy)
	{
		LOG_ERROR("sensor not initialised\n");
		return IMG_ERROR_NOT_INITIALISED;
	}

	//sui32Current = 20000;
	psCam->ui32Exposure = ui32Exposure;

	//ui32Lines = (psCam->ui32ExposureMax - psCam->ui32Exposure) / psCam->ui32ExposureMin;
        //i32Lines = asModes[psCam->ui16CurrentMode].ui16VerticalTotal - (psCam->ui32Exposure / psCam->ui32ExposureMin);
        i32Lines = psCam->ui32Exposure / psCam->dbExposureMin;
        //printf("====IMX179MIPI_SetExposure time(%d), lines(%d)====\n", ui32Current, i32Lines);
        
	if (g_IMX179_ui32LinesBackup == i32Lines) {
		
		goto SET_GAIN;
	}

	g_IMX179_ui32LinesBackup = i32Lines;

#if defined(ENABLE_AE_DEBUG_MSG)
	//LOG_DEBUG("Exposure Val %x\n", i32Lines);
	printf("IMX179_SetExposure lines = 0x%08X (Exposure %d)\n", i32Lines, psCam->ui32Exposure);
#endif

	grouped_flag = 1;
	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x01); 

	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0202, ((i32Lines) >> 8)); 
	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0203, (i32Lines) & 0xff);  
	
SET_GAIN:

    psCam->flGain = flGain;

    while(nIndex < (sizeof(asIMX179GainValues) / sizeof(ARGAINTABLE)) - 1)
    {
        if(asIMX179GainValues[nIndex].flGain > flGain)
		{
            break;
        }
        nIndex++;
    }

    if(nIndex > 0)
    {
        nIndex = nIndex - 1;
    }

	if (g_IMX179_ui8GainValBackup == nIndex)
	{
		if (grouped_flag == 1)
		{
			IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x00);
		}
		return IMG_SUCCESS;
	}
	
	g_IMX179_ui8GainValBackup = nIndex;

    flGain = psCam->flGain / asIMX179GainValues[nIndex].flGain;

	if (psCam->flGain > MIN_PLUS_GAIN)
	{
		return IMG_SUCCESS;
	}

	if (grouped_flag == 0)
	{
		IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x01);
	}
	
	if (psCam->flGain < MIN_DIGITAL_GAIN)
	{
		aui16Regs[0] = 0x0204;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);

		aui16Regs[0] = 0x0205;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
	}
	else
	{
		aui16Regs[0] = 0x020E;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);

		aui16Regs[0] = 0x020F;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]); 

		aui16Regs[0] = 0x0210;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
		
		aui16Regs[0] = 0x0211;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]); 

		aui16Regs[0] = 0x0212;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
		
		aui16Regs[0] = 0x0213;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]); 

		aui16Regs[0] = 0x0214;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal>>8)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
		
		aui16Regs[0] = 0x0215;
		aui16Regs[1] = (IMG_UINT16)((IMG_UINT16)(asIMX179GainValues[nIndex].ui8GainRegVal)&0xFF);
		ret = IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]); 
	}

		
#if defined(ENABLE_AE_DEBUG_MSG)
    printf("====>IMX179MIPI_SetGain Change Analog Gain = 0x%04X\n", asIMX179GainValues[nIndex].ui8GainRegVal);
#endif

	IMX179MIPI_I2C_WRITE(psCam->i2c, 0x0104, 0x00);
#endif

#endif
    return IMG_SUCCESS;
}
			
#endif //INFOTM_ISP
static IMG_RESULT IMX179MIPI_SetFPS(SENSOR_HANDLE hHandle, double fps)
{
    IMX179CAM_STRUCT *psCam = container_of(hHandle, IMX179CAM_STRUCT, sFuncs);
    IMG_UINT16 aui16Regs[2];

    double realFps = 0.0;
    IMG_UINT32 frameLine = 0;

    if (fps > psCam->fixedFps)
        realFps = psCam->fixedFps;
    else
        realFps = fps;
    frameLine = ((double)1 * 1000 * 1000) / (realFps * psCam->ui32ExposureMin);

    //printf("(%s,L%d) current fps =%.10f, fixFps=%d  realFps=%f \n", __func__, __LINE__, fps, psCam->fixedFps, realFps);

    //printf("(%s,l%d) frameLine=%d min=%d \n", __func__, __LINE__,frameLine, psCam->ui32ExposureMin);

    aui16Regs[0] = 0x0340;
    aui16Regs[1] = (frameLine >> 8) & 0xFF;
    IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0],  aui16Regs[1]);
    aui16Regs[0] = 0x0341;
    aui16Regs[1] = frameLine & 0xFF;
    IMX179MIPI_I2C_WRITE(psCam->i2c, aui16Regs[0],  aui16Regs[1]);

    psCam->fCurrentFps = realFps;
    return IMG_SUCCESS;
}
