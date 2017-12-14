/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : ASIC low level controller
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/
#include "encpreprocess.h"
#include "encasiccontroller.h"
#include "enccommon.h"
#include "ewl.h"

/*------------------------------------------------------------------------------
    External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/* Mask fields */
#define mask_2b         (u32)0x00000003
#define mask_3b         (u32)0x00000007
#define mask_4b         (u32)0x0000000F
#define mask_5b         (u32)0x0000001F
#define mask_6b         (u32)0x0000003F
#define mask_11b        (u32)0x000007FF
#define mask_14b        (u32)0x00003FFF
#define mask_16b        (u32)0x0000FFFF

#define HSWREG(n)       ((n)*4)

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    EncAsicMemAlloc_V2

    Allocate HW/SW shared memory

    Input:
        asic        asicData structure
        width       width of encoded image, multiple of four
        height      height of encoded image
        type        ASIC_MPEG4 / ASIC_H263 / ASIC_JPEG
        numRefBuffs amount of reference luma frame buffers to be allocated

    Output:
        asic        base addresses point to allocated memory areas

    Return:
        ENCHW_OK        Success.
        ENCHW_NOK       Error: memory allocation failed, no memories allocated
                        and EWL instance released

------------------------------------------------------------------------------*/
i32 EncAsicMemAlloc_V2(asicData_s * asic, u32 width, u32 height,
                       u32 scaledWidth, u32 scaledHeight,
                       u32 encodingType, u32 numRefBuffsLum, u32 numRefBuffsChr)
{
    u32 mbTotal, i;
    regValues_s *regs;
    EWLLinearMem_t *buff = NULL;

    ASSERT(asic != NULL);
    ASSERT(width != 0);
    ASSERT(height != 0);
    ASSERT((height % 2) == 0);
    ASSERT((width % 4) == 0);
    ASSERT(sizeof(encOutputMbInfoDebug_s)%8 == 0);
    ASSERT(sizeof(encOutputMbInfo_s)%8 == 0);

    regs = &asic->regs;

    regs->codingType = encodingType;

    width = (width + 15) / 16;
    height = (height + 15) / 16;

    mbTotal = width * height;

    /* Allocate H.264 internal memories */
    if(regs->codingType != ASIC_JPEG)
    {
        /* The sizes of the memories */
        u32 internalImageLumaSize = mbTotal * (16 * 16);
        u32 internalImageChromaSize = mbTotal * (2 * 8 * 8);

        ASSERT((numRefBuffsLum >= 1) && (numRefBuffsLum <= ASIC_FRAME_BUF_LUM_MAX));
        ASSERT((numRefBuffsChr >= 2) && (numRefBuffsChr <= ASIC_FRAME_BUF_CHR_MAX));

        for (i = 0; i < numRefBuffsLum; i++)
        {
            if(EWLMallocRefFrm(asic->ewl, internalImageLumaSize,
                               &asic->internalImageLuma[i]) != EWL_OK)
            {
                EncAsicMemFree_V2(asic);
                return ENCHW_NOK;
            }
        }

        for (i = 0; i < numRefBuffsChr; i++)
        {
            if(EWLMallocRefFrm(asic->ewl, internalImageChromaSize,
                               &asic->internalImageChroma[i]) != EWL_OK)
            {
                EncAsicMemFree_V2(asic);
                return ENCHW_NOK;
            }
        }

        /* Set base addresses to the registers */
        regs->internalImageLumBaseW = asic->internalImageLuma[0].busAddress;
        regs->internalImageChrBaseW = asic->internalImageChroma[0].busAddress;
        if (numRefBuffsLum > 1)
            regs->internalImageLumBaseR[0] = asic->internalImageLuma[1].busAddress;
        else
            regs->internalImageLumBaseR[0] = asic->internalImageLuma[0].busAddress;
        regs->internalImageChrBaseR[0] = asic->internalImageChroma[1].busAddress;

        /* Optional scaled image output */
        if(scaledWidth*scaledHeight) {
            if (EWLMallocRefFrm(asic->ewl, scaledWidth*scaledHeight*2,
                           &asic->scaledImage) != EWL_OK)
            {
                EncAsicMemFree_V2(asic);
                return ENCHW_NOK;
            }
            regs->scaledLumBase = asic->scaledImage.busAddress;
        }

        /* NAL size table, table size must be 64-bit multiple,
         * space for SEI, MVC prefix, filler and zero at the end of table.
         * Atleast 1 macroblock row in every slice.
         * Also used for VP8 partitions. */
        buff = &asic->sizeTbl;
        asic->sizeTblSize = (sizeof(u32) * (height+4) + 7) & (~7);

        if(EWLMallocLinear(asic->ewl, asic->sizeTblSize, buff) != EWL_OK)
        {
            EncAsicMemFree_V2(asic);
            return ENCHW_NOK;
        }

        /* H264: CABAC context tables: all qps, intra+inter, 464 bytes/table.
         * VP8: The same table is used for probability tables, 1208 bytes. */
        if (regs->codingType == ASIC_VP8) i = 8*55+8*96;
        else i = 52*2*464;

        if(EWLMallocLinear(asic->ewl, i, &asic->cabacCtx) != EWL_OK)
        {
            EncAsicMemFree_V2(asic);
            return ENCHW_NOK;
        }
        regs->cabacCtxBase = asic->cabacCtx.busAddress;

        /* MV output table */
        if(EWLMallocLinear(asic->ewl, mbTotal*sizeof(encOutputMbInfo_s),
                          &asic->mvOutput) != EWL_OK)
        {
            EncAsicMemFree_V2(asic);
            return ENCHW_NOK;
        }
        regs->mvOutputBase = asic->mvOutput.busAddress;

        /* Clear mv output memory*/
        EWLmemset(asic->mvOutput.virtualAddress, 0, asic->mvOutput.size);

        if(regs->codingType == ASIC_VP8) {
            /* VP8: Table of counter for probability updates. */
            if(EWLMallocLinear(asic->ewl, ASIC_VP8_PROB_COUNT_SIZE,
                              &asic->probCount) != EWL_OK)
            {
                EncAsicMemFree_V2(asic);
                return ENCHW_NOK;
            }
            regs->probCountBase = asic->probCount.busAddress;

        }

        /* Segmentation map, 4 bits/mb, 64-bit multiple. */
        if(EWLMallocLinear(asic->ewl, (mbTotal*4 + 63)/64*8,
                          &asic->segmentMap) != EWL_OK)
        {
            EncAsicMemFree_V2(asic);
            return ENCHW_NOK;
        }
        regs->segmentMapBase = asic->segmentMap.busAddress;

        EWLmemset(asic->segmentMap.virtualAddress, 0,
                  asic->segmentMap.size);

    }

    return ENCHW_OK;
}

