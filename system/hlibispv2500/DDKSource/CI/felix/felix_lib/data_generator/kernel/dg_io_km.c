/**
 ******************************************************************************
 @file dg_io_km.c

 @brief Implementation of the IO functions needed for the Data Generator kernel module

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
#include "dg_kernel/dg_ioctl.h"

#include "ci_kernel/ci_kernel.h" // CI_FATAL
#include "ci_internal/ci_errors.h" // toErrno and toImgResult

#ifdef FELIX_FAKE

#include <errno.h>
// in user mode debuge copy to and from "kernel" does not make a difference
// copy_to_user and copy_from_user return the amount of memory still to be copied: i.e. 0 is the success result
#define copy_from_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define copy_to_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ? size : 0 )

#else
#include <linux/errno.h>
#include <asm/uaccess.h> // copy_to_user and copy_from_user
#endif

int DEV_DG_Open(struct inode *inode, struct file *flip)
{
    KRN_DG_CONNECTION *pConn = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    (void)inode;

    if ((ret = KRN_DG_ConnectionCreate(&pConn)) != IMG_SUCCESS)
    {
        return toErrno(ret);
    }

    flip->private_data = (void*)pConn;
    return 0;
}

int DEV_DG_Close(struct inode *inode, struct file *flip)
{
    IMG_RESULT ret = IMG_SUCCESS;
    (void)inode; // not used at the moment

    IMG_ASSERT(flip->private_data);

    if ((ret = KRN_DG_ConnectionDestroy((KRN_DG_CONNECTION*)flip->private_data)) != IMG_SUCCESS)
    {
        return toErrno(ret);
    }

    return 0;
}

long DEV_DG_Ioctl(struct file *flip, unsigned int ioctl_cmd, unsigned long ioctl_param)
{
    int ret = 0;
    int giveId = 0;
    IMG_RESULT imgRet = IMG_SUCCESS;
    KRN_DG_CONNECTION *pKrnConnection = NULL;

    IMG_ASSERT(g_pDGDriver); // because user-side should never be able to call if it is NULL
    IMG_ASSERT(flip);
    IMG_ASSERT(flip->private_data);
    pKrnConnection = (KRN_DG_CONNECTION*)flip->private_data;

    switch (ioctl_cmd)
    {
        // driver IOCTL

    case DG_IOCTL_INFO:
        CI_DEBUG("ioctl 0x%X=DG_INFO\n", ioctl_cmd);
        if (copy_to_user((void*)ioctl_param, &(g_pDGDriver->sHWInfo), sizeof(DG_HWINFO)) != 0)
        {
            ret = toErrno(IMG_ERROR_FATAL);
        }
        break;

        // camera IOCTL

    case DG_IOCTL_CAM_REG: // register camera
        CI_DEBUG("ioctl 0x%X=CAM_REG\n", ioctl_cmd);
        if ((imgRet = INT_DG_CameraRegister(pKrnConnection, (DG_CAMERA*)ioctl_param, &giveId)) == IMG_SUCCESS)
        {
            ret = giveId;
        }
        else
        {
            ret = toErrno(imgRet);
        }
        break;

    case DG_IOCTL_CAM_DEL: // deregister
        CI_DEBUG("ioctl 0x%X=CAM_DEL\n", ioctl_cmd);
        imgRet = INT_DG_CameraDeregister(pKrnConnection, (int)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_ADD: // add buffer
    {
        struct DG_CAMERA_PARAM sKrnParam, *pUser = NULL;
        pUser = (struct DG_CAMERA_PARAM*)ioctl_param;

        CI_DEBUG("ioctl 0x%X=CAM_ADD\n", ioctl_cmd);
        if (copy_from_user(&sKrnParam, pUser, sizeof(struct DG_CAMERA_PARAM)) != 0)
        {
            ret = toErrno(IMG_ERROR_FATAL);
        }

        // allocate kernel side arrays
        sKrnParam.aMemMapIds = (int*)IMG_CALLOC(sKrnParam.nbBuffers, sizeof(int));

        if ((imgRet = INT_DG_CameraAddBuffers(pKrnConnection, &sKrnParam)) != IMG_SUCCESS)
        {
            ret = toErrno(imgRet);
        }
        else
        {
            // copy the output strides
            if (copy_to_user(&(pUser->stride), &(sKrnParam.stride), sizeof(size_t)) != 0)
            {
                ret = toErrno(IMG_ERROR_FATAL);
            }

            // copy ids to user side
            if (copy_to_user(pUser->aMemMapIds, sKrnParam.aMemMapIds, sKrnParam.nbBuffers*sizeof(int)) != 0)
            {
                ret = toErrno(IMG_ERROR_FATAL);
            }
        }

        IMG_FREE(sKrnParam.aMemMapIds);
    }
    break;

    case DG_IOCTL_CAM_STA: // start
        CI_DEBUG("ioctl 0x%X=CAM_STA\n", ioctl_cmd);
        if ((imgRet = INT_DG_CameraStart(pKrnConnection, (int)ioctl_param)) != IMG_SUCCESS)
        {
            ret = toErrno(imgRet);
        }
        break;

    case DG_IOCTL_CAM_STP: // stop
        CI_DEBUG("ioctl 0x%X=CAM_STP\n", ioctl_cmd);
        if ((imgRet = INT_DG_CameraStop(pKrnConnection, (int)ioctl_param)) != IMG_SUCCESS)
        {
            ret = toErrno(imgRet);
        }
        break;

    case DG_IOCTL_CAM_BAQ: // acquire a buffer before triggering a shot
    {
        struct DG_FRAME_PARAM sKrnParam, *pUser = (struct DG_FRAME_PARAM*)ioctl_param;

        CI_DEBUG("ioctl 0x%X=CAM_BAQ\n", ioctl_cmd);
        if (copy_from_user(&sKrnParam, pUser, sizeof(struct DG_FRAME_PARAM)) != 0)
        {
            ret = toErrno(IMG_ERROR_FATAL);
        }

        if ((imgRet = INT_DG_CameraAcquireBuffer(pKrnConnection, sKrnParam.cameraId, sKrnParam.bOption, &(sKrnParam.frameId))) != IMG_SUCCESS)
        {
            ret = toErrno(imgRet);
        }
        else
        {
            if (copy_to_user(&(pUser->frameId), &(sKrnParam.frameId), sizeof(int)) != 0)
            {
                ret = toErrno(IMG_ERROR_FATAL);
            }
        }
    }
    break;

    case DG_IOCTL_CAM_BRE:
    {
        struct DG_FRAME_PARAM sKrnParam, *pUser = (struct DG_FRAME_PARAM*)ioctl_param;

        CI_DEBUG("ioctl 0x%X=CAM_BRE\n", ioctl_cmd);
        if (copy_from_user(&sKrnParam, pUser, sizeof(struct DG_FRAME_PARAM)) != 0)
        {
            ret = toErrno(IMG_ERROR_FATAL);
        }

        if ((imgRet = INT_DG_CameraShoot(pKrnConnection, sKrnParam.cameraId, sKrnParam.frameId, sKrnParam.bOption)) != IMG_SUCCESS)
        {
            ret = toErrno(imgRet);
        }
    }
    break;

    default:
        CI_WARNING("Received unsupported IOCTL %u\n", ioctl_cmd);
        ret = -ENOTTY;
    } // IOCTL switch

    CI_DEBUG("ioctl 0x%X returns %d\n", ioctl_cmd, ret);
    return ret;
}

static IMG_BOOL8 List_SearchFrame(void *listElem, void *searched)
{
    unsigned int *pId = (unsigned int*)searched;
    KRN_DG_FRAME *pFrame = (KRN_DG_FRAME*)listElem;

    if (pFrame->identifier == *pId)
    {
        return IMG_FALSE;
    }
    return IMG_TRUE;
}

static IMG_RESULT IMG_DG_map(struct vm_area_struct *vma, KRN_DG_FRAME *pShot)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_ASSERT(g_pDGDriver != NULL);

    CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[pShot->pParent->publicCamera.ui8DGContext], "mapping shot to user-space");

    //Platform_MemUpdateHost(&(pShot->sBuffer)); // we map just after allocation there should be no need to flush the cache just yet
    ret = Platform_MemMapUser(&(pShot->sBuffer), vma);

    CI_PDUMP_COMMENT(g_pDGDriver->hRegFelixDG[pShot->pParent->publicCamera.ui8DGContext], "done");

    return ret;
}

int DEV_DG_Mmap(struct file *flip, struct vm_area_struct *vma)
{
    IMG_RESULT imgRet = IMG_SUCCESS;
    KRN_DG_CONNECTION *pKrnConnection = NULL;
    sCell_T *pFound = NULL;

    IMG_ASSERT(flip);
    IMG_ASSERT(flip->private_data);
    pKrnConnection = (KRN_DG_CONNECTION*)flip->private_data;

    CI_DEBUG("DG MemMap of offset %ld", vma->vm_pgoff);
    SYS_LockAcquire(&(pKrnConnection->sLock));
    {
        unsigned int id = vma->vm_pgoff;//<<PAGE_SHIFT;
        if ((pFound = List_visitor(&(pKrnConnection->sList_unmappedFrames), (void*)&id, &List_SearchFrame)) == NULL)
        {
            CI_FATAL("Failed to find the unmapped frame\n");
        }
        else
        {
            KRN_DG_FRAME *pShot = (KRN_DG_FRAME*)pFound->object;

            imgRet = IMG_DG_map(vma, pShot);

            if (imgRet == IMG_SUCCESS)
            {
                CI_DEBUG("Mapping frame to user-space\n");
                imgRet = List_detach(pFound);
                IMG_ASSERT(imgRet == IMG_SUCCESS);

                SYS_LockAcquire(&(pShot->pParent->sPendingLock));
                {
                    imgRet = List_pushBack(&(pShot->pParent->sList_available), pFound);
                    SYS_SemIncrement(&(pShot->pParent->sSemAvailable));
                }
                SYS_LockRelease(&(pShot->pParent->sPendingLock));
                IMG_ASSERT(imgRet == IMG_SUCCESS);
            }
            else
            {
                CI_FATAL("IMG_DG_map failed!\n");
            }
        }
    }
    SYS_LockRelease(&(pKrnConnection->sLock));

    return toErrno(imgRet);
}
