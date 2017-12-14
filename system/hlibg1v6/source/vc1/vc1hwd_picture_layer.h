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
--  Description : Interface for picture layer decoding
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_picture_layer.h,v $
--  $Revision: 1.2 $
--  $Date: 2007/06/28 12:24:02 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_PICTURE_LAYER_H
#define VC1HWD_PICTURE_LAYER_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1hwd_util.h"
#include "vc1hwd_stream.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/* Picture type */
typedef enum {
    PTYPE_I = 0,
    PTYPE_P = 1,
    PTYPE_B = 2,
    PTYPE_BI = 3,
    PTYPE_Skip
} picType_e;

/* Field type */
typedef enum {
    FTYPE_I = 0,
    FTYPE_P = 1,
    FTYPE_B = 2,
    FTYPE_BI = 3
} fieldType_e;

/* RESPIC syntax element (Main Profile) */
typedef enum {
    RESPIC_FULL_FULL = 0,
    RESPIC_HALF_FULL = 1,
    RESPIC_FULL_HALF = 2,
    RESPIC_HALF_HALF = 3
} resPic_e;

/* DQPROFILE syntax element */
typedef enum {
    DQPROFILE_N_A,
    DQPROFILE_ALL_FOUR,
    DQPROFILE_DOUBLE_EDGES,
    DQPROFILE_SINGLE_EDGES,
    DQPROFILE_ALL_MACROBLOCKS
} dqProfile_e;

/* DQSBEDGE and DQDBEDGE */
typedef enum {
    DQEDGE_LEFT     = 1,
    DQEDGE_TOP      = 2,
    DQEDGE_RIGHT    = 4,
    DQEDGE_BOTTOM   = 8
} dqEdge_e;

/* BFRACTION syntax element */
typedef enum {
    BFRACT_1_2,
    BFRACT_1_3,
    BFRACT_2_3,
    BFRACT_1_4,
    BFRACT_3_4,
    BFRACT_1_5,
    BFRACT_2_5,
    BFRACT_3_5,
    BFRACT_4_5,
    BFRACT_1_6,
    BFRACT_5_6,
    BFRACT_1_7,
    BFRACT_2_7,
    BFRACT_3_7,
    BFRACT_4_7,
    BFRACT_5_7,
    BFRACT_6_7,
    BFRACT_1_8,
    BFRACT_3_8,
    BFRACT_5_8,
    BFRACT_7_8,
    BFRACT_SMPTE_RESERVED,
    BFRACT_PTYPE_BI
} bfract_e;

/* Supported transform types */
typedef enum {
    TT_8x8 = 0,
    TT_8x4 = 1,
    TT_4x8 = 2,
    TT_4x4 = 3
} transformType_e;

/* MVMODE syntax element */
typedef enum {
    MVMODE_1MV_HALFPEL_LINEAR,
    MVMODE_1MV,
    MVMODE_1MV_HALFPEL,
    MVMODE_MIXEDMV,
    MVMODE_INVALID
} mvmode_e;

/* BMVTYPE syntax element */
typedef enum {
    BMV_BACKWARD,
    BMV_FORWARD,
    BMV_INTERPOLATED,
    BMV_DIRECT, /* used in mv prediction function */
    BMV_P_PICTURE
} bmvType_e;

/* FCM syntax element */
typedef enum {
    PROGRESSIVE = 0,
    FRAME_INTERLACE = 1,
    FIELD_INTERLACE = 2
} fcm_e;

/* Pan scan info */
typedef struct {
    u32 hOffset;
    u32 vOffset;
    u32 width;
    u32 height;
} psw_t;

/* FPTYPE */
typedef enum {
    FP_I_I = 0,
    FP_I_P = 1,
    FP_P_I = 2,
    FP_P_P = 3,
    FP_B_B = 4,
    FP_B_BI = 5,
    FP_BI_B = 6,
    PF_BI_BI = 7
} fpType_e;

/* INTCOMPFIELD */
typedef enum {
    IC_BOTH_FIELDS = 0,
    IC_TOP_FIELD = 1,
    IC_BOTTOM_FIELD = 2,
    IC_NONE
} intCompField_e;

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

