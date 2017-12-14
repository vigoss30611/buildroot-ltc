/**
******************************************************************************
@file felixtest.c

@brief Helper functions implementation for test applications using CI

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <felix_hw_info.h>
#include <ci/ci_api.h>
#include <mc/module_config.h>
// used to compute the save flags (has to be same than registers for HW)
#include <registers/context0.h> 
#include <linkedlist.h> // used for the fifo
// used to setup black level and other registers
#include <ctx_reg_precisions.h> 

#include <sim_image.h>

#ifdef CI_EXT_DATAGEN
#include <dg/dg_api.h>

#else

typedef struct DG_CAMERA {
    int placeholder;
} DG_CAMERA;

#endif

#include "felix.h"
#include "savefile.h"

#include <felixcommon/pixel_format.h>
#include <felixcommon/lshgrid.h>
#include <felixcommon/dpfmap.h> // to read/write some config files

#ifdef FELIX_FAKE
#include <sched.h>

#include <semaphore.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <errno.h>

#define NO_TIMED_WAIT // uncomment to avoid waiting for semaphores with a limited amount of time
#define WAIT_FOR_INT_SEC 10
#define NANOSEC_PER_MILLISEC 1000000

#endif

#define SAVE_FLAG(A) (FELIX_CONTEXT0_SAVE_CONFIG_FLAGS_ ## A ## _ENABLE_MASK)

#ifdef _WIN32
#include <Windows.h>

struct timezone
{
    int placeholder;
};

int gettimeofday(struct timeval *tp, struct timezone *tz)
{
    struct timeb curr;
    ftime(&curr);
    tp->tv_sec = (long)curr.time;
    tp->tv_usec = curr.millitm*1000; // not very precise... but on win32 we run against the simulator
    if ( tz != NULL )
    {
        return -EINVAL;
    }
    return 0;
}

#endif

#define FAKE_INTERRUPT_N_TRIES 10

// define to output only 1 channel of the display
// 0 = r, 1 = g, 2 = b
#define FELIX_DBG_CHANNEL_DISP 1

#ifdef FELIX_FAKE
#define FAKE_INTERRUPT_WAIT_MS 2*FAKE_INTERRUPT_N_TRIES*10
#else
#define FAKE_INTERRUPT_WAIT_MS 0
#endif
/*
 * remove that block when interrupts are working in simulator
 */

// fake kernel module - needs initialisation
#ifdef FELIX_FAKE
#include <ci_kernel/ci_kernel.h>
#include <ci_kernel/ci_debug.h>

#ifdef CI_EXT_DATAGEN
#include <dg_kernel/dg_camera.h>
#include <dg_kernel/dg_debug.h>
#endif

#undef CI_FATAL
#undef CI_WARNING
#undef CI_INFO

#ifdef CI_DEBUG_FCT

/// register overwrite file name format (context, frame number)
#define REGOVER_FMT "drv_regovr_ctx%d_frm%d.txt"
#endif

#endif // FELIX_FAKE

#define LOG_TAG "driver_test"
#include <felixcommon/userlog.h>

// for the moment this is used only in "capture" function but could be exported outside of it in the future
struct captureRun {
    sSimImageIn sCurrentFrame; // current frame for DG
    sSimImageIn sHDRIns;
    int iIIFDG; // internal DG to use - >= 0 if using internal dg
    CI_DG_CONVERTER sConverter;

    SaveFile encoderOutput;
    SaveFile displayOutput;
    SaveFile hdrExtOutput;
    SaveFile raw2dOutput;

    SaveFile statsOutput;

    IMG_BOOL8 bWriteDGFrame; // write converted DG frame to file
    IMG_BOOL8 bOverrideDPF; // overrides DPF in statistics
    IMG_BOOL8 bDPFEnabled; // enable dpf output
    IMG_BOOL8 bENSEnabled; // emable ens output
    IMG_BOOL8 bRawYUV; // enable output of RAW YUV data
    IMG_BOOL8 bRawDisp; // enable output of RAW RGB data or Raw Bayer data
    IMG_BOOL8 bRawHDRExt; // enable output of RAW HDR extraction
    IMG_BOOL8 bRawTiff; // enable output of the raw TIFF from raw 2D extraction
    IMG_BOOL8 bEnableTimestamps; // override timestamps output to 0
    char *pszHDRInsFile; // file to load HDR insertion from
    IMG_BOOL8 bRescaleHDRIns; // rescale HDR insertion to 16 or use data from FLX
    double runTime; // in seconds
    CI_CONNECTION *pConnection; // can be used to access some info

    IMG_UINT32 window; // computation window
    IMG_UINT32 framesToCapture; // number of frames to capture
    IMG_UINT32 maxInputFrames; // maximum number of frames to load from input file
    IMG_UINT32 nFrames; // frame already computed
    IMG_UINT32 computing; // nb of frames submitted
    IMG_UINT32 uiRegOverride; // use register override file or not for that context, > 0 enables, value as a modulo

    // stride information - set to 0 to use auto-computed ones
    IMG_UINT32 encStrideY;
    IMG_UINT32 encStrideC;
    IMG_UINT32 encOffsetY;
    IMG_UINT32 encOffsetC;

    IMG_UINT32 dispStride;
    IMG_UINT32 dispOffset;

    IMG_UINT32 HDRExtStride;
    IMG_UINT32 HDRExtOffset;

    //IMG_UINT32 HDRInsStride;
    //IMG_UINT32 HDRInsOffset;

    IMG_UINT32 raw2DStride;
    IMG_UINT32 raw2DOffset;
};

struct datagen
{
    IMG_BOOL8 bIsInternal;
    IMG_UINT32 ui32HBlanking; // horizontal blanking to use for internal DG
    IMG_UINT32 ui32VBlanking; // vertical blanking to use for internal DG

    union { // either external or internal DG
        CI_DATAGEN *intDG;
        DG_CAMERA *extDG;
    };

    //CI_GASKET sGasket;
};



static IMG_RESULT yield(IMG_UINT32 ui32SleepMiliSec)
{
#ifdef FELIX_FAKE
    sched_yield(); // let the other threads have a chance to run when faking the interrupts
    if ( ui32SleepMiliSec>0 )
    {
#ifdef _WIN32
        Sleep(ui32SleepMiliSec);
#else
        usleep(ui32SleepMiliSec*100);
#endif
    }
#else
    if ( ui32SleepMiliSec > 0 )
    {
        struct timespec toSleep = { 0, ui32SleepMiliSec*1000000 };
        LOG_INFO("wait %dms\n", ui32SleepMiliSec);
        nanosleep(&toSleep, NULL);
    }
#endif
    return IMG_SUCCESS;
}

// statsOutput can be NULL
static IMG_RESULT WriteToFile(CI_SHOT *pBuffer, CI_PIPELINE *pPipeline,
    struct captureRun *pRunInfo)
{
    void* memory = NULL;
    int tileW = pRunInfo->pConnection->sHWInfo.uiTiledScheme;
    int tileH = 4096/pRunInfo->pConnection->sHWInfo.uiTiledScheme;
    char name[128];
    IMG_RESULT ret;

    LOG_INFO("--- CTX %d got a frame ---\n", pPipeline->ui8Context);
    if ( pBuffer->bFrameError )
    {
        LOG_WARNING("!!!! CTX %d Frame is erroneous - content may me "\
            "incorrect memory !!!!\n", pPipeline->ui8Context);
    }
    
    if ( pBuffer->pEncoderOutput != NULL ) // encoder
    {
        int outWidth = pPipeline->sEncoderScaler.aOutputSize[0]*2 +2;
        int outHeigh = (pPipeline->sEncoderScaler.aOutputSize[1]+1);
        IMG_UINT32 chromaOff = pBuffer->aEncOffset[1];

        memory = (void*)((char*)pBuffer->pEncoderOutput
            + pBuffer->aEncOffset[0]);

        if ( pBuffer->bEncTiled && pRunInfo->encoderOutput.saveTo != NULL )
        {
            static int nTiled = 0;
            FILE *f = NULL;
            IMG_UINT8 *tiledMemory = NULL;

            LOG_INFO("detiling Encoder buffer (%dx%d - %d)\n",
                tileW, tileH, pBuffer->aEncYSize[0]);

            tiledMemory = (IMG_UINT8*)IMG_MALLOC(
                pBuffer->aEncYSize[0]
                *(pBuffer->aEncYSize[1]+pBuffer->aEncCbCrSize[1])
                *sizeof(IMG_UINT8));
            /* copy the memory to be faster on fpga (otherwise the access
             * for detiling is very slow) */
            IMG_MEMCPY(tiledMemory, memory,
                pBuffer->aEncYSize[0]
                *(pBuffer->aEncYSize[1]+pBuffer->aEncCbCrSize[1])
                *sizeof(IMG_UINT8));

            sprintf(name, "tiled%d_%dx%d_(%dx%d-%d).yuv", nTiled++,
                    pPipeline->sEncoderScaler.aOutputSize[0]*2 +2,
                    (pPipeline->sEncoderScaler.aOutputSize[1]+1),
                    tileW, tileH,
                    pBuffer->aEncYSize[0]);
            f = fopen(name, "wb");

            fwrite(tiledMemory, sizeof(char),
                pBuffer->aEncYSize[0]*pBuffer->aEncYSize[1]
                + pBuffer->aEncCbCrSize[0]*pBuffer->aEncCbCrSize[1],
                f);

            fclose(f);

            memory = IMG_CALLOC(pBuffer->aEncYSize[0]
                *(pBuffer->aEncYSize[1]+pBuffer->aEncCbCrSize[1]),
                sizeof(IMG_UINT8));

            // detile Y
            ret = BufferDeTile(tileW, tileH, pBuffer->aEncYSize[0],
                tiledMemory,
                (IMG_UINT8*)memory,
                pBuffer->aEncYSize[0], pBuffer->aEncYSize[1]);
            if (ret)
            {
                LOG_WARNING("Failed to convert the Y tiled buffer!\n");
            }
            chromaOff = pBuffer->aEncYSize[0] * pBuffer->aEncYSize[1];

            // detile CbCr
            ret = BufferDeTile(tileW, tileH, pBuffer->aEncCbCrSize[0],
                &(tiledMemory[pBuffer->aEncOffset[1]]),
                &(((IMG_UINT8*)memory)[chromaOff]),
                pBuffer->aEncCbCrSize[0], pBuffer->aEncCbCrSize[1]);
            if (ret)
            {
                LOG_WARNING("Failed to convert the CbCr tiled buffer!\n");
            }

            IMG_FREE(tiledMemory);
        }
        else if ( pRunInfo->bRawYUV )
        {
            FILE *f = NULL;
            static unsigned yuvraw = 0;
            sprintf(name, "raw%s_%u_%dx%d-str_%d-lines_%d-offY_%d-offC_%d.yuv",
                FormatString(pPipeline->eEncType.eFmt),
                yuvraw++,
                pPipeline->sEncoderScaler.aOutputSize[0]*2 +2,
                (pPipeline->sEncoderScaler.aOutputSize[1]+1),
                pBuffer->aEncYSize[0], pBuffer->aEncYSize[1],
                pBuffer->aEncOffset[0],
                pBuffer->aEncOffset[1]);
            f = fopen(name, "wb");

            // Y and CbCr at once
            // used to be before offset:
            /* pBuffer->aEncYSize[0] * pBuffer->aEncYSize[1]
                + pBuffer->aEncCbCrSize[0]*pBuffer->aEncCbCrSize[1] */
            fwrite(pBuffer->pEncoderOutput, sizeof(char),
                pBuffer->aEncOffset[1]
                + pBuffer->aEncCbCrSize[0]*pBuffer->aEncCbCrSize[1],
                f);

            fclose(f);
        }

        if ( pRunInfo->encoderOutput.saveTo != NULL
            && pPipeline->eEncType.ui8BitDepth == 8 )
        {
            IMG_UINT8 *pSave = (IMG_UINT8*)memory;
                 
            ret = SaveFile_writeFrame(&(pRunInfo->encoderOutput), pSave,
                pBuffer->aEncYSize[0], outWidth, outHeigh);
            if (ret)
            {
                LOG_WARNING("failed to save luma part\n");
            }
            else
            {
                // now beginning of chroma
                pSave += chromaOff;
                /* not using pBuffer->aEncOffset[1]; because if untiled
                 * the offset may have changed */
                ret = SaveFile_writeFrame(&(pRunInfo->encoderOutput), pSave,
                    pBuffer->aEncCbCrSize[0],
                    (2 * (outWidth) / pPipeline->eEncType.ui8HSubsampling),
                    ((outHeigh) / pPipeline->eEncType.ui8VSubsampling));
                if (ret)
                {
                    LOG_WARNING("failed to save chroma part\n");
                }
            }
        }
        else if ( pRunInfo->encoderOutput.saveTo != NULL
            && pPipeline->eEncType.ui8BitDepth == 10 )
        {
            // need to remove the packing to 64B
            // save with mem alignment would be:
            /*SaveFile_write(&(pRunInfo->encoderOutput),
            memory, pBuffer->aEncYSize[0]*pBuffer->aEncYSize[1]
            +pBuffer->aEncCbCrSize[0]*pBuffer->aEncCbCrSize[1]);*/
            IMG_UINT8 *pSave = (IMG_UINT8*)memory;
            // width
            IMG_SIZE uiSize = pPipeline->sEncoderScaler.aOutputSize[0]*2 +2;
            IMG_UINT32 bop;

            bop = uiSize/pPipeline->eEncType.ui8PackedElements;
            if ( uiSize%pPipeline->eEncType.ui8PackedElements != 0 )
            {
                bop++;
            }

            uiSize = (bop*pPipeline->eEncType.ui8PackedStride);
            //height is (pPipeline->sEncoderScaler.aOutputSize[1]+1);

            ret = SaveFile_writeFrame(&(pRunInfo->encoderOutput), pSave,
                pBuffer->aEncYSize[0], uiSize,
                (pPipeline->sEncoderScaler.aOutputSize[1] + 1));
            if (ret)
            {
                LOG_WARNING("failed to save luma part\n");
            }
            else
            {
                // now beginning of chroma
                pSave += chromaOff;
                /* not using pBuffer->aEncOffset[1]; because if untiled
                * the offset may have changed */

                // output width both UV plane (interleaved)
                uiSize = 2*(pPipeline->sEncoderScaler.aOutputSize[0]*2 +2)
                    /pPipeline->eEncType.ui8HSubsampling;

                bop = uiSize/pPipeline->eEncType.ui8PackedElements;
                if ( uiSize%pPipeline->eEncType.ui8PackedElements != 0 )
                {
                    bop++;
                }

                uiSize = (bop*pPipeline->eEncType.ui8PackedStride);
                /*height is (pPipeline->sEncoderScaler.aOutputSize[1]+1)
                /pPipeline->eEncType.ui8VSubsampling;*/

                ret = SaveFile_writeFrame(&(pRunInfo->encoderOutput), pSave,
                    pBuffer->aEncCbCrSize[0], uiSize,
                    ((pPipeline->sEncoderScaler.aOutputSize[1] + 1)
                    / pPipeline->eEncType.ui8VSubsampling));
                if (ret)
                {
                    LOG_WARNING("failed to save chroma part\n");
                }
            }
        }
        else
        {
            LOG_WARNING("encoder output is NULL\n");
        }

        if ( pBuffer->bEncTiled && pRunInfo->encoderOutput.saveTo != NULL )
        {
            IMG_FREE(memory);
        }
    }

