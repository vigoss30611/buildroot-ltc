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
--  Description : VLC decoding functionality
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_vlc.c,v $
--  $Revision: 1.6 $
--  $Date: 2008/12/05 10:00:42 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1hwd_vlc.h"
#include "vc1hwd_picture_layer.h"
#include "vc1hwd_util.h"

/*------------------------------------------------------------------------------
    External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define VLC_ESCAPE  (-2)

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeIntCompField

        Functional description:
            Decodes intensity compensation information for fields

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            intCompField_e  Intensity compensation field

------------------------------------------------------------------------------*/
intCompField_e vc1hwdDecodeIntCompField( strmData_t * const strmData )
{

/* Variables */
    intCompField_e icf;

/* Code */
    ASSERT( strmData );

    if (vc1hwdGetBits(strmData, 1))     /* 1b */
        icf = IC_BOTH_FIELDS;
    else
    {
        if (vc1hwdGetBits(strmData, 1)) /* 01b */
            icf = IC_BOTTOM_FIELD;
        else                            /* 00b */
            icf = IC_TOP_FIELD;
    }

    return icf;
}

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeDmvRange

        Functional description:
            Decodes differential MV range codewords

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            dmv             Extended differential MV range

------------------------------------------------------------------------------*/
u16x vc1hwdDecodeDmvRange( strmData_t * const strmData )
{

/* Variables */
    u16x dmv = 0;

/* Code */
    ASSERT( strmData );

    if (vc1hwdGetBits(strmData, 1))
    {
        if (vc1hwdGetBits(strmData, 1))
        {
            if (vc1hwdGetBits(strmData, 1)) /* 111b */
                dmv = 3;
            else                            /* 110b */
                dmv = 2; 
        }
        else /* 10b */
            dmv = 1; 
    }
    else /* 0b */
        dmv = 0;

    return dmv;
}

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeRefDist

        Functional description:
            Decodes reference distance syntax element.

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            u16x            Reference distance

------------------------------------------------------------------------------*/
u16x vc1hwdDecodeRefDist( strmData_t * const strmData )
{

/* Variables */
    u16x tmp;
    u16x len = 0;
    u16x refDist = 0;

/* Code */
    ASSERT( strmData );
    tmp = vc1hwdShowBits(strmData, 2);
    if (tmp == 0) {
        len = 2;
        refDist = 0;
    } else if (tmp == 1) {
        len = 2;
        refDist = 1;
    } else if (tmp == 2) {
        len = 2;
        refDist = 2;
    } else {
        refDist = 3;
        (void)vc1hwdFlushBits(strmData, 2);
        while (vc1hwdGetBits(strmData, 1)) {
            refDist++;
        } 
        return refDist;
    }
    (void)vc1hwdFlushBits(strmData, len);

    return refDist;
}

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeCondOver

        Functional description:
            Decods conditional overlapping VLC

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            condOver        conditional overlap info

------------------------------------------------------------------------------*/
u16x vc1hwdDecodeCondOver( strmData_t * const strmData )
{

/* Variables */
    u16x condOver;

/* Code */
    ASSERT( strmData );

    if (vc1hwdGetBits(strmData, 1))
    {
        if (vc1hwdGetBits(strmData, 1)) /* 11b */
            condOver = 3;
        else                            /* 10b */
            condOver = 2;
    }
    else                                /* 0b */
        condOver = 0;
        
    return condOver;
}
/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeFcm

        Functional description:
            Decodes frame coding mode VLC

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            fcm_e           Frame coding mode

------------------------------------------------------------------------------*/
fcm_e vc1hwdDecodeFcm( strmData_t * const strmData )
{

/* Variables */
    fcm_e fcm;

/* Code */
    ASSERT( strmData );

    if (vc1hwdGetBits(strmData, 1))
    {
        if (vc1hwdGetBits(strmData, 1)) /* 11b */
            fcm = FIELD_INTERLACE;
        else                            /* 10b */
            fcm = FRAME_INTERLACE;
    }
    else                                /* 0b */
        fcm = PROGRESSIVE;
        
    return fcm;
}

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodePtype

        Functional description:
            Decode element PTYPE from bitstream.

        Inputs:
            strmData        Pointer to stream data descriptor.
            advanced        HANTRO_TRUE    - Advanced Profile
                            HANTRO_FALSE   - Simple and Main Profiles
            maxBframes      Maximum n:o of B frames (relevant for M.P. only)

        Outputs:
            picType_e       Picture type.

