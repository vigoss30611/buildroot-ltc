/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Description : Preprocessor setup
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "encpreprocess.h"
#include "enccommon.h"

/*------------------------------------------------------------------------------
  h265_EncPreProcessAlloc
------------------------------------------------------------------------------*/
i32 h265_EncPreProcessAlloc(preProcess_s *preProcess, i32 mbPerPicture)
{
  i32 status = ENCHW_OK;
  i32 i;

  for (i = 0; i < 3; i++)
  {
    preProcess->roiSegmentMap[i] = (u8 *)h2_EWLcalloc(mbPerPicture, sizeof(u8));
    if (preProcess->roiSegmentMap[i] == NULL) status = ENCHW_NOK;
  }

  for (i = 0; i < 2; i++)
  {
    preProcess->skinMap[i] = (u8 *)h2_EWLcalloc(mbPerPicture, sizeof(u8));
    if (preProcess->skinMap[i] == NULL) status = ENCHW_NOK;
  }

  preProcess->mvMap = (i32 *)h2_EWLcalloc(mbPerPicture, sizeof(i32));
  if (preProcess->mvMap == NULL) status = ENCHW_NOK;

  preProcess->scoreMap = (u8 *)h2_EWLcalloc(mbPerPicture, sizeof(u8));
  if (preProcess->scoreMap == NULL) status = ENCHW_NOK;

  if (status != ENCHW_OK)
  {
    h265_EncPreProcessFree(preProcess);
    return ENCHW_NOK;
  }

  return ENCHW_OK;
}

/*------------------------------------------------------------------------------
  h265_EncPreProcessFree
------------------------------------------------------------------------------*/
void h265_EncPreProcessFree(preProcess_s *preProcess)
{
  i32 i;

  for (i = 0; i < 3; i++)
  {
    if (preProcess->roiSegmentMap[i]) h2_EWLfree(preProcess->roiSegmentMap[i]);
    preProcess->roiSegmentMap[i] = NULL;
  }

  for (i = 0; i < 2; i++)
  {
    if (preProcess->skinMap[i]) h2_EWLfree(preProcess->skinMap[i]);
    preProcess->skinMap[i] = NULL;
  }

  if (preProcess->mvMap) h2_EWLfree(preProcess->mvMap);
  preProcess->mvMap = NULL;

  if (preProcess->scoreMap) h2_EWLfree(preProcess->scoreMap);
  preProcess->scoreMap = NULL;
}
/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/* Input format mapping API values to SW/HW register values. */
static const u32 inputFormatMapping[15] =
{
  ASIC_INPUT_YUV420PLANAR,
  ASIC_INPUT_YUV420SEMIPLANAR,
  ASIC_INPUT_YUV420SEMIPLANAR,
  ASIC_INPUT_YUYV422INTERLEAVED,
  ASIC_INPUT_UYVY422INTERLEAVED,
  ASIC_INPUT_RGB565,
  ASIC_INPUT_RGB565,
  ASIC_INPUT_RGB555,
  ASIC_INPUT_RGB555,
  ASIC_INPUT_RGB444,
  ASIC_INPUT_RGB444,
  ASIC_INPUT_RGB888,
  ASIC_INPUT_RGB888,
  ASIC_INPUT_RGB101010,
  ASIC_INPUT_RGB101010
};

static const u32 rgbMaskBits[15][3] =
{
  {  0,  0,  0 },
  {  0,  0,  0 },
  {  0,  0,  0 },
  {  0,  0,  0 },
  {  0,  0,  0 },
  { 15, 10,  4 }, /* RGB565 */
  {  4, 10, 15 }, /* BGR565 */
  { 14,  9,  4 }, /* RGB565 */
  {  4,  9, 14 }, /* BGR565 */
  { 11,  7,  3 }, /* RGB444 */
  {  3,  7, 11 }, /* BGR444 */
  { 23, 15,  7 }, /* RGB888 */
  {  7, 15, 23 }, /* BGR888 */
  { 29, 19,  9 }, /* RGB101010 */
  {  9, 19, 29 }  /* BGR101010 */
};

