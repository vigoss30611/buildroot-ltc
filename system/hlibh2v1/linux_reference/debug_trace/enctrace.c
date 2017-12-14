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
--  Description : Internal traces
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "enctrace.h"
#include "queue.h"
#include "error.h"
#include "instance.h"
#include "sw_slice.h"





/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/
static FILE *fileAsic = NULL;
static FILE *filePreProcess = NULL;
static FILE *fRegs = NULL;
static FILE *fRecon = NULL;
static FILE *fTrace = NULL;
static FILE *fTraceProbs = NULL;
static FILE *fTraceSegments = NULL;
static FILE *fDump = NULL;

#define HSWREG(n)       ((n)*4)

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
struct enc_sw_trace
{
  struct container *container;
  struct queue files;         /* Open files */
  struct queue stream_trace;      /* Stream trace buffer store */

  FILE *stream_trace_fp;          /* Stream trace file */
  FILE *deblock_fp;           /* deblock.yuv file */
  //    FILE *prof_fp;              /* profile.yuv file, record the bitrate and PSNR */

  int trace_frame_id;
  int cur_frame_idx;
  bool bTraceCurFrame;
};

struct enc_sw_open_file
{
  struct node *next;
  FILE *file;
};

static struct enc_sw_trace ctrl_sw_trace;

static FILE *Enc_sw_open_file(FILE *file, char *name);

/*------------------------------------------------------------------------------
  open_file
------------------------------------------------------------------------------*/
int Enc_get_param(FILE *file, char *name)
{
  char buffer[FILENAME_MAX];
  char bufferT[FILENAME_MAX];


  int val;

  ASSERT(file && name);
  rewind(file);
  while (fgets(buffer, FILENAME_MAX, file))
  {
    sscanf(buffer, "%s %d\n", bufferT, &val);   /* Pick first word */
    if (!strcmp(name, bufferT))
    {
      return val;
    }
  }

  return -1;
}

/*------------------------------------------------------------------------------
  test_data_init
------------------------------------------------------------------------------*/
#define DEFAULT_TB_CFG_FILENAME "tb.cfg"

i32 Enc_test_data_init(void)
{
  FILE *fp;

  memset(&ctrl_sw_trace, 0, sizeof(struct enc_sw_trace));

  if ((getenv("TEST_DATA_FILES") == NULL))
  {
    fp = fopen(DEFAULT_TB_CFG_FILENAME, "r");
    //fprintf(stderr, "Generating traces from default file <%s>.\n", DEFAULT_TB_CFG_FILENAME);
  }
  else
  {
    fp = fopen(getenv("TEST_DATA_FILES"), "r");
    //fprintf(stderr, "Generating traces from <%s>\n", getenv("TEST_DATA_FILES"));
  }

  if (fp == NULL)
  {
    //fprintf(stderr, "Cannot open trace configuration file.\n");
    //Error(4, ERR, "tb.cfg", ", ", SYSERR);
    return NOK;
  }

  if ((getenv("TEST_DATA_FILES") == NULL))
  {
    printf("Generating traces by <%s>\n", DEFAULT_TB_CFG_FILENAME);
  }
  else
  {
    printf("Generating traces by <%s>\n", getenv("TEST_DATA_FILES"));
  }

  ctrl_sw_trace.stream_trace_fp = Enc_sw_open_file(fp, "stream.trc");

  ctrl_sw_trace.cur_frame_idx = 0;
  ctrl_sw_trace.trace_frame_id = Enc_get_param(fp, "trace_frame_id");
  ctrl_sw_trace.bTraceCurFrame = ((ctrl_sw_trace.trace_frame_id == -1) || (ctrl_sw_trace.trace_frame_id == ctrl_sw_trace.cur_frame_idx));

  fclose(fp);

  return OK;
}

/*------------------------------------------------------------------------------
  test_data_release
------------------------------------------------------------------------------*/
void Enc_test_data_release(void)
{
  struct node *n;


  /* Close open files */
  while ((n = queue_get(&ctrl_sw_trace.files)))
  {
    fclose(((struct enc_sw_open_file *)n)->file);
    free(n);
  }

  return;
}

static FILE *Enc_sw_open_file(FILE *file, char *name)
{
  char buffer[FILENAME_MAX];
  FILE *fp;

  ASSERT(file && name);
  rewind(file);
  while (fgets(buffer, FILENAME_MAX, file))
  {
    sscanf(buffer, "%s\n", buffer);   /* Pick first word */
    if (!strcmp(name, buffer) || !strcmp("ALL", buffer))
    {
      struct enc_sw_open_file *n;   /* Close file node */
      if (!(n = malloc(sizeof(struct enc_sw_open_file))))
      {
        Error(2, ERR, SYSERR);
        return NULL;
      }
      if (!(fp = fopen(name, "wb")))
      {
        Error(4, ERR, name, ", ", SYSERR);
        free(n);
        return NULL;
      }
      n->file = fp;
      queue_put(&ctrl_sw_trace.files, (struct node *)n);
      return fp;
    }
  }

  return NULL;
}


