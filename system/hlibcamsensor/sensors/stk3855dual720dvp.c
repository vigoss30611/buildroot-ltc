/******************************************************************************
@file stk3855dual720dvp.c
******************************************************************************/

#include "sensors/stk3855dual720dvp.h"

#include <img_types.h>
#include <img_errors.h>

#include <ci/ci_api_structs.h> // access to CI_GASKET
#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"

#include <stdio.h>
#include <math.h>

#if !file_i2c
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#endif

#include "sensors/ddk_sensor_driver.h"

#ifdef INFOTM_ISP
#include <linux/i2c.h>
#endif //INFOTM_ISP

#define LOG_TAG "STK3855DUAL720DVP_SENSOR"
#include <felixcommon/userlog.h>

#define STK3855DUAL720DVP_WRITE_ADDR (0x54 >> 1)

/** @brief Sensor number on IMG PHY */
#define STK3855DUAL720DVP_PHY_NUM 0

#define STK3855DUAL720DVP_SENSOR_VERSION "not-verified"  // until read from sensor

#ifdef WIN32 // on windows we do not need to sleep to wait for the bus
static void usleep(int t)
{
    (void)t;
}
#endif

#ifdef INFOTM_ISP
static IMG_UINT32 i2c_addr = 0;
#endif

typedef struct _stk3855dual720dvpcam_struct
{
    SENSOR_FUNCS sFuncs;
    
    // in MHz
    double refClock;
    
    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
    IMG_UINT8 ui8CurrentFlipping;
    IMG_BOOL bEnabled;
    IMG_UINT8 imager;

    SENSOR_MODE currMode;
    
    IMG_UINT32 ui32Exposure;
    double flGain;
    
    int i2c;
    SENSOR_PHY *psSensorPhy;

#ifdef INFOTM_ISP
	double fCurrentFps;
	IMG_UINT32 fixedFps;
	IMG_UINT32 initClk;
#endif
}STK3855DUAL720DVPCAM_STRUCT;

// not a real register - marks the end of register array
#define REG_STOP 0xFFFF

