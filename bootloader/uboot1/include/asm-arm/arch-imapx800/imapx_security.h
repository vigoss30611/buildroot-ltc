#ifndef __IMAPX_SECURITY_H__
#define __IMAPX_SECURITY_H__

/*
 * security sub-system Registers
 */
#define SECURITY_BASE_ADD    CRYPTO_BASE_ADDR    /* fix it */

/*
 * DMA
 * 
 * there four kinds of DMA,   BRDMA : BLCK receive DMA     BTDMA : BLCK transmission DMA
 *                            HRDMA : Hsah receive DMA     PKADMA: PKA Bi-Directional DMA
 * 
 * x depends on the DMA choosen
 */  
#define	rSEC_DMA_START(x)	      (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x000))	/* DMA */
#define	rSEC_DMA_ADMA_CFG(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x004))	/* DMA ADMA config register */
#define	rSEC_DMA_ADMA_ADDR(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x008))	/* DMA ADMA address register */
#define	rSEC_DMA_SDMA_CTRL0(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x00c))	/* DMA SDMA control register0 */
#define	rSEC_DMA_SDMA_CTRL1(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x010))	/* DMA SDMA control register1 */
#define	rSEC_DMA_SDMA_ADDR0(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x014))	/* DMA SDMA address register0 */
#define	rSEC_DMA_SDMA_ADDR1(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x018))	/* DMA SDMA address register1 */
#define	rSEC_DMA_INT_ACK(x)  	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x01c))	/* DMA interrupt ack register */
#define	rSEC_DMA_STATE(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x020))	/* DMA state register */
#define	rSEC_DMA_STATUS(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x024))	/* DMA status register */
#define	rSEC_DMA_INT_MASK(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x028))	/* DMA interrupt mask register */
#define	rSEC_DMA_INT_STATUS(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+x*0x100+0x02c))	/* DMA interrupt status register */


/* Block cipher */
#define	rSEC_BLKC_CONTROL		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x400)) /* blcok cipher control R */
#define	rSEC_BLKC_FIFO_MODE_EN	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x404))	/* enable BLCK FIFO mode */
#define	rSEC_BLKC_SWAP   		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x408))	/* BLKC SWAP R */
#define	rSEC_BLKC_STATUS		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x40c))	/* BLKC Status R */
#define	rSEC_AES_CNTDATA(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x410 + x * 4))	/* AES counter data register */
#define rSEC_AES_KEYDATA(x)       (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x420 + x * 4))	/* AES key data register */
#define	rSEC_AES_IVDATA(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x440 + x * 4))	/* AES initial vector data register */
#define	rSEC_AES_INDATA(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x450 + x * 4))	/* AES input data register 1 */
#define	rSEC_AES_OUTDATA(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x460 + x * 4))	/* AES output data register */
#define rSEC_TDES_CNTDATA(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x470 + x * 4))	/* triple DES counter data register */
#define	rSEC_TDES_CNTDATA1	   	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x470))	/* triple DES counter data register 1 */
#define	rSEC_TDES_CNTDATA2		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x474))	/* triple DES counter data register 2 */
#define rSEC_TDES_KEYDATA(x, y)   (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x478 + x * 8 + y * 4))	/* triple DES key  data register */
#define	rSEC_TDES_KEYDATA11		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x478))	/* triple DES key 1 data register 1 */
#define	rSEC_TDES_KEYDATA12	  	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x47c))	/* triple DES key 1 data register 2 */
#define	rSEC_TDES_KEYDATA21		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x480))	/* triple DES key 2 data register 1 */
#define	rSEC_TDES_KEYDATA22		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x484))	/* triple DES key 2 data register 2 */
#define	rSEC_TDES_KEYDATA31		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x488))	/* triple DES key 3 data register 1 */
#define	rSEC_TDES_KEYDATA32		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x48c))	/* triple DES key 3 data register 2 */
#define rSEC_TDES_IVDATA(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x490 + x * 4))	/* triple DES initial vector data register */
#define rSEC_TDES_INDATA(x)	      (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x498 + x * 4))	/* triple DES input data register */
#define	rSEC_TDES_OUTDATA(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x4a0 + x * 4))	/* triple DES output data register */
#define	rSEC_RC4_KEYDATA(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x4a8 + x * 4))	/* RC4 key data register */
#define	rSEC_RC4_INDATA 		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x4c8))	/* RC4 input data register */
#define	rSEC_RC4_OUTDATA		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x4cc))	/* RC4 output data register */
#define	rSEC_BLCK_INT_MASK	      (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x4d0))	/* BLCK interrupt mask register */
#define	rSEC_BLCK_INT_STATUS  	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x4d4))	/* BLCK interrupt status register */

