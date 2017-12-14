/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Description :  Encoder SW/HW interface register definitions
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. External compiler flags
    3. Module defines

------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "encswhwregisters.h"
#include "enccommon.h"
#include "ewl.h"

/* NOTE: Don't use ',' in descriptions, because it is used as separator in csv
 * parsing. */
const regField_s asicRegisterDesc[] =
{
#include "registertable.h"
};

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/* Define this to print debug info for every register write.
#define DEBUG_PRINT_REGS */

/*------------------------------------------------------------------------------

    h2_EncAsicSetRegisterValue

    Set a value into a defined register field

------------------------------------------------------------------------------*/
void h2_EncAsicSetRegisterValue(u32 *regMirror, regName name, u32 value)
{
  const regField_s *field;
  u32 regVal;

  field = &asicRegisterDesc[name];

#ifdef DEBUG_PRINT_REGS
  printf("EncAsicSetRegister 0x%2x  0x%08x  Value: %10d  %s\n",
         field->base, field->mask, value, field->description);
#endif

  /* Check that value fits in field */
  ASSERT(field->name == name);
  ASSERT(((field->mask >> field->lsb) << field->lsb) == field->mask);
  ASSERT((field->mask >> field->lsb) >= value);
  ASSERT(field->base < ASIC_SWREG_AMOUNT * 4);

  /* Clear previous value of field in register */
  regVal = regMirror[field->base / 4] & ~(field->mask);

  /* Put new value of field in register */
  regMirror[field->base / 4] = regVal | ((value << field->lsb) & field->mask);
}
/*------------------------------------------------------------------------------

    h2_EncAsicGetRegisterValue

    Get an unsigned value from the ASIC registers

------------------------------------------------------------------------------*/
u32 h2_EncAsicGetRegisterValue(const void *ewl, u32 *regMirror, regName name)
{
  const regField_s *field;
  u32 value;

  field = &asicRegisterDesc[name];

  ASSERT(field->base < ASIC_SWREG_AMOUNT * 4);

  value = regMirror[field->base / 4] = h2_EWLReadReg(ewl, field->base);
  value = (value & field->mask) >> field->lsb;

  return value;
}


