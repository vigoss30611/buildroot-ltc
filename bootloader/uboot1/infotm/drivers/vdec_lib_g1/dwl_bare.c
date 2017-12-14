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
--  Description :  dwl common part
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: dwl_linux.c,v $
--  $Revision: 1.54 $
--  $Date: 2011/01/17 13:18:22 $
--
------------------------------------------------------------------------------*/

#include <malloc.h>
#include "basetype.h"
#include "dwl.h"
#include "dwl_bare.h"
#include "imapx_dec_irq.h"


#define DWL_MPEG2_E         31  /* 1 bit */
#define DWL_VC1_E           29  /* 2 bits */
#define DWL_JPEG_E          28  /* 1 bit */
#define DWL_MPEG4_E         26  /* 2 bits */
#define DWL_H264_E          24  /* 2 bits */
#define DWL_VP6_E           23  /* 1 bit */
#define DWL_PJPEG_E         22  /* 1 bit */
#define DWL_REF_BUFF_E      20  /* 1 bit */

#define DWL_JPEG_EXT_E          31  /* 1 bit */
#define DWL_REF_BUFF_ILACE_E    30  /* 1 bit */
#define DWL_MPEG4_CUSTOM_E      29  /* 1 bit */
#define DWL_REF_BUFF_DOUBLE_E   28  /* 1 bit */
#define DWL_RV_E            26  /* 2 bits */
#define DWL_VP7_E           24  /* 1 bit */
#define DWL_VP8_E           23  /* 1 bit */
#define DWL_AVS_E           22  /* 1 bit */
#define DWL_MVC_E           20  /* 2 bits */
#define DWL_WEBP_E          19  /* 1 bit */
#define DWL_DEC_TILED_L     17  /* 2 bits */

#define DWL_CFG_E           24  /* 4 bits */
#define DWL_PP_E            16  /* 1 bit */
#define DWL_PP_IN_TILED_L   14  /* 2 bits */

#define DWL_SORENSONSPARK_E 11  /* 1 bit */

#define DWL_H264_FUSE_E          31 /* 1 bit */
#define DWL_MPEG4_FUSE_E         30 /* 1 bit */
#define DWL_MPEG2_FUSE_E         29 /* 1 bit */
#define DWL_SORENSONSPARK_FUSE_E 28 /* 1 bit */
#define DWL_JPEG_FUSE_E          27 /* 1 bit */
#define DWL_VP6_FUSE_E           26 /* 1 bit */
#define DWL_VC1_FUSE_E           25 /* 1 bit */
#define DWL_PJPEG_FUSE_E         24 /* 1 bit */
#define DWL_CUSTOM_MPEG4_FUSE_E  23 /* 1 bit */
#define DWL_RV_FUSE_E            22 /* 1 bit */
#define DWL_VP7_FUSE_E           21 /* 1 bit */
#define DWL_VP8_FUSE_E           20 /* 1 bit */
#define DWL_AVS_FUSE_E           19 /* 1 bit */
#define DWL_MVC_FUSE_E           18 /* 1 bit */

#define DWL_DEC_MAX_1920_FUSE_E  15 /* 1 bit */
#define DWL_DEC_MAX_1280_FUSE_E  14 /* 1 bit */
#define DWL_DEC_MAX_720_FUSE_E   13 /* 1 bit */
#define DWL_DEC_MAX_352_FUSE_E   12 /* 1 bit */
#define DWL_REF_BUFF_FUSE_E       7 /* 1 bit */


#define DWL_PP_FUSE_E				31  /* 1 bit */
#define DWL_PP_DEINTERLACE_FUSE_E   30  /* 1 bit */
#define DWL_PP_ALPHA_BLEND_FUSE_E   29  /* 1 bit */
#define DWL_PP_MAX_1920_FUSE_E		15  /* 1 bit */
#define DWL_PP_MAX_1280_FUSE_E		14  /* 1 bit */
#define DWL_PP_MAX_720_FUSE_E		13  /* 1 bit */
#define DWL_PP_MAX_352_FUSE_E		12  /* 1 bit */

