#ifndef __IMAPX910_CLK_H__
#define __IMAPX910_CLK_H__

#include <linux/types.h>
#include <linux/clk-provider.h>
#include <linux/clk-private.h>
#include <mach/imap-iomap.h>
#include <linux/io.h>

#define TAG	"Infotm Clk: "
#ifdef CLK_DBG
#define clk_dbg(x...) printk(TAG""x)
#else
#define clk_dbg(...)  do {} while (0)
#endif
#define clk_err(x...) printk(TAG""x)

#define IMAP_PA_SYSMGR_CLK			(IMAP_SYSMGR_BASE + 0xa000)
#define IMAP_CLK_ADDR_OFF			(IO_ADDRESS(IMAP_PA_SYSMGR_CLK))

#define __clk_readl(x)          __raw_readl((IMAP_CLK_ADDR_OFF + (x)))
#define __clk_writel(val, x)    __raw_writel(val, (IMAP_CLK_ADDR_OFF + (x)))

#define IMAP_OSC_CLK	24000000

#define DEFAULT_RATE	0

struct clk_collect{
	struct list_head list;
	struct clk *clk;
};

struct clk_init_enable {
	char * name;
	uint32_t rate;
	int state;
};

#define CLK_INIT_CONF(_name, _rate, _state) \
{\
	.name = _name,				\
	.rate = _rate,				\
	.state = _state,			\
}

#define SOFT_EVENT1_REG              (0xc18)

#define ENABLE	1
#define DISABLE	0

#define CPU_DIVIDER_CLK 1
#define CPU_APLL_CLK    0

enum pll_type {
	OSC_CLK,
	APLL,
	DPLL,
	EPLL,
	VPLL,
	XEXTCLK,
	DEFAULT_SRC = 0xff,
};

#define DEV_NUM		30
#define PLL_NUM		4
#define TOT_NUM		(DEV_NUM + PLL_NUM)
struct clk_restore {
	uint32_t clkgate[DEV_NUM];
	uint32_t clknco[DEV_NUM];
	uint32_t clksrc[DEV_NUM];
	uint32_t clkdiv[DEV_NUM];
	uint32_t pllval[PLL_NUM];
};

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


#define PLL_REFDIV(x)         (0x000 + (x) * 0x30)
#define PLL_FBDIV_LOW(x)        (0x004 + (x) * 0x30)
#define PLL_FBDIV_HIGH(x)        (0x008 + (x) * 0x30)
#define PLL_POSTDIV(x)        (0x00c + (x) * 0x30)
#define PLL_SRC_STATUS(x)            (0x024 + (x) * 0x30)
#define PLL_LOAD_AND_GATE(x)         (0x01c + (x) * 0x30)
#define PLL_LOCKED_STATUS(x)         (0x028 + (x) * 0x30)
#define PLL_ENABLE(x)                (0x020 + (x) * 0x30)

#define APLL_PARAMETER_LOW           (0x000)
#define APLL_PARAMETER_HIGH          (0x004)
#define APLL_SRC_STATUS              (0x024)
#define APLL_LOAD_AND_GATE           (0x01c)
#define APLL_LOCKED_STATUS           (0x028)
#define APLL_ENABLE                  (0x020)

#define UNLOAD          0
#define LOAD            1

#define FREF_CLK_MAX    50000000
#define FREF_CLK_MIN    10000000

#define FVCO_CLK_MAX    2000000000
#define FVCO_CLK_MIN    1000000000

#define FOUT_CLK_MAX    2000000000
#define FOUT_CLK_MIN    125000000

#define REFERENCE_CLK   (0 << 1)
#define PLL_OUTPUT      (1 << 1)

#define PLL_OUTENABLE		1
#define PLL_OUTDISABLE		0

#if 0
struct pll_clk_info {
	uint32_t bypass;             /* ENALBE or DISABLE*/
	uint32_t od;                 /* 0 ~ 3 */
	uint32_t r;                  /* 0 : 4       10MHz << Fref/NR << 50MHz */
	uint32_t f;                  /* as default r = 0, f should be 41 ~ 82 */
	uint32_t out_sel;            /* REFERENCE_CLK or PLL_OUTPUT */
	uint32_t src;                /* PLL_EXT_OSC_CLK or PLL_XEXTCLK */
};
#endif

