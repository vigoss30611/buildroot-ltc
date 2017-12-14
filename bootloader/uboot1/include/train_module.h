#ifndef __TRAIN_MODULE_H__
#define __TRAIN_MODULE_H__

#include "dramc_init.h"
#include "training.h"
#include "common.h"

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
    uint8_t nop[3];
    struct gdma_lp lp1_0;
    uint8_t emit_ld1;
    struct gdma_lpend lpend1_0;
    uint8_t emit_rmb1;
    struct gdma_lp lp1_1;
    uint8_t emit_st1;
    struct gdma_lpend lpend1_1;
    uint8_t emit_wmb1;
    struct gdma_sev sev;
    uint8_t emit_end;
};


/* register define  */
#define GDMA_BASE_ADDR_					(0x21900000)
#define GDMA_SYSM_ADDR_					(0x21e08000)


// instruction command value
#define CMD_DMAADDH	0x54
#define CMD_DMAADNH	0x5c
#define CMD_DMAEND	0x00
#define CMD_DMAFLUSHP	0x35
#define CMD_DMAGO	0xa0
#define CMD_DMALD	0x04
#define CMD_DMALDP	0x25
#define CMD_DMALP	0x20
#define CMD_DMALPEND	0x28
#define CMD_DMAKILL	0x01
#define CMD_DMAMOV	0xbc
#define CMD_DMANOP	0x18
#define CMD_DMARMB	0x12
#define CMD_DMASEV	0x34
#define CMD_DMAST	0x08
#define CMD_DMASTP	0x29
#define CMD_DMASTZ	0x0c
#define CMD_DMAWFE	0x36
#define CMD_DMAWFP	0x30
#define CMD_DMAWMB	0x13

// instruct command size
#define SZ_DMAADDH	3
#define SZ_DMAADNH	3
#define SZ_DMAEND	1
#define SZ_DMAFLUSHP	2
#define SZ_DMALD	1
#define SZ_DMALDP	2
#define SZ_DMALP	2
#define SZ_DMALPEND	2
#define SZ_DMAKILL	1
#define SZ_DMAMOV	6
#define SZ_DMANOP	1
#define SZ_DMARMB	1
#define SZ_DMASEV	2
#define SZ_DMAST	1
#define SZ_DMASTP	2
#define SZ_DMASTZ	1
#define SZ_DMAWFE	2
#define SZ_DMAWFP	2
#define SZ_DMAWMB	1
#define SZ_DMAGO	6


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


// register offset setting 
#define GDMA_DMS       0x000            // DMA Manager Status Register
#define GDMA_DPC       0x004            // DMA Program Counter Register
#define GDMA_IE        0x020            // Interrupt Enable Register
#define GDMA_EIRS      0x024            // Event-Interrupt Raw Status Register
#define GDMA_IS        0x028            // Interrupt Status Register
#define GDMA_IC        0x02c            // Interrupt Clear Register
#define GDMA_FSDM      0x030            // Fault Status DMA Manager Register
#define GDMA_FSDC      0x034            // Fault Status DMA Channel Register
#define GDMA_FTDM      0x038            // Fault Type DMA Manager Register
#define GDMA_FTDC(n)   (0x040+n*0x4)    // Fault Type DMA Channel Registers
#define GDMA_CS(n)     (0x100+n*0x8)    // Channel Status Registers
#define GDMA_CPC(n)    (0x104+n*0x4)    // Channel Program Counter Registers
#define GDMA_SAR(n)    (0x400+n*0x20)   // Source Address Registers
#define GDMA_DAR(n)    (0x404+n*0x20)   // Destination Address Registers
#define GDMA_CCR(n)    (0x408+n*0x20)   // Channel Control Registers
#define GDMA_LC0(n)    (0x40c+n*0x20)   // Loop Counter 0 Registers
#define GDMA_LC1(n)    (0x410+n*0x20)   // Loop Counter 1 Registers
#define GDMA_DS        0xd00            // Debug Status Register
#define GDMA_DC        0xd04            // Debug Command Register
#define GDMA_DI0       0xd08            // Debug Instruction-0 Register
#define GDMA_DI1       0xd0c            // Debug Instruction-1 Register

