/* imap_eth.h
 *
 * Header file for imap_eth.c, definetion of macro and declaration of
 * functions.
 *
 * Copyright (c) 2013 Shanghai InfoTMIC Co.,Ltd.
 *
 * Version:
 *
 * 1.1  Fix coding style
 *      xecle, 05/03/2013 02:20:05 PM
 *
 * 1.0  Created.
 *      xecle, 12/21/2013 04:21:59 PM
 *
 */

#ifndef _IMAP_GMAC_ETH_H_
#define _IMAP_GMAC_ETH_H_

#ifdef CONFIG_OMC300_ETH_PHY
#include <mach/ip2906.h>
#endif

#define MAC_ENV "mac_addr"
#define DEFAULT_LOOP_NUM 1000
#define DEFAULT_LOOP_DEALY 10

#define SPEED_10M	(0)
#define SPEED_100M	(1)
#define DUPLEX_HALF	(0)
#define DUPLEX_FULL	(1)

/* max transmit frames waiting in TX RING, should less than RING_TX_LENGTH */
#define MAX_TX_FRAME	10

/*
 * defination of tx and rx queue length,
 * tx ring length must be 2^N for entry loop
 */
#define RING_TX_LENGTH  16
#define RING_RX_LENGTH  128
#define DEFAULT_MTU	 0x600

#define NAPI_WEIGHT     16

/*
 * skb buffer entry
 */
struct ig_skb_entry {
	struct sk_buff *skb;
	struct sk_buff *skb2;
};

/*
 * DMA descriptor for ETH Rx and Tx
 */
struct ig_dma_des {
	uint32_t		status;
	uint32_t		length;
	dma_addr_t	  data1;
	dma_addr_t	  data2;
	void			*buff1;
	void			*buff2;
};
/* Default Descriptor Skip Length in Ring mode, 64it bus */
#define DEFAULT_DSL 0x1

/*
 * device private data struct, include all data used
 */
struct ig_adapter {
	struct device *dev;
	struct net_device *netdev;
	struct mii_if_info mii;
	void __iomem *base_addr;
	int phydev;
	int isolate;	/* gpio pin to control PHY isolate */
	struct napi_struct  napi;
	/* delayed work queue to check cable plug and unplug*/
	struct delayed_work work_queue;
	uint16_t delay_time;	/* dynamic delay time for work_queue */
	uint16_t delay_num;

	uint8_t mac_addr[6];  /* netcard physics net address*/

	uint8_t start;	  /* device opened */
	uint8_t	link_up;	/* Link status */
#ifdef CONFIG_OMC300_ETH_PHY
	uint8_t	first_time_link_up;	/* For OMC300 eth phy */
#endif

	/* mapped DMA tx descriptor ring base address */
	struct ig_dma_des *tx_des_base;
	/* mapped DMA rx descriptor ring base address */
	struct ig_dma_des *rx_des_base;
	/* Tx DMA descriptor ring physics base addreee */
	dma_addr_t tx_dma_base;
	/* Rx DMA descriptor ring physics base addreee */
	dma_addr_t rx_dma_base;

	uint32_t tx_entry;
	uint32_t current_tx_entry;
	struct ig_skb_entry tx_skb[RING_TX_LENGTH];
	/* current Rx descriptor used by DMA */
	struct ig_dma_des *current_rx_des;
};

/*
 * define PHY register and register mask
 */
#define DEFAULT_PHY_DEV	 0x1	 /* use the phy1 in board*/

