#ifndef __IMAPX_DRAMC__
#define __IMAPX_DRAMC__

/*========================================================================
 * DWC_DDR_UMCTL2_MAP Registers
 *======================================================================*/

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
#define ADDR_UMCTL2_PERFHPR1               DWC_DDR_UMCTL_BASE_ADDR+0x25c    //         High Priority Read CAM Register 1
#define ADDR_UMCTL2_PERFLPR1               DWC_DDR_UMCTL_BASE_ADDR+0x264    //         Low Priority Read CAM Register 1
#define ADDR_UMCTL2_PERFWR1                DWC_DDR_UMCTL_BASE_ADDR+0x26C    //         Write CAM Register 1
#define ADDR_UMCTL2_PERFVPR1               DWC_DDR_UMCTL_BASE_ADDR+0x274    //         Write CAM Register 1
#define ADDR_UMCTL2_PERFVPW1               DWC_DDR_UMCTL_BASE_ADDR+0x278    //         Write CAM Register 1
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
#define ADDR_UMCTL2_PCFGR_1                DWC_DDR_UMCTL_BASE_ADDR+0x4B4    //Port 1  Configuration Register
#define ADDR_UMCTL2_PCFGW_1                DWC_DDR_UMCTL_BASE_ADDR+0x4B8    //Port 1  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_1        DWC_DDR_UMCTL_BASE_ADDR+0x4BC    //Port 1  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_1       DWC_DDR_UMCTL_BASE_ADDR+0x4C0    //Port 1  Configuration Register
#define ADDR_UMCTL2_PCFGR_2                DWC_DDR_UMCTL_BASE_ADDR+0x564    //Port 2  Configuration Register
#define ADDR_UMCTL2_PCFGW_2                DWC_DDR_UMCTL_BASE_ADDR+0x568    //Port 2  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_2        DWC_DDR_UMCTL_BASE_ADDR+0x56C    //Port 2  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_2       DWC_DDR_UMCTL_BASE_ADDR+0x570    //Port 2  Configuration Register
#define ADDR_UMCTL2_PCFGR_3                DWC_DDR_UMCTL_BASE_ADDR+0x614    //Port 3  Configuration Register
#define ADDR_UMCTL2_PCFGW_3                DWC_DDR_UMCTL_BASE_ADDR+0x618    //Port 3  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_3        DWC_DDR_UMCTL_BASE_ADDR+0x61C    //Port 3  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_3       DWC_DDR_UMCTL_BASE_ADDR+0x620    //Port 3  Configuration Register
#define ADDR_UMCTL2_PCFGR_4                DWC_DDR_UMCTL_BASE_ADDR+0x6c4    //Port 4  Configuration Register
#define ADDR_UMCTL2_PCFGW_4                DWC_DDR_UMCTL_BASE_ADDR+0x6c8    //Port 4  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_4        DWC_DDR_UMCTL_BASE_ADDR+0x6cC    //Port 4  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_4       DWC_DDR_UMCTL_BASE_ADDR+0x6d0    //Port 4  Configuration Register
#define ADDR_UMCTL2_PCFGR_5                DWC_DDR_UMCTL_BASE_ADDR+0x774    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGW_5                DWC_DDR_UMCTL_BASE_ADDR+0x778    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_5        DWC_DDR_UMCTL_BASE_ADDR+0x77C    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_5       DWC_DDR_UMCTL_BASE_ADDR+0x780    //Port 5  Configuration Register
#define ADDR_UMCTL2_PCFGR_6                DWC_DDR_UMCTL_BASE_ADDR+0x824    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGW_6                DWC_DDR_UMCTL_BASE_ADDR+0x828    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_6        DWC_DDR_UMCTL_BASE_ADDR+0x82C    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_6       DWC_DDR_UMCTL_BASE_ADDR+0x830    //Port 6  Configuration Register
#define ADDR_UMCTL2_PCFGR_7                DWC_DDR_UMCTL_BASE_ADDR+0x8d4    //Port 7  Configuration Register
#define ADDR_UMCTL2_PCFGW_7                DWC_DDR_UMCTL_BASE_ADDR+0x8d8    //Port 7  Configuration Register
#define ADDR_UMCTL2_PCFGIDMASKCH0_7        DWC_DDR_UMCTL_BASE_ADDR+0x8dC    //Port 7  Configuration Register
#define ADDR_UMCTL2_PCFGIDVALIDCH0_7       DWC_DDR_UMCTL_BASE_ADDR+0x8e0    //Port 7  Configuration Register
 
#define ADDR_UMCTL2_PCTRL_0                 DWC_DDR_UMCTL_BASE_ADDR+0x490    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_1                 DWC_DDR_UMCTL_BASE_ADDR+0x540    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_2                 DWC_DDR_UMCTL_BASE_ADDR+0x5f0    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_3                 DWC_DDR_UMCTL_BASE_ADDR+0x6a0    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCTRL_4                 DWC_DDR_UMCTL_BASE_ADDR+0x750    //Port 2  Configuration Register!

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

