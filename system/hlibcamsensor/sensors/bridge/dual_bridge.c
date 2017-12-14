/******************************************************************************
@file sx2_bridge.c
******************************************************************************/

#include <img_types.h>
#include <img_errors.h>

#include <ci/ci_api_structs.h> // access to CI_GASKET
#include "sensorapi/sensorapi.h"
#include "sensorapi/sensor_phy.h"

#include <stdio.h>
#include <math.h>
#include <unistd.h>

#if !file_i2c
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#endif

#ifdef INFOTM_ISP
#include <linux/i2c.h>
#endif //INFOTM_ISP

#include "sensors/ddk_sensor_driver.h"

#include "sensors/bridge/list.h"
#include "sensors/bridge/bridge_sensors.h"
#include "sensors/bridge/bridge_i2c.h"
#include "sensors/bridge/bridge_dev.h"
#include "sensors/bridge/dual_bridge.h"

#define LOG_TAG "DUAL_BRIDGE"
#include <felixcommon/userlog.h>

/** @brief Sensor number on IMG PHY */
#define DUAL_BRIDGE_PARALLEL_PHY_NUM 0
#define DUAL_BRIDGE_MIPI_PHY_NUM 1

#define DUAL_BRIDGE_VERSION "not-verified"  // until read from sensor

typedef struct _dual_bridge_struct
{
    SENSOR_FUNCS sFuncs;

    // in MHz
    double refClock;
    double pClock;

    // local stuff goes here.
    IMG_UINT16 ui16CurrentMode;
    IMG_UINT8 ui8CurrentFlipping;
    IMG_BOOL bEnabled;
    IMG_UINT8 imager;

    SENSOR_MODE currMode;

    IMG_UINT32 ui32Exposure;
    double flGain;
    IMG_UINT16 ui16CurrentFocus;
    double curFps;
    int flxedFps;

    SENSOR_PHY *psSensorPhy;

#ifdef INFOTM_ISP
    double fCurrentFps;
    IMG_UINT32 fixedFps;
    IMG_UINT32 initClk;
#endif

    BRIDGE_I2C_CLIENT* bridge_i2c;
    BRIDGE_I2C_CLIENT* sensor_i2c;
    BRIDGE_SENSOR* sensor;
    BRIDGE_DEV* bridge_dev;
    SENSOR_CONFIG* sensor_cfg;
    SENSOR_CONFIG* bridge_cfg;

    BRIDGE_STRUCT* bridge;
}DUAL_BRIDGE_STRUCT;

/*Note: flPixelRate = HT*VT*FPS / 1000000 = PCLK*/
static SENSOR_MODE asModes[] = {
    {10, 2560, 720, 30.0f, 72.0f, 3200, 750, SENSOR_FLIP_BOTH},/*PCLK = 36MHz*/
};

static SENSOR_MODE currMode = {10, 2560, 720, 30.0f, 72, 3200, 750, SENSOR_FLIP_BOTH};

static IMG_RESULT doSetup(DUAL_BRIDGE_STRUCT *psCam, IMG_UINT16 ui16Mode,
    IMG_BOOL bFlipHorizontal,IMG_BOOL bFlipVertical);

// pui32ExposureDef is optional
static IMG_RESULT DualBridge_GetModeInfo(DUAL_BRIDGE_STRUCT *psCam,
    IMG_UINT16 ui16Mode, SENSOR_MODE *info);

static int GetMosaicType(const char * mosaic_type)
{
    LOG_DEBUG("mosaic type :%s\n", mosaic_type);

    if (0 == strncasecmp(mosaic_type, "RGGB", 16))
        return MOSAIC_RGGB;
    else if (0 == strncasecmp(mosaic_type, "GRBG", 16))
        return MOSAIC_GRBG;
    else if (0 == strncasecmp(mosaic_type, "GBRG", 16))
        return MOSAIC_GBRG;
    else if (0 == strncasecmp(mosaic_type, "BGGR", 16))
        return MOSAIC_BGGR;
    else
        return MOSAIC_NONE;
}

