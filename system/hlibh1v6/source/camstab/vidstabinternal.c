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
-
-  Description : Video stabilization standalone control
-
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "vidstabcommon.h"
#include "vidstabinternal.h"
#include "vidstabcfg.h"
#include "ewl.h"
#include "encswhwregisters.h"

#ifdef ASIC_WAVE_TRACE_TRIGGER
extern i32 trigger_point;    /* picture which will be traced */
#endif

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
    Function name   : VSCheckInput
    Description     : 
    Return type     : i32 
    Argument        : const VideoStbParam * param
------------------------------------------------------------------------------*/
i32 VSCheckInput(const VideoStbParam * param)
{
    /* Input picture minimum dimensions */
    if((param->inputWidth < 104) || (param->inputHeight < 104))
        return 1;

    /* Stabilized picture minimum  values */
    if((param->stabilizedWidth < 96) || (param->stabilizedHeight < 96))
        return 1;

    /* Stabilized dimensions multiple of 4 */
    if(((param->stabilizedWidth & 3) != 0) ||
       ((param->stabilizedHeight & 3) != 0))
        return 1;

    /* Edge >= 4 pixels */
    if((param->inputWidth < (param->stabilizedWidth + 8)) ||
       (param->inputHeight < (param->stabilizedHeight + 8)))
        return 1;

    /* stride 8 multiple */
    if((param->stride < param->inputWidth) || (param->stride & 7) != 0)
        return 1;

    /* input format */
    if(param->format > VIDEOSTB_BGR101010)
    {
        return 1;
    }

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : VSInitAsicCtrl
    Description     : 
    Return type     : void 
    Argument        : VideoStb * pVidStab
------------------------------------------------------------------------------*/
void VSInitAsicCtrl(VideoStb * pVidStab)
{
    RegValues *val = &pVidStab->regval;
    u32 *regMirror = pVidStab->regMirror;

    ASSERT(pVidStab != NULL);

    /* Initialize default values from defined configuration */
    val->asicCfgReg = 
        ((VSH1_AXI_WRITE_ID & (255)) << 24) |
        ((VSH1_AXI_READ_ID & (255)) << 16) |
        ((VSH1_BURST_LENGTH & (63)) << 8) |
        ((VSH1_BURST_INCR_TYPE_ENABLED & (1)) << 6) |
        ((VSH1_BURST_DATA_DISCARD_ENABLED & (1)) << 5) |
        ((VSH1_ASIC_CLOCK_GATING_ENABLED & (1)) << 4);

    val->irqDisable = VSH1_IRQ_DISABLE;
    val->rwStabMode = ASIC_VS_MODE_ALONE;

    (void) EWLmemset(regMirror, 0, sizeof(regMirror));
}

/*------------------------------------------------------------------------------
    Function name   : VSSetupAsicAll
    Description     : 
    Return type     : void 
    Argument        : VideoStb * pVidStab
------------------------------------------------------------------------------*/
void VSSetupAsicAll(VideoStb * pVidStab)
{

    const void *ewl = pVidStab->ewl;
    RegValues *val = &pVidStab->regval;
    u32 *regMirror = pVidStab->regMirror;

    regMirror[1] = ((val->irqDisable & 1) << 1);    /* clear IRQ status */
    regMirror[14] = 0;                              /* clear enable */

    /* system configuration */
    if (val->inputImageFormat < ASIC_INPUT_RGB565)      /* YUV input */
        regMirror[2] = val->asicCfgReg |
            ((VSH1_INPUT_SWAP_16_YUV & (1)) << 14) |
            ((VSH1_INPUT_SWAP_32_YUV & (1)) << 2) |
            (VSH1_INPUT_SWAP_8_YUV & (1));
    else if (val->inputImageFormat < ASIC_INPUT_RGB888) /* 16-bit RGB input */
        regMirror[2] = val->asicCfgReg |
            ((VSH1_INPUT_SWAP_16_RGB16 & (1)) << 14) |
            ((VSH1_INPUT_SWAP_32_RGB16 & (1)) << 2) |
            (VSH1_INPUT_SWAP_8_RGB16 & (1));
    else                                                /* 32-bit RGB input */
        regMirror[2] = val->asicCfgReg |
            ((VSH1_INPUT_SWAP_16_RGB32 & (1)) << 14) |
            ((VSH1_INPUT_SWAP_32_RGB32 & (1)) << 2) |
            (VSH1_INPUT_SWAP_8_RGB32 & (1));

    /* Input picture buffers */
    EncAsicSetRegisterValue(regMirror, HEncBaseInLum, val->inputLumBase);

    /* Common control register, use INTRA mode */
    EncAsicSetRegisterValue(regMirror, HEncWidth, val->mbsInRow);
    EncAsicSetRegisterValue(regMirror, HEncHeight, val->mbsInCol);
    EncAsicSetRegisterValue(regMirror, HEncPictureType, 1);

    /* PreP control */
    EncAsicSetRegisterValue(regMirror, HEncLumOffset, val->inputLumaBaseOffset);
    EncAsicSetRegisterValue(regMirror, HEncRowLength, val->pixelsOnRow);
    EncAsicSetRegisterValue(regMirror, HEncXFill, val->xFill);
    EncAsicSetRegisterValue(regMirror, HEncYFill, val->yFill);
    EncAsicSetRegisterValue(regMirror, HEncInputFormat, val->inputImageFormat);

    EncAsicSetRegisterValue(regMirror, HEncBaseNextLum, val->rwNextLumaBase);
    EncAsicSetRegisterValue(regMirror, HEncStabMode, val->rwStabMode);

    EncAsicSetRegisterValue(regMirror, HEncRGBCoeffA,
                            val->colorConversionCoeffA & mask_16b);
    EncAsicSetRegisterValue(regMirror, HEncRGBCoeffB,
                            val->colorConversionCoeffB & mask_16b);
    EncAsicSetRegisterValue(regMirror, HEncRGBCoeffC,
                            val->colorConversionCoeffC & mask_16b);
    EncAsicSetRegisterValue(regMirror, HEncRGBCoeffE,
                            val->colorConversionCoeffE & mask_16b);
    EncAsicSetRegisterValue(regMirror, HEncRGBCoeffF,
                            val->colorConversionCoeffF & mask_16b);

    EncAsicSetRegisterValue(regMirror, HEncRMaskMSB, val->rMaskMsb & mask_5b);
    EncAsicSetRegisterValue(regMirror, HEncGMaskMSB, val->gMaskMsb & mask_5b);
    EncAsicSetRegisterValue(regMirror, HEncBMaskMSB, val->bMaskMsb & mask_5b);

#ifdef ASIC_WAVE_TRACE_TRIGGER
    if(val->vop_count++ == trigger_point)
    {
        /* logic analyzer triggered by writing to the ID reg */
        EWLWriteReg(ewl, 0x00, ~0);
    }
#endif

    {
        i32 i;

        for(i = 1; i <= 63; i++)
        {
            /* Write all regs except 0xA0 with standalone stab enable */
            if(i != 40)
                EWLWriteReg(ewl, HSWREG(i), regMirror[i]);
        }
    }

#ifdef TRACE_REGS
    EncTraceRegs(ewl, 0, 0);
#endif

    /* Register with enable bit is written last */
    EWLWriteReg(ewl, HSWREG(40), regMirror[40]);
    regMirror[14] |= ASIC_STATUS_ENABLE;
    EWLEnableHW(ewl, HSWREG(14), regMirror[14]);
}

/*------------------------------------------------------------------------------
    Function name   : CheckAsicStatus
    Description     : 
    Return type     : i32 
    Argument        : u32 status
------------------------------------------------------------------------------*/
i32 CheckAsicStatus(u32 status)
{
    i32 ret;

    if(status & ASIC_STATUS_HW_RESET)
    {
        ret = VIDEOSTB_HW_RESET;
    }
    else if(status & ASIC_STATUS_FRAME_READY)
    {
        ret = VIDEOSTB_OK;
    }
    else
    {
        ret = VIDEOSTB_HW_BUS_ERROR;
    }

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : VSWaitAsicReady
    Description     : 
    Return type     : i32 
    Argument        : VideoStb * pVidStab
------------------------------------------------------------------------------*/
i32 VSWaitAsicReady(VideoStb * pVidStab)
{
    const void *ewl = pVidStab->ewl;
    u32 *regMirror = pVidStab->regMirror;
    i32 ret;

    /* Wait for IRQ */
    ret = EWLWaitHwRdy(ewl, NULL);

    if(ret != EWL_OK)
    {
        if(ret == EWL_ERROR)
        {
            /* IRQ error => Stop and release HW */
            ret = VIDEOSTB_SYSTEM_ERROR;
        }
        else    /*if(ewl_ret == EWL_HW_WAIT_TIMEOUT) */
        {
            /* IRQ Timeout => Stop and release HW */
            ret = VIDEOSTB_HW_TIMEOUT;
        }

        EWLWriteReg(ewl, HSWREG(40), 0);
        EWLDisableHW(ewl, HSWREG(14), 0);       /* make sure ASIC is OFF */
    }
    else
    {
        i32 i;

        regMirror[1] = EWLReadReg(ewl, HSWREG(1));  /* IRQ status */
        for(i = 40; i <= 50; i++)
        {
            regMirror[i] = EWLReadReg(ewl, HSWREG(i));  /* VS results */
        }

        ret = CheckAsicStatus(regMirror[1]);
    }

#ifdef TRACE_REGS
    EncTraceRegs(ewl, 1, 0);
#endif

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : VSSetCropping
    Description     : 
    Return type     : void 
    Argument        : VideoStb * pVidStab
    Argument        : u32 currentPictBus
    Argument        : u32 nextPictBus
------------------------------------------------------------------------------*/
void VSSetCropping(VideoStb * pVidStab, u32 currentPictBus, u32 nextPictBus)
{
    u32 byteOffsetCurrent;
    u32 width, height;
    RegValues *regs;

    ASSERT(pVidStab != NULL && currentPictBus != 0 && nextPictBus != 0);

    regs = &pVidStab->regval;

    regs->inputLumBase = currentPictBus;
    regs->rwNextLumaBase = nextPictBus;

    /* RGB conversion coefficients for RGB input */
    if (pVidStab->yuvFormat >= 4) {
        regs->colorConversionCoeffA = 19589;
        regs->colorConversionCoeffB = 38443;
        regs->colorConversionCoeffC = 7504;
        regs->colorConversionCoeffE = 37008;
        regs->colorConversionCoeffF = 46740;
    }

    /* Setup masks to separate R, G and B from RGB */
    switch ((i32)pVidStab->yuvFormat)
    {
        case 4: /* RGB565 */
            regs->rMaskMsb = 15;
            regs->gMaskMsb = 10;
            regs->bMaskMsb = 4;
            break;
        case 5: /* BGR565 */
            regs->bMaskMsb = 15;
            regs->gMaskMsb = 10;
            regs->rMaskMsb = 4;
            break;
        case 6: /* RGB555 */
            regs->rMaskMsb = 14;
            regs->gMaskMsb = 9;
            regs->bMaskMsb = 4;
            break;
        case 7: /* BGR555 */
            regs->bMaskMsb = 14;
            regs->gMaskMsb = 9;
            regs->rMaskMsb = 4;
            break;
        case 8: /* RGB444 */
            regs->rMaskMsb = 11;
            regs->gMaskMsb = 7;
            regs->bMaskMsb = 3;
            break;
        case 9: /* BGR444 */
            regs->bMaskMsb = 11;
            regs->gMaskMsb = 7;
            regs->rMaskMsb = 3;
            break;
        case 10: /* RGB888 */
            regs->rMaskMsb = 23;
            regs->gMaskMsb = 15;
            regs->bMaskMsb = 7;
            break;
        case 11: /* BGR888 */
            regs->bMaskMsb = 23;
            regs->gMaskMsb = 15;
            regs->rMaskMsb = 7;
            break;
        case 12: /* RGB101010 */
            regs->rMaskMsb = 29;
            regs->gMaskMsb = 19;
            regs->bMaskMsb = 9;
            break;
        case 13: /* BGR101010 */
            regs->bMaskMsb = 29;
            regs->gMaskMsb = 19;
            regs->rMaskMsb = 9;
            break;
        default:
            /* No masks needed for YUV format */
            regs->rMaskMsb = regs->gMaskMsb = regs->bMaskMsb = 0;
    }

    if (pVidStab->yuvFormat <= 3)
        regs->inputImageFormat = pVidStab->yuvFormat;       /* YUV */
    else if (pVidStab->yuvFormat <= 5)
        regs->inputImageFormat = ASIC_INPUT_RGB565;         /* 16-bit RGB */
    else if (pVidStab->yuvFormat <= 7)
        regs->inputImageFormat = ASIC_INPUT_RGB555;         /* 15-bit RGB */
    else if (pVidStab->yuvFormat <= 9)
        regs->inputImageFormat = ASIC_INPUT_RGB444;         /* 12-bit RGB */
    else if (pVidStab->yuvFormat <= 11)
        regs->inputImageFormat = ASIC_INPUT_RGB888;         /* 24-bit RGB */
    else
        regs->inputImageFormat = ASIC_INPUT_RGB101010;      /* 30-bit RGB */


    regs->pixelsOnRow = pVidStab->stride;

    /* cropping */

    /* Current image position */
    byteOffsetCurrent = pVidStab->data.stabOffsetY;
    byteOffsetCurrent *= pVidStab->stride;
    byteOffsetCurrent += pVidStab->data.stabOffsetX;

    if(pVidStab->yuvFormat >=2 && pVidStab->yuvFormat <= 9)    /* YUV 422 / RGB 16bpp */
    {
        byteOffsetCurrent *= 2;
    }
    else if(pVidStab->yuvFormat > 9)    /* RGB 32bpp */
    {
        byteOffsetCurrent *= 4;
    }

    regs->inputLumBase += (byteOffsetCurrent & (~7));
    if(pVidStab->yuvFormat >= 10)
    {
        /* Note: HW does the cropping AFTER RGB to YUYV conversion
         * so the offset is calculated using 16bpp */
        regs->inputLumaBaseOffset = (byteOffsetCurrent & 7)/2;
    }
    else
    {
        regs->inputLumaBaseOffset = (byteOffsetCurrent & 7);
    }

    /* next picture's offset same as above */
    regs->rwNextLumaBase += (byteOffsetCurrent & (~7));

    /* source image setup, size and fill */
    width = pVidStab->data.stabilizedWidth;
    height = pVidStab->data.stabilizedHeight;

    /* Set stabilized picture dimensions */
    regs->mbsInRow = (width + 15) / 16;
    regs->mbsInCol = (height + 15) / 16;

    /* Set the overfill values */
    if(width & 0x0F)
        regs->xFill = (16 - (width & 0x0F)) / 4;
    else
        regs->xFill = 0;

    if(height & 0x0F)
        regs->yFill = 16 - (height & 0x0F);
    else
        regs->yFill = 0;

    return;
}