    if ( pBuffer->pDisplayOutput != NULL ) // display
    {
        memory = (void*)((char*)pBuffer->pDisplayOutput
            + pBuffer->ui32DispOffset);

        if ( pBuffer->bDispTiled && pRunInfo->displayOutput.pSimImage != NULL )
        {
            static int nTiled = 0;
            FILE *f = NULL;
            IMG_UINT8 *tiledMemory = NULL;

            LOG_INFO("detiling Display buffer (%dx%d - %d)\n",
                tileW, tileH, pBuffer->aDispSize[0]);

            tiledMemory = (IMG_UINT8*)IMG_MALLOC(pBuffer->aDispSize[0]
                *pBuffer->aDispSize[1]*sizeof(IMG_UINT8));

            IMG_MEMCPY(tiledMemory, memory, pBuffer->aDispSize[0]
                *pBuffer->aDispSize[1]*sizeof(IMG_UINT8));

            sprintf(name, "tiled%d_%dx%d_(%dx%d-%d).rgb", nTiled++,
                    pPipeline->sDisplayScaler.aOutputSize[0]*2 +2,
                    (pPipeline->sDisplayScaler.aOutputSize[1]+1),
                    tileW, tileH,
                    pBuffer->aDispSize[0]);
            f = fopen(name, "wb");

            fwrite(memory, sizeof(char),
                pBuffer->aDispSize[0]*pBuffer->aDispSize[1],
                f);

            fclose(f);

            memory = IMG_CALLOC(pBuffer->aDispSize[0]*pBuffer->aDispSize[1],
                sizeof(IMG_UINT8));

            ret = BufferDeTile(tileW, tileH, pBuffer->aDispSize[0],
                tiledMemory, (IMG_UINT8*)memory,
                pBuffer->aDispSize[0], pBuffer->aDispSize[1]);
            if (ret)
            {
                LOG_WARNING("failed to detile Display buffer\n");
            }

            IMG_FREE(tiledMemory);
        }
        else if (pRunInfo->bRawDisp)
        {
            FILE *f = NULL;
            static unsigned dispraw = 0;
            
            if (pPipeline->eDataExtraction != CI_INOUT_NONE)
            {
                sprintf(name, "%s_%u_%dx%d-str_%d-lines_%d-offset_%u.yuv",
                    FormatString(pPipeline->eDispType.eFmt),
                    dispraw++,
                    (pPipeline->sImagerInterface.ui16ImagerSize[0]+1) * 2,
                    (pPipeline->sImagerInterface.ui16ImagerSize[1]+1) * 2,
                    pBuffer->aEncYSize[0], pBuffer->aEncYSize[1],
                    pBuffer->ui32DispOffset);
            }
            else
            {
                sprintf(name, "%s_%u_%dx%d-str_%d-lines_%d-offset_%u.rgb",
                    FormatString(pPipeline->eDispType.eFmt),
                    dispraw++,
                    pPipeline->sDisplayScaler.aOutputSize[0] * 2 + 2,
                    (pPipeline->sDisplayScaler.aOutputSize[1] + 1),
                    pBuffer->aDispSize[0], pBuffer->aDispSize[1],
                    pBuffer->ui32DispOffset);
            }

            f = fopen(name, "wb");

            // Y and CbCr at once
            // assumes there is no offset for CbCr start
            fwrite(pBuffer->pDisplayOutput, sizeof(char),
                pBuffer->ui32DispOffset
                + (pBuffer->aDispSize[0] * pBuffer->aDispSize[1]),
                f);

            fclose(f);
        }

        if ( pRunInfo->displayOutput.pSimImage != NULL )
        {
            if ( pPipeline->eDataExtraction == CI_INOUT_NONE )
            {
                // RGB format
                ret = SaveFile_writeFrame(&(pRunInfo->displayOutput), memory,
                    pBuffer->aDispSize[0],
                    (pPipeline->sDisplayScaler.aOutputSize[0] * 2 + 2)
                    *pPipeline->eDispType.ui8PackedStride,
                    (pPipeline->sDisplayScaler.aOutputSize[1] + 1));
                if (ret)
                {
                    LOG_WARNING("failed to write a display frame!\n");
                }
            }
            else
            {
                // Bayer format
                void *pConverted = NULL;
                IMG_SIZE outsize = 0;
                IMG_UINT16 outSizes[2] = {
                    pPipeline->ui16MaxDispOutWidth,
                    pPipeline->ui16MaxDispOutHeight
                };

                ret = convertToPlanarBayer(outSizes, memory,
                    pBuffer->aDispSize[0], pPipeline->eDispType.ui8BitDepth,
                    &pConverted, &outsize);
                if (ret)
                {
                    LOG_WARNING("failed to convert bayer frame\n");
                }
                else
                {
                    ret = SaveFile_write(&(pRunInfo->displayOutput),
                        pConverted, outsize);
                    if (ret)
                    {
                        LOG_WARNING("failed to write a data-extraction "\
                            "frame!\n");
                    }
                }

                if ( pConverted != NULL )
                {
                    IMG_FREE(pConverted);
                    pConverted = NULL;
                }
            }

        }
        else
        {
            LOG_WARNING("display output is NULL\n");
        }

        if ( pBuffer->bDispTiled && pRunInfo->displayOutput.pSimImage != NULL )
        {
            IMG_FREE(memory);
        }
    }
    if ( pBuffer->pHDRExtOutput != NULL ) // HDR Extraction
    {
        memory = (void*)((char*)pBuffer->pHDRExtOutput
            + pBuffer->ui32HDRExtOffset);

        if ( pRunInfo->hdrExtOutput.pSimImage != NULL )
        {
            if ( pBuffer->bHDRExtTiled )
            {
                static int nTiled = 0;
                FILE *f = NULL;
                IMG_UINT8 *tiledMemory = NULL;

                LOG_INFO("detiling HDR Extraction buffer (%dx%d - %d)\n",
                    tileW, tileH, pBuffer->aHDRExtSize[0]);

                tiledMemory = (IMG_UINT8*)IMG_MALLOC(pBuffer->aHDRExtSize[0]
                    *pBuffer->aHDRExtSize[1]*sizeof(IMG_UINT8));

                IMG_MEMCPY(tiledMemory, memory, pBuffer->aHDRExtSize[0]
                    *pBuffer->aHDRExtSize[1]*sizeof(IMG_UINT8));

                sprintf(name, "tiled%d_%dx%d_(%dx%d-%d)-hdr.rgb", nTiled++,
                    (pPipeline->sImagerInterface.ui16ImagerSize[0]+1)*2,
                    (pPipeline->sImagerInterface.ui16ImagerSize[1]+1)*2,
                        tileW, tileH,
                        pBuffer->aHDRExtSize[0]);
                f = fopen(name, "wb");

                fwrite(memory, sizeof(char),
                    pBuffer->aHDRExtSize[0] * pBuffer->aHDRExtSize[1],
                    f);

                fclose(f);

                memory = IMG_CALLOC(pBuffer->aHDRExtSize[0]
                    *pBuffer->aHDRExtSize[1], sizeof(IMG_UINT8));

                ret = BufferDeTile(tileW, tileH, pBuffer->aHDRExtSize[0],
                    tiledMemory, (IMG_UINT8*)memory,
                    pBuffer->aHDRExtSize[0], pBuffer->aHDRExtSize[1]);
                if (ret)
                {
                    LOG_WARNING("failed to detile HDR Extraction buffer\n");
                }

                IMG_FREE(tiledMemory);
            }
            else if ( pRunInfo->bRawHDRExt )
            {
                // output raw HDR extraction data (done if tiled)
                FILE *f = NULL;
                static unsigned hdrext = 0;
                sprintf(name, "hdrext%d_%dx%d-str_%d-lines_%d-offset_%u.rgb", 
                    hdrext++,
                    (pPipeline->sImagerInterface.ui16ImagerSize[0]+1)*2,
                    (pPipeline->sImagerInterface.ui16ImagerSize[1]+1)*2,
                    pBuffer->aHDRExtSize[0], pBuffer->aHDRExtSize[1],
                    pBuffer->ui32HDRExtOffset);
                f = fopen(name, "wb");

                fwrite(pBuffer->pHDRExtOutput, sizeof(char),
                    pBuffer->ui32HDRExtOffset
                    + (pBuffer->aHDRExtSize[0] * pBuffer->aHDRExtSize[1]),
                    f);

                fclose(f);
            
            }
        
            ret = SaveFile_writeFrame(&(pRunInfo->hdrExtOutput), memory,
                pBuffer->aHDRExtSize[0],
                ((pPipeline->sImagerInterface.ui16ImagerSize[0] + 1) * 2)
                *pPipeline->eHDRExtType.ui8PackedStride,
                ((pPipeline->sImagerInterface.ui16ImagerSize[1] + 1) * 2));
            if (ret)
            {
                LOG_WARNING("failed to write a HDR Extraction frame!\n");
            }
        }
        else
        {
            LOG_WARNING("display output is NULL\n");
        }

        if (pBuffer->bHDRExtTiled && pRunInfo->hdrExtOutput.pSimImage != NULL)
        {
            IMG_FREE(memory);
        }
    }
    if ( pBuffer->pRaw2DExtOutput != NULL )
    {
        memory = (void*)((char*)pBuffer->pRaw2DExtOutput
            + pBuffer->ui32Raw2DOffset);

        if ( pRunInfo->raw2dOutput.pSimImage != NULL )
        {
            void *pConverted = NULL;
            IMG_SIZE outsize = 0;
            IMG_UINT16 outSizes[2] = {
                (pPipeline->sImagerInterface.ui16ImagerSize[0]+1)*2,
                (pPipeline->sImagerInterface.ui16ImagerSize[1]+1)*2
            };

            ret = convertToPlanarBayerTiff(outSizes, memory,
                pBuffer->aRaw2DSize[0],
                pPipeline->eRaw2DExtraction.ui8BitDepth, &pConverted,
                &outsize);
            if (ret)
            {
                LOG_WARNING("failed to convert raw2D Tiff bayer frame\n");
            }
            else
            {
                ret = SaveFile_write(&(pRunInfo->raw2dOutput), pConverted,
                    outsize);
                if (ret)
                {
                    LOG_WARNING("failed to write a raw 2D frame!\n");
                }
            }

            if ( pConverted != NULL )
            {
                IMG_FREE(pConverted);
                pConverted = NULL;
            }
        }

        if ( pRunInfo->bRawTiff ) 
        {
            // output raw raw2d extraction data
            FILE *f = NULL;
            static unsigned rawtiff = 0;
            sprintf(name, "raw2d%d_%dx%d-str_%d-lines_%d-offset_%u-tiff%d.rggb", 
                rawtiff++,
                (pPipeline->sImagerInterface.ui16ImagerSize[0]+1)*2,
                (pPipeline->sImagerInterface.ui16ImagerSize[1]+1)*2,
                pBuffer->aRaw2DSize[0], pBuffer->aRaw2DSize[1],
                pBuffer->ui32Raw2DOffset,
                pPipeline->eRaw2DExtraction.ui8BitDepth
            );
            f = fopen(name, "wb");

            fwrite(pBuffer->pRaw2DExtOutput, sizeof(char),
                pBuffer->ui32Raw2DOffset
                + (pBuffer->aRaw2DSize[0] *pBuffer->aRaw2DSize[1]),
                f);

            fclose(f);
            
        }
    }
    // save stats
    if ( pBuffer->pStatistics != NULL && pRunInfo->statsOutput.saveTo != NULL )
    {
        IMG_CHAR header[64];
        IMG_UINT32 *toWrite = pBuffer->pStatistics;
        sprintf(header, "#stats-%dB#", pBuffer->statsSize);

        ret = SaveFile_write(&(pRunInfo->statsOutput), header, strlen(header));
        if (ret)
        {
            LOG_WARNING("failed to write the stats 'header'\n");
        }

        if (pRunInfo->bOverrideDPF)
        {
            IMG_UINT32 *copy = IMG_MALLOC(pBuffer->statsSize);
            if (copy)
            {
                IMG_MEMCPY(copy, pBuffer->pStatistics, pBuffer->statsSize);

                copy[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET / 4] = 0;
                LOG_WARNING("Override DPF FIXED from 0x%x to 0x%x\n",
                    toWrite[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET / 4],
                    copy[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET / 4]);

                copy[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET / 4] = 0;
                LOG_WARNING("Override DPF MODIFICATIONS from 0x%x to 0x%x\n",
                    toWrite[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET / 4],
                    copy[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET / 4]);

                copy[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET / 4] = 0;
                LOG_WARNING("Override DPF DROPPED from 0x%x to 0x%x\n",
                    toWrite[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET / 4],
                    copy[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET / 4]);
            }
            else
            {
                LOG_WARNING("failed to allocate temporary copy of "\
                    "statistics of %u Bytes\n", pBuffer->statsSize);
            }
            toWrite = copy;
        }
        else
        {
            toWrite = pBuffer->pStatistics;
        }

        if (toWrite)
        {
            ret = SaveFile_write(&(pRunInfo->statsOutput), toWrite,
                pBuffer->statsSize);
            if (ret)
            {
                LOG_WARNING("failed to write the stats\n");
            }
        }

        sprintf(header, "#stats-done#");
        ret = SaveFile_write(&(pRunInfo->statsOutput), header, strlen(header));
        if (ret)
        {
            LOG_WARNING("failed to write the stats 'footer'\n");
        }

        if (pRunInfo->bOverrideDPF && toWrite)
        {
            IMG_FREE(toWrite);
        }
    }
    // defective pixel saving
    if ( pBuffer->pDPFMap != NULL && pRunInfo->bDPFEnabled )
    {
        IMG_CHAR header[64];
        IMG_UINT32 nCorr = 0;
        int ret;

        IMG_UINT32 *pStast = (IMG_UINT32*)pBuffer->pStatistics;
        FILE *f = NULL;

        nCorr = pStast[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET/4]
            - pStast[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET/4];

        sprintf(header, "ctx%d-%d-dpf_write_%d.dat", pPipeline->ui8Context,
            pRunInfo->nFrames, nCorr);
        LOG_INFO("DPF corrected %d and has %d (%d dropped) in the "\
            "output map '%s'\n",
            pStast[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET/4],
            nCorr, pStast[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET/4],
            header);

#ifdef DPF_CONV_CHECK_FMT // test valid format
        {
            int coordMask = DPF_MAP_COORD_X_COORD_MASK
                |DPF_MAP_COORD_Y_COORD_MASK;
            int dataMask = DPF_MAP_DATA_STATUS_MASK
                |DPF_MAP_DATA_CONFIDENCE_MASK|DPF_MAP_DATA_PIXEL_MASK;
            int vC, vD, i;
            IMG_UINT32 *vInput = (IMG_UINT32*)pBuffer->pDPFMap;

            for ( i = 0 ; i < nCorr ; i++ )
            {
                vC = vInput[2*i + DPF_MAP_COORD_OFFSET/4];
                vD = vInput[2*i + DPF_MAP_DATA_OFFSET/4];

                if ( (vC&coordMask) != vC || (vD&dataMask) != vD )
                {
                    LOG_WARNING("binary is corrupted at DPF correction %d "\
                        "(0x%x): coord=0x%08x (coord mask=0x%08x) "\
                        "data=0x%08x (data mask=0x%08x)\n",
                        i, i*DPF_MAP_OUTPUT_SIZE, vC, coordMask, vD, dataMask);
                }
            }
        }
#endif

        if (nCorr*DPF_MAP_OUTPUT_SIZE > pBuffer->uiDPFSize)
        {
            LOG_ERROR("nb of correct pixels given by HW does not fit in "\
                "buffer!\n");
        }
        else if ( (f = fopen(header, "wb")) != NULL )
        {
            if ( nCorr > 0 )
            {
                ret = fwrite(pBuffer->pDPFMap, DPF_MAP_OUTPUT_SIZE, nCorr, f);
                if (ret!=nCorr)
                {
                    LOG_WARNING("failed to save DPF to '%s' (fwrite "\
                        "returned %d)!\n", header, ret);
                }
            }
            else
            {
                LOG_WARNING("no pixel corrected! '%s' will be empty!\n",
                    header);
            }
            fclose(f);
        }
        else
        {
            LOG_ERROR("failed to open '%s'!\n", header);
        }
    }

    // encoder starts output
    // @ add no_write support
    if ( pBuffer->pENSOutput != NULL && pRunInfo->bENSEnabled )
    {
        IMG_CHAR header[64];
        FILE *f = NULL;

        sprintf(header, "ctx%d-%d-ens_out-%dB.dat",
            pPipeline->ui8Context, pRunInfo->nFrames, pBuffer->uiENSSize);

        LOG_INFO("ENS saving to '%s'\n", header);
        if ( (f = fopen(header, "wb")) != NULL )
        {
            int written =
                fwrite(pBuffer->pENSOutput, sizeof(char),
                pBuffer->uiENSSize, f);
            if (written!=pBuffer->uiENSSize)
            {
                LOG_WARNING("failed to save ENS to '%s' wrote %d/%dB!\n",
                    header, written, pBuffer->uiENSSize);
            }
            fclose(f);
        }
        else
        {
            LOG_ERROR("failed to open '%s'!\n", header);
        }
    }

    return IMG_SUCCESS;
}

/*
 * end of interrupt block
 */

/**
 * @brief Configure the Camera, register it and add buffers - ready to be started
 */
