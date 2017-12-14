/* 
 *(1)This head file is modified by summer.
 *(2)Date:2015-05-06
 *(3)For ssp .
 * */

#ifndef __Q3F_SPIFLASH_H__
#define __Q3F_SPIFLASH_H__

#include <asm-arm/arch-q3f/imapx_base_reg.h>


#define spi_printf(format,args...)  printf("[SPI]:"format,##args)

//errno
#define SPI_SUCCESS				0
#define SPI_OPERATE_FAILED		( (SPI_SUCCESS) -1 )

//Config
#define SPI_CLK						400000
#define SPI_CLK_DIV_I				2
#define SPI_CLK_DIV_J				39	
#define SPI_CASE_STR_LEN			40

//One wire ip
//open: ssp0
//close: ssp1
#define COMPILE_FOR_SSP0 1

#ifdef COMPILE_FOR_SSP0

#define SSP_CR0                      SSP_BASE_0_ADDR+0x0
#define SSP_CR1                      SSP_BASE_0_ADDR+0x4
#define SSP_DR                       SSP_BASE_0_ADDR+0x8
#define SSP_SR                       SSP_BASE_0_ADDR+0xC
#define SSP_CPSR                     SSP_BASE_0_ADDR+0x10
#define SSP_IMSC                     SSP_BASE_0_ADDR+0x14
#define SSP_RIS                      SSP_BASE_0_ADDR+0x18
#define SSP_MIS                      SSP_BASE_0_ADDR+0x1C
#define SSP_ICR                      SSP_BASE_0_ADDR+0x20
#define SSP_DMACR                    SSP_BASE_0_ADDR+0x24

#else

#define SSP_CR0                      SSP_BASE_1_ADDR+0x0
#define SSP_CR1                      SSP_BASE_1_ADDR+0x4
#define SSP_DR                       SSP_BASE_1_ADDR+0x8
#define SSP_SR                       SSP_BASE_1_ADDR+0xC
#define SSP_CPSR                     SSP_BASE_1_ADDR+0x10
#define SSP_IMSC                     SSP_BASE_1_ADDR+0x14
#define SSP_RIS                      SSP_BASE_1_ADDR+0x18
#define SSP_MIS                      SSP_BASE_1_ADDR+0x1C
#define SSP_ICR                      SSP_BASE_1_ADDR+0x20
#define SSP_DMACR                    SSP_BASE_1_ADDR+0x24

#endif

//CONTROL0:SCR CLOCK 
#define SSP_SET_CLOCK_SCR_ZERO		 (~0xff00)	


//CONTROL0: Clock polarity,phase,off=6bit
#define SSP_CLOCK_POL_PH_ZERO		 (~0xc0)
#define SSP_CLOCK_POL1_PH1_FOR_MOTO	 0x3<<6
#define SSP_CLOCK_POL0_PH1_FOR_MOTO	 0x2<<6
#define SSP_CLOCK_POL1_PH0_FOR_MOTO	 0x1<<6
#define SSP_CLOCK_POL0_PH0_FOR_MOTO	 0x0<<6

//#define SSP_MODULE_WIRES_EN			 0x21e11420 
#define SSP_MODULE_WIRES_EN			 (OTF_SYSM_ADDR + 0x20)
#define SSP_MODULE_ONE_WIRE			 0xf//old ip only has one wire

#define SSP_MODULE_ONE_WIRE_NEW		 0x1//new ip,has 1/2/4 wire
#define SSP_MODULE_TWO_WIRE			 0x2
#define SSP_MODULE_FOUR_WIRE		 0x4
#define SSP_MODULE_QPI_WIRE			 0x5

//FIFO WIDTH
#define SSP_FIFO_WIDTH_8			 0x8 
#define SSP_FIFO_WIDTH_16			 0x10
#define SSP_FIFO_WIDTH_32			 0x20
//Status Register:-transmit fifo
//Status Register:-receive fifo
//Status Register:-busy status
#define SSP_BUSY					 0x1<<4
#define SSP_NOT_BUSY				 0x0<<4
//CLOCK
#define SSP_CLK_SRC					"spi"

/* 
 *(1)Flash command set
 *(2)Flash specific
 *(3.1)EG-register
 *(3.2)PRG-program
 *(3.3)REG-register
 *(3.4)STAT-Status
 *(3.5)ER-erase
 *(3.6)EN-enable
 *(3.7)WR-write;WE-write enable
 *(3.8)RD-read
 *(3.9)RDY-ready
 ***************************Common Instruction Set*****************************
 * */
