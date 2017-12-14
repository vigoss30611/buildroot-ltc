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
--  Description : Picture layer decoding functions
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_picture_layer.c,v $
--  $Revision: 1.23 $
--  $Date: 2009/03/30 12:11:02 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

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

static void DecodeQpInfo( swStrmStorage_t* pStorage,
                          pictureLayer_t* pPicLayer,
                          strmData_t* pStrmData );

static void DecodeInterInformation( pictureLayer_t* pPicLayer,
                                    strmData_t* pStrmData,
                                    u32 vsTransform,
                                    u16x dquant );

static void DecodeIntensityCompensationInfo(strmData_t * const pStrmData,
                                            i32 *iShift, i32 *iScale );

static void NewFrameDimensions( swStrmStorage_t *pStorage, resPic_e res );

static void ProcessBitplanes( swStrmStorage_t* pStorage, 
    pictureLayer_t * pPicLayer );

static fieldType_e GetFieldType( pictureLayer_t * const pPicLayer, 
                                 u16x isFirstField );

/*------------------------------------------------------------------------------

    Function name: GetFieldType

        Functional description:
            Determines current field type I / P / B / BI
        
        Inputs:
            pPicLayer       Pointer to picture layer data structure
            isFirstField    is the first or the second field of the frame
        
        Outputs:
            None

        Returns:
            Current field type

------------------------------------------------------------------------------*/
fieldType_e GetFieldType( pictureLayer_t * const pPicLayer,
                          u16x isFirstField )
{
    if (isFirstField)
    {
        pPicLayer->isFF = HANTRO_TRUE;
        pPicLayer->topField = (pPicLayer->tff) ? 1 : 0;

        switch (pPicLayer->fieldPicType)
        {
            case FP_I_I: 
            case FP_I_P:
                pPicLayer->picType = PTYPE_I;
                return FTYPE_I;
            case FP_P_I:
            case FP_P_P:
                pPicLayer->picType = PTYPE_P;
                return FTYPE_P;
            case FP_B_B:
            case FP_B_BI:
                pPicLayer->picType = PTYPE_B;
                return FTYPE_B;
            default:
                pPicLayer->picType = PTYPE_BI;
                return FTYPE_BI;
        }
    }
    else
    {
        pPicLayer->isFF = HANTRO_FALSE;
        pPicLayer->topField = (pPicLayer->tff) ? 0 : 1;

        switch (pPicLayer->fieldPicType)
        {
            case FP_I_I: 
            case FP_P_I:
                pPicLayer->picType = PTYPE_I;
                return FTYPE_I;
            case FP_I_P:
            case FP_P_P:
                pPicLayer->picType = PTYPE_P;
                return FTYPE_P;
            case FP_B_B:
            case FP_BI_B:
                pPicLayer->picType = PTYPE_B;
                return FTYPE_B;
            default:
                pPicLayer->picType = PTYPE_BI;
                return FTYPE_BI;
        }
    }
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdDecodeFieldLayer

        Functional description:
            Decodes field headers

        Inputs:
            pStorage        Stream storage descriptor
            pStrmData       Pointer to stream data structure
            isFirstField    is the first or the second field of the frame

        Outputs:
            Field header information

        Returns:
            VC1HWD_FIELD_HDRS_RDY / HANTRO_NOK

------------------------------------------------------------------------------*/
u16x vc1hwdDecodeFieldLayer( swStrmStorage_t *pStorage, 
                             strmData_t *pStrmData,
                             u16x isFirstField )
{
    u16x ret;
    pictureLayer_t *pPicLayer;
    fieldType_e ft;
    u32 readBitsStart;

    ASSERT(pStorage);
    ASSERT(pStrmData);

    pPicLayer = &pStorage->picLayer;
    pPicLayer->rawMask = 0;
    pPicLayer->dqProfile = DQPROFILE_N_A;

    readBitsStart = pStrmData->strmBuffReadBits;

    ft = GetFieldType(pPicLayer, isFirstField);

    (void)G1DWLmemset(pStorage->pMbFlags, 0,((pStorage->numOfMbs+9)/10)*10 );

    /* PQINDEX, HALFQP, PQUANTIZER */
    DecodeQpInfo(pStorage, pPicLayer, pStrmData);
    
    /* POSTPROQ */
    if (pStorage->postProcFlag)
    {
        pPicLayer->postProc = vc1hwdGetBits(pStrmData, 2);
    }

    /* NUMREF */
    if (ft == FTYPE_P)
    {
        pPicLayer->numRef = vc1hwdGetBits(pStrmData, 1);
        /* REFFIELD */
        if (pPicLayer->numRef == 0)
            pPicLayer->refField = vc1hwdGetBits(pStrmData, 1);
    }

    if ( (ft == FTYPE_P) || (ft == FTYPE_B) )
    {
        /* MVRANGE */
        if (pStorage->extendedMv)
            pPicLayer->mvRange = vc1hwdDecodeMvRange(pStrmData);
        else
            pPicLayer->mvRange = 0;

        /* DMVRANGE */
        if (pStorage->extendedDmv)
            pPicLayer->dmvRange = vc1hwdDecodeDmvRange(pStrmData);
        else
            pPicLayer->dmvRange = 0;

        /* MVMODE/MVMODE2 */
        if (ft == FTYPE_P)
        {
            pPicLayer->mvmode = vc1hwdDecodeMvMode(pStrmData, 
                                    HANTRO_FALSE, pPicLayer->pquant,
                                    &pPicLayer->intensityCompensation);

            if (pPicLayer->intensityCompensation)
            {
                /* INTCOMPFIELD */
                pPicLayer->intCompField = vc1hwdDecodeIntCompField(pStrmData);

                /* LUMSCALE and LUMSHIFT */
                if (pPicLayer->intCompField == IC_BOTTOM_FIELD)
                {
                    DecodeIntensityCompensationInfo(pStrmData,
                                                    &pPicLayer->iShift2,
                                                    &pPicLayer->iScale2);
                }
                else
                {
                    DecodeIntensityCompensationInfo(pStrmData,
                                                    &pPicLayer->iShift,
                                                    &pPicLayer->iScale);
                }
                if (pPicLayer->intCompField == IC_BOTH_FIELDS)
                {
                    /* LUMSCALE2 and LUMSHIFT2 */
                    DecodeIntensityCompensationInfo(pStrmData,
                                                    &pPicLayer->iShift2,
                                                    &pPicLayer->iScale2);
                }
            }
            else
            {
                pPicLayer->intCompField = IC_NONE;
            }
        }
        else /* FTYPE_B */
        {
            pPicLayer->numRef = 1; /* Always 2 reference fields for B field 
                                    * pictures */
            pPicLayer->mvmode = vc1hwdDecodeMvModeB(pStrmData, 
                                                    pPicLayer->pquant);

            /* FORWARDMB Bitplane */
            ret = vc1hwdDecodeBitPlane(pStrmData, 
                    pStorage->picWidthInMbs,
                    ( pStorage->picHeightInMbs + 1 ) / 2, 
                    pStorage->pMbFlags,
                    MB_FORWARD_MB, &pPicLayer->rawMask, 
                    MB_FORWARD_MB, pStorage->syncMarker );
            if (ret != HANTRO_OK)
                return(ret);

        }
    }
   
    if ( (ft == FTYPE_P) || (ft == FTYPE_B) )
    {
        /* MBMODETAB */
        pPicLayer->mbModeTab = vc1hwdGetBits(pStrmData, 3);

        /* IMVTAB */
        if ((ft == FTYPE_P) && (pPicLayer->numRef == 0))
            pPicLayer->mvTableIndex = vc1hwdGetBits(pStrmData, 2);
        else
            pPicLayer->mvTableIndex = vc1hwdGetBits(pStrmData, 3);
        /* ICBPTAB */
        pPicLayer->cbpTableIndex = vc1hwdGetBits(pStrmData, 3);
        /* 4MVBPTAB */
        if (pPicLayer->mvmode == MVMODE_MIXEDMV )
            pPicLayer->mvbpTableIndex4 = vc1hwdGetBits(pStrmData, 2);
        /* VOPDQUANT */
        if (pStorage->dquant != 0)
        {
            vc1hwdDecodeVopDquant(pStrmData, pStorage->dquant, pPicLayer);

            /* Note: if DQ_PROFILE is ALL_MACROBLOCKS and DQBILEVEL 
             * is zero then set HALFQP to zero as in that case no 
             * MQUANT is considered to be derived from PQUANT. */
            if( pPicLayer->dqProfile == DQPROFILE_ALL_MACROBLOCKS &&
                pPicLayer->dqbiLevel == 0 )
                pPicLayer->halfQp = 0;        
        }
        /* TTMBF */
        pPicLayer->mbLevelTransformTypeFlag = 1;
        pPicLayer->ttFrm = TT_8x8;
        if ( pStorage->vsTransform ) {
            pPicLayer->mbLevelTransformTypeFlag = vc1hwdGetBits(pStrmData, 1);
            if( pPicLayer->mbLevelTransformTypeFlag == 1 ) {
                /* TTFRM */
                pPicLayer->ttFrm = (transformType_e)vc1hwdGetBits(pStrmData, 2);
            }
        }
    }

    if ( (ft == FTYPE_I) || (ft == FTYPE_BI) )
    {
        /* ACPRED Bitplane */
        ret = vc1hwdDecodeBitPlane(pStrmData, 
                pStorage->picWidthInMbs,
                ( pStorage->picHeightInMbs + 1 ) / 2, 
                pStorage->pMbFlags,
                MB_AC_PRED, &pPicLayer->rawMask, 
                MB_AC_PRED, pStorage->syncMarker );
        if (ret != HANTRO_OK)
            return(ret);

        if( pStorage->overlap && (pPicLayer->pquant <= 8) )
        {
            /* CONDOVER */
            pPicLayer->condOver = vc1hwdDecodeCondOver(pStrmData);
 
            /* OVERFLAGS Bitplane */
            if (pPicLayer->condOver == 3)
            {
                ret = vc1hwdDecodeBitPlane(pStrmData, 
                        pStorage->picWidthInMbs,
                        ( pStorage->picHeightInMbs + 1 ) / 2, 
                        pStorage->pMbFlags,
                        MB_OVERLAP_SMOOTH, &pPicLayer->rawMask, 
                        MB_OVERLAP_SMOOTH, pStorage->syncMarker);
                if (ret != HANTRO_OK)
                    return(ret);
            }
            else
            {
                pPicLayer->rawMask |= MB_OVERLAP_SMOOTH;
            }
        }
        else
        {
            pPicLayer->rawMask |= MB_OVERLAP_SMOOTH;
        }
    }
    /* TRANSACFRM */
    pPicLayer->acCodingSetIndexCbCr = vc1hwdDecodeTransAcFrm(pStrmData);

    /* TRANSACFRM2 */
    if ((ft == FTYPE_I) || (ft == FTYPE_BI))
    {
        pPicLayer->acCodingSetIndexY = vc1hwdDecodeTransAcFrm(pStrmData);
    }
    else {
        pPicLayer->acCodingSetIndexY = pPicLayer->acCodingSetIndexCbCr;
    }
    /* TRANSDCTAB */
    pPicLayer->intraTransformDcIndex = vc1hwdGetBits(pStrmData, 1);

    /* VOPDQUANT */
    if ( (pStorage->dquant != 0) && 
         ((ft == FTYPE_I) || (ft == FTYPE_BI)))
    {
        vc1hwdDecodeVopDquant(pStrmData, pStorage->dquant, pPicLayer);

        /* Note: if DQ_PROFILE is ALL_MACROBLOCKS and DQBILEVEL is zero then set
         * HALFQP to zero as in that case no MQUANT is considered to be derived
         * from PQUANT. */
        if( pPicLayer->dqProfile == DQPROFILE_ALL_MACROBLOCKS &&
            pPicLayer->dqbiLevel == 0 )
            pPicLayer->halfQp = 0;        
    }

    pPicLayer->fieldHeaderBits = pStrmData->strmBuffReadBits - readBitsStart;
    
#ifdef ASIC_TRACE_SUPPORT
    {
    mvmode_e mvMode = pPicLayer->mvmode;
        
    if (MVMODE_1MV == mvMode || MVMODE_MIXEDMV == mvMode)
    {
        trace_vc1DecodingTools.qpelLuma = 1;
        if (!pStorage->fastUvMc)
            trace_vc1DecodingTools.qpelChroma = 1;
    }
        
    }
#endif

    ProcessBitplanes( pStorage, pPicLayer );

    return VC1HWD_FIELD_HDRS_RDY;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdDecodePictureLayerAP

        Functional description:
            Decodes advanced profile picture layer headers

        Inputs:
            pStorage    Stream storage descriptor
            pStrmData   Pointer to stream data structure

        Outputs:
            Picture layer information

        Returns:
            VC1HWD_PIC_HDRS_RDY / VC1HWD_FIELD_HDRS_RDY / HANTRO_NOK
------------------------------------------------------------------------------*/
u16x vc1hwdDecodePictureLayerAP( swStrmStorage_t *pStorage, 
                                 strmData_t *pStrmData )
{
    pictureLayer_t *pPicLayer;
    u16x ret, i;
    u16x numberOfPanScanWindows = 0;

    ASSERT(pStorage);
    ASSERT(pStrmData);

    pPicLayer = &pStorage->picLayer;

    pPicLayer->rawMask = 0;
    pPicLayer->intensityCompensation = 0;

    (void)G1DWLmemset(pStorage->pMbFlags, 0,((pStorage->numOfMbs+9)/10)*10 );
    
    /* init these here */
    pPicLayer->isFF = HANTRO_TRUE;
    pPicLayer->topField = HANTRO_TRUE;
    pPicLayer->intCompField = IC_NONE;
    pPicLayer->dquantInFrame = HANTRO_FALSE;
    pPicLayer->dqProfile = DQPROFILE_N_A;
    
    /* FCM */
    if (pStorage->interlace)
        pPicLayer->fcm = vc1hwdDecodeFcm(pStrmData);
    else
        pPicLayer->fcm = PROGRESSIVE;
   
    /* picType is set for field pictures in GetFieldType function */
    if (pStorage->interlace && (pPicLayer->fcm == FIELD_INTERLACE))
    {
        /* FPTYPE */
        pPicLayer->fieldPicType = (fpType_e)vc1hwdGetBits(pStrmData, 3);
        /* set picType for current field */
        (void)GetFieldType(pPicLayer, 1);
    }
    else
    {
        /* PTYPE */
        pPicLayer->picType =
            vc1hwdDecodePtype(pStrmData, HANTRO_TRUE, pStorage->maxBframes);
    }
    /* TFCNTR */
    if (pStorage->tfcntrFlag && (pPicLayer->picType != PTYPE_Skip))
        pPicLayer->tfcntr = vc1hwdGetBits(pStrmData, 8);

    if (pStorage->pullDown)
    {
        /* RPTFRM */
        if ( (pStorage->interlace == 0) || (pStorage->psf == 1) )
        {
            pPicLayer->rptfrm = vc1hwdGetBits(pStrmData, 2);
        }
        else
        {
            /* TFF and RFF */
            pPicLayer->tff = vc1hwdGetBits(pStrmData, 1);
            pPicLayer->rff = vc1hwdGetBits(pStrmData, 1);
        }
    }
	else
	{
		pPicLayer->rptfrm = 0;
		pPicLayer->tff = HANTRO_TRUE;
		pPicLayer->rff = HANTRO_FALSE;
	}
    if (pStorage->panScanFlag)
    {
        /* PSPRESENT */
        pPicLayer->psPresent = vc1hwdGetBits(pStrmData, 1);
        if (pPicLayer->psPresent)
        {
            if ((pStorage->interlace == 1) && (pStorage->psf == 0))
            {
                if (pStorage->pullDown)
                    numberOfPanScanWindows = 2 + pPicLayer->rff;
                else
                    numberOfPanScanWindows = 2;
            }
            else
            {
                if (pStorage->pullDown)
                    numberOfPanScanWindows = 1 + pPicLayer->rptfrm;
                else
                    numberOfPanScanWindows = 1;
            }
            /* PS_HOFFSET, PS_VOFFSET, PS_WIDTH, PS_HEIGHT */
            for (i = 0; i < numberOfPanScanWindows; i++)
            {
                pPicLayer->psw.hOffset = vc1hwdGetBits(pStrmData, 18);
                pPicLayer->psw.vOffset = vc1hwdGetBits(pStrmData, 18);
                pPicLayer->psw.width = vc1hwdGetBits(pStrmData, 14);
                pPicLayer->psw.height = vc1hwdGetBits(pStrmData, 14);
            }
        }
    }

    /* return if skipped progressive frame */
    if (pPicLayer->picType == PTYPE_Skip)
    {
        return VC1HWD_PIC_HDRS_RDY;
    }

    /* RNDCTRL */
    pStorage->rnd = 1 - vc1hwdGetBits(pStrmData, 1);
    /* UVSAMP */
    if (pStorage->interlace)
        pPicLayer->uvSamp = vc1hwdGetBits(pStrmData, 1);
    /* INTERPFRM */
    if (pStorage->finterpFlag && (pPicLayer->fcm == PROGRESSIVE))
        pPicLayer->interpFrm = vc1hwdGetBits(pStrmData, 1);
    
    /* REFDIST */
    if ( pStorage->refDistFlag &&
         pPicLayer->fcm == FIELD_INTERLACE )
    {
        if (pPicLayer->fieldPicType < 4)
            pPicLayer->refDist = vc1hwdDecodeRefDist( pStrmData );
        /* Use previous anchor value for B/B etc field pairs */
    }
    else
        pPicLayer->refDist = 0;

    /* BFRACTION */
    if( ((pPicLayer->picType == PTYPE_B) && (pPicLayer->fcm == PROGRESSIVE)) ||
        ((pPicLayer->fieldPicType > 3) && (pPicLayer->fcm == FIELD_INTERLACE)))
    {
        pPicLayer->bfraction = vc1hwdDecodeBfraction( pStrmData,
            &pPicLayer->scaleFactor );
    }

    /* Decode rest of interlace field picture headers */
    if (pPicLayer->fcm == FIELD_INTERLACE)
    {
        ret = vc1hwdDecodeFieldLayer(pStorage, pStrmData, HANTRO_TRUE);
        return ret;
    }

    /* PQINDEX, HALFQP, PQUANTIZER */
    DecodeQpInfo(pStorage, pPicLayer, pStrmData);
    
    /* POSTPROQ */
    if (pStorage->postProcFlag)
    {
        pPicLayer->postProc = vc1hwdGetBits(pStrmData, 2);
    }

    /* BFRACTION */
    if ((pPicLayer->picType == PTYPE_B) && (pPicLayer->fcm == FRAME_INTERLACE))
    {
        pPicLayer->bfraction = vc1hwdDecodeBfraction( pStrmData,
            &pPicLayer->scaleFactor );
    }

    if ((pPicLayer->picType == PTYPE_I) || (pPicLayer->picType == PTYPE_BI))
    {

        if (pPicLayer->fcm == FRAME_INTERLACE)
        {
            /* FIELDTX */
            ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                        pStorage->picHeightInMbs, pStorage->pMbFlags,
                        MB_FIELD_TX, &pPicLayer->rawMask, 
                        MB_FIELD_TX, pStorage->syncMarker );
            if (ret != HANTRO_OK)
                return(ret);
        }

        /* ACPRED Bitplane */
        ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                pStorage->picHeightInMbs, pStorage->pMbFlags,
                MB_AC_PRED, &pPicLayer->rawMask, 
                MB_AC_PRED, pStorage->syncMarker );
        if (ret != HANTRO_OK)
            return(ret);

        if( pStorage->overlap && (pPicLayer->pquant <= 8) )
        {
            /* CONDOVER */
            pPicLayer->condOver = vc1hwdDecodeCondOver(pStrmData);
 
            /* OVERFLAGS Bitplane */
            if (pPicLayer->condOver == 3)
            {
                ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                        pStorage->picHeightInMbs, pStorage->pMbFlags,
                        MB_OVERLAP_SMOOTH, &pPicLayer->rawMask, 
                        MB_OVERLAP_SMOOTH, pStorage->syncMarker);
                if (ret != HANTRO_OK)
                    return(ret);
            }
            else
            {
                pPicLayer->rawMask |= MB_OVERLAP_SMOOTH;
            }
        }
        else
        {
            pPicLayer->rawMask |= MB_OVERLAP_SMOOTH;
        }
    }
    if ((pPicLayer->picType == PTYPE_P) || (pPicLayer->picType == PTYPE_B))
    {
        /* MVRANGE */
        if (pStorage->extendedMv)
            pPicLayer->mvRange = vc1hwdDecodeMvRange(pStrmData);
        else
            pPicLayer->mvRange = 0;
     
        /* DMVRANGE */
        if (pStorage->extendedDmv && (pPicLayer->fcm != PROGRESSIVE))
            pPicLayer->dmvRange = vc1hwdDecodeDmvRange(pStrmData);
        else
            pPicLayer->dmvRange = 0;

        /* MVMODE/MVMODE2 */
        if (pPicLayer->fcm == PROGRESSIVE)
        {
            pPicLayer->mvmode =
                vc1hwdDecodeMvMode(pStrmData, (pPicLayer->picType == PTYPE_B),
                    pPicLayer->pquant, &pPicLayer->intensityCompensation);
        }
    }

    if ( pPicLayer->fcm == FRAME_INTERLACE )
    {
        /* 4MVSWITCH */
        if (pPicLayer->picType == PTYPE_P)
        {
            pPicLayer->mvSwitch = vc1hwdGetBits(pStrmData, 1);
            pPicLayer->mvmode = 
                        pPicLayer->mvSwitch ? MVMODE_MIXEDMV : MVMODE_1MV;
        }
        else
        {
            pPicLayer->mvSwitch = 0;
            pPicLayer->mvmode = MVMODE_1MV;
        }
    }

    /* INTCOMP */
    if ( ((pPicLayer->picType == PTYPE_B) || 
          (pPicLayer->picType == PTYPE_P)) && 
          (pPicLayer->fcm == FRAME_INTERLACE))
        pPicLayer->intensityCompensation = vc1hwdGetBits(pStrmData, 1);

    /* LUMSCALE and LUMSHIFT */
    if ( pPicLayer->intensityCompensation && 
        (pPicLayer->fcm == FRAME_INTERLACE))
    {
        pPicLayer->intCompField = IC_BOTH_FIELDS;
        DecodeIntensityCompensationInfo(pStrmData, 
                                        &pPicLayer->iShift, 
                                        &pPicLayer->iScale);
    }

    if ((pPicLayer->picType == PTYPE_P) && (pPicLayer->fcm == PROGRESSIVE))
    {
        if (pPicLayer->intensityCompensation)
        {
            /* LUMSCALE and LUMSHIFT */
            pPicLayer->intCompField = IC_BOTH_FIELDS;
            DecodeIntensityCompensationInfo(pStrmData, 
                                            &pPicLayer->iShift, 
                                            &pPicLayer->iScale);
        }
        /* MVTYPEMB Bitplane */
        if( pPicLayer->mvmode == MVMODE_MIXEDMV )
        {
            ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                    pStorage->picHeightInMbs, pStorage->pMbFlags,
                    MB_4MV, &pPicLayer->rawMask, MB_4MV, pStorage->syncMarker);
            if (ret != HANTRO_OK)
                return(ret);
        }
    }
    if (pPicLayer->picType == PTYPE_B)
    {
        /* DIRECTMB Bitplane */
        ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                pStorage->picHeightInMbs, pStorage->pMbFlags,
                MB_4MV, &pPicLayer->rawMask,  MB_DIRECT, pStorage->syncMarker);
        if (ret != HANTRO_OK)
        {
            return(ret);
        }
    }
    if ((pPicLayer->picType == PTYPE_P) || (pPicLayer->picType == PTYPE_B))
    {
        /* SKIPMB Bitplane */
        ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                pStorage->picHeightInMbs, pStorage->pMbFlags,
                MB_SKIPPED, &pPicLayer->rawMask, 
                MB_SKIPPED, pStorage->syncMarker );
        if (ret != HANTRO_OK)
            return(ret);
        
        /* Decode Inter Stuff */
        if (pPicLayer->fcm == PROGRESSIVE)
        {
            DecodeInterInformation( pPicLayer, pStrmData,
                                    pStorage->vsTransform,
                                    pStorage->dquant);
        }

        if( pPicLayer->fcm == FRAME_INTERLACE )
        {
            /* MBMODETAB */
            pPicLayer->mbModeTab = vc1hwdGetBits(pStrmData, 2);
            /* IMVTAB */
            pPicLayer->mvTableIndex = vc1hwdGetBits(pStrmData, 2);
            /* ICBPTAB */
            pPicLayer->cbpTableIndex = vc1hwdGetBits(pStrmData, 3);
            /* 2MVBPTAB */
            pPicLayer->mvbpTableIndex2 = vc1hwdGetBits(pStrmData, 2);
            /* 4MVBPTAB */
            if ( (pPicLayer->picType == PTYPE_B) ||
                ((pPicLayer->picType == PTYPE_P) && (pPicLayer->mvSwitch==1)) )
                pPicLayer->mvbpTableIndex4 = vc1hwdGetBits(pStrmData, 2);
        }

        /* VOPDQUANT */
        if( pPicLayer->fcm != PROGRESSIVE)
        {
            if (pStorage->dquant != 0)
            {
               vc1hwdDecodeVopDquant(pStrmData, pStorage->dquant, pPicLayer);

                /* Note: if DQ_PROFILE is ALL_MACROBLOCKS and DQBILEVEL 
                 * is zero then set HALFQP to zero as in that case no 
                 * MQUANT is considered to be derived from PQUANT. */
                if( pPicLayer->dqProfile == DQPROFILE_ALL_MACROBLOCKS &&
                    pPicLayer->dqbiLevel == 0 )
                    pPicLayer->halfQp = 0;        
            }

            /* TTMBF */
            pPicLayer->mbLevelTransformTypeFlag = 1;
            pPicLayer->ttFrm = TT_8x8;
            if ( pStorage->vsTransform ) {
                pPicLayer->mbLevelTransformTypeFlag=vc1hwdGetBits(pStrmData, 1);
                if( pPicLayer->mbLevelTransformTypeFlag == 1 ) {
                    /* TTFRM */
                    pPicLayer->ttFrm = 
                                (transformType_e)vc1hwdGetBits(pStrmData, 2);
                }
            }
        }
    }

    /* TRANSACFRM */
    pPicLayer->acCodingSetIndexCbCr = vc1hwdDecodeTransAcFrm(pStrmData);

    /* TRANSACFRM2 */
    if ((pPicLayer->picType == PTYPE_I) || (pPicLayer->picType == PTYPE_BI))
    {
        pPicLayer->acCodingSetIndexY = vc1hwdDecodeTransAcFrm(pStrmData);
    }
    else {
        pPicLayer->acCodingSetIndexY = pPicLayer->acCodingSetIndexCbCr;
    }
    /* TRANSDCTAB */
    pPicLayer->intraTransformDcIndex = vc1hwdGetBits(pStrmData, 1);

    /* VOPDQUANT */
    if ( (pStorage->dquant != 0) && 
         ((pPicLayer->picType == PTYPE_I) || (pPicLayer->picType == PTYPE_BI)))
    {
        vc1hwdDecodeVopDquant(pStrmData, pStorage->dquant, pPicLayer);

        /* Note: if DQ_PROFILE is ALL_MACROBLOCKS and DQBILEVEL is zero then set
         * HALFQP to zero as in that case no MQUANT is considered to be derived
         * from PQUANT. */
        if( pPicLayer->dqProfile == DQPROFILE_ALL_MACROBLOCKS &&
            pPicLayer->dqbiLevel == 0 )
            pPicLayer->halfQp = 0;        
    }

