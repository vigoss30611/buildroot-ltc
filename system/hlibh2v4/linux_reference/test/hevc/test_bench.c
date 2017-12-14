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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include "get_option.h"
#include "base_type.h"
//#include "version.h"
#include "error.h"
#include "hevcencapi.h"
#include "HevcTestBench.h"
#ifdef TEST_DATA
#include "enctrace.h"
#endif
#include "encasiccontroller.h"
#include "instance.h"

#define HEVCERR_OUTPUT stdout
#define MAX_GOP_LEN 300
#define MOVING_AVERAGE_FRAMES    120
#define MAXARGS 128
#define CMDLENMAX 256



struct test_bench
{
  char *input;
  char *output;
  char *test_data_files;
  FILE *in;
  FILE *out;
  i32 width;
  i32 height;
  i32 outputRateNumer;      /* Output frame rate numerator */
  i32 outputRateDenom;      /* Output frame rate denominator */
  i32 inputRateNumer;      /* Input frame rate numerator */
  i32 inputRateDenom;      /* Input frame rate denominator */
  i32 firstPic;
  i32 lastPic;
  i32 picture_cnt;
  i32 picture_enc_cnt;
  i32 idr_interval;
  i32 last_idr_picture_cnt;
  i32 byteStream;
  u8 *lum;
  u8 *cb;
  u8 *cr;
  i32 interlacedFrame;
  u32 validencodedframenumber;

  char **argv;      /* Command line parameter... */
  i32 argc;
  /* Moved from global space */
  FILE *yuvFile;
  //static FILE *nalfile = NULL;

  /* SW/HW shared memories for input/output buffers */
  EWLLinearMem_t pictureMem;
  EWLLinearMem_t pictureStabMem;
  EWLLinearMem_t outbufMem;
  EWLLinearMem_t roiMapDeltaQpMem;

  EWLLinearMem_t scaledPictureMem;
  float sumsquareoferror;
  float averagesquareoferror;
  i32 maxerrorovertarget;
  i32 maxerrorundertarget;
  long numbersquareoferror;

  u32 gopSize;
  HEVCEncIn encIn;
  u8 gopCfgOffset[MAX_GOP_SIZE + 1];

  //extern int static_gopsize;
  //extern signed int statics_mv_x_avg;
  //extern signed int statics_mv_y_avg;
  //extern signed int static_cost4N[2];

  //static char *yuvFileName = NULL;
  //static FILE *yuvFileMvc = NULL;
  //static off_t file_size;
};

static struct option options[] =
{
  {"help",    'H', 2},
  {"firstPic", 'a', 1},
  {"lastPic",  'b', 1},
  {"width", 'x', 1},
  {"height", 'y', 1},
  {"lumWidthSrc", 'w', 1},
  {"lumHeightSrc", 'h', 1},
  {"horOffsetSrc", 'X', 1},
  {"verOffsetSrc", 'Y', 1},
  {"inputFormat", 'l', 1},        /* Input image format */
  {"colorConversion", 'O', 1},    /* RGB to YCbCr conversion type */
  {"rotation", 'r', 1},           /* Input image rotation */
  {"outputRateNumer", 'f', 1},
  {"outputRateDenom", 'F', 1},
  {"inputRateNumer", 'j', 1},
  {"inputRateDenom", 'J', 1},
  {"input",   'i', 1},
  {"output",    'o', 1},
  {"test_data_files", 'T', 1},
  {"cabacInitFlag", 'p', 1},
  {"qp_size",   'Q', 1},
  {"qpMin", 'n', 1},              /* Minimum frame header qp */
  {"qpMax", 'm', 1},              /* Maximum frame header qp */
  {"qpHdr",     'q', 1},
  {"hrdConformance", 'C', 1},     /* HDR Conformance (ANNEX C) */
  {"cpbSize", 'c', 1},            /* Coded Picture Buffer Size */
  {"intraQpDelta", 'A', 1},       /* QP adjustment for intra frames */
  {"fixedIntraQp", 'G', 1},       /* Fixed QP for all intra frames */
  {"bFrameQpDelta", 'V', 1},       /* QP adjustment for B frames */
  {"byteStream", 'N', 1},         /* Byte stream format (ANNEX B) */
  {"bitPerSecond", 'B', 1},
  {"picRc",   'U', 1},
  {"ctbRc",   'u', 1},
  {"picSkip", 's', 1},            /* Frame skipping */
  {"profile",       'P', 1},   /* profile   (ANNEX A):support main and main still picture */
  {"level",         'L', 1},   /* Level * 30  (ANNEX A) */
  {"intraPicRate",  'R', 1},   /* IDR interval */
  {"bpsAdjust", '1', 1},          /* Setting bitrate on the fly */
  {"gopLength", 'g', 1},          /* group of pictures length */
  {"disableDeblocking", 'D', 1},
  {"tc_Offset", 'W', 1},
  {"beta_Offset", 'E', 1},
  {"sliceSize", 'e', 1},
  {"chromaQpOffset", 'I', 1},     /* Chroma qp index offset */
  {"enableSao", 'M', 1},          /* Enable or disable SAO */
  {"videoRange", 'k', 1},
  {"sei", 'S', 1},                /* SEI messages */
  {"userData", 'z', 1},           /* SEI User data file */




  /* Only long option can be used for all the following parameters because
   * we have no more letters to use. All shortOpt=0 will be identified by
   * long option. */
  {"cir", '0', 1},
  {"intraArea", '0', 1},
  {"roi1Area", '0', 1},
  {"roi2Area", '0', 1},
  {"roi1DeltaQp", '0', 1},
  {"roi2DeltaQp", '0', 1},
  
  {"roiMapDeltaQpBlockUnit", '0', 1},
  {"roiMapDeltaQpEnable", '0', 1},

  {"outBufSizeMax", '0', 1},

  {"constrainIntra", '0', 1},
  {"smoothingIntra", '0', 1},
  {"scaledWidth", '0', 1},
  {"scaledHeight", '0', 1},
  {"enableDeblockOverride", '0', 1},
  {"deblockOverride", '0', 1},
  {"enableScalingList", '0', 1},
  {"compressor", '0', 1},
  {"testId", '0', 1},            /* TestID for generate vector. */
  {"gopSize", '0', 1},
  {"gopConfig", '0', 1},
  {"gopLowdelay", '0', 1},
  {"interlacedFrame", '0', 1},
  {"fieldOrder", '0', 1},
  {"outReconFrame", '0', 1},
  
  {"gdrDuration", '0', 1},
  {"bitVarRangeI", '0', 1},
  {"bitVarRangeP", '0', 1},
  {"bitVarRangeB", '0', 1},
  
  {"tolMovingBitRate", '0', 1},
  {"monitorFrames", '0', 1},
  {"roiMapDeltaQpFile", '0', 1},
  //WIENER_DENOISE
  {"noiseReductionEnable", '0', 1},
  {"noiseLow", '0', 1},
  {"noiseFirstFrameSigma", '0', 1},
  /* multi stream configs */
  {"multimode", '0', 1}, // Multi-stream mode, 0--disable, 1--mult-thread, 2--multi-process
  {"streamcfg", '0', 1}, // extra stream config.
  {"bitDepthLuma",   '0', 1},   /* luma bit depth */
  {"bitDepthChroma",   '0', 1},   /* chroma bit depth */
  {"blockRCSize",   '0', 1},   /*block rc size */
  

  {NULL,      0,   0}        /* Format of last line */
};


typedef struct {
    i32 frame[MOVING_AVERAGE_FRAMES];
    i32 length;
    i32 count;
    i32 pos;
    i32 frameRateNumer;
    i32 frameRateDenom;
} ma_s;

typedef struct {
  u8 *strmPtr;
  u32 multislice_encoding;
  u32 output_byte_stream;
  FILE *outStreamFile;
} SliceCtl_s;

static int parse_stream_cfg(const char *streamcfg, commandLine_s *pcml);
static void *thread_main(void *arg);
static int run_instance(commandLine_s  *cml);
static i32 encode(struct test_bench *tb, HEVCEncInst encoder, commandLine_s *cml);
static i32 Parameter_Get(i32 argc, char **argv, commandLine_s *cml);
static void default_parameter(commandLine_s *cml);
static void help(void);
static u64 next_picture(struct test_bench *tb, int picture_cnt);
static i32 read_picture(struct test_bench *tb, u32 inputFormat, u32 src_img_size, u32 src_width, u32 src_height);
static i32 change_input(struct test_bench *tb);
static FILE *open_file(char *name, char *mode);
static i32 file_read(FILE *file, u8 *data, u64 seek, size_t size);
static u32 GetResolution(char *filename, i32 *pWidth, i32 *pHeight);
static int OpenEncoder(commandLine_s *cml, HEVCEncInst *pEnc, struct test_bench *tb);
static void CloseEncoder(HEVCEncInst encoder, struct test_bench *tb);
static int AllocRes(commandLine_s *cmdl, HEVCEncInst enc, struct test_bench *tb);
static void FreeRes(HEVCEncInst enc, struct test_bench *tb);
static void HEVCSliceReady(HEVCEncSliceReady *slice);
static void WriteStrm(FILE *fout, u32 *strmbuf, u32 size, u32 endian);
static void WriteNals(FILE *fout, u32 *strmbuf, const u32 *pNaluSizeBuf, u32 numNalus, u32 endian);
#if 0
static void WriteNalSizesToFile(FILE *file, const u32 *pNaluSizeBuf,
                                u32 numNalus);
#endif
static u8 *ReadUserData(HEVCEncInst encoder, char *name);
static void WriteScaled(u32 *strmbuf, HEVCEncInst inst);
static int HEVCInitGopConfigs(int gopSize, commandLine_s *cml, HEVCGopConfig *gopCfg, u8 *gopCfgOffset);
static i32 CheckArea(HEVCEncPictureArea *area, commandLine_s *cml);
static void MaAddFrame(ma_s *ma, i32 frameSizeBits);
static char *nextToken (char *str);
static i32 Ma(ma_s *ma);




/*------------------------------------------------------------------------------
  main
------------------------------------------------------------------------------*/
int main(i32 argc, char **argv)
{
  i32 ret = OK;
  commandLine_s  cml, *pcml;
  //HEVCEncConfig config;

  HEVCEncApiVersion apiVer;
  HEVCEncBuild encBuild;
  pthread_attr_t attr;
  pthread_t tid;
  int pid;
  int status;
  int i;


  apiVer = HEVCEncGetApiVersion();
  encBuild = HEVCEncGetBuild();

  fprintf(stdout, "HEVC Encoder API version %d.%d\n", apiVer.major,
          apiVer.minor);
  fprintf(stdout, "HW ID: %c%c 0x%08x\t SW Build: %u.%u.%u\n",
          encBuild.hwBuild >> 24, (encBuild.hwBuild >> 16) & 0xff,
          encBuild.hwBuild, encBuild.swBuild / 1000000,
          (encBuild.swBuild / 1000) % 1000, encBuild.swBuild % 1000);
  //fprintf(stdout, "GIT version %s\n\n", git_version);

  memset(&cml, 0, sizeof(commandLine_s));

  default_parameter(&cml);

  if (argc < 2)
  {
    help();
    exit(0);
  }
#if 0
  remove("nal_sizes.txt");

  nalfile = fopen("nal_sizes.txt", "wb");
#endif

  if (Parameter_Get(argc, argv, &cml))
  {
    Error(2, ERR, "Input parameter error");
    return NOK;
  }
  if(encBuild.hwBuild<=0x48320101)
  {
    //H2V1 only support 1.
    cml.gopSize = 1;
  }

  if(cml.nstream > 0) {
    if(cml.multimode == 1) {
      /* multi thread */
      for(i = 0; i < cml.nstream; i++) {
        pcml = cml.streamcml[i] = (commandLine_s*)malloc(sizeof(commandLine_s));
        parse_stream_cfg(cml.streamcfg[i], pcml);
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, &thread_main, pcml);
        cml.tid[i] = tid;
      }
    } else if(cml.multimode == 2) {
      /* multi process */
      for(i = 0; i < cml.nstream; i++) {
        pcml = cml.streamcml[i] = (commandLine_s*)malloc(sizeof(commandLine_s));
        parse_stream_cfg(cml.streamcfg[i], pcml);
        pcml->recon = malloc(255);
        sprintf(pcml->recon, "deblock%d.yuv", i+1);
        if(0 == (pid = fork())) {
          return run_instance(pcml);
        } else if(pid > 0) {
          cml.pid[i] = pid;
        } else {
          perror("failed to fork new process to process streams");
          exit(pid);
        }
      }
    } else {
      if(cml.multimode == 0) {
        printf("multi-stream disabled, ignore extra stream configs\n");
      } else {
        printf("Invalid multi stream mode\n");
        exit(-1);
      }
    }
  }
  cml.argc = argc;
  cml.argv = argv;
  ret = run_instance(&cml);
  if(cml.multimode == 1) {
    for(i = 0; i < cml.nstream; i++)
      if(cml.tid[i] > 0)
        pthread_join(cml.tid[i],NULL);
  }
  else if(cml.multimode == 2) {
    for(i = 0; i < cml.nstream; i++)
      if(cml.pid[i] > 0)
        waitpid(cml.pid[i], &status, 0);
  }
  if(cml.multimode != 0) {
    for(i = 0; i < cml.nstream; i++)
      free(cml.streamcml[i]);
  }
  return ret;
}

int parse_stream_cfg(const char *streamcfg, commandLine_s *pcml)
{
  i32 ret, i;
  char *p;
  FILE *fp = fopen(streamcfg, "r");
  default_parameter(pcml);
  if(fp == NULL)
    return NOK;
  pcml->argv = (char **)malloc(MAXARGS*sizeof(char *));
  pcml->argv[0] = (char *)malloc(CMDLENMAX);
  ret = fread(pcml->argv[0], 1, CMDLENMAX, fp);
  if(ret < 0)
    return ret;
  fclose(fp);

  p = pcml->argv[0];
  for(i = 1; i < MAXARGS; i++) {
    while(*p && *p <= 32)
      ++p;
    if(!*p) break;
    pcml->argv[i] = p;
    while(*p > 32)
      ++p;
    *p = 0; ++p;
  }
  pcml->argc = i;
  if (Parameter_Get(pcml->argc, pcml->argv, pcml))
  {
    Error(2, ERR, "Input parameter error");
    return NOK;
  }
  return OK;
}

void *thread_main(void *arg) {
  run_instance((commandLine_s *)arg);
  pthread_exit(NULL);
}

int run_instance(commandLine_s  *cml) {
  struct test_bench  tb;
  HEVCEncInst hevc_encoder;
  HEVCGopPicConfig gopPicCfg[MAX_GOP_PIC_CONFIG_NUM];
  i32 ret = OK;

  tb.sumsquareoferror = 0;
  tb.averagesquareoferror = 0;
  tb.maxerrorovertarget = 0;
  tb.maxerrorundertarget = 0;
  tb.numbersquareoferror = 0;

  /* Check that input file exists */
  tb.yuvFile = fopen(cml->input, "rb");

  if (tb.yuvFile == NULL) {
    fprintf(HEVCERR_OUTPUT, "Unable to open input file: %s\n", cml->input);
    return -1;
  }
  else
  {
    fclose(tb.yuvFile);
    tb.yuvFile = NULL;
  }

  memset(&tb, 0, sizeof(struct test_bench));
  tb.argc = cml->argc;
  tb.argv = cml->argv;
  
  /* get GOP configuration */  
  tb.gopSize = MIN(cml->gopSize, MAX_GOP_SIZE);
  if (tb.gopSize==0 && cml->gopLowdelay)
  {
    tb.gopSize = 4;
  }
  memset (gopPicCfg, 0, sizeof(gopPicCfg));
  tb.encIn.gopConfig.pGopPicCfg = gopPicCfg;
  if (HEVCInitGopConfigs (tb.gopSize, cml, &(tb.encIn.gopConfig), tb.gopCfgOffset) != 0)
  {
    return -ret;
  }

  /* Encoder initialization */
  if ((ret = OpenEncoder(cml, &hevc_encoder, &tb)) != 0)
  {
    return -ret;
  }
  tb.input = cml->input;
  tb.output     = cml->output;
  tb.firstPic    = cml->firstPic;
  tb.lastPic   = cml->lastPic;
  tb.inputRateNumer      = cml->inputRateNumer;
  tb.inputRateDenom      = cml->inputRateDenom;
  tb.outputRateNumer      = cml->outputRateNumer;
  tb.outputRateDenom      = cml->outputRateDenom;
  tb.test_data_files    = cml->test_data_files;
  tb.width      = cml->width;
  tb.height     = cml->height;

  tb.idr_interval   = cml->intraPicRate;
  tb.byteStream   = cml->byteStream;
  tb.interlacedFrame = cml->interlacedFrame;

  /* Set the test ID for internal testing,
   * the SW must be compiled with testing flags */
  HEVCEncSetTestId(hevc_encoder, cml->testId);

  /* Allocate input and output buffers */
  if (AllocRes(cml, hevc_encoder, &tb) != 0)
  {
    FreeRes(hevc_encoder, &tb);
    CloseEncoder(hevc_encoder, &tb);
    return -HEVCENC_MEMORY_ERROR;
  }
#ifdef TEST_DATA
  Enc_test_data_init();
#endif


  ret = encode(&tb, hevc_encoder, cml);

#ifdef TEST_DATA
  Enc_test_data_release();
#endif

  FreeRes(hevc_encoder, &tb);

  CloseEncoder(hevc_encoder, &tb);

  return ret;
}

