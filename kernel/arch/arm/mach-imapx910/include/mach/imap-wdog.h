#ifndef __IMAPX_WATCHDOG__
#define __IMAPX_WATCHDOG__

#define WDT_CR   			 (0x00 )			// receive buffer register
#define WDT_TORR    			 (0x04 )			// interrupt enable register                                                                                   									// divisor latch high         (0x04)
#define WDT_CCVR    			 (0x08 )			// interrupt identity register
#define WDT_CRR			 (0x0C )			// line control register      (0x0c)
#define WDT_STAT    			 (0x10 )     		// modem control register     (0x10)
#define WDT_EOI    			 (0x14 )   			// line status register       (0x14)
#define WDT_COMP_PARAMS_5  		 (0xE4 )     		// modem status register      (0x18)
#define WDT_COMP_PARAMS_4  		 (0xE8 )    		 // scratch register           (0x1c)
#define WDT_COMP_PARAMS_3  		 (0xEC )
#define WDT_COMP_PARAMS_2  		 (0xF0 )                    	
#define WDT_COMP_PARAMS_1 		 (0xF4 )			// FIFO access register       (0x70)
#define WDT_COMP_VERSION  	 	 (0xF8 )        		// transmit FIFO read         (0x74)
#define WDT_COMP_TYPE    		 (0xFC )        		// receiver FIFO write        (0x78)


#define WDT_CR				(0x00)
#define WDT_TORR			(0x04)
#define WDT_CCVR			(0x08)
#define WDT_CRR				(0x0C)

#define SW_RST				(0x100)
#define INFO0				(0x22C)
#define INFO1                           (0x230)
#define INFO2                           (0x234)
#define INFO3                           (0x238)


#endif

