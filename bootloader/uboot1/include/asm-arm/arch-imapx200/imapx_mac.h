#ifndef __IMAPX_MAC_H__
#define __IMAPX_MAC_H__


/*
 * MAC registers description:
 @MACConfig:  This is the operation mode register for the MAC.
 @MACFrameFilter:  Contains the frame filtering controls.
 @HashTableH:  Contains the higher 32 bits of the Multicast Hash table.This register
				is present only when the Hash filter function is selected in coreConsultant.
 @HashTableL:  Contains the lower 32 bits of the Multicast Hash table.This register is
				present only when the Hash filter function is selected in coreConsultant.
 @GMllAddr:  Controls the management cycles to an external PHY. This register is present
				only when the Station Management (MDIO) feature is selected in coreConsultant.
 @GMllData:  Contains the data to be written to or read from the PHY register. This register
				is present only when the Station Management (MDIO) feature is selected in coreConsultant.
 @FlowControl:  Controls the generation of control frames.
 @VLANTag:  Identifies IEEE 802.1Q VLAN type frames.
 @IdeftifiesVersion:  Identifies the version of the Core
 @Reserved:  Reserved
 @RemoteWakeUp:  This is the address through which the remote Wake-up Frame Filter registers
 @PMTControl:  This register is present only when the PMT module is selected in coreConsultant.
 @InterruptStatus:  Contains the interrupt status.
 @InterruptMask:  Contains the masks for generating the interrupts.
 @MACAddr0H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr0L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr1H:  Contains the interrupt status.
 @MACAddr1L:  Contains the masks for generating the interrupts.
 @MACAddr2H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr2L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr3H:  Contains the interrupt status.
 @MACAddr3L:  Contains the masks for generating the interrupts.
 @MACAddr4H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr4L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr5H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr5L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr6H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr6L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr7H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr7L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr8H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr8L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr9H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr9L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr10H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr10L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr11H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr11L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr12H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr12L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr13H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr13L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr14H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr14L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr15H:  Contains the higher 16 bits of the first MAC address.
 @MACAddr15L:  Contains the lower 32 bits of the first MAC address.
 @ANControl:  Enables and/or restarts auto-negotiation. It also enables PCS loopback.
 @ANStatus:  Indicates the link and auto-negotiation status.
 @ANAdvertise:  This register is configured before auto-negotiation begins.
				It contains the advertised ability of the GMAC.
 @ANegoLink:  Contains the advertised ability of the link partner.
 @ANExpansion:  Indicates whether a new base page has been received from the link partner.
 @TBIExStatus:  Indicates all modes of operation of the GMAC.
 @SGRGStatus:  Indicates the status signals received from the PHY through the SGMII/RGMII interface.
 @TimeStampControl:  Controls the time stamp generation and update logic.
 @SubSecIncrement:  Contains the 8-bit value by which the Sub-Second register is incremented.
 @TimeStampHigh:  Contains the most significant (higher) 32 time bits.
 @TimeStampLow:  Contains the least significant (lower) 32 time bits.
 @TimeStampHighUp:  Contains the most significant (higher) 32 bits of the time
					to be written to, added to, or subtracted from the System Time value.
 @TimeStampLowUp:  Contains the least significant (lower) 32 bits of the time
					to be written to, added to, or subtracted from the System Time value.
 @TimeStampAddend:  This register is used by the software to readjust the clock
					frequency linearly to match the master clock frequency.
 @TargetTimeHigh:  This register is used by the software to readjust the clock
					frequency linearly to match the master clock frequency.
 @TargetTimeLow:  Contains the lower 32 bits of time to be compared with the
					system time for interrupt event generation.
 @MACAddr16H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr16L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr17H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr17L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr18H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr18L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr19H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr19L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr20H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr20L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr21H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr21L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr22H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr22L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr23H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr23L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr24H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr24L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr25H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr25L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr26H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr26L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr27H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr27L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr28H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr28L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr29H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr29L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr30H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr30L:  Contains the lower 32 bits of the first MAC address.
 @MACAddr31H:  Contains the lower 32 bits of the first MAC address.
 @MACAddr31L:  Contains the lower 32 bits of the first MAC address.
 @BUSMODE:     Controls the Host Interface Mode
 @TxPollDemand:     Used by the host to instruct the DMA to poll the Transmit Descriptor List
 @RxPollDemand:     Used by the host to instruct the DMA to poll the Receive Descriptor List
 @RxDescriptorListAddr:     Points the DMA to the start of the Receive Descriptor list
 @TxDescriptorListAddr:     Points the DMA to the start of the Transmit Descriptor list
 @Status:     The Software driver reads this register during interrupt service
				routine or polling to determint the status of the DMA
 @OperationMode:     Establishes the Receive and Transmit operating modes and command
 @InterruptEnable:     Enables the interrupt reported by the Status Register
 @MissOverflow:     Contains the counters for discarded frames because no host
				Receive Descriptor was available,and discarded frames because of Receive FIFO Overflow
 @CurrHostTxDescriptor:     Points to the start of current Transmit Descriptor read by the DMA.
 @CurrHostRxDescriptor: 	  Points to the start of current Receive Descriptor read by the DMA.
 @CurrHostTxBufferAddr:    Points to the current Transmit Buffer address read by the DMA.
 @CurrHostRxBufferAddr:    Points to the current Receive Buffer address read by the DMA.
 */

