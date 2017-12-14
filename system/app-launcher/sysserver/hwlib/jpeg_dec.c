/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : JPEG Decoder Testbench
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include "trace.h"

/* NOTE! This is needed when user allocated memory used */
#ifdef LINUX
#include <fcntl.h>
#include <sys/mman.h>
#endif /* #ifdef LINUX */

#include "jpegdecapi.h"
#include "dwl.h"
#include "jpegdeccontainer.h"

#ifdef PP_PIPELINE_ENABLED
#include "ppapi.h"
#include "pptestbench.h"
#endif

#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif

#include "deccfg.h"
#include "regdrv.h"
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define DEC_X170_SERVICE_MERGE_DISABLE 0

#ifndef MAX_PATH_
#define MAX_PATH   256  /* maximum lenght of the file path */
#endif
#define DEFAULT -1
#define JPEG_INPUT_BUFFER 0x5120
#define IMAPFB_CONFIG_VIDEO_SIZE 20005

/* SW/SW testing, read stream trace file */
FILE *fStreamTrace = NULL;

static u32 memAllocation = 0;

/* memory parameters */
static u32 out_pic_size_luma;
static u32 out_pic_size_chroma;
static fd_mem;
static JpegDecLinearMem outputAddressY;
static JpegDecLinearMem outputAddressCbCr;
static u32 frameReady = 0;
static u32 slicedOutputUsed = 0;
static u32 mode = 0;
static u32 writeOutput = 0;
static u32 sizeLuma = 0;
static u32 sizeChroma = 0;
static u32 sliceToUser = 0;
static u32 sliceSize = 0;
static u32 nonInterleaved = 0;
static i32 fullSliceCounter = -1;
static u32 output_picture_endian = DEC_X170_OUTPUT_PICTURE_ENDIAN;
static u32 service_merge_disable = DEC_X170_SERVICE_MERGE_DISABLE;

/* stream start address */
u8 *byteStrmStart;

/* user allocated output */
DWLLinearMem_t userAllocLuma;
DWLLinearMem_t userAllocChroma;
DWLLinearMem_t userAllocCr;

/* progressive parameters */
static u32 scanCounter = 0;
static u32 progressive = 0;
static u32 scanReady = 0;
static u32 is8170HW = 0;

/* prototypes */
u32 allocMemory(JpegDecInst decInst, JpegDecImageInfo * imageInfo,
                JpegDecInput * jpegIn);
void calcSize(JpegDecImageInfo * imageInfo, u32 picMode);
void WriteOutput(u8 * dataLuma, u32 picSizeLuma, u8 * dataChroma,
                 u32 picSizeChroma, u32 picMode);
void WriteOutputLuma(u8 * dataLuma, u32 picSizeLuma, u32 picMode);
void WriteOutputChroma(u8 * dataChroma, u32 picSizeChroma, u32 picMode);
void WriteFullOutput(u32 picMode);

void handleSlicedOutput(JpegDecImageInfo * imageInfo, JpegDecInput * jpegIn,
                        JpegDecOutput * jpegOut);

void WriteCroppedOutput(JpegDecImageInfo * info, u8 * dataLuma, u8 * dataCb,
                        u8 * dataCr);

void WriteProgressiveOutput(u32 sizeLuma, u32 sizeChroma, u32 mode,
                            u8 * dataLuma, u8 * dataCb, u8 * dataCr);
void printJpegVersion(void);

void decsw_performance(void)
{
}

void *JpegDecMalloc(unsigned int size);
void *JpegDecMemset(void *ptr, int c, unsigned int size);
void JpegDecFree(void *ptr);

void PrintJpegRet(JpegDecRet * pJpegRet);
u32 FindImageInfoEnd(u8 * pStream, u32 streamLength, u32 * pOffset);

u32 planarOutput = 0;
u32 bFrames = 0;

#ifdef ASIC_TRACE_SUPPORT
u32 picNumber;
#endif


#if defined(ASIC_TRACE_SUPPORT) || defined(SYSTEM_VERIFICATION)
extern u32 useJpegIdct;
#endif

#ifdef ASIC_TRACE_SUPPORT
extern u32 gHwVer;
#endif

#ifdef JPEG_EVALUATION
extern u32 gHwVer;
#endif

u32 crop = 0;

struct imapfb_info {
	unsigned int width;
	unsigned int height;
	unsigned int win_width;
	unsigned int win_height;
	unsigned int scaler_buff;
	int format;
};

void set_osd0(int fd, unsigned int addr, int w, int h, int format)
{
	struct imapfb_info iinfo;
	iinfo.width = w;
	iinfo.height = h;
	iinfo.win_width = w;
	iinfo.win_height = h;
	iinfo.scaler_buff = addr;
	iinfo.format   = format; // 2:NV21 1:NV12 0:other

	if (ioctl(fd, IMAPFB_CONFIG_VIDEO_SIZE, &iinfo) == -1) {
		printf("set osd0 size failed\n");
		return ;
	}
	//	ioctl(fd, FBIOBLANK, FB_BLANK_UNBLANK);
}

int video_display_do(unsigned int phyAddr, int w, int h, int PixelFormat)
{
	int fd0 = 0;
	fd0 = open("/dev/fb0", O_RDWR);
	if(fd0 < 0) {
		printf("open /dev/fb0 fail !\n");
		return -1;
	}

	struct fb_var_screeninfo vinfo;
	ioctl(fd0, FBIOGET_VSCREENINFO, &vinfo);
	printf("osd0 info: bpp%d, std%d, x%d, y%d, vx%d, vy%d\n",
	 vinfo.bits_per_pixel, vinfo.nonstd, vinfo.xres, vinfo.yres,
	 vinfo.xres_virtual, vinfo.yres_virtual);

	set_osd0(fd0, phyAddr, w, h, PixelFormat);

	vinfo.reserved[0] = phyAddr;
    ioctl(fd0, FBIOPAN_DISPLAY, &vinfo);

	close(fd0);
    return 0;
}

int jpeg_dec(char *filename)
{
    u8 *inputBuffer = NULL;
    u32 len;
    u32 streamTotalLen = 0;
    u32 streamSeekLen = 0;

    u32 streamInFile = 0;
    u32 mcuSizeDivider = 0;

    i32 i, j = 0;
    u32 tmp = 0;
    u32 size = 0;
    u32 loop;
    u32 frameCounter = 0;
    u32 inputReadType = 0;
    i32 bufferSize = 0;
    u32 amountOfMCUs = 0;
    u32 mcuInRow = 0;

    JpegDecInst jpeg;
    JpegDecRet jpegRet;
    JpegDecImageInfo imageInfo;
    JpegDecInput jpegIn;
    JpegDecOutput jpegOut;
    JpegDecApiVersion decVer;
    JpegDecBuild decBuild;

    DWLLinearMem_t streamMem;
    DWLLinearMem_t OutStreamMem;

    u8 *pImage = NULL;

    FILE *fout = NULL;
    FILE *fIn = NULL;

    u32 clock_gating = DEC_X170_INTERNAL_CLOCK_GATING;
    u32 latency_comp = DEC_X170_LATENCY_COMPENSATION;
    u32 bus_burst_length = DEC_X170_BUS_BURST_LENGTH;
    u32 asic_service_priority = DEC_X170_ASIC_SERVICE_PRIORITY;
    u32 data_discard = DEC_X170_DATA_DISCARD_ENABLE;

#ifdef PP_PIPELINE_ENABLED
    PPApiVersion ppVer;
    PPBuild ppBuild;
#endif

    u32 streamHeaderCorrupt = 0;
    u32 seedRnd = 0;
    u32 streamBitSwap = 0;
    u32 streamTruncate = 0;
    FILE *fTBCfg;
    u32 imageInfoLength = 0;

#ifdef ASIC_TRACE_SUPPORT
    gHwVer = 8190;   /* default to 8190 mode */
#endif

#ifdef JPEG_EVALUATION_8170
    gHwVer = 8170;
#elif JPEG_EVALUATION_8190
    gHwVer = 8190;
#elif JPEG_EVALUATION_9170
    gHwVer = 9170;
#elif JPEG_EVALUATION_9190
    gHwVer = 9190;
#elif JPEG_EVALUATION_G1
    gHwVer = 10000;
#endif

#ifndef EXPIRY_DATE
#define EXPIRY_DATE (u32)0xFFFFFFFF
#endif /* EXPIRY_DATE */

    /* expiry stuff */
    {
        u8 tmBuf[7];
        time_t sysTime;
        struct tm *tm;
        u32 tmp1;

        /* Check expiry date */
        time(&sysTime);
        tm = localtime(&sysTime);
        strftime(tmBuf, sizeof(tmBuf), "%y%m%d", tm);
        tmp1 = 1000000 + atoi(tmBuf);
        if(tmp1 > (EXPIRY_DATE) && (EXPIRY_DATE) > 1)
        {
            fprintf(stderr,
                    "EVALUATION PERIOD EXPIRED.\n"
                    "Please contact On2 Sales.\n");
            return -1;
        }
    }

    /* allocate memory for stream buffer. if unsuccessful -> exit */
    streamMem.virtualAddress = NULL;
    streamMem.busAddress = 0;
    OutStreamMem.virtualAddress = NULL;
    OutStreamMem.busAddress = 0;



    fprintf(stdout, "\n* * * * * * * * * * * * * * * * \n\n\n"
            "      "
            "X170 JPEG TESTBENCH\n" "\n\n* * * * * * * * * * * * * * * * \n");

    /* reset input */
    jpegIn.streamBuffer.pVirtualAddress = NULL;
    jpegIn.streamBuffer.busAddress = 0;
    jpegIn.streamLength = 0;
    jpegIn.pictureBufferY.pVirtualAddress = NULL;
    jpegIn.pictureBufferY.busAddress = 0;
    jpegIn.pictureBufferCbCr.pVirtualAddress = NULL;
    jpegIn.pictureBufferCbCr.busAddress = 0;

    /* reset output */
    jpegOut.outputPictureY.pVirtualAddress = NULL;
    jpegOut.outputPictureY.busAddress = 0;
    jpegOut.outputPictureCbCr.pVirtualAddress = NULL;
    jpegOut.outputPictureCbCr.busAddress = 0;
    jpegOut.outputPictureCr.pVirtualAddress = NULL;
    jpegOut.outputPictureCr.busAddress = 0;

    /* reset imageInfo */
    imageInfo.displayWidth = 0;
    imageInfo.displayHeight = 0;
    imageInfo.outputWidth = 0;
    imageInfo.outputHeight = 0;
    imageInfo.version = 0;
    imageInfo.units = 0;
    imageInfo.xDensity = 0;
    imageInfo.yDensity = 0;
    imageInfo.outputFormat = 0;
    imageInfo.thumbnailType = 0;
    imageInfo.displayWidthThumb = 0;
    imageInfo.displayHeightThumb = 0;
    imageInfo.outputWidthThumb = 0;
    imageInfo.outputHeightThumb = 0;
    imageInfo.outputFormatThumb = 0;

    /* set default */
    bufferSize = 0;
    jpegIn.sliceMbSet = 0;
    jpegIn.bufferSize = 0;

	printJpegVersion();

    /* Print API and build version numbers */
    decVer = JpegGetAPIVersion();
    decBuild = JpegDecGetBuild();

    /* Version */
    fprintf(stdout,
            "\nX170 JPEG Decoder API v%d.%d - SW build: %d - HW build: %x\n",
            decVer.major, decVer.minor, decBuild.swBuild, decBuild.hwBuild);

#ifdef PP_PIPELINE_ENABLED
    /* Print API and build version numbers */
    ppVer = PPGetAPIVersion();
    ppBuild = PPGetBuild();

    /* Version */
    fprintf(stdout,
            "\nX170 PP API v%d.%d - SW build: %d - HW build: %x\n",
            ppVer.major, ppVer.minor, ppBuild.swBuild, ppBuild.hwBuild);

#endif

    /* check if 8170 HW */
    is8170HW = (decBuild.hwBuild >> 16) == 0x8170U ? 1 : 0;

    /*TBPrintCfg(&tbCfg); */
    printf("Decoder Memory Allocation %d\n", memAllocation);
    printf("Decoder Jpeg Input Buffer Size %d\n", jpegIn.bufferSize);
    printf("Decoder Slice MB Set %d\n", jpegIn.sliceMbSet);
    printf("Decoder Clock Gating %d\n", clock_gating);
    printf("Decoder Data Discard %d\n", data_discard);
    printf("Decoder Latency Compensation %d\n", latency_comp);
    printf("Decoder Output Picture Endian %d\n", output_picture_endian);
    printf("Decoder Bus Burst Length %d\n", bus_burst_length);
    printf("Decoder Asic Service Priority %d\n", asic_service_priority);

    printf("TB Seed Rnd %d\n", seedRnd);
    printf("TB Stream Truncate %d\n", streamTruncate);
    printf("TB Stream Header Corrupt %d\n", streamHeaderCorrupt);


    /******** PHASE 1 ********/
    fprintf(stdout, "\nPHASE 1: INIT JPEG DECODER\n");

    /* Jpeg initialization */
    jpegRet = JpegDecInit(&jpeg, &streamMem, 0);
    if(jpegRet != JPEGDEC_OK)
    {
        /* Handle here the error situation */
        PrintJpegRet(&jpegRet);
        goto end;
    }

    /* NOTE: The registers should not be used outside decoder SW for other
     * than compile time setting test purposes */
    SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs, HWIF_DEC_LATENCY,
                   latency_comp);
    SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs, HWIF_DEC_CLK_GATE_E,
                   clock_gating);
    SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs, HWIF_DEC_OUT_ENDIAN,
                   output_picture_endian);
    SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs, HWIF_DEC_MAX_BURST,
                   bus_burst_length);
    if ((G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs, HWIF_PRIORITY_MODE,
                       asic_service_priority);
        SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs, HWIF_DEC_DATA_DISC_E,
                       data_discard);
    }

    fprintf(stdout, "PHASE 1: INIT JPEG DECODER successful\n");

    /******** PHASE 2 ********/
    fprintf(stdout, "\nPHASE 2: OPEN/READ FILE \n");

