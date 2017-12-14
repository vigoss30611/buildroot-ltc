/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : H264 Encoder testbench for linux
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

/* For command line structure */
#include "H264TestBench.h"

/* For parameter parsing */
#include "EncGetOption.h"

/* For SW/HW shared memory allocation */
#include "ewl.h"

/* For accessing the EWL instance inside the encoder */
#include "H264Instance.h"

/* For compiler flags, test data, debug and tracing */
#include "enccommon.h"

/* For Hantro H.264 encoder */
#include "h264encapi.h"

#ifdef INTERNAL_TEST
#include "h264encapi_ext.h"
#endif

/* For printing and file IO */
#include <stdio.h>
#include <stddef.h>

/* For dynamic memory allocation */
#include <stdlib.h>

/* For memset, strcpy and strlen */
#include <string.h>

/* For sleep */
#include <unistd.h>

#ifdef USE_EFENCE
#include "efence.h"
#endif


/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------
              
NO_OUTPUT_WRITE: Output stream is not written to file. This should be used
                 when running performance simulations.
NO_INPUT_READ:   Input frames are not read from file. This should be used
                 when running performance simulations.
PSNR:            Enable PSNR calculation with --psnr option, only works with
                 system model

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

#define H264ERR_OUTPUT stdout
#define MAX_GOP_LEN 300
/* The maximum amount of frames for bitrate moving average calculation */
#define MOVING_AVERAGE_FRAMES    30

#ifdef PSNR
	float log10f(float x);
	float roundf(float x);
#endif

/* Global variables */

static char input[] = "input.yuv";
static char output[] = "stream.h264";
static char nal_sizes_file[] = "nal_sizes.txt";

static char *streamType[2] = { "BYTE_STREAM", "NAL_UNITS" };
static char *viewMode[6] = { "H264_DOUBLE_BUFFER", "H264_SINGLE_BUFFER",
                             "MVC_INTER_VIEW_PRED", "MVC_INTER_PRED",
                             "H264_MULTI_BUFFER", "INTERLACED"};

static option_s option[] = {
    {"help", 'H'},
    {"firstPic", 'a', 1},
    {"lastPic", 'b', 1},
    {"width", 'x', 1},
    {"height", 'y', 1},
    {"lumWidthSrc", 'w', 1},
    {"lumHeightSrc", 'h', 1},
    {"horOffsetSrc", 'X', 1},
    {"verOffsetSrc", 'Y', 1},
    {"outputRateNumer", 'f', 1},
    {"outputRateDenom", 'F', 1},
    {"inputRateNumer", 'j', 1},
    {"inputRateDenom", 'J', 1},
    {"inputFormat", 'l', 1},        /* Input image format */
    {"colorConversion", 'O', 1},    /* RGB to YCbCr conversion type */
    {"inputMvc", '9', 1},
    {"input", 'i', 1},              /* "input" must be after "inputFormat" */
    {"output", 'o', 1},
    {"videoRange", 'k', 1},
    {"rotation", 'r', 1},           /* Input image rotation */
    {"intraPicRate", 'I', 1},
    {"constIntraPred", 'T', 1},
    {"disableDeblocking", 'D', 1},
    {"filterOffsetA", 'W', 1},
    {"filterOffsetB", 'E', 1},

    {"trans8x8", '8', 1},           /* adaptive 4x4/8x8 transform */
    {"enableCabac", 'K', 1},
    {"cabacInitIdc", 'p', 1},
    {"mbRowPerSlice", 'V', 1},

    {"bitPerSecond", 'B', 1},
    {"picRc", 'U', 1},
    {"mbRc", 'u', 1},
    {"picSkip", 's', 1},            /* Frame skipping */
    {"gopLength", 'g', 1},          /* group of pictures length */
    {"qpMin", 'n', 1},              /* Minimum frame header qp */
    {"qpMax", 'm', 1},              /* Maximum frame header qp */
    {"qpHdr", 'q', 1},              /* Defaul qp */
    {"chromaQpOffset", 'Q', 1},     /* Chroma qp index offset */
    {"hrdConformance", 'C', 1},     /* HDR Conformance (ANNEX C) */
    {"cpbSize", 'c', 1},            /* Coded Picture Buffer Size */
    {"intraQpDelta", 'A', 1},       /* QP adjustment for intra frames */
    {"fixedIntraQp", 'G', 1},       /* Fixed QP for all intra frames */
    
    {"userData", 'z', 1},           /* SEI User data file */
    {"level", 'L', 1},              /* Level * 10  (ANNEX A) */
    {"byteStream", 'R', 1},         /* Byte stream format (ANNEX B) */
    {"sei", 'S', 1},                /* SEI messages */
    {"videoStab", 'Z', 1},          /* video stabilization */
    {"bpsAdjust", '1', 1},          /* Setting bitrate on the fly */
    {"mbQpAdjustment", '2', 1},     /* MAD based MB QP adjustment */
    {"mvOutput", '3', 1},           /* MV output in mv.txt */
    {"mbQpAutoBoost", '4', 1},      /* MB QP adjustment based on mv&variance */

    {"testId", 'e', 1},
    {"burstSize", 'N', 1},
    {"burstType", 't', 1},
    {"quarterPixelMv", 'M', 1},
    {"trigger", 'P', 1},
    {"psnr", 'd', 0},
    {"testParam", 'v', 1},

    /* Only long option can be used for all the following parameters because
     * we have no more letters to use. All shortOpt=0 will be identified by
     * long option. */
    {"cir", '0', 1},
    {"intraSliceMap1", '0', 1},
    {"intraSliceMap2", '0', 1},
    {"intraSliceMap3", '0', 1},
    {"intraArea", '0', 1},
    {"roi1Area", '0', 1},
    {"roi2Area", '0', 1},
    {"roi1DeltaQp", '0', 1},
    {"roi2DeltaQp", '0', 1},
    {"adaptiveRoiColor",  '0', 1},
    {"adaptiveRoi",       '0', 1},
    {"viewMode", '0', 1},
    {"firstTracePic", '0', 1},
    {"traceRecon", '0', 0},
    {"outBufSizeMax", '0', 1},
    {"scaledWidth", '0', 1},
    {"scaledHeight", '0', 1},
    {"interlacedFrame", '0', 1},
    {"fieldOrder", '0', 1},
    {"refFrameAmount", '0', 1},     /* Number of reference frames */
    {"longTermPicRate", '0', 1},

    {0, 0, 0}                       /* End of options */
};

typedef struct {
    i32 frame[MOVING_AVERAGE_FRAMES];
    i32 length;
    i32 count;
    i32 pos;
    i32 frameRateNumer;
    i32 frameRateDenom;
} ma_s;

/* SW/HW shared memories for input/output buffers */
static EWLLinearMem_t pictureMem;
static EWLLinearMem_t pictureStabMem;
static EWLLinearMem_t outbufMem;
static i32 outBufSizeMax;               /* outbufMem max in bytes */

static FILE *yuvFile = NULL;
static char *yuvFileName = NULL;
static FILE *yuvFileMvc = NULL;
static off_t file_size;

#ifdef MULTIFILEINPUT
/* Try to read input from input.yuv, input.yuv1, input.yuv2, etc.. */
static i32 fileNumber=0;
char inputFilename[256];
i32 inputFilenameLength;
#endif

i32 trigger_point = -1;      /* Logic Analyzer trigger point */

/* External variable for controlling the test data tracing in system model */
extern int H1EncTraceFirstFrame;

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static int AllocRes(commandLine_s * cmdl, H264EncInst enc);
static void FreeRes(H264EncInst enc);
static int OpenEncoder(commandLine_s * cml, H264EncInst * encoder);
static i32 Encode(i32 argc, char **argv, H264EncInst inst, commandLine_s * cml);
static void CloseEncoder(H264EncInst encoder);
static i32 NextPic(i32 inputRateNumer, i32 inputRateDenom, i32 outputRateNumer,
                   i32 outputRateDenom, i32 frameCnt, i32 firstPic);
static int ReadPic(u8 * image, i32 size, i32 nro, char *name, i32 width,
                   i32 height, i32 format);
static u8* ReadUserData(H264EncInst encoder, char *name);
static int Parameter(i32 argc, char **argv, commandLine_s * ep);
static void Help(void);
static void WriteStrm(FILE * fout, u32 * outbuf, u32 size, u32 endian);
static int ChangeInput(i32 argc, char **argv, char **name, option_s * option);
static void PrintTitle(commandLine_s * cml, i32 maLength, i32 qpHdr,
        i32 streamSize, H264EncOut *encOut);
static u32 PrintFrame(H264EncInst encoder, H264EncRet ret, i32 next,
                u32 frameCntTotal, i32 codedFrameCnt, i32 qpHdr,
                commandLine_s * cml, H264EncOut *encOut, i32 bitrate,
                i32 movingAverageBitrate, i32 streamSize);
static void PrintNalSizes(const u32 *pNaluSizeBuf, const u8 *pOutBuf, 
        u32 strmSize, i32 byteStream);

static void WriteNalSizesToFile(const char *file, const u32 * pNaluSizeBuf,
                                u32 numNalus);
static void WriteMotionVectors(FILE *file, H264EncInst encoder, i32 frame,
                                i32 width, i32 height);
static void PrintErrorValue(const char *errorDesc, u32 retVal);
static u32 PrintPSNR(u8 *a, u8 *b, i32 scanline, i32 wdh, i32 hgt, i32 rotation);
static u32 GetResolution(char *filename, i32 *pWidth, i32 *pHeight);
static void MaAddFrame(ma_s *ma, i32 frameSizeBits);
static i32 Ma(ma_s *ma);

void H264SliceReady(H264EncSliceReady *slice);

/*------------------------------------------------------------------------------

    main

------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    H264EncInst encoder;
    commandLine_s cmdl;
    H264EncApiVersion apiVer;
    H264EncBuild encBuild;
    int ret;

    apiVer = H264EncGetApiVersion();
    encBuild = H264EncGetBuild();

    fprintf(stdout, "H.264 Encoder API version %d.%d\n", apiVer.major,
            apiVer.minor);
    fprintf(stdout, "HW ID: %c%c 0x%08x\t SW Build: %u.%u.%u\n\n",
            encBuild.hwBuild>>24, (encBuild.hwBuild>>16)&0xff,
            encBuild.hwBuild, encBuild.swBuild / 1000000,
            (encBuild.swBuild / 1000) % 1000, encBuild.swBuild % 1000);
#ifdef EVALUATION_LIMIT
    fprintf(stdout, "Evaluation version with %d frame limit.\n\n", EVALUATION_LIMIT);
#endif

    if(argc < 2)
    {
        Help();
        exit(0);
    }

    remove(nal_sizes_file);
    remove("stream.trc");

    /* Parse command line parameters */
    if(Parameter(argc, argv, &cmdl) != 0)
    {
        fprintf(H264ERR_OUTPUT, "Input parameter error\n");
        return -1;
    }

    /* Check that input file exists */
    yuvFile = fopen(cmdl.input, "rb");

    if(yuvFile == NULL)
    {
        fprintf(H264ERR_OUTPUT, "Unable to open input file: %s\n", cmdl.input);
        return -1;
    }
    else
    {
        fclose(yuvFile);
        yuvFile = NULL;
    }

    /* Check that MVC input file exists */
    if (cmdl.inputMvc)
    {
        yuvFile = fopen(cmdl.inputMvc, "rb");

        if(yuvFile == NULL)
        {
            fprintf(H264ERR_OUTPUT, "Unable to open input file: %s\n", cmdl.input);
            return -1;
        }
        else
        {
            fclose(yuvFile);
            yuvFile = NULL;
        }

        if ((cmdl.viewMode != H264ENC_MVC_STEREO_INTER_VIEW_PRED) &&
            (cmdl.viewMode != H264ENC_MVC_STEREO_INTER_PRED))
            fprintf(H264ERR_OUTPUT,
                    "Warning: MVC input with H.264 view mode! Check --viewMode!\n");
    }

    /* Encoder initialization */
    if((ret = OpenEncoder(&cmdl, &encoder)) != 0)
    {
        return -ret;
    }

    /* Set the test ID for internal testing,
     * the SW must be compiled with testing flags */
    H264EncSetTestId(encoder, cmdl.testId);

    /* Allocate input and output buffers */
    if(AllocRes(&cmdl, encoder) != 0)
    {
        fprintf(H264ERR_OUTPUT, "Failed to allocate the external resources!\n");
        FreeRes(encoder);
        CloseEncoder(encoder);
        return -H264ENC_MEMORY_ERROR;
    }

    ret = Encode(argc, argv, encoder, &cmdl);

    FreeRes(encoder);

    CloseEncoder(encoder);

    return ret;
}

