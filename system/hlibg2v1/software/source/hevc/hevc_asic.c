/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * SW/HW interface related definitions and functions. */

#include "basetype.h"
#include "regdrv.h"
#include "hevc_asic.h"
#include "hevc_container.h"
#include "hevc_util.h"
#include "dwl.h"
#include "commonconfig.h"

static void HevcStreamPosUpdate(struct HevcDecContainer *dec_cont);

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...) \
  do {                     \
  } while (0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...) printf(__VA_ARGS__)
#endif

const u32 ref_ybase[16] = {
    HWIF_REFER0_YBASE_LSB,  HWIF_REFER1_YBASE_LSB,  HWIF_REFER2_YBASE_LSB,
    HWIF_REFER3_YBASE_LSB,  HWIF_REFER4_YBASE_LSB,  HWIF_REFER5_YBASE_LSB,
    HWIF_REFER6_YBASE_LSB,  HWIF_REFER7_YBASE_LSB,  HWIF_REFER8_YBASE_LSB,
    HWIF_REFER9_YBASE_LSB,  HWIF_REFER10_YBASE_LSB, HWIF_REFER11_YBASE_LSB,
    HWIF_REFER12_YBASE_LSB, HWIF_REFER13_YBASE_LSB, HWIF_REFER14_YBASE_LSB,
    HWIF_REFER15_YBASE_LSB};

const u32 ref_cbase[16] = {
    HWIF_REFER0_CBASE_LSB,  HWIF_REFER1_CBASE_LSB,  HWIF_REFER2_CBASE_LSB,
    HWIF_REFER3_CBASE_LSB,  HWIF_REFER4_CBASE_LSB,  HWIF_REFER5_CBASE_LSB,
    HWIF_REFER6_CBASE_LSB,  HWIF_REFER7_CBASE_LSB,  HWIF_REFER8_CBASE_LSB,
    HWIF_REFER9_CBASE_LSB,  HWIF_REFER10_CBASE_LSB, HWIF_REFER11_CBASE_LSB,
    HWIF_REFER12_CBASE_LSB, HWIF_REFER13_CBASE_LSB, HWIF_REFER14_CBASE_LSB,
    HWIF_REFER15_CBASE_LSB};

const u32 ref_dbase[16] = {
    HWIF_REFER0_DBASE_LSB,  HWIF_REFER1_DBASE_LSB,  HWIF_REFER2_DBASE_LSB,
    HWIF_REFER3_DBASE_LSB,  HWIF_REFER4_DBASE_LSB,  HWIF_REFER5_DBASE_LSB,
    HWIF_REFER6_DBASE_LSB,  HWIF_REFER7_DBASE_LSB,  HWIF_REFER8_DBASE_LSB,
    HWIF_REFER9_DBASE_LSB,  HWIF_REFER10_DBASE_LSB, HWIF_REFER11_DBASE_LSB,
    HWIF_REFER12_DBASE_LSB, HWIF_REFER13_DBASE_LSB, HWIF_REFER14_DBASE_LSB,
    HWIF_REFER15_DBASE_LSB};
#ifdef NEW_REG
const u32 ref_tybase[16] = {
    HWIF_REFER0_TYBASE_LSB, HWIF_REFER1_TYBASE_LSB, HWIF_REFER2_TYBASE_LSB,
    HWIF_REFER3_TYBASE_LSB, HWIF_REFER4_TYBASE_LSB, HWIF_REFER5_TYBASE_LSB,
    HWIF_REFER6_TYBASE_LSB, HWIF_REFER7_TYBASE_LSB, HWIF_REFER8_TYBASE_LSB,
    HWIF_REFER9_TYBASE_LSB, HWIF_REFER10_TYBASE_LSB, HWIF_REFER11_TYBASE_LSB,
    HWIF_REFER12_TYBASE_LSB, HWIF_REFER13_TYBASE_LSB, HWIF_REFER14_TYBASE_LSB,
    HWIF_REFER15_TYBASE_LSB};
const u32 ref_tcbase[16] = {
    HWIF_REFER0_TCBASE_LSB, HWIF_REFER1_TCBASE_LSB, HWIF_REFER2_TCBASE_LSB,
    HWIF_REFER3_TCBASE_LSB, HWIF_REFER4_TCBASE_LSB, HWIF_REFER5_TCBASE_LSB,
    HWIF_REFER6_TCBASE_LSB, HWIF_REFER7_TCBASE_LSB, HWIF_REFER8_TCBASE_LSB,
    HWIF_REFER9_TCBASE_LSB, HWIF_REFER10_TCBASE_LSB, HWIF_REFER11_TCBASE_LSB,
    HWIF_REFER12_TCBASE_LSB, HWIF_REFER13_TCBASE_LSB, HWIF_REFER14_TCBASE_LSB,
    HWIF_REFER15_TCBASE_LSB};
#endif
/*------------------------------------------------------------------------------
    Reference list initialization
------------------------------------------------------------------------------*/
#define IS_SHORT_TERM_FRAME(a) ((a).status == SHORT_TERM)
#define IS_LONG_TERM_FRAME(a) ((a).status == LONG_TERM)
#define IS_REF_FRAME(a) ((a).status &&(a).status != EMPTY)

#define INVALID_IDX 0xFFFFFFFF
#define MIN_POC 0x80000000
#define MAX_POC 0x7FFFFFFF

const u32 ref_pic_list0[16] = {
    HWIF_INIT_RLIST_F0,  HWIF_INIT_RLIST_F1,  HWIF_INIT_RLIST_F2,
    HWIF_INIT_RLIST_F3,  HWIF_INIT_RLIST_F4,  HWIF_INIT_RLIST_F5,
    HWIF_INIT_RLIST_F6,  HWIF_INIT_RLIST_F7,  HWIF_INIT_RLIST_F8,
    HWIF_INIT_RLIST_F9,  HWIF_INIT_RLIST_F10, HWIF_INIT_RLIST_F11,
    HWIF_INIT_RLIST_F12, HWIF_INIT_RLIST_F13, HWIF_INIT_RLIST_F14,
    HWIF_INIT_RLIST_F15};