#ifdef ASIC_TRACE_SUPPORT
    tmp = openTraceFiles();
    if(!tmp)
    {
        fprintf(stdout, "Unable to open trace file(s)\n");
    }
#endif

  reallocate_input_buffer:

    /* Reading input file */
    fIn = fopen(filename, "rb");
    if(fIn == NULL)
    {
        fprintf(stdout, "Unable to open input file: %s\n", filename);
		return -1;
    }

    /* file i/o pointer to full */
    fseek(fIn, 0L, SEEK_END);
    len = ftell(fIn);
    rewind(fIn);

    /* Handle input buffer load */
    if(jpegIn.bufferSize)
    {
        if(len > jpegIn.bufferSize)
        {
            streamTotalLen = len;
            len = jpegIn.bufferSize;
        }
        else
        {
            streamTotalLen = len;
            len = streamTotalLen;
            jpegIn.bufferSize = 0;
        }
    }
    else
    {
        jpegIn.bufferSize = 0;
        streamTotalLen = len;
    }

    streamInFile = streamTotalLen;

    /* NOTE: The DWL should not be used outside decoder SW
     * here we call it because it is the easiest way to get
     * dynamically allocated linear memory
     * */

    /* allocate memory for stream buffer. if unsuccessful -> exit */
    if(G1DWLMallocLinear
       (((JpegDecContainer *) jpeg)->dwl, len, &streamMem) != DWL_OK)
    {
        fprintf(stdout, "UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n");
        goto end;
    }

    fprintf(stdout, "\t-Input: Allocated buffer: virt: 0x%08x bus: 0x%08x\n",
            streamMem.virtualAddress, streamMem.busAddress);

    /* memset input */
    (void) G1DWLmemset(streamMem.virtualAddress, 0, len);

    byteStrmStart = (u8 *) streamMem.virtualAddress;
    if(byteStrmStart == NULL)
    {
        fprintf(stdout, "UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n");
        goto end;
    }

    /* read input stream from file to buffer and close input file */
    fread(byteStrmStart, sizeof(u8), len, fIn);

    fclose(fIn);

    /* initialize JpegDecDecode input structure */
    jpegIn.streamBuffer.pVirtualAddress = (u32 *) byteStrmStart;
    jpegIn.streamBuffer.busAddress = streamMem.busAddress;
    jpegIn.streamLength = streamTotalLen;


    fprintf(stdout, "\nPHASE 2: OPEN/READ FILE successful\n");

    /******** PHASE 3 ********/
    fprintf(stdout, "\nPHASE 3: GET IMAGE INFO\n");

    jpegRet = FindImageInfoEnd(byteStrmStart, len, &imageInfoLength);
    printf("\timageInfoLength %d\n", imageInfoLength);
    /* If image info is not found, do not corrupt the header */
    if(jpegRet != 0)
    {
        if(streamHeaderCorrupt)
        {
            u32 ret = 0;

           // ret =
           //     TBRandomizeBitSwapInStream(byteStrmStart, imageInfoLength,
           //                                tbCfg.tbParams.streamBitSwap);
            if(ret != 0)
            {
                printf("RANDOM STREAM ERROR FAILED\n");
                goto end;
            }
        }
		printf("FindImageInfoEnd fail\n");
		goto end;
    }

    /* Get image information of the JFIF and decode JFIF header */
    jpegRet = JpegDecGetImageInfo(jpeg, &jpegIn, &imageInfo);
    if(jpegRet != JPEGDEC_OK)
    {
        /* Handle here the error situation */
        PrintJpegRet(&jpegRet);
        if(JPEGDEC_INCREASE_INPUT_BUFFER == jpegRet)
        {
            DWLFreeLinear(((JpegDecContainer *) jpeg)->dwl, &streamMem);
            jpegIn.bufferSize += 256;
            goto reallocate_input_buffer;
        }
        else
        {
            goto end;
        }
    }

    /*  ******************** THUMBNAIL **************************** */
    /* Select if Thumbnail or full resolution image will be decoded */
    if(imageInfo.thumbnailType == JPEGDEC_THUMBNAIL_JPEG)
    {
        /* decode thumbnail */
        fprintf(stdout, "\t-JPEG THUMBNAIL IN STREAM\n");
        fprintf(stdout, "\t-JPEG THUMBNAIL INFO\n");
        fprintf(stdout, "\t\t-JPEG thumbnail width: %d\n",
                imageInfo.outputWidthThumb);
        fprintf(stdout, "\t\t-JPEG thumbnail height: %d\n",
                imageInfo.outputHeightThumb);

        /* stream type */
        switch (imageInfo.codingModeThumb)
        {
        case JPEGDEC_BASELINE:
            fprintf(stdout, "\t\t-JPEG: STREAM TYPE: JPEGDEC_BASELINE\n");
            break;
        case JPEGDEC_PROGRESSIVE:
            fprintf(stdout, "\t\t-JPEG: STREAM TYPE: JPEGDEC_PROGRESSIVE\n");
            break;
        case JPEGDEC_NONINTERLEAVED:
            fprintf(stdout, "\t\t-JPEG: STREAM TYPE: JPEGDEC_NONINTERLEAVED\n");
            break;
        }

        if(imageInfo.outputFormatThumb)
        {
            switch (imageInfo.outputFormatThumb)
            {
            case JPEGDEC_YCbCr400:
                fprintf(stdout,
                        "\t\t-JPEG: THUMBNAIL OUTPUT: JPEGDEC_YCbCr400\n");
                break;
            case JPEGDEC_YCbCr420_SEMIPLANAR:
                fprintf(stdout,
                        "\t\t-JPEG: THUMBNAIL OUTPUT: JPEGDEC_YCbCr420_SEMIPLANAR\n");
                break;
            case JPEGDEC_YCbCr422_SEMIPLANAR:
                fprintf(stdout,
                        "\t\t-JPEG: THUMBNAIL OUTPUT: JPEGDEC_YCbCr422_SEMIPLANAR\n");
                break;
            case JPEGDEC_YCbCr440:
                fprintf(stdout,
                        "\t\t-JPEG: THUMBNAIL OUTPUT: JPEGDEC_YCbCr440\n");
                break;
            case JPEGDEC_YCbCr411_SEMIPLANAR:
                fprintf(stdout,
                        "\t\t-JPEG: THUMBNAIL OUTPUT: JPEGDEC_YCbCr411_SEMIPLANAR\n");
                break;
            case JPEGDEC_YCbCr444_SEMIPLANAR:
                fprintf(stdout,
                        "\t\t-JPEG: THUMBNAIL OUTPUT: JPEGDEC_YCbCr444_SEMIPLANAR\n");
                break;
            }
        }
        jpegIn.decImageType = JPEGDEC_THUMBNAIL;
    }
    else if(imageInfo.thumbnailType == JPEGDEC_NO_THUMBNAIL)
    {
        /* decode full image */
        fprintf(stdout,
                "\t-NO THUMBNAIL IN STREAM ==> Decode full resolution image\n");
        jpegIn.decImageType = JPEGDEC_IMAGE;
    }
    else if(imageInfo.thumbnailType == JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT)
    {
        /* decode full image */
        fprintf(stdout,
                "\tNOT SUPPORTED THUMBNAIL IN STREAM ==> Decode full resolution image\n");
        jpegIn.decImageType = JPEGDEC_IMAGE;
    }

    fprintf(stdout, "\t-JPEG FULL RESOLUTION INFO\n");
	fprintf(stdout, "\t\t-JPEG width: %d\n", imageInfo.outputWidth);
    fprintf(stdout, "\t\t-JPEG height: %d\n", imageInfo.outputHeight);
    if(imageInfo.outputFormat)
    {
        switch (imageInfo.outputFormat)
        {
        case JPEGDEC_YCbCr400:
            fprintf(stdout,
                    "\t\t-JPEG: FULL RESOLUTION OUTPUT: JPEGDEC_YCbCr400\n");
#ifdef ASIC_TRACE_SUPPORT
            trace_jpegDecodingTools.sampling_4_0_0 = 1;
#endif
            break;
        case JPEGDEC_YCbCr420_SEMIPLANAR:
            fprintf(stdout,
                    "\t\t-JPEG: FULL RESOLUTION OUTPUT: JPEGDEC_YCbCr420_SEMIPLANAR\n");
#ifdef ASIC_TRACE_SUPPORT
            trace_jpegDecodingTools.sampling_4_2_0 = 1;
#endif
            break;
        case JPEGDEC_YCbCr422_SEMIPLANAR:
            fprintf(stdout,
                    "\t\t-JPEG: FULL RESOLUTION OUTPUT: JPEGDEC_YCbCr422_SEMIPLANAR\n");
#ifdef ASIC_TRACE_SUPPORT
            trace_jpegDecodingTools.sampling_4_2_2 = 1;
#endif
            break;
        case JPEGDEC_YCbCr440:
            fprintf(stdout,
                    "\t\t-JPEG: FULL RESOLUTION OUTPUT: JPEGDEC_YCbCr440\n");
#ifdef ASIC_TRACE_SUPPORT
            trace_jpegDecodingTools.sampling_4_4_0 = 1;
#endif
            break;
        case JPEGDEC_YCbCr411_SEMIPLANAR:
            fprintf(stdout,
                    "\t\t-JPEG: FULL RESOLUTION OUTPUT: JPEGDEC_YCbCr411_SEMIPLANAR\n");
#ifdef ASIC_TRACE_SUPPORT
            trace_jpegDecodingTools.sampling_4_1_1 = 1;
#endif
            break;
        case JPEGDEC_YCbCr444_SEMIPLANAR:
            fprintf(stdout,
                    "\t\t-JPEG: FULL RESOLUTION OUTPUT: JPEGDEC_YCbCr444_SEMIPLANAR\n");
#ifdef ASIC_TRACE_SUPPORT
            trace_jpegDecodingTools.sampling_4_4_4 = 1;
#endif
            break;
        }
    }

    /* stream type */
    switch (imageInfo.codingMode)
    {
    case JPEGDEC_BASELINE:
        fprintf(stdout, "\t\t-JPEG: STREAM TYPE: JPEGDEC_BASELINE\n");
        break;
    case JPEGDEC_PROGRESSIVE:
        fprintf(stdout, "\t\t-JPEG: STREAM TYPE: JPEGDEC_PROGRESSIVE\n");
#ifdef ASIC_TRACE_SUPPORT
        trace_jpegDecodingTools.progressive = 1;
#endif
        break;
    case JPEGDEC_NONINTERLEAVED:
        fprintf(stdout, "\t\t-JPEG: STREAM TYPE: JPEGDEC_NONINTERLEAVED\n");
        break;
    }

    if(imageInfo.thumbnailType == JPEGDEC_THUMBNAIL_JPEG)
    {
        fprintf(stdout, "\t-JPEG ThumbnailType: JPEG\n");
#ifdef ASIC_TRACE_SUPPORT
        trace_jpegDecodingTools.thumbnail = 1;
#endif
    }
    else if(imageInfo.thumbnailType == JPEGDEC_NO_THUMBNAIL)
        fprintf(stdout, "\t-JPEG ThumbnailType: NO THUMBNAIL\n");
    else if(imageInfo.thumbnailType == JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT)
        fprintf(stdout, "\t-JPEG ThumbnailType: NOT SUPPORTED THUMBNAIL\n");

    fprintf(stdout, "PHASE 3: GET IMAGE INFO successful\n");

    /* TB SPECIFIC == LOOP IF THUMBNAIL IN JFIF */
    /* Decode JFIF */
    if(jpegIn.decImageType == JPEGDEC_THUMBNAIL)
        frameCounter = 2;
    else
        frameCounter = 1;

