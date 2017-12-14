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
--  Abstract : Common tracing functionalities for the 8170 system model.
--
------------------------------------------------------------------------------*/
#include "trace.h"

extern u32 picNumber;
extern u32 testDataFormat;  /* 0->trc, 1->hex */

trace_mpeg2DecodingTools_t trace_mpeg2DecodingTools;
trace_h264DecodingTools_t trace_h264DecodingTools;
trace_jpegDecodingTools_t trace_jpegDecodingTools;
trace_mpeg4DecodingTools_t trace_mpeg4DecodingTools;
trace_vc1DecodingTools_t trace_vc1DecodingTools;
trace_rvDecodingTools_t trace_rvDecodingTools;
u32 trace_refBuffHitRate;

/*
extern FILE *traceAcdcdOut;
extern FILE *traceAcdcdOutData;
extern FILE *traceDecodedMvs;
extern FILE *traceRlc;
extern FILE *traceMbCtrl;
extern FILE *traceMbCtrlHex;
extern FILE *tracePictureCtrlDec;
extern FILE *tracePictureCtrlPp;
extern FILE *tracePictureCtrlDecTiled;
extern FILE *tracePictureCtrlPpTiled;
extern FILE *traceMotionVectors;
extern FILE *traceMotionVectorsHex;
extern FILE *traceDirModeMvs;
extern FILE *traceFinalMvs;
extern FILE *traceDirModeMvsHex;
extern FILE *traceSeparDc;
extern FILE *traceSeparDcHex;
extern FILE *traceRecon;
extern FILE *traceScdOutData;
extern FILE *traceBs;
extern FILE *traceBitPlaneCtrl;
extern FILE *traceTransdFirstRound;
extern FILE *traceInterOutData;
extern FILE *traceOverlapSmooth;
extern FILE *traceStream;
extern FILE *traceStreamCtrl;
extern FILE *traceSwRegAccess;
extern FILE *traceSwRegAccessTiled;
extern FILE *traceQTables;
extern FILE *traceIntra4x4Modes;
extern FILE *traceIntra4x4ModesHex;
extern FILE *traceDctOutData;
extern FILE *traceInterRefY;
extern FILE *traceInterRefY1;
extern FILE *traceInterRefCb;
extern FILE *traceInterRefCb1;
extern FILE *traceInterRefCr;
extern FILE *traceInterRefCr1;
extern FILE *traceOverfill;
extern FILE *traceOverfill1;
extern FILE *traceIntraPred;
extern FILE *traceH264PicIdMap;
extern FILE *traceResidual;
extern FILE *traceIQ;
extern FILE *traceJpegTables;
extern FILE *traceOut;
extern FILE *traceOutTiled;
extern FILE *traceVc1FilteringCtrl;
extern FILE *traceStreamTxt;
extern FILE *traceCabacTable[4];
extern FILE *traceCabacBin;
extern FILE *traceCabacCtx;
extern FILE *traceNeighbourMv;
extern FILE *traceIcSets;
extern FILE *traceAboveMbMem;
extern FILE *tracePicOrdCnts;
extern FILE *traceScalingLists;
extern FILE *traceRefPicList;
extern FILE *traceImplicitWeights;
extern FILE *traceIntraFilteredPels;
extern FILE *traceMotionVectorsFixed;
extern FILE *traceDecodedMvsFixed;
extern FILE *traceDirMvFetch;
extern FILE *traceScalingOut;
extern FILE *tracePjpegCoeffs;
extern FILE *tracePjpegCoeffsHex;
extern FILE *traceInterOutY;
extern FILE *traceInterOutY1;
extern FILE *traceInterOutCb;
extern FILE *traceInterOutCb1;
extern FILE *traceInterOutCr;
extern FILE *traceInterOutCr1;
extern FILE *traceFilterInternal;
extern FILE *traceOut2ndCh;

extern FILE *tracePpAblend1;
extern FILE *tracePpAblend2;
extern FILE *tracePpIn;
extern FILE *tracePpInTiled;
extern FILE *tracePpInBot;
extern FILE *tracePpOut;
extern FILE *tracePpBackground;
extern FILE *tracePpDeintIn;
extern FILE *tracePpDeintOutY;
extern FILE *tracePpDeintOutCb;
extern FILE *tracePpDeintOutCr;
extern FILE *tracePpCrop;
extern FILE *tracePpRotation;
extern FILE *tracePpScalingKernel;
extern FILE *tracePpScalingR;
extern FILE *tracePpScalingG;
extern FILE *tracePpScalingB;
extern FILE *tracePpColorConvR;
extern FILE *tracePpColorConvG;
extern FILE *tracePpColorConvB;
extern FILE *tracePpWeights;

extern FILE *traceRefBufferdPicCtrl;
extern FILE *traceRefBufferdCtrl;
extern FILE *traceRefBufferdPicY;
extern FILE *traceRefBufferdPicCbCr;
extern FILE *traceCustomIdct;
extern FILE *traceBusload;
extern FILE *traceVariance;

extern FILE *traceSliceSizes;
extern FILE *traceRvMvdBits;
extern FILE *traceSegmentation;
extern FILE *traceBoolcoder[9];
extern FILE *traceBcStream[9];
extern FILE *traceBcStreamCtrl[9];
extern FILE *traceProbTables;
extern FILE *traceVp78AboveCbf;
extern FILE *traceVp78MvWeight;

extern FILE *traceRlcUnpacked;
extern FILE *traceAdvPreFetch;
extern FILE *traceHuffmanCtx;
extern FILE *traceHuffman;
extern FILE *traceProb1;
extern FILE *traceProb2;
extern FILE *traceZigZag;
extern FILE *traceNearest;
extern FILE *traceInterFilteredY;
extern FILE *traceInterFilteredCb;
extern FILE *traceInterFilteredCr;
*/

