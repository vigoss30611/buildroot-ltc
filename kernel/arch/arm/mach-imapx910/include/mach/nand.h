/*****************************************************************************
** regs-nand.h
**      
** Revision History: 
** ----------------- 
*****************************************************************************/
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#ifndef __ARM_IMAPX_REGS_NAND
#define __ARM_IMAPX_REGS_NAND
#define	NAND_TEST	1
/* Register Layout */

#define iMAPX200_NAND_BASE
#define RESERVED_RRTB_SIZE					0x4000
#define iMAPX200_NFCONF						0x00
#define iMAPX200_NFCONT						0x04
#define iMAPX200_NFCMD						0x08
#define iMAPX200_NFADDR						0x0c
#define iMAPX200_NFDATA						0x10
#define iMAPX200_NFMECCD0					0x14
#define iMAPX200_NFMECCD1					0x18
#define iMAPX200_NFMECCD2					0x1c
#define iMAPX200_NFSBLK						0x20
#define iMAPX200_NFEBLK						0x24
#define iMAPX200_NFSTAT						0x28
#define iMAPX200_NFESTAT0					0x2c
#define iMAPX200_NFESTAT1					0x30
#define iMAPX200_NFMECC0					0x34
#define iMAPX200_NFMECC1					0x38
#define iMAPX200_NFMECC2					0x3c
#define iMAPX200_NFESTAT2					0x40
#define iMAPX200_NFSECCD					0x44
#define iMAPX200_NFSECC						0x48
#define iMAPX200_NFDESPCFG					0x4c
#define iMAPX200_NFDESPADDR					0x50
#define iMAPX200_NFDESPCNT					0x54
#define iMAPX200_NFDECERRFLAG				0x58		/* if 1, spare ecc is error, else main ess is error */
#define iMAPX200_NFPAGECNT					0x5c		/* [0:3]: page count, [4:15]: unit count, means '6+1' counter */
#define iMAPX200_NFDMAADDR_A				0x80
#define iMAPX200_NFDMAC_A					0x84
#define iMAPX200_NFDMAADDR_B				0x88
#define iMAPX200_NFDMAC_B					0x8c
#define iMAPX200_NFDMAADDR_C				0x90
#define iMAPX200_NFDMAC_C					0x94
#define iMAPX200_NFDMAADDR_D				0x98
#define iMAPX200_NFDMAC_D					0x9c

#define iMAPX800_NAND_BASE

#define NF2AFTM				(0x00)		/* NAND Flash AsynIF timing */
#define NF2SFTM				(0x04)      	/* NAND Flash SynIF timing */
#define NF2TOUT				(0x08)      	/* NAND Flash RnB timeout cfg. */
#define NF2STSC				(0x0C)      	/* NAND Flash status check cfg. */
#define NF2STSR0			(0x10)      	/* NAND Flash Status register 0, read only */
#define NF2STSR1			(0x14)      	/* NAND Flash Status register 1, read only */
#define NF2ECCC				(0x18)       /*  ECC configuration */
#define NF2PGEC				(0x1C)       /*  Page configuration */
#define NF2RADR0			(0x20)		/* NAND Flash Row ADDR0 */
#define NF2RADR1			(0x24)		/* NAND Flash Row ADDR1 */
#define NF2RADR2			(0x28)		/* NAND Flash Row ADDR2 */
#define NF2RADR3			(0x2C)		/* NAND Flash Row ADDR3 */
#define NF2CADR				(0x30)       /* Column Address */
#define NF2AADR				(0x34)       /* ADMA descriptor address */
#define NF2SADR0			(0x38)       /* SDMA Ch0 address */
#define NF2SADR1			(0x3C)       /* SDMA Ch1 address */
#define NF2SBLKS			(0x40)       /* SDMA block size */
#define NF2SBLKN			(0x44)       /* SDMA block number */
#define NF2DMAC				(0x48)       /* DMA Configuration */
#define NF2CSR				(0x4C)       /* Chip select register */

#define NF2DATC			(0x50)       /* data interrupt configuration */
#define NF2FFCE			(0x54)       /* FIFO Configuration enable */
#define NF2AFPT			(0x58)       /* AFIFO access register */
#define NF2AFST			(0x5C)       /* AFIFO status register, read only */
#define NF2TFPT			(0x60)       /* TRX-AFIFO access register */
#define NF2TFST			(0x64)       /* TRX-AFIFO status register, read only */
#define NF2TFDE			(0x68)       /* TRX-AFIFO debug enable */
#define NF2DRDE			(0x6C)       /* DataRAM debug enable */
#define NF2DRPT			(0x70)       /* DataRAM read access register, read only */
#define NF2ERDE			(0x74)       /* ECCRAM debug enable */
#define NF2ERPT			(0x78)       /* ECCRAM access register */
#define NF2FFTH			(0x7C)       /* FiFo Threshold */

#define NF2SFTR			(0x80)       /* Soft reset register */
#define NF2STRR			(0x84)       /* Start register */
#define NF2INTA			(0x88)       /* interactive interrrupt ack register */
#define NF2INTR			(0x8C)       /* interrrupt status register */
#define NF2INTE			(0x90)       /* interrrupt enable register */
#define NF2WSTA			(0x94)       /* Work status register, read only */
#define NF2SCADR	    	(0x98)       /* SDMA current address, read only */
#define NF2ACADR	    	(0x9C)       /* ADMA current address, read only */
#define NF2IFST			(0xA0)       /* ISFIFO status register, read only */
#define NF2OFST			(0xA4)       /* OSFIFO status register, read only */
#define NF2TFST2	    	(0xA8)       /* TRX-FIFO status register2, read only */
#define NF2WDATL	    	(0xAC)       /* Write data low 32 bits. */
#define NF2WDATH	    	(0xB0)       /* Write data high 32 bits. */
#define NF2DATC2	    	(0xB4)       /* data interrupt configuration2 */

