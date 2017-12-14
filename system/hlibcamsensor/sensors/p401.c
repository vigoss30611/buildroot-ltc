/**
 ******************************************************************************
@file p401.c

@brief P401 camera implementation

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

#include "sensors/p401.h"

#include "img_types.h"
#include "img_errors.h"

#include "ci/ci_api_structs.h"
#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"

#include <stdio.h>
#include <math.h>
#if !file_i2c
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#endif


#ifdef INFOTM_ISP
static IMG_RESULT P401_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag); //The function is not support now.
static IMG_RESULT P401_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
#endif //INFOTM_ISP


#define LOG_TAG "P401_SENSOR"
#include <felixcommon/userlog.h>

#define P401_WRITE_ADDR (0xBA >> 1)
#define P401_READ_ADDR (0xBB >> 1)
#define P401_PHY_NUM 0

#define P401_SENSOR_VERSION "not-verified"

/** @brief P401 chip version is not the full 16b but only top 7b */
#define P401_CHIP_VERSION_CHIP_MASK 0xFE00
#define P401_CHIP_VERSION_CHIP_SHIFT 8
#define P401_CHIP_VERSION_ANALOG_MASK 0x01F0
#define P401_CHIP_VERSION_ANALOG_SHIFT 4
#define P401_CHIP_VERSION_DIGITAL_MASK 0x000F
#define P401_CHIP_VERSION_DIGITAL_SHIFT 0

/** @brief expected value for CHIP_VERSION register (mask applied) */
#define P401_CHIP_VERSION (0x1801 & P401_CHIP_VERSION_CHIP_MASK)

#define CHIP_VERSION 0x0

/* disabled as i2c drivers locks up if device is not present */
#define NO_DEV_CHECK

#ifdef WIN32 // on windows we do not need to sleep to wait for the bus
static void usleep(int t)
{
    (void)t;
}
#endif

typedef struct _p401cam_struct
{
    SENSOR_FUNCS sFuncs;
    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
    IMG_BOOL nEnabled;
  
    SENSOR_MODE currMode;
  
    IMG_UINT32 ui32Exposure;
    double flGain;
    IMG_UINT8 imager;
  
    int i2c;
    SENSOR_PHY *psSensorPhy;
}P401CAM_STRUCT;

static SENSOR_MODE p401_modes[]=
{
    // line length in micros assuming a 24MHz clock for exposure times
    {10, 640, 480, 30.0, 640, 486, SENSOR_FLIP_NONE, 96, 96*479, 0},
};

static IMG_RESULT P401_ApplyMode(P401CAM_STRUCT *psCam);

static IMG_RESULT p401_i2c_write8(int i2c, IMG_UINT8 *data, unsigned int len)
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
    if (ioctl(i2c, I2C_SLAVE, P401_WRITE_ADDR))
    {
        LOG_ERROR("Failed to write I2C write address!\n");
        return IMG_ERROR_BUSY;
    }

    for (i = 0; i < len; data += 3)
    {
        if (write(i2c, data, 3) != 3)
        {
            LOG_ERROR("Failed to write I2C data!\n");
            return IMG_ERROR_BUSY;
        }
    }
#endif
    return IMG_SUCCESS;
}

static IMG_RESULT p401_i2c_read8(int i2c, IMG_UINT16 offset,
    IMG_UINT8 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
    
    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE, P401_READ_ADDR))
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
#endif  /* file_i2c */
    return IMG_SUCCESS;
}

