/**
*******************************************************************************
@file dg_connection.c

@brief user and kernel-side communications for the Data Generator kernel-module

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
#include "dg_kernel/dg_camera.h"
#include "dg_kernel/dg_connection.h"
#include "dg_kernel/dg_ioctl.h"  // for the parameter structures

// #include <hw_struct/data_generation.h>

#include "ci_kernel/ci_kernel.h"  // CI_FATAL
#include "mmulib/mmu.h"  // to clean the pages when last connection is removed

#ifdef FELIX_FAKE

#include <errno.h>
/* in user mode debug copy to and from "kernel" does not make a difference
 * copy_to_user and copy_from_user return the amount of memory still to be
 * copied: i.e. 0 is the success result */
#define copy_from_user(to, from, size) ( \
    IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define copy_to_user(to, from, size) ( \
    IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define __user

#else
#include <asm/uaccess.h>  // copy_to_user and copy_from_user
#include <linux/compiler.h>  // for the __user macro
#include <linux/kernel.h>  // container_of
#include <linux/errno.h>
#endif

static IMG_BOOL8 List_FindCamera(void *listelem, void *searched)
{
    KRN_DG_CAMERA *pCam = (KRN_DG_CAMERA*)listelem;
    int *pID = (int*)searched;

    if (pCam->identifier == *pID)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

IMG_BOOL8 List_DestroyFrames(void *elem, void *param)
{
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME *)elem;
    KRN_DG_FrameDestroy(pFrame);
    return IMG_TRUE;
}

static IMG_BOOL8 List_DestroyCamera(void *elem, void *param)
{
    KRN_DG_CAMERA *pCam = (KRN_DG_CAMERA*)elem;
    KRN_DG_CameraDestroy(pCam);
    return IMG_TRUE;
}

static KRN_DG_CAMERA* findCamera(KRN_DG_CONNECTION *pConn, int camid)
{
    sCell_T *pFound = NULL;
    pFound = List_visitor(&(pConn->sList_camera),
        (void*)&camid, List_FindCamera);
    if (pFound == NULL)
    {
        return NULL;
    }
    return container_of(pFound, KRN_DG_CAMERA, sCell);
}

static IMG_BOOL8 List_FindFrame(void *elem, void *param)
{
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME *)elem;
    int *frameId = (int *)param;

    if (pFrame->identifier == *frameId)
    {
        return IMG_FALSE;  // we found it - we stop
    }
    return IMG_TRUE;
}

KRN_DG_CONNECTION* KRN_DG_ConnectionCreate(void)
{
    IMG_RESULT ret = IMG_SUCCESS;
    KRN_DG_CONNECTION *pNew = NULL;

    IMG_ASSERT(g_pDGDriver);

    pNew = (KRN_DG_CONNECTION*)IMG_CALLOC(1, sizeof(KRN_DG_CONNECTION));
    if (!pNew)
    {
        CI_FATAL("Failed to allocate the connection structure (%uB)\n",
            sizeof(KRN_DG_CONNECTION));
        return NULL;
    }

    pNew->sCell.object = pNew;

    ret = List_init(&(pNew->sList_camera));
    if (ret)  // unlikely
    {
        IMG_FREE(pNew);
        return NULL;
    }
    ret = List_init(&(pNew->sList_unmappedFrames));
    if (ret)  // unlikely
    {
        // connection list is empty
        IMG_FREE(pNew);
        return NULL;
    }

    ret = SYS_LockInit(&(pNew->sLock));
    if (ret)
    {
        // unmapped list is empty
        // connection list is empty
        IMG_FREE(pNew);
        return NULL;
    }

    SYS_LockAcquire(&(g_pDGDriver->sConnectionLock));
    {
        ret = List_pushBack(&(g_pDGDriver->sList_connection), &(pNew->sCell));
        // should never fail

        if (IMG_SUCCESS == ret
            && g_pDGDriver->sList_connection.ui32Elements == 1)
        {
            ret = KRN_CI_MMUConfigure(&(g_pDGDriver->sMMU),
                g_pDGDriver->sHWInfo.config_ui8NDatagen,
                g_pDGDriver->sHWInfo.config_ui8NDatagen,
                g_pDGDriver->sMMU.apDirectory[0] != NULL ?
                IMG_FALSE : IMG_TRUE);
            if (ret)
            {
                CI_FATAL("Failed to configure external MMU\n");
                ret = IMG_ERROR_FATAL;
            }
        }
    }
    SYS_LockRelease(&(g_pDGDriver->sConnectionLock));

    if (ret)
    {
        SYS_LockDestroy(&(pNew->sLock));
        // unmapped list is empty
        // connection list is empty
        IMG_FREE(pNew);
        pNew = NULL;
    }

    return pNew;
}

