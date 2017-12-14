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
--  Abstract : post-processor testbench
--
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef __arm__
#include <fcntl.h>
#include <sys/mman.h>
#include "memalloc.h"
#endif
#include "pptestbench.h"

#include "defs.h"
#include "frame.h"
#include "pp.h"
#include "cfg.h"
#include "dwl.h"

#include "ppapi.h"
#include "ppinternal.h"

#ifdef MD5SUM
#include "tb_md5.h"
#endif

#define PP_BLEND_COMPONENT0_FILE "pp_ablend1_in.raw"
#define PP_BLEND_COMPONENT1_FILE "pp_ablend2_in.raw"

typedef struct
{
    PpParams *pp;
    PpParams *currPp;
    u32 ppParamIdx;
    u32 numPpParams;
} ppContainer;

static ppContainer pp = { NULL, NULL, 0, 0 };

/* Kernel driver for linear memory allocation of SW/HW shared memories */
const char *memdev = "/tmp/dev/memalloc";
static int memdev_fd = -1;
static int fd_mem = -1;

static u32 in_pic_size = 8 * 2048 * 1536;
u32 in_pic_ba = 0;
#ifndef __arm__
u32 *in_pic_va = NULL;
#else
u32 *in_pic_va = MAP_FAILED;
#endif

DWLLinearMem_t buffer;
DWLLinearMem_t bufferOut;

FILE *fbddata0 = NULL;       /* blend component 0 file */
FILE *fbddata1 = NULL;       /* blend component 1 file */
char blendfile[2][256] = { "\n", "\n" };

static u32 out_pic_size = (4096 * 4096 * 4 * 2 * 4);
u32 out_pic_ba = 0;
#ifndef __arm__
u32 *out_pic_va = NULL;
#else
u32 *out_pic_va = MAP_FAILED;
#endif

u32 multibuffer = 0;
u32 forceTiledInput = 0;
u32 planarToSemiplanarInput = 0;

u32 numberOfBuffers = 2;
PPOutputBuffers outputBuffer;
u32 *pMultibuffer_va[17];

u32 display_buff_ba = 0;
u32 *display_buff_va = NULL;
PPOutput last_out;

u32 *interlaced_frame_va = NULL;
#ifdef ASIC_TRACE_SUPPORT
extern u8 *ablendBase[2];
extern u32 ablendWidth[2];
extern u32 ablendHeight[2];
#endif /* ASIC_TRACE_SUPPORT */

static u32 blend_size[2] = { 0, 0 };
u32 blend_ba[2] = { 0, 0 };
#ifndef __arm__
u32 *blend_va[2] = { NULL, NULL };
#else
u32 *blend_va[2] = { MAP_FAILED, MAP_FAILED };
#endif
u32 *blendTmp[2] = { NULL, NULL };
u32 *blendTmp444[2] = { NULL, NULL };
u32 blendpos[2] = { 0, 0 };

u32 second_field = 0;        /* output is second field of a frame */

static PPInst ppInst = NULL;
static const void *decoder;

static PPConfig ppConf;

static u32 input_endian_big = 0;
static u32 input_word_swap  = 0;
static u32 output_endian_big = 0;
static u32 output_rgb_swap = 0;
static u32 output_word_swap = 0;
static u32 output_word_swap_16 = 0;
static u32 blend_endian_change = 0;
static u32 output_tiled_4x4 = 0;

static out_pic_pixels;
static decodertype;          /* store type of decoder here */

u32 vc1Multires;             /* multi resolution is use */
u32 vc1Rangered;             /* was picture range reduced */

u32 interlaced = 0;

u32 bFrames;

static int pp_read_input(i32 frame);

static void pp_api_release(PPInst pp);
static void pp_release_buffers(PpParams * pp);

static void ReleasePp(PpParams * pp);
static int WritePicture(u32 * image, i32 size, const char *fname, int frame);
static void MakeTiled(u8 * data, u32 w, u32 h);
static void MakeTiled8x4(u8 * data, u32 w, u32 h, u32);
static void Tiled4x4ToPlanar(u8 * data, u32 w, u32 h);

static void swap_in_out_pointers(u32 width, u32 height);
static void reset_vc1_specific(void);

static int read_mask_file(u32 id, u32);

static int pp_alloc_blend_components(PpParams * pp, u32 ablendCrop);

static void toggle_endian_yuv(u32 width, u32 height, u32 * virtual);

static void Yuv2Rgb(u32 * p, u32 w, u32 h, ColorConversion * cc);

static int pp_load_cfg_file(char *cfgFile, ppContainer * pp);
static void pp_print_result(PPResult ret);

static void TBOverridePpContainer(PPContainer *ppC, const TBCfg * tbCfg);
u32 pp_setup_multibuffer(PPInst pp, const void *decInst,
                            u32 decType, PpParams * params);
/*------------------------------------------------------------------------------
    Function name   : pp_external_run
    Description     : Runs the post processor in a stand alone mode
    Return type     : int
    Argument        : char *cfgFile, const TBCfg* tbCfg
------------------------------------------------------------------------------*/

#if defined(ASIC_TRACE_SUPPORT) && !defined(PP_VP6DEC_PIPELINE_SUPPORT)
extern u32 hwPpPicCount;
extern u32 gHwVer;
#endif

#if defined (PP_EVALUATION)
extern u32 gHwVer;
#endif