const u32 ref_pic_list1[16] = {
    HWIF_INIT_RLIST_B0,  HWIF_INIT_RLIST_B1,  HWIF_INIT_RLIST_B2,
    HWIF_INIT_RLIST_B3,  HWIF_INIT_RLIST_B4,  HWIF_INIT_RLIST_B5,
    HWIF_INIT_RLIST_B6,  HWIF_INIT_RLIST_B7,  HWIF_INIT_RLIST_B8,
    HWIF_INIT_RLIST_B9,  HWIF_INIT_RLIST_B10, HWIF_INIT_RLIST_B11,
    HWIF_INIT_RLIST_B12, HWIF_INIT_RLIST_B13, HWIF_INIT_RLIST_B14,
    HWIF_INIT_RLIST_B15};

static const u32 ref_poc_regs[16] = {
    HWIF_CUR_POC_00, HWIF_CUR_POC_01, HWIF_CUR_POC_02, HWIF_CUR_POC_03,
    HWIF_CUR_POC_04, HWIF_CUR_POC_05, HWIF_CUR_POC_06, HWIF_CUR_POC_07,
    HWIF_CUR_POC_08, HWIF_CUR_POC_09, HWIF_CUR_POC_10, HWIF_CUR_POC_11,
    HWIF_CUR_POC_12, HWIF_CUR_POC_13, HWIF_CUR_POC_14, HWIF_CUR_POC_15};

#ifndef USE_EXTERNAL_BUFFER
u32 AllocateAsicBuffers(struct HevcDecContainer *dec_cont,
                        struct HevcDecAsic *asic_buff) {
  i32 ret = 0;
  u32 size;

  /* cabac tables and scaling lists in separate bases, but memory allocated
   * in one chunk */
  size = ASIC_SCALING_LIST_SIZE;
  ret |= DWLMallocLinear(dec_cont->dwl, size, &asic_buff->scaling_lists);
  /* max number of tiles times width and height (2 bytes each),
   * rounding up to next 16 bytes boundary + one extra 16 byte
   * chunk (HW guys wanted to have this) */
  size = (MAX_TILE_COLS * MAX_TILE_ROWS * 4 * sizeof(u16) + 15 + 16) & ~0xF;
  ret |= DWLMallocLinear(dec_cont->dwl, size, &asic_buff->tile_info);

  if (ret != 0)
    return 1;
  else
    return 0;
}

void ReleaseAsicBuffers(const void *dwl, struct HevcDecAsic *asic_buff) {
  if (asic_buff->scaling_lists.virtual_address != NULL) {
    DWLFreeLinear(dwl, &asic_buff->scaling_lists);
    asic_buff->scaling_lists.virtual_address = NULL;
  }
  if (asic_buff->tile_info.virtual_address != NULL) {
    DWLFreeLinear(dwl, &asic_buff->tile_info);
    asic_buff->tile_info.virtual_address = NULL;
  }
  if (asic_buff->filter_mem.virtual_address != NULL) {
    DWLFreeLinear(dwl, &asic_buff->filter_mem);
    asic_buff->filter_mem.virtual_address = NULL;
  }
  if (asic_buff->bsd_control_mem.virtual_address != NULL) {
    DWLFreeLinear(dwl, &asic_buff->bsd_control_mem);
    asic_buff->bsd_control_mem.virtual_address = NULL;
  }
}
#else
u32 AllocateAsicBuffers(struct HevcDecContainer *dec_cont,
                        struct HevcDecAsic *asic_buff) {
//  i32 ret = 0;
  u32 size;

  /* cabac tables and scaling lists in separate bases, but memory allocated
   * in one chunk */
  size = ASIC_SCALING_LIST_SIZE;
  /* max number of tiles times width and height (2 bytes each),
   * rounding up to next 16 bytes boundary + one extra 16 byte
   * chunk (HW guys wanted to have this) */
  size += (MAX_TILE_COLS * MAX_TILE_ROWS * 4 * sizeof(u16) + 15 + 16) & ~0xF;
  asic_buff->scaling_lists_offset = 0;
  asic_buff->tile_info_offset = ASIC_SCALING_LIST_SIZE;

  if (asic_buff->misc_linear.virtual_address == NULL) {
    if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, MISC_LINEAR_BUFFER)) {
      dec_cont->next_buf_size = size;
      dec_cont->buf_to_free = NULL;
      dec_cont->buf_type = MISC_LINEAR_BUFFER;
      dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
      return DEC_WAITING_FOR_BUFFER;
    } else {
      i32 ret = 0;
      ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->misc_linear);

      if (ret) return 1;
    }
  }

  return 0;
}

i32 ReleaseAsicBuffers(struct HevcDecContainer *dec_cont, struct HevcDecAsic *asic_buff) {
  const void *dwl = dec_cont->dwl;
  if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, MISC_LINEAR_BUFFER)) {
    if (asic_buff->misc_linear.virtual_address != NULL) {
      DWLFreeLinear(dwl, &asic_buff->misc_linear);
      asic_buff->misc_linear.virtual_address = NULL;
      asic_buff->misc_linear.size = 0;
    }
  }
  return 0;
}
#endif

#ifndef USE_EXTERNAL_BUFFER
u32 AllocateAsicTileEdgeMems(struct HevcDecContainer *dec_cont) {
  struct HevcDecAsic *asic_buff = dec_cont->asic_buff;
  u32 num_tile_cols = dec_cont->storage.active_pps->tile_info.num_tile_columns;
  struct SeqParamSet *sps = dec_cont->storage.active_sps;
  u32 pixel_width = (sps->bit_depth_luma == 8 && sps->bit_depth_chroma == 8) ? 8 : 10;
  if (num_tile_cols < 2) return HANTRO_OK;

  u32 height64 = (dec_cont->storage.active_sps->pic_height + 63) & ~63;
  u32 size = ASIC_VERT_FILTER_RAM_SIZE * height64 * (num_tile_cols - 1);
  size = size * pixel_width / 8;
  if (asic_buff->filter_mem.size >= size) return HANTRO_OK;

  /* If already allocated, release the old, too small buffers. */
  ReleaseAsicTileEdgeMems(dec_cont);

  i32 dwl_ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->filter_mem);
  if (dwl_ret != DWL_OK) {
    ReleaseAsicTileEdgeMems(dec_cont);
    return HANTRO_NOK;
  }

  size = ASIC_BSD_CTRL_RAM_SIZE * height64 * (num_tile_cols - 1);
  dwl_ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->bsd_control_mem);
  if (dwl_ret != DWL_OK) {
    ReleaseAsicTileEdgeMems(dec_cont);
    return HANTRO_NOK;
  }

  size = ASIC_VERT_SAO_RAM_SIZE * height64 * (num_tile_cols - 1);
  size = size * pixel_width / 8;
  dwl_ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->sao_mem);
  if (dwl_ret != DWL_OK) {
    ReleaseAsicTileEdgeMems(dec_cont);
    return HANTRO_NOK;
  }

  return HANTRO_OK;
}