//Flash specific 
#define FLASH_DEV_WREN					0x1 
#define FLASH_DEV_RDY					0x0 
#define FLASH_STAT_REG2_RD				0x2 
#define FLASH_STAT_REG2_WR				0x3
#define FLASH_STAT_REG2_QE_EN			0x4
#define FLASH_STAT_REG2_QE_DIS			0x5
#define FLASH_DEV_RDY_ND				0x6
#define FLASH_DEV_WREN_ND				0x7
#define FLASH_DEV_GET_REG_A0_ND			0x8
#define FLASH_DEV_GET_REG_B0_ND			0x9
#define FLASH_DEV_GET_REG_C0_ND			0x10
#define FLASH_DEV_GET_REG_D0_ND			0x11
#define FLASH_STAT_REG_RD				0x12
#define FLASH_CFG_REG_RD				0x13
#define FLASH_STAT_REG_QE_EN			0x14
#define FLASH_STAT_REG_QE_DIS			0x15
#define FLASH_STAT_REG3_RD				0x16
#define FLASH_STAT_REG3_WR				0x17
//read 
#define FLASH_CMD_RD_JEDEC				0x9F    //1 dummy
#define FLASH_CMD_RD_FT					0xB	    //1 dummy
#define	FLASH_CMD_RD_DUAL				0x3B	//1 dummy
#define FLASH_CMD_RD_QUAD				0x6B	//1 dummy
//write
#define	FLASH_CMD_PP					0x02	//1 dummy
#define FLASH_CMD_PP_QUAD				0x32	//1 dummy
#define FLASH_CMD_4PP					0x38	//1 dummy
//status
#define FLASH_CMD_WR_EN					0x6 //write enable
#define FLASH_CMD_WR_DIS				0x4 //write disable
#define FLASH_CMD_WR_STAT_REG			0x01
#define FLASH_CMD_RD_STAT_REG_1			0x5 //read status reg 1
#define FLASH_CMD_WR_STAT_REG_1			0x1	//write status reg 1	
#define FLASH_CMD_RD_STAT_REG_2			0x35//read status reg 2
#define FLASH_CMD_WR_STAT_REG_2			0x31//write status reg 2	
//erase
#define FLASH_CMD_ERAS_SECT			0x20 //erase 4K
#define FLASH_CMD_ERAS_BLK32		0x52 //erase 32K
#define FLASH_CMD_ERAS_BLK			0xD8 //erase 64K
#define FLASH_CMD_ERAS_CHIP			0xC7 //erase chip

#define FLASH_CMD_EN_4B					0xB7 //enter 4byte mode
#define FLASH_CMD_EX_4B					0xE9 //exit 4byte mode

 /***************************Special Instruction For mx25l256*****************************/
#define FLASH_CMD_RD_STAT_REG			FLASH_CMD_RD_STAT_REG_1
#define FLASH_CMD_RD_CFG_REG			0x15
#define FLASH_CMD_WR_STAT_REG			0x01
 /***************************Special Instruction For Nand*****************************/
#define FLASH_CMD_RD2CACHE_ND			0x13
#define FLASH_CMD_GET_FEATURE_ND		0xf
#define FLASH_CMD_SET_FEATURE_ND		0x1f
#define FLASH_BLOCK_ER_ND				0xd8
#define FLASH_CMD_PRG_EXECUTE_ND		0x10//page program execute
#define FLASH_CMD_ECC_STATE_ND			0x7c
#define FLASH_FEATURE_PROT_ADDR_ND      0xa0//protection addr                      
#define FLASH_FEATURE_FEAT_ADDR_ND      0xb0//feature addr                         
#define FLASH_FEATURE_STAT_ADDR_ND      0xc0//status addr                          
#define FLASH_FEATURE_FUTU_ADDR_ND      0xd0//future addr   
//===============================================================1/2/4 wire
//One wire,two wires,four wires ip
/*
 *(.)OTF:OneTwoFour
 */
