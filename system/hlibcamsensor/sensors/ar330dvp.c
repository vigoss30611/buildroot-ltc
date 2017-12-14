/**
*******************************************************************************
@file ar330dvp.c

@brief AR330DVP camera implementation

To add a mode to the camera:
@li create an array of ui16 with registers (such as ui16ModeRegs_0)
@warning the array must finish with REG_STOP, REG_STOP
@li add an entry in ar330dvp_modes[] with correct values for flipping and number
of registers to 0

@note The first time getRegisters() is called the number of registers is
computed by searching for REG_STOP

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

******************************************************************************/

#include "sensors/ar330dvp.h"

#include <img_types.h>
#include <img_errors.h>

#include <ci/ci_api_structs.h> // access to CI_GASKET
#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#ifdef INFOTM_ISP
#include <linux/i2c.h>
#include "sensors/ddk_sensor_driver.h"
#endif //INFOTM_ISP

/* disabled as i2c drivers locks up if device is not present */
#define NO_DEV_CHECK

#define LOG_TAG "AR330DVP_SENSOR"
#include <felixcommon/userlog.h>

#define AR330DVP_WRITE_ADDR (0x20 >> 1)
#define AR330DVP_READ_ADDR (0x21 >> 1)

/** @brief Sensor number on IMG PHY */
#define AR330DVP_PHY_NUM 0

#define AR330DVP_SENSOR_VERSION "not-verified"  // until read from sensor
/** @brief expected value for CHIP_VERSION register */
#define AR330DVP_CHIP_VERSION 0x2604

#ifdef INFOTM_ISP
#define DEV_PATH    ("/dev/ddk_sensor")
#define EXTRA_CFG_PATH  ("/root/.ispddk/")
static IMG_UINT32 i2c_addr = 0;
static IMG_UINT32 imgr = 0;
static char extra_cfg[64];
#endif
typedef struct _ar330dvpcam_struct
{
    SENSOR_FUNCS sFuncs;
    
    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
    IMG_UINT8 ui8CurrentFlipping;
    IMG_BOOL bEnabled;
    IMG_UINT8 imager;

    SENSOR_MODE currMode;
    IMG_UINT32 ui32ExposureMax;
    IMG_UINT32 ui32Exposure;
    IMG_UINT32 ui32ExposureMin;
    IMG_DOUBLE dbExposureMin;
    double flGain;
    
    int i2c;
    SENSOR_PHY *psSensorPhy;
#ifdef INFOTM_ISP
    double fCurrentFps;
    IMG_UINT32 fixedFps;
    IMG_UINT32 initClk;
#endif

}AR330DVPCAM_STRUCT;

#define COARSE_INTEGRATION_TIME 0x3012
#define SERIAL_FORMAT 0x31AE
#define FRAME_LENGTH_LINES 0x300A
#define READ_MODE 0x3040
#define LINE_LENGTH_PCK 0x300C
#define VT_PIX_CLK_DIV 0x302A
#define VT_SYS_CLK_DIV 0x302C
#define PLL_MULTIPLIER 0x3030
#define PRE_PLL_CLK_DIV 0x302E
#define REVISION_NUMBER 0x300E
#define TEST_DATA_RED 0x3072
#define DATAPATH_STATUS 0x306A
#define FRAME_COUNT 0x303A
#define RESET_REGISTER 0x301A
#define FLASH 0x3046
#define FLASH2 0x3048
#define GLOBAL_GAIN 0x305E
#define ANALOG_GAIN 0x3060
#define X_ADDR_START 0x3004
#define X_ADDR_END 0x3008
#define Y_ADDR_START 0x3002
#define Y_ADDR_END 0x3006
#define DATA_FORMAT_BITS 0x31AC
#define CHIP_VERSION 0x3000

static SENSOR_MODE asModes[] = {
        {10, 1920, 1088, 30.0f, 1125, 2254, 1242, SENSOR_FLIP_BOTH},
        {10, 1920, 1088, 15.0f, 1125, 2280, 1242, SENSOR_FLIP_BOTH},
};