static IMG_RESULT p401_i2c_read16(int i2c, IMG_UINT16 offset,
    IMG_UINT16 *data)
{
#if !file_i2c
    int ret;
    IMG_UINT8 buff[2];
    
    IMG_ASSERT(data);  // null pointer forbidden
    
    /* Set I2C slave address */
    if (ioctl(i2c, I2C_SLAVE, P401_READ_ADDR))
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

static IMG_RESULT P401_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);
    int ret;
    IMG_UINT16 v = 0;

    IMG_ASSERT(psCam);
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!psCam->psSensorPhy->psConnection)
    {
        LOG_ERROR("sensor connection to CI not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    
    IMG_ASSERT(strlen(P401_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(P401_SENSOR_VERSION) < SENSOR_INFO_VERSION_MAX);
    
    psInfo->eBayerOriginal = MOSAIC_GRBG;
    psInfo->eBayerEnabled = psInfo->eBayerOriginal;
    // assumes that when flipping changes the bayer pattern
    if (SENSOR_FLIP_NONE!=psInfo->sStatus.ui8Flipping)
    {
        psInfo->eBayerEnabled = MosaicFlip(psInfo->eBayerOriginal,
            psInfo->sStatus.ui8Flipping&SENSOR_FLIP_HORIZONTAL?1:0,
            psInfo->sStatus.ui8Flipping&SENSOR_FLIP_VERTICAL?1:0);
    }
    sprintf(psInfo->pszSensorName, P401_SENSOR_INFO_NAME);

#ifndef NO_DEV_CHECK
    ret = p401_i2c_read16(psCam->i2c, P401_CHIP_VERSION, &v);
#else
    ret = 1;
#endif
    if (!ret)
    {
        sprintf(psInfo->pszSensorVersion, "%d-%d",
            (v&P401_CHIP_VERSION_ANALOG_MASK) >> P401_CHIP_VERSION_ANALOG_SHIFT,
            (v&P401_CHIP_VERSION_DIGITAL_MASK) >> P401_CHIP_VERSION_DIGITAL_SHIFT);
    }
    else
    {
        sprintf(psInfo->pszSensorVersion, P401_SENSOR_VERSION);
    }
    
    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 6040;
    // bitdepth is mode information
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = psCam->imager;
    psInfo->bBackFacing = IMG_TRUE;
    // other information should be filled by Sensor_GetInfo()
    
    
    
    return IMG_SUCCESS;
}

static IMG_RESULT P401_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
    SENSOR_MODE *psModes)
{
    /*P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    if (nIndex < (sizeof(p401_modes) / sizeof(SENSOR_MODE)))
    {
        IMG_MEMCPY(psModes, &(p401_modes[nIndex]), sizeof(SENSOR_MODE));
        return IMG_SUCCESS;
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

static IMG_RESULT P401_GetState(SENSOR_HANDLE hHandle,
        SENSOR_STATUS *psStatus)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy || !psCam->psSensorPhy->psConnection)
    {
        psStatus->eState = SENSOR_STATE_UNINITIALISED;
        psStatus->ui16CurrentMode = 0;
    }
    else
    {
        psStatus->eState = psCam->nEnabled ?
            SENSOR_STATE_RUNNING : SENSOR_STATE_IDLE;
        psStatus->ui16CurrentMode = psCam->ui16CurrentMode;
    }
    return IMG_SUCCESS;
}

static IMG_RESULT P401_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
    IMG_UINT8 ui8Flipping)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if(SENSOR_FLIP_NONE != ui8Flipping)
    {
        LOG_ERROR("sensor does not support flipping\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (nMode < (sizeof(p401_modes) / sizeof(SENSOR_MODE)))
    {
        if((ui8Flipping&(p401_modes[nMode].ui8SupportFlipping))!=ui8Flipping)
        {
            LOG_ERROR("sensor mode does not support selected flipping "\
                "0x%x (supports 0x%x)\n", 
                ui8Flipping, p401_modes[nMode].ui8SupportFlipping);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        
        IMG_MEMCPY(&(psCam->currMode), &(p401_modes[nMode]),
            sizeof(SENSOR_MODE));
        
        ret = P401_ApplyMode(psCam);
        if (IMG_SUCCESS != ret )
        {
            LOG_ERROR("failed to set mode %d\n", nMode);
        }
        else
        {
            psCam->ui16CurrentMode = nMode;
        }
    }
    else
    {
        LOG_ERROR("invalid mode %d\n", nMode);
        ret = IMG_ERROR_INVALID_PARAMETERS;
    }

    return ret;
}

static IMG_RESULT P401_Enable(SENSOR_HANDLE hHandle)
{
    IMG_RESULT ret;
    IMG_UINT8 aui8Regvals[] =
    {
        0x0b, 0x00, 0x01
    };
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling P401 Camera!\n");
    psCam->nEnabled = 1;

    SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE, psCam->currMode.ui8MipiLanes,
        P401_PHY_NUM);
    
    ret=p401_i2c_write8(psCam->i2c, aui8Regvals, sizeof(aui8Regvals));

    if (IMG_SUCCESS == ret)
    {
        LOG_DEBUG("camera enabled\n");
    }
    else
    {
        LOG_ERROR("failed to enable camera\n");
    }
    return ret;
}

static IMG_RESULT P401_Disable(SENSOR_HANDLE hHandle)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);
    IMG_UINT8 aui8Regvals[3];
    IMG_RESULT ret;
    int delay = 0;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Disabling P401 camera\n");
    psCam->nEnabled = 0;
    
    // : no disabling removal
    /*
    aui16Regs[0] = 0x301a; // RESET_REGISTER
    aui16Regs[1] = 0x0058;
    SensorI2CWrite16(psCam->i2c, aui16Regs, 2);
    */
    aui8Regvals[0] = 0x03;
    aui8Regvals[1] = 0x1a;
    aui8Regvals[2] = 0x58;
    ret = p401_i2c_write8(psCam->i2c, aui8Regvals, sizeof(aui8Regvals));
    if (IMG_SUCCESS == ret)
    {
        LOG_DEBUG("camera disabled\n");
    }
    else
    {
        LOG_ERROR("failed to disable camera\n");
    }

    delay = (int)floor(1.0 / (psCam->currMode.flFrameRate) * 1000 * 1000);
    LOG_DEBUG("delay of %uus between disabling sensor/phy\n", delay);
    
    // delay of a frame period to ensure sensor has stopped
    // flFrameRate in Hz, change to MHz to have micro seconds
    usleep(delay);

    SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, P401_PHY_NUM);
    return IMG_SUCCESS;
}