void ReleaseAsicTileEdgeMems(struct HevcDecContainer *dec_cont) {
  struct HevcDecAsic *asic_buff = dec_cont->asic_buff;
  if (asic_buff->filter_mem.virtual_address != NULL) {
    DWLFreeLinear(dec_cont->dwl, &asic_buff->filter_mem);
    asic_buff->filter_mem.virtual_address = NULL;
    asic_buff->filter_mem.size = 0;
  }
  if (asic_buff->bsd_control_mem.virtual_address != NULL) {
    DWLFreeLinear(dec_cont->dwl, &asic_buff->bsd_control_mem);
    asic_buff->bsd_control_mem.virtual_address = NULL;
    asic_buff->bsd_control_mem.size = 0;
  }
  if (asic_buff->sao_mem.virtual_address != NULL) {
    DWLFreeLinear(dec_cont->dwl, &asic_buff->sao_mem);
    asic_buff->sao_mem.virtual_address = NULL;
    asic_buff->sao_mem.size = 0;
  }
}
#else

u32 AllocateAsicTileEdgeMems(struct HevcDecContainer *dec_cont) {
  struct HevcDecAsic *asic_buff = dec_cont->asic_buff;
  u32 num_tile_cols = dec_cont->storage.active_pps->tile_info.num_tile_columns;
  struct SeqParamSet *sps = dec_cont->storage.active_sps;
  u32 pixel_width = (sps->bit_depth_luma == 8 && sps->bit_depth_chroma == 8) ? 8 : 10;
  if (num_tile_cols < 2) return HANTRO_OK;

  u32 height64 = (dec_cont->storage.active_sps->pic_height + 63) & ~63;
  u32 size = ASIC_VERT_FILTER_RAM_SIZE * height64 * (num_tile_cols - 1) * pixel_width / 8
           + ASIC_BSD_CTRL_RAM_SIZE * height64 * (num_tile_cols - 1)
           + ASIC_VERT_SAO_RAM_SIZE * height64 * (num_tile_cols - 1) * pixel_width / 8;
  if (asic_buff->tile_edge.size >= size) return HANTRO_OK;

  if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, TILE_EDGE_BUFFER)) {
    /* If already allocated, release the old, too small buffers. */
    dec_cont->buf_to_free = NULL;
    if (asic_buff->tile_edge.virtual_address != NULL) {
      dec_cont->buf_to_free = &asic_buff->tile_edge;
      dec_cont->next_buf_size = 0;
    }

    dec_cont->next_buf_size = size;
    dec_cont->buf_type = TILE_EDGE_BUFFER;
    dec_cont->buf_num = 1;
#ifdef ASIC_TRACE_SUPPORT
      dec_cont->is_frame_buffer = 0;
#endif
    asic_buff->filter_mem_offset = 0;
    asic_buff->bsd_control_mem_offset = asic_buff->filter_mem_offset
                                        + ASIC_VERT_FILTER_RAM_SIZE * height64 * (num_tile_cols - 1) * pixel_width / 8;
    asic_buff->sao_mem_offset = asic_buff->bsd_control_mem_offset + ASIC_BSD_CTRL_RAM_SIZE * height64 * (num_tile_cols - 1);
  } else {
    ReleaseAsicTileEdgeMems(dec_cont);

    i32 dwl_ret = DWLMallocLinear(dec_cont->dwl, size, &asic_buff->tile_edge);
    if (dwl_ret != DWL_OK) {
      return HANTRO_NOK;
    }
    return HANTRO_OK;
  }

  return DEC_WAITING_FOR_BUFFER;
}

void ReleaseAsicTileEdgeMems(struct HevcDecContainer *dec_cont) {
  struct HevcDecAsic *asic_buff = dec_cont->asic_buff;
  if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, TILE_EDGE_BUFFER)) {
    if (asic_buff->tile_edge.virtual_address != NULL) {
      DWLFreeLinear(dec_cont->dwl, &asic_buff->tile_edge);
      asic_buff->tile_edge.virtual_address = NULL;
      asic_buff->tile_edge.size = 0;
    }
  }
}

#endif