/*------------------------------------------------------------------------------

    EncAsicMemFree_V2

    Free HW/SW shared memory

------------------------------------------------------------------------------*/
void EncAsicMemFree_V2(asicData_s * asic)
{
    i32 i;

    ASSERT(asic != NULL);
    ASSERT(asic->ewl != NULL);

    for (i = 0; i < ASIC_FRAME_BUF_LUM_MAX; i++) {
        if(asic->internalImageLuma[i].virtualAddress != NULL)
            EWLFreeRefFrm(asic->ewl, &asic->internalImageLuma[i]);
        asic->internalImageLuma[i].virtualAddress = NULL;
    }

    for (i = 0; i < ASIC_FRAME_BUF_CHR_MAX; i++) {
        if(asic->internalImageChroma[i].virtualAddress != NULL)
            EWLFreeRefFrm(asic->ewl, &asic->internalImageChroma[i]);
        asic->internalImageChroma[i].virtualAddress = NULL;
    }

    if (asic->scaledImage.virtualAddress != NULL)
        EWLFreeRefFrm(asic->ewl, &asic->scaledImage);

    if(asic->sizeTbl.virtualAddress != NULL)
        EWLFreeLinear(asic->ewl, &asic->sizeTbl);

    if(asic->cabacCtx.virtualAddress != NULL)
        EWLFreeLinear(asic->ewl, &asic->cabacCtx);

    if(asic->mvOutput.virtualAddress != NULL)
        EWLFreeLinear(asic->ewl, &asic->mvOutput);

    if(asic->probCount.virtualAddress != NULL)
        EWLFreeLinear(asic->ewl, &asic->probCount);

    if(asic->segmentMap.virtualAddress != NULL)
        EWLFreeLinear(asic->ewl, &asic->segmentMap);

    asic->scaledImage.virtualAddress = NULL;
    asic->sizeTbl.virtualAddress = NULL;
    asic->cabacCtx.virtualAddress = NULL;
    asic->mvOutput.virtualAddress = NULL;
    asic->probCount.virtualAddress = NULL;
    asic->segmentMap.virtualAddress = NULL;
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
i32 EncAsicCheckStatus_V2(asicData_s * asic)
{
    i32 ret;
    u32 status;

    status = EncAsicGetStatus(asic->ewl);

    if(status & ASIC_STATUS_ERROR)
    {
        /* Get registers for debugging */
        EncAsicGetRegisters(asic->ewl, &asic->regs);

        ret = ASIC_STATUS_ERROR;

        EWLReleaseHw(asic->ewl);
    }
    else if(status & ASIC_STATUS_HW_TIMEOUT)
    {
        /* Get registers for debugging */
        EncAsicGetRegisters(asic->ewl, &asic->regs);

        ret = ASIC_STATUS_HW_TIMEOUT;

        EWLReleaseHw(asic->ewl);
    }
    else if(status & ASIC_STATUS_FRAME_READY)
    {
        EncAsicGetRegisters(asic->ewl, &asic->regs);

        ret = ASIC_STATUS_FRAME_READY;

        EWLReleaseHw(asic->ewl);
    }
    else if(status & ASIC_STATUS_BUFF_FULL)
    {
        ret = ASIC_STATUS_BUFF_FULL;
        /* ASIC doesn't support recovery from buffer full situation,
         * at the same time with buff full ASIC also resets itself. */
        EWLReleaseHw(asic->ewl);
    }
    else if(status & ASIC_STATUS_HW_RESET)
    {
        ret = ASIC_STATUS_HW_RESET;

        EWLReleaseHw(asic->ewl);
    }
    else if(status & ASIC_STATUS_FUSE)
    {
        ret = ASIC_STATUS_ERROR;

        EWLReleaseHw(asic->ewl);
    }
    else /* Don't check SLICE_READY status bit, it is reseted by IRQ handler */
    {
        /* IRQ received but none of the status bits is high, so it must be
         * slice ready. */
        ret = ASIC_STATUS_SLICE_READY;
    }

    return ret;
}