#ifdef _DWL_FAKE_HW_TIMEOUT
static void DWLFakeTimeout(u32 * status);
#endif

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicID
    Description     : Read the HW ID. Does not need a DWL instance to run

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
u32 DWLReadAsicID()
{
    /*
    u32 *io =NULL;
    DWL_DEBUG("DWLReadAsicID\n");
    unsigned int tmp;

   *(volatile unsigned int *)(0x21e0a160 + 0xc) = 1; // disable output
    *(volatile unsigned int *)(0x21e0a160 + 0x0) = 1; // choose pll, apll-0, dpll-1, epll-2, vpll-3
    *(volatile unsigned int *)(0x21e0a160 + 0x4) = 1; // set div
    *(volatile unsigned int *)(0x21e0a160 + 0xc) = 0; // enable output
    // bus clock
    *(volatile unsigned int *)(0x21e30000 + 0) = 0xffffffff; //  
    *(volatile unsigned int *)(0x21e30000 + 0xc) = 0; //  
    *(volatile unsigned int *)(0x21e30000 + 4) = 0xffffffff; // clock enable
    *(volatile unsigned int *)(0x21e30000 + 8) = 0x11; // module power on
    do{ 
        tmp = *(volatile unsigned int *)(0x21e30000 + 8); 
    }while(!(tmp & 0x2)); // wait ack.

    *(volatile unsigned int *)(0x21e30000 + 8) = 0x1; //  
    *(volatile unsigned int *)(0x21e30000 + 0x18) = 0x1; //  
    *(volatile unsigned int *)(0x21e30000 + 0xc) = 3; //  
    *(volatile unsigned int *)(0x21e30000 + 0x14) = 0; //  
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x0; //  
    *(volatile unsigned int *)0x21e0c820 = 0x1; //MEM set, vdec mode
    *(volatile unsigned int *)0x21e0d000 = 0x1;     // trigger efuse read
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x7; // module reset
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x0;

	io = (u32 *)IMAPX_VDEC_REG_BASE;
    printf("xxxx io[0] = %x \n", io[0]);
    return io[0];
    */
    return 0x67312058;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicConfig
    Description     : Read HW configuration. Does not need a DWL instance to run

    Return type     : DWLHwConfig_t - structure with HW configuration
------------------------------------------------------------------------------*/
void DWLReadAsicConfig(DWLHwConfig_t * pHwCfg)
{
    u32 *io = NULL;
    u32 configReg;
    u32 asicID;
    unsigned int tmp;
    DWL_DEBUG("DWLReadAsicConfig\n");

    *(volatile unsigned int *)(0x21e0a160 + 0xc) = 1; // disable output
    *(volatile unsigned int *)(0x21e0a160 + 0x0) = 2; // choose pll, apll-0, dpll-1, epll-2, vpll-3
    *(volatile unsigned int *)(0x21e0a160 + 0x4) = 5; // set div
    *(volatile unsigned int *)(0x21e0a160 + 0xc) = 0; // enable output
    // bus clock
    *(volatile unsigned int *)(0x21e30000 + 0) = 0xffffffff; //  
    *(volatile unsigned int *)(0x21e30000 + 0xc) = 0; //  
    *(volatile unsigned int *)(0x21e30000 + 4) = 0xffffffff; // clock enable
    *(volatile unsigned int *)(0x21e30000 + 8) = 0x11; // module power on
	//return;//ok
    do{ 
        tmp = *(volatile unsigned int *)(0x21e30000 + 8); 
    }while(!(tmp & 0x2)); // wait ack.

	//return;//ok
    *(volatile unsigned int *)(0x21e30000 + 8) = 0x1; //  
    *(volatile unsigned int *)(0x21e30000 + 0x18) = 0x1; //  
    *(volatile unsigned int *)(0x21e30000 + 0xc) = 3; //  
    *(volatile unsigned int *)(0x21e30000 + 0x14) = 0; //  
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x0; //  
	//return;//ok
    //*(volatile unsigned int *)0x21e0c820 = 0x1; //MEM set, vdec mode
	//return;//failed
    *(volatile unsigned int *)0x21e0d000 = 0x1;     // trigger efuse read
	//return;//failed
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x7; // module reset
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x0;

	//return;//failed
    DWLmemset(pHwCfg, 0, sizeof(*pHwCfg));

    io = (u32 *)IMAPX_VDEC_REG_BASE;

  //  printf("io[0]= %x \n", io[0]);
    /* Decoder configuration */
    configReg = io[HX170DEC_SYNTH_CFG];
  //  printf("configReg = %x \n", configReg);

    pHwCfg->h264Support = (configReg >> DWL_H264_E) & 0x3U;
    /* check jpeg */
    pHwCfg->jpegSupport = (configReg >> DWL_JPEG_E) & 0x01U;
    if(pHwCfg->jpegSupport && ((configReg >> DWL_PJPEG_E) & 0x01U))
        pHwCfg->jpegSupport = JPEG_PROGRESSIVE;
    pHwCfg->mpeg4Support = (configReg >> DWL_MPEG4_E) & 0x3U;
    pHwCfg->vc1Support = (configReg >> DWL_VC1_E) & 0x3U;
    pHwCfg->mpeg2Support = (configReg >> DWL_MPEG2_E) & 0x01U;
    pHwCfg->sorensonSparkSupport = (configReg >> DWL_SORENSONSPARK_E) & 0x01U;
    pHwCfg->refBufSupport = (configReg >> DWL_REF_BUFF_E) & 0x01U;
    pHwCfg->vp6Support = (configReg >> DWL_VP6_E) & 0x01U;
#ifdef DEC_X170_APF_DISABLE
    if(DEC_X170_APF_DISABLE)
    {
        pHwCfg->tiledModeSupport = 0;
    }
#endif /* DEC_X170_APF_DISABLE */

    pHwCfg->maxDecPicWidth = configReg & 0x07FFU;

    /* 2nd Config register */
    configReg = io[HX170DEC_SYNTH_CFG_2];
    if(pHwCfg->refBufSupport)
    {
        if((configReg >> DWL_REF_BUFF_ILACE_E) & 0x01U)
            pHwCfg->refBufSupport |= 2;
        if((configReg >> DWL_REF_BUFF_DOUBLE_E) & 0x01U)
            pHwCfg->refBufSupport |= 4;
    }

    pHwCfg->customMpeg4Support = (configReg >> DWL_MPEG4_CUSTOM_E) & 0x01U;
    pHwCfg->vp7Support = (configReg >> DWL_VP7_E) & 0x01U;
    pHwCfg->vp8Support = (configReg >> DWL_VP8_E) & 0x01U;
    pHwCfg->avsSupport = (configReg >> DWL_AVS_E) & 0x01U;

    /* JPEG xtensions */
    asicID = DWLReadAsicID();    
    if(((asicID >> 16) >= 0x8190U) ||
       ((asicID >> 16) == 0x6731U) )
        pHwCfg->jpegESupport = (configReg >> DWL_JPEG_EXT_E) & 0x01U;
    else
        pHwCfg->jpegESupport = JPEG_EXT_NOT_SUPPORTED;    
    
    if(((asicID >> 16) >= 0x9170U) ||
       ((asicID >> 16) == 0x6731U) )
        pHwCfg->rvSupport = (configReg >> DWL_RV_E) & 0x03U;
    else
        pHwCfg->rvSupport = RV_NOT_SUPPORTED;

    pHwCfg->mvcSupport = (configReg >> DWL_MVC_E) & 0x03U;

	// +Leo@2011/10/09, force set this to support webP.
    		//pHwCfg->webpSupport = (configReg >> DWL_WEBP_E) & 0x01U;
    		pHwCfg->webpSupport = 1;
	// -Leo@2011/10/09    
    pHwCfg->tiledModeSupport = (configReg >> DWL_DEC_TILED_L) & 0x03U;

    if(pHwCfg->refBufSupport &&
       (asicID >> 16) == 0x6731U )
    {
        pHwCfg->refBufSupport |= 8; /* enable HW support for offset */
    }

    /* Pp configuration */
    configReg = io[HX170PP_SYNTH_CFG];

    if((configReg >> DWL_PP_E) & 0x01U)
    {
        pHwCfg->ppSupport = 1;
        pHwCfg->maxPpOutPicWidth = configReg & 0x07FFU;
        /*pHwCfg->ppConfig = (configReg >> DWL_CFG_E) & 0x0FU; */
        pHwCfg->ppConfig = configReg;
    }
    else
    {
        pHwCfg->ppSupport = 0;
        pHwCfg->maxPpOutPicWidth = 0;
        pHwCfg->ppConfig = 0;
    }

    {
        /* check the HW versio */
        asicID = DWLReadAsicID();
        if(((asicID >> 16) >= 0x8190U) ||
           ((asicID >> 16) == 0x6731U) )
        {
            u32 deInterlace;
            u32 alphaBlend;
            u32 deInterlaceFuse;
            u32 alphaBlendFuse;
            DWLHwFuseStatus_t hwFuseSts;

            /* check fuse status */
            DWLReadAsicFuseStatus(&hwFuseSts);

            /* Maximum decoding width supported by the HW */
            if(pHwCfg->maxDecPicWidth > hwFuseSts.maxDecPicWidthFuse)
                pHwCfg->maxDecPicWidth = hwFuseSts.maxDecPicWidthFuse;
            /* Maximum output width of Post-Processor */
            if(pHwCfg->maxPpOutPicWidth > hwFuseSts.maxPpOutPicWidthFuse)
                pHwCfg->maxPpOutPicWidth = hwFuseSts.maxPpOutPicWidthFuse;
            /* h264 */
            if(!hwFuseSts.h264SupportFuse)
                pHwCfg->h264Support = H264_NOT_SUPPORTED;
            /* mpeg-4 */
            if(!hwFuseSts.mpeg4SupportFuse)
                pHwCfg->mpeg4Support = MPEG4_NOT_SUPPORTED;
            /* custom mpeg-4 */
            if(!hwFuseSts.customMpeg4SupportFuse)
                pHwCfg->customMpeg4Support = MPEG4_CUSTOM_NOT_SUPPORTED;
            /* jpeg (baseline && progressive) */
            if(!hwFuseSts.jpegSupportFuse)
                pHwCfg->jpegSupport = JPEG_NOT_SUPPORTED;
            if((pHwCfg->jpegSupport == JPEG_PROGRESSIVE) &&
               !hwFuseSts.jpegProgSupportFuse)
                pHwCfg->jpegSupport = JPEG_BASELINE;
            /* mpeg-2 */
            if(!hwFuseSts.mpeg2SupportFuse)
                pHwCfg->mpeg2Support = MPEG2_NOT_SUPPORTED;
            /* vc-1 */
            if(!hwFuseSts.vc1SupportFuse)
                pHwCfg->vc1Support = VC1_NOT_SUPPORTED;
            /* vp6 */
            if(!hwFuseSts.vp6SupportFuse)
                pHwCfg->vp6Support = VP6_NOT_SUPPORTED;
            /* vp7 */
            if(!hwFuseSts.vp7SupportFuse)
                pHwCfg->vp7Support = VP7_NOT_SUPPORTED;
            /* vp6 */
            if(!hwFuseSts.vp8SupportFuse)
                pHwCfg->vp8Support = VP8_NOT_SUPPORTED;
            /* pp */
            if(!hwFuseSts.ppSupportFuse)
                pHwCfg->ppSupport = PP_NOT_SUPPORTED;
            /* check the pp config vs fuse status */
            if((pHwCfg->ppConfig & 0xFC000000) &&
               ((hwFuseSts.ppConfigFuse & 0xF0000000) >> 5))
            {
                /* config */
                deInterlace = ((pHwCfg->ppConfig & PP_DEINTERLACING) >> 25);
                alphaBlend = ((pHwCfg->ppConfig & PP_ALPHA_BLENDING) >> 24);
                /* fuse */
                deInterlaceFuse =
                    (((hwFuseSts.ppConfigFuse >> 5) & PP_DEINTERLACING) >> 25);
                alphaBlendFuse =
                    (((hwFuseSts.ppConfigFuse >> 5) & PP_ALPHA_BLENDING) >> 24);

                /* check if */
                if(deInterlace && !deInterlaceFuse)
                    pHwCfg->ppConfig &= 0xFD000000;
                if(alphaBlend && !alphaBlendFuse)
                    pHwCfg->ppConfig &= 0xFE000000;
            }
            /* sorenson */
            if(!hwFuseSts.sorensonSparkSupportFuse)
                pHwCfg->sorensonSparkSupport = SORENSON_SPARK_NOT_SUPPORTED;
            /* ref. picture buffer */
            if(!hwFuseSts.refBufSupportFuse)
                pHwCfg->refBufSupport = REF_BUF_NOT_SUPPORTED;

            /* rv */
            if(!hwFuseSts.rvSupportFuse)
                pHwCfg->rvSupport = RV_NOT_SUPPORTED;
            /* avs */
            if(!hwFuseSts.avsSupportFuse)
                pHwCfg->avsSupport = AVS_NOT_SUPPORTED;
            /* mvc */
            if(!hwFuseSts.mvcSupportFuse)
                pHwCfg->mvcSupport = MVC_NOT_SUPPORTED;
        }
    }

}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicFuseStatus
    Description     : Read HW fuse configuration. Does not need a DWL instance to run

    Returns     : DWLHwFuseStatus_t * pHwFuseSts - structure with HW fuse configuration
------------------------------------------------------------------------------*/
void DWLReadAsicFuseStatus(DWLHwFuseStatus_t * pHwFuseSts)
{
    u32*io = NULL;
    u32 configReg;
    u32 fuseReg;
    u32 fuseRegPp;

    DWL_DEBUG("DWLReadAsicFuseStatus\n");

    DWLmemset(pHwFuseSts, 0, sizeof(*pHwFuseSts));


    io = (u32 *)IMAPX_VDEC_REG_BASE;

    /* Decoder fuse configuration */
    fuseReg = io[HX170DEC_FUSE_CFG];

    pHwFuseSts->h264SupportFuse = (fuseReg >> DWL_H264_FUSE_E) & 0x01U;
    pHwFuseSts->mpeg4SupportFuse = (fuseReg >> DWL_MPEG4_FUSE_E) & 0x01U;
    pHwFuseSts->mpeg2SupportFuse = (fuseReg >> DWL_MPEG2_FUSE_E) & 0x01U;
    pHwFuseSts->sorensonSparkSupportFuse =
        (fuseReg >> DWL_SORENSONSPARK_FUSE_E) & 0x01U;
    pHwFuseSts->jpegSupportFuse = (fuseReg >> DWL_JPEG_FUSE_E) & 0x01U;
    pHwFuseSts->vp6SupportFuse = (fuseReg >> DWL_VP6_FUSE_E) & 0x01U;
    pHwFuseSts->vc1SupportFuse = (fuseReg >> DWL_VC1_FUSE_E) & 0x01U;
    pHwFuseSts->jpegProgSupportFuse = (fuseReg >> DWL_PJPEG_FUSE_E) & 0x01U;
    pHwFuseSts->rvSupportFuse = (fuseReg >> DWL_RV_FUSE_E) & 0x01U;
    pHwFuseSts->avsSupportFuse = (fuseReg >> DWL_AVS_FUSE_E) & 0x01U;
	pHwFuseSts->vp7SupportFuse = (fuseReg >> DWL_VP7_FUSE_E) & 0x01U;
	pHwFuseSts->vp8SupportFuse = (fuseReg >> DWL_VP8_FUSE_E) & 0x01U;
    pHwFuseSts->customMpeg4SupportFuse = (fuseReg >> DWL_CUSTOM_MPEG4_FUSE_E) & 0x01U;
    pHwFuseSts->mvcSupportFuse = (fuseReg >> DWL_MVC_FUSE_E) & 0x01U;

    /* check max. decoder output width */
    if(fuseReg & 0x8000U)
        pHwFuseSts->maxDecPicWidthFuse = 1920;
    else if(fuseReg & 0x4000U)
        pHwFuseSts->maxDecPicWidthFuse = 1280;
    else if(fuseReg & 0x2000U)
        pHwFuseSts->maxDecPicWidthFuse = 720;
    else if(fuseReg & 0x1000U)
        pHwFuseSts->maxDecPicWidthFuse = 352;

    pHwFuseSts->refBufSupportFuse = (fuseReg >> DWL_REF_BUFF_FUSE_E) & 0x01U;

    /* Pp configuration */
    configReg = io[HX170PP_SYNTH_CFG];

    if((configReg >> DWL_PP_E) & 0x01U)
    {
        /* Pp fuse configuration */
        fuseRegPp = io[HX170PP_FUSE_CFG];

        if((fuseRegPp >> DWL_PP_FUSE_E) & 0x01U)
        {
            pHwFuseSts->ppSupportFuse = 1;

            /* check max. pp output width */
            if(fuseRegPp & 0x8000U)
                pHwFuseSts->maxPpOutPicWidthFuse = 1920;
            else if(fuseRegPp & 0x4000U)
                pHwFuseSts->maxPpOutPicWidthFuse = 1280;
            else if(fuseRegPp & 0x2000U)
                pHwFuseSts->maxPpOutPicWidthFuse = 720;
            else if(fuseRegPp & 0x1000U)
                pHwFuseSts->maxPpOutPicWidthFuse = 352;

            pHwFuseSts->ppConfigFuse = fuseRegPp;
        }
        else
        {
            pHwFuseSts->ppSupportFuse = 0;
            pHwFuseSts->maxPpOutPicWidthFuse = 0;
            pHwFuseSts->ppConfigFuse = 0;
        }
    }

}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocRefFrm
    Description     : Allocate a frame buffer (contiguous linear RAM memory)

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocRefFrm(const void *instance, u32 size, DWLLinearMem_t * info)
{
    return DWLMallocLinear(instance, size, info);
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeRefFrm
    Description     : Release a frame buffer previously allocated with
                        DWLMallocRefFrm.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - frame buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeRefFrm(const void *instance, DWLLinearMem_t * info)
{
    DWLFreeLinear(instance, info);
}

/*------------------------------------------------------------------------------
    Function name   : DWLMallocLinear
    Description     : Allocate a contiguous, linear RAM  memory buffer

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 DWLMallocLinear(const void *instance, u32 size, DWLLinearMem_t * info)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    u32 pgsize = 8;

    assert(dec_dwl != NULL);
    assert(info != NULL);

    info->size = (size + (pgsize - 1)) & (~(pgsize - 1));

    info->virtualAddress = (u32 *)DWLmalloc(info->size);
    info->busAddress = (u32)info->virtualAddress;

	if((info->virtualAddress == NULL) || ( (info->busAddress & 0x07) != 0)){
		return DWL_ERROR;
	}
    return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeLinear
    Description     : Release a linera memory buffer, previously allocated with
                        DWLMallocLinear.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - linear buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeLinear(const void *instance, DWLLinearMem_t * info)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    assert(dec_dwl != NULL);
    assert(info != NULL);
	if(info->virtualAddress !=0)
		DWLfree((void *)info->virtualAddress);
	info->virtualAddress = NULL; 
	info->busAddress = 0;
	info->size = 0;

}

/*------------------------------------------------------------------------------
    Function name   : DWLWriteReg
    Description     : Write a value to a hardware IO register

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLWriteReg(const void *instance, u32 offset, u32 value)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    DWL_DEBUG("DWL: Write reg %d at offset 0x%02X --> %08X\n", offset / 4,
              offset, value);
    assert((dec_dwl->clientType != DWL_CLIENT_TYPE_PP &&
            offset < HX170PP_REG_START) ||
           (dec_dwl->clientType == DWL_CLIENT_TYPE_PP &&
            offset >= HX170PP_REG_START));

    assert(dec_dwl != NULL);
    assert(offset < dec_dwl->regSize);

    offset = offset / 4;

#ifdef _DWL_HW_PERFORMANCE
    if(offset == HX170DEC_REG_START / 4 && value & DWL_HW_ENABLE_BIT)
    {
        DwlDecoderEnable();
    }
    if(offset == HX170PP_REG_START / 4 && value & DWL_HW_ENABLE_BIT)
    {
        DwlPpEnable();
    }
#endif

#ifdef _DWL_FAKE_HW_TIMEOUT
    /* Compensate for hacking the timeout bit in the read */
    if(offset == HX170DEC_REG_START / 4 && value & DWL_HW_TIMEOUT_BIT)
        *(dec_dwl->pRegBase + offset) = value & ~DWL_HW_TIMEOUT_BIT;
    else
        *(dec_dwl->pRegBase + offset) = value;
#else
    *(dec_dwl->pRegBase + offset) = value;
#endif
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadReg
    Description     : Read the value of a hardware IO register

    Return type     : u32 - the value stored in the register

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be read
------------------------------------------------------------------------------*/
u32 DWLReadReg(const void *instance, u32 offset)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
    u32 val;

    assert((dec_dwl->clientType != DWL_CLIENT_TYPE_PP &&
            offset < HX170PP_REG_START) ||
           (dec_dwl->clientType == DWL_CLIENT_TYPE_PP &&
            offset >= HX170PP_REG_START) || (offset == 0) ||
           (offset == HX170PP_SYNTH_CFG));

    assert(dec_dwl != NULL);
    assert(offset < dec_dwl->regSize);

    offset = offset / 4;
    val = *(dec_dwl->pRegBase + offset);

    DWL_DEBUG("DWL: Read reg %d at offset 0x%02X --> %08X\n", offset,
              offset * 4, val);

#ifdef _DWL_FAKE_HW_TIMEOUT
    if(offset == HX170DEC_REG_START / 4)
    {
        DWLFakeTimeout(&val);
    }
#endif
    return val;
}

/*------------------------------------------------------------------------------
    Function name   : DWLEnableHW
    Description     : Enable hw by writing to register
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLEnableHW(const void *instance, u32 offset, u32 value)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    DWLWriteReg(dec_dwl, offset, value);
    DWL_DEBUG("DWLEnableHW (by previous DWLWriteReg)\n");
}

/*------------------------------------------------------------------------------
    Function name   : DWLDisableHW
    Description     : Disable hw by writing to register
    Return type     : void
    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void DWLDisableHW(const void *instance, u32 offset, u32 value)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    DWLWriteReg(dec_dwl, offset, value);
    DWL_DEBUG("DWLDisableHW (by previous DWLWriteReg)\n");
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitHwReady
    Description     : Wait until hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR

    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitHwReady(const void *instance, u32 timeout)
{
    const hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    i32 ret;

    assert(dec_dwl);

    switch (dec_dwl->clientType)
    {
    case DWL_CLIENT_TYPE_H264_DEC:
    case DWL_CLIENT_TYPE_MPEG4_DEC:
    case DWL_CLIENT_TYPE_JPEG_DEC:
    case DWL_CLIENT_TYPE_VC1_DEC:
    case DWL_CLIENT_TYPE_MPEG2_DEC:
    case DWL_CLIENT_TYPE_RV_DEC:
    case DWL_CLIENT_TYPE_VP6_DEC:
    case DWL_CLIENT_TYPE_VP8_DEC:
    case DWL_CLIENT_TYPE_AVS_DEC:
        {
            ret = DWLWaitDecHwReady(dec_dwl, timeout);
            break;
        }
    case DWL_CLIENT_TYPE_PP:
        {
            ret = DWLWaitPpHwReady(dec_dwl, timeout);
            break;
        }
    default:
        {
            assert(0);  /* should not happen */
            ret = DWL_HW_WAIT_ERROR;
        }
    }

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : DWLmalloc
    Description     : Allocate a memory block. Same functionality as
                      the ANSI C malloc()

    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available

    Argument        : u32 n - Bytes to allocate
