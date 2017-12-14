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
--  Abstract : Jpeg Encoder testbench
--
------------------------------------------------------------------------------*/

/* For parameter parsing */
#include "EncGetOption.h"
#include "JpegTestBench.h"

/* For SW/HW shared memory allocation */
#include "ewl.h"

/* For accessing the EWL instance inside the encoder */
#include "EncJpegInstance.h"

/* For compiler flags, test data, debug and tracing */
#include "enccommon.h"

/* For Hantro Jpeg encoder */
#include "jpegencapi.h"

/* For printing and file IO */
#include <stdio.h>
#include <stddef.h>

/* For dynamic memory allocation */
#include <stdlib.h>

/* For memset, strcpy and strlen */
#include <string.h>

#include <assert.h>

/*--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/* User selectable testbench configuration */

/* Define this if you want to save each frame of motion jpeg 
 * into frame%d.jpg */
/* #define SEPARATE_FRAME_OUTPUT */

/* Define this if yuv don't want to use debug printf */
/*#define ASIC_WAVE_TRACE_TRIGGER*/

/* Output stream is not written to file. This should be used
   when running performance simulations. */
/*#define NO_OUTPUT_WRITE */

/* Define these if you want to use testbench defined 
 * comment header */
/*#define TB_DEFINED_COMMENT */

#define USER_DEFINED_QTABLE 10

/* Global variables */

/* Command line options */

static option_s options[] = {
    {"help", 'H', 0},
    {"inputThumb", 'I', 1},
    {"thumbnail", 'T', 1},
    {"widthThumb", 'K', 1},
    {"heightThumb", 'L', 1},
    {"input", 'i', 1},
    {"output", 'o', 1},
    {"firstPic", 'a', 1},
    {"lastPic", 'b', 1},
    {"lumWidthSrc", 'w', 1},
    {"lumHeightSrc", 'h', 1},
    {"width", 'x', 1},
    {"height", 'y', 1},
    {"horOffsetSrc", 'X', 1},
    {"verOffsetSrc", 'Y', 1},
    {"restartInterval", 'R', 1},
    {"qLevel", 'q', 1},
    {"frameType", 'g', 1},
    {"colorConversion", 'v', 1},
    {"rotation", 'G', 1},
    {"codingType", 'p', 1},
    {"codingMode", 'm', 1},
    {"markerType", 't', 1},
    {"units", 'u', 1},
    {"xdensity", 'k', 1},
    {"ydensity", 'l', 1},
    {"write", 'W', 1},
    {"comLength", 'c', 1},
    {"comFile", 'C', 1},
    {"trigger", 'P', 1},
    {NULL, 0, 0}
};

/* SW/HW shared memories for input/output buffers */
EWLLinearMem_t pictureMem;
EWLLinearMem_t outbufMem;

/* Test bench definition of comment header */
#ifdef TB_DEFINED_COMMENT
    /* COM data */
static u32 comLen = 38;
static u8 comment[39] = "This is Hantro's test COM data header.";
#endif

static JpegEncCfg cfg;

static u32 writeOutput = 1;

/* Logic Analyzer trigger point */
i32 trigger_point = -1;

u32 thumbDataLength;
u8 * thumbData = NULL; /* thumbnail data buffer */

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static void FreeRes(JpegEncInst enc);
static int AllocRes(commandLine_s * cmdl, JpegEncInst encoder);
static int OpenEncoder(commandLine_s * cml, JpegEncInst * encoder);
static void CloseEncoder(JpegEncInst encoder);
static int ReadPic(u8 * image, i32 width, i32 height, i32 sliceNum,
                   i32 sliceRows, i32 frameNum, char *name, u32 inputMode);
static int Parameter(i32 argc, char **argv, commandLine_s * ep);
static void Help(void);
static void WriteStrm(FILE * fout, u32 * outbuf, u32 size, u32 endian);
static u32 GetResolution(char *filename, i32 *pWidth, i32 *pHeight);


#ifdef SEPARATE_FRAME_OUTPUT
static void WriteFrame(char *filename, u32 * strmbuf, u32 size);
#endif

