/**
 ******************************************************************************
 @file imx322dvp.c
 ******************************************************************************/

#include "sensors/imx322dvp.h"
#include "sensors/ddk_sensor_driver.h"
#define LOG_TAG "IMX322DVP_SENSOR"

static IMG_RESULT IMX322DVP_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
static IMG_RESULT IMX322DVP_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag);
static IMG_RESULT IMX322DVP_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed);
static IMG_RESULT IMX322DVP_SetFPS(SENSOR_HANDLE hHandle, double fps);

#define IMX322DVP_WRITE_ADDR		(0x1a)
#define IMX322DVP_READ_ADDR			(0x1a)
#define IMX322_SENSOR_VERSION 	    "not-verified"

typedef struct _imx322cam_struct
{
    SENSOR_FUNCS sFuncs;
    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
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
}IMX322CAM_STRUCT;

static IMG_UINT32 g_IMX322_ui32LinesBackup = 0;
static IMG_UINT8 g_IMX322_ui8GainValBackup = 0xff;
static IMG_UINT32 i2c_addr = 0;

static SENSOR_MODE asModes[]=
{
	{10, 1920, 1088, 30.0f, 1125,  SENSOR_FLIP_BOTH},
	{10, 2188, 1124, 15.0f, 1125,  SENSOR_FLIP_BOTH},
	{10, 1628, 736,  30.0f, 750,   SENSOR_FLIP_BOTH},
};

static IMG_UINT16 ui16ModeRegs_0[] = {
    //1080p30fps

    0x3000, 0x31,
	0x0100, 0x00,
	0x302c, 0x01,
	0x0008, 0x00,
	0x0009, 0x40,
	0x0101, 0x03,
	0x0104, 0x00,
	0x0112, 0x0a,
	0x0113, 0x0a,
	0x0202, 0x00,
	0x0203, 0x00,
	0x0340, 0x04,
	0x0341, 0x64,//0x65
	0x0342, 0x04,
	0x0343, 0x4c,
	
	0x3001, 0x00,
	0x3002, 0x0f,
	0x3003, 0x4C, 
	0x3004, 0x04,
	0x3005, 0x65,
	0x3006, 0x04,
	0x3007, 0x00,
	0x3008, 0x00, 
	0x3009, 0x00, 
	0x300A, 0xF0, 
	0x300B, 0x00, 
	0x300C, 0x00, 
	0x300D, 0x00, 
	0x300E, 0x00, 
	0x300F, 0x00, 
	0x3010, 0x00, 
	0x3011, 0x00,
	0x3012, 0x80,
	0x3013, 0x40, 
	0x3014, 0x00, 
	0x3015, 0x00, 
	0x3016, 0x3c,
	0x3017, 0x00, 
	0x3018, 0x00, 
	0x3019, 0x00, 
	0x301A, 0x00, 
	0x301B, 0x00, 
	0x301C, 0x50, 
	0x301D, 0x00, 
	0x301E, 0x00, 
	0x301f, 0x73,
	0x3020, 0x0c,
	//0x3020, 0x00,	
	
	0x3021, 0x90,  //0x80
	0x3022, 0x43,  //0x00
	0x3027, 0x20,
	0x302C, 0x01,
	0x302D, 0x40, 

	0x307a, 0x40,
	0x307b, 0x02,
	0x3098, 0x26, 
	0x3099, 0x02, 
	0x309a, 0x4c,
	0x309b, 0x04,
	0x30ce, 0x16,
	0x30cf, 0x82,
	0x30d0, 0x00,
	0x3117, 0x0d,
//  0x302c, 0x00,
//  0x3000, 0x30,
//  0x0100, 0x01,
};

