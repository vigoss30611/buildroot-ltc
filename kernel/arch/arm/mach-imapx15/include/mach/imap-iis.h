#ifndef __IMAPX_IIS__
#define __IMAPX_IIS__

#define IER				(0x00)	/*Enable Register*/
#define IRER			(0x04)	/*Receiver Block Enable Register*/
#define ITER			(0x08)	/*Transmitter Block Enable Register*/
#define CER             (0x0C)	/*Clock Enable Register*/
#define CCR				(0x10)	/*Clock Configuration Register*/
#define RXFFR           (0x14)	/*Receiver Block FIFO Register*/
#define TXFFR           (0x18)	/*Transmitter Block FIFO Register*/
#define LRBR0           (0x20)	/*Left Receive Buffer 0*/
#define LTHR0           (0x20)	/*Left Transmit Holding Register 0*/
#define RRBR0           (0x24)	/*Right Receive Buffer 0*/
#define RTHR0           (0x24)	/*Right Transmit Holding Register 0*/
#define RER0            (0x28)	/*Receive Enable Register 0*/
#define TER0            (0x2C)	/*Transmit Enable Register 0*/
#define RCR0            (0x30)	/*Receive Configuration Register 0*/
#define TCR0            (0x34)	/*Transmit Configuration Register 0*/
#define ISR0            (0x38)	/*Interrupt Status Register 0*/
#define IMR0            (0x3C)	/*Interrupt Mask Register 0*/
#define ROR0            (0x40)	/*Receive Overrun Register 0*/
#define TOR0            (0x44)	/*Transmit Overrun Register 0*/
#define RFCR0           (0x48)	/*Receive FIFO Configuration Register 0*/
#define TFCR0           (0x4C)	/*Transimit FIFO Configuration Register 0*/
#define RFF0            (0x50)	/*Receive FIFO Flush 0*/
#define TFF0            (0x54)	/*Transmit FIFO Flush 0*/
#define LRBR1           (0x60)	/*Left Receive Buffer 1*/
#define LTHR1           (0x60)	/*Left Transmit Holding Register 1*/
#define RRBR1           (0x64)	/*Right Receive Buffer 1*/
#define RTHR1           (0x64)	/*Right Transmit Holding Register 1*/
#define RER1            (0x68)	/*Receive Enable Register 1*/
#define TER1            (0x6C)	/*Transmit Enable Register 1*/
#define RCR1            (0x70)	/*Receive Configuration Register 1*/
#define TCR1            (0x74)	/*Transmit Configuration Register 1*/
#define ISR1            (0x78)	/*Interrupt Status Register 1*/
#define IMR1            (0x7C)	/*Interrupt Mask Register 1*/
#define ROR1            (0x80)	/*Receive Overrun Register 1*/
#define TOR1            (0x84)	/*Transmit Overrun Register 1*/
#define RFCR1           (0x88)	/*Receive FIFO Configuration Register 1*/
#define TFCR1           (0x8C)	/*Transmit FIFO Configuration Register 1*/
#define RFF1            (0x90)	/*Receive FIFO Flush 1*/
#define TFF1            (0x94)	/*Transmit FIFO Flush 1*/
#define LRBR2           (0xA0)	/*Left Receive Buffer 2*/
#define LTHR2           (0xA0)	/*Left Transmit Holding Register 2*/
#define RRBR2           (0xA4)	/*Right Receive Buffer2*/
#define RTHR2           (0xA4)	/*Right Transmit Holding Register 2*/
#define RER2            (0xA8)	/*Receive Enable Register 2*/
#define TER2            (0xAC)	/*Transmit Enable Register 2*/
#define RCR2            (0xB0)	/*Receive Configuration Register 2*/
#define TCR2            (0xB4)	/*Transmit Configuration Register 2*/
#define ISR2            (0xB8)	/*Interrupt Status Register 2*/
#define IMR2            (0xBC)	/*Interrupt Mask Register 2*/
#define ROR2            (0xC0)	/*Receive Overrun Register 2*/
#define TOR2            (0xC4)	/*Transmit Overrun Register 2*/
#define RFCR2           (0xC8)	/*Receive FIFO Configuration Register 2*/
#define TFCR2           (0xCC)	/*Transmit FIFO Configuration Register 2*/
#define RFF2            (0xD0)	/*Receive FIFO Flush 2*/
#define TFF2            (0xD4)	/*Transmit FIFO Flush 2*/
#define LRBR3           (0xE0)	/*Left Receive Buffer 3*/
#define LTHR3           (0xE0)	/*Left Transmit Holding Register 3*/
#define RRBR3           (0xE4)	/*Right Receive Buffer3*/
#define RTHR3           (0xE4)	/*Right Transmit Holding Register 3*/
#define RER3            (0xE8)	/*Receive Enable Register 3*/
#define TER3            (0xEC)	/*Transmit Enable Register 3*/
#define RCR3            (0xF0)	/*Receive Configuration Register 3*/
#define TCR3            (0xF4)	/*Transmit Configuration Register 3*/
#define ISR3            (0xF8)	/*Interrupt Status Register 3*/
#define IMR3            (0xFC)	/*Interrupt Mask Register 3*/
#define ROR3            (0x100)	/*Receive Overrun Register 3*/
#define TOR3            (0x104)	/*Transmit Overrun Register 3*/
#define RFCR3           (0x108)	/*Receive FIFO Configuration Register 3*/
#define TFCR3           (0x10C)	/*Transmit FIFO Configuration Register 3*/
#define RFF3            (0x110)	/*Receive FIFO Flush 3*/
#define TFF3            (0x114)	/*Transmit FIFO Flush 3*/
#define RXDMA           (0x1C0)	/*Receiver Block DMA Register*/
#define RRXDMA          (0x1C4)	/*Reset Receiver Block DMA Register*/
#define TXDMA           (0x1C8)	/*Transmitter Block DMA Register*/
#define RTXDMA          (0x1CC)	/*Reset Transmitter Block DMA Register*/

#define I2S_MCLKDIV     (0x20003004)	/*Mclk Configuration Register*/
#define I2S_BCLKDIV     (0x20003008)	/*Bclk Configuration Register*/
#define I2S_INTERFACE	(0x20003000)	/*interface*/

#endif
