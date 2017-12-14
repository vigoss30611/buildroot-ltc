/* Copyright 2012 Google Inc. All Rights Reserved. */
/* Author: mkarki@google.com (Matti Kärki) */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "internal_test.h"

#define REG_DUMP_FILE "swreg.trc"
static FILE* reg_dump = NULL;

/*------------------------------------------------------------------------------
    Function name   : InternalTestInit
    Description     : Initialize InternalTest

    Return type     : -

    Argument        : -
------------------------------------------------------------------------------*/
void InternalTestInit() {
  if (reg_dump != NULL) {
    return;
  }

  reg_dump = fopen(REG_DUMP_FILE, "a+");
  if (NULL == reg_dump) {
    printf("struct DWL: failed to open: %s\n", REG_DUMP_FILE);
  }
}

/*------------------------------------------------------------------------------
    Function name   : InternalTestFinalize
    Description     : Cleans up InternalTest

    Return type     : -

    Argument        : -
------------------------------------------------------------------------------*/
void InternalTestFinalize() {
  if (reg_dump != NULL) {
    fclose(reg_dump);
    reg_dump = NULL;
  }
}

/*------------------------------------------------------------------------------
    Function name   : InternalTestDumpReadSwReg
    Description     :

    Return type     : -

    Argument        : i32 core_id, u32 sw_reg, u32 value, u32* sw_regs
------------------------------------------------------------------------------*/
void InternalTestDumpReadSwReg(i32 core_id, u32 sw_reg, u32 value,
                               u32* sw_regs) {
  if (core_id == 0) {
    /* status register dump */
    if (sw_reg == 1) {
      /* write out the interrupt status bit */
      u32 tmp_val = (value >> 11) & 0xFF;
      if (tmp_val & 0x02) {
        fprintf(reg_dump, "R INTERRUPT STATUS: PICTURE DECODED\n");
      } else if (tmp_val & 0x08) {
        fprintf(reg_dump, "R INTERRUPT STATUS: BUFFER EMPTY\n");
      } else if (tmp_val & 0x10) {
        fprintf(reg_dump, "R INTERRUPT STATUS: ASO DETECTED\n");
      } else if (tmp_val & 0x20) {
        fprintf(reg_dump, "R INTERRUPT STATUS: ERROR DETECTED\n");
      } else if (tmp_val & 0x40) {
        fprintf(reg_dump, "R INTERRUPT STATUS: SLICE DECODED\n");
      } else {
        fprintf(reg_dump, "R SWREG%d:          %08X\n", sw_reg, value);
      }
    }
    /* stream end address dump */
    else if (sw_reg == 12) {
      u32 tmp_val = sw_regs[1];
      /* check if ASO detected */
      if ((tmp_val >> 12) & 0x7F & 0x8) {
        fprintf(reg_dump, "R STREAM END ADDRESS: N.A. (ASO DETECTED)\n");
      }
      /* check if error detected */
      else if ((tmp_val >> 12) & 0x7F & 0x10) {
        fprintf(reg_dump, "R STREAM END ADDRESS: N.A. (ERROR DETECTED)\n");
      }
      /* check if slice detected */
      else if ((tmp_val >> 12) & 0x7F & 0x20) {
        fprintf(reg_dump, "R STREAM END ADDRESS: N.A. (SLICE DECODED)\n");
      } else {
        u32 tmp_val2 = (sw_regs[3] >> 27) & 0x01;
        /* check if RLC mode -> swreg12 contains base address */
        if (tmp_val2) {
          fprintf(reg_dump, "R STREAM END ADDRESS: N.A. (RLC MODE ENABLED)\n");
        }
        /* write the end address */
        else {
          fprintf(reg_dump, "R STREAM END ADDRESS: %u\n", value);
        }
      }
    }
    /* control stream end address dump */
    else if (sw_reg == 27) {
      u32 tmp_val = sw_regs[1];
      /* check if ASO detected */
      if ((tmp_val >> 12) & 0x7F & 0x8) {
        fprintf(reg_dump, "R CTRL STREAM END ADDRESS: N.A. (ASO DETECTED)\n");
      }
      /* check if error detected */
      else if ((tmp_val >> 12) & 0x7F & 0x10) {
        fprintf(reg_dump, "R CTRL STREAM END ADDRESS: N.A. (ERROR DETECTED)\n");
      }
      /* check if slice detected */
      else if ((tmp_val >> 12) & 0x7F & 0x20) {
        fprintf(reg_dump, "R STREAM END ADDRESS: N.A. (SLICE DECODED)\n");
      } else {
        u32 tmp_val2 = sw_regs[3] >> 28;
        /* check if vp8 mode  */
        if (tmp_val2 != 10) {
          fprintf(reg_dump,
                  "R CTRL STREAM END ADDRESS: N.A. (VP8 MODE DISABLED)\n");
        } else {
          fprintf(reg_dump, "R CTRL STREAM END ADDRESS: %u\n", value);
        }
      }
    }
    /* reference cubber return values */
    else if (sw_reg == 52 || sw_reg == 53 || sw_reg == 56) {
      u32 tmp_val2 = sw_regs[51] & 0x80000000;
      if (tmp_val2) {
        if (sw_reg == 52) {
          fprintf(reg_dump, "R REFERENCE BUFFER HIT SUM: %u\n",
                  (value >> 16) & 0xFFFF);
          fprintf(reg_dump, "R REFERENCE BUFFER INTRA SUM: %u\n",
                  value & 0xFFFF);
        } else if (sw_reg == 53) {
          fprintf(reg_dump, "R REFERENCE BUFFER Y_MV SUM: %u\n",
                  value & 0x3FFFFF);
        } else if (sw_reg == 56) {
          /*u32 tmp_val = sw_regs[1];*/
          fprintf(reg_dump, "R REFERENCE BUFFER TOP SUM: %u\n",
                  (value >> 16) & 0xFFFF);
          fprintf(reg_dump, "R REFERENCE BUFFER BOTTOM SUM: %u\n",
                  value & 0xFFFF);
        }
      } else {
        if (sw_reg == 52) {
          fprintf(reg_dump, "R REFERENCE BUFFER HIT SUM: N.A.\n");
          fprintf(reg_dump, "R REFERENCE BUFFER INTRA SUM: N.A.\n");
        } else if (sw_reg == 53) {
          fprintf(reg_dump, "R REFERENCE BUFFER Y_MV SUM: N.A.\n");
        } else if (sw_reg == 56) {
          u32 tmp_val = sw_regs[1];
          fprintf(reg_dump, "R REFERENCE BUFFER TOP SUM: N.A.\n");
          fprintf(reg_dump, "R REFERENCE BUFFER BOTTOM SUM: N.A.\n");
        }
      }
    } else if (sw_reg == 60) {
      u32 tmp_val = (value >> 12) & 0xFF;
      if (tmp_val & 0x01) {
        fprintf(reg_dump, "R PP INTERRUPT STATUS: PICTURE PROCESSED\n");
      }
    }
    fflush(reg_dump);
  }
}