static IMG_RESULT DualBridge_GetInfo(SENSOR_HANDLE hHandle,
    SENSOR_INFO *psInfo)
{
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_ASSERT(strlen(DUAL_BRIDGE_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    IMG_ASSERT(strlen(DUAL_BRIDGE_VERSION) < SENSOR_INFO_VERSION_MAX);

    psInfo->eBayerOriginal = GetMosaicType(psCam->bridge_cfg->mosaic_type);
    psInfo->eBayerEnabled =GetMosaicType(psCam->bridge_cfg->mosaic_type);

    sprintf(psInfo->pszSensorName, DUAL_BRIDGE_INFO_NAME);

    psInfo->fNumber = 1.2;
    psInfo->ui16FocalLength = 30;
    psInfo->ui32WellDepth = 7500;
    // bitdepth moved to mode information
    psInfo->flReadNoise = 5.0;
    psInfo->ui8Imager = psCam->bridge_cfg->imager;
    psInfo->bBackFacing = IMG_TRUE;
#ifdef INFOTM_ISP
    psInfo->ui32ModeCount = 0;
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

static IMG_RESULT DualBridge_GetMode(SENSOR_HANDLE hHandle,
    IMG_UINT16 ui16Mode, SENSOR_MODE *psModes)
{
    IMG_RESULT ret;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = DualBridge_GetModeInfo(psCam, ui16Mode, psModes);
    if (ret != IMG_SUCCESS)
    {
        ret = IMG_ERROR_NOT_SUPPORTED;
    }
    return ret;
}

static IMG_RESULT DualBridge_GetState(SENSOR_HANDLE hHandle,
    SENSOR_STATUS *psStatus)
{
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

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

static IMG_RESULT DualBridge_SetMode(SENSOR_HANDLE hHandle,
    IMG_UINT16 ui16Mode, IMG_UINT8 ui8Flipping)
{
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

        ret = doSetup(psCam, ui16Mode,
            ui8Flipping&SENSOR_FLIP_HORIZONTAL, ui8Flipping&SENSOR_FLIP_VERTICAL);
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

    LOG_ERROR("invalid mode %d\n", ui16Mode);

    return IMG_ERROR_INVALID_PARAMETERS;
}

static IMG_RESULT DualBridge_Stream_Enable(
    DUAL_BRIDGE_STRUCT *psCam, int enable)
{
     if (psCam->sensor && psCam->sensor->enable)
         return psCam->sensor->enable(psCam->sensor_i2c, enable);
    else
         return IMG_ERROR_NOT_INITIALISED;
}

static IMG_RESULT DualBridge_Enable(SENSOR_HANDLE hHandle)
{
    IMG_RESULT ret;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Enabling DUAL_BRIDGE Camera!\n");
    psCam->bEnabled = IMG_TRUE;

    // setup the sensor number differently when using multiple sensors
    if (psCam->psSensorPhy->psGasket->bParallel)
        ret = SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE,
            psCam->currMode.ui8MipiLanes, DUAL_BRIDGE_PARALLEL_PHY_NUM);
    else
        ret = SensorPhyCtrl(psCam->psSensorPhy, IMG_TRUE,
            psCam->currMode.ui8MipiLanes, DUAL_BRIDGE_MIPI_PHY_NUM);

    if (ret == IMG_SUCCESS)
    {
        ret = IMG_SUCCESS;
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to enable sensor\n");
        }
        else
        {
            DualBridge_Stream_Enable(psCam, 1);
            LOG_DEBUG("camera enabled\n");
        }
    }

    return ret;
}

static IMG_RESULT DualBridge_DisableSensor(SENSOR_HANDLE hHandle)
{
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    IMG_RESULT ret = IMG_SUCCESS;
    int delay = 0;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Disabling DUAL_BRIDGE camera\n");
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
    DualBridge_Stream_Enable(psCam, 0);
    Bridge_Enable(0);

    if (psCam->psSensorPhy->psGasket->bParallel)
        SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE, 0, DUAL_BRIDGE_PARALLEL_PHY_NUM);
    else
        SensorPhyCtrl(psCam->psSensorPhy, IMG_FALSE,
            psCam->currMode.ui8MipiLanes, DUAL_BRIDGE_MIPI_PHY_NUM);

    return ret;
}

static IMG_RESULT DualBridge_DestroySensor(SENSOR_HANDLE hHandle)
{
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("Destroying DUAL_BRIDGE camera\n");
    /* remember to free anything that might have been allocated dynamically
     * (like extended params...)*/
    if (psCam->bEnabled)
    {
        DualBridge_DisableSensor(hHandle);
    }

    SensorPhyDeinit(psCam->psSensorPhy);
    Bridge_Destory();
    IMG_FREE(psCam);
    return IMG_SUCCESS;
}

static IMG_RESULT DualBridge_GetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pi32Current, IMG_UINT8 ui8Context)
{
    IMG_RESULT ret = IMG_ERROR_NOT_INITIALISED;
    SENSOR_MODE info;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("sensor not GetModeInfo\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (psCam->sensor && psCam->sensor->get_exposure)
        ret = psCam->sensor->get_exposure(psCam->sensor_i2c, &psCam->ui32Exposure, &info);
    if (ret == IMG_SUCCESS)
        *pi32Current = psCam->ui32Exposure;
    return ret;
}

static IMG_RESULT DualBridge_SetExposure(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32Current, IMG_UINT8 ui8Context)
{
    IMG_RESULT ret = IMG_ERROR_NOT_INITIALISED;
    SENSOR_MODE info;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);


    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("sensor not GetModeInfo\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (psCam->sensor && psCam->sensor->set_exposure)
        ret = psCam->sensor->set_exposure(psCam->sensor_i2c, ui32Current, &info);

    return ret;
}

static IMG_RESULT DualBridge_GetGainRange(SENSOR_HANDLE hHandle,
    double *pflMin, double *pflMax, IMG_UINT8 *pui8Contexts)
{
    IMG_RESULT ret = IMG_SUCCESS;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    SENSOR_MODE info;
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pflMin = 1.0;
    *pflMax = 8.0;

    if (psCam->sensor && psCam->sensor->get_gain_range)
        ret = psCam->sensor->get_gain_range(psCam->sensor_i2c, pflMin, pflMax, &info);

    *pui8Contexts = 1;
    return ret;
}

static IMG_RESULT DualBridge_GetGain(SENSOR_HANDLE hHandle,
    double *pflGain, IMG_UINT8 ui8Context)
{
    IMG_RESULT ret = IMG_SUCCESS;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    SENSOR_MODE info;

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("sensor not GetModeInfo\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (psCam->sensor && psCam->sensor->get_gain)
        ret = psCam->sensor->get_gain(psCam->sensor_i2c, &psCam->flGain, &info);
    if (ret == IMG_SUCCESS)
        *pflGain = psCam->flGain;
    return ret;
}

static IMG_RESULT DualBridge_SetGain(SENSOR_HANDLE hHandle,
    double flGain, IMG_UINT8 ui8Context)
{
    IMG_RESULT ret = IMG_SUCCESS;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    SENSOR_MODE info;


    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("sensor not GetModeInfo\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (psCam->sensor && psCam->sensor->set_gain)
        ret = psCam->sensor->set_gain(psCam->sensor_i2c, flGain, &info);

    return ret;
}

static IMG_RESULT DualBridge_GetExposureRange(
    SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
    IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    *pui32Min = psCam->currMode.ui32ExposureMin;
    *pui32Max = psCam->currMode.ui32ExposureMax;

#if 0
    if (psCam->sensor && psCam->sensor->get_exposure_range)
        ret = psCam->sensor->get_exposure_range(psCam->sensor_i2c,
                pui32Min, pui32Max, &info);
#endif

    *pui8Contexts = 1;
    return ret;
}

#ifdef INFOTM_ISP
static IMG_RESULT DualBridge_SetFlipMirror(SENSOR_HANDLE hHandle,
    IMG_UINT8 ui8Flag)
{
    return IMG_SUCCESS;
}

static IMG_RESULT DualBridge_SetGainAndExposure(
    SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);


    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("sensor not GetModeInfo\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (psCam->sensor && psCam->sensor->set_exposure_gain)
        ret = psCam->sensor->set_exposure_gain(psCam->sensor_i2c,
                ui32Exposure, flGain, &info);
    return ret;
}

IMG_RESULT DualBridge_CheckChipID(DUAL_BRIDGE_STRUCT *psCam,
    BRIDGE_I2C_CLIENT* i2c)
{
    if (psCam->sensor && psCam->sensor->check_id)
         return psCam->sensor->check_id(i2c);
    else
         return IMG_SUCCESS;
}

IMG_RESULT DualBridge_GetFocusRange(SENSOR_HANDLE hHandle,
    IMG_UINT16 *pui16Min, IMG_UINT16 *pui16Max)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (psCam->sensor && psCam->sensor->get_focus_range)
        ret = psCam->sensor->get_focus_range(psCam->sensor_i2c,
            (IMG_UINT32*)pui16Min, (IMG_UINT32*)pui16Max, &info);
    return ret;
}

IMG_RESULT DualBridge_GetCurrentFocus(SENSOR_HANDLE hHandle,
    IMG_UINT16 *pui16Current)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    *pui16Current = psCam->ui16CurrentFocus;
    return ret;
}

IMG_RESULT DualBridge_SetFocus(SENSOR_HANDLE hHandle,
    IMG_UINT16 ui16Focus)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->ui16CurrentFocus = ui16Focus;
    if (psCam->sensor && psCam->sensor->set_focus)
        ret = psCam->sensor->set_focus(psCam->sensor_i2c, ui16Focus, &info);
    return ret;
}

