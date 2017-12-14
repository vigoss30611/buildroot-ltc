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



struct test_bench
{
  char *input;
  char *output;
  char *test_data_files;
  FILE *in;
  FILE *out;
  i32 width;
  i32 height;
  i32 ofr_num;      /* Output frame rate numerator */
  i32 ofr_den;      /* Output frame rate denominator */
  i32 ifr_num;      /* Input frame rate numerator */
  i32 ifr_den;      /* Input frame rate denominator */
  i32 first_picture;
  i32 last_picture;
  i32 picture_cnt;
  i32 idr_interval;
  i32 byteStream;
  u8 *lum;
  u8 *cb;
  u8 *cr;
  i32 interlacedFrame;
  u32 validencodedframenumber;
  u32 GDR_interval;

  char **argv;      /* Command line parameter... */
  i32 argc;
};

static struct option options[] =
{
  {"help",    'H', 2},
  {"first_picture", 'a', 1},
  {"last_picture",  'b', 1},
  {"width", 'x', 1},
  {"height", 'y', 1},
  {"lumWidthSrc", 'w', 1},
  {"lumHeightSrc", 'h', 1},
  {"horOffsetSrc", 'X', 1},
  {"verOffsetSrc", 'Y', 1},
  {"inputFormat", 'l', 1},        /* Input image format */
  {"colorConversion", 'O', 1},    /* RGB to YCbCr conversion type */
  {"videoStab", 'Z', 1},          /* video stabilization */
  {"rotation", 'r', 1},           /* Input image rotation */
  {"output_frame_rate", 'f', 1},
  {"input_frame_rate",  'F', 1},
  {"input",   'i', 1},
  {"output",    'o', 1},
  {"test_data_files", 'T', 1},
  {"cu_size",   'S', 1},
  {"tr_size",   't', 1},
  {"tr_depth",    'd', 1},
  {"cabacInitFlag", 'p', 1},
  {"qp_size",   'Q', 1},
  {"qpMin", 'n', 1},              /* Minimum frame header qp */
  {"qpMax", 'm', 1},              /* Maximum frame header qp */
  {"qpHdr",     'q', 1},
  {"hrdConformance", 'C', 1},     /* HDR Conformance (ANNEX C) */
  {"cpbSize", 'c', 1},            /* Coded Picture Buffer Size */
  {"intraQpDelta", 'A', 1},       /* QP adjustment for intra frames */
  {"fixedIntraQp", 'G', 1},       /* Fixed QP for all intra frames */
  {"byteStream", 'N', 1},         /* Byte stream format (ANNEX B) */
  {"bitPerSecond", 'B', 1},
  {"picRc",   'U', 1},
  {"ctbRc",   'u', 1},
  {"picSkip", 's', 1},            /* Frame skipping */
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




  /* Only long option can be used for all the following parameters because
   * we have no more letters to use. All shortOpt=0 will be identified by
   * long option. */
  {"cir", '0', 1},
  {"intraArea", '0', 1},
  {"roi1Area", '0', 1},
  {"roi2Area", '0', 1},
  {"roi1DeltaQp", '0', 1},
  {"roi2DeltaQp", '0', 1},
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

  {"videoRange", 'k', 1},
  {"interlacedFrame", '0', 1},
  {"fieldOrder", '0', 1},
  {"outReconFrame", '0', 1},
  
  {"enableGDR", '0', 1},
  {"dynamicenableGDR", '0', 1},
    
  {"sei", 'J', 1},                /* SEI messages */

  {"userData", 'z', 1},           /* SEI User data file */

  {NULL,      0,   0}        /* Format of last line */
};


static FILE *yuvFile = NULL;
//static FILE *nalfile = NULL;

/* SW/HW shared memories for input/output buffers */
static EWLLinearMem_t pictureMem;
static EWLLinearMem_t pictureStabMem;
static EWLLinearMem_t outbufMem;
static i32 outBufSizeMax;               /* outbufMem max in bytes */
static EWLLinearMem_t scaledPictureMem;
static u32 multislice_encoding;
static u32 output_byte_stream;
static FILE *outStreamFile;



//static char *yuvFileName = NULL;
//static FILE *yuvFileMvc = NULL;
//static off_t file_size;


static i32 encode(struct test_bench *tb, HEVCEncInst encoder, commandLine_s *cml);
static i32 Parameter_Get(i32 argc, char **argv, commandLine_s *cml);
static void default_parameter(commandLine_s *cml);
static void help(void);
static u64 next_picture(struct test_bench *tb);
static i32 read_picture(struct test_bench *tb, u32 inputFormat, u32 src_img_size, u32 src_width, u32 src_height);
static i32 change_input(struct test_bench *tb);
static FILE *open_file(char *name, char *mode);
static i32 read(FILE *file, u8 *data, u64 seek, size_t size);
static u32 GetResolution(char *filename, i32 *pWidth, i32 *pHeight);
static int OpenEncoder(commandLine_s *cml, HEVCEncInst *pEnc);
static void CloseEncoder(HEVCEncInst encoder);
static int AllocRes(commandLine_s *cmdl, HEVCEncInst enc);
static void FreeRes(HEVCEncInst enc);
static void HEVCSliceReady(HEVCEncSliceReady *slice);
static void WriteStrm(FILE *fout, u32 *strmbuf, u32 size, u32 endian);
static void WriteNals(FILE *fout, u32 *strmbuf, const u32 *pNaluSizeBuf, u32 numNalus, u32 endian);
#if 0
static void WriteNalSizesToFile(FILE *file, const u32 *pNaluSizeBuf,
                                u32 numNalus);
#endif
static u8 *ReadUserData(HEVCEncInst encoder, char *name);
static void WriteScaled(u32 *strmbuf, HEVCEncInst inst);
static i32 CheckArea(HEVCEncPictureArea *area, commandLine_s *cml);