static IMG_UINT16 ui16ModeRegs_1[] = {
	//1080p 15fps
	0x3000, 0x31,
	0x0100, 0x00,
	0x302c, 0x01,
	0x0008, 0x00,
	0x0009, 0x40,
	0x0101, 0x03,
	0x0104, 0x00,
	0x0112, 0x0a,
	0x0113, 0x0a,
	0x0202, 0x00,
	0x0203, 0x00,
	0x0340, 0x04,
	0x0341, 0x65,
	0x0342, 0x08,
	0x0343, 0x98,
	0x3001, 0x00,
	0x3002, 0x0f,
	0x3003, 0x98, 
	0x3004, 0x08,
	0x3005, 0x65,
	0x3006, 0x04,
	0x3007, 0x00,
	0x3008, 0x00, 
	0x3009, 0x00, 
	0x300A, 0x00, 
	0x300B, 0x00, 
	0x300C, 0x00, 
	0x300D, 0x00, 
	0x300E, 0x00, 
	0x300F, 0x00, 
	0x3010, 0x00, 
	0x3011, 0x01,
	0x3012, 0x80,
	0x3013, 0x40, 
	0x3014, 0x00, 
	0x3015, 0x00, 
	0x3016, 0x3c,
	0x3017, 0x00, 
	0x3018, 0x00, 
	0x3019, 0x00, 
	0x301A, 0x00, 
	0x301B, 0x00, 
	0x301C, 0x50, 
	0x301D, 0x00, 
	0x301E, 0x00, 
	0x301f, 0x73,
	0x3020, 0x1c,
	
	0x3021, 0x90,  //0x80
	0x3022, 0x40,  //0x00
	0x3027, 0x20,
	0x302C, 0x01,
	0x302D, 0x40, 
    
	0x307a, 0x40,
	0x307b, 0x02,
	0x3098, 0x26, 
	0x3099, 0x02, 
	0x309a, 0x4c,
	0x309b, 0x04,
	0x30ce, 0x16,
	0x30cf, 0x82,
	0x30d0, 0x00,
	0x3117, 0x0d,
};

static IMG_UINT16 ui16ModeRegs_2[] = {
    //720p30fps
#if 1    
    0x3000, 0x31,
    0x0100, 0x00,
    0x302c, 0x01,

    0x0008, 0x00,
    0x0009, 0x40,
    0x0101, 0x03,
    0x0104, 0x00,
    0x0112, 0x0a,
    0x0113, 0x0a,
    0x0202, 0x00,
    0x0203, 0x00,
    0x0340, 0x02,
    0x0341, 0xee,
    0x0342, 0x06,
    0x0343, 0x72,
    0x0345, 0x40,
    0x0346, 0x01,

    0x3001, 0x00,
    0x3002, 0x01,
    0x3003, 0x04,
    0x3004, 0x4c,
    0x3005, 0x65,
    0x3006, 0x04,
    0x3007, 0x00,
    0x3011, 0x01,
    0x3012, 0x80,
    
    0x3016, 0xf0,
    0x301f, 0x33,
    0x3020, 0x3c,
    0x3021, 0xa0,
    0x3022, 0xc3,
    0x3027, 0x20,
    0x302c, 0x00,

    0x307a, 0x40,
    0x307b, 0x02,
    0x3098, 0x02,
    0x3099, 0x26,
    0x309a, 0x4c,
    0x309b, 0x04,
    0x30ce, 0x40,
    0x30cf, 0x81,
    0x30d0, 0x01,

    0x3117, 0x0d,
#else
	0x3000, 0x31,
	0x0100, 0x00,
	0x302c, 0x01,
	
	0x0008, 0x00,
	0x0009, 0x40,
	0x0101, 0x03,
	0x0104, 0x00,
	0x0112, 0x0a,
	0x0113, 0x0a,
	0x0202, 0x00,
	0x0203, 0x00,
	0x0340, 0x02,
	0x0341, 0xee,
	0x0342, 0x06,
	0x0343, 0x72,
	0x3045, 0x40,
	0x3046, 0x01,
	0x3001, 0x00,
	0x3002, 0x01,
	0x3003, 0x4C, 
	0x3004, 0x04,
	0x3005, 0x65,
	0x3006, 0x04,
	0x3007, 0x00,
	0x3008, 0x00, 
	0x3009, 0x00, 
	0x300A, 0x00, 
	0x300B, 0x00, 
	0x300C, 0x00, 
	0x300D, 0x00, 
	0x300E, 0x00, 
	0x300F, 0x00, 
	0x3010, 0x00, 
	0x3011, 0x01,
	0x3012, 0x80,
	0x3013, 0x40, 
	0x3014, 0x00, 
	0x3015, 0x00, 
	0x3016, 0xf0,
	0x3017, 0x00, 
	0x3018, 0x00, 
	0x3019, 0x00, 
	0x301A, 0x00, 
	0x301B, 0x00, 
	0x301C, 0x50, 
	0x301D, 0x00, 
	0x301E, 0x00, 
	0x301f, 0x33,
	
	0x3020, 0x3c,
	0x3021, 0xa0,  
	0x3022, 0xc3,  
	0x3027, 0x20,
	0x302C, 0x01,

	0x302D, 0x40, 
	0x307a, 0x40,
	0x307b, 0x02,
	0x3098, 0x26, 
	0x3099, 0x02, 
	0x309a, 0x4c,
	0x309b, 0x04,
	0x30ce, 0x40,
	0x30cf, 0x81,
	0x30d0, 0x01,
	0x3117, 0x0d,
#endif    
    
};