enum ETHREGMAP {
	MCR	= 0x0000, /*This is the operation mode register for the MAC.*/
	MFF	= 0x0004, /*Contains the frame filtering controls.*/
	/*Contains the higher 32 bits of the Multicast Hash table.*/
	HTHR	= 0x0008,
	/*Contains the lower 32 bits of the Multicast Hash table.*/
	HTLR	= 0x000c,
	/*Controls the management cycles to an external PHY.*/
	GAR	= 0x0010,
	/*Contains the data to be written to or read from the PHY register.*/
	GDR	= 0x0014,
	FCR	= 0x0018, /*Controls the generation of control frames.*/
	VTR	= 0x001c, /*Identifies IEEE 802.1Q VLAN type frames.*/
	VERR	= 0x0020, /*Identifies the version of the Core*/
	/*Gives the status of various internal blocks for debugging*/
	DBGR	= 0x0024,
	/* Undefined 0x0028 Reserved */
	/* Undefined 0x002c Reserved */
	/* Undefined 0x0030 Reserved */
	/* Undefined 0x0034 Reserved */
	ISR	= 0x0038, /*Contains the interrupt status.*/
	IMR	= 0x003c, /*Contains the masks for generating the interrupts.*/
	MAHR0   = 0x0040, /*Contains the higher 16 bits of the MAC address 0*/
	MALR0   = 0x0044, /*Contains the lower 32 bits of the MAC address 0*/
	MAHR1   = 0x0048, /*Contains the higher 16 bits of the MAC address 1*/
	MALR1   = 0x004c, /*Contains the lower 32 bits of the MAC address 1*/
	/* ... */
	MAHR15  = 0x00b8, /*Contains the higher 16 bits of the MAC address 15*/
	MALR15  = 0x00bc, /*Contains the lower 32 bits of the MAC address 15*/
	/* Undefined 0x00c0~0x07fc Reserved */
	MAHR16  = 0x0800, /*Contains the higher 16 bits of the MAC address 16*/
	MALR16  = 0x0804, /*Contains the lower 32 bits of the MAC address 16*/
	/* ... */
	MAHR31  = 0x0878, /*Contains the higher 16 bits of the MAC address 31*/
	MALR31  = 0x087c, /*Contains the lower 32 bits of the MAC address 31*/
	/* Undefined 0x0880~0x0ffc Reserved */
	BMR	= 0x1000, /*Controls the Host Interface Mode.*/
	/*
	 * Used by the host to instruct the DMA to poll the Transmit
	 * Descriptor List.
	 */
	TPDR	= 0x1004,
	/*
	 * Used by the Host to instruct the DMA to poll the Receive
	 * Descriptor list.
	 */
	RPDR	= 0x1008,
	/*Points the DMA to the start of the Receive Descriptor list.*/
	RDLAR   = 0x100c,
	/*Points the DMA to the start of the Transmit Descriptor List.*/
	TDLAR   = 0x1010,
	/*
	 * The Software driver (application) reads this register during
	 * interrupt service routine or polling to determine the status
	 * of the DMA.
	 */
	DMASR   = 0x1014,
	/*Establishes the Receive and Transmit operating modes and command.*/
	OMR	= 0x1018,
	/*Enables the interrupts reported by the Status Register.*/
	IER	= 0x101c,
	/*
	 * Contains the counters for discarded frames because no host
	 * Receive Descriptor was available, and discarded frames because
	 * of Receive FIFO Overflow.
	 */
	MFBOCR  = 0x1020,
	/*Watchdog time-out for Receive Interrupt (RI) from DMA*/
	RIWTR   = 0x1024,
	/*
	 * Controls AXI Master behavior (mainly controls burst splitting
	 * and number of outstanding requests).
	 */
	ABMR	= 0x1028,
	/*Gives the idle status of the AXI master's read/write channels.*/
	AXISR   = 0x102c,

	/* Undefined = 0x1030~0x1044 Reserved */

	/*Points to the start of current Transmit Descriptor read by the DMA.*/
	CHTDR   = 0x1048,
	/*Points to the start of current Receive Descriptor read by the DMA.*/
	CHRDR   = 0x104c,
	/*Points to the current Transmit Buffer address read by the DMA.*/
	CHTBAR  = 0x1050,
	/*Points to the current Receive Buffer address read by the DMA.*/
	CHRBAR  = 0x1054,
	/*Indicates the presence of the optional features of the core.*/
	HWFR	= 0x1058,
};
#define MA_MAX	31
#define MA_REG(x) (0x40+(x/16)*0x7c0+(x%16)*8)
#define MAHR(x) (0x40+(x/16)*0x7c0+(x%16)*8)
#define MALR(x) (0x44+(x/16)*0x7c0+(x%16)*8)

