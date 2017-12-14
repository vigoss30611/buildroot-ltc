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
--  $RCSfile: mpeg2hwd_utils.c,v $
--  $Date: 2008/11/26 07:16:43 $
--  $Revision: 1.8 $
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

#include "mpeg2hwd_utils.h"
#include "mpeg2hwd_debug.h"

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

/*------------------------------------------------------------------------------

   5.6  Function name: mpeg2StrmDec_NextStartCode

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_NextStartCode(DecContainer * pDecContainer)
{
    u32 tmp;

    ASSERT(pDecContainer);

    if((pDecContainer->StrmDesc.bitPosInWord % 8) != 0)
    {
        tmp = mpeg2StrmDec_FlushBits(pDecContainer,
                                     (8 -
                                      (pDecContainer->StrmDesc.bitPosInWord %
                                       8)));
        if(tmp != HANTRO_OK)
        {
            return END_OF_STREAM;
        }
    }

    do
    {
        tmp = mpeg2StrmDec_ShowBits32(pDecContainer);
        if((tmp >> 8) == 0x1)
        {
            if(mpeg2StrmDec_FlushBits(pDecContainer, 32) != HANTRO_OK)
            {
                return END_OF_STREAM;
            }
            else
            {
                return (tmp & 0xFF);
            }
        }
    }
    while(mpeg2StrmDec_FlushBits(pDecContainer, 8) == HANTRO_OK);

    return (END_OF_STREAM);

}

/*------------------------------------------------------------------------------

   5.8  Function name: mpeg2StrmDec_NumBits

        Purpose: computes number of bits needed to represent value given as
        argument
                 
        Input: 
            u32 value [0,2^32)

        Output:
            Number of bits needed to represent input value

------------------------------------------------------------------------------*/

u32 mpeg2StrmDec_NumBits(u32 value)
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

   5.10  Function name: mpeg2StrmDec_CheckStuffing

        Purpose: checks mpeg-2 stuffing from input stream
                 
        Input: 
            Pointer to decContainer_t structure
                -uses and updates StrmDesc

        Output:
            HANTRO_OK if there is stuffing at current stream position
            HANTRO_NOK otherwise

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_CheckStuffing(DecContainer * pDecContainer)
{

    u32 length;

    ASSERT(pDecContainer);
    ASSERT(pDecContainer->StrmDesc.bitPosInWord < 8);

    length = (8 - pDecContainer->StrmDesc.bitPosInWord) & 0x3;

#ifdef HANTRO_PEDANTIC_MODE
    if((mpeg2StrmDec_ShowBits32(pDecContainer) >> (32 - length)) == 0)
        return (HANTRO_OK);
    else
        return (HANTRO_NOK);
#else
    return (HANTRO_OK);
#endif

}

/*------------------------------------------------------------------------------

    Function: mpeg2StrmDec_GetBits

        Functional description:
            Read and remove bits from the stream buffer.
                 
        Input: 
            pStrmData   pointer to stream data structure
            numBits     number of bits to read

        Output:
            none

        Returns:
            bits read from stream
            END_OF_STREAM if not enough bits left

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_GetBits(DecContainer * pDecContainer, u32 numBits)
{

    u32 out;

    ASSERT(pDecContainer);
    ASSERT(numBits < 32);

    out = mpeg2StrmDec_ShowBits32(pDecContainer) >> (32 - numBits);

    if(mpeg2StrmDec_FlushBits(pDecContainer, numBits) == HANTRO_OK)
    {
        return (out);
    }
    else
    {
        return (END_OF_STREAM);
    }
}

/*------------------------------------------------------------------------------

    Function: mmpeg2StrmDec_ShowBits32

        Functional description:
            Read 32 bits from the stream buffer. Buffer is left as it is, i.e.
            no bits are removed. First bit read from the stream is the MSB of
            the return value. If there is not enough bits in the buffer ->
            bits beyong the end of the stream are set to '0' in the return
            value.
                 
        Input: 
            pStrmData   pointer to stream data structure

        Output:
            none

        Returns:
            bits read from stream
                 
------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_ShowBits32(DecContainer * pDecContainer)
{

    i32 bits, shift;
    u32 out;
    u8 *pStrm;

    ASSERT(pDecContainer);
    ASSERT(pDecContainer->StrmDesc.pStrmCurrPos);
    ASSERT(pDecContainer->StrmDesc.bitPosInWord < 8);
    ASSERT(pDecContainer->StrmDesc.bitPosInWord ==
           (pDecContainer->StrmDesc.strmBuffReadBits & 0x7));

    pStrm = pDecContainer->StrmDesc.pStrmCurrPos;

    /* number of bits left in the buffer */
    bits =
        (i32) pDecContainer->StrmDesc.strmBuffSize * 8 -
        (i32) pDecContainer->StrmDesc.strmBuffReadBits;

    /* at least 32-bits in the buffer */
    if(bits >= 32)
    {
        u32 bitPosInWord = pDecContainer->StrmDesc.bitPosInWord;

        out = ((u32) pStrm[3]) | ((u32) pStrm[2] << 8) |
            ((u32) pStrm[1] << 16) | ((u32) pStrm[0] << 24);

        if(bitPosInWord)
        {
            out <<= bitPosInWord;
            out |= (u32) pStrm[4] >> (8 - bitPosInWord);
        }
        return (out);
    }
    /* at least one bit in the buffer */
    else if(bits > 0)
    {
        shift = (i32) (24 + pDecContainer->StrmDesc.bitPosInWord);
        out = (u32) (*pStrm++) << shift;
        bits -= (i32) (8 - pDecContainer->StrmDesc.bitPosInWord);
        while(bits > 0)
        {
            shift -= 8;
            out |= (u32) (*pStrm++) << shift;
            bits -= 8;
        }
        return (out);
    }
    else
        return (0);

}