/*------------------------------------------------------------------------------

    main

------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    JpegEncInst encoder;
    JpegEncRet ret;
    JpegEncIn encIn;
    JpegEncOut encOut;
    JpegEncApiVersion encVer;
    JpegEncBuild encBuild;
    int encodeFail = 0;

    FILE *fout = NULL;
    i32 picBytes = 0;

    commandLine_s cmdl;

    fprintf(stdout,
	    "\n* * * * * * * * * * * * * * * * * * * * *\n\n"
            "      HANTRO JPEG ENCODER TESTBENCH\n"
            "\n* * * * * * * * * * * * * * * * * * * * *\n\n");

    /* Print API and build version numbers */
    encVer = JpegEncGetApiVersion();
    encBuild = JpegEncGetBuild();

    /* Version */
    fprintf(stdout, "JPEG Encoder API v%d.%d\n", encVer.major, encVer.minor);

    fprintf(stdout, "HW ID: %c%c 0x%08x\t SW Build: %u.%u.%u\n\n",
            encBuild.hwBuild>>24, (encBuild.hwBuild>>16)&0xff,
            encBuild.hwBuild, encBuild.swBuild / 1000000,
            (encBuild.swBuild / 1000) % 1000, encBuild.swBuild % 1000);

    if(argc < 2)
    {
        Help();
        exit(0);
    }

    /* Parse command line parameters */
    if(Parameter(argc, argv, &cmdl) != 0)
    {
        fprintf(stderr, "Input parameter error\n");
        return -1;
    }

    /* Encoder initialization */
    if((ret = OpenEncoder(&cmdl, &encoder)) != 0)
    {
        return -ret;    /* Return positive value for test scripts */
    }

    /* Allocate input and output buffers */
    if(AllocRes(&cmdl, encoder) != 0)
    {
        fprintf(stderr, "Failed to allocate the external resources!\n");
        FreeRes(encoder);
        CloseEncoder(encoder);
        return 1;
    }

    /* Setup encoder input */
    encIn.pOutBuf = (u8 *)outbufMem.virtualAddress;

    printf("encIn.pOutBuf 0x%08x\n", (unsigned int) encIn.pOutBuf);
    encIn.busOutBuf = outbufMem.busAddress;
    encIn.outBufSize = outbufMem.size;
    encIn.frameHeader = 1;

    {
        i32 slice = 0, sliceRows = 0;
        i32 next = 0, last = 0, picCnt = 0;
        i32 widthSrc, heightSrc;
        char *input;


        /* Set Full Resolution mode */
        ret = JpegEncSetPictureSize(encoder, &cfg);

	/* Handle error situation */
        if(ret != JPEGENC_OK)
        {
#ifndef ASIC_WAVE_TRACE_TRIGGER
            printf("FAILED. Error code: %i\n", ret);
#endif
            goto end;
        }

            /* If no slice mode, the slice equals whole frame */
            if(cmdl.partialCoding == 0)
                sliceRows = cmdl.lumHeightSrc;
            else
                sliceRows = cmdl.restartInterval * 16;

            widthSrc = (cmdl.lumWidthSrc+15)/16*16;
            heightSrc = cmdl.lumHeightSrc;

            input = cmdl.input;

            last = cmdl.lastPic;


        if(cmdl.frameType <= JPEGENC_YUV420_SEMIPLANAR_VU)
        {
            /* Bus addresses of input picture, used by hardware encoder */
            encIn.busLum = pictureMem.busAddress;
            encIn.busCb = encIn.busLum + (widthSrc * sliceRows);
            encIn.busCr = encIn.busCb +
                (((widthSrc + 1) / 2) * ((sliceRows + 1) / 2));

            /* Virtual addresses of input picture, used by software encoder */
            encIn.pLum = (u8 *)pictureMem.virtualAddress;
            encIn.pCb = encIn.pLum + (widthSrc * sliceRows);
            encIn.pCr = encIn.pCb +
                (((widthSrc + 1) / 2) * ((sliceRows + 1) / 2));
        }
        else
        {
            /* Bus addresses of input picture, used by hardware encoder */
            encIn.busLum = pictureMem.busAddress;
            encIn.busCb = encIn.busLum;
            encIn.busCr = encIn.busCb;

            /* Virtual addresses of input picture, used by software encoder */
            encIn.pLum = (u8 *)pictureMem.virtualAddress;
            encIn.pCb = encIn.pLum;
            encIn.pCr = encIn.pCb;
        }

        fout = fopen(cmdl.output, "wb");
        if(fout == NULL)
        {
            fprintf(stderr, "Failed to create the output file.\n");
            FreeRes(encoder);
            CloseEncoder(encoder);
            return -1;
        }

        /* Main encoding loop */
        ret = JPEGENC_FRAME_READY;
        next = cmdl.firstPic;
        while(next <= last &&
              (ret == JPEGENC_FRAME_READY ||
               ret == JPEGENC_OUTPUT_BUFFER_OVERFLOW))
        {
#ifdef SEPARATE_FRAME_OUTPUT
            char framefile[50];

            sprintf(framefile, "frame%d%s.jpg", picCnt, mode == 1 ? "tn" : "");
            remove(framefile);
#endif

#ifndef ASIC_WAVE_TRACE_TRIGGER
            printf("Frame %3d started...\n", picCnt);
#endif
            fflush(stdout);

            /* Loop until one frame is encoded */
            do
            {
#ifndef NO_INPUT_YUV
                /* Read next slice */
                if(ReadPic
                   ((u8 *) pictureMem.virtualAddress,
                    cmdl.lumWidthSrc, heightSrc, slice,
                    sliceRows, next, input, cmdl.frameType) != 0)
                    break;
#endif
                ret = JpegEncEncode(encoder, &encIn, &encOut);

                switch (ret)
                {
                case JPEGENC_RESTART_INTERVAL:

#ifndef ASIC_WAVE_TRACE_TRIGGER
                    printf("Frame %3d restart interval! %6u bytes\n",
                           picCnt, encOut.jfifSize);
                    fflush(stdout);
#endif

                    if(writeOutput)
                        WriteStrm(fout, outbufMem.virtualAddress, 
                                encOut.jfifSize, 0);
#ifdef SEPARATE_FRAME_OUTPUT
                    if(writeOutput)
                        WriteFrame(framefile, outbufMem.virtualAddress, 
                                encOut.jfifSize);
#endif
                    picBytes += encOut.jfifSize;
                    slice++;    /* Encode next slice */
                    break;

                case JPEGENC_FRAME_READY:
#ifndef ASIC_WAVE_TRACE_TRIGGER
                    printf("Frame %3d ready! %6u bytes\n",
                           picCnt, encOut.jfifSize);
                    fflush(stdout);
#endif

                    if(writeOutput)
                        WriteStrm(fout, outbufMem.virtualAddress, 
                                encOut.jfifSize, 0);
#ifdef SEPARATE_FRAME_OUTPUT
                    if(writeOutput)
                        WriteFrame(framefile, outbufMem.virtualAddress, 
                                encOut.jfifSize);
#endif
                    picBytes = 0;
                    slice = 0;
                    break;

                case JPEGENC_OUTPUT_BUFFER_OVERFLOW:

#ifndef ASIC_WAVE_TRACE_TRIGGER
                    printf("Frame %3d lost! Output buffer overflow.\n",
                           picCnt);
#endif

                    /* For debugging
                    if(writeOutput)
                        WriteStrm(fout, outbufMem.virtualAddress, 
                                outbufMem.size, 0);*/
                    /* Rewind the file back this picture's bytes
                    fseek(fout, -picBytes, SEEK_CUR);*/
                    picBytes = 0;
                    slice = 0;
                    break;

                default:

#ifndef ASIC_WAVE_TRACE_TRIGGER
                    printf("FAILED. Error code: %i\n", ret);
#endif
                    encodeFail = (int)ret;
                    /* For debugging */
                    if(writeOutput)
                        WriteStrm(fout, outbufMem.virtualAddress,
                                encOut.jfifSize, 0);
                    break;
                }
            }
            while(ret == JPEGENC_RESTART_INTERVAL);

            picCnt++;
            next = picCnt + cmdl.firstPic;
        }   /* End of main encoding loop */

    }   /* End of encoding modes */