/*------------------------------------------------------------------------------
    Function name   : InternalTestDumpWriteSwReg
    Description     :

    Return type     : -

    Argument        : i32 core_id, u32 sw_reg, u32 value, u32* sw_regs
------------------------------------------------------------------------------*/
void InternalTestDumpWriteSwReg(i32 core_id, u32 sw_reg, u32 value,
                                u32* sw_regs) {
  if (core_id == 0) {
    u32 tmp_val = (sw_regs[3] >> 27) & 0x01;
    u32 tmp_val2 = sw_regs[3] >> 28;

    if (sw_reg == 12) {
      /* check if RLC mode -> swreg12 contains base address */
      if (tmp_val) {
        fprintf(reg_dump,
                "-\n_w STREAM START ADDRESS: N.A. (RLC MODE ENABLED)\n");
      } else {
        fprintf(reg_dump, "-\n_w STREAM START ADDRESS: %u\n", value);
      }

    } else if (sw_reg == 27) {
      /* check if vp8/webp mode */
      if (tmp_val2 != 10) {
        fprintf(reg_dump,
                "W CTRL STREAM START ADDRESS: N.A. (VP8 MODE DISABLED)\n");
      } else {
        fprintf(reg_dump, "W CTRL STREAM START ADDRESS: %u\n", value);
      }
    }
    fflush(reg_dump);
  }
}
