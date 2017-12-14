/**
*******************************************************************************
@file dg_camera.c

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
#include "ci_internal/ci_errors.h"  // toImgErrors and toErrno
#include "ci/ci_api.h"  // access to CI_CONNECTION and gasket control

#ifdef FELIX_FAKE

// fake device
#include "sys/sys_userio_fake.h"
#endif /* FELIX_FAKE */
#define CAMERA_STOP_SLEEP 2000  // miliseconds
/* number of ties to try if not using the fake interface
 * when using the fake driver it tries forever */
#define CAMERA_STOP_TRIES 10

#include <registers/ext_data_generator.h>
// FELIX_TEST_DG_DG_MIPI_START_OF_DATA_TRANSMISSION_DG_TLPX_DEFAULT
#define REG_DEFAULT(REG, FIELD) \
    FELIX_TEST_DG_ ## REG ## _ ## FIELD ## _DEFAULT

#define LOG_TAG DG_LOG_TAG
#include <felixcommon/userlog.h>

static void milisleep(int milisec);

static IMG_BOOL8 List_destroyFrame(void *listElem, void *param)
{
    struct INT_DG_FRAME *pFrame = (struct INT_DG_FRAME*)listElem;

    IMG_FREE(pFrame);

    return IMG_TRUE;
}

