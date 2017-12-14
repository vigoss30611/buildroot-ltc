/**
******************************************************************************
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
#include "dg_kernel/dg_ioctl.h" // for the parameter structures

//#include <hw_struct/data_generation.h>

#include "ci_kernel/ci_kernel.h" // CI_FATAL
#include "mmulib/mmu.h" // to clean the pages when last connection is removed

#ifdef FELIX_FAKE

#include <errno.h>
// in user mode debuge copy to and from "kernel" does not make a difference
// copy_to_user and copy_from_user return the amount of memory still to be copied: i.e. 0 is the success result
#define copy_from_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define copy_to_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define __user

#else
#include <asm/uaccess.h> // copy_to_user and copy_from_user
#include <linux/compiler.h> // for the __user macro
#include <linux/kernel.h> // container_of
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

static void List_DestroyCamera(void *listelem)
{
    KRN_DG_CAMERA *pCam = (KRN_DG_CAMERA*)listelem;

    KRN_DG_CameraDestroy(pCam);
}

static KRN_DG_CAMERA* findCamera(KRN_DG_CONNECTION *pConn, int camid)
{
    sCell_T *pFound = NULL;
    pFound = List_visitor(&(pConn->sList_camera), (void*)&camid, List_FindCamera);
    if (pFound == NULL)
    {
        return NULL;
    }
    return (KRN_DG_CAMERA*)pFound->object;
}

IMG_RESULT KRN_DG_ConnectionCreate(KRN_DG_CONNECTION **ppConnection)
{
    IMG_RESULT ret = IMG_SUCCESS;

    if (g_pDGDriver == NULL)
    {
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (ppConnection == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (*ppConnection != NULL)
    {
        return IMG_ERROR_MEMORY_IN_USE;
    }

    *ppConnection = (KRN_DG_CONNECTION*)IMG_CALLOC(1, sizeof(KRN_DG_CONNECTION));
    if (*ppConnection == NULL)
    {
        CI_FATAL("Failed to allocate the connection structure (%ld bytes)\n", sizeof(KRN_DG_CONNECTION));
        return IMG_ERROR_MALLOC_FAILED;
    }

    if ((ret = List_init(&((*ppConnection)->sList_camera))) != IMG_SUCCESS)
    {
        IMG_FREE(*ppConnection);
        *ppConnection = NULL;
        return ret;
    }
    if ((ret = List_init(&((*ppConnection)->sList_unmappedFrames))) != IMG_SUCCESS)
    {
        // connection list is empty
        IMG_FREE(*ppConnection);
        *ppConnection = NULL;
        return ret;
    }

    if ((ret = SYS_LockInit(&((*ppConnection)->sLock))) != IMG_SUCCESS)
    {
        // unmapped list is empty
        // connection list is empty
        IMG_FREE(*ppConnection);
        *ppConnection = NULL;
        return ret;
    }

    SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
    {
        ret = List_pushBackObject(&(g_pDGDriver->sList_connection), *ppConnection);

        if (ret == IMG_SUCCESS && g_pDGDriver->sList_connection.ui32Elements == 1)
        {
            if ((ret = KRN_CI_MMUConfigure(&(g_pDGDriver->sMMU), g_pDGDriver->sHWInfo.config_ui8NDatagen, g_pDGDriver->sHWInfo.config_ui8NDatagen, g_pDGDriver->sMMU.apDirectory[0] != NULL ? IMG_FALSE : IMG_TRUE)) != IMG_SUCCESS)
            {
                CI_FATAL("Failed to configure external MMU\n");
            }
        }
    }
    SYS_LockRelease(&(g_psCIDriver->sConnectionLock));

    if (ret != IMG_SUCCESS)
    {
        SYS_LockDestroy(&((*ppConnection)->sLock));
        // unmapped list is empty
        // connection list is empty
        IMG_FREE(*ppConnection);
        *ppConnection = NULL;
    }

    return ret;
}

IMG_RESULT KRN_DG_ConnectionDestroy(KRN_DG_CONNECTION *pConnection)
{
    IMG_ASSERT(pConnection != NULL);

    if (g_pDGDriver != NULL)
    {
        sCell_T *pFound = NULL;

        SYS_LockAcquire(&(g_psCIDriver->sConnectionLock));
        {
            if ((pFound = List_visitor(&(g_pDGDriver->sList_connection), pConnection, &ListVisitor_findPointer)) == NULL)
            {
                CI_FATAL("the connection was not registered!\n");
            }
            else
            {
                List_remove(pFound); // free the cell
            }

            if (g_pDGDriver->sList_connection.ui32Elements == 0)
            {
                IMG_UINT32 i;
                // no more memory request
                KRN_CI_MMUPause(&(g_pDGDriver->sMMU), IMG_TRUE);

                for (i = 0; g_pDGDriver->sMMU.apDirectory[0] != NULL && i < g_pDGDriver->sHWInfo.config_ui8NDatagen; i++)
                {
                    IMGMMU_DirectoryClean(g_pDGDriver->sMMU.apDirectory[i]); // clean the allocated page tables
                }
            }
        }
        SYS_LockRelease(&(g_psCIDriver->sConnectionLock));
    }

    List_visitor(&(pConnection->sList_unmappedFrames), NULL, &List_DestroyFrames);
    // could clear the list as well but it is not needed as we are going to free it
    List_clearObjects(&(pConnection->sList_camera), &List_DestroyCamera);
    SYS_LockDestroy(&(pConnection->sLock));

    IMG_FREE(pConnection);

    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_CameraRegister(KRN_DG_CONNECTION *pConnection, DG_CAMERA __user *pCam, int *pGivenId)
{
    KRN_DG_CAMERA *pNeo = NULL;
    IMG_RESULT ret;
    if (pConnection == NULL || pCam == NULL || pGivenId == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ((ret = KRN_DG_CameraCreate(&pNeo)) != IMG_SUCCESS)
    {
        return ret;
    }

    if (copy_from_user(&(pNeo->publicCamera), pCam, sizeof(DG_CAMERA)) != 0)
    {
        CI_FATAL("Failed to copy_from_user camera information\n");
        IMG_FREE(pNeo);
        return IMG_ERROR_FATAL;
    }

    if (pNeo->publicCamera.ui8DGContext >= g_pDGDriver->sHWInfo.config_ui8NDatagen)
    {
        CI_FATAL("trying to use camera %d but there is only %d cameras available\n", pNeo->publicCamera.ui8DGContext, g_pDGDriver->sHWInfo.config_ui8NDatagen);
        IMG_FREE(pNeo);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        if ((ret = List_pushBackObject(&(pConnection->sList_camera), pNeo)) != IMG_SUCCESS)
        {
            IMG_FREE(pNeo);
        }
        else
        {
            pNeo->identifier = ++(pConnection->iCameraIDBase); // pre-increment to avoid having 0
        }
    }
    SYS_LockRelease(&(pConnection->sLock));
    *pGivenId = pNeo->identifier;

    return ret;
}

IMG_RESULT INT_DG_CameraAddBuffers(KRN_DG_CONNECTION *pConnection, struct DG_CAMERA_PARAM *pParam)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_UINT32 i;
    IMG_RESULT ret;
    KRN_DG_FRAME *pNeo = NULL;

    IMG_ASSERT(pConnection != NULL);

    if (pParam->nbBuffers == 0) // only if corrupted user-side
    {
        CI_FATAL("nbBuffers to add is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ((pCam = findCamera(pConnection, pParam->cameraId)) == NULL)
    {
        CI_FATAL("Camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    for (i = 0; i < pParam->nbBuffers; i++)
    {
        pNeo = NULL;

        if ((ret = KRN_DG_FrameCreate(&pNeo, pCam)) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to create frame\n");
            return ret;
        }

        SYS_LockAcquire(&(pConnection->sLock));
        {
            pNeo->identifier = (++pConnection->iFrameIDBase);//<<PAGE_SHIFT;
            ret = List_pushBack(&(pConnection->sList_unmappedFrames), &(pNeo->sListCell));
        }
        SYS_LockRelease(&(pConnection->sLock));
        IMG_ASSERT(pNeo->identifier > 0);

        if (ret != IMG_SUCCESS)
        {
            KRN_DG_FrameDestroy(pNeo);
            return ret;
        }

        pParam->aMemMapIds[i] = pNeo->identifier;
    }

    // using the last buffer to fill up information
    pParam->stride = pNeo->uiStride;

    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_CameraStart(KRN_DG_CONNECTION *pConnection, int cameraId)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret;

    if (pConnection == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ((pCam = findCamera(pConnection, cameraId)) == NULL)
    {
        return IMG_ERROR_FATAL;
    }

    if ((ret = KRN_DG_CameraStart(pCam)) != IMG_SUCCESS)
    {
        CI_FATAL("failed to acquire the camera context\n");
    }

    return ret;
}

IMG_RESULT INT_DG_CameraStop(KRN_DG_CONNECTION *pConnection, int cameraId)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret;

    if (pConnection == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ((pCam = findCamera(pConnection, cameraId)) == NULL)
    {
        return IMG_ERROR_FATAL;
    }

    ret = IMG_SUCCESS;
    SYS_LockAcquire(&(pCam->sPendingLock));
    {
        if (pCam->sList_pending.ui32Elements > 0)
        {
            ret = IMG_ERROR_INTERRUPTED;  // try again later
        }
    }
    SYS_LockRelease(&(pCam->sPendingLock));

    if (ret == IMG_SUCCESS)
    {
        if ((ret = KRN_DG_CameraStop(pCam)) != IMG_SUCCESS)
        {
            CI_FATAL("Failed to stop the camera\n");  // unlikely
        }
    }

    return ret;
}

IMG_RESULT INT_DG_CameraAcquireBuffer(KRN_DG_CONNECTION *pConnection, int cameraId, IMG_BOOL8 bBlocking, int *pGivenShot)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    KRN_DG_FRAME *pFrame = NULL;

    IMG_ASSERT(pConnection != NULL);
    IMG_ASSERT(pGivenShot != NULL);

    if ((pCam = findCamera(pConnection, cameraId)) == NULL)
    {
        CI_FATAL("Camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    if (bBlocking == IMG_FALSE)
    {
        if (SYS_SemTryDecrement(&(pCam->sSemAvailable)) != IMG_SUCCESS)
        {
            CI_FATAL("Camera does not have an available frame\n");
            return IMG_ERROR_INTERRUPTED;
        }
    }
    else
    {
        if (SYS_SemDecrement(&(pCam->sSemAvailable)) != IMG_SUCCESS)
        {
            CI_FATAL("waiting on the semaphore failed\n");
            return IMG_ERROR_INTERRUPTED;
        }
    }

    SYS_LockAcquire(&(pCam->sPendingLock));
    {
        sCell_T *pFound = NULL;

        pFound = List_popFront(&(pCam->sList_available));
        if (pFound)
        {
            pFrame = (KRN_DG_FRAME*)pFound->object;
            *pGivenShot = pFrame->identifier;

            ret = List_pushBack(&(pCam->sList_converting), pFound);
        }
    }
    SYS_LockRelease(&(pCam->sPendingLock));

    // cannot be NULL because we checked the semaphore beforehand
    IMG_ASSERT(pFrame != NULL);
    /*if ( pFrame == NULL )
    {
    CI_FATAL("Did not find an available frame\n");
    return IMG_ERROR_NOT_SUPPORTED;
    }*/
    // need to acquire the buffer as writable memory
    // we could update host memory but it's not needed
    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_CameraShoot(KRN_DG_CONNECTION *pConnection, int cameraId, int frameId, IMG_BOOL8 bTrigger)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    IMG_ASSERT(pConnection != NULL);

    if ((pCam = findCamera(pConnection, cameraId)) == NULL)
    {
        CI_FATAL("Camera is not registered\n");
        return IMG_ERROR_FATAL;
    }

    if (pCam->bStarted == IMG_FALSE)
    {
        CI_FATAL("Camera is not started!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if ((ret = KRN_DG_CameraShoot(pCam, frameId, bTrigger)) != IMG_SUCCESS)
    {
        CI_FATAL("Failed to shoot\n");
        return ret;
    }

    return IMG_SUCCESS;
}

IMG_RESULT INT_DG_CameraDeregister(KRN_DG_CONNECTION *pConnection, int iCameraId)
{
    KRN_DG_CAMERA *pCam = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    sCell_T *pFound = NULL;

    if (pConnection == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        if ((pFound = List_visitor(&(pConnection->sList_camera), (void*)&iCameraId, List_FindCamera)) != NULL)
        {
            pCam = (KRN_DG_CAMERA*)pFound->object;
            List_remove(pFound); // free the cell
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
