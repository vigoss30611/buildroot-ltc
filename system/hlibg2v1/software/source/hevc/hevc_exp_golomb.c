/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Exp-Golomb decoding. */

#include "hevc_exp_golomb.h"
#include "basetype.h"
#include "sw_stream.h"
#include "hevc_util.h"

/* definition of special code num, this along with the return value is used
 * to handle code num in the range [0, 2^32] in the DecodeExpGolombUnsigned
 * function */
#define BIG_CODE_NUM 0xFFFFFFFFU

/* Decodes unsigned Exp-Golomb codeword. This is the same as code_num used in
 * other Exp-Golomb code mappings.
 *
 * Code num (i.e. the decoded symbol) is determined as
 *
 * code_num = 2^leading_zeros - 1 + SwGetBits(leading_zeros)
 *
 * Normal decoded symbols are in the range [0, 2^32 - 2]. Symbol
 * 2^32-1 is indicated by BIG_CODE_NUM with return value HANTRO_OK
 * while symbol 2^32  is indicated by BIG_CODE_NUM with return value
 * HANTRO_NOK.  These two symbols are special cases with code length
 * of 65, i.e.  32 '0' bits, a '1' bit, and either 0 or 1 represented
 * by 32 bits.
 *
 * Symbol 2^32 is out of unsigned 32-bit range but is needed for
 * DecodeExpGolombSigned to express value -2^31. */
u32 HevcDecodeExpGolombUnsigned(struct StrmData *stream, u32 *code_num) {

  u32 bits, num_zeros;

  ASSERT(stream);
  ASSERT(code_num);

  bits = SwShowBits(stream, 32);

  /* first bit is 1 -> code length 1 */
  if (bits >= 0x80000000) {
    if (SwFlushBits(stream, 1) == END_OF_STREAM) return (HANTRO_NOK);
    *code_num = 0;
    return (HANTRO_OK);
  }
  /* second bit is 1 -> code length 3 */
  else if (bits >= 0x40000000) {
    if (SwFlushBits(stream, 3) == END_OF_STREAM) return (HANTRO_NOK);
    *code_num = 1 + ((bits >> 29) & 0x1);
    return (HANTRO_OK);
  }
  /* third bit is 1 -> code length 5 */
  else if (bits >= 0x20000000) {
    if (SwFlushBits(stream, 5) == END_OF_STREAM) return (HANTRO_NOK);
    *code_num = 3 + ((bits >> 27) & 0x3);
    return (HANTRO_OK);
  }
  /* fourth bit is 1 -> code length 7 */
  else if (bits >= 0x10000000) {
    if (SwFlushBits(stream, 7) == END_OF_STREAM) return (HANTRO_NOK);
    *code_num = 7 + ((bits >> 25) & 0x7);
    return (HANTRO_OK);
  }
  /* other code lengths */
  else {
    num_zeros = 4 + SwCountLeadingZeros(bits, 28);

    /* all 32 bits are zero */
    if (num_zeros == 32) {
      *code_num = 0;
      if (SwFlushBits(stream, 32) == END_OF_STREAM) return (HANTRO_NOK);
      bits = SwGetBits(stream, 1);
      /* check 33rd bit, must be 1 */
      if (bits == 1) {
        /* cannot use SwGetBits, limited to 31 bits */
        bits = SwShowBits(stream, 32);
        if (SwFlushBits(stream, 32) == END_OF_STREAM) return (HANTRO_NOK);
        /* code num 2^32 - 1, needed for unsigned mapping */
        if (bits == 0) {
          *code_num = BIG_CODE_NUM;
          return (HANTRO_OK);
        }
        /* code num 2^32, needed for unsigned mapping
         * (results in -2^31) */
        else if (bits == 1) {
          *code_num = BIG_CODE_NUM;
          return (HANTRO_NOK);
        }
      }
      /* if more zeros than 32, it is an error */
      return (HANTRO_NOK);
    } else if (SwFlushBits(stream, num_zeros + 1) == END_OF_STREAM)
      return (HANTRO_NOK);

    bits = SwGetBits(stream, num_zeros);
    if (bits == END_OF_STREAM) return (HANTRO_NOK);

    *code_num = (1 << num_zeros) - 1 + bits;
  }

  return (HANTRO_OK);
}

/* Decode signed Exp-Golomb code. Code num is determined by
 * HevcDecodeExpGolombUnsigned and then mapped to signed representation as
 *
 * symbol = (-1)^(code_num+1) * (code_num+1)/2
 *
 * Signed symbols shall be in the range [-2^31, 2^31 - 1]. Symbol -2^31 is
 * obtained when code_num is 2^32, which cannot be expressed by unsigned
 * 32-bit value. This is signaled as a special case from the
 * HevcDecodeExpGolombUnsigned by setting code_num to BIG_CODE_NUM and
 * returning HANTRO_NOK status. */
u32 HevcDecodeExpGolombSigned(struct StrmData *stream, i32 *value) {

  u32 status, code_num = 0;

  ASSERT(stream);
  ASSERT(value);

  status = HevcDecodeExpGolombUnsigned(stream, &code_num);

  if (code_num == BIG_CODE_NUM) {
    /* BIG_CODE_NUM and HANTRO_OK status means code_num 2^32-1 which would
     * result in signed integer valued 2^31 (i.e. out of 32-bit signed
     * integer range) */
    if (status == HANTRO_OK) return (HANTRO_NOK);
    /* BIG_CODE_NUM and HANTRO_NOK status means code_num 2^32 which results
     * in signed integer valued -2^31 */
    else {
      *value = (i32)(2147483648U);
      return (HANTRO_OK);
    }
  } else if (status == HANTRO_OK) {
    /* (-1)^(code_num+1) results in positive sign if code_num is odd,
     * negative when it is even. (code_num+1)/2 is obtained as
     * (code_num+1)>>1 when value is positive and as (-code_num)>>1 for
     * negative value */
    /*lint -e702 */
    *value = (code_num & 0x1) ? (i32)((code_num + 1) >> 1)
                              : -(i32)((code_num + 1) >> 1);
    /*lint +e702 */
    return (HANTRO_OK);
  }

  return (HANTRO_NOK);
}