u32 HevcRunAsic(struct HevcDecContainer *dec_cont,
                struct HevcDecAsic *asic_buff) {

  u32 asic_status = 0;
  i32 ret = 0;

  /* start new picture */
  if (!dec_cont->asic_running) {
    u32 reserve_ret = 0;

    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_OUT_DIS,
                   asic_buff->disable_out_writing);

    reserve_ret = DWLReserveHw(dec_cont->dwl, &dec_cont->core_id);
    if (reserve_ret != DWL_OK) {
      return X170_DEC_HW_RESERVED;
    }

    dec_cont->asic_running = 1;

    FlushDecRegisters(dec_cont->dwl, dec_cont->core_id, dec_cont->hevc_regs);

    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_E, 1);
    DWLEnableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1,
                dec_cont->hevc_regs[1]);
  } else /* buffer was empty and now we restart with new stream values */
  {
    HevcStreamPosUpdate(dec_cont);

    /* HWIF_STRM_START_BIT */
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 5,
                dec_cont->hevc_regs[5]);
    /* HWIF_STREAM_DATA_LEN */
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 6,
                dec_cont->hevc_regs[6]);
    /* HWIF_STREAM_BASE_MSB */
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 168,
                dec_cont->hevc_regs[168]);
    /* HWIF_STREAM_BASE_LSB */
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 169,
                dec_cont->hevc_regs[169]);
    /* HWIF_STREAM_BUFFER_LEN */
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 257,
                dec_cont->hevc_regs[257]);
    /* HWIF_STREAM_START_OFFSET */
    DWLWriteReg(dec_cont->dwl, dec_cont->core_id, 4 * 258,
                dec_cont->hevc_regs[258]);

    /* start HW by clearing IRQ_BUFFER_EMPTY status bit */
    DWLEnableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1,
                dec_cont->hevc_regs[1]);
  }

  ret = DWLWaitHwReady(dec_cont->dwl, dec_cont->core_id,
                       (u32)DEC_X170_TIMEOUT_LENGTH);

  if (ret != DWL_HW_WAIT_OK) {
    ERROR_PRINT("DWLWaitHwReady");
    DEBUG_PRINT(("DWLWaitHwReady returned: %d\n", ret));

    /* Reset HW */
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ_STAT, 0);
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ, 0);

    DWLDisableHw(dec_cont->dwl, dec_cont->core_id, 4 * 1,
                 dec_cont->hevc_regs[1]);

    dec_cont->asic_running = 0;
    DWLReleaseHw(dec_cont->dwl, dec_cont->core_id);

    return (ret == DWL_HW_WAIT_ERROR) ? X170_DEC_SYSTEM_ERROR
                                      : X170_DEC_TIMEOUT;
  }

  RefreshDecRegisters(dec_cont->dwl, dec_cont->core_id, dec_cont->hevc_regs);

  /* React to the HW return value */

  asic_status = GetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ_STAT);

  SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ_STAT, 0);
  SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_IRQ, 0); /* just in case */

  if (!(asic_status & DEC_HW_IRQ_BUFFER)) {
    /* HW done, release it! */
    dec_cont->asic_running = 0;

    DWLReleaseHw(dec_cont->dwl, dec_cont->core_id);
  }

  {
    addr_t last_read_address;
    u32 bytes_processed;
    const addr_t start_address =
        dec_cont->hw_stream_start_bus & (~DEC_HW_ALIGN_MASK);
    const u32 offset_bytes = dec_cont->hw_stream_start_bus & DEC_HW_ALIGN_MASK;

    last_read_address =
        GetDecRegister(dec_cont->hevc_regs, HWIF_STREAM_BASE_LSB);
#ifdef USE_64BIT_ENV
    last_read_address |= ((addr_t)GetDecRegister(dec_cont->hevc_regs, HWIF_STREAM_BASE_MSB))<<32;
#endif

    if(last_read_address <= start_address)
      bytes_processed = dec_cont->hw_buffer_length - (u32)(start_address - last_read_address);
    else
      bytes_processed = (u32)(last_read_address - start_address);

    DEBUG_PRINT(
        ("HW updated stream position: %08x\n"
         "           processed bytes: %8d\n"
         "     of which offset bytes: %8d\n",
         last_read_address, bytes_processed, offset_bytes));

    /* from start of the buffer add what HW has decoded */
    /* end - start smaller or equal than maximum */
    if ((bytes_processed - offset_bytes) > dec_cont->hw_length) {

        if ((asic_status & DEC_HW_IRQ_RDY) || (asic_status & DEC_HW_IRQ_BUFFER)) {
            DEBUG_PRINT(("New stream position out of range!\n"));
            //ASSERT(0);
            dec_cont->hw_stream_start += dec_cont->hw_length;
            dec_cont->hw_stream_start_bus += dec_cont->hw_length;
            dec_cont->hw_length = 0; /* no bytes left */
            dec_cont->stream_pos_updated = 1;
            if(asic_status & DEC_HW_IRQ_BUFFER) {
                dec_cont->asic_running = 0;
                DWLReleaseHw(dec_cont->dwl, dec_cont->core_id);
            }
            return DEC_HW_IRQ_ERROR;
      }

      /* consider all buffer processed */
      dec_cont->hw_stream_start += dec_cont->hw_length;
      dec_cont->hw_stream_start_bus += dec_cont->hw_length;
      dec_cont->hw_length = 0; /* no bytes left */
    } else {
      dec_cont->hw_length -= (bytes_processed - offset_bytes);
      dec_cont->hw_stream_start += (bytes_processed - offset_bytes);
      dec_cont->hw_stream_start_bus += (bytes_processed - offset_bytes);
    }

    /* if turnaround */
    if(dec_cont->hw_stream_start > (dec_cont->hw_buffer + dec_cont->hw_buffer_length)){
      dec_cont->hw_stream_start -= dec_cont->hw_buffer_length;
      dec_cont->hw_stream_start_bus -= dec_cont->hw_buffer_length;
    }
    /* else will continue decoding from the beginning of buffer */

    dec_cont->stream_pos_updated = 1;
  }

  return asic_status;
}