#define ADDR_UMCTL2_PCFGQOS0_2              DWC_DDR_UMCTL_BASE_ADDR+0x5f4    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS1_2              DWC_DDR_UMCTL_BASE_ADDR+0x5f8    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS0_2             DWC_DDR_UMCTL_BASE_ADDR+0x5fc    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS1_2             DWC_DDR_UMCTL_BASE_ADDR+0x600    //Port 2  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS0_3               DWC_DDR_UMCTL_BASE_ADDR+0x6a4    //Port 3  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS1_3               DWC_DDR_UMCTL_BASE_ADDR+0x6a8    //Port 3  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS0_3             DWC_DDR_UMCTL_BASE_ADDR+0x6ac    //Port 3  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS1_3             DWC_DDR_UMCTL_BASE_ADDR+0x6b0    //Port 3  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS0_4              DWC_DDR_UMCTL_BASE_ADDR+0x754    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGQOS1_4              DWC_DDR_UMCTL_BASE_ADDR+0x758    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS0_4              DWC_DDR_UMCTL_BASE_ADDR+0x75c    //Port 4  Configuration Register!
#define ADDR_UMCTL2_PCFGWQOS1_4              DWC_DDR_UMCTL_BASE_ADDR+0x760    //Port 4  Configuration Register!
/*=======================================================================
 * DWC_DDR_PUBL Registers  PHY                                                                   
 *=====================================================================*/                        
                                                                                                 
