/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : post-processor external mode testbench
--
------------------------------------------------------------------------------*/
#ifndef TB_CFG_H
#define TB_CFG_H

#include "refbuffer.h"
#include "tb_defs.h"
#include "dwl.h"

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
typedef enum {
    TB_CFG_OK,
    TB_CFG_ERROR           = 500,
    TB_CFG_INVALID_BLOCK   = 501,
    TB_CFG_INVALID_PARAM   = 502,
    TB_CFG_INVALID_VALUE   = 503,
    TB_CFG_INVALID_CODE    = 504,
    TB_CFG_DUPLICATE_BLOCK = 505
} TBCfgCallbackResult;

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
typedef enum {
    TB_CFG_CALLBACK_BLK_START  = 300,
    TB_CFG_CALLBACK_VALUE      = 301,
} TBCfgCallbackParam;

/*------------------------------------------------------------------------------
    Interface to parsing
------------------------------------------------------------------------------*/
typedef TBCfgCallbackResult (*TBCfgCallback)(char*, char*, char*, TBCfgCallbackParam, void* );
TBCfgCallbackResult TBReadParam( char * block, char * key, char * value, TBCfgCallbackParam state, void *cbParam );

TBBool TBParseConfig( char * filename, TBCfgCallback callback, void *cbParam );
void TBSetDefaultCfg(TBCfg* tbCfg);
void TBPrintCfg(const TBCfg* tbCfg);
u32 TBCheckCfg(const TBCfg* tbCfg);

u32 TBGetPPDataDiscard(const TBCfg* tbCfg);
u32 TBGetPPClockGating(const TBCfg* tbCfg);
u32 TBGetPPWordSwap(const TBCfg* tbCfg);
u32 TBGetPPWordSwap16(const TBCfg* tbCfg);
u32 TBGetPPInputPictureEndian(const TBCfg* tbCfg);
u32 TBGetPPOutputPictureEndian(const TBCfg* tbCfg);

u32 TBGetDecErrorConcealment(const TBCfg* tbCfg);
u32 TBGetDecRlcModeForced(const TBCfg* tbCfg);
u32 TBGetDecMemoryAllocation(const TBCfg* tbCfg);
u32 TBGetDecDataDiscard(const TBCfg* tbCfg);
u32 TBGetDecClockGating(const TBCfg* tbCfg);
u32 TBGetDecOutputFormat(const TBCfg* tbCfg);
u32 TBGetDecOutputPictureEndian(const TBCfg* tbCfg);

u32 TBGetTBPacketByPacket(const TBCfg* tbCfg);
u32 TBGetTBNalUnitStream(const TBCfg* tbCfg);
u32 TBGetTBStreamHeaderCorrupt(const TBCfg* tbCfg);
u32 TBGetTBStreamTruncate(const TBCfg* tbCfg);
u32 TBGetTBSliceUdInPacket(const TBCfg* tbCfg);
u32 TBGetTBFirstTraceFrame(const TBCfg* tbCfg);

u32 TBGetDecRefbuEnabled(const TBCfg* tbCfg);
u32 TBGetDecRefbuDisableEvalMode(const TBCfg* tbCfg);
u32 TBGetDecRefbuDisableCheckpoint(const TBCfg* tbCfg);
u32 TBGetDecRefbuDisableOffset(const TBCfg* tbCfg);
u32 TBGetDec64BitEnable(const TBCfg* tbCfg);
u32 TBGetDecSupportNonCompliant(const TBCfg* tbCfg);
u32 TBGetDecIntraFreezeEnable(const TBCfg* tbCfg);
u32 TBGetDecDoubleBufferSupported(const TBCfg* tbCfg);
u32 TBGetDecTopBotSumSupported(const TBCfg* tbCfg);
void TBGetHwConfig( const TBCfg* tbCfg, DWLHwConfig_t *pHwCfg );
void TBSetRefbuMemModel( const TBCfg* tbCfg, u32 *regBase, refBuffer_t *pRefbu );
u32 TBGetDecForceMpeg4Idct(const TBCfg* tbCfg);
u32 TBGetDecCh8PixIleavSupported(const TBCfg* tbCfg);
void TBRefbuTestMode( refBuffer_t *pRefbu, u32 *regBase,
                      u32 isIntraFrame, u32 mode );

u32 TBGetDecApfThresholdEnabled( const TBCfg* tbCfg );

u32 TBGetDecServiceMergeDisable( const TBCfg* tbCfg );

#endif /* TB_CFG_H */