#ifdef ASIC_TRACE_SUPPORT
    /* Handle incorrect slice size for HW testing */
    if(jpegIn.sliceMbSet > (imageInfo.outputHeight >> 4))
    {
        jpegIn.sliceMbSet = (imageInfo.outputHeight >> 4);
        printf("FIXED Decoder Slice MB Set %d\n", jpegIn.sliceMbSet);
    }
#endif

    /* no slice mode supported in progressive || non-interleaved ==> force to full mode */
    if((jpegIn.decImageType == JPEGDEC_THUMBNAIL &&
        imageInfo.codingModeThumb == JPEGDEC_PROGRESSIVE) ||
       (jpegIn.decImageType == JPEGDEC_IMAGE &&
        imageInfo.codingMode == JPEGDEC_PROGRESSIVE))
        jpegIn.sliceMbSet = 0;

    /******** PHASE 4 ********/
    /* Decode Image */
    for(loop = 0; loop < frameCounter; loop++)
    {
        /* Tn */
        if(loop == 0 && frameCounter > 1)
        {
            fprintf(stdout, "\nPHASE 4: DECODE FRAME: THUMBNAIL\n");
            /* thumbnail mode */
            mode = 1;
        }
        else if(loop == 0 && frameCounter == 1)
        {
            fprintf(stdout, "\nPHASE 4: DECODE FRAME: FULL RESOLUTION\n");
            /* full mode */
            mode = 0;
        }
        else
        {
            fprintf(stdout, "\nPHASE 4: DECODE FRAME: FULL RESOLUTION\n");
            /* full mode */
            mode = 0;
        }

        /* if input (only full, not tn) > 4096 MCU      */
        /* ==> force to slice mode                                      */
        if(mode == 0)
        {
            /* calculate MCU's */
            if(imageInfo.outputFormat == JPEGDEC_YCbCr400 ||
               imageInfo.outputFormat == JPEGDEC_YCbCr444_SEMIPLANAR)
            {
                amountOfMCUs =
                    ((imageInfo.outputWidth * imageInfo.outputHeight) / 64);
                mcuInRow = (imageInfo.outputWidth / 8);
            }
            else if(imageInfo.outputFormat == JPEGDEC_YCbCr420_SEMIPLANAR)
            {
                /* 265 is the amount of luma samples in MB for 4:2:0 */
                amountOfMCUs =
                    ((imageInfo.outputWidth * imageInfo.outputHeight) / 256);
                mcuInRow = (imageInfo.outputWidth / 16);
            }
            else if(imageInfo.outputFormat == JPEGDEC_YCbCr422_SEMIPLANAR)
            {
                /* 128 is the amount of luma samples in MB for 4:2:2 */
                amountOfMCUs =
                    ((imageInfo.outputWidth * imageInfo.outputHeight) / 128);
                mcuInRow = (imageInfo.outputWidth / 16);
            }
            else if(imageInfo.outputFormat == JPEGDEC_YCbCr440)
            {
                /* 128 is the amount of luma samples in MB for 4:4:0 */
                amountOfMCUs =
                    ((imageInfo.outputWidth * imageInfo.outputHeight) / 128);
                mcuInRow = (imageInfo.outputWidth / 8);
            }
            else if(imageInfo.outputFormat == JPEGDEC_YCbCr411_SEMIPLANAR)
            {
                amountOfMCUs =
                    ((imageInfo.outputWidth * imageInfo.outputHeight) / 256);
                mcuInRow = (imageInfo.outputWidth / 32);
            }

            /* set mcuSizeDivider for slice size count */
            if(imageInfo.outputFormat == JPEGDEC_YCbCr400 ||
               imageInfo.outputFormat == JPEGDEC_YCbCr440 ||
               imageInfo.outputFormat == JPEGDEC_YCbCr444_SEMIPLANAR)
                mcuSizeDivider = 2;
            else
                mcuSizeDivider = 1;

#ifdef ASIC_TRACE_SUPPORT
            if(is8170HW)
            {
                /* over max MCU ==> force to slice mode */
                if((jpegIn.sliceMbSet == 0) &&
                   (amountOfMCUs > JPEGDEC_MAX_SLICE_SIZE))
                {
                    do
                    {
                        jpegIn.sliceMbSet++;
                    }
                    while(((jpegIn.sliceMbSet * (mcuInRow / mcuSizeDivider)) +
                           (mcuInRow / mcuSizeDivider)) <
                          JPEGDEC_MAX_SLICE_SIZE);
                    printf("Force to slice mode ==> Decoder Slice MB Set %d\n",
                           jpegIn.sliceMbSet);
                }
            }
            else
            {
                /* 8190 and over 16M ==> force to slice mode */
                if((jpegIn.sliceMbSet == 0) &&
                   ((imageInfo.outputWidth * imageInfo.outputHeight) >
                    JPEGDEC_MAX_PIXEL_AMOUNT))
                {
                    do
                    {
                        jpegIn.sliceMbSet++;
                    }
                    while(((jpegIn.sliceMbSet * (mcuInRow / mcuSizeDivider)) +
                           (mcuInRow / mcuSizeDivider)) <
                          JPEGDEC_MAX_SLICE_SIZE_8190);
                    printf
                        ("Force to slice mode (over 16M) ==> Decoder Slice MB Set %d\n",
                         jpegIn.sliceMbSet);
                }
            }
#else
            if(is8170HW)
            {
                /* over max MCU ==> force to slice mode */
                if((jpegIn.sliceMbSet == 0) &&
                   (amountOfMCUs > JPEGDEC_MAX_SLICE_SIZE))
                {
                    do
                    {
                        jpegIn.sliceMbSet++;
                    }
                    while(((jpegIn.sliceMbSet * (mcuInRow / mcuSizeDivider)) +
                           (mcuInRow / mcuSizeDivider)) <
                          JPEGDEC_MAX_SLICE_SIZE);
                    printf("Force to slice mode ==> Decoder Slice MB Set %d\n",
                           jpegIn.sliceMbSet);
                }
            }
            else
            {
                /* 8190 and over 16M ==> force to slice mode */
                if((jpegIn.sliceMbSet == 0) &&
                   ((imageInfo.outputWidth * imageInfo.outputHeight) >
                    JPEGDEC_MAX_PIXEL_AMOUNT))
                {
                    do
                    {
                        jpegIn.sliceMbSet++;
                    }
                    while(((jpegIn.sliceMbSet * (mcuInRow / mcuSizeDivider)) +
                           (mcuInRow / mcuSizeDivider)) <
                          JPEGDEC_MAX_SLICE_SIZE_8190);
                    printf
                        ("Force to slice mode (over 16M) ==> Decoder Slice MB Set %d\n",
                         jpegIn.sliceMbSet);
                }
            }
#endif
        }

        /* if user allocated memory */
        if(memAllocation)
        {
            fprintf(stdout, "\n\t-JPEG: USER ALLOCATED MEMORY\n");
            jpegRet = allocMemory(jpeg, &imageInfo, &jpegIn);
            if(jpegRet != JPEGDEC_OK)
            {
                /* Handle here the error situation */
                PrintJpegRet(&jpegRet);
                goto end;
            }
            fprintf(stdout, "\t-JPEG: USER ALLOCATED MEMORY successful\n\n");
        }

        /*  Now corrupt only data beyong image info */
        if(streamBitSwap)
        {
            //jpegRet =
            //    TBRandomizeBitSwapInStream(byteStrmStart + imageInfoLength,
            //                               len - imageInfoLength,
            //                               tbCfg.tbParams.streamBitSwap);
            if(jpegRet != 0)
            {
                printf("RANDOM STREAM ERROR FAILED\n");
                goto end;
            }
        }

        /* decode */
        do
        {
            jpegRet = JpegDecDecode(jpeg, &jpegIn, &jpegOut);

            if(jpegRet == JPEGDEC_FRAME_READY)
            {
                fprintf(stdout, "\t-JPEG: JPEGDEC_FRAME_READY\n");

                /* check if progressive ==> planar output */
                if((imageInfo.codingMode == JPEGDEC_PROGRESSIVE && mode == 0) ||
                   (imageInfo.codingModeThumb == JPEGDEC_PROGRESSIVE &&
                    mode == 1))
                {
                    progressive = 1;
                }

                if((imageInfo.codingMode == JPEGDEC_NONINTERLEAVED && mode == 0)
                   || (imageInfo.codingModeThumb == JPEGDEC_NONINTERLEAVED &&
                       mode == 1))
                    nonInterleaved = 1;
                else
                    nonInterleaved = 0;

                if(jpegIn.sliceMbSet && fullSliceCounter == -1)
                    slicedOutputUsed = 1;

                /* info to handleSlicedOutput */
                frameReady = 1;
            }
            else if(jpegRet == JPEGDEC_SCAN_PROCESSED)
            {
                /* TODO! Progressive scan ready... */
                fprintf(stdout, "\t-JPEG: JPEGDEC_SCAN_PROCESSED\n");

                /* progressive ==> planar output */
                if(imageInfo.codingMode == JPEGDEC_PROGRESSIVE)
                    progressive = 1;

                /* info to handleSlicedOutput */
                printf("SCAN %d READY\n", scanCounter);

                if(imageInfo.codingMode == JPEGDEC_PROGRESSIVE)
                {
                    /* calculate size for output */
                    calcSize(&imageInfo, mode);

                    printf("sizeLuma %d and sizeChroma %d\n", sizeLuma,
                           sizeChroma);

                    WriteProgressiveOutput(sizeLuma, sizeChroma, mode,
                                           (u8*)jpegOut.outputPictureY.
                                           pVirtualAddress,
                                           (u8*)jpegOut.outputPictureCbCr.
                                           pVirtualAddress,
                                           (u8*)jpegOut.outputPictureCr.
                                           pVirtualAddress);

                    scanCounter++;
                }

                /* update/reset */
                progressive = 0;
                scanReady = 0;

            }
            else if(jpegRet == JPEGDEC_SLICE_READY)
            {
                fprintf(stdout, "\t-JPEG: JPEGDEC_SLICE_READY\n");

                slicedOutputUsed = 1;

                /* calculate/write output of slice 
                 * and update output budder in case of 
                 * user allocated memory */
                if(jpegOut.outputPictureY.pVirtualAddress != NULL)
                    handleSlicedOutput(&imageInfo, &jpegIn, &jpegOut);

                scanCounter++;
            }
            else if(jpegRet == JPEGDEC_STRM_PROCESSED)
            {
                fprintf(stdout,
                        "\t-JPEG: JPEGDEC_STRM_PROCESSED ==> Load input buffer\n");

                /* update seek value */
                streamInFile -= len;
                streamSeekLen += len;

                if(streamInFile <= 0)
                {
                    fprintf(stdout, "\t\t==> Unable to load input buffer\n");
                    fprintf(stdout,
                            "\t\t\t==> TRUNCATED INPUT ==> JPEGDEC_STRM_ERROR\n");
                    jpegRet = JPEGDEC_STRM_ERROR;
                    goto strm_error;
                }

                if(streamInFile < len)
                {
                    len = streamInFile;
                }
				
                /* update the buffer size in case last buffer 
                   doesn't have the same amount of data as defined */
                if(len < jpegIn.bufferSize)
                {
                    jpegIn.bufferSize = len;
                }

                /* Reading input file */
                fIn = fopen(filename, "rb");
                if(fIn == NULL)
                {
                    fprintf(stdout, "Unable to open input file: %s\n", filename);
                    return -1;
                }

                /* file i/o pointer to full */
                fseek(fIn, streamSeekLen, SEEK_SET);
                /* read input stream from file to buffer and close input file */
                fread(byteStrmStart, sizeof(u8), len, fIn);
                fclose(fIn);

                /* update */
                jpegIn.streamBuffer.pVirtualAddress = (u32 *) byteStrmStart;
                jpegIn.streamBuffer.busAddress = streamMem.busAddress;

                if(streamBitSwap)
                {
                    //jpegRet =
                    //    TBRandomizeBitSwapInStream(byteStrmStart, len,
                    //                               tbCfg.tbParams.
                    //                               streamBitSwap);
                    if(jpegRet != 0)
                    {
                        printf("RANDOM STREAM ERROR FAILED\n");
                        goto end;
                    }
                }
            }
            else if(jpegRet == JPEGDEC_STRM_ERROR)
            {
              strm_error:

              if(jpegIn.sliceMbSet && fullSliceCounter == -1)
                  slicedOutputUsed = 1;    

                /* calculate/write output of slice 
                 * and update output budder in case of 
                 * user allocated memory */
                if(slicedOutputUsed &&
                   jpegOut.outputPictureY.pVirtualAddress != NULL)
                    handleSlicedOutput(&imageInfo, &jpegIn, &jpegOut);

                /* info to handleSlicedOutput */
                frameReady = 1;
                slicedOutputUsed = 0;

                /* Handle here the error situation */
                PrintJpegRet(&jpegRet);
                if(mode == 1)
                    break;
                else
                    goto error;
            }
            else
            {
                /* Handle here the error situation */
                PrintJpegRet(&jpegRet);
                goto end;
            }
        }
        while(jpegRet != JPEGDEC_FRAME_READY);

        /* new config */
        if(frameCounter > 1 && loop == 0)
        {
            /* calculate/write output of slice */
            if(slicedOutputUsed &&
               jpegOut.outputPictureY.pVirtualAddress != NULL)
            {
                handleSlicedOutput(&imageInfo, &jpegIn, &jpegOut);
                slicedOutputUsed = 0;
            }

            /* thumbnail mode */
            mode = 1;

            /* calculate size for output */
            calcSize(&imageInfo, mode);

            fprintf(stdout, "\t-JPEG: ++++++++++ THUMBNAIL ++++++++++\n");
            fprintf(stdout, "\t-JPEG: Instance %x\n",
                    (JpegDecContainer *) jpeg);
            fprintf(stdout, "\t-JPEG: Luma output: 0x%08x size: %d\n",
                    jpegOut.outputPictureY.pVirtualAddress, sizeLuma);
            fprintf(stdout, "\t-JPEG: Chroma output: 0x%08x size: %d\n",
                    jpegOut.outputPictureCbCr.pVirtualAddress, sizeChroma);
            fprintf(stdout, "\t-JPEG: Luma output bus: 0x%08x\n",
                    (u8 *) jpegOut.outputPictureY.busAddress);
            fprintf(stdout, "\t-JPEG: Chroma output bus: 0x%08x\n",
                    (u8 *) jpegOut.outputPictureCbCr.busAddress);

            fprintf(stdout, "PHASE 4: DECODE FRAME: THUMBNAIL successful\n");

            /* if output write not disabled by TB */
            if(writeOutput)
            {
                /******** PHASE 5 ********/
                fprintf(stdout, "\nPHASE 5: WRITE OUTPUT\n");

                if(imageInfo.outputFormat)
                {
                    switch (imageInfo.outputFormat)
                    {
                    case JPEGDEC_YCbCr400:
                        fprintf(stdout,
                                "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr400\n");
                        break;
                    case JPEGDEC_YCbCr420_SEMIPLANAR:
                        fprintf(stdout,
                                "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr420_SEMIPLANAR\n");
                        break;
                    case JPEGDEC_YCbCr422_SEMIPLANAR:
                        fprintf(stdout,
                                "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr422_SEMIPLANAR\n");
                        break;
                    case JPEGDEC_YCbCr440:
                        fprintf(stdout,
                                "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr440\n");
                        break;
                    case JPEGDEC_YCbCr411_SEMIPLANAR:
                        fprintf(stdout,
                                "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr411_SEMIPLANAR\n");
                        break;
                    case JPEGDEC_YCbCr444_SEMIPLANAR:
                        fprintf(stdout,
                                "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr444_SEMIPLANAR\n");
                        break;
                    }
                }
#ifndef PP_PIPELINE_ENABLED
                /* write output */
                if(jpegIn.sliceMbSet)
                {
                    if(imageInfo.outputFormat != JPEGDEC_YCbCr400)
                        WriteFullOutput(mode);
                }
                else
                {
                    WriteOutput(((u8 *) jpegOut.outputPictureY.pVirtualAddress),
                                sizeLuma,
                                ((u8 *) jpegOut.outputPictureCbCr.
                                 pVirtualAddress), sizeChroma, mode);
                }
#else
                /* PP test bench will do the operations only if enabled */
                /*pp_set_rotation(); */

                printf("PP_OUTPUT_WRITE\n");
                pp_write_output(0, 0, 0);
                pp_check_combined_status();
#endif

                fprintf(stdout, "PHASE 5: WRITE OUTPUT successful\n");
            }
            else
            {
				video_display_do(OutStreamMem.busAddress, imageInfo.outputWidth, imageInfo.outputHeight, 1);
                fprintf(stdout, "\nPHASE 5: WRITE OUTPUT DISABLED\n");
            }

            /******** PHASE 6 ********/
            fprintf(stdout, "\nPHASE 6: RELEASE JPEG DECODER\n");

            if(streamMem.virtualAddress != NULL)
                DWLFreeLinear(((JpegDecContainer *) jpeg)->dwl, &streamMem);

            //if(OutStreamMem.virtualAddress != NULL)
            //    DWLFreeLinear(((JpegDecContainer *) jpeg)->dwl, &OutStreamMem);

            if(userAllocLuma.virtualAddress != NULL)
                DWLFreeRefFrm(((JpegDecContainer *) jpeg)->dwl, &userAllocLuma);

            if(userAllocChroma.virtualAddress != NULL)
                DWLFreeRefFrm(((JpegDecContainer *) jpeg)->dwl,
                              &userAllocChroma);

            if(userAllocCr.virtualAddress != NULL)
                DWLFreeRefFrm(((JpegDecContainer *) jpeg)->dwl, &userAllocCr);

            /* release decoder instance */
            JpegDecRelease(jpeg);

            fprintf(stdout, "PHASE 6: RELEASE JPEG DECODER successful\n\n");

            if(inputReadType)
            {
                if(fIn)
                {
                    fclose(fIn);
                }
            }

            if(fout)
            {
                fclose(fout);
            }

            fprintf(stdout, "TB: ...released\n");

            /* thumbnail done ==> set mode to full */
            mode = 0;

            /* Print API and build version numbers */
            decVer = JpegGetAPIVersion();
            decBuild = JpegDecGetBuild();

            /* Version */
            fprintf(stdout,
                    "\nX170 JPEG Decoder API v%d.%d - SW build: %d - HW build: %x\n",
                    decVer.major, decVer.minor, decBuild.swBuild,
                    decBuild.hwBuild);

#ifdef ASIC_TRACE_SUPPORT
            /* update picture counter for tracing */
            picNumber++;
#endif

            /******** PHASE 1 ********/
            fprintf(stdout, "\nPHASE 1: INIT JPEG DECODER\n");

            /* Jpeg initialization */
            jpegRet = JpegDecInit(&jpeg, &streamMem, 0);
            if(jpegRet != JPEGDEC_OK)
            {
                /* Handle here the error situation */
                PrintJpegRet(&jpegRet);
                goto end;
            }

            /* NOTE: The registers should not be used outside decoder SW for other
             * than compile time setting test purposes */
            printf("---Clock Gating %d---\n", clock_gating);
            printf("---Latency Compensation %d---\n", latency_comp);
            printf("---Output Picture Endian %d---\n", output_picture_endian);
            printf("---Bus Burst Length %d---\n", bus_burst_length);
            printf("---Asic Service Priority %d---\n", asic_service_priority);
            printf("---Data Discard  %d---\n", data_discard);
            fflush(stdout);

            SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs,
                           HWIF_DEC_LATENCY, latency_comp);
            SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs,
                           HWIF_DEC_CLK_GATE_E, clock_gating);
            SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs,
                           HWIF_DEC_OUT_ENDIAN, output_picture_endian);
            SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs,
                           HWIF_DEC_MAX_BURST, bus_burst_length);
            if ((G1DWLReadAsicID() >> 16) == 0x8170U)
            {   
                SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs,
                               HWIF_PRIORITY_MODE, asic_service_priority);
            }
            SetDecRegister(((JpegDecContainer *) jpeg)->jpegRegs,
                           HWIF_DEC_DATA_DISC_E, data_discard);

            fprintf(stdout, "PHASE 1: INIT JPEG DECODER successful\n");

            /******** PHASE 2 ********/
            fprintf(stdout, "\nPHASE 2: SET FILE IO\n");

            /******** PHASE 2 ********/
            fprintf(stdout, "\nPHASE 2: OPEN/READ FILE \n");

            /* Reading input file */
            fIn = fopen(filename, "rb");
            if(fIn == NULL)
            {
                fprintf(stdout, "Unable to open input file\n");
               	return -1; 
            }

            /* file i/o pointer to full */
            fseek(fIn, 0L, SEEK_END);
            len = ftell(fIn);
            rewind(fIn);

            streamTotalLen = 0;
            streamSeekLen = 0;
            streamInFile = 0;

            /* Handle input buffer load */
            if(jpegIn.bufferSize)
            {
                if(len > jpegIn.bufferSize)
                {
                    streamTotalLen = len;
                    len = jpegIn.bufferSize;
                }
                else
                {
                    streamTotalLen = len;
                    len = streamTotalLen;
                    jpegIn.bufferSize = 0;
                }
            }
            else
            {
                jpegIn.bufferSize = 0;
                streamTotalLen = len;
            }

            streamInFile = streamTotalLen;

            /* NOTE: The DWL should not be used outside decoder SW
             * here we call it because it is the easiest way to get
             * dynamically allocated linear memory
             * */

            /* allocate memory for stream buffer. if unsuccessful -> exit */
            streamMem.virtualAddress = NULL;
            streamMem.busAddress = 0;

            /* allocate memory for stream buffer. if unsuccessful -> exit */
            if(G1DWLMallocLinear
               (((JpegDecContainer *) jpeg)->dwl, len, &streamMem) != DWL_OK)
            {
                fprintf(stdout, "UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n");
                goto end;
            }

            byteStrmStart = (u8 *) streamMem.virtualAddress;

            if(byteStrmStart == NULL)
            {
                fprintf(stdout, "UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n");
                goto end;
            }

            /* read input stream from file to buffer and close input file */
            fread(byteStrmStart, sizeof(u8), len, fIn);
            fclose(fIn);

            /* initialize JpegDecDecode input structure */
            jpegIn.streamBuffer.pVirtualAddress = (u32 *) byteStrmStart;
            jpegIn.streamBuffer.busAddress = (u32) streamMem.busAddress;
            jpegIn.streamLength = streamTotalLen;

            if(writeOutput)
                fprintf(stdout, "\t-File IO: Write output: YES: %d\n",
                        writeOutput);
            else
                fprintf(stdout, "\t-File IO: Write output: NO: %d\n",
                        writeOutput);
            fprintf(stdout, "\t-File IO: MbRows/slice: %d\n",
                    jpegIn.sliceMbSet);

            fprintf(stdout, "PHASE 2: SET FILE IO successful\n");

            /******** PHASE 3 ********/
            fprintf(stdout, "\nPHASE 3: GET IMAGE INFO\n");

            /* Get image information of the JFIF and decode JFIF header */
            jpegRet = JpegDecGetImageInfo(jpeg, &jpegIn, &imageInfo);

            if(jpegRet != JPEGDEC_OK)
            {
                PrintJpegRet(&jpegRet);
                /* Handle here the error situation */
                goto end;
            }
            /* Decode Full Image */
            jpegIn.decImageType = JPEGDEC_IMAGE;

            /* no slice mode supported in progressive || non-interleaved ==> force to full mode */
            if(imageInfo.codingMode == JPEGDEC_PROGRESSIVE ||
               imageInfo.codingMode == JPEGDEC_NONINTERLEAVED)
                jpegIn.sliceMbSet = 0;

            fprintf(stdout, "PHASE 3: GET IMAGE INFO successful\n");
        }
    }

  error:

    /* calculate/write output of slice */
    if(slicedOutputUsed && jpegOut.outputPictureY.pVirtualAddress != NULL)
    {
        handleSlicedOutput(&imageInfo, &jpegIn, &jpegOut);
        slicedOutputUsed = 0;
    }

    /* full resolution mode */
    mode = 0;

    if(jpegOut.outputPictureY.pVirtualAddress != NULL)
    {
        /* calculate size for output */
        calcSize(&imageInfo, mode);

        fprintf(stdout, "\n\t-JPEG: ++++++++++ FULL RESOLUTION ++++++++++\n");
        fprintf(stdout, "\t-JPEG: Instance %x\n", (JpegDecContainer *) jpeg);
        fprintf(stdout, "\t-JPEG: Luma output: 0x%08x size: %d\n",
                jpegOut.outputPictureY.pVirtualAddress, sizeLuma);
        fprintf(stdout, "\t-JPEG: Chroma output: 0x%08x size: %d\n",
                jpegOut.outputPictureCbCr.pVirtualAddress, sizeChroma);
        fprintf(stdout, "\t-JPEG: Luma output bus: 0x%08x\n",
                (u8 *) jpegOut.outputPictureY.busAddress);
        fprintf(stdout, "\t-JPEG: Chroma output bus: 0x%08x\n",
                (u8 *) jpegOut.outputPictureCbCr.busAddress);
    }

    fprintf(stdout, "PHASE 4: DECODE FRAME successful\n");

	if(G1DWLMallocLinear
	   (((JpegDecContainer *) jpeg)->dwl, sizeLuma+sizeChroma, &OutStreamMem) != DWL_OK)
	{
	    fprintf(stdout, "UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n");
	    goto end;
	}

	(void) G1DWLmemset(OutStreamMem.virtualAddress, 0, sizeLuma+sizeChroma);

	memcpy(OutStreamMem.virtualAddress, jpegOut.outputPictureY.pVirtualAddress, sizeLuma);
	memcpy((u8 *)OutStreamMem.virtualAddress+sizeLuma, jpegOut.outputPictureCbCr.pVirtualAddress, sizeChroma);

    /* if output write not disabled by TB */
    if(writeOutput)
    {
        /******** PHASE 5 ********/
        fprintf(stdout, "\nPHASE 5: WRITE OUTPUT\n");

#ifndef PP_PIPELINE_ENABLED
        if(imageInfo.outputFormat)
        {
            switch (imageInfo.outputFormat)
            {
            case JPEGDEC_YCbCr400:
                fprintf(stdout, "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr400\n");
                break;
            case JPEGDEC_YCbCr420_SEMIPLANAR:
                fprintf(stdout, "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr420_SEMIPLANAR\n");
                break;
            case JPEGDEC_YCbCr422_SEMIPLANAR:
                fprintf(stdout, "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr422_SEMIPLANAR\n");
                break;
            case JPEGDEC_YCbCr440:
                fprintf(stdout, "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr440\n");
                break;
            case JPEGDEC_YCbCr411_SEMIPLANAR:
                fprintf(stdout, "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr411_SEMIPLANAR\n");
                break;
            case JPEGDEC_YCbCr444_SEMIPLANAR:
                fprintf(stdout, "\t-JPEG: DECODER OUTPUT: JPEGDEC_YCbCr444_SEMIPLANAR\n");
                break;
            }
        }

        if(imageInfo.codingMode == JPEGDEC_PROGRESSIVE)
            progressive = 1;

        /* write output */
        if(jpegIn.sliceMbSet)
        {
            if(imageInfo.outputFormat != JPEGDEC_YCbCr400)
                WriteFullOutput(mode);
        }
        else
        {
            if(imageInfo.codingMode != JPEGDEC_PROGRESSIVE)
            {
                WriteOutput(((u8 *) jpegOut.outputPictureY.pVirtualAddress),
                            sizeLuma,
                            ((u8 *) jpegOut.outputPictureCbCr.
                             pVirtualAddress), sizeChroma, mode);
            }
            else
            {
                /* calculate size for output */
                calcSize(&imageInfo, mode);

                printf("sizeLuma %d and sizeChroma %d\n", sizeLuma, sizeChroma);

                WriteProgressiveOutput(sizeLuma, sizeChroma, mode,
                                       (u8*)jpegOut.outputPictureY.pVirtualAddress,
                                       (u8*)jpegOut.outputPictureCbCr.
                                       pVirtualAddress,
                                       (u8*)jpegOut.outputPictureCr.pVirtualAddress);
            }

        }

        if(crop)
            WriteCroppedOutput(&imageInfo,
                               (u8*)jpegOut.outputPictureY.pVirtualAddress,
                               (u8*)jpegOut.outputPictureCbCr.pVirtualAddress,
                               (u8*)jpegOut.outputPictureCr.pVirtualAddress);

        progressive = 0;
#else
        /* PP test bench will do the operations only if enabled */
        /*pp_set_rotation(); */

        fprintf(stdout, "\t-JPEG: PP_OUTPUT_WRITE\n");
        pp_write_output(0, 0, 0);
        pp_check_combined_status();
#endif
        fprintf(stdout, "PHASE 5: WRITE OUTPUT successful\n");
    }
    else
    {
		video_display_do(OutStreamMem.busAddress, imageInfo.outputWidth, imageInfo.outputHeight, 1);
        fprintf(stdout, "\nPHASE 5: WRITE OUTPUT DISABLED\n");
    }

  end:
    /******** PHASE 6 ********/
    fprintf(stdout, "\nPHASE 6: RELEASE JPEG DECODER\n");

    /* reset output write option */
    progressive = 0;