struct pll_clk_info {
	uint32_t bypass;             /* ENALBE or DISABLE*/
	uint32_t refdiv;
	uint32_t fbdiv;
	uint32_t postdiv1;
	uint32_t postdiv2;
	uint32_t out_sel;
	uint32_t src;
};

#define GET_BYPASS(x)		((x >> PLL_BYPASS) & 0x1)
#define GET_OD(x)			((x >> PLL_OD) & 0x3)
#define GET_R(x)			((x >> PLL_R) & 0xf)
#define GET_F(x)			((x >> PLL_F) & 0x7f)

struct imapx_pll_clk {
	struct clk_hw		hw;
	enum pll_type		type;
	struct pll_clk_info *params;
	spinlock_t          *lock;
};

#define to_clk_pll(_hw) container_of(_hw, struct imapx_pll_clk, hw)

#define BUS_FREQ_RANGE(_name, _max, _min) \
{\
	.name = _name,	\
	.max = _max,	\
	.min = _min,	\
}

struct bus_freq_range {
	char name[32];
	uint32_t max;
	uint32_t min;
};

struct imapx_bus_clk {
	struct clk_hw			hw;
	struct bus_freq_range	*range;
};
#define to_clk_bus(_hw) container_of(_hw, struct imapx_bus_clk, hw)

/* bus clock */
#define BUS_CLK_SRC(x)               (0x130 + (x) * 0x10)
#define BUS_CLK_DIVI_R(x)            (0x134 + (x) * 0x10)
#define BUS_CLK_NCO(x)               (0x138 + (x) * 0x10)
#define BUS_CLK_GATE(x)              (0x13c + (x) * 0x10)

#define BUS_NCO				3
#define BUS_CLK_SRC_BIT     0

#define CLK_NCO_MODE		(0x1 << 3)

enum dev_type {
	BUS1,
	BUS3 = 2,
	BUS4,
	BUS6 = 5,
	DSP_CLK_SRC,
	IDS0_EITF_CLK_SRC,
	IDS0_OSD_CLK_SRC,
	MIPI_DPHY_CON_CLK_SRC = 13,
	SD2_CLK_SRC = 17,
	SD1_CLK_SRC,
	SD0_CLK_SRC,
	UART_CLK_SRC = 22,
	SSP_CLK_SRC,
	AUDIO_CLK_SRC,
	USBREF_CLK_SRC,
	CAMO_CLK_SRC,
	CLKOUT1_CLK_SRC,
	MIPI_DPHY_PIXEL_CLK_SRC = 29,
	DDRPHY_CLK_SRC,
	CRYPTO_CLK_SRC,
	TSC_CLK_SRC,
	APB_CLK_SRC = 35,
};

#define DEV_CLK_INFO(_dev_id, _nco_en, _clk_src,\
		_nco_value, _clk_divider, disable) \
{\
	.dev_id = _dev_id, \
	.nco_en = _nco_en, \
	.clk_src = _clk_src, \
	.nco_value = _nco_value, \
	.clk_divider = _clk_divider, \
	.nco_disable = disable, \
}

struct bus_and_dev_clk_info {
	uint32_t dev_id;
	uint32_t nco_en;
	uint32_t clk_src;
	uint32_t nco_value;
	uint32_t clk_divider;
	uint32_t nco_disable;
};

struct imapx_dev_clk {
	struct clk_hw				hw;
	enum dev_type				type;
	struct bus_and_dev_clk_info	*info;
	spinlock_t					*lock;
};
#define to_clk_dev(_hw) container_of(_hw, struct imapx_dev_clk, hw)

struct imapx_vitual_clk {
	struct clk_hw			hw;
};

struct imapx_fixed_clk {
	struct clk_hw			hw;
	uint32_t fixed_div;
};

#define to_clk_fixed(_hw) container_of(_hw, struct imapx_fixed_clk, hw)

#define CPU_CLK_PARA_EN              (0x064)    /* cpu clk parameter enable  */
#define CPU_CLK_OUT_SEL              (0x068)    /* cpu clk output selection  */
#define CPU_CLK_DIVIDER_PA           (0x06c)    /* cpu clk divider parameter */