#define GDMA_CFG0      0xe00            // Configuration Register 0
#define GDMA_CFG1      0xe04            // Configuration Register 1
#define GDMA_CFG2      0xe08            // Configuration Register 2
#define GDMA_CFG3      0xe0c            // Configuration Register 3
#define GDMA_CFG4      0xe10            // Configuration Register 4
#define GDMA_DCFG      0xe14            // DMA Configuration Register
#define GDMA_WD        0xe80            // Watchdog Register

#define GDMA_PIR       0xfe0            // Peripheral Identification Registers
#define GDMA_CIR       0xff0            // Component Identification Registers
//gdma system register
#define GDMA_SR        0x00
#define GDMA_CGE       0x04
#define GDMA_PCR       0x08
#define GDMA_GM        0x0c
#define GDMA_SC        0x20

//========================================================================
// G2D
#define G2D_BASE_ADDR				(0x29100000)
//========================================================================
#define G2D_SRC_XYST                *(volatile unsigned *)(G2D_BASE_ADDR + 0x0000 )
#define G2D_SRC_WIDTH               *(volatile unsigned *)(G2D_BASE_ADDR + 0x0004 )
#define G2D_SRC_WIDTH_1             *(volatile unsigned *)(G2D_BASE_ADDR + 0x0008 )
#define G2D_SRC_FULL_WIDTH          *(volatile unsigned *)(G2D_BASE_ADDR + 0x000C )
#define G2D_DEST_XYST               *(volatile unsigned *)(G2D_BASE_ADDR + 0x0010 )
#define G2D_DEST_WIDTH              *(volatile unsigned *)(G2D_BASE_ADDR + 0x0014 )
#define G2D_DEST_WIDTH_1            *(volatile unsigned *)(G2D_BASE_ADDR + 0x0018 )
#define G2D_DEST_FULL_WIDTH         *(volatile unsigned *)(G2D_BASE_ADDR + 0x001C )
#define G2D_PAT_WIDTH               *(volatile unsigned *)(G2D_BASE_ADDR + 0x0020 )
#define G2D_SRC_BITSWP              *(volatile unsigned *)(G2D_BASE_ADDR + 0x0024 )
#define G2D_SRC_FORMAT              *(volatile unsigned *)(G2D_BASE_ADDR + 0x0028 )
#define G2D_SRC_WIDOFFSET           *(volatile unsigned *)(G2D_BASE_ADDR + 0x002C )
#define G2D_SRC_RGB                 *(volatile unsigned *)(G2D_BASE_ADDR + 0x0030 )
#define G2D_SRC_COEF1112            *(volatile unsigned *)(G2D_BASE_ADDR + 0x0034 )
#define G2D_SRC_COEF1321            *(volatile unsigned *)(G2D_BASE_ADDR + 0x0038 )
#define G2D_SRC_COEF2223            *(volatile unsigned *)(G2D_BASE_ADDR + 0x003C )
#define G2D_SRC_COEF3132            *(volatile unsigned *)(G2D_BASE_ADDR + 0x0040 )
#define G2D_SRC_COEF33_OFT          *(volatile unsigned *)(G2D_BASE_ADDR + 0x0044 )
#define G2D_DEST_BITSWP             *(volatile unsigned *)(G2D_BASE_ADDR + 0x0048 )
#define G2D_DEST_FORMAT             *(volatile unsigned *)(G2D_BASE_ADDR + 0x004C )
#define G2D_DEST_WIDOFFSET          *(volatile unsigned *)(G2D_BASE_ADDR + 0x0050 )
#define G2D_OUT_BITSWP              *(volatile unsigned *)(G2D_BASE_ADDR + 0x0054 )
#define G2D_IF_XYST                 *(volatile unsigned *)(G2D_BASE_ADDR + 0x0058 )