#ifdef TEST_DATA
/*------------------------------------------------------------------------------
  open_stream_trace
------------------------------------------------------------------------------*/
i32 Enc_open_stream_trace(struct buffer *b)
{
  struct stream_trace *stream_trace;

  ASSERT(b);

  if (!ctrl_sw_trace.stream_trace_fp) return OK;

  stream_trace = malloc(sizeof(struct stream_trace));
  if (!stream_trace) goto error;

  memset(stream_trace, 0, sizeof(struct stream_trace));
  stream_trace->fp = open_memstream(&stream_trace->buffer,
                                    &stream_trace->size);
  if (!stream_trace->fp) goto error;

  b->stream_trace = stream_trace;
  fprintf(stream_trace->fp, "Next buffer\n");
  queue_put(&ctrl_sw_trace.stream_trace, (struct node *)stream_trace);
  return OK;

error:
  free(stream_trace);
  return NOK;
}

/*------------------------------------------------------------------------------
  close_stream_trace
------------------------------------------------------------------------------*/
i32 Enc_close_stream_trace(void)
{
  struct stream_trace *stream_trace;
  struct node *n;
  size_t cnt;
  i32 ret = OK;

  while ((n = queue_get(&ctrl_sw_trace.stream_trace)))
  {
    stream_trace = (struct stream_trace *)n;
    fclose(stream_trace->fp);
    cnt = fwrite(stream_trace->buffer, sizeof(char),
                 stream_trace->size, ctrl_sw_trace.stream_trace_fp);
    fflush(ctrl_sw_trace.stream_trace_fp);
    if (cnt != stream_trace->size)
    {
      Error(2, ERR, "write_stream_trace()");
      ret = NOK;
    }
    free(stream_trace->buffer);
    free(stream_trace);
  }
  return ret;
}


/*------------------------------------------------------------------------------
  Enc_add_comment
------------------------------------------------------------------------------*/
void Enc_add_comment(struct buffer *b, i32 value, i32 number, char *comment)
{
  FILE *fp;
  i32 i;

  if (!b->stream_trace) return;

  fp = b->stream_trace->fp;
  if (!comment)
  {
    fprintf(fp, "      %4i%2i ", value, number);
    comment = b->stream_trace->comment;
  }
  else
  {
    fprintf(fp, "%6i    %02X ", b->stream_trace->cnt, value);
    b->stream_trace->cnt++;
  }

  if (buffer_full(b))
    comment = "FAIL: BUFFER FULL";

  for (i = number; i; i--)
  {
    if (value & (1 << (i - 1)))
    {
      fprintf(fp, "1");
    }
    else
    {
      fprintf(fp, "0");
    }
  }
  for (i = number; i < 10; i++)
  {
    fprintf(fp, " ");
  }
  fprintf(fp, "%s\n", comment);
  b->stream_trace->comment[0] = '\0';
}
#endif

void EncTraceUpdateStatus()
{
  //Update trace frame count
  ctrl_sw_trace.cur_frame_idx ++;
  ctrl_sw_trace.bTraceCurFrame = ((ctrl_sw_trace.trace_frame_id == -1) || (ctrl_sw_trace.trace_frame_id == ctrl_sw_trace.cur_frame_idx));
}


/*------------------------------------------------------------------------------
  write
------------------------------------------------------------------------------*/
static void write(FILE *fp, u8 *data, i32 width, i32 height, i32 stripe)
{
  i32 i;

  for (i = 0; i < height; i++)
  {
    if (fwrite(data, sizeof(u8), width, fp) < (size_t)width)
    {
      Error(2, ERR, SYSERR);
      return;
    }
    data += stripe;
  }
}

/*------------------------------------------------------------------------------

    h265_EncTraceRegs

------------------------------------------------------------------------------*/
void h265_EncTraceRegs(const void *ewl, u32 readWriteFlag, u32 mbNum)
{
  i32 i;
  i32 lastRegAddr = ASIC_SWREG_AMOUNT * 4; //0x48C;
  char rw = 'W';
  static i32 frame = 0;

  if (fRegs == NULL)
    fRegs = fopen("sw_reg.trc", "w");

  if (fRegs == NULL)
    fRegs = stderr;

  fprintf(fRegs, "pic=%d\n", frame);
  fprintf(fRegs, "mb=%d\n", mbNum);

  /* After frame is finished, registers are read */
  if (readWriteFlag)
  {
    rw = 'R';
    frame++;
  }

  /* Dump registers in same denali format as the system model */
  for (i = 0; i < lastRegAddr; i += 4)
  {
    /* DMV penalty tables not possible to read from ASIC: 0x180-0x27C */
    //if ((i != 0xA0) && (i != 0x38) && (i < 0x180 || i > 0x27C))
    if (i != HSWREG(ASIC_REG_INDEX_STATUS))
      fprintf(fRegs, "%c %08x/%08x\n", rw, i, h2_EWLReadReg(ewl, i));
  }

  /* Regs with enable bits last, force encoder enable high for frame start */
  fprintf(fRegs, "%c %08x/%08x\n", rw, HSWREG(ASIC_REG_INDEX_STATUS),
          h2_EWLReadReg(ewl, HSWREG(ASIC_REG_INDEX_STATUS)) | (readWriteFlag == 0));

  fprintf(fRegs, "\n");

  /*fclose(fRegs);
   * fRegs = NULL; */

}

