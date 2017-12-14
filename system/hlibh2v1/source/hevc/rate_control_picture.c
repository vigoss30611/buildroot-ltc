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
--------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
//#include "ctu_tools.h"
#include "rate_control_picture.h"
#include "tools.h"
#include "sw_slice.h"
#include <math.h> /* for ln() */



/*
 * Using only one leaky bucket (Multible buckets is supported by std).
 * Constant bit rate (CBR) operation, ie. leaky bucket drain rate equals
 * average rate of the stream, is enabled if RC_CBR_HRD = 1. Frame skiping and
 * filler data are minimum requirements for the CBR conformance.
 *
 * Constant HRD parameters:
 *   low_delay_hrd_flag = 0, assumes constant delay mode.
 *       cpb_cnt_minus1 = 0, only one leaky bucket used.
 *         (cbr_flag[0] = RC_CBR_HRD, CBR mode.)
 */
#ifdef ROI_SUPPORT
#include "HevcRoiModel.h"
#endif

/*------------------------------------------------------------------------------
  Module defines
------------------------------------------------------------------------------*/

/* Define this if strict bitrate control is needed, each window has a strict
 * bit budget. Otherwise a moving window is used for smoother quality.
*/
#define RC_WINDOW_STRICT

#ifdef TRACE_RC
#include <stdio.h>
FILE *fpRcTrc = NULL;
/* Select debug output: fpRcTrc or stdout */
#define DBGOUTPUT fpRcTrc
/* Select debug level: 0 = minimum, 2 = maximum */
#define DBG_LEVEL 2
#define DBG(l, str) if (l <= DBG_LEVEL) fprintf str
#else
#define DBG(l, str)
#endif

#define INITIAL_BUFFER_FULLNESS   60    /* Decoder Buffer in procents */
#define MIN_PIC_SIZE              50    /* Procents from picPerPic */

#define DIV(a, b)       ((b) ? ((a) + (SIGN(a) * (b)) / 2) / (b) : (a))
#define DSCY                      32 /* n * 32 */
#define DSCBITPERMB               128 //128 /* bitPerMb scaler  */
#define I32_MAX           2147483647 /* 2 ^ 31 - 1 */
#define QP_DELTA          2
#define QP_DELTA_LIMIT    8     /* Threshold when above limit is abandoned. */




#define INTRA_QP_DELTA    (0)
#define WORD_CNT_MAX      65535


/*------------------------------------------------------------------------------
  Local structures
------------------------------------------------------------------------------*/
/* q_step values scaled up by 4 and evenly rounded */
static const i32 q_step[53] = { 3, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11,
                                13, 14, 16, 18, 20, 23, 25, 28, 32, 36, 40, 45, 51, 57, 64, 72, 80, 90,
                                101, 114, 128, 144, 160, 180, 203, 228, 256, 288, 320, 360, 405, 456,
                                513, 577, 640, 720, 810, 896
                              };

/*------------------------------------------------------------------------------
  Local function prototypes
------------------------------------------------------------------------------*/

static i32 InitialQp(i32 bits, i32 pels);
static void MbQuant(hevcRateControl_s *rc);
static void LinearModel(hevcRateControl_s *rc);
static void AdaptiveModel(hevcRateControl_s *rc);
static void SourceParameter(hevcRateControl_s *rc, i32 nonZeroCnt);
static void PicSkip(hevcRateControl_s *rc);
static void PicQuantLimit(hevcRateControl_s *rc);
static i32 VirtualBuffer(hevcVirtualBuffer_s *vb, i32 timeInc, true_e hrd);
static void PicQuant(hevcRateControl_s *rc);
static i32 avg_rc_error(linReg_s *p);
static void update_rc_error(linReg_s *p, i32 bits, i32 windowLength);
static i32 gop_avg_qp(hevcRateControl_s *rc);
static i32 new_pic_quant(linReg_s *p, i32 bits, true_e useQpDeltaLimit);
static i32 get_avg_bits(linReg_s *p, i32 n);
static void update_tables(linReg_s *p, i32 qp, i32 bits);
static void update_model(linReg_s *p);
static i32 lin_sy(i32 *qp, i32 *r, i32 n);
static i32 lin_sx(i32 *qp, i32 n);
static i32 lin_sxy(i32 *qp, i32 *r, i32 n);
static i32 lin_nsxx(i32 *qp, i32 n);

/*------------------------------------------------------------------------------

  HevcInitRc() Initialize rate control.

------------------------------------------------------------------------------*/
bool_e HevcInitRc(hevcRateControl_s *rc, u32 newStream)
{
  hevcVirtualBuffer_s *vb = &rc->virtualBuffer;

  if ((rc->qpMax > 51))
  {
    printf("rc->qpMax error\n");
    return ENCHW_NOK;
  }

  /* QP -1: Initial QP estimation done by RC */
  if (rc->qpHdr == -1)
  {
    i32 tmp = HevcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
    rc->qpHdr = InitialQp(tmp, rc->picArea/*rc->ctbPerPic*rc->ctbSize*rc->ctbSize*/);
    PicQuantLimit(rc);
  }

  if ((rc->qpHdr > rc->qpMax) || (rc->qpHdr < rc->qpMin))
  {
      printf("rc->qpHdr error\n");
    return ENCHW_NOK;
  }

  /* HRD needs frame RC and macroblock RC*/
  if (rc->hrd == ENCHW_YES)
  {
    rc->picRc = ENCHW_YES;
  }

  rc->ctbRc = ENCHW_NO;
  rc->coeffCntMax = rc->ctbPerPic * rc->ctbSize * rc->ctbSize * 3 / 2;
  rc->frameCoded = ENCHW_YES;
  rc->sliceTypeCur = I_SLICE;
  rc->sliceTypePrev = P_SLICE;

  rc->qpHdrPrev  = rc->qpHdr;
  rc->fixedQp    = rc->qpHdr;

  vb->bitPerPic = HevcCalculate(vb->bitRate, rc->outRateDenom, rc->outRateNum);
  rc->qpCtrl.checkPointDistance = 0;
  rc->qpCtrl.checkPoints = 0;

#if defined(TRACE_RC) && (DBGOUTPUT == fpRcTrc)
  if (!fpRcTrc) fpRcTrc = fopen("rc.trc", "wt");
#endif

  /* new rate control algorithm */
  /* API parameter is named gopLen but the actual usage is rate controlling
   * window in frames. RC tries to match the target bitrate inside the
   * window. Each window can contain multiple GOPs and the RC adapts to the
   * intra rate by calculating intraInterval. */

  DBG(0, (DBGOUTPUT, "\nInitRc: picRc\t\t%i  hrd\t%i  picSkip\t%i\n",
          rc->picRc, rc->hrd, rc->picSkip));
  DBG(0, (DBGOUTPUT, "  mbRc\t\t\t%i  qpHdr\t%i  Min,Max\t%i,%i\n",
          rc->mbRc, rc->qpHdr, rc->qpMin, rc->qpMax));
  DBG(0, (DBGOUTPUT, "  checkPointDistance\t%i\n",
          rc->qpCtrl.checkPointDistance));

  DBG(0, (DBGOUTPUT, "  CPBsize\t%i\n BitRate\t%i\n BitPerPic\t%i\n",
          vb->bufferSize, vb->bitRate, vb->bitPerPic));

  /* If changing QP between frames don't reset GOP RC.
   * Changing bitrate resets RC the same way as new stream. */
  if (!newStream) {
   //printf("!newStream error\n");
    return ENCHW_OK;
    }

  /* new rate control algorithm */
  update_rc_error(&rc->rError, 0x7fffffff, 0);
  update_rc_error(&rc->intraError, 0x7fffffff, 0);

  h2_EWLmemset(&rc->linReg, 0, sizeof(linReg_s));
  rc->linReg.qs[0]   = q_step[51];
  rc->linReg.qp_prev = rc->qpHdr;
  h2_EWLmemset(&rc->intra, 0, sizeof(linReg_s));
  rc->intra.qs[0]   = q_step[51];
  rc->intra.qp_prev = rc->qpHdr;
  /* API parameter is named gopLen but the actual usage is rate controlling
   * window in frames. RC tries to match the target bitrate inside the
   * window. Each window can contain multiple GOPs and the RC adapts to the
   * intra rate by calculating intraInterval. */
  rc->windowLen = rc->gopLen;
  vb->windowRem = rc->gopLen;
  rc->intraIntervalCtr = rc->intraInterval = rc->gopLen;
  rc->targetPicSize = 0;
  rc->frameBitCnt = 0;
  rc->firstsecond_intra_bit_error=0;
  rc->trans_bit2intra=0;
  rc->first_frame=1;
  rc->gopQpSum = 0;
  rc->gopQpDiv = 0;

  vb->picTimeInc      = 0;
  vb->realBitCnt      = 0;
  vb->virtualBitCnt   = 0;
  //    rc->sei.hrd = rc->hrd;

  if (rc->hrd)
  {
    vb->bucketFullness =
      HevcCalculate(vb->bufferSize, INITIAL_BUFFER_FULLNESS, 100);
    rc->gDelaySum = HevcCalculate(90000, vb->bufferSize, vb->bitRate);
    rc->gInitialDelay = HevcCalculate(90000, vb->bucketFullness, vb->bitRate);
    rc->gInitialDoffs = rc->gDelaySum - rc->gInitialDelay;
    vb->bucketFullness = vb->bucketLevel =
                           vb->bufferSize - vb->bucketFullness;
#ifdef TRACE_RC
    rc->gBufferMin = vb->bufferSize;
    rc->gBufferMax = 0;
#endif
    rc->sei.icrd = (u32)rc->gInitialDelay;
    rc->sei.icrdo = (u32)rc->gInitialDoffs;

    DBG(1, (DBGOUTPUT, "\n InitialDelay\t%i\n Offset\t\t%i\n",
            rc->gInitialDelay, rc->gInitialDoffs));
  }
  return ENCHW_OK;
}