end:

#ifndef ASIC_WAVE_TRACE_TRIGGER
    printf("Release encoder\n");
#endif

    /* Free all resources */
    FreeRes(encoder);
    CloseEncoder(encoder);
    if(fout != NULL)
        fclose(fout);

    return encodeFail;
}

/*------------------------------------------------------------------------------

    AllocRes

    Allocation of the physical memories used by both SW and HW: 
    the input picture and the output stream buffer.

    NOTE! The implementation uses the EWL instance from the encoder
          for OS independence. This is not recommended in final environment 
          because the encoder will release the EWL instance in case of error.
          Instead, the memories should be allocated from the OS the same way
          as inside EWLMallocLinear().

------------------------------------------------------------------------------*/
int AllocRes(commandLine_s * cmdl, JpegEncInst enc)
{
    i32 sliceRows = 0;
    u32 pictureSize;
    u32 outbufSize;
    i32 ret;

    /* Set slice size and output buffer size
     * For output buffer size, 1 byte/pixel is enough for most images.
     * Some extra is needed for testing purposes (noise input) */

    if(cmdl->partialCoding == 0)
        sliceRows = cmdl->lumHeightSrc;
    else
        sliceRows = cmdl->restartInterval * 16;
            
    outbufSize = cmdl->width * sliceRows * 2 + 100 * 1024;

    if(cmdl->thumbnail)
        outbufSize += cmdl->widthThumb * cmdl->heightThumb;

    /* calculate picture size */
    pictureSize = (cmdl->lumWidthSrc+15)/16*16 * sliceRows *
                    JpegEncGetBitsPerPixel(cmdl->frameType)/8;
    
    pictureMem.virtualAddress = NULL;
    outbufMem.virtualAddress = NULL;
    
     /* Here we use the EWL instance directly from the encoder 
     * because it is the easiest way to allocate the linear memories */
    ret = EWLMallocLinear(((jpegInstance_s *)enc)->asic.ewl, pictureSize, 
                &pictureMem);
    if (ret != EWL_OK)
    {
        fprintf(stderr, "Failed to allocate input picture!\n");
        pictureMem.virtualAddress = NULL;        
        return 1;
    }    
    /* this is limitation for FPGA testing */
    outbufSize = outbufSize < (1024*1024*7) ? outbufSize : (1024*1024*7);

    ret = EWLMallocLinear(((jpegInstance_s *)enc)->asic.ewl, outbufSize, 
                &outbufMem);
    if (ret != EWL_OK)
    {
        fprintf(stderr, "Failed to allocate output buffer!\n");
        outbufMem.virtualAddress = NULL;
        return 1;
    }

#ifndef ASIC_WAVE_TRACE_TRIGGER
    printf("Input %dx%d + %dx%d encoding at %dx%d + %dx%d ",
               cmdl->lumWidthSrcThumb, cmdl->lumHeightSrcThumb,
               cmdl->lumWidthSrc, cmdl->lumHeightSrc,
               cmdl->widthThumb, cmdl->heightThumb, cmdl->width, cmdl->height);

    if(cmdl->partialCoding != 0)
        printf("in slices of %dx%d", cmdl->width, sliceRows);
    printf("\n");
#endif


#ifndef ASIC_WAVE_TRACE_TRIGGER
    printf("Input buffer size:          %d bytes\n", pictureMem.size);
    printf("Input buffer bus address:   0x%08x\n", pictureMem.busAddress);
    printf("Input buffer user address:  %10p\n", pictureMem.virtualAddress);
    printf("Output buffer size:         %d bytes\n", outbufMem.size);
    printf("Output buffer bus address:  0x%08x\n", outbufMem.busAddress);
    printf("Output buffer user address: %10p\n",  outbufMem.virtualAddress);
#endif

    return 0;
}