#define ADDR_PHY_RIDR                    (DWC_DDR_PUBL_BASE_ADDR+0X000)      /* Revision Identification Register */
#define ADDR_PHY_PIR                     (DWC_DDR_PUBL_BASE_ADDR+0X004)      /* PHY Initialization Register */
#define ADDR_PHY_PGCR                    (DWC_DDR_PUBL_BASE_ADDR+0X008)      /* PHY General Configuration Register */
#define ADDR_PHY_PGCR1                   (DWC_DDR_PUBL_BASE_ADDR+0X00c)      /* PHY General Configuration Register1 */
#define ADDR_PHY_PGCR2                   (DWC_DDR_PUBL_BASE_ADDR+0X010)      /* PHY General Configuration Register2 */
#define ADDR_PHY_PGCR3                   (DWC_DDR_PUBL_BASE_ADDR+0X014)      /* PHY General Configuration Register3 */
#define ADDR_PHY_PGSR                    (DWC_DDR_PUBL_BASE_ADDR+0X018)      /* PHY General Status Register */
#define ADDR_PHY_PGSR1                   (DWC_DDR_PUBL_BASE_ADDR+0X01c)      /* PHY General Status Register1 */
#define ADDR_PHY_PLLCR                   (DWC_DDR_PUBL_BASE_ADDR+0X020)      /* DLL General Control Register */
#define ADDR_PHY_PTR0                    (DWC_DDR_PUBL_BASE_ADDR+0X024)      /* PHY Timing Register 0 */
#define ADDR_PHY_PTR1                    (DWC_DDR_PUBL_BASE_ADDR+0X028)      /* PHY Timing Register 1 */
#define ADDR_PHY_PTR2                    (DWC_DDR_PUBL_BASE_ADDR+0X02c)      /* PHY Timing Register 2 */
#define ADDR_PHY_PTR3                    (DWC_DDR_PUBL_BASE_ADDR+0X030)      /* PHY Timing Register 3 */
#define ADDR_PHY_PTR4                    (DWC_DDR_PUBL_BASE_ADDR+0X034)      /* PHY Timing Register 4 */
#define ADDR_PHY_ACMDLR                  (DWC_DDR_PUBL_BASE_ADDR+0X038)      /* AC Master Delay Line Register */
#define ADDR_PHY_ACLCDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X03c)      /* AC Local Calibrated Dealy Line Register */
#define ADDR_PHY_ACBDLR0                 (DWC_DDR_PUBL_BASE_ADDR+0X040)      /* AC Bit Delay LINE Register 0 */
#define ADDR_PHY_ACBDLR(x)               (DWC_DDR_PUBL_BASE_ADDR+0X040+x*4)  /* AC Bit Delay LINE Register 0~9 */
#define ADDR_PHY_ACIOCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X068)      /* AC I/O Configuration Register 0 */
#define ADDR_PHY_ACIOCR(x)               (DWC_DDR_PUBL_BASE_ADDR+0X068+x*4)  /* AC I/O Configuration Register 0~5 */
#define ADDR_PHY_DXCCR                   (DWC_DDR_PUBL_BASE_ADDR+0X080)      /* DATX8 Common Configuration Register */
#define ADDR_PHY_DSGCR                   (DWC_DDR_PUBL_BASE_ADDR+0X084)      /* DDR System General configuration Register */
#define ADDR_PHY_DCR					 (DWC_DDR_PUBL_BASE_ADDR+0X088)		 /* DRAM Configure Register*/
#define ADDR_PHY_DTPR0                   (DWC_DDR_PUBL_BASE_ADDR+0X08c)      /* DRAM Timing Parameters Register 0 */
#define ADDR_PHY_DTPR1                   (DWC_DDR_PUBL_BASE_ADDR+0X090)      /* DRAM Timing Parameters Register 1 */
#define ADDR_PHY_DTPR2                   (DWC_DDR_PUBL_BASE_ADDR+0X094)      /* DRAM Timing Parameters Register 2 */
#define ADDR_PHY_DTPR3                   (DWC_DDR_PUBL_BASE_ADDR+0X098)      /* DRAM Timing Parameters Register 3 */
#define ADDR_PHY_MR0                     (DWC_DDR_PUBL_BASE_ADDR+0X09c)      /* Mode Register 0 */
#define ADDR_PHY_MR1                     (DWC_DDR_PUBL_BASE_ADDR+0X0a0)      /* Mode Register 1 */
#define ADDR_PHY_MR2                     (DWC_DDR_PUBL_BASE_ADDR+0X0a4)      /* Mode Register 2 */
#define ADDR_PHY_MR3                     (DWC_DDR_PUBL_BASE_ADDR+0X0a8)      /* Mode Register 3 */
#define ADDR_PHY_ODTCR                   (DWC_DDR_PUBL_BASE_ADDR+0X0ac)      /* ODT Configuration Register */
#define ADDR_PHY_DTCR                    (DWC_DDR_PUBL_BASE_ADDR+0X0b0)      /* Data Training Configuration Register */
#define ADDR_PHY_DTAR0                   (DWC_DDR_PUBL_BASE_ADDR+0X0b4)      /* Data Training Address Register 0 */
#define ADDR_PHY_DTAR1                   (DWC_DDR_PUBL_BASE_ADDR+0X0b8)      /* Data Training Address Register 1 */
#define ADDR_PHY_DTAR2                   (DWC_DDR_PUBL_BASE_ADDR+0X0bc)      /* Data Training Address Register 2 */
#define ADDR_PHY_DTAR3                   (DWC_DDR_PUBL_BASE_ADDR+0X0c0)      /* Data Training Address Register 3 */
#define ADDR_PHY_DTDR0                   (DWC_DDR_PUBL_BASE_ADDR+0X0c4)      /* Data Training Data Register 0 */
#define ADDR_PHY_DTDR1                   (DWC_DDR_PUBL_BASE_ADDR+0X0c8)      /* Data Training Data Register 1 */
#define ADDR_PHY_DTEDR0                  (DWC_DDR_PUBL_BASE_ADDR+0X0cc)      /* Data Training Eye Data Register 0 */
#define ADDR_PHY_DTEDR1                  (DWC_DDR_PUBL_BASE_ADDR+0X0d0)      /* Data Training Eye Data Register 1 */
#define ADDR_PHY_RDIMMGCR0				 (DWC_DDR_PUBL_BASE_ADDR+0X0d4)      /* RDIMM Generalg Configure Register 0 */
#define ADDR_PHY_RDIMMGCR1				 (DWC_DDR_PUBL_BASE_ADDR+0X0d8)      /* RDIMM Generalg Configure Register 1 */
#define ADDR_PHY_RDIMMCR0				 (DWC_DDR_PUBL_BASE_ADDR+0X0dc)      /* RDIMM Control Register 0 */
#define ADDR_PHY_RDIMMCR1				 (DWC_DDR_PUBL_BASE_ADDR+0X0e0)      /* RDIMM Control Register 1 */
#define ADDR_PHY_GPR0                    (DWC_DDR_PUBL_BASE_ADDR+0X0e4)      /* Data Generalg Purpose Register 0 */
#define ADDR_PHY_GPR1                    (DWC_DDR_PUBL_BASE_ADDR+0X0e8)      /* Data Generalg Purpose Register 0 */
#define ADDR_PHY_CATR0                   (DWC_DDR_PUBL_BASE_ADDR+0X0ec)      /* CA Training Register 0 */
#define ADDR_PHY_CATR1                   (DWC_DDR_PUBL_BASE_ADDR+0X0f0)      /* CA Training Register 1 */
#define ADDR_PHY_DQSDR                   (DWC_DDR_PUBL_BASE_ADDR+0X0f4)      /* DQS Drift Detection Register */
#define ADDR_PHY_DTMR0                   (DWC_DDR_PUBL_BASE_ADDR+0X0f8)      /* Data Training Mask Regisetr 0 */
#define ADDR_PHY_DTMR1                   (DWC_DDR_PUBL_BASE_ADDR+0X0fc)      /* Data Training Mask Regisetr 1 */
/* 0X40~0X5F RESERVED */

#define ADDR_PHY_DCUAR                   (DWC_DDR_PUBL_BASE_ADDR+0X180)      /* DCU Address Register */
#define ADDR_PHY_DCUDR                   (DWC_DDR_PUBL_BASE_ADDR+0X184)      /* DCU Data Register */
#define ADDR_PHY_DCURR                   (DWC_DDR_PUBL_BASE_ADDR+0X188)      /* DCU Run Register */
#define ADDR_PHY_DCULR                   (DWC_DDR_PUBL_BASE_ADDR+0X18c)      /* DCU Loop Register */
#define ADDR_PHY_DCUGCR                  (DWC_DDR_PUBL_BASE_ADDR+0X190)      /* DCU General Configuration Register */
#define ADDR_PHY_DCUTPR                  (DWC_DDR_PUBL_BASE_ADDR+0X194)      /* DCU Timing Rarameters Register */
#define ADDR_PHY_DCUSR0                  (DWC_DDR_PUBL_BASE_ADDR+0X198)      /* DCU Status Register 0 */
#define ADDR_PHY_DCUSR1                  (DWC_DDR_PUBL_BASE_ADDR+0X19c)      /* DCU Status Register 1 */
#define ADDR_PHY_MR11                    (DWC_DDR_PUBL_BASE_ADDR+0X1a0)      /* Mode Register 11 */
/* 0X69~0X6F RESERVED */

