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
--  Abstract :  Header decoding
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_headers.c,v $
--  $Date: 2009/09/09 12:28:01 $
--  $Revision: 1.15 $
-
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecodeHdrs
        5.2     StrmDec_DefineVopComplexityEstimationHdr
        5.3     StrmDec_SaveUserData
        5.4     StrmDec_ClearHeaders

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "mp4dechwd_headers.h"
#include "mp4dechwd_utils.h"
#include "mp4dechwd_generic.h"
#include "mp4decapi_internal.h"
#include "mp4debug.h"
#include "mp4deccfg.h"
#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

enum
{
    VIDEO_ID = 1
};
enum
{
    SIMPLE_OBJECT_TYPE = 1
};

/* aspect ratio info */
enum
{
    EXTENDED_PAR = 15
};
enum
{
    RECTANGULAR = 0
};

enum
{
    VERSION1 = 0,
    VERSION2 = 1
};

u32 StrmDec_DefineVopComplexityEstimationHdr(DecContainer * pDecContainer);

static const u32 zigZag[64] = {
    0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63 };

static u32 QuantMat(DecContainer * pDecContainer, u32 intra);

static void MP4DecSetLowDelay(DecContainer * pDecContainer);
extern void StrmDec_CheckPackedMode(DecContainer * pDecContainer);

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name:
                StrmDec_DecodeHdrs

        Purpose:
                Decodes MPEG-4 higher level (Up to VOL + GVOP) headers from
                input stream

        Input:
                pointer to DecContainer, name of first header to decode

        Output:
                status (HANTRO_OK/NOK/END_OF_STREAM/... enum in .h file!)

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeHdrs(DecContainer * pDecContainer, u32 mode)
{

    u32 tmp;
    u32 markerBit;
    u32 tmp2;

    if (pDecContainer->Hdrs.lock)
    {
        while ((mode == SC_VOS_START) || (mode == SC_VISO_START) ||
               (mode == SC_VO_START)  || (mode == SC_VOL_START) )
        {
            mode = StrmDec_FindSync(pDecContainer);
        }
        if (mode == SC_SV_START)
        {
            (void)StrmDec_UnFlushBits(pDecContainer,22);
        }
        else if (mode == SC_RESYNC)
        {
            (void)StrmDec_UnFlushBits(pDecContainer,
                pDecContainer->StrmStorage.resyncMarkerLength);
        }
        else if (mode != END_OF_STREAM)
        {
            (void)StrmDec_UnFlushBits(pDecContainer,32);
        }
        return(HANTRO_OK);
    }

    /* in following switch case syntax, the u32_mode is used only to decide
     * from witch header the decoding starts. After that the decoding will
     * run through all headers and will break after the VOL header is decoded.
     * GVOP header will be decoded separately and after that the execution
     * will jump back to the top level */

    switch (mode)
    {
        case SC_VOS_START:
            /* visual object sequence */
            tmp = StrmDec_GetBits(pDecContainer, 8);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            pDecContainer->Hdrs.profileAndLevelIndication = tmp;
            pDecContainer->Hdrs.lastHeaderType = SC_VOS_START;
            tmp = StrmDec_SaveUserData(pDecContainer, SC_VOS_START);
            if(tmp == HANTRO_NOK)
                return (HANTRO_NOK);
            MP4DEC_DEBUG(("Decoded VOS Start\n"));
            break;

        case SC_VISO_START:
            /* visual object */
            tmp = pDecContainer->Hdrs.isVisualObjectIdentifier =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp)
            {
                tmp = pDecContainer->Hdrs.visualObjectVerid =
                    StrmDec_GetBits(pDecContainer, 4);
                pDecContainer->Hdrs.visualObjectPriority =
                    StrmDec_GetBits(pDecContainer, 3);
            }
            else
                pDecContainer->Hdrs.visualObjectVerid = 0x1;
            tmp = pDecContainer->Hdrs.visualObjectType =
                StrmDec_GetBits(pDecContainer, 4);
            if((tmp != VIDEO_ID) && (tmp != END_OF_STREAM))
                return (HANTRO_NOK);
            tmp = pDecContainer->Hdrs.videoSignalType =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp)
            {
                pDecContainer->Hdrs.videoFormat =
                    StrmDec_GetBits(pDecContainer, 3);
                pDecContainer->Hdrs.videoRange =
                    StrmDec_GetBits(pDecContainer, 1);
                tmp = pDecContainer->Hdrs.colourDescription =
                    StrmDec_GetBits(pDecContainer, 1);
                if(tmp)
                {
                    tmp = pDecContainer->Hdrs.colourPrimaries =
                        StrmDec_GetBits(pDecContainer, 8);
#ifdef HANTRO_PEDANTIC_MODE
                    if(tmp == 0)
                        return (HANTRO_NOK);
#endif
                    tmp = pDecContainer->Hdrs.transferCharacteristics =
                        StrmDec_GetBits(pDecContainer, 8);
#ifdef HANTRO_PEDANTIC_MODE
                    if(tmp == 0)
                        return (HANTRO_NOK);
#endif
                    tmp = pDecContainer->Hdrs.matrixCoefficients =
                        StrmDec_GetBits(pDecContainer, 8);
#ifdef HANTRO_PEDANTIC_MODE
                    if(tmp == 0)
                        return (HANTRO_NOK);
#endif
                }
            }
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            /* stuffing */
            tmp = StrmDec_GetStuffing(pDecContainer);
            if(tmp != HANTRO_OK)
                return (tmp);
            pDecContainer->Hdrs.lastHeaderType = SC_VISO_START;
            /* user data */
            tmp = StrmDec_SaveUserData(pDecContainer, SC_VISO_START);
            if(tmp == HANTRO_NOK)
                return (HANTRO_NOK);
            MP4DEC_DEBUG(("Decoded VISO Start\n"));
            break;

        case SC_VO_START:
            /* video object */
            MP4DEC_DEBUG(("Decoded VO Start\n"));
            break;

        case SC_VOL_START:
            MP4DEC_DEBUG(("Decode VOL Start\n"));
            /* video object layer */
            pDecContainer->Hdrs.randomAccessibleVol =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.videoObjectTypeIndication =
                StrmDec_GetBits(pDecContainer, 8);
            /* NOTE: video_object_type cannot be checked if we want to be
             * able to decode conformance test streams (they contain
             * streams where type is e.g. main). However streams do not
             * utilize all tools of "higher" profiles and can be decoded
             * by our codec, even though we only support objects of type
             * simple and advanced simple */
            tmp = pDecContainer->Hdrs.isObjectLayerIdentifier =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp)
            {
                pDecContainer->Hdrs.videoObjectLayerVerid =
                    StrmDec_GetBits(pDecContainer, 4);
                pDecContainer->Hdrs.videoObjectLayerPriority =
                    StrmDec_GetBits(pDecContainer, 3);
            }
            else
            {
                pDecContainer->Hdrs.videoObjectLayerVerid =
                    pDecContainer->Hdrs.visualObjectVerid;
            }
            tmp = pDecContainer->Hdrs.aspectRatioInfo =
                StrmDec_GetBits(pDecContainer, 4);
            if(tmp == EXTENDED_PAR)
            {
                tmp = pDecContainer->Hdrs.parWidth =
                    StrmDec_GetBits(pDecContainer, 8);
#ifdef HANTRO_PEDANTIC_MODE
                if(tmp == 0)
                    return (HANTRO_NOK);
#endif
                tmp = pDecContainer->Hdrs.parHeight =
                    StrmDec_GetBits(pDecContainer, 8);
#ifdef HANTRO_PEDANTIC_MODE
                if(tmp == 0)
                    return (HANTRO_NOK);
#endif
            }
