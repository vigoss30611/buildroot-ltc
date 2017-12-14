/**
******************************************************************************
@file iifdatagen.c

@brief Internal Data Generator camera implementation

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

#include <img_types.h>
#include <img_errors.h>

#include "sensors/iifdatagen.h"
#include <ci/ci_api.h>
#include <ci/ci_converter.h>
#include <sim_image.h>

#define LOG_TAG "IntDG_SENSOR"
#include <felixcommon/userlog.h>

// fake frame rate
#define IIFDG_SENSOR_FAKE_FRAMERATE (11.0f)

#ifndef FELIX_FAKE
/* do a memcpy of frames before using datagen because fpga memory access
 * if platform is not memory write limited do not define it */
#ifndef INFOTM_ISP
#define MEMCPY_FRAME
#endif

#endif

typedef struct _iifdgcam_struct
{
    SENSOR_FUNCS sFuncs;

    /**
     * @brief CI Connection to use to register datagen object
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetConnection()
     */
    CI_CONNECTION *pConnection;
    /**
     * Object created when enabling sensor.
     *
     * Object destroyed when disabling sensor.
     */
    CI_DATAGEN *pDatagen;
    /**
     * configured when enabling sensor
     */
    CI_CONVERTER sConverter;

    /**
     * Should always be 0.
     *
     * Could add other modes to have preload support
     */
    IMG_UINT8 ui8Mode;
    // comented because int dg does not change exposure
    //IMG_UINT32 ui32CurrentExposure;

    /**
     * @brief source of image data
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetSourceFile()
     */
    char *pszFilename;
    /**
     * image data loaded using pszFilename for every frame
     *
     * if bIsVideo is false only loads the 1st frame
     */
    sSimImageIn sImage;

    /**
     * @brief Maximum number of images to load from source-file
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetFrameCap()
     */
    IMG_UINT32 ui32FrameCount;
    /**
     * frame number to load next from pszFilename
     *
     * if bIsVideo will increment after every loading
     *
     * @note read-only through extended parameters @ref IIFDG_ExtendedGetFrameCap()
     */
    IMG_UINT32 ui32FrameNoToLoad;
    /**
     * number of buffers to allocate when enabling sensor
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetNbBuffers()
     */
    IMG_UINT32 ui32BuffersCount;
    /**
     * load more than 1st frame from file
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetIsVideo()
     */
    IMG_BOOL8 bIsVideo;
    /**
     * internal data generator to use
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetDatagen()
     */
    IMG_UINT8 ui8IIFDG;
    /**
     * Gasket to replace data from
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetGasket()
     */
    IMG_UINT8 ui8Gasket;

    /**
     * Number of horizontal blanking columns.
     * Can be used to change input pixel speed.
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetBlanking()
     */
    IMG_UINT16 ui16HBlanking;
    /**
     * Number of vertical blanking lines.
     *
     * @note given through extended parameters @ref IIFDG_ExtendedSetBlanking()
     */
    IMG_UINT16 ui16VBlanking;

    /**
     * Enable line preloading
     *
     * @note given through extended parameter @ref IIFDG_ExtendedSetPreload()
     */
    IMG_BOOL8 bPreload;

}IIF_DGCAM_SENSOR;

/*
* function implementation
*/

IMG_RESULT IIFDG_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
    SENSOR_MODE *psModes)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    if (0 != nIndex)
    {
        LOG_ERROR("only supports mode 0\n");
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
        // could support another mode for preload but not yet
    }

    if (NULL != psCam->pszFilename)
    {
        psModes->ui8BitDepth = psCam->sImage.info.ui8BitDepth;
        psModes->ui16Width = psCam->sImage.info.ui32Width;
        psModes->ui16Height = psCam->sImage.info.ui32Height;
        psModes->flFrameRate = IIFDG_SENSOR_FAKE_FRAMERATE; // fake value
        psModes->ui16HorizontalTotal = psCam->sImage.info.ui32Width
            + psCam->ui16HBlanking;
        psModes->ui16VerticalTotal = psCam->sImage.info.ui32Height
            + psCam->ui16VBlanking;
        psModes->flPixelRate = psModes->ui16HorizontalTotal
            *psModes->ui16VerticalTotal*psModes->flFrameRate;
        psModes->ui8SupportFlipping = SENSOR_FLIP_NONE;
        psModes->ui32ExposureMin = 0;
        psModes->ui32ExposureMax = 0;
        psModes->ui8MipiLanes = 0;
        return IMG_SUCCESS;
    }

    LOG_ERROR("must specify the source FLX file before getMode\n");
    return IMG_ERROR_UNEXPECTED_STATE;
}