void HevcSetRegs(struct HevcDecContainer *dec_cont) {
  u32 i, tmp;
  u32 long_term_flags;
  i8 poc_diff;
  struct HevcDecAsic *asic_buff = dec_cont->asic_buff;

  struct SeqParamSet *sps = dec_cont->storage.active_sps;
  struct PicParamSet *pps = dec_cont->storage.active_pps;
  const struct SliceHeader *slice_header = dec_cont->storage.slice_header;
  const struct DpbStorage *dpb = dec_cont->storage.dpb;
  const struct Storage *storage = &dec_cont->storage;
  u32 pixel_width = (sps->bit_depth_luma == 8 && sps->bit_depth_chroma == 8) ? 8 : 10;
  u32 rs_pixel_width = (dec_cont->use_8bits_output || pixel_width == 8) ? 8 :
                       dec_cont->use_p010_output ? 16 : 10;

  /* not supported yet */
  SetDecRegister(dec_cont->hevc_regs, HWIF_BLACKWHITE_E, 0);
  /*sps->chroma_format_idc == 0);*/

  SetDecRegister(dec_cont->hevc_regs, HWIF_BIT_DEPTH_Y_MINUS8,
                 sps->bit_depth_luma - 8);
  SetDecRegister(dec_cont->hevc_regs, HWIF_BIT_DEPTH_C_MINUS8,
                 sps->bit_depth_chroma - 8);
  SetDecRegister(dec_cont->hevc_regs, HWIF_OUTPUT_8_BITS, dec_cont->use_8bits_output);
  SetDecRegister(dec_cont->hevc_regs,
                 HWIF_OUTPUT_FORMAT,
                 dec_cont->use_p010_output ? 1 :
                 (dec_cont->pixel_format == DEC_OUT_PIXEL_CUSTOMER1 ? 2 : 0));

  SetDecRegister(dec_cont->hevc_regs, HWIF_PCM_E, sps->pcm_enabled);
  SetDecRegister(dec_cont->hevc_regs, HWIF_PCM_BITDEPTH_Y,
                 sps->pcm_bit_depth_luma);
  SetDecRegister(dec_cont->hevc_regs, HWIF_PCM_BITDEPTH_C,
                 sps->pcm_bit_depth_chroma);

  SetDecRegister(dec_cont->hevc_regs, HWIF_REFPICLIST_MOD_E,
                 pps->lists_modification_present_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_MIN_CB_SIZE,
                 sps->log_min_coding_block_size);
  SetDecRegister(dec_cont->hevc_regs, HWIF_MAX_CB_SIZE,
                 sps->log_max_coding_block_size);
  SetDecRegister(dec_cont->hevc_regs, HWIF_MIN_TRB_SIZE,
                 sps->log_min_transform_block_size);
  SetDecRegister(dec_cont->hevc_regs, HWIF_MAX_TRB_SIZE,
                 sps->log_max_transform_block_size);
  SetDecRegister(dec_cont->hevc_regs, HWIF_MIN_PCM_SIZE,
                 sps->log_min_pcm_block_size);
  SetDecRegister(dec_cont->hevc_regs, HWIF_MAX_PCM_SIZE,
                 sps->log_max_pcm_block_size);

  SetDecRegister(dec_cont->hevc_regs, HWIF_MAX_INTER_HIERDEPTH,
                 sps->max_transform_hierarchy_depth_inter);
  SetDecRegister(dec_cont->hevc_regs, HWIF_MAX_INTRA_HIERDEPTH,
                 sps->max_transform_hierarchy_depth_intra);

  SetDecRegister(dec_cont->hevc_regs, HWIF_ASYM_PRED_E,
                 sps->asymmetric_motion_partitions_enable);
  SetDecRegister(dec_cont->hevc_regs, HWIF_SAO_E,
                 sps->sample_adaptive_offset_enable);

  SetDecRegister(dec_cont->hevc_regs, HWIF_PCM_FILT_DISABLE,
                 sps->pcm_loop_filter_disable);

  SetDecRegister(
      dec_cont->hevc_regs, HWIF_TEMPOR_MVP_E,
      sps->temporal_mvp_enable && !IS_IDR_NAL_UNIT(storage->prev_nal_unit));

  SetDecRegister(dec_cont->hevc_regs, HWIF_SIGN_DATA_HIDE,
                 pps->sign_data_hiding_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_CABAC_INIT_PRESENT,
                 pps->cabac_init_present);

  SetDecRegister(dec_cont->hevc_regs, HWIF_REFIDX0_ACTIVE,
                 pps->num_ref_idx_l0_active);
  SetDecRegister(dec_cont->hevc_regs, HWIF_REFIDX1_ACTIVE,
                 pps->num_ref_idx_l1_active);

  if (dec_cont->legacy_regs)
    SetDecRegister(dec_cont->hevc_regs, HWIF_INIT_QP_V0, pps->pic_init_qp);
  else
    SetDecRegister(dec_cont->hevc_regs, HWIF_INIT_QP, pps->pic_init_qp);

  SetDecRegister(dec_cont->hevc_regs, HWIF_CONST_INTRA_E,
                 pps->constrained_intra_pred_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_TRANSFORM_SKIP_E,
                 pps->transform_skip_enable);

  SetDecRegister(dec_cont->hevc_regs, HWIF_CU_QPD_E, pps->cu_qp_delta_enabled);

  SetDecRegister(dec_cont->hevc_regs, HWIF_MAX_CU_QPD_DEPTH,
                 pps->diff_cu_qp_delta_depth);

  SetDecRegister(dec_cont->hevc_regs, HWIF_CH_QP_OFFSET,
                 asic_buff->chroma_qp_index_offset);
  SetDecRegister(dec_cont->hevc_regs, HWIF_CH_QP_OFFSET2,
                 asic_buff->chroma_qp_index_offset2);

  SetDecRegister(dec_cont->hevc_regs, HWIF_SLICE_CHQP_FLAG,
                 pps->slice_level_chroma_qp_offsets_present);

  SetDecRegister(dec_cont->hevc_regs, HWIF_WEIGHT_PRED_E,
                 pps->weighted_pred_flag);
  SetDecRegister(dec_cont->hevc_regs, HWIF_WEIGHT_BIPR_IDC,
                 pps->weighted_bi_pred_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_TRANSQ_BYPASS_E,
                 pps->trans_quant_bypass_enable);

  SetDecRegister(dec_cont->hevc_regs, HWIF_DEPEND_SLICE_E,
                 pps->dependent_slice_segments_enabled);

  SetDecRegister(dec_cont->hevc_regs, HWIF_SLICE_HDR_EBITS,
                 pps->num_extra_slice_header_bits);

  SetDecRegister(dec_cont->hevc_regs, HWIF_STRONG_SMOOTH_E,
                 sps->strong_intra_smoothing_enable);

#ifndef USE_EXTERNAL_BUFFER
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_TILE_BASE,
               asic_buff->tile_info.bus_address);
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_VERT_FILT_BASE,
               asic_buff->filter_mem.bus_address);
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_VERT_SAO_BASE,
               asic_buff->sao_mem.bus_address);
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_BSD_CTRL_BASE,
               asic_buff->bsd_control_mem.bus_address);
#else
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_TILE_BASE,
               asic_buff->misc_linear.bus_address + asic_buff->tile_info_offset);
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_VERT_FILT_BASE,
               asic_buff->tile_edge.bus_address + asic_buff->filter_mem_offset);
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_VERT_SAO_BASE,
               asic_buff->tile_edge.bus_address + asic_buff->sao_mem_offset);
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_BSD_CTRL_BASE,
               asic_buff->tile_edge.bus_address + asic_buff->bsd_control_mem_offset);
