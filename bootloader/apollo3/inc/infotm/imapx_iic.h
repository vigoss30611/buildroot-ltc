#ifndef __IMAPX_IIC_H__
#define __IMAPX_IIC_H__

#include <asm-arm/types.h>
#include <linux/types.h>
#define U32 unsigned int
#define U16 unsigned short
#define S32 int
#define S16 short int
#define U8  unsigned char
#define	S8  char

#define	BYTE	unsigned char
#define	WORD 	short
#define	DWORD	int
#define	UINT	U32
#define	LPSTR	U8 *

#define TRUE 	1
#define FALSE 	0
#define true    1
#define false   0

#define OK		1
#define FAIL	0
#define BOOL  U32


#define IIC_REG_READ_BEGIN     	0x1
#define IIC_REG_READ_RUN        	0x2      
#define IIC_REG_WRITE_BEGIN   	0x3
#define IIC_REG_WRITE_RUN      	0x4  

#define STANDARD  (0x1<<1)
#define FASTMODE  (0x2<<1)
#define HIGHMODE  (0x3<<1)
#define ADDRING10Master (0x1<<4)
#define ADDRING10Slave  (0x1<<3)
#define MASTER_MODE  (0x1<<0)
#define SLAVE_DISABLE (0x1<<6)
#define IC_RESTART_EN (0x1<<5)

// MASK BIT
#define TX_ABORT  (0x1<<6)
#define TX_EMPTY  (0x1<<4)
#define TX_OVER    (0x1<<3)
#define RX_FULL    (0x1<<2)
#define RX_OVER    (0x1<<1)
#define RX_UNDER  (0x1<<0)


//iic mode
#define IIC_MASTER        0x1
#define IIC_SLAVE          0x0

//iic state
#define IIC_WRITE_BUSY 0x1
#define IIC_WRITE_IDLE  0x2
#define IIC_WRITE_FINISH 0x3
#define IIC_WRITE_RUN   0x4
#define IIC_WRITE_OVER 0x5
#define IIC_READ_BUSY   0x6
#define IIC_READ_IDLE    0x7
#define IIC_READ_FINISH 0x8
#define IIC_READ_CMDFIN  0x9

#define WRITE_ADDR_FIN  0xa
#define READ_DATA_FLS   0xb
#define WRITE_ADDR_FLS  0xc
#define READ_DATA_FIN   0xd

#define IIC_CUR_READ 	0x0
#define IIC_CUR_WRITE 	0x1

__u8 iic_init(__u8 iicidex, __u8 iicmode, __u8 devaddr, __u8 intren, __u8 isrestart, __u8 ignoreack);

__u8 iic_read(__u8 iicidex, __u8 subaddr, __u8 * data);

__u8 iic_write(__u8 iicidex, __u8 subaddr,__u8 data);

__u8 iicreg_init(__u8 iicidex, __u8 iicmode, __u8 devaddr, __u8 intren, __u8 isrestart, __u8 ignoreack);


__u8 iicreg_read(__u8 iicidex, __u8 *data, __u32 num);

__u8 iicreg_write(uint8_t iicindex, uint8_t *subAddr, uint32_t addr_len, uint8_t *data, uint32_t data_num, uint8_t isstop);

__u8 iicreg_readwrite(__u8 iicidex, __u8 *data, __u8 num);

__u8 iic_writeread(__u8 iicidex, __u8 subaddr, __u8 * data);

__u8 iic_read_func(__u8 iicidex, __u8 *subaddr, U32 addr_len, __u8 *data, U32 data_num);

__u8 iic_write_func(__u8 iicidex, __u8 *subaddr, U32 addr_len, __u8 *data, U32 data_num);


