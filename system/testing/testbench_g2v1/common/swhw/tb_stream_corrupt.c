/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * This module implements the functions for stream corrupting. */

#include "tb_stream_corrupt.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#ifdef _ASSERT_USED
#include <assert.h>
#endif

#ifdef _ASSERT_USED
#define ASSERT(expr) if (!(expr)) printf("+++ G2 ASSERT(%s:%d) 666:"#expr"...\n", __FILE__, __LINE__)//assert(expr)
#else
#define ASSERT(expr) if (!(expr)) printf("+++ G2 ASSERT(%s:%d) 666:"#expr"...\n", __FILE__, __LINE__)
#endif

static u32 ParseOdds(char* odds, u32* dividend, u32* divisor);

static void SwapBit(u8* stream, u32 byte_position, u8 bit_position);

static void SetBit(u8* byte, u8 bit_position);

static void ClearBit(u8* byte, u8 bit_position);

static u8 GetBit(u8 byte, u8 bit_position);

/* compensate for stuff available in linux */
#if !defined(srand48) || !defined(lrand48)
#define srand48 srand
#define lrand48 rand
#endif

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

void TBInitializeRandom(u32 seed) { srand48(seed); }

/*------------------------------------------------------------------------------

        Function name: TBRandomizeBitSwapInStream

        Functional description:
          Corrupts bits in the provided stream according to provided odds.
          E.g., using "1 : 10", there is 1 : 10 propability to set the bit into
a
          different value.

        Inputs:
          u8* stream           Stream to be corrupted.
          u32 stream_len     Length of the stream.
          char* odds          Odds to corrupt a bit.

        Outputs:
          u8* stream           Corrupeted stream.

        Returns:
          u32                     0, if no error. != 0 if error


------------------------------------------------------------------------------*/

u32 TBRandomizeBitSwapInStream(u8* stream, u32 stream_len, char* odds) {
  u32 dividend;
  u32 divisor;
  u32 ret_val;
  u32 chunks;
  u32 i = 0;
  u32 j = 0;
  u32 k = 0;
  u32 bit_in_chunk = 0;
  u8 bit_in_byte = 0;
  u32 byte_in_stream = 0;
  u32* randomized_bits = NULL;

  ret_val = ParseOdds(odds, &dividend, &divisor);
  if (ret_val == 1) return 1;

  /* calculate the number of parts in the stream
     divisor is the chunk size */
  chunks = ((stream_len * 8) / divisor);
  if ((stream_len * 8) % divisor != 0) ++chunks;

  randomized_bits = (u32*)malloc(dividend * sizeof(u32));
  if (NULL == randomized_bits) return 1;

  /* select (randomize) and swap the bits in the stream */
  /* for each chunk */
  for (i = 0; i < chunks; ++i) {
    /* randomize and swap the bits */
    while (j < dividend) {
      /* do not randomize the same bit for the same chunck */
      bit_in_chunk = lrand48() % divisor;
      for (k = 0; k < j; ++k) {
        /* try again */
        if (bit_in_chunk == randomized_bits[k]) {
          bit_in_chunk = lrand48() % divisor;
          k = -1;
        }
      }
      randomized_bits[j] = bit_in_chunk;

      /* calculate the bit position in stream and swap it */
      byte_in_stream = ((i * divisor) + bit_in_chunk) / 8;
      bit_in_byte = (divisor * i + bit_in_chunk) & 0x7;
      /* the last swapping can be beyond the stream length */
      if (byte_in_stream < stream_len)
        SwapBit(stream, byte_in_stream, bit_in_byte);
      ++j;
    }
    j = 0;
  }

  free(randomized_bits);

  return 0;
}

/*------------------------------------------------------------------------------

        Function name: TBRandomizePacketLoss

        Functional description:
          Set the provided packet_loss value to 1 according to odds.
          E.g., using "1 : 10", there is 1 : 10 propability to set the
packet_loss
          to 1.

        Inputs:
          char* odds          Odds to set packet_loss to 1.

        Outputs:
          u8* packet_loss    This value is set to 1 according to odds.

        Returns:
          u32                     0, if no error. != 0 if error


------------------------------------------------------------------------------*/