#endif
  SetDecRegister(dec_cont->hevc_regs, HWIF_TILE_ENABLE,
                 pps->tiles_enabled_flag);

  if (pps->tiles_enabled_flag) {
    u32 j, h;
#ifndef USE_EXTERNAL_BUFFER
    u16 *p = (u16 *)asic_buff->tile_info.virtual_address;
#else
    u16 *p = (u16 *)((u8 *)asic_buff->misc_linear.virtual_address + asic_buff->tile_info_offset);
#endif
  if (dec_cont->legacy_regs) {
    SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_COLS_V0,
                   pps->tile_info.num_tile_columns);
    SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_ROWS_V0,
                   pps->tile_info.num_tile_rows);
  } else {
    SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_COLS,
                   pps->tile_info.num_tile_columns);
    SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_ROWS,
                   pps->tile_info.num_tile_rows);
  }

    /* write width + height for each tile in pic */
    if (!pps->tile_info.uniform_spacing) {
      u32 tmp_w = 0, tmp_h = 0;

      for (i = 0; i < pps->tile_info.num_tile_rows; i++) {
        if (i == pps->tile_info.num_tile_rows - 1)
          h = storage->pic_height_in_ctbs - tmp_h;
        else
          h = pps->tile_info.row_height[i];
        tmp_h += h;

        for (j = 0, tmp_w = 0; j < pps->tile_info.num_tile_columns - 1; j++) {
          tmp_w += pps->tile_info.col_width[j];
          *p++ = pps->tile_info.col_width[j];
          *p++ = h;
        }
        /* last column */
        *p++ = storage->pic_width_in_ctbs - tmp_w;
        *p++ = h;
      }
    } else /* uniform spacing */
    {
      u32 prev_h, prev_w;

      for (i = 0, prev_h = 0; i < pps->tile_info.num_tile_rows; i++) {
        tmp = (i + 1) * storage->pic_height_in_ctbs /
              pps->tile_info.num_tile_rows;
        h = tmp - prev_h;
        prev_h = tmp;

        for (j = 0, prev_w = 0; j < pps->tile_info.num_tile_columns; j++) {
          tmp = (j + 1) * storage->pic_width_in_ctbs /
                pps->tile_info.num_tile_columns;
          *p++ = tmp - prev_w;
          *p++ = h;
          prev_w = tmp;
        }
      }
    }
  } else {
    /* just one "tile", dimensions equal to pic size */
#ifndef USE_EXTERNAL_BUFFER
    u16 *p = (u16 *)asic_buff->tile_info.virtual_address;
#else
    u16 *p = (u16 *)((u8 *)asic_buff->misc_linear.virtual_address + asic_buff->tile_info_offset);
#endif
    if(dec_cont->legacy_regs){
      SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_COLS_V0, 1);
      SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_ROWS_V0, 1);
  } else {
    SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_COLS, 1);
    SetDecRegister(dec_cont->hevc_regs, HWIF_NUM_TILE_ROWS, 1);
  }
    p[0] = storage->pic_width_in_ctbs;
    p[1] = storage->pic_height_in_ctbs;
  }
  SetDecRegister(dec_cont->hevc_regs, HWIF_ENTR_CODE_SYNCH_E,
                 pps->entropy_coding_sync_enabled_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_FILT_SLICE_BORDER,
                 pps->loop_filter_across_slices_enabled_flag);
  SetDecRegister(dec_cont->hevc_regs, HWIF_FILT_TILE_BORDER,
                 pps->loop_filter_across_tiles_enabled_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_FILT_CTRL_PRES,
                 pps->deblocking_filter_control_present_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_FILT_OVERRIDE_E,
                 pps->deblocking_filter_override_enabled_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_FILTERING_DIS,
                 pps->disable_deblocking_filter_flag);

  SetDecRegister(dec_cont->hevc_regs, HWIF_FILT_OFFSET_BETA,
                 pps->beta_offset / 2);
  SetDecRegister(dec_cont->hevc_regs, HWIF_FILT_OFFSET_TC, pps->tc_offset / 2);

  SetDecRegister(dec_cont->hevc_regs, HWIF_PARALLEL_MERGE,
                 pps->log_parallel_merge_level);

  SetDecRegister(dec_cont->hevc_regs, HWIF_IDR_PIC_E,
                 IS_RAP_NAL_UNIT(storage->prev_nal_unit));

  SetDecRegister(dec_cont->hevc_regs, HWIF_HDR_SKIP_LENGTH,
                 slice_header->hw_skip_bits);

#ifdef CLEAR_OUT_BUFFER
  DWLPrivateAreaMemset(asic_buff->out_buffer->virtual_address, 0,
         storage->pic_size * 3 / 2);
#endif

  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_OUT_YBASE,
               asic_buff->out_buffer->bus_address);

  /* offset to beginning of chroma part of out and ref pics */
  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_OUT_CBASE,
               asic_buff->out_buffer->bus_address + storage->pic_size);

  SetDecRegister(dec_cont->hevc_regs, HWIF_WRITE_MVS_E, 1);

  SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_OUT_DBASE,
               asic_buff->out_buffer->bus_address + dpb->dir_mv_offset);

  /* reference pictures */
  SetDecRegister(dec_cont->hevc_regs, HWIF_REF_FRAMES,
                 dpb->num_poc_st_curr + dpb->num_poc_lt_curr);

  if (dec_cont->use_video_compressor) {
#ifdef NEW_REG
    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_OUT_TYBASE,
                 asic_buff->out_buffer->bus_address + dpb->cbs_tbl_offsety);
    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_OUT_TCBASE,
                 asic_buff->out_buffer->bus_address + dpb->cbs_tbl_offsetc);
#endif
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_OUT_EC_BYPASS, 0);
  } else {
#ifdef NEW_REG
    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_OUT_TYBASE, 0L);
    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_OUT_TCBASE, 0L);
