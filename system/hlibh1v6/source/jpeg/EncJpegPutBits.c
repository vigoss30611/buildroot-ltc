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
--  Abstract  : 
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. External compiler flags
    3. Module defines
    4. Local function prototypes
    5. Functions

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "EncJpegPutBits.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

	EncJpegHeaderPutBits

	Write bits to stream. For example (value=2, number=4) write 0010 to the
	stream. Number of bits must be < 25, otherwise overflow occur.  Four
	bytes is maximum number of bytes to put stream and there should be at
	least 5 byte free space available because of byte buffer.
	stream[1] bits in byte buffer
	stream[0] byte buffer

	Input	stream	Pointer to the stream stucture
		value	Bit pattern
		number	Number of bits

------------------------------------------------------------------------------*/
void EncJpegHeaderPutBits(stream_s * buffer, u32 value, u32 number)
{
    u32 bits;
    u32 byteBuffer = buffer->byteBuffer;
    u8 *stream = buffer->stream;

    if(EncJpegBufferStatus(buffer) != ENCHW_OK)
    {
        return;
    }

    /* Debug: value is too big */
    ASSERT(value < ((u32) 1 << number));
    ASSERT(number < 25);

    TRACE_BIT_STREAM(value, number);

    bits = number + stream[1];

    value <<= (32 - bits);
    byteBuffer = (((u32) stream[0]) << 24) | value;

    while(bits > 7)
    {
        *stream = (u8) (byteBuffer >> 24);
        bits -= 8;
        byteBuffer <<= 8;
        stream++;
        buffer->byteCnt++;
    }

    stream[0] = (u8) (byteBuffer >> 24);
    stream[1] = (u8) bits;
    buffer->stream = stream;
    buffer->bitCnt += number;
    buffer->byteBuffer = byteBuffer;
    buffer->bufferedBits = (u8) bits;

    return;
}

/*------------------------------------------------------------------------------

	EncJpegNextByteAligned

	Function add zero stuffing until next byte aligned if needed. Note that
	stream->stream[1] is bits in byte bufer.

	Input	stream	Pointer to the stream structure.

------------------------------------------------------------------------------*/
void EncJpegNextByteAligned(stream_s * stream)
{
    if(stream->stream[1] > 0)
    {
        EncJpegHeaderPutBits(stream, 0, 8 - stream->stream[1]);
        COMMENT("Stuffing");
    }

    return;
}

/*------------------------------------------------------------------------------

	EncJpegBufferStatus

	Check fullness of stream buffer.

	Input	stream	Pointer to the stream stucture.

	Return	ENCHW_OK	Buffer status is OK.
			ENCHW_NOK	Buffer overflow.

------------------------------------------------------------------------------*/
bool_e EncJpegBufferStatus(stream_s * stream)
{
    if(stream->byteCnt + 5 > stream->size)
    {
        stream->overflow = ENCHW_YES;
        COMMENT("\nStream buffer is full     ");
        return ENCHW_NOK;
    }

    return ENCHW_OK;
}

/*------------------------------------------------------------------------------

	EncJpegSetBuffer

	Set stream buffer.

	Input	buffer	Pointer to the stream_s structure.
		stream	Pointer to stream buffer.
		size	Size of stream buffer.

------------------------------------------------------------------------------*/
bool_e EncJpegSetBuffer(stream_s * buffer, u8 * stream, u32 size)
{
    buffer->stream = stream;
    buffer->size = size;
    buffer->byteCnt = 0;
    buffer->overflow = ENCHW_NO;
    buffer->zeroBytes = 0;
    buffer->byteBuffer = 0;
    buffer->bufferedBits = 0;

    if(EncJpegBufferStatus(buffer) != ENCHW_OK)
    {
        return ENCHW_NOK;
    }
    buffer->stream[0] = 0;
    buffer->stream[1] = 0;

    return ENCHW_OK;
}