------------------------------------------------------------------------------*/
picType_e vc1hwdDecodePtype( strmData_t * const strmData, u32 advanced,
                             u16x maxBframes)
{

/* Variables */

    u16x tmp;
    u16x len;
    picType_e   ptype;

/* Code */

    ASSERT( strmData );
    if (advanced)
    {
        tmp = (u16x)vc1hwdShowBits(strmData, 3);
        if (tmp < 4) {  /* 0b */
            len = 1;
            ptype = PTYPE_P;
        } else if (tmp < 6) {   /* 10b */
            len = 2;
            ptype = PTYPE_B;
        } else if (tmp < 7) {   /* 110b */
            len = 3;
            ptype = PTYPE_I;
        } else {
            len = 0;
            (void)vc1hwdFlushBits(strmData, 3);
            if (vc1hwdGetBits(strmData, 1)) /* 1111b */
                ptype = PTYPE_Skip;
            else                            /* 1110b */
                ptype = PTYPE_BI;
        }
        (void)vc1hwdFlushBits(strmData, len);
    }
    else
    {
        if( maxBframes > 0 ) {
            if( vc1hwdGetBits( strmData, 1 )) /* 1 */
                ptype = PTYPE_P;
            else if ( vc1hwdGetBits( strmData, 1 )) /* 01 */
                ptype = PTYPE_I;
            else /* 00 */
                ptype = PTYPE_B;
        } else
            ptype = vc1hwdGetBits( strmData, 1 ) == 1 ? PTYPE_P : PTYPE_I;
    }

    return ptype;
}

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeTransAcFrm

        Functional description:
            Decode element TRANSACFRM from bitstream.

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            Coding set index, range [0, 2].

------------------------------------------------------------------------------*/
u16x vc1hwdDecodeTransAcFrm( strmData_t * const strmData )
{

/* Variables */

    u16x i;

/* Code */

    ASSERT( strmData );

    if( 0 == vc1hwdGetBits( strmData, 1 )) /* 0 */
        i = 0;
    else if ( vc1hwdGetBits( strmData, 1 )) /* 11 */
        i = 2;
    else /* 00 */
        i = 1;
    return i;
}

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeMvMode

        Functional description:
            Decode elements MVMODE and MVMODE2 (if present) from bitstream.

        Inputs:
            strmData        Pointer to stream data descriptor.
            bPic            HANTRO_TRUE    - B picture
                            HANTRO_FALSE   - P picture
            pquant          PQUANT

        Outputs:
            Motion vector coding mode.