/*------------------------------------------------------------------------------

  h265_EncPreProcessCheck

  Check image size: Cropped frame _must_ fit inside of source image

  Input preProcess Pointer to preProcess_s structure.

  Return  ENCHW_OK  No errors.
    ENCHW_NOK Error condition.

------------------------------------------------------------------------------*/
i32 h265_EncPreProcessCheck(const preProcess_s *preProcess)
{
  i32 status = ENCHW_OK;
  u32 tmp;
  u32 width, height;

#if 0   /* These limits apply for input stride but camstab needs the
  actual pixels without padding. */
  u32 w_mask;

  /* Check width limits: lum&ch strides must be full 64-bit addresses */
  if (preProcess->inputFormat == 0)       /* YUV 420 planar */
    w_mask = 0x0F;                      /* 16 multiple */
  else if (preProcess->inputFormat <= 1)  /* YUV 420 semiplanar */
    w_mask = 0x07;                      /* 8 multiple  */
  else if (preProcess->inputFormat <= 9)  /* YUYV 422 or 16-bit RGB */
    w_mask = 0x03;                      /* 4 multiple  */
  else                                    /* 32-bit RGB */
    w_mask = 0x01;                      /* 2 multiple  */

  if (preProcess->lumWidthSrc & w_mask)
  {
    status = ENCHW_NOK;
  }
#endif

  if (preProcess->lumHeightSrc & 0x01)
  {
    status = ENCHW_NOK;
  }

  if (preProcess->lumWidthSrc > MAX_INPUT_IMAGE_WIDTH)
  {
    status = ENCHW_NOK;
  }

  width = preProcess->lumWidth;
  height = preProcess->lumHeight;
  if (preProcess->rotation)
  {
    u32 tmp;

    tmp = width;
    width = height;
    height = tmp;
  }

  /* Bottom right corner */
  tmp = MAX(preProcess->horOffsetSrc + width, preProcess->horOffsetSrc);
  if (tmp > preProcess->lumWidthSrc)
  {
    status = ENCHW_NOK;
  }

  tmp = MAX(preProcess->verOffsetSrc + height, preProcess->verOffsetSrc);
  if (tmp > preProcess->lumHeightSrc)
  {
    status = ENCHW_NOK;
  }

  return status;
}

static u32 GetAlignedStride(int width, i32 input_format)
{
#if 0
  // if YUV420 planar must support 32x width for input image.
  if (input_format == 0)
    return (width + 31) & (~31);
  else
    return (width + 15) & (~15);
#else
  // input luma width should be 16x samples.
  return (width + 15) & (~15);
#endif
}

