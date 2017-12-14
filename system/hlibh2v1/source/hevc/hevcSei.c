/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
-- Description : HEVC SEI Messages.
--
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hevcSei.h"
#include "sw_put_bits.h"
#include "sw_nal_unit.h"
#include "enccommon.h"
#include "base_type.h"



#define SEI_BUFFERING_PERIOD        0
#define SEI_PIC_TIMING              1
#define SEI_FILLER_PAYLOAD          3
#define SEI_USER_DATA_UNREGISTERED  5
#define SEI_RECOVERY_POINT_PAYLOAD  6
#define SEI_ACTIVE_PARAMETER_SETS  129


/*------------------------------------------------------------------------------
  HevcInitSei()
------------------------------------------------------------------------------*/
void HevcInitSei(sei_s *sei, true_e byteStream, u32 hrd, u32 timeScale,
                 u32 nuit)
{
  ASSERT(sei != NULL);

  sei->byteStream = byteStream;
  sei->hrd = hrd;
  sei->seqId = 0x0;
  sei->psp = (u32) ENCHW_YES;
  sei->cts = (u32) ENCHW_YES;
  /* sei->icrd = 0; */
  sei->icrdLen = 24;
  /* sei->icrdo = 0; */
  sei->icrdoLen = 24;
  /* sei->crd = 0; */
  sei->crdLen = 24;
  /* sei->dod = 0; */
  sei->dodLen = 24;
  sei->ps = 0;
  sei->cntType = 1;
  sei->cdf = 0;
  sei->nframes = 0;
  sei->toffs = 0;

  {
    u32 n = 1;

    while (nuit > (1U << n))
      n++;
    sei->toffsLen = n;
  }

  sei->ts.timeScale = timeScale;
  sei->ts.nuit = nuit;
  sei->ts.time = 0;
  sei->ts.sec = 0;
  sei->ts.min = 0;
  sei->ts.hr = 0;
  sei->ts.fts = (u32) ENCHW_YES;
  sei->ts.secf = (u32) ENCHW_NO;
  sei->ts.minf = (u32) ENCHW_NO;
  sei->ts.hrf = (u32) ENCHW_NO;

  sei->userDataEnabled = (u32) ENCHW_NO;
  sei->userDataSize = 0;
  sei->pUserData = NULL;
  sei->activated_sps = 0;
}

/*------------------------------------------------------------------------------
  HevcUpdateSeiPS()  Calculate new pic_struct.
------------------------------------------------------------------------------*/
void HevcUpdateSeiPS(sei_s *sei, u32 interlacedFrame, u32 bottomfield)
{
  //Table D 2 - Interpretation of pic_struct
  if (interlacedFrame == 0)
  {
    // progressive frame.
    sei->ps = 0;
  }
  else
  {
    if (bottomfield)
    {
      sei->ps = 2;
    }
    else
      sei->ps = 1;
  }
}

/*------------------------------------------------------------------------------
  HevcUpdateSeiTS()  Calculate new time stamp.
------------------------------------------------------------------------------*/
void HevcUpdateSeiTS(sei_s *sei, u32 timeInc)
{
  timeStamp_s *ts = &sei->ts;

  ASSERT(sei != NULL);
  timeInc += ts->time;

  while (timeInc >= ts->timeScale)
  {
    timeInc -= ts->timeScale;
    ts->sec++;
    if (ts->sec == 60)
    {
      ts->sec = 0;
      ts->min++;
      if (ts->min == 60)
      {
        ts->min = 0;
        ts->hr++;
        if (ts->hr == 32)
        {
          ts->hr = 0;
        }
      }
    }
  }

  ts->time = timeInc;

  sei->nframes = ts->time / ts->nuit;
  sei->toffs = ts->time - sei->nframes * ts->nuit;

  ts->hrf = (ts->hr != 0);
  ts->minf = ts->hrf || (ts->min != 0);
  ts->secf = ts->minf || (ts->sec != 0);

#ifdef TRACE_PIC_TIMING
  DEBUG_PRINT(("Picture Timing: %02i:%02i:%02i  %6i ticks\n", ts->hr, ts->min,
               ts->sec, (sei->nframes * ts->nuit + sei->toffs)));
#endif
}