static IMG_RESULT ConfigureExtDG(struct CMD_PARAM *parameters, const CI_HWINFO *pHWInfo, int dg, sSimImageIn *pPGM, struct datagen *pDatagen)
{
#ifdef CI_EXT_DATAGEN
    IMG_RESULT ret;
    IMG_UINT32 i;
    IMG_UINT32 nBuffers = 0;
    int parallelBitDepth = 0;
    DG_CAMERA *pCamera = NULL;

    if ( pDatagen->bIsInternal )
    {
        LOG_ERROR("Given datagen is internal!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCamera = pDatagen->extDG;
    pCamera->uiWidth = pPGM->info.ui32Width;
    pCamera->uiHeight = pPGM->info.ui32Height;

    if ( (pHWInfo->gasket_aType[dg]&CI_GASKET_MIPI) != 0 )
    {
        if ( pPGM->info.ui8BitDepth <= 10 )
        {
            pCamera->eBufferFormats = DG_MEMFMT_MIPI_RAW10;
            LOG_INFO("ExtDG enable MIPI 10b\n");
        }
        else if ( pPGM->info.ui8BitDepth <= 12 )
        {
            pCamera->eBufferFormats = DG_MEMFMT_MIPI_RAW12;
            LOG_INFO("ExtDG enable MIPI 12b\n");
        }
        else if ( pPGM->info.ui8BitDepth <= 14 )
        {
            pCamera->eBufferFormats = DG_MEMFMT_MIPI_RAW14;
            LOG_INFO("ExtDG enable MIPI 14b\n");
        }
        else
        {
            LOG_ERROR("input bit depth %d isn't supported (MIPI max is 14b)\n", pPGM->info.ui8BitDepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        parallelBitDepth = pPGM->info.ui8BitDepth;

        if ( parallelBitDepth < pHWInfo->gasket_aBitDepth[dg] )
        {
            LOG_WARNING("image bitdepth is %d but gasket is %d - forcing DG to gasket bitdepth (NO RESAMPLING)\n", pPGM->info.ui8BitDepth, pHWInfo->gasket_aBitDepth[dg]);
            parallelBitDepth = pHWInfo->gasket_aBitDepth[dg];
        }

        if ( parallelBitDepth <= 10 )
        {
            pCamera->eBufferFormats = DG_MEMFMT_BT656_10;
            LOG_INFO("ExtDG enable Parallel 10b\n");
        }
        else if ( parallelBitDepth <= 12 )
        {
            pCamera->eBufferFormats = DG_MEMFMT_BT656_12;
            LOG_INFO("ExtDG enable Parallel 12b\n");
        }
        else
        {
            LOG_ERROR("input bit depth %d isn't supported (PARALLEL max is 12b)\n", pPGM->info.ui8BitDepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }

    if ( (pHWInfo->gasket_aType[dg]&CI_GASKET_MIPI) != 0 )
    {
        LOG_INFO("DG %d uses MIPI\n", dg);
    }
    else if ( (pHWInfo->gasket_aType[dg]&CI_GASKET_PARALLEL) != 0 )
    {
        LOG_INFO("DG %d uses PARALLEL\n", dg);

        if ( parallelBitDepth != pHWInfo->gasket_aBitDepth[dg] )
        {
            LOG_ERROR("parallel bitdepth of %d chosen but gasket %d only support bitdepth %d (image has bitdepth of %d)\n",
                parallelBitDepth, dg, pHWInfo->gasket_aBitDepth[dg], pPGM->info.ui8BitDepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        LOG_ERROR("Unknown type %d for DG %d\n", pHWInfo->gasket_aType[dg], dg);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pCamera->bMIPI_LF = parameters->sDataGen[dg].bMIPIUsesLineFlag;
    pCamera->ui8DGContext = dg;

    /// @ check that ui8DGContext < nb datagenerators

    // configure blanking with fixed rule
    /*pCamera->ui16HoriBlanking = (pPGM->info.ui32Width/10); // 10%
      if ( pCamera->ui16HoriBlanking < FELIX_MIN_H_BLANKING ) pCamera->ui16HoriBlanking = FELIX_MIN_H_BLANKING;
      pCamera->ui16VertBlanking = (pPGM->info.ui32Height/5); // 20%
      if ( pCamera->ui16VertBlanking < FELIX_MIN_V_BLANKING ) pCamera->ui16VertBlanking = FELIX_MIN_V_BLANKING;*/
    pCamera->ui16HoriBlanking = parameters->sDataGen[dg].aBlanking[0];
    pCamera->ui16VertBlanking = parameters->sDataGen[dg].aBlanking[1];

    switch(pPGM->info.eColourModel)
    {
    case SimImage_RGGB:
        pCamera->eBayerOrder = MOSAIC_RGGB;
        break;
    case SimImage_GRBG:
        pCamera->eBayerOrder = MOSAIC_GRBG;
        break;
    case SimImage_GBRG:
        pCamera->eBayerOrder = MOSAIC_GBRG;
        break;
    case SimImage_BGGR:
        pCamera->eBayerOrder = MOSAIC_BGGR;
        break;
    }

    if ( (ret=DG_CameraRegister(pCamera)) != IMG_SUCCESS )
    {
        LOG_ERROR("failed to register camera\n");
        return ret;
    }

    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        if ( parameters->sCapture[i].ui8imager == dg )
        {
            nBuffers = IMG_MAX_INT(parameters->sCapture[i].ui32ConfigBuffers, nBuffers);
        }
    }

    if ( (ret=DG_CameraAddPool(pCamera, nBuffers)) != IMG_SUCCESS )
    {
        LOG_ERROR("failed to add buffers\n");
        return ret;
    }

    return IMG_SUCCESS;
#else
    LOG_ERROR("Application compiled without external data genrator support!\n");
    return IMG_ERROR_NOT_SUPPORTED;
#endif // CI_EXT_DATAGEN
}

/**
 * @brief Configure the internal Data generator
 */
static IMG_RESULT ConfigureIntDG(struct CMD_PARAM *parameters, const CI_HWINFO *pHWInfo, int gasket, sSimImageIn *pImage, struct datagen *pDatagen)
{
    IMG_RESULT ret;
    IMG_UINT32 i;
    IMG_UINT32 nBuffers = 0;
    int parallelBitDepth = 0;
    CI_DG_CONVERTER sTmpConv;
    IMG_UINT32 ui32AllocSize = 0;
    CI_DATAGEN *pCamera = NULL;

    if ( pDatagen->bIsInternal == IMG_FALSE )
    {
        LOG_ERROR("given datagen is not internal!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pCamera = pDatagen->intDG;
    pCamera->ui8Gasket = gasket;
    pCamera->bPreload = IMG_FALSE; /// @ add parameter for preload
    
    {
        parallelBitDepth = pImage->info.ui8BitDepth;

        if ( parallelBitDepth < pHWInfo->gasket_aBitDepth[gasket] )
        {
            LOG_WARNING("image bitdepth is %d but gasket is %d - forcing DG to gasket bitdepth (NO RESAMPLING)\n", pImage->info.ui8BitDepth, pHWInfo->gasket_aBitDepth[gasket]);
            parallelBitDepth = pHWInfo->gasket_aBitDepth[gasket];
        }

        if ( parallelBitDepth <= 10 )
        {
            pCamera->eFormat = CI_DGFMT_PARALLEL_10;
            LOG_INFO("IntDG enable Parallel 10b\n");
        }
        else if ( parallelBitDepth <= 12 )
        {
            pCamera->eFormat = CI_DGFMT_PARALLEL_12;
            LOG_INFO("IntDG enable Parallel 12b\n");
        }
        else
        {
            LOG_ERROR("input bit depth %d isn't supported (PARALLEL max is 12b)\n", pImage->info.ui8BitDepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }

    if ( parallelBitDepth != pHWInfo->gasket_aBitDepth[gasket] )
    {
        LOG_ERROR("parallel bitdepth of %d chosen but gasket %d only support bitdepth %d (image has bitdepth of %d)\n",
            parallelBitDepth, gasket, pHWInfo->gasket_aBitDepth[gasket], pImage->info.ui8BitDepth);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // blanking is configured per frame
    
    switch(pImage->info.eColourModel)
    {
    case SimImage_RGGB:
        pCamera->eBayerMosaic = MOSAIC_RGGB;
        break;
    case SimImage_GRBG:
        pCamera->eBayerMosaic = MOSAIC_GRBG;
        break;
    case SimImage_GBRG:
        pCamera->eBayerMosaic = MOSAIC_GBRG;
        break;
    case SimImage_BGGR:
        pCamera->eBayerMosaic = MOSAIC_BGGR;
        break;
    }

    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        if ( parameters->sCapture[i].ui8imager == gasket )
        {
            nBuffers = IMG_MAX_INT(parameters->sCapture[i].ui32ConfigBuffers, nBuffers);
        }
    }

    ret = CI_ConverterConfigure(&sTmpConv, pCamera->eFormat);
    if ( IMG_SUCCESS != ret )
    {
        LOG_ERROR("Failed to initialise converted!\n");
        return ret;
    }

    ui32AllocSize = CI_ConverterFrameSize(&sTmpConv, pImage->info.ui32Width, pImage->info.ui32Height);

    for ( i = 0 ; i < nBuffers ; i++ )
    {
        ret = CI_DatagenAllocateFrame(pCamera, ui32AllocSize, NULL);
        if ( IMG_SUCCESS != ret )
        {
            LOG_ERROR("failed to allocate IntDG buffers %d/%d\n", i, nBuffers);
            // already allocated frames should be done when destroying Datagen object
            return ret;
        }
    }

    //
    // configure the gasket part of the internal dg
    //
    /*pDatagen->sGasket.bParallel = IMG_TRUE;
    pDatagen->sGasket.ui8ParallelBitdepth = parallelBitDepth;
    pDatagen->sGasket.uiGasket = gasket;
    pDatagen->sGasket.uiWidth = pImage->info.ui32Width-1;
    pDatagen->sGasket.uiHeight = pImage->info.ui32Height-1;*/

    return IMG_SUCCESS;
}

/* used nBuffers instead of  parameters->sCapture[ctx].ui32ConfigBuffers
 * because we want to allocate 1 by 1 when running parallel to test
 * the HW flush */
static IMG_RESULT AllocateBuffers(const struct CMD_PARAM *parameters,
    int ctx, CI_PIPELINE *pPipeline, unsigned int nBuffers)
{
    IMG_RESULT ret = IMG_SUCCESS;
    unsigned int f;
    IMG_BOOL8 bTiled = IMG_FALSE;
    // if tiled the size is 0 to let driver choose
    IMG_UINT32 uiSize;

    if (pPipeline->eEncType.eFmt != PXL_NONE && ret == IMG_SUCCESS)
    {
        for (f = 0; f < nBuffers && ret == IMG_SUCCESS; f++)
        {
            uiSize = parameters->sCapture[ctx].uiYUVAlloc;
            bTiled = IMG_FALSE;
            if (f < parameters->sCapture[ctx].ui32NEncTiled
                && pPipeline->bSupportTiling)
            {
                bTiled = IMG_TRUE;
                uiSize = 0;
            }

            // we do not care about the bufferId as we trigger 1st available
            // we give a size of 0 so that kernel side computes it
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER,
                uiSize, bTiled, NULL);
            if (ret)
            {
                LOG_ERROR("Failed to allocate encoder buffer %d/%d "\
                    "(tiled=%d)\n",
                    f + 1,
                    parameters->sCapture[ctx].ui32ConfigBuffers, bTiled);
            }
        }
    }
    
    // not using pPipeline->eDispType.eFmt to know if DISP or DE
    if (parameters->sCapture[ctx].pszDisplayOutput && ret == IMG_SUCCESS)
    {
        for (f = 0; f < nBuffers && ret == IMG_SUCCESS; f++)
        {
            bTiled = IMG_FALSE;
            uiSize = parameters->sCapture[ctx].uiDispAlloc;
            if (f < parameters->sCapture[ctx].ui32NDispTiled
                && pPipeline->bSupportTiling)
            {
                bTiled = IMG_TRUE;
                uiSize = 0;
            }

            // we do not care about the bufferId as we trigger 1st available
            // we give a size of 0 so that kernel side computes it
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_DISPLAY,
                uiSize, bTiled, NULL);
            if (ret)
            {
                LOG_ERROR("Failed to allocate display buffer %d/%d "\
                    "(tiled=%d)\n",
                    f + 1,
                    parameters->sCapture[ctx].ui32ConfigBuffers, bTiled);
            }
        }
    }

    // not using pPipeline->eDispType.eFmt to know if DISP or DE
    if (parameters->sCapture[ctx].pszDataExtOutput && ret == IMG_SUCCESS)
    {
        bTiled = IMG_FALSE;
        uiSize = parameters->sCapture[ctx].uiDispAlloc;
        for (f = 0; f < nBuffers && ret == IMG_SUCCESS; f++)
        {
            // we do not tile data extraction output

            // we do not care about the bufferId as we trigger 1st available
            // we give a size of 0 so that kernel side computes it
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_DATAEXT,
                uiSize, IMG_FALSE, NULL);
            if (ret)
            {
                LOG_ERROR("Failed to allocate data extraction buffer %d/%d "\
                    "(tiled=%d)\n",
                    f + 1,
                    parameters->sCapture[ctx].ui32ConfigBuffers, bTiled);
            }
        }
    }

    if (pPipeline->eHDRExtType.eFmt != PXL_NONE && ret == IMG_SUCCESS)
    {
        for (f = 0; f < nBuffers && ret == IMG_SUCCESS; f++)
        {
            bTiled = IMG_FALSE;
            uiSize = parameters->sCapture[ctx].uiHDRExtAlloc;
            if (f < parameters->sCapture[ctx].ui32NHDRExtTiled
                && pPipeline->bSupportTiling)
            {
                bTiled = IMG_TRUE;
                uiSize = 0;
            }

            // we do not care about the bufferId as we trigger 1st available
            // we give a size of 0 so that kernel side computes it
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_HDREXT,
                uiSize, bTiled, NULL);
            if (ret)
            {
                LOG_ERROR("Failed to allocate HDR extraction buffer %d/%d "\
                    "(tiled=%d)\n",
                    f + 1,
                    parameters->sCapture[ctx].ui32ConfigBuffers, bTiled);
            }
        }
    }

    if (pPipeline->eHDRInsType.eFmt != PXL_NONE && ret == IMG_SUCCESS)
    {
        bTiled = IMG_FALSE;
        uiSize = 0; // we always let the driver compute HDR insertion size
        for (f = 0; f < nBuffers && ret == IMG_SUCCESS; f++)
        {
            // we CANNOT tile HDR Insertion
            // we do not care about the bufferId as we trigger 1st available
            // we give a size of 0 so that kernel side computes it
            // parameters->sCapture[ctx].uiHDRInsSize if existing
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_HDRINS,
                uiSize, IMG_FALSE, NULL);
            if (ret)
            {
                LOG_ERROR("Failed to allocate HDR insertion buffer %d/%d "\
                    "(tiled=%d)\n",
                    f + 1,
                    parameters->sCapture[ctx].ui32ConfigBuffers, bTiled);
            }
        }
    }

    if (pPipeline->eRaw2DExtraction.eFmt != PXL_NONE && ret == IMG_SUCCESS)
    {
        bTiled = IMG_FALSE;
        uiSize = parameters->sCapture[ctx].uiRaw2DAlloc;
        for (f = 0; f < nBuffers && ret == IMG_SUCCESS; f++)
        {
            // we CANNOT tile Raw2D output
            // we do not care about the bufferId as we trigger 1st available
            // we give a size of 0 so that kernel side computes it
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_RAW2D,
                uiSize, IMG_FALSE, NULL);
            if (ret)
            {
                LOG_ERROR("Failed to allocate Raw2D extraction buffer %d/%d "\
                    "(tiled=%d)\n",
                    f + 1,
                    parameters->sCapture[ctx].ui32ConfigBuffers, bTiled);
            }
        }
    }

    return ret;
}

/**
 * @brief Configure the Pipeline, register it and add buffers - ready to be started
 */
static IMG_RESULT ConfigurePipeline(const struct CMD_PARAM *parameters, int ctx, const struct SimImageInfo *pImageInfo, CI_PIPELINE *pPipeline, const CI_HWINFO *pHWInfo)
{
    IMG_RESULT ret;
    MC_PIPELINE sPipelineConfig;
    int saveFlag;
    sSimImageIn image;

    if ( (ret=MC_IIFInit(&(sPipelineConfig.sIIF))) != IMG_SUCCESS )
    {
        return ret;
    }

    
    if ( pImageInfo->ui32Width%CI_CFA_WIDTH != 0 || pImageInfo->ui32Height%CI_CFA_HEIGHT != 0 )
    {
        LOG_ERROR("input image must have even sizes (current size is %dx%d)\n", pImageInfo->ui32Width, pImageInfo->ui32Height);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    /*
     * setup everything in the imager interface or that is context related
     */

    switch(pImageInfo->eColourModel)
    {
    case SimImage_RGGB:
        sPipelineConfig.sIIF.eBayerFormat = MOSAIC_RGGB;
        break;
    case SimImage_GRBG:
        sPipelineConfig.sIIF.eBayerFormat = MOSAIC_GRBG;
        break;
    case SimImage_GBRG:
        sPipelineConfig.sIIF.eBayerFormat = MOSAIC_GBRG;
        break;
    case SimImage_BGGR:
        sPipelineConfig.sIIF.eBayerFormat = MOSAIC_BGGR;
        break;
    }
    sPipelineConfig.sIIF.ui8Imager = parameters->sCapture[ctx].ui8imager;
    sPipelineConfig.sIIF.ui16ImagerSize[0] = pImageInfo->ui32Width/CI_CFA_WIDTH;
    sPipelineConfig.sIIF.ui16ImagerSize[1] = pImageInfo->ui32Height/CI_CFA_HEIGHT;
    // leave offset to 0
    
    if ( sPipelineConfig.sIIF.ui16ImagerSize[0]*2 > pHWInfo->context_aMaxWidthMult[ctx] ||
         sPipelineConfig.sIIF.ui16ImagerSize[1]*2 > pHWInfo->context_aMaxHeight[ctx])
    {
        LOG_ERROR("image is too big for the context: %dx%d (max supported %dx%d)\n",
                sPipelineConfig.sIIF.ui16ImagerSize[0]*2, sPipelineConfig.sIIF.ui16ImagerSize[1]*2,
                pHWInfo->context_aMaxWidthMult[ctx], pHWInfo->context_aMaxHeight[ctx]);
        return IMG_ERROR_FATAL;
    }

    if ( (ret=MC_PipelineInit(&sPipelineConfig, pHWInfo)) != IMG_SUCCESS )
    {
        return ret;
    }

    /*
     * setup output choice
     */
    sPipelineConfig.eEncOutput = PXL_NONE;
    sPipelineConfig.eDispOutput = PXL_NONE;
    sPipelineConfig.eHDRExtOutput = PXL_NONE;
    sPipelineConfig.eRaw2DExtOutput = PXL_NONE;
    //sPipelineConfig.fSystemBlackLevel = parameters->sCapture[ctx].sysBlack; // system black level is overriden after conversion to allow the whole register range usage

    pPipeline->eEncType.eBuffer = TYPE_NONE;
    pPipeline->eDispType.eBuffer = TYPE_NONE;
    pPipeline->ui8Context = ctx;

    saveFlag = parameters->sCapture[ctx].iStats;
    if ( parameters->sCapture[ctx].iStats == -1 )
    {
        saveFlag = SAVE_FLAG(EXS_GLOB)|SAVE_FLAG(EXS_REGION)
            |SAVE_FLAG(HIS_GLOB)|SAVE_FLAG(HIS_REGION)
            |SAVE_FLAG(FOS_GRID)|SAVE_FLAG(FOS_ROI)
            |SAVE_FLAG(FLD)
            |SAVE_FLAG(WBS)
            |SAVE_FLAG(ENS);

        LOG_INFO("enabling all stats (-1 replaced by %d)\n", saveFlag);
    }

    sPipelineConfig.sEXS.bGlobalEnable = (saveFlag&SAVE_FLAG(EXS_GLOB)) != 0;
    sPipelineConfig.sEXS.bRegionEnable = (saveFlag&SAVE_FLAG(EXS_REGION)) != 0;
    sPipelineConfig.sHIS.bGlobalEnable = (saveFlag&SAVE_FLAG(HIS_GLOB)) != 0;
    sPipelineConfig.sHIS.bRegionEnable = (saveFlag&SAVE_FLAG(HIS_REGION)) != 0;
    sPipelineConfig.sFOS.bGlobalEnable = (saveFlag&SAVE_FLAG(FOS_GRID)) != 0;
    sPipelineConfig.sFOS.bRegionEnable = (saveFlag&SAVE_FLAG(FOS_ROI)) != 0;
    sPipelineConfig.sFLD.bEnable = (saveFlag&SAVE_FLAG(FLD)) != 0;
    sPipelineConfig.sWBS.ui8ActiveROI = (saveFlag&SAVE_FLAG(WBS)) != 0; // for the moment no White balance
    sPipelineConfig.sENS.bEnable = (saveFlag&SAVE_FLAG(ENS)) != 0;
    sPipelineConfig.bEnableCRC = parameters->sCapture[ctx].bCRC;

    if ( parameters->sCapture[ctx].iDPFEnabled > 0 )
    {
        LOG_INFO("enable DPF detect\n");
        sPipelineConfig.sDPF.eDPFEnable = CI_DPF_DETECT_ENABLED; // disable DPF until HW is ready
        if ( parameters->sCapture[ctx].iDPFEnabled > 1 )
        {
            LOG_INFO("enable DPF write\n");
            sPipelineConfig.sDPF.eDPFEnable |= CI_DPF_WRITE_MAP_ENABLED;
        }
    }

    if ( parameters->sCapture[ctx].pszDPFRead != NULL )
    {
        if ( DPF_Load_bin(&(sPipelineConfig.sDPF.apDefectInput), &(sPipelineConfig.sDPF.ui32NDefect), parameters->sCapture[ctx].pszDPFRead) != IMG_SUCCESS )
        {
            LOG_ERROR("failed to configure DPF read map!\n");
            return IMG_ERROR_FATAL;
        }
        sPipelineConfig.sDPF.eDPFEnable |= CI_DPF_READ_MAP_ENABLED;
    }

#ifdef FELIX_FAKE
    sPipelineConfig.bEnableTimestamp = IMG_FALSE; // no timestamp for pdump!
    if(parameters->sCapture[ctx].bEnableTimestamps)
    {
        LOG_WARNING("Ignoring timestamp option when running against CSIM\n");
    }
#else
    sPipelineConfig.bEnableTimestamp = parameters->sCapture[ctx].bEnableTimestamps;
#endif
    LOG_INFO("Timestamps are %s\n", sPipelineConfig.bEnableTimestamp?"ON":"OFF");
    

    /*if ( sPipelineConfig.sFLD.bEnable == IMG_TRUE )
    {
        // use DG info to setup flicker
        sPipelineConfig.sFLD.ui16VTot = pDGCamera->ui16VertBlanking + pDGCamera->uiHeight;
    }*/
    /// @ update flicker

    if ( (saveFlag&SAVE_FLAG(ENS)) !=0 )
    {
        sPipelineConfig.sENS.ui32NLines = parameters->sCapture[ctx].uiENSNLines;
        sPipelineConfig.sENS.ui32KernelSubsampling = 0; // no subsampling
    }

    if ( parameters->sCapture[ctx].pszEncoderOutput != NULL )
    {
        if ( parameters->sCapture[ctx].bEncOut422 == IMG_FALSE )
        {
            sPipelineConfig.eEncOutput = YVU_420_PL12_8;
            if ( parameters->sCapture[ctx].bEncOut10b == IMG_TRUE )
            {
                sPipelineConfig.eEncOutput = YVU_420_PL12_10;
            }

            if ( parameters->sCapture[ctx].escPitch[0] > 4.0 || parameters->sCapture[ctx].escPitch[1] > 4.0 )
            {
                sPipelineConfig.sESC.eSubsampling = EncOut420_vss;
            }
            else
            {
                sPipelineConfig.sESC.eSubsampling = EncOut420_scaler;
            }
            LOG_INFO("ctx %d YUV 420 %db\n", ctx, parameters->sCapture[ctx].bEncOut10b?10:8);
        }
        else
        {
            sPipelineConfig.eEncOutput = YVU_422_PL12_8;
            if ( parameters->sCapture[ctx].bEncOut10b == IMG_TRUE )
            {
                sPipelineConfig.eEncOutput = YVU_422_PL12_10;
            }
            sPipelineConfig.sESC.eSubsampling = EncOut422;
            LOG_INFO("ctx %d YUV 422 %db\n", ctx, parameters->sCapture[ctx].bEncOut10b?10:8);
        }

        sPipelineConfig.sESC.aPitch[0] = parameters->sCapture[ctx].escPitch[0];
        sPipelineConfig.sESC.aPitch[1] = parameters->sCapture[ctx].escPitch[1];
        sPipelineConfig.sESC.aOutputSize[0] = (IMG_UINT16)((sPipelineConfig.sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH)/parameters->sCapture[ctx].escPitch[0]);
        sPipelineConfig.sESC.aOutputSize[1] = (IMG_UINT16)((sPipelineConfig.sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT)/parameters->sCapture[ctx].escPitch[1]);

        if ( sPipelineConfig.sESC.aOutputSize[0]%2 != 0 ) // insure size is even
        {
            sPipelineConfig.sESC.aOutputSize[0]--;
        }
        if ( sPipelineConfig.sESC.aOutputSize[1]%2 != 0 && sPipelineConfig.eEncOutput != YVU_422_PL12_8 && sPipelineConfig.eEncOutput != YVU_422_PL12_10 ) // insure size is even
        {
            sPipelineConfig.sESC.aOutputSize[1]--;
        }

        if ( sPipelineConfig.sESC.aOutputSize[0] == (sPipelineConfig.sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH) &&
             sPipelineConfig.sESC.aOutputSize[1] == (sPipelineConfig.sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT)
            )
        {
            LOG_INFO("ctx %d bypass ESC\n", ctx);
            sPipelineConfig.sESC.bBypassESC = IMG_TRUE;
        }
    }
    else
    {
        sPipelineConfig.sESC.bBypassESC = IMG_TRUE;
    }

    if ( parameters->sCapture[ctx].pszDisplayOutput != NULL )
    {
        sPipelineConfig.eDispOutput = RGB_888_32;//RGB_888_32, RGB_888_24 or RGB_101010_32

        if (parameters->sCapture[ctx].bDispOut10 == IMG_TRUE )
        {
            sPipelineConfig.eDispOutput = RGB_101010_32;
            LOG_INFO("ctx %d RGB10 32\n", ctx);
        }
        else if ( parameters->sCapture[ctx].bDispOut24 == IMG_TRUE )
        {
            sPipelineConfig.eDispOutput = RGB_888_24;
            LOG_INFO("ctx %d RGB24\n", ctx);
        }
        else
        {
            LOG_INFO("ctx %d RGB8 32\n", ctx);
        }

        sPipelineConfig.sDSC.aPitch[0] = parameters->sCapture[ctx].dscPitch[0];
        sPipelineConfig.sDSC.aPitch[1] = parameters->sCapture[ctx].dscPitch[1];
        sPipelineConfig.sDSC.aOutputSize[0] = (IMG_UINT16)((sPipelineConfig.sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH)/parameters->sCapture[ctx].dscPitch[0]);
        sPipelineConfig.sDSC.aOutputSize[1] = (IMG_UINT16)((sPipelineConfig.sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT)/parameters->sCapture[ctx].dscPitch[1]);

        if ( sPipelineConfig.sDSC.aOutputSize[0]%2 != 0 ) // insure size is even
        {
            sPipelineConfig.sDSC.aOutputSize[0]--;
        }

        if ( sPipelineConfig.sDSC.aOutputSize[0] == (sPipelineConfig.sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH) &&
             sPipelineConfig.sDSC.aOutputSize[1] == (sPipelineConfig.sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT)
            )
        {
            LOG_INFO("ctx %d bypass DSC\n", ctx);
            sPipelineConfig.sDSC.bBypassDSC = IMG_TRUE;
        }
    }
    else
    {
        if ( parameters->sCapture[ctx].pszDataExtOutput != NULL )
        {
            IMG_UINT8 bitDepth = parameters->sCapture[ctx].uiDEBitDepth;
            sPipelineConfig.eDEPoint = (enum CI_INOUT_POINTS)(parameters->ui8DataExtraction-1);

            // param 0 is disabled but HW expects from 0 to CI_INOUT_NONE-1
            if ( parameters->ui8DataExtraction > CI_INOUT_NONE )
            {
                LOG_ERROR("Data Extraction point %d not supported\n", parameters->ui8DataExtraction);
                return IMG_ERROR_NOT_SUPPORTED;
            }

            if ( bitDepth == 0 )
            {
                bitDepth = pHWInfo->config_ui8BitDepth;
            }

            // try to use the best bayer format for the current HW
            if ( bitDepth < 10 )
            {
                sPipelineConfig.eDispOutput = BAYER_RGGB_8;
                LOG_INFO("ctx %d Bayer 8b\n", ctx);
            }
            else if ( bitDepth > 10 )
            {
                sPipelineConfig.eDispOutput = BAYER_RGGB_12;
                LOG_INFO("ctx %d Bayer 12b\n", ctx);
            }
            else
            {
                sPipelineConfig.eDispOutput = BAYER_RGGB_10;
                LOG_INFO("ctx %d Bayer 10b\n", ctx);
            }
        }
        else
        {
            sPipelineConfig.eDEPoint = CI_INOUT_NONE; // no allocation for display pipeline
        }
    }
    if ( parameters->sCapture[ctx].pszHDRextOutput != NULL )
    {
        /* driver should fail to apply this configuration later if HDR is
         * not possible to allow testing */
        
        sPipelineConfig.eHDRExtOutput = BGR_101010_32;
        LOG_INFO("ctx %d HDR Ext\n", ctx);
    }
    if (parameters->sCapture[ctx].pszHDRInsInput != NULL)
    {
        /* driver should fail to apply this configuration later if HDR is
         * not possible to allow testing */
        sPipelineConfig.eHDRInsertion = BGR_161616_64;
        //sSimImageIn image;

        LOG_INFO("ctx %d HDR Ins\n", ctx);


        // check sizes of provided image are big enough
        SimImageIn_init(&image);

        ret = SimImageIn_loadFLX(parameters->sCapture[ctx].pszHDRInsInput, &image, 0);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to load 1st frame of HDR Insertion '%s'\n",
                parameters->sCapture[ctx].pszHDRInsInput);
            return IMG_ERROR_FATAL;
        }

        if (image.info.ui32Width != pImageInfo->ui32Width
            || image.info.ui32Height != pImageInfo->ui32Height)
        {
            LOG_ERROR("Provided HDR Insertion image is %dx%d while image "\
                "provided to DG is %dx%d - they should be of the same size\n",
                image.info.ui32Width, image.info.ui32Height,
                pImageInfo->ui32Width, pImageInfo->ui32Height);
            SimImageIn_clean(&image);
            return IMG_ERROR_FATAL;
        }
            
        if (image.info.ui8BitDepth > 16)
        {
            LOG_ERROR("Provided HDR Insertion image has a bitdepth of "\
                "%d while the maximum is 16\n",
                image.info.ui8BitDepth);
            SimImageIn_clean(&image);
            return IMG_ERROR_FATAL;
        }

        SimImageIn_clean(&image);
    }
    if ( parameters->sCapture[ctx].pszRaw2ExtDOutput != NULL )
    {
        /* driver should fail to apply this configuration later if RAW2D is
         * not possible to allow testing */
        switch (parameters->sCapture[ctx].uiRaw2DBitDepth)
        {
        case 10:
            sPipelineConfig.eRaw2DExtOutput = BAYER_TIFF_10;
            LOG_INFO("ctx %d RAW2D Ext TIFF10\n", ctx);
            break;
        case 12:
            sPipelineConfig.eRaw2DExtOutput = BAYER_TIFF_12;
            LOG_INFO("ctx %d RAW2D Ext TIFF12\n", ctx);
            break;
        default:
            LOG_ERROR("TIFF bitdepth %d not supported - RAW 2D disabled\n",
                parameters->sCapture[ctx].uiRaw2DBitDepth);
            return IMG_ERROR_FATAL;
        }
    }

#ifndef RLT_NOT_AVAILABLE
    // RLT config
    if ( parameters->sCapture[ctx].ui32RLTMode == CI_RLT_LINEAR )
    {
        int p, i, s, c;
        const IMG_UINT32 nBits = RLT_LUT_0_POINTS_ODD_1_TO_15_INT+RLT_LUT_0_POINTS_ODD_1_TO_15_FRAC;
        const IMG_UINT32 multiplier = (1<<(nBits));
        IMG_UINT32 pos=0;

        // v = (pos - a)*slope[n] + b
        double a[RLT_SLICE_N];
        double b[RLT_SLICE_N];
        double slope[RLT_SLICE_N];
        IMG_UINT32 knee[RLT_SLICE_N];

        sPipelineConfig.sRLT.eMode = CI_RLT_LINEAR;
        
        LOG_INFO("computing linear Raw Look Up Table\n");
        for ( i = 0 ; i < RLT_SLICE_N-1 ; i++ )
        {
            knee[i] = parameters->sCapture[ctx].aRLTKnee[i];
            for ( s = i+1 ; s < RLT_SLICE_N-1 ; s++ )
            {
                if(parameters->sCapture[ctx].aRLTKnee[s]<parameters->sCapture[ctx].aRLTKnee[i])
                {
                    LOG_ERROR("given RLT knee point %d=%d < %d=%d is not valid\n",
                        s, parameters->sCapture[ctx].aRLTKnee[s], i, parameters->sCapture[ctx].aRLTKnee[i]);
                    return IMG_ERROR_FATAL;
                }
            }
        }
        knee[RLT_SLICE_N-1] = multiplier+1; // +1 because we compute with p+1 which make it go just at the limit
                
        a[0] = 0.0;
        b[0] = 0.0;
        slope[0] = parameters->sCapture[ctx].aRLTSlopes[0]*multiplier;

        a[1] = knee[0]/(double)multiplier;
        b[1] = (a[1]-a[0])*slope[0] + b[0];
        slope[1] = parameters->sCapture[ctx].aRLTSlopes[1]*multiplier;

        a[2] = knee[1]/(double)multiplier;
        b[2] = (a[2]-a[1])*slope[1] + b[1];
        slope[2] = parameters->sCapture[ctx].aRLTSlopes[2]*multiplier;

        a[3] = knee[2]/(double)multiplier;
        b[3] = (a[3]-a[2])*slope[2] + b[2];
        slope[3] = parameters->sCapture[ctx].aRLTSlopes[3]*multiplier;

        i = 0;
        s = 0;
        p = 0;
        pos = (IMG_UINT32)floor((p+1.0)/RLT_N_POINTS*multiplier);
        for ( c = 0 ; c < RLT_SLICE_N ; c++ )
        {
            while( p < RLT_N_POINTS && pos < knee[c] )
            {
                s = p/RLT_SLICE_N_POINTS;
                i = p - s*RLT_SLICE_N_POINTS;

                sPipelineConfig.sRLT.aPoints[s][i] = (IMG_UINT16)IMG_clip(
                    (((p+1.0)/RLT_N_POINTS)-a[c])*slope[c] + b[c],
                    nBits, IMG_FALSE, "RLT_point");
#if 0
                LOG_DEBUG("RLT [%d][%d] %u -> %u - c=%d\n",
                    s, i, (unsigned)(((p+1.0)/RLT_N_POINTS)*multiplier), (unsigned)sPipelineConfig.sRLT.aPoints[s][i], c);
#endif

                p++;
                pos = (IMG_UINT32)floor((p+1.0)/RLT_N_POINTS*multiplier);
            }
        }
        if ( p != RLT_N_POINTS ) // unlikely
        {
            LOG_ERROR("Given RLT knee points did not configure all points!\n");
            return IMG_ERROR_FATAL;
        }
    }
    else if ( parameters->sCapture[ctx].ui32RLTMode == CI_RLT_CUBIC )
    {
        int i, s;
        const IMG_UINT32 nBits = RLT_LUT_0_POINTS_ODD_1_TO_15_INT+RLT_LUT_0_POINTS_ODD_1_TO_15_FRAC;
        const IMG_UINT32 multiplier = (1<<(nBits));
        sPipelineConfig.sRLT.eMode = CI_RLT_CUBIC;

        LOG_INFO("computing cubic Raw Look Up Table\n");

        for ( s = 0 ; s < RLT_SLICE_N ; s++ )
        {
#if 0
            LOG_DEBUG("RLT channel %d (%f*(x/%d))\n", 
                s, parameters->sCapture[ctx].aRLTSlopes[s], RLT_SLICE_N_POINTS);
#endif

            for ( i = 0 ; i < RLT_SLICE_N_POINTS ; i++ )
            {
                sPipelineConfig.sRLT.aPoints[s][i] = (IMG_UINT16)IMG_clip(
                    (((i+1.0)/RLT_SLICE_N_POINTS)*parameters->sCapture[ctx].aRLTSlopes[s]*multiplier),
                    nBits, IMG_FALSE, "RLT_point");
#if 0
                LOG_DEBUG("RLT [%d][%d] = %u\n", s, i, (unsigned)sPipelineConfig.sRLT.aPoints[s][i]);
#endif
            }
        }
    }
    else
#endif //RLT_NOT_AVAILABLE
    {
        sPipelineConfig.sRLT.eMode = CI_RLT_DISABLED; // it is the default but at least it's obvious here
    }

    if (sPipelineConfig.sRLT.eMode != CI_RLT_DISABLED)
    {
        int s, i;
        char name[64];
        FILE *f = NULL;

        sprintf(name, "ctx%d_rlt_curve.txt", ctx);
        f = fopen(name, "w");
        if (f)
        {
            if (sPipelineConfig.sRLT.eMode==CI_RLT_LINEAR)
            {
                fprintf(f, "mode: linear\ncurve:");
                for ( s = 0 ; s < RLT_SLICE_N ; s++ )
                {
                    for ( i = 0 ; i < RLT_SLICE_N_POINTS ; i++ )
                    {
                        fprintf(f, " %u", sPipelineConfig.sRLT.aPoints[s][i]);
                    }
                }
            }
            else
            {
                fprintf(f, "mode: cubic");
                for ( s = 0 ; s < RLT_SLICE_N ; s++ )
                {
                    fprintf(f, "\nchannel %d:", s);
                    for ( i = 0 ; i < RLT_SLICE_N_POINTS ; i++ )
                    {
                        fprintf(f, " %u", sPipelineConfig.sRLT.aPoints[s][i]);
                    }
                }
            }
            fclose(f);
        }
        else
        {
            LOG_WARNING("failed to open '%s' to write rlt curve used\n", name);
        }
    }
    
    // LSH config
    sPipelineConfig.sLSH.bUseDeshadingGrid = parameters->sCapture[ctx].iUseDeshading > 0 ? IMG_TRUE : IMG_FALSE;
    if (parameters->sCapture[ctx].iUseDeshading > 0)
    {
        int c;
        IMG_BOOL8 bComputeLSH = IMG_TRUE;
        unsigned int tileSize = parameters->sCapture[ctx].uiLshTile;

        while (bComputeLSH)
        {
            if (tileSize > LSH_TILE_MAX || tileSize < LSH_TILE_MIN)
            {
                LOG_ERROR("LSH tile size %u is not correct, must be between %u and %u\n",
                    parameters->sCapture[ctx].uiLshTile,
                    LSH_TILE_MIN, LSH_TILE_MAX);
                return IMG_ERROR_FATAL;
            }

            ret = LSH_CreateMatrix(&(sPipelineConfig.sLSH.sGrid),
                pImageInfo->ui32Width / CI_CFA_WIDTH,
                pImageInfo->ui32Height / CI_CFA_HEIGHT,
                tileSize);
            if (ret)
            {
                LOG_ERROR("Failed to create LSH grid\n");
                return IMG_ERROR_FATAL;
            }

            if (parameters->sCapture[ctx].iUseDeshading == 1)
            {
                for (c = 0; c < LSH_GRADS_NO; c++)
                {
                    LSH_FillLinear(&(sPipelineConfig.sLSH.sGrid), c,
                        &(parameters->sCapture[ctx].lshCorners[c*LSH_N_CORNERS]));
                }
            }
            else
            {
                for (c = 0; c < LSH_GRADS_NO; c++)
                {
                    LSH_FillBowl(&(sPipelineConfig.sLSH.sGrid), c,
                        &(parameters->sCapture[ctx].lshCorners[c*LSH_N_CORNERS]));
                }
            }
            bComputeLSH = IMG_FALSE;

            if (parameters->sCapture[ctx].bFixLSH[0]
                && parameters->sCapture[ctx].iUseDeshading > 0)
            {
                ret = MC_LSHPreCheckTest(&sPipelineConfig);
                if (IMG_ERROR_NOT_SUPPORTED == ret &&
                    parameters->sCapture[ctx].bFixLSH[1])
                {
                    // only if we are allowed to change the tile size
                    bComputeLSH = IMG_TRUE;

                    LSH_Free(&(sPipelineConfig.sLSH.sGrid));
                    tileSize *= 2;
                    LOG_WARNING("Trying to recompute LSH with tile size of "\
                        "%u instead of %u to fit HW size limitation\n",
                        tileSize, parameters->sCapture[ctx].uiLshTile);
                    ret = IMG_SUCCESS;
                }
                // no else as it may be forbidden to change the tile size
                if (ret)
                {
                    return IMG_ERROR_FATAL;
                }
            }
        }

        {
            char tmp[64];
            sprintf(tmp, "lsh_matrix_ctx%d.txt", ctx);
            LSH_Save_txt(&(sPipelineConfig.sLSH.sGrid), tmp);

            sprintf(tmp, "lsh_matrix_ctx%d.dat", ctx);
            LSH_Save_bin(&(sPipelineConfig.sLSH.sGrid), tmp);
        }
    }
    {
        int i;
        for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
        {
            sPipelineConfig.sLSH.aGradients[i][0] = parameters->sCapture[ctx].lshGradients[i*2+0];
            sPipelineConfig.sLSH.aGradients[i][1] = parameters->sCapture[ctx].lshGradients[i*2+1];
        }
    }

    // TNM config
    while ( pImageInfo->ui32Width/sPipelineConfig.sTNM.ui16LocalColumns < TNM_MIN_COL_WIDTH )
    {
        if ( sPipelineConfig.sTNM.ui16LocalColumns == 1 )
        {
            LOG_ERROR("The input image is too small: the TNM cannot have a minimum column size of %u pixels\n", TNM_MIN_COL_WIDTH);
            return IMG_ERROR_FATAL;
        }
        sPipelineConfig.sTNM.ui16LocalColumns--;
    }

    if ( sPipelineConfig.sLSH.bUseDeshadingGrid == IMG_TRUE )
    {
        char tmp[64];

        // dump the matrix as an image
        sprintf(tmp, "lsh_matrix_ctx%d.pgm", ctx);
        LSH_Save_pgm(&(sPipelineConfig.sLSH.sGrid), tmp);
    }

    if ( (ret=MC_PipelineConvert(&sPipelineConfig, pPipeline)) != IMG_SUCCESS )
    {
        LOG_ERROR("failed to convert the pipeline as bypass!\n");
        LSH_Free(&(sPipelineConfig.sLSH.sGrid));
        return ret;
    }

    if ( sPipelineConfig.sLSH.bUseDeshadingGrid == IMG_TRUE )
    {
        LOG_INFO("LSH grid uses %u bits per difference\n", pPipeline->sDeshading.ui8BitsPerDiff);
    }

    // override system black level
    pPipeline->ui16SysBlackLevel = parameters->sCapture[ctx].uiSysBlack;

    // choose which context is using the encoder output line
    if ( parameters->ui8EncOutSelect == ctx )
    {
        pPipeline->bUseEncOutLine = IMG_TRUE;
    }

    if ( parameters->sCapture[ctx].ui32NEncTiled + parameters->sCapture[ctx].ui32NDispTiled + parameters->sCapture[ctx].ui32NHDRExtTiled > 0 )
    {
        if ( pHWInfo->eFunctionalities&CI_INFO_SUPPORTED_TILING )
        {
            LOG_INFO("Pipeline %d uses tiling\n", ctx);
            pPipeline->bSupportTiling = IMG_TRUE;
        }
        else
        {
            LOG_WARNING("HW %d.%d does not support tiling! Tiling not enabled\n",
                pHWInfo->rev_ui8Major, pHWInfo->rev_ui8Minor);
        }
    }

    if ( (ret=CI_PipelineRegister(pPipeline) != IMG_SUCCESS) )
    {
        LOG_ERROR("failed to register the pipeline configuration\n");
        LSH_Free(&(sPipelineConfig.sLSH.sGrid));
        return ret;
    }


    if ( (ret=CI_PipelineAddPool(pPipeline, parameters->sCapture[ctx].ui32ConfigBuffers)) != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to allocate the Shots\n");
        LSH_Free(&(sPipelineConfig.sLSH.sGrid));

        return ret;

    }

    LSH_Free(&(sPipelineConfig.sLSH.sGrid));
    MC_DPFFree(&(sPipelineConfig.sDPF));
    return ret;
}

static void CaptureRunClean(struct captureRun *pRunInfo)
{
    SimImageIn_clean(&(pRunInfo->sCurrentFrame));
    SimImageIn_clean(&(pRunInfo->sHDRIns));

    SaveFile_close(&(pRunInfo->encoderOutput));
    SaveFile_destroy(&(pRunInfo->encoderOutput));

    SaveFile_close(&(pRunInfo->displayOutput));
    SaveFile_destroy(&(pRunInfo->displayOutput));

    SaveFile_close(&(pRunInfo->hdrExtOutput));
    SaveFile_destroy(&(pRunInfo->hdrExtOutput));

    SaveFile_close(&(pRunInfo->raw2dOutput));
    SaveFile_destroy(&(pRunInfo->raw2dOutput));

    SaveFile_close(&(pRunInfo->statsOutput));
    SaveFile_destroy(&(pRunInfo->statsOutput));
}

static int CaptureRunInit(struct captureRun *pRunInfo, struct CMD_PARAM *parameters, CI_PIPELINE *pPipeline, IMG_UINT32 dg, CI_CONNECTION *pConnection)
{
    IMG_UINT32 ctx;
    char *pszInputFLX = NULL;

    IMG_MEMSET(pRunInfo, 0, sizeof(struct captureRun));
    if ( pPipeline == NULL )
    {
        return EXIT_SUCCESS;
    }
    ctx = pPipeline->ui8Context;

    if ( SimImageIn_init(&(pRunInfo->sCurrentFrame)) != IMG_SUCCESS )
    {
        LOG_ERROR("failed to initialise CTX %d DG image\n", ctx);
        return EXIT_FAILURE;
    }

    if (SimImageIn_init(&(pRunInfo->sHDRIns)) != IMG_SUCCESS)
    {
        LOG_ERROR("failed to initialise CTX %d HDR ins image\n", ctx);
        return EXIT_FAILURE;
    }

    // compute "window" frames as the minimum value between the whole video and the number of available buffers
    pRunInfo->framesToCapture = parameters->sCapture[ctx].ui32RunForFrames;
    pRunInfo->window = IMG_MIN_INT(pRunInfo->framesToCapture, parameters->sCapture[ctx].ui32ConfigBuffers);
    // take the max submit to HW into account otherwise we will lockup because we don't trigger the DG
    pRunInfo->window = IMG_MIN_INT(pRunInfo->window, pConnection->sHWInfo.context_aPendingQueue[pPipeline->ui8Context]);
    pRunInfo->maxInputFrames = parameters->sDataGen[dg].ui32Frames;
    pRunInfo->uiRegOverride = parameters->sCapture[ctx].uiRegOverride;
    pRunInfo->pConnection = pConnection;
    pRunInfo->bWriteDGFrame = parameters->bWriteDGFrame;
    pRunInfo->bOverrideDPF = parameters->bIgnoreDPF;
    pRunInfo->bDPFEnabled = !parameters->bNowrite && !parameters->bIgnoreDPF;
    pRunInfo->bENSEnabled = !parameters->bNowrite;
    pRunInfo->bRawHDRExt = parameters->sCapture[ctx].bAddRawHDR;
    pRunInfo->bRawYUV = parameters->sCapture[ctx].bRawYUV;
    pRunInfo->bRawDisp = parameters->sCapture[ctx].bRawDisp;
    pRunInfo->bRawTiff = parameters->sCapture[ctx].brawTiff;
    pRunInfo->bEnableTimestamps = parameters->sCapture[ctx].bEnableTimestamps;
    pRunInfo->pszHDRInsFile = parameters->sCapture[ctx].pszHDRInsInput;
    pRunInfo->bRescaleHDRIns = parameters->sCapture[ctx].bRescaleHDRIns;
    pRunInfo->iIIFDG = parameters->sCapture[ctx].iIIFDG;

    // strides
    pRunInfo->encStrideY = parameters->sCapture[ctx].uiYUVStrides[0];
    pRunInfo->encStrideC = parameters->sCapture[ctx].uiYUVStrides[1];
    pRunInfo->encOffsetY = parameters->sCapture[ctx].uiYUVOffsets[0];
    pRunInfo->encOffsetC = parameters->sCapture[ctx].uiYUVOffsets[1];

    pRunInfo->dispStride = parameters->sCapture[ctx].uiDispStride;
    pRunInfo->dispOffset = parameters->sCapture[ctx].uiDispOffset;

    pRunInfo->HDRExtStride = parameters->sCapture[ctx].uiHDRExtStride;
    pRunInfo->HDRExtOffset = parameters->sCapture[ctx].uiHDRExtOffset;

    //pRunInfo->HDRInsStride = parameters->sCapture[ctx].uiHDRInsStride;
    //pRunInfo->HDRInsOffset = parameters->sCapture[ctx].uiHDRInsOffset;

    pRunInfo->raw2DStride = parameters->sCapture[ctx].uiRaw2DStride;
    pRunInfo->raw2DOffset = parameters->sCapture[ctx].uiRaw2DOffset;

    IMG_MEMSET(&(pRunInfo->sConverter), 0, sizeof(CI_DG_CONVERTER));
    
    LOG_INFO ("---\nCTX %d is running for %d frames (window is %d)\n---\n", ctx, pRunInfo->framesToCapture, pRunInfo->window);
    
    SaveFile_init(&(pRunInfo->encoderOutput));
    SaveFile_init(&(pRunInfo->displayOutput));
    SaveFile_init(&(pRunInfo->hdrExtOutput));
    SaveFile_init(&(pRunInfo->raw2dOutput));
    SaveFile_init(&(pRunInfo->statsOutput));

    if ( parameters->sCapture[ctx].pszEncoderOutput != NULL && parameters->bNowrite == IMG_FALSE )
    {
        size_t len = strlen(parameters->sCapture[ctx].pszEncoderOutput);

        if ( len > 4 &&
             parameters->sCapture[ctx].pszEncoderOutput[len-4] != '.' &&
             parameters->sCapture[ctx].pszEncoderOutput[len-3] != 'y' &&
             parameters->sCapture[ctx].pszEncoderOutput[len-2] != 'u' &&
             parameters->sCapture[ctx].pszEncoderOutput[len-1] != 'v' )
        {
            const char *fmt = FormatString(pPipeline->eEncType.eFmt); // format is always VU in felix
            char *filename = (char*)IMG_CALLOC(len+32, 1); // needs 25 = '-' + 4 digit + 'x' + 4 digits + '-align' + 4 digit + ".yuv" + '\0'
            int align = pPipeline->eEncType.ui8PackedStride;

            if ( filename == NULL )
            {
                LOG_ERROR("failed to allocate copy of the filename!\n");
                goto error_init;//return EXIT_FAILURE;
            }
            IMG_STRNCPY(filename, parameters->sCapture[ctx].pszEncoderOutput, len);

            sprintf(filename, "%s-%dx%d-%s-align%d.yuv", filename,
                    pPipeline->sEncoderScaler.aOutputSize[0]*2 +2, pPipeline->sEncoderScaler.aOutputSize[1]+1,
                fmt,
                align
                );

            if ( SaveFile_open(&(pRunInfo->encoderOutput), filename) != IMG_SUCCESS )
            {
                IMG_FREE(filename);
                goto error_init;//return EXIT_FAILURE;
            }

            IMG_FREE(filename);
        }
        else
        {
            if ( SaveFile_open(&(pRunInfo->encoderOutput), parameters->sCapture[ctx].pszEncoderOutput) != IMG_SUCCESS )
            {
                goto error_init;//return EXIT_FAILURE;
            }
        }
    }
    if ( parameters->sCapture[ctx].pszDisplayOutput != NULL && parameters->bNowrite == IMG_FALSE )
    {
        struct SimImageInfo imageInfo;
        imageInfo.eColourModel = SimImage_RGB32;

        imageInfo.ui32Width = (pPipeline->sDisplayScaler.aOutputSize[0]*2 +2);
        imageInfo.ui32Height = pPipeline->sDisplayScaler.aOutputSize[1]+1;

        if ( imageInfo.ui32Width*imageInfo.ui32Height == 0 )
        {
            LOG_ERROR("display image size is 0\n");
            goto error_init;//return EXIT_FAILURE;
        }

        // RGB
        imageInfo.stride = imageInfo.ui32Width*pPipeline->eDispType.ui8PackedStride; // 3 bytes per pixel
        imageInfo.ui8BitDepth = pPipeline->eDispType.ui8BitDepth;
        
        if ( pPipeline->eDispType.ui8PackedStride == 3 ) // RGB888 24b
        {
            imageInfo.eColourModel = SimImage_RGB24;
        }
        imageInfo.isBGR = IMG_FALSE;
        if ( SaveFile_openFLX(&(pRunInfo->displayOutput), parameters->sCapture[ctx].pszDisplayOutput, &imageInfo) != IMG_SUCCESS )
        {
            goto error_init;//return EXIT_FAILURE;
        }
    }
    else if ( parameters->sCapture[ctx].pszDataExtOutput != NULL && parameters->bNowrite == IMG_FALSE )
    {
        struct SimImageInfo imageInfo;

        imageInfo.ui32Width = pPipeline->ui16MaxDispOutWidth;
        imageInfo.ui32Height = pPipeline->ui16MaxDispOutHeight;
        switch(pPipeline->sImagerInterface.eBayerFormat)
        {
        case MOSAIC_RGGB:
            imageInfo.eColourModel = SimImage_RGGB;
            break;
        case MOSAIC_GRBG:
            imageInfo.eColourModel = SimImage_GRBG;
            break;
        case MOSAIC_GBRG:
            imageInfo.eColourModel = SimImage_GBRG;
            break;
        case MOSAIC_BGGR:
            imageInfo.eColourModel = SimImage_BGGR;
            break;
        }

        // data extraction
        imageInfo.stride = imageInfo.ui32Width*8; /// 4*2 bytes per pixel - @ is this even correct?
        imageInfo.ui8BitDepth = pPipeline->eDispType.ui8BitDepth;

        if ( imageInfo.stride*imageInfo.ui32Height == 0 )
        {
            LOG_ERROR("DE image size is 0\n");
            goto error_init;//return EXIT_FAILURE;
        }

        if ( SaveFile_openFLX(&(pRunInfo->displayOutput), parameters->sCapture[ctx].pszDataExtOutput, &imageInfo) != IMG_SUCCESS )
        {
            goto error_init;//return EXIT_FAILURE;
        }
    }
    if ( parameters->sCapture[ctx].pszHDRextOutput != NULL && parameters->bNowrite == IMG_FALSE )
    {
        struct SimImageInfo imageInfo;
        imageInfo.eColourModel = SimImage_RGB32;

        imageInfo.ui32Width = (pPipeline->sImagerInterface.ui16ImagerSize[0]+1)*2;
        imageInfo.ui32Height = (pPipeline->sImagerInterface.ui16ImagerSize[1]+1)*2;

        if ( imageInfo.ui32Width*imageInfo.ui32Height == 0 )
        {
            LOG_ERROR("display image size is 0\n");
            goto error_init;//return EXIT_FAILURE;
        }

        // RGB
        imageInfo.stride = imageInfo.ui32Width*pPipeline->eHDRExtType.ui8PackedStride; // 3 bytes per pixel
        imageInfo.ui8BitDepth = pPipeline->eHDRExtType.ui8BitDepth;
        
        if ( pPipeline->eHDRExtType.ui8PackedStride == 3 ) // RGB888 24b
        {
            imageInfo.eColourModel = SimImage_RGB24;
        }
        imageInfo.isBGR = IMG_TRUE;
        if ( SaveFile_openFLX(&(pRunInfo->hdrExtOutput), parameters->sCapture[ctx].pszHDRextOutput, &imageInfo) != IMG_SUCCESS )
        {
            goto error_init;//return EXIT_FAILURE;
        }
    }
    if ( parameters->sCapture[ctx].pszRaw2ExtDOutput != NULL && parameters->bNowrite == IMG_FALSE )
    {
        struct SimImageInfo imageInfo;

        imageInfo.ui32Width = (pPipeline->sImagerInterface.ui16ImagerSize[0]+1)*2;
        imageInfo.ui32Height = (pPipeline->sImagerInterface.ui16ImagerSize[1]+1)*2;

        switch(pPipeline->sImagerInterface.eBayerFormat)
        {
        case MOSAIC_RGGB:
            imageInfo.eColourModel = SimImage_RGGB;
            break;
        case MOSAIC_GRBG:
            imageInfo.eColourModel = SimImage_GRBG;
            break;
        case MOSAIC_GBRG:
            imageInfo.eColourModel = SimImage_GBRG;
            break;
        case MOSAIC_BGGR:
            imageInfo.eColourModel = SimImage_BGGR;
            break;
        }

        // data extraction
        imageInfo.stride = imageInfo.ui32Width*sizeof(IMG_UINT16);
        imageInfo.ui8BitDepth = pPipeline->eRaw2DExtraction.ui8BitDepth;

        if ( imageInfo.stride*imageInfo.ui32Height == 0 )
        {
            LOG_ERROR("RAW2D image size is 0\n");
            goto error_init;//return EXIT_FAILURE;
        }

        if ( SaveFile_openFLX(&(pRunInfo->raw2dOutput), parameters->sCapture[ctx].pszRaw2ExtDOutput, &imageInfo) != IMG_SUCCESS )
        {
            goto error_init;//return EXIT_FAILURE;
        }
    }
    if ( (parameters->sCapture[ctx].iStats != 0 || parameters->sCapture[ctx].iDPFEnabled > 1) && parameters->bNowrite == IMG_FALSE )
    {
        IMG_CHAR statsName[64];

        sprintf(statsName, "ctx_%d_stats.dat", ctx);
        if ( SaveFile_open(&(pRunInfo->statsOutput), statsName) != IMG_SUCCESS )
        {
            goto error_init;//return EXIT_FAILURE;
        }
    }

    // load the 1st frame
    if ( pRunInfo->iIIFDG >= 0 )
    {
        pszInputFLX = parameters->sIIFDG[pRunInfo->iIIFDG].pszInputFLX;
    }
    else
    {
        pszInputFLX = parameters->sDataGen[dg].pszInputFLX;
    }

    if ( SimImageIn_loadFLX(pszInputFLX, &(pRunInfo->sCurrentFrame), 0) != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to pre-load the 1st frame from '%s'\n", parameters->sDataGen[dg].pszInputFLX);
        goto error_init;
    }

    if ( parameters->sCapture[ctx].iIIFDG >= 0 )
    {
        IMG_UINT8 gasketBit = pConnection->sHWInfo.gasket_aBitDepth[parameters->sCapture[ctx].ui8imager];
        if (pRunInfo->sCurrentFrame.info.ui8BitDepth != gasketBit)
        {
            LOG_WARNING("Loading a %d bits image for a %d bit gasket may produce unexpected results!\n",
                pRunInfo->sCurrentFrame.info.ui8BitDepth, gasketBit);
        }
        if (gasketBit <= 10 )
        {
            CI_ConverterConfigure(&(pRunInfo->sConverter), CI_DGFMT_PARALLEL_10);
        }
        else
        {
            CI_ConverterConfigure(&(pRunInfo->sConverter), CI_DGFMT_PARALLEL_12);
        }
    }

    // load 1st frame of HDR Insertion to get number of frames
    if (pRunInfo->pszHDRInsFile)
    {
        IMG_RESULT ret;

        ret = SimImageIn_loadFLX(pRunInfo->pszHDRInsFile, &(pRunInfo->sHDRIns), 0);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to load first frame of HDR Insertion\n");
            goto error_init;
        }
    }

    return EXIT_SUCCESS;

error_init:
    CaptureRunClean(pRunInfo);
    return EXIT_FAILURE;
}

static int CaptureShoot(struct captureRun *pRunInfo, CI_PIPELINE *pCapture,
    int f)
{
    IMG_RESULT ret;
    int ctx = pCapture->ui8Context;
    CI_BUFFID ids;
    CI_BUFFER_INFO info;

    /*
     * Could simply go for the following code but we want to be able to do
     * HDR insertion and change some of the strides therefore we have to
     * search for first available buffers to use trigger specified shot

     ret = CI_PipelineTriggerShoot(pCapture);
     if (IMG_SUCCESS != ret)
     {
     LOG_ERROR("CTX %d could not trigger a shoot\n", ctx);
     return EXIT_FAILURE;
     }
     */

    ret = CI_PipelineFindFirstAvailable(pCapture, &ids);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("CTX %d failed to find available buffers\n", ctx);
        return EXIT_FAILURE;
    }

    if (pCapture->eHDRInsType.eFmt != PXL_NONE)
    {
        CI_BUFFER HDRInsBuffer;
        int HDRFrame = (pRunInfo->nFrames + f) % (pRunInfo->sHDRIns.nFrames);

        ret = CI_PipelineAcquireHDRBuffer(pCapture, &HDRInsBuffer, 0);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("CTX %d failed to find available HDR Insertion buffer\n",
                ctx);
            return EXIT_FAILURE;
        }

        ids.idHDRIns = HDRInsBuffer.id;

        // now convert HDR insertion
        ret = SimImageIn_loadFLX(pRunInfo->pszHDRInsFile,
            &(pRunInfo->sHDRIns), HDRFrame);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("CTX %d failed to load frame %d from HDR "\
                "Insertion FLX\n", ctx, HDRFrame);
            return EXIT_FAILURE;
        }

        // could use pRunInfo->HDRInsStride

        ret = CI_Convert_HDRInsertion(&(pRunInfo->sHDRIns), &HDRInsBuffer,
            pRunInfo->bRescaleHDRIns);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("CTX %d failed to convert frame %d from HDR "\
                "Insertion FLX (rescale=%d)",
                ctx, HDRFrame, (int)pRunInfo->bRescaleHDRIns);
            return EXIT_FAILURE;
        }
    }

    //
    // warning if any buffer is tiled this will create problems and triggering
    // will fail therefore we check each buffer
    //

    if (ids.encId)
    {
        ret = CI_PipelineGetBufferInfo(pCapture, ids.encId, &info);
        if (ret)
        {
            LOG_ERROR("failed to get encoder buffer information!\n");
            return EXIT_FAILURE;
        }

        if (!info.bIsTiled)
        {
            ids.encStrideY = pRunInfo->encStrideY;
            ids.encStrideC = pRunInfo->encStrideC;
            ids.encOffsetY = pRunInfo->encOffsetY;
            ids.encOffsetC = pRunInfo->encOffsetC;
        }
    }

    if (ids.dispId)
    {
        ret = CI_PipelineGetBufferInfo(pCapture, ids.dispId, &info);
        if (ret)
        {
            LOG_ERROR("failed to get encoder buffer information!\n");
            return EXIT_FAILURE;
        }

        if (!info.bIsTiled)
        {
            ids.dispStride = pRunInfo->dispStride;
            ids.dispOffset = pRunInfo->dispOffset;
        }
    }

    if (ids.idHDRExt)
    {
        ret = CI_PipelineGetBufferInfo(pCapture, ids.idHDRExt, &info);
        if (ret)
        {
            LOG_ERROR("failed to get encoder buffer information!\n");
            return EXIT_FAILURE;
        }

        if (!info.bIsTiled)
        {
            ids.HDRExtStride = pRunInfo->HDRExtStride;
            ids.HDRExtOffset = pRunInfo->HDRExtOffset;
        }
    }

    // cannot be tiled so it does not matter
    ids.raw2DStride = pRunInfo->raw2DStride;
    ids.raw2DOffset = pRunInfo->raw2DOffset;

    ret = CI_PipelineTriggerSpecifiedShoot(pCapture, &ids);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("CTX %d failed to trigger a shoot\n", ctx);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int CaptureRunShoot(struct captureRun *pRunInfo, CI_PIPELINE *pCapture, struct datagen *pDatagen, const char *pszInputFLX)
{
    IMG_UINT32 ctx = pCapture->ui8Context;
    int gasket = pCapture->sImagerInterface.ui8Imager;
    char name[64]; // used for DG tmp file output

    // otherwise we will lockup waiting for an available buffer because we don't submit to DG
    IMG_ASSERT(pRunInfo->computing <= pRunInfo->pConnection->sHWInfo.context_aPendingQueue[ctx]); 

    // still has frames to compute
    if ( pRunInfo->computing > 0 )
    {
        IMG_UINT32 f;
        IMG_RESULT ret;

        LOG_INFO("CTX %d compute from %d to %d\n", ctx, pRunInfo->nFrames, (pRunInfo->nFrames)+pRunInfo->computing-1);

        // 1.1 loop to submit shot to Felix
        for ( f = 0 ; f < pRunInfo->computing && pDatagen != NULL ; f++ )
        {

#ifdef CI_DEBUG_FCT
            if ( pRunInfo->uiRegOverride > 0 )
            {
                // check if a register overwrite function exists
                FILE *file = NULL;
                IMG_CHAR name[64];
                IMG_RESULT ret;
                IMG_UINT frame = pRunInfo->nFrames+f;

                if ( pRunInfo->uiRegOverride > 1 )
                {
                    frame = frame % pRunInfo->uiRegOverride;
                }

                sprintf(name, REGOVER_FMT, ctx, frame); // adding current expected frame?
                if ( (file=fopen(name, "r")) != NULL )
                {
                    LOG_INFO("-- ctx %d uses %s as register override\n", ctx, name);
                    // file exists
                    fclose(file);
                    if ( (ret=CI_DebugSetRegisterOverride(ctx, name)) != IMG_SUCCESS )
                    {
                        LOG_ERROR("Failed to use the register overwrite file '%s' (returned %d)\n", name, ret);
                    }
                    else
                    {
                        int failed = CI_UPD_NONE;

                        if ( CI_PipelineUpdate(pCapture, (CI_UPD_ALL &(~CI_UPD_REG)), &failed) != IMG_SUCCESS )
                        {
                            LOG_ERROR("Failed to update the configuration using the register override - module %d failed\n", failed);
                        }
                    }
                }
            }
#endif
            if (CaptureShoot(pRunInfo, pCapture, f) != EXIT_SUCCESS)
            {
                return EXIT_FAILURE;
            }

#ifdef FELIX_FAKE
            CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixContext[ctx], "capture triggered");
#endif
            LOG_INFO("--- CTX %d FLX frame %d submitted ---\n", ctx, f);
        } // for f < compute

        // 1.2 loops to trigger DG
        for ( f = 0 ; f < pRunInfo->computing && pDatagen != NULL ; f++ )
        {
            // read from file - read only 1st frame if bNoUpdate is true
            if ( pszInputFLX != NULL )
            {
                if ( SimImageIn_loadFLX(pszInputFLX, &(pRunInfo->sCurrentFrame), pRunInfo->nFrames%pRunInfo->maxInputFrames) != IMG_SUCCESS )
                {
                    LOG_ERROR("CTX %d failed to load frame %d(mod[%d]=%d) from '%s'\n", ctx, pRunInfo->nFrames, pRunInfo->maxInputFrames, pRunInfo->nFrames%pRunInfo->maxInputFrames, pszInputFLX);
                    return EXIT_FAILURE;
                }
            }

            if ( pDatagen->bIsInternal )
            {
                CI_DG_FRAME *pFrame = CI_DatagenGetAvailableFrame(pDatagen->intDG);

                if ( !pFrame )
                {
                    LOG_ERROR("failed to get an available frame in ther IntDG\n");
                    return EXIT_FAILURE;
                }

                ret = CI_ConverterConvertFrame(&(pRunInfo->sConverter), &(pRunInfo->sCurrentFrame), pFrame);
                if ( IMG_SUCCESS != ret )
                {
                    CI_DatagenReleaseFrame(pFrame);
                    return EXIT_FAILURE;
                }

                pFrame->ui32HorizontalBlanking = pDatagen->ui32HBlanking;
                pFrame->ui32VerticalBlanking = pDatagen->ui32VBlanking;

                if ( pRunInfo->bWriteDGFrame )
                {
                    FILE *f=NULL;
                    sprintf(name, "intDG_frame_%d.dat", pRunInfo->nFrames);
                    
                    f = fopen(name, "wb");
                    if (f)
                    {
                        fwrite(pFrame->data, 1,
                            pFrame->ui32Stride*pFrame->ui32Height, f);
                        fclose(f);
                    }
                    else
                    {
                        LOG_WARNING("Failed to open tmp DG file '%s'\n",
                            name);
                    }
                }

                ret = CI_DatagenInsertFrame(pFrame);
                if ( IMG_SUCCESS != ret )
                {
                    LOG_ERROR("CTX %d IntDG%d on gasket %d failed to insert frame\n", ctx, pDatagen->intDG->ui8IIFDGIndex, gasket);
                    return EXIT_FAILURE;
                }
            }
#ifdef CI_EXT_DATAGEN
            else // if External DG not available should have failed to configure it!
            {
                char *pszName = NULL;
                
                if ( pRunInfo->bWriteDGFrame )
                {
                    sprintf(name, "extDG_frame_%d.dat", pRunInfo->nFrames);
                    pszName = name;
                }
                
                ret=DG_CameraShoot(pDatagen->extDG,
                    &(pRunInfo->sCurrentFrame), pszName);
                if ( IMG_SUCCESS != ret )
                {
                    LOG_ERROR("CTX %d DG %d failed to shoot\n", ctx, gasket);
                    return EXIT_FAILURE;
                }
            }
#endif
            
            LOG_INFO("--- CTX %d DG %d frame %d submitted ---\n", ctx, gasket, pRunInfo->nFrames);
            pRunInfo->nFrames++;
        } // for f < compute
        if ( pDatagen == NULL )
        {
            // update nFrames
            pRunInfo->nFrames+=pRunInfo->computing;
        }

        yield(FAKE_INTERRUPT_WAIT_MS);
    }

    return EXIT_SUCCESS;
}