/*
 * MAC Configuration register mask
 */
enum MACCONFIGREG {
	SFTERR	= 0x04000000,	 /* SMII Force Transmit Error */
	CST	=   0x02000000,	 /* CRC stripping for Type frames */
	TC	=   0x01000000,	 /* Transmit Configuration in RGMII/SGMII/SMII*/
	WD	=   0x00800000,	 /* Watchdog Disable */
	JD	=   0x00400000,	 /* Jabber Disable */
	BE	=   0x00200000,	 /* Frame Burst Enable */
	JE	=   0x00100000,	 /* Jumbo Frame Enable */

	/* Inter Frame Gap */
	IFG7	=   0x000E0000,	 /* 40 bit times */
	IFG6	=   0x000C0000,	 /* 48 bit times */
	IFG5	=   0x000A0000,	 /* 56 bit times */
	IFG4	=   0x00080000,	 /* 64 bit times */
	IFG3	=   0x00060000,	 /* 72 bit times */
	IFG2	=   0x00040000,	 /* 80 bit times */
	IFG1	=   0x00020000,	 /* 88 bit times */
	IFG0	=   0x00000000,	 /* 96 bit times */

	DCRS	=   0x00010000,	 /* Disable Carrier Sense During Transmission */
	/* bit 15 undefined */
	FES	=   0x00004000,	 /* Speed */
	DO	=   0x00002000,	 /* Disable Receive Own */
	LM	=   0x00001000,	 /* Loopback Mode */
	DM	=   0x00000800,	 /* Duplex Mode */
	/* bit 10 undefined */
	DR	=   0x00000200,	 /* Disable Retry */
	/* bit 8 undefined */
	ACS	=   0x00000080,	 /* Automatic Pad/CRC Stripping */

	/* Back-Off Limit */
	BL3	=   0x00000060,	 /* BL 1 */
	BL2	=   0x00000040,	 /* BL 4 */
	BL1	=   0x00000020,	 /* BL 8 */
	BL0	=   0x00000000,	 /* BL 10 */

	DC	=   0x00000010,	 /* Deferral Check */
	TE	=   0x00000008,	 /* Transmitter Enable */
	RE	=   0x00000004,	 /* Receiver Enable */
};

/*
 * MAC Frame Filter
 */
enum MACFRAMEFILTER {
	RA  =   0x80000000,	 /* Receive All */
	/* bits 30:11 undefined */
	HPF =   0x00000400,	 /* Hash or Perfect Filter */
	SAF =   0x00000200,	 /* Source Address Filter */
	SAIF =   0x00000100,	 /* SA Inverse Filtering */
	/* Pass Control Frames */
	/* MAC forwards control frames that pass the Address Filter */
	PCF3 =   0x000000C0,
	/*
	 * MAC forwards all control frames to application even if
	 * they fail the Address Filter
	 */
	PCF2 =   0x00000080,
	/*
	 * MAC forwards all control frames except PAUSE control frames
	 * to application even if they fail the Address filter
	 */
	PCF1 =   0x00000040,
	/* MAC filters all control frames from reaching the application */
	PCF0 =   0x00000000,
	DBF =   0x00000020,	 /* Disable Broadcast Frames */
	PM  =   0x00000010,	 /* Pass All Multicast */
	DAIF =   0x00000008,	 /* DA Inverse Filtering */
	HMC =   0x00000004,	 /* Hash Multicast */
	HUC =   0x00000002,	 /* Hash Unicast */
	PR  =   0x00000001,	 /* Promiscuous Mode */
};

/*
 * GMII Address register mask
 */
enum GMIIADDRREG {
	/* bits 15~11 Phy Layer address */
	GMIIPHYMASK	= 0x0F80,
	GMIIPHYSHIFT = 11,
	/* bits 10~6 GMII register in Phy device */
	GMIIREGMASK	= 0x07C0,
	GMIIREGSHIFT = 6,

