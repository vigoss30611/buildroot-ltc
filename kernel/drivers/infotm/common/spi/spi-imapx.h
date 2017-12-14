

#define SPI_EN   (0x000)
#define SPI_CTL  (0x004)
#define SPI_CFDF (0x008)
#define SPI_IMSC (0x00C)
#define SPI_RIS  (0x010)
#define SPI_MIS  (0x014)
#define SPI_ICR  (0x018)
#define SPI_DMACR (0x01C)
#define SPI_FTHD  (0x020)
#define SPI_FSTA  (0x024)
#define SPI_TXTOUT   (0x028)
#define SPI_TFDAC (0x02C)
#define SPI_RFDAC (0x030)
#define SPI_DR0   (0x100)
#define SPI_DCTL0 (0x104)
#define SPI_DR1   (0x110)
#define SPI_DCTL1 (0x114)
#define SPI_DR2   (0x120)
#define SPI_DCTL2 (0x124)
#define SPI_DR3   (0x130)
#define SPI_DCTL3 (0x134)
#define SPI_DR4   (0x140)
#define SPI_DCTL4 (0x144)
#define SPI_FDR   (0x150)
#define SPI_FDRCTL (0x154)

#define SPI_DR(n) (SPI_DR0 + n * 0x10)
#define SPI_DCTL(n) (SPI_DCTL0 + n * 0x10)

#define STATE_START                     ((void *) 0)
#define STATE_RUNNING                   ((void *) 1)
#define STATE_DONE                      ((void *) 2)
#define STATE_ERROR                     ((void *) -1)

#define SPI_CS_SELECT		0
#define SPI_CS_DESELECT		1

#define SPI_INT_MASK_ALL	(SPI_RXOVR_IM | SPI_RXTIO_IM | SPI_RXTHD_IM \
				| SPI_TXTIO_IM | SPI_TXTHD_IM)

#define SPI_RXOVR_IM	(1 << 4)
#define SPI_RXTIO_IM	(1 << 3)
#define SPI_RXTHD_IM	(1 << 2)
#define SPI_TXTIO_IM	(1 << 1)
#define SPI_TXTHD_IM	(1 << 0)


enum bits_width {
	BIT_WIDTH_U8,
	BIT_WIDTH_U16,
	BIT_WIDTH_U32,
};

/*Slave spi_dev related*/
struct chip_data {
	bool 				enable_dma;
	int				xfer_type;
	int 				speed_hz;
	int				clk_div;
	int 				mode;
	int				n_bytes;
	int				tx_threshold;
	int				rx_threshold;
	enum bits_width			bits_per_word;
	void (*cs_control)(u32 command);
};

struct data_port {
	uint32_t 		dctl;
	uint32_t		dr;
	int			enable;
};

struct imapx_spi {
	struct device			*dev;
	void __iomem			*base;
	u32				phy_base;
	int				irq;  /*irq for this controller*/
	int 				chan; /*host controller channel*/
	int 				quirk;
	struct spi_master		*master;
	struct spi_message		*message;
	struct spi_transfer		*cur_transfer;
	struct chip_data		*cur_chip;
	bool 				next_msg_cs_active;
	struct imapx_spi_host_config	*master_info;
	struct clk			*busclk;
	struct clk                      *extclk;
	int                    		busclk_rate;
	int                    		extclk_rate;
	int 				cur_speed;
	int				max_clk_div;
	int 				min_clk_div;
	enum spimul_rx_level_trig       rx_lev_trig;
	enum spimul_tx_level_trig       tx_lev_trig;
	enum bits_width			bitwidth;
	enum bits_width			rx_bitwidth;
	enum bits_width			tx_bitwidth;
	int				cur_wire;
	void 				*tx;
	void 				*rx;
	unsigned  			cur_rx_len;
	unsigned 			cur_tx_len;
	void				*tx_end; 
	void 				*rx_end;
#if CONFIG_DMA_ENGINE
	struct dma_chan                 *dma_rx_channel;
	struct dma_chan                 *dma_tx_channel;
	struct sg_table			sgt_rx;
	struct sg_table			sgt_tx;
	bool				dma_running;
	char                            *buffer;
	dma_addr_t                      buffer_dma_addr;
#endif

#ifdef CONFIG_DEBUG_FS
	struct dentry 			*debugfs;
#endif
	struct data_port		dp0;
	struct data_port		dp1;
	struct data_port		dp2;
	struct data_port		dp3;
	struct data_port		dp4;
	struct data_port		fdp;
};



static inline u32 spi_readl(struct imapx_spi *host, u32 offset) {
	return __raw_readl(host->base + offset);
}

static inline void spi_writel(struct imapx_spi *host, u32 offset, u32 val) {
	__raw_writel(val, host->base + offset);
}

static inline u16 spi_readw(struct imapx_spi *host, u32 offset) {
	return __raw_readw(host->base + offset);
}

static inline void spi_writew(struct imapx_spi *host, u32 offset, u32 val) {
	__raw_writew(val, host->base + offset);
}
