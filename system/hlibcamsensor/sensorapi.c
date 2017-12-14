/**
******************************************************************************
@file sensorapi.c

@brief Implementation of platform sensor integration file

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
#include "sensorapi/sensorapi.h"

#if defined(SENSOR_P401)
#include "sensors/p401.h"
#endif

#if defined(SENSOR_OV4688)
#include "sensors/ov4688.h"
#endif

#if defined(SENSOR_IIFDG)
#include "sensors/iifdatagen.h"
#endif

#if defined(SENSOR_DG)
#include "sensors/dgsensor.h"
#endif

#if defined(SENSOR_AR330_DVP)
#include "sensors/ar330dvp.h"
#endif

#if defined(SENSOR_DUAL_BRIDGE)
#include "sensors/bridge/dual_bridge.h"
#endif

#if defined(SENSOR_NT99142_DVP)
#include "sensors/nt99142dvp.h"
#endif

#if defined(SENSOR_AR330_MIPI)
#include "sensors/ar330mipi.h"
#endif

#if defined(SENSOR_OV2710)
#include "sensors/ov2710.h"
#endif

#if defined(SENSOR_OV4689)
#include "sensors/ov4689.h"
#endif

#if defined(SENSOR_IMX322_DVP)
#include "sensors/imx322dvp.h"
#endif

#if defined(SENSOR_IMX179_MIPI)
#include "sensors/imx179mipi.h"
#endif

#if defined(SENSOR_OV683_MIPI)
#include "sensors/ov683Dual.h"
#endif

#if defined(SENSOR_SC2135_DVP)
#include "sensors/sc2135dvp.h"
#endif

#if defined(SENSOR_SC2235_DVP)
#include "sensors/sc2235dvp.h"
#endif

#if defined(SENSOR_SC1135_DVP)
#include "sensors/sc1135dvp.h"
#endif
#if defined(SENSOR_99141_DVP)
#include "sensors/stk3855dual720dvp.h"
#endif

#if defined(SENSOR_SC3035_DVP)
#include "sensors/sc3035dvp.h"
#endif

#if defined(SENSOR_HM2131_MIPI)
#include "sensors/hm2131Dual.h"
#endif

//#include "sensors/iifdatagen.h"

#include "img_errors.h"

#define LOG_TAG "SENSOR"
#include <felixcommon/userlog.h>

IMG_RESULT (*InitialiseSensors[])(SENSOR_HANDLE *phHandle, int index) = {
#if !defined(INFOTM_ISP)
    IIFDG_Create,
#endif
#if defined(SENSOR_IIFDG)
	IIFDG_Create,
#endif
#if defined(SENSOR_DG)
	DGCam_Create,
#endif
#if defined(SENSOR_AR330_DVP)
	AR330DVP_Create,
#endif
#if defined(SENSOR_DUAL_BRIDGE)
	DualBridge_Create,
#endif
#if defined(SENSOR_NT99142_DVP)
	NT99142DVP_Create,
#endif
#if defined(SENSOR_AR330_MIPI)
	AR330_Create_MIPI,
#endif
#if defined(SENSOR_IMX322_DVP)
	IMX322_Create_DVP,
#endif
#if defined(SENSOR_IMX179_MIPI)
	IMX179_Create_MIPI,
#endif
#if defined(SENSOR_OV683_MIPI)
	OV683Dual_Create,
#endif
//#if defined(SENSOR_P401)
//	P401_Create,
//#endif
//#if defined(SENSOR_OV4688)
//	OV4688_Create,
//#endif
#if defined(SENSOR_OV2710)
	OV2710_Create,
#endif
#if defined(SENSOR_OV4689)
	OV4689_Create,
#endif
#if defined(SENSOR_SC2135_DVP)
	SC2135DVP_Create,
#endif
#if defined(SENSOR_SC2235_DVP)
	SC2235DVP_Create,
#endif
#if defined(SENSOR_SC1135_DVP)
	SC1135DVP_Create,
#endif
#if defined(SENSOR_SC3035_DVP)
	SC3035DVP_Create,
#endif
#if defined(SENSOR_99141_DVP)
	STK3855DUAL720DVP_Create,
#endif
#if defined(SENSOR_HM2131_MIPI)
	HM2131Dual_Create,
#endif
};

const char *Sensors[]={
#if !defined(INFOTM_ISP)
    IIFDG_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_IIFDG)
	IIFDG_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_DG)
	EXTDG_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_AR330_DVP)
	AR330DVP_SENSOR_INFO_NAME,
#endif

#if defined(SENSOR_DUAL_BRIDGE)
	DUAL_BRIDGE_INFO_NAME,
#endif

#if defined(SENSOR_NT99142_DVP)
	NT99142DVP_SENSOR_INFO_NAME,
#endif

#if defined(SENSOR_AR330_MIPI)
	AR330MIPI_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_IMX322_DVP)
	IMX322DVP_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_IMX179_MIPI)
	IMX179MIPI_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_OV683_MIPI)
	OV683Dual_SENSOR_INFO_NAME,
#endif

//#if defined(SENSOR_P401)
//	P401_SENSOR_INFO_NAME,
//#endif
//#if defined(SENSOR_OV4688)
//	OV4688_SENSOR_INFO_NAME,
//#endif
#if defined(SENSOR_OV2710)
	OV2710_SENSOR_INFO_NAME,
#endif

#if defined(SENSOR_OV4689)
	OV4689_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_SC2135_DVP)
	SC2135DVP_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_SC2235_DVP)
	SC2235DVP_SENSOR_INFO_NAME,
#endif

#if defined(SENSOR_SC1135_DVP)
	SC1135DVP_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_SC3035_DVP)
	SC3035DVP_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_99141_DVP)
	STK3855DUAL720DVP_SENSOR_INFO_NAME,
#endif
#if defined(SENSOR_HM2131_MIPI)
	HM2131Dual_SENSOR_INFO_NAME,
#endif
	NULL,
};

/*
 * Sensor controls
 */