#ifdef PP_PIPELINE_ENABLED
    pp_close();
#endif

    if(streamMem.virtualAddress != NULL)
        DWLFreeLinear(((JpegDecContainer *) jpeg)->dwl, &streamMem);

	//if(OutStreamMem.virtualAddress != NULL)
	//DWLFreeLinear(((JpegDecContainer *) jpeg)->dwl, &OutStreamMem);

    if(userAllocLuma.virtualAddress != NULL)
        DWLFreeRefFrm(((JpegDecContainer *) jpeg)->dwl, &userAllocLuma);

    if(userAllocChroma.virtualAddress != NULL)
        DWLFreeRefFrm(((JpegDecContainer *) jpeg)->dwl, &userAllocChroma);

    if(userAllocCr.virtualAddress != NULL)
        DWLFreeRefFrm(((JpegDecContainer *) jpeg)->dwl, &userAllocCr);

    /* release decoder instance */
    JpegDecRelease(jpeg);

    fprintf(stdout, "PHASE 6: RELEASE JPEG DECODER successful\n\n");

    if(inputReadType)
    {
        if(fIn)
        {
            fclose(fIn);
        }
    }

    if(fout)
    {
        fclose(fout);
        if(fStreamTrace)
            fclose(fStreamTrace);
    }

#ifdef ASIC_TRACE_SUPPORT
    trace_SequenceCtrl(picNumber + 1, bFrames);
    trace_JpegDecodingTools();
    closeTraceFiles();
