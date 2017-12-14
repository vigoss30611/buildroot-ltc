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
-
-  Description : ...
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp9hwd_headers.h"
#include "vp9hwd_probs.h"
#include "vp9hwd_container.h"

void Vp9ResetDecoder(struct Vp9Decoder *dec, struct DecAsicBuffers *asic_buff) {
  i32 i;

  /* Clear all previous segment data */
  DWLmemset(dec->segment_feature_enable, 0,
            sizeof(dec->segment_feature_enable));
  DWLmemset(dec->segment_feature_data, 0, sizeof(dec->segment_feature_data));

#ifndef USE_EXTERNAL_BUFFER
  if (asic_buff->segment_map[0].virtual_address)
    DWLmemset(asic_buff->segment_map[0].virtual_address, 0,
              asic_buff->segment_map_size);
  if (asic_buff->segment_map[1].virtual_address)
    DWLmemset(asic_buff->segment_map[1].virtual_address, 0,
              asic_buff->segment_map_size);
#else
if (asic_buff->segment_map.virtual_address)
    DWLmemset(asic_buff->segment_map.virtual_address, 0,
              asic_buff->segment_map.logical_size);
#endif
  Vp9ResetProbs(dec);

  dec->frame_context_idx = 0;
  dec->ref_frame_sign_bias[0] = 
  dec->ref_frame_sign_bias[1] = 
  dec->ref_frame_sign_bias[2] = 
  dec->ref_frame_sign_bias[3] = 0;
  dec->allow_comp_inter_inter = 0;
  for (i = 0; i < NUM_REF_FRAMES; i++) {
    dec->ref_frame_map[i] = i;
  }
}