#define NF2PBSTL		(0xC0)       /* PHY burst length, byte unit */
#define NF2PSYNC	    	(0xC4)       /* PHY Sync mode cfg. */
#define NF2PCLKM	    	(0xC8)       /* PHY Clock mode cfg. */
#define NF2PCLKS	    	(0xCC)       /* PHY Clock STOP cfg. */
#define NF2PRDC			(0xD0)       /* PHY Read cfg. */
#define NF2PDLY			(0xD4)       /* PHY Delay line cfg. */
#define NF2RESTCNT		(0xD8)
#define NF2TRXST		(0xDC)

#define NF2STSR2			(0xE0)      	/* NAND Flash Status register 2, read only */
#define NF2STSR3			(0xE4)      	/* NAND Flash Status register 3, read only */
#define NF2ECCINFO0			(0xE8)		/* 0-3 block ecc */
#define NF2ECCINFO1			(0xEC)		/* 4-7 block ecc */
#define NF2ECCINFO2			(0xF0)		/* 8-11 block ecc */
#define NF2ECCINFO3			(0xF4)		/* 12-15 block ecc */
#define NF2ECCINFO4			(0xF8)		/* 16-19 block ecc */
#define NF2ECCINFO5			(0xFC)		/* 20-23 block ecc */
#define NF2ECCINFO6			(0x100)		/* 24-27 block ecc */
#define NF2ECCINFO7			(0x104)		/* 28-31 block ecc */
#define NF2ECCINFO8			(0x108)		/* ECC unfixed info */
#define NF2ECCINFO9			(0x10C)		/* secc ecc info, bit 7 vailed, bit 6 unfixed, 3-0 bit errors */
#define NF2ECCLOCA			(0x110)
#define NF2ECCLEVEL			(0x11C)
#define NF2SECCDBG0			(0x120)
#define NF2SECCDBG1			(0x124)
#define NF2SECCDBG2			(0x128)
#define NF2SECCDBG3			(0x12C)
#define NF2DATSEED0			(0x140)
#define NF2DATSEED1			(0x144)
#define NF2DATSEED2			(0x148)
#define NF2DATSEED3			(0x14C)
#define NF2ECCSEED0			(0x150)
#define NF2ECCSEED1			(0x154)
#define NF2ECCSEED2			(0x158)
#define NF2ECCSEED3			(0x15C)
#define NF2RANDMP			(0x160)
#define NF2RANDME			(0x164)
#define NF2DEBUG0			(0x168)
#define NF2DEBUG1			(0x16c)
#define NF2DEBUG2			(0x170)
#define NF2DEBUG3			(0x174)
#define NF2DEBUG4			(0x178)
#define NF2DEBUG5			(0x17c)
#define NF2DEBUG6			(0x180)
#define NF2DEBUG7			(0x184)
#define NF2DEBUG8			(0x188)
#define NF2DEBUG9			(0x18c)
#define NF2DEBUG10			(0x190)
#define NF2DEBUG11			(0x194)

#define NF2BSWMODE			(0x1B8) //bit[0] = b'1' (16bit mode), bit[0] = b'0' (8bit mode)
#define NF2BSWF0			(0x1BC)
#define NF2BSWF1			(0x1C0)

#define NF2RAND0			(0x1C4) //bit[0:15] = seed, bit[16:31] = cycles
#define NF2RAND1			(0x1C8) //bit[0] = start, frist wirte bit[0] = b'1' then write bit[0] = b'0'
#define NF2RAND2			(0x1CC) //bit[16] = state [0: finished] [1: busy], bit[0:15] = result

#define DIVCFG4_ECC			(SYSMGR_BASE_ADDR+0x060)
/* Defination of NFCONF */

#define iMAPX200_NFCONF_TWRPH0_(x)			((x)<<8)
#define iMAPX200_NFCONF_TWRPH1_(x)			((x)<<4)
#define iMAPX200_NFCONF_TACLS_(x)			((x)<<12)
#define iMAPX200_NFCONF_TMSK				(0x07)
#define iMAPX200_NFCONF_ECCTYPE4			(1<<24)		/* 1 for MLC(4-bit), 0 for SLC(1-bit). */
#define iMAPX200_NFCONF_BusWidth16			(1<<0)		/* 1 for 16bit, 0 for 8bit. */

/* Defination of NFCONT */

#define iMAPX200_NFCONT_MODE				(1<<0)
#define iMAPX200_NFCONT_Reg_nCE0			(1<<1)
#define iMAPX200_NFCONT_Reg_nCE1			(1<<2)
#define iMAPX200_NFCONT_InitSECC			(1<<4)
#define iMAPX200_NFCONT_InitMECC			(1<<5)
#define iMAPX200_NFCONT_SpareECCLock		(1<<6)
#define iMAPX200_NFCONT_MainECCLock			(1<<7)
#define iMAPX200_NFCONT_RnB_TransMode		(1<<8)
#define iMAPX200_NFCONT_EnRnBINT			(1<<9)
#define iMAPX200_NFCONT_EnIllegalAccINT		(1<<10)
#define iMAPX200_NFCONT_EnECCDecINT			(1<<12)
#define iMAPX200_NFCONT_EnECCEncINT			(1<<13)
#define iMAPX200_NFCONT_DMACompleteINT		(1<<14)
#define iMAPX200_NFCONT_DMABlockEndINT		(1<<15)
#define iMAPX200_NFCONT_INTMSK				(0x3f << 9)
#define iMAPX200_NFCONT_SoftLock			(1<<16)
#define iMAPX200_NFCONT_LockTight			(1<<17)
#define iMAPX200_NFCONT_ECCDirectionEnc		(1<<18)

/* Defination of NFSTAT */

#define iMAPX200_NFSTAT_RnB_ro				(1<<0)		/* 0 busy, 1 ready */
#define iMAPX200_NFSTAT_nFCE_ro				(1<<2)
#define iMAPX200_NFSTAT_nCE_ro				(1<<3)
#define iMAPX200_NFSTAT_RnB_TransDetect		(1<<4)		/* write 1 to clear */
#define iMAPX200_NFSTAT_IllegalAccess		(1<<5)		/* write 1 to clear(to be confirmed) */
#define iMAPX200_NFSTAT_ECCDecDone			(1<<6)		/* write 1 to clear */
#define iMAPX200_NFSTAT_ECCEncDone			(1<<7)		/* write 1 to clear */
#define iMAPX200_NFSTAT_DMA_COMPLETE		(1<<8)
#define iMAPX200_NFSTAT_DMA_BLOCKEND		(1<<9)
#define iMAPX200_NFSTAT_ProgramErr			(1<<10)		/* Err=1, OK=0, write 1 to clear */
#define iMAPX200_NFSTAT_DespEnd				(1<<11)		/* OK=1 */
#define iMAPX200_NFSTAT_DespECCErr			(1<<12)		/* if intr en in description, Err=1, OK=0, write 1 to clear */
#define iMAPX200_NFSTAT_ECCErrResult		(1<<13)

