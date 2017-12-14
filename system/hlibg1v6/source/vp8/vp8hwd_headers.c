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
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp8hwd_headers.c,v $
-  $Revision: 1.15 $
-  $Date: 2010/09/29 11:52:37 $
-
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Includes
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp8hwd_headers.h"
#include "vp8hwd_probs.h"

#include "dwl.h"

#include "vp8hwd_debug.h"

#if 0
    #define STREAM_TRACE(x,y) printf("%-24s-%9d\n", x, y);
    #define VP8DEC_DEBUG(x) printf x
#else
    #define STREAM_TRACE(x,y)
    #define VP8DEC_DEBUG(x) 
#endif

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/* Define here if it's necessary to print errors */
#define DEC_HDRS_ERR(x) 

#define VP8_KEY_FRAME_START_CODE    0x9d012a

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

static void SetupVersion( vp8Decoder_t *dec );
static u32 ScaleDimension( u32 orig, u32 scale );

static u32 DecodeSegmentationData( vpBoolCoder_t *bc, vp8Decoder_t *dec );
static u32 DecodeMbLfAdjustments( vpBoolCoder_t *bc, vp8Decoder_t *dec );
static i32 DecodeQuantizerDelta(vpBoolCoder_t*bc );
static u32 ReadPartitionSize(u8 *cxSize);

static u32 DecodeVp8FrameHeader( u8 *pStrm, u32 strmLen, vpBoolCoder_t*bc, 
                                  vp8Decoder_t* dec );

static u32 DecodeVp7FrameHeader( vpBoolCoder_t*bc, vp8Decoder_t* dec );


/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    SetupVersion

        Set internal flags according to version number in bitstream.
------------------------------------------------------------------------------*/
void SetupVersion( vp8Decoder_t *dec )
{
}

/*------------------------------------------------------------------------------
    ScaleDimension

        Scale frame dimension
------------------------------------------------------------------------------*/
u32 ScaleDimension( u32 orig, u32 scale )
{
    switch(scale)
    {
        case 0:
            return orig;
            break;
        case 1: /* 5/4 */
            return (5*orig)/4;
            break;
        case 2: /* 5/3 */
            return (5*orig)/3;
            break;
        case 3: /* 2 */
            return 2*orig;
            break;
    }
    ASSERT(0);
    return 0;
}

/*------------------------------------------------------------------------------
    vp8hwdDecodeFrameTag

        Decode 3-byte frame tag. pStrm is assumed to contain at least 3
        bytes of valid data.
------------------------------------------------------------------------------*/
void vp8hwdDecodeFrameTag( u8 *pStrm, vp8Decoder_t* dec )
{
    u32 keyFrame = 0;
    u32 showFrame = 1;
    u32 version = 0;
    u32 partSize = 0;

    partSize = (pStrm[1] << 3) |
               (pStrm[2] << 11);
    keyFrame = pStrm[0] & 0x1;
    version = (pStrm[0] >> 1) & 0x7;
    if(dec->decMode == VP8HWD_VP7)
    {
        partSize <<= 1;
        partSize = partSize | ((pStrm[0] >> 4) & 0xF);
        dec->frameTagSize = version >= 1 ? 3 : 4;
    }
    else
    {
        showFrame = (pStrm[0] >> 4) & 0x1;
        partSize = partSize | ((pStrm[0] >> 5) & 0x7);
        dec->frameTagSize = 3;
    }

    dec->showFrame          = showFrame;
    dec->vpVersion          = version;
    dec->offsetToDctParts   = partSize;
    dec->keyFrame           = !keyFrame;

    VP8DEC_DEBUG(("#### FRAME TAG ####\n"));
    VP8DEC_DEBUG(("First partition size    = %d\n", partSize ));
    VP8DEC_DEBUG(("VP version              = %d\n", version ));
    VP8DEC_DEBUG(("Key frame ?             = %d\n", dec->keyFrame ));
    VP8DEC_DEBUG(("Show frame ?            = %d\n", showFrame ));

}