#ifdef HANTRO_PEDANTIC_MODE
            else if(tmp == 0)
                return (HANTRO_NOK);
#endif
            tmp = pDecContainer->Hdrs.volControlParameters =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp)
            {
                pDecContainer->Hdrs.chromaFormat =
                    StrmDec_GetBits(pDecContainer, 2);
                tmp = pDecContainer->Hdrs.lowDelay =
                    StrmDec_GetBits(pDecContainer, 1);
                tmp = pDecContainer->Hdrs.vbvParameters =
                    StrmDec_GetBits(pDecContainer, 1);
                if(tmp)
                {
                    tmp = pDecContainer->Hdrs.firstHalfBitRate =
                        StrmDec_GetBits(pDecContainer, 15);
                    markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
                    tmp2 = pDecContainer->Hdrs.latterHalfBitRate =
                        StrmDec_GetBits(pDecContainer, 15);
#ifdef HANTRO_PEDANTIC_MODE
                    if((tmp == 0) && (tmp2 == 0))
                        return (HANTRO_NOK);
#endif
                    markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
                    tmp = pDecContainer->Hdrs.firstHalfVbvBufferSize =
                        StrmDec_GetBits(pDecContainer, 15);
                    markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
                    tmp2 = pDecContainer->Hdrs.latterHalfVbvBufferSize =
                        StrmDec_GetBits(pDecContainer, 3);
#ifdef HANTRO_PEDANTIC_MODE
                    if((tmp == 0) && (tmp2 == 0))
                        return (HANTRO_NOK);
#endif
                    pDecContainer->Hdrs.firstHalfVbvOccupancy =
                        StrmDec_GetBits(pDecContainer, 11);
                    markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
                    pDecContainer->Hdrs.latterHalfVbvOccupancy =
                        StrmDec_GetBits(pDecContainer, 15);
                    markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
                }
            }
            else
            {
                /* Set low delay based on profile */
                MP4DecSetLowDelay(pDecContainer);
            }

            tmp = pDecContainer->Hdrs.videoObjectLayerShape =
                StrmDec_GetBits(pDecContainer, 2);
            if((tmp != RECTANGULAR) && (tmp != END_OF_STREAM))
                return (HANTRO_NOK);
            markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
            tmp = pDecContainer->Hdrs.vopTimeIncrementResolution =
                StrmDec_GetBits(pDecContainer, 16);
            if(tmp == 0)
                return (HANTRO_NOK);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
            tmp = pDecContainer->Hdrs.fixedVopRate =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp)
            {
                tmp =
                    StrmDec_NumBits(pDecContainer->Hdrs.
                                    vopTimeIncrementResolution - 1);
                tmp = pDecContainer->Hdrs.fixedVopTimeIncrement =
                    StrmDec_GetBits(pDecContainer, tmp);
                if (tmp == 0 ||
                    tmp >= pDecContainer->Hdrs.vopTimeIncrementResolution)
                    return (HANTRO_NOK);
            }
            /* marker bit after if video_object_layer==rectangular */
            markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
            tmp = pDecContainer->Hdrs.videoObjectLayerWidth =
                StrmDec_GetBits(pDecContainer, 13);
            if(tmp == 0)
                return (HANTRO_NOK);
            markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
            tmp = pDecContainer->Hdrs.videoObjectLayerHeight =
                StrmDec_GetBits(pDecContainer, 13);
            if(tmp == 0)
                return (HANTRO_NOK);
            markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
                    if(!markerBit)
                        return (HANTRO_NOK);