/* Defination of SLC NFESTAT0 */

#define iMAPX200_NFESTAT0_SLC_MErrType_ro_		(0)
#define iMAPX200_NFESTAT0_SLC_SErrType_ro_		(2)
#define iMAPX200_NFESTAT0_SLC_Err_MSK			(0x03)
#define iMAPX200_NFESTAT0_SLC_MByte_Loc_ro_		(7)
#define iMAPX200_NFESTAT0_SLC_SByte_Loc_ro_		(21)
#define iMAPX200_NFESTAT0_SLC_MByte_Loc_MSK		(0x7ff)
#define iMAPX200_NFESTAT0_SLC_SByte_Loc_MSK		(0xf)
#define iMAPX200_NFESTAT0_SLC_MBit_Loc_ro_		(4)
#define iMAPX200_NFESTAT0_SLC_SBit_Loc_ro_		(18)
#define iMAPX200_NFESTAT0_SLC_Bit_MSK			(0x7)

/* Defination of MLC NFESTAT0 & NFESTAT1 & NFESTAT2*/

#define iMAPX200_NFESTAT0_Busy_ro				(1<<31)
#define iMAPX200_NFESTAT0_Ready_ro				(1<<30)
#define iMAPX200_NFESTAT0_MLC_MErrType_ro_		(27)
#define iMAPX200_NFESTAT0_MLC_MErrType_MSK		(0x7)
#define	iMAPX200_NFESTAT0_MLC_Loc1_ro_			(0)
#define	iMAPX200_NFESTAT1_MLC_Loc2_ro_			(0)
#define	iMAPX200_NFESTAT1_MLC_Loc3_ro_			(9)
#define	iMAPX200_NFESTAT1_MLC_Loc4_ro_			(18)
#define iMAPX200_NFESTAT0_MLC_PT1_ro_			(16)
#define iMAPX200_NFESTAT2_MLC_PT2_ro_			(0)
#define iMAPX200_NFESTAT2_MLC_PT3_ro_			(9)
#define iMAPX200_NFESTAT2_MLC_PT4_ro_			(18)
#define iMAPX200_NFESTATX_MLC_PT_MSK			(0x1ff)
#define iMAPX200_NFESTATX_MLC_Loc_MSK			(0x1ff)

/* Defination of DMAC */
#define iMAPX200_NFDMAC_DMADIROut						(1<<25)
#define iMAPX200_NFDMAC_DMAAUTO							(1<<26)
#define iMAPX200_NFDMAC_DMAALT							(1<<27)
#define iMAPX200_NFDMAC_DMARPT							(1<<28)
#define iMAPX200_NFDMAC_DMAWIND							(1<<29)
#define iMAPX200_NFDMAC_DMARST							(1<<30)
#define iMAPX200_NFDMAC_DMAEN							(1<<31)

#define IMAPX800_DEFAULT_CLK			888
#define IMAPX800_NF_DELAY				4
#define NF_TRXF_FINISH_INT				(1)
#define NF_SDMA_FINISH_INT				(1<<5)
#define NF_RB_TIMEOUT_INT				(1<<13)
#define NF_SECC_UNCORRECT_INT			(1<<21)

#define NF_DMA_SDMA				(0)
#define NF_DMA_ADMA				(1)

#define NF_TRXF_START			(1<<0)
#define NF_ECC_START			(1<<1)
#define NF_DMA_START			(1<<2)

#define NF_CMD_CMD				(0)
#define NF_CMD_ADDR				(1)
#define NF_CMD_DATA_OUT			(2)
#define NF_CMD_DATA_IN			(3)

#define NF_FLAG_EXEC_DIRECTLY				(0)
#define NF_FLAG_EXEC_READ_RDY_WHOLE_PAGE	(1)
#define NF_FLAG_EXEC_READ_STATUS			(2)
#define NF_FLAG_EXEC_WRITE_WHOLE_PAGE		(3)
#define NF_FLAG_EXEC_CHECK_RB				(4)
#define NF_FLAG_EXEC_READ_WHOLE_PAGE		(5)
#define NF_FLAG_EXEC_CHECK_STATUS			(6)
#define NF_FLAG_EXEC_WAIT_RDY				(7)
#define NF_FLAG_DATA_FROM_INTER				(8)

#define NF_RANDOMIZER_ENABLE				(1)
#define NF_RANDOMIZER_DISABLE				(0)

#define NF_RESET				(1<<0)
#define TRXF_RESET				(1<<1)
#define ECC_RESET				(1<<2)
#define DMA_RESET				(1<<3)
#define FIFO_RESET				(1<<4)

#define ECCRAM_TWO_BUFFER		(0)
#define ECCRAM_FOUR_BUFFER		(1)
#define ECCRAM_EIGHT_BUFFER		(2)
#define ECC_DECODER				(0)
#define ECC_ENCODER				(1)

#define SECC0_RESULT_FLAG		(1<<6)
#define SECC0_RESULT_BITS_MASK	(0xf)

#define CONTROLLER_READY		0
#define CONTROLLER_BUSY			1
#define CONTROLLER_SUSPEND		2

#define DEFAULT_TIMING			0x31ffffff
#define DEFAULT_RNB_TIMEOUT		0xffffff
#define DEFAULT_PHY_READ		0x4023
#define DEFAULT_PHY_DELAY		0x400
#define DEFAULT_PHY_DELAY2		0x3818
#define	BUSW_8BIT				0
#define	BUSW_16BIT				1
#define ASYNC_INTERFACE			0
#define ONFI_SYNC_INTERFACE		1
#define TOGGLE_INTERFACE		2