#ifdef ASIC_TRACE_SUPPORT
    {
    mvmode_e mvMode = pPicLayer->mvmode;
        
    if (MVMODE_1MV == mvMode || MVMODE_MIXEDMV == mvMode)
    {
        trace_vc1DecodingTools.qpelLuma = 1;
        if (!pStorage->fastUvMc)
            trace_vc1DecodingTools.qpelChroma = 1;
    }
        
    }
#endif

    ProcessBitplanes( pStorage, pPicLayer );

    return VC1HWD_PIC_HDRS_RDY;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdDecodePictureLayer

        Functional description:
            Decodes simple and main profile picture layer

        Inputs:
            pStorage    Stream storage descriptor
            pStrmData   Pointer to stream data structure

        Outputs:
            Picture layer information

        Returns:
            VC1HWD_PIC_HDRS_RDY / HANTRO_NOK

------------------------------------------------------------------------------*/

u16x vc1hwdDecodePictureLayer(swStrmStorage_t *pStorage, strmData_t *pStrmData)
{
    pictureLayer_t *pPicLayer;
    i16x tmp;
    u16x ret;
    resPic_e    resPic;

    ASSERT(pStorage);
    ASSERT(pStrmData);

    pPicLayer = &pStorage->picLayer;

    pPicLayer->rawMask = 0;

    /* init values for siple and main profiles */
    pPicLayer->fcm = PROGRESSIVE;
    pPicLayer->isFF = HANTRO_TRUE;
    pPicLayer->topField = HANTRO_TRUE;
    pPicLayer->tff = HANTRO_TRUE;
    pPicLayer->intCompField = IC_NONE;
    pPicLayer->dqProfile = DQPROFILE_N_A;

    /* INTERPFRM */
    if (pStorage->frameInterpFlag) {
        pPicLayer->interpFrm = vc1hwdGetBits(pStrmData, 1);
    }

    /* FRMCNT */
    pPicLayer->frameCount = vc1hwdGetBits(pStrmData, 2);

    /* RANGEREDFRM */
    if (pStorage->rangeRed)
    {
        pPicLayer->rangeRedFrm = vc1hwdGetBits(pStrmData,1);
    }

    /* PTYPE */
    pPicLayer->picType =
        vc1hwdDecodePtype(pStrmData,HANTRO_FALSE,pStorage->maxBframes);

#ifdef _DPRINT_PRINT
    switch(pPicLayer->picType)
    {
        case PTYPE_P: DPRINT(("PTYPE=P\n")); break;
        case PTYPE_I: DPRINT(("PTYPE=I\n")); break;
        case PTYPE_B: DPRINT(("PTYPE=B\n")); break;
        case PTYPE_BI: DPRINT(("PTYPE=BI\n")); break;
    }
#endif /*_DPRINT_PRINT*/

    /* BFRACTION */
    if( pPicLayer->picType == PTYPE_B)
    {
        pPicLayer->bfraction = vc1hwdDecodeBfraction( pStrmData,
            &pPicLayer->scaleFactor );

        if( pPicLayer->bfraction == BFRACT_PTYPE_BI )
        {
            pPicLayer->picType = PTYPE_BI;
        }

#ifdef _DPRINT_PRINT
        DPRINT(("BFRACTION: "));
        switch( pPicLayer->bfraction )
        {
            case BFRACT_1_2: DPRINT(("1/2\n")); break;
            case BFRACT_1_3: DPRINT(("1/3\n")); break;
            case BFRACT_2_3: DPRINT(("2/3\n")); break;
            case BFRACT_1_4: DPRINT(("1/4\n")); break;
            case BFRACT_3_4: DPRINT(("3/4\n")); break;
            case BFRACT_1_5: DPRINT(("1/5\n")); break;
            case BFRACT_2_5: DPRINT(("2/5\n")); break;
            case BFRACT_3_5: DPRINT(("3/5\n")); break;
            case BFRACT_4_5: DPRINT(("4/5\n")); break;
            case BFRACT_1_6: DPRINT(("1/6\n")); break;
            case BFRACT_5_6: DPRINT(("5/6\n")); break;
            case BFRACT_1_7: DPRINT(("1/7\n")); break;
            case BFRACT_2_7: DPRINT(("2/7\n")); break;
            case BFRACT_3_7: DPRINT(("3/7\n")); break;
            case BFRACT_4_7: DPRINT(("4/7\n")); break;
            case BFRACT_5_7: DPRINT(("5/7\n")); break;
            case BFRACT_6_7: DPRINT(("6/7\n")); break;
            case BFRACT_1_8: DPRINT(("1/8\n")); break;
            case BFRACT_3_8: DPRINT(("3/8\n")); break;
            case BFRACT_5_8: DPRINT(("5/8\n")); break;
            case BFRACT_7_8: DPRINT(("7/8\n")); break;
            case BFRACT_SMPTE_RESERVED: DPRINT(("SMPTE Reserved\n")); break;
            case BFRACT_PTYPE_BI: DPRINT(("BI\n")); break;
        }

#endif


    }

    /* Initialize dquantInFrm for intra pictures... */
    if (pPicLayer->picType == PTYPE_I ||
        pPicLayer->picType == PTYPE_BI )
    {
        pPicLayer->dquantInFrame = HANTRO_FALSE;
    }

    /* BF */
    if (pPicLayer->picType == PTYPE_I ||
        pPicLayer->picType == PTYPE_BI )
    {
        pPicLayer->bufferFullness = vc1hwdGetBits(pStrmData, 7);

    }

    /* PQINDEX, HALFQP, PQUANTIZER */
    DecodeQpInfo(pStorage, pPicLayer, pStrmData);
    
    /* MVRANGE */
    if (pStorage->extendedMv) {
        pPicLayer->mvRange = vc1hwdDecodeMvRange(pStrmData);
    }
    else
        pPicLayer->mvRange = 0;

    /* RESPIC */
    if (((pPicLayer->picType == PTYPE_I) ||
         (pPicLayer->picType == PTYPE_P)) &&
          pStorage->multiRes )
    {
        resPic = (resPic_e)vc1hwdGetBits(pStrmData, 2);
        
#ifdef ASIC_TRACE_SUPPORT
        if (resPic)
            trace_vc1DecodingTools.multiResolution = 1;
#endif

        if( resPic != pPicLayer->resPic )
        {
            if( pPicLayer->picType == PTYPE_I )
            {
                /* Picture dimensions changed */
                DPRINT(("Picture dimensions changed! (%d)-->(%d)\n",
                    pPicLayer->resPic, resPic));
                NewFrameDimensions( pStorage, resPic );
                pPicLayer->resPic = resPic;

                pStorage->resolutionChanged = HANTRO_TRUE;
    
                /* Check against minimum size. 
                 * Maximum size is check in Seq layer */
                if ( (pStorage->curCodedWidth < MIN_PIC_WIDTH) ||
                     (pStorage->curCodedHeight < MIN_PIC_WIDTH) )
                {
                    EPRINT("Error! Changed picture dimensions are smaller "\
                            "than the allowed minimum!");
                    return (HANTRO_NOK);
                }
            }
            else
            {
                EPRINT("Error! RESPIC element of P picture differs from "\
                      "previous I picture!");
                return(HANTRO_NOK);
            }
        }
    }

    if (pPicLayer->picType == PTYPE_P)
    {

        (void)G1DWLmemset(pStorage->pMbFlags, 0,((pStorage->numOfMbs+9)/10)*10 );

        /* MVMODE */
        pPicLayer->mvmode =
            vc1hwdDecodeMvMode(pStrmData, HANTRO_FALSE, pPicLayer->pquant,
            &pPicLayer->intensityCompensation);

        if (pPicLayer->mvmode > MVMODE_MIXEDMV)
            return(HANTRO_NOK);
#if defined(_DPRINT_PRINT)
        DPRINT(("MVMODE: "));
        switch( pPicLayer->mvmode )
        {
        case MVMODE_MIXEDMV:
        DPRINT(("Mixed-MV\n"));
        break;
        case MVMODE_1MV:
        DPRINT(("1MV\n"));
        break;
        case MVMODE_1MV_HALFPEL:
        DPRINT(("1MV-halfpel\n"));
        break;
        case MVMODE_1MV_HALFPEL_LINEAR:
        DPRINT(("1MV-halfpel-linear\n"));
        break;
        }
#endif  /* defined(_DPRINT_PRINT) */

        if (pPicLayer->intensityCompensation)
        {
            DPRINT(("Intensity compensation used.\n"));
            pPicLayer->intCompField = IC_BOTH_FIELDS;
            DecodeIntensityCompensationInfo(pStrmData, 
                                            &pPicLayer->iShift,
                                            &pPicLayer->iScale);
        }

        /* MVTYPEMB Bitplane */
        if( pPicLayer->mvmode == MVMODE_MIXEDMV )
        {
            ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                    pStorage->picHeightInMbs, pStorage->pMbFlags,
                    MB_4MV, &pPicLayer->rawMask, MB_4MV, pStorage->syncMarker );
            if (ret != HANTRO_OK)
                return(ret);
        }

        /* SKIPMB Bitplane */
        ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                pStorage->picHeightInMbs, pStorage->pMbFlags,
                MB_SKIPPED, &pPicLayer->rawMask, 
                MB_SKIPPED, pStorage->syncMarker );
        if (ret != HANTRO_OK)
        {
            if (pPicLayer->frameCount == 3 &&
                vc1hwdShowBits(pStrmData, 8) == 0x0F)
            {
                (void)vc1hwdFlushBits(pStrmData, 8);
                pPicLayer->picType = PTYPE_I;
            }
            else
                return(ret);
        }

        /* Decode Inter Stuff */
        DecodeInterInformation(pPicLayer, pStrmData,
            pStorage->vsTransform,
            pStorage->dquant);
    }
    else if (pPicLayer->picType == PTYPE_B)
    {
        (void)G1DWLmemset(pStorage->pMbFlags, 0,((pStorage->numOfMbs+9)/10)*10 );

        /* MVMODE */
        pPicLayer->mvmode =
            vc1hwdDecodeMvMode(pStrmData, HANTRO_TRUE, pPicLayer->pquant,
            (u32*)&tmp ); /* preserve intensity compensation status from
                            * previous anchor frame */

#if defined(_DPRINT_PRINT)
DPRINT(("MVMODE: "));
        switch( pPicLayer->mvmode )
        {
        case MVMODE_MIXEDMV:
        DPRINT(("Mixed-MV\n"));
        break;
        case MVMODE_1MV:
        DPRINT(("1MV\n"));
        break;
        case MVMODE_1MV_HALFPEL:
        DPRINT(("1MV-halfpel\n"));
        break;
        case MVMODE_1MV_HALFPEL_LINEAR:
        DPRINT(("1MV-halfpel-linear\n"));
        break;
}
#endif  /* defined(_DPRINT_PRINT) */

        /* DIRECTMB Bitplane */
        ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                pStorage->picHeightInMbs, pStorage->pMbFlags,
                MB_4MV, &pPicLayer->rawMask,  MB_DIRECT, pStorage->syncMarker);
        if (ret != HANTRO_OK)
        {
                return(ret);
        }

        /* SKIPMB Bitplane */
        ret = vc1hwdDecodeBitPlane(pStrmData, pStorage->picWidthInMbs,
                pStorage->picHeightInMbs, pStorage->pMbFlags,
                MB_SKIPPED, &pPicLayer->rawMask, 
                MB_SKIPPED, pStorage->syncMarker);
        if (ret != HANTRO_OK)
        {
                return(ret);
        }

        /* Decode Inter Stuff */
        DecodeInterInformation(pPicLayer, pStrmData,
            pStorage->vsTransform,
            pStorage->dquant);
    }
    else if (pPicLayer->picType == PTYPE_I)
    {
        pPicLayer->intensityCompensation = HANTRO_FALSE;
    }

    /* AC pred always raw coded for SP/MP content */
    if( pPicLayer->picType == PTYPE_I || pPicLayer->picType == PTYPE_BI )
        pPicLayer->rawMask |= MB_AC_PRED;

    /* TRANSACFRM */
    pPicLayer->acCodingSetIndexCbCr = vc1hwdDecodeTransAcFrm(pStrmData);

    /* TRANSACFRM2 */
    if ((pPicLayer->picType == PTYPE_I) || (pPicLayer->picType == PTYPE_BI))
    {
        pPicLayer->acCodingSetIndexY = vc1hwdDecodeTransAcFrm(pStrmData);

    }
    else {
        pPicLayer->acCodingSetIndexY = pPicLayer->acCodingSetIndexCbCr;
    }
    /* TRANSDCTAB */
    pPicLayer->intraTransformDcIndex = vc1hwdGetBits(pStrmData, 1);