------------------------------------------------------------------------------*/
mvmode_e vc1hwdDecodeMvMode( strmData_t * const strmData, const u32 bPic,
                             const u16x pquant, u32 *pIntComp )
{

/* Variables */

    mvmode_e    mvmode;
    static const mvmode_e mvmTab0[4] = {/* PQUANT > 12 */
        MVMODE_1MV_HALFPEL_LINEAR, MVMODE_1MV,
        MVMODE_1MV_HALFPEL, MVMODE_MIXEDMV
    };
    static const mvmode_e mvmTab1[4] = {/* PQUANT <= 12 */
        MVMODE_1MV, MVMODE_MIXEDMV,
        MVMODE_1MV_HALFPEL, MVMODE_1MV_HALFPEL_LINEAR
    };
    const mvmode_e * mvmTab;
    u16x        bits, len;

/* Code */

    ASSERT( strmData );

    if( bPic ) { /* B picture */
        *pIntComp = HANTRO_FALSE;
        if( vc1hwdGetBits( strmData, 1 ) == 1 )
            mvmode = MVMODE_1MV;
        else
            mvmode = MVMODE_1MV_HALFPEL_LINEAR;
    }
    else
    { /* P picture */

        /* Choose LUT */
        if(pquant > 12) mvmTab = mvmTab0;
        else mvmTab = mvmTab1;

        *pIntComp = HANTRO_FALSE;

        bits = (u16x) vc1hwdShowBits( strmData, 7 );
        if( bits >= 64 ) { /* 1 */
            mvmode = mvmTab[0];
            len = 1;
        } else if ( bits >= 32 ) { /* 01 */
            mvmode = mvmTab[1];
            len = 2;
        } else if ( bits >= 16 ) { /* 001 */
            mvmode = mvmTab[2];
            len = 3;
        } else if ( bits >= 8 ) { /* 0001 */
            *pIntComp = HANTRO_TRUE;
            bits -= 8;
            if( bits >= 4 ) { /* 00011 */
                mvmode = mvmTab[0];
                len = 5;
            } else if ( bits >= 2 ) { /* 000101 */
                mvmode = mvmTab[1];
                len = 6;
            } else if ( bits == 1 ) { /* 0001001 */
                mvmode = mvmTab[2];
                len = 7;
            } else { /* 0001000 */
                mvmode = mvmTab[3];
                len = 7;
            }
        } else { /* 0000 */
            mvmode = mvmTab[3];
            len = 4;
        }
        (void)vc1hwdFlushBits( strmData, len );
    }

    return mvmode;
}
/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeMvModeB

        Functional description:
            Decode elements MVMODE for interlace B picture.

        Inputs:
            strmData        Pointer to stream data descriptor.
            pquant          PQUANT

        Outputs:
            Motion vector coding mode.

------------------------------------------------------------------------------*/
mvmode_e vc1hwdDecodeMvModeB( strmData_t * const strmData, const u16x pquant)
{

/* Variables */

    mvmode_e    mvmode;
    u16x tmp;
    u16x len = 0;

/* Code */
    ASSERT(strmData);

    tmp = (u16x)vc1hwdShowBits( strmData, 3 );
    if (pquant > 12)
    {
        if (tmp == 0) {         /* 000b */
            len = 3;
            mvmode = MVMODE_MIXEDMV;
        } else if (tmp == 1) {  /* 001b */
            len = 3;
            mvmode = MVMODE_1MV_HALFPEL;
        } else if (tmp < 4) {   /* 01b */
            len = 2;
            mvmode = MVMODE_1MV;
        } else {                /* 1b */
            len = 1;
            mvmode = MVMODE_1MV_HALFPEL_LINEAR;
        }
    }
    else
    {
        if (tmp == 0) {         /* 000b */
            len = 3;
            mvmode = MVMODE_1MV_HALFPEL_LINEAR;
        } else if (tmp == 1) {  /* 001b */
            len = 3;
            mvmode = MVMODE_1MV_HALFPEL;
        } else if (tmp < 4) {   /* 01b */
            len = 2;
            mvmode = MVMODE_MIXEDMV;
        } else {                /* 1b */
            len = 1;
            mvmode = MVMODE_1MV;
        }
    }
    (void)vc1hwdFlushBits(strmData, len);

    return mvmode;
}
/*------------------------------------------------------------------------------

   Function: vc1swdDecodeMvRange

        Functional description:
            Decode element MVRANGE from bitstream.

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            Return value    MV range
            0               [-64, 63.f] x [-32, 31.f]
            1               [-128, 127.f] x [-64, 63.f]
            2               [-512, 511.f] x [-128, 127.f]
            3               [-1024, 1023.f] x [-256, 255.f]

------------------------------------------------------------------------------*/
u16x vc1hwdDecodeMvRange( strmData_t * const strmData )
{

/* Variables */

    u16x tmp;
    u16x len;
    u16x range;

/* Code */

    ASSERT( strmData );

    tmp = (u16x)vc1hwdShowBits( strmData, 3 );
    if( tmp < 4 ) { /* 0 */
        len = 1;
        range = 0;
    } else if ( tmp  < 6 ) {
        len = 2;
        range = 1;
    } else {
        len = 3;
        range = tmp - 4;
    }
    (void)vc1hwdFlushBits( strmData, len );
    return range;
}