#ifdef INFOTM_ISP
static IMG_UINT16 ui16ModeRegs_1280_720_30[] = 
{
    0x3069, 0x02,	//Pad_Config_Pix_Out
    0x306A, 0x02,	//Pclk_Odrv

    0x3100, 0x03,
    0x3101, 0x00,
    0x3102, 0x0A,
    0x3103, 0x00,
    0x3105, 0x03,
    0x3106, 0x04,
    0x3107, 0x10,
    0x3108, 0x00,
    0x3109, 0x00,
    0x307D, 0x00,
    0x310A, 0x05,
    0x310C, 0x00,
    0x310D, 0x80,
    0x3110, 0x33,
    0x3111, 0x59,
    0x3112, 0x44,
    0x3113, 0x66,
    0x3114, 0x66,
    0x311D, 0x40,
    0x3127, 0x01,
    0x3129, 0x44,
    0x3136, 0x59,
    0x313F, 0x02,

//Tex  = (Exposure [15:0]) x Row_time
    0x3012, 0x02,	//Integration_Time_H
    0x3013, 0xd0,	//Integration_Time_L

    0x3262, 0x10,
    0x32B3, 0x80,
    0x32B4, 0x20,

    0x32CB, 0x30,
    0x32CC, 0x70,
    0x32CD, 0xA0,

//AWB adjust speed, value from 0 to 15
//0: Slowly.
//15: Quickly.
    0x3297, 0x03,	//AWB_Speed

    0x30A1, 0x23,
    0x30A2, 0x70,
    0x30A3, 0x01,
    0x30A0, 0x03,

    0x303E, 0x04,	//Clamp_En
    0x303F, 0x32,	//Calibration_Ctrl_1

    0x3055, 0x00,
    0x3056, 0x18,
    0x305F, 0x00,

    0x3268, 0x01,
    0x3051, 0x3A,	//ABLC_Ofs_Wgt
    0x308E, 0x3A,

    0x3360, 0x14,
    0x3361, 0x24,
    0x3362, 0x28,
    0x3363, 0x01,
    0x324F, 0x00,

	//SOC Lens Shading Correction Registers (0x3210)
    0x3210, 0x0C,
    0x3211, 0x0C,
    0x3212, 0x0C,
    0x3213, 0x0C,
    0x3214, 0x08,
    0x3215, 0x08,
    0x3216, 0x08,
    0x3217, 0x08,
    0x3218, 0x08,
    0x3219, 0x08,
    0x321A, 0x08,
    0x321B, 0x08,
    0x321C, 0x0C,
    0x321D, 0x0C,
    0x321E, 0x0C,
    0x321F, 0x0C,
    0x3230, 0x00,
    0x3231, 0x17,
    0x3232, 0x09,
    0x3233, 0x09,
    0x3234, 0x00,
    0x3235, 0x00,
    0x3236, 0x00,
    0x3237, 0x00,
    0x3238, 0x18,
    0x3239, 0x18,
    0x323A, 0x18,
    0x3243, 0xC3,
    0x3244, 0x00,
    0x3245, 0x00,

	//SOC Gamma Registers (0x3270)
    0x3270, 0x00,
    0x3271, 0x0B,
    0x3272, 0x16,
    0x3273, 0x2B,
    0x3274, 0x3F,
    0x3275, 0x51,
    0x3276, 0x72,
    0x3277, 0x8F,
    0x3278, 0xA7,
    0x3279, 0xBC,
    0x327A, 0xDC,
    0x327B, 0xF0,
    0x327C, 0xFA,
    0x327D, 0xFE,
    0x327E, 0xFF,


    0x3371, 0x00,
    0x3372, 0x00,
    0x3374, 0x00,
    0x3379, 0x00,
    0x337A, 0x00,
    0x337C, 0x00,
    0x33A9, 0x00,
    0x33AA, 0x00,
    0x33AC, 0x00,
    0x33AD, 0x00,
    0x33AE, 0x00,
    0x33B0, 0x00,

    0x3710, 0x07,
    0x371E, 0x02,
    0x371F, 0x02,
    0x3364, 0x0B,
    0x33BD, 0x00,
    0x33BE, 0x08,
    0x33BF, 0x10,
    0x3700, 0x00,
    0x3701, 0x0C,
    0x3702, 0x18,
    0x3703, 0x32,
    0x3704, 0x44,
    0x3705, 0x54,
    0x3706, 0x70,
    0x3707, 0x88,
    0x3708, 0x9D,
    0x3709, 0xB0,
    0x370A, 0xCF,
    0x370B, 0xE2,
    0x370C, 0xEF,
    0x370D, 0xF7,
    0x370E, 0xFF,

    0x3250, 0x35,
    0x3251, 0x21,
    0x3252, 0x30,
    0x3253, 0x10,
    0x3254, 0x3E,
    0x3255, 0x2A,
    0x3256, 0x2A,
    0x3257, 0x1B,
    0x3258, 0x4D,
    0x3259, 0x3C,
    0x325A, 0x28,
    0x325B, 0x16,
    0x325C, 0x80,
    0x325D, 0x08,

    0x3813, 0x07,
    0x32BB, 0xE7,
    0x32B8, 0x08,
    0x32B9, 0x04,
    0x32BC, 0x40,
    0x32BD, 0x04,
    0x32BE, 0x02,
    0x32BF, 0x60,
    0x32C0, 0x6A,
    0x32C1, 0x6A,
    0x32C2, 0x6A,
    0x32C3, 0x00,
    0x32C4, 0x30,
    0x32C5, 0x3F,
    0x32C6, 0x3F,
    0x32D3, 0x00,
    0x32D4, 0xE3,
    0x32D5, 0x7C,
    0x32D6, 0x00,
    0x32D7, 0xBD,
    0x32D8, 0x77,
#if 0
    0x32F0, 0x30,
#else
    //0x32E0, 0x03,
    //0x32E1, 0xC0, //scaler H   

    //0x32E0, 0x02,
    //0x32E1, 0xD0, 
    
	//0x32E2, 0x00,
	//0x32E3, 0x00,//scaler V
    0x32F0, 0x30,// out format   1280 * 720  >>0x20  out format 960*720 >>0x30
#endif
    //SOC Control Registers (0x3200)
    0x3200, 0x00,
    0x3201, 0x08,

    0x302A, 0x80,////0x84
    0x302C, 0x17,
    0x302D, 0x11,

    0x3022, 0x03,	//Mirror_Horizontal[1], Flip_Vertical[0]

    0x300A, 0x06,	//Line_Length_Pck_H
    0x300B, 0x32,	//Line_Length_Pck_L
    0x300C, 0x02,	//Frame_Length_Line_H
    0x300D, 0xF4,	//Frame_Length_Line_L

    0x301E, 0x08,
    0x3024, 0x00,
    0x334B, 0x00,
    0x3810, 0x00,
    0x320A, 0x00,
    0x3021, 0x02,

//[Expo_Gain_Ctrl1]
    //0x3012, 0x01,
    //0x3013, 0x80,
    //0x32CF, 0x70,
    //0x3060, 0x02,

//Expo_Gain_Ctrl2]
    //0x3012, 0x02,
    //0x3013, 0x00,
    //0x32CF, 0x18,
    //0x3060, 0x02,
	REG_STOP, REG_STOP,
};
#endif //INFOTM_ISP

struct stk3855dual720dvp_mode {
    IMG_UINT8 ui8Flipping;
    const IMG_UINT16 *modeRegisters;
    IMG_UINT32 nRegisters;
};