#define ADDR_PHY_BISTRR                  (DWC_DDR_PUBL_BASE_ADDR+0X1c0)      /* BIST Run Register */
#define ADDR_PHY_BISTWCR                 (DWC_DDR_PUBL_BASE_ADDR+0X1c4)      /* BIST Word Count  Register */
#define ADDR_PHY_BISTMSKR0               (DWC_DDR_PUBL_BASE_ADDR+0X1c8)      /* BIST Mask Register 0 */
#define ADDR_PHY_BISTMSKR1               (DWC_DDR_PUBL_BASE_ADDR+0X1cc)      /* BIST Mask Register 1 */
#define ADDR_PHY_BISTMSKR2               (DWC_DDR_PUBL_BASE_ADDR+0X1d0)      /* BIST Mask Register 1 */
#define ADDR_PHY_BISTLSR                 (DWC_DDR_PUBL_BASE_ADDR+0X1d4)      /* BIST LFSR Seed Register */
#define ADDR_PHY_BISTAR0                 (DWC_DDR_PUBL_BASE_ADDR+0X1d8)      /* BIST Address Register 0 */
#define ADDR_PHY_BISTAR1                 (DWC_DDR_PUBL_BASE_ADDR+0X1dc)      /* BIST Address Register 1 */
#define ADDR_PHY_BISTAR2                 (DWC_DDR_PUBL_BASE_ADDR+0X1e0)      /* BIST Address Register 2 */
#define ADDR_PHY_BISTUDPR                (DWC_DDR_PUBL_BASE_ADDR+0X1e4)      /* BIST User Data Pattern Register */
#define ADDR_PHY_BISTGSR                 (DWC_DDR_PUBL_BASE_ADDR+0X1e8)      /* BIST General Status Register */
#define ADDR_PHY_BISTWER                 (DWC_DDR_PUBL_BASE_ADDR+0X1ec)      /* BIST Word Error Register */
#define ADDR_PHY_BISTBER0                (DWC_DDR_PUBL_BASE_ADDR+0X1f4)      /* BIST Bit Error Register 0 */
#define ADDR_PHY_BISTBER1                (DWC_DDR_PUBL_BASE_ADDR+0X1f8)      /* BIST Bit Error Register 1 */
#define ADDR_PHY_BISTBER2                (DWC_DDR_PUBL_BASE_ADDR+0X1fc)      /* BIST Bit Error Register 2 */
#define ADDR_PHY_BISTWCSR                (DWC_DDR_PUBL_BASE_ADDR+0X200)      /* BIST Wor Count Status Register */
#define ADDR_PHY_BISTFWR0                (DWC_DDR_PUBL_BASE_ADDR+0X204)      /* BIST Fail Word Register 0 */
#define ADDR_PHY_BISTFWR1                (DWC_DDR_PUBL_BASE_ADDR+0X208)      /* BIST Fail Word Register 1 */
#define ADDR_PHY_BISTFWR2                (DWC_DDR_PUBL_BASE_ADDR+0X20c)      /* BIST Fail Word Register 2 */
/* 0X84~0X8D RESERVED */
#define ADDR_PHY_IOVCR0					 (DWC_DDR_PUBL_BASE_ADDR+0X238)		 /* IO VREF Control Register 0*/
#define ADDR_PHY_IOVCR1					 (DWC_DDR_PUBL_BASE_ADDR+0X23c)		 /* IO VREF Control Register 1*/

#define ADDR_PHY_ZQCR					 (DWC_DDR_PUBL_BASE_ADDR+0X240)      /* ZQ Impedance Control Register */
#define ADDR_PHY_ZQ0PR                   (DWC_DDR_PUBL_BASE_ADDR+0X244)      /* ZQ 0 Impedance Control Program Register */
#define ADDR_PHY_ZQ0DR                   (DWC_DDR_PUBL_BASE_ADDR+0X248)      /* ZQ 0 Impedance Control data Register */
#define ADDR_PHY_ZQ0SR                   (DWC_DDR_PUBL_BASE_ADDR+0X24c)      /* ZQ 0 Impedance Control Status Register */
#define ADDR_PHY_ZQ1PR                   (DWC_DDR_PUBL_BASE_ADDR+0X254)      /* ZQ 1 Impedance Control Program Register */
#define ADDR_PHY_ZQ1DR                   (DWC_DDR_PUBL_BASE_ADDR+0X258)      /* ZQ 1 Impedance Control data Register */
#define ADDR_PHY_ZQ1SR                   (DWC_DDR_PUBL_BASE_ADDR+0X25c)      /* ZQ 1 Impedance Control Status Register */
#define ADDR_PHY_ZQ2PR                   (DWC_DDR_PUBL_BASE_ADDR+0X264)      /* ZQ 2 Impedance Control Program Register */
#define ADDR_PHY_ZQ2DR                   (DWC_DDR_PUBL_BASE_ADDR+0X268)      /* ZQ 2 Impedance Control data Register */
#define ADDR_PHY_ZQ2SR                   (DWC_DDR_PUBL_BASE_ADDR+0X26c)      /* ZQ 2 Impedance Control Status Register */
#define ADDR_PHY_ZQ3PR                   (DWC_DDR_PUBL_BASE_ADDR+0X274)      /* ZQ 3 Impedance Control Program Register */
#define ADDR_PHY_ZQ3DR                   (DWC_DDR_PUBL_BASE_ADDR+0X278)      /* ZQ 3 Impedance Control data Register */
#define ADDR_PHY_ZQ3SR                   (DWC_DDR_PUBL_BASE_ADDR+0X27c)      /* ZQ 3 Impedance Control Status Register */

