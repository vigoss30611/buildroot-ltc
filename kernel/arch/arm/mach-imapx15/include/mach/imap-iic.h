#ifndef __IMAP_IIC__
#define __IMAP_IIC__

#define IIC_CON              		 (0x00 )     //IIC Control register
#define IIC_TAR              		 (0x04 )     //IIC Targer address register
#define IIC_SAR              		 (0x08 )     //IIC Slave address register
#define IIC_HS_MADDR         		 (0x0C )     //IIC High speed master mode code address register
#define IIC_DATA_CMD         		 (0x10 )     //IIC Rx/Tx data buffer and command register
#define IIC_SS_SCL_HCNT      		 (0x14 )     //Standard speed IIC clock SCL high count register
#define IIC_SS_SCL_LCNT      		 (0x18 )     //Standard speed IIC clock SCL low count register
#define IIC_FS_SCL_HCNT      		 (0x1C )     //Fast speed IIC clock SCL high count register
#define IIC_FS_SCL_LCNT      		 (0x20 )     //Fast speed IIC clock SCL low count register
#define IIC_HS_SCL_HCNT      		 (0x24 )     //High speed IIC clock SCL high count register
#define IIC_HS_SCL_LCNT      		 (0x28 )     //High speed IIC clock SCL low count register
#define IIC_INTR_STAT        		 (0x2C )     //IIC Interrupt Status register
#define IIC_INTR_MASK        		 (0x30 )     //IIC Interrupt Mask register
#define IIC_RAW_INTR_STAT    		 (0x34 )     //IIC raw interrupt status register
#define IIC_RX_TL            		 (0x38 )     //IIC receive FIFO Threshold register
#define IIC_TX_TL            		 (0x3C )     //IIC transmit FIFO Threshold register
#define IIC_CLR_INTR         		 (0x40 )     //clear combined and individual interrupt register
#define IIC_CLR_RX_UNDER     		 (0x44 )     //clear RX_UNDER interrupt register
#define IIC_CLR_RX_OVER      		 (0x48 )     //clear RX_OVER interrupt register
#define IIC_CLR_TX_OVER      		 (0x4C )     //clear TX_OVER interrupt register
#define IIC_CLR_RD_REQ       		 (0x50 )     //clear RD_REQ interrupt register
#define IIC_CLR_TX_ABRT      		 (0x54 )     //clear TX_ABRT interrupt register
#define IIC_CLR_RX_DONE      		 (0x58 )     //clear RX_DONE interrupt register
#define IIC_CLR_ACTIVITY     		 (0x5C )     //clear ACTIVITY interrupt register
#define IIC_CLR_STOP_DET     		 (0x60 )     //clear STOP_DET interrupt register
#define IIC_CLR_START_DET    		 (0x64 )     //clear START_DET interrupt register
#define IIC_CLR_GEN_CALL     		 (0x68 )     //clear GEN_CALL interrupt register
#define IIC_ENABLE           		 (0x6C )     //IIC enable register
#define IIC_STATUS           		 (0x70 )     //IIC status register
#define IIC_TXFLR            		 (0x74 )     //IIC transmit FIFO level register
#define IIC_RXFLR            		 (0x78 )     //IIC receive FIFO level register
#define IIC_TX_ABRT_SOURCE   		 (0x80 )     //IIC transmit abort source register

#define IIC_SLV_DATA_NACK_ONLY	 	 (0x84 )     //generate slave data NACK register
#define IIC_DMA_CR           		 (0x88 )     //DMA control register
#define IIC_DMA_TDLR         		 (0x8C )     //DAM transmit data level register
#define IIC_DMA_RDLR         		 (0x90 )     //IIC receive data level register
#define IIC_SDA_SETUP        		 (0x94 )     //IIC SDA setup register
#define IIC_ACK_GENERAL_CALL 		 (0x98 )     //IIC ACK general call register
#define IIC_ENABLE_STATUS    		 (0x9C )     //IIC enable status register
#define IIC_IGNORE_ACK0    		 (0xA0 )     //IIC enable status register