#ifdef INFOTM_ISP
static IMG_UINT16 ui16ModeRegs_0[] =
{

		0x301A, 0x10DD,
		//0x31AE, 0x0301,
		0x301A, 0x0058,
		//0x301A, 0x0058,
		0x301A, 0x00D8,
		0x301A, 0x10D8,
		0x3064, 0x1802,
		0x3078, 0x0001,
		0x3046, 0x4038,
		0x3048, 0x8480,
		0x3ED2, 0x0146,
		0x3ED6, 0x66CC,
		0x3ED8, 0x8C42,
		0x302A, 0x0006,
		0x302C, 0x0001,
		#if !defined(NEW_SETTING)
		0x302E, 0x0002,
		0x3030, 0x0020,
		#else
		0x302E, 0x0003,
		0x3030, 0x003c,
		#endif
		0x3036, 0x000C,
		0x3038, 0x0001,
		0x301A, 0x10D8,
		0x301A, 0x10DC,
		0x306E, 0x9C10,
		0x306E, 0xFC00,
		#if !defined(NEW_SETTING)
		0x3012, 0x0010,  //ExposureLine
		//0x3012, 0x0189,  //ExposureLine
		//0x3012, 0x01C3,  //ExposureLine
		//0x3012, 0x0460,  //ExposureLine
		//0x3012, 0x051B,  //ExposureLine
		#else
		0x3012, 0x0010,  //ExposureLine
		//0x3012, 0x03C0,  //ExposureLine
		#endif
		0x301A, 0x10DC,
		0x301A, 0x10D8,
		0x301A, 0x10D8,
		0x301A, 0x10DC,
		0x31AE, 0x0301,
		0x31E0, 0x0703,
		#if 1
		0x3004, 0x00c6,  // X_ADDR_START
		0x3008, 0x0845,  // X_ADDR_END
		0x3002, 0x00ea,  // Y_ADDR_START
		//0x3006, 0x0521,  // Y_ADDR_END
		0x3006, 0x0529,  // Y_ADDR_END
		#else
		0x3004, 0x00C0,  // X_ADDR_START
		0x3002, 0x00E4,  // Y_ADDR_START
		0x3008, 0x083F,  // X_ADDR_END
		0x3006, 0x0523,  // Y_ADDR_END
		#endif
		0x30A2, 0x0001,
		0x30A6, 0x0001,
		0x3040, 0x0000,// | 0x8000 | 0x4000,
		#if !defined(NEW_SETTING)
		0x300C, 0x04da,  // npixles
		0x300A, 0x0467,  // lines
		#else
		//0x300C, 0x044c,  // npixles
		0x300C, 0x04B4,  // npixles
		//0x300C, 0x0500,  // npixles
		//0x300A, 0x0411,  // lines
		0x300A, 0x0453,  // lines
		//0x300A, 0x0456,  // lines
		//0x300A, 0x04bc,  // lines
		#endif
		0x3014, 0x0000,
		0x30BA, 0x002C,
		//0x305E, 0x0085,  // Digital Gain,
		//0x3060, 0x20 | 0x0A,  // Analog Gain
		//0x3042, 0x010A,
		0x301A, 0x10D8,
		0x3088, 0x80BA,
		0x3086, 0x0253,
		//0x301A, 0x10DC,
};

