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

#include "basetype.h"
#include "dwl.h"
#include "dwl_linux.h"

//#include "memalloc.h"
#include "dwl_linux_lock.h"

#include "hx170dec.h"   /* This DWL uses the kernel module */

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include <errno.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fr/libfr.h>
#ifdef ANDROID
#include <cutils/log.h>
#endif

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


/*------------------------------------------------------------------------------
    Function name   : G1DWLMapRegisters
    Description     :

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
u32 *G1DWLMapRegisters(int mem_dev, unsigned int base,
                     unsigned int regSize, u32 write)
{
    const int pageSize = getpagesize();
    const int pageAlignment = pageSize - 1;

    size_t mapSize;
    const char *io = MAP_FAILED;
    DWL_DEBUG("base =%x, regsize = %d, pagesize= %d \n",base, regSize, pageSize);

    /* increase mapping size with unaligned part */
    mapSize = regSize + (base & pageAlignment);

    /* map page aligned base */
    if(write)
        io = (char *) mmap(0, mapSize, PROT_READ | PROT_WRITE,
                           MAP_SHARED, mem_dev, base & ~pageAlignment);
    else
        io = (char *) mmap(0, mapSize, PROT_READ, MAP_SHARED,
                           mem_dev, base & ~pageAlignment);

    /* add offset from alignment to the io start address */
    if(io != MAP_FAILED)
        io += (base & pageAlignment);
    DWL_DEBUG("map vir =%p", io);

    return (u32 *) io;
}

