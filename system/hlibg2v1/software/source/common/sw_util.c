/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "sw_util.h"
#include "sw_debug.h"

u32 SwCountLeadingZeros(u32 value, u32 length) {

  u32 zeros = 0;
  u32 mask = 1 << (length - 1);

  ASSERT(length <= 32);

  while (mask && !(value & mask)) {
    zeros++;
    mask >>= 1;
  }
  return (zeros);
}

u32 SwNumBits(u32 value) { return 32 - SwCountLeadingZeros(value, 32); }

u8* SwTurnAround(const u8 * strm, const u8* buf, u8* tmp_buf, u32 buf_size, u32 num_bits) {
  u32 bytes = (num_bits+7)/8;
  u32 is_turn_around = 0;
  if((strm + bytes) > (buf + buf_size))
    is_turn_around = 1;

  if((addr_t)strm - (addr_t)buf < 2)  {
    ASSERT(is_turn_around == 0);
    is_turn_around = 2;
  }

  if(is_turn_around == 0) {
    return NULL;
  } else if(is_turn_around == 1){
    i32 i;
    u32 bytes_left = (u32)((addr_t)(buf + buf_size) - (addr_t)strm);

    /* turn around */
    for(i = -3; i < (i32)bytes_left; i++) {
      tmp_buf[3 + i] = DWLPrivateAreaReadByte(strm + i);
    }
  
    /*turn around point*/
    for(i = 0; i < (bytes - bytes_left); i++) {
      tmp_buf[3 + bytes_left + i] = DWLPrivateAreaReadByte(buf + i);
    }
  
    return tmp_buf+3;
  } else {
    i32 i;
    u32 left_byte = (u32)((addr_t) strm - (addr_t) buf);
    /* turn around */
    for(i = 0; i < 2; i++) {
      tmp_buf[i] = DWLPrivateAreaReadByte(buf + buf_size - 2 + i);
    }

    /*turn around point*/
    for(i = 0; i < bytes + left_byte; i++) {
      tmp_buf[i + 2] = DWLPrivateAreaReadByte(buf + i);
    }

    return (tmp_buf + 2 + left_byte);
  }
}
