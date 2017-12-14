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
#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>
#include <string.h>
#include "basetype.h"

/* common tools */
typedef enum
{
    TRACE_H263 = 0,
    TRACE_H264 = 1,
    TRACE_MPEG1 = 2,
    TRACE_MPEG2 = 3,
    TRACE_MPEG4 = 4,
    TRACE_VC1 = 5,
    TRACE_JPEG = 6,
    TRACE_RV = 8
} trace_decodingMode_e;

typedef struct
{
    u8 i_coded; /* intra coded */
    u8 p_coded; /* predictive coded */
    u8 b_coded; /* bidirectionally predictive coded */
} trace_picCodingType_t;

typedef struct
{
    u8 interlaced;
    u8 progressive;
} trace_sequenceType_t;

/* mpeg-1 & mpeg-2 tools */
typedef struct
{
    trace_decodingMode_e decodingMode;  /* MPEG-1 or MPEG-2 */
    trace_picCodingType_t picCodingType;
    u8 d_coded; /* MPEG-1 specific */
    trace_sequenceType_t sequenceType;
} trace_mpeg2DecodingTools_t;

/* h.264 tools */
typedef struct
{
    u8 nalUnitStrm;
    u8 byteStrm;
} trace_h264StreamMode_t;

typedef struct
{
    u8 cavlc;
    u8 cabac;
} trace_h264EntropyCoding_t;

typedef struct
{
    u8 baseline;
    u8 main;
    u8 high;
} trace_h264ProfileType_t;

typedef struct
{
    u8 pic;
    u8 seq;
} trace_h264ScalingMatrixPresentType_t;

typedef struct
{
    u8 explicit;
    u8 explicit_b;
    u8 implicit;
} trace_h264WeightedPredictionType_t;

typedef struct
{
    u8 picAff;
    u8 mbAff;
} trace_h264InterlaceType_t;

typedef struct
{
    u8 ilaced;
    u8 prog;
} trace_h264sequenceType_t;

typedef struct
{
    trace_decodingMode_e decodingMode;
    trace_picCodingType_t picCodingType;
    trace_h264StreamMode_t streamMode;
    trace_h264EntropyCoding_t entropyCoding;
    trace_h264ProfileType_t profileType;
    trace_h264ScalingMatrixPresentType_t scalingMatrixPresentType;
    trace_h264WeightedPredictionType_t weightedPredictionType;
    trace_h264InterlaceType_t interlaceType;
    trace_h264sequenceType_t seqType;
    u8 sliceGroups; /* more thant 1 slice groups */
    u8 arbitrarySliceOrder;
    u8 redundantSlices;
    u8 weightedPrediction;
    u8 imageCropping;
    u8 monochrome;
    u8 scalingListPresent;
    u8 transform8x8;
    u8 loopFilter;
    u8 loopFilterDis;
    u8 error;
    u8 intraPrediction8x8;
    u8 ipcm;
    u8 directMode;
    u8 multipleSlicesPerPicture;
} trace_h264DecodingTools_t;

/* jpeg tools */
typedef struct
{
    trace_decodingMode_e decodingMode;
    u8 sampling_4_2_0;
    u8 sampling_4_2_2;
    u8 sampling_4_0_0;
    u8 sampling_4_4_0;
    u8 sampling_4_1_1;    
    u8 sampling_4_4_4;    
    u8 thumbnail;
    u8 progressive;
} trace_jpegDecodingTools_t;

/* mpeg-4 & h.263 tools */
typedef struct
{
    trace_decodingMode_e decodingMode;
    trace_picCodingType_t picCodingType;
    trace_sequenceType_t sequenceType;
    u32 fourMv;
    u32 acPred;
    u32 dataPartition;
    u32 resyncMarker;
    u32 reversibleVlc;
    u32 hdrExtensionCode;
    u32 qMethod1;
    u32 qMethod2;
    u32 halfPel;
    u32 quarterPel;
    u32 shortVideo;
} trace_mpeg4DecodingTools_t;

/* vc-1 tools */

#if 0
/* storage for temp trace values */
typedef struct
{
    u32 quantizer;
} trace_Storage_t;
#endif

typedef struct
{
    trace_decodingMode_e decodingMode;
    trace_picCodingType_t picCodingType;
    trace_sequenceType_t sequenceType;
    u32 vsTransform;
    u32 overlapTransform;
    u32 fourMv;
    u32 qpelLuma;
    u32 qpelChroma;
    u32 rangeReduction;
    u32 intensityCompensation;
    u32 multiResolution;
    u32 adaptiveMBlockQuant;
    u32 loopFilter;
    u32 rangeMapping;
    u32 extendedMV;
#if 0
    trace_Storage_t storage;
#endif
} trace_vc1DecodingTools_t;

typedef struct
{
    trace_decodingMode_e decodingMode;
    trace_picCodingType_t picCodingType;
} trace_rvDecodingTools_t;

/* global variables for tool tracing */
extern trace_mpeg2DecodingTools_t trace_mpeg2DecodingTools;
extern trace_h264DecodingTools_t trace_h264DecodingTools;
extern trace_jpegDecodingTools_t trace_jpegDecodingTools;
extern trace_mpeg4DecodingTools_t trace_mpeg4DecodingTools;
extern trace_vc1DecodingTools_t trace_vc1DecodingTools;
extern trace_rvDecodingTools_t trace_rvDecodingTools;

/* tracing functions */
u32 openTraceFiles(void);
void closeTraceFiles(void);
void trace_SequenceCtrl(u32 nmbOfPics, u32 bFrames);
void trace_MPEG2DecodingTools();
void trace_H264DecodingTools();
void trace_JpegDecodingTools();
void trace_MPEG4DecodingTools();
void trace_VC1DecodingTools();
void trace_RvDecodingTools();
void trace_RefbufferHitrate();

#endif /* TRACE_H */
