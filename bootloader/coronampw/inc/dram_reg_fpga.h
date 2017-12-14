#ifndef __IMAPX_DRAMC_FPGA__
#define __IMAPX_DRAMC_FPGA__

/*========================================================================
 * DWC_DDR_UMCTL2_MAP Registers 
 * control for q3e version 2.4 / 2.5, phy publ not change   -----2015.4.7
 *======================================================================*/
#define DWC_DDR_PUBL_BASE_ADDR			 (DWC_DDR_UMCTL_BASE_ADDR + 0x8000)

#define ADDR_UPCTL_IPVR                  (DWC_DDR_UMCTL_BASE_ADDR+0x3f8)     /* IP Version Register */
#define ADDR_UPCTL_IPTR                  (DWC_DDR_UMCTL_BASE_ADDR+0x3fc)     /* IP Type Register */
/*X9 DWC_#define ADDR_UMCTL2_MAP Registers */
#define ADDR_UMCTL2_MSTR                   DWC_DDR_UMCTL_BASE_ADDR+0x000    //         Master Register *
#define ADDR_UMCTL2_STAT                   DWC_DDR_UMCTL_BASE_ADDR+0x004    //         Operating Mode Status Register, read only
#define ADDR_UMCTL2_MRCTRL0                DWC_DDR_UMCTL_BASE_ADDR+0x010    //         Mode Register Read/Write Control Register 0 *
#define ADDR_UMCTL2_MRCTRL1                DWC_DDR_UMCTL_BASE_ADDR+0x014    //         Mode Register Read/Write Control Register 1 *
#define ADDR_UMCTL2_MRSTAT                 DWC_DDR_UMCTL_BASE_ADDR+0x018    //         Mode Register Read/Write Status Register, read only.
#define ADDR_UMCTL2_MRCTRL2                DWC_DDR_UMCTL_BASE_ADDR+0x01c    //         Mode Register Read/Write Status Register, read only.
#define ADDR_UMCTL2_DERATEEN               DWC_DDR_UMCTL_BASE_ADDR+0x020    //         Temperature Derate Enable Register, LPDDR2
#define ADDR_UMCTL2_DERATEINT              DWC_DDR_UMCTL_BASE_ADDR+0x024    //         Temperature Derate Interval Register, LPDDR2
#define ADDR_UMCTL2_PWRCTL                 DWC_DDR_UMCTL_BASE_ADDR+0x030    //         Low Power Control Register, not use now
#define ADDR_UMCTL2_PWRTMG                 DWC_DDR_UMCTL_BASE_ADDR+0x034    //         Low Power Timing Register
#define ADDR_UMCTL2_HWLPCTL                DWC_DDR_UMCTL_BASE_ADDR+0x038    //         Hardware Low Power Control Register
#define ADDR_UMCTL2_RFSHCTL0               DWC_DDR_UMCTL_BASE_ADDR+0x050    //         Refresh Control Register 0, use default
#define ADDR_UMCTL2_RFSHCTL1               DWC_DDR_UMCTL_BASE_ADDR+0x054    //         Refresh Control Register 1, RANK>=2
#define ADDR_UMCTL2_RFSHCTL2               DWC_DDR_UMCTL_BASE_ADDR+0x058    //         Refresh Control Register 2, RANK=4
#define ADDR_UMCTL2_RFSHCTL3               DWC_DDR_UMCTL_BASE_ADDR+0x060    //         Refresh Control Register 3, use default
#define ADDR_UMCTL2_RFSHTMG                DWC_DDR_UMCTL_BASE_ADDR+0x064    //         Refresh Timing Register      *
#define ADDR_UMCTL2_CRCPARCTL0             DWC_DDR_UMCTL_BASE_ADDR+0x0c0    //         CRC Parity Control Register0
#define ADDR_UMCTL2_CRCPARCTL1             DWC_DDR_UMCTL_BASE_ADDR+0x0c4    //         Refresh Timing Register      *
#define ADDR_UMCTL2_INIT0                  DWC_DDR_UMCTL_BASE_ADDR+0x0D0    //         SDRAM Initialization Register 0  *
#define ADDR_UMCTL2_INIT1                  DWC_DDR_UMCTL_BASE_ADDR+0x0D4    //         SDRAM Initialization Register 1  *
#define ADDR_UMCTL2_INIT2                  DWC_DDR_UMCTL_BASE_ADDR+0x0D8    //         SDRAM Initialization Register 2, LPDDR2
#define ADDR_UMCTL2_INIT3                  DWC_DDR_UMCTL_BASE_ADDR+0x0DC    //         SDRAM Initialization Register 3
#define ADDR_UMCTL2_INIT4                  DWC_DDR_UMCTL_BASE_ADDR+0x0E0    //         SDRAM Initialization Register 4
#define ADDR_UMCTL2_INIT5                  DWC_DDR_UMCTL_BASE_ADDR+0x0E4    //         SDRAM Initialization Register 5, LPDDR2/DDR3 *
#define ADDR_UMCTL2_INIT6                  DWC_DDR_UMCTL_BASE_ADDR+0x0E8    //         SDRAM Initialization Register 6, DDR4 only*
#define ADDR_UMCTL2_INIT7                  DWC_DDR_UMCTL_BASE_ADDR+0x0EC    //         SDRAM Initialization Register 7, DDR4 only*
#define ADDR_UMCTL2_DIMMCTL                DWC_DDR_UMCTL_BASE_ADDR+0x0F0    //         DIMM Control Register
#define ADDR_UMCTL2_RANKCTL                DWC_DDR_UMCTL_BASE_ADDR+0x0F4    //         Rank Control Register *
#define ADDR_UMCTL2_DRAMTMG0               DWC_DDR_UMCTL_BASE_ADDR+0x100    //         SDRAM Timing Register 0
#define ADDR_UMCTL2_DRAMTMG1               DWC_DDR_UMCTL_BASE_ADDR+0x104    //         SDRAM Timing Register 1
#define ADDR_UMCTL2_DRAMTMG2               DWC_DDR_UMCTL_BASE_ADDR+0x108    //         SDRAM Timing Register 2
#define ADDR_UMCTL2_DRAMTMG3               DWC_DDR_UMCTL_BASE_ADDR+0x10C    //         SDRAM Timing Register 3
#define ADDR_UMCTL2_DRAMTMG4               DWC_DDR_UMCTL_BASE_ADDR+0x110    //         SDRAM Timing Register 4
#define ADDR_UMCTL2_DRAMTMG5               DWC_DDR_UMCTL_BASE_ADDR+0x114    //         SDRAM Timing Register 5
#define ADDR_UMCTL2_DRAMTMG6               DWC_DDR_UMCTL_BASE_ADDR+0x118    //         SDRAM Timing Register 6
#define ADDR_UMCTL2_DRAMTMG7               DWC_DDR_UMCTL_BASE_ADDR+0x11C    //         SDRAM Timing Register 7
#define ADDR_UMCTL2_DRAMTMG8               DWC_DDR_UMCTL_BASE_ADDR+0x120    //         SDRAM Timing Register 8
#define ADDR_UMCTL2_DRAMTMG9               DWC_DDR_UMCTL_BASE_ADDR+0x124    //         SDRAM Timing Register 9 DDR4 only
#define ADDR_UMCTL2_DRAMTMG10              DWC_DDR_UMCTL_BASE_ADDR+0x128    //         SDRAM Timing Register 10 DDR4 only
#define ADDR_UMCTL2_DRAMTMG11              DWC_DDR_UMCTL_BASE_ADDR+0x12c    //         SDRAM Timing Register 11 DDR4 only
#define ADDR_UMCTL2_DRAMTMG12              DWC_DDR_UMCTL_BASE_ADDR+0x130    //         SDRAM Timing Register 12 DDR4 only
#define ADDR_UMCTL2_DRAMTMG14              DWC_DDR_UMCTL_BASE_ADDR+0x138    //         SDRAM Timing Register 12 DDR4 only

