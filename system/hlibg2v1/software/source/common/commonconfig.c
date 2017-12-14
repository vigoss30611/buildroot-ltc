/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "commonconfig.h"
#include "dwl.h"
#include "deccfg.h"
#include "regdrv.h"

#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

/* These are globals, so that test code can meddle with them. On production
 * they will always retain their values. */
u32 dec_stream_swap = HANTRODEC_STREAM_SWAP;
u32 dec_pic_swap = HANTRODEC_STREAM_SWAP;
u32 dec_dirmv_swap = HANTRODEC_STREAM_SWAP;
u32 dec_tab0_swap = 0;
u32 dec_tab1_swap = 0;
u32 dec_tab2_swap = 0;
u32 dec_tab3_swap = 0;
u32 dec_rscan_swap = 0;
u32 dec_comp_tab_swap = HANTRODEC_STREAM_SWAP;
u32 dec_burst_length = DEC_X170_BUS_BURST_LENGTH;
u32 dec_bus_width = DEC_X170_BUS_WIDTH;
#ifdef DEC_X170_REFBU_SEQ
u32 dec_apf_treshold = DEC_X170_REFBU_NONSEQ / DEC_X170_REFBU_SEQ;
#else
u32 dec_apf_treshold = DEC_X170_REFBU_NONSEQ;
#endif /* DEC_X170_REFBU_SEQ */
u32 dec_apf_disable = DEC_X170_APF_DISABLE;

u32 dec_clock_gating = DEC_X170_INTERNAL_CLOCK_GATING;
u32 dec_clock_gating_runtime = DEC_X170_INTERNAL_CLOCK_GATING_RUNTIME;

u32 dec_ref_double_buffer = HANTRODEC_INTERNAL_DOUBLE_REF_BUFFER;

u32 dec_timeout_cycles = HANTRODEC_TIMEOUT_OVERRIDE;
u32 dec_axi_id_rd = DEC_X170_AXI_ID_R;
u32 dec_axi_id_rd_unique_enable = DEC_X170_AXI_ID_R_E;
u32 dec_axi_id_wr = DEC_X170_AXI_ID_W;
u32 dec_axi_id_wr_unique_enable = DEC_X170_AXI_ID_W_E;

void SetCommonConfigRegs(u32 *regs) {
  DWLHwConfig hw_config;
  (void)DWLmemset(&hw_config, 0, sizeof(hw_config));
  DWLReadAsicConfig(&hw_config);

  /* Check that the register count define DEC_X170_REGISTERS is
   * big enough for the defined HW registers in hw_dec_reg_spec */
  ASSERT(hw_dec_reg_spec[HWIF_LAST_REG - 3][0] < DEC_X170_REGISTERS);

  /* set parameters defined in deccfg.h */
  SetDecRegister(regs, HWIF_DEC_STRM_SWAP, dec_stream_swap);
  SetDecRegister(regs, HWIF_DEC_PIC_SWAP, dec_pic_swap);
  SetDecRegister(regs, HWIF_DEC_DIRMV_SWAP, dec_dirmv_swap);
  SetDecRegister(regs, HWIF_DEC_TAB0_SWAP, dec_tab0_swap);
  SetDecRegister(regs, HWIF_DEC_TAB1_SWAP, dec_tab1_swap);
  SetDecRegister(regs, HWIF_DEC_TAB2_SWAP, dec_tab2_swap);
  SetDecRegister(regs, HWIF_DEC_TAB3_SWAP, dec_tab3_swap);
  SetDecRegister(regs, HWIF_DEC_RSCAN_SWAP, dec_rscan_swap);
  SetDecRegister(regs, HWIF_DEC_COMP_TABLE_SWAP, dec_comp_tab_swap);
  SetDecRegister(regs, HWIF_DEC_BUSWIDTH, dec_bus_width);
  SetDecRegister(regs, HWIF_DEC_MAX_BURST, dec_burst_length);
  if (hw_config.double_buffer_support == DOUBLE_BUFFER_SUPPORTED)
    SetDecRegister(regs, HWIF_DEC_REFER_DOUBLEBUFFER_E, dec_ref_double_buffer);
  else
    SetDecRegister(regs, HWIF_DEC_REFER_DOUBLEBUFFER_E, 0);
#if (HWIF_APF_SINGLE_PU_MODE)
  SetDecRegister(regs, HWIF_APF_SINGLE_PU_MODE, 1);
  SetDecRegister(regs, HWIF_APF_DISABLE, 1);
  SetDecRegister(regs, HWIF_APF_THRESHOLD, 0);
#elif(DEC_X170_APF_DISABLE)
  SetDecRegister(regs, HWIF_APF_SINGLE_PU_MODE, 0);
  SetDecRegister(regs, HWIF_APF_DISABLE, 1);
  SetDecRegister(regs, HWIF_APF_THRESHOLD, 0);
#else
  {
    u32 apf_tmp_threshold = dec_apf_treshold;
    SetDecRegister(regs, HWIF_APF_DISABLE, dec_apf_disable);
    if (apf_tmp_threshold > 63) apf_tmp_threshold = 63;
    SetDecRegister(regs, HWIF_APF_THRESHOLD, apf_tmp_threshold);
  }
#endif

#if (DEC_X170_USING_IRQ == 0)
  SetDecRegister(regs, HWIF_DEC_IRQ_DIS, 1);
#else
  SetDecRegister(regs, HWIF_DEC_IRQ_DIS, 0);
#endif

  SetDecRegister(regs, HWIF_DEC_OUT_EC_BYPASS, 0);
  SetDecRegister(regs, HWIF_APF_ONE_PID, 0);

  /* Clock gating parameters */
  SetDecRegister(regs, HWIF_DEC_CLK_GATE_E, dec_clock_gating);
  SetDecRegister(regs, HWIF_DEC_CLK_GATE_IDLE_E, dec_clock_gating_runtime);

  /* set AXI RW IDs */
  SetDecRegister(regs, HWIF_DEC_AXI_RD_ID_E,
      (dec_axi_id_rd_unique_enable & 0x1));
  SetDecRegister(regs, HWIF_DEC_AXI_WD_ID_E,
      (dec_axi_id_wr_unique_enable & 0x1));
  SetDecRegister(regs, HWIF_DEC_AXI_RD_ID,
      (dec_axi_id_rd & 0xFFU));
  SetDecRegister(regs, HWIF_DEC_AXI_WR_ID,
      (dec_axi_id_wr & 0xFFU));

  /* Set timeouts. Value of 0 implies use of hardware default. */
  SetDecRegister(regs, HWIF_TIMEOUT_OVERRIDE_E, dec_timeout_cycles ? 1 : 0);
  SetDecRegister(regs, HWIF_TIMEOUT_CYCLES, dec_timeout_cycles);
}