static IMG_RESULT doStaticSetup(IMX322CAM_STRUCT *psCam, IMG_INT16 ui16Width,
        IMG_UINT16 ui16Height, IMG_UINT16 ui16Mode, IMG_BOOL bFlipHorizontal,
        IMG_BOOL bFlipVertical);

static int IMX322DVP_I2C_WRITE(int i2c,  IMG_UINT16 reg, IMG_UINT16 val)
{
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

    return 0;  
}  

static uint16_t IMX322DVP_I2C_READ(int i2c, uint16_t reg)
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

    LOG_DEBUG("RD:0x%04x 0x%04x\n", reg, buf[0]);

    return buf[0];  
}

static IMG_RESULT IMX322DVP_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

	psInfo->eBayerOriginal = MOSAIC_GBRG;//MOSAIC_GRBG;
    psInfo->eBayerEnabled  = MOSAIC_GBRG;//MOSAIC_GRBG;
    sprintf(psInfo->pszSensorName, IMX322_SENSOR_VERSION);
    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 7500;
    //psInfo->ui8BitDepth = 10;
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = 1;
    psInfo->bBackFacing = IMG_TRUE;
	//other fields should be filled by calling function Sensor_GetInfo() before this is called

    return IMG_SUCCESS;
}

static IMG_RESULT IMX322DVP_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes)
{
    if(nIndex < sizeof(asModes) / sizeof(SENSOR_MODE)) {
		IMG_MEMCPY(psModes, &(asModes[nIndex]), sizeof(SENSOR_MODE));
        return IMG_SUCCESS;
    }

    return IMG_ERROR_NOT_SUPPORTED;
}

static IMG_RESULT IMX322DVP_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy) {
        LOG_WARNING("sensor not initialised\n");
        psStatus->eState = SENSOR_STATE_UNINITIALISED;
        psStatus->ui16CurrentMode = 0;
    } else {
        psStatus->eState = (psCam->bEnabled ? SENSOR_STATE_RUNNING : SENSOR_STATE_IDLE);
        psStatus->ui16CurrentMode = psCam->ui16CurrentMode;
		psStatus->ui8Flipping = psCam->ui8CurrentFlipping;
    }

    return IMG_SUCCESS;
}