#define NAND_BOOT_NAME		  "bootloader"
#define NAND_NORMAL_NAME	  "nandnormal"
#define NAND_NFTL_NAME		  "nandftl"

#define NAND_DEFAULT_OPTIONS			(NAND_ECC_BCH8_MODE)

#define INFOTM_NORMAL						0
#define INFOTM_MULTI_CHIP					1
#define INFOTM_MULTI_CHIP_SHARE_RB			2
#define INFOTM_CHIP_NONE_RB					4
#define INFOTM_INTERLEAVING_MODE			8
#define INFOTM_RANDOMIZER_MODE				16
#define INFOTM_RETRY_MODE					32
#define INFOTM_ESLC_MODE					64

#define INFOTM_BADBLK_POS					0
#define NAND_ECC_UNIT_SIZE					1024
#define NAND_SECC_UNIT_SIZE					16

#define NAND_BCH2_ECC_SIZE				4
#define NAND_BCH4_ECC_SIZE				8
#define NAND_BCH8_ECC_SIZE				16
#define NAND_BCH16_ECC_SIZE				28
#define NAND_BCH24_ECC_SIZE				44
#define NAND_BCH32_ECC_SIZE				56
#define NAND_BCH40_ECC_SIZE				72
#define NAND_BCH48_ECC_SIZE				84
#define NAND_BCH56_ECC_SIZE				100
#define NAND_BCH60_ECC_SIZE				108
#define NAND_BCH64_ECC_SIZE				112
#define NAND_BCH72_ECC_SIZE				128
#define NAND_BCH80_ECC_SIZE				140
#define NAND_BCH96_ECC_SIZE				168
#define NAND_BCH112_ECC_SIZE			196
#define NAND_BCH127_ECC_SIZE			224
#define NAND_BCH4_SECC_SIZE				8
#define NAND_BCH8_SECC_SIZE				12

#define NAND_ECC_OPTIONS_MASK			0x0000000f
#define NAND_PLANE_OPTIONS_MASK			0x000000f0
#define NAND_TIMING_OPTIONS_MASK		0x00000f00
#define NAND_BUSW_OPTIONS_MASK			0x0000f000
#define NAND_INTERLEAVING_OPTIONS_MASK	0x000f0000
#define NAND_RANDOMIZER_OPTIONS_MASK	0x00f00000
#define NAND_READ_RETRY_OPTIONS_MASK	0x0f000000
#define NAND_ESLC_OPTIONS_MASK			0xf0000000
#define NAND_ECC_BCH2_MODE				0x00000000
#define NAND_ECC_BCH4_MODE				0x00000001
#define NAND_ECC_BCH8_MODE				0x00000002
#define NAND_ECC_BCH16_MODE				0x00000003
#define NAND_ECC_BCH24_MODE				0x00000004
#define NAND_ECC_BCH32_MODE				0x00000005
#define NAND_ECC_BCH40_MODE				0x00000006
#define NAND_ECC_BCH48_MODE				0x00000007
#define NAND_ECC_BCH56_MODE				0x00000008
#define NAND_ECC_BCH60_MODE				0x00000009
#define NAND_ECC_BCH64_MODE				0x0000000a
#define NAND_ECC_BCH72_MODE				0x0000000b
#define NAND_ECC_BCH80_MODE				0x0000000c
#define NAND_ECC_BCH96_MODE				0x0000000d
#define NAND_ECC_BCH112_MODE			0x0000000e
#define NAND_ECC_BCH127_MODE			0x0000000f
#define NAND_SECC_BCH4_MODE				0x00000000
#define NAND_SECC_BCH8_MODE				0x00000001
#define NAND_TWO_PLANE_MODE				0x00000010
#define NAND_TIMING_MODE0				0x00000000
#define NAND_TIMING_MODE1				0x00000100
#define NAND_TIMING_MODE2				0x00000200
#define NAND_TIMING_MODE3				0x00000300
#define NAND_TIMING_MODE4				0x00000400
#define NAND_TIMING_MODE5				0x00000500
#define NAND_BUS16_MODE					0x00001000
#define NAND_INTERLEAVING_MODE			0x00010000
#define NAND_RANDOMIZER_MODE			0x00100000
#define NAND_READ_RETRY_MODE			0x01000000
#define NAND_ESLC_MODE					0x10000000

#define DEFAULT_T_REA					40
#define DEFAULT_T_RHOH					0

#define INFOTM_NAND_BUSY_TIMEOUT		0x60000
#define INFOTM_DMA_BUSY_TIMEOUT			0x100000
#define MAX_ID_LEN						8
#define MAX_REGISTER_LEN				32
#define MAX_RETRY_LEVEL					32
#define MAX_HYNIX_REGISTER_LEN			16
#define MAX_HYNIX_RETRY_LEVEL			16
#define MAX_ESLC_PARA_LEN				8

#define NAND_CMD_RANDOM_DATA_READ		0x05
#define NAND_CMD_PLANE2_READ_START		0x06
#define NAND_CMD_TWOPLANE_READ1_MICRO	0x32
#define NAND_CMD_TWOPLANE_PREVIOS_READ	0x60
#define NAND_CMD_TWOPLANE_READ1			0x5a
#define NAND_CMD_TWOPLANE_READ2			0xa5
#define NAND_CMD_TWOPLANE_WRITE2_MICRO	0x80
#define NAND_CMD_TWOPLANE_WRITE2		0x81
#define NAND_CMD_DUMMY_PROGRAM			0x11
#define NAND_CMD_ERASE1_END				0xd1
#define NAND_CMD_MULTI_CHIP_STATUS		0x78
#define NAND_CMD_SET_FEATURES			0xEF
#define NAND_CMD_GET_FEATURES			0xEE
#define SYNC_FEATURES			        0x10
#define ONFI_TIMING_ADDR				0x01
#define ONFI_READ_RETRY_ADDR			0x89
#define HYNIX_GET_PARAMETER				0x37
#define HYNIX_SET_PARAMETER				0x36
#define HYNIX_ENABLE_PARAMETER			0x16
#define SAMSUNG_SET_PARAMETER			0xA1
#define SANDISK_DYNAMIC_PARAMETER1		0x3B
#define SANDISK_DYNAMIC_PARAMETER2		0xB9
#define SANDISK_SWITCH_LEGACY			0x53
#define SANDISK_DYNAMIC_READ_ENABLE		0xB6
#define SANDISK_DYNAMIC_READ_DISABLE	0xD6
#define TOSHIBA_RETRY_PRE_CMD1			0x5C
#define TOSHIBA_RETRY_PRE_CMD2			0xC5
#define TOSHIBA_SET_PARAMETER			0x55
#define TOSHIBA_READ_RETRY_START1		0x26
#define TOSHIBA_READ_RETRY_START2		0x5D

