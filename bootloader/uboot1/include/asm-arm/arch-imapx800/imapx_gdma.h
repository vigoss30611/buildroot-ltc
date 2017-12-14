#ifndef __IMAPX_GDMA_H__
#define __IMAPX_GDMA_H__

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

#endif