FILE *traceAcdcdOut;
FILE *traceAcdcdOutData;
FILE *traceDecodedMvs;
FILE *traceRlc;
FILE *traceMbCtrl;
FILE *traceMbCtrlHex;
FILE *tracePictureCtrlDec;
FILE *tracePictureCtrlPp;
FILE *tracePictureCtrlDecTiled;
FILE *tracePictureCtrlPpTiled;
FILE *traceMotionVectors;
FILE *traceMotionVectorsHex;
FILE *traceDirModeMvs;
FILE *traceFinalMvs;
FILE *traceDirModeMvsHex;
FILE *traceSeparDc;
FILE *traceSeparDcHex;
FILE *traceRecon;
FILE *traceScdOutData;
FILE *traceBs;
FILE *traceBitPlaneCtrl;
FILE *traceTransdFirstRound;
FILE *traceInterOutData;
FILE *traceOverlapSmooth;
FILE *traceStream;
FILE *traceStreamCtrl;
FILE *traceSwRegAccess;
FILE *traceSwRegAccessTiled;
FILE *traceQTables;
FILE *traceIntra4x4Modes;
FILE *traceIntra4x4ModesHex;
FILE *traceDctOutData;
FILE *traceInterRefY;
FILE *traceInterRefY1;
FILE *traceInterRefCb;
FILE *traceInterRefCb1;
FILE *traceInterRefCr;
FILE *traceInterRefCr1;
FILE *traceOverfill;
FILE *traceOverfill1;
FILE *traceIntraPred;
FILE *traceH264PicIdMap;
FILE *traceResidual;
FILE *traceIQ;
FILE *traceJpegTables;
FILE *traceOut;
FILE *traceOutTiled;
FILE *traceVc1FilteringCtrl;
FILE *traceStreamTxt;
FILE *traceCabacTable[4];
FILE *traceCabacBin;
FILE *traceCabacCtx;
FILE *traceNeighbourMv;
FILE *traceIcSets;
FILE *traceAboveMbMem;
FILE *tracePicOrdCnts;
FILE *traceScalingLists;
FILE *traceRefPicList;
FILE *traceImplicitWeights;
FILE *traceIntraFilteredPels;
FILE *traceMotionVectorsFixed;
FILE *traceDecodedMvsFixed;
FILE *traceDirMvFetch;
FILE *traceScalingOut;
FILE *tracePjpegCoeffs;
FILE *tracePjpegCoeffsHex;
FILE *traceInterOutY;
FILE *traceInterOutY1;
FILE *traceInterOutCb;
FILE *traceInterOutCb1;
FILE *traceInterOutCr;
FILE *traceInterOutCr1;
FILE *traceFilterInternal;
FILE *traceOut2ndCh;
FILE *tracePpAblend1;
FILE *tracePpAblend2;
FILE *tracePpIn;
FILE *tracePpInTiled;
FILE *tracePpInBot;
FILE *tracePpOut;
FILE *tracePpBackground;
FILE *tracePpDeintIn;
FILE *tracePpDeintOutY;
FILE *tracePpDeintOutCb;
FILE *tracePpDeintOutCr;
FILE *tracePpCrop;
FILE *tracePpRotation;
FILE *tracePpScalingKernel;
FILE *tracePpScalingR;
FILE *tracePpScalingG;
FILE *tracePpScalingB;
FILE *tracePpColorConvR;
FILE *tracePpColorConvG;
FILE *tracePpColorConvB;
FILE *tracePpWeights;
FILE *traceRefBufferdPicCtrl;
FILE *traceRefBufferdCtrl;
FILE *traceRefBufferdPicY;
FILE *traceRefBufferdPicCbCr;
FILE *traceCustomIdct;
FILE *traceBusload;
FILE *traceVariance;

FILE *traceSliceSizes;
FILE *traceRvMvdBits;
FILE *traceSegmentation;
FILE *traceBoolcoder[9];
FILE *traceBcStream[9];
FILE *traceBcStreamCtrl[9];
FILE *traceProbTables;
FILE *traceVp78AboveCbf;
FILE *traceVp78MvWeight;

FILE *traceRlcUnpacked;
FILE *traceAdvPreFetch;
FILE *traceHuffmanCtx;
FILE *traceHuffman;
FILE *traceProb1;
FILE *traceProb2;
FILE *traceZigZag;
FILE *traceNearest;
FILE *traceInterFilteredY;
FILE *traceInterFilteredCb;
FILE *traceInterFilteredCr;


static FILE *traceSequenceCtrl;
static FILE *traceSequenceCtrlPp;
FILE *traceDecodingTools;

/*------------------------------------------------------------------------------
 *   Function : openTraceFiles
 * ---------------------------------------------------------------------------*/
