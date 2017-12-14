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
--  Abstract : Video stabilization stnadalone testbench
--
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "EncGetOption.h"

#include "vidstbapi.h"

/* For accessing the EWL instance inside the video stabilizer instance */
#include "vidstabinternal.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------


--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

#define DEFAULT -1

/* Global variables */

static char *input = "input.yuv";

/* SW/HW shared memories for input/output buffers */
static EWLLinearMem_t pictureMem;
static EWLLinearMem_t nextPictureMem;

static FILE *yuvFile = NULL;
static long int file_size;

i32 trigger_point = -1;      /* Logic Analyzer trigger point */

static option_s option[] = {
    {"help", '?'},
    {"firstPic", 'a', 1},
    {"lastPic", 'b', 1},
    {"width", 'W', 1},
    {"height", 'H', 1},
    {"lumWidthSrc", 'w', 1},
    {"lumHeightSrc", 'h', 1},
    {"inputFormat", 'l', 1},    /* Input image format */
    {"input", 'i', 1},
    {"rotation", 'r', 1},   /* Input image rotation */
    {"traceresult", 'T', 0},    /* trace result to file */
    {"burstSize", 'N', 1},  /* Coded Picture Buffer Size */
    {"trigger", 'P', 1},
    {0, 0, 0}
};

const char *strFormat[14] =
    { "YUV 420 planar", "YUV 420 semiplanar", "YUV 422 (YUYV)",
    "YUV 422 (UYVY)", "RGB 565", "BGR 565", "RGB 555", "BGR 555",
    "RGB 444", "BGR 444", "RGB 888", "BGR 888", "RGB 101010", "BGR 101010"
};

/* Structure for command line options */
typedef struct
{
    char *input;
    i32 inputFormat;
    i32 firstPic;
    i32 lastPic;
    i32 width;
    i32 height;
    i32 lumWidthSrc;
    i32 lumHeightSrc;
    i32 horOffsetSrc;
    i32 verOffsetSrc;
    i32 burst;
    i32 trace;
} commandLine_s;

#ifdef TEST_DATA
/* This is defined because system model needs this. Not needed otherwise. */
char * H1EncTraceFileConfig = NULL;
int H1EncTraceFirstFrame    = 0;
#endif

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static int AllocRes(commandLine_s * cmdl, VideoStbInst inst);
static void FreeRes(VideoStbInst inst);
static int ReadPic(u8 * image, i32 nro, char *name, i32 width,
                   i32 height, i32 format);
static int Parameter(i32 argc, char **argv, commandLine_s * ep);
static void Help(void);

int main(int argc, char *argv[])
{
    VideoStbInst videoStab;
    VideoStbParam stabParam;
    commandLine_s cmdl;
    VideoStbRet ret;

    i32 nextPict;

    VideoStbApiVersion apiVer;
    VideoStbBuild csBuild;

    FILE *fTrc = NULL;

    apiVer = VideoStbGetApiVersion();
    csBuild = VideoStbGetBuild();

    fprintf(stdout, "VideoStb API version %d.%d\n", apiVer.major, apiVer.minor);
    fprintf(stdout, "HW ID: %c%c 0x%08x\t SW Build: %u.%u.%u\n\n",
            csBuild.hwBuild>>24, (csBuild.hwBuild>>16)&0xff,
            csBuild.hwBuild, csBuild.swBuild / 1000000,
            (csBuild.swBuild / 1000) % 1000, csBuild.swBuild % 1000);

    if(argc < 2)
    {
        Help();
        exit(0);
    }

    /* Parse command line parameters */
    if(Parameter(argc, argv, &cmdl) != 0)
    {
        printf("Input parameter error\n");
        return -1;
    }

    stabParam.format = (VideoStbInputFormat)cmdl.inputFormat;
    stabParam.inputWidth = cmdl.lumWidthSrc;
    stabParam.inputHeight = cmdl.lumHeightSrc;
    stabParam.stabilizedWidth = cmdl.width;
    stabParam.stabilizedHeight = cmdl.height;

    stabParam.stride = (cmdl.lumWidthSrc + 7) & (~7);   /* 8 multiple */

    if((ret = VideoStbInit(&videoStab, &stabParam)) != VIDEOSTB_OK)
    {
        printf("VideoStbInit ERROR: %d\n", ret);
        goto end;
    }

    /* Allocate input and output buffers */
    if(AllocRes(&cmdl, videoStab) != 0)
    {
        printf("Failed to allocate the external resources!\n");
        FreeRes(videoStab);
        VideoStbRelease(videoStab);
        return 1;
    }

    if(cmdl.trace)
    {
        fTrc = fopen("video_stab_result.log", "w");
        if(fTrc == NULL)
            printf("\nWARNING! Could not open 'video_stab_result.log'\n\n");
        else
            fprintf(fTrc, "offX, offY\n");
    }

    nextPict = cmdl.firstPic;
    while(nextPict <= cmdl.lastPic)
    {
        VideoStbResult result;

        if(ReadPic((u8 *) pictureMem.virtualAddress, nextPict, cmdl.input,
                   cmdl.lumWidthSrc, cmdl.lumHeightSrc, cmdl.inputFormat) != 0)
            break;

        if(ReadPic((u8 *) nextPictureMem.virtualAddress, nextPict + 1, cmdl.input,
                   cmdl.lumWidthSrc, cmdl.lumHeightSrc, cmdl.inputFormat) != 0)
            break;

        ret = VideoStbStabilize(videoStab, &result, pictureMem.busAddress,
                              nextPictureMem.busAddress);
        if(ret != VIDEOSTB_OK)
        {
            printf("VideoStbStabilize ERROR: %d\n", ret);
            break;
        }

        printf("PIC %d: (%2d,%2d)\n", nextPict, result.stabOffsetX,
               result.stabOffsetY);

        if(cmdl.trace)
        {
            if(fTrc != NULL)
                fprintf(fTrc, "%4d, %4d\n", result.stabOffsetX,
                        result.stabOffsetY);
        }

        nextPict++;
    }

    if(fTrc != NULL)
        fclose(fTrc);

    if(yuvFile != NULL)
        fclose(yuvFile);

  end:
    FreeRes(videoStab);

    VideoStbRelease(videoStab);

    return 0;
}