	CLK_CSR_5	= 0x0014,	/* 250-300MHz  clk_csr_i/122 */
	CLK_CSR_4	= 0x0010,	/* 150-250MHz  clk_csr_i/102 */
	CLK_CSR_3	= 0x000C,	/* 35-60MHz	clk_csr_i/26 */
	CLK_CSR_2	= 0x0008,	/* 20-35MHz	clk_csr_i/16 */
	CLK_CSR_1	= 0x0004,	/* 100-150MHz  clk_csr_i/62 */
	CLK_CSR_0	= 0x0000,	/* 60-100MHz   clk_csr_i/42 */

	CLK_CSR_X	= 0x0024,	/* 8-16MHz	 clk_csr_i/6 */

	GMIIWRITE	= 0x0002,	/* GMII write  */
	GMIIBUSY	= 0x0001,
};

/*
 * Flow Control Register
 */
enum FLOWCONTROLREG {
	PTMASK  = 0x7FFE0000,
	PTSHIFT = 16,
	/* bits 15:8 undefined */
	DZPQ	= 0x00000080,
	/* bits 6 undefined */
	PLT3	= 0x00000030,	 /* Pause Low Threshold, minus 256 slot */
	PLT2	= 0x00000020,	 /* Pause Low Threshold, minus 144 slot */
	PLT1	= 0x00000010,	 /* Pause Low Threshold, minus 28 slot */
	PLT0	= 0x00000000,	 /* Pause Low Threshold, minus 4 slot */

	UP	= 0x00000008,	 /* Unicast Pause Frame Detect */
	RFE	= 0x00000004,	 /* Receive Flow Control Enable */
	TFE	= 0x00000002,	 /* Transmit Flow Control Enable */
	FCB	= 0x00000001,	 /* Flow Control Busy/Backpressure Activate */
};
#define DEFAULT_PT  0x200

/*
 * Bus Mode Register
 */
enum BUSMODEREG {
	ALL	= 0x02000000,	 /* Address-Aligned Beats */
	PBLx8   = 0x01000000,	 /*  */
	USP	= 0x00800000,	 /*  */
	RPBLMASK = 0x007E0000,	 /* bits 22~17 */
	RPBLSHIFT =  17,
	FB	= 0x00010000,	 /* Fixed Burst */
	FR4	= 0x0000C000,	 /* Rx:Tx priority ratio 4:1 */
	FR3	= 0x00008000,	 /* Rx:Tx priority ratio 3:1 */
	FR2	= 0x00004000,	 /* Rx:Tx priority ratio 2:1 */
	FR1	= 0x00000000,	 /* Rx:Tx priority ratio 1:1 */
	PBLMASK	= 0x00003F00,	 /* bits 13~8 Programmable Burst Length */
	PBLSHIFT = 8,
	DSLMASK = 0x0000007C,	 /* bits 6~2 */
	DSLSHIFT = 2,
	SWR	= 0x00000001,	 /* Software Reset */
};

#define DEFAULT_PBL 0x8

/*
 * DMA Status Register
 */
enum STATUSREG {
	/* bits 31:30 undefined */
	TTI	= 0x20000000,	 /* Time-Stamp Trigger Interrupt */
	GPI	= 0x10000000,
	GMI	= 0x08000000,
	GLI	= 0x04000000,

	/* Error Bits */
	/* Error during access, 1 for descriptor, 0 for data buffer */
	EB_AC   = 0x02000000,
	EB_RW   = 0x01000000, /* Error during transfer, 1 in read, 0 in write */
	/* Error during data transfer, 1 by TxDMA, 0 by RxDMA */
	EB_DT   = 0x00800000,

	/* bits 22:20 defined Transmit Process State */
	/* bits 19:17 defined receive Process State */

	NIS	= 0x00010000,	 /* Normal Interrupt Summary */
	AIS	= 0x00008000,	 /* Abnormal Interrupt Summary */
	ERI	= 0x00004000,	 /* Early Receive Interrupt */
	FBI	= 0x00002000,	 /* Fatal Bus Error Interrupt */
	/* bits 12:11 undefined */
	ETI	= 0x00000400,	 /* Early Transmit Interrupt */
	SRWT	= 0x00000200,	 /* Receive Watchdog Timeout */
	RPS	= 0x00000100,	 /* Receive Process Stopped */
	RU	= 0x00000080,	 /* Receive Buffer Unavailable */
	RI	= 0x00000040,	 /* Receive Interrupt  */
	UNF	= 0x00000020,	 /* Transmit Underflow */
	OVF	= 0x00000010,	 /* Receive Overflow */
	STJT	= 0x00000008,	 /* Transmit Jabber Timeout */
	TU	= 0x00000004,	 /* Transmit Buffer Unavailable */
	TPS	= 0x00000002,	 /* Transmit Process Stopped */
	TI	= 0x00000001,	 /* Transmit Interrupt */
};

