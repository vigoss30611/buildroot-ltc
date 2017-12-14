/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2007 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : Sequence and Entry-Point layer decoding functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_headers.c,v $
--  $Revision: 1.21 $
--  $Date: 2008/12/05 10:00:42 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1hwd_headers.h"
#include "vc1hwd_picture_layer.h"
#include "vc1hwd_vlc.h"
#include "vc1hwd_bitplane.h"
#include "vc1hwd_util.h"
#include "vc1hwd_storage.h"
#include "vc1hwd_decoder.h"
#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif

/*------------------------------------------------------------------------------
    External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function name: vc1hwdGetStartCode

        Functional description:
            Function to seek start codes from the stream

        Inputs:
            pStrmData   pointer to stream data structure
            
        Outputs:

        Returns:
            startcode

------------------------------------------------------------------------------*/
u32 vc1hwdGetStartCode(strmData_t *pStrmData)
{
    u32 tmp;
    u32 sc = 0;
    u8 * pStrm;

    /* Check that we are in bytealigned position */
    tmp = pStrmData->bitPosInWord;  
    if (tmp)
    {
        if (vc1hwdFlushBits(pStrmData, 8-tmp) != HANTRO_OK)
            return SC_NOT_FOUND;
    }
    /* Check that we have enough stream data */   
    if (((pStrmData->strmBuffReadBits>>3) + 4) > pStrmData->strmBuffSize)
    {
        return SC_NOT_FOUND;
    }

    pStrm = pStrmData->pStrmCurrPos;

    sc = ( ((u32)pStrm[0]) << 16 ) +
         ( ((u32)pStrm[1]) << 8  ) +
         ( ((u32)pStrm[2]) );

    if ( sc != 0x000001 )
    {
        sc = SC_NOT_FOUND;
    }
    else
    {
        sc = (sc << 8 ) + pStrm[3];
        pStrmData->pStrmCurrPos += 4;
        pStrmData->strmBuffReadBits += 32;
    }

    return sc;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdGetUserData

        Functional description:
            Flushes user data bytes

        Inputs:
            pStorage    Stream storage descriptor
            pStrmData   Pointer to input stream data structure

        Outputs:

        Returns:
            VC1HWD_OK

------------------------------------------------------------------------------*/
u32 vc1hwdGetUserData( swStrmStorage_t *pStorage, 
                       strmData_t *pStrmData )
{
/* Variables */
    u32 sc;
    u8 *pStrm;

/* Code */
    do
    {
        pStrm = pStrmData->pStrmCurrPos;
        
        /* Check that we have enough stream data */   
        if (((pStrmData->strmBuffReadBits>>3)+3) <= pStrmData->strmBuffSize)
        {
            sc = ( ((u32)pStrm[0]) << 16 ) +
                 ( ((u32)pStrm[1]) << 8  ) +
                 ( ((u32)pStrm[2]) );
        }
        else
            sc = 0;
    
        /* start code prefix found */
        if ( sc == 0x000001 )
        {
            break;
        }
    } while (vc1hwdFlushBits(pStrmData, 8) == HANTRO_OK);

    if (vc1hwdIsExhausted(pStrmData))
        return END_OF_STREAM;
    else
        return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdDecodeSequenceLayer

        Functional description:
            Decodes sequence layer of video bit stream

        Inputs:
            pStorage    Stream storage descriptor
            pStrmData   Pointer to input stream data structure

        Outputs:

        Returns:
            VC1HWD_SEQ_HDRS_RDY
            VC1HWD_METADATA_ERROR
            VC1HWD_MEMORY_FAIL

------------------------------------------------------------------------------*/

u32 vc1hwdDecodeSequenceLayer( swStrmStorage_t *pStorage, 
                               strmData_t *pStrmData )
{
    i32 tmp;
    u32 w, h, i;
    DWLHwConfig_t config;

    ASSERT(pStorage);
    ASSERT(pStrmData);

    (void)G1DWLmemset(&config, 0, sizeof(DWLHwConfig_t));
    DWLReadAsicConfig(&config);

    ASSERT( config.maxDecPicWidth > 0 );
    ASSERT( config.maxDecPicWidth < 1921 );

    /* reset flag indicating already decoded sequence layer header */
    pStorage->hdrsDecoded &= ~HDR_SEQ;

    /* PROFILE */
    tmp = vc1hwdGetBits(pStrmData, 2);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp != 3)
        return HANTRO_NOK;
#endif
    /* LEVEL */
    pStorage->level = vc1hwdGetBits(pStrmData, 3);
#ifdef HANTRO_PEDANTIC_MODE
    if(pStorage->level >= 5)
        return HANTRO_NOK;
#endif
    /* COLORDIFF_FORMAT */
    pStorage->colorDiffFormat = vc1hwdGetBits(pStrmData, 2);
#ifdef HANTRO_PEDANTIC_MODE
    if(pStorage->colorDiffFormat != 1)
        return HANTRO_NOK;
#endif
    /* FRMRTQ_POSTPROC */
    pStorage->frmrtqPostProc = vc1hwdGetBits(pStrmData, 3);
    /* BITRTQ_POSTPROC */
    pStorage->bitrtqPostProc = vc1hwdGetBits(pStrmData, 5);
    /* POSTPROCFLAG */
    pStorage->postProcFlag = vc1hwdGetBits(pStrmData, 1);
    
    /* MAX_CODED_WIDTH */
    tmp = vc1hwdGetBits(pStrmData, 12);
    pStorage->maxCodedWidth = 2*tmp + 2;
      /* MAX_CODED_HEIGHT */
    tmp = vc1hwdGetBits(pStrmData, 12);
    pStorage->maxCodedHeight = 2*tmp + 2;

    /* check size information */
    if ( pStorage->maxCodedWidth < MIN_PIC_WIDTH ||
         pStorage->maxCodedWidth > config.maxDecPicWidth ||
         pStorage->maxCodedHeight < MIN_PIC_HEIGHT ||
         pStorage->maxCodedHeight > config.maxDecPicWidth ||
         (pStorage->maxCodedWidth & 0x1) ||
         (pStorage->maxCodedHeight & 0x1) )
        return (VC1HWD_METADATA_ERROR);

    pStorage->picWidthInMbs  = (pStorage->maxCodedWidth+15) >> 4;
    pStorage->picHeightInMbs = (pStorage->maxCodedHeight+15) >> 4;
    pStorage->numOfMbs       = (pStorage->picWidthInMbs *
                                pStorage->picHeightInMbs);

    if (pStorage->numOfMbs > MAX_NUM_MBS)
        return (VC1HWD_METADATA_ERROR);

    if ((pStorage->curCodedWidth &&
         pStorage->curCodedWidth != pStorage->maxCodedWidth) ||
        (pStorage->curCodedHeight &&
         pStorage->curCodedHeight != pStorage->maxCodedHeight))
        pStorage->resolutionChanged = HANTRO_TRUE;

    pStorage->curCodedWidth = pStorage->maxCodedWidth;
    pStorage->curCodedHeight = pStorage->maxCodedHeight;

    /* PULLDOWN */
    pStorage->pullDown = vc1hwdGetBits(pStrmData, 1);
    /* INTERLACE */
    pStorage->interlace = vc1hwdGetBits(pStrmData, 1);

    /* height of field must be atleast 48 pixels */
    if (pStorage->interlace && 
        ((pStorage->maxCodedHeight>>1) < MIN_PIC_HEIGHT))
        return VC1HWD_METADATA_ERROR;

    /* TFCNTRFLAG */
    pStorage->tfcntrFlag = vc1hwdGetBits(pStrmData, 1);
    /* FINTERPFLAG */
    pStorage->finterpFlag = vc1hwdGetBits(pStrmData, 1);
    /* RESERVED */
    tmp = vc1hwdGetBits(pStrmData, 1);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp != 1)
        return HANTRO_NOK;
#endif
    /* PSF */
    pStorage->psf = vc1hwdGetBits(pStrmData, 1);
    /* DISPLAY_EXT */
    tmp = vc1hwdGetBits(pStrmData, 1);
    if (tmp)
    {
        /* DISP_HORIZ_SIZE */
        pStorage->dispHorizSize = vc1hwdGetBits(pStrmData, 14);
        /* DISP_VERT_SIZE */
        pStorage->dispVertSize = vc1hwdGetBits(pStrmData, 14);
        /* ASPECT_RATIO_FLAG */
        tmp = vc1hwdGetBits(pStrmData, 1);
        if (tmp)
        {
            /* ASPECT_RATIO */
            tmp = vc1hwdGetBits(pStrmData, 4);
            switch (tmp)
            {
                case ASPECT_RATIO_UNSPECIFIED:  w =   0; h =  0; break;
                case ASPECT_RATIO_1_1:          w =   1; h =  1; break;
                case ASPECT_RATIO_12_11:        w =  12; h = 11; break;
                case ASPECT_RATIO_10_11:        w =  10; h = 11; break;
                case ASPECT_RATIO_16_11:        w =  16; h = 11; break;
                case ASPECT_RATIO_40_33:        w =  40; h = 33; break;
                case ASPECT_RATIO_24_11:        w =  24; h = 11; break;
                case ASPECT_RATIO_20_11:        w =  20; h = 11; break;
                case ASPECT_RATIO_32_11:        w =  32; h = 11; break;
                case ASPECT_RATIO_80_33:        w =  80; h = 33; break;
                case ASPECT_RATIO_18_11:        w =  18; h = 11; break;
                case ASPECT_RATIO_15_11:        w =  15; h = 11; break;
                case ASPECT_RATIO_64_33:        w =  64; h = 33; break;
                case ASPECT_RATIO_160_99:       w = 160; h = 99; break;
                case ASPECT_RATIO_EXTENDED: /* 15 */
                    w = vc1hwdGetBits(pStrmData, 8);
                    h = vc1hwdGetBits(pStrmData, 8);
                    if ((w == 0) || (h == 0))
                        w = h = 0;
                    break;
                default:
                    w = 0;
                    h = 0;
                    break;
            }
            /* aspect ratio width */
            pStorage->aspectHorizSize = w;
            /* aspect ratio height */
            pStorage->aspectVertSize = h;
        }
        /* FRAMERATE_FLAG */
        pStorage->frameRateFlag = vc1hwdGetBits(pStrmData, 1);
        if (pStorage->frameRateFlag)
        {
            /* FRAMERATEIND */
            pStorage->frameRateInd = vc1hwdGetBits(pStrmData, 1);
            if (pStorage->frameRateInd == 0)
            {
                /* FRAMERATENR */
                tmp = vc1hwdGetBits(pStrmData, 8);
                switch (tmp)
                {
                    case 1: pStorage->frameRateNr = 24*1000; break;
                    case 2: pStorage->frameRateNr = 25*1000; break;
                    case 3: pStorage->frameRateNr = 30*1000; break;
                    case 4: pStorage->frameRateNr = 50*1000; break;
                    case 5: pStorage->frameRateNr = 60*1000; break;
                    case 6: pStorage->frameRateNr = 48*1000; break;
                    case 7: pStorage->frameRateNr = 72*1000; break;
                    default: pStorage->frameRateNr = (u32)-1; break;
                }
                /* FRAMERATEDR */
                tmp = vc1hwdGetBits(pStrmData, 4);
                switch (tmp)
                {
                    case 1: pStorage->frameRateDr = 1000; break;
                    case 2: pStorage->frameRateDr = 1001; break;
                    default: pStorage->frameRateDr = (u32)-1; break;
                }
            }
            else
            {   /* FRAMERATEEXP */
                tmp = vc1hwdGetBits(pStrmData, 16);
                pStorage->frameRateNr = (tmp+1);
                pStorage->frameRateDr = 32000;
            }
        }
        /* COLOR_FORMAT_FLAG */
        pStorage->colorFormatFlag = vc1hwdGetBits(pStrmData, 1);
        if (pStorage->colorFormatFlag)
        {
            /* COLOR_PRIM */
            pStorage->colorPrim = vc1hwdGetBits(pStrmData, 8);
            /* TRANSFER_CHAR */
            pStorage->transferChar = vc1hwdGetBits(pStrmData, 8);
            /* MATRIX_COEFF */
            pStorage->matrixCoef = vc1hwdGetBits(pStrmData, 8);
        }

    } /* end of DISPLAY_EXT */

    /* HDR_PARAM_FLAG */
    pStorage->hrdParamFlag = vc1hwdGetBits(pStrmData, 1);

    if( pStorage->hrdParamFlag )
    {
        /* HRD_NUM_LEAKY_BUCKETS */
        pStorage->hrdNumLeakyBuckets = vc1hwdGetBits(pStrmData, 5);
        /* BIT_RATE_EXPONENT */
        pStorage->bitRateExponent = vc1hwdGetBits(pStrmData, 4);
        /* BUFFER_SIZE_EXPONENT */
        pStorage->bufferSizeExponent = vc1hwdGetBits(pStrmData, 4);

        /* check if case of re-initialization */
        if (pStorage->hrdRate)
        {
            G1DWLfree(pStorage->hrdRate);
            pStorage->hrdRate = NULL;
        }
        if (pStorage->hrdBuffer)
        {
            G1DWLfree(pStorage->hrdBuffer);
            pStorage->hrdBuffer = NULL;
        }
        pStorage->hrdRate = 
                    G1DWLmalloc( pStorage->hrdNumLeakyBuckets * sizeof(u32) );
        pStorage->hrdBuffer = 
                    G1DWLmalloc( pStorage->hrdNumLeakyBuckets * sizeof(u32) );

        if ( (pStorage->hrdRate == NULL) || (pStorage->hrdBuffer == NULL) )
        {
            G1DWLfree(pStorage->hrdRate);
            G1DWLfree(pStorage->hrdBuffer);
            pStorage->hrdBuffer = NULL;
            pStorage->hrdRate = NULL;

            return (VC1HWD_MEMORY_FAIL);
        }
		G1DWLmemset(pStorage->hrdRate,0,pStorage->hrdNumLeakyBuckets * sizeof(u32));
		G1DWLmemset(pStorage->hrdBuffer,0,pStorage->hrdNumLeakyBuckets * sizeof(u32));

        for (i = 0; i < pStorage->hrdNumLeakyBuckets; i++)
        {
            /* HRD_RATE */
            pStorage->hrdRate[i] = vc1hwdGetBits(pStrmData, 16);
            /* HRD_BUFFER */
            pStorage->hrdBuffer[i] = vc1hwdGetBits(pStrmData, 16);
        }
    }

    /* Sequence headers succesfully decoded */
    pStorage->hdrsDecoded |= HDR_SEQ;

    return VC1HWD_SEQ_HDRS_RDY;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdDecodeEntryPointLayer

        Functional description:
            Decodes entry-point layer of video bit stream

        Inputs:
            pStorage    Stream storage descriptor
            pStrmData   Pointer to input stream data structure

        Outputs:

        Returns:
            VC1HWD_MEMORY_FAIL
            VC1HWD_METADATA_ERROR
            VC1HWD_ENTRY_POINT_HDRS_RDY

------------------------------------------------------------------------------*/
u32 vc1hwdDecodeEntryPointLayer( swStrmStorage_t *pStorage,
                                 strmData_t *pStrmData )
{

/* Variables */

    i32 tmp;
    u32 i;
    u32 w, h;

/* Code */

    ASSERT(pStorage);
    ASSERT(pStrmData);

    /* reset flag indicating already decoded entry point  header */
    pStorage->hdrsDecoded &= ~HDR_ENTRY;

    /* BROKEN_LINK */
    pStorage->brokenLink = vc1hwdGetBits(pStrmData, 1);
    /* CLOSED_ENTRY */
    pStorage->closedEntry = vc1hwdGetBits(pStrmData, 1);

    /* PANSCAN_FLAG */
    pStorage->panScanFlag = vc1hwdGetBits(pStrmData, 1);
    /* REFDIST_FLAG */
    pStorage->refDistFlag = vc1hwdGetBits(pStrmData, 1);
    /* LOOPFILTER */
    pStorage->loopFilter = vc1hwdGetBits(pStrmData, 1);
    /* FASTUVMC */
    pStorage->fastUvMc = vc1hwdGetBits(pStrmData, 1);
    /* EXTENDED_MV */
    pStorage->extendedMv = vc1hwdGetBits(pStrmData, 1);
    /* DQUANT */
    pStorage->dquant = vc1hwdGetBits(pStrmData, 2);
    /* is dquant valid */
    if (pStorage->dquant > 2)
        return (VC1HWD_METADATA_ERROR);

    /* VSTRANSFORM */
    pStorage->vsTransform = vc1hwdGetBits(pStrmData, 1);
    /* OVERLAP */
    pStorage->overlap = vc1hwdGetBits(pStrmData, 1);
    /* QUANTIZER */
    pStorage->quantizer = vc1hwdGetBits(pStrmData, 2);

    /* HRD_FULLNESS */
    if (pStorage->hrdParamFlag)
    {
        /* re-initialize if headers repeated */
        if (pStorage->hrdFullness)
        {
            G1DWLfree(pStorage->hrdFullness);
            pStorage->hrdFullness = NULL;
        }
        pStorage->hrdFullness = 
            G1DWLmalloc(pStorage->hrdNumLeakyBuckets * sizeof(u32));

        if(pStorage->hrdFullness == NULL)
            return (VC1HWD_MEMORY_FAIL);
		G1DWLmemset(pStorage->hrdFullness,0,pStorage->hrdNumLeakyBuckets * sizeof(u32));

        for (i = 0; i < pStorage->hrdNumLeakyBuckets; i++)
            pStorage->hrdFullness[i] = vc1hwdGetBits(pStrmData, 8);
    }
    /* CODED_SIZE_FLAG */
    tmp = vc1hwdGetBits(pStrmData, 1);
    if (tmp)
    {
        tmp = vc1hwdGetBits(pStrmData, 12);
        w = 2*tmp + 2;
        tmp = vc1hwdGetBits(pStrmData, 12);
        h = 2*tmp + 2;

        if ( (pStorage->curCodedWidth != w) ||
             (pStorage->curCodedHeight != h) )
        {
            pStorage->resolutionChanged = HANTRO_TRUE;
        }
        pStorage->curCodedWidth = w;
        pStorage->curCodedHeight = h;

        if ((w > pStorage->maxCodedWidth) || 
            (h > pStorage->maxCodedHeight) )
        {
            return VC1HWD_METADATA_ERROR;
        }
#ifdef ASIC_TRACE_SUPPORT
        if (pStorage->curCodedWidth != pStorage->maxCodedWidth ||
                pStorage->curCodedHeight != pStorage->maxCodedHeight)
            trace_vc1DecodingTools.multiResolution = 1;
#endif
    }
    else
    {
        if ( (pStorage->curCodedWidth != pStorage->maxCodedWidth) ||
             (pStorage->curCodedHeight != pStorage->maxCodedHeight) )
        {
            pStorage->resolutionChanged = HANTRO_TRUE;
        }
        pStorage->curCodedWidth = pStorage->maxCodedWidth;
        pStorage->curCodedHeight = pStorage->maxCodedHeight;
    }
    /* Check against minimum size. Maximum size is check in Seq layer */
    if ( (pStorage->curCodedWidth < MIN_PIC_WIDTH) ||
         (pStorage->curCodedHeight < MIN_PIC_HEIGHT) ||
         (pStorage->interlace && 
        ((pStorage->maxCodedHeight>>1) < MIN_PIC_HEIGHT)) )
        return (VC1HWD_METADATA_ERROR);

    pStorage->picWidthInMbs  = (pStorage->curCodedWidth+15) >> 4;
    pStorage->picHeightInMbs = (pStorage->curCodedHeight+15) >> 4;
    pStorage->numOfMbs       = (pStorage->picWidthInMbs *
                                pStorage->picHeightInMbs);
    if (pStorage->numOfMbs > MAX_NUM_MBS)
        return (VC1HWD_METADATA_ERROR);

    /* EXTENDED_DMV */
    if (pStorage->extendedMv)
        pStorage->extendedDmv = vc1hwdGetBits(pStrmData, 1);
    /* RANGE_MAPY_FLAG */
    pStorage->rangeMapYFlag = vc1hwdGetBits(pStrmData, 1);
    /* RANGE_MAPY */
    if (pStorage->rangeMapYFlag)
        pStorage->rangeMapY = vc1hwdGetBits(pStrmData, 3);
    /* RANGE_MAPUV_FLAG */
    pStorage->rangeMapUvFlag = vc1hwdGetBits(pStrmData, 1);
    /* RANGE_MAPUV */
    if (pStorage->rangeMapUvFlag)
        pStorage->rangeMapUv = vc1hwdGetBits(pStrmData, 3);
#ifdef ASIC_TRACE_SUPPORT
    if (pStorage->rangeMapYFlag || pStorage->rangeMapUvFlag)
        trace_vc1DecodingTools.rangeMapping = 1;
#endif
    
    /* Entry-Point headers succesfully decoded */
    pStorage->hdrsDecoded |= HDR_ENTRY;

    return VC1HWD_ENTRY_POINT_HDRS_RDY;
}

