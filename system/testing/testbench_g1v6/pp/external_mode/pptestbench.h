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
--  Abstract : post-processor testbench
--
------------------------------------------------------------------------------*/

#ifndef __PPTESTBENCH_H__
#define __PPTESTBENCH_H__

#include "basetype.h"
#include "ppapi.h"

#include "tb_cfg.h"
#include "defs.h"

enum {
    CFG_UPDATE_OK = 0,
    CFG_UPDATE_FAIL = 1,
    CFG_UPDATE_NEW = 2
};
int pp_startup(char *ppCfgFile, const void *decInst, u32 decType,
    const TBCfg* tbCfg);

int pp_update_config(const void *decInst, u32 decType, const TBCfg* tbCfg);

void pp_write_output(int frame, u32 fieldOutput, u32 top);
void pp_write_output_plain(int frame);

void pp_close(void);

int pp_external_run(char *cfgFile, const TBCfg* tbCfg);

int pp_alloc_buffers(PpParams *pp, u32 ablendCrop);
int pp_api_init(PPInst * pp, const void *decInst, u32 decType);
int pp_api_cfg(PPInst pp, const void *decInst, u32 decType,
    PpParams * params, const TBCfg* tbCfg);

void pipeline_disable(void);
int pp_set_rotation();
int pp_set_cropping(void);
void pp_vc1_specific( u32 multiresEnable, u32 rangered);
int pp_vc1_non_pipelined_pics(u32 busAddr, u32 width,
                               u32 height,  u32 rangeRed,
                               u32 rangeMapYEnable, u32 rangeMapYCoeff,
                               u32 rangeMapCEnable, u32 rangeMapCCoeff,
                               u32 *virtual, u32 dec_output_endian_big,
                               u32 dec_output_tiled);
void pp_set_input_interlaced(u32 isInterlaced);
u32 pp_rotation_used(void);
u32 pp_crop_used(void);
u32 pp_mpeg4_filter_used(void);

int pp_change_resolution(u32 width, u32 height, const TBCfg * tbCfg);
void pp_number_of_buffers(u32 amount);

char *pp_get_out_name(void);

#endif /* __PPTESTBENCH_H__ */