IMG_RESULT DualBridge_SetFPS(SENSOR_HANDLE hHandle, double fps)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->curFps= fps;
    if (psCam->sensor && psCam->sensor->set_fps)
        ret = psCam->sensor->set_fps(psCam->sensor_i2c, fps, &info);
    return ret;
}

IMG_RESULT  DualBridge_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;
    double fps;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (psCam->sensor && psCam->sensor->get_fixedfps)
        ret = psCam->sensor->get_fixedfps(psCam->sensor_i2c, &fps, &info);

    psCam->fixedFps = (int)fps;
   *pfixed = (int)fps;
    return ret;
}

IMG_RESULT  DualBridge_Reset(SENSOR_HANDLE hHandle)
{
    IMG_RESULT ret = IMG_SUCCESS;
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (psCam->sensor && psCam->sensor->reset)
        ret = psCam->sensor->reset(psCam->sensor_i2c, &info);
    return ret;
}

IMG_UINT8* DualBridge_ReadCalibrationData(
    SENSOR_HANDLE hHandle,
    int product, int idx,
    IMG_FLOAT gain, IMG_UINT16* version)
{
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return 0;
    }

    if (psCam->sensor && psCam->sensor->read_calib_data)
        return psCam->sensor->read_calib_data(
                psCam->sensor_i2c, product, idx, gain, version, &info);

    return 0;
}
IMG_UINT8* DualBridge_ReadCalibrationVersion(
    SENSOR_HANDLE hHandle,
    int product, int idx,
    IMG_UINT16* version)
{
    SENSOR_MODE info;

    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &info);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return 0;
    }

    if (psCam->sensor && psCam->sensor->read_calib_version)
        return psCam->sensor->read_calib_version(
                psCam->sensor_i2c, product, idx, version, &info);

    return 0;
}