#define G2D_ALPHAG                  *(volatile unsigned *)(G2D_BASE_ADDR + 0x005C )
#define G2D_CK_KEY                  *(volatile unsigned *)(G2D_BASE_ADDR + 0x0060 )
#define G2D_CK_MASK                 *(volatile unsigned *)(G2D_BASE_ADDR + 0x0064 )
#define G2D_CK_CNTL                 *(volatile unsigned *)(G2D_BASE_ADDR + 0x0068 )
#define G2D_ALU_CNTL                *(volatile unsigned *)(G2D_BASE_ADDR + 0x006C )
#define G2D_Y_BITSWP                *(volatile unsigned *)(G2D_BASE_ADDR + 0x0070 )
#define G2D_UV_BITSWP               *(volatile unsigned *)(G2D_BASE_ADDR + 0x0074 )
#define G2D_IF_WIDTH                *(volatile unsigned *)(G2D_BASE_ADDR + 0x0078 )
#define G2D_IF_HEIGHT               *(volatile unsigned *)(G2D_BASE_ADDR + 0x007C )
#define G2D_IF_FULL_WIDTH           *(volatile unsigned *)(G2D_BASE_ADDR + 0x0080 )
#define G2D_YUV_OFFSET		    *(volatile unsigned *)(G2D_BASE_ADDR + 0x0084 )
#define G2D_Y_ADDR_ST               *(volatile unsigned *)(G2D_BASE_ADDR + 0x0088 )
#define G2D_UV_ADDR_ST              *(volatile unsigned *)(G2D_BASE_ADDR + 0x008C )
#define G2D_DEST_ADDR_ST            *(volatile unsigned *)(G2D_BASE_ADDR + 0x0090 )
#define G2D_IF_ADDR_ST              *(volatile unsigned *)(G2D_BASE_ADDR + 0x0094 )
#define G2D_MEM_ADDR_ST             *(volatile unsigned *)(G2D_BASE_ADDR + 0x0098 )
#define G2D_MEM_LENGTH_ST           *(volatile unsigned *)(G2D_BASE_ADDR + 0x009C )
#define G2D_MEM_SETDATA32           *(volatile unsigned *)(G2D_BASE_ADDR + 0x00A0 )
#define G2D_MEM_SETDATA64           *(volatile unsigned *)(G2D_BASE_ADDR + 0x00A4 )
#define G2D_MEM_CODE_LEN            *(volatile unsigned *)(G2D_BASE_ADDR + 0x00A8 )
#define G2D_DRAW_CNTL               *(volatile unsigned *)(G2D_BASE_ADDR + 0x00AC )
#define G2D_DRAW_START              *(volatile unsigned *)(G2D_BASE_ADDR + 0x00B0 )
#define G2D_DRAW_END                *(volatile unsigned *)(G2D_BASE_ADDR + 0x00B4 )
#define G2D_DRAW_MASK               *(volatile unsigned *)(G2D_BASE_ADDR + 0x00B8 )
#define G2D_DRAW_MASK_DATA          *(volatile unsigned *)(G2D_BASE_ADDR + 0x00BC )
#define G2D_LUT_READY               *(volatile unsigned *)(G2D_BASE_ADDR + 0x00C0 )

#define G2D_INT_OUT		    *(volatile unsigned *)(G2D_BASE_ADDR + 0x00e4 )

#define G2D_SRC_MONO_LUT_BASE	    *(volatile unsigned *)(G2D_BASE_ADDR + 0x5000 )
#define G2D_DEST_MONO_LUT_BASE	    *(volatile unsigned *)(G2D_BASE_ADDR + 0x6000 )
#define G2D_PAT_LUT_BASE	    *(volatile unsigned *)(G2D_BASE_ADDR + 0x7000 )

/* register read&write operation */
#define __raw_writeb(v,a)	(*(volatile unsigned char  *)(a) = (v))
#define __raw_writew(v,a)	(*(volatile unsigned short *)(a) = (v))
#define __raw_writel(v,a)	(*(volatile unsigned int   *)(a) = (v))
#define __raw_writeq(v,a)	(*(volatile unsigned long long   *)(a) = (v))

#define __raw_readb(a)		(*(volatile unsigned char  *)(a))
#define __raw_readw(a)		(*(volatile unsigned short *)(a))
#define __raw_readl(a)		(*(volatile unsigned int   *)(a))
#define __raw_readq(a)		(*(volatile unsigned long long   *)(a))

#define readb		__raw_readb
#define readw		__raw_readw
#define readl		__raw_readl
#define readq		__raw_readq

#define writeb		__raw_writeb
#define writew		__raw_writew
#define writel		__raw_writel
#define writeq		__raw_writeq


extern int init_gdma (void);
#endif /* train_module.h */
