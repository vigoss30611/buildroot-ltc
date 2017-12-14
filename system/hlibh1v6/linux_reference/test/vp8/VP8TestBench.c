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
--  Abstract : VP8 Encoder testbench for linux
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

/* For parameter parsing */
#include "EncGetOption.h"

/* For SW/HW shared memory allocation */
#include "ewl.h"

/* For accessing the EWL instance inside the encoder */
#include "vp8instance.h"

/* For compiler flags, test data, debug and tracing */
#include "enccommon.h"

/* For Hantro VP8 encoder */
#include "vp8encapi.h"

/* For printing and file IO */
#include <stdio.h>
#include <stddef.h>

/* For dynamic memory allocation */
#include <stdlib.h>

/* For memset, strcpy and strlen */
#include <string.h>

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

#define VP8ERR_OUTPUT stdout

/* The maximum amount of frames for bitrate moving average calculation */
#define MOVING_AVERAGE_FRAMES    30

/* Value for parameter to use API default */
#define DEFAULT -255

/* Intermediate Video File Format */
#define IVF_HDR_BYTES       32
#define IVF_FRM_BYTES       12

#define MAX_BPS_ADJUST 20

/* Map HW ID to named release, SW is backwards compatible. */
u32 hwId[] = {  ASIC_ID_BLUEBERRY, ASIC_ID_CLOUDBERRY, ASIC_ID_DRAGONFLY,
                ASIC_ID_EVERGREEN, ASIC_ID_EVERGREEN_PLUS, ASIC_ID_FOXTAIL };
char * hwIdName[] = {"Blueberry", "Cloudberry", "Dragonfly", "Evergreen",
                     "Evergreen+", "Foxtail"};

/* For temporal layers [0..3] the layer ID and reference picture usage 
 * (VP8EncRefPictureMode) for [amount of layers][pic idx] */
u32 temporalLayerId[4][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0 },  /* 1 layer == base layer only */
  {0, 1, 0, 0, 0, 0, 0, 0 },  /* 2 layers, pattern length = 2 */
  {0, 2, 1, 2, 0, 0, 0, 0 },  /* 3 layers, pattern length = 4 */
  {0, 3, 2, 3, 1, 3, 2, 3 }   /* 4 layers, pattern length = 8 */
};

u32 temporalLayerIpf[4][8] = {
  {3, 0, 0, 0, 0, 0, 0, 0 },
  {3, 1, 0, 0, 0, 0, 0, 0 },
  {3, 1, 1, 0, 0, 0, 0, 0 },
  {3, 1, 1, 0, 1, 0, 0, 0 }
};

u32 temporalLayerGrf[4][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 2, 1, 0, 0, 0, 0 },
  {0, 0, 0, 0, 2, 1, 1, 0 }
};

u32 temporalLayerArf[4][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 0, 0, 0, 0, 0, 0 },
  {0, 0, 2, 1, 0, 0, 2, 1 }
};

u32 temporalLayerTicScale[4][4] = {
  { 1, 0, 0, 0 },
  { 2, 2, 0, 0 },
  { 4, 4, 2, 0 },
  { 8, 8, 4, 2 }
};

#ifdef PSNR
    float log10f(float x);
    float roundf(float x);
#endif

typedef struct {
    i32 frame[MOVING_AVERAGE_FRAMES];
    i32 length;
    i32 count;
    i32 pos;
    i32 frameRateNumer;
    i32 frameRateDenom;
} ma_s;

/* Structure for command line options and testbench variables */
typedef struct
{
    char *input;
    char *output;
    i32 firstPic;
    i32 lastPic;
    i32 width;
    i32 height;
    i32 lumWidthSrc;
    i32 lumHeightSrc;
    i32 horOffsetSrc;
    i32 verOffsetSrc;
    i32 outputRateNumer;
    i32 outputRateDenom;
    i32 inputRateNumer;
    i32 inputRateDenom;
    i32 refFrameAmount;
    i32 scaledWidth;
    i32 scaledHeight;

    i32 inputFormat;
    i32 rotation;
    i32 colorConversion;
    i32 videoStab;

    i32 bitPerSecond;
    i32 layerBitPerSecond[4];
    i32 temporalLayers;
    i32 picRc;
    i32 picSkip;
    i32 gopLength;
    i32 qpHdr;
    i32 qpMin;
    i32 qpMax;
    i32 intraQpDelta;
    i32 fixedIntraQp;
    i32 intraPicRate;
    i32 goldenPictureRate;
    i32 altrefPictureRate;
    i32 bpsAdjustFrame[MAX_BPS_ADJUST];
    i32 bpsAdjustBitrate[MAX_BPS_ADJUST];
    i32 goldenPictureBoost;

    i32 dctPartitions;  /* Dct data partitions 0=1, 1=2, 2=4, 3=8 */
    i32 errorResilient;
    i32 ipolFilter;
    i32 quarterPixelMv;
    i32 splitMv;
    i32 deadzone;
    i32 adaptiveRoiColor;
    i32 adaptiveRoi;
    i32 filterType;
    i32 filterLevel;
    i32 filterSharpness;
    i32 cirStart;
    i32 cirInterval;
    i32 intraAreaLeft;
    i32 intraAreaTop;
    i32 intraAreaRight;
    i32 intraAreaBottom;
    i32 roi1AreaTop;
    i32 roi1AreaLeft;
    i32 roi1AreaBottom;
    i32 roi1AreaRight;
    i32 roi2AreaTop;
    i32 roi2AreaLeft;
    i32 roi2AreaBottom;
    i32 roi2AreaRight;
    i32 roi1DeltaQp;
    i32 roi2DeltaQp;

    i32 loopInput;
    i32 printPsnr;
    i32 mvOutput;
    i32 testId;
    i32 droppable;

    u32 intra16Favor;
    u32 intraPenalty;
    i32 traceRecon;
    i32 traceMbTiming;
    
    i32 adaptiveGoldenBoost;
    i32 adaptiveGoldenUpdate;
    i32 maxNumPasses;
    i32 qualityMetric;
    i32 qpDelta[5];

    /* SW/HW shared memories for input/output buffers */
    EWLLinearMem_t pictureMem;
    EWLLinearMem_t pictureStabMem;
    EWLLinearMem_t outbufMem;
    i32 outBufSizeMax;

    FILE *yuvFile;
    char *yuvFileName;
    off_t file_size;

    VP8EncRateCtrl rc;
    VP8EncOut encOut;

    u32 streamSize;     /* Size of output stream in bytes */
    u32 bitrate;        /* Calculate average bitrate of encoded frames */
    ma_s ma;            /* Calculate moving average of bitrate */
    u32 psnrSum;        /* Calculate average PSNR over encoded frames */
    u32 psnrCnt;

    i32 frameCnt;       /* Frame counter of current input file */
    u64 frameCntTotal;  /* Frame counter of all encoded frames */
    u64 inputFrameTotal;  /* Frame counter of all input frames */
} testbench_s;


/* Global variables */

/* Default input and output file names */
static char input[] = "input.yuv";
static char output[] = "stream.ivf";