#define ADDR_PHY_DX0GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X280)      /* DATX8 0 General Configuration Register 0 */
#define ADDR_PHY_DX0GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X284)      /* DATX8 0 General Configuration Register 1 */
#define ADDR_PHY_DX0GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X288)      /* DATX8 0 General Configuration Register 2 */
#define ADDR_PHY_DX0GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X28c)      /* DATX8 0 General Configuration Register 3 */
#define ADDR_PHY_DX0GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X290)      /* DATX8 0 General Status Register 0 */
#define ADDR_PHY_DX0GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X294)      /* DATX8 0 General Status Register 1 */
#define ADDR_PHY_DX0GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X298)      /* DATX8 0 General Status Register 1 */
#define ADDR_PHY_DX0BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X29c)      /* DATX8 0 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX0BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X2a0)      /* DATX8 0 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX0BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X2a4)      /* DATX8 0 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX0BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X2a8)      /* DATX8 0 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX0BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X2ac)      /* DATX8 0 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX0BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X2b0)      /* DATX8 0 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX0BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X2b4)      /* DATX8 0 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX0LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X2b8)      /* DATX8 0 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX0LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X2bc)      /* DATX8 0 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX0LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X2c0)      /* DATX8 0 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX0MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X2c4)      /* DATX8 0 Master Dealy Line Register */
#define ADDR_PHY_DX0GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X2c8)      /* DATX8 0 General Timing Register */
/* 0XB4~0XBF RESERVED */

#define ADDR_PHY_DX1GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X300)      /* DATX8 1 General Configuration Register 0 */
#define ADDR_PHY_DX1GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X304)      /* DATX8 1 General Configuration Register 1 */
#define ADDR_PHY_DX1GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X308)      /* DATX8 1 General Configuration Register 2 */
#define ADDR_PHY_DX1GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X30c)      /* DATX8 1 General Configuration Register 3 */
#define ADDR_PHY_DX1GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X310)      /* DATX8 1 General Status Register 0 */
#define ADDR_PHY_DX1GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X314)      /* DATX8 1 General Status Register 1 */
#define ADDR_PHY_DX1GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X318)      /* DATX8 1 General Status Register 1 */
#define ADDR_PHY_DX1BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X31c)      /* DATX8 1 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX1BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X320)      /* DATX8 1 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX1BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X324)      /* DATX8 1 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX1BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X328)      /* DATX8 1 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX1BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X32c)      /* DATX8 1 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX1BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X330)      /* DATX8 1 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX1BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X334)      /* DATX8 1 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX1LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X338)      /* DATX8 1 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX1LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X33c)      /* DATX8 1 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX1LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X340)      /* DATX8 1 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX1MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X344)      /* DATX8 1 Master Dealy Line Register */
#define ADDR_PHY_DX1GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X348)      /* DATX8 1 General Timing Register */
/* 0XD4~0XDF RESERVED */

#define ADDR_PHY_DX2GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X380)      /* DATX8 2 General Configuration Register 0 */
#define ADDR_PHY_DX2GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X384)      /* DATX8 2 General Configuration Register 1 */
#define ADDR_PHY_DX2GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X388)      /* DATX8 2 General Configuration Register 2 */
#define ADDR_PHY_DX2GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X38c)      /* DATX8 2 General Configuration Register 3 */
#define ADDR_PHY_DX2GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X390)      /* DATX8 2 General Status Register 0 */
#define ADDR_PHY_DX2GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X394)      /* DATX8 2 General Status Register 1 */
#define ADDR_PHY_DX2GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X398)      /* DATX8 2 General Status Register 1 */
#define ADDR_PHY_DX2BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X39c)      /* DATX8 2 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX2BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X3a0)      /* DATX8 2 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX2BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X3a4)      /* DATX8 2 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX2BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X3a8)      /* DATX8 2 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX2BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X3ac)      /* DATX8 2 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX2BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X3b0)      /* DATX8 2 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX2BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X3b4)      /* DATX8 2 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX2LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X3b8)      /* DATX8 2 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX2LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X3bc)      /* DATX8 2 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX2LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X3c0)      /* DATX8 2 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX2MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X3c4)      /* DATX8 2 Master Dealy Line Register */
#define ADDR_PHY_DX2GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X3c8)      /* DATX8 2 General Timing Register */
/* 0XF4~0XFF RESERVED */