#define MACConfig     	  		 	 (MAC_BASE_REG_PA+0x00)
#define MACFrameFilter   		 	 (MAC_BASE_REG_PA+0x04)
#define HashTableH	    			 (MAC_BASE_REG_PA+0x08)
#define HashTableL	    			 (MAC_BASE_REG_PA+0x0c)
#define GMIIAddr		     		 (MAC_BASE_REG_PA+0x10)
#define GMIIData		     		 (MAC_BASE_REG_PA+0x14)
#define FlowControl     			 (MAC_BASE_REG_PA+0x18)
#define VLANTag			    		 (MAC_BASE_REG_PA+0x1c)
                                                   
#define IdeftifiesVersion  	 		 (MAC_BASE_REG_PA+0x20)
#define Reserved		   			 (MAC_BASE_REG_PA+0x24)
#define RemoteWakeUp	   			 (MAC_BASE_REG_PA+0x28)
#define PMTControl					 (MAC_BASE_REG_PA+0x2c)
#define InterruptStatus   		 	 (MAC_BASE_REG_PA+0x38)
#define InterruptMask     		 	 (MAC_BASE_REG_PA+0x3c)
#define MACAddr0H     				 (MAC_BASE_REG_PA+0x40)
#define MACAddr0L	    			 (MAC_BASE_REG_PA+0x44)
#define MACAddr1H    				 (MAC_BASE_REG_PA+0x48)
#define MACAddr1L     				 (MAC_BASE_REG_PA+0x4c)
#define MACAddr2H					 (MAC_BASE_REG_PA+0x50)
#define MACAddr2L 	    			 (MAC_BASE_REG_PA+0x54)
#define MACAddr3H    				 (MAC_BASE_REG_PA+0x58)
#define MACAddr3L					 (MAC_BASE_REG_PA+0x5c)
#define MACAddr4H     				 (MAC_BASE_REG_PA+0x60)
#define MACAddr4L 	    			 (MAC_BASE_REG_PA+0x64)
#define MACAddr5H     				 (MAC_BASE_REG_PA+0x68)
#define MACAddr5L 	    			 (MAC_BASE_REG_PA+0x6C)
#define MACAddr6H     				 (MAC_BASE_REG_PA+0x70)
#define MACAddr6L 	    			 (MAC_BASE_REG_PA+0x74)
#define MACAddr7H     				 (MAC_BASE_REG_PA+0x78)
#define MACAddr7L 	    			 (MAC_BASE_REG_PA+0x7C)
#define MACAddr8H     				 (MAC_BASE_REG_PA+0x80)
#define MACAddr8L 	    			 (MAC_BASE_REG_PA+0x84)
#define MACAddr9H     				 (MAC_BASE_REG_PA+0x88)
#define MACAddr9L 	    			 (MAC_BASE_REG_PA+0x8C)
                                                     
#define MACAddr10H					 (MAC_BASE_REG_PA+0x90)
#define MACAddr10L 	    		 	 (MAC_BASE_REG_PA+0x94)
#define MACAddr11H					 (MAC_BASE_REG_PA+0x98)
#define MACAddr11L 	    		 	 (MAC_BASE_REG_PA+0x9C)
#define MACAddr12H		   			 (MAC_BASE_REG_PA+0xA0)
#define MACAddr12L				 	 (MAC_BASE_REG_PA+0xA4)
#define MACAddr13H					 (MAC_BASE_REG_PA+0xA8)
#define MACAddr13L 	    		 	 (MAC_BASE_REG_PA+0xAC)
#define MACAddr14H					 (MAC_BASE_REG_PA+0xB0)
#define MACAddr14L 	    		 	 (MAC_BASE_REG_PA+0xB4)
#define MACAddr15H					 (MAC_BASE_REG_PA+0xB8)
#define MACAddr15L 	    		 	 (MAC_BASE_REG_PA+0xBC)
                                                
#define ANControl 	    			 (MAC_BASE_REG_PA+0xC0)
#define ANStatus		    		 (MAC_BASE_REG_PA+0xC4)
#define ANAdvertise					 (MAC_BASE_REG_PA+0xC8)
#define ANegoLink					 (MAC_BASE_REG_PA+0xCC)
#define ANExpansion     		 	 (MAC_BASE_REG_PA+0xD0)
#define TBIExStatus	    		 	 (MAC_BASE_REG_PA+0xD4)
#define SGRGStatus	    			 (MAC_BASE_REG_PA+0xD8)
                                               