static IMG_RESULT IMX322DVP_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Mode,
        IMG_UINT8 ui8Flipping)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy) {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(ui16Mode < (sizeof(asModes) / sizeof(SENSOR_MODE))) {
        psCam->ui16CurrentMode = ui16Mode;
		if((ui8Flipping&(asModes[ui16Mode].ui8SupportFlipping))!=ui8Flipping) {
            LOG_ERROR("sensor mode does not support selected flipping 0x%x (supports 0x%x)\n", 
                    ui8Flipping, asModes[ui16Mode].ui8SupportFlipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        ret = doStaticSetup(psCam, asModes[ui16Mode].ui16Width, asModes[ui16Mode].ui16Height,
                ui16Mode, ui8Flipping&SENSOR_FLIP_HORIZONTAL, ui8Flipping&SENSOR_FLIP_VERTICAL);
        if (IMG_SUCCESS != ret) {
            LOG_ERROR("failed to configure mode %d\n", ui16Mode);
        } else {
            psCam->ui16CurrentMode = ui16Mode;
            psCam->ui8CurrentFlipping = ui8Flipping;
        }
        return ret;
    }
	
    LOG_ERROR("invalid mode %d\n", ui16Mode);

    return IMG_ERROR_INVALID_PARAMETERS;
}

static IMG_RESULT IMX322DVP_Enable(SENSOR_HANDLE hHandle)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 aui16Regs[2];
    IMG_UINT8 value = 0;
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy) {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling IMX322 Camera!\n");
    psCam->bEnabled = IMG_TRUE;
    aui16Regs[0] = 0x0100;
    aui16Regs[1] = 0x0001;

    ret = SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->ui8MipiLanes, 0);
    if (ret == IMG_SUCCESS) {
#if 1        
		ret = IMX322DVP_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
        if (IMG_SUCCESS != ret) {
            LOG_ERROR("failed to enable sensor\n");
        } else {
            LOG_DEBUG("camera enabled\n");
//            value = IMX322DVP_I2C_READ(psCam->i2c, 0x0100);
//            LOG_ERROR("Read 0x0100 register value = 0x%x\n",value);
        }
#endif
        g_IMX322_ui32LinesBackup = 0;
        g_IMX322_ui8GainValBackup = 0xff;

    }
#if 0    
    IMX322DVP_I2C_WRITE(psCam->i2c, 0x301e, 0x0f); 
    IMX322DVP_I2C_WRITE(psCam->i2c, 0x301f, 0x73);
    
    IMX322DVP_I2C_WRITE(psCam->i2c, 0x0202, 0x00); 
    IMX322DVP_I2C_WRITE(psCam->i2c, 0x0203, 0xfe);  
#endif
    return ret;
}

static IMG_RESULT IMX322DVP_DisableSensor(SENSOR_HANDLE hHandle)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 aui16Regs[2];
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy) {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Disabling IMX322 camera\n");
    psCam->bEnabled = IMG_FALSE;
#if 1    
    aui16Regs[0] = 0x0100;
    aui16Regs[1] = 0x0000;

    ret = IMX322DVP_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    if (IMG_SUCCESS != ret) {
        LOG_ERROR("failed to disable sensor!\n");
    }
#endif
    usleep(100000);
    SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, 0);

    return ret;
}

static IMG_RESULT IMX322DVP_DestroySensor(SENSOR_HANDLE hHandle)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy) {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Destroying IMX322 camera\n");
    if(psCam->bEnabled) {
        IMX322DVP_DisableSensor(hHandle);
    }

    SensorPhyDeinit(psCam->psSensorPhy);

    close(psCam->i2c);

    IMG_FREE(psCam);

    return IMG_SUCCESS;
}

static IMG_RESULT IMX322DVP_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy) {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current = psCam->ui32Exposure;

    return IMG_SUCCESS;
}


static IMG_RESULT IMX322DVP_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Current,
        IMG_UINT8 ui8Context)
{
        IMG_RESULT ret = IMG_SUCCESS;
    
        IMG_UINT32 ui32Lines;
    
        IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);
    
        if (!psCam->psSensorPhy)
        {
            LOG_ERROR("sensor not initialised\n");
            return IMG_ERROR_NOT_INITIALISED;
        }
    
        psCam->ui32Exposure=ui32Current;
    
        ui32Lines = psCam->ui32Exposure / psCam->dbExposureMin;

//        if ((1098 < ui32Lines) && (1104 >= ui32Lines)) {
//            ui32Lines = 1098;
//        }
        if (g_IMX322_ui32LinesBackup == ui32Lines) {
            return IMG_SUCCESS;
        }
        g_IMX322_ui32LinesBackup = ui32Lines;

        LOG_DEBUG("Exposure Val %x\n", ui32Lines);
#if defined(ENABLE_AE_DEBUG_MSG)
        printf("IMX322_SetExposure Exposure = 0x%08X (%d)\n", psCam->ui32Exposure, psCam->ui32Exposure);
#endif
    
        IMX322DVP_I2C_WRITE(psCam->i2c, 0x0202, (ui32Lines>>8)); 
        IMX322DVP_I2C_WRITE(psCam->i2c, 0x0203, ui32Lines&0xff);  
    
        return ;

    return ret;
}

static IMG_RESULT IMX322DVP_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
        double *pflMax, IMG_UINT8 *pui8Contexts)
{
    *pflMin = 1.0;
    *pflMax = 16.0;
    *pui8Contexts = 1;

    return IMG_SUCCESS;
}

