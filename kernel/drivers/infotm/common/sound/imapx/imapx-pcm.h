#ifndef __IMAPX800_PCM_H__
#define __IMAPX800_PCM_H__

/*  Register Offsets */
#define IMAPX_PCM_CFG            0x00
#define IMAPX_PCM_CTL            0x04
#define IMAPX_PCM_CLKCTL         0x08
#define IMAPX_PCM_MCLK_CFG       0x0c
#define IMAPX_PCM_BCLK_CFG       0x10
#define IMAPX_PCM_TXFIFO         0x14
#define IMAPX_PCM_RXFIFO         0x18
#define IMAPX_PCM_IRQ_CTL        0x1c
#define IMAPX_PCM_IRQ_STAT       0x20
#define IMAPX_PCM_FIFO_STAT      0x24
#define IMAPX_PCM_CLRINT         0x28

/* frame len / 2 */
#define PCM_WL_REG_16   (0x0)
#define PCM_WL_REG_17   (0x1)
#define PCM_WL_REG_18   (0x2)
#define PCM_WL_REG_19   (0x3)
#define PCM_WL_REG_20   (0x4)
#define PCM_WL_REG_21   (0x5)
#define PCM_WL_REG_22   (0x6)
#define PCM_WL_REG_23   (0x7)
#define PCM_WL_REG_24   (0x8)
#define PCM_WL_REG_25   (0x9)
#define PCM_WL_REG_26   (0xa)
#define PCM_WL_REG_27   (0xb)
#define PCM_WL_REG_28   (0xc)
#define PCM_WL_REG_29   (0xd)
#define PCM_WL_REG_30   (0xe)
#define PCM_WL_REG_31   (0xf)
#define PCM_WL_REG_32   (0x10)
#define PCM_WL_REG_8    (0x11)

/* irq mask */
#define RXFIFO_ERROR_OVERFLOW       (1 << 0)
#define RXFIFO_ERROR_STARVE         (1 << 1)
#define RX_FIFO_ALMOST_FULL         (1 << 2)
#define RX_FIFO_FULL                (1 << 3)
#define RX_FIFO_ALMOST_EMPTY        (1 << 4)
#define RX_FIFO_EMPTY               (1 << 5)
#define TXFIFO_ERROR_OVERFLOW       (1 << 6)
#define TXFIFO_ERROR_STARVE         (1 << 7)
#define TX_FIFO_ALMOST_FULL         (1 << 8)
#define TX_FIFO_FULL                (1 << 9)
#define TX_FIFO_ALMOST_EMPTY        (1 << 10)
#define TX_FIFO_EMPTY               (1 << 11)
#define TRANSFER_DONE               (1 << 12)
#define EN_IRQ_TO_ARM               (1 << 14)


struct imapx_pcm_info {
	spinlock_t lock;
	struct device   *dev;
	void __iomem    *regs;
	struct clk  *extclk;
	struct imapx_pcm_dma_params  *dma_playback;
	struct imapx_pcm_dma_params  *dma_capture;
	unsigned int    iis_clock_rate;
	int sync_num;
};

#endif