#define ADDR_UMCTL2_ZQCTL0                 DWC_DDR_UMCTL_BASE_ADDR+0x180    //         ZQ Control Register 0, LPDDR2/DDR3 *
#define ADDR_UMCTL2_ZQCTL1                 DWC_DDR_UMCTL_BASE_ADDR+0x184    //         ZQ Control Register 1, LPDDR2/DDR3
#define ADDR_UMCTL2_ZQCTL2                 DWC_DDR_UMCTL_BASE_ADDR+0x188    //         ZQ Control Register 2, LPDDR2
#define ADDR_UMCTL2_ZQSTAT                 DWC_DDR_UMCTL_BASE_ADDR+0x18C    //         ZQ Status Register, LPDDR2
#define ADDR_UMCTL2_DFITMG0                DWC_DDR_UMCTL_BASE_ADDR+0x190    //         DFI Timing Register 0  *
#define ADDR_UMCTL2_DFITMG1                DWC_DDR_UMCTL_BASE_ADDR+0x194    //         DFI Timing Register 1  *
#define ADDR_UMCTL2_DFILPCFG0		   DWC_DDR_UMCTL_BASE_ADDR+0x198    //         DFI Timing Register 1  *
#define ADDR_UMCTL2_DFILPCFG1		   DWC_DDR_UMCTL_BASE_ADDR+0x19c    //         DFI Timing Register 1  DDR4 only*
#define ADDR_UMCTL2_DFIUPD0                DWC_DDR_UMCTL_BASE_ADDR+0x1A0    //         DFI Update Register 0  *
#define ADDR_UMCTL2_DFIUPD1                DWC_DDR_UMCTL_BASE_ADDR+0x1A4    //         DFI Update Register 1
#define ADDR_UMCTL2_DFIUPD2                DWC_DDR_UMCTL_BASE_ADDR+0x1A8    //         DFI Update Register 2
#define ADDR_UMCTL2_DFIUPD3                DWC_DDR_UMCTL_BASE_ADDR+0x1AC    //         DFI Update Register 3
#define ADDR_UMCTL2_DFIMISC                DWC_DDR_UMCTL_BASE_ADDR+0x1B0    //         DFI Miscellaneous Control Register
#define ADDR_UMCTL2_TRAINCTL0              DWC_DDR_UMCTL_BASE_ADDR+0x1D0    //         PHY Eval Training Control Register 0
#define ADDR_UMCTL2_TRAINCTL1              DWC_DDR_UMCTL_BASE_ADDR+0x1D4    //         PHY Eval Training Control Register 1
#define ADDR_UMCTL2_TRAINCTL2              DWC_DDR_UMCTL_BASE_ADDR+0x1D8    //         PHY Eval Training Control Register 2
#define ADDR_UMCTL2_TRAINSTAT              DWC_DDR_UMCTL_BASE_ADDR+0x1DC    //         PHY Eval Training Status Register
#define ADDR_UMCTL2_ADDRMAP0               DWC_DDR_UMCTL_BASE_ADDR+0x200    //         Address Map Register 0 *
#define ADDR_UMCTL2_ADDRMAP1               DWC_DDR_UMCTL_BASE_ADDR+0x204    //         Address Map Register 1
#define ADDR_UMCTL2_ADDRMAP2               DWC_DDR_UMCTL_BASE_ADDR+0x208    //         Address Map Register 2
#define ADDR_UMCTL2_ADDRMAP3               DWC_DDR_UMCTL_BASE_ADDR+0x20C    //         Address Map Register 3
#define ADDR_UMCTL2_ADDRMAP4               DWC_DDR_UMCTL_BASE_ADDR+0x210    //         Address Map Register 4
#define ADDR_UMCTL2_ADDRMAP5               DWC_DDR_UMCTL_BASE_ADDR+0x214    //         Address Map Register 5
#define ADDR_UMCTL2_ADDRMAP6               DWC_DDR_UMCTL_BASE_ADDR+0x218    //         Address Map Register 6
#define ADDR_UMCTL2_ADDRMAP7               DWC_DDR_UMCTL_BASE_ADDR+0x21c    //         Address Map Register 6
#define ADDR_UMCTL2_ADDRMAP8               DWC_DDR_UMCTL_BASE_ADDR+0x220    //         Address Map Register 6
#define ADDR_UMCTL2_ADDRMAP9               DWC_DDR_UMCTL_BASE_ADDR+0x224    //         Address Map Register 6
#define ADDR_UMCTL2_ADDRMAP10              DWC_DDR_UMCTL_BASE_ADDR+0x228    //         Address Map Register 6
#define ADDR_UMCTL2_ADDRMAP11              DWC_DDR_UMCTL_BASE_ADDR+0x22c    //         Address Map Register 6
#define ADDR_UMCTL2_ODTCFG                 DWC_DDR_UMCTL_BASE_ADDR+0x240    //         ODT Configuration Register
#define ADDR_UMCTL2_ODTMAP                 DWC_DDR_UMCTL_BASE_ADDR+0x244    //         ODT/Rank Map Register
#define ADDR_UMCTL2_SCHED                  DWC_DDR_UMCTL_BASE_ADDR+0x250    //         Scheduler Control Register    *
#define ADDR_UMCTL2_SCHED1                 DWC_DDR_UMCTL_BASE_ADDR+0x254    //         Scheduler Control Register 1  *
#define ADDR_UMCTL2_PERFHPR0               DWC_DDR_UMCTL_BASE_ADDR+0x258    //         High Priority Read CAM Register 0
#define ADDR_UMCTL2_PERFHPR1               DWC_DDR_UMCTL_BASE_ADDR+0x25c    //         High Priority Read CAM Register 1
#define ADDR_UMCTL2_PERFLPR0               DWC_DDR_UMCTL_BASE_ADDR+0x260    //         Low Priority Read CAM Register 0
#define ADDR_UMCTL2_PERFLPR1               DWC_DDR_UMCTL_BASE_ADDR+0x264    //         Low Priority Read CAM Register 1
#define ADDR_UMCTL2_PERFWR0                DWC_DDR_UMCTL_BASE_ADDR+0x268    //         Write CAM Register 0
#define ADDR_UMCTL2_PERFWR1                DWC_DDR_UMCTL_BASE_ADDR+0x26C    //         Write CAM Register 1