/* Command line option definitions */
static option_s option[] = {
    {"help",            'H', 0, "Print this help."},
    {"firstPic",        'a', 1, "First picture of input file to encode. [0]"},
    {"lastPic",         'b', 1, "Last picture of input file to encode. [100]"},
    {"width",           'x', 1, "Width of encoded output image. [--lumWidthSrc]"},
    {"height",          'y', 1, "Height of encoded output image. [--lumHeightSrc]"},
    {"lumWidthSrc",     'w', 1, "Width of source image. [176]"},
    {"lumHeightSrc",    'h', 1, "Height of source image. [144]"},
    {"horOffsetSrc",    'X', 1, "Output image horizontal cropping offset. [0]"},
    {"verOffsetSrc",    'Y', 1, "Output image vertical cropping offset. [0]"},
    {"outputRateNumer", 'f', 1, "1..1048575 Output picture rate numerator. [--inputRateNumer]"},
    {"outputRateDenom", 'F', 1, "1..1048575 Output picture rate denominator. [--inputRateDenom]"},
    {"inputRateNumer",  'j', 1, "1..1048575 Input picture rate numerator. [30]"},
    {"inputRateDenom",  'J', 1, "1..1048575 Input picture rate denominator. [1]"},
    {"refFrameAmount",  'C', 1, "1..3 Amount of buffered reference frames. [1]"},
    {"inputFormat",     'l', 1, "Input YUV format. [0]"},
    {"input",           'i', 1, "Read input video sequence from file. [input.yuv]"},
    {"output",          'o', 1, "Write output VP8 stream to file. [stream.ivf]"},

    {"colorConversion", 'O', 1, "RGB to YCbCr color conversion type. [0]"},
    {"rotation",        'r', 1, "Rotate input image. [0]"},
    {"videoStab",       'Z', 1, "Enable video stabilization or scene change detection. [0]"},

    {"intraPicRate",    'I', 1, "Intra picture rate in frames. [0]"},
    {"goldenPicRate",   'k', 1, "Golden picture rate in frames. [15]"},
    {"altrefPicRate",   'u', 1, "Altref picture rate in frames. [0]"},
    {"dctPartitions",   'd', 1, "0=1, 1=2, 2=4, 3=8, Amount of DCT partitions to create. [0]"},
    {"errorResilient",  'R', 1, "Enable error resilient stream mode. [0]"},
    {"ipolFilter",      'P', 1, "0=Bicubic, 1=Bilinear, 2=None, Interpolation filter mode. [0]"},
    {"quarterPixelMv",  'M', 1, "0=OFF, 1=Adaptive, 2=ON, use 1/4 pixel MVs. [1]"},
    {"splitMv",         'N', 1, "0=OFF, 1=Adaptive, 2=ON, allowed to to use more than 1 MV/MB. [1]"},
    {"deadzone",        'z', 1, "0=OFF, 1=ON, deadzone optimization for trailing ones. [1]"},
    {"filterType",      'D', 1, "0=Normal, 1=Simple, Type of in-loop deblocking filter. [0]"},
    {"filterLevel",     'W', 1, "0..64, 64=auto, Filter strength level for deblocking. [64]"},
    {"filterSharpness", 'E', 1, "0..8, 8=auto, Filter sharpness for deblocking. [8]"},
    {"cir",             'c', 1, "start:interval for Cyclic Intra Refresh, forces MBs intra."},
    {"intraArea",       't', 1, "left:top:right:bottom macroblock coordinates"},
    {"roi1Area",        'K', 1, "left:top:right:bottom macroblock coordinates"},
    {"roi2Area",        'L', 1, "left:top:right:bottom macroblock coordinates"},
    {"roi1DeltaQp",     'Q', 1, "-127..0 QP delta value for 1st Region-Of-Interest. [0]"},
    {"roi2DeltaQp",     'S', 1, "-127..0 QP delta value for 2nd Region-Of-Interest. [0]"},

    {"bitPerSecond",    'B', 1, "10000..60000000, Target bitrate for rate control [1000000]"},
    {"picRc",           'U', 1, "0=OFF, 1=ON, Picture rate control enable. [1]"},
    {"picSkip",         's', 1, "0=OFF, 1=ON, Picture skip rate control. [0]"},
    {"gopLength",       'g', 1, "1..300, Group Of Pictures length in frames. [--intraPicRate]"},
    {"qpHdr",           'q', 1, "-1..127, Initial QP used for the first frame. [-1]"},
    {"qpMin",           'n', 1, "0..127, Minimum frame header QP. [10]"},
    {"qpMax",           'm', 1, "0..127, Maximum frame header QP. [51]"},
    {"intraQpDelta",    'A', 1, "-50..50, Intra QP delta. [0]"},
    {"fixedIntraQp",    'G', 1, "0..127, Fixed Intra QP, 0 = disabled. [0]"},
    {"bpsAdjust",       'V', 1, "frame:bitrate for adjusting bitrate between frames."},

    {"psnr",            'p', 0, "Enables PSNR calculation for each frame."},

    {"mvOutput",        'v', 1, "Enable MV writing in <mv.txt> [0]"},
    {"testId",          'e', 1, "Internal test ID. [0]"},
    {"traceFileConfig", 'T', 1, "File holding names of trace files to create. []"},

    /* All the following share the short option '0', long option must be used */
    {"loopInput"    ,       '0', 0, "Enable looping of input file."},
    {"firstTracePic",       '0', 1, "First picture for tracing. [0]"},
    {"outBufSizeMax",       '0', 1, "Maximum value for output buffer size in megs. [8]"},
    {"intra16Favor",        '0', 1, "Favor value for I16x16 mode in I16/I4"},
    {"intraPenalty",        '0', 1, "Penalty value for intra mode in intra/inter"},
    {"traceRecon",          '0', 0, "Enable writing sw_recon_internal.yuv"},
    {"traceMbTiming",       '0', 0, "Enable MB timing info writing to mbtiming.trc"},
    {"droppable",           '0', 1, "Code all odd frames as droppable. [0]"},
    {"goldenPictureBoost",  '0', 1, " 0..100 Golden picture quality boost. 0=disable [0]"},
    {"adaptiveGoldenBoost", '0', 1, " 0..100 Adaptive golden picture quality boost. 0=disable [25]"},
    {"adaptiveGoldenUpdate",'0', 1, " Enable adaptive golden update [1]"},
    {"maxNumPasses",        '0', 1, "1..n Maximum number of passes to run per frame. [1]"},
    {"qpDeltaChDc",         '0', 1, "-15..16 QP Delta for chroma DC. 16=adaptive [0]"},
    {"qpDeltaChAc",         '0', 1, "-15..16 QP Delta for chroma AC. 16=adaptive [0]"},
    {"adaptiveRoiColor",    '0', 1, " -10..10 color sensitivity for adaptive ROI. [0]"},
    {"adaptiveRoi",         '0', 1, "-127..0 QP delta for adaptive ROI. 0=disable [0]"},

    {"tune",                '0', 1, "Tune encoder towards specified quality metric. Valid values: psnr, ssim. [ssim]"},
    {"scaledWidth",         '0', 1, "Width of scaled encoder image. 0=disable scaling. [0]"},
    {"scaledHeight",        '0', 1, "Height of scaled encoder image. [0]"},
    {"config",              '0', 0, "Prints the hardware config register."},
    {"layerBitPerSecond0",  '0', 1, "0..10000000, Enables temporal layers. Target bitrate for base layer [0]"},
    {"layerBitPerSecond1",  '0', 1, "0..10000000, Target bitrate for layer 1 [0]"},
    {"layerBitPerSecond2",  '0', 1, "0..10000000, Target bitrate for layer 2 [0]"},
    {"layerBitPerSecond3",  '0', 1, "0..10000000, Target bitrate for layer 3 [0]"},
    {0, 0, 0, 0}    /* End of options */
};

#ifdef MULTIFILEINPUT
/* Try to read input from input.yuv, input.yuv1, input.yuv2, etc.. */
static i32 fileNumber=0;
static char inputFilename[256];
static i32 inputFilenameLength;
#endif

/* External variable for passing the test data trace files to system model */
extern char * H1EncTraceFileConfig;
extern int H1EncTraceFirstFrame;

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static int AllocRes(testbench_s * tb, VP8EncInst enc);
static void FreeRes(testbench_s * tb, VP8EncInst enc);

static int OpenEncoder(testbench_s * tb, VP8EncInst * encoder);
static i32 Encode(i32 argc, char **argv, VP8EncInst inst, testbench_s *tb);
static void CloseEncoder(VP8EncInst encoder);

static void Help(void);
static int Parameter(i32 argc, char **argv, testbench_s * tb);
static i32 NextPic(i32 inputRateNumer, i32 inputRateDenom, i32 outputRateNumer,
                    i32 outputRateDenom, i32 frameCnt, i32 firstPic);
static int ReadPic(testbench_s * cml, u8 * image, i32 size, i32 nro);
static int ChangeInput(i32 argc, char **argv, testbench_s * cml, option_s * option);
static void IvfHeader(testbench_s *tb, FILE *fout);
static void IvfFrame(testbench_s *tb, FILE *fout);
static void WriteStrm(FILE * fout, u32 * outbuf, u32 size, u32 endian);
static void WriteStrmLayer(testbench_s *tb, u32 layerId);
static void WriteMotionVectors(FILE *file, VP8EncInst encoder, i32 frame,
                               i32 width, i32 height);
static void WriteMotionVectorsDebug(FILE *file, VP8EncInst encoder, i32 frame,
                                    i32 width, i32 height);
static u32 GetResolution(char *filename, i32 *pWidth, i32 *pHeight);
static void PrintTitle(testbench_s *cml);
static void PrintFrame(testbench_s *cml, VP8EncInst encoder, u32 frameNumber,
        VP8EncRet ret);
static void PrintErrorValue(const char *errorDesc, u32 retVal);
static u32 PrintPSNR(u8 *a, u8 *b, i32 scanline, i32 wdh, i32 hgt,
        i32 rotation, u32 mse);

static void MaAddFrame(ma_s *ma, i32 frameSizeBits);
static i32 Ma(ma_s *ma);


/*------------------------------------------------------------------------------

    main

------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    VP8EncInst encoder;
    testbench_s cml = {0};
    VP8EncApiVersion apiVer;
    VP8EncBuild encBuild;
    int ret;
    i32 i=0;

    apiVer = VP8EncGetApiVersion();
    encBuild = VP8EncGetBuild();

    fprintf(stdout, "VP8 Encoder API version %d.%d\n", apiVer.major,
            apiVer.minor);
    fprintf(stdout, "HW ID: %c%c 0x%08x\t SW Build: %u.%u.%u\n",
            encBuild.hwBuild>>24, (encBuild.hwBuild>>16)&0xff,
            encBuild.hwBuild, encBuild.swBuild / 1000000,
            (encBuild.swBuild / 1000) % 1000, encBuild.swBuild % 1000);
    while (i < sizeof(hwId) && encBuild.hwBuild != hwId[i]) i++;
    if (i < sizeof(hwId)) fprintf(stdout, "HW version %s\n\n", hwIdName[i]);
#ifdef EVALUATION_LIMIT
    fprintf(stdout, "Evaluation version with %d frame limit.\n\n", EVALUATION_LIMIT);
#endif

    if(argc < 2)
    {
        Help();
        exit(0);
    }

    /* Parse command line parameters */
    if(Parameter(argc, argv, &cml) != 0)
    {
        fprintf(VP8ERR_OUTPUT, "Input parameter error\n");
        return -1;
    }

    /* Check that input file exists */
    cml.yuvFile = fopen(cml.input, "rb");

    if(cml.yuvFile == NULL)
    {
        fprintf(VP8ERR_OUTPUT, "Unable to open input file: %s\n", cml.input);
        return -1;
    }
    else
    {
        fclose(cml.yuvFile);
        cml.yuvFile = NULL;
    }

    /* Encoder initialization */
    if((ret = OpenEncoder(&cml, &encoder)) != 0)
    {
        return -ret;    /* Return positive value for test scripts */
    }

    /* Set the test ID for internal testing,
     * the SW must be compiled with testing flags */
    VP8EncSetTestId(encoder, cml.testId);

    /* Allocate input and output buffers */
    if(AllocRes(&cml, encoder) != 0)
    {
        fprintf(VP8ERR_OUTPUT, "Failed to allocate the external resources!\n");
        FreeRes(&cml, encoder);
        CloseEncoder(encoder);
        return -VP8ENC_MEMORY_ERROR;
    }

    ret = Encode(argc, argv, encoder, &cml);

    FreeRes(&cml, encoder);

    CloseEncoder(encoder);

    return ret;
}