/*------------------------------------------------------------------------------

    AllocRes

    OS dependent implementation for allocating the physical memories 
    used by both SW and HW: the input picture.

    To access the memory HW uses the physical linear address (bus address) 
    and SW uses virtual address (user address).

    In Linux the physical memories can only be allocated with sizes
    of power of two times the page size.

------------------------------------------------------------------------------*/
int AllocRes(commandLine_s * cmdl, VideoStbInst inst)
{
    u32 pictureSize;
    u32 ret;

    if(cmdl->inputFormat <= 1)
    {
        /* Input picture in planar YUV 4:2:0 format */
        pictureSize = ((cmdl->lumWidthSrc + 7) & (~7)) * cmdl->lumHeightSrc;
    }
    else if(cmdl->inputFormat <= 9)
    {
        /* Input picture in YUYV 4:2:2 format or 16-bit RGB format */
        pictureSize = ((cmdl->lumWidthSrc + 7) & (~7)) * cmdl->lumHeightSrc * 2;
    }
    else
    {
        /* Input picture in 32-bit RGB format */
        pictureSize =
            ((cmdl->lumWidthSrc + 7) & (~7)) * cmdl->lumHeightSrc * 4;
    }

    printf("Input size %dx%d Stabilized %dx%d\n", cmdl->lumWidthSrc,
           cmdl->lumHeightSrc, cmdl->width, cmdl->height);

    printf("Input format: %d = %s\n", cmdl->inputFormat,
           ((unsigned) cmdl->inputFormat) <
           14 ? strFormat[cmdl->inputFormat] : "Unknown");

    /* Here we use the EWL instance directly from the stabilizer instance 
     * because it is the easiest way to allocate the linear memories */
    if (((VideoStb *)inst)->ewl == NULL) return 1;

    ret = EWLMallocLinear(((VideoStb *)inst)->ewl, pictureSize, 
                &pictureMem);
    if (ret != EWL_OK) {
        pictureMem.virtualAddress = NULL;
        return 1;
    }

    ret = EWLMallocLinear(((VideoStb *)inst)->ewl, pictureSize, 
                &nextPictureMem);
    if (ret != EWL_OK) {
        nextPictureMem.virtualAddress = NULL;
        return 1;
    }
    

    printf("Input buffer size: %d bytes\n", pictureSize);

    printf("Input buffer bus address:       0x%08x\n", pictureMem.busAddress);
    printf("Input buffer user address:      0x%08x\n", (u32)pictureMem.virtualAddress);
    printf("Next input buffer bus address:  0x%08x\n", nextPictureMem.busAddress);
    printf("Next input buffer user address: 0x%08x\n", (u32)nextPictureMem.virtualAddress);

    return 0;
}

/*------------------------------------------------------------------------------

    FreeRes

    Release all resources allcoated byt AllocRes()

------------------------------------------------------------------------------*/
void FreeRes(VideoStbInst inst)
{
    if (pictureMem.virtualAddress)
        EWLFreeLinear(((VideoStb *)inst)->ewl, &pictureMem);
    if (nextPictureMem.virtualAddress)
        EWLFreeLinear(((VideoStb *)inst)->ewl, &nextPictureMem);
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
    i32 ret;
    char *optArg;
    argument_s argument;
    i32 status = 0;

    memset(cml, DEFAULT, sizeof(commandLine_s));

    cml->input = input;
    cml->firstPic = 0;
    cml->lastPic = 100;

    cml->lumWidthSrc = 176;
    cml->lumHeightSrc = 144;
    cml->inputFormat = 0;
    cml->horOffsetSrc = 0;
    cml->verOffsetSrc = 0;

    cml->burst = 16;
    cml->trace = 0;

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
        case 'a':
            cml->firstPic = atoi(optArg);
            break;
        case 'b':
            cml->lastPic = atoi(optArg);
            break;
        case 'W':
            cml->width = atoi(optArg);
            break;
        case 'H':
            cml->height = atoi(optArg);
            break;
        case 'w':
            cml->lumWidthSrc = atoi(optArg);
            break;
        case 'h':
            cml->lumHeightSrc = atoi(optArg);
            break;
        case 'N':
            cml->burst = atoi(optArg);
            break;
        case 'T':
            cml->trace = 1;
            break;
        case 'l':
            cml->inputFormat = atoi(optArg);
            break;
        case 'P':
            trigger_point = atoi(optArg);
            break;

        default:
            break;
        }
    }

    if(cml->width == DEFAULT)
        cml->width = cml->lumWidthSrc - 16;

    if(cml->height == DEFAULT)
        cml->height = cml->lumHeightSrc - 16;

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
        format  - image format (YUV 420 planar or semiplanar, or YUV 422)

    Returns:
        0 - for success
        non-zero error code 