static int CaptureRunWrite(struct captureRun *pRunInfo, CI_PIPELINE *pCapture,
    IMG_BOOL8 bBlocking)
{
    CI_SHOT *pBuffer = NULL;
    IMG_RESULT ret;

    yield(FAKE_INTERRUPT_WAIT_MS);

    if ( bBlocking == IMG_TRUE )
    {
        // if this context is waiting for frames
        if ( CI_PipelineHasWaiting(pCapture) == 0 )
        {
            LOG_INFO("--- no available buffer: will wait ---\n");
        }

        ret = CI_PipelineAcquireShot(pCapture, &pBuffer);
        if (ret) // blocking call
        {
            LOG_ERROR("failed to acquire a buffer with blocking call\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        ret = CI_PipelineAcquireShotNB(pCapture, &pBuffer);
        if (ret)
        {
            LOG_WARNING("failed to acquire a buffer with non-blocking call\n");
            return EXIT_FAILURE;
        }
    }

    // this acquires the buffer, uses it and releases it
    ret = WriteToFile(pBuffer, pCapture, pRunInfo);
    if (ret)
    {
        LOG_ERROR("failed to write a frame to file\n");
        return EXIT_FAILURE;
    }
    pRunInfo->computing--;
    //pRunInfo->nFrames++;

    // release buffer
    ret = CI_PipelineReleaseShot(pCapture, pBuffer);
    if (ret)
    {
        LOG_ERROR("failed to release the buffer\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int CaptureParallel(struct CMD_PARAM *parameters, CI_PIPELINE **apPipeline, struct datagen *aDatagen, CI_CONNECTION *pConnection)
{
    int ctx;
    int iRet = EXIT_FAILURE;
    struct captureRun runInfo[CI_N_CONTEXT];
    IMG_UINT32 uiContinueProcessing = 0; // nb of ctx that need frames to be computed
    IMG_UINT32 uiToWaitFor = 0;
    int gasketOwner[CI_N_EXT_DATAGEN]; // owner of DG, last CTX to use it so that the capture is trigger for both at once
    IMG_RESULT ret;
    char name[64]; // used for DG tmp file and DBG functions

    for ( ctx = 0 ; ctx < CI_N_EXT_DATAGEN ; ctx++ )
    {
        gasketOwner[ctx] = -1; // no owner
    }

    // setup loop
    for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
    {
        IMG_UINT32 gasket = 0;

        if (  apPipeline[ctx] != NULL )
        {
            gasket = apPipeline[ctx]->sImagerInterface.ui8Imager;
        }

        // if gasket is 0 because apCamera[ctx] is NULL it's ok because apPipeline[ctx] is NULL and the runInfo will be set-up to be bypassed
        if ( CaptureRunInit(&(runInfo[ctx]), parameters, apPipeline[ctx], gasket, pConnection) != EXIT_SUCCESS )
        {
            LOG_ERROR("CTX %d failed to init run info\n", ctx);
            goto prun_clean;
        }

        if ( apPipeline[ctx] != NULL )
        {
            if ( ctx > gasketOwner[gasket] ) // last ctx to use it is the owner - owner stores the image to give to DG
            {
                if ( gasketOwner[gasket] != -1 ) // if it had an owner we need to adapt the window otherwise when pushing more frames than expected the SIM dies instead of sending an interrupt ERROR_IGNORE
                {
                    runInfo[ctx].window = IMG_MIN_INT(runInfo[ctx].window, runInfo[gasketOwner[gasket]].window);
                    runInfo[gasketOwner[gasket]].window = runInfo[ctx].window;
                }
                gasketOwner[gasket] = ctx;
            }

            if ( CI_PipelineComputeLinestore(apPipeline[ctx]) != IMG_SUCCESS )
            {
                LOG_ERROR("CTX %d failed to compute linestore\n", ctx);
                goto prun_clean;
            }

            if ( CI_PipelineStartCapture(apPipeline[ctx]) != IMG_SUCCESS )
            {
                LOG_ERROR("CTX %d failed to start capture\n", ctx);
                goto prun_clean;
            }

            uiContinueProcessing++; // one more context need computation
        }
    }

    yield(FAKE_INTERRUPT_WAIT_MS);

    while( uiContinueProcessing > 0 )
    {
        IMG_UINT32 max_triggered = 0, max_gasket = 0;
        IMG_UINT32 f, gasket;
        IMG_UINT32 gasketToSubmit[CI_N_EXT_DATAGEN], gasketSubmitted[CI_N_EXT_DATAGEN];

        for ( gasket = 0 ; gasket < CI_N_EXT_DATAGEN ; gasket++ )
        {
            gasketToSubmit[gasket] = 0;
            gasketSubmitted[gasket] = 0;
        }

        // 1.1 submit on all contexts
        for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
        {
            // has frames left
            if ( apPipeline[ctx] != NULL && runInfo[ctx].nFrames < parameters->sCapture[ctx].ui32RunForFrames )
            {
                runInfo[ctx].computing = IMG_MIN_INT(runInfo[ctx].window, parameters->sCapture[ctx].ui32RunForFrames-runInfo[ctx].nFrames);
                for ( f = 0 ; f < runInfo[ctx].computing  ; f++ )
                {

                    if (parameters->sCapture[ctx].bAllocateLate)
                    {
                        LOG_INFO("--- CTX %d allocate buffers ---\n", ctx, f);
                        /* if first frame has not been computed yet we need
                        * to allocate!
                        * done that late to allocate while other pipeline may
                        * be running */
                        ret = AllocateBuffers(parameters, ctx, apPipeline[ctx], 1);
                        if (IMG_SUCCESS != ret)
                        {
                            LOG_ERROR("failed to allocate the buffers!\n");
                            goto prun_clean;
                        }
                    }

#ifdef CI_DEBUG_FCT
                    if ( runInfo[ctx].uiRegOverride > 0 )
                    {
                        // check if a register overwrite function exists
                        FILE *file = NULL;
                        IMG_RESULT ret;

                        sprintf(name, REGOVER_FMT, ctx, runInfo[ctx].nFrames);
                        if ( (file=fopen(name, "r")) != NULL )
                        {
                            // file exists
                            fclose(file);
                            LOG_INFO("-- ctx %d use %s as register override\n", ctx, name);
                            if ( (ret=CI_DebugSetRegisterOverride(ctx, name)) != IMG_SUCCESS )
                            {
                                LOG_ERROR("Failed to use the register overwrite file '%s' (returned %d)\n", name);
                            }
                        }
                    }
#endif
                    ret = CaptureShoot(&runInfo[ctx], apPipeline[ctx], f);
                    if (EXIT_SUCCESS != ret)
                    {
                        LOG_WARNING("Failed to submit frame %f for context %d", f, ctx);
                    }
                    
#ifdef FELIX_FAKE
                    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hRegFelixContext[ctx], "capture triggered");
#endif
                    LOG_INFO("--- CTX %d FLX frame %d submitted ---\n", ctx, f);
                }

                //runInfo[ctx].computing = runInfo[ctx].window;
                max_triggered = IMG_MAX_INT(max_triggered, runInfo[ctx].computing);
                gasket = apPipeline[ctx]->sImagerInterface.ui8Imager;
                gasketToSubmit[gasket] = IMG_MAX_INT(gasketToSubmit[gasket], runInfo[ctx].computing);
                max_gasket = IMG_MAX_INT(max_gasket, gasketToSubmit[gasket]);
            }
        }

        // 1.2 submit on all DG one frame at a time - requested by HW for the tests
        for ( f = 0 ; f < max_gasket ; f++ )
        {
            for ( gasket = 0 ; gasket < CI_N_EXT_DATAGEN ; gasket++ )
            {
                if ( gasketToSubmit[gasket] > gasketSubmitted[gasket] )
                {
                    int toLoad = 0;
                    IMG_UINT32 ui32Frames = 0;
                    const char *pszInputFLX = NULL;
                    ctx = gasketOwner[gasket];

                    if ( aDatagen[gasket].bIsInternal )
                    {
                        ui32Frames = parameters->sIIFDG[parameters->sCapture[ctx].iIIFDG].uiFrames;
                        pszInputFLX = parameters->sIIFDG[parameters->sCapture[ctx].iIIFDG].pszInputFLX;
                    }
#ifdef CI_EXT_DATAGEN
                    else // if External DG not available should have failed to configure it!
                    {
                        ui32Frames = parameters->sDataGen[gasket].ui32Frames;
                        pszInputFLX = parameters->sDataGen[gasket].pszInputFLX;
                    }
#endif
                    toLoad = gasketSubmitted[gasket]%ui32Frames;
                    
                    // load only if the input image has several frames
                    if ( ui32Frames > 1 )
                    {
                        if ( SimImageIn_loadFLX(pszInputFLX, &(runInfo[ctx].sCurrentFrame), toLoad) != IMG_SUCCESS )
                        {
                            LOG_ERROR("DG %d failed to load frame %d(mod[%d]=%d) from '%s'\n", gasket, gasketSubmitted[gasket], ui32Frames, toLoad, pszInputFLX);
                            iRet = EXIT_FAILURE;
                            goto prun_clean;
                        }
                    }

                    if ( aDatagen[gasket].bIsInternal )
                    {
                        CI_DG_FRAME *pFrame = CI_DatagenGetAvailableFrame(aDatagen[gasket].intDG);

                        if ( !pFrame )
                        {
                            LOG_ERROR("failed to get an available frame in ther IntDG\n");
                            iRet = EXIT_FAILURE;
                            goto prun_clean;
                        }

                        ret = CI_ConverterConvertFrame(&(runInfo[ctx].sConverter), &(runInfo[ctx].sCurrentFrame), pFrame);
                        if ( IMG_SUCCESS != ret )
                        {
                            CI_DatagenReleaseFrame(pFrame);
                            iRet = EXIT_FAILURE;
                            goto prun_clean;
                        }

                        pFrame->ui32HorizontalBlanking = aDatagen[gasket].ui32HBlanking;
                        pFrame->ui32VerticalBlanking = aDatagen[gasket].ui32VBlanking;

                        if ( runInfo[ctx].bWriteDGFrame )
                        {
                            FILE *f=NULL;
                            sprintf(name, "intDG_frame_%d.dat", runInfo[ctx].nFrames);
                            
                            f = fopen(name, "wb");
                            if (f)
                            {
                                fwrite(pFrame->data, 1,
                                    pFrame->ui32Stride*pFrame->ui32Height, f);
                                fclose(f);
                            }
                            else
                            {
                                LOG_WARNING("Failed to open tmp DG file '%s'\n",
                                    name);
                            }
                        }
                        
                        ret = CI_DatagenInsertFrame(pFrame);
                        if ( IMG_SUCCESS != ret )
                        {
                            LOG_ERROR("CTX %d IntDG%d on gasket %d failed to insert frame\n", ctx, aDatagen[gasket].intDG->ui8IIFDGIndex, gasket);
                            iRet = EXIT_FAILURE;
                            goto prun_clean;
                        }
                    }
#ifdef CI_EXT_DATAGEN
                    else // if External DG not available should have failed to configure it!
                    {
                        char *pszName = NULL;
                        
                        if ( runInfo[ctx].bWriteDGFrame )
                        {
                            sprintf(name, "extDG_frame_%d.dat",
                                runInfo[ctx].nFrames);
                            pszName = name;
                        }
                        
                        ret=DG_CameraShoot(aDatagen[gasket].extDG,
                            &(runInfo[ctx].sCurrentFrame), pszName);
                        if ( IMG_SUCCESS != ret )
                        {
                            LOG_ERROR("DG %d failed to shoot (returned %d)\n", gasket, ret);
                            iRet = EXIT_FAILURE;
                            goto prun_clean;
                        }
                    }
#endif

                    LOG_INFO("--- DG %d frame %d submitted ---\n", gasket, gasketSubmitted[gasket]);
                    gasketSubmitted[gasket]++;
                }
            }
        }

        for ( f = 0 ; f < max_triggered ; f++ )
        {
            for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
            {
                if ( runInfo[ctx].computing > 0 )
                {
                    if ( CaptureRunWrite(&(runInfo[ctx]), apPipeline[ctx], IMG_TRUE) != EXIT_SUCCESS )
                    {
                        LOG_ERROR("failed to write a frame for CTX %d\n", ctx);
                        iRet = EXIT_FAILURE;
                        goto prun_clean;
                    }

                    if (parameters->sCapture[ctx].bAllocateLate)
                    {
                        CI_BUFFID ids;
                        CI_BUFFER hdrIns;

                        ret = CI_PipelineFindFirstAvailable(apPipeline[ctx], &ids);
                        if (IMG_SUCCESS != ret)
                        {
                            LOG_ERROR("failed to find available buffer!\n");
                            iRet = EXIT_FAILURE;
                            goto prun_clean;
                        }
                        
                        if (parameters->sCapture[ctx].pszHDRInsInput)
                        {
                            // also find 1st available HDRIns buffer as they are allocated too
                            ret = CI_PipelineAcquireHDRBuffer(apPipeline[ctx], &hdrIns, 0);
                            if (IMG_SUCCESS == ret)
                            {
                                ids.idHDRIns = hdrIns.id;
                            }
                            else
                            {
                                LOG_ERROR("failed to find available HDR buffer!\n");
                                iRet = EXIT_FAILURE;
                                goto prun_clean;
                            }
                        }

                        LOG_INFO("--- CTX %d deregister buffers ---\n", ctx, f);

                        if (ids.encId)
                        {
                            ret = CI_PipelineDeregisterBuffer(apPipeline[ctx], ids.encId);
                            if (IMG_SUCCESS != ret)
                            {
                                LOG_WARNING("failed to deregister encoder buffer\n");
                            }
                        }
                        if (ids.dispId)
                        {
                            ret = CI_PipelineDeregisterBuffer(apPipeline[ctx], ids.dispId);
                            if (IMG_SUCCESS != ret)
                            {
                                LOG_WARNING("failed to deregister display buffer\n");
                            }
                        }
                        if (ids.idHDRExt)
                        {
                            ret = CI_PipelineDeregisterBuffer(apPipeline[ctx], ids.idHDRExt);
                            if (IMG_SUCCESS != ret)
                            {
                                LOG_WARNING("failed to deregister HDR extraction buffer\n");
                            }
                        }
                        if (ids.idHDRIns)
                        {
                            ret = CI_PipelineDeregisterBuffer(apPipeline[ctx], ids.idHDRIns);
                            if (IMG_SUCCESS != ret)
                            {
                                LOG_WARNING("failed to deregister HDR insertion buffer\n");
                            }
                        }
                        if (ids.idRaw2D)
                        {
                            ret = CI_PipelineDeregisterBuffer(apPipeline[ctx], ids.idRaw2D);
                            if (IMG_SUCCESS != ret)
                            {
                                LOG_WARNING("failed to deregister Raw2D buffer\n");
                            }
                        }
                    }

                    runInfo[ctx].nFrames++;
                    if ( runInfo[ctx].nFrames == parameters->sCapture[ctx].ui32RunForFrames )
                    {
                        if ( CI_PipelineStopCapture(apPipeline[ctx]) != IMG_SUCCESS )
                        {
                            LOG_ERROR("CTX %d failed to stop capture\n", ctx);
                            goto prun_clean;
                        }
                        uiContinueProcessing--; // one less context to compute for
                    }
                }
            }
        }
    }

    iRet = EXIT_SUCCESS;
prun_clean:
    for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
    {
        if ( apPipeline[ctx] != NULL )
        {
            CaptureRunClean(&(runInfo[ctx]));

            // CI_Pipeline stopped when exiting correctly or stoped at destruction in case of error
        }
    }

#ifdef CI_DEBUG_FCT
    CI_DebugTestIO(); // testio register read/poll
#endif

    return iRet;
}

static int CaptureSerial(struct CMD_PARAM *parameters, CI_PIPELINE *pPipeline, struct datagen *pDatagen, CI_CONNECTION *pConnection)
{
    IMG_UINT32 iret;
    IMG_UINT32 gasket = pPipeline->sImagerInterface.ui8Imager;
    struct captureRun runInfo;
    const char *pszInpuFile = NULL;
    struct timeval startTime, endTime;
    int ctx = pPipeline->ui8Context;
    IMG_UINT32 ui32Frames = 0;

    if ( pDatagen->bIsInternal )
    {
        ui32Frames = parameters->sIIFDG[parameters->sCapture[ctx].iIIFDG].uiFrames;
        if ( ui32Frames > 1 )
        {
            pszInpuFile = parameters->sIIFDG[parameters->sCapture[ctx].iIIFDG].pszInputFLX;
        }
        // otherwise stays NULL and not reloaded (1st frame loaded in CaptureRunInit)

        IMG_ASSERT(gasket == pDatagen->intDG->ui8Gasket); // verified beforehand
    }
#ifdef CI_EXT_DATAGEN
    else // if External DG not available should have failed to configure it!
    {
        ui32Frames = parameters->sDataGen[gasket].ui32Frames;
        if ( ui32Frames > 1 )
        {
            pszInpuFile = parameters->sDataGen[gasket].pszInputFLX;
        }
        // otherwise stays NULL and not reloaded (1st frame loaded in CaptureRunInit)

        IMG_ASSERT(gasket == pDatagen->extDG->ui8DGContext);
    }
#endif

    // configure the run and also loads the 1st frame
    if ( CaptureRunInit(&runInfo, parameters, pPipeline, gasket, pConnection) != EXIT_SUCCESS )
    {
        LOG_ERROR("failed to init runInfo\n");
        return EXIT_FAILURE;
    }
    
    iret = EXIT_FAILURE; // from now on goto run_clean is a failure
    if ( CI_PipelineComputeLinestore(pPipeline) != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to compute linestore\n");
        goto run_clean;
    }

    if ( CI_PipelineStartCapture(pPipeline) != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to start capture\n");
        goto run_clean;
    }

#if 0
    LOG_INFO("press enter to continue ");
    getchar();
#endif

    gettimeofday(&startTime, NULL);

    while ( runInfo.nFrames < runInfo.framesToCapture )
    {
        IMG_UINT32 store = 0;
        // compute is usually window unless the last step when the window is bigger than what is left
        runInfo.computing = IMG_MIN_INT(runInfo.framesToCapture-runInfo.nFrames, runInfo.window);

        if (parameters->sCapture[ctx].bAllocateLate
            && runInfo.nFrames < parameters->sCapture[ctx].ui32ConfigBuffers)
        {
            iret = AllocateBuffers(parameters, ctx, pPipeline, 1);
            if (IMG_SUCCESS != iret)
            {
                LOG_ERROR("failed to allocate the buffers!\n");
                goto run_clean;
            }
        }

        // once the 1st shot has been done do not update the image from disk
        if ( ui32Frames == 1 && runInfo.nFrames == 0 )
        {
            store = runInfo.computing;
            runInfo.computing = 1;

            // computing is 1 - only 1 frame read from file
            if ( CaptureRunShoot(&runInfo, pPipeline, pDatagen, pszInpuFile) != EXIT_SUCCESS )
            {
                LOG_ERROR("failed to shoot frame %d\n", runInfo.nFrames);
                goto run_clean;
            }

            // from now on no more reading from file
            pszInpuFile = NULL;
            // compute the computing-1 remaining frames
            runInfo.computing = store-1;
            store = 1; // the one we need to add
        }

        if ( CaptureRunShoot(&runInfo, pPipeline, pDatagen, pszInpuFile) != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to shoot frame %d\n", runInfo.nFrames);
            goto run_clean;
        }

        if ( store > 0 )
        {
            runInfo.computing++; // add the one that was removed
            store = 0;
        }

        LOG_INFO("--- wait for %d frames ---\n", runInfo.computing);

        while (runInfo.computing > 0)
        {
            // blocking call
            if ( CaptureRunWrite(&runInfo, pPipeline, IMG_TRUE) != EXIT_SUCCESS)
            {
                LOG_ERROR("failed to write a frame to file\n");
                goto run_clean;
            }
        }

    } // nframes < parameters->sDataGen.ui32Frames

    // yield one last time to get interrupts and other pending things
    yield(FAKE_INTERRUPT_WAIT_MS);
    iret = EXIT_SUCCESS;

    gettimeofday(&endTime, NULL);
    endTime.tv_sec -= startTime.tv_sec;
    endTime.tv_usec -= startTime.tv_usec;
    runInfo.runTime = endTime.tv_sec + endTime.tv_usec/1000000.0;

    LOG_INFO("=== average FPS %lf (%u frames in %lf sec) ===\n", runInfo.framesToCapture/runInfo.runTime, runInfo.framesToCapture, runInfo.runTime);
#ifdef CI_DEBUG_FCT
    if ( parameters->uiRegisterTest == 2 ) // verify load
    {
        CI_DebugLoadTest(ctx);
        CI_DebugGMALut();
    }
#endif

run_clean:
    CaptureRunClean(&runInfo);

    if ( CI_PipelineStopCapture(pPipeline) != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to stop capture\n");
    }

#ifdef CI_DEBUG_FCT
    CI_DebugTestIO(); // testio register read/poll
#endif

    return iret;
}

#ifdef FELIX_FAKE

#include "transif_adaptor.h"

static IMG_HANDLE gTransifAdaptor = NULL;

#endif // FELIX_FAKE

// DG_DriverFinalise does not fail if DG driver does not exits - just does not deinit it
#define CLEAN_RUN_DRIVERS(retCode)              \
    returnCode = retCode;                       \
    goto clean_run_driver;

#ifdef FELIX_FAKE
pthread_t interruptThread;
pthread_mutex_t interruptMutex;
pthread_cond_t interruptCondVar;
static sLinkedList_T * fifo;

#ifdef CI_TEST_USES_TRANSIF
static IMG_BOOL interruptExit = IMG_FALSE;
#endif

// used to receive interrupts with transif
static void Handle_interrupt(void* userData, IMG_UINT32 ui32IntLines)
{
    int ret;

    IMG_ASSERT(g_psCIDriver); // should be initialised
    IMG_ASSERT(g_psCIDriver->pDevice); // should have an attached device
    // should have an interrupt handler
    IMG_ASSERT(g_psCIDriver->pDevice->irqHardHandle);
    IMG_ASSERT(g_psCIDriver->pDevice->irqThreadHandle);
    // otherwise we have to allocate an element...
    IMG_ASSERT(sizeof(void*)>=sizeof(IMG_UINT32));

    if (gTransifAdaptor)
    {
        IMG_UINT32 *t = IMG_CALLOC(1, sizeof(IMG_UINT32));
        *t = ui32IntLines;
        // Transif in use.
        pthread_mutex_lock(&interruptMutex);
        List_pushBackObject(fifo, (void*)t);
        pthread_cond_signal(&interruptCondVar);
        pthread_mutex_unlock(&interruptMutex);
    }
    else
    {
        ret = g_psCIDriver->pDevice->irqHardHandle(ui32IntLines, NULL);
        if (IRQ_WAKE_THREAD == ret)
        {
            g_psCIDriver->pDevice->irqThreadHandle(ui32IntLines, NULL);
        }
    }
}

void * InterruptThread(void * arg)
{
#ifndef CI_TEST_USES_TRANSIF
    int error = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    int ret;

    if (error)
    {
        LOG_ERROR("pthread_setcancelstate failed");
        exit(EXIT_FAILURE);
    }
#endif
    LOG_INFO("interrupt thread started\n");

    while (1)
    {
        sCell_T *pFront = NULL;
        pthread_mutex_lock(&interruptMutex);
        pthread_cond_wait(&interruptCondVar, &interruptMutex);
        pthread_mutex_unlock(&interruptMutex);

        while((pFront=List_popFront(fifo)) != NULL)
        {
            IMG_UINT32 *t = (IMG_UINT32*)pFront->object;
            ret = g_psCIDriver->pDevice->irqHardHandle(*t, NULL);
            if (IRQ_WAKE_THREAD == ret)
            {
                g_psCIDriver->pDevice->irqThreadHandle(*t, NULL);
            }
            IMG_FREE(t);
            IMG_FREE(pFront);
        }
#ifdef CI_TEST_USES_TRANSIF
        if (interruptExit) break;
#endif
    };
    LOG_INFO("interrupt thread exiting...\n");
    pthread_exit(NULL);
}
#endif

int init_drivers(struct CMD_PARAM *parameters)
{
#ifdef FELIX_FAKE
    static SYS_DEVICE sDevice;
    static KRN_CI_DRIVER sCIDriver; // resilient
#ifdef CI_EXT_DATAGEN
    static KRN_DG_DRIVER sDGDriver; // resilient
#endif

    sDevice.probeSuccess = NULL;
    SYS_DevRegister(&sDevice);

    if ( setPageSize((1<<10)*(parameters->uiCPUPageSize)) != 0 )
    {
        LOG_ERROR("Failed to create transif layer\n");
        return EXIT_FAILURE;
    }

    if ( parameters->pszTransifLib != NULL && parameters->pszTransifSimParam != NULL )
    {
        int error;

        // init transif here
        TransifAdaptorConfig sAdaptorConfig = { 0 };
        char *ppszArgs[2] = { "-argfile", parameters->pszTransifSimParam };
        bUseTCP = IMG_FALSE;

        sAdaptorConfig.nArgCount = 2;
        sAdaptorConfig.ppszArgs = ppszArgs;
        sAdaptorConfig.pszLibName = parameters->pszTransifLib;
        sAdaptorConfig.pszDevifName = "FELIX";
        sAdaptorConfig.bTimedModel = parameters->bTransifUseTimedModel;
        sAdaptorConfig.bDebug =  parameters->bTransifDebug;
        sAdaptorConfig.sFuncs.pfnInterrupt = &Handle_interrupt;

        sAdaptorConfig.eMemType = IMG_MEMORY_MODEL_DYNAMIC;
        sAdaptorConfig.bForceAlloc = IMG_FALSE;

        gTransifAdaptor = TransifAdaptor_Create(&sAdaptorConfig);
        if ( gTransifAdaptor == NULL )
        {
            LOG_ERROR("Failed to create transif layer\n");
            return EXIT_FAILURE;
        }

#ifdef CI_EXT_DATAGEN
        DG_SetUseTransif(IMG_TRUE);
#endif

        // Defer interrupt work. Interrupts are reported from SystemC thread
        // context but all register/memory requests shouldn't be triggered
        // this way.
        fifo = (sLinkedList_T*)malloc(sizeof(sLinkedList_T));

        if (!fifo)
        {
            LOG_ERROR("Failed to create fifo\n");
            TransifAdaptor_Destroy(gTransifAdaptor);
            return EXIT_FAILURE;
        }
        List_init(fifo);

        pthread_mutex_init(&interruptMutex, NULL);
        pthread_cond_init(&interruptCondVar, NULL);

        error = pthread_create(&interruptThread, NULL, &InterruptThread, NULL);
        if (error)
        {
            LOG_ERROR("Failed to create interrupt thread\n");
            pthread_cond_destroy(&interruptCondVar);
            pthread_mutex_destroy(&interruptMutex);
            List_clear(fifo); // may have to use List_clearObject(fifo, free) if objects are allocated
            free(fifo);
            TransifAdaptor_Destroy(gTransifAdaptor);
            return EXIT_FAILURE;
        }
    }
    else
    {
        int i;
        if ( parameters->pszSimIP )
        {
            pszTCPSimIp = parameters->pszSimIP;
        }
        if ( parameters->uiSimPort != 0 )
        {
            ui32TCPPort = parameters->uiSimPort;
        }
        for (i = 0 ; i < CI_N_HEAPS ; i++)
        {
            aVirtualHeapsOffset[i] = parameters->aHeapOffsets[i];
        }
    }
    bDisablePdumpRDW = !parameters->bEnableRDW;
    
    if ( KRN_CI_DriverCreate(&sCIDriver, parameters->uiUseMMU, parameters->uiTilingScheme, parameters->uiGammaCurve, &sDevice) != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to create global CI driver\n");
        return EXIT_FAILURE;
    }

#ifdef CI_EXT_DATAGEN
    if ( KRN_DG_DriverCreate(&sDGDriver, parameters->uiUseMMU) != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to create global DG driver\n");
        KRN_CI_DriverDestroy(&sCIDriver);
        return EXIT_FAILURE;
    }
#endif

    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "");
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "end insmod time");
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "");
#endif // FELIX_FAKE
    return EXIT_SUCCESS;
}

int finalise_drivers()
{
#ifdef FELIX_FAKE
    SYS_DEVICE *pDev = g_psCIDriver->pDevice;
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "");
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "beginning rmmod time");
    CI_PDUMP_COMMENT(g_psCIDriver->sTalHandles.hMemHandle, "");