/*------------------------------------------------------------------------------

    Function: mpeg2StrmDec_FlushBits

        Functional description:
            Remove bits from the stream buffer
                 
        Input: 
            pStrmData       pointer to stream data structure
            numBits         number of bits to remove

        Output:
            none

        Returns:
            HANTRO_OK       success
            END_OF_STREAM   not enough bits left
            
------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_FlushBits(DecContainer * pDecContainer, u32 numBits)
{

    ASSERT(pDecContainer);
    ASSERT(pDecContainer->StrmDesc.pStrmBuffStart);
    ASSERT(pDecContainer->StrmDesc.pStrmCurrPos);
    ASSERT(pDecContainer->StrmDesc.bitPosInWord < 8);
    ASSERT(pDecContainer->StrmDesc.bitPosInWord ==
           (pDecContainer->StrmDesc.strmBuffReadBits & 0x7));

    pDecContainer->StrmDesc.strmBuffReadBits += numBits;
    pDecContainer->StrmDesc.bitPosInWord =
        pDecContainer->StrmDesc.strmBuffReadBits & 0x7;
    if((pDecContainer->StrmDesc.strmBuffReadBits) <=
       (8 * pDecContainer->StrmDesc.strmBuffSize))
    {
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmDesc.pStrmBuffStart +
            (pDecContainer->StrmDesc.strmBuffReadBits >> 3);
        return (HANTRO_OK);
    }
    else
    {
        pDecContainer->StrmDesc.pStrmCurrPos =
            pDecContainer->StrmDesc.pStrmBuffStart +
            pDecContainer->StrmDesc.strmBuffSize;
        return (END_OF_STREAM);
    }
}

/*------------------------------------------------------------------------------

   5.2  Function name: mpeg2StrmDec_ShowBits

        Purpose: read bits from mpeg-2 input stream. Bits are located right
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
u32 mpeg2StrmDec_ShowBits(DecContainer * pDecContainer, u32 numBits)
{

    u32 i;
    i32 bits, shift;
    u32 out;
    u8 *pstrm = pDecContainer->StrmDesc.pStrmCurrPos;

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
            out |= (u32) pstrm[4] >> (8 - pDecContainer->StrmDesc.bitPosInWord);
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

   5.3  Function name: mpeg2StrmDec_ShowBitsAligned

        Purpose: read bits from mpeg-2 input stream at byte aligned position
        given as parameter (offset from current stream position).
                 
        Input: 
            Pointer to DecContainer structure
                -uses but does not update StrmDesc
            Number of bits to read [1,32]
            Byte offset from current stream position

        Output:
            u32 containing bits read from stream

------------------------------------------------------------------------------*/

u32 mpeg2StrmDec_ShowBitsAligned(DecContainer * pDecContainer, u32 numBits,
                                 u32 byteOffset)
{

    u32 i;
    u32 out;
    u32 bytes, shift;
    u8 *pstrm = pDecContainer->StrmDesc.pStrmCurrPos + byteOffset;

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