struct stk3855dual720dvp_mode stk3855dual720dvp_modes[] = {
#ifdef INFOTM_ISP
    {SENSOR_FLIP_NONE, ui16ModeRegs_1280_720_30, 0}, //0
#endif //INFOTM_ISP
};

static const IMG_UINT16 *getRegisters(STK3855DUAL720DVPCAM_STRUCT *psCam, 
    unsigned int uiMode, IMG_UINT32 *nRegisters)
{
    if(uiMode < sizeof(stk3855dual720dvp_modes)/sizeof(struct stk3855dual720dvp_mode))
    {
        const IMG_UINT16 *registerArray = stk3855dual720dvp_modes[uiMode].modeRegisters;
        if(stk3855dual720dvp_modes[uiMode].nRegisters==0)
        {
            // should be done only once per mode
            int i = 0;
            while(REG_STOP != registerArray[2*i])
            {
                i++;
            }
            stk3855dual720dvp_modes[uiMode].nRegisters = i;
        }
        *nRegisters = stk3855dual720dvp_modes[uiMode].nRegisters;
        return registerArray;
    }
    // if it is an invalid mode returns NULL

    return NULL;
}


static IMG_RESULT STK3855DUAL720DVP_ApplyMode(STK3855DUAL720DVPCAM_STRUCT *psCam, IMG_UINT16 ui16Mode,
    IMG_BOOL bFlipHorizontal, IMG_BOOL bFlipVertical);

// pui32ExposureDef is optional
static IMG_RESULT STK3855DUAL720DVP_GetModeInfo(STK3855DUAL720DVPCAM_STRUCT *psCam, IMG_UINT16 ui16Mode,
    SENSOR_MODE *info, IMG_UINT32 *pui32ExposureDef);

#ifdef INFOTM_ISP
static IMG_RESULT STK3855DUAL720DVP_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Flag);
static IMG_RESULT STK3855DUAL720DVP_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
#endif //INFOTM_ISP

static IMG_RESULT stk3855dual720dvp_i2c_write(int i2c, IMG_UINT16 offset,
    IMG_UINT16 data)
{
#ifdef INFOTM_ISP
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    uint8_t buf[3];
    buf[0] = (uint8_t)((offset >> 8) & 0xff);
    buf[1] = (uint8_t)(offset & 0xff);
    buf[2] = (uint8_t)(data & 0xff);

    messages[0].addr  = i2c_addr;
    messages[0].flags = 0;
    messages[0].len   = sizeof(buf);
    messages[0].buf   = buf;

    packets.msgs  = messages;
    packets.nmsgs = 1;

    //printf("Writing 0x%04X, 0x%02X\n", offset, data);

    if(ioctl(i2c, I2C_RDWR, &packets) < 0)
    {
        LOG_ERROR("Unable to write data: 0x%04x 0x%02x.\n", offset, data);
        return IMG_ERROR_FATAL;
    }

    usleep(2);

#else
#if !file_i2c
    IMG_UINT8 buff[3];

    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave write address!\n");
        return IMG_ERROR_BUSY;
    }

    buff[0] = offset >> 8;
    buff[1] = offset & 0xFF;
    buff[2] = data & 0xFF;

    LOG_DEBUG("Writing 0x%04x, 0x%04X\n", offset, data);
    write(i2c, buff, 4);
#endif
#endif //INFOTM_ISP
    return IMG_SUCCESS;
}

static IMG_RESULT STK3855DUAL720DVP_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    
    IMG_ASSERT(strlen(STK3855DUAL720DVP_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(STK3855DUAL720DVP_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);

    psInfo->eBayerOriginal = MOSAIC_BGGR;//MOSAIC_RGGB;
    psInfo->eBayerEnabled = MOSAIC_BGGR;//MOSAIC_RGGB;

    sprintf(psInfo->pszSensorName, STK3855DUAL720DVP_SENSOR_INFO_NAME);

    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 7500;
    // bitdepth moved to mode information
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = psCam->imager;
    psInfo->bBackFacing = IMG_TRUE;
#ifdef INFOTM_ISP
    psInfo->ui32ModeCount = sizeof(stk3855dual720dvp_modes) / sizeof(struct stk3855dual720dvp_mode);
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

static IMG_RESULT STK3855DUAL720DVP_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Mode,
    SENSOR_MODE *psModes)
{
    IMG_RESULT ret;
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    
    ret = STK3855DUAL720DVP_GetModeInfo(psCam, ui16Mode, psModes, NULL);
    if (ret != IMG_SUCCESS)
    {
        //LOG_ERROR("Failed to get information about mode %u\n", ui16Mode);
        ret = IMG_ERROR_NOT_SUPPORTED;
    }
    return ret;
}

static IMG_RESULT STK3855DUAL720DVP_GetState(SENSOR_HANDLE hHandle,
    SENSOR_STATUS *psStatus)
{
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_WARNING("sensor not initialised\n");
        psStatus->eState = SENSOR_STATE_UNINITIALISED;
        psStatus->ui16CurrentMode = 0;
    }
    else
    {
        psStatus->eState = (psCam->bEnabled ?
            SENSOR_STATE_RUNNING : SENSOR_STATE_IDLE);
        psStatus->ui16CurrentMode = psCam->ui16CurrentMode;
        psStatus->ui8Flipping = psCam->ui8CurrentFlipping;

#ifdef INFOTM_ISP
		psStatus->fCurrentFps = psCam->fCurrentFps;
#endif
    }
    return IMG_SUCCESS;
}

