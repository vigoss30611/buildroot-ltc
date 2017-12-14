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
-  $RCSfile: vp6decodemode.h,v $
-  $Revision: 1.1 $
-  $Date: 2008/04/14 10:13:24 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6DECODEMODE_H__
#define __VP6DECODEMODE_H__

#include "basetype.h"
#include "vp6dec.h"

/****************************************************************************
*  Module statics
****************************************************************************/        
#define MODETYPES       3
#define MODEVECTORS     16
#define PROBVECTORXMIT  174
#define PROBIDEALXMIT   254

/****************************************************************************
*  Typedefs
****************************************************************************/        
typedef struct _modeContext
{
	u8 left;
	u8 above;
	u8 last;
} MODE_CONTEXT;

typedef struct _htorp
{
    unsigned char selector : 1;   // 1 bit selector 0->ptr, 1->token
    unsigned char value : 7;
} torp;

typedef struct _hnode
{
	torp left;
	torp right;
} HNODE;

typedef enum _MODETYPE 
{
	MACROBLOCK,
	NONEAREST_MACROBLOCK,
	NONEAR_MACROBLOCK,
	BLOCK
} MODETYPE;

typedef struct LineEq
{
    i32	M;
    i32	C;
} LINE_EQ;

/****************************************************************************
*  Exports
****************************************************************************/        
extern const u8 VP6HWModeVq[MODETYPES][MODEVECTORS][MAX_MODES*2];
extern const u8 VP6HWBaselineXmittedProbs[4][2][MAX_MODES];

extern const u8 VP6HWDcUpdateProbs[2][MAX_ENTROPY_TOKENS-1];
extern const u8 VP6HWAcUpdateProbs[PREC_CASES][2][VP6HWAC_BANDS][MAX_ENTROPY_TOKENS-1];
extern const u8 VP6HWPrevTokenIndex[MAX_ENTROPY_TOKENS];

extern const u8 VP6HW_ZrlUpdateProbs[ZRL_BANDS][ZERO_RUN_PROB_CASES];
extern const u8 VP6HW_ZeroRunProbDefaults[ZRL_BANDS][ZERO_RUN_PROB_CASES];


extern const LINE_EQ VP6HWDcNodeEqs[CONTEXT_NODES][DC_TOKEN_CONTEXTS];


typedef enum
{
    CODE_INTER_NO_MV        = 0x0,      // INTER prediction, (0,0) motion vector implied.
    CODE_INTRA              = 0x1,      // INTRA i.e. no prediction.
    CODE_INTER_PLUS_MV      = 0x2,      // INTER prediction, non zero motion vector.
    CODE_INTER_NEAREST_MV   = 0x3,      // Use Last Motion vector
    CODE_INTER_NEAR_MV      = 0x4,      // Prior last motion vector
    CODE_USING_GOLDEN       = 0x5,      // 'Golden frame' prediction (no MV).
    CODE_GOLDEN_MV          = 0x6,      // 'Golden frame' prediction plus MV.
    CODE_INTER_FOURMV       = 0x7,      // Inter prediction 4MV per macro block.
    CODE_GOLD_NEAREST_MV    = 0x8,      // Use Last Motion vector
    CODE_GOLD_NEAR_MV       = 0x9,      // Prior last motion vector
    DO_NOT_CODE             = 0x10       // Fake Mode
} CODING_MODE;


/****************************************************************************

*  Function Prototypes
****************************************************************************/
void VP6HWDecodeModeProbs(PB_INSTANCE *pbi);
void VP6HWBuildModeTree ( PB_INSTANCE *pbi );

#endif /* __VP6DECODEMODE_H__ */