------------------------------------------------------------------------------*/
void *DWLmalloc(u32 n)
{
    return (void *)malloc(n);
}

/*------------------------------------------------------------------------------
    Function name   : DWLfree
    Description     : Deallocates or frees a memory block. Same functionality as
                      the ANSI C free()

    Return type     : void

    Argument        : void *p - Previously allocated memory block to be freed
------------------------------------------------------------------------------*/
void DWLfree(void *p)
{
    if(p != NULL)
        free(p);
}

/*------------------------------------------------------------------------------
    Function name   : DWLcalloc
    Description     : Allocates an array in memory with elements initialized
                      to 0. Same functionality as the ANSI C calloc()

    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available

    Argument        : u32 n - Number of elements
    Argument        : u32 s - Length in bytes of each element.
------------------------------------------------------------------------------*/
void *DWLcalloc(u32 n, u32 s)
{
    return (void *)calloc(n, s);
}

/*------------------------------------------------------------------------------
    Function name   : DWLmemcpy
    Description     : Copies characters between buffers. Same functionality as
                      the ANSI C memcpy()

    Return type     : The value of destination d

    Argument        : void *d - Destination buffer
    Argument        : const void *s - Buffer to copy from
    Argument        : u32 n - Number of bytes to copy
------------------------------------------------------------------------------*/
void *DWLmemcpy(void *d, const void *s, u32 n)
{
    return (void *)memcpy(d, s, n);
}