u32 openTraceFiles(void)
{

    char traceString[80];
    FILE *traceCfg;
    u32 i;
    char tmpString[80];

    traceCfg = fopen("trace.cfg", "r");
    if(!traceCfg)
    {
        return (1);
    }

    while(fscanf(traceCfg, "%s\n", traceString) != EOF)
    {
        if(!strcmp(traceString, "toplevel") && !traceSequenceCtrl)
        {
            for (i = 0 ; i < 9 ; ++i)
            {
                sprintf(tmpString, "stream_%d.trc", i+1 );
                traceBcStream[i] = fopen(tmpString, "w");
                sprintf(tmpString, "stream_control_%d.trc", i+1 );
                traceBcStreamCtrl[i] = fopen(tmpString, "w");
            }
            traceProbTables = fopen("boolcoder_prob.trc", "w");
            traceSequenceCtrl = fopen("sequence_ctrl.trc", "w");
            tracePictureCtrlDec = fopen("picture_ctrl_dec.trc", "w");
            tracePictureCtrlDecTiled = fopen("picture_ctrl_dec_tiled.trc", "w");
            traceJpegTables = fopen("jpeg_tables.trc", "w");
            traceStream = fopen("stream.trc", "w");
            traceStreamCtrl = fopen("stream_control.trc", "w");
            traceOut = fopen("decoder_out.trc", "w");
            traceOutTiled = fopen("decoder_out_tiled.trc", "w");
            traceSeparDc = fopen("dc_separate_coeffs.trc", "w");
            traceSeparDcHex = fopen("dc_separate_coeffs.hex", "w");
            traceBitPlaneCtrl = fopen("vc1_bitplane_ctrl.trc", "w");
            traceDirModeMvs = fopen("direct_mode_mvs.trc", "w");
            traceDirModeMvsHex = fopen("direct_mode_mvs.hex", "w");
            traceQTables = fopen("qtables.trc", "w");
            traceSwRegAccess = fopen("swreg_accesses.trc", "w");
            traceSwRegAccessTiled = fopen("swreg_accesses_tiled.trc", "w");
            traceBusload = fopen("busload.trc", "w");

            traceCabacTable[0] = fopen("cabac_table_intra.trc", "w");
            traceCabacTable[1] = fopen("cabac_table_inter0.trc", "w");
            traceCabacTable[2] = fopen("cabac_table_inter1.trc", "w");
            traceCabacTable[3] = fopen("cabac_table_inter2.trc", "w");
            traceSegmentation = fopen("segmentation.trc", "w");

            /*required if sw is performing entropy decoding */
            traceMbCtrl = fopen("mbcontrol.trc", "w");
            traceMbCtrlHex = fopen("mbcontrol.hex", "w");
            traceMotionVectors = fopen("motion_vectors.trc", "w");
            traceMotionVectorsHex = fopen("motion_vectors.hex", "w");
            traceIntra4x4Modes = fopen("intra4x4_modes.trc", "w");
            traceIntra4x4ModesHex = fopen("intra4x4_modes.hex", "w");
            traceRlc = fopen("rlc.trc", "w");
            traceRlcUnpacked = fopen("rlc_unpacked.trc", "w");
            traceVp78MvWeight = fopen("vp78_context_weight.trc", "w" );

            traceRefBufferdPicCtrl = fopen("refbufferd_picctrl.trc", "w");
            traceRefBufferdCtrl = fopen("refbufferd_ctrl.trc", "w");

            tracePjpegCoeffs = fopen("prog_jpeg_coefficients.trc", "w");
            tracePjpegCoeffsHex = fopen("prog_jpeg_coefficients.hex", "w");

            tracePicOrdCnts = fopen("picord_counts.trc", "w");
            traceScalingLists = fopen("scaling_lists.trc", "w");

            traceSliceSizes = fopen("slice_sizes.trc", "w");;

            traceRvMvdBits = fopen("mvd_flags.trc", "w");;
            traceHuffman = fopen("huffman.trc", "w");
            traceProb1 = fopen("boolcoder_1.trc", "w");
            traceProb2 = fopen("boolcoder_2.trc", "w");

            traceOut2ndCh = fopen("decoder_out_ch_8pix.trc", "w");

            if((traceSequenceCtrl == NULL) || (tracePictureCtrlDec == NULL) ||
               (traceJpegTables == NULL) || (traceStream == NULL) ||
               (traceStreamCtrl == NULL) || (traceOut == NULL) ||
               /*(traceOutTiled == NULL) || */(traceSeparDc == NULL) ||
               (traceBitPlaneCtrl == NULL) || (traceDirModeMvs == NULL) ||
               (traceQTables == NULL) || (traceSwRegAccess == NULL) ||
               (traceMbCtrl == NULL) || (traceMotionVectors == NULL) ||
               (traceIntra4x4Modes == NULL) || (traceRlc == NULL) ||
               (traceRefBufferdPicCtrl == NULL) ||
               (traceRefBufferdCtrl == NULL) ||
               (tracePjpegCoeffs == NULL) || (tracePjpegCoeffsHex == NULL) ||
               (tracePicOrdCnts == NULL) || (traceScalingLists == NULL) ||
               (traceSliceSizes == NULL) || (traceRvMvdBits == NULL) ||
               (traceOut2ndCh == NULL))

            {
                return (0);
            }
        }

        if(!strcmp(traceString, "all") && !traceAcdcdOut)
        {
            for (i = 0 ; i < 9 ; ++i)
            {
                sprintf(tmpString, "boolcoder_%d_ctx.trc", i+1 );
                traceBoolcoder[i] = fopen(tmpString, "w");
            }
            traceAcdcdOut = fopen("acdcd_out.trc", "w");
            traceAcdcdOutData = fopen("acdcd_outdata.trc", "w");
            traceBs = fopen("bs.trc", "w");
            traceDctOutData = fopen("dct_outdata.trc", "w");
            traceDecodedMvs = fopen("decoded_mvs.trc", "w");
            traceFinalMvs = fopen("final_mvs.trc", "w");
            traceInterRefY = fopen("inter_reference_y.trc", "w");
            traceInterRefY1 = fopen("inter_reference1_y.trc", "w");
            traceInterRefCb = fopen("inter_reference_cb.trc", "w");
            traceInterRefCb1 = fopen("inter_reference1_cb.trc", "w");
            traceInterRefCr = fopen("inter_reference_cr.trc", "w");
            traceInterRefCr1 = fopen("inter_reference1_cr.trc", "w");
            traceOverfill = fopen("inter_overfill.trc", "w");
            traceOverfill1 = fopen("inter_overfill1.trc", "w");
            traceInterOutData = fopen("inter_outdata.trc", "w");
            traceIntraPred = fopen("intra_predicted.trc", "w");
            traceRecon = fopen("reconstructed.trc", "w");
            traceScdOutData = fopen("scd_outdata.trc", "w");
            traceTransdFirstRound = fopen("transd_1rnd.trc", "w");
            traceH264PicIdMap = fopen("h264_picid_map.trc", "w");
            traceResidual = fopen("residual.trc", "w");
            traceIQ = fopen("inverse_quant.trc", "w");
            traceOverlapSmooth = fopen("overlap_smoothed.trc", "w");
            traceVc1FilteringCtrl = fopen("vc1_filtering_ctrl.trc", "w");
            traceStreamTxt = fopen("stream.txt", "w");
            traceNeighbourMv = fopen("neighbour_mvs.trc", "w");
            traceIcSets = fopen("intensity_sets.trc", "w");
            traceAboveMbMem = fopen("above_mb_ctrl_sram.trc", "w");
            traceRefPicList = fopen("ref_pic_list.trc", "w");
            traceImplicitWeights = fopen("implicit_weights.trc", "w");
            traceIntraFilteredPels = fopen("intra_filtered_pxls.trc", "w");
            traceMotionVectorsFixed = fopen("motion_vectors_fixed.trc", "w");
            traceDecodedMvsFixed = fopen("decoded_mvs_fixed.trc", "w");
            traceDirMvFetch = fopen("h264_dirmv_fetch.trc", "w");
            traceScalingOut = fopen("h264_scaling_out.trc", "w");
            traceCustomIdct = fopen("custom_idct.trc", "w");
            traceInterOutY = fopen("inter_interpolated_y.trc", "w");
            traceInterOutY1 = fopen("inter_interpolated1_y.trc", "w");
            traceInterOutCb = fopen("inter_interpolated_cb.trc", "w");
            traceInterOutCb1 = fopen("inter_interpolated1_cb.trc", "w");
            traceInterOutCr = fopen("inter_interpolated_cr.trc", "w");
            traceInterOutCr1 = fopen("inter_interpolated1_cr.trc", "w");
            traceFilterInternal = fopen("rv_filter.trc", "w");
            traceVariance = fopen("inter_variance.trc", "w");
            traceHuffmanCtx = fopen("huffman_ctx.trc", "w");
            traceZigZag = fopen("zigzag.trc", "w");
            traceNearest = fopen("nearest.trc", "w" );
            traceInterFilteredY = fopen("inter_vp6_filtered_ref_y.trc","w");
            traceInterFilteredCb = fopen("inter_vp6_filtered_ref_cb.trc","w");
            traceInterFilteredCr = fopen("inter_vp6_filtered_ref_cr.trc","w");

            traceCabacBin = fopen("cabac_bin.trc", "w");
            traceRefBufferdPicY = fopen("refbufferd_buffil_y.trc", "w");
            traceAdvPreFetch = fopen("advanced_prefetch.trc", "w" );
            traceRefBufferdPicCbCr = fopen("refbufferd_buffil_c.trc", "w");

            traceVp78AboveCbf = fopen("vp78_above_cbf.trc", "w");

            if((traceAcdcdOut == NULL) || (traceAcdcdOutData == NULL) ||
               (traceBs == NULL) || (traceDctOutData == NULL) ||
               (traceDecodedMvs == NULL) || (traceInterRefY == NULL) ||
               (traceInterRefY1 == NULL) || (traceInterRefCb == NULL) ||
               (traceInterRefCb1 == NULL) || (traceInterRefCr == NULL) ||
               (traceInterRefCr1 == NULL) || (traceOverfill == NULL) ||
               (traceOverfill1 == NULL) || (traceInterOutData == NULL) ||
               (traceIntraPred == NULL) || (traceRecon == NULL) ||
               (traceScdOutData == NULL) || (traceTransdFirstRound == NULL) ||
               (traceH264PicIdMap == NULL) || (traceResidual == NULL) ||
               (traceIQ == NULL) || (traceOverlapSmooth == NULL) ||
               (traceVc1FilteringCtrl == NULL) || (traceStreamTxt == NULL) ||
               (traceFinalMvs == NULL) || (traceNeighbourMv == NULL) ||
               (traceIcSets == NULL) || (traceAboveMbMem == NULL) ||
               (traceRefPicList == NULL) || (traceImplicitWeights == NULL) ||
               (traceIntraFilteredPels == NULL) ||
               (traceMotionVectorsFixed == NULL) ||
               (traceCustomIdct == NULL) ||
               (traceCabacBin == NULL) ||
               (traceDecodedMvsFixed == NULL) || (traceDirMvFetch == NULL) ||
               (traceInterOutY == NULL) || (traceInterOutY1 == NULL) ||
               (traceInterOutCb == NULL) || (traceInterOutCb1 == NULL) ||
               (traceInterOutCr == NULL) || (traceInterOutCr1 == NULL) ||
               (traceRefBufferdPicY == NULL) || (traceRefBufferdPicCbCr == NULL) ||
               (traceFilterInternal == NULL))
            {
                return (1);
            }
        }
        if(!strcmp(traceString, "fpga") && !tracePictureCtrlDec)
        {
            tracePictureCtrlDec = fopen("picture_ctrl_dec.trc", "w");
            tracePictureCtrlPp = fopen("picture_ctrl_pp.trc", "w");
            tracePictureCtrlDecTiled = fopen("picture_ctrl_dec_tiled.trc", "w");
            tracePictureCtrlPpTiled = fopen("picture_ctrl_pp_tiled.trc", "w");
            traceBusload = fopen("busload.trc", "w");


            if(tracePictureCtrlDec == NULL)
                return (1);
        }
        if(!strcmp(traceString, "decoding_tools"))
        {
            /* MPEG2 decoding tools trace */
            memset(&trace_mpeg2DecodingTools, 0,
                   sizeof(trace_mpeg2DecodingTools_t));
#if 0
            /* by default MPEG-1 is decoded and MPEG-2 is signaled */
            trace_mpeg2DecodingTools.decodingMode = TRACE_MPEG1;

            /* MPEG-1 is always progressive */
            trace_mpeg2DecodingTools.sequenceType.interlaced = 0;
            trace_mpeg2DecodingTools.sequenceType.progressive = 1;
#endif
            /* H.264 decoding tools trace */
            memset(&trace_h264DecodingTools, 0,
                   sizeof(trace_h264DecodingTools_t));
#if 0
            trace_h264DecodingTools.decodingMode = TRACE_H264;
#endif

            /* JPEG decoding tools trace */
            memset(&trace_jpegDecodingTools, 0,
                   sizeof(trace_jpegDecodingTools_t));
#if 0
            trace_jpegDecodingTools.decodingMode = TRACE_JPEG;
#endif

            /* MPEG4 decoding tools trace */
            memset(&trace_mpeg4DecodingTools, 0,
                   sizeof(trace_mpeg4DecodingTools_t));
#if 0
            trace_mpeg4DecodingTools.decodingMode = TRACE_MPEG4;
#endif

            /* VC1 decoding tools trace */
            memset(&trace_vc1DecodingTools, 0,
                   sizeof(trace_vc1DecodingTools_t));
#if 0
            trace_vc1DecodingTools.decodingMode = TRACE_VC1;
#endif

            memset(&trace_rvDecodingTools, 0,
                   sizeof(trace_rvDecodingTools_t));

            traceDecodingTools = fopen("decoding_tools.trc", "w");

            if(traceDecodingTools == NULL)
                return (1);
        }
        if(!strcmp(traceString, "pp_toplevel"))
        {
            traceSequenceCtrlPp = fopen("sequence_ctrl_pp.trc", "w");
            tracePictureCtrlPp = fopen("picture_ctrl_pp.trc", "w");
            tracePictureCtrlPpTiled = fopen("picture_ctrl_pp_tiled.trc", "w");
            tracePpIn = fopen("pp_in.trc", "w");
            tracePpInTiled = fopen("pp_in_tiled.trc", "w");
            tracePpInBot = fopen("pp_in_bot.trc", "w");
            tracePpOut = fopen("pp_out.trc", "w");
            tracePpBackground = fopen("pp_background.trc", "w");
            tracePpAblend1 = fopen("pp_ablend1.trc", "w");
            tracePpAblend2 = fopen("pp_ablend2.trc", "w");
            if(traceSequenceCtrlPp == NULL ||
               tracePictureCtrlPp == NULL ||
               tracePpIn == NULL ||
               tracePpInBot == NULL ||
               tracePpOut == NULL ||
               tracePpBackground == NULL ||
               tracePpAblend1 == NULL || tracePpAblend2 == NULL)
                return (1);
        }
        if(!strcmp(traceString, "pp_all"))
        {
            tracePpDeintIn = fopen("pp_deint_in.trc", "w");
            tracePpDeintOutY = fopen("pp_deint_out_y.trc", "w");
            tracePpDeintOutCb = fopen("pp_deint_out_cb.trc", "w");
            tracePpDeintOutCr = fopen("pp_deint_out_cr.trc", "w");
            tracePpCrop = fopen("pp_crop.trc", "w");;
            tracePpRotation = fopen("pp_rotation.trc", "w");;
            tracePpScalingKernel = fopen("pp_scaling_kernel.trc", "w");;
            tracePpScalingR = fopen("pp_scaling_r.trc", "w");;
            tracePpScalingG = fopen("pp_scaling_g.trc", "w");;
            tracePpScalingB = fopen("pp_scaling_b.trc", "w");;
            tracePpColorConvR = fopen("pp_colorconv_r.trc", "w");;
            tracePpColorConvG = fopen("pp_colorconv_g.trc", "w");;
            tracePpColorConvB = fopen("pp_colorconv_b.trc", "w");;
            tracePpWeights = fopen("pp_weights.trc", "w");;
            if(tracePpDeintIn == NULL ||
               tracePpDeintOutY == NULL ||
               tracePpDeintOutCb == NULL ||
               tracePpDeintOutCr == NULL ||
               tracePpCrop == NULL ||
               tracePpRotation == NULL ||
               tracePpScalingKernel == NULL ||
               tracePpScalingR == NULL ||
               tracePpScalingG == NULL ||
               tracePpScalingB == NULL ||
               tracePpColorConvR == NULL ||
               tracePpColorConvG == NULL ||
               tracePpColorConvB == NULL || tracePpWeights == NULL)
                return (1);
        }
    }
    return (1);
}

