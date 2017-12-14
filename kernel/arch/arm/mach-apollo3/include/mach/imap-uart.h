#ifndef __IMAPX_UART__
#define __IMAPX_UART__

#ifndef __ASSEMBLY__

/* struct imapx200_uart_clksrc
 *
 * this structure defines a named clock source that can be used for the uart,
 * so that the best clock can be selected for the requested baud rate.
 *
 * min_baud and max_baud define the range of baud-rates this clock is
 * acceptable for, if they are both zero, it is assumed any baud rate that
 * can be generated from this clock will be used.
 *
 * divisor gives the divisor from the clock to the one seen by the uart
*/
#if 0
#define NR_PORTS 			4
#define UART_FIFO_SIZE		64
#define UART_HAS_INTMSK
#define UART_C_CFLAG
#define UART_UMCON
#define UART_CLK			115200
#endif

#define UART_DR			(0x00)	// Data Register
#define UART_RSR		(0x04)	// Receive Status Register
#define UART_ECR		(0x04)	// Error Clear Register
#define UART_FR			(0x18)	// Flag Register
#define UART_ILPR		(0x20)	// IrDA Low-Power Counter Register
#define UART_IBRD		(0x24)	// Integer Baud Rate Register
#define UART_FBRD		(0x28)	// Fractional Baud Rate Register
#define UART_LCRH		(0x2c)	// Line Control Register
#define UART_CR			(0x30)	// Control Register
#define UART_IFLS		(0x34)	// Interrupt FIFO Level Select Register
#define UART_IMSC		(0x38)	// Interrupt Mask Set/Clear Register
#define UART_RIS		(0x3c)	// Raw Interrupt Status Register
#define UART_MIS		(0x40)	// Masked Interrupt Status Register
#define UART_ICR		(0x44)	// Interrupt Clear Register
#define UART_DMACR		(0x48)	// DMA Control Register

#endif /* __ASSEMBLY__ */

#endif
