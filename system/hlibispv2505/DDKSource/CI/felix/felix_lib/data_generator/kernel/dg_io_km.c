/**
 ******************************************************************************
 @file dg_io_km.c

 @brief Implementation of the IO functions needed for the Data Generator
 kernel module

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

#include "ci_kernel/ci_kernel.h"  // CI_FATAL
#include "ci_internal/ci_errors.h"  // toErrno and toImgResult

#ifdef FELIX_FAKE

#include <errno.h>
/* in user mode debuge copy to and from "kernel" does not make a difference
 * copy_to_user and copy_from_user return the amount of memory still to be
 * copied: i.e. 0 is the success result */
#define copy_from_user(to, from, size) ( \
    IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define copy_to_user(to, from, size) ( \
    IMG_MEMCPY(to, from, size) == NULL ? size : 0 )

#else
#include <linux/errno.h>
#include <asm/uaccess.h>  // copy_to_user and copy_from_user
#endif

int DEV_DG_Open(struct inode *inode, struct file *flip)
{
    KRN_DG_CONNECTION *pConn = NULL;
    (void)inode;

    pConn = KRN_DG_ConnectionCreate();
    if (!pConn)
    {
        return toErrno(IMG_ERROR_FATAL);
    }

    flip->private_data = (void*)pConn;
    return 0;
}

int DEV_DG_Close(struct inode *inode, struct file *flip)
{
    IMG_RESULT ret = IMG_SUCCESS;
    (void)inode;  // not used at the moment

    IMG_ASSERT(flip->private_data);

    ret = KRN_DG_ConnectionDestroy((KRN_DG_CONNECTION*)flip->private_data);
    if (ret)
    {
        return toErrno(ret);
    }

    return 0;
}

