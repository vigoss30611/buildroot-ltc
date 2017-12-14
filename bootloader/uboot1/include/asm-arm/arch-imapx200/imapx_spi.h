#ifndef __IMAPX_SPI__
#define __IMAPX_SPI__


#define rSPCON0     (0x00 )	//SPI0 control
#define rSPSTA0     (0x04 )	//SPI0 status
#define rSPPIN0     (0x08 )	//SPI0 pin control
#define rSPPRE0     (0x0c )	//SPI0 baud rate prescaler
#define rSPTDAT0    (0x10 )	//SPI0 Tx data
#define rSPRDAT0    (0x14 )	//SPI0 Rx data

#define rSSI_CTLR0_M						 (0x0 )
#define rSSI_CTLR1_M						 (0x4 )
#define rSSI_ENR_M						 (0x8 )
#define rSSI_MWCR_M						 (0xC )
#define rSSI_SER_M						 (0x10 )
#define rSSI_BAUDR_M						 (0x14 )
#define rSSI_TXFTLR_M						 (0x18 )
#define rSSI_RXFTLR_M						 (0x1C )
#define rSSI_TXFLR_M						 (0x20 )
#define rSSI_RXFLR_M						 (0x24 )
#define rSSI_SR_M						 (0x28 )
#define rSSI_IMR_M						 (0x2C )
#define rSSI_ISR_M						 (0x30 )
#define rSSI_RISR_M						 (0x34 )
#define rSSI_TXOICR_M						 (0x38 )
#define rSSI_RXOICR_M						 (0x3C )
#define rSSI_RXUICR_M						 (0x40 )
#define rSSI_MSTICR_M						 (0x44 )
#define rSSI_ICR_M						 (0x48 )
#define rSSI_DMACR_M						 (0x4C )
#define rSSI_DMATDLR_M					     	 (0x50 )
#define rSSI_DMARDLR_M					     	 (0x54 )
#define rSSI_IDR_M						 (0x58 )
#define rSSI_VERSION_M					     	 (0x5C )
#define rSSI_DR_M						 (0x60 )



#define rSSI_CTLR0_M_1						 (0x0 )
#define rSSI_CTLR1_M_1						 (0x4 )
#define rSSI_ENR_M_1						 (0x8 )
#define rSSI_MWCR_M_1						 (0xC )
#define rSSI_SER_M_1						 (0x10 )
#define rSSI_BAUDR_M_1						 (0x14 )
#define rSSI_TXFTLR_M_1						 (0x18 )
#define rSSI_RXFTLR_M_1						 (0x1C )
#define rSSI_TXFLR_M_1						 (0x20 )
#define rSSI_RXFLR_M_1						 (0x24 )
#define rSSI_SR_M_1						 (0x28 )
#define rSSI_IMR_M_1						 (0x2C )
#define rSSI_ISR_M_1						 (0x30 )
#define rSSI_RISR_M_1						 (0x34 )
#define rSSI_TXOICR_M_1						 (0x38 )
#define rSSI_RXOICR_M_1						 (0x3C )
#define rSSI_RXUICR_M_1						 (0x40 )
#define rSSI_MSTICR_M_1						 (0x44 )
#define rSSI_ICR_M_1						 (0x48 )
#define rSSI_DMACR_M_1						 (0x4C )
#define rSSI_DMATDLR_M_1					 (0x50 )
#define rSSI_DMARDLR_M_1					 (0x54 )
#define rSSI_IDR_M_1						 (0x58 )
#define rSSI_VERSION_M_1					 (0x5C )
#define rSSI_DR_M_1						 (0x60 )



#define rSSI_CTLR0_M_2						 (0x0 )
#define rSSI_CTLR1_M_2						 (0x4 )
#define rSSI_ENR_M_2						 (0x8 )
#define rSSI_MWCR_M_2						 (0xC )
#define rSSI_SER_M_2						 (0x10 )
#define rSSI_BAUDR_M_2						 (0x14 )
#define rSSI_TXFTLR_M_2						 (0x18 )
#define rSSI_RXFTLR_M_2						 (0x1C )
#define rSSI_TXFLR_M_2						 (0x20 )
#define rSSI_RXFLR_M_2						 (0x24 )
#define rSSI_SR_M_2						 (0x28 )
#define rSSI_IMR_M_2						 (0x2C )
#define rSSI_ISR_M_2						 (0x30 )
#define rSSI_RISR_M_2						 (0x34 )
#define rSSI_TXOICR_M_2						 (0x38 )
#define rSSI_RXOICR_M_2						 (0x3C )
#define rSSI_RXUICR_M_2						 (0x40 )
#define rSSI_MSTICR_M_2						 (0x44 )
#define rSSI_ICR_M_2						 (0x48 )
#define rSSI_DMACR_M_2						 (0x4C )
#define rSSI_DMATDLR_M_2					 (0x50 )
#define rSSI_DMARDLR_M_2					 (0x54 )
#define rSSI_IDR_M_2						 (0x58 )
#define rSSI_VERSION_M_2					 (0x5C )
#define rSSI_DR_M_2						 (0x60 )