#endif
    SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_OUT_EC_BYPASS, 1);
  }

  if (dec_cont->use_fetch_one_pic) {
    SetDecRegister(dec_cont->hevc_regs, HWIF_APF_ONE_PID, 1);
  } else {
    SetDecRegister(dec_cont->hevc_regs, HWIF_APF_ONE_PID, 0);
  }

  SetDecRegister(dec_cont->hevc_regs, HWIF_SLICE_HDR_EXT_E,
                 pps->slice_header_extension_present_flag);

  /* write all bases */
  long_term_flags = 0;
  for (i = 0; i < dpb->dpb_size; i++) {
    SET_ADDR_REG2(dec_cont->hevc_regs, ref_ybase[i], ref_ybase[i]-1,
                   dpb->buffer[i].data->bus_address);
    SET_ADDR_REG2(dec_cont->hevc_regs, ref_cbase[i], ref_cbase[i]-1,
                   dpb->buffer[i].data->bus_address + storage->pic_size);
    long_term_flags |= (IS_LONG_TERM_FRAME(dpb->buffer[i]))<<(15-i);
    SET_ADDR_REG2(dec_cont->hevc_regs, ref_dbase[i], ref_dbase[i]-1,
                   dpb->buffer[i].data->bus_address + dpb->dir_mv_offset);

#if NEW_REG
    if (dec_cont->use_video_compressor) {
      SET_ADDR_REG2(dec_cont->hevc_regs, ref_tybase[i], ref_tybase[i]-1,
          dpb->buffer[i].data->bus_address + dpb->cbs_tbl_offsety);
      SET_ADDR_REG2(dec_cont->hevc_regs, ref_tcbase[i], ref_tcbase[i]-1,
          dpb->buffer[i].data->bus_address + dpb->cbs_tbl_offsetc);
    } else {
      SET_ADDR_REG2(dec_cont->hevc_regs, ref_tybase[i], ref_tybase[i]-1, 0L);
      SET_ADDR_REG2(dec_cont->hevc_regs, ref_tcbase[i], ref_tcbase[i]-1, 0L);
    }
#endif
  }
  SetDecRegister(dec_cont->hevc_regs, HWIF_REFER_LTERM_E, long_term_flags);

  /* Raster output configuration. */
  {
    u32 rw = storage->pic_width_in_cbs * (1 << sps->log_min_coding_block_size);
    rw = NEXT_MULTIPLE(rw * rs_pixel_width, 16 * 8) / 8;
    u32 rh = storage->pic_height_in_cbs * (1 << sps->log_min_coding_block_size);
    u32 id = DWLReadAsicID();
    if ((id & 0x0000F000) >> 12 == 0 && /* MAJOR_VERSION */
        (id & 0x00000FF0) >> 4 == 0)    /* MINOR_VERSION */
      /* On early versions RS_E was RS_DIS and we need to take this into
       * account to support the first hardware version properly. */
      SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_OUT_RS_E /* RS_DIS */,
                     dec_cont->output_format == DEC_OUT_FRM_TILED_4X4);
    else
      SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_OUT_RS_E,
                     dec_cont->output_format != DEC_OUT_FRM_TILED_4X4);

    if (storage->raster_buffer_mgr) {
      struct DWLLinearMem raster_buffer = RbmGetRasterBuffer(
          storage->raster_buffer_mgr, *asic_buff->out_buffer);
      SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_RSY_BASE,
                   raster_buffer.bus_address);
      SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_RSC_BASE,
                   raster_buffer.bus_address + rw * rh);

#ifdef DOWN_SCALER
        SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_DS_E, dec_cont->down_scale_enabled);
        SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_DS_X,
                       dec_cont->down_scale_x >> 2);
        SetDecRegister(dec_cont->hevc_regs, HWIF_DEC_DS_Y,
                       dec_cont->down_scale_y >> 2);
      if (dec_cont->down_scale_enabled) {
        u32 dsw, dsh ;
        rw = storage->pic_width_in_cbs * (1 << sps->log_min_coding_block_size);
        dsw = NEXT_MULTIPLE((rw >> storage->down_scale_x_shift) * rs_pixel_width, 16 * 8) / 8;
        dsh = rh >> storage->down_scale_y_shift;
        struct DWLLinearMem dscale_buffer = RbmGetDscaleBuffer(
          storage->raster_buffer_mgr, *asic_buff->out_buffer);
        SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_DSY_BASE,
                     dscale_buffer.bus_address);
        SET_ADDR_REG(dec_cont->hevc_regs, HWIF_DEC_DSC_BASE,
                     dscale_buffer.bus_address + dsw * dsh);
      }
#endif
    }
  }

  /* scaling lists */
  SetDecRegister(dec_cont->hevc_regs, HWIF_SCALING_LIST_E,
                 sps->scaling_list_enable);
  if (sps->scaling_list_enable) {
    u32 s, j, k;
    u8 *p;

    u8(*scaling_list)[6][64];

    /* determine where to read lists from */
    if (pps->scaling_list_present_flag)
      scaling_list = pps->scaling_list;
    else {
      if (!sps->scaling_list_present_flag)
        DefaultScalingList(sps->scaling_list);
      scaling_list = sps->scaling_list;
    }

#ifndef USE_EXTERNAL_BUFFER
    p = (u8 *)dec_cont->asic_buff->scaling_lists.virtual_address;
    ASSERT(!((addr_t)p & 0x7));
    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_SCALE_LIST_BASE,
                 dec_cont->asic_buff->scaling_lists.bus_address);

#else
    p = (u8 *)dec_cont->asic_buff->misc_linear.virtual_address;
    ASSERT(!((addr_t)p & 0x7));
    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_SCALE_LIST_BASE,
                 dec_cont->asic_buff->misc_linear.bus_address);
#endif

    /* dc coeffs of 16x16 and 32x32 lists, stored after 16 coeffs of first
     * 4x4 list */
    for (i = 0; i < 8; i++) *p++ = scaling_list[0][0][16 + i];
    /* next 128b boundary */
    p += 8;

    /* write scaling lists column by column */

    /* 4x4 */
    for (i = 0; i < 6; i++) {
      for (j = 0; j < 4; j++)
        for (k = 0; k < 4; k++) *p++ = scaling_list[0][i][4 * k + j];
    }
    /* 8x8 -> 32x32 */
    for (s = 1; s < 4; s++) {
      for (i = 0; i < (s == 3 ? 2 : 6); i++) {
        for (j = 0; j < 8; j++)
          for (k = 0; k < 8; k++) *p++ = scaling_list[s][i][8 * k + j];
      }
    }
  }

  /* POCs of reference pictures */
  for (i = 0; i < MAX_DPB_SIZE; i++) {
    poc_diff = CLIP3(-128, 127, (i32)storage->poc->pic_order_cnt -
                                    (i32)dpb->current_out->ref_poc[i]);
    SetDecRegister(dec_cont->hevc_regs, ref_poc_regs[i], poc_diff);
  }

  HevcStreamPosUpdate(dec_cont);
}

