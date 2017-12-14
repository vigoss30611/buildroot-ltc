/**
*******************************************************************************
@file dg_api.c

@brief Data Generator user-side library

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

#include "dg/dg_api.h"
#include "dg/dg_api_internal.h"

#include "dg_kernel/dg_ioctl.h"
#include "ci_internal/ci_errors.h" // toImgErrors and toErrno
#include "ci/ci_api.h" // access to CI_CONNECTION and gasket control

#include <img_defs.h>
#include <img_errors.h>

#include <sim_image.h>

#include <registers/ext_data_generator.h> // for default register values

#define LOG_TAG DG_LOG_TAG
#include <felixcommon/userlog.h>

#define DG_DEV "/dev/imgfelixDG0"

#ifdef WIN32
#ifndef MAP_FAILED
#define MAP_FAILED ((void *)-1)
#endif
#endif

#ifdef FELIX_FAKE

#include "dg_kernel/dg_connection.h"
// fake device
#include "sys/sys_userio_fake.h"

static int DEV_DG_MUnmap(void *addr, size_t length)
{
    (void)addr;
    (void)length;
    return 0;
}
#define CAMERA_STOP_SLEEP 2000 // miliseconds
#else
#define CAMERA_STOP_SLEEP 20 // miliseconds
#endif // FELIX_FAKE

static void milisleep(int milisec);

static INT_DATAGEN *g_pConnection = NULL;

static void List_destroyFrame(void *listElem)
{
    INT_FRAME *pFrame = (INT_FRAME*)listElem;

    /// @ unmap buffer

    IMG_FREE(pFrame);
}

static void List_destroyCamera(void *listElem)
{
    INT_CAMERA *pCam = (INT_CAMERA*)listElem;

    DG_CameraDestroy(&(pCam->publicCamera));
}

static IMG_BOOL8 List_searchCam(void *listElem, void *searched)
{
    INT_CAMERA *pCurr = (INT_CAMERA*)listElem;
    int *camId = (int*)searched;
    if (pCurr->identifier == *camId)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_searchFrame(void *listElem, void *searched)
{
    INT_FRAME *pFrame = (INT_FRAME*)listElem;
    int *offset = (int*)searched;

    if (pFrame->identifier == (IMG_UINT)*offset)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_BOOL8 IMG_DG_UnmapFrame(void *elem, void *param)
{
    INT_FRAME *pFrame = (INT_FRAME*)elem;
    INT_DATAGEN *pDG = (INT_DATAGEN*)param;

    if (pFrame->pImage != NULL && pDG != NULL)
    {
        SYS_IO_MemUnmap(pDG->fileDesc, pFrame->pImage,
            pFrame->uiStride*pFrame->uiHeight);
        pFrame->pImage = NULL;
    }

    return IMG_TRUE; // continue
}

/*
 * Driver part
 */

