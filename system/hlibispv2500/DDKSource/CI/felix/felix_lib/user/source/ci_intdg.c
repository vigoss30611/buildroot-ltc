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

#include "ci_internal/ci_errors.h" // toErrno and toImgResult
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

#endif // FELIX_FAKE

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

    if ( pframe->ui32FrameID == id )
    {
        return IMG_FALSE; // stop
    }
    return IMG_TRUE; // continue
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
        (long)&sFrameInfo); //INT_CI_DatagenFreeFrame()
    ret = toImgResult(ret);
    if ( ret != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to free device memory for Frame %d - returned %d\n",
            pIntFrame->ui32FrameID, ret);
    }

    List_detach(&(pIntFrame->sCell)); // access pIntDG->sListPendingFrames
    IMG_FREE(pIntFrame);

    return ret;
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

    if ( !ppDatagen || !pConnection )
    {
        LOG_ERROR("ppDatagen or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if ( *ppDatagen )
    {
        LOG_ERROR("datagen alread allocated\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    if ( (pConnection->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_IIF_DATAGEN)
        == 0 )
    {
        LOG_ERROR("Current HW does not support IIF Data Generator\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ui32DGID = -1; // invalid parameter - will ask for a registration
    ui32DGID = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_INDG_REG, ui32DGID); //INT_CI_DatagenRegister()

    if ( ui32DGID < 0 )
    {
        LOG_ERROR("Failed to get a unique DG identifier from the "\
            "kernel-module\n");
        return IMG_ERROR_FATAL;
    }

    pIntDG = (INT_INTDATAGEN*)IMG_CALLOC(1, sizeof(INT_INTDATAGEN));
    if ( !pIntDG )
    {
        LOG_ERROR("Failed to allocate INT_INTDATAGEN (%lu Bytes)\n",
            sizeof(INT_INTDATAGEN));
        return IMG_ERROR_MALLOC_FAILED;
    }

    pIntDG->pConnection = pIntCon;
    pIntDG->ui32DatagenID = ui32DGID;

    List_init(&(pIntDG->sListFrames));

    ret=List_pushBack((&pIntCon->sList_datagen), &(pIntDG->sCell));
    IMG_ASSERT(ret == IMG_SUCCESS);
// the insertion cannot fail, the cell is allocated and not attached to a list

    *ppDatagen = &(pIntDG->publicDatagen);

    return IMG_SUCCESS;
}

IMG_RESULT CI_DatagenDestroy(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    sCell_T *pCell = NULL;

    if ( !pDatagen )
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if ( pIntDG->bStarted )
    {
        ret = CI_DatagenStop(pDatagen);
        if ( ret != IMG_SUCCESS )
        {
            LOG_ERROR("Failed to stop a Datagen before destroying it\n");
            return ret;
        }
    }

    pCell = List_popFront(&(pIntDG->sListFrames));
    while ( IMG_SUCCESS == ret && pCell )
    {
        IMG_deleteFrame(pIntDG, (INT_DGFRAME*)pCell->object);
        pCell = List_popFront(&(pIntDG->sListFrames));
    }

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_REG,
        pIntDG->ui32DatagenID); //INT_CI_DatagenRegister()
    if ( ret < 0 )
    {
        LOG_ERROR("Failed to deregister datagenerator!\n");
        return toImgResult(ret);
    }

    // access to detach it pIntDG->pConnection->sList_datagen
    List_detach(&(pIntDG->sCell));

    IMG_FREE(pIntDG);

    return IMG_SUCCESS;
}

IMG_RESULT CI_DatagenStart(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    IMG_RESULT ret;
    struct CI_DG_PARAM sDGInfo;

    if ( !pDatagen )
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if ( pIntDG->bStarted )
    {
        LOG_WARNING("DG %d Already started\n", pIntDG->ui32DatagenID);
        return IMG_SUCCESS;
    }

    IMG_MEMSET(&sDGInfo, 0, sizeof(struct CI_DG_PARAM));
    sDGInfo.datagenId = pIntDG->ui32DatagenID;
    IMG_MEMCPY(&(sDGInfo.sDG), pDatagen, sizeof(CI_DATAGEN));

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_STA,
        (long)&sDGInfo); //INT_CI_DatagenStart()
    if ( 0 != ret )
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

    if ( !pDatagen )
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_FALSE;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_ISS,
        (long)pIntDG->ui32DatagenID); //INT_CI_DatagenIsStarted()
    if ( ret < 0 )
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

    if ( !pDatagen )
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if ( pIntDG->bStarted == IMG_FALSE )
    {
        LOG_WARNING("DG %d Already stopped\n", pIntDG->ui32DatagenID);
        return IMG_SUCCESS;
    }

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_STP,
        (long)pIntDG->ui32DatagenID);
    if ( 0 != ret )
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

    if ( !pDatagen )
    {
        LOG_ERROR("pDatagen is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    if ( pIntDG->bStarted )
    {
        LOG_WARNING("DG %d is started - cannot allocate more buffers\n",
            pIntDG->ui32DatagenID);
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    IMG_MEMSET(&sFrameInfo, 0, sizeof(struct CI_DG_FRAMEINFO));
    sFrameInfo.datagenId = pIntDG->ui32DatagenID;
    sFrameInfo.uiSize = ui32Size;

    pNewFrame = (INT_DGFRAME*)IMG_CALLOC(1, sizeof(INT_DGFRAME));
    if ( !pNewFrame )
    {
        LOG_ERROR("Failed to allocate internal frame object of %ld Bytes\n",
            sizeof(INT_DGFRAME));
        return IMG_ERROR_MALLOC_FAILED;
    }
    pNewFrame->sCell.object = pNewFrame;

    ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_ALL,
        (long)&sFrameInfo);
    if ( ret < 0 )
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
    if ( !pNewFrame->data )
    {
        LOG_ERROR("Failed to map a DG frame!\n");
        IMG_FREE(pNewFrame);
        return IMG_ERROR_FATAL;
    }

    ret = List_pushBack(&(pIntDG->sListFrames), &(pNewFrame->sCell));
    IMG_ASSERT(ret == IMG_SUCCESS);

    if ( pFrameID )
    {
        *pFrameID = pNewFrame->ui32FrameID;
    }

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
    if ( ret != IMG_SUCCESS )
    {
        if ( ui32FrameID > 0 )
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
    if ( !pFound ) // unlikely - means corruption
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

    if ( pFrame->publicFrame.ui32HorizontalBlanking < FELIX_MIN_H_BLANKING ||
         pFrame->publicFrame.ui32VerticalBlanking < FELIX_MIN_V_BLANKING)
    {
        LOG_ERROR("Cannot trigger a frame with blanking H=%u V=%u (min is" \
            " H=%u V=%u)\n",
            pFrame->publicFrame.ui32HorizontalBlanking,
            pFrame->publicFrame.ui32VerticalBlanking,
            FELIX_MIN_H_BLANKING, FELIX_MIN_V_BLANKING);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if ( pFrame->publicFrame.eFormat != pIntDG->publicDatagen.eFormat
        && bTrigger )
    {
        LOG_ERROR("Cannot trigger a frame of a different format!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    if ( pFrame->publicFrame.ui32Stride*pFrame->publicFrame.ui32Height >
        pFrame->ui32AllocSize && bTrigger )
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

    if ( pFound )
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

        if ( pFrame->publicFrame.ui32Stride%SYSMEM_ALIGNMENT != 0 )
        {
            LOG_ERROR("Memory stride has to be a multiple of SYSMEM_ALIGNMENT"\
                " (%d)\n", SYSMEM_ALIGNMENT);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        ret = SYS_IO_Control(pIntDG->pConnection->fileDesc, CI_IOCTL_INDG_TRG,
            (long)&sFrameInfo);
        ret = toImgResult(ret);
        if ( ret != IMG_SUCCESS )
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

CI_DG_FRAME* CI_DatagenGetAvailableFrame(CI_DATAGEN *pDatagen)
{
    INT_INTDATAGEN *pIntDG = NULL;
    sCell_T *pfound = NULL;

    if ( !pDatagen )
    {
        LOG_ERROR("pDatagen is NULL\n");
        return NULL;
    }

    pIntDG = container_of(pDatagen, INT_INTDATAGEN, publicDatagen);
    IMG_ASSERT(pIntDG);

    return IMG_DatagenGetFrame(pIntDG, 0);
}

CI_DG_FRAME* CI_DatagenGetFrame(CI_DATAGEN *pDatagen, IMG_UINT32 ui32FrameID)
{
    INT_INTDATAGEN *pIntDG = NULL;
    sCell_T *pFound = NULL;

    if ( !pDatagen )
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

    if ( !pFrame )
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

    if ( !pFrame )
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntFrame = container_of(pFrame, INT_DGFRAME, publicFrame);
    IMG_ASSERT(pIntFrame);

    return IMG_DatagenTrigger(pIntFrame, IMG_FALSE);
}

IMG_RESULT CI_DatagenDestroyFrame(CI_DG_FRAME *pFrame)
{
    INT_INTDATAGEN *pIntDG = NULL;
    INT_DGFRAME *pIntFrame = NULL;
    sCell_T *pFound = NULL;

    if ( !pFrame )
    {
        LOG_ERROR("pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntFrame = container_of(pFrame, INT_DGFRAME, publicFrame);
    IMG_ASSERT(pIntFrame);

    pIntDG = pIntFrame->pDatagen;
    IMG_ASSERT(pIntDG != NULL);

    // could use directly pFrame->sCell but we have to make sure it is a
    // pending frame (was acquired)!
    pFound = List_visitor(&(pIntDG->sListFrames), &(pIntFrame->ui32FrameID),
        &IMG_FindDGFrame);
    if ( pFound )
    {
        IMG_deleteFrame(pIntDG, pIntFrame);

        return IMG_SUCCESS;
    }

    LOG_ERROR("Given frame %d is not in the frame list!\n",
        pFrame->ui32FrameID);
    return IMG_ERROR_INVALID_PARAMETERS;
}

/*
 * Convert functions
 */

/**
 * @brief just check whether all pixels are in valid range, no real
 * functionality
 */
static void CheckPixelRange(const IMG_UINT16 *inPixels, IMG_UINT32 stride,
    IMG_UINT32 Npixels, IMG_UINT32 bitsPerPixel)
{
#ifndef NDEBUG
    IMG_UINT32 x;
    IMG_UINT32 maxpix = (1<<bitsPerPixel);
    for (x = 0; x < Npixels; x++)
    {
        IMG_UINT32 pix;
        pix = inPixels[x*stride];
        if (!(pix < maxpix))
        {
            LOG_WARNING("pixel %d value 0x%X out of %d-bit range (is input" \
                " image compatible with selected gasket?)\n",
                x, pix, bitsPerPixel);
        }
    }
#endif
}

IMG_SIZE IMG_Convert_Parallel10(const IMG_UINT16 *inPixels,
    IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    IMG_UINT32 *wr = NULL;
    IMG_SIZE Nwritten;

    //calculate number of bytes that will/would be written
    Nwritten = (NpixelsPerCh / 3) * 8; //8 output bytes per 3 input Bayer cells
    if ((NpixelsPerCh % 3) > 1) Nwritten += 8;
    else if (NpixelsPerCh % 3) Nwritten += 4;

    //actually convert the pixels
    if ( outPixels )
    {
        CheckPixelRange(inPixels, inStride, NpixelsPerCh, 10);
        CheckPixelRange(inPixels+1, inStride, NpixelsPerCh, 10);

        wr = (IMG_UINT32*)outPixels;

        for (;NpixelsPerCh >= 3;NpixelsPerCh -= 3)
        {
            *wr++ = (inPixels[0*inStride +0] & 0x3ff)
                | ((inPixels[0*inStride +1] & 0x3ff) << 10)
                | ((inPixels[1*inStride +0] & 0x3ff) << 20);
            *wr++ = (inPixels[1*inStride +1] & 0x3ff)
                | ((inPixels[2*inStride +0] & 0x3ff) << 10)
                | ((inPixels[2*inStride +1] & 0x3ff) << 20);
            inPixels += 3*inStride;
        }
        if (NpixelsPerCh > 0)
        {
            *wr++ = (inPixels[0*inStride +0] & 0x3ff)
                    | ((inPixels[0*inStride +1] & 0x3ff) << 10)
                    | ((NpixelsPerCh > 1) ?
                        ((inPixels[1*inStride +0] & 0x3ff) << 20) : 0);
            if (NpixelsPerCh > 1)
                *wr++ = (inPixels[1*inStride +1] & 0x3ff)
                        | ((NpixelsPerCh > 2) ?
                            ((inPixels[2*inStride +0] & 0x3ff) << 10) : 0)
                        | ((NpixelsPerCh > 2) ?
                            ((inPixels[2*inStride +1] & 0x3ff) << 20) : 0);
        }
        IMG_ASSERT(Nwritten == (IMG_UINT8*)wr - outPixels);
    }
    return Nwritten;
}

IMG_SIZE IMG_Convert_Parallel12(const IMG_UINT16 *inPixels,
    IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    IMG_UINT8 *wr = NULL;
    IMG_SIZE Nwritten;

    //calculate number of bytes that will/would be written
    Nwritten = NpixelsPerCh * 3; //3 output bytes per 1 input Bayer cell

    //actually convert the pixels
    if ( outPixels )
    {
        CheckPixelRange(inPixels, inStride, NpixelsPerCh, 12);
        CheckPixelRange(inPixels+1, inStride, NpixelsPerCh, 12);
        wr = outPixels;
        while (NpixelsPerCh--)
        {
            *wr++ = inPixels[0] & 0xff;
            *wr++ = (inPixels[0] >> 8) | ((inPixels[1] & 0xf) << 4);
            *wr++ = inPixels[1] >> 4;
            inPixels += inStride;
        }
        IMG_ASSERT(Nwritten == wr - outPixels);
    }
    return Nwritten;
}

IMG_RESULT CI_Convert_HDRInsertion(const struct _sSimImageIn_ *pImage,
    CI_BUFFER *pBuffer, IMG_BOOL8 bRescale)
{
    if (!pImage || !pBuffer)
    {
        LOG_ERROR("pImage or pBuffer is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (SimImage_RGB64 == pImage->info.eColourModel)
    {
        struct CI_SIZEINFO sizeInfo;
        PIXELTYPE pixelType;
        IMG_INT32 neededSize = 0;
        IMG_UINT16 *buffData = (IMG_UINT16*)pBuffer->data;
        unsigned i = 0, j = 0;
        unsigned inStride = 0, outStride = 0;
        int r = 2, g = 1, b = 0; // pixel orders
        IMG_RESULT ret;

        ret = PixelTransformRGB(&pixelType, BGR_161616_64);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to get information about %s",
                FormatString(BGR_161616_64));
            return IMG_ERROR_FATAL;
        }

        ret = CI_ALLOC_RGBSizeInfo(&pixelType, pImage->info.ui32Width,
            pImage->info.ui32Height, NULL, &sizeInfo);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to get allocation information about %s for "\
                "image size %dx%d",
                FormatString(BGR_161616_64), pImage->info.ui32Width,
                pImage->info.ui32Height);
            return IMG_ERROR_FATAL;
        }

        neededSize = sizeInfo.ui32Stride*sizeInfo.ui32Height;

        if ((IMG_UINT32)neededSize > pBuffer->ui32Size)
        {
            LOG_ERROR("a buffer of %u Bytes is too small - %u needed\n",
                pBuffer->ui32Size, neededSize);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
        if (pImage->info.ui8BitDepth > 16)
        {
            LOG_ERROR("unexpected large bitdepth of %d in input image!\n",
                pImage->info.ui8BitDepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        IMG_ASSERT(sizeInfo.ui32Stride % sizeof(IMG_UINT16) == 0);
        IMG_ASSERT(pImage->info.stride % sizeof(IMG_UINT16) == 0);
        outStride = sizeInfo.ui32Stride / sizeof(IMG_UINT16);
        inStride = pImage->info.stride / sizeof(IMG_UINT16);

        if (!pImage->info.isBGR)
        {
            r = 2;
            b = 0;
        }

        if (bRescale)
        {
            int shift = 16 - pImage->info.ui8BitDepth;
            int mask = (1 << (shift + 1))-1;

            for (j = 0; j < pImage->info.ui32Height; j++)
            {
                for (i = 0; i < pImage->info.ui32Width; i++)
                {
                    int out = j*outStride + i*4,
                        in = j*inStride + i*4;
                    buffData[out + 0] = (pImage->pBuffer[in + r]) << shift
                        | ((pImage->pBuffer[in + r])
                        >> (pImage->info.ui8BitDepth - shift))&mask;

                    buffData[out + 1] = (pImage->pBuffer[in + g]) << shift
                        | ((pImage->pBuffer[in + g])
                        >> (pImage->info.ui8BitDepth - shift))&mask;

                    buffData[out + 2] = (pImage->pBuffer[in + b]) << shift
                        | ((pImage->pBuffer[in + b])
                        >> (pImage->info.ui8BitDepth - shift))&mask;

                    buffData[out + 3] = 0; // blank space
                }
            }
        }
        else
        {
            for (j = 0; j < pImage->info.ui32Height; j++)
            {
                for (i = 0; i < pImage->info.ui32Width; i++)
                {
                    int out = j*outStride + i*4,
                        in = j*inStride + i*4;
                    buffData[out + 0] = pImage->pBuffer[in + r];

                    buffData[out + 1] = pImage->pBuffer[in + g];

                    buffData[out + 2] = (pImage->pBuffer[in + b]);

                    buffData[out + 3] = 0; // blank space
                }
            }
        }
    }
    else
    {
        LOG_ERROR("given pImage needs to be RGB\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

/*
 * Converter
 */

IMG_RESULT CI_ConverterConfigure(CI_DG_CONVERTER *pConverter,
    CI_CONV_FMT eFormat)
{
    if ( !pConverter )
    {
        LOG_ERROR("pConverter is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    switch(eFormat)
    {
    case CI_DGFMT_PARALLEL_10:
        pConverter->eFormat = eFormat;
        pConverter->pfnConverter = &IMG_Convert_Parallel10;
        break;

    case CI_DGFMT_PARALLEL_12:
        pConverter->eFormat = eFormat;
        pConverter->pfnConverter = &IMG_Convert_Parallel12;
        break;

    default:
        LOG_ERROR("Unsupported format %d\n", (int)eFormat);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_ConverterConvertFrame(CI_DG_CONVERTER *pConverter,
    const struct _sSimImageIn_ *pImage, CI_DG_FRAME *pFrame)
{
    IMG_UINT32 i = 0;
    IMG_UINT32 written, inputStride;
    IMG_UINT8 *pWrite = NULL;
    IMG_UINT32 ui32Stride = 0;

    if ( !pConverter || !pImage || !pFrame )
    {
        LOG_ERROR("pConverter or pImage or pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( !pConverter->pfnConverter ||
        pConverter->eFormat >= CI_DGFMT_MIPI || !pFrame->data )
    {
        LOG_ERROR("pConverted was not configured properly or pFrame has not" \
            " attached data\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    inputStride = pImage->info.ui32Width*sizeof(IMG_UINT16);
    ui32Stride = CI_ConverterFrameSize(pConverter, pImage->info.ui32Width, 1);
    // 1 line means stride
    pWrite = (IMG_UINT8*)pFrame->data;
    pFrame->eFormat = pConverter->eFormat;
    pFrame->ui32Width = pImage->info.ui32Width;
    pFrame->ui32Height = pImage->info.ui32Height;
    pFrame->ui32HorizontalBlanking = 0;
    pFrame->ui32VerticalBlanking = 0;
    pFrame->ui32Stride = ui32Stride;

    switch(pImage->info.eColourModel)
    {
    case SimImage_RGGB:
        pFrame->eBayerMosaic = MOSAIC_RGGB;
        break;
    case SimImage_GRBG:
        pFrame->eBayerMosaic = MOSAIC_GRBG;
        break;
    case SimImage_GBRG:
        pFrame->eBayerMosaic = MOSAIC_GBRG;
        break;
    case SimImage_BGGR:
        pFrame->eBayerMosaic = MOSAIC_BGGR;
        break;
    }

    if ( ui32Stride*pImage->info.ui32Height > pFrame->ui32AllocSize )
    {
        LOG_ERROR("given pFrame is too small (%d Bytes) to store converted" \
            " data (%dx%d = %d Bytes)\n",
            pFrame->ui32AllocSize, ui32Stride, pImage->info.ui32Height,
            ui32Stride*pImage->info.ui32Height
        );
        return IMG_ERROR_NOT_SUPPORTED;
    }
    IMG_MEMSET(pFrame->data, 0, pFrame->ui32AllocSize);

    for ( i = 0 ; i < pImage->info.ui32Height ; i++ )
    {
        written = pConverter->pfnConverter(
            &(pImage->pBuffer[i*pImage->info.ui32Width]), sizeof(IMG_UINT16),
            pImage->info.ui32Width/2, pWrite);

        if (written > ui32Stride)
        {
            LOG_ERROR("incorrect number of bytes written (%d) for line %d" \
                " (stride is %d)\n", written, i, ui32Stride);
            return IMG_ERROR_FATAL;
        }
        pWrite+=ui32Stride;

#ifdef DEBUG_FIRST_LINE
        if ( i == 0 )
        {
            FILE *f = fopen("dg_first.dat", "w");
            if ( f != NULL )
            {
                fwrite(pImage->pBuffer, inputStride, sizeof(IMG_UINT8), f);

                fclose(f);
            }
        }
#endif
    }

    return IMG_SUCCESS;
}

IMG_UINT32 CI_ConverterFrameSize(CI_DG_CONVERTER *pConverter,
    IMG_UINT32 ui32Width, IMG_UINT32 ui32Height)
{
    IMG_UINT32 ui32Size = 0;

    if ( !pConverter )
    {
        LOG_ERROR("pConverter is NULL\n");
        return 0;
    }

    if ( !pConverter->pfnConverter ||
        pConverter->eFormat >= CI_DGFMT_MIPI )
    {
        LOG_ERROR("pConverted was not configured properly or pFrame has not" \
            " attached data\n");
        return 0;
    }

    // get output stride - /2 because RG GB?
    ui32Size = pConverter->pfnConverter(NULL, 0, ui32Width/2, NULL);
    // round up to multiple of memory alignment
    ui32Size =
        (ui32Size+SYSMEM_ALIGNMENT-1)/SYSMEM_ALIGNMENT *SYSMEM_ALIGNMENT;
    ui32Size *= ui32Height;
    return ui32Size;
}
