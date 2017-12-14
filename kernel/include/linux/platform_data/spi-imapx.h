
#ifndef SPI_IMAPX_H
#define SPI_IMAPX_H

#define SPIMUL_QUIRK_RXLEN_ALLIGN_8BYTES		(1 << 0)
#define SPIMUL_QUIRK_DMA_BYTESWAP			(1 << 1)

enum  spimul_mode {
	SPIMUL_TRANS_POLL,
	SPIMUL_TRANS_DMA,
};

enum spimul_interface {
	SPIMUL_INTERFACE_MOTOROLA_SPI,
	SPIMUL_INTERFACE_TI_SYNC_SERIAL,
	SPIMUL_INTERFACE_NATIONAL_MICROWIRE,
	SPIMUL_INTERFACE_UNIDIRECTIONAL
};

enum spimul_rx_level_trig {
	SPIMUL_RX_1_OR_MORE_ELEM,
	SPIMUL_RX_4_OR_MORE_ELEM,
	SPIMUL_RX_8_OR_MORE_ELEM,
	SPIMUL_RX_16_OR_MORE_ELEM,
	SPIMUL_RX_32_OR_MORE_ELEM,
};

enum spimul_tx_level_trig {
	SPIMUL_TX_1_OR_MORE_EMPTY_LOC,
	SPIMUL_TX_4_OR_MORE_EMPTY_LOC,
	SPIMUL_TX_8_OR_MORE_EMPTY_LOC,
	SPIMUL_TX_16_OR_MORE_EMPTY_LOC,
	SPIMUL_TX_32_OR_MORE_EMPTY_LOC,
};

struct imapx_spi_host_config {
	int 		bus_id;
	int		num_chipselect;
	int 		enable_dma;
	int 		rt;
	int		quirk;
	int 		extclk_rate;
	bool (*dma_filter)(struct dma_chan *chan, void *filter_param);
	void *dma_rx_param;
	void *dma_tx_param;

};


struct imapx_spi_chip_config {
	enum spimul_mode	com_mode;
	int 			iface;
	enum spimul_rx_level_trig	rx_lev_trig;
	enum spimul_tx_level_trig 	tx_lev_trig;
	void (*cs_control) (u32 control);
};
#endif