#ifdef CI_EXT_DATAGEN
    KRN_DG_DriverDestroy(g_pDGDriver);
#endif
    KRN_CI_DriverDestroy(g_psCIDriver);
    SYS_DevDeregister(pDev);

    if ( gTransifAdaptor != NULL )
    {
        int error;
        TransifAdaptor_Destroy(gTransifAdaptor);
        gTransifAdaptor = NULL;
#ifdef CI_TEST_USES_TRANSIF
        interruptExit = IMG_TRUE;
#else
        // Clean up interrupt thread...
        error = pthread_cancel(interruptThread);
        if (error) LOG_ERROR("pthread_cancel failed!\n");
#endif

        // Wait until the thread is gone.
        pthread_cond_destroy(&interruptCondVar);
        pthread_mutex_destroy(&interruptMutex);

        List_clear(fifo);
        free(fifo);
    }
#endif // FELIX_FAKE
    return EXIT_SUCCESS;
}

IMG_STATIC_ASSERT(CI_N_IIF_DATAGEN<=CI_N_EXT_DATAGEN, lessIIFDGThanExtDG); // to make sure we can use CI_N_EXT_DATAGEN to define the list of config pictures

//
// global drivers are created before that (so that potentially several of those can be run in parallel - if that ever works)
//
int run_drivers(struct CMD_PARAM *parameters)
{
    CI_CONNECTION *pDriver = NULL;

    CI_PIPELINE *apPipeline[CI_N_CONTEXT];
    struct datagen aDatagen[CI_N_IMAGERS]; // 1 per gasket

    sSimImageIn aConfigPicture[CI_N_IMAGERS]; // 1 per gasket

    IMG_UINT32 returnCode = 0, i;
    IMG_RESULT ret;

    if ( CI_DriverInit(&pDriver) != IMG_SUCCESS )
    {
        return EXIT_FAILURE; // nothing to clean yet
    }

#ifndef FELIX_FAKE
    printConnection(pDriver, stdout);
#endif

#ifdef CI_EXT_DATAGEN
    ret = DG_DriverInit();
    if (ret)
    {
        CI_DriverFinalise(pDriver);
        return EXIT_FAILURE;
    }
#endif

    if ( parameters->uiRegisterTest == 1 )
    {
#ifdef CI_DEBUG_FCT
        KRN_CI_DebugRegistersTest();
#ifdef CI_EXT_DATAGEN
        DG_DriverFinalise();
#endif
        CI_DriverFinalise(pDriver);
        return EXIT_SUCCESS;
#else
        LOG_ERROR("CI_DEBUG_FCT are not included in this build");
#ifdef CI_EXT_DATAGEN
        DG_DriverFinalise();
#endif
        CI_DriverFinalise(pDriver);
        return EXIT_SUCCESS;
#endif
    }
#if defined(CI_DEBUG_FCT) && defined(FELIX_FAKE)
    KRN_CI_DebugEnableRegisterDump(IMG_FALSE);
#endif

    // init arrays
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        apPipeline[i] = NULL;
    }
    for ( i = 0 ; i < CI_N_IMAGERS ; i++ )
    {
        aDatagen[i].bIsInternal = IMG_FALSE;
        aDatagen[i].intDG = NULL;
        aDatagen[i].extDG = NULL; // should have been done by intDG unless changed from union

        //CI_GasketInit(&(aDatagen[i].sGasket)); // used only if internal DG

        ret = SimImageIn_init(&(aConfigPicture[i]));
        if (ret)
        {
            LOG_ERROR("failed to initialise configuration image\n");
            CLEAN_RUN_DRIVERS(EXIT_FAILURE);
        }
    }

    // for every context
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        IMG_UINT32 gasket = parameters->sCapture[i].ui8imager;
        CI_PIPELINE *pPipeline = NULL;

        if ( parameters->sCapture[i].pszEncoderOutput == NULL && // encoder output
             parameters->sCapture[i].pszDisplayOutput == NULL && // display output
             parameters->sCapture[i].pszDataExtOutput == NULL && // data extraction output
             parameters->sCapture[i].pszHDRextOutput == NULL && // HDR extraction output
             parameters->sCapture[i].pszRaw2ExtDOutput == NULL && // Raw 2D extraction output
             (parameters->sCapture[i].iStats == 0 && parameters->sCapture[i].iDPFEnabled <= 1) ) // stats output (DPF write map (>1) is considered a stat, not detect (1))
        {
            LOG_INFO("skip ctx %d (gasket %d) - no output found\n", i, gasket);
            // no output enabled
            continue;
        }

        if ( i >= pDriver->sHWInfo.config_ui8NContexts )
        {
            LOG_ERROR("the current HW can only support %d context(s) (CTX %d is configured)\n", pDriver->sHWInfo.config_ui8NContexts, i);
            CLEAN_RUN_DRIVERS(EXIT_FAILURE);
        }
        if ( parameters->sCapture[i].ui8imager >= pDriver->sHWInfo.config_ui8NImagers )
        {
            LOG_ERROR("the current HW can only support %d imager(s) (CTX %d is trying to use imager %d)\n", pDriver->sHWInfo.config_ui8NImagers, i, parameters->sCapture[i].ui8imager);
            CLEAN_RUN_DRIVERS(EXIT_FAILURE);
        }

        /*
         * configure internal DG
         */
        if ( parameters->sCapture[i].iIIFDG >= 0 )
        {
            int iifDg = parameters->sCapture[i].iIIFDG;
            if ( (pDriver->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_IIF_DATAGEN) == 0 )
            {
                LOG_ERROR("cannot run the configuration: HW does not support internal DG but context %d uses it\n", i);
                CLEAN_RUN_DRIVERS(EXIT_FAILURE);
            }

            if ( iifDg >= pDriver->sHWInfo.config_ui8NIIFDataGenerator )
            {
                LOG_ERROR("cannot run the configuration: HW has %d IntDG but context %d wants to use IntDG%d\n", 
                    pDriver->sHWInfo.config_ui8NIIFDataGenerator, i, iifDg
                );
                CLEAN_RUN_DRIVERS(EXIT_FAILURE);
            }

            aDatagen[gasket].bIsInternal = IMG_TRUE;

            if ( aDatagen[gasket].intDG == NULL )
            {
                IMG_INT32 nbFramesToLoad = parameters->sIIFDG[iifDg].uiFrames-1;
                IMG_ASSERT(gasket == parameters->sIIFDG[iifDg].uiGasket); // should have been verified when loading parameters

                ret = SimImageIn_loadFLX(parameters->sIIFDG[iifDg].pszInputFLX, &(aConfigPicture[gasket]), nbFramesToLoad);
                if (ret)
                {
                    LOG_ERROR("failed load FLX file '%s' (frame %d)\n", parameters->sIIFDG[iifDg].pszInputFLX, parameters->sIIFDG[iifDg].uiFrames-1);
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }

                LOG_INFO("loaded %dx%d %db %s pixels\n---\n",
                       aConfigPicture[gasket].info.ui32Width, aConfigPicture[gasket].info.ui32Height, aConfigPicture[gasket].info.ui8BitDepth,
                       aConfigPicture[gasket].info.eColourModel == SimImage_RGGB ? "RGGB" : aConfigPicture[gasket].info.eColourModel == SimImage_GRBG ? "GRBG" : aConfigPicture[gasket].info.eColourModel == SimImage_GBRG ? "GBRG" : "BGGR"
                    );

                ret = CI_DatagenCreate(&(aDatagen[gasket].intDG), pDriver);
                if (ret)
                {
                    LOG_ERROR("failed to create Internal Data Generator\n");
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }
                
                ret = ConfigureIntDG(parameters, &(pDriver->sHWInfo), parameters->sCapture[i].ui8imager, &(aConfigPicture[gasket]), &(aDatagen[gasket]));
                if (ret)
                {
                    LOG_ERROR("failed to configure Internal Data Generator\n");
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }

                aDatagen[gasket].ui32HBlanking = parameters->sIIFDG[iifDg].aBlanking[0];
                aDatagen[gasket].ui32VBlanking = parameters->sIIFDG[iifDg].aBlanking[1];
                
                LOG_INFO("Configure blanking %dx%d\n", aDatagen[gasket].ui32HBlanking, aDatagen[gasket].ui32VBlanking);

                /*if ( CI_GasketAcquire(&(aDatagen[gasket].sGasket), pDriver) != IMG_SUCCESS )
                {
                    LOG_ERROR("Failed to acquire gasket %d for internal data generator\n", gasket);
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }*/

                ret = CI_DatagenStart(aDatagen[gasket].intDG);
                if (ret)
                {
                    LOG_ERROR("failed to start the Internal Data Generator\n");
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }
            }
            else
            {
                LOG_INFO("DG %d already initialised\n---\n", gasket);
            }
        }
        /*
         * configure external DG
         */
