#ifndef __IMAP_EMIF_H__
#define __IMAP_EMIF_H__

#define IMAP_EMIF_ADDR_OFF			(IO_ADDRESS(IMAP_EMIF_BASE))
#define IMAP_SYSMGR_EMIF_ADDR_OFF  	(IO_ADDRESS(SYSMGR_EMIF_BASE))


/* uMCTL Registers */
#define UMCTL_PCFG(x)               (0x400 + 4 * x)     /* Port x */     
                                                                                               
#define UMCTL_CCFG                  (0x480)             /* Controller Configuration Register */
#define UMCTL_DCFG                  (0x484)             /* DRAM Configuration Register */       
#define UMCTL_CSTAT                 (0x488)             /* Controller Status Register */        
#define UMCTL_CCFG1                 (0x48c)             /* Controller Status Register 1     add */

#define UMCTL_SCFG                  (0x000)
#define UMCTL_SCTL                  (0x004)
#define UMCTL_STAT                  (0x008)
#define UMCTL_MCMD                  (0x040)
#define UMCTL_TREFI                 (0x0d0)
#define UMCTL_DFITREFMSKI           (0x294)
#define UMCTL_DFITCTRLUPDI          (0x298)
#define UMCTL_DTUCFG                (0x208)
#define UMCTL_TOGCNT1U              (0x0c0)
#define UMCTL_TOGCNT100N            (0x0cc)

#define PUBL_OFF                    (0x8000)
#define PHY_ZQ0CR0                  (PUBL_OFF + 0x180)
#define PHY_ZQ0CR1                  (PUBL_OFF + 0x184)
#define PHY_ZQ0SR0                  (PUBL_OFF + 0x188)
#define PHY_ZQ0SR1                  (PUBL_OFF + 0x18c)
#define PHY_ZQ1CR0                  (PUBL_OFF + 0x190)
#define PHY_ZQ1CR1                  (PUBL_OFF + 0x194)
#define PHY_ZQ1SR0                  (PUBL_OFF + 0x198)
#define PHY_ZQ1SR1                  (PUBL_OFF + 0x19c)
#define PHY_PIR                     (PUBL_OFF + 0x004)
#define PHY_PGSR                    (PUBL_OFF + 0x00c)
#define PHY_DTAR                    (PUBL_OFF + 0x054)

#define PHY_ACIOCR                  (PUBL_OFF + 0x024) /* AC I/O Configuration Register */
#define PHY_DXCCR                   (PUBL_OFF + 0x028) /* DATX8 Common Configuratio Register */
#define PHY_DSGCR                   (PUBL_OFF + 0x02c) /* DDR System General Configuration Register */

#define EMIF_RESET_CTRL				(0x0)

#define UMCTL_STAT_MASK             (0x7)


// for dram
/* SCTL   ; stat change */
#define SCTL_INT             		0          /* move to Init_mem from Config */
#define SCTL_CFG             		1          /* move to Config from Init_mem or Access */
#define SCTL_GO              		2          /* move to Access from Config */
#define SCTL_SLEEP           		3          /* move to Low_power from Access */
#define SCTL_WAKEUP          		4          /* move to Access from Low_power */

/* STAT */
#define STAT_INIT_MEM 				0
#define STAT_CONFIG     			1
#define STAT_CONFIG_REQ           	2
#define STAT_ACCESS              	3
#define STAT_ACCESS_REQ           	4
#define STAT_LOW_POWER            	5
#define STAT_LOW_POWER_ENTRY_REQ   	6
#define STAT_LOW_POWER_EXIT_REQ   	7


#endif /* imap-emif.h */