#define ADDR_PHY_DX3GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X400)      /* DATX8 3 General Configuration Register 0 */
#define ADDR_PHY_DX3GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X404)      /* DATX8 3 General Configuration Register 1 */
#define ADDR_PHY_DX3GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X408)      /* DATX8 3 General Configuration Register 2 */
#define ADDR_PHY_DX3GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X40c)      /* DATX8 3 General Configuration Register 3 */
#define ADDR_PHY_DX3GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X410)      /* DATX8 3 General Status Register 0 */
#define ADDR_PHY_DX3GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X414)      /* DATX8 3 General Status Register 1 */
#define ADDR_PHY_DX3GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X418)      /* DATX8 3 General Status Register 1 */
#define ADDR_PHY_DX3BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X41c)      /* DATX8 3 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX3BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X420)      /* DATX8 3 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX3BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X424)      /* DATX8 3 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX3BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X428)      /* DATX8 3 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX3BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X42c)      /* DATX8 3 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX3BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X430)      /* DATX8 3 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX3BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X434)      /* DATX8 3 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX3LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X438)      /* DATX8 3 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX3LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X43c)      /* DATX8 3 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX3LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X440)      /* DATX8 3 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX3MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X444)      /* DATX8 3 Master Dealy Line Register */
#define ADDR_PHY_DX3GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X448)      /* DATX8 3 General Timing Register */
/* 0X114~0X11F RESERVED */

#define ADDR_PHY_DX4GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X480)      /* DATX8 4 General Configuration Register 0 */
#define ADDR_PHY_DX4GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X484)      /* DATX8 4 General Configuration Register 1 */
#define ADDR_PHY_DX4GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X488)      /* DATX8 4 General Configuration Register 2 */
#define ADDR_PHY_DX4GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X48c)      /* DATX8 4 General Configuration Register 3 */
#define ADDR_PHY_DX4GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X490)      /* DATX8 4 General Status Register 0 */
#define ADDR_PHY_DX4GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X494)      /* DATX8 4 General Status Register 1 */
#define ADDR_PHY_DX4GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X498)      /* DATX8 4 General Status Register 1 */
#define ADDR_PHY_DX4BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X49c)      /* DATX8 4 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX4BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X4a0)      /* DATX8 4 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX4BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X4a4)      /* DATX8 4 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX4BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X4a8)      /* DATX8 4 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX4BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X4ac)      /* DATX8 4 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX4BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X4b0)      /* DATX8 4 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX4BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X4b4)      /* DATX8 4 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX4LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X4b8)      /* DATX8 4 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX4LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X4bc)      /* DATX8 4 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX4LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X4c0)      /* DATX8 4 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX4MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X4c4)      /* DATX8 4 Master Dealy Line Register */
#define ADDR_PHY_DX4GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X4c8)      /* DATX8 4 General Timing Register */
/* 0X134~0X13F RESERVED */

#define ADDR_PHY_DX5GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X500)      /* DATX8 5 General Configuration Register 0 */
#define ADDR_PHY_DX5GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X504)      /* DATX8 5 General Configuration Register 1 */
#define ADDR_PHY_DX5GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X508)      /* DATX8 5 General Configuration Register 2 */
#define ADDR_PHY_DX5GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X50c)      /* DATX8 5 General Configuration Register 3 */
#define ADDR_PHY_DX5GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X510)      /* DATX8 5 General Status Register 0 */
#define ADDR_PHY_DX5GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X514)      /* DATX8 5 General Status Register 1 */
#define ADDR_PHY_DX5GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X518)      /* DATX8 5 General Status Register 1 */
#define ADDR_PHY_DX5BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X51c)      /* DATX8 5 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX5BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X520)      /* DATX8 5 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX5BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X524)      /* DATX8 5 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX5BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X528)      /* DATX8 5 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX5BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X52c)      /* DATX8 5 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX5BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X530)      /* DATX8 5 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX5BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X534)      /* DATX8 5 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX5LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X538)      /* DATX8 5 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX5LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X53c)      /* DATX8 5 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX5LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X540)      /* DATX8 5 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX5MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X544)      /* DATX8 5 Master Dealy Line Register */
#define ADDR_PHY_DX5GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X548)      /* DATX8 5 General Timing Register */
/* 0X154~0X15F RESERVED */

#define ADDR_PHY_DX6GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X580)      /* DATX8 6 General Configuration Register 0 */
#define ADDR_PHY_DX6GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X584)      /* DATX8 6 General Configuration Register 1 */
#define ADDR_PHY_DX6GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X588)      /* DATX8 6 General Configuration Register 2 */
#define ADDR_PHY_DX6GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X58c)      /* DATX8 6 General Configuration Register 3 */
#define ADDR_PHY_DX6GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X590)      /* DATX8 6 General Status Register 0 */
#define ADDR_PHY_DX6GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X594)      /* DATX8 6 General Status Register 1 */
#define ADDR_PHY_DX6GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X598)      /* DATX8 6 General Status Register 1 */
#define ADDR_PHY_DX6BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X59c)      /* DATX8 6 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX6BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X5a0)      /* DATX8 6 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX6BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X5a4)      /* DATX8 6 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX6BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X5a8)      /* DATX8 6 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX6BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X5ac)      /* DATX8 6 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX6BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X5b0)      /* DATX8 6 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX6BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X5b4)      /* DATX8 6 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX6LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X5b8)      /* DATX8 6 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX6LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X5bc)      /* DATX8 6 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX6LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X5c0)      /* DATX8 6 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX6MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X5c4)      /* DATX8 6 Master Dealy Line Register */
#define ADDR_PHY_DX6GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X5c8)      /* DATX8 6 General Timing Register */
/* 0X174~0X17F RESERVED */

