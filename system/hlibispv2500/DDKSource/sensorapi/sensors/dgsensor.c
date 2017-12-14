/**
******************************************************************************
@file dummycam.c

@brief DG camera implementation

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
#include "sensors/dgsensor.h"
#include <img_types.h>
#include <img_errors.h>

#include <stdio.h>

#include <ci/ci_api.h>
#include <dg/dg_api.h>
#include <sim_image.h>

//#define DG_SENSOR_DEBUG_ENABLED

#define LOG_TAG "ExtDG_SENSOR"
#include <felixcommon/userlog.h>

#define DG_PARAM_FRAME_RATE (5)

#define DG_SENSOR_MODE_ID (0)
#define DG_SENSOR_INFO_NUMBER (0.0)
#define DG_SENSOR_INFO_FOCAL_LENGTH (0)
#define DG_SENSOR_INFO_WELL_DEPTH (0)
#define DG_SENSOR_INFO_BIT_DEPTH (0)
#define DG_SENSOR_INFO_READ_NOISE (0.0)
#define DG_SENSOR_FOCUS (0)
#define DG_SENSOR_FOCUS_MIN (0)
#define DG_SENSOR_FOCUS_MAX (0)
#define DG_SENSOR_GAIN (1.0)
#define DG_SENSOR_GAIN_MIN (1.0)
#define DG_SENSOR_GAIN_MAX (1.0)
#define DG_SENSOR_EXPOSURE (0)
#define DG_SENSOR_EXPOSURE_MIN (0)
#define DG_SENSOR_EXPOSURE_MAX (0)

enum {
    DG_SENSOR_DISABLED,
    DG_SENSOR_ENABLED
};

typedef struct _dgcam_struct
{
    SENSOR_FUNCS sFuncs;

    // local stuff goes here.
    IMG_UINT16 ui16CurrentModeIndex;
    IMG_INT16 i16CurrentFrameIndex;
    IMG_BOOL nEnabled;

    DG_CAMERA *pDGCamera;
    CI_CONNECTION *pConnection; // used to initialise DG so it should be CI_CONNECTION*

    char *pszFilename;
    sSimImageIn *psImage;
    IMG_UINT32 ui32FrameNoToLoad;
    IMG_BOOL8 bFirstFrame;
    IMG_BOOL8 bIncrementFrameNo; // Every shoot frame number will be incremented
    IMG_UINT32 ui32FrameCount; // How many frames FLX file contains - loaded from FLX file
    IMG_UINT32 ui32BuffersCount;
    eDG_MEMFMT buffersFormat;
//    IMG_BOOL bUseMipi; // determined from pConnection and used context
    IMG_BOOL8 bMIPI_LF; // If MIPI has line flags.
    IMG_UINT8 ui8DGContext; // data generator to use
    IMG_UINT16 ui16HBlanking; // Number of horizontal blanking columns + 40.
    IMG_UINT16 ui16VBlanking; // Number of vertical blanking lines.
} DGCAM_STRUCT;

static unsigned int g_nDGCamera = 0;

static IMG_RESULT DGCam_useMIPI(DGCAM_STRUCT *psCam, IMG_BOOL *pbUseMIPI)
{
    IMG_ASSERT(psCam);
    if ( psCam->pConnection && psCam->ui8DGContext < CI_N_IMAGERS )
    {
        *pbUseMIPI = ((psCam->pConnection->sHWInfo.gasket_aType[psCam->ui8DGContext] & CI_GASKET_MIPI) != 0);
        return IMG_SUCCESS;
    }
    LOG_ERROR("Need to set the CONNECTION before checking for MIPI usage\n");
    return IMG_ERROR_INVALID_PARAMETERS;
}

IMG_RESULT DGCam_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16ModeIndex,
                             SENSOR_MODE *psModes)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psModes)
    {
        LOG_ERROR("psModes is NULL.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!psCam->psImage)
    {
        LOG_ERROR("Sensor mode not available. Provide source file extended parameter first.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // Only one mode available.
    if (ui16ModeIndex == DG_SENSOR_MODE_ID)
    {
        psModes->ui8BitDepth = psCam->psImage->info.ui8BitDepth;
        psModes->ui16Width = psCam->psImage->info.ui32Width;
        psModes->ui16Height = psCam->psImage->info.ui32Height;
        psModes->flFrameRate = DG_PARAM_FRAME_RATE;
        psModes->ui16VerticalTotal = psCam->psImage->info.ui32Height+psCam->ui16VBlanking;
        psModes->ui8SupportFlipping = SENSOR_FLIP_NONE;
        return IMG_SUCCESS;
    }
    else
    {
        LOG_WARNING("Cannot get sensor mode with index %d out of range. "
                          "Only index %d is supported.\n", ui16ModeIndex,
                          DG_SENSOR_MODE_ID);
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
    }
}

IMG_RESULT DGCam_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    psStatus->eState = (psCam->nEnabled ? SENSOR_STATE_RUNNING : SENSOR_STATE_IDLE);
    psStatus->ui16CurrentMode = psCam->ui16CurrentModeIndex;
    psStatus->ui8Flipping = SENSOR_FLIP_NONE;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16ModeIndex, IMG_UINT8 ui8Flipping)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    LOG_DEBUG("Setting sensor mode = %d\n", ui16ModeIndex);
    if (DG_SENSOR_MODE_ID == ui16ModeIndex && SENSOR_FLIP_NONE == ui8Flipping)
    {
        psCam->ui16CurrentModeIndex = ui16ModeIndex;
        return IMG_SUCCESS;
    }
    else
    {
        LOG_ERROR("Mode %d (flipH=%d flipV=%d) not supported.\n", ui16ModeIndex, 
            (int)(ui8Flipping&SENSOR_FLIP_HORIZONTAL), (int)(ui8Flipping&SENSOR_FLIP_VERTICAL));
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
    }
}

#ifdef DG_SENSOR_DEBUG_ENABLED
void printSetup(SENSOR_HANDLE hHandle)
{
    const char *bufferFormatName = NULL;
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    IMG_BOOL bUseMipi = IMG_FALSE;
    DGCam_useMIPI(psCam, &bUseMipi);

    LOG_DEBUG("Enabling DG sensor for the following parameters:\n");
    LOG_DEBUG("- image width = %"IMG_SIZEPR"u\n", psCam->pDGCamera->uiWidth);
    LOG_DEBUG("- image height = %"IMG_SIZEPR"u\n", psCam->pDGCamera->uiHeight);

    switch (psCam->pDGCamera->eBufferFormats)
    {
    case DG_MEMFMT_BT656_10:
        bufferFormatName = "DG_MEMFMT_BT656_10";
        break;
    case DG_MEMFMT_BT656_12:
        bufferFormatName = "DG_MEMFMT_BT656_12";
        break;
    case DG_MEMFMT_MIPI_RAW10:
        bufferFormatName = "DG_MEMFMT_MIPI_RAW10";
        break;
    case DG_MEMFMT_MIPI_RAW12:
        bufferFormatName = "DG_MEMFMT_MIPI_RAW12";
        break;
    case DG_MEMFMT_MIPI_RAW14:
        bufferFormatName = "DG_MEMFMT_MIPI_RAW14";
        break;
    default:
        bufferFormatName = "undefined";
    }

    LOG_DEBUG("- image buffer format = %s\n", bufferFormatName);
    LOG_DEBUG("- use MIPI = %s\n",
                    bUseMipi ? "yes" : "no");
    LOG_DEBUG("- MIPI has long format = %s\n",
                    psCam->pDGCamera->bMIPI_LF ? "yes" : "no");
    LOG_DEBUG("- number of vertical blanking lines = %d\n",
                    psCam->pDGCamera->ui16VertBlanking);
    LOG_DEBUG("- number of horizontal blanking columns = %d\n",
                    psCam->pDGCamera->ui16HoriBlanking);
    LOG_DEBUG("- context of DG = %d\n", psCam->pDGCamera->ui8DGContext);
}
#endif

IMG_RESULT DGCam_Enable(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    IMG_BOOL bUseMipi;
    IMG_RESULT res;

    DGCam_useMIPI(psCam, &bUseMipi);

    LOG_INFO("Enabling DG Camera!\n");

    if (psCam->nEnabled)
    {
        LOG_WARNING("DG camera already enabled.");
        return IMG_SUCCESS;
    }
    if ( psCam->pDGCamera == NULL )
    {
        LOG_ERROR("DGCamera object is not initialised! Was the connection to the driver a success?\n");
        return IMG_ERROR_FATAL;
    }

    if (psCam->ui32BuffersCount == 0)
    {
        LOG_ERROR("Buffers count is 0. Provide NBuffer extended parameter first.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (psCam->psImage == NULL ||
        psCam->psImage->info.ui32Width == 0 ||
        psCam->psImage->info.ui32Height == 0)
    {
        LOG_ERROR("Image size is unknown (%dx%d). Provide source file extended parameter first.\n",
                        psCam->psImage ? (IMG_INT32)psCam->psImage->info.ui32Width : -1,
                        psCam->psImage ? (IMG_INT32)psCam->psImage->info.ui32Height : -1);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

#if FELIX_MIN_H_BLANKING > 0
    if (psCam->ui16HBlanking < FELIX_MIN_H_BLANKING)
    {
        LOG_ERROR("Incorrect number of horizontal blanking columns (%d). "
                        "Provide blanking extended parameter first.\n",
                        psCam->ui16HBlanking);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
#endif
#if FELIX_MIN_V_BLANKING > 0
    if (psCam->ui16VBlanking < FELIX_MIN_V_BLANKING)
    {
        LOG_ERROR("Incorrect number of vertical blanking lines (%d). "
                        "Provide blanking extended parameter first.\n",
                        psCam->ui16VBlanking);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
#endif

    if (psCam->psImage->info.ui8BitDepth <= 10)
    {
        if (bUseMipi)
        {
            psCam->buffersFormat = DG_MEMFMT_MIPI_RAW10;
        }
        else
        {
            psCam->buffersFormat = DG_MEMFMT_BT656_10;
        }
    }
    else if (psCam->psImage->info.ui8BitDepth <= 12)
    {
        if (bUseMipi)
        {
            psCam->buffersFormat = DG_MEMFMT_MIPI_RAW12;
        }
        else
        {
            psCam->buffersFormat = DG_MEMFMT_BT656_12;
        }
    }
    else
    {
        LOG_ERROR("Colour depth not recognized.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    // Use values provided as extended parameters
    psCam->pDGCamera->uiWidth = psCam->psImage->info.ui32Width;
    psCam->pDGCamera->uiHeight = psCam->psImage->info.ui32Height;
    psCam->pDGCamera->eBufferFormats = psCam->buffersFormat;
    psCam->pDGCamera->bMIPI_LF = psCam->bMIPI_LF;

    if (psCam->ui16HBlanking > DG_H_BLANKING_SUB)
    {
        // HW applies blanking = reg value + 40
        psCam->pDGCamera->ui16HoriBlanking = psCam->ui16HBlanking-DG_H_BLANKING_SUB;
    }
    else
    {
        // unlikely because checking that DGCam_H_BLANKING_SUB < FELIX_MIN_H_BLANKING
        LOG_WARNING("blanking smaller than min blanking %d\n", DG_H_BLANKING_SUB);
        psCam->pDGCamera->ui16HoriBlanking = 0;
    }
    psCam->pDGCamera->ui16VertBlanking = psCam->ui16VBlanking;
    psCam->pDGCamera->ui8DGContext = psCam->ui8DGContext;

#ifdef DGCam_SENSOR_DEBUG_ENABLED
    printSetup(hHandle);
#endif

    res = DG_CameraRegister(psCam->pDGCamera);
    if(res != IMG_ERROR_ALREADY_INITIALISED)
    {
        if (res != IMG_SUCCESS)
        {
            LOG_ERROR("Failed to register camera.\n");
            return IMG_ERROR_FATAL;
        }
        // do DGCam_CameraAddPool after first camera registration only
        if (DG_CameraAddPool(psCam->pDGCamera,
                             psCam->ui32BuffersCount) != IMG_SUCCESS)
        {
            LOG_ERROR("Failed to add pool of buffers.\n");
            return IMG_ERROR_FATAL;
        }
    }
    LOG_DEBUG("Starting camera...\n");
    if (DG_CameraStart(psCam->pDGCamera, psCam->pConnection) != IMG_SUCCESS)
    {
        LOG_ERROR("Could not start DG for context %d.\n",
                        psCam->pDGCamera->ui8DGContext);
        return IMG_ERROR_FATAL;
    }
    psCam->nEnabled = DG_SENSOR_ENABLED;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_Disable(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    LOG_INFO("Disabling DG sensor.\n");

    if (DG_CameraStop(psCam->pDGCamera) != IMG_SUCCESS)
    {
        LOG_ERROR("Could not stop DG for context %d.\n",
                        psCam->pDGCamera->ui8DGContext);
        return IMG_ERROR_FATAL;
    }
    psCam->ui32FrameNoToLoad = 0;

    psCam->nEnabled = DG_SENSOR_DISABLED;
    return IMG_SUCCESS;
}

static IMG_RESULT loadImageFromFile(DGCAM_STRUCT *psCam)
{
    if (!psCam->pszFilename)
    {
        LOG_ERROR("Image file name has not been provided.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!psCam->psImage)
    {
        psCam->psImage = (sSimImageIn *)calloc(1, sizeof(*psCam->psImage));

        if (!psCam->psImage)
        {
            LOG_ERROR("Error allocating sSimImageIn.\n");
            return IMG_ERROR_MALLOC_FAILED;
        }
    }

    // Clean up old image buffer if used. SimImageIn_init() function will be
    // called internally.
    if (SimImageIn_clean(psCam->psImage) != IMG_SUCCESS)
    {
        LOG_ERROR("Failed to initialise image.\n");
        free(psCam->psImage);
        psCam->psImage = NULL;
        return IMG_ERROR_FATAL;
    }

    if ( psCam->ui32FrameNoToLoad >= psCam->ui32FrameCount )
    {
        LOG_ERROR("Frame to load bigger than number of available frames in input file\n");
        return IMG_ERROR_FATAL;
    }

    LOG_INFO("Loading frame %d from %s...\n", psCam->ui32FrameNoToLoad,
                   psCam->pszFilename);

    // Load image from file...
    if (SimImageIn_loadFLX(psCam->pszFilename, psCam->psImage,
                           psCam->ui32FrameNoToLoad) != IMG_SUCCESS)
    {
        LOG_ERROR("Failed to load frame %d from %s.\n",
                        psCam->ui32FrameNoToLoad, psCam->pszFilename);
        free(psCam->psImage);
        psCam->psImage = NULL;
        return IMG_ERROR_FATAL;
    }

    if ( psCam->bFirstFrame )
    {
        // if it was the first frame we check how many frames can be loaded
        psCam->ui32FrameCount = psCam->psImage->nFrames;

        if ( psCam->psImage->nFrames == 1 )
        {
            if ( psCam->bIncrementFrameNo )
            {
                LOG_WARNING("Force IsVideo to FALSE because input FLX only has 1 frame\n");
            }
            psCam->bIncrementFrameNo = IMG_FALSE;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_Insert(SENSOR_HANDLE hHandle)
{
    IMG_RESULT result = IMG_SUCCESS;
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    LOG_INFO("Performing DG shot...\n");

    if (!psCam->nEnabled)
    {
        LOG_ERROR("Insert failed - sensor disabled.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!psCam->pConnection)
    {
        LOG_ERROR("Connection handle not set. Set CI_Connection first.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    // The first frame should already be loaded - skip loading...
    if (!psCam->bFirstFrame && psCam->bIncrementFrameNo)
    {
        LOG_DEBUG("Loading image from file...\n");
        if (loadImageFromFile(psCam) != IMG_SUCCESS)
        {
            LOG_ERROR("Failed to load image frame %d from file %s.\n",
                            psCam->ui32FrameNoToLoad, psCam->pszFilename);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
        psCam->ui32FrameNoToLoad = (psCam->ui32FrameNoToLoad+1)%psCam->ui32FrameCount;
    }
    else
    {
        psCam->bFirstFrame = IMG_FALSE;
    }

    LOG_DEBUG("Shooting...\n");
    if (DG_CameraShoot(psCam->pDGCamera, psCam->psImage, NULL) != IMG_SUCCESS)
    {
        LOG_ERROR("DG failed to shoot.\n");
        result = IMG_ERROR_FATAL;
    }

    return result;
}

IMG_RESULT DGCam_Destroy(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    LOG_INFO("Destroying DG sensor...\n");

    if (!psCam)
    {
        LOG_ERROR("Failed to destroy not created sensor!");
        return IMG_ERROR_FATAL;
    }

    // remember to free anything that might have been allocated dynamically
    // (like extended params...)
    if (psCam->pszFilename)
    {
        IMG_FREE(psCam->pszFilename);
        psCam->pszFilename = NULL;
    }

    if (psCam->psImage)
    {
        SimImageIn_clean(psCam->psImage);
        free(psCam->psImage);
        psCam->psImage = NULL;
    }

    if (DG_CameraDestroy(psCam->pDGCamera) != IMG_SUCCESS)
    {
        LOG_ERROR("DGCam_CameraDestroy failed.\n");
    }

    g_nDGCamera--;

    if (g_nDGCamera==0 && DG_DriverFinalise() != IMG_SUCCESS)
    {
        LOG_ERROR("DGCam_DriverFinalise failed.\n");
    }

    free(psCam);
    psCam = NULL;

    return IMG_SUCCESS;
}

// HW applies blanking = register + DGCam_H_BLANKING_SUB so actual minimum blanking is slightly different
IMG_STATIC_ASSERT(DG_H_BLANKING_SUB < FELIX_MIN_H_BLANKING, min_blanking_changed);

IMG_RESULT DGCam_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psInfo)
    {
        LOG_ERROR("psInfo is NULL.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!psCam->psImage)
    {
        LOG_ERROR("FLX image not loaded. Provide source file extended parameter first.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    switch (psCam->psImage->info.eColourModel)
    {
    case SimImage_RGGB:
        psInfo->eBayerOriginal = MOSAIC_RGGB;
        LOG_DEBUG("Bayer format = MOSAIC_RGGB\n");
        break;
    case SimImage_GRBG:
        psInfo->eBayerOriginal = MOSAIC_GRBG;
        LOG_DEBUG("Bayer format = MOSAIC_GRBG\n");
        break;
    case SimImage_GBRG:
        psInfo->eBayerOriginal = MOSAIC_GBRG;
        LOG_DEBUG("Bayer format = MOSAIC_GBRG\n");
        break;
    case SimImage_BGGR:
        psInfo->eBayerOriginal = MOSAIC_BGGR;
        LOG_DEBUG("Bayer format = MOSAIC_BGGR\n");
        break;
    default:
        psInfo->eBayerOriginal = MOSAIC_NONE;
        LOG_ERROR("Bayer format not recognized. Provide "
                        "EXTENDED_DGCam_SOURCE_FILE extended parameter first.\n");
    }
    // flipping not supported
    psInfo->eBayerEnabled = psInfo->eBayerOriginal;
    
    IMG_ASSERT(strlen(EXTDG_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);
    sprintf(psInfo->pszSensorName, EXTDG_SENSOR_INFO_NAME);
    sprintf(psInfo->pszSensorVersion, "HW %d.%d.%d", 
        psCam->pConnection->sHWInfo.rev_ui8Major, 
        psCam->pConnection->sHWInfo.rev_ui8Minor, 
        psCam->pConnection->sHWInfo.rev_ui8Maint);
    psInfo->fNumber = DG_SENSOR_INFO_NUMBER;
    psInfo->ui16FocalLength = DG_SENSOR_INFO_FOCAL_LENGTH;
    psInfo->ui32WellDepth = DG_SENSOR_INFO_WELL_DEPTH;
    // bitdepth is part of the mode
    psInfo->flReadNoise = DG_SENSOR_INFO_READ_NOISE;
    psInfo->ui8Imager = psCam->ui8DGContext;

    // other information should have been loaded by Sensor_GetInfo()
    return IMG_SUCCESS;
}

//
// These functions return dummy data but it could be read from input FLX
//
IMG_RESULT DGCam_GetCurrentFocus(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Current)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (!pui16Current)
    {
        LOG_ERROR("Invalid parameters.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *pui16Current = DG_SENSOR_FOCUS;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetFocusRange(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Min,
                            IMG_UINT16 *pui16Max)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (!pui16Min || !pui16Max)
    {
        LOG_ERROR("Invalid parameters.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *pui16Min = DG_SENSOR_FOCUS_MIN;
    *pui16Max = DG_SENSOR_FOCUS_MAX;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    
    //psCam->ui16FocusCurrent = ui16Focus;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetCurrentGain(SENSOR_HANDLE hHandle, double *pflCurrent,
                             IMG_UINT8 ui8Context)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (!pflCurrent)
    {
        LOG_ERROR("Invalid parameters.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    // Return the same value for all contexts.
    *pflCurrent = DG_SENSOR_GAIN;//psCam->flGainCurrent;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
                           double *pflMax, IMG_UINT8 *puiContexts)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (!pflMin || !pflMax)
    {
        LOG_ERROR("Invalid parameters.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *pflMin = DG_SENSOR_GAIN_MIN;//psCam->flGainMin;
    *pflMax = DG_SENSOR_GAIN_MAX;//psCam->flGainMax;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetGain(SENSOR_HANDLE hHandle, double flGain,
                      IMG_UINT8 ui8Context)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    // Ignore context and set new value of gain.
    //psCam->flGainCurrent = flGain;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure,
                          IMG_UINT8 ui8Context)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    // Ignore context and set new value of exposure.
    //psCam->ui32ExposureCurrent = ui32Exposure;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
                               IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    SENSOR_MODE sMode;
    IMG_RESULT ret;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = DGCam_GetMode(hHandle, DG_SENSOR_MODE_ID, &sMode);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("could not get sensor mode\n");
        return ret;
    }

    *pui32Min = (IMG_UINT32)( (1.0f/sMode.flFrameRate)*1000000 );
    *pui32Max = *pui32Min;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Exposure,
                          IMG_UINT8 ui8Context)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (!pui32Exposure)
    {
        LOG_ERROR("Invalid parameters.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    // Return the same value for all contexts.
    *pui32Exposure = DG_SENSOR_EXPOSURE;//psCam->ui32ExposureCurrent;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_Create(SENSOR_HANDLE *phHandle)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)IMG_CALLOC(1, sizeof(DGCAM_STRUCT));

    if (!psCam)
        return IMG_ERROR_MALLOC_FAILED;

    psCam->sFuncs.GetMode              = DGCam_GetMode;
    psCam->sFuncs.GetState              = DGCam_GetState;
    psCam->sFuncs.SetMode               = DGCam_SetMode;
    psCam->sFuncs.Enable                = DGCam_Enable;
    psCam->sFuncs.Disable               = DGCam_Disable;
    psCam->sFuncs.Destroy               = DGCam_Destroy;
    psCam->sFuncs.GetInfo               = DGCam_GetInfo;

    psCam->sFuncs.GetCurrentGain        = DGCam_GetCurrentGain;
    psCam->sFuncs.GetGainRange          = DGCam_GetGainRange;
    psCam->sFuncs.SetGain               = DGCam_SetGain;
    psCam->sFuncs.SetExposure           = DGCam_SetExposure;
    psCam->sFuncs.GetExposureRange      = DGCam_GetExposureRange;
    psCam->sFuncs.GetExposure           = DGCam_GetExposure;

    /*psCam->sFuncs.GetCurrentFocus       = DGCam_GetCurrentFocus;
    psCam->sFuncs.GetFocusRange         = DGCam_GetFocusRange;
    psCam->sFuncs.SetFocus              = DGCam_SetFocus;*/

    psCam->sFuncs.Insert                = DGCam_Insert;

    // Set default values.
    psCam->nEnabled = DG_SENSOR_DISABLED;
    psCam->ui16CurrentModeIndex = DG_SENSOR_MODE_ID;
    psCam->pszFilename = NULL;

    psCam->pConnection = NULL;
    psCam->buffersFormat = DG_MEMFMT_MIPI_RAW10; // some default -- will be computed when camera is started
    psCam->bMIPI_LF = IMG_FALSE;
    psCam->ui16HBlanking = FELIX_MIN_H_BLANKING;
    psCam->ui16VBlanking = FELIX_MIN_V_BLANKING;
    psCam->ui8DGContext = 0;
    psCam->ui32BuffersCount = 1;
    psCam->ui32FrameNoToLoad = 0;

    psCam->ui32FrameCount = 1;
    psCam->bIncrementFrameNo = IMG_TRUE;

    // Initialise DG driver...
    if (g_nDGCamera==0 && DG_DriverInit() != IMG_SUCCESS)
    {
        LOG_ERROR("DGCam_DriverInit() failed.\n");
        return IMG_ERROR_FATAL;
    }
    
    if (DG_CameraCreate(&psCam->pDGCamera) != IMG_SUCCESS)
    {
        LOG_ERROR("DGCam_CameraCreate() failed.\n");

        if (g_nDGCamera==0 && DG_DriverFinalise() != IMG_SUCCESS)
        {
            LOG_ERROR("DGCam_DriverFinalise() failed.\n");
        }
        return IMG_ERROR_FATAL;
    }

    *phHandle = &psCam->sFuncs;
    g_nDGCamera++;

    return IMG_SUCCESS;
}

