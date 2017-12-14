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
--  Description : This module implements the functions for stream corrupting
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. External compiler flags
    3. Module defines
    4. Local function prototypes
    5. Functions
        - InitializeRandom
        - RandomizeBitSwapInStream
	- RandomizePacketLoss
        - ParseOdds
	- SwapBit
	- SetBit
	- ClearBit
	- GetBit

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "tb_stream_corrupt.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#ifdef _ASSERT_USED
#include <assert.h>
#endif


/*------------------------------------------------------------------------------
    2. External compiler flags
-------------------------------------------------------------------------------*/

/* _ASSERT_USED enables assertions */

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

#ifdef _ASSERT_USED
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr)
#endif

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

static u32 ParseOdds(char* odds, u32* dividend, u32* divisor);

static void SwapBit(u8* stream, u32 bytePosition, u8 bitPosition);

static void SetBit(u8* byte, u8 bitPosition);

static void ClearBit(u8* byte, u8 bitPosition);

static u8 GetBit(u8 byte, u8 bitPosition);

/* compensate for stuff available in linux */
#if ! defined(srand48) || ! defined(lrand48)
#define srand48     srand
#define lrand48     rand
#endif


/*------------------------------------------------------------------------------
    5. Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

  	Function name: TBInitializeRandom

        Functional description:
          Sets seed for randomize functions.

        Inputs:
            u32 seed    The seed value. The same seed value can be used
                             to enable reproducible behaviour.

        Outputs: -

        Returns: -


------------------------------------------------------------------------------*/

void TBInitializeRandom(u32 seed)
{
	srand48(seed);
}

/*------------------------------------------------------------------------------

  	Function name: TBRandomizeBitSwapInStream

        Functional description:
          Corrupts bits in the provided stream according to provided odds.
          E.g., using "1 : 10", there is 1 : 10 propability to set the bit into a
          different value.

        Inputs:
          u8* stream           Stream to be corrupted.
          u32 streamLen     Length of the stream.
          char* odds          Odds to corrupt a bit.

        Outputs:
          u8* stream           Corrupeted stream.

        Returns:
          u32                     0, if no error. != 0 if error


------------------------------------------------------------------------------*/

u32 TBRandomizeBitSwapInStream(u8* stream, u32 streamLen, char* odds)
{
    u32 dividend;
	u32 divisor;
	u32 retVal;
	u32 chunks;
	u32 i = 0;
	u32 j = 0;
	u32 k = 0;
	u32 bitInChunk = 0;
	u8 bitInByte = 0;
	u32 byteInStream = 0;
	u32* randomizedBits = NULL;

	retVal = ParseOdds(odds, &dividend, &divisor);
	if (retVal == 1)
		return 1;

	/* calculate the number of parts in the stream
	   divisor is the chunk size */
	chunks = ((streamLen * 8) / divisor);
	if ((streamLen * 8) % divisor != 0)
		++chunks;

	randomizedBits = (u32*) malloc(dividend*sizeof(u32));
	if (NULL == randomizedBits)
		return 1;

	/* select (randomize) and swap the bits in the stream */
	/* for each chunk */
	for (i = 0; i < chunks; ++i)
	{
		/* randomize and swap the bits */
		while (j < dividend)
		{
			/* do not randomize the same bit for the same chunck */
			bitInChunk = lrand48() % divisor;
			for (k = 0; k < j; ++k)
			{
				/* try again */
				if (bitInChunk == randomizedBits[k])
				{
					bitInChunk = lrand48() % divisor;
					k = -1;
				}
			}
			randomizedBits[j] = bitInChunk;

			/* calculate the bit position in stream and swap it */
			byteInStream = ((i * divisor) + bitInChunk) / 8;
			bitInByte = (divisor * i + bitInChunk) & 0x7;
			/* the last swapping can be beyond the stream length */
            if (byteInStream < streamLen)
				SwapBit(stream, byteInStream, bitInByte);
			++j;
		}
		j = 0;
	}
    
    free(randomizedBits);
    
	return 0;
}

/*------------------------------------------------------------------------------

  	Function name: TBRandomizePacketLoss

        Functional description:
          Set the provided packetLoss value to 1 according to odds.
          E.g., using "1 : 10", there is 1 : 10 propability to set the packetLoss
          to 1.

        Inputs:
          char* odds          Odds to set packetLoss to 1.

        Outputs:
          u8* packetLoss    This value is set to 1 according to odds.

        Returns:
          u32                     0, if no error. != 0 if error


------------------------------------------------------------------------------*/