IMG_RESULT KRN_DG_ConnectionDestroy(KRN_DG_CONNECTION *pConnection)
{
    IMG_ASSERT(pConnection);

    if (g_pDGDriver != NULL)
    {
        sCell_T *pFound = NULL;

        SYS_LockAcquire(&(g_pDGDriver->sConnectionLock));
        {
            pFound = List_visitor(&(g_pDGDriver->sList_connection),
                pConnection, &ListVisitor_findPointer);
            if (!pFound)
            {
                CI_FATAL("the connection was not registered!\n");
            }
            else
            {
                List_detach(pFound);
            }

            if (g_pDGDriver->sList_connection.ui32Elements == 0)
            {
                IMG_UINT32 i;
                // no more memory request
                KRN_CI_MMUPause(&(g_pDGDriver->sMMU), IMG_TRUE);

                for (i = 0; g_pDGDriver->sMMU.apDirectory[0] != NULL
                    && i < g_pDGDriver->sHWInfo.config_ui8NDatagen; i++)
                {
                    // clean the allocated page tables
                    IMGMMU_DirectoryClean(g_pDGDriver->sMMU.apDirectory[i]);
                }
            }
        }
        SYS_LockRelease(&(g_pDGDriver->sConnectionLock));
    }

    List_visitor(&(pConnection->sList_unmappedFrames), NULL,
        &List_DestroyFrames);
    /* could clear the list as well but it is not needed as we are going to
     * free it */
    List_visitor(&(pConnection->sList_camera), NULL, &List_DestroyCamera);
    SYS_LockDestroy(&(pConnection->sLock));

    IMG_FREE(pConnection);

    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_DriverGetInfo(KRN_DG_CONNECTION *pConnection,
    DG_HWINFO __user *pUserParam)
{
    IMG_RESULT ret;
    IMG_ASSERT(pConnection);
    IMG_ASSERT(g_pDGDriver);

    ret = copy_to_user(pUserParam, &(g_pDGDriver->sHWInfo), sizeof(DG_HWINFO));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user DG_HWINFO\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_CameraRegister(KRN_DG_CONNECTION *pConnection,
    int *pGivenId)
{
    KRN_DG_CAMERA *pNew = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection);
    IMG_ASSERT(pGivenId);

    pNew = KRN_DG_CameraCreate();
    if (!pNew)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        ret = List_pushBack(&(pConnection->sList_camera), &(pNew->sCell));
        if (ret)  // unlikely
        {
            IMG_FREE(pNew);
            ret = IMG_ERROR_FATAL;
        }
        else
        {
            // pre-increment to avoid having 0
            pNew->identifier = ++(pConnection->iCameraIDBase);
        }
    }
    SYS_LockRelease(&(pConnection->sLock));

    *pGivenId = pNew->identifier;

    return ret;
}