#endif
            tmp = pDecContainer->Hdrs.interlaced =
                StrmDec_GetBits(pDecContainer, 1);

            tmp = pDecContainer->Hdrs.obmcDisable =
                StrmDec_GetBits(pDecContainer, 1);
            if(!tmp)
                return (HANTRO_NOK);
            if(pDecContainer->Hdrs.videoObjectLayerVerid == 1)
            {
                tmp = pDecContainer->Hdrs.spriteEnable =
                    StrmDec_GetBits(pDecContainer, 1);
            }
            else
            {
                tmp = pDecContainer->Hdrs.spriteEnable =
                    StrmDec_GetBits(pDecContainer, 2);
            }
            /* 1. increment */
            if(tmp && (tmp != END_OF_STREAM))
            {
                return (HANTRO_NOK);
            }
            tmp = pDecContainer->Hdrs.not8Bit =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp && (tmp != END_OF_STREAM))
                return (HANTRO_NOK);

            tmp = pDecContainer->Hdrs.quantType =
                StrmDec_GetBits(pDecContainer, 1);
#ifdef ASIC_TRACE_SUPPORT
            if (tmp)
                trace_mpeg4DecodingTools.qMethod1 = 1;
            else
                trace_mpeg4DecodingTools.qMethod2 = 1;
