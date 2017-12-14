/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef REGDRV_H
#define REGDRV_H

#include "basetype.h"

#define DEC_HW_ALIGN_MASK 0x0F

#define DEC_HW_IRQ_RDY 0x01
#define DEC_HW_IRQ_BUS 0x02
#define DEC_HW_IRQ_BUFFER 0x04
#define DEC_HW_IRQ_ASO 0x08
#define DEC_HW_IRQ_ERROR 0x10
#define DEC_HW_IRQ_TIMEOUT 0x40

enum HwIfName {
/* include script-generated part */
#include "8170enum.h"
  HWIF_DEC_IRQ_STAT,
  HWIF_PP_IRQ_STAT,
  HWIF_LAST_REG,
};

/* { SWREG, BITS, POSITION, WRITABLE } */
static const u32 hw_dec_reg_spec[HWIF_LAST_REG + 1][4] = {
/* include script-generated part */
#include "8170table.h"
    /* HWIF_DEC_IRQ_STAT */ {1, 7, 12, 0},
    /* HWIF_PP_IRQ_STAT */ {60, 2, 12, 0},
    /* dummy entry */ {0, 0, 0, 0}};



/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

void SetDecRegister(u32* reg_base, u32 id, u32 value);
u32 GetDecRegister(const u32* reg_base, u32 id);
void FlushDecRegisters(const void* dwl, i32 core_id, u32* regs);
void RefreshDecRegisters(const void* dwl, i32 core_id, u32* regs);

#ifdef USE_64BIT_ENV
#define SET_ADDR_REG(reg_base, REGBASE, addr) do {\
    SetDecRegister((reg_base), REGBASE##_LSB, (u32)(addr));  \
    SetDecRegister((reg_base), REGBASE##_MSB, (u32)((addr) >> 32)); \
  } while (0)

#define SET_ADDR_REG2(reg_base, lsb, msb, addr) do {\
    SetDecRegister((reg_base), (lsb), (u32)(addr));  \
    SetDecRegister((reg_base), (msb), (u32)((addr) >> 32)); \
  } while (0)
#else
#define SET_ADDR_REG(reg_base, REGBASE, addr) do {\
    SetDecRegister((reg_base), REGBASE##_LSB, (u32)(addr));  \
  } while (0)
#define SET_ADDR_REG2(reg_base, lsb, msb, addr) do {\
    SetDecRegister((reg_base), (lsb), (u32)(addr));  \
    SetDecRegister((reg_base), (msb), 0); \
  } while (0)
#endif

#endif /* #ifndef REGDRV_H */