const char **Sensor_ListAll(void) {
	return Sensors;
}

unsigned Sensor_NumberSensors(void) {
    return (sizeof(InitialiseSensors) / sizeof(InitialiseSensors[0]));
}

void Sensor_PrintAllModes(FILE *f)
{
    const char **pszSensors = Sensor_ListAll();
    int s = 0;
    int ret;

    fprintf(f, "Available sensors in sensor API:\n");
    while(pszSensors[s])
    {
        int strn = strlen(pszSensors[s]);
        SENSOR_HANDLE hSensorHandle = NULL;
#ifdef INFOTM_ISP
        if (Sensor_Initialise(s, &hSensorHandle, 0) == IMG_SUCCESS)
#else
        if (Sensor_Initialise(s, &hSensorHandle) == IMG_SUCCESS)
#endif
        {
            SENSOR_MODE mode = {0,};
            SENSOR_INFO info = {0,};
            int m = 0;

            ret = Sensor_GetInfo(hSensorHandle, &info);

            fprintf(f, "\t%d: %s (v%s imager %d)\n",
                s, pszSensors[s], info.pszSensorVersion, (int)info.ui8Imager);

            while ( Sensor_GetMode(hSensorHandle, m, &mode) == IMG_SUCCESS )
            {
                double bRate = mode.flPixelRate * mode.ui8BitDepth;
                char c = ' ';
                fprintf(f, "\t\tmode %2d: %5dx%5d @%.2f %ubit (total %dx%d "\
                    "mipi_lane=%u) exposure=(%u..%u) %s\n",
                    m, mode.ui16Width, mode.ui16Height, mode.flFrameRate,
                    mode.ui8BitDepth,
                    mode.ui16HorizontalTotal, mode.ui16VerticalTotal,
                    mode.ui8MipiLanes,
                    mode.ui32ExposureMin, mode.ui32ExposureMax,
                    mode.ui8SupportFlipping==SENSOR_FLIP_NONE?"flipping=none":
                    mode.ui8SupportFlipping==SENSOR_FLIP_BOTH?
                    "flipping=horizontal|vertical":
                    mode.ui8SupportFlipping==SENSOR_FLIP_HORIZONTAL?
                    "flipping=horizontal":"flipping=vertical");
                fprintf(f, "\t\t         pixel rate %.4lf Mpx/s, ",
                    mode.flPixelRate);
                if (bRate >= 1000000000)
                {
                    c = 'G';
                    bRate = bRate / 1000.0;
                }
                else
                {
                    c = 'M';
                }
                if (mode.ui8MipiLanes > 0)
                {
                    fprintf(f, "bit rate %.4lf %cbits/s (per mipi lane)\n",
                        (bRate / mode.ui8MipiLanes), c);
                }
                else
                {
                    fprintf(f, "bit rate %.4lf %cbits/s\n",
                        bRate, c);
                }
                m++;
            }
#ifdef SENSOR_AR330
            if (strncmp(pszSensors[s], AR330_SENSOR_INFO_NAME,
                strlen(AR330_SENSOR_INFO_NAME)) == 0)
            {
                fprintf(f, "\t\tmode %d: special register load from file\n",
                    AR330_SPECIAL_MODE);
            }
#endif
            Sensor_Destroy(hSensorHandle);
        }
        else
        {
            printf("\t%d: %s - no modes display available\n",
                s, pszSensors[s]);
        }

        s++;
    }
}