#ifdef ASIC_TRACE_SUPPORT
    {
    mvmode_e mvMode = pPicLayer->mvmode;
        
    if (MVMODE_1MV == mvMode || MVMODE_MIXEDMV == mvMode)
    {
        trace_vc1DecodingTools.qpelLuma = 1;
        if (!pStorage->fastUvMc)
            trace_vc1DecodingTools.qpelChroma = 1;
    }
        
    }
#endif

    ProcessBitplanes( pStorage, pPicLayer );
    return VC1HWD_PIC_HDRS_RDY;
}

/*------------------------------------------------------------------------------

    Function name: DecodeQpInfo

        Functional description:
            Decodes quantization parameters

        Inputs:
            pStorage        Pointer to stream storage descriptor
            pPicLayer       Pointer to picture layer descriptor
            pStrmData       Pointer to stream data

        Outputs:
            pqIndex, pquant, uniformQuantizer, halfQp

        Returns:
            None

------------------------------------------------------------------------------*/
void DecodeQpInfo( swStrmStorage_t* pStorage,
                   pictureLayer_t* pPicLayer,
                   strmData_t* pStrmData )
{
    u32 tmp;

    /* PQINDEX */
    tmp = vc1hwdGetBits(pStrmData, 5);

    if (tmp == 0)
        tmp = 1; /* force to valid PQINDEX value */

    /* store pqIndex */
    pPicLayer->pqIndex = tmp;
    if (pStorage->quantizer == 0)
    {
        /* Implicit quantizer (Table 36 in standard)*/
        if (tmp <= 8)
        {
            pPicLayer->pquant = tmp;
            pPicLayer->uniformQuantizer = HANTRO_TRUE;
        }
        else if (tmp >= 29)
        {
            if (tmp == 29)
                pPicLayer->pquant = 27;
            else if (tmp == 30)
                pPicLayer->pquant = 29;
            else /*if (tmp == 31)*/
                pPicLayer->pquant = 31;

            pPicLayer->uniformQuantizer = HANTRO_FALSE;
        }
        else
        {
            pPicLayer->pquant = tmp-3;
            pPicLayer->uniformQuantizer = HANTRO_FALSE;
        }
    }
    else
    {
        /* Explicit quantizer */
        pPicLayer->pquant = tmp;
        if( pStorage->quantizer == 2 )
            pPicLayer->uniformQuantizer = HANTRO_FALSE;
        else if( pStorage->quantizer == 3 )
            pPicLayer->uniformQuantizer = HANTRO_TRUE;
    }
    /* HALFQP */
    if (tmp <= 8) 
        pPicLayer->halfQp = vc1hwdGetBits(pStrmData, 1);
    else
        pPicLayer->halfQp = 0;

    /* PQUANTIZER */
    if (pStorage->quantizer == 1) {
        pPicLayer->uniformQuantizer = vc1hwdGetBits(pStrmData, 1);
    }

}