#ifdef CI_EXT_DATAGEN
        else if ( parameters->sDataGen[gasket].pszInputFLX != NULL )
        {
            LOG_INFO("---\nCTX %d - DG %d loading FLX '%s'\n", i, gasket, parameters->sDataGen[gasket].pszInputFLX);
            if ( aDatagen[gasket].extDG == NULL ) // if not already initialised
            {
                DG_CAMERA *pCamera = NULL;
                IMG_INT32 nbFramesToLoad = parameters->sDataGen[gasket].ui32Frames-1;
                
                ret = SimImageIn_loadFLX(parameters->sDataGen[gasket].pszInputFLX, &(aConfigPicture[gasket]), nbFramesToLoad);
                if (ret)
                {
                    LOG_ERROR("failed load FLX file '%s' (frame %d)\n", parameters->sDataGen[gasket].pszInputFLX, parameters->sDataGen[gasket].ui32Frames-1);
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }

                LOG_INFO("loaded %dx%d %db %s pixels\n---\n",
                       aConfigPicture[gasket].info.ui32Width, aConfigPicture[gasket].info.ui32Height, aConfigPicture[gasket].info.ui8BitDepth,
                       aConfigPicture[gasket].info.eColourModel == SimImage_RGGB ? "RGGB" : aConfigPicture[gasket].info.eColourModel == SimImage_GRBG ? "GRBG" : aConfigPicture[gasket].info.eColourModel == SimImage_GBRG ? "GBRG" : "BGGR"
                    );

                ret = DG_CameraCreate(&pCamera);
                if (ret)
                {
                    LOG_ERROR("failed to create Data Generator\n");
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }
                aDatagen[gasket].bIsInternal = IMG_FALSE;
                aDatagen[gasket].extDG = pCamera;

                ret = ConfigureExtDG(parameters, &(pDriver->sHWInfo), parameters->sCapture[i].ui8imager, &(aConfigPicture[gasket]), &(aDatagen[gasket]));
                if (ret)
                {
                    LOG_ERROR("failed to configure Data Generator\n");
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }

                ret = DG_CameraStart(pCamera, pDriver);
                if (ret)
                {
                    LOG_ERROR("Could not start DG\n");
                    CLEAN_RUN_DRIVERS(EXIT_FAILURE);
                }
            }
            else
            {
                LOG_INFO("DG %d already initialised\n---\n", gasket);
                //pCamera = aDatagen[gasket].extDG;
            }
        }