/*------------------------------------------------------------------------------

    FreeRes

------------------------------------------------------------------------------*/
void FreeRes(JpegEncInst enc)
{
    if(pictureMem.virtualAddress != NULL)
        EWLFreeLinear(((jpegInstance_s *)enc)->asic.ewl, &pictureMem); 
    if(outbufMem.virtualAddress != NULL)
        EWLFreeLinear(((jpegInstance_s *)enc)->asic.ewl, &outbufMem);
    if(thumbData != NULL)
        free(thumbData);
#ifndef TB_DEFINED_COMMENT   
    if(cfg.pCom != NULL)
	    free(cfg.pCom);
#endif
}

/*------------------------------------------------------------------------------

    OpenEncoder

------------------------------------------------------------------------------*/
int OpenEncoder(commandLine_s * cml, JpegEncInst * pEnc)
{
    JpegEncRet ret;

    /* An example of user defined quantization table */
    const u8 qTable[64] = {1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1};

#ifndef TB_DEFINED_COMMENT
    FILE *fileCom = NULL;
#endif

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

    cfg.rotation = (JpegEncPictureRotation)cml->rotation;
    cfg.inputWidth = (cml->lumWidthSrc + 15) & (~15);   /* API limitation */
    if (cfg.inputWidth != (u32)cml->lumWidthSrc)
        fprintf(stdout, "Warning: Input width must be multiple of 16!\n");
    cfg.inputHeight = cml->lumHeightSrc;

    if(cfg.rotation)
    {
        /* full */
        cfg.xOffset = cml->verOffsetSrc;
        cfg.yOffset = cml->horOffsetSrc;

        cfg.codingWidth = cml->height;
        cfg.codingHeight = cml->width;
        cfg.xDensity = cml->ydensity;
        cfg.yDensity = cml->xdensity;	
    }
    else
    {
        /* full */
        cfg.xOffset = cml->horOffsetSrc;
        cfg.yOffset = cml->verOffsetSrc;

        cfg.codingWidth = cml->width;
        cfg.codingHeight = cml->height;
        cfg.xDensity = cml->xdensity;
        cfg.yDensity = cml->ydensity;
    }

    if (cml->qLevel == USER_DEFINED_QTABLE)
    {
        cfg.qTableLuma = qTable;
        cfg.qTableChroma = qTable;
    }
    else
        cfg.qLevel = cml->qLevel;

    cfg.restartInterval = cml->restartInterval;
    cfg.codingType = (JpegEncCodingType)cml->partialCoding;
    cfg.frameType = (JpegEncFrameType)cml->frameType;
    cfg.unitsType = (JpegEncAppUnitsType)cml->unitsType;
    cfg.markerType = (JpegEncTableMarkerType)cml->markerType;
    cfg.colorConversion.type = (JpegEncColorConversionType)cml->colorConversion;
    if (cfg.colorConversion.type == JPEGENC_RGBTOYUV_USER_DEFINED)
    {
        /* User defined RGB to YCbCr conversion coefficients, scaled by 16-bits */
        cfg.colorConversion.coeffA = 20000;
        cfg.colorConversion.coeffB = 44000;
        cfg.colorConversion.coeffC = 5000;
        cfg.colorConversion.coeffE = 35000;
        cfg.colorConversion.coeffF = 38000;
    }
    writeOutput = cml->write;
    cfg.codingMode = (JpegEncCodingMode)cml->codingMode;

#ifdef NO_OUTPUT_WRITE
    writeOutput = 0;
#endif

    if(cml->thumbnail < 0 || cml->thumbnail > 3)
    {
        fprintf(stderr, "\nNot valid thumbnail format!");
	    return -1;	
    }
    
    if(cml->thumbnail != 0)
    {
	FILE *fThumb;

	fThumb = fopen(cml->inputThumb, "rb");
	if(fThumb == NULL)
	{
	    fprintf(stderr, "\nUnable to open Thumbnail file: %s\n", cml->inputThumb);
	    return -1;
	}
	
	switch(cml->thumbnail)
	{
	    case 1:
		fseek(fThumb,0,SEEK_END);		
		thumbDataLength = ftell(fThumb);
		fseek(fThumb,0,SEEK_SET);
		break;
	    case 2:
		thumbDataLength = 3*256 + cml->widthThumb * cml->heightThumb;
		break;
	    case 3:
		thumbDataLength = cml->widthThumb * cml->heightThumb * 3;		
		break;
	    default:
		assert(0);		
	}

	thumbData = (u8*)malloc(thumbDataLength);
	fread(thumbData,1,thumbDataLength, fThumb);
	fclose(fThumb);
    }

/* use either "hard-coded"/testbench COM data or user specific */
#ifdef TB_DEFINED_COMMENT
    cfg.comLength = comLen;
    cfg.pCom = comment;
#else
    cfg.comLength = cml->comLength;

    if(cfg.comLength)
    {
        /* allocate mem for & read comment data */
        cfg.pCom = (u8 *) malloc(cfg.comLength);

        fileCom = fopen(cml->com, "rb");
        if(fileCom == NULL)
        {
            fprintf(stderr, "\nUnable to open COMMENT file: %s\n", cml->com);
            return -1;
        }

        fread(cfg.pCom, 1, cfg.comLength, fileCom);
        fclose(fileCom);
    }

#endif

#ifndef ASIC_WAVE_TRACE_TRIGGER
    fprintf(stdout, "Init config: %dx%d @ x%dy%d => %dx%d   \n",
            cfg.inputWidth, cfg.inputHeight, cfg.xOffset, cfg.yOffset,
            cfg.codingWidth, cfg.codingHeight);

    fprintf(stdout,
            "\n\t**********************************************************\n");
    fprintf(stdout, "\n\t-JPEG: ENCODER CONFIGURATION\n");
    if (cml->qLevel == USER_DEFINED_QTABLE)
    {
        i32 i;
        fprintf(stdout, "JPEG: qTableLuma \t:");
        for (i = 0; i < 64; i++)
            fprintf(stdout, " %d", cfg.qTableLuma[i]);
        fprintf(stdout, "\n");
        fprintf(stdout, "JPEG: qTableChroma \t:");
        for (i = 0; i < 64; i++)
            fprintf(stdout, " %d", cfg.qTableChroma[i]);
        fprintf(stdout, "\n");
    }
    else
        fprintf(stdout, "\t-JPEG: qp \t\t:%d\n", cfg.qLevel);
    fprintf(stdout, "\t-JPEG: inX \t\t:%d\n", cfg.inputWidth);
    fprintf(stdout, "\t-JPEG: inY \t\t:%d\n", cfg.inputHeight);
    fprintf(stdout, "\t-JPEG: outX \t\t:%d\n", cfg.codingWidth);
    fprintf(stdout, "\t-JPEG: outY \t\t:%d\n", cfg.codingHeight);
    fprintf(stdout, "\t-JPEG: rst \t\t:%d\n", cfg.restartInterval);
    fprintf(stdout, "\t-JPEG: xOff \t\t:%d\n", cfg.xOffset);
    fprintf(stdout, "\t-JPEG: yOff \t\t:%d\n", cfg.yOffset);
    fprintf(stdout, "\t-JPEG: frameType \t:%d\n", cfg.frameType);
    fprintf(stdout, "\t-JPEG: colorConversionType :%d\n", cfg.colorConversion.type);
    fprintf(stdout, "\t-JPEG: colorConversionA    :%d\n", cfg.colorConversion.coeffA);
    fprintf(stdout, "\t-JPEG: colorConversionB    :%d\n", cfg.colorConversion.coeffB);
    fprintf(stdout, "\t-JPEG: colorConversionC    :%d\n", cfg.colorConversion.coeffC);
    fprintf(stdout, "\t-JPEG: colorConversionE    :%d\n", cfg.colorConversion.coeffE);
    fprintf(stdout, "\t-JPEG: colorConversionF    :%d\n", cfg.colorConversion.coeffF);
    fprintf(stdout, "\t-JPEG: rotation \t:%d\n", cfg.rotation);
    fprintf(stdout, "\t-JPEG: codingType \t:%d\n", cfg.codingType);
    fprintf(stdout, "\t-JPEG: codingMode \t:%d\n", cfg.codingMode);
    fprintf(stdout, "\t-JPEG: markerType \t:%d\n", cfg.markerType);
    fprintf(stdout, "\t-JPEG: units \t\t:%d\n", cfg.unitsType);
    fprintf(stdout, "\t-JPEG: xDen \t\t:%d\n", cfg.xDensity);
    fprintf(stdout, "\t-JPEG: yDen \t\t:%d\n", cfg.yDensity);


    fprintf(stdout, "\t-JPEG: thumbnail format\t:%d\n", cml->thumbnail);
    fprintf(stdout, "\t-JPEG: Xthumbnail\t:%d\n", cml->widthThumb);
    fprintf(stdout, "\t-JPEG: Ythumbnail\t:%d\n", cml->heightThumb);
    
    fprintf(stdout, "\t-JPEG: First picture\t:%d\n", cml->firstPic);
    fprintf(stdout, "\t-JPEG: Last picture\t\t:%d\n", cml->lastPic);
#ifdef TB_DEFINED_COMMENT
    fprintf(stdout, "\n\tNOTE! Using comment values defined in testbench!\n");
#else
    fprintf(stdout, "\t-JPEG: comlen \t\t:%d\n", cfg.comLength);
    fprintf(stdout, "\t-JPEG: COM \t\t:%s\n", cfg.pCom);
#endif
    fprintf(stdout,
            "\n\t**********************************************************\n\n");
#endif

    if((ret = JpegEncInit(&cfg, pEnc)) != JPEGENC_OK)
    {
        fprintf(stderr,
                "Failed to initialize the encoder. Error code: %8i\n", ret);
        return (int)ret;
    }

    if(thumbData != NULL)
    {
	JpegEncThumb jpegThumb;
	jpegThumb.format = cml->thumbnail == 1 ? JPEGENC_THUMB_JPEG : cml->thumbnail == 3 ?
				     JPEGENC_THUMB_RGB24 : JPEGENC_THUMB_PALETTE_RGB8;
	jpegThumb.width = cml->widthThumb;
	jpegThumb.height = cml->heightThumb;
	jpegThumb.data = thumbData;
       	jpegThumb.dataLength = thumbDataLength;
	
	ret = JpegEncSetThumbnail(*pEnc, &jpegThumb );
	if(ret != JPEGENC_OK )
	{
	    fprintf(stderr,
                "Failed to set thumbnail. Error code: %8i\n", ret);
    	    return -1;
    	}
    }

    return 0;
}

