#ifndef __GDMA_H__
#define __GDMA_H__

#include <common.h>

// instruction command value 
#define CMD_DMAADDH 0x54     
#define CMD_DMAADNH 0x5c     
#define CMD_DMAEND  0x00     
#define CMD_DMAFLUSHP   0x35 
#define CMD_DMAGO   0xa0     
#define CMD_DMALD   0x04     
#define CMD_DMALDP  0x25     
#define CMD_DMALP   0x20     
#define CMD_DMALPEND    0x28 
#define CMD_DMAKILL 0x01     
#define CMD_DMAMOV  0xbc     
#define CMD_DMANOP  0x18     
#define CMD_DMARMB  0x12     
#define CMD_DMASEV  0x34     
#define CMD_DMAST   0x08     
#define CMD_DMASTP  0x29     
#define CMD_DMASTZ  0x0c     
#define CMD_DMAWFE  0x36     
#define CMD_DMAWFP  0x30     
#define CMD_DMAWMB  0x13     

// instruct command size 
#define SZ_DMAADDH  3    
#define SZ_DMAADNH  3    
#define SZ_DMAEND   1    
#define SZ_DMAFLUSHP    2
#define SZ_DMALD    1    
#define SZ_DMALDP   2    
#define SZ_DMALP    2    
#define SZ_DMALPEND 2    
#define SZ_DMAKILL  1    
#define SZ_DMAMOV   6    
#define SZ_DMANOP   1    
#define SZ_DMARMB   1    
#define SZ_DMASEV   2    
#define SZ_DMAST    1    
#define SZ_DMASTP   2    
#define SZ_DMASTZ   1    
#define SZ_DMAWFE   2    
#define SZ_DMAWFP   2    
#define SZ_DMAWMB   1    
#define SZ_DMAGO    6    

// GDMA_CCR: register setting    
#define CCR_SRCINC  (1 << 0)      
#define CCR_DSTINC  (1 << 14)     
#define CCR_SRCPRI  (1 << 8)      
#define CCR_DSTPRI  (1 << 22)     
#define CCR_SRCNS   (1 << 9)      
#define CCR_DSTNS   (1 << 23)     
#define CCR_SRCIA   (1 << 10)     
#define CCR_DSTIA   (1 << 24)     
#define CCR_SRCBRSTLEN_SHFT 4     
#define CCR_DSTBRSTLEN_SHFT 18    
#define CCR_SRCBRSTSIZE_SHFT    1 
#define CCR_DSTBRSTSIZE_SHFT    15
#define CCR_SRCCCTRL_SHFT   11    
#define CCR_SRCCCTRL_MASK   0x7   
#define CCR_DSTCCTRL_SHFT   25    
#define CCR_DRCCCTRL_MASK   0x7   
#define CCR_SWAP_SHFT   28        

#define BRST_LEN(ccr)   ((((ccr) >> CCR_SRCBRSTLEN_SHFT) & 0xf) + 1)
#define BRST_SIZE(ccr)  (1 << (((ccr) >> CCR_SRCBRSTSIZE_SHFT) & 0x7))
#define BYTE_TO_BURST(b, ccr)  ((b) / BRST_SIZE(ccr) / BRST_LEN(ccr))
#define BURST_TO_BYTE(c, ccr)	((c) * BRST_SIZE(ccr) * BRST_LEN(ccr))

extern int gdma_memcpy (uint8_t *src_addr, uint8_t *dst_addr, uint32_t data_len);
extern int gdma_memcpy_align (uint8_t *src_addr, uint8_t *dst_addr, uint32_t data_len, uint32_t align);
extern void dma_clear_result(char *data_addr, unsigned int data_len);
extern int dma_compare_result(uint32_t src_addr, uint32_t dst_addr, uint32_t data_len);
int init_gdma(void);
extern int test_gdma(uint32_t src_addr, uint32_t dst_addr, uint32_t len, uint32_t align);
enum srccachectrl {                                                
    SCCTRL0 = 0, /*  Noncacheable and nonbufferable */              
    SCCTRL1, /*  Bufferable only */                                 
    SCCTRL2, /*  Cacheable, but do not allocate */                  
    SCCTRL3, /*  Cacheable and bufferable, but do not allocate */   
    SINVALID1,                                                     
    SINVALID2,                                                     
    SCCTRL6, /*  Cacheable write-through, allocate on reads only */ 
    SCCTRL7, /*  Cacheable write-back, allocate on reads only */    
};                                                                 

enum dstcachectrl {                                                
    DCCTRL0 = 0, /*  Noncacheable and nonbufferable */              
    DCCTRL1, /*  Bufferable only */                                 
    DCCTRL2, /*  Cacheable, but do not allocate */                  
    DCCTRL3, /*  Cacheable and bufferable, but do not allocate */   
    DINVALID1 = 8,                                                 
    DINVALID2,                                                     
    DCCTRL6, /*  Cacheable write-through, allocate on writes only */
    DCCTRL7, /*  Cacheable write-back, allocate on writes only */   
};                                                                 

enum byteswap {                                                    
    SWAP_NO = 0,                                                   
    SWAP_2,                                                        
    SWAP_4,                                                        
    SWAP_8,                                                        
    SWAP_16,                                                       
};                                                                 

