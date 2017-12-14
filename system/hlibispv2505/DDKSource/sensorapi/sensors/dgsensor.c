/**
*******************************************************************************
@file dgsensor.c

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
#include <ci/ci_converter.h>
#include <dg/dg_api.h>
#include <sim_image.h>

// #define DG_SENSOR_DEBUG_ENABLED

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

#ifndef FELIX_FAKE
/* do a memcpy of frames before using datagen because fpga memory access
 * if platform is not memory write limited do not define it */
#define MEMCPY_FRAME
#endif

//#define DG_SENSOR_DEBUG_ENABLED

typedef struct _dgcam_struct
{
    SENSOR_FUNCS sFuncs;

    // local stuff goes here.
    IMG_UINT16 ui16CurrentModeIndex;
    IMG_INT16 i16CurrentFrameIndex;

    // used to get gasket info and acquire gasket
    CI_CONNECTION *pConnection;
    CI_CONVERTER sFrameConverter;

    // connection to DG driver - created with object
    DG_CONNECTION *pDGConnection;
    // camera object - created when starting
    DG_CAMERA *pDGCamera;

    char *pszFilename;
    sSimImageIn sImage;

    IMG_UINT32 ui32FrameNoToLoad;

    // Every shoot frame number will be incremented
    IMG_BOOL8 bIncrementFrameNo;
    /* How many frames FLX file contains is loaded from FLX file but can be
     * limited to frame count - will be overwritten if bigger than the number
     * loaded from the FLX */
    IMG_UINT32 ui32FrameCount;

    // number of buffers to allocate
    IMG_UINT32 ui32BuffersCount;

    // when using MIPI choice of using short or long format
    IMG_BOOL8 bMIPI_LF;
    IMG_BOOL8 bPreload;
    IMG_UINT8 ui8MipiLanes;

    // data generator to use
    IMG_UINT8 ui8Gasket;

    // Number of horizontal blanking columns + 40.
    IMG_UINT16 ui16HBlanking;
    // Number of vertical blanking lines.
    IMG_UINT16 ui16VBlanking;
} DGCAM_STRUCT;

//static unsigned int g_nDGCamera = 0;

static IMG_RESULT DGCam_useMIPI(DGCAM_STRUCT *psCam, IMG_BOOL *pbUseMIPI)
{
    IMG_ASSERT(psCam);
    if (psCam->pConnection && psCam->ui8Gasket < CI_N_IMAGERS)
    {
        *pbUseMIPI = ((psCam->pConnection->sHWInfo.\
            gasket_aType[psCam->ui8Gasket] & CI_GASKET_MIPI) != 0);
        return IMG_SUCCESS;
    }
    LOG_ERROR("Need to set the CONNECTION before checking for MIPI usage\n");
    return IMG_ERROR_INVALID_PARAMETERS;
}

