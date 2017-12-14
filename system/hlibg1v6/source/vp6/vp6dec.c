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
-  $RCSfile: vp6dec.c,v $
-  $Revision: 1.8 $
-  $Date: 2009/09/03 08:24:48 $
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"
#include "vp6dec.h"
#include "vp6booldec.h"
#include "vp6strmbuffer.h"
#include "vp6decodemode.h"
#include "vp6decodemv.h"
#include "vp6scanorder.h"

#define ReadHeaderBits(strm, bits)      Vp6StrmGetBits(strm, bits)

/****************************************************************************
 * 
 *  ROUTINE       :     VP6HWDeleteHuffman
 *
 *  INPUTS        :     PB_INSTANCE *pbi.
 *  
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     None.
 *
 *  FUNCTION      :     Kills huffman space.
 *
 *  SPECIAL NOTES :     None.
 *
 ****************************************************************************/
void VP6HWDeleteHuffman(PB_INSTANCE * pbi)
{
    if(pbi->huff != NULL)
        G1DWLfree(pbi->huff);

    pbi->huff = NULL;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6HWAllocateHuffman
 *
 *  INPUTS        :     PB_INSTANCE *pbi.
 *  
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     i32 0 if sucessfull, -1 if unsucessfull
 *
 *  FUNCTION      :     Sets aside space for the Huffman tables for decoding.
 *
 *  SPECIAL NOTES :     None.
 *
 ****************************************************************************/
i32 VP6HWAllocateHuffman(PB_INSTANCE * pbi)
{
    VP6HWDeleteHuffman(pbi);

    pbi->huff = (HUFF_INSTANCE *) G1DWLmalloc(sizeof(HUFF_INSTANCE));

    if(pbi->huff == NULL)
        return -1;

    G1DWLmemset(pbi->huff, 0, sizeof(HUFF_INSTANCE));

    return 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     LoadFrameHeader
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     BOOL: FALSE in case of error, TRUE otherwise.
 *
 *  FUNCTION      :     Loads a frame header & carries out some initialization.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
i32 VP6HWLoadFrameHeader(PB_INSTANCE * pbi)
{
    u8 DctQMask;
    Vp6StrmBuffer *Header = &pbi->strm;

    // Is the frame and inter frame or a key frame
    pbi->FrameType = (u8) ReadHeaderBits(Header, 1);

    // Quality (Q) index
    DctQMask = (u8) ReadHeaderBits(Header, 6);

    // Are we using two BOOL coder data streams/partitions
    pbi->MultiStream = (u8) ReadHeaderBits(Header, 1);

    // If the frame was a base frame then read the frame dimensions and build a bitmap structure. 
    if((pbi->FrameType == BASE_FRAME))
    {
        // Read the frame dimensions bytes (0,0 indicates vp31 or later)
        pbi->Vp3VersionNo = (u8) ReadHeaderBits(Header, 5);
        pbi->VpProfile = (u8) ReadHeaderBits(Header, 2);

        if(pbi->Vp3VersionNo > CURRENT_DECODE_VERSION)
        {
            return -1;
        }

        // reserved
        if((u8) ReadHeaderBits(Header, 1) != 0)
        {
            //this is an old test bitstream and is not supported
            return -1;
        }

        // Start the first bool decoder (modes, mv, probs and some flags)
        // The offset depends on whether or not we are using multiple bool code streams
        if(pbi->MultiStream)
        {
            VP6HWStartDecode(&pbi->br, ((u8 *) (Header->buffer + 4)),
                Header->amountLeft);

            // Read the buffer offset for the second bool decoder buffer if it is being used
            pbi->Buff2Offset = (u32) ReadHeaderBits(Header, 16);
        }
        else
            VP6HWStartDecode(&pbi->br, ((u8 *) (Header->buffer + 2)),
                Header->amountLeft+2);

        {
            u32 HFragments;
            u32 VFragments;
            u32 OutputHFragments;
            u32 OutputVFragments;

            VFragments = 2 * ((u8) VP6HWbitread(&pbi->br, 8));
            HFragments = 2 * ((u8) VP6HWbitread(&pbi->br, 8));

            OutputVFragments = 2 * ((u8) VP6HWbitread(&pbi->br, 8));
            OutputHFragments = 2 * ((u8) VP6HWbitread(&pbi->br, 8));

            pbi->ScalingMode = ((u32) VP6HWbitread(&pbi->br, 2));

            // we have a new input size
            if(VFragments != pbi->VFragments || HFragments != pbi->HFragments)
            {

            }

            pbi->HFragments = HFragments;
            pbi->VFragments = VFragments;
            pbi->OutputHeight = OutputVFragments;
            pbi->OutputWidth = OutputHFragments;
        }

        // Unless in SIMPLE_PROFILE read the the filter strategy for fractional pels
        if(pbi->VpProfile != SIMPLE_PROFILE)
        {
            // Find out if selective bicubic filtering should be used for motion prediction.
            if(VP6HWDecodeBool(&pbi->br, 128))
            {
                pbi->PredictionFilterMode = AUTO_SELECT_PM;

                // Read in the variance threshold to be used
                pbi->PredictionFilterVarThresh =
                    ((u32) VP6HWbitread(&pbi->br, 5) <<
                     ((pbi->Vp3VersionNo > 7) ? 0 : 5));

                // Read the bicubic vector length limit (0 actually means ignore vector length)
                pbi->PredictionFilterMvSizeThresh =
                    (u8) VP6HWbitread(&pbi->br, 3);
            }
            else
            {
                if(VP6HWDecodeBool(&pbi->br, 128))
                {
                    pbi->PredictionFilterMode = BICUBIC_ONLY_PM;
                }
                else
                {
                    pbi->PredictionFilterMode = BILINEAR_ONLY_PM;
                }
            }

            if(pbi->Vp3VersionNo > 7)
                pbi->PredictionFilterAlpha = VP6HWbitread(&pbi->br, 4);
            else
                pbi->PredictionFilterAlpha = 16;    // VP61 backwards compatibility

            //uses vp62 predicted filters
            /*overridePredictFilteredFuncs(0); */
        }
        else
        {
            // VP60 backwards compatibility
            pbi->PredictionFilterAlpha = 14;
            pbi->PredictionFilterMode = BILINEAR_ONLY_PM;

            //uses vp60 predicted filters
            /*overridePredictFilteredFuncs(1); */
            // VP60 backwards compatibility
        }

        pbi->UseLoopFilter = 0;

    }
    // Non key frame specific stuff
    else
    {
        //Check to make sure that the first frame is not a non-keyframe
        if(pbi->Vp3VersionNo == 0)
        {
            return -88;
        }

        // Start the first bool decoder (modes, mv, probs and some flags)
        // The offset depends on whether or not we are using multiple bool code streams
        if(pbi->MultiStream)
        {
            VP6HWStartDecode(&pbi->br, ((u8 *) (Header->buffer + 3)),
                Header->amountLeft+1);

            // Read the buffer offset for the second bool decoder buffer if it is being used
            pbi->Buff2Offset = (u32) ReadHeaderBits(Header, 16);
        }
        else
            VP6HWStartDecode(&pbi->br, ((u8 *) (Header->buffer + 1)),
                Header->amountLeft+3);

        // Find out if the golden frame should be refreshed this frame - use bool decoder
        pbi->RefreshGoldenFrame = VP6HWDecodeBool(&pbi->br, 128);

        if(pbi->VpProfile != SIMPLE_PROFILE)
        {
            // Determine if loop filtering is on and if so what type should be used
            pbi->UseLoopFilter = VP6HWDecodeBool(&pbi->br, 128);
            if(pbi->UseLoopFilter)
            {
                pbi->UseLoopFilter =
                    (pbi->UseLoopFilter << 1) | VP6HWDecodeBool(&pbi->br, 128);
            }

            if(pbi->Vp3VersionNo > 7)
            {
                // Are the prediction characteristics being updated this frame
                if(VP6HWDecodeBool(&pbi->br, 128))
                {
                    // Find out if selective bicubic filtering should be used for motion prediction.
                    if(VP6HWDecodeBool(&pbi->br, 128))
                    {
                        pbi->PredictionFilterMode = AUTO_SELECT_PM;

                        // Read in the variance threshold to be used
                        pbi->PredictionFilterVarThresh =
                            (u32) VP6HWbitread(&pbi->br, 5);

                        // Read the bicubic vector length limit (0 actually means ignore vector length)
                        pbi->PredictionFilterMvSizeThresh =
                            (u8) VP6HWbitread(&pbi->br, 3);
                    }
                    else
                    {
                        if(VP6HWDecodeBool(&pbi->br, 128))
                        {
                            pbi->PredictionFilterMode = BICUBIC_ONLY_PM;
                        }
                        else
                        {
                            pbi->PredictionFilterMode = BILINEAR_ONLY_PM;
                        }
                    }

                    pbi->PredictionFilterAlpha = VP6HWbitread(&pbi->br, 4);
                }
            }
            else
                pbi->PredictionFilterAlpha = 16;    // VP61 backwards compatibility

            //uses vp62 predicted filters
            /*overridePredictFilteredFuncs(0); */
        }
        else
        {
            // VP60 backwards compatibility
            pbi->UseLoopFilter = 0;
            /*pbi->PredictionFilterAlpha = 14; */
            pbi->PredictionFilterMode = BILINEAR_ONLY_PM;

            //uses vp60 predicted filters
            /*overridePredictFilteredFuncs(1); */
            // VP60 backwards compatibility
        }

    }

    // All frames (Key & Inter frames)

    // Should this frame use huffman for the dct data
    // need to check if huffman buffers have been initialized, if not, do it.
    pbi->UseHuffman = VP6HWDecodeBool(&pbi->br, 128);

    if(pbi->UseHuffman && !pbi->huff)
        if(VP6HWAllocateHuffman(pbi))
            return -2;

    // Set this frame quality value from Q Index
    pbi->DctQMask = DctQMask;

    /*VP6HWUpdateQ(pbi->quantizer, pbi->Vp3VersionNo); */

    return 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6HWConfigureContexts
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *                      
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Configures the context dependant entropy probabilities.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6HWConfigureContexts(PB_INSTANCE * pbi)
{
    u32 i;
    u32 Node;
    u32 Plane;
    i32 Temp;

    // DC Node Probabilities
    for(Plane = 0; Plane < 2; Plane++)
    {
        for(i = 0; i < DC_TOKEN_CONTEXTS; i++)
        {
            // Tree Nodes
            for(Node = 0; Node < CONTEXT_NODES; Node++)
            {
                Temp =
                    ((pbi->DcProbs[DCProbOffset(Plane, Node)] *
                      VP6HWDcNodeEqs[Node][i].M + 128) >> 8) +
                    VP6HWDcNodeEqs[Node][i].C;
                Temp = (Temp > 255) ? 255 : Temp;
                Temp = (Temp < 1) ? 1 : Temp;

                //pbi->DcNodeContexts[Plane][i][Node] = (u8)Temp;
                *(pbi->DcNodeContexts + DcNodeOffset(Plane, i, Node)) =
                    (u8) Temp;
            }
        }
    }
}

#define nDecodeBool(br, probability)        VP6HWDecodeBool(br, probability)
/****************************************************************************
* 
*  ROUTINE       :     VP6HWConfigureEntropyDecoder
*
*  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
*                      u8 FrameType  : Type of frame.
*
*  OUTPUTS       :     None.
*
*  RETURNS       :     void
*
*  FUNCTION      :     Configure entropy subsystem ready for decode
*
*  SPECIAL NOTES :     None. 
*
****************************************************************************/
void VP6HWConfigureEntropyDecoder(PB_INSTANCE * pbi, u8 FrameType)
{
    u32 i, j;
    u32 Plane;
    u32 Band;
    i32 Prec;
    u8 LastProb[MAX_ENTROPY_TOKENS - 1];

    // Clear down Last Probs data structure
    G1DWLmemset(LastProb, 128, MAX_ENTROPY_TOKENS - 1);

    // Read in the Baseline DC probabilities and initialise the DC context for Y and then UV plane
    for(Plane = 0; Plane < 2; Plane++)
    {
        // If so then read them in.
        for(i = 0; i < MAX_ENTROPY_TOKENS - 1; i++)
        {
            if(nDecodeBool(&pbi->br, VP6HWDcUpdateProbs[Plane][i]))
            {
                // 0 is not a legal value, clip to 1.
                LastProb[i] =
                    VP6HWbitread(&pbi->br, PROB_UPDATE_BASELINE_COST) << 1;
                LastProb[i] += (LastProb[i] == 0);
                pbi->DcProbs[DCProbOffset(Plane, i)] = LastProb[i];

                pbi->probDcUpdate = 1;
            }
            else if(FrameType == BASE_FRAME)
            {
                pbi->DcProbs[DCProbOffset(Plane, i)] = LastProb[i];
            }
        }
    }

    // Set Zero run probabilities to defaults if this is a key frame
    if(FrameType == BASE_FRAME)
    {
        G1DWLmemcpy(pbi->ZeroRunProbs, VP6HW_ZeroRunProbDefaults,
                  sizeof(pbi->ZeroRunProbs));
    }

    // If this frame contains updates to the scan order then read them
    if(nDecodeBool(&pbi->br, 128))
    {
        // Read in the AC scan bands and build the custom scan order
        for(i = 1; i < BLOCK_SIZE; i++)
        {
            // Has the band for this coef been updated ?
            if(nDecodeBool(&pbi->br, VP6HW_ScanBandUpdateProbs[i]))
                pbi->ScanBands[i] =
                    VP6HWbitread(&pbi->br, SCAN_BAND_UPDATE_BITS);
        }
        // Build the scan order
        VP6HWBuildScanOrder(pbi, pbi->ScanBands);

        pbi->scanUpdate = 1;
    }

    // Update the Zero Run probabilities
    for(i = 0; i < ZRL_BANDS; i++)
    {
        for(j = 0; j < ZERO_RUN_PROB_CASES; j++)
        {
            if(nDecodeBool(&pbi->br, VP6HW_ZrlUpdateProbs[i][j]))
            {
                // Probabilities sent
                pbi->ZeroRunProbs[i][j] =
                    VP6HWbitread(&pbi->br, PROB_UPDATE_BASELINE_COST) << 1;
                pbi->ZeroRunProbs[i][j] += (pbi->ZeroRunProbs[i][j] == 0);

                pbi->probZrlUpdate = 1;
            }
        }
    }

    // Read in the Baseline AC band probabilities and initialise the appropriate contexts
    // Prec=0 means last token in current block was 0: Prec=1 means it was 1. Prec=2 means it was > 1
    for(Prec = 0; Prec < PREC_CASES; Prec++)
    {
        //PrecNonZero = ( Prec > 0 ) ? 1 : 0;
        for(Plane = 0; Plane < 2; Plane++)
        {
            for(Band = 0; Band < VP6HWAC_BANDS; Band++)
            {
                // If so then read them in.
                for(i = 0; i < MAX_ENTROPY_TOKENS - 1; i++)
                {
                    if(nDecodeBool
                       (&pbi->br, VP6HWAcUpdateProbs[Prec][Plane][Band][i]))
                    {
                        // Probabilities transmitted at reduced resolution. 
                        // 0 is not a legal value, clip to 1.
                        LastProb[i] =
                            VP6HWbitread(&pbi->br,
                                         PROB_UPDATE_BASELINE_COST) << 1;
                        LastProb[i] += (LastProb[i] == 0);
                        pbi->AcProbs[ACProbOffset(Plane, Prec, Band, i)] =
                            LastProb[i];

                        pbi->probAcUpdate = 1;
                    }
                    else if(FrameType == BASE_FRAME)
                    {
                        pbi->AcProbs[ACProbOffset(Plane, Prec, Band, i)] =
                            LastProb[i];
                    }
                }
            }
        }
    }

    // Create all the context specific propabilities based upon the new baseline data
    VP6HWConfigureContexts(pbi);

}

/****************************************************************************
* 
*  ROUTINE       :     VP6_ConvertDecodeBoolTrees
*
*  INPUTS        :     PB_INSTANCE *pbi
*
*  OUTPUTS       :     None
*
*  RETURNS       :     None.
*
*  FUNCTION      :     Convert trees used for Bool coding to set of token probs.
*
*  SPECIAL NOTES :     None. 
*
*
*  ERRORS        :     None.
*
****************************************************************************/
void VP6HW_ConvertDecodeBoolTrees(PB_INSTANCE * pbi)
{
    u32 i;
    u32 Plane;
    u32 Band;
    i32 Prec;
    HUFF_INSTANCE *huff = pbi->huff;

    // Convert bool tree node probabilities into array of token 
    // probabilities. Use these to create a set of Huffman codes
    // DC
    for(Plane = 0; Plane < 2; Plane++)
    {
        VP6HW_BoolTreeToHuffCodes(pbi->DcProbs + DCProbOffset(Plane, 0),
                                  huff->DcHuffProbs[Plane]);
        VP6HW_BuildHuffTree(huff->DcHuffTree[Plane], huff->DcHuffProbs[Plane],
                            MAX_ENTROPY_TOKENS);

        // fast huffman lookup
        /*VP6HW_BuildHuffLookupTable (huff->DcHuffTree[Plane], huff->DcHuffLUT[Plane]); */
        VP6HW_CreateHuffmanLUT(huff->DcHuffTree[Plane], huff->DcHuffLUT[Plane],
                               MAX_ENTROPY_TOKENS);
    }

    // ZEROS
    for(i = 0; i < ZRL_BANDS; i++)
    {
        VP6HW_ZerosBoolTreeToHuffCodes(pbi->ZeroRunProbs[i],
                                       huff->ZeroHuffProbs[i]);
        VP6HW_BuildHuffTree(huff->ZeroHuffTree[i], huff->ZeroHuffProbs[i], 9);

        // fast huffman lookup
        /*VP6HW_BuildHuffLookupTable (huff->ZeroHuffTree[i], huff->ZeroHuffLUT[i]); */
        VP6HW_CreateHuffmanLUT(huff->ZeroHuffTree[i], huff->ZeroHuffLUT[i], 9);
    }

    // AC
    for(Prec = 0; Prec < PREC_CASES; Prec++)
    {
        // Baseline probabilities for each AC band.
        for(Plane = 0; Plane < 2; Plane++)
        {
            for(Band = 0; Band < VP6HWAC_BANDS; Band++)
            {
                VP6HW_BoolTreeToHuffCodes(pbi->AcProbs +
                                          ACProbOffset(Plane, Prec, Band, 0),
                                          huff->AcHuffProbs[Prec][Plane][Band]);
                VP6HW_BuildHuffTree(huff->AcHuffTree[Prec][Plane][Band],
                                    huff->AcHuffProbs[Prec][Plane][Band],
                                    MAX_ENTROPY_TOKENS);

                // fast huffman lookup
                /*VP6HW_BuildHuffLookupTable (huff->AcHuffTree[Prec][Plane][Band], huff->AcHuffLUT[Prec][Plane][Band]); */
                /* VP6HW_CreateHuffmanLUT(huff->AcHuffTree[Prec][Plane][Band],
                                       huff->AcHuffLUT[Prec][Plane][Band],
                                       MAX_ENTROPY_TOKENS); */
            }
        }
    }
    
    /* Baseline probabilities for each AC band. */
    for(Plane = 0; Plane < 2; Plane++)
    {
        for(Prec = 0; Prec < PREC_CASES; Prec++)
        {
            for(Band = 0; Band < /* VP6HWAC_BANDS */ 4; Band++)
            {
                /* fast huffman lookup */
                VP6HW_CreateHuffmanLUT(huff->AcHuffTree[Prec][Plane][Band],
                                       huff->AcHuffLUT[Plane][Prec][Band],
                                       MAX_ENTROPY_TOKENS);
            }
        }
    }

}

/*------------------------------------------------------------------------------
    Function name   : VP6HWDecodeProbUpdates
    Description     : 
    Return type     : i32 
    Argument        : PB_INSTANCE * pbi
------------------------------------------------------------------------------*/
i32 VP6HWDecodeProbUpdates(PB_INSTANCE * pbi)
{
    pbi->probModeUpdate = 0;
    pbi->probMvUpdate = 0;
    pbi->scanUpdate = 0;
    pbi->probDcUpdate = 0;
    pbi->probAcUpdate = 0;
    pbi->probZrlUpdate = 0;

    if(pbi->FrameType != BASE_FRAME)
    {
        VP6HWDecodeModeProbs(pbi);
        VP6HWConfigureMvEntropyDecoder(pbi, pbi->FrameType);
        /*pbi->LastMode = CODE_INTER_NO_MV; */
    }
    else
    {
        G1DWLmemcpy(pbi->probXmitted, VP6HWBaselineXmittedProbs,
                  sizeof(pbi->probXmitted));

        G1DWLmemcpy(pbi->IsMvShortProb, VP6HW_DefaultIsShortProbs,
                  sizeof(pbi->IsMvShortProb));
        G1DWLmemcpy(pbi->MvShortProbs, VP6HW_DefaultMvShortProbs,
                  sizeof(pbi->MvShortProbs));
        G1DWLmemcpy(pbi->MvSignProbs, VP6HW_DefaultSignProbs,
                  sizeof(pbi->MvSignProbs));
        G1DWLmemcpy(pbi->MvSizeProbs, VP6HW_DefaultMvLongProbs,
                  sizeof(pbi->MvSizeProbs));

        /*G1DWLmemset ( pbi->predictionMode,1,sizeof(char)*pbi->MacroBlocks ); */

        G1DWLmemcpy(pbi->ScanBands, VP6HW_DefaultScanBands,
                  sizeof(pbi->ScanBands));

        // Build the scan order
        VP6HWBuildScanOrder(pbi, pbi->ScanBands);

    }

    VP6HWConfigureEntropyDecoder(pbi, pbi->FrameType);

    /* Create Huffman codes for tokens based on tree probabilities */
    if(pbi->UseHuffman)
    {
        VP6HW_ConvertDecodeBoolTrees(pbi);
    }

    return 0;
}