#define HYNIX_OTP_SIZE					1026
#define HYNIX_OTP_OFFSET				2

#define PARAMETER_FROM_NONE				0
#define PARAMETER_FROM_NAND				1
#define PARAMETER_FROM_NAND_OTP			2
#define PARAMETER_FROM_IDS				3
#define PARAMETER_FROM_UBOOT			4

#define MAX_DMA_DATA_SIZE				0x8000
#define MAX_DMA_OOB_SIZE				0x2000
#define PAGE_MANAGE_SIZE				0x4
#define BASIC_RAW_OOB_SIZE				128

#define MAX_CHIP_NUM		4
#define MAX_PLANE_NUM		4
#define USER_BYTE_NUM		4
#define PARA_TYPE_LEN		4
#define PARA_SERIALS_LEN	64

#define NAND_STATUS_READY_MULTI			0x20

#define NAND_BLOCK_GOOD					0x5a
#define NAND_BLOCK_BAD					0xa5
#define NAND_MINI_PART_SIZE				0x1000000
#define MAX_ENV_BAD_BLK_NUM				128
#define NAND_MINI_PART_NUM				4
#define NAND_BBT_BLK_NUM				2
#define MAX_BAD_BLK_NUM					2000
#define MAX_MTD_PART_NUM				16
#define MAX_MTD_PART_NAME_LEN			24
#define ENV_NAND_MAGIC					"envx"
#define BBT_HEAD_MAGIC					"bbts"
#define BBT_TAIL_MAGIC					"bbte"
#define MTD_PART_MAGIC					"anpt"
#define SHIPMENT_BAD                    1
#define USELESS_BAD                     0
#define CONFIG_ENV_BBT_SIZE         	0x2000
#define ENV_BBT_SIZE                    (CONFIG_ENV_BBT_SIZE - (sizeof(uint32_t)))
#define NAND_SYS_PART_SIZE				0x10000000

#define RESERVED_PART_NAME		"reserved"
#define RESERVED_PART_SIZE		8

#define DEV_UBOOT0		0
#define DEV_UBOOT1		1
#define DEV_ITEM		2

#define MAX_SEED_CYCLE				256
#define IMAPX800_NORMAL_SEED_CYCLE	8
#define MAX_UBOOT0_BLKS				64
#define IMAPX800_BOOT_COPY_NUM  	  4
#define IMAPX800_BOOT_PAGES_PER_COPY  512
#define IMAPX800_UBOOT0_SIZE    	  0x8000
#define IMAPXX15_UBOOT0_UNIT    	  0x400
#define IMAPX820_UBOOT0_UNIT    	  0x800
#define IMAPX800_UBOOT00_SIZE  	      0x1000
#define IMAPX800_PARA_SAVE_SIZE		  0x800
#define IMAPX800_PARA_SAVE_PAGES	  0x800
#define IMAPX800_UBOOT_MAGIC    	  0x0318
#define IMAPX800_VERSION_MAGIC    	  0x1375
#define IMAPX9_NEW_IROM				  0xdb12
#define IMAPX800_UBOOT_MAX_BAD   	  200
#define IMAPX800_RETRY_PARA_OFFSET	  1020
#define IMAPX800_UBOOT_PARA_MAGIC	  0x616f6f74
#define VERSION_MAGIC                 0x20140701
#define IMAPX800_RETRY_PARA_MAGIC	  0x349a8756
#define IMAPX800_PARA_ONESEED_SAVE_PAGES		4

#define RESERVED_NAND_RRTB_NAME		"nandrrtb"
#define RESERVED_NAND_IDS_NAME		"idsrrtb"
#define RETRY_PARAMETER_MAGIC		0x8a7a6a5a
#define RETRY_PARAMETER_PAGE_MAGIC	0x8a6a4a2a
#define RRTB_NFTL_NODESAVE_MAGIC 	0x6c74666e
#define RETRY_PARAMETER_CMD_MAGIC	0xa56aa78a
#define IMAPX800_ESLC_PARA_MAGIC    0x45534c43

#define RRTB_RETRY_LEVEL_OFFSET		  	4
#define RRTB_MAGIC_OFFSET		  	  	8
#define RRTB_UBOOT0_PARAMETER_OFFSET  	0x14

#define RRTB_RETRY_PARAMETER_OFFSET		0x100
#define RRTB_RETRY_UBOOT_PARA_OFFSET	0x1000

#define INFOTM_PARA_STRING			"nandpara"
#define INFOTM_UBOOT0_STRING		"uboot0"
#define INFOTM_UBOOT1_STRING		"uboot1"
#define INFOTM_ITEM_STRING			"item"
#define INFOTM_BBT_STRING			"bbtt"
#define INFOTM_ENV_VALID_STRING		"envvalid"
#define INFOTM_ENV_FREE_STRING		"envfree"
#define INFOTM_UBOOT0_REWRITE_MAGIC	"infotm_uboot0_rewrite"

#define INFOTM_UBOOT0_OFFSET	 0
#define INFOTM_UBOOT1_OFFSET	 0x800000
#define INFOTM_ITEM_OFFSET	 	 0x1000000
#define INFOTM_MAGIC_PART_SIZE	 0x100000
#define INFOTM_MAGIC_PART_PAGES	 0x100

#define CDBG_NAND_ID_COUNT 			8
#define CDBG_MAGIC_NAND				0xdfbda458

