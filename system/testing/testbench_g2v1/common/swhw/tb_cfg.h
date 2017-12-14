/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef TB_CFG_H
#define TB_CFG_H

#include "tb_defs.h"
#include "dwl.h"

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
enum TBCfgCallbackResult {
  TB_CFG_OK,
  TB_CFG_ERROR = 500,
  TB_CFG_INVALID_BLOCK = 501,
  TB_CFG_INVALID_PARAM = 502,
  TB_CFG_INVALID_VALUE = 503,
  TB_CFG_INVALID_CODE = 504,
  TB_CFG_DUPLICATE_BLOCK = 505
};

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
enum TBCfgCallbackParam {
  TB_CFG_CALLBACK_BLK_START = 300,
  TB_CFG_CALLBACK_VALUE = 301,
};

/*------------------------------------------------------------------------------
    Interface to parsing
------------------------------------------------------------------------------*/
typedef enum TBCfgCallbackResult (*TBCfgCallback)(char*, char*, char*,
                                                  enum TBCfgCallbackParam,
                                                  void*);
enum TBCfgCallbackResult TBReadParam(char* block, char* key, char* value,
                                     enum TBCfgCallbackParam state,
                                     void* cb_param);

TBBool TBParseConfig(char* filename, TBCfgCallback callback, void* cb_param);
void TBSetDefaultCfg(struct TBCfg* tb_cfg);
void TBPrintCfg(const struct TBCfg* tb_cfg);
u32 TBCheckCfg(const struct TBCfg* tb_cfg);

u32 TBGetPPDataDiscard(const struct TBCfg* tb_cfg);
u32 TBGetPPClockGating(const struct TBCfg* tb_cfg);
u32 TBGetPPWordSwap(const struct TBCfg* tb_cfg);
u32 TBGetPPWordSwap16(const struct TBCfg* tb_cfg);
u32 TBGetPPInputPictureEndian(const struct TBCfg* tb_cfg);
u32 TBGetPPOutputPictureEndian(const struct TBCfg* tb_cfg);

u32 TBGetDecErrorConcealment(const struct TBCfg* tb_cfg);
u32 TBGetDecRlcModeForced(const struct TBCfg* tb_cfg);
u32 TBGetDecMemoryAllocation(const struct TBCfg* tb_cfg);
u32 TBGetDecDataDiscard(const struct TBCfg* tb_cfg);
u32 TBGetDecClockGating(const struct TBCfg* tb_cfg);
u32 TBGetDecClockGatingRuntime(const struct TBCfg* tb_cfg);
u32 TBGetDecOutputFormat(const struct TBCfg* tb_cfg);
u32 TBGetDecOutputPictureEndian(const struct TBCfg* tb_cfg);

u32 TBGetTBPacketByPacket(const struct TBCfg* tb_cfg);
u32 TBGetTBNalUnitStream(const struct TBCfg* tb_cfg);
u32 TBGetTBStreamHeaderCorrupt(const struct TBCfg* tb_cfg);
u32 TBGetTBStreamTruncate(const struct TBCfg* tb_cfg);
u32 TBGetTBSliceUdInPacket(const struct TBCfg* tb_cfg);
u32 TBGetTBFirstTraceFrame(const struct TBCfg* tb_cfg);

u32 TBGetDec64BitEnable(const struct TBCfg* tb_cfg);
u32 TBGetDecSupportNonCompliant(const struct TBCfg* tb_cfg);
u32 TBGetDecIntraFreezeEnable(const struct TBCfg* tb_cfg);
u32 TBGetDecDoubleBufferSupported(const struct TBCfg* tb_cfg);
u32 TBGetDecTopBotSumSupported(const struct TBCfg* tb_cfg);
void TBGetHwConfig(const struct TBCfg* tb_cfg, DWLHwConfig* hw_cfg);
u32 TBGetDecForceMpeg4Idct(const struct TBCfg* tb_cfg);
u32 TBGetDecCh8PixIleavSupported(const struct TBCfg* tb_cfg);

u32 TBGetDecApfThresholdEnabled(const struct TBCfg* tb_cfg);

u32 TBGetDecServiceMergeDisable(const struct TBCfg* tb_cfg);

u32 TBGetDecBusWidth(const struct TBCfg* tb_cfg);

#endif /* TB_CFG_H */
