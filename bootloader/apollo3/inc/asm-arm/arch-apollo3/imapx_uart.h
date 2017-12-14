
#ifndef __IMAPX800_UART_H__
#define __IMAPX800_UART_H__

#define UART_LOG_BASE   UART3_BASE_ADDR

#define UART_DR			(UART_LOG_BASE + 0x00)	// Data Register
#define UART_RSR		(UART_LOG_BASE + 0x04)	// Receive Status Register
#define UART_ECR		(UART_LOG_BASE + 0x04)	// Error Clear Register
#define UART_FR			(UART_LOG_BASE + 0x18)	// Flag Register
#define UART_ILPR		(UART_LOG_BASE + 0x20)	// IrDA Low-Power Counter Register
#define UART_IBRD		(UART_LOG_BASE + 0x24)	// Integer Baud Rate Register
#define UART_FBRD		(UART_LOG_BASE + 0x28)	// Fractional Baud Rate Register
#define UART_LCRH		(UART_LOG_BASE + 0x2c)	// Line Control Register
#define UART_CR			(UART_LOG_BASE + 0x30)	// Control Register
#define UART_IFLS		(UART_LOG_BASE + 0x34)	// Interrupt FIFO Level Select Register
#define UART_IMSC		(UART_LOG_BASE + 0x38)	// Interrupt Mask Set/Clear Register
#define UART_RIS		(UART_LOG_BASE + 0x3c)	// Raw Interrupt Status Register
#define UART_MIS		(UART_LOG_BASE + 0x40)	// Masked Interrupt Status Register
#define UART_ICR		(UART_LOG_BASE + 0x44)	// Interrupt Clear Register
#define UART_DMACR		(UART_LOG_BASE + 0x48)	// DMA Control Register


#define UART_FR_RXFE		0x010
#define UART_FR_TXFF		0x020

#endif /*__IMAPX800_UART_H__*/ 