void SetFrameInternalTest(VP8EncInst encoder, testbench_s *cml)
{
    /* Set some values outside the product API for internal testing purposes. */

#ifdef INTERNAL_TEST
    if (cml->intra16Favor)
        ((vp8Instance_s *)encoder)->asic.regs.pen[0][ASIC_PENALTY_I16FAVOR] = cml->intra16Favor;
    if (cml->intraPenalty)
        ((vp8Instance_s *)encoder)->asic.regs.pen[0][ASIC_PENALTY_INTER_FAVOR] = cml->intraPenalty;
    if (cml->traceRecon)
        ((vp8Instance_s *)encoder)->asic.traceRecon = 1;
    if (cml->traceMbTiming)
        ((vp8Instance_s *)encoder)->asic.regs.traceMbTiming = 1;
#endif
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
i32 Encode(i32 argc, char **argv, VP8EncInst encoder, testbench_s * cml)
{
    VP8EncIn encIn;
    VP8EncRet ret;
    i32 encodeFail = 0;
    int i, intraPeriodCnt = 0, codedFrameCnt = 0, next = 0, src_img_size;
    FILE *fout = NULL;
    FILE *fmv = NULL;
    FILE *fscaled = NULL;
    i32 usedMinQp = 127, usedMaxQp = 0;

    /* Set the window length for bitrate moving average calculation */
    cml->ma.pos = cml->ma.count = 0;
    cml->ma.frameRateNumer = cml->outputRateNumer;
    cml->ma.frameRateDenom = cml->outputRateDenom;
    if (cml->outputRateDenom)
        cml->ma.length = MAX(1, MIN(cml->outputRateNumer / cml->outputRateDenom,
                                MOVING_AVERAGE_FRAMES));
    else
        cml->ma.length = MOVING_AVERAGE_FRAMES;

    encIn.pOutBuf = cml->outbufMem.virtualAddress;
    encIn.busOutBuf = cml->outbufMem.busAddress;
    encIn.outBufSize = cml->outbufMem.size;

    /* Source Image Size */
    src_img_size = cml->lumWidthSrc * cml->lumHeightSrc *
                    VP8EncGetBitsPerPixel(cml->inputFormat)/8;

    printf("Reading input from file <%s>,\n frame size %d bytes.\n",
            cml->input, src_img_size);

    fout = fopen(cml->output, "wb");
    if(fout == NULL)
    {
        fprintf(VP8ERR_OUTPUT, "Failed to create the output file.\n");
        return -1;
    }

    if (cml->mvOutput)
    {
        fmv = fopen("mv.txt", "wb");
        if(fmv == NULL)
        {
            fprintf(VP8ERR_OUTPUT, "Failed to create mv.txt output file.\n");
            return -1;
        }
    }

    if (cml->scaledWidth)
    {
        fscaled = fopen("scaled.yuyv", "wb");
        if(fscaled == NULL)
        {
            fprintf(VP8ERR_OUTPUT, "Failed to create scaled.yuv output file.\n");
            return -1;
        }
    }

    VP8EncGetRateCtrl(encoder, &cml->rc);

    IvfHeader(cml, fout);
    cml->streamSize += IVF_HDR_BYTES;
    PrintTitle(cml);

    /* Setup encoder input */
    {
        u32 w = (cml->lumWidthSrc + 15) & (~0x0f);

        encIn.busLuma = cml->pictureMem.busAddress;

        encIn.busChromaU = encIn.busLuma + (w * cml->lumHeightSrc);
        encIn.busChromaV = encIn.busChromaU +
            (((w + 1) >> 1) * ((cml->lumHeightSrc + 1) >> 1));
    }

    /* First frame is always intra with zero time increment */
    encIn.codingType = VP8ENC_INTRA_FRAME;
    encIn.timeIncrement = 0;
    encIn.busLumaStab = cml->pictureStabMem.busAddress;

    /* Main encoding loop */
  nextinput:
    while(1)
    {
        next = NextPic(cml->inputRateNumer, cml->inputRateDenom,
                          cml->outputRateNumer, cml->outputRateDenom,
                          cml->frameCnt, cml->firstPic);

        /* End of encoding conditions, stop looping input file read. */
        if (cml->inputFrameTotal+next > cml->lastPic) {
            cml->loopInput = 0; break;
        }

#ifdef EVALUATION_LIMIT
        if(cml->frameCntTotal >= EVALUATION_LIMIT) {
            cml->loopInput = 0; break;
        }
#endif

#ifndef NO_INPUT_READ
        /* Read next frame */
        if(ReadPic(cml, (u8 *) cml->pictureMem.virtualAddress,
                    src_img_size, next) != 0)
            break;

        if(cml->videoStab > 0)
        {
            /* Stabilize the frame after current frame */
            i32 nextStab = NextPic(cml->inputRateNumer, cml->inputRateDenom,
                          cml->outputRateNumer, cml->outputRateDenom,
                          cml->frameCnt+1, cml->firstPic);

            if(ReadPic(cml, (u8 *) cml->pictureStabMem.virtualAddress,
                        src_img_size, nextStab) != 0)
                break;
        }
#endif

        for (i = 0; i < MAX_BPS_ADJUST; i++)
            if (cml->bpsAdjustFrame[i] &&
                (codedFrameCnt == cml->bpsAdjustFrame[i]))
            {
                cml->rc.bitPerSecond = cml->bpsAdjustBitrate[i];
                printf("Adjusting bitrate target: %d\n", cml->rc.bitPerSecond);
                if((ret = VP8EncSetRateCtrl(encoder, &cml->rc)) != VP8ENC_OK)
                    PrintErrorValue("VP8EncSetRateCtrl() failed.", ret);
            }

        /* Select frame type */
        if((cml->intraPicRate != 0) && (intraPeriodCnt >= cml->intraPicRate))
            encIn.codingType = VP8ENC_INTRA_FRAME;
        else
            encIn.codingType = VP8ENC_PREDICTED_FRAME;

        if(encIn.codingType == VP8ENC_INTRA_FRAME)
            intraPeriodCnt = 0;

        /* This applies for PREDICTED frames only. By default always predict
         * from the previous frame only. */
        encIn.ipf = VP8ENC_REFERENCE_AND_REFRESH;
        encIn.grf = encIn.arf = VP8ENC_REFERENCE;

        /* Force odd frames to be coded as droppable. */
        if (cml->droppable && cml->frameCnt&1) {
            encIn.codingType = VP8ENC_PREDICTED_FRAME;
            encIn.ipf = encIn.grf = encIn.arf = VP8ENC_REFERENCE;
        }
        
        /* When temporal layers enabled, force references&updates for layers. */
        if (cml->temporalLayers) {
            u32 layerPatternPics = 1 << (cml->temporalLayers-1);
            u32 picIdx = intraPeriodCnt % layerPatternPics;
            encIn.ipf = temporalLayerIpf[cml->temporalLayers-1][picIdx];
            encIn.grf = temporalLayerGrf[cml->temporalLayers-1][picIdx];
            encIn.arf = temporalLayerArf[cml->temporalLayers-1][picIdx];
            encIn.layerId = temporalLayerId[cml->temporalLayers-1][picIdx];
        }

        SetFrameInternalTest(encoder, cml);

        /* Encode one frame */
        ret = VP8EncStrmEncode(encoder, &encIn, &cml->encOut);

        VP8EncGetRateCtrl(encoder, &cml->rc);

        /* Calculate stream size and bitrate moving average */
        if (cml->encOut.frameSize)
            cml->streamSize += IVF_FRM_BYTES + cml->encOut.frameSize;
        MaAddFrame(&cml->ma, cml->encOut.frameSize*8);

        switch (ret)
        {
        case VP8ENC_FRAME_READY:
            PrintFrame(cml, encoder, next, ret);

            /* Write stream to file in IVF format, max 5 partitions supported */
            if (cml->encOut.frameSize) IvfFrame(cml, fout);
            for (i = 0; i < 5; i++)
                WriteStrm(fout, cml->encOut.pOutBuf[i], cml->encOut.streamSize[i], 0);
            if (cml->temporalLayers)
                WriteStrmLayer(cml, encIn.layerId);

            if (cml->traceMbTiming)
                WriteMotionVectorsDebug(fmv, encoder, cml->frameCnt,
                                cml->width, cml->height);
            else
                WriteMotionVectors(fmv, encoder, cml->frameCnt,
                                cml->width, cml->height);
            /* Write scaled encoder picture to file, packed yuyv 4:2:2 format */
            WriteStrm(fscaled, (u32*)cml->encOut.scaledPicture,
                    cml->scaledWidth*cml->scaledHeight*2, 0);

            if (usedMinQp > cml->rc.qpHdr) usedMinQp = cml->rc.qpHdr;
            if (usedMaxQp < cml->rc.qpHdr) usedMaxQp = cml->rc.qpHdr;
            break;

        case VP8ENC_OUTPUT_BUFFER_OVERFLOW:
            PrintFrame(cml, encoder, next, ret);
            break;

        default:
            PrintErrorValue("VP8EncStrmEncode() failed.", ret);
            PrintFrame(cml, encoder, next, ret);
            encodeFail = (i32) ret;
            /* For debugging, can be removed */
            IvfFrame(cml, fout);
            for (i = 0; i < 5; i++)
                WriteStrm(fout, cml->encOut.pOutBuf[i], cml->encOut.streamSize[i], 0);
            /* We try to continue encoding the next frame */
            break;
        }

        encIn.timeIncrement = cml->outputRateDenom;

        cml->frameCnt++;
        cml->frameCntTotal++;

        if (cml->encOut.codingType != VP8ENC_NOTCODED_FRAME) {
            intraPeriodCnt++; codedFrameCnt++;
        }

    }   /* End of main encoding loop */

    /* Change next input sequence */
    if(ChangeInput(argc, argv, cml, option))
    {
        if(cml->yuvFile != NULL)
        {
            fclose(cml->yuvFile);
            cml->yuvFile = NULL;
        }

        cml->frameCnt = 0;
        cml->inputFrameTotal += MAX(1,next);
        goto nextinput;
    }

    /* Rewrite ivf header with correct frame count */
    IvfHeader(cml, fout);

    /* Print information about encoded frames */
    printf("\nBitrate target %d bps, actual %d bps (%d%%).\n",
            cml->rc.bitPerSecond, cml->bitrate,
            (cml->rc.bitPerSecond) ? cml->bitrate*100/cml->rc.bitPerSecond : 0);
    printf("Total of %llu frames processed, %d frames encoded, %d bytes.\n",
            cml->frameCntTotal, codedFrameCnt, cml->streamSize);
    printf("Used Qp min: %d max: %d\n", usedMinQp, usedMaxQp);

    if (cml->psnrCnt)
        printf("Average PSNR %d.%02d\n",
            (cml->psnrSum/cml->psnrCnt)/100, (cml->psnrSum/cml->psnrCnt)%100);


    /* Free all resources */
    if(fout != NULL)
        fclose(fout);

    if(fmv != NULL)
        fclose(fmv);

    if(fscaled != NULL)
        fclose(fscaled);

    if(cml->yuvFile != NULL)
        fclose(cml->yuvFile);

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
int AllocRes(testbench_s * cml, VP8EncInst enc)
{
    i32 ret;
    u32 pictureSize;
    u32 outbufSize;

    pictureSize = ((cml->lumWidthSrc + 15) & (~15)) * cml->lumHeightSrc *
                    VP8EncGetBitsPerPixel(cml->inputFormat)/8;

    printf("Input %dx%d encoding at %dx%d\n", cml->lumWidthSrc,
           cml->lumHeightSrc, cml->width, cml->height);

    cml->pictureMem.virtualAddress = NULL;
    cml->outbufMem.virtualAddress = NULL;
    cml->pictureStabMem.virtualAddress = NULL;

    /* Here we use the EWL instance directly from the encoder
     * because it is the easiest way to allocate the linear memories */
    ret = EWLMallocLinear(((vp8Instance_s *)enc)->asic.ewl, pictureSize,
                &cml->pictureMem);
    if (ret != EWL_OK)
    {
        fprintf(VP8ERR_OUTPUT, "Failed to allocate input picture!\n");
        cml->pictureMem.virtualAddress = NULL;
        return 1;
    }

    if(cml->videoStab > 0)
    {
        ret = EWLMallocLinear(((vp8Instance_s *)enc)->asic.ewl, pictureSize,
                &cml->pictureStabMem);
        if (ret != EWL_OK)
        {
            fprintf(VP8ERR_OUTPUT, "Failed to allocate stab input picture!\n");
            cml->pictureStabMem.virtualAddress = NULL;
            return 1;
        }
    }

    /* Limited amount of memory on some test environment */
    if (cml->outBufSizeMax == 0) cml->outBufSizeMax = 8;
    outbufSize = 4 * cml->pictureMem.size < (cml->outBufSizeMax*1024*1024) ?
        4 * cml->pictureMem.size : (cml->outBufSizeMax*1024*1024);

    ret = EWLMallocLinear(((vp8Instance_s *)enc)->asic.ewl, outbufSize,
                &cml->outbufMem);
    if (ret != EWL_OK)
    {
        fprintf(VP8ERR_OUTPUT, "Failed to allocate output buffer!\n");
        cml->outbufMem.virtualAddress = NULL;
        return 1;
    }

    printf("Input buffer size:          %d bytes\n", cml->pictureMem.size);
    printf("Input buffer bus address:   0x%08x\n", cml->pictureMem.busAddress);
    printf("Input buffer user address:  0x%08x\n",
                                        (u32) cml->pictureMem.virtualAddress);
    printf("Output buffer size:         %d bytes\n", cml->outbufMem.size);
    printf("Output buffer bus address:  0x%08x\n", cml->outbufMem.busAddress);
    printf("Output buffer user address: 0x%08x\n",
                                        (u32) cml->outbufMem.virtualAddress);

    return 0;
}

/*------------------------------------------------------------------------------

    FreeRes

    Release all resources allcoated byt AllocRes()

------------------------------------------------------------------------------*/
void FreeRes(testbench_s * cml, VP8EncInst enc)
{
    if(cml->pictureMem.virtualAddress != NULL)
        EWLFreeLinear(((vp8Instance_s *)enc)->asic.ewl, &cml->pictureMem);
    if(cml->pictureStabMem.virtualAddress != NULL)
        EWLFreeLinear(((vp8Instance_s *)enc)->asic.ewl, &cml->pictureStabMem);
    if(cml->outbufMem.virtualAddress != NULL)
        EWLFreeLinear(((vp8Instance_s *)enc)->asic.ewl, &cml->outbufMem);
}

/*------------------------------------------------------------------------------

    CheckArea

------------------------------------------------------------------------------*/
i32 CheckArea(VP8EncPictureArea *area, testbench_s * cml)
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
int OpenEncoder(testbench_s * cml, VP8EncInst * pEnc)
{
    VP8EncRet ret;
    VP8EncConfig cfg;
    VP8EncCodingCtrl codingCfg;
    VP8EncRateCtrl rcCfg;
    VP8EncPreProcessingCfg preProcCfg;

    VP8EncInst encoder;

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

    /* input resolution == encoded resolution if not defined */
    if(cml->width == DEFAULT)
        cml->width = cml->lumWidthSrc;
    if(cml->height == DEFAULT)
        cml->height = cml->lumHeightSrc;

    /* output frame rate == input frame rate if not defined */
    if(cml->outputRateNumer == DEFAULT)
        cml->outputRateNumer = cml->inputRateNumer;
    if(cml->outputRateDenom == DEFAULT)
        cml->outputRateDenom = cml->inputRateDenom;

    if(cml->rotation) {
        cfg.width = cml->height;
        cfg.height = cml->width;
        cfg.scaledWidth = cml->scaledHeight;
        cfg.scaledHeight = cml->scaledWidth;
    } else {
        cfg.width = cml->width;
        cfg.height = cml->height;
        cfg.scaledWidth = cml->scaledWidth;
        cfg.scaledHeight = cml->scaledHeight;
    }

    cfg.frameRateDenom = cml->outputRateDenom;
    cfg.frameRateNum = cml->outputRateNumer;
    cfg.refFrameAmount = cml->refFrameAmount;

    printf("Init config: size %dx%d   %d/%d fps  %d refFrames  scaled %dx%d\n",
         cfg.width, cfg.height, cfg.frameRateNum, cfg.frameRateDenom,
         cfg.refFrameAmount, cfg.scaledWidth, cfg.scaledHeight);

    if((ret = VP8EncInit(&cfg, pEnc)) != VP8ENC_OK)
    {
        PrintErrorValue("VP8EncInit() failed.", ret);
        return (int)ret;
    }

    encoder = *pEnc;

    /* Encoder setup: rate control */
    if((ret = VP8EncGetRateCtrl(encoder, &rcCfg)) != VP8ENC_OK)
    {
        PrintErrorValue("VP8EncGetRateCtrl() failed.", ret);
        CloseEncoder(encoder);
        return -1;
    }
    else
    {
        printf("Get rate control: qp=%3d [%2d..%3d] %8d bps,"
                " picRc=%d gop=%d\n",
                rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
                rcCfg.pictureRc, rcCfg.bitrateWindow);

        if (cml->picRc != DEFAULT)
            rcCfg.pictureRc = cml->picRc;
        if (cml->picSkip != DEFAULT)
            rcCfg.pictureSkip = cml->picSkip;
        if (cml->qpHdr != DEFAULT)
            rcCfg.qpHdr = cml->qpHdr;
        else
            rcCfg.qpHdr = -1;
        if (cml->qpMin != DEFAULT)
            rcCfg.qpMin = cml->qpMin;
        if (cml->qpMax != DEFAULT)
            rcCfg.qpMax = cml->qpMax;
        if (cml->bitPerSecond != DEFAULT)
            rcCfg.bitPerSecond = cml->bitPerSecond;
        if (cml->layerBitPerSecond[0])
            rcCfg.layerBitPerSecond[0] = cml->layerBitPerSecond[0];
        if (cml->layerBitPerSecond[1])
            rcCfg.layerBitPerSecond[1] = cml->layerBitPerSecond[1];
        if (cml->layerBitPerSecond[2])
            rcCfg.layerBitPerSecond[2] = cml->layerBitPerSecond[2];
        if (cml->layerBitPerSecond[3])
            rcCfg.layerBitPerSecond[3] = cml->layerBitPerSecond[3];
        if (cml->gopLength != DEFAULT)
            rcCfg.bitrateWindow = cml->gopLength;
        if (cml->intraQpDelta != DEFAULT)
            rcCfg.intraQpDelta = cml->intraQpDelta;
        if (cml->fixedIntraQp != DEFAULT)
            rcCfg.fixedIntraQp = cml->fixedIntraQp;
        if (cml->intraPicRate != 0)
            rcCfg.intraPictureRate = cml->intraPicRate;
        if (cml->goldenPictureRate != DEFAULT)
            rcCfg.goldenPictureRate = cml->goldenPictureRate;
        if (cml->altrefPictureRate != DEFAULT)
            rcCfg.altrefPictureRate = cml->altrefPictureRate;
        if (cml->goldenPictureBoost != DEFAULT)
            rcCfg.goldenPictureBoost = cml->goldenPictureBoost;
        if (cml->adaptiveGoldenBoost != DEFAULT)
            rcCfg.adaptiveGoldenBoost = cml->adaptiveGoldenBoost;
        if (cml->adaptiveGoldenUpdate != DEFAULT)
            rcCfg.adaptiveGoldenUpdate = cml->adaptiveGoldenUpdate;

        if (rcCfg.layerBitPerSecond[0]) {
            u32 f, l;
            /* Enables temporal layers, disables golden&altref boost&update */
            cml->temporalLayers=1;
            rcCfg.adaptiveGoldenBoost = rcCfg.adaptiveGoldenUpdate =
              rcCfg.goldenPictureBoost = rcCfg.goldenPictureRate =
              rcCfg.altrefPictureRate = 0;
            if (rcCfg.layerBitPerSecond[1]) cml->temporalLayers++;
            if (rcCfg.layerBitPerSecond[2]) cml->temporalLayers++;
            if (rcCfg.layerBitPerSecond[3]) cml->temporalLayers++;
            
            /* Set framerates for individual layers, fixed pattern */
            f = cfg.frameRateDenom;
            l = cml->temporalLayers-1;
            rcCfg.layerFrameRateDenom[0] = f * temporalLayerTicScale[l][0];
            rcCfg.layerFrameRateDenom[1] = f * temporalLayerTicScale[l][1];
            rcCfg.layerFrameRateDenom[2] = f * temporalLayerTicScale[l][2];
            rcCfg.layerFrameRateDenom[3] = f * temporalLayerTicScale[l][3];
            
            printf("Layer rate control: base=%d bps, 1st=%d bps, "
                "2nd=%d bps, 3rd=%d bps\n",
                rcCfg.layerBitPerSecond[0], rcCfg.layerBitPerSecond[1],
                rcCfg.layerBitPerSecond[2], rcCfg.layerBitPerSecond[3]);
            printf("              tics: base=%d, 1st=%d, 2nd=%d, 3rd=%d\n",
                rcCfg.layerFrameRateDenom[0], rcCfg.layerFrameRateDenom[1],
                rcCfg.layerFrameRateDenom[2], rcCfg.layerFrameRateDenom[3]);
        }

        printf("Set rate control: qp=%3d [%2d..%3d] %8d bps,"
                " picRc=%d gop=%d\n goldenBoost=%d  goldenUpdate=%d\n",
                rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
                rcCfg.pictureRc, rcCfg.bitrateWindow,
                rcCfg.adaptiveGoldenBoost, rcCfg.adaptiveGoldenUpdate);

        if((ret = VP8EncSetRateCtrl(encoder, &rcCfg)) != VP8ENC_OK)
        {
            PrintErrorValue("VP8EncSetRateCtrl() failed.", ret);
            CloseEncoder(encoder);
            return -1;
        }
    }

    /* Encoder setup: coding control */
    if((ret = VP8EncGetCodingCtrl(encoder, &codingCfg)) != VP8ENC_OK)
    {
        PrintErrorValue("VP8EncGetCodingCtrl() failed.", ret);
        CloseEncoder(encoder);
        return -1;
    }
    else
    {
        if (cml->dctPartitions != DEFAULT)
            codingCfg.dctPartitions = cml->dctPartitions;
        if (cml->errorResilient != DEFAULT)
            codingCfg.errorResilient = cml->errorResilient;
        if (cml->ipolFilter != DEFAULT)
            codingCfg.interpolationFilter = cml->ipolFilter;
        if (cml->filterType != DEFAULT)
            codingCfg.filterType = cml->filterType;
        if (cml->filterLevel != DEFAULT)
            codingCfg.filterLevel = cml->filterLevel;
        if (cml->filterSharpness != DEFAULT)
            codingCfg.filterSharpness = cml->filterSharpness;
        if (cml->quarterPixelMv != DEFAULT)
            codingCfg.quarterPixelMv = cml->quarterPixelMv;
        if (cml->splitMv != DEFAULT)
            codingCfg.splitMv = cml->splitMv;
        if (cml->deadzone != DEFAULT)
            codingCfg.deadzone = cml->deadzone;
        if (cml->maxNumPasses != DEFAULT)
            codingCfg.maxNumPasses = cml->maxNumPasses;
        if (cml->qualityMetric != DEFAULT)
            codingCfg.qualityMetric = cml->qualityMetric;

        codingCfg.qpDelta[0] = cml->qpDelta[0];
        codingCfg.qpDelta[1] = cml->qpDelta[1];
        codingCfg.qpDelta[2] = cml->qpDelta[2];
        codingCfg.qpDelta[3] = cml->qpDelta[3];
        codingCfg.qpDelta[4] = cml->qpDelta[4];

        if (cml->adaptiveRoi != DEFAULT)
            codingCfg.adaptiveRoi = cml->adaptiveRoi;
        if (cml->adaptiveRoiColor != DEFAULT)
            codingCfg.adaptiveRoiColor = cml->adaptiveRoiColor;

        codingCfg.cirStart = cml->cirStart;
        codingCfg.cirInterval = cml->cirInterval;
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

        printf("Set coding control: dctPartitions=%d ipolFilter=%d"
                " errorResilient=%d\n"
                " filterType=%d filterLevel=%d filterSharpness=%d"
                " quarterPixelMv=%d\n"
                " splitMv=%d deadzone=%d passes=%d"
                " qpDeltas=%d,%d,%d,%d,%d tune=%s\n"
                " adaptiveRoi=%d,%d\n",
                codingCfg.dctPartitions, codingCfg.interpolationFilter,
                codingCfg.errorResilient, codingCfg.filterType,
                codingCfg.filterLevel, codingCfg.filterSharpness,
                codingCfg.quarterPixelMv, codingCfg.splitMv,
                codingCfg.deadzone, codingCfg.maxNumPasses,
                codingCfg.qpDelta[0], codingCfg.qpDelta[1],
                codingCfg.qpDelta[2], codingCfg.qpDelta[3],
                codingCfg.qpDelta[4],
                codingCfg.qualityMetric == VP8ENC_QM_SSIM ? "ssim" : "psnr",
                codingCfg.adaptiveRoi, codingCfg.adaptiveRoiColor);

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


        if((ret = VP8EncSetCodingCtrl(encoder, &codingCfg)) != VP8ENC_OK)
        {
            PrintErrorValue("VP8EncSetCodingCtrl() failed.", ret);
            CloseEncoder(encoder);
            return -1;
        }
    }

    /* PreP setup */
    if((ret = VP8EncGetPreProcessing(encoder, &preProcCfg)) != VP8ENC_OK)
    {
        PrintErrorValue("VP8EncGetPreProcessing() failed.", ret);
        CloseEncoder(encoder);
        return -1;
    }
    printf
        ("Get PreP: input %4dx%d, offset %4dx%d, format=%d rotation=%d\n"
           " stab=%d cc=%d scaling=%d\n",
         preProcCfg.origWidth, preProcCfg.origHeight, preProcCfg.xOffset,
         preProcCfg.yOffset, preProcCfg.inputType, preProcCfg.rotation,
         preProcCfg.videoStabilization, preProcCfg.colorConversion.type,
         preProcCfg.scaledOutput);

    preProcCfg.inputType = (VP8EncPictureType)cml->inputFormat;
    preProcCfg.rotation = (VP8EncPictureRotation)cml->rotation;

    preProcCfg.origWidth = cml->lumWidthSrc;
    preProcCfg.origHeight = cml->lumHeightSrc;

    if(cml->horOffsetSrc != DEFAULT)
        preProcCfg.xOffset = cml->horOffsetSrc;
    if(cml->verOffsetSrc != DEFAULT)
        preProcCfg.yOffset = cml->verOffsetSrc;
    if(cml->videoStab != DEFAULT)
        preProcCfg.videoStabilization = cml->videoStab;
    if(cml->colorConversion != DEFAULT)
        preProcCfg.colorConversion.type =
                        (VP8EncColorConversionType)cml->colorConversion;
    if(preProcCfg.colorConversion.type == VP8ENC_RGBTOYUV_USER_DEFINED)
    {
        preProcCfg.colorConversion.coeffA = 20000;
        preProcCfg.colorConversion.coeffB = 44000;
        preProcCfg.colorConversion.coeffC = 5000;
        preProcCfg.colorConversion.coeffE = 35000;
        preProcCfg.colorConversion.coeffF = 38000;
    }

    printf
        ("Set PreP: input %4dx%d, offset %4dx%d, format=%d rotation=%d\n"
           " stab=%d cc=%d scaling=%d\n",
         preProcCfg.origWidth, preProcCfg.origHeight, preProcCfg.xOffset,
         preProcCfg.yOffset, preProcCfg.inputType, preProcCfg.rotation,
         preProcCfg.videoStabilization, preProcCfg.colorConversion.type,
         preProcCfg.scaledOutput);

    /* If HW doesn't support scaling there will be no scaled output. */
    if (!preProcCfg.scaledOutput) cml->scaledWidth = 0;

    if((ret = VP8EncSetPreProcessing(encoder, &preProcCfg)) != VP8ENC_OK)
    {
        PrintErrorValue("VP8EncSetPreProcessing() failed.", ret);
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
void CloseEncoder(VP8EncInst encoder)
{
    VP8EncRet ret;

    if((ret = VP8EncRelease(encoder)) != VP8ENC_OK)
    {
        PrintErrorValue("VP8EncRelease() failed.", ret);
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
int Parameter(i32 argc, char **argv, testbench_s * cml)
{
    i32 ret, i;
    char *optArg;
    argument_s argument;
    i32 status = 0;
    i32 bpsAdjustCount = 0;

    memset(cml, 0, sizeof(testbench_s));

    /* Default setting tries to parse resolution from file name */
    cml->width              = DEFAULT;
    cml->height             = DEFAULT;
    cml->lumWidthSrc        = DEFAULT;
    cml->lumHeightSrc       = DEFAULT;

    cml->input              = input;
    cml->output             = output;
    cml->firstPic           = 0;
    cml->lastPic            = 100;
    cml->inputRateNumer     = 30;
    cml->inputRateDenom     = 1;
    cml->outputRateNumer    = DEFAULT;
    cml->outputRateDenom    = DEFAULT;
    cml->refFrameAmount     = 1;

    /* Default settings are get from API and not changed in testbench */
    cml->colorConversion    = DEFAULT;
    cml->videoStab          = DEFAULT;

    cml->qpHdr              = DEFAULT;
    cml->qpMin              = DEFAULT;
    cml->qpMax              = DEFAULT;
    cml->bitPerSecond       = DEFAULT;
    cml->gopLength          = DEFAULT;
    cml->picRc              = DEFAULT;
    cml->intraQpDelta       = DEFAULT;
    cml->fixedIntraQp       = DEFAULT;
    cml->goldenPictureRate  = DEFAULT;
    cml->altrefPictureRate  = DEFAULT;
    cml->goldenPictureBoost = DEFAULT;
    cml->adaptiveGoldenBoost = DEFAULT;
    cml->adaptiveGoldenUpdate = DEFAULT;    
    cml->maxNumPasses         = DEFAULT;
    cml->qualityMetric      = DEFAULT;

    cml->dctPartitions      = DEFAULT;
    cml->errorResilient     = DEFAULT;
    cml->ipolFilter         = DEFAULT;
    cml->filterType         = DEFAULT;
    cml->filterLevel        = DEFAULT;
    cml->filterSharpness    = DEFAULT;
    cml->quarterPixelMv     = DEFAULT;
    cml->splitMv            = DEFAULT;
    cml->deadzone           = DEFAULT;
    cml->adaptiveRoi        = DEFAULT;
    cml->adaptiveRoiColor   = DEFAULT;
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
        case 'o':
            cml->output = optArg;
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
        case 'C':
            cml->refFrameAmount = atoi(optArg);
            break;
        case 'l':
            cml->inputFormat = atoi(optArg);
            break;
        case 'O':
            cml->colorConversion = atoi(optArg);
            break;
        case 'r':
            cml->rotation = atoi(optArg);
            break;
        case 'Z':
            cml->videoStab = atoi(optArg);
            break;
        case 'I':
            cml->intraPicRate = atoi(optArg);
            break;
        case 'k':
            cml->goldenPictureRate = atoi(optArg);
            break;
        case 'u':
            cml->altrefPictureRate = atoi(optArg);
            break;
        case 'd':
            cml->dctPartitions = atoi(optArg);
            break;
        case 'R':
            cml->errorResilient = atoi(optArg);
            break;
        case 'P':
            cml->ipolFilter = atoi(optArg);
            break;
        case 'M':
            cml->quarterPixelMv = atoi(optArg);
            break;
        case 'N':
            cml->splitMv = atoi(optArg);
            break;
        case 'z':
            cml->deadzone = atoi(optArg);
            break;
        case 'D':
            cml->filterType = atoi(optArg);
            break;
        case 'W':
            cml->filterLevel = atoi(optArg);
            break;
        case 'E':
            cml->filterSharpness = atoi(optArg);
            break;
        case 'V':
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
        case 'c':
            {
                /* Argument must be "xx:yy", replace ':' with 0 */
                if ((i = ParseDelim(optArg, ':')) == -1) break;
                /* xx is cir start */
                cml->cirStart = atoi(optArg);
                /* yy is cir interval */
                cml->cirInterval = atoi(optArg+i+1);
            }
            break;

        case 't':
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
            }
            break;
        case 'K':
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
            }
            break;
        case 'L':
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
            }
            break;
        case 'Q':
            cml->roi1DeltaQp = atoi(optArg);
            break;
        case 'S':
            cml->roi2DeltaQp = atoi(optArg);
            break;
        case 'B':
            cml->bitPerSecond = atoi(optArg);
            break;
        case 'U':
            cml->picRc = atoi(optArg);
            break;
        case 's':
            cml->picSkip = atoi(optArg);
            break;
        case 'g':
            cml->gopLength = atoi(optArg);
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
        case 'A':
            cml->intraQpDelta = atoi(optArg);
            break;
        case 'G':
            cml->fixedIntraQp = atoi(optArg);
            break;
        case 'p':
            cml->printPsnr = 1;
            break;
        case 'v':
            cml->mvOutput = atoi(optArg);
            break;
        case 'e':
            cml->testId = atoi(optArg);
            break;
        case 'T':
            H1EncTraceFileConfig = optArg;
            break;
        case '0':
            /* Check long option */
            if (strcmp(argument.longOpt, "loopInput") == 0)
                cml->loopInput = 1;

            if (strcmp(argument.longOpt, "firstTracePic") == 0)
                H1EncTraceFirstFrame = atoi(optArg);

            if (strcmp(argument.longOpt, "outBufSizeMax") == 0)
                cml->outBufSizeMax = atoi(optArg);

            if (strcmp(argument.longOpt, "intra16Favor") == 0)
                cml->intra16Favor = atoi(optArg);

            if (strcmp(argument.longOpt, "intraPenalty") == 0)
                cml->intraPenalty = atoi(optArg);

            if (strcmp(argument.longOpt, "traceRecon") == 0)
                cml->traceRecon = 1;

            if (strcmp(argument.longOpt, "traceMbTiming") == 0)
                cml->traceMbTiming = 1;

            if (strcmp(argument.longOpt, "droppable") == 0)
                cml->droppable = atoi(optArg);

            if (strcmp(argument.longOpt, "goldenPictureBoost") == 0)
                cml->goldenPictureBoost = atoi(optArg);

            if (strcmp(argument.longOpt, "adaptiveGoldenBoost") == 0)
                cml->adaptiveGoldenBoost = atoi(optArg);

            if (strcmp(argument.longOpt, "adaptiveGoldenUpdate") == 0)
                cml->adaptiveGoldenUpdate = atoi(optArg);
                
            if (strcmp(argument.longOpt, "maxNumPasses") == 0)
                cml->maxNumPasses = atoi(optArg);

            if (strcmp(argument.longOpt, "tune") == 0)
            {
                if(strcmp(optArg, "ssim") == 0)
                    cml->qualityMetric = VP8ENC_QM_SSIM;
                else if (strcmp(optArg, "psnr") == 0)
                    cml->qualityMetric = VP8ENC_QM_PSNR;
                else
                    status = 1;
            }

            if (strcmp(argument.longOpt, "qpDeltaChDc") == 0)
                cml->qpDelta[3] = atoi(optArg);

            if (strcmp(argument.longOpt, "qpDeltaChAc") == 0)
                cml->qpDelta[4] = atoi(optArg);

            if (strcmp(argument.longOpt, "scaledWidth") == 0)
                cml->scaledWidth = atoi(optArg);

            if (strcmp(argument.longOpt, "scaledHeight") == 0)
                cml->scaledHeight = atoi(optArg);

            if (strcmp(argument.longOpt, "adaptiveRoiColor") == 0) {
                cml->adaptiveRoiColor = atoi(optArg);
                break;
            }

            if (strcmp(argument.longOpt, "adaptiveRoi") == 0)
                cml->adaptiveRoi = atoi(optArg);

            if (strcmp(argument.longOpt, "layerBitPerSecond0") == 0)
                cml->layerBitPerSecond[0] = atoi(optArg);

            if (strcmp(argument.longOpt, "layerBitPerSecond1") == 0)
                cml->layerBitPerSecond[1] = atoi(optArg);

            if (strcmp(argument.longOpt, "layerBitPerSecond2") == 0)
                cml->layerBitPerSecond[2] = atoi(optArg);

            if (strcmp(argument.longOpt, "layerBitPerSecond3") == 0)
                cml->layerBitPerSecond[3] = atoi(optArg);

            if (strcmp(argument.longOpt, "config") == 0) {
                /* Use the EWL function directly to read hardware config
                   register and print its contents. */
                EWLHwConfig_t cfg = EWLReadAsicConfig();
                printf(
                   "    maxEncodedWidth     %d\n"
                   "    h264Enabled         %d\n"
                   "    jpegEnabled         %d\n"
                   "    vp8Enabled          %d\n"
                   "    vsEnabled           %d\n"
                   "    rgbEnabled          %d\n"
                   "    searchAreaSmall     %d\n"
                   "    scalingEnabled      %d\n"
                   "    busType             %d\n"
                   "    synthesisLanguage   %d\n"
                   "    busWidth            %d\n",
                   cfg.maxEncodedWidth, cfg.h264Enabled, cfg.jpegEnabled,
                   cfg.vp8Enabled, cfg.vsEnabled, cfg.rgbEnabled,
                   cfg.searchAreaSmall, cfg.scalingEnabled, cfg.busType,
                   cfg.synthesisLanguage, cfg.busWidth * 32);
                exit(0);
            }

        default:
            break;
        }
    }

    return status;
}

/*------------------------------------------------------------------------------

    ReadPic

    Read raw YUV or RGB image data from file

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
int ReadPic(testbench_s * cml, u8 * image, i32 size, i32 nro)
{
    int ret = 0;
    FILE *readFile = NULL;
    i32 width = cml->lumWidthSrc;
    i32 height = cml->lumHeightSrc;

    if(cml->yuvFile == NULL)
    {
        cml->yuvFile = fopen(cml->input, "rb");
        cml->yuvFileName = cml->input;

        if(cml->yuvFile == NULL)
        {
            fprintf(VP8ERR_OUTPUT, "\nUnable to open YUV file: %s\n", cml->input);
            ret = -1;
            goto end;
        }

        fseeko(cml->yuvFile, 0, SEEK_END);
        cml->file_size = ftello(cml->yuvFile);
    }

    readFile = cml->yuvFile;

    /* Stop if over last frame of the file */
    if((off_t)size * (nro + 1) > cml->file_size)
    {
        printf("\nCan't read frame, EOF\n");
        ret = -1;
        goto end;
    }

    if(fseeko(readFile, (off_t)size * nro, SEEK_SET) != 0)
    {
        fprintf(VP8ERR_OUTPUT, "\nI can't seek frame no: %i from file: %s\n",
                nro, cml->yuvFileName);
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

        if(cml->inputFormat == VP8ENC_YUV420_PLANAR)
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
        else if(cml->inputFormat <= VP8ENC_YUV420_SEMIPLANAR_VU)
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
        else if(cml->inputFormat <= VP8ENC_BGR444)  /* 16 bpp */
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
    PrintParameterHelp
        Print out description of a parameter.
------------------------------------------------------------------------------*/
void PrintParameterHelp(char *param)
{
    option_s *opt = option;
    i32 i;

    while (opt->shortOpt != 0)
    {
        if (strcmp(opt->longOpt, param) == 0)
        {
            /* '0' short option is shared, only long opt can be used. */
            if (opt->shortOpt == '0') {
                fprintf(stdout, "        --%s%s", opt->longOpt,
                    opt->enableArg ? "=[n]" : "    ");
                i = strlen(opt->longOpt) + 4;
            } else {
                fprintf(stdout, "  -%c%s --%s", opt->shortOpt,
                    opt->enableArg ? "[n]" : "   ", opt->longOpt);
                i = strlen(opt->longOpt);
            }
            for (; i < 20; i++)
                fprintf(stdout, "%c", ' ');
            fprintf(stdout, "%s\n", opt->description);
            return;
        }

        opt++;
    }
}

/*------------------------------------------------------------------------------
    Help
        Print out some instructions about usage.
------------------------------------------------------------------------------*/
void Help(void)
{
    fprintf(stdout, "Usage:  %s [options] -i inputfile\n\n", "vp8_testenc");
    fprintf(stdout, "Default parameters are marked inside []. More detailed description\n"
                    "of VP8 API parameters can be found from VP8 API Manual.\n");

    /* Frame parameters */
    fprintf(stdout, "\n Parameters affecting which input frames are encoded:\n");
    PrintParameterHelp("input");
    PrintParameterHelp("output");
    PrintParameterHelp("firstPic");
    PrintParameterHelp("lastPic");
    PrintParameterHelp("inputRateNumer");
    PrintParameterHelp("inputRateDenom");
    PrintParameterHelp("outputRateNumer");
    PrintParameterHelp("outputRateDenom");
    fprintf(stdout,
            "                               Encoded frame rate will be\n"
            "                               outputRateNumer/outputRateDenom fps.\n");

    /* Resolution parameters */
    fprintf(stdout, "\n Parameters affecting frame resolutions and cropping:\n");
    PrintParameterHelp("lumWidthSrc");
    PrintParameterHelp("lumHeightSrc");
    PrintParameterHelp("width");
    PrintParameterHelp("height");
    PrintParameterHelp("horOffsetSrc");
    PrintParameterHelp("verOffsetSrc");
    PrintParameterHelp("refFrameAmount");
    PrintParameterHelp("scaledWidth");
    PrintParameterHelp("scaledHeight");

    /* Pre-processing parameters */
    fprintf(stdout, "\n Parameters for pre-processing frames before encoding:\n");
    PrintParameterHelp("inputFormat");
    fprintf(stdout,
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
            "                                14 - BGR101010 32bpp\n");
    PrintParameterHelp("colorConversion");
    fprintf(stdout,
            "                                0 - BT.601\n"
            "                                1 - BT.709\n"
            "                                2 - User defined, coeffs defined in testbench.\n");
    PrintParameterHelp("rotation");
    fprintf(stdout,
            "                                0 - disabled\n"
            "                                1 - 90 degrees right\n"
            "                                2 - 90 degrees left\n");
    PrintParameterHelp("videoStab");
    fprintf(stdout,
            "                                Stabilization works by adjusting cropping offset for\n"
            "                                every frame. Scene change detection encodes scene\n"
            "                                changes as intra frames, it is enabled when\n"
            "                                input resolution == encoded resolution.\n");

    /* Coding config parameters */
    fprintf(stdout, "\n Parameters affecting the output stream and encoding tools:\n");
    PrintParameterHelp("intraPicRate");
    fprintf(stdout,
            "                                Forces every Nth frame to be encoded as intra frame.\n");
    PrintParameterHelp("goldenPicRate");
    PrintParameterHelp("altrefPicRate");
    PrintParameterHelp("dctPartitions");
    PrintParameterHelp("errorResilient");
    PrintParameterHelp("ipolFilter");
    PrintParameterHelp("quarterPixelMv");
    fprintf(stdout,
            "                                Adaptive setting disables 1/4p MVs for resolutions > 1080p.\n");
    PrintParameterHelp("splitMv");
    PrintParameterHelp("deadzone");
    PrintParameterHelp("filterType");
    PrintParameterHelp("filterLevel");
    PrintParameterHelp("filterSharpness");
    PrintParameterHelp("cir");
    PrintParameterHelp("intraArea");
    fprintf(stdout,
            "                                specifying rectangular area of MBs to\n"
            "                                force encoding in intra mode.\n");
    PrintParameterHelp("roi1Area");
    PrintParameterHelp("roi2Area");
    fprintf(stdout,
            "                                specifying rectangular area of MBs as\n"
            "                                two separate Regions-Of-Interest.\n");
    PrintParameterHelp("roi1DeltaQp");
    PrintParameterHelp("roi2DeltaQp");
    PrintParameterHelp("adaptiveRoi");
    PrintParameterHelp("adaptiveRoiColor");
    PrintParameterHelp("droppable");
    PrintParameterHelp("goldenPictureBoost");
    PrintParameterHelp("adaptiveGoldenBoost");
    PrintParameterHelp("adaptiveGoldenUpdate");
    PrintParameterHelp("maxNumPasses");
    PrintParameterHelp("tune");
    PrintParameterHelp("qpDeltaChDc");
    PrintParameterHelp("qpDeltaChAc");

    fprintf(stdout, "\n Parameters for bitrate control:\n");
    PrintParameterHelp("bitPerSecond");
    PrintParameterHelp("layerBitPerSecond0");
    PrintParameterHelp("layerBitPerSecond1");
    PrintParameterHelp("layerBitPerSecond2");
    PrintParameterHelp("layerBitPerSecond3");
    PrintParameterHelp("picRc");
    fprintf(stdout,
            "                                Calculates new target QP for every frame.\n");
    PrintParameterHelp("picSkip");
    fprintf(stdout,
            "                                Allows rate control to skip frames if needed.\n");
    PrintParameterHelp("gopLength");
    fprintf(stdout,
            "                                Rate control allocates bits for one GOP and tries to\n"
            "                                match the target bitrate at the end of each GOP.\n"
            "                                Typically GOP begins with an intra frame, but\n"
            "                                this is not mandatory.\n");
    PrintParameterHelp("qpHdr");
    fprintf(stdout,
            "                                -1 = Encoder calculates initial QP. NOTE: use -q-1\n");
    PrintParameterHelp("qpMin");
    PrintParameterHelp("qpMax");
    PrintParameterHelp("intraQpDelta");
    fprintf(stdout,
            "                                QP difference between target QP and intra frame QP.\n");
    PrintParameterHelp("fixedIntraQp");
    fprintf(stdout,
            "                                Use fixed QP value for every intra frame in stream.\n");
    PrintParameterHelp("bpsAdjust");
    fprintf(stdout,
            "                                Sets new target bitrate at specific frame.\n");

    /* Utility parameters */
    fprintf(stdout, "\n Other parameters for coding and reporting:\n");
    PrintParameterHelp("loopInput");
    PrintParameterHelp("psnr");
    PrintParameterHelp("mvOutput");

    fprintf(stdout, "\n Testing parameters that are not supported for end-user:\n");
    PrintParameterHelp("testId");
    PrintParameterHelp("traceFileConfig");
    PrintParameterHelp("firstTracePic");
    PrintParameterHelp("traceRecon");
    PrintParameterHelp("traceMbTiming");
    fprintf(stdout, "\n");
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
    if (fwrite(strmbuf, sizeof(u8), size, fout) < size)
        fprintf(VP8ERR_OUTPUT, "Failed to write output file!\n");
}

/*------------------------------------------------------------------------------

    WriteStrmLayer
        Write encoded frame to layer "sub-streams" that it belongs to

------------------------------------------------------------------------------*/
void WriteStrmLayer(testbench_s *cml, u32 layerId)
{
    i32 i, j;
    FILE *fLayer;
    char filename[100];

#ifdef NO_OUTPUT_WRITE
    return;
#endif

    /* Create a file for each layer */
    for (i = 0; i < cml->temporalLayers; i++) {
        if (i >= layerId) {
            sprintf(filename, "stream_layer%d.ivf", i);
            if (!cml->frameCntTotal) remove(filename);
            fLayer = fopen(filename, "a");
            if (!cml->frameCntTotal) IvfHeader(cml, fLayer);
            if (cml->encOut.frameSize) IvfFrame(cml, fLayer);
            for (j = 0; j < 5; j++)
                WriteStrm(fLayer, cml->encOut.pOutBuf[j],
                    cml->encOut.streamSize[j], 0);
            fclose(fLayer);
        }
    }
}

/*------------------------------------------------------------------------------
    IvfHeader
------------------------------------------------------------------------------*/
void IvfHeader(testbench_s *tb, FILE *fpOut)
{
    u8 data[IVF_HDR_BYTES] = {0};

    /* File header signature */
    data[0] = 'D';
    data[1] = 'K';
    data[2] = 'I';
    data[3] = 'F';

    /* File format version and file header size */
    data[6] = 32;

    /* Video data FourCC */
    data[8] = 'V';
    data[9] = 'P';
    data[10] = '8';
    data[11] = '0';

    /* Video Image width and height */
    data[12] = tb->width & 0xff;
    data[13] = (tb->width >> 8) & 0xff;
    data[14] = tb->height & 0xff;
    data[15] = (tb->height >> 8) & 0xff;

    /* Frame rate rate */
    data[16] = tb->outputRateNumer & 0xff;
    data[17] = (tb->outputRateNumer >> 8) & 0xff;
    data[18] = (tb->outputRateNumer >> 16) & 0xff;
    data[19] = (tb->outputRateNumer >> 24) & 0xff;

    /* Frame rate scale */
    data[20] = tb->outputRateDenom & 0xff;
    data[21] = (tb->outputRateDenom >> 8) & 0xff;
    data[22] = (tb->outputRateDenom >> 16) & 0xff;
    data[23] = (tb->outputRateDenom >> 24) & 0xff;

    /* Video length in frames */
    data[24] = tb->frameCntTotal & 0xff;
    data[25] = (tb->frameCntTotal >> 8) & 0xff;
    data[26] = (tb->frameCntTotal >> 16) & 0xff;
    data[27] = (tb->frameCntTotal >> 24) & 0xff;

    /* The Ivf "File Header" is in the beginning of the file */
    rewind(fpOut);
    WriteStrm(fpOut, (u32*)data, IVF_HDR_BYTES, 0);
}

/*------------------------------------------------------------------------------
    IvfFrame
------------------------------------------------------------------------------*/
void IvfFrame(testbench_s *tb, FILE *fpOut)
{
    i32 byteCnt = 0;
    u8 data[IVF_FRM_BYTES];

    /* Frame size (4 bytes) */
    byteCnt = tb->encOut.frameSize;
    data[0] =  byteCnt        & 0xff;
    data[1] = (byteCnt >> 8)  & 0xff;
    data[2] = (byteCnt >> 16) & 0xff;
    data[3] = (byteCnt >> 24) & 0xff;

    /* Timestamp (8 bytes) */
    data[4]  =  (tb->frameCntTotal)        & 0xff;
    data[5]  = ((tb->frameCntTotal) >> 8)  & 0xff;
    data[6]  = ((tb->frameCntTotal) >> 16) & 0xff;
    data[7]  = ((tb->frameCntTotal) >> 24) & 0xff;
    data[8]  = ((tb->frameCntTotal) >> 32) & 0xff;
    data[9]  = ((tb->frameCntTotal) >> 40) & 0xff;
    data[10] = ((tb->frameCntTotal) >> 48) & 0xff;
    data[11] = ((tb->frameCntTotal) >> 56) & 0xff;

    WriteStrm(fpOut, (u32*)data, IVF_FRM_BYTES, 0);
}

/*------------------------------------------------------------------------------
    WriteMotionVectors

        Write motion vector and HW output data into a file. Format is defined
        by encOutputMbInfo_s struct, but it relies that HW output endianess
        is configured the same way as compiler constructs the struct.

------------------------------------------------------------------------------*/
void WriteMotionVectors(FILE *file, VP8EncInst encoder, i32 frame, i32 width,
                        i32 height)
{
    i32 i;
    i32 mbPerRow = (width+15)/16;
    i32 mbPerCol = (height+15)/16;
    i32 mbPerFrame = mbPerRow * mbPerCol;
    encOutputMbInfo_s *mbInfo;
    VP8EncOut encOut;


    /* We rely on the compiler to create the struct similar to
       ASIC output configuration. */

    if (!file)
        return;

    /* Print first motion vector in pixel accuracy for every macroblock. */
    fprintf(file, "\npic=%d  MV full-pixel X,Y "
            "for %d macroblocks (%dx%d) block=0\n",
            frame, mbPerFrame, mbPerRow, mbPerCol);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, "  %3d,%3d", mbInfo->mvX[0]/4, mbInfo->mvY[0]/4);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print best inter SAD for every macroblock. */
    fprintf(file, "\npic=%d  inter-SAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->interSad);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print best intra SAD for every macroblock. */
    fprintf(file, "\npic=%d  intra-SAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->intraSad);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print input MAD for every macroblock. HW output is MAD div 128. */
    fprintf(file, "\npic=%d  input-MAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->inputMad_div128*128);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print bit count for every macroblock. */
    fprintf(file, "\npic=%d  bit-count\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %4d", mbInfo->bitCount);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print non-zero block count for every macroblock. */
    fprintf(file, "\npic=%d  non-zero-blocks\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %2d", mbInfo->nonZeroCnt);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print MB mode for every macroblock. */
    fprintf(file, "\npic=%d  coding-mode  0=I16x16_DC, .. 4=I4x4,"
                  " 6=P16x16_Zero, .. 10=P16x8, .. \n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %2d", mbInfo->mode & MBOUT_8_MODE_MASK);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print I4x4 modes for every macroblock. */
    fprintf(file, "\npic=%d  I4x4-modes\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
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
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->yMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print Cb mean for every macroblock. */
    fprintf(file, "\npic=%d  Cb-mean\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->cbMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print Cr mean for every macroblock. */
    fprintf(file, "\npic=%d  Cr-mean\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfo_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->crMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }
}

void WriteMotionVectorsDebug(FILE *file, VP8EncInst encoder, i32 frame, i32 width,
                             i32 height)
{
    i32 i;
    i32 mbPerRow = (width+15)/16;
    i32 mbPerCol = (height+15)/16;
    i32 mbPerFrame = mbPerRow * mbPerCol;
    encOutputMbInfoDebug_s *mbInfo;
    VP8EncOut encOut;

    /* We rely on the compiler to create the struct similar to
       ASIC output configuration. */

    if (!file)
        return;

    /* Print first motion vector in pixel accuracy for every macroblock. */
    fprintf(file, "\npic=%d  MV full-pixel X,Y "
            "for %d macroblocks (%dx%d) block=0\n",
            frame, mbPerFrame, mbPerRow, mbPerCol);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, "  %3d,%3d", mbInfo->mvX[0]/4, mbInfo->mvY[0]/4);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print best inter SAD for every macroblock. */
    fprintf(file, "\npic=%d  inter-SAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->interSad);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print best intra SAD for every macroblock. */
    fprintf(file, "\npic=%d  intra-SAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->intraSad);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print input MAD for every macroblock. HW output is MAD div 128. */
    fprintf(file, "\npic=%d  input-MAD\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %5d", mbInfo->inputMad_div128*128);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print bit count for every macroblock. */
    fprintf(file, "\npic=%d  bit-count\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %4d", mbInfo->bitCount);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print non-zero block count for every macroblock. */
    fprintf(file, "\npic=%d  non-zero-blocks\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %2d", mbInfo->nonZeroCnt);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print MB mode for every macroblock. */
    fprintf(file, "\npic=%d  coding-mode, 0=I16x16_DC, .. 4=I4x4,"
                  " 6=P16x16_Zero, .. 10=P16x8, .. \n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %2d", mbInfo->mode & MBOUT_8_MODE_MASK);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print I4x4 modes for every macroblock. */
    fprintf(file, "\npic=%d  I4x4-modes\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
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
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->yMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print Cb mean for every macroblock. */
    fprintf(file, "\npic=%d  Cb-mean\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->cbMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    /* Print Cr mean for every macroblock. */
    fprintf(file, "\npic=%d  Cr-mean\n", frame);
    for (i = 0; i < mbPerFrame; i++) {
        VP8EncGetMbInfo(encoder, &encOut, i);
        mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
        fprintf(file, " %3d", mbInfo->crMean);
        if ((i % mbPerRow) == mbPerRow-1) fprintf(file, "\n");
    }

    {
        static FILE *ftiming = NULL;

        if (!ftiming)
            ftiming = fopen("mbtiming.trc", "w");

        /* Print bus timing data for every macroblock. */
        fprintf(ftiming, "\npic=%d  Bus-timing\n", frame);
        for (i = 0; i < mbPerFrame; i++) {
            VP8EncGetMbInfo(encoder, &encOut, i);
            mbInfo = (encOutputMbInfoDebug_s *)encOut.motionVectors;
            fprintf(ftiming, "%5d,", (mbInfo->data[0] << 8) | mbInfo->data[1]);
            fprintf(ftiming, "%5d,", (mbInfo->data[2] << 8) | mbInfo->data[3]);
            fprintf(ftiming, "%5d,", (mbInfo->data[4] << 8) | mbInfo->data[5]);
            fprintf(ftiming, "%5d\n", (mbInfo->data[6] << 8) | mbInfo->data[7]);
        }
    }
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
int ChangeInput(i32 argc, char **argv, testbench_s * cml, option_s * option)
{
    i32 ret;
    argument_s argument;
    i32 enable = 0;

#ifdef MULTIFILEINPUT
    /* End when can't open next file */
    if (!cml->yuvFile)
        return 0;

    /* Use the first file name as base: input.yuv */
    if (fileNumber == 0)
    {
        strcpy(inputFilename, cml->input);
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
    cml->input = inputFilename;
    printf("\nNext file: %s\n", cml->input);
    return 1;
#endif

    if (cml->loopInput)
        return 1;

    argument.optCnt = 1;
    while((ret = EncGetOption(argc, argv, option, &argument)) != -1)
    {
        if((ret == 1) && (enable))
        {
            cml->input = argument.optArg;
            printf("\nNext file: %s\n", cml->input);
            return 1;
        }
        if(argument.optArg == cml->input)
        {
            enable = 1;
        }
    }

    return 0;
}

/*------------------------------------------------------------------------------

    API tracing
        Tracing as defined by the API.
    Params:
        msg - null terminated tracing message
------------------------------------------------------------------------------*/
void VP8EncTrace(const char *msg)
{
    static FILE *fp = NULL;

    if(fp == NULL)
        fp = fopen("api.trc", "wt");

    if(fp)
        fprintf(fp, "%s\n", msg);
}

/*------------------------------------------------------------------------------
    Print title for per frame prints
------------------------------------------------------------------------------*/
void PrintTitle(testbench_s *cml)
{
    printf("\n");
    printf(" In Pic  QP Type IP ");
    if (cml->refFrameAmount >= 2) printf("GR ");
    if (cml->refFrameAmount >= 3) printf("AR ");
    printf("|   BR avg    MA(%3d) | ByteCnt (inst) |", cml->ma.length);

    if (cml->printPsnr)
        printf(" PSNR  |");

    printf("\n");
    printf("-------------------------------------------------------------------------\n");

    printf("        %3d HDR     ", cml->rc.qpHdr);
    if (cml->refFrameAmount >= 2) printf("   ");
    if (cml->refFrameAmount >= 3) printf("   ");
    printf("|                     | %7i %6i |", cml->streamSize, IVF_HDR_BYTES);

    if (cml->printPsnr)
        printf("       |");
    printf("\n");
}

/*------------------------------------------------------------------------------
    Print information per frame
------------------------------------------------------------------------------*/
void PrintFrame(testbench_s *cml, VP8EncInst encoder, u32 frameNumber,
        VP8EncRet ret)
{
    u32 i, psnr = 0, partSum = 0;

    if((cml->frameCntTotal+1) && cml->outputRateDenom)
    {
        /* Using 64-bits to avoid overflow */
        u64 tmp = cml->streamSize / (cml->frameCntTotal+1);
        tmp *= (u32) cml->outputRateNumer;

        cml->bitrate = (u32) (8 * (tmp / (u32) cml->outputRateDenom));
    }

    printf("%3i %3llu %3d ",
        frameNumber, cml->frameCntTotal, cml->rc.qpHdr);

    printf("%s",
        (ret == VP8ENC_OUTPUT_BUFFER_OVERFLOW) ? "lost" :
        (cml->encOut.codingType == VP8ENC_INTRA_FRAME) ? " I  " :
        (cml->encOut.codingType == VP8ENC_PREDICTED_FRAME) ? " P  " : "skip");

    /* Print reference frame usage */
    printf(" %c%c",
        cml->encOut.ipf & VP8ENC_REFERENCE ? 'R' : '-',
        cml->encOut.ipf & VP8ENC_REFRESH   ? 'W' : '-');
    if (cml->refFrameAmount >= 2) printf("-%c%c",
        cml->encOut.grf & VP8ENC_REFERENCE ? 'R' : '-',
        cml->encOut.grf & VP8ENC_REFRESH   ? 'W' : '-');
    if (cml->refFrameAmount >= 3) printf("-%c%c",
        cml->encOut.arf & VP8ENC_REFERENCE ? 'R' : '-',
        cml->encOut.arf & VP8ENC_REFRESH   ? 'W' : '-');

    /* Print bitrate statistics and frame size */
    printf(" | %9u %9u | %7i %6i | ",
        cml->bitrate, Ma(&cml->ma), cml->streamSize, cml->encOut.frameSize);

    /* This works only with system model, bases are pointers. */
    if (cml->printPsnr)
        psnr = PrintPSNR((u8 *)
            (((vp8Instance_s *)encoder)->asic.regs.inputLumBase +
            ((vp8Instance_s *)encoder)->asic.regs.inputLumaBaseOffset),
            (u8 *)
            (((vp8Instance_s *)encoder)->asic.regs.internalImageLumBaseW),
            cml->lumWidthSrc, cml->width, cml->height, cml->rotation,
            cml->encOut.mse_mul256);

    if (psnr) {
        cml->psnrSum += psnr;
        cml->psnrCnt++;
    }

    /* Print size of each partition in bytes */
    printf("%d %d %d", cml->encOut.frameSize ? IVF_FRM_BYTES : 0,
            cml->encOut.streamSize[0],
            cml->encOut.streamSize[1]);
    for (i = 2; i <= 4; i++)
        if (cml->encOut.streamSize[i]) printf(" %d", cml->encOut.streamSize[i]);
    printf("\n");

    /* Check that partition sizes match frame size */
    for (i = 0; i < 5; i++)
        partSum += cml->encOut.streamSize[i];

    if (cml->encOut.frameSize != partSum)
        printf("ERROR: Frame size doesn't match partition sizes!\n");
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
        case VP8ENC_ERROR: str = "VP8ENC_ERROR"; break;
        case VP8ENC_NULL_ARGUMENT: str = "VP8ENC_NULL_ARGUMENT"; break;
        case VP8ENC_INVALID_ARGUMENT: str = "VP8ENC_INVALID_ARGUMENT"; break;
        case VP8ENC_MEMORY_ERROR: str = "VP8ENC_MEMORY_ERROR"; break;
        case VP8ENC_EWL_ERROR: str = "VP8ENC_EWL_ERROR"; break;
        case VP8ENC_EWL_MEMORY_ERROR: str = "VP8ENC_EWL_MEMORY_ERROR"; break;
        case VP8ENC_INVALID_STATUS: str = "VP8ENC_INVALID_STATUS"; break;
        case VP8ENC_OUTPUT_BUFFER_OVERFLOW: str = "VP8ENC_OUTPUT_BUFFER_OVERFLOW"; break;
        case VP8ENC_HW_BUS_ERROR: str = "VP8ENC_HW_BUS_ERROR"; break;
        case VP8ENC_HW_DATA_ERROR: str = "VP8ENC_HW_DATA_ERROR"; break;
        case VP8ENC_HW_TIMEOUT: str = "VP8ENC_HW_TIMEOUT"; break;
        case VP8ENC_HW_RESERVED: str = "VP8ENC_HW_RESERVED"; break;
        case VP8ENC_SYSTEM_ERROR: str = "VP8ENC_SYSTEM_ERROR"; break;
        case VP8ENC_INSTANCE_ERROR: str = "VP8ENC_INSTANCE_ERROR"; break;
        case VP8ENC_HRD_ERROR: str = "VP8ENC_HRD_ERROR"; break;
        case VP8ENC_HW_RESET: str = "VP8ENC_HW_RESET"; break;
        default: str = "UNDEFINED";
    }

    fprintf(VP8ERR_OUTPUT, "%s Return value: %s\n", errorDesc, str);
}

/*------------------------------------------------------------------------------
    PrintPSNR
        Calculate and print frame PSNR
------------------------------------------------------------------------------*/
u32 PrintPSNR(u8 *a, u8 *b, i32 scanline, i32 wdh, i32 hgt, i32 rot, u32 mse)
{
#ifdef PSNR
        float fmse = 0.0;

#ifdef TRUE_MSE
        u32 tmp, i, j;

        /* Calculate true MSE using every encoded pixel. */
        if (a && b && !rot)
        {
            for (j = 0 ; j < hgt; j++) {
                    for (i = 0; i < wdh; i++) {
                            tmp = a[i] - b[i];
                            tmp *= tmp;
                            fmse += tmp;
                    }
                    a += scanline;
                    b += wdh;
            }
            fmse /= wdh * hgt;
        }
#else
        /* If MSE calculated by HW is available, use it. */
        if (mse)
            fmse = (float)mse/256;
#endif

        if (fmse == 0.0) {
                printf("--.-- | ");
        } else {
                fmse = 10.0 * log10f(65025.0 / fmse);
                printf("%5.2f | ", fmse);
        }

        return (u32)roundf(fmse*100);
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
    u64 sum = 0;     /* Using 64-bits to avoid overflow */

    for (i = 0; i < ma->count; i++)
        sum += ma->frame[i];

    if (!ma->frameRateDenom)
        return 0;

    sum = sum / ma->length;

    return sum * ma->frameRateNumer / ma->frameRateDenom;
}