#define ADDR_UMCTL2_PERFVPR1                DWC_DDR_UMCTL_BASE_ADDR+0x274    //         variable priority read cam register 1 !
#define ADDR_UMCTL2_PERFVPW1                DWC_DDR_UMCTL_BASE_ADDR+0x278    //         variable priority read cam register 1 !


#define ADDR_UMCTL2_DBG0                   DWC_DDR_UMCTL_BASE_ADDR+0x300    //         Debug Register 0
#define ADDR_UMCTL2_DBG1                   DWC_DDR_UMCTL_BASE_ADDR+0x304    //         Debug Register 1
#define ADDR_UMCTL2_DBGCAM                 DWC_DDR_UMCTL_BASE_ADDR+0x308    //         CAM Debug Register
#define ADDR_UMCTL2_DBGCMD                 DWC_DDR_UMCTL_BASE_ADDR+0x30c    //         CAM Debug Register
#define ADDR_UMCTL2_SWCTL				   DWC_DDR_UMCTL_BASE_ADDR+0x320	//		   Software Register Programming Control Enable
#define ADDR_UMCTL2_SWSTAT				   DWC_DDR_UMCTL_BASE_ADDR+0x324	//		   Register programming done ack

/*uMCTL2 Multi Port Registers*/
#define ADDR_UMCTL2_PSTAT                  DWC_DDR_UMCTL_BASE_ADDR+0x3fc    //         port status register
#define ADDR_UMCTL2_PCCFG                  DWC_DDR_UMCTL_BASE_ADDR+0x400    //         port common config Register
#define	ADDR_UMCTL2_PCFGR(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0x404 + x * 0xb0)
#define	ADDR_UMCTL2_PCFGW(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0x408 + x * 0xb0)
#define	ADDR_UMCTL2_PCFGC(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0x40c + x * 0xb0)
#define	ADDR_UMCTL2_PCFGIDMASKCH(x, y)	   (DWC_DDR_UMCTL_BASE_ADDR+0x410 + x * 0x08 + y * 0xb0)
#define	ADDR_UMCTL2_PCFGIDVALIDCH(x, y)	   (DWC_DDR_UMCTL_BASE_ADDR+0x414 + x * 0x08 + y * 0xb0)
#define	ADDR_UMCTL2_PCTRL(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0x490 + x * 0xb0)	//port x control register
#define	ADDR_UMCTL2_PCFGQOS0(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0x494 + x * 0xb0)
#define	ADDR_UMCTL2_PCFGQOS1(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0x498 + x * 0xb0)
#define	ADDR_UMCTL2_PCFGWQOS0(x)		   (DWC_DDR_UMCTL_BASE_ADDR+0x49c + x * 0xb0)
#define	ADDR_UMCTL2_PCFGWQOS1(x)		   (DWC_DDR_UMCTL_BASE_ADDR+0x4a0 + x * 0xb0)
#define ADDR_UMCTL2_SARBASE(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0xf04 + x * 0x08)
#define ADDR_UMCTL2_SARSIZE(x)			   (DWC_DDR_UMCTL_BASE_ADDR+0xf08 + x * 0x08)
#define ADDR_UMCTL2_SBRCTL					DWC_DDR_UMCTL_BASE_ADDR+0xf24    //         port common config Register
#define ADDR_UMCTL2_SBRSTAT                 DWC_DDR_UMCTL_BASE_ADDR+0xf28    //         port common config Register
#define ADDR_UMCTL2_SBRWDATA0				DWC_DDR_UMCTL_BASE_ADDR+0xf2c    //         port common config Register
#define ADDR_UMCTL2_SBRWDATA1               DWC_DDR_UMCTL_BASE_ADDR+0xf30    //         port common config Register