#endif
            (void)G1DWLmemset(pDecContainer->StrmStorage.quantMat,
                      0, MP4DEC_QUANT_TABLE_SIZE);
            if (pDecContainer->Hdrs.quantType)
            {
                tmp = StrmDec_GetBits(pDecContainer, 1);
                if (tmp && tmp != END_OF_STREAM)
                    tmp = QuantMat(pDecContainer, 1);
                tmp = StrmDec_GetBits(pDecContainer, 1);
                if (tmp && tmp != END_OF_STREAM)
                    tmp = QuantMat(pDecContainer, 0);
            }

            if(pDecContainer->Hdrs.videoObjectLayerVerid != 1)
            {
                /* quarter_sample */
                tmp = StrmDec_GetBits(pDecContainer, 1);
                pDecContainer->Hdrs.quarterpel = tmp;

            }
            tmp = pDecContainer->Hdrs.complexityEstimationDisable =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp == 0)
            {
                /* complexity estimation header */
                tmp = StrmDec_DefineVopComplexityEstimationHdr(pDecContainer);
#ifdef HANTRO_PEDANTIC_MODE
                if(tmp == HANTRO_NOK)
                    return (HANTRO_NOK);
#endif
            }
            pDecContainer->Hdrs.resyncMarkerDisable = 0;
#ifdef ASIC_TRACE_SUPPORT
            tmp = StrmDec_GetBits(pDecContainer, 1);
            if (!tmp)
                trace_mpeg4DecodingTools.resyncMarker = 1;
#else
            (void) StrmDec_GetBits(pDecContainer, 1);
#endif
            tmp = pDecContainer->Hdrs.dataPartitioned =
                    StrmDec_GetBits(pDecContainer, 1);
#ifdef ASIC_TRACE_SUPPORT
            if (tmp)
                trace_mpeg4DecodingTools.dataPartition = 1;