IMG_RESULT IIFDG_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    IMG_ASSERT(psStatus);
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    psStatus->ui16CurrentMode = 0;
    psStatus->eState = SENSOR_STATE_UNINITIALISED;
    if (NULL != psCam->pDatagen && NULL != psCam->pszFilename)
    {
        psStatus->eState = SENSOR_STATE_IDLE;
        if (CI_DatagenIsStarted(psCam->pDatagen) == IMG_TRUE)
        {
            psStatus->eState = SENSOR_STATE_RUNNING;
        }
    }
    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
    IMG_UINT8 ui8Flipping)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    if (0 != nMode)
    {
        LOG_ERROR("supports only mode 0!\n");
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
    }
    if (SENSOR_FLIP_NONE != ui8Flipping)
    {
        LOG_ERROR("cannot enable flipping!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_Enable(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    if (NULL != psCam->pszFilename && NULL != psCam->pConnection)
    {
        IMG_UINT32 ui32FrameSize;
        IMG_UINT32 f;
        psCam->ui32FrameNoToLoad = 0;

        if (psCam->pDatagen)
        {
            ret = CI_DatagenDestroy(psCam->pDatagen);
            psCam->pDatagen = NULL;
        }
        ret = CI_DatagenCreate(&(psCam->pDatagen), psCam->pConnection);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to create datagen object! Can HW support it?\n");
            return ret;
        }

        psCam->pDatagen->bPreload = IMG_FALSE;
        psCam->pDatagen->ui8Gasket = psCam->ui8Gasket;
        psCam->pDatagen->ui8IIFDGIndex = psCam->ui8IIFDG;
        psCam->pDatagen->eFormat = CI_DGFMT_PARALLEL;
        psCam->pDatagen->bPreload = psCam->bPreload;

        if (10 >= psCam->sImage.info.ui8BitDepth)
        {
            psCam->pDatagen->ui8FormatBitdepth = 10;
            
        }
        else if (12 >= psCam->sImage.info.ui8BitDepth)
        {
            psCam->pDatagen->ui8FormatBitdepth = 12;
        }
        else
        {
            LOG_ERROR("input bit depth %d isn't supported (PARALLEL max is "\
                "12b)\n", psCam->sImage.info.ui8BitDepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        LOG_DEBUG("IntDG enable Parallel %db\n",
            (int)psCam->pDatagen->ui8FormatBitdepth);

        switch (psCam->sImage.info.eColourModel)
        {
        case SimImage_RGGB:
            psCam->pDatagen->eBayerMosaic = MOSAIC_RGGB;
            break;
        case SimImage_GRBG:
            psCam->pDatagen->eBayerMosaic = MOSAIC_GRBG;
            break;
        case SimImage_GBRG:
            psCam->pDatagen->eBayerMosaic = MOSAIC_GBRG;
            break;
        case SimImage_BGGR:
            psCam->pDatagen->eBayerMosaic = MOSAIC_BGGR;
            break;
        default:
            LOG_ERROR("un-expected input format!\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }

        ret = CI_ConverterConfigure(&(psCam->sConverter),
            psCam->pDatagen->eFormat, psCam->pDatagen->ui8FormatBitdepth);
        if (ret)
        {
            LOG_ERROR("failed to configure image converter from loaded image "\
                "format!\n");
            CI_DatagenDestroy(psCam->pDatagen);
            psCam->pDatagen = NULL;
            return ret;
        }

        ui32FrameSize = CI_ConverterFrameSize(&(psCam->sConverter),
            psCam->sImage.info.ui32Width, psCam->sImage.info.ui32Height);
        if (0 == ui32FrameSize)
        {
            LOG_ERROR("failed to get frame size from image's sizes\n");
            CI_DatagenDestroy(psCam->pDatagen);
            psCam->pDatagen = NULL;
            return IMG_ERROR_FATAL;
        }

        for (f = 0; f < psCam->ui32BuffersCount; f++)
        {
            ret = CI_DatagenAllocateFrame(psCam->pDatagen, ui32FrameSize,
                NULL);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate frame %d/%d\n", f + 1,
                    psCam->ui32BuffersCount);
                CI_DatagenDestroy(psCam->pDatagen);
                psCam->pDatagen = NULL;
                return ret;
            }
        }

        ret = CI_DatagenStart(psCam->pDatagen);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to start datagen!\n");
            CI_DatagenDestroy(psCam->pDatagen);
            psCam->pDatagen = NULL;
            return ret;
        }

        return IMG_SUCCESS;
    }

    LOG_ERROR("selecting a connection and an input file is needed before "\
        "enabling the sensor!\n");
    return IMG_ERROR_NOT_INITIALISED;
}

#ifdef INFOTM_ISP
IMG_RESULT IIFDG_Insert(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret = IMG_SUCCESS;

    int i, found;

    static int tail = 0;
    static void* frame_list[8] = {0};

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get sensor status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING == sStatus.eState)
    {
        CI_DG_FRAME *pFrame = NULL;
#ifdef MEMCPY_FRAME
        CI_DG_FRAME *pOriginalFrame = NULL;
        CI_DG_FRAME sToConvFrame;
#endif

        pFrame = CI_DatagenGetAvailableFrame(psCam->pDatagen);
        if (NULL == pFrame)
        {
            LOG_ERROR("failed to acquire an available frame\n");
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

        LOG_DEBUG("loading frame %d from '%s'\n",
            psCam->ui32FrameNoToLoad, psCam->pszFilename);

         found = 0;
         for (i =0; i < tail; i++)
         {
             if (frame_list[i] == pFrame->data)
             {
                 //printf("found %d\n", i);
                 found = 1;
                 break;
             }
         }

         if (found == 0)
         {
             printf("tail %d push %p\n",tail,  pFrame->data);
             frame_list[tail] = pFrame->data;
             tail ++;
         }

        if (psCam->bIsVideo)
            found = 0; /*alway convert frame*/

        if (!found)
        {
#if 0
            printf("data =%p allocSize = %d frameid=%d height=%d width=%d "
                        "mosaic=%d format=%d bitdepth=%d packetwidth=%d "
                        "vb=%d hb=%d stride=%d\n", pFrame->data,
                        pFrame->ui32AllocSize, pFrame->ui32FrameID,
                        pFrame->ui32Height, pFrame->ui32Width,
                        pFrame->eBayerMosaic, pFrame->eFormat, pFrame->ui8FormatBitdepth,
                        pFrame->ui32PacketWidth, pFrame->ui32VerticalBlanking,
                        pFrame->ui32HorizontalBlanking, pFrame->ui32Stride);
#endif
            ret = SimImageIn_convertFrame(&(psCam->sImage),
                psCam->ui32FrameNoToLoad);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to load frame %d from '%s'\n",
                    psCam->ui32FrameNoToLoad, psCam->pszFilename);
    #ifdef MEMCPY_FRAME
                IMG_FREE(pFrame->data);
                pFrame = pOriginalFrame;
    #endif
                CI_DatagenReleaseFrame(pFrame);
                return ret;
            }
            IMG_ASSERT(0 != psCam->sImage.nFrames);

            if (psCam->bIsVideo)
            {
                psCam->ui32FrameNoToLoad = (psCam->ui32FrameNoToLoad + 1)
                    % psCam->ui32FrameCount;
            }

            ret = CI_ConverterConvertFrame(&(psCam->sConverter), &(psCam->sImage),
                pFrame);

            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to convert frame!\n");
    #ifdef MEMCPY_FRAME
                IMG_FREE(pFrame->data);
                pFrame = pOriginalFrame;
    #endif
                CI_DatagenReleaseFrame(pFrame);
                return ret;
            }
        }

        pFrame->ui32HorizontalBlanking = psCam->ui16HBlanking;
        pFrame->ui32VerticalBlanking = psCam->ui16VBlanking;

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

        LOG_DEBUG("trigger frameID=%u\n", pFrame->ui32FrameID);
        ret = CI_DatagenInsertFrame(pFrame);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to trigger frame!\n");