static IMG_RESULT P401_Destroy(SENSOR_HANDLE hHandle)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Destroying P401 camera\n");
    if (psCam->nEnabled)
        P401_Disable(hHandle);
    SensorPhyDeinit(psCam->psSensorPhy);
#if !file_i2c
    close(psCam->i2c);
#endif
    IMG_FREE(psCam);
    return IMG_SUCCESS;
}

static IMG_RESULT P401_GetExposure(SENSOR_HANDLE hHandle,
        IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pi32Current = psCam->ui32Exposure;
    return IMG_SUCCESS;
}

static IMG_RESULT P401_SetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
    IMG_UINT8 aui8I2CData[6];
    IMG_UINT32 ui32ExposureLines;
    IMG_UINT16 ui16Lower, ui16Upper;
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui32Exposure = ui32Current;

    // first convert the exposure to something sensible then send it!
    ui32ExposureLines = psCam->ui32Exposure/psCam->currMode.ui32ExposureMin;
    ui16Lower = ui32ExposureLines & 0xffff;
    ui16Upper = ui32ExposureLines >> 16;
    aui8I2CData[0] = 0x8; // shutter width upper
    aui8I2CData[1] = ui16Upper >> 8; // shutter width upper
    aui8I2CData[2] = ui16Upper & 0xff; // shutter width upper
    aui8I2CData[3] = 0x9; // shutter width lower
    aui8I2CData[4] = ui16Lower >> 8; // shutter width ui16Lower
    aui8I2CData[5] = ui16Lower & 0xff; // shutter width ui16Lower

    ret=p401_i2c_write8(psCam->i2c, aui8I2CData, sizeof(aui8I2CData));
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set exposure\n");
    }

    return ret;
}

static IMG_RESULT P401_GetCurrentGainRange(SENSOR_HANDLE hHandle,
    double *pflMin, double *pflMax, IMG_UINT8 *pui8Contexts)
{
    /*P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }*/

    *pflMin = 1.0;
    *pflMax = 16.0;
    *pui8Contexts = 1;
    return IMG_SUCCESS;
}

static IMG_RESULT P401_GetCurrentGain(SENSOR_HANDLE hHandle, double *pflGain,
    IMG_UINT8 ui8Context)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflGain = psCam->flGain;
    return IMG_SUCCESS;
}

static IMG_RESULT P401_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
    IMG_UINT8 aui8I2CData[3];
    IMG_UINT16 ui8MultiplierVal = 0;
    IMG_UINT16 ui16AnalogGain, ui16DigitalGain;
    double flSourceGain;
    double flMultiplier, flAnalogGain, flDigialGain;
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->flGain = flGain;

    /*
     * Convert gain to fixed point 5.3 format.
     * Figure out multiplier first, then analog remainder is digital.
     */
