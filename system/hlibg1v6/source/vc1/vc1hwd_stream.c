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
--  Description : Stream accessing
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_stream.c,v $
--  $Revision: 1.10 $
--  $Date: 2007/12/12 13:25:58 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1hwd_util.h"
#include "vc1hwd_stream.h"

/*------------------------------------------------------------------------------
    External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

static u32 SwShowBits32(strmData_t *pStrmData);
static u32 SwFlushBits(strmData_t *pStrmData, u32 numBits);
static u32 SwFlushBitsAdv(strmData_t *pStrmData, u32 numBits);

/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function: vc1hwdGetBits

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

u32 vc1hwdGetBits(strmData_t *pStrmData, u32 numBits)
{

    u32 out;

    ASSERT(pStrmData);

    if (pStrmData->removeEmulPrevBytes)
        out = vc1hwdShowBits( pStrmData, numBits );
    else
        out = (SwShowBits32(pStrmData) >> (32 - numBits));

    if (vc1hwdFlushBits(pStrmData, numBits) == HANTRO_OK)
    {
        return(out);
    }
    else
    {
        return(END_OF_STREAM);
    }
}

/*------------------------------------------------------------------------------

    Function: vc1hwdShowBits

        Functional description:
            Read bits from the stream buffer. Buffer is left as it is, i.e.
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
u32 vc1hwdShowBits(strmData_t *pStrmData, u32 numBits )
{

    i32 bits;
    u32 out, outBits;
    u32 tmpReadBits;
    u8 *pStrm;

    ASSERT(pStrmData);
    ASSERT(pStrmData->pStrmCurrPos);
    ASSERT(pStrmData->bitPosInWord < 8);
    ASSERT(pStrmData->bitPosInWord ==
           (pStrmData->strmBuffReadBits & 0x7));
    ASSERT(numBits <= 32);

    pStrm = pStrmData->pStrmCurrPos;

    /* bits left in the buffer */
    bits = (i32)pStrmData->strmBuffSize*8 - (i32)pStrmData->strmBuffReadBits;
    
    if (!numBits || !bits)
    {
        return(0);
    }

    out = outBits = 0;
    tmpReadBits = pStrmData->strmBuffReadBits;

    if (pStrmData->bitPosInWord)
    {
        out = pStrm[0]<<(24+pStrmData->bitPosInWord); 
        pStrm++;
        outBits = 8-pStrmData->bitPosInWord;
        bits -= outBits;
        tmpReadBits += outBits;
    }

    while (bits && outBits < numBits)
    {
        /* check emulation prevention byte */
        if (pStrmData->removeEmulPrevBytes && tmpReadBits >= 16 &&
            pStrm[-2] == 0x0 && pStrm[-1] == 0x0 && pStrm[0] == 0x3)
        {
            pStrm++;
            tmpReadBits += 8;
            bits -= 8;
            /* emulation prevention byte shall not be the last byte of the
             * stream */
            if (bits <= 0)
                break;
        }

        if (outBits <= 24)
            out |= (u32)(*pStrm++) << (24 - outBits);
        else
            out |= (u32)(*pStrm++) >> (outBits-24);
        outBits += 8;

        tmpReadBits += 8;
        bits -= 8;
    }

    return(out>>(32-numBits));

}

/*------------------------------------------------------------------------------

    Function: SwShowBits32

        Functional description:
            Read 32 bits from the stream buffer when emulation prevention bytes
            are not present in the bitstream. Buffer is left as it is, i.e.
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
u32 SwShowBits32(strmData_t *pStrmData)
{
    i32 bits, shift;
    u32 out;
    u8 *pStrm;

    ASSERT(pStrmData);
    ASSERT(pStrmData->pStrmCurrPos);
    ASSERT(pStrmData->bitPosInWord < 8);
    ASSERT(pStrmData->bitPosInWord ==
           (pStrmData->strmBuffReadBits & 0x7));

    pStrm = pStrmData->pStrmCurrPos;

    /* number of bits left in the buffer */
    bits = (i32)pStrmData->strmBuffSize*8 - (i32)pStrmData->strmBuffReadBits;
        
    /* at least 32-bits in the buffer */
    if (bits >= 32)
    {
        u32 bitPosInWord = pStrmData->bitPosInWord;
        
        out = ((u32)pStrm[3]) | ((u32)pStrm[2] <<  8) | 
              ((u32)pStrm[1] << 16) | ((u32)pStrm[0] << 24);

        if (bitPosInWord)
        {
            out <<= bitPosInWord;
            out |= (u32)pStrm[4]>>(8-bitPosInWord);
        }
        return (out);
    }
    /* at least one bit in the buffer */
    else if (bits > 0)
    {
        shift = (i32)(24 + pStrmData->bitPosInWord);
        out = (u32)(*pStrm++) << shift;
        bits -= (i32)(8 - pStrmData->bitPosInWord);
        while (bits > 0)
        {
            shift -= 8;
            out |= (u32)(*pStrm++) << shift;
            bits -= 8;
        }
        return (out);
    }
    else
        return (0);
}