i32 HevcCalculateInitialQp(hevcRateControl_s * rc, i32 bitRate)
{
  i32 qpHdr = HevcCalculate(bitRate, rc->outRateDenom, rc->outRateNum);
  qpHdr = InitialQp(qpHdr, rc->picArea/*rc->ctbPerPic*rc->ctbSize*rc->ctbSize*/);
  return qpHdr;
}

/*------------------------------------------------------------------------------

  InitialQp()  Returns sequence initial quantization parameter.

------------------------------------------------------------------------------*/
#define OFFSET_QP 0
static i32 InitialQp(i32 bits, i32 pels)
{
  const i32 qp_tbl[2][36] =
  {
    /*{27, 32, 36, 40, 44, 51, 58, 65, 72, 84, 96, 108, 119, 138, 156, 174, 192, 223, 253, 285, 314, 349, 384, 419, 453, 503, 553, 603, 653, 719, 784, 864, 0x7FFFFFFF},*/
    {16,19,23, 27, 32, 36, 40, 44, 51, 58, 65, 72, 84, 96, 108, 119, 138, 156, 174, 192, 223, 253, 285, 314, 349, 384, 419, 453, 503, 553, 603, 653, 719, 784, 864, 0x7FFFFFFF},
    /*{26, 38, 59, 96, 173, 305, 545, 0x7FFFFFFF},*/
    {51,50,49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16}
  };
  const i32 upscale = 16000/2;
  i32 i = -1;

  /* prevents overflow, QP would anyway be 17 with this high bitrate
     for all resolutions under and including 1920x1088 */
  //    if (bits > 1000000)
  //      return 17;

  /* Make room for multiplication */
  pels >>= 8;
  bits >>= 5;

  /* Use maximum QP if bitrate way too low. */
  if (!bits)
    return 51;

  /* Adjust the bits value for the current resolution */
  bits *= pels + 250;
  ASSERT(pels > 0);
  ASSERT(bits > 0);
  bits /= 350 + (3 * pels) / 4;
  bits = HevcCalculate(bits, upscale, pels << 6);

  while ((qp_tbl[0][++i]-OFFSET_QP) < bits);

  DBG(0, (DBGOUTPUT, "BPP\t\t%d\n", bits));

  return qp_tbl[1][i];
}

/*------------------------------------------------------------------------------

      HevcFillerRc

      Stream watermarking. Insert filler NAL unit of certain size after each
      Nth frame.

------------------------------------------------------------------------------*/
u32 HevcFillerRc(hevcRateControl_s *rc, u32 frameCnt)
{
  const u8 filler[] = { 0, 9, 0, 9, 9, 9, 0, 2, 2, 0 };
  u32 idx;

  if (rc->fillerIdx == (u32)(-1))
  {
    rc->fillerIdx = sizeof(filler) / sizeof(*filler) - 1;
  }

  idx = rc->fillerIdx;
  if (frameCnt != 0 && ((frameCnt % 128) == 0))
  {
    idx++;
  }
  idx %= sizeof(filler) / sizeof(*filler);

  if (idx != rc->fillerIdx)
  {
    rc->fillerIdx = idx;
    return filler[idx] + 1;
  }
  return 0;
}
/*------------------------------------------------------------------------------
  VirtualBuffer()  Return difference of target and real buffer fullness.
  Virtual buffer and real bit count grow until one second.  After one second
  output bit rate per second is removed from virtualBitCnt and realBitCnt. Bit
  drifting has been taken care.

  If the leaky bucket in VBR mode becomes empty (e.g. underflow), those R * T_e
  bits are lost and must be decremented from virtualBitCnt. (NOTE: Drift
  calculation will mess virtualBitCnt up, so the loss is added to realBitCnt)
------------------------------------------------------------------------------*/
static i32 VirtualBuffer(hevcVirtualBuffer_s *vb, i32 timeInc, true_e hrd)
{
  i32 target;

  vb->picTimeInc    += timeInc;

  /* picTimeInc must be in range of [0, timeScale) */
  while (vb->picTimeInc >= vb->timeScale)
  {
    vb->picTimeInc    -= vb->timeScale;

    //infotm patch
    if(vb->skipFrameTarget) {
        vb->realBitCnt = vb->realBitCnt % vb->bitRate;
        vb->bucketLevel = vb->bucketLevel % vb->bitRate;
    } else {
        vb->realBitCnt    -= vb->bitRate;
        vb->bucketLevel   -= vb->bitRate;
    }

    vb->seconds++;
    vb->averageBitRate = vb->bitRate + vb->realBitCnt / vb->seconds;
  }
  vb->virtualBitCnt = HevcCalculate(vb->bitRate, vb->picTimeInc, vb->timeScale);

  if (hrd)
  {
#if RC_CBR_HRD
    /* In CBR mode, bucket _must not_ underflow. Insert filler when
     * needed. */
    vb->bucketFullness = vb->bucketLevel - vb->virtualBitCnt;
#else
    if (vb->bucketLevel >= vb->virtualBitCnt)
    {
      vb->bucketFullness = vb->bucketLevel - vb->virtualBitCnt;
    }
    else
    {
      vb->bucketFullness = 0;
      vb->realBitCnt += vb->virtualBitCnt - vb->bucketLevel;
      vb->bucketLevel = vb->virtualBitCnt;
    }
#endif
  }

  /* Saturate realBitCnt, this is to prevent overflows caused by much greater
     bitrate setting than is really possible to reach */
  if (vb->realBitCnt > 0x1FFFFFFF)
    vb->realBitCnt = 0x1FFFFFFF;
  if (vb->realBitCnt < -0x1FFFFFFF)
    vb->realBitCnt = -0x1FFFFFFF;

  target = vb->virtualBitCnt - vb->realBitCnt;

  /* Saturate target, prevents rc going totally out of control.
     This situation should never happen. */
  if (target > 0x1FFFFFFF)
    target = 0x1FFFFFFF;
  if (target < -0x1FFFFFFF)
    target = -0x1FFFFFFF;


  DBG(1, (DBGOUTPUT, "virtualBitCnt:\t%8i  realBitCnt:\t%8i",
          vb->virtualBitCnt, vb->realBitCnt));
  DBG(1, (DBGOUTPUT, "  diff bits:\t%8i  avg bitrate:\t%8i\n", target,
          vb->averageBitRate));
  return target;
}