#define	SSP_EN_OTF						(OTF_BASE_ADDR+0x0)
#define SSP_CTL_OTF						(OTF_BASE_ADDR+0x4) 
#define SSP_CFDF_OTF					(OTF_BASE_ADDR+0x8) 
#define SSP_INTRMASK_OTF				(OTF_BASE_ADDR+0xc) 
#define SSP_RIS_OTF						(OTF_BASE_ADDR+0x10) 
#define	SSP_MIS_OTF						(OTF_BASE_ADDR+0x14) 
#define SSP_INTRCLR_OTF					(OTF_BASE_ADDR+0x18)
#define SSP_DMACTL_OTF					(OTF_BASE_ADDR+0x1c) 
#define SSP_FIFOTHD_OTF					(OTF_BASE_ADDR+0x20) 
#define SSP_FIFOSTAT_OTF				(OTF_BASE_ADDR+0x24) 
#define SSP_TXTICNT_OTF					(OTF_BASE_ADDR+0x28) 
#define SSP_TXFIFO_DCNT_OTF				(OTF_BASE_ADDR+0x2c) 
#define SSP_RXFIFO_DCNT_OTF				(OTF_BASE_ADDR+0x30) 
#define SSP_DR0_OTF						(OTF_BASE_ADDR+0x100) 
#define SSP_DCTL0_OTF					(OTF_BASE_ADDR+0x104) 
#define SSP_DR1_OTF						(OTF_BASE_ADDR+0x110) 
#define SSP_DCTL1_OTF					(OTF_BASE_ADDR+0x114) 
#define SSP_DR2_OTF						(OTF_BASE_ADDR+0x120) 
#define SSP_DCTL2_OTF					(OTF_BASE_ADDR+0x124) 
#define SSP_DR3_OTF						(OTF_BASE_ADDR+0x130) 
#define SSP_DCTL3_OTF					(OTF_BASE_ADDR+0x134) 
#define SSP_DR4_OTF						(OTF_BASE_ADDR+0x140) 
#define SSP_DCTL4_OTF					(OTF_BASE_ADDR+0x144) 
#define SSP_FDR_OTF						(OTF_BASE_ADDR+0x150) 
#define SSP_FDCTL_OTF					(OTF_BASE_ADDR+0x154)
////////////////////////////////////////////////
#define	SSP_OTF_MODULE_EN           0x1
#define SSP_OTF_MODULE_DIS			0x0

#define SSP_CLOCK_POL_PH_ZERO_OTF	 (~0x3)
#define SSP_CLOCK_POL1_PH1_FOR_OTF	 0x3
#define SSP_CLOCK_POL0_PH1_FOR_OTF	 0x2
#define SSP_CLOCK_POL1_PH0_FOR_OTF	 0x1
#define SSP_CLOCK_POL0_PH0_FOR_OTF	 0x0
#define SSP_SET_CLOCK_SCR_ZERO_OTF		 (~0xff00)	


#define SSP_ROR_INTR_CLEAR_OTF		 (~0x8)
#define SSP_ROR_INTR_UNMASK_OTF		 0x1<<3
#define SSP_RF_INTR_CLEAR_OTF		 (~0x4)
#define SSP_RF_INTR_UNMASK_OTF		 0x1<<2
#define SSP_TT_INTR_CLEAR_OTF		 (~0x2)
#define SSP_TT_INTR_UNMASK_OTF		 0x1<<1
#define SSP_TF_INTR_CLEAR_OTF		 (~0x1)
#define SSP_TF_INTR_UNMASK_OTF		 0x1<<0

//Clear all the intr state
#define SSP_ALL_INTR_STATE_CLEAR_OTF 0x7
//Clear the receive overrun intr state
#define SSP_ROR_INTR_STATE_CLEAR_OTF 0x1<<2
//Clear the receive timeout intr state
#define	SSP_RT_INTR_STATE_CLEAR_OTF	 0x1<<1 
//Clear the transmit timeout intr state
#define SSP_TT_INTR_STATE_CLEAR_OTF	 0x1<<0


//DMA 
#define SSP_RECV_FIFO_DMA_ZERO_OTF		 (~0x2)
#define SSP_RECV_FIFO_DMA_ENABLE_OTF	 0x1<<1
#define SSP_RECV_FIFO_DMA_DISABLE_OTF	 0x0<<1
#define SSP_TRANS_FIFO_DMA_ZERO_OTF		 (~0x1)
#define SSP_TRANS_FIFO_DMA_ENABLE_OTF	 0x1<<0
#define SSP_TRANS_FIFO_DMA_DISABLE_OTF	 0x0<<0

//rx threshold 
#define SSP_RX_FIFO_DEPTH_CLEAR			(~0xff00)
#define SSP_TX_FIFO_DEPTH_CLEAR			(~0xff)

//states 
#define SSP_BUSY_OTF					0x1<<4 
#define SSP_RFE_OTF						0x1<<3 //rx fifo empty 
#define SSP_RFF_OTF						0x1<<2 //rx fifo full
#define SSP_TFE_OTF						0x1<<1 //tx	fifo empty
#define SSP_TFF_OTF						0x1<<0 //tx fifo full

//cmd  
#define SSP_DR_8BITS_OTF				7	
#define SSP_DR_16BITS_OTF				15	
#define SSP_DR_24BITS_OTF				23	
#define SSP_DR_32BITS_OTF				31	
//
#define SSP_RX_DIR_OTF						1 
#define SSP_TX_DIR_OTF						0
#define SSP_DR_EN_OTF						1 
#define SSP_DR_DIS_OTF						0 


#endif