#endif

    /* Leave properly */
    JpegDecFree(pImage);


    fprintf(stdout, "TB: ...released\n");

    return 0;
}

/*------------------------------------------------------------------------------

    Function name:  WriteOutput

    Purpose:
        Write picture pointed by data to file. Size of the
        picture in pixels is indicated by picSize.

------------------------------------------------------------------------------*/
void
WriteOutput(u8 * dataLuma, u32 picSizeLuma, u8 * dataChroma,
            u32 picSizeChroma, u32 picMode)
{
    u32 i;
    FILE *foutput = NULL;
    u8 *pYuvOut = NULL;
    u8 file[256];

    if(!sliceToUser)
    {
        /* foutput is global file pointer */
        if(foutput == NULL)
        {
            if(picMode == 0)
                foutput = fopen("out.yuv", "wb");
            else
                foutput = fopen("out_tn.yuv", "wb");
            if(foutput == NULL)
            {
                fprintf(stdout, "UNABLE TO OPEN OUTPUT FILE\n");
                return;
            }
        }
    }
    else
    {
        /* foutput is global file pointer */
        if(foutput == NULL)
        {
            if(picMode == 0)
            {
                sprintf(file, "out_%d.yuv", fullSliceCounter);
                foutput = fopen(file, "wb");
            }
            else
            {
                sprintf(file, "tn_%d.yuv", fullSliceCounter);
                foutput = fopen(file, "wb");
            }

            if(foutput == NULL)
            {
                fprintf(stdout, "UNABLE TO OPEN OUTPUT FILE\n");
                return;
            }
        }
    }

    if(foutput && dataLuma)
    {
        if(1)
        {
            fprintf(stdout, "\t-JPEG: Luminance\n");
            /* write decoder output to file */
            pYuvOut = dataLuma;
            for(i = 0; i < (picSizeLuma >> 2); i++)
            {
#ifndef ASIC_TRACE_SUPPORT
                if(DEC_X170_BIG_ENDIAN == output_picture_endian)
                {
                    fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1, foutput);
                }
                else
                {
#endif
                    fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1, foutput);
#ifndef ASIC_TRACE_SUPPORT
                }
#endif
            }
        }
    }

    if(!nonInterleaved)
    {
        /* progressive ==> planar */
        if(!progressive)
        {
            if(foutput && dataChroma)
            {
                fprintf(stdout, "\t-JPEG: Chrominance\n");
                /* write decoder output to file */
                pYuvOut = dataChroma;
                if(!planarOutput)
                {
                    for(i = 0; i < (picSizeChroma >> 2); i++)
                    {
#ifndef ASIC_TRACE_SUPPORT
                        if(DEC_X170_BIG_ENDIAN == output_picture_endian)
                        {
                            fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1,
                                   foutput);
                        }
                        else
                        {
#endif
                            fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1,
                                   foutput);
#ifndef ASIC_TRACE_SUPPORT
                        }
#endif
                    }
                }
                else
                {
                    printf("BASELINE PLANAR\n");
                    for(i = 0; i < picSizeChroma / 2; i++)
                        fwrite(pYuvOut + 2 * i, sizeof(u8), 1, foutput);
                    for(i = 0; i < picSizeChroma / 2; i++)
                        fwrite(pYuvOut + 2 * i + 1, sizeof(u8), 1, foutput);
                }
            }
        }
        else
        {
            if(foutput && dataChroma)
            {
                fprintf(stdout, "\t-JPEG: Chrominance\n");
                /* write decoder output to file */
                pYuvOut = dataChroma;
                if(!planarOutput)
                {
                    for(i = 0; i < (picSizeChroma >> 2); i++)
                    {
#ifndef ASIC_TRACE_SUPPORT
                        if(DEC_X170_BIG_ENDIAN == output_picture_endian)
                        {
                            fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1,
                                   foutput);
                        }
                        else
                        {
#endif
                            fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1,
                                   foutput);
                            fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1,
                                   foutput);
#ifndef ASIC_TRACE_SUPPORT
                        }
#endif
                    }
                }
                else
                {
                    printf("PROGRESSIVE PLANAR OUTPUT\n");
                    for(i = 0; i < picSizeChroma; i++)
                        fwrite(pYuvOut + (1 * i), sizeof(u8), 1, foutput);
                }
            }
        }
    }
    else
    {
        if(foutput && dataChroma)
        {
            fprintf(stdout, "\t-JPEG: Chrominance\n");
            /* write decoder output to file */
            pYuvOut = dataChroma;

            printf("NONINTERLEAVED: PLANAR OUTPUT\n");
            for(i = 0; i < picSizeChroma; i++)
                fwrite(pYuvOut + (1 * i), sizeof(u8), 1, foutput);
        }
    }
    fclose(foutput);
}

