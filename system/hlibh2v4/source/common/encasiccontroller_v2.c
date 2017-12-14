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
        numRefBuffs amount of reference luma frame buffers to be allocated

    Output:
        asic        base addresses point to allocated memory areas

    Return:
        ENCHW_OK        Success.
        ENCHW_NOK       Error: memory allocation failed, no memories allocated
                        and EWL instance released

------------------------------------------------------------------------------*/
i32 EncAsicMemAlloc_V2(asicData_s *asic, u32 width, u32 height,
                       u32 scaledWidth, u32 scaledHeight,
                       u32 encodingType, u32 numRefBuffsLum, u32 numRefBuffsChr, u32 compressor,u32 bitDepthLuma,u32 bitDepthChroma)
{

  (void)numRefBuffsChr;
  u32  i;
  regValues_s *regs;
  EWLLinearMem_t *buff = NULL;
  u32 internalImageLumaSize, internalImageLumaSize_xn, width_xn, height_xn;
  u32 total_allocated_buffers;
  u32 width_4n, height_4n;
  u32 block_unit,block_size;

  ASSERT(asic != NULL);
  ASSERT(width != 0);
  ASSERT(height != 0);
  ASSERT((height % 2) == 0);
  ASSERT((width % 2) == 0);
  //scaledWidth=((scaledWidth+7)/8)*8;
  regs = &asic->regs;

  regs->codingType = encodingType;
  ASSERT(numRefBuffsLum < ASIC_FRAME_BUF_LUM_MAX);
  total_allocated_buffers = numRefBuffsLum + 1;

  width = ((width + 63) >> 6) << 6;
  height = (((height + 63) >> 6) << 6);

  //width_xn=width+PADDING_LENGTH;
  //height_xn=height+PADDING_LENGTH;

  width_4n = ((width + 15) / 16) * 4;
  height_4n = ((height + 15) / 16) * 4;

  internalImageLumaSize = (width * height + width_4n * height_4n)+(width * height + width_4n * height_4n)*(bitDepthLuma-8)/8;
  asic->regs.recon_chroma_half_size = internalImageLumaSize / 4;
  {


    for (i = 0; i < total_allocated_buffers; i++)
    {
      if (EWLMallocRefFrm(asic->ewl, internalImageLumaSize,
                          &asic->internalreconLuma[i]) != EWL_OK)
      {
        EncAsicMemFree_V2(asic);
        return ENCHW_NOK;
      }
      asic->internalreconLuma_4n[i].busAddress = asic->internalreconLuma[i].busAddress + width * height+width * height*(bitDepthLuma-8)/8;
      asic->internalreconLuma_4n[i].virtualAddress = asic->internalreconLuma[i].virtualAddress + width * height+width * height*(bitDepthLuma-8)/8;
      asic->internalreconLuma_4n[i].size = width_4n * height_4n+width_4n * height_4n*(bitDepthLuma-8)/8;

    }

    for (i = 0; i < total_allocated_buffers; i++)
    {
      if (EWLMallocRefFrm(asic->ewl, internalImageLumaSize / 2,
                          &asic->internalreconChroma[i]) != EWL_OK)
      {
        EncAsicMemFree_V2(asic);
        return ENCHW_NOK;
      }
    }

    /*  VP9: The table is used for probability tables, 1208 bytes. */
    if (regs->codingType == ASIC_VP9) 
    {
      i = 8 * 55 + 8 * 96;
      if (EWLMallocLinear(asic->ewl, i, &asic->cabacCtx) != EWL_OK)
      {
        EncAsicMemFree_V2(asic);
        return ENCHW_NOK;
      }
    }

    regs->cabacCtxBase = asic->cabacCtx.busAddress;

    if (regs->codingType == ASIC_VP9)
    {
      /* VP9: Table of counter for probability updates. */
      if (EWLMallocLinear(asic->ewl, ASIC_VP9_PROB_COUNT_SIZE,
                          &asic->probCount) != EWL_OK)
      {
        EncAsicMemFree_V2(asic);
        return ENCHW_NOK;
      }
      regs->probCountBase = asic->probCount.busAddress;

    }


    /* NAL size table, table size must be 64-bit multiple,
     * space for SEI, MVC prefix, filler and zero at the end of table.
     * Atleast 1 macroblock row in every slice.
     */
    buff = &asic->sizeTbl;
    asic->sizeTblSize = (sizeof(u32) * ((height + 63) / 64 + 1) + 7) & (~7);

    if (EWLMallocLinear(asic->ewl, asic->sizeTblSize, buff) != EWL_OK)
    {
      EncAsicMemFree_V2(asic);
      return ENCHW_NOK;
    }

    //data from Yuxiang.
    if (EWLMallocLinear(asic->ewl, 288 * 1024 / 8,
                        &asic->compress_coeff_SACN) != EWL_OK)
    {
      EncAsicMemFree_V2(asic);
      return ENCHW_NOK;
    }
    regs->compress_coeff_scan_base = asic->compress_coeff_SACN.busAddress;

    if(asic->regs.asicHwId  > 0x48320101)
    {
      if (EWLMallocLinear(asic->ewl, width*height/64/32,
                        &asic->ctbRCCur) != EWL_OK)
      {
        EncAsicMemFree_V2(asic);
        return ENCHW_NOK;
      }
      if (EWLMallocLinear(asic->ewl, width*height/64/32,
                        &asic->ctbRCPre) != EWL_OK)
      {
        EncAsicMemFree_V2(asic);
        return ENCHW_NOK;
      }
    }


    //allocate compressor table
    //TODO: to support turn on/off compressor on the fly
    if (compressor)
    {
      u32 tblLumaSize = 0;
      u32 tblChromaSize = 0;
      u32 tblSize = 0;
      if (compressor & 1)
      {
        tblLumaSize = ((width + 63) / 64) * ((height + 63) / 64) * 8; //ctu_num * 8
        tblLumaSize = ((tblLumaSize + 15) >> 4) << 4;
      }
      if (compressor & 2)
      {
        int cbs_w = ((width >> 1) + 7) / 8;
        int cbs_h = ((height >> 1) + 3) / 4;
        int cbsg_w = (cbs_w + 15) / 16;
        tblChromaSize = cbsg_w * cbs_h * 16;
      }

      tblSize = tblLumaSize + tblChromaSize;

      for (i = 0; i < total_allocated_buffers; i++)
      {
        if (EWLMallocLinear(asic->ewl, tblSize, &asic->compressTbl[i]) != EWL_OK)
        {
          EncAsicMemFree_V2(asic);
          return ENCHW_NOK;
        }
      }
    }
  }
  return ENCHW_OK;
}