#ifdef MEMCPY_FRAME
            IMG_FREE(pFrame->data);
            pFrame = pOriginalFrame;
#endif
            CI_DatagenReleaseFrame(pFrame);
            return ret;
        }

        return IMG_SUCCESS;
    }

    LOG_ERROR("cannot insert before enabling the sensor!\n");
    return IMG_ERROR_NOT_INITIALISED;
}
#else
IMG_RESULT IIFDG_Insert(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get sensor status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING == sStatus.eState)
    {
        CI_DG_FRAME *pFrame = NULL;
#ifdef MEMCPY_FRAME
        CI_DG_FRAME *pOriginalFrame = NULL;
        CI_DG_FRAME sToConvFrame;
#endif

        pFrame = CI_DatagenGetAvailableFrame(psCam->pDatagen);
        if (NULL == pFrame)
        {
            LOG_ERROR("failed to acquire an available frame\n");
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

        LOG_DEBUG("loading frame %d from '%s'\n",
            psCam->ui32FrameNoToLoad, psCam->pszFilename);

        ret = SimImageIn_convertFrame(&(psCam->sImage),
            psCam->ui32FrameNoToLoad);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to load frame %d from '%s'\n",
                psCam->ui32FrameNoToLoad, psCam->pszFilename);
#ifdef MEMCPY_FRAME
            IMG_FREE(pFrame->data);
            pFrame = pOriginalFrame;
#endif
            CI_DatagenReleaseFrame(pFrame);
            return ret;
        }
        IMG_ASSERT(0 != psCam->sImage.nFrames);

        if (psCam->bIsVideo)
        {
            psCam->ui32FrameNoToLoad = (psCam->ui32FrameNoToLoad + 1)
                % psCam->ui32FrameCount;
        }

        ret = CI_ConverterConvertFrame(&(psCam->sConverter), &(psCam->sImage),
            pFrame);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to convert frame!\n");