/*------------------------------------------------------------------------------

    Function name: DecodeInterInformation

        Functional description:
            Decodes some Inter frame specific syntax elements from stream
        
        Inputs:
            pPicLayer       Pointer to picture layer structure
            pStrmData       Pointer to stream data
            vsTransform     value of vsTransform
            dquant          is dquant present

        Outputs:

        Returns:
            None
------------------------------------------------------------------------------*/
void DecodeInterInformation( pictureLayer_t* pPicLayer,
                             strmData_t* pStrmData,
                             u32 vsTransform,
                             u16x dquant )
{

    /* MVTAB */
    pPicLayer->mvTableIndex = vc1hwdGetBits(pStrmData, 2);

    /* CBPTAB */
    pPicLayer->cbpTableIndex = vc1hwdGetBits(pStrmData, 2);

    vc1hwdDecodeVopDquant(pStrmData, dquant, pPicLayer);

    /* Note: if DQ_PROFILE is ALL_MACROBLOCKS and DQBILEVEL is zero then set
     * HALFQP to zero as in that case no MQUANT is considered to be derived
     * from PQUANT. */
    if( pPicLayer->dqProfile == DQPROFILE_ALL_MACROBLOCKS &&
        pPicLayer->dqbiLevel == 0 )
        pPicLayer->halfQp = 0;

    /* TTMBF */
    pPicLayer->mbLevelTransformTypeFlag = 1;
    pPicLayer->ttFrm = TT_8x8;
    if ( vsTransform ) {
        pPicLayer->mbLevelTransformTypeFlag = vc1hwdGetBits(pStrmData, 1);
        if( pPicLayer->mbLevelTransformTypeFlag == 1 ) {
            /* TTFRM */
            pPicLayer->ttFrm = (transformType_e)vc1hwdGetBits(pStrmData, 2);
        }
    }
}
/*------------------------------------------------------------------------------

    Function name: DecodeIntensityCompensationInfo
    Functional description:
            This function generages intensity compensation tables needed
            in motion compensation.

        Inputs:
            pStrmData   pointer to stream data

        Outputs:
            iShift      pointer to pPiclayer->iShift
            iScale      pointer to pPiclayer->iScale

        Returns:
            none

------------------------------------------------------------------------------*/
void DecodeIntensityCompensationInfo( strmData_t * const pStrmData,
                                      i32 *iShift, i32 *iScale )
{
    i16x lumScale, lumShift;

    /* LUMSCALE */
    lumScale = vc1hwdGetBits(pStrmData, 6);

    /* LUMSHIFT */
    lumShift = vc1hwdGetBits(pStrmData, 6);

    /* calculate scale and shift */
    if (lumScale == 0)
    {
        *iScale = -64;
        *iShift = (255*64) - (lumShift*2*64);
        if (lumShift > 31)
            *iShift += 128*64;
    }
    else
    {
        *iScale = lumScale+32;
        if (lumShift > 31)
            *iShift = (lumShift*64) - (64*64);
        else
            *iShift = lumShift*64;
    }

}



