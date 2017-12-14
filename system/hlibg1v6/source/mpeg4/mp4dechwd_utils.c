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
--  Abstract : Stream decoding utilities
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_utils.c,v $
--  $Date: 2010/04/15 12:42:03 $
--  $Revision: 1.11 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_GetBits
        5.2     StrmDec_ShowBits
        5.3     StrmDec_ShowBitsAligned
        5.4     StrmDec_FlushBits
        5.5     StrmDec_NextStartCode
        5.6     StrmDec_FindSync
        5.7     StrmDec_GetStartCode
        5.8     StrmDec_NumBits
        5.9     StrmDec_UnFlushBits

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_utils.h"
#include "mp4dechwd_videopacket.h"

#include "mp4debug.h"
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

enum
{
    SV_MARKER_MASK = 0x1FFFF,
    SV_END_MASK = 0x3FFFFF,
    SECOND_BYTE_ZERO_MASK = 0x00FF0000
};

/* to check first 23 bits */
#define START_CODE_MASK      0xFFFFFE00
/* to check first 16 bits */
#define RESYNC_MASK          0xFFFF0000

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

static const u32 StuffingTable[8] =
    { 0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F };

/* table containing start code id for each possible start code suffix. Start
 * code suffixes that are either reserved or non-applicable have been marked
 * with 0 */