static IMG_RESULT IMX322DVP_GetGain(SENSOR_HANDLE hHandle,
        double *pflGain, IMG_UINT8 ui8Context)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy) {
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

static ARGAINTABLE asIMXGainValues[]=
{
    {0.0, 0x00},
    {0.3, 0x01},
    {0.6, 0x02},
    {0.9, 0x03},
    {1.2, 0x04},
    {1.5, 0x05},
    {1.8, 0x06},
    {2.1, 0x07},
    {2.4, 0x08},
    {2.7, 0x09},
    {3.0, 0x0a},
    {3.3, 0x0b},
    {3.6, 0x0c},
    {3.9, 0x0d},
    {4.2, 0x0e},
    {4.5, 0x0f},
    {4.8, 0x10},
    {5.1, 0x11},
    {5.4, 0x12},
    {5.7, 0x13},
    {6.0, 0x14},
    {6.3, 0x15},
    {6.6, 0x16},
    {6.9, 0x17},
    {7.2, 0x18},
    {7.5, 0x19},
    {7.8, 0x1A},
    {8.1, 0x1B},
    {8.4, 0x1C},
    {8.7, 0x1D},
    {9.0, 0x1E},    
    {9.3, 0x1F},    

    {9.6, 0x20},
    {9.9, 0x21},
    {10.2, 0x22},
    {10.5, 0x23},
    {10.8, 0x24},
    {11.1, 0x25},
    {11.4, 0x26},
    {11.7, 0x27},
    {12.0, 0x28},
    {12.3, 0x29},
    {12.6, 0x2A},
    {12.9, 0x2B},
    {13.2, 0x2C},
    {13.5, 0x2D},
    {13.8, 0x2E},    
    {14.1, 0x2F},  

    {14.4, 0x30},
    {14.7, 0x31},
    {15.0, 0x32},
    {15.3, 0x33},
    {15.6, 0x34},
    {15.9, 0x35},
    {16.2, 0x36},
    {16.5, 0x37},
    {16.8, 0x38},
    {17.1, 0x39},
    {17.4, 0x3A},
    {17.7, 0x3B},
    {18.0, 0x3C},
    {18.3, 0x3D},
    {18.6, 0x3E},    
    {18.9, 0x3F},  

    {19.2, 0x40},
    {19.5, 0x41},
    {19.8, 0x42},
    {20.1, 0x43},
    {20.4, 0x44},
    {20.7, 0x45},
    {21.0, 0x46},
    {21.3, 0x47},
    {21.6, 0x48},
    {21.9, 0x49},
    {22.2, 0x4A},
    {22.5, 0x4B},
    {22.8, 0x4C},
    {23.1, 0x4D},
    {23.4, 0x4E},    
    {23.7, 0x4F},  
};


static IMG_RESULT IMX322DVP_SetGain(SENSOR_HANDLE hHandle, double flGain,
        IMG_UINT8 ui8Context)
{
        IMG_UINT16 aui16Regs[4];
        unsigned int nIndex = 0;
        IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);
        IMG_RESULT ret;

        //printf("-->IMX322DVP_SetGain %f\n", flGain);
        if (!psCam->psSensorPhy)
        {
            LOG_ERROR("sensor not initialised\n");
            return IMG_ERROR_NOT_INITIALISED;
        }
    
        psCam->flGain = flGain;
    
        while(nIndex < (sizeof(asIMXGainValues) / sizeof(ARGAINTABLE)) - 1)
        {
            if(asIMXGainValues[nIndex].flGain > flGain)
                break;
            nIndex++;
        }
    
        if(nIndex > 0)
            nIndex = nIndex - 1;

        if (g_IMX322_ui8GainValBackup == asIMXGainValues[nIndex].ui8GainRegVal)
        {
             return IMG_SUCCESS;
        }
        
        g_IMX322_ui8GainValBackup = asIMXGainValues[nIndex].ui8GainRegVal;
        //flGain = psCam->flGain / asIMXGainValues[nIndex].flGain;
    
        aui16Regs[0] = 0x301E;// Global Gain register
        aui16Regs[1] = (IMG_UINT16)asIMXGainValues[nIndex].ui8GainRegVal;//(IMG_UINT16)(flGain*128);
        ret = IMX322DVP_I2C_WRITE(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#if defined(ENABLE_AE_DEBUG_MSG)
        printf("IMX322_SetGain Change Global Gain 0x301E = 0x%04X\n", aui16Regs[1]);
#endif
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to set global gain register\n");
            return ret;
        }
    
        return IMG_SUCCESS;

}