/*------------------------------------------------------------------------------
  HevcFillerSei()  Filler payload SEI message. Requested filler payload size
  could be huge. Use of temporary stream buffer is not needed, because size is
  know.
------------------------------------------------------------------------------*/
void HevcFillerSei(struct buffer *sp, sei_s *sei, i32 cnt)
{
  i32 i = cnt;

  struct nal_unit nal_unit_val;

  ASSERT(sp != NULL && sei != NULL);
  if (sei->byteStream == ENCHW_YES)
  {
    put_bits_startcode(sp);
  }

  nal_unit_val.type = PREFIX_SEI_NUT;
  nal_unit_val.temporal_id = 0;
  nal_unit(sp, &nal_unit_val);

  put_bit(sp, SEI_FILLER_PAYLOAD, 8);
  COMMENT(sp, "last_payload_type_byte")

  while (cnt >= 255)
  {
    put_bit(sp, 0xFF, 0x8);
    COMMENT(sp, "ff_byte")
    cnt -= 255;
  }
  put_bit(sp, cnt, 8);
  COMMENT(sp, "last_payload_size_byte");

  for (; i > 0; i--)
  {
    put_bit(sp, 0xFF, 8);
    COMMENT(sp, "filler ff_byte");
  }
  rbsp_trailing_bits(sp);
}

/*------------------------------------------------------------------------------
  HevcBufferingSei()  Buffering period SEI message.
------------------------------------------------------------------------------*/
void HevcBufferingSei(struct buffer *sp, sei_s *sei, vui_t *vui)
{
  u8 *pPayloadSizePos;
  u32 startByteCnt;

  ASSERT(sei != NULL);

  if (sei->hrd == ENCHW_NO)
  {
    return;
  }

  put_bit(sp, SEI_BUFFERING_PERIOD, 8);
  COMMENT(sp, "last_payload_type_byte");
  //rbsp_flush_bits(sp);
  pPayloadSizePos = sp->stream + (sp->bit_cnt >> 3);

  put_bit(sp, 0xFF, 8);   /* this will be updated after we know exact payload size */
  COMMENT(sp, "last_payload_size_byte");
  //rbsp_flush_bits(sp);
  //startByteCnt = *sp->cnt;
  sp->emulCnt = 0;    /* count emul_3_byte for this payload */

  put_bit_ue(sp, sei->seqId);
  COMMENT(sp, "seq_parameter_set_id");

  put_bit(sp, 0, 1);
  COMMENT(sp, "irap_cpb_params_present_flag");
  //how to config this
  put_bit(sp, 0, 1);
  COMMENT(sp, "concatenation_flag");

  put_bit_32(sp, 0, vui->cpbRemovalDelayLength);
  COMMENT(sp, "au_cpb_removal_delay_delta_minus1");

  put_bit_32(sp, sei->icrd, vui->initialCpbRemovalDelayLength);
  COMMENT(sp, "nal_initial_cpb_removal_delay[ i ]");

  put_bit_32(sp, sei->icrdo, vui->initialCpbRemovalDelayLength);
  COMMENT(sp, "nal_initial_cpb_removal_offset[ i ]");
  //rbsp_flush_bits(sp);
  if (sp->bit_cnt)
  {
    rbsp_trailing_bits(sp);
  }

  {
    u32 payload_size;

    payload_size = (u32)(sp->stream - pPayloadSizePos) - 1 - sp->emulCnt;
    *pPayloadSizePos = payload_size;
  }

  /* reset cpb_removal_delay */
  sei->crd = 0;
}