void G1G1DWLUnmapRegisters(const void *io, unsigned int regSize)
{
    const int pageSize = getpagesize();
    const int pageAlignment = pageSize - 1;

    munmap((void *) ((int) io & (~pageAlignment)),
           regSize + ((int) io & pageAlignment));
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLReadAsicID
    Description     : Read the HW ID. Does not need a DWL instance to run

    Return type     : u32 - the HW ID
------------------------------------------------------------------------------*/
u32 G1DWLReadAsicID()
{
    u32 *io = MAP_FAILED, id = ~0;
    int fd_dec = -1, fd;
    unsigned long base;
    unsigned int regSize;
    DWL_DEBUG("G1DWLReadAsicID\n");

	fd_dec = open(DEC_MODULE_PATH, O_RDWR | O_SYNC);
	if(fd_dec == -1)
	{
		DWL_DEBUG("G1DWLReadAsicID: failed to open %s\n", DEC_MODULE_PATH);
		goto end;
	}

    /* ask module for base */
    if(ioctl(fd_dec, HX170DEC_IOCGHWOFFSET, &base) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto end;
    }

    /* ask module for reg size */
    if(ioctl(fd_dec, HX170DEC_IOCGHWIOSIZE, &regSize) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto end;
    }

    io = G1DWLMapRegisters(fd_dec, base, regSize, 0);

    if(io == MAP_FAILED)
    {
        DWL_DEBUG("G1DWLReadAsicID: failed to mmap regs.");
        goto end;
    }

    id = io[0];

    G1G1DWLUnmapRegisters(io, regSize);

  end:
    if(fd_dec != -1)
        close(fd_dec);

    return id;
}

/*------------------------------------------------------------------------------
    Function name   : DWLReadAsicConfig
    Description     : Read HW configuration. Does not need a DWL instance to run

    Return type     : DWLHwConfig_t - structure with HW configuration
------------------------------------------------------------------------------*/
void DWLReadAsicConfig(DWLHwConfig_t * pHwCfg)
{
    const u32 *io = MAP_FAILED;
    u32 configReg;
    u32 asicID;
    unsigned long base;
    unsigned int regSize;

    int fd_dec = (-1);

    DWL_DEBUG("DWLReadAsicConfig\n");

    memset(pHwCfg, 0, sizeof(*pHwCfg));

	fd_dec = open(DEC_MODULE_PATH, O_RDWR | O_SYNC);
	if(fd_dec == -1)
	{
		DWL_DEBUG("DWLReadAsicConfig: failed to open %s\n", DEC_MODULE_PATH);
		goto end;
	}

    /* ask module for base */
    if(ioctl(fd_dec, HX170DEC_IOCGHWOFFSET, &base) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto end;
    }

    /* ask module for reg size */
    if(ioctl(fd_dec, HX170DEC_IOCGHWIOSIZE, &regSize) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto end;
    }
    io = G1DWLMapRegisters(fd_dec, base, regSize, 0);

    if(io == MAP_FAILED)
    {
        DWL_DEBUG("DWLReadAsicConfig: failed to mmap\n");
        goto end;
    }
    /* Decoder configuration */
    configReg = io[HX170DEC_SYNTH_CFG];

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
    asicID = G1DWLReadAsicID();    
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

    pHwCfg->webpSupport = (configReg >> DWL_WEBP_E) & 0x01U;
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
        asicID = G1DWLReadAsicID();
        if(((asicID >> 16) >= 0x8190U) ||
           ((asicID >> 16) == 0x6731U) )
        {
            u32 deInterlace;
            u32 alphaBlend;
            u32 deInterlaceFuse;
            u32 alphaBlendFuse;
            DWLHwFuseStatus_t hwFuseSts;

            /* check fuse status */
            G1DWLReadAsicFuseStatus(&hwFuseSts);

            /* Maximum decoding width supported by the HW */
            if(pHwCfg->maxDecPicWidth > hwFuseSts.maxDecPicWidthFuse)
                pHwCfg->maxDecPicWidth = hwFuseSts.maxDecPicWidthFuse;
            /* Maximum output width of Post-Processor */
            if(pHwCfg->maxPpOutPicWidth > hwFuseSts.maxPpOutPicWidthFuse)
                pHwCfg->maxPpOutPicWidth = hwFuseSts.maxPpOutPicWidthFuse;
            /* h264 */
            if(!hwFuseSts.h264SupportFuse){
                pHwCfg->h264Support = H264_NOT_SUPPORTED;
            }
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
	 
	G1G1DWLUnmapRegisters(io, regSize);

  end:
    if(fd_dec != -1)
        close(fd_dec);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLReadAsicFuseStatus
    Description     : Read HW fuse configuration. Does not need a DWL instance to run

    Returns     : DWLHwFuseStatus_t * pHwFuseSts - structure with HW fuse configuration
------------------------------------------------------------------------------*/
void G1DWLReadAsicFuseStatus(DWLHwFuseStatus_t * pHwFuseSts)
{
    const u32 *io = MAP_FAILED;
    u32 configReg;
    u32 fuseReg;
    u32 fuseRegPp;
    unsigned long base;
    unsigned int regSize;

    int fd_dec = (-1);

    DWL_DEBUG("G1DWLReadAsicFuseStatus\n");

    memset(pHwFuseSts, 0, sizeof(*pHwFuseSts));

	fd_dec = open(DEC_MODULE_PATH, O_RDWR | O_SYNC);
	if(fd_dec == -1)
	{
		DWL_DEBUG("G1DWLReadAsicFuseStatus: failed to open %s\n",
				DEC_MODULE_PATH);
		goto end;
	}

    /* ask module for base */
	DWL_DEBUG("fd_dec ioctl1");
    if(ioctl(fd_dec, HX170DEC_IOCGHWOFFSET, &base) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto end;
    }

    /* ask module for reg size */
    if(ioctl(fd_dec, HX170DEC_IOCGHWIOSIZE, &regSize) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto end;
    }

    io = G1DWLMapRegisters(fd_dec, base, regSize, 0);

    if(io == MAP_FAILED)
    {
        DWL_DEBUG("G1DWLReadAsicFuseStatus: failed to mmap\n");
        goto end;
    }

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

    G1G1DWLUnmapRegisters(io, regSize);

  end:
    if(fd_dec != -1)
        close(fd_dec);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLMallocRefFrm
    Description     : Allocate a frame buffer (contiguous linear RAM memory)

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 G1DWLMallocRefFrm(const void *instance, u32 size, DWLLinearMem_t * info)
{
	return G1DWLMallocLinear(instance, size, info);
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeRefFrm
    Description     : Release a frame buffer previously allocated with
                        G1DWLMallocRefFrm.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - frame buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeRefFrm(const void *instance, DWLLinearMem_t * info)
{
    DWLFreeLinear(instance, info);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLMallocLinear
    Description     : Allocate a contiguous, linear RAM  memory buffer

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - DWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : void *info - place where the allocated memory buffer
                        parameters are returned
------------------------------------------------------------------------------*/
i32 G1DWLMallocLinear(const void *instance, u32 size, DWLLinearMem_t * info)
{
	DWL_DEBUG("G1DWLMallocLinear \n");
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    u32 pgsize = getpagesize();
	//IM_Buffer buffer;
	//IM_RET ret;
    i32 flags; 
    u32 dev_addr;


    assert(dec_dwl != NULL);
    assert(info != NULL);

	info->size = size;
	info->virtualAddress = MAP_FAILED;
	info->busAddress = 0;

	size = (size + (pgsize - 1)) & (~(pgsize - 1));

#if 0
	/* get memory linear memory buffers */
	flags = ALC_FLAG_ALIGN_8BYTES;

#ifdef ION_USE
	flags |= dec_dwl->mmu_enable ? ALC_FLAG_DEVADDR : ALC_FLAG_PHYADDR;
	ret = alc_alloc(dec_dwl->alc_inst, size, flags, &buffer);
    info->uid = buffer.uid;
#else
	flags |= dec_dwl->mmu_enable ? (ALC_FLAG_DEVADDR | ALC_FLAG_PHY_LINEAR_PREFER) : ALC_FLAG_PHY_MUST;
	ret = alc_alloc(dec_dwl->alc_inst, size, &buffer, flags);
#endif 


	if(ret != IM_RET_OK){
		DWL_DEBUG("G1DWLMallocLinear: ERROR! No linear buffer available\n");
		return DWL_ERROR;
	}

 	/* physic linear addr */
    if(dec_dwl->mmu_enable)
    {
#ifdef ION_USE 
        info->busAddress = buffer.dev_addr;

#ifdef USE_DMMU
        dmmu_attach_buffer(dec_dwl->dmmu_inst, info->uid);
#endif
#else
         if(alc_get_devaddr(dec_dwl->alc_inst, &buffer, &dev_addr) != IM_RET_OK)
         {
		     DWL_DEBUG("G1DWLMallocLinear: ERROR! can not get dev addr \n");
             return DWL_ERROR;
         }
         info->busAddress = dev_addr;
#endif
    }
    else
    {
         info->busAddress = buffer.phy_addr;
    }
	
	/* virtual address */
	info->virtualAddress = buffer.vir_addr;
#endif	
	
	info->virtualAddress = fr_alloc_single("dwl_g1", size, &info->busAddress);
	if(!info->virtualAddress) {
		DWL_DEBUG("unable to allocat memory anymore\n");
		return DWL_ERROR  ;
	}

    DWL_DEBUG("G1DWLMallocLinear::mmu(%d) uid= %d, phy= %x, vir =%p \n",dec_dwl->mmu_enable, info->uid, info->busAddress,info->virtualAddress);

    return DWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLFreeLinear
    Description     : Release a linera memory buffer, previously allocated with
                        G1DWLMallocLinear.

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : void *info - linear buffer memory information
------------------------------------------------------------------------------*/
void DWLFreeLinear(const void *instance, DWLLinearMem_t * info)
{
	DWL_DEBUG("DWLFreeLinear \n");
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
	u32 pgsize = getpagesize();
	u32 size = 0;
	//IM_Buffer buffer;

    assert(dec_dwl != NULL);
    assert(info != NULL);

	size = (info->size + (pgsize - 1)) & (~(pgsize - 1));

#if 0
	buffer.phy_addr = dec_dwl->mmu_enable ? 0 : info->busAddress;
	buffer.vir_addr = info->virtualAddress ;
	buffer.size = size;
	buffer.flag = ALC_FLAG_ALIGN_8BYTES;
	
#ifdef ION_USE
#ifdef USE_DMMU
    if(dec_dwl->mmu_enable) 
        dmmu_detach_buffer(dec_dwl->dmmu_inst, info->uid);
#endif
    buffer.uid = info->uid;
	buffer.flag |= dec_dwl->mmu_enable ? ALC_FLAG_DEVADDR : ALC_FLAG_PHYADDR;
#else
	buffer.flag |= dec_dwl->mmu_enable ? (ALC_FLAG_DEVADDR | ALC_FLAG_PHY_LINEAR_PREFER) : ALC_FLAG_PHY_MUST;
#endif 

	if(info->virtualAddress != 0)
		alc_free(dec_dwl->alc_inst, &buffer);
#else
	fr_free_single(info->virtualAddress, info->busAddress);
	DWL_DEBUG("~~~G1DWLFreeLinear::mmu(%d) uid= %d, phy= %x, vir =%p \n",
    dec_dwl->mmu_enable, info->uid, info->busAddress,info->virtualAddress);

#endif

	return ;
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLWriteReg
    Description     : Write a value to a hardware IO register

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void G1DWLWriteReg(const void *instance, u32 offset, u32 value)
{
	DWL_DEBUG("G1DWLWriteReg \n");
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

	assert((dec_dwl->clientType != DWL_CLIENT_TYPE_PP &&
				offset < HX170PP_REG_START) ||
			(dec_dwl->clientType == DWL_CLIENT_TYPE_PP &&
			 offset >= HX170PP_REG_START));

    assert(dec_dwl != NULL);
    assert(offset < dec_dwl->regSize);

    offset = offset / 4;

	*(dec_dwl->pRegBase + offset) = value;
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLReadReg
    Description     : Read the value of a hardware IO register

    Return type     : u32 - the value stored in the register

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be read
------------------------------------------------------------------------------*/
u32 G1DWLReadReg(const void *instance, u32 offset)
{
	DWL_DEBUG("G1DWLReadReg \n");
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

	return val;
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLWriteNV21Reg
    Description     : Write a value to a hardware IO register

    Return type     : void

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be written
    Argument        : u32 value - value to be written out
------------------------------------------------------------------------------*/
void G1DWLWriteNV21Reg(const void *instance, u32 offset, u32 value)
{
	DWL_DEBUG("G1DWLWriteReg \n");
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    assert(dec_dwl != NULL);

    offset = offset / 4;

	*(dec_dwl->pNV21Base + offset) = value;
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLReadNV21Reg
    Description     : Read the value of a hardware IO register

    Return type     : u32 - the value stored in the register

    Argument        : const void * instance - DWL instance
    Argument        : u32 offset - byte offset of the register to be read
------------------------------------------------------------------------------*/
u32 G1DWLReadNV21Reg(const void *instance, u32 offset)
{
	DWL_DEBUG("G1DWLReadReg \n");
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
    u32 val;

    assert(dec_dwl != NULL);

    offset = offset / 4;
    val = *(dec_dwl->pNV21Base + offset);

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

	if(dec_dwl->dvfsEna  && !dec_dwl->hwEna){
		dec_dwl->hwEna = true;
        
		if(ioctl(dec_dwl->fd, VDEC_NOTIFY_HW_ENABLE, &dec_dwl->latencyMS) != 0){
			DWL_DEBUG("DWL: ioctl(VDEC_NOTIFY_HW_ENABLE) failed\n");
		}
	}

	G1DWLWriteReg(dec_dwl, offset, value);
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

	G1DWLWriteReg(dec_dwl, offset, value);
	DWL_DEBUG("DWLDisableHW (by previous G1DWLWriteReg)\n");

	if(dec_dwl->clientType != DWL_CLIENT_TYPE_H264_DEC && dec_dwl->dvfsEna && dec_dwl->hwEna){
		dec_dwl->hwEna = false;
		if(ioctl(dec_dwl->fd, VDEC_NOTIFY_HW_DISABLE) != 0){
			DWL_DEBUG("DWL: ioctl(VDEC_NOTIFY_HW_DISABLE) failed\n");
		}
	}
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLWaitHwReady
    Description     : Wait until hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR

    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 G1DWLWaitHwReady(const void *instance, u32 timeout)
{
	DWL_DEBUG("G1DWLWaitHwReady \n");
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
    Function name   : G1DWLmalloc
    Description     : Allocate a memory block. Same functionality as
                      the ANSI C malloc()

    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available

    Argument        : u32 n - Bytes to allocate
------------------------------------------------------------------------------*/
void *G1DWLmalloc(u32 n)
{
	return malloc((size_t) n);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLfree
    Description     : Deallocates or frees a memory block. Same functionality as
                      the ANSI C free()

    Return type     : void

    Argument        : void *p - Previously allocated memory block to be freed
------------------------------------------------------------------------------*/
void G1DWLfree(void *p)
{
	DWL_DEBUG("dwlFree\n");
    if(p != NULL)
        free(p);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLcalloc
    Description     : Allocates an array in memory with elements initialized
                      to 0. Same functionality as the ANSI C calloc()

    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available

    Argument        : u32 n - Number of elements
    Argument        : u32 s - Length in bytes of each element.
------------------------------------------------------------------------------*/
void *G1DWLcalloc(u32 n, u32 s)
{
	return calloc((size_t) n, (size_t) s);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLmemcpy
    Description     : Copies characters between buffers. Same functionality as
                      the ANSI C memcpy()

    Return type     : The value of destination d

    Argument        : void *d - Destination buffer
    Argument        : const void *s - Buffer to copy from
    Argument        : u32 n - Number of bytes to copy
------------------------------------------------------------------------------*/
void *G1DWLmemcpy(void *d, const void *s, u32 n)
{
    return memcpy(d, s, (size_t) n);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLmemset
    Description     : Sets buffers to a specified character. Same functionality
                      as the ANSI C memset()

    Return type     : The value of destination d

    Argument        : void *d - Pointer to destination
    Argument        : i32 c - Character to set
    Argument        : u32 n - Number of characters
------------------------------------------------------------------------------*/
void *G1DWLmemset(void *d, i32 c, u32 n)
{
    return memset(d, (int) c, (size_t) n);
}

/*------------------------------------------------------------------------------
    Function name   : G1G1DWLReleaseHw
    Description     :
    Return type     : void
    Argument        : const void *instance
------------------------------------------------------------------------------*/
void G1G1DWLReleaseHw(const void *instance)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *)instance;

	if(dec_dwl->clientType == DWL_CLIENT_TYPE_H264_DEC && dec_dwl->dvfsEna && dec_dwl->hwEna){
		dec_dwl->hwEna = false;
		if(ioctl(dec_dwl->fd, VDEC_NOTIFY_HW_DISABLE) != 0){
			DWL_DEBUG("DWL: ioctl(VDEC_NOTIFY_HW_DISABLE) failed\n");
		}
	}

	DWL_DEBUG("G1G1DWLReleaseHw+++\n");
	if(ioctl(dec_dwl->fd, HX170DEC_MUTEX_UNLOCK) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
    }
	DWL_DEBUG("G1G1DWLReleaseHw---\n");

	return;
}

void DWLSetLatency(const void *instance, int latencyMS)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *)instance;

    dec_dwl->latencyMS = latencyMS;
    dec_dwl->latencyValid = 1;

    return;
}