#define ADDR_UMCTL2_PCFGR_0                DWC_DDR_UMCTL_BASE_ADDR+0x404    //Port 0   Configuration Register
#define ADDR_UMCTL2_PCFGW_0                DWC_DDR_UMCTL_BASE_ADDR+0x408    //Port 0   Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_0        DWC_DDR_UMCTL_BASE_ADDR+0x40C    //Port 0   Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_0       DWC_DDR_UMCTL_BASE_ADDR+0x410    //Port 0   Configuration Register

#define ADDR_UMCTL2_PCFGQOS0_0               DWC_DDR_UMCTL_BASE_ADDR+0x494    //Port n read qos configuration register0!
#define ADDR_UMCTL2_PCFGQOS1_0               DWC_DDR_UMCTL_BASE_ADDR+0x498    //Port n read qos configuration register1!

#define ADDR_UMCTL2_PCFGWQOS0_0               DWC_DDR_UMCTL_BASE_ADDR+0x49c    //Port n Write qos configuration register0!
#define ADDR_UMCTL2_PCFGWQOS1_0               DWC_DDR_UMCTL_BASE_ADDR+0x4a0    //Port n Write qos configuration register1!

#define ADDR_UMCTL2_PCFGR_1                DWC_DDR_UMCTL_BASE_ADDR+0x4B4    //Port 1  Configuration Register
#define ADDR_UMCTL2_PCFGW_1                DWC_DDR_UMCTL_BASE_ADDR+0x4B8    //Port 1  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_1        DWC_DDR_UMCTL_BASE_ADDR+0x4BC    //Port 1  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_1       DWC_DDR_UMCTL_BASE_ADDR+0x4C0    //Port 1  Configuration Register

#define ADDR_UMCTL2_PCFGQOS0_1                  DWC_DDR_UMCTL_BASE_ADDR+0x544    //!
#define ADDR_UMCTL2_PCFGQOS1_1                  DWC_DDR_UMCTL_BASE_ADDR+0x548    //!
#define ADDR_UMCTL2_PCFGWQOS0_1                             DWC_DDR_UMCTL_BASE_ADDR+0x54c  //!
#define ADDR_UMCTL2_PCFGWQOS1_1                             DWC_DDR_UMCTL_BASE_ADDR+0x550  //!


#define ADDR_UMCTL2_PCFGR_2                DWC_DDR_UMCTL_BASE_ADDR+0x564    //Port 2  Configuration Register
#define ADDR_UMCTL2_PCFGW_2                DWC_DDR_UMCTL_BASE_ADDR+0x568    //Port 2  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_2        DWC_DDR_UMCTL_BASE_ADDR+0x56C    //Port 2  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_2       DWC_DDR_UMCTL_BASE_ADDR+0x570    //Port 2  Configuration Register

#define ADDR_UMCTL2_PCFGQOS0_2              DWC_DDR_UMCTL_BASE_ADDR+0x5f4    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS1_2              DWC_DDR_UMCTL_BASE_ADDR+0x5f8    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS0_2             DWC_DDR_UMCTL_BASE_ADDR+0x5fc    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS1_2             DWC_DDR_UMCTL_BASE_ADDR+0x600    //Port 2  Configuration Register!


#define ADDR_UMCTL2_PCFGR_3                DWC_DDR_UMCTL_BASE_ADDR+0x614    //Port 3  Configuration Register
#define ADDR_UMCTL2_PCFGW_3                DWC_DDR_UMCTL_BASE_ADDR+0x618    //Port 3  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_3        DWC_DDR_UMCTL_BASE_ADDR+0x61C    //Port 3  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_3       DWC_DDR_UMCTL_BASE_ADDR+0x620    //Port 3  Configuration Register  !
#define ADDR_UMCTL2_PCFGQOS0_3               DWC_DDR_UMCTL_BASE_ADDR+0x6a4    //Port 3  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS1_3               DWC_DDR_UMCTL_BASE_ADDR+0x6a8    //Port 3  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS0_3             DWC_DDR_UMCTL_BASE_ADDR+0x6ac    //Port 3  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS1_3             DWC_DDR_UMCTL_BASE_ADDR+0x6b0    //Port 3  Configuration Register!