static IMG_RESULT STK3855DUAL720DVP_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Mode,
    IMG_UINT8 ui8Flipping)
{
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);
    IMG_RESULT ret;
    IMG_UINT32 ui32ExposureDef;

    //printf("STK3855DUAL720DVP_SetMode(%d)\n", ui16Mode);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = STK3855DUAL720DVP_GetModeInfo(psCam, ui16Mode, &(psCam->currMode), &ui32ExposureDef);
    if (IMG_SUCCESS != ret)
    {
        LOG_WARNING("failed to get information about mode %u\n", ui16Mode);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    
	system("echo 1 > /sys/class/ddk_sensor_debug/enable");
    ret = STK3855DUAL720DVP_ApplyMode(psCam, ui16Mode, 
        ui8Flipping&SENSOR_FLIP_HORIZONTAL,
        ui8Flipping&SENSOR_FLIP_VERTICAL);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to configure mode %d\n", ui16Mode);
    }
    else
    {
        psCam->ui32Exposure = ui32ExposureDef;
        psCam->flGain = 1.0;
        psCam->psSensorPhy->psGasket->bParallel = IMG_TRUE;
        psCam->psSensorPhy->psGasket->uiGasket = psCam->imager;
        psCam->psSensorPhy->psGasket->uiWidth = psCam->currMode.ui16Width;
        psCam->psSensorPhy->psGasket->uiHeight = psCam->currMode.ui16Height - 1;
        psCam->psSensorPhy->psGasket->bVSync = IMG_FALSE;
        psCam->psSensorPhy->psGasket->bHSync = IMG_FALSE;
        psCam->psSensorPhy->psGasket->ui8ParallelBitdepth = 10;//12;

        printf("psGasket->uiWidth=%d, psGasket->uiHeight=%d\n", psCam->psSensorPhy->psGasket->uiWidth, psCam->psSensorPhy->psGasket->uiHeight);


        psCam->ui16CurrentMode = ui16Mode;
        psCam->ui8CurrentFlipping = ui8Flipping;
    }
    
    return ret;
}

static IMG_RESULT STK3855DUAL720DVP_Enable(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    IMG_RESULT ret;
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    //printf("STK3855DUAL720DVP_Enable()\n");

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling STK3855DUAL720DVP Camera!\n");
    psCam->bEnabled = IMG_TRUE;
    
    // setup the sensor number differently when using multiple sensors
    ret = SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE,
        psCam->currMode.ui8MipiLanes, STK3855DUAL720DVP_PHY_NUM);
    if (ret == IMG_SUCCESS)
    {
        ret = IMG_SUCCESS;
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to enable sensor\n");
        }
        else
        {
            LOG_DEBUG("camera enabled\n");
        }
    }

	LOG_ERROR("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ before set 3855\n");
	usleep(1000000);
	system("echo 1 > /sys/class/stk3855-debug/enable");
    return ret;
}

static IMG_RESULT STK3855DUAL720DVP_DisableSensor(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);
    IMG_RESULT ret = IMG_SUCCESS;
    int delay = 0;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Disabling STK3855DUAL720DVP camera\n");
    psCam->bEnabled = IMG_FALSE;

    ret = IMG_SUCCESS;
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to disable sensor!\n");
    }

    delay = (int)floor(1.0 / (psCam->currMode.flFrameRate) * 1000 * 1000);
    LOG_DEBUG("delay of %uus between disabling sensor/phy\n", delay);
    
    // delay of a frame period to ensure sensor has stopped
    // flFrameRate in Hz, change to MHz to have micro seconds
    usleep(delay);

    SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, STK3855DUAL720DVP_PHY_NUM);
    
    return ret;
}

static IMG_RESULT STK3855DUAL720DVP_DestroySensor(SENSOR_HANDLE hHandle)
{
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Destroying STK3855DUAL720DVP camera\n");
    /* remember to free anything that might have been allocated dynamically
     * (like extended params...)*/
    if (psCam->bEnabled)
    {
        STK3855DUAL720DVP_DisableSensor(hHandle);
    }

    SensorPhyDeinit(psCam->psSensorPhy);
#if !file_i2c
    close(psCam->i2c);
#endif
    IMG_FREE(psCam);
    return IMG_SUCCESS;
}