/*------------------------------------------------------------------------------
  HevcPicTimingSei()  Picture timing SEI message.
------------------------------------------------------------------------------*/
void HevcPicTimingSei(struct buffer *sp, sei_s *sei, vui_t *vui)
{
  u8 *pPayloadSizePos;
  u32 startByteCnt;

  ASSERT(sei != NULL);

  put_bit(sp, SEI_PIC_TIMING, 8);
  COMMENT(sp, "last_payload_type_byte");
  //rbsp_flush_bits(sp);
  pPayloadSizePos = sp->stream + (sp->bit_cnt >> 3);

  put_bit(sp, 0xFF, 8);   /* this will be updated after we know exact payload size */
  COMMENT(sp, "last_payload_size_byte");
  //rbsp_flush_bits(sp);
  //startByteCnt = *sp->cnt;
  sp->emulCnt = 0;    /* count emul_3_byte for this payload */
  put_bit(sp, sei->ps, 4);
  COMMENT(sp, "pic_struct");
  put_bit(sp, !sei->ps, 2);
  COMMENT(sp, "source_scan_type");
  put_bit(sp, 0, 1);
  COMMENT(sp, "duplicate_flag");
  if (sei->hrd)
  {
    put_bit_32(sp, sei->crd - 1, vui->cpbRemovalDelayLength);
    COMMENT(sp, "au_cpb_removal_delay_minus1");
    put_bit_32(sp, sei->dod, vui->dpbOutputDelayLength);
    COMMENT(sp, "pic_dpb_output_delay");
  }



  if (sp->bit_cnt)
  {
    rbsp_trailing_bits(sp);
  }

  {
    u32 payload_size;

    payload_size = (u32)(sp->stream - pPayloadSizePos) - 1 - sp->emulCnt;
    *pPayloadSizePos = payload_size;
  }
}
/*------------------------------------------------------------------------------
  HevcPicTimingSei()  Picture timing SEI message.
------------------------------------------------------------------------------*/
void HevcActiveParameterSetsSei(struct buffer *sp, sei_s *sei, vui_t *vui)
{
  u8 *pPayloadSizePos;
  u32 startByteCnt;

  ASSERT(sei != NULL);

  put_bit(sp, SEI_ACTIVE_PARAMETER_SETS, 8);
  COMMENT(sp, "last_payload_type_byte");
  //rbsp_flush_bits(sp);
  pPayloadSizePos = sp->stream + (sp->bit_cnt >> 3);

  put_bit(sp, 0xFF, 8);   /* this will be updated after we know exact payload size */
  COMMENT(sp, "last_payload_size_byte");
  //rbsp_flush_bits(sp);
  //startByteCnt = *sp->cnt;
  sp->emulCnt = 0;    /* count emul_3_byte for this payload */
  put_bit(sp, sei->seqId, 4);
  COMMENT(sp, "active_video_parameter_set_id");
  put_bit(sp, 0, 1);
  COMMENT(sp, "self_contained_cvs_flag");
  put_bit(sp, 1, 1);
  COMMENT(sp, "no_parameter_set_update_flag");
  put_bit_ue(sp, 0);
  COMMENT(sp, "num_sps_ids_minus1");
  put_bit_ue(sp, 0);
  COMMENT(sp, "active_seq_parameter_set_id[ 0 ]");


  if (sp->bit_cnt)
  {
    rbsp_trailing_bits(sp);
  }

  {
    u32 payload_size;

    payload_size = (u32)(sp->stream - pPayloadSizePos) - 1 - sp->emulCnt;
    *pPayloadSizePos = payload_size;
  }
}

/*------------------------------------------------------------------------------
  HevcUserDataUnregSei()  User data unregistered SEI message.
------------------------------------------------------------------------------*/
void HevcUserDataUnregSei(struct buffer *sp, sei_s *sei)
{
  u32 i, cnt;
  const u8 *pUserData;


  ASSERT(sei != NULL);
  ASSERT(sei->pUserData != NULL);
  ASSERT(sei->userDataSize >= 16);

  pUserData = sei->pUserData;
  cnt = sei->userDataSize;
  if (sei->userDataEnabled == ENCHW_NO)
  {
    return;
  }
  put_bit(sp, SEI_USER_DATA_UNREGISTERED, 8);
  COMMENT(sp, "last_payload_type_byte");

  while (cnt >= 255)
  {
    put_bit(sp, 0xFF, 0x8);
    COMMENT(sp, "ff_byte");
    cnt -= 255;
  }

  put_bit(sp, cnt, 8);
  COMMENT(sp, "last_payload_size_byte");

  /* Write uuid */
  for (i = 0; i < 16; i++)
  {
    put_bit(sp, pUserData[i], 8);
    COMMENT(sp, "uuid_iso_iec_11578_byte");
  }

  /* Write payload */
  for (i = 16; i < sei->userDataSize; i++)
  {
    put_bit(sp, pUserData[i], 8);
    COMMENT(sp, "user_data_payload_byte");
  }
}

/*------------------------------------------------------------------------------
  HevcRecoveryPointSei()  recovery point SEI message.
------------------------------------------------------------------------------*/
void HevcRecoveryPointSei(struct buffer *sp, sei_s *sei)
{
  u8 *pPayloadSizePos;
  u32 startByteCnt;

  ASSERT(sei != NULL);

  put_bit(sp, SEI_RECOVERY_POINT_PAYLOAD, 8);
  COMMENT(sp, "last_payload_type_byte");
  //rbsp_flush_bits(sp);
  pPayloadSizePos = sp->stream + (sp->bit_cnt >> 3);

  put_bit(sp, 0xFF, 8);   /* this will be updated after we know exact payload size */
  COMMENT(sp, "last_payload_size_byte");
  //rbsp_flush_bits(sp);
  //startByteCnt = *sp->cnt;
  sp->emulCnt = 0;    /* count emul_3_byte for this payload */
  put_bit_se(sp, sei->recoverypointpoc);
  COMMENT(sp, "recovery_poc_cnt");
  put_bit(sp, 1, 1);
  COMMENT(sp, "exact_match_flag");
  put_bit(sp, 0, 1);
  COMMENT(sp, "broken_link_flag");

  if (sp->bit_cnt)
  {
    rbsp_trailing_bits(sp);
  }

  {
    u32 payload_size;

    payload_size = (u32)(sp->stream - pPayloadSizePos) - 1 - sp->emulCnt;
    *pPayloadSizePos = payload_size;
  }
}