#define ADDR_UMCTL2_PCFGR_4                DWC_DDR_UMCTL_BASE_ADDR+0x6c4    //Port 4  Configuration Register !
#define ADDR_UMCTL2_PCFGW_4                DWC_DDR_UMCTL_BASE_ADDR+0x6c8    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGIDMASKCH0_4        DWC_DDR_UMCTL_BASE_ADDR+0x6cc    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGIDVALIDCH0_4       DWC_DDR_UMCTL_BASE_ADDR+0x6d0    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS0_4              DWC_DDR_UMCTL_BASE_ADDR+0x754    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS1_4              DWC_DDR_UMCTL_BASE_ADDR+0x758    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS0_4              DWC_DDR_UMCTL_BASE_ADDR+0x75c    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS1_4              DWC_DDR_UMCTL_BASE_ADDR+0x760    //Port 4  Configuration Register!


#define ADDR_UMCTL2_PCFGR_5                DWC_DDR_UMCTL_BASE_ADDR+0x6C4    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGW_5                DWC_DDR_UMCTL_BASE_ADDR+0x6C8    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_5        DWC_DDR_UMCTL_BASE_ADDR+0x6CC    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_5       DWC_DDR_UMCTL_BASE_ADDR+0x6D0    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGR_6                DWC_DDR_UMCTL_BASE_ADDR+0x774    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGW_6                DWC_DDR_UMCTL_BASE_ADDR+0x778    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_6        DWC_DDR_UMCTL_BASE_ADDR+0x77C    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_6       DWC_DDR_UMCTL_BASE_ADDR+0x780    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGR_7                DWC_DDR_UMCTL_BASE_ADDR+0x824    //Port 7  Configuration Register
#define ADDR_UMCTL2_PCFGW_7                DWC_DDR_UMCTL_BASE_ADDR+0x828    //Port 7  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_7        DWC_DDR_UMCTL_BASE_ADDR+0x82C    //Port 7  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_7       DWC_DDR_UMCTL_BASE_ADDR+0x830    //Port 7  Configuration Register

#define ADDR_UMCTL2_PCTRL_0                 DWC_DDR_UMCTL_BASE_ADDR+0x490    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_1                 DWC_DDR_UMCTL_BASE_ADDR+0x540    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_2                 DWC_DDR_UMCTL_BASE_ADDR+0x5f0    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_3                 DWC_DDR_UMCTL_BASE_ADDR+0x6a0    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_4                 DWC_DDR_UMCTL_BASE_ADDR+0x750    //Port 2  Configuration Register!

                                                                                                
/*=======================================================================                        
 * DWC_DDR_PUBL Registers  PHY                                                                   
 *=====================================================================*/                        
                                                                                                 