/*------------------------------------------------------------------------------
 *   Function : closeTraceFiles
 * ---------------------------------------------------------------------------*/
void closeTraceFiles(void)
{
    u32 i;
    for( i = 0 ; i < 9 ; ++i )
    {
        if(traceBoolcoder[i])
            fclose(traceBoolcoder[i]);
        if(traceBcStream[i])
            fclose(traceBcStream[i]);
        if(traceBcStreamCtrl[i])
            fclose(traceBcStreamCtrl[i]);
    }
    if(traceAcdcdOut)
        fclose(traceAcdcdOut);
    if(traceAcdcdOutData)
        fclose(traceAcdcdOutData);
    if(traceDecodedMvs)
        fclose(traceDecodedMvs);
    if(traceRlc)
        fclose(traceRlc);
    if(traceRlcUnpacked)
        fclose(traceRlcUnpacked);
    if(traceVp78MvWeight)
        fclose(traceVp78MvWeight);
    if(traceMbCtrl)
        fclose(traceMbCtrl);
    if(tracePictureCtrlDec)
        fclose(tracePictureCtrlDec);
    if(tracePictureCtrlPp)
        fclose(tracePictureCtrlPp);
    if(traceMotionVectors)
        fclose(traceMotionVectors);
    if(traceDirModeMvs)
        fclose(traceDirModeMvs);
    if(traceFinalMvs)
        fclose(traceFinalMvs);
    if(traceSeparDc)
        fclose(traceSeparDc);
    if(traceRecon)
        fclose(traceRecon);
    if(traceScdOutData)
        fclose(traceScdOutData);
    if(traceBs)
        fclose(traceBs);
    if(traceBitPlaneCtrl)
        fclose(traceBitPlaneCtrl);
    if(traceTransdFirstRound)
        fclose(traceTransdFirstRound);
    if(traceInterOutData)
        fclose(traceInterOutData);
    if(traceOverlapSmooth)
        fclose(traceOverlapSmooth);
    if(traceStream)
        fclose(traceStream);
    if(traceStreamCtrl)
        fclose(traceStreamCtrl);
    if(traceSwRegAccess)
        fclose(traceSwRegAccess);
    if(traceQTables)
        fclose(traceQTables);
    if(traceIntra4x4Modes)
        fclose(traceIntra4x4Modes);
    if(traceDctOutData)
        fclose(traceDctOutData);
    if(traceInterRefY)
        fclose(traceInterRefY);
    if(traceInterRefY1)
        fclose(traceInterRefY1);
    if(traceInterRefCb)
        fclose(traceInterRefCb);
    if(traceInterRefCb1)
        fclose(traceInterRefCb1);
    if(traceInterRefCr)
        fclose(traceInterRefCr);
    if(traceInterRefCr1)
        fclose(traceInterRefCr1);
    if(traceOverfill)
        fclose(traceOverfill);
    if(traceOverfill1)
        fclose(traceOverfill1);
    if(traceIntraPred)
        fclose(traceIntraPred);
    if(traceH264PicIdMap)
        fclose(traceH264PicIdMap);
    if(traceResidual)
        fclose(traceResidual);
    if(traceIQ)
        fclose(traceIQ);
    if(traceSequenceCtrl)
        fclose(traceSequenceCtrl);
    if(traceJpegTables)
        fclose(traceJpegTables);
    if(traceOut)
        fclose(traceOut);
    if(traceOutTiled)
        fclose(traceOutTiled);
    if(traceVc1FilteringCtrl)
        fclose(traceVc1FilteringCtrl);
    if(traceDecodingTools)
        fclose(traceDecodingTools);
    if(tracePpWeights)
        fclose(tracePpWeights);
    if(tracePpColorConvR)
        fclose(tracePpColorConvR);
    if(tracePpColorConvG)
        fclose(tracePpColorConvG);
    if(tracePpColorConvB)
        fclose(tracePpColorConvB);
    if(tracePpScalingR)
        fclose(tracePpScalingR);
    if(tracePpScalingG)
        fclose(tracePpScalingG);
    if(tracePpScalingB)
        fclose(tracePpScalingB);
    if(tracePpScalingKernel)
        fclose(tracePpScalingKernel);
    if(tracePpRotation)
        fclose(tracePpRotation);
    if(tracePpCrop)
        fclose(tracePpCrop);
    if(tracePpDeintIn)
        fclose(tracePpDeintIn);
    if(tracePpDeintOutY)
        fclose(tracePpDeintOutY);
    if(tracePpDeintOutCb)
        fclose(tracePpDeintOutCb);
    if(tracePpDeintOutCr)
        fclose(tracePpDeintOutCr);
    if(tracePpBackground)
        fclose(tracePpBackground);
    if(tracePpIn)
        fclose(tracePpIn);
    if(tracePpInBot)
        fclose(tracePpInBot);
    if(tracePpOut)
        fclose(tracePpOut);
    if(tracePpAblend1)
        fclose(tracePpAblend1);
    if(tracePpAblend2)
        fclose(tracePpAblend2);
    if(traceCabacTable[0])
        fclose(traceCabacTable[0]);
    if(traceCabacTable[1])
        fclose(traceCabacTable[1]);
    if(traceCabacTable[2])
        fclose(traceCabacTable[2]);
    if(traceCabacTable[3])
        fclose(traceCabacTable[3]);
    if(traceCabacBin)
        fclose(traceCabacBin);
    if(traceCabacCtx)
        fclose(traceCabacCtx);
    if(traceNeighbourMv)
        fclose(traceNeighbourMv);
    if(traceIcSets)
        fclose(traceIcSets);
    if(traceRefBufferdPicCtrl)
        fclose(traceRefBufferdPicCtrl);
    if(traceRefBufferdCtrl)
        fclose(traceRefBufferdCtrl);
    if(traceRefBufferdPicY)
        fclose(traceRefBufferdPicY);
    if(traceRefBufferdPicCbCr)
        fclose(traceRefBufferdPicCbCr);
    if(traceScalingOut)
        fclose(traceScalingOut);
    if(traceCustomIdct)
        fclose(traceCustomIdct);
    if(traceInterOutY)
        fclose(traceInterOutY);
    if(traceInterOutY1)
        fclose(traceInterOutY1);
    if(traceInterOutCb)
        fclose(traceInterOutCb);
    if(traceInterOutCb1)
        fclose(traceInterOutCb1);
    if(traceInterOutCr)
        fclose(traceInterOutCr);
    if(traceInterOutCr1)
        fclose(traceInterOutCr1);
    if(traceFilterInternal)
        fclose(traceFilterInternal);

    if (traceSliceSizes)
        fclose(traceSliceSizes);
    if (traceScalingLists)
        fclose(traceScalingLists);
    if (tracePicOrdCnts)
        fclose(tracePicOrdCnts);
    if (tracePjpegCoeffs)
        fclose(tracePjpegCoeffs);
    if (tracePjpegCoeffsHex)
        fclose(tracePjpegCoeffsHex);
    if (traceRvMvdBits)
        fclose(traceRvMvdBits);
    if (traceOut2ndCh)
        fclose(traceOut2ndCh);
    if (traceBusload)
        fclose(traceBusload);
    if (traceSegmentation)
        fclose(traceSegmentation);
    if (traceVp78AboveCbf)
        fclose(traceVp78AboveCbf);
}