/*------------------------------------------------------------------------------
  main
------------------------------------------------------------------------------*/
int main(i32 argc, char **argv)
{
  struct test_bench  tb;
  HEVCEncInst hevc_encoder;
  commandLine_s  cml;
  //HEVCEncConfig config;

  HEVCEncApiVersion apiVer;
  HEVCEncBuild encBuild;
  i32 ret = OK;

  apiVer = HEVCEncGetApiVersion();
  encBuild = HEVCEncGetBuild();

  fprintf(stderr, "HEVC Encoder API version %d.%d\n", apiVer.major,
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
    //help();
    fprintf(stdout, "%s width height input_file\n", argv[0]);
    
    exit(0);
  }

#if 0
  if (Parameter_Get(argc, argv, &cml))
  {
    Error(2, ERR, "Input parameter error");
    return NOK;
  }
#else
  cml.lumWidthSrc = atoi(argv[1]);
  cml.lumHeightSrc = atoi(argv[2]);
  cml.input = (argv[3]);
  cml.inputFormat = 0;
#endif

  yuvFile = fopen(cml.input, "rb");
  if (yuvFile == NULL)
  {
    fprintf(HEVCERR_OUTPUT, "Unable to open input file: %s\n", cml.input);
    return -1;
  }
  else
  {
    fclose(yuvFile);
    yuvFile = NULL;
  }


  memset(&tb, 0, sizeof(struct test_bench));
#if 0
  tb.argc = argc;
  tb.argv = argv;
#endif


  /* Encoder initialization */
  if ((ret = OpenEncoder(&cml, &hevc_encoder)) != 0)
  {
    printf("OpenEncoder error:%d\n", ret);
    return -ret;
  }
  tb.input = cml.input;
  tb.output     = cml.output;
  tb.first_picture    = cml.first_picture;
  tb.last_picture   = cml.last_picture;
  tb.ifr_num      = cml.ifr_num;
  tb.ifr_den      = cml.ifr_den;
  tb.ofr_num      = cml.ofr_num;
  tb.ofr_den      = cml.ofr_den;
  tb.test_data_files    = cml.test_data_files;
  tb.width      = cml.width;
  tb.height     = cml.height;

  tb.idr_interval   = cml.intraPicRate;
  tb.byteStream   = cml.byteStream;
  tb.interlacedFrame = cml.interlacedFrame;

  /* Set the test ID for internal testing,
   * the SW must be compiled with testing flags */
  HEVCEncSetTestId(hevc_encoder, cml.testId);

  /* Allocate input and output buffers */
  if (AllocRes(&cml, hevc_encoder) != 0)
  {
    FreeRes(hevc_encoder);
    CloseEncoder(hevc_encoder);
    return -HEVCENC_MEMORY_ERROR;
  }
#ifdef TEST_DATA
  Enc_test_data_init();
#endif


  ret = encode(&tb, hevc_encoder, &cml);

#ifdef TEST_DATA
  Enc_test_data_release();
#endif

  FreeRes(hevc_encoder);

  CloseEncoder(hevc_encoder);

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
  return (width + 15) & (~15);
#endif
}