//
// extended functions
//

IMG_RESULT DGCam_ExtendedSetConnection(SENSOR_HANDLE hHandle, CI_CONNECTION* conn)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(!psCam->pConnection)
    {
        psCam->pConnection = conn;
        return IMG_SUCCESS;
    }
    return IMG_ERROR_ALREADY_INITIALISED;
}

IMG_RESULT DGCam_ExtendedSetSourceFile(SENSOR_HANDLE hHandle, const char *filename)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = DGCam_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        if(psCam->pszFilename)
        {
            IMG_FREE(psCam->pszFilename);
        }
        psCam->pszFilename = IMG_STRDUP(filename);

        psCam->bFirstFrame = IMG_TRUE;
        // Switch to auto incrementing mode
        psCam->bIncrementFrameNo = IMG_TRUE;
        if (loadImageFromFile(psCam) != IMG_SUCCESS)
        {
            LOG_ERROR("Failed to load image frame %d from file %s.\n",
                            psCam->ui32FrameNoToLoad, psCam->pszFilename);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
        LOG_DEBUG("Frame %d loaded.\n", psCam->ui32FrameNoToLoad);
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter source file\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT DGCam_ExtendedGetSourceFile(SENSOR_HANDLE hHandle, const char **filename)   
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(!filename)
    {
        LOG_ERROR("filename is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *filename = psCam->pszFilename;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedSetFrameCap(SENSOR_HANDLE hHandle, IMG_UINT32 ui32FrameCap)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if(psCam->psImage->nFrames<ui32FrameCap)
    {
        LOG_ERROR("currently loaded image is %d frames - proposed cap of %d is not possible\n",
            psCam->psImage->nFrames, ui32FrameCap);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    psCam->ui32FrameCount = ui32FrameCap;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedGetFrameCap(SENSOR_HANDLE hHandle, IMG_UINT32 *pFrameCap)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if(!pFrameCap)
    {
        LOG_ERROR("pFrameCap is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *pFrameCap = psCam->ui32FrameCount;
    return IMG_SUCCESS;
}

IMG_UINT32 DGCam_ExtendedGetFrameNo(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    return psCam->ui32FrameNoToLoad;
}

IMG_BOOL8 DGCam_ExtendedGetUseMIPI(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    IMG_BOOL bUseMIPI = IMG_FALSE;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (IMG_SUCCESS != DGCam_useMIPI(psCam, &bUseMIPI))
    {
        return IMG_FALSE;
    }

    return bUseMIPI;
}

IMG_RESULT DGCam_ExtendedSetUseMIPILF(SENSOR_HANDLE hHandle, IMG_BOOL8 bUseMIPILF)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = DGCam_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->bMIPI_LF = bUseMIPILF;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter use MIPILF\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT DGCam_ExtendedGetUseMIPILF(SENSOR_HANDLE hHandle, IMG_BOOL8 *pUseMIPILF)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    // also copes with !hHandle
    if(DGCam_ExtendedGetUseMIPI(hHandle))
    {
        *pUseMIPILF = psCam->bMIPI_LF;
    }
    else
    {
        *pUseMIPILF = IMG_FALSE;
    }
    return IMG_SUCCESS;
}

IMG_UINT32 DGCam_ExtendedGetFrameCount(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    return psCam->ui32FrameCount;
}

IMG_RESULT DGCam_ExtendedSetIsVideo(SENSOR_HANDLE hHandle, IMG_BOOL8 bIsVideo)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam->bIncrementFrameNo = bIsVideo;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedGetIsVideo(SENSOR_HANDLE hHandle, IMG_BOOL8 *pIsVideo)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if(NULL==pIsVideo)
    {
        LOG_ERROR("pIsVideo is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *pIsVideo = psCam->bIncrementFrameNo;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedSetNbBuffers(SENSOR_HANDLE hHandle, IMG_UINT32 ui32NBuffers)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = DGCam_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->ui32BuffersCount = ui32NBuffers;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter NBuffers\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT DGCam_ExtendedGetNbBuffers(SENSOR_HANDLE hHandle, IMG_UINT32 *pNBuffers)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if(NULL==pNBuffers)
    {
        LOG_ERROR("pNBuffers is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *pNBuffers = psCam->ui32BuffersCount;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedSetBlanking(SENSOR_HANDLE hHandle, IMG_UINT32 ui32HBlanking, IMG_UINT32 ui32VBlanking)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = DGCam_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->ui16HBlanking = (IMG_UINT16)ui32HBlanking;
        psCam->ui16VBlanking = (IMG_UINT16)ui32VBlanking;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter blanking\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT DGCam_ExtendedGetBlanking(SENSOR_HANDLE hHandle, IMG_UINT32 *pHBlanking, IMG_UINT32 *pVBlanking)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if(!pHBlanking||!pVBlanking)
    {
        LOG_ERROR("pHBlanking or pVBlanking is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    *pHBlanking = psCam->ui16HBlanking;
    *pVBlanking = psCam->ui16VBlanking;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedSetGasket(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Gasket)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    ret = DGCam_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->ui8DGContext = ui8Gasket;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change gasket\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT DGCam_ExtendedGetGasket(SENSOR_HANDLE hHandle, IMG_UINT8 *pGasket)
{
    DGCAM_STRUCT *psCam = (DGCAM_STRUCT *)hHandle;

    if (!psCam)
    {
        LOG_ERROR("psCam is NULL (DG sensor not created).\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if(!pGasket)
    {
        LOG_ERROR("pGasket is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    *pGasket = psCam->ui8DGContext;
    return IMG_SUCCESS;
}
