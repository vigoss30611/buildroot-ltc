/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * NAL unit decoding. */

#include "hevc_nal_unit.h"
#include "hevc_util.h"

/* Decodes header of one NAL unit. */
u32 HevcDecodeNalUnit(struct StrmData *stream, struct NalUnit *nal_unit) {

  u32 tmp;

  ASSERT(stream);
  ASSERT(nal_unit);
  ASSERT(stream->bit_pos_in_word == 0);

  (void)DWLmemset(nal_unit, 0, sizeof(struct NalUnit));

  /* forbidden_zero_bit (not checked to be zero, errors ignored) */
  tmp = SwGetBits(stream, 1);

  tmp = SwGetBits(stream, 6);
  nal_unit->nal_unit_type = (enum NalUnitType)tmp;

  DEBUG_PRINT(("NAL TYPE %d\n", tmp));

  /* reserved_zero_6bits */
  tmp = SwGetBits(stream, 6);

  tmp = SwGetBits(stream, 3);
  nal_unit->temporal_id = tmp ? tmp - 1 : 0;

  return (HANTRO_OK);
}