#define ADDR_PHY_DX7GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X600)      /* DATX8 7 General Configuration Register 0 */
#define ADDR_PHY_DX7GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X604)      /* DATX8 7 General Configuration Register 1 */
#define ADDR_PHY_DX7GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X608)      /* DATX8 7 General Configuration Register 2 */
#define ADDR_PHY_DX7GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X60c)      /* DATX8 7 General Configuration Register 3 */
#define ADDR_PHY_DX7GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X610)      /* DATX8 7 General Status Register 0 */
#define ADDR_PHY_DX7GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X614)      /* DATX8 7 General Status Register 1 */
#define ADDR_PHY_DX7GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X618)      /* DATX8 7 General Status Register 1 */
#define ADDR_PHY_DX7BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X61c)      /* DATX8 7 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX7BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X620)      /* DATX8 7 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX7BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X624)      /* DATX8 7 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX7BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X628)      /* DATX8 7 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX7BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X62c)      /* DATX8 7 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX7BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X630)      /* DATX8 7 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX7BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X634)      /* DATX8 7 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX7LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X638)      /* DATX8 7 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX7LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X63c)      /* DATX8 7 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX7LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X640)      /* DATX8 7 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX7MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X644)      /* DATX8 7 Master Dealy Line Register */
#define ADDR_PHY_DX7GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X648)      /* DATX8 7 General Timing Register */
/* 0X194~0X19F RESERVED */

#define ADDR_PHY_DX8GCR0                 (DWC_DDR_PUBL_BASE_ADDR+0X680)      /* DATX8 8 General Configuration Register 0 */
#define ADDR_PHY_DX8GCR1                 (DWC_DDR_PUBL_BASE_ADDR+0X684)      /* DATX8 8 General Configuration Register 1 */
#define ADDR_PHY_DX8GCR2                 (DWC_DDR_PUBL_BASE_ADDR+0X688)      /* DATX8 8 General Configuration Register 2 */
#define ADDR_PHY_DX8GCR3                 (DWC_DDR_PUBL_BASE_ADDR+0X68c)      /* DATX8 8 General Configuration Register 3 */
#define ADDR_PHY_DX8GSR0                 (DWC_DDR_PUBL_BASE_ADDR+0X690)      /* DATX8 8 General Status Register 0 */
#define ADDR_PHY_DX8GSR1                 (DWC_DDR_PUBL_BASE_ADDR+0X694)      /* DATX8 8 General Status Register 1 */
#define ADDR_PHY_DX8GSR2                 (DWC_DDR_PUBL_BASE_ADDR+0X698)      /* DATX8 8 General Status Register 1 */
#define ADDR_PHY_DX8BDLR0                (DWC_DDR_PUBL_BASE_ADDR+0X69c)      /* DATX8 8 Bit Dealy Line Register 0 */
#define ADDR_PHY_DX8BDLR1                (DWC_DDR_PUBL_BASE_ADDR+0X6a0)      /* DATX8 8 Bit Dealy Line Register 1 */
#define ADDR_PHY_DX8BDLR2                (DWC_DDR_PUBL_BASE_ADDR+0X6a4)      /* DATX8 8 Bit Dealy Line Register 2 */
#define ADDR_PHY_DX8BDLR3                (DWC_DDR_PUBL_BASE_ADDR+0X6a8)      /* DATX8 8 Bit Dealy Line Register 3 */
#define ADDR_PHY_DX8BDLR4                (DWC_DDR_PUBL_BASE_ADDR+0X6ac)      /* DATX8 8 Bit Dealy Line Register 4 */
#define ADDR_PHY_DX8BDLR5                (DWC_DDR_PUBL_BASE_ADDR+0X6b0)      /* DATX8 8 Bit Dealy Line Register 5 */
#define ADDR_PHY_DX8BDLR6                (DWC_DDR_PUBL_BASE_ADDR+0X6b4)      /* DATX8 8 Bit Dealy Line Register 6 */
#define ADDR_PHY_DX8LCDLR0               (DWC_DDR_PUBL_BASE_ADDR+0X6b8)      /* DATX8 8 Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DX8LCDLR1               (DWC_DDR_PUBL_BASE_ADDR+0X6bc)      /* DATX8 8 Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DX8LCDLR2               (DWC_DDR_PUBL_BASE_ADDR+0X6c0)      /* DATX8 8 Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DX8MDLR                 (DWC_DDR_PUBL_BASE_ADDR+0X6c4)      /* DATX8 8 Master Dealy Line Register */
#define ADDR_PHY_DX8GTR                  (DWC_DDR_PUBL_BASE_ADDR+0X6c8)      /* DATX8 8 General Timing Register */
/* 0X1B4~0X1BF RESERVED */