IMG_RESULT DG_DriverInit(void)
{
    INT_DATAGEN *pConn = NULL;
    IMG_RESULT ret;
    void *fakeOps = NULL; // used in case of Fake device

    if (g_pConnection != NULL)
    {
        LOG_WARNING("connection already established\n");
        return IMG_SUCCESS;//return IMG_ERROR_ALREADY_INITIALISED;
    }

    pConn = (INT_DATAGEN*)IMG_CALLOC(1, sizeof(INT_DATAGEN));
    if (pConn == NULL)
    {
        LOG_ERROR("failed to allocate internal structure (%" IMG_SIZEPR \
            "u Bytes)\n", sizeof(INT_DATAGEN));
        return IMG_ERROR_MALLOC_FAILED;
    }

    if ((ret = List_init(&(pConn->sList_cameras))) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to initialise the internal list of cameras\n");
        IMG_FREE(pConn);
        return IMG_ERROR_MALLOC_FAILED;
    }

#ifdef FELIX_FAKE
    if (g_pDGDriver == NULL)
    {
        // list is empty, no need for cleaning
        IMG_FREE(pConn);
        LOG_ERROR("could not find the kernel-side driver: did you call " \
            "KRN_DG_DriverCreate()?\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    // does not exists in the kernel so add it here
    g_pDGDriver->sDevice.sFileOps.munmap = &DEV_DG_MUnmap;
    fakeOps = (void*)&(g_pDGDriver->sDevice.sFileOps);
#endif

    if ((pConn->fileDesc = SYS_IO_Open(DG_DEV, O_RDWR, fakeOps)) != NULL)
    {
        ret = SYS_IO_Control(pConn->fileDesc, DG_IOCTL_INFO,
            (long int)&(pConn->sHWinfo)); //DEV_DG_Ioctl()
        if (0 != ret)
        {
            LOG_ERROR("failed to receive informations from kernel-side\n");
            ret = IMG_ERROR_FATAL;
            goto open_failure;
        }
    }
    else
    {
        LOG_ERROR("failed to connect to the kernel-side driver\n");
        ret = IMG_ERROR_UNEXPECTED_STATE;
        goto open_failure;
    }

    g_pConnection = pConn;

    return ret;
open_failure:
    // camera list is empty
    if (pConn->fileDesc != NULL)
    {
        SYS_IO_Close(pConn->fileDesc);
    }
    IMG_FREE(pConn);
    pConn = NULL;
    return ret;
}

IMG_UINT8 DG_DriverNDatagen(void)
{
    if (g_pConnection != NULL)
    {
        return g_pConnection->sHWinfo.config_ui8NDatagen;
    }
    LOG_ERROR("not connected\n");
    return 0;
}

IMG_RESULT DG_DriverFinalise(void)
{
    IMG_RESULT ret;
    INT_DATAGEN *pConn = NULL;
    if (g_pConnection == NULL)
    {
        LOG_ERROR("not connected\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    pConn = g_pConnection;
    // so nobody can add more things and components know it's cleanup time
    g_pConnection = NULL;

    List_clearObjects(&(pConn->sList_cameras), &List_destroyCamera);

    if ((ret = SYS_IO_Close(pConn->fileDesc)) != 0)
    {
        LOG_ERROR("failed to close device (returned %d)\n", ret);
    }

    IMG_FREE(pConn);

    return IMG_SUCCESS;
}

IMG_RESULT DG_CameraCreate(DG_CAMERA **ppCamera)
{
    IMG_RESULT ret;
    INT_CAMERA *pPrvCam = NULL;
    if (ppCamera == NULL)
    {
        LOG_ERROR("ppCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (*ppCamera != NULL)
    {
        LOG_ERROR("*ppCamera is already allocated\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }
    if (g_pConnection == NULL)
    {
        LOG_ERROR("not connected\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    pPrvCam = (INT_CAMERA *)IMG_CALLOC(1, sizeof(INT_CAMERA));

    if (pPrvCam == NULL)
    {
        LOG_ERROR("failed to allocate INT_CAMERA (%" IMG_SIZEPR "u Bytes)\n",
            sizeof(INT_CAMERA));
        return IMG_ERROR_MALLOC_FAILED;
    }

    pPrvCam->publicCamera.pConverter = (DG_CONV*)IMG_CALLOC(1,
        sizeof(DG_CONV));
    if (pPrvCam->publicCamera.pConverter == NULL)
    {
        IMG_FREE(pPrvCam);
        LOG_ERROR("failed to allocate DG_CONV (%" IMG_SIZEPR "u Bytes)\n",
            sizeof(DG_CONV));
        return IMG_ERROR_MALLOC_FAILED;
    }


    if ((ret = List_init(&(pPrvCam->sList_frames))) != IMG_SUCCESS)
    {
        IMG_FREE(pPrvCam);
    }
    else
    {
        ret = List_pushBackObject(&(g_pConnection->sList_cameras), pPrvCam);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to push the camera in the list");
            // frame list is empty
            IMG_FREE(pPrvCam);
        }
    }

    if (ret == IMG_SUCCESS)
    {
        pPrvCam->pGasket = NULL;

        *ppCamera = &(pPrvCam->publicCamera);

        // some default values
        (*ppCamera)->ui16HoriBlanking = FELIX_MIN_H_BLANKING;
        (*ppCamera)->ui16VertBlanking = FELIX_MIN_V_BLANKING;

        // these are the defaults values for mipi
        (*ppCamera)->ui16MipiTLPX =
            FELIX_TEST_DG_DG_MIPI_START_OF_DATA_TRANSMISSION_DG_TLPX_DEFAULT;
        (*ppCamera)->ui16MipiTHS_prepare =
            FELIX_TEST_DG_DG_MIPI_START_OF_DATA_TRANSMISSION_DG_THS_PREPARE_DEFAULT;
        (*ppCamera)->ui16MipiTHS_zero =
            FELIX_TEST_DG_DG_MIPI_START_OF_DATA_TRANSMISSION_DG_THS_ZERO_DEFAULT;
        (*ppCamera)->ui16MipiTHS_trail =
            FELIX_TEST_DG_DG_MIPI_END_OF_DATA_TRANSMISSION_DG_THS_TRAIL_DEFAULT;
        (*ppCamera)->ui16MipiTHS_exit =
            FELIX_TEST_DG_DG_MIPI_END_OF_DATA_TRANSMISSION_DG_THS_EXIT_DEFAULT;
        (*ppCamera)->ui16MipiTCLK_prepare =
            FELIX_TEST_DG_DG_MIPI_START_OF_CLOCK_TRANSMISSION_DG_TCLK_PREPARE_DEFAULT;
        (*ppCamera)->ui16MipiTCLK_zero =
            FELIX_TEST_DG_DG_MIPI_START_OF_CLOCK_TRANSMISSION_DG_TCLK_ZERO_DEFAULT;
        (*ppCamera)->ui16MipiTCLK_post =
            FELIX_TEST_DG_DG_MIPI_END_OF_CLOCK_TRANSMISSION_DG_TCLK_POST_DEFAULT;
        (*ppCamera)->ui16MipiTCLK_trail =
            FELIX_TEST_DG_DG_MIPI_END_OF_CLOCK_TRANSMISSION_DG_TCLK_TRAIL_DEFAULT;
    }

    return ret;
}

IMG_RESULT DG_CameraDestroy(DG_CAMERA *pCamera)
{
    INT_CAMERA *pPrvCam = NULL;
    IMG_RESULT ret;
    if (pCamera == NULL)
    {
        LOG_ERROR("pCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, INT_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    // if NULL it is clean-up time - the connection is removing the cells
    if (g_pConnection != NULL)
    {
        sCell_T *pFound = NULL;

        List_visitor(&(pPrvCam->sList_frames), g_pConnection,
            &IMG_DG_UnmapFrame);

        if (pPrvCam->identifier > 0)
        {
            if (pPrvCam->bStarted == IMG_TRUE)
            {
                if ((ret = DG_CameraStop(pCamera)) != IMG_SUCCESS)
                {
                    LOG_ERROR("Failed to stop Camera\n");
                    return ret;
                }
            }

            ret = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_DEL,
                (long)pPrvCam->identifier); //INT_DG_CameraDeregister()
            if (ret < 0)
            {
                LOG_ERROR("Failed to remove kernel-side camera\n");
            }
        }

        pFound = List_visitor(&(g_pConnection->sList_cameras),
            &(pPrvCam->identifier), &List_searchCam);
        if (!pFound)
        {
            LOG_ERROR("Could not find the camera in the DG connection\n");
        }
        else
        {
            List_remove(pFound); // free the cell
        }
    }

    if (pPrvCam->pGasket != NULL)
    {
        IMG_FREE(pPrvCam->pGasket);
        pPrvCam->pGasket = NULL;
    }
    IMG_FREE(pCamera->pConverter);
    List_clearObjects(&(pPrvCam->sList_frames), &List_destroyFrame);

    IMG_FREE(pPrvCam);
    return IMG_SUCCESS;
}

IMG_RESULT DG_CameraRegister(DG_CAMERA *pCamera)
{
    INT_CAMERA *pPrvCam = NULL;
    int res;
    IMG_BOOL8 bIsMipi = IMG_TRUE;

    if (pCamera == NULL)
    {
        LOG_ERROR("pCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, INT_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier > 0)
    {
        LOG_INFO("Camera already registered to kernel-side\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    // configure the converter according to the format
    switch (pCamera->eBufferFormats)
    {
    case DG_MEMFMT_MIPI_RAW10:
        pCamera->pConverter->pfnConverter = &DG_ConvBayerToMIPI_RAW10;
        pCamera->pConverter->eMIPIFormat = DG_MIPI_ID_RAW10;
        pCamera->pConverter->eFormat = DG_FMT_MIPI;
        bIsMipi = IMG_TRUE;
        break;
    case DG_MEMFMT_MIPI_RAW12:
        pCamera->pConverter->pfnConverter = &DG_ConvBayerToMIPI_RAW12;
        pCamera->pConverter->eMIPIFormat = DG_MIPI_ID_RAW12;
        pCamera->pConverter->eFormat = DG_FMT_MIPI;
        bIsMipi = IMG_TRUE;
        break;
    case DG_MEMFMT_MIPI_RAW14:
        pCamera->pConverter->pfnConverter = &DG_ConvBayerToMIPI_RAW14;
        pCamera->pConverter->eMIPIFormat = DG_MIPI_ID_RAW14;
        pCamera->pConverter->eFormat = DG_FMT_MIPI;
        bIsMipi = IMG_TRUE;
        break;

    case DG_MEMFMT_BT656_10:
        pCamera->pConverter->pfnConverter = &DG_ConvBayerToBT656_10;
        pCamera->pConverter->eFormat = DG_FMT_BT656;
        bIsMipi = IMG_FALSE;
        break;
    case DG_MEMFMT_BT656_12:
        pCamera->pConverter->pfnConverter = &DG_ConvBayerToBT656_12;
        pCamera->pConverter->eFormat = DG_FMT_BT656;
        bIsMipi = IMG_FALSE;
        break;

    default:
        LOG_ERROR("Format not supported\n");
        return IMG_ERROR_FATAL;
    }

    if (pCamera->pConverter->eFormat == DG_FMT_MIPI
        && pCamera->bMIPI_LF == IMG_TRUE)
    {
        pCamera->pConverter->eFormat = DG_FMT_MIPI_LF;
    }

    pCamera->uiStride = DG_ConvGetConvertedStride(pCamera->pConverter,
        pCamera->uiWidth);
    pCamera->uiConvWidth = DG_ConvGetConvertedSize(pCamera->pConverter,
        pCamera->uiWidth);

    if ((res = DG_CameraVerify(pCamera)) != IMG_SUCCESS)
    {
        LOG_ERROR("The camera is badly configured\n");
        return res;
    }

    if ((res = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_REG,
        (long)pCamera)) < 0) //INT_DG_CameraRegister()
    {
        LOG_ERROR("failed to register Camera in kernel-side\n");
        return IMG_ERROR_FATAL;
    }
    if (res == 0)
    {
        LOG_ERROR("kernel-side returned 0 as an ID which is invalid\n");
        return IMG_ERROR_FATAL;
    }
    pPrvCam->identifier = res;

    return IMG_SUCCESS;
}

IMG_RESULT DG_CameraAddPool(DG_CAMERA *pCamera, IMG_UINT32 ui32ToAdd)
{
    INT_CAMERA *pPrvCam = NULL;
    IMG_UINT32 i;
    IMG_RESULT ret = IMG_SUCCESS;

    if (pCamera == NULL || ui32ToAdd == 0)
    {
        LOG_ERROR("pCamera is NULL or ui32ToAdd is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, INT_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera not registered to kernel-side\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    else if (pPrvCam->bStarted == IMG_TRUE)
    {
        LOG_ERROR("Cannot add frames when camera is running\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    else
    {
        // should we care that the camera acquired the HW when we add buffers?
        struct DG_CAMERA_PARAM param = {
            pPrvCam->identifier,
            ui32ToAdd,
            NULL,
            0
        };

        param.aMemMapIds = (int*)IMG_CALLOC(ui32ToAdd, sizeof(int));
        if (param.aMemMapIds == NULL)
        {
            LOG_ERROR("failed to allocate communication structure (%" \
                IMG_SIZEPR "u Bytes)\n", ui32ToAdd*sizeof(int));
            return IMG_ERROR_MALLOC_FAILED;
        }

        ret = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_ADD,
            (long)&param); //INT_DG_CameraAddBuffers()
        if (ret < 0)
        {
            LOG_ERROR("failed to add buffers to camera in kernel-side\n");
            IMG_FREE(param.aMemMapIds);
            return IMG_ERROR_FATAL;
        }

        for (i = 0; i < ui32ToAdd && ret == IMG_SUCCESS; i++)
        {
            INT_FRAME *pNeoFrame = (INT_FRAME*)IMG_CALLOC(1,
                sizeof(INT_FRAME));

            if (pNeoFrame == NULL)
            {
                LOG_ERROR("failed to allocate internal frame structure (%" \
                    IMG_SIZEPR "u Bytes)\n", sizeof(INT_FRAME));
                ret = IMG_ERROR_MALLOC_FAILED;
                break;
                /* that is not nice - some shots are registered on kernel
                 * side but not available on user side...
                 * but I guess we don't care, the system ran out of memory
                 */
            }

            pNeoFrame->identifier = param.aMemMapIds[i];
            pNeoFrame->uiStride = param.stride;
            pNeoFrame->uiHeight = pCamera->uiHeight;//param.height;

            pNeoFrame->pImage = SYS_IO_MemMap2(g_pConnection->fileDesc,
                param.stride*pCamera->uiHeight, PROT_WRITE, MAP_SHARED,
                param.aMemMapIds[i]);
            if (pNeoFrame->pImage == MAP_FAILED)
            {
                LOG_ERROR("Failed to map image to user-space\n");
                ret = IMG_ERROR_FATAL;
                IMG_FREE(pNeoFrame);
                /// @ clean up all already mapped buffers?
                break;
            }
            IMG_ASSERT(pNeoFrame->pImage);

            List_pushBackObject(&(pPrvCam->sList_frames), pNeoFrame);
        }

        IMG_FREE(param.aMemMapIds);
    }

    return ret;
}

IMG_RESULT DG_CameraStart(DG_CAMERA *pCamera, CI_CONNECTION *pCIConnection)
{
    INT_CAMERA *pPrvCam = NULL;
    IMG_RESULT ret;
    if (pCamera == NULL || pCIConnection == NULL)
    {
        LOG_ERROR("pCamera or pCIConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, INT_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (pPrvCam->bStarted == IMG_TRUE)
    {
        LOG_ERROR("Camera already started\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (pPrvCam->pGasket == NULL)
    {
        pPrvCam->pGasket = (CI_GASKET*)IMG_CALLOC(1, sizeof(CI_GASKET));
        if (pPrvCam->pGasket == NULL)
        {
            LOG_ERROR("failed to allocate CI_GASKET\n");
            return IMG_ERROR_MALLOC_FAILED;
        }
    }
    //
    // convert Camera to Gasket
    //

    pPrvCam->pGasket->uiGasket = pPrvCam->publicCamera.ui8DGContext;
    if (pPrvCam->publicCamera.eBufferFormats == DG_MEMFMT_BT656_10 ||
        pPrvCam->publicCamera.eBufferFormats == DG_MEMFMT_BT656_12)
    {
        pPrvCam->pGasket->bParallel = IMG_TRUE;

        switch (pPrvCam->publicCamera.eBufferFormats)
        {
        case DG_MEMFMT_BT656_10:
            pPrvCam->pGasket->ui8ParallelBitdepth = 10;
            break;
        case DG_MEMFMT_BT656_12:
            pPrvCam->pGasket->ui8ParallelBitdepth = 12;
            break;
        case DG_MEMFMT_MIPI_RAW10:
        case DG_MEMFMT_MIPI_RAW12:
        case DG_MEMFMT_MIPI_RAW14:
            LOG_ERROR("%s: Buffer format not supported\n", __FUNCTION__);
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        pPrvCam->pGasket->uiWidth = pPrvCam->publicCamera.uiWidth - 1;
        pPrvCam->pGasket->uiHeight = pPrvCam->publicCamera.uiHeight - 1;
        pPrvCam->pGasket->bHSync = IMG_TRUE;
        pPrvCam->pGasket->bVSync = IMG_TRUE;
    }
    else
    {
        pPrvCam->pGasket->bParallel = IMG_FALSE;
    }

    ret = CI_GasketAcquire(pPrvCam->pGasket, pCIConnection);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to acquire associated gasket\n");
        return ret;
    }
    pPrvCam->pCIConnection = pCIConnection;

    ret = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_STA,
        (long)pPrvCam->identifier); //INT_DG_CameraStart()
    if (ret < 0)
    {
        LOG_ERROR("kernel-side failed to start the camera\n");
        return IMG_ERROR_FATAL;
    }
    pPrvCam->bStarted = IMG_TRUE;

    return IMG_SUCCESS;
}

IMG_RESULT DG_CameraStop(DG_CAMERA *pCamera)
{
    INT_CAMERA *pPrvCam = NULL;
    int ret, tries = 0;
    if (pCamera == NULL)
    {
        LOG_ERROR("pCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, INT_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (pPrvCam->bStarted == IMG_FALSE)
    {
        LOG_ERROR("Camera is not started\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_STP,
        (long)pPrvCam->identifier); //INT_DG_CameraStop()
    ret = toImgResult(ret);
    while (ret == IMG_ERROR_INTERRUPTED) // try again
    {
        if (tries == 0)
        {
            LOG_WARNING("Camera is busy, cannot be stopped. Sleep and try"\
                "again...\n");
        }
        tries++;
        milisleep(CAMERA_STOP_SLEEP);
        ret = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_STP,
            (long)pPrvCam->identifier); //INT_DG_CameraStop()
        ret = toImgResult(ret);
    }

    if (ret == IMG_SUCCESS)
    {
        pPrvCam->bStarted = IMG_FALSE;
        // should have been created at start time
        IMG_ASSERT(pPrvCam->pGasket != NULL);

        ret = CI_GasketRelease(pPrvCam->pGasket, pPrvCam->pCIConnection);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to release the gasket!\n");
        }
    }
    return ret;
}

IMG_RESULT DG_CameraVerify(DG_CAMERA *pCamera)
{
    if (pCamera == NULL)
    {
        LOG_ERROR("pCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (pCamera->uiWidth == 0 || pCamera->uiHeight == 0)
    {
        LOG_ERROR("wrong witdh or height\n");
        return IMG_ERROR_FATAL;
    }

    /* HW does blanking = reg + DG_H_BLANKING_SUB so actual recommended
     * minimum blanking is different than ISP recommended one */
    if (pCamera->ui16HoriBlanking < (FELIX_MIN_H_BLANKING - DG_H_BLANKING_SUB)
        || pCamera->ui16VertBlanking < FELIX_MIN_V_BLANKING)
    {
        LOG_ERROR("blanking values are smaller than minimums\n");
        return IMG_ERROR_FATAL;
    }

    if (pCamera->uiStride < DG_ConvGetConvertedStride(pCamera->pConverter,
        pCamera->uiWidth))
    {
        LOG_ERROR("the stride is too small for selected format\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (pCamera->uiStride < DG_ConvGetConvertedSize(pCamera->pConverter,
        pCamera->uiWidth))
    {
        LOG_ERROR("the converter widht is bigger than the stride\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

static IMG_RESULT IMG_CameraShoot(DG_CAMERA *pCamera, const sSimImageIn* pIMG,
    IMG_BOOL8 bBlocking, const char *pszTmpFile)
{
    INT_CAMERA *pPrvCam = NULL;
    struct DG_FRAME_PARAM sParam = { 0, 0, IMG_FALSE };
    INT_FRAME *pFrame = NULL;
    IMG_RESULT ret;

    if (pCamera == NULL || pIMG == NULL)
    {
        LOG_ERROR("pCamera is NULL or pIMG is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, INT_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (pPrvCam->bStarted == IMG_FALSE)
    {
        LOG_ERROR("Camera is not started\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // IOCTL to get the next available buffer
    sParam.cameraId = pPrvCam->identifier;
    sParam.bOption = bBlocking;

    ret = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_BAQ,
        (long)&sParam); //INT_DG_CameraAcquireBuffer()
    if (ret < 0)
    {
        LOG_ERROR("Failed to acquire an available frame\n");
        return toImgResult(ret);
    }
#ifdef FELIX_FAKE // this is workaround: sometimes simulator doesn't respond
    // with further interrupts if CAM_BRE comes in to early after CAM_BAQ.
    // This should be investigated.
    LOG_INFO("waiting...\n");
    milisleep(200);
#endif
    // search for it in the list
    {
        sCell_T *pFound = List_visitor(&(pPrvCam->sList_frames),
            &(sParam.frameId), &List_searchFrame);
        if (pFound == NULL)
        {
            LOG_ERROR("Could not find frame %d in the user-mapped list\n",
                sParam.frameId);
            return IMG_ERROR_FATAL;
        }
        pFrame = (INT_FRAME*)pFound->object;
    }

    // convert it
    ret = DG_ConvSimImage(pCamera->pConverter, pIMG,
        (IMG_UINT8*)pFrame->pImage, pFrame->uiStride, pszTmpFile);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to convert the image (returned %d)\n", ret);
        sParam.bOption = IMG_FALSE;
    }
    else
    {
        sParam.bOption = IMG_TRUE;
    }

#if 0 // enable to dump converted frames
    {
        char fname[64];
        static int frame = 0;
        FILE *f = NULL;

        sprintf(fname, "extdgframe%d.dat", frame++);
        f = fopen(fname, "wb");

        fwrite(pFrame->pImage, 1, pFrame->uiStride*pFrame->uiHeight, f);
        fclose(f);
    }
#endif

    // IOCTL to trigger the shot or release the buffer
    ret = SYS_IO_Control(g_pConnection->fileDesc, DG_IOCTL_CAM_BRE,
        (long)&sParam); //INT_DG_CameraShoot()
    if (ret < 0)
    {
        LOG_ERROR("Failed to submit frame\n");
        return toImgResult(ret);
    }

    return IMG_SUCCESS;
}

IMG_RESULT DG_CameraShoot(DG_CAMERA *pCamera, const sSimImageIn* pIMG,
    const char *pszTmpFile)
{
    return IMG_CameraShoot(pCamera, pIMG, IMG_TRUE, pszTmpFile);
}

IMG_RESULT DG_CameraShootNB(DG_CAMERA *pCamera, const sSimImageIn* pIMG,
    const char *pszTmpFile)
{
    return IMG_CameraShoot(pCamera, pIMG, IMG_FALSE, pszTmpFile);
}

//
#ifdef WIN32
/* included at the bottom so that macro redefinitions of _IOR _IOW do not
 * affect the IOCTL calls */
#undef _IO
#undef _IOR
#undef _IOW
#undef IOC_IN
#undef IOC_INOUT
#undef IOC_OUT

#include <Windows.h> 

void milisleep(int mili)
{
    Sleep(mili);
}
#else
void milisleep(int mili)
{
    usleep(mili*100);
}
#endif