//========================================================================
// SSI SLAVE 
//========================================================================
#define rSSI_CTLR0_S						 (0x0 )
#define rSSI_CTLR1_S						 (0x4 )
#define rSSI_ENR_S						 (0x8 )
#define rSSI_MWCR_S						 (0xC )
#define rSSI_SER_S						 (0x10 )
#define rSSI_BAUDR_S						 (0x14 )
#define rSSI_TXFTLR_S						 (0x18 )
#define rSSI_RXFTLR_S						 (0x1C )
#define rSSI_TXFLR_S						 (0x20 )
#define rSSI_RXFLR_S						 (0x24 )
#define rSSI_SR_S						 (0x28 )
#define rSSI_IMR_S						 (0x2C )
#define rSSI_ISR_S						 (0x30 )
#define rSSI_RISR_S						 (0x34 )
#define rSSI_TXOICR_S						 (0x38 )
#define rSSI_RXOICR_S						 (0x3C )
#define rSSI_RXUICR_S						 (0x40 )
#define rSSI_MSTICR_S						 (0x44 )
#define rSSI_ICR_S						 (0x48 )
#define rSSI_DMACR_S						 (0x4C )
#define rSSI_DMATDLR_S					 	 (0x50 )
#define rSSI_DMARDLR_S					 	 (0x54 )
#define rSSI_IDR_S						 (0x58 )
#define rSSI_VERSION_S					 	 (0x5C )
#define rSSI_DR_S						 (0x60 )

//
#define SSI_CFS  	(0x0<<12)
#define SSI_SRL  	(0x0<<11)  //0 is normal mode , 1 is test mode
#define SSI_OE   	(0x1<<10)  //0 is enable slave txd , 1 is disable slave txd
#define SSI_TMODE_TXRD  	(0x0<<8)   //0 is Transmit & Receive
#define SSI_TMODE_TX  		(0x1<<8)					   		   //1 is Transmit only
#define SSI_TMODE_RD  		(0x2<<8)					   		   //2 is Reveive only
#define SSI_TMODE_EEPROM  	(0x3<<8)					   		   //3 is EEPROM read
#define SSI_SCPOL_LOW   	(0x0<<7)
#define SSI_SCPOL_HIGH   	(0x1<<7)
#define SSI_SCPH_A          (0x0<<6)//data are captured on the first edge of the serial clock
#define SSI_SCPH_B          (0x1<<6)//data are captured on the second edge of the serial clock
#define SSI_SPI             (0x0<<4)//format spi
#define SSI_SSP             (0x1<<4)//format ssp
#define SSI_NSM             (0x2<<4)//format National Semiconductors Microwire
#define SSI_DFS             (0x7<<0)//0x3 - 4bit
                                    //0x4 - 5bit 
                                    //0x5 - 6bit
                                    //......
                                    //0xf - 16bit
#define SSI_NDF             (0x7)   //when TMOD = 2 or 3 ,it config the number of data frames to be received
#define SSI_EN              (0x1)
#define SSI_MHS             (0x0<<2)  //0 is handshaking interface disable
									  //1 is handshaking interface enable
#define SSI_MDD				(0x0<<1)  //0 is receive
                                      //1 is send
#define SSI_MWMODE          (0x0<<0)  //0 is non-sequential
                                      //1 is sequential
#define SSI_SER             (0x1<<0)  //ss_1_n is select
#define SSI_BAUDR           (0x2<<0)  //for div the ssi fre ,must be a even value 
#define SSI_TXFTLR          (0x10)    //control the level of entries(or below)at which the transmit FIFO controller trigger an interrupt
#define SSI_RXFTLR          (0x10)    //control the level of entries(or above)at which the transmit FIFO controller trigger an interrupt

#define SSI_DCOL            (0x1<<6)  //Data Collision Error
#define SSI_TXE             (0x1<<5)  //transmit fifo is empty when transfer is start,only set when it config as slave
#define SSI_RFE             (0x1<<4)  //Receive fifo is full
#define SSI_RFNE            (0x1<<3)  //Receive fifo is not empty
#define SSI_TFE             (0x1<<2)  //Transmit fifo is empty
#define SSI_TFNF            (0x1<<1)  //Transmit fifo is not full
#define SSI_BUSY            (0x1<<0)  //ssi is busy

#define SSI_MSTIM           (0x1<<5)  //multi master interrupt mask
#define SSI_RXFIM           (0x1<<4)  //receive fifo full interrupt mask
#define SSI_RXOIM           (0x1<<3)  //receive fifo overflow interrupt mask
#define SSI_RXUIM           (0x1<<2)  //receive fifo underflow interrupt mask
#define SSI_TXOIM           (0x1<<1)  //transmit fifo overfolw interrupt mask
#define SSI_TXEIM           (0x1<<0)  //transmit fifo empty interrupt mask

#define SSI_MSTIS           (0x1<<5)  //multi master interrupt 
#define SSI_RXFIS           (0x1<<4)  //receive fifo full interrupt 
#define SSI_RXOIS           (0x1<<3)  //receive fifo overflow interrupt 
#define SSI_RXUIS           (0x1<<2)  //receive fifo underflow interrupt 
#define SSI_TXOIS           (0x1<<1)  //transmit fifo overfolw interrupt 
#define SSI_TXEIS           (0x1<<0)  //transmit fifo empty interrupt 

#define SSI_TDMAEN          (0x1<<1)  //transmit dma enable
#define SSI_RDMAEN          (0x1<<0)  //receive dma enable
#define SSI_DMATDLR         (0x10) 	  //This bit field controls the level at which a DMA request is made by the transmit logic
#define SSI_DMARDLR         (0x10)    //This bit field controls the level at which a DMA request is made by the receive logic


#define SSI_NORMAL          (0x1)
#define SSI_INTERRUPT       (0x2)
#define SSI_DMA          	(0x3)

#define SSI_MASTER          (0x1)
#define SSI_SLAVE           (0x2)


#endif