/*------------------------------------------------------------------------------

    Function name:  WriteOutputLuma

    Purpose:
        Write picture pointed by data to file. Size of the
        picture in pixels is indicated by picSize.

------------------------------------------------------------------------------*/
void WriteOutputLuma(u8 * dataLuma, u32 picSizeLuma, u32 picMode)
{
    u32 i;
    FILE *foutput = NULL;
    u8 *pYuvOut = NULL;

    /* foutput is global file pointer */
    if(foutput == NULL)
    {
        if(picMode == 0)
        {
            foutput = fopen("out.yuv", "ab");
        }
        else
        {
            foutput = fopen("out_tn.yuv", "ab");
        }

        if(foutput == NULL)
        {
            fprintf(stdout, "UNABLE TO OPEN OUTPUT FILE\n");
            return;
        }
    }

    if(foutput && dataLuma)
    {
        if(1)
        {
            fprintf(stdout, "\t-JPEG: Luminance\n");
            /* write decoder output to file */
            pYuvOut = dataLuma;
            for(i = 0; i < (picSizeLuma >> 2); i++)
            {
#ifndef ASIC_TRACE_SUPPORT
                if(DEC_X170_BIG_ENDIAN == output_picture_endian)
                {
                    fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1, foutput);
                }
                else
                {
#endif
                    fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1, foutput);
                    fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1, foutput);
#ifndef ASIC_TRACE_SUPPORT
                }
#endif
            }
        }
    }

    fclose(foutput);
}

/*------------------------------------------------------------------------------

    Function name:  WriteOutput

    Purpose:
        Write picture pointed by data to file. Size of the
        picture in pixels is indicated by picSize.

------------------------------------------------------------------------------*/
void WriteOutputChroma(u8 * dataChroma, u32 picSizeChroma, u32 picMode)
{
    u32 i;
    FILE *foutputChroma = NULL;
    u8 *pYuvOut = NULL;

    /* file pointer */
    if(foutputChroma == NULL)
    {
        if(picMode == 0)
        {
            if(!progressive)
            {
                if(fullSliceCounter == 0)
                    foutputChroma = fopen("outChroma.yuv", "wb");
                else
                    foutputChroma = fopen("outChroma.yuv", "ab");
            }
            else
            {
                if(!slicedOutputUsed)
                {
                    foutputChroma = fopen("outChroma.yuv", "wb");
                }
                else
                {
                    if(scanCounter == 0 || fullSliceCounter == 0)
                        foutputChroma = fopen("outChroma.yuv", "wb");
                    else
                        foutputChroma = fopen("outChroma.yuv", "ab");
                }
            }
        }
        else
        {
            if(!progressive)
            {
                if(fullSliceCounter == 0)
                    foutputChroma = fopen("outChroma_tn.yuv", "wb");
                else
                    foutputChroma = fopen("outChroma_tn.yuv", "ab");
            }
            else
            {
                if(!slicedOutputUsed)
                {
                    foutputChroma = fopen("outChroma_tn.yuv", "wb");
                }
                else
                {
                    if(scanCounter == 0 || fullSliceCounter == 0)
                        foutputChroma = fopen("outChroma_tn.yuv", "wb");
                    else
                        foutputChroma = fopen("outChroma_tn.yuv", "ab");
                }
            }
        }

        if(foutputChroma == NULL)
        {
            fprintf(stdout, "UNABLE TO OPEN OUTPUT FILE\n");
            return;
        }
    }

    if(foutputChroma && dataChroma)
    {
        fprintf(stdout, "\t-JPEG: Chrominance\n");
        /* write decoder output to file */
        pYuvOut = dataChroma;

        if(!progressive)
        {
            for(i = 0; i < (picSizeChroma >> 2); i++)
            {
#ifndef ASIC_TRACE_SUPPORT
                if(DEC_X170_BIG_ENDIAN == output_picture_endian)
                {
                    fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1, foutputChroma);
                    fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1, foutputChroma);
                    fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1, foutputChroma);
                    fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1, foutputChroma);
                }
                else
                {
#endif
                    fwrite(pYuvOut + (4 * i) + 0, sizeof(u8), 1, foutputChroma);
                    fwrite(pYuvOut + (4 * i) + 1, sizeof(u8), 1, foutputChroma);
                    fwrite(pYuvOut + (4 * i) + 2, sizeof(u8), 1, foutputChroma);
                    fwrite(pYuvOut + (4 * i) + 3, sizeof(u8), 1, foutputChroma);
#ifndef ASIC_TRACE_SUPPORT
                }
#endif
            }
        }
        else
        {
            printf("PROGRESSIVE PLANAR OUTPUT CHROMA\n");
            for(i = 0; i < picSizeChroma; i++)
                fwrite(pYuvOut + (1 * i), sizeof(u8), 1, foutputChroma);
        }
    }
    fclose(foutputChroma);
}

/*------------------------------------------------------------------------------

    Function name:  WriteFullOutput

    Purpose:
        Write picture pointed by data to file. 

------------------------------------------------------------------------------*/
void WriteFullOutput(u32 picMode)
{
    u32 i;
    FILE *foutput = NULL;
    u8 *pYuvOutChroma = NULL;
    FILE *fInputChroma = NULL;
    u32 length = 0;
    u32 chromaLen = 0;

	fprintf(stdout, "\t-JPEG: WriteFullOutput\n");

	/* if semi-planar output */
	if(!planarOutput)
	{
		/* Reading chroma file */
		if(picMode == 0)
			system("cat outChroma.yuv >> out.yuv");
		else
			system("cat outChroma_tn.yuv >> out_tn.yuv");
	}
	else
	{
		/* Reading chroma file */
		if(picMode == 0)
			fInputChroma = fopen("outChroma.yuv", "rb");
		else
			fInputChroma = fopen("outChroma_tn.yuv", "rb");

		if(fInputChroma == NULL)
		{
			fprintf(stdout, "Unable to open chroma output tmp file\n");
			exit(-1);
		}

		/* file i/o pointer to full */
		fseek(fInputChroma, 0L, SEEK_END);
		length = ftell(fInputChroma);
		rewind(fInputChroma);

		/* check length */
		chromaLen = length;

		/*pYuvOutChroma = JpegDecMalloc(sizeof(u8) * (chromaLen));*/
		/*because malloc(size) will fail when size is too big for some OS,   
                we just allocate a small memory and read 4 bytes at most from fInputChroma file and then write data into foutput file repeately. */
		pYuvOutChroma = JpegDecMalloc(sizeof(u8) * (4));

		/* read output stream from file to buffer and close input file */
		/*fread(pYuvOutChroma, sizeof(u8), chromaLen, fInputChroma);*/

		/*fclose(fInputChroma);*/

		/* foutput is global file pointer */
		if(picMode == 0)
			foutput = fopen("out.yuv", "ab");
		else
			foutput = fopen("out_tn.yuv", "ab");

		if(foutput == NULL)
		{
			fprintf(stdout, "UNABLE TO OPEN OUTPUT FILE\n");
			return;
		}

		if(foutput && pYuvOutChroma)
		{
			fprintf(stdout, "\t-JPEG: Chrominance\n");
			if(!progressive)
			{
				if(!planarOutput)
				{
					/* write decoder output to file */
					for(i = 0; i < (chromaLen >> 2); i++)
					{
                                                fread(pYuvOutChroma, sizeof(u8), 4, fInputChroma);
						fwrite(pYuvOutChroma , sizeof(u8), 1, foutput);
						fwrite(pYuvOutChroma + 1, sizeof(u8), 1, foutput);
						fwrite(pYuvOutChroma + 2, sizeof(u8), 1, foutput);
						fwrite(pYuvOutChroma + 3, sizeof(u8), 1, foutput);
					}
				}
				else
				{
					for(i = 0; i < chromaLen / 2; i++)
					{
                                                fread(pYuvOutChroma, sizeof(u8), 2, fInputChroma);
						fwrite(pYuvOutChroma, sizeof(u8), 1, foutput);
					}
                                        fseek(fInputChroma, 0L, SEEK_SET);
					for(i = 0; i < chromaLen / 2; i++)
					{
                                                fread(pYuvOutChroma, sizeof(u8), 2, fInputChroma);
						fwrite(pYuvOutChroma + 1, sizeof(u8), 1, foutput);
					}
				}
			}
			else
			{
				if(!planarOutput)
				{
					/* write decoder output to file */
					for(i = 0; i < (chromaLen >> 2); i++)
					{
                                                fread(pYuvOutChroma, sizeof(u8), 4, fInputChroma);
						fwrite(pYuvOutChroma + 0, sizeof(u8), 1, foutput);
						fwrite(pYuvOutChroma + 1, sizeof(u8), 1, foutput);
						fwrite(pYuvOutChroma + 2, sizeof(u8), 1, foutput);
						fwrite(pYuvOutChroma + 3, sizeof(u8), 1, foutput);
					}
				}
				else
				{
					printf("PROGRESSIVE FULL CHROMA %d\n", chromaLen);
					for(i = 0; i < chromaLen; i++)
					{
                                                fread(pYuvOutChroma, sizeof(u8), 1, fInputChroma);
						fwrite(pYuvOutChroma , sizeof(u8), 1, foutput);
					}
				}
			}
		}
		fclose(foutput);
                fclose(fInputChroma);

		/* Leave properly */
		JpegDecFree(pYuvOutChroma);
	}
}