/*------------------------------------------------------------------------------
    Function name   : DWLmemset
    Description     : Sets buffers to a specified character. Same functionality
                      as the ANSI C memset()

    Return type     : The value of destination d

    Argument        : void *d - Pointer to destination
    Argument        : i32 c - Character to set
    Argument        : u32 n - Number of characters
------------------------------------------------------------------------------*/
void *DWLmemset(void *d, i32 c, u32 n)
{
    return (void *)memset(d, c, n);
}

/*------------------------------------------------------------------------------
    Function name   : DWLReleaseHw
    Description     :
    Return type     : void
    Argument        : const void *instance
------------------------------------------------------------------------------*/
void DWLReleaseHw(const void *instance)
{
}

/*------------------------------------------------------------------------------
    Function name   : DWLFakeTimeout
    Description     : Testing help function that changes HW stream errors info
                        HW timeouts. You can check how the SW behaves or not.
    Return type     : void
    Argument        : void
------------------------------------------------------------------------------*/

#ifdef _DWL_FAKE_HW_TIMEOUT
void DWLFakeTimeout(u32 * status)
{

    if((*status) & DWL_STREAM_ERROR_BIT)
    {
        *status &= ~DWL_STREAM_ERROR_BIT;
        *status |= DWL_HW_TIMEOUT_BIT;
        printf("\nDWL: Change stream error to hw timeout\n");
    }
}
#endif