IMG_RESULT DGCam_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16ModeIndex,
    SENSOR_MODE *psModes)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    if (!psModes)
    {
        LOG_ERROR("psModes is NULL.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!psCam->sImage.pBuffer)
    {
        LOG_ERROR("Sensor mode not available. Provide source file "\
            "extended parameter first.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // Only one mode available.
    if (ui16ModeIndex == DG_SENSOR_MODE_ID)
    {
        psModes->ui8BitDepth = psCam->sImage.info.ui8BitDepth;
        psModes->ui16Width = psCam->sImage.info.ui32Width;
        psModes->ui16Height = psCam->sImage.info.ui32Height;
        psModes->flFrameRate = DG_PARAM_FRAME_RATE;
        psModes->ui16HorizontalTotal = psCam->sImage.info.ui32Width
            + psCam->ui16HBlanking;
        psModes->ui16VerticalTotal = psCam->sImage.info.ui32Height
            + psCam->ui16VBlanking;
        psModes->flPixelRate = psModes->ui16HorizontalTotal
            *psModes->ui16VerticalTotal*psModes->flFrameRate;
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
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    psStatus->eState = SENSOR_STATE_IDLE;
    if (psCam->pDGCamera)  // can be used to know if enabled
    {
        psStatus->eState = SENSOR_STATE_RUNNING;
    }
    psStatus->ui16CurrentMode = psCam->ui16CurrentModeIndex;
    psStatus->ui8Flipping = SENSOR_FLIP_NONE;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 ui16ModeIndex,
    IMG_UINT8 ui8Flipping)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    LOG_DEBUG("Setting sensor mode = %d\n", ui16ModeIndex);
    if (DG_SENSOR_MODE_ID == ui16ModeIndex && SENSOR_FLIP_NONE == ui8Flipping)
    {
        psCam->ui16CurrentModeIndex = ui16ModeIndex;
        return IMG_SUCCESS;
    }
    else
    {
        LOG_ERROR("Mode %d (flipH=%d flipV=%d) not supported.\n",
            ui16ModeIndex,
            (int)(ui8Flipping&SENSOR_FLIP_HORIZONTAL),
            (int)(ui8Flipping&SENSOR_FLIP_VERTICAL));
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
    }
}

#if defined(DG_SENSOR_DEBUG_ENABLED)
void printSetup(SENSOR_HANDLE hHandle)
{
    const char *bufferFormatName = NULL;
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    IMG_BOOL bUseMipi = IMG_FALSE;
    DGCam_useMIPI(psCam, &bUseMipi);

    LOG_DEBUG("Enabling DG sensor for the following parameters:\n");
    if (psCam->sImage.pBuffer)
    {
        LOG_DEBUG("- image width = %u\n", psCam->sImage.info.ui32Width);
        LOG_DEBUG("- image height = %u\n", psCam->sImage.info.ui32Height);
    }
    else
    {
        LOG_DEBUG("no image was loaded\n");
    }

    switch (psCam->pDGCamera->eFormat)
    {
    case CI_DGFMT_PARALLEL:
        bufferFormatName = "PARALLEL";
        break;
    case CI_DGFMT_MIPI:
        bufferFormatName = "MIPI";
        break;
    case CI_DGFMT_MIPI_LF:
        bufferFormatName = "MIPI_LF";
        break;
    default:
        bufferFormatName = "undefined";
    }

    LOG_DEBUG("- image buffer format = %s %db\n",
        bufferFormatName, psCam->pDGCamera->ui8FormatBitdepth);
    LOG_DEBUG("- DG uses gasket = %d\n",
        psCam->pDGCamera->ui8Gasket);
    LOG_DEBUG("- MIPI gasket = %s\n",
        bUseMipi ? "yes" : "no");
    LOG_DEBUG("- number of horizontal blanking columns = %d\n",
        psCam->ui16HBlanking);
    LOG_DEBUG("- number of vertical blanking lines = %d\n",
        psCam->ui16VBlanking);
    
}
#endif /* DG_SENSOR_DEBUG_ENABLED */

IMG_RESULT DGCam_Enable(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    IMG_BOOL bUseMipi;
    IMG_RESULT res;
    unsigned int i;
    IMG_UINT32 ui32AllocSize = 0;
    CI_GASKET sGasket;
    
    res = DGCam_useMIPI(psCam, &bUseMipi);
    if (res)
    {
        LOG_ERROR("cannot check if gasket is MIPI or Parallel\n");
        return IMG_ERROR_FATAL;
    }

    LOG_INFO("Enabling DG Camera!\n");

    if (psCam->pDGCamera)  // can be used to know if enabled
    {
        LOG_WARNING("DG camera already enabled.");
        return IMG_SUCCESS;
    }

    res = DG_CameraCreate(&(psCam->pDGCamera), psCam->pDGConnection);
    if (res)
    {
        LOG_ERROR("Failed to create the camera object\n");
        return IMG_ERROR_FATAL;
    }

    if (psCam->ui32BuffersCount == 0)
    {
        LOG_ERROR("Buffers count is 0. Provide NBuffer extended "\
            "parameter first.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (psCam->sImage.pBuffer == NULL ||
        psCam->sImage.info.ui32Width == 0 ||
        psCam->sImage.info.ui32Height == 0)
    {
        LOG_ERROR("Image size is unknown (%dx%d). Provide source file "\
            "extended parameter first.\n",
            psCam->sImage.pBuffer ?
            (IMG_INT32)psCam->sImage.info.ui32Width : -1,
            psCam->sImage.pBuffer ?
            (IMG_INT32)psCam->sImage.info.ui32Height : -1);
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

    if (bUseMipi)
    {
        psCam->pDGCamera->eFormat = CI_DGFMT_MIPI;
        if (psCam->bMIPI_LF)
        {
            psCam->pDGCamera->eFormat = CI_DGFMT_MIPI_LF;
        }
    }
    else
    {
        psCam->pDGCamera->eFormat = CI_DGFMT_PARALLEL;
    }

    if (psCam->sImage.info.ui8BitDepth <= 10)
    {
        psCam->pDGCamera->ui8FormatBitdepth = 10;
    }
    else if (psCam->sImage.info.ui8BitDepth <= 12)
    {
        psCam->pDGCamera->ui8FormatBitdepth = 12;
    }
    else if (psCam->sImage.info.ui8BitDepth <= 14 && bUseMipi)
    {
        psCam->pDGCamera->ui8FormatBitdepth = 14;
    }
    else
    {
        LOG_ERROR("Colour bitdepth %d not supported for gasket format.\n",
            psCam->sImage.info.ui8BitDepth);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    // Use values provided as extended parameters
    psCam->pDGCamera->ui8Gasket = psCam->ui8Gasket;
    psCam->pDGCamera->bPreload = psCam->bPreload;
    psCam->pDGCamera->ui8MipiLanes = psCam->ui8MipiLanes;

    if (psCam->sFrameConverter.privateData)
    {
        CI_ConverterClear(&(psCam->sFrameConverter));
    }

    res = CI_ConverterConfigure(&(psCam->sFrameConverter),
        psCam->pDGCamera->eFormat, psCam->pDGCamera->ui8FormatBitdepth);
    if (res)
    {
        LOG_ERROR("failed to setup the frame converter\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

#if defined(DG_SENSOR_DEBUG_ENABLED)
    printSetup(hHandle);
#endif

    ui32AllocSize = CI_ConverterFrameSize(&(psCam->sFrameConverter),
        psCam->sImage.info.ui32Width, psCam->sImage.info.ui32Height);

    for (i = 0; i < psCam->ui32BuffersCount; i++)
    {
        res = DG_CameraAllocateFrame(psCam->pDGCamera, ui32AllocSize, NULL);
        if (res)
        {
            LOG_ERROR("failed to allocate frame %d/%d!\n");
            return IMG_ERROR_FATAL;
        }
    }

    sGasket.uiGasket = psCam->pDGCamera->ui8Gasket;
    sGasket.bParallel = !bUseMipi;
    sGasket.ui8ParallelBitdepth = psCam->pDGCamera->ui8FormatBitdepth;
    if (!bUseMipi)
    {
        sGasket.bHSync = psCam->pDGCamera->bParallelActive[0];
        sGasket.bVSync = psCam->pDGCamera->bParallelActive[1];
        sGasket.uiWidth = psCam->sImage.info.ui32Width - 1;
        sGasket.uiHeight = psCam->sImage.info.ui32Height - 1;
    }

    res = CI_GasketAcquire(&sGasket, psCam->pConnection);
    if (res)
    {
        LOG_ERROR("failed to acquire gasket %d\n", (int)sGasket.uiGasket);
        return IMG_ERROR_FATAL;
    }

    LOG_DEBUG("Starting camera...\n");
    res = DG_CameraStart(psCam->pDGCamera);
    if (res)
    {
        LOG_ERROR("Could not start DG for context %d.\n",
            psCam->pDGCamera->ui8Gasket);
        return IMG_ERROR_FATAL;
    }

    // pDGCamera can be used to know if enabled

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_Disable(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    IMG_RESULT ret;
    DG_FRAME *pFrame;
    CI_GASKET sGasket;

    if (!psCam->pDGCamera)
    {
        LOG_ERROR("DGCamera object is not initialised! Was the connection "\
            "to the driver a success?\n");
        return IMG_ERROR_FATAL;
    }

    LOG_INFO("Disabling DG sensor.\n");

    ret = DG_CameraStop(psCam->pDGCamera);
    if (ret)
    {
        LOG_ERROR("Could not stop DG for gasket %d.\n",
            psCam->pDGCamera->ui8Gasket);
        return IMG_ERROR_FATAL;
    }
    psCam->ui32FrameNoToLoad = 0;

    // free all allocate frames
    pFrame = DG_CameraGetAvailableFrame(psCam->pDGCamera);
    while (pFrame)
    {
        ret = DG_CameraDestroyFrame(pFrame);
        if (ret)
        {
            LOG_WARNING("failed to destroy frame id=%d\n",
                pFrame->ui32FrameID);
        }

        pFrame = DG_CameraGetAvailableFrame(psCam->pDGCamera);
    }

    sGasket.uiGasket = psCam->pDGCamera->ui8Gasket;

    ret = CI_GasketRelease(&sGasket, psCam->pConnection);
    if (ret)
    {
        LOG_WARNING("failed to release the gasket!\n");
    }

    ret = DG_CameraDestroy(psCam->pDGCamera);
    if (ret)
    {
        LOG_WARNING("failed to destroy camera object!\n");
    }
    psCam->pDGCamera = NULL;  // can be used to know if enabled

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_Insert(SENSOR_HANDLE hHandle)
{
    IMG_RESULT result = IMG_SUCCESS;
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    IMG_RESULT ret;
    CI_DG_FRAME *pFrame = NULL;
#ifdef MEMCPY_FRAME
    CI_DG_FRAME *pOriginalFrame = NULL;
    CI_DG_FRAME sToConvFrame;
#endif

    LOG_INFO("Performing DG shot...\n");
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    if (!psCam->pDGCamera)  // can be used to know if enabled
    {
        LOG_ERROR("Insert failed - sensor disabled.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!psCam->pConnection)
    {
        LOG_ERROR("Connection handle not set. Set CI_Connection first.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFrame = DG_CameraGetAvailableFrame(psCam->pDGCamera);
    if (!pFrame)
    {
        LOG_ERROR("could not get an available frame\n");
        return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
    }
#ifdef MEMCPY_FRAME
    // LOG_PERF("memcpy init values\n");
    IMG_MEMCPY(&sToConvFrame, pFrame, sizeof(CI_DG_FRAME));
    pOriginalFrame = pFrame;
    pFrame = &sToConvFrame;
    pFrame->data = (void*)IMG_MALLOC(pFrame->ui32AllocSize);

    if (!pFrame->data)
    {
        LOG_ERROR("failed to allocate temporary memory\n");
        return IMG_ERROR_MALLOC_FAILED;
    }
#endif

    ret = SimImageIn_convertFrame(&(psCam->sImage), psCam->ui32FrameNoToLoad);
    if (ret)
    {
        LOG_ERROR("failed to load frame %d from '%s'\n",
            psCam->ui32FrameNoToLoad, psCam->pszFilename);
#ifdef MEMCPY_FRAME
        IMG_FREE(pFrame->data);
        pFrame = pOriginalFrame;
#endif
        return IMG_ERROR_FATAL;
    }

    ret = CI_ConverterConvertFrame(&(psCam->sFrameConverter),
        &(psCam->sImage), pFrame);
    if (ret)
    {
        LOG_ERROR("failed to convert frame %d\n", psCam->ui32FrameNoToLoad);
#ifdef MEMCPY_FRAME
        IMG_FREE(pFrame->data);
        pFrame = pOriginalFrame;
#endif
        return IMG_ERROR_FATAL;
    }

    pFrame->ui32HorizontalBlanking = psCam->ui16HBlanking;
    pFrame->ui32VerticalBlanking = psCam->ui16VBlanking;

    LOG_DEBUG("Shooting with blanking %dx%d...\n",
        pFrame->ui32HorizontalBlanking, pFrame->ui32VerticalBlanking);

#ifdef MEMCPY_FRAME
    // LOG_PERF("memcpy back\n");
    IMG_MEMCPY(pOriginalFrame->data, pFrame->data,
        pFrame->ui32AllocSize);
    IMG_FREE(pFrame->data);
    // copy actual address
    pFrame->data = pOriginalFrame->data;
    // memcpy any other configuration
    IMG_MEMCPY(pOriginalFrame, pFrame, sizeof(CI_DG_FRAME));
    pFrame = pOriginalFrame;
#endif

    ret = DG_CameraInsertFrame(pFrame);
    if (ret)
    {
        LOG_ERROR("failed to insert frame %d\n",
            psCam->ui32FrameNoToLoad);
    }

    if (psCam->bIncrementFrameNo)
    {
        psCam->ui32FrameNoToLoad =
            (psCam->ui32FrameNoToLoad + 1) % psCam->ui32FrameCount;
    }

    return result;
}

IMG_RESULT DGCam_WaitProcessed(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    if (!psCam->pDGCamera)  // can be used to know if enabled
    {
        LOG_ERROR("Insert failed - sensor disabled.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!psCam->pConnection)
    {
        LOG_ERROR("Connection handle not set. Set CI_Connection first.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = DG_CameraWaitProcessedFrame(psCam->pDGCamera, IMG_TRUE);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to wait for processed frame\n");
        ret = IMG_ERROR_FATAL;
    }
    return ret;
}

IMG_RESULT DGCam_Destroy(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    IMG_RESULT ret;

    LOG_INFO("Destroying DG sensor...\n");

    // remember to free anything that might have been allocated dynamically
    // (like extended params...)
    if (psCam->pszFilename)
    {
        IMG_FREE(psCam->pszFilename);
        psCam->pszFilename = NULL;
    }

    if (psCam->sImage.pBuffer)
    {
        SimImageIn_close(&(psCam->sImage));
    }

    if (psCam->pDGCamera)
    {
        // should have been destroyed by stopping but just to be sure
        ret = DG_CameraDestroy(psCam->pDGCamera);
        if (ret)
        {
            LOG_WARNING("DGCam_CameraDestroy failed.\n");
        }
    }

    ret = DG_DriverFinalise(psCam->pDGConnection);
    if (ret)
    {
        LOG_WARNING("failed to close connection to DG driver\n");
    }

    CI_ConverterClear(&(psCam->sFrameConverter));
    IMG_FREE(psCam);

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    if (!psCam->sImage.pBuffer)
    {
        LOG_ERROR("FLX image not loaded. Provide source file extended "\
            "parameter first.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    switch (psCam->sImage.info.eColourModel)
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
    psInfo->ui8Imager = psCam->ui8Gasket;

    // other information should have been loaded by Sensor_GetInfo()
    return IMG_SUCCESS;
}

//
// These functions return dummy data but it could be read from input FLX
//
IMG_RESULT DGCam_GetCurrentFocus(SENSOR_HANDLE hHandle,
    IMG_UINT16 *pui16Current)
{
    // DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    *pui16Current = DG_SENSOR_FOCUS;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetFocusRange(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Min,
    IMG_UINT16 *pui16Max)
{
    // DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    *pui16Min = DG_SENSOR_FOCUS_MIN;
    *pui16Max = DG_SENSOR_FOCUS_MAX;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus)
{
    // DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    //psCam->ui16FocusCurrent = ui16Focus;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetCurrentGain(SENSOR_HANDLE hHandle, double *pflCurrent,
    IMG_UINT8 ui8Context)
{
    // DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    // Return the same value for all contexts.
    *pflCurrent = DG_SENSOR_GAIN;//psCam->flGainCurrent;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *puiContexts)
{
    // DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    *pflMin = DG_SENSOR_GAIN_MIN;//psCam->flGainMin;
    *pflMax = DG_SENSOR_GAIN_MAX;//psCam->flGainMax;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
    // DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    // Ignore context and set new value of gain.
    // psCam->flGainCurrent = flGain;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure,
    IMG_UINT8 ui8Context)
{
    // DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    // Ignore context and set new value of exposure.
    //psCam->ui32ExposureCurrent = ui32Exposure;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
    IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    SENSOR_MODE sMode;
    IMG_RESULT ret;

    ret = DGCam_GetMode(hHandle, DG_SENSOR_MODE_ID, &sMode);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("could not get sensor mode\n");
        return ret;
    }

    *pui32Min = (IMG_UINT32)((1.0f / sMode.flFrameRate) * 1000000);
    *pui32Max = *pui32Min;

    return IMG_SUCCESS;
}

IMG_RESULT DGCam_GetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Exposure,
    IMG_UINT8 ui8Context)
{
    DGCAM_STRUCT *psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

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
    IMG_RESULT ret;

    if (!psCam)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    psCam->sFuncs.GetMode = DGCam_GetMode;
    psCam->sFuncs.GetState = DGCam_GetState;
    psCam->sFuncs.SetMode = DGCam_SetMode;
    psCam->sFuncs.Enable = DGCam_Enable;
    psCam->sFuncs.Disable = DGCam_Disable;
    psCam->sFuncs.Destroy = DGCam_Destroy;
    psCam->sFuncs.GetInfo = DGCam_GetInfo;

    psCam->sFuncs.GetCurrentGain = DGCam_GetCurrentGain;
    psCam->sFuncs.GetGainRange = DGCam_GetGainRange;
    psCam->sFuncs.SetGain = DGCam_SetGain;
    psCam->sFuncs.SetExposure = DGCam_SetExposure;
    psCam->sFuncs.GetExposureRange = DGCam_GetExposureRange;
    psCam->sFuncs.GetExposure = DGCam_GetExposure;

    /*psCam->sFuncs.GetCurrentFocus       = DGCam_GetCurrentFocus;
    psCam->sFuncs.GetFocusRange         = DGCam_GetFocusRange;
    psCam->sFuncs.SetFocus              = DGCam_SetFocus;*/

    psCam->sFuncs.Insert = DGCam_Insert;
    psCam->sFuncs.WaitProcessed = DGCam_WaitProcessed;

    // Set default values.
    psCam->ui16CurrentModeIndex = DG_SENSOR_MODE_ID;
    psCam->pszFilename = NULL;

    psCam->pConnection = NULL;
    psCam->ui16HBlanking = FELIX_MIN_H_BLANKING;
    psCam->ui16VBlanking = FELIX_MIN_V_BLANKING;
    psCam->ui32BuffersCount = 1;
    psCam->ui32FrameNoToLoad = 0;
    psCam->ui32FrameCount = 1;

    psCam->bIncrementFrameNo = IMG_TRUE;

    // Initialise DG driver...
    ret = DG_DriverInit(&(psCam->pDGConnection));
    if (ret)
    {
        LOG_ERROR("DGCam_DriverInit() failed.\n");
        return IMG_ERROR_FATAL;
    }
    
    *phHandle = &psCam->sFuncs;

    return IMG_SUCCESS;
}

//
// extended functions
//

IMG_RESULT DGCam_ExtendedSetConnection(SENSOR_HANDLE hHandle,
    CI_CONNECTION* conn)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    if (!psCam->pConnection)
    {
        psCam->pConnection = conn;
        return IMG_SUCCESS;
    }
    return IMG_ERROR_ALREADY_INITIALISED;
}

IMG_RESULT DGCam_ExtendedSetSourceFile(SENSOR_HANDLE hHandle,
    const char *filename)
{
    DGCAM_STRUCT *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    ret = DGCam_GetState(hHandle, &sStatus);
    if (ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        if (psCam->pszFilename)
        {
            IMG_FREE(psCam->pszFilename);
        }
        psCam->pszFilename = IMG_STRDUP(filename);

        // Switch to auto incrementing mode
        psCam->bIncrementFrameNo = IMG_TRUE;

        // Clean up old image buffer if used
        ret = SimImageIn_close(&(psCam->sImage));
        if (ret)
        {
            LOG_ERROR("Failed to close image.\n");
            return IMG_ERROR_FATAL;
        }

        // Load image from file...
        ret = SimImageIn_loadFLX(&(psCam->sImage), psCam->pszFilename);
        if (ret)
        {
            LOG_ERROR("Failed to load data from '%s'",
                psCam->pszFilename);
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        if (1 == psCam->sImage.nFrames)
        {
            if (psCam->bIncrementFrameNo)
            {
                LOG_WARNING("Force IsVideo to FALSE because input FLX "\
                    "only has 1 frame\n");
            }
            psCam->bIncrementFrameNo = IMG_FALSE;
        }

        psCam->ui32FrameCount =
            IMG_MIN_INT(psCam->ui32FrameCount, psCam->sImage.nFrames);
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change "\
            "parameter source file\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT DGCam_ExtendedGetSourceFile(SENSOR_HANDLE hHandle,
    const char **filename)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!filename)
    {
        LOG_ERROR("filename is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    *filename = psCam->pszFilename;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedSetFrameCap(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32FrameCap)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    if (psCam->sImage.nFrames < ui32FrameCap)
    {
        LOG_ERROR("currently loaded image is %d frames - "\
            "proposed cap of %d is not possible\n",
            psCam->sImage.nFrames, ui32FrameCap);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    psCam->ui32FrameCount = ui32FrameCap;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedGetFrameCap(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pFrameCap)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pFrameCap)
    {
        LOG_ERROR("pFrameCap is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    *pFrameCap = psCam->ui32FrameCount;
    return IMG_SUCCESS;
}

IMG_RESULT DGCam_ExtendedGetFrameCount(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pFrameCount)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    if (!psCam->sImage.pBuffer)
    {
        LOG_ERROR("source faile was not selected!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (pFrameCount)
    {
        *pFrameCount = psCam->sImage.nFrames;
        return IMG_SUCCESS;
    }
    return IMG_ERROR_INVALID_PARAMETERS;
}

IMG_UINT32 DGCam_ExtendedGetFrameNo(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return (IMG_UINT32)-1;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    return psCam->ui32FrameNoToLoad;
}

IMG_BOOL8 DGCam_ExtendedGetUseMIPI(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = NULL;
    IMG_BOOL bUseMIPI = IMG_FALSE;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    ret = DGCam_useMIPI(psCam, &bUseMIPI);
    if (ret)
    {
        return IMG_FALSE;
    }

    return bUseMIPI;
}

IMG_RESULT DGCam_ExtendedSetUseMIPILF(SENSOR_HANDLE hHandle,
    IMG_BOOL8 bUseMIPILF)
{
    DGCAM_STRUCT *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    ret = DGCam_GetState(hHandle, &sStatus);
    if (ret)
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
        LOG_ERROR("sensor is in wrong state %d to change parameter "\
            "use MIPILF\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_BOOL8 DGCam_ExtendedGetUseMIPILF(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    if (DGCam_ExtendedGetUseMIPI(hHandle))
    {
        return psCam->bMIPI_LF;
    }
    return IMG_FALSE;
}

IMG_RESULT DGCam_ExtendedSetPreload(SENSOR_HANDLE hHandle,
    IMG_BOOL8 bPreload)
{
    DGCAM_STRUCT *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    ret = DGCam_GetState(hHandle, &sStatus);
    if (ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->bPreload = bPreload;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter "\
            "PRELOAD\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_BOOL8 DGCam_ExtendedGetPreload(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_FALSE;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    return psCam->bPreload;
}

IMG_RESULT DGCam_ExtendedSetMipiLanes(struct _Sensor_Functions_ *hHandle,
    IMG_UINT8 ui8MipiLanes)
{
    DGCAM_STRUCT *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    ret = DGCam_GetState(hHandle, &sStatus);
    if (ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->ui8MipiLanes = ui8MipiLanes;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter "\
            "MIPILANES\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_UINT8 DGCam_ExtendedGetMipiLanes(struct _Sensor_Functions_ *hHandle)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_FALSE;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    return psCam->ui8MipiLanes;
}

IMG_RESULT DGCam_ExtendedSetIsVideo(SENSOR_HANDLE hHandle, IMG_BOOL8 bIsVideo)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    psCam->bIncrementFrameNo = bIsVideo;
    return IMG_SUCCESS;
}

IMG_BOOL8 DGCam_ExtendedGetIsVideo(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_FALSE;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    return psCam->bIncrementFrameNo;
}

IMG_RESULT DGCam_ExtendedSetNbBuffers(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32NBuffers)
{
    DGCAM_STRUCT *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    ret = DGCam_GetState(hHandle, &sStatus);
    if (ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->ui32BuffersCount = ui32NBuffers;
        ret = IMG_SUCCESS;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter "\
            "NBuffers\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_UINT32 DGCam_ExtendedGetNbBuffers(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return 0;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    return psCam->ui32BuffersCount;
}

IMG_RESULT DGCam_ExtendedSetBlanking(SENSOR_HANDLE hHandle,
    IMG_UINT32 ui32HBlanking, IMG_UINT32 ui32VBlanking)
{
    DGCAM_STRUCT *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    ret = DGCam_GetState(hHandle, &sStatus);
    if (ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    psCam->ui16HBlanking = (IMG_UINT16)ui32HBlanking;
    psCam->ui16VBlanking = (IMG_UINT16)ui32VBlanking;

    return ret;
}

IMG_RESULT DGCam_ExtendedGetBlanking(SENSOR_HANDLE hHandle,
    IMG_UINT32 *pHBlanking, IMG_UINT32 *pVBlanking)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    if (!pHBlanking || !pVBlanking)
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
    DGCAM_STRUCT *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);

    ret = DGCam_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->ui8Gasket = ui8Gasket;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change gasket\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_UINT8 DGCam_ExtendedGetGasket(SENSOR_HANDLE hHandle)
{
    DGCAM_STRUCT *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("given handle is NULL\n");
        return CI_N_IMAGERS;
    }
    psCam = container_of(hHandle, DGCAM_STRUCT, sFuncs);
    return psCam->ui8Gasket;
}