/*------------------------------------------------------------------------------

    EncAsicMemFree_V2

    Free HW/SW shared memory

------------------------------------------------------------------------------*/
void EncAsicMemFree_V2(asicData_s *asic)
{
  i32 i;

  ASSERT(asic != NULL);
  ASSERT(asic->ewl != NULL);

  for (i = 0; i < ASIC_FRAME_BUF_LUM_MAX; i++)
  {
    if (asic->internalreconLuma[i].virtualAddress != NULL)
      EWLFreeRefFrm(asic->ewl, &asic->internalreconLuma[i]);
    asic->internalreconLuma[i].virtualAddress = NULL;

    if (asic->internalreconChroma[i].virtualAddress != NULL)
      EWLFreeRefFrm(asic->ewl, &asic->internalreconChroma[i]);
    asic->internalreconChroma[i].virtualAddress = NULL;

    if (asic->compressTbl[i].virtualAddress != NULL)
      EWLFreeRefFrm(asic->ewl, &asic->compressTbl[i]);
    asic->compressTbl[i].virtualAddress = NULL;
  }

  if (asic->cabacCtx.virtualAddress != NULL)
    EWLFreeLinear(asic->ewl, &asic->cabacCtx);

  if (asic->probCount.virtualAddress != NULL)
    EWLFreeLinear(asic->ewl, &asic->probCount);

  if(asic->compress_coeff_SACN.virtualAddress != NULL)
    EWLFreeLinear(asic->ewl, &asic->compress_coeff_SACN);

  if (asic->sizeTbl.virtualAddress != NULL)
    EWLFreeLinear(asic->ewl, &asic->sizeTbl);
  if (asic->ctbRCCur.virtualAddress != NULL)
    EWLFreeLinear(asic->ewl, &asic->ctbRCCur);
  if (asic->ctbRCPre.virtualAddress != NULL)
    EWLFreeLinear(asic->ewl, &asic->ctbRCPre);
   
  asic->sizeTbl.virtualAddress = NULL;
  asic->cabacCtx.virtualAddress = NULL;
  asic->mvOutput.virtualAddress = NULL;
  asic->probCount.virtualAddress = NULL;
  asic->segmentMap.virtualAddress = NULL;
  asic->ctbRCCur.virtualAddress = NULL;
  asic->ctbRCPre.virtualAddress = NULL;
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
i32 EncAsicCheckStatus_V2(asicData_s *asic,u32 status)
{
  i32 ret;

  //status = EncAsicGetStatus(asic->ewl);

  if (status & ASIC_STATUS_ERROR)
  {
    /* Get registers for debugging */
    EncAsicGetRegisters(asic->ewl, &asic->regs);

    ret = ASIC_STATUS_ERROR;
    //EncAsicClearStatus(asic->ewl,status&(~(ASIC_STATUS_ERROR|ASIC_IRQ_LINE)));
    EWLReleaseHw(asic->ewl);
  }
  else if (status & ASIC_STATUS_HW_TIMEOUT)
  {
    /* Get registers for debugging */
    EncAsicGetRegisters(asic->ewl, &asic->regs);

    ret = ASIC_STATUS_HW_TIMEOUT;
    //EncAsicClearStatus(asic->ewl,status&(~(ASIC_STATUS_HW_TIMEOUT|ASIC_IRQ_LINE)));
    EWLReleaseHw(asic->ewl);
  }
  else if (status & ASIC_STATUS_FRAME_READY)
  {
    EncAsicGetRegisters(asic->ewl, &asic->regs);

    ret = ASIC_STATUS_FRAME_READY;
    //EncAsicClearStatus(asic->ewl,status&(~(ASIC_STATUS_FRAME_READY|ASIC_IRQ_LINE)));
    //EWLReleaseHw(asic->ewl);
  }
  else if (status & ASIC_STATUS_BUFF_FULL)
  {
    ret = ASIC_STATUS_BUFF_FULL;
    /* ASIC doesn't support recovery from buffer full situation,
     * at the same time with buff full ASIC also resets itself. */
    //EncAsicClearStatus(asic->ewl,status&(~(ASIC_STATUS_BUFF_FULL|ASIC_IRQ_LINE)));
    EWLReleaseHw(asic->ewl);
  }
  else if (status & ASIC_STATUS_HW_RESET)
  {
    ret = ASIC_STATUS_HW_RESET;
    //EncAsicClearStatus(asic->ewl,status&(~(ASIC_STATUS_HW_RESET|ASIC_IRQ_LINE)));
    EWLReleaseHw(asic->ewl);
  }
  else if (status & ASIC_STATUS_FUSE)
  {
    ret = ASIC_STATUS_ERROR;
    //EncAsicClearStatus(asic->ewl,status&(~(ASIC_STATUS_FUSE|ASIC_IRQ_LINE)));
    EWLReleaseHw(asic->ewl);
  }
  else /* Don't check SLICE_READY status bit, it is reseted by IRQ handler */
  {
    /* IRQ received but none of the status bits is high, so it must be
     * slice ready. */
    //EncAsicClearStatus(asic->ewl,status&(~(ASIC_STATUS_SLICE_READY|ASIC_IRQ_LINE)));
    ret = ASIC_STATUS_SLICE_READY;
  }

  return ret;
}