static IMG_RESULT STK3855DUAL720DVP_GetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current = psCam->ui32Exposure;
    return IMG_SUCCESS;
}


static IMG_RESULT STK3855DUAL720DVP_SetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
    //printf("------------------->>> SetExposure func is set\n");

    IMG_UINT16 aui16Regs[] = {
        0x3012, 0x00,
        0x3013, 0x01,
        0x3060, 0x02,
    };
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT32 ui32Lines;
	SENSOR_MODE info;
	
	STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    ret = STK3855DUAL720DVP_GetModeInfo(psCam, psCam->ui16CurrentMode, &info, NULL);


    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui32Exposure = ui32Current;

	ui32Lines    = psCam->ui32Exposure / info.ui32ExposureMin;
    aui16Regs[1] = (IMG_UINT8)(ui32Lines >> 8);
    aui16Regs[3] = (IMG_UINT8)(ui32Lines & 0xff);
	ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[2], aui16Regs[3]);
    ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[4], aui16Regs[5]);

	//printf("STK3855DUAL720DVP_SetExposure psCam->ui32Exposure=%d, info.ui32ExposureMin=%f, ui32Lines=%d\n",
	//	psCam->ui32Exposure,
	//	info.ui32ExposureMin,
	//	ui32Lines);


	return ret;
}

static IMG_RESULT STK3855DUAL720DVP_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *pui8Contexts)
{
    *pflMin = 1.0;
    *pflMax = 8.0;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

static IMG_RESULT STK3855DUAL720DVP_GetGain(SENSOR_HANDLE hHandle, double *pflGain,
    IMG_UINT8 ui8Context)
{
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain = psCam->flGain;
    return IMG_SUCCESS;
}

static IMG_RESULT STK3855DUAL720DVP_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
    //printf("------------------->>> SetGain func is set\n");

	double aflRegs[16]= {
        1.0625,
        1.1250,
        1.1875,
        1.2500,
        1.3125,
        1.3750,
        1.4375,
        1.5000,
        1.5625,
        1.6250,
        1.6875,
        1.7500,
        1.8125,
        1.8750,
        1.9375,
        2.0000,
    };

    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT16 tmp = 0, nIndex = 0, val = 0x10;
    IMG_UINT16 aui16Regs[2];
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

	psCam->flGain = flGain;

    while (flGain >= 2.0)
    {
        tmp = tmp | val; 
        flGain = flGain / 2.0;
        val = val << 1;
    }
    while(flGain > aflRegs[nIndex])
    {
        nIndex++;
    }
    tmp = tmp + nIndex;

    aui16Regs[0] = 0x32cf;
    aui16Regs[1] = tmp;

	ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[0], aui16Regs[1]);

	return ret;
}