static const u32 startCodeTable[256] = {
    SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START,
    SC_VO_START,
    SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START,
    SC_VO_START,
    SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START,
    SC_VO_START,
    SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START,
    SC_VO_START,
    SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START, SC_VO_START,
    SC_VO_START,
    SC_VO_START, SC_VO_START,
    SC_VOL_START, SC_VOL_START, SC_VOL_START, SC_VOL_START, SC_VOL_START,
    SC_VOL_START, SC_VOL_START, SC_VOL_START, SC_VOL_START, SC_VOL_START,
    SC_VOL_START, SC_VOL_START, SC_VOL_START, SC_VOL_START, SC_VOL_START,
    SC_VOL_START,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    SC_VOS_START, SC_VOS_END, SC_UD_START, SC_GVOP_START, 0, SC_VISO_START,
    SC_VOP_START,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_GetBits

        Purpose: read and remove bits from mpeg-4 input stream

        Input:
            Pointer to DecContainer structure
            Number of bits to read [0,31]

        Output:
            u32 containing bits read from stream. Value END_OF_STREAM
            reserved to indicate failure.

------------------------------------------------------------------------------*/

u32 StrmDec_GetBits(DecContainer * pDecContainer, u32 numBits)
{

    u32 out, tmp;

    ASSERT(numBits < 32);

    /*out = StrmDec_ShowBits(pDecContainer, numBits); */
    SHOWBITS32(out);
    out = out >> (32 - numBits);

    if((pDecContainer->StrmDesc.strmBuffReadBits + numBits) >
       (8 * pDecContainer->StrmDesc.strmBuffSize))
    {
        pDecContainer->StrmDesc.strmBuffReadBits =
            8 * pDecContainer->StrmDesc.strmBuffSize;
        pDecContainer->StrmDesc.bitPosInWord = 0;
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmDesc.pStrmBuffStart +
            pDecContainer->StrmDesc.strmBuffSize;
        return (END_OF_STREAM);
    }
    else
    {
        pDecContainer->StrmDesc.strmBuffReadBits += numBits;
        tmp = pDecContainer->StrmDesc.bitPosInWord + numBits;
        pDecContainer->StrmDesc.pStrmCurrPos += tmp >> 3;
        pDecContainer->StrmDesc.bitPosInWord = tmp & 0x7;
        return (out);
    }
    /*if (StrmDec_FlushBits(pDecContainer, numBits) == HANTRO_OK)
     * {
     * return(out);
     * }
     * else
     * {
     * return(END_OF_STREAM);
     * } */

}

/*------------------------------------------------------------------------------

   5.2  Function name: StrmDec_ShowBits

        Purpose: read bits from mpeg-4 input stream. Bits are located right
        aligned in the 32-bit output word. In case stream ends,
        function fills the word with zeros. For example, numBits = 18 and
        there are 7 bits left in the stream buffer -> return
            00000000000000xxxxxxx00000000000,
        where 'x's represent actual bits read from buffer.

        Input:
            Pointer to DecContainer structure
                -uses but does not update StrmDesc
            Number of bits to read [0,32]

        Output:
            u32 containing bits read from stream

------------------------------------------------------------------------------*/

u32 StrmDec_ShowBits(DecContainer * pDecContainer, u32 numBits)
{

    u32 i;
    i32 bits, shift;
    u32 out;
    const u8 *pstrm = pDecContainer->StrmDesc.pStrmCurrPos;

    ASSERT(numBits <= 32);

    /* bits left in the buffer */
    bits = (i32) pDecContainer->StrmDesc.strmBuffSize * 8 -
        (i32) pDecContainer->StrmDesc.strmBuffReadBits;

    if(!numBits || !bits)
    {
        return (0);
    }

    /* at least 32-bits in the buffer -> get 32 bits and drop extra bits out */
    if(bits >= 32)
    {
        out = ((u32) pstrm[0] << 24) | ((u32) pstrm[1] << 16) |
            ((u32) pstrm[2] << 8) | ((u32) pstrm[3]);
        if(pDecContainer->StrmDesc.bitPosInWord)
        {
            out <<= pDecContainer->StrmDesc.bitPosInWord;
            out |=
                (u32) pstrm[4] >> (8 -
                                      pDecContainer->StrmDesc.bitPosInWord);
        }
    }
    else
    {
        shift = 24 + pDecContainer->StrmDesc.bitPosInWord;
        out = (u32) pstrm[0] << shift;
        bits -= 8 - pDecContainer->StrmDesc.bitPosInWord;
        i = 1;
        while(bits > 0)
        {
            shift -= 8;
            out |= (u32) pstrm[i] << shift;
            bits -= 8;
            i++;
        }
    }

    return (out >> (32 - numBits));

}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_ShowBitsAligned

        Purpose: read bits from mpeg-4 input stream at byte aligned position
        given as parameter (offset from current stream position).

        Input:
            Pointer to DecContainer structure
                -uses but does not update StrmDesc
            Number of bits to read [1,32]
            Byte offset from current stream position

        Output:
            u32 containing bits read from stream

------------------------------------------------------------------------------*/

u32 StrmDec_ShowBitsAligned(DecContainer * pDecContainer, u32 numBits,
                            u32 byteOffset)
{

    u32 i;
    u32 out;
    u32 bytes, shift;
    const u8 *pstrm = pDecContainer->StrmDesc.pStrmCurrPos + byteOffset;

    ASSERT(numBits);
    ASSERT(numBits <= 32);
    out = 0;

    /* at least four bytes available starting byteOffset bytes ahead */
    if((pDecContainer->StrmDesc.strmBuffSize >= (4 + byteOffset)) &&
       ((pDecContainer->StrmDesc.strmBuffReadBits >> 3) <=
        (pDecContainer->StrmDesc.strmBuffSize - byteOffset - 4)))
    {
        out = ((u32) pstrm[0] << 24) | ((u32) pstrm[1] << 16) |
            ((u32) pstrm[2] << 8) | ((u32) pstrm[3]);
        out >>= (32 - numBits);
    }
    else
    {
        bytes = pDecContainer->StrmDesc.strmBuffSize -
            (pDecContainer->StrmDesc.strmBuffReadBits >> 3);
        if(bytes > byteOffset)
        {
            bytes -= byteOffset;
        }
        else
        {
            bytes = 0;
        }

        shift = 24;
        i = 0;
        while(bytes)
        {
            out |= (u32) pstrm[i] << shift;
            i++;
            shift -= 8;
            bytes--;
        }

        out >>= (32 - numBits);
    }

    return (out);

}

/*------------------------------------------------------------------------------

   5.4  Function name: StrmDec_FlushBits

        Purpose: removes bits from mpeg-4 input stream

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
            Number of bits to remove [0,2^32)

        Output:
            HANTRO_OK if operation was successful
            END_OF_STREAM if stream buffer run empty

------------------------------------------------------------------------------*/

u32 StrmDec_FlushBits(DecContainer * pDecContainer, u32 numBits)
{

    u32 tmp;

    if((pDecContainer->StrmDesc.strmBuffReadBits + numBits) >
       (8 * pDecContainer->StrmDesc.strmBuffSize))
    {
        pDecContainer->StrmDesc.strmBuffReadBits =
            8 * pDecContainer->StrmDesc.strmBuffSize;
        pDecContainer->StrmDesc.bitPosInWord = 0;
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmDesc.pStrmBuffStart +
            pDecContainer->StrmDesc.strmBuffSize;
        return (END_OF_STREAM);
    }
    else
    {
        pDecContainer->StrmDesc.strmBuffReadBits += numBits;
        tmp = pDecContainer->StrmDesc.bitPosInWord + numBits;
        pDecContainer->StrmDesc.pStrmCurrPos += tmp >> 3;
        pDecContainer->StrmDesc.bitPosInWord = tmp & 0x7;
        return (HANTRO_OK);
    }

}

/*------------------------------------------------------------------------------

   5.5  Function name: StrmDec_GetStuffing

        Purpose: removes mpeg-4 stuffing from input stream

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc

        Output:
            HANTRO_OK if operation was successful
            HANTRO_NOK if error in stuffing (i.e. not 0111...)
            END_OF_STREAM

------------------------------------------------------------------------------*/

u32 StrmDec_GetStuffing(DecContainer * pDecContainer)
{

    u32 stuffing;
    u32 stuffingLength = 8 - pDecContainer->StrmDesc.bitPosInWord;

    ASSERT(stuffingLength && (stuffingLength <= 8));

#ifdef HANTRO_PEDANTIC_MODE

    stuffing = StrmDec_GetBits(pDecContainer, stuffingLength);

    if(stuffing == END_OF_STREAM)
    {
        return (END_OF_STREAM);
    }

    if(stuffing != StuffingTable[stuffingLength - 1])
        return (HANTRO_NOK);
    else
        return (HANTRO_OK);

#else

    stuffing = StrmDec_ShowBits(pDecContainer, stuffingLength);
    if(stuffing != StuffingTable[stuffingLength - 1])
        return (HANTRO_OK);

    stuffing = StrmDec_GetBits(pDecContainer, stuffingLength);
    return (HANTRO_OK);

#endif

}

/*------------------------------------------------------------------------------

   5.5  Function name: StrmDec_CheckStuffing

        Purpose: checks mpeg-4 stuffing from input stream

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc

        Output:
            HANTRO_OK if operation was successful
            HANTRO_NOK if error in stuffing (i.e. not 0111...)

------------------------------------------------------------------------------*/

u32 StrmDec_CheckStuffing(DecContainer * pDecContainer)
{

    u32 length;

    ASSERT(pDecContainer);
    ASSERT(pDecContainer->StrmDesc.bitPosInWord < 8);

    length = 8 - pDecContainer->StrmDesc.bitPosInWord;
    if ( StrmDec_ShowBits(pDecContainer, length) == StuffingTable[length-1] )
        return (HANTRO_OK);
    else
        return (HANTRO_NOK);

}

/*------------------------------------------------------------------------------

   5.6  Function name: StrmDec_FindSync

        Purpose: searches next syncronization point in mpeg-4 stream. Takes
        into account StrmDecStorage.status as follows:
            STATE_NOT_READY -> only accept SC_VOS_START, SC_VO_START,
                SC_VISO_START and SC_VOL_START codes (to get initial headers
                for decoding). Also accept SC_SV_START as short video vops
                provide all information needed in decoding.

        In addition to above mentioned condition short video markers are
        accepted only in case shortVideo in DecStrmStrorage is HANTRO_TRUE.

        Function starts searching sync four bytes back from current stream
        position. However, if this would result starting before last found sync,
        search is started one byte ahead fo last found sync. Search is always
        started at byte aligned position.

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
                -uses and updates StrmStorage

        Output:
            Start code value in case one was found, END_OF_STREAM if stream
            end was encountered before sync was found. SC_ERROR if start code
            prefix correct but suffix does not match or more than 23
            successive zeros found.

------------------------------------------------------------------------------*/

u32 StrmDec_FindSync(DecContainer * pDecContainer)
{

    u32 i, tmp;
    u32 code;
    u32 status;
    u32 codeLength;
    u32 mask;
    u32 markerLength;

    if(pDecContainer->StrmStorage.validVopHeader == HANTRO_TRUE)
    {
        markerLength = pDecContainer->StrmStorage.resyncMarkerLength;
    }
    else
    {
        markerLength = 0;
    }

    /* go back 4 bytes and to beginning of byte (or to beginning of stream)
     * before starting search */
    if(pDecContainer->StrmDesc.strmBuffReadBits > 39)
    {
        pDecContainer->StrmDesc.strmBuffReadBits -= 32;
        /* drop 3 lsbs (i.e. make read bits next lowest multiple of byte) */
        pDecContainer->StrmDesc.strmBuffReadBits &= 0xFFFFFFF8;
        pDecContainer->StrmDesc.bitPosInWord = 0;
        pDecContainer->StrmDesc.pStrmCurrPos -= 4;
    }
    else
    {
        pDecContainer->StrmDesc.strmBuffReadBits = 0;
        pDecContainer->StrmDesc.bitPosInWord = 0;
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmDesc.pStrmBuffStart;
    }

    /* beyond last sync point -> go to last sync point */
    if(pDecContainer->StrmDesc.pStrmCurrPos <
       pDecContainer->StrmStorage.pLastSync)
    {
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmStorage.pLastSync;
        pDecContainer->StrmDesc.strmBuffReadBits = 8 *
            (pDecContainer->StrmDesc.pStrmCurrPos -
             pDecContainer->StrmDesc.pStrmBuffStart);
    }

    code = 0;
    codeLength = 0;

    while(!code)
    {
        tmp = StrmDec_ShowBits(pDecContainer, 32);
        /* search if two first bytes equal to zero (all start codes have at
         * least 16 zeros in the beginning) or short video case (not
         * necessarily byte aligned start codes) */
        if(tmp && (!(tmp & RESYNC_MASK) ||
                   (pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE)))
        {
            if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE)
            {
                if(((tmp & 0xFFFFFFE0) == SC_VO_START) ||
                   ((tmp & 0xFFFFFFF0) == SC_VOL_START) ||
                   (tmp == SC_VISO_START) || (tmp == SC_VOS_START))
                {
                    code = startCodeTable[tmp & 0xFF];
                    codeLength = 32;
                }
                else if((tmp >> 10) == SC_SV_START)
                {
                    code = tmp >> 10;
                    codeLength = 22;
                }
            }
            else if(pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE)
            {
                if(((tmp >> 8) == 0x01) && startCodeTable[tmp & 0xFF])
                {
                    code = startCodeTable[tmp & 0xFF];
                    /* do not accept user data start code here */
                    if(code == SC_UD_START)
                    {
                        code = 0;
                    }
                    codeLength = 32;
                }
                /* either start code prefix or 24 zeros -> start code error */
                else if(pDecContainer->rlcMode && (tmp & 0xFFFFFE00) == 0)
                {
                    /* consider VOP start code lost */
                    codeLength = 32;
                    pDecContainer->StrmStorage.startCodeLoss = HANTRO_TRUE;
                }
                else if(pDecContainer->rlcMode &&
                        !pDecContainer->Hdrs.resyncMarkerDisable)
                {
                    /* resync marker length known? */
                    if(markerLength)
                    {
                        if((tmp >> (32 - markerLength)) == 0x01)
                        {
                            code = SC_RESYNC;
                            codeLength = markerLength;
                        }
                    }
                    /* try all possible lengths [17,23] if third byte contains
                     * at least one '1' */
                    else
                    {
                        markerLength = 17;
                        mask = 0x8000;
                        while(!(tmp & mask))
                        {
                            mask >>= 1;
                            markerLength++;
                        }
                        code = SC_RESYNC;
                        codeLength = markerLength;
                        pDecContainer->StrmStorage.resyncMarkerLength =
                            markerLength;
                    }
                }
            }
            else    /* short video */
            {
                if(((tmp & 0xFFFFFFE0) == SC_VO_START) ||
                   (tmp == SC_VOS_START) ||
                   (tmp == SC_VOS_END) || (tmp == SC_VISO_START))
                {
                    code = startCodeTable[tmp & 0xFF];
                    codeLength = 32;
                }
                else if((tmp >> 10) == SC_SV_START)
                {
                    code = tmp >> 10;
                    codeLength = 22;
                }
                /* check short video resync and end code at each possible bit
                 * position [0,7] if second byte of tmp is 0 */
                else if(pDecContainer->rlcMode &&
                        !(tmp & SECOND_BYTE_ZERO_MASK))
                {
                    for(i = 15; i >= 8; i--)
                    {
                        if(((tmp >> i) & SV_MARKER_MASK) == SC_RESYNC)
                        {
                            /* Not short video end marker */
                            if(((tmp >> (i - 5)) & SV_END_MASK) != SC_SV_END)
                            {
                                code = SC_RESYNC;
                                codeLength = 32 - i;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if(codeLength)
        {
            status = StrmDec_FlushBits(pDecContainer, codeLength);
            pDecContainer->StrmStorage.pLastSync =
                pDecContainer->StrmDesc.pStrmCurrPos;
            codeLength = 0;
        }
        else
        {
            status = StrmDec_FlushBits(pDecContainer, 8);
        }
        if(status == END_OF_STREAM)
        {
            return (END_OF_STREAM);
        }
    }

    return (code);

}

/*------------------------------------------------------------------------------

   5.7  Function name: StrmDec_GetStartCode

        Purpose: tries to read next start code from input stream. Start code
        is not searched but it is assumed that if there is start code it is at
        current stream position.

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
                -uses and updates StrmStorage

        Output:
            Start code value if found, SC_NOT_FOUND if no start code detected
            and SC_ERROR if start code prefix correct but suffix does not match
            or more than 23 successive zeros found. END_OF_STREAM if stream
            ended.

------------------------------------------------------------------------------*/

u32 StrmDec_GetStartCode(DecContainer * pDecContainer)
{

    u32 tmp;
    u32 markerLength;
    u32 codeLength = 0;
    u32 startCode = SC_NOT_FOUND;

    markerLength = pDecContainer->StrmStorage.resyncMarkerLength;

    /* reset at next byte boundary */
    if(!pDecContainer->rlcMode)
    {
        if( pDecContainer->StrmDesc.bitPosInWord )
        {
            tmp = StrmDec_FlushBits( pDecContainer, 
                8-pDecContainer->StrmDesc.bitPosInWord );
            if( tmp == END_OF_STREAM )
                return END_OF_STREAM;
        }
    }

  
  //if the buffer contains start code following some unused user data,we should skip the user data 
  //add by franklin
    tmp = StrmDec_ShowBits(pDecContainer, 32);
    while( tmp <= 0x1 && 
           pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE )
    {
        tmp = StrmDec_FlushBits( pDecContainer, 8 );
        if( tmp == END_OF_STREAM )
            return (END_OF_STREAM);
        tmp = StrmDec_ShowBits(pDecContainer, 32);
    }

     if (pDecContainer->StrmStorage.sorensonSpark)
    {
        if ((tmp >> 11) == SC_SORENSON_START)
        {
            codeLength = 21;
            startCode = tmp >> 11;
        }
    }
    /* only check VisualObjectSequence, VisualObject, VisualObjectLayer and
     * short video start codes if decoder is not ready yet */
    else if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE)
    {
        if(((tmp & 0xFFFFFFE0) == SC_VO_START) ||
           ((tmp & 0xFFFFFFF0) == SC_VOL_START) ||
           (tmp == SC_VOS_START) ||
           (tmp == SC_VOS_END) || (tmp == SC_VISO_START))
        {
            codeLength = 32;
            startCode = startCodeTable[tmp & 0xFF];
        }
        else if((tmp >> 10) == SC_SV_START)
        {
            codeLength = 22;
            startCode = tmp >> 10;
        }
    }

    else if((pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE) &&
            ((tmp >> 8) == 0x01) && startCodeTable[tmp & 0xFF])
    {
        codeLength = 32;
        startCode = startCodeTable[tmp & 0xFF];
    }
    else if((pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE) &&
            (((tmp & 0xFFFFFFE0) == SC_VO_START) || (tmp == SC_VOS_START) ||
             (tmp == SC_VOS_END) || (tmp == SC_VISO_START)))
    {
        codeLength = 32;
        startCode = startCodeTable[tmp & 0xFF];
    }
    else if((pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE) &&
            (((tmp >> 10) == SC_SV_START) || ((tmp >> 10) == SC_SV_END)))
    {
        codeLength = 22;
        startCode = tmp >> 10;
    }
    /* accept resync marker only if in RLC mode */
    else if(pDecContainer->rlcMode &&
           (tmp >> (32 - markerLength)) == SC_RESYNC)
    {
        codeLength = markerLength;
        startCode = SC_RESYNC;
    }
    else if((tmp & 0xFFFFFE00) == 0)
    {
        codeLength = 32;
        startCode = SC_ERROR;
        /* consider VOP start code lost */
        pDecContainer->StrmStorage.startCodeLoss = HANTRO_TRUE;
    }

    /* Hack for H.263 streams -- SC_SV_END is not necessarily stuffed with
     * zeroes. */
    if((pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE) &&
        startCode == SC_NOT_FOUND )
    {
        u32 i;
        u32 stuffingBits = 0;
        /* There might be 1...7 bits of zeroes required to form an end
         * marker. */
        for( i = 1 ; i <= 7 ; ++i )
        {
            if( tmp >> (10+i) == SC_SV_END )
            {
                stuffingBits = i;
                break;
            }
        }

        if( stuffingBits )
        {
            /* Get last byte from bitstream. */
            tmp = pDecContainer->StrmStorage.lastPacketByte; 

            if( (tmp & (( 1 << stuffingBits ) - 1 )) == 0x0 )
            {
                startCode = SC_SV_END;
                codeLength = 22 - stuffingBits;
                pDecContainer->StrmStorage.lastPacketByte = 0xFF;
            }
        }

    }

    if(startCode != SC_NOT_FOUND)
    {
        tmp = StrmDec_FlushBits(pDecContainer, codeLength);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        pDecContainer->StrmStorage.pLastSync =
            pDecContainer->StrmDesc.pStrmCurrPos;
    }

    return (startCode);

}

/*------------------------------------------------------------------------------

   5.8  Function name: StrmDec_NumBits

        Purpose: computes number of bits needed to represent value given as
        argument

        Input:
            u32 value [0,2^32)

        Output:
            Number of bits needed to represent input value

------------------------------------------------------------------------------*/

u32 StrmDec_NumBits(u32 value)
{

    u32 numBits = 0;

    while(value)
    {
        value >>= 1;
        numBits++;
    }

    if(!numBits)
    {
        numBits = 1;
    }

    return (numBits);

}

/*------------------------------------------------------------------------------

   5.9  Function name: StrmDec_UnFlushBits

        Purpose: unflushes bits from mpeg-4 input stream

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
            Number of bits to unflush [0,2^32)

        Output:
            HANTRO_OK if operation was successful

------------------------------------------------------------------------------*/

u32 StrmDec_UnFlushBits(DecContainer * pDecContainer, u32 numBits)
{

    if(pDecContainer->StrmDesc.strmBuffReadBits < numBits)
    {
        pDecContainer->StrmDesc.strmBuffReadBits = 0;
        pDecContainer->StrmDesc.bitPosInWord = 0;
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmDesc.pStrmBuffStart;
    }
    else
    {
        pDecContainer->StrmDesc.strmBuffReadBits -= numBits;
        pDecContainer->StrmDesc.bitPosInWord =
            pDecContainer->StrmDesc.strmBuffReadBits & 0x7;
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmDesc.pStrmBuffStart +
            (pDecContainer->StrmDesc.strmBuffReadBits >> 3);
    }

    return (HANTRO_OK);
}


/*------------------------------------------------------------------------------

   5.9  Function name: StrmDec_ProcessPacketFooter

        Purpose: store last byte of processed packet for checking SC_SV_END
           existence.

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc

        Output:

------------------------------------------------------------------------------*/

void StrmDec_ProcessPacketFooter( DecContainer * pDecCont )
{

/* Variables */

    u32 lastByte;

/* Code */

    if ( pDecCont->StrmStorage.shortVideo == HANTRO_TRUE &&
         pDecCont->StrmDesc.pStrmCurrPos > pDecCont->StrmDesc.pStrmBuffStart )
    {
        lastByte = pDecCont->StrmDesc.pStrmCurrPos[-1];
    }
    else
    {
        lastByte = 0xFF;
    }

    pDecCont->StrmStorage.lastPacketByte = lastByte;

}

/*------------------------------------------------------------------------------

   5.9  Function name: StrmDec_ProcessBvopExtraResync

        Purpose: 

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc

        Output:

------------------------------------------------------------------------------*/

u32 StrmDec_ProcessBvopExtraResync(DecContainer *pDecCont)
{

/* Variables */

    u32 markerLength;
    u32 tmp;
    u32 prevVpMbNum;

/* Code */

    if(pDecCont->StrmStorage.validVopHeader == HANTRO_TRUE)
    {
        markerLength = pDecCont->StrmStorage.resyncMarkerLength;

        tmp = StrmDec_ShowBits(pDecCont, markerLength);
        while( tmp == 0x01 )
        {
            tmp = StrmDec_FlushBits(pDecCont, markerLength);
            if( tmp == END_OF_STREAM )
                return HANTRO_NOK;
            /* We just want to decode the video packet header, so cheat on the 
             * macroblock number */
            prevVpMbNum = pDecCont->StrmStorage.vpMbNumber;
            pDecCont->StrmStorage.vpMbNumber = 
                StrmDec_CheckNextVpMbNumber( pDecCont );
            tmp = StrmDec_DecodeVideoPacketHeader( pDecCont );
            if( tmp != HANTRO_OK )
                return tmp;
            tmp = StrmDec_GetStuffing( pDecCont );
            if( tmp != HANTRO_OK )
                return tmp;

            pDecCont->StrmStorage.vpMbNumber = prevVpMbNum;

            /* Show next bits and loop again. */
            tmp = StrmDec_ShowBits(pDecCont, markerLength);
        } 
    }

    return HANTRO_OK;
}