/*------------------------------------------------------------------------------
    vp8hwdDecodeFrameHeader

        Decode frame header, either VP7 or VP8.
------------------------------------------------------------------------------*/
u32 vp8hwdDecodeFrameHeader( u8 *pStrm, u32 strmLen, vpBoolCoder_t*bc, 
                                vp8Decoder_t* dec )
{
    if(dec->decMode == VP8HWD_VP8)
    {
        return DecodeVp8FrameHeader( pStrm, strmLen, bc, dec );
    }
    else
    {
        /* Start bool coder for legacy VP7 header here already. */
        vp8hwdBoolStart(bc, pStrm, strmLen);
        return DecodeVp7FrameHeader( bc, dec );
    }
}

/*------------------------------------------------------------------------------
    DecodeVp8FrameHeader

        Decode VP8 frame header.
------------------------------------------------------------------------------*/
u32 DecodeVp8FrameHeader( u8 *pStrm, u32 strmLen, vpBoolCoder_t*bc, 
                          vp8Decoder_t* dec )
{
    u32 tmp;
    u32 i, j;

    if(dec->keyFrame)
    {
        /* Check stream length */
        if( strmLen >= 7 )
        {
            /* Read our "start code" */
            tmp = (pStrm[0] << 16)|
                  (pStrm[1] << 8 )|
                  (pStrm[2] << 0 );
        }
        else /* Too few bytes in buffer */
        {
            tmp = ~VP8_KEY_FRAME_START_CODE;
        }

        if( tmp != VP8_KEY_FRAME_START_CODE )
        {
            DEC_HDRS_ERR("VP8 Key-frame start code missing or invalid!\n");
            return(HANTRO_NOK);
        }

        pStrm += 3; /* Skip used bytes */

        tmp = (pStrm[1] << 8)|(pStrm[0]); /* Read 16-bit chunk */
        /* Frame width */
        dec->width = tmp & 0x3fff;
        STREAM_TRACE("frame_width", dec->width );

        /* Scaled width */
        tmp >>= 14;
        dec->scaledWidth = ScaleDimension( dec->width, tmp );
        STREAM_TRACE("scaled_width", dec->scaledWidth );

        pStrm += 2; /* Skip used bytes */

        tmp = (pStrm[1] << 8)|(pStrm[0]); /* Read 16-bit chunk */
        /* Frame height */
        dec->height = tmp & 0x3fff;
        STREAM_TRACE("frame_height", dec->height );

        /* Scaled height */
        tmp >>= 14;
        dec->scaledHeight = ScaleDimension( dec->height, tmp );
        STREAM_TRACE("scaled_height", dec->scaledHeight );

        pStrm += 2; /* Skip used bytes */
        strmLen -= 7;
    }

    /* Start bool code, note that we skip first 3/4 bytes used by the
     * frame tag */
    vp8hwdBoolStart(bc, pStrm, strmLen );

    if(dec->keyFrame)
    {       
        /* Color space type */
        dec->colorSpace = (vpColorSpace_e)vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("vp8_color_space", dec->colorSpace );

        /* Clamping type */
        dec->clamping = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("vp8_clamping", dec->clamping);
    }

    /* Segment based adjustments */
    tmp = DecodeSegmentationData( bc, dec );
    if( tmp != HANTRO_OK )
        return tmp;

    /* Loop filter adjustments */
    dec->loopFilterType      = vp8hwdReadBits( bc, 1 );
    dec->loopFilterLevel     = vp8hwdReadBits( bc, 6 );
    dec->loopFilterSharpness = vp8hwdReadBits( bc, 3 );
    STREAM_TRACE("loop_filter_type", dec->loopFilterType);
    STREAM_TRACE("loop_filter_level", dec->loopFilterLevel);
    STREAM_TRACE("loop_filter_sharpness", dec->loopFilterSharpness);

    tmp = DecodeMbLfAdjustments( bc, dec );
    if( tmp != HANTRO_OK )
        return tmp;

    /* Number of DCT partitions */
    tmp = vp8hwdReadBits( bc, 2 );
    STREAM_TRACE("nbr_of_dct_partitions", tmp);
    dec->nbrDctPartitions = 1<<tmp;

    /* Quantizers */
    dec->qpYAc = vp8hwdReadBits( bc, 7 );
    STREAM_TRACE("qp_y_ac", dec->qpYAc);
    dec->qpYDc = DecodeQuantizerDelta( bc );
    dec->qpY2Dc = DecodeQuantizerDelta( bc );
    dec->qpY2Ac = DecodeQuantizerDelta( bc );
    dec->qpChDc = DecodeQuantizerDelta( bc );
    dec->qpChAc = DecodeQuantizerDelta( bc );
    STREAM_TRACE("qp_y_dc", dec->qpYDc);
    STREAM_TRACE("qp_y2_dc", dec->qpY2Dc);
    STREAM_TRACE("qp_y2_ac", dec->qpY2Ac);
    STREAM_TRACE("qp_ch_dc", dec->qpChDc);
    STREAM_TRACE("qp_ch_ac", dec->qpChAc);
    
    /* Frame buffer operations */
    if( !dec->keyFrame )
    {
        /* Refresh golden */
        dec->refreshGolden = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("refresh_golden", dec->refreshGolden);

        /* Refresh alternate */
        dec->refreshAlternate = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("refresh_alternate", dec->refreshAlternate);

        if( dec->refreshGolden == 0 )
        {
            /* Copy to golden */
            dec->copyBufferToGolden = vp8hwdReadBits( bc, 2 );
            STREAM_TRACE("copy_buffer_to_golden", dec->copyBufferToGolden);
        }
        else
            dec->copyBufferToGolden = 0;

        if( dec->refreshAlternate == 0 )
        {
            /* Copy to alternate */
            dec->copyBufferToAlternate = vp8hwdReadBits( bc, 2 );
            STREAM_TRACE("copy_buffer_to_alternate", dec->copyBufferToAlternate);
        }
        else
            dec->copyBufferToAlternate = 0;

        /* Sign bias for golden frame */
        dec->refFrameSignBias[0] = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("sign_bias_golden", dec->refFrameSignBias[0]);

        /* Sign bias for alternate frame */
        dec->refFrameSignBias[1] = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("sign_bias_alternate", dec->refFrameSignBias[1]);

        /* Refresh entropy probs */
        dec->refreshEntropyProbs = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("refresh_entropy_probs", dec->refreshEntropyProbs);

        /* Refresh last */
        dec->refreshLast = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("refresh_last", dec->refreshLast);
    }
    else /* Key frame */
    {
        dec->refreshGolden          = HANTRO_TRUE;
        dec->refreshAlternate       = HANTRO_TRUE;
        dec->copyBufferToGolden     = 0;
        dec->copyBufferToAlternate  = 0;

        /* Refresh entropy probs */
        dec->refreshEntropyProbs = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("refresh_entropy_probs", dec->refreshEntropyProbs);

        dec->refFrameSignBias[0] = 
            dec->refFrameSignBias[1] = 0;
        dec->refreshLast = HANTRO_TRUE;
    }

    /* Make a "backup" of current entropy probabilities if refresh is 
     * not set */
    if(dec->refreshEntropyProbs == HANTRO_FALSE)
    {
        G1DWLmemcpy( &dec->entropyLast, &dec->entropy, 
                   sizeof(vp8EntropyProbs_t));
        G1DWLmemcpy( dec->vp7PrevScanOrder, dec->vp7ScanOrder, 
                   sizeof(dec->vp7ScanOrder));
    }

    /* Coefficient probability update */
    tmp = vp8hwdDecodeCoeffUpdate(bc, dec);
    if( tmp != HANTRO_OK )
        return (tmp);

    /* Coeff skip element used */
    tmp = vp8hwdReadBits( bc, 1 );
    STREAM_TRACE("no_coeff_skip", tmp);
    dec->coeffSkipMode = tmp;

    if( !dec->keyFrame )
    {
        /* Skipped MB probability */
        tmp = vp8hwdReadBits( bc, 8 );
        STREAM_TRACE("prob_skip_mb", tmp);
        dec->probMbSkipFalse = tmp;

        /* Intra MB probability */
        tmp = vp8hwdReadBits( bc, 8 );
        STREAM_TRACE("prob_intra_mb", tmp);
        dec->probIntra = tmp;

        /* Last ref frame probability */
        tmp = vp8hwdReadBits( bc, 8 );
        STREAM_TRACE("prob_ref_frame_0", tmp);
        dec->probRefLast = tmp;

        /* Golden ref frame probability */
        tmp = vp8hwdReadBits( bc, 8 );
        STREAM_TRACE("prob_ref_frame_1", tmp);
        dec->probRefGolden = tmp;

        /* Intra 16x16 pred mode probabilities */
        tmp = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("intra_16x16_prob_update_flag", tmp);
        if( tmp )
        {
            for( i = 0 ; i < 4 ; ++i )
            {
                tmp = vp8hwdReadBits( bc, 8 );
                STREAM_TRACE("intra_16x16_prob", tmp);
                dec->entropy.probLuma16x16PredMode[i] = tmp;
            }
        }

        /* Chroma pred mode probabilities */
        tmp = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("chroma_prob_update_flag", tmp);
        if( tmp )
        {
            for( i = 0 ; i < 3 ; ++i )
            {
                tmp = vp8hwdReadBits( bc, 8 );
                STREAM_TRACE("chroma_prob", tmp);
                dec->entropy.probChromaPredMode[i] = tmp;
            }
        }

        /* Motion vector tree update */
        tmp = vp8hwdDecodeMvUpdate( bc, dec );
        if( tmp != HANTRO_OK )
            return (tmp);
    }
    else
    {
        /* Skipped MB probability */
        tmp = vp8hwdReadBits( bc, 8 );
        STREAM_TRACE("prob_skip_mb", tmp);
        dec->probMbSkipFalse = tmp;
    }

    if(bc->strmError)
        return (HANTRO_NOK);

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------
    DecodeVp7FrameHeader

        Decode VP7 frame header.
------------------------------------------------------------------------------*/
u32 DecodeVp7FrameHeader( vpBoolCoder_t*bc, vp8Decoder_t* dec )
{
    u32 tmp;
    u32 i, j;

    if(dec->keyFrame)
    {
        /* Frame width */
        dec->width = vp8hwdReadBits( bc, 12 );
        STREAM_TRACE("frame_width", dec->width );

        /* Frame height */
        dec->height = vp8hwdReadBits( bc, 12 );
        STREAM_TRACE("frame_height", dec->height );

        /* Scaled width */
        tmp = vp8hwdReadBits( bc, 2 );
        STREAM_TRACE("scaled_width", tmp );
        dec->scaledWidth = ScaleDimension( dec->width, tmp );

        /* Scaled height */
        tmp = vp8hwdReadBits( bc, 2 );
        STREAM_TRACE("scaled_height", tmp );
        dec->scaledHeight = ScaleDimension( dec->height, tmp );
    }

    /* Feature bits */
    {
        const u32 vp70FeatureBits[4] = { 7, 6, 0, 8 };
        const u32 vp71FeatureBits[4] = { 7, 6, 0, 5 };
        const u32 *featureBits;
        if( dec->vpVersion == 0 )   featureBits = vp70FeatureBits;
        else                        featureBits = vp71FeatureBits;
        for( i = 0 ; i < MAX_NBR_OF_VP7_MB_FEATURES ; ++i )
        {
            /* Feature enabled? */
            if( vp8hwdReadBits( bc, 1 ) )
            {
                /* MB-level probability flag */
                tmp = vp8hwdReadBits( bc, 8 ); 

                /* Feature tree probabilities */
                for( j = 0 ; j < 3 ; ++j )
                {
                    if( vp8hwdReadBits( bc, 1 ) )
                        tmp = vp8hwdReadBits( bc, 8 );
                }

                if(featureBits[i])
                {
                    for( j = 0 ; j < 4 ; ++j )
                    {
                        if( vp8hwdReadBits( bc, 1 ) )
                            tmp = vp8hwdReadBits( bc, featureBits[i] );
                    }
                }

                DEC_HDRS_ERR("VP7 MB-level features enabled!\n");
                return(HANTRO_NOK);
            }
        }

        dec->nbrDctPartitions = 1;
    }

    /* Quantizers */
    dec->qpYAc = vp8hwdReadBits( bc, 7 );
    STREAM_TRACE("qp_y_ac", dec->qpYAc);
    dec->qpYDc  = vp8hwdReadBits( bc, 1 ) ? vp8hwdReadBits( bc, 7 ) : dec->qpYAc;
    dec->qpY2Dc = vp8hwdReadBits( bc, 1 ) ? vp8hwdReadBits( bc, 7 ) : dec->qpYAc;
    dec->qpY2Ac = vp8hwdReadBits( bc, 1 ) ? vp8hwdReadBits( bc, 7 ) : dec->qpYAc;
    dec->qpChDc = vp8hwdReadBits( bc, 1 ) ? vp8hwdReadBits( bc, 7 ) : dec->qpYAc;
    dec->qpChAc = vp8hwdReadBits( bc, 1 ) ? vp8hwdReadBits( bc, 7 ) : dec->qpYAc;
    STREAM_TRACE("qp_y_dc", dec->qpYDc);
    STREAM_TRACE("qp_y2_dc", dec->qpY2Dc);
    STREAM_TRACE("qp_y2_ac", dec->qpY2Ac);
    STREAM_TRACE("qp_ch_dc", dec->qpChDc);
    STREAM_TRACE("qp_ch_ac", dec->qpChAc);
    
    /* Frame buffer operations */
    if( !dec->keyFrame )
    {
        /* Refresh golden */
        dec->refreshGolden = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("refresh_golden", dec->refreshGolden);

        if( dec->vpVersion >= 1)
        {
            /* Refresh entropy probs */
            dec->refreshEntropyProbs = vp8hwdReadBits( bc, 1 );
            STREAM_TRACE("refresh_entropy_probs", dec->refreshEntropyProbs);

            /* Refresh last */
            dec->refreshLast = vp8hwdReadBits( bc, 1 );
            STREAM_TRACE("refresh_last", dec->refreshLast);
        }
        else
        {
            dec->refreshEntropyProbs = HANTRO_TRUE;
            dec->refreshLast = HANTRO_TRUE;
        }
    }
    else /* Key frame */
    {
        dec->refreshGolden          = HANTRO_TRUE;
        dec->refreshAlternate       = HANTRO_TRUE;
        dec->copyBufferToGolden     = 0;
        dec->copyBufferToAlternate  = 0;

        /* Refresh entropy probs */
        if( dec->vpVersion >= 1 )
        {
            dec->refreshEntropyProbs = vp8hwdReadBits( bc, 1 );
            STREAM_TRACE("refresh_entropy_probs", dec->refreshEntropyProbs);
        }
        else
        {
            dec->refreshEntropyProbs = HANTRO_TRUE;
        }

        dec->refFrameSignBias[0] = 
            dec->refFrameSignBias[1] = 0;
        dec->refreshLast = HANTRO_TRUE;
    }

    /* Make a "backup" of current entropy probabilities if refresh is 
     * not set */
    if(dec->refreshEntropyProbs == HANTRO_FALSE)
    {
        G1DWLmemcpy( &dec->entropyLast, &dec->entropy, 
                   sizeof(vp8EntropyProbs_t));
        G1DWLmemcpy( dec->vp7PrevScanOrder, dec->vp7ScanOrder, 
                   sizeof(dec->vp7ScanOrder));
    }

    /* Faded reference frame (NOT IMPLEMENTED) */
    if(dec->refreshLast != 0)
    {
        tmp = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("use_faded_reference", tmp);
        if( tmp == 1 )
        {
            /* Alpha */
            tmp = vp8hwdReadBits( bc, 8 );
            /* Beta */
            tmp = vp8hwdReadBits( bc, 8 );
            DEC_HDRS_ERR("Faded reference used!\n");
            return(HANTRO_NOK);
        }
    }
    /* Loop filter type (for VP7 version 0) */
    if(dec->vpVersion == 0)
    {
        dec->loopFilterType = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("loop_filter_type", dec->loopFilterType);
    }

    /* Scan order update? */
    tmp = vp8hwdReadBits( bc, 1 );
    STREAM_TRACE("vp7_scan_order_update_flag", tmp);
    if(tmp)
    {
        u32 newOrder[16] = { 0 };
        for( i = 1 ; i < 16 ; ++i )
        {
            tmp = vp8hwdReadBits( bc, 4 );
            STREAM_TRACE("scan_index", tmp);
            newOrder[i] = tmp; 
        }
        vp8hwdPrepareVp7Scan( dec, newOrder );
    }

    /* Loop filter type (for VP7 version 1 and later) */
    if(dec->vpVersion >= 1)
    {
        dec->loopFilterType = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("loop_filter_type", dec->loopFilterType);
    }

    /* Loop filter adjustments */
    dec->loopFilterLevel     = vp8hwdReadBits( bc, 6 );
    dec->loopFilterSharpness = vp8hwdReadBits( bc, 3 );
    STREAM_TRACE("loop_filter_level", dec->loopFilterLevel);
    STREAM_TRACE("loop_filter_sharpness", dec->loopFilterSharpness);

    /* Coefficient probability update */
    tmp = vp8hwdDecodeCoeffUpdate(bc, dec);
    if( tmp != HANTRO_OK )
        return (tmp);

    if( !dec->keyFrame )
    {
        /* Intra MB probability */
        tmp = vp8hwdReadBits( bc, 8 );
        STREAM_TRACE("prob_intra_mb", tmp);
        dec->probIntra = tmp;

        /* Last ref frame probability */
        tmp = vp8hwdReadBits( bc, 8 );
        STREAM_TRACE("prob_ref_frame_0", tmp);
        dec->probRefLast = tmp;

        /* Intra 16x16 pred mode probabilities */
        tmp = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("intra_16x16_prob_update_flag", tmp);
        if( tmp )
        {
            for( i = 0 ; i < 4 ; ++i )
            {
                tmp = vp8hwdReadBits( bc, 8 );
                STREAM_TRACE("intra_16x16_prob", tmp);
                dec->entropy.probLuma16x16PredMode[i] = tmp;
            }
        }

        /* Chroma pred mode probabilities */
        tmp = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("chroma_prob_update_flag", tmp);
        if( tmp )
        {
            for( i = 0 ; i < 3 ; ++i )
            {
                tmp = vp8hwdReadBits( bc, 8 );
                STREAM_TRACE("chroma_prob", tmp);
                dec->entropy.probChromaPredMode[i] = tmp;
            }
        }

        /* Motion vector tree update */
        tmp = vp8hwdDecodeMvUpdate( bc, dec );
        if( tmp != HANTRO_OK )
            return (tmp);
    }

    if(bc->strmError)
        return (HANTRO_NOK);

    return (HANTRO_OK);
}


/*------------------------------------------------------------------------------
    DecodeQuantizerDelta

        Decode VP8 delta-coded quantizer value
------------------------------------------------------------------------------*/
i32 DecodeQuantizerDelta(vpBoolCoder_t*bc )
{
    u32 sign;
    i32 delta;

    if( vp8hwdReadBits( bc, 1 ) )
    {
        delta = vp8hwdReadBits(bc, 4);
        sign = vp8hwdReadBits(bc, 1);
        if( sign )
            delta = -delta;
        return delta;
    }
    else
    {
        return 0;
    }
}

/*------------------------------------------------------------------------------
    vp8hwdSetPartitionOffsets

        Read partition offsets from stream and initialize into internal 
        structures.
------------------------------------------------------------------------------*/
u32 vp8hwdSetPartitionOffsets( u8 *stream, u32 len, vp8Decoder_t *dec )
{
    u32 i = 0;
    u32 offset = 0;
    u32 baseOffset;
    u32 extraBytesPacked = 0;

    if(dec->decMode == VP8HWD_VP8 &&
       dec->keyFrame)
        extraBytesPacked += 7;

    stream += dec->frameTagSize;

    baseOffset = dec->frameTagSize + dec->offsetToDctParts +
                 3*(dec->nbrDctPartitions-1);

    stream += dec->offsetToDctParts + extraBytesPacked;
    for( i = 0 ; i < dec->nbrDctPartitions - 1 ; ++i )
    {
        dec->dctPartitionOffsets[i] = baseOffset + offset;
        offset += ReadPartitionSize( stream );
        stream += 3;
    }
    dec->dctPartitionOffsets[i] = baseOffset + offset;

    return (dec->dctPartitionOffsets[i] < len ? HANTRO_OK : HANTRO_NOK);

}


/*------------------------------------------------------------------------------
    ReadPartitionSize

        Read partition size from stream.
------------------------------------------------------------------------------*/
u32 ReadPartitionSize(u8 *cxSize)
{
	u32 size; 
	size =	(u32)(*cxSize)
		 +	((u32)(* (cxSize+1))<<8) 
		 +  ((u32)(* (cxSize+2))<<16); 
	return size; 
}


/*------------------------------------------------------------------------------
    DecodeSegmentationData

        Decode segment-based adjustments from bitstream.
------------------------------------------------------------------------------*/
u32 DecodeSegmentationData( vpBoolCoder_t *bc, vp8Decoder_t *dec )
{
    u32 tmp;
    u32 sign;
    u32 j;

    /* Segmentation enabled? */
    dec->segmentationEnabled = vp8hwdReadBits( bc, 1 );
    STREAM_TRACE("segmentation_enabled", dec->segmentationEnabled);
    if( dec->segmentationEnabled )
    {
        /* Segmentation map update */
        dec->segmentationMapUpdate = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("segmentation_map_update", dec->segmentationMapUpdate);
        /* Segment feature data update */
        tmp = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("segment_feature_data_update", tmp);
        if( tmp )
        {
            /* Absolute/relative mode */
            dec->segmentFeatureMode = vp8hwdReadBits( bc, 1 );
            STREAM_TRACE("segment_feature_mode", dec->segmentFeatureMode);

            /* TODO: what to do with negative numbers if absolute mode? */
            /* Quantiser */
            for( j = 0 ; j < MAX_NBR_OF_SEGMENTS ; ++j )
            {
                /* Feature data update ? */
                tmp = vp8hwdReadBits( bc, 1 );
                STREAM_TRACE("quantizer_update_flag", tmp);
                if( tmp )
                {
                    /* Payload */
                    tmp = vp8hwdReadBits( bc, 7 );
                    /* Sign */
                    sign = vp8hwdReadBits( bc, 1 );
                    STREAM_TRACE("quantizer_payload", tmp);
                    STREAM_TRACE("quantizer_sign", sign);
                    dec->segmentQp[j] = tmp;
                    if( sign )
                        dec->segmentQp[j] = -dec->segmentQp[j];
                }
            }

            /* Loop filter level */
            for( j = 0 ; j < MAX_NBR_OF_SEGMENTS ; ++j )
            {
                /* Feature data update ? */
                tmp = vp8hwdReadBits( bc, 1 );
                STREAM_TRACE("loop_filter_update_flag", tmp);
                if( tmp )
                {
                    /* Payload */
                    tmp = vp8hwdReadBits( bc, 6 );
                    /* Sign */
                    sign = vp8hwdReadBits( bc, 1 );
                    STREAM_TRACE("loop_filter_payload", tmp);
                    STREAM_TRACE("loop_filter_sign", sign);
                    dec->segmentLoopfilter[j] = tmp;
                    if( sign )
                        dec->segmentLoopfilter[j] = -dec->segmentLoopfilter[j];
                }
            }

        }

        /* Segment probabilities */
        if(dec->segmentationMapUpdate)
        {

                //dec->probSegment[1] =
            dec->probSegment[0] =
                dec->probSegment[1] = 255;
            for( j = 0 ; j < 3 ; ++j )
            {
                tmp = vp8hwdReadBits( bc, 1 );
                STREAM_TRACE("segment_prob_update_flag", tmp);
                if( tmp )
                {
                    tmp = vp8hwdReadBits( bc, 8 );
                    STREAM_TRACE("segment_prob", tmp);
                    dec->probSegment[j] = tmp;
                }
            }
        }
    } /* SegmentationEnabled */

    if(bc->strmError)
        return (HANTRO_NOK);

    return (HANTRO_OK);
}


/*------------------------------------------------------------------------------
    DecodeMbLfAdjustments

        Decode MB loop filter adjustments from bitstream.
------------------------------------------------------------------------------*/
u32 DecodeMbLfAdjustments( vpBoolCoder_t*bc, vp8Decoder_t* dec )
{
    u32 sign;
    u32 tmp;
    u32 j;

    /* Adjustments enabled? */
    dec->modeRefLfEnabled = vp8hwdReadBits( bc, 1 );
    STREAM_TRACE("loop_filter_adj_enable", dec->modeRefLfEnabled);
    
    if( dec->modeRefLfEnabled ) 
    {
        /* Mode update? */
        tmp = vp8hwdReadBits( bc, 1 );
        STREAM_TRACE("loop_filter_adj_update_flag", tmp);
        if( tmp )
        {
			/* Reference frame deltas */
			for ( j = 0; j < MAX_NBR_OF_MB_REF_LF_DELTAS; j++ )
			{
                tmp = vp8hwdReadBits( bc, 1 );
                STREAM_TRACE("ref_frame_delta_update_flag", tmp);
                if( tmp )
				{
                    /* Payload */
                    tmp = vp8hwdReadBits( bc, 6 );
                    /* Sign */
                    sign = vp8hwdReadBits( bc, 1 );
                    STREAM_TRACE("loop_filter_payload", tmp);
                    STREAM_TRACE("loop_filter_sign", sign);

                    dec->mbRefLfDelta[j] = tmp;
                    if( sign )
                        dec->mbRefLfDelta[j] = -dec->mbRefLfDelta[j];
				}
			}

			/* Mode deltas */
			for ( j = 0; j < MAX_NBR_OF_MB_MODE_LF_DELTAS; j++ )
			{
                tmp = vp8hwdReadBits( bc, 1 );
                STREAM_TRACE("mb_type_delta_update_flag", tmp);
                if( tmp )
				{
                    /* Payload */
                    tmp = vp8hwdReadBits( bc, 6 );
                    /* Sign */
                    sign = vp8hwdReadBits( bc, 1 );
                    STREAM_TRACE("loop_filter_payload", tmp);
                    STREAM_TRACE("loop_filter_sign", sign);

                    dec->mbModeLfDelta[j] = tmp;
                    if( sign )
                        dec->mbModeLfDelta[j] = -dec->mbModeLfDelta[j];
				}
			}
		}
    } /* Mb mode/ref lf adjustment */

    if(bc->strmError)
        return (HANTRO_NOK);

    return (HANTRO_OK);
}