static IMG_RESULT STK3855DUAL720DVP_GetExposureRange(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui32Min = psCam->currMode.ui32ExposureMin;
    *pui32Max = psCam->currMode.ui32ExposureMax;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

IMG_RESULT STK3855DUAL720DVP_Create(SENSOR_HANDLE *phHandle, int index)
{
#if !file_i2c
        STK3855DUAL720DVPCAM_STRUCT *psCam;
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
	psCam = (STK3855DUAL720DVPCAM_STRUCT *)IMG_CALLOC(1, sizeof(STK3855DUAL720DVPCAM_STRUCT));
	if (!psCam)
		return IMG_ERROR_MALLOC_FAILED;

	*phHandle = &psCam->sFuncs;
	psCam->sFuncs.GetMode = STK3855DUAL720DVP_GetMode;
	psCam->sFuncs.GetState = STK3855DUAL720DVP_GetState;
	psCam->sFuncs.SetMode = STK3855DUAL720DVP_SetMode;
    psCam->sFuncs.Enable = STK3855DUAL720DVP_Enable;
    psCam->sFuncs.Disable = STK3855DUAL720DVP_DisableSensor;
    psCam->sFuncs.Destroy = STK3855DUAL720DVP_DestroySensor;
    
    psCam->sFuncs.GetGainRange = STK3855DUAL720DVP_GetGainRange;
    psCam->sFuncs.GetCurrentGain = STK3855DUAL720DVP_GetGain;
    psCam->sFuncs.SetGain = STK3855DUAL720DVP_SetGain;

    psCam->sFuncs.GetExposureRange = STK3855DUAL720DVP_GetExposureRange;
    psCam->sFuncs.GetExposure = STK3855DUAL720DVP_GetExposure;
    psCam->sFuncs.SetExposure = STK3855DUAL720DVP_SetExposure;
    
    psCam->sFuncs.GetInfo = STK3855DUAL720DVP_GetInfo;
    
#ifdef INFOTM_ISP
    psCam->sFuncs.SetFlipMirror = STK3855DUAL720DVP_SetFlipMirror;
	psCam->sFuncs.SetGainAndExposure = STK3855DUAL720DVP_SetGainAndExposure;
#endif //INFOTM_ISP

    psCam->bEnabled = IMG_FALSE;
    psCam->ui16CurrentMode = 0;
    psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    psCam->refClock = 24.0 * 1000 * 1000;
    psCam->imager = imgr;

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
#endif

    ret = STK3855DUAL720DVP_GetModeInfo(psCam, psCam->ui16CurrentMode, &(psCam->currMode),
        &(psCam->ui32Exposure));
    if(IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get initial mode information!\n");
#if !file_i2c
        close(psCam->i2c);
#endif
        IMG_FREE(psCam);
        return IMG_ERROR_FATAL;
    }

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

    return IMG_SUCCESS;
}

static IMG_RESULT STK3855DUAL720DVP_ApplyMode(STK3855DUAL720DVPCAM_STRUCT *psCam, IMG_UINT16 ui16Mode,
    IMG_BOOL bFlipHorizontal, IMG_BOOL bFlipVertical)
{
    int n;
    IMG_RESULT ret = IMG_SUCCESS;

    const IMG_UINT16 *registerArray = NULL;
    int nRegisters = 0;

    registerArray = getRegisters(psCam, ui16Mode, &nRegisters);
    if(!registerArray || nRegisters==0)
    {
        LOG_ERROR("failed to get registers for mode %u\n", ui16Mode);
        return IMG_ERROR_FATAL;
    }
    
    LOG_DEBUG("mode %u has %u registers\n", ui16Mode, nRegisters);
    
    for (n = 0; n < 2*nRegisters && IMG_SUCCESS==ret; n += 2)
    {
        IMG_UINT16 values[2];
        
        values[0] = registerArray[n];   // register
        values[1] = registerArray[n+1]; // value
        
        ret = stk3855dual720dvp_i2c_write(psCam->i2c, values[0], values[1]);
		if(ret != IMG_SUCCESS)
		{
			return ret;
		}
    }
    
    if (!ret)
    {
        LOG_DEBUG("Sensor ready to go\n");
        usleep(1000);
    }
    else
    {
        LOG_ERROR("failed to setup register %d/%d\n", n, nRegisters*2);
    }
    
    return ret;
}

static IMG_RESULT STK3855DUAL720DVP_GetModeInfo(STK3855DUAL720DVPCAM_STRUCT *psCam, IMG_UINT16 ui16Mode,
    SENSOR_MODE *info, IMG_UINT32 *pui32ExposureDef)
{
    /* line_length_pck in pixels, clk_pix in Hz, trow in micro seconds 
     * (time in a row)
     * trow = (line_length_pck / clk_pix) * 1000 * 1000;*/
    double trow = 88.11111;
    IMG_UINT32 framelenghtlines = 756;


    const IMG_UINT16 *registerArray = NULL;
    int nRegisters = 0;
    registerArray = getRegisters(psCam, ui16Mode, &nRegisters);
    if(!registerArray || nRegisters==0)
    {
        //LOG_ERROR("failed to get registers for mode %u\n", ui16Mode);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    {
        info->ui16Width = 720 * 2;//1280*2;
        info->ui16Height = 720;//720;
        info->ui16VerticalTotal = framelenghtlines;
        info->ui8SupportFlipping = SENSOR_FLIP_NONE;
        //info->ui32ExposureMax = (IMG_UINT32)floor(trow * framelenghtlines);
        info->ui32ExposureMax = 40000;
        info->flFrameRate = 1200000.0/info->ui32ExposureMax;
        /* Minimum exposure time in us (line period) (use the frmae length 
         * lines entry to calculate this...*/
        info->ui32ExposureMin = (IMG_UINT32)floor(trow);
        info->ui8MipiLanes = 0;
        info->ui8BitDepth = 10;
        
        if(pui32ExposureDef)
        {
            /* should be set based on the coarse integration time x line lenght
             * in us.*/
            *pui32ExposureDef = info->ui32ExposureMin * 720;
        }
    }

#if defined(INFOTM_ISP)
	psCam->fCurrentFps = info->flFrameRate - 8; //is not sure why only 22fps
//	printf("the flFrameRate = %f\n", info->flFrameRate);
	psCam->fixedFps = psCam->fCurrentFps;
	psCam->initClk = framelenghtlines;
#endif
	return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
static IMG_RESULT STK3855DUAL720DVP_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Flag)
{
	return IMG_SUCCESS;
}

typedef struct _stk385dual720DVP_gain_ {
	double flGain;
	IMG_UINT16 ui8GainRegValHi;
	IMG_UINT16 ui8GainRegValLo;
} STK3855DUAL720DVP_GAIN_TABLE;

STK3855DUAL720DVP_GAIN_TABLE asSTK3855DUAL720DVPGainValues[]=
{
	{ 1,	  0x00,	0x00 },
	{ 1.0625, 0x00,	0x01 },
	{ 1.125,  0x00,	0x02 },
	{ 1.1875, 0x00,	0x03 },
	{ 1.25,   0x00,	0x04 },
	{ 1.3125, 0x00,	0x05 },
	{ 1.375,  0x00,	0x06 },
	{ 1.4375, 0x00,	0x07 },
	{ 1.5,	  0x00,	0x08 },
	{ 1.5625, 0x00,	0x09 },
	{ 1.625,  0x00,	0x0A },
	{ 1.6875, 0x00,	0x0B },
	{ 1.75,   0x00,	0x0C },
	{ 1.8125, 0x00,	0x0D },
	{ 1.875,  0x00,	0x0E },
	{ 1.9375, 0x00,	0x0F },
	{ 2,	  0x00,	0x10 },	//2x
	{ 2.125,  0x00,	0x11 },
	{ 2.25,   0x00,	0x12 },
	{ 2.375,  0x00,	0x13 },
	{ 2.5,	  0x00,	0x14 },
	{ 2.625,  0x00,	0x15 },
	{ 2.75,   0x00,	0x16 },
	{ 2.875,  0x00,	0x17 },
	{ 3,	  0x00,	0x18 },
	{ 3.125,  0x00,	0x19 },
	{ 3.25,   0x00,	0x1A },
	{ 3.375,  0x00,	0x1B },
	{ 3.5,	  0x00,	0x1C },
	{ 3.625,  0x00,	0x1D },
	{ 3.75,   0x00,	0x1E },
	{ 3.875,  0x00,	0x1F },
	{ 4,	  0x00,	0x30 },	//4x
	{ 4.25,   0x00,	0x31 },
	{ 4.5,	  0x00,	0x32 },
	{ 4.75,   0x00,	0x33 },
	{ 5,	  0x00,	0x34 },
	{ 5.25,   0x00,	0x35 },
	{ 5.5,	  0x00,	0x36 },
	{ 5.75,   0x00,	0x37 },
	{ 6,	  0x00,	0x38 },
	{ 6.25,   0x00,	0x39 },
	{ 6.5,	  0x00,	0x3A },
	{ 6.75,   0x00,	0x3B },
	{ 7,	  0x00,	0x3C },
	{ 7.25,   0x00,	0x3D },
	{ 7.5,	  0x00,	0x3E },
	{ 7.75,   0x00,	0x3F },
	{ 8,	  0x00,	0x70 },	//8x
	{ 8.5,	  0x00,	0x71 },
	{ 9,	  0x00,	0x72 },
	{ 9.5,	  0x00,	0x73 },
	{ 10,	  0x00,	0x74 },
	{ 10.5,   0x00,	0x75 },
	{ 11,	  0x00,	0x76 },
	{ 11.5,   0x00,	0x77 },
	{ 12,	  0x00,	0x78 },
	{ 12.5,   0x00,	0x79 },
	{ 13,	  0x00,	0x7A },
	{ 13.5,   0x00,	0x7B },
	{ 14,	  0x00,	0x7C },
	{ 14.5,   0x00,	0x7D },
	{ 15.5,   0x00,	0x7F },
	{ 16,	  0x01,	0x70 },	//16x
	{ 17,	  0x01,	0x71 },
	{ 18,	  0x01,	0x72 },
	{ 19,	  0x01,	0x73 },
	{ 20,	  0x01,	0x74 },
	{ 21,	  0x01,	0x75 },
	{ 22,	  0x01,	0x76 },
	{ 23,	  0x01,	0x77 },
	{ 24,	  0x01,	0x78 },
	{ 25,	  0x01,	0x79 },
	{ 26,	  0x01,	0x7A },
	{ 27,	  0x01,	0x7B },
	{ 28,	  0x01,	0x7C },
	{ 29,	  0x01,	0x7D },
	{ 30,	  0x01,	0x7E },
	{ 31,	  0x01,	0x7F },
	{ 32,	  0x03,	0x70 },	//32x
	{ 34,	  0x03,	0x71 },
	{ 36,	  0x03,	0x72 },
	{ 38,	  0x03,	0x73 },
	{ 40,	  0x03,	0x74 },
	{ 42,	  0x03,	0x75 },
	{ 44,	  0x03,	0x76 },
	{ 46,	  0x03,	0x77 },
	{ 48,	  0x03,	0x78 },
	{ 50,	  0x03,	0x79 },
	{ 52,	  0x03,	0x7A },
	{ 54,	  0x03,	0x7B },
	{ 56,	  0x03,	0x7C },
	{ 58,	  0x03,	0x7D },
	{ 60,	  0x03,	0x7E },
	{ 62,	  0x03,	0x7F }
};

static IMG_UINT16 g_TK3855DUAL720DVP_ui8LinesValHiBackup = 0xff;
static IMG_UINT16 g_TK3855DUAL720DVP_ui8LinesValLoBackup = 0xff;
static IMG_UINT16 g_TK3855DUAL720DVP_ui8GainValHiBackup = 0xff;
static IMG_UINT16 g_TK3855DUAL720DVP_ui8GainValLoBackup = 0xff;
static int g_count = -1;
static IMG_RESULT STK3855DUAL720DVP_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    //return IMG_SUCCESS;

    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;
    unsigned int nIndex;
    IMG_UINT32 ui32Lines;
    IMG_UINT16 aui16Regs[] = {
        0x3012, 0x00,	//exposure 1 line
        0x3013, 0x01,
        0x301C, 0x00,	//gain 1x
        0x301D, 0x00,
        0x3060, 0x02
    };

    //Eason
    g_count ++;
    if (g_count > 0)
    {
        g_count = -1;
        return IMG_SUCCESS;
    }


    STK3855DUAL720DVPCAM_STRUCT *psCam = container_of(hHandle, STK3855DUAL720DVPCAM_STRUCT, sFuncs);
    ret = STK3855DUAL720DVP_GetModeInfo(psCam, psCam->ui16CurrentMode, &info, NULL);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }


    //01.Exposure part
    //
    psCam->ui32Exposure = ui32Exposure;

    ui32Lines = psCam->ui32Exposure / info.ui32ExposureMin;

    //Eason
    if (ui32Lines >= info.ui16VerticalTotal)
    {
        ui32Lines = info.ui16VerticalTotal - 2;
        //printf("ui32Lines >= VerticalTotal, set to %d\n", ui32Lines);
    }
    aui16Regs[1] = (IMG_UINT8)(ui32Lines >> 8);
    aui16Regs[3] = (IMG_UINT8)(ui32Lines & 0xff);

	//printf("psCam->ui32Exposure=%d, info.ui32ExposureMin=%f, ui32Lines=%d\n",
	//	psCam->ui32Exposure,
	//	info.ui32ExposureMin,
	//	ui32Lines);


    //02.Gain part
    //
    psCam->flGain = flGain;
    nIndex = 0;

    /*
     * First search the gain table for the lowest analog gain we can use,
     * then use the digital gain to get more accuracy/range.
     */
    while (nIndex < (sizeof(asSTK3855DUAL720DVPGainValues) / sizeof(STK3855DUAL720DVP_GAIN_TABLE)))
    {
        if (asSTK3855DUAL720DVPGainValues[nIndex].flGain > flGain)
            break;
        nIndex++;
    }

    if (nIndex > 0)
        nIndex = nIndex - 1;
    
    aui16Regs[5] = asSTK3855DUAL720DVPGainValues[nIndex].ui8GainRegValHi;
    aui16Regs[7] = asSTK3855DUAL720DVPGainValues[nIndex].ui8GainRegValLo;

	//printf("STK3855DUAL720DVP_SetGain flGian = %f, Gain = 0x%02X, 0x%02X\n",
	//	psCam->flGain,
	//	asSTK3855DUAL720DVPGainValues[nIndex].ui8GainRegValHi,
	//	asSTK3855DUAL720DVPGainValues[nIndex].ui8GainRegValLo);


    //03.check the new gain & exposure is the same as the last set or not.
    //
    if ( (g_TK3855DUAL720DVP_ui8LinesValHiBackup == aui16Regs[1]) &&
         (g_TK3855DUAL720DVP_ui8LinesValLoBackup == aui16Regs[3]) &&
         (g_TK3855DUAL720DVP_ui8GainValHiBackup == aui16Regs[5]) &&
         (g_TK3855DUAL720DVP_ui8GainValLoBackup == aui16Regs[7]) )
    {
        return IMG_SUCCESS;
    }

    g_TK3855DUAL720DVP_ui8LinesValHiBackup = aui16Regs[1];
    g_TK3855DUAL720DVP_ui8LinesValLoBackup = aui16Regs[3];
    g_TK3855DUAL720DVP_ui8GainValHiBackup = aui16Regs[5];
    g_TK3855DUAL720DVP_ui8GainValLoBackup = aui16Regs[7];


    //04.set to sensor
    //
    ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[2], aui16Regs[3]);
    ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[4], aui16Regs[5]);
    ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[6], aui16Regs[7]);
    ret = stk3855dual720dvp_i2c_write(psCam->i2c, aui16Regs[8], aui16Regs[9]);

    return IMG_SUCCESS;
}
#endif //INFOTM_ISP