#define CDBG_NF_DELAY(a, b) (a | (b << 8))
#define CDBG_NF_PARAMETER(a, b, c, d, e, f, g, h)	(a | (b << 2)	 \
		| (c << 4) | (d << 7)	 \
		| (e << 8) | (f << 16) | (g << 24) | (h << 28))

#define CDBG_NF_INTERFACE(x)	(x & 0x3)
#define CDBG_NF_BUSW(x)			((x >> 2) & 0x3)
#define CDBG_NF_CYCLE(x)		((x >> 4) & 0x7)
#define CDBG_NF_RANDOMIZER(x)	((x >> 7) & 1)
#define CDBG_NF_MLEVEL(x)		({int __t = (x >> 8) & 0xff;	 \
			(__t >= 80)? 12:	 \
			(__t >= 72)? 11:	 \
			(__t >= 64)? 10:	 \
			(__t >= 60)?  9:	 \
			(__t >= 56)?  8:	 \
			(__t >= 48)?  7:	 \
			(__t >= 40)?  6:	 \
			(__t >= 32)?  5:	 \
			(__t >= 24)?  4:	 \
			(__t >= 16)?  3:	 \
			(__t >=  8)?  2:	 \
			(__t >=  4)?  1: 0;})
#define CDBG_NF_SLEVEL(x) ((((x >> 16) & 0xff) >= 8)? 1: 0)
#define CDBG_NF_ECCEN(x) !!((x >> 8) & 0xffff)
#define CDBG_NF_ONFIMODE(x) ((x >> 24) & 0xf)
#define CDBG_NF_INTERNR(x) ((x >> 28) & 0x3)

struct infotm_nand_retry_parameter {
	int retry_level;
	int register_num;
	int parameter_source;
	int register_addr[MAX_REGISTER_LEN];
	int8_t parameter_offset[MAX_RETRY_LEVEL][MAX_REGISTER_LEN];
	int8_t otp_parameter[MAX_RETRY_LEVEL];
};

#define MAX_PAIRED_PAGES 256
#define START_ONLY	0
#define START_END	1

/*
 *(1)Author:summer
 *(2)Time:2014-5-15
 *(3)pri_warning,level 4
 *(4)pri_notice,level 5
 *(5)pri_info,level 6
 *(6)pri_debug,level 7
 *
*/