IMG_RESULT DualBridge_UpdateWBGain(
    SENSOR_HANDLE hHandle,
    int product, IMG_FLOAT gain)
{
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return 0;
    }

    if (psCam->sensor && psCam->sensor->update_wb_gain)
        return psCam->sensor->update_wb_gain(
                psCam->sensor_i2c, product, gain, 0);

    return IMG_SUCCESS;
}

IMG_RESULT DualBridge_ExchangeCalibDirection(SENSOR_HANDLE hHandle, int flag)
{
    DUAL_BRIDGE_STRUCT *psCam = container_of(hHandle, DUAL_BRIDGE_STRUCT, sFuncs);

    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("sensor not initialised\n");
        return 0;
    }

    if (psCam->sensor && psCam->sensor->exchange_calib_direction)
        return psCam->sensor->exchange_calib_direction(psCam->sensor_i2c, flag);

    return IMG_SUCCESS;
}
#endif //INFOTM_ISP

IMG_RESULT DualBridge_Create(SENSOR_HANDLE *phHandle, int index)
{
    IMG_RESULT ret = IMG_SUCCESS;
    DUAL_BRIDGE_STRUCT *psCam = NULL;

    psCam = (DUAL_BRIDGE_STRUCT *)IMG_CALLOC(1, sizeof(DUAL_BRIDGE_STRUCT));
    if (!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    *phHandle = &psCam->sFuncs;
    psCam->sFuncs.GetMode = DualBridge_GetMode;
    psCam->sFuncs.GetState = DualBridge_GetState;
    psCam->sFuncs.SetMode = DualBridge_SetMode;
    psCam->sFuncs.Enable = DualBridge_Enable;
    psCam->sFuncs.Disable = DualBridge_DisableSensor;
    psCam->sFuncs.Destroy = DualBridge_DestroySensor;

    psCam->sFuncs.GetGainRange = DualBridge_GetGainRange;
    psCam->sFuncs.GetCurrentGain = DualBridge_GetGain;
    psCam->sFuncs.SetGain = DualBridge_SetGain;

    psCam->sFuncs.GetExposureRange = DualBridge_GetExposureRange;
    psCam->sFuncs.GetExposure = DualBridge_GetExposure;
    psCam->sFuncs.SetExposure = DualBridge_SetExposure;

    psCam->sFuncs.GetFocusRange = DualBridge_GetFocusRange;
    psCam->sFuncs.GetCurrentFocus = DualBridge_GetCurrentFocus;
    psCam->sFuncs.SetFocus = DualBridge_SetFocus;

    psCam->sFuncs.GetInfo = DualBridge_GetInfo;

#ifdef INFOTM_ISP
    psCam->sFuncs.SetFlipMirror = DualBridge_SetFlipMirror;
    psCam->sFuncs.SetGainAndExposure = DualBridge_SetGainAndExposure;

    psCam->sFuncs.Reset = DualBridge_Reset;
    psCam->sFuncs.SetFPS = DualBridge_SetFPS;
    psCam->sFuncs.GetFixedFPS = DualBridge_GetFixedFPS;

    psCam->sFuncs.ReadSensorCalibrationData = DualBridge_ReadCalibrationData;
    psCam->sFuncs.ReadSensorCalibrationVersion = DualBridge_ReadCalibrationVersion;
    psCam->sFuncs.UpdateSensorWBGain = DualBridge_UpdateWBGain;
    psCam->sFuncs.ExchangeCalibDirection = DualBridge_ExchangeCalibDirection;
#endif //INFOTM_ISP

    psCam->bEnabled = IMG_FALSE;
    psCam->ui16CurrentMode = 0;
    psCam->ui8CurrentFlipping = SENSOR_FLIP_NONE;
    psCam->refClock = 24.0 * 1000 * 1000;

    ret = DualBridge_GetModeInfo(psCam, psCam->ui16CurrentMode, &(psCam->currMode));
    if(IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get initial mode information!\n");
        IMG_FREE(psCam);
        return IMG_ERROR_FATAL;
    }

    psCam->psSensorPhy = SensorPhyInit();
    if (!psCam->psSensorPhy)
    {
        LOG_ERROR("Failed to create sensor phy!\n");
        IMG_FREE(psCam);
        *phHandle = NULL;
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    return IMG_SUCCESS;
}

static IMG_RESULT DualBridge_GetModeInfo(DUAL_BRIDGE_STRUCT *psCam,
    IMG_UINT16 ui16Mode, SENSOR_MODE *info)
{
    double trow = 0.0;
    IMG_UINT32 framelenghtlines = currMode.ui16VerticalTotal;

    info->ui16Width = currMode.ui16Width;
    info->ui16Height = currMode.ui16Height;
    info->ui16HorizontalTotal = currMode.ui16HorizontalTotal;
    info->ui16VerticalTotal = currMode.ui16VerticalTotal;
    info->ui8SupportFlipping = SENSOR_FLIP_NONE;
    info->flFrameRate = currMode.flFrameRate;
    info->flPixelRate = currMode.flPixelRate;

    /*
     * line_length_pck in pixels, clk_pix in Hz, trow in micro seconds
     * (time in a row)
     * trow = (line_length_pck/2 / clk_pix) * 1000 * 1000;
     * Trow = (1600/36 *1000 * 1000) * 1000 *1000;
     */
    trow = (currMode.ui16HorizontalTotal / 2) / info->flPixelRate;

    info->ui32ExposureMax = (IMG_UINT32)floor(trow * framelenghtlines);
    /* Minimum exposure time in us (line period) (use the frmae length
     * lines entry to calculate this...*/
    info->ui32ExposureMin = (IMG_UINT32)floor(trow);
    info->ui8MipiLanes = currMode.ui8MipiLanes;
    info->ui8BitDepth = currMode.ui8BitDepth;
    info->dbExposureMin = trow;

    LOG_DEBUG("Trow = %f ExposureMin = %d\n", trow, info->ui32ExposureMin);

#if defined(INFOTM_ISP)
    psCam->fCurrentFps = info->flFrameRate - 8;
    psCam->fixedFps = psCam->fCurrentFps;
    psCam->initClk = framelenghtlines;
#endif
    return IMG_SUCCESS;
}

static void DualBridge_SensorPhyConfig(DUAL_BRIDGE_STRUCT *psCam,
        SENSOR_MODE mode)
{
    double ref = mode.flPixelRate * 1000 * 1000;
    IMG_UINT32 framelenghtlines = 0;
    IMG_UINT32 coarsetime = 1;
    double trow = 1.0;

    framelenghtlines = mode.ui16VerticalTotal;
    trow = (mode.ui16HorizontalTotal/2)/ mode.flPixelRate;/*one sensor Trow*/

    memcpy(&psCam->currMode, &mode, sizeof(mode));

    // setup the exposure setting information
    psCam->pClock = mode.flPixelRate;
    psCam->currMode.ui32ExposureMax = (IMG_UINT32)floor(trow * framelenghtlines);
    psCam->currMode.ui32ExposureMin = (IMG_UINT32)floor(trow);
    psCam->currMode.dbExposureMin = trow;

    psCam->ui32Exposure = psCam->currMode.ui32ExposureMin * coarsetime;
    if (psCam->sensor && psCam->sensor->get_exposure)
        psCam->sensor->get_exposure(psCam->sensor_i2c,
                &psCam->ui32Exposure, &psCam->currMode);

#if defined(ENABLE_AE_DEBUG_MSG)
    printf("===========================\n");
    printf("ref     = %f Hz\n", ref);
    printf("ui32ExposureMax = %d\n", psCam->currMode.ui32ExposureMax);
    printf("ui32ExposureMin = %d\n", psCam->currMode.ui32ExposureMin);
    printf("ui32Exposure    = %d\n", psCam->ui32Exposure);
    printf("imager = %d\n", psCam->sensor_cfg->imager);
    printf("===========================\n");
#endif

    psCam->currMode.ui16Width =  mode.ui16Width;
    psCam->currMode.ui16HorizontalTotal = mode.ui16HorizontalTotal;
    psCam->currMode.ui8MipiLanes = psCam->bridge_cfg->mipi_lanes;

    psCam->imager = psCam->bridge_cfg->imager;
    psCam->flGain = 1.0;

#if defined(INFOTM_ISP)
    psCam->fCurrentFps = mode.flFrameRate;
    psCam->fixedFps = (IMG_UINT32)psCam->fCurrentFps;
    psCam->initClk = framelenghtlines;
#endif

    //Config ISP Gasket
    psCam->psSensorPhy->psGasket->bParallel = psCam->bridge_cfg->parallel;
    psCam->psSensorPhy->psGasket->uiGasket= psCam->bridge_cfg->imager;
    psCam->psSensorPhy->psGasket->uiWidth = mode.ui16Width -1;
    psCam->psSensorPhy->psGasket->uiHeight = mode.ui16Height-1;
    psCam->psSensorPhy->psGasket->bVSync = psCam->bridge_cfg->vsync;
    psCam->psSensorPhy->psGasket->bHSync = psCam->bridge_cfg->hsync;
    psCam->psSensorPhy->psGasket->ui8ParallelBitdepth = mode.ui8BitDepth;
}

static IMG_RESULT doSetup(DUAL_BRIDGE_STRUCT *psCam, IMG_UINT16 ui16Mode,
    IMG_BOOL bFlipHorizontal,IMG_BOOL bFlipVertical)
{
    int  ret = 0;
    BRIDGE_STRUCT*bridge = NULL;

    bridge = Bridge_Create();
    if (!bridge)
    {
        LOG_ERROR("Create Bridge Failed\n");
        return -1;
    }

    ret = Bridge_DoSetup();
    if (! ret)
    {
        psCam->bridge = bridge;
        if (psCam->bridge)
        {
            psCam->sensor_cfg = &(bridge->sensor_cfg);
            psCam->bridge_cfg = &(bridge->bridge_cfg);
            psCam->sensor =bridge->sensor;
            psCam->bridge_dev = bridge->dev;
            psCam->bridge_i2c = &(bridge->client);
            psCam->sensor_i2c = &(bridge->sensor_client);
        }

        psCam->imager = psCam->bridge_cfg->imager;
        LOG_DEBUG("%s sensor=0x%x sensor_i2c=x%x\n",
            __func__, psCam->sensor, psCam->sensor_i2c);

         ret = DualBridge_CheckChipID(psCam, psCam->sensor_i2c);
         if (ret != IMG_SUCCESS)
             LOG_ERROR("Check Bridge Sensor ID Failed\n");

        LOG_DEBUG("Sensor ready to go\n");

        if (psCam->bridge_cfg->width > 0)
            currMode.ui16Width = psCam->bridge_cfg->width;
        else
            currMode.ui16Width = asModes[ui16Mode].ui16Width;

        if (psCam->bridge_cfg->height > 0)
            currMode.ui16Height= psCam->bridge_cfg->height;
        else
            currMode.ui16Height= asModes[ui16Mode].ui16Height;

        if ( psCam->bridge_cfg->widthTotal> 0)
            currMode.ui16HorizontalTotal = psCam->bridge_cfg->widthTotal;
        else
            currMode.ui16HorizontalTotal = asModes[ui16Mode].ui16HorizontalTotal;

        if ( psCam->bridge_cfg->heightTotal > 0)
            currMode.ui16VerticalTotal  = psCam->bridge_cfg->heightTotal;
        else
            currMode.ui16VerticalTotal = asModes[ui16Mode].ui16VerticalTotal;

        if (psCam->bridge_cfg->fps > 0)
            currMode.flFrameRate = psCam->bridge_cfg->fps*1.0;
        else
            currMode.flFrameRate = asModes[ui16Mode].flFrameRate;

        if (psCam->bridge_cfg->pclk > 0)
            currMode.flPixelRate = psCam->bridge_cfg->pclk*1.0;
        else
            currMode.flPixelRate = asModes[ui16Mode].flPixelRate;

        if (psCam->bridge_cfg->bits > 0)
            currMode.ui8BitDepth = psCam->bridge_cfg->bits;
        else
            currMode.ui8BitDepth = asModes[ui16Mode].ui8BitDepth;

        LOG_INFO("name = %s w = %d, h = %d, tw = %d, th = %d\n "
                    "fps = %f pclk =%f bits=%d i2c_bits = %d mipi_lanes=%d imager=%d\n"
                    "hsync = %d vsync=%d mosaic=%s parallel=%d\n",
                    psCam->bridge_cfg->sensor_name,
                    currMode.ui16Width, currMode.ui16Height,
                    currMode.ui16HorizontalTotal, currMode.ui16VerticalTotal,
                    currMode.flFrameRate, currMode.flPixelRate,
                    currMode.ui8BitDepth,
                    psCam->bridge_cfg->i2c_data_bits,
                    psCam->bridge_cfg->mipi_lanes,
                    psCam->bridge_cfg->imager,
                    psCam->bridge_cfg->hsync,
                    psCam->bridge_cfg->vsync,
                    psCam->bridge_cfg->mosaic_type,
                    psCam->bridge_cfg->parallel);

            DualBridge_SensorPhyConfig(psCam,currMode);
    }
    else
    {
        return ret;
    }

    usleep(2000);
    Bridge_Enable(1);

    return ret;
}