IMG_RESULT INT_DG_CameraAddBuffers(KRN_DG_CONNECTION *pConnection,
    struct DG_BUFFER_PARAM __user *pUserParam)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret;
    KRN_DG_FRAME *pNew = NULL;
    struct DG_BUFFER_PARAM sParam;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sParam, pUserParam, sizeof(struct DG_BUFFER_PARAM));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user DG_BUFFER_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCam = findCamera(pConnection, sParam.cameraId);
    if (!pCam)
    {
        CI_FATAL("camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    ret = KRN_DG_FrameCreate(&pNew, sParam.uiAllocSize, pCam);
    if (ret)
    {
        CI_FATAL("Failed to create frame\n");
        return ret;
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        pNew->identifier = (++pConnection->iFrameIDBase);  // <<PAGE_SHIFT;
        ret = List_pushBack(&(pConnection->sList_unmappedFrames),
            &(pNew->sListCell));
    }
    SYS_LockRelease(&(pConnection->sLock));

    IMG_ASSERT(pNew->identifier > 0);

    if (ret)
    {
        KRN_DG_FrameDestroy(pNew);
        return ret;
    }

    //  check that we map it to device MMU if dg is started

    sParam.bufferId = pNew->identifier;

    ret = copy_to_user(pUserParam, &sParam, sizeof(struct DG_BUFFER_PARAM));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user DG_BUFFER_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_CameraStart(KRN_DG_CONNECTION *pConnection,
    const struct DG_CAMERA_PARAM __user *pUserParam)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret;
    struct DG_CAMERA_PARAM sParam;

    IMG_ASSERT(pConnection);

    ret = copy_from_user(&sParam, pUserParam, sizeof(struct DG_CAMERA_PARAM));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user DG_CAMERA_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCam = findCamera(pConnection, sParam.cameraId);
    if (!pCam)
    {
        CI_FATAL("camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    pCam->publicCamera.eBayerOrder = sParam.eBayerOrder;
    pCam->publicCamera.eFormat = sParam.eFormat;
    pCam->publicCamera.ui8FormatBitdepth = sParam.ui8FormatBitdepth;
    pCam->publicCamera.ui8Gasket = sParam.ui8Gasket;

    pCam->publicCamera.bParallelActive[0] = sParam.bParallelActive[0];
    pCam->publicCamera.bParallelActive[1] = sParam.bParallelActive[1];

    pCam->publicCamera.ui16MipiTLPX = sParam.ui16MipiTLPX;
    pCam->publicCamera.ui16MipiTHS_prepare = sParam.ui16MipiTHS_prepare;
    pCam->publicCamera.ui16MipiTHS_zero = sParam.ui16MipiTHS_zero;
    pCam->publicCamera.ui16MipiTHS_trail = sParam.ui16MipiTHS_trail;
    pCam->publicCamera.ui16MipiTHS_exit = sParam.ui16MipiTHS_exit;
    pCam->publicCamera.ui16MipiTCLK_prepare = sParam.ui16MipiTCLK_prepare;
    pCam->publicCamera.ui16MipiTCLK_zero = sParam.ui16MipiTCLK_zero;
    pCam->publicCamera.ui16MipiTCLK_post = sParam.ui16MipiTCLK_post;
    pCam->publicCamera.ui16MipiTCLK_trail = sParam.ui16MipiTCLK_trail;

    ret = KRN_DG_CameraStart(pCam);
    if (ret)
    {
        CI_FATAL("failed to start the camera\n");
    }

    return ret;
}

IMG_RESULT INT_DG_CameraStop(KRN_DG_CONNECTION *pConnection, int cameraId)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pConnection);

    pCam = findCamera(pConnection, cameraId);
    if (!pCam)
    {
        CI_FATAL("camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    ret = IMG_SUCCESS;
    SYS_LockAcquire(&(pCam->sLock));
    {
        if (pCam->sList_pending.ui32Elements > 0)
        {
            ret = IMG_ERROR_INTERRUPTED;  // try again later
        }
    }
    SYS_LockRelease(&(pCam->sLock));

    if (IMG_SUCCESS == ret)
    {
        ret = KRN_DG_CameraStop(pCam);
        if (ret)
        {
            CI_FATAL("Failed to stop the camera\n");  // unlikely
        }
    }
    else
    {
        CI_FATAL("Tried to stop DG while still have pending frames\n");
    }

    return ret;
}

IMG_RESULT INT_DG_CameraAcquireBuffer(KRN_DG_CONNECTION *pConnection,
    struct DG_FRAME_PARAM __user *pUserParam)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    KRN_DG_FRAME *pFrame = NULL;
    struct DG_FRAME_PARAM sParam;
    sCell_T *pFound = NULL;

    IMG_ASSERT(pConnection != NULL);

    ret = copy_from_user(&sParam, pUserParam, sizeof(struct DG_FRAME_PARAM));
    if (ret)
    {
        CI_FATAL("Failed to copy_from_user DG_FRAME_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCam = findCamera(pConnection, sParam.cameraId);
    if (!pCam)
    {
        CI_FATAL("camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    SYS_LockAcquire(&(pCam->sLock));
    {
        if (sParam.frameId > 0)
        {
            pFound = List_visitor(&(pCam->sList_available),
                &(sParam.frameId), &List_FindFrame);
        }
        else
        {
            pFound = List_getHead(&(pCam->sList_available));
        }

        if (pFound)
        {
            pFrame = container_of(pFound, KRN_DG_FRAME, sListCell);

            List_detach(pFound);
            List_pushBack(&(pCam->sList_converting), pFound);
        }
        else
        {
            ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
        }
    }
    SYS_LockRelease(&(pCam->sLock));

    if (ret)
    {
        CI_FATAL("Could not find frame %d in the available list!\n",
            sParam.frameId);
        return ret;
    }

    // cannot be NULL because we checked the semaphore beforehand
    IMG_ASSERT(pFrame);
    IMG_ASSERT(IMG_SUCCESS == ret);

    sParam.frameId = pFrame->identifier;

    ret = copy_to_user(pUserParam, &sParam, sizeof(struct DG_FRAME_PARAM));
    if (ret)
    {
        CI_FATAL("failed to copy_to_user DG_FRAME_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_CameraShoot(KRN_DG_CONNECTION *pConnection,
    const struct DG_FRAME_PARAM __user *pUserParam)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    struct DG_FRAME_PARAM sParam;

    IMG_ASSERT(pConnection);

    ret = copy_from_user(&sParam, pUserParam, sizeof(struct DG_FRAME_PARAM));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user DG_FRAME_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCam = findCamera(pConnection, sParam.cameraId);
    if (!pCam)
    {
        CI_FATAL("camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    // bOption used as trigger option
    ret = KRN_DG_CameraShoot(pCam, &sParam);
    if (ret)
    {
        CI_FATAL("Failed to shoot/release\n");
    }

    return ret;
}

IMG_RESULT INT_DG_CameraWaitProcessed(KRN_DG_CONNECTION *pConnection,
    const struct DG_FRAME_WAIT __user *pParam)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    struct DG_FRAME_WAIT sParam;

    IMG_ASSERT(pConnection);

    ret = copy_from_user(&sParam, pParam, sizeof(struct DG_FRAME_WAIT));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user DG_BUFFER_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCam = findCamera(pConnection, sParam.cameraId);
    if (!pCam)
    {
        CI_FATAL("camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    ret = KRN_DG_CameraWaitProcessed(pCam, sParam.bBlocking);
    return ret;
}

IMG_RESULT INT_DG_CameraDeleteBuffer(KRN_DG_CONNECTION *pConnection,
    const struct DG_BUFFER_PARAM __user *pUserParam)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    struct DG_BUFFER_PARAM sParam;
    KRN_DG_FRAME *pFrame = NULL;

    IMG_ASSERT(pConnection);

    ret = copy_from_user(&sParam, pUserParam, sizeof(struct DG_BUFFER_PARAM));
    if (ret)
    {
        CI_FATAL("failed to copy_from_user DG_BUFFER_PARAM\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCam = findCamera(pConnection, sParam.cameraId);
    if (!pCam)
    {
        CI_FATAL("camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    SYS_LockAcquire(&(pCam->sLock));
    {
        sCell_T *pFound = NULL;

        // need to be acquired first
        pFound = List_visitor(&(pCam->sList_converting),
            (void*)&(sParam.bufferId), &List_FindFrame);
        if (pFound)
        {
            pFrame = container_of(pFound, KRN_DG_FRAME, sListCell);

            ret = List_detach(pFound);
        }
    }
    SYS_LockRelease(&(pCam->sLock));

    if (pFrame)
    {
        if (pCam->bStarted)
        {
            // flush the MMU
            struct MMUDirectory *pDir =
                g_pDGDriver->sMMU.apDirectory[pFrame->pParent->\
                publicCamera.ui8Gasket];
            if (pDir)
            {
                KRN_DG_FrameUnmap(pFrame);

                KRN_CI_MMUFlushCache(&(g_pDGDriver->sMMU),
                    pFrame->pParent->publicCamera.ui8Gasket,
                    IMG_FALSE);
            }
        }

        ret = KRN_DG_FrameDestroy(pFrame);
        if (ret)
        {
            CI_FATAL("failed to destroy frame %u\n", sParam.bufferId);
        }
    }
    else
    {
        CI_FATAL("failed to find frame %u in the converting list\n",
            sParam.bufferId);
        ret = IMG_ERROR_FATAL;
    }

    return ret;
}

IMG_RESULT INT_DG_CameraDeregister(KRN_DG_CONNECTION *pConnection,
    int iCameraId)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    sCell_T *pFound = NULL;

    IMG_ASSERT(pConnection);

    SYS_LockAcquire(&(pConnection->sLock));
    {
        pFound = List_visitor(&(pConnection->sList_camera),
            (void*)&iCameraId, List_FindCamera);
        if (pFound)
        {
            pCam = container_of(pFound, KRN_DG_CAMERA, sCell);
            List_detach(pFound);
        }
    }
    SYS_LockRelease(&(pConnection->sLock));

    if (pCam != NULL)
    {
        ret = KRN_DG_CameraDestroy(pCam);
    }
    else
    {
        CI_FATAL("Camera is not registered\n");
        ret = IMG_ERROR_FATAL;
    }

    return ret;
}