#define NAND_DEBUG(level,x...)  printk(level "[imapx_nand_x9] "x)
//level 0
#define pri_emerg(fmt,...)  do \
{\
	    printk(KERN_EMERG fmt,##__VA_ARGS__);\
}while(0)

//level 1
#define pri_alert(fmt,...)  do \
{\
	    printk(KERN_ALERT fmt,##__VA_ARGS__);\
}while(0)

//level 2
#define pri_crit(fmt,...)  do \
{\
	    printk(KERN_CRIT fmt,##__VA_ARGS__);\
}while(0)

//level 3
#define pri_err(fmt,...)  do \
{\
	    printk(KERN_ERR fmt,##__VA_ARGS__);\
}while(0)

//level 4
#define pri_warning(fmt,...)  do \
{\
	    printk(KERN_WARNING fmt,##__VA_ARGS__);\
}while(0)

//level 5
#define pri_notice(fmt,...)  do\
{\
	    printk(KERN_NOTICE fmt,##__VA_ARGS__);\
}while(0)

//level 6
#define pri_info(fmt,...) do \
{\
	    printk(KERN_INFO fmt,##__VA_ARGS__);\
}while(0)

//level 7		
#define pri_debug(fmt,...)  do \
{\
	    printk(KERN_DEBUG fmt,##__VA_ARGS__);\
}while(0)





struct infotm_nand_eslc_parameter {
	int register_num;
	int parameter_set_mode;
	int register_addr[MAX_ESLC_PARA_LEN];
	int8_t parameter_offset[2][MAX_ESLC_PARA_LEN];
	uint8_t paired_page_addr[MAX_PAIRED_PAGES][2];
};

enum cdbg_nand_timing {
	NF_tREA = 0,
	NF_tREH,
	NF_tCLS,
	NF_tCLH,
	NF_tWH,
	NF_tWP,
	NF_tRP,
	NF_tDS,
	NF_tCAD,
	NF_tDQSCK,  
};

struct infotm_nand_extend_para {
	unsigned max_seed_cycle;
	unsigned uboot0_rewrite_count;
};

struct cdbg_cfg_nand {
	uint32_t	magic;

	uint8_t		name[24];
	uint8_t		id[CDBG_NAND_ID_COUNT * 2];
	uint8_t		timing[12];

	uint16_t	pagesize;
	uint16_t	blockpages;		/* pages per block */
	uint16_t	blockcount;		/* blocks per device */
	uint16_t	bad0;			/* first bad block mark */
	uint16_t	bad1;			/* second bad block mark */

	uint16_t	sysinfosize;
	uint16_t	rnbtimeout;		/* ms */
	uint16_t	syncdelay;		/* percentage of a peroid [7:0] , phy cycle time (ns) [15:8]*/
	uint16_t	syncread;

	uint16_t	polynomial;
	uint32_t	seed[4];

	/* [1:  0]: interface: 0: legacy, 1: ONFI sync, 2: toggle
	 * [3:  2]: bus width: 0: 8-bit, 1: 16-bit
	 * [6:  4]: cycle
	 * [    7]: randomizer
	 * [15: 8]: main ECC bits
	 * [23:16]: spare ECC bits
	 * [27:24]: onfi timing mode
	 * [29:28]: inter chip nr
	 */
	uint32_t	parameter;
	uint32_t	resv0;		/* options */
	uint32_t	resv1;		/* retry parameter */
	uint32_t	resv2;		/* max seed cycle */
	uint32_t    resv3;          /* eslc parameter */
};
#if 0
struct infotm_nand_flash_dev {
	char *name;
	u8 id[MAX_ID_LEN];
	unsigned pagesize;
	unsigned chipsize;
	unsigned erasesize;
	unsigned oobsize;
	unsigned internal_chipnr;
	unsigned T_REA;
	unsigned T_WP;
	unsigned T_WH;
	unsigned T_REH;
	unsigned T_CLS;
	unsigned T_CLH;
	u8 onfi_mode;

	unsigned interface;
	unsigned bad0;
	unsigned bad1;
	unsigned seed[8];
	unsigned options;
	struct infotm_nand_retry_parameter *retry_parameter;
	unsigned max_seed_cycle;
	unsigned *diff_data_seed;
	unsigned *diff_sysinfo_seed;
	unsigned *diff_ecc_seed;
	unsigned *diff_secc_seed;
};
#endif

struct infotm_nand_part_info {
	char mtd_part_magic[4];
	char mtd_part_name[MAX_MTD_PART_NAME_LEN];
	uint64_t size;
	uint64_t offset;
	u_int32_t mask_flags;
};

struct nand_bbt_t {
    unsigned        reserved: 2;
    unsigned        bad_type: 2;
    unsigned        chip_num: 2;
    unsigned        plane_num: 2;
    int16_t         blk_addr;
};

struct infotm_nand_bbt_info {
	char bbt_head_magic[4];
	int  shipment_bad_num;
	int  total_bad_num;
	struct nand_bbt_t nand_bbt[MAX_BAD_BLK_NUM];
	char bbt_tail_magic[4];
};

struct env_valid_node_t {
	int16_t  ec;
	int16_t	phy_blk_addr;
	int16_t	phy_page_addr;
	int timestamp;
};

struct env_free_node_t {
	int16_t  ec;
	int16_t	phy_blk_addr;
	int dirty_flag;
	struct env_free_node_t *next;
};

struct env_oobinfo_t {
	char name[4];
    int16_t  ec;
    unsigned        timestamp: 15;
    unsigned       status_page: 1;
};

struct infotm_nandenv_info_t {
	struct mtd_info *mtd;
	struct env_valid_node_t *env_valid_node;
	struct env_free_node_t *env_free_node;
	u_char env_valid;
	u_char env_init;
	u_char part_num_before_sys;
	struct infotm_nand_bbt_info nand_bbt_info;
};

typedef	struct environment_s {
	uint32_t	crc;		/* CRC32 over data bytes	*/
	unsigned char	data[ENV_BBT_SIZE]; /* Environment data		*/
} env_t;


struct infotm_nand_ecc_desc{
    char * name;
    unsigned ecc_mode;
    unsigned ecc_unit_size;
    unsigned ecc_bytes;
    unsigned secc_mode;
    unsigned secc_unit_size;
    unsigned secc_bytes;
    unsigned page_manage_bytes;
    unsigned max_correct_bits;
};

struct infotm_nand_chip {
	struct list_head list;

	u8 mfr_type;
	unsigned onfi_mode;
	unsigned options;
	unsigned page_size;
	unsigned block_size;
	unsigned oob_size;
	unsigned virtual_page_size;
	unsigned virtual_block_size;
	u8 plane_num;
	u8 real_plane;
	u8 chip_num;
	u8 internal_chipnr;
	u8 half_page_en;
	unsigned internal_page_nums;

	unsigned internal_chip_shift;
	unsigned ecc_mode;
	unsigned secc_mode;
	unsigned secc_bytes;
	unsigned max_correct_bits;
	int retry_level_support;
	u8 ops_mode;
	u8 cached_prog_status;
	u8 max_ecc_mode;
	u8 prog_start;
	u8 short_mode;

	unsigned chip_enable[MAX_CHIP_NUM];
	unsigned rb_enable[MAX_CHIP_NUM];
	unsigned valid_chip[MAX_CHIP_NUM];
	unsigned retry_level[MAX_CHIP_NUM];
	void	**retry_cmd_list;
	struct infotm_nand_retry_parameter retry_parameter[MAX_CHIP_NUM];
	struct infotm_nand_eslc_parameter eslc_parameter[MAX_CHIP_NUM];
	unsigned page_addr;
	unsigned real_page;
	unsigned subpage_cache_page;
	unsigned char *subpage_cache_buf;
	unsigned char subpage_cache_status[MAX_CHIP_NUM][MAX_PLANE_NUM];
	int max_seed_cycle;
	uint16_t	data_seed[MAX_SEED_CYCLE][8];
	uint16_t	sysinfo_seed[MAX_SEED_CYCLE][8];
	uint16_t	ecc_seed[MAX_SEED_CYCLE][8];
	uint16_t	secc_seed[MAX_SEED_CYCLE][8];
	uint16_t	data_last1K_seed[MAX_SEED_CYCLE][8];
	uint16_t	ecc_last1K_seed[MAX_SEED_CYCLE][8];
	int ecc_status;
	int secc_status;
	int uboot0_size;
	int uboot0_unit;
	int uboot0_rewrite_count;
	int uboot0_blk[IMAPX800_BOOT_COPY_NUM];
	int uboot1_blk[IMAPX800_BOOT_COPY_NUM];
	int item_blk[IMAPX800_BOOT_COPY_NUM];
	int reserved_pages;
	int startup_cnt;
	int env_need_save;

	struct mtd_info			mtd;
	struct nand_chip		chip;
	struct notifier_block 	nb;
	//struct infotm_nandenv_info_t *infotm_nandenv_info;
	struct infotm_nand_bbt_info *nand_bbt_info;
	struct cdbg_cfg_nand *infotm_nand_dev;
	struct infotm_nand_ecc_desc 	*ecc_desc;
	struct cdev				nand_env_cdev;

	/* platform info */
	struct infotm_nand_platform	*platform;
	struct class      cls;

	/* device info */
	struct device			device;
	void					*priv;

	//plateform operation function
	int		(*infotm_nand_hw_init)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_cmdfifo_start)(struct infotm_nand_chip *infotm_chip, int timeout);
	void	(*infotm_nand_adjust_timing)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_options_confirm)(struct infotm_nand_chip *infotm_chip);
	void 	(*infotm_nand_cmd_ctrl)(struct infotm_nand_chip *infotm_chip, int cmd,  unsigned int ctrl);
	void	(*infotm_nand_select_chip)(struct infotm_nand_chip *infotm_chip, int chipnr);
	void	(*infotm_nand_get_controller)(struct infotm_nand_chip *infotm_chip);
	void	(*infotm_nand_release_controller)(struct infotm_nand_chip *infotm_chip);
	int	    (*infotm_nand_read_byte)(struct infotm_nand_chip *infotm_chip, uint8_t *data, int count, int timeout);
	void	(*infotm_nand_write_byte)(struct infotm_nand_chip *infotm_chip, uint8_t data);
	void	(*infotm_nand_command)(struct infotm_nand_chip *infotm_chip, unsigned command, int column, int page_addr, int chipnr);
	void	(*infotm_nand_set_retry_level)(struct infotm_nand_chip *infotm_chip, int chipnr, int retry_level);
	void    (*infotm_nand_set_eslc_para)(struct infotm_nand_chip *infotm_chip, int chipnr, int start);
	int		(*infotm_nand_get_retry_table)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_get_eslc_para)(struct infotm_nand_chip *infotm_chip);
	int		(*infotm_nand_wait_devready)(struct infotm_nand_chip *infotm_chip, int chipnr);
	int		(*infotm_nand_dma_read)(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode,  unsigned char *sys_info_buf, int sys_info_len, int secc_mode);
	int		(*infotm_nand_dma_write)(struct infotm_nand_chip *infotm_chip, unsigned char *buf, int len, int ecc_mode, unsigned char *sys_info_buf, int sys_info_len, int secc_mode, int prog_type);
	int		(*infotm_nand_hwecc_correct)(struct infotm_nand_chip *infotm_chip, unsigned size, unsigned oob_size);
	void	(*infotm_nand_save_para)(struct infotm_nand_chip *infotm_chip);
	int 	(*infotm_nand_saveenv)(void);
};

struct imapx800_nand_chip {
	struct list_head chip_list;

	int ce1_gpio_index;
	int ce2_gpio_index;
	int ce3_gpio_index;
	int power_gpio_index;

	unsigned chip_selected;
	unsigned rb_received;
	int drv_ver_changed;
	int hw_inited;

    struct  resource    *area;
    struct  clk         *sys_clk;
    struct  clk         *ecc_clk;
    void    __iomem     *regs;
    void    __iomem     *sys_rtc_regs;
    unsigned char    *resved_mem;
    int     vitual_fifo_start;
    int     vitual_fifo_offset;
    int 	controller_state;
    int     controller_claim;
    spinlock_t controller_lock;
    wait_queue_head_t wq;
	int		nand_irq_no;
	int		nand_irq_type;
	int		nand_irq_condition;
	wait_queue_head_t irq_wq;

	unsigned char *nand_data_buf;
	dma_addr_t data_dma_addr;
	unsigned char *sys_info_buf;
	dma_addr_t oob_dma_addr;

	u8 busw;
	int polynomial;
};

struct infotm_nand_platform {
	char *name;
	struct platform_nand_data platform_nand_data;
	struct infotm_nand_chip  *infotm_chip;
	struct cdbg_cfg_nand *infotm_nand_dev;
};

struct infotm_nand_device {
	struct infotm_nand_platform *infotm_nand_platform;
	u8 dev_num;
};

struct infotm_nand_para_save {
	uint32_t head_magic;
	uint32_t chip0_para_size;
	uint32_t timing;
	uint32_t boot_copy_num;
	int32_t uboot0_blk[IMAPX800_BOOT_COPY_NUM];
	int32_t uboot1_blk[IMAPX800_BOOT_COPY_NUM];
	int32_t item_blk[IMAPX800_BOOT_COPY_NUM];
	int32_t bbt_blk[NAND_BBT_BLK_NUM];
	int mfr_type;
	int valid_chip_num;
	int page_shift;
	int erase_shift;
	int pages_per_blk_shift;
	int oob_size;
	int seed_cycle;
	int uboot0_unit;
	uint16_t data_seed[IMAPX800_NORMAL_SEED_CYCLE][8];
	uint16_t ecc_seed[IMAPX800_NORMAL_SEED_CYCLE][8];
	uint32_t ecc_mode;
	uint32_t secc_mode;
	uint32_t ecc_bytes;
	uint32_t secc_bytes;
	uint32_t ecc_bits;
	uint32_t secc_bits;
	uint32_t nftl_node_magic;
	int32_t nftl_node_blk;
	uint32_t eslc_para_magic;
	int8_t eslc_para[2][MAX_ESLC_PARA_LEN];
	uint32_t retry_para_magic;
	int32_t retry_level_circulate;
	uint32_t retry_level_support;
	uint32_t retry_level[MAX_CHIP_NUM];
	uint32_t version_magic;
};

#define chip_list_to_imapx800(l)	container_of(l, struct infotm_nand_chip, list)
static inline struct imapx800_nand_chip *infotm_chip_to_imapx800(struct infotm_nand_chip *infotm_chip)
{
	return (struct imapx800_nand_chip *)infotm_chip->priv;
}

static inline struct imapx800_nand_chip *to_nand_chip(struct platform_device *pdev)
{
	return platform_get_drvdata(pdev);
}

static inline struct infotm_nand_chip *mtd_to_nand_chip(struct mtd_info *mtd)
{
	return container_of(mtd, struct infotm_nand_chip, mtd);
}

extern int16_t uboot0_default_seed[8];
extern unsigned imapx800_seed[][8];
extern unsigned imapx800_sysinfo_seed_1k[][8];
extern unsigned imapx800_ecc_seed_1k[][8];
extern unsigned imapx800_secc_seed_1k[][8];
extern unsigned imapx800_ecc_seed_2k[][8];
extern unsigned imapx800_ecc_seed_1k_16bit[][8];
extern unsigned imapx800_sysinfo_seed_8k[][8];
extern unsigned imapx800_ecc_seed_8k[][8];
extern unsigned imapx800_secc_seed_8k[][8];
extern unsigned imapx800_sysinfo_seed_16k[][8];
extern unsigned imapx800_ecc_seed_16k[][8];
extern unsigned imapx800_secc_seed_16k[][8];
extern struct cdbg_cfg_nand infotm_nand_flash_ids[];

extern int infotm_nand_init(struct infotm_nand_chip *infotm_chip);
extern int add_mtd_device(struct mtd_info *mtd);
extern int saveenv (void);
#endif