#ifdef MEMCPY_FRAME
            IMG_FREE(pFrame->data);
            pFrame = pOriginalFrame;
#endif
            CI_DatagenReleaseFrame(pFrame);
            return ret;
        }

        pFrame->ui32HorizontalBlanking = psCam->ui16HBlanking;
        pFrame->ui32VerticalBlanking = psCam->ui16VBlanking;

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

        LOG_DEBUG("trigger frameID=%u\n", pFrame->ui32FrameID);
        ret = CI_DatagenInsertFrame(pFrame);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to trigger frame!\n");
#ifdef MEMCPY_FRAME
            IMG_FREE(pFrame->data);
            pFrame = pOriginalFrame;
#endif
            CI_DatagenReleaseFrame(pFrame);
            return ret;
        }

        return IMG_SUCCESS;
    }

    LOG_ERROR("cannot insert before enabling the sensor!\n");
    return IMG_ERROR_NOT_INITIALISED;
}
#endif

IMG_RESULT IIFDG_WaitProcessed(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    IMG_ASSERT(hHandle); // checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get sensor status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING == sStatus.eState)
    {
        ret = CI_DatagenWaitProcessedFrame(psCam->pDatagen, IMG_TRUE);
    }
    else
    {
        LOG_ERROR("cannot wait for processed frame if not enabled!\n");
        ret = IMG_ERROR_NOT_INITIALISED;
    }

    return ret;
}