#endif
            if(tmp)
            {
                pDecContainer->Hdrs.reversibleVlc =
                    StrmDec_GetBits(pDecContainer, 1);
#ifdef ASIC_TRACE_SUPPORT
                if (pDecContainer->Hdrs.reversibleVlc)
                    trace_mpeg4DecodingTools.reversibleVlc = 1;
#endif
            }
            else
                pDecContainer->Hdrs.reversibleVlc = 0;
            if(pDecContainer->Hdrs.videoObjectLayerVerid != 1)
            {
                /* newpred_enable */
                tmp = StrmDec_GetBits(pDecContainer, 1);
                if(tmp == 1)
                    return (HANTRO_NOK);
                /* reduced_resolution_vop_enable */
                tmp = StrmDec_GetBits(pDecContainer, 1);
                if(tmp == 1)
                    return (HANTRO_NOK);
            }
            tmp = pDecContainer->Hdrs.scalability =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp == 1)
                return (HANTRO_NOK);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            tmp = StrmDec_GetStuffing(pDecContainer);
            if(tmp != HANTRO_OK)
                return (tmp);
            pDecContainer->Hdrs.lastHeaderType = SC_VOL_START;
            /* user data */
            tmp = StrmDec_SaveUserData(pDecContainer, SC_VOL_START);
            if(tmp == HANTRO_NOK)
                return (HANTRO_NOK);
            MP4DEC_DEBUG(("Decoded VOL Start\n"));
            break;

        default:
            return (HANTRO_NOK);

    }
    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.1  Function name:
                StrmDec_DecodeGovHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeGovHeader(DecContainer *pDecContainer)
{
    u32 tmp;

    ASSERT(pDecContainer);

    MP4DEC_DEBUG(("Decode GVOP Start\n"));
    /* NOTE: in rare cases computation of GovTimeIncrement may cause
     * overflow. This requires that time code change is ~18 hours and
     * VopTimeIncrementResolution is ~2^16. */
    pDecContainer->StrmStorage.govTimeIncrement =
        pDecContainer->VopDesc.timeCodeHours * 3600 +
        pDecContainer->VopDesc.timeCodeMinutes * 60 +
        pDecContainer->VopDesc.timeCodeSeconds;

    tmp = pDecContainer->VopDesc.timeCodeHours =
        StrmDec_GetBits(pDecContainer, 5);
    if((tmp > 23) && (tmp != END_OF_STREAM))
        return (HANTRO_NOK);
    tmp = pDecContainer->VopDesc.timeCodeMinutes =
        StrmDec_GetBits(pDecContainer, 6);
    if((tmp > 59) && (tmp != END_OF_STREAM))
        return (HANTRO_NOK);
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == 0)
        return (HANTRO_NOK);
    tmp = pDecContainer->VopDesc.timeCodeSeconds =
        StrmDec_GetBits(pDecContainer, 6);
    if((tmp > 59) && (tmp != END_OF_STREAM))
        return (HANTRO_NOK);
    tmp =
        pDecContainer->VopDesc.timeCodeHours * 3600 +
        pDecContainer->VopDesc.timeCodeMinutes * 60 +
        pDecContainer->VopDesc.timeCodeSeconds;
    if(tmp == pDecContainer->StrmStorage.govTimeIncrement)
    {
        pDecContainer->StrmStorage.govTimeIncrement = 0;
    }
    else if(tmp > pDecContainer->StrmStorage.govTimeIncrement)
    {
        pDecContainer->StrmStorage.govTimeIncrement = tmp -
            pDecContainer->StrmStorage.govTimeIncrement;
        pDecContainer->StrmStorage.govTimeIncrement *=
            pDecContainer->Hdrs.vopTimeIncrementResolution;
    }
    else
    {
        //pDecContainer->StrmStorage.govTimeIncrement =
            pDecContainer->StrmStorage.govTimeIncrement =
            (24 * 3600 + tmp) -
            pDecContainer->StrmStorage.govTimeIncrement;
        pDecContainer->StrmStorage.govTimeIncrement *=
            pDecContainer->Hdrs.vopTimeIncrementResolution;
    }

    pDecContainer->Hdrs.closedGov = StrmDec_GetBits(pDecContainer, 1);
    pDecContainer->Hdrs.brokenLink = StrmDec_GetBits(pDecContainer, 1);
    tmp = StrmDec_GetStuffing(pDecContainer);
    if(tmp != HANTRO_OK)
        return (tmp);
    pDecContainer->Hdrs.lastHeaderType = SC_GVOP_START;
    pDecContainer->VopDesc.govCounter++;
    tmp = StrmDec_SaveUserData(pDecContainer, SC_GVOP_START);
    return (tmp);

}

