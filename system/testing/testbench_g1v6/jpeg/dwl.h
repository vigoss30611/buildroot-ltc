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
--  Description : Sytem Wrapper Layer
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: dwl.h,v $
--  $Revision: 1.21 $
--  $Date: 2010/12/01 12:31:03 $
--
------------------------------------------------------------------------------*/
#ifndef __DWL_H__
#define __DWL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "basetype.h"
#include "decapicommon.h"

#define DWL_OK                      0
#define DWL_ERROR                  -1

#define DWL_HW_WAIT_OK              DWL_OK
#define DWL_HW_WAIT_ERROR           DWL_ERROR
#define DWL_HW_WAIT_TIMEOUT         1

#define DWL_CLIENT_TYPE_H264_DEC         1U
#define DWL_CLIENT_TYPE_MPEG4_DEC        2U
#define DWL_CLIENT_TYPE_JPEG_DEC         3U
#define DWL_CLIENT_TYPE_PP               4U
#define DWL_CLIENT_TYPE_VC1_DEC          5U
#define DWL_CLIENT_TYPE_MPEG2_DEC        6U
#define DWL_CLIENT_TYPE_VP6_DEC          7U
#define DWL_CLIENT_TYPE_AVS_DEC          9U /* TODO: fix */
#define DWL_CLIENT_TYPE_RV_DEC           8U
#define DWL_CLIENT_TYPE_VP8_DEC          10U

    /* Linear memory area descriptor */
    typedef struct DWLLinearMem
    {
        u32 *virtualAddress;
        u32 busAddress;
        u32 size;
        u32 uid;
    } DWLLinearMem_t;

    /* G1DWLInitParam is used to pass parameters when initializing the DWL */
    typedef struct G1DWLInitParam
    {
        u32 clientType;

        /* added by infotm for mmu control */
        u32 mmuEnable;
    } G1DWLInitParam_t;

    /* Hardware configuration description */

    typedef struct DWLHwConfig
    {
        u32 maxDecPicWidth;  /* Maximum video decoding width supported  */
        u32 maxPpOutPicWidth;   /* Maximum output width of Post-Processor */
        u32 h264Support;     /* HW supports h.264 */
        u32 jpegSupport;     /* HW supports JPEG */
        u32 mpeg4Support;    /* HW supports MPEG-4 */
        u32 customMpeg4Support; /* HW supports custom MPEG-4 features */
        u32 vc1Support;      /* HW supports VC-1 Simple */
        u32 mpeg2Support;    /* HW supports MPEG-2 */
        u32 ppSupport;       /* HW supports post-processor */
        u32 ppConfig;        /* HW post-processor functions bitmask */
        u32 sorensonSparkSupport;   /* HW supports Sorenson Spark */
        u32 refBufSupport;   /* HW supports reference picture buffering */
        u32 tiledModeSupport; /* HW supports tiled reference pictuers */
        u32 vp6Support;      /* HW supports VP6 */
        u32 vp7Support;      /* HW supports VP7 */
        u32 vp8Support;      /* HW supports VP8 */
        u32 avsSupport;      /* HW supports AVS */
        u32 jpegESupport;    /* HW supports JPEG extensions */
        u32 rvSupport;       /* HW supports REAL */
        u32 mvcSupport;      /* HW supports H264 MVC extension */
        u32 webpSupport;     /* HW supports WebP (VP8 snapshot) */
		u32 strideSupport;
    	i32 fieldDpbSupport;
		u32 ecSupport;
    } DWLHwConfig_t;

	typedef struct DWLHwFuseStatus
    {
        u32 h264SupportFuse;     /* HW supports h.264 */
        u32 mpeg4SupportFuse;    /* HW supports MPEG-4 */
        u32 mpeg2SupportFuse;    /* HW supports MPEG-2 */
        u32 sorensonSparkSupportFuse;   /* HW supports Sorenson Spark */
		u32 jpegSupportFuse;     /* HW supports JPEG */
        u32 vp6SupportFuse;      /* HW supports VP6 */
        u32 vp7SupportFuse;      /* HW supports VP6 */
        u32 vp8SupportFuse;      /* HW supports VP6 */
        u32 vc1SupportFuse;      /* HW supports VC-1 Simple */
		u32 jpegProgSupportFuse; /* HW supports Progressive JPEG */
        u32 ppSupportFuse;       /* HW supports post-processor */
        u32 ppConfigFuse;        /* HW post-processor functions bitmask */
        u32 maxDecPicWidthFuse;  /* Maximum video decoding width supported  */
        u32 maxPpOutPicWidthFuse; /* Maximum output width of Post-Processor */
        u32 refBufSupportFuse;   /* HW supports reference picture buffering */
		u32 avsSupportFuse;      /* one of the AVS values defined above */
		u32 rvSupportFuse;       /* one of the REAL values defined above */
		u32 mvcSupportFuse;
        u32 customMpeg4SupportFuse; /* Fuse for custom MPEG-4 */

    } DWLHwFuseStatus_t;