#define TimeStampControl			 (MAC_BASE_REG_PA+0x700)
#define SubSecIncrement				 (MAC_BASE_REG_PA+0x704)
#define TimeStampHigh    		 	 (MAC_BASE_REG_PA+0x708)
#define TimeStampLow  				 (MAC_BASE_REG_PA+0x70C)
#define TimeStampHighUp 			 (MAC_BASE_REG_PA+0x710)
#define TimeStampLowUp	     	  	 (MAC_BASE_REG_PA+0x714)
#define TimeStampAddend				 (MAC_BASE_REG_PA+0x718)
#define TargetTimeHigh	     	  	 (MAC_BASE_REG_PA+0x71C)
#define TargetTimeLow	     		 (MAC_BASE_REG_PA+0x720)
                                             
#define MACAddr16H	    			 (MAC_BASE_REG_PA+0x800)
#define MACAddr16L					 (MAC_BASE_REG_PA+0x804)
#define MACAddr17H    				 (MAC_BASE_REG_PA+0x808)
#define MACAddr17L  				 (MAC_BASE_REG_PA+0x80C)
#define MACAddr18H					 (MAC_BASE_REG_PA+0x810)
#define MACAddr18L	    			 (MAC_BASE_REG_PA+0x814)
#define MACAddr19H	    			 (MAC_BASE_REG_PA+0x818)
#define MACAddr19L	    			 (MAC_BASE_REG_PA+0x81C)
#define MACAddr20H	    			 (MAC_BASE_REG_PA+0x820)
#define MACAddr20L	    			 (MAC_BASE_REG_PA+0x824)
                         		                 
#define MACAddr21H	    			 (MAC_BASE_REG_PA+0x828)
#define MACAddr21L					 (MAC_BASE_REG_PA+0x82C)
#define MACAddr22H    				 (MAC_BASE_REG_PA+0x830)
#define MACAddr22L  				 (MAC_BASE_REG_PA+0x834)
#define MACAddr23H					 (MAC_BASE_REG_PA+0x838)
#define MACAddr23L	    			 (MAC_BASE_REG_PA+0x83C)
#define MACAddr24H	    			 (MAC_BASE_REG_PA+0x840)
#define MACAddr24L	    			 (MAC_BASE_REG_PA+0x844)
#define MACAddr25H	    			 (MAC_BASE_REG_PA+0x848)
#define MACAddr25L	    			 (MAC_BASE_REG_PA+0x84C)
                         		                     
#define MACAddr26H	    			 (MAC_BASE_REG_PA+0x850)
#define MACAddr26L					 (MAC_BASE_REG_PA+0x854)
#define MACAddr27H    				 (MAC_BASE_REG_PA+0x858)
#define MACAddr27L  				 (MAC_BASE_REG_PA+0x85C)
#define MACAddr28H					 (MAC_BASE_REG_PA+0x860)
#define MACAddr28L	    			 (MAC_BASE_REG_PA+0x864)
#define MACAddr29H	    			 (MAC_BASE_REG_PA+0x868)
#define MACAddr29L	    			 (MAC_BASE_REG_PA+0x86C)
#define MACAddr30H	    			 (MAC_BASE_REG_PA+0x870)
#define MACAddr30L	    			 (MAC_BASE_REG_PA+0x874)
#define MACAddr31H	    			 (MAC_BASE_REG_PA+0x878)
#define MACAddr31L	    			 (MAC_BASE_REG_PA+0x87C)


/* Ethernet DMA Registers */
#define BUSMODE     	       		 (MAC_BASE_REG_PA+0x1000)
#define TxPollDemand      			 (MAC_BASE_REG_PA+0x1004)
#define RxPollDemand     			 (MAC_BASE_REG_PA+0x1008)
#define RxDescriptorListAddr   		 (MAC_BASE_REG_PA+0x100c)
#define TxDescriptorListAddr   	 	 (MAC_BASE_REG_PA+0x1010)
#define Status						 (MAC_BASE_REG_PA+0x1014)
#define OperationMode    			 (MAC_BASE_REG_PA+0x1018)
#define InterruptEnable   			 (MAC_BASE_REG_PA+0x101c)
#define MissOverflow     			 (MAC_BASE_REG_PA+0x1020)
#define CurrHostTxDescriptor   		 (MAC_BASE_REG_PA+0x1048)
#define CurrHostRxDescriptor   	 	 (MAC_BASE_REG_PA+0x104c)
#define CurrHostTxBufferAddr   	 	 (MAC_BASE_REG_PA+0x1050)
#define CurrHostRxBufferAddr   	 	 (MAC_BASE_REG_PA+0x1054)

#endif /*__IMAPX_MAC_H__*/