/* aes custom sbox */
#define	rSEC_AES_CUSTOM_SBOX(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x500 + x * 4))	/* AES custom SBOX  0x500 ~ 0x5fc */

/* des custom sbox */
#define	rSEC_DES_CUSTOM_SBOX(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x600 + x * 4))	/* DES custom SBOX  0x600 ~ 0x6fc */

/* rc4 custom sbox */
#define	rSEC_RC4_CUSTOM_SBOX(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x700 + x * 4))	/* RC4 custom SBOX  0x700 ~ 0x7fc */

/* HASH */
#define	rSEC_HASH_CONTROL		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x800))	/* hash control register */
#define	rSEC_HASH_FIFO_MODE_EN	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x804))	/* enable hash fifo mode */
#define	rSEC_HASH_SWAP  		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x808))	/* hash swap register */
#define	rSEC_HASH_STATUS		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x80c))	/* hash status register */
#define	rSEC_HASH_MSG_SIZE_LOW	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x810))	/* hash message size register - LSB */
#define	rSEC_HASH_MSG_SIZE_HIGH	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x814))	/* hash message size register - MSB */
#define	rSEC_HASH_DATA_IN(x)	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x818 + x * 4))	/* hash input data register */
#define	rSEC_HASH_CV_IN(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x838 + x * 4))	/* hash custom initial vector register */
#define rSEC_HASH_RESULT(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x858 + x * 4))	/* hash result register */
#define	rSEC_HASH_HMAC_KEY(x)     (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x878 + x * 4))	/* hash HMAC key data register */
#define	rSEC_HASH_INT_MASK	   	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x8b8))	/* hash interrupt mask register */
#define	rSEC_HASH_INT_STATUS      (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x8bc))	/* hash interrupt status register */
                               
/* PRNG */
#define	rSEC_PRNG_CONTROL		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x900))	/* PRNG control register */
#define	rSEC_PRNG_SEED  		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x304))	/* PRNG seed data register */
#define rSEC_PRNG_RNDNUM(x)		  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x908 + x * 4))	/* PRNG generated random number register */
                                
/* PKA */
#define rSEC_PKA_CONTROL    	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA00))	/* PKA control */
#define rSEC_PKA_FIFO_MODE_EN     (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA04))	/* PKA fifo mode en */
#define rSEC_PKA_SWAP        	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA08))	/* PKA swap */
#define rSEC_PKA_STATUS      	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA0C))	/* PKA status */
#define rSEC_PKA_STATE       	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA10))	/* PKA state */
#define rSEC_PKA_Q_IN1       	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA14))	/* PKA Q input data1 */
#define rSEC_PKA_Q_IN2      	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA18))	/* PKA Q input data2 */
#define rSEC_PKA_INT_MASK    	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA1C))	/* PKA int mask */
#define rSEC_PKA_INT_STAT    	  (*(volatile unsigned int *)(SECURITY_BASE_ADD+0xA20))	/* PKA int state */
#define rSEC_PKA_BUF(x, y)        (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x1000 + x * 0x100 + y * 4))
#define rSEC_PKA_DE_BUF(x)        (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x1000 + x * 4))	/* 1000 ~ 13ff */
#define rSEC_PKA_P_BUF(x)         (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x1400 + x * 4))	/* 1400 ~ 17ff */
#define rSEC_PKA_R_BUF(x)         (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x1800 + x * 4))	/* 1800 ~ 1bff */
#define rSEC_PKA_MC_BUF(x)        (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x1c00 + x * 4))	/* 1c00 ~ 1fff */

/* clk */
#define rSEC_CG_CFG               (*(volatile unsigned int *)(SECURITY_BASE_ADD+0x2000)) /* clk en */

#endif /* imapx_seucrity.h */