/*------------------------------------------------------------------------------

    Function name:  handleSlicedOutput

    Purpose:
        Calculates size for slice and writes sliced output

------------------------------------------------------------------------------*/
void
handleSlicedOutput(JpegDecImageInfo * imageInfo,
                   JpegDecInput * jpegIn, JpegDecOutput * jpegOut)
{
    /* for output name */
    fullSliceCounter++;

    /******** PHASE X ********/
    if(jpegIn->sliceMbSet)
        fprintf(stdout, "\nPHASE SLICE: HANDLE SLICE %d\n", fullSliceCounter);

    /* save start pointers for whole output */
    if(fullSliceCounter == 0)
    {
        /* virtual address */
        outputAddressY.pVirtualAddress =
            jpegOut->outputPictureY.pVirtualAddress;
        outputAddressCbCr.pVirtualAddress =
            jpegOut->outputPictureCbCr.pVirtualAddress;

        /* bus address */
        outputAddressY.busAddress = jpegOut->outputPictureY.busAddress;
        outputAddressCbCr.busAddress = jpegOut->outputPictureCbCr.busAddress;
    }

    /* if output write not disabled by TB */
    if(writeOutput)
    {
        /******** PHASE 5 ********/
        fprintf(stdout, "\nPHASE 5: WRITE OUTPUT\n");

        if(imageInfo->outputFormat)
        {
            if(!frameReady)
            {
                sliceSize = jpegIn->sliceMbSet * 16;
            }
            else
            {
                if(mode == 0)
                    sliceSize =
                        (imageInfo->outputHeight -
                         ((fullSliceCounter) * (sliceSize)));
                else
                    sliceSize =
                        (imageInfo->outputHeightThumb -
                         ((fullSliceCounter) * (sliceSize)));
            }
        }

        /* slice interrupt from decoder */
        sliceToUser = 1;

        /* calculate size for output */
        calcSize(imageInfo, mode);

        /* test printf */
        fprintf(stdout, "\t-JPEG: ++++++++++ SLICE INFORMATION ++++++++++\n");
        fprintf(stdout, "\t-JPEG: Luma output: 0x%08x size: %d\n",
                jpegOut->outputPictureY.pVirtualAddress, sizeLuma);
        fprintf(stdout, "\t-JPEG: Chroma output: 0x%08x size: %d\n",
                jpegOut->outputPictureCbCr.pVirtualAddress, sizeChroma);
        fprintf(stdout, "\t-JPEG: Luma output bus: 0x%08x\n",
                (u8 *) jpegOut->outputPictureY.busAddress);
        fprintf(stdout, "\t-JPEG: Chroma output bus: 0x%08x\n",
                (u8 *) jpegOut->outputPictureCbCr.busAddress);

        /* write slice output */
        WriteOutput(((u8 *) jpegOut->outputPictureY.pVirtualAddress),
                    sizeLuma,
                    ((u8 *) jpegOut->outputPictureCbCr.pVirtualAddress),
                    sizeChroma, mode);

        /* write luma to final output file */
        WriteOutputLuma(((u8 *) jpegOut->outputPictureY.pVirtualAddress),
                        sizeLuma, mode);

        if(imageInfo->outputFormat != JPEGDEC_YCbCr400)
        {
            /* write chroam to tmp file */
            WriteOutputChroma(((u8 *) jpegOut->outputPictureCbCr.
                               pVirtualAddress), sizeChroma, mode);
        }

        fprintf(stdout, "PHASE 5: WRITE OUTPUT successful\n");
    }
    else
    {
        fprintf(stdout, "\nPHASE 5: WRITE OUTPUT DISABLED\n");
    }

    if(frameReady)
    {
        /* give start pointers for whole output write */

        /* virtual address */
        jpegOut->outputPictureY.pVirtualAddress =
            outputAddressY.pVirtualAddress;
        jpegOut->outputPictureCbCr.pVirtualAddress =
            outputAddressCbCr.pVirtualAddress;

        /* bus address */
        jpegOut->outputPictureY.busAddress = outputAddressY.busAddress;
        jpegOut->outputPictureCbCr.busAddress = outputAddressCbCr.busAddress;
    }

    if(frameReady)
    {
        frameReady = 0;
        sliceToUser = 0;

        /******** PHASE X ********/
        if(jpegIn->sliceMbSet)
            fprintf(stdout, "\nPHASE SLICE: HANDLE SLICE %d successful\n",
                    fullSliceCounter);

        fullSliceCounter = -1;
    }
    else
    {
        /******** PHASE X ********/
        if(jpegIn->sliceMbSet)
            fprintf(stdout, "\nPHASE SLICE: HANDLE SLICE %d successful\n",
                    fullSliceCounter);
    }

}

/*------------------------------------------------------------------------------

    Function name:  calcSize

    Purpose:
        Calculate size

------------------------------------------------------------------------------*/
void calcSize(JpegDecImageInfo * imageInfo, u32 picMode)
{

    u32 format;

    sizeLuma = 0;
    sizeChroma = 0;

    format = picMode == 0 ?
        imageInfo->outputFormat : imageInfo->outputFormatThumb;

    /* if slice interrupt not given to user */
    if(!sliceToUser || scanReady)
    {
        if(picMode == 0)    /* full */
        {
            sizeLuma = (imageInfo->outputWidth * imageInfo->outputHeight);
        }
        else    /* thumbnail */
        {
            sizeLuma =
                (imageInfo->outputWidthThumb * imageInfo->outputHeightThumb);
        }
    }
    else
    {
        if(picMode == 0)    /* full */
        {
            sizeLuma = (imageInfo->outputWidth * sliceSize);
        }
        else    /* thumbnail */
        {
            sizeLuma = (imageInfo->outputWidthThumb * sliceSize);
        }
    }

    if(format != JPEGDEC_YCbCr400)
    {
        if(format == JPEGDEC_YCbCr420_SEMIPLANAR ||
           format == JPEGDEC_YCbCr411_SEMIPLANAR)
        {
            sizeChroma = (sizeLuma / 2);
        }
        else if(format == JPEGDEC_YCbCr444_SEMIPLANAR)
        {
            sizeChroma = sizeLuma * 2;
        }
        else
        {
            sizeChroma = sizeLuma;
        }
    }
}

/*------------------------------------------------------------------------------

    Function name:  allocMemory

    Purpose:
        Allocates user specific memory for output.

------------------------------------------------------------------------------*/
u32
allocMemory(JpegDecInst decInst, JpegDecImageInfo * imageInfo,
            JpegDecInput * jpegIn)
{
    u32 separateChroma = 0;
    u32 rotation = 0;

    out_pic_size_luma = 0;
    out_pic_size_chroma = 0;
    jpegIn->pictureBufferY.pVirtualAddress = NULL;
    jpegIn->pictureBufferY.busAddress = 0;
    jpegIn->pictureBufferCbCr.pVirtualAddress = NULL;
    jpegIn->pictureBufferCbCr.busAddress = 0;
    jpegIn->pictureBufferCr.pVirtualAddress = NULL;
    jpegIn->pictureBufferCr.busAddress = 0;

#ifdef PP_PIPELINE_ENABLED
    /* check if rotation used */
    rotation = pp_rotation_used();

    if(rotation)
        fprintf(stdout,
                "\t-JPEG: IN CASE ROTATION ==> USER NEEDS TO ALLOCATE FULL OUTPUT MEMORY\n");
#endif

    /* calculate sizes */
    if(jpegIn->decImageType == 0)
    {
        /* luma size */
        if(jpegIn->sliceMbSet && !rotation)
            out_pic_size_luma =
                (imageInfo->outputWidth * (jpegIn->sliceMbSet * 16));
        else
            out_pic_size_luma =
                (imageInfo->outputWidth * imageInfo->outputHeight);

        /* chroma size ==> semiplanar output */
        if(imageInfo->outputFormat == JPEGDEC_YCbCr420_SEMIPLANAR ||
           imageInfo->outputFormat == JPEGDEC_YCbCr411_SEMIPLANAR)
            out_pic_size_chroma = out_pic_size_luma / 2;
        else if(imageInfo->outputFormat == JPEGDEC_YCbCr422_SEMIPLANAR ||
                imageInfo->outputFormat == JPEGDEC_YCbCr440)
            out_pic_size_chroma = out_pic_size_luma;
        else if(imageInfo->outputFormat == JPEGDEC_YCbCr444_SEMIPLANAR)
            out_pic_size_chroma = out_pic_size_luma * 2;

        if(imageInfo->codingMode != JPEGDEC_BASELINE)
            separateChroma = 1;
    }
    else
    {
        /* luma size */
        if(jpegIn->sliceMbSet && !rotation)
            out_pic_size_luma =
                (imageInfo->outputWidthThumb * (jpegIn->sliceMbSet * 16));
        else
            out_pic_size_luma =
                (imageInfo->outputWidthThumb * imageInfo->outputHeightThumb);

        /* chroma size ==> semiplanar output */
        if(imageInfo->outputFormatThumb == JPEGDEC_YCbCr420_SEMIPLANAR ||
           imageInfo->outputFormatThumb == JPEGDEC_YCbCr411_SEMIPLANAR)
            out_pic_size_chroma = out_pic_size_luma / 2;
        else if(imageInfo->outputFormatThumb == JPEGDEC_YCbCr422_SEMIPLANAR ||
                imageInfo->outputFormatThumb == JPEGDEC_YCbCr440)
            out_pic_size_chroma = out_pic_size_luma;
        else if(imageInfo->outputFormatThumb == JPEGDEC_YCbCr444_SEMIPLANAR)
            out_pic_size_chroma = out_pic_size_luma * 2;

        if(imageInfo->codingModeThumb != JPEGDEC_BASELINE)
            separateChroma = 1;
    }

#ifdef LINUX
    {
        fprintf(stdout, "\t\t-JPEG: USER OUTPUT MEMORY ALLOCATION\n");

        jpegIn->pictureBufferY.pVirtualAddress = NULL;
        jpegIn->pictureBufferCbCr.pVirtualAddress = NULL;
        jpegIn->pictureBufferCr.pVirtualAddress = NULL;

        /**** memory area ****/

        /* allocate memory for stream buffer. if unsuccessful -> exit */
        userAllocLuma.virtualAddress = NULL;
        userAllocLuma.busAddress = 0;

        /* allocate memory for stream buffer. if unsuccessful -> exit */
        if(DWLMallocRefFrm
           (((JpegDecContainer *) decInst)->dwl, out_pic_size_luma,
            &userAllocLuma) != DWL_OK)
        {
            fprintf(stdout, "UNABLE TO ALLOCATE USER LUMA OUTPUT MEMORY\n");
            return JPEGDEC_MEMFAIL;
        }

        /* Luma Bus */
        jpegIn->pictureBufferY.pVirtualAddress = userAllocLuma.virtualAddress;
        jpegIn->pictureBufferY.busAddress = userAllocLuma.busAddress;

        /* memset output to gray */
        (void) G1DWLmemset(jpegIn->pictureBufferY.pVirtualAddress, 128,
                         out_pic_size_luma);

        /* allocate chroma */
        if(out_pic_size_chroma)
        {
            /* Baseline ==> semiplanar */
            if(separateChroma == 0)
            {
                /* allocate memory for stream buffer. if unsuccessful -> exit */
                if(DWLMallocRefFrm
                   (((JpegDecContainer *) decInst)->dwl, out_pic_size_chroma,
                    &userAllocChroma) != DWL_OK)
                {
                    fprintf(stdout,
                            "UNABLE TO ALLOCATE USER CHROMA OUTPUT MEMORY\n");
                    return JPEGDEC_MEMFAIL;
                }

                /* Chroma Bus */
                jpegIn->pictureBufferCbCr.pVirtualAddress =
                    userAllocChroma.virtualAddress;
                jpegIn->pictureBufferCbCr.busAddress =
                    userAllocChroma.busAddress;

                /* memset output to gray */
                (void) DWLmemset(jpegIn->pictureBufferCbCr.pVirtualAddress, 128,
                                 out_pic_size_chroma);
            }
            else    /* Progressive or non-interleaved ==> planar */
            {
                /* allocate memory for stream buffer. if unsuccessful -> exit */
                /* Cb */
                if(DWLMallocRefFrm
                   (((JpegDecContainer *) decInst)->dwl,
                    (out_pic_size_chroma / 2), &userAllocChroma) != DWL_OK)
                {
                    fprintf(stdout,
                            "UNABLE TO ALLOCATE USER CHROMA OUTPUT MEMORY\n");
                    return JPEGDEC_MEMFAIL;
                }

                /* Chroma Bus */
                jpegIn->pictureBufferCbCr.pVirtualAddress =
                    userAllocChroma.virtualAddress;
                jpegIn->pictureBufferCbCr.busAddress =
                    userAllocChroma.busAddress;

                /* Cr */
                if(DWLMallocRefFrm
                   (((JpegDecContainer *) decInst)->dwl,
                    (out_pic_size_chroma / 2), &userAllocCr) != DWL_OK)
                {
                    fprintf(stdout,
                            "UNABLE TO ALLOCATE USER CHROMA OUTPUT MEMORY\n");
                    return JPEGDEC_MEMFAIL;
                }

                /* Chroma Bus */
                jpegIn->pictureBufferCr.pVirtualAddress =
                    userAllocCr.virtualAddress;
                jpegIn->pictureBufferCr.busAddress = userAllocCr.busAddress;

                /* memset output to gray */
                /* Cb */
                (void) DWLmemset(jpegIn->pictureBufferCbCr.pVirtualAddress, 128,
                                 (out_pic_size_chroma / 2));

                /* Cr */
                (void) DWLmemset(jpegIn->pictureBufferCr.pVirtualAddress, 128,
                                 (out_pic_size_chroma / 2));
            }
        }
    }
#endif /* #ifdef LINUX */

#ifndef LINUX
    {
        fprintf(stdout, "\t\t-JPEG: MALLOC\n");

        /* allocate luma */
        jpegIn->pictureBufferY.pVirtualAddress =
            (u32 *) JpegDecMalloc(sizeof(u8) * out_pic_size_luma);

        JpegDecMemset(jpegIn->pictureBufferY.pVirtualAddress, 128,
                      out_pic_size_luma);

        /* allocate chroma */
        if(out_pic_size_chroma)
        {
            jpegIn->pictureBufferCbCr.pVirtualAddress =
                (u32 *) JpegDecMalloc(sizeof(u8) * out_pic_size_chroma);

            JpegDecMemset(jpegIn->pictureBufferCbCr.pVirtualAddress, 128,
                          out_pic_size_chroma);
        }
    }
#endif /* #ifndef LINUX */

    fprintf(stdout, "\t\t-JPEG: Allocate: Luma virtual %x bus %x size %d\n",
            (u32) jpegIn->pictureBufferY.pVirtualAddress,
            jpegIn->pictureBufferY.busAddress, out_pic_size_luma);

    if(separateChroma == 0)
    {
        fprintf(stdout,
                "\t\t-JPEG: Allocate: Chroma virtual %x bus %x size %d\n",
                (u32) jpegIn->pictureBufferCbCr.pVirtualAddress,
                jpegIn->pictureBufferCbCr.busAddress, out_pic_size_chroma);
    }
    else
    {
        fprintf(stdout,
                "\t\t-JPEG: Allocate: Cb virtual %x bus %x size %d\n",
                (u32) jpegIn->pictureBufferCbCr.pVirtualAddress,
                jpegIn->pictureBufferCbCr.busAddress,
                (out_pic_size_chroma / 2));

        fprintf(stdout,
                "\t\t-JPEG: Allocate: Cr virtual %x bus %x size %d\n",
                (u32) jpegIn->pictureBufferCr.pVirtualAddress,
                jpegIn->pictureBufferCr.busAddress, (out_pic_size_chroma / 2));
    }

    return JPEGDEC_OK;
}