/*------------------------------------------------------------------------------

  h265_EncPreProcess

  Preform cropping

  Input asic  Pointer to asicData_s structure
    preProcess Pointer to preProcess_s structure.

------------------------------------------------------------------------------*/
void h265_EncPreProcess(asicData_s *asic, preProcess_s *preProcess)
{
  u32 tmp;
  u32 width, height;
  regValues_s *regs;
  u32 stride;
  u32 horOffsetSrc = preProcess->horOffsetSrc;

  ASSERT(asic != NULL && preProcess != NULL);

  regs = &asic->regs;

  //stride = (preProcess->lumWidthSrc + 15) & (~15); /* 16 pixel multiple stride */
  stride = GetAlignedStride(preProcess->lumWidthSrc, preProcess->inputFormat);

  /* When input is interlaced frame containing two fields we read the frame
     with twice the width and crop the left(top) or right(bottom) field. */
  if (preProcess->interlacedFrame)
  {
    if (preProcess->bottomField) horOffsetSrc += stride;
    stride *= 2;
  }


  /* cropping */
  if (preProcess->inputFormat <= 2)   /* YUV 420 planar/semiplanar */
  {
    /* Input image position after crop and stabilization */
    tmp = preProcess->verOffsetSrc;
    tmp *= stride;
    tmp += horOffsetSrc;
    regs->inputLumBase += (tmp & (~15));
    regs->inputLumaBaseOffset = tmp & 15;

    /* Chroma */
    if (preProcess->inputFormat == 0)
    {
      tmp = preProcess->verOffsetSrc / 2;
      tmp *= stride / 2;
      tmp += horOffsetSrc / 2;
      if (((stride / 2) % 16) == 0)
      {
        regs->inputCbBase += (tmp & (~15));
        regs->inputCrBase += (tmp & (~15));
        regs->inputChromaBaseOffset = tmp & 15;
      }
      else
      {
        regs->inputCbBase += (tmp & (~7));
        regs->inputCrBase += (tmp & (~7));
        regs->inputChromaBaseOffset = tmp & 7;
      }
    }
    else
    {
      tmp = preProcess->verOffsetSrc / 2;
      tmp *= stride / 2;
      tmp += horOffsetSrc / 2;
      tmp *= 2;

      regs->inputCbBase += (tmp & (~15));
      regs->inputChromaBaseOffset = tmp & 15;
    }
  }
  else if (preProcess->inputFormat <= 10)   /* YUV 422 / RGB 16bpp */
  {
    /* Input image position after crop and stabilization */
    tmp = preProcess->verOffsetSrc;
    tmp *= stride;
    tmp += horOffsetSrc;
    tmp *= 2;

    regs->inputLumBase += (tmp & (~15));
    regs->inputLumaBaseOffset = tmp & 15;
    regs->inputChromaBaseOffset = (regs->inputLumaBaseOffset / 4) * 4;
  }
  else     /* RGB 32bpp */
  {
    /* Input image position after crop and stabilization */
    tmp = preProcess->verOffsetSrc;
    tmp *= stride;
    tmp += horOffsetSrc;
    tmp *= 4;

    regs->inputLumBase += (tmp & (~15));
    /* Note: HW does the cropping AFTER RGB to YUYV conversion
     * so the offset is calculated using 16bpp */
    regs->inputLumaBaseOffset = (tmp & 15);
    regs->inputChromaBaseOffset = (regs->inputLumaBaseOffset / 4) * 4;
  }

  /* YUV subsampling, map API values into HW reg values */
  regs->inputImageFormat = inputFormatMapping[preProcess->inputFormat];

  if (preProcess->inputFormat == 2)
    regs->chromaSwap = 1;

  regs->inputImageRotation = preProcess->rotation;

  /* source image setup, size and fill */
  width = preProcess->lumWidth;
  height = preProcess->lumHeight;

  /* Scaling ratio for down-scaling, fixed point 1.16, calculate from
      rotated dimensions. */
  if (preProcess->scaledWidth * preProcess->scaledHeight &&
      preProcess->scaledOutput)
  {
    u32 width16 = (width + 15) / 16 * 16;
    u32 height16 = (height + 15) / 16 * 16;

    regs->scaledWidth = preProcess->scaledWidth;
    regs->scaledHeight = preProcess->scaledHeight;
    if (preProcess->rotation)
    {
      /* Scaling factors calculated from overfilled dimensions. */
      regs->scaledWidthRatio =
        (u32)(preProcess->scaledWidth << 16) / width16 + 1;
      regs->scaledHeightRatio =
        (u32)(preProcess->scaledHeight << 16) / height16 + 1;
    }
    else
    {
      /* Scaling factors calculated from image pixels, round upwards. */
      regs->scaledWidthRatio =
        (u32)(preProcess->scaledWidth << 16) / width + 1;
      regs->scaledHeightRatio =
        (u32)(preProcess->scaledHeight << 16) / height + 1;
      /* Adjust horizontal scaling factor using overfilled dimensions
         to avoid problems in HW implementation with pixel leftover. */
      regs->scaledWidthRatio =
        ((regs->scaledWidthRatio * width16) & 0xFFF0000) / width16 + 1;
    }
    regs->scaledHeightRatio = MIN(65535, regs->scaledHeightRatio);
    regs->scaledWidthRatio = MIN(65535, regs->scaledWidthRatio);
  }
  else
  {
    regs->scaledWidth = regs->scaledHeight = 0;
    regs->scaledWidthRatio = regs->scaledHeightRatio = 0;
  }

  /* For rotated image, swap dimensions back to normal. */
  if (preProcess->rotation)
  {
    u32 tmp;

    tmp = width;
    width = height;
    height = tmp;
  }

  /* Set mandatory input parameters in asic structure */
  //regs->picWidth = (width + 7) / 8;
  //regs->picHeight = (height + 7) / 8;
  //asic->regs.picWidth = pic->sps->width_min_cbs;
  //asic->regs.picHeight = pic->sps->height_min_cbs;

  regs->pixelsOnRow = stride;

  /* Set the overfill values */
  if (width & 0x07)
    regs->xFill = (8 - (width & 0x07)) / 2;
  else
    regs->xFill = 0;

  if (height & 0x07)
    regs->yFill = 8 - (height & 0x07);
  else
    regs->yFill = 0;
#ifdef TRACE_PREPROCESS
  EncTracePreProcess(preProcess);
#endif

  return;
}