void HevcInitRefPicList(struct HevcDecContainer *dec_cont) {
  struct DpbStorage *dpb = dec_cont->storage.dpb;
  u32 i, j;
  u32 list0[16] = {0};
  u32 list1[16] = {0};

  /* list 0, short term before + short term after + long term */
  for (i = 0, j = 0; i < dpb->num_poc_st_curr; i++)
    list0[j++] = dpb->ref_pic_set_st[i];
  for (i = 0; i < dpb->num_poc_lt_curr; i++)
    list0[j++] = dpb->ref_pic_set_lt[i];

  /* fill upto 16 elems, copy over and over */
  i = 0;
  while (j < 16) list0[j++] = list0[i++];

  /* list 1, short term after + short term before + long term */
  /* after */
  for (i = dpb->num_poc_st_curr_before, j = 0; i < dpb->num_poc_st_curr; i++)
    list1[j++] = dpb->ref_pic_set_st[i];
  /* before */
  for (i = 0; i < dpb->num_poc_st_curr_before; i++)
    list1[j++] = dpb->ref_pic_set_st[i];
  for (i = 0; i < dpb->num_poc_lt_curr; i++)
    list1[j++] = dpb->ref_pic_set_lt[i];

  /* fill upto 16 elems, copy over and over */
  i = 0;
  while (j < 16) list1[j++] = list1[i++];

  /* TODO: size? */
  for (i = 0; i < MAX_DPB_SIZE; i++) {
    SetDecRegister(dec_cont->hevc_regs, ref_pic_list0[i], list0[i]);
    SetDecRegister(dec_cont->hevc_regs, ref_pic_list1[i], list1[i]);
  }
}

void HevcStreamPosUpdate(struct HevcDecContainer *dec_cont) {
  u32 tmp, is_rb;
  addr_t tmp_addr;

  tmp = 0;
  is_rb = dec_cont->use_ringbuffer;

  /* NAL start prefix in stream start is 0 0 0 or 0 0 1 */
  if (dec_cont->hw_stream_start + 2 >= dec_cont->hw_buffer + dec_cont->hw_buffer_length) {
    u8 i, start_prefix[3];
    for(i = 0; i < 3; i++) {
      if (dec_cont->hw_stream_start + i < dec_cont->hw_buffer + dec_cont->hw_buffer_length)
        start_prefix[i] = DWLPrivateAreaReadByte(dec_cont->hw_stream_start + i);
      else
        start_prefix[i] = DWLPrivateAreaReadByte(dec_cont->hw_stream_start + i - dec_cont->hw_buffer_length);
    }
    if (!(*start_prefix + *(start_prefix + 1))) {
      if (*(start_prefix + 2) < 2) {
        tmp = 1;
      }
    }
  } else {
    if (!(DWLPrivateAreaReadByte(dec_cont->hw_stream_start) +
          DWLPrivateAreaReadByte(dec_cont->hw_stream_start + 1))) {
      if (DWLPrivateAreaReadByte(dec_cont->hw_stream_start + 2) < 2) {
        tmp = 1;
      }
    }
  }

  SetDecRegister(dec_cont->hevc_regs, HWIF_START_CODE_E, tmp);

  /* bit offset if base is unaligned */
  tmp = (dec_cont->hw_stream_start_bus & DEC_HW_ALIGN_MASK) * 8;

  SetDecRegister(dec_cont->hevc_regs, HWIF_STRM_START_BIT, tmp);

  dec_cont->hw_bit_pos = tmp;

  if(is_rb) {
    /* stream buffer base address */
    tmp_addr = dec_cont->hw_buffer_start_bus; /* aligned base */
    ASSERT((tmp_addr & 0xF) == 0);

    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_STREAM_BASE, tmp_addr);

    /* stream data start offset */
    tmp = dec_cont->hw_stream_start_bus - dec_cont->hw_buffer_start_bus; /* unaligned base */
    tmp &= (~DEC_HW_ALIGN_MASK);         /* align the base */

#ifdef NEW_REG
    if (!dec_cont->legacy_regs)
      SetDecRegister(dec_cont->hevc_regs, HWIF_STRM_START_OFFSET, tmp);
#endif

    /* stream data length */
    tmp = dec_cont->hw_length;       /* unaligned stream */
    tmp += dec_cont->hw_bit_pos / 8; /* add the alignmet bytes */

    SetDecRegister(dec_cont->hevc_regs, HWIF_STREAM_LEN, tmp);

    /* stream buffer size */
    tmp = dec_cont->hw_buffer_length;       /* stream buffer size */

#ifdef NEW_REG
    if (!dec_cont->legacy_regs)
      SetDecRegister(dec_cont->hevc_regs, HWIF_STRM_BUF_LEN, tmp);
#endif
  } else {
    /* stream data base address */
    tmp_addr = dec_cont->hw_stream_start_bus; /* unaligned base */
    tmp_addr &= (~DEC_HW_ALIGN_MASK);         /* align the base */
    ASSERT((tmp_addr & 0xF) == 0);

    SET_ADDR_REG(dec_cont->hevc_regs, HWIF_STREAM_BASE, tmp_addr);

#ifdef NEW_REG
    /* stream data start offset */
    if (!dec_cont->legacy_regs)
      SetDecRegister(dec_cont->hevc_regs, HWIF_STRM_START_OFFSET, 0);
#endif

    /* stream data length */
    tmp = dec_cont->hw_length;       /* unaligned stream */
    tmp += dec_cont->hw_bit_pos / 8; /* add the alignmet bytes */

    SetDecRegister(dec_cont->hevc_regs, HWIF_STREAM_LEN, tmp);

    /* stream buffer size */
    tmp = dec_cont->hw_buffer_length;       /* stream buffer size */

#ifdef NEW_REG
    if (!dec_cont->legacy_regs)
      SetDecRegister(dec_cont->hevc_regs, HWIF_STRM_BUF_LEN, tmp);
#endif
  }
}