/*------------------------------------------------------------------------------
  HevcAfterPicRc()  Update source model, bit rate target error and linear
  regression model for frame QP calculation. If HRD enabled, check leaky bucket
  status and return RC_OVERFLOW if coded frame must be skipped. Otherwise
  returns number of required filler payload bytes.
------------------------------------------------------------------------------*/
i32 HevcAfterPicRc(hevcRateControl_s *rc, u32 nonZeroCnt, u32 byteCnt,
                   u32 qpSum)
{
  hevcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 bitPerPic = rc->virtualBuffer.bitPerPic;
  i32 tmp, stat, bitCnt = (i32)byteCnt * 8;

  (void) bitPerPic;
  rc->qpSum = (i32)qpSum;
  rc->frameBitCnt = bitCnt;
  rc->nonZeroCnt = nonZeroCnt;
  rc->gopBitCnt += bitCnt;
  rc->frameCnt++;
  if(rc->first_frame==1)
  {
    rc->firstsecond_intra_bit_error=bitCnt-bitPerPic;
    rc->first_frame=0;
  }
  if(rc->trans_bit2intra==1)
  {
    rc->firstsecond_intra_bit_error+=bitCnt-bitPerPic;
  }

  //printf("\n rc->frameBitCnt=%d,rc->gopBitCnt=%d,bitPerPic=%d\n",rc->frameBitCnt,rc->gopBitCnt,bitPerPic);
  if (rc->targetPicSize)
  {
    tmp = ((bitCnt - rc->targetPicSize) * 100) /
          rc->targetPicSize;
  }
  else
  {
    tmp = 0;
  }

  DBG(1, (DBGOUTPUT, "\nAFTER PIC RC:\n"));
  DBG(0, (DBGOUTPUT, "BitCnt\t\t%8d\n", bitCnt));
  DBG(1, (DBGOUTPUT, "BitErr/avg\t%7d%%  ",
          ((bitCnt - bitPerPic) * 100) / (bitPerPic + 1)));
  DBG(1, (DBGOUTPUT, "BitErr/target\t%7i%%  qpHdr %2i  avgQp %2i\n",
          tmp, rc->qpHdr, rc->qpSum / rc->mbPerPic));

  /* Calculate the source parameter only for INTER frames */
  if (rc->sliceTypeCur != I_SLICE)
    SourceParameter(rc, rc->nonZeroCnt);

  /* Store the error between target and actual frame size in percentage.
   * Saturate the error to avoid inter frames with mostly intra MBs
   * to affect too much. */
  if (rc->sliceTypeCur != I_SLICE)
  {
    update_rc_error(&rc->rError,
                    MIN(bitCnt - rc->targetPicSize, 2 * rc->targetPicSize), rc->windowLen);
  }
  else
  {
    update_rc_error(&rc->intraError,
                    MIN(bitCnt - rc->targetPicSize, 2 * rc->targetPicSize), rc->windowLen);
  }

  /* Update number of bits used for residual, inter or intra */
  if (rc->sliceTypeCur != I_SLICE)
  {

    if(rc->trans_bit2intra==0)
    {
      update_tables(&rc->linReg, rc->qpHdrPrev,
                    HevcCalculate(bitCnt, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16));
      update_model(&rc->linReg);
    }
    else
      rc->trans_bit2intra=0;
  }
  else
  {
    update_tables(&rc->intra, rc->qpHdrPrev,
                  HevcCalculate(bitCnt, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16));
    update_model(&rc->intra);
  }

  /* Post-frame skip if HRD buffer overflow */
  if ((rc->hrd == ENCHW_YES) && (bitCnt > (vb->bufferSize - vb->bucketFullness)))
  {
    DBG(1, (DBGOUTPUT, "Be: %7i  ", vb->bucketFullness));
    DBG(1, (DBGOUTPUT, "fillerBits %5i  ", 0));
    DBG(1, (DBGOUTPUT, "bitCnt %d  spaceLeft %d  ",
            bitCnt, (vb->bufferSize - vb->bucketFullness)));
    DBG(1, (DBGOUTPUT, "bufSize %d  bucketFullness %d  bitPerPic %d\n",
            vb->bufferSize, vb->bucketFullness, bitPerPic));
    DBG(0, (DBGOUTPUT, "HRD overflow, frame discard\n"));
    rc->frameCoded = ENCHW_NO;
    return HEVCRC_OVERFLOW;
  }
  else
  {
    vb->bucketLevel += bitCnt;
    vb->bucketFullness += bitCnt;
    vb->realBitCnt += bitCnt;
  }

  DBG(1, (DBGOUTPUT, "plot\t%4i\t%4i\t%8i\t%8i\t%8i\t%8i\t%8i\t%8i\t%8i\n",
          rc->frameCnt, rc->qpHdr, rc->targetPicSize, bitCnt,
          bitPerPic, rc->gopAvgBitCnt, vb->realBitCnt - vb->virtualBitCnt,
          vb->bitRate, vb->bucketFullness));

  if (rc->hrd == ENCHW_NO)
  {
    return 0;
  }

  tmp = 0;

#if RC_CBR_HRD
  /* Bits needed to prevent bucket underflow */
  tmp = bitPerPic - vb->bucketFullness;

  if (tmp > 0)
  {
    tmp = (tmp + 7) / 8;
    vb->bucketFullness += tmp * 8;
    vb->realBitCnt += tmp * 8;
  }
  else
  {
    tmp = 0;
  }
#endif

  /* Update Buffering Info */
  stat = vb->bufferSize - vb->bucketFullness;

  rc->gInitialDelay = HevcCalculate(90000, stat, vb->bitRate);
  rc->gInitialDoffs = rc->gDelaySum - rc->gInitialDelay;

  rc->sei.icrd  = (u32)rc->gInitialDelay;
  rc->sei.icrdo = (u32)rc->gInitialDoffs;

  DBG(1, (DBGOUTPUT, "initialDelay: %5i  ", rc->gInitialDelay));
  DBG(1, (DBGOUTPUT, "initialDoffs: %5i\n", rc->gInitialDoffs));
  DBG(1, (DBGOUTPUT, "Be: %7i  ", vb->bucketFullness));
  DBG(1, (DBGOUTPUT, "fillerBits %5i\n", tmp * 8));

#ifdef TRACE_RC
  if (vb->bucketFullness < rc->gBufferMin)
  {
    rc->gBufferMin = vb->bucketFullness;
  }
  if (vb->bucketFullness > rc->gBufferMax)
  {
    rc->gBufferMax = vb->bucketFullness;
  }
  DBG(1, (DBGOUTPUT, "\nLeaky Bucket Min: %i (%d%%)  Max: %i (%d%%)\n",
          rc->gBufferMin, rc->gBufferMin * 100 / vb->bufferSize,
          rc->gBufferMax, rc->gBufferMax * 100 / vb->bufferSize));
#endif
  return tmp;
}