void EncTraceReconEnd()
{
  if(fRecon)
  {
    fclose(fRecon);
    fRecon = NULL;
  }
}

void EncTraceRecon(HEVCEncInst inst, i32 poc)
{
  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  struct container *c;
  struct sw_picture *pic;
  asicData_s *asic = &hevc_instance->asic;

  i32 width, height, stripe, offset;

  if (fRecon == NULL)
    fRecon = fopen("deblock.yuv", "w");
  if (!fRecon) return;

  c = get_container(hevc_instance);
  pic = get_picture(c, poc);
  if (hevc_instance->rateControl.frameCoded == ENCHW_NO)
  {
    printf("frame skipped \n");
    return;
  }
#ifdef INTERNAL_DECOMPRESS_RECON_TEST
  //decompress
  extern int decompress_luma(u8 * pixel, u8 * comLum, int width, int height, int stride, u8 * table);
  extern int decompress_chroma(u8 * pixelCb, u8 * pixelCr, u8 * dstCh, int src_width, int height, int stride, u8 * table);
  u8 *lum = NULL;
  u8 *cb = NULL;
  u8 *cr = NULL;

  width = pic->sps->width;
  height = pic->sps->height;
  stripe = pic->recon.lum_width;
  offset = (pic->sps->frameCropTopOffset * 2) * stripe + (pic->sps->frameCropLeftOffset * 2);

  int de_w = (width + 7) >> 3 << 3;
  int de_h = (height + 7) >> 3 << 3;
  if (pic->recon_compress.lumaCompressed && (lum = (u8 *)malloc(stripe * de_h)))
    decompress_luma(lum, (u8 *)(asic->internalreconLuma[pic->picture_memeory_id].virtualAddress), de_w, de_h, stripe, (u8 *)asic->compressTbl[pic->picture_memeory_id].virtualAddress);
  write(fRecon, (lum ? lum : ((u8 *)asic->internalreconLuma[pic->picture_memeory_id].virtualAddress)) + offset, width, height, stripe);

  width = (width + 1) >> 1;
  height = (height + 1) >> 1;
  stripe = pic->recon.ch_width;
  offset = pic->sps->frameCropTopOffset * stripe + pic->sps->frameCropLeftOffset;

  de_w = de_w >> 1;
  de_h = de_h >> 1;
  {
    u32 lumaTblSize = asic->regs.recon_luma_compress ?
                      (((pic->sps->width + 63) / 64) * ((pic->sps->height + 63) / 64) * 8) : 0;
    if (pic->recon_compress.chromaCompressed && (cb = (u8 *)malloc(stripe * de_h)) && (cr = (u8 *)malloc(stripe * de_h)))
      decompress_chroma(cb, cr, (u8 *)asic->internalreconChroma[pic->picture_memeory_id].virtualAddress, de_w, de_h, stripe, (u8 *)asic->compressTbl[pic->picture_memeory_id].virtualAddress + lumaTblSize);
    write(fRecon, (cb ? cb : ((u8 *)asic->internalreconChroma[pic->picture_memeory_id].virtualAddress)) + offset, width, height, stripe);
    write(fRecon, (cr ? cr : ((u8 *)asic->internalreconChroma[pic->picture_memeory_id].virtualAddress + asic->regs.recon_chroma_half_size)) + offset, width, height, stripe);
  }

  if (lum) free(lum);
  if (cb) free(cb);
  if (cr) free(cr);
  return;
#endif

  width = pic->sps->width;
  height = pic->sps->height;
  stripe = pic->recon.lum_width;
  offset = (pic->sps->frameCropTopOffset * 2) * stripe + (pic->sps->frameCropLeftOffset * 2);
  write(fRecon, (u8 *)asic->internalreconLuma[pic->picture_memeory_id].virtualAddress + offset, width, height, stripe);

  width = (width + 1) >> 1;
  height = (height + 1) >> 1;
  stripe = pic->recon.ch_width;
  offset = pic->sps->frameCropTopOffset * stripe + pic->sps->frameCropLeftOffset;
  write(fRecon, (u8 *)asic->internalreconChroma[pic->picture_memeory_id].virtualAddress + offset, width, height, stripe);
  write(fRecon, ((u8 *)asic->internalreconChroma[pic->picture_memeory_id].virtualAddress + asic->regs.recon_chroma_half_size) + offset, width, height, stripe);
}