long DEV_DG_Ioctl(struct file *flip, unsigned int ioctl_cmd,
    unsigned long ioctl_param)
{
    int ret = 0;
    int giveId = 0;
    IMG_RESULT imgRet = IMG_SUCCESS;
    KRN_DG_CONNECTION *pKrnConnection = NULL;

    // because user-side should never be able to call if it is NULL
    IMG_ASSERT(g_pDGDriver);
    IMG_ASSERT(flip);
    IMG_ASSERT(flip->private_data);
    pKrnConnection = (KRN_DG_CONNECTION*)flip->private_data;

    switch (ioctl_cmd)
    {
        /*
         * driver IOCTL
         */

    case DG_IOCTL_INFO:
        CI_DEBUG("ioctl 0x%X=DG_INFO\n", ioctl_cmd);
        imgRet = INT_DG_DriverGetInfo(pKrnConnection,
            (DG_HWINFO __user *)ioctl_param);
        if (imgRet)
        {
            ret = toErrno(imgRet);
        }
        break;

        /*
         * camera IOCTL
         */

    case DG_IOCTL_CAM_REG:  // register camera
        CI_DEBUG("ioctl 0x%X=CAM_REG\n", ioctl_cmd);
        imgRet = INT_DG_CameraRegister(pKrnConnection, &giveId);
        if (imgRet)
        {
            ret = toErrno(imgRet);
        }
        else
        {
            ret = giveId;
        }
        break;

    case DG_IOCTL_CAM_DEL:  // deregister
        CI_DEBUG("ioctl 0x%X=CAM_DEL\n", ioctl_cmd);
        imgRet = INT_DG_CameraDeregister(pKrnConnection, (int)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_ADD:  // add buffer
        CI_DEBUG("ioctl 0x%X=CAM_ADD\n", ioctl_cmd);

        imgRet = INT_DG_CameraAddBuffers(pKrnConnection,
            (struct DG_BUFFER_PARAM __user *)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_STA:  // start
        CI_DEBUG("ioctl 0x%X=CAM_STA\n", ioctl_cmd);
        imgRet = INT_DG_CameraStart(pKrnConnection,
            (const struct DG_CAMERA_PARAM __user *)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_STP:  // stop
        CI_DEBUG("ioctl 0x%X=CAM_STP\n", ioctl_cmd);
        imgRet = INT_DG_CameraStop(pKrnConnection, (int)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_BAQ:  // acquire a buffer before triggering a shot
        CI_DEBUG("ioctl 0x%X=CAM_BAQ\n", ioctl_cmd);
        imgRet = INT_DG_CameraAcquireBuffer(pKrnConnection,
            (struct DG_FRAME_PARAM __user *)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_BRE:
        CI_DEBUG("ioctl 0x%X=CAM_BRE\n", ioctl_cmd);
        imgRet = INT_DG_CameraShoot(pKrnConnection,
            (const struct DG_FRAME_PARAM __user *)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_WAI:
        CI_DEBUG("ioctl 0x%X=CAM_WAI\n", ioctl_cmd);
        imgRet = INT_DG_CameraWaitProcessed(pKrnConnection,
            (const struct DG_FRAME_WAIT __user *)ioctl_param);
        ret = toErrno(imgRet);
        break;

    case DG_IOCTL_CAM_BDE:
        CI_DEBUG("ioctl 0x%X=CAM_BDE\n", ioctl_cmd);
        imgRet = INT_DG_CameraDeleteBuffer(pKrnConnection,
            (const struct DG_BUFFER_PARAM __user *)ioctl_param);
        ret = toErrno(imgRet);
        break;

    default:
        CI_WARNING("Received unsupported IOCTL %u\n", ioctl_cmd);
        ret = -ENOTTY;
    }  // IOCTL switch

    CI_DEBUG("ioctl 0x%X returns %d\n", ioctl_cmd, ret);
    return ret;
}

static IMG_RESULT IMG_DG_map(struct vm_area_struct *vma, KRN_DG_FRAME *pShot)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_HANDLE hReg = NULL;

    IMG_ASSERT(g_pDGDriver);
    IMG_ASSERT(pShot);
    hReg = g_pDGDriver->hRegFelixDG[pShot->pParent->publicCamera.ui8Gasket];

    CI_PDUMP_COMMENT(hReg, "mapping shot to user-space");

    /* we map just after allocation there should be no need to flush the
     * cache just yet */
    // Platform_MemUpdateHost(&(pShot->sBuffer));
    ret = Platform_MemMapUser(&(pShot->sBuffer), vma);

    CI_PDUMP_COMMENT(hReg, "done");

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
        unsigned int id = vma->vm_pgoff;  // <<PAGE_SHIFT;
        pFound = List_visitor(&(pKrnConnection->sList_unmappedFrames),
            (void*)&id, &List_SearchFrame);
        if (!pFound)
        {
            CI_FATAL("Failed to find the unmapped frame\n");
        }
        else
        {
            KRN_DG_FRAME *pShot =
                container_of(pFound, KRN_DG_FRAME, sListCell);

            imgRet = IMG_DG_map(vma, pShot);

            if (IMG_SUCCESS == imgRet)
            {
                KRN_DG_FRAME *pFrame = container_of(pFound, KRN_DG_FRAME,
                    sListCell);
                CI_DEBUG("Mapping frame to user-space\n");
                imgRet = List_detach(pFound);
                IMG_ASSERT(IMG_SUCCESS == imgRet);

                if (pFrame->pParent->bStarted)
                {
                    struct MMUDirectory *pDir =
                        g_pDGDriver->sMMU.apDirectory[pFrame->pParent->\
                        publicCamera.ui8Gasket];
                    if (pDir)
                    {
                        KRN_DG_FrameMap(pFrame, pDir);

                        KRN_CI_MMUFlushCache(&(g_pDGDriver->sMMU),
                            pFrame->pParent->publicCamera.ui8Gasket,
                            IMG_FALSE);
                    }
                }

                SYS_LockAcquire(&(pShot->pParent->sLock));
                {
                    imgRet = List_pushBack(&(pShot->pParent->sList_available),
                        pFound);
                }
                SYS_LockRelease(&(pShot->pParent->sLock));
                IMG_ASSERT(IMG_SUCCESS == imgRet);
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