#ifdef INFOTM_ISP
IMG_RESULT Sensor_Initialise(IMG_UINT16 nSensor, SENSOR_HANDLE *phHandle, int index) {
#else
IMG_RESULT Sensor_Initialise(IMG_UINT16 nSensor, SENSOR_HANDLE *phHandle) {
#endif
    if (!phHandle) {
        LOG_ERROR("phHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
	if (nSensor < (sizeof(InitialiseSensors) / sizeof(InitialiseSensors[0]))) {
        return InitialiseSensors[nSensor](phHandle, index);
    }

    LOG_ERROR("nSensor=%d supports %d sensors\n", nSensor,
        (sizeof(InitialiseSensors) / sizeof(InitialiseSensors[0])));
    return IMG_ERROR_INVALID_PARAMETERS;
}

IMG_RESULT Sensor_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
                           SENSOR_MODE *psModes) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!psModes) {
        LOG_ERROR("psModes is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetMode) {
        return (hHandle->GetMode)(hHandle, nIndex, psModes);

    }
    LOG_ERROR("GetMode is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!psStatus) {
        LOG_ERROR("psStatus is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetState) {
        return (hHandle->GetState)(hHandle, psStatus);
    }
    LOG_ERROR("GetState is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
                          IMG_UINT8 ui8Flipping) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->SetMode) {
        return (hHandle->SetMode)(hHandle, nMode, ui8Flipping);
    }
    LOG_ERROR("SetMode is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_Enable(SENSOR_HANDLE hHandle) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->Enable) {
        return (hHandle->Enable)(hHandle);
    }
    LOG_ERROR("Enable is not defined!\n");
    return IMG_ERROR_FATAL;

}


IMG_RESULT Sensor_Disable(SENSOR_HANDLE hHandle) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->Disable) {
        return (hHandle->Disable)(hHandle);
    }
    LOG_ERROR("Disable is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_Destroy(SENSOR_HANDLE hHandle) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->Destroy) {
        return (hHandle->Destroy)(hHandle);
    }
    LOG_ERROR("Destroy is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!psInfo) {
        LOG_ERROR("psInfo is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetInfo) {
        IMG_RESULT ret;

        ret = Sensor_GetState(hHandle, &(psInfo->sStatus));
        if(ret) {
            LOG_ERROR("failed to get sensor's state\n");
            return ret;
        }

        ret = (hHandle->GetInfo)(hHandle, psInfo);
        if(ret) {
            LOG_ERROR("failed to get sensor's info\n");
            return ret;
        }
        else {
            LOG_DEBUG("original_mosaic=%d flip=0x%x enabled_mosaic=%d\n",
                (int)psInfo->eBayerOriginal, psInfo->sStatus.ui8Flipping,
                psInfo->eBayerEnabled);
        }

        ret = Sensor_GetMode(hHandle, psInfo->sStatus.ui16CurrentMode,
            &(psInfo->sMode));
        if(ret) {
            LOG_ERROR("failed to get sensor's mode\n");
            return ret;
        }

        return IMG_SUCCESS;
    }
    LOG_ERROR("GetInfo is not defined!\n");
    return IMG_ERROR_FATAL;
}