/*register definition*/
//========================================================================
// IIC0
//========================================================================
#define IC_CON              		(I2C0_BASE_ADDR+0x00)     //IIC Control register
#define IC_TAR              		(I2C0_BASE_ADDR+0x04)     //IIC Targer address register
#define IC_SAR              		(I2C0_BASE_ADDR+0x08)     //IIC Slave address register
#define IC_HS_MADDR         		(I2C0_BASE_ADDR+0x0C)     //IIC High speed master mode code address register
#define IC_DATA_CMD         		(I2C0_BASE_ADDR+0x10)     //IIC Rx/Tx data buffer and command register
#define IC_SS_SCL_HCNT      		(I2C0_BASE_ADDR+0x14)     //Standard speed IIC clock SCL high count register
#define IC_SS_SCL_LCNT      		(I2C0_BASE_ADDR+0x18)     //Standard speed IIC clock SCL low count register
#define IC_FS_SCL_HCNT      		(I2C0_BASE_ADDR+0x1C)     //Fast speed IIC clock SCL high count register
#define IC_FS_SCL_LCNT      		(I2C0_BASE_ADDR+0x20)     //Fast speed IIC clock SCL low count register
#define IC_HS_SCL_HCNT      		(I2C0_BASE_ADDR+0x24)     //High speed IIC clock SCL high count register
#define IC_HS_SCL_LCNT      		(I2C0_BASE_ADDR+0x28)     //High speed IIC clock SCL low count register
#define IC_INTR_STAT        		(I2C0_BASE_ADDR+0x2C)     //IIC Interrupt Status register
#define IC_INTR_MASK        		(I2C0_BASE_ADDR+0x30)     //IIC Interrupt Mask register
#define IC_RAW_INTR_STAT    		(I2C0_BASE_ADDR+0x34)     //IIC raw interrupt status register
#define IC_RX_TL            		(I2C0_BASE_ADDR+0x38)     //IIC receive FIFO Threshold register
#define IC_TX_TL            		(I2C0_BASE_ADDR+0x3C)     //IIC transmit FIFO Threshold register
#define IC_CLR_INTR         		(I2C0_BASE_ADDR+0x40)     //clear combined and individual interrupt register
#define IC_CLR_RX_UNDER     		(I2C0_BASE_ADDR+0x44)     //clear RX_UNDER interrupt register
#define IC_CLR_RX_OVER      		(I2C0_BASE_ADDR+0x48)     //clear RX_OVER interrupt register
#define IC_CLR_TX_OVER      		(I2C0_BASE_ADDR+0x4C)     //clear TX_OVER interrupt register
#define IC_CLR_RD_REQ       		(I2C0_BASE_ADDR+0x50)     //clear RD_REQ interrupt register
#define IC_CLR_TX_ABRT      		(I2C0_BASE_ADDR+0x54)     //clear TX_ABRT interrupt register
#define IC_CLR_RX_DONE      		(I2C0_BASE_ADDR+0x58)     //clear RX_DONE interrupt register
#define IC_CLR_ACTIVITY     		(I2C0_BASE_ADDR+0x5C)     //clear ACTIVITY interrupt register
#define IC_CLR_STOP_DET     		(I2C0_BASE_ADDR+0x60)     //clear STOP_DET interrupt register
#define IC_CLR_START_DET    		(I2C0_BASE_ADDR+0x64)     //clear START_DET interrupt register
#define IC_CLR_GEN_CALL     		(I2C0_BASE_ADDR+0x68)     //clear GEN_CALL interrupt register
#define IC_ENABLE           		(I2C0_BASE_ADDR+0x6C)     //IIC enable register
#define IC_STATUS           		(I2C0_BASE_ADDR+0x70)     //IIC status register
#define IC_TXFLR            		(I2C0_BASE_ADDR+0x74)     //IIC transmit FIFO level register
#define IC_RXFLR            		(I2C0_BASE_ADDR+0x78)     //IIC receive FIFO level register
#define IC_TX_ABRT_SOURCE   		(I2C0_BASE_ADDR+0x80)     //IIC transmit abort source register
#define IC_SLV_DATA_NACK_ONLY		(I2C0_BASE_ADDR+0x84)     //generate slave data NACK register
#define IC_DMA_CR           		(I2C0_BASE_ADDR+0x88)     //DMA control register
#define IC_DMA_TDLR         		(I2C0_BASE_ADDR+0x8C)     //DAM transmit data level register
#define IC_DMA_RDLR         		(I2C0_BASE_ADDR+0x90)     //IIC receive data level register
#define IC_SDA_SETUP        		(I2C0_BASE_ADDR+0x94)     //IIC SDA setup register
#define IC_ACK_GENERAL_CALL 		(I2C0_BASE_ADDR+0x98)     //IIC ACK general call register
#define IC_ENABLE_STATUS    		(I2C0_BASE_ADDR+0x9C)     //IIC enable status register
#define IC_IGNORE_ACK0    		(I2C0_BASE_ADDR+0xA0)     //IIC enable status register
#define IC_COMP_PARAM_1     		(I2C0_BASE_ADDR+0xF4)     //component parameter register 1
#define IC_COMP_VERSION     		(I2C0_BASE_ADDR+0xF8)     //IIC component version register
#define IC_COMP_TYPE        		(I2C0_BASE_ADDR+0xFC)     //IIC component type register