#define IIC_COMP_PARAM_1     		 (0xF4 )     //component parameter register 1
#define IIC_COMP_VERSION     		 (0xF8 )     //IIC component version register
#define IIC_COMP_TYPE        		 (0xFC )     //IIC component type register

//Bit Control

#define IMAP_IIC_CON_MASTER             (1 << 0)
#define IMAP_IIC_CON_MASTER_DISALBE     (0 << 0)
#define IMAP_IIC_CON_SPEED_SS           (1 << 1)
#define IMAP_IIC_CON_SPEED_FS           (2 << 1)
#define IMAP_IIC_CON_SPEED_HS           (3 << 1)
#define IMAP_IIC_CON_10BITADDR_SLAVE    (1 << 3)
#define IMAP_IIC_CON_10BITADDR_MASTER   (1 << 4)
#define IMAP_IIC_CON_RESTART_EN         (1 << 5)
#define IMAP_IIC_CON_SLAVE_DISABLE      (1 << 6)
#define IMAP_IIC_CON_SLAVE_ENABLE	(0 << 6)

#define IMAP_IIC_INTR_RX_UNDER          (1 << 0)
#define IMAP_IIC_INTR_RX_OVER           (1 << 1)
#define IMAP_IIC_INTR_RX_FULL           (1 << 2)
#define IMAP_IIC_INTR_TX_OVER           (1 << 3)
#define IMAP_IIC_INTR_TX_EMPTY          (1 << 4)
#define IMAP_IIC_INTR_RD_REQ            (1 << 5)
#define IMAP_IIC_INTR_TX_ABRT           (1 << 6)
#define IMAP_IIC_INTR_RX_DONE           (1 << 7)
#define IMAP_IIC_INTR_ACTIVITY          (1 << 8)
#define IMAP_IIC_INTR_STOP_DET          (1 << 9)
#define IMAP_IIC_INTR_START_DET         (1 << 10)
#define IMAP_IIC_INTR_GEN_CALL          (1 << 11)

#define IMAP_IIC_INTR_DEFAULT_MASK      (IMAP_IIC_INTR_RX_FULL | \
                                         IMAP_IIC_INTR_TX_EMPTY | \
                                         IMAP_IIC_INTR_TX_ABRT | \
                                         IMAP_IIC_INTR_STOP_DET)

#define IMAP_IIC_SLAVE_INTR_DEFAULT_MASK     (IMAP_IIC_INTR_RX_FULL | \
						IMAP_IIC_INTR_TX_ABRT | \
						IMAP_IIC_INTR_STOP_DET)
				

#define IMAP_IIC_STATUS_ACTIVITY        (1 << 0)
#define IMAP_IIC_ERR_TX_ABRT            (0x1)
#define IMAP_IIC_TIMEOUT                (20)/*ms*/

/*
 * status
 */
#define STATUS_IDLE                     (0x0)
#define STATUS_WRITE_IN_PROGRESS        (0x1)
#define STATUS_READ_IN_PROGRESS         (0x2)

/*
 * tx abort source
 */
#define ABRT_7B_ADDR_NOACK              (0)
#define ABRT_10BADDR1_NOACK             (1)
#define ABRT_10BADDR2_NOACK             (2)
#define ABRT_TXDATA_NOACK               (3)
#define ABRT_GCALL_NOACK                (4)
#define ABRT_GCALL_READ                 (5)
#define ABRT_HS_ACKDET                  (6)
#define ABRT_SBYTE_ACKDET               (7)
#define ABRT_HS_NORSTRT                 (8)
#define ABRT_SBYTE_NORSTRT              (9)
#define ABRT_10B_RD_NORSTRT             (10)
#define ABRT_MASTER_DIS                 (11)
#define ARB_LOST                        (12)
#define ABRT_SLVFLUSH_TXFIFO            (13)
#define ABRT_SLV_ARBLOST                (14)
#define ABRT_SLVRD_INTX                 (15)