static u32 get_aligned_width(int width, i32 input_format)
{
#if 0
  if (input_format == HEVCENC_YUV420_PLANAR)
    return (width + 31) & (~31);
  else
    return (width + 15) & (~15);
#else
  if (input_format == HEVCENC_YUV420_PLANAR_10BIT_PACKED_PLANAR)
    return (width + 63) & (~63);//64 pixel =640 bits = 80 bytes= 5 x 128 bits.
  else if(input_format == HEVCENC_YUV420_10BIT_PACKED_Y0L2)
    return (width + 3) & (~3);
  else
    return (width + 15) & (~15);
#endif
}
static float getPixelWidthInByte(HEVCEncPictureType type)
{
  switch (type)
  {
    case HEVCENC_YUV420_PLANAR_10BIT_PACKED_PLANAR:
      return 1.25;
    case HEVCENC_YUV420_10BIT_PACKED_Y0L2:
      return 2;
    case HEVCENC_YUV420_PLANAR_10BIT_I010:
    case HEVCENC_YUV420_PLANAR_10BIT_P010:
      return 2;
    case HEVCENC_YUV420_PLANAR:
    case HEVCENC_YUV420_SEMIPLANAR:
    case HEVCENC_YUV420_SEMIPLANAR_VU:
    case HEVCENC_YUV422_INTERLEAVED_YUYV:
    case HEVCENC_YUV422_INTERLEAVED_UYVY:
    case HEVCENC_RGB565:
    case HEVCENC_BGR565:
    case HEVCENC_RGB555:
    case HEVCENC_BGR555:
    case HEVCENC_RGB444:
    case HEVCENC_BGR444:
    case HEVCENC_RGB888:
    case HEVCENC_BGR888:
    case HEVCENC_RGB101010:
    case HEVCENC_BGR101010:
      return 1;
    default:
      return 1;
  }
}

static HEVCEncPictureCodingType find_next_pic (HEVCEncIn *encIn, struct test_bench *tb,
                                                  commandLine_s *cml, i32 nextGopSize, u8 *gopCfgOffset)
{
  HEVCEncPictureCodingType nextCodingType;
  int idx, offset, cur_poc, delta_poc_to_next;
  int next_gop_size = nextGopSize;
  int picture_cnt_tmp = tb->picture_cnt;
  HEVCGopConfig *gopCfg = (HEVCGopConfig *)(&(encIn->gopConfig));

  //get current poc within GOP
  if (encIn->codingType == HEVCENC_INTRA_FRAME)
  {
    cur_poc = 0;
    encIn->gopPicIdx = 0;
  }
  else
  {
    idx = encIn->gopPicIdx + gopCfgOffset[encIn->gopSize];
    cur_poc = gopCfg->pGopPicCfg[idx].poc;
    encIn->gopPicIdx = (encIn->gopPicIdx + 1) % encIn->gopSize;
    if (encIn->gopPicIdx == 0)
      cur_poc -= encIn->gopSize;
  }

  //a GOP end, to start next GOP
  if (encIn->gopPicIdx == 0)
    offset = gopCfgOffset[next_gop_size];
  else
    offset = gopCfgOffset[encIn->gopSize];

  //get next poc within GOP, and delta_poc
  idx = encIn->gopPicIdx + offset;
  delta_poc_to_next = gopCfg->pGopPicCfg[idx].poc - cur_poc;
  tb->picture_cnt = picture_cnt_tmp + delta_poc_to_next;

  {
    //we just finished a GOP and need will jump to a P frame
    if (encIn->gopPicIdx == 0 && delta_poc_to_next > 1)
    {
       int gop_end_pic = tb->picture_cnt;
       int gop_shorten = 0, gop_shorten_idr = 0, gop_shorten_tail = 0;

       //handle IDR
       if ((tb->idr_interval) && ((gop_end_pic - tb->last_idr_picture_cnt) >= tb->idr_interval) && 
            !cml->gdrDuration)
          gop_shorten_idr = 1 + ((gop_end_pic - tb->last_idr_picture_cnt) - tb->idr_interval);

       //handle sequence tail
       while (((next_picture(tb, gop_end_pic --) + tb->firstPic) > tb->lastPic) &&
               (gop_shorten_tail < next_gop_size - 1))
         gop_shorten_tail ++;

       gop_shorten = gop_shorten_idr > gop_shorten_tail ? gop_shorten_idr : gop_shorten_tail;

       if (gop_shorten >= next_gop_size)
         tb->picture_cnt = picture_cnt_tmp + 1 - cur_poc;
       //reduce gop size
       else if (gop_shorten > 0)
       {
         const int max_reduced_gop_size = cml->gopLowdelay ? 1 : 4;
         next_gop_size -= gop_shorten;
         if (next_gop_size > max_reduced_gop_size)
           next_gop_size = max_reduced_gop_size;

         idx = gopCfgOffset[next_gop_size];
         delta_poc_to_next = gopCfg->pGopPicCfg[idx].poc - cur_poc;
         tb->picture_cnt = picture_cnt_tmp + delta_poc_to_next;
       }
       encIn->gopSize = next_gop_size;
    }

    encIn->poc += tb->picture_cnt - picture_cnt_tmp;
    //next coding type
    bool forceIntra = tb->idr_interval && ((tb->picture_cnt - tb->last_idr_picture_cnt) >= tb->idr_interval);
    if (forceIntra)
      nextCodingType = HEVCENC_INTRA_FRAME;
    else
    {
      idx = encIn->gopPicIdx + gopCfgOffset[encIn->gopSize];
      nextCodingType = gopCfg->pGopPicCfg[idx].codingType;
    }
  }
  gopCfg->id = encIn->gopPicIdx + gopCfgOffset[encIn->gopSize];
  return nextCodingType;
}
void writeQpDeltaData2Memory(char qpDelta,u8* memory,u16 column,u16 row,u16 blockunit,u16 width)
{
  u8 twoBlockDataCombined;
  int r,c;
  u32 ctb_row_number=row/(1<<blockunit);
  u32 ctb_row_stride=width/(1<<blockunit)*(8*8)/2;
  u32 ctb_row_offset=(row-(row/(1<<blockunit))*(1<<blockunit))*(1<<(3-blockunit));
  u32 internal_ctb_stride=8/2;
  u32 ctb_column_number;
  u8* rowMemoryStartPtr=memory+ctb_row_number*ctb_row_stride;
  u8* ctbMemoryStartPtr, *curMemoryStartPtr;
  u32 columns,rows;
  u32 xoffset;

  {
    ctb_column_number=column/(1<<blockunit);
    ctbMemoryStartPtr=rowMemoryStartPtr+ctb_column_number*8*8/2;
    switch(blockunit)
    {
      case 0:
        twoBlockDataCombined=(-qpDelta&0X0F)|(((-qpDelta&0X0F))<<4);
        rows=8;
        columns=4;
        xoffset=0;
        break;
      case 1:
        twoBlockDataCombined=(-qpDelta&0X0F)|(((-qpDelta&0X0F))<<4);
        rows=4;
        columns=2;
        xoffset=column%2;
        xoffset=xoffset<<1;
        break;
      case 2:
        twoBlockDataCombined=(-qpDelta&0X0F)|(((-qpDelta&0X0F))<<4);
        rows=2;
        columns=1;
        xoffset=column%4;
        break;
      case 3:
        xoffset=column>>1;
        xoffset=xoffset%4;
        curMemoryStartPtr=ctbMemoryStartPtr+ctb_row_offset*internal_ctb_stride + xoffset;
        twoBlockDataCombined=*curMemoryStartPtr;
        if(column%2)
        {
          twoBlockDataCombined=(twoBlockDataCombined&0x0f)|(((-qpDelta&0X0F))<<4);
        }
        else
        {
          twoBlockDataCombined=(twoBlockDataCombined&0xf0)|(-qpDelta&0X0F);
        }
        rows=1;
        columns=1;
        break;
      default:
        rows=0;
        twoBlockDataCombined=0;
        columns=0;
        xoffset=0;
        break;
    }
    for(r=0;r<rows;r++)
    {
      curMemoryStartPtr=ctbMemoryStartPtr+(ctb_row_offset+r)*internal_ctb_stride + xoffset;
      for(c=0;c<columns;c++)
      {
        *curMemoryStartPtr++=twoBlockDataCombined;
      }
    }
  }
}
void writeQpDeltaRowData2Memory(char* qpDeltaRowStartAddr,u8* memory,u16 width,u16 rowNumber,u16 blockunit)
{
  int i;
  i=0;
  while(i<width)
  {
    writeQpDeltaData2Memory(*qpDeltaRowStartAddr,memory,i,rowNumber,blockunit,width);
    qpDeltaRowStartAddr++;
    i++;
  }

}
int copyQPDelta2Memory(char *fname, u8* memory,u16 width,u16 height,u16 blockunit)
{
#define MAX_LINE_LENGTH_BLOCK 512*3
  char achParserBuffer[MAX_LINE_LENGTH_BLOCK];
  char rowbuffer[MAX_LINE_LENGTH_BLOCK];
  int line_idx = 0,i;
  int qpdelta,qpdelta_num=0;
  char* rowbufferptr;
  

  if(fname == NULL)
      return 0;

  FILE *fIn = fopen (fname, "r");
  if (fIn == NULL)
  {
    //printf("Qp delta config: Error, Can Not Open File %s\n", fname );
    return -1;
  }
  
  while (line_idx < height)
  {
    if (feof (fIn)) break;
    line_idx ++;
    achParserBuffer[0] = '\0';
    // Read one line
    char *line = fgets ((char *) achParserBuffer, MAX_LINE_LENGTH_BLOCK, fIn);
    if (!line) break;
    //handle line end
    char* s = strpbrk(line, "#\n");
    if(s) *s = '\0';
  
    if (line)
    {
      i=0;
      rowbufferptr=rowbuffer;
      while(i<width)
      {
        sscanf (line, "%d", &qpdelta);
        if(qpdelta>0||qpdelta<-15)
        {
          printf("Qp Delta out of range.\n");
          return -1;
        }
        *(rowbufferptr++)=(char)qpdelta;
        i++;
        qpdelta_num++;
        line= strchr(line, ',');
        if (line)
        {
          while (*line == ',') line ++;
          if (*line == '\0') break;
        }
        else
          break;
      }
      writeQpDeltaRowData2Memory(rowbuffer,memory,width,line_idx-1,blockunit);
    }
  }
  fclose(fIn);
  if (qpdelta_num != width*height)
  {
    printf ("QP delta Config: Error, Parsing File %s Failed\n", fname);
    return -1;
  }

  return 0;
}

/*------------------------------------------------------------------------------
  encode
------------------------------------------------------------------------------*/

