/**
*******************************************************************************
@file ci_intdg.c

@brief Implementation of the Internal DG and format conversion in CI

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
#define LOG_TAG "CI_API"

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <sim_image.h>

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"

#ifdef IMG_KERNEL_MODULE
#error "this should not be in a kernel module"
#endif

#include "ci_internal/ci_errors.h"  // toErrno and toImgResult
#include "ci_kernel/ci_ioctrl.h"
#include "felixcommon/ci_alloc_info.h"

#ifdef WIN32
#ifndef MAP_FAILED
#define MAP_FAILED ((void *)-1)
#endif
#endif

#ifdef FELIX_FAKE

#include "ci_kernel/ci_connection.h"
// fake device
#include "sys/sys_userio_fake.h"

#endif /* FELIX_FAKE */

#include <felixcommon/userlog.h>

/*
 * local functions
 */

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL8 IMG_FindDGFrame(void *listBuffer, void *param)
{
    IMG_UINT32 id = *((IMG_UINT32*)param);
    INT_DGFRAME *pframe = (INT_DGFRAME*)listBuffer;

    if (pframe->ui32FrameID == id)
    {
        return IMG_FALSE;  // stop
    }
    return IMG_TRUE;  // continue
}

/**
 * @ingroup INT_FCT
 * @brief updates the public part of the frame from the private part (in case
 * modified by user)
 */
static void IMG_updateFrame(INT_DGFRAME *pFrame)
{
    pFrame->publicFrame.data = pFrame->data;
    pFrame->publicFrame.ui32AllocSize = pFrame->ui32AllocSize;
    pFrame->publicFrame.ui32FrameID = pFrame->ui32FrameID;
    pFrame->publicFrame.ui32HorizontalBlanking = FELIX_MIN_H_BLANKING;
    pFrame->publicFrame.ui32VerticalBlanking = FELIX_MIN_V_BLANKING;
    // other elements should be populated when conversion occurs
}

static IMG_RESULT IMG_deleteFrame(INT_INTDATAGEN *pIntDG,
    INT_DGFRAME *pIntFrame)
{
    struct CI_DG_FRAMEINFO sFrameInfo;
    IMG_RESULT ret;

    IMG_MEMSET(&sFrameInfo, 0, sizeof(struct CI_DG_FRAMEINFO));
    sFrameInfo.datagenId = pIntDG->ui32DatagenID;
    sFrameInfo.uiFrameId = pIntFrame->ui32FrameID;

    SYS_IO_MemUnmap(pIntDG->pConnection->fileDesc, pIntFrame->data,
        pIntFrame->ui32AllocSize);

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_FRE,
        (long)&sFrameInfo);
    ret = toImgResult(ret);
    if (ret)
    {
        LOG_ERROR("Failed to free device memory for Frame %d - returned %d\n",
            pIntFrame->ui32FrameID, ret);
    }

    List_detach(&(pIntFrame->sCell));  // access pIntDG->sListPendingFrames
    IMG_FREE(pIntFrame);

    return ret;
}

// ui32FrameID of 0 is invalid and means acquire 1st available
static CI_DG_FRAME* IMG_DatagenGetFrame(INT_INTDATAGEN *pDG,
    IMG_UINT32 ui32FrameID)
{
    struct CI_DG_FRAMEINFO sFrameInfo;
    INT_DGFRAME *pFrame = NULL;
    IMG_RESULT ret;
    sCell_T *pFound = NULL;

    IMG_MEMSET(&(sFrameInfo), 0, sizeof(struct CI_DG_FRAMEINFO));
    sFrameInfo.datagenId = pDG->ui32DatagenID;
    sFrameInfo.uiFrameId = ui32FrameID;

    ret = SYS_IO_Control(pDG->pConnection->fileDesc, CI_IOCTL_INDG_ACQ,
        (long)&sFrameInfo);
    ret = toImgResult(ret);
    if (ret)
    {
        if (ui32FrameID > 0)
        {
            LOG_ERROR("Failed to acquire DG Buffer %d\n", ui32FrameID);
        }
        else
        {
            LOG_ERROR("Available list is empty\n");
        }
        return NULL;
    }

    pFound = List_visitor(&(pDG->sListFrames), &(sFrameInfo.uiFrameId),
        &IMG_FindDGFrame);
    if (!pFound)  // unlikely - means corruption
    {
        LOG_ERROR("Failed to find the frame in the user-space list!\n");
        // @ we could release the frame in kernel-side
        return NULL;
    }

    pFrame = (INT_DGFRAME *)pFound->object;
    IMG_updateFrame(pFrame);

    return &(pFrame->publicFrame);
}