/*------------------------------------------------------------------------------

    CloseEncoder

------------------------------------------------------------------------------*/
void CloseEncoder(JpegEncInst encoder)
{
    JpegEncRet ret;

    if((ret = JpegEncRelease(encoder)) != JPEGENC_OK)
    {
        fprintf(stderr,
                "Failed to release the encoder. Error code: %8i\n", ret);
    }
}

/*------------------------------------------------------------------------------

    Parameter

------------------------------------------------------------------------------*/
int Parameter(i32 argc, char **argv, commandLine_s * cml)
{
    i32 ret;
    char *optarg;
    argument_s argument;
    int status = 0;

    memset(cml, 0, sizeof(commandLine_s));
    strcpy(cml->input, "input.yuv");
    strcpy(cml->inputThumb, "thumbnail.jpg");
    strcpy(cml->com, "com.txt");
    strcpy(cml->output, "stream.jpg");
    cml->firstPic = 0;
    cml->lastPic = 0;
    cml->lumWidthSrc = DEFAULT;
    cml->lumHeightSrc = DEFAULT;
    cml->width = DEFAULT;
    cml->height = DEFAULT;
    cml->horOffsetSrc = 0;
    cml->verOffsetSrc = 0;
    cml->qLevel = 1;
    cml->restartInterval = 0;
    cml->thumbnail = 0;
    cml->widthThumb = 32;
    cml->heightThumb = 32;
    cml->frameType = 0;
    cml->colorConversion = 0;
    cml->rotation = 0;
    cml->partialCoding = 0;
    cml->codingMode = 0;
    cml->markerType = 0;
    cml->unitsType = 0;
    cml->xdensity = 1;
    cml->ydensity = 1;
    cml->write = 1;
    cml->comLength = 0;

    argument.optCnt = 1;
    while((ret = EncGetOption(argc, argv, options, &argument)) != -1)
    {
        if(ret == -2)
        {
            status = -1;
        }
        optarg = argument.optArg;
        switch (argument.shortOpt)
        {
        case 'H':
            Help();
            exit(0);
        case 'i':
            if(strlen(optarg) < MAX_PATH)
            {
                strcpy(cml->input, optarg);
            }
            else
            {
                status = -1;
            }
            break;
        case 'I':
            if(strlen(optarg) < MAX_PATH)
            {
                strcpy(cml->inputThumb, optarg);
            }
            else
            {
                status = -1;
            }
            break;
        case 'o':
            if(strlen(optarg) < MAX_PATH)
            {
                strcpy(cml->output, optarg);
            }
            else
            {
                status = -1;
            }
            break;
        case 'C':
            if(strlen(optarg) < MAX_PATH)
            {
                strcpy(cml->com, optarg);
            }
            else
            {
                status = -1;
            }
            break;
        case 'a':
            cml->firstPic = atoi(optarg);
            break;
        case 'b':
            cml->lastPic = atoi(optarg);
            break;
        case 'x':
            cml->width = atoi(optarg);
            break;
        case 'y':
            cml->height = atoi(optarg);
            break;
        case 'w':
            cml->lumWidthSrc = atoi(optarg);
            break;
        case 'h':
            cml->lumHeightSrc = atoi(optarg);
            break;
        case 'X':
            cml->horOffsetSrc = atoi(optarg);
            break;
        case 'Y':
            cml->verOffsetSrc = atoi(optarg);
            break;
        case 'R':
            cml->restartInterval = atoi(optarg);
            break;
        case 'q':
            cml->qLevel = atoi(optarg);
            break;
        case 'g':
            cml->frameType = atoi(optarg);
            break;
        case 'v':
            cml->colorConversion = atoi(optarg);
            break;
        case 'G':
            cml->rotation = atoi(optarg);
            break;
        case 'p':
            cml->partialCoding = atoi(optarg);
            break;
        case 'm':
            cml->codingMode = atoi(optarg);
            break;
        case 't':
            cml->markerType = atoi(optarg);
            break;
        case 'u':
            cml->unitsType = atoi(optarg);
            break;
        case 'k':
            cml->xdensity = atoi(optarg);
            break;
        case 'l':
            cml->ydensity = atoi(optarg);
            break;
        case 'T':
            cml->thumbnail = atoi(optarg);
            break;
        case 'K':
            cml->widthThumb = atoi(optarg);
            break;
        case 'L':
            cml->heightThumb = atoi(optarg);
            break;
        case 'W':
            cml->write = atoi(optarg);
            break;
        case 'c':
            cml->comLength = atoi(optarg);
            break;
        case 'P':
            trigger_point = atoi(optarg);
            break;
        default:
            break;
        }
    }

    return status;
}