/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeBfraction

        Functional description:
            Decode element BFRACTION from bitstream. Note! For Main Profile
            the return value may be BFRACT_PTYPE_BI in which case the picture
            type should be updated to BI.

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            bfract_e value corresponding to decoded BFRACTION.

------------------------------------------------------------------------------*/
bfract_e vc1hwdDecodeBfraction( strmData_t * const strmData,
    i16x * pScaleFactor )
{

/* Variables */

    bfract_e    bf = BFRACT_SMPTE_RESERVED;
    u16x        tmp;
    u16x        tmp2;
    static const bfract_e bfTab[7] = {
        BFRACT_1_2, BFRACT_1_3, BFRACT_2_3, BFRACT_1_4,
        BFRACT_3_4, BFRACT_1_5, BFRACT_2_5
    };
    static const bfract_e bfTab2[16] = {
        BFRACT_3_5, BFRACT_4_5, BFRACT_1_6, BFRACT_5_6,
        BFRACT_1_7, BFRACT_2_7, BFRACT_3_7, BFRACT_4_7,
        BFRACT_5_7, BFRACT_6_7, BFRACT_1_8, BFRACT_3_8,
        BFRACT_5_8, BFRACT_7_8, BFRACT_SMPTE_RESERVED,
        BFRACT_PTYPE_BI
    };
    static const u16x sfShort[7] = { 128, 85, 170, 64, 192, 51, 102 };
    static const u16x sfLong[16] = {
        153, 204, 43, 215, 37, 74, 111, 148, 185, 222, 32, 96, 160, 224,
        (u16x)-1, (u16x)-1 };

/* Code */

    ASSERT( strmData );
    ASSERT( pScaleFactor );

    tmp2 = tmp = vc1hwdGetBits( strmData, 3 );
    if(tmp == END_OF_STREAM)
        return bf;


    if( tmp == 7 ) { /* long code word */
        tmp = vc1hwdGetBits( strmData, 4 );

        if(tmp == END_OF_STREAM)
            return bf;

        tmp2 = ( tmp2 << 4 ) | tmp;
        bf = bfTab2[tmp];
        *pScaleFactor = sfLong[tmp];
    } else { /* short code word */
        bf = bfTab[tmp];
        *pScaleFactor = sfShort[tmp];
    }

    DPRINT(("ScaleFactor=%d\n", *pScaleFactor));

    return bf;
}
/*------------------------------------------------------------------------------

   Function: vc1hwdDecodeVopDquant

        Functional description:
            Decode element VOPDQUANT from bitstream.

        Inputs:
            strmData        Pointer to stream data descriptor.
            dquant          DQUANT sequence header element, range [0, 2].
            pLayer          Pointer to picture layer descriptor. This function
                            updates quantization attributes into the descriptor.

        Outputs:

------------------------------------------------------------------------------*/
void vc1hwdDecodeVopDquant( strmData_t * const strmData, const u16x dquant,
                            pictureLayer_t * const pLayer )
{

/* Variables */

/* The enums that cause this warning are safe for this bit-operation */
/*lint -save -e655 */

    u16x    tmp, tmp2;
    u16x    len;
    static const dqEdge_e dqEdgeDouble[4] = {
        DQEDGE_LEFT | DQEDGE_TOP,
        DQEDGE_RIGHT | DQEDGE_TOP,
        DQEDGE_RIGHT | DQEDGE_BOTTOM,
        DQEDGE_LEFT | DQEDGE_BOTTOM,
    };

/* Code */

    ASSERT( strmData );
    ASSERT( dquant == 0 || dquant == 1 || dquant == 2 );
    ASSERT( pLayer );

    DPRINT(("VOPDQUANT (DQUANT=%d)\n",dquant));

    if( dquant == 0 )
        return;
    if( dquant == 1 ) {

        tmp = vc1hwdShowBits( strmData, 13 );
        if( tmp >= 4096 ) { /* DQUANTFRM == 1 */
            tmp2 = (tmp >> 10) & 3;
            if( tmp2 == 0 ) { /* All four Edges */
                pLayer->dqProfile = DQPROFILE_ALL_FOUR;
                pLayer->dqEdges = DQEDGE_TOP | DQEDGE_LEFT |
                                  DQEDGE_BOTTOM | DQEDGE_RIGHT;
                len = 3;
                tmp2 = (tmp >> 2) & 255;
                DPRINT(("DQPROFILE_ALL_FOUR\n"));


            } else if( tmp2 == 1 ) { /* Double Edges */
                tmp2 = (tmp >> 8) & 3;
                pLayer->dqEdges = dqEdgeDouble[tmp2];
                pLayer->dqProfile = DQPROFILE_DOUBLE_EDGES;
                len = 5;
                tmp2 = tmp & 255;
                DPRINT(("DQPROFILE_DOUBLE_EDGES (%d)\n", pLayer->dqEdges));


            } else if( tmp2 == 2 ) { /* Single Edges */
                tmp2 = (tmp >> 8) & 3;
                pLayer->dqProfile = DQPROFILE_SINGLE_EDGES;
                pLayer->dqEdges = (dqEdge_e)(1<<tmp2);
                len = 5;
                tmp2 = tmp & 255;
                DPRINT(("DQPROFILE_SINGLE_EDGES (%d)\n", pLayer->dqEdges));


            } else { /* All Macroblocks */
                pLayer->dqProfile = DQPROFILE_ALL_MACROBLOCKS;
                pLayer->dqbiLevel = ( tmp & 512 ) >> 9;
                len = 4;
                tmp2 = (tmp >> 1) & 255;
                DPRINT(("DQPROFILE_ALL_MACROBLOCKS\n"));
                DPRINT(("DQBILEVEL=%d\n", pLayer->dqbiLevel));


            }
            /* read PQDIFF/ABSPQ */
            if( !( pLayer->dqProfile == DQPROFILE_ALL_MACROBLOCKS &&
                   pLayer->dqbiLevel == 0 ) ) {
                if( tmp2 >= 224 ) { /* 111 */
                    len += 8;
                    pLayer->altPquant = tmp2 & 31;
                } else {
                    len += 3;
                    pLayer->altPquant = pLayer->pquant + ( tmp2 >> 5 ) + 1;
                }
            }
            DPRINT(("DQUANTINFRM=1\n"));
            DPRINT(("ALTPQUANT=%d\n",pLayer->altPquant));
            pLayer->dquantInFrame = HANTRO_TRUE;
            /* make sure that altPquant is in range [1,31], just force by
             * clipping, no errors */
            if (pLayer->altPquant < 1)
                pLayer->altPquant = 1;
            else if (pLayer->altPquant > 31)
                pLayer->altPquant = 31;
        } else {
            pLayer->dquantInFrame = HANTRO_FALSE;
            DPRINT(("DQUANTINFRM=0\n"));

            len = 1;
        }
        (void)vc1hwdFlushBits( strmData, len );
    }
    else { /* dquant = 2 */

        pLayer->dquantInFrame = HANTRO_TRUE;
        pLayer->dqProfile = DQPROFILE_ALL_FOUR;
        pLayer->dqEdges = DQEDGE_TOP | DQEDGE_LEFT |
                          DQEDGE_BOTTOM | DQEDGE_RIGHT;
        /* read PQDIFF/ABSPQ */
        tmp = vc1hwdGetBits( strmData, 3 );

        if( tmp == 7 ) {
            pLayer->altPquant = vc1hwdGetBits( strmData, 5 );

        } else {
            pLayer->altPquant = pLayer->pquant + tmp + 1;
        }
        DPRINT(("DQPROFILE_ALL_FOUR\n"));
        DPRINT(("DQUANTINFRM=1\n"));
        DPRINT(("ALTPQUANT=%d\n",pLayer->altPquant));

        /* make sure that altPquant is in range [1,31], just force by
         * clipping, no errors */
        if (pLayer->altPquant < 1)
            pLayer->altPquant = 1;
        else if (pLayer->altPquant > 31)
            pLayer->altPquant = 31;
    }


/*lint -restore */
}