/*
 * Focus control
 */

IMG_RESULT Sensor_GetFocusRange(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Min,
                                IMG_UINT16 *pui16Max) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!pui16Min || !pui16Max) {
        LOG_ERROR("pui16Min or pui16Max is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists - optional implementation */
    if (hHandle->GetFocusRange) {
        return (hHandle->GetFocusRange)(hHandle, pui16Min, pui16Max);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}


IMG_RESULT Sensor_GetCurrentFocus(SENSOR_HANDLE hHandle,
                                  IMG_UINT16 *pui16Current) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!pui16Current) {
        LOG_ERROR("pui16Current is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists - optional implementation */
    if (hHandle->GetCurrentFocus) {
        return (hHandle->GetCurrentFocus)(hHandle, pui16Current);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}


IMG_RESULT Sensor_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* could also check that focus fits in min/max for the mode */

    /* Check function exists - optional implementation */
    if (hHandle->SetFocus) {
        return (hHandle->SetFocus)(hHandle, ui16Focus);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

/*
 * Exposure and gain controls
 */

IMG_RESULT Sensor_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
                               double *pflMax, IMG_UINT8 *puiContexts) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!pflMin || !pflMax || !puiContexts) {
        LOG_ERROR("pflMin, pflMax or puiContexts is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetGainRange) {
        return (hHandle->GetGainRange)(hHandle, pflMin, pflMax, puiContexts);
    }
    LOG_ERROR("GetGainRange is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_GetCurrentGain(SENSOR_HANDLE hHandle, double *pflCurrent,
                                 IMG_UINT8 ui8Context) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!pflCurrent) {
        LOG_ERROR("pflCurrent is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetCurrentGain) {
        return (hHandle->GetCurrentGain)(hHandle, pflCurrent, ui8Context);
    }
    LOG_ERROR("GetCurrentGain is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_SetGain(SENSOR_HANDLE hHandle, double flGain,
                          IMG_UINT8 ui8Context) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* could also check that gain fits in min/max for the mode */

    /* Check function exists */
    if (hHandle->SetGain) {
        return (hHandle->SetGain)(hHandle, flGain, ui8Context);
    }
    LOG_ERROR("SetGain is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
                                   IMG_UINT32 *pui32Max,
                                   IMG_UINT8 *pui8Contexts) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!pui32Min || !pui32Max || !pui8Contexts) {
        LOG_ERROR("pui32Min, pui32Max or pui8Contexts is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetExposureRange) {
        return (hHandle->GetExposureRange)(hHandle, pui32Min, pui32Max,
            pui8Contexts);
    }
    LOG_ERROR("GetExposureRange is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_GetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Exposure,
                              IMG_UINT8 ui8Context) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check parameters */
    if (!pui32Exposure) {
        LOG_ERROR("pui32Exposure is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetExposure) {
        return (hHandle->GetExposure)(hHandle, pui32Exposure, ui8Context);
    }
    LOG_ERROR("GetExposure is not defined!\n");
    return IMG_ERROR_FATAL;
}


IMG_RESULT Sensor_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure,
                              IMG_UINT8 ui8Context) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* could also check that given exposure fits in min/max for the mode */

    /* Check function exists */
    if (hHandle->SetExposure) {
        return (hHandle->SetExposure)(hHandle, ui32Exposure, ui8Context);
    }
    LOG_ERROR("SetExposure is not defined!\n");
    return IMG_ERROR_FATAL;
}

/*
 * miscellaneous controls
 */

IMG_RESULT Sensor_Insert(SENSOR_HANDLE hHandle) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists - optional implementation */
    if (hHandle->Insert) {
        return (hHandle->Insert)(hHandle);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_WaitProcessed(SENSOR_HANDLE hHandle) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists - optional implementation */
    if (hHandle->WaitProcessed) {
        return (hHandle->WaitProcessed)(hHandle);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_ConfigureFlash(SENSOR_HANDLE hHandle, IMG_BOOL bAlwaysOn,
                                 IMG_INT16 i16FrameDelay, IMG_INT16 i16Frames,
                                 IMG_UINT16 ui16FlashPulseWidth) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists - optional implementation */
    if (hHandle->ConfigureFlash) {
        return (hHandle->ConfigureFlash)(hHandle, bAlwaysOn, i16FrameDelay,
                                            i16Frames, ui16FlashPulseWidth);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

#ifdef INFOTM_ISP
IMG_RESULT Sensor_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists - optional implementation */
    if (hHandle->SetFlipMirror) {
        return (hHandle->SetFlipMirror)(hHandle, flag);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed)
{
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	/* Check function exists - optional implementation */
	if (hHandle->GetFixedFPS) {
		return (hHandle->GetFixedFPS)(hHandle, pfixed);
	}
	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_SetFPS(SENSOR_HANDLE hHandle, double fps)
{
	 /* Check handle */
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	/* Check function exists - optional implementation */
	if (hHandle->SetFPS) {
		return (hHandle->SetFPS)(hHandle, fps);
	}
	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
{
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists - optional implementation */
    if (hHandle->SetGainAndExposure) {
        return (hHandle->SetGainAndExposure)(hHandle, flGain, ui32Exposure, ui8Context);
    }
    return IMG_ERROR_NOT_SUPPORTED;    
}

IMG_RESULT Sensor_SetResolution(SENSOR_HANDLE hHandle, int imgW, int imgH)
{
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	/* Check function exists - optional implementation */
	if (hHandle->SetResolution) {
		return (hHandle->SetResolution)(hHandle, imgW, imgH);
	}
	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_GetSnapRes(SENSOR_HANDLE hHandle, reslist_t *presList)
{
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if (hHandle->GetSnapRes) {
		return (hHandle->GetSnapRes)(hHandle, presList);
	}

	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_Reset(SENSOR_HANDLE hHandle) {
    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->Reset) {
        return (hHandle->Reset)(hHandle);
    }
    LOG_ERROR(" Sensor Reset is not defined!\n");
    return IMG_ERROR_FATAL;
}

IMG_RESULT Sensor_GetGasketNum(SENSOR_HANDLE hHandle, IMG_UINT32 *GasketNum)
{
    SENSOR_INFO psInfo;

    /* Check handle */
    if (!hHandle) {
        LOG_ERROR("hHandle is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Check function exists */
    if (hHandle->GetInfo) {
        (hHandle->GetInfo)(hHandle,&psInfo);
        *GasketNum = psInfo.ui8Imager;
        return IMG_SUCCESS;
    }

    return IMG_ERROR_FATAL;
}

IMG_UINT8* Sensor_ReadSensorCalibrationData(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version)
{
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if (hHandle->ReadSensorCalibrationData) {
		return (hHandle->ReadSensorCalibrationData)(hHandle, infotm_method, sensor_index, awb_convert_gain, otp_calibration_version);
	}

	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_UINT8* Sensor_ReadSensorCalibrationVersion(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version)
{
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if (hHandle->ReadSensorCalibrationVersion) {
		return (hHandle->ReadSensorCalibrationVersion)(hHandle, infotm_method, sensor_index, otp_calibration_version);
	}

	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_UpdateSensorWBGain(SENSOR_HANDLE hHandle, int infotm_method, IMG_FLOAT awb_convert_gain)
{
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if (hHandle->UpdateSensorWBGain) {
		return (hHandle->UpdateSensorWBGain)(hHandle, infotm_method, awb_convert_gain);
	}

	return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT Sensor_ExchangeCalibDirection(SENSOR_HANDLE hHandle, int flag)
{
	if (!hHandle) {
		LOG_ERROR("hHandle is NULL\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if (hHandle->ExchangeCalibDirection) {
		return (hHandle->ExchangeCalibDirection)(hHandle, flag);
	}

	return IMG_ERROR_NOT_SUPPORTED;
}

#endif //INFOTM_ISP