static IMG_BOOL8 List_searchCam(void *listElem, void *searched)
{
    struct INT_DG_CAMERA *pCurr = (struct INT_DG_CAMERA*)listElem;
    int *camId = (int*)searched;

    if (pCurr->identifier == *camId)
    {
        return IMG_FALSE;  // we found it - we stop
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_searchFrame(void *listElem, void *searched)
{
    struct INT_DG_FRAME *pFrame = (struct INT_DG_FRAME*)listElem;
    int *offset = (int*)searched;

    if (pFrame->identifier == (IMG_UINT)*offset)
    {
        return IMG_FALSE;  // we found it - we stop
    }
    return IMG_TRUE;
}

static IMG_BOOL8 List_unmapFrame(void *elem, void *param)
{
    struct INT_DG_FRAME *pFrame = (struct INT_DG_FRAME*)elem;
    struct INT_DG_CONNECTION *pDG = (struct INT_DG_CONNECTION*)param;

    if (pFrame->data && pDG)
    {
        // could also delete the kernel-side object
        SYS_IO_MemUnmap(pDG->fileDesc, pFrame->data,
            pFrame->ui32Size);
        pFrame->data = NULL;
    }

    return IMG_TRUE;  // continue
}

IMG_RESULT DG_CameraCreate(DG_CAMERA **ppCamera, DG_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    struct INT_DG_CONNECTION *pConn = NULL;
    struct INT_DG_CAMERA *pPrvCam = NULL;

    if (!ppCamera || *ppCamera)
    {
        LOG_ERROR("ppCamera is NULL or *ppCamera already has an object\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (!pConnection)
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pConn = container_of(pConnection, struct INT_DG_CONNECTION,
        publicConnection);
    IMG_ASSERT(pConn);

    pPrvCam = (struct INT_DG_CAMERA *)IMG_CALLOC(1,
        sizeof(struct INT_DG_CAMERA));
    if (!pPrvCam)
    {
        LOG_ERROR("failed to allocate INT_CAMERA (%" IMG_SIZEPR "u Bytes)\n",
            sizeof(struct INT_DG_CAMERA));
        return IMG_ERROR_MALLOC_FAILED;
    }

    pPrvCam->sCell.object = pPrvCam;
    pPrvCam->pConnection = pConn;

    ret = List_init(&(pPrvCam->sList_frames));
    if (ret)  // unlikely
    {
        IMG_FREE(pPrvCam);
        return IMG_ERROR_FATAL;
    }

    ret = SYS_IO_Control(pConn->fileDesc, DG_IOCTL_CAM_REG, 0);
    if (0 > ret)
    {
        ret = toImgResult(ret);
        IMG_FREE(pPrvCam);
    }
    else
    {
        pPrvCam->identifier = ret;

        ret = List_pushBack(&(pConn->sList_cameras), &(pPrvCam->sCell));
        if (ret)  // unlikely
        {
            LOG_ERROR("failed to push the camera in the list");
            // frame list is empty
            // should we notify kernel-space?
            IMG_FREE(pPrvCam);
            return IMG_ERROR_FATAL;
        }

        *ppCamera = &(pPrvCam->publicCamera);

        // these are the defaults values for mipi from registers
        (*ppCamera)->ui16MipiTLPX =
            REG_DEFAULT(DG_MIPI_START_OF_DATA_TRANSMISSION, DG_TLPX);
        (*ppCamera)->ui16MipiTHS_prepare =
            REG_DEFAULT(DG_MIPI_START_OF_DATA_TRANSMISSION, DG_THS_PREPARE);
        (*ppCamera)->ui16MipiTHS_zero =
            REG_DEFAULT(DG_MIPI_START_OF_DATA_TRANSMISSION, DG_THS_ZERO);
        (*ppCamera)->ui16MipiTHS_trail =
            REG_DEFAULT(DG_MIPI_END_OF_DATA_TRANSMISSION, DG_THS_TRAIL);
        (*ppCamera)->ui16MipiTHS_exit =
            REG_DEFAULT(DG_MIPI_END_OF_DATA_TRANSMISSION, DG_THS_EXIT);
        (*ppCamera)->ui16MipiTCLK_prepare =
            REG_DEFAULT(DG_MIPI_START_OF_CLOCK_TRANSMISSION, DG_TCLK_PREPARE);
        (*ppCamera)->ui16MipiTCLK_zero =
            REG_DEFAULT(DG_MIPI_START_OF_CLOCK_TRANSMISSION, DG_TCLK_ZERO);
        (*ppCamera)->ui16MipiTCLK_post =
            REG_DEFAULT(DG_MIPI_END_OF_CLOCK_TRANSMISSION, DG_TCLK_POST);
        (*ppCamera)->ui16MipiTCLK_trail =
            REG_DEFAULT(DG_MIPI_END_OF_CLOCK_TRANSMISSION, DG_TCLK_TRAIL);
    }

    return ret;
}

IMG_RESULT DG_CameraDestroy(DG_CAMERA *pCamera)
{
    struct INT_DG_CAMERA *pPrvCam = NULL;
    struct INT_DG_CONNECTION *pConn = NULL;
    IMG_RESULT ret;

    if (!pCamera)
    {
        LOG_ERROR("pCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, struct INT_DG_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    pConn = pPrvCam->pConnection;

    // if NULL it is clean-up time - the connection is removing the cells
    if (pConn)
    {
        sCell_T *pFound = NULL;

        if (pPrvCam->identifier > 0)
        {
            if (pPrvCam->bStarted)
            {
                ret = DG_CameraStop(pCamera);
                if (ret)
                {
                    LOG_ERROR("Failed to stop Camera\n");
                    return ret;
                }
            }

            List_visitor(&(pPrvCam->sList_frames), pConn, &List_unmapFrame);

            ret = SYS_IO_Control(pConn->fileDesc, DG_IOCTL_CAM_DEL,
                (long)pPrvCam->identifier);
            if (ret < 0)
            {
                LOG_ERROR("Failed to remove kernel-side camera\n");
            }
        }

        pFound = List_visitor(&(pConn->sList_cameras),
            &(pPrvCam->identifier), &List_searchCam);
        if (!pFound)
        {
            LOG_ERROR("Could not find the camera in the DG connection\n");
        }
        else
        {
            List_detach(pFound);
        }
    }

    // destroy user-side allocation
    List_visitor(&(pPrvCam->sList_frames), NULL, &List_destroyFrame);

    IMG_FREE(pPrvCam);
    return IMG_SUCCESS;
}

IMG_RESULT DG_CameraAllocateFrame(DG_CAMERA *pCamera, IMG_UINT32 ui32Size,
    IMG_UINT32 *pFrameID)
{
    struct INT_DG_CAMERA *pPrvCam = NULL;
    struct DG_BUFFER_PARAM sParam;
    IMG_RESULT ret;

    if (!pCamera || ui32Size == 0)
    {
        LOG_ERROR("pCamera is NULL or size is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, struct INT_DG_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_MEMSET(&sParam, 0, sizeof(struct DG_BUFFER_PARAM));

    sParam.cameraId = pPrvCam->identifier;
    sParam.uiAllocSize = ui32Size;

    ret = SYS_IO_Control(pPrvCam->pConnection->fileDesc,
        DG_IOCTL_CAM_ADD, (long)&sParam);
    ret = toImgResult(ret);
    if (!ret)
    {
        struct INT_DG_FRAME *pNew = (struct INT_DG_FRAME*)IMG_CALLOC(1,
            sizeof(struct INT_DG_FRAME));

        if (!pNew)
        {
            LOG_ERROR("failed to allocate internal frame structure (%" \
                IMG_SIZEPR "u Bytes)\n", sizeof(struct INT_DG_FRAME));
            return IMG_ERROR_MALLOC_FAILED;
            /* that is not nice - some shots are registered on kernel
             * side but not available on user side...
             * but I guess we don't care, the system ran out of memory
             */
        }

        pNew->sCell.object = pNew;
        pNew->pParent = pPrvCam;
        pNew->identifier = sParam.bufferId;
        pNew->ui32Size = ui32Size;

        pNew->data = SYS_IO_MemMap2(pPrvCam->pConnection->fileDesc,
            pNew->ui32Size, PROT_WRITE, MAP_SHARED,
            pNew->identifier);
        if (!pNew->data)
        {
            LOG_ERROR("Failed to map image to user-space\n");
            IMG_FREE(pNew);
            return IMG_ERROR_FATAL;
        }

        ret = List_pushBack(&(pPrvCam->sList_frames), &(pNew->sCell));
        IMG_ASSERT(IMG_SUCCESS == ret);  // should never fail
    }
    else
    {
        LOG_ERROR("Failed to allocate new buffer of %u Bytes\n", ui32Size);
    }

    return ret;
}

IMG_RESULT DG_CameraStart(DG_CAMERA *pCamera)
{
    struct INT_DG_CAMERA *pPrvCam = NULL;
    IMG_RESULT ret;
    struct DG_CAMERA_PARAM sParam;

    if (!pCamera)
    {
        LOG_ERROR("pCamera or pCIConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, struct INT_DG_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    //  may need to ask HW side because could be stopped on suspend
    if (pPrvCam->bStarted == IMG_TRUE)
    {
        LOG_ERROR("Camera already started\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    IMG_MEMSET(&sParam, 0, sizeof(struct DG_CAMERA_PARAM));
    sParam.cameraId = pPrvCam->identifier;
    sParam.eBayerOrder = pCamera->eBayerOrder;
    sParam.eFormat = pCamera->eFormat;
    sParam.ui8FormatBitdepth = pCamera->ui8FormatBitdepth;
    sParam.ui8Gasket = pCamera->ui8Gasket;
    if (CI_DGFMT_MIPI <= pCamera->eFormat)
    {
        sParam.ui16MipiTLPX = pCamera->ui16MipiTLPX;
        sParam.ui16MipiTHS_prepare = pCamera->ui16MipiTHS_prepare;
        sParam.ui16MipiTHS_zero = pCamera->ui16MipiTHS_zero;
        sParam.ui16MipiTHS_trail = pCamera->ui16MipiTHS_trail;
        sParam.ui16MipiTHS_exit = pCamera->ui16MipiTHS_exit;
        sParam.ui16MipiTCLK_prepare = pCamera->ui16MipiTCLK_prepare;
        sParam.ui16MipiTCLK_zero = pCamera->ui16MipiTCLK_zero;
        sParam.ui16MipiTCLK_post = pCamera->ui16MipiTCLK_post;
        sParam.ui16MipiTCLK_trail = pCamera->ui16MipiTCLK_trail;
    }
    else
    {
        sParam.bParallelActive[0] = pCamera->bParallelActive[0];
        sParam.bParallelActive[1] = pCamera->bParallelActive[1];
    }

    ret = SYS_IO_Control(pPrvCam->pConnection->fileDesc,
        DG_IOCTL_CAM_STA, (long)&sParam);
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
    struct INT_DG_CAMERA *pPrvCam = NULL;
    int ret, tries = CAMERA_STOP_TRIES;

    if (!pCamera)
    {
        LOG_ERROR("pCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, struct INT_DG_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    //  check with KM to know if started
    if (pPrvCam->bStarted == IMG_FALSE)
    {
        LOG_ERROR("Camera is not started\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    do
    {
        ret = SYS_IO_Control(pPrvCam->pConnection->fileDesc,
            DG_IOCTL_CAM_STP, (long)pPrvCam->identifier);
        ret = toImgResult(ret);

        if (ret)
        {
            if (CAMERA_STOP_TRIES == tries)
            {
                LOG_WARNING("Camera is busy, cannot be stopped. "\
                    "Sleep and try again in %ums %d times...\n",
                    CAMERA_STOP_SLEEP, tries);
            }
            tries--;
            milisleep(CAMERA_STOP_SLEEP);
        }
    } while (IMG_ERROR_INTERRUPTED == ret && tries);  // try again

    if (ret)
    {
        LOG_ERROR("Failed to stop the kernel-side!\n");
        return IMG_ERROR_FATAL;
    }

    pPrvCam->bStarted = IMG_FALSE;
    return IMG_SUCCESS;
}

#if DG_H_BLANKING_SUB > FELIX_MIN_H_BLANKING
#error DG blanking bigger than felix MIN
#endif

IMG_RESULT DG_FrameVerify(DG_FRAME *pFrame)
{
    struct INT_DG_FRAME *pPrvFrame = NULL;

    if (!pFrame)
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (0 == pFrame->ui32Width || 0 == pFrame->ui32Height)
    {
        LOG_ERROR("wrong witdh or height\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    /* HW does blanking = reg + DG_H_BLANKING_SUB so actual recommended
     * minimum blanking is different than ISP recommended one */
    if (FELIX_MIN_H_BLANKING > pFrame->ui32HorizontalBlanking
        || FELIX_MIN_V_BLANKING > pFrame->ui32VerticalBlanking)
    {
        LOG_ERROR("blanking values are smaller than minimums\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pPrvFrame = container_of(pFrame, struct INT_DG_FRAME, publicFrame);
    IMG_ASSERT(pPrvFrame);

    IMG_ASSERT(pPrvFrame->ui32Size == pFrame->ui32AllocSize);

    if ((pFrame->ui32Stride * pFrame->ui32Height) > pFrame->ui32AllocSize)
    {
        LOG_ERROR("stride (%u) * height (%u) = %u > allocated size (%u)\n",
            pFrame->ui32Stride, pFrame->ui32Height,
            (pFrame->ui32Stride * pFrame->ui32Height), pFrame->ui32AllocSize);
        return IMG_ERROR_FATAL;
    }

    if (pPrvFrame->pParent->publicCamera.eBayerOrder != pFrame->eBayerMosaic
        || pPrvFrame->pParent->publicCamera.eFormat != pFrame->eFormat
        || pPrvFrame->pParent->publicCamera.ui8FormatBitdepth
        != pFrame->ui8FormatBitdepth)
    {
        LOG_ERROR("frame format different from the camera it is "\
            "attached to!\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

/**
 * if ui32FrameID == 0 then get the 1st available
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera or ppFrame is NULL
 * @return IMG_ERROR_NOT_INITIALISED if the Camera is not registered
 * to kernel-side
 * @return IMG_ERROR_FATAL if kernel side did not find a frame or the chosen
 * frame is not available in user-side
 */
static IMG_RESULT IMG_DG_GetAvailableFrame(DG_CAMERA *pCamera,
    IMG_UINT32 ui32FrameID, DG_FRAME **ppFrame)
{
    struct INT_DG_CAMERA *pPrvCam = NULL;
    struct DG_FRAME_PARAM sParam;
    int frameId = (int)ui32FrameID;
    IMG_RESULT ret;
    int tries = 5;
    sCell_T *pFound = NULL;

    if (!pCamera || !ppFrame)
    {
        LOG_ERROR("pCamera or ppFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, struct INT_DG_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_MEMSET(&sParam, 0, sizeof(struct DG_FRAME_PARAM));
    sParam.cameraId = pPrvCam->identifier;
    sParam.frameId = frameId;

    do {
        ret = SYS_IO_Control(pPrvCam->pConnection->fileDesc,
            DG_IOCTL_CAM_BAQ, (long)&sParam);
        ret = toImgResult(ret);
    } while (IMG_ERROR_INTERRUPTED == ret && tries >= 0);

    if (ret)
    {
        LOG_ERROR("Failed to find frame %d\n", sParam.frameId);
        return IMG_ERROR_FATAL;
    }

    pFound = List_visitor(&(pPrvCam->sList_frames),
        &(sParam.frameId), &List_searchFrame);
    if (pFound)
    {
        struct INT_DG_FRAME *pFrame =
            container_of(pFound, struct INT_DG_FRAME, sCell);
        IMG_ASSERT(pFrame);

        *ppFrame = &(pFrame->publicFrame);
        // update the fields
        pFrame->publicFrame.data = pFrame->data;
        pFrame->publicFrame.ui32AllocSize = pFrame->ui32Size;
        pFrame->publicFrame.ui32FrameID = pFrame->identifier;
        pFrame->publicFrame.ui32HorizontalBlanking = FELIX_MIN_H_BLANKING;
        pFrame->publicFrame.ui32VerticalBlanking = FELIX_MIN_V_BLANKING;

        return IMG_SUCCESS;
    }

    // user-space and kernel space are out of sync?
    LOG_ERROR("Did not find frame %d in the user-space list\n",
        sParam.frameId);
    // we could delete the frame in kernel
    return IMG_ERROR_FATAL;
}

static IMG_RESULT IMG_DG_FrameTrigger(DG_FRAME *pFrame, IMG_BOOL8 bTrigger)
{
    struct INT_DG_FRAME *pPrvFrame = NULL;
    struct DG_FRAME_PARAM sParam;
    IMG_RESULT ret;

    if (!pFrame)
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvFrame = container_of(pFrame, struct INT_DG_FRAME, publicFrame);
    IMG_ASSERT(pPrvFrame);

    if (pPrvFrame->pParent->identifier <= 0)
    {
        LOG_ERROR("Camera is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_MEMSET(&sParam, 0, sizeof(struct DG_FRAME_PARAM));
    sParam.cameraId = pPrvFrame->pParent->identifier;
    sParam.frameId = pPrvFrame->identifier;
    sParam.bOption = bTrigger;
    sParam.ui32HoriBlanking = pFrame->ui32HorizontalBlanking;
    sParam.ui32VertBlanking = pFrame->ui32VerticalBlanking;
    if (CI_DGFMT_MIPI <= pFrame->eFormat)
    {
        sParam.ui16Width = pFrame->ui32PacketWidth;
    }
    else
    {
        sParam.ui16Width = pFrame->ui32Width;
    }
    sParam.ui16Height = pFrame->ui32Height;
    sParam.ui32Stride = pFrame->ui32Stride;

    if (pFrame->ui32Stride*pFrame->ui32Height > pPrvFrame->ui32Size
        && bTrigger)
    {
        // not a problem if we just cancel the acquisition
        LOG_ERROR("Given frame stride*height (%u B) is bigger than "\
            "allocated memory (%u B)!\n",
            pFrame->ui32Stride*pFrame->ui32Height,
            pPrvFrame->ui32Size);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = SYS_IO_Control(pPrvFrame->pParent->pConnection->fileDesc,
        DG_IOCTL_CAM_BRE, (long)&sParam);
    ret = toImgResult(ret);
    if (ret)
    {
        if (bTrigger)
        {
            LOG_ERROR("failed to insert frame %u\n", pPrvFrame->identifier);
        }
        else
        {
            LOG_ERROR("failed to release frame %u\n", pPrvFrame->identifier);
        }
        ret = IMG_ERROR_FATAL;
    }

    return ret;
}

DG_FRAME* DG_CameraGetAvailableFrame(DG_CAMERA *pCamera)
{
    DG_FRAME *pFrame = NULL;
    IMG_RESULT ret = IMG_DG_GetAvailableFrame(pCamera, 0, &pFrame);
    if (ret)
    {
        return NULL;
    }
    return pFrame;
}

DG_FRAME* DG_CameraGetFrame(DG_CAMERA *pCamera, IMG_UINT32 ui32FrameID)
{
    DG_FRAME *pFrame = NULL;
    IMG_RESULT ret =
        IMG_DG_GetAvailableFrame(pCamera, ui32FrameID, &pFrame);
    if (ret)
    {
        return NULL;
    }
    return pFrame;
}

IMG_RESULT DG_CameraInsertFrame(DG_FRAME *pFrame)
{
    return IMG_DG_FrameTrigger(pFrame, IMG_TRUE);
}

IMG_RESULT DG_CameraReleaseFrame(DG_FRAME *pFrame)
{
    return IMG_DG_FrameTrigger(pFrame, IMG_FALSE);
}

IMG_RESULT DG_CameraWaitProcessedFrame(DG_CAMERA *pCamera,
    IMG_BOOL8 bBlocking)
{
    struct INT_DG_CAMERA *pPrvCam = NULL;
    struct DG_FRAME_WAIT sParam;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pCamera)
    {
        LOG_ERROR("pCamera is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvCam = container_of(pCamera, struct INT_DG_CAMERA, publicCamera);
    IMG_ASSERT(pPrvCam);

    if (pPrvCam->identifier <= 0)
    {
        LOG_ERROR("Camera is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_MEMSET(&sParam, 0, sizeof(struct DG_FRAME_WAIT));
    sParam.cameraId = pPrvCam->identifier;
    sParam.bBlocking = bBlocking;

#ifdef FELIX_FAKE
    /* when using fake interface the timeout in the kernel size is not
     * enough so we just continue trying */
    do
    {
        ret = SYS_IO_Control(pPrvCam->pConnection->fileDesc,
            DG_IOCTL_CAM_WAI, (long)&sParam);
    } while (ret == -EINTR || (bBlocking && ret == -ETIME));
#else
    do
    {
        ret = SYS_IO_Control(pPrvCam->pConnection->fileDesc,
            DG_IOCTL_CAM_WAI, (long)&sParam);
    } while (ret == -EINTR);
#endif
    ret = toImgResult(ret);

    if (IMG_SUCCESS != ret && !bBlocking)
    {
        LOG_ERROR("failed to wait for a processed frame to be completed\n");
    }
    return ret;
}

IMG_RESULT DG_CameraDestroyFrame(DG_FRAME *pFrame)
{
    struct INT_DG_FRAME *pPrvFrame = NULL;
    struct DG_BUFFER_PARAM sParam;
    IMG_RESULT ret;

    if (!pFrame)
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pPrvFrame = container_of(pFrame, struct INT_DG_FRAME, publicFrame);
    IMG_ASSERT(pPrvFrame);

    if (pPrvFrame->pParent->identifier <= 0)
    {
        LOG_ERROR("Camera is not registered\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    IMG_MEMSET(&sParam, 0, sizeof(struct DG_BUFFER_PARAM));
    sParam.cameraId = pPrvFrame->pParent->identifier;
    sParam.bufferId = pPrvFrame->identifier;

    ret = SYS_IO_Control(pPrvFrame->pParent->pConnection->fileDesc,
        DG_IOCTL_CAM_BDE, (long)&sParam);
    ret = toImgResult(ret);

    // we should un-map after deleting (if in use should not unmap yet!)
    // but if we un-map after deleting, the buffer does not exist anymore
    SYS_IO_MemUnmap(pPrvFrame->pParent->pConnection->fileDesc,
        pPrvFrame->data, pPrvFrame->ui32Size);

    if (ret)
    {
        LOG_ERROR("failed to destroy frame %d\n", pPrvFrame->identifier);
    }

    List_detach(&(pPrvFrame->sCell));
    IMG_FREE(pPrvFrame);
    return ret;
}

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
    usleep(mili * 100);
}
#endif