static IMG_RESULT IMG_DatagenTrigger(INT_DGFRAME *pFrame, IMG_BOOL8 bTrigger)
{
    INT_INTDATAGEN *pIntDG = NULL;
    sCell_T *pFound = NULL;
    IMG_RESULT ret;

    IMG_ASSERT(pFrame != NULL);

    pIntDG = pFrame->pDatagen;
    IMG_ASSERT(pIntDG != NULL);

    if ((pFrame->publicFrame.ui32HorizontalBlanking < FELIX_MIN_H_BLANKING ||
        pFrame->publicFrame.ui32VerticalBlanking < FELIX_MIN_V_BLANKING)
        && bTrigger)
    {
        LOG_ERROR("Cannot trigger a frame with blanking H=%u V=%u (min is" \
            " H=%u V=%u)\n",
            pFrame->publicFrame.ui32HorizontalBlanking,
            pFrame->publicFrame.ui32VerticalBlanking,
            FELIX_MIN_H_BLANKING, FELIX_MIN_V_BLANKING);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if ((pFrame->publicFrame.eFormat != pIntDG->publicDatagen.eFormat
        || pFrame->publicFrame.ui8FormatBitdepth
        != pIntDG->publicDatagen.ui8FormatBitdepth)
        && bTrigger)
    {
        LOG_ERROR("Cannot trigger a frame of a different format!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    if (pFrame->publicFrame.ui32Stride*pFrame->publicFrame.ui32Height >
        pFrame->ui32AllocSize && bTrigger)
    {
        LOG_ERROR("Cannot trigger a frame that has a stride*height (%dx%d=%d)"\
            " bigger than its allocated size %d\n",
            pFrame->publicFrame.ui32Stride, pFrame->publicFrame.ui32Height,
            pFrame->publicFrame.ui32Stride*pFrame->publicFrame.ui32Height,
            pFrame->ui32AllocSize);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFound = List_visitor(&(pIntDG->sListFrames), &(pFrame->ui32FrameID),
        &IMG_FindDGFrame);

    if (pFound)
    {
        struct CI_DG_FRAMETRIG sFrameInfo;

        IMG_MEMSET(&sFrameInfo, 0, sizeof(struct CI_DG_FRAMETRIG));

        sFrameInfo.datagenId = pIntDG->ui32DatagenID;
        sFrameInfo.uiFrameId = pFrame->ui32FrameID;
        sFrameInfo.bTrigger = bTrigger;
        sFrameInfo.uiStride = pFrame->publicFrame.ui32Stride;
        sFrameInfo.uiWidth = pFrame->publicFrame.ui32Width;
        sFrameInfo.uiHeight = pFrame->publicFrame.ui32Height;
        sFrameInfo.uiHorizontalBlanking =
            pFrame->publicFrame.ui32HorizontalBlanking;
        sFrameInfo.uiVerticalBlanking =
            pFrame->publicFrame.ui32VerticalBlanking;

        if (0 != pFrame->publicFrame.ui32Stride%SYSMEM_ALIGNMENT)
        {
            LOG_ERROR("Memory stride has to be a multiple of SYSMEM_ALIGNMENT"\
                " (%d)\n", SYSMEM_ALIGNMENT);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_TRG,
            (long)&sFrameInfo);
        ret = toImgResult(ret);
        if (ret)
        {
            LOG_ERROR("Failed to trigger the capture of frame %d\n",
                pFrame->ui32FrameID);
            return ret;
        }

        return IMG_SUCCESS;
    }

    LOG_ERROR("Given frame %d is not in the frame list!\n",
        pFrame->ui32FrameID);
    return IMG_ERROR_INVALID_PARAMETERS;
}

/*
 * Data Generator functions
 */

IMG_RESULT CI_DatagenCreate(CI_DATAGEN **ppDatagen, CI_CONNECTION *pConnection)
{
    INT_CONNECTION *pIntCon = NULL;
    INT_INTDATAGEN *pIntDG = NULL;
    int ui32DGID = 0;
    IMG_RESULT ret;

    if (!ppDatagen || !pConnection)
    {
        LOG_ERROR("ppDatagen or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (*ppDatagen)
    {
        LOG_ERROR("datagen alread allocated\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    if ((pConnection->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_IIF_DATAGEN)
        == 0)
    {
        LOG_ERROR("Current HW does not support IIF Data Generator\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    /* valgrind may complain that -1 is unaddressable memory but we don't
     * use it as an address so it is safe */
    ui32DGID = -1;  // invalid parameter - will ask for a registration
    ui32DGID = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_INDG_REG, ui32DGID);

    if (ui32DGID < 0)
    {
        LOG_ERROR("Failed to get a unique DG identifier from the "\
            "kernel-module\n");
        return IMG_ERROR_FATAL;
    }

    pIntDG = (INT_INTDATAGEN*)IMG_CALLOC(1, sizeof(INT_INTDATAGEN));
    if (!pIntDG)
    {
        LOG_ERROR("Failed to allocate INT_INTDATAGEN (%lu Bytes)\n",
            sizeof(INT_INTDATAGEN));
        return IMG_ERROR_MALLOC_FAILED;
    }

    pIntDG->pConnection = pIntCon;
    pIntDG->ui32DatagenID = ui32DGID;
    pIntDG->sCell.object = pIntDG;

    List_init(&(pIntDG->sListFrames));

    ret = List_pushBack((&pIntCon->sList_datagen), &(pIntDG->sCell));
    IMG_ASSERT(IMG_SUCCESS == ret);
    /* the insertion cannot fail, the cell is allocated and not attached
     * to a list */

    *ppDatagen = &(pIntDG->publicDatagen);

    return IMG_SUCCESS;
}

IMG_RESULT CI_DatagenDestroy(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    CI_DG_FRAME *pFrame = NULL;

    if (!pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if (pIntDG->bStarted)
    {
        ret = CI_DatagenStop(pDatagen);
        if (ret)
        {
            LOG_ERROR("Failed to stop a Datagen before destroying it\n");
            return ret;
        }
    }

    if (pIntDG->sListFrames.ui32Elements > 0)
    {
        pFrame = IMG_DatagenGetFrame(pIntDG, 0);
        while (pFrame)
        {
            INT_DGFRAME *pIntFrame = container_of(pFrame, INT_DGFRAME,
                publicFrame);
            IMG_deleteFrame(pIntDG, pIntFrame);

            // get next frame
            pFrame = IMG_DatagenGetFrame(pIntDG, 0);
        }
    }

    if (pIntDG->pConnection)
    {
        ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_REG,
            pIntDG->ui32DatagenID);
        if (ret < 0)
        {
            LOG_ERROR("Failed to deregister datagenerator!\n");
            return toImgResult(ret);
        }

        // access to detach it pIntDG->pConnection->sList_datagen
        List_detach(&(pIntDG->sCell));
    }

    IMG_FREE(pIntDG);

    return IMG_SUCCESS;
}

IMG_RESULT CI_DatagenStart(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    IMG_RESULT ret;
    struct CI_DG_PARAM sDGInfo;

    if (!pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if (pIntDG->bStarted)
    {
        LOG_WARNING("DG %d Already started\n", pIntDG->ui32DatagenID);
        return IMG_SUCCESS;
    }

    IMG_MEMSET(&sDGInfo, 0, sizeof(struct CI_DG_PARAM));
    sDGInfo.datagenId = pIntDG->ui32DatagenID;
    IMG_MEMCPY(&(sDGInfo.sDG), pDatagen, sizeof(CI_DATAGEN));

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_STA,
        (long)&sDGInfo);
    if (0 != ret)
    {
        LOG_ERROR("Failed to start internal DG %d to replace imager %d\n",
            pDatagen->ui8IIFDGIndex, pDatagen->ui8Gasket);
        ret = toImgResult(ret);
        return ret;
    }

    pIntDG->bStarted = IMG_TRUE;

    return IMG_SUCCESS;
}

IMG_BOOL8 CI_DatagenIsStarted(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    IMG_RESULT ret;

    if (!pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_FALSE;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_ISS,
        (long)pIntDG->ui32DatagenID);
    if (ret < 0)
    {
        ret = toImgResult(ret);
        LOG_ERROR("Failed to enquire if Datagen is started (returned %d)\n",
            ret);
        return IMG_FALSE;
    }

    pIntDG->bStarted = ret != 0 ? IMG_TRUE : IMG_FALSE;

    return pIntDG->bStarted;
}

IMG_RESULT CI_DatagenStop(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    IMG_RESULT ret;

    if (!pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if (pIntDG->bStarted == IMG_FALSE)
    {
        LOG_WARNING("DG %d Already stopped\n", pIntDG->ui32DatagenID);
        return IMG_SUCCESS;
    }

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_STP,
        (long)pIntDG->ui32DatagenID);
    if (0 != ret)
    {
        ret = toImgResult(ret);
        LOG_ERROR("Failed to stop Datagen\n");
        return ret;
    }

    pIntDG->bStarted = IMG_FALSE;

    return IMG_SUCCESS;
}

IMG_RESULT CI_DatagenAllocateFrame(CI_DATAGEN *pDatagen, IMG_UINT32 ui32Size,
    IMG_UINT32 *pFrameID)
{
    INT_INTDATAGEN *pIntDG = NULL;
    struct CI_DG_FRAMEINFO sFrameInfo;
    INT_DGFRAME *pNewFrame = NULL;
    IMG_RESULT ret;

    if (!pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if (pIntDG->bStarted)
    {
        LOG_WARNING("DG %d is started - cannot allocate more buffers\n",
            pIntDG->ui32DatagenID);
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    IMG_MEMSET(&sFrameInfo, 0, sizeof(struct CI_DG_FRAMEINFO));
    sFrameInfo.datagenId = pIntDG->ui32DatagenID;
    sFrameInfo.uiSize = ui32Size;

    pNewFrame = (INT_DGFRAME*)IMG_CALLOC(1, sizeof(INT_DGFRAME));
    if (!pNewFrame)
    {
        LOG_ERROR("Failed to allocate internal frame object of %ld Bytes\n",
            sizeof(INT_DGFRAME));
        return IMG_ERROR_MALLOC_FAILED;
    }
    pNewFrame->sCell.object = pNewFrame;

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_ALL,
        (long)&sFrameInfo);
    if (ret < 0)
    {
        LOG_ERROR("Failed to allocate a DG frame of %d Bytes\n", ui32Size);
        IMG_FREE(pNewFrame);
        return toImgResult(ret);
    }

    pNewFrame->ui32FrameID = sFrameInfo.mmapId;
    pNewFrame->ui32AllocSize = ui32Size;
    pNewFrame->pDatagen = pIntDG;
    pNewFrame->publicFrame.ui32HorizontalBlanking = FELIX_MIN_H_BLANKING;
    pNewFrame->publicFrame.ui32VerticalBlanking = FELIX_MIN_V_BLANKING;

    pNewFrame->data = SYS_IO_MemMap2(pIntDG->pConnection->fileDesc, ui32Size,
        PROT_WRITE, MAP_SHARED, pNewFrame->ui32FrameID);
    if (!pNewFrame->data)
    {
        LOG_ERROR("Failed to map a DG frame!\n");
        IMG_FREE(pNewFrame);
        return IMG_ERROR_FATAL;
    }

    ret = List_pushBack(&(pIntDG->sListFrames), &(pNewFrame->sCell));
    IMG_ASSERT(IMG_SUCCESS == ret);

    if (pFrameID)
    {
        *pFrameID = pNewFrame->ui32FrameID;
    }

    return ret;
}

CI_DG_FRAME* CI_DatagenGetAvailableFrame(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    sCell_T *pfound = NULL;

    if (!pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return NULL;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if (!pIntDG->pConnection)
    {
        LOG_ERROR("pDatagen does not have a connection!\n");
        return NULL;
    }

    return IMG_DatagenGetFrame(pIntDG, 0);
}

CI_DG_FRAME* CI_DatagenGetFrame(CI_DATAGEN *pDatagen, IMG_UINT32 ui32FrameID)
{
    INT_INTDATAGEN *pIntDG = NULL;
    sCell_T *pFound = NULL;

    if (!pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return NULL;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    return IMG_DatagenGetFrame(pIntDG, ui32FrameID);
}

IMG_RESULT CI_DatagenInsertFrame(CI_DG_FRAME *pFrame)
{
    INT_DGFRAME *pIntFrame = NULL;

    if (!pFrame)
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntFrame = container_of(pFrame, INT_DGFRAME, publicFrame);
    IMG_ASSERT(pIntFrame);

    return IMG_DatagenTrigger(pIntFrame, IMG_TRUE);
}

IMG_RESULT CI_DatagenReleaseFrame(CI_DG_FRAME *pFrame)
{
    INT_DGFRAME *pIntFrame = NULL;

    if (!pFrame)
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntFrame = container_of(pFrame, INT_DGFRAME, publicFrame);
    IMG_ASSERT(pIntFrame);

    return IMG_DatagenTrigger(pIntFrame, IMG_FALSE);
}

IMG_RESULT CI_DatagenWaitProcessedFrame(CI_DATAGEN *pDatagen,
    IMG_BOOL8 bBlocking)
{
    INT_INTDATAGEN *pIntDG = NULL;
    IMG_RESULT ret;
    struct CI_DG_FRAMEWAIT sParam;

    if (NULL == pDatagen)
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    IMG_MEMSET(&sParam, 0, sizeof(struct CI_DG_FRAMEWAIT));
    sParam.datagenId = pIntDG->ui32DatagenID;
    sParam.bBlocking = bBlocking;

#ifdef FELIX_FAKE
    /* when using fake interface the timeout in the kernel size is not
     * enough so we just continue trying */
    do
    {
        ret = SYS_IO_Control(pIntDG->pConnection->fileDesc,
            CI_IOCTL_INDG_WAI, (long)&sParam);
    } while (ret == -EINTR || (bBlocking && ret == -ETIME));
#else
    do
    {
        ret = SYS_IO_Control(pIntDG->pConnection->fileDesc,
            CI_IOCTL_INDG_WAI, (long)&sParam);
    } while (ret == -EINTR);
#endif
    ret = toImgResult(ret);

    if (ret && bBlocking)  // when not blocking it's not an error
    {
        LOG_ERROR("Failed to wait for processed frame\n");
    }
    return ret;
}

IMG_RESULT CI_DatagenDestroyFrame(CI_DG_FRAME *pFrame)
{
    INT_INTDATAGEN *pIntDG = NULL;
    INT_DGFRAME *pIntFrame = NULL;
    sCell_T *pFound = NULL;

    if (!pFrame)
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntFrame = container_of(pFrame, INT_DGFRAME, publicFrame);
    IMG_ASSERT(pIntFrame);

    pIntDG = pIntFrame->pDatagen;
    IMG_ASSERT(pIntDG);

    // could use directly pFrame->sCell but we have to make sure it is a
    // pending frame (was acquired)!
    pFound = List_visitor(&(pIntDG->sListFrames), &(pIntFrame->ui32FrameID),
        &IMG_FindDGFrame);
    if (pFound)
    {
        IMG_deleteFrame(pIntDG, pIntFrame);

        return IMG_SUCCESS;
    }

    LOG_ERROR("Given frame %d is not in the frame list!\n",
        pFrame->ui32FrameID);
    return IMG_ERROR_INVALID_PARAMETERS;
}