#define ADDR_PHY_RIDR                    (DWC_DDR_PUBL_BASE_ADDR+0X000)      /* Revision Identification Register */
#define ADDR_PHY_PIR                     (DWC_DDR_PUBL_BASE_ADDR+0X004)      /* PHY Initialization Register */
#define ADDR_PHY_PGCR                    (DWC_DDR_PUBL_BASE_ADDR+0X008)      /* PHY General Configuration Register */
#define ADDR_PHY_PGSR                    (DWC_DDR_PUBL_BASE_ADDR+0X00c)      /* PHY General Status Register */
#define ADDR_PHY_DLLGCR                  (DWC_DDR_PUBL_BASE_ADDR+0X010)      /* DLL General Control Register */
#define ADDR_PHY_ACDLLCR                 (DWC_DDR_PUBL_BASE_ADDR+0X014)      /* AC DLL Control Register */
#define ADDR_PHY_PTR0                    (DWC_DDR_PUBL_BASE_ADDR+0X018)      /* PHY Timing Register 0 */
#define ADDR_PHY_PTR1                    (DWC_DDR_PUBL_BASE_ADDR+0X01c)      /* PHY Timing Register 1 */
#define ADDR_PHY_PTR2                    (DWC_DDR_PUBL_BASE_ADDR+0X020)      /* PHY Timing Register 2 */
#define ADDR_PHY_ACIOCR                  (DWC_DDR_PUBL_BASE_ADDR+0X024)      /* AC I/O Configuration Register */
#define ADDR_PHY_DXCCR                   (DWC_DDR_PUBL_BASE_ADDR+0X028)      /* DATX8 Common Configuratio Register */
#define ADDR_PHY_DSGCR                   (DWC_DDR_PUBL_BASE_ADDR+0X02c)      /* DDR System General Configuration Register */
#define ADDR_PHY_DCR                     (DWC_DDR_PUBL_BASE_ADDR+0X030)      /* DRAM Configuration Register */
#define ADDR_PHY_DTPR0                   (DWC_DDR_PUBL_BASE_ADDR+0X034)      /* DRAM Timing Parameters Register 0 */
#define ADDR_PHY_DTPR1                   (DWC_DDR_PUBL_BASE_ADDR+0X038)      /* DRAM Timing Parameters Register 1 */
#define ADDR_PHY_DTPR2                   (DWC_DDR_PUBL_BASE_ADDR+0X03c)      /* DRAM Timing Parameters Register 2 */
#define ADDR_PHY_MR0                     (DWC_DDR_PUBL_BASE_ADDR+0X040)      /* Mode Register 0 */
#define ADDR_PHY_MR1                     (DWC_DDR_PUBL_BASE_ADDR+0X044)      /* Mode Register 1 */
#define ADDR_PHY_MR2                     (DWC_DDR_PUBL_BASE_ADDR+0X048)      /* Mode Register 2 */
#define ADDR_PHY_MR3                     (DWC_DDR_PUBL_BASE_ADDR+0X04c)      /* Mode Register 3 */
#define ADDR_PHY_ODTCR                   (DWC_DDR_PUBL_BASE_ADDR+0X050)      /* ODT Configuration Register */
#define ADDR_PHY_DTAR                    (DWC_DDR_PUBL_BASE_ADDR+0X054)      /* Data Training Address Register */
#define ADDR_PHY_DTDR0                   (DWC_DDR_PUBL_BASE_ADDR+0X058)      /* Data Training Data Register 0 */
#define ADDR_PHY_DTDR1                   (DWC_DDR_PUBL_BASE_ADDR+0X05c)      /* Data Training Data Register 1 */
/* 0X18~0X2F RESERVED */                                                                        
#define ADDR_PHY_DCUAR                   (DWC_DDR_PUBL_BASE_ADDR+0X0c0)      /* DCU Address Register */
#define ADDR_PHY_DCUDR                   (DWC_DDR_PUBL_BASE_ADDR+0X0c4)      /* DCU Data Register */
#define ADDR_PHY_DCURR                   (DWC_DDR_PUBL_BASE_ADDR+0X0c8)      /* DCU Run Register */
#define ADDR_PHY_DCULR                   (DWC_DDR_PUBL_BASE_ADDR+0X0cc)      /* DCU Loop Register */
#define ADDR_PHY_DCUGCR                  (DWC_DDR_PUBL_BASE_ADDR+0X0d0)      /* DCU General Configuration Register */
#define ADDR_PHY_DCUTPR                  (DWC_DDR_PUBL_BASE_ADDR+0X0d4)      /* DCU Timing Rarameters Register */
#define ADDR_PHY_DCUSR0                  (DWC_DDR_PUBL_BASE_ADDR+0X0d8)      /* DCU Status Register 0 */
#define ADDR_PHY_DCUSR1                  (DWC_DDR_PUBL_BASE_ADDR+0X0dc)      /* DCU Status Register 1 */
/* 0X38~0X5E RESERVED */                                                                        
#define ADDR_PHY_BISTRR                  (DWC_DDR_PUBL_BASE_ADDR+0X100)      /* BIST Run Register */
#define ADDR_PHY_BISTMSKR0               (DWC_DDR_PUBL_BASE_ADDR+0X104)      /* BIST Mask Register 0 */
#define ADDR_PHY_BISTMSKR1               (DWC_DDR_PUBL_BASE_ADDR+0X108)      /* BIST Mask Register 1 */
#define ADDR_PHY_BISTWCR                 (DWC_DDR_PUBL_BASE_ADDR+0X10c)      /* BIST Word Count  Register */
#define ADDR_PHY_BISTLSR                 (DWC_DDR_PUBL_BASE_ADDR+0X110)      /* BIST LFSR Seed Register */
#define ADDR_PHY_BISTAR0                 (DWC_DDR_PUBL_BASE_ADDR+0X114)      /* BIST Address Register 0 */
#define ADDR_PHY_BISTAR1                 (DWC_DDR_PUBL_BASE_ADDR+0X118)      /* BIST Address Register 1 */
#define ADDR_PHY_BISTAR2                 (DWC_DDR_PUBL_BASE_ADDR+0X11c)      /* BIST Address Register 2 */
#define ADDR_PHY_BISTUDPR                (DWC_DDR_PUBL_BASE_ADDR+0X120)      /* BIST User Data Pattern Register */
#define ADDR_PHY_BISTGSR                 (DWC_DDR_PUBL_BASE_ADDR+0X124)      /* BIST General Status Register */
#define ADDR_PHY_BISTWER                 (DWC_DDR_PUBL_BASE_ADDR+0X128)      /* BIST Word Error Register */
#define ADDR_PHY_BISTBER0                (DWC_DDR_PUBL_BASE_ADDR+0X12c)      /* BIST Bit Error Register 0 */
#define ADDR_PHY_BISTBER1                (DWC_DDR_PUBL_BASE_ADDR+0X130)      /* BIST Bit Error Register 1 */
#define ADDR_PHY_BISTBER2                (DWC_DDR_PUBL_BASE_ADDR+0X134)      /* BIST Bit Error Register 2 */
#define ADDR_PHY_BISTWCSR                (DWC_DDR_PUBL_BASE_ADDR+0X138)      /* BIST Wor Count Status Register */
#define ADDR_PHY_BISTFWR0                (DWC_DDR_PUBL_BASE_ADDR+0X13c)      /* BIST Fail Word Register 0 */
#define ADDR_PHY_BISTFWR1                (DWC_DDR_PUBL_BASE_ADDR+0X140)      /* BIST Fail Word Register 1 */
/* 0X51~0X5F RESERVED */                                                                        
#define ADDR_PHY_ZQ0CR0                  (DWC_DDR_PUBL_BASE_ADDR+0X180)      /* ZQ 0 Impedance Control Register 0 */
#define ADDR_PHY_ZQ0CR1                  (DWC_DDR_PUBL_BASE_ADDR+0X184)      /* ZQ 0 Impedance Control Register 1 */
#define ADDR_PHY_ZQ0SR0                  (DWC_DDR_PUBL_BASE_ADDR+0X188)      /* ZQ 0 Impedance Status Register 0 */
#define ADDR_PHY_ZQ0SR1                  (DWC_DDR_PUBL_BASE_ADDR+0X18c)      /* ZQ 0 Impedance Status Register 1 */
#define ADDR_PHY_ZQ1CR0                  (DWC_DDR_PUBL_BASE_ADDR+0X190)      /* ZQ 1 Impedance Control Register 0 */
#define ADDR_PHY_ZQ1CR1                  (DWC_DDR_PUBL_BASE_ADDR+0X194)      /* ZQ 1 Impedance Control Register 1 */
#define ADDR_PHY_ZQ1SR0                  (DWC_DDR_PUBL_BASE_ADDR+0X198)      /* ZQ 1 Impedance Status Register 0 */
#define ADDR_PHY_ZQ1SR1                  (DWC_DDR_PUBL_BASE_ADDR+0X19c)      /* ZQ 1 Impedance Status Register 1 */
#define ADDR_PHY_ZQ2CR0                  (DWC_DDR_PUBL_BASE_ADDR+0X1a0)      /* ZQ 2 Impedance Control Register 0 */
#define ADDR_PHY_ZQ2CR1                  (DWC_DDR_PUBL_BASE_ADDR+0X1a4)      /* ZQ 2 Impedance Control Register 1 */
#define ADDR_PHY_ZQ2SR0                  (DWC_DDR_PUBL_BASE_ADDR+0X1a8)      /* ZQ 2 Impedance Status Register 0 */
#define ADDR_PHY_ZQ2SR1                  (DWC_DDR_PUBL_BASE_ADDR+0X1ac)      /* ZQ 2 Impedance Status Register 1 */
#define ADDR_PHY_ZQ3CR0                  (DWC_DDR_PUBL_BASE_ADDR+0X1b0)      /* ZQ 3 Impedance Control Register 0 */
#define ADDR_PHY_ZQ3CR1                  (DWC_DDR_PUBL_BASE_ADDR+0X1b4)      /* ZQ 3 Impedance Control Register 1 */
#define ADDR_PHY_ZQ3SR0                  (DWC_DDR_PUBL_BASE_ADDR+0X1b8)      /* ZQ 3 Impedance Status Register 0 */
#define ADDR_PHY_ZQ3SR1                  (DWC_DDR_PUBL_BASE_ADDR+0X1bc)      /* ZQ 3 Impedance Status Register 1 */
#define ADDR_PHY_DX0GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X1c0)      /* DATX8 0 General Configuration Register */
#define ADDR_PHY_DX0GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X1c4)      /* DATX8 0 General Status Register 0 */
#define ADDR_PHY_DX0GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X1c8)      /* DATX8 0 General Status Register 1 */
#define ADDR_PHY_DX0DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X1cc)      /* DATX8 0 DLL Control Register */
#define ADDR_PHY_DX0DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X1d0)      /* DATX8 0 DQ Timing Register */
#define ADDR_PHY_DX0DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X1d4)      /* DATX8 0 DQS Timing Register */
/* 0X76~0X7F RESERVED */                                                                        
#define ADDR_PHY_DX1GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X200)      /* DATX8 1 General Configuration Register */
#define ADDR_PHY_DX1GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X204)      /* DATX8 1 General Status Register 0 */
#define ADDR_PHY_DX1GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X208)      /* DATX8 1 General Status Register 1 */     
#define ADDR_PHY_DX1DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X20c)      /* DATX8 1 DLL Control Register */          
#define ADDR_PHY_DX1DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X210)      /* DATX8 1 DQ Timing Register */            
#define ADDR_PHY_DX1DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X214)      /* DATX8 1 DQS Timing Register */           
/* 0X86~0X8F RESERVED */                                                                        
#define ADDR_PHY_DX2GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X240)      /* DATX8 2 General Configuration Register */
#define ADDR_PHY_DX2GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X244)      /* DATX8 2 General Status Register 0 */     
#define ADDR_PHY_DX2GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X248)      /* DATX8 2 General Status Register 1 */     
#define ADDR_PHY_DX2DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X24c)      /* DATX8 2 DLL Control Register */          
#define ADDR_PHY_DX2DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X250)      /* DATX8 2 DQ Timing Register */            
#define ADDR_PHY_DX2DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X254)      /* DATX8 2 DQS Timing Register */           
/* 0X96~0X9F RESERVED */                                                                        
#define ADDR_PHY_DX3GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X280)      /* DATX8 3 General Configuration Register */
#define ADDR_PHY_DX3GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X284)      /* DATX8 3 General Status Register 0 */     
#define ADDR_PHY_DX3GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X288)      /* DATX8 3 General Status Register 1 */     
#define ADDR_PHY_DX3DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X28c)      /* DATX8 3 DLL Control Register */          
#define ADDR_PHY_DX3DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X290)      /* DATX8 3 DQ Timing Register */            
#define ADDR_PHY_DX3DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X294)      /* DATX8 3 DQS Timing Register */           
/* 0XA6~0XAF RESERVED */                                                                         
#define ADDR_PHY_DX4GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X2c0)      /* DATX8 4 General Configuration Register */
#define ADDR_PHY_DX4GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X2c4)      /* DATX8 4 General Status Register 0 */     
#define ADDR_PHY_DX4GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X2c8)      /* DATX8 4 General Status Register 1 */     
#define ADDR_PHY_DX4DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X2cc)      /* DATX8 4 DLL Control Register */          
#define ADDR_PHY_DX4DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X2d0)      /* DATX8 4 DQ Timing Register */            
#define ADDR_PHY_DX4DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X2d4)      /* DATX8 4 DQS Timing Register */           
/* 0XB6~0XBF RESERVED */                                                                         
#define ADDR_PHY_DX5GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X300)      /* DATX8 5 General Configuration Register */
#define ADDR_PHY_DX5GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X304)      /* DATX8 5 General Status Register 0 */     
#define ADDR_PHY_DX5GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X308)      /* DATX8 5 General Status Register 1 */     
#define ADDR_PHY_DX5DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X30c)      /* DATX8 5 DLL Control Register */          
#define ADDR_PHY_DX5DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X310)      /* DATX8 5 DQ Timing Register */            
#define ADDR_PHY_DX5DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X314)      /* DATX8 5 DQS Timing Register */           
/* 0XC6~0XCF RESERVED */                                                                        
#define ADDR_PHY_DX6GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X340)      /* DATX8 6 General Configuration Register */
#define ADDR_PHY_DX6GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X344)      /* DATX8 6 General Status Register 0 */     
#define ADDR_PHY_DX6GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X348)      /* DATX8 6 General Status Register 1 */     
#define ADDR_PHY_DX6DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X34c)      /* DATX8 6 DLL Control Register */          
#define ADDR_PHY_DX6DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X350)      /* DATX8 6 DQ Timing Register */            
#define ADDR_PHY_DX6DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X354)      /* DATX8 6 DQS Timing Register */           
/* 0XD6~0XDF RESERVED */                                                                        
#define ADDR_PHY_DX7GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X380)      /* DATX8 7 General Configuration Register */
#define ADDR_PHY_DX7GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X384)      /* DATX8 7 General Status Register 0 */     
#define ADDR_PHY_DX7GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X388)      /* DATX8 7 General Status Register 1 */     
#define ADDR_PHY_DX7DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X38c)      /* DATX8 7 DLL Control Register */          
#define ADDR_PHY_DX7DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X390)      /* DATX8 7 DQ Timing Register */            
#define ADDR_PHY_DX7DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X394)      /* DATX8 7 DQS Timing Register */           
/* 0XE6~0XEF RESERVED */                                                                        
#define ADDR_PHY_DX8GCR                  (DWC_DDR_PUBL_BASE_ADDR+0X3c0)      /* DATX8 8 General Configuration Register */
#define ADDR_PHY_DX8GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X3c4)      /* DATX8 8 General Status Register 0 */     
#define ADDR_PHY_DX8GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X3c8)      /* DATX8 8 General Status Register 1 */     
#define ADDR_PHY_DX8DLLCR                (DWC_DDR_PUBL_BASE_ADDR+0X3cc)      /* DATX8 8 DLL Control Register */          
#define ADDR_PHY_DX8DQTR                 (DWC_DDR_PUBL_BASE_ADDR+0X3d0)      /* DATX8 8 DQ Timing Register */            
#define ADDR_PHY_DX8DQSTR                (DWC_DDR_PUBL_BASE_ADDR+0X3d4)      /* DATX8 8 DQS Timing Register */                         
/* 0XF6~0XFF RESERVED */ 
#define ADDR_PHY_DXGCR(x)                (DWC_DDR_PUBL_BASE_ADDR + 0X1c0 + 0x40 * x)      /* DATX8 0 General Configuration Register */
#define ADDR_PHY_DXGSR0(x)               (DWC_DDR_PUBL_BASE_ADDR + 0X1c4 + 0x40 * x)      /* DATX8 0 General Status Register 0 */
#define ADDR_PHY_DXGSR1(x)               (DWC_DDR_PUBL_BASE_ADDR + 0X1c8 + 0x40 * x)      /* DATX8 0 General Status Register 1 */
#define ADDR_PHY_DXDLLCR(x)              (DWC_DDR_PUBL_BASE_ADDR + 0X1cc + 0x40 * x)      /* DATX8 0 DLL Control Register */
#define ADDR_PHY_DXDQTR(x)               (DWC_DDR_PUBL_BASE_ADDR + 0X1d0 + 0x40 * x)      /* DATX8 0 DQ Timing Register */
#define ADDR_PHY_DXDQSTR(x)              (DWC_DDR_PUBL_BASE_ADDR + 0X1d4 + 0x40 * x)      /* DATX8 0 DQS Timing Register */

/*ADDITIONAL*/
/*DRAM Controller System Management Register*/
#define	EMIF_SYSM_ADDR_CORE_RES		(EMIF_SYSM_ADDR + 0x0)
#define	EMIF_SYSM_ADDR_BUS_AND_PHY_PLL_CLK		(EMIF_SYSM_ADDR + 0x4)
#define	EMIF_SYSM_ADDR_BUS_RES		(EMIF_SYSM_ADDR + 0xc)
#define	EMIF_SYSM_ADDR_APB_AND_PHY_CLK	   (EMIF_SYSM_ADDR + 0x10)
#define EMIF_SYSM_ADDR_MAP_REG		(EMIF_SYSM_ADDR + 0x38)
#define EMIF_SYSM_ADDR_CE2_A15		(EMIF_SYSM_ADDR + 0x84)
//#define CONFIG_COMPILE_DDR2
#endif