#define IMAP_IIC_TX_ABRT_7B_ADDR_NOACK  (1 << ABRT_7B_ADDR_NOACK)
#define IMAP_IIC_TX_ABRT_10BADDR1_NOACK (1 << ABRT_10BADDR1_NOACK)
#define IMAP_IIC_TX_ABRT_10BADDR2_NOACK (1 << ABRT_10BADDR2_NOACK)
#define IMAP_IIC_TX_ABRT_TXDATA_NOACK   (1 << ABRT_TXDATA_NOACK)
#define IMAP_IIC_TX_ABRT_GCALL_NOACK    (1 << ABRT_GCALL_NOACK)
#define IMAP_IIC_TX_ABRT_GCALL_READ     (1 << ABRT_GCALL_READ)
#define IMAP_IIC_TX_ABRT_HS_ACKDET      (1 << ABRT_HS_ACKDET)
#define IMAP_IIC_TX_ABRT_SBYTE_ACKDET   (1 << ABRT_SBYTE_ACKDET)
#define IMAP_IIC_TX_ABRT_HS_NORSTRT     (1 << ABRT_HS_NORSTRT)
#define IMAP_IIC_TX_ABRT_SBYTE_NORSTRT  (1 << ABRT_SBYTE_NORSTRT)
#define IMAP_IIC_TX_ABRT_10B_RD_NORSTRT (1 << ABRT_10B_RD_NORSTRT)
#define IMAP_IIC_TX_ABRT_MASTER_DIS     (1 << ABRT_MASTER_DIS)
#define IMAP_IIC_TX_ARB_LOST            (1 << ARB_LOST)
#define IMAP_IIC_TX_ABRT_SLVFLUSH_TXFIFO        (1 << ABRT_SLVFLUSH_TXFIFO)
#define IMAP_IIC_TX_ABRT_SLV_ARBLOST    (1 << ABRT_SLV_ARBLOST)
#define IMAP_IIC_TX_ABRT_SLVRD_INTX     (1 << ABRT_SLVRD_INTX)
#define IMAP_IIC_TX_ABRT_NOACK          (IMAP_IIC_TX_ABRT_7B_ADDR_NOACK | \
                                         IMAP_IIC_TX_ABRT_10BADDR1_NOACK | \
                                         IMAP_IIC_TX_ABRT_10BADDR2_NOACK | \
                                         IMAP_IIC_TX_ABRT_TXDATA_NOACK | \
                                         IMAP_IIC_TX_ABRT_GCALL_NOACK)
#define IIC_DEFAULT_SS_SCL_HCNT		(0x190)
#define IIC_DEFAULT_SS_SCL_LCNT		(0x1d6)

/*
#define IIC_ENABLE			(0x1)
#define IIC_DEFAULT_SS_SCL_HCNT		(0xf0)
#define IIC_DEFAULT_SS_SCL_LCNT		(0xf0)
#define IIC_DEFAULT_SS_SCL_HCNT_SLOW	(0x180)
#define IIC_DEFAULT_SS_SCL_LCNT_SLOW	(0x180)
#define IIC_DEFAULT_RX_TL		(0x0)
#define IIC_DEFAULT_TX_TL		(0x2)
#define IIC_DEFAULT_SDA 		(0x20)
#define STANDARD_SPEED			(0x1<<1)
#define MATER_MODE			(0x1)
#define SLAVE_DISABLE			(0x1<<6)
#define IGNOREACK			(0x1<<8)
#define RESTART_ENABLE			(0x1<<5)
#define MASK_ALL_INT			(0x0)
#define START_INT			(0x1<<10)
#define STOP_INT			(0x1<<9)
#define ACTIVITY_INT			(0x1<<8)
#define IIC_ENABLE			(0x1)
#define START_INT			(0x1<<10)
#define STOP_INT			(0x1<<9)
#define ACTIVITY_INT			(0x1<<8)
#define IIC_TX_FIFO_DEPTH		(0x10)
#define IIC_RX_FIFO_DEPTH		(0x10)
#define IIC_READ_CMD			(0x1<<8)
#define IIC_RX_FULL			(0x1<<2)
#define IIC_TX_ABORT			(0x1<<6)
#define IIC_TX_EMPTY			(0x1<<4)
#define IIC_LOST_ARBITRATION		(0x1<<12)
*/

#endif