/* HW ID retriving, static implementation */
    u32 G1DWLReadAsicID(void);

/* HW configuration retrieving, static implementation */
    void DWLReadAsicConfig(DWLHwConfig_t * pHwCfg);

/* HW fuse retrieving, static implementation */
	void G1DWLReadAsicFuseStatus(DWLHwFuseStatus_t * pHwFuseSts);

/* DWL initilaization and release */
    const void *G1DWLInit(G1DWLInitParam_t * param);
    i32 G1DWLRelease(const void *instance);

/* HW sharing */
    i32 G1DWLReserveHw(const void *instance);
    void G1G1DWLReleaseHw(const void *instance);

/* Frame buffers memory */
    i32 G1DWLMallocRefFrm(const void *instance, u32 size, DWLLinearMem_t * info);
    void DWLFreeRefFrm(const void *instance, DWLLinearMem_t * info);

/* SW/HW shared memory */
    i32 G1DWLMallocLinear(const void *instance, u32 size, DWLLinearMem_t * info);
    void DWLFreeLinear(const void *instance, DWLLinearMem_t * info);

/* D-Cache coherence */
    void DWLDCacheRangeFlush(const void *instance, DWLLinearMem_t * info);  /* NOT in use */
    void DWLDCacheRangeRefresh(const void *instance, DWLLinearMem_t * info);    /* NOT in use */

/* Register access */
    void G1DWLWriteReg(const void *instance, u32 offset, u32 value);
    u32 G1DWLReadReg(const void *instance, u32 offset);

    void G1DWLWriteNV21Reg(const void *instance, u32 offset, u32 value);
    u32 G1DWLReadNV21Reg(const void *instance, u32 offset);

    void G1DWLWriteRegAll(const void *instance, const u32 * table, u32 size); /* NOT in use */
    void G1DWLReadRegAll(const void *instance, u32 * table, u32 size);    /* NOT in use */

/* HW starting/stopping */
    void DWLEnableHW(const void *instance, u32 offset, u32 value);
    void DWLDisableHW(const void *instance, u32 offset, u32 value);

/* HW synchronization */
    i32 G1DWLWaitHwReady(const void *instance, u32 timeout);

/* SW/SW shared memory */
    void *G1DWLmalloc(u32 n);
    void G1G1DWLfree(void *p);
    void *G1G1DWLcalloc(u32 n, u32 s);
    void *G1G1DWLmemcpy(void *d, const void *s, u32 n);
    void *G1G1DWLmemset(void *d, i32 c, u32 n);
  
/* VPU DVFS  */   
    void DWLLibReset(const void *instance);
    void DWLUpdateClk(const void *instance);
    void DWLSetLatency(const void *instance, int latencyMS);

#ifdef __cplusplus
}
#endif

#endif                       /* __DWL_H__ */