i32 encode(struct test_bench *tb, HEVCEncInst encoder, commandLine_s *cml)
{
  HEVCEncIn *pEncIn = &(tb->encIn);
  HEVCEncOut encOut;
  HEVCEncRet ret;
  u32 frameCntTotal = 0;
  u32 streamSize = 0;
  u32 ctb_rows;
  ma_s ma;
  i32 cnt = 1;
  u32 i;
  i32 p;
  u32 total_bits = 0;
  u32 w = get_aligned_width(cml->lumWidthSrc, cml->inputFormat); //(cml->lumWidthSrc + 15) & (~15);
  u32 h = cml->lumHeightSrc;
  u8 *pUserData;
  u32 src_img_size;
  HEVCEncRateCtrl rc;
  u32 frameCntOutput = 0;
  u32 gopSize = tb->gopSize;
  i32 nextGopSize = 0;
  HEVCEncPictureCodingType nextCodingType = HEVCENC_INTRA_FRAME;
  bool adaptiveGop = (gopSize == 0); 
  //Adaptive Gop variables
  int gop_frm_num = 0;
  double sum_intra_vs_interskip = 0;
  double sum_skip_vs_interskip = 0;
  double sum_intra_vs_interskipP = 0;
  double sum_intra_vs_interskipB = 0;
  int sum_costP = 0;
  int sum_costB = 0;
  int last_gopsize;
  // u32 block_size;
  u32 block_unit;

  /* Output/input streams */
  if (!(tb->in = open_file(tb->input, "rb"))) goto error;
  if (!(tb->out = open_file(tb->output, "wb"))) goto error;
  ma.pos = ma.count = 0;
  ma.frameRateNumer = cml->outputRateNumer;
  ma.frameRateDenom = cml->outputRateDenom;
  if (cml->outputRateDenom)
      ma.length = MAX(1, MIN(cml->monitorFrames,
                              MOVING_AVERAGE_FRAMES));
  else
      ma.length = MOVING_AVERAGE_FRAMES;

  pEncIn->timeIncrement = 0;
  pEncIn->busOutBuf = tb->outbufMem.busAddress;
  pEncIn->outBufSize = tb->outbufMem.size;
  pEncIn->pOutBuf = tb->outbufMem.virtualAddress;
  SliceCtl_s ctl;
  ctl.multislice_encoding=(cml->sliceSize!=0&&(((cml->height+63)/64)>cml->sliceSize))?1:0;
  ctl.output_byte_stream=cml->byteStream?1:0;
  ctl.outStreamFile=tb->out;
  ctl.strmPtr = NULL;
  /* Video, sequence and picture parameter sets */
  for (p = 0; p < cnt; p++)
  {

    if (HEVCEncStrmStart(encoder, pEncIn, &encOut))
    {
      Error(2, ERR, "hevc_set_parameter() fails");
      goto error;
    }
    if (cml->byteStream == 0)
    {
      //WriteNalSizesToFile(nalfile, encOut.pNaluSizeBuf,
      //                    encOut.numNalus);
      WriteNals(tb->out, tb->outbufMem.virtualAddress, encOut.pNaluSizeBuf,
                encOut.numNalus, 0);
    }
    else
    {
      WriteStrm(tb->out, tb->outbufMem.virtualAddress, encOut.streamSize, 0);
    }
    total_bits += encOut.streamSize * 8;
    streamSize += encOut.streamSize;
  }

  pEncIn->poc = 0;

  //default gop size as IPPP
  pEncIn->gopSize =  nextGopSize = (adaptiveGop ? 1 : gopSize);
  HEVCEncGetRateCtrl(encoder, &rc);


  /* Allocate a buffer for user data and read data from file */
  pUserData = ReadUserData(encoder, cml->userData);
  /* Setup encoder input */
  {

    u32 size_lum = w * h;
    u32 size_ch  = ((w + 1) >> 1) * ((h + 1) >> 1);
    pEncIn->busLuma = tb->pictureMem.busAddress;
    tb->lum = (u8 *)tb->pictureMem.virtualAddress;

    pEncIn->busChromaU = pEncIn->busLuma + (u32)((float)size_lum*getPixelWidthInByte(cml->inputFormat));
    tb->cb = tb->lum + (u32)((float)size_lum*getPixelWidthInByte(cml->inputFormat));
    pEncIn->busChromaV = pEncIn->busChromaU + (u32)((float)size_ch*getPixelWidthInByte(cml->inputFormat));
    tb->cr = tb->cb + (u32)((float)size_ch*getPixelWidthInByte(cml->inputFormat));
  }

  src_img_size = cml->lumWidthSrc * cml->lumHeightSrc *
                 HEVCEncGetBitsPerPixel(cml->inputFormat) / 8;
  printf("Reading input from file <%s>,\n frame size %d bytes.\n",
         cml->input, src_img_size);
  tb->validencodedframenumber=0;
  ctb_rows=(cml->height+63)/64;
  last_gopsize = MAX_ADAPTIVE_GOP_SIZE;
  gop_frm_num = 0;
  sum_intra_vs_interskip = 0;
  sum_skip_vs_interskip = 0;
  sum_intra_vs_interskipP = 0;
  sum_intra_vs_interskipB = 0;
  sum_costP = 0;
  sum_costB = 0;
  while (true)
  {
    if (read_picture(tb, cml->inputFormat, src_img_size, cml->lumWidthSrc, cml->lumHeightSrc))
    {
      /* Next input stream (if any) */
      if (change_input(tb)) break;
      fclose(tb->in);
      if (!(tb->in = open_file(tb->input, "rb"))) goto error;
      tb->picture_cnt = 0;
      tb->picture_enc_cnt = 0;
      tb->validencodedframenumber=0;
      continue;
    }
    frameCntTotal++;
  

    pEncIn->codingType = (pEncIn->poc == 0) ? HEVCENC_INTRA_FRAME : nextCodingType;
    printf("=== Encoding %i %s codeType=%d ...\n", tb->picture_enc_cnt, tb->input, pEncIn->codingType);
    //select rps based on frame type
    if (pEncIn->codingType == HEVCENC_INTRA_FRAME&&cml->gdrDuration==0)
    {
      //refrest IDR poc

      pEncIn->poc = 0;
      frameCntOutput = 0;
      tb->last_idr_picture_cnt = tb->picture_cnt;
    }
    else if(pEncIn->codingType == HEVCENC_INTRA_FRAME)
    {
      //frameCntOutput = 0;
      tb->last_idr_picture_cnt = tb->picture_cnt;
    }

    for (i = 0; i < MAX_BPS_ADJUST; i++)
      if (cml->bpsAdjustFrame[i] &&
          (tb->picture_cnt == cml->bpsAdjustFrame[i]))
      {
        rc.bitPerSecond = cml->bpsAdjustBitrate[i];
        printf("Adjusting bitrate target: %d\n", rc.bitPerSecond);
        if ((ret = HEVCEncSetRateCtrl(encoder, &rc)) != HEVCENC_OK)
        {
          //PrintErrorValue("HEVCEncSetRateCtrl() failed.", ret);
        }
      }
    pEncIn->roiMapDeltaQpAddr=tb->roiMapDeltaQpMem.busAddress;

    //copy config data to memory.
     //allocate delta qp map memory.
    switch(cml->roiMapDeltaQpBlockUnit)
    {
      case 0:
        block_unit=64;
        break;
      case 1:
        block_unit=32;
        break;
      case 2:
        block_unit=16;
        break;
      case 3:
        block_unit=8;
        break;
      default:
        block_unit=64;
        break;
    }
  // 4 bits per block.
    copyQPDelta2Memory(cml->roiMapDeltaQpFile,(u8*)tb->roiMapDeltaQpMem.virtualAddress,((cml->width+64-1)& (~(64 - 1)))/block_unit,((cml->height+64-1)& (~(64 - 1)))/block_unit,cml->roiMapDeltaQpBlockUnit);
    ret = HEVCEncStrmEncode(encoder, pEncIn, &encOut, &HEVCSliceReady, &ctl);
    switch (ret)
    {
      case HEVCENC_FRAME_READY:
        tb->picture_enc_cnt++;
        if (encOut.streamSize == 0)
        {
          tb->picture_cnt ++;
          break;
        }

#ifdef PSNR
        extern void EWLTraceProfile();
        EWLTraceProfile();
#endif
#ifdef TEST_DATA
        //show profile data, only for c-model;
        //write_recon_sw(encoder, encIn.poc);
        EncTraceUpdateStatus();
        if(cml->outReconFrame==1)
          while (EncTraceRecon(encoder, frameCntOutput/*encIn.poc*/, cml->recon))
          {
            frameCntOutput ++;
          }
#endif

        HEVCEncGetRateCtrl(encoder, &rc);
        /* Write scaled encoder picture to file, packed yuyv 4:2:2 format */
        WriteScaled((u32 *)encOut.scaledPicture, encoder);
        if (encOut.streamSize != 0)
        {
          if((ctl.multislice_encoding==0)||(ENCH2_SLICE_READY_INTERRUPT==0)||(cml->hrdConformance==1))
          {
            if (cml->byteStream == 0)
            {
              //WriteNalSizesToFile(nalfile, encOut.pNaluSizeBuf,
              //            encOut.numNalus);
              WriteNals(tb->out, tb->outbufMem.virtualAddress, encOut.pNaluSizeBuf,
                        encOut.numNalus, 0);
            }
            else
            {
              WriteStrm(tb->out, tb->outbufMem.virtualAddress, encOut.streamSize, 0);
            }
          }
          pEncIn->timeIncrement = tb->outputRateDenom;

          total_bits += encOut.streamSize * 8;
          streamSize += encOut.streamSize;
          tb->validencodedframenumber++;
          MaAddFrame(&ma, encOut.streamSize*8);
          if(cml->picRc&&tb->validencodedframenumber>=ma.length)
          {
            tb->numbersquareoferror++;
            if(tb->maxerrorovertarget<(Ma(&ma)- cml->bitPerSecond))
              tb->maxerrorovertarget=(Ma(&ma)- cml->bitPerSecond);
            if(tb->maxerrorundertarget<(cml->bitPerSecond-Ma(&ma)))
              tb->maxerrorundertarget=(cml->bitPerSecond-Ma(&ma));
            tb->sumsquareoferror+=((float)(ABS(Ma(&ma)- cml->bitPerSecond))*100/cml->bitPerSecond);
            tb->averagesquareoferror=(tb->sumsquareoferror/tb->numbersquareoferror);
            printf("moving bit rate=%d,max over target=%% %d,max under target=%% %d,average deviation per frame=%% %f\n",Ma(&ma),tb->maxerrorovertarget*100/cml->bitPerSecond,tb->maxerrorundertarget*100/cml->bitPerSecond,tb->averagesquareoferror);
          }
          printf("Encoded %d, bits=%d,average bit rate=%lld\n", tb->picture_enc_cnt-1, total_bits, ((unsigned long long)total_bits * tb->outputRateNumer) / (tb->picture_enc_cnt * tb->outputRateDenom));

          //Adaptive GOP size decision
          if (adaptiveGop && pEncIn->codingType != HEVCENC_INTRA_FRAME)
          {
          struct hevc_instance *hevc_instance = (struct hevc_instance *)encoder;
          unsigned int uiIntraCu8Num = hevc_instance->asic.regs.intraCu8Num;
          unsigned int uiSkipCu8Num = hevc_instance->asic.regs.skipCu8Num;
          unsigned int uiPBFrameCost = hevc_instance->asic.regs.PBFrame4NRdCost;
          double dIntraVsInterskip = (double)uiIntraCu8Num/(double)((tb->width/8) * (tb->height/8));
          double dSkipVsInterskip = (double)uiSkipCu8Num/(double)((tb->width/8) * (tb->height/8));

          //printf("GF:::intraCu8Num = %d, skipCu8Num = %d\n", uiIntraCu8Num, uiSkipCu8Num);
          //printf("GF:::dIntraVsInterskip = %f,frmType=%d\n", dIntraVsInterskip, encIn.codingType );
          //printf("GF:::dSkipVsInterskip = %f,frmType=%d\n",dSkipVsInterskip , encIn.codingType );
          //printf("GF:::cost4N = %d, frmType=%d\n", uiPBFrameCost, encIn.codingType );
          
          gop_frm_num++;
          sum_intra_vs_interskip += dIntraVsInterskip;
          sum_skip_vs_interskip += dSkipVsInterskip;
          sum_costP += (pEncIn->codingType == HEVCENC_PREDICTED_FRAME)? uiPBFrameCost:0;
          sum_costB += (pEncIn->codingType == HEVCENC_BIDIR_PREDICTED_FRAME)? uiPBFrameCost:0;
          sum_intra_vs_interskipP += (pEncIn->codingType == HEVCENC_PREDICTED_FRAME)? dIntraVsInterskip:0;
          sum_intra_vs_interskipB += (pEncIn->codingType == HEVCENC_BIDIR_PREDICTED_FRAME)? dIntraVsInterskip:0; 
          if(pEncIn->gopPicIdx == pEncIn->gopSize-1)//last frame of the current gop. decide the gopsize of next gop.
          {
            dIntraVsInterskip = sum_intra_vs_interskip/gop_frm_num;
            dSkipVsInterskip = sum_skip_vs_interskip/gop_frm_num;
            sum_costB = (gop_frm_num>1)?(sum_costB/(gop_frm_num-1)):0xFFFFFFF;
            sum_intra_vs_interskipB = (gop_frm_num>1)?(sum_intra_vs_interskipB/(gop_frm_num-1)):0xFFFFFFF;
            //Enabled adaptive GOP size for large resolution
            if (((tb->width * tb->height) >= (1280 * 720)) || ((MAX_ADAPTIVE_GOP_SIZE >3)&&((tb->width * tb->height) >= (416 * 240))))
            {
                if ((((double)sum_costP/(double)sum_costB)<1.1)&&(dSkipVsInterskip >= 0.95))
                {
                    last_gopsize = nextGopSize = 1;
                }
                else if (((double)sum_costP/(double)sum_costB)>5)
                {
                    nextGopSize = last_gopsize;
                }
                else
                {
                    if( ((sum_intra_vs_interskipP > 0.40) && (sum_intra_vs_interskipP < 0.70)&& (sum_intra_vs_interskipB < 0.10)) )
                    {
                        last_gopsize++;
                        if(last_gopsize==5 || last_gopsize==7)  
                        {
                            last_gopsize++;
                        }
                        last_gopsize = MIN(last_gopsize, MAX_ADAPTIVE_GOP_SIZE);
                        nextGopSize = last_gopsize; //
                    }
                    else if (dIntraVsInterskip >= 0.30)
                    {
                        last_gopsize = nextGopSize = 1; //No B
                    }
                    else if (dIntraVsInterskip >= 0.20)
                    {
                        last_gopsize = nextGopSize = 2; //One B
                    }
                    else if (dIntraVsInterskip >= 0.10)
                    {
                        last_gopsize--;
                        if(last_gopsize == 5 || last_gopsize==7) 
                        {
                            last_gopsize--;
                        }
                        last_gopsize = MAX(last_gopsize, 3);
                        nextGopSize = last_gopsize; //
                    }
                    else
                    {
                        last_gopsize++;
                        if(last_gopsize==5 || last_gopsize==7)  
                        {
                            last_gopsize++;
                        }
                        last_gopsize = MIN(last_gopsize, MAX_ADAPTIVE_GOP_SIZE);
                        nextGopSize = last_gopsize; //
                    }
                }
            }
            else
            {
              nextGopSize = 3;
            }
            gop_frm_num = 0;
            sum_intra_vs_interskip = 0;
            sum_skip_vs_interskip = 0;
            sum_costP = 0;
            sum_costB = 0;
            sum_intra_vs_interskipP = 0;
            sum_intra_vs_interskipB = 0;

            nextGopSize = MIN(nextGopSize, MAX_ADAPTIVE_GOP_SIZE);
            }
          }
          nextCodingType = find_next_pic (pEncIn, tb, cml, nextGopSize, tb->gopCfgOffset);
        }

        if (pUserData)
        {
          /* We want the user data to be written only once so
           * we disable the user data and free the memory after
           * first frame has been encoded. */
          HEVCEncSetSeiUserData(encoder, NULL, 0);
          free(pUserData);
          pUserData = NULL;
        }
        printf("current picture hardware performance=%d cycles\n",HEVCGetPerformance(encoder));
        break;
      case HEVCENC_OUTPUT_BUFFER_OVERFLOW:
        tb->picture_cnt ++;
        break;
      default:
        goto error;
        break;
    }
    if(cml->profile==HEVCENC_MAIN_STILL_PICTURE_PROFILE)
      break;
  }
  /* End stream */
#ifdef TEST_DATA  
  EncTraceReconEnd();
#endif
  ret = HEVCEncStrmEnd(encoder, pEncIn, &encOut);
  if (ret == HEVCENC_OK)
  {
    if (cml->byteStream == 0)
    {
      //WriteNalSizesToFile(nalfile, encOut.pNaluSizeBuf,encOut.numNalus);
      WriteNals(tb->out, tb->outbufMem.virtualAddress, encOut.pNaluSizeBuf,
                encOut.numNalus, 0);
    }
    else
    {
      WriteStrm(tb->out, tb->outbufMem.virtualAddress, encOut.streamSize, 0);
    }
    streamSize += encOut.streamSize;
  }

  printf("Total of %d frames processed, %d frames encoded, %d bytes.\n",
         frameCntTotal, tb->picture_enc_cnt, streamSize);

  if (tb->in) fclose(tb->in);
  if (tb->out) fclose(tb->out);

  return OK;

error:
  Error(2, ERR, "encode() fails");
  if (tb->in) fclose(tb->in);
  if (tb->out) fclose(tb->out);

  return NOK;
}

/*------------------------------------------------------------------------------

    CheckArea

------------------------------------------------------------------------------*/
i32 CheckArea(HEVCEncPictureArea *area, commandLine_s *cml)
{
  i32 w = (cml->width + cml->max_cu_size - 1) / cml->max_cu_size;
  i32 h = (cml->height + cml->max_cu_size - 1) / cml->max_cu_size;

  if ((area->left < (u32)w) && (area->right < (u32)w) &&
      (area->top < (u32)h) && (area->bottom < (u32)h)) return 1;

  return 0;
}