/*------------------------------------------------------------------------------
 *   Function : trace_SequenceCtrl
 * ---------------------------------------------------------------------------*/
void trace_SequenceCtrl(u32 nmbOfPics, u32 bFrames)
{
    if(!traceSequenceCtrl)
        return;

    fprintf(traceSequenceCtrl, "%d\tAmount of pictures in the sequence\n",
            nmbOfPics);
    fprintf(traceSequenceCtrl, "%d\tSequence includes B frames", bFrames);
}

void trace_SequenceCtrlPp(u32 nmbOfPics)
{
    if(!traceSequenceCtrlPp)
        return;

    fprintf(traceSequenceCtrlPp, "%d\tAmount of pictures in the testcase\n",
            nmbOfPics);

}

static void traceDecodingMode(FILE * trc, trace_decodingMode_e decMode)
{
    switch (decMode)
    {
    case TRACE_H263:
        fprintf(trc, "# H.263\n");
        break;
    case TRACE_H264:
        fprintf(trc, "# H.264\n");
        break;
    case TRACE_MPEG1:
        fprintf(trc, "# MPEG-1\n");
        break;
    case TRACE_MPEG2:
        fprintf(trc, "# MPEG-2\n");
        break;
    case TRACE_MPEG4:
        fprintf(trc, "# MPEG-4\n");
        break;
    case TRACE_VC1:
        fprintf(trc, "# VC-1\n");
        break;
    case TRACE_JPEG:
        fprintf(trc, "# JPEG\n");
        break;
    default:
        fprintf(trc, "# UNKNOWN\n");
        break;
    }
}

