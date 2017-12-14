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
--  Description : Decoder top level control for sequence and picture layer
--                decoding.
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_decoder.c,v $
--  $Revision: 1.54 $
--  $Date: 2010/12/01 12:31:04 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1decapi.h"
#include "vc1hwd_decoder.h"
#include "vc1hwd_stream.h"
#include "vc1hwd_asic.h"
#include "vc1hwd_picture_layer.h"
#include "refbuffer.h"
#include "vc1hwd_regdrv.h"
#include "bqueue.h"

/*------------------------------------------------------------------------------
    External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define SHOW1(p) (p[0]); p+=1;
#define SHOW4(p) (p[0]) | (p[1]<<8) | (p[2]<<16) | (p[3]<<24); p+=4;

#define    BIT0(tmp)  ((tmp & 1)   >>0);
#define    BIT1(tmp)  ((tmp & 2)   >>1);
#define    BIT2(tmp)  ((tmp & 4)   >>2);
#define    BIT3(tmp)  ((tmp & 8)   >>3);
#define    BIT4(tmp)  ((tmp & 16)  >>4);
#define    BIT5(tmp)  ((tmp & 32)  >>5);
#define    BIT6(tmp)  ((tmp & 64)  >>6);
#define    BIT7(tmp)  ((tmp & 128) >>7);

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

static u16x ValidateMetadata(swStrmStorage_t * pStorage,
                             const VC1DecMetaData *pMetaData);

static u16x AllocateMemories( decContainer_t *pDecCont, 
                              swStrmStorage_t *pStorage, 
                              const void *dwl );

/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function name: vc1hwdInit

        Functional description:
                    Initializes decoder and allocates memories.

        Inputs:
            dwl             Instance of decoder wrapper layer
            pStorage        Stream storage descriptor
            pMetaData       Pointer to metadata information

        Outputs:
            None

        Returns:
            VC1HWD_METADATA_ERROR / VC1HWD_OK

------------------------------------------------------------------------------*/
u16x vc1hwdInit(const void *dwl, swStrmStorage_t *pStorage,
    const VC1DecMetaData *pMetaData,
    u32 numFrameBuffers )
{
    u16x rv;

    ASSERT(pStorage);
    ASSERT(pMetaData);

    /* init stream storage structure */
    (void)G1DWLmemset(pStorage, 0, sizeof(swStrmStorage_t));

    /* check initialization data if not advanced profile stream */
    if (pMetaData->profile != 8)
    {
        rv = ValidateMetadata(pStorage, pMetaData);
        if (rv != VC1HWD_OK)
            return (VC1HWD_METADATA_ERROR);
    }
    /* Advanced profile */
    else
    {
        pStorage->maxBframes = 7;
        pStorage->profile = VC1_ADVANCED;
    }

    if( pStorage->maxBframes > 0 )
        pStorage->minCount = 1;
    else
        pStorage->minCount = 0;

    if( numFrameBuffers > 16 )
        numFrameBuffers = 16;

    pStorage->maxNumBuffers = numFrameBuffers;

    pStorage->work0 = pStorage->work1 = INVALID_ANCHOR_PICTURE;
    pStorage->workOutPrev = 
        pStorage->workOut = BqueueNext( 
        &pStorage->bq,
        pStorage->work0,
        pStorage->work1,
        BQUEUE_UNUSED,
        0 );
    pStorage->firstFrame = HANTRO_TRUE;
    pStorage->pictureBroken = HANTRO_FALSE;

    return (VC1HWD_OK);

}
/*------------------------------------------------------------------------------

    Function name: AllocateMemories

        Functional description:
            Allocates decoder internal memories

        Inputs:
            pDecCont    Decoder container
            pStorage    Stream storage descriptor
            dwl         Instance of decoder wrapper layer

        Outputs:
            None

        Returns:
            VC1HWD_MEMORY_FAIL / VC1HWD_OK

------------------------------------------------------------------------------*/
u16x AllocateMemories( decContainer_t *pDecCont, 
                       swStrmStorage_t *pStorage, 
                       const void *dwl )
{
    u16x rv,i;
    u16x size;
    picture_t *pPic = NULL;
    u32 buffers;

    if( pStorage->maxBframes > 0 )
    {
        buffers = 3;
    }
    else
    {
        buffers = 2;
    }

    /* Calculate minimum amount of buffers */
    pStorage->parallelMode2 = 0;
    pDecCont->ppControl.prevAnchorDisplayIndex = BQUEUE_UNUSED;
    if( pDecCont->ppInstance ) /* Combined mode used */
    {

        /* If low-delay not set, then we might need extra buffers in
         * case we're using multibuffer mode. */
        if( pStorage->maxBframes > 0 )
        {
            /*
            pDecCont->PPConfigQuery(pDecCont->ppInstance,
                                         &pDecCont->ppConfigQuery);
            if(pDecCont->ppConfigQuery.multiBuffer)
            */
            buffers = 4; 
        }

        pStorage->numPpBuffers = pStorage->maxNumBuffers;
        pStorage->workBufAmount = buffers; /* Use bare minimum in decoder */
        buffers = 2; /*pStorage->interlace ? 1 : 2;*/
        if( pStorage->numPpBuffers < buffers )
            pStorage->numPpBuffers = buffers;
    }
    else /* Dec only or separate PP */
    {
        pStorage->workBufAmount = pStorage->maxNumBuffers;
        pStorage->numPpBuffers = 0;
        if( pStorage->workBufAmount < buffers )
            pStorage->workBufAmount = buffers;
    }

    rv = BqueueInit(&pStorage->bq, 
                     pStorage->workBufAmount );
    if(rv != HANTRO_OK)
    {
        (void)vc1hwdRelease(dwl, pStorage);
        return (VC1HWD_MEMORY_FAIL);
    }

    rv = BqueueInit(&pStorage->bqPp, 
                     pStorage->numPpBuffers );
    if(rv != HANTRO_OK)
    {
        (void)vc1hwdRelease(dwl, pStorage);
        return (VC1HWD_MEMORY_FAIL);
    }

    /* Memory for all picture_t structures */
    pPic = (picture_t*)G1DWLmalloc(sizeof(picture_t) * pStorage->workBufAmount);
    if(pPic == NULL)
    {
        (void)vc1hwdRelease(dwl, pStorage);
        return (VC1HWD_MEMORY_FAIL);
    }
    (void)G1DWLmemset(pPic, 0, sizeof(picture_t)*pStorage->workBufAmount);

    /* set pointer to picture_t buffer */
    pStorage->pPicBuf = (const struct picture*)pPic;

    size = pStorage->numOfMbs*384;      /* size of YUV picture */

    /* memory for pictures */
    for (i = 0; i < pStorage->workBufAmount; i++)
    {
        if (G1DWLMallocRefFrm(dwl, size, &pPic[i].data) != 0)
        {
            (void)vc1hwdRelease(dwl, pStorage);
            return (VC1HWD_MEMORY_FAIL);
        }
        (void)G1DWLmemset(pPic[i].data.virtualAddress, 0 , size);
        /* init coded image size to max coded image size */
        pPic[i].codedWidth = pStorage->maxCodedWidth;
        pPic[i].codedHeight = pStorage->maxCodedHeight;
    }

    pStorage->pMbFlags = (u8*)G1DWLmalloc(((pStorage->numOfMbs+9)/10)*10);
    if (pStorage->pMbFlags == NULL)
    {
        (void)vc1hwdRelease(dwl, pStorage);
        return (VC1HWD_MEMORY_FAIL);
    }
    (void)G1DWLmemset(pStorage->pMbFlags, 0, ((pStorage->numOfMbs+9)/10)*10);

    /* bit plane coded data, 3 bits per macroblock, have to allocate integer
     * number of 4-byte words */
    rv = G1DWLMallocLinear(dwl,
        (pStorage->numOfMbs + 9)/10 * sizeof(u32),
        &pDecCont->bitPlaneCtrl);
    if ((rv != 0) ||  
        X170_CHECK_VIRTUAL_ADDRESS( pDecCont->bitPlaneCtrl.virtualAddress ) ||
        X170_CHECK_BUS_ADDRESS( pDecCont->bitPlaneCtrl.busAddress ))
    {
        (void)vc1hwdRelease(dwl, &pDecCont->storage);
        return (VC1HWD_MEMORY_FAIL);
    }

    /* If B pictures in the stream allocate space for direct mode mvs */
    if(pDecCont->storage.maxBframes)
    {
        /* allocate for even number of macroblock rows to accommodate direct
         * mvs of field pictures */
        if (pDecCont->storage.picHeightInMbs & 0x1)
        {
            rv = G1DWLMallocLinear(pDecCont->dwl,
                ((pDecCont->storage.numOfMbs +
                  pDecCont->storage.picWidthInMbs+7) & ~0x7) * 2 * sizeof(u32),
                &pDecCont->directMvs);
        }
        else
        {
            rv = G1DWLMallocLinear(pDecCont->dwl,
                ((pDecCont->storage.numOfMbs+7) & ~0x7) * 2 * sizeof(u32),
                &pDecCont->directMvs);
        }
        if (rv != 0 ||
            X170_CHECK_VIRTUAL_ADDRESS( pDecCont->directMvs.virtualAddress ) ||
            X170_CHECK_BUS_ADDRESS( pDecCont->directMvs.busAddress ))
        {
            DWLFreeLinear(dwl, &pDecCont->bitPlaneCtrl);
            (void)vc1hwdRelease(dwl, &pDecCont->storage);
            return (VC1HWD_MEMORY_FAIL);
        }
    }
    else
        pDecCont->directMvs.virtualAddress = 0;

    return (VC1HWD_OK);
}