/* Descriptor for the picture layer data */
typedef struct pictureLayer {
    picType_e picType;          /* Picture type */

    u16x pqIndex;
    u16x pquant;                /* Picture level quantization parameters */
    u16x halfQp;
    u32 uniformQuantizer;

    resPic_e    resPic;         /* Progressive picture resolution code [0,3] */
    u16x bufferFullness;

    u32 interpFrm;
    u16x frameCount;

    mvmode_e mvmode;            /* Motion vector coding mode for frame */
    mvmode_e mvmode2;

    u16x rawMask;               /* Raw mode mask; specifies which bitplanes are
                                 * coded with raw coding mode as well as invert
                                 * element status. */

    u16x mvTableIndex;
    u16x cbpTableIndex;
    u16x acCodingSetIndexY;
    u16x acCodingSetIndexCbCr;
    u16x intraTransformDcIndex;

    u32 mbLevelTransformTypeFlag;  /* Transform type signaled in MB level */

    transformType_e ttFrm;      /* Frame level transform type */

    /* Main profile stuff */
    u32 intensityCompensation;
    u16x mvRange;               /* Motion vector range [0,3] */
    u16x dquantInFrame;         /* Implied from decoding VOPDQUANT */
    bfract_e bfraction;         /* Temporal placement for B pictures */
    i16x scaleFactor;

    u16x altPquant;
    dqProfile_e dqProfile;
    dqEdge_e dqEdges;           /* Edges to be quantized with ALTPQUANT
                                 * instead of PQUANT */

    u16x dqbiLevel;             /* Used only if dqProfile==All
                                 * 0  mb may take any quantization step
                                 * 1  only PQUANT and ALTPQUANT allowed */
    i32 iShift;
    i32 iScale;
    u32 rangeRedFrm;

    u32 topField;   /* 1 TOP, 0 BOTTOM */
    u32 isFF;       /* is first field */

    /* advance profile stuff */
    fcm_e fcm;      /* frame coding mode */
    u32 tfcntr;     /* Temporal reference frame count */
    u32 tff;        /* Top field first TFF=1 -> top, TFF=0 -> bottom */
    u32 rff;        /* Repeat first field */
    u32 rptfrm;     /* Repeat Frame Count; Number of frames to repeat (0-3) */
    u32 psPresent;  /* Pan Scan Flag */
    psw_t psw;      /* Pan scan window information */
    
    u32 uvSamp;     /* subsampling of color-difference: progressive/interlace */
    u32 postProc;   /* may be used in display process */
    u32 condOver;   /* conditional overlap flag: 0b, 10b, 11b */

    u32 dmvRange;   /* extended differential mv range [0,3] */
    u32 mvSwitch;   /* If 0, only 1mv for MB else 1,2 or 4 mvs for MB */
    u32 mbModeTab;  /* macroblock mode code table */
    u32 mvbpTableIndex2;  /* 2-mv block pattern table */
    u32 mvbpTableIndex4; /* 4-mv block pattern table */

    fpType_e fieldPicType; /* Field picture type */
    u32 refDist;    /* P reference distance */
    u32 numRef;     /* 0 = one reference field, 1 = two reference fields */
    u32 refField;   /* 0 = closest, 1 = second most recent */
    intCompField_e intCompField; /* fields to intensity compensate */
    i32 iShift2;    /* LUMSHIFT2 (shall be applied to bottom field) */
    i32 iScale2;    /* LUMSCALE2 (shall be applied to bottom field) */

    u32 picHeaderBits;  /* number of bits in picture layer header */
    u32 fieldHeaderBits; /* same for field header */

} pictureLayer_t;


/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

struct swStrmStorage;
u16x vc1hwdDecodePictureLayer( struct swStrmStorage *pStorage, 
                               strmData_t *pStrmData );

u16x vc1hwdDecodeFieldLayer( struct swStrmStorage *pStorage, 
                             strmData_t *pStrmData,
                             u16x isFirstField );

u16x vc1hwdDecodePictureLayerAP(struct swStrmStorage *pStorage, 
                                 strmData_t *pStrmData );


#endif /* #ifndef VC1HWD_PICTURE_LAYER_H */