/*------------------------------------------------------------------------------

    ReadPic

    Read raw YUV image data from file
    Image is divided into slices, each slice consists of equal amount of 
    image rows except for the bottom slice which may be smaller than the 
    others. sliceNum is the number of the slice to be read
    and sliceRows is the amount of rows in each slice (or 0 for all rows).

------------------------------------------------------------------------------*/
int ReadPic(u8 * image, i32 width, i32 height, i32 sliceNum, i32 sliceRows,
            i32 frameNum, char *name, u32 inputMode)
{
    FILE *file = NULL;
    i32 frameSize;
    i32 frameOffset;
    i32 sliceLumOffset = 0;
    i32 sliceCbOffset = 0;
    i32 sliceCrOffset = 0;
    i32 sliceLumSize;        /* The size of one slice in bytes */
    i32 sliceCbSize;
    i32 sliceCrSize;
    i32 sliceLumWidth;       /* Picture line length to be read */
    i32 sliceCbWidth;
    i32 sliceCrWidth;
    i32 i;
    i32 lumRowBpp, cbRowBpp, crRowBpp; /* Bits per pixel for lum,cb,cr */
    i32 width16 = (width+15)/16*16;

    if(sliceRows == 0)
        sliceRows = height;

    frameSize = width * height * JpegEncGetBitsPerPixel(inputMode)/8;

    if(inputMode == JPEGENC_YUV420_PLANAR) {
        lumRowBpp = 8;
        cbRowBpp = 4;
        crRowBpp = 4;
    } else if(inputMode <= JPEGENC_YUV420_SEMIPLANAR_VU) {
        lumRowBpp = 8;
        cbRowBpp = 8;
        crRowBpp = 0;
    } else if(inputMode <= JPEGENC_BGR444) {
        lumRowBpp = 16;
        cbRowBpp = 0;
        crRowBpp = 0;
    } else { /* 32-bit RGB */
        lumRowBpp = 32;
        cbRowBpp = 0;
        crRowBpp = 0;
    }
    sliceLumWidth = width*lumRowBpp/8;  /* Luma bytes per input row */
    sliceCbWidth = width*cbRowBpp/8;    /* Cb bytes per input row */
    sliceCrWidth = width*crRowBpp/8;
    /* Size of complete slice in input file */
    sliceLumSize = sliceLumWidth * sliceRows;
    sliceCbSize = sliceCbWidth * sliceRows / 2;
    sliceCrSize = sliceCrWidth * sliceRows / 2;

    /* Offset for frame start from start of file */
    frameOffset = frameSize * frameNum;
    /* Offset for slice luma start from start of frame */
    sliceLumOffset = sliceLumSize * sliceNum;
    /* Offset for slice cb start from start of frame */
    if(sliceCbSize)
        sliceCbOffset = width * height + sliceCbSize * sliceNum;
    /* Offset for slice cr start from start of frame */
    if(sliceCrSize)
        sliceCrOffset = width * height + 
                        width/2 * height/2 + sliceCrSize * sliceNum;

    /* Size of complete slice after padding to 16-multiple width */
    sliceLumSize = width16*lumRowBpp/8 * sliceRows;
    sliceCbSize = width16*cbRowBpp/8 * sliceRows / 2;

    /* The bottom slice may be smaller than the others */
    if(sliceRows * (sliceNum + 1) > height)
        sliceRows = height - sliceRows * sliceNum;

    /* Read input from file frame by frame */
#ifndef ASIC_WAVE_TRACE_TRIGGER
    printf("Reading frame %d slice %d (%d bytes) from %s... ",
           frameNum, sliceNum, 
           sliceLumWidth*sliceRows + sliceCbWidth*sliceRows/2 +
           sliceCrWidth*sliceRows/2, name);
    fflush(stdout);
#endif

    file = fopen(name, "rb");
    if(file == NULL)
    {
        fprintf(stderr, "\nUnable to open VOP file: %s\n", name);
        return -1;
    }

    fseek(file, frameOffset + sliceLumOffset, SEEK_SET);
    if ((width & 0xf) == 0)
        fread(image, 1, sliceLumWidth*sliceRows, file);
    else {
        i32 scan = width16*lumRowBpp/8;
        /* Read the luminance so that scan (=stride) is multiple of 16 pixels */
        for(i = 0; i < sliceRows; i++)
            fread(image+scan*i, 1, sliceLumWidth, file);
    }
    if (sliceCbSize) {
        fseek(file, frameOffset + sliceCbOffset, SEEK_SET);
        if ((width & 0xf) == 0)
            fread(image + sliceLumSize, 1, sliceCbWidth*sliceRows/2, file);
        else {
            i32 scan = width16*cbRowBpp/8;
            /* Read the chrominance so that scan (=stride) is multiple of 8 pixels */
            for(i = 0; i < sliceRows/2; i++)
                fread(image + sliceLumSize + scan*i, 1, sliceCbWidth, file);
        }
    }
    if (sliceCrSize) {
        fseek(file, frameOffset + sliceCrOffset, SEEK_SET);
        if ((width & 0xf) == 0)
            fread(image + sliceLumSize + sliceCbSize, 1, sliceCrWidth*sliceRows/2, file);
        else {
            i32 scan = width16*crRowBpp/8;
            /* Read the chrominance so that scan (=stride) is multiple of 8 pixels */
            for(i = 0; i < sliceRows/2; i++)
                fread(image + sliceLumSize + sliceCbSize + scan*i, 1, sliceCbWidth, file);
        }
    }

    /* Stop if last VOP of the file */
    if(feof(file)) {
        fprintf(stderr, "\nI can't read VOP no: %d ", frameNum);
        fprintf(stderr, "from file: %s\n", name);
        fclose(file);
        return -1;
    }

#ifndef ASIC_WAVE_TRACE_TRIGGER
    printf("OK\n");
    fflush(stdout);
#endif

    fclose(file);

    return 0;
}