//========================================================================
// IIC1
//========================================================================
#define IC_CON1              		(I2C1_BASE_ADDR+0x00)     //IIC Control register
#define IC_TAR1              		(I2C1_BASE_ADDR+0x04)     //IIC Targer address register
#define IC_SAR1              		(I2C1_BASE_ADDR+0x08)     //IIC Slave address register
#define IC_HS_MADDR1         		(I2C1_BASE_ADDR+0x0C)     //IIC High speed master mode code address register
#define IC_DATA_CMD1         		(I2C1_BASE_ADDR+0x10)     //IIC Rx/Tx data buffer and command register
#define IC_SS_SCL_HCNT1      		(I2C1_BASE_ADDR+0x14)     //Standard speed IIC clock SCL high count register
#define IC_SS_SCL_LCNT1      		(I2C1_BASE_ADDR+0x18)     //Standard speed IIC clock SCL low count register
#define IC_FS_SCL_HCNT1      		(I2C1_BASE_ADDR+0x1C)     //Fast speed IIC clock SCL high count register
#define IC_FS_SCL_LCNT1      		(I2C1_BASE_ADDR+0x20)     //Fast speed IIC clock SCL low count register
#define IC_HS_SCL_HCNT1      		(I2C1_BASE_ADDR+0x24)     //High speed IIC clock SCL high count register
#define IC_HS_SCL_LCNT1      		(I2C1_BASE_ADDR+0x28)     //High speed IIC clock SCL low count register
#define IC_INTR_STAT1        		(I2C1_BASE_ADDR+0x2C)     //IIC Interrupt Status register
#define IC_INTR_MASK1        		(I2C1_BASE_ADDR+0x30)     //IIC Interrupt Mask register
#define IC_RAW_INTR_STAT1    		(I2C1_BASE_ADDR+0x34)     //IIC raw interrupt status register
#define IC_RX_TL1            		(I2C1_BASE_ADDR+0x38)     //IIC receive FIFO Threshold register
#define IC_TX_TL1            		(I2C1_BASE_ADDR+0x3C)     //IIC transmit FIFO Threshold register
#define IC_CLR_INTR1         		(I2C1_BASE_ADDR+0x40)     //clear combined and individual interrupt register
#define IC_CLR_RX_UNDER1     		(I2C1_BASE_ADDR+0x44)     //clear RX_UNDER interrupt register
#define IC_CLR_RX_OVER1      		(I2C1_BASE_ADDR+0x48)     //clear RX_OVER interrupt register
#define IC_CLR_TX_OVER1      		(I2C1_BASE_ADDR+0x4C)     //clear TX_OVER interrupt register
#define IC_CLR_RD_REQ1       		(I2C1_BASE_ADDR+0x50)     //clear RD_REQ interrupt register
#define IC_CLR_TX_ABRT1      		(I2C1_BASE_ADDR+0x54)     //clear TX_ABRT interrupt register
#define IC_CLR_RX_DONE1      		(I2C1_BASE_ADDR+0x58)     //clear RX_DONE interrupt register
#define IC_CLR_ACTIVITY1     		(I2C1_BASE_ADDR+0x5C)     //clear ACTIVITY interrupt register
#define IC_CLR_STOP_DET1     		(I2C1_BASE_ADDR+0x60)     //clear STOP_DET interrupt register
#define IC_CLR_START_DET1    		(I2C1_BASE_ADDR+0x64)     //clear START_DET interrupt register
#define IC_CLR_GEN_CALL1     		(I2C1_BASE_ADDR+0x68)     //clear GEN_CALL interrupt register
#define IC_ENABLE1           		(I2C1_BASE_ADDR+0x6C)     //IIC enable register
#define IC_STATUS1           		(I2C1_BASE_ADDR+0x70)     //IIC status register
#define IC_TXFLR1            		(I2C1_BASE_ADDR+0x74)     //IIC transmit FIFO level register
#define IC_RXFLR1            		(I2C1_BASE_ADDR+0x78)     //IIC receive FIFO level register
#define IC_TX_ABRT_SOURCE1   		(I2C1_BASE_ADDR+0x80)     //IIC transmit abort source register
#define IC_SLV_DATA_NACK_ONLY1	  	(I2C1_BASE_ADDR+0x84)     //generate slave data NACK register
#define IC_DMA_CR1           		(I2C1_BASE_ADDR+0x88)     //DMA control register
#define IC_DMA_TDLR1         		(I2C1_BASE_ADDR+0x8C)     //DAM transmit data level register
#define IC_DMA_RDLR1         		(I2C1_BASE_ADDR+0x90)     //IIC receive data level register
#define IC_SDA_SETUP1        		(I2C1_BASE_ADDR+0x94)     //IIC SDA setup register
#define IC_ACK_GENERAL_CALL1 		(I2C1_BASE_ADDR+0x98)     //IIC ACK general call register
#define IC_ENABLE_STATUS1    		(I2C1_BASE_ADDR+0x9C)     //IIC enable status register
#define IC_IGNORE_ACK1    		(I2C1_BASE_ADDR+0xA0)     //IIC enable status register
#define IC_COMP_PARAM_11     		(I2C1_BASE_ADDR+0xF4)     //component parameter register 1
#define IC_COMP_VERSION1     		(I2C1_BASE_ADDR+0xF8)     //IIC component version register
#define IC_COMP_TYPE1        		(I2C1_BASE_ADDR+0xFC)     //IIC component type register

extern uint8_t iic_init(uint8_t iicindex, uint8_t iicmode, uint8_t devaddr, uint8_t intren, uint8_t isrestart, uint8_t ignoreack);
extern uint8_t iicreg_write(uint8_t iicindex, uint8_t *subAddr, uint32_t addr_len, uint8_t *data, uint32_t data_num, uint8_t isstop);
extern uint8_t iicreg_read(uint8_t iicindex, uint8_t *data, uint32_t num);

#endif