static void tracePicCodingType(FILE * trc, char *name,
                               trace_picCodingType_t * picCodingType)
{
    fprintf(trc, "%d    I-%s\n", picCodingType->i_coded, name);
    fprintf(trc, "%d    P-%s\n", picCodingType->p_coded, name);
    fprintf(trc, "%d    B-%s\n", picCodingType->b_coded, name);
}

static void traceSequenceType(FILE * trc, trace_sequenceType_t * sequenceType)
{
    fprintf(trc, "%d    Interlaced sequence type\n", sequenceType->interlaced);
    fprintf(trc, "%d    Progressive sequence type\n",
            sequenceType->progressive);
}

void trace_RefbufferHitrate()
{
    printf("Refbuffer hit percentage: %d%%\n", trace_refBuffHitRate);
}

void trace_MPEG2DecodingTools()
{
    if(!traceDecodingTools)
        return;

    traceDecodingMode(traceDecodingTools,
                      trace_mpeg2DecodingTools.decodingMode);

    tracePicCodingType(traceDecodingTools, "Picture",
                       &trace_mpeg2DecodingTools.picCodingType);

    fprintf(traceDecodingTools, "%d    D-Picture\n",
            trace_mpeg2DecodingTools.d_coded);

    traceSequenceType(traceDecodingTools,
                      &trace_mpeg2DecodingTools.sequenceType);
}