int pp_external_run(char *cfgFile, const TBCfg * tbCfg)
{
    int ret = 0, frame, tmpFrame;
    PPResult res;

#if defined(ASIC_TRACE_SUPPORT) && !defined(PP_VP6DEC_PIPELINE_SUPPORT)
    openTraceFiles();
    gHwVer = 8190;
#endif

#if defined(PP_EVALUATION_8170)
    gHwVer = 8170;
#elif defined(PP_EVALUATION_8190)
    gHwVer = 8190;
#elif defined(PP_EVALUATION_9170)
    gHwVer = 9170;
#elif defined(PP_EVALUATION_9190)
    gHwVer = 9190;
#elif defined(PP_EVALUATION_G1)
    gHwVer = 10000;
#endif

    if(pp_startup(cfgFile, NULL, PP_PIPELINE_DISABLED, tbCfg) != 0)
    {
        ret = 1;
        goto end;
    }

    /* Start process */
    for(frame = 0, tmpFrame = 0;; ++frame, ++tmpFrame)
    {
        ret = pp_update_config(NULL, PP_PIPELINE_DISABLED, tbCfg);
        if(ret == CFG_UPDATE_NEW)
            tmpFrame = 0;
        else if(ret == CFG_UPDATE_FAIL)
            goto end;

        if(pp_read_input(tmpFrame) != 0)
            break;
        if(frame)
        {
            (void) pp_read_blend_components( ppInst );
        }

        if(pp.currPp->inputTraceMode == 2)  /* tiled input */
        {
            MakeTiled((u8 *) in_pic_va, pp.currPp->input.width,
                      pp.currPp->input.height);
        }

        res = PPGetResult(ppInst);
        pp_print_result(res);

        if(res != PP_OK)
        {
            printf("ERROR: PPGetResult returned %d\n", res);
            break;
        }
#ifdef PP_WRITE_BMP /* enable for writing BMP headers for rgb images */
        /* For internal debug use only */
        if(ppConf.ppOutImg.pixFormat & 0x040000)
        {
            u32 w, h;

            w = pp.currPp->pip ? pp.currPp->pip->width : ppConf.ppOutImg.width;
            h = pp.currPp->pip ? pp.currPp->pip->height : ppConf.ppOutImg.
                height;

            WriteBmp(pp.currPp->outputFile,
                     ppConf.ppOutImg.pixFormat, (u8 *) out_pic_va, w, h);
        }
        else
#endif
	    pp_write_output(frame, 0, 0);

    }

  end:

    pp_close();

#if defined(ASIC_TRACE_SUPPORT) && !defined(PP_VP6DEC_PIPELINE_SUPPORT)
    trace_SequenceCtrlPp(hwPpPicCount);
    closeTraceFiles();
#endif

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : pp_close
    Description     : Clean up
    Return type     : void
    Argument        : void
------------------------------------------------------------------------------*/
void pp_close(void)
{
    pp_api_release(ppInst);
    if(pp.currPp)
        pp_release_buffers(pp.currPp);
    if(pp.pp)
        free(pp.pp);
}

/*------------------------------------------------------------------------------
    Function name   : pp_read_input
    Description     : Read the input frame, and perform input word swap and
                      endian changes, if needed.
    Return type     : int
    Argument        : int frame
------------------------------------------------------------------------------*/
int pp_read_input(int frame)
{
    /* Read input frame and resample it to YUV444 or RGB */
    if(!ReadFrame(pp.currPp->inputFile, &pp.currPp->input,
                  pp.currPp->firstFrame + frame, pp.currPp->inputFormat,
                  pp.currPp->inputStruct == PP_PIC_TOP_AND_BOT_FIELD))
    {
        printf("EOF at frame %d\n", pp.currPp->firstFrame + frame);
        return 1;
    }
#ifndef ASIC_TRACE_SUPPORT
    if(input_word_swap)
    {
        PixelData *p = &pp.currPp->input;
        u32 x, bytes = FrameBytes(p, NULL);

        for(x = 0; x < bytes; x += 8)
        {
            u32 tmp = 0;
            u32 *word = (u32 *) (p->base + x);
            u32 *word2 = (u32 *) (p->base + x + 4);

            tmp = *word;
            *word = *word2;
            *word2 = tmp;
        }
    }
    if(input_endian_big)
    {
        PixelData *p = &pp.currPp->input;
        u32 x, bytes = FrameBytes(p, NULL);

        for(x = 0; x < bytes; x += 4)
        {
            u32 tmp = 0;
            u32 *word = (u32 *) (p->base + x);

            tmp |= (*word & 0xFF) << 24;
            tmp |= (*word & 0xFF00) << 8;
            tmp |= (*word & 0xFF0000) >> 8;
            tmp |= (*word & 0xFF000000) >> 24;
            *word = tmp;
        }
    }
#endif

    /* Make conversion to tiled if input mode forced, so as to make 
     * a bit more visually pleasing output. Note that this doesn't
     * perform planar-->semiplanar so planar YUV inputs will still
     * have faulty color channels*/
    if(forceTiledInput &&
       (pp.currPp->inputFormat == YUV420C ))
    {
        MakeTiled8x4( pp.currPp->input.base, 
                      pp.currPp->input.width,
                      pp.currPp->input.height,
                      planarToSemiplanarInput );
    }

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : pp_write_output
    Description     : Write the output frame, and perform output word swap and
                      endian changes, if needed.
    Return type     : void
    Argument        : int frame
------------------------------------------------------------------------------*/

void pp_write_output(int frame, u32 fieldOutput, u32 top)
{
    u32 bytes;
    PPOutput tmpout;
    PPResult ret;
    u32 * pOut_va = NULL;

    if(decoder != NULL && multibuffer)
    {
        /* currently API struct for multibuffers does not contain virtual
         * addresses. loop goes through list of buffers. when match for
         * current out is found, same index used to get virtual for
         * virtual buffer list */
        u32 k;
        ret = PPGetNextOutput(ppInst, &tmpout);
        ASSERT(ret == PP_OK);
        
        for(k=0; k<numberOfBuffers; k++)
        {
            if(tmpout.bufferBusAddr == outputBuffer.ppOutputBuffers[k].bufferBusAddr)
            {
                pOut_va = pMultibuffer_va[k];
                break;
            }
        }

        if(tmpout.bufferBusAddr == display_buff_ba)
        {
            /* it's our spare buffer */
            pOut_va = display_buff_va;
        }
        
        ASSERT(pOut_va != NULL);

        /* do we double buffer for display? */
        if(display_buff_ba)
        {
            /* for field output swap just after second field is ready */
            if((!fieldOutput) || (fieldOutput && second_field))
            {
                /* swap the output with a new buffer */
                //ret = PPDecSwapLastOutputBuffer(ppInst, &tmpout, &last_out);

                /* save current output for next swap */
                last_out = tmpout;
            }
        }

    }
    else
    {
        pOut_va = out_pic_va;
    }
    
    switch (pp.currPp->outputFormat)
    {
    case YUV420C:
        bytes = (out_pic_pixels * 3) / 2;
        break;
    case YUYV422:
    case YVYU422:
    case UYVY422:
    case VYUY422:
    case YUYV422T4:
    case YVYU422T4:
    case UYVY422T4:
    case VYUY422T4:
        bytes = out_pic_pixels * 2;
        break;
    case RGB:
        bytes = out_pic_pixels * pp.currPp->outRgbFmt.bytes;

    }

    /* if picture upside down, write fields in inverted order. looks better */
    top = top ? 1 : 0;
    if(pp.currPp->rotation == 180 || pp.currPp->rotation == -180 ||
       pp.currPp->rotation == 1)
        top = 1 - top;


#ifndef ASIC_TRACE_SUPPORT

    if(output_tiled_4x4)
    {
        Tiled4x4ToPlanar( (u8*)pOut_va, 
                          ppConf.ppOutImg.width,
                          ppConf.ppOutImg.height );
    }

    if(((PPContainer*)ppInst)->hwEndianVer == 0) /* HWIF_PPD_OEN_VERSION */
    {

        if(output_endian_big)
        {
            /*printf("output big endian\n"); */
            u32 x;

            for(x = 0; x < bytes; x += 4)
            {
                u32 tmp = 0;
                u32 *word = (u32 *) (((u32) pOut_va) + x);

                tmp |= (*word & 0xFF) << 24;
                tmp |= (*word & 0xFF00) << 8;
                tmp |= (*word & 0xFF0000) >> 8;
                tmp |= (*word & 0xFF000000) >> 24;
                *word = tmp;
            }
        }

        if(output_rgb_swap)
        {
            u32 x;

            /*printf("output rgb swap\n"); */

            for(x = 0; x < bytes; x += 4)
            {
                u32 tmp = 0;
                u32 *word = (u32 *) (((u32) pOut_va) + x);

                tmp |= (*word & 0xFF) << 8;
                tmp |= (*word & 0xFF00) >> 8;
                tmp |= (*word & 0xFF0000) << 8;
                tmp |= (*word & 0xFF000000) >> 8;
                *word = tmp;
            }
        }

        if(output_word_swap)
        {
            u32 x;

            /*printf("output word swap\n"); */

            for(x = 0; x < bytes; x += 4 * 2)
            {
                u32 tmp1 = 0;
                u32 tmp2 = 0;
                u32 *word1 = (u32 *) (((u32) pOut_va) + x);
                u32 *word2 = (u32 *) (((u32) pOut_va) + x + 4);

                tmp1 = *word1;
                tmp2 = *word2;
                *word1 = tmp2;
                *word2 = tmp1;
            }
        }
    }
    else /* HWIF_PPD_OEN_VERSION == 1 */
    {

        if(output_word_swap)
        {
            u32 x;

            /*printf("output word swap\n"); */

            for(x = 0; x < bytes; x += 4 * 2)
            {
                u32 tmp1 = 0;
                u32 tmp2 = 0;
                u32 *word1 = (u32 *) (((u32) pOut_va) + x);
                u32 *word2 = (u32 *) (((u32) pOut_va) + x + 4);

                tmp1 = *word1;
                tmp2 = *word2;
                *word1 = tmp2;
                *word2 = tmp1;
            }
        }

        if(output_word_swap_16)
        {
            u32 x;

            /*printf("output word swap\n"); */

            for(x = 0; x < bytes; x += 4 )
            {
                u32 tmp1 = 0;
                u32 tmp2 = 0;
                u32 *word = (u32 *) (((u32) pOut_va) + x);

                tmp1 = ((*word) >> 16) & 0xFFFF;
                tmp2 = (*word) & 0xFFFF;
                *word = (tmp2 << 16) | (tmp1) ;
            }
        }

        if(output_endian_big)
        {
            /*printf("output big endian\n"); */
            u32 x;

            for(x = 0; x < bytes; x += 4)
            {
                u32 tmp = 0;
                u32 *word = (u32 *) (((u32) pOut_va) + x);

                tmp |= (*word & 0xFF) << 24;
                tmp |= (*word & 0xFF00) << 8;
                tmp |= (*word & 0xFF0000) >> 8;
                tmp |= (*word & 0xFF000000) >> 24;
                *word = tmp;
            }
        }
    }
#endif
    /* combine YUV 420 fields back into frames for testing */
    if(fieldOutput && (pp.currPp->outputFormat == YUV420C))
    {
        u32 x;
        u32 startOfCh;
        u8 *p, *pfi;
        u32 widt = pp.currPp->output.width;
        u32 heig = pp.currPp->output.height;

        if(pp.currPp->pip)
            heig = pp.currPp->pip->height;

        if(interlaced_frame_va == NULL)
            interlaced_frame_va = malloc(widt * heig * 3);

        ASSERT(interlaced_frame_va);

        pfi = (u8 *) pOut_va;
        p = (u8 *) interlaced_frame_va;

        if(!top)
            p += widt;

        /* luma */
        for(x = 0; x < heig; x++, p += (widt * 2))
            memcpy(p, pfi + (x * widt), widt);

        startOfCh = (widt * heig);

        /* chroma */
        for(x = 0; x < heig / 2; x++, p += (widt * 2))
            memcpy(p, pfi + startOfCh + (x * widt), widt);

        if(second_field)
        {
            WritePicture(interlaced_frame_va, bytes * 2, pp.currPp->outputFile,
                         frame);
        }
        second_field ^= 1;
    }
    else
    {
        WritePicture(pOut_va, bytes, pp.currPp->outputFile, frame);

        if(multibuffer)
            memset(pOut_va, 0, bytes);

        /* Reset vc1 picture specific information */
        reset_vc1_specific();
    }

}

/*------------------------------------------------------------------------------
    Function name   : pp_write_output_plain
    Description     : Write output picture without considering endiannness modes.
                      This is useful if the endianness is already handled
                      (not coded picture)

    Return type     : void
    Argument        : int frame
------------------------------------------------------------------------------*/
void pp_write_output_plain(int frame)
{
    u32 bytes;

    switch (pp.currPp->outputFormat)
    {
    case YUV420C:
        bytes = (out_pic_pixels * 3) / 2;
        break;
    case YUYV422:
        bytes = out_pic_pixels * 2;
        break;
    case RGB:
        bytes = out_pic_pixels * pp.currPp->outRgbFmt.bytes;

    }

    WritePicture(out_pic_va, bytes, pp.currPp->outputFile, frame);

}

/*------------------------------------------------------------------------------
    Function name   : pp_startup
    Description     : Prepare the post processor according to the post processor
                      configuration file. Allocate input and output picture
                      buffers.
    Return type     : int
    Argument        : char *ppCfgFile
    Argument        : const void *decInst
    Argument        : u32 decType
------------------------------------------------------------------------------*/
int pp_startup(char *ppCfgFile, const void *decInst, u32 decType,
               const TBCfg * tbCfg)
{
    int ret = 0;

    if(pp_load_cfg_file(ppCfgFile, &pp) != 0)
    {
        ret = 1;
        goto end;
    }

    printf("---init PP API---\n");

    if(pp_api_init(&ppInst, decInst, decType) != 0)
    {
        printf("\t\tFAILED\n");
        ret = 1;
        goto end;
    }

  end:

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : pp_alloc_buffers
    Description     : Allocate input and output picture buffers.
    Return type     : int
------------------------------------------------------------------------------*/
int pp_alloc_buffers(PpParams * pp, u32 ablendCrop)
{
    int ret = 0;
    int maxDimension = 0;

#ifdef __arm__
    MemallocParams params;
    
    memdev_fd = open(memdev, O_RDWR);
    if(memdev_fd == -1)
    {
        printf("Failed to open dev: %s\n", memdev);
        return -1;
    }

    /* open mem device for memory mapping */
    fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd_mem == -1)
    {
        printf("Failed to open: %s\n", "/dev/mem");
        ret = 1;
        goto err;
    }
#endif

    if(pp_alloc_blend_components(pp, ablendCrop))
    {
        printf("failed to alloc blend components\n");
        return -1;
    }
    
#ifndef __arm__
    /* Allocations for sw/sw testing */

#ifndef PP_PIPELINE_ENABLED
    if(in_pic_va == NULL)
        in_pic_va = malloc(in_pic_size);
    ASSERT(in_pic_va != NULL);    
#endif
    
    if(out_pic_va == NULL)
        out_pic_va = malloc(out_pic_size);
    
    ASSERT(out_pic_va != NULL);
    
    in_pic_ba = (u32) in_pic_va;
    out_pic_ba = (u32) out_pic_va;

    /* allocate extra buffer for swapping the multibuffer output */
    if (multibuffer)
    {
        if(display_buff_va == NULL)
        {
            u32 s;

            if(pp->pip == NULL)
            {
                s = pp->output.width * pp->output.height * 4;
            }
            else
            {
                s = pp->pip->width * pp->pip->height * 4;
            }

            display_buff_va = (u32 *)malloc(s);
        }

        ASSERT(display_buff_va != NULL);

        display_buff_ba = (u32)display_buff_va;
    }

#else
    /* allocation for sw/hw testing on fpga */
    
    /* if PIP used, alloc based on that, otherwise output size */
    if(pp->pip == NULL)
    {
        params.size = pp->output.width * pp->output.height * 4;
        maxDimension = MAX(pp->output.width, pp->output.height);
    }
    else
    {
        params.size = pp->pip->width * pp->pip->height * 4;
        maxDimension = MAX(pp->pip->width, pp->pip->height);
    }

    /* Allocate buffers */
    if(multibuffer != 0)
    {   
        /* Allocate more memory if running 4K case (must be run with a board
         * with a larger memory config than 128M */
        if(maxDimension > 1920)
            params.size = 4096 * 4096 * 4 * 2;   
        else    
        /* params.size = 4096*4096; */ /* allocate one big buffer that will be split */
        /* Buffer increased for cases 2215 & 8412 */
            params.size = 1920 * 1920 * 4 * 2;
    }
    
    out_pic_size = params.size;
    
    /* Allocate buffer and get the bus address */
    ioctl(memdev_fd, MEMALLOC_IOCXGETBUFFER, &params);

    if(params.busAddress == 0)
    {
        printf("Output allocation failed\n");
        goto err;

    }
    out_pic_ba = params.busAddress;

#ifndef PP_PIPELINE_ENABLED
    /* input buffer allocation */
    in_pic_size = params.size = pp->input.width * pp->input.height * 2;

    /* Allocate buffer and get the bus address */
    ioctl(memdev_fd, MEMALLOC_IOCXGETBUFFER, &params);

    if(params.busAddress == 0)
    {
        printf("Input allocation failed\n");
        goto err;
    }
    in_pic_ba = params.busAddress;

        /* Map the bus address to virtual address */
    in_pic_va = MAP_FAILED;
    in_pic_va = (u32 *) mmap(0, in_pic_size, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd_mem, in_pic_ba);

    if(in_pic_va == MAP_FAILED)
    {
        printf("Failed to alloc input image\n");
        ret = 1;
        goto err;
    }
#endif
    
    out_pic_va = MAP_FAILED;
    out_pic_va = (u32 *) mmap(0, out_pic_size, PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd_mem, out_pic_ba);

    if(out_pic_va == MAP_FAILED)
    {
        printf("Failed to alloc input image\n");
        ret = 1;
        goto err;
    }

    printf("Input buffer %ld bytes       Output buffer %ld bytes\n",
           in_pic_size, out_pic_size);
    printf("Input buffer bus address:\t\t0x%08lx\n", in_pic_ba);
    printf("Output buffer bus address:\t\t0x%08x\n", out_pic_ba);
    printf("Input buffer user address:\t\t0x%08lx\n", (u32) in_pic_va);
    printf("Output buffer user address:\t\t0x%08lx\n", (u32) out_pic_va);
#endif

    memset(out_pic_va, 0x0, out_pic_size);

  err:
    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : pp_release_buffers
    Description     : Release input and output picture buffers.
    Return type     : void
------------------------------------------------------------------------------*/
void pp_release_buffers(PpParams * pp)
{

#ifndef __arm__

    if(in_pic_va != NULL)
         free(in_pic_va);
    if(out_pic_va != NULL)
        free(out_pic_va);
    if(display_buff_va != NULL)
        free(display_buff_va);
    
    in_pic_va = NULL;
    out_pic_va = NULL;
    display_buff_va = NULL;
    
#else
    if(in_pic_va != MAP_FAILED)
        munmap(in_pic_va, in_pic_size);

    in_pic_va = MAP_FAILED;

    if(out_pic_va != MAP_FAILED)
        munmap(out_pic_va, out_pic_size);

    out_pic_va = MAP_FAILED;

    if(in_pic_ba != 0)
        ioctl(memdev_fd, MEMALLOC_IOCSFREEBUFFER, &in_pic_ba);

    in_pic_ba = 0;

    if(out_pic_ba != 0)
        ioctl(memdev_fd, MEMALLOC_IOCSFREEBUFFER, &out_pic_ba);

    out_pic_ba = 0;

    if(memdev_fd != -1)
        close(memdev_fd);

    if(fd_mem != -1)
        close(fd_mem);

    fd_mem = memdev_fd = -1;
#endif

/* release  blend buffers */
    if(blendTmp[0])
    {
        free(blendTmp[0]);
        blendTmp[0] = NULL;
    }
    if(blendTmp[1])
    {
        free(blendTmp[1]);
        blendTmp[1] = NULL;
    }
    if(blendTmp444[0])
    {
        free(blendTmp444[0]);
        blendTmp444[0] = NULL;
    }
    if(blendTmp444[1])
    {
        free(blendTmp444[1]);
        blendTmp444[1] = NULL;
    }    
    
#ifndef __arm__
    if(blend_va[0])
    {
        free(blend_va[0]);
        blend_va[0] = NULL;
    }
    if(blend_va[1])
    {
        free(blend_va[1]);
        blend_va[1] = NULL;
    }
#else
    if(blend_va[0] != MAP_FAILED)
    {
        munmap(blend_va[0], blend_size[0]);
        blend_va[0] = MAP_FAILED;
    }
    if(blend_va[1] != MAP_FAILED)
    {
        munmap(blend_va[1], blend_size[1]);
        blend_va[1] = MAP_FAILED;
    }

    if(blend_ba[0] != 0)
    {
        ioctl(memdev_fd, MEMALLOC_IOCSFREEBUFFER, &blend_ba[0]);
        blend_ba[0] = 0;
    }

    if(blend_ba[1] != 0)
    {
        ioctl(memdev_fd, MEMALLOC_IOCSFREEBUFFER, &blend_ba[1]);
        blend_ba[1] = 0;
    }
#endif    
    
    ReleaseFrame(&pp->input);

    ReleaseFrame(&pp->output);

    if(interlaced_frame_va != NULL)
    {
        free(interlaced_frame_va);
        interlaced_frame_va = NULL;
    }

    ReleasePp(pp);
}

/*------------------------------------------------------------------------------
    Function name   : pp_api_init
    Description     : Initialize the post processor. Enables joined mode if needed.
    Return type     : int
    Argument        : PPInst * pp, const void *decInst, u32 decType
------------------------------------------------------------------------------*/
int pp_api_init(PPInst * pp, const void *decInst, u32 decType)
{
    PPResult res;

    res = PPInit(pp, 0);

    if(res != PP_OK)
    {
        printf("Failed to init the PP: %d\n", res);

        *pp = NULL;

        return 1;
    }

    decoder = NULL;

    decodertype = decType;

    if(decInst != NULL && decType != PP_PIPELINE_DISABLED)
    {
        res = PPDecCombinedModeEnable(*pp, decInst, decType);

        if(res != PP_OK)
        {
            printf("Failed to enable PP-DEC pipeline: %d\n", res);

            *pp = NULL;

            return 1;
        }
        decoder = decInst;
    }

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : pp_api_release
    Description     : Release the post processor. Disable pipeline if needed.
    Return type     : void
    Argument        : PPInst * pp
------------------------------------------------------------------------------*/
void pp_api_release(PPInst pp)
{
    if(pp != NULL && decoder != NULL)
        PPDecCombinedModeDisable(pp, decoder);

    if(pp != NULL)
        PPRelease(pp);
}

/*------------------------------------------------------------------------------
    Function name   : pp_api_cfg
    Description     : Set the post processor according to the post processor
                      configuration.
    Return type     : int
    Argument        : PPInst pp
    Argument        : const void *decInst
    Argument        : u32 decType
    Argument        : PpParams * params
    Argument        : const TBCfg* tbCfg
------------------------------------------------------------------------------*/
int pp_api_cfg(PPInst pp, const void *decInst, u32 decType, PpParams * params,
               const TBCfg * tbCfg)
{
    int ret = 0;
    PPResult res;

    u32 clock_gating = TBGetPPClockGating(tbCfg);
    u32 output_endian = TBGetPPOutputPictureEndian(tbCfg);
    u32 input_endian = TBGetPPInputPictureEndian(tbCfg);
    u32 bus_burst_length = tbCfg->ppParams.busBurstLength;
    u32 word_swap = TBGetPPWordSwap(tbCfg);
    u32 word_swap_16 = TBGetPPWordSwap16(tbCfg);
    u32 data_discard = TBGetPPDataDiscard(tbCfg);
    u32 decOutFormat = TBGetDecOutputFormat(tbCfg);

    u32 *regBase;
    multibuffer = TBGetTBMultiBuffer(tbCfg);
    if(multibuffer && 
       (decType == PP_PIPELINED_DEC_TYPE_VP6 ||
        decType == PP_PIPELINED_DEC_TYPE_VP8 ||
        decType == PP_PIPELINED_DEC_TYPE_WEBP ||
        decType == PP_PIPELINE_DISABLED))
    {
        multibuffer = 0;
    }

    regBase = ((PPContainer *) pp)->ppRegs;

    if(interlaced) 
    {
        decOutFormat = 0;
    }


#ifndef ASIC_TRACE_SUPPORT

    /* set up pp device configuration */
    {

        printf("---Clock Gating %d ---\n", clock_gating);
        SetPpRegister(regBase, HWIF_PP_CLK_GATE_E, clock_gating);

        printf("---Amba Burst Length %d ---\n", bus_burst_length);
        SetPpRegister(regBase, HWIF_PP_MAX_BURST, bus_burst_length);

        printf("---Data discard %d ---\n", data_discard);
        SetPpRegister(regBase, HWIF_PP_DATA_DISC_E, data_discard);

        blend_endian_change = 0;
        input_endian_big = 0;
        output_endian_big = 0;
        output_rgb_swap = 0;
        output_word_swap = 0;
        output_word_swap_16 = 0;
        output_tiled_4x4 = 0;

        if(PP_IS_TILED_4(params->outputFormat))
        {
            printf("---tiled 4x4 output---\n");
            output_tiled_4x4 = 1;
        }

        /* Override the configuration endianess (i.e., randomize is on) */
        if(0 == input_endian)
        {
            printf("---input endian BIG---\n");
            SetPpRegister(regBase, HWIF_PP_IN_ENDIAN, 0);
            input_endian_big = 1;
            blend_endian_change = 1;
        }
        else if(1 == input_endian)
        {
            printf("---input endian LITTLE---\n");
            SetPpRegister(regBase, HWIF_PP_IN_ENDIAN, 1);
            input_endian_big = 0;
        }

        if(0 == output_endian)
        {
            printf("---output endian BIG---\n");
            SetPpRegister(regBase, HWIF_PP_OUT_ENDIAN, 0);

            if(RGB == params->outputFormat && 2 != params->outRgbFmt.bytes)
            {
                /* endian does not affect to 32 bit RGB */
                if(((PPContainer*)ppInst)->hwEndianVer == 0)
                    output_endian_big = 0;
                else
                    output_endian_big = 1;
            }
            else if(RGB == params->outputFormat && 2 == params->outRgbFmt.bytes)
            {
                output_endian_big = 1;
                /* swap also the 16 bits words for 16 bit RGB */
                if(((PPContainer*)ppInst)->hwEndianVer == 0)
                    output_rgb_swap = 1;
            }
            else
            {
                output_endian_big = 1;
            }
        }
        else if(1 == output_endian)
        {
            printf("---output endian LITTLE---\n");
            SetPpRegister(regBase, HWIF_PP_OUT_ENDIAN, 1);
            output_endian_big = 0;
        }

        if(0 == word_swap)
        {
            SetPpRegister(regBase, HWIF_PP_OUT_SWAP32_E, 0);
            output_word_swap = 0;
        }
        else if(1 == word_swap)
        {
            SetPpRegister(regBase, HWIF_PP_OUT_SWAP32_E, 1);
            output_word_swap = 1;
        }

        if(((PPContainer*)ppInst)->hwEndianVer > 0)
        {
            if( word_swap_16 == 1 )
            {
                SetPpRegister(regBase, HWIF_PP_OUT_SWAP16_E, 1);
                output_word_swap_16 = 1;
            }
            else if ( word_swap_16 == 0 )
            {
                SetPpRegister(regBase, HWIF_PP_OUT_SWAP16_E, 0);
                output_word_swap_16 = 0;
            }
        }
    }
#endif

    if(forceTiledInput &&
       (params->inputFormat == YUV420 ||
        params->inputFormat == YUV420C ))
    {
        SetPpRegister( regBase, HWIF_PP_TILED_MODE, forceTiledInput);
    }

    res = PPGetConfig(pp, &ppConf);

    if(params->zoom)
    {
        ppConf.ppInCrop.enable = 1;
        ppConf.ppInCrop.originX = params->zoomx0;
        ppConf.ppInCrop.originY = params->zoomy0;
        ppConf.ppInCrop.width = params->zoom->width;

        if(params->crop8Right)
        {
            ppConf.ppInCrop.width -= 8;
        }

        ppConf.ppInCrop.height = params->zoom->height;

        if(params->crop8Down)
        {
            ppConf.ppInCrop.height -= 8;
        }
    }
    else if(params->crop8Right || params->crop8Down)
    {
        ppConf.ppInCrop.enable = 1;
        ppConf.ppInCrop.originX = 0;
        ppConf.ppInCrop.originY = 0;

        ppConf.ppInCrop.width = params->input.width;
        ppConf.ppInCrop.height = params->input.height;

        if(params->crop8Right)
        {
            ppConf.ppInCrop.width -= 8;
        }
        else
        {
            ppConf.ppInCrop.width = params->input.width;
        }

        if(params->crop8Down)
        {
            ppConf.ppInCrop.height -= 8;
        }
        else
        {
            ppConf.ppInCrop.height = params->input.height;
        }
    }

    /* ppConf.ppInImg.crop8Rightmost = params->crop8Right; */
    /* ppConf.ppInImg.crop8Downmost = params->crop8Down; */

    /* if progressive or deinterlace (progressive output */
    ppConf.ppOutDeinterlace.enable = 0;
    if(!params->output.fieldOutput || params->deint.enable)
    {

        if(params->deint.enable)
        {
            ppConf.ppOutDeinterlace.enable = params->deint.enable;
        }

        ppConf.ppInImg.width = params->input.width;
        ppConf.ppInImg.height = params->input.height;

    }
    /* if field output */
    else if(params->output.fieldOutput /* && !params->deint.enable */ )
    {

        ppConf.ppInImg.width = params->input.width;
        ppConf.ppInImg.height = params->input.height;

    }
    else
    {

        ppConf.ppInImg.width = params->input.width;
        ppConf.ppInImg.height = params->input.height;

    }

    ppConf.ppInImg.picStruct = params->inputStruct;

    if(params->inputStruct != PP_PIC_BOT_FIELD)
    {
        ppConf.ppInImg.bufferBusAddr = in_pic_ba;

        ppConf.ppInImg.bufferCbBusAddr =
            in_pic_ba + ppConf.ppInImg.width * ppConf.ppInImg.height;

        ppConf.ppInImg.bufferCrBusAddr =
            ppConf.ppInImg.bufferCbBusAddr +
            (ppConf.ppInImg.width * ppConf.ppInImg.height) / 4;
    }
    else
    {
        ASSERT(params->inputFormat == YUV420C ||
               params->inputFormat == YUYV422 ||
               params->inputFormat == YVYU422 ||
               params->inputFormat == UYVY422 ||
               params->inputFormat == VYUY422);
        ppConf.ppInImg.bufferBusAddrBot = in_pic_ba;

        ppConf.ppInImg.bufferBusAddrChBot =
            in_pic_ba + ppConf.ppInImg.width * ppConf.ppInImg.height;
    }

    if(params->inputStruct == PP_PIC_TOP_AND_BOT_FIELD_FRAME)
    {
        ASSERT(params->inputFormat == YUV420C ||
               params->inputFormat == YUYV422 ||
               params->inputFormat == YVYU422 ||
               params->inputFormat == UYVY422 ||
               params->inputFormat == VYUY422);
        if(params->inputFormat == YUV420C)
        {
            ppConf.ppInImg.bufferBusAddrBot =
                ppConf.ppInImg.bufferBusAddr + params->input.width;
            ppConf.ppInImg.bufferBusAddrChBot =
                ppConf.ppInImg.bufferCbBusAddr + params->input.width;
        }
        else
            ppConf.ppInImg.bufferBusAddrBot =
                ppConf.ppInImg.bufferBusAddr + 2 * params->input.width;
    }
    else if(params->inputStruct == PP_PIC_TOP_AND_BOT_FIELD)
    {
        ASSERT(params->inputFormat == YUV420C ||
               params->inputFormat == YUYV422 ||
               params->inputFormat == YVYU422 ||
               params->inputFormat == UYVY422 ||
               params->inputFormat == VYUY422);
        if(params->inputFormat == YUV420C)
        {
            ppConf.ppInImg.bufferBusAddrBot =
                ppConf.ppInImg.bufferBusAddr +
                params->input.width * params->input.height / 2;
            ppConf.ppInImg.bufferBusAddrChBot =
                ppConf.ppInImg.bufferCbBusAddr +
                params->input.width * params->input.height / 4;
        }
        else
            ppConf.ppInImg.bufferBusAddrBot =
                ppConf.ppInImg.bufferBusAddr +
                2 * params->input.width * params->input.height / 2;
    }

    ppConf.ppInImg.vc1MultiResEnable = vc1Multires;
    ppConf.ppInImg.vc1RangeRedFrm = vc1Rangered;
    ppConf.ppInImg.vc1RangeMapYEnable = params->range.enableY;
    ppConf.ppInImg.vc1RangeMapYCoeff = params->range.coeffY;
    ppConf.ppInImg.vc1RangeMapCEnable = params->range.enableC;
    ppConf.ppInImg.vc1RangeMapCCoeff = params->range.coeffC;

    switch (params->inputFormat)
    {
    case YUV420:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_PLANAR;
        break;
    case YUV420C:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
        break;
    case YUYV422:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED;
        break;
    case YVYU422:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED;
        break;
    case UYVY422:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED;
        break;
    case VYUY422:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED;
        break;
    case YUV422C:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
        break;
    case YUV400:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_0_0;
        break;
    case YUV422CR:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_4_0;
        break;
    case YUV411C:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_1_1_SEMIPLANAR;
        break;
    case YUV444C:
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_4_4_SEMIPLANAR;
        break;
    default:
        ret = 1;
        goto end;
    }

    if(params->inputTraceMode)
    {
        ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_TILED;
    }

    if(decInst != NULL && decType != PP_PIPELINED_DEC_TYPE_JPEG)
    {
        if( decType != PP_PIPELINED_DEC_TYPE_H264 ||
            ppConf.ppInImg.pixFormat != PP_PIX_FMT_YCBCR_4_0_0 )
        {            
#ifdef __arm__
            /* tiled in combined mode supported only in FPGA env */
            ppConf.ppInImg.pixFormat = decOutFormat == 0 ?
                PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR : PP_PIX_FMT_YCBCR_4_2_0_TILED;
#else
            ppConf.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
#endif
        }
    }

    ppConf.ppInImg.videoRange = params->cnv.videoRange;

    ppConf.ppOutImg.width = params->output.width;
    ppConf.ppOutImg.height = params->output.height;

    /* Set output buffer */
    if(multibuffer && (decType == PP_PIPELINED_DEC_TYPE_MPEG4 ||
     decType == PP_PIPELINED_DEC_TYPE_H264 || 
     decType == PP_PIPELINED_DEC_TYPE_MPEG2 ||
     decType == PP_PIPELINED_DEC_TYPE_VC1 ||
     decType == PP_PIPELINED_DEC_TYPE_RV ||
     decType == PP_PIPELINED_DEC_TYPE_AVS) )
    {
        ppConf.ppOutImg.bufferBusAddr = 0;
        if(pp_setup_multibuffer(pp, decInst, decType, params))
            return 1;
    }
    else        
    {
        ppConf.ppOutImg.bufferBusAddr = out_pic_ba;
    }

    ppConf.ppOutImg.bufferChromaBusAddr =
        out_pic_ba+ ppConf.ppOutImg.width * ppConf.ppOutImg.height;

    out_pic_pixels = ppConf.ppOutImg.width * ppConf.ppOutImg.height;

    switch (params->outputFormat)
    {
    case YUV420C:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
        break;
    case YUYV422:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED;
        break;
    case YVYU422:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED;
        break;
    case UYVY422:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED;
        break;
    case VYUY422:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED;
        break;
    case YUYV422T4:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4;
        break;
    case YVYU422T4:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4;
        break;
    case UYVY422T4:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4;
        break;
    case VYUY422T4:
        ppConf.ppOutImg.pixFormat = PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4;
        break;
    case RGB:
        if(params->outRgbFmt.bytes == 2)
        {
            ppConf.ppOutImg.pixFormat = PP_PIX_FMT_RGB16_CUSTOM;
            ppConf.ppOutRgb.rgbBitmask.maskR = params->outRgbFmt.mask[0] >> 16;
            ppConf.ppOutRgb.rgbBitmask.maskG = params->outRgbFmt.mask[1] >> 16;
            ppConf.ppOutRgb.rgbBitmask.maskB = params->outRgbFmt.mask[2] >> 16;
            ppConf.ppOutRgb.rgbBitmask.maskAlpha =
                params->outRgbFmt.mask[3] >> 16;
        }
        else
        {
            ppConf.ppOutImg.pixFormat = PP_PIX_FMT_RGB32_CUSTOM;

            ppConf.ppOutRgb.rgbBitmask.maskR = params->outRgbFmt.mask[0];
            ppConf.ppOutRgb.rgbBitmask.maskG = params->outRgbFmt.mask[1];
            ppConf.ppOutRgb.rgbBitmask.maskB = params->outRgbFmt.mask[2];
            ppConf.ppOutRgb.rgbBitmask.maskAlpha = params->outRgbFmt.mask[3];

            ppConf.ppOutRgb.alpha = 255 /*params->outRgbFmt.mask[3] */ ;
        }

        ppConf.ppOutRgb.ditheringEnable = params->output.ditheringEna;

        break;
    default:
        ret = 1;
        goto end;
    }

    switch (params->rotation)
    {
    case 90:
        ppConf.ppInRotation.rotation = PP_ROTATION_RIGHT_90;
        break;
    case -90:
        ppConf.ppInRotation.rotation = PP_ROTATION_LEFT_90;
        break;
    case 1:
        ppConf.ppInRotation.rotation = PP_ROTATION_HOR_FLIP;
        break;
    case 2:
        ppConf.ppInRotation.rotation = PP_ROTATION_VER_FLIP;
        break;
    case 180:
        ppConf.ppInRotation.rotation = PP_ROTATION_180;
        break;
    default:
        ppConf.ppInRotation.rotation = PP_ROTATION_NONE;

    }

    if(params->pip)
    {
        ppConf.ppOutFrmBuffer.enable = 1;
        ppConf.ppOutFrmBuffer.frameBufferWidth = params->pip->width;
        ppConf.ppOutFrmBuffer.frameBufferHeight = params->pip->height;
        ppConf.ppOutFrmBuffer.writeOriginX = params->pipx0;
        ppConf.ppOutFrmBuffer.writeOriginY = params->pipy0;

        ppConf.ppOutImg.bufferChromaBusAddr =
            out_pic_ba +
            ppConf.ppOutFrmBuffer.frameBufferWidth *
            ppConf.ppOutFrmBuffer.frameBufferHeight;

        out_pic_pixels =
            ppConf.ppOutFrmBuffer.frameBufferWidth *
            ppConf.ppOutFrmBuffer.frameBufferHeight;
    }

    if(params->maskCount != 0)
    {
        ppConf.ppOutMask1.enable = 1;
        ppConf.ppOutMask1.originX = params->mask[0]->x0;
        ppConf.ppOutMask1.originY = params->mask[0]->y0;
        ppConf.ppOutMask1.width = params->mask[0]->x1 - params->mask[0]->x0;
        ppConf.ppOutMask1.height = params->mask[0]->y1 - params->mask[0]->y0;

        /* blending */
        if(params->mask[0]->overlay.enabled /*blend_size[0] != 0*/)
        {

            Overlay *overlay = &params->mask[0]->overlay;

            overlay->enabled = TRUE;

            /* formulate filename for blending component */
            strcpy(blendfile[0], params->mask[0]->overlay.fileName);

            if(read_mask_file(0, ((PPContainer *) pp)->blendCropSupport))
            {
                return 1;
            }

            /* set up configuration */
            ppConf.ppOutMask1.alphaBlendEna = 1;
            ppConf.ppOutMask1.blendComponentBase = blend_ba[0];

            if(((PPContainer *) pp)->blendCropSupport)
            {
                ppConf.ppOutMask1.blendOriginX = params->mask[0]->overlay.x0;
                ppConf.ppOutMask1.blendOriginY = params->mask[0]->overlay.y0;
                ppConf.ppOutMask1.blendWidth = params->mask[0]->overlay.data.width;
                ppConf.ppOutMask1.blendHeight = params->mask[0]->overlay.data.height;
            }

        }
    }

    if(params->maskCount > 1)
    {
        ppConf.ppOutMask2.enable = 1;
        ppConf.ppOutMask2.originX = params->mask[1]->x0;
        ppConf.ppOutMask2.originY = params->mask[1]->y0;
        ppConf.ppOutMask2.width = params->mask[1]->x1 - params->mask[1]->x0;
        ppConf.ppOutMask2.height = params->mask[1]->y1 - params->mask[1]->y0;

        /* blending */
        if(params->mask[1]->overlay.enabled /*blend_size[1] != 0*/)
        {

            Overlay *overlay = &params->mask[1]->overlay;

            overlay->enabled = TRUE;

            /* formulate filename for blending component */
            strcpy(blendfile[1], params->mask[1]->overlay.fileName);

            if(read_mask_file(1, ((PPContainer *) pp)->blendCropSupport))
            {
                return 1;
            }

            /* set up configuration */
            ppConf.ppOutMask2.alphaBlendEna = 1;
            ppConf.ppOutMask2.blendComponentBase = blend_ba[1];

            if(((PPContainer *) pp)->blendCropSupport)
            {
                ppConf.ppOutMask2.blendOriginX = params->mask[1]->overlay.x0;
                ppConf.ppOutMask2.blendOriginY = params->mask[1]->overlay.y0;
                ppConf.ppOutMask2.blendWidth = params->mask[1]->overlay.data.width;
                ppConf.ppOutMask2.blendHeight = params->mask[1]->overlay.data.height;
            }
        }
    }

    ppConf.ppOutRgb.rgbTransform = PP_YCBCR2RGB_TRANSFORM_CUSTOM;

    ppConf.ppOutRgb.rgbTransformCoeffs.a = params->cnv.a;
    ppConf.ppOutRgb.rgbTransformCoeffs.b = params->cnv.b;
    ppConf.ppOutRgb.rgbTransformCoeffs.c = params->cnv.c;
    ppConf.ppOutRgb.rgbTransformCoeffs.d = params->cnv.d;
    ppConf.ppOutRgb.rgbTransformCoeffs.e = params->cnv.e;

    ppConf.ppOutRgb.brightness = params->cnv.brightness;
    ppConf.ppOutRgb.saturation = params->cnv.saturation;
    ppConf.ppOutRgb.contrast = params->cnv.contrast;

    res = PPSetConfig(pp, &ppConf);

    /* Restore the PP setup */
/*    memcpy(&ppConf, &tmpConfig, sizeof(PPConfig));
*/
    if(res != PP_OK)
    {
        printf("Failed to setup the PP\n");
        pp_print_result(res);
        ret = 1;
    }

    /* write deinterlacing parameters */
    if(params->deint.enable)
    {
        SetPpRegister(regBase, HWIF_DEINT_BLEND_E, params->deint.blendEnable);
        SetPpRegister(regBase, HWIF_DEINT_THRESHOLD, params->deint.threshold);
        SetPpRegister(regBase, HWIF_DEINT_EDGE_DET,
                      params->deint.edgeDetectValue);
    }
  end:
    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PrintBinary
    Description     :
    Return type     : void
    Argument        : u32 d
------------------------------------------------------------------------------*/
void PrintBinary(u32 d)
{
    u32 v = 1 << 31;

    while(v)
    {
        printf("%d", (d & v) ? 1 : 0);
        v >>= 1;
    }
    printf("\n");
}

/*------------------------------------------------------------------------------

   .<++>  Function: CheckParams

        Functional description:
          Check parameter consistency

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
bool CheckParams(PpParams * pp)
{
    bool ret = TRUE;
    u32 i, j;

    ASSERT(pp);

#define CHECK_IMAGE_DIMENSION( v, min, max, mask, t) \
    if(v<min || v>max || (v&mask)) { \
        fprintf( stderr, t, v ); \
        ret = FALSE; \
    }

    if(pp->zoom)
    {
        if(pp->zoomx0 + pp->zoom->width > pp->input.width ||
           pp->zoomy0 + pp->zoom->height > pp->input.height ||
           pp->zoomx0 < 0 || pp->zoomy0 < 0)
        {
            fprintf(stderr,
                    "Zoom area [%d.%d]...[%d.%d] exceeds input dimensions [%d.%d].\n",
                    pp->zoomx0, pp->zoomy0, pp->zoomx0 + pp->zoom->width,
                    pp->zoomy0 + pp->zoom->height, pp->input.width,
                    pp->input.height);
            ret = FALSE;
        }
    }

    /* 2. Rotation */
    if(pp->rotation != 0 && pp->rotation != 90 && pp->rotation != -90 &&
       pp->rotation != 1 && pp->rotation != 2 && pp->rotation != 180)
    {
        fprintf(stderr, "Rotation '%d' is invalid.\n", pp->rotation);
        ret = FALSE;
    }
    /* 3. Frame count */
    if(pp->frames == 0)
    {
        fprintf(stderr, "Frame count must be specified >= 1.\n");
        ret = FALSE;
    }
    /* 4. Input sequence must be specified */
    if(!*pp->inputFile)
    {
        fprintf(stderr, "Input sequence must be specified.\n");
        ret = FALSE;
    }
    /* 5. Check color conversion parameters */
    if(pp->cnv.videoRange != 0 && pp->cnv.videoRange != 1)
    {
        fprintf(stderr, "Video range must be inside [0,1].\n");
        ret = FALSE;
    }
    /* 6. byte format specification */
    if(pp->byteFmt.bytes != pp->byteFmt.wordWidth)
    {
        fprintf(stderr, "Inconsistent output byte format specification.\n");
        ret = FALSE;
    }
    else
    {
        /* Process the byte order.. */
        pp->bigEndianFmt.wordWidth = pp->byteFmt.wordWidth;
        pp->bigEndianFmt.bytes = pp->byteFmt.bytes;
        for(i = 0; i < pp->byteFmt.wordWidth; ++i)
        {
            pp->bigEndianFmt.byteOrder[i] = i;
        }
    }
    if(pp->byteFmtInput.bytes != pp->byteFmtInput.wordWidth)
    {
        fprintf(stderr, "Inconsistent input byte format specification.\n");
        ret = FALSE;
    }
    else
    {
        /* Process the byte order.. */
        pp->bigEndianFmtInput.wordWidth = pp->byteFmtInput.wordWidth;
        pp->bigEndianFmtInput.bytes = pp->byteFmtInput.bytes;
        for(i = 0; i < pp->byteFmtInput.wordWidth; ++i)
        {
            pp->bigEndianFmtInput.byteOrder[i] = i;
        }
    }

#undef CHECK_IMAGE_DIMENSION

    /* Prepare channel masks and offsets for trace module */
    if(PP_IS_RGB(pp->outputFormat))
    {
        u32 offsAdjust = 0;

        if(PP_IS_INTERLEAVED(pp->outputFormat))
        {
            u32 bits =
                pp->outRgbFmt.pad[0] + pp->outRgbFmt.chan[0] +
                pp->outRgbFmt.pad[1] + pp->outRgbFmt.chan[1] +
                pp->outRgbFmt.pad[2] + pp->outRgbFmt.chan[2] +
                pp->outRgbFmt.pad[3] + pp->outRgbFmt.chan[3] +
                pp->outRgbFmt.pad[4];
            u32 target = (bits > 16) ? 32 : 16;

            if(target > bits)
            {
                pp->outRgbFmt.pad[4] += (target - bits);
                pp->outRgbFmt.bytes = target / 8;
                printf("%d bit(s) extra padding added to RGB output.\n",
                       target - bits);
            }

            if(pp->outRgbFmt.chan[0] > 8 || pp->outRgbFmt.chan[1] > 8 ||
               pp->outRgbFmt.chan[2] > 8 || pp->outRgbFmt.chan[3] > 8)
            {
                fprintf(stderr,
                        "Single RGB channel may not contain more than 8 bits.\n");
                ret = FALSE;
            }
        }

        offsAdjust = (pp->outRgbFmt.bytes & 1) ? 8 : 0; /* Use only 32/16 bits in padding calcs */

        for(i = 0; i < PP_MAX_CHANNELS; ++i)
        {
            u32 offs = 0;
            u32 mask = 0;

            for(j = 0; j < PP_MAX_CHANNELS; ++j)
            {
                offs += pp->outRgbFmt.pad[j];
                /* jth output channel is internal channel i */
                if(pp->outRgbFmt.order[j] == i)
                {
                    if(pp->outRgbFmt.chan[j])
                        mask = ((1 << pp->outRgbFmt.chan[j]) - 1);
                    pp->outRgbFmt.offs[i] = offs + offsAdjust;
                    pp->outRgbFmt.mask[i] =
                        (mask <<
                         (8 * pp->outRgbFmt.bytes -
                          pp->outRgbFmt.chan[j])) >> offs;
                    if(pp->outRgbFmt.bytes <= 2)    /* double mask for smaller values */
                        pp->outRgbFmt.mask[i] |= (pp->outRgbFmt.mask[i] << 16);
                }
                else
                {
                    offs += pp->outRgbFmt.chan[j];
                }
            }
        }
    }

    if(pp->inputStruct > PP_PIC_TOP_AND_BOT_FIELD_FRAME)
        ret = FALSE;

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : pp_load_cfg_file
    Description     :
    Return type     : int
    Argument        : char * cfgFile
    Argument        : PpParams *pp
------------------------------------------------------------------------------*/
int pp_load_cfg_file(char *cfgFile, ppContainer * pp)
{

    u32 i, tmp;
    PpParams *p;

    /* Parse configuration file... */
    printf("---parse config file...---\n");
    tmp = ParseConfig(cfgFile, ReadParam, (void *) pp);
    if(tmp == 0)
        return 1;

    pp->numPpParams = tmp;
    for(i = 0, p = pp->pp; i < tmp; i++, p++)
    {
        if(!CheckParams(p))
            return 1;

        /* Map color conversion parameters */
        MapColorConversion(&p->cnv);

        p->ppOrigWidth = (p->input.width + 15) / 16;
        p->ppOrigHeight = (p->input.height + 15) / 16;
        p->ppInWidth = p->zoom ? (p->zoom->width + 15) / 16 : p->ppOrigWidth;
        p->ppInHeight = p->zoom ? (p->zoom->height + 15) / 16 : p->ppOrigHeight;
    }

    /* Use default output file name if not specified. Output file name
     * decided based on first PpParams block, same file written even if
     * parts of the output are in RGB and others in YUV format */
    if(!*pp->pp->outputFile)
    {
        strcpy(pp->pp->outputFile,
               PP_IS_RGB(pp->pp->outputFormat) ?
               PP_DEFAULT_OUT_RGB : PP_DEFAULT_OUT_YUV);
    }
    for(i = 1; i < tmp; i++)
        strcpy(pp->pp[i].outputFile, pp->pp[0].outputFile);

    return 0;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: MapColorConversion

        Functional description:
          Map from "intuitive" values brightness, contrast and saturation
          into color conversion parameters a1, a2, b, ..., f, thr1, thr2.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void MapColorConversion(ColorConversion * cc)
{
    i32 th1y, th2y;
    i32 s;

    ASSERT(cc);

    /* Defaults for b,c,d,e parameters */
    if(cc->videoRange == 0)
    {   /* 16-235, 16-240 video range */
        if(cc->b == INT_MAX)
            cc->b = 403;
        if(cc->c == INT_MAX)
            cc->c = 120;
        if(cc->d == INT_MAX)
            cc->d = 48;
        if(cc->e == INT_MAX)
            cc->e = 475;
    }
    else
    {   /* 0-255 video range */
        if(cc->b == INT_MAX)
            cc->b = 409;
        if(cc->c == INT_MAX)
            cc->c = 208;
        if(cc->d == INT_MAX)
            cc->d = 100;
        if(cc->e == INT_MAX)
            cc->e = 516;
    }

    /* Perform mapping if one or more of the "intuitive" values is specified,
     * i.e. differs from INT_MAX. In this case, missing values (equal to
     * INT_MAX) are treated as zeroes. */
    if(cc->contrast != INT_MAX ||
       cc->brightness != INT_MAX ||
       cc->a != INT_MAX || cc->saturation != INT_MAX)
    {

        /* Use zero for missing values */
        if(cc->contrast == INT_MAX)
            cc->contrast = 0;
        if(cc->brightness == INT_MAX)
            cc->brightness = 0;
        if(cc->saturation == INT_MAX)
            cc->saturation = 0;
        if(cc->a == INT_MAX)
            cc->a = 298;

        s = 64 + cc->saturation;

        /* Map to color conversion parameters */

        if(cc->videoRange == 0)
        {   /* 16-235, 16-240 video range */

            i32 tmp1, tmp2;

            /* Contrast */
            if(cc->contrast == 0)
            {
                cc->thr1 = cc->thr2 = 0;
                cc->off1 = cc->off2 = 0;
                cc->a = cc->a1 = cc->a2 = 298;
            }
            else
            {

                cc->thr1 = (219 * (cc->contrast + 128)) / 512;
                th1y = (219 - 2 * cc->thr1) / 2;
                cc->thr2 = 219 - cc->thr1;
                th2y = 219 - th1y;

                tmp1 = (th1y * 256) / cc->thr1;
                tmp2 = ((th2y - th1y) * 256) / (cc->thr2 - cc->thr1);
                cc->off1 = ((th1y - ((tmp2 * cc->thr1) / 256)) * cc->a) / 256;
                cc->off2 = ((th2y - ((tmp1 * cc->thr2) / 256)) * cc->a) / 256;

                tmp1 = (64 * (cc->contrast + 128)) / 128;
                tmp2 = 256 * (128 - tmp1);
                cc->a1 = (tmp2 + cc->off2) / cc->thr1;
                cc->a2 =
                    cc->a1 + (256 * (cc->off2 - 1)) / (cc->thr2 - cc->thr1);

            }

            cc->b = SATURATE(0, 1023, (s * cc->b) >> 6);
            cc->c = SATURATE(0, 1023, (s * cc->c) >> 6);
            cc->d = SATURATE(0, 1023, (s * cc->d) >> 6);
            cc->e = SATURATE(0, 1023, (s * cc->e) >> 6);

            /* Brightness */
            cc->f = cc->brightness;

        }
        else
        {   /* 0-255 video range */

            /* Contrast */
            if(cc->contrast == 0)
            {
                cc->thr1 = cc->thr2 = 0;
                cc->off1 = cc->off2 = 0;
                cc->a = cc->a1 = cc->a2 = 256;
            }
            else
            {
                cc->thr1 = (64 * (cc->contrast + 128)) / 128;
                th1y = 128 - cc->thr1;
                cc->thr2 = 256 - cc->thr1;
                th2y = 256 - th1y;
                cc->a1 = (th1y * 256) / cc->thr1;
                cc->a2 = ((th2y - th1y) * 256) / (cc->thr2 - cc->thr1);
                cc->off1 = th1y - (cc->a2 * cc->thr1) / 256;
                cc->off2 = th2y - (cc->a1 * cc->thr2) / 256;
            }

            cc->b = SATURATE(0, 1023, (s * cc->b) >> 6);
            cc->c = SATURATE(0, 1023, (s * cc->c) >> 6);
            cc->d = SATURATE(0, 1023, (s * cc->d) >> 6);
            cc->e = SATURATE(0, 1023, (s * cc->e) >> 6);

            /* Brightness */
            cc->f = cc->brightness;
        }
    }
   
    /* Use zero for missing values */
    if(cc->contrast == INT_MAX)
        cc->contrast = 0;
    if(cc->brightness == INT_MAX)
        cc->brightness = 0;
    if(cc->saturation == INT_MAX)
        cc->saturation = 0;

    if(cc->a == INT_MAX)
    {
        if(cc->a1 != INT_MAX)
            cc->a = cc->a1;
        else if(cc->videoRange == 0)
            cc->a = cc->a1 = cc->a2 = 298;
        else
            cc->a = cc->a1 = cc->a2 = 256;
    }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: ReleasePp

        Functional description:
          Release memory alloc'd by PP descriptor

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void ReleasePp(PpParams * pp)
{
    u32 i;

    ASSERT(pp);
    /* Free pip block */
    if(pp->pip)
    {
        free(pp->pip);
    }
    /* Free pip block */
    if(pp->pipTmp)
    {
        free(pp->pipTmp);
    }
    /* Free pip YUV block */
    if(pp->pipYuv)
    {
        free(pp->pipYuv);
    }
    /* Free zoom block */
    if(pp->zoom)
    {
        free(pp->zoom);
    }
    /* Free masks */
    for(i = 0; i < pp->maskCount; ++i)
    {
        free(pp->mask[i]);
    }

    blend_size[0] = blend_size[1] = blendpos[0] = blendpos[1] = 0;

    /* Free filters */
    if(pp->firFilter)
    {
        free(pp->firFilter);
        pp->firFilter = NULL;
    }

}

/*------------------------------------------------------------------------------
    Function name   : WritePicture
    Description     :
    Return type     : int
    Argument        : u32 * image
    Argument        : i32 size
    Argument        : const char *fname
    Argument        : int frame
------------------------------------------------------------------------------*/
int WritePicture(u32 * image, i32 size, const char *fname, int frame)
{
    FILE *file = NULL;

    if(frame == 0)
        file = fopen(fname, "wb");
    else
        file = fopen(fname, "ab");

    if(file == NULL)
    {
        fprintf(stderr, "Unable to open output file: %s\n", fname);
        return -1;
    }

#ifdef MD5SUM
    TBWriteFrameMD5Sum(file, image, size, frame);
#else
    fwrite(image, 1, size, file);
#endif

    fclose(file);

    return 0;
}

/*------------------------------------------------------------------------------

    Function name:  PPTrace

    Purpose:
        Example implementation of PPTrace function. Prototype of this
        function is given in ppapi.h. This implementation appends
        trace messages to file named 'pp_api.trc'.

------------------------------------------------------------------------------*/
void PPTrace(const char *string)
{
    printf("%s", string);
}

#if 0
void PPTrace(const char *string)
{
    FILE *fp;

    fp = fopen("pp_api.trc", "at");

    if(!fp)
        return;

    fwrite(string, 1, strlen(string), fp);
    fwrite("\n", 1, 1, fp);

    fclose(fp);
}
#endif
/*------------------------------------------------------------------------------

   <++>.<++>  Function: WriteBmp

        Functional description:
          Write raw RGB data into BMP file.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/

#ifdef PP_WRITE_BMP

struct BITMAPINFOHEADER
{
    u32 biSize;
    u32 biWidth;
    u32 biHeight;
    u16 biPlanes;
    u16 biBitCount;
    u32 biCompression;
    u32 biSizeImage;
    u32 biXPelsPerMeter;
    u32 biYPelsPerMeter;
    u32 biClrUsed;
    u32 biClrImportant;
} __attribute__ ((__packed__));

struct BITMAPFILEHEADER
{
    u16 bfType;
    u32 bfSize;
    u16 bfReserved1;
    u16 bfReserved2;
    u32 bfOffBits;
} __attribute__ ((__packed__));

typedef struct BITMAPFILEHEADER BITMAPFILEHEADER_t;
typedef struct BITMAPINFOHEADER BITMAPINFOHEADER_t;

#define BI_RGB        0L
#define BI_BITFIELDS  3L

i32 WriteBmp(const char *filename, u32 pix_fmt, u8 * rgb, u32 w, u32 h)
{
    FILE *fp;
    u32 rgb_size;
    u32 mask;
    u32 bpp;

    BITMAPFILEHEADER_t bmpfh;
    BITMAPINFOHEADER_t bmpih;

    fp = fopen(filename, "wb");
    if(!fp)
    {
        fprintf(stderr, "Error: Unable to open file: %s\n", filename);
        return -1;
    }

    if(pix_fmt & 0x01000)
    {
        rgb_size = w * h * 4;
        bpp = 32;
    }
    else
    {
        rgb_size = w * h * 2;
        bpp = 16;
    }

    bmpfh.bfType = 'MB';
    bmpfh.bfSize =
        sizeof(BITMAPFILEHEADER_t) + sizeof(BITMAPINFOHEADER_t) + 3 * 4 +
        rgb_size;
    bmpfh.bfReserved1 = 0;
    bmpfh.bfReserved2 = 0;
    bmpfh.bfOffBits =
        sizeof(BITMAPFILEHEADER_t) + sizeof(BITMAPINFOHEADER_t) + 3 * 4;

    bmpih.biSize = sizeof(BITMAPINFOHEADER_t);
    bmpih.biWidth = w;
    bmpih.biHeight = h;
    bmpih.biPlanes = 1;
    bmpih.biBitCount = bpp;
    bmpih.biCompression = /* BI_RGB */ BI_BITFIELDS;
    bmpih.biSizeImage = rgb_size;
    bmpih.biXPelsPerMeter = 0;
    bmpih.biYPelsPerMeter = 0;
    bmpih.biClrUsed = 0;
    bmpih.biClrImportant = 0;

    /* write BMP header */

    fwrite(&bmpfh, sizeof(BITMAPFILEHEADER_t), 1, fp);
    fwrite(&bmpih, sizeof(BITMAPINFOHEADER_t), 1, fp);

    if(pix_fmt & 0x01000)
    {
        mask = 0x00FF0000;  /* R */
        mask = pp.currPp->outRgbFmt.mask[0];
        fwrite(&mask, 4, 1, fp);

        mask = 0x0000FF00;  /* G */
        mask = pp.currPp->outRgbFmt.mask[1];
        fwrite(&mask, 4, 1, fp);

        mask = 0x000000FF;  /* B */
        mask = pp.currPp->outRgbFmt.mask[2];
        fwrite(&mask, 4, 1, fp);
    }
    else
    {
        mask = 0x0000F800;  /* R */

        mask = pp.currPp->outRgbFmt.mask[0] >> 16;
        fwrite(&mask, 4, 1, fp);

        printf("maskR %08x\n", mask);

        mask = 0x000007E0;  /* G */

        mask = pp.currPp->outRgbFmt.mask[1] >> 16;;
        printf("maskG %08x\n", mask);
        fwrite(&mask, 4, 1, fp);

        mask = 0x0000001F;  /* B */

        mask = pp.currPp->outRgbFmt.mask[2] >> 16;
        printf("maskB %08x\n", mask);
        fwrite(&mask, 4, 1, fp);

    }

#if 0
    {   /* vertical flip bitmap data */
        i32 x, y;
        u8 *img = (u8 *) rgb;

        for(y = 0; y < h; y++)
        {
            for(x = 0; x < w; x++)
            {
                u8 tmp = *img;

                *img = *(img + 1);
                *(img + 1) = tmp;
                img += 2;
            }

        }
    }
#endif

    fwrite(rgb, rgb_size, 1, fp);

    fclose(fp);

    return 0;
}
#endif

/*------------------------------------------------------------------------------

   <++>.<++>  Function: MakeTiled

        Functional description:
          Perform conversion YUV420C to TILED YUV420.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void MakeTiled(u8 * data, u32 w, u32 h)
{
    u32 i, j;
    u8 *orig = data;
    u8 *tmp_img = (u8 *) calloc((w * h * 3) / 2, 1);
    u8 *pt;

    printf("Performing conversion YUV420C to TILED YUV420!\n");

    pt = tmp_img;

    for(i = 0; i < h / 16; i++)
    {
        for(j = 0; j < w / 16; j++)
        {
            u32 z;
            u8 *pRead = data + (i * 16 * w) + (j * 16);

            for(z = 0; z < 16; z++)
            {
                memcpy(pt, pRead, 16);
                pt += 16;
                pRead += w;
            }
        }
    }

    data += w * h;

    for(i = 0; i < h / 16; i++)
    {
        for(j = 0; j < w / 16; j++)
        {
            u32 z;
            u8 *pRead = data + (i * 8 * w) + j * 16;

            for(z = 0; z < 8; z++)
            {
                memcpy(pt, pRead, 16);
                pt += 16;
                pRead += w;
            }
        }
    }

    memcpy(orig, tmp_img, (w * h * 3) / 2);
    free(tmp_img);
}


/*------------------------------------------------------------------------------

   <++>.<++>  Function: MakeTiled

        Functional description:
          Perform conversion YUV420C to TILED YUV420.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void MakeTiled8x4(u8 * data, u32 w, u32 h, u32 planarToSemiplanar)
{
    u32 i, j;
    u8 *orig = data;
    u8 *tmp_img = (u8 *) calloc((w * h * 3) / 2, 1);
    u8 *pt;
    u32 tilesW;
    u32 tilesH;
    u32 skip;
    u32 *pIn0 = (u32*)(data),
        *pIn1 = (u32*)(data +   w),
        *pIn2 = (u32*)(data + 2*w),
        *pIn3 = (u32*)(data + 3*w);
    u32 *outp = (u32*)tmp_img;
   
    tilesW = w/8;
    tilesH = h/4;
    skip = w-w/4;

    for( i = 0 ; i < tilesH ; ++i)
    {
        for( j = 0 ; j < tilesW ; ++j )
        {
            *outp++ = *pIn0++;
            *outp++ = *pIn0++;
            *outp++ = *pIn1++;
            *outp++ = *pIn1++;
            *outp++ = *pIn2++;
            *outp++ = *pIn2++;
            *outp++ = *pIn3++;
            *outp++ = *pIn3++;
        }

        pIn0 += skip;
        pIn1 += skip;
        pIn2 += skip;
        pIn3 += skip;
    }

    if(planarToSemiplanar)
    {
        u8 * pCb, * pCr;
        u8 * pOutU8 = (u8*)outp;
        pCb = data + w*h;
        pCr = pCb + ((w*h)/4);
        u32 r;
        u32 wc = w/2; /* chroma width in pels */

        printf("Convert input data from PLANAR to SEMIPLANAR\n");

        for( i = 0 ; i < tilesH/2 ; ++i)
        {
            r = 0;
            for( j = 0 ; j < tilesW ; ++j )
            {
                *pOutU8++ = pCb[r+0]; *pOutU8++ = pCr[r+0];
                *pOutU8++ = pCb[r+1]; *pOutU8++ = pCr[r+1];
                *pOutU8++ = pCb[r+2]; *pOutU8++ = pCr[r+2];
                *pOutU8++ = pCb[r+3]; *pOutU8++ = pCr[r+3];

                *pOutU8++ = pCb[wc+r+0]; *pOutU8++ = pCr[wc+r+0];
                *pOutU8++ = pCb[wc+r+1]; *pOutU8++ = pCr[wc+r+1];
                *pOutU8++ = pCb[wc+r+2]; *pOutU8++ = pCr[wc+r+2];
                *pOutU8++ = pCb[wc+r+3]; *pOutU8++ = pCr[wc+r+3];

                *pOutU8++ = pCb[2*wc+r+0]; *pOutU8++ = pCr[2*wc+r+0];
                *pOutU8++ = pCb[2*wc+r+1]; *pOutU8++ = pCr[2*wc+r+1];
                *pOutU8++ = pCb[2*wc+r+2]; *pOutU8++ = pCr[2*wc+r+2];
                *pOutU8++ = pCb[2*wc+r+3]; *pOutU8++ = pCr[2*wc+r+3];

                *pOutU8++ = pCb[3*wc+r+0]; *pOutU8++ = pCr[3*wc+r+0];
                *pOutU8++ = pCb[3*wc+r+1]; *pOutU8++ = pCr[3*wc+r+1];
                *pOutU8++ = pCb[3*wc+r+2]; *pOutU8++ = pCr[3*wc+r+2];
                *pOutU8++ = pCb[3*wc+r+3]; *pOutU8++ = pCr[3*wc+r+3];

                r += 4;
            }

            pCb += wc*4;
            pCr += wc*4;
        }
    }
    else
    {
        for( i = 0 ; i < tilesH/2 ; ++i)
        {
            for( j = 0 ; j < tilesW ; ++j )
            {
                *outp++ = *pIn0++;
                *outp++ = *pIn0++;
                *outp++ = *pIn1++;
                *outp++ = *pIn1++;
                *outp++ = *pIn2++;
                *outp++ = *pIn2++;
                *outp++ = *pIn3++;
                *outp++ = *pIn3++;
            }

            pIn0 += skip;
            pIn1 += skip;
            pIn2 += skip;
            pIn3 += skip;
        }
    }

    memcpy(orig, tmp_img, (w * h * 3) / 2);
    free(tmp_img);
}


/*------------------------------------------------------------------------------

   <++>.<++>  Function: Tiled4x4ToPlanar

        Functional description:
          Perform conversion from 4:2:2 TILED 4x4 to 4:2:2 INTERLEAVED

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void Tiled4x4ToPlanar(u8 * data, u32 w, u32 h)
{
    u32 i, j;
    u8 *orig = data;
    u8 *tmp_img = (u8 *) calloc((w * ((h+3)&~3) * 2), 1);
    u32 *pt = NULL;
    u32 *pRead = NULL;
    u32 *pWrite = NULL;
    u32 stride = 0;

#if 0 
    static u32 header = 0;
    static u32 frameNum = 0;
    static FILE * trcFile = NULL;
#endif

    printf("Performing conversion 4:2:2 TILED 4x4 to 4:2:2 INTERLEAVED!\n");

#if 0 
    if( header == 0 )
    {
        header = 1;
        trcFile = fopen("tiled_to_planar.trc", "wt");
    }
#endif

    pt = (u32*)tmp_img;

    pRead = (u32*)data;

    stride = w/2; /* one row in 32-bit addresses */

#if 0 
    fprintf(trcFile, "--picture=%d\n", frameNum++);
    fprintf(trcFile, "  stride = %d\n", stride);
#endif

    for(i = 0; i < h ; i+=4 )
    {
        u32 wOffset;
        /* Set write pointer to correct 4x4 block row */
        pWrite = pt + i*stride;

        wOffset = i*stride;

#if 0
        fprintf(trcFile, "  tile row %d\n", i/4 );
        fprintf(trcFile, "  w offset %d words\n", i*stride);
        fprintf(trcFile, "      ptr %p\n", pWrite );
#endif

        for(j = 0; j < w ; j+=4 )
        {
#if 0
            fprintf(trcFile, "    tile nbr %d\n", j/4 );
            fprintf(trcFile, "      read ptr %p\n", pRead );
            fprintf(trcFile, "                    %8.8x %8.8x %8.8x %8.8x   %8.8x %8.8x %8.8x %8.8x\n",
                pRead[0],
                pRead[1],
                pRead[2],
                pRead[3],
                pRead[4],
                pRead[5],
                pRead[6],
                pRead[7] );

            fprintf(trcFile, "    write to offset %8d %8d %8d %8d   %8d %8d %8d %8d\n",
                wOffset+0,
                wOffset+1,
                wOffset+stride+0,
                wOffset+stride+1,
                wOffset+stride*2+0,
                wOffset+stride*2+1,
                wOffset+stride*3+0,
                wOffset+stride*3+1 );
#endif

            /* left pixel group */
            pWrite[0]           = pRead[0];
            pWrite[stride]      = pRead[2];
            pWrite[2*stride]    = pRead[4];
            pWrite[3*stride]    = pRead[6];

            /* right pixel group */
            if( j + 2 < w )
            {
                pWrite[1]           = pRead[1];
                pWrite[stride+1]    = pRead[3];
                pWrite[2*stride+1]  = pRead[5];
                pWrite[3*stride+1]  = pRead[7];
            }

            pRead += 8; /* skip to next block */
            pWrite += 2;

#if 0
            wOffset += 2;
#endif
        }
    }

    memcpy(orig, tmp_img, (w * ((h+3)&~3) * 2) );
    free(tmp_img);
}


/*------------------------------------------------------------------------------

   <++>.<++>  Function: pipeline_disable

        Functional description:
          Disable the pipeline.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/

void pipeline_disable(void)
{

    PPDecCombinedModeDisable(ppInst, decoder);
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: swap_in_out_pointers

        Functional description:
          Swap input and output pointers if external mode used
          during pipeline decoding (for rotation, cropping)

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
static void swap_in_out_pointers(u32 width, u32 height)
{

    u32 tmpbus;

    tmpbus = ppConf.ppInImg.bufferBusAddr;

    ppConf.ppInImg.bufferBusAddr = ppConf.ppOutImg.bufferBusAddr;

    ppConf.ppInImg.bufferCbBusAddr =
        ppConf.ppInImg.bufferBusAddr + width * height;

    ppConf.ppInImg.bufferCrBusAddr =
        ppConf.ppInImg.bufferCbBusAddr + (width * height) / 4;

    ppConf.ppOutImg.bufferBusAddr = tmpbus;

    ppConf.ppOutImg.bufferChromaBusAddr =
        ppConf.ppOutImg.bufferBusAddr +
        ppConf.ppOutImg.width * ppConf.ppOutImg.height;

}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: pp_vc1_specific

        Functional description:
          Set the VC-1 specific flags

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void pp_vc1_specific(u32 multiresEnable, u32 rangered)
{
    vc1Multires = multiresEnable;
    vc1Rangered = rangered;

}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: reset_vc1_specific

        Functional description:
          Initialize the VC-1 specific flags

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
static void reset_vc1_specific(void)
{
    vc1Multires = 0;
    vc1Rangered = 0;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: pp_change_resolution

        Functional description:
            Updates pp input frame resolution in multiresolution sequences

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
int pp_change_resolution(u32 width, u32 height, const TBCfg * tbCfg)
{
    pp.currPp->input.width = width;
    pp.currPp->input.height = height;

    /* Override selected PP features */
    TBOverridePpContainer((PPContainer *)ppInst, tbCfg );

    if(pp_api_cfg(ppInst, decoder, decodertype, pp.currPp, tbCfg) != 0)
    {
        printf("PP CHANGE RESOLUTION FAILED\n");
        return CFG_UPDATE_FAIL;
    }

    return 0;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: Resample

        Functional description:
          Resample channels of frame.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void Resample(PixelData * p, PixelFormat fmtNew)
{
    i32 x, y;

    ASSERT(p);

    /* Check if no resampling required */
    if(p->fmt == fmtNew)
        return;

    if(p->fmt == PP_INTERNAL_FMT(fmtNew))
    {
        p->fmt = fmtNew;
        return;
    }

    /* YUV 4:0:0 --> YUV 4:4:4 */
    if(p->fmt == YUV400 && fmtNew == YUV444)
    {
        for(y = 0; y < p->height; ++y)
        {
            for(x = 0; x < p->width; ++x)
            {
                p->yuv.Cb[y][x] = 128;
                p->yuv.Cr[y][x] = 128;
            }
        }
    }
    /* YUV 4:2:0 --> YUV 4:4:4 */
    else if(p->fmt == YUV420 && fmtNew == YUV444)
    {
        for(y = p->height - 1; y >= 0; --y)
        {
            for(x = p->width - 1; x >= 0; --x)
            {
                p->yuv.Cb[y][x] = p->yuv.Cb[y >> 1][x >> 1];
                p->yuv.Cr[y][x] = p->yuv.Cr[y >> 1][x >> 1];
            }
        }
    }
    /* YUV 4:2:2 --> YUV 4:4:4 */
    else if(p->fmt == YUV422 && fmtNew == YUV444)
    {
        for(y = 0; y < p->height; ++y)
        {
            for(x = p->width - 1; x >= 0; --x)
            {
                p->yuv.Cb[y][x] = p->yuv.Cb[y][x >> 1];
                p->yuv.Cr[y][x] = p->yuv.Cr[y][x >> 1];
            }
        }
    }
    /* YUV 4:4:4 --> YUV 4:2:0 */
    else if(p->fmt == YUV444 && PP_INTERNAL_FMT(fmtNew) == YUV420)
    {
        for(y = 0; y < p->height; y += 2)
        {
            for(x = 0; x < p->width; x += 2)
            {
                p->yuv.Cb[y >> 1][x >> 1] = p->yuv.Cb[y][x];
                p->yuv.Cr[y >> 1][x >> 1] = p->yuv.Cr[y][x];
            }
        }
    }
    /* YUV 4:4:4 --> YUV 4:0:0 */
    else if(p->fmt == YUV444 && PP_INTERNAL_FMT(fmtNew) == YUV400)
    {
        for(y = 0; y < p->height; y++)
        {
            for(x = 0; x < p->width; x++)
            {
                p->yuv.Cb[y][x] = 128;
                p->yuv.Cr[y][x] = 128;
            }
        }
    }
    /* YUV 4:4:4 --> YUV 4:2:2 */
    else if(p->fmt == YUV444 && PP_INTERNAL_FMT(fmtNew) == YUV422)
    {
        for(y = 0; y < p->height; y++)
        {
            for(x = 0; x < p->width; x += 2)
            {
                p->yuv.Cb[y][x >> 1] = p->yuv.Cb[y][x];
                p->yuv.Cr[y][x >> 1] = p->yuv.Cr[y][x];
            }
        }
    }
    /* Unsupported resampling */
    else
    {
        ASSERT(0);
    }

    p->fmt = fmtNew;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: pp_alloc_blend_components

        Functional description:
          alloc mem for blending components

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/

int pp_alloc_blend_components(PpParams * pp, u32 ablendCrop)
{
#ifdef __arm__
    MemallocParams params;
#endif
    Overlay *blend1 = NULL;
    Overlay *blend0 = &pp->mask[0]->overlay;

    blend1 = &pp->mask[1]->overlay;

    if(pp->mask[0] != NULL)
        if(blend0 != NULL)
            if(*(blend0->fileName))
            {
                blend0->enabled = 1;

                /* larger buffer required if cropping supported */
                if(ablendCrop)
                    blend_size[0] = blend0->data.width * blend0->data.height * 4;
                else
                    blend_size[0] = (pp->mask[0]->x1 - pp->mask[0]->x0)
                        * (pp->mask[0]->y1 - pp->mask[0]->y0) * 4;

                blendTmp[0] =
                    malloc(blend0->data.width * blend0->data.height * 3 / 2);
                blendTmp444[0] =
                    malloc(blend0->data.width * blend0->data.height * 4);
#ifndef __arm__
                blend_va[0] = malloc(blend_size[0]);
                blend_ba[0] = (u32) blend_va[0];
                if(blend_va[0] == NULL)
                    goto blend_err;
#else

                /* input buffer allocation */

                params.size = blend_size[0];

                /* Allocate buffer and get the bus address */
                ioctl(memdev_fd, MEMALLOC_IOCXGETBUFFER, &params);

                if(params.busAddress == 0)
                {
                    printf("Input allocation failed\n");
                    goto blend_err;
                }
                blend_ba[0] = params.busAddress;
                /* Map the bus address to virtual address */
                blend_va[0] = MAP_FAILED;
                blend_va[0] =
                    (u32 *) mmap(0, blend_size[0], PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd_mem, blend_ba[0]);

                if(blend_va[0] == MAP_FAILED)
                {
                    printf("Failed to alloc input image\n");
                    goto blend_err;
                }
#endif

#ifdef ASIC_TRACE_SUPPORT
                ablendBase[0] = (u8*)blend_va[0];
                if(ablendCrop)
                {
                    ablendWidth[0] = blend0->data.width;
                    ablendHeight[0] = blend0->data.height;
                }
                else
                {
                    ablendWidth[0] = pp->mask[0]->x1 - pp->mask[0]->x0;
                    ablendHeight[0] = pp->mask[0]->y1 - pp->mask[0]->y0;
                }
#endif

            }

    if(pp->mask[1] != NULL)
        if(blend1 != NULL)
            if(*(blend1->fileName))
            {

                blend1->enabled = 1;

                /* larger buffer required if cropping supported */
                if(ablendCrop)
                    blend_size[1] = blend1->data.width * blend1->data.height * 4;
                else
                    blend_size[1] = (pp->mask[1]->x1 - pp->mask[1]->x0)
                        * (pp->mask[1]->y1 - pp->mask[1]->y0) * 4;

                blendTmp[1] =
                    malloc(blend1->data.width * blend1->data.height * 3 / 2);
                blendTmp444[1] =
                    malloc(blend1->data.width * blend1->data.height * 4);
#ifndef __arm__
                blend_va[1] = malloc(blend_size[1]);
                blend_ba[1] = (u32) blend_va[1];
                if(blend_va[1] == NULL)
                    goto blend_err;

#else
                /* input buffer allocation */
                params.size = blend_size[1];

                /* Allocate buffer and get the bus address */
                ioctl(memdev_fd, MEMALLOC_IOCXGETBUFFER, &params);

                if(params.busAddress == 0)
                {
                    printf("Input allocation failed\n");
                    goto blend_err;
                }
                blend_ba[1] = params.busAddress;
                /* Map the bus address to virtual address */
                blend_va[1] = MAP_FAILED;
                blend_va[1] =
                    (u32 *) mmap(0, blend_size[1], PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd_mem, blend_ba[1]);

                if(blend_va[1] == MAP_FAILED)
                {
                    printf("Failed to alloc input image\n");
                    goto blend_err;
                }
#endif

#ifdef ASIC_TRACE_SUPPORT
                ablendBase[1] = (u8*)blend_va[1];
                if(ablendCrop)
                {
                    ablendWidth[1] = blend1->data.width;
                    ablendHeight[1] = blend1->data.height;
                }
                else
                {
                    ablendWidth[1] = pp->mask[1]->x1 - pp->mask[1]->x0;
                    ablendHeight[1] = pp->mask[1]->y1 - pp->mask[1]->y0;
                }
#endif

            }

    return 0;
  blend_err:
    return 1;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: read_mask_file

        Functional description:
          read blend component from file

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/

int read_mask_file(u32 id, const u32 ablendCropSupport)
{
    u32 i, j, tmp;
    u32 topSkip, leftSkip, rightSkip;
    u32 height, width;
    u32 size;
    u8 *sY, *sCb, *sCr, *d;
    u32 *s32, *d32;
    u32 alpha = 0;
    Area *mask = pp.currPp->mask[id];
    FILE *fid;

    if(!mask)
        return 0;

    fid = fopen(blendfile[id], "r");
    if(fid == NULL)
    {
        printf("    %s\n", blendfile[id]);
        return 1;
    }

    fseek(fid, blendpos[id], SEEK_SET);
    if(mask->overlay.pixelFormat == YUV420)
    {
        size = mask->overlay.data.width * mask->overlay.data.height * 3 / 2;
        fread(blendTmp[id], size, 1, fid);
    }
    else
    {
        ASSERT(mask->overlay.pixelFormat == AYCBCR);
        size = mask->overlay.data.width * mask->overlay.data.height * 4;
        fread(blendTmp444[id], size, 1, fid);
    }
    blendpos[id] += size;
    fclose(fid);

    /* resample to AYCbCr */
    if(mask->overlay.pixelFormat == YUV420)
    {
        if(mask->overlay.alphaSource >= ALPHA_CONSTANT)
            alpha = mask->overlay.alphaSource - ALPHA_CONSTANT;

        size = size * 2 / 3;
        sY = (u8 *) blendTmp[id];
        sCb = sY + size;
        sCr = sCb + size / 4;
        d = (u8 *) blendTmp444[id];

        width = mask->overlay.data.width;
        height = mask->overlay.data.height;
        if(mask->overlay.pixelFormat == YUV420)
        {
            for(i = 0; i < height; i++)
            {
                for(j = 0; j < width; j++)
                {
                    *d++ = alpha;
                    *d++ = sY[i * width + j];
                    *d++ = sCb[(i / 2) * width / 2 + j / 2];
                    *d++ = sCr[(i / 2) * width / 2 + j / 2];
                }
            }
        }
    }

    /* color conversion if needed */
    if(PP_IS_RGB(pp.currPp->outputFormat))
    {
        Yuv2Rgb(blendTmp444[id], mask->overlay.data.width,
                mask->overlay.data.height, &pp.currPp->cnv);
    }

    width = mask->x1 - mask->x0;
    height = mask->y1 - mask->y0;

    s32 = blendTmp444[id];
    d32 = blend_va[id];

    if(!ablendCropSupport)
    {
        topSkip = mask->overlay.data.width * mask->overlay.y0;
        leftSkip = mask->overlay.x0;
        rightSkip = mask->overlay.data.width - width - leftSkip;
    }
    else
    {
        topSkip = leftSkip = rightSkip = 0;
        width = mask->overlay.data.width;
        height = mask->overlay.data.height;
    }

    s32 += topSkip + leftSkip;
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            if(mask->overlay.alphaSource == ALPHA_SEQUENTIAL)
            {
                *d32 = *s32++;
                *((u8 *) d32) = alpha;
                alpha = (alpha + 1) & 0xFF;
                d32++;
            }
            else
                *d32++ = *s32++;
        }
        s32 += rightSkip + leftSkip;
    }

#ifndef ASIC_TRACE_SUPPORT
    if(blend_endian_change)
    {
        for(i = 0; i < (blend_size[id] / 4); i++)
        {
            tmp = blend_va[id][i];
            blend_va[id][i] = ((tmp & 0x000000FF) << 24) |
                ((tmp & 0x0000FF00) << 8) |
                ((tmp & 0x00FF0000) >> 8) | ((tmp & 0xFF000000) >> 24);
        }
    }
#endif

    return 0;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: pp_read_blend_components

        Functional description:
          read blend component from file

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/

int pp_read_blend_components(const void *ppInst)
{
    i32 ret = 0;

    ret = read_mask_file(0, ((PPContainer *) ppInst)->blendCropSupport);
    ret |= read_mask_file(1, ((PPContainer *) ppInst)->blendCropSupport);

    return ret;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: toggle_endian_yuv

        Functional description:
            Changes the endianness of a yuv image

        Inputs: width, height, virtual pointer

        Outputs:

------------------------------------------------------------------------------*/
static void toggle_endian_yuv(u32 width, u32 height, u32 * virtual)
{
    u32 i, tmp;

    printf("toggle_endian\n");

    ASSERT(virtual != NULL);
    for(i = 0; i < ((width * height * 3) / 2) / 4; i++) /* the input is YUV,
                                                         * hence the size */
    {

        tmp = virtual[i];
        virtual[i] = ((tmp & 0x000000FF) << 24) |
            ((tmp & 0x0000FF00) << 8) |
            ((tmp & 0x00FF0000) >> 8) | ((tmp & 0xFF000000) >> 24);
    }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: Yuv2Rgb

        Functional description:
            Color conversion for alpha blending component

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void Yuv2Rgb(u32 * p, u32 w, u32 h, ColorConversion * cc)
{
    u32 x, y;
    u32 tmp;
    i32 Y, Cb, Cr;
    i32 R, G, B;
    i32 a, off;
    i32 adj = 0;
    u8 *pComp;

    ASSERT(p);
    ASSERT(cc);

    pComp = (u8 *) p;
    /* Color conversion */
    for(y = 0; y < h; y++)
    {
        for(x = 0; x < w; x++)
        {
            pComp++;
            Y = pComp[0];
            Cb = pComp[1];
            Cr = pComp[2];

            /* Range is normalized to start from 0 */
            if(cc->videoRange == 0)
            {
                Y = Y - 16;
                Y = MAX(Y, 0);
                adj = 0;
            }

            if(Y < cc->thr1)
            {
                a = cc->a1;
                off = 0;
            }
            else if(Y <= cc->thr2)
            {
                a = cc->a2;
                off = cc->off1;
            }
            else
            {
                a = cc->a1;
                off = cc->off2;
            }

            R = (((a * Y) + (cc->b * (Cr - 128))) >> 8) + cc->f + off;

            G = (((a * Y) - (cc->c * (Cr - 128)) -
                  (cc->d * (Cb - 128))) >> 8) + cc->f + off;

            B = (((a * Y) + (cc->e * (Cb - 128))) >> 8) + cc->f + off;

            R = MIN(MAX(0, R), 255);
            G = MIN(MAX(0, G), 255);
            B = MIN(MAX(0, B), 255);

            *pComp++ = R;
            *pComp++ = G;
            *pComp++ = B;
        }
    }
}

/*------------------------------------------------------------------------------
    Function name   : pp_update_config
    Description     : Prepare the post processor according to the post processor
                      configuration file. Allocate input and output picture
                      buffers.
    Return type     : int
    Argument        : const void *decInst
    Argument        : u32 decType
------------------------------------------------------------------------------*/
#ifdef ASIC_TRACE_SUPPORT
extern u8 *pDisplayY;
extern u8 *pDisplayC;
extern u32 displayHeight;
extern i32 leftOffset;
extern i32 topOffset;
extern u32 inputOrigHeight;
#endif

int pp_update_config(const void *decInst, u32 decType, const TBCfg * tbCfg)
{
    u32 retVal = CFG_UPDATE_OK;
    static i32 frame = 0;

    if(pp.currPp == NULL ||
       (++frame == pp.currPp->frames && pp.ppParamIdx < pp.numPpParams - 1))
    {
        if(!pp.currPp)
        {
            pp.currPp = pp.pp;
            pp.ppParamIdx = 0;
        }
        else
        {
            pp_release_buffers(pp.currPp);
            pp.ppParamIdx++;
            pp.currPp++;
        }

        frame = 0;

    if(forceTiledInput &&
       pp.currPp->inputFormat == YUV420 )
    {
        planarToSemiplanarInput = 1;
        pp.currPp->inputFormat =
            pp.currPp->input.fmt = YUV420C;
    }
	
	/* Check multibuffer status before buffer allocation */
	multibuffer = TBGetTBMultiBuffer(tbCfg);
    if(multibuffer && 
       (decType == PP_PIPELINED_DEC_TYPE_VP6 ||
        decType == PP_PIPELINED_DEC_TYPE_VP8 ||
        decType == PP_PIPELINED_DEC_TYPE_WEBP ||
        decType == PP_PIPELINE_DISABLED))
    {
        multibuffer = 0;
    }

        printf("Multibuffer %s\n", multibuffer ? "ENABLED" : "DISABLED");

        printf("---alloc frames---\n");

        if(pp_alloc_buffers(pp.currPp,
            ((PPContainer *) ppInst)->blendCropSupport) != 0)
        {
            printf("\t\tFAILED\n");
            return CFG_UPDATE_FAIL;
        }

#ifndef PP_PIPELINE_ENABLED
        pp.currPp->input.base = (u8 *) in_pic_va;

        AllocFrame(&pp.currPp->input);
#endif
        pp.currPp->output.base = (u8 *) out_pic_va;

        AllocFrame(&pp.currPp->output);

        printf("---config PP---\n");

        /* Override selected PP features */
        TBOverridePpContainer((PPContainer *)ppInst, tbCfg);

        if(pp_api_cfg(ppInst, decInst, decType, pp.currPp, tbCfg) != 0)
        {
            printf("\t\tFAILED\n");
            return CFG_UPDATE_FAIL;
        }

        /* Print model parameters */
        printf("input image: %d %d\n", pp.currPp->input.width,
               pp.currPp->input.height);
        printf("output image: %d %d\n", pp.currPp->output.width,
               pp.currPp->output.height);
        if(pp.currPp->zoom)
        {
            printf("zoom enabled, input area size %d %d\n",
                   pp.currPp->zoom->width - pp.currPp->crop8Right * 8,
                   pp.currPp->zoom->height - pp.currPp->crop8Down * 8);
            printf("scaling factors X %.2f  Y %.2f\n",
                   (float) pp.currPp->output.width /
                   (float) (pp.currPp->rotation ? pp.currPp->zoom->height -
                            pp.currPp->crop8Down * 8 : pp.currPp->zoom->width -
                            pp.currPp->crop8Right * 8),
                   (float) pp.currPp->output.height /
                   (float) (pp.currPp->rotation ? pp.currPp->zoom->width -
                            pp.currPp->crop8Right *
                            8 : pp.currPp->zoom->height -
                            pp.currPp->crop8Down * 8));
        }
        else
        {
            printf("input area size %d %d\n",
                   pp.currPp->output.width - pp.currPp->crop8Right * 8,
                   pp.currPp->output.height - pp.currPp->crop8Down * 8);
            printf("scaling factors X %.2f  Y %.2f\n",
                   (float) pp.currPp->output.width /
                   (float) (pp.currPp->rotation ? pp.currPp->input.height -
                            pp.currPp->crop8Down * 8 : pp.currPp->input.width -
                            pp.currPp->crop8Right * 8),
                   (float) pp.currPp->output.height /
                   (float) (pp.currPp->rotation ? pp.currPp->input.width -
                            pp.currPp->crop8Right *
                            8 : pp.currPp->input.height -
                            pp.currPp->crop8Down * 8));
        }

        if(PP_IS_RGB(pp.currPp->outputFormat))
        {
            if(PP_IS_INTERLEAVED(pp.currPp->outputFormat))
            {
                printf("RGB output width: %d bytes per pel.\n",
                       pp.currPp->outRgbFmt.bytes);
            }
            else
            {
                printf("use planar RGB.\n");
            }
        }
        if(pp.currPp->pip)
        {
            printf("PiP enabled, frame size %d %d.\n", pp.currPp->pip->width,
                   pp.currPp->pip->height);
        }
        if(pp.currPp->firFilter)
        {
            printf("FIR filter enabled.\n");
        }
        if(pp.currPp->pipeline)
        {
            printf("pipeline enabled.\n");
        }
        if(pp.currPp->filterEnabled)
        {
            printf("filter enabled.\n");
        }

        /* config updated */
        retVal = CFG_UPDATE_NEW;
    }
    /* stand-alone test -> stop if all configs handled */
    else if(frame == pp.currPp->frames && decInst == NULL)
        return CFG_UPDATE_FAIL;

#ifdef ASIC_TRACE_SUPPORT
    pDisplayY = (u8 *) out_pic_va;
    pDisplayC = pDisplayY +
        (pp.currPp->pip ? pp.currPp->pip->width : ppConf.ppOutImg.width) *
        (pp.currPp->pip ? pp.currPp->pip->height : ppConf.ppOutImg.height);
    displayHeight = pp.currPp->pip ? pp.currPp->pip->height :
        ppConf.ppOutImg.height;
    inputOrigHeight = pp.currPp->input.height / 16;

    /* Fix original input height for vertical downscale shortcut */
    {
        PPContainer *ppc = (PPContainer *)ppInst;
        if(ppc->fastVerticalDownscale &&
           ( ppConf.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_PLANAR ||
             ppConf.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR ||
             ppConf.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_TILED ||
             ppConf.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_0_0 ))         
        {
            inputOrigHeight = (inputOrigHeight+1)/2;
        }
    }

    leftOffset = pp.currPp->pipx0;
    topOffset = pp.currPp->pipy0;
#endif

    return retVal;

}

/*------------------------------------------------------------------------------
    Function name   : pp_set_input_interlaced
    Description     : decoder sets up interlaced input type
    Return type     : void
    Argument        : is interlaced or not
------------------------------------------------------------------------------*/
void pp_set_input_interlaced(u32 isInterlaced)
{
    interlaced = isInterlaced ? 1 : 0;
}

/*------------------------------------------------------------------------------
    Function name   : pp_check_combined_status
    Description     : Check return value of post-processing
    Return type     : void
    Argument        : is interlaced or not
------------------------------------------------------------------------------*/
PPResult pp_check_combined_status(void)
{
    PPResult ret;

    ret = PPGetResult(ppInst);
    pp_print_result(ret);
    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : pp_set_input_interlaced
    Description     : decoder sets up interlaced input type
    Return type     : void
    Argument        : is interlaced or not
------------------------------------------------------------------------------*/
void pp_print_result(PPResult ret)
{
    printf("PPGetResult: ");

    switch (ret)
    {
    case PP_OK:
        printf("PP_OK\n");
        break;
    case PP_PARAM_ERROR:
        printf("PP_PARAM_ERROR\n");
        break;
    case PP_MEMFAIL:
        printf("PP_MEMFAIL\n");
        break;
    case PP_SET_IN_SIZE_INVALID:
        printf("PP_SET_IN_SIZE_INVALID\n");
        break;
    case PP_SET_IN_ADDRESS_INVALID:
        printf("PP_SET_IN_ADDRESS_INVALID\n");
        break;
    case PP_SET_IN_FORMAT_INVALID:
        printf("PP_SET_IN_FORMAT_INVALID\n");
        break;
    case PP_SET_CROP_INVALID:
        printf("PP_SET_CROP_INVALID\n");
        break;
    case PP_SET_ROTATION_INVALID:
        printf("PP_SET_ROTATION_INVALID\n");
        break;
    case PP_SET_OUT_SIZE_INVALID:
        printf("PP_SET_OUT_SIZE_INVALID\n");
        break;
    case PP_SET_OUT_ADDRESS_INVALID:
        printf("PP_SET_OUT_ADDRESS_INVALID\n");
        break;
    case PP_SET_OUT_FORMAT_INVALID:
        printf("PP_SET_OUT_FORMAT_INVALID\n");
        break;
    case PP_SET_VIDEO_ADJUST_INVALID:
        printf("PP_SET_VIDEO_ADJUST_INVALID\n");
        break;
    case PP_SET_RGB_BITMASK_INVALID:
        printf("PP_SET_RGB_BITMASK_INVALID\n");
        break;
    case PP_SET_FRAMEBUFFER_INVALID:
        printf("PP_SET_FRAMEBUFFER_INVALID\n");
        break;
    case PP_SET_MASK1_INVALID:
        printf("PP_SET_MASK1_INVALID\n");
        break;
    case PP_SET_MASK2_INVALID:
        printf("PP_SET_MASK2_INVALID\n");
        break;
    case PP_SET_DEINTERLACE_INVALID:
        printf("PP_SET_DEINTERLACE_INVALID\n");
        break;
    case PP_SET_IN_STRUCT_INVALID:
        printf("PP_SET_IN_STRUCT_INVALID\n");
        break;
    case PP_SET_IN_RANGE_MAP_INVALID:
        printf("PP_SET_IN_RANGE_MAP_INVALID\n");
        break;
    case PP_SET_ABLEND_UNSUPPORTED:
        printf("PP_SET_ABLEND_UNSUPPORTED\n");
        break;
    case PP_SET_DEINTERLACING_UNSUPPORTED:
        printf("PP_SET_DEINTERLACING_UNSUPPORTED\n");
        break;
    case PP_SET_DITHERING_UNSUPPORTED:
        printf("PP_SET_DITHERING_UNSUPPORTED\n");
        break;
    case PP_SET_SCALING_UNSUPPORTED:
        printf("PP_SET_SCALING_UNSUPPORTED\n");
        break;
    case PP_BUSY:
        printf("PP_BUSY\n");
        break;
    case PP_HW_BUS_ERROR:
        printf("PP_HW_BUS_ERROR\n");
        break;
    case PP_HW_TIMEOUT:
        printf("PP_HW_TIMEOUT\n");
        break;
    case PP_DWL_ERROR:
        printf("PP_DWL_ERROR\n");
        break;
    case PP_SYSTEM_ERROR:
        printf("PP_SYSTEM_ERROR\n");
        break;
    case PP_DEC_COMBINED_MODE_ERROR:
        printf("PP_DEC_COMBINED_MODE_ERROR\n");
        break;
    case PP_DEC_RUNTIME_ERROR:
        printf("PP_DEC_RUNTIME_ERROR\n");
        break;
    default:
        printf("other %d\n", ret);
        break;
    }
}

/*------------------------------------------------------------------------------
    Function name   : pp_rotation_used
    Description     : used to ask from pp if rotation on
    Return type     : u32
    Argument        : void
------------------------------------------------------------------------------*/
u32 pp_rotation_used(void)
{
    return pp.currPp->rotation ? 1 : 0;
}

/*------------------------------------------------------------------------------
    Function name   : pp_cropping_used
    Description     : used to ask from pp if cropping on
    Return type     : u32
    Argument        : void
------------------------------------------------------------------------------*/
u32 pp_cropping_used(void)
{
    return ppConf.ppInCrop.enable ? 1 : 0;
}

/*------------------------------------------------------------------------------
    Function name   : pp_rotation_used
    Description     : used to ask from pp if rotation on
    Return type     : u32
    Argument        : void
------------------------------------------------------------------------------*/
u32 pp_mpeg4_filter_used(void)
{
    return pp.currPp->filterEnabled ? 1 : 0;
}

/*
    typedef struct PPoutput_
    {
        u32 bufferBusAddr;
        u32 bufferChromaBusAddr;
    } PPOutput;

    typedef struct PPOutputBuffers_
    {
        u32 nbrOfBuffers;
        PPOutput PPoutputBuffer[17];
    } PPOutputBuffers;
*/

/*------------------------------------------------------------------------------
    Function name   : pp_setup_multibuffer
    Description     : setup pipeline multiple buffers for full pipeline
    Return type     : u32
    Argument        : pp instance, dec instance, decoder type, pp parameters
------------------------------------------------------------------------------*/
u32 pp_setup_multibuffer(PPInst pp, const void *decInst,
                            u32 decType, PpParams * params)
{
    PPResult ret;
    u32 luma_size, pic_size, i;
    u32 bytesPerPixelMul2;
    outputBuffer.nbrOfBuffers = numberOfBuffers;

    /* TODO fix for even more buffers */

    if(!numberOfBuffers)
        return 1;

    if(numberOfBuffers > 17)
        outputBuffer.nbrOfBuffers = 17;

/*    
    outputBuffer.PPoutputBuffer[0].bufferBusAddr = out_pic_ba;
    pMultibuffer_va[0] = out_pic_va;

    outputBuffer.PPoutputBuffer[0].bufferChromaBusAddr =
    outputBuffer.PPoutputBuffer[0].bufferBusAddr
    + ppConf.ppOutImg.width * ppConf.ppOutImg.height;
    if(numberOfBuffers > 1){
        outputBuffer.PPoutputBuffer[1].bufferBusAddr = out_pic_ba + (out_pic_size/2);

        pMultibuffer_va[1] = out_pic_va + out_pic_size/2/4;
        outputBuffer.PPoutputBuffer[1].bufferChromaBusAddr =
        outputBuffer.PPoutputBuffer[1].bufferBusAddr
       + ppConf.ppOutImg.width * ppConf.ppOutImg.height;
    }
*/
    /* if PIP used, alloc based on that, otherwise output size */

    /* note: 4x4 tiled mode requires dimensions to be multiples of 4, however
     * only 2 bytes per pixel are then used so the current assumption of 4 bytes
     * per pixel times the actual dimensions should suffice */
    if(params->pip == NULL)
    {
        luma_size = params->output.width * params->output.height;
    }
    else
    {
        luma_size = params->pip->width * params->pip->height;
    }
    
    if(PP_IS_RGB(params->outputFormat))
    {
        bytesPerPixelMul2 = 2*params->outRgbFmt.bytes;
    }
    else
    {
        switch(params->outputFormat)
        {
            case YUV420C:
                bytesPerPixelMul2 = 3;
                break;
            case YUYV422:
            case YVYU422:
            case UYVY422:
            case VYUY422:
            case YUYV422T4:
            case YVYU422T4:
            case UYVY422T4:
            case VYUY422T4:
                bytesPerPixelMul2 = 4;                    
                break;
            default:
                bytesPerPixelMul2 = 8;                    
                break;
        }
    }
    pic_size = luma_size * bytesPerPixelMul2 / 2;
    
    ASSERT(out_pic_size >= (numberOfBuffers * pic_size));
    
    outputBuffer.ppOutputBuffers[0].bufferBusAddr = out_pic_ba;
    outputBuffer.ppOutputBuffers[0].bufferChromaBusAddr =
    outputBuffer.ppOutputBuffers[0].bufferBusAddr + luma_size;

    pMultibuffer_va[0] = out_pic_va;    
             
    for(i=1; i <  numberOfBuffers; i++)
    {
        outputBuffer.ppOutputBuffers[i].bufferBusAddr = outputBuffer.ppOutputBuffers[i-1].bufferBusAddr + pic_size;
        outputBuffer.ppOutputBuffers[i].bufferChromaBusAddr =
        outputBuffer.ppOutputBuffers[i].bufferBusAddr + luma_size;
        
        pMultibuffer_va[i] = (u32*)((char *)pMultibuffer_va[i-1] + pic_size);
    }

    ret = PPDecSetMultipleOutput(pp, &outputBuffer);
    if( ret != PP_OK )
        return 1;

    /* setup our double buffering for display */
    last_out.bufferBusAddr = display_buff_ba;
    last_out.bufferChromaBusAddr = last_out.bufferBusAddr + luma_size;

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : pp_number_of_buffers
    Description     : decoder uses this function to say how many buffers needed
    Return type     :
    Argument        : amount of buffers
------------------------------------------------------------------------------*/
void pp_number_of_buffers(u32 amount)
{
    if(multibuffer)
    {
        numberOfBuffers = amount;
        pp_setup_multibuffer(ppInst, NULL,0, pp.currPp);
    }
}

char *pp_get_out_name(void)
{
    return pp.currPp->outputFile;
}


/*------------------------------------------------------------------------------
    Function name   : TBOverridePpContainer
    Description     : Override register settings generated by PPSetupHW() based
                      on tb.cfg settings
    Return type     :
    Argument        : PPContainer *
                      TBCfg *
------------------------------------------------------------------------------*/
void TBOverridePpContainer(PPContainer *ppC, const TBCfg * tbCfg)
{
    /* Override fast downscale if tb.cfg says so */
    if(tbCfg->ppParams.fastHorDownScaleDisable)
        ppC->fastHorizontalDownscaleDisable = 1;
    if(tbCfg->ppParams.fastVerDownScaleDisable)
        ppC->fastVerticalDownscaleDisable = 1;
}