enum dma_transaction_type {
	DMA_MEMCPY,
	DMA_XOR,
	DMA_PQ,
	DMA_XOR_VAL,
	DMA_PQ_VAL,
	DMA_MEMSET,
	DMA_INTERRUPT,
	DMA_SG,
	DMA_PRIVATE,
	DMA_ASYNC_TX,
	DMA_SLAVE,
	DMA_LLI,
	DMA_CYCLIC,
	DMA_INTERLEAVE,
/* last transaction type for creation of the capabilities mask */
	DMA_TX_TYPE_END,
};

enum dma_transfer_direction {
	DMA_MEM_TO_MEM,
	DMA_MEM_TO_DEV,
	DMA_DEV_TO_MEM,
	DMA_DEV_TO_DEV,
	DMA_TRANS_NONE,
};

/*
 * PL330 can assign any channel to communicate with
 * any of the peripherals attched to the DMAC.
 * For the sake of consistency across client drivers,
 * We keep the channel names unchanged and only add
 * missing peripherals are added.
 * Order is not important since IMAPX PL330 API driver
 * use these just as IDs.
 */
enum dma_ch {
	IMAPX_PCM0_TX = 0,
	IMAPX_PCM0_RX = 2,
	IMAPX_SSP0_TX = 4,
	IMAPX_SSP1_TX = 5,
	IMAPX_SSP0_RX = 6,
	IMAPX_SSP1_RX = 7,
	IMAPX_UART0_TX = 10,
	IMAPX_UART1_TX = 11,
	IMAPX_UART2_TX = 12,
	IMAPX_UART3_TX = 13,
	IMAPX_UART0_RX = 14,
	IMAPX_UART1_RX = 15,
	IMAPX_UART2_RX = 16,
	IMAPX_UART3_RX = 17,
	IMAPX_I2S_MASTER_TX = 20,
	IMAPX_I2S_MASTER_RX = 21,
	IMAPX_TOUCHSCREEN_RX = 22,
	/*  END Marker, also used to denote a reserved channel */
	IMAPX_MAX,
	IMAPX_I2S_SLAVE_TX,
	IMAPX_I2S_SLAVE_RX,
	IMAPX_AC97_TX,
	IMAPX_AC97_RX,
	IMAPX_PCM1_TX,
	IMAPX_PCM1_RX,
	IMAPX_SPDIF_TX,
	IMAPX_SPDIF_RX,
	IMAPX_PWMA_TX,
	IMAPX_DMIC_RX,
};

struct gdma_reqcfg {                          
    uint32_t port_num;                     
    /*  Address Incrementing */                 
    uint32_t dst_inc;                          
    uint32_t src_inc;                          

    /*                                          
     ** For now, the SRC & DST protection levels
     ** and burst size/length are assumed same. 
     */                                        
    uint32_t nonsecure;                        
    uint32_t privileged;                       
    uint32_t insnaccess;                       
    uint32_t brst_len;                         
    uint32_t brst_size; /*  in power of 2 */    

    enum dstcachectrl dcctl;                   
    enum srccachectrl scctl;                   
    enum byteswap swap;                        
};

struct _arg_GO {
    uint8_t chan;
    uint32_t addr;
    uint32_t ns;
};

enum gdma_cond {
    SINGLE, 
    BURST,
    ALWAYS,
};       

struct _arg_LPEND {      
    enum gdma_cond cond;
    uint32_t forever;    
    uint32_t loop;
    uint8_t bjump;
};

enum dmamov_dst {
    SAR = 0,     
    CCR,
    DAR,         
};  

struct gdma_addr {
    uint32_t byte;
    int modbyte;
};

struct gdma_mov {
    uint8_t nop[2];         //for 4 bytes aligned
    uint8_t command;
    uint8_t attribute;
    uint32_t addr;
};

struct gdma_lp {
    uint8_t command;
    uint8_t lpnum;
};

struct gdma_lpend {
    uint8_t command;
    uint8_t lpjmp;
};

struct gdma_sev {
    uint8_t command;
    uint8_t channum;
};

struct gdma_desc {
    struct gdma_mov mov[3];
    struct gdma_lp lp[2];
    uint8_t emit_ld;
#define SINGLE_LP   (sizeof(uint8_t))
    struct gdma_lpend lpend0;
    uint8_t emit_rmb;
    struct gdma_lp lp1;
    uint8_t emit_st;
    struct gdma_lpend lpend1;
    uint8_t emit_wmb;
#define DOUBLE_LP   (2 * sizeof(struct gdma_lpend) + 2 * sizeof(struct gdma_lp) + 4)
    struct gdma_lpend lpend2;
#if 1
    uint8_t nop[3];
    struct gdma_lp lp1_0;
    uint8_t emit_ld1;
    struct gdma_lpend lpend1_0;
    uint8_t emit_rmb1;
    struct gdma_lp lp1_1;
    uint8_t emit_st1;
    struct gdma_lpend lpend1_1;
    uint8_t emit_wmb1;
#endif
    struct gdma_sev sev;
    uint8_t emit_end;
};

#endif