void trace_H264DecodingTools()
{
    if(!traceDecodingTools)
        return;

    traceDecodingMode(traceDecodingTools,
                      trace_h264DecodingTools.decodingMode);

    tracePicCodingType(traceDecodingTools, "Slice",
                       &trace_h264DecodingTools.picCodingType);

    fprintf(traceDecodingTools, "%d    Multiple slices per picture\n",
            trace_h264DecodingTools.multipleSlicesPerPicture);

    fprintf(traceDecodingTools, "%d    Baseline profile\n",
            trace_h264DecodingTools.profileType.baseline);

    fprintf(traceDecodingTools, "%d    Main profile\n",
            trace_h264DecodingTools.profileType.main);

    fprintf(traceDecodingTools, "%d    High profile\n",
            trace_h264DecodingTools.profileType.high);

    fprintf(traceDecodingTools, "%d    I-PCM MB type\n",
            trace_h264DecodingTools.ipcm);

    fprintf(traceDecodingTools, "%d    Direct mode MB type\n",
            trace_h264DecodingTools.directMode);

    fprintf(traceDecodingTools, "%d    Monochrome\n",
            trace_h264DecodingTools.monochrome);

    fprintf(traceDecodingTools, "%d    8x8 transform\n",
            trace_h264DecodingTools.transform8x8);

    fprintf(traceDecodingTools, "%d    8x8 intra prediction\n",
            trace_h264DecodingTools.intraPrediction8x8);

    fprintf(traceDecodingTools, "%d    Scaling list present\n",
            trace_h264DecodingTools.scalingListPresent);

    fprintf(traceDecodingTools, "%d    Scaling list present (SPS)\n",
            trace_h264DecodingTools.scalingMatrixPresentType.seq);

    fprintf(traceDecodingTools, "%d    Scaling list present (PPS)\n",
            trace_h264DecodingTools.scalingMatrixPresentType.pic);

    fprintf(traceDecodingTools,
            "%d    Weighted prediction, explicit, P slice\n",
            trace_h264DecodingTools.weightedPredictionType.explicit);

    fprintf(traceDecodingTools,
            "%d    Weighted prediction, explicit, B slice\n",
            trace_h264DecodingTools.weightedPredictionType.explicit_b);

    fprintf(traceDecodingTools, "%d    Weighted prediction, implicit\n",
            trace_h264DecodingTools.weightedPredictionType.implicit);

    fprintf(traceDecodingTools, "%d    In-loop filter control present\n",
            trace_h264DecodingTools.loopFilter);

    fprintf(traceDecodingTools,
            "%d    Disable Loop filter in slice header\n",
            trace_h264DecodingTools.loopFilterDis);

    fprintf(traceDecodingTools, "%d    CAVLC entropy coding\n",
            trace_h264DecodingTools.entropyCoding.cavlc);

    fprintf(traceDecodingTools, "%d    CABAC entropy coding\n",
            trace_h264DecodingTools.entropyCoding.cabac);

    if(trace_h264DecodingTools.seqType.ilaced == 0)
        trace_h264DecodingTools.seqType.prog = 1;
    fprintf(traceDecodingTools, "%d    Progressive sequence type\n",
            trace_h264DecodingTools.seqType.prog);

    fprintf(traceDecodingTools, "%d    Interlace sequence type\n",
            trace_h264DecodingTools.seqType.ilaced);

    fprintf(traceDecodingTools, "%d    PicAff\n",
            trace_h264DecodingTools.interlaceType.picAff);

    fprintf(traceDecodingTools, "%d    MbAff\n",
            trace_h264DecodingTools.interlaceType.mbAff);

    fprintf(traceDecodingTools, "%d    NAL unit stream\n",
            trace_h264DecodingTools.streamMode.nalUnitStrm);

    fprintf(traceDecodingTools, "%d    Byte stream\n",
            trace_h264DecodingTools.streamMode.byteStrm);

    fprintf(traceDecodingTools, "%d    More than 1 slice groups (FMO)\n",
            trace_h264DecodingTools.sliceGroups);

    fprintf(traceDecodingTools, "%d    Arbitrary slice order (ASO)\n",
            trace_h264DecodingTools.arbitrarySliceOrder);

    fprintf(traceDecodingTools, "%d    Redundant slices\n",
            trace_h264DecodingTools.redundantSlices);

    fprintf(traceDecodingTools, "%d    Image cropping\n",
            trace_h264DecodingTools.imageCropping);

    fprintf(traceDecodingTools, "%d    Error stream\n",
            trace_h264DecodingTools.error);
}