static IMG_RESULT IMX322DVP_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
        IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy) {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui32Min = psCam->ui32ExposureMin;
    *pui32Max = psCam->ui32ExposureMax;
    *pui8Contexts = 1;

    return IMG_SUCCESS;
}

IMG_RESULT IMX322_Create_DVP(struct _Sensor_Functions_ **hHandle)
{
    char i2c_dev_path[NAME_MAX];
    IMX322CAM_STRUCT *psCam;
    char adaptor[64];
    int fd = 0;
	fd = open("/dev/ddk_sensor", O_RDWR);
	if(fd <0)
	{
		LOG_ERROR("open ddk sensor error\n");
		return IMG_ERROR_FATAL;
	}

	ioctl(fd, GETI2CADDR, &i2c_addr);
	close(fd);
    psCam = (IMX322CAM_STRUCT *)IMG_CALLOC(1, sizeof(IMX322CAM_STRUCT));
    if (!psCam) {
        return IMG_ERROR_MALLOC_FAILED;
    }

    *hHandle = &psCam->sFuncs;
    psCam->sFuncs.GetMode  = IMX322DVP_GetMode;
    psCam->sFuncs.GetState = IMX322DVP_GetState;
    psCam->sFuncs.SetMode  = IMX322DVP_SetMode;
    psCam->sFuncs.Enable   = IMX322DVP_Enable;
    psCam->sFuncs.Disable  = IMX322DVP_DisableSensor;
    psCam->sFuncs.Destroy  = IMX322DVP_DestroySensor;

    psCam->sFuncs.GetGainRange   = IMX322DVP_GetGainRange;
    psCam->sFuncs.GetCurrentGain = IMX322DVP_GetGain;
    psCam->sFuncs.SetGain        = IMX322DVP_SetGain;

    psCam->sFuncs.GetExposureRange = IMX322DVP_GetExposureRange;
    psCam->sFuncs.GetExposure      = IMX322DVP_GetExposure;
    psCam->sFuncs.SetExposure      = IMX322DVP_SetExposure;

    psCam->sFuncs.GetInfo = IMX322DVP_GetInfo;
    psCam->sFuncs.SetGainAndExposure = IMX322DVP_SetGainAndExposure;

    psCam->sFuncs.SetFlipMirror = IMX322DVP_SetFlipMirror;
    psCam->sFuncs.SetFPS = IMX322DVP_SetFPS;
    psCam->sFuncs.GetFixedFPS = IMX322DVP_GetFixedFPS;

    psCam->bEnabled           = IMG_FALSE;
    psCam->ui16CurrentMode    = 0;
    psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;

    if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), i2c_addr, adaptor)) {
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

    return IMG_SUCCESS;
}