/*------------------------------------------------------------------------------

    Encode

    Do the encoding.

    Params:
        argc    - number of arguments to the application
        argv    - argument list as provided to the application
        encoder - encoder instance
        cml     - processed comand line options
    
    Return:
        0   - for success
        -1  - error

------------------------------------------------------------------------------*/
i32 Encode(i32 argc, char **argv, H264EncInst encoder, commandLine_s * cml)
{
    H264EncIn encIn;
    H264EncOut encOut;
    H264EncRet ret;
    H264EncRateCtrl rc;
    int intraPeriodCnt = 0, codedFrameCnt = 0, next = 0, src_img_size;
    i32 encodeFail = 0;
    u32 frameCnt = 0;
    u32 frameCntTotal = 0;
    u32 streamSize = 0;
    u32 bitrate = 0;
    u32 psnrSum = 0;
    u32 psnrCnt = 0;
    u32 psnr = 0;
    ma_s ma;
    i32 i;
    FILE *fout = NULL;
    FILE *fmv = NULL;
    FILE *fscaled = NULL;
    u8 *pUserData;
    i32 usedMinQp = 51, usedMaxQp = 0;
    u32 interlaced = (cml->viewMode == H264ENC_INTERLACED_FIELD);

    /* Set the window length for bitrate moving average calculation */
    ma.pos = ma.count = 0;
    ma.frameRateNumer = cml->outputRateNumer;
    ma.frameRateDenom = cml->outputRateDenom;
    if (cml->outputRateDenom)
        ma.length = MAX(1, MIN(cml->outputRateNumer / cml->outputRateDenom,
                                MOVING_AVERAGE_FRAMES));
    else
        ma.length = MOVING_AVERAGE_FRAMES;

    encIn.pOutBuf = outbufMem.virtualAddress;
    encIn.busOutBuf = outbufMem.busAddress;
    encIn.outBufSize = outbufMem.size;

    /* Source Image Size */
    src_img_size = cml->lumWidthSrc * cml->lumHeightSrc *
                    H264EncGetBitsPerPixel(cml->inputFormat)/8;

    printf("Reading input from file <%s>,\n frame size %d bytes.\n",
            cml->input, src_img_size);

    if (cml->inputMvc)
        printf("Reading second view input from file <%s>, frame size %d bytes.\n",
            cml->inputMvc, src_img_size);

    /* Start stream */
    ret = H264EncStrmStart(encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        PrintErrorValue("H264EncStrmStart() failed.", ret);
        return -1;
    }

    fout = fopen(cml->output, "wb");
    if(fout == NULL)
    {
        fprintf(H264ERR_OUTPUT, "Failed to create the output file.\n");
        return -1;
    }

    if (cml->mvOutput)
    {
        fmv = fopen("mv.txt", "wb");
        if(fmv == NULL)
        {
            fprintf(H264ERR_OUTPUT, "Failed to create mv.txt output file.\n");
            return -1;
        }
    }

    if (cml->scaledWidth)
    {
        fscaled = fopen("scaled.yuyv", "wb");
        if(fscaled == NULL)
        {
            fprintf(H264ERR_OUTPUT, "Failed to create scaled.yuv output file.\n");
            return -1;
        }
    }

    WriteStrm(fout, outbufMem.virtualAddress, encOut.streamSize, 0);
    if(cml->byteStream == 0)
    {
        WriteNalSizesToFile(nal_sizes_file, encOut.pNaluSizeBuf,
                            encOut.numNalus);
    }

    streamSize += encOut.streamSize;

    H264EncGetRateCtrl(encoder, &rc);

    /* Allocate a buffer for user data and read data from file */
    pUserData = ReadUserData(encoder, cml->userData);

    PrintTitle(cml, ma.length, rc.qpHdr, streamSize, &encOut);

    /* Setup encoder input */
    {
        u32 w = (cml->lumWidthSrc + 15) & (~0x0f);

        encIn.busLuma = pictureMem.busAddress;

        encIn.busChromaU = encIn.busLuma + (w * cml->lumHeightSrc);
        encIn.busChromaV = encIn.busChromaU +
            (((w + 1) >> 1) * ((cml->lumHeightSrc + 1) >> 1));
    }

    /* First frame is always intra with time increment = 0 */
    encIn.codingType = H264ENC_INTRA_FRAME;
    encIn.timeIncrement = 0;

    encIn.busLumaStab = pictureStabMem.busAddress;

    intraPeriodCnt = cml->intraPicRate;
    /* Main encoding loop */
  nextinput:
    while((next = NextPic(cml->inputRateNumer, cml->inputRateDenom,
                          cml->outputRateNumer, cml->outputRateDenom,
                          (cml->inputMvc || cml->interlacedFrame) ? frameCnt/2 : frameCnt,
                          cml->firstPic)) <= cml->lastPic)
    {
#ifdef EVALUATION_LIMIT
    if(frameCntTotal >= EVALUATION_LIMIT)
        break;
#endif

#ifndef NO_INPUT_READ
        /* Read next frame */
        if(ReadPic((u8 *) pictureMem.virtualAddress,
                    src_img_size, next,
                    (cml->inputMvc && (frameCnt%2)) ? cml->inputMvc : cml->input,
                   cml->lumWidthSrc, cml->lumHeightSrc, cml->inputFormat) != 0)
            break;

        if(cml->videoStab > 0)
        {
            /* Stabilize the frame after current frame */
            i32 nextStab = NextPic(cml->inputRateNumer, cml->inputRateDenom,
                          cml->outputRateNumer, cml->outputRateDenom, frameCnt+1,
                          cml->firstPic);

            if(ReadPic((u8 *) pictureStabMem.virtualAddress,
                        src_img_size, nextStab, cml->input,
                       cml->lumWidthSrc, cml->lumHeightSrc,
                       cml->inputFormat) != 0)
                break;
        }
#endif

        for (i = 0; i < MAX_BPS_ADJUST; i++)
            if (cml->bpsAdjustFrame[i] &&
                (codedFrameCnt == cml->bpsAdjustFrame[i]))
            {
                rc.bitPerSecond = cml->bpsAdjustBitrate[i];
                printf("Adjusting bitrate target: %d\n", rc.bitPerSecond);
                if((ret = H264EncSetRateCtrl(encoder, &rc)) != H264ENC_OK)
                {
                    PrintErrorValue("H264EncSetRateCtrl() failed.", ret);
                }
            }


        /* Select frame type */
        if((cml->intraPicRate != 0) && (intraPeriodCnt >= cml->intraPicRate)) {
            encIn.codingType = H264ENC_INTRA_FRAME;
            encIn.ipf = H264ENC_NO_REFERENCE_NO_REFRESH;
            encIn.ltrf = H264ENC_NO_REFERENCE_NO_REFRESH;
        }
        else if(cml->refresh_interval != 0 && codedFrameCnt % cml->refresh_interval == 0 &&
            cml->refFrameAmount > 1)
        {
            encIn.codingType = H264ENC_PREDICTED_FRAME;
            encIn.ipf = H264ENC_REFRESH;
            encIn.ltrf = H264ENC_REFERENCE;
        }
        else {
            encIn.codingType = H264ENC_PREDICTED_FRAME;
            encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
            encIn.ltrf = H264ENC_REFERENCE;
        }

        if(interlaced && (cml->intraPicRate != 0) && (intraPeriodCnt == 1))
            encIn.codingType = H264ENC_NONIDR_INTRA_FRAME;

        ret = H264EncStrmEncode(encoder, &encIn, &encOut, &H264SliceReady, NULL);

        H264EncGetRateCtrl(encoder, &rc);

        streamSize += encOut.streamSize;
        MaAddFrame(&ma, encOut.streamSize*8);

        if((frameCntTotal+1) && cml->outputRateDenom)
        {
            /* Using 64-bits to avoid overflow */
            unsigned long long tmp = streamSize / (frameCntTotal+1);
            tmp *= (u32) cml->outputRateNumer;

            bitrate = (u32) (8 * (tmp / (u32) cml->outputRateDenom));
        }

        switch (ret)
        {
        case H264ENC_FRAME_READY:

            psnr = PrintFrame(encoder, ret, next, frameCntTotal, codedFrameCnt,
                    rc.qpHdr, cml, &encOut, bitrate, Ma(&ma), streamSize);
            if (psnr) {
                psnrSum += psnr;
                psnrCnt++;
            }

            WriteStrm(fout, outbufMem.virtualAddress, encOut.streamSize, 0);

            if(cml->byteStream == 0)
                WriteNalSizesToFile(nal_sizes_file, encOut.pNaluSizeBuf,
                                    encOut.numNalus);

            WriteMotionVectors(fmv, encoder, frameCnt, cml->width,
                                interlaced ? cml->height/2 : cml->height);

            /* Write scaled encoder picture to file, packed yuyv 4:2:2 format */
            WriteStrm(fscaled, (u32*)encOut.scaledPicture,
                    cml->scaledWidth*cml->scaledHeight*2, 0);

            if (pUserData)
            {
                /* We want the user data to be written only once so
                 * we disable the user data and free the memory after
                 * first frame has been encoded. */
                H264EncSetSeiUserData(encoder, NULL, 0);
                free(pUserData);
                pUserData = NULL;
            }
            if (usedMinQp > rc.qpHdr) usedMinQp = rc.qpHdr;
            if (usedMaxQp < rc.qpHdr) usedMaxQp = rc.qpHdr;

            break;

        case H264ENC_OUTPUT_BUFFER_OVERFLOW:
            PrintFrame(encoder, ret, next, frameCntTotal, codedFrameCnt,
                    rc.qpHdr, cml, &encOut, bitrate, Ma(&ma), streamSize);
            break;

        default:
            PrintErrorValue("H264EncStrmEncode() failed.", ret);
            encodeFail = (i32) ret;
            /* For debugging, can be removed */
            WriteStrm(fout, outbufMem.virtualAddress, encOut.streamSize, 0);
            /* We try to continue encoding the next frame */
            break;
        }

        encIn.timeIncrement = cml->outputRateDenom;

        frameCnt++;
        frameCntTotal++;

        if (encOut.codingType == H264ENC_INTRA_FRAME)
            intraPeriodCnt = 0;

        if (encOut.codingType != H264ENC_NOTCODED_FRAME) {
            intraPeriodCnt++; codedFrameCnt++;
        }

    }   /* End of main encoding loop */

    /* Change next input sequence */
    if(ChangeInput(argc, argv, &cml->input, option))
    {
        if(yuvFile != NULL)
        {
            fclose(yuvFile);
            yuvFile = NULL;
        }

        frameCnt = 0;
        goto nextinput;
    }

    /* End stream */
    ret = H264EncStrmEnd(encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        PrintErrorValue("H264EncStrmEnd() failed.", ret);
    }
    else
    {
        streamSize += encOut.streamSize;
        printf("             END     %s|                     | %7i %6i | ",
                cml->refFrameAmount > 1 ? "   " : "", streamSize, encOut.streamSize);
        if (cml->psnr)
            printf("      | ");
        PrintNalSizes(encOut.pNaluSizeBuf, (u8 *) outbufMem.virtualAddress,
                encOut.streamSize, cml->byteStream);
        printf("\n");

        WriteStrm(fout, outbufMem.virtualAddress, encOut.streamSize, 0);

        if(cml->byteStream == 0)
        {
            WriteNalSizesToFile(nal_sizes_file, encOut.pNaluSizeBuf,
                                encOut.numNalus);
        }
    }

    printf("\nBitrate target %d bps, actual %d bps (%d%%).\n",
            rc.bitPerSecond, bitrate,
            (rc.bitPerSecond) ? bitrate*100/rc.bitPerSecond : 0);
    printf("Total of %d frames processed, %d frames encoded, %d bytes.\n",
            frameCntTotal, codedFrameCnt, streamSize);
    printf("Used Qp min: %d max: %d\n", usedMinQp, usedMaxQp);

    if (psnrCnt)
        printf("Average PSNR %d.%02d\n",
            (psnrSum/psnrCnt)/100, (psnrSum/psnrCnt)%100);


    /* Free all resources */
    if(fout != NULL)
        fclose(fout);

    if(fmv != NULL)
        fclose(fmv);

    if(fscaled != NULL)
        fclose(fscaled);

    if(yuvFile != NULL)
        fclose(yuvFile);

    if(yuvFileMvc != NULL)
        fclose(yuvFileMvc);

    return encodeFail;
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
int AllocRes(commandLine_s * cmdl, H264EncInst enc)
{
    i32 ret;
    u32 pictureSize;
    u32 outbufSize;
    u32 interlaced = (cmdl->viewMode == H264ENC_INTERLACED_FIELD);

    pictureSize = ((cmdl->lumWidthSrc + 15) & (~15)) * cmdl->lumHeightSrc *
                    H264EncGetBitsPerPixel(cmdl->inputFormat)/8;

    printf("Input %dx%d %s encoding %dx%d %s\n",
           cmdl->lumWidthSrc, cmdl->lumHeightSrc,
           interlaced && !cmdl->interlacedFrame ? "field" : "frame",
           cmdl->width, interlaced ? cmdl->height/2 : cmdl->height,
           interlaced ? "field" : "frame");
    
    pictureMem.virtualAddress = NULL;
    outbufMem.virtualAddress = NULL;
    pictureStabMem.virtualAddress = NULL;

    /* Here we use the EWL instance directly from the encoder 
     * because it is the easiest way to allocate the linear memories */
    ret = EWLMallocLinear(((h264Instance_s *)enc)->asic.ewl, pictureSize, 
                &pictureMem);
    if (ret != EWL_OK)
    {
        fprintf(H264ERR_OUTPUT, "Failed to allocate input picture!\n");
        pictureMem.virtualAddress = NULL;
        return 1;
    }
       
    if(cmdl->videoStab > 0)
    {
        ret = EWLMallocLinear(((h264Instance_s *)enc)->asic.ewl, pictureSize, 
                &pictureStabMem);
        if (ret != EWL_OK)
        {
            fprintf(H264ERR_OUTPUT, "Failed to allocate stab input picture!\n");
            pictureStabMem.virtualAddress = NULL;
            return 1;
        }
    }

    /* Limited amount of memory on some test environment */
    if (outBufSizeMax == 0) outBufSizeMax = 8;
    outbufSize = 4 * pictureMem.size < (outBufSizeMax*1024*1024) ?
        4 * pictureMem.size : (outBufSizeMax*1024*1024);

    ret = EWLMallocLinear(((h264Instance_s *)enc)->asic.ewl, outbufSize, 
                &outbufMem);
    if (ret != EWL_OK)
    {
        fprintf(H264ERR_OUTPUT, "Failed to allocate output buffer!\n");
        outbufMem.virtualAddress = NULL;
        return 1;
    }

    printf("Input buffer size:          %d bytes\n", pictureMem.size);
    printf("Input buffer bus address:   0x%08x\n", pictureMem.busAddress);
    printf("Input buffer user address:  0x%08x\n", (u32) pictureMem.virtualAddress);
    printf("Output buffer size:         %d bytes\n", outbufMem.size);
    printf("Output buffer bus address:  0x%08x\n", outbufMem.busAddress);
    printf("Output buffer user address: 0x%08x\n", (u32) outbufMem.virtualAddress);

    return 0;
}

/*------------------------------------------------------------------------------

    FreeRes

    Release all resources allcoated byt AllocRes()

------------------------------------------------------------------------------*/
void FreeRes(H264EncInst enc)
{
    if(pictureMem.virtualAddress != NULL)
        EWLFreeLinear(((h264Instance_s *)enc)->asic.ewl, &pictureMem); 
    if(pictureStabMem.virtualAddress != NULL)
        EWLFreeLinear(((h264Instance_s *)enc)->asic.ewl, &pictureStabMem); 
    if(outbufMem.virtualAddress != NULL)
        EWLFreeLinear(((h264Instance_s *)enc)->asic.ewl, &outbufMem); 
}

/*------------------------------------------------------------------------------

    CheckArea

------------------------------------------------------------------------------*/
i32 CheckArea(H264EncPictureArea *area, commandLine_s * cml)
{
    i32 w = (cml->width+15)/16;
    i32 h = (cml->height+15)/16;

    if ((area->left < w) && (area->right < w) &&
        (area->top < h) && (area->bottom < h)) return 1;

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
int OpenEncoder(commandLine_s * cml, H264EncInst * pEnc)
{
    H264EncRet ret;
    H264EncConfig cfg;
    H264EncCodingCtrl codingCfg;
    H264EncRateCtrl rcCfg;
    H264EncPreProcessingCfg preProcCfg;
    H264EncInst encoder;

    /* Default resolution, try parsing input file name */
    if(cml->lumWidthSrc == DEFAULT || cml->lumHeightSrc == DEFAULT)
    {
        if (GetResolution(cml->input, &cml->lumWidthSrc, &cml->lumHeightSrc))
        {
            /* No dimensions found in filename, using default QCIF */
            cml->lumWidthSrc = 176;
            cml->lumHeightSrc = 144;
        }
    }

    /* Encoder initialization */
    if(cml->width == DEFAULT)
        cml->width = cml->lumWidthSrc;

    if(cml->height == DEFAULT)
        cml->height = cml->lumHeightSrc;

    /* outputRateNumer */
    if(cml->outputRateNumer == DEFAULT)
    {
        cml->outputRateNumer = cml->inputRateNumer;
    }

    /* outputRateDenom */
    if(cml->outputRateDenom == DEFAULT)
    {
        cml->outputRateDenom = cml->inputRateDenom;
    }

    if(cml->rotation)
    {
        cfg.width = cml->height;
        cfg.height = cml->width;
        cfg.scaledWidth = cml->scaledHeight;
        cfg.scaledHeight = cml->scaledWidth;
    }
    else
    {
        cfg.width = cml->width;
        cfg.height = cml->height;
        cfg.scaledWidth = cml->scaledWidth;
        cfg.scaledHeight = cml->scaledHeight;
    }

    cfg.frameRateDenom = cml->outputRateDenom;
    cfg.frameRateNum = cml->outputRateNumer;
    cfg.viewMode = H264ENC_BASE_VIEW_DOUBLE_BUFFER; /* Two buffers by default */
    if(cml->viewMode != DEFAULT)
        cfg.viewMode = cml->viewMode;
    if(cml->byteStream)
        cfg.streamType = H264ENC_BYTE_STREAM;
    else
        cfg.streamType = H264ENC_NAL_UNIT_STREAM;

    cfg.level = H264ENC_LEVEL_4;

    if(cml->level != DEFAULT && cml->level != 0)
        cfg.level = (H264EncLevel)cml->level;

    cfg.refFrameAmount = cml->refFrameAmount;
    if(cfg.refFrameAmount > 1 && cml->viewMode == DEFAULT)
        cfg.viewMode = H264ENC_BASE_VIEW_MULTI_BUFFER;

    printf("Init config: size %dx%d   %d/%d fps  %s  %s  P&L %d\n"
           "  scaled %dx%d  refFrames %d\n",
         cfg.width, cfg.height, cfg.frameRateNum,
         cfg.frameRateDenom, streamType[cfg.streamType],
         viewMode[cfg.viewMode], cfg.level,
         cfg.scaledWidth, cfg.scaledHeight, cfg.refFrameAmount);

    if((ret = H264EncInit(&cfg, pEnc)) != H264ENC_OK)
    {
        PrintErrorValue("H264EncInit() failed.", ret);
        return (int)ret;
    }

    encoder = *pEnc;

    /* Encoder setup: rate control */
    if((ret = H264EncGetRateCtrl(encoder, &rcCfg)) != H264ENC_OK)
    {
        PrintErrorValue("H264EncGetRateCtrl() failed.", ret);
        CloseEncoder(encoder);
        return -1;
    }
    else
    {
        printf("Get rate control: qp %2d [%2d, %2d]  %8d bps  "
               "pic %d mb %d skip %d  hrd %d\n  cpbSize %d gopLen %d "
               "intraQpDelta %2d\n",
               rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
               rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
               rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta);

        if(cml->qpHdr != DEFAULT)
            rcCfg.qpHdr = cml->qpHdr;
        if(cml->qpMin != DEFAULT)
            rcCfg.qpMin = cml->qpMin;
        if(cml->qpMax != DEFAULT)
            rcCfg.qpMax = cml->qpMax;
        if(cml->picSkip != DEFAULT)
            rcCfg.pictureSkip = cml->picSkip;
        if(cml->picRc != DEFAULT)
            rcCfg.pictureRc = cml->picRc;
        if(cml->mbRc != DEFAULT)
            rcCfg.mbRc = cml->mbRc != 0 ? 1 : 0;
        if(cml->bitPerSecond != DEFAULT)
            rcCfg.bitPerSecond = cml->bitPerSecond;

        if(cml->hrdConformance != DEFAULT)
            rcCfg.hrd = cml->hrdConformance;

        if(cml->cpbSize != DEFAULT)
            rcCfg.hrdCpbSize = cml->cpbSize;

        if(cml->intraPicRate != 0)
            rcCfg.gopLen = MIN(cml->intraPicRate, MAX_GOP_LEN);

        if(cml->gopLength != DEFAULT)
            rcCfg.gopLen = cml->gopLength;
        
        if(cml->intraQpDelta != DEFAULT)
            rcCfg.intraQpDelta = cml->intraQpDelta;

        rcCfg.fixedIntraQp = cml->fixedIntraQp;
        rcCfg.mbQpAdjustment = cml->mbQpAdjustment;

        if (cml->longTermPicRate != DEFAULT)
            rcCfg.longTermPicRate = cml->longTermPicRate;
                
        rcCfg.mbQpAutoBoost = cml->mbQpAutoBoost;

        printf("Set rate control: qp %2d [%2d, %2d] %8d bps  "
               "pic %d mb %d skip %d  hrd %d\n"
               "  cpbSize %d gopLen %d intraQpDelta %2d "
               "fixedIntraQp %2d mbQpAdjustment %d "
               "mbQpAutoBoost %d\n",
               rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
               rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
               rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta,
               rcCfg.fixedIntraQp, rcCfg.mbQpAdjustment, rcCfg.mbQpAutoBoost);

        if((ret = H264EncSetRateCtrl(encoder, &rcCfg)) != H264ENC_OK)
        {
            PrintErrorValue("H264EncSetRateCtrl() failed.", ret);
            CloseEncoder(encoder);
            return -1;
        }
    }

    /* Encoder setup: coding control */
    if((ret = H264EncGetCodingCtrl(encoder, &codingCfg)) != H264ENC_OK)
    {
        PrintErrorValue("H264EncGetCodingCtrl() failed.", ret);
        CloseEncoder(encoder);
        return -1;
    }
    else
    {
        if(cml->mbRowPerSlice != DEFAULT)
        {
            codingCfg.sliceSize = cml->mbRowPerSlice;
        }

        if(cml->constIntraPred != DEFAULT)
        {
            if(cml->constIntraPred != 0)
                codingCfg.constrainedIntraPrediction = 1;
            else
                codingCfg.constrainedIntraPrediction = 0;
        }

        if(cml->disableDeblocking != 0)
            codingCfg.disableDeblockingFilter = 1;
        else
            codingCfg.disableDeblockingFilter = 0;

        if((cml->disableDeblocking != 1) &&
           ((cml->filterOffsetA != 0) || (cml->filterOffsetB != 0)))
            codingCfg.disableDeblockingFilter = 1;

        if(cml->enableCabac != DEFAULT)
        {
            codingCfg.enableCabac = cml->enableCabac;
            if (cml->cabacInitIdc != DEFAULT)
                codingCfg.cabacInitIdc = cml->cabacInitIdc;
        }

        codingCfg.transform8x8Mode = cml->trans8x8;

        if(cml->quarterPixelMv != DEFAULT)
            codingCfg.quarterPixelMv = cml->quarterPixelMv;

        if(cml->videoRange != DEFAULT)
        {
            if(cml->videoRange != 0)
                codingCfg.videoFullRange = 1;
            else
                codingCfg.videoFullRange = 0;
        }

        if(cml->sei)
            codingCfg.seiMessages = 1;
        else
            codingCfg.seiMessages = 0;

        codingCfg.fieldOrder = cml->fieldOrder;
        codingCfg.cirStart = cml->cirStart;
        codingCfg.cirInterval = cml->cirInterval;
        codingCfg.intraSliceMap1 = cml->intraSliceMap1;
        codingCfg.intraSliceMap2 = cml->intraSliceMap2;
        codingCfg.intraSliceMap3 = cml->intraSliceMap3;
        codingCfg.intraArea.top = cml->intraAreaTop;
        codingCfg.intraArea.left = cml->intraAreaLeft;
        codingCfg.intraArea.bottom = cml->intraAreaBottom;
        codingCfg.intraArea.right = cml->intraAreaRight;
        codingCfg.intraArea.enable = CheckArea(&codingCfg.intraArea, cml);

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

        if (cml->adaptiveRoi != DEFAULT)
            codingCfg.adaptiveRoi = cml->adaptiveRoi;
        if (cml->adaptiveRoiColor != DEFAULT)
            codingCfg.adaptiveRoiColor = cml->adaptiveRoiColor;

        printf
            ("Set coding control: SEI %d Slice %5d   deblocking %d "
             "constrained intra %d video range %d\n"
             "  cabac %d  cabac initial idc %d  Adaptive 8x8 transform %d"
             "  quarter-pixel MV %d\n"
             "  adaptiveRoi %d,%d\n",
             codingCfg.seiMessages, codingCfg.sliceSize,
             codingCfg.disableDeblockingFilter,
             codingCfg.constrainedIntraPrediction, codingCfg.videoFullRange,
             codingCfg.enableCabac, codingCfg.cabacInitIdc,
             codingCfg.transform8x8Mode, codingCfg.quarterPixelMv,
             codingCfg.adaptiveRoi, codingCfg.adaptiveRoiColor);

        if (codingCfg.cirInterval)
            printf("  CIR: %d %d\n",
                codingCfg.cirStart, codingCfg.cirInterval);

        if (codingCfg.intraSliceMap1 || codingCfg.intraSliceMap2 ||
            codingCfg.intraSliceMap3)
            printf("  IntraSliceMap: 0x%0x 0x%0x 0x%0x\n",
                codingCfg.intraSliceMap1, codingCfg.intraSliceMap2,
                codingCfg.intraSliceMap3);

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

        if((ret = H264EncSetCodingCtrl(encoder, &codingCfg)) != H264ENC_OK)
        {
            PrintErrorValue("H264EncSetCodingCtrl() failed.", ret);
            CloseEncoder(encoder);
            return -1;
        }

#ifdef INTERNAL_TEST
        /* Set some values outside the product API for internal
         * testing purposes */

        H264EncSetChromaQpIndexOffset(encoder, cml->chromaQpOffset);
        printf("Set ChromaQpIndexOffset: %d\n", cml->chromaQpOffset);

        H264EncSetHwBurstSize(encoder, cml->burst);
        printf("Set HW Burst Size: %d\n", cml->burst);
        H264EncSetHwBurstType(encoder, cml->bursttype);
        printf("Set HW Burst Type: %d\n", cml->bursttype);
        if(codingCfg.disableDeblockingFilter == 1)
        {
            H264EncFilter advCoding;

            H264EncGetFilter(encoder, &advCoding);

            advCoding.disableDeblocking = cml->disableDeblocking;

            advCoding.filterOffsetA = cml->filterOffsetA * 2;
            advCoding.filterOffsetB = cml->filterOffsetB * 2;

            printf
                ("Set filter params: disableDeblocking %d filterOffsetA = %i filterOffsetB = %i\n",
                 advCoding.disableDeblocking, advCoding.filterOffsetA,
                 advCoding.filterOffsetB);

            ret = H264EncSetFilter(encoder, &advCoding);
            if(ret != H264ENC_OK)
            {
                PrintErrorValue("H264EncSetFilter() failed.", ret);
                CloseEncoder(encoder);
                return -1;
            }
        }

        if (cml->traceRecon)
            ((h264Instance_s *)encoder)->asic.traceRecon = 1;
#endif

    }

    /* PreP setup */
    if((ret = H264EncGetPreProcessing(encoder, &preProcCfg)) != H264ENC_OK)
    {
        PrintErrorValue("H264EncGetPreProcessing() failed.", ret);
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

    preProcCfg.inputType = (H264EncPictureType)cml->inputFormat;
    preProcCfg.rotation = (H264EncPictureRotation)cml->rotation;
    preProcCfg.interlacedFrame = cml->interlacedFrame;

    preProcCfg.origWidth = cml->lumWidthSrc;
    preProcCfg.origHeight = cml->lumHeightSrc;
    if (cml->interlacedFrame) preProcCfg.origHeight /= 2;

    if(cml->horOffsetSrc != DEFAULT)
        preProcCfg.xOffset = cml->horOffsetSrc;
    if(cml->verOffsetSrc != DEFAULT)
        preProcCfg.yOffset = cml->verOffsetSrc;
    if(cml->videoStab != DEFAULT)
        preProcCfg.videoStabilization = cml->videoStab;
    if(cml->colorConversion != DEFAULT)
        preProcCfg.colorConversion.type =
                        (H264EncColorConversionType)cml->colorConversion;
    if(preProcCfg.colorConversion.type == H264ENC_RGBTOYUV_USER_DEFINED)
    {
        preProcCfg.colorConversion.coeffA = 20000;
        preProcCfg.colorConversion.coeffB = 44000;
        preProcCfg.colorConversion.coeffC = 5000;
        preProcCfg.colorConversion.coeffE = 35000;
        preProcCfg.colorConversion.coeffF = 38000;
    }

    printf
        ("Set PreP: input %4dx%d : offset %4dx%d : format %d : rotation %d\n"
           " stab %d : cc %d : scaling %d\n",
         preProcCfg.origWidth, preProcCfg.origHeight, preProcCfg.xOffset,
         preProcCfg.yOffset, preProcCfg.inputType, preProcCfg.rotation,
         preProcCfg.videoStabilization, preProcCfg.colorConversion.type,
         preProcCfg.scaledOutput);

    /* If HW doesn't support scaling there will be no scaled output. */
    if (!preProcCfg.scaledOutput) cml->scaledWidth = 0;

    if((ret = H264EncSetPreProcessing(encoder, &preProcCfg)) != H264ENC_OK)
    {
        PrintErrorValue("H264EncSetPreProcessing() failed.", ret);
        CloseEncoder(encoder);
        return (int)ret;
    }
    return 0;
}

/*------------------------------------------------------------------------------

    CloseEncoder
       Release an encoder insatnce.

   Params:
        encoder - the instance to be released
------------------------------------------------------------------------------*/
void CloseEncoder(H264EncInst encoder)
{
    H264EncRet ret;

    if((ret = H264EncRelease(encoder)) != H264ENC_OK)
    {
        PrintErrorValue("H264EncRelease() failed.", ret);
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
/*------------------------------------------------------------------------------

    Parameter
        Process the testbench calling arguments.

    Params:
        argc    - number of arguments to the application
        argv    - argument list as provided to the application
        cml     - processed comand line options   
    Return:
        0   - for success
        -1  - error    

------------------------------------------------------------------------------*/
int Parameter(i32 argc, char **argv, commandLine_s * cml)
{
    i32 ret, i;
    char *optArg;
    argument_s argument;
    i32 status = 0;
    i32 bpsAdjustCount = 0;

    memset(cml, 0, sizeof(commandLine_s));

    cml->width = DEFAULT;
    cml->height = DEFAULT;
    cml->outputRateNumer = DEFAULT;
    cml->outputRateDenom = DEFAULT;
    cml->qpHdr = DEFAULT;
    cml->qpMin = DEFAULT;
    cml->qpMax = DEFAULT;
    cml->picRc = DEFAULT;
    cml->mbRc = DEFAULT;
    cml->cpbSize = DEFAULT;
    cml->constIntraPred = DEFAULT;
    cml->horOffsetSrc = DEFAULT;
    cml->verOffsetSrc = DEFAULT;
    cml->videoStab = DEFAULT;
    cml->gopLength = DEFAULT;

    cml->input = input;
    cml->output = output;
    cml->firstPic = 0;
    cml->lastPic = 100;
    cml->inputRateNumer = 30;
    cml->inputRateDenom = 1;
    cml->refFrameAmount = 1;
    cml->refresh_interval = 0;
    cml->longTermPicRate = DEFAULT;

    cml->lumWidthSrc = DEFAULT;
    cml->lumHeightSrc = DEFAULT;
    cml->rotation = 0;
    cml->inputFormat = 0;

    cml->bitPerSecond = 1000000;
    cml->intraQpDelta = DEFAULT;
    cml->fixedIntraQp=0;

    cml->chromaQpOffset = 2;
    cml->disableDeblocking = 0;
    cml->filterOffsetA = 0;
    cml->filterOffsetB = 0;
    cml->enableCabac = DEFAULT;
    cml->cabacInitIdc = DEFAULT;
    cml->trans8x8 = 0;
    cml->burst = 16;
    cml->bursttype = 0;
    cml->quarterPixelMv = DEFAULT;
    cml->testId = 0;
    cml->viewMode = DEFAULT;
    cml->adaptiveRoi       = DEFAULT;
    cml->adaptiveRoiColor  = DEFAULT;

    cml->sei = 0;
    cml->byteStream = 1;
    cml->hrdConformance = 0;
    cml->videoRange = 0;
    cml->picSkip = 0;
    cml->psnr = 0;
    cml->testParam = 0;
    cml->intraAreaLeft = cml->intraAreaRight = cml->intraAreaTop =
        cml->intraAreaBottom = -1;  /* Disabled */

    argument.optCnt = 1;
    while((ret = EncGetOption(argc, argv, option, &argument)) != -1)
    {
        if(ret == -2)
        {
            status = 1;
        }
        optArg = argument.optArg;
        switch (argument.shortOpt)
        {
        case 'i':
            cml->input = optArg;
            break;
        case '9':
            cml->inputMvc = optArg;
            break;
        case 'o':
            cml->output = optArg;
            break;
        case 'z':
            cml->userData = optArg;
            break;
        case 'a':
            cml->firstPic = atoi(optArg);
            break;
        case 'b':
            cml->lastPic = atoi(optArg);
            break;
        case 'x':
            cml->width = atoi(optArg);
            break;
        case 'y':
            cml->height = atoi(optArg);
            break;
        case 'w':
            cml->lumWidthSrc = atoi(optArg);
            break;
        case 'h':
            cml->lumHeightSrc = atoi(optArg);
            break;
        case 'X':
            cml->horOffsetSrc = atoi(optArg);
            break;
        case 'Y':
            cml->verOffsetSrc = atoi(optArg);
            break;
        case 'f':
            cml->outputRateNumer = atoi(optArg);
            break;
        case 'F':
            cml->outputRateDenom = atoi(optArg);
            break;
        case 'j':
            cml->inputRateNumer = atoi(optArg);
            break;
        case 'J':
            cml->inputRateDenom = atoi(optArg);
            break;
        case 'T':
            cml->constIntraPred = atoi(optArg);
            break;
        case 'D':
            cml->disableDeblocking = atoi(optArg);
            break;
        case 'W':
            cml->filterOffsetA = atoi(optArg);
            break;
        case 'E':
            cml->filterOffsetB = atoi(optArg);
            break;
        case '8':
            cml->trans8x8 = atoi(optArg);
            break;
        case 'K':
            cml->enableCabac = atoi(optArg);
            break;
        case 'p':
            cml->cabacInitIdc = atoi(optArg);
            break;
        case 'I':
            cml->intraPicRate = atoi(optArg);
            break;
        case 'N':
            cml->burst = atoi(optArg);
            break;
        case 't':
            cml->bursttype = atoi(optArg);
            break;
        case 'q':
            cml->qpHdr = atoi(optArg);
            break;
        case 'n':
            cml->qpMin = atoi(optArg);
            break;
        case 'm':
            cml->qpMax = atoi(optArg);
            break;
        case 'B':
            cml->bitPerSecond = atoi(optArg);
            break;
        case 'U':
            cml->picRc = atoi(optArg);
            break;
        case 'u':
            cml->mbRc = atoi(optArg);
            break;
        case 's':
            cml->picSkip = atoi(optArg);
            break;
        case 'r':
            cml->rotation = atoi(optArg);
            break;
        case 'l':
            cml->inputFormat = atoi(optArg);
            break;
        case 'O':
            cml->colorConversion = atoi(optArg);
            break;
        case 'Q':
            cml->chromaQpOffset = atoi(optArg);
            break;
        case 'V':
            cml->mbRowPerSlice = atoi(optArg);
            break;
        case 'k':
            cml->videoRange = atoi(optArg);
            break;
        case 'L':
            cml->level = atoi(optArg);
            break;
        case 'C':
            cml->hrdConformance = atoi(optArg);
            break;
        case 'c':
            cml->cpbSize = atoi(optArg);
            break;
        case 'e':
            cml->testId = atoi(optArg);
            break;
        case 'S':
            cml->sei = atoi(optArg);
            break;
        case 'M':
            cml->quarterPixelMv = atoi(optArg);
            break;
        case 'P':
            trigger_point = atoi(optArg);
            break;
        case 'R':
            cml->byteStream = atoi(optArg);
            break;
        case 'Z':
            cml->videoStab = atoi(optArg);
            break;
        case 'g':
            cml->gopLength = atoi(optArg);
            break;
        case 'A':
            cml->intraQpDelta = atoi(optArg);
            break;
        case 'G':
            cml->fixedIntraQp = atoi(optArg);
            break;
        case '1':
            if (bpsAdjustCount == MAX_BPS_ADJUST)
                break;
            /* Argument must be "xx:yy", replace ':' with 0 */
            if ((i = ParseDelim(optArg, ':')) == -1) break;
            /* xx is frame number */
            cml->bpsAdjustFrame[bpsAdjustCount] = atoi(optArg);
            /* yy is new target bitrate */
            cml->bpsAdjustBitrate[bpsAdjustCount] = atoi(optArg+i+1);
            bpsAdjustCount++;
            break;
        case '2':
            cml->mbQpAdjustment = atoi(optArg);
            break;
        case '4':
            cml->mbQpAutoBoost = atoi(optArg);
            break;
        case 'v':
            cml->testParam = atoi(optArg);
            break;
        case 'd':
            cml->psnr = 1;
            break;
        case '3':
            cml->mvOutput = atoi(optArg);
            break;
        case '0':
            /* Check long option */
            if (strcmp(argument.longOpt, "intraSliceMap1") == 0)
                cml->intraSliceMap1 = strtoul(optArg, NULL, 16);

            if (strcmp(argument.longOpt, "intraSliceMap2") == 0)
                cml->intraSliceMap2 = strtoul(optArg, NULL, 16);

            if (strcmp(argument.longOpt, "intraSliceMap3") == 0)
                cml->intraSliceMap3 = strtoul(optArg, NULL, 16);

            if (strcmp(argument.longOpt, "roi1DeltaQp") == 0)
                cml->roi1AreaEnable = cml->roi1DeltaQp = atoi(optArg);

            if (strcmp(argument.longOpt, "roi2DeltaQp") == 0)
                cml->roi2AreaEnable = cml->roi2DeltaQp = atoi(optArg);

            if (strcmp(argument.longOpt, "adaptiveRoiColor") == 0) {
                cml->adaptiveRoiColor = atoi(optArg); break;
            }

            if (strcmp(argument.longOpt, "adaptiveRoi") == 0)
                cml->adaptiveRoi = atoi(optArg);

            if (strcmp(argument.longOpt, "viewMode") == 0)
                cml->viewMode = atoi(optArg);

            if (strcmp(argument.longOpt, "traceRecon") == 0)
                cml->traceRecon = 1;

            if (strcmp(argument.longOpt, "firstTracePic") == 0)
                H1EncTraceFirstFrame = atoi(optArg);

            if (strcmp(argument.longOpt, "outBufSizeMax") == 0)
                outBufSizeMax = atoi(optArg);

            if (strcmp(argument.longOpt, "cir") == 0)
            {
                /* Argument must be "xx:yy", replace ':' with 0 */
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                /* xx is cir start */
                cml->cirStart = atoi(optArg);
                /* yy is cir interval */
                cml->cirInterval = atoi(optArg+i+1);
            }

            if (strcmp(argument.longOpt, "intraArea") == 0)
            {
                /* Argument must be "xx:yy:XX:YY".
                 * xx is left coordinate, replace first ':' with 0 */
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->intraAreaLeft = atoi(optArg);
                /* yy is top coordinate */
                optArg += i+1;
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->intraAreaTop = atoi(optArg);
                /* XX is right coordinate */
                optArg += i+1;
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->intraAreaRight = atoi(optArg);
                /* YY is bottom coordinate */
                optArg += i+1;
                cml->intraAreaBottom = atoi(optArg);
                cml->intraAreaEnable = 1;
            }

            if (strcmp(argument.longOpt, "roi1Area") == 0)
            {
                /* Argument must be "xx:yy:XX:YY".
                 * xx is left coordinate, replace first ':' with 0 */
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->roi1AreaLeft = atoi(optArg);
                /* yy is top coordinate */
                optArg += i+1;
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->roi1AreaTop = atoi(optArg);
                /* XX is right coordinate */
                optArg += i+1;
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->roi1AreaRight = atoi(optArg);
                /* YY is bottom coordinate */
                optArg += i+1;
                cml->roi1AreaBottom = atoi(optArg);
                cml->roi1AreaEnable = 1;
            }

            if (strcmp(argument.longOpt, "roi2Area") == 0)
            {
                /* Argument must be "xx:yy:XX:YY".
                 * xx is left coordinate, replace first ':' with 0 */
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->roi2AreaLeft = atoi(optArg);
                /* yy is top coordinate */
                optArg += i+1;
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->roi2AreaTop = atoi(optArg);
                /* XX is right coordinate */
                optArg += i+1;
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                cml->roi2AreaRight = atoi(optArg);
                /* YY is bottom coordinate */
                optArg += i+1;
                cml->roi2AreaBottom = atoi(optArg);
                cml->roi2AreaEnable = 1;
            }

            if (strcmp(argument.longOpt, "scaledWidth") == 0)
                cml->scaledWidth = atoi(optArg);

            if (strcmp(argument.longOpt, "scaledHeight") == 0)
                cml->scaledHeight = atoi(optArg);

            if (strcmp(argument.longOpt, "interlacedFrame") == 0)
                cml->interlacedFrame = atoi(optArg);

            if (strcmp(argument.longOpt, "fieldOrder") == 0)
                cml->fieldOrder = atoi(optArg);

            if (strcmp(argument.longOpt, "refFrameAmount") == 0)
                cml->refFrameAmount = atoi(optArg);

            if (strcmp(argument.longOpt, "longTermPicRate") == 0)
                cml->longTermPicRate = atoi(optArg);

            break;

        default:
            break;
        }
    }

    return status;
}

/*------------------------------------------------------------------------------

    ReadPic

    Read raw YUV 4:2:0 or 4:2:2 image data from file

    Params:
        image   - buffer where the image will be saved
        size    - amount of image data to be read
        nro     - picture number to be read
        name    - name of the file to read
        width   - image width in pixels
        height  - image height in pixels
        format  - image format (YUV 420/422/RGB16/RGB32)

    Returns:
        0 - for success
        non-zero error code 
------------------------------------------------------------------------------*/
int ReadPic(u8 * image, i32 size, i32 nro, char *name, i32 width, i32 height,
            i32 format)
{
    int ret = 0;
    FILE *readFile = NULL;

    if(yuvFile == NULL)
    {
        yuvFile = fopen(name, "rb");
        yuvFileName = name;

        if(yuvFile == NULL)
        {
            fprintf(H264ERR_OUTPUT, "\nUnable to open YUV file: %s\n", name);
            ret = -1;
            goto end;
        }

        fseeko(yuvFile, 0, SEEK_END);
        file_size = ftello(yuvFile);
    }

    if((name != yuvFileName) && (yuvFileMvc == NULL))
    {
        yuvFileMvc = fopen(name, "rb");

        if(yuvFileMvc == NULL)
        {
            fprintf(H264ERR_OUTPUT, "\nUnable to open YUV file: %s\n", name);
            ret = -1;
            goto end;
        }

        fseeko(yuvFileMvc, 0, SEEK_END);
        file_size = ftello(yuvFileMvc);
    }

    if(name == yuvFileName)
        readFile = yuvFile;
    else
        readFile = yuvFileMvc;

    /* Stop if over last frame of the file */
    if((off_t)size * (nro + 1) > file_size)
    {
        printf("\nCan't read frame, EOF\n");
        ret = -1;
        goto end;
    }

    if(fseeko(readFile, (off_t)size * nro, SEEK_SET) != 0)
    {
        fprintf(H264ERR_OUTPUT, "\nI can't seek frame no: %i from file: %s\n",
                nro, name);
        ret = -1;
        goto end;
    }

    if((width & 0x0f) == 0)
        fread(image, 1, size, readFile);
    else
    {
        i32 i;
        u8 *buf = image;
        i32 scan = (width + 15) & (~0x0f);

        /* Read the frame so that scan (=stride) is multiple of 16 pixels */

        if(format == H264ENC_YUV420_PLANAR)
        {
            /* Y */
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width, readFile);
                buf += scan;
            }
            /* Cb */
            for(i = 0; i < (height / 2); i++)
            {
                fread(buf, 1, width / 2, readFile);
                buf += scan / 2;
            }
            /* Cr */
            for(i = 0; i < (height / 2); i++)
            {
                fread(buf, 1, width / 2, readFile);
                buf += scan / 2;
            }
        }
        else if(format < H264ENC_YUV422_INTERLEAVED_YUYV)
        {
            /* Y */
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width, readFile);
                buf += scan;
            }
            /* CbCr */
            for(i = 0; i < (height / 2); i++)
            {
                fread(buf, 1, width, readFile);
                buf += scan;
            }
        }
        else if(format < H264ENC_RGB888)   /* 16 bpp */
        {
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width * 2, readFile);
                buf += scan * 2;
            }
        }
        else    /* 32 bpp */
        {
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width * 4, readFile);
                buf += scan * 4;
            }
        }

    }

  end:

    return ret;
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
u8* ReadUserData(H264EncInst encoder, char *name)
{
    FILE *file = NULL;
    i32 byteCnt;
    u8 *data;

    if(name == NULL)
        return NULL;

    if(strcmp("0", name) == 0)
        return NULL;

    /* Get user data length from file */
    file = fopen(name, "rb");
    if(file == NULL)
    {
        fprintf(H264ERR_OUTPUT, "Unable to open User Data file: %s\n", name);
        return NULL;
    }
    fseek(file, 0L, SEEK_END);
    byteCnt = ftell(file);
    rewind(file);

    /* Minimum size of user data */
    if (byteCnt < 16)
        byteCnt = 16;

    /* Maximum size of user data */
    if (byteCnt > 2048)
        byteCnt = 2048;

    /* Allocate memory for user data */
    if((data = (u8 *) malloc(sizeof(u8) * byteCnt)) == NULL)
    {
        fclose(file);
        fprintf(H264ERR_OUTPUT, "Unable to alloc User Data memory\n");
        return NULL;
    }

    /* Read user data from file */
    fread(data, sizeof(u8), byteCnt, file);
    fclose(file);

    printf("User data: %d bytes [%d %d %d %d ...]\n",
        byteCnt, data[0], data[1], data[2], data[3]);

    /* Pass the data buffer to encoder
     * The encoder reads the buffer during following H264EncStrmEncode() calls.
     * User data writing must be disabled (with SetSeiUserData(enc, 0, 0)) */
    H264EncSetSeiUserData(encoder, data, byteCnt);

    return data;
}