//    LOG_DEBUG("\nRequested gain %f \n",flGain);
    flSourceGain = flGain;
    if (flGain >= 2.0)
    {
        ui8MultiplierVal = 1 << 6;
        flGain /= 2;
    }

    if (flGain > 8.0)
        flGain = 8.0;
    /// @ fix float to uint conversion
    ui16AnalogGain = (IMG_UINT16)floor((flGain * 8.0) + 0.5);
    flMultiplier = (double)(ui8MultiplierVal >> 6);

    flDigialGain = flSourceGain / ((ui16AnalogGain/8.0) * (flMultiplier+1));

    LOG_DEBUG("flDigitalGain = %f, flAnalogGain = %f\n",
        flDigialGain, flGain);
    ui16AnalogGain |= ui8MultiplierVal;
    ui16DigitalGain = (IMG_UINT16)(((flDigialGain-1.0)*8)+0.5);

    flMultiplier = (float)(ui8MultiplierVal >> 6);
    flAnalogGain = (float)(ui16AnalogGain & 0x3f);
    flDigialGain = ui16DigitalGain;
    LOG_DEBUG("Applied Gain = ((%f / 8) + 1)* (%f +1) * (%f / 8) = %f \n",
            flDigialGain, flMultiplier, flAnalogGain,
            ((flDigialGain / 8) + 1) * (flMultiplier + 1)
            * (flAnalogGain / 8));

    // just need to write to the gain registers now.
    aui8I2CData[0] = 0x35; // address of green gain
    aui8I2CData[1] = (IMG_UINT8)ui16DigitalGain;
    aui8I2CData[2] = (IMG_UINT8)ui16AnalogGain;

    ret=p401_i2c_write8(psCam->i2c, aui8I2CData, sizeof(aui8I2CData));
    if(IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set gains\n");
    }

    return ret;
}