/*------------------------------------------------------------------------------

    Function: vc1hwdFlushBits

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
u32 vc1hwdFlushBits(strmData_t *pStrmData, u32 numBits)
{
    u32 rv;

    ASSERT(pStrmData);
    ASSERT(pStrmData->pStrmBuffStart);
    ASSERT(pStrmData->pStrmCurrPos);
    ASSERT(pStrmData->bitPosInWord < 8);
    ASSERT(pStrmData->bitPosInWord == (pStrmData->strmBuffReadBits & 0x7));

    if (pStrmData->removeEmulPrevBytes)
        rv = SwFlushBitsAdv( pStrmData, numBits );
    else
        rv = SwFlushBits(pStrmData, numBits);

    return rv;
}


/*------------------------------------------------------------------------------

    Function: SwFlushBits

        Functional description:
            Remove bits from the stream buffer for streams that are not
            containing emulation prevention bytes

        Input:
            pStrmData       pointer to stream data structure
            numBits         number of bits to remove

        Output:
            none

        Returns:
            HANTRO_OK       success
            END_OF_STREAM   not enough bits left

------------------------------------------------------------------------------*/
u32 SwFlushBits(strmData_t *pStrmData, u32 numBits)
{
    ASSERT(pStrmData);
    ASSERT(pStrmData->pStrmBuffStart);
    ASSERT(pStrmData->pStrmCurrPos);
    ASSERT(pStrmData->bitPosInWord < 8);
    ASSERT(pStrmData->bitPosInWord == (pStrmData->strmBuffReadBits & 0x7));

    pStrmData->strmBuffReadBits += numBits;
    pStrmData->bitPosInWord = pStrmData->strmBuffReadBits & 0x7;
    if ( (pStrmData->strmBuffReadBits ) <= (8*pStrmData->strmBuffSize) )
    {
        pStrmData->pStrmCurrPos = pStrmData->pStrmBuffStart +
            (pStrmData->strmBuffReadBits >> 3);
        return(HANTRO_OK);
    }
    else
    {
        pStrmData->strmExhausted = HANTRO_TRUE;
        return(END_OF_STREAM);
    }
}

/*------------------------------------------------------------------------------

    Function: SwFlushBitsAdv

        Functional description:
            Remove bits from the stream buffer for streams with emulation
            prevention bytes.

        Input:
            pStrmData       pointer to stream data structure
            numBits         number of bits to remove

        Output:
            none

        Returns:
            HANTRO_OK       success
            END_OF_STREAM   not enough bits left

------------------------------------------------------------------------------*/
u32 SwFlushBitsAdv(strmData_t *pStrmData, u32 numBits)
{

    u32 bytesLeft;
    u8 *pStrm;

    ASSERT(pStrmData);
    ASSERT(pStrmData->pStrmBuffStart);
    ASSERT(pStrmData->pStrmCurrPos);
    ASSERT(pStrmData->bitPosInWord < 8);
    ASSERT(pStrmData->bitPosInWord == (pStrmData->strmBuffReadBits & 0x7));

    if ( (pStrmData->strmBuffReadBits + numBits) > (8*pStrmData->strmBuffSize) )
    {
        pStrmData->strmBuffReadBits = 8 * pStrmData->strmBuffSize;
        pStrmData->bitPosInWord = 0;
        pStrmData->pStrmCurrPos =
            pStrmData->pStrmBuffStart + pStrmData->strmBuffSize;
        pStrmData->strmExhausted = HANTRO_TRUE;
        return(END_OF_STREAM);
    }
    else
    {
        bytesLeft = (8*pStrmData->strmBuffSize-pStrmData->strmBuffReadBits)/8;
        pStrm = pStrmData->pStrmCurrPos;
        if (pStrmData->bitPosInWord)
        {
            if (numBits < 8-pStrmData->bitPosInWord)
            {
                pStrmData->strmBuffReadBits += numBits;
                pStrmData->bitPosInWord += numBits;
                return(HANTRO_OK);
            }
            numBits -= 8-pStrmData->bitPosInWord;
            pStrmData->strmBuffReadBits += 8-pStrmData->bitPosInWord;
            pStrmData->bitPosInWord = 0;
            pStrm++;
            if (pStrmData->strmBuffReadBits >= 16 && bytesLeft &&
                pStrm[-2] == 0x0 && pStrm[-1] == 0x0 && pStrm[0] == 0x3)
            {
                pStrm++;
                pStrmData->strmBuffReadBits += 8;
                bytesLeft--;
            }
        }

        while (numBits >= 8 && bytesLeft)
        {
            pStrm++;
            pStrmData->strmBuffReadBits += 8;
            bytesLeft--;
            /* check emulation prevention byte */
            if (pStrmData->strmBuffReadBits >= 16 && bytesLeft &&
                pStrm[-2] == 0x0 && pStrm[-1] == 0x0 && pStrm[0] == 0x3)
            {
                pStrm++;
                pStrmData->strmBuffReadBits += 8;
                bytesLeft--;
            }

            numBits -= 8;
        }

        if (numBits && bytesLeft)
        {
            pStrmData->strmBuffReadBits += numBits;
            pStrmData->bitPosInWord = numBits;
            numBits = 0;
        }
        pStrmData->pStrmCurrPos = pStrm;

        if (numBits)
        {
            pStrmData->strmExhausted = HANTRO_TRUE;
            return(END_OF_STREAM);
        }
        else
            return(HANTRO_OK);
    }

}

/*------------------------------------------------------------------------------

    Function: vc1hwdIsExhausted

        Functional description:
            Check if attempted to read more bits from stream than there are
            available.

        Inputs:
            pStrmData   pointer to stream data structure

        Outputs:
            None

        Returns:
            TRUE        stream is exhausted.
            FALSE       stream is not exhausted.


------------------------------------------------------------------------------*/

u32 vc1hwdIsExhausted(const strmData_t * const pStrmData)
{
    return pStrmData->strmExhausted;
}