/*------------------------------------------------------------------------------

    Help
        Print out some instructions about usage.
------------------------------------------------------------------------------*/
void Help(void)
{
    fprintf(stdout, "Usage:  %s [options] -i inputfile\n\n", "h264_testenc");
    fprintf(stdout, "Default parameters are marked inside []. More detailed description of\n"
                    "H.264 API parameters can be found from H.264 API Manual.\n\n");

    fprintf(stdout,
            " Parameters affecting which input frames are encoded:\n"
            "  -i[s] --input             Read input video sequence from file. [input.yuv]\n"
            "  -9[s] --inputMvc          Read MVC second view input video sequence from file. []\n"
            "  -o[s] --output            Write output H.264 stream to file. [stream.h264]\n"
            "  -a[n] --firstPic          First picture of input file to encode. [0]\n"
            "  -b[n] --lastPic           Last picture of input file to encode. [100]\n"
            "  -f[n] --outputRateNumer   1..1048575 Output picture rate numerator. [30]\n"
            "  -F[n] --outputRateDenom   1..1048575 Output picture rate denominator. [1]\n"
            "                               Encoded frame rate will be\n"
            "                               outputRateNumer/outputRateDenom fps\n"
            "  -j[n] --inputRateNumer    1..1048575 Input picture rate numerator. [30]\n"
            "  -J[n] --inputRateDenom    1..1048575 Input picture rate denominator. [1]\n\n");

    fprintf(stdout,
            " Parameters affecting input frame and encoded frame resolutions and cropping:\n"
            "  -w[n] --lumWidthSrc       Width of source image. [176]\n"
            "  -h[n] --lumHeightSrc      Height of source image. [144]\n");
    fprintf(stdout,
            "  -x[n] --width             Width of encoded output image. [--lumWidthSrc]\n"
            "  -y[n] --height            Height of encoded output image. [--lumHeightSrc]\n"
            "  -X[n] --horOffsetSrc      Output image horizontal cropping offset. [0]\n"
            "  -Y[n] --verOffsetSrc      Output image vertical cropping offset. [0]\n\n");

    fprintf(stdout,
            " Parameters for pre-processing frames before encoding:\n"
            "  -l[n] --inputFormat       Input YUV format. [0]\n"
            "                                0 - YUV420 planar CbCr (IYUV/I420)\n"
            "                                1 - YUV420 semiplanar CbCr (NV12)\n"
            "                                2 - YUV420 semiplanar CrCb (NV21)\n"
            "                                3 - YUYV422 interleaved (YUYV/YUY2)\n"
            "                                4 - UYVY422 interleaved (UYVY/Y422)\n"
            "                                5 - RGB565 16bpp\n"
            "                                6 - BGR565 16bpp\n"
            "                                7 - RGB555 16bpp\n"
            "                                8 - BGR555 16bpp\n"
            "                                9 - RGB444 16bpp\n"
            "                                10 - BGR444 16bpp\n"
            "                                11 - RGB888 32bpp\n"
            "                                12 - BGR888 32bpp\n"
            "                                13 - RGB101010 32bpp\n"
            "                                14 - BGR101010 32bpp\n"
            "  -O[n] --colorConversion   RGB to YCbCr color conversion type. [0]\n"
            "                                0 - BT.601\n"
            "                                1 - BT.709\n"
            "                                2 - User defined, coeffs defined in testbench.\n");

    fprintf(stdout,
            "  -r[n] --rotation          Rotate input image. [0]\n"
            "                                0 - disabled\n"
            "                                1 - 90 degrees right\n"
            "                                2 - 90 degrees left\n"
            "  -Z[n] --videoStab         Enable video stabilization or scene change detection. [0]\n"
            "                                Stabilization works by adjusting cropping offset for\n"
            "                                every frame. Scene change detection encodes scene\n"
            "                                changes as intra frames, it is enabled when\n"
            "                                input resolution == encoded resolution.\n\n");

    fprintf(stdout,
            " Parameters affecting the output stream and encoding tools:\n"
            "        --viewMode          Mode for encoder view prediction and buffering [0]\n"
            "                                0 = Base view stream with two frame buffers\n"
            "                                1 = Base view stream with one frame buffer\n"
            "                                2 = Multiview stream with one frame buffer\n"
            "                                3 = Multiview stream with two frame buffers\n"
            "                                4 = Base view stream with multiple reference frames\n"
            "                                5 = Interlaced field stream\n"
            "  -L[n] --level             10..51, H264 Level. 40=Level 4.0 [40]\n"
            "                                Each level has resolution and bitrate limitations:\n"
            "                                10 - QCIF  (176x144)   75kbits\n"
            "                                20 - CIF   (352x288)   2.4Mbits\n"
            "                                30 - SD    (720x576)   12Mbits\n"
            "                                40 - 1080p (1920x1080) 24Mbits\n"
            "  -R[n] --byteStream        Stream type. [1]\n"
            "                                0 - NAL units. Nal sizes returned in <nal_sizes.txt>\n"
            "                                1 - byte stream according to H.264 Standard Annex B.\n");
    fprintf(stdout,
            "  -S[n] --sei               Enable SEI messages (buffering period + picture timing) [0]\n"
            "                                Writes Supplemental Enhancement Information messages\n"
            "                                containing information about picture time stamps\n"
            "                                and picture buffering into stream.\n");
    fprintf(stdout,
            "  -8[n] --trans8x8          0=OFF, 1=Adaptive, 2=ON [0]\n"
            "                                Adaptive setting enables 8x8 transform for >= 720p.\n"
            "                                Enabling 8x8 transform makes the stream High Profile.\n"
            "  -M[n] --quarterPixelMv    0=OFF, 1=Adaptive, 2=ON [1]\n"
            "                                Adaptive setting disables 1/4p MVs for > 720p.\n"
            "  -K[n] --enableCabac       0=OFF (CAVLC), 1=ON (CABAC),\n"
            "                                2=ON for Inter frames (intra=CAVLC, inter=CABAC) [1]\n"
            "                                Enabling CABAC makes the stream Main Profile.\n"
            "  -p[n] --cabacInitIdc      0..2 Initialization value for CABAC. [0]\n"
            "  -k[n] --videoRange        0..1 Video signal sample range value in H.264 stream. [0]\n"
            "                                0 - Y range in [16..235] Cb,Cr in [16..240]\n"
            "                                1 - Y,Cb,Cr range in [0..255]\n");

    fprintf(stdout,
            "  -T[n] --constIntraPred    0=OFF, 1=ON Constrained intra prediction flag [0]\n"
            "                                Improves stream error recovery by not allowing\n"
            "                                intra MBs to predict from neighboring inter MBs.\n"
            "  -D[n] --disableDeblocking 0..2 Value of disable_deblocking_filter_idc [0]\n"
            "                                0 = Inloop deblocking filter enabled (best quality)\n"
            "                                1 = Inloop deblocking filter disabled\n"
            "                                2 = Inloop deblocking filter disabled on slice edges\n"
            "  -I[n] --intraPicRate      Intra picture rate in frames. [0]\n"
            "                                Forces every Nth frame to be encoded as intra frame.\n"
            "  -V[n] --mbRowPerSlice     Slice size in macroblock rows. [0]\n\n");

    fprintf(stdout,
            " Parameters affecting the rate control and output stream bitrate:\n"
            "  -B[n] --bitPerSecond      10000..levelMax, target bitrate for rate control [1000000]\n"
            "  -U[n] --picRc             0=OFF, 1=ON Picture rate control. [1]\n"
            "                                Calculates new target QP for every frame.\n"
            "  -u[n] --mbRc              0=OFF, 1=ON Macroblock rate control (Check point rc). [1]\n"
            "                                Updates target QP inside frame.\n"
            "  -C[n] --hrdConformance    0=OFF, 1=ON HRD conformance. [0]\n"
            "                                Uses standard defined model to limit bitrate variance.\n"
            "  -c[n] --cpbSize           HRD Coded Picture Buffer size in bits. [0]\n"
            "                                Buffer size used by the HRD model.\n"
            "  -g[n] --gopLength         Group Of Pictures length in frames, 1..300 [intraPicRate]\n"
            "                                Rate control allocates bits for one GOP and tries to\n"
            "                                match the target bitrate at the end of each GOP.\n"
            "                                Typically GOP begins with an intra frame, but this\n"
            "                                is not mandatory.\n");

    fprintf(stdout,
            "  -s[n] --picSkip           0=OFF, 1=ON Picture skip rate control. [0]\n"
            "                                Allows rate control to skip frames if needed.\n"
            "  -q[n] --qpHdr             -1..51, Initial target QP. [26]\n"
            "                                -1 = Encoder calculates initial QP. NOTE: use -q-1\n"
            "                                The initial QP used in the beginning of stream.\n"
            "  -n[n] --qpMin             0..51, Minimum frame header QP. [10]\n"
            "  -m[n] --qpMax             0..51, Maximum frame header QP. [51]\n"
            "  -A[n] --intraQpDelta      -51..51, Intra QP delta. [-3]\n"
            "                                QP difference between target QP and intra frame QP.\n"
            "  -G[n] --fixedIntraQp      0..51, Fixed Intra QP, 0 = disabled. [0]\n"
            "                                Use fixed QP value for every intra frame in stream.\n"
            "  -2[n] --mbQpAdjustment    -8..7, MAD based MB QP adjustment, 0 = disabled. [0]\n"
            "                                Improves quality of macroblocks based on MAD value.\n"
            "  -4[n] --mbQpAutoBoost     0=OFF, 1=ON Motion and variance based MB QP adjustment. [0]\n"
            "  -1[n]:[n] --bpsAdjust     Frame:bitrate for adjusting bitrate on the fly.\n"
            "                                Sets new target bitrate at specific frame.\n\n");

    fprintf(stdout,
            " Other parameters for coding and reporting:\n"
            "  -z[n] --userData          SEI User data file name. File is read and inserted\n"
            "                                as SEI message before first frame.\n"
            "  -d    --psnr              Enables PSNR calculation for each frame.\n"
            "  -3[n] --mvOutput          Enable MV writing in <mv.txt> [0]\n\n");

    fprintf(stdout,
            "        --cir               start:interval for Cyclic Intra Refresh.\n"
            "                                Forces macroblocks in intra mode. [0:0]\n"
            "        --intraSliceMap1    32b hex bitmap for forcing slices 0..31\n"
            "                                coding as intra. LSB=slice0 [0]\n"
            "        --intraSliceMap2    32b hex bitmap for forcing slices 32..63\n"
            "                                coding as intra. LSB=slice32 [0]\n"
            "        --intraSliceMap3    32b hex bitmap for forcing slices 64..95\n"
            "                                coding as intra. LSB=slice64 [0]\n"
            "        --intraArea         left:top:right:bottom macroblock coordinates\n"
            "                                specifying rectangular area of MBs to\n"
            "                                force encoding in intra mode.\n"
            "        --roi1Area          left:top:right:bottom macroblock coordinates\n"
            "        --roi2Area          left:top:right:bottom macroblock coordinates\n"
            "                                specifying rectangular area of MBs as\n"
            "                                Region Of Interest with lower QP.\n"
            "        --roi1DeltaQp       -15..0, QP delta value for ROI 1 MBs. [0]\n"
            "        --roi2DeltaQp       -15..0, QP delta value for ROI 2 MBs. [0]\n"
            "        --adaptiveRoi       -51..0, QP delta value for adaptive ROI. [0]\n"
            "        --adaptiveRoiColor  -10..10 color sensitivity for adaptive ROI. [0]\n"
            "        --scaledWidth       0..width, encoder image down-scaled, 0=disable [0]\n"
            "        --scaledHeight      0..height, encoder image down-scaled, written in scaled.yuv\n"
            "        --interlacedFrame   0=Input progressive field,\n"
            "                            1=Input frame with fields interlaced [0]\n"
            "        --fieldOrder        Interlaced field order, 0=bottom first [0]\n"
            "        --refFrameAmount    1..2 amount of buffered reference frames [1]\n"
            "        --longTermPicRate   Long term picture rate in frames [15]\n");

    fprintf(stdout,
            "\nTesting parameters that are not supported for end-user:\n"
            "  -Q[n] --chromaQpOffset    -12..12 Chroma QP offset. [2]\n"
            "  -W[n] --filterOffsetA     -6..6 Deblocking filter offset A. [0]\n"
            "  -E[n] --filterOffsetB     -6..6 Deblocking filter offset B. [0]\n"
            "  -N[n] --burstSize          0..63 HW bus burst size. [16]\n"
            "  -t[n] --burstType          0=SINGLE, 1=INCR HW bus burst type. [0]\n"
            "  -e[n] --testId            Internal test ID. [0]\n"
            "        --traceRecon        Enable writing sw_recon_internal.yuv. [0]\n"
            "  -P[n] --trigger           Logic Analyzer trigger at picture <n>. [-1]\n"
            "\n");
    ;
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
void WriteStrm(FILE * fout, u32 * strmbuf, u32 size, u32 endian)
{

#ifdef NO_OUTPUT_WRITE
    return;
#endif

    if (!fout || !strmbuf || !size) return;

    /* Swap the stream endianess before writing to file if needed */
    if(endian == 1)
    {
        u32 i = 0, words = (size + 3) / 4;

        while(words)
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

    /* Write the stream to file */
    fwrite(strmbuf, 1, size, fout);
}

/*------------------------------------------------------------------------------

    NextPic

    Function calculates next input picture depending input and output frame
    rates.

    Input   inputRateNumer  (input.yuv) frame rate numerator.
            inputRateDenom  (input.yuv) frame rate denominator
            outputRateNumer (stream.mpeg4) frame rate numerator.
            outputRateDenom (stream.mpeg4) frame rate denominator.
            frameCnt        Frame counter.
            firstPic        The first picture of input.yuv sequence.

    Return  next    The next picture of input.yuv sequence.

------------------------------------------------------------------------------*/
i32 NextPic(i32 inputRateNumer, i32 inputRateDenom, i32 outputRateNumer,
            i32 outputRateDenom, i32 frameCnt, i32 firstPic)
{
    u32 sift;
    u32 skip;
    u32 numer;
    u32 denom;
    u32 next;

    numer = (u32) inputRateNumer *(u32) outputRateDenom;
    denom = (u32) inputRateDenom *(u32) outputRateNumer;

    if(numer >= denom)
    {
        sift = 9;
        do
        {
            sift--;
        }
        while(((numer << sift) >> sift) != numer);
    }
    else
    {
        sift = 17;
        do
        {
            sift--;
        }
        while(((numer << sift) >> sift) != numer);
    }
    skip = (numer << sift) / denom;
    next = (((u32) frameCnt * skip) >> sift) + (u32) firstPic;

    return (i32) next;
}

/*------------------------------------------------------------------------------

    ChangeInput
        Change input file.
    Params:
        argc    - number of arguments to the application
        argv    - argument list as provided to the application
        option  - list of accepted cmdline options

    Returns:
        1 - for success
------------------------------------------------------------------------------*/
int ChangeInput(i32 argc, char **argv, char **name, option_s * option)
{
    i32 ret;
    argument_s argument;
    i32 enable = 0;

#ifdef MULTIFILEINPUT
    /* End when can't open next file */
    if (!yuvFile)
        return 0;

    /* Use the first file name as base: input.yuv */
    if (fileNumber == 0)
    {
        strcpy(inputFilename, *name);
        inputFilenameLength = strlen(inputFilename);
    }

    /* Remove the previous input file, decoder will create next file. */
    remove(inputFilename);

    /* First check and wait until the decoder has created next file */
    {
        FILE * fp = NULL;
        sprintf(inputFilename+inputFilenameLength, "%d", fileNumber+2);
        printf("\nWaiting for file: %s\n", inputFilename);
        while (!fp)
        {
            sleep(1);
            fp = fopen(inputFilename, "r");
            /* End when a file named EOF exists */
            if (!fp)
                fp = fopen("EOF", "r");
        }
        fclose(fp);
    }

    /* The next file should be named: input.yuv1, input.yuv2, etc.. */
    sprintf(inputFilename+inputFilenameLength, "%d", ++fileNumber);
    *name = inputFilename;
    printf("\nNext file: %s\n", *name);
    return 1;
#endif

    argument.optCnt = 1;
    while((ret = EncGetOption(argc, argv, option, &argument)) != -1)
    {
        if((ret == 1) && (enable))
        {
            *name = argument.optArg;
            printf("\nNext file: %s\n", *name);
            return 1;
        }
        if(argument.optArg == *name)
        {
            enable = 1;
        }
    }

    return 0;
}

/*------------------------------------------------------------------------------

    API tracing
        TRacing as defined by the API.
    Params:
        msg - null terminated tracing message
------------------------------------------------------------------------------*/
#if 0
void H264EncTrace(const char *msg)
{
    static FILE *fp = NULL;

    if(fp == NULL)
        fp = fopen("api.trc", "wt");

    if(fp)
        fprintf(fp, "%s\n", msg);
}
#endif
/*------------------------------------------------------------------------------
    Get out pure NAL units from byte stream format (one picture data)
    This is an example!

    Params:
        pNaluSizeBuf - buffer where the individual NAL size are (return by API)
        pStream- buffre containign  the whole picture data
------------------------------------------------------------------------------*/
void NalUnitsFromByteStream(const u32 * pNaluSizeBuf, const u8 * pStream)
{
    u32 nalSize, nal;
    const u8 *pNalBase;

    nal = 0;
    pNalBase = pStream + 4; /* skip the 4-byte startcode */

    while(pNaluSizeBuf[nal] != 0)   /* after the last NAL unit size we have a zero */
    {
        nalSize = pNaluSizeBuf[nal] - 4;

        /* now we have the pure NAL unit, do something with it */
        /* DoSomethingWithThisNAL(pNalBase, nalSize); */

        pNalBase += pNaluSizeBuf[nal];  /* next NAL data base address */
        nal++;  /* next NAL unit */
    }
}

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void PrintTitle(commandLine_s * cml, i32 maLength, i32 qpHdr, i32 streamSize,
                H264EncOut *encOut)
{
    printf("\n In Pic  QP  Type IP %s|   BR avg    MA(%3d) | ByteCnt (inst) |",
            cml->refFrameAmount > 1 ? "LT " : "", maLength);
    if (cml->psnr)
        printf(" PSNR  | NALU sizes\n");
    else
        printf(" NALU sizes\n");

    printf("--------------------------------------------------------------------------------\n");
    printf("         %2d  HDR  %s |                     | %7i %6i | ",
            qpHdr, cml->refFrameAmount > 1 ? "     " : "  ", streamSize, encOut->streamSize);
    if (cml->psnr)
        printf("      | ");
    PrintNalSizes(encOut->pNaluSizeBuf, (u8 *) outbufMem.virtualAddress,
            encOut->streamSize, cml->byteStream);
    printf("\n");
}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
u32 PrintFrame(H264EncInst encoder, H264EncRet ret, i32 next, u32 frameCntTotal,
        i32 codedFrameCnt, i32 qpHdr, commandLine_s * cml, H264EncOut *encOut,
        i32 bitrate, i32 movingAverageBitrate, i32 streamSize)
{
    u32 psnr = 0;
    char *type = "skip";

    if (ret == H264ENC_OUTPUT_BUFFER_OVERFLOW) type = "lost";
    else if (cml->viewMode == H264ENC_INTERLACED_FIELD)
    {
        if (encOut->codingType == H264ENC_INTRA_FRAME) {
            if (codedFrameCnt%2 == cml->fieldOrder) type = "I-bf";
            else type = "I-tf"; /* Intra top-field */
        } else if (encOut->codingType == H264ENC_NONIDR_INTRA_FRAME) {
            if (codedFrameCnt%2 == cml->fieldOrder) type = "i-bf";
            else type = "i-tf";
        } else if (encOut->codingType == H264ENC_PREDICTED_FRAME) {
            if (codedFrameCnt%2 == cml->fieldOrder) type = "P-bf";
            else type = "P-tf";
        }
    }
    else if (cml->viewMode == H264ENC_MVC_STEREO_INTER_PRED ||
             cml->viewMode == H264ENC_MVC_STEREO_INTER_VIEW_PRED)
    {
        if (encOut->codingType == H264ENC_INTRA_FRAME) {
            if (codedFrameCnt%2) type = "I-sv";     /* Intra stereo-view */
            else type = "I-bv";                     /* Intra base-view */
        } else if (encOut->codingType == H264ENC_NONIDR_INTRA_FRAME) {
            if (codedFrameCnt%2) type = "i-sv";
            else type = "i-bv";
        } else if (encOut->codingType == H264ENC_PREDICTED_FRAME) {
            if (codedFrameCnt%2) type = "P-sv";
            else type = "P-bv";
        }
    }
    else if (encOut->codingType == H264ENC_INTRA_FRAME) type = " I  ";
    else if (encOut->codingType == H264ENC_NONIDR_INTRA_FRAME) type = " i  ";
    else if (encOut->codingType == H264ENC_PREDICTED_FRAME) type = " P  ";

    printf("%3i %3i  %2i %s  ", next, frameCntTotal, qpHdr, type);

    /* Print reference frame usage */
    printf("%c%c",
        encOut->ipf & H264ENC_REFERENCE ? 'R' : '-',
        encOut->ipf & H264ENC_REFRESH   ? 'W' : '-');
    if (cml->refFrameAmount >= 2) printf("-%c%c",
        encOut->ltrf & H264ENC_REFERENCE ? 'R' : '-',
        encOut->ltrf & H264ENC_REFRESH   ? 'W' : '-');

    printf(" | %9u %9u | %7i %6i | ",
        bitrate, movingAverageBitrate, streamSize, encOut->streamSize);

    /* This works only with system model, bases are pointers. */
    if (cml->psnr)
        psnr = PrintPSNR((u8 *)
            (((h264Instance_s *)encoder)->asic.regs.inputLumBase +
            ((h264Instance_s *)encoder)->asic.regs.inputLumaBaseOffset),
            (u8 *)
            (((h264Instance_s *)encoder)->asic.regs.internalImageLumBaseR[0]),
            cml->lumWidthSrc, cml->width, cml->height, cml->rotation);

    PrintNalSizes(encOut->pNaluSizeBuf, (u8 *) outbufMem.virtualAddress,
        encOut->streamSize, cml->byteStream);
    printf("\n");

    return psnr;
}

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void PrintNalSizes(const u32 *pNaluSizeBuf, const u8 *pOutBuf, u32 strmSize, 
        i32 byteStream)
{
    u32 nalu = 0, naluSum = 0;

    /* Step through the NALU size buffer */
    while(pNaluSizeBuf && pNaluSizeBuf[nalu] != 0) 
    {
#ifdef INTERNAL_TEST
        /* In byte stream format each NAL must start with 
         * start code: any number of leading zeros + 0x000001 */
        if (byteStream) {
            int zero_count = 0;
            const u8 *pTmp = pOutBuf + naluSum;

            /* count zeros, shall be at least 2 */
            while(*pTmp++ == 0x00)
                zero_count++;

            if(zero_count < 2 || pTmp[-1] != 0x01)
                printf
                    ("Error: NAL unit %d at %d doesn't start with '00 00 01'  ",
                     nalu, naluSum);
        }
#endif

        naluSum += pNaluSizeBuf[nalu];
        printf("%d  ", pNaluSizeBuf[nalu++]);
    }

    /* NAL sum must be the same as the whole frame stream size */
    if (naluSum && naluSum != strmSize)
        printf("Error: NAL size sum (%d) does not match stream size",
                naluSum);
}

/*------------------------------------------------------------------------------
    WriteNalSizesToFile    
        Dump NAL size to a file
    
    Params:
        file         - file name where toi dump
        pNaluSizeBuf - buffer where the individual NAL size are (return by API)
        numNalus     - amount of NAL units in the above buffer
------------------------------------------------------------------------------*/
void WriteNalSizesToFile(const char *file, const u32 * pNaluSizeBuf,
                         u32 numNalus)
{
    FILE *fo;
    u32 offset = 0;

    fo = fopen(file, "at");

    if(fo == NULL) {
        printf("FAILED to open NAL size tracing file <%s>\n", file);
        return;
    }

    while(offset++ < numNalus && *pNaluSizeBuf != 0) {
        fprintf(fo, "%d\n", *pNaluSizeBuf++);
    }

    fclose(fo);
}

/*------------------------------------------------------------------------------
    WriteMotionVectors

        Write motion vector and HW output data into a file. Format is defined
        by encOutputMbInfo_s struct, but it relies that HW output endianess
        is configured the same way as compiler constructs the struct.

------------------------------------------------------------------------------*/
void WriteMotionVectors(FILE *file, H264EncInst encoder, i32 frame, i32 width,
                        i32 height)
{
    i32 i;
    i32 mbPerRow = (width+15)/16;
    i32 mbPerCol = (height+15)/16;
    i32 mbPerFrame = mbPerRow * mbPerCol;
    encOutputMbInfo_s *mbInfo;
    H264EncOut encOut;
    H264EncBuild encBuild;

    encBuild = H264EncGetBuild();

    /* We rely on the compiler to create the struct similar to
       ASIC output configuration. */

    if (!file)
        return;

    /* Print first motion vector in pixel accuracy for every macroblock. */
    fprintf(file, "\npic=%d  MV full-pixel X,Y "
            "for %d macroblocks (%dx%d) block=0\n",
            frame, mbPerFrame, mbPerRow, mbPerCol);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, "  %3d,%3d", mbInfo->mvX[0]/4, mbInfo->mvY[0]/4);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print best inter SAD for every macroblock. */
    fprintf(file, "\npic=%d  inter-SAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->interSad);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print best intra SAD for every macroblock. */
    fprintf(file, "\npic=%d  intra-SAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->intraSad);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print input MAD for every macroblock. HW output is MAD div 128. */
    fprintf(file, "\npic=%d  input-MAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->inputMad_div128*128);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print bit count for every macroblock. */
    fprintf(file, "\npic=%d  bit-count\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %4d", mbInfo->bitCount);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print non-zero block count for every macroblock. */
    fprintf(file, "\npic=%d  non-zero-blocks\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %2d", mbInfo->nonZeroCnt);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print MB mode for every macroblock. */
    fprintf(file, "\npic=%d  coding-mode, 0=I16x16_V, .. 4=I4x4,"
                  " 6=P16x16, 7=P16x8, 8=P8x16, 9=P8x8 \n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %2d", mbInfo->mode & MBOUT_8_MODE_MASK);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print I4x4 modes for every macroblock. */
    fprintf(file, "\npic=%d  I4x4-modes\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file," %1d%1d", mbInfo->intra4x4[0]>>4, mbInfo->intra4x4[0]&0xF);
        fprintf(file, "%1d%1d", mbInfo->intra4x4[1]>>4, mbInfo->intra4x4[1]&0xF);
        fprintf(file, "%1d%1d", mbInfo->intra4x4[2]>>4, mbInfo->intra4x4[2]&0xF);
        fprintf(file, "%1d%1d", mbInfo->intra4x4[3]>>4, mbInfo->intra4x4[3]&0xF);
        fprintf(file, "%1d%1d", mbInfo->intra4x4[4]>>4, mbInfo->intra4x4[4]&0xF);
        fprintf(file, "%1d%1d", mbInfo->intra4x4[5]>>4, mbInfo->intra4x4[5]&0xF);
        fprintf(file, "%1d%1d", mbInfo->intra4x4[6]>>4, mbInfo->intra4x4[6]&0xF);
        fprintf(file, "%1d%1d", mbInfo->intra4x4[7]>>4, mbInfo->intra4x4[7]&0xF);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print luma mean for every macroblock. */
    fprintf(file, "\npic=%d  luma-mean\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->yMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print Cb mean for every macroblock. */
    fprintf(file, "\npic=%d  Cb-mean\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->cbMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print Cr mean for every macroblock. */
    fprintf(file, "\npic=%d  Cr-mean\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->crMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    if (encBuild.hwBuild <= ASIC_ID_EVERGREEN) return;

    /* Print boostQp for every macroblock. */
    fprintf(file, "\npic=%d  boostQp\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->boostQp);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print input and reconstructed luma variance for every macroblock. */
    fprintf(file, "\npic=%d  luma-variance (input, recon)\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d,%3d", mbInfo->varInput[0], mbInfo->varRecon[0]);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    fprintf(file, "\npic=%d  Cb-variance (input, recon)\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d,%3d", mbInfo->varInput[1], mbInfo->varRecon[1]);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    fprintf(file, "\npic=%d  Cr-variance (input, recon)\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        H264EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d,%3d", mbInfo->varInput[2], mbInfo->varRecon[2]);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }
}

/*------------------------------------------------------------------------------
    PrintErrorValue
        Print return error value
------------------------------------------------------------------------------*/
void PrintErrorValue(const char *errorDesc, u32 retVal)
{
    char * str;

    switch (retVal)
    {
        case H264ENC_ERROR: str = "H264ENC_ERROR"; break;
        case H264ENC_NULL_ARGUMENT: str = "H264ENC_NULL_ARGUMENT"; break;
        case H264ENC_INVALID_ARGUMENT: str = "H264ENC_INVALID_ARGUMENT"; break;
        case H264ENC_MEMORY_ERROR: str = "H264ENC_MEMORY_ERROR"; break;
        case H264ENC_EWL_ERROR: str = "H264ENC_EWL_ERROR"; break;
        case H264ENC_EWL_MEMORY_ERROR: str = "H264ENC_EWL_MEMORY_ERROR"; break;
        case H264ENC_INVALID_STATUS: str = "H264ENC_INVALID_STATUS"; break;
        case H264ENC_OUTPUT_BUFFER_OVERFLOW: str = "H264ENC_OUTPUT_BUFFER_OVERFLOW"; break;
        case H264ENC_HW_BUS_ERROR: str = "H264ENC_HW_BUS_ERROR"; break;
        case H264ENC_HW_DATA_ERROR: str = "H264ENC_HW_DATA_ERROR"; break;
        case H264ENC_HW_TIMEOUT: str = "H264ENC_HW_TIMEOUT"; break;
        case H264ENC_HW_RESERVED: str = "H264ENC_HW_RESERVED"; break;
        case H264ENC_SYSTEM_ERROR: str = "H264ENC_SYSTEM_ERROR"; break;
        case H264ENC_INSTANCE_ERROR: str = "H264ENC_INSTANCE_ERROR"; break;
        case H264ENC_HRD_ERROR: str = "H264ENC_HRD_ERROR"; break;
        case H264ENC_HW_RESET: str = "H264ENC_HW_RESET"; break;
        default: str = "UNDEFINED";
    }

    fprintf(H264ERR_OUTPUT, "%s Return value: %s\n", errorDesc, str);
}

/*------------------------------------------------------------------------------
    PrintPSNR
        Calculate and print frame PSNR
------------------------------------------------------------------------------*/
u32 PrintPSNR(u8 *a, u8 *b, i32 scanline, i32 wdh, i32 hgt, i32 rotation)
{
#ifdef PSNR
        float mse = 0.0;
        u32 tmp, i, j;

        if (a && b && !rotation)
        {
            for (j = 0 ; j < hgt; j++) {
                    for (i = 0; i < wdh; i++) {
                            tmp = a[i] - b[i];
                            tmp *= tmp;
                            mse += tmp;
                    }
                    a += scanline;
                    b += wdh;
            }
            mse /= wdh * hgt;
        }

        if (mse == 0.0) {
                printf("--.-- | ");
        } else {
                mse = 10.0 * log10f(65025.0 / mse);
                printf("%5.2f | ", mse);
        }

        return (u32)roundf(mse*100);
#else
        printf("xx.xx | ");
        return 0;
#endif
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
    for (i = len-1; i; --i)
        if (filename[i] == '/') {
            filenameBegin = i+1;
            break;
        }

    /* If '/' found, it separates trailing path from file name */
    for (i = filenameBegin; i <= len-3; ++i)
    {
        if ((strncmp(filename+i, "subqcif", 7) == 0) ||
            (strncmp(filename+i, "sqcif", 5) == 0))
        {
            *pWidth = 128;
            *pHeight = 96;
            printf("Detected resolution SubQCIF (128x96) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "qcif", 4) == 0)
        {
            *pWidth = 176;
            *pHeight = 144;
            printf("Detected resolution QCIF (176x144) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "4cif", 4) == 0)
        {
            *pWidth = 704;
            *pHeight = 576;
            printf("Detected resolution 4CIF (704x576) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "cif", 3) == 0)
        {
            *pWidth = 352;
            *pHeight = 288;
            printf("Detected resolution CIF (352x288) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "qqvga", 5) == 0)
        {
            *pWidth = 160;
            *pHeight = 120;
            printf("Detected resolution QQVGA (160x120) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "qvga", 4) == 0)
        {
            *pWidth = 320;
            *pHeight = 240;
            printf("Detected resolution QVGA (320x240) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "vga", 3) == 0)
        {
            *pWidth = 640;
            *pHeight = 480;
            printf("Detected resolution VGA (640x480) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "720p", 4) == 0)
        {
            *pWidth = 1280;
            *pHeight = 720;
            printf("Detected resolution 720p (1280x720) from file name.\n");
            return 0;
        }
        if (strncmp(filename+i, "1080p", 5) == 0)
        {
            *pWidth = 1920;
            *pHeight = 1080;
            printf("Detected resolution 1080p (1920x1080) from file name.\n");
            return 0;
        }
        if (filename[i] == 'x')
        {
            if (sscanf(filename+i-4, "%ux%u", &w, &h) == 2)
            {
                *pWidth = w;
                *pHeight = h;
                printf("Detected resolution %dx%d from file name.\n", w, h);
                return 0;
            }
            else if (sscanf(filename+i-3, "%ux%u", &w, &h) == 2)
            {
                *pWidth = w;
                *pHeight = h;
                printf("Detected resolution %dx%d from file name.\n", w, h);
                return 0;
            }
            else if (sscanf(filename+i-2, "%ux%u", &w, &h) == 2)
            {
                *pWidth = w;
                *pHeight = h;
                printf("Detected resolution %dx%d from file name.\n", w, h);
                return 0;
            }
        }
        if (filename[i] == 'w')
        {
            if (sscanf(filename+i, "w%uh%u", &w, &h) == 2)
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

    sum = sum / ma->length;

    return sum * ma->frameRateNumer / ma->frameRateDenom;
}

/*------------------------------------------------------------------------------
    Callback function called by the encoder SW after "slice ready"
    interrupt from HW. Note that this function is not necessarily called
    after every slice i.e. it is possible that two or more slices are
    completed between callbacks. The callback is not called for the last slice.
------------------------------------------------------------------------------*/
void H264SliceReady(H264EncSliceReady *slice)
{
    /* Here is possible to implement low-latency streaming by
     * sending the complete slices before the whole frame is completed. */
#if 0
    /* This is an example how to access the slices, but it will mess up prints */
    {
        i32 i;
        u8 *strmPtr;

        if (slice->slicesReadyPrev == 0)    /* New frame */
            strmPtr = (u8*)slice->pOutBuf;  /* Pointer to beginning of frame */
        else
            strmPtr = (u8*)slice->pAppData; /* Here we store the slice pointer */

        for (i = slice->slicesReadyPrev; i < slice->slicesReady; i++)
        {
            printf("#%d:%p:%d\n", i, strmPtr, slice->sliceSizes[i]);
            strmPtr += slice->sliceSizes[i];
        }

        /* Store the slice pointer for next callback */
        slice->pAppData = strmPtr;
    }
#endif
}

