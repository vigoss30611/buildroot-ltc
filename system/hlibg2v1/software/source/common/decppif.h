/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef DECPPIF_H
#define DECPPIF_H

#include "basetype.h"

/* PP input types (pic_struct) */
/* Frame or top field */
#define DECPP_PIC_FRAME_OR_TOP_FIELD 0U
/* Bottom field only */
#define DECPP_PIC_BOT_FIELD 1U
/* top and bottom is separate locations */
#define DECPP_PIC_TOP_AND_BOT_FIELD 2U
/* top and bottom interleaved */
#define DECPP_PIC_TOP_AND_BOT_FIELD_FRAME 3U
/* interleaved top only */
#define DECPP_PIC_TOP_FIELD_FRAME 4U
/* interleaved bottom only */
#define DECPP_PIC_BOT_FIELD_FRAME 5U

/* Control interface between decoder and pp */
/* decoder writes, pp read-only */

typedef struct DecPpInterface_ {
  enum {
    DECPP_IDLE = 0,
    DECPP_RUNNING,         /* PP was started */
    DECPP_PIC_READY,       /* PP has finished a picture */
    DECPP_PIC_NOT_FINISHED /* PP still processing a picture */
  } pp_status; /* Decoder keeps track of what it asked the pp to do */

  enum {
    MULTIBUFFER_UNINIT = 0, /* buffering mode not yet decided */
    MULTIBUFFER_DISABLED,   /* Single buffer legacy mode */
    MULTIBUFFER_SEMIMODE,   /* enabled but full pipel cannot be used */
    MULTIBUFFER_FULLMODE    /* enabled and full pipeline successful */
  } multi_buf_stat;

  u32 input_bus_luma;
  u32 input_bus_chroma;
  u32 bottom_bus_luma;
  u32 bottom_bus_chroma;
  u32 pic_struct; /* structure of input picture */
  u32 top_field;
  u32 inwidth;
  u32 inheight;
  u32 use_pipeline;
  u32 little_endian;
  u32 word_swap;
  u32 cropped_w;
  u32 cropped_h;

  u32 buffer_index;  /* multibuffer, where to put PP output */
  u32 display_index; /* multibuffer, next picture in display order */
  u32 prev_anchor_display_index;

  /* VC-1 */
  u32 range_red;
  u32 range_map_yenable;
  u32 range_map_ycoeff;
  u32 range_map_cenable;
  u32 range_map_ccoeff;

  u32 tiled_input_mode;
  u32 progressive_sequence;

  u32 luma_stride;
  u32 chroma_stride;

} DecPpInterface;

/* Decoder asks with this struct information about pp setup */
/* pp writes, decoder read-only */

typedef struct DecPpQuery_ {
  /* Dec-to-PP direction */
  u32 tiled_mode;

  /* PP-to-Dec direction */
  u32 pipeline_accepted; /* PP accepts pipeline */
  u32 deinterlace;       /* Deinterlace feature used */
  u32 multi_buffer;      /* multibuffer PP output enabled */
  u32 pp_config_changed; /* PP config changed after previous output */
} DecPpQuery;

#endif