#define ADDR_PHY_DXnGCR0(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X280 + x*0x80)      /* DATX8 n General Configuration Register 0 */
#define ADDR_PHY_DXnGCR1(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X284 + x*0x80)      /* DATX8 n General Configuration Register 1 */
#define ADDR_PHY_DXnGCR2(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X288 + x*0x80)      /* DATX8 n General Configuration Register 2 */
#define ADDR_PHY_DXnGCR3(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X28c + x*0x80)      /* DATX8 n General Configuration Register 3 */
#define ADDR_PHY_DXnGSR0(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X290 + x*0x80)      /* DATX8 n General Status Register 0 */
#define ADDR_PHY_DXnGSR1(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X294 + x*0x80)      /* DATX8 n General Status Register 1 */
#define ADDR_PHY_DXnGSR2(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X298 + x*0x80)      /* DATX8 n General Status Register 1 */
#define ADDR_PHY_DXnBDLR0(x)                (DWC_DDR_PUBL_BASE_ADDR+0X29c + x*0x80)      /* DATX8 n Bit Dealy Line Register 0 */
#define ADDR_PHY_DXnBDLR1(x)                (DWC_DDR_PUBL_BASE_ADDR+0X2a0 + x*0x80)      /* DATX8 n Bit Dealy Line Register 1 */
#define ADDR_PHY_DXnBDLR2(x)                (DWC_DDR_PUBL_BASE_ADDR+0X2a4 + x*0x80)      /* DATX8 n Bit Dealy Line Register 2 */
#define ADDR_PHY_DXnBDLR3(x)                (DWC_DDR_PUBL_BASE_ADDR+0X2a8 + x*0x80)      /* DATX8 n Bit Dealy Line Register 3 */
#define ADDR_PHY_DXnBDLR4(x)                (DWC_DDR_PUBL_BASE_ADDR+0X2ac + x*0x80)      /* DATX8 n Bit Dealy Line Register 4 */
#define ADDR_PHY_DXnBDLR5(x)                (DWC_DDR_PUBL_BASE_ADDR+0X2b0 + x*0x80)      /* DATX8 n Bit Dealy Line Register 5 */
#define ADDR_PHY_DXnBDLR6(x)                (DWC_DDR_PUBL_BASE_ADDR+0X2b4 + x*0x80)      /* DATX8 n Bit Dealy Line Register 6 */
#define ADDR_PHY_DXnLCDLR0(x)               (DWC_DDR_PUBL_BASE_ADDR+0X2b8 + x*0x80)      /* DATX8 n Local Calibrated Delay Line Register 0 */
#define ADDR_PHY_DXnLCDLR1(x)               (DWC_DDR_PUBL_BASE_ADDR+0X2bc + x*0x80)      /* DATX8 n Local Calibrated Delay Line Register 1 */
#define ADDR_PHY_DXnLCDLR2(x)               (DWC_DDR_PUBL_BASE_ADDR+0X2c0 + x*0x80)      /* DATX8 n Local Calibrated Delay Line Register 2 */
#define ADDR_PHY_DXnMDLR(x)                 (DWC_DDR_PUBL_BASE_ADDR+0X2c4 + x*0x80)      /* DATX8 n Master Dealy Line Register */
#define ADDR_PHY_DXnGTR(x)                  (DWC_DDR_PUBL_BASE_ADDR+0X2c8 + x*0x80)      /* DATX8 n General Timing Register */
/* 0X174~0X17F RESERVED */



/* 0XF6~0XFF RESERVED */
#define ADDR_PHY_DXGCR(x)                (DWC_DDR_PUBL_BASE_ADDR + 0X1c0 + 0x40 * x)      /* DATX8 0 General Configuration Register */
#define ADDR_PHY_DXGSR0(x)               (DWC_DDR_PUBL_BASE_ADDR + 0X1c4 + 0x40 * x)      /* DATX8 0 General Status Register 0 */
#define ADDR_PHY_DXGSR1(x)               (DWC_DDR_PUBL_BASE_ADDR + 0X1c8 + 0x40 * x)      /* DATX8 0 General Status Register 1 */
#define ADDR_PHY_DXDLLCR(x)              (DWC_DDR_PUBL_BASE_ADDR + 0X1cc + 0x40 * x)      /* DATX8 0 DLL Control Register */
#define ADDR_PHY_DXDQTR(x)               (DWC_DDR_PUBL_BASE_ADDR + 0X1d0 + 0x40 * x)      /* DATX8 0 DQ Timing Register */
#define ADDR_PHY_DXDQSTR(x)              (DWC_DDR_PUBL_BASE_ADDR + 0X1d4 + 0x40 * x)      /* DATX8 0 DQS Timing Register */

/*ADDITIONAL*/
/*DRAM Controller System Management Register*/
#define	EMIF_SYSM_ADDR_CORE_RES			(EMIF_SYSM_ADDR + 0x0)
#define EMIF_SYSM_ADDR_BUS_AND_PHY_PLL_CLK  	(EMIF_SYSM_ADDR + 0x4)	
#define EMIF_SYSM_ADDR_APB_AND_PHY_CLK   	(EMIF_SYSM_ADDR + 0x10)	
#define	EMIF_SYSM_ADDR_BUS_RES			(EMIF_SYSM_ADDR + 0xc)
#define EMIF_SYSM_ADDR_MAP_REG		(EMIF_SYSM_ADDR + 0x38)
#endif