------------------------------------------------------------------------------*/
int ReadPic(u8 * image, i32 nro, char *name, i32 width, i32 height, i32 format)
{
    int ret = 0;
    i32 src_img_size;
    i32 luma_size;

    luma_size = width * height;

    if(format > 1)
    {
        if(format < 10)
        {
            src_img_size = luma_size * 2;
            luma_size *= 2;
        }
        else
        {
            src_img_size = luma_size * 4;
            luma_size *= 4;
        }
    }
    else
    {
        src_img_size = (luma_size * 3) / 2;
    }

#ifdef VIDEOSTB_SIMULATION
    return 0;
#endif

    /* Read input from file frame by frame */
    printf("Reading frame %i (%i bytes) from %s... ", nro, luma_size, name);

    if(yuvFile == NULL)
    {
        yuvFile = fopen(name, "rb");

        if(yuvFile == NULL)
        {
            printf("Unable to open file\n");
            ret = -1;
            goto end;
        }

        fseek(yuvFile, 0, SEEK_END);
        file_size = ftell(yuvFile);
    }

    /* Stop if over last frame of the file */
    if(src_img_size * (nro + 1) > file_size)
    {
        printf("Can't read frame, EOF\n");
        ret = -1;
        goto end;
    }

    if(fseek(yuvFile, src_img_size * nro, SEEK_SET) != 0)
    {
        printf("Can't seek to frame\n");
        ret = -1;
        goto end;
    }

    if((width & 7) == 0)
        fread(image, 1, luma_size, yuvFile);
    else
    {
        i32 i;
        u8 *buf = image;
        i32 scan = (width + 7) & (~7);  /* we need 8 multiple stride */

        if(format == 0)
        {
            /* Y */
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width, yuvFile);
                buf += scan;
            }
        }
        else if(format == 1)
        {
            /* Y */
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width, yuvFile);
                buf += scan;
            }
        }
        else if (format < 10)
        {
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width * 2, yuvFile);
                buf += scan * 2;
            }
        }
        else
        {
            for(i = 0; i < height; i++)
            {
                fread(buf, 1, width * 4, yuvFile);
                buf += scan * 4;
            }
        }

    }

    printf("OK\n");

  end:

    return ret;
}

/*------------------------------------------------------------------------------

    Help
        Print out some instructions about usage.
------------------------------------------------------------------------------*/
void Help(void)
{
    fprintf(stdout, "Usage:  %s [options] -i inputfile\n\n", "camstabtest");

    fprintf(stdout,
            "  -i[s] --input             Read input from file. [input.yuv]\n"
            "  -a[n] --firstPic          First picture of input file. [0]\n"
            "  -b[n] --lastPic           Last picture of input file. [100]\n"
            "  -w[n] --lumWidthSrc       Width of source image. [176]\n"
            "  -h[n] --lumHeightSrc      Height of source image. [144]\n");
    fprintf(stdout,
            "  -W[n] --width             Width of output image. [--lumWidthSrc]\n"
            "  -H[n] --height            Height of output image. [--lumHeightSrc]\n");

    fprintf(stdout,
            "  -l[n] --inputFormat       Input YUV format. [0]\n"
            "                                0 - YUV420\n"
            "                                1 - YUV420 semiplanar\n"
            "                                2 - YUYV422\n"
            "                                3 - UYVY422\n"
            "                                4 - RGB565\n"
            "                                5 - BGR565\n"
            "                                6 - RGB555\n"
            "                                7 - BGR555\n"
            "                                8 - RGB444\n"
            "                                9 - BGR444\n"
            "                                10 - RGB888\n"
            "                                11 - BGR888\n"
            "                                12 - RGB101010\n"
            "                                13 - BGR101010\n\n");

    fprintf(stdout,
            "  -T    --traceresult       Write output to file <video_stab_result.log>\n");

    fprintf(stdout,
            "\nTesting parameters that are not supported for end-user:\n"
            "  -N[n] --burstSize          0..63 HW bus burst size. [16]\n"
            "  -P[n] --trigger           Logic Analyzer trigger at picture <n>. [-1]\n"
            "\n");
    ;
}

void VideoStb_Trace(const char *msg)
{
    printf("%s\n", msg);
}