/*------------------------------------------------------------------------------
  encode
------------------------------------------------------------------------------*/
i32 encode(struct test_bench *tb, HEVCEncInst encoder, commandLine_s *cml)
{
  printf("%s enter\n", __FUNCTION__);
  HEVCEncIn encIn;
  HEVCEncOut encOut;
  HEVCEncRet ret;
  HEVCEncCodingCtrl codingCfg;
  u32 frameCntTotal = 0;
  u32 streamSize = 0;
  u32 ctb_rows;
  i32 cnt = 1;
  u32 i;
  i32 p;
  u32 total_bits = 0;
  u32 w = get_aligned_width(cml->lumWidthSrc, cml->inputFormat); //(cml->lumWidthSrc + 15) & (~15);
  u32 h = cml->lumHeightSrc;
  u8 *pUserData;
  u32 src_img_size;
  HEVCEncRateCtrl rc;
  u32 left_pos,right_pos,top_pos,bottom_pos;

  /* Output/input streams */
  printf("%s open %s\n", __FUNCTION__, tb->input);
  if (!(tb->in = open_file(tb->input, "rb"))) goto error;
  if (!(tb->out = open_file(tb->output, "wb"))) goto error;

  encIn.timeIncrement = 0;
  encIn.busOutBuf = outbufMem.busAddress;
  encIn.outBufSize = outbufMem.size;
  encIn.pOutBuf = outbufMem.virtualAddress;
  multislice_encoding=(cml->sliceSize!=0&&(((cml->height+63)/64)>cml->sliceSize))?1:0;
  output_byte_stream=cml->byteStream?1:0;
  outStreamFile=tb->out;
  /* Video, sequence and picture parameter sets */
  for (p = 0; p < cnt; p++)
  {

    printf("%s -> HEVCEncStrmStart\n", __FUNCTION__);
    if (HEVCEncStrmStart(encoder, &encIn, &encOut))
    {
      Error(2, ERR, "hevc_set_parameter() fails");
      goto error;
    }
    if (cml->byteStream == 0)
    {
      //WriteNalSizesToFile(nalfile, encOut.pNaluSizeBuf,
      //                    encOut.numNalus);
      WriteNals(tb->out, outbufMem.virtualAddress, encOut.pNaluSizeBuf,
                encOut.numNalus, 0);
    }
    else
    {
      WriteStrm(tb->out, outbufMem.virtualAddress, encOut.streamSize, 0);
    }
    total_bits += encOut.streamSize * 8;
    streamSize += encOut.streamSize;
  }



  encIn.poc = 0;

  HEVCEncGetRateCtrl(encoder, &rc);


  /* Allocate a buffer for user data and read data from file */
  pUserData = ReadUserData(encoder, cml->userData);
  /* Setup encoder input */
  {

    u32 size_lum = w * h;
    u32 size_ch  = ((w + 1) >> 1) * ((h + 1) >> 1);
    encIn.busLuma = pictureMem.busAddress;
    tb->lum = (u8 *)pictureMem.virtualAddress;

    encIn.busChromaU = encIn.busLuma + size_lum;
    tb->cb = tb->lum + size_lum;
    encIn.busChromaV = encIn.busChromaU + size_ch;
    tb->cr = tb->cb + size_ch;
  }

  src_img_size = cml->lumWidthSrc * cml->lumHeightSrc *
                 HEVCEncGetBitsPerPixel(cml->inputFormat) / 8;
  printf("Reading input from file <%s>,\n frame size %d bytes.\n",
         cml->input, src_img_size);
  tb->validencodedframenumber=0;
  ctb_rows=(cml->height+63)/64;
  tb->GDR_interval=ctb_rows-1;
  if(tb->idr_interval>tb->GDR_interval)
    tb->GDR_interval=tb->idr_interval;
  while (true)
  {
    if (read_picture(tb, cml->inputFormat, src_img_size, cml->lumWidthSrc, cml->lumHeightSrc))
    {
      /* Next input stream (if any) */
      if (change_input(tb)) break;
      fclose(tb->in);
      if (!(tb->in = open_file(tb->input, "rb"))) goto error;
      tb->picture_cnt = 0;
      tb->validencodedframenumber=0;
      continue;
    }
    frameCntTotal++;
    //for GDR test
    if(cml->enableGDR==1)
    {
        printf("%s cml->enableGDR\n", __FUNCTION__);
      //only the first frame is intra frame. All the other frames are predicted frames.
      encIn.codingType = (tb->validencodedframenumber == 0) ? HEVCENC_INTRA_FRAME : HEVCENC_PREDICTED_FRAME;
      if(tb->validencodedframenumber>=tb->GDR_interval)
      {
        //GDR from the second GOP.
        
        if ((ret = HEVCEncGetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
        {
          break;
        }
        
        //encoded 2 ctb rows intra block per frame.
        left_pos=0;
        right_pos=((cml->width+63)/64)-1;

        top_pos=tb->validencodedframenumber%tb->GDR_interval;
        if(top_pos>ctb_rows-1)
        {
          top_pos=255;
          bottom_pos=255;
        }
        else
        {
          bottom_pos=top_pos+1;
        }
        
        codingCfg.intraArea.top = top_pos;
        codingCfg.intraArea.left = left_pos;
        codingCfg.intraArea.bottom = bottom_pos;
        codingCfg.intraArea.right = right_pos;
        codingCfg.intraArea.enable = CheckArea(&codingCfg.intraArea, cml);
        if((tb->validencodedframenumber%tb->GDR_interval)==0)
        {
          codingCfg.insertrecoverypointmessage=1;
          codingCfg.recoverypointpoc=ctb_rows-2;
        }
        else
        {
          codingCfg.insertrecoverypointmessage=0;
          codingCfg.recoverypointpoc=0;
        }
        
        if ((ret = HEVCEncSetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
        {
          break;
        }
      }
  
    }
    else if(cml->dynamicenableGDR==1)
    {
        printf("%s dynamicenableGDR==1\n", __FUNCTION__);
      //we demo one GOP GDR is enabled and the next GOP is disabled repeately.
            //only the first frame is intra frame. All the other frames are predicted frames.
      encIn.codingType = (tb->validencodedframenumber%(2*tb->GDR_interval) == 0) ? HEVCENC_INTRA_FRAME : HEVCENC_PREDICTED_FRAME;
      if((tb->validencodedframenumber%(2*tb->GDR_interval))>=tb->GDR_interval)
      {
        //GDR from the second GOP.
        
        if ((ret = HEVCEncGetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
        {
          break;
        }
        
        //encoded 2 ctb rows intra block per frame.
        left_pos=0;
        right_pos=((cml->width+63)/64)-1;

        top_pos=tb->validencodedframenumber%tb->GDR_interval;
        if(top_pos>ctb_rows-1)
        {
          top_pos=255;
          bottom_pos=255;
        }
        else
        {
          bottom_pos=top_pos+1;
        }
        
        codingCfg.intraArea.top = top_pos;
        codingCfg.intraArea.left = left_pos;
        codingCfg.intraArea.bottom = bottom_pos;
        codingCfg.intraArea.right = right_pos;
        codingCfg.intraArea.enable = CheckArea(&codingCfg.intraArea, cml);
        
        if((tb->validencodedframenumber%tb->GDR_interval)==0)
        {
          codingCfg.insertrecoverypointmessage=1;
          codingCfg.recoverypointpoc=ctb_rows-2;
        }
        else
        {
          codingCfg.insertrecoverypointmessage=0;
          codingCfg.recoverypointpoc=0;
        }

        
        if ((ret = HEVCEncSetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
        {
          break;
        }
      }
      else
      {
        if ((ret = HEVCEncGetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
        {
          break;
        }
        codingCfg.insertrecoverypointmessage=0;
        codingCfg.recoverypointpoc=0;
        if ((ret = HEVCEncSetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
        {
          break;
        }
      }
      
    }
    else
    {
        printf("%s dynamicenableGDR==0 enableGDR==0\n", __FUNCTION__);
      if (tb->idr_interval == 0)
      {
        encIn.codingType = (encIn.poc == 0) ? HEVCENC_INTRA_FRAME : HEVCENC_PREDICTED_FRAME;
      }
      else
      {
        encIn.codingType = ((encIn.poc % tb->idr_interval) == 0) ? HEVCENC_INTRA_FRAME : HEVCENC_PREDICTED_FRAME;
      }
    }
    printf("Encoding %i %s codeType =%d\n", tb->picture_cnt, tb->input, encIn.codingType);
    //select rps based on frame type
    if (encIn.codingType == HEVCENC_INTRA_FRAME)
    {
      //refrest IDR poc 
      encIn.poc = 0;
      //encIn.rps_id = 1;
    }
    else
    { 
      //encIn.rps_id = 0;
    }

        printf("%s MAX_BPS_ADJUST:%d\n", __FUNCTION__, MAX_BPS_ADJUST);
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

    struct timeval startTime, endTime;
    gettimeofday(&startTime, 0);
    ret = HEVCEncStrmEncode(encoder, &encIn, &encOut, &HEVCSliceReady, NULL);
    gettimeofday(&endTime, 0);
    int diff = (endTime.tv_sec-startTime.tv_sec)*1000 + (endTime.tv_usec-startTime.tv_usec)/1000;
    printf("%s HEVCEncStrmEncode:%d, time:%dms\n", __FUNCTION__, ret, diff);
    switch (ret)
    {
      case HEVCENC_FRAME_READY:

        tb->picture_cnt++;
        if (encOut.streamSize == 0)
          break;
#ifdef TEST_DATA
        //EncTraceWriteProfile(encoder, encIn.poc);
        //write_recon_sw(encoder, encIn.poc);
        EncTraceUpdateStatus();
#endif
#ifdef TEST_DATA
        if(cml->outReconFrame==1)
          EncTraceRecon(encoder, encIn.poc);
#endif //TRACE_RECON

        HEVCEncGetRateCtrl(encoder, &rc);
        /* Write scaled encoder picture to file, packed yuyv 4:2:2 format */
        WriteScaled((u32 *)encOut.scaledPicture, encoder);
        if (encOut.streamSize != 0)
        {
          if((multislice_encoding==0)||(ENCH2_SLICE_READY_INTERRUPT==0)||(cml->hrdConformance==1))
          {
            if (cml->byteStream == 0)
            {
              //WriteNalSizesToFile(nalfile, encOut.pNaluSizeBuf,
              //            encOut.numNalus);
              WriteNals(tb->out, outbufMem.virtualAddress, encOut.pNaluSizeBuf,
                        encOut.numNalus, 0);
            }
            else
            {
              WriteStrm(tb->out, outbufMem.virtualAddress, encOut.streamSize, 0);
            }
          }
          encIn.timeIncrement = tb->ofr_den;

          total_bits += encOut.streamSize * 8;
          streamSize += encOut.streamSize;

          encIn.poc++;
          tb->validencodedframenumber++;
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
        printf("Encoded %d, bits=%d, average bit rate=%lld\n", tb->picture_cnt-1, total_bits, ((unsigned long long)total_bits * tb->ofr_num) / (tb->picture_cnt * tb->ofr_den));
        break;
      case HEVCENC_OUTPUT_BUFFER_OVERFLOW:
        printf("HEVCEncStrmEncode HEVCENC_OUTPUT_BUFFER_OVERFLOW:\n");
        tb->picture_cnt++;
        break;
      default:
        printf("HEVCEncStrmEncode ret:%d\n", ret);
        goto error;
        break;
    }
  }
  /* End stream */
#ifdef TEST_DATA  
  EncTraceReconEnd();
#endif
  ret = HEVCEncStrmEnd(encoder, &encIn, &encOut);
  if (ret == HEVCENC_OK)
  {
    if (cml->byteStream == 0)
    {
      //WriteNalSizesToFile(nalfile, encOut.pNaluSizeBuf,encOut.numNalus);
      WriteNals(tb->out, outbufMem.virtualAddress, encOut.pNaluSizeBuf,
                encOut.numNalus, 0);
    }
    else
    {
      WriteStrm(tb->out, outbufMem.virtualAddress, encOut.streamSize, 0);
    }
    streamSize += encOut.streamSize;
  }

  printf("Total of %d frames processed, %d frames encoded, %d bytes.\n",
         frameCntTotal, tb->picture_cnt, streamSize);

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
int OpenEncoder(commandLine_s *cml, HEVCEncInst *pEnc)
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
  if (cml->ofr_num == DEFAULT)
  {
    cml->ofr_num = cml->ifr_num;
  }

  /* outputRateDenom */
  if (cml->ofr_den == DEFAULT)
  {
    cml->ofr_den = cml->ifr_den;
  }
  /*cfg.ctb_size = cml->max_cu_size;*/
  if (cml->rotation)
  {
    cfg.width = cml->height;
    cfg.height = cml->width;
#if 0
    cfg.scaledWidth = cml->scaledHeight;
    cfg.scaledHeight = cml->scaledWidth;
#endif
  }
  else
  {
    cfg.width = cml->width;
    cfg.height = cml->height;
#if 0
    cfg.scaledWidth = cml->scaledWidth;
    cfg.scaledHeight = cml->scaledHeight;
#endif
  }


  cfg.frameRateDenom = cml->ofr_den;
  cfg.frameRateNum = cml->ofr_num;
  cfg.sliceSize = cml->sliceSize;

  /* intra tools in sps and pps */
  cfg.constrainedIntraPrediction = cml->constrained_intra_pred_flag;
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

  cfg.refFrameAmount = 1 + cml->interlacedFrame;

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
    CloseEncoder(encoder);
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



    if (cml->videoRange != DEFAULT)
    {
      if (cml->videoRange == 0)
        codingCfg.videoFullRange = 1;
      else
        codingCfg.videoFullRange = 0;
    }
    
    if (cml->sei||cml->enableGDR||cml->dynamicenableGDR)
      codingCfg.seiMessages = 1;
    else
      codingCfg.seiMessages = 0;

    codingCfg.fieldOrder = cml->fieldOrder;

    codingCfg.cirStart = cml->cirStart;
    codingCfg.cirInterval = cml->cirInterval;
    codingCfg.intraArea.top = cml->intraAreaTop;
    codingCfg.intraArea.left = cml->intraAreaLeft;
    codingCfg.intraArea.bottom = cml->intraAreaBottom;
    codingCfg.intraArea.right = cml->intraAreaRight;
    codingCfg.intraArea.enable = CheckArea(&codingCfg.intraArea, cml);
    codingCfg.insertrecoverypointmessage=0;
    codingCfg.roi1Area.top = cml->roi1AreaTop;
    codingCfg.roi1Area.left = cml->roi1AreaLeft;
    codingCfg.roi1Area.bottom = cml->roi1AreaBottom;
    codingCfg.roi1Area.right = cml->roi1AreaRight;
    if (CheckArea(&codingCfg.roi1Area, cml) && cml->roi1DeltaQp)
      codingCfg.roi1Area.enable = 1;
    else
      codingCfg.roi1Area.enable = 0;
    
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

    codingCfg.max_cu_size = cml->max_cu_size;
    codingCfg.min_cu_size = cml->min_cu_size;
    codingCfg.max_tr_size = cml->max_tr_size;
    codingCfg.min_tr_size = cml->min_tr_size;
    codingCfg.tr_depth_intra = cml->tr_depth_intra;
    codingCfg.tr_depth_inter = cml->tr_depth_inter;

    codingCfg.enableScalingList = cml->enableScalingList;
    codingCfg.chroma_qp_offset= cml->chromaQpOffset;
    if ((ret = HEVCEncSetCodingCtrl(encoder, &codingCfg)) != HEVCENC_OK)
    {
      //PrintErrorValue("HEVCEncSetCodingCtrl() failed.", ret);
      CloseEncoder(encoder);
      return -1;
    }

  }
#endif


  /* Encoder setup: rate control */
  if ((ret = HEVCEncGetRateCtrl(encoder, &rcCfg)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncGetRateCtrl() failed.", ret);
    CloseEncoder(encoder);
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
    if (cml->qpMin != DEFAULT)
      rcCfg.qpMin = cml->qpMin;
    if (cml->qpMax != DEFAULT)
      rcCfg.qpMax = cml->qpMax;
    if (cml->picSkip != DEFAULT)
      rcCfg.pictureSkip = cml->picSkip;
    if (cml->picRc != DEFAULT)
      rcCfg.pictureRc = cml->picRc;
    if (cml->bitPerSecond != DEFAULT)
      rcCfg.bitPerSecond = cml->bitPerSecond;

    if (cml->hrdConformance != DEFAULT)
      rcCfg.hrd = cml->hrdConformance;

    if (cml->cpbSize != DEFAULT)
      rcCfg.hrdCpbSize = cml->cpbSize;

    if(cml->intraPicRate != 0)
      rcCfg.gopLen = MIN(cml->intraPicRate, MAX_GOP_LEN);

    if (cml->gopLength != DEFAULT)
      rcCfg.gopLen = cml->gopLength;

    if (cml->intraQpDelta != DEFAULT)
      rcCfg.intraQpDelta = cml->intraQpDelta;


    rcCfg.fixedIntraQp = cml->fixedIntraQp;



    printf("Set rate control: qp %2d [%2d, %2d] %8d bps  "
           "pic %d skip %d  hrd %d\n"
           "  cpbSize %d gopLen %d intraQpDelta %2d "
           "fixedIntraQp %2d ",
           rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
           rcCfg.pictureRc, rcCfg.pictureSkip, rcCfg.hrd,
           rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta,
           rcCfg.fixedIntraQp);

    if ((ret = HEVCEncSetRateCtrl(encoder, &rcCfg)) != HEVCENC_OK)
    {
      //PrintErrorValue("HEVCEncSetRateCtrl() failed.", ret);
      CloseEncoder(encoder);
      return -1;
    }
  }
#if 1

#if 1
  /* Optional scaled image output */
  if (cml->scaledWidth * cml->scaledHeight)
  {
    if (h2_EWLMallocRefFrm(((struct hevc_instance *)encoder)->asic.ewl, cml->scaledWidth * cml->scaledHeight * 2,
                        &scaledPictureMem) != EWL_OK)
    {
      scaledPictureMem.virtualAddress = NULL;
      scaledPictureMem.busAddress = 0;

    }
  }
#endif

  /* PreP setup */
  if ((ret = HEVCEncGetPreProcessing(encoder, &preProcCfg)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncGetPreProcessing() failed.", ret);
    CloseEncoder(encoder);
    return -1;
  }
  printf
  ("Get PreP: input %4dx%d : offset %4dx%d : format %d : rotation %d\n"
   " stab %d : cc %d : scaling %d\n",
   preProcCfg.origWidth, preProcCfg.origHeight, preProcCfg.xOffset,
   preProcCfg.yOffset, preProcCfg.inputType, preProcCfg.rotation,
   preProcCfg.videoStabilization, preProcCfg.colorConversion.type,
   preProcCfg.scaledOutput);

  preProcCfg.inputType = (HEVCEncPictureType)cml->inputFormat;
  preProcCfg.rotation = (HEVCEncPictureRotation)cml->rotation;
  preProcCfg.interlacedFrame = cml->interlacedFrame;

  preProcCfg.origWidth = cml->lumWidthSrc;
  preProcCfg.origHeight = cml->lumHeightSrc;
  if (cml->interlacedFrame) preProcCfg.origHeight /= 2;

  if (cml->horOffsetSrc != DEFAULT)
    preProcCfg.xOffset = cml->horOffsetSrc;
  if (cml->verOffsetSrc != DEFAULT)
    preProcCfg.yOffset = cml->verOffsetSrc;
  if (cml->videoStab != DEFAULT)
    preProcCfg.videoStabilization = cml->videoStab;
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

  if (cml->rotation)
  {
    preProcCfg.scaledWidth = cml->scaledHeight;
    preProcCfg.scaledHeight = cml->scaledWidth;
  }
  else
  {
    preProcCfg.scaledWidth = cml->scaledWidth;
    preProcCfg.scaledHeight = cml->scaledHeight;
  }
  preProcCfg.busAddressScaledBuff = scaledPictureMem.busAddress;
  preProcCfg.virtualAddressScaledBuff = scaledPictureMem.virtualAddress;
  preProcCfg.sizeScaledBuff = scaledPictureMem.size;

  printf
  ("Set PreP: input %4dx%d : offset %4dx%d : format %d : rotation %d\n"
   " stab %d : cc %d : scaling %d\n",
   preProcCfg.origWidth, preProcCfg.origHeight, preProcCfg.xOffset,
   preProcCfg.yOffset, preProcCfg.inputType, preProcCfg.rotation,
   preProcCfg.videoStabilization, preProcCfg.colorConversion.type,
   preProcCfg.scaledOutput);

  /* If HW doesn't support scaling there will be no scaled output. */
  if (!preProcCfg.scaledOutput) cml->scaledWidth = 0;

  if ((ret = HEVCEncSetPreProcessing(encoder, &preProcCfg)) != HEVCENC_OK)
  {
    //PrintErrorValue("HEVCEncSetPreProcessing() failed.", ret);
    CloseEncoder(encoder);
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
void CloseEncoder(HEVCEncInst encoder)
{
  HEVCEncRet ret;

  if (scaledPictureMem.virtualAddress != NULL)
    h2_EWLFreeLinear(((struct hevc_instance *)encoder)->asic.ewl, &scaledPictureMem);
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
i32 Parameter_Get(i32 argc, char **argv, commandLine_s *cml)
{
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
        cml->first_picture = atoi(p);
        break;
      case 'b':
        cml->last_picture = atoi(p);
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
      case 'Z':
        cml->videoStab = atoi(p);
        break;
      case 'f':
        if (HasDelim(p, ':'))
        {
          sscanf(p, "%i:%i\n", &cml->ofr_num, &cml->ofr_den);
        }
        else
        {
          sscanf(p, "%i/%i\n", &cml->ofr_num, &cml->ofr_den);
        }
        break;
      case 'F':
        if (HasDelim(p, ':'))
        {
          sscanf(p, "%i:%i\n", &cml->ifr_num, &cml->ifr_den);
        }
        else
        {
          sscanf(p, "%i/%i\n", &cml->ifr_num, &cml->ifr_den);
        }
        break;
      case 'S':
        sscanf(p, "%i:%i\n",
               &cml->max_cu_size,
               &cml->min_cu_size);
        break;
      case 't':
        sscanf(p, "%i:%i\n",
               &cml->max_tr_size,
               &cml->min_tr_size);
        break;
      case 'd':
        sscanf(p, "%i:%i\n",
               &cml->tr_depth_intra,
               &cml->tr_depth_inter);
        break;
      case 'Q':
        cml->min_qp_size = atoi(p);
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
        //cml->ctbRc = atoi(p);
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

      case 'J':
        cml->sei = atoi(p);
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

        if (strcmp(prm.longOpt, "outBufSizeMax") == 0)
          outBufSizeMax = atoi(p);

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
        
        if (strcmp(prm.longOpt, "constrainIntra") == 0)
        {
          /* Argument must be "xx:yy:XX:YY".
           * xx is left coordinate, replace first ':' with 0 */
          cml->constrained_intra_pred_flag = atoi(p);
          ASSERT(cml->constrained_intra_pred_flag < 2);
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
        
        if (strcmp(prm.longOpt, "outReconFrame") == 0)
          cml->outReconFrame = atoi(p);

        
        if (strcmp(prm.longOpt, "enableGDR") == 0)
          cml->enableGDR = atoi(p);

        
        if (strcmp(prm.longOpt, "dynamicenableGDR") == 0)
          cml->dynamicenableGDR = atoi(p);
          
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
  cml->input      = "input.yuv";
  cml->output     = "stream.hevc";
  cml->first_picture    = 0;
  cml->last_picture   = 100;
  cml->ifr_num      = 30;
  cml->ifr_den      = 1;
  cml->ofr_num      = DEFAULT;
  cml->ofr_den      = DEFAULT;
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

  cml->intraQpDelta = DEFAULT;

  cml->disableDeblocking = 0;

  cml->tc_Offset = -2;
  cml->beta_Offset = 5;

  cml->qpHdr = DEFAULT;
  cml->qpMin = DEFAULT;
  cml->qpMax = DEFAULT;
  cml->picRc = DEFAULT;
  //    cml->ctbRc = DEFAULT;
  cml->cpbSize = DEFAULT;
  cml->gopLength = DEFAULT;
  cml->fixedIntraQp = 0;
  cml->hrdConformance = 0;

  cml->videoStab = DEFAULT;
  cml->byteStream = 1;

  cml->chromaQpOffset = 0;


  cml->enableSao = 1;

  cml->strong_intra_smoothing_enabled_flag = 1;
  cml->constrained_intra_pred_flag = 0;

  cml->intraAreaLeft = cml->intraAreaRight = cml->intraAreaTop =
                         cml->intraAreaBottom = -1;  /* Disabled */

  cml->picSkip = 0;

  cml->sliceSize = 0;

  cml->cabacInitFlag = 0;
  cml->enableDeblockOverride = 0;
  cml->deblockOverride = 0;

  cml->enableScalingList = 0;

  cml->compressor = 0;
  cml->sei = 0;
  cml->videoRange = 0;
  cml->level = DEFAULT;
  cml->gopLength = DEFAULT;

  cml->outReconFrame=1;
  
  cml->enableGDR=0;
  cml->dynamicenableGDR=0;
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

    printf("%s fname :%s\n", __FUNCTION__, (name==NULL)?"null":name);
  if (name == NULL)
  {
    return NULL;
  }

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
          "  -a[n] --first_picture            First picture of input file to encode. [0]\n"
          "  -b[n] --last_picture             Last picture of input file to encode. [100]\n"
          "  -f[n:n] --ofr_num:--ofr_den  \n"
          "                                   ofr_num:1..1048575 Output picture rate numerator. [30]\n"
          "                                   ofr_den:1..1048575 Output picture rate denominator. [1]\n"
          "                                   Encoded frame rate will be\n"
          "                                   ofr_num/ofr_den fps\n"
          "  -F[n:n] --ifr_num:--ifr_den  \n"
          "                                   ifr_num:1..1048575 input picture rate numerator. [30]\n"
          "                                   ifr_den:1..1048575 input picture rate denominator. [1]\n"
          "                                   input frame rate will be\n"
          "                                   ifr_num/ifr_den fps\n");

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

          "  -O[n] --colorConversion          RGB to YCbCr color conversion type. [0]\n"
          "                                   0 - BT.601\n"
          "                                   1 - BT.709\n"
          "                                   2 - User defined, coeffs defined in testbench.\n"
         );


  fprintf(stdout,

          "  -r[n] --rotation                 Rotate input image. [0]\n"
          "                                   0 - disabled\n"
          "                                   1 - 90 degrees right\n"
          "                                   2 - 90 degrees left\n"
         );


  fprintf(stdout,
          " Parameters affecting the output stream and encoding tools:\n"
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

          "  -N[n] --byteStream               Stream type. [1]\n"
          "                                   0 - NAL units. Nal sizes returned in <nal_sizes.txt>\n"
          "                                   1 - byte stream according to Hevc Standard Annex B.\n"

          "  -k[n] --videoRange               0..1 Video signal sample range value in Hevc stream. [0]\n"
          "                                   0: full range\n"
         );

  fprintf(stdout,
          "  -J[n] --sei                      Enable SEI messages (buffering period + picture timing) [0]\n"
          "                                   Writes Supplemental Enhancement Information messages\n"
          "                                   containing information about picture time stamps\n"
          "                                   and picture buffering into stream.\n");


#if 0
  fprintf(stdout,
          "  -S[n:n] --max_cu_size:--min_cu_size   \n"
          "                                   max_cu_size:8,16,32,64. max coding unit size in pixels. [64]\n"
          "                                   min_cu_size:8,16,32,64. min coding unit size in pixels. [8]\n"
         );
  fprintf(stdout,
          "  -t[n:n] --max_tr_size:--min_tr_size   \n"
          "                                   max_tr_size:4,8,16. max transform unit size in pixels. [16]\n"
          "                                   min_tr_size:4,8,16. min transform unit size in pixels. [4]\n"
         );


  fprintf(stdout,
          "  -d[n:n] --tr_depth_intra:--tr_depth_inter            \n"
          "                                   tr_depth_intra:Max transform hierarchy depth of intra prediction. [2]\n"
          "                                   tr_depth_inter:Max transform hierarchy depth of inter prediction. [4]\n"
         );



  fprintf(stdout,
          "  -Q[n] --min_qp_size              Minimum luma coding block size of\n"
          "                                   coding units that convey cu_qp_delta_abs\n"
          "                                   and cu_qp_delta_sign. [--min_cu_size]\n"
         );
#endif
  fprintf(stdout,
          "  -R[n] --intraPicRate             Intra picture rate in frames. [0]\n"
          "                                   Forces every Nth frame to be encoded as intra frame.\n"
          "  -p[n] --cabacInitFlag            0..1 Initialization value for CABAC. [0]\n"
         );

  fprintf(stdout,
          " Parameters affecting the rate control and output stream bitrate:\n"
          "  -B[n] --bitPerSecond             10000..levelMax, target bitrate for rate control [1000000]\n"
          "  -U[n] --picRc                    0=OFF, 1=ON Picture rate control. [0]\n"
          "                                   Calculates new target QP for every frame.\n"
          //                "  -u[n] --ctbRc                    0=OFF, 1=ON ctb rate control (Check point rc). [0]\n"
          "                                   Updates target QP inside frame. not support at present\n"
          "  -C[n] --hrdConformance           0=OFF, 1=ON HRD conformance. [0]\n"
          "                                   Uses standard defined model to limit bitrate variance.\n"
          "  -c[n] --cpbSize                  HRD Coded Picture Buffer size in bits. [0]\n"
          "                                   Buffer size used by the HRD model.\n"
          "  -g[n] --gopLength                Group Of Pictures length in frames, 1..300 [intraPicRate]\n"
          "                                   Rate control allocates bits for one GOP and tries to\n"
          "                                   match the target bitrate at the end of each GOP.\n"
          "                                   Typically GOP begins with an intra frame, but this\n"
          "                                   is not mandatory.\n");

  fprintf(stdout,

          "  -s[n] --picSkip                  0=OFF, 1=ON Picture skip rate control. [0]\n"
          "                                   Allows rate control to skip frames if needed.\n"
          "  -q[n] --qpHdr                    -1..51, Initial target QP. [26]\n"
          "                                   -1 = Encoder calculates initial QP. NOTE: use -q-1\n"
          "                                   The initial QP used in the beginning of stream.\n"
          "  -n[n] --qpMin                    0..51, Minimum frame header QP. [0]\n"
          "  -m[n] --qpMax                    0..51, Maximum frame header QP. [51]\n"
          "  -A[n] --intraQpDelta             -51..51, Intra QP delta. [-5]\n"
          "                                   QP difference between target QP and intra frame QP.\n"
          "  -G[n] --fixedIntraQp             0..51, Fixed Intra QP, 0 = disabled. [0]\n"
          "                                   Use fixed QP value for every intra frame in stream.\n"


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
          "        --roi1DeltaQp              -15..0, QP delta value for ROI 1 CTBs. [0]\n"
          "        --roi2DeltaQp              -15..0, QP delta value for ROI 2 CTBs. [0]\n"
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
         );

  fprintf(stdout,
          "\nTesting parameters that are not supported for end-user:\n"
          "  -I[n] --chromaQpOffset           -12..12 Chroma QP offset. [0]\n"
          "  --smoothingIntra                 [n]   0=Normal, 1=Strong. Enable or disable strong_intra_smoothing_enabled_flag. [1]\n"
          "  --testId                         Internal test ID. [0]\n"
          "  --outReconFrame                  0=reconstructed frame is not output, 1= reconstructed frame is output [1]\n"
          "  --enableGDR                      0=disable GDR(Gradual decoder refresh), 1= enable GDR [0]\n"
          "  --dynamicenableGDR               0=disable GDR(Gradual decoder refresh), 1= dynamic enable GDR [0]\n"
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

  num = next_picture(tb) + tb->first_picture;

  if (num > tb->last_picture) return NOK;
  seek     = ((u64)num) * ((u64)src_img_size);
  lum = tb->lum;
  cb = tb->cb;
  cr = tb->cr;
  stride = get_aligned_width(src_width, inputFormat);

  if (stride == src_width)
  {
    if (read(tb->in, lum , seek, src_img_size)) return NOK;
  }
  else
  {

    //stride = (src_width+15)&(~15);
    if (inputFormat == HEVCENC_YUV420_PLANAR)
    {


      /* Lum */
      for (i = 0; i < src_height; i++)
      {
        if (read(tb->in, lum , seek, src_width)) return NOK;
        seek += src_width;
        lum += stride;
      }
      w = ((src_width + 1) >> 1);
      h = ((src_height + 1) >> 1);
      /* Cb */
      //seek += size_lum;
      for (i = 0; i < h; i++)
      {
        if (read(tb->in, cb, seek, w)) return NOK;
        seek += w;
        cb += stride / 2;
      }

      /* Cr */
      //seek += size_ch;
      for (i = 0; i < h; i++)
      {
        if (read(tb->in, cr, seek, w)) return NOK;

        seek += w;
        cr += stride / 2;
      }
    }
    else if (inputFormat < HEVCENC_YUV422_INTERLEAVED_YUYV)
    {
      /* Y */
      for (i = 0; i < src_height; i++)
      {
        if (read(tb->in, lum , seek, src_width)) return NOK;
        seek += src_width;
        lum += stride;
      }
      /* CbCr */
      for (i = 0; i < (src_height / 2); i++)
      {
        if (read(tb->in, cb, seek, src_width)) return NOK;
        seek += src_width;
        cb += stride;
      }
    }
    else if (inputFormat < HEVCENC_RGB888)
    {

      for (i = 0; i < src_height; i++)
      {

        if (read(tb->in, lum , seek, src_width * 2)) return NOK;
        seek += src_width * 2;
        lum += stride * 2;
      }
    }
    else
    {
      for (i = 0; i < src_height; i++)
      {

        if (read(tb->in, lum , seek, src_width * 4)) return NOK;
        seek += src_width * 4;
        lum += stride * 4;
      }
    }
  }

  return OK;
}

/*------------------------------------------------------------------------------
  read
------------------------------------------------------------------------------*/
i32 read(FILE *file, u8 *data, u64 seek, size_t size)
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
u64 next_picture(struct test_bench *tb)
{
  u64 numer, denom;

  numer = (u64)tb->ifr_num * (u64)tb->ofr_den;
  denom = (u64)tb->ifr_den * (u64)tb->ofr_num;

  return numer * (tb->picture_cnt / (1 << tb->interlacedFrame)) / denom;
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
          as inside h2_EWLMallocLinear().

------------------------------------------------------------------------------*/
int AllocRes(commandLine_s *cmdl, HEVCEncInst enc)
{
  i32 ret;
  u32 pictureSize;
  u32 outbufSize;


  pictureSize = ((cmdl->lumWidthSrc + cmdl->max_cu_size - 1) & (~(cmdl->max_cu_size - 1))) *
                ((cmdl->lumHeightSrc + cmdl->max_cu_size - 1) & (~(cmdl->max_cu_size - 1))) *
                HEVCEncGetBitsPerPixel(cmdl->inputFormat) / 8;

  pictureMem.virtualAddress = NULL;
  outbufMem.virtualAddress = NULL;
  pictureStabMem.virtualAddress = NULL;

  /* Here we use the EWL instance directly from the encoder
   * because it is the easiest way to allocate the linear memories */
  ret = h2_EWLMallocLinear(((struct hevc_instance *)enc)->asic.ewl, pictureSize,
                        &pictureMem);
  if (ret != EWL_OK)
  {
    pictureMem.virtualAddress = NULL;
    return 1;
  }

  if (cmdl->videoStab > 0)
  {
    ret = h2_EWLMallocLinear(((struct hevc_instance *)enc)->asic.ewl, pictureSize,
                          &pictureStabMem);
    if (ret != EWL_OK)
    {
      pictureStabMem.virtualAddress = NULL;
      return 1;
    }
  }

  /* Limited amount of memory on some test environment */
  if (outBufSizeMax == 0) outBufSizeMax = 12;
  outbufSize = 4 * pictureMem.size < ((u32)outBufSizeMax * 1024 * 1024) ?
               4 * pictureMem.size : ((u32)outBufSizeMax * 1024 * 1024);

  ret = h2_EWLMallocLinear(((struct hevc_instance *)enc)->asic.ewl, outbufSize,
                        &outbufMem);
  if (ret != EWL_OK)
  {
    outbufMem.virtualAddress = NULL;
    return 1;
  }

  printf("Input buffer size:          %d bytes\n", pictureMem.size);
  printf("Input buffer bus address:   0x%08x\n", pictureMem.busAddress);
  printf("Input buffer user address:  0x%p\n", pictureMem.virtualAddress);
  printf("Output buffer size:         %d bytes\n", outbufMem.size);
  printf("Output buffer bus address:  0x%08x\n", outbufMem.busAddress);
  printf("Output buffer user address: 0x%p\n",  outbufMem.virtualAddress);

  return 0;
}

/*------------------------------------------------------------------------------

    FreeRes

    Release all resources allcoated byt AllocRes()

------------------------------------------------------------------------------*/
void FreeRes(HEVCEncInst enc)
{
  if (pictureMem.virtualAddress != NULL)
    h2_EWLFreeLinear(((struct hevc_instance *)enc)->asic.ewl, &pictureMem);
  if (pictureStabMem.virtualAddress != NULL)
    h2_EWLFreeLinear(((struct hevc_instance *)enc)->asic.ewl, &pictureStabMem);
  if (outbufMem.virtualAddress != NULL)
    h2_EWLFreeLinear(((struct hevc_instance *)enc)->asic.ewl, &outbufMem);
}
/*------------------------------------------------------------------------------
    Callback function called by the encoder SW after "slice ready"
    interrupt from HW. Note that this function is not necessarily called
    after every slice i.e. it is possible that two or more slices are
    completed between callbacks. 
------------------------------------------------------------------------------*/
void HEVCSliceReady(HEVCEncSliceReady *slice)
{
  u32 i;
  u32 streamSize;
  u8 *strmPtr;
  /* Here is possible to implement low-latency streaming by
   * sending the complete slices before the whole frame is completed. */
   
  if(multislice_encoding&&(ENCH2_SLICE_READY_INTERRUPT))
  {
    if (slice->slicesReadyPrev == 0)    /* New frame */
    {
      
        strmPtr = (u8 *)slice->pOutBuf; /* Pointer to beginning of frame */
        streamSize=0;
        for(i=0;i<slice->nalUnitInfoNum+slice->slicesReady;i++)
        {
          streamSize+=*(slice->sliceSizes+i);
        }
        if (output_byte_stream == 0)
        {
          WriteNals(outStreamFile, (u32*)strmPtr, slice->sliceSizes,
                    slice->nalUnitInfoNum+slice->slicesReady, 0);
        }
        else
        {

          WriteStrm(outStreamFile, (u32*)strmPtr, streamSize, 0);
        }

        
      
    }
    else
    {
      strmPtr = (u8 *)slice->pAppData; /* Here we store the slice pointer */
      streamSize=0;
      for(i=(slice->nalUnitInfoNum+slice->slicesReadyPrev);i<slice->slicesReady+slice->nalUnitInfoNum;i++)
      {
        streamSize+=*(slice->sliceSizes+i);
      }
      if (output_byte_stream == 0)
      {
        WriteNals(outStreamFile, (u32*)strmPtr, slice->sliceSizes+slice->nalUnitInfoNum+slice->slicesReadyPrev,
                  slice->slicesReady-slice->slicesReadyPrev, 0);
      }
      else
      {
        WriteStrm(outStreamFile, (u32*)strmPtr, streamSize, 0);
      }      
    }
    strmPtr+=streamSize;
    /* Store the slice pointer for next callback */
    slice->pAppData = strmPtr;
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