/*------------------------------------------------------------------------------

    Help

------------------------------------------------------------------------------*/
void Help(void)
{
    fprintf(stdout, "Usage:  %s [options] -i inputfile\n", "jpeg_testenc");
    fprintf(stdout,
            "  -H    --help              Display this help.\n"
            "  -W[n] --write             0=NO, 1=YES write output. [1]\n"
            "  -i[s] --input             Read input from file. [input.yuv]\n"
            "  -I[s] --inputThumb        Read thumbnail input from file. [thumbnail.jpg]\n"
            "  -o[s] --output            Write output to file. [stream.jpg]\n"
            "  -a[n] --firstPic          First picture of input file. [0]\n"
            "  -b[n] --lastPic           Last picture of input file. [0]\n"
            "  -w[n] --lumWidthSrc       Width of source image. [176]\n"
            "  -h[n] --lumHeightSrc      Height of source image. [144]\n");
    fprintf(stdout,
            "  -x[n] --width             Width of output image. [--lumWidthSrc]\n"
            "  -y[n] --height            Height of output image. [--lumHeightSrc]\n"
            "  -X[n] --horOffsetSrc      Output image horizontal offset. [0]\n"
            "  -Y[n] --verOffsetSrc      Output image vertical offset. [0]\n");
    fprintf(stdout,
            "  -R[n] --restartInterval   Restart interval in MCU rows. [0]\n"
            "  -q[n] --qLevel            0..10, quantization scale. [1]\n"
            "                            10 = use testbench defined qtable\n"
            "  -g[n] --frameType         Input YUV format. [0]\n"
            "                               0 - YUV420 planar CbCr (IYUV/I420)\n"
            "                               1 - YUV420 semiplanar CbCr (NV12)\n"
            "                               2 - YUV420 semiplanar CrCb (NV21)\n"
            "                               3 - YUYV422 interleaved (YUYV/YUY2)\n"
            "                               4 - UYVY422 interleaved (UYVY/Y422)\n"
            "                               5 - RGB565 16bpp\n"
            "                               6 - BRG565 16bpp\n"
            "                               7 - RGB555 16bpp\n"
            "                               8 - BRG555 16bpp\n"
            "                               9 - RGB444 16bpp\n"
            "                               10 - BGR444 16bpp\n"
            "                               11 - RGB888 32bpp\n"
            "                               12 - BGR888 32bpp\n"
            "                               13 - RGB101010 32bpp\n"
            "  -v[n] --colorConversion   RGB to YCbCr color conversion type. [0]\n"
            "                               0 - BT.601\n"
            "                               1 - BT.709\n"
            "                               2 - User defined\n"
            "  -G[n] --rotation          Rotate input image. [0]\n"
            "                               0 - disabled\n"
            "                               1 - 90 degrees right\n"
            "                               2 - 90 degrees right\n"
            "  -p[n] --codingType        0=whole frame, 1=partial frame encoding. [0]\n"
            "  -m[n] --codingMode        0=YUV 4:2:0, 1=YUV 4:2:2. [0]\n"
            "  -t[n] --markerType        Quantization/Huffman table markers. [0]\n"
            "                               0 = single marker\n"
            "                               1 = multi marker\n"
            "  -u[n] --units             Units type of x- and y-density. [0]\n"
            "                               0 = pixel aspect ratio\n"
            "                               1 = dots/inch\n"
            "                               2 = dots/cm\n"
            "  -k[n] --xdensity          Xdensity to APP0 header. [1]\n"
            "  -l[n] --ydensity          Ydensity to APP0 header. [1]\n");
    fprintf(stdout,
            "  -T[n] --thumbnail         0=NO, 1=JPEG, 2=RGB8, 3=RGB24 Thumbnail to stream. [0]\n"
            "  -K[n] --widthThumb        Width of thumbnail output image. [32]\n"
            "  -L[n] --heightThumb       Height of thumbnail output image. [32]\n");
#ifdef TB_DEFINED_COMMENT
    fprintf(stdout, 
            "\n   Using comment values defined in testbench!\n");
#else
    fprintf(stdout,
            "  -c[n] --comLength         Comment header data length. [0]\n"
            "  -C[s] --comFile           Comment header data file. [com.txt]\n");
#endif
    fprintf(stdout,
            "\nTesting parameters that are not supported for end-user:\n"
            "  -P[n] --trigger           Logic Analyzer trigger at picture <n>. [-1]\n"
            "\n");
}

/*------------------------------------------------------------------------------

    Write encoded stream to file

------------------------------------------------------------------------------*/
void WriteStrm(FILE * fout, u32 * strmbuf, u32 size, u32 endian)
{

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

#ifndef ASIC_WAVE_TRACE_TRIGGER
    printf("Writing stream (%i bytes)... ", size);
    fflush(stdout);
#endif

    fwrite(strmbuf, 1, size, fout);

#ifndef ASIC_WAVE_TRACE_TRIGGER
    printf("OK\n");
    fflush(stdout);
#endif

}

#ifdef SEPARATE_FRAME_OUTPUT
/*------------------------------------------------------------------------------

    Write encoded frame to file

------------------------------------------------------------------------------*/
void WriteFrame(char *filename, u32 * strmbuf, u32 size)
{
    FILE *fp;

    fp = fopen(filename, "ab");

        if(fp)
        {
            fwrite(strmbuf, 1, size, fp);
            fclose(fp);
        }
}
#endif

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

    API tracing

------------------------------------------------------------------------------*/
void JpegEnc_Trace(const char *msg)
{
    static FILE *fp = NULL;

    if(fp == NULL)
        fp = fopen("api.trc", "wt");

    if(fp)
        fprintf(fp, "%s\n", msg);
}
