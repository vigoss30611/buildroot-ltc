#ifndef __IMAPX_CLOCK__
#define __IMAPX_CLOCK__

#define IMAP_PA_SYSMGR_CLK        (IMAP_SYSMGR_BASE + 0xa000)

/* pll */
#define PLL_PARAMETER_LOW(x)         (0x000 + (x) * 0x18)    /* */ 
#define PLL_PARAMETER_HIGH(x)        (0x004 + (x) * 0x18)    /* */ 
#define PLL_SRC_STATUS(x)            (0x008 + (x) * 0x18)    /* */ 
#define PLL_LOAD_AND_GATE(x)         (0x00c + (x) * 0x18)    /* */
/* TODO
   it is exchanged */
#define PLL_LOCKED_STATUS(x)         (0x014 + (x) * 0x18)    /* */ 
#define PLL_ENABLE(x)                (0x010 + (x) * 0x18)    /* */ 

/* apll */
#define APLL_PARAMETER_LOW           (0x000)    /* apll parameter low */
#define APLL_PARAMETER_HIGH          (0x004)    /* apll parameter high */
#define APLL_SRC_STATUS              (0x008)    /* apll src & status */
#define APLL_LOAD_AND_GATE           (0x00c)    /* apll load parameter and output gate */
#define APLL_LOCKED_STATUS           (0x014)    /* apll load status */
#define APLL_ENABLE                  (0x010)    /* apll enable */

/* dpll */
#define DPLL_PARAMETER_LOW           (0x018)    /* dpll parameter low */
#define DPLL_PARAMETER_HIGH          (0x01c)    /* dpll parameter high */
#define DPLL_SRC_STATUS              (0x020)    /* dpll src & status */
#define DPLL_LOAD_AND_GATE           (0x024)    /* dpll load parameter and output gate */
#define DPLL_LOCKED_STATUS           (0x02c)    /* dpll load status */
#define DPLL_ENABLE                  (0x028)    /* dpll enable */

/* epll */
#define EPLL_PARAMETER_LOW           (0x030)    /* epll parameter low */
#define EPLL_PARAMETER_HIGH          (0x034)    /* epll parameter high */
#define EPLL_SRC_STATUS              (0x038)    /* epll src & status */
#define EPLL_LOAD_AND_GATE           (0x03c)    /* epll load parameter and output gate */
#define EPLL_LOCKED_STATUS           (0x044)    /* epll load status */
#define EPLL_ENABLE                  (0x040)    /* epll enable */

/* vpll */
#define VPLL_PARAMETER_LOW           (0x048)    /* vpll parameter low */
#define VPLL_PARAMETER_HIGH          (0x04c)    /* vpll parameter high */
#define VPLL_SRC_STATUS              (0x050)    /* vpll src & status */
#define VPLL_LOAD_AND_GATE           (0x054)    /* vpll load parameter and output gate */
#define VPLL_LOCKED_STATUS           (0x05c)    /* vpll load status */
#define VPLL_ENABLE                  (0x058)    /* vpll enable */

/* pll src */
#define PLL_OSRC        1      /* output clock selection */
#define PLL_SRC         0      /* pll reference clock source */

#define PLL_BYPASS      14       
#define PLL_OD          12     /* 2 ^ od         [13:12] */
#define PLL_R           7      /* NR  2 ^ R      [11:7]  */
#define PLL_F           0      /* NF  2 * 2 ^ F  [6:0]   */

/* pll load parameter & output gate */
#define PLL_LOAD_PA     1     /* load parameter */
#define PLL_OUT_GATE    0     /* output clock gate enable */

/*--------------------------------------------------------------------------------------------------
  ------------------------------------------------------------------------------------------------*/
/* cpu */
#define CPU_CLK_PARA_EN              (0x064)    /* cpu clk parameter enable  */
#define CPU_CLK_OUT_SEL              (0x068)    /* cpu clk output selection  */
#define CPU_CLK_DIVIDER_PA           (0x06c)    /* cpu clk divider parameter */

#define CPU_CLK_DIVIDER_SEQ(x)       (0x070 + x * 0x4)  /* cpu clk divider sequence     */
#define CPU_AXI_CLK_DIVIDER_SEQ(x)   (0x090 + x * 0x4)  /* cpu axi clk divider sequence */
#define CPU_APB_CLK_DIVIDER_SEQ(x)   (0x0b0 + x * 0x4)  /* cpu axi clk divider sequence */
#define CPU_AXI_A_CPU_CLK_EN_SEQ(x)  (0x0d0 + x * 0x4)  /* axi and cpu clk en sequence  */
#define CPU_APB_A_CPU_CLK_EN_SEQ(x)  (0x0f0 + x * 0x4)  /* apb and cpu clk en sequence  */
#define CPU_APB_A_AXI_CLK_EN_SEQ(x)  (0x110 + x * 0x4)  /* apb and axi clk en sequence  */

#define SOFT_EVENT1_REG              (0xc18)    /* soft event1 it can be used to reset apll fr, and check whether set done */

/* cpu divider parameter */
#define CPU_DIVIDER_EN  7
#define CPU_DIVLEN      0    /* 0 ~ 5 */

/*--------------------------------------------------------------------------------------------------
  ------------------------------------------------------------------------------------------------*/
/* bus clock */
#define BUS_CLK_SRC(x)               (0x130 + (x) * 0x10)  /* bus clock source register    */
#define BUS_CLK_DIVI_R(x)            (0x134 + (x) * 0x10)  /* bus clock divider ratio      */
#define BUS_CLK_NCO(x)               (0x138 + (x) * 0x10)  /* bus clock NCO value register */
#define BUS_CLK_GATE(x)              (0x13c + (x) * 0x10)  /* bus clock gated register     */

/* bus clk src */
#define BUS_NCO         3
#define BUS_CLK_SRC_BIT     0

/*--------------------------------------------------------------------------------------------------
  ------------------------------------------------------------------------------------------------*/


#endif /*__IMAPX800_CLOCK___*/