/*------------------------------------------------------------------------------

    OpenEncoder
        Create and configure an encoder instance.

    Params:
        cml     - processed comand line options
        pEnc    - place where to save the new encoder instance
    Return:
        0   - for success
        -1  - error

------------------------------------------------------------------------------*/
int OpenEncoder(commandLine_s *cml, HEVCEncInst *pEnc, struct test_bench *tb)
{
  HEVCEncRet ret;
  HEVCEncConfig cfg;
  HEVCEncCodingCtrl codingCfg;
  HEVCEncRateCtrl rcCfg;
  HEVCEncPreProcessingCfg preProcCfg;
  HEVCEncInst encoder;

  /* Default resolution, try parsing input file name */
  if (cml->lumWidthSrc == DEFAULT || cml->lumHeightSrc == DEFAULT)
  {
    if (GetResolution(cml->input, &cml->lumWidthSrc, &cml->lumHeightSrc))
    {
      /* No dimensions found in filename, using default QCIF */
      cml->lumWidthSrc = 176;
      cml->lumHeightSrc = 144;
    }
  }

  /* Encoder initialization */
  if (cml->width == DEFAULT)
    cml->width = cml->lumWidthSrc;

  if (cml->height == DEFAULT)
    cml->height = cml->lumHeightSrc;

  /* outputRateNumer */
  if (cml->outputRateNumer == DEFAULT)
  {
    cml->outputRateNumer = cml->inputRateNumer;
  }

  /* outputRateDenom */
  if (cml->outputRateDenom == DEFAULT)
  {
    cml->outputRateDenom = cml->inputRateDenom;
  }
  /*cfg.ctb_size = cml->max_cu_size;*/
  if (cml->rotation&&cml->rotation!=3)
  {
    cfg.width = cml->height;
    cfg.height = cml->width;
  }
  else
  {
    cfg.width = cml->width;
    cfg.height = cml->height;
  }


  cfg.frameRateDenom = cml->outputRateDenom;
  cfg.frameRateNum = cml->outputRateNumer;

  /* intra tools in sps and pps */
  cfg.strongIntraSmoothing = cml->strong_intra_smoothing_enabled_flag;

  if (cml->byteStream)
  {
    cfg.streamType = HEVCENC_BYTE_STREAM;
  }
  else
  {
    cfg.streamType = HEVCENC_NAL_UNIT_STREAM;
  }

  cfg.level = HEVCENC_LEVEL_6;

  if (cml->level != DEFAULT && cml->level != 0)
    cfg.level = (HEVCEncLevel)cml->level;
    
  cfg.profile = HEVCENC_MAIN_PROFILE;

  if (cml->profile != DEFAULT && cml->profile != 0)
    cfg.profile = (HEVCEncProfile)cml->profile;    

  cfg.bitDepthLuma = 8;

  if (cml->bitDepthLuma != DEFAULT && cml->bitDepthLuma != 8)
    cfg.bitDepthLuma = cml->bitDepthLuma;    

  cfg.bitDepthChroma= 8;

  if (cml->bitDepthChroma != DEFAULT && cml->bitDepthChroma != 8)
    cfg.bitDepthChroma = cml->bitDepthChroma;    
  if (cml->interlacedFrame && cml->gopSize != 1)
  {
    printf("OpenEncoder: treat interlace to progressive for gopSize!=1 case\n");
    cml->interlacedFrame = 0;
  }

  //default maxTLayer
  cfg.maxTLayers = 1;

  /* Find the max number of reference frame */
  if (cml->intraPicRate == 1)
  {
    cfg.refFrameAmount = 0;
  }
  else
  {
    u32 maxRefPics = 0;
    u32 maxTemporalId = 0;
    int idx;
    for (idx = 0; idx < tb->encIn.gopConfig.size; idx ++)
    {
      HEVCGopPicConfig *cfg = &(tb->encIn.gopConfig.pGopPicCfg[idx]);
      if (cfg->codingType != HEVCENC_INTRA_FRAME)
      {
        if (maxRefPics < cfg->numRefPics)
          maxRefPics = cfg->numRefPics;

        if (maxTemporalId < cfg->temporalId)
          maxTemporalId = cfg->temporalId;
      }
    }
    cfg.refFrameAmount = maxRefPics + cml->interlacedFrame;
    cfg.maxTLayers = maxTemporalId +1;
  }
  cfg.compressor = cml->compressor;
  cfg.interlacedFrame = cml->interlacedFrame;
  if ((ret = HEVCEncInit(&cfg, pEnc)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncInit() failed.", ret);
    return (int)ret;
  }

  encoder = *pEnc;

#if 1
  /* Encoder setup: coding control */
  if ((ret = HEVCEncGetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncGetCodingCtrl() failed.", ret);
    CloseEncoder(encoder, tb);
    return -1;
  }
  else
  {

    if (cml->sliceSize != DEFAULT)
    {
      codingCfg.sliceSize = cml->sliceSize;
    }
    if (cml->disableDeblocking != 0)
      codingCfg.disableDeblockingFilter = 1;
    else
      codingCfg.disableDeblockingFilter = 0;


    codingCfg.tc_Offset = cml->tc_Offset;
    codingCfg.beta_Offset = cml->beta_Offset;
    codingCfg.enableSao = cml->enableSao;
    codingCfg.enableDeblockOverride = cml->enableDeblockOverride;
    codingCfg.deblockOverride = cml->deblockOverride;
    if (cml->cabacInitFlag != DEFAULT)
      codingCfg.cabacInitFlag = cml->cabacInitFlag;
    codingCfg.videoFullRange = 0;
    if (cml->videoRange != DEFAULT)
    {
      codingCfg.videoFullRange = cml->videoRange;
    }
    
    if (cml->sei)
      codingCfg.seiMessages = 1;
    else
      codingCfg.seiMessages = 0;

    codingCfg.gdrDuration = cml->gdrDuration;
    codingCfg.fieldOrder = cml->fieldOrder;

    codingCfg.cirStart = cml->cirStart;
    codingCfg.cirInterval = cml->cirInterval;
    
    if(codingCfg.gdrDuration == 0)
    {
      codingCfg.intraArea.top = cml->intraAreaTop;
      codingCfg.intraArea.left = cml->intraAreaLeft;
      codingCfg.intraArea.bottom = cml->intraAreaBottom;
      codingCfg.intraArea.right = cml->intraAreaRight;
      codingCfg.intraArea.enable = CheckArea(&codingCfg.intraArea, cml);
    }
    else
    {
      //intraArea will be used by GDR, customer can not use intraArea when GDR is enabled. 
      codingCfg.intraArea.enable = 0;
    }
    
    if(codingCfg.gdrDuration == 0)
    {
      codingCfg.roi1Area.top = cml->roi1AreaTop;
      codingCfg.roi1Area.left = cml->roi1AreaLeft;
      codingCfg.roi1Area.bottom = cml->roi1AreaBottom;
      codingCfg.roi1Area.right = cml->roi1AreaRight;
      if (CheckArea(&codingCfg.roi1Area, cml) && cml->roi1DeltaQp)
        codingCfg.roi1Area.enable = 1;
      else
        codingCfg.roi1Area.enable = 0;
    }
    else
    {
      codingCfg.roi1Area.enable = 0;
    }
    
    codingCfg.roi2Area.top = cml->roi2AreaTop;
    codingCfg.roi2Area.left = cml->roi2AreaLeft;
    codingCfg.roi2Area.bottom = cml->roi2AreaBottom;
    codingCfg.roi2Area.right = cml->roi2AreaRight;
    if (CheckArea(&codingCfg.roi2Area, cml) && cml->roi2DeltaQp)
      codingCfg.roi2Area.enable = 1;
    else
      codingCfg.roi2Area.enable = 0;
    codingCfg.roi1DeltaQp = cml->roi1DeltaQp;
    codingCfg.roi2DeltaQp = cml->roi2DeltaQp;
    
    if (codingCfg.cirInterval)
      printf("  CIR: %d %d\n",
             codingCfg.cirStart, codingCfg.cirInterval);

    if (codingCfg.intraArea.enable)
      printf("  IntraArea: %dx%d-%dx%d\n",
             codingCfg.intraArea.left, codingCfg.intraArea.top,
             codingCfg.intraArea.right, codingCfg.intraArea.bottom);

    if (codingCfg.roi1Area.enable)
      printf("  ROI 1: %d  %dx%d-%dx%d\n", codingCfg.roi1DeltaQp,
             codingCfg.roi1Area.left, codingCfg.roi1Area.top,
             codingCfg.roi1Area.right, codingCfg.roi1Area.bottom);

    if (codingCfg.roi2Area.enable)
      printf("  ROI 2: %d  %dx%d-%dx%d\n", codingCfg.roi2DeltaQp,
             codingCfg.roi2Area.left, codingCfg.roi2Area.top,
             codingCfg.roi2Area.right, codingCfg.roi2Area.bottom);
             
    codingCfg.roiMapDeltaQpEnable = cml->roiMapDeltaQpEnable;
    codingCfg.roiMapDeltaQpBlockUnit=cml->roiMapDeltaQpBlockUnit;

    codingCfg.enableScalingList = cml->enableScalingList;
    codingCfg.chroma_qp_offset= cml->chromaQpOffset;

    codingCfg.noiseReductionEnable = cml->noiseReductionEnable;//0: disable noise reduction; 1: enable noise reduction
    if(cml->noiseLow == 0)
    {
        codingCfg.noiseLow = 10;        
    }
    else
    {
        codingCfg.noiseLow = CLIP3(1, 30, cml->noiseLow);//0: use default value; valid value range: [1, 30]
    }
    
    if(cml->firstFrameSigma == 0)
    {
        codingCfg.firstFrameSigma = 11;
    }
    else
    {
        codingCfg.firstFrameSigma = CLIP3(1, 30, cml->firstFrameSigma);
    }
    //printf("\n\n\n\n");
    //printf("nr_enable = %d, nL = %d, firstSig = %d \n", codingCfg.noiseReductionEnable, codingCfg.noiseLow, codingCfg.firstFrameSigma);

    if ((ret = HEVCEncSetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
    {
      //PrintErrorValue("HEVCEncSetCodingCtrl() failed.", ret);
      CloseEncoder(encoder, tb);
      return -1;
    }

  }
#endif


  /* Encoder setup: rate control */
  if ((ret = HEVCEncGetRateCtrl(encoder, &rcCfg)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncGetRateCtrl() failed.", ret);
    CloseEncoder(encoder, tb);
    return -1;
  }
  else
  {
    printf("Get rate control: qp %2d [%2d, %2d]  %8d bps  "
           "pic %d skip %d  hrd %d\n  cpbSize %d gopLen %d "
           "intraQpDelta %2d\n",
           rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
           rcCfg.pictureRc, rcCfg.pictureSkip, rcCfg.hrd,
           rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta);

    if (cml->qpHdr != DEFAULT)
      rcCfg.qpHdr = cml->qpHdr;
    else
      rcCfg.qpHdr = -1;
    if (cml->qpMin != DEFAULT)
      rcCfg.qpMin = cml->qpMin;
    if (cml->qpMax != DEFAULT)
      rcCfg.qpMax = cml->qpMax;
    if (cml->picSkip != DEFAULT)
      rcCfg.pictureSkip = cml->picSkip;
    if (cml->picRc != DEFAULT)
      rcCfg.pictureRc = cml->picRc;
	//CTB_RC
    if (cml->ctbRc != DEFAULT)
      rcCfg.ctbRc = cml->ctbRc;

    rcCfg.blockRCSize = 0;

    if (cml->blockRCSize != DEFAULT)
      rcCfg.blockRCSize = cml->blockRCSize;
    
    if (cml->bitPerSecond != DEFAULT)
      rcCfg.bitPerSecond = cml->bitPerSecond;
    if (cml->bitVarRangeI != DEFAULT)
      rcCfg.bitVarRangeI = cml->bitVarRangeI;
    if (cml->bitVarRangeP != DEFAULT)
      rcCfg.bitVarRangeP = cml->bitVarRangeP;
    if (cml->bitVarRangeB != DEFAULT)
      rcCfg.bitVarRangeB = cml->bitVarRangeB;

    if (cml->tolMovingBitRate != DEFAULT)
      rcCfg.tolMovingBitRate = cml->tolMovingBitRate;
    
    if (cml->monitorFrames != DEFAULT)
      rcCfg.monitorFrames = cml->monitorFrames;
    else 
    {
      rcCfg.monitorFrames =(cml->outputRateNumer+cml->outputRateDenom-1) / cml->outputRateDenom;
      cml->monitorFrames = (cml->outputRateNumer+cml->outputRateDenom-1) / cml->outputRateDenom;
    }
      
    if(rcCfg.monitorFrames>MOVING_AVERAGE_FRAMES)
      rcCfg.monitorFrames=MOVING_AVERAGE_FRAMES;

    if(rcCfg.monitorFrames<10)
      rcCfg.monitorFrames=10;

    if (cml->hrdConformance != DEFAULT)
      rcCfg.hrd = cml->hrdConformance;

    if (cml->cpbSize != DEFAULT)
      rcCfg.hrdCpbSize = cml->cpbSize;

    if (cml->intraPicRate != 0)
      rcCfg.gopLen = MIN(cml->intraPicRate, MAX_GOP_LEN);

    if (cml->gopLength != DEFAULT)
      rcCfg.gopLen = cml->gopLength;

    if (cml->intraQpDelta != DEFAULT)
      rcCfg.intraQpDelta = cml->intraQpDelta;

    rcCfg.fixedIntraQp = cml->fixedIntraQp;

    printf("Set rate control: qp %2d [%2d, %2d] %8d bps  "
           "pic %d skip %d  hrd %d\n"
           "  cpbSize %d gopLen %d intraQpDelta %2d "
           "fixedIntraQp %2d\n",
           rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
           rcCfg.pictureRc, rcCfg.pictureSkip, rcCfg.hrd,
           rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta,
           rcCfg.fixedIntraQp);

    if ((ret = HEVCEncSetRateCtrl(encoder, &rcCfg)) != HEVCENC_OK)
    {
      //PrintErrorValue("HEVCEncSetRateCtrl() failed.", ret);
      CloseEncoder(encoder, tb);
      return -1;
    }
  }
#ifndef CTBRC_STRENGTH

  /* check the conflict setting for ctbRC and ROI */
  if ((rcCfg.ctbRc!=0) && ((codingCfg.roiMapDeltaQpEnable)
                           ||codingCfg.roi1Area.enable||codingCfg.roi2Area.enable) )
  {
      //PrintErrorValue("HEVCEnc ROI setting conflict with ctbR.", ret);
      //CloseEncoder(encoder);
      //return -1;
      fprintf(stderr, "Warning: ctbRC will be disable for ROI is enable.\n");
  }
#endif  
#if 1
#if 1
  /* Optional scaled image output */
  if (cml->scaledWidth * cml->scaledHeight)
  {
    if (EWLMallocRefFrm(((struct hevc_instance *)encoder)->asic.ewl, cml->scaledWidth*(1<<((cml->bitDepthLuma!=8 || (cml->bitDepthChroma!=8))? 1:0)) * cml->scaledHeight * 2,
                        &tb->scaledPictureMem) != EWL_OK)
    {
      tb->scaledPictureMem.virtualAddress = NULL;
      tb->scaledPictureMem.busAddress = 0;

    }
  }
#endif

  /* PreP setup */
  if ((ret = HEVCEncGetPreProcessing(encoder, &preProcCfg)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncGetPreProcessing() failed.", ret);
    CloseEncoder(encoder, tb);
    return -1;
  }
  printf
  ("Get PreP: input %4dx%d : offset %4dx%d : format %d : rotation %d\n"
   "cc %d : scaling %d\n",
   preProcCfg.origWidth, preProcCfg.origHeight, preProcCfg.xOffset,
   preProcCfg.yOffset, preProcCfg.inputType, preProcCfg.rotation,
   preProcCfg.colorConversion.type,
   preProcCfg.scaledOutput);

  preProcCfg.inputType = (HEVCEncPictureType)cml->inputFormat;
  preProcCfg.rotation = (HEVCEncPictureRotation)cml->rotation;

  preProcCfg.origWidth = cml->lumWidthSrc;
  preProcCfg.origHeight = cml->lumHeightSrc;
  if (cml->interlacedFrame) preProcCfg.origHeight /= 2;

  if (cml->horOffsetSrc != DEFAULT)
    preProcCfg.xOffset = cml->horOffsetSrc;
  if (cml->verOffsetSrc != DEFAULT)
    preProcCfg.yOffset = cml->verOffsetSrc;
  if (cml->colorConversion != DEFAULT)
    preProcCfg.colorConversion.type =
      (HEVCEncColorConversionType)cml->colorConversion;
  if (preProcCfg.colorConversion.type == HEVCENC_RGBTOYUV_USER_DEFINED)
  {
    preProcCfg.colorConversion.coeffA = 20000;
    preProcCfg.colorConversion.coeffB = 44000;
    preProcCfg.colorConversion.coeffC = 5000;
    preProcCfg.colorConversion.coeffE = 35000;
    preProcCfg.colorConversion.coeffF = 38000;
  }

  if (cml->rotation&&cml->rotation!=3)
  {
    preProcCfg.scaledWidth = cml->scaledHeight;
    preProcCfg.scaledHeight = cml->scaledWidth;
  }
  else
  {
    preProcCfg.scaledWidth = cml->scaledWidth;
    preProcCfg.scaledHeight = cml->scaledHeight;
  }
  preProcCfg.busAddressScaledBuff = tb->scaledPictureMem.busAddress;
  preProcCfg.virtualAddressScaledBuff = tb->scaledPictureMem.virtualAddress;
  preProcCfg.sizeScaledBuff = tb->scaledPictureMem.size;

  printf
  ("Set PreP: input %4dx%d : offset %4dx%d : format %d : rotation %d\n"
   "cc %d : scaling %d\n",
   preProcCfg.origWidth, preProcCfg.origHeight, preProcCfg.xOffset,
   preProcCfg.yOffset, preProcCfg.inputType, preProcCfg.rotation,
   preProcCfg.colorConversion.type,
   preProcCfg.scaledOutput);

  if(cml->scaledWidth*cml->scaledHeight)
    preProcCfg.scaledOutput=1;

  if ((ret = HEVCEncSetPreProcessing(encoder, &preProcCfg)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncSetPreProcessing() failed.", ret);
    CloseEncoder(encoder, tb);
    return (int)ret;
  }
#endif
  return 0;
}
/*------------------------------------------------------------------------------

    CloseEncoder
       Release an encoder insatnce.

   Params:
        encoder - the instance to be released
------------------------------------------------------------------------------*/
void CloseEncoder(HEVCEncInst encoder, struct test_bench *tb)
{
  HEVCEncRet ret;

  if (tb->scaledPictureMem.virtualAddress != NULL)
    EWLFreeLinear(((struct hevc_instance *)encoder)->asic.ewl, &tb->scaledPictureMem);
  if ((ret = HEVCEncRelease(encoder)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncRelease() failed.", ret);
  }
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
int ParseDelim(char *optArg, char delim)
{
  i32 i;

  for (i = 0; i < (i32)strlen(optArg); i++)
    if (optArg[i] == delim)
    {
      optArg[i] = 0;
      return i;
    }

  return -1;
}

int HasDelim(char *optArg, char delim)
{
  i32 i;

  for (i = 0; i < (i32)strlen(optArg); i++)
    if (optArg[i] == delim)
    {
      return 1;
    }
  
  return 0;
}

/*------------------------------------------------------------------------------
  parameter
------------------------------------------------------------------------------*/
i32 Parameter_Get(i32 argc, char **argv, commandLine_s *cml) {
  struct parameter prm;
  i32 status = OK;
  i32 ret, i;
  char *p;
  i32 bpsAdjustCount = 0;

  prm.cnt = 1;
  while ((ret = get_option(argc, argv, options, &prm)) != -1)
  {
    if (ret == -2) status = NOK;
    p = prm.argument;
    switch (prm.short_opt)
    {
      case 'H':
        help();
        break;
      case 'i':
        cml->input = p;
        break;
      case 'o':
        cml->output = p;
        break;
      case 'a':
        cml->firstPic = atoi(p);
        break;
      case 'b':
        cml->lastPic = atoi(p);
        break;
      case 'x':
        cml->width = atoi(p);
        break;
      case 'y':
        cml->height = atoi(p);
        break;
      case 'z':
        cml->userData = p;
        break;
      case 'w':
        cml->lumWidthSrc = atoi(p);
        break;
      case 'h':
        cml->lumHeightSrc = atoi(p);
        break;
      case 'X':
        cml->horOffsetSrc = atoi(p);
        break;
      case 'Y':
        cml->verOffsetSrc = atoi(p);
        break;
      case 'l':
        cml->inputFormat = atoi(p);
        break;
      case 'O':
        cml->colorConversion = atoi(p);
        break;
      case 'f':
        cml->outputRateNumer = atoi(p);
        break;
      case 'F':
        cml->outputRateDenom = atoi(p);
        break;
      case 'j':
        cml->inputRateNumer = atoi(p);
        break;
      case 'J':
        cml->inputRateDenom = atoi(p);
        break;
      case 'p':
        cml->cabacInitFlag = atoi(p);
        break;

      case 'q':
        cml->qpHdr = atoi(p);
        break;
      case 'n':
        cml->qpMin = atoi(p);
        break;
      case 'm':
        cml->qpMax = atoi(p);
        break;

      case 'B':
        cml->bitPerSecond = atoi(p);
        break;

      case 'U':
        cml->picRc = atoi(p);
        break;
      case 'u':
        cml->ctbRc = atoi(p); //CTB_RC
        break;
      case 'C':
        cml->hrdConformance = atoi(p);
        break;
      case 'c':
        cml->cpbSize = atoi(p);
        break;
      case 's':
        cml->picSkip = atoi(p);
        break;
      case 'T':
        cml->test_data_files = p;
        break;
      case 'L':
        cml->level = atoi(p);
        break;
      case 'P':
        cml->profile = atoi(p);
        break;
      case 'r':
        cml->rotation = atoi(p);
        break;
      case 'R':
        cml->intraPicRate = atoi(p);
        break;
      case 'A':
        cml->intraQpDelta = atoi(p);
        break;
      case 'D':
        cml->disableDeblocking = atoi(p);
        break;
      case 'W':
        cml->tc_Offset = atoi(p);
        break;
      case 'E':
        cml->beta_Offset = atoi(p);
        break;
      case 'e':
        cml->sliceSize = atoi(p);
        break;

      case 'g':
        cml->gopLength = atoi(p);
        break;
      case 'G':
        cml->fixedIntraQp = atoi(p);
        break;
      case 'N':
        cml->byteStream = atoi(p);
        break;
      case 'I':
        cml->chromaQpOffset = atoi(p);
        break;
      case 'M':
        cml->enableSao = atoi(p);
        break;

      case 'k':
        cml->videoRange = atoi(p);
        break;

      case 'S':
        cml->sei = atoi(p);
        break;
      case 'V':
        cml->bFrameQpDelta = atoi(p);
        break;
      case '1':
        if (bpsAdjustCount == MAX_BPS_ADJUST)
          break;
        /* Argument must be "xx:yy", replace ':' with 0 */
        if ((i = ParseDelim(p, ':')) == -1) break;
        /* xx is frame number */
        cml->bpsAdjustFrame[bpsAdjustCount] = atoi(p);
        /* yy is new target bitrate */
        cml->bpsAdjustBitrate[bpsAdjustCount] = atoi(p + i + 1);
        bpsAdjustCount++;
        break;

      case '0':
        /* Check long option */
        if (strcmp(prm.longOpt, "roi1DeltaQp") == 0)
          cml->roi1AreaEnable = cml->roi1DeltaQp = atoi(p);

        if (strcmp(prm.longOpt, "roi2DeltaQp") == 0)
          cml->roi2AreaEnable = cml->roi2DeltaQp = atoi(p);

        
        if (strcmp(prm.longOpt, "roiMapDeltaQpBlockUnit") == 0)
          cml->roiMapDeltaQpBlockUnit = atoi(p);

        if (strcmp(prm.longOpt, "roiMapDeltaQpEnable") == 0)
          cml->roiMapDeltaQpEnable = atoi(p);

        if (strcmp(prm.longOpt, "outBufSizeMax") == 0)
          cml->outBufSizeMax = atoi(p);

        if (strcmp(prm.longOpt, "cir") == 0)
        {
          /* Argument must be "xx:yy", replace ':' with 0 */
          if ((i = ParseDelim(p, ':')) == -1) break;
          /* xx is cir start */
          cml->cirStart = atoi(p);
          /* yy is cir interval */
          cml->cirInterval = atoi(p + i + 1);
        }

        if (strcmp(prm.longOpt, "intraArea") == 0)
        {
          /* Argument must be "xx:yy:XX:YY".
           * xx is left coordinate, replace first ':' with 0 */
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->intraAreaLeft = atoi(p);
          /* yy is top coordinate */
          p += i + 1;
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->intraAreaTop = atoi(p);
          /* XX is right coordinate */
          p += i + 1;
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->intraAreaRight = atoi(p);
          /* YY is bottom coordinate */
          p += i + 1;
          cml->intraAreaBottom = atoi(p);
          cml->intraAreaEnable = 1;
        }

        if (strcmp(prm.longOpt, "roi1Area") == 0)
        {
          /* Argument must be "xx:yy:XX:YY".
           * xx is left coordinate, replace first ':' with 0 */
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->roi1AreaLeft = atoi(p);
          /* yy is top coordinate */
          p += i + 1;
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->roi1AreaTop = atoi(p);
          /* XX is right coordinate */
          p += i + 1;
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->roi1AreaRight = atoi(p);
          /* YY is bottom coordinate */
          p += i + 1;
          cml->roi1AreaBottom = atoi(p);
          cml->roi1AreaEnable = 1;
        }

        if (strcmp(prm.longOpt, "roi2Area") == 0)
        {
          /* Argument must be "xx:yy:XX:YY".
           * xx is left coordinate, replace first ':' with 0 */
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->roi2AreaLeft = atoi(p);
          /* yy is top coordinate */
          p += i + 1;
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->roi2AreaTop = atoi(p);
          /* XX is right coordinate */
          p += i + 1;
          if ((i = ParseDelim(p, ':')) == -1) break;
          cml->roi2AreaRight = atoi(p);
          /* YY is bottom coordinate */
          p += i + 1;
          cml->roi2AreaBottom = atoi(p);
          cml->roi2AreaEnable = 1;
        }

        if (strcmp(prm.longOpt, "smoothingIntra") == 0)
        {
          /* Argument must be "xx:yy:XX:YY".
           * xx is left coordinate, replace first ':' with 0 */
          cml->strong_intra_smoothing_enabled_flag = atoi(p);
          ASSERT(cml->strong_intra_smoothing_enabled_flag < 2);
        }
        
        if (strcmp(prm.longOpt, "scaledWidth") == 0)
          cml->scaledWidth = atoi(p);

        if (strcmp(prm.longOpt, "scaledHeight") == 0)
          cml->scaledHeight = atoi(p);

        if (strcmp(prm.longOpt, "enableDeblockOverride") == 0)
          cml->enableDeblockOverride = atoi(p);
        if (strcmp(prm.longOpt, "deblockOverride") == 0)
          cml->deblockOverride = atoi(p);

        if (strcmp(prm.longOpt, "enableScalingList") == 0)
          cml->enableScalingList = atoi(p);

        if (strcmp(prm.longOpt, "compressor") == 0)
          cml->compressor = atoi(p);
        if (strcmp(prm.longOpt, "testId") == 0)
          cml->testId = atoi(p);
        if (strcmp(prm.longOpt, "interlacedFrame") == 0)
          cml->interlacedFrame = atoi(p);

        if (strcmp(prm.longOpt, "fieldOrder") == 0)
          cml->fieldOrder = atoi(p);
        if (strcmp(prm.longOpt, "gopSize") == 0)
          cml->gopSize = atoi(p);
        if (strcmp(prm.longOpt, "gopConfig") == 0)
          cml->gopCfg = p;
        if (strcmp(prm.longOpt, "gopLowdelay") == 0)
          cml->gopLowdelay = atoi(p);

        if (strcmp(prm.longOpt, "bFrameQpDelta") == 0)
          cml->bFrameQpDelta = atoi(p);

        if (strcmp(prm.longOpt, "outReconFrame") == 0)
          cml->outReconFrame = atoi(p);

        
        if (strcmp(prm.longOpt, "gdrDuration") == 0)
          cml->gdrDuration = atoi(p);

        
        if (strcmp(prm.longOpt, "bitVarRangeI") == 0)
          cml->bitVarRangeI = atoi(p);

        if (strcmp(prm.longOpt, "bitVarRangeP") == 0)
          cml->bitVarRangeP = atoi(p);

        if (strcmp(prm.longOpt, "bitVarRangeB") == 0)
          cml->bitVarRangeB = atoi(p);
        if (strcmp(prm.longOpt, "tolMovingBitRate") == 0)
          cml->tolMovingBitRate = atoi(p);

        if (strcmp(prm.longOpt, "monitorFrames") == 0)
          cml->monitorFrames = atoi(p);
        if (strcmp(prm.longOpt, "roiMapDeltaQpFile") == 0)
          cml->roiMapDeltaQpFile = p;


       //wiener denoise
        if (strcmp(prm.longOpt, "noiseReductionEnable") == 0)
          cml->noiseReductionEnable = atoi(p);
        if (strcmp(prm.longOpt, "noiseLow") == 0)
          cml->noiseLow = atoi(p);
        if (strcmp(prm.longOpt, "noiseFirstFrameSigma") == 0)
          cml->firstFrameSigma = atoi(p);

        /* multi-stream */
        if (strcmp(prm.longOpt, "multimode") == 0)
          cml->multimode = atoi(p);
        if (strcmp(prm.longOpt, "streamcfg") == 0)
          cml->streamcfg[cml->nstream++] = p;
        if (strcmp(prm.longOpt, "bitDepthLuma") == 0)
          cml->bitDepthLuma = atoi(p);

        if (strcmp(prm.longOpt, "bitDepthChroma") == 0)
          cml->bitDepthChroma = atoi(p);

        if (strcmp(prm.longOpt, "blockRCSize") == 0)
          cml->blockRCSize = atoi(p);
          
          
        break;



      default:
        break;
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
  default_parameter
------------------------------------------------------------------------------*/
void default_parameter(commandLine_s *cml)
{
  int i;
  cml->input      = "input.yuv";
  cml->output     = "stream.hevc";
  cml->recon      = "deblock.yuv";
  cml->firstPic    = 0;
  cml->lastPic   = 100;
  cml->inputRateNumer      = 30;
  cml->inputRateDenom      = 1;
  cml->outputRateNumer      = DEFAULT;
  cml->outputRateDenom      = DEFAULT;
  cml->test_data_files    = getenv("TEST_DATA_FILES");
  cml->lumWidthSrc      = DEFAULT;
  cml->lumHeightSrc     = DEFAULT;
  cml->horOffsetSrc = DEFAULT;
  cml->verOffsetSrc = DEFAULT;
  cml->rotation = 0;
  cml->inputFormat = 0;

  cml->width      = DEFAULT;
  cml->height     = DEFAULT;
  cml->max_cu_size  = 64;
  cml->min_cu_size  = 8;
  cml->max_tr_size  = 16;
  cml->min_tr_size  = 4;
  cml->tr_depth_intra = 2;  //mfu =>0
  cml->tr_depth_inter = (cml->max_cu_size == 64) ? 4 : 3;
  cml->intraPicRate   = 0;  // only first is IDR.


  cml->bitPerSecond = 1000000;
  cml->bitVarRangeI = 2000;
  cml->bitVarRangeP = 2000;
  cml->bitVarRangeB = 2000;
  cml->tolMovingBitRate = 2000;
  cml->monitorFrames = DEFAULT;
  

  cml->intraQpDelta = DEFAULT;
  cml->bFrameQpDelta = -1;

  cml->disableDeblocking = 0;

  cml->tc_Offset = -2;
  cml->beta_Offset = 5;

  cml->qpHdr = DEFAULT;
  cml->qpMin = DEFAULT;
  cml->qpMax = DEFAULT;
  cml->picRc = DEFAULT;
  cml->ctbRc = DEFAULT; //CTB_RC
  cml->cpbSize = 1000000;
  cml->gopLength = DEFAULT;
  cml->fixedIntraQp = 0;
  cml->hrdConformance = 0;

  cml->byteStream = 1;

  cml->chromaQpOffset = 0;


  cml->enableSao = 1;

  cml->strong_intra_smoothing_enabled_flag = 0;

  cml->intraAreaLeft = cml->intraAreaRight = cml->intraAreaTop =
                         cml->intraAreaBottom = -1;  /* Disabled */
  cml->gdrDuration=0;

  cml->picSkip = 0;

  cml->sliceSize = 0;

  cml->cabacInitFlag = 0;
  cml->cirStart = 0;
  cml->cirInterval = 0;
  cml->enableDeblockOverride = 0;
  cml->deblockOverride = 0;

  cml->enableScalingList = 0;

  cml->compressor = 0;
  cml->sei = 0;
  cml->videoRange = 0;
  cml->level = DEFAULT;
  cml->profile = DEFAULT;
  cml->bitDepthLuma = DEFAULT;
  cml->bitDepthChroma= DEFAULT;
  cml->blockRCSize= DEFAULT;
  
  cml->gopLength = DEFAULT;
  cml->gopSize = 0;
  cml->gopCfg = NULL;
  cml->gopLowdelay = 0;

  cml->outReconFrame=1;
  
  cml->roiMapDeltaQpBlockUnit=0;
  
  cml->roiMapDeltaQpEnable=0;
  cml->roiMapDeltaQpFile = NULL;
  cml->interlacedFrame = 0;
  cml->noiseReductionEnable = 0;

  cml->multimode = 0;
  cml->nstream = 0;
  for(i = 0; i < MAX_STREAMS; i++)
    cml->streamcfg[i] = NULL;
}
/*------------------------------------------------------------------------------

    ReadUserData
        Read user data from file and pass to encoder

    Params:
        name - name of file in which user data is located

    Returns:
        NULL - when user data reading failed
        pointer - allocated buffer containing user data

------------------------------------------------------------------------------*/
u8 *ReadUserData(HEVCEncInst encoder, char *name)
{
  FILE *file = NULL;
  i32 byteCnt;
  u8 *data;

  if (name == NULL)
    return NULL;

  if (strcmp("0", name) == 0)
    return NULL;

  /* Get user data length from file */
  file = fopen(name, "rb");
  if (file == NULL)
  {
    printf("Unable to open User Data file: %s\n", name);
    return NULL;
  }
  fseeko(file, 0L, SEEK_END);
  byteCnt = ftell(file);
  rewind(file);

  /* Minimum size of user data */
  if (byteCnt < 16)
    byteCnt = 16;

  /* Maximum size of user data */
  if (byteCnt > 2048)
    byteCnt = 2048;

  /* Allocate memory for user data */
  if ((data = (u8 *) malloc(sizeof(u8) * byteCnt)) == NULL)
  {
    fclose(file);
    printf("Unable to alloc User Data memory\n");
    return NULL;
  }

  /* Read user data from file */
  fread(data, sizeof(u8), byteCnt, file);
  fclose(file);

  printf("User data: %d bytes [%d %d %d %d ...]\n",
         byteCnt, data[0], data[1], data[2], data[3]);

  /* Pass the data buffer to encoder
   * The encoder reads the buffer during following HEVCEncStrmEncode() calls.
   * User data writing must be disabled (with HEVCEncSetSeiUserData(enc, 0, 0)) */
  HEVCEncSetSeiUserData(encoder, data, byteCnt);

  return data;
}

//           Type POC QPoffset  QPfactor  num_ref_pics ref_pics  used_by_cur
char *RpsDefault_GOPSize_1[] = {
    "Frame1:  P    1   0        0.8     0      1        -1         1",
    NULL,
};

char *RpsDefault_GOPSize_2[] = {
    "Frame1:  P    2   0        0.6     0      1        -2         1",
    "Frame2:  B    1   0        0.68    0      2        -1 1       1 1",
    NULL,
};

char *RpsDefault_GOPSize_3[] = {
    "Frame1:  P    3   0        0.5     0      1        -3         1   ",
    "Frame2:  B    1   0        0.5     0      2        -1 2       1 1 ",
    "Frame3:  B    2   0        0.68    0      2        -1 1       1 1 ",
    NULL,
};


char *RpsDefault_GOPSize_4[] = {
    "Frame1:  P    4   0        0.5      0     1       -4         1 ",
    "Frame2:  B    2   0        0.3536   0     2       -2 2       1 1", 
    "Frame3:  B    1   0        0.5      0     3       -1 1 3     1 1 0", 
    "Frame4:  B    3   0        0.5      0     2       -1 1       1 1 ",
    NULL,
};

char *RpsDefault_GOPSize_5[] = {
    "Frame1:  P    5   0        0.442    0     1       -5         1 ",
    "Frame2:  B    2   0        0.3536   0     2       -2 3       1 1", 
    "Frame3:  B    1   0        0.68     0     3       -1 1 4     1 1 0", 
    "Frame4:  B    3   0        0.3536   0     2       -1 2       1 1 ",
    "Frame5:  B    4   0        0.68     0     2       -1 1       1 1 ",
    NULL,
};

char *RpsDefault_GOPSize_6[] = {
    "Frame1:  P    6   0        0.442    0     1       -6         1 ",
    "Frame2:  B    3   0        0.3536   0     2       -3 3       1 1", 
    "Frame3:  B    1   0        0.3536   0     3       -1 2 5     1 1 0", 
    "Frame4:  B    2   0        0.68     0     3       -1 1 4     1 1 0",
    "Frame5:  B    4   0        0.3536   0     2       -1 2       1 1 ",
    "Frame6:  B    5   0        0.68     0     2       -1 1       1 1 ",
    NULL,
};

char *RpsDefault_GOPSize_7[] = {
    "Frame1:  P    7   0        0.442    0     1       -7         1 ",
    "Frame2:  B    3   0        0.3536   0     2       -3 4       1 1", 
    "Frame3:  B    1   0        0.3536   0     3       -1 2 6     1 1 0", 
    "Frame4:  B    2   0        0.68     0     3       -1 1 5     1 1 0",
    "Frame5:  B    5   0        0.3536   0     2       -2 2       1 1 ",
    "Frame6:  B    4   0        0.68     0     3       -1 1 3     1 1 0",
    "Frame7:  B    6   0        0.68     0     2       -1 1       1 1 ",
    NULL,
};

char *RpsDefault_GOPSize_8[] = {
    "Frame1:  P    8   0        0.442    0  1           -8        1 ",
    "Frame2:  B    4   0        0.3536   0  2           -4 4      1 1 ",
    "Frame3:  B    2   0        0.3536   0  3           -2 2 6    1 1 0 ",
    "Frame4:  B    1   0        0.68     0  4           -1 1 3 7  1 1 0 0",
    "Frame5:  B    3   0        0.68     0  3           -1 1 5    1 1 0",
    "Frame6:  B    6   0        0.3536   0  2           -2 2      1 1",
    "Frame7:  B    5   0        0.68     0  3           -1 1 3    1 1 0",
    "Frame8:  B    7   0        0.68     0  2           -1 1      1 1",
    NULL,
};

char *RpsLowdelayDefault_GOPSize_1[] = {
    "Frame1:  B    1   0        0.65      0     2       -1 -2         1 1",
    NULL,
};

char *RpsLowdelayDefault_GOPSize_2[] = {
    "Frame1:  B    1   0        0.4624    0     2       -1 -3         1 1",
    "Frame2:  B    2   0        0.578     0     2       -1 -2         1 1", 
    NULL,
};

char *RpsLowdelayDefault_GOPSize_3[] = {
    "Frame1:  B    1   0        0.4624    0     2       -1 -4         1 1",
    "Frame2:  B    2   0        0.4624    0     2       -1 -2         1 1", 
    "Frame3:  B    3   0        0.578     0     2       -1 -3         1 1", 
    NULL,
};

char *RpsLowdelayDefault_GOPSize_4[] = {
    "Frame1:  B    1   0        0.4624    0     2       -1 -5         1 1",
    "Frame2:  B    2   0        0.4624    0     2       -1 -2         1 1", 
    "Frame3:  B    3   0        0.4624    0     2       -1 -3         1 1", 
    "Frame4:  B    4   0        0.578     0     2       -1 -4         1 1",
    NULL,
};

char *nextToken (char *str)
{
  char *p = strchr(str, ' ');
  if (p)
  {
    while (*p == ' ') p ++;
    if (*p == '\0') p = NULL;
  }
  return p;
}
int ParseGopConfigString (char *line, HEVCGopConfig *gopCfg, int frame_idx, int gopSize)
{
  if (!line)
    return -1;

  //format: FrameN Type POC QPoffset QPfactor  num_ref_pics ref_pics  used_by_cur
  int frameN, poc, num_ref_pics, i;
  char type;
  HEVCGopPicConfig *cfg = &(gopCfg->pGopPicCfg[gopCfg->size ++]);

  //frame idx
  sscanf (line, "Frame%d", &frameN);
  if (frameN != (frame_idx + 1)) return -1;

  //frame type
  line = nextToken(line);
  if (!line) return -1;
  sscanf (line, "%c", &type);
  if (type == 'P' || type == 'p')
    cfg->codingType = HEVCENC_PREDICTED_FRAME;
  else if (type == 'B' || type == 'b')
    cfg->codingType = HEVCENC_BIDIR_PREDICTED_FRAME;
  else
    return -1;

  //poc
  line = nextToken(line);
  if (!line) return -1;
  sscanf (line, "%d", &poc);
  if (poc < 1 || poc > gopSize) return -1;
  cfg->poc = poc;

  //qp offset
  line = nextToken(line);
  if (!line) return -1;
  sscanf (line, "%d", &(cfg->QpOffset));

  //qp factor
  line = nextToken(line);
  if (!line) return -1;
  sscanf (line, "%lf", &(cfg->QpFactor));

  //temporalId factor
  line = nextToken(line);
  if (!line) return -1;
  sscanf (line, "%d", &(cfg->temporalId));

  //num_ref_pics
  line = nextToken(line);
  if (!line) return -1;
  sscanf (line, "%d", &num_ref_pics);
  if (num_ref_pics < 0 || num_ref_pics > HEVCENC_MAX_REF_FRAMES)
  {
    printf ("GOP Config: Error, num_ref_pic can not be more than %d \n", HEVCENC_MAX_REF_FRAMES);
    return -1;
  }
  cfg->numRefPics = num_ref_pics;

  //ref_pics
  for (i = 0; i < num_ref_pics; i ++)
  {
    line = nextToken(line);
    if (!line) return -1;
    sscanf (line, "%d", &(cfg->refPics[i].ref_pic));

    if (cfg->refPics[i].ref_pic < (-gopSize-1))
    {
      printf ("GOP Config: Error, Reference picture %d for GOP size = %d is not supported\n", cfg->refPics[i].ref_pic, gopSize);
      return -1;
    }
  }
  if (i < num_ref_pics) return -1;

  //used_by_cur
  for (i = 0; i < num_ref_pics; i ++)
  {
    line = nextToken(line);
    if (!line) return -1;
    sscanf (line, "%u", &(cfg->refPics[i].used_by_cur));
  }
  if (i < num_ref_pics) return -1;
  return 0;
}

int HEVCParseGopConfigFile (int gopSize, char *fname, HEVCGopConfig *gopCfg)
{
#define MAX_LINE_LENGTH 1024
  int frame_idx = 0, line_idx = 0;
  char achParserBuffer[MAX_LINE_LENGTH];
  FILE *fIn = fopen (fname, "r");
  if (fIn == NULL)
  {
    printf("GOP Config: Error, Can Not Open File %s\n", fname );
    return -1;
  }

  while (frame_idx < gopSize)
  {
    if (feof (fIn)) break;
    line_idx ++;
    achParserBuffer[0] = '\0';
    // Read one line
    char *line = fgets ((char *) achParserBuffer, MAX_LINE_LENGTH, fIn);
    if (!line) break;
    //handle line end
    char* s = strpbrk(line, "#\n");
    if(s) *s = '\0';

    line = strstr(line, "Frame");
    if (line)
    {
      if (ParseGopConfigString (line, gopCfg, frame_idx, gopSize) < 0)
        break;

      frame_idx ++;
    }
  }

  fclose(fIn);
  if (frame_idx != gopSize)
  {
    printf ("GOP Config: Error, Parsing File %s Failed at Line %d\n", fname, line_idx);
    return -1;
  }
  return 0;
}

int HEVCReadGopConfig (char *fname, char **config, HEVCGopConfig *gopCfg, int gopSize, u8 *gopCfgOffset)
{
  int ret = -1;
  if (gopCfgOffset)
    gopCfgOffset[gopSize] = gopCfg->size;
  if(fname)
  {
    ret = HEVCParseGopConfigFile (gopSize, fname, gopCfg);
  }
  else if(config)
  {
    int id = 0;
    while (config[id])
    {
      ParseGopConfigString (config[id], gopCfg, id, gopSize);
      id ++;
    }
    ret = 0;
  }
  return ret;
}

int HEVCInitGopConfigs (int gopSize, commandLine_s *cml, HEVCGopConfig *gopCfg, u8 *gopCfgOffset)
{
  char *fname = cml->gopCfg;
  char **default_configs[8] = {
              cml->gopLowdelay ? RpsLowdelayDefault_GOPSize_1: RpsDefault_GOPSize_1, 
              cml->gopLowdelay ? RpsLowdelayDefault_GOPSize_2: RpsDefault_GOPSize_2,
              cml->gopLowdelay ? RpsLowdelayDefault_GOPSize_3: RpsDefault_GOPSize_3,
              cml->gopLowdelay ? RpsLowdelayDefault_GOPSize_4: RpsDefault_GOPSize_4,
              RpsDefault_GOPSize_5,
              RpsDefault_GOPSize_6,
              RpsDefault_GOPSize_7,
              RpsDefault_GOPSize_8 };

  if (cml->gopLowdelay && gopSize > 4)
  {
    printf ("GOP Config: Error, Not support low delay for GOP size > 4\n");
    return -1;
  }
  if (gopSize < 0 || gopSize > 8)
  {
    printf ("GOP Config: Error, Invalid GOP Size\n");
    return -1;
  }

  int pre_load_num = (gopSize >= 4 || gopSize == 0) ? 4 : gopSize;
  int i;
  for (i = 1; i <= pre_load_num; i ++)
  {    
    if (HEVCReadGopConfig (gopSize==i ? fname : NULL, default_configs[i-1], gopCfg, i, gopCfgOffset))
      return -1;
  }

  if (gopSize == 0)
  {
    //gop6
    if (HEVCReadGopConfig (NULL, default_configs[5], gopCfg, 6, gopCfgOffset))
      return -1;
    //gop8
    if (HEVCReadGopConfig (NULL, default_configs[7], gopCfg, 8, gopCfgOffset))
      return -1;
  }
  else if (gopSize > 4)
  {
    //gopSize
    if (HEVCReadGopConfig (fname, default_configs[gopSize-1], gopCfg, gopSize, gopCfgOffset))
      return -1;
  }

  //Compatible with old bFrameQpDelta setting
  if (cml->bFrameQpDelta >= 0 && fname == NULL)
  {
    for (i = 0; i < gopCfg->size; i++)
    {
      HEVCGopPicConfig *cfg = &(gopCfg->pGopPicCfg[i]);
      if (cfg->codingType == HEVCENC_BIDIR_PREDICTED_FRAME)
        cfg->QpOffset = cml->bFrameQpDelta;
    }
  }
#if 0
      //print for debug
      int idx;
      printf ("=== REF PICTURE SETS from %s ===\n",fname ? fname : "DEFAULT");
      for (idx = 0; idx < gopCfg->size; idx ++)
      {
        int i;
        HEVCGopPicConfig *cfg = &(gopCfg->pGopPicCfg[idx]);
        char type = cfg->codingType==HEVCENC_PREDICTED_FRAME ? 'P' : cfg->codingType == HEVCENC_INTRA_FRAME ? 'I' : 'B';
        printf (" %2d Frame %c %d %d %f %d", idx, type, cfg->poc, cfg->QpOffset, cfg->QpFactor, cfg->numRefPics);
        for (i = 0; i < cfg->numRefPics; i ++)
          printf (" %d", cfg->refPics[i].ref_pic);
        for (i = 0; i < cfg->numRefPics; i ++)
          printf (" %d", cfg->refPics[i].used_by_cur);
        printf("\n");
      }
#endif
  return 0;
}

/*------------------------------------------------------------------------------
  help
------------------------------------------------------------------------------*/
void help(void)
{
  //fprintf(stdout, "Version: %s\n", git_version);
  fprintf(stdout, "Usage:  %s [options] -i inputfile\n\n", "enc");
  fprintf(stdout,
          "Default parameters are marked inside []. More detailed description of\n"
          "HEVC API parameters can be found from HEVC API Manual.\n\n");

  fprintf(stdout,
          "  -H --help                        This help,\n");
  fprintf(stdout,
          " Parameters affecting which input frames are encoded:\n"
          "  -i[s] --input                    Read input video sequence from file. [input.yuv]\n"
          "  -o[s] --output                   Write output HEVC stream to file. [stream.hevc]\n"
          "  -a[n] --firstPic                 First picture of input file to encode. [0]\n"
          "  -b[n] --lastPic                  Last picture of input file to encode. [100]\n"
          "  -f[n] --outputRateNumer          1..1048575 Output picture rate numerator. [30]\n"
          "  -F[n] --outputRateDenom          1..1048575 Output picture rate denominator. [1]\n"
          "                                   Encoded frame rate will be\n"
          "                                   outputRateNumer/outputRateDenom fps\n"
          "  -j[n] --inputRateNumer           1..1048575 Input picture rate numerator. [30]\n"
          "  -J[n] --inputRateDenom           1..1048575 Input picture rate denominator. [1]\n\n");

  fprintf(stdout,
          " Parameters affecting input frame and encoded frame resolutions and cropping:\n"
          "  -w[n] --lumWidthSrc              Width of source image. [176]\n"
          "  -h[n] --lumHeightSrc             Height of source image. [144]\n");

  fprintf(stdout,
          "  -x[n] --width                    Width of encoded output image. [--lumWidthSrc]\n"
          "  -y[n] --height                   Height of encoded output image. [--lumHeightSrc]\n"
          "  -X[n] --horOffsetSrc             Output image horizontal cropping offset,must be even. [0]\n"
          "  -Y[n] --verOffsetSrc             Output image vertical cropping offset,must be even. [0]\n\n"
         );

  fprintf(stdout,
          " Parameters for pre-processing frames before encoding:\n"
          "  -l[n] --inputFormat              Input YUV format. [0]\n"
          "                                   0 - YUV420 planar CbCr (IYUV/I420)\n"
          "                                   1 - YUV420 semiplanar CbCr (NV12)\n"
          "                                   2 - YUV420 semiplanar CrCb (NV21)\n"
          "                                   3 - YUYV422 interleaved (YUYV/YUY2)\n"
          "                                   4 - UYVY422 interleaved (UYVY/Y422)\n"
          "                                   5 - RGB565 16bpp\n"
          "                                   6 - BGR565 16bpp\n"
          "                                   7 - RGB555 16bpp\n"
          "                                   8 - BGR555 16bpp\n"
          "                                   9 - RGB444 16bpp\n"
          "                                   10 - BGR444 16bpp\n"
          "                                   11 - RGB888 32bpp\n"
          "                                   12 - BGR888 32bpp\n"
          "                                   13 - RGB101010 32bpp\n"
          "                                   14 - BGR101010 32bpp\n"
          "                                   15 - YUV420 10 bit planar CbCr (I010)\n"
          "                                   16 - YUV420 10 bit planar CbCr (P010)\n"
          "                                   17 - YUV420 10 bit planar CbCr packed \n"
          "                                   18 - YUV420 10 bit packed(Y0L2) \n"

          "  -O[n] --colorConversion          RGB to YCbCr color conversion type. [0]\n"
          "                                   0 - BT.601\n"
          "                                   1 - BT.709\n"
          "                                   2 - User defined, coeffs defined in testbench.\n"
         );


  fprintf(stdout,

          "  -r[n] --rotation                 Rotate input image. [0]\n"
          "                                   0 - disabled\n"
          "                                   1 - 90  degrees right\n"
          "                                   2 - 90  degrees left\n"
          "                                   3 - 180 degrees right\n"
         );


  fprintf(stdout,
          " Parameters affecting the output stream and encoding tools:\n"
          "  -P[n] --profile                  HEVC profile  [0]\n"
          "                                   H2 only support main profile and main still picture profile\n"   
          "                                   0=main profile,1=main still picture profile\n"
          "                                   2=main 10 profile\n"
          "  -L[n] --level                    HEVC level 180 = Level 6.0*30  [180]\n"
          "                                   H2 only support less than or equal to Level 5.1 main tier\n"
          "                                   for Level 5.0 and Level 5.1, only support resolution up to 4096x2048 \n"
          "                                   Each level has resolution and bitrate limitations:\n"
          "                                   30  -Level1.0  QCIF      (176x144)       128kbits\n"
          "                                   60  -Level2.0  CIF       (352x288)       1.5Mbits\n"
          "                                   63  -Level2.1  Q720p     (640x360)       3.0Mbits\n"
          "                                   90  -Level3.0  QHD       (960x540)       6.0Mbits\n"
          "                                   93  -Level3.1  720p HD   (1280x720)      10.0Mbits\n"
          "                                   120 -Level4.0  2Kx1080   (2048x1080)     12.0Mbits   High tier 30Mbits\n"
          "                                   123 -Level4.1  2Kx1080   (2048x1080)     20.0Mbits   High tier 50Mbits\n"
          "                                   150 -Level5.0  4096x2160 (4096x2160)     25.0Mbits   High tier 100Mbits\n"
          "                                   153 -Level5.1  4096x2160 (4096x2160)     40.0Mbits   High tier 160Mbits\n"
          "                                   156 -Level5.2  4096x2160 (4096x2160)     60.0Mbits   High tier 240Mbits\n"
          "                                   180 -Level6.0  8192x4320 (8192x4320)     60.0Mbits   High tier 240Mbits\n"
          /*
                                      "                                                              183 -Level6.1  8192x4320 (8192x4320)     120.0Mbits  High tier 480Mbits\n"
                                      "                                                              186 -Level6.2  8192x4320 (8192x4320)     240.0Mbits  High tier 800Mbits\n"*/
          "        --bitDepthLuma             bit depth of sample of luma in encoded stream [8]\n"
          "                                   8=8 bit,9=9 bit, 10= 10bit\n"   
          "        --bitDepthChroma           bit depth of sample of Chroma in encoded stream  [8]\n"
          "                                   8=8 bit,9=9 bit, 10= 10bit\n"   

          "  -N[n] --byteStream               Stream type. [1]\n"
          "                                   0 - NAL units. Nal sizes returned in <nal_sizes.txt>\n"
          "                                   1 - byte stream according to Hevc Standard Annex B.\n"

          "  -k[n] --videoRange               0..1 Video signal sample range value in Hevc stream. [0]\n"
          "                                   0 - Y range in [16..235] Cb,Cr in [16..240]\n"
          "                                   1 - Y,Cb,Cr range in [0..255]\n"
         );

  fprintf(stdout,
          "  -S[n] --sei                      Enable SEI messages (buffering period + picture timing) [0]\n"
          "                                   Writes Supplemental Enhancement Information messages\n"
          "                                   containing information about picture time stamps\n"
          "                                   and picture buffering into stream.\n");


  fprintf(stdout,
          "  -R[n] --intraPicRate             Intra picture rate in frames. [0]\n"
          "                                   Forces every Nth frame to be encoded as intra frame.\n"
          "  -p[n] --cabacInitFlag            0..1 Initialization value for CABAC. [0]\n"
         );

  fprintf(stdout,
          " Parameters affecting the rate control and output stream bitrate:\n"
          "  -B[n] --bitPerSecond             10000..levelMax, target bitrate for rate control [1000000]\n"
          "        --tolMovingBitRate         0...2000%% percent tolerance over target bitrate of moving bit rate [2000]\n"
          "        --monitorFrames            10...120, how many frames will be monitored for moving bit rate [frame rate]\n"
          "        --bitVarRangeI             10...2000%% percent variations over average bits per frame for I frame [2000]\n"
          "        --bitVarRangeP             10...2000%% percent variations over average bits per frame for P frame [2000]\n"
          "        --bitVarRangeB             10...2000%% percent variations over average bits per frame for B frame [2000]\n"
          "  -U[n] --picRc                    0=OFF, 1=ON Picture rate control. [0]\n"
          "                                   Calculates new target QP for every frame.\n"
          "  -u[n] --ctbRc                    0=OFF, 1=ON block rate control. [0]\n"/*CTB_RC*/
          "                                   adaptive adjustment of QP inside frame. \n"
          "        --blockRCSize              unit size of block Rate control [0]\n"
          "                                   0=64x64 ,1=32x32, 2=16x16\n"  
          "  -C[n] --hrdConformance           0=OFF, 1=ON HRD conformance. [0]\n"
          "                                   Uses standard defined model to limit bitrate variance.\n"
          "  -c[n] --cpbSize                  HRD Coded Picture Buffer size in bits. [1000000]\n"
          "                                   Buffer size used by the HRD model.\n"
          "  -g[n] --gopLength                Group Of Pictures length in frames, 1..300 [intraPicRate]\n"
          "                                   Rate control allocates bits for one GOP and tries to\n"
          "                                   match the target bitrate at the end of each GOP.\n"
          "                                   Typically GOP begins with an intra frame, but this\n"
          "                                   is not mandatory.\n"
          "        --gopSize                  GOP Size. [0]\n"
          "                                   0..8, 0 for adaptive GOP size; 1~7 for fixed GOP size\n"
          "        --gopConfig                GOP configuration file by user, which defines the GOP structure table:\n"
          "                                   FrameN Type POC QPoffset QPfactor  num_ref_pics ref_pics  used_by_cur\n"
          "                                   -FrameN: The table should contain GOPSize lines, named Frame1, Frame2, etc.\n"
          "                                    The frames are listed in decoding order.\n"
          "                                   -Type: Slice type, can be either P or B.\n"
          "                                   -POC: Display order of the frame within a GOP, ranging from 1 to GOPSize.\n"
          "                                   -QPOffset: It is added to the QP parameter to set the final QP value used for this frame.\n"
          "                                   -QPFactor: Weight used during rate distortion optimization.\n"
          "                                   -num_ref_pics: The number of reference pictures kept for this frame,\n"
          "                                    including references for current and future pictures.\n"
          "                                   -ref_pics: A List of num_ref_pics integers,\n" 
          "                                    specifying the delta_POC of the reference pictures kept, relative the POC of the current frame.\n"
          "                                   -used_by_cur: A List of num_ref_pics binaries,\n"
          "                                    specifying whether the corresponding reference picture is used by current picture.\n"
          "                                   If this config file is not specified, Default parameters will be used.\n"
          "        --gopLowdelay              Use default lowDelay GOP configuration if --gopConfig not specified, only valid for GOP size <= 4. [0]\n");

  fprintf(stdout,

          "  -s[n] --picSkip                  0=OFF, 1=ON Picture skip rate control. [0]\n"
          "                                   Allows rate control to skip frames if needed.\n"
          "  -q[n] --qpHdr                    -1..51, Initial target QP. [-1]\n"
          "                                   -1 = Encoder calculates initial QP. NOTE: use -q-1\n"
          "                                   The initial QP used in the beginning of stream.\n"
          "  -n[n] --qpMin                    0..51, Minimum frame header QP. [0]\n"
          "  -m[n] --qpMax                    0..51, Maximum frame header QP. [51]\n"
          "  -A[n] --intraQpDelta             -51..51, Intra QP delta. [-5]\n"
          "                                   QP difference between target QP and intra frame QP.\n"
          "  -G[n] --fixedIntraQp             0..51, Fixed Intra QP, 0 = disabled. [0]\n"
          "                                   Use fixed QP value for every intra frame in stream.\n"
          "  -V[n] --bFrameQpDelta            0..51, BFrame QP Delta.\n"
          "                                   QP difference between BFrame QP and target QP.\n"
          "                                   If a GOP config file is specified by --gopConfig, it will be overrided\n"

         );


  fprintf(stdout,

          "  -D[n] --disableDeblocking        0..1 Value of disable_deblocking_filter_idc [0]\n"
          "                                   0 = Inloop deblocking filter enabled (best quality)\n"
          "                                   1 = Inloop deblocking filter disabled\n"

          "  -M[n] --enableSao                0..1 Disable or Enable SAO Filter [1]\n"
          "                                   0 = SAO disabled \n"
          "                                   1 = SAO enabled\n"
          /*                "                                   2 = Inloop deblocking filter disabled on slice edges\n"*/
          "  -W[n] --tc_Offset                -6..6 Deblocking filter tc offset. [-2]\n"
          "  -E[n] --beta_Offset              -6..6 Deblocking filter beta offset. [5]\n"
          "  -e[n] --sliceSize                [0..height/ctu_size] slice size in number of CTU rows [0]\n"
          "                                   0 to encode each picture in one slice\n"
          "                                   [1..height/ctu_size] to each slice with N CTU row\n"


         );

  fprintf(stdout,
          " Other parameters for coding and reporting:\n"

          "  -z[n] --userData                 SEI User data file name. File is read and inserted\n"
          "                                       as SEI message before first frame.\n"
         );

  fprintf(stdout,
          "        --cir                      start:interval for Cyclic Intra Refresh.\n"
          "                                   Forces ctbs in intra mode. [0:0]\n"
          "        --intraArea                left:top:right:bottom CTB coordinates\n"
          "                                       specifying rectangular area of CTBs to\n"
          "                                       force encoding in intra mode.\n"
          "        --roi1Area                 left:top:right:bottom CTB coordinates\n"
          "        --roi2Area                 left:top:right:bottom CTB coordinates\n"
          "                                       specifying rectangular area of CTBs as\n"
          "                                       Region Of Interest with lower QP.\n"
          "        --roi1DeltaQp              -30..0, QP delta value for ROI 1 CTBs. [0]\n"
          "        --roi2DeltaQp              -30..0, QP delta value for ROI 2 CTBs. [0]\n"
          "        --roiMapDeltaQpBlockUnit   0-64x64,1-32x32,2-16x16,3-8x8. [0]\n"
          "        --roiMapDeltaQpEnable         0-disable,1-enable. [0]\n"
          "        --roiMapDeltaQpFile        DeltaQp map file is set by user, which defines the QP delta for each block in frame. \n"
          "                                   the block size is defined by --roiMapDeltaQpBlockUnit \n"
          "                                   QP delta range is from 0 to -15. To calculate block width and height, \n"
          "                                   alignment of 64 of picture width and height should be used.\n"          
          "        --scaledWidth              0..width, encoder image down-scaled, 0=disable [0]\n"
          "        --scaledHeight             0..height, encoder image down-scaled, written in scaled.yuv\n"
          "        --interlacedFrame          0=Input progressive field,\n"
          "                                   1=Input frame with fields interlaced [0]\n"
          "        --fieldOrder               Interlaced field order, 0=bottom first [0]\n"

         );

  fprintf(stdout,
          "        --compressor               0..3 Enable/Disable Embedded Compression [0]\n"
          "                                   0 = Disable Compression \n"
          "                                   1 = Only Enable Luma Compression \n"
          "                                   2 = Only Enable Chroma Compression \n"
          "                                   3 = Enable Both Luma and Chroma Compression \n"
         );

  fprintf(stdout,
          "        --enableDeblockOverride         0..1 deblocking override enable flag[0]\n"
          "                                   0 = Disable override \n"
          "                                   1 = enable override \n"
          "        --deblockOverride           0..1 deblocking override flag[0]\n"
          "                                   0 = not override filter parameter\n"
          "                                   1 = override filter parameter\n"
         );

  fprintf(stdout,
          "        --enableScalingList        0..1 using default scaling list or not[0]\n"
          "                                   0 = using average scaling list. \n"
          "                                   1 = using default scaling list. \n"
          "        --gdrDuration              how many pictures(frames not fields) it will take to do GDR [0]\n"
          "                                   0 : disable GDR(Gradual decoder refresh), >0: enable GDR \n"
          "                                   the start point of GDR is the frame which type is set to H264ENC_INTRA_FRAME \n"
          "                                   intraArea and roi1Area will be used to implement GDR function.\n"
         );

  fprintf(stdout,
          "        --noiseReductionEnable     enable noise reduction or not[0]\n"
          "                                   0 = disable NR. \n"
          "                                   1 = enable NR. \n"
          "        --noiseLow                 minimum noise value[10]. \n"
          "        --noiseFirstFrameSigma     noise estimation for start frames[11].\n"
         );

  fprintf(stdout,
          "\nTesting parameters that are not supported for end-user:\n"
          "  -I[n] --chromaQpOffset           -12..12 Chroma QP offset. [0]\n"
          "  --smoothingIntra                 [n]   0=Normal, 1=Strong. Enable or disable strong_intra_smoothing_enabled_flag. [1]\n"
          "  --testId                         Internal test ID. [0]\n"
          "  --outReconFrame                  0=reconstructed frame is not output, 1= reconstructed frame is output [1]\n"
          "\n");

  exit(OK);
}


/*------------------------------------------------------------------------------
  read_picture
------------------------------------------------------------------------------*/
i32 read_picture(struct test_bench *tb, u32 inputFormat, u32 src_img_size, u32 src_width, u32 src_height)
{
  i32 num;
  u64 seek;
  i32 w;
  i32 h;
  u8 *lum;
  u8 *cb;
  u8 *cr;
  i32 i;
  i32 stride;

  num = next_picture(tb, tb->picture_cnt) + tb->firstPic;

  if (num > tb->lastPic) return NOK;
  seek     = ((u64)num) * ((u64)src_img_size);
  lum = tb->lum;
  cb = tb->cb;
  cr = tb->cr;
  stride = get_aligned_width(src_width, inputFormat);

  if (stride == src_width)
  {
    if (file_read(tb->in, lum , seek, src_img_size)) return NOK;
  }
  else
  {

    //stride = (src_width+15)&(~15);
    if (inputFormat == HEVCENC_YUV420_PLANAR)
    {


      /* Lum */
      for (i = 0; i < src_height; i++)
      {
        if (file_read(tb->in, lum , seek, src_width)) return NOK;
        seek += src_width;
        lum += stride;
      }
      w = ((src_width + 1) >> 1);
      h = ((src_height + 1) >> 1);
      /* Cb */
      //seek += size_lum;
      for (i = 0; i < h; i++)
      {
        if (file_read(tb->in, cb, seek, w)) return NOK;
        seek += w;
        cb += stride / 2;
      }

      /* Cr */
      //seek += size_ch;
      for (i = 0; i < h; i++)
      {
        if (file_read(tb->in, cr, seek, w)) return NOK;

        seek += w;
        cr += stride / 2;
      }
    }
    else if (inputFormat < HEVCENC_YUV422_INTERLEAVED_YUYV)
    {
      /* Y */
      for (i = 0; i < src_height; i++)
      {
        if (file_read(tb->in, lum , seek, src_width)) return NOK;
        seek += src_width;
        lum += stride;
      }
      /* CbCr */
      for (i = 0; i < (src_height / 2); i++)
      {
        if (file_read(tb->in, cb, seek, src_width)) return NOK;
        seek += src_width;
        cb += stride;
      }
    }
    else if (inputFormat < HEVCENC_RGB888)
    {

      for (i = 0; i < src_height; i++)
      {

        if (file_read(tb->in, lum , seek, src_width * 2)) return NOK;
        seek += src_width * 2;
        lum += stride * 2;
      }
    }
    else if(inputFormat < HEVCENC_YUV420_PLANAR_10BIT_I010)
    {
      for (i = 0; i < src_height; i++)
      {

        if (file_read(tb->in, lum , seek, src_width * 4)) return NOK;
        seek += src_width * 4;
        lum += stride * 4;
      }
    }
    else if(inputFormat == HEVCENC_YUV420_PLANAR_10BIT_I010)
    {
    
      /* Lum */
      for (i = 0; i < src_height; i++)
      {
        if (file_read(tb->in, lum , seek, src_width*2)) return NOK;
        seek += src_width*2;
        lum += stride*2;
      }
      w = ((src_width + 1) >> 1);
      h = ((src_height + 1) >> 1);
      /* Cb */
      //seek += size_lum;
      for (i = 0; i < h; i++)
      {
        if (file_read(tb->in, cb, seek, w*2)) return NOK;
        seek += w*2;
        cb += stride;
      }

      /* Cr */
      //seek += size_ch;
      for (i = 0; i < h; i++)
      {
        if (file_read(tb->in, cr, seek, w*2)) return NOK;

        seek += w*2;
        cr += stride;
      }
    }
    else if(inputFormat == HEVCENC_YUV420_PLANAR_10BIT_P010)
    {
    
      /* Lum */
      for (i = 0; i < src_height; i++)
      {
        if (file_read(tb->in, lum , seek, src_width*2)) return NOK;
        seek += src_width*2;
        lum += stride*2;
      }

      h = ((src_height + 1) >> 1);
      /* CbCr */
      //seek += size_lum;
      for (i = 0; i < h; i++)
      {
        if (file_read(tb->in, cb, seek, src_width*2)) return NOK;
        seek += src_width*2;
        cb += stride*2;
      }
    }
    else if(inputFormat == HEVCENC_YUV420_PLANAR_10BIT_PACKED_PLANAR)
    {
    
      /* Lum */
      for (i = 0; i < src_height; i++)
      {
        if (file_read(tb->in, lum , seek, src_width*10/8)) return NOK;
        seek += src_width*10/8;
        lum += stride*10/8;
      }
      w = ((src_width + 1) >> 1);
      h = ((src_height + 1) >> 1);
      /* Cb */
      //seek += size_lum;
      for (i = 0; i < h; i++)
      {
        if (file_read(tb->in, cb, seek, w*10/8)) return NOK;
        seek += w*10/8;
        cb += stride*5/8;
      }

      /* Cr */
      //seek += size_ch;
      for (i = 0; i < h; i++)
      {
        if (file_read(tb->in, cr, seek, w*10/8)) return NOK;

        seek += w*10/8;
        cr += stride*5/8;
      }
    }
    else if(inputFormat == HEVCENC_YUV420_10BIT_PACKED_Y0L2)
    {
      for (i = 0; i < src_height/2; i++)
      {
      
        if (file_read(tb->in, lum , seek, src_width * 2*2)) return NOK;
        seek += src_width * 2*2;
        lum += stride * 2*2;
      }
    }
  }
  

  return OK;
}

/*------------------------------------------------------------------------------
  file_read
------------------------------------------------------------------------------*/
i32 file_read(FILE *file, u8 *data, u64 seek, size_t size)
{
  if ((file == NULL) || (data == NULL)) return NOK;

  fseeko(file, seek, SEEK_SET);
  if (fread(data, sizeof(u8), size, file) < size)
  {
    if (!feof(file))
    {
      Error(2, ERR, SYSERR);
    }
    return NOK;
  }

  return OK;
}

/*------------------------------------------------------------------------------
  next_picture calculates next input picture depending input and output
  frame rates.
------------------------------------------------------------------------------*/
u64 next_picture(struct test_bench *tb, int picture_cnt)
{
  u64 numer, denom;

  numer = (u64)tb->inputRateNumer * (u64)tb->outputRateDenom;
  denom = (u64)tb->inputRateDenom * (u64)tb->outputRateNumer;

  return numer * (picture_cnt / (1 << tb->interlacedFrame)) / denom;
}

/*------------------------------------------------------------------------------
  change_input
------------------------------------------------------------------------------*/
i32 change_input(struct test_bench *tb)
{
  struct parameter prm;
  i32 enable = false;
  i32 ret;

  prm.cnt = 1;
  while ((ret = get_option(tb->argc, tb->argv, options, &prm)) != -1)
  {
    if ((ret == 1) && enable)
    {
      tb->input = prm.argument;
      printf("Next input file %s\n", tb->input);
      return OK;
    }
    if (prm.argument == tb->input)
    {
      enable = true;
    }
  }

  return NOK;
}

/*------------------------------------------------------------------------------
  open_file
------------------------------------------------------------------------------*/
FILE *open_file(char *name, char *mode)
{
  FILE *fp;

  if (!(fp = fopen(name, mode)))
  {
    Error(4, ERR, name, ", ", SYSERR);
  }

  return fp;
}

/*------------------------------------------------------------------------------
    GetResolution
        Parse image resolution from file name
------------------------------------------------------------------------------*/
u32 GetResolution(char *filename, i32 *pWidth, i32 *pHeight)
{
  i32 i;
  u32 w, h;
  i32 len = strlen(filename);
  i32 filenameBegin = 0;

  /* Find last '/' in the file name, it marks the beginning of file name */
  for (i = len - 1; i; --i)
    if (filename[i] == '/')
    {
      filenameBegin = i + 1;
      break;
    }

  /* If '/' found, it separates trailing path from file name */
  for (i = filenameBegin; i <= len - 3; ++i)
  {
    if ((strncmp(filename + i, "subqcif", 7) == 0) ||
        (strncmp(filename + i, "sqcif", 5) == 0))
    {
      *pWidth = 128;
      *pHeight = 96;
      printf("Detected resolution SubQCIF (128x96) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "qcif", 4) == 0)
    {
      *pWidth = 176;
      *pHeight = 144;
      printf("Detected resolution QCIF (176x144) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "4cif", 4) == 0)
    {
      *pWidth = 704;
      *pHeight = 576;
      printf("Detected resolution 4CIF (704x576) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "cif", 3) == 0)
    {
      *pWidth = 352;
      *pHeight = 288;
      printf("Detected resolution CIF (352x288) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "qqvga", 5) == 0)
    {
      *pWidth = 160;
      *pHeight = 120;
      printf("Detected resolution QQVGA (160x120) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "qvga", 4) == 0)
    {
      *pWidth = 320;
      *pHeight = 240;
      printf("Detected resolution QVGA (320x240) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "vga", 3) == 0)
    {
      *pWidth = 640;
      *pHeight = 480;
      printf("Detected resolution VGA (640x480) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "720p", 4) == 0)
    {
      *pWidth = 1280;
      *pHeight = 720;
      printf("Detected resolution 720p (1280x720) from file name.\n");
      return 0;
    }
    if (strncmp(filename + i, "1080p", 5) == 0)
    {
      *pWidth = 1920;
      *pHeight = 1080;
      printf("Detected resolution 1080p (1920x1080) from file name.\n");
      return 0;
    }
    if (filename[i] == 'x')
    {
      if (sscanf(filename + i - 4, "%ux%u", &w, &h) == 2)
      {
        *pWidth = w;
        *pHeight = h;
        printf("Detected resolution %dx%d from file name.\n", w, h);
        return 0;
      }
      else if (sscanf(filename + i - 3, "%ux%u", &w, &h) == 2)
      {
        *pWidth = w;
        *pHeight = h;
        printf("Detected resolution %dx%d from file name.\n", w, h);
        return 0;
      }
      else if (sscanf(filename + i - 2, "%ux%u", &w, &h) == 2)
      {
        *pWidth = w;
        *pHeight = h;
        printf("Detected resolution %dx%d from file name.\n", w, h);
        return 0;
      }
    }
    if (filename[i] == 'w')
    {
      if (sscanf(filename + i, "w%uh%u", &w, &h) == 2)
      {
        *pWidth = w;
        *pHeight = h;
        printf("Detected resolution %dx%d from file name.\n", w, h);
        return 0;
      }
    }
  }

  return 1;   /* Error - no resolution found */
}
/*------------------------------------------------------------------------------

    AllocRes

    Allocation of the physical memories used by both SW and HW:
    the input pictures and the output stream buffer.

    NOTE! The implementation uses the EWL instance from the encoder
          for OS independence. This is not recommended in final environment
          because the encoder will release the EWL instance in case of error.
          Instead, the memories should be allocated from the OS the same way
          as inside EWLMallocLinear().

------------------------------------------------------------------------------*/
int AllocRes(commandLine_s *cmdl, HEVCEncInst enc, struct test_bench *tb)
{
  i32 ret;
  u32 pictureSize;
  u32 outbufSize;
  u32 block_size;
  u32 block_unit;

  pictureSize = ((cmdl->lumWidthSrc + 16 - 1) & (~(16 - 1))) * cmdl->lumHeightSrc *
                HEVCEncGetBitsPerPixel(cmdl->inputFormat) / 8;

  tb->pictureMem.virtualAddress = NULL;
  tb->outbufMem.virtualAddress = NULL;
  tb->pictureStabMem.virtualAddress = NULL;
  
  tb->roiMapDeltaQpMem.virtualAddress = NULL;

  /* Here we use the EWL instance directly from the encoder
   * because it is the easiest way to allocate the linear memories */
  ret = EWLMallocLinear(((struct hevc_instance *)enc)->asic.ewl, pictureSize,
                        &tb->pictureMem);
  if (ret != EWL_OK)
  {
    tb->pictureMem.virtualAddress = NULL;
    return 1;
  }

  /* Limited amount of memory on some test environment */
  if (cmdl->outBufSizeMax == 0) cmdl->outBufSizeMax = 12;
  outbufSize = 4 * tb->pictureMem.size < ((u32)cmdl->outBufSizeMax * 1024 * 1024) ?
               4 * tb->pictureMem.size : ((u32)cmdl->outBufSizeMax * 1024 * 1024);

  ret = EWLMallocLinear(((struct hevc_instance *)enc)->asic.ewl, outbufSize,
                        &tb->outbufMem);
  if (ret != EWL_OK)
  {
    tb->outbufMem.virtualAddress = NULL;
    return 1;
  }
  //allocate delta qp map memory.
  switch(cmdl->roiMapDeltaQpBlockUnit)
  {
    case 0:
      block_unit=64;
      break;
    case 1:
      block_unit=32;
      break;
    case 2:
      block_unit=16;
      break;
    case 3:
      block_unit=8;
      break;
    default:
      block_unit=64;
      break;
  }
  // 4 bits per block.
  block_size=((cmdl->width+64-1)& (~(64 - 1)))*((cmdl->height+64-1)& (~(64 - 1)))/(8*8*2);
  if (EWLMallocLinear(((struct hevc_instance *)enc)->asic.ewl, block_size, &tb->roiMapDeltaQpMem) != EWL_OK)
  {
    tb->roiMapDeltaQpMem.virtualAddress = NULL;
    return 1;
  }
  printf("Input buffer size:          %d bytes\n", tb->pictureMem.size);
  printf("Input buffer bus address:   0x%08x\n", tb->pictureMem.busAddress);
  printf("Input buffer user address:  0x%p\n", tb->pictureMem.virtualAddress);
  printf("Output buffer size:         %d bytes\n", tb->outbufMem.size);
  printf("Output buffer bus address:  0x%08x\n", tb->outbufMem.busAddress);
  printf("Output buffer user address: 0x%p\n",  tb->outbufMem.virtualAddress);

  return 0;
}

/*------------------------------------------------------------------------------

    FreeRes

    Release all resources allcoated byt AllocRes()

------------------------------------------------------------------------------*/
void FreeRes(HEVCEncInst enc, struct test_bench *tb)
{
  if (tb->pictureMem.virtualAddress != NULL)
    EWLFreeLinear(((struct hevc_instance *)enc)->asic.ewl, &tb->pictureMem);
  if (tb->pictureStabMem.virtualAddress != NULL)
    EWLFreeLinear(((struct hevc_instance *)enc)->asic.ewl, &tb->pictureStabMem);
  if (tb->outbufMem.virtualAddress != NULL)
    EWLFreeLinear(((struct hevc_instance *)enc)->asic.ewl, &tb->outbufMem);
    
  if (tb->roiMapDeltaQpMem.virtualAddress != NULL)
    EWLFreeLinear(((struct hevc_instance *)enc)->asic.ewl, &tb->roiMapDeltaQpMem);
}
/*------------------------------------------------------------------------------
    Add new frame bits for moving average bitrate calculation
------------------------------------------------------------------------------*/
void MaAddFrame(ma_s *ma, i32 frameSizeBits)
{
    ma->frame[ma->pos++] = frameSizeBits;

    if (ma->pos == ma->length)
        ma->pos = 0;

    if (ma->count < ma->length)
        ma->count++;
}

/*------------------------------------------------------------------------------
    Calculate average bitrate of moving window
------------------------------------------------------------------------------*/
i32 Ma(ma_s *ma)
{
    i32 i;
    unsigned long long sum = 0;     /* Using 64-bits to avoid overflow */

    for (i = 0; i < ma->count; i++)
        sum += ma->frame[i];

    if (!ma->frameRateDenom)
        return 0;

    sum = sum / ma->count;

    return sum * (ma->frameRateNumer+ma->frameRateDenom-1) / ma->frameRateDenom;
}

/*    Callback function called by the encoder SW after "slice ready"
    interrupt from HW. Note that this function is not necessarily called
    after every slice i.e. it is possible that two or more slices are
    completed between callbacks. 
------------------------------------------------------------------------------*/
void HEVCSliceReady(HEVCEncSliceReady *slice)
{
  u32 i;
  u32 streamSize;
  u8 *strmPtr;
  SliceCtl_s *ctl = (SliceCtl_s*)slice->pAppData;
  /* Here is possible to implement low-latency streaming by
   * sending the complete slices before the whole frame is completed. */
   
  if(ctl->multislice_encoding&&(ENCH2_SLICE_READY_INTERRUPT))
  {
    if (slice->slicesReadyPrev == 0)    /* New frame */
    {
      
        strmPtr = (u8 *)slice->pOutBuf; /* Pointer to beginning of frame */
        streamSize=0;
        for(i=0;i<slice->nalUnitInfoNum+slice->slicesReady;i++)
        {
          streamSize+=*(slice->sliceSizes+i);
        }
        if (ctl->output_byte_stream == 0)
        {
          WriteNals(ctl->outStreamFile, (u32*)strmPtr, slice->sliceSizes,
                    slice->nalUnitInfoNum+slice->slicesReady, 0);
        }
        else
        {

          WriteStrm(ctl->outStreamFile, (u32*)strmPtr, streamSize, 0);
        }

        
      
    }
    else
    {
      strmPtr = (u8 *)ctl->strmPtr; /* Here we store the slice pointer */
      streamSize=0;
      for(i=(slice->nalUnitInfoNum+slice->slicesReadyPrev);i<slice->slicesReady+slice->nalUnitInfoNum;i++)
      {
        streamSize+=*(slice->sliceSizes+i);
      }
      if (ctl->output_byte_stream == 0)
      {
        WriteNals(ctl->outStreamFile, (u32*)strmPtr, slice->sliceSizes+slice->nalUnitInfoNum+slice->slicesReadyPrev,
                  slice->slicesReady-slice->slicesReadyPrev, 0);
      }
      else
      {
        WriteStrm(ctl->outStreamFile, (u32*)strmPtr, streamSize, 0);
      }      
    }
    strmPtr+=streamSize;
    /* Store the slice pointer for next callback */
    ctl->strmPtr = strmPtr;
  }
}

/*------------------------------------------------------------------------------

    WriteStrm
        Write encoded stream to file

    Params:
        fout    - file to write
        strbuf  - data to be written
        size    - amount of data to write
        endian  - data endianess, big or little

------------------------------------------------------------------------------*/
void WriteNals(FILE *fout, u32 *strmbuf, const u32 *pNaluSizeBuf, u32 numNalus, u32 endian)
{
  const u8 start_code_prefix[4] = {0x0, 0x0, 0x0, 0x1};
  u32 count = 0;
  u32 offset = 0;
  u32 size;
  u8  *stream;

#ifdef NO_OUTPUT_WRITE
  return;
#endif

  ASSERT(endian == 0); //FIXME: not support abnormal endian now.

  stream = (u8 *)strmbuf;
  while (count++ < numNalus && *pNaluSizeBuf != 0)
  {
    fwrite(start_code_prefix, 1, 4, fout);
    size = *pNaluSizeBuf++;
    fwrite(stream + offset, 1, size, fout);
    offset += size;
  }

}

/*------------------------------------------------------------------------------

    WriteStrm
        Write encoded stream to file

    Params:
        fout    - file to write
        strbuf  - data to be written
        size    - amount of data to write
        endian  - data endianess, big or little

------------------------------------------------------------------------------*/
void WriteStrm(FILE *fout, u32 *strmbuf, u32 size, u32 endian)
{

#ifdef NO_OUTPUT_WRITE
  return;
#endif

  if (!fout || !strmbuf || !size) return;

  /* Swap the stream endianess before writing to file if needed */
  if (endian == 1)
  {
    u32 i = 0, words = (size + 3) / 4;

    while (words)
    {
      u32 val = strmbuf[i];
      u32 tmp = 0;

      tmp |= (val & 0xFF) << 24;
      tmp |= (val & 0xFF00) << 8;
      tmp |= (val & 0xFF0000) >> 8;
      tmp |= (val & 0xFF000000) >> 24;
      strmbuf[i] = tmp;
      words--;
      i++;
    }

  }
  printf("Write out stream size %d \n", size);
  /* Write the stream to file */
  fwrite(strmbuf, 1, size, fout);
}
#if 0
/*------------------------------------------------------------------------------
    WriteNalSizesToFile
        Dump NAL size to a file

    Params:
        file         - file name where toi dump
        pNaluSizeBuf - buffer where the individual NAL size are (return by API)
        numNalus     - amount of NAL units in the above buffer
------------------------------------------------------------------------------*/
void WriteNalSizesToFile(FILE *file, const u32 *pNaluSizeBuf,
                         u32 numNalus)
{
  u32 offset = 0;


  while (offset++ < numNalus && *pNaluSizeBuf != 0)
  {
    fprintf(file, "%d\n", *pNaluSizeBuf++);
  }


}
#endif
/*------------------------------------------------------------------------------

    WriteScaled
        Write scaled image to file "scaled_wxh.yuyv", support interleaved yuv422 only.

    Params:
        strbuf  - data to be written
        inst    - encoder instance

------------------------------------------------------------------------------*/
void WriteScaled(u32 *strmbuf, HEVCEncInst inst)
{

#ifdef NO_OUTPUT_WRITE
  return;
#endif

  struct hevc_instance *hevc_instance = (struct hevc_instance *)inst;
  if (!strmbuf || !hevc_instance)
    return;

  u32 width = hevc_instance->preProcess.scaledWidth;
  u32 height = hevc_instance->preProcess.scaledHeight;
  u32 size =  width * height * 2;
  if (!size)
    return;

  char fname[100];
  sprintf(fname, "scaled_%dx%d.yuyv", width, height);

  FILE *fout = fopen(fname, "ab+");
  if (fout == NULL)
    return;

  fwrite(strmbuf, 1, size, fout);

  fclose(fout);
}
/*------------------------------------------------------------------------------

    API tracing
        TRacing as defined by the API.
    Params:
        msg - null terminated tracing message
------------------------------------------------------------------------------*/
void HEVCEncTrace(const char *msg)
{
    static FILE *fp = NULL;

    if(fp == NULL)
        fp = fopen("api.trc", "wt");

    if(fp)
        fprintf(fp, "%s\n", msg);
}