/*------------------------------------------------------------------------------

  h265_EncSetColorConversion

  Set color conversion coefficients and RGB input mask

  Input asic  Pointer to asicData_s structure
    preProcess Pointer to preProcess_s structure.

------------------------------------------------------------------------------*/
void h265_EncSetColorConversion(preProcess_s *preProcess, asicData_s *asic)
{
  regValues_s *regs;

  ASSERT(asic != NULL && preProcess != NULL);

  regs = &asic->regs;

  switch (preProcess->colorConversionType)
  {
    case 0:         /* BT.601 */
    default:
      /* Y  = 0.2989 R + 0.5866 G + 0.1145 B
       * Cb = 0.5647 (B - Y) + 128
       * Cr = 0.7132 (R - Y) + 128
       */
      preProcess->colorConversionType = 0;
      regs->colorConversionCoeffA = preProcess->colorConversionCoeffA = 19589;
      regs->colorConversionCoeffB = preProcess->colorConversionCoeffB = 38443;
      regs->colorConversionCoeffC = preProcess->colorConversionCoeffC = 7504;
      regs->colorConversionCoeffE = preProcess->colorConversionCoeffE = 37008;
      regs->colorConversionCoeffF = preProcess->colorConversionCoeffF = 46740;
      break;

    case 1:         /* BT.709 */
      /* Y  = 0.2126 R + 0.7152 G + 0.0722 B
       * Cb = 0.5389 (B - Y) + 128
       * Cr = 0.6350 (R - Y) + 128
       */
      regs->colorConversionCoeffA = preProcess->colorConversionCoeffA = 13933;
      regs->colorConversionCoeffB = preProcess->colorConversionCoeffB = 46871;
      regs->colorConversionCoeffC = preProcess->colorConversionCoeffC = 4732;
      regs->colorConversionCoeffE = preProcess->colorConversionCoeffE = 35317;
      regs->colorConversionCoeffF = preProcess->colorConversionCoeffF = 41615;
      break;

    case 2:         /* User defined */
      /* Limitations for coefficients: A+B+C <= 65536 */
      regs->colorConversionCoeffA = preProcess->colorConversionCoeffA;
      regs->colorConversionCoeffB = preProcess->colorConversionCoeffB;
      regs->colorConversionCoeffC = preProcess->colorConversionCoeffC;
      regs->colorConversionCoeffE = preProcess->colorConversionCoeffE;
      regs->colorConversionCoeffF = preProcess->colorConversionCoeffF;
  }

  /* Setup masks to separate R, G and B from RGB */
  regs->rMaskMsb = rgbMaskBits[preProcess->inputFormat][0];
  regs->gMaskMsb = rgbMaskBits[preProcess->inputFormat][1];
  regs->bMaskMsb = rgbMaskBits[preProcess->inputFormat][2];
}