static IMG_UINT16 ui16ModeRegs_1[] = {
    0x301A, 0x10D8,     //RESET_REGISTER = 4312
    0x302A, 0x0006,     //VT_PIX_CLK_DIV = 6
    0x302C, 0x0001,     //VT_SYS_CLK_DIV = 1
    0x302E, 0x0004,     //PRE_PLL_CLK_DIV = 4
    0x3030, 0x0055,     //PLL_MULTIPLIER = 85
    0x3036, 0x000C,     //OP_PIX_CLK_DIV = 12
    0x3038, 0x0001,     //OP_SYS_CLK_DIV = 1
    0x31AC, 0x0C0C,     //DATA_FORMAT_BITS = 3084
    0x31AE, 0x0301,     //SERIAL_FORMAT = 769
#if 0
    0x3002, 0x00E6,     //Y_ADDR_START = 230
    0x3004, 0x00C6,     //X_ADDR_START = 198
    0x3006, 0x0525,     //Y_ADDR_END = 1317
    0x3008, 0x0845,     //X_ADDR_END = 2117
#else
    0x3002, 0x00ea,  // Y_ADDR_START
    0x3004, 0x00c6,  // X_ADDR_START
    0x3006, 0x0529/*0x0521*/,  // Y_ADDR_END
    0x3008, 0x0845,  // X_ADDR_END
#endif
    0x300A, 0x0474,     //FRAME_LENGTH_LINES = 1140
    0x300C, 0x04DA,     //LINE_LENGTH_PCK = 1242
    0x3012, 0x023A,     //COARSE_INTEGRATION_TIME = 570
    0x3014, 0x0000,     //FINE_INTEGRATION_TIME = 0
    0x30A2, 0x0001,     //X_ODD_INC = 1
    0x30A6, 0x0001,     //Y_ODD_INC = 1
    0x3040, 0x0000,     //READ_MODE = 0
    0x3042, 0x0312,     //EXTRA_DELAY = 786
    0x30BA, 0x00AC,     //DIGITAL_CTRL = 172
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
#endif //INFOTM_ISP

#ifdef INFOTM_ISP
static IMG_RESULT doStaticSetup(AR330DVPCAM_STRUCT *psCam, IMG_INT16 ui16Width,
        IMG_UINT16 ui16Height, IMG_UINT16 ui16Mode, IMG_BOOL bFlipHorizontal,
        IMG_BOOL bFlipVertical);
static IMG_RESULT AR330DVP_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Flag);
static IMG_RESULT AR330DVP_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
static IMG_RESULT AR330DVP_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed);
static IMG_RESULT AR330DVP_SetFPS(SENSOR_HANDLE hHandle, double fps);
static IMG_RESULT AR330DVP_Reset(SENSOR_HANDLE hHandle);
#endif //INFOTM_ISP

static IMG_RESULT ar330dvp_i2c_write16(int i2c, IMG_UINT16 offset,
    IMG_UINT16 data)
{
#ifdef INFOTM_ISP
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    uint8_t buf[4];
    buf[0] = (uint8_t)((offset >> 8) & 0xff);
    buf[1] = (uint8_t)(offset & 0xff);
    buf[2] = (uint8_t)((data >> 8) & 0xff);
    buf[3] = (uint8_t)(data & 0xff);

    messages[0].addr  = i2c_addr;
    messages[0].flags = 0;
    messages[0].len   = sizeof(buf);
    messages[0].buf   = buf;

    packets.msgs  = messages;
    packets.nmsgs = 1;

    //printf("Writing 0x%04X, 0x%04X\n", offset, data);

    if(ioctl(i2c, I2C_RDWR, &packets) < 0)
    {
        LOG_ERROR("Unable to write data: 0x%04x 0x%04x.\n", offset, data);
        return IMG_ERROR_FATAL;
    }

    usleep(2);

#else
#if !file_i2c
    IMG_UINT8 buff[4];

    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave write address!\n");
        return IMG_ERROR_BUSY;
    }

    /* Prepare 8-bit data using 16-bit input data. */
    /*for (i = 0; i < len; ++i)
    {
        *buf_p++ = data[i] >> 8;
        *buf_p++ = data[i] & 0xff;
    }
    
    write(i2c, buf, len * sizeof(*data));*/
    buff[0] = offset >> 8;
    buff[1] = offset & 0xFF;
    buff[2] = data >> 8;
    buff[3] = data & 0xFF;

    LOG_DEBUG("Writing 0x%04x, 0x%04X\n", offset, data);
    write(i2c, buff, 4);
#endif
#endif //INFOTM_ISP
    return IMG_SUCCESS;
}

static IMG_RESULT ar330dvp_i2c_read16(int i2c, IMG_UINT16 offset,
    IMG_UINT16 *data)
{
#ifdef INFOTM_ISP
    uint8_t buf[2];

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    buf[0] = (uint8_t)((offset >> 8) & 0xff);
    buf[1] = (uint8_t)(offset & 0xff);

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

    if(ioctl(i2c, I2C_RDWR, &packets) < 0)
    {
        LOG_ERROR("Unable to read data: 0x%04X.\n", offset);
        return IMG_ERROR_FATAL;
    }

    *data = (buf[0] << 8) + buf[1];

    //printf("Reading 0x%04X = 0x%04X\n", offset, *data);
    usleep(2);

#else
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];

    IMG_ASSERT(data);  // null pointer forbidden

    if (ioctl(i2c, I2C_SLAVE, i2c_addr))
    {
        LOG_ERROR("Failed to write I2C slave read address!\n", offset);
        return IMG_ERROR_BUSY;
    }

    buff[0] = offset >> 8;
    buff[1] = offset & 0xFF;
    if (sizeof(buff) > 2)
    {
        buff[2] = 0;
        buff[3] = 0;
    }

    LOG_DEBUG("Reading 0x%04x\n", offset);

    // write to set the address
    ret = write(i2c, buff, sizeof(buff));
    if (sizeof(buff) != ret)
    {
        LOG_WARNING("Wrote %dB instead of %luB before reading\n",
            ret, sizeof(buff));
    }

    // read to get the data
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
#endif //INFOTM_ISP
    return IMG_SUCCESS;

}