/*------------------------------------------------------------------------------
  HevcBeforePicRc()  Update virtual buffer, and calculate picInitQp for current
  picture , and coded status.
------------------------------------------------------------------------------*/
void HevcBeforePicRc(hevcRateControl_s *rc, u32 timeInc, u32 sliceType)
{
  hevcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 i, rcWindow, intraBits = 0, tmp = 0, qp_move = 0;

  rc->frameCoded = ENCHW_YES;
  rc->sliceTypeCur = sliceType;

  if(rc->sliceTypeCur==I_SLICE)
    rc->firstsecond_intra_bit_error=0;

  DBG(1, (DBGOUTPUT, "\nBEFORE PIC RC: pic=%d\n", rc->frameCnt));
  DBG(1, (DBGOUTPUT, "Frame type:\t%8i  timeInc:\t%8i\n",
          sliceType, timeInc));

  tmp = VirtualBuffer(&rc->virtualBuffer, (i32) timeInc, rc->hrd);
  //printf("\n VirtualBuffer=%d\n",tmp);
  for (i = 0; i < CHECK_POINTS_MAX; i++)
  {
    rc->qpCtrl.wordCntTarget[i] = 0;
  }

  if (vb->windowRem == 0)
  {
    vb->windowRem = rc->windowLen - 1;
    /* New bitrate window, reset error counters. */
    update_rc_error(&rc->rError, 0x7fffffff, rc->windowLen);
    /* Don't reset intra error in case of intra-only, it would cause step. */
    if (rc->sliceTypeCur != rc->sliceTypePrev)
      update_rc_error(&rc->intraError, 0x7fffffff, rc->windowLen);
  }
  else
  {
    vb->windowRem--;
  }

  /* Calculate target size for this picture. Adjust the target bitPerPic
   * with the cumulated error between target and actual bitrates (tmp).
   * Also take into account the bits used by intra frame starting the GOP. */
  if (rc->sliceTypeCur != I_SLICE &&
      rc->intraInterval > 1)
  {
    /* GOP bits that are used by intra frame. Amount of bits
     * "stolen" by intra from each inter frame in the GOP. */
#if 0
    if(get_avg_bits(&rc->gop, 10)==0)
      intraBits = vb->bitPerPic * rc->intraInterval * get_avg_bits(&rc->gop, 10) / 100;
    else
#endif
    intraBits = vb->bitPerPic * rc->intraInterval * get_avg_bits(&rc->gop, 10) / 100;
    intraBits -= vb->bitPerPic;
    intraBits +=rc->firstsecond_intra_bit_error;
    intraBits /= (rc->intraInterval - 1);
    intraBits = MAX(0, intraBits);
  }

  /* Compensate the virtual bit buffer for intra frames "stealing" bits
   * from inter frames because after intra frame the bit buffer seems
   * to overflow but the following inters will level the buffer. */
  tmp += intraBits * (rc->intraInterval - rc->intraIntervalCtr);

#ifdef RC_WINDOW_STRICT
  /* In the end of window don't be too strict with matching the error
   * otherwise the end of window tends to twist QP. */
  rcWindow = MAX(MAX(3, rc->windowLen / 6), vb->windowRem);
#else
  /* Actually we can be fairly easy with this one, let's make it
   * a moving window to smoothen the changes. */
  rcWindow = MAX(1, rc->windowLen);
#endif


  rc->targetPicSize = vb->bitPerPic - intraBits + DIV(tmp, rcWindow) + qp_move;
  /* Limit the target to a realistic minimum that can be reached.
   * Setting target lower than this will confuse RC because it can never
   * be reached. Frame with only skipped mbs == 96 bits. */// carl 96 should be changed to what number
  rc->targetPicSize = MAX(96 + rc->ctbRows * rc->ctbSize, rc->targetPicSize);
  //printf("\n rc->targetPicSize=%d\n",rc->targetPicSize);
  DBG(1, (DBGOUTPUT, "intraRatio: %3i%%\tintraBits: %7i\tbufferComp: %7i\n",
          get_avg_bits(&rc->gop, 10), intraBits, DIV(tmp, rcWindow)));
  DBG(1, (DBGOUTPUT, "WndRem: %4i  ", vb->windowRem));
  if (rc->sliceTypeCur == I_SLICE)
  {
    DBG(1, (DBGOUTPUT, "Rd: %6d  ", avg_rc_error(&rc->intraError)));
  }
  else
  {
    DBG(1, (DBGOUTPUT, "Rd: %6d  ", avg_rc_error(&rc->rError)));
  }
  DBG(1, (DBGOUTPUT, "Tr: %7d  ", rc->targetPicSize));
  if (rc->sliceTypeCur == I_SLICE)
  {
    DBG(1, (DBGOUTPUT, "Td: %7d\n",
            CLIP3(rc->targetPicSize - avg_rc_error(&rc->intraError),
                  0, 2 * rc->targetPicSize)));
  }
  else
  {
    DBG(1, (DBGOUTPUT, "Td: %7d\n",
            CLIP3(rc->targetPicSize - avg_rc_error(&rc->rError),
                  0, 2 * rc->targetPicSize)));
  }

  if (rc->sliceTypeCur != I_SLICE && rc->picSkip)
    PicSkip(rc);

  /* determine initial quantization parameter for current picture */
  PicQuant(rc);

  /* quantization parameter user defined limitations */
  PicQuantLimit(rc);
  /* Store the start QP, before ROI adjustment */
  rc->qpHdrPrev = rc->qpHdr;

  if (rc->sliceTypeCur == I_SLICE)
  {
    if (rc->fixedIntraQp)
      rc->qpHdr = rc->fixedIntraQp;
    else if (rc->sliceTypePrev != I_SLICE)
    {
      //if(rc->picRc)
      if(rc->virtualBuffer.bitPerPic<(400*rc->picArea/(3*192*108)))
      {
        if(rc->intraQpDelta<-3)
        {
          rc->intraQpDelta=-3;
          //printf("rc->intraQpDelta=%d\n",rc->intraQpDelta);
        }

      }
      rc->qpHdr += rc->intraQpDelta;
    }

    /* quantization parameter user defined limitations still apply */
    PicQuantLimit(rc);
    if (rc->intraIntervalCtr > 1)
      rc->intraInterval = rc->intraIntervalCtr;
    rc->intraIntervalCtr = 1;
  }
  else
  {
    /* trace the QP over GOP, excluding Intra QP */
    rc->gopQpSum += rc->qpHdr;
    rc->gopQpDiv++;
    rc->intraIntervalCtr++;

    /* Check that interval is repeating */
    if (rc->intraIntervalCtr > rc->intraInterval)
      rc->intraInterval = rc->intraIntervalCtr;
  }

  /* mb rate control (check point rate control) */
#ifdef ROI_SUPPORT
  if (rc->roiRc)
  {
    HevcEncRoiModel(rc);
    PicQuantLimit(rc);
  }
  else
#endif
    if (rc->ctbRc)
    {
      MbQuant(rc);
    }

  /* reset counters */
  rc->qpSum = 0;
  rc->qpLastCoded = rc->qpHdr;
  rc->qpTarget = rc->qpHdr;
  rc->nonZeroCnt = 0;
  rc->sliceTypePrev = rc->sliceTypeCur;

  DBG(0, (DBGOUTPUT, "Frame coded\t%8d  ", rc->frameCoded));
  DBG(0, (DBGOUTPUT, "Frame qpHdr\t%8d  ", rc->qpHdr));
  DBG(0, (DBGOUTPUT, "GopRem:\t%8d  ", vb->windowRem));
  DBG(0, (DBGOUTPUT, "Target bits:\t%8d  ", rc->targetPicSize));
  DBG(1, (DBGOUTPUT, "\nintraBits:\t%8d  ", intraBits));
  DBG(1, (DBGOUTPUT, "bufferComp:\t%8d  ", DIV(tmp, rcWindow)));
  DBG(1, (DBGOUTPUT, "Rd:\t\t%8d\n", avg_rc_error(&rc->rError)));

  for (i = 0; i < CHECK_POINTS_MAX; i++)
  {
    DBG(1, (DBGOUTPUT, "CP %i  mbNum %4i  wTarg %5i\n", i,
            (rc->qpCtrl.checkPointDistance * (i + 1)),
            rc->qpCtrl.wordCntTarget[i] * 32));
  }

  rc->sei.crd += timeInc;

  rc->sei.dod = 0;
}