/*------------------------------------------------------------------------------

    Function name: NewFrameDimensions

        Functional description:
            Calculate new frame dimensions after a RESPIC element is read
            from bitstream.

        Inputs:
            pStorage    pointer to stream storage
            res         new resolution

        Outputs:
            pStorage    picture size and macroblock amount constants
                        reinitialized

        Returns:
            none

------------------------------------------------------------------------------*/
void NewFrameDimensions( swStrmStorage_t *pStorage, resPic_e res )
{

/* Variables */

    i16x x, y;

/* Code */

    ASSERT(pStorage);

    x = pStorage->maxCodedWidth;
    y = pStorage->maxCodedHeight;

    if( res == RESPIC_HALF_FULL || res == RESPIC_HALF_HALF )
    {
        x /= 2;
    }

    if( res == RESPIC_FULL_HALF || res == RESPIC_HALF_HALF )
    {
        y /= 2;
    }

    pStorage->picWidthInMbs     = ( x + 15 ) >> 4;
    pStorage->picHeightInMbs    = ( y + 15 ) >> 4;
    pStorage->numOfMbs          = (pStorage->picWidthInMbs *
                                   pStorage->picHeightInMbs);

    /* init coded image size to max coded image size */
    pStorage->curCodedWidth = x;
    pStorage->curCodedHeight = y;
}