static IMG_RESULT P401_GetExposureRange(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    P401CAM_STRUCT *psCam = container_of(hHandle, P401CAM_STRUCT, sFuncs);

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


IMG_RESULT P401_Create(SENSOR_HANDLE *phHandle)
{
#if !file_i2c
    char i2c_dev_path[NAME_MAX];
    IMG_RESULT ret;
#endif
    P401CAM_STRUCT *psCam;
    CI_HWINFO *pInfo = NULL;
    IMG_UINT16 chipV = 0;
    char adaptor[64];
    psCam = (P401CAM_STRUCT *)IMG_CALLOC(1, sizeof(P401CAM_STRUCT));
    if (!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    psCam->sFuncs.GetMode = P401_GetMode;
    psCam->sFuncs.GetState = P401_GetState;
    psCam->sFuncs.SetMode = P401_SetMode;
    psCam->sFuncs.Enable = P401_Enable;
    psCam->sFuncs.Disable = P401_Disable;
    psCam->sFuncs.Destroy = P401_Destroy;
    
    psCam->sFuncs.GetGainRange = P401_GetCurrentGainRange;
    psCam->sFuncs.GetCurrentGain = P401_GetCurrentGain;
    psCam->sFuncs.SetGain = P401_SetGain;
    
    psCam->sFuncs.GetExposureRange = P401_GetExposureRange;
    psCam->sFuncs.GetExposure = P401_GetExposure;
    psCam->sFuncs.SetExposure = P401_SetExposure;

#ifdef INFOTM_ISP
	psCam->sFuncs.SetFlipMirror = P401_SetFlipMirror;
	psCam->sFuncs.SetGainAndExposure = P401_SetGainAndExposure;
#endif //INFOTM_ISP
	
    psCam->sFuncs.GetInfo = P401_GetInfo;
    
    psCam->nEnabled = 0;
    psCam->ui16CurrentMode = 0;
    
    IMG_MEMCPY(&(psCam->currMode), &(p401_modes[psCam->ui16CurrentMode]),
        sizeof(SENSOR_MODE));

    /*
     * 50KHz max bitrate for P401 I2C interface, default slave write address
     * is 0xba, read address is 0xbb.
     */
    //psCam->psI2C = SensorI2CInit(0, 0xba, 0xbb, 400*1000, IMG_TRUE);
#if !file_i2c
    if (find_i2c_dev(i2c_dev_path, sizeof(i2c_dev_path), P401_WRITE_ADDR, adaptor))
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
    // disabled until we have a timeout in the i2c poll
    // we now verify that the i2c device has an P401 sensor
    ret = p401_i2c_read16(psCam->i2c, CHIP_VERSION, &chipV);
    if (ret || P401_CHIP_VERSION != (chipV & P401_CHIP_VERSION_CHIP_MASK))
    {
        LOG_ERROR("Failed to ensure that the i2c device has a compatible "\
            "P401 sensor! ret=%d chip_version=0x%x (expect chip 0x%x)\n",
            ret, chipV & P401_CHIP_VERSION_CHIP_MASK, P401_CHIP_VERSION);
        close(psCam->i2c);
        IMG_FREE(psCam);
        *phHandle = NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
#endif

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

    pInfo = &(psCam->psSensorPhy->psConnection->sHWInfo);

    /* IMG PHY was first connected to all gasket in version 2.2.4 which is
     * specific to imagination FPGA
     * on a real system it is expected that the gasket is known! */
    if (pInfo->rev_ui8Major == 1
        || (pInfo->rev_ui8Major == 2 && pInfo->rev_ui8Minor == 2
        && pInfo->rev_ui8Maint < 4)
        )
    {
        psCam->imager = 0;
        LOG_WARNING("version %d.%d.%d of HW may not allow gasket selection! "\
            "Gasket forced to %d\n",
            pInfo->rev_ui8Major,
            pInfo->rev_ui8Minor,
            pInfo->rev_ui8Maint,
            psCam->imager
            );
    }
    else
    {
        int g;
        // sensor could be connected to other gaskets in some of IMG test boards
        for (g = 0; g < pInfo->config_ui8NImagers; g++)
        {
            if ((pInfo->gasket_aType[g] & CI_GASKET_PARALLEL) != 0)
            {
                break;
            }
        }
        if (g != pInfo->config_ui8NImagers)
        {
            psCam->imager = g;
            LOG_DEBUG("selecting gasket %d (first Parallel found)\n", g);
        }
        else
        {
            LOG_ERROR("could not find a parallel gasket!\n");
            SensorPhyDeinit(psCam->psSensorPhy);
#if !file_i2c
            close(psCam->i2c);
#endif
            IMG_FREE(psCam);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }

    *phHandle = &psCam->sFuncs;
    /* Try and read the chip version register. */
    //SensorI2CRead16(psCam->i2c, &ui16Value, 0x3000, 1);
    LOG_DEBUG("Sensor initialised\n");
    return IMG_SUCCESS;
}

static IMG_UINT8 mode640x480[]=
{
    0x03,0x07,0x78,
    0x04,0x09,0xf8,
    0x0a,0x80,0x00,
    0x22,0x00,0x33,
    0x23,0x00,0x33,
    0x08,0x00,0x00,
    0x09,0x02,0x96,
    0x0c,0x00,0x00,
    0x70,0x00,0x5c,
    0x71,0x5b,0x00,
    0x72,0x59,0x00,
    0x73,0x02,0x00,
    0x74,0x02,0x00,
    0x75,0x28,0x00,
    0x76,0x3e,0x29,
    0x77,0x3e,0x29,
    0x78,0x58,0x3f,
    0x79,0x5b,0x00,
    0x7a,0x5a,0x00,
    0x7b,0x59,0x00,
    0x7c,0x59,0x00,
    0x7e,0x59,0x00,
    0x7f,0x59,0x00,
    0x06,0x00,0x00,
    0x29,0x04,0x81,
    0x3e,0x00,0x87,
    0x3f,0x00,0x07,
    0x41,0x00,0x03,
    0x48,0x00,0x18,
    0x5f,0x1c,0x16,
    0x57,0x00,0x07,
    0x2a,0xff,0x74,
    0x0b,0x00,0x03,
};

// static setup based on register dump
static IMG_RESULT P401_ApplyMode(P401CAM_STRUCT *psCam)
{
    IMG_RESULT ret;

    LOG_DEBUG("Writing I2C\n");

    ret=p401_i2c_write8(psCam->i2c, mode640x480, sizeof(mode640x480));
    
    if (IMG_SUCCESS == ret)
    {
        usleep(1000);
        LOG_DEBUG("Sensor ready to go\n");
        
        psCam->ui32Exposure = psCam->currMode.ui32ExposureMin * 100;
        psCam->flGain = 1.0;
        
        IMG_ASSERT(psCam->currMode.ui8MipiLanes==0);
        
        psCam->psSensorPhy->psGasket->bParallel = IMG_TRUE;
        // note that height-1 is to make gasket work - : verify if still needed
        psCam->psSensorPhy->psGasket->uiWidth = psCam->currMode.ui16Width;
        psCam->psSensorPhy->psGasket->uiHeight = psCam->currMode.ui16Height-1;
        psCam->psSensorPhy->psGasket->bVSync = IMG_TRUE;
        psCam->psSensorPhy->psGasket->bHSync = IMG_TRUE;
        psCam->psSensorPhy->psGasket->ui8ParallelBitdepth =
            psCam->currMode.ui8BitDepth;
    }
    return ret;
}

#ifdef INFOTM_ISP
static IMG_RESULT P401_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag)
{
	//Now not support.
	//..
	
    return IMG_SUCCESS;
}

static IMG_RESULT P401_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    P401_SetExposure(hHandle, ui32Exposure, ui8Context);
    P401_SetGain(hHandle, flGain, ui8Context);
    return IMG_SUCCESS;
}
#endif //INFOTM_ISP

