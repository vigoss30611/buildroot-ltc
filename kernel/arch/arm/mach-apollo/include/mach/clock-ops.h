#ifndef __CLOCK_OPS_H__
#define __CLOCK_OPS_H__

#define ENABLE             1
#define DISABLE            0

/* pll */
#define APLL            0
#define DPLL            1
#define EPLL            2
#define VPLL            3

#define REFERENCE_CLK   1
#define PLL_OUTPUT      0

#define PLL_EXT_OSC_CLK 1
#define PLL_XEXTCLK     0

#define UNLOAD          0
#define LOAD            1

#define GATE_ENABLE     0
#define GATE_DISABLE    1

/* cpu */
#define CPU_DIVIDER_CLK 1
#define CPU_APLL_CLK    0

/* bus */
#define BUS1                     0
#define BUS2                     1
#define BUS3                     2
#define BUS4                     3
#define BUS5                     4
#define BUS6                     5
#define BUS7                     6
#define IDS0_EITF_CLK_SRC        7           /* 0x1a */    
#define IDS0_OSD_CLK_SRC         8           /* 0x1b */
#define IDS0_TVIF_CLK_SRC        9           /* 0x1c */
#define IDS1_EITF_CLK_SRC        10          /* 0x1d */
#define IDS1_OSD_CLK_SRC         11          /* 0x1e */
#define IDS1_TVIF_CLK_SRC        12          /* 0x1f */
#define MIPI_DPHY_CON_CLK_SRC    13          /* 0x20    mipi dphy configuration clk */
#define MIPI_DPHY_REF_CLK_SRC    14          /* 0x21    mipi dphy reference clk     */
#define MIPI_DPHY_PIXEL_CLK_SRC  15          /* 0x22    mipi dphy pixel clk         */
#define ISP_OSD_CLK_SRC          16          /* 0x23    isp osd clk                 */
#define SD2_CLK_SRC              17          /* 0x24    sd2 clk                     */
#define SD1_CLK_SRC              18          /* 0x25    sd1 clk                     */
#define SD0_CLK_SRC              19          /* 0x26    sd0 clk                     */
#define NFECC_CLK_SRC            20          /* 0x27    nfecc clk                   */
#define HDMI_CLK_SRC             21          /* 0x28    hdmi clk                    */
#define UART_CLK_SRC             22          /* 0x29    uart clk                    */
#define SPDIF_CLK_SRC            23          /* 0x2a    spdif clk                   */
#define AUDIO_CLK_SRC            24          /* 0x2b    audio clk                   */
#define USBREF_CLK_SRC           25          /* 0x2c    usbref clk                  */
#define CLKOUT0_CLK_SRC          26          /* 0x2d    clk out 0 clk               */
#define CLKOUT1_CLK_SRC          27          /* 0x2e    clk out 1 clk               */
#define SATAPHY_CLK_SRC          28          /* 0x2f    sata phy clk                */
#define CAMO_CLK_SRC             29          /* 0x30    camo clk                    */
#define DDRPHY_CLK_SRC           30          /* 0x31    ddrphy clk                  */
#define CRYPTO_CLK_SRC           31          /* 0x32    crypto clk                  */
#define TSC_CLK_SRC              32          /* 0x33    tsc clk                     */
#define DMIC_CLK_SRC             33          /* 0x34    dmic clk                    */
#define APB_CLK_SRC             35

#define OSC_CLK         4
#define XEXTCLK         5

#define CLK_ERROR       1

#define DEFAULT_PLL_R   0

#define FOUT_CLK_MAX    2000000000
#define FOUT_CLK_MIN    125000000

#define FREF_CLK_MAX    50000000
#define FREF_CLK_MIN    10000000

#define FVCO_CLK_MAX    2000000000
#define FVCO_CLK_MIN    1000000000

/*-------------------------------------------------------------------------------
  -----------------------------------------------------------------------------*/

struct pll_clk_info{
    uint32_t bypass;             /* ENALBE or DISABLE*/
	uint32_t od;                 /* 0 ~ 3 */
	uint32_t r;                  /* 0 : 4       10MHz << Fref/NR << 50MHz */
	uint32_t f;                  /* 0 : 6       as default r = 0, f should be 41 ~ 82 */
	uint32_t out_sel;            /* REFERENCE_CLK or PLL_OUTPUT */
	uint32_t src;                /* PLL_EXT_OSC_CLK or PLL_XEXTCLK */
};

struct bus_and_dev_clk_info{
    uint32_t dev_id; 
    uint32_t nco_en;            /* NCO en  should be ENABLE or DISABLE */
    uint32_t clk_src;           /* APLL, DPLL, EPLL, VPLL, OSC_CLK, XEXTCLK */
    uint32_t nco_value;         /* nco is 8 bit effective  */
    uint32_t clk_divider;       /* clk divider  [4 : 0] */
	uint32_t nco_disable;
};

struct cpu_clk_info{
    uint32_t cpu_clk[8];        /* cpu clk divider sequence */
	uint32_t axi_clk[8];        /* axi clk divider sequence */
	uint32_t apb_clk[8];        /* apb clk divider sequence */
	uint32_t axi_cpu[8];        /* cpu axi and cpu clk enable sequence */     
	uint32_t apb_cpu[8];        /* cpu apb and cpu clk enable sequence*/
	uint32_t apb_axi[8];        /* cpu apb and cpu axi clk enable sequence*/
	uint32_t seq_len;           /* sequence bit len, should be not more than 64bit*/
    uint32_t cpu_co;            /* cpu coefficent like 1, 2, ..   if cpu_co == 0 we take the struct as axi clk*/
	uint32_t ratio;             /* cpu(16 : 31) : apb(0 : 15)   gerenally apb multi axi multi cpu */
};

struct bus_freq_range {
    uint32_t dev_id;
    uint32_t max;
    uint32_t min;
};


/*****************************************************************************************************************
 ***************************************************************************************************************/

/*
 * if testing on fpga, need to set this bit
 * then, need to set FPGA_CPU_CLK, FPGA_CPU_APB_CLK_RATIO, FPGA_CPU_APB_CLK_RATIO
 * and FPGA_EXTEND_CLK 
 * cpu clk uses the FPGA_CPU_CLK, 
 * all the bus and module through bus use FPGA_BUS_CLK
 * apb dev use FPGA_APB use FPGA_APB_CLK
 * some special clk and modules use FPGA_EXTEND_CLK
 */

//#define IMAPX800_FPGA_TEST      1             /* 1, means using on fpga */

#if (defined(CONFIG_IMAPX800_FPGA_PLATFORM) || defined(CONFIG_IMAPX9_FPGA_PLATFORM))
/* fpga cpu clk 
 *  
 * define the cpu clk, is clk1
 * */
#define FPGA_CPU_CLK            CONFIG_FPGA_CPU_CLK      /* 40 MHz */
#define FPGA_EXTEND_CLK         CONFIG_FPGA_EXTEND_CLK      /* 20 MHz */

#define FPGA_CPU_BUS_CLK_RATIO  2             /* cpu clk : bus clk   1 : 2 */
#define FPGA_CPU_APB_CLK_RATIO  4             /* cpu clk : apb clk   1:  4 */

#define FPGA_BUS_CLK            (CONFIG_FPGA_CPU_CLK / FPGA_CPU_BUS_CLK_RATIO)
#define FPGA_APB_CLK            (CONFIG_FPGA_CPU_CLK / FPGA_CPU_APB_CLK_RATIO)
#endif

#endif /*__CLOCK_OPS_H__ */