/*------------------------------------------------------------------------------

    Function name: ProcessBitplanes

        Functional description:

        Inputs:

        Outputs:

        Returns:

------------------------------------------------------------------------------*/
void ProcessBitplanes( swStrmStorage_t* pStorage, pictureLayer_t * pPicLayer )
{

/* Variables */
    
    u8 * pTmp;
    i32 width;
    u32 mbs;
    u32 numSkipped = 0;
    static const u32 rawMasksMainProf[4] = {
        0L,
        MB_4MV | MB_SKIPPED,
        MB_DIRECT | MB_SKIPPED,
        0L
    };
    static const u32 rawMasksAdvProf[3][4] = 
    {
        /* Progressive */
        {   MB_AC_PRED | MB_OVERLAP_SMOOTH,     /* I */
            MB_4MV | MB_SKIPPED,                /* P */
            MB_DIRECT | MB_SKIPPED,             /* B */
            MB_AC_PRED | MB_OVERLAP_SMOOTH },   /* BI */
        /* Frame interlaced */
        {   MB_AC_PRED | MB_OVERLAP_SMOOTH | MB_FIELD_TX,   /* I */
            MB_SKIPPED,                                     /* P */
            MB_DIRECT | MB_SKIPPED,                         /* B */
            MB_AC_PRED | MB_OVERLAP_SMOOTH | MB_FIELD_TX},  /* BI */
        /* Field interlaced */
        {   MB_AC_PRED | MB_OVERLAP_SMOOTH,     /* I */
            0,                                  /* P */
            MB_FORWARD_MB,                      /* B */
            MB_AC_PRED | MB_OVERLAP_SMOOTH },   /* BI */
    };

/* Code */

    /* Set "unused" bitplanes to raw mode */
    if( pStorage->profile == VC1_ADVANCED )
        pPicLayer->rawMask |= 
            ( (~rawMasksAdvProf[ pPicLayer->fcm ][ pPicLayer->picType ]) & 7 );
    else
        pPicLayer->rawMask |= 
            ( (~rawMasksMainProf[ pPicLayer->picType ]) & 7 );

    /* Now check if all MBs are skipped */
    width = pStorage->picWidthInMbs;
    mbs = pStorage->numOfMbs;
    pTmp = pStorage->pMbFlags; 

    if( pPicLayer->fcm != FIELD_INTERLACE &&
        (pPicLayer->picType == PTYPE_B || pPicLayer->picType == PTYPE_P ) &&
        (pPicLayer->rawMask & MB_SKIPPED) == 0 ) 
    {
        while(mbs--)
        {
            if ( (*pTmp++) & MB_SKIPPED )
                numSkipped++;
        }
    }

    if( numSkipped == pStorage->numOfMbs )
    {
        /* Todo change picture type */
    }

}