#define CASE_MODE(N) \
case N:\
    registerArray = ui16ModeRegs_##N;\
    nRegisters = sizeof(ui16ModeRegs_##N)/(2*sizeof(IMG_UINT16)); \
    break;

static IMG_RESULT doStaticSetup(IMX322CAM_STRUCT *psCam, IMG_INT16 ui16Width,
        IMG_UINT16 ui16Height, IMG_UINT16 ui16Mode, IMG_BOOL bFlipHorizontal,
        IMG_BOOL bFlipVertical)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 *registerArray = NULL;
    int n, nRegisters = 0;
    IMG_UINT8 value = 0;
	
	IMG_ASSERT(sizeof(asModes)/sizeof(SENSOR_MODE)>=3);
	
    switch(ui16Mode) {
        CASE_MODE(0)
        CASE_MODE(1)
        CASE_MODE(2)
        default:
            LOG_ERROR("Unknown mode %d\n", ui16Mode);
            return IMG_ERROR_FATAL;
    }
#if 1
    value = IMX322DVP_I2C_READ(psCam->i2c, 0x3117);
    
    LOG_ERROR("read chip id,chipid=0x%x\n",value);
    
    for (n = 0; n < nRegisters * 2; n += 2) {
//       LOG_DEBUG("Writing %04x,%04x\n", registerArray[n], registerArray[n+1]);

        ret = IMX322DVP_I2C_WRITE(psCam->i2c, registerArray[n], registerArray[n+1]);
        if(ret != IMG_SUCCESS)
        {
        	return ret;
        }
        value = IMX322DVP_I2C_READ(psCam->i2c, registerArray[n]);
//        LOG_ERROR("read register 0x%x, value = 0x%x\n", registerArray[n], value);
    }
    usleep(100*1000);                                                             
    IMX322DVP_I2C_WRITE(psCam->i2c, 0x302c, 0x00);                           
    usleep(100*1000);                                                             
    IMX322DVP_I2C_WRITE(psCam->i2c, 0x3000, 0x30); 
                              
//    IMX322DVP_I2C_WRITE(psCam->i2c, 0x0100, 0x01);
#endif
    if (IMG_SUCCESS == ret) {
        LOG_DEBUG("Sensor ready to go\n");

        psCam->ui32ExposureMin = 96;
        //psCam->ui32ExposureMax = psCam->ui32ExposureMin * (ui16Height - 1);

//    psCam->dbExposureMin = 2416.0 / 80;//75;
//    psCam->ui32ExposureMin = (IMG_UINT32)floor(psCam->dbExposureMin);
#if 1 //!defined(IQ_TUNING)
        psCam->ui32ExposureMax = psCam->ui32ExposureMin * asModes[psCam->ui16CurrentMode].ui16VerticalTotal;
#else
        psCam->ui32ExposureMax = 600000;
#endif
        psCam->ui32Exposure    = psCam->ui32ExposureMin/* * 100*/;

        printf("--->ui32ExposureMin=%d, ui32ExposureMax=%d, ui32Exposure=%d\n", 
                psCam->ui32ExposureMin, psCam->ui32ExposureMax, psCam->ui32Exposure);
        
        psCam->flGain = 1.0;
        psCam->psSensorPhy->psGasket->bParallel = IMG_TRUE;
        psCam->psSensorPhy->psGasket->bVSync    = IMG_TRUE;
        psCam->psSensorPhy->psGasket->bHSync    = IMG_TRUE;
        psCam->psSensorPhy->psGasket->uiGasket  = 1;
        psCam->psSensorPhy->psGasket->uiWidth   = ui16Width;
        psCam->psSensorPhy->psGasket->uiHeight  = ui16Height - 1;
        psCam->psSensorPhy->psGasket->ui8ParallelBitdepth = 10;

        usleep(1000);
    } else {
        LOG_ERROR("Failed to setup register\n");
    }

    return ret;
}
static IMG_RESULT IMX322DVP_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
	IMX322DVP_SetGain(hHandle, flGain, ui8Context);
	IMX322DVP_SetExposure(hHandle, ui32Exposure, ui8Context);
    return IMG_SUCCESS;
}

static IMG_RESULT IMX322DVP_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{
    IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

    switch (flag)
    {
    	case SENSOR_FLIP_NONE:
		IMX322DVP_I2C_WRITE(psCam->i2c, 0x0101, 0x00); 
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    		break;

    	case SENSOR_FLIP_HORIZONTAL:
                  IMX322DVP_I2C_WRITE(psCam->i2c, 0x0101, 0x01); 
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_HORIZONTAL;
    		break;

    	case SENSOR_FLIP_VERTICAL:
                   IMX322DVP_I2C_WRITE(psCam->i2c, 0x0101, 0x02); 	
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_VERTICAL;
    		break;

    	case SENSOR_FLIP_BOTH:
                   IMX322DVP_I2C_WRITE(psCam->i2c, 0x0101, 0x03); 
    		psCam->ui8CurrentFlipping = SENSOR_FLIP_BOTH;
    		break;

    }
    	return IMG_SUCCESS;
}

static IMG_RESULT IMX322DVP_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
	if (pfixed != NULL)
	{
		IMX322CAM_STRUCT *psCam = container_of(hHandle, IMX322CAM_STRUCT, sFuncs);

		*pfixed = 30;
	}

	return IMG_SUCCESS;
}

static IMG_RESULT IMX322DVP_SetFPS(SENSOR_HANDLE hHandle, double fps)
{

	return IMG_SUCCESS;
}