static IMG_RESULT AR330DVP_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{

    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    
    IMG_ASSERT(strlen(AR330DVP_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(AR330DVP_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);

    psInfo->eBayerOriginal = MOSAIC_GRBG;
    psInfo->eBayerEnabled = MOSAIC_GRBG;

    // because it seems AR330DVP conserves same pattern - otherwise uncomment
    /*if (SENSOR_FLIP_NONE!=psInfo->sStatus.ui8Flipping)
    {
        psInfo->eBayerEnabled = MosaicFlip(psInfo->eBayerOriginal,
            psInfo->sStatus.ui8Flipping&SENSOR_FLIP_HORIZONTAL?1:0,
            psInfo->sStatus.ui8Flipping&SENSOR_FLIP_VERTICAL?1:0);
    }*/

    sprintf(psInfo->pszSensorName, AR330DVP_SENSOR_INFO_NAME);
    /// @ could we read that from sensor?
    //psInfo->ui8BitDepth = 10;
    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 7500;
    // bitdepth moved to mode information
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = imgr;
    psInfo->bBackFacing = IMG_TRUE;

    return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 index,
    SENSOR_MODE *psModes)
{
    if(index < sizeof(asModes) / sizeof(SENSOR_MODE)) {
           IMG_MEMCPY(psModes, &(asModes[index]), sizeof(SENSOR_MODE));
           return IMG_SUCCESS;
       }

       return IMG_ERROR_NOT_SUPPORTED;
}

static IMG_RESULT AR330DVP_GetState(SENSOR_HANDLE hHandle,
    SENSOR_STATUS *psStatus)
{
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

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

static IMG_RESULT AR330DVP_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Mode,
    IMG_UINT8 ui8Flipping)
{
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);
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

static IMG_RESULT AR330DVP_Enable(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    IMG_RESULT ret;
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

    //printf("AR330DVP_Enable()\n");

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling AR330DVP Camera!\n");
    psCam->bEnabled = IMG_TRUE;
    
    aui16Regs[0] = RESET_REGISTER;
    aui16Regs[1] = 0x10DC;
    
    // setup the sensor number differently when using multiple sensors
    ret = SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE,
        psCam->currMode.ui8MipiLanes, AR330DVP_PHY_NUM);
    if (ret == IMG_SUCCESS)
    {
        ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to enable sensor\n");
        }
        else
        {
            //IMG_UINT16 reg = 0;
            LOG_DEBUG("camera enabled\n");


        }
    }

    return ret;
}

static IMG_RESULT AR330DVP_DisableSensor(SENSOR_HANDLE hHandle)
{
    IMG_UINT16 aui16Regs[2];
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);
    IMG_RESULT ret = IMG_SUCCESS;
    int delay = 0;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Disabling AR330DVP camera\n");
    psCam->bEnabled = IMG_FALSE;

    aui16Regs[0] = RESET_REGISTER;
    aui16Regs[1] = 0x10D8;

    ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to disable sensor!\n");
    }

    delay = 100;
    LOG_DEBUG("delay of %uus between disabling sensor/phy\n", delay);
    
    // delay of a frame period to ensure sensor has stopped
    // flFrameRate in Hz, change to MHz to have micro seconds
    usleep(delay);

    SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, AR330DVP_PHY_NUM);
    
    return ret;
}

