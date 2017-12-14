/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "dwl.h"
#include "regdrv.h"

#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

#if 0
#ifdef COMPRESSED
#define MAX_REG_COUNT 257
#elif defined(DOWN_SCALER)
#define MAX_REG_COUNT 189
#else
#define MAX_REG_COUNT 184
#endif
#else
#define MAX_REG_COUNT 257
#endif

static const u32 reg_mask[33] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000F, 0x0000001F,
    0x0000003F, 0x0000007F, 0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
    0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF, 0x0001FFFF,
    0x0003FFFF, 0x0007FFFF, 0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
    0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF, 0x1FFFFFFF,
    0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF};

void SetDecRegister(u32* reg_base, u32 id, u32 value) {

  u32 tmp;

  ASSERT(id < HWIF_LAST_REG);

  tmp = reg_base[hw_dec_reg_spec[id][0]];
  tmp &= ~(reg_mask[hw_dec_reg_spec[id][1]] << hw_dec_reg_spec[id][2]);
  tmp |= (value & reg_mask[hw_dec_reg_spec[id][1]]) << hw_dec_reg_spec[id][2];
  reg_base[hw_dec_reg_spec[id][0]] = tmp;
}

u32 GetDecRegister(const u32* reg_base, u32 id) {

  u32 tmp;

  ASSERT(id < HWIF_LAST_REG);

  tmp = reg_base[hw_dec_reg_spec[id][0]];
  tmp = tmp >> hw_dec_reg_spec[id][2];
  tmp &= reg_mask[hw_dec_reg_spec[id][1]];
  return (tmp);
}

static void GetDecRegNumbers(u32* reg_count, u32* reg_offsets,
                             u32 writable_only) {
  u32 offset = -1, prev_offset = -1;
  *reg_count = 0;
  /* Loop through registers (as defined in enum HwIfName enumeration). */
  for (u32 reg = 0; reg != HWIF_LAST_REG; reg++) {
    if (!writable_only || hw_dec_reg_spec[reg][3] == 1) {
      offset = hw_dec_reg_spec[reg][0];
      /* Loop may write single reg multiple time, even if we do this simple
       * check for the same reg multiple times in a row. */
      if (offset == prev_offset) continue;
      (*reg_offsets++) = prev_offset = offset;
      (*reg_count)++;
    }
  }
}

void FlushDecRegisters(const void* dwl, i32 core_id, u32* regs) {
  u32 reg_count = MAX_REG_COUNT;
  u32 reg_offsets[MAX_REG_COUNT] = {0};

#ifdef TRACE_START_MARKER
  /* write ID register to trigger logic analyzer */
  DWLWriteReg(dwl, dec_cont->core_id, 0x00, ~0);
#endif

  GetDecRegNumbers(&reg_count, reg_offsets, /* Writable regs only */1);
  for (u32 i = 0; i < reg_count; i++)
    DWLWriteReg(dwl, core_id, reg_offsets[i] * 4, regs[reg_offsets[i]]);
}

void RefreshDecRegisters(const void* dwl, i32 core_id, u32* regs) {
  u32 reg_count = MAX_REG_COUNT;
  u32 reg_offsets[MAX_REG_COUNT] = {0};

  GetDecRegNumbers(&reg_count, reg_offsets, /* All regs */0);
  for (u32 i = 0; i < reg_count; i++)
    regs[reg_offsets[i]] = DWLReadReg(dwl, core_id, reg_offsets[i] * 4);
}