/*
 * Operation Mode Register
 */
enum OPERATIONMODEREG {
	/* bits 31:26 undefined */
	RSF	 =   0x02000000, /* Receive Store and Forward */
	DFF	 =   0x01000000, /* Disable Flushing of Received Frames */
	/* bits 23:22 undefined */
	TSF	 =   0x00200000, /* Transmit Store and Forward */
	FTF	 =   0x00100000, /* Flush Transmit FIFO */
	/* bits 19:17 undefined */

	TTC7	= 0x0001C000,	 /* Transmit Threshold Control 16 */
	TTC6	= 0x00018000,	 /* Transmit Threshold Control 24 */
	TTC5	= 0x00014000,	 /* Transmit Threshold Control 32 */
	TTC4	= 0x00010000,	 /* Transmit Threshold Control 40 */
	TTC3	= 0x0000C000,	 /* Transmit Threshold Control 256 */
	TTC2	= 0x00008000,	 /* Transmit Threshold Control 192 */
	TTC1	= 0x00004000,	 /* Transmit Threshold Control 128 */
	TTC0	= 0x00000000,	 /* Transmit Threshold Control 64 */

	ST	= 0x00002000,	 /* Start/Stop Transmission Command */
	/* bits 12:8 undefined */
	FEF	= 0x00000080,	 /* Forward Error Frames */
	FUF	= 0x00000040,	 /* Forward Undersized Good Frames */
	/* bit 5 undefined */
	RTC3	= 0x00000018,	 /* Receive Threshold Control 128 */
	RTC2	= 0x00000010,	 /* Receive Threshold Control 96 */
	RTC1	= 0x00000008,	 /* Receive Threshold Control 32 */
	RTC0	= 0x00000000,	 /* Receive Threshold Control 64 */
	OSF	= 0x00000004,	 /* Operate on Second Frame */
	SR	= 0x00000002,	 /* Start/Stop Receive */
	/* bit 0 undefined */
};

/*
 * Interrupt Enable Register
 */
enum INTERRUPTENREG {
	/* bits 31:17 undefined */
	NIE	 = 0x00010000,	 /* Normal Interrupt Summary Enable */
	AIE	 = 0x00008000,	 /* Abnormal Interrupt Summary Enable */
	ERE	 = 0x00004000,	 /* Early Receive Interrupt Enable */
	FBE	 = 0x00002000,	 /* Fatal Bus Error Enable */
	/* bits 12:11 undeifned */
	ETE	 = 0x00000400,	 /* Early Transmit Interrupt Enable */
	RWE	 = 0x00000200,	 /* Receive Watchdog Timeout Enable */
	RSE	 = 0x00000100,	 /* Receive Stopped Enable */
	RUE	 = 0x00000080,	 /* Receive Buffer Unavailable Enable */
	RIE	 = 0x00000040,	 /* Receive Interrupt Enable */
	UNE	 = 0x00000020,	 /* Underflow Interrupt Enable */
	OVE	 = 0x00000010,	 /* Overflow Interrupt Enable */
	TJE	 = 0x00000008,	 /* Transmit Jabber Timeout Enable */
	TUE	 = 0x00000004,	 /* Transmit Buffer Unavailable Enable */
	TSE	 = 0x00000002,	 /* Transmit Stopped Enable */
	TIE	 = 0x00000001,	 /* Transmit Interrupt Enable */
};

/*
 * MAC Frame Filter 
 */
enum {
	MF_AE	= 0x80000000,
};


/*
 * DMA Descriptor
 */