static IMG_RESULT AR330DVP_DestroySensor(SENSOR_HANDLE hHandle)
{
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Destroying AR330DVP camera\n");

    /* remember to free anything that might have been allocated dynamically
     * (like extended params...)*/
    if (psCam->bEnabled)
    {
        AR330DVP_DisableSensor(hHandle);
    }

    SensorPhyDeinit(psCam->psSensorPhy);
#if !file_i2c
    close(psCam->i2c);
#endif
    IMG_FREE(psCam);
    return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_ConfigureFlash(SENSOR_HANDLE hHandle,
    IMG_BOOL bAlwaysOn, IMG_INT16 i16FrameDelay, IMG_INT16 i16Frames,
    IMG_UINT16 ui16FlashPulseWidth)
{
    IMG_UINT16 aui16Regs[2];
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);
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

    aui16Regs[0] = FLASH;
    aui16Regs[1] = 
        (IMG_UINT16)(bAlwaysOn?((1<<8)|(7<<3)):(i16Frames<<3)|i16FrameDelay);
        
    ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to write flash register\n");
        return ret;
    }
#ifdef INFOTM_ENABLE_AE_DEBUG
    printf("AR330DVP_ConfigureFlash: Global Gain 0x%04X = 0x%04X\n", aui16Regs[0], aui16Regs[1]);
#endif

            
    aui16Regs[0] = FLASH2;
    aui16Regs[1] = ui16FlashPulseWidth;
    
    ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to write flash2 register\n");
    }
    return ret;
}

static IMG_RESULT AR330DVP_GetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current = psCam->ui32Exposure;
    return IMG_SUCCESS;
}


static IMG_RESULT AR330DVP_SetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
    IMG_UINT16 aui16Regs[2];
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui32Exposure = ui32Current;

    aui16Regs[0] = COARSE_INTEGRATION_TIME; // COARSE_INTEGRATION_TIME
    aui16Regs[1] = psCam->ui32Exposure / psCam->ui32ExposureMin;
    
    ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#ifdef INFOTM_ENABLE_AE_DEBUG
	printf("AR330DVP_SetExposure: Exposure = 0x%08X, ExposureMin = 0x%08X\n", psCam->ui32Exposure, psCam->currMode.ui32ExposureMin);
	printf("AR330DVP_SetExposure: Change COARSE_INTEGRATION_TIME 0x3012 = 0x%04X\n", aui16Regs[1]);
#endif //INFOTM_ENABLE_AE_DEBUG

    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set exposure\n");
    }

    return ret;
}

static IMG_RESULT AR330DVP_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    *pflMin = 1.0;// / 128;
    *pflMax = 16.0;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_GetGain(SENSOR_HANDLE hHandle, double *pflGain,
    IMG_UINT8 ui8Context)
{
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

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
}AR330DVP_GAINTABLE;

AR330DVP_GAINTABLE asAR330DVPGainValues[]=
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

static IMG_RESULT AR330DVP_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
    IMG_UINT16 aui16Regs[2];
    unsigned int nIndex = 0;
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->flGain = flGain;

    /*
        * First search the gain table for the lowest analog gain we can use,
        * then use the digital gain to get more accuracy/range.
        */
    while(nIndex < (sizeof(asAR330DVPGainValues) / sizeof(AR330DVP_GAINTABLE)) - 1)
    {
        if(asAR330DVPGainValues[nIndex].flGain > flGain)
            break;
        nIndex++;
    }

    if(nIndex > 0)
    {
        nIndex = nIndex - 1;
    }


    flGain = psCam->flGain / asAR330DVPGainValues[nIndex].flGain;

    aui16Regs[0] = GLOBAL_GAIN;
    aui16Regs[1] = (IMG_UINT16)(flGain*128);
    
    ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#ifdef INFOTM_ENABLE_AE_DEBUG
    printf("AR330DVP_SetGain: Global Gain 0x%04X = 0x%04X\n", aui16Regs[0], aui16Regs[1]);
#endif //INFOTM_ENABLE_AE_DEBUG
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set global gain register\n");
        return ret;
    }

    aui16Regs[0] = ANALOG_GAIN;
    aui16Regs[1] =
        (IMG_UINT16)((IMG_UINT16)asAR330DVPGainValues[nIndex].ui8GainRegVal
        |((IMG_UINT16)asAR330DVPGainValues[nIndex].ui8GainRegVal<<8));
        
    ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
#ifdef INFOTM_ENABLE_AE_DEBUG
    printf("AR330DVP_SetGain: Analog Gain 0x%04X = 0x%04X\n", aui16Regs[0], aui16Regs[1]);