u32 TBRandomizePacketLoss(char* odds, u8* packetLoss)
{
	u32 dividend;
	u32 divisor;
	u32 random;
	u32 retVal;

	retVal = ParseOdds(odds, &dividend, &divisor);
	if (retVal == 1)
		return 1;

	random = lrand48() % divisor + 1;
	if (random <= dividend)
		*packetLoss = 1;
	else
		*packetLoss = 0;

	return 0;
}

/*------------------------------------------------------------------------------

  	Function name: TBRandomizeU32

        Functional description:
          Randomizes the provided value in range of [0, value].

        Inputs:
          u32* value        The upper limit of randomized value.

        Outputs:
          u32* value        The randomized value.

        Returns:
          u32                  0, if no error. != 0 if error


------------------------------------------------------------------------------*/
u32 TBRandomizeU32(u32* value)
{
    *value = lrand48() % (*value + 1);
    return 0;
}

/*------------------------------------------------------------------------------

  	Function name: ParseOdds

        Functional description:
          Parses the dividend and divisor from the provided odds.

        Inputs:
          char* odds            The odds.

        Outputs:
          u32* dividend       Dividend part of the odds.
          u32* divisor          Divisor part of the odds.

        Returns:
          u32                     0, if no error. != 0 if error


------------------------------------------------------------------------------*/

u32 ParseOdds(char* odds, u32* dividend, u32* divisor)
{
	u32 i;
	char oddsCopy[23];
	char* ptr;
	u32 strLen = strlen(odds);

	strcpy(oddsCopy, odds);
	ptr = oddsCopy;

	/* minimum is "1 : 1" */
	if (strLen < 5)
	{
		return 1;
	}

	for(i = 0; i < strLen - 2; i++)
	{
		if (oddsCopy[i] == ' ' &&
			oddsCopy[i+1] == ':' &&
			oddsCopy[i+2] == ' ')
		{
			oddsCopy[i] = '\0';
			*dividend = atoi(ptr);
			ptr += 3 + i;
			*divisor = atoi(ptr);
			ptr -= 3 - i;
			if (*divisor == 0)
				return 1;
			return 0;
		}

	}
	return 1;
}

/*------------------------------------------------------------------------------

  	Function name: SwapBit

        Functional description:
          Change a bit in a different value of the provided stream.

        Inputs:
          u8* stream                 The stream to be corrupted
          u32 bytePosition         The offset of the byte in which the bit is swapped.
          u8 bitPosition             The offset of the bit to be swapped in the byte.

        Outputs:
          u8* stream                 The corrupted stream.

	    Returns:
           -


------------------------------------------------------------------------------*/

void SwapBit(u8* stream, u32 bytePosition, u8 bitPosition)
{
	u8 bit;

	ASSERT(bitPosition < 8);

	bit = GetBit(stream[bytePosition], bitPosition);
	if (bit == 0)
		SetBit(&(stream[bytePosition]), bitPosition);
	else
		ClearBit(&(stream[bytePosition]), bitPosition);
}

/*------------------------------------------------------------------------------

  	Function name: SetBit

        Functional description:
          Sets the bit in the provided byte.

        Inputs:
          u8* byte                  The byte in which the bit is set.
          u8 bitPosition           The offset of the bit to be set in the byte.

        Outputs:
          u8* byte                  The changed byte.

	    Returns:
          -


------------------------------------------------------------------------------*/

void SetBit(u8* byte, u8 bitPosition)
{
	u8 mask = 0x01;

	ASSERT(bitPosition < 8);

	mask <<= bitPosition;
	*byte |= mask;
}

/*------------------------------------------------------------------------------

  	Function name: ClearBit

        Functional description:
          Clears the bit in the provided byte.

        Inputs:
          u8* byte                  The byte in which the bit is cleared.
          u8 bitPosition           The offset of the bit to be cleared in the byte.

        Outputs:
          u8* byte                  The changed byte.

	    Returns:
          -


------------------------------------------------------------------------------*/

void ClearBit(u8* byte, u8 bitPosition)
{
	u8 mask = 0x01;

	ASSERT(bitPosition < 8);

	mask = ~(mask << bitPosition);
	*byte &= mask;
}

/*------------------------------------------------------------------------------

  	Function name: GetBit

        Functional description:
          Gets the bit status in the provided byte

        Inputs:
          u8 byte                  The byte from which the bit status is read.
          u8 bitPosition          The offset of the bit to be get in the byte.

        Outputs:
          u8* byte                 The changed byte.

	  Returns:
         u8                          The bit value, i.e., 1 if the bit is set and 0 if the bit is cleared.


------------------------------------------------------------------------------*/

u8 GetBit(u8 byte, u8 bitPosition)
{
	u8 mask = 0x01;

	ASSERT(bitPosition < 8);

	mask <<= bitPosition;
	if ((byte & mask) == mask)
		return 1;
	else
		return 0;
}