enum DMATXDES {
	/* TDES0 */
	TOWN	= 0x80000000,
	/* bits 30:18 undefined */
	TTSS	= 0x00020000,
	TIHE	= 0x00010000,
	TES	= 0x00008000,
	TJT	= 0x00004000,
	TFF	= 0x00002000,
	TPCE	= 0x00001000,
	TCL	= 0x00000800,
	TNC	= 0x00000400,
	TLC	= 0x00000200,
	TEC	= 0x00000100,
	TVF	= 0x00000080,
	TCCMASK = 0x00000078,
	TED	= 0x00000004,
	TUF	= 0x00000002,
	TDB	= 0x00000001,

	/* TDES1 */
	TIC	= 0x80000000,
	TLS	= 0x40000000,
	TFS	= 0x20000000,
	/* IP Header and payload enabled, pseudo-header in hardware */
	TCIC3   = 0x10000000,
	/* IP header and payload enabled, pseudo-header not in hardware */
	TCIC2   = 0x18000000,
	/* Only IP header enabled */
	TCIC1   = 0x08000000,
	/* Checksum Insertion Disabled */
	TCIC0   = 0x00000000,
	TDC	= 0x04000000,
	TER	= 0x02000000,
	TCH	= 0x01000000,
	TDP	= 0x00800000,
	TTSE	= 0x00400000,
	TBS2MASK = 0x003FF100,
	TBS2SHIFT = 11,
	TBS1MASK = 0x000007FF,
	TBS1SHIFT = 0,
};
enum DMARXDES {
	/* RDES0 */
	ROWN	= 0x80000000,
	RAFM	= 0x40000000,
	RFLMASK = 0x3FFF0000,
	RFLSHIFT = 16,
	RES	= 0x00008000,
	RDE	= 0x00004000,
	RSAF	= 0x00002000,
	RLE	= 0x00001000,
	ROE	= 0x00000800,
	RVLAN   = 0x00000400,
	RFS	= 0x00000200,
	RLS	= 0x00000100,
	RICE	= 0x00000080,
	RLC	= 0x00000040,
	RFT	= 0x00000020,
	RWT	= 0x00000010,
	RRE	= 0x00000008,
	RDBE	= 0x00000004,
	RCE	= 0x00000002,
	RPCE	= 0x00000001,

	/* RDES1 */
	RDIC	= 0x80000000,
	/* bits 30:26 undefined */
	RER	= 0x02000000,
	RCH	= 0x01000000,
	/* bits 23:22 undefined */
	RBS2MASK = 0x003FF100,
	RBS2SHIFT = 11,
	RBS1MASK = 0x000007FF,
	RBS1SHIFT = 0,
};

/* Imapx800 System control register for ETH */
enum SYSMGRMACREGISTER {
	SYSMGR_MAC_RESET	= SYSMGR_MAC_BASE + 0x00,
	SYSMGR_MAC_CLK_EN	= SYSMGR_MAC_BASE + 0x04,
	SYSMGR_MAC_POWER	= SYSMGR_MAC_BASE + 0x08,
	SYSMGR_MAC_BUS_EN	= SYSMGR_MAC_BASE + 0x0C,
	SYSMGR_MAC_BUS_QOS	= SYSMGR_MAC_BASE + 0x10,
	SYSMGR_MAC_ISO_EN	= SYSMGR_MAC_BASE + 0x18,
	SYSMGR_MAC_RMII		= SYSMGR_MAC_BASE + 0x20,
};

/* ImapX series may have a MAC address in efuse, formated uint32_t[6] */
#define IMAP_ETH_MAC_ADDR   (IMAP_SYSMGR_VA + 0xD99C)
/*
 * The MAC addresses owned by Shanghai Infotmic CO.,LTD. should be
 * 28:E2:97:XX:XX:XX
 */
#define IMAP_ETH_MAC_PREFIX {0x28, 0xE2, 0x97, 0x00, 0x00, 0x00}

extern int ip2906_write(uint8_t reg, uint16_t val);
extern int ip2906_read(uint8_t reg, uint16_t *val);

#endif  /* _IMAP_GMAC_ETH_H_*/