/*------------------------------------------------------------------------------

    Function name: ValidateMetadata

        Functional description:
                    Function checks that all necessary initialization metadata
                    are present in metadata structure and values are sane.

        Inputs:
            pStorage        Stream storage descriptor
            pMetaData       Pointer to metadata information

        Outputs:

        Returns:
            VC1HWD_OK / VC1HWD_METADATA_ERROR

------------------------------------------------------------------------------*/
u16x ValidateMetadata(swStrmStorage_t *pStorage,const VC1DecMetaData *pMetaData)
{

    DWLHwConfig_t config;

    (void)G1DWLmemset(&config, 0, sizeof(DWLHwConfig_t));

    DWLReadAsicConfig(&config);

    ASSERT( config.maxDecPicWidth > 0 );
    ASSERT( config.maxDecPicWidth < 1921 );

    /* check metadata information */
    if ( pMetaData->maxCodedWidth < MIN_PIC_WIDTH ||
         pMetaData->maxCodedWidth > config.maxDecPicWidth ||
         pMetaData->maxCodedHeight < MIN_PIC_HEIGHT ||
         pMetaData->maxCodedHeight > config.maxDecPicWidth ||
         (pMetaData->maxCodedWidth & 0x1) ||
         (pMetaData->maxCodedHeight & 0x1) ||
         pMetaData->quantizer > 3 )
        return (VC1HWD_METADATA_ERROR);

    pStorage->curCodedWidth     = pStorage->maxCodedWidth
                                = pMetaData->maxCodedWidth;
    pStorage->curCodedHeight    = pStorage->maxCodedHeight
                                = pMetaData->maxCodedHeight;
    pStorage->picWidthInMbs  = (pMetaData->maxCodedWidth+15) >> 4;
    pStorage->picHeightInMbs = (pMetaData->maxCodedHeight+15) >> 4;
    pStorage->numOfMbs       = (pStorage->picWidthInMbs *
                                pStorage->picHeightInMbs);

    if (pStorage->numOfMbs > MAX_NUM_MBS)
        return(VC1HWD_METADATA_ERROR);

    pStorage->vsTransform       = pMetaData->vsTransform ? 1 : 0;
    pStorage->overlap           = pMetaData->overlap ? 1 : 0;
    pStorage->syncMarker        = pMetaData->syncMarker ? 1 : 0;
    pStorage->frameInterpFlag   = pMetaData->frameInterp ? 1 : 0;
    pStorage->quantizer         = pMetaData->quantizer;

    pStorage->maxBframes        = pMetaData->maxBframes;
    pStorage->fastUvMc          = pMetaData->fastUvMc ? 1 : 0;
    pStorage->extendedMv        = pMetaData->extendedMv ? 1 : 0;
    pStorage->multiRes          = pMetaData->multiRes ? 1 : 0;
    pStorage->rangeRed          = pMetaData->rangeRed ? 1 : 0;
    pStorage->dquant            = pMetaData->dquant;
    pStorage->loopFilter        = pMetaData->loopFilter ? 1 : 0;
    pStorage->profile           = 
                (pMetaData->profile == 0) ? VC1_SIMPLE : VC1_MAIN;

    /* is dquant valid */
    if (pStorage->dquant > 2)
        return (VC1HWD_METADATA_ERROR);


    /* Quantizer specification. > 3 is SMPTE reserved */
    if (pMetaData->quantizer > 3)
        return (VC1HWD_METADATA_ERROR);

    /* maximum number of consecutive B frames. > 7 SMPTE reserved */
    if (pMetaData->maxBframes > 7)
        return (VC1HWD_METADATA_ERROR);

    return (VC1HWD_OK);
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdDecode

        Functional description:
            Control for sequence and picture layer decoding

        Inputs:
            pDecCont    Decoder container
            pStorage    Stream storage descriptor
            streamData  Descriptor for stream data

        Outputs:
            None

        Returns:
            VC1HWD_ERROR
            VC1HWD_END_OF_SEQ
            VC1HWD_USER_DATA_RDY
            VC1HWD_METADATA_ERROR
            VC1HWD_SEQ_HDRS_RDY
            VC1HWD_PIC_HDRS_RDY
            VC1HWD_FIELD_HDRS_RDY
            VC1HWD_ENTRY_POINT_HDRS_RDY
            VC1HWD_USER_DATA_RDY
            VC1HWD_NOT_CODED_PIC
            VC1HWD_MEMORY_FAIL
            VC1HWD_HDRS_ERROR

------------------------------------------------------------------------------*/
u16x vc1hwdDecode( decContainer_t *pDecCont, 
                   swStrmStorage_t *pStorage, 
                   strmData_t *streamData )
{
/* Variables */

    u32 startCode;
    u32 tmp;
    u32 readBitsStart, readBitsEnd;
    u16x retVal = HANTRO_OK;

/* Code */

    ASSERT(pStorage);
    ASSERT(streamData);

    /* ADVANCED PROFILE */
    if (pStorage->profile == VC1_ADVANCED)
    {
        pStorage->slice = HANTRO_FALSE;
        pStorage->missingField = HANTRO_FALSE;
        do {
            /* Get Start Code */
            startCode = vc1hwdGetStartCode(streamData);
            switch (startCode)
            {
                case SC_SEQ:
                    DPRINT(("\nSC_SEQ found\n"));
                    /* skip seq layer decoding after the first 
                     * pic is successfully decoded*/
                    if (pStorage->firstFrame)
                    {
                        retVal = vc1hwdDecodeSequenceLayer(pStorage,streamData);
                        if( pDecCont->refBufSupport )
                        {
                            RefbuInit( &pDecCont->refBufferCtrl, DEC_X170_MODE_VC1, 
                                       pDecCont->storage.picWidthInMbs,
                                       pDecCont->storage.picHeightInMbs,
                                       pDecCont->refBufSupport );
                        }
                    }
                    else
                    {   /* need to seek new SC prefix */
                        retVal = vc1hwdGetUserData(pStorage, streamData);
                        /* return user data ready even if stream ends */
                        retVal = VC1HWD_USER_DATA_RDY;
                    }
                    break;
                case SC_SEQ_UD:
                    DPRINT(("\nSC_SEQ_UD found\n"));
                    retVal = vc1hwdGetUserData(pStorage, streamData);
                    retVal = VC1HWD_USER_DATA_RDY;
                    break;
                case SC_ENTRY_POINT:
                    DPRINT(("\nSC_ENTRY_POINT found\n"));

                    /* Check that sequence headers are decoded succesfully */
                    if (!(pStorage->hdrsDecoded & HDR_SEQ))
                        return VC1HWD_HDRS_ERROR;

                    retVal = vc1hwdDecodeEntryPointLayer(pStorage, streamData);
                    break;
                case SC_ENTRY_POINT_UD:
                    DPRINT(("\nSC_ENTRY_POINT_UD found\n"));
                    retVal = vc1hwdGetUserData(pStorage, streamData);
                    retVal = VC1HWD_USER_DATA_RDY;
                    break;
                case SC_FRAME:
                    DPRINT(("\nSC_FRAME found\n"));
                    
                    /* Check that headers are decoded succesfully */
                    if (pStorage->hdrsDecoded != HDR_BOTH)
                        return VC1HWD_HDRS_ERROR;

                    readBitsStart = streamData->strmBuffReadBits;
                    retVal = vc1hwdDecodePictureLayerAP(pStorage, streamData);
                    
                    readBitsEnd = streamData->strmBuffReadBits;
                    pStorage->picLayer.picHeaderBits = 
                                    readBitsEnd - readBitsStart;

                    /* Set anchor frame/field inter/intra status */
                    if( pStorage->picLayer.fcm == FIELD_INTERLACE )
                        tmp = pStorage->picLayer.tff ^ 1;
                    else
                        tmp = 0;
                    if( pStorage->picLayer.picType == PTYPE_I )
                        pStorage->anchorInter[ tmp ] = HANTRO_FALSE;
                    else if ( (pStorage->picLayer.picType == PTYPE_P) || 
                              (pStorage->picLayer.picType == PTYPE_Skip) )
                        pStorage->anchorInter[ tmp ] = HANTRO_TRUE;
                    
                    /* flag used to detect missing field */ 
                    if( pStorage->picLayer.fcm == FIELD_INTERLACE )
                        pStorage->ffStart = HANTRO_TRUE;
                    else
                        pStorage->ffStart = HANTRO_FALSE;

                    /* previous frame was field coded and 
                     * second field missing */
                    if ( (pStorage->pPicBuf[pStorage->workOut].fcm == 
                                                        FIELD_INTERLACE) &&
                         (pStorage->pPicBuf[pStorage->workOut].isFirstField == 
                                                        HANTRO_TRUE) )
                    {
                        /* replace orphan 1.st field of previous frame in
                         * frame buffer */
                        if (pStorage->fieldCount)
                            pStorage->fieldCount--;
                        if (pStorage->outpCount)
                            pStorage->outpCount--;
                        BqueueDiscard( &pStorage->bq, pStorage->workOut );
                        pStorage->workOutPrev =
                        pStorage->workOut = pStorage->work0;

                        EPRINT(("MISSING SECOND FIELD\n"));
                        pStorage->missingField = HANTRO_TRUE;
                    }
                    break;
                case SC_FRAME_UD:
                    DPRINT(("\nSC_FRAME_UD found\n"));
                    retVal = vc1hwdGetUserData(pStorage, streamData);
                    retVal = VC1HWD_USER_DATA_RDY;
                    break;
                case SC_FIELD:
                    DPRINT(("\nSC_FIELD found\n"));

                    /* If FCM at this point is other than field interlace, 
                     * then we have an extra field in the stream. Let's skip
                     * it altogether and move on to the next SC. */
                    if( pStorage->picLayer.fcm != FIELD_INTERLACE )
                        continue; 

                    /* Check that headers are decoded succesfully */
                    if (pStorage->hdrsDecoded != HDR_BOTH)
                        return VC1HWD_HDRS_ERROR;

                    /* previous frame finished and field SC found 
                     * -> first field missing -> conceal */
                    if( (pStorage->picLayer.fcm == FIELD_INTERLACE) &&
                        (pStorage->ffStart == HANTRO_FALSE))
                    {
                        EPRINT(("MISSING FIRST FIELD\n"));
                        pStorage->missingField = HANTRO_TRUE;
                        return VC1HWD_ERROR;
                    }
                    pStorage->ffStart = HANTRO_FALSE;
                    
                    /* Substract field header bits from total length */
                    pStorage->picLayer.picHeaderBits -= 
                        pStorage->picLayer.fieldHeaderBits;

                    retVal = vc1hwdDecodeFieldLayer( pStorage, 
                                                     streamData,
                                                     HANTRO_FALSE );
                    /* And add length of new header */
                    pStorage->picLayer.picHeaderBits += 
                        pStorage->picLayer.fieldHeaderBits;

                    /* Set anchor field inter/intra status */
                    if( pStorage->picLayer.picType == PTYPE_I )
                    {
                        pStorage->anchorInter[ pStorage->picLayer.tff ] = 
                                                                HANTRO_FALSE;
                    }
                    else if ( (pStorage->picLayer.picType == PTYPE_P) ||
                              (pStorage->picLayer.picType == PTYPE_Skip) )
                    {
                        pStorage->anchorInter[ pStorage->picLayer.tff ] = 
                                                                HANTRO_TRUE;
                    }
                    break;
                case SC_FIELD_UD:
                    DPRINT(("\nSC_FIELD_UD found\n"));
                    retVal = vc1hwdGetUserData(pStorage, streamData);
                    retVal = VC1HWD_USER_DATA_RDY;
                    break;
                case SC_SLICE:
                    DPRINT(("\nSC_SLICE found\n"));
                    
                    /* Check that headers are decoded succesfully */
                    if (pStorage->hdrsDecoded != HDR_BOTH)
                        return VC1HWD_HDRS_ERROR;

                    /* If we found slice start code, and picture type is skipped,
                     * it must be an error. */
                    if( pStorage->picLayer.picType == PTYPE_Skip )
                        return VC1HWD_ERROR;
                    
                    retVal = VC1HWD_PIC_HDRS_RDY;
                    pStorage->slice = HANTRO_TRUE;
                    
                    /* unflush startcode */
                    streamData->strmBuffReadBits-=32;
                    streamData->pStrmCurrPos -= 4;
                    break;
                case SC_SLICE_UD:
                    DPRINT(("\nSC_SLICE_UD found\n"));
                    retVal = VC1HWD_USER_DATA_RDY;
                    break;
                case SC_END_OF_SEQ:
                    DPRINT(("\nSC_END_OF_SEQ found\n"));
                    return (VC1HWD_END_OF_SEQ);
                case SC_NOT_FOUND:
                    retVal = vc1hwdFlushBits(streamData, 8);
                    break;
                default:
                    EPRINT(("\nSC_ERROR found\n"));
                    retVal = HANTRO_NOK;
                    if (pStorage->hdrsDecoded != HDR_BOTH)
                        return (VC1HWD_HDRS_ERROR);
                    break;
            }
            /* check stream exhaustion */
            if(vc1hwdIsExhausted(streamData))
            {
                /* Notice!! VC1HWD_USER_DATA_RDY is used to return
                 * VC1DEC_STRM_PROCESSED from the API in these cases */
                if ( (startCode == SC_NOT_FOUND) ||
                     (retVal == VC1HWD_USER_DATA_RDY) )
                {
                    return VC1HWD_USER_DATA_RDY;
                }
                
                EPRINT(("Stream exhausted!"));
                retVal = HANTRO_NOK;
            }
        } while ( (retVal != HANTRO_NOK) && 
                  (retVal != VC1HWD_METADATA_ERROR ) &&
                  (retVal != VC1HWD_SEQ_HDRS_RDY) &&
                  (retVal != VC1HWD_PIC_HDRS_RDY) &&
                  (retVal != VC1HWD_FIELD_HDRS_RDY) &&
                  (retVal != VC1HWD_ENTRY_POINT_HDRS_RDY) &&
                  (retVal != VC1HWD_USER_DATA_RDY) );
    }
    /* SIMPLE AND MAIN PROFILE */
    else
    {
        /* if size of the stream is either 1 or 0 bytes,
         * then picture is skipped and it shall be reconstructed
         * as a P frame which is identical to its reference frame */
        if (streamData->strmBuffSize <= 1)
        {
            if(streamData->strmBuffSize == 1)
            {
                retVal = vc1hwdFlushBits(streamData, 8);
            }
            /* just adjust picture buffer indexes
             * to correct positions */
            if (pStorage->maxBframes > 0)
            {
                /* Increase references to reference picture */
                if( pStorage->work1 != pStorage->work0)
                {
                    pStorage->work1 = pStorage->work0;
                }
                /* Skipped frame shall be reconstructed as a 
                 * P-frame which is identical to its reference */
                pStorage->picLayer.picType = PTYPE_P;

                BqueueDiscard( &pStorage->bq, pStorage->workOut );
                pStorage->workOutPrev = pStorage->workOut;
                pStorage->workOut = pStorage->work0;
                EPRINT(("Skipped picture with MAXBFRAMES>0!"));
                return(VC1HWD_ERROR);
            }
            else
            {
                return VC1HWD_NOT_CODED_PIC;
            }
        }
        /* Allocate memories */
        if (pDecCont->decStat == VC1DEC_INITIALIZED)
        {
            tmp = AllocateMemories(pDecCont, pStorage, pDecCont->dwl);
            if (tmp != HANTRO_OK)
                return (VC1HWD_MEMORY_FAIL);
            else
                pDecCont->decStat = VC1DEC_RESOURCES_ALLOCATED;
        }
        /* Decode picture layer headers */
        retVal = vc1hwdDecodePictureLayer(pStorage, streamData);
        
        /* check stream exhaustion */
        if(vc1hwdIsExhausted(streamData))
        {
            EPRINT(("Stream exhausted!"));
            retVal = HANTRO_NOK;
        }
    
        if (pStorage->picLayer.picType == PTYPE_I ||
                pStorage->picLayer.picType == PTYPE_BI )
            pStorage->rnd = 0;
        else if (pStorage->picLayer.picType == PTYPE_P)
            pStorage->rnd = 1 - pStorage->rnd;

        if( pStorage->picLayer.picType == PTYPE_I )
            pStorage->anchorInter[ 0 ] = HANTRO_FALSE;
        else if ( pStorage->picLayer.picType == PTYPE_P )
            pStorage->anchorInter[ 0 ] = HANTRO_TRUE;

    }

    /* Allocate memories (advanced profile) */
    if (retVal == VC1HWD_SEQ_HDRS_RDY)
    {
        /* Allocate memories */
        if (pDecCont->decStat != VC1DEC_INITIALIZED)
        {
            (void)vc1hwdRelease(pDecCont->dwl, pStorage);
            if(pDecCont->bitPlaneCtrl.virtualAddress)
                DWLFreeLinear(pDecCont->dwl, &pDecCont->bitPlaneCtrl);
            if(pDecCont->directMvs.virtualAddress)
                DWLFreeLinear(pDecCont->dwl, &pDecCont->directMvs);
        }
        tmp = AllocateMemories(pDecCont, pStorage, pDecCont->dwl);
        if (tmp != HANTRO_OK)
            return (VC1HWD_MEMORY_FAIL);
        else
            pDecCont->decStat = VC1DEC_RESOURCES_ALLOCATED;
    }

    if( (retVal == VC1HWD_PIC_HDRS_RDY) || 
        (retVal == VC1HWD_FIELD_HDRS_RDY) )
    {
        if( pStorage->picLayer.picType == PTYPE_Skip )
        {
            /* Increase references to reference picture */
            if( pStorage->work1 != pStorage->work0)
            {
                pStorage->work1 = pStorage->work0;
            }
            pStorage->workOutPrev = pStorage->workOut;
            pStorage->workOut = pStorage->work0;
            return VC1HWD_NOT_CODED_PIC;
        }

        /* field interlace */
        if (pStorage->picLayer.fcm == FIELD_INTERLACE)
        {
            /* no reference picture for P field */
            if ( (pStorage->picLayer.fieldPicType == FP_I_P) &&
                 (pStorage->picLayer.isFF == HANTRO_FALSE) &&
                 (pStorage->firstFrame) &&
                 (pStorage->picLayer.numRef == 0) &&
                 (pStorage->picLayer.refField != 0) )
            {
                EPRINT(("No anchor picture for P field"));
                retVal = HANTRO_NOK;
            }
            
            /* invalid field parity */
            if ( (pStorage->picLayer.isFF == HANTRO_FALSE) &&
                 (pStorage->pPicBuf[pStorage->workOut].isTopFieldFirst ==
                  pStorage->picLayer.topField) )
            {
                EPRINT(("Same parity for successive fields in the frame"));
                retVal = HANTRO_NOK;
            }
        }
        /* if P frame, check that anchor available */
        if( (pStorage->picLayer.isFF == HANTRO_TRUE) &&
            (pStorage->picLayer.picType == PTYPE_P) &&
            (pStorage->work0 == INVALID_ANCHOR_PICTURE) )
        {
            EPRINT(("No anchor picture for P picture"));
            retVal = HANTRO_NOK;
        }
        else if( pDecCont->storage.picLayer.picType == PTYPE_B &&
                 pDecCont->storage.work1 == INVALID_ANCHOR_PICTURE )
        {
            EPRINT(("No anchor pictures for B picture!"));
            retVal = HANTRO_NOK;
        }
    }

    if (retVal == HANTRO_NOK)
    {
        pStorage->pictureBroken = HANTRO_TRUE;
        return(VC1HWD_ERROR);
    }
    else
        return(retVal);

}

/*------------------------------------------------------------------------------

    Function name: vc1hwdRelease

        Functional description:
            Release allocated resources 

        Inputs:
            dwl             DWL instance
            pStorage        Stream storage descriptor.

        Outputs:
            None

        Returns:
            HANTRO_OK

------------------------------------------------------------------------------*/
u16x vc1hwdRelease(const void *dwl, swStrmStorage_t *pStorage)
{

/* Variables */
    u16x i;
    picture_t *pPic = NULL;

/* Code */
    ASSERT(pStorage);

    BqueueRelease(&pStorage->bq);

    pPic = (picture_t*)pStorage->pPicBuf;

    /* free all allocated picture buffers */
    if (pPic != NULL)
    {
        for (i = 0; i < pStorage->workBufAmount; i++)
        {
            DWLFreeRefFrm(dwl, &pPic[i].data);
        }
        G1DWLfree((picture_t*)pStorage->pPicBuf);

        pStorage->pPicBuf = NULL;
    }
    if(pStorage->pMbFlags)
    {
        G1DWLfree(pStorage->pMbFlags);
        pStorage->pMbFlags = NULL;
    }

    return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function: vc1hwdUnpackMetaData

        Functional description:
            Unpacks metadata elements from buffer, when metadata is packed
            according to SMPTE VC-1 Standard Annexes J and L.

        Inputs:
            pBuffer         Buffer containing packed metadata

        Outputs:
            pMetaData       Unpacked metadata

        Returns:
            HANTRO_OK       Metadata unpacked OK
            HANTRO_NOK      Metadata is in wrong format or indicates
                            unsupported tools

------------------------------------------------------------------------------*/
u16x vc1hwdUnpackMetaData(const u8 * pBuffer, VC1DecMetaData *pMetaData)
{

/* Variables */

    u32 tmp1, tmp2;

/* Code */

    DPRINT(("\nUNPACKING META DATA"));
    DPRINT(("\nSTRUCT_SEQUENCE_HEADER_C\n"));

    /* Initialize sequence header C elements */
    pMetaData->loopFilter =
        pMetaData->multiRes =
        pMetaData->fastUvMc =
        pMetaData->extendedMv =
        pMetaData->dquant =
        pMetaData->vsTransform =
        pMetaData->overlap =
        pMetaData->syncMarker =
        pMetaData->rangeRed =
        pMetaData->maxBframes =
        pMetaData->quantizer =
        pMetaData->frameInterp = 0;

    tmp1 = SHOW1(pBuffer);
    tmp2  = BIT7(tmp1); tmp2 <<=1;
    tmp2 |= BIT6(tmp1); tmp2 <<=1;
    tmp2 |= BIT5(tmp1); tmp2 <<=1;
    tmp2 |= BIT4(tmp1);

    DPRINT(("PROFILE: \t %d\n",tmp2));

    if (tmp2 == 8) /* Advanced profile */
    {
        pMetaData->profile = 8;
    }
    else
    {
        pMetaData->profile = tmp2;

        tmp1 = SHOW1(pBuffer);

        tmp2 = BIT3(tmp1);
        DPRINT(("LOOPFILTER: \t %d\n",tmp2));
        pMetaData->loopFilter = tmp2;
        tmp2 = BIT2(tmp1);
        DPRINT(("Reserved3: \t %d\n",tmp2));
#ifdef HANTRO_PEDANTIC_MODE
        if( tmp2 != 0 ) return HANTRO_NOK;
#endif /* HANTRO_PEDANTIC_MODE */
        tmp2 = BIT1(tmp1);
        DPRINT(("MULTIRES: \t %d\n",tmp2));
        pMetaData->multiRes = tmp2;
        tmp2 = BIT0(tmp1);
        DPRINT(("Reserved4: \t %d\n",tmp2));
#ifdef HANTRO_PEDANTIC_MODE
        if( tmp2 != 1 ) return HANTRO_NOK;
#endif /* HANTRO_PEDANTIC_MODE */
        tmp1 = SHOW1(pBuffer);
        tmp2 = BIT7(tmp1);
        DPRINT(("FASTUVMC: \t %d\n",tmp2));
        pMetaData->fastUvMc = tmp2;
        tmp2 = BIT6(tmp1);
        DPRINT(("EXTENDED_MV: \t %d\n",tmp2));
        pMetaData->extendedMv = tmp2;
        tmp2 = BIT5(tmp1);
        tmp2 <<=1;
        tmp2 |= BIT4(tmp1);
        DPRINT(("DQUANT: \t %d\n",tmp2));
        pMetaData->dquant = tmp2;
        if(pMetaData->dquant > 2)
            return HANTRO_NOK;

        tmp2 = BIT3(tmp1);
        pMetaData->vsTransform = tmp2;
        DPRINT(("VTRANSFORM: \t %d\n",tmp2));
        tmp2 = BIT2(tmp1);
        DPRINT(("Reserved5: \t %d\n",tmp2));
        /* Reserved5 needs to be checked, it affects stream syntax. */
        if( tmp2 != 0 ) return HANTRO_NOK; 
        tmp2 = BIT1(tmp1);
        pMetaData->overlap = tmp2;
        DPRINT(("OVERLAP: \t %d\n",tmp2));
        tmp2 = BIT0(tmp1);
        pMetaData->syncMarker = tmp2;
        DPRINT(("SYNCMARKER: \t %d\n",tmp2));

        tmp1 = SHOW1(pBuffer);
        tmp2 = BIT7(tmp1);
        DPRINT(("RANGERED: \t %d\n",tmp2));
        pMetaData->rangeRed = tmp2;
        tmp2 =  BIT6(tmp1); tmp2 <<=1;
        tmp2 |= BIT5(tmp1); tmp2 <<=1;
        tmp2 |= BIT4(tmp1);
        DPRINT(("MAXBFRAMES: \t %d\n",tmp2));
        pMetaData->maxBframes = tmp2;
        tmp2 = BIT3(tmp1); tmp2 <<=1;
        tmp2 |= BIT2(tmp1);
        pMetaData->quantizer = tmp2;
        DPRINT(("QUANTIZER: \t %d\n",tmp2));
        tmp2 = BIT1(tmp1);
        pMetaData->frameInterp = tmp2;
        DPRINT(("FINTERPFLAG: \t %d\n",tmp2));
        tmp2 = BIT0(tmp1);
        DPRINT(("Reserved6: \t %d\n", tmp2));
        /* Reserved6 needs to be checked, it affects stream syntax. */
        if( tmp2 != 1 ) return HANTRO_NOK;
    }

    return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdErrorConcealment

        Functional description:
            Perform error concealment.

        Inputs:
            flush           HANTRO_TRUE    - Initialize picture to 128
                            HANTRO_FALSE   - Freeze to previous picture
            pStorage        Stream storage descriptor.

        Outputs:

        Returns:
            None

------------------------------------------------------------------------------*/
void vc1hwdErrorConcealment( const u16x flush, 
                             swStrmStorage_t * pStorage )
{

/* Variables */
    u32 tmp;
    u32 tmpOut;

/* Code */

    tmpOut = pStorage->workOut;
    if(flush)
    {
        (void)G1DWLmemset(
                pStorage->pPicBuf[ pStorage->workOut ].data.virtualAddress,
                                128,
                                pStorage->numOfMbs * 384 );
        /* if other buffer contains non-paired field -> throw away */
        if (pStorage->pPicBuf[1-pStorage->workOut].fcm == FIELD_INTERLACE &&
            pStorage->pPicBuf[1-pStorage->workOut].isFirstField == HANTRO_TRUE)
        {
            /* replace orphan 1.st field of previous frame in
             * frame buffer */
            if (pStorage->fieldCount)
                pStorage->fieldCount--;
            if (pStorage->outpCount)
                pStorage->outpCount--;
        }
    }
    else
    {
        /* we don't change indexes for broken B frames 
         * because those are not buffered */
        if ( (pStorage->picLayer.picType == PTYPE_I) ||
             (pStorage->picLayer.picType == PTYPE_P) ||
             pStorage->intraFreeze )
        {
            BqueueDiscard( &pStorage->bq, pStorage->workOut );
            pStorage->workOut = pStorage->work0;
            pStorage->workOutPrev = pStorage->workOut;
            if (!pStorage->pPicBuf[tmpOut].isFirstField)
                tmpOut = pStorage->workOut;
        }
    }

    /* unbuffer first field if second field is corrupted */
    if ((!pStorage->picLayer.isFF && !pStorage->missingField))
    {
        /* replace orphan 1.st field of previous frame in
         * frame buffer */
        if (pStorage->fieldCount)
            pStorage->fieldCount--;
        if (pStorage->outpCount)
            pStorage->outpCount--;
    }
    /* mark fcm as frame interlaced because concealment is frame based */
    if (pStorage->picLayer.fcm == FIELD_INTERLACE)
    {
        /* conceal whole frame */
        pStorage->picLayer.fcm = FRAME_INTERLACE;
    }
    /* skip B frames after corrupted anchor frame */
    if ( (pStorage->picLayer.picType == PTYPE_I) ||
         (pStorage->picLayer.picType == PTYPE_P) )
    {
        pStorage->skipB = 2;
        tmp = pStorage->workOut;
    }
    else
        tmp = pStorage->prevBIdx; /* concealing B frame */
    
    /* both fields are concealed */
    ((picture_t*)pStorage->pPicBuf)[tmp].isFirstField = HANTRO_FALSE;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdSeekFrameStart

        Functional description:
            
        Inputs:
            pStorage    Stream storage descriptor.
            pStrmData   Input stream data.

        Outputs:
            None

        Returns:
            OK/NOK/END_OF_STREAM

------------------------------------------------------------------------------*/
u32 vc1hwdSeekFrameStart(swStrmStorage_t * pStorage,
                             strmData_t *pStrmData )
{
    u32 rv;
    u32 sc;
    u32 tmp;
    u8 *pStrm;
        
    if (pStorage->profile == VC1_ADVANCED)
    {
        /* Seek next frame start */
        tmp = pStrmData->bitPosInWord;
        if (tmp)
            rv = vc1hwdFlushBits(pStrmData, 8-tmp);
        
        do
        {
            pStrm = pStrmData->pStrmCurrPos;
            
            /* Check that we have enough stream data */   
            if (((pStrmData->strmBuffReadBits>>3)+4) <= pStrmData->strmBuffSize)
            {
                sc = ( ((u32)pStrm[0]) << 24 ) +
                     ( ((u32)pStrm[1]) << 16 ) +
                     ( ((u32)pStrm[2]) << 8  ) +
                     ( ((u32)pStrm[3]) );
            }
            else
                sc = 0;
   
            if (sc == SC_FRAME || 
                sc == SC_SEQ || 
                sc == SC_ENTRY_POINT ||
                sc == SC_END_OF_SEQ ||
                sc == END_OF_STREAM )
            {
                break;
            }
        } while (vc1hwdFlushBits(pStrmData, 8) == HANTRO_OK);
    }
    else
    {
        /* mark all stream processed for simple/main profiles */
        pStrmData->pStrmCurrPos = pStrmData->pStrmBuffStart +
            pStrmData->strmBuffSize;
    }

    if (vc1hwdIsExhausted(pStrmData))
        rv = END_OF_STREAM;
    else
        rv = HANTRO_OK;

    return rv;
}

/*------------------------------------------------------------------------------

    Function name: BufferPicture

        Functional description:
            Perform error concealment.

        Inputs:
            pStorage        Stream storage descriptor.
            picToBuffer     Picture buffer index of pic to buffer.

        Outputs:

        Returns:
            HANTRO_OK       Picture buffered ok
            HANTRO_NOK      Buffer full

------------------------------------------------------------------------------*/
u16x vc1hwdBufferPicture( swStrmStorage_t * pStorage, u16x picToBuffer,
                          u16x bufferB, u16x picId, u16x errMbs )
{

/* Variables */

    i32 i, j;
    u32 ffIndex;
    static u32 counter = 0;

/* Code */
    ASSERT(pStorage);
        
    /* Index 0 for first field, 1 for second field */
    ffIndex = (pStorage->picLayer.isFF) ? 0 : 1;

    if (pStorage->picLayer.fcm == FIELD_INTERLACE)
    {
        pStorage->fieldCount++;
        /* for error concealment purposes */
        if (pStorage->fieldCount >= 2)
            pStorage->firstFrame = HANTRO_FALSE;

        /* Just return if second field of the frame */
        if (ffIndex)
        {
            /* store picId and number of err MBs for second field also */
            pStorage->outPicId[ffIndex][pStorage->prevOutpIdx] = picId;
            pStorage->outErrMbs[pStorage->prevOutpIdx] = errMbs;
            
            return HANTRO_OK;
        }
        else
        {
            if ( (pStorage->fieldCount >=3) && (pStorage->outpCount >= 2) )
            {
                if( pStorage->parallelMode2)
                {
                    if ( (pStorage->fieldCount >=5) && (pStorage->outpCount >= 3) )
                    {
                        /* force output count to be 1 if first field 
                         * and outpCount == 2. This prevents buffer overflow 
                         * in some rare error cases */
                        EPRINT("Picture buffer output count exceeded. Overwriting picture!!!");
                        pStorage->outpCount = 2;
                    }

                }
                else
                {
                    /* force output count to be 1 if first field 
                     * and outpCount == 2. This prevents buffer overflow 
                     * in some rare error cases */
                    EPRINT("Picture buffer output count exceeded. Overwriting picture!!!");
                    pStorage->outpCount = 1;
                }
            }
        }
    }
    else
    {
        pStorage->fieldCount += 2;
        /* for error concealment purposes */
        if (pStorage->fieldCount >= 2)
            pStorage->firstFrame = HANTRO_FALSE;
    }

    if ( pStorage->outpCount >= MAX_OUTPUT_PICS )
    {
        return HANTRO_NOK;
    }

    if( bufferB == 0 ) /* Buffer I or P picture */
    {
        i = pStorage->outpIdx + pStorage->outpCount;
        if( i >= MAX_OUTPUT_PICS ) i -= MAX_OUTPUT_PICS;
    }
    else /* Buffer B picture */
    {
        j = pStorage->outpIdx + pStorage->outpCount;
        i = j - 1;
        if( j >= MAX_OUTPUT_PICS ) j -= MAX_OUTPUT_PICS;
        if( i < 0 ) i += MAX_OUTPUT_PICS;
        /* Added check due to max 2 pic latency */
        else if( i >= MAX_OUTPUT_PICS ) i -= MAX_OUTPUT_PICS; 

        pStorage->outpBuf[j] = pStorage->outpBuf[i];
        pStorage->outPicId[0][j] = pStorage->outPicId[0][i];
        pStorage->outPicId[1][j] = pStorage->outPicId[1][i];
        pStorage->outErrMbs[j] = pStorage->outErrMbs[i];
    }
    pStorage->prevOutpIdx = i;
    pStorage->outpBuf[i] = picToBuffer;
    pStorage->outErrMbs[i] = errMbs;

    if (pStorage->picLayer.fcm == FIELD_INTERLACE)
    {
        pStorage->outPicId[ffIndex][i] = picId;
    }
    else
    {   /* set same picId for both fields of frame */
        pStorage->outPicId[0][i] = picId;
        pStorage->outPicId[1][i] = picId;
    }

    pStorage->outpCount++;
    
    return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdNextPicture

        Functional description:
            Perform error concealment.

        Inputs:
            pStorage        Stream storage descriptor.
            endOfStream     Indicates whether end of stream has been reached.

        Outputs:
            pNextPicture    Next picture buffer index

        Returns:
            HANTRO_OK       Picture ok
            HANTRO_NOK      No more pictures

------------------------------------------------------------------------------*/
u16x vc1hwdNextPicture( swStrmStorage_t * pStorage, u16x * pNextPicture,
                        u32 *pFieldToRet, u16x endOfStream, u32 deinterlace,
                        u32* pPicId, u32* pErrMbs)
{

/* Variables */

    u32 i;
    u32 minCount;

/* Code */

    ASSERT(pStorage);
    ASSERT(pNextPicture);

    minCount = 0;
    /* Determine how many pictures we are willing to give out... */
    if(pStorage->minCount > 0 && !endOfStream)
        minCount = 1;

    if(pStorage->parallelMode2 && !endOfStream)
        minCount = 2;

    /* when deinterlacing is ON, we don't give first field out 
     * before second field is also decoded */
    if ((pStorage->fieldCount % 2) && deinterlace)
    {
        return HANTRO_NOK;
    }

    if (pStorage->outpCount <= minCount) 
    {
        return HANTRO_NOK;
    }

    if (pStorage->interlace && !deinterlace)
    {
        if ((pStorage->fieldCount < 3) && !endOfStream)
            return HANTRO_NOK;

        if ((pStorage->fieldCount < 5) && !endOfStream &&
            pStorage->parallelMode2 )
            return HANTRO_NOK;

        i = pStorage->outpIdx;
        *pFieldToRet = pStorage->fieldToReturn;
        *pNextPicture = pStorage->outpBuf[i];
        *pPicId = pStorage->outPicId[pStorage->fieldToReturn][i];
        *pErrMbs = pStorage->outErrMbs[i];
        
        if (pStorage->fieldToReturn == 1) /* return second field */
        {
            pStorage->outpCount--;
            i++;
            if( i == MAX_OUTPUT_PICS ) i = 0;
            pStorage->outpIdx = i;
        }

        pStorage->fieldToReturn = 1-pStorage->fieldToReturn;
        pStorage->fieldCount--;
    }
    else
    {
        i = pStorage->outpIdx;
        pStorage->outpCount--;
        *pPicId = pStorage->outPicId[0][i];
        *pErrMbs = pStorage->outErrMbs[i];
        *pNextPicture = pStorage->outpBuf[i++];
        if( i == MAX_OUTPUT_PICS ) i = 0;
        pStorage->outpIdx = i;
        
        pStorage->fieldCount -= 2;
    }

    return HANTRO_OK;

}

/*------------------------------------------------------------------------------

    Function name: vc1hwdSetPictureInfo

        Functional description:
            Fill picture layer information to picture_t structure.

        Inputs:
            pDecCont        Container for the decoder
            picId           Current picture id

        Outputs:

        Returns:
            None

------------------------------------------------------------------------------*/
void vc1hwdSetPictureInfo( decContainer_t *pDecCont, u32 picId )
{

/* Variables */
    u32 workIndex;
    picture_t *pPic;
    u32 ffIndex;

/* Code */
    ASSERT(pDecCont);

    pPic = (picture_t*)pDecCont->storage.pPicBuf;
    workIndex = pDecCont->storage.workOut;

    pPic[ workIndex ].codedWidth  = pDecCont->storage.curCodedWidth;
    pPic[ workIndex ].codedHeight = pDecCont->storage.curCodedHeight;
    pPic[ workIndex ].rangeRedFrm = pDecCont->storage.picLayer.rangeRedFrm;
    pPic[ workIndex ].fcm = pDecCont->storage.picLayer.fcm;

    if (pDecCont->storage.picLayer.fcm == PROGRESSIVE || 
        pDecCont->storage.picLayer.fcm == FRAME_INTERLACE )
    {
        pPic[ workIndex ].keyFrame =
                ( pDecCont->storage.picLayer.picType == PTYPE_I ) ?
                HANTRO_TRUE : HANTRO_FALSE;
    }
    else
    {
        pPic[ workIndex ].keyFrame =
                ( pDecCont->storage.picLayer.fieldPicType == FP_I_I ) ?
                HANTRO_TRUE : HANTRO_FALSE;
    }
    pPic[ workIndex ].tiledMode      = pDecCont->tiledReferenceEnable;
    pPic[ workIndex ].rangeMapYFlag  = pDecCont->storage.rangeMapYFlag;
    pPic[ workIndex ].rangeMapY      = pDecCont->storage.rangeMapY;
    pPic[ workIndex ].rangeMapUvFlag = pDecCont->storage.rangeMapUvFlag;
    pPic[ workIndex ].rangeMapUv     = pDecCont->storage.rangeMapUv;

    pPic[ workIndex ].isFirstField = pDecCont->storage.picLayer.isFF;
    pPic[ workIndex ].isTopFieldFirst = pDecCont->storage.picLayer.tff;
    pPic[ workIndex ].rff = pDecCont->storage.picLayer.rff;
    pPic[ workIndex ].rptfrm = pDecCont->storage.picLayer.rptfrm;


    /* fill correct field structure */
    ffIndex = (pDecCont->storage.picLayer.isFF) ? 0 : 1;
    pPic[ workIndex ].field[ffIndex].intCompF = 
                        pDecCont->storage.picLayer.intCompField;
    pPic[ workIndex ].field[ffIndex].iScaleA =
                        pDecCont->storage.picLayer.iScale;
    pPic[ workIndex ].field[ffIndex].iShiftA =
                        pDecCont->storage.picLayer.iShift;
    pPic[ workIndex ].field[ffIndex].iScaleB =
                        pDecCont->storage.picLayer.iScale2;
    pPic[ workIndex ].field[ffIndex].iShiftB =
                        pDecCont->storage.picLayer.iShift2;
    pPic[ workIndex ].field[ffIndex].type = 
                        pDecCont->storage.picLayer.picType;
    pPic[ workIndex ].field[ffIndex].picId = picId;
    /* reset post-processing status */
    pPic[ workIndex ].field[ffIndex].decPpStat = NONE;

}

/*------------------------------------------------------------------------------

    Function name: vc1hwdUpdateWorkBufferIndexes

        Functional description:
            Check anchor availability and update picture buffer work indexes.

        Inputs:
            pStorage        Stream storage descriptor.
            isBPic          Is current picture B or BI picture

        Outputs:

        Returns:
            None

------------------------------------------------------------------------------*/
void vc1hwdUpdateWorkBufferIndexes( swStrmStorage_t * pStorage, u32 isBPic )
{
   if (pStorage->picLayer.isFF == HANTRO_TRUE)
   {
       u32 work0, work1;
       
       /* Determine work buffers (updating is done only after the frame
        * so here we assume situation after decoding frame */
       if( isBPic )
       {
           work0 = pStorage->work0;
           work1 = pStorage->work1;
       }
       else
       {
           work0 = pStorage->workOut;
           work1 = pStorage->work0;
       }

        /* Get free buffer */
        pStorage->workOutPrev = pStorage->workOut;
        pStorage->workOut = BqueueNext( 
            &pStorage->bq,
            work0,
            work1,
            BQUEUE_UNUSED, 
            isBPic );

       Vc1DecPpNextInput( &pStorage->decPp,
           pStorage->picLayer.fcm != FIELD_INTERLACE );

       if( isBPic )
       {
           pStorage->prevBIdx = pStorage->workOut;

       }
   }
}