IMG_RESULT IIFDG_Disable(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get sensor status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING == sStatus.eState)
    {
        unsigned int f;
        ret = CI_DatagenStop(psCam->pDatagen);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to stop the data-generator!\n");
            return IMG_ERROR_FATAL;
        }

        /* remove all allocated buffers */
        for (f = 0; f < psCam->ui32BuffersCount; f++)
        {
            CI_DG_FRAME *pFrame = NULL;

            pFrame = CI_DatagenGetAvailableFrame(psCam->pDatagen);
            if (pFrame)
            {
                ret = CI_DatagenDestroyFrame(pFrame);
                if (IMG_SUCCESS != ret)
                {
                    LOG_WARNING("failed to destroy frame %d/%d\n",
                        f + 1, psCam->ui32BuffersCount);
                }
            }
            else
            {
                LOG_WARNING("failed to get frame %d/%d for destruction\n",
                    f + 1, psCam->ui32BuffersCount);
            }
        }

        ret = CI_DatagenDestroy(psCam->pDatagen);
        psCam->pDatagen = NULL;

        return IMG_SUCCESS;
    }

    LOG_ERROR("cannot disable before enabling the sensor!\n");
    return IMG_ERROR_NOT_INITIALISED;
}

IMG_RESULT IIFDG_Destroy(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get sensor status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING == sStatus.eState)
    {
        LOG_WARNING("destroying a running sensor! forces to stop\n");
        ret = IIFDG_Disable(hHandle);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("could not stop a sensor before destruction!\n");
        }
    }

    if (psCam->pszFilename)
    {
        IMG_FREE(psCam->pszFilename);
        psCam->pszFilename = NULL;
    }

    SimImageIn_close(&(psCam->sImage));

    // pDatagen is destroyed when disabling
    CI_ConverterClear(&(psCam->sConverter));
    IMG_FREE(psCam);

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin,
    double *pflMax, IMG_UINT8 *puiContexts)
{
    //IIF_DGCAM_SENSOR *psCam = NULL;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    IMG_ASSERT(pflMin);
    IMG_ASSERT(pflMax);
    IMG_ASSERT(puiContexts);
    //psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    // dummy values because datagen cannot change gains
    *pflMin = 1.0;
    *pflMax = 1.0;
    *puiContexts = 1;

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_GetCurrentGain(SENSOR_HANDLE hHandle, double *pflCurrent,
    IMG_UINT8 ui8Context)
{
    //IIF_DGCAM_SENSOR *psCam = NULL;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    IMG_ASSERT(pflCurrent);
    //psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    //if ( ui8Context == 0 )
    {
        // returns a dummy value because IntDG cannot change gains
        *pflCurrent = 1.0f;
        return IMG_SUCCESS;
    }

    //LOG_ERROR("gain context %d too big (max = %u)\n", ui8Context, 1);
    //return IMG_ERROR_INVALID_PARAMETER;
}

IMG_RESULT IIFDG_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context)
{
    //IIF_DGCAM_SENSOR *psCam = NULL;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    //psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    if (1.0f != flGain)
    {
        LOG_ERROR("cannot change gain\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // implementation should return dummy 1

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
    IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_MODE sMode;
    IMG_RESULT ret;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    IMG_ASSERT(pui32Min);
    IMG_ASSERT(pui32Max);
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetMode(hHandle, psCam->ui8Mode, &sMode);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("could not get sensor mode\n");
        return ret;
    }

    *pui32Min = (IMG_UINT32)((1.0f / sMode.flFrameRate) * 1000000);
    *pui32Max = *pui32Min;
    *pui8Contexts = 0;
    // could also try to load information from meta-data

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_GetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Exposure,
    IMG_UINT8 ui8Context)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_MODE sMode;
    IMG_RESULT ret;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    IMG_ASSERT(pui32Exposure);
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    if (0 != ui8Context)
    {
        LOG_ERROR("supports only sensor context 0\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = IIFDG_GetMode(hHandle, psCam->ui8Mode, &sMode);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("could not get sensor mode\n");
        return ret;
    }

    *pui32Exposure = (IMG_UINT32)((1.0f / sMode.flFrameRate) * 1000000);
    // could also try to load information from meta-data
    //*pui32Exposure = psCam->ui32CurrentExposure;

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure,
    IMG_UINT8 ui8Context)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    IMG_RESULT ret;
    IMG_UINT32 minExp, maxExp;
    IMG_UINT8 expCtx;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetExposureRange(hHandle, &minExp, &maxExp, &expCtx);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("could not get exposure ranges\n");
    }

    if (ui32Exposure > maxExp || ui32Exposure < minExp)
    {
        LOG_ERROR("given exposure %d is illegal (mode=%d min=%d max=%d)\n",
            ui32Exposure, psCam->ui8Mode, minExp, maxExp);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    //psCam->ui32CurrentExposure = ui32Exposure;

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    IMG_ASSERT(strlen(IIFDG_SENSOR_INFO_NAME) < SENSOR_INFO_NAME_MAX);

    sprintf(psInfo->pszSensorName, IIFDG_SENSOR_INFO_NAME);
    psInfo->fNumber = 0.0f; // could be loaded from parameters!
    psInfo->ui16FocalLength = 0; // could be loaded from parameters!
    psInfo->ui32WellDepth = 0; // could be loaded from parameters!
    // bitdepth is part of the modeF
    psInfo->flReadNoise = 0.0f; // could be loaded from parameters!
    psInfo->ui8Imager = psCam->ui8Gasket;
    psInfo->bBackFacing = IMG_TRUE;

    if (psCam->pConnection)
    {
        sprintf(psInfo->pszSensorVersion, "%d.%d.%d",
            psCam->pConnection->sHWInfo.rev_ui8Major,
            psCam->pConnection->sHWInfo.rev_ui8Minor,
            psCam->pConnection->sHWInfo.rev_ui8Maint);
    }
    else
    {
        LOG_WARNING("need the connection to have been set to get version\n");
        sprintf(psInfo->pszSensorVersion, "not-verified");
    }

    if (psCam->pszFilename)
    {
        switch (psCam->sImage.info.eColourModel)
        {
        case SimImage_RGGB:
            psInfo->eBayerOriginal = MOSAIC_RGGB;
            break;
        case SimImage_GRBG:
            psInfo->eBayerOriginal = MOSAIC_GRBG;
            break;
        case SimImage_GBRG:
            psInfo->eBayerOriginal = MOSAIC_GBRG;
            break;
        case SimImage_BGGR:
            psInfo->eBayerOriginal = MOSAIC_BGGR;
            break;
        default: // very unlikely
            LOG_ERROR("un-expected image format!\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }
        // flipping not supported
        psInfo->eBayerEnabled = psInfo->eBayerOriginal;

        // other value should be loaded from Sensor_GetInfo()
    }
    else
    {
        LOG_WARNING("need a filename to have been selected before getting "\
            "all dg sensor information!\n");
        psInfo->eBayerOriginal = MOSAIC_RGGB;
    }

    return ret;
}

/*
 * camera creation
 */

IMG_RESULT IIFDG_Create(SENSOR_HANDLE *phHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    psCam = (IIF_DGCAM_SENSOR*)IMG_CALLOC(1, sizeof(IIF_DGCAM_SENSOR));
    if (!psCam)
    {
        LOG_ERROR("Failed to allocate internal structure (%"IMG_SIZEPR"d bytes)\n",
            sizeof(IIF_DGCAM_SENSOR));
        return IMG_ERROR_MALLOC_FAILED;
    }

    psCam->sFuncs.GetMode = IIFDG_GetMode;
    psCam->sFuncs.GetState = IIFDG_GetState;
    psCam->sFuncs.SetMode = IIFDG_SetMode;
    psCam->sFuncs.Enable = IIFDG_Enable;
    psCam->sFuncs.Insert = IIFDG_Insert;
    psCam->sFuncs.WaitProcessed = IIFDG_WaitProcessed;
    psCam->sFuncs.Disable = IIFDG_Disable;
    psCam->sFuncs.Destroy = IIFDG_Destroy;

    psCam->sFuncs.GetFocusRange = NULL;
    psCam->sFuncs.GetCurrentFocus = NULL;
    psCam->sFuncs.SetFocus = NULL;

    psCam->sFuncs.GetGainRange = IIFDG_GetGainRange;
    psCam->sFuncs.GetCurrentGain = IIFDG_GetCurrentGain;
    psCam->sFuncs.SetGain = IIFDG_SetGain;

    psCam->sFuncs.GetExposureRange = IIFDG_GetExposureRange;
    psCam->sFuncs.GetExposure = IIFDG_GetExposure;
    psCam->sFuncs.SetExposure = IIFDG_SetExposure;

    psCam->sFuncs.GetInfo = IIFDG_GetInfo;

    psCam->sFuncs.ConfigureFlash = NULL;

    SimImageIn_init(&(psCam->sImage));
    psCam->ui32BuffersCount = 1;

    *phHandle = &(psCam->sFuncs);
    return IMG_SUCCESS;
}

/*
 * Extended parameters
 */

IMG_RESULT IIFDG_ExtendedSetConnection(SENSOR_HANDLE hHandle, CI_CONNECTION* conn)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!conn)
    {
        LOG_ERROR("NULL conn given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    if (!psCam->pConnection)
    {
        psCam->pConnection = conn;
        return IMG_SUCCESS;
    }
    return IMG_ERROR_ALREADY_INITIALISED;
}

IMG_RESULT IIFDG_ExtendedSetSourceFile(SENSOR_HANDLE hHandle, const char *filename)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
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

        SimImageIn_close(&(psCam->sImage));

        // load 1st frame just to get information
        ret = SimImageIn_loadFLX(&(psCam->sImage), psCam->pszFilename);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to load '%s'\n",
                psCam->pszFilename);
            IMG_FREE(psCam->pszFilename);
            psCam->pszFilename = NULL;
            ret = IMG_ERROR_INVALID_PARAMETERS;
        }

        /*ret = SimImageIn_convertFrame(&(psCam->sImage), 0);
        if (IMG_SUCCESS != ret)
        {
        LOG_ERROR("failed to load 1st frame from '%s'\n",
        psCam->pszFilename);
        IMG_FREE(psCam->pszFilename);
        psCam->pszFilename = NULL;
        ret = IMG_ERROR_INVALID_PARAMETERS;
        }*/

        psCam->ui32FrameCount = psCam->sImage.nFrames;
        LOG_INFO("change input file to '%s' containing %d frames\n",
            filename, psCam->ui32FrameCount);
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change parameter source file\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT IIFDG_ExtendedGetSourceFile(SENSOR_HANDLE hHandle, const char **filename)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!filename)
    {
        LOG_ERROR("NULL filename given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *filename = psCam->pszFilename;
    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedSetGasket(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Gasket)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
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

IMG_RESULT IIFDG_ExtendedGetGasket(SENSOR_HANDLE hHandle, IMG_UINT8 *pGasket)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pGasket)
    {
        LOG_ERROR("NULL pGasket given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *pGasket = psCam->ui8Gasket;
    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedSetDatagen(SENSOR_HANDLE hHandle, IMG_UINT8 ui8Context)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get status!\n");
        return ret;
    }

    if (SENSOR_STATE_RUNNING != sStatus.eState)
    {
        psCam->ui8IIFDG = ui8Context;
    }
    else
    {
        LOG_ERROR("sensor is in wrong state %d to change Datagen context\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT IIFDG_ExtendedGetDatagen(SENSOR_HANDLE hHandle, IMG_UINT8 *pContext)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pContext)
    {
        LOG_ERROR("NULL pContext given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *pContext = psCam->ui8IIFDG;
    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedSetFrameCap(struct _Sensor_Functions_ *hHandle, IMG_UINT32 ui32FrameCap)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    if (psCam->sImage.nFrames < ui32FrameCap)
    {
        LOG_ERROR("currently loaded image is %d frames - proposed cap of %d is not possible\n",
            psCam->sImage.nFrames, ui32FrameCap);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    psCam->ui32FrameCount = ui32FrameCap;
    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedGetFrameCap(SENSOR_HANDLE hHandle, IMG_UINT32 *pFrameCap)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pFrameCap)
    {
        LOG_ERROR("NULL pFrameCap given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *pFrameCap = psCam->ui32FrameCount;
    return IMG_SUCCESS;
}

IMG_UINT32 IIFDG_ExtendedGetFrameNO(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    return psCam->ui32FrameNoToLoad;
}

IMG_UINT32 IIFDG_ExtendedGetFrameCount(SENSOR_HANDLE hHandle)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    return psCam->sImage.nFrames;
}

IMG_RESULT IIFDG_ExtendedSetIsVideo(SENSOR_HANDLE hHandle, IMG_BOOL8 bIsVideo)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    psCam->bIsVideo = bIsVideo;

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedGetIsVideo(SENSOR_HANDLE hHandle, IMG_BOOL8 *pIsVideo)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pIsVideo)
    {
        LOG_ERROR("NULL pIsVideo given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_ASSERT(hHandle); //checked in sensorAPI
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *pIsVideo = psCam->bIsVideo;
    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedSetNbBuffers(SENSOR_HANDLE hHandle, IMG_UINT32 ui32NBuffers)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    SENSOR_STATUS sStatus;
    IMG_RESULT ret;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
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
        LOG_ERROR("sensor is in wrong state %d to change number of buffers\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT IIFDG_ExtendedGetNbBuffers(SENSOR_HANDLE hHandle, IMG_UINT32 *pNBuffers)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pNBuffers)
    {
        LOG_ERROR("NULL pNBuffers given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *pNBuffers = psCam->ui32BuffersCount;
    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedSetBlanking(SENSOR_HANDLE hHandle, IMG_UINT32 ui32HBlanking, IMG_UINT32 ui32VBlanking)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    psCam->ui16HBlanking = (IMG_UINT16)ui32HBlanking;
    psCam->ui16VBlanking = (IMG_UINT16)ui32VBlanking;

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedGetBlanking(SENSOR_HANDLE hHandle, IMG_UINT32 *pHBlanking, IMG_UINT32 *pVBlanking)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pHBlanking || !pVBlanking)
    {
        LOG_ERROR("NULL pHBlanking or pVBlanking given\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *pHBlanking = psCam->ui16HBlanking;
    *pVBlanking = psCam->ui16VBlanking;

    return IMG_SUCCESS;
}

IMG_RESULT IIFDG_ExtendedSetPreload(SENSOR_HANDLE hHandle, IMG_BOOL8 bPreload)
{
    IIF_DGCAM_SENSOR *psCam = NULL;
    IMG_RESULT ret;
    SENSOR_STATUS sStatus;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    ret = IIFDG_GetState(hHandle, &sStatus);
    if (IMG_SUCCESS != ret)
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
        LOG_ERROR("sensor is in wrong state %d to change the preload option\n",
            sStatus.eState);
        ret = IMG_ERROR_OPERATION_PROHIBITED;
    }

    return ret;
}

IMG_RESULT IIFDG_ExtendedGetPreload(SENSOR_HANDLE hHandle,
    IMG_BOOL8 *pbPreload)
{
    IIF_DGCAM_SENSOR *psCam = NULL;

    if (!hHandle)
    {
        LOG_ERROR("NULL handle given\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!pbPreload)
    {
        LOG_ERROR("pbPreload is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    psCam = container_of(hHandle, IIF_DGCAM_SENSOR, sFuncs);

    *pbPreload = psCam->bPreload;
    return IMG_SUCCESS;
}