/*-----------------------------------------------------------------------------

Print JPEG api return value

-----------------------------------------------------------------------------*/
void PrintJpegRet(JpegDecRet * pJpegRet)
{

    assert(pJpegRet);

    switch (*pJpegRet)
    {
    case JPEGDEC_FRAME_READY:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_FRAME_READY\n");
        break;
    case JPEGDEC_OK:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_OK\n");
        break;
    case JPEGDEC_ERROR:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_ERROR\n");
        break;
    case JPEGDEC_DWL_HW_TIMEOUT:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_HW_TIMEOUT\n");
        break;
    case JPEGDEC_UNSUPPORTED:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_UNSUPPORTED\n");
        break;
    case JPEGDEC_PARAM_ERROR:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_PARAM_ERROR\n");
        break;
    case JPEGDEC_MEMFAIL:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_MEMFAIL\n");
        break;
    case JPEGDEC_INITFAIL:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_INITFAIL\n");
        break;
    case JPEGDEC_HW_BUS_ERROR:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_HW_BUS_ERROR\n");
        break;
    case JPEGDEC_SYSTEM_ERROR:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_SYSTEM_ERROR\n");
        break;
    case JPEGDEC_DWL_ERROR:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_DWL_ERROR\n");
        break;
    case JPEGDEC_INVALID_STREAM_LENGTH:
        fprintf(stdout,
                "TB: jpeg API returned : JPEGDEC_INVALID_STREAM_LENGTH\n");
        break;
    case JPEGDEC_STRM_ERROR:
        fprintf(stdout, "TB: jpeg API returned : JPEGDEC_STRM_ERROR\n");
        break;
    case JPEGDEC_INVALID_INPUT_BUFFER_SIZE:
        fprintf(stdout,
                "TB: jpeg API returned : JPEGDEC_INVALID_INPUT_BUFFER_SIZE\n");
        break;
    case JPEGDEC_INCREASE_INPUT_BUFFER:
        fprintf(stdout,
                "TB: jpeg API returned : JPEGDEC_INCREASE_INPUT_BUFFER\n");
        break;
    case JPEGDEC_SLICE_MODE_UNSUPPORTED:
        fprintf(stdout,
                "TB: jpeg API returned : JPEGDEC_SLICE_MODE_UNSUPPORTED\n");
        break;
    default:
        fprintf(stdout, "TB: jpeg API returned unknown status\n");
        break;
    }
}

/*------------------------------------------------------------------------------

    Function name:  JpegDecMalloc

------------------------------------------------------------------------------*/
void *JpegDecMalloc(unsigned int size)
{
    void *memPtr = (char *) malloc(size);

    return memPtr;
}

/*------------------------------------------------------------------------------

    Function name:  JpegDecMemset

------------------------------------------------------------------------------*/
void *JpegDecMemset(void *ptr, int c, unsigned int size)
{
    void *rv = NULL;

    if(ptr != NULL)
    {
        rv = memset(ptr, c, size);
    }
    return rv;
}

/*------------------------------------------------------------------------------

    Function name:  JpegDecFree

------------------------------------------------------------------------------*/
void JpegDecFree(void *ptr)
{
    if(ptr != NULL)
    {
        free(ptr);
    }
}

/*------------------------------------------------------------------------------

    Function name:  JpegDecTrace

    Purpose:
        Example implementation of JpegDecTrace function. Prototype of this
        function is given in jpegdecapi.h. This implementation appends
        trace messages to file named 'dec_api.trc'.

------------------------------------------------------------------------------*/
void JpegDecTrace(const char *string)
{
    FILE *fp;

    fp = fopen("dec_api.trc", "at");

    if(!fp)
        return;

    fwrite(string, 1, strlen(string), fp);
    fwrite("\n", 1, 1, fp);

    fclose(fp);
}

/*-----------------------------------------------------------------------------

    Function name:  FindImageInfoEnd

    Purpose:
        Finds 0xFFC4 from the stream and pOffset includes number of bytes to
        this marker. In case of an error returns != 0
        (i.e., the marker not found).

-----------------------------------------------------------------------------*/
u32 FindImageInfoEnd(u8 * pStream, u32 streamLength, u32 * pOffset)
{
    u32 i;

    for(i = 0; i < streamLength; ++i)
    {
        if(0xFF == pStream[i])
        {
            if(((i + 1) < streamLength) && 0xC4 == pStream[i + 1])
            {
                *pOffset = i;
                return 0;
            }
        }
    }
    return -1;
}

void WriteCroppedOutput(JpegDecImageInfo * info, u8 * dataLuma, u8 * dataCb,
                        u8 * dataCr)
{
    u32 i, j;
    FILE *foutput = NULL;
    u8 *pYuvOut = NULL;
    u32 lumaW, lumaH, chromaW, chromaH, chromaOutputWidth, chromaOutputHeight;

    fprintf(stdout, "TB: WriteCroppedOut, displayW %d, displayH %d\n",
            info->displayWidth, info->displayHeight);

    foutput = fopen("cropped.yuv", "wb");
    if(foutput == NULL)
    {
        fprintf(stdout, "UNABLE TO OPEN OUTPUT FILE\n");
        return;
    }

    if(info->outputFormat == JPEGDEC_YCbCr420_SEMIPLANAR)
    {
        lumaW = (info->displayWidth + 1) & ~0x1;
        lumaH = (info->displayHeight + 1) & ~0x1;
        chromaW = lumaW / 2;
        chromaH = lumaH / 2;
        chromaOutputWidth = info->outputWidth / 2;
        chromaOutputHeight = info->outputHeight / 2;
    }
    else if(info->outputFormat == JPEGDEC_YCbCr422_SEMIPLANAR)
    {
        lumaW = (info->displayWidth + 1) & ~0x1;
        lumaH = info->displayHeight;
        chromaW = lumaW / 2;
        chromaH = lumaH;
        chromaOutputWidth = info->outputWidth / 2;
        chromaOutputHeight = info->outputHeight;
    }
    else if(info->outputFormat == JPEGDEC_YCbCr440)
    {
        lumaW = info->displayWidth;
        lumaH = (info->displayHeight + 1) & ~0x1;
        chromaW = lumaW;
        chromaH = lumaH / 2;
        chromaOutputWidth = info->outputWidth;
        chromaOutputHeight = info->outputHeight / 2;
    }
    else if(info->outputFormat == JPEGDEC_YCbCr411_SEMIPLANAR)
    {
        lumaW = (info->displayWidth + 3) & ~0x3;
        lumaH = info->displayHeight;
        chromaW = lumaW / 4;
        chromaH = lumaH;
        chromaOutputWidth = info->outputWidth / 4;
        chromaOutputHeight = info->outputHeight;
    }
    else if(info->outputFormat == JPEGDEC_YCbCr444_SEMIPLANAR)
    {
        lumaW = info->displayWidth;
        lumaH = info->displayHeight;
        chromaW = lumaW;
        chromaH = lumaH;
        chromaOutputWidth = info->outputWidth;
        chromaOutputHeight = info->outputHeight;
    }
    else
    {
        lumaW = info->displayWidth;
        lumaH = info->displayHeight;
        chromaW = 0;
        chromaH = 0;
        chromaOutputHeight = 0;
        chromaOutputHeight = 0;

    }

    /* write decoder output to file */
    pYuvOut = dataLuma;
    for(i = 0; i < lumaH; i++)
    {
        fwrite(pYuvOut, sizeof(u8), lumaW, foutput);
        pYuvOut += info->outputWidth;
    }

    pYuvOut += (info->outputHeight - lumaH) * info->outputWidth;

    /* baseline -> output in semiplanar format */
    if(info->codingMode != JPEGDEC_PROGRESSIVE)
    {
        for(i = 0; i < chromaH; i++)
            for(j = 0; j < chromaW; j++)
                fwrite(pYuvOut + i * chromaOutputWidth * 2 + j * 2,
                       sizeof(u8), 1, foutput);
        for(i = 0; i < chromaH; i++)
            for(j = 0; j < chromaW; j++)
                fwrite(pYuvOut + i * chromaOutputWidth * 2 + j * 2 + 1,
                       sizeof(u8), 1, foutput);
    }
    else
    {
        pYuvOut = dataCb;
        for(i = 0; i < chromaH; i++)
        {
            fwrite(pYuvOut, sizeof(u8), chromaW, foutput);
            pYuvOut += chromaOutputWidth;
        }
        /*pYuvOut += (chromaOutputHeight-chromaH)*chromaOutputWidth; */
        pYuvOut = dataCr;
        for(i = 0; i < chromaH; i++)
        {
            fwrite(pYuvOut, sizeof(u8), chromaW, foutput);
            pYuvOut += chromaOutputWidth;
        }
    }

    fclose(foutput);
}

void WriteProgressiveOutput(u32 sizeLuma, u32 sizeChroma, u32 mode,
                            u8 * dataLuma, u8 * dataCb, u8 * dataCr)
{
    u32 i;
    FILE *foutput = NULL;

    fprintf(stdout, "TB: WriteProgressiveOutput\n");

    foutput = fopen("out.yuv", "ab");
    if(foutput == NULL)
    {
        fprintf(stdout, "UNABLE TO OPEN OUTPUT FILE\n");
        return;
    }

    /* write decoder output to file */
    fwrite(dataLuma, sizeof(u8), sizeLuma, foutput);
    fwrite(dataCb, sizeof(u8), sizeChroma / 2, foutput);
    fwrite(dataCr, sizeof(u8), sizeChroma / 2, foutput);

    fclose(foutput);
}
/*------------------------------------------------------------------------------

    Function name: printJpegVersion

    Functional description: Print version info

    Inputs:

    Outputs:    NONE

    Returns:    NONE

------------------------------------------------------------------------------*/
void printJpegVersion(void)
{

    JpegDecApiVersion decVer;
    JpegDecBuild decBuild;

    /*
     * Get decoder version info
     */

    decVer = JpegGetAPIVersion();
    printf("\nAPI version:  %d.%d, ", decVer.major, decVer.minor);

    decBuild = JpegDecGetBuild();
    printf("sw build nbr: %d, hw build nbr: %x\n\n",
           decBuild.swBuild, decBuild.hwBuild);

}