/*------------------------------------------------------------------------------

   5.2  Function name:
                StrmDec_DefineVopComplexityEstimationHdr

        Purpose:
                Decodes fields from VOL define_vop_complexity_estimation_
                header

        Input:
                pointer to DecContainer

        Output:
                status (HANTRO_OK/NOK)

------------------------------------------------------------------------------*/
u32 StrmDec_DefineVopComplexityEstimationHdr(DecContainer * pDecContainer)
{
    u32 tmp;
    u32 markerBit;
    u32 estimationMethod;

    estimationMethod = pDecContainer->Hdrs.estimationMethod =
        StrmDec_GetBits(pDecContainer, 2);
    if((estimationMethod == VERSION1) ||
       (estimationMethod == VERSION2))
    {
        tmp = pDecContainer->Hdrs.shapeComplexityEstimationDisable =
            StrmDec_GetBits(pDecContainer, 1);
        if(tmp == 0)
        {
            pDecContainer->Hdrs.opaque =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.transparent =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.intraCae =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.interCae =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.noUpdate =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.upsampling =
                StrmDec_GetBits(pDecContainer, 1);
        }
        else
        {
            /* initialisation to zero */
            pDecContainer->Hdrs.opaque = 0;
            pDecContainer->Hdrs.transparent = 0;
            pDecContainer->Hdrs.intraCae = 0;
            pDecContainer->Hdrs.interCae = 0;
            pDecContainer->Hdrs.noUpdate = 0;
            pDecContainer->Hdrs.upsampling = 0;
        }
        tmp =
            pDecContainer->Hdrs.textureComplexityEstimationSet1Disable =
            StrmDec_GetBits(pDecContainer, 1);
        if(tmp == 0)
        {
            pDecContainer->Hdrs.intraBlocks =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.interBlocks =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.inter4vBlocks =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.notCodedBlocks =
                StrmDec_GetBits(pDecContainer, 1);
        }
        else
        {
            /* Initialization to zero */
            pDecContainer->Hdrs.intraBlocks = 0;
            pDecContainer->Hdrs.interBlocks = 0;
            pDecContainer->Hdrs.inter4vBlocks = 0;
            pDecContainer->Hdrs.notCodedBlocks = 0;
        }
        markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
        if(!markerBit)
            return (HANTRO_NOK);
#endif
        tmp =
            pDecContainer->Hdrs.textureComplexityEstimationSet2Disable =
            StrmDec_GetBits(pDecContainer, 1);
        if(tmp == 0)
        {
            pDecContainer->Hdrs.dctCoefs =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.dctLines =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.vlcSymbols =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.vlcBits =
                StrmDec_GetBits(pDecContainer, 1);
        }
        else
        {
            /* initialisation to zero */
            pDecContainer->Hdrs.dctCoefs = 0;
            pDecContainer->Hdrs.dctLines = 0;
            pDecContainer->Hdrs.vlcSymbols = 0;
            pDecContainer->Hdrs.vlcBits = 0;
        }
        tmp = pDecContainer->Hdrs.motionCompensationComplexityDisable =
            StrmDec_GetBits(pDecContainer, 1);
        if(tmp == 0)
        {
            pDecContainer->Hdrs.apm = StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.npm = StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.interpolateMcQ =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.forwBackMcQ =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.halfpel2 =
                StrmDec_GetBits(pDecContainer, 1);
            pDecContainer->Hdrs.halfpel4 =
                StrmDec_GetBits(pDecContainer, 1);
        }
        else
        {
            /* initilisation to zero */
            pDecContainer->Hdrs.apm = 0;
            pDecContainer->Hdrs.npm = 0;
            pDecContainer->Hdrs.interpolateMcQ = 0;
            pDecContainer->Hdrs.forwBackMcQ = 0;
            pDecContainer->Hdrs.halfpel2 = 0;
            pDecContainer->Hdrs.halfpel4 = 0;
        }
        markerBit = StrmDec_GetBits(pDecContainer, 1);
#ifdef HANTRO_PEDANTIC_MODE
        if(!markerBit)
            return (HANTRO_NOK);
#endif
        if(estimationMethod == VERSION2)
        {
            tmp =
                pDecContainer->Hdrs.
                version2ComplexityEstimationDisable =
                StrmDec_GetBits(pDecContainer, 1);
            if(tmp == 0)
            {
                pDecContainer->Hdrs.sadct =
                    StrmDec_GetBits(pDecContainer, 1);
                pDecContainer->Hdrs.quarterpel =
                    StrmDec_GetBits(pDecContainer, 1);
            }
            else
            {
                /* initialisation to zero */
                pDecContainer->Hdrs.sadct = 0;
                pDecContainer->Hdrs.quarterpel = 0;
            }
        }
        else
        {
            /* init to zero */
            pDecContainer->Hdrs.sadct = 0;
            pDecContainer->Hdrs.quarterpel = 0;
        }
    }
    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.3  Function name:
                StrmDec_SaveUserData

        Purpose:
                Saves user data from headers

        Input:
                pointer to DecContainer

        Output:
                status (HANTRO_OK/NOK/END_OF_STREAM)

------------------------------------------------------------------------------*/

u32 StrmDec_SaveUserData(DecContainer * pDecContainer, u32 u32_mode)
{

    u32 tmp;
    u32 maxLen;
    u32 len;
    u8 *pOutput;
    u32 enable;
    u32 *plen;
    u32 status = HANTRO_OK;

    MP4DEC_DEBUG(("SAVE USER DATA\n"));

    tmp = StrmDec_ShowBits(pDecContainer, 32);
    if(tmp != SC_UD_START)
    {
        /* there is no user data */
        return (HANTRO_OK);
    }
    else
    {
        tmp = StrmDec_FlushBits(pDecContainer, 32);
        if (tmp == END_OF_STREAM)
            return (END_OF_STREAM);
    }
    MP4DEC_DEBUG(("SAVE USER DATA\n"));

    switch (u32_mode)
    {
        case SC_VOS_START:
            maxLen = pDecContainer->StrmDesc.userDataVOSMaxLen;
            plen = &(pDecContainer->StrmDesc.userDataVOSLen);
            pOutput = pDecContainer->StrmDesc.pUserDataVOS;
            break;

        case SC_VISO_START:
            maxLen = pDecContainer->StrmDesc.userDataVOMaxLen;
            plen = &(pDecContainer->StrmDesc.userDataVOLen);
            pOutput = pDecContainer->StrmDesc.pUserDataVO;
            break;

        case SC_VOL_START:
            maxLen = pDecContainer->StrmDesc.userDataVOLMaxLen;
            plen = &(pDecContainer->StrmDesc.userDataVOLLen);
            pOutput = pDecContainer->StrmDesc.pUserDataVOL;
            break;

        case SC_GVOP_START:
            maxLen = pDecContainer->StrmDesc.userDataGOVMaxLen;
            plen = &(pDecContainer->StrmDesc.userDataGOVLen);
            pOutput = pDecContainer->StrmDesc.pUserDataGOV;
            break;

        default:
            return (HANTRO_NOK);
    }

    MP4DEC_DEBUG(("SAVE USER DATA\n"));

    if(maxLen && pOutput)
        enable = 1;
    else
        enable = 0;

    MP4DEC_DEBUG(("SAVE USER DATA\n"));
    len = 0;

    ProcessUserData(pDecContainer);
    
    /* read (and save) user data */
    while(!IS_END_OF_STREAM(pDecContainer))
    {

        MP4DEC_DEBUG(("SAVING USER DATA\n"));
        tmp = StrmDec_ShowBits(pDecContainer, 32);
        if((tmp >> 8) == 0x01)
        {
            /* start code */
            if(tmp != SC_UD_START)
                break;
            else
            {
                (void) StrmDec_FlushBits(pDecContainer, 32);
                /* "new" user data -> have to process it also */
                ProcessUserData(pDecContainer);
                continue;
            }
        }
        tmp = tmp >> 24;
        (void) StrmDec_FlushBits(pDecContainer, 8);

        if(enable && (len < maxLen))
        {
            /*lint --e(613) */
            *pOutput++ = (u8) tmp;
        }

        len++;
    }

    /*lint --e(613) */
    *plen = len;

    /* did we catch something unsupported from the user data */
    if( pDecContainer->StrmStorage.unsupportedFeaturesPresent )
        return (HANTRO_NOK);

    return (status);

}

/*------------------------------------------------------------------------------

    5.4. Function name: StrmDec_ClearHeaders

         Purpose:   Initialize data to default values

         Input:     pointer to DecHdrs

         Output:

------------------------------------------------------------------------------*/

void StrmDec_ClearHeaders(DecHdrs * hdrs)
{
    u32 tmp;

    tmp = hdrs->profileAndLevelIndication;

    (void)G1DWLmemset(hdrs, 0, sizeof(DecHdrs));

    /* restore profile and level */
    hdrs->profileAndLevelIndication = tmp;

    /* have to be initialized into 1 to enable decoding stream without
     * VO-headers */
    hdrs->visualObjectVerid = 1;
    hdrs->videoFormat = 5;
    hdrs->colourPrimaries = 1;
    hdrs->transferCharacteristics = 1;
    hdrs->matrixCoefficients = 1;
    /* TODO */
    /*hdrs->lowDelay = 0;*/

    return;

}

/*------------------------------------------------------------------------------

        Function name:
                IntraQuantMat

        Purpose:
                Read quact matrises from the stream
        Input:
                pointer to DecContainer

        Output:
                status (HANTRO_OK/NOK/END_OF_STREAM)

------------------------------------------------------------------------------*/

u32 QuantMat(DecContainer * pDecContainer, u32 intra)
{

    u32 i;
    u32 tmp;
    u8 *p;

    p = pDecContainer->StrmStorage.quantMat;

    if (!intra)
        p += 64;

    i = 1;
    tmp = StrmDec_GetBits(pDecContainer, 8);

    if (tmp == 0)
        return(HANTRO_NOK);

    p[0] = tmp;
    while(i < 64)
    {
        tmp = StrmDec_GetBits(pDecContainer, 8);
        if (tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if (tmp == 0)
            break;
        p[zigZag[i++]] = tmp;
    }
    tmp = p[zigZag[i-1]];
    for (; i < 64; i++)
        p[zigZag[i]] = tmp;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

        Function name:
                MP4DecSetLowDelay

        Purpose:
                Based on profile set in stream, decide value for low delay
                (deprecated) 

        Input:
                pointer to DecContainer

        Output:
                status (HANTRO_OK/NOK/END_OF_STREAM)

------------------------------------------------------------------------------*/
static void MP4DecSetLowDelay(DecContainer * pDecContainer)
{

    /*
    1=Simple Profile/Level 1
    2=Simple Profile/Level 2
    3=Simple Profile/Level 3
    4=Simple Profile/Level 4a
    5=Simple Profile/Level 5
    8=Simple Profile/Level 0
    9=Simple Profile/Level 0B
    50=Main Profile/Level 2
    51=Main Profile/Level 3
    52=Main Profile/Level 4
    */
    u32 simple, tmp;

#if 0
    tmp = pDecContainer->Hdrs.profileAndLevelIndication;

    simple = (tmp == 1) || (tmp == 2) || (tmp == 3) ||
             (tmp == 4) || (tmp == 5) || (tmp == 8) || (tmp == 9) ||
             (tmp == 50) || (tmp == 51) || (tmp == 52) ||
             (tmp == 0) /* in case VOS headers missing */;

    pDecContainer->Hdrs.lowDelay = simple ? 1 : 0;
#endif

    pDecContainer->Hdrs.lowDelay = MP4DecBFrameSupport(pDecContainer) ? 0 : 1;

}
