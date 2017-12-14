#ifndef __SECURITY_DMA_H__
#define __SECURITY_DMA_H__

/*
 * DMA
 */
/* start */
#define BRDMA                        0
#define BTDMA                        1
#define HRDMA                        2

#define DMA_START_BIT                0
#define DMA_RESET_BIT                1

/* ADMA CFG */
#define ADMA_ENABLE                  0        /* 1, enable and start DMA ....      0, disable and reset ADMA */
#define ADMA_MODE                    1        /* 0, SDMA   1, ADMA */
#define SDMA_2CH_MODE                2        /* 0, using sdma addr0    1, using sdma addr0 and addr1 */
#define BUFBOUND_CHK                 3        /* 1 enable    0 disable */
#define BUFBOUND                     4        /* 0~7   4Kbytes ~ 512Kbytes */
#define SDMA_BINT_MODE               8        /* 0 no inter, 1 channel1, 2 channel2, 3 channel 1and2 */

#define SDMA_MODE                    0
#define ADMA_MODE                    1

/* SDMA CTRL0 */
#define BLK_NUMBER                   0      /* 15:0    SDMA mode repeat number */
#define BST_LEN                      16     /* 27:16   SDMA burst length */

/* SDMA CTRL1 */
#define BLK_CH0_SIZE                 0
#define BLK_CH1_SIZE                 16

/* DMA INT ACK */
#define ADMA_INT_ACK                 0
#define SDMA_INT_ACK                 1
#define SDMA_BINT_ACK                2

/* DMA STATUS */
#define SDMA_BINT                    0      
#define SDMA_INT                     1      
#define ADMA_INT                     2
#define SDMA_FINISH                  3
#define ADMA_FINISH                  4
#define ADMA_ERROR                   5

#define ENABLE                       1
#define DISABLE                      0

/* function */
extern void dma_start(uint32_t dma);
extern uint32_t dma_reset(uint32_t dma);
extern uint32_t dma_get_status(uint32_t dma, uint32_t bit);
extern uint32_t dma_sdma_init(uint32_t dma, uint32_t add, uint32_t len);

/* security clk */
#define SEC_BLCK_CKE                 0
#define SEC_HASH_CKE                 1
#define SEC_PKA_CKE                  2

extern void security_cke_control(uint32_t module_cke, uint32_t mode);

#endif /* security_dma.h */