u32 TBRandomizePacketLoss(char* odds, u8* packet_loss) {
  u32 dividend;
  u32 divisor;
  u32 random;
  u32 ret_val;

  ret_val = ParseOdds(odds, &dividend, &divisor);
  if (ret_val == 1) return 1;

  random = lrand48() % divisor + 1;
  if (random <= dividend)
    *packet_loss = 1;
  else
    *packet_loss = 0;

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
u32 TBRandomizeU32(u32* value) {
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

u32 ParseOdds(char* odds, u32* dividend, u32* divisor) {
  u32 i;
  char odds_copy[23];
  char* ptr;
  u32 str_len = strlen(odds);

  strcpy(odds_copy, odds);
  ptr = odds_copy;

  /* minimum is "1 : 1" */
  if (str_len < 5) {
    return 1;
  }

  for (i = 0; i < str_len - 2; i++) {
    if (odds_copy[i] == ' ' && odds_copy[i + 1] == ':' &&
        odds_copy[i + 2] == ' ') {
      odds_copy[i] = '\0';
      *dividend = atoi(ptr);
      ptr += 3 + i;
      *divisor = atoi(ptr);
      ptr -= 3 - i;
      if (*divisor == 0) return 1;
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
          u32 byte_position         The offset of the byte in which the bit is
swapped.
          u8 bit_position             The offset of the bit to be swapped in the
byte.

        Outputs:
          u8* stream                 The corrupted stream.

            Returns:
           -


------------------------------------------------------------------------------*/

void SwapBit(u8* stream, u32 byte_position, u8 bit_position) {
  u8 bit;

  ASSERT(bit_position < 8);

  bit = GetBit(stream[byte_position], bit_position);
  if (bit == 0)
    SetBit(&(stream[byte_position]), bit_position);
  else
    ClearBit(&(stream[byte_position]), bit_position);
}

/*------------------------------------------------------------------------------

        Function name: SetBit

        Functional description:
          Sets the bit in the provided byte.

        Inputs:
          u8* byte                  The byte in which the bit is set.
          u8 bit_position           The offset of the bit to be set in the byte.

        Outputs:
          u8* byte                  The changed byte.

            Returns:
          -


------------------------------------------------------------------------------*/

void SetBit(u8* byte, u8 bit_position) {
  u8 mask = 0x01;

  ASSERT(bit_position < 8);

  mask <<= bit_position;
  *byte |= mask;
}

/*------------------------------------------------------------------------------

        Function name: ClearBit

        Functional description:
          Clears the bit in the provided byte.

        Inputs:
          u8* byte                  The byte in which the bit is cleared.
          u8 bit_position           The offset of the bit to be cleared in the
byte.

        Outputs:
          u8* byte                  The changed byte.

            Returns:
          -


------------------------------------------------------------------------------*/

void ClearBit(u8* byte, u8 bit_position) {
  u8 mask = 0x01;

  ASSERT(bit_position < 8);

  mask = ~(mask << bit_position);
  *byte &= mask;
}

/*------------------------------------------------------------------------------

        Function name: GetBit

        Functional description:
          Gets the bit status in the provided byte

        Inputs:
          u8 byte                  The byte from which the bit status is read.
          u8 bit_position          The offset of the bit to be get in the byte.

        Outputs:
          u8* byte                 The changed byte.

          Returns:
         u8                          The bit value, i.e., 1 if the bit is set
and 0 if the bit is cleared.


------------------------------------------------------------------------------*/

u8 GetBit(u8 byte, u8 bit_position) {
  u8 mask = 0x01;

  ASSERT(bit_position < 8);

  mask <<= bit_position;
  if ((byte & mask) == mask)
    return 1;
  else
    return 0;
}