void trace_JpegDecodingTools()
{
    if(!traceDecodingTools)
        return;

    traceDecodingMode(traceDecodingTools,
                      trace_jpegDecodingTools.decodingMode);

    fprintf(traceDecodingTools, "%d     YCbCr 4:2:0 sampling\n",
            trace_jpegDecodingTools.sampling_4_2_0);

    fprintf(traceDecodingTools, "%d     YCbCr 4:2:2 sampling\n",
            trace_jpegDecodingTools.sampling_4_2_2);

    fprintf(traceDecodingTools, "%d     YCbCr 4:0:0 sampling\n",
            trace_jpegDecodingTools.sampling_4_0_0);

    fprintf(traceDecodingTools, "%d     YCbCr 4:4:0 sampling\n",
            trace_jpegDecodingTools.sampling_4_4_0);

    fprintf(traceDecodingTools, "%d     YCbCr 4:1:1 sampling\n",
            trace_jpegDecodingTools.sampling_4_1_1);

    fprintf(traceDecodingTools, "%d     YCbCr 4:4:4 sampling\n",
            trace_jpegDecodingTools.sampling_4_4_4);

    fprintf(traceDecodingTools, "%d     JPEG compressed thumbnail\n",
            trace_jpegDecodingTools.thumbnail);

    fprintf(traceDecodingTools, "%d     Progressive decoding\n",
            trace_jpegDecodingTools.progressive);
}

void trace_MPEG4DecodingTools()
{
    if(!traceDecodingTools)
        return;

    traceDecodingMode(traceDecodingTools,
                      trace_mpeg4DecodingTools.decodingMode);

    tracePicCodingType(traceDecodingTools, "VOP",
                       &trace_mpeg4DecodingTools.picCodingType);

    traceSequenceType(traceDecodingTools,
                      &trace_mpeg4DecodingTools.sequenceType);

    fprintf(traceDecodingTools, "%d    4-MV\n",
            trace_mpeg4DecodingTools.fourMv);

    fprintf(traceDecodingTools, "%d    AC/DC prediction\n",
            trace_mpeg4DecodingTools.acPred);

    fprintf(traceDecodingTools, "%d    Data partition\n",
            trace_mpeg4DecodingTools.dataPartition);

    fprintf(traceDecodingTools,
            "%d    Slice resynchronization / Video packets\n",
            trace_mpeg4DecodingTools.resyncMarker);

    fprintf(traceDecodingTools, "%d    Reversible VLC\n",
            trace_mpeg4DecodingTools.reversibleVlc);

    fprintf(traceDecodingTools, "%d    Header extension code\n",
            trace_mpeg4DecodingTools.hdrExtensionCode);

    fprintf(traceDecodingTools, "%d    Quantisation Method 1\n",
            trace_mpeg4DecodingTools.qMethod1);

    fprintf(traceDecodingTools, "%d    Quantisation Method 2\n",
            trace_mpeg4DecodingTools.qMethod2);

    fprintf(traceDecodingTools, "%d    Half-pel motion compensation\n",
            trace_mpeg4DecodingTools.halfPel);

    fprintf(traceDecodingTools, "%d    Quarter-pel motion compensation\n",
            trace_mpeg4DecodingTools.quarterPel);

    fprintf(traceDecodingTools, "%d    Short video header\n",
            trace_mpeg4DecodingTools.shortVideo);
}

void trace_VC1DecodingTools()
{
    if(!traceDecodingTools)
        return;

    traceDecodingMode(traceDecodingTools,
                      trace_vc1DecodingTools.decodingMode);

    tracePicCodingType(traceDecodingTools, "frames",
                       &trace_vc1DecodingTools.picCodingType);

    traceSequenceType(traceDecodingTools,
                      &trace_vc1DecodingTools.sequenceType);

    fprintf(traceDecodingTools, "%d    Variable-sized transform\n",
            trace_vc1DecodingTools.vsTransform);

    fprintf(traceDecodingTools, "%d    Overlapped transform\n",
            trace_vc1DecodingTools.overlapTransform);

    fprintf(traceDecodingTools, "%d    4 motion vectors per macroblock\n",
            trace_vc1DecodingTools.fourMv);

    fprintf(traceDecodingTools,
            "%d    Quarter-pixel motion compensation Y\n",
            trace_vc1DecodingTools.qpelLuma);

    fprintf(traceDecodingTools,
            "%d    Quarter-pixel motion compensation C\n",
            trace_vc1DecodingTools.qpelChroma);

    fprintf(traceDecodingTools, "%d    Range reduction\n",
            trace_vc1DecodingTools.rangeReduction);

    fprintf(traceDecodingTools, "%d    Intensity compensation\n",
            trace_vc1DecodingTools.intensityCompensation);

    fprintf(traceDecodingTools, "%d    Multi-resolution\n",
            trace_vc1DecodingTools.multiResolution);

    fprintf(traceDecodingTools, "%d    Adaptive macroblock quantization\n",
            trace_vc1DecodingTools.adaptiveMBlockQuant);

    fprintf(traceDecodingTools, "%d    In-loop deblock filtering\n",
            trace_vc1DecodingTools.loopFilter);

    fprintf(traceDecodingTools, "%d    Range mapping\n",
            trace_vc1DecodingTools.rangeMapping);

    fprintf(traceDecodingTools, "%d    Extended motion vectors\n",
            trace_vc1DecodingTools.extendedMV);
}

void trace_RvDecodingTools()
{
    if(!traceDecodingTools)
        return;

    traceDecodingMode(traceDecodingTools,
                      trace_rvDecodingTools.decodingMode);

    tracePicCodingType(traceDecodingTools, "frames",
                       &trace_rvDecodingTools.picCodingType);

}