/*------------------------------------------------------------------------------

  MbQuant()

------------------------------------------------------------------------------*/
void MbQuant(hevcRateControl_s *rc)
{
  i32 nonZeroTarget;

  /* Disable Mb Rc for Intra Slices, because coeffTarget will be wrong */
  if (rc->sliceTypeCur == I_SLICE ||
      rc->srcPrm == 0)
  {
    return;
  }

  /* Required zero cnt */
  nonZeroTarget = HevcCalculate(rc->targetPicSize, 256, rc->srcPrm);
  nonZeroTarget = MIN(rc->coeffCntMax, MAX(0, nonZeroTarget));

  nonZeroTarget = MIN(0x7FFFFFFFU / 1024U, (u32)nonZeroTarget);

  rc->virtualBuffer.nonZeroTarget = nonZeroTarget;

  /* Use linear model when previous frame can't be used for prediction */
  if ((rc->sliceTypeCur != rc->sliceTypePrev) || (rc->nonZeroCnt == 0))
  {
    LinearModel(rc);
  }
  else
  {
    AdaptiveModel(rc);
  }
}

/*------------------------------------------------------------------------------

  LinearModel()

------------------------------------------------------------------------------*/
void LinearModel(hevcRateControl_s *rc)
{
  const i32 sscale = 256;
  hevcQpCtrl_s *qc = &rc->qpCtrl;
  i32 scaler;
  i32 i;
  i32 tmp, nonZeroTarget = rc->virtualBuffer.nonZeroTarget;

  ASSERT(nonZeroTarget < (0x7FFFFFFF / sscale));

  if (nonZeroTarget > 0)
  {
    scaler = HevcCalculate(nonZeroTarget, sscale, (i32) rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
  }
  else
  {
    return;
  }

  DBG(1, (DBGOUTPUT, " Linear Target: %8d prevCnt:\t %6d Scaler:\t %6d\n",
          nonZeroTarget, rc->nonZeroCnt, scaler / sscale));

  for (i = 0; i < rc->qpCtrl.checkPoints; i++)
  {
    tmp = (scaler * (qc->checkPointDistance * (i + 1) + 1)) / sscale;
    tmp = MIN(WORD_CNT_MAX, tmp / 32 + 1);
    if (tmp < 0) tmp = WORD_CNT_MAX;    /* Detect overflow */
    qc->wordCntTarget[i] = tmp; /* div32 for regs */
  }

  /* calculate nz count for avg. bits per frame */
  tmp = HevcCalculate(rc->virtualBuffer.bitPerPic, rc->ctbSize * rc->ctbSize, rc->srcPrm);

  DBG(1, (DBGOUTPUT, "Error Limit:\t %8d SrcPrm:\t %6d\n",
          tmp, rc->srcPrm / 256));

  qc->wordError[0] = -tmp * 3;
  qc->qpChange[0] = -3;
  qc->wordError[1] = -tmp * 2;
  qc->qpChange[1] = -2;
  qc->wordError[2] = -tmp * 1;
  qc->qpChange[2] = -1;
  qc->wordError[3] = tmp * 1;
  qc->qpChange[3] = 0;
  qc->wordError[4] = tmp * 2;
  qc->qpChange[4] = 1;
  qc->wordError[5] = tmp * 3;
  qc->qpChange[5] = 2;
  qc->wordError[6] = tmp * 4;
  qc->qpChange[6] = 3;

  for (i = 0; i < CTRL_LEVELS; i++)
  {
    tmp = qc->wordError[i];
    tmp = CLIP3(-32768, 32767, tmp / 4);
    qc->wordError[i] = tmp;
  }
}


/*------------------------------------------------------------------------------

  AdaptiveModel()

------------------------------------------------------------------------------*/
void AdaptiveModel(hevcRateControl_s *rc)
{
  const i32 sscale = rc->ctbSize * rc->ctbSize;
  hevcQpCtrl_s *qc = &rc->qpCtrl;
  i32 i;
  i32 tmp, nonZeroTarget = rc->virtualBuffer.nonZeroTarget;
  i32 scaler;

  ASSERT(nonZeroTarget < (0x7FFFFFFF / sscale));

  if ((nonZeroTarget > 0) && (rc->nonZeroCnt > 0))
  {
    scaler = HevcCalculate(nonZeroTarget, sscale, rc->nonZeroCnt);
  }
  else
  {
    return;
  }
  DBG(1, (DBGOUTPUT, "Adaptive Target: %8d prevCnt:\t %6d Scaler:\t %6d\n",
          nonZeroTarget, rc->nonZeroCnt, scaler / sscale));

  for (i = 0; i < rc->qpCtrl.checkPoints; i++)
  {
    tmp = (i32)(qc->wordCntPrev[i] * scaler) / sscale;
    tmp = MIN(WORD_CNT_MAX, tmp / 32 + 1);
    if (tmp < 0) tmp = WORD_CNT_MAX;    /* Detect overflow */
    qc->wordCntTarget[i] = tmp; /* div32 for regs */
    DBG(2, (DBGOUTPUT, " CP %i  wordCntPrev %6i  wordCntTarget_div32 %6i\n",
            i, qc->wordCntPrev[i], qc->wordCntTarget[i]));
  }

  /* Qp change table */

  /* calculate nz count for avg. bits per frame */
  tmp = HevcCalculate(rc->virtualBuffer.bitPerPic, rc->ctbSize * rc->ctbSize, (rc->srcPrm * 3));

  DBG(1, (DBGOUTPUT, "Error Limit:\t %8d SrcPrm:\t %6d\n",
          tmp, rc->srcPrm / (rc->ctbSize * rc->ctbSize)));

  qc->wordError[0] = -tmp * 3;
  qc->qpChange[0] = -3;
  qc->wordError[1] = -tmp * 2;
  qc->qpChange[1] = -2;
  qc->wordError[2] = -tmp * 1;
  qc->qpChange[2] = -1;
  qc->wordError[3] = tmp * 1;
  qc->qpChange[3] = 0;
  qc->wordError[4] = tmp * 2;
  qc->qpChange[4] = 1;
  qc->wordError[5] = tmp * 3;
  qc->qpChange[5] = 2;
  qc->wordError[6] = tmp * 4;
  qc->qpChange[6] = 3;

  for (i = 0; i < CTRL_LEVELS; i++)
  {
    tmp = qc->wordError[i];
    tmp = CLIP3(-32768, 32767, tmp / 4);
    qc->wordError[i] = tmp;
  }
}

/*------------------------------------------------------------------------------

  SourceParameter()  Source parameter of last coded frame. Parameters
  has been scaled up by factor 256.

------------------------------------------------------------------------------*/
void SourceParameter(hevcRateControl_s *rc, i32 nonZeroCnt)
{
  ASSERT(rc->qpSum <= 51 * rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
  ASSERT(nonZeroCnt <= rc->coeffCntMax);
  ASSERT(nonZeroCnt >= 0 && rc->coeffCntMax >= 0);

  /* AVOID division by zero */
  if (nonZeroCnt == 0)
  {
    nonZeroCnt = 1;
  }

  rc->srcPrm = HevcCalculate(rc->frameBitCnt, rc->ctbSize * rc->ctbSize, nonZeroCnt);

  DBG(1, (DBGOUTPUT, "nonZeroCnt %6i, srcPrm %i\n",
          nonZeroCnt, rc->srcPrm / (rc->ctbSize * rc->ctbSize)));

}

/*------------------------------------------------------------------------------
  PicSkip()  Decrease framerate if not enough bits available.
------------------------------------------------------------------------------*/
void PicSkip(hevcRateControl_s *rc)
{
  hevcVirtualBuffer_s *vb = &rc->virtualBuffer;
  i32 bitAvailable = vb->virtualBitCnt - vb->realBitCnt;
  i32 skipIncLimit = -vb->bitPerPic / 3;
  i32 skipDecLimit = vb->bitPerPic / 3;

  /* When frameRc is enabled, skipFrameTarget is not allowed to be > 1
   * This makes sure that not too many frames is skipped and lets
   * the frameRc adjust QP instead of skipping many frames */
  if ((vb->skipFrameTarget == 0) &&
      (bitAvailable < skipIncLimit))
  {
    vb->skipFrameTarget++;
  }

  if ((bitAvailable > skipDecLimit) && vb->skipFrameTarget > 0)
  {
    vb->skipFrameTarget--;
  }

  if (vb->skippedFrames < vb->skipFrameTarget)
  {
    vb->skippedFrames++;
    rc->frameCoded = ENCHW_NO;
  }
  else
  {
    vb->skippedFrames = 0;
  }
}

/*------------------------------------------------------------------------------
  PicQuant()  Calculate quantization parameter for next frame. In the beginning
                of window use previous GOP average QP and otherwise find new QP
                using the target size and previous frames QPs and bit counts.
------------------------------------------------------------------------------*/
void PicQuant(hevcRateControl_s *rc)
{
  i32 normBits, targetBits;
  true_e useQpDeltaLimit = ENCHW_YES;

  const float ratio_qp[52]={0.87,0.85,0.84,0.82,0.81,0.78,0.76,0.75,0.73,0.72,0.70,0.68,0.6,0.55,0.5,0.45,0.4,0.35,0.3,0.25,0.2,0.15,0.1,0.09,0.08,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07,0.07};

  if (rc->picRc != ENCHW_YES)
  {
    rc->qpHdr = rc->fixedQp;
    DBG(1, (DBGOUTPUT, "R/cx:  xxxx  QP: xx xx D:  xxxx newQP: xx\n"));
    return;
  }

  /* If HRD is enabled we must make sure this frame fits in buffer */
  if (rc->hrd == ENCHW_YES)
  {
    i32 bitsAvailable =
      (rc->virtualBuffer.bufferSize - rc->virtualBuffer.bucketFullness);

    /* If the previous frame didn't fit the buffer we don't limit QP change */
    if (rc->frameBitCnt > bitsAvailable)
    {
      useQpDeltaLimit = ENCHW_NO;
    }
  }

  /* determine initial quantization parameter for current picture */
  if (rc->sliceTypeCur == I_SLICE)
  {

    /* Default intra QP == average of prev frame and prev GOP average */
    rc->qpHdr = (rc->qpHdrPrev + gop_avg_qp(rc) + 1) / 2;
    /* If all frames are intra we calculate new QP
     * for intra the same way as for inter */
    if (rc->sliceTypePrev == I_SLICE)
    {
      targetBits = rc->targetPicSize - avg_rc_error(&rc->intraError);
      targetBits = CLIP3(0, 2 * rc->targetPicSize, targetBits);
      normBits = HevcCalculate(targetBits, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
      rc->qpHdr = new_pic_quant(&rc->intra, normBits, useQpDeltaLimit);
    }
    else
    {
      DBG(1, (DBGOUTPUT, "R/cx:  xxxx  QP: xx xx D:  xxxx newQP: xx\n"));
    }
  }
  else
  {

    /* Calculate new QP by matching to previous inter frames R-Q curve */
    targetBits = rc->targetPicSize - avg_rc_error(&rc->rError);
    targetBits = CLIP3(0, 2 * rc->targetPicSize, targetBits);

    normBits = HevcCalculate(targetBits, DSCBITPERMB, rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16);
    //printf("\n normBits=%d,qpprev=%d\n",normBits,rc->linReg.qp_prev);
    rc->qpHdr = new_pic_quant(&rc->linReg, normBits, useQpDeltaLimit);

    if (rc->linReg.a1 == 0 && rc->linReg.a2 == 0)
    {
      if (rc->sliceTypePrev == I_SLICE && rc->sliceTypeCur == P_SLICE)
      {
        if(rc->intraQpDelta<0)
        {
          if ((rc->frameBitCnt > rc->virtualBuffer.bitPerPic/3))
          {
            i32 errorqp=0;
            float actual_bitrate;
            float divider_qp=1;
            i32 i;
            i32 test_bitrate;
            i32 test_qp;
            for(i=0;i<-rc->intraQpDelta;i++)
            {
              test_qp=rc->qpHdr-i;
              if(test_qp<0)
                test_qp=0;
              if(test_qp>51)
                test_qp=51;
              divider_qp*=(1+ratio_qp[test_qp]);
            }
            test_qp=rc->qpHdr+rc->intraQpDelta;
            if(test_qp<0)
                test_qp=0;
              if(test_qp>51)
                test_qp=51;
            actual_bitrate=rc->frameBitCnt+(float)rc->frameBitCnt*ratio_qp[test_qp]*((float)rc->outRateNum/rc->outRateDenom-1)/divider_qp;
            divider_qp=1;
            test_bitrate=actual_bitrate;
            if(test_bitrate>rc->virtualBuffer.bitRate)
            {
              for(i=0;;i++)
              {
                test_qp=rc->qpHdr+i;
                if(test_qp<0)
                    test_qp=0;
                if(test_qp>51)
                  test_qp=51;
                test_bitrate*=(1-ratio_qp[test_qp]);
                if(rc->virtualBuffer.bitRate>test_bitrate)
                  break;
                else
                {
                  errorqp-=1;
                }
              }
            }
            else
            {
              for(i=0;;i++)
              {
                test_qp=rc->qpHdr-i;
                if(test_qp<0)
                    test_qp=0;
                if(test_qp>51)
                  test_qp=51;
                test_bitrate*=(1+ratio_qp[test_qp]);
                //printf("test_bitrate=%d \n",test_bitrate);
                if(rc->virtualBuffer.bitRate<test_bitrate)
                  break;
                else
                {
                  errorqp+=1;
                }
              }
            }
            //errorqp= (i32)((float)log10((float)( rc->virtualBuffer.bitRate) / ((float)actual_bitrate))/(float)log10(1.1));/*((8 * rc->virtualBuffer.bitPerPic) / rc->frameBitCnt)*/

            rc->qpHdr =rc->qpHdr - errorqp;

            if (rc->qpHdr < 0)
              rc->qpHdr = 0;
            else if(rc->qpHdr>51)
              rc->qpHdr=51;

            if(rc->qpHdr==51 && rc->qpHdrPrev==51)
            {
              if(rc->intraQpDelta<-1)
                rc->intraQpDelta=-1;
             // printf("rc->intraQpDelta=%d\n",rc->intraQpDelta);
            }

            //rc->targetPicSize=rc->targetPicSize*pow(1.1,(u8)((float)log10((float)(8 * rc->virtualBuffer.bitPerPic) / (float)rc->frameBitCnt)/(float)log10(1.1))-rc->intraQpDelta);
            rc->trans_bit2intra=1;
            rc->linReg.qp_prev=rc->qpHdr;


          }
        }
        else
        {
        }

      }
    }


  }
}

/*------------------------------------------------------------------------------

  PicQuantLimit()

------------------------------------------------------------------------------*/
void PicQuantLimit(hevcRateControl_s *rc)
{
  rc->qpHdr = MIN(rc->qpMax, MAX(rc->qpMin, rc->qpHdr));
}

/*------------------------------------------------------------------------------

  Calculate()  I try to avoid overflow and calculate good enough result of a*b/c

------------------------------------------------------------------------------*/
i32 HevcCalculate(i32 a, i32 b, i32 c)
{
  u32 left = 32;
  u32 right = 0;
  u32 shift;
  i32 sign = 1;
  i32 tmp;

  if (a == 0 || b == 0)
  {
    return 0;
  }
  else if ((a * b / b) == a && c != 0)
  {
    return (a * b / c);
  }
  if (a < 0)
  {
    sign = -1;
    a = -a;
  }
  if (b < 0)
  {
    sign *= -1;
    b = -b;
  }
  if (c < 0)
  {
    sign *= -1;
    c = -c;
  }

  if (c == 0)
  {
    return 0x7FFFFFFF * sign;
  }

  if (b > a)
  {
    tmp = b;
    b = a;
    a = tmp;
  }

  for (--left; (((u32)a << left) >> left) != (u32)a; --left);
  left--; /* unsigned values have one more bit on left,
               we want signed accuracy. shifting signed values gives
               lint warnings */

  while (((u32)b >> right) > (u32)c)
  {
    right++;
  }

  if (right > left)
  {
    return 0x7FFFFFFF * sign;
  }
  else
  {
    shift = left - right;
    return (i32)((((u32)a << shift) / (u32)c * (u32)b) >> shift) * sign;
  }
}

/*------------------------------------------------------------------------------
  avg_rc_error()  PI(D)-control for rate prediction error.
------------------------------------------------------------------------------*/
static i32 avg_rc_error(linReg_s *p)
{
  if (ABS(p->bits[2]) < 0xFFFFFFF && ABS(p->bits[1]) < 0xFFFFFFF)
    return DIV(p->bits[2] * 8 + p->bits[1] * 4 + p->bits[0] * 0, 100);

  /* Avoid overflow */
  return HevcCalculate(p->bits[2], 8, 100) +
         HevcCalculate(p->bits[1], 4, 100);
}

/*------------------------------------------------------------------------------
  update_overhead()  Update PI(D)-control values
------------------------------------------------------------------------------*/
static void update_rc_error(linReg_s *p, i32 bits, i32 windowLen)
{
  p->len = 3;

  if (bits == (i32)0x7fffffff)
  {
    /* RESET */
    p->bits[0] = 0;
    if (windowLen)  /* Store the average error of previous GOP. */
      p->bits[1] = p->bits[1] / windowLen;
    else
      p->bits[1] = 0;
    p->bits[2] = 0;
    return;
  }
  p->bits[0] = bits - p->bits[2]; /* Derivative */
  if ((bits > 0) && (bits + p->bits[1] > p->bits[1]))
    p->bits[1] = bits + p->bits[1]; /* Integral */
  if ((bits < 0) && (bits + p->bits[1] < p->bits[1]))
    p->bits[1] = bits + p->bits[1]; /* Integral */
  p->bits[2] = bits;              /* Proportional */
  DBG(1, (DBGOUTPUT, "P %6d I %7d D %7d\n", p->bits[2],  p->bits[1], p->bits[0]));
}

/*------------------------------------------------------------------------------
  gop_avg_qp()  Average quantization parameter of P frames since previous I.
------------------------------------------------------------------------------*/
i32 gop_avg_qp(hevcRateControl_s *rc)
{
  i32 tmp = rc->qpHdrPrev;
  i32 maxIntraBitRatio = 95;  /* Percentage of total GOP bits. */

  /* Average QP of previous GOP inter frames */
  if (rc->gopQpSum && rc->gopQpDiv)
  {
    tmp = DIV(rc->gopQpSum, rc->gopQpDiv);
  }
  /* Average bit count per frame for previous GOP (intra + inter) */
  rc->gopAvgBitCnt = DIV(rc->gopBitCnt, (rc->gopQpDiv + 1));
  /* Ratio of intra_frame_bits/all_gop_bits % for previous GOP */
  if (rc->gopBitCnt)
  {
    i32 gopIntraBitRatio =
      HevcCalculate(get_avg_bits(&rc->intra, 1), rc->ctbPerPic * rc->ctbSize * rc->ctbSize / 16 / 16, DSCBITPERMB) * 100;
    gopIntraBitRatio = DIV(gopIntraBitRatio, rc->gopBitCnt);
    /* Limit GOP intra bit ratio to leave room for inters. */
    gopIntraBitRatio = MIN(maxIntraBitRatio, gopIntraBitRatio);
    update_tables(&rc->gop, tmp, gopIntraBitRatio);
  }
  rc->gopQpSum = 0;
  rc->gopQpDiv = 0;
  rc->gopBitCnt = 0;

  return tmp;
}

/*------------------------------------------------------------------------------
  new_pic_quant()  Calculate new quantization parameter from the 2nd degree R-Q
  equation. Further adjust Qp for "smoother" visual quality.
------------------------------------------------------------------------------*/
static i32 new_pic_quant(linReg_s *p, i32 bits, true_e useQpDeltaLimit)
{
  i32 tmp, qp_best = p->qp_prev, qp = p->qp_prev, diff;
  i32 diff_prev = 0, qp_prev = 0, diff_best = 0x7FFFFFFF;

  DBG(1, (DBGOUTPUT, "R/cx:%6d ", bits));

  if (p->a1 == 0 && p->a2 == 0)
  {
    DBG(1, (DBGOUTPUT, " QP: xx xx D:  ==== newQP: %2d\n", qp));
    return qp;
  }
  /* This can make QP shoot to max too easily.
  if (bits <= 0) {
      if (useQpDeltaLimit)
          qp = MIN(51, MAX(0, qp + QP_DELTA));
      else
          qp = MIN(51, MAX(0, qp + 10));

      DBG(1, (DBGOUTPUT, " QP: xx xx D:  ---- newQP: %2d\n", qp));
      return qp;
  }*/

  do
  {
    tmp  = DIV(p->a1, q_step[qp]);
    tmp += DIV(p->a2, q_step[qp] * q_step[qp]);
    diff = ABS(tmp - bits);

    if (diff < diff_best)
    {
      if (diff_best == 0x7FFFFFFF)
      {
        diff_prev = diff;
        qp_prev   = qp;
      }
      else
      {
        diff_prev = diff_best;
        qp_prev   = qp_best;
      }
      diff_best = diff;
      qp_best   = qp;
      if ((tmp - bits) <= 0)
      {
        if (qp < 1)
        {
          break;
        }
        qp--;
      }
      else
      {
        if (qp > 50)
        {
          break;
        }
        qp++;
      }
    }
    else
    {
      break;
    }
  }
  while ((qp >= 0) && (qp <= 51));
  qp = qp_best;

  DBG(1, (DBGOUTPUT, " QP: %2d %2d D: %5d", qp, qp_prev, diff_prev - diff_best));

  /* One unit change in Qp changes rate about 12% ca. 1/8. If the
   * difference is less than half use the one closer to previous value */
  if (ABS(diff_prev - diff_best) <= ABS(bits) / 16)
  {
    qp = qp_prev;
  }
  DBG(1, (DBGOUTPUT, " newQP: %2d\n", qp));

  /* Limit Qp change for smoother visual quality */
  if (useQpDeltaLimit)
  {
    tmp = qp - p->qp_prev;

    if (tmp > QP_DELTA)
    {
      qp = p->qp_prev + QP_DELTA;
      /* When QP is totally wrong, allow faster QP increase */
      if (tmp > QP_DELTA_LIMIT)
        qp = p->qp_prev + QP_DELTA * 2;
    }
    else if (tmp < -QP_DELTA)
    {
      qp = p->qp_prev - QP_DELTA;
      if (tmp < -QP_DELTA_LIMIT)
        qp = p->qp_prev + tmp * 2 / 3;
    }
  }

  return qp;
}

/*------------------------------------------------------------------------------
  get_avg_bits()
------------------------------------------------------------------------------*/
static i32 get_avg_bits(linReg_s *p, i32 n)
{
  i32 i;
  i32 sum = 0;
  i32 pos = p->pos;

  if (!p->len) return 0;

  if (n == -1 || n > p->len)
    n = p->len;

  i = n;
  while (i--)
  {
    if (pos) pos--;
    else pos = p->len - 1;
    sum += p->bits[pos];
    if (sum < 0)
    {
      return I32_MAX / (n - i);
    }
  }
  return DIV(sum, n);
}

/*------------------------------------------------------------------------------
  update_tables()  only statistics of PSLICE, please.
------------------------------------------------------------------------------*/
static void update_tables(linReg_s *p, i32 qp, i32 bits)
{
  const i32 clen = RC_TABLE_LENGTH;
  i32 tmp = p->pos;

  p->qp_prev   = qp;
  p->qs[tmp]   = q_step[qp];
  p->bits[tmp] = bits;

  if (++p->pos >= clen)
  {
    p->pos = 0;
  }
  if (p->len < clen)
  {
    p->len++;
  }
}

/*------------------------------------------------------------------------------
            update_model()  Update model parameter by Linear Regression.
------------------------------------------------------------------------------*/
static void update_model(linReg_s *p)
{
  i32 *qs = p->qs, *r = p->bits, n = p->len;
  i32 i, a1, a2, sx = lin_sx(qs, n), sy = lin_sy(qs, r, n);

  for (i = 0; i < n; i++)
  {
    DBG(2, (DBGOUTPUT, "model: qs %i  r %i\n", qs[i], r[i]));
  }

  a1 = lin_sxy(qs, r, n);
  a1 = a1 < I32_MAX / n ? a1 * n : I32_MAX;

  if (sy == 0)
  {
    a1 = 0;
  }
  else
  {
    a1 -= sx < I32_MAX / sy ? sx * sy : I32_MAX;
  }

  a2 = (lin_nsxx(qs, n) - (sx * sx));
  if (a2 == 0)
  {
    if (p->a1 == 0)
    {
      /* If encountered in the beginning */
      a1 = 0;
    }
    else
    {
      a1 = (p->a1 * 2) / 3;
    }
  }
  else
  {
    a1 = HevcCalculate(a1, DSCY, a2);
  }

  /* Value of a1 shouldn't be excessive (small) */
  a1 = MAX(a1, -262144);
  a1 = MIN(a1,  262143);
  a1 = MAX(a1, -I32_MAX / q_step[51] / RC_TABLE_LENGTH);
  a1 = MIN(a1,  I32_MAX / q_step[51] / RC_TABLE_LENGTH);

  ASSERT(ABS(a1) * sx >= 0);
  ASSERT(sx * DSCY >= 0);
  a2 = DIV(sy * DSCY, n) - DIV(a1 * sx, n);

  DBG(2, (DBGOUTPUT, "model: a2:%9d  a1:%8d\n", a2, a1));

  if (p->len > 0)
  {
    p->a1 = a1;
    p->a2 = a2;
  }
}

/*------------------------------------------------------------------------------
  lin_sy()  calculate value of Sy for n points.
------------------------------------------------------------------------------*/
static i32 lin_sy(i32 *qp, i32 *r, i32 n)
{
  i32 sum = 0;

  while (n--)
  {
    sum += qp[n] * qp[n] * r[n];
    if (sum < 0)
    {
      return I32_MAX / DSCY;
    }
  }
  return DIV(sum, DSCY);
}

/*------------------------------------------------------------------------------
  lin_sx()  calculate value of Sx for n points.
------------------------------------------------------------------------------*/
static i32 lin_sx(i32 *qp, i32 n)
{
  i32 tmp = 0;

  while (n--)
  {
    ASSERT(qp[n]);
    tmp += qp[n];
  }
  return tmp;
}

/*------------------------------------------------------------------------------
  lin_sxy()  calculate value of Sxy for n points.
------------------------------------------------------------------------------*/
static i32 lin_sxy(i32 *qp, i32 *r, i32 n)
{
  i32 tmp, sum = 0;

  while (n--)
  {
    tmp = qp[n] * qp[n] * qp[n];
    if (tmp > r[n])
    {
      sum += DIV(tmp, DSCY) * r[n];
    }
    else
    {
      sum += tmp * DIV(r[n], DSCY);
    }
    if (sum < 0)
    {
      return I32_MAX;
    }
  }
  return sum;
}

/*------------------------------------------------------------------------------
  lin_nsxx()  calculate value of n * Sxy for n points.
------------------------------------------------------------------------------*/
static i32 lin_nsxx(i32 *qp, i32 n)
{
  i32 tmp = 0, sum = 0, d = n ;

  while (n--)
  {
    tmp = qp[n];
    tmp *= tmp;
    sum += d * tmp;
  }
  return sum;
}