/* cpu clk divider sequence     */
#define CPU_CLK_DIVIDER_SEQ(x)       (0x070 + x * 0x4)
/* cpu axi clk divider sequence */
#define CPU_AXI_CLK_DIVIDER_SEQ(x)   (0x090 + x * 0x4)
/* cpu axi clk divider sequence */
#define CPU_APB_CLK_DIVIDER_SEQ(x)   (0x0b0 + x * 0x4)
/* axi and cpu clk en sequence  */
#define CPU_AXI_A_CPU_CLK_EN_SEQ(x)  (0x0d0 + x * 0x4)
/* apb and cpu clk en sequence  */
#define CPU_APB_A_CPU_CLK_EN_SEQ(x)  (0x0f0 + x * 0x4)
/* apb and axi clk en sequence  */
#define CPU_APB_A_AXI_CLK_EN_SEQ(x)  (0x110 + x * 0x4)

/* soft event1 it can be used to reset apll fr, and check whether set done */
#define SOFT_EVENT1_REG              (0xc18)

/* cpu divider parameter */
#define CPU_DIVIDER_EN  7
#define CPU_DIVLEN      0    /* 0 ~ 5 */

#define GATE_ENABLE     0
#define GATE_DISABLE    1

struct cpu_clk_info {
	uint32_t cpu_clk[8];        /* cpu clk divider sequence */
	uint32_t axi_clk[8];        /* axi clk divider sequence */
	uint32_t apb_clk[8];        /* apb clk divider sequence */
	/* cpu axi and cpu clk enable sequence */
	uint32_t axi_cpu[8];
	uint32_t apb_cpu[8];        /* cpu apb and cpu clk enable sequence*/
	uint32_t apb_axi[8];        /* cpu apb and cpu axi clk enable sequence*/
	/* sequence bit len, should be not more than 64bit*/
	uint32_t seq_len;
	uint32_t cpu_co;            /* cpu coefficent like 1, 2 */
	uint32_t ratio;             /* gerenally apb multi axi multi cpu */
};

struct imapx_cpu_clk {
	struct clk_hw           hw;
	uint32_t				ratio;
	spinlock_t              *lock;
};

#define to_clk_cpu(_hw) container_of(_hw, struct imapx_cpu_clk, hw)

struct imapx_clk_init {
	char *name;
	char *con_id;
	char *dev_id;
	int type;
};

#define CLK_INIT(_name, _dev_id, _con_id, _type) \
{\
	.name = _name,					\
	.con_id = _con_id,				\
	.dev_id = _dev_id,				\
	.type = _type,					\
}

#define CLK_APB_INIT(_name, _dev_id, _con_id) \
{\
	.name = _name,					\
	.con_id = _con_id,				\
	.dev_id = _dev_id,				\
}

#define CLK_VITUAL_INIT(_name, _dev_id, _con_id) \
{\
	.name = _name,					\
	.con_id = _con_id,				\
	.dev_id = _dev_id,				\
}

#define CLK_CPU_INIT(_name, _dev_id, _con_id) \
{\
	.name = _name,					\
	.con_id = _con_id,				\
	.dev_id = _dev_id,				\
}

/* register clk in bus */
extern struct clk *imapx_bus_clk_register(const char *name,
		const char *parent_names, struct bus_freq_range *range,
		unsigned long flags);
/* register clk about pll */
extern struct clk *imapx_pll_clk_register(const char *name,
		const char *parent_name, enum pll_type type,
		unsigned long flags, spinlock_t *lock);
/* register clk about dev and bus */
extern struct clk *imapx_dev_clk_register(const char *name,
		const char **parent_names, int num_parents,
		enum dev_type type, unsigned long flags,
		struct bus_and_dev_clk_info *info, spinlock_t *lock);
/* register clk about virtual */
extern struct clk *imapx_vitual_clk_register(const char *name);
/* register clk about cpu */
extern struct clk *imapx_cpu_clk_register(const char *name,
		const char *parent_names, unsigned long flags,
		spinlock_t *lock, uint32_t ratio);

/* init clk */
extern void imapx_clock_init(void);

/*get clk struct by name*/
extern struct clk *get_clk_by_name(char *name);
#endif