#endif //INFOTM_ENABLE_AE_DEBUG
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set analogue gain register\n");
    }

    return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_GetExposureRange(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

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

IMG_RESULT AR330DVP_Create(SENSOR_HANDLE *phHandle, int index)
{
#if !file_i2c
    char i2c_dev_path[NAME_MAX];
#endif
    IMG_RESULT ret;
    AR330DVPCAM_STRUCT *psCam;
    IMG_UINT16 chipV = 0;
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
    psCam = (AR330DVPCAM_STRUCT *)IMG_CALLOC(1, sizeof(AR330DVPCAM_STRUCT));
    if (!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    *phHandle = &psCam->sFuncs;
    psCam->sFuncs.GetMode = AR330DVP_GetMode;
    psCam->sFuncs.GetState = AR330DVP_GetState;
    psCam->sFuncs.SetMode = AR330DVP_SetMode;
    psCam->sFuncs.Enable = AR330DVP_Enable;
    psCam->sFuncs.Disable = AR330DVP_DisableSensor;
    psCam->sFuncs.Destroy = AR330DVP_DestroySensor;
    
    psCam->sFuncs.GetGainRange = AR330DVP_GetGainRange;
    psCam->sFuncs.GetCurrentGain = AR330DVP_GetGain;
    psCam->sFuncs.SetGain = AR330DVP_SetGain;

    psCam->sFuncs.GetExposureRange = AR330DVP_GetExposureRange;
    psCam->sFuncs.GetExposure = AR330DVP_GetExposure;
    psCam->sFuncs.SetExposure = AR330DVP_SetExposure;
    
    psCam->sFuncs.GetInfo = AR330DVP_GetInfo;
    
#ifdef INFOTM_ISP
    psCam->sFuncs.SetFlipMirror = AR330DVP_SetFlipMirror;
    psCam->sFuncs.SetGainAndExposure = AR330DVP_SetGainAndExposure;
    psCam->sFuncs.SetFPS = AR330DVP_SetFPS;
    psCam->sFuncs.GetFixedFPS =  AR330DVP_GetFixedFPS;
    psCam->sFuncs.Reset = AR330DVP_Reset;
#endif //INFOTM_ISP

    psCam->bEnabled = IMG_FALSE;
    psCam->ui16CurrentMode = 0;
    psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;


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

//#ifndef NO_DEV_CHECK
    // we now verify that the i2c device has an AR330DVP sensor
    ret = ar330dvp_i2c_read16(psCam->i2c, CHIP_VERSION, &chipV);
    if (ret || AR330DVP_CHIP_VERSION != chipV)
    {
        LOG_ERROR("Failed to ensure that the i2c device has a compatible "\
            "AR330DVP sensor! ret=%d chip_version=0x%x (expect chip 0x%x)\n",
            ret, chipV, AR330DVP_CHIP_VERSION);
        close(psCam->i2c);
        IMG_FREE(psCam);
        *phHandle = NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    else
    {
    	LOG_ERROR("Check sensor ID successfully, ID = 0x%x\n", chipV);
    }
//#endif

#endif

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

#define CASE_MODE(N) \
case N:\
    registerArray = ui16ModeRegs_##N;\
    nRegisters = sizeof(ui16ModeRegs_##N)/(2*sizeof(IMG_UINT16)); \
    break;



static IMG_RESULT doStaticSetup(AR330DVPCAM_STRUCT *psCam, IMG_INT16 ui16Width,
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

    switch(ui16Mode)
    {
        CASE_MODE(0)
        CASE_MODE(1)

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
        char *pt = NULL; //for macro delete_space used
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
                //printf("val[0] = 0x%x, val[1]=0%x,\n", val[0], val[1]);
                ret = ar330dvp_i2c_write16(psCam->i2c, val[0], val[1]);
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

            //printf("w = %d, h = %d\n", ui16Width, ui16Height);
            if(val[0] == 0x3012)
            // store the coarse integration time, so we can report it correctly
            coarsetime = val[1];

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

            if(registerArray[n] == 0x300A)
                framelenghtlines = registerArray[n+1];
            if(registerArray[n] == 0x3040)
            {
                ui16read_mode = registerArray[n+1] & 0x3fff;
                registerArray[n+1] = ui16read_mode | (bFlipHorizontal?0x4000:0) | (bFlipVertical?0x8000:0);
                //          registerArray[n+1] = 0x0000;
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

            ret = ar330dvp_i2c_write16(psCam->i2c, registerArray[n], registerArray[n+1]);
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
        clk_pix /= 2;
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
        psCam->psSensorPhy->psGasket->bParallel = IMG_TRUE;
        printf("the imager = %d with dvp \n", imgr);
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
    psCam->fixedFps = (IMG_UINT32)psCam->fCurrentFps;
    psCam->initClk = framelenghtlines;
#endif
    return ret;
}

#ifdef INFOTM_ISP
static IMG_RESULT AR330DVP_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Flag)
{
    AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);
    IMG_BOOL bFlipHorizontal, bFlipVertical;
    IMG_UINT16 aui16Regs[2], read_mode;
    IMG_RESULT ret = 0;

	if (psCam->ui8CurrentFlipping == ui8Flag)
	{
		printf("the sensor had been FlipMirror.\n");
		return IMG_SUCCESS;
	}

	bFlipHorizontal = ui8Flag & SENSOR_FLIP_HORIZONTAL,
	bFlipVertical = ui8Flag & SENSOR_FLIP_VERTICAL;

	ret = ar330dvp_i2c_read16(psCam->i2c, READ_MODE, aui16Regs);
	read_mode = aui16Regs[0] & 0x3FFF;

	aui16Regs[0] = READ_MODE;
	aui16Regs[1] = read_mode | (bFlipHorizontal? 0x4000:0) | (bFlipVertical? 0x8000:0);
    ret = ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
    psCam->ui8CurrentFlipping = ui8Flag;

    return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    AR330DVP_SetExposure(hHandle, ui32Exposure, ui8Context);
	AR330DVP_SetGain(hHandle, flGain, ui8Context);
    return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
	if (pfixed != NULL)
	{
		AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);

		*pfixed = (int)psCam->fixedFps;
	}

	return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_SetFPS(SENSOR_HANDLE hHandle, double fps)
{
	AR330DVPCAM_STRUCT *psCam = container_of(hHandle, AR330DVPCAM_STRUCT, sFuncs);
	 IMG_UINT16 aui16Regs[2];
    float down_ratio = 0.0;
    int fixedfps = 0;


    fixedfps = psCam->fixedFps;
    if(fps > fixedfps)
    {
        fps = fixedfps;
    }
    else if (fps < 5)
    {
        fps = 5;
    }
    down_ratio = psCam->fCurrentFps / fps;

	//printf("current down_ratio =%f\n", down_ratio);

	//switch (psCam->ui16CurrentMode)
	{

		if (down_ratio >= 1 && down_ratio <= 12) //if ratio > 1, then down the frame rate
		{

			aui16Regs[0] = 0x302E;
			ar330dvp_i2c_read16(psCam->i2c, aui16Regs[0], &aui16Regs[1]);
			if(psCam->fCurrentFps/down_ratio < 5)//minimum fps
			{
				aui16Regs[1] *= (down_ratio - 2);
			}
			else
			{
				aui16Regs[1] =aui16Regs[1]*(IMG_UINT16)(down_ratio + 0.5);
			}
			//aui16Regs[1] *= (down_ratio);
			ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
			//psCam->ui8CurrentFps = psCam->ui8CurrentFps/(down_ratio);
			psCam->fCurrentFps = psCam->fCurrentFps/(down_ratio + 0.5);
		}
		else if(down_ratio > 0 && down_ratio < 1) //then up the frame rate
		{
			aui16Regs[0] = 0x302E;
			ar330dvp_i2c_read16(psCam->i2c, aui16Regs[0], &aui16Regs[1]);
			aui16Regs[1] *=down_ratio;
			if(aui16Regs[1] >=psCam->initClk)
			{
				ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
				psCam->fCurrentFps = psCam->fCurrentFps/down_ratio;
			}
			else
			{
				aui16Regs[1] = psCam->initClk;
				ar330dvp_i2c_write16(psCam->i2c, aui16Regs[0], aui16Regs[1]);
				psCam->fCurrentFps = psCam->fixedFps;
			}

		}

	}

	return IMG_SUCCESS;
}

static IMG_RESULT AR330DVP_Reset(SENSOR_HANDLE hHandle)
{
    int ret = 0;

    LOG_ERROR("first: disable AR330 mipi camera\n");
    AR330DVP_DisableSensor(hHandle);

    LOG_ERROR("second: enable AR330 mipi camera\n");

    usleep(100000); // the delay time used for phy staus change deinit to init

    AR330DVP_Enable(hHandle);

    return ret;
}
#endif //INFOTM_ISP