#endif // CI_EXT_DATAGEN
        else
        {
            // tmp
            LOG_ERROR("DG %d associated with CTX %d is disabled\n", gasket, i);
            CLEAN_RUN_DRIVERS(EXIT_FAILURE);
        }

        /*
         * configure driver
         */

        ret = CI_PipelineCreate(&pPipeline, pDriver);
        if (ret)
        {
            LOG_ERROR("Failed to create configuration object\n");
            CLEAN_RUN_DRIVERS(EXIT_FAILURE);
        }
        apPipeline[i] = pPipeline;

        ret = ConfigurePipeline(parameters, i, &(aConfigPicture[gasket].info), pPipeline, &(pDriver->sHWInfo));
        if (ret)
        {
            LOG_ERROR("failed to configure the pipeline\n");
            CLEAN_RUN_DRIVERS(EXIT_FAILURE);
        }

        if (parameters->sCapture[i].bAllocateLate == IMG_FALSE)
        {
            ret = AllocateBuffers(parameters, i, pPipeline, parameters->sCapture[i].ui32ConfigBuffers);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate the buffers\n");
                CLEAN_RUN_DRIVERS(EXIT_FAILURE);
            }
        }

    } // for every context

    //for ( i = CI_N_CONTEXT-1 ; i >= 0 ; i-- )
    if ( parameters->bParallel == IMG_FALSE )
    {
        for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
        {
            int gasket = parameters->sCapture[i].ui8imager;
            // if configured
            if ( apPipeline[i] == NULL )//|| apCapture[i] == NULL || apCamera[i] == NULL )
            {
                continue;
            }

            if ( aDatagen[gasket].bIsInternal )
            {
                LOG_INFO("=== running CTX %d with Int DG %d on gasket %d ===\n", 
                    i, aDatagen[gasket].intDG->ui8IIFDGIndex, aDatagen[gasket].intDG->ui8Gasket);
            }
#ifdef CI_EXT_DATAGEN
            else // if External DG not available should have failed to configure it!
            {
                LOG_INFO("=== running CTX %d with Ext DG %d ===\n", 
                    i, aDatagen[gasket].extDG->ui8DGContext);
            }
#endif

            returnCode = CaptureSerial(parameters, apPipeline[i], &(aDatagen[gasket]), pDriver);
            if (EXIT_SUCCESS != returnCode)
            {
                LOG_ERROR("CTX %d run failed for %d frames\n", i, parameters->sDataGen[gasket].ui32Frames);
                goto clean_run_driver;
            }

            LOG_INFO("=== running CTX %d on gasket %d completed ===\n", i, gasket);
        }
    }
    else
    {
        LOG_INFO("=== running in 'parallel' ===\n");

        returnCode = CaptureParallel(parameters, apPipeline, aDatagen, pDriver);
        if (EXIT_SUCCESS != returnCode)
        {
            LOG_ERROR("failed to run in parallel\n");
        }

        LOG_INFO("=== running in 'parallel' completed ===\n");
    }

    //CLEAN_RUN_DRIVERS(retCode) comes here
clean_run_driver:
    //LOG_INFO("clean run drivers\n");
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        if ( apPipeline[i] != NULL ) CI_PipelineDestroy(apPipeline[i]);
    }
    for ( i = 0 ; i < CI_N_IMAGERS ; i++ )
    {
        if ( aDatagen[i].bIsInternal )
        {
            if ( aDatagen[i].intDG )
            {
                CI_DatagenDestroy(aDatagen[i].intDG);
            }
            //CI_GasketRelease(&(aDatagen[i].sGasket), pDriver);
        }
#ifdef CI_EXT_DATAGEN
        else if (aDatagen[i].extDG)
        {
            DG_CameraDestroy(aDatagen[i].extDG);
        }
#endif
        //LOG_INFO("pCamera cleant\n");
        //if ( pPGM != NULL ) { SimImageIn_clean(pPGM); IMG_FREE(pPGM); }
        SimImageIn_clean(&(aConfigPicture[i]));
    }
    //LOG_INFO("pPipeline cleant\n");
#ifdef CI_EXT_DATAGEN
    DG_DriverFinalise();
#endif
    CI_DriverFinalise(pDriver);
    return returnCode;
}
