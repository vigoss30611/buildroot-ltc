/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Video Usability Information decoding. */

#include "dwl.h"
#include "hevc_vui.h"
#include "basetype.h"
#include "hevc_exp_golomb.h"
#include "hevc_util.h"
#include "sw_stream.h"

/* Decode VUI parameters from the stream. See standard for details. */
u32 HevcDecodeVuiParameters(struct StrmData *stream,
                            struct VuiParameters *vui_parameters) {

  u32 tmp;

  ASSERT(stream);
  ASSERT(vui_parameters);

  (void)DWLmemset(vui_parameters, 0, sizeof(struct VuiParameters));

  tmp = SwGetBits(stream, 1);
  if (tmp == END_OF_STREAM) return (END_OF_STREAM);
  vui_parameters->aspect_ratio_present_flag = tmp;

  if (vui_parameters->aspect_ratio_present_flag) {
    tmp = SwGetBits(stream, 8);
    if (tmp == END_OF_STREAM) return (END_OF_STREAM);
    vui_parameters->aspect_ratio_idc = tmp;

    if (vui_parameters->aspect_ratio_idc == ASPECT_RATIO_EXTENDED_SAR) {
      tmp = SwGetBits(stream, 16);
      if (tmp == END_OF_STREAM) return (END_OF_STREAM);
      vui_parameters->sar_width = tmp;

      tmp = SwGetBits(stream, 16);
      if (tmp == END_OF_STREAM) return (END_OF_STREAM);
      vui_parameters->sar_height = tmp;
    }
  }

  /* overscan_info_present_flag */
  tmp = SwGetBits(stream, 1);
  if (tmp) {
    /* overscan_appropriate_flag */
    tmp = SwGetBits(stream, 1);
  }

  tmp = SwGetBits(stream, 1);
  vui_parameters->video_signal_type_present_flag = tmp;

  if (vui_parameters->video_signal_type_present_flag) {
    tmp = SwGetBits(stream, 3);
    vui_parameters->video_format = tmp;

    tmp = SwGetBits(stream, 1);
    vui_parameters->video_full_range_flag = tmp;

    tmp = SwGetBits(stream, 1);
    vui_parameters->colour_description_present_flag = tmp;

    if (vui_parameters->colour_description_present_flag) {
      tmp = SwGetBits(stream, 8);
      vui_parameters->colour_primaries = tmp;

      tmp = SwGetBits(stream, 8);
      vui_parameters->transfer_characteristics = tmp;

      tmp = SwGetBits(stream, 8);
      vui_parameters->matrix_coefficients = tmp;
    } else {
      vui_parameters->colour_primaries = 2;
      vui_parameters->transfer_characteristics = 2;
      vui_parameters->matrix_coefficients = 2;
    }
  } else {
    vui_parameters->video_format = 5;
    vui_parameters->colour_primaries = 2;
    vui_parameters->transfer_characteristics = 2;
    vui_parameters->matrix_coefficients = 2;
  }

  return (tmp != END_OF_STREAM ? HANTRO_OK : END_OF_STREAM);
}
