#ifndef __IMAPX800_UDC_H__
#define __IMAPX800_UDC_H__

#define OTG_ADDR	USBO_BASE_ADDR

// Global CSR Map
#define GOTGCTL		(OTG_ADDR + 0x000)		// Control and Status Register
#define GOTGINT		(OTG_ADDR + 0x004)		// Interrupt Register
#define GAHBCFG		(OTG_ADDR + 0x008)		// AHB Configuration Register
#define GUSBCFG		(OTG_ADDR + 0x00c)		// USB Configuration Register
#define GRSTCTL		(OTG_ADDR + 0x010)		// Reset Register
#define GINTSTS		(OTG_ADDR + 0x014)		// Interrupt Register
#define GINTMSK		(OTG_ADDR + 0x018)		// Interrupt Mask Register
#define GRXSTSR		(OTG_ADDR + 0x01c)		// Receive Status Debug Read
#define GRXSTSP		(OTG_ADDR + 0x020)		// Status Read and Pop Registers
#define GRXFSIZ		(OTG_ADDR + 0x024)		// Receive FIFO Size Register
#define GNPTXFSIZ	(OTG_ADDR + 0x028)		// Non-Periodic Transmit FIFO Size Register
#define GNPTXSTS	(OTG_ADDR + 0x02c)		// Non-Periodic Transmit FIFO/Queue Status Register
#define GI2CCTL		(OTG_ADDR + 0x030)		// I2C Access Register
#define GPVNDCTL	(OTG_ADDR + 0x034)		// PHY Vendor Control Register
#define GGPIO		(OTG_ADDR + 0x038)		// General Purpose Input/Output Register
#define GUID		(OTG_ADDR + 0x03c)		// User ID Register
#define GSNPSID		(OTG_ADDR + 0x040)		// Synopsys ID Register
#define GHWCFG1		(OTG_ADDR + 0x044)		// User HW Config1 Register
#define GHWCFG2		(OTG_ADDR + 0x048)		// User HW Config2 Register
#define GHWCFG3		(OTG_ADDR + 0x04c)		// User HW Config3 Register
#define GHWCFG4		(OTG_ADDR + 0x050)		// User HW Config4 Register
#define GLPMCFG		(OTG_ADDR + 0x054)		// Core LPM Configuration Register
#define GPWRDN		(OTG_ADDR + 0x058)		// Power Down Register
#define GDFIFOCFG	(OTG_ADDR + 0x05c)		// DFIFO Software Config Register
#define GADPCTL		(OTG_ADDR + 0x060)		// ADP Timer, Control and Status Register
#define HPTXFSIZ	(OTG_ADDR + 0x100)		// Host Periodic Transmit FIFO Size Register
#define DPTXFSIZ(n)	(OTG_ADDR + (0x104 + (n-1)*4))	// Device Periodic Transmit FIFO-n Size Register   1 ≤ n ≤ 15
#define DIEPTXF(n)	(OTG_ADDR + (0x104 + (n-1)*4))	// Device IN Endpoint Transmit FIFO Size Register  1 ≤ n ≤ 15


#define USBCFG_ForceDevMode            (1 << 30)
#define USBCFG_PHYIf16                 (1 << 3)

#define GAHBCFG_HBstLen_Single          (0x0 << 1)
#define GAHBCFG_HBstLen_Incr            (0x1 << 1)
#define GAHBCFG_HBstLen_Incr4           (0x3 << 1)
#define GAHBCFG_HBstLen_Incr8           (0x5 << 1)
#define GAHBCFG_HBstLen_Incr16          (0x7 << 1)
#define GAHBCFG_GlblIntrEn              (1 << 0)

//Device Mode CSR Map
#define DCFG		(OTG_ADDR + 0x800)		// Device Configuration Register
#define DCTL		(OTG_ADDR + 0x804)		// Device Control Register
#define DSTS		(OTG_ADDR + 0x808)		// Device Status Register
#define DIEPMSK		(OTG_ADDR + 0x810)		// Device IN Endpoint Common Interrupt Mask Register
#define DOEPMSK		(OTG_ADDR + 0x814)		// Device OUT Endpoint Common Interrupt Mask Register
#define DAINT		(OTG_ADDR + 0x818)		// Device All Endpoints Interrupt Register
#define DAINTMSK	(OTG_ADDR + 0x81c)		// Device All Endpoints Interrupt Mask Register
#define DTKNQR1		(OTG_ADDR + 0x820)		// Device IN Token Sequence Learning Queue Read Register 1
#define DTKNQR2		(OTG_ADDR + 0x824)		// Device IN Token Sequence Learning Queue Read Register 2
#define DTKNQR3		(OTG_ADDR + 0x830)		// Device IN Token Sequence Learning Queue Read Register 3
#define DTKNQR4		(OTG_ADDR + 0x834)		// Device IN Token Sequence Learning Queue Read Register 4
#define DVBUSDIS	(OTG_ADDR + 0x828)		// Device VBUS Discharge Time Register
#define DVBUSPULSE	(OTG_ADDR + 0x82c)		// Device VBUS Pulsing Time Register
#define DTHRCTL		(OTG_ADDR + 0x830)		// Device Threshold Control Register
#define DIEPEMPMSK	(OTG_ADDR + 0x834)		// Device IN Endpoint FIFO Empty Interrupt Mask Register
#define DEACHINT	(OTG_ADDR + 0x838)		// Device Each Endpoint Interrupt Register
#define DEACHINTMSK	(OTG_ADDR + 0x83c)	// Device Each Endpoint Interrupt Register Mask
#define DIEPEACHMSK(n)	(OTG_ADDR + 0x840 + n*4)	// Device Each In Endpoint-n Interrupt Register
#define DOEPEACHMSK(n)	(OTG_ADDR + 0x880 + n*4)	// Device Each Out Endpoint-n Interrupt Register
#define DIEPCTL(n)	(OTG_ADDR + 0x900 + n*0x20)		// Device IN Endpoint-n Control Register
#define DIEPINT(n)	(OTG_ADDR + 0x908 + n*0x20)		// Device IN Endpoint-n Interrupt Register
#define DIEPTSIZ(n)	(OTG_ADDR + 0x910 + n*0x20)		// Device IN Endpoint-n Transfer Size Register
#define DIEPDMA(n)	(OTG_ADDR + 0x914 + n*0x20)		// Device IN Endpoint-n DMA Address Register
#define DTXFSTS(n)	(OTG_ADDR + 0x918 + n*0x20)		// Device Endpoint Transmit FIFO Status Register
#define DIEPDMAB(n)	(OTG_ADDR + 0x91c + n*0x20)		// Device IN Endpoint-n DMA Buffer Address Register
#define DOEPCTL(n)	(OTG_ADDR + 0xb00 + n*0x20)		// Device OUT Endpoint-n Control Register
#define DOEPINT(n)	(OTG_ADDR + 0xb08 + n*0x20)		// Device OUT Endpoint-n Interrupt Register
#define DOEPTSIZ(n)	(OTG_ADDR + 0xb10 + n*0x20)		// Device OUT Endpoint-n Transfer Size Register
#define DOEPDMA(n)	(OTG_ADDR + 0xb14 + n*0x20)		// Device OUT Endpoint-n DMA Address Register
#define DOEPDMAB(n)	(OTG_ADDR + 0xb1c + n*0x20)		// Device OUT Endpoint Transmit FIFO Status Register

// OTHER register
#define DFIFO(n)	(OTG_ADDR + 0x1000 *(n+1))
#define PCGCCTL		(OTG_ADDR + 0xe00)			//Power and Clock Gating Control Register





#endif